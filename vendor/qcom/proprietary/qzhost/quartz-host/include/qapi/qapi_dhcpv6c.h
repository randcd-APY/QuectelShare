/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_dhcpv6c.h
 *
 * @brief DHCPv6 Client Service
 *
 * @addtogroup qapi_networking_dhcpv6c
 * @{
 *
 * @details DHCPv6 client is a mechanism to request an IPv6 address
 * from the DHCPv6 server. In order for the DHCPv6 client to obtain
 * an IPv6 address, it must first be enabled by calling the
 * qapi_Net_DHCPv6c_Enable() QAPI function. When this function is
 * called, the DHCPv6 client module internally opens an IPv6 UDP socket
 * on port 546. If a Router Advertisement has already been received
 * on the interface with the M bit set, the DHCPv6 client will send
 * a solicit message to request an IPv6 address. Otherwise, the
 * qapi_Net_DHCPv6c_New_Lease() QAPI function must be called in order
 * to explicitly send the solicit  message to request an IPv6 address.
 * If the host no longer needs the IPv6 address obtained through DHCPv6
 * client, it may call the qapi_Net_DHCPv6c_Release_Lease() QAPI function
 * to release its IPv6 address. Consequently, it may call the
 * qapi_Net_DHCPv6c_Disable() QAPI function to disable the DHCPv6 client,
 * which will internally close the IPv6 UDP socket on port 546 that was
 * previously opened with a call to qapi_Net_DHCPv6c_Enable().
 * The module is using callbacks to notify the caller when a new IPv6
 * address is obtained or release. It is highly advised to register
 * both the DHCPv6c_New_Lease_Complete_CB (callback that is called when a
 * new IPv6 address is obtained) and the DHCPv6c_Release_Lease_Complete_CB
 * (callback that is called when the IPv6 address is released) prior to
 * calling the qapi_Net_DHCPv6c_Enable() QAPI function.
 *
 * The DHCPv6 client can also be used to obtain an IPv6 prefix that would
 * be advertised on some other interface. In order to obtain an IPv6 prefix,
 * the DHCPv6 must be enabled (using the qapi_Net_DHCPv6c_Enable() QAPI function).
 * To lease the actual prefix, the qapi_Net_DHCPv6c_New_Lease() function must
 * be called with the first parameter being the interface name on which the
 * DHCPv6 client is running, and the second parameter being the interface
 * name for which the prefix is desired.
 *
 * To release the prefix, the qapi_Net_DHCPv6c_Release_Lease() function must
 * be called with the first parameter being the interface name on which
 * the DHCPv6 client is running, and the second parameter being the interface
 * name for which the prefix was previously obtained and now is required to be
 * released.
 *
 * @code {.c}
 * // Code snippet demonstrating usage of the DHCPv6 client
 * // Assuming that "wlan1" is the station interface on which
 * // DHCPv6 client will be running, and "wlan0" is the Soft AP
 * // interface for which a prefix is desired.
 *
 * // Register IPv6 lease & release callbacks for "wlan1" interface
 * qapi_Net_DHCPv6c_Register_New_IPv6_Lease_Complete_Callback(
 *   "wlan1",
 *   DHCPv6c_New_IPv6_Lease_Complete_CB,
 *   NULL
 *   );
 * qapi_Net_DHCPv6c_Register_Release_IPv6_Lease_Complete_Callback(
 *   "wlan1",
 *   DHCPv6c_Release_IPv6_Lease_Complete_CB,
 *   NULL
 *   );
 *
 * // Register prefix lease & release callbacks for "wlan0" interface
 * qapi_Net_DHCPv6c_Register_New_Prefix_Lease_Complete_Callback(
 *   "wlan0",
 *   DHCPv6c_New_Prefix_Lease_Complete_CB,
 *   NULL
 *   );
 * qapi_Net_DHCPv6c_Register_Release_Prefix_Lease_Complete_Callback(
 *   "wlan0",
 *   DHCPv6c_Release_Prefix_Lease_Complete_CB,
 *   NULL
 *   );
 *
 * // Enable DHCPv6 client on the "wlan1" interface
 * qapi_Net_DHCPv6c_Enable("wlan1");
 *
 * // Obtain IPv6 address for wlan1 interface
 * qapi_Net_DHCPv6c_New_Lease("wlan1", "wlan1");
 *
 * // Obtain Prefix for "wlan0" interface
 * qapi_Net_DHCPv6c_New_Lease("wlan1", "wlan0");
 *
 * ...
 *
 * // Release the IPv6 address for wlan1 interface
 * qapi_Net_DHCPv6c_Release_Lease("wlan1", "wlan1");
 *
 * // Release the Prefix for the wlan0 interface
 * qapi_Net_DHCPv6c_Release_Lease("wlan1", "wlan0");
 *
 * // Disable the DHCPv6 client
 * qapi_Net_DHCPv6c_Disable("wlan1");
 * @endcode
 *
 * @}
 */


