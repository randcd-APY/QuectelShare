/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_NS_GEN_V6_H_
#define _QAPI_NS_GEN_V6_H_

#include "stdint.h"
#include "qapi/qapi_status.h"   /* qapi_Status_t */
#include "qapi_addr.h"          /* ip6_addr */

/** @file qapi_ns_gen_v6.h
*
*/

/** @addtogroup qapi_networking_services
@{ */

/**
 * @brief Checks whether the IPv6 address is link local.
 *
 * @details This macro returns 1 if the passed IPv6 address is link local.
 *          The link local address format is fe80::/64. In other words, the
 *          first 10 bits of the address are 1111111010, followed by
 *          54 zeros, followed by 64 bits of interface identifier.
 *
 * @param[in] ipv6_Address    IPv6 address to check.
 *
 * @return
 * 1 if the IPv6 address is link local, or 0 otherwise.
 *
 */
#define QAPI_IS_IPV6_LINK_LOCAL(ipv6_Address) \
       (((void *)ipv6_Address != NULL) && \
        (((uint8_t *)ipv6_Address)[0] == 0xfe) && \
        (((uint8_t *)ipv6_Address)[1] == 0x80) && \
        (((uint8_t *)ipv6_Address)[2] == 0x0) && \
        (((uint8_t *)ipv6_Address)[3] == 0x0) && \
        (((uint8_t *)ipv6_Address)[4] == 0x0) && \
        (((uint8_t *)ipv6_Address)[5] == 0x0) && \
        (((uint8_t *)ipv6_Address)[6] == 0x0) && \
        (((uint8_t *)ipv6_Address)[7] == 0x0))

/**
 * @brief Checks if the IPv6 address is a multicast address.
 *
 * @param[in] ipv6_Address    IPv6 address to check.
 *
 * @return
 * 1 if the IPv6 address is multicast, or 0 otherwise.
 *
 */
#define QAPI_IS_IPV6_MULTICAST(ipv6_Address)    (((uint8_t *)ipv6_Address)[0] == 0xff)

/**
 * @brief IPv6 routing object.
 */
typedef struct
{
    uint8_t  ipv6RouteDest[16];
    /**< Destination IPv6 address of this route. */

    uint32_t ipv6RoutePfxLength;
    /**< Indicates the prefix length of the destination address. */

    uint8_t  ipv6RouteNextHop[16];
    /**< Address of the next system en route. */

    char     ifName[8];
    /**< Textual name of the local interface through
         which the next hop of this route should be reached. */

} qapi_Net_IPv6_Route_t;

/**
 * @brief Sends an IPv6 ping request.
 *
 * @param[in] ipv6_Addr    IPv6 address to which to send a ping.
 * @param[in] size         Number of data bytes to send.
 * @param[in] interface_Name  When pinging an IPv6 link-local address, the interface name is required.
 *
 * @return
 * 0 -- A ping response was received. \n
 * 1 -- The ping request timed out. \n
 * -1 -- Error.
 */
qapi_Status_t qapi_Net_Ping6(uint8_t ipv6_Addr[16], uint32_t size, const char * interface_Name);

/**
 * @brief Gets the IPv6 addresses of an interface.
 *
 * @param[in] interface_Name            Name of the network interface.
 * @param[in] link_Local                Link-local unicast address.
 * @param[in] global                    First global unicast address.
 * @param[in] default_Gateway           Default gateway's address.
 * @param[in] global_Second             Second global unicast address.
 * @param[in] link_Local_Prefix         Prefix length of link-local address.
 * @param[in] global_Prefix             Prefix length of the first global address.
 * @param[in] default_Gateway_Prefix    Prefix length of the default gateway's address.
 * @param[in] global_Second_Prefix      Prefix length of the second global address.
 *
 * @note1hang    All parameters except interface_Name can be NULL.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv6_Get_Address(
                const char *interface_Name,
                uint8_t *link_Local,
                uint8_t *global,
                uint8_t *default_Gateway,
                uint8_t *global_Second,
                uint32_t *link_Local_Prefix,
                uint32_t *global_Prefix,
                uint32_t *default_Gateway_Prefix,
                uint32_t *global_Second_Prefix);

/**
 * @brief Adds a new entry to the prefixes to be advertised by the router.
 *
 * @param[in] interface_Name        Name of the network interface.
 * @param[in] ipv6_Addr             Destination IPv6 address of this route (ipv6RouteDest).
 * @param[in] prefix_Length         Prefix length (1 to 128).
 * @param[in] preferred_Lifetime    Preferred lifetime in seconds.
 * @param[in] valid_Lifetime        Valid lifetime in seconds.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv6_Config_Router_Prefix(
                const char *interface_Name,
                uint8_t *ipv6_Addr,
                uint32_t prefix_Length,
                uint32_t preferred_Lifetime,
                uint32_t valid_Lifetime);

/**
 * @brief Adds an IPv6 route.
 *
 * @param[in] interface_Name    Name of the local network interface through which the next hop of this route should be reached.
 * @param[in] dest              Pointer to destination IPv6 address of this route.
 * @param[in] prefix_Length     Prefix length of the destination address.
 * @param[in] next_Hop          Pointer to the IPv6 address of the next system en route.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv6_Route_Add(const char *interface_Name, ip6_addr *dest, uint32_t prefix_Length, ip6_addr *next_Hop);

/**
 * @brief Deletes an IPv6 route.
 *
 * @param[in] interface_Name    Name of the local network interface through which the next hop of this route should be reached.
 * @param[in] dest              Pointer to the destination IPv6 address of this route.
 * @param[in] prefix_Length     Prefix length of the destination address.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_IPv6_Route_Del(const char *interface_Name, ip6_addr *dest, uint32_t prefix_Length);

/**
 * @brief Gets the IPv6 routing table.
 *
 * @param[in] buf           Pointer to a buffer to contain the routing table.
 *                          When buf is NULL, the number of entries in the routing table is returned in *pcount.
 * @param[in,out] pcount    This is a value-result argument. Before the call, the caller should set the integer value pointed to
 *                          by pcount to the number of entries requested to be copied to buf, and on return *pcount contains the
 *                          actual number of entries copied to buf.
 *
 * @return  0 on success, or a negative error code.
 */
qapi_Status_t qapi_Net_IPv6_Routing_Table_Get(qapi_Net_IPv6_Route_t *buf, uint32_t *pcount);

/**
 * @brief  Enables or disables IPv6 routing functionality.
 *
 * @param[in] enable    "true" to enable the routing; \n
 *                      "false" to disable the routing.
 *
 * @return
 * 0 on success, or -1 on error.
 *
 */
qapi_Status_t qapi_Net_IPv6_Routing_Enable(qbool_t enable);

/**
 * @brief Returns the scope ID for the interface.
 *
 * @details When using link-local addressing with IPv6 protocol, it is
 *          necessary to specify the scope ID along with the destination address.
 *          The application is to use this function to retrieve a scope ID
 *          based on the interface name.
 *
 * @param[in]  interface_Name    Name of the interface for which to
 *                               retrieve the scope ID.
 * @param[out] scope_ID          Pointer to where to store the scope ID.
 *
 * @return
 * 0 on success, or a negative error code.
 *
 */
qapi_Status_t qapi_Net_IPv6_Get_Scope_ID(const char *interface_Name, int32_t *scope_ID);

/** @} */ /* end_addtogroup qapi_networking_services */

#endif /* _QAPI_NS_GEN_V6_H_ */
