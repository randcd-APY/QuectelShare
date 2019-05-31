/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_RAMDUMP_H_
#define _QAPI_RAMDUMP_H_

/**
@file qapi_ramdump.h
This section provides APIs, macros definitions, enumerations and data structures
for applications to perform ramdump.
*/

#include <stdint.h>
#include "qapi/qapi_status.h"

/** @addtogroup qapi_ramdump
@{ */

/**
@ingroup qapi_ramdump
Describe ramdump state.
*/
typedef enum 
{
    QAPI_RAMDUMP_STATE_NONE_E = 1,          /**< No ramdump is flashed. */
    QAPI_RAMDUMP_STATE_FLASHED_E = 2,       /**< Ramdump is flashed. */
} qapi_Ramdump_State_e;

/**
@ingroup qapi_ramdump
Describe ftp configuration.
*/
typedef struct
{
    char            *ip_Version;
    char            *ftps_Ip_Addr;
    char            *login_Name;
    char            *login_Password;
    char            *path;
    uint16_t        data_Port;
    uint16_t        cmd_Port;
} qapi_Ftpc_Config_t;

#define QAPI_RAMDUMP_SERVER_TYPE_FTP    "ftp"

#define QAPI_FTPC_IP_V4                 "v4"
#define QAPI_FTPC_IP_V6                 "v6"

#define QAPI_RAMDUMP_SERVER_CFG_LEN	    16

/**
@ingroup qapi_ramdump
Data structure used by the application to pass ramdump parameters.
*/
typedef struct
{
    char            *server_Type;
    union {
        uint32_t            cfg[QAPI_RAMDUMP_SERVER_CFG_LEN];
        qapi_Ftpc_Config_t  ftpc_Cfg;
    } svc_U;
    int             encryption;
    uint32_t        *encrypt_Cfg;
} qapi_Ramdump_Config_t;

/**
@ingroup qapi_ramdump
Get ramdump state.

@param[out] pState  qapi_Ramdump_State_e.

@return
QAPI_OK -- Get the right ramdump state. \n
Nonzero value -- Not get the ramdump state.
*/
qapi_Status_t qapi_Ramdump_Get_State (qapi_Ramdump_State_e *pQ_State);

/**
@ingroup qapi_ramdump
Do ramdump.

@param[in] pRamdump_Params  qapi_Ramdump_Params_t.

@return
QAPI_OK -- Ramdump ok. \n
Nonzero value -- Ramdump fail.
*/
qapi_Status_t qapi_Ramdump_Handle (const qapi_Ramdump_Config_t *pQ_Ramdump_Config);

#endif
