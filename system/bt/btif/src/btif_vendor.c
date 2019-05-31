	/*
	 * Copyright (C) 2016 The Linux Foundation. All rights reserved
	 * Not a Contribution.
	 *  Copyright (C) 2009-2012 Broadcom Corporation
	 *
	 *  Licensed under the Apache License, Version 2.0 (the "License");
	 *  you may not use this file except in compliance with the License.
	 *  You may obtain a copy of the License at:
	 *
	 *  http://www.apache.org/licenses/LICENSE-2.0
	 *
	 *  Unless required by applicable law or agreed to in writing, software
	 *  distributed under the License is distributed on an "AS IS" BASIS,
	 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 *  See the License for the specific language governing permissions and
	 *  limitations under the License.
	 *
	 ******************************************************************************/

	/************************************************************************************
	 *
	 *  Filename:      btif_vendor.c
	 *
	 *  Description:   Vendor Bluetooth Interface
	 *
	 *
	 ***********************************************************************************/

	#include <hardware/bluetooth.h>
	#include <hardware/vendor.h>
	#include <stdlib.h>
	#include <string.h>

	#define LOG_TAG "bt_btif_vendor"

	#include <cutils/properties.h>
	#include "bt_utils.h"
	#include "btif_common.h"
	#include "btif_util.h"
	#include "btif_profile_queue.h"
	#include "stack_config.h"
	#include "stack_manager.h"

	#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)  (1 << (id))
	extern bt_status_t btif_in_execute_service_request(tBTA_SERVICE_ID service_id,
						       BOOLEAN b_enable);
	static void get_logger_config_value(void);
	void btif_vendor_snooplog_status_event(UINT16, char *p_param);

btvendor_callbacks_t *bt_vendor_callbacks = NULL;

typedef enum {
    BTIF_VENDOR_BREDR_CLEANUP = 1,
    BTIF_VENDOR_SET_LE_BT_NAME,
}btif_vendor_event_t;

/*******************************************************************************
** VENDOR INTERFACE FUNCTIONS
*******************************************************************************/

/*******************************************************************************
**
** Function         btif_vendor_init
**
** Description     initializes the vendor interface
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t init( btvendor_callbacks_t* callbacks)
{
    LOG_ERROR(LOG_TAG,"init");
    bt_vendor_callbacks = callbacks;

    get_logger_config_value();

    return BT_STATUS_SUCCESS;
}

static void ssrcleanup(void)
{
    LOG_INFO(LOG_TAG,"ssrcleanup");
    int soc_type = get_soc_type();
    if (soc_type == BT_SOC_ROME || soc_type == BT_SOC_CHEROKEE)
        btif_ssr_cleanup();
    return;
}

static void btif_vendor_bredr_cleanup_event(UINT16 event, char *p_param)
{
    tBTA_SERVICE_MASK service_mask;
    uint32_t i;
    service_mask = btif_get_enabled_services_mask();
    for (i = 0; i <= BTA_MAX_SERVICE_ID; i++)
    {
        if (i != BTA_BLE_SERVICE_ID && (service_mask &
              (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))))
        {
            btif_in_execute_service_request(i, FALSE);
        }
    }
    btif_queue_release();
    HAL_CBACK(bt_vendor_callbacks, bredr_cleanup_cb, true);
}

static void bredrcleanup(void)
{
    LOG_INFO(LOG_TAG,"bredrcleanup");
    btif_transfer_context(btif_vendor_bredr_cleanup_event,BTIF_VENDOR_BREDR_CLEANUP,
                          NULL, 0, NULL);
}

static void capture_vnd_logs(void)
{
    LOG_INFO(LOG_TAG,"capture_vnd_logs");
    GENERATE_VND_LOGS();
}

static void cleanup(void)
{
    LOG_INFO(LOG_TAG,"cleanup");
    if (bt_vendor_callbacks)
        bt_vendor_callbacks = NULL;
}

static void setLeBtName(btvendor_lename_t *name)
{
    if (name == NULL || name->val == NULL) {
        BTIF_TRACE_EVENT(" %s LE name cannot be set to NULL.",__FUNCTION__);
        return;
    }

    char bname [BTM_MAX_LOC_BD_NAME_LEN+1];
    UINT16 name_len = 0;

    name_len = name->len > BTM_MAX_LOC_BD_NAME_LEN ? BTM_MAX_LOC_BD_NAME_LEN:name->len;
    memcpy(bname,name->val,name_len);
    bname[name_len] = '\0';

    BTIF_TRACE_EVENT("%s name : %s len : %d",__FUNCTION__,bname,name_len);
    BTA_DmSetLeDeviceName(bname);
}

static const btvendor_interface_t btvendorInterface = {
    sizeof(btvendorInterface),
    init,
    ssrcleanup,
    bredrcleanup,
    capture_vnd_logs,
    cleanup,
    NULL,
    setLeBtName,
};

/*******************************************************************************
** LOCAL FUNCTIONS
*******************************************************************************/

