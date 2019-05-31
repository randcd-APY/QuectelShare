/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_dhcpv4s.h
 *
 * @addtogroup qapi_networking_dhcpv4s
 * @{
 *
 * @details The Dynamic Host Configuration Protocol IPv4 (DHCPv4) service
 * provides a collection of API functions that allow the application to manage
 * a local DHCPv4 Server configuration, which is used by clients attached to
 * the device running the server. The application configures the IPv4 address
 * pool and lease time, and the rest of the configuration is taken from the
 * interface itself.
 * In order to start the DHCPv4 server, the application must setup the
 * pool configuration and, optionally, register to the success callback.
 *
 * @}
 */

#ifndef _QAPI_DHCPV4S_H_
#define _QAPI_DHCPV4S_H_

#include "stdint.h"

/** @addtogroup qapi_networking_dhcpv4s
@{ */

/**
 * @brief DHCPv4 server success callback.
 */
typedef int32_t (* qapi_Net_DHCPv4s_Success_CB_t)(
    uint8_t  *mac_Addr,
    /**< Client MAC address. */

    uint32_t ipv4_Addr
    /**< Client IPv4 address in network order. */
    );

/**
 * @brief Configures the IPv4 address pool of the DHCPv4 server.
 *
 * @param[in] interface_Name    Must be wlan0 or wlan1.
 * @param[in] start_IP          Starting IPv4 address in network order.
 * @param[in] end_IP            Ending IPv4 address in network order.
 * @param[in] lease_Time        Lease time in seconds.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DHCPv4s_Set_Pool(const char *interface_Name, uint32_t start_IP, uint32_t end_IP, uint32_t lease_Time);

/**
 * @brief Registers a callback for the DHCPv4 server.
 *
 * @details When the client successfully obtains an address, the callback will be invoked to return its
 * IPv4 address and hardware address.
 *
 * @param[in] interface_Name    Must be wlan0 or wlan1.
 * @param[in] CB    Callback function.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DHCPv4s_Register_Success_Callback(const char *interface_Name, qapi_Net_DHCPv4s_Success_CB_t CB);

/** @} */

#endif /* _QAPI_DHCPV4S_H_ */
