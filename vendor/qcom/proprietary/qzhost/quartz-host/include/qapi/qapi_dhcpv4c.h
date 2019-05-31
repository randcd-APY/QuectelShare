/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_dhcpv4c.h
 *
 * @addtogroup qapi_networking_dhcpv4c
 * @{
 *
 * @details The Dynamic Host Configuration Protocol IPv4 (DHCPv4) client service
 * provides a collection of API functions that allow the application to manage
 * automatic IPv4 configuration for a given network interface.
 * This configuration includes the interface IPv4 address, subnet mask,
 * default gateway, and DNS configuration.
 * In order to initiate a DHCPv4 client transaction, the application must
 * use the IP configuration API.
 *
 * @}
 */

#ifndef _QAPI_DHCPV4C_H_
#define _QAPI_DHCPV4C_H_

#include "stdint.h"

/** @addtogroup qapi_networking_dhcpv4c
@{ */

/**
 * @brief DHCPv4 client success callback,
 */
typedef int32_t (* qapi_Net_DHCPv4c_Success_CB_t)(
    uint32_t ipv4_Addr,
    /**< IPv4 address in network order. */

    uint32_t subnet_Mask,
    /**< Subnet mask in network order. */

    uint32_t gateway
    /**< Default gateway's IPv4 address in network order. */
    );

/**
 * @brief Releases a DHCPv4 lease.
 *
 * @param[in] interface_Name    Must be wlan0 or wlan1.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_DHCPv4c_Release(const char * interface_Name);

/**
 * @brief Registers a callback for a DHCPv4 client.
 *
 * @details When the client successfully obtains an address, the callback will be invoked to return its
 * IPv4 address, subnet mask, and default gateway.
 *
 * @param[in] interface_Name    Must be wlan0 or wlan1.
 * @param[in] CB    Callback function.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DHCPv4c_Register_Success_Callback(const char * interface_Name, qapi_Net_DHCPv4c_Success_CB_t CB);

/** @} */

#endif /* _QAPI_DHCPV4C_H_ */
