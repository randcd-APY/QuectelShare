/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2011-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
// $QTI_LICENSE_QDN_C$

/**
* @file qapi_wlan_prof.h
*
* @brief WLAN API for Profiling tool only available with integrated IPStack
*
* @details Provide API to install performance measurement, record stats in data path and measure delay in thread context switch
*
*/

#ifndef QAPI_PROFILER_H
#define QAPI_PROFILER_H

/**
* @brief Enum of Command passed to .qapi_Prof_Cmd_Handler
* @details Define command enum that is passed to driver
*/
typedef enum {
    QAPI_PROF_INSTALL_E = 0, /** INSTALL performance tool */
    QAPI_PROF_UNINSTALL_E,   /** Uninstall tool */
    QAPI_PROF_START_E,       /** Start measuring performance*/
    QAPI_PROF_STOP_E,        /** Stop measuring */
    QAPI_PROF_RESET_E,       /** Reset the stats */
    QAPI_PROF_GET_STATS_E,   /** Get recording stats */
    QAPI_PROF_MAX_CMD_E,     
} qapi_Prof_Cmd_t;

/**
* @brief Structure to hold all performance stats
* @details This structure is used hold different stats vertically across data path layers
*/
typedef struct
{
    uint32_t    avg_Time_Outside_Freeq;    /**< avg time netbuf in usage: enqueueTimestamp - dequeueTimestamp to/from freeQueue */
    uint32_t    avg_Wait_Time_In_Tx_Queue; /**< netbuf time waiting in wlan txqueue */
    uint32_t    avg_Socket_BlockTime;      /**< how long was avg socket block call */
    uint32_t    netbuf_Low_Watermark_0;    /**< netbuf queue0 low watermark hit any time since system up time */
    uint32_t    netbuf_Low_Watermark_1;    /**< netbuf queue1 low watermark hit any time since system up time */
    uint32_t    netbuf_Low_Watermark_2;    /**< netbuf queue2 low watermark hit any time since system up time */
    uint32_t    test_Runtime;              /**< Total time profiling test run */
    uint32_t    rx_Pkt_Count;              /**< Number of received packets */
    uint32_t    tx_Pkt_Count;              /**< Number of transmitted packets */
    uint32_t    wlan_Rx_Bytes;             /**< Number of bytes received by WLAN driver */
    uint32_t    wlan_Tx_Bytes;             /**< Number of bytes transmitted from WLAN driver */
    uint32_t    bus_Rx_Bytes;              /**< Number of bytes received by the bus */
    uint32_t    bus_Tx_Bytes;              /**< Number of bytes transmitted from the bus */
    uint32_t    bus_Rw_Error;              /**< Read/Write error on the bus */
} qapi_Prof_Stats_t;

/**
* @brief API that handle all Profiling Command.
*
* @details Call this API to execute any Profiling command defined in "qapi_Prof_Cmd_t".
*
* @param[in]      cmd  Command defined in qapi_Prof_Cmd_t.
* @param[in]      data Data pointer using to pass information from/to caller and driver
*
* @return       0 if operation succeeded, -1 otherwise.
*/
int8_t qapi_Prof_Cmd_Handler(qapi_Prof_Cmd_t cmd, qapi_Prof_Stats_t *data);

#endif //QAPI_PROFILER_H
