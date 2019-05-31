#ifndef CSR_HCI_PRIM_H__
#define CSR_HCI_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrHciPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrHciPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_HCI_PRIM_DOWNSTREAM_LOWEST                                        (0x0000)

#define CSR_HCI_REGISTER_EVENT_HANDLER_REQ                      ((CsrHciPrim) (0x0000 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_REGISTER_ACL_HANDLER_REQ                        ((CsrHciPrim) (0x0001 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_ACL_HANDLER_REQ                      ((CsrHciPrim) (0x0002 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_REGISTER_SCO_HANDLER_REQ                        ((CsrHciPrim) (0x0003 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_SCO_HANDLER_REQ                      ((CsrHciPrim) (0x0004 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_REGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_REQ      ((CsrHciPrim) (0x0005 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_REQ    ((CsrHciPrim) (0x0006 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_COMMAND_REQ                                     ((CsrHciPrim) (0x0007 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_ACL_DATA_REQ                                    ((CsrHciPrim) (0x0008 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_SCO_DATA_REQ                                    ((CsrHciPrim) (0x0009 + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_VENDOR_SPECIFIC_COMMAND_REQ                     ((CsrHciPrim) (0x000A + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_HCI_INCOMING_DATA_REQ                               ((CsrHciPrim) (0x000B + CSR_HCI_PRIM_DOWNSTREAM_LOWEST))

#define CSR_HCI_PRIM_DOWNSTREAM_HIGHEST                                       (0x000B + CSR_HCI_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_HCI_PRIM_UPSTREAM_LOWEST                                          (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_HCI_REGISTER_EVENT_HANDLER_CFM                      ((CsrHciPrim) (0x0000 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_REGISTER_ACL_HANDLER_CFM                        ((CsrHciPrim) (0x0001 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_ACL_HANDLER_CFM                      ((CsrHciPrim) (0x0002 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_REGISTER_SCO_HANDLER_CFM                        ((CsrHciPrim) (0x0003 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_SCO_HANDLER_CFM                      ((CsrHciPrim) (0x0004 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_REGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_CFM      ((CsrHciPrim) (0x0005 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_UNREGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_CFM    ((CsrHciPrim) (0x0006 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_EVENT_IND                                       ((CsrHciPrim) (0x0007 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_ACL_DATA_IND                                    ((CsrHciPrim) (0x0008 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_SCO_DATA_IND                                    ((CsrHciPrim) (0x0009 + CSR_HCI_PRIM_UPSTREAM_LOWEST))
#define CSR_HCI_VENDOR_SPECIFIC_EVENT_IND                       ((CsrHciPrim) (0x000A + CSR_HCI_PRIM_UPSTREAM_LOWEST))

#define CSR_HCI_PRIM_UPSTREAM_HIGHEST                                         (0x000A + CSR_HCI_PRIM_UPSTREAM_LOWEST)

#define CSR_HCI_PRIM_DOWNSTREAM_COUNT         (CSR_HCI_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_HCI_PRIM_DOWNSTREAM_LOWEST)
#define CSR_HCI_PRIM_UPSTREAM_COUNT           (CSR_HCI_PRIM_UPSTREAM_HIGHEST + 1 - CSR_HCI_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId to which incoming HCI events should be sent */
} CsrHciRegisterEventHandlerReq;

typedef struct
{
    CsrHciPrim type;
} CsrHciRegisterEventHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId to which incoming HCI ACL data on a given acl handle should be sent */
    CsrUint16   aclHandle;          /* the acl handle a given queueId wants to receive data from.
                                       NB: a value of 0xFFFF means that the queueId will receive incoming acl data
                                       from all handles which hasn't explicit been registred for by other queueIds */
} CsrHciRegisterAclHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  aclHandle;           /* The acl handle a given task registered to */
} CsrHciRegisterAclHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId that wish to usubscribe from incoming HCI ACL data for a given acl handle */
    CsrUint16   aclHandle;          /* the acl handle a given queueId wants to unsubscribe from */
} CsrHciUnregisterAclHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  aclHandle;           /* The acl handle a given task unsubscribed from */
} CsrHciUnregisterAclHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId to which incoming HCI SCO data on a given sco handle should be sent */
    CsrUint16   scoHandle;          /* the sco handle a given queueId wants to receive sco data from. */
} CsrHciRegisterScoHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  scoHandle;           /* the sco handle registered to */
} CsrHciRegisterScoHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId that wish to usubscribe from incoming HCI SCO data for a given sco handle */
    CsrUint16   scoHandle;          /* the sco handle a given queueId wants to unsubscripe from. */
} CsrHciUnregisterScoHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  scoHandle;           /* the sco handle unsubscriped from. */
} CsrHciUnregisterScoHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId to which incoming vendor specific HCI event data on a given channel should be sent */
    CsrUint8    channel;            /* the channel a given queueId wants to receive data from. NB: Valid ranges are 15-63 */
} CsrHciRegisterVendorSpecificEventHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint8   channel;             /* the channel a given queueId registred to */
} CsrHciRegisterVendorSpecificEventHandlerCfm;

typedef struct
{
    CsrHciPrim  type;
    CsrSchedQid queueId;                 /* queueId that wish to usubscribe from incoming vendor specific HCI event data for a given channel */
    CsrUint8    channel;            /* the channel a given queueId wants to receive data from. NB: Valid ranges for technologies are 15-63 */
} CsrHciUnregisterVendorSpecificEventHandlerReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint8   channel;             /* the channel a given queueId unsubscribed from */
} CsrHciUnregisterVendorSpecificEventHandlerCfm;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  payloadLength;       /* length of the payload */
    CsrUint8  *payload;             /* Pointer to complete formatted HCI command. */
} CsrHciCommandReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  payloadLength;       /* length of the payload */
    CsrUint8  *payload;             /* Pointer to complete formatted HCI event. */
} CsrHciEventInd;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  handlePlusFlags;     /* handle is 12 lsb, flags 4 msb */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciAclDataReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  handlePlusFlags;     /* handle is 12 lsb, flags 4 msb */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciAclDataInd;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  handlePlusFlags;     /* handle is 12 lsb, flags 4 msb */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciScoDataReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint16  handlePlusFlags;     /* handle is 12 lsb, flags 4 msb */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciScoDataInd;

typedef struct
{
    CsrHciPrim type;
    CsrUint8   channel;             /* the channel on which data should be sent */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciVendorSpecificCommandReq;

typedef struct
{
    CsrHciPrim type;
    CsrUint8   channel;             /* the channel on which data were received */
    CsrMblk   *data;                /* Pointer to MBLK data. */
} CsrHciVendorSpecificEventInd;

typedef struct
{
    CsrHciPrim type;
    CsrUint8   channel;             /* channel */
    CsrUint16  dataLen;             /* reported data length from lower layers */
    CsrMblk   *data;                /* Pointer to incoming data. */
} CsrHciIncomingDataReq;

#ifdef __cplusplus
}
#endif

#endif
