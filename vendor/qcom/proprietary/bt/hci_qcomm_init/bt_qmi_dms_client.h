/******************************************************************************NS

  @file    bt_qmi_dms_client.h
  @brief

  DESCRIPTION

******************************************************************************/
/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#if defined(BT_QSOC_GET_ITEMS_FROM_NV)

#ifndef BT_QMI_DMS_CLIENT_H
#define BT_QMI_DMS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif


#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"
#include "device_management_service_v01.h"
#include "comdef.h"

#ifdef ANDROID
#include <utils/Log.h>
#endif
#include <stdio.h>
#include <fcntl.h>
#include <cutils/properties.h>

#define LOG_TAG "hci_qcomm_init"

#ifndef ANDROID
#define PRI_INFO " I"
#define PRI_WARN " W"
#define PRI_ERROR " E"
#define PRI_DEBUG " D"
#define PRI_VERB " V"

#define ALOG(pri, tag, fmt, arg...) fprintf(stderr, tag pri ": " fmt"\n", ##arg)
#define ALOGV(fmt, arg...) ALOG(PRI_VERB, LOG_TAG, fmt, ##arg)
#define ALOGD(fmt, arg...) ALOG(PRI_DEBUG, LOG_TAG, fmt, ##arg)
#define ALOGI(fmt, arg...) ALOG(PRI_INFO, LOG_TAG, fmt, ##arg)
#define ALOGW(fmt, arg...) ALOG(PRI_WARN, LOG_TAG, fmt, ##arg)
#define ALOGE(fmt, arg...) ALOG(PRI_ERROR, LOG_TAG, fmt, ##arg)

#define LOG_VERBOSE(fmt, arg...) ALOG(PRI_VERB, LOG_TAG, fmt, ##arg)
#define LOG_DEBUG(fmt, arg...) ALOG(PRI_DEBUG, LOG_TAG, fmt, ##arg)
#define LOG_INFO(fmt, arg...) ALOG(PRI_INFO, LOG_TAG, fmt, ##arg)
#define LOG_WARN(fmt, arg...) ALOG(PRI_WARN, LOG_TAG, fmt, ##arg)
#define LOG_ERROR(fmt, arg...) ALOG(PRI_ERROR, LOG_TAG, fmt, ##arg)
#endif

#define DMS_QMI_TIMEOUT (2000)
#define BD_ADDR_SIZE    (0x06)

#ifndef ANDROID
extern int property_get_bt(const char *key, char *value, const char *default_value);
extern int property_set_bt(const char *key, const char *value);
#endif

typedef enum {
  MODEM_NV_BD_ADDR,
  MODEM_NV_BT_SOC_REFCLOCK_TYPE,
  MODEM_NV_BT_SOC_CLK_SHARING_TYPE,
} modem_bt_nv_items_type;

typedef unsigned char boolean;

boolean bt_qmi_dms_init(void);
boolean qmi_dms_get_bt_address(unsigned char *pBdAddr);
boolean qmi_dms_get_refclock(unsigned long *ref_clock);
boolean qmi_dms_get_clk_sharing(boolean *clock_sharing);
boolean is_modem_bt_nv_supported(modem_bt_nv_items_type nv_item_type);

void cleanup_qmi_dms(void);


#ifdef __cplusplus
}
#endif

#endif

#endif
