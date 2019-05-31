/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_sntpc.h
 *
 * @addtogroup qapi_networking_sntpc
 * @{
 *
 * @details The Simple Network Time Protocol (SNTP) client service provides
 * a collection of API functions that allow the application to enable automatic
 * acquisition of time from the network using SNTP.
 * Once successful, the SNTP client will update the system time and the
 * application thread will be able to use the Time Services API to get the
 * calendar time.
 *
 * @}
 */

#ifndef _QAPI_SNTPC_H_
#define _QAPI_SNTPC_H_

#include "stdint.h"

/** @addtogroup qapi_networking_sntpc
@{ */

/** The default SNTP server if the server hostname/address is not specified. */
#define QAPI_NET_SNTP_DEFAULT_SERVER        "pool.ntp.org"

/**
 *  The system keeps two SNTP servers with SERVER_ID 0 and 1.
 *  When adding a server to the system, ANY_SERVER_ID means to add
 *  the server as either primary or secondary.
 */
#define QAPI_NET_SNTP_SERVER_MAX            2

/** Primary server ID. */
#define QAPI_NET_SNTP_PRIMARY_SERVER_ID     0

/** Secondary server ID. */
#define QAPI_NET_SNTP_SECONDARY_SERVER_ID   1

/** Any server ID. */
#define QAPI_NET_SNTP_ANY_SERVER_ID         0xFFFF

/** Byte length of the SNTP server name (DNS name or IPv4/IPv6 address). */
#define QAPI_NET_SNTP_SERVER_NAME_SIZE      68

/**
 * @brief Commands to start/stop/disable an SNTP client.
 */
typedef enum {
    QAPI_NET_SNTP_DISABLE_E,
    /**< Stop plus free the space for internal data structures. */

    QAPI_NET_SNTP_START_E,
    /**< Allocate space for internal data structures, send SNTP requests, and
         process SNTP responses from the server. */

    QAPI_NET_SNTP_STOP_E
    /**< Stop sending SNTP requests and processing SNTP responses. Keep
         internal data structures. */

} qapi_Net_SNTP_Command_t;

/** Status of the SNTP server: Sent a KOD */   /* means that this server sent a KOD. */
#define QAPI_NET_SNTP_SERVER_STATUS_KOD     1

/** Status of SNTP server: No reply */   /* means that this server did not reply. */
#define QAPI_NET_SNTP_SERVER_STATUS_NORESP  2

/** IP address string length. @newpage */
#define QAPI_NET_IP_ADDR_STR_LEN     48


/**
 * @brief For use with qapi_Net_SNTPc_Get_Server_List() to get server information.
 */
typedef struct {
    char name[QAPI_NET_SNTP_SERVER_NAME_SIZE];
    /**< DNS name or IP address string. */

    char addr[QAPI_NET_IP_ADDR_STR_LEN];
    /**< IP address string. */

    uint32_t status;
    /**< Server status: 0, QAPI_NET_SNTP_SERVER_STATUS_KOD or QAPI_NET_SNTP_SERVER_STATUS_NORESP. */
} sntp_svr_entry_t;

typedef struct {
    sntp_svr_entry_t    svr[QAPI_NET_SNTP_SERVER_MAX];
} qapi_Net_SNTP_Server_List_t;

/**
 * @brief Time broken down.
 */
typedef struct
{
    uint32_t  tm_sec;
    /**< Seconds after the miniute    -- 0 to 60. */

    uint32_t  tm_min;
    /**< Miniutes after the hour      -- 0 to 59. */

    uint32_t  tm_hour;
    /**< Hours since midnight         -- 0 to 23. */

    uint32_t  tm_mday;
    /**< Day of the month             -- 1 to 31. */

    uint32_t  tm_mon;
    /**< Months since January         -- 0 to 11. */

    uint32_t  tm_year;
    /**< Years since 1900             -- @ge 0. */

    uint32_t  tm_wday;
    /**< Day of the week; Sunday = 0  -- 0 to 6. */

    uint32_t  tm_yday;
    /**< Day in the year; Jan. 1 = 0 -- 0 to 365. */

    uint32_t  tm_isdst;
    /**< Daylight savings time. */
} qapi_Net_SNTP_Tm_t;

/**
 * @brief Check whether the SNTP client is started.
 *
 * @return
 * 0 if not started or 1 if started.
 */
int32_t qapi_Net_SNTPc_Is_Started(void);

/**
 * @brief Starts, stops, or disables the SNTP client.
 *
 * @param[in] cmd   Command to start/stop/disable the SNTP client.
 *                  The supported commands are QAPI_NET_SNTP_DISABLE_E,
 *                  QAPI_NET_SNTP_START_E, and QAPI_NET_SNTP_STOP_E.
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_SNTPc_Command(qapi_Net_SNTP_Command_t cmd);

/**
 * @brief Adds an SNTP server to the system.
 *
 * @details After SNTP client starts, the server can be added to the system.
 *
 * @param[in] name  Pointer to the SNTP server's name or IP address string.
 *
 * @param[in] ID    Server ID; can be QAPI_NET_SNTP_PRIMARY_SERVER_ID,
 *                  QAPI_NET_SNTP_SECONDARY_SERVER_ID, or QAPI_NET_SNTP_ANY_SERVER_ID.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_SNTPc_Add_Server(char *name, uint32_t ID);

/**
 * @brief Removes an SNTP server from the system.
 *
 * @details After the SNTP client starts, the server can be removed from the system.
 *
 * @param[in] ID    Server ID; can be QAPI_NET_SNTP_PRIMARY_SERVER_ID or
 *                  QAPI_NET_SNTP_SECONDARY_SERVER_ID.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_SNTPc_Del_Server(uint32_t ID);

/**
 * @brief Gets the list of configured SNTP servers.
 *
 * @param[in] svr_List  Pointer to a buffer to contain the server list.
 *
 * @return On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_SNTPc_Get_Server_List(qapi_Net_SNTP_Server_List_t *svr_List);

/**
 * @brief Gets the broken down time. See #qapi_Net_SNTP_Tm_t.
 *
 * @param[in] time  Pointer to a buffer to contain the broken down time.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_Net_SNTPc_Get_Brokendown_Time(qapi_Net_SNTP_Tm_t *time);

/** @} */

#endif /* _QAPI_SNTPC_H_ */
