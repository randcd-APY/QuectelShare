/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_addr.h
 */

#ifndef _QAPI_ADDR_H_
#define _QAPI_ADDR_H_

#include <stdint.h>

/** @addtogroup qapi_networking_socket
@{ */

/**
 * @brief IPv4 Internet address.
 */
#ifndef __IN_ADDR__
#define __IN_ADDR__
struct in_addr
{
   uint32_t s_addr;
   /**< IPv4 address in network order. */
};

typedef unsigned long ip_addr;
#endif

/**
 * @brief IPv6 Internet address.
 */
#ifndef __IN6_ADDR__
#define __IN6_ADDR__
typedef struct in6_addr
{
   uint8_t  s_addr[16];
   /**< 128-bit IPv6 address. */
} ip6_addr;
#endif

/**
 * @brief IPv4/IPv6 Internet address union.
 */
#ifndef __IP46_ADDR__
#define __IP46_ADDR__
struct ip46addr
{
    uint16_t type;
    /**< AF_INET or AF_INET6. */
    union
    {
        unsigned long   addr4;
        /**< IPv4 address. */
        ip6_addr        addr6;
        /**< IPv6 address. */
    } a;
    /**< Address union. */
};
#endif

/** @} */

#endif /* _QAPI_ADDR_H_ */
