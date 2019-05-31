/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "xmlparse.h"
/**
*   Parse resource files, strings.xml or module_config.xml
*   string / name files.
*
*/
static unordered_map < string, string > strings_map;

int parse_strings(char *filepath, unordered_map < string, string > &strings_map) {

    xmlDocPtr doc;
    xmlNodePtr curNode;
    char *name = NULL;
    char *value = NULL;

    if(filepath == NULL) {
        ALOGE("Invalid file name param");
        return -1;
    }

    ALOGI("start to load xml: %s", filepath);

    doc = xmlReadFile(filepath, "UTF-8", XML_PARSE_RECOVER);
    if(NULL == doc) {
        ALOGI("Document not parsed successfully.\n ");
        return -1;
    }

    curNode = xmlDocGetRootElement(doc);
    if(NULL == curNode) {
        ALOGI("empty document \n");
        xmlFreeDoc(doc);
        return -1;
    }


    if(xmlStrcmp(curNode->name, BAD_CAST "resources")) {
        ALOGI("document of the wrong type, root node != root \n");
        xmlFreeDoc(doc);
        return -1;
    }

    /*clear map before initialize */
    strings_map.clear();

    curNode = curNode->xmlChildrenNode;
    while(curNode != NULL) {
        if(!xmlStrcmp(curNode->name, BAD_CAST "string")) {
            name = (char *) xmlGetProp(curNode, BAD_CAST "name");
            value = (char *) xmlNodeGetContent(curNode);
            if(name != NULL && value != NULL) {
                strings_map[name] = value;
                xmlFree(value);
            }
        }
        curNode = curNode->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_strings(char *filepath) {
    return parse_strings(filepath, strings_map);
}

const char *get_value(const char *key) {
    if(key == NULL)
        return "";

    return get_value((string) key);
}
const char *get_value(string key) {
    return strings_map[key].c_str();
}
