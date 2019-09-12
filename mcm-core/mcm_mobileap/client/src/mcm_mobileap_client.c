/*====================================================

FILE:  Mcm_mobileap_client.c

SERVICES:
QCMAP Client Implementation

=====================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE
 01/03/14   vm    Changes to support IoE on 9x25

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
#include "mcm_mobileap_v01.h"
#include "mcm_mobileap_cli.h"
#include "mcm_client_v01.h"
#include "mcm_client.h"

#define MCM_MOBILEAP_PORT_MAX_VALUE 65535
#define SERVICE_REQUIRED             1
#define SERVICE_NOT_REQUIRED         0

mcm_mobileap_cb_t mcm_mobileap_config;
int token_id =0 ;
uint8_t services_available = FALSE;
uint8_t deinit_called = FALSE;
uint8_t SIGTERM_received = FALSE;
uint16_t service_list = MCM_MOBILEAP_V01;
mcm_mobileap_enable_resp_msg_v01 *mcm_mobileap_enable_resp_msg = NULL;
mcm_mobileap_disable_resp_msg_v01 *mcm_mobileap_disable_resp_msg = NULL;
mcm_mobileap_bring_up_wwan_resp_msg_v01 *mcm_mobileap_bring_up_wwan_resp_msg = NULL;
mcm_mobileap_tear_down_wwan_resp_msg_v01 *mcm_mobileap_tear_down_wwan_resp_msg = NULL;
mcm_mobileap_set_nat_type_resp_msg_v01 *mcm_mobileap_set_nat_type_resp_msg = NULL;
mcm_mobileap_enable_wlan_resp_msg_v01 *mcm_mobileap_enable_wlan_resp_msg = NULL;
mcm_mobileap_disable_wlan_resp_msg_v01 *mcm_mobileap_disable_wlan_resp_msg = NULL;
mcm_mobileap_station_mode_enable_resp_msg_v01 *mcm_mobileap_station_mode_enable_resp_msg = NULL;
mcm_mobileap_station_mode_disable_resp_msg_v01 *mcm_mobileap_station_mode_disable_resp_msg = NULL;
mcm_client_require_resp_msg_v01      *mcm_client_require_resp_msg = NULL;
mcm_client_not_require_resp_msg_v01  *mcm_client_not_require_resp_msg = NULL;

void mcm_mobileap_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_len,
  void                  *token_id
 );

static void mcm_mobileap_ind_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *ind_c_struct,
  uint32_t                 ind_len
);


int16_t mcm_mobileap_check_port (uint32_t sport);

int16_t check_port (uint32_t sport);

int16_t check_proto (uint8_t sport);

int16_t check_tos (uint8_t tos);

int mcm_mobileap_srv_mgr_service_handle (int handle, int require_flag, uint16_t service_list);

int16_t mcm_mobileap_check_port (uint32_t sport)
{
  if((sport > MCM_MOBILEAP_PORT_MAX_VALUE) || (sport < 1) )
  {
    printf(" port value should be between 1 - %d \n",MCM_MOBILEAP_PORT_MAX_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION  check_port
===========================================================================
 @brief
   Port value is validated against the range 1 - MAX_PORT_VALUE
 @input
   sport - port value
 @return
   0  - success
   -1 - failure
 @dependencies
   None
 @sideefects
   None
=========================================================================*/
int16_t check_port (uint32_t sport)
{
  if((sport > MCM_MOBILEAP_PORT_MAX_VALUE) || (sport < 1) )
  {
    printf(" port value should be between 1 - %d\n",MCM_MOBILEAP_PORT_MAX_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION  check_proto
===========================================================================
 @brief
   protocol value is validated against the range 1 - MAX_PROTO_VALUE
 @input
   sport - protocol value
 @return
   0  - success
   -1 - failure
 @dependencies
   None
 @sideefects
   None
=========================================================================*/
int16_t check_proto (uint8_t sport)
{
  if( sport > MAX_PROTO_VALUE )
  {
    printf(" port value should be between 1 - %d\n",MAX_PROTO_VALUE);
    return -1;
  }
  else
    return 0;
}

/*===========================================================================
  FUNCTION  check_tos
===========================================================================
 @brief
   Tos value is validated against the range 1 - MAX_TOS_VALUE
 @input
   tos - port value
 @return
   0  - success
   -1 - failure
 @dependencies
   None
 @sideefects
   None
=========================================================================*/
int16_t check_tos (uint8_t tos)
{
  if( tos > MAX_TOS_VALUE )
  {
    printf(" Tos value should be between 0 - %d\n",MAX_TOS_VALUE);
    return -1;
  }
  else
    return 0;
}


/*==================================================================
FUNCTION mcm_mobileap_client_exit()
====================================================================
@brief
  Used to free memory and exit gracefully
===================================================================*/

void mcm_mobileap_client_exit()
{
  printf("/n Freeing Memory /n");

  if (mcm_mobileap_enable_resp_msg)
    free(mcm_mobileap_enable_resp_msg);

  if (mcm_mobileap_disable_resp_msg)
    free(mcm_mobileap_disable_resp_msg);

  if (mcm_mobileap_bring_up_wwan_resp_msg)
    free(mcm_mobileap_bring_up_wwan_resp_msg);

  if (mcm_mobileap_tear_down_wwan_resp_msg)
    free(mcm_mobileap_tear_down_wwan_resp_msg);

  if (mcm_mobileap_set_nat_type_resp_msg)
    free(mcm_mobileap_set_nat_type_resp_msg);

  if (mcm_mobileap_enable_wlan_resp_msg)
    free(mcm_mobileap_enable_wlan_resp_msg);

  if (mcm_mobileap_disable_wlan_resp_msg)
    free(mcm_mobileap_disable_wlan_resp_msg);

  if (mcm_mobileap_station_mode_enable_resp_msg)
    free(mcm_mobileap_station_mode_enable_resp_msg);

  if (mcm_mobileap_station_mode_disable_resp_msg)
    free(mcm_mobileap_station_mode_disable_resp_msg);

  if (mcm_client_require_resp_msg)
    free(mcm_client_require_resp_msg);

  if (mcm_client_not_require_resp_msg)
    free(mcm_client_not_require_resp_msg);

  printf("\n\nMOBILEAP CLIENT EXITING......!!!!!!!\n\n");

  exit(0);
}


/*==================================================================
  FUNCTION mcm_mobileap_srv_mgr_service_handle

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

int mcm_mobileap_srv_mgr_service_handle
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
                                               (mcm_client_async_cb)mcm_mobileap_async_cb,
                                               &token_id);
    if(NULL == mcm_client_require_resp_msg)
    {
      LOG_MSG_ERROR("mcm_client_require_resp_msg is NULL", 0, 0, 0);
      printf("//////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%  Require services failed  %%%%%%%%%%%%%%\n");
      printf("///////////////////////////////////////////////////////////\n");
      return ret_val;
    }
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
    mcm_client_not_require_resp_msg = (mcm_client_not_require_resp_msg_v01 *)malloc(
                                       sizeof(mcm_client_not_require_resp_msg_v01));

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
                                               (mcm_client_async_cb)mcm_mobileap_async_cb,
                                               &token_id);
    if(NULL == mcm_client_not_require_resp_msg)
    {
      LOG_MSG_ERROR("mcm_client_not_require_resp_msg is NULL", 0, 0, 0);
      printf("/////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%  Not Require services failed  %%%%%%%%\n");
      printf("/////////////////////////////////////////////////////////\n");
      return ret_val;
    }
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

void mcm_mobileap_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_c_struct_len,
  void                  *token_id
 )
{
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;

switch(msg_id)
  {
  case MCM_MOBILEAP_ENABLE_RESP_V01:
    mcm_mobileap_enable_resp_msg = (mcm_mobileap_enable_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Enable Response.\n");
    if (( mcm_mobileap_enable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01) ||
        ( mcm_mobileap_enable_resp_msg->mcm_mobileap_handle_valid != TRUE ))
    {
      LOG_MSG_ERROR("Can not enable qcmap : %d %d",
                     mcm_mobileap_enable_resp_msg->resp.error, mcm_mobileap_enable_resp_msg->mcm_mobileap_handle_valid, 0);
      printf("\nEnable MobileAP Failed\n");
    }
    else
    {
      mcm_mobileap_config.mcm_mobileap_handle = mcm_mobileap_enable_resp_msg->mcm_mobileap_handle;
      mcm_mobileap_config.mcm_mobileap_enable = TRUE;
      LOG_MSG_INFO1("MobileAp Enable Successful with handle :%d ",
                    mcm_mobileap_enable_resp_msg->mcm_mobileap_handle,0,0);
      printf("\nMobileAP Enabled Successfully with handle : %d \n",
             mcm_mobileap_enable_resp_msg->mcm_mobileap_handle);
    }
    free(mcm_mobileap_enable_resp_msg);
    mcm_mobileap_enable_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_DISABLE_RESP_V01:
    mcm_mobileap_disable_resp_msg = (mcm_mobileap_disable_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Disable Response.\n");
    if ( mcm_mobileap_disable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not disable qcmap : %d",
      mcm_mobileap_disable_resp_msg->resp.error,0, 0);
      printf("\nFailed to Disable MobileAP\n");
    }
    else
    {
      mcm_mobileap_config.mcm_mobileap_handle = 0;
      mcm_mobileap_config.mcm_mobileap_enable = FALSE;
      LOG_MSG_INFO1("MobileAP Disable Successful",0,0,0);
      printf("\nMobileAP Disabled Successfully \n");
    }
    free(mcm_mobileap_disable_resp_msg);
    mcm_mobileap_disable_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01:
    mcm_mobileap_bring_up_wwan_resp_msg = (mcm_mobileap_bring_up_wwan_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Bring Up WWAN Response\n");
    if(NULL == mcm_mobileap_bring_up_wwan_resp_msg)
    {
      LOG_MSG_ERROR("mcm_mobileap_bring_up_wwan_resp_msg is NULL", 0,0,0);
      printf("\nBring Up WWAN Failed\n");
      break;
    }
    if(mcm_mobileap_bring_up_wwan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not bring up wwan : %d",
                     mcm_mobileap_bring_up_wwan_resp_msg->resp.error,0,0);
      printf("\nBring Up WWAN Failed\n");
    }
    else
    {
      LOG_MSG_INFO1("Bring up WWAN Successful",0,0,0);
      printf("\nBring Up WWAN Successful \n");
    }
    if(mcm_mobileap_bring_up_wwan_resp_msg)
    {
      free(mcm_mobileap_bring_up_wwan_resp_msg);
      mcm_mobileap_bring_up_wwan_resp_msg = NULL;
    }
    break;

  case MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01:
    mcm_mobileap_tear_down_wwan_resp_msg = (mcm_mobileap_tear_down_wwan_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Tear Down WWAN Response\n");
    if(mcm_mobileap_tear_down_wwan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not tear down wwan : %d",
                     mcm_mobileap_tear_down_wwan_resp_msg->resp.error,0,0);
        printf("\nTear Down WWAN Failed \n");
    }
    else
    {
      LOG_MSG_INFO1("Tear Down WWAN Successful",0,0,0);
      printf("\nTear Down WWAN Successful \n");
    }
    free(mcm_mobileap_tear_down_wwan_resp_msg);
    mcm_mobileap_tear_down_wwan_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_SET_NAT_TYPE_RESP_V01:
    mcm_mobileap_set_nat_type_resp_msg = (mcm_mobileap_set_nat_type_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Set NAT Type Response\n");
    if(mcm_mobileap_set_nat_type_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not set nat type : %d",
                     mcm_mobileap_set_nat_type_resp_msg->resp.error,0,0);
      printf("\nSet NAT Type Failed \n");
    }
    else
    {
      LOG_MSG_INFO1("Set NAT Type Successful",0,0,0);
      printf("\nSet NAT Type Successful \n");
    }
    free(mcm_mobileap_set_nat_type_resp_msg);
    mcm_mobileap_set_nat_type_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_ENABLE_WLAN_RESP_V01:
    mcm_mobileap_enable_wlan_resp_msg = (mcm_mobileap_enable_wlan_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Enable WLAN Response\n");
    if(mcm_mobileap_enable_wlan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not enable WLAN : %d\n",
                     mcm_mobileap_enable_wlan_resp_msg->resp.error,0,0);
      printf("Enable WLAN Failed\n");
    }
    else
    {
      LOG_MSG_INFO1("Enable WLAN Successful\n",0,0,0);
      printf("Enable WLAN Successful \n");
    }
    free(mcm_mobileap_enable_wlan_resp_msg);
    mcm_mobileap_enable_wlan_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_DISABLE_WLAN_RESP_V01:
    mcm_mobileap_disable_wlan_resp_msg = (mcm_mobileap_disable_wlan_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Disable WLAN Response\n");
    if(mcm_mobileap_disable_wlan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not disable WLAN : %d\n",
                     mcm_mobileap_disable_wlan_resp_msg->resp.error,0,0);
      printf("Disable WLAN Failed\n");
    }
    else
    {
      LOG_MSG_INFO1("Disable WLAN Successful\n",0,0,0);
      printf("Disable WLAN Successful \n");
    }
    free(mcm_mobileap_disable_wlan_resp_msg);
    mcm_mobileap_disable_wlan_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_STATION_MODE_ENABLE_RESP_V01:
    mcm_mobileap_station_mode_enable_resp_msg = (mcm_mobileap_station_mode_enable_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Enable STATION Mode Response\n");
    if(mcm_mobileap_station_mode_enable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not enable STATION Mode : %d\n",
                     mcm_mobileap_station_mode_enable_resp_msg->resp.error,0,0);
      printf("Enable STATION Mode Failed\n");
    }
    else
    {
      LOG_MSG_INFO1("Enable STATION Mode Successful\n",0,0,0);
      printf("Enable STATION Mode Successful \n");
    }
    free(mcm_mobileap_station_mode_enable_resp_msg);
    mcm_mobileap_station_mode_enable_resp_msg = NULL;
    break;

  case MCM_MOBILEAP_STATION_MODE_DISABLE_RESP_V01:
    mcm_mobileap_station_mode_disable_resp_msg = (mcm_mobileap_station_mode_disable_resp_msg_v01 *)resp_c_struct;
    printf("\nGot the MobileAP Disable STATION Mode Response\n");
    if(mcm_mobileap_station_mode_disable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01)
    {
      LOG_MSG_ERROR("Can not disable STATION Mode : %d\n",
                     mcm_mobileap_station_mode_disable_resp_msg->resp.error,0,0);
      printf("Disable STATION Mode Failed\n");
    }
    else
    {
      LOG_MSG_INFO1("Disable STATION Mode Successful\n",0,0,0);
      printf("Disable STATION Mode Successful \n");
    }
    free(mcm_mobileap_station_mode_disable_resp_msg);
    mcm_mobileap_station_mode_disable_resp_msg = NULL;
    break;

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
      printf("\n == Services could not be brought up.Result = %d Error = %d ==\n",
             mcm_client_require_resp_msg->response.result,
             mcm_client_require_resp_msg->response.error);
      printf("////////////////////////////////////////////////////////////\n\n");
    }
    free(mcm_client_require_resp_msg);
    mcm_client_require_resp_msg = NULL;
    break;

  case MCM_CLIENT_NOT_REQUIRE_REQ_V01:

    mcm_client_not_require_resp_msg = (mcm_client_not_require_resp_msg_v01 *)resp_c_struct;
    printf("\nGot NOT Require Service response\n");

    if((mcm_client_not_require_resp_msg->response.result == MCM_RESULT_SUCCESS_V01) &&
       (mcm_client_not_require_resp_msg->response.error == MCM_SUCCESS_V01))
    {
      printf("\n///////////////////////////////////////////////////////////\n");
      printf("\n Services Stopped. Please Enable services if you want to proceed \n");
      printf("////////////////////////////////////////////////////////////\n\n");
    }
    else
    {
      printf("\n///////////////////////////////////////////////////////////\n");
      printf("\n == Services could not be brought down.Result = %d Error = %d ==\n",
             mcm_client_not_require_resp_msg->response.result,
             mcm_client_not_require_resp_msg->response.error);
      printf("///////////////////////////////////////////////////////////\n\n");
    }

    services_available = FALSE;

    mcm_error = mcm_client_release(mcm_mobileap_config.mcm_client_handle);

    if(mcm_error != MCM_SUCCESS_V01)
    {
      LOG_MSG_ERROR("\nMobileAP deinit failed with error %d",mcm_error,0,0);
      printf("///////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%%  MobileAP deinit failed  %%%%%%%%%%%%%%\n");
      printf("///////////////////////////////////////////////////////////\n");
    }
    else
    {
      mcm_mobileap_config.mcm_mobileap_enable = 0;
      mcm_mobileap_config.mcm_mobileap_handle = 0;
      mcm_mobileap_config.mcm_client_handle = 0;
      printf("///////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%  MobileAP deinit successful  %%%%%%%%%%%%%%%%\n");
      printf("///////////////////////////////////////////////////////////\n");
    }

    if (SIGTERM_received)
    {
      mcm_mobileap_client_exit();
    }

    deinit_called = FALSE;

    free(mcm_client_not_require_resp_msg);
    mcm_client_not_require_resp_msg = NULL;
    break;

  default:
    printf("\nUnhandled MSG : 0x%x\n", msg_id);
    break;
  }

  return;
}

static void mcm_mobileap_ind_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *ind_data,
  uint32_t                 ind_buf_len
)
{
  mcm_mobileap_unsol_event_ind_msg_v01 *ind_msg = (mcm_mobileap_unsol_event_ind_msg_v01 *)ind_data;

  LOG_MSG_INFO1("mcm_mobileap_ind_cb: user_handle %X msg_id %d ind_buf_len %d.",
                 hndl, msg_id, ind_buf_len);

  switch(ind_msg->event_id)
  {
    case MCM_MOBILEAP_ENABLED_EV_V01:
      printf("\nEnabled Event Received\n");
      break;

    case MCM_MOBILEAP_DISABLED_EV_V01:
      printf("\nDisabled Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_CONNECTING_EV_V01:
      printf("\nLan Connecting Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_CONNECTING_FAIL_EV_V01:
      printf("\nLan Connecting Fail Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_IPv6_CONNECTING_FAIL_EV_V01:
      printf("\nLan IPv6 Connecting Fail Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_CONNECTED_EV_V01:
      printf("\nLan Connected Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_IPv6_CONNECTED_EV_V01:
      printf("\nLan IPv6 Connected Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_DISCONNECTED_EV_V01:
      printf("\nLan Disconnected Event Received\n");
      break;

    case MCM_MOBILEAP_LAN_IPv6_DISCONNECTED_EV_V01:
      printf("\nLan IPv6 Disconnected Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_CONNECTING_EV_V01:
      printf("\nWan Connecting Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_CONNECTING_FAIL_EV_V01:
      printf("\nWan Connecting Fail Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EV_V01:
      printf("\nWan IPv6 Connecting Fail Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_CONNECTED_EV_V01:
      printf("\nWan Connected Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_IPv6_CONNECTED_EV_V01:
      printf("\nWan IPv6 Connected Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_DISCONNECTED_EV_V01:
      printf("\nWan Disconnected Event Received\n");
      break;

    case MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EV_V01:
      printf("\nWan IPv6 Disconnected Event Received\n");
      break;

    case MCM_MOBILEAP_STA_CONNECTED_EV_V01:
      printf("\nStation Connected Event Received\n");
      break;

    default:
      printf("\nUnknown Event: 0x%x\n", ind_msg->event_id);
      break;
  }

}

void sighandler(int signal)
{
  mcm_error_t_v01 mcm_error;
  printf("\n Enterring Signal Handler \n");

  switch (signal)
  {
    case SIGTERM:
    case SIGINT:

      if (deinit_called)
      {
        printf("\n\n CLIENT ALREADY IN PROCESS OF DEINIT\n\n");
        SIGTERM_received = TRUE;
        break;
      }

      else if (services_available && (mcm_mobileap_config.mcm_client_handle != 0))
      {
        mcm_error = mcm_mobileap_srv_mgr_service_handle(mcm_mobileap_config.mcm_client_handle,
                                                        SERVICE_NOT_REQUIRED,
                                                        service_list);

        if (mcm_error != MCM_SUCCESS_V01)
        {
          LOG_MSG_ERROR("Failed to Remove required services, Error : 0x%x\n",mcm_error,0,0);
          mcm_error = mcm_client_release(mcm_mobileap_config.mcm_client_handle);

          if(mcm_error != MCM_SUCCESS_V01)
          {
            LOG_MSG_ERROR("\nMobileAP deinit failed with error %d",mcm_error,0,0);
            printf("////////////////////////////////////////////////////\n");
            printf("%%%%%%%%%%%%% MobileAP deinit failed  %%%%%%%%%%%%%%\n");
            printf("////////////////////////////////////////////////////\n");
          }
          else
          {
            mcm_mobileap_config.mcm_mobileap_enable = 0;
            mcm_mobileap_config.mcm_mobileap_handle = 0;
            mcm_mobileap_config.mcm_client_handle = 0;
            printf("////////////////////////////////////////////////////\n");
            printf("%%%%%%%%%%% MobileAP deinit successful  %%%%%%%%%%%%\n");
            printf("////////////////////////////////////////////////////\n");
          }

          mcm_mobileap_client_exit();

        }
        else
        {
          LOG_MSG_INFO1("Service Removal Request Sent from sighandler.\n",0,0,0);
          SIGTERM_received = TRUE;
        }

      }

      else
      {
        mcm_mobileap_client_exit();
      }

    break;

    default:
      printf("Received unexpected signal %d\n", signal);
    break;
  }
}

int main(int argc, char **argv)
{

  char scan_string[MCM_MOBILEAP_MAX_FILE_PATH_LEN] = {0};
  uint8_t type;
  uint32_t error_num, opt= 0, num_entries, tmp_input;

  /* Register the sighandlers, so the app may be shutdown with a
     kill command.*/
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

  LOG_MSG_INFO1("Entered Main function:", 0, 0,0);
  while (TRUE)
  {

    /* Display menu of options. */
    printf("Please select an option to test from the items listed below.\n\n");
    printf(" 1. Mobile AP Init                   2. Mobile AP Deinit  \n");
    printf(" 3. Mobile AP Enable                 4. MobileAP Disable\n");
    printf(" 5. Connect Backhaul                 6. Disconnect Backhaul \n");
    printf(" 7. Add Static NAT Entry             8. Get Static NAT Entry \n");
    printf(" 9. Delete Static NAT Entry          10. Set NAT Timeout \n");
    printf(" 11. Get Nat Timeout                 12. Set NAT Type\n");
    printf(" 13. Get NAT Type                    14. Add Firewall Entry \n");
    printf(" 15. Delete Firewall Entry           16. Get Firewall Entry\n");
    printf(" 17. Set Firewall                    18. Add DMZ\n");
    printf(" 19. Delete DMZ                      20. Get DMZ\n");
    printf(" 21. Get IPv4 WWAN Configuration     22. Get WWAN Statistics \n");
    printf(" 23. Reset WWAN Statistics           24. Set DHCPD Config\n");
    printf(" 25. Enable WLAN                     26. Disable WLAN\n");
    printf(" 27. Set IPSec VPN Passthrough       28. Set PPTP VPN Passthrough\n");
    printf(" 29. Set L2TP VPN Passthrough        30. Get IPSec VPN Passthrough\n");
    printf(" 31. Get PPTP VPN Passthrough        32. Get L2TP VPN Passthrough\n");
    printf(" 33. Set Autoconnect                 34. Set Roaming\n");
    printf(" 35. Get Autoconnect                 36. Get Roaming\n");
    printf(" 37. Set DUAL AP Config              38. Enable STA Mode\n");
    printf(" 39. Disable STA Mode                40. Event Register\n");
    printf(" 41. Exit \n\n");
    printf("Option > ");

    /* Read the option from the standard input. */
    if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
      continue;

    /* Convert the option to an integer, and switch on the option entered. */
    opt = atoi(scan_string);
    printf(" Option Entered is : %d \n", opt);

    if ((opt > 2 && opt<41) && (!services_available) && (!deinit_called))
    {
      printf("\n//////////////////////////////////////////////////////////");
      printf("\n ====Sorry, you can't proceed. Services Unavailable=====\n");
      printf("//////////////////////////////////////////////////////////\n");
      continue;
    }

    if ((opt == 2) && (deinit_called))
    {
      printf("\n////////////////////////////////////////////////////////");
      printf("\n ===Client De-init already in process. Please wait====\n");
      printf("////////////////////////////////////////////////////////\n");
      continue;
    }

    switch (opt)
    {
      /* Display the current configuration of the modem. */
    case 1:
    {
      mcm_error_t_v01 mcm_error;

      printf(" Option Entered case : %d \n", opt);
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);


      printf(" Entered @ : %s 1 \n", __FUNCTION__);
      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      if (mcm_mobileap_config.mcm_client_handle == 0)
      {
        /* Intialize. */
        mcm_mobileap_config.mcm_mobileap_handle = 0;
        mcm_mobileap_config.mcm_client_handle = 0;

        /* Initializing Diag for QXDM loga*/
        if (TRUE != Diag_LSM_Init(NULL))
        {
          printf("Diag_LSM_Init failed !!");
        }

        mcm_error = mcm_client_init(&mcm_mobileap_config.mcm_client_handle,
                                    mcm_mobileap_ind_cb,
                                    mcm_mobileap_async_cb);

        if ( mcm_error != MCM_SUCCESS_V01 && mcm_error != MCM_SUCCESS_CONDITIONAL_SUCCESS_V01 )
        {
          LOG_MSG_ERROR("Can not init client 0x%x",mcm_error,0,0);
          mcm_mobileap_config.mcm_mobileap_enable = 0;
          mcm_mobileap_config.mcm_mobileap_handle = 0;
          mcm_mobileap_config.mcm_client_handle = 0;
        }
        else
        {
          LOG_MSG_INFO1("Client initialized successfully 0x%x\n",
                        mcm_mobileap_config.mcm_client_handle,0,0);
        }
      }

      if (mcm_mobileap_config.mcm_client_handle != 0)
      {
        if(!services_available)
        {
          mcm_error = mcm_mobileap_srv_mgr_service_handle(mcm_mobileap_config.mcm_client_handle,
                                                          SERVICE_REQUIRED,
                                                          service_list);

          if (mcm_error != MCM_SUCCESS_V01)
          {
            LOG_MSG_ERROR("Failed to Initialize required services, Error : 0x%x\n",mcm_error,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Service Initialization Request Sent. Please wait until services start \n",0,0,0);
          }
        }

        else
        {
          LOG_MSG_INFO1("Client already initialized , handle = %d\n",
                        mcm_mobileap_config.mcm_client_handle,0,0);
          LOG_MSG_INFO1("SERVICES ARE AVAILABLE....!!!!\n",0,0,0);
        }
      }

    }
    break;

/*=======================================================================================*/

    case 2:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_error_t_v01 mcm_error;
      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      if (mcm_mobileap_config.mcm_client_handle == 0)
      {
        LOG_MSG_INFO1("Client already de-initialized successfully 0x%x",
                      0,0,0);
        break;
      }

      deinit_called = TRUE;

      mcm_error = mcm_mobileap_srv_mgr_service_handle(mcm_mobileap_config.mcm_client_handle,
                                                      SERVICE_NOT_REQUIRED,
                                                      service_list);

      if (mcm_error != MCM_SUCCESS_V01)
      {
        LOG_MSG_ERROR("Failed to Remove required services, Error : 0x%x\n",mcm_error,0,0);
        services_available = FALSE;
        mcm_error = mcm_client_release(mcm_mobileap_config.mcm_client_handle);

        if(mcm_error != MCM_SUCCESS_V01)
        {
          LOG_MSG_ERROR("\nMobileAP deinit failed with error %d",mcm_error,0,0);
          printf("////////////////////////////////////////////////////\n");
          printf("%%%%%%%%%  MobileAP deinit failed  %%%%%%%%%%%%%%%%%\n");
          printf("////////////////////////////////////////////////////\n");
        }
        else
        {
          mcm_mobileap_config.mcm_mobileap_enable = 0;
          mcm_mobileap_config.mcm_mobileap_handle = 0;
          mcm_mobileap_config.mcm_client_handle = 0;
          printf("///////////////////////////////////////////////////\n");
          printf("%%%%%%%%%%%  MobileAP deinit successful %%%%%%%%%%%\n");
          printf("////////////////////////////////////////////////////\n");
        }
        deinit_called = FALSE;
      }
      else
      {
        LOG_MSG_INFO1("Service Removal Request Sent.\n",0,0,0);
      }
    }
      break;

    case 3:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;

      mcm_mobileap_enable_resp_msg = (mcm_mobileap_enable_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_enable_resp_msg_v01));

      if ( mcm_mobileap_enable_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(mcm_mobileap_enable_resp_msg, 0, sizeof(mcm_mobileap_enable_resp_msg_v01));

      mcm_error = mcm_client_execute_command_async(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_ENABLE_REQ_V01,
                                            NULL,
                                            0,
                                            mcm_mobileap_enable_resp_msg,
                                            sizeof(mcm_mobileap_enable_resp_msg_v01),
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_enable_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_enable_resp_msg is NULL",0,0,0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      LOG_MSG_INFO1("MCM_CLIENT_EXECUTE_COMMAND_ASYNC(enable): error %d "
                    "result %d valid %d\n",
                    mcm_error, mcm_mobileap_enable_resp_msg->resp.result,
                    mcm_mobileap_enable_resp_msg->mcm_mobileap_handle_valid);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
          ( mcm_mobileap_enable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
          ( mcm_mobileap_enable_resp_msg->resp.error != MCM_SUCCESS_V01))
      {
        LOG_MSG_ERROR("Can not enable qcmap %d : %d\n",
                      mcm_error, mcm_mobileap_enable_resp_msg->resp.error,0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("MobileAP Enable request sent\n",0,0,0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
      break;

    case 4:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_disable_req_msg_v01 mcm_mobileap_disable_req_msg;
      mcm_error_t_v01 mcm_error = 0;
      uint32_t resp_cb_data = 2;

      mcm_mobileap_disable_resp_msg = (mcm_mobileap_disable_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_disable_resp_msg_v01));

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      if (!mcm_mobileap_config.mcm_mobileap_enable)
      {
       /* QCMAP is not enabled */
        LOG_MSG_INFO1("MCM_MOBILEAP not enabled\n",0,0,0);
        break;
      }

      memset(&mcm_mobileap_disable_req_msg, 0, sizeof(mcm_mobileap_disable_req_msg_v01));
      memset(mcm_mobileap_disable_resp_msg, 0, sizeof(mcm_mobileap_disable_resp_msg_v01));
      mcm_mobileap_disable_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_DISABLE_REQ_V01,
                                            &mcm_mobileap_disable_req_msg,
                                            mcm_mobileap_disable_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_disable_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_disable_resp_msg is NULL", 0, 0, 0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if ( ( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_disable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01) ||
           (mcm_mobileap_disable_resp_msg->resp.error != MCM_SUCCESS_V01))
      {
        LOG_MSG_ERROR( "Can not disable qcmap %d : %d",
                        mcm_error, mcm_mobileap_disable_resp_msg->resp.error,0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("MobileAP disable request sent.", 0, 0, 0);
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("/////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
      break;

    case 5:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_bring_up_wwan_req_msg_v01 mcm_mobileap_bring_up_wwan_req_msg;
      mcm_error_t_v01 mcm_error;

#ifndef TARGET_IS_9615
      mcm_mobileap_ip_version_t_v01 ip_version;
      printf("\n Please input Call Type (4-IPV4; 6-IPV6) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      ip_version = (mcm_mobileap_ip_version_t_v01)atoi(scan_string);

      if (ip_version != MCM_MOBILEAP_IP_V4_V01 &&
          ip_version != MCM_MOBILEAP_IP_V6_V01)
      {
        printf("\nInvalid Call Type : %d", ip_version );
        break;
      }
#endif /* !(TARGET_IS_9615) */

      mcm_mobileap_bring_up_wwan_resp_msg =(mcm_mobileap_bring_up_wwan_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
      if(NULL == mcm_mobileap_bring_up_wwan_resp_msg)
      {
        printf("\n Could not allocate memory \n");
        break;
      }
      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&mcm_mobileap_bring_up_wwan_req_msg, 0, sizeof(mcm_mobileap_bring_up_wwan_req_msg_v01));
      memset(mcm_mobileap_bring_up_wwan_resp_msg, 0, sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01));
    /* Bring up the data call. */
      LOG_MSG_INFO1("Bring up wwan",0,0,0);
      mcm_mobileap_bring_up_wwan_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

#ifndef TARGET_IS_9615
      mcm_mobileap_bring_up_wwan_req_msg.ip_version = ip_version;
      mcm_mobileap_bring_up_wwan_req_msg.ip_version_valid = TRUE;
#endif /* !(TARGET_IS_9615) */

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_BRING_UP_WWAN_REQ_V01,
                                            &mcm_mobileap_bring_up_wwan_req_msg,
                                            mcm_mobileap_bring_up_wwan_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_bring_up_wwan_resp_msg)
      {
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP connect backhaul failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }
      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_bring_up_wwan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01) ||
           ( mcm_mobileap_bring_up_wwan_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not bring up wwan mcm_mobileap %d : %d",
                      mcm_error, mcm_mobileap_bring_up_wwan_resp_msg->resp.error,0);
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
         printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP connect backhaul failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Bringing up wwan...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP connect backhaul successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;
/********************************************************************/

    case 6:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_tear_down_wwan_req_msg_v01 mcm_mobileap_tear_down_wwan_req_msg;
      mcm_error_t_v01 mcm_error;

#ifndef TARGET_IS_9615
      mcm_mobileap_ip_version_t_v01 ip_version;
      printf("\n Please input Call Type (4-IPV4; 6-IPV6) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      ip_version = (mcm_mobileap_ip_version_t_v01)atoi(scan_string);

      if (ip_version != MCM_MOBILEAP_IP_V4_V01 &&
          ip_version != MCM_MOBILEAP_IP_V6_V01)
      {
        printf("\nInvalid Call Type : %d", ip_version );
        break;
      }
#endif /* !(TARGET_IS_9615) */

      mcm_mobileap_tear_down_wwan_resp_msg = (mcm_mobileap_tear_down_wwan_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      LOG_MSG_INFO1("Bringing down wwan",0,0,0);

      memset(&mcm_mobileap_tear_down_wwan_req_msg, 0, sizeof(mcm_mobileap_tear_down_wwan_req_msg_v01));
      memset(mcm_mobileap_tear_down_wwan_resp_msg, 0, sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01));

      mcm_mobileap_tear_down_wwan_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
#ifndef TARGET_IS_9615
      mcm_mobileap_tear_down_wwan_req_msg.ip_version = ip_version;
      mcm_mobileap_tear_down_wwan_req_msg.ip_version_valid = TRUE;
#endif /* !(TARGET_IS_9615) */

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_TEAR_DOWN_WWAN_REQ_V01,
                                            &mcm_mobileap_tear_down_wwan_req_msg,
                                            mcm_mobileap_tear_down_wwan_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_tear_down_wwan_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_tear_down_wwan_resp_msg is NULL", 0, 0, 0);
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
         printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disconnect backhaul failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }
      if (( mcm_error != MCM_SUCCESS_V01) ||
           ( mcm_mobileap_tear_down_wwan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01) ||
           ( mcm_mobileap_tear_down_wwan_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not tear down wwan qcmap %d : %d",
                      mcm_error,
                      mcm_mobileap_tear_down_wwan_resp_msg->resp.error,0);
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
         printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disconnect backhaul failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
         printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Tearing down wwan...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disconnect backhaul successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;
    /********************************************************************/

    case 7:
    {
      struct in_addr tmp1, tmp2, addr;
      memset(scan_string, 0, MCM_MOBILEAP_MAX_FILE_PATH_LEN);
      mcm_mobileap_add_static_nat_entry_req_msg_v01 add_static_nat_req_msg;
      mcm_mobileap_add_static_nat_entry_resp_msg_v01 add_static_nat_resp_msg;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      inet_aton("0.0.0.0",&tmp1);
      inet_aton("255.255.255.255",&tmp2);

      while(TRUE)
      {
        printf("   Please input port_fwding_protocol   : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if ( check_proto(tmp_input) == 0 )
          break;
      }
      add_static_nat_req_msg.nat_entry_config.port_fwding_protocol = (uint8_t)tmp_input;
      while (TRUE)
      {
        printf("   Please input port_fwding_private_ip(xxx.xxx.xxx.xxx)   : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !( inet_aton(scan_string, &addr) <= 0) &&
             ( addr.s_addr != tmp1.s_addr ) &&
             ( addr.s_addr != tmp2.s_addr ))
            break;
        }
        printf("   Invalid IPv4 address %s",scan_string);
      }

      add_static_nat_req_msg.nat_entry_config.port_fwding_private_ip = ntohl(addr.s_addr);
      while (TRUE)
      {
        printf("   Please input port_fwding_private_port : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        /*No port validation check for ICMP*/
        if(add_static_nat_req_msg.nat_entry_config.port_fwding_protocol == 1)
          break;
        if(check_port (tmp_input) == 0 )
          break;
      }
      add_static_nat_req_msg.nat_entry_config.port_fwding_private_port = (uint16_t)tmp_input;

      while (TRUE)
      {
        printf("   Please input port_fwding_global_port  : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        /*No port validation check for ICMP*/
        if(add_static_nat_req_msg.nat_entry_config.port_fwding_protocol == 1)
          break;
        if(check_port (tmp_input) == 0 )
          break;
      }
      add_static_nat_req_msg.nat_entry_config.port_fwding_global_port = (uint16_t)tmp_input;

      mcm_error_t_v01 mcm_error;
      uint32_t mcm_err_num;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();


      add_static_nat_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;


      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_ADD_STATIC_NAT_ENTRY_REQ_V01,
                                           &add_static_nat_req_msg,
                                           &add_static_nat_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);
      mcm_err_num = add_static_nat_resp_msg.resp.error;

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( add_static_nat_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01) ||
           ( add_static_nat_resp_msg.resp.error != MCM_SUCCESS_V01))
      {
#ifdef TARGET_IS_9615
        LOG_MSG_ERROR("Can not add static_nat entry %d : %d\n",
                      mcm_error, add_static_nat_resp_msg.resp.error, 0);
#else /* TARGET_IS_9615 */
        if (add_static_nat_resp_msg.resp.error ==
            MCM_ERROR_INTERFACE_NOT_FOUND_V01)
          printf("\nBackhaul down, SNAT Entry added to xml file\n\n");
        else
          printf("\nSNAT Entry add failed, Error: 0x%x\n\n",
                 add_static_nat_resp_msg.resp.error);

#endif /* !(TARGET_IS_9615) */
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add snat entry failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Added static_nat Entry...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add snat entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
      break;

    /********************************************************************/
    case 8:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      struct in_addr addr;
      int i=0;
      char *port_fwding_private_ip;
      while (TRUE)
      {
        printf("   Please Enter the number of entries to retrieve: 1- 50: \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if(tmp_input > 0 && tmp_input <= 50 )
          break;
        printf("   Please Enter a valid number : 1- 50: \n");
      }
      mcm_mobileap_get_static_nat_entry_req_msg_v01 get_static_nat_req_msg;
      mcm_mobileap_get_static_nat_entry_resp_msg_v01 get_static_nat_resp_msg;
      mcm_error_t_v01 mcm_error;
      mcm_error_t_v01 mcm_err_num;
      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_static_nat_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      get_static_nat_req_msg.max_entries = tmp_input;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_STATIC_NAT_ENTRY_REQ_V01,
                                           &get_static_nat_req_msg,
                                           &get_static_nat_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_static_nat_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_static_nat_resp_msg.resp.error != MCM_SUCCESS_V01))
      {
        LOG_MSG_ERROR("Can not get snat entries %d : %d",
                       mcm_error, get_static_nat_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    MobileAP get snat entry failed    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        if (get_static_nat_resp_msg.snat_entries_valid == TRUE)
        {
          LOG_MSG_INFO1("\nNum STATIC NAT entries confged: %d",
                        get_static_nat_resp_msg.snat_entries_len, 0, 0);
          if (get_static_nat_resp_msg.snat_entries_len <=
              MCM_MOBILEAP_MAX_STATIC_NAT_ENTRIES_V01)
          {
            /*Doesn't have any impact for 9x15. For 9x25 and above
              get_static_nat_resp_msg.snat_entries_len returns all the entries configured.
              Hence this check would print only the required number of entries.*/
            if(tmp_input > get_static_nat_resp_msg.snat_entries_len)
            {
              tmp_input = get_static_nat_resp_msg.snat_entries_len;
            }
            while (i < tmp_input)
            {
              addr.s_addr = ntohl(get_static_nat_resp_msg.snat_entries[i].port_fwding_private_ip);
              port_fwding_private_ip= inet_ntoa(addr);
              printf("Printing Snat Entry [%d]\n", i+1);
              printf("Port Forwading Private IP = %s\n",port_fwding_private_ip);
              printf(" Port Forwarding Global Port = %d\n", get_static_nat_resp_msg.snat_entries[i].port_fwding_global_port);
              printf(" Port Forwarding Private Port = %d\n", get_static_nat_resp_msg.snat_entries[i].port_fwding_private_port);
              printf(" Port Forwarding Protocol = %d\n\n", get_static_nat_resp_msg.snat_entries[i].port_fwding_protocol);
              i = i+1;
            }
          }
          else
          {
            LOG_MSG_INFO1("\nNum STATIC NAT entries confged > "
                          "MCM_MOBILEAP_MAX_STATIC_NAT_ENTRIES", 0, 0, 0);
          }
        }
        else
        {
          LOG_MSG_INFO1("\nNo STATIC NAT entries configured",0,0,0);
          printf("\nNo STATIC NAT entries configured\n");
        }
        LOG_MSG_INFO1("Get STATIC NAT Entries Succeeded...\n",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get snat entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 9:
    {
      struct in_addr tmp1, tmp2, addr;
      mcm_mobileap_delete_static_nat_entry_req_msg_v01 delete_static_nat_req_msg;
      mcm_mobileap_delete_static_nat_entry_resp_msg_v01 delete_static_nat_resp_msg;
      mcm_error_t_v01 mcm_error;

      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      memset(scan_string, 0, MCM_MOBILEAP_MAX_FILE_PATH_LEN);
      inet_aton("0.0.0.0",&tmp1);
      inet_aton("255.255.255.255",&tmp2);

      while(TRUE)
      {
        printf("   Please input port_fwding_protocol   : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if ( check_proto(tmp_input) == 0 )
        break;
      }
      delete_static_nat_req_msg.snat_entry.port_fwding_protocol = (uint8_t)tmp_input;
      while (TRUE)
      {
        printf("   Please input port_fwding_private_ip(xxx.xxx.xxx.xxx)   : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !( inet_aton(scan_string, &addr) <= 0) &&
               ( addr.s_addr != tmp1.s_addr ) &&
               ( addr.s_addr != tmp2.s_addr ))
            break;
        }
        printf("   Invalid IPv4 address %s",scan_string);
      }
      delete_static_nat_req_msg.snat_entry.port_fwding_private_ip = ntohl(addr.s_addr);
      while (TRUE)
      {
        printf("   Please input port_fwding_private_port : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
         /*No port validation check for ICMP*/
        if(delete_static_nat_req_msg.snat_entry.port_fwding_protocol == 1)
          break;
        if(check_port (tmp_input) == 0 )
          break;
      }

      delete_static_nat_req_msg.snat_entry.port_fwding_private_port = (uint16_t)tmp_input;

      while (TRUE)
      {
        printf("   Please input port_fwding_global_port  : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        /*No port validation check for ICMP*/
        if(delete_static_nat_req_msg.snat_entry.port_fwding_protocol == 1)
        break;
        if(check_port (tmp_input) == 0 )
          break;
      }

      delete_static_nat_req_msg.snat_entry.port_fwding_global_port = (uint16_t)tmp_input;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      delete_static_nat_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_DELETE_STATIC_NAT_ENTRY_REQ_V01,
                                           &delete_static_nat_req_msg,
                                           &delete_static_nat_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( delete_static_nat_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           (delete_static_nat_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
#ifdef TARGET_IS_9615
        LOG_MSG_ERROR("Can not delete static_nat entry %d : %d\n",
                      mcm_error, delete_static_nat_resp_msg.resp.error,0);
#else /* TARGET_IS_9615 */
        if (delete_static_nat_resp_msg.resp.error ==
            MCM_ERROR_INTERFACE_NOT_FOUND_V01)
          printf("\nBackhaul down, SNAT Entry deleted from xml file.\n\n");
        else
          printf("\nSNAT Entry delete failed, Error: 0x%x\n\n",
                 delete_static_nat_resp_msg.resp.error);
#endif /* !(TARGET_IS_9615) */
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete snat entry failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Deleted snat entry successfully. ",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete snat entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 10:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_set_nat_timeout_req_msg_v01 set_nat_timeout_req_msg;
      mcm_mobileap_set_nat_timeout_resp_msg_v01 set_nat_timeout_resp_msg;
      mcm_error_t_v01 mcm_error;

      while (TRUE)
      {
#ifdef TARGET_IS_9615
        printf(" Please input timeout type (should be 1) (Generic Timeout Type supported for now) : \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = (mcm_mobileap_nat_timeout_t_v01)atoi(scan_string);
        if (tmp_input == MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01)
        {
          break;
        }
#else /* TARGET_IS_9615 */
        printf("Select the Type of Timeout : \n"
               "1: GENRIC TIMEOUT\t2: ICMP TIMEOUT\n"
               "3: TCP TIMEOUT ESTABLISHED\t4: UDP TIMEOUT\t:::");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = (mcm_mobileap_nat_timeout_t_v01)atoi(scan_string);
        if ((tmp_input >= MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01) &&
            (tmp_input <= MCM_MOBILEAP_NAT_TIMEOUT_UDP_V01))
        {
          break;
        }
#endif /* !(TARGET_IS_9615) */
        printf("Invalid Input. \n");
      }

      set_nat_timeout_req_msg.timeout_type = tmp_input;

      while (TRUE)
      {
        printf("Please input timeout: (should be >= 30)\n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        if(tmp_input >= 30)
          break;
        printf("Invalid Input. Please input timeout: (should be >= 30)\n");
      }

      set_nat_timeout_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      set_nat_timeout_req_msg.timeout_value = tmp_input;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_NAT_TIMEOUT_REQ_V01,
                                           &set_nat_timeout_req_msg,
                                           &set_nat_timeout_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( set_nat_timeout_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( set_nat_timeout_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set nat timeout %d : %d\n",
                       mcm_error, set_nat_timeout_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set nat timeout failed    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("NAT Timeout Set succeeded...\n", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set nat timeout successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 11:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_get_nat_timeout_req_msg_v01 get_nat_timeout_req_msg;
      mcm_mobileap_get_nat_timeout_resp_msg_v01 get_nat_timeout_resp_msg;
      mcm_error_t_v01 mcm_error;

      while (TRUE)
      {
#ifdef TARGET_IS_9615
        printf("   Please input timeout type (should be 1) (Generic Timeout Type supported for now) : \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = (mcm_mobileap_nat_timeout_t_v01)atoi(scan_string);
        if (tmp_input == MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01)
          break;
#else /* TARGET_IS_9615 */
        printf("Select the Type of Timeout : \n"
               "1: GENRIC TIMEOUT\t2: ICMP TIMEOUT\n"
               "3: TCP TIMEOUT ESTABLISHED\t4: UDP TIMEOUT\t:::");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = (mcm_mobileap_nat_timeout_t_v01)atoi(scan_string);
        if ((tmp_input >= MCM_MOBILEAP_NAT_TIMEOUT_GENERIC_V01) &&
            (tmp_input <= MCM_MOBILEAP_NAT_TIMEOUT_UDP_V01))
          break;
#endif /* !(TARGET_IS_9615) */
        printf("Invalid Input. \n");
      }
      get_nat_timeout_req_msg.timeout_type = tmp_input;
      get_nat_timeout_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_NAT_TIMEOUT_REQ_V01,
                                           &get_nat_timeout_req_msg,
                                           &get_nat_timeout_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_nat_timeout_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_nat_timeout_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not get nat timeout %d : %d\n",
                       mcm_error, get_nat_timeout_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get nat timeout failed    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Get NAT Timeout succeeded...\n", 0, 0, 0);
        if( get_nat_timeout_resp_msg.timeout_value_valid)
        {
          printf(" NAT Timeout value : %d\n", get_nat_timeout_resp_msg.timeout_value);
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get nat timeout successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;


/********************************************************************/
    case 12:
    {
      mcm_mobileap_set_nat_type_req_msg_v01 set_nat_type_req_msg;
      mcm_error_t_v01 mcm_error;
      uint32_t mcm_err_num;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      while (TRUE)
      {
#ifdef TARGET_IS_9615
        printf("   Please input nat_type: (should be 0: SYMMETRIC or 1: PORT RESTRICTED CONE) : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);

        if((tmp_input == 0) || (tmp_input == 1))
          break;
#else /* TARGET_IS_9615 */
        printf(" Please input nat_type: (should be 0: SYMMETRIC NAT\n "
               "1: PORT RESTRICTED CONE NAT \n2: FULL CONE NAT \n"
               "3: ADDRESS RESTRICTED CONE NAT\n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if((tmp_input >= MCM_MOBILEAP_NAT_SYMMETRIC_NAT_V01) &&
           (tmp_input <= MCM_MOBILEAP_NAT_ADDRESS_RESTRICTED_NAT_V01))
          break;
#endif /* !(TARGET_IS_9615) */
        printf("Invalid Input.\n");
      }

      mcm_mobileap_set_nat_type_resp_msg = (mcm_mobileap_set_nat_type_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));

      if ( mcm_mobileap_set_nat_type_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();
      memset(&set_nat_type_req_msg, 0, sizeof(mcm_mobileap_set_nat_type_req_msg_v01));
      memset(mcm_mobileap_set_nat_type_resp_msg, 0, sizeof(mcm_mobileap_set_nat_type_resp_msg_v01));


      set_nat_type_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      set_nat_type_req_msg.nat_type = tmp_input;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_NAT_TYPE_REQ_V01,
                                           &set_nat_type_req_msg,
                                           mcm_mobileap_set_nat_type_resp_msg,
                                           (mcm_client_async_cb)mcm_mobileap_async_cb,
                                           &token_id);
      if(NULL == mcm_mobileap_set_nat_type_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_set_nat_type_resp_msg is NULL", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP change nat type failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_set_nat_type_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           (mcm_mobileap_set_nat_type_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set nat_type %d : %d",
        mcm_error, mcm_mobileap_set_nat_type_resp_msg->resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP change nat type failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
       printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("NAT Type Set succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP change nat type successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 13:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_nat_type_req_msg_v01 get_nat_type_req_msg;
      mcm_mobileap_get_nat_type_resp_msg_v01 get_nat_type_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_nat_type_req_msg.mcm_mobileap_handle= mcm_mobileap_config.mcm_mobileap_handle;
      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_NAT_TYPE_REQ_V01,
                                           &get_nat_type_req_msg,
                                           &get_nat_type_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_nat_type_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_nat_type_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not get nat type %d : %d\n",
                        mcm_error, get_nat_type_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get nat type failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        if (get_nat_type_resp_msg.nat_type_valid)
        {
          LOG_MSG_INFO1("NAT Type Get succeeded...%d",
                        get_nat_type_resp_msg.nat_type,0,0);
          if(get_nat_type_resp_msg.nat_type ==
             MCM_MOBILEAP_NAT_SYMMETRIC_NAT_V01)
          {
            printf("NAT Type is SYMMETRIC NAT\n");
          }
          else if(get_nat_type_resp_msg.nat_type ==
                  MCM_MOBILEAP_NAT_PORT_RESTRICTED_CONE_NAT_V01)
          {
            printf("NAT Type is PORT RESTRICTED CONE NAT\n");
          }
#ifndef TARGET_IS_9615
          else if(get_nat_type_resp_msg.nat_type ==
                  MCM_MOBILEAP_NAT_FULL_CONE_NAT_V01)
          {
            printf("NAT Type is FULL CONE NAT\n");
          }
          else if(get_nat_type_resp_msg.nat_type ==
                  MCM_MOBILEAP_NAT_ADDRESS_RESTRICTED_NAT_V01)
          {
            printf("NAT Type is NAT ADDRESS RESTRICTED CONE NAT\n");
          }
#endif /* !(TARGET_IS_9615) */
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get nat type successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 14:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      struct in_addr                             ip4_src_addr;
      struct in_addr                             ip4_dst_addr;
      struct in_addr                             ip4_src_subnet_mask;
      struct in_addr                             ip4_dst_subnet_mask;
      struct in6_addr                     ip6_src_addr;
      struct in6_addr                     ip6_dst_addr;
      int                                 input_len;
      int                                 inc;
      uint32_t                             result;
      int                                 ip4_res;
      int                                 ip6_res;
      int                                 ip4_result;
      int                                 ip6_result;
      uint8_t                             next_hdr_prot = 0;
      char                              scan_string[32];
      char                              ip4_input[32];
      char                              ip6_input[48];
      char                              next_hdr_input[32];

      mcm_mobileap_add_firewall_entry_req_msg_v01 add_firewall_config_req_msg_v01;
      mcm_mobileap_add_firewall_entry_resp_msg_v01 add_firewall_config_resp_msg_v01;
      mcm_error_t_v01 mcm_error;

      memset(&add_firewall_config_req_msg_v01, 0, sizeof(add_firewall_config_req_msg_v01));

      add_firewall_config_req_msg_v01.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

       printf("\n Please input IP family type : \t"
              "Enter 4 for IPV4 and 6 for IPV6:");
       fgets(scan_string, sizeof(scan_string), stdin);
       result = atoi(scan_string);

      if(result == MCM_MOBILEAP_IP_FAMILY_V4_V01)
      {
        add_firewall_config_req_msg_v01.ip_version = MCM_MOBILEAP_IP_FAMILY_V4_V01;
        printf("\n Do you want to enter IPV4 source address \t "
               "and subnet mask: 1 for YES, 0 for NO:\n");
        fgets(ip4_input, sizeof(ip4_input), stdin);
        ip4_res = atoi(ip4_input);
        if(ip4_res==1)
        {
          while (TRUE)
          {
            printf("\n Please input IPV4 address: ");
            if (fgets(ip4_input, sizeof(ip4_input), stdin) != NULL)
            {
              if (inet_aton(ip4_input, &ip4_src_addr))
              {
                add_firewall_config_req_msg_v01.ip4_src_addr_valid = TRUE;
                add_firewall_config_req_msg_v01.ip4_src_addr.addr = ip4_src_addr.s_addr;
                LOG_MSG_INFO1("IP4 src addr is:", 0, 0, 0);
                IPV4_ADDR_MSG(add_firewall_config_req_msg_v01.ip4_src_addr.addr);
                break;
              }
            }
            printf("Invalid IPv4 address \n");
          }

          while (TRUE)
          {
            printf("\n Please input IPV4 subnet mask: ");
            if (fgets(ip4_input, sizeof(ip4_input), stdin) != NULL)
            {
              if (inet_aton(ip4_input, &ip4_src_subnet_mask))
              {
                add_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask = ip4_src_subnet_mask.s_addr;
                LOG_MSG_INFO1("IP4 src subnet mask is:", 0, 0, 0);
                IPV4_ADDR_MSG(add_firewall_config_req_msg_v01.ip4_src_addr.subnet_mask);
                break;
              }
            }
            printf("\n Invalid IPv4 subnet mask \n");
          }
        }

        printf("\n Do you want to enter IPV4 destination address and \t"
                "subnet mask: enter 1 for Yes and 0 for No\n");
        fgets(ip4_input, sizeof(ip4_input), stdin);
        ip4_res = atoi(ip4_input);
        if(ip4_res == 1)
        {
          while (TRUE)
          {
            printf("\n Please input IPV4 address: ");
            if (fgets(ip4_input, sizeof(ip4_input), stdin) != NULL)
            {
              if (inet_aton(ip4_input, &ip4_dst_addr))
              {
                add_firewall_config_req_msg_v01.ip4_dst_addr_valid = TRUE;
                add_firewall_config_req_msg_v01.ip4_dst_addr.addr= ip4_dst_addr.s_addr;
                LOG_MSG_INFO1("IP4 dst addr is:", 0, 0, 0);
                IPV4_ADDR_MSG(add_firewall_config_req_msg_v01.ip4_dst_addr.addr);
                break;
              }
            }
            printf("Invalid IPv4 address\n");
          }

          while (TRUE)
          {
            printf("\n Please input IPV4 subnet mask: ");
            if (fgets(ip4_input, sizeof(ip4_input), stdin) != NULL)
            {
              if (inet_aton(ip4_input, &ip4_dst_subnet_mask))
              {
                add_firewall_config_req_msg_v01.ip4_dst_addr.subnet_mask=
                                              ip4_dst_subnet_mask.s_addr;
                LOG_MSG_INFO1("IP4 dst subnet mask is:", 0, 0, 0);
                IPV4_ADDR_MSG(add_firewall_config_req_msg_v01.ip4_dst_addr.subnet_mask);
                break;
             }
            }
            printf("\n Invalid IPv4 subnet mask \n");
          }
        }

        printf("\n Please input IPV4 type of service value: ");
        fgets(ip4_input, sizeof(ip4_input), stdin);
        if ( ip4_input[0]!='\n')
        {
          add_firewall_config_req_msg_v01.ip4_tos_valid = TRUE;
          add_firewall_config_req_msg_v01.ip4_tos.value= (uint8_t)atoi(ip4_input);
          LOG_MSG_INFO1("IPv4 ToS value : %d",add_firewall_config_req_msg_v01.ip4_tos.value,0,0);
          printf("\n Please input IPV4 type of service mask: ");
          fgets(ip4_input, sizeof(ip4_input), stdin);
          if ( ip4_input[0]!='\n')
          {
            add_firewall_config_req_msg_v01.ip4_tos.mask = (uint8_t)atoi(ip4_input);
            LOG_MSG_INFO1("IPv4 ToS mask : %d",add_firewall_config_req_msg_v01.ip4_tos.mask,0,0);
          }
        }

        while (TRUE)
        {
          printf("\n Please input IPV4 next header protocol : \t"
                 "TCP=6, UDP=17,ICMP=1,ESP=50, TCP_UDP=253: ");
          fgets(ip4_input, sizeof(ip4_input), stdin);
          if ( ip4_input[0]!= '\n')
          {
            ip4_result = atoi(ip4_input);
            if(ip4_result == 6 || ip4_result == 17 || ip4_result == 1 ||
               ip4_result ==50 || ip4_result ==253)
            {
              add_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
              add_firewall_config_req_msg_v01.next_hdr_prot = (uint8_t)ip4_result;
              next_hdr_prot = add_firewall_config_req_msg_v01.next_hdr_prot;
             break;
            }
          }
        }
      }

      else if(result == MCM_MOBILEAP_IP_FAMILY_V6_V01)
      {
        add_firewall_config_req_msg_v01.ip_version = MCM_MOBILEAP_IP_FAMILY_V6_V01;
        printf("\n Do you want to enter IPV6 source address and prefix:\t"
               " 1 for yes and 0 for no\n");
        fgets(ip6_input, sizeof(ip6_input), stdin);
        ip6_res = atoi(ip6_input);
        if(ip6_res==1)
        {
          while (TRUE)
          {
            printf("\n Please input IPV6 address: ");
            if (fgets(ip6_input, sizeof(ip6_input), stdin) != NULL)
            {
              input_len = strlen(ip6_input);
              ip6_input[input_len-1] = '\0';
              ip6_result =inet_pton(AF_INET6,ip6_input, &ip6_src_addr);
              if (ip6_result)
              {
                add_firewall_config_req_msg_v01.ip6_src_addr_valid = TRUE;
                memcpy( add_firewall_config_req_msg_v01.ip6_src_addr.addr,
                        ip6_src_addr.s6_addr, MCM_MOBILEAP_IPV6_ADDR_LEN_V01*sizeof(uint8_t));
                break;
              }
            }
            printf("Invalid IPv6 address \n");
          }

          while (TRUE)
          {
            printf("\n Please input IPV6 prefix length: ");
            fgets(ip6_input, sizeof(ip6_input), stdin);
            if ( ip6_input[0] != '\n' )
            {
              add_firewall_config_req_msg_v01.ip6_src_addr.prefix_len = (uint8_t)atoi(ip6_input);
              LOG_MSG_INFO1("IPv6 Source Prefix Length : %d",
                            add_firewall_config_req_msg_v01.ip6_src_addr.prefix_len,0,0);
              break;
            }
          }
        }

        printf("\n Do you want to enter IPV6 destination address \t"
               "and prefix: 1 for Yes and 0 for No\n");
        fgets(ip6_input, sizeof(ip6_input), stdin);
        ip6_res = atoi(ip6_input);
        if(ip6_res==1)
        {
          while (TRUE)
          {
            printf("\n Please input IPV6 address: ");
            if (fgets(ip6_input, sizeof(ip6_input), stdin) != NULL)
            {
              input_len = strlen(ip6_input);
              ip6_input[input_len-1] = '\0';
              ip6_result = inet_pton(AF_INET6,ip6_input, &ip6_dst_addr);
              if (ip6_result)
              {
                add_firewall_config_req_msg_v01.ip6_dst_addr_valid = TRUE;
                memcpy( add_firewall_config_req_msg_v01.ip6_dst_addr.addr,
                        ip6_dst_addr.s6_addr, MCM_MOBILEAP_IPV6_ADDR_LEN_V01*sizeof(uint8_t));
                break;
              }
            }
            printf("Invalid IPv6 address \n");
          }

          while (TRUE)
          {
            printf("\n Please input IPV6 prefix length: ");
            fgets(ip6_input, sizeof(ip6_input), stdin);
            if ( ip6_input[0] != '\n')
            {
              add_firewall_config_req_msg_v01.ip6_dst_addr.prefix_len = (uint8_t)atoi(ip6_input);
              LOG_MSG_INFO1("IPv6 Dst Prefix Length : ",
                            add_firewall_config_req_msg_v01.ip6_dst_addr.prefix_len,0,0);
              break;
            }
          }
        }

        printf("\n Please input IPV6 traffic class value: ");
        fgets(ip6_input, sizeof(ip6_input), stdin);
        if ( ip6_input[0]!='\n')
        {
          add_firewall_config_req_msg_v01.ip6_trf_cls_valid = TRUE;
          add_firewall_config_req_msg_v01.ip6_trf_cls.value = (uint8_t)atoi(ip6_input);
          LOG_MSG_INFO1("IPv6 Traffic class value : ",
                        add_firewall_config_req_msg_v01.ip6_trf_cls.value,0,0);
        }

        printf("\n Please input IPV6 traffic class mask:");
        fgets(ip6_input, sizeof(ip6_input), stdin);
        if( ip6_input[0] != '\n')
        {
          add_firewall_config_req_msg_v01.ip6_trf_cls.mask = (uint8_t)atoi(ip6_input);
          LOG_MSG_INFO1("IPv6 Traffic class mask : ",
                        add_firewall_config_req_msg_v01.ip6_trf_cls.mask,0,0);
        }

        while (TRUE)
        {
          printf("\n Please input IPV6 next header protocol: \t"
                 "TCP=6, UDP=17,ICMP6=58,ESP=50, TCP_UDP=253 :");
          fgets(ip6_input, sizeof(ip6_input), stdin);
          if ( ip6_input[0]!= '\n')
          {
            ip6_result = atoi(ip6_input);
            if(ip6_result == 6 || ip6_result == 17 || ip6_result == 58 ||
               ip6_result ==50 || ip6_result ==253)
            {
              add_firewall_config_req_msg_v01.next_hdr_prot_valid = TRUE;
              add_firewall_config_req_msg_v01.next_hdr_prot = (uint8_t)ip6_result;
              next_hdr_prot = add_firewall_config_req_msg_v01.next_hdr_prot;
              break;
            }
          }
        }
      }

      else
      {
        printf("\n Unsupported IP protocol ");
        break;
      }

      if(next_hdr_prot == IPPROTO_TCP)
      {
        while (TRUE)
        {
          printf("\n Please enter source/destination port and range. \n");
          printf("\n Please input TCP source port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
             break;
        }
        if ( next_hdr_input[0] != '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_src.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("TCP Source Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.port,0,0);
        }

        printf("\n Please input TCP source port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input [0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("TCP Source Port Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.range,0,0);
        }

        while (TRUE)
        {
          printf("\n Please input TCP destination port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
             break;
        }
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_dst.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("TCP Destination Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.port,0,0);
        }

        printf("\n Please input TCP destination port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0] != '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("TCP Dst Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.range,0,0);
        }
      }

      else if(next_hdr_prot == IPPROTO_UDP)
      {
        while (TRUE)
        {
          printf("\n Please enter source/destination port and range \n");
          printf("\n Please input UDP source port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
             break;
        }
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_src.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("UDP Source Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.port,0,0);
        }

        printf("\n Please input UDP source port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("UDP Source Port Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.range,0,0);
        }

        while (TRUE)
        {
          printf("\n Please input UDP destination port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
             break;
        }
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_dst.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("UDP Dst Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.port,0,0);
        }

        printf("\n Please input UDP destination port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("UDP Dst Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.range,0,0);
        }
      }

//////////////////////////////////////////////////////////////////////////////////////////////


      else if (next_hdr_prot == IPPROTO_TCP_UDP)
      {
        while (TRUE)
        {
          printf("\n Please enter source/destination port and range \n");
          printf("\n Please input TCP_UDP source port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
            break;
        }
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_src.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("TCP_UDP Source Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.port,0,0);
        }

        printf("\n Please input TCP_UDP source port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_src.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("TCP_UDP Source Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_src.range,0,0);
        }

        while (TRUE)
        {
          printf("\n Please input TCP_UDP destination port: ");
          fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
          tmp_input = atoi(next_hdr_input);
          if(mcm_mobileap_check_port (tmp_input) == 0 )
             break;
        }
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst_valid = TRUE;
          add_firewall_config_req_msg_v01.tcp_udp_dst.port = (uint16_t)tmp_input;
          LOG_MSG_INFO1("TCP_UDP Dst Port : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.port,0,0);
        }

        printf("\n Please input TCP_UDP destination port range: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.tcp_udp_dst.range = (uint16_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("TCP_UDP Dst Range : %d",
                        add_firewall_config_req_msg_v01.tcp_udp_dst.range,0,0);
        }
      }

      else if (next_hdr_prot == IPPROTO_ICMP || next_hdr_prot == IPPROTO_ICMP6)
      {
        printf("\n Please input ICMP type: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.icmp_type_valid = TRUE;
          add_firewall_config_req_msg_v01.icmp_type = (uint8_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("ICMP Type : %d",
                        add_firewall_config_req_msg_v01.icmp_type,0,0);
        }

        printf("\n Please input ICMP code: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.icmp_code_valid = TRUE;
          add_firewall_config_req_msg_v01.icmp_code = (uint8_t)atoi(next_hdr_input);
          LOG_MSG_INFO1("ICMP Code : %d",
                        add_firewall_config_req_msg_v01.icmp_code,0,0);
        }
      }

      else if (next_hdr_prot == IPPROTO_ESP)
      {
        printf("\n Please input ESP SPI: ");
        fgets(next_hdr_input, sizeof(next_hdr_input), stdin);
        if ( next_hdr_input[0]!= '\n')
        {
          add_firewall_config_req_msg_v01.esp_spi_valid = TRUE;
          add_firewall_config_req_msg_v01.esp_spi = atoi(next_hdr_input);
          LOG_MSG_INFO1("ESP SPI : %d",
                        add_firewall_config_req_msg_v01.esp_spi,0,0);
        }
      }

      else
      {
        printf("Unsupported protocol");
        break;
      }

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_ADD_FIREWALL_ENTRY_REQ_V01,
                                           &add_firewall_config_req_msg_v01,
                                           &add_firewall_config_resp_msg_v01,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( add_firewall_config_resp_msg_v01.resp.result != MCM_RESULT_SUCCESS_V01) ||
           ( add_firewall_config_resp_msg_v01.firewall_handle_valid != TRUE ) ||
           ( add_firewall_config_resp_msg_v01.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR( "Add firewall config failed %d %d", mcm_error,
                       add_firewall_config_resp_msg_v01.resp.error,0);

        if(add_firewall_config_resp_msg_v01.resp.error == MCM_ERROR_NO_EFFECT_V01 )
        {
          LOG_MSG_ERROR("Entry Already present ",0,0,0);
          printf("\nEntry already present\n\n");
        }
        else if( add_firewall_config_resp_msg_v01.resp.error == MCM_ERROR_INSUFFICIENT_RESOURCES_V01)
        {
          LOG_MSG_ERROR("Maximium entries Added",0,0,0);
          printf("\nMaximum entries added\n\n");
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add firewall entry failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("\nAdded the firewall entry and handle is =%d \n",
                      add_firewall_config_resp_msg_v01.firewall_handle,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add firewall entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/********************************************************************/
    case 15:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01 get_firewall_entries_handle_list_req_msg;
      mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01 get_firewall_entries_handle_list_resp_msg;
      uint32_t result;
      uint32_t index = 0;
      struct in_addr addr;
      mcm_error_t_v01 mcm_error;

      printf("\n Please input IP family type : \t"
             "Enter 4 for IPV4 and 6 for IPV6:");
      fgets(scan_string, sizeof(scan_string), stdin);
      result = atoi(scan_string);
      if(result == MCM_MOBILEAP_IP_FAMILY_V4_V01)
      {
        get_firewall_entries_handle_list_req_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V4_V01;
      }
      else if(result == MCM_MOBILEAP_IP_FAMILY_V6_V01)
      {
        get_firewall_entries_handle_list_req_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V6_V01;
      }

      get_firewall_entries_handle_list_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01,
                                           &get_firewall_entries_handle_list_req_msg,
                                           &get_firewall_entries_handle_list_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_firewall_entries_handle_list_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 )||
           ( get_firewall_entries_handle_list_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Get FIREWALL Entry Handle List failed : %d %d\n",mcm_error,
                      get_firewall_entries_handle_list_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get firewall entry handle list failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Get FIREWALL Entry Handle List success\n",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP Get firewall entry handle list successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");

        if( get_firewall_entries_handle_list_resp_msg.firewall_handle_list_valid &&
          ( get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len != 0))
        {
          printf("Printing Handle list\n\n");
          while (index < get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len)
          {
            printf(" %d. %d", index + 1, get_firewall_entries_handle_list_resp_msg.firewall_handle_list[index]);
            index = index + 1;
          }
          while (TRUE)
          {
            printf("Please Enter serial number of firewall handle which you want to get : ");
            fgets(scan_string, sizeof(scan_string), stdin);
            index = atoi(scan_string);
            if ((index >=1) && (index <= get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len))
              break;
            printf("\nInvalid Serial Number.\n\n");
          }
        }
        else
        {
          printf("No firewalls configured\n");
          break;
        }
      }

      mcm_mobileap_delete_firewall_entry_req_msg_v01 delete_firewall_entry_req_msg;
      mcm_mobileap_delete_firewall_entry_resp_msg_v01 delete_firewall_entry_resp_msg;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      delete_firewall_entry_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      delete_firewall_entry_req_msg.firewall_handle = get_firewall_entries_handle_list_resp_msg.firewall_handle_list[index - 1];
      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_DELETE_FIREWALL_ENTRY_REQ_V01,
                                           &delete_firewall_entry_req_msg,
                                           &delete_firewall_entry_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
         ( delete_firewall_entry_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 )||
         ( delete_firewall_entry_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Deleted FIREWALL Entry failed : %d %d",mcm_error,
                      delete_firewall_entry_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete firewall entry failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Deleted FIREWALL Entry success",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete firewall entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

break;
/********************************************************************/

    case 16:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01 get_firewall_entries_handle_list_req_msg;
      mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01 get_firewall_entries_handle_list_resp_msg;
      uint32_t                result;
      uint32_t                index = 0;
      char*                 output;
      struct in_addr addr;
      mcm_error_t_v01 mcm_error;

      printf("\n Please input IP family type : \t"
             "Enter 4 for IPV4 and 6 for IPV6:");
      fgets(scan_string, sizeof(scan_string), stdin);
      result = atoi(scan_string);
      if(result == MCM_MOBILEAP_IP_FAMILY_V4_V01)
      {
        get_firewall_entries_handle_list_req_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V4_V01;
      }
      else if(result == MCM_MOBILEAP_IP_FAMILY_V6_V01)
      {
        get_firewall_entries_handle_list_req_msg.ip_version = MCM_MOBILEAP_IP_FAMILY_V6_V01;
      }
      else
      {
        LOG_MSG_INFO1("\n Invalid IP Family. \n", 0, 0, 0);
        break;
      }

      get_firewall_entries_handle_list_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01,
                                           &get_firewall_entries_handle_list_req_msg,
                                           &get_firewall_entries_handle_list_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
         ( get_firewall_entries_handle_list_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 )||
         ( get_firewall_entries_handle_list_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Get FIREWALL Entry Handle List failed : %d %d",mcm_error,
                      get_firewall_entries_handle_list_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get firewall entry handle list failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Get FIREWALL Entry Handle List success",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP Get firewall entry handle list successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");

        if( get_firewall_entries_handle_list_resp_msg.firewall_handle_list_valid &&
            (get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len != 0))
        {
          printf("Printing Handle list\n\n");
          while (index < get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len)
          {
            printf(" %d. %d\n", index + 1, get_firewall_entries_handle_list_resp_msg.firewall_handle_list[index]);
            index = index + 1;
          }

          while (TRUE)
          {
            printf("Please Enter serial number of firewall handle which you want to get : ");
            fgets(scan_string, sizeof(scan_string), stdin);
            index = atoi(scan_string);
            if ((index >=1) && (index <= get_firewall_entries_handle_list_resp_msg.firewall_handle_list_len))
              break;
            printf("\nInvalid Serial Number.\n\n");
          }
        }
        else
        {
          printf("No firewalls configured\n");
          break;
        }
      }

      int ret = 0;
      mcm_mobileap_get_firewall_entry_req_msg_v01 get_firewall_req;
      mcm_mobileap_get_firewall_entry_resp_msg_v01 get_firewall_resp;
      uint32_t i;
      int next_hdr_prot;
      char v6_addr[48];

      memset(&get_firewall_req,0,sizeof(get_firewall_req));
      memset(&get_firewall_resp,0,sizeof(get_firewall_resp));

      get_firewall_req.mcm_mobileap_handle =mcm_mobileap_config.mcm_mobileap_handle;
      get_firewall_req.firewall_handle = get_firewall_entries_handle_list_resp_msg.firewall_handle_list[index -1];
      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_FIREWALL_ENTRY_REQ_V01,
                                           &get_firewall_req,
                                           &get_firewall_resp,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
         ( get_firewall_resp.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
         ( get_firewall_resp.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR( "Get firewall config failed %d %d", mcm_error,get_firewall_resp.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get firewall entry failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("\nGet FireWallEntries .... \n",0,0,0);
        LOG_MSG_INFO1("IP family type = %d\n", get_firewall_resp.ip_version, 0, 0);
        switch(get_firewall_resp.ip_version)
        {
          case MCM_MOBILEAP_IP_FAMILY_V4_V01:
            if(get_firewall_resp.ip4_src_addr_valid)
            {
              LOG_MSG_INFO1("IPV4 src addr is:\n", get_firewall_resp.ip4_src_addr.addr, 0, 0);
              IPV4_ADDR_MSG(get_firewall_resp.ip4_src_addr.addr);
              addr.s_addr = ntohl(get_firewall_resp.ip4_src_addr.addr);
              output = inet_ntoa(addr);
              printf("\nIPV4 Src Addr = %s\n",output);

              LOG_MSG_INFO1("IPV4 src subnet mask\n", get_firewall_resp.ip4_src_addr.subnet_mask, 0, 0);
              IPV4_ADDR_MSG(get_firewall_resp.ip4_src_addr.subnet_mask);
              addr.s_addr = ntohl(get_firewall_resp.ip4_src_addr.subnet_mask);
              output = inet_ntoa(addr);
              printf("\nIPv4 Src Subnet Mask = %s\n",output);
            }

            if(get_firewall_resp.ip4_dst_addr_valid)
            {
              LOG_MSG_INFO1("IPV4 dst addr is:\n", get_firewall_resp.ip4_dst_addr.addr, 0, 0);
              IPV4_ADDR_MSG(get_firewall_resp.ip4_dst_addr.addr);
              addr.s_addr = ntohl(get_firewall_resp.ip4_dst_addr.addr);
              output = inet_ntoa(addr);
              printf("\nIPv4 Dst Addr = %s\n",output);

              LOG_MSG_INFO1("IPV4 dst subnet mask\n", get_firewall_resp.ip4_dst_addr.subnet_mask, 0, 0);
              IPV4_ADDR_MSG(get_firewall_resp.ip4_dst_addr.subnet_mask);
              addr.s_addr = ntohl(get_firewall_resp.ip4_dst_addr.subnet_mask);
              output = inet_ntoa(addr);
              printf("\nIPv4 Dst Subnet Mask = %s\n",output);
            }

            if(get_firewall_resp.ip4_tos_valid)
            {
              LOG_MSG_INFO1("tos val %d mask %d\n",
                            get_firewall_resp.ip4_tos.value,
                            get_firewall_resp.ip4_tos.mask, 0);
              printf ("tos val %d mask %d\n",
                      get_firewall_resp.ip4_tos.value,
                      get_firewall_resp.ip4_tos.mask);
            }
          break;

          case MCM_MOBILEAP_IP_FAMILY_V6_V01:
          if(get_firewall_resp.ip6_src_addr_valid)
          {
            inet_ntop(AF_INET6, &get_firewall_resp.ip6_src_addr.addr, v6_addr,
                      sizeof(v6_addr));
            LOG_MSG_INFO1("IPV6 src addr is: %s\n", v6_addr, 0, 0);
            LOG_MSG_INFO1("IPV6 src prefix length %d ",
                          get_firewall_resp.ip6_src_addr.prefix_len, 0, 0);
          }

          if(get_firewall_resp.ip6_dst_addr_valid)
          {
            inet_ntop(AF_INET6, &get_firewall_resp.ip6_dst_addr.addr, v6_addr,
                      sizeof(v6_addr));
            LOG_MSG_INFO1("IPV6 dst addr is: %s\n", v6_addr, 0, 0);
            LOG_MSG_INFO1("IPV6 src prefix length %d \n",
                          get_firewall_resp.ip6_dst_addr.prefix_len, 0, 0);
          }

          if(get_firewall_resp.ip6_trf_cls_valid)
          {
            LOG_MSG_INFO1("ip6 trf class value %d mask %d\n",
                          get_firewall_resp.ip6_trf_cls.value,
                          get_firewall_resp.ip6_trf_cls.mask, 0);
            printf("ip6 trf class value %d mask %d\n",
                   get_firewall_resp.ip6_trf_cls.value,
                   get_firewall_resp.ip6_trf_cls.mask);
          }
          break;

          default:
            LOG_MSG_ERROR("BAD IP protocol\n", 0, 0, 0);
            printf("Bad IP protocol\n");
          break;
        }

        if(get_firewall_resp.next_hdr_prot_valid)
        {
          LOG_MSG_INFO1("Next header protocol is %d \n",
                        get_firewall_resp.next_hdr_prot, 0, 0);
          switch(get_firewall_resp.next_hdr_prot)
          {
            case IPPROTO_TCP:
            if(get_firewall_resp.tcp_udp_src_valid)
            {
              LOG_MSG_INFO1("TCP src port %d range %d\n",
                            get_firewall_resp.tcp_udp_src.port,
                            get_firewall_resp.tcp_udp_src.range,0);
              printf("TCP src port %d range %d\n",
                     get_firewall_resp.tcp_udp_src.port,
                     get_firewall_resp.tcp_udp_src.range);
            }

            if(get_firewall_resp.tcp_udp_dst_valid)
            {
              LOG_MSG_INFO1("TCP dst port %d range %d\n",
                            get_firewall_resp.tcp_udp_dst.port,
                            get_firewall_resp.tcp_udp_dst.range,0);
              printf("TCP dst port %d range %d\n",
                     get_firewall_resp.tcp_udp_dst.port,
                     get_firewall_resp.tcp_udp_dst.range);
            }
            break;

            case IPPROTO_UDP:
            if(get_firewall_resp.tcp_udp_src_valid)
            {
              LOG_MSG_INFO1("UDP src port %d range %d\n",
                            get_firewall_resp.tcp_udp_src.port,
                            get_firewall_resp.tcp_udp_src.range,0);
              printf("UDP src port %d range %d\n",
                     get_firewall_resp.tcp_udp_src.port,
                     get_firewall_resp.tcp_udp_src.range);
            }

            if(get_firewall_resp.tcp_udp_dst_valid)
            {
              LOG_MSG_INFO1("UDP dst port %d range %d\n",
                            get_firewall_resp.tcp_udp_dst.port,
                            get_firewall_resp.tcp_udp_dst.range,0);
              printf("UDP dst port %d range %d\n",
                     get_firewall_resp.tcp_udp_dst.port,
                     get_firewall_resp.tcp_udp_dst.range);
            }
            break;

            case IPPROTO_ICMP:
            case IPPROTO_ICMP6:
            if(get_firewall_resp.icmp_code_valid)
            {
              LOG_MSG_INFO1("ICMP code %d\n", get_firewall_resp.icmp_code, 0, 0);
              printf("ICMP code %d, get_firewall_resp.icmp_code\n");
            }

            if(get_firewall_resp.icmp_type_valid)
            {
              LOG_MSG_INFO1("ICMP type %d\n", get_firewall_resp.icmp_type, 0, 0);
              printf("ICMP type %d\n", get_firewall_resp.icmp_type);
            }
            break;

            case IPPROTO_ESP:
            if(get_firewall_resp.esp_spi_valid)
            {
              LOG_MSG_INFO1("ESP SPI %d\n", get_firewall_resp.esp_spi, 0, 0);
              printf("ESP SPI %d\n", get_firewall_resp.esp_spi);
            }
            break;

            case IPPROTO_TCP_UDP:
            if(get_firewall_resp.tcp_udp_src_valid)
            {
              LOG_MSG_INFO1("TCP_UDP src port %d range %d\n",
                            get_firewall_resp.tcp_udp_src.port,
                            get_firewall_resp.tcp_udp_src.range, 0);
              printf("TCP_UDP src port %d range %d\n",
                     get_firewall_resp.tcp_udp_src.port,
                     get_firewall_resp.tcp_udp_src.range);
            }

            if(get_firewall_resp.tcp_udp_dst_valid)
            {
              LOG_MSG_INFO1("TCP_UDP dst port %d range %d\n",
                            get_firewall_resp.tcp_udp_dst.port,
                            get_firewall_resp.tcp_udp_dst.range, 0);
              printf("TCP_UDP dst port %d range %d\n",
                     get_firewall_resp.tcp_udp_dst.port,
                     get_firewall_resp.tcp_udp_dst.range);
            }
            break;

            default:
              LOG_MSG_INFO1("Bad next header protocol %d\n",
                            get_firewall_resp.next_hdr_prot , 0, 0);
          }
        }

        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get firewall entry successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");

      }
    }

    break;

/*=======================================================================================*/

    case 17:
    {
      uint8_t enable_firewall, pkts_allowed;
      printf("   Please input Firewall State          : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (enable_firewall = atoi(scan_string))
      {
        printf("   Please input Packets Allowed Setting : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        pkts_allowed = atoi(scan_string);
      }
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_set_firewall_config_req_msg_v01 set_firewall_req_msg;
      mcm_mobileap_set_firewall_config_resp_msg_v01 set_firewall_resp_msg;
      mcm_error_t_v01 mcm_error;

      set_firewall_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      set_firewall_req_msg.firewall_enabled = enable_firewall;

      if(enable_firewall)
      {
      set_firewall_req_msg.pkts_allowed_valid = TRUE;
      set_firewall_req_msg.pkts_allowed = pkts_allowed;
      }

      LOG_MSG_INFO1("\n firewall_enabled =%d pkts_allowed =%d ", enable_firewall, pkts_allowed,0);

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_FIREWALL_CONFIG_REQ_V01,
                                           &set_firewall_req_msg,
                                           &set_firewall_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (
          ( mcm_error != MCM_SUCCESS_V01) ||
          ( set_firewall_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
          ( set_firewall_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set firewall config %d : %d",
                       mcm_error, set_firewall_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set firewall failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Set firewall config successfully ",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set firewall successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/*===================================================================================*/

    case 18:
    {
      mcm_mobileap_add_dmz_req_msg_v01 add_dmz_req_msg;
      mcm_mobileap_add_dmz_resp_msg_v01 add_dmz_resp_msg;
      mcm_error_t_v01 mcm_error;

      struct in_addr addr;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      memset(scan_string, 0, MCM_MOBILEAP_MAX_FILE_PATH_LEN);
      while (TRUE)
      {
        printf("   Please input DMZ IP to add(xxx.xxx.xxx.xxx) : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !(inet_aton(scan_string, &addr) <=0 ))
            break;
        }
        printf("   Invalid IPv4 address %d\n", scan_string);
      }
      add_dmz_req_msg.dmz_ip_addr = ntohl(addr.s_addr);

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      add_dmz_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;


      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_DMZ_REQ_V01,
                                           &add_dmz_req_msg,
                                           &add_dmz_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( add_dmz_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( add_dmz_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {

#ifndef TARGET_IS_9615
        if ( add_dmz_resp_msg.resp.error == MCM_ERROR_NO_EFFECT_V01 )
        {
          LOG_MSG_INFO1(" DMZ is already configured. Delete the current "
                        "configuration, if DMZ reconfiguration is needed ",
                        0, 0, 0);
        }
        else if (add_dmz_resp_msg.resp.error ==
                 MCM_ERROR_INTERFACE_NOT_FOUND_V01)
        {
          LOG_MSG_INFO1("\nBackhaul down. DMZ Entry added to xml file.",
                        0, 0, 0);
        }
        else
          LOG_MSG_ERROR("DMZ add fails. Error: 0x%x",
                        add_dmz_resp_msg.resp.error, 0, 0);
#else /* !(TARGET_IS_9615) */
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add dmz failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
#endif /* TARGET_IS_9615 */

      }
      else
      {
        LOG_MSG_INFO1("Added DMZ...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP add dmz successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*==========================================================================================*/

    case 19:
    {
      uint32_t dmz_ip=0;
      struct in_addr addr;
      mcm_mobileap_delete_dmz_req_msg_v01 delete_dmz_req_msg;
      mcm_mobileap_delete_dmz_resp_msg_v01 delete_dmz_resp_msg;
      mcm_error_t_v01 mcm_error;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      memset(scan_string, 0, MCM_MOBILEAP_MAX_FILE_PATH_LEN);
      while (TRUE)
      {
        printf("   Please input DMZ IP to delete(xxx.xxx.xxx.xxx) : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if ( !(inet_aton(scan_string, &addr) <=0 ))
            break;
        }
        printf("   Invalid IPv4 address %d\n", scan_string);
      }

      delete_dmz_req_msg.dmz_ip_addr = ntohl(addr.s_addr);

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      delete_dmz_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_DELETE_DMZ_REQ_V01,
                                           &delete_dmz_req_msg,
                                           &delete_dmz_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( delete_dmz_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( delete_dmz_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
#ifndef TARGET_IS_9615
        if (delete_dmz_resp_msg.resp.error == MCM_ERROR_INTERFACE_NOT_FOUND_V01)
        {
          LOG_MSG_INFO1("\nBackhaul down. DMZ Entry deleted from xml file. \n",
                        0, 0 ,0);
        }
        else
          LOG_MSG_ERROR("DMZ delete fails. Error: 0x%x",
                        delete_dmz_resp_msg.resp.error, 0, 0);
#else /* !(TARGET_IS_9615) */
        LOG_MSG_ERROR("Can not delete dmz %d : %d",
                      mcm_error, delete_dmz_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete dmz failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
#endif /* TARGET_IS_9615 */
      }
      else
      {
        LOG_MSG_INFO1("Deleted DMZ...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP delete dmz successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=============================================================================================*/


    case 20:
    {
      uint32_t dmz_ip=0;
      struct in_addr addr;
      char *dmz_ip_reply;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_get_dmz_req_msg_v01 get_dmz_req_msg;
      mcm_mobileap_get_dmz_resp_msg_v01 get_dmz_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_dmz_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_DMZ_REQ_V01,
                                           &get_dmz_req_msg,
                                           &get_dmz_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_dmz_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_dmz_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not get dmz %d : %d",
                      mcm_error, get_dmz_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get dmz failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        if (get_dmz_resp_msg.dmz_ip_addr_valid && get_dmz_resp_msg.dmz_ip_addr)
        {
          LOG_MSG_INFO1("\nGetDMZ IP successful",0,0,0);
          addr.s_addr = ntohl(get_dmz_resp_msg.dmz_ip_addr);
          dmz_ip_reply = inet_ntoa(addr);
          printf("DMZ IP = %s\n",dmz_ip_reply);
        }
        else
        {
          LOG_MSG_INFO1("\nNo DMZ IP addr configured",0,0,0);
          printf("/nNo DMZ IP addr configured \n");
        }

        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get dmz successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=======================================================================================*/

    case 21:
    {
      char *public_ip, *primary_dns, *secondary_dns;
      struct in_addr addr;
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_ipv4_wwan_config_req_msg_v01 get_ipv4_wwan_config_req_msg;
      mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01 get_ipv4_wwan_config_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_ipv4_wwan_config_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_IPV4_WWAN_CONFIG_REQ_V01,
                                           &get_ipv4_wwan_config_req_msg,
                                           &get_ipv4_wwan_config_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_ipv4_wwan_config_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_ipv4_wwan_config_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not get wwan config %d : %d",
                      mcm_error, get_ipv4_wwan_config_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get ipv4 wwan network config failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Get WWAN Config succeeded...",0,0,0);
        if (get_ipv4_wwan_config_resp_msg.v4_addr_valid)
        {
          addr.s_addr = ntohl(get_ipv4_wwan_config_resp_msg.v4_addr);
          public_ip = inet_ntoa(addr);
          printf("Public IP = %s\n",public_ip);
        }

        if (get_ipv4_wwan_config_resp_msg.v4_prim_dns_addr_valid)
        {
          addr.s_addr = ntohl(get_ipv4_wwan_config_resp_msg.v4_prim_dns_addr);
          primary_dns = inet_ntoa(addr);
          printf("Primary Dns IP = %s\n",primary_dns);
        }

        if (get_ipv4_wwan_config_resp_msg.v4_sec_dns_addr_valid)
        {
          addr.s_addr = ntohl(get_ipv4_wwan_config_resp_msg.v4_sec_dns_addr);
          secondary_dns = inet_ntoa(addr);
          printf("Secondary Dns IP = %s\n",secondary_dns);
        }

        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get ipv4 network config successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*==============================================================================================*/

    case 22:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_wwan_stats_req_msg_v01 get_wwan_stats_req_msg;
      mcm_mobileap_get_wwan_stats_resp_msg_v01 get_wwan_stats_resp_msg;
      mcm_error_t_v01 mcm_error;

      while (TRUE)
      {
        printf("     Please input IP family: (4 or 6) ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          get_wwan_stats_req_msg.ip_family = atoi(scan_string);
          if ((get_wwan_stats_req_msg.ip_family == MCM_MOBILEAP_IP_FAMILY_V4_V01) ||
              (get_wwan_stats_req_msg.ip_family == MCM_MOBILEAP_IP_FAMILY_V6_V01))
            break;
        }
        printf("   Invalid Input type %d\n", scan_string);
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_wwan_stats_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_WWAN_STATS_REQ_V01,
                                           &get_wwan_stats_req_msg,
                                           &get_wwan_stats_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_wwan_stats_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_wwan_stats_resp_msg.resp.error != MCM_SUCCESS_V01))
      {
        LOG_MSG_ERROR("Can not get wwan stats %d : %d",
                      mcm_error, get_wwan_stats_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get wwan stats failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        LOG_MSG_INFO1("Get WWAN Stats succeeded...",0,0,0);
        if(get_wwan_stats_resp_msg.wwan_stats_valid)
        {
          printf("bytes_rx = %llu\n", get_wwan_stats_resp_msg.wwan_stats.bytes_rx);
          printf("bytes_tx = %llu\n", get_wwan_stats_resp_msg.wwan_stats.bytes_tx);
          printf("pkts_rx = %d\n", get_wwan_stats_resp_msg.wwan_stats.pkts_rx);
          printf("pkts_tx = %d\n", get_wwan_stats_resp_msg.wwan_stats.pkts_tx);
          printf("pkts_dropped_rx = %d\n", get_wwan_stats_resp_msg.wwan_stats.pkts_dropped_rx);
          printf("pkts_dropped_tx = %d\n", get_wwan_stats_resp_msg.wwan_stats.pkts_dropped_tx);
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP wwan stats successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;

/*========================================================================================*/

    case 23:
    {
      mcm_mobileap_reset_wwan_stats_req_msg_v01 reset_wwan_stats_req_msg;
      mcm_mobileap_reset_wwan_stats_resp_msg_v01 reset_wwan_stats_resp_msg;
      mcm_error_t_v01 mcm_error;

      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      while (TRUE)
      {
        printf(" Please input IP family: (4 or 6) ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          reset_wwan_stats_req_msg.ip_family = atoi(scan_string);
          if ((reset_wwan_stats_req_msg.ip_family == MCM_MOBILEAP_IP_FAMILY_V4_V01) ||
              (reset_wwan_stats_req_msg.ip_family == MCM_MOBILEAP_IP_FAMILY_V6_V01))
            break;
        }
        printf("   Invalid Input type %d\n", scan_string);
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      reset_wwan_stats_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_RESET_WWAN_STATS_REQ_V01,
                                           &reset_wwan_stats_req_msg,
                                           &reset_wwan_stats_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( reset_wwan_stats_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( reset_wwan_stats_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not reset wwan stats %d : %d",
                      mcm_error, reset_wwan_stats_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP reset wwan stats failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Reset WWAN Stats succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP reset wwan stats successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*================================================================================================*/

    case 24:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_set_dhcpd_config_req_msg_v01 dhcpd_config_req_msg;
      mcm_mobileap_set_dhcpd_config_resp_msg_v01 dhcpd_config_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      dhcpd_config_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      struct in_addr start, end;
      uint32_t leasetime;
      int intf = 0;
      long val = 0;
      char *endptr;

#ifdef TARGET_IS_9615
      printf("   Please input Interface number (should be 0 or 1 or 2) : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      intf = atoi(scan_string);
#endif /* TARGET_IS_9615 */
      while (TRUE)
      {
        printf("   Please input starting DHCPD address : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if (inet_aton(scan_string, &start))
            break;
        }
        printf("      Invalid IPv4 address %d\n", scan_string);
      }
      while (TRUE)
      {
        printf("   Please input ending DHCPD address : ");
        if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
        {
          if (inet_aton(scan_string, &end))
            break;
        }
        printf("      Invalid IPv4 address %d\n", scan_string);
      }
      while (TRUE)
      {
        printf("   Please input DHCP lease time : ");
        fgets(scan_string, sizeof(scan_string), stdin);
        val = strtol(scan_string, &endptr, 10);
        if (val > 0)
        {
          if ((*endptr == '\0') || (*endptr == '\n') || (*endptr == '\r'))
          {
            printf("Lease time will be %ld seconds\n", val);
            *endptr = '\0';
            break;
          }
          else if ((*endptr == 'h') || (*endptr == 'H'))
          {
            printf("Lease time will be %ld hours\n", val);
            *(endptr + 1) = '\0';
            break;
          }
          else if ((*endptr == 'm') || (*endptr == 'M'))
          {
            printf("Lease time will be %ld minutes\n", val);
           *(endptr + 1) = '\0';
            break;
          }
        }
#ifdef TARGET_IS_9615
        else
        {
          scan_string[sizeof("infinite") - 1] = '\0';
          if (!strncmp(scan_string, "infinite", sizeof(scan_string)))
          {
            printf("Lease time will be infinite\n");
            break;
          }
        }
#endif /* TARGET_IS_9615 */
        printf("Lease time format is a positive decimal number followed by\n");
        printf("h for hours, m for minutes, or nothing for seconds.\n");
        printf("Or, can be infinite for an infinite lease time.\n");
      }
      printf("Lease time: %s\n", scan_string);

      dhcpd_config_req_msg.dhcpd_config.start= ntohl(start.s_addr);
      dhcpd_config_req_msg.dhcpd_config.end= ntohl(end.s_addr);
      dhcpd_config_req_msg.dhcpd_config.intf= intf;
      memcpy(dhcpd_config_req_msg.dhcpd_config.leasetime, scan_string, MCM_MOBILEAP_LEASE_TIME_LEN_V01);

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_DHCPD_CONFIG_REQ_V01,
                                           &dhcpd_config_req_msg,
                                           &dhcpd_config_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( dhcpd_config_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( dhcpd_config_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set dhcpd config  %d : %d",
                      mcm_error, dhcpd_config_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set dhcpd config failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Set DHCPD Config succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set dhcpd config successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }
    break;
/*==========================================================================================*/

    case 25:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_error_t_v01 mcm_error;
      mcm_mobileap_enable_wlan_req_msg_v01 enable_wlan_req_msg;

      mcm_mobileap_enable_wlan_resp_msg = (mcm_mobileap_enable_wlan_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));
      if ( mcm_mobileap_enable_wlan_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&enable_wlan_req_msg, 0, sizeof(mcm_mobileap_enable_wlan_req_msg_v01));
      memset(mcm_mobileap_enable_wlan_resp_msg, 0, sizeof(mcm_mobileap_enable_wlan_resp_msg_v01));

      LOG_MSG_INFO1("Enabling WLAN",0,0,0);

      enable_wlan_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_ENABLE_WLAN_REQ_V01,
                                            &enable_wlan_req_msg,
                                            mcm_mobileap_enable_wlan_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_enable_wlan_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_enable_wlan_resp_msg is NULL", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable wlan failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_enable_wlan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( mcm_mobileap_enable_wlan_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not enable WLAN %d : %d\n",
                      mcm_error, mcm_mobileap_enable_wlan_resp_msg->resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable wlan failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("MobileAP Enable WLAN Successful\n",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Posted Enable WLAN to server  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*============================================================================================*/

    case 26:
    {
      LOG_MSG_INFO1("Entered case: %d:\n", opt, 0,0);

      mcm_error_t_v01 mcm_error;
      mcm_mobileap_disable_wlan_req_msg_v01  disable_wlan_req_msg;

      mcm_mobileap_disable_wlan_resp_msg = (mcm_mobileap_disable_wlan_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));
      if ( mcm_mobileap_disable_wlan_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&disable_wlan_req_msg, 0, sizeof(mcm_mobileap_disable_wlan_req_msg_v01));
      memset(mcm_mobileap_disable_wlan_resp_msg, 0, sizeof(mcm_mobileap_disable_wlan_resp_msg_v01));

      LOG_MSG_INFO1("Disabling WLAN",0,0,0);

      disable_wlan_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_DISABLE_WLAN_REQ_V01,
                                            &disable_wlan_req_msg,
                                            mcm_mobileap_disable_wlan_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_disable_wlan_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_disable_wlan_resp_msg is NULL", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable wlan failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_disable_wlan_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( mcm_mobileap_disable_wlan_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not disable WLAN %d : %d\n",
                      mcm_error, mcm_mobileap_disable_wlan_resp_msg->resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable wlan failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("MobileAP Disable WLAN Successful\n",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Posted Disable WLAN to server  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=====================================================================================*/

    case 27:
      {
        LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
        mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01 set_ipsec_vpn_pt_req_msg;
        mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01 set_ipsec_vpn_pt_resp_msg;
        mcm_error_t_v01 mcm_error;

        while (TRUE)
        {
          printf("   Please input status of Pass Through: (1-Enable, 0-Disable) \n");
          fgets(scan_string, sizeof(scan_string), stdin);
          set_ipsec_vpn_pt_req_msg.vpn_pass_through_value = atoi(scan_string);
          if(( set_ipsec_vpn_pt_req_msg.vpn_pass_through_value == 0 ) ||
             ( set_ipsec_vpn_pt_req_msg.vpn_pass_through_value == 1 ))
            break;
          printf("Invalid Input.   Please input valid status\n");
        }

        MCM_MOBILEAP_LOG_FUNC_ENTRY();
        set_ipsec_vpn_pt_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

        mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_IPSEC_VPN_PASS_THROUGH_REQ_V01,
                                           &set_ipsec_vpn_pt_req_msg,
                                           &set_ipsec_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

        if (( mcm_error != MCM_SUCCESS_V01 ) ||
            (set_ipsec_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01))
        {
#ifndef TARGET_IS_9615
          if (set_ipsec_vpn_pt_resp_msg.resp.error ==
              MCM_ERROR_INTERFACE_NOT_FOUND_V01)
          {
            LOG_MSG_INFO1("Backhaul down. IPSEC VPN passthrough enabled in xml"
                          " file.", 0, 0, 0);
          }
          else
          {
            LOG_MSG_ERROR("IPSEC VPN passthrough set fails. Error: %d:  %d",
                          mcm_error, set_ipsec_vpn_pt_resp_msg.resp.error, 0);
          }
#else /* !(TARGET_IS_9615) */
          LOG_MSG_ERROR("Can not set ipsec vpn passthrough %d : %d",
                        mcm_error, set_ipsec_vpn_pt_resp_msg.resp.error,0);
#endif /* TARGET_IS_9615 */
          printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
          printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set ipsec vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
          printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        }
        else
        {
          LOG_MSG_INFO1("IPSEC Vpn Passthrough Set succeeded...",0,0,0);
          printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
          printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set ipsec vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
          printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        }
      }
    break;

/*============================================================================================*/

    case 28:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01 set_pptp_vpn_pt_req_msg;
      mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01 set_pptp_vpn_pt_resp_msg;
      mcm_error_t_v01 mcm_error;

      while (TRUE)
      {
        printf("   Please input status of Pass Through: (1-Enable, 0-Disable) \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        set_pptp_vpn_pt_req_msg.vpn_pass_through_value = atoi(scan_string);

        if(( set_pptp_vpn_pt_req_msg.vpn_pass_through_value == 0 ) ||
           ( set_pptp_vpn_pt_req_msg.vpn_pass_through_value == 1 ))
          break;
        printf("Invalid Input.   Please input valid status\n");
      }

      set_pptp_vpn_pt_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_PPTP_VPN_PASS_THROUGH_REQ_V01,
                                           &set_pptp_vpn_pt_req_msg,
                                           &set_pptp_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( set_pptp_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( set_pptp_vpn_pt_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
#ifndef TARGET_IS_9615
        if (set_pptp_vpn_pt_resp_msg.resp.error ==
            MCM_ERROR_INTERFACE_NOT_FOUND_V01)
        {
          LOG_MSG_INFO1("Backhaul down. PPTP VPN passthrough enabled"
                        " in xml file.\n", 0, 0, 0);
        }
        else
        {
          LOG_MSG_ERROR("PPTP VPN passthrough set fails. Error: 0x%x\n",
                        set_pptp_vpn_pt_resp_msg.resp.error, 0, 0);
        }
#else /* !(TARGET_IS_9615) */
        LOG_MSG_ERROR("Can not set pptp vpn passthrough %d : %d",
                      mcm_error, set_pptp_vpn_pt_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set pptp vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
#endif /* TARGET_IS_9615 */
      }
      else
      {
        LOG_MSG_INFO1("PPTP Vpn Passthrough Set succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set pptp vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=====================================================================================*/

    case 29:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01 set_l2tp_vpn_pt_req_msg;
      mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01 set_l2tp_vpn_pt_resp_msg;
      mcm_error_t_v01 mcm_error;
      while (TRUE)
      {
        printf("   Please input status of Pass Through: (1-Enable, 0-Disable) \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        set_l2tp_vpn_pt_req_msg.vpn_pass_through_value = atoi(scan_string);

        if(( set_l2tp_vpn_pt_req_msg.vpn_pass_through_value == 0 ) ||
           ( set_l2tp_vpn_pt_req_msg.vpn_pass_through_value == 1 ))
          break;
        printf("Invalid Input.   Please input valid status\n");
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      set_l2tp_vpn_pt_req_msg.mcm_mobileap_handle =mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_L2TP_VPN_PASS_THROUGH_REQ_V01,
                                           &set_l2tp_vpn_pt_req_msg,
                                           &set_l2tp_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if ((mcm_error != MCM_SUCCESS_V01) ||
          (set_l2tp_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01))
      {
#ifdef TARGET_IS_9615
        LOG_MSG_ERROR("Can not set l2tp vpn passthrough %d : %d",
                      mcm_error, set_l2tp_vpn_pt_resp_msg.resp.error, 0);
#else /* TARGET_IS_9615 */
        if (set_l2tp_vpn_pt_resp_msg.resp.error ==
            MCM_ERROR_INTERFACE_NOT_FOUND_V01 )
        {
          LOG_MSG_INFO1("Backhaul down. L2TP VPN passthrough enabled"
                        " in xml file.\n", 0, 0, 0);
        }
        else
        {
          LOG_MSG_ERROR("\nL2TP VPN passthrough set fails. Error: 0x%x\n",
                        set_l2tp_vpn_pt_resp_msg.resp.error, 0, 0);
        }
#endif /* !(TARGET_IS_9615) */
      printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set l2tp vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
      printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");

      }
      else
      {
        LOG_MSG_INFO1("L2TP Vpn Passthrough Set succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set l2tp vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=============================================================================================*/

    case 30:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_v01 get_ipsec_vpn_pt_req_msg;
      mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01 get_ipsec_vpn_pt_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_ipsec_vpn_pt_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_IPSEC_VPN_PASS_THROUGH_REQ_V01,
                                           &get_ipsec_vpn_pt_req_msg,
                                           &get_ipsec_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_ipsec_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) )
      {
        LOG_MSG_ERROR("Can not get ipsec vpn passthrough %d : %d",
                      mcm_error, get_ipsec_vpn_pt_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get ipsec vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        if (get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value_valid)
        {
          LOG_MSG_ERROR("IPSEC Vpn Passthrough Get succeeded...%d",
                        get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value,0,0);
          if(get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value == 1)
          {
            printf("IPSEC VPN PassThrough Flag : SET\n");
          }
          else if(get_ipsec_vpn_pt_resp_msg.vpn_pass_through_value == 0)
          {
            printf("IPSEC VPN PassThrough Flag : UNSET\n");
          }
        }
      printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP get ipsec vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
      printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=======================================================================================*/


    case 31:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_get_pptp_vpn_pass_through_req_msg_v01 get_pptp_vpn_pt_req_msg;
      mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01 get_pptp_vpn_pt_resp_msg;
      mcm_error_t_v01 mcm_error;

      get_pptp_vpn_pt_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_PPTP_VPN_PASS_THROUGH_REQ_V01,
                                           &get_pptp_vpn_pt_req_msg,
                                           &get_pptp_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_pptp_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_pptp_vpn_pt_resp_msg.resp.error = MCM_SUCCESS_V01))
      {
        LOG_MSG_ERROR("Can not get pptp vpn passthrough %d : %d",
                       mcm_error, get_pptp_vpn_pt_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get pptp vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        if (get_pptp_vpn_pt_resp_msg.vpn_pass_through_value_valid)
        {
          LOG_MSG_ERROR("PPTP Vpn Passthrough Get succeeded...%d",
                        get_pptp_vpn_pt_resp_msg.vpn_pass_through_value,0,0);
          if(get_pptp_vpn_pt_resp_msg.vpn_pass_through_value == 1)
          {
            printf("PPTP VPN PassThrough Flag : SET\n");
          }
          else if(get_pptp_vpn_pt_resp_msg.vpn_pass_through_value == 0)
          {
            printf("PPTP VPN PassThrough Flag : UNSET\n");
          }
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP get pptp vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=============================================================================================*/

    case 32:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_v01 get_l2tp_vpn_pt_req_msg;
      mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01 get_l2tp_vpn_pt_resp_msg;
      mcm_error_t_v01 mcm_error;

     MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_l2tp_vpn_pt_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_L2TP_VPN_PASS_THROUGH_REQ_V01,
                                           &get_l2tp_vpn_pt_req_msg,
                                           &get_l2tp_vpn_pt_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_l2tp_vpn_pt_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) )
      {
        LOG_MSG_ERROR("Can not get l2tp vpn passthrough %d : %d",
                      mcm_error, get_l2tp_vpn_pt_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get l2tp vpn pass through failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        if (get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value_valid)
        {
          LOG_MSG_INFO1("L2TP Vpn Passthrough Get succeeded...%d",
                        get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value,0,0);
          if(get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value == 1)
          {
            printf("L2TP VPN PassThrough Flag : SET\n");
          }
          else if(get_l2tp_vpn_pt_resp_msg.vpn_pass_through_value == 0)
          {
            printf("L2TP VPN PassThrough Flag : UNSET\n");
          }
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP get l2tp vpn pass through successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=============================================================================================*/

    case 33:
    {
      mcm_mobileap_set_auto_connect_req_msg_v01 set_auto_connect_req_msg;
      mcm_mobileap_set_auto_connect_resp_msg_v01 set_auto_connect_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      while (TRUE)
      {
        printf(" Please input status of Autoconnect: (1-Enable, 0-Disable) \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input= atoi(scan_string);
        if(( tmp_input == 0 ) || ( tmp_input == 1 ))
          break;
        printf("Invalid Input.     Please input valid status\n");
      }

      set_auto_connect_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      set_auto_connect_req_msg.enable = tmp_input;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_AUTO_CONNECT_REQ_V01,
                                           &set_auto_connect_req_msg,
                                           &set_auto_connect_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( set_auto_connect_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( set_auto_connect_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set auto connect flag %d : %d",
                      mcm_error, set_auto_connect_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set autoconnect failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Auto Connect Mode Set succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set autoconnect successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*============================================================================================*/


    case 34:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      while (TRUE)
      {
        printf(" Please input status of Roaming: (1-Enable, 0-Disable) \n");
        fgets(scan_string, sizeof(scan_string), stdin);
        tmp_input = atoi(scan_string);
        if(( tmp_input == 0 ) || ( tmp_input == 1 ))
          break;
        printf("Invalid Input.   Please input valid status\n");
      }
      mcm_mobileap_set_roaming_pref_req_msg_v01 set_roaming_req_msg;
      mcm_mobileap_set_roaming_pref_resp_msg_v01 set_roaming_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      set_roaming_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
      set_roaming_req_msg.allow_wwan_calls_while_roaming = tmp_input;
      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_ROAMING_PREF_REQ_V01,
                                           &set_roaming_req_msg,
                                           &set_roaming_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( set_roaming_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( set_roaming_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set roaming flag %d : %d",
                      mcm_error, set_roaming_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set roaming failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Roaming is Set succesfully...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set roaming successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*=============================================================================================*/

    case 35:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_get_auto_connect_req_msg_v01 get_auto_connect_req_msg;
      mcm_mobileap_get_auto_connect_resp_msg_v01 get_auto_connect_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_auto_connect_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_AUTO_CONNECT_REQ_V01,
                                           &get_auto_connect_req_msg,
                                           &get_auto_connect_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_auto_connect_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_auto_connect_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not get Autoconnect mode flag %d : %d",
           mcm_error, get_auto_connect_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get autoconnect failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Get Autoconnect succeeded...%d",get_auto_connect_resp_msg.auto_conn_flag,0,0);
        if(get_auto_connect_resp_msg.auto_conn_flag == 1)
        {
          printf("Autoconnect Flag : SET\n");
        }
        else if(get_auto_connect_resp_msg.auto_conn_flag == 0)
        {
          printf("Autoconnect Flag : UNSET\n");
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP get autoconnect successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*==============================================================================================*/

    case 36:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_mobileap_get_roaming_pref_req_msg_v01 get_roaming_req_msg;
      mcm_mobileap_get_roaming_pref_resp_msg_v01 get_roaming_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      get_roaming_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_GET_ROAMING_PREF_REQ_V01,
                                           &get_roaming_req_msg,
                                           &get_roaming_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( get_roaming_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( get_roaming_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set auto roaming flag %d : %d",
                      mcm_error, get_roaming_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP get roaming failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
      else
      {
        if(get_roaming_resp_msg.allow_wwan_calls_while_roaming_valid)
        {
          LOG_MSG_INFO1("Get Roaming succeeded...%d",get_roaming_resp_msg.allow_wwan_calls_while_roaming,0,0);
          if(get_roaming_resp_msg.allow_wwan_calls_while_roaming == 1)
          {
            printf("Roaming Flag : SET\n");
          }
          else if(get_roaming_resp_msg.allow_wwan_calls_while_roaming == 0)
          {
            printf("Roaming Flag : UNSET\n");
          }
        }
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP get roaming successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*===============================================================================================*/

    case 37:
    {
        LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_set_dualap_config_req_msg_v01 set_dualap_config_req_msg;
      mcm_mobileap_set_dualap_config_resp_msg_v01 set_dualap_config_resp_msg;
      mcm_error_t_v01 mcm_error;
      set_dualap_config_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      struct in_addr a5_ip_addr, sub_net_mask;
#ifdef TARGET_IS_9615
      printf("   Please input WLAN State : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      uint32_t wlan_state = atoi(scan_string);
      if (wlan_state)
      {
        while (TRUE)
        {
          printf("   Please input interface IP address : ");
          if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
          {
            if (inet_aton(scan_string, &a5_ip_addr))
              break;
          }
          printf("      Invalid IPv4 address %d\n", scan_string);
        }
        while (TRUE)
        {
          printf("   Please input interface IP subnet  : ");
          if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
          {
           if (inet_aton(scan_string, &sub_net_mask))
              break;
          }
          printf("      Invalid IPv4 address %d\n", scan_string);
        }
      }

      set_dualap_config_req_msg.dualap_config.a5_ip_address = a5_ip_addr.s_addr;
      set_dualap_config_req_msg.dualap_config.sub_net_mask = sub_net_mask.s_addr;
      if(wlan_state > 0)
        set_dualap_config_req_msg.dualap_config.enable = TRUE; //Enable
      else
        set_dualap_config_req_msg.dualap_config.enable = FALSE; //Disable

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_SET_DUALAP_CONFIG_REQ_V01,
                                           &set_dualap_config_req_msg,
                                           &set_dualap_config_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( set_dualap_config_resp_msg.resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( set_dualap_config_resp_msg.resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not set dualap config %d : %d",
                      mcm_error, set_dualap_config_resp_msg.resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP set dualap config failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Set Dualap config succeeded...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP set dualap config successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
#else /* TARGET_IS_9615 */
      LOG_MSG_INFO1(" Dual AP Config is not supported for this target \n", 0, 0, 0);
#endif /* !(TARGET_IS_9615) */
    }
    break;

/*===========================================================================================*/

    case 38:
    {
      mcm_error_t_v01 mcm_error;
      mcm_mobileap_station_mode_enable_req_msg_v01  station_mode_enable_req_msg;
      struct in_addr static_ip_addr, netmask, gw_addr, dns_addr;

      mcm_mobileap_station_mode_enable_resp_msg = (mcm_mobileap_station_mode_enable_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));
      if ( mcm_mobileap_station_mode_enable_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&station_mode_enable_req_msg, 0, sizeof(mcm_mobileap_station_mode_enable_req_msg_v01));
      memset(mcm_mobileap_station_mode_enable_resp_msg, 0, sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01));

      station_mode_enable_req_msg.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;
#ifdef TARGET_IS_9615
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      printf("     Please input STA State : ");
      fgets(scan_string, sizeof(scan_string), stdin);
      if (atoi(scan_string))
      {
#endif /* TARGET_IS_9615 */
        printf("   Please input: 1 for New Config/else for Default Config: ");
        fgets(scan_string, sizeof(scan_string), stdin);
        if (atoi(scan_string) == 1)
        {
          struct in_addr static_ip_addr, netmask, gw_addr, dns_addr;
          printf("     Please input Connection Type, 0 for DYNAMIC/1 for STATIC:");
          fgets(scan_string, sizeof(scan_string), stdin);
          if (atoi(scan_string) == 1)
          {
            while (TRUE)
            {
              printf("     Please input a valid Static IP address:");
              if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
              {
                if (inet_aton(scan_string, &static_ip_addr))
                  break;
              }
              printf("        Invalid IPv4 address %s", scan_string);
            }
            while (TRUE)
            {
              printf("     Please input a valid Gateway address:");
              if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
              {
                if (inet_aton(scan_string, &gw_addr))
                  break;
              }
              printf("        Invalid IPv4 address %s", scan_string);
            }
            while (TRUE)
            {
              printf("     Please input a valid Netmask:");
              if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
              {
                if (inet_aton(scan_string, &netmask))
                  break;
              }
              printf("        Invalid IPv4 address %s", scan_string);
            }
            while (TRUE)
            {
              printf("     Please input a valid DNS Address:");
              if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
              {
                if (inet_aton(scan_string, &dns_addr))
                  break;
              }
              printf("        Invalid IPv4 address %s", scan_string);
            }
            station_mode_enable_req_msg.cfg.static_ip_config.ip_addr = ntohl(static_ip_addr.s_addr);
            station_mode_enable_req_msg.cfg.static_ip_config.gw_ip = ntohl(gw_addr.s_addr);
            station_mode_enable_req_msg.cfg.static_ip_config.netmask = ntohl(netmask.s_addr);
            station_mode_enable_req_msg.cfg.static_ip_config.dns_addr = ntohl(dns_addr.s_addr);
            station_mode_enable_req_msg.cfg.conn_type = MCM_MOBILEAP_STA_CONNECTION_STATIC_V01;
          }
          else
          {
            station_mode_enable_req_msg.cfg.conn_type = MCM_MOBILEAP_STA_CONNECTION_DYNAMIC_V01;
          }
        }
#ifdef TARGET_IS_9615
      }
#endif /* TARGET_IS_9615 */
      LOG_MSG_INFO1("Enabling STATION Mode",0,0,0);

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_STATION_MODE_ENABLE_REQ_V01,
                                            &station_mode_enable_req_msg,
                                            mcm_mobileap_station_mode_enable_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_station_mode_enable_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_station_mode_enable_resp_msg is NULL", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable sta mode failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
          ( mcm_mobileap_station_mode_enable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
          ( mcm_mobileap_station_mode_enable_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not enable Sta Mode %d : %d\n",
                      mcm_error, mcm_mobileap_station_mode_enable_resp_msg->resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP enable sta mode failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Enable Sta Mode Request sent\n",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Posted Enable STATION Mode to server  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*===================================================================================*/

    case 39:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      mcm_error_t_v01 mcm_error;
      mcm_mobileap_station_mode_disable_req_msg_v01 station_mode_disable_req;

      mcm_mobileap_station_mode_disable_resp_msg = (mcm_mobileap_station_mode_disable_resp_msg_v01 *)malloc(sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));
      if ( mcm_mobileap_station_mode_disable_resp_msg == NULL )
      {
        printf("\nCannot Allocate Memory\n");
        break;
      }

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&station_mode_disable_req, 0, sizeof(mcm_mobileap_station_mode_disable_req_msg_v01));
      memset(mcm_mobileap_station_mode_disable_resp_msg, 0, sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01));

      LOG_MSG_INFO1("Disabling STATION Mode",0,0,0);

      station_mode_disable_req.mcm_mobileap_handle = mcm_mobileap_config.mcm_mobileap_handle;

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_mobileap_config.mcm_client_handle,
                                            MCM_MOBILEAP_STATION_MODE_DISABLE_REQ_V01,
                                            &station_mode_disable_req,
                                            mcm_mobileap_station_mode_disable_resp_msg,
                                            (mcm_client_async_cb)mcm_mobileap_async_cb,
                                            &token_id);
      if(NULL == mcm_mobileap_station_mode_disable_resp_msg)
      {
        LOG_MSG_ERROR("mcm_mobileap_station_mode_disable_resp_msg is NULL", 0, 0, 0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable sta mode failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        break;
      }

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_station_mode_disable_resp_msg->resp.result != MCM_RESULT_SUCCESS_V01 ) ||
           ( mcm_mobileap_station_mode_disable_resp_msg->resp.error != MCM_SUCCESS_V01 ))
      {
        LOG_MSG_ERROR("Can not disable Sta Mode %d : %d",
                      mcm_error, mcm_mobileap_station_mode_disable_resp_msg->resp.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  MobileAP disable sta mode failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Disabled Sta Mode successfully",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Posted Disable STATION Mode to server  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*==========================================================================================*/

    case 40:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);

      mcm_mobileap_event_register_req_msg_v01 mcm_mobileap_event_register_req_msg;
      mcm_mobileap_event_register_resp_msg_v01 mcm_mobileap_event_register_resp_msg;
      mcm_error_t_v01 mcm_error;

      MCM_MOBILEAP_LOG_FUNC_ENTRY();

      memset(&mcm_mobileap_event_register_req_msg, 0, sizeof(mcm_mobileap_event_register_req_msg_v01));
      memset(&mcm_mobileap_event_register_resp_msg, 0, sizeof(mcm_mobileap_event_register_resp_msg_v01));

      printf("Register for Enable event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_enabled_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_enabled= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_enabled_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_enabled= FALSE;
        }
      }
#ifdef TARGET_IS_9615
      printf("Register for Lan Connecting event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting= FALSE;
        }
      }

      printf("Register for Lan Connecting Fail event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_fail_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_fail= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_fail_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connecting_fail= FALSE;
        }
      }

      printf("Register for Lan IPv6 Connecting Fail event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connecting_fail_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connecting_fail= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connecting_fail_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connecting_fail= FALSE;
        }
      }

      printf("Register for Lan Connected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_connected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_connected= FALSE;
        }
      }
#endif /* TARGET_IS_9615 */
      printf("Register for Sta Connected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_sta_connected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_sta_connected= TRUE;
        }
        else
       {
         mcm_mobileap_event_register_req_msg.register_event_sta_connected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_sta_connected= FALSE;
        }
      }
#ifdef TARGET_IS_9615
      printf("Register for Lan IPv6 Connected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_connected= FALSE;
        }
      }
#endif /* TARGET_IS_9615 */
      printf("Register for Wan Connecting event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting= FALSE;
        }
      }

      printf("Register for Wan Connecting Fail event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_fail_valid = TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_fail = TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_fail_valid = FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connecting_fail = FALSE;
        }
      }

      printf("Register for Wan IPv6 Connecting Fail event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connecting_fail_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connecting_fail= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connecting_fail_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connecting_fail= FALSE;
        }
      }

      printf("Register for Wan Connected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_connected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_connected= FALSE;
        }
      }


      printf("Register for Wan IPv6 Connected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_connected= FALSE;
        }
      }

      printf("Register for Wan Disconnected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_disconnected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_disconnected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_disconnected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_disconnected= FALSE;
        }
      }

      printf("Register for Wan IPv6 Disconnected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_disconnected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_disconnected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_disconnected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_wan_ipv6_disconnected= FALSE;
        }
      }
#ifdef TARGET_IS_9615
      printf("Register for Lan Disconnected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_disconnected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_disconnected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_disconnected_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_lan_disconnected= FALSE;
        }
      }

      printf("Register for Lan IPv6 Disconnected event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_disconnected_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_disconnected= TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_disconnected_valid= FALSE;
           mcm_mobileap_event_register_req_msg.register_event_lan_ipv6_disconnected= FALSE;
        }
      }
#endif /* TARGET_IS_9615 */
      printf("Register for Disable event (0 : no , 1 : yes) : ");
      if (fgets(scan_string, sizeof(scan_string), stdin) != NULL)
      {
        if ( scan_string[0] == '1' )
        {
          mcm_mobileap_event_register_req_msg.register_event_disabled_valid= TRUE;
          mcm_mobileap_event_register_req_msg.register_event_disabled = TRUE;
        }
        else
        {
          mcm_mobileap_event_register_req_msg.register_event_disabled_valid= FALSE;
          mcm_mobileap_event_register_req_msg.register_event_disabled = FALSE;
        }
      }

      mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC_EX(mcm_mobileap_config.mcm_client_handle,
                                           MCM_MOBILEAP_EVENT_REGISTER_REQ_V01,
                                           &mcm_mobileap_event_register_req_msg,
                                           &mcm_mobileap_event_register_resp_msg,
                                           MCM_MOBILEAP_MSG_TIMEOUT_VALUE_V01);

      if (( mcm_error != MCM_SUCCESS_V01 ) ||
           ( mcm_mobileap_event_register_resp_msg.response.result != MCM_SUCCESS_V01) )
      {
        LOG_MSG_ERROR("Can not register for events %d : %d",
                      mcm_error, mcm_mobileap_event_register_resp_msg.response.error,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP Event Register failed  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }

      else
      {
        LOG_MSG_INFO1("Event Registration Successful...",0,0,0);
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MobileAP Event Register successful  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
        printf("//////////////////////////////////////////////////////////////////////////////////////////////////////\n");
      }
    }

    break;

/*==================================================================================================*/

    case 41:
    {
      LOG_MSG_INFO1("Entered case: %d:", opt, 0,0);
      return 0;
    }
    break;

/*==================================================================================================*/

        default:
      break;
    }
  }
}
