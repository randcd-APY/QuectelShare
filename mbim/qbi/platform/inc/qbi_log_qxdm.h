/*!
  @file
  qbi_log_qxdm.h

  @brief
  Macro definitions for logging to QXDM

  @detail
  If silent mode is on, informational and debug messages will not be logged to
  avoid recursive messages for Diag over MBIM.
*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
10/08/12  hz   Add silent mode support
08/23/11  bd   Log to QBI SSID instead of DS
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_LOG_QXDM_H
#define QBI_LOG_QXDM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_log.h"

#include "msg.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/* Error messages */
#define QBI_LOG_E_0(fmt) \
  MSG(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt)

#define QBI_LOG_E_1(fmt, a1) \
  MSG_1(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1)

#define QBI_LOG_E_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2)

#define QBI_LOG_E_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3)

#define QBI_LOG_E_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4)

#define QBI_LOG_E_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_E_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_E_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_E_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_QBI, MSG_LEGACY_ERROR, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Warning messages */
#define QBI_LOG_W_0(fmt) \
  MSG(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt)

#define QBI_LOG_W_1(fmt, a1) \
  MSG_1(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1)

#define QBI_LOG_W_2(fmt, a1, a2) \
  MSG_2(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2)

#define QBI_LOG_W_3(fmt, a1, a2, a3) \
  MSG_3(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3)

#define QBI_LOG_W_4(fmt, a1, a2, a3, a4) \
  MSG_4(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4)

#define QBI_LOG_W_5(fmt, a1, a2, a3, a4, a5) \
  MSG_5(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_W_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_6(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_W_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_7(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_W_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_8(MSG_SSID_QBI, MSG_LEGACY_HIGH, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Informational messages */
#define QBI_LOG_I_0(fmt) if (!qbi_log_silent_mode_get()) \
      MSG(MSG_SSID_QBI, MSG_LEGACY_MED, fmt)

#define QBI_LOG_I_1(fmt, a1) if (!qbi_log_silent_mode_get()) \
    MSG_1(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1)

#define QBI_LOG_I_2(fmt, a1, a2) if (!qbi_log_silent_mode_get()) \
    MSG_2(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2)

#define QBI_LOG_I_3(fmt, a1, a2, a3) if (!qbi_log_silent_mode_get()) \
    MSG_3(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3)

#define QBI_LOG_I_4(fmt, a1, a2, a3, a4) if (!qbi_log_silent_mode_get()) \
    MSG_4(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4)

#define QBI_LOG_I_5(fmt, a1, a2, a3, a4, a5) if (!qbi_log_silent_mode_get()) \
    MSG_5(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_I_6(fmt, a1, a2, a3, a4, a5, a6) if (!qbi_log_silent_mode_get()) \
    MSG_6(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_I_7(fmt, a1, a2, a3, a4, a5, a6, a7) if (!qbi_log_silent_mode_get()) \
    MSG_7(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_I_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) if (!qbi_log_silent_mode_get()) \
    MSG_8(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Debug messages */
#define QBI_LOG_D_0(fmt) if (!qbi_log_silent_mode_get()) \
    MSG(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt)

#define QBI_LOG_D_1(fmt, a1) if (!qbi_log_silent_mode_get()) \
    MSG_1(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1)

#define QBI_LOG_D_2(fmt, a1, a2) if (!qbi_log_silent_mode_get()) \
    MSG_2(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2)

#define QBI_LOG_D_3(fmt, a1, a2, a3) if (!qbi_log_silent_mode_get()) \
    MSG_3(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3)

#define QBI_LOG_D_4(fmt, a1, a2, a3, a4) if (!qbi_log_silent_mode_get()) \
    MSG_4(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4)

#define QBI_LOG_D_5(fmt, a1, a2, a3, a4, a5) if (!qbi_log_silent_mode_get()) \
    MSG_5(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_D_6(fmt, a1, a2, a3, a4, a5, a6) if (!qbi_log_silent_mode_get()) \
    MSG_6(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_D_7(fmt, a1, a2, a3, a4, a5, a6, a7) if (!qbi_log_silent_mode_get()) \
    MSG_7(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_D_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) if (!qbi_log_silent_mode_get()) \
    MSG_8(MSG_SSID_QBI, MSG_LEGACY_LOW, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Messages allowing string values via the %s keyword. These are separate from
   the regular messages since they can have a performance impact. */
#define QBI_LOG_STR_1(fmt, a1) \
  MSG_SPRINTF_1(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1)

#define QBI_LOG_STR_2(fmt, a1, a2) \
  MSG_SPRINTF_2(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2)

#define QBI_LOG_STR_3(fmt, a1, a2, a3) \
  MSG_SPRINTF_3(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3)

#define QBI_LOG_STR_4(fmt, a1, a2, a3, a4) \
  MSG_SPRINTF_4(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4)

#define QBI_LOG_STR_5(fmt, a1, a2, a3, a4, a5) \
  MSG_SPRINTF_5(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_STR_6(fmt, a1, a2, a3, a4, a5, a6) \
  MSG_SPRINTF_6(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_STR_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  MSG_SPRINTF_7(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_STR_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  MSG_SPRINTF_8(MSG_SSID_QBI, MSG_LEGACY_MED, fmt, a1, a2, a3, a4, a5, a6, a7, a8)

#endif /* QBI_LOG_QXDM_H */

