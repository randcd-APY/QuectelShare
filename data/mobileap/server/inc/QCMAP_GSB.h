#ifndef _QCMAP_GSB_H_
#define _QCMAP_GSB_H_

/*======================================================

FILE:  QCMAP_GSB.h

SERVICES:
   QCMAP GSB Class

=======================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  06/01/17   gs           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <linux/ioctl.h>
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "ds_util.h"
#include "ds_list.h"
#include "qcmap_cm_api.h"
#include "ds_string.h"
#include "QCMAP_ConnectionManager.h"

#define QCMAP_GSB_DELAY_COUNT 10000 /*10 ms or 10000 us*/

#define GSB_CONFIG_TAG "GSBConfig"
#define GSB_BOOTUP_CFG_TAG "GSBBootUpcfg"
#define GSB_ENTRIES_TAG "num_of_entries"
#define GSB_CUSTOM_TAG "custom_params"
#define GSB_INTERFACE_ENTRY_TAG "config"
#define GSB_INTERFACE_NAME_TAG "if_name"
#define GSB_INTERFACE_TYPE_TAG "if_type"
#define IPACM_ODU_TAG "ODU"
#define IPACM_ODU_STA_TAG "WAN"
#define IPACM_DEFAULT_TAG "UNKNOWN"
#define GSB_BW_REQD_TAG "bw_reqd"
#define GSB_IF_HIGH_WM_TAG "if_high_wm"
#define GSB_IF_LOW_WM_TAG "if_low_wm"
#define GSB_IF_AP_IP "ap_ip"
#define GSB_MAX_IF_SUPPORT 3
#define MAX_CMD_LEN 100

#define QCMAP_DEFAULT_CONFIG_TEMP "/tmp/mobileap_cfg_tmp.xml"
#define IPACM_DEFAULT_CONFIG_TEMP "/tmp/ipacm_cfg_tmp.xml"
#define IPACM_CONFIG_TEMP         "/tmp/IPACM_cfg.xml_bak"

#define GSB_DEFAULT_BW 900
#define GSB_DEFAULT_HIGH_WM 200
#define GSB_DEFAULT_LOW_WM 5

#define NET_DEV_FILE_ROOT_PATH "/sys/class/net"
#define IPA_DEVICE_NAME "/dev/ipa"

#define IF_STATE_UP "up"
#define IF_STATE_DOWN "down"
#define IF_STATUS_UP 1
#define IF_STATUS_DOWN 0

#define QCMAP_DEFAULT_GSB_VAL 0
#define GSB_TIMER_DEFAULT_VAL 5
#define GSB_TIMER_DEFAULT_MULTIPLIER 6
#define WLAN_IF_NUM_OFFSET 4

typedef enum {
  MSG_TYPE_ADD = 0x01,
  MSG_TYPE_DEL = 0x02,
  MSG_TYPE_UNLOAD = 0x03
}gsb_msg_type_enum;

/*=====================================================
                Helper functions Headers
  =====================================================*/

  /* IPA is not supported for 9607 */
#ifndef FEATURE_DATA_TARGET_MDM9607
boolean UpdateIPACMcfg(char* iface_name, char* tag);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
boolean SetGSBBootUpConfig(boolean flag);
uint8 GetGSBEntryCountFromXML(void);
uint8 GetGSBConfigFromXML(qcmap_msgr_gsb_config_v01 *conf);
int IsDuplicateEntry(char * iface_name);
boolean SetGSBConfigToXML(qcmap_msgr_gsb_config_v01 *conf);
boolean RemoveGSBConfigFromXML(char * iface_name);
int isInterfaceUP(char* if_name);
void ChangeIFState(char* if_name, char* state);
int SendMSGToGSB(qcmap_msgr_gsb_config_v01 *conf, int code);
extern void setGSBTimer(int time_in_sec, timer_t timerid);
extern void destroyGSBTimer(timer_t timerid);
extern timer_t createGSBTimer(void);
extern void SetGSBConfigforWLAN(int if_num,
                    qcmap_msgr_wlan_iface_active_state_enum_v01 state,
                    qcmap_msgr_wlan_iface_index_enum_v01 ap_type,
                    qcmap_msgr_wlan_device_type_v01 dev_type);

/*=====================================================
                Class definition
  =====================================================*/
class QCMAP_GSB
{
private:
  static QCMAP_GSB *object;
  static bool flag;
  static bool GSBEnableFlag;

  QCMAP_GSB();

public:
  ~QCMAP_GSB();
  static timer_t timerid;

  static QCMAP_GSB *Get_Instance(boolean obj_create=false);

  static boolean LoadGSB(qmi_error_type_v01 *qmi_err_num );
  static boolean UnLoadGSB(qmi_error_type_v01 *qmi_err_num);
  static boolean ConfigureGSB(qmi_error_type_v01 *qmi_err_num );
  static boolean DisableGSB(qmi_error_type_v01 *qmi_err_num );

  static boolean SetGSBConfig(qcmap_msgr_gsb_config_v01 *gsb_conf,
                                qmi_error_type_v01 *qmi_err_num );

  static boolean GetGSBConfig(qcmap_msgr_gsb_config_v01 *gsb_conf,
                              uint8 *num_of_entries,
                              qmi_error_type_v01 *qmi_err_num);

  static boolean DeleteGSBConfig(char* if_name,
                                 qmi_error_type_v01 *qmi_err_num,
                                 boolean update_xml );

  static boolean SetCustomGSBDataPath(char* if_name,
                                      qcmap_msgr_gsb_interface_type_enum_v01 if_type,
                                      boolean flag);
  static void QCMAP_GSB::BindIFwithGSB(char* if_name, qcmap_msgr_gsb_interface_type_enum_v01 iface_type);
};
#endif
