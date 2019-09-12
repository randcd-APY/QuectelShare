/******************************************************************************

                      M C M _ D A T A _ T E S T . C

******************************************************************************/

/******************************************************************************

  @file    mcm_data_test.c
  @brief   mcm_data test

  DESCRIPTION
  Interactive test that allows making data calls using mcm_data

  ---------------------------------------------------------------------------
  Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id: $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/07/13   ta          added more api support
06/26/13   vb         created

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ds_string.h"
#include <pthread.h>
#include <arpa/inet.h>
#include "mcm_data_v01.h"
#include "mcm_client.h"
#include "mcm_client_v01.h"

#define TRUE 1
#define FALSE 0

#define SERVICE_REQUIRED 1
#define SERVICE_NOT_REQUIRED 0

#define MCM_DATA_SUCCESS 0
#define MCM_DATA_ERROR -1

#define MCM_DATA_TEST_DEVICE_NAME_LEN 255

#define MCM_DATA_TEST_LOG_ERROR(...) \
printf("Error: ",__VA_ARGS__); \
printf("\n")
#define MCM_DATA_TEST_LOG(...) \
printf( __VA_ARGS__); \
printf("\n")
#define MCM_DATA_TEST_LOG_FUNC_ENTRY() \
MCM_DATA_TEST_LOG("\n%s: ENTRY", __FUNCTION__);
#define MCM_DATA_TEST_LOG_FUNC_EXIT() \
MCM_DATA_TEST_LOG("\n%s: EXIT", __FUNCTION__);

#define BAILOUT MCM_DATA_TEST_LOG("Bailed out at %s:%d ", \
                                          __FILE__, __LINE__, 0); \
                goto BAIL

#define IPV4_FAM 4
#define IPV6_FAM 6
#define IPV4V6_FAM 10
#define UMTS_TECH 1
#define CDMA_TECH 0
#define MAX_DATA_CALLS     20
#define INVALID_CALL_ID -1

#define MCM_DATA_TEST_MODE_MENU 0
#define MCM_DATA_TEST_MODE_CLI 1

#define MAX_UNSOL_EVENTS 7
char mcm_data_test_mode;

typedef  unsigned char      boolean;     /* Boolean value type. */

/* All the test APIs can only be used after registering as
   a client of mcm_data service. mcm_data_inited will be
   set to TRUE only after that registration has succeeded */
boolean mcm_data_inited = FALSE;

typedef struct mcm_data_call_config_s {
  uint32_t ip_family;                          ///< IP family requested
  char apn_name[MCM_DATA_MAX_APN_LEN_V01];       ///< APN name requested
  char user_name[MCM_DATA_MAX_USERNAME_LEN_V01]; ///< Username for APN
  char password[MCM_DATA_MAX_PASSWORD_LEN_V01];  ///< Password for APN
  uint32_t tech_pref;                          ///< Technology preference
                                             ///< (3GPP / 3GPP2)
  uint32_t profile;                            ///< Profile call_id (optional)
} mcm_data_call_config;

typedef struct {
  mcm_client_handle_type         mcm_client_handle;
} mcm_data_conf_t;

mcm_data_conf_t mcm_data_config;

char *mcm_data_test_tech_map[30] = {"UNKNOWN", "1X","REV0", "REVA", "REVB",
                                    "EHRPD", "FMC", "HRPD", "3GPP2_WLAN",
                                    "WCDMA", "GPRS", "HSDPA", "HSUPA", "EDGE",
                                    "LTE", "HSDPA_PLUS", "DC_HSDPA_PLUS",
                                    "HSPA", "64_QAM", "TDSCDMA", "GSM",
                                    "3GPP_WLAN"};

typedef struct mcm_data_addr_s {
  char valid_addr;              ///< indicates if a valid address is
                                ///< available or not
  struct sockaddr_storage addr; ///< stores ip address
} mcm_data_addr;

/// Data type to store IP network addresses information
/// @ingroup mcm_data_api
typedef struct mcm_data_addr_info_s {
  mcm_data_addr iface_addr_s;    ///< Network interface address structure
  unsigned int iface_mask;         ///< Network interface subnet mask

  mcm_data_addr gtwy_addr_s;     ///< Gateway address structure
  unsigned int gtwy_mask;          ///< Gateway address prefix length

  mcm_data_addr dnsp_addr_s;     ///< Primary DNS address structure
  mcm_data_addr dnss_addr_s;     ///< Secondary DNS address structure
} mcm_data_addr_info;


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

mcm_data_call_config mcm_data_test_config;
uint32_t mcm_data_test_call_tech;
uint32_t mcm_data_test_req_call_cnt;
uint32_t mcm_data_test_rsp_call_cnt;

int32_t call_id_list[MAX_DATA_CALLS];

mcm_data_pkt_stats_t_v01 mcm_data_test_pkt_stats;
mcm_data_channel_rate_t_v01 *mcm_data_test_channel_rate;

int token_id=0;

/*--------------------------  START - Function Declarations --------------------------*/

char mcm_data_test_input
(
  void
);

int mcm_data_srv_mgr_service_handle
(
  int    handle,
  int    require_flag,
  uint16_t service_list
);

int mcm_data_test_input_string
(
  char * buff,
  unsigned int len
);

uint8_t mcm_data_test_is_num
(
  char *word
);

int32_t mcm_data_test_init
(
  void
);

void mcm_data_test_set_call_configs
(
  void
);

void mcm_data_test_set_IP_family
(
  void
);

void mcm_data_test_set_tech_pref
(
  void
);

void mcm_data_test_set_profile
(
  void
);

void mcm_data_test_set_APN(
  void
);

void mcm_data_test_set_username
(
  void
);

void mcm_data_test_set_password
(
  void
);

void mcm_data_test_display_call_config
(
  void
);

int32_t mcm_data_test_request_start_data_call
(
  void
);

int32_t mcm_data_test_get_device_address
(
  uint32_t *callid_p
);

int32_t mcm_data_test_get_device_address_count
(
  uint32_t *callid_p
);

void mcm_data_test_display_pkt_stats
(
  mcm_data_pkt_stats_t_v01 *mcm_data_test_pkt_stats
);

void mcm_data_test_display_channel_rate
(
  mcm_data_channel_rate_t_v01 *mcm_data_test_channel_rate
);

uint32_t mcm_data_test_add_route
(
  void
);

void list_active_calls
(
  void
);

int active_call_count
(
  void
);

void mcm_data_test_app_init
(
  void
);

void mcm_data_test_main_menu
(
  void
);

int execute_cli_args
(
  int  argc,
  char **argv
);

static void mcm_data_test_cli_ind_cb
(
  mcm_client_handle_type hndl,         /* QMI user handle       */
  uint32_t                 msg_id,       /* Indicator message ID  */
  void                   *ind_data,    /* Raw indication data   */
  unsigned int           ind_buf_len   /* Raw data length       */
);

static void mcm_data_test_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_c_struct_len,
  void                  *token_id
);

uint32_t mcm_data_test_event_register
(
  boolean *events
);

/*--------------------------  END - Function Declarations --------------------------*/

int mcm_data_test_system_call
(
  char *cmd,
  char *args[]
)
{
  int childExitStatus = MCM_DATA_ERROR;
  int i;
  MCM_DATA_TEST_LOG("Executing %s with arguments-", cmd);
  for(i=0;args[i]!=NULL;i++)
  {
    MCM_DATA_TEST_LOG("arg[%d]=%s", i, args[i]);
  }
  pid_t pid = fork();
  if ( pid == 0 )
  {
    execvp(cmd, args);
  }
  waitpid(pid, &childExitStatus, 0);
  return childExitStatus;
}

char mcm_data_test_input
(
  void
)
{
  char ch;
  while((ch = getchar()) == '\n');
  return ch;
}

uint8_t mcm_data_test_is_num
(
  char *word
)
{
  int i=0;
  for(;i<strlen(word);i++)
    if(word[i]<'0' || word[i]>'9')
      return 0;
  return 1;
}

int32_t mcm_data_test_list_and_get_call_id
(
  void
)
{
  uint32_t call_id, i;

  MCM_DATA_TEST_LOG("\n=== Active Call Ids ===\n");
  for(i=0; i< MAX_DATA_CALLS; i++)
  {
    if(call_id_list[i] != INVALID_CALL_ID)
    {
      MCM_DATA_TEST_LOG(" %d",call_id_list[i]);
    }
  }
  MCM_DATA_TEST_LOG("\n=======================\n");

  MCM_DATA_TEST_LOG("Enter : ");
  scanf("%d",&call_id);

  for(i=0;i< MAX_DATA_CALLS; i++)
  {
    if(call_id_list[i] == call_id)
      break;
  }
  if(i == MAX_DATA_CALLS)
  {
    MCM_DATA_TEST_LOG("Invalid Input");
    return INVALID_CALL_ID;
  }

  return call_id;
}

