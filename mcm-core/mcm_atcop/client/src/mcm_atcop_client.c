/*===========================================================================

                         M C M _ A T C O P_ CLIENT.C

DESCRIPTION

  The MCM ATCOP Client Module.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
 09/03/13   vm     Created module
 ===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>

#include "stringl.h"
#include "mcm_atcop_v01.h"
#include "mcm_atcop_client.h"
#include "mcm_client_v01.h"
#include "mcm_client.h"


mcm_atcop_cb_t mcm_atcop_config;
uint8_t services_available = FALSE;
uint16_t service_list = MCM_ATCOP_V01;
int token_id =0 ;
mcm_client_require_resp_msg_v01      *mcm_client_require_resp_msg = NULL;
mcm_client_not_require_resp_msg_v01  *mcm_client_not_require_resp_msg = NULL;

#define SERVICE_REQUIRED             1
#define SERVICE_NOT_REQUIRED         0

int mcm_atcop_srv_mgr_service_handle
(
  int           handle,
  int           require_flag,
  uint16_t        service_list
);

void mcm_atcop_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_c_struct_len,
  void                  *token_id
);

/*===========================================================================
  FUNCTION sighandler
  ===========================================================================
  @brief
  Signal Handler
  @input
  signal- signal number
  @return
  void
  @dependencies
  Under lying os to generate the signal
  @sideefects
  None
  =========================================================================*/
void sighandler(int signal)
{
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  switch (signal)
  {
    case SIGTERM:
    case SIGINT:
      mcm_error = mcm_atcop_srv_mgr_service_handle(mcm_atcop_config.mcm_client_handle,
                                                   SERVICE_NOT_REQUIRED,
                                                   service_list);

      if (mcm_error != MCM_SUCCESS_V01)
      {
        LOG_MSG_ERROR("Failed to stop required services, Error : 0x%x\n",mcm_error,0,0);
        mcm_error = mcm_client_release(mcm_atcop_config.mcm_client_handle);

        if(mcm_error != MCM_SUCCESS_V01)
        {
          LOG_MSG_ERROR("\nAtcop deinit failed with error %d",mcm_error,0,0);
          printf("///////////////////////////////////////////////////////////\n");
          printf("%%%%%%%%%%%%%%%%%%%  Atcop deinit failed  %%%%%%%%%%%%%%\n");
          printf("///////////////////////////////////////////////////////////\n");
        }
        else
        {
          printf("///////////////////////////////////////////////////////////\n");
          printf("%%%%%%%%%%%  MCM ATCOP terminated gracefully  %%%%%%%%%%%%%%\n");
          printf("///////////////////////////////////////////////////////////\n");
        }

        exit(0);
      }
      else
      {
        LOG_MSG_INFO1("Service Removal Request Sent. \n",0,0,0);
      }
    break;

    default:
      printf("Received unexpected signal %s\n", signal);
    break;
  }
}


