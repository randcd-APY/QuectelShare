/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_NS_UTILS_H_
#define _QAPI_NS_UTILS_H_

#include "stdint.h"
#include "qapi/qapi_status.h"

/** @file qapi_ns_utils.h
*
*/

/** @addtogroup qapi_networking_services
@{ */

/** Converts the unsigned integer l from host byte order to network byte order.*/
#undef htonl
#define htonl(l)    qapi_Net_Htonl((l))

/** Converts the unsigned integer l from network byte order to host byte order.*/
#undef ntohl
#define ntohl(l)    qapi_Net_Htonl((l))

/** Converts the unsigned short integer s from host byte order to network byte order.*/
#undef htons
#define htons(s)    qapi_Net_Htons((s))

/** Converts the unsigned short integer s from network byte order to host byte order.*/
#undef ntohs
#define ntohs(s)    qapi_Net_Htons((s))

/** Maximum length for an interface name. */
#define QAPI_NET_IFNAME_LEN     8

/**
 * @brief Network interface object.
 */
typedef struct
{
    uint32_t if_Index;
    /**< ifIndex in rfc1213-mib2, which ranges from 1 to the returned
         value of qapi_Net_Get_Number_of_Interfaces() if the value is @ge 1. */

    char     interface_Name[QAPI_NET_IFNAME_LEN];  /* null terminated name (e.g., "wlan0") */
    /**< Interface name (NULL terminated, e.g., wlan0). */

    qbool_t  if_Is_Up;
    /**< TRUE if the interface is up (active); FALSE if the interface is not up (e.g., down, testing). */

} qapi_Net_Ifnameindex_t;

/**
 * @brief Network socket status.
 */
typedef struct
{
	uint32_t total_sockets;
	/**< Total number of sockets in the system. */
	uint32_t open_af_inet;
	/**< Number of open IPv4/AF_INET sockets. */
	uint32_t open_af_inet6;
	/**< Number of open IPv6/AF_INET6 sockets. */

} qapi_Net_Socket_Status_t;


/**
 * @brief Retrieves the device ID of the WLAN interface.
 *
 * @param[in] interface_Name    Name of the WLAN interface for which to
 *                              retrive device ID.
 *                              Must be wlan0 or wlan1.
 *
 * @return
 * Device ID on success, or -1 on error.
 */
int32_t qapi_Net_Get_Wlan_Dev_Id(const char *interface_Name);

/**
 * @brief Retrieves the device name of the WLAN interface.
 *
 * @code
 *      char *devName;
 *      int devId = 0;
 *
 *      if (qapi_Net_Get_Wlan_Dev_Name(devId, &devName) == 0)
 *      {
 *          printf("Device name of WLAN device %d is %s\n",
 *                  devId, devName);
 *      }
 * @endcode
 *
 * @param[in] dev_ID    Device ID of the WLAN interface for which to retrive the device name.
 *                     Must be @ge 0.
 *
 * @param[out] *pname   Pointer to the device name string.
 *
 * @return
 * 0 on success, or -1 on error.
 */
int32_t qapi_Net_Get_Wlan_Dev_Name(uint32_t dev_ID, const char **pname);

/**
 * @brief Retrieves the textual names of all network interfaces.
 *
 * @param[out] if_Name_Index    Pointer to a buffer that contains the retrieved names.
 *                             If NULL, only 'Number of network interfaces' is returned.
 *
 * @return
 * Number of network interfaces.
 */
int32_t qapi_Net_Get_All_Ifnames(qapi_Net_Ifnameindex_t *if_Name_Index);

/**
 * @brief Retrieves MIB-2 ifIndex.
 *
 * @param[in]  interface_Name  Name of the interface for which to retrive MIB-2 ifIndex.
 *
 * @return
 * MIB-2 ifIndex if OK, -1 if error.
 */
int32_t qapi_Net_Interface_Get_Ifindex(const char *interface_Name);