int mcm_data_test_input_string( char * buff, unsigned int len )
{
  buff[0]='\0';
  if(len <= 0)
    return 0;

  char ch;
  int i=0;

  while(scanf("%c",&ch)!=0 && ch=='\n');
  do
  {
    buff[i++] = ch;
  } while(scanf("%c",&ch)!=0 && ch!='\n' && i<len-1);
  buff[i]='\0';
  if(ch!='\n')
  {
    while(((ch = getchar()) != '\n') && (ch != EOF));
  }
  return i;
}

int mcm_data_srv_mgr_service_handle
(
  int    handle,
  int    require_flag,
  uint16_t service_list
)
{
  int ret_val = MCM_ERROR_GENERIC_V01;

  mcm_client_require_req_msg_v01       *req_msg;
  mcm_client_require_resp_msg_v01      *resp_msg;

  mcm_client_not_require_req_msg_v01   *not_req_msg;
  mcm_client_not_require_resp_msg_v01  *not_resp_msg;

  /* If request is to dynamically load services */
  if (SERVICE_REQUIRED == require_flag)
  {
    req_msg  = (mcm_client_require_req_msg_v01 *)mcm_util_memory_alloc(
                                    sizeof(mcm_client_require_req_msg_v01));
    resp_msg = (mcm_client_require_resp_msg_v01 *)mcm_util_memory_alloc(
                                    sizeof(mcm_client_require_resp_msg_v01));
    if (( req_msg == NULL ) ||
        ( resp_msg == NULL ))
    {
      MCM_DATA_TEST_LOG("\nCannot Allocate Momory");
      free(req_msg? req_msg: resp_msg);
      return ret_val;
    }
    memset(req_msg, 0, sizeof(mcm_client_require_req_msg_v01));
    memset(resp_msg, 0, sizeof(mcm_client_require_resp_msg_v01));

    req_msg->require_service = service_list;
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                        MCM_CLIENT_REQUIRE_REQ_V01,
                        req_msg,
                        resp_msg,
                        (mcm_client_async_cb)mcm_data_test_async_cb,
                        &token_id);

    MCM_DATA_TEST_LOG("mcm_client 'require service' request returned [%d],"
                      " resp code - %d, error code - %d\n",
                      ret_val,
                      resp_msg->response.result,
                      resp_msg->response.error);
  }
  /* If request is to dynamically UNLOAD services */
  else if ( SERVICE_NOT_REQUIRED == require_flag )
  {
    not_req_msg  = (mcm_client_not_require_req_msg_v01 *)mcm_util_memory_alloc(
                                    sizeof(mcm_client_not_require_req_msg_v01));
    not_resp_msg = (mcm_client_not_require_resp_msg_v01 *)mcm_util_memory_alloc(
                                   sizeof(mcm_client_not_require_resp_msg_v01));

    if (( not_req_msg == NULL ) ||
        ( not_resp_msg == NULL ))
    {
      MCM_DATA_TEST_LOG("\nCannot Allocate Momory");
      free(not_req_msg? not_req_msg: not_resp_msg);
      return ret_val;
    }

    memset(not_req_msg, 0, sizeof(mcm_client_not_require_req_msg_v01));
    memset(not_resp_msg, 0, sizeof(mcm_client_not_require_resp_msg_v01));

    not_req_msg->not_require_service = service_list;
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                        MCM_CLIENT_NOT_REQUIRE_REQ_V01,
                        not_req_msg,
                        not_resp_msg,
                        (mcm_client_async_cb)mcm_data_test_async_cb,
                        &token_id);

    MCM_DATA_TEST_LOG("mcm_client 'service not required' request returned [%d],"
                      " resp code - %d, error code - %d\n",
                      ret_val,
                      not_resp_msg->response.result,
                      not_resp_msg->response.error);
  }
  else
  {
    MCM_DATA_TEST_LOG("Unknown require flag set");
  }

  return ret_val;

}

