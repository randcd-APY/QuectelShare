/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_WLAN_8021X_H_
#define _QAPI_WLAN_8021X_H_

/**
@file qapi_wlan_8021x.h
This section provides APIs, macros definitions, enumerations and data structures
for applications to perform WLAN 8021x control operations.
*/

#include "qapi/qapi_status.h"

/** @addtogroup qapi_wlan_8021x
@{ */

/**
@ingroup qapi_wlan_8021x
Identifies the enable/disable options for WLAN 8021x .
*/
typedef enum
{
    QAPI_WLAN_8021X_DISABLE_E  = 0, /**< Disable WLAN 8021X. */
    QAPI_WLAN_8021X_ENABLE_E   = 1  /**< Enable the WLAN 8021X. */
} qapi_WLAN_8021x_Enable_e;

/**
@ingroup qapi_wlan_8021x
Enables/disables the WLAN 8021X.
This is a blocking call and returns on allocated/freed resource for WLAN 8021x.

Use QAPI_WLAN_ENABLE_E as the parameter for enabling and QAPI_WLAN_DISABLE_E for disabling WLAN.

@datatypes
#qapi_WLAN_8021x_Enable_e

@param[in] enable  QAPI_WLAN_8021X_DISABLE_E or QAPI_WLAN_8021X_ENABLE_E.

@return
QAPI_OK -- Enabling or disabling WLAN succeeded. \n
Nonzero value -- Enabling or disabling failed.

@dependencies
To enable, use qapi_WLAN_Enable before qapi_wlan_8021x_enable. \n
To Disable, use qapi_WLAN_Enable after qapi_wlan_8021x_enable.
*/
qapi_Status_t qapi_wlan_8021x_enable (qapi_WLAN_8021x_Enable_e enable);

#endif
