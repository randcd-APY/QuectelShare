/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
* @file qapi_wlan_bridge.h
*
* @addtogroup qapi_networking_wlanbrg
* @{
*
 * @details The WLAN bridging service provides a collection of API functions
 * that allow the application to set up and manage a 802.1D Bridge over two
 * Wi-Fi interfaces; one is an AP, and one a STA (or P2P equivalents).
 *
* @}
*/

#ifndef _QAPI_WLAN_BRIDGE_H_
#define _QAPI_WLAN_BRIDGE_H_

#include "stdint.h"
#include "qapi/qapi_status.h"

/** @addtogroup qapi_networking_wlanbrg
@{ */

/** MAC address length of the relayed addresses. */
#define QAPI_WLAN_BRIDGE_MAC_ADDR_LEN	6

/**
* @brief Structure to hold relay MAC database information.
*
* @details This structure is used when allocating a buffer to hold
* a bridge relay database.
*/
typedef struct {
   uint8_t addr[QAPI_WLAN_BRIDGE_MAC_ADDR_LEN];
   /**< Hardware MAC address. */

   uint16_t dev_ID;
   /**< Device ID. */

   uint32_t age;
   /**< Age timeout for MAC entry. */
} qapi_Net_Bridge_DB_Entry_t;

/**
* @brief Enables/disable a bridge in a target in the case of an IP offload.
*
* @param[in] enable  1 to enable a bridge; 0 to disable.
*
* @return
* 0 if the operation succeeded, -1 otherwise.
*/
qapi_Status_t qapi_Net_Bridge_Enable(uint32_t enable);

/**
* @brief Configures a MAC age timeout. 
*
* @param[in] timeout  Timeout value.
*
* @return
* 0 if the operation succeeded, -1 otherwise.
*/
qapi_Status_t qapi_Net_Bridge_Set_Aging_Timeout(uint32_t timeout);

/**
* @brief Shows bridge relay table.
*
* @details Call this API to fetch a bridge relay MAC database.
*
* @param[in] ptr    Pointer address to where the data is updated.
* @param[in] count  Number of entries in the relay table.
*
* @return
* 0 if the operation succeeded, -1 otherwise.
*/
qapi_Status_t qapi_Net_Bridge_Show_MACs(qapi_Net_Bridge_DB_Entry_t **ptr, uint32_t *count);

/** @} */

#endif /* _QAPI_WLAN_BRIDGE_H_ */