/**
 * @brief Parses the passed address string into an IPv4/IPv6 address.
 *
 * @param[in]  af      Address family. AF_INET for IPv4, AF_INET6 for IPv6.
 *
 * @param[in]  src     IPv4 or IPv6 address string (NULL-terminated).
 *
 * @param[out] dst     Resulting IPv4/IPv6 address.
 *
 * @return
 * 0 if OK, 1 if bad address format, -1 if AF is not AF_INET or AF_INET6.
 */
int32_t inet_pton(int32_t af, const char *src, void *dst);

/**
 * @brief Formats an IPv4/IPv6 address into a NULL-terminated string.
 *
 * @param[in]  af      Address family. AF_INET for IPv4, AF_INET6 for IPv6.
 *
 * @param[in]  src     Pointer to the IPv4 or IPv6 address.
 *
 * @param[out] dst     Pointer to the output buffer that contains the IPv4/IPv6 address string.
 *
 * @param[out] size    Size of the output buffer in bytes.
 *
 * @return
 * Pointer to the resulting string if OK, else NULL.
 */
const char *inet_ntop(int32_t af, const void *src, char *dst, size_t size);

/**
 * @brief Retrieves the physical address and physical address length of an
 *        interface.
 *
 * @details This function retrieves the physical address and physical address
 * length for the interface. Note that all arguments must not be 0.
 * Also note that this function does not allocate space for the address,
 * and therefore the caller must not free it.
 *
 * @code
 *      int status;
 *      const char * address = 0;
 *      uint32_t address_length = 0;
 *      status = qapi_Net_Interface_Get_Physical_Address("wlan0", &address, &address_length);
 *      if ( status == 0 ) {
 *              // at this point address contains the physical address and
 *              // address_length contains the physical address length
 *              // address[0] is the MSB of the physical address
 *      }
 * @endcode
 *
 * @param[in]  interface_Name  Name of the interface for which to
 *                             retrive the physical address and/or physical
 *                             address length.
 * @param[out] address         Pointer to the address of the buffer
 *                             containing the saved physical address.
 * @param[out] address_Len     Pointer to where the physical address length is stored.
 *
 * @return
 * 0 on success, or a negative error code on failure.
 *
 */
int32_t qapi_Net_Interface_Get_Physical_Address(const char * interface_Name, const uint8_t ** address, uint32_t * address_Len);

/**
 * @brief Checks whether the interface exists.
 *
 * @code
 *      qbool_t exist, if_is_up;
 *
 *      exist = qapi_Net_Interface_Exist("wlan0", &if_is_up);
 *      if ( exist == 1 )
 *      {
 *          if (if_is_up)
 *          {
 *              printf("wlan0 exists and UP\n");
 *          }
 *          else
 *          {
 *              printf("wlan0 exists but DOWN\n");
 *          }
 *      }
 * @endcode
 *
 * @param[in] interface_Name    Name of the interface for which to
 *                              check whether it exists.
 * @param[in]      if_Is_Up         If not NULL, if_Is_Up indicates the RFC1213-MIB2 ifOperStatus of the interface. \n
 *                                  If if_Is_Up is TRUE, the interface is up \n
 *                                  If if_Is_Up is FALSE, the interface is not up
 *
 * @return       FALSE if NOT exist or TRUE if the interface does exist.
 *
 */
qbool_t qapi_Net_Interface_Exist(const char * interface_Name, qbool_t *if_Is_Up);


/**
 * @brief Gets the status of the networking sockets in the system.
 * @details This function gets the total number
 *          of sockets in the system, both open IPv4/AF_INET sockets and open
 *          IPv6/AF_INET6 sockets.
 * @param[out] status  Network socket structure to be filled by this function.
 *
 * @return      QAPI_OK on success, or QAPI_ERROR on error.
 */
qapi_Status_t qapi_Net_Get_Socket_Status(qapi_Net_Socket_Status_t *status);

