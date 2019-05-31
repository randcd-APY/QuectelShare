/******************************************************************************
  @file    mcm_sim_test.c
  @brief   MCM UIM test application

  DESCRIPTION
  test suite of basic MCM UIM functionality
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/******************************************************************************

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   tl      Fixed KW errors
10/25/13   tl      Initial version

******************************************************************************/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mcm_sim_v01.h"
#include "mcm_common_v01.h"
#include "mcm_client.h"
#include "utils_common.h"
#include "qmi_client.h"

/*===========================================================================

                           DEFINES

===========================================================================*/

#define MCM_SIM_TEST_LOG_DEBUG(...) \
  printf(__VA_ARGS__); \
  printf("\n")
#define MCM_SIM_TEST_LOG_ERROR(...) \
  printf(__VA_ARGS__); \
  printf("\n")
#define MCM_SIM_TEST_LOG_FATAL(...) \
  printf(__VA_ARGS__); \
  printf("\n")
#define MCM_SIM_TEST_LOG_VERBOSE(...) \
  printf( __VA_ARGS__); \
  printf("\n")
#define MCM_SIM_TEST_LOG_INFO(...) \
  printf(__VA_ARGS__); \
  printf("\n")

/*===========================================================================

                           GLOBALS

===========================================================================*/

uint32_t                    mcm_client_handle   = -1;
static pthread_t            sim_test_thread;
util_list_info_type       * sim_test_queue      = NULL;

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

int sim_test_add_event
(
  uint32_t                message_id,
  void                  * resp_data,
  uint32_t                resp_data_len
);

int sim_test_remove_event
(
  void * event_data_to_be_removed
);

void * sim_test_thread_proc
(
  void * sim_test_thread_proc_param
);

typedef struct sim_test_resp_data_type
{
  int                    is_processed;
  uint32_t               msg_id;
  void                 * resp_data;
  uint32_t               resp_data_len;
} sim_test_resp_data_type;

/*===========================================================================

                           FUNCTIONS

===========================================================================*/

static void mcm_ind_cb
(
  mcm_client_handle_type      client_handle,
  uint32_t                    message_id,
  void                      * ind_c_struct,
  uint32_t                    ind_c_struct_len
)
{
  void * indication_ptr         = NULL;

  MCM_SIM_TEST_LOG_INFO("mcm_ind_cb ENTER");

  if(client_handle != mcm_client_handle)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_ind_cb: FATAL ERROR invalid client handle");
    return;
  }

  if(ind_c_struct == NULL || ind_c_struct_len == 0)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_ind_cb: ERROR invalid input parameters");
    return;
  }

  indication_ptr = (void*)util_memory_alloc(ind_c_struct_len);
  if(indication_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_ind_cb: failed to allocate memory");
    return;
  }
  memcpy(indication_ptr, ind_c_struct, ind_c_struct_len);

  (void)sim_test_add_event(message_id, indication_ptr, ind_c_struct_len);
} /* mcm_ind_cb */


static void mcm_async_cb
(
  mcm_client_handle_type    client_handle,
  uint32_t                  message_id,
  void                    * resp_c_struct,
  uint32_t                  resp_c_struct_len,
  void                    * token_id
)
{
  int    ret_code               = ENOMEM;
  void * resp_ptr               = NULL;

  MCM_SIM_TEST_LOG_INFO("mcm_async_cb ENTER");

  if(client_handle != mcm_client_handle)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_async_cb: FATAL ERROR invalid client handle");
    return;
  }

  if(resp_c_struct == NULL || resp_c_struct_len == 0)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_async_cb: ERROR invalid input parameters");
    return;
  }

  resp_ptr = (void*)util_memory_alloc(resp_c_struct_len);
  if(resp_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_async_cb: failed to allocate memory");
    return;
  }
  memcpy(resp_ptr, resp_c_struct, resp_c_struct_len);

  ret_code = sim_test_add_event(message_id, resp_ptr, resp_c_struct_len);
} /* mcm_async_cb */