#ifndef _QAPI_NET_DHCPV6C_H_
#define _QAPI_NET_DHCPV6C_H_

#include "stdint.h"
#include "qapi_addr.h"  /* ip6_addr */

/** @addtogroup qapi_networking_dhcpv6c
@{ */


/**
 * Declaration of the callback to be called when the DHCPv6 client leases a new IPv6 address for the interface.
 *
 * @param[in] cb_Ctxt         Opaque context. This is the context that was specified
 *                            to the qapi_Net_DHCPv6c_Register_New_IPv6_Lease_Complete_Callback()
 *                            when this callback was registered.
 * @param[in] interface_Name  Interface name for which the IPv6 address is leased.
 * @param[in] ip6_Address     IPv6 address that is being leased.
 *
 * @return
 * None.
 */
typedef void (* qapi_Net_DHCPv6c_New_IPv6_Lease_Complete_CB_t)(
    void * cb_Ctxt,
    const char * interface_Name,
    const ip6_addr * ip6_Address
    );


/**
 * Declaration of the callback to be called when the DHCPv6 client releases a previously leased IPv6 address for the interface.
 *
 * @param[in] cb_Ctxt         Opaque context. This is the context that was specified
 *                            to the qapi_Net_DHCPv6c_Register_Release_IPv6_Lease_Complete_Callback()
 *                            when this callback was registered.
 * @param[in] interface_Name  Interface name for which the IPv6 address is released.
 * @param[in] ip6_Address     IPv6 address that is being released.
 *
 * @return
 * None.
 */
typedef void (* qapi_Net_DHCPv6c_Release_IPv6_Lease_Complete_CB_t)(
    void * cb_Ctxt,
    const char * interface_Name,
    const ip6_addr * ip6_Address
    );


/**
 * Declaration of the callback to be called when the DHCPv6 client leases a new prefix for the interface.
 *
 * @param[in] cb_Ctxt         Opaque context. This is the context that was specified
 *                            to the qapi_Net_DHCPv6c_Register_New_Prefix_Lease_Complete_Callback()
 *                            when this callback was registered.
 * @param[in] interface_Name  Interface name for which the prefix is leased. This is not the same interface
 *                            on which the DHCPv6 client is running.
 * @param[in] prefix_Base     Leased prefix base address.
 * @param[in] prefix_Length   Leased prefix length.
 *
 * @return
 * None.
 */
typedef void (* qapi_Net_DHCPv6c_New_Prefix_Lease_Complete_CB_t)(
    void * cb_Ctxt,
    const char * interface_Name,
    const ip6_addr * prefix_Base,
    const uint8_t prefix_Length
    );


/**
 * Declaration of the callback to be called when the DHCPv6 client releases a previously leased prefix for the interface.
 *
 * @param[in] cb_Ctxt         Opaque context. This is the context that was specified
 *                            to the qapi_Net_DHCPv6c_Register_Release_Prefix_Lease_Complete_Callback()
 *                            when this callback was registered.
 * @param[in] interface_Name  Interface name for which the prefix is released. This is not the same interface
 *                            on which the DHCPv6 client is running.
 * @param[in] prefix_Base     Released prefix base address.
 * @param[in] prefix_Length   Released prefix length.
 *
 * @return
 * None.
 */
typedef void (* qapi_Net_DHCPv6c_Release_Prefix_Lease_Complete_CB_t)(
    void * cb_Ctxt,
    const char * interface_Name,
    const ip6_addr * prefix_Base,
    const uint8_t prefix_Length
    );


/**
 * Registers a callback to be called when a new IPv6 address is leased
 * by the DHCPv6 client for the interface.
 *
 * @param[in] interface_Name  Name of the interface for which the IPv6 address is to be leased.
 *                            This is the same interface on which the DHCPv6 client is running.
 * @param[in] cb              Callback of type qapi_Net_DHCPv6c_New_IPv6_Lease_Complete_CB_t.
 * @param[in] cb_Ctxt         Opaque context that is to be passed to the callback when
 *                            a new IPv6 address is obtained by the DHCPv6 client.
 *
 * @return
 * 0 if the callback is successfully set for the interface, or an
 * error code otherwise.
 *
 */
