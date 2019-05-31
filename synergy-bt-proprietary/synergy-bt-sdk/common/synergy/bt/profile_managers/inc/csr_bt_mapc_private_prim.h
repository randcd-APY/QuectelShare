#ifndef CSR_BT_MAPC_PRIVATE_PRIM_H__
#define CSR_BT_MAPC_PRIVATE_PRIM_H__

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_mapc_prim.h"
#include "csr_bt_addr.h"
#include "dm_prim.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtMapcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/**** Controller ****/

#define CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST            (0x0200)
#define CSR_BT_MAPC_ADD_NOTI_REQ                              ((CsrBtMapcPrim) (0x0000 + CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPC_REMOVE_NOTI_REQ                           ((CsrBtMapcPrim) (0x0001 + CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPC_SERVICE_CLEANUP                           ((CsrBtMapcPrim) (0x0002 + CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_HIGHEST           (0x0002 + CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST)

#define CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_LOWEST              (CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST + CSR_PRIM_UPSTREAM)
#define CSR_BT_MAPC_ADD_NOTI_CFM                              ((CsrBtMapcPrim) (0x0000 + CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPC_REMOVE_NOTI_CFM                           ((CsrBtMapcPrim) (0x0001 + CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_HIGHEST             (0x0001 + CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_LOWEST)

#define CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_COUNT             (CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_HIGHEST + 1 - CSR_BT_MAPC_PRIM_DOWNSTREAM_PRIVATE_LOWEST)
#define CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_COUNT               (CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_HIGHEST + 1 - CSR_BT_MAPC_PRIM_UPSTREAM_PRIVATE_LOWEST)

/***********************************************************************************
 Private Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtMapcPrim         type;
    CsrBtDeviceAddr       deviceAddr;    /* remote device address */
    /* may not be required since mapHandle will be unique; check if this is rqeuired */
    /* Ok this may be rquired when we get the event notification to send it to the right application */
    CsrUint8              masInstanceId; /* MASInstanceID from SDP record */
    CsrSchedQid           mapHandle;
    CsrSchedQid           appHandle;
    dm_security_level_t   security;
    CsrUint16             maxFrameSize;
    CsrUint16             windowSize;
} CsrBtMapcAddNotiReq;

typedef struct
{
    CsrBtMapcPrim         type;
    CsrBtResultCode       resultCode; /* Obex result codes from csr_bt_obex.h */
} CsrBtMapcAddNotiCfm;

typedef struct
{
    CsrBtMapcPrim         type;
    CsrBtDeviceAddr       deviceAddr;    /* remote device address */
    CsrUint8              masInstanceId; /* MASInstanceID from SDP record */
    CsrSchedQid           mapHandle;
    CsrSchedQid           appHandle;
    CsrBool               forceDisc;
} CsrBtMapcRemoveNotiReq;

typedef struct
{
    CsrBtMapcPrim         type;
    CsrBtResultCode       resultCode; /* Obex result codes from csr_bt_obex.h */
} CsrBtMapcRemoveNotiCfm;

typedef struct
{
    CsrBtMapcPrim         type;
    CsrUint8              obexInstId;
} CsrBtMapcServiceCleanup;

/***********************************************************************************
 * End Private Primitive typedefs
 ***********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_MAPC_PRIVATE_PRIM_H__ */