static mcm_sim_slot_id_t_v01 mcm_sim_test_util_get_slot_id_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nSelect Slot ID");
    MCM_SIM_TEST_LOG_VERBOSE("1 : slot id 1");
    MCM_SIM_TEST_LOG_VERBOSE("2 : slot id 2");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return MCM_SIM_SLOT_ID_1_V01;
    }
    else if(input == '2')
    {
      return MCM_SIM_SLOT_ID_2_V01;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid slot id input. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_slot_id_input */


static mcm_sim_slot_id_t_v01 mcm_sim_test_util_get_app_type_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nSelect App Type");
    MCM_SIM_TEST_LOG_VERBOSE("1 : 3gpp");
    MCM_SIM_TEST_LOG_VERBOSE("2 : 3gpp2");
    MCM_SIM_TEST_LOG_VERBOSE("3 : isim");
    MCM_SIM_TEST_LOG_VERBOSE("4 : other");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return MCM_SIM_APP_TYPE_3GPP_V01;
    }
    else if(input == '2')
    {
      return MCM_SIM_APP_TYPE_3GPP2_V01;
    }
    else if(input == '3')
    {
      return MCM_SIM_APP_TYPE_ISIM_V01;
    }
    else if(input == '4')
    {
      return MCM_SIM_APP_TYPE_UNKNOWN_V01;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid app type input. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_app_type_input */


static uint8_t mcm_sim_test_util_get_record_num_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("Enter Record Number (0 for transparent file): ");

    while((input = getchar()) == '\n');

    if(input == '0')
    {
      return 0;
    }
    if(input == '1')
    {
      return 1;
    }
    if(input == '2')
    {
      return 2;
    }
    if(input == '3')
    {
      return 3;
    }
    if(input == '4')
    {
      return 4;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nRecord Number not supported. Please try number between (0-4).");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_record_num_input */


static mcm_sim_pin_id_t_v01 mcm_sim_test_util_get_pin_id_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nSelect PIN ID");
    MCM_SIM_TEST_LOG_VERBOSE("1 : PIN id 1");
    MCM_SIM_TEST_LOG_VERBOSE("2 : PIN id 2");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return MCM_SIM_PIN_ID_1_V01;
    }
    else if(input == '2')
    {
      return MCM_SIM_PIN_ID_2_V01;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid PIN id input. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_pin_id_input */


static mcm_sim_perso_feature_t_v01 mcm_sim_test_util_get_perso_feature_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nSelect Perso Feature");
    MCM_SIM_TEST_LOG_VERBOSE("1 : GW NW");
    MCM_SIM_TEST_LOG_VERBOSE("2 : GW NS");
    MCM_SIM_TEST_LOG_VERBOSE("3 : GW SP");
    MCM_SIM_TEST_LOG_VERBOSE("4 : GW CP");
    MCM_SIM_TEST_LOG_VERBOSE("5 : GW SIM");
    MCM_SIM_TEST_LOG_VERBOSE("6 : 1X NW1");
    MCM_SIM_TEST_LOG_VERBOSE("7 : 1X NW2");
    MCM_SIM_TEST_LOG_VERBOSE("8 : 1X RUIM");
    MCM_SIM_TEST_LOG_VERBOSE("9 : Unknown");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_V01;
    }
    else if(input == '2')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_SUBSET_V01;
    }
    else if(input == '3')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SERVICE_PROVIDER_V01;
    }
    else if(input == '4')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP_CORPORATE_V01;
    }
    else if(input == '5')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SIM_V01;
    }
    else if(input == '6')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_1_V01;
    }
    else if(input == '7')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_2_V01;
    }
    else if(input == '8')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_RUIM_V01;
    }
    else if(input == '9')
    {
      return MCM_SIM_PERSO_FEATURE_STATUS_UNKNOWN_V01;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid Perso Feature input. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_perso_feature_input */


static mcm_sim_perso_operation_t_v01 mcm_sim_test_util_get_perso_operation_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nSelect Perso Operation");
    MCM_SIM_TEST_LOG_VERBOSE("1 : Deactivate");
    MCM_SIM_TEST_LOG_VERBOSE("2 : Unblock");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return MCM_SIM_PERSO_OPERATION_DEACTIVATE_V01;
    }
    else if(input == '2')
    {
      return MCM_SIM_PERSO_OPERATION_UNBLOCK_V01;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid Perso Operation. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_perso_operation_input */


static uint32_t mcm_sim_test_util_get_bin_number_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    while((input = getchar()) == '\n');

    if(input >= '0' && input <= '9')
    {
      return (input - '0');
    }
    else if(input >= 'a' && input <= 'z')
    {
      return (input - 'a' + 10);
    }
    else if(input >= 'A' && input <= 'Z')
    {
      return (input - 'A' + 10);
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid input number. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_bin_number_input */


static uint32_t mcm_sim_test_util_get_char_number_input
(
  void
)
{
  char input = 0;

  while(1)
  {
    while((input = getchar()) == '\n');

    if(input >= '0' && input <= '9')
    {
      return input;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid input character. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_get_char_number_input */


static void mcm_sim_test_util_get_perso_network_data_input
(
  mcm_sim_network_perso_t_v01 * network_data_ptr
)
{
  char mcc_string[MCM_SIM_MCC_LEN_V01 + 1];
  char mnc_string[MCM_SIM_MNC_MAX_V01 + 1];

  if(network_data_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\Invalid network data input");
    return;
  }

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nEnter MCC: ");
    scanf("%4s", mcc_string);
    if(strlen(mcc_string) == 3)
    {
      memcpy(network_data_ptr->mcc,
             mcc_string,
             strlen(mcc_string));
      break;
    }
    MCM_SIM_TEST_LOG_VERBOSE("\nInvalid MCC input");
    sleep(1);
  }

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nEnter MNC: ");
    scanf("%4s", mnc_string);
    network_data_ptr->mnc_len = strlen(mnc_string);
    if(network_data_ptr->mnc_len == 3)
    {
      memcpy(network_data_ptr->mnc,
             mnc_string,
             network_data_ptr->mnc_len);
      break;
    }
    MCM_SIM_TEST_LOG_VERBOSE("\nInvalid MNC input");
    sleep(1);
  }
}

static int mcm_sim_test_util_event_reg
(
  void
)
{
  char input = 0;

  while(1)
  {
    MCM_SIM_TEST_LOG_VERBOSE("\nRegister for event?");
    MCM_SIM_TEST_LOG_VERBOSE("1 : Register");
    MCM_SIM_TEST_LOG_VERBOSE("0 : Deregister");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    if(input == '1')
    {
      return TRUE;
    }
    else if(input == '0')
    {
      return FALSE;
    }
    else
    {
      MCM_SIM_TEST_LOG_VERBOSE("\nInvalid registration input. Please try again.");
      sleep(1);
    }
  }
} /* mcm_sim_test_util_event_reg */


static void mcm_sim_test_print_card_state
(
  mcm_sim_card_state_t_v01  card_state
)
{
  switch(card_state)
  {
    case MCM_SIM_CARD_STATE_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("card state unknown");
      break;
    case MCM_SIM_CARD_STATE_ABSENT_V01:
      MCM_SIM_TEST_LOG_INFO("card state absent");
      break;
    case MCM_SIM_CARD_STATE_PRESENT_V01:
      MCM_SIM_TEST_LOG_INFO("card state present");
      break;
    case MCM_SIM_CARD_STATE_ERROR_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("card state error unknown");
      break;
    case MCM_SIM_CARD_STATE_ERROR_POWER_DOWN_V01:
      MCM_SIM_TEST_LOG_INFO("card state error power down");
      break;
    case MCM_SIM_CARD_STATE_ERROR_POLL_ERROR_V01:
      MCM_SIM_TEST_LOG_INFO("card state error poll error");
      break;
    case MCM_SIM_CARD_STATE_ERROR_NO_ATR_RECEIVED_V01:
      MCM_SIM_TEST_LOG_INFO("card state error no atr received");
      break;
    case MCM_SIM_CARD_STATE_ERROR_VOLT_MISMATCH_V01:
      MCM_SIM_TEST_LOG_INFO("card state error volt mismatch");
      break;
    case MCM_SIM_CARD_STATE_ERROR_PARITY_ERROR_V01:
      MCM_SIM_TEST_LOG_INFO("card state error parity error");
      break;
    case MCM_SIM_CARD_STATE_ERROR_SIM_TECHNICAL_PROBLEMS_V01:
      MCM_SIM_TEST_LOG_INFO("card state error SIM technical problems");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid card state");
      break;
  }
} /* mcm_sim_test_print_card_state */


static void mcm_sim_test_print_card_type
(
  mcm_sim_card_t_v01  card_type
)
{
  switch(card_type)
  {
    case MCM_SIM_CARD_TYPE_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("card type unknown");
      break;
    case MCM_SIM_CARD_TYPE_ICC_V01:
      MCM_SIM_TEST_LOG_INFO("card type icc");
      break;
    case MCM_SIM_CARD_TYPE_UICC_V01:
      MCM_SIM_TEST_LOG_INFO("card type uicc");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid card type");
      break;
  }
} /* mcm_sim_test_print_card_type */


static void mcm_sim_test_print_subscription
(
  mcm_sim_subscription_t_v01  subscription
)
{
  switch(subscription)
  {
    case MCM_SIM_PROV_STATE_NONE_V01:
      MCM_SIM_TEST_LOG_INFO("subscription unknown");
      break;
    case MCM_SIM_PROV_STATE_PRI_V01:
      MCM_SIM_TEST_LOG_INFO("primary subscription");
      break;
    case MCM_SIM_PROV_STATE_SEC_V01:
      MCM_SIM_TEST_LOG_INFO("secondary subscription");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid card subscription");
      break;
  }
} /* mcm_sim_test_print_subscription */


static void mcm_sim_test_print_app_state
(
  mcm_sim_app_state_t_v01  app_state
)
{
  switch(app_state)
  {
    case MCM_SIM_APP_STATE_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("app state unknown");
      break;
    case MCM_SIM_APP_STATE_DETECTED_V01:
      MCM_SIM_TEST_LOG_INFO("app state detected");
      break;
    case MCM_SIM_APP_STATE_PIN1_REQ_V01:
      MCM_SIM_TEST_LOG_INFO("app state PIN1 required");
      break;
    case MCM_SIM_APP_STATE_PUK1_REQ_V01:
      MCM_SIM_TEST_LOG_INFO("app state PUK1 required");
      break;
    case MCM_SIM_APP_STATE_INITALIZATING_V01:
      MCM_SIM_TEST_LOG_INFO("app state initalizating");
      break;
    case MCM_SIM_APP_STATE_PERSO_CK_REQ_V01:
      MCM_SIM_TEST_LOG_INFO("app state perso CK required");
      break;
    case MCM_SIM_APP_STATE_PERSO_PUK_REQ_V01:
      MCM_SIM_TEST_LOG_INFO("app state perso PUK required");
      break;
    case MCM_SIM_APP_STATE_PERSO_PERMANENTLY_BLOCKED_V01:
      MCM_SIM_TEST_LOG_INFO("app state perso perm blocked");
      break;
    case MCM_SIM_APP_STATE_PIN1_PERM_BLOCKED_V01:
      MCM_SIM_TEST_LOG_INFO("app state PIN1 perm blocked");
      break;
    case MCM_SIM_APP_STATE_ILLEGAL_V01:
      MCM_SIM_TEST_LOG_INFO("app state illegal");
      break;
    case MCM_SIM_APP_STATE_READY_V01:
      MCM_SIM_TEST_LOG_INFO("app state ready");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid app state");
      break;
  }
} /* mcm_sim_test_print_app_state */


static void mcm_sim_test_print_perso_feature
(
  mcm_sim_perso_feature_t_v01  perso_feature
)
{
  switch(perso_feature)
  {
    case MCM_SIM_PERSO_FEATURE_STATUS_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status unknown");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp network");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_SUBSET_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp network subset");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SERVICE_PROVIDER_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp service provider");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_CORPORATE_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp corporate");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SIM_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp SIM/USIM");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_1_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp2 network type 1");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_2_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp2 network type 2");
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_RUIM_V01:
      MCM_SIM_TEST_LOG_INFO("perso feature status 3gpp2 RUIM/CSIM");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid perso feature status");
      break;
  }
} /* mcm_sim_test_print_perso_feature */