int qapi_Net_DHCPv6c_Register_New_IPv6_Lease_Complete_Callback(
    const char * interface_Name,
    qapi_Net_DHCPv6c_New_IPv6_Lease_Complete_CB_t cb,
    void * cb_Ctxt
    );

/**
 * @brief Registers a callback to be called when an existing IPv6 address is
 * released by the DHCPv6 client for the interface.
 *
 * @param[in] interface_Name  Name of the interface for which the IPv6 address is to be released.
 *                            This is the same interface on which the DHCPv6 client is running.
 * @param[in] cb              Callback of type qapi_Net_DHCPv6c_Release_IPv6_Lease_Complete_CB_t.
 * @param[in] cb_Ctxt         Opaque context that is to be passed to the callback when
 *                            an existing IPv6 address is released by the DHCPv6 client.
 *
 * @return
 * 0 if the callback is successfully set for the interface, or an
 * error code otherwise.
 */
int qapi_Net_DHCPv6c_Register_Release_IPv6_Lease_Complete_Callback(
    const char * interface_Name,
    qapi_Net_DHCPv6c_Release_IPv6_Lease_Complete_CB_t cb,
    void * cb_Ctxt
    );


/**
 * Registers a callback to be called when a new prefix is obtained
 * by the DHCPv6 client for the interface.
 *
 * @param[in] interface_Name  Name of the interface for which the prefix is to be leased.
 *                            This is not the same interface
 *                            on which the DHCPv6 client is running. This is the
 *                            interface for which the prefix is being obtained.
 * @param[in] cb              Callback of type qapi_Net_DHCPv6c_New_Prefix_Lease_Complete_CB_t.
 * @param[in] cb_Ctxt         Opaque context that is to be passed to the callback when
 *                            a new prefix is obtained by the DHCPv6 client.
 *
 * @return
 * 0 if the callback is successfully set for the interface, or an
 * error code otherwise.
 */
int qapi_Net_DHCPv6c_Register_New_Prefix_Lease_Complete_Callback(
    const char * interface_Name,
    qapi_Net_DHCPv6c_New_Prefix_Lease_Complete_CB_t cb,
    void * cb_Ctxt
    );

/**
 * Registers a callback to be called when an existing prefix is
 * released by the DHCPv6 client.
 *
 * @param[in] interface_Name  Name of the interface for which the prefix is to be released.
 *                            This is not the same interface
 *                            on which the DHCPv6 client is running. This is the
 *                            interface for which the prefix is being obtained.
 * @param[in] cb              Callback of type qapi_Net_DHCPv6c_Release_Prefix_Lease_Complete_CB_t.
 * @param[in] cb_Ctxt         Opaque context that is to be passed to the callback when
 *                            an existing prefix is released by the DHCPv6 client.
 *
 * @return
 * 0 if the callback is successfully set for the interface, or an
 * error code otherwise.
 */
int qapi_Net_DHCPv6c_Register_Release_Prefix_Lease_Complete_Callback(
    const char * interface_Name,
    qapi_Net_DHCPv6c_Release_Prefix_Lease_Complete_CB_t cb,
    void * cb_Ctxt
    );


/**
 * Enables a DHCPv6 client on a specified interface.
 *
 * When the DHCPv6 client is
 * enabled, it will first check if the interface is managed (M bit was set in the
 * last Router Advertisement). If the interface is managed, the client will
 * automatically send out a DHCPv6 solicit message to obtain an IPv6 address.
 * Otherwise, the user must call qapi_Net_DHCPv6c_New_Lease() to obtain
 * the IPv6 address manually. In either case, when a new address is obtained,
 * a New_Lease_Complete_Callback will be called (assuming it was already registered
 * using the qapi_Net_DHCPv6c_Register_New_IPv6_Lease_Complete_Callback() function).
 *
 * @param[in] interface_Name  Name of the interface on which to enable the DHCPv6 client.
 *
 * @return
 * 0 if the DHCPv6 client has been enabled, or an error code otherwise.
 */
int qapi_Net_DHCPv6c_Enable(const char * interface_Name);

