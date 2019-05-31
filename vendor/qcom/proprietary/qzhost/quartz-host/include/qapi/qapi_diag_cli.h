#ifndef __QAPI_DIAG_CLI_H__
#define __QAPI_DIAG_CLI_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       DIAGNOSTICS - COMMAND LINE INTERFACE
 *
 *===============================================================================*/

/** @file qapi_diag_cli.h
 *
 * @addtogroup qapi_diag_cli
 * @{ 
 *
 * @brief Command Line Interface (CLI)
 *
 * @details The command line interface (CLI) implements an extensible mechanism for device-resident
 *          modules to interact with an external agent. The interaction consists of
 *          a stimulus sent by the external agent and action performed by the client,
 *          followed by a dispatch of the result of the action to the agent. The external agent
 *          is typically a software program residing on a host PC connected
 *          to the device via a serial interface like UART or USB. The Stimulus and
 *          Return results are strings. The CLI module implements functionality
 *          that is necessary to processes inbound command strings
 *          from the external agent and return response strings to the external agent.
 * 
 *          Via the CLI interface, clients can register a callback function corresponding
 *          to a command token string. If the command string from an external agent
 *          contains the registered command token and optional command payload,
 *          this module invokes a client callback passing the entire command string
 *          to the client module.
 * 
 *          The interface also provides a mechanism for client modules to return a
 *          response string, which is forwarded to the external agent. Clients 
 *          have the option of not returning a response string, in which case the CLI
 *          module will send an acknowledgement to the external agent.
 * 
 * @note    The interface allows only one outstanding command at any given time;
 *          i.e., until the command being processed does not return either a
 *          response or an acknowledgement, the next command will not be processed.
 *          
 *
 * @code {.c}
 *
 *    * The code snippet below demonstrates the use of the CLI interface. In this example,
 *    * a client registers for command token string "my_test" and returns
 *    * a response string.
 *    *
 *    * For brevity, the sequence assumes that all calls succeed. Clients should
 *    * check the return status from each call.
 *
 *    // Callback Definition
 *    qapi_Status_t client_callback(const char* cmd_str,
 *                                  size_t cmd_str_len,
 *                                  char* rsp_buf, 
 *                                  size_t rsp_buf_len,
 *                                  size_t* rsp_len)
 *
 *    {
 *      *rsp_len = strlen("success");
 *      if (*rsp_len < rsp_buf_len)
 *      {
 *        strlcpy(rsp_buf, "success", *rsp_len);
 *        return QAPI_OK;
 *      }
 *      else
 *      {
 *        return QAPI_ERROR;
 *      }
 *    } 
 * 
 *    // Registration Sequence 
 *    char my_test_cmd_token[] = "my_test";
 *    qapi_Diag_CLI_Cmd_Reg_t cmd_reg;
 *    cmd_reg.cmd_Tok = my_test_cmd_token;
 *    cmd_reg.cmd_Tok_Len = strlen(my_test_cmd_token);
 *    cmd_reg.callback = client_callback;
 *    qapi_Diag_CLI_Register_Command(&cmd_reg);
 *
 * @endcode
 *
 * @}
*/

/*===========================================================================
                              Edit History
 $Header: //components/rel/core.ioe/1.0/api/services/qapi_diag_cli.h#5 $

when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
12/03/15   leo    (TechComm) Edited/added Doxygen comments and markup.
10/29/15   mad    Created
===========================================================================*/

/** @addtogroup qapi_diag_cli
@{ */

/** 
 * Maximum command token length clients can use to register with this
 *        interface
 *
 * This is the maximum possible size of the cmd_tok member string of
 *          qapi_Diag_CLI_Cmd_Reg_t, and the maximum allowed value for
 * the cmd_tok_len member of qapi_Diag_CLI_Cmd_Reg_t. Note that this
 *          maximum does not include the optional payload.
 */
#define QAPI_DIAG_CLI_MAX_CMDTOKEN_LEN (16)

/** @} */ /* end_addtogroup qapi_diag_cli */

/** @addtogroup qapi_diag_cli
@{ */

/**
 * Callback type definition used by clients to receive notification
 * of an input command from an external agent.
 *
 * This function is called in the context of a thread implemented
 *       by the core module. This thread will block on this function. Clients
 *       are expected to keep callback functions slim, allowing it to return
 *       without minimum delay.
 *
 * @param[in]     cmd_Str      Entire NULL-terminated command 
 *                             string received from the tool (includes
 *                             command token and optional payload).
 * @param[in]     cmd_Str_Len  Length of cmd_Str.
 * @param[in,out] rsp_Buf      Buffer provided by the interface for the client 
 *                             to write its response to the command.
 * @param[in]     rsp_Buf_Len  Length of rsp_Buf. Clients should not
 *                             exceed this length, including the NULL
 *                             terminator, to avoid a buffer overflow.
 * @param [out]   rsp_Len      Length of the response that the client 
 *                             has written into the response buffer. The Client
 *                             can set this to zero if it does not
 *                             have a result to relay back. With this
 *                             field set to zero, depending on the return
 *                             value of the callback, the module will send a
 *                             canned response.
 * 
 * @return
 * QAPI_OK -- Command string was successfully processed by the client. \n
 * QAPI_ERR_BAD_PAYLOAD -- Command payload did not match the client's specification.
 */ 
typedef qapi_Status_t (*qapi_Diag_CLI_Cmd_Handler_CB_t)(const char* cmd_Str,
		                                        size_t cmd_Str_Len,
		                                        char* rsp_Buf,
							size_t rsp_Buf_Len,
							size_t *rsp_Len);

/** @} */ /* end_addtogroup qapi_diag_cli */

/** @addtogroup qapi_diag_cli
@{ */

/**
 * Structure used by clients to register with a CLI module.
 */
typedef struct
{
   char* cmd_Tok;                           /**< Command token to be registered. */
   size_t cmd_Tok_Len;                      /**< Command token length. */
   qapi_Diag_CLI_Cmd_Handler_CB_t callback; /**< Callback invoked against the command token. */
}qapi_Diag_CLI_Cmd_Reg_t;

/** @} */ /* end_addtogroup qapi_diag_cli */

/** @addtogroup qapi_diag_cli
@{ */

/*==================================================================================
  FUNCTION      qapi_Diag_CLI_Register_Command
==================================================================================*/
/**
 * Enables clients to register a command token along
 * with an associated handler to process the command with the module.
 *
 * This function uses malloc to allocate heap memory and a mutex to protect shared
 *       resources. It should only be called in Full Operation Mode (FOM) of the system. 
 *
 * @param[in] cmd_Reg Reference to a structure containing the command token and
 *                    associated handler.
 *
 * @return
 * QAPI_OK -- Registration was successful. \n
 * QAPI_ERR_INVALID_PARAM -- Token is larger than the maximum allowed length. \n
 * QAPI_ERR_NO_MEMORY -- Memory allocation failure.
 *
 */
qapi_Status_t qapi_Diag_CLI_Register_Command(qapi_Diag_CLI_Cmd_Reg_t* const cmd_Reg);

/** @} */ /* end_addtogroup qapi_diag_cli */

#endif /*__QAPI_DIAG_CLI_H__ */