static void mcm_sim_test_print_pin_state
(
  mcm_sim_pin_state_t_v01  pin_state
)
{
  switch(pin_state)
  {
    case MCM_SIM_PIN_STATE_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("pin state unknown");
      break;
    case MCM_SIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01:
      MCM_SIM_TEST_LOG_INFO("pin state enabled not verified");
      break;
    case MCM_SIM_PIN_STATE_ENABLED_VERIFIED_V01:
      MCM_SIM_TEST_LOG_INFO("pin state enabled verified");
      break;
    case MCM_SIM_PIN_STATE_DISABLED_V01:
      MCM_SIM_TEST_LOG_INFO("pin state disabled");
      break;
    case MCM_SIM_PIN_STATE_BLOCKED_V01:
      MCM_SIM_TEST_LOG_INFO("pin state blocked");
      break;
    case MCM_SIM_PIN_STATE_PERMANENTLY_BLOCKED_V01:
      MCM_SIM_TEST_LOG_INFO("pin state perm blocked");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid pin state");
      break;
  }
} /* mcm_sim_test_print_pin_state */


static void mcm_sim_test_print_app_info
(
  mcm_sim_app_info_t_v01 * app_info_ptr
)
{
  if(app_info_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_print_app_info: invalid app info pointer");
    return;
  }

  mcm_sim_test_print_subscription(app_info_ptr->subscription);
  mcm_sim_test_print_app_state(app_info_ptr->app_state);
  mcm_sim_test_print_perso_feature(app_info_ptr->perso_feature);
  MCM_SIM_TEST_LOG_INFO("pin 1:");
  mcm_sim_test_print_pin_state(app_info_ptr->pin1_state);
  MCM_SIM_TEST_LOG_INFO("pin 2:");
  mcm_sim_test_print_pin_state(app_info_ptr->pin2_state);
} /* mcm_sim_test_print_app_info */


static void mcm_sim_test_print_file_type
(
  mcm_sim_file_type_t_v01  file_type
)
{
  switch(file_type)
  {
    case MCM_SIM_FILE_TYPE_UNKNOWN_V01:
      MCM_SIM_TEST_LOG_INFO("file type unknown");
      break;
    case MCM_SIM_FILE_TYPE_TRANSPARENT_V01:
      MCM_SIM_TEST_LOG_INFO("file type transparent");
      break;
    case MCM_SIM_FILE_TYPE_CYCLIC_V01:
      MCM_SIM_TEST_LOG_INFO("file type cyclic");
      break;
    case MCM_SIM_FILE_TYPE_LINEAR_FIXED_V01:
      MCM_SIM_TEST_LOG_INFO("file type linear fixed");
      break;
    default:
      MCM_SIM_TEST_LOG_INFO("invalid file type");
      break;
  }
} /* mcm_sim_test_print_file_type */


static void mcm_sim_test_get_card_status_resp
(
  mcm_sim_get_card_status_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_card_status_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->card_info_valid)
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_card_status_resp SUCCESS");
      mcm_sim_test_print_card_state(resp_ptr->card_info.card_state);
      mcm_sim_test_print_card_type(resp_ptr->card_info.card_t);
      MCM_SIM_TEST_LOG_INFO("\n3gpp application info");
      mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_3gpp);
      MCM_SIM_TEST_LOG_INFO("\n3gpp2 application info");
      mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_3gpp2);
      MCM_SIM_TEST_LOG_INFO("\nisim application info");
      mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_isim);
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_card_status_resp: card info invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_card_status_resp: card error: 0x%x",
                           resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_card_status_resp */


static void mcm_sim_test_get_imsi_resp
(
  mcm_sim_get_subscriber_id_resp_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_imsi_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->imsi_valid)
    {
      if(resp_ptr->imsi_len > 0 &&
         resp_ptr->imsi_len <= MCM_SIM_IMSI_LEN_V01)
      {
        for(i = 0; i < resp_ptr->imsi_len; i++)
        {
          MCM_SIM_TEST_LOG_INFO("imsi[%d]: %c", i, resp_ptr->imsi[i]);
        }
        MCM_SIM_TEST_LOG_INFO("imsi length: 0x%x", resp_ptr->imsi_len);
        MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_imsi_resp: SUCCESS");
      }
      else
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_imsi_resp: invalid imsi data: 0x%x",
                               resp_ptr->imsi_len);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_imsi_resp: imsi invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_imsi_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_imsi_resp */


static void mcm_sim_test_get_iccid_resp
(
  mcm_sim_get_card_id_resp_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_iccid_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->iccid_valid)
    {
      if(resp_ptr->iccid_len > 0 &&
         resp_ptr->iccid_len <= MCM_SIM_ICCID_LEN_V01)
      {
        for(i = 0; i < resp_ptr->iccid_len; i++)
        {
          MCM_SIM_TEST_LOG_INFO("iccid[%d]: %c", i, resp_ptr->iccid[i]);
        }
        MCM_SIM_TEST_LOG_INFO("iccid length: 0x%x", resp_ptr->iccid_len);
        MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_iccid_resp: SUCCESS");
      }
      else
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_iccid_resp: invalid iccid data: 0x%x",
                               resp_ptr->iccid_len);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_iccid_resp: iccid invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_iccid_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_iccid_resp */


static void mcm_sim_test_get_phone_number_resp
(
  mcm_sim_get_device_phone_number_resp_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_phone_number_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->phone_number_valid)
    {
      if(resp_ptr->phone_number_len > 0 &&
         resp_ptr->phone_number_len <= MCM_SIM_PHONE_NUMBER_MAX_V01)
      {
        for(i = 0; i < resp_ptr->phone_number_len; i++)
        {
          MCM_SIM_TEST_LOG_INFO("phone_number[%d]: %c", i, resp_ptr->phone_number[i]);
        }
        MCM_SIM_TEST_LOG_INFO("phone_number length: 0x%x", resp_ptr->phone_number_len);
        MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_phone_number_resp: SUCCESS");
      }
      else
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_phone_number_resp: invalid phone number: 0x%x",
                               resp_ptr->phone_number_len);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_phone_number_resp: phone number invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_phone_number_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_phone_number_resp */


static void mcm_sim_test_get_plmn_list_resp
(
  mcm_sim_get_preferred_operator_list_resp_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;
  unsigned int j = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_plmn_list_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->preferred_operator_list_valid)
    {
      if(resp_ptr->preferred_operator_list_len > 0 &&
         resp_ptr->preferred_operator_list_len <= MCM_SIM_NUM_PLMN_MAX_V01)
      {
        MCM_SIM_TEST_LOG_INFO("number of plmn: 0x%x", resp_ptr->preferred_operator_list_len);
        for(i = 0; i < resp_ptr->preferred_operator_list_len; i++)
        {
          MCM_SIM_TEST_LOG_INFO("mcc[0]:%c, mcc[1]:%c, mcc[2]:%c",
                                resp_ptr->preferred_operator_list[i].mcc[0],
                                resp_ptr->preferred_operator_list[i].mcc[1],
                                resp_ptr->preferred_operator_list[i].mcc[2]);
          if(resp_ptr->preferred_operator_list[i].mnc_len > MCM_SIM_MNC_MAX_V01)
          {
            MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_plmn_list_resp: invalid mnc len: 0x%x",
                                   resp_ptr->preferred_operator_list[i].mnc_len);
          }
          for(j = 0; j < resp_ptr->preferred_operator_list[i].mnc_len; j++)
          {
            MCM_SIM_TEST_LOG_INFO("mnc[%d]:%c",j,resp_ptr->preferred_operator_list[i].mnc[j]);
          }
        }
        MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_plmn_list_resp: SUCCESS");
      }
      else
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_plmn_list_resp: invalid number of plmn: 0x%x",
                               resp_ptr->preferred_operator_list_len);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_plmn_list_resp: plmn list invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_plmn_list_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_plmn_list_resp */


