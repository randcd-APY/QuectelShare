#ifndef CSR_BT_BSL_PRIM_H__
#define CSR_BT_BSL_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2001-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_bnep_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtBslPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim                      CsrBtBslPrim;
typedef CsrUint16                          CsrBtBslPacketType;
typedef CsrUint8                           CsrBtBslStatus;
typedef CsrUint8                           CsrBtBslEvent;

/* ---------- Defines the BSL CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_BSL_SUCCESS                  ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_BSL_UNACCEPTABLE_PARAMETER   ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_BSL_CONNECT_CANCELLED        ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_BSL_CONNECT_LIMIT            ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_BSL_UNDEFINED_SERVICE        ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_BSL_ABNORMAL_LINK_DISCONNECT ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_BSL_UNEXPECTED_PARAMETER     ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_BSL_TIMEOUT                  ((CsrBtResultCode) (0x0007))

/* Defines for Unicast Data */
#define CSR_BT_BSL_ID_DEST_ADDR                  BNEP_ID_DST

/* Defines for Multicast Data */
#define CSR_BT_BSL_ID_MULTI_TO_LOCAL             ((CsrUint16) (0x01) )
#define CSR_BT_BSL_ID_MULTI_NO_LOCAL             BNEP_ID_LOCAL


#define CSR_BT_MAX_NUM_OF_NETW_PACKET_TYPE       50

#define CSR_BT_BSL_LINK_STATE_EVENT              ((CsrBtBslEvent) 0x0000)
#define CSR_BT_BSL_SWITCH_ROLE_EVENT             ((CsrBtBslEvent) 0x0001)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_BSL_REGISTER_REQ                  ((CsrBtBslPrim) (0x0000 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_ACTIVATE_REQ                  ((CsrBtBslPrim) (0x0001 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_CONNECT_REQ                   ((CsrBtBslPrim) (0x0002 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_DATA_REQ                      ((CsrBtBslPrim) (0x0003 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_MULTICAST_DATA_REQ            ((CsrBtBslPrim) (0x0004 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_DISCONNECT_REQ                ((CsrBtBslPrim) (0x0005 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_DISCONNECT_RES                ((CsrBtBslPrim) (0x0006 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_SERVICE_SEARCH_REQ            ((CsrBtBslPrim) (0x0007 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_SECURITY_IN_REQ               ((CsrBtBslPrim) (0x0008 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_SECURITY_OUT_REQ              ((CsrBtBslPrim) (0x0009 + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BSL_CANCEL_CONNECT_REQ            ((CsrBtBslPrim) (0x000A + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BSL_PRIM_DOWNSTREAM_HIGHEST                    (0x000A + CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_BSL_HOUSE_CLEANING                ((CsrBtBslPrim) (CSR_BT_BSL_PRIM_DOWNSTREAM_HIGHEST + 1))

/*******************************************************************************/

#define CSR_BT_BSL_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BSL_ACTIVATE_CFM                  ((CsrBtBslPrim) (0x0000 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_CONNECT_IND                   ((CsrBtBslPrim) (0x0001 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_DATA_IND                      ((CsrBtBslPrim) (0x0002 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_DISCONNECT_IND                ((CsrBtBslPrim) (0x0003 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_STATUS_IND                    ((CsrBtBslPrim) (0x0004 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_SERVICE_SEARCH_CFM            ((CsrBtBslPrim) (0x0005 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_SERVICE_SEARCH_RESULT_IND     ((CsrBtBslPrim) (0x0006 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_SECURITY_IN_CFM               ((CsrBtBslPrim) (0x0007 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_SECURITY_OUT_CFM              ((CsrBtBslPrim) (0x0008 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BSL_DATA_CFM                      ((CsrBtBslPrim) (0x0009 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BSL_PRIM_UPSTREAM_HIGHEST                      (0x0009 + CSR_BT_BSL_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_BSL_PRIM_DOWNSTREAM_COUNT         (CSR_BT_BSL_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BSL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BSL_PRIM_UPSTREAM_COUNT           (CSR_BT_BSL_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BSL_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrUint16                protocolVersion;
    CsrBool                  loadFactorIncluded;
    CsrUint8                 loadFactor;
    CsrUint16                bluetoothProfileVersion;
    CsrUint16                securityDescription;
    CsrCharString                *serviceName;
    CsrCharString                *serviceDescription;
    CsrUint16                languageElementsCount;
    CsrBtLanguageElement    *languageElements;
    CsrUint16                numOfSupportedNetwPacketTypes;
    CsrUint16                supportedNetwPacketType[CSR_BT_MAX_NUM_OF_NETW_PACKET_TYPE];
    CsrBool                  netAccessTypeIncluded;
    CsrUint16                netAccessType;
    CsrBool                  maxNetAccessRateIncluded;
    CsrUint32                maxNetAccessRate;
    CsrBool                  ipv4SubnetIncluded;
    CsrCharString                *ipv4Subnet;
    CsrBool                  ipv6SubnetIncluded;
    CsrCharString                *ipv6Subnet;
} CsrBtBslSearchRecord;

