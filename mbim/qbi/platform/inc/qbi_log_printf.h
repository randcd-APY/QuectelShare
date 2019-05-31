/*!
  @file
  qbi_log_printf.h

  @brief
  Macro definitions for logging via printf
*/

/*=============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
02/10/12  bd   Updates for Linux
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_LOG_PRINTF_H
#define QBI_LOG_PRINTF_H

/*=============================================================================

  Include Files

=============================================================================*/

#include <stdio.h>

/*=============================================================================

  Constants and Macros

=============================================================================*/

//#define FEATURE_QBI_LOG_PRINTF_COLOR

#ifdef FEATURE_QBI_LOG_PRINTF_COLOR

  /*! Error */
  #define QBI_LOG_E(fmt, args...) \
    printf("\e[31m[e] %s:%d " fmt "\e[m\n", __FILE__, __LINE__, ## args)

  /*! Warning */
  #define QBI_LOG_W(fmt, args...) \
    printf("\e[33m[w] %s:%d " fmt "\e[m\n", __FILE__, __LINE__, ## args)

  /*! Informational */
  #define QBI_LOG_I(fmt, args...) \
    printf("\e[1;34m[i] %s:%d " fmt "\e[m\n", __FILE__, __LINE__, ## args)

  /*! Debug */
  #define QBI_LOG_D(fmt, args...) \
    printf("\e[36m[d] %s:%d " fmt "\e[m\n", __FILE__, __LINE__, ## args)

#else /* QBI_LOG_PRINTF_COLOR */

  /*! Error */
  #define QBI_LOG_E(fmt, args...) \
    printf("[e] %s:%d " fmt "\n", __FILE__, __LINE__, ## args)

  /*! Warning */
  #define QBI_LOG_W(fmt, args...) \
    printf("[w] %s:%d " fmt "\n", __FILE__, __LINE__, ## args)

  /*! Informational */
  #define QBI_LOG_I(fmt, args...) \
    printf("[i] %s:%d " fmt "\n", __FILE__, __LINE__, ## args)

  /*! Debug */
  #define QBI_LOG_D(fmt, args...) \
    printf("[d] %s:%d " fmt "\n", __FILE__, __LINE__, ## args)

#endif /* FEATURE_QBI_LOG_PRINTF_COLOR */

/* Error messages */
#define QBI_LOG_E_0(fmt) \
  QBI_LOG_E(fmt)

#define QBI_LOG_E_1(fmt, a1) \
  QBI_LOG_E(fmt, a1)

#define QBI_LOG_E_2(fmt, a1, a2) \
  QBI_LOG_E(fmt, a1, a2)

#define QBI_LOG_E_3(fmt, a1, a2, a3) \
  QBI_LOG_E(fmt, a1, a2, a3)

#define QBI_LOG_E_4(fmt, a1, a2, a3, a4) \
  QBI_LOG_E(fmt, a1, a2, a3, a4)

#define QBI_LOG_E_5(fmt, a1, a2, a3, a4, a5) \
  QBI_LOG_E(fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_E_6(fmt, a1, a2, a3, a4, a5, a6) \
  QBI_LOG_E(fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_E_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  QBI_LOG_E(fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_E_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  QBI_LOG_E(fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Warning messages */
#define QBI_LOG_W_0(fmt) \
  QBI_LOG_W(fmt)

#define QBI_LOG_W_1(fmt, a1) \
  QBI_LOG_W(fmt, a1)

#define QBI_LOG_W_2(fmt, a1, a2) \
  QBI_LOG_W(fmt, a1, a2)

#define QBI_LOG_W_3(fmt, a1, a2, a3) \
  QBI_LOG_W(fmt, a1, a2, a3)

#define QBI_LOG_W_4(fmt, a1, a2, a3, a4) \
  QBI_LOG_W(fmt, a1, a2, a3, a4)

#define QBI_LOG_W_5(fmt, a1, a2, a3, a4, a5) \
  QBI_LOG_W(fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_W_6(fmt, a1, a2, a3, a4, a5, a6) \
  QBI_LOG_W(fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_W_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  QBI_LOG_W(fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_W_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  QBI_LOG_W(fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Informational messages */
#define QBI_LOG_I_0(fmt) \
  QBI_LOG_I(fmt)

#define QBI_LOG_I_1(fmt, a1) \
  QBI_LOG_I(fmt, a1)

#define QBI_LOG_I_2(fmt, a1, a2) \
  QBI_LOG_I(fmt, a1, a2)

#define QBI_LOG_I_3(fmt, a1, a2, a3) \
  QBI_LOG_I(fmt, a1, a2, a3)

#define QBI_LOG_I_4(fmt, a1, a2, a3, a4) \
  QBI_LOG_I(fmt, a1, a2, a3, a4)

#define QBI_LOG_I_5(fmt, a1, a2, a3, a4, a5) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_I_6(fmt, a1, a2, a3, a4, a5, a6) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_I_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_I_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Debug messages */
#define QBI_LOG_D_0(fmt) \
  QBI_LOG_D(fmt)

#define QBI_LOG_D_1(fmt, a1) \
  QBI_LOG_D(fmt, a1)

#define QBI_LOG_D_2(fmt, a1, a2) \
  QBI_LOG_D(fmt, a1, a2)

#define QBI_LOG_D_3(fmt, a1, a2, a3) \
  QBI_LOG_D(fmt, a1, a2, a3)

#define QBI_LOG_D_4(fmt, a1, a2, a3, a4) \
  QBI_LOG_D(fmt, a1, a2, a3, a4)

#define QBI_LOG_D_5(fmt, a1, a2, a3, a4, a5) \
  QBI_LOG_D(fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_D_6(fmt, a1, a2, a3, a4, a5, a6) \
  QBI_LOG_D(fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_D_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  QBI_LOG_D(fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_D_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  QBI_LOG_D(fmt, a1, a2, a3, a4, a5, a6, a7, a8)

/* Messages allowing string values via the %s keyword. These are separate from
   the regular messages since they can have a performance impact. */
#define QBI_LOG_STR_1(fmt, a1) \
  QBI_LOG_I(fmt, a1)

#define QBI_LOG_STR_2(fmt, a1, a2) \
  QBI_LOG_I(fmt, a1, a2)

#define QBI_LOG_STR_3(fmt, a1, a2, a3) \
  QBI_LOG_I(fmt, a1, a2, a3)

#define QBI_LOG_STR_4(fmt, a1, a2, a3, a4) \
  QBI_LOG_I(fmt, a1, a2, a3, a4)

#define QBI_LOG_STR_5(fmt, a1, a2, a3, a4, a5) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5)

#define QBI_LOG_STR_6(fmt, a1, a2, a3, a4, a5, a6) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6)

#define QBI_LOG_STR_7(fmt, a1, a2, a3, a4, a5, a6, a7) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6, a7)

#define QBI_LOG_STR_8(fmt, a1, a2, a3, a4, a5, a6, a7, a8) \
  QBI_LOG_I(fmt, a1, a2, a3, a4, a5, a6, a7, a8)

#endif /* QBI_LOG_PRINTF_H */

