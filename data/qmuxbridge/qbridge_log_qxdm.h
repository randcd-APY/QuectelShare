/******************************************************************************

                        QBRIDGE_LOG_QXDM.H

******************************************************************************/

/******************************************************************************

  @file    qbridge_log_qxdm.h
  @brief   This handles logging for Qmuxbridge Component.

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/02/18    rv         Initial version

******************************************************************************/

/*=============================================================================

  Include Files

=============================================================================*/

#include "msg.h"
#include "ds_util.h"
#include "log.h"

/*=============================================================================

  Function Prototypes

=============================================================================*/
boolean qbridge_log_silent_mode_get
(
  void
);

/*=============================================================================

  Constants and Macros

=============================================================================*/

/* Error messages */
#define QBRIDGE_LOG_E_0(fmt) \
  MSG(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt)

#define QBRIDGE_LOG_E_1(fmt, a1) \
  MSG_1(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1)

#define QBRIDGE_LOG_E_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2)

#define QBRIDGE_LOG_E_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3)

#define QBRIDGE_LOG_E_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4)

#define QBRIDGE_LOG_E_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5)

#define QBRIDGE_LOG_E_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6)

#define QBRIDGE_LOG_E_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBRIDGE_LOG_E_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Warning messages */
#define QBRIDGE_LOG_W_0(fmt) \
  MSG(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt)

#define QBRIDGE_LOG_W_1(fmt, a1) \
  MSG_1(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1)

#define QBRIDGE_LOG_W_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2)

#define QBRIDGE_LOG_W_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3)

#define QBRIDGE_LOG_W_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4)

#define QBRIDGE_LOG_W_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5)

#define QBRIDGE_LOG_W_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6)

#define QBRIDGE_LOG_W_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBRIDGE_LOG_W_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Informational messages */
#define QBRIDGE_LOG_I_0(fmt) if (!qbridge_log_silent_mode_get()) \
    MSG(MSG_SSID_QBI, MSG_LEGACY_MED, fmt)

#define QBRIDGE_LOG_I_1(fmt, a1) if (!qbridge_log_silent_mode_get()) \
    MSG_1(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1)

#define QBRIDGE_LOG_I_2(fmt, a1, a2) if (!qbridge_log_silent_mode_get()) \
    MSG_2(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2)

#define QBRIDGE_LOG_I_3(fmt, a1, a2, a3) if (!qbridge_log_silent_mode_get()) \
    MSG_3(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3)

#define QBRIDGE_LOG_I_4(fmt, a1, a2, a3, a4) if (!qbridge_log_silent_mode_get()) \
    MSG_4(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4)

#define QBRIDGE_LOG_I_5(fmt, a1, a2, a3, a4, a5) if (!qbridge_log_silent_mode_get()) \
    MSG_5(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5)

#define QBRIDGE_LOG_I_6(fmt, a1, a2, a3, a4, a5, a6) if (!qbridge_log_silent_mode_get()) \
    MSG_6(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6)

#define QBRIDGE_LOG_I_7(fmt, a1, a2, a3, a4, a5, a6, a7) if (!qbridge_log_silent_mode_get()) \
    MSG_7(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBRIDGE_LOG_I_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) if (!qbridge_log_silent_mode_get()) \
    MSG_8(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Debug messages */
#define QBRIDGE_LOG_D_0(fmt) if (!qbridge_log_silent_mode_get()) \
    MSG(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt)

#define QBRIDGE_LOG_D_1(fmt, a1) if (!qbridge_log_silent_mode_get()) \
    MSG_1(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1)

#define QBRIDGE_LOG_D_2(fmt, a1, a2) if (!qbridge_log_silent_mode_get()) \
    MSG_2(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2)

#define QBRIDGE_LOG_D_3(fmt, a1, a2, a3) if (!qbridge_log_silent_mode_get()) \
    MSG_3(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3)

#define QBRIDGE_LOG_D_4(fmt, a1, a2, a3, a4) if (!qbridge_log_silent_mode_get()) \
    MSG_4(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4)

#define QBRIDGE_LOG_D_5(fmt, a1, a2, a3, a4, a5) if (!qbridge_log_silent_mode_get()) \
    MSG_5(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5)

#define QBRIDGE_LOG_D_6(fmt, a1, a2, a3, a4, a5, a6) if (!qbridge_log_silent_mode_get()) \
    MSG_6(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6)

#define QBRIDGE_LOG_D_7(fmt, a1, a2, a3, a4, a5, a6, a7) if (!qbridge_log_silent_mode_get()) \
    MSG_7(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBRIDGE_LOG_D_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) if (!qbridge_log_silent_mode_get()) \
    MSG_8(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Messages allowing string values via the %s keyword. These are separate from
   the regular messages since they can have a performance impact. */
#define QBRIDGE_LOG_STR_1(fmt, a1) \
  MSG_SPRINTF_1(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1)

#define QBRIDGE_LOG_STR_2(fmt, a1, a2) \
  MSG_SPRINTF_2(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2)

#define QBRIDGE_LOG_STR_3(fmt, a1, a2, a3) \
  MSG_SPRINTF_3(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3)

#define QBRIDGE_LOG_STR_4(fmt, a1, a2, a3, a4) \
  MSG_SPRINTF_4(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4)

#define QBRIDGE_LOG_STR_5(fmt, a1, a2, a3, a4, a5) \
  MSG_SPRINTF_5(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5)

#define QBRIDGE_LOG_STR_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_SPRINTF_6(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6)

#define QBRIDGE_LOG_STR_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_SPRINTF_7(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBRIDGE_LOG_STR_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_SPRINTF_8(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7, a8)