static void mcm_sim_test_read_file_resp
(
  mcm_sim_read_file_resp_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_read_file_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->data_valid)
    {
      if(resp_ptr->data_len > 0 &&
         resp_ptr->data_len <= MCM_SIM_DATA_MAX_V01)
      {
        for(i = 0; i < resp_ptr->data_len; i++)
        {
          MCM_SIM_TEST_LOG_INFO("data[%d]: 0x%x", i, resp_ptr->data[i]);
        }
        MCM_SIM_TEST_LOG_INFO("data length: 0x%x", resp_ptr->data_len);
        MCM_SIM_TEST_LOG_INFO("mcm_sim_test_read_file_resp: SUCCESS");
      }
      else
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_read_file_resp: invalid read data: 0x%x",
                               resp_ptr->data_len);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_read_file_resp: read data invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_read_file_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_read_file_resp */


static void mcm_sim_test_write_file_resp
(
  mcm_sim_write_file_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_write_file_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_write_file_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_write_file_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_write_file_resp */


static void mcm_sim_test_get_file_size_resp
(
  mcm_sim_get_file_size_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_file_size_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_ptr->file_info_valid)
    {
      mcm_sim_test_print_file_type(resp_ptr->file_info.file_t);
      if(resp_ptr->file_info.record_count)
      {
        MCM_SIM_TEST_LOG_INFO("record size: 0x%x", resp_ptr->file_info.record_size);
        MCM_SIM_TEST_LOG_INFO("record count: 0x%x", resp_ptr->file_info.record_count);
      }
      else
      {
        MCM_SIM_TEST_LOG_INFO("transparent file size: 0x%x", resp_ptr->file_info.file_size);
      }
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_file_size_resp: SUCCESS");
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_file_size_resp: file info invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_file_size_resp: ERROR: 0x%x", resp_ptr->resp.error);
  }
} /* mcm_sim_test_get_file_size_resp */


static void mcm_sim_test_verify_pin_resp
(
  mcm_sim_verify_pin_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_verify_pin_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_verify_pin_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_verify_pin_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of retires left: 0x%x", resp_ptr->retries_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_verify_pin_resp: retries invalid");
    }
  }
} /* mcm_sim_test_verify_pin_resp */


static void mcm_sim_test_change_pin_resp
(
  mcm_sim_change_pin_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_change_pin_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_change_pin_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_change_pin_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of retires left: 0x%x", resp_ptr->retries_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_change_pin_resp: retries invalid");
    }
  }
} /* mcm_sim_test_change_pin_resp */


static void mcm_sim_test_unblock_pin_resp
(
  mcm_sim_unblock_pin_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_unblock_pin_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_unblock_pin_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_unblock_pin_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of retires left: 0x%x", resp_ptr->retries_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_unblock_pin_resp: retries invalid");
    }
  }
} /* mcm_sim_test_unblock_pin_resp */


static void mcm_sim_test_enable_pin_resp
(
  mcm_sim_enable_pin_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_enable_pin_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_enable_pin_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_enable_pin_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of retires left: 0x%x", resp_ptr->retries_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_enable_pin_resp: retries invalid");
    }
  }
} /* mcm_sim_test_enable_pin_resp */


static void mcm_sim_test_disable_pin_resp
(
  mcm_sim_disable_pin_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_disable_pin_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_disable_pin_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_disable_pin_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of retires left: 0x%x", resp_ptr->retries_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_disable_pin_resp: retries invalid");
    }
  }
} /* mcm_sim_test_disable_pin_resp */


static void mcm_sim_test_depersonalization_resp
(
  mcm_sim_depersonalization_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_depersonalization_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_depersonalization_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_depersonalization_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of verify retires left: 0x%x", resp_ptr->retries_left.verify_left);
      MCM_SIM_TEST_LOG_INFO("number of unblock retires left: 0x%x", resp_ptr->retries_left.unblock_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_depersonalization_resp: retries invalid");
    }
  }
} /* mcm_sim_test_depersonalization_resp */


static void mcm_sim_test_personalization_resp
(
  mcm_sim_personalization_resp_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_personalization_resp RESPONSE ENTER");

  if(resp_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_personalization_resp: SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_personalization_resp: ERROR: 0x%x", resp_ptr->resp.error);
    if(resp_ptr->retries_left_valid)
    {
      MCM_SIM_TEST_LOG_INFO("number of verify retires left: 0x%x", resp_ptr->retries_left.verify_left);
      MCM_SIM_TEST_LOG_INFO("number of unblock retires left: 0x%x", resp_ptr->retries_left.unblock_left);
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_personalization_resp: retries invalid");
    }
  }
} /* mcm_sim_test_personalization_resp */


static void mcm_sim_test_card_status_ind
(
  mcm_sim_card_status_event_ind_msg_v01 * resp_ptr
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_card_status_ind RESPONSE ENTER");

  if(resp_ptr->slot_id_valid == TRUE)
  {
    MCM_SIM_TEST_LOG_INFO("card status indication from slot: 0x%x", resp_ptr->slot_id);
  }

  if(resp_ptr->card_info_valid == TRUE)
  {
    mcm_sim_test_print_card_state(resp_ptr->card_info.card_state);
    mcm_sim_test_print_card_type(resp_ptr->card_info.card_t);
    MCM_SIM_TEST_LOG_INFO("\n3gpp application info");
    mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_3gpp);
    MCM_SIM_TEST_LOG_INFO("\n3gpp2 application info");
    mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_3gpp2);
    MCM_SIM_TEST_LOG_INFO("\nisim application info");
    mcm_sim_test_print_app_info(&resp_ptr->card_info.card_app_info.app_isim);
  }
} /* mcm_sim_test_card_status_ind */


static void mcm_sim_test_refresh_ind
(
  mcm_sim_refresh_event_ind_msg_v01 * resp_ptr
)
{
  unsigned int i = 0;
  unsigned int j = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_refresh_ind RESPONSE ENTER");

  if(resp_ptr->refresh_event_valid == TRUE)
  {
    MCM_SIM_TEST_LOG_INFO("slot id: 0x%x", resp_ptr->refresh_event.app_info.slot_id);
    MCM_SIM_TEST_LOG_INFO("app type: 0x%x", resp_ptr->refresh_event.app_info.app_t);
    MCM_SIM_TEST_LOG_INFO("refresh mode: 0x%x", resp_ptr->refresh_event.refresh_mode);

    if(resp_ptr->refresh_event.refresh_files_len > 0 &&
       resp_ptr->refresh_event.refresh_files_len <= MCM_SIM_MAX_REFRESH_FILES_V01)
    {
      for(i = 0; i < resp_ptr->refresh_event.refresh_files_len; i++)
      {
        if(resp_ptr->refresh_event.refresh_files[i].path_value_len > 0 &&
           resp_ptr->refresh_event.refresh_files[i].path_value_len <= MCM_SIM_CHAR_PATH_MAX_V01)
        {
          for(j = 0; j < resp_ptr->refresh_event.refresh_files[i].path_value_len; j++)
          {
            MCM_SIM_TEST_LOG_INFO("path[%d]: %c", j,
                                  resp_ptr->refresh_event.refresh_files[i].path_value[j]);
          }
        }
        MCM_SIM_TEST_LOG_INFO("refresh size of path: 0x%x",
                              resp_ptr->refresh_event.refresh_files[i].path_value_len);
      }
      MCM_SIM_TEST_LOG_INFO("refresh number of files: 0x%x",
                            resp_ptr->refresh_event.refresh_files_len);
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_refresh_ind: SUCCESS");
    }
  }
} /* mcm_sim_test_refresh_ind */