/**
 * @brief Returns the size of the base64 encoded buffer necessary to encode input_size bytes of data.
 *
 * @param[in] input_Size    Size of the input data.
 *
 * @return Number of bytes required to base64-encode the buffer of size input_size.
 */
size_t qapi_Net_Get_Base64_Encoded_Output_Size(size_t input_Size);

/**
 * @brief Encodes data using base64 encoding.
 *
 * @param[in] input_Buffer    Pointer to the buffer to encode.
 *
 * @param[in] input_Buffer_Size    Size of the buffer to encode.
 *
 * @param[out] output_Buffer   Pointer to the buffer where the encoded data is to be stored.
 *
 * @param[in] output_Buffer_Size  Size of output_Buffer.
 *
 * @return QAPI_OK on success, other value on error.
 */
qapi_Status_t qapi_Net_Base64_Encode(
		const void * input_Buffer,
	    size_t input_Buffer_Size,
	    char * output_Buffer,
	    size_t output_Buffer_Size);

/**
 * @brief Returns the size of the buffer needed to store base64 decoded data, given the base64 encoded data size.
 *
 * @param[in] input_Size    Size of the base64 encoded data
 *
 * @return Number of bytes required to to decode base64 encrypted data into.
 */
size_t qapi_Net_Get_Base64_Decoded_Output_Size(size_t input_Size);

/**
 * @brief Decodes base64 encrypted data.
 *
 * @param[in] input_Buffer    Pointer to the base64 encrypted buffer to decode.
 *
 * @param[in] input_Buffer_Size    Size of the base64 encrypted buffer to decode.
 *
 * @param[out] output_Buffer   Pointer to the buffer where the decoded data is to be stored.
 *
 * @param[in] output_Buffer_Size  Size of output_buffer.
 *
 * @return Number of bytes written into the decoded buffer.
 */
size_t qapi_Net_Base64_Decode(
        const char * input_Buffer,
        size_t input_Buffer_Size,
        void * output_Buffer,
        size_t output_Buffer_Size);

/**
 * @brief Set TCP Keepalive parameters
 *
 * @details The default initial idle time is 7,200 seconds (2 hours).
 *          The default waiting time for the response is 75 seconds after a TCP Keepalive probe is sent.
 *
 * @param[in] idle_Time    Initial idle time (in seconds).
 *                         If idle_Time <= 0, then 7,200 is used.
 *
 * @param[in] resp_Timeout  Waiting time (in seconds) for the response after a TCP Keepalive probe is sent.
 *                          If resp_Timeout <= 0, then 75 is used.
 *
 * @return QAPI_OK on success, other value on error.
 */
qapi_Status_t qapi_Net_TCP_Keepalive_Set(int32_t idle_Time, int32_t resp_Timeout);

/**
 * @brief Get TCP Keepalive parameters.
 *
 * @param[in] idle_Time    Pointer to an int which will contain the initial idle time (in seconds).
 *
 * @param[in] resp_Timeout  Pointer to an int which will contain the waiting time (in seconds) for 
 *                          the response after TCP Keepalive probe is sent.    
 *
 * @return QAPI_OK on success, other value on error.
 */
qapi_Status_t qapi_Net_TCP_Keepalive_Get(int32_t *idle_Time, int32_t *resp_Timeout);

/**
 * @brief Converts the unsigned integer from host byte order to network byte order.
 *
 * @param[in] hostlong  value to be converted.
 *
 * @return  Converted unsigned integer in network byte order.
 */
uint32_t qapi_Net_Htonl(uint32_t hostlong);

/**
 * @brief Converts the unsigned short integer from host byte order to network byte order.
 *
 * @param[in] hostshort value to be converted.
 *
 * @return  Converted unsigned short integer in network byte order.
 */
uint16_t qapi_Net_Htons(uint16_t hostshort);


/** @} */ /* end_addtogroup qapi_networking_services */

#endif /* _QAPI_NS_UTILS_H_ */
