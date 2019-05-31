/**************************************************************
 * Copyright (C) 2017 Qualcomm Technologies, Inc.
 * 2016, Qualcomm Atheros, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/
#ifndef UTILS_COMMANDPARSER_H_
#define UTILS_COMMANDPARSER_H_

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// This is a simple command parser for easily checking that a command is valid
// and has all the required arguments.
// This however is not a generic "command line parser" in the sense that
// it doesn't handle options, i.e. it doesn't replace getopt/getopt_long.
// In particular, it doesn't handle unordered parameters and "optional
// parameters" is limited to "the last X arguments are optional".
//
// "Code" should be an enum of IDs for each possible commands.
//
// "ERROR" is the enum value returned for unknown commands or errors.
//
// "Map" lists all possible strings for the argument at a specific position.
//
// "List" tries to match each entry in turn until one does.
//
// "Args" matches n args, regardless of the values. The match will fail if
// there are not enough arguments.
//
// "End" indicates the end of the command. There shouldn't be any arguments
// left or the match fails.
//
// In case of success, match() will return the code for the command.
// In case of failure, match() will return the ERROR value and index will
// point to the argument that couldn't match (or args.size() if an argument is
// missing)
//
// Typical usage:
//		enum CommandCode {
//			CMD_ERROR = -1,
//			CMD_1,
//			CMD_2,
//			...
//		}
//
//		// typedef to avoid having to pass the template parameters all the time
//		typedef CommandParser<CommandCode, CMD_ERROR> Parser;
//
//		// accepted input ( [] = optional, | = alternative, * = any value )
//		//	cmd1
//		//	cmd2 * *
//		//	cmd3 [*]
//		//	cmd4 sub1|sub2
//		//	cmd5 [sub1|sub2]
//		Parser commandList = Parser::Map
//			("cmd1", Parser::End(CMD_1))  // command without arguments
//			("cmd2", Parser::Args(2)(Parser::End(CMD_2))) // command with 2 arguments
//				// the above can be simplified to:
//				//   ("cmd2", Parser::Args(2, CMD_2))
//			("cmd3", Parser::List // command with 1 optional argument
//				(Parser::End(CMD_3_NO_ARG))
//				(Parser::Args(1, CMD_3_WITH_ARG))
//			)
//			("cmd4", Parser::Map // command with sub commands
//				("sub1", Parser::End(CMD_4_SUB1))
//					// the above can be simplified to:
//					//   ("sub1", CMD_4_SUB1)
//				("sub2", CMD_4_SUB2)
//			)
//			("cmd5", Parser::List // command with optional sub commands
//				(Parser::End(CMD_5_NO_SUB))
//				(Parser::Map
//					("sub1", CMD_5_SUB1)
//					("sub2", CMD_5_SUB2)
//				)
//			)
//			;
//
//
//		std::vector<std::string> args;
//		CommandCode code = commandList.match(args);
//		switch (code) {
//			case CMD_ERROR:
//				// error handling
//				if (commandList.index == args.size()) {
//					// missing arguments
//				}
//				else {
//					// unexpected argument (too many or wrong value)
//					// first error at "args[commandList.index]"
//				}
//				break;
//			case CMD_1:
//		...

template<typename Code, Code ERROR>
struct CommandParser {
	template <typename T>
	CommandParser(const T &node): index(0), code(ERROR) {
		top = boost::make_shared<T>(node);
	}
	Code match(const std::vector<std::string> &args) {
		index = 0;
		code = top->match(args, index);
		return code;
	}

	struct Node {
		virtual ~Node() {}
		virtual Code match(const std::vector<std::string> &args, size_t &index) const = 0;
	};

	struct End: public Node {
		End(Code code): size(0), code(code) {}

		virtual Code match(const std::vector<std::string> &args, size_t &index) const {
			if (index != args.size()) {
				return ERROR;
			}
			return code;
		}

		size_t size;
		Code code;
	};

	struct Args: public Node {
		Args(size_t size): size(size) {}
		// Convenient constructor since Args is nearly always followed by End
		Args(size_t size, Code code): size(size), next(boost::make_shared<End>(code)) {}

		// return const so we can't "append" more nodes to it (there can be only
		// one node following Args), but we still need to return *this to build
		// the parent node
		template <typename T>
		const Args& operator()(const T &node) {
			next = boost::make_shared<T>(node);
			return *this;
		}

		virtual Code match(const std::vector<std::string> &args, size_t &index) const {
			if ((index + size) > args.size()) {
				index = args.size();
				return ERROR;
			}

			index += size;
			return next->match(args, index);
		}

		size_t size;
		boost::shared_ptr<Node> next;
	};

	struct List: public Node {
		List() {}
		template <typename T>
		List(const T &node) {
			(*this)(node);
		}

		template <typename T>
		List& operator()(const T &node) {
			list.push_back(boost::make_shared<T>(node));
			return *this;
		}

		virtual Code match(const std::vector<std::string> &args, size_t &index) const {
			// Save current index so we can restore it when an iteration fails
			size_t savedIndex = index;
			for (typename std::vector< boost::shared_ptr<Node> >::const_iterator i = list.begin(); i != list.end(); ++i) {
				index = savedIndex;
				Code code = (*i)->match(args, index);
				if (code != ERROR) {
					return code;
				}
			}
			return ERROR;
		}

		std::vector< boost::shared_ptr<Node> > list;
	};

	struct Map: public Node {
		Map() {}
		template <typename T>
		Map(const std::string &name, const T &node) {
			(*this)(name, node);
		}

		template <typename T>
		Map& operator()(const std::string &name, const T &node) {
			map.insert(std::make_pair(name, boost::make_shared<T>(node)));
			return *this;
		}
		Map& operator()(const std::string &name, Code code) {
			map.insert(std::make_pair(name, boost::make_shared<End>(code)));
			return *this;
		}

		virtual Code match(const std::vector<std::string> &args, size_t &index) const {
			if (index >= args.size()) {
				return ERROR;
			}

			typename std::map<std::string, boost::shared_ptr<Node> >::const_iterator i = map.find(args[index]);
			if (i == map.end()) {
				return ERROR;
			}

			++index;
			return i->second->match(args, index);
		}

		std::map<std::string, boost::shared_ptr<Node> > map;
	};

	boost::shared_ptr<Node> top;
	size_t index;
	Code code;
};

#endif /* UTILS_COMMANDPARSER_H_ */