int main(int argc, char **argv)
{

  mcm_atcop_req_msg_v01 req_msg;
  mcm_atcop_resp_msg_v01 resp_msg;

  char scan_string[MCM_ATCOP_MAX_CMD_LEN + 1] = {0};
  uint8_t type;
  uint32_t error_num, opt= 0, num_entries, tmp_input;
  mcm_error_t_v01 mcm_error;
  int result;

  MCM_ATCOP_LOG_FUNC_ENTRY();


  /* Intialize. */
  mcm_atcop_config.mcm_client_handle = 0;

  /* Register the sighandlers, so the app may be shutdown with a
     kill command.*/
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);


  /* Initializing Diag for QXDM loga*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
    printf("Diag_LSM_Init failed !!");
  }
  mcm_error = mcm_client_init(&mcm_atcop_config.mcm_client_handle,
                              NULL, mcm_atcop_async_cb);

  if ( mcm_error != MCM_SUCCESS_V01 &&
       mcm_error != MCM_SUCCESS_CONDITIONAL_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Can not init client 0x%x",mcm_error,0,0);
    mcm_atcop_config.mcm_atcop_handle = 0;
    mcm_atcop_config.mcm_client_handle = 0;
    return 0;
  }
  else
  {
    LOG_MSG_INFO1("Client initialized successfully 0x%x",
                  mcm_atcop_config.mcm_client_handle,0,0);
  }

  mcm_error = mcm_atcop_srv_mgr_service_handle(mcm_atcop_config.mcm_client_handle,
                                               SERVICE_REQUIRED,
                                               service_list);

  if (mcm_error != MCM_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to Start required services, Error : 0x%x\n",mcm_error,0,0);
  }
  else
  {
    LOG_MSG_INFO1("Service Initialization Request Sent. Please wait until services start \n",0,0,0);
  }

  while (TRUE)
  {
    memset(scan_string, 0, sizeof(scan_string));
    printf("\nPlease Enter the ATCOP Command:\n\n");
    printf("Option > ");

    /* Read the option from the standard input. */
    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
      continue;

    if ( strlen(scan_string) < 1 )
    {
      LOG_MSG_ERROR("Invalid ATCOP command entered ", 0, 0, 0);
      continue;
    }

    /*Check for Services*/
    if (!services_available)
    {
      printf("\n//////////////////////////////////////////////////////////////////////////");
      printf("\n ========== Services Unavailable. Please Wait. ===========\n");
      printf("///////////////////////////////////////////////////////////////////////////\n");
      continue;
    }

    /* Set the last character which is LINE FEED to NULL */
    scan_string[strlen(scan_string) -1] = '\0';

    memset(&req_msg, 0, sizeof(mcm_atcop_req_msg_v01));
    memset(&resp_msg, 0, sizeof(mcm_atcop_resp_msg_v01));

    memcpy(&req_msg.cmd_req, &scan_string, strlen(scan_string));
    req_msg.cmd_len = strlen(scan_string);

    mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(
                   mcm_atcop_config.mcm_client_handle,
                   MCM_ATCOP_REQ_V01, &req_msg,
                   &resp_msg, MCM_ATCOP_MSG_TIMEOUT_VALUE_V01);

    if (( mcm_error != MCM_SUCCESS_V01 ) ||
        ( resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
       (( resp_msg.resp.error != MCM_SUCCESS_V01) &&
        ( resp_msg.resp.error != MCM_ERROR_INSUFFICIENT_RESOURCES_V01)))
    {
      if (resp_msg.resp.error == MCM_ERROR_GENERIC_V01)
      {
        LOG_MSG_ERROR("\n\nError in executing ATCOP command: "
                      "Command not supported  \n", 0, 0, 0);
      }
      else
      {
        LOG_MSG_ERROR("\n\nError in executing ATCOP command:"
                      " Invalid Command: Error: %d \n",
                       resp_msg.resp.error, 0,0);
      }
    }
    else
    {
      printf("\n\n ATCOP Command Execution successful.\n");

      if ( resp_msg.resp_len )
      {
        if( resp_msg.resp.error == MCM_ERROR_INSUFFICIENT_RESOURCES_V01)
        {
          printf("\n Since the length of response has crossed the maximum limit"
                 " (4K) output might be truncated: \n");
        }
        printf("\n Response is: \n");
        printf("\n----------------------------------------------------\n");
        printf(" %s ", resp_msg.cmd_resp);
      }
      else
        printf(" NULL response obtained ");
      printf("\n----------------------------------------------------\n");
    }
  }
}

/*==================================================================
  FUNCTION mcm_atcop_srv_mgr_service_handle

  DESCRIPTION
  Used to start and stop requires mcm services

PARAMETERS
  handle : client handle
  require_flag: flag indicating whether services are required or not required
  service_list : list of services

RETURN VALUE
  error value

DEPENDENCIES
  mcm_client_init should be called

SIDE EFFECTS
  None
====================================================================*/