typedef struct
{
    CsrBtBslPrim               type;
} CsrBtBslHouseCleaning;

typedef struct
{
    CsrBtBslPrim               type;
    CsrSchedQid               connPhandle;            /* protocol handle for connection manager */
    CsrSchedQid               dataPhandle;            /* protocol handle for default data manager */
} CsrBtBslRegisterReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrBool                  singleUser;
    CsrBtBslPanRole            localRole;
    CsrBtBslPanRole            remoteRole;
} CsrBtBslActivateReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtBslActivateCfm;

typedef struct
{
    CsrBtBslPrim               type;
    BD_ADDR_T               bdAddr;
    CsrBtBslPanRole            localRole;
    CsrBtBslPanRole            remoteRole;
} CsrBtBslConnectReq;

typedef struct
{
    CsrBtBslPrim            type;
    CsrUint16                id;
    BD_ADDR_T               bdAddr;
    CsrBtBslPanRole         localRole;
    CsrBtBslPanRole         remoteRole;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtBslConnectInd;

typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
} CsrBtBslDisconnectReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
} CsrBtBslDisconnectRes;

typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier     reasonSupplier;
} CsrBtBslDisconnectInd;

/* NOTE: this type should be identical with the CM extended data definition in
   order to pass the signal through without any new memory allocations */
typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr;
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtBslDataReq;

/* NOTE: this type should be identical with the CM extended data definition in
   order to pass the signal through without any new memory allocations */
typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr; /* note may be multicast */
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtBslDataInd;

/* NOTE: this type should be identical with the CM extended multicast data definition in
   order to pass the signal through without any new memory allocations */
typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                idNot;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr; /* note may be multicast */
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtBslMulticastDataReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
    CsrBtBslEvent              event;
    CsrUint8                 status;
} CsrBtBslStatusInd;

typedef struct
{
    CsrBtBslPrim               type;
    CsrUint16                id;
} CsrBtBslDataCfm;

typedef struct
{
    CsrBtBslPrim               type;
    CsrSchedQid               phandle;    /* routing handle */
    BD_ADDR_T               bd_addr;    /* Address of Bluetooth device to be searched */
    CsrBtBslPanRole            searchRole;    /* Perform sdp search on these roles */
} CsrBtBslServiceSearchReq;

typedef struct
{
    CsrBtBslPrim               type;
    BD_ADDR_T               bd_addr;    /* Address of Bluetooth device to be searched */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtBslServiceSearchCfm;

typedef struct
{
    CsrBtBslPrim               type;
    BD_ADDR_T               bd_addr;     /* Address of Bluetooth device to be searched */
    CsrBool                  moreResults; /* True if more records found and more primitives will follow */
    CsrBtBslPanRole            searchRole;
    CsrBtBslSearchRecord       searchResult;
} CsrBtBslServiceSearchResultInd;

typedef struct
{
    CsrBtBslPrim               type;
    CsrSchedQid               appHandle;
    CsrUint16                secLevel;
} CsrBtBslSecurityInReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrSchedQid               appHandle;
    CsrUint16                secLevel;
} CsrBtBslSecurityOutReq;

typedef struct
{
    CsrBtBslPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtBslSecurityInCfm;

typedef struct
{
    CsrBtBslPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtBslSecurityOutCfm;

typedef struct
{
    CsrBtBslPrim               type;
    BD_ADDR_T               bdAddr;
} CsrBtBslCancelConnectReq;

#ifdef __cplusplus
}
#endif

#endif