/*******************************************************************************
**
** Function         btif_vendor_get_interface
**
** Description      Get the vendor callback interface
**
** Returns          btvendor_interface_t
**
*******************************************************************************/
const btvendor_interface_t *btif_vendor_get_interface()
{
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    return &btvendorInterface;
}


#ifdef BLUEDROID_DEBUG
/*******************************************************************************
**
** Function         set_logging_pref
**
** Description      Set logging preference property to control BT logging
**                  based on config file or developer option
**
** Returns          void
**
*******************************************************************************/
void set_logging_pref(uint16_t pref_val)
{
  char new_log_pref[2];
  snprintf(new_log_pref, 2, "%d", pref_val);
  property_set("persist.bluetooth.log_pref", new_log_pref);
  LOG_INFO(LOG_TAG, "%s, New Logging preference value: %d", __func__, pref_val);
}

/*******************************************************************************
**
** Function         get_logging_pref
**
** Description      Returns current logging preference to set logging status
**                  based on config file or developer option
**
** Returns          void
**
*******************************************************************************/
uint16_t get_logging_pref()
{
  return (uint16_t)property_get_int32("persist.bluetooth.log_pref", NO_PREFERENCE);
}
#endif

/*******************************************************************************
**
** Function         get_logger_config_value
**
** Description      Set logging option based on flag values set
**                  in config file
**
** Returns          void
**
*******************************************************************************/
static void get_logger_config_value()
{
  bool hci_ext_dump_enabled = false;
  bool btsnoop_conf_from_file = false;

  LOG_INFO(LOG_TAG, "%s", __func__);

#ifdef BLUEDROID_DEBUG
  uint16_t logging_pref;
  logging_pref = get_logging_pref();

  LOG_INFO(LOG_TAG, "%s, logging_pref = %d", __func__, logging_pref);
  if(logging_pref == DEV_OPT_PREFERENCE)
    return;
#endif

  stack_config_get_interface()->get_btsnoop_ext_options(&hci_ext_dump_enabled, &btsnoop_conf_from_file);

  /* ToDo: Change dependency to work on one config option*/
#ifdef BLUEDROID_DEBUG
  if(!btsnoop_conf_from_file)
    hci_ext_dump_enabled = true;
#endif

  if(hci_ext_dump_enabled)
    bt_logger_enabled = true;
}

#ifdef BLUEDROID_DEBUG
/*******************************************************************************
**
** Function         enable_bt_logger_debug
**
** Description      enable Bluetooth Unified logger based on values
**                  from config file and developer option and current
**                  logging preference.
**
** Returns          void
**
*******************************************************************************/
void enable_bt_logger_debug(bool enable)
{
    if(stack_manager_get_interface()->get_stack_is_running()) {
        if(enable && !bt_logger_enabled)
        {
            property_set("bluetooth.startbtlogger", "true");
            usleep(500000);
            bt_logger_enabled = true;
            init_vnd_Logger();
        }
        set_logging_pref(DEV_OPT_PREFERENCE);
    } else {
        uint16_t curr_log_pref;
        curr_log_pref = get_logging_pref();

        LOG_INFO(LOG_TAG, "config_hci_snoop_log, curr_log_pref = %d", curr_log_pref);
        if(curr_log_pref < DEV_OPT_PREFERENCE && enable != bt_logger_enabled) {
            if(curr_log_pref == bt_logger_enabled) {
                /*It means user have updated the logging option
                 *from Developer Option while Bluetooth was Off.*/
                bt_logger_enabled = enable;
                set_logging_pref(DEV_OPT_PREFERENCE);
            } else {
                /*It means user have updated the logging option
                 *from bt_stack.conf file.*/
                set_logging_pref(bt_logger_enabled);
            }
        } else if(curr_log_pref == DEV_OPT_PREFERENCE && enable) {
            bt_logger_enabled = enable;
        }
        LOG_INFO(LOG_TAG, "config_hci_snoop_log, logging status = %d", bt_logger_enabled);
        if(bt_logger_enabled)
            property_set("bluetooth.startbtlogger", "true");
    }
}
#else
/*******************************************************************************
**
** Function         enable_bt_logger
**
** Description      Enable Bluetooth Unified Logger based on input
**                  from developer option.
**
** Returns          void
**
*******************************************************************************/
void enable_bt_logger(bool enable)
{
    if(stack_manager_get_interface()->get_stack_is_running()) {
        if(enable && !bt_logger_enabled)
        {
            property_set("bluetooth.startbtlogger", "true");
            usleep(500000);
            bt_logger_enabled = true;
            init_vnd_Logger();
        }
    } else {
        bt_logger_enabled = enable;
        if(bt_logger_enabled)
            property_set("bluetooth.startbtlogger", "true");
    }
}
#endif

/*******************************************************************************
**
** Function        btif_vendor_snooplog_status_event
**
** Description     updates snoop log status to application layer
**
** Returns         void
**
*******************************************************************************/
void btif_vendor_snooplog_status_event(UINT16 status, char *p_param)
{
    LOG_INFO(LOG_TAG, "%s", __func__);
    HAL_CBACK(bt_vendor_callbacks, update_snooplog_status_cb, status);
}