int mcm_atcop_srv_mgr_service_handle
(
  int           handle,
  int           require_flag,
  uint16_t        service_list
)
{
  int ret_val = MCM_ERROR_GENERIC_V01;

  mcm_client_require_req_msg_v01       mcm_client_require_req_msg;
  mcm_client_not_require_req_msg_v01   mcm_client_not_require_req_msg;

  if (require_flag == SERVICE_REQUIRED)
  {
    mcm_client_require_resp_msg = (mcm_client_require_resp_msg_v01 *)malloc(
                                   sizeof(mcm_client_require_resp_msg_v01));

    if (mcm_client_require_resp_msg == NULL)
    {
      printf("\nCannot Allocate Momory to require resp msg/n");
      return ret_val;
    }

    LOG_MSG_INFO1("About to start required services \n",0,0,0);

    memset(&mcm_client_require_req_msg, 0, sizeof(mcm_client_require_req_msg_v01));
    memset(mcm_client_require_resp_msg, 0, sizeof(mcm_client_require_resp_msg_v01));

    mcm_client_require_req_msg.require_service = service_list;
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_CLIENT_REQUIRE_REQ_V01,
                                               &mcm_client_require_req_msg,
                                               mcm_client_require_resp_msg,
                                               (mcm_client_async_cb)mcm_atcop_async_cb,
                                               &token_id);

    LOG_MSG_INFO1("MCM_CLIENT_EXECUTE_COMMAND_ASYNC(enable): error %d "
                  "result %d error %d\n",
                  ret_val, mcm_client_require_resp_msg->response.result,
                  mcm_client_require_resp_msg->response.error);

    if (( ret_val!= MCM_SUCCESS_V01 ) ||
        ( mcm_client_require_resp_msg->response.result != MCM_RESULT_SUCCESS_V01 ) ||
        ( mcm_client_require_resp_msg->response.error != MCM_SUCCESS_V01))
    {
        LOG_MSG_ERROR("Can not bring up required services %d : %d\n",
                      ret_val, mcm_client_require_resp_msg->response.error,0);
        printf("//////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%  Require services failed  %%%%%%%%%%%%%%\n");
        printf("///////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Require Services request sent\n",0,0,0);
        printf("/////////////////////////////////////////////////////////\n");
        printf("%%%%  Sent request to bring up required services  %%%%%\n");
        printf("/////////////////////////////////////////////////////////\n");
      }

  }

  else if (require_flag == SERVICE_NOT_REQUIRED)
  {
    mcm_client_not_require_resp_msg = (mcm_client_not_require_resp_msg_v01 *)malloc(sizeof(mcm_client_not_require_resp_msg_v01));

    if (mcm_client_not_require_resp_msg == NULL)
    {
      printf("\nCannot Allocate Momory to not require resp msg/n");
      return ret_val;
    }

    LOG_MSG_INFO1("About to stop required services \n",0,0,0);

    memset(&mcm_client_not_require_req_msg, 0, sizeof(mcm_client_not_require_req_msg_v01));
    memset(mcm_client_not_require_resp_msg, 0, sizeof(mcm_client_not_require_resp_msg_v01));

    mcm_client_not_require_req_msg.not_require_service = service_list;
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_CLIENT_NOT_REQUIRE_REQ_V01,
                                               &mcm_client_not_require_req_msg,
                                               mcm_client_not_require_resp_msg,
                                               (mcm_client_async_cb)mcm_atcop_async_cb,
                                               &token_id);

    LOG_MSG_INFO1("MCM_CLIENT_EXECUTE_COMMAND_ASYNC(enable): error %d "
                  "result %d error %d\n",
                  ret_val, mcm_client_not_require_resp_msg->response.result,
                  mcm_client_not_require_resp_msg->response.error);

    if (( ret_val!= MCM_SUCCESS_V01 ) ||
        ( mcm_client_not_require_resp_msg->response.result != MCM_RESULT_SUCCESS_V01 ) ||
        ( mcm_client_not_require_resp_msg->response.error != MCM_SUCCESS_V01))
    {
      LOG_MSG_ERROR("Can not bring down required services %d : %d\n",
                    ret_val, mcm_client_not_require_resp_msg->response.error,0);
      printf("/////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%  Not Require services failed  %%%%%%%%\n");
      printf("/////////////////////////////////////////////////////////\n");
    }
    else
    {
      LOG_MSG_INFO1("Not Require Services request sent\n",0,0,0);
      printf("////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%  Sent request to bring down services  %%%%%%%\n");
      printf("////////////////////////////////////////////////////////\n");
    }

  }

  return ret_val;
}