static void mcm_data_test_cli_ind_cb
(
  mcm_client_handle_type hndl,         /* QMI user handle       */
  uint32_t                 msg_id,       /* Indicator message ID  */
  void                   *ind_data,    /* Raw indication data   */
  unsigned int           ind_buf_len   /* Raw data length       */
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  mcm_data_unsol_event_ind_msg_v01 *ind_msg = (mcm_data_unsol_event_ind_msg_v01 *)ind_data;
  uint32_t i;

  switch (ind_msg->event_id)
  {
    case MCM_DATA_NET_UP_EV_V01:
      MCM_DATA_TEST_LOG("Received Net Up Event");
      break;
    case MCM_DATA_NET_DOWN_EV_V01:
      MCM_DATA_TEST_LOG("Received Net Down Event");
      if(ind_msg->call_id_valid)
      {
        for(i=0;i<MAX_DATA_CALLS;i++)
        {
          if(call_id_list[i] == ind_msg->call_id)
          {
            call_id_list[i] = INVALID_CALL_ID;
            MCM_DATA_TEST_LOG("Call teared down by server..Cleared call table index @ %d",i);
          }
        }
      }
      break;
    case MCM_DATA_NET_NEW_ADDR_EV_V01:
      MCM_DATA_TEST_LOG("Received Address Added Event");
      break;
    case MCM_DATA_NET_DEL_ADDR_EV_V01:
      MCM_DATA_TEST_LOG("Recieved Address Deleted Event");
      break;
    case MCM_DATA_REG_SRVC_STATUS_EV_V01:
      if ( !ind_msg->reg_status_valid )
      {
        MCM_DATA_TEST_LOG("Received INVALID Modem Service Status");
      }
      else if ( ind_msg->reg_status.srv_status == MCM_DATA_MODEM_STATE_IN_SERVICE_V01 )
      {
        MCM_DATA_TEST_LOG("\nModem is IN Service");
        MCM_DATA_TEST_LOG("\nPreferred tech type is %d [%s]",
                        ind_msg->reg_status.tech_info,
                        mcm_data_test_tech_map[ind_msg->reg_status.tech_info]);
      }
      else if ( ind_msg->reg_status.srv_status == MCM_DATA_MODEM_STATE_OOS_V01 )
      {
        MCM_DATA_TEST_LOG("\nModem is OUT of Service");
      }
      else
      {
        MCM_DATA_TEST_LOG("Received INVALID Modem Service Status");
      }
      break;
    case MCM_DATA_BEARER_TECH_STATUS_EV_V01:
      MCM_DATA_TEST_LOG("Received Bearer Tech Status Event");
      break;
    case MCM_DATA_DORMANCY_STATUS_EV_V01:
      MCM_DATA_TEST_LOG("Received Dormancy Status Event");
      break;
    default:
      MCM_DATA_TEST_LOG("Received UNKNOWN Event");
      break;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
}

static void mcm_data_test_async_cb
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_c_struct_len,
  void                  *token_id
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  uint32_t i;
  mcm_data_start_data_call_rsp_msg_v01 *start_call_rsp_msg;
  mcm_data_stop_data_call_rsp_msg_v01  *stop_call_rsp_msg;
  mcm_client_require_resp_msg_v01      *service_require_rsp_msg;
  mcm_client_not_require_resp_msg_v01  *service_not_required_rsp_msg;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  boolean events[MAX_UNSOL_EVENTS + 1];

  for (i=1;i<MAX_UNSOL_EVENTS + 1;i++)
    events[i]=1;

  switch(msg_id)
  {
    case MCM_DATA_START_DATA_CALL_REQ_V01:
      start_call_rsp_msg = (mcm_data_start_data_call_rsp_msg_v01 *)resp_c_struct;
      MCM_DATA_TEST_LOG("Start Data Call Async Rsp: call_id %d, "
                        "call_id_valid %d, vce_valid %d",
                        start_call_rsp_msg->call_id,
                        start_call_rsp_msg->call_id_valid,
                        start_call_rsp_msg->vce_reason_valid);
      if(start_call_rsp_msg->vce_reason_valid == TRUE)
      {
        MCM_DATA_TEST_LOG("Call End Reason Type %d", start_call_rsp_msg->vce_reason.call_end_reason_type);
        MCM_DATA_TEST_LOG("Call End Reason Code %d", start_call_rsp_msg->vce_reason.call_end_reason_code);
      }
      if (start_call_rsp_msg->call_id_valid)
      {
        for (i=0;i<MAX_DATA_CALLS;i++)
        {
          if(call_id_list[i] == INVALID_CALL_ID)
          {
            MCM_DATA_TEST_LOG("Storing call_id @ index %d", i);
            call_id_list[i] = start_call_rsp_msg->call_id;
            break;
          }
        }
      }
      if(mcm_data_test_mode == MCM_DATA_TEST_MODE_CLI)
      {
        mcm_data_test_rsp_call_cnt++;
        if (mcm_data_test_req_call_cnt == mcm_data_test_rsp_call_cnt)
        {
          pthread_mutex_lock(&mutex);
          pthread_cond_signal(&cond);
          pthread_mutex_unlock(&mutex);
        }
      }

      free(start_call_rsp_msg);
      start_call_rsp_msg = NULL;
      break;
    case MCM_DATA_STOP_DATA_CALL_REQ_V01:
      stop_call_rsp_msg = (mcm_data_stop_data_call_rsp_msg_v01 *)resp_c_struct;
      MCM_DATA_TEST_LOG("Stop Data Call Async Rsp");

      free(stop_call_rsp_msg);
      stop_call_rsp_msg = NULL;
      break;
    case MCM_CLIENT_REQUIRE_REQ_V01:
      service_require_rsp_msg = ( mcm_client_require_resp_msg_v01 *)resp_c_struct;
      /* Requested services came up successfully */
      if (service_require_rsp_msg->response.result==MCM_RESULT_SUCCESS_V01)
      {
        /* Update mcm_data_inited so that all test APIs can be unblocked */
        mcm_data_inited = TRUE;

        /* Register for all indications from mcm_data service */
        mcm_data_test_event_register(events);
        MCM_DATA_TEST_LOG("Successfully loaded required services");
      }
      else
      {
        MCM_DATA_TEST_LOG("FAILED loaded required services");
      }

      free(service_require_rsp_msg);
      service_require_rsp_msg = NULL;
      break;
    case MCM_CLIENT_NOT_REQUIRE_REQ_V01:
      service_not_required_rsp_msg =
                    ( mcm_client_not_require_resp_msg_v01 *)resp_c_struct;
      /* Requested services were brought down successfully */
      if (service_not_required_rsp_msg->response.result==MCM_RESULT_SUCCESS_V01)
      {
        MCM_DATA_TEST_LOG("Successfully unloaded services. Releasing Client");
      }
      else
      {
        MCM_DATA_TEST_LOG("Failed to unloaded services!! Releasing Client");
      }
      mcm_error = mcm_client_release(mcm_data_config.mcm_client_handle);
      if (mcm_error != MCM_SUCCESS_V01)
      {
        MCM_DATA_TEST_LOG("mcm_client_release: Failed with error %d",mcm_error);
      }
      /* mcm client released successfully, reset default values */
      else
      {
        mcm_data_test_app_init();
        mcm_data_config.mcm_client_handle = 0;
        MCM_DATA_TEST_LOG("mcm_client_release: Success");
      }

      free(service_not_required_rsp_msg);
      service_not_required_rsp_msg = NULL;
      break;
    default :
      MCM_DATA_TEST_LOG("Unknown Async Message : %d", msg_id, 0, 0);
      break;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
}

int32_t mcm_data_test_init
(
  void
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  mcm_error_t_v01 mcm_error;
  uint16_t service_list = MCM_DATA_V01;

  if(mcm_data_config.mcm_client_handle != 0)
  {
    MCM_DATA_TEST_LOG("Client already initialized successfully 0x%x",
                      &mcm_data_config.mcm_client_handle,0,0);
    return MCM_DATA_SUCCESS;
  }

  mcm_error = mcm_client_init(&mcm_data_config.mcm_client_handle,
                              mcm_data_test_cli_ind_cb,
                              mcm_data_test_async_cb);

  /* Continue only if client initialized for atleast one mcm service */
  if ((mcm_error != MCM_SUCCESS_V01) && (mcm_error != MCM_SUCCESS_CONDITIONAL_SUCCESS_V01))
  {
    MCM_DATA_TEST_LOG("mcm_client_init: Failed with error %d", mcm_error);
    return MCM_DATA_ERROR;
  }
  else
  {
    MCM_DATA_TEST_LOG("mcm_client_init: Succeded with client_handle 0x%x",
                      mcm_data_config.mcm_client_handle);

    /* Do dynamic service loading of mcm_data */
    mcm_error = mcm_data_srv_mgr_service_handle(
                               mcm_data_config.mcm_client_handle,
                               SERVICE_REQUIRED,
                               service_list);
    if ( mcm_error != MCM_SUCCESS_V01 )
    {
      MCM_DATA_TEST_LOG("Failed to request loading of required services [%d]",
                        mcm_error);
    }
    else
    {
      MCM_DATA_TEST_LOG("Successfully requested loading of required services");
    }
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
  return MCM_DATA_SUCCESS;
}

void mcm_data_test_set_IP_family
(
  void
)
{
  char ip_family;

  MCM_DATA_TEST_LOG("\n==== Select IP Family ====");
  MCM_DATA_TEST_LOG("\n1. IPV4");
  MCM_DATA_TEST_LOG("\n2. IPV6");
  MCM_DATA_TEST_LOG("\n==========================");
  MCM_DATA_TEST_LOG("\nEnter : ");
  ip_family = mcm_data_test_input();

  if ( ip_family == '1' )
  {
    mcm_data_test_config.ip_family = 4;
  }
  else if ( ip_family == '2' )
  {
    mcm_data_test_config.ip_family = 6;
  }
  MCM_DATA_TEST_LOG("\nSet IP family : %d ",
                    mcm_data_test_config.ip_family);
}

void mcm_data_test_set_tech_pref
(
  void
)
{
  char tech_pref;

  MCM_DATA_TEST_LOG("\n==== Select Tech. Preference ====");
  MCM_DATA_TEST_LOG("\n1. CDMA");
  MCM_DATA_TEST_LOG("\n2. UMTS");
  MCM_DATA_TEST_LOG("\n3. AUTO");
  MCM_DATA_TEST_LOG("\n=================================");
  MCM_DATA_TEST_LOG("\nEnter : ");
  tech_pref = mcm_data_test_input();

  if ( tech_pref == '1' )
  {
    mcm_data_test_config.tech_pref = 1;
  }
  else if ( tech_pref == '2' )
  {
    mcm_data_test_config.tech_pref = 2;
  }
  else if ( tech_pref == '3' )
  {
    mcm_data_test_config.tech_pref = 7;
  }
  MCM_DATA_TEST_LOG("\nSet Tech. Preference : %d",
                    mcm_data_test_config.tech_pref);
}

void mcm_data_test_set_profile
(
  void
)
{
  mcm_data_test_config.profile = 0;

  MCM_DATA_TEST_LOG("\n==== Enter Profile Number ====");
  MCM_DATA_TEST_LOG("\nEnter : ");
  scanf("%d",&mcm_data_test_config.profile);

  MCM_DATA_TEST_LOG("\nSet Profile : %d",
                    mcm_data_test_config.profile);
}

void mcm_data_test_set_APN
(
  void
)
{
  int result = 0;

  MCM_DATA_TEST_LOG("\n==== Enter APN ====");
  MCM_DATA_TEST_LOG("\nEnter (max length %d) : ",sizeof(mcm_data_test_config.user_name)/8);
  memset(mcm_data_test_config.apn_name, 0x0,
                            sizeof(mcm_data_test_config.user_name));
  result = mcm_data_test_input_string( mcm_data_test_config.apn_name,
                            sizeof(mcm_data_test_config.user_name)/8);
  MCM_DATA_TEST_LOG("\nNo. of characters read : %d", result);
  MCM_DATA_TEST_LOG("\nSet APN : %s , len : %d",
                    mcm_data_test_config.apn_name,
                    strlen(mcm_data_test_config.apn_name));
}

void mcm_data_test_set_username
(
  void
)
{
  int result = 0;

  MCM_DATA_TEST_LOG("\n==== Enter User Name ====");
  MCM_DATA_TEST_LOG("\nEnter : ");
  memset(mcm_data_test_config.user_name, 0x0,
                            sizeof(mcm_data_test_config.user_name));
  result = mcm_data_test_input_string(mcm_data_test_config.user_name,
                            sizeof(mcm_data_test_config.user_name));
  MCM_DATA_TEST_LOG("\nNo. of characters read : %d", result);
  MCM_DATA_TEST_LOG("\nSet User Name : %s , len : %d",
                    mcm_data_test_config.user_name,
                    strlen(mcm_data_test_config.user_name));
}

void mcm_data_test_set_password
(
  void
)
{
  int result = 0;

  MCM_DATA_TEST_LOG("\n==== Enter Password ====");
  MCM_DATA_TEST_LOG("\nEnter : ");
  memset(mcm_data_test_config.password, 0x0,
                            sizeof(mcm_data_test_config.password));
  result = mcm_data_test_input_string(mcm_data_test_config.password,
                            sizeof(mcm_data_test_config.password));
  MCM_DATA_TEST_LOG("\nNo. of characters read : %d", result);
  MCM_DATA_TEST_LOG("\nSet Password : %s , len : %d",
                    mcm_data_test_config.password,
                    strlen(mcm_data_test_config.password));
}

void mcm_data_test_display_call_config
(
  void
)
{
  MCM_DATA_TEST_LOG("\n====== Data call current configuration =====");
  MCM_DATA_TEST_LOG("\nIP family : %d ",
                            mcm_data_test_config.ip_family);
  MCM_DATA_TEST_LOG("\nTech. Preference : %d",
                            mcm_data_test_config.tech_pref);
  MCM_DATA_TEST_LOG("\nProfile : %d", mcm_data_test_config.profile);
  MCM_DATA_TEST_LOG("\nAPN : %s , len : %d",
                            mcm_data_test_config.apn_name,
                            strlen(mcm_data_test_config.apn_name));
  MCM_DATA_TEST_LOG("\nUser Name : %s , len : %d",
                            mcm_data_test_config.user_name,
                            strlen(mcm_data_test_config.user_name));
  MCM_DATA_TEST_LOG("\nPassword : %s , len : %d",
                            mcm_data_test_config.password,
                            strlen(mcm_data_test_config.password));
}

void mcm_data_test_set_call_configs
(
  void
)
{
  char ch;
  int continue_config = 1;
  int32_t result = MCM_DATA_SUCCESS;

  while ( continue_config )
  {
    MCM_DATA_TEST_LOG("\n====== Data call Configuration setup =====");
    MCM_DATA_TEST_LOG("\n1 : To enter IP family");
    MCM_DATA_TEST_LOG("\n2 : To enter Tech. preference");
    MCM_DATA_TEST_LOG("\n3 : To enter profile");
    MCM_DATA_TEST_LOG("\n4 : To enter APN (Max. 100 characters)");
    MCM_DATA_TEST_LOG("\n5 : To enter Username (Max 127 characters)");
    MCM_DATA_TEST_LOG("\n6 : To enter Password (Max 127 characters)");
    MCM_DATA_TEST_LOG("\n7 : Display Call Configuration");
    MCM_DATA_TEST_LOG("\n8 : Make Data Call");
    MCM_DATA_TEST_LOG("\n9 : To return to main menu");
    MCM_DATA_TEST_LOG("\n==========================================");
    MCM_DATA_TEST_LOG("\nEnter : ");
    ch = mcm_data_test_input();
    switch (ch)
    {
      case '1' :
        mcm_data_test_set_IP_family();
        break;
      case '2' :
        mcm_data_test_set_tech_pref();
        break;
      case '3' :
        mcm_data_test_set_profile();
        break;
      case '4' :
        mcm_data_test_set_APN();
        break;
      case '5' :
        mcm_data_test_set_username();
        break;
      case '6' :
        mcm_data_test_set_password();
        break;
      case '7' :
        mcm_data_test_display_call_config();
        break;
      case '8' :
        result = mcm_data_test_request_start_data_call();

        if ( result == MCM_DATA_ERROR )
        {
          MCM_DATA_TEST_LOG("\nrequest_start_data_call: Failed.");
        }
        else if ( result == MCM_DATA_SUCCESS )
        {
          MCM_DATA_TEST_LOG("\nrequest_start_data_call: Succeded.");
        }
        break;
      case '9' :
        continue_config = 0;
        break;
      default :
        MCM_DATA_TEST_LOG("\nInvalid input.");
        break;
    }
  }
}

int32_t mcm_data_test_request_start_data_call
(
  void
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_data_start_data_call_req_msg_v01 start_data_call_req_msg;
  mcm_data_start_data_call_rsp_msg_v01 *start_data_call_rsp_msg;

  mcm_data_test_display_call_config();

  start_data_call_rsp_msg = NULL;
  start_data_call_rsp_msg = (mcm_data_start_data_call_rsp_msg_v01 *)malloc(sizeof(mcm_data_start_data_call_rsp_msg_v01));

  if (start_data_call_rsp_msg == NULL)
  {
    MCM_DATA_TEST_LOG("Failed to allocate memory");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  memset(&start_data_call_req_msg, 0x00, sizeof(mcm_data_start_data_call_req_msg_v01));
  memset(start_data_call_rsp_msg, 0x00, sizeof(mcm_data_start_data_call_rsp_msg_v01));


  if ((mcm_data_test_config.ip_family == IPV4_FAM) ||
      (mcm_data_test_config.ip_family == IPV6_FAM) ||
      (mcm_data_test_config.ip_family == IPV4V6_FAM))
  {
    start_data_call_req_msg.ip_family = mcm_data_test_config.ip_family;
    start_data_call_req_msg.ip_family_valid = TRUE;
  }
  else
  {
    start_data_call_req_msg.ip_family_valid = FALSE;
  }

  memcpy(&start_data_call_req_msg.apn_name[0], &mcm_data_test_config.apn_name[0],
           MCM_DATA_MAX_APN_LEN_V01);
  start_data_call_req_msg.apn_name_valid = TRUE;

  memcpy(&start_data_call_req_msg.user_name[0], &mcm_data_test_config.user_name[0],
           MCM_DATA_MAX_USERNAME_LEN_V01);
  start_data_call_req_msg.user_name_valid = TRUE;

  memcpy(&start_data_call_req_msg.password[0], &mcm_data_test_config.password[0],
           MCM_DATA_MAX_PASSWORD_LEN_V01);
  start_data_call_req_msg.password_valid = TRUE;

  if ((mcm_data_test_config.tech_pref == UMTS_TECH) ||
      (mcm_data_test_config.tech_pref == CDMA_TECH))
  {
    start_data_call_req_msg.tech_pref = mcm_data_test_config.tech_pref;
    start_data_call_req_msg.tech_pref_valid = TRUE;
  }
  else
  {
   start_data_call_req_msg.tech_pref_valid = FALSE;
  }

  start_data_call_req_msg.umts_profile = mcm_data_test_config.profile;
  start_data_call_req_msg.umts_profile_valid = TRUE;
  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_data_config.mcm_client_handle,
                        MCM_DATA_START_DATA_CALL_REQ_V01,
                        &start_data_call_req_msg,
                        start_data_call_rsp_msg,
                        (mcm_client_async_cb)mcm_data_test_async_cb,
                        &token_id);

  if ( mcm_error != MCM_SUCCESS_V01 )
  {
    MCM_DATA_TEST_LOG("\nrequest_start_data_call: Failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  else
  {
    MCM_DATA_TEST_LOG("\nrequest_start_data_call: Success");
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_request_stop_data_call
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  mcm_data_stop_data_call_req_msg_v01 stop_data_call_req_msg;
  mcm_data_stop_data_call_rsp_msg_v01 *stop_data_call_rsp_msg;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  uint32_t call_id, i;
  int32_t return_val = MCM_DATA_SUCCESS;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  MCM_DATA_TEST_LOG("\nrequest_stop_data_call: Stopping Call Id %d.",
                    call_id);

  stop_data_call_rsp_msg = NULL;
  stop_data_call_rsp_msg = (mcm_data_stop_data_call_rsp_msg_v01 *)malloc(sizeof(mcm_data_stop_data_call_rsp_msg_v01));

  if (stop_data_call_rsp_msg == NULL)
  {
    MCM_DATA_TEST_LOG("Failed to allocate memory");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  memset(&stop_data_call_req_msg, 0x00, sizeof(mcm_data_stop_data_call_req_msg_v01));
  memset(stop_data_call_rsp_msg, 0x00, sizeof(mcm_data_stop_data_call_rsp_msg_v01));


  stop_data_call_req_msg.call_id = call_id;
  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_data_config.mcm_client_handle,
                        MCM_DATA_STOP_DATA_CALL_REQ_V01,
                        &stop_data_call_req_msg,
                        stop_data_call_rsp_msg,
                        (mcm_client_async_cb)mcm_data_test_async_cb,
                        &token_id);
  if ( mcm_error != MCM_SUCCESS_V01 )
  {
    MCM_DATA_TEST_LOG("\nrequest_stop_data_call: Failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  else
  {
    for(i=0;i<MAX_DATA_CALLS;i++)
    {
      if(call_id_list[i] == call_id)
      {
        call_id_list[i] = INVALID_CALL_ID;
        MCM_DATA_TEST_LOG("Cleared call table index @ %d",i);
      }
    }

    MCM_DATA_TEST_LOG("\nrequest_stop_data_call: Success");
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_deinit
(
  void
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  int32_t return_val = MCM_DATA_SUCCESS;
  uint16_t service_list = MCM_DATA_V01;

  if (mcm_data_config.mcm_client_handle == 0)
  {
    MCM_DATA_TEST_LOG("Client already de-initialized");
    goto BAIL;
  }

  /* Stop the mcm_data service */
  mcm_error = mcm_data_srv_mgr_service_handle(
                             mcm_data_config.mcm_client_handle,
                             SERVICE_NOT_REQUIRED,
                             service_list);
  if (mcm_error != MCM_SUCCESS_V01)
  {
    MCM_DATA_TEST_LOG("Failed to request unload of services [%d]",mcm_error);
  }
  else
  {
    MCM_DATA_TEST_LOG("Successfully requested unload of services");
  }

  MCM_DATA_TEST_LOG("Waiting for indication from framework to release handle");
  MCM_DATA_TEST_LOG_FUNC_EXIT();

BAIL:
  return return_val;
}

int32_t mcm_data_test_get_data_call_status
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_call_status_req_msg_v01 req_msg;
  mcm_data_get_call_status_rsp_msg_v01 rsp_msg;
  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val =  MCM_DATA_ERROR;
    goto BAIL;
  }
  req_msg.call_id = call_id;

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_CALL_STATUS_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  if(mcm_error == MCM_SUCCESS_V01)
  {
    switch(rsp_msg.call_status){
      case MCM_DATA_CALL_STATE_INVALID_V01 :
        MCM_DATA_TEST_LOG("IDLE");
        break;
      case MCM_DATA_CALL_STATE_CONNECTING_V01:
        MCM_DATA_TEST_LOG("CONNECTING");
        break;
      case MCM_DATA_CALL_STATE_CONNECTED_V01:
        MCM_DATA_TEST_LOG("CONNECTED");
        break;
      case MCM_DATA_CALL_STATE_DISCONNECTING_V01:
        MCM_DATA_TEST_LOG("DISCONNECTING");
        break;
      case MCM_DATA_CALL_STATE_DISCONNECTED_V01:
        MCM_DATA_TEST_LOG("DISCONNECTING");
        break;
      default :
        MCM_DATA_TEST_LOG("\nUNSUPPORTED CALL STATUS");
    }
    MCM_DATA_TEST_LOG("\nget_data_call_status: Success");
  }
  else{
    MCM_DATA_TEST_LOG("\nget_data_call_status: Failed "
                              "with error code %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_get_data_call_tech
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_call_tech_req_msg_v01 req_msg;
  mcm_data_get_call_tech_rsp_msg_v01 rsp_msg;
  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  req_msg.call_id = call_id;

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_CALL_TECH_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  if (mcm_error == MCM_SUCCESS_V01)
  {
    if (rsp_msg.call_tech_valid == TRUE)
    {
      mcm_data_test_call_tech = rsp_msg.call_tech;
      MCM_DATA_TEST_LOG("\nget_data_call_tech : %s[%d]",mcm_data_test_tech_map[mcm_data_test_call_tech], mcm_data_test_call_tech);
    }
    else
    {
      MCM_DATA_TEST_LOG("Failed to get valid Call Tech", 0, 0, 0);
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("get_data_call_tech failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_get_device_name
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_device_name_req_msg_v01 req_msg;
  mcm_data_get_device_name_rsp_msg_v01 rsp_msg;
  uint32_t call_id;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(callid_p == NULL){
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else{
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  req_msg.call_id = call_id;

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                        MCM_DATA_GET_DEVICE_NAME_REQ_V01,
                        &req_msg,
                        &rsp_msg);

  if (mcm_error == MCM_SUCCESS_V01)
  {
    if (rsp_msg.device_name_valid == TRUE)
    {
      MCM_DATA_TEST_LOG("\nmcm_data_get_device_name: %s",
                              rsp_msg.device_name);
    }
    else
    {
      MCM_DATA_TEST_LOG("get_device_name: Failed due invalid device name", 0, 0, 0);
      return_val = MCM_DATA_ERROR;
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("get_device_name: Failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return  return_val;
}
int32_t mcm_data_test_get_device_address
(
  uint32_t *callid_p
)
{
  int32_t return_val = MCM_DATA_SUCCESS;
  /* No. of address available.*/
  uint32_t count = 0;
  uint32_t i=0;

  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_device_addr_req_msg_v01 req_msg;
  mcm_data_get_device_addr_rsp_msg_v01 rsp_msg;
  mcm_data_addr_info *address;
  uint32_t call_id;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  req_msg.call_id = call_id;

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_DEVICE_ADDR_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  address = (mcm_data_addr_info *)&rsp_msg.addr_info[0];

  if (mcm_error == MCM_SUCCESS_V01)
  {
    if (rsp_msg.addr_info_valid == TRUE)
    {
      count = rsp_msg.addr_info_len;
      MCM_DATA_TEST_LOG("\nNo. of addresses available : %d", count);

      while(i<count){
        MCM_DATA_TEST_LOG("\nAddress %d",i+1);
        MCM_DATA_TEST_LOG("\n\tInterface Address   : %d.%d.%d.%d"
                                  ,address[i].iface_addr_s.addr.__ss_padding[0]
                                  ,address[i].iface_addr_s.addr.__ss_padding[1]
                                  ,address[i].iface_addr_s.addr.__ss_padding[2]
                                  ,address[i].iface_addr_s.addr.__ss_padding[3]);
        MCM_DATA_TEST_LOG("\n\tGateway Address     : %d.%d.%d.%d"
                                  ,address[i].gtwy_addr_s.addr.__ss_padding[0]
                                  ,address[i].gtwy_addr_s.addr.__ss_padding[1]
                                  ,address[i].gtwy_addr_s.addr.__ss_padding[2]
                                  ,address[i].gtwy_addr_s.addr.__ss_padding[3]);
        MCM_DATA_TEST_LOG("\n\tPrimary DNS Address : %d.%d.%d.%d"
                                  ,address[i].dnsp_addr_s.addr.__ss_padding[0]
                                  ,address[i].dnsp_addr_s.addr.__ss_padding[1]
                                  ,address[i].dnsp_addr_s.addr.__ss_padding[2]
                                  ,address[i].dnsp_addr_s.addr.__ss_padding[3]);
        i++;
      }
    }
    else
    {
      MCM_DATA_TEST_LOG("\nmcm_data_get_device_address failed due to invalid address");
      return_val = MCM_DATA_ERROR;
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_get_device_address failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_get_device_address_count
(
  uint32_t *callid_p
)
{
  int32_t return_val = MCM_DATA_SUCCESS;
  /* No. of address available.*/
  uint32_t count = 0;

  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_device_addr_count_req_msg_v01 req_msg;
  mcm_data_get_device_addr_count_rsp_msg_v01 rsp_msg;
  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  req_msg.call_id = call_id;
  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_DEVICE_ADDR_COUNT_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  if (mcm_error == MCM_SUCCESS_V01)
  {
    if (rsp_msg.addr_count_valid == TRUE)
    {
      count = rsp_msg.addr_count;
      MCM_DATA_TEST_LOG("\nNo. of addresses available : %d", count);
    }
    else
    {
      MCM_DATA_TEST_LOG("\nmcm_data_get_device_address_count failed due to invalid address");
      return_val = MCM_DATA_ERROR;
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_get_device_address_count failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

int32_t mcm_data_test_get_pkt_stats
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_pkt_stats_req_msg_v01         req_msg;
  mcm_data_get_pkt_stats_rsp_msg_v01         rsp_msg;
  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  req_msg.call_id = call_id;
  memset(&rsp_msg.pkt_stats, 0x00, sizeof(mcm_data_pkt_stats_t_v01));

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                        MCM_DATA_GET_PKT_STATS_REQ_V01,
                        &req_msg,
                        &rsp_msg);

  if (mcm_error == MCM_SUCCESS_V01)
  {
    if (rsp_msg.pkt_stats_valid == TRUE)
    {
      mcm_data_test_display_pkt_stats(&rsp_msg.pkt_stats);
    }
    else
    {
      MCM_DATA_TEST_LOG("\nmcm_data_get_pkt_stats failed due to invalid packet stats");
      return_val = MCM_DATA_ERROR;
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("mcm_data_get_pkt_stats failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}


int32_t mcm_data_test_reset_pkt_stats( uint32_t *callid_p )
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_reset_pkt_stats_req_msg_v01         req_msg;
  mcm_data_reset_pkt_stats_rsp_msg_v01         rsp_msg;

  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  req_msg.call_id = call_id;
  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_RESET_PKT_STATS_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  if(mcm_error == MCM_SUCCESS_V01)
  {
    MCM_DATA_TEST_LOG("\nmcm_data_reset_pkt_stats : Success");
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_reset_pkt_stats : Failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}
int32_t mcm_data_test_get_reg_status
(
  void
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_reg_status_req_msg_v01         req_msg;
  mcm_data_get_reg_status_rsp_msg_v01         rsp_msg;

  memset(&rsp_msg, 0x00, sizeof(mcm_data_get_reg_status_rsp_msg_v01));

  mcm_error = mcm_client_execute_command_sync(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_REG_STATUS_REQ_MSG_V01,
                       NULL,
                       0,
                       &rsp_msg,
                       sizeof(rsp_msg));

  if(mcm_error == MCM_SUCCESS_V01)
  {
    MCM_DATA_TEST_LOG("\nmcm_data_test_get_reg_status: Success");
    if ( rsp_msg.reg_status_valid )
    {
      if ( rsp_msg.reg_status.srv_status != MCM_DATA_MODEM_STATE_OOS_V01 )
      {
        MCM_DATA_TEST_LOG("\nModem is IN Service");
        MCM_DATA_TEST_LOG("\nPreferred tech type is %d [%s]",
                        rsp_msg.reg_status.tech_info,
                        mcm_data_test_tech_map[rsp_msg.reg_status.tech_info]);
      }
      else
      {
        MCM_DATA_TEST_LOG("\nModem is OUT of Service");
      }
    }
    else
    {
      MCM_DATA_TEST_LOG("Invalid Reg Status received");
      return_val = MCM_DATA_ERROR;
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_test_get_reg_status failed with error %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}
int32_t mcm_data_test_get_data_channel_rate
(
  uint32_t *callid_p
)
{
  MCM_DATA_TEST_LOG_FUNC_ENTRY();

  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_get_channel_rate_req_msg_v01         req_msg;
  mcm_data_get_channel_rate_rsp_msg_v01         rsp_msg;

  uint32_t call_id;

  if(callid_p == NULL)
  {
    call_id = mcm_data_test_list_and_get_call_id();
  }
  else
  {
    call_id = *callid_p;
  }

  if(call_id == INVALID_CALL_ID)
  {
    MCM_DATA_TEST_LOG("Incorrect Call ID");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  req_msg.call_id = call_id;

  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_GET_CHANNEL_RATE_REQ_MSG_V01,
                       &req_msg,
                       &rsp_msg);

  if(mcm_error == MCM_SUCCESS_V01)
  {
    MCM_DATA_TEST_LOG("\nmcm_data_get_data_channel_rate : Success");
    mcm_data_test_display_channel_rate(&(rsp_msg.channel_rate));
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_get_data_channel_rate : Error Code %d", mcm_error);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  MCM_DATA_TEST_LOG_FUNC_EXIT();
BAIL:
  return return_val;
}

void mcm_data_test_display_pkt_stats
(
  mcm_data_pkt_stats_t_v01 *mcm_data_test_pkt_stats
)
{
  MCM_DATA_TEST_LOG("\n============== Packet Stats ==============");
  MCM_DATA_TEST_LOG("\nNumber of packets transmitted : %lu",
                            mcm_data_test_pkt_stats->pkts_tx);
  MCM_DATA_TEST_LOG("\nNumber of packets received : %lu",
                            mcm_data_test_pkt_stats->pkts_rx);
  MCM_DATA_TEST_LOG("\nNumber of bytes transmitted : %lld",
                            mcm_data_test_pkt_stats->bytes_tx);
  MCM_DATA_TEST_LOG("\nNumber of bytes received : %lld",
                            mcm_data_test_pkt_stats->bytes_rx);
  MCM_DATA_TEST_LOG("\nNumber of transmit packets dropped : %lu",
                            mcm_data_test_pkt_stats->pkts_dropped_tx);
  MCM_DATA_TEST_LOG("\nNumber of receive packets dropped : %lu",
                            mcm_data_test_pkt_stats->pkts_dropped_rx);
  MCM_DATA_TEST_LOG("\n==========================================");
}

void mcm_data_test_display_channel_rate
(
  mcm_data_channel_rate_t_v01 *mcm_data_test_channel_rate
)
{
  MCM_DATA_TEST_LOG("\n============== Channel Rate ==============");
  if(mcm_data_test_channel_rate->current_tx_rate != UINT32_MAX)
  {
    MCM_DATA_TEST_LOG("Current Transmission Rate : %lu", mcm_data_test_channel_rate->current_tx_rate);
  }
  else
  {
    MCM_DATA_TEST_LOG("Current Transmission Rate : Unsupported");
  }
  if(mcm_data_test_channel_rate->current_rx_rate != UINT32_MAX)
  {
    MCM_DATA_TEST_LOG("Current Recieve Rate : %lu", mcm_data_test_channel_rate->current_rx_rate);
  }
  else
  {
    MCM_DATA_TEST_LOG("Current Recieve Rate : Unsupported");
  }
  if(mcm_data_test_channel_rate->max_tx_rate != UINT32_MAX)
  {
    MCM_DATA_TEST_LOG("Max Transmission Rate : %lu", mcm_data_test_channel_rate->max_tx_rate);
  }
  else
  {
    MCM_DATA_TEST_LOG("Max Transmission Rate : Unsupported");
  }
  if(mcm_data_test_channel_rate->max_rx_rate != UINT32_MAX)
  {
    MCM_DATA_TEST_LOG("Max Recieve Rate : %lu", mcm_data_test_channel_rate->max_rx_rate);
  }
  else
  {
    MCM_DATA_TEST_LOG("Max Recieve Rate : Unsupported");
  }
    MCM_DATA_TEST_LOG("\n==========================================");
}

uint32_t mcm_data_test_add_route
(
  void
)
{
  int32_t return_val = MCM_DATA_SUCCESS;
  char ip_addr[16];
  char *iface_name;
  int ret_val;

  iface_name = (char *)malloc(sizeof(char)*100);
  if ( iface_name == NULL )
  {
    MCM_DATA_TEST_LOG("mcm_data_test_add_route: Out of memory !!");
    MCM_DATA_TEST_LOG("\nExiting..");
    return MCM_DATA_ERROR;
  }
  MCM_DATA_TEST_LOG("\nEnter IP Address to add");
  mcm_data_test_input_string((char *)ip_addr, 16);
  MCM_DATA_TEST_LOG("\nEnter Interface Name");
  mcm_data_test_input_string(iface_name,100);

  //Deleting then adding route makes it the active route
  char *route_del_cmd[] ={"route", "del", "default", "gw", (char *)ip_addr, (char *) 0 };
  mcm_data_test_system_call("route", route_del_cmd);

  char *route_add_cmd[] ={"route", "add", "default", "gw", (char *)ip_addr, iface_name, (char *) 0 };
  if( (ret_val = mcm_data_test_system_call("route", route_add_cmd)) != 0)
  {
    MCM_DATA_TEST_LOG("mcm_data_test_add_route: Failed with Error code %d", ret_val);
    return_val = MCM_DATA_ERROR;
  }
  else
  {
    MCM_DATA_TEST_LOG("mcm_data_test_add_route: Success");
  }
  return return_val;
}

uint32_t mcm_data_test_data_transfer
(
  void
)
{
  uint32_t return_val = MCM_DATA_SUCCESS;
  char ip_addr[16], cmd[50], n[100];
  MCM_DATA_TEST_LOG("Enter number of pings:");
  mcm_data_test_input_string((char *)n, 100);

  MCM_DATA_TEST_LOG("Enter IP Address to ping:");
  mcm_data_test_input_string((char *)ip_addr, 16);

  char *ping_cmd[] ={"ping", "-c", n, (char *)ip_addr, (char *) 0 };
  if((return_val = mcm_data_test_system_call("ping", ping_cmd)) != MCM_DATA_SUCCESS)
  {
    MCM_DATA_TEST_LOG("Ping failed");
    return_val = MCM_DATA_ERROR;
  }
BAIL:
  return return_val;
}

void list_active_calls
(
  void
)
{
  int i=0;
  MCM_DATA_TEST_LOG("\n============== Active Calls ==============");
  for(;i<MAX_DATA_CALLS;i++)
  {
    if(call_id_list[i] != INVALID_CALL_ID)
    {
        MCM_DATA_TEST_LOG("\n%d",call_id_list[i]);
    }
  }
  MCM_DATA_TEST_LOG("\n==========================================");
}

int active_call_count
(
  void
)
{
  int i=0, cnt=0;
  for(;i<MAX_DATA_CALLS;i++)
  {
    if(call_id_list[i] != INVALID_CALL_ID)
      cnt++;
  }
  return cnt;
}

uint32_t mcm_data_test_event_register
(
  boolean *events
)
{
  int32_t return_val = MCM_DATA_SUCCESS;
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;
  mcm_data_event_register_req_msg_v01      req_msg;
  mcm_data_event_register_resp_msg_v01     rsp_msg;
  uint32_t num_events_to_register, x, i;
  memset(&req_msg,0,sizeof(req_msg));
  if(!events){
      MCM_DATA_TEST_LOG("\n========= List of Event Types Available =========");
      MCM_DATA_TEST_LOG("\n1. Net Up");
      MCM_DATA_TEST_LOG("\n2. Net Down");
      MCM_DATA_TEST_LOG("\n3. Net New Addr");
      MCM_DATA_TEST_LOG("\n4. Net Del Addr");
      MCM_DATA_TEST_LOG("\n5. Reg Service Status");
      MCM_DATA_TEST_LOG("\n6. Bearer Tech Status");
      MCM_DATA_TEST_LOG("\n7. Dormancy Status");
      MCM_DATA_TEST_LOG("\n=================================================");
      MCM_DATA_TEST_LOG("\nEnter number of events to register for followed by the event numbers");
      scanf("%d",&num_events_to_register);
      getchar();
  }
  else
    num_events_to_register = MAX_UNSOL_EVENTS;
  for(i=0;i<num_events_to_register;i++)
  {
    if(!events)
    {
      scanf("%d",&x);
      getchar();
    }
    else if ( events[i+1] )
      x = i+1;
    else
      continue;
    switch(x)
    {
      case 1:
        req_msg.register_net_up_event_valid = TRUE;
        req_msg.register_net_up_event = TRUE;
        break;
      case 2:
        req_msg.register_net_down_event_valid = TRUE;
        req_msg.register_net_down_event = TRUE;
        break;
      case 3:
        req_msg.register_net_new_addr_event_valid = TRUE;
        req_msg.register_net_new_addr_event = TRUE;
        break;
      case 4:
        req_msg.register_net_del_addr_event_valid = TRUE;
        req_msg.register_net_del_addr_event= TRUE;
        break;
      case 5:
        req_msg.register_reg_srvc_status_event_valid = TRUE;
        req_msg.register_reg_srvc_status_event = TRUE;
        break;
      case 6:
        req_msg.register_bearer_tech_status_event_valid = TRUE;
        req_msg.register_bearer_tech_status_event = TRUE;
        break;
      case 7:
        req_msg.register_dormancy_status_event_valid = TRUE;
        req_msg.register_dormancy_status_event = TRUE;
        break;
      default:
        MCM_DATA_TEST_LOG("Please enter a number between 1 and 7");
        i--;
    }
  }
  mcm_error = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_data_config.mcm_client_handle,
                       MCM_DATA_EVENT_REGISTER_REQ_V01,
                       &req_msg,
                       &rsp_msg);

  if(mcm_error == MCM_SUCCESS_V01)
  {
    MCM_DATA_TEST_LOG("\nmcm_data_test_event_register: Success");
  }
  else
  {
    MCM_DATA_TEST_LOG("\nmcm_data_test_event_register : Error Code %d", rsp_msg.response.error);
    return_val = MCM_DATA_ERROR;
  }
  return return_val;
}

void mcm_data_test_app_init
(
  void
)
{
  uint8_t i = 0;

  memset((mcm_data_call_config *)&mcm_data_test_config, 0x00,
                            sizeof(mcm_data_call_config));
  mcm_data_test_call_tech = 0;
  mcm_data_test_channel_rate = (mcm_data_channel_rate_t_v01 *)malloc(sizeof(mcm_data_channel_rate_t_v01));
  mcm_data_test_req_call_cnt = 0;
  mcm_data_test_rsp_call_cnt = 0;
  mcm_data_inited = FALSE;

  for(i=0;i< MAX_DATA_CALLS; i++)
  {
    call_id_list[i] = INVALID_CALL_ID;
  }
}

/* Decides if any CLI options needs mcm_data_inited flag set to TRUE */
boolean opt_needs_mcm_data_init
(
  char opt
)
{
  /* Exit, Init and De-Init don't need mcm_data_inited */
  if ( opt == 'x' || opt=='1' || opt=='5' )
  {
    return FALSE;
  }
  return TRUE;
}
void mcm_data_test_main_menu
(
  void
)
{
  char ch;
  int32_t result = MCM_DATA_SUCCESS;
  int continue_test = 1;
  char *mcm_data_test_menu = "\n========= Main Menu =========\n"
                             "\n1 : mcm_data_init\n"
                             "\n2 : set_call_config\n"
                             "\n3 : mcm_data_start_data_call\n"
                             "\n4 : mcm_data_stop_data_call\n"
                             "\n5 : mcm_data_deinit\n"
                             "\n6 : mcm_data_get_data_call_status\n"
                             "\n7 : mcm_data_get_data_call_tech\n"
                             "\n8 : mcm_data_get_device_name\n"
                             "\n9 : mcm_data_get_device_addr_count\n"
                             "\na : mcm_data_get_device_addr\n"
                             "\nb : mcm_data_get_pkt_stats\n"
                             "\nc : mcm_data_reset_pkt_stats\n"
                             "\nd : mcm_data_get_data_channel_rate\n"
                             "\ne : mcm_data_get_reg_status\n"
                             "\nf : mcm_data_event_register\n"
                             "\nl : list_active_calls\n"
                             "\nr : mcm_data_test_add_route\n"
                             "\nt : do_data_transfer\n"
                             "\nx : exit\n"
                             "\n=============================\n"
                             "\nEnter : \n";
  while(continue_test)
  {
    MCM_DATA_TEST_LOG("%s", mcm_data_test_menu);
    ch = mcm_data_test_input();

    if ( opt_needs_mcm_data_init(ch) && !mcm_data_inited)
    {
      MCM_DATA_TEST_LOG("\nmcm_data not inited");
      continue;
    }
    switch(ch)
    {
      case '1' :
        result = mcm_data_test_init();
        break;
      case '2' :
        mcm_data_test_set_call_configs();
        break;
      case '3' :
        result = mcm_data_test_request_start_data_call();
        break;
      case '4' :
        result = mcm_data_test_request_stop_data_call(NULL);
        break;
      case '5' :
        result = mcm_data_test_deinit();
        break;
      case '6' :
        result = mcm_data_test_get_data_call_status(NULL);
        break;
      case '7' :
        result = mcm_data_test_get_data_call_tech(NULL);
        break;
      case '8' :
        result = mcm_data_test_get_device_name(NULL);
        break;
      case '9' :
        result = mcm_data_test_get_device_address_count(NULL);
        break;
      case 'a' :
        result = mcm_data_test_get_device_address(NULL);
        break;
      case 'b' :
        result = mcm_data_test_get_pkt_stats(NULL);
        break;
      case 'c' :
        result = mcm_data_test_reset_pkt_stats(NULL);
        break;
      case 'd' :
        result = mcm_data_test_get_data_channel_rate(NULL);
        break;
      case 'e' :
        result = mcm_data_test_get_reg_status();
        break;
      case 'f' :
        result = mcm_data_test_event_register(NULL);
        break;
      case 'l':
        list_active_calls();
        break;
      case 'r':
        result = mcm_data_test_add_route();
        break;
      case 't' :
        result = mcm_data_test_data_transfer();
        break;
      case 'x' :
        continue_test = 0;
        break;
      default :
        MCM_DATA_TEST_LOG("\nInvalid input.");
    }
  }
}

void mcm_data_test_display_help
(
  void
)
{
  MCM_DATA_TEST_LOG("USAGE:\n/usr/tests/mcm_data_test\n"\
      "\t-n NUM_CALLS \\\n"\
      "\t-apn_list apn_1 apn_2 ... apn_n \\\n"\
      "\t-tech_list techpref_1 techpref_2 ... techpref_n \\ [1 for UMTS, 0 for CDMA]\n"\
      "\t-uname_list uname_1 uname_2 ... uname_n \\\n"\
      "\t-pwd_list pwd_1 pwd_2 ... pwd_n \\\n"\
      "\t-ip_list ipfamily_1 ipfamily_2 ... ipfamily_n \\ [4 for IPv4, 6 for IPv6 and 10 for IPv4v6]\n\n" \
      "ARGUMENTS\n" \
      "\t-n      Sets the number of data calls to bring up\n" \
      "\t-apn_list  Sets the apn_name for each of the n data calls to bring up\n" \
      "\t-tech_list  Sets the tech_pref for each of the n data calls to bring up\n" \
      "\t-uname_list  Sets the user_name for each of the n data calls to bring up\n" \
      "\t-pwd_list  Sets the password for each of the n data calls to bring up\n" \
      "\t-ip_list    Sets the ip_family for each of the n data calls to bring up\n" \
      "\t-help    Lists the help options\n\n" \
      "EXAMPLES\n" \
      "\t\"/usr/tests/mcm_data_test\" - Starts an interactive menu based test app\n" \
      "\t\"/usr/tests/mcm_data_test -n 3\" - Brings up 3 data calls on default configs\n" \
      "\t\"/usr/tests/mcm_data_test -n 3 -apn_list a b c\" - Brings up 3 data calls using apn names for each call as \"a\" \"b\" and \"c\"\n" \
      );
}

int execute_cli_args
(
  int argc,
  char **argv
)
{
  int i=0,j =0, n = 0, ret_val = MCM_DATA_SUCCESS;

  if(argc > 1)
  {
    n = atoi(argv[1]);
  }

  mcm_data_call_config config_list[n];
  uint32_t up_seq[n], down_seq[n];

  memset(config_list, 0x00, n*sizeof(mcm_data_call_config));
  for(i=0;i<n;i++)
  {
    up_seq[i]=i;
    down_seq[i]=i;
  }

  if(strcmp(argv[0], "-help")==0 || strcmp(argv[0], "--help")==0 || strcmp(argv[0], "help")==0)
  {
    mcm_data_test_display_help();
    ret_val = MCM_DATA_SUCCESS;
    goto BAIL_DIRECT;
  }

  MCM_DATA_TEST_LOG("Initializing...");
  if(mcm_data_test_init() == MCM_DATA_ERROR)
  {
    MCM_DATA_TEST_LOG("Initialization Failed!");
    ret_val = MCM_DATA_ERROR;
    BAILOUT;
  }
  MCM_DATA_TEST_LOG("Initialization Complete!");
  if(argc<2)
  {
    MCM_DATA_TEST_LOG_ERROR("Too few options");
    ret_val = MCM_DATA_ERROR;
    BAILOUT;
  }

  if(strcmp(argv[0], "-n")!=0)
  {
    MCM_DATA_TEST_LOG_ERROR("Specify number of calls using -n");
    ret_val = MCM_DATA_ERROR;
    BAILOUT;
  }

  if(!mcm_data_test_is_num(argv[1]))
  {
    MCM_DATA_TEST_LOG_ERROR("Error reading number of calls after -n switch");
    ret_val = MCM_DATA_ERROR;
    BAILOUT;
  }

  if(n == 0)
  {
    MCM_DATA_TEST_LOG("Nothing to do");
    ret_val = MCM_DATA_SUCCESS;
    BAILOUT;
  }
  i=2;
  while(i<argc)
  {
    if(strcmp(argv[i], "-apn_list") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        std_strlcpy(config_list[j].apn_name, argv[i], MCM_DATA_MAX_APN_LEN_V01);
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -apn_list");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-tech_list") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        config_list[j].tech_pref = atoi(argv[i]);
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -tech_list");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-uname_list") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        std_strlcpy(config_list[j].user_name, argv[i], MCM_DATA_MAX_USERNAME_LEN_V01);
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -uname_list");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-pwd_list") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        std_strlcpy(config_list[j].password, argv[i], MCM_DATA_MAX_PASSWORD_LEN_V01);
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -pwd_list");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-ip_list") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        config_list[j].ip_family = atoi(argv[i]);
        if(config_list[j].ip_family !=4 && config_list[j].ip_family != 6 && config_list[j].ip_family != 10)
        {
          MCM_DATA_TEST_LOG_ERROR("Invalid argument for -ip_list");
          ret_val = MCM_DATA_ERROR;
          BAILOUT;
        }
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -ip_list");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-up_seq") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        up_seq[j] = atoi(argv[i]) - 1;
        if(up_seq[j]<0 || up_seq[j]>n-1)
        {
          MCM_DATA_TEST_LOG_ERROR("Invalid argument for -up_seq");
          ret_val = MCM_DATA_ERROR;
          BAILOUT;
        }
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -up_seq");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
    else if(strcmp(argv[i], "-down_seq") == 0)
    {
      i++;
      for(j=0; j<n && i<argc ;j++)
      {
        down_seq[j] = atoi(argv[i]) - 1;
        if(down_seq[j]<0 || down_seq[j]>n-1)
        {
          MCM_DATA_TEST_LOG_ERROR("Invalid argument for -down_seq");
          ret_val = MCM_DATA_ERROR;
          BAILOUT;
        }
        i++;
      }
      if(j!=n)
      {
        MCM_DATA_TEST_LOG_ERROR("Insufficient arguments for -down_seq");
        ret_val = MCM_DATA_ERROR;
        BAILOUT;
      }
    }
  }
  mcm_data_test_req_call_cnt = n;
  for(j=0;j<n;j++)
  {
    mcm_data_test_config = config_list[up_seq[j]];
    mcm_data_test_request_start_data_call();
  }
  MCM_DATA_TEST_LOG("Waiting on pthread_cond_wait");
  pthread_mutex_lock(&mutex);
  pthread_cond_wait(&cond, &mutex);
  if(active_call_count() == n)
  {
    MCM_DATA_TEST_LOG("All Calls are up");
  }
  else
  {
    MCM_DATA_TEST_LOG("Only %d of %d calls came up ", active_call_count(), n);
  }
  pthread_mutex_unlock(&mutex);
  int k=1;
  for(j=0; j< MAX_DATA_CALLS; j++)
  {
    if(call_id_list[j] != INVALID_CALL_ID)
    {
      MCM_DATA_TEST_LOG("==================================== Call %d =================================",k);
      k++;
      mcm_data_test_get_data_call_status(&call_id_list[j]);
      mcm_data_test_get_data_call_tech(&call_id_list[j]);
      mcm_data_test_get_device_name(&call_id_list[j]);
      mcm_data_test_get_device_address_count(&call_id_list[j]);
      mcm_data_test_get_device_address(&call_id_list[j]);
      mcm_data_test_get_pkt_stats(&call_id_list[j]);
      mcm_data_test_get_data_channel_rate(&call_id_list[j]);
      MCM_DATA_TEST_LOG("==============================================================================");
    }
  }
  for(j=0;j<n;j++)
  {
    if(call_id_list[down_seq[j]] != INVALID_CALL_ID)
    {
      mcm_data_test_request_stop_data_call(&call_id_list[down_seq[j]]);
    }
  }
BAIL:
  MCM_DATA_TEST_LOG("De-Initializing...");
  if(mcm_data_test_deinit() == MCM_DATA_ERROR)
  {
    MCM_DATA_TEST_LOG("De-Initialization Failed!");
    ret_val = MCM_DATA_ERROR;
    goto BAIL_DIRECT;
  }
  MCM_DATA_TEST_LOG("De-Initialization Complete!");
BAIL_DIRECT:
  return ret_val;
}

int main
(
  int argc,
  char **argv
)
{
  mcm_data_test_app_init();
  if(argc == 1)
  {
    mcm_data_test_mode = MCM_DATA_TEST_MODE_MENU;
    mcm_data_test_main_menu();
  }
  else
  {
    mcm_data_test_mode = MCM_DATA_TEST_MODE_CLI;
    return execute_cli_args(argc-1, argv+1);
  }
  return MCM_DATA_SUCCESS;
}