/**
 * Leases a new DHCPv6 IPv6 address or prefix for the specified interface.
 *
 * This function should be called to obtain a new DHCPv6 IPv6 address or a new prefix.
 * When a new IPv6 address is obtained, the qapi_Net_DHCPv6c_New_IPv6_Lease_Complete_CB_t will be called,
 * assuming it was previously registered using the
 * qapi_Net_DHCPv6c_Register_New_IPv6_Lease_Complete_Callback() function. When a new prefix is obtained,
 * the qapi_Net_DHCPv6c_New_Prefix_Lease_Complete_CB_t will be called, assuming it was previously registered
 * using the qapi_Net_DHCPv6c_Register_New_Prefix_Lease_Complete_Callback() function.
 *
 * @param[in] dhcpv6c_Client_Interface_Name  Interface name on which the DHCPv6 client is running.
 * @param[in] interface_Name                 Interface name for which to obtain an identity association.
 *                                           If interface_name == dhcpv6c_client_interface_name, the
 *                                           interface_name will get the IPv6 address from the DHCPv6 server.
 *                                           If interface_name is not the same as dhcpv6c_client_interface_name, the
 *                                           interface_name will get a prefix that it will advertise on interface_name.
 *
 * @return
 * 0 if the DHCPv6 solicit message has been sent,
 * or an error code otherwise.
 */
int qapi_Net_DHCPv6c_New_Lease(const char * dhcpv6c_Client_Interface_Name, const char * interface_Name);

/**
 * Releases a DHCPv6 obtained IPv6 address for a specified interface.
 *
 * This function should be called to release a DHCPv6 obtained IPv6 address or prefix.
 * When the IPv6 address is released, the qapi_Net_DHCPv6c_Release_IPv6_Lease_Complete_CB will be called,
 * assuming it was previously registered using the
 * qapi_Net_DHCPv6c_Register_Release_IPv6_Lease_Complete_Callback() function. If prefix is released,
 * the qapi_Net_DHCPv6c_Release_Prefix_Lease_Complete_CB will be called, assuming it was previously
 * registered using the qapi_Net_DHCPv6c_Register_Release_Prefix_Lease_Complete_Callback() function.
 *
 * @param[in] dhcpv6c_Client_Interface_Name  Interface name on which the DHCPv6 client is running.
 * @param[in] interface_Name                 Interface name for which to release the identity association.
 *                                           If interface_name == dhcpv6c_client_interface_name, the
 *                                           interface_name will release the IPv6 address from the DHCPv6 server.
 *                                           If interface_name is not the same as dhcpv6c_client_interface_name, the
 *                                           interface_name will release a prefix.
 *
 * @return
 * 0 if the DHCPv6 release message has been sent,
 * or an error code otherwise.
 */
int qapi_Net_DHCPv6c_Release_Lease(const char * dhcpv6c_Client_Interface_Name, const char * interface_Name);

/**
 * Confirms a DHCPv6 obtained IPv6 address or prefix for a specified interface.
 *
 * This function should be called to confirm that the DHCPv6 obtained
 * IPv6 address/prefix is still valid on the link. If the IPv6 address is no longer valid,
 * the qapi_Net_DHCPv6c_Release_IPv6_Lease_Complete_CB will be called, assuming it was
 * previously registered using the
 * qapi_Net_DHCPv6c_Register_Release_IPv6_Lease_Complete_Callback() function.
 *
 * @param[in] dhcpv6c_Client_Interface_Name  Interface name on which the DHCPv6 client is running.
 * @param[in] interface_Name                 Interface name for which to confirm the identity association.
 *                                           If interface_name == dhcpv6c_client_interface_name, the
 *                                           interface_name will confirm the IPv6 address from the DHCPv6 server.
 *                                           If interface_name is not the same as dhcpv6c_client_interface_name, the
 *                                           interface_name will confirm a prefix.
 *
 * @return
 * 0 if the DHCPv6 confirm message has been sent,
 * or an error code otherwise.
 */
int qapi_Net_DHCPv6c_Confirm_Lease(const char * dhcpv6c_Client_Interface_Name, const char * interface_Name);

/**
 * Disables the DHCPv6 client on a specified interface.
 *
 * Disables the DHCPv6 client on the specified interface. A DHCPv6 release message
 * will be sent on the interface if it had a DHCPv6 obtained IPv6 address/prefix.
 * This function will also close the associated UDP socket which was used
 * by the DHCPv6 client.
 *
 * @param[in] interface_Name  Name of the interface on which to disable
 *                            the DHCPv6 client.
 *
 * @return
 * 0 if the DHCPv6 client has been disabled, or an error code otherwise.
 */
int qapi_Net_DHCPv6c_Disable(const char * interface_Name);

/** @} */

#endif /* _QAPI_NET_DHCPV6C_H_ */
