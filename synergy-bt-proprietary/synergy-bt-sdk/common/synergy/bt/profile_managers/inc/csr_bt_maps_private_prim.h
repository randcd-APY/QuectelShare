#ifndef CSR_BT_MAPS_PRIVATE_PRIM_H__
#define CSR_BT_MAPS_PRIVATE_PRIM_H__

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_maps_prim.h"
#include "csr_bt_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtMapsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/**** Controller ****/

#define CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST            (0x0200)
#define CSR_BT_MAPS_ADD_NOTI_REQ                              ((CsrBtMapsPrim) (0x0000 + CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPS_REMOVE_NOTI_REQ                           ((CsrBtMapsPrim) (0x0001 + CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPS_SERVICE_CLEANUP                           ((CsrBtMapsPrim) (0x0002 + CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_HIGHEST           (0x0002 + CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST)

#define CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_LOWEST              (CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST + CSR_PRIM_UPSTREAM)
#define CSR_BT_MAPS_ADD_NOTI_CFM                              ((CsrBtMapsPrim) (0x0000 + CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPS_REMOVE_NOTI_CFM                           ((CsrBtMapsPrim) (0x0001 + CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_HIGHEST             (0x0001 + CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_LOWEST)

#define CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_COUNT             (CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_HIGHEST + 1 - CSR_BT_MAPS_PRIM_DOWNSTREAM_PRIVATE_LOWEST)
#define CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_COUNT               (CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_HIGHEST + 1 - CSR_BT_MAPS_PRIM_UPSTREAM_PRIVATE_LOWEST)

/***********************************************************************************
 Private Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtMapsPrim         type;
    CsrBtDeviceAddr       deviceAddr;    /* remote device address */
    CsrBtConnId           masConnId;     /* MAS connection id for which this notification belongs to */
    CsrUint8              masInstanceId; /* MASInstanceID from SDP record */
    CsrSchedQid           mapHandle;
    CsrSchedQid           appHandle;
    dm_security_level_t   security;
    CsrUint16             maxFrameSize;
    CsrUint16             windowSize;
} CsrBtMapsAddNotiReq;

typedef struct
{
    CsrBtMapsPrim         type;
    CsrBtResultCode       resultCode;
    /* CsrBtSupplier         resultSupplier; */
} CsrBtMapsAddNotiCfm;

typedef struct
{
    CsrBtMapsPrim         type;
    CsrBtDeviceAddr       deviceAddr;    /* remote device address */
    CsrSchedQid           mapHandle;
    CsrSchedQid           appHandle;
} CsrBtMapsRemoveNotiReq;

typedef struct
{
    CsrBtMapsPrim         type;
    CsrBtResultCode       resultCode;
    /* CsrBtSupplier         resultSupplier; */
} CsrBtMapsRemoveNotiCfm;

typedef struct
{
    CsrBtMapsPrim         type;
    CsrUint8              obexInstId;
} CsrBtMapsServiceCleanup;


/***********************************************************************************
 * End Private Primitive typedefs
 ***********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_MAPS_PRIVATE_PRIM_H__ */

