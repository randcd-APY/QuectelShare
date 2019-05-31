/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_netprofile.h
 *
 * @addtogroup qapi_networking_netprofile
 * @{
 *
 * @details The Network Profile service provides a collection of API functions
 * that allow the application to configure the networking subsystem in terms
 * of performance versus memory consumption. A larger buffer pool allows a
 * higher possible throughput, and a smaller buffer pool allows the system
 * to run with a smaller memory footprint. There are three predefined profiles that
 * the application can select from, or it can define its own custom profile.
 * It is also possible to configure a custom profile, as well as choose
 * one of the predefined profiles using Device Configuration.
 *
 * @}
 */

#ifndef _QAPI_NETBUF_PROFILE_H_
#define _QAPI_NETBUF_PROFILE_H_

#include <qapi/qapi_status.h>

/** @addtogroup qapi_networking_netprofile
@{ */

/**
*  Predefines the netbuf pool ID. QAPI_NET_PROFILE_CUST_E is used to update a customized netbuf pool.
*/
typedef enum {
    QAPI_NET_PROFILE_PREF_E = 1,
    /**< Maximum throughput (performance) profile. */
    QAPI_NET_PROFILE_BESTEFFORT_E,
    /**< Performance/memory trade-off (best effort) profile. */
    QAPI_NET_PROFILE_MEMOPT_E,
    /**< Minimum memory usage profile. */
    QAPI_NET_PROFILE_CUST_E,
    /**< Custom buffer pool profile. */
    QAPI_NET_PROFILE_MAX = QAPI_NET_PROFILE_CUST_E,
} qapi_Net_Profile_Type_t;


/**
* Structure to hold the netbuf pool entity. Each netbuf entry is the number of the buffer and its size.
*/
typedef struct {
   int32_t buf_Num;
   /**< Initial number of buffers in the queue. */
   int32_t buf_Size;
   /**< Size of each buffer. */
} qapi_Net_Profile_Custom_Pool_t;




/**
* @brief Sets a customized netbuf pool.
*
* @param[in] pNetBuf  Netbuf of type #qapi_Net_Profile_Custom_Pool_t; holds netbuf pool entries.
* @param[in] net_bufq_size  Size of the netbuf pool.
*
* @return
* 0 if operation succeeded, -1 otherwise.
*/
qapi_Status_t qapi_Net_Profile_Set_Custom(qapi_Net_Profile_Custom_Pool_t *pNet_buf, uint8_t net_bufq_size);

/**
* @brief Sets the active profile. The profile can be from the predefined netbuf profiles.
*
* @details Call this function to switch from one netbuf pool profile to another.
*
* @param[in] profile  ID defined in #qapi_Net_Profile_Type_t.
*
* @return
* 0 if operation succeeded, non zero otherwise.
*/
qapi_Status_t qapi_Net_Profile_Set_Active(qapi_Net_Profile_Type_t profile);




/**
* @brief Gets the active profile.
*
* @param[in] pNet_buf  Memory to hold the returned netbuf pool.
* @param[in] net_bufq_size  Number entries in the current pool.
* @param[in] profile  Profile ID of type #qapi_Net_Profile_Type_t.
*
* @return
* 0 if operation succeeded, -1 otherwise.
*/
qapi_Status_t qapi_Net_Profile_Get_Active(qapi_Net_Profile_Custom_Pool_t **pNet_buf, uint8_t *net_bufq_size, qapi_Net_Profile_Type_t *profile);

/** @} */
#endif /* _QAPI_NETBUF_PROFILE_H_ */