static void mcm_sim_test_response_main
(
  uint32_t                  message_id,
  void                    * resp_data_ptr,
  uint32_t                  resp_data_len
)
{
  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_response_main ENTER");

  if(resp_data_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid input");
    return;
  }

  switch(message_id)
  {
    case MCM_SIM_GET_CARD_STATUS_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_card_status_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_card_status_resp((mcm_sim_get_card_status_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_GET_SUBSCRIBER_ID_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_subscriber_id_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_imsi_resp((mcm_sim_get_subscriber_id_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_GET_CARD_ID_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_card_id_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_iccid_resp((mcm_sim_get_card_id_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_GET_DEVICE_PHONE_NUMBER_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_device_phone_number_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_phone_number_resp((mcm_sim_get_device_phone_number_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_GET_PREFERRED_OPERATOR_LIST_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_plmn_list_resp((mcm_sim_get_preferred_operator_list_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_READ_FILE_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_read_file_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_read_file_resp((mcm_sim_read_file_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_WRITE_FILE_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_write_file_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_write_file_resp((mcm_sim_write_file_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_GET_FILE_SIZE_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_get_file_size_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_get_file_size_resp((mcm_sim_get_file_size_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_VERIFY_PIN_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_verify_pin_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_verify_pin_resp((mcm_sim_verify_pin_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_CHANGE_PIN_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_change_pin_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_change_pin_resp((mcm_sim_change_pin_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_UNBLOCK_PIN_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_unblock_pin_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_unblock_pin_resp((mcm_sim_unblock_pin_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_ENABLE_PIN_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_enable_pin_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_enable_pin_resp((mcm_sim_enable_pin_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_DISABLE_PIN_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_disable_pin_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_disable_pin_resp((mcm_sim_disable_pin_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_DEPERSONALIZATION_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_depersonalization_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_depersonalization_resp((mcm_sim_depersonalization_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_PERSONALIZATION_RESP_V01:
      if(resp_data_len != sizeof(mcm_sim_personalization_resp_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_personalization_resp((mcm_sim_personalization_resp_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_CARD_STATUS_EVENT_IND_V01:
      if(resp_data_len != sizeof(mcm_sim_card_status_event_ind_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_card_status_ind((mcm_sim_card_status_event_ind_msg_v01*)resp_data_ptr);
      break;
    case MCM_SIM_REFRESH_EVENT_IND_V01:
      if(resp_data_len != sizeof(mcm_sim_refresh_event_ind_msg_v01))
      {
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid data length");
        break;
      }
      mcm_sim_test_refresh_ind((mcm_sim_refresh_event_ind_msg_v01*)resp_data_ptr);
      break;
    default:
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_response_main: invalid message id: 0x%x:", (unsigned int)message_id);
      break;
  }

  sim_test_remove_event(resp_data_ptr);
} /* mcm_sim_test_response_main */


static void mcm_sim_test_get_card_status
(
  void
)
{
  mcm_sim_get_card_status_req_msg_v01    req_msg;
  mcm_sim_get_card_status_resp_msg_v01 * resp_msg_ptr = NULL;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_card_status REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_card_status_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_card_status_resp_msg_v01));
  if(resp_msg_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_card_status: insufficient memory to allocate");
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.slot_id = mcm_sim_test_util_get_slot_id_input();

  MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                  MCM_SIM_GET_CARD_STATUS_REQ_V01,
                                  &req_msg,
                                  resp_msg_ptr);

  if(resp_msg_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    if(resp_msg_ptr->card_info_valid)
    {
      MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_card_status SUCCESS");
      mcm_sim_test_print_card_state(resp_msg_ptr->card_info.card_state);
      mcm_sim_test_print_card_type(resp_msg_ptr->card_info.card_t);
      MCM_SIM_TEST_LOG_INFO("\n3gpp application info");
      mcm_sim_test_print_app_info(&resp_msg_ptr->card_info.card_app_info.app_3gpp);
      MCM_SIM_TEST_LOG_INFO("\n3gpp2 application info");
      mcm_sim_test_print_app_info(&resp_msg_ptr->card_info.card_app_info.app_3gpp2);
      MCM_SIM_TEST_LOG_INFO("\nisim application info");
      mcm_sim_test_print_app_info(&resp_msg_ptr->card_info.card_app_info.app_isim);
    }
    else
    {
      MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_card_status: card info invalid");
    }
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_get_card_status: card error: 0x%x",
                           resp_msg_ptr->resp.error);
  }

  util_memory_free((void**) &resp_msg_ptr);
} /* mcm_sim_test_get_card_status */


static void mcm_sim_test_get_imsi
(
  void
)
{
  mcm_sim_get_subscriber_id_req_msg_v01    req_msg;
  mcm_sim_get_subscriber_id_resp_msg_v01 * resp_msg_ptr = NULL;
  int                                      token_id     = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_imsi REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_subscriber_id_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_subscriber_id_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_GET_SUBSCRIBER_ID_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_get_imsi */


static void mcm_sim_test_get_iccid
(
  void
)
{
  mcm_sim_get_card_id_req_msg_v01          req_msg;
  mcm_sim_get_card_id_resp_msg_v01       * resp_msg_ptr = NULL;
  int                                      token_id     = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_iccid REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_card_id_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_card_id_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.slot_id = mcm_sim_test_util_get_slot_id_input();

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_GET_CARD_ID_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_get_iccid */


static void mcm_sim_test_get_phone_number
(
  void
)
{
  mcm_sim_get_device_phone_number_req_msg_v01    req_msg;
  mcm_sim_get_device_phone_number_resp_msg_v01 * resp_msg_ptr = NULL;
  int                                            token_id     = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_phone_number REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_device_phone_number_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_device_phone_number_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_GET_DEVICE_PHONE_NUMBER_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_get_phone_number */


static void mcm_sim_test_get_plmn_list
(
  void
)
{
  mcm_sim_get_preferred_operator_list_req_msg_v01    req_msg;
  mcm_sim_get_preferred_operator_list_resp_msg_v01 * resp_msg_ptr = NULL;
  int                                                token_id     = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_plmn_list REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_preferred_operator_list_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.slot_id = mcm_sim_test_util_get_slot_id_input();

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_GET_PREFERRED_OPERATOR_LIST_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_get_plmn_list */


static void mcm_sim_test_read_file
(
  void
)
{
  mcm_sim_read_file_req_msg_v01            req_msg;
  mcm_sim_read_file_resp_msg_v01         * resp_msg_ptr = NULL;
  int                                      token_id     = 0;
  char                                     path_string[MCM_SIM_CHAR_PATH_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_read_file REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_read_file_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_read_file_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();

  req_msg.file_access.offset = 0;
  req_msg.file_access.record_num = mcm_sim_test_util_get_record_num_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter Path: ");
  scanf("%21s", path_string);
  req_msg.file_access.path_len = strlen(path_string);
  memcpy(req_msg.file_access.path,
         path_string,
         req_msg.file_access.path_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_READ_FILE_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_read_file */


static void mcm_sim_test_write_file
(
  void
)
{
  mcm_sim_write_file_req_msg_v01            req_msg;
  mcm_sim_write_file_resp_msg_v01         * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      path_string[MCM_SIM_CHAR_PATH_MAX_V01 + 1];
  uint8_t                                   data_array[] = {1,1,0,0,0,1,9,8,7,6,5,4,3,2,1,0};

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_write_file REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_write_file_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_write_file_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();

  req_msg.file_access.offset = 0;
  req_msg.file_access.record_num = mcm_sim_test_util_get_record_num_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter Path: ");
  scanf("%21s", path_string);
  req_msg.file_access.path_len = strlen(path_string);
  memcpy(req_msg.file_access.path,
         path_string,
         req_msg.file_access.path_len);

  req_msg.data_len = sizeof(data_array);
  memcpy(req_msg.data,
         data_array,
         req_msg.data_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_WRITE_FILE_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_write_file */


static void mcm_sim_test_get_file_size
(
  void
)
{
  mcm_sim_get_file_size_req_msg_v01         req_msg;
  mcm_sim_get_file_size_resp_msg_v01      * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      path_string[MCM_SIM_CHAR_PATH_MAX_V01];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_file_size REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_get_file_size_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_get_file_size_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter Path: ");
  scanf("%21s", path_string);
  req_msg.path_len = strlen(path_string);
  memcpy(req_msg.path,
         path_string,
         req_msg.path_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_GET_FILE_SIZE_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_get_file_size */


static void mcm_sim_test_verify_pin
(
  void
)
{
  mcm_sim_verify_pin_req_msg_v01            req_msg;
  mcm_sim_verify_pin_resp_msg_v01         * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      pin_string[MCM_SIM_PIN_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_verify_pin REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_verify_pin_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_verify_pin_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();
  req_msg.pin_id = mcm_sim_test_util_get_pin_id_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter PIN: ");
  scanf("%9s", pin_string);
  req_msg.pin_value_len = strlen(pin_string);
  memcpy(req_msg.pin_value,
         pin_string,
         req_msg.pin_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_VERIFY_PIN_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_verify_pin */


static void mcm_sim_test_change_pin
(
  void
)
{
  mcm_sim_change_pin_req_msg_v01            req_msg;
  mcm_sim_change_pin_resp_msg_v01         * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      old_pin_string[MCM_SIM_PIN_MAX_V01 + 1];
  char                                      new_pin_string[MCM_SIM_PIN_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_get_card_status REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_change_pin_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_change_pin_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();
  req_msg.pin_id = mcm_sim_test_util_get_pin_id_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter Old PIN: ");
  scanf("%9s", old_pin_string);
  req_msg.old_pin_value_len = strlen(old_pin_string);
  memcpy(req_msg.old_pin_value,
         old_pin_string,
         req_msg.old_pin_value_len);

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter New PIN: ");
  scanf("%9s", new_pin_string);
  req_msg.new_pin_value_len = strlen(new_pin_string);
  memcpy(req_msg.new_pin_value,
         new_pin_string,
         req_msg.new_pin_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_CHANGE_PIN_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_change_pin */


static void mcm_sim_test_unblock_pin
(
  void
)
{
  mcm_sim_unblock_pin_req_msg_v01           req_msg;
  mcm_sim_unblock_pin_resp_msg_v01        * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      puk_string[MCM_SIM_PIN_MAX_V01 + 1];
  char                                      new_pin_string[MCM_SIM_PIN_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_unblock_pin REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_unblock_pin_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_unblock_pin_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();
  req_msg.pin_id = mcm_sim_test_util_get_pin_id_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter PUK: ");
  scanf("%9s", puk_string);
  req_msg.puk_value_len = strlen(puk_string);
  memcpy(req_msg.puk_value,
         puk_string,
         req_msg.puk_value_len);

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter New PIN: ");
  scanf("%9s", new_pin_string);
  req_msg.new_pin_value_len = strlen(new_pin_string);
  memcpy(req_msg.new_pin_value,
         new_pin_string,
         req_msg.new_pin_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_UNBLOCK_PIN_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_unblock_pin */


static void mcm_sim_test_enable_pin
(
  void
)
{
  mcm_sim_enable_pin_req_msg_v01            req_msg;
  mcm_sim_enable_pin_resp_msg_v01         * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      pin_string[MCM_SIM_PIN_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_enable_pin REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_enable_pin_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_enable_pin_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();
  req_msg.pin_id = mcm_sim_test_util_get_pin_id_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter PIN: ");
  scanf("%9s", pin_string);
  req_msg.pin_value_len = strlen(pin_string);
  memcpy(req_msg.pin_value,
         pin_string,
         req_msg.pin_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_ENABLE_PIN_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_enable_pin */


static void mcm_sim_test_disable_pin
(
  void
)
{
  mcm_sim_disable_pin_req_msg_v01           req_msg;
  mcm_sim_disable_pin_resp_msg_v01        * resp_msg_ptr = NULL;
  int                                       token_id     = 0;
  char                                      pin_string[MCM_SIM_PIN_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_disable_pin REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_disable_pin_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_disable_pin_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.app_info.slot_id = mcm_sim_test_util_get_slot_id_input();
  req_msg.app_info.app_t = mcm_sim_test_util_get_app_type_input();
  req_msg.pin_id = mcm_sim_test_util_get_pin_id_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter PIN: ");
  scanf("%9s", pin_string);
  req_msg.pin_value_len = strlen(pin_string);
  memcpy(req_msg.pin_value,
         pin_string,
         req_msg.pin_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_DISABLE_PIN_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_disable_pin */


static void mcm_sim_test_depersonalization
(
  void
)
{
  mcm_sim_depersonalization_req_msg_v01      req_msg;
  mcm_sim_depersonalization_resp_msg_v01   * resp_msg_ptr = NULL;
  int                                        token_id     = 0;
  char                                       dck_string[MCM_SIM_CK_MAX_V01 + 1];

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_depersonalization REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_depersonalization_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_depersonalization_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_depersonalization: insufficient memory to allocate");
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.depersonalization.feature = mcm_sim_test_util_get_perso_feature_input();
  req_msg.depersonalization.operation = mcm_sim_test_util_get_perso_operation_input();

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter DCK: ");
  scanf("%17s", dck_string);
  req_msg.depersonalization.ck_value_len = strlen(dck_string);
  memcpy(req_msg.depersonalization.ck_value,
         dck_string,
         req_msg.depersonalization.ck_value_len);

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_DEPERSONALIZATION_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_depersonalization */


static void mcm_sim_test_personalization
(
  void
)
{
  mcm_sim_personalization_req_msg_v01        req_msg;
  mcm_sim_personalization_resp_msg_v01     * resp_msg_ptr = NULL;
  int                                        token_id     = 0;
  char                                       dck_string[MCM_SIM_CK_MAX_V01 + 1];
  char                                       irm_string[MCM_SIM_IRM_CODE_LEN_V01 + 1];
  char                                       msin_string[MCM_SIM_MSIN_MAX_V01 + 1];
  mcm_sim_perso_feature_t_v01                feature;
  uint32_t                                   i            = 0;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_personalization REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_personalization_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_personalization_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_personalization: insufficient memory to allocate");
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  MCM_SIM_TEST_LOG_VERBOSE("\nEnter DCK: ");
  scanf("%17s", dck_string);
  req_msg.ck_value_len = strlen(dck_string);
  memcpy(req_msg.ck_value,
         dck_string,
         req_msg.ck_value_len);

  feature = mcm_sim_test_util_get_perso_feature_input();

  switch(feature)
  {
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_V01:
      req_msg.feature_gw_network_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of gw network perso data sets: ");
      req_msg.feature_gw_network_perso_len = mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_gw_network_perso_len && i < MCM_SIM_PERSO_NUM_NW_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_gw_network_perso[i]);
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_SUBSET_V01:
      req_msg.feature_gw_network_subset_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of network subset perso data sets: ");
      req_msg.feature_gw_network_subset_perso_len = mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_gw_network_subset_perso_len && i < MCM_SIM_PERSO_NUM_NS_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_gw_network_subset_perso[i].network);
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter digit 6: ");
        req_msg.feature_gw_network_subset_perso[i].digit6 = mcm_sim_test_util_get_char_number_input();
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter digit 7: ");
        req_msg.feature_gw_network_subset_perso[i].digit7 = mcm_sim_test_util_get_char_number_input();
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SERVICE_PROVIDER_V01:
      req_msg.feature_gw_sp_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of service provider perso data sets: ");
      req_msg.feature_gw_sp_perso_len = mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_gw_sp_perso_len && i < MCM_SIM_PERSO_NUM_GW_SP_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_gw_sp_perso[i].network);
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter gid1: ");
        req_msg.feature_gw_sp_perso[i].gid1 = mcm_sim_test_util_get_bin_number_input();
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_CORPORATE_V01:
      req_msg.feature_gw_corporate_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of corporate perso data sets: ");
      req_msg.feature_gw_corporate_perso_len = mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_gw_corporate_perso_len && i < MCM_SIM_PERSO_NUM_GW_CP_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_gw_corporate_perso[i].network);
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter gid1: ");
        req_msg.feature_gw_corporate_perso[i].gid1 = mcm_sim_test_util_get_bin_number_input();
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter gid2: ");
        req_msg.feature_gw_corporate_perso[i].gid2 = mcm_sim_test_util_get_bin_number_input();
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SIM_V01:
      req_msg.feature_gw_sim_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of SIM perso data sets: ");
      req_msg.feature_gw_sim_perso_len = (uint32_t)mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_gw_sim_perso_len && i < MCM_SIM_PERSO_NUM_SIM_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_gw_sim_perso[i].network);

        MCM_SIM_TEST_LOG_VERBOSE("\nEnter MSIN: ");
        scanf("%11s", msin_string);
        req_msg.feature_gw_sim_perso[i].msin_len = strlen(msin_string);
        memcpy(req_msg.feature_gw_sim_perso[i].msin,
               msin_string,
               req_msg.feature_gw_sim_perso[i].msin_len);
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_1_V01:
      req_msg.feature_1x_network1_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of 1x network perso data sets: ");
      req_msg.feature_1x_network1_perso_len = mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_1x_network1_perso_len && i < MCM_SIM_PERSO_NUM_NW_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_1x_network1_perso[i]);
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_2_V01:
      req_msg.feature_1x_network2_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of SIM perso data sets: ");
      req_msg.feature_1x_network2_perso_len = (uint32_t)mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_1x_network2_perso_len && i < MCM_SIM_PERSO_NUM_1X_NW2_MAX_V01; i++)
      {
        MCM_SIM_TEST_LOG_VERBOSE("\nEnter IRM code: ");
        scanf("%5s", irm_string);
        memcpy(req_msg.feature_1x_network2_perso[i].irm_code,
               irm_string,
               strlen(irm_string));
      }
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_RUIM_V01:
      req_msg.feature_1x_ruim_perso_valid = 1;
      MCM_SIM_TEST_LOG_VERBOSE("\nEnter number of SIM perso data sets: ");
      req_msg.feature_1x_ruim_perso_len = (uint32_t)mcm_sim_test_util_get_bin_number_input();
      for(i = 0; i < req_msg.feature_1x_ruim_perso_len && i < MCM_SIM_PERSO_NUM_SIM_MAX_V01; i++)
      {
        mcm_sim_test_util_get_perso_network_data_input(&req_msg.feature_1x_ruim_perso[i].network);

        MCM_SIM_TEST_LOG_VERBOSE("\nEnter MSIN: ");
        scanf("%11s", msin_string);
        req_msg.feature_gw_sim_perso[i].msin_len = strlen(msin_string);
        memcpy(req_msg.feature_gw_sim_perso[i].msin,
               msin_string,
               req_msg.feature_gw_sim_perso[i].msin_len);
      }
      break;
    default:
      MCM_SIM_TEST_LOG_ERROR("Invalid feature selected");
      return;
  }

  MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                   MCM_SIM_PERSONALIZATION_REQ_V01,
                                   &req_msg,
                                   resp_msg_ptr,
                                   mcm_async_cb,
                                   &token_id);
} /* mcm_sim_test_personalization */


static void mcm_sim_test_event_register
(
  void
)
{
  mcm_sim_event_register_req_msg_v01     req_msg;
  mcm_sim_event_register_resp_msg_v01  * resp_msg_ptr = NULL;

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_event_register REQEUST ENTER");

  resp_msg_ptr = (mcm_sim_event_register_resp_msg_v01*)
                    util_memory_alloc(sizeof(mcm_sim_event_register_resp_msg_v01));

  if(resp_msg_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_event_register: insufficient memory to allocate");
    return;
  }

  memset(&req_msg,0x00,sizeof(req_msg));
  memset(resp_msg_ptr,0x00,sizeof(*resp_msg_ptr));

  req_msg.register_card_status_event_valid = TRUE;
  MCM_SIM_TEST_LOG_INFO("\nRegister for card status event");
  req_msg.register_card_status_event = mcm_sim_test_util_event_reg();

  req_msg.register_refresh_event_valid = TRUE;
  MCM_SIM_TEST_LOG_INFO("\nRegister for refresh event");
  req_msg.register_refresh_event = mcm_sim_test_util_event_reg();

  MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                  MCM_SIM_EVENT_REGISTER_REQ_V01,
                                  &req_msg,
                                  resp_msg_ptr);

  if(resp_msg_ptr->resp.result == MCM_RESULT_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_INFO("mcm_sim_test_event_register SUCCESS");
  }
  else
  {
    MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_event_register: card error: 0x%x",
                           resp_msg_ptr->resp.error);
  }

  util_memory_free((void**) &resp_msg_ptr);
} /* mcm_sim_test_event_register */


int mcm_sim_test_main_menu
(
  void
)
{
  char                  input         = 0;

  while(1)
  {
    sleep(1);

    MCM_SIM_TEST_LOG_VERBOSE("\n\n========= Main Menu =========");
    MCM_SIM_TEST_LOG_VERBOSE("\n1 : To retrieve card status");
    MCM_SIM_TEST_LOG_VERBOSE("\n2 : To retrieve IMSI");
    MCM_SIM_TEST_LOG_VERBOSE("\n3 : To retrieve Card ID");
    MCM_SIM_TEST_LOG_VERBOSE("\n4 : To retrieve Phone Number");
    MCM_SIM_TEST_LOG_VERBOSE("\n5 : To retrieve preferred operator list");
    MCM_SIM_TEST_LOG_VERBOSE("\n6 : To read file");
    MCM_SIM_TEST_LOG_VERBOSE("\n7 : To write file");
    MCM_SIM_TEST_LOG_VERBOSE("\n8 : To retrieve file size");
    MCM_SIM_TEST_LOG_VERBOSE("\n9 : To verify PIN");
    MCM_SIM_TEST_LOG_VERBOSE("\na : To change PIN");
    MCM_SIM_TEST_LOG_VERBOSE("\nb : To unblock PIN");
    MCM_SIM_TEST_LOG_VERBOSE("\nc : To enable PIN");
    MCM_SIM_TEST_LOG_VERBOSE("\nd : To disable PIN");
    MCM_SIM_TEST_LOG_VERBOSE("\ne : To depersonalize");
    MCM_SIM_TEST_LOG_VERBOSE("\nf : To personalize");
    MCM_SIM_TEST_LOG_VERBOSE("\ng : To register for unsol event");
    MCM_SIM_TEST_LOG_VERBOSE("\nq : quit");
    MCM_SIM_TEST_LOG_VERBOSE("=============================");
    MCM_SIM_TEST_LOG_VERBOSE("Enter : ");

    while((input = getchar()) == '\n');

    MCM_SIM_TEST_LOG_ERROR("input: %c", input);

    switch(input)
    {
      case '1' :
        mcm_sim_test_get_card_status();
        break;
      case '2' :
        mcm_sim_test_get_imsi();
        break;
      case '3' :
        mcm_sim_test_get_iccid();
        break;
      case '4' :
        mcm_sim_test_get_phone_number();
        break;
      case '5' :
        mcm_sim_test_get_plmn_list();
        break;
      case '6' :
        mcm_sim_test_read_file();
        break;
      case '7' :
        mcm_sim_test_write_file();
        break;
      case '8' :
        mcm_sim_test_get_file_size();
        break;
      case '9' :
        mcm_sim_test_verify_pin();
        break;
      case 'a' :
        mcm_sim_test_change_pin();
        break;
      case 'b' :
        mcm_sim_test_unblock_pin();
        break;
      case 'c' :
        mcm_sim_test_enable_pin();
        break;
      case 'd' :
        mcm_sim_test_disable_pin();
        break;
      case 'e' :
        mcm_sim_test_depersonalization();
        break;
      case 'f' :
        mcm_sim_test_personalization();
        break;
      case 'g' :
        mcm_sim_test_event_register();
        break;
      case 'q' :
        return -1;
      default :
        MCM_SIM_TEST_LOG_ERROR("mcm_sim_test_main_menu: invalid input character: %c", input);
        return mcm_sim_test_main_menu();
    }
  }

  return 0;
} /* mcm_sim_test_main_menu */


static int mcm_sim_test_queue_add
(
  util_list_node_data_type *to_be_added_data,
  util_list_node_data_type *to_be_evaluated_data
)
{
  return FALSE;
} /* mcm_sim_test_queue_add */


static void mcm_sim_test_queue_delete
(
  util_list_node_data_type *to_be_deleted_data
)
{
  if(to_be_deleted_data && to_be_deleted_data->user_data)
  {
    util_memory_free((void**) &to_be_deleted_data->user_data);
  }
} /* mcm_sim_test_queue_delete */


static int mcm_sim_test_start
(
  void
)
{
  int err_code;

  err_code = ENOMEM;

  sim_test_queue = util_list_create(NULL,
                                    mcm_sim_test_queue_add,
                                    mcm_sim_test_queue_delete,
                                    UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP |
                                    UTIL_LIST_BIT_FIELD_USE_COND_VAR);

  if(sim_test_queue)
  {
    err_code = pthread_create(&sim_test_thread,
                              NULL,
                              sim_test_thread_proc,
                              NULL);
    if(err_code)
    {
      util_list_cleanup(sim_test_queue,
                        NULL);
    }
  }

  MCM_SIM_TEST_LOG_INFO("mcm_sim_test_state: status %d", err_code);

  return err_code;
} /* mcm_sim_test_start */


int main()
{
  mcm_error_t_v01 mcm_status = MCM_SUCCESS_V01;

  mcm_status = mcm_client_init(&mcm_client_handle, mcm_ind_cb, mcm_async_cb);

  MCM_SIM_TEST_LOG_INFO("\n\n*** MCM UIM test console ***");

  if (mcm_status != MCM_SUCCESS_CONDITIONAL_SUCCESS_V01 &&
      mcm_status != MCM_SUCCESS_V01)
  {
    MCM_SIM_TEST_LOG_ERROR("ERROR: mcm client init failed");
    return -1;
  }

  mcm_sim_test_start();
  mcm_sim_test_main_menu();

  return 0;
}


int sim_test_queue_find_for_processing_evaluator
(
  util_list_node_data_type *to_be_found_data_ptr
)
{
  int                       ret           = FALSE;
  sim_test_resp_data_type * resp_data_ptr = NULL;

  if((to_be_found_data_ptr != NULL) && (to_be_found_data_ptr->user_data))
  {
    resp_data_ptr = (sim_test_resp_data_type*) to_be_found_data_ptr->user_data;
    if(FALSE == resp_data_ptr->is_processed)
    {
      ret = TRUE;
    }
  }

  return ret;
} /* sim_test_queue_find_for_processing_evaluator */


int sim_test_add_event
(
  uint32_t                message_id,
  void                  * resp_data,
  uint32_t                resp_data_len
)
{
  int                       ret_code               = ENOMEM;
  sim_test_resp_data_type * resp_data_ptr          = NULL;

  resp_data_ptr = util_memory_alloc(sizeof(sim_test_resp_data_type));
  if(resp_data_ptr == NULL)
  {
    MCM_SIM_TEST_LOG_INFO("sim_test_add_event: failed to allocate memory");
    return ENOMEM;
  }
  memset(resp_data_ptr, 0x00, sizeof(sim_test_resp_data_type));

  if(resp_data_ptr != NULL)
  {
    resp_data_ptr->is_processed    = FALSE;
    resp_data_ptr->msg_id          = message_id;
    resp_data_ptr->resp_data       = resp_data;
    resp_data_ptr->resp_data_len   = resp_data_len;

    util_list_lock_list(sim_test_queue);
    ret_code = util_list_add(sim_test_queue,
                             resp_data_ptr,
                             NULL,
                             NIL);

    MCM_SIM_TEST_LOG_INFO("sim_test_add_event: ADDED resp_data %p, error %d",
                          resp_data,
                          ret_code);

    if(ret_code)
    {
      MCM_SIM_TEST_LOG_INFO("sim_test_add_event: ERROR adding to queue: 0x%x", ret_code);
      util_memory_free((void**) &resp_data_ptr);
    }
    else
    {
      util_list_signal_for_list(sim_test_queue);
    }

    util_list_unlock_list(sim_test_queue);
  }

  return ret_code;
} /* sim_test_add_event */


int sim_test_remove_event
(
  void * event_data_to_be_removed
)
{
  int                        is_match              = FALSE;
  int                        ret_code              = EINVAL;;
  util_list_node_data_type * temp_node_data_ptr    = NULL;
  sim_test_resp_data_type  * resp_data_ptr         = NULL;

  util_list_lock_list(sim_test_queue);

  temp_node_data_ptr = util_list_retrieve_head(sim_test_queue);

  while(FALSE == is_match && temp_node_data_ptr != NULL)
  {
    if(temp_node_data_ptr->user_data)
    {
      resp_data_ptr = (sim_test_resp_data_type*) temp_node_data_ptr->user_data;
      if(event_data_to_be_removed == resp_data_ptr->resp_data)
      {
        is_match = TRUE;
        util_list_delete(sim_test_queue,
                         temp_node_data_ptr,
                         NULL);
        util_list_signal_for_list(sim_test_queue);
      }
      else
      {
        temp_node_data_ptr = util_list_retrieve_successor(sim_test_queue,
                                                          temp_node_data_ptr);
      }
    }
    else
    {
      MCM_SIM_TEST_LOG_INFO("unexpected : user_data is NULL");
    }
  }
  util_list_unlock_list(sim_test_queue);

  if(TRUE == is_match)
  {
    ret_code = ESUCCESS;
  }

  MCM_SIM_TEST_LOG_INFO("sim_test_remove_event: REMOVED event data %p, status %d",
                        event_data_to_be_removed,
                        ret_code);

  return ret_code;
} /* sim_test_remove_event */


void* sim_test_thread_proc
(
  void * sim_test_thread_proc_param
)
{
  util_list_node_data_type   * node_data_ptr       = NULL;
  sim_test_resp_data_type    * resp_data_ptr       = NULL;

  MCM_SIM_TEST_LOG_INFO("ENTER sim_test_thread_proc");

  while(1)
  {
    util_list_lock_list(sim_test_queue);

    resp_data_ptr = NULL;
    while(NULL == resp_data_ptr)
    {
      node_data_ptr = util_list_find(sim_test_queue,
                                     sim_test_queue_find_for_processing_evaluator);

      if(node_data_ptr == NULL)
      {
        MCM_SIM_TEST_LOG_INFO("no unprocessed entries in core queue");
        util_list_wait_on_list(sim_test_queue, NIL);
      }
      else
      {
        resp_data_ptr = (sim_test_resp_data_type*)(node_data_ptr->user_data);
        if(resp_data_ptr)
        {
          resp_data_ptr->is_processed = TRUE;
        }
        else
        {
          MCM_SIM_TEST_LOG_ERROR("sim_test_thread_proc ERROR: resp_data_ptr is NULL");
        }
      }
    }
    util_list_unlock_list(sim_test_queue);

    if(resp_data_ptr != NULL)
    {
      mcm_sim_test_response_main(resp_data_ptr->msg_id,
                                 resp_data_ptr->resp_data,
                                 resp_data_ptr->resp_data_len);
    }
  }

  return NULL;
} /* sim_test_thread_proc */