void mcm_atcop_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_c_struct_len,
  void                  *token_id
)
{
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;

  LOG_MSG_INFO1("mcm_atcop_async_cb: user_handle %X msg_id %d ",
                 hndl, msg_id, 0);

  switch(msg_id)
  {
    case MCM_CLIENT_REQUIRE_REQ_V01:

      mcm_client_require_resp_msg = (mcm_client_require_resp_msg_v01 *)resp_c_struct;
      printf("\nGot Require Service Response\n");

      if((mcm_client_require_resp_msg->response.result == MCM_RESULT_SUCCESS_V01) &&
         (mcm_client_require_resp_msg->response.error == MCM_SUCCESS_V01))
      {
        services_available = TRUE;
        printf("\n//////////////////////////////////////////////////////////\n");
        printf("\n ==Services Available. You can proceed with other APIs==\n");
        printf("///////////////////////////////////////////////////////////\n\n");
      }
      else
      {
        printf("\n///////////////////////////////////////////////////////////\n");
        printf("\n ===== Services could not be brought up.Error = %d =========\n",
               mcm_client_require_resp_msg->response.error);
        printf("////////////////////////////////////////////////////////////\n\n");
      }
      free(mcm_client_require_resp_msg);
      mcm_client_require_resp_msg = NULL;
    break;

    case MCM_CLIENT_NOT_REQUIRE_REQ_V01:

      mcm_client_not_require_resp_msg = (mcm_client_not_require_resp_msg_v01 *)resp_c_struct;
      printf("\Got NOT Require Service response\n");

      if((mcm_client_not_require_resp_msg->response.result == MCM_RESULT_SUCCESS_V01) &&
         (mcm_client_not_require_resp_msg->response.error == MCM_SUCCESS_V01))
      {
        services_available = FALSE;
        printf("\n///////////////////////////////////////////////////////////\n");
        printf("\n Services Stopped. Please Enable services if you want to proceed \n");
        printf("////////////////////////////////////////////////////////////\n\n");
      }
      else
      {
        printf("\n///////////////////////////////////////////////////////////\n");
        printf("\n ===== Services could not be brought down.Error = %d ======\n",
               mcm_client_not_require_resp_msg->response.error);
        printf("///////////////////////////////////////////////////////////\n\n");
      }

      mcm_error = mcm_client_release(mcm_atcop_config.mcm_client_handle);

      if(mcm_error != MCM_SUCCESS_V01)
      {
        LOG_MSG_ERROR("\nAtcop deinit failed with error %d",mcm_error,0,0);
        printf("///////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%  Atcop deinit failed  %%%%%%%%%%%%%%\n");
        printf("///////////////////////////////////////////////////////////\n");
      }
      else
      {
        mcm_atcop_config.mcm_client_handle = 0;
        printf("///////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%  MCM ATCOP terminated gracefully  %%%%%%%%%%%%%%\n");
        printf("///////////////////////////////////////////////////////////\n");
      }
      free(mcm_client_not_require_resp_msg);
      mcm_client_not_require_resp_msg = NULL;
    exit(0);

    break;

  default:
    printf("\nUnhandled MSG : 0x%x\n", msg_id);
  break;

  }
}
