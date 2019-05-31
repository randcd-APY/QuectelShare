/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_dnsc.h
 *
 * @addtogroup qapi_networking_dnsc
 * @{
 *
 * @details The domain name system (DNS) client service provides a collection
 * of API functions that allow the application both configure DNS services
 * in the system and translate domain names to their numerical IPv4
 * or IPv6 (or both) addresses, which is needed for the purpose of initiating
 * communications with a remote server or service.
 * The DNS service may be either manually configured or automatically
 * configured when the DHCP client is enabled.
 *
 * @}
 */

#ifndef _QAPI_DNSC_H_
#define _QAPI_DNSC_H_

#include "stdint.h"
#include "qapi_addr.h"  /* ip46addr */

/** @addtogroup qapi_networking_dnsc
@{ */

/** DNS server port */
#define     QAPI_DNS_PORT       53

/**
 * Maximum number of DNS servers in the system.
 */
#define QAPI_NET_DNS_SERVER_MAX 3

/**
 *  Use ANY_SERVER_ID to populate a free entry, or use
 *  an index (starts from zero) to update a specific entry.
 */
#define QAPI_NET_DNS_ANY_SERVER_ID         0xFFFF

/**
 * DNS server ID
 */
#define QAPI_NET_DNS_PRIMARY_SERVER_ID 0

#define QAPI_NET_DNS_SECONDARY_SERVER_ID 1

/**
 * Commands to start/stop/disable a DNS client.
 */
typedef enum {
    QAPI_NET_DNS_DISABLE_E,
    /**< STOP plus free the space for internal data structures. */
    QAPI_NET_DNS_START_E,
    /**< Allocate space for internal data structures. A DNS query is allowed after the START command. */
    QAPI_NET_DNS_STOP_E
    /**< Stop sending DNS requests and processing DNS responses. Keep internal data structures. */
} qapi_Net_DNS_Command_t;

/**
 * For use with qapi_Net_DNSc_Get_Server_List() to get IP addresses of DNS servers.
 */
typedef struct {
    struct ip46addr svr[QAPI_NET_DNS_SERVER_MAX];  /**< Array of IPv4 or IPv6 DNS server addresses. */
} qapi_Net_DNS_Server_List_t;

/**
 * Data structure returned from qapi_Net_DNSc_Get_Host_By_Name() or
 * qapi_Net_DNSc_Get_Host_By_Name2().
 * This is the same as the UNIX struct hostent{}.
 */
struct qapi_hostent_s {
    char  *h_name;	     /**< Official name of the host. */
    char **h_aliases;	 /**< Alias list. */
    int    h_addrtype;	 /**< Host address type. */
    int    h_length; 	 /**< Length of the address. */
    char **h_addr_list;	 /**< List of addresses. */
};

/**
 * Checks whether the DNS client is started.
 *
 * @return
 * 0 if not started, 1 if started.
 */
int32_t qapi_Net_DNSc_Is_Started(void);

/**
 * Starts, stops, or disables the DNS client.
 *
 * @param[in] cmd   Command to start/stop/disable the DNS client. \n
 *                  The supported commands are QAPI_NET_DNS_DISABLE_E,
 *                  QAPI_NET_DNS_START_E, and QAPI_NET_DNS_STOP_E.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DNSc_Command(qapi_Net_DNS_Command_t cmd);

/**
 * Resolves an IP address text string into an actual IP address.
 *
 * @param[in] hostname  IP address string or host name string.
 * @param[in] ipaddr    Pointer to struct ip46addr for the resolved IP address.
 *                      Caller has to specify which IP address (v4 or v6) it intends
 *                      to resolve to: \n
 *                      - If ipaddr->type is AF_INET, resolve to IPv4 address
 *                      - If ipaddr->type is AF_INET6, resolve to IPv6 address
 *
 * @return
 * On success, 0 is returned; on error, <0 is returned.
 */
int32_t qapi_Net_DNSc_Reshost(char *hostname, struct ip46addr *ipaddr);

/**
 * Gets a list of configured DNS servers.
 *
 * @param[in] svr_list  Pointer to a buffer to contain the list.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DNSc_Get_Server_List(qapi_Net_DNS_Server_List_t *svr_list);

/**
 * Adds a DNS server to the system.
 *
 * @param[in] svr_addr  Pointer to the DNS server's IP address string,
 *                      e.g., 8.8.8.8.
 * @param[in] id    Server ID; can be QAPI_NET_DNS_PRIMARY_SERVER_ID,
 *                  QAPI_NET_DNS_SECONDARY_SERVER_ID, or QAPI_NET_DNS_ANY_SERVER_ID.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DNSc_Add_Server(char *svr_addr, uint32_t id);

/**
 * Removes a DNS server from the system.
 *
 * @param[in] id    Server ID; can be QAPI_NET_DNS_PRIMARY_SERVER_ID or
 *                  QAPI_NET_DNS_SECONDARY_SERVER_ID
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_DNSc_Del_Server(uint32_t id);

/**
 * Get host information for an IPv4 host with host name.
 *
 * Implements a standard Unix version of gethostbyname().
 * The returned structure should not be freed by the caller.
 *
 * @param[in] name  Either a hostname or an IPv4 address in standard dot notation.
 *
 * @return
 * On success, a pointer to a hostent structure. \n
 * On error, NULL is returned.
 */
struct qapi_hostent_s * qapi_Net_DNSc_Get_Host_By_Name(char *name);

/**
 * Gets host information for an IPv4/Ipv6 host with host name.
 *
 * Implements a standard Unix version of gethostbyname2().
 * The returned hostent structure is not thread safe. It can be
 * freed by internal DNS client routines if the entry ages out or if the
 * table becomes full and space is needed for another entry.
 *
 * @param[in] name  Either a hostname, an IPv4 address in standard dot notation, or
 *                  an IPv6 address in colon notation.
 * @param[in] af    Address family; either AF_INET or AF_INET6.
 *
 * @return
 * On success, a pointer to a hostent structure. \n
 * On error, NULL is returned.
 */
struct qapi_hostent_s * qapi_Net_DNSc_Get_Host_By_Name2(char *name, int32_t af);

/**
 * Macro that returns a pointer to a hostent struct of a host with the given name.
 */
#define gethostbyname(__name) qapi_Net_DNSc_Get_Host_By_Name(__name)

/**
 * Macro that returns a pointer to a hostent struct of a host with the given name
 * and family (v4 or v6).
 *
 */
#define gethostbyname2(__name, __af) qapi_Net_DNSc_Get_Host_By_Name2(__name, __af)
/** @} */

#endif /* _QAPI_DNSC_H_ */
