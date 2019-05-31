#ifndef CSR_BT_AMPM_PRIM_H__
#define CSR_BT_AMPM_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2012 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_mblk.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_amp_hci.h"
#include "l2cap_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAmpmPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* AMPM specific types */
typedef CsrPrim   CsrBtAmpmPrim;
typedef CsrUint8  CsrBtAmpmType;
typedef CsrUint8  CsrBtAmpmId;
typedef CsrUint16 CsrBtAmpmResult;
typedef CsrUint16 CsrBtAmpmControl;

/* The invalid AMP ID */
#define CSR_BT_AMPM_INVALID_ID                                  ((CsrBtAmpmId)0)     /* Invalid AMP, same value as BR/EDR */

/* AMPM error codes for the CSR_BT_SUPPLIER_AMPM */
#define CSR_BT_AMPM_RESULT_SUCCESS                              ((CsrBtAmpmResult)0x0000) /* Success */
#define CSR_BT_AMPM_RESULT_ERROR                                ((CsrBtAmpmResult)0x0001) /* Internal unknown error */
#define CSR_BT_AMPM_RESULT_TOO_MANY_PALS                        ((CsrBtAmpmResult)0x0002) /* Could not register any more PALs */
#define CSR_BT_AMPM_RESULT_NO_ID                                ((CsrBtAmpmResult)0x0003) /* Could not assign ID for PAL */
#define CSR_BT_AMPM_RESULT_ALREADY_REGISTERED                   ((CsrBtAmpmResult)0x0004) /* AMPM is already active */
#define CSR_BT_AMPM_RESULT_NO_LOCAL_AMPS                        ((CsrBtAmpmResult)0x0005) /* No local AMPs have been registered */
#define CSR_BT_AMPM_RESULT_NO_REMOTE_AMPS                       ((CsrBtAmpmResult)0x0006) /* No remote AMPs found */
#define CSR_BT_AMPM_RESULT_NOT_REGISTERED                       ((CsrBtAmpmResult)0x0007) /* Application has not registered */
#define CSR_BT_AMPM_RESULT_INVALID_PARAM                        ((CsrBtAmpmResult)0x0008) /* Invalid parameter(s) */
#define CSR_BT_AMPM_RESULT_SSP_NOT_ENABLED                      ((CsrBtAmpmResult)0x0009) /* Security mode 4 (SSP) not enabled */
#define CSR_BT_AMPM_RESULT_NO_REMOTE_AMP_MANAGER                ((CsrBtAmpmResult)0x000a) /* Peer does not have an AMP manager */
#define CSR_BT_AMPM_RESULT_NO_CONNECTION                        ((CsrBtAmpmResult)0x000b) /* No connection to peer */
#define CSR_BT_AMPM_RESULT_PEER_PROTOCOL_ERROR                  ((CsrBtAmpmResult)0x000c) /* Peer has violated the A2MP protocol */
#define CSR_BT_AMPM_RESULT_SECURITY_FAILURE                     ((CsrBtAmpmResult)0x000d) /* SSP/AMP link key failure */
#define CSR_BT_AMPM_RESULT_MOVE_RSP_TIMEOUT                     ((CsrBtAmpmResult)0x000e) /* Application did not send move response */
#define CSR_BT_AMPM_RESULT_WRONG_STATE                          ((CsrBtAmpmResult)0x000f) /* Move req/rsp received in wrong state */

/* AMPM event bit masks */
#define CSR_BT_AMPM_EVENTS_DISABLE_ALL                          ((CsrUint32)0x00000000) /* Keep quiet */
#define CSR_BT_AMPM_EVENTS_ENABLE_ALL                           ((CsrUint32)0xFFFFFFFF) /* All changes */
#define CSR_BT_AMPM_EVENTS_AUTO_DISCOVERY                       ((CsrUint32)0x00000001) /* Automatic remote controller discovery */
#define CSR_BT_AMPM_EVENTS_AUTO_MOVE                            ((CsrUint32)0x00000002) /* Info app about automatic moves */

/* AMPM flags bit mask */
#define CSR_BT_AMPM_FLAGS_NONE                                  ((CsrUint32)0x00000000) /* No flags */
#define CSR_BT_AMPM_FLAGS_AUTO_MOVE_ACCEPT                      ((CsrUint32)0x00000001) /* Auto-accept move indications */
#define CSR_BT_AMPM_FLAGS_AUTO_MOVE_ALWAYS                      ((CsrUint32)0x00000002) /* Auto-initiate move on AMP availability */
#define CSR_BT_AMPM_FLAGS_ENABLE_TEST                           ((CsrUint32)0x80000000) /* Enable test mode. Only for internal CSR use */

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

/* Application interface, downstream */
#define CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST                  ((CsrUint16)0x0000)
#define CSR_BT_AMPM_REGISTER_REQ                          ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_DEREGISTER_REQ                        ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_CONTROLLER_REQ                        ((CsrBtAmpmPrim)(0x0002 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_MOVE_REQ                              ((CsrBtAmpmPrim)(0x0003 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_MOVE_RES                              ((CsrBtAmpmPrim)(0x0004 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_REGISTER_POWER_ON_REQ                 ((CsrBtAmpmPrim)(0x0005 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_POWER_ON_RES                          ((CsrBtAmpmPrim)(0x0006 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_APP_PRIM_DOWN_HIGHEST                 ((CsrUint16)(0x0006 + CSR_BT_AMPM_APP_PRIM_DOWN_LOWEST))

/* PAL interface, downstream */
#define CSR_BT_AMPM_PAL_PRIM_DOWN_LOWEST                  ((CsrUint16)0x0100)
#define CSR_BT_AMPM_PAL_REGISTER_REQ                      ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_PAL_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_PAL_DEREGISTER_REQ                    ((CsrBtAmpmPrim)(0x0002 + CSR_BT_AMPM_PAL_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_PAL_HCI_EVENT_REQ                     ((CsrBtAmpmPrim)(0x0003 + CSR_BT_AMPM_PAL_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_PAL_PRIM_DOWN_HIGHEST                 ((CsrUint16)(0x0003 + CSR_BT_AMPM_PAL_PRIM_DOWN_LOWEST))

/* Test interface, downstream */
#define CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST                 ((CsrUint16)0x0200)
#define CSR_BT_AMPM_TEST_TIMER_CONTROL_REQ                ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_SDU_REQ                          ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_ROUTE_RECEIVED_SDU_REQ           ((CsrBtAmpmPrim)(0x0002 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_CANCEL_CREATE_PHYSICAL_LINK_REQ  ((CsrBtAmpmPrim)(0x0003 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_IGNORE_SDU_REQ                   ((CsrBtAmpmPrim)(0x0004 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_CHANGE_AMP_KEY_REQ               ((CsrBtAmpmPrim)(0x0005 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_DONT_START_PHY_LINK_REQ          ((CsrBtAmpmPrim)(0x0006 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_DONT_START_LOG_LINK_REQ          ((CsrBtAmpmPrim)(0x0007 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_PHY_COLLISION_REQ                ((CsrBtAmpmPrim)(0x0008 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_GET_ASSOC_REQ                    ((CsrBtAmpmPrim)(0x0009 + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_HCI_REQ                          ((CsrBtAmpmPrim)(0x000A + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_TEST_PRIM_DOWN_HIGHEST                ((CsrUint16)(0x000A + CSR_BT_AMPM_TEST_PRIM_DOWN_LOWEST))

/* Profile interface, downstream */
#define CSR_BT_AMPM_PRF_PRIM_DOWN_LOWEST                  ((CsrUint16)0x0300)
#define CSR_BT_AMPM_PRF_MULTI_CONN_REQ                    ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_PRF_PRIM_DOWN_LOWEST))
#define CSR_BT_AMPM_PRF_PRIM_DOWN_HIGHEST                 ((CsrUint16)(0x0000 + CSR_BT_AMPM_PRF_PRIM_DOWN_LOWEST))

/* Application interface, upstream */
#define CSR_BT_AMPM_APP_PRIM_UP_LOWEST                    ((CsrUint16)0x0000 + CSR_PRIM_UPSTREAM)
#define CSR_BT_AMPM_REGISTER_CFM                          ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_DEREGISTER_CFM                        ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_CONTROLLER_CFM                        ((CsrBtAmpmPrim)(0x0002 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_CONTROLLER_IND                        ((CsrBtAmpmPrim)(0x0003 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_MOVE_IND                              ((CsrBtAmpmPrim)(0x0004 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_MOVE_CFM                              ((CsrBtAmpmPrim)(0x0005 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_MOVE_CMP_IND                          ((CsrBtAmpmPrim)(0x0006 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_AUTO_MOVE_CMP_IND                     ((CsrBtAmpmPrim)(0x0007 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_POWER_ON_IND                          ((CsrBtAmpmPrim)(0x0008 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_APP_PRIM_UP_HIGHEST                   ((CsrUint16)(0x0008 + CSR_BT_AMPM_APP_PRIM_UP_LOWEST))

/* PAL interface, upstream */
#define CSR_BT_AMPM_PAL_PRIM_UP_LOWEST                    ((CsrUint16)0x0100 + CSR_PRIM_UPSTREAM)
#define CSR_BT_AMPM_PAL_REGISTER_CFM                      ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_PAL_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_PAL_DEREGISTER_CFM                    ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_PAL_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_PAL_HCI_COMMAND_IND                   ((CsrBtAmpmPrim)(0x0002 + CSR_BT_AMPM_PAL_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_PAL_PRIM_UP_HIGHEST                   ((CsrUint16)(0x0002 + CSR_BT_AMPM_PAL_PRIM_UP_LOWEST))

/* Test interface, upstream */
#define CSR_BT_AMPM_TEST_PRIM_UP_LOWEST                   ((CsrUint16)0x200 + CSR_PRIM_UPSTREAM)
#define CSR_BT_AMPM_TEST_RECEIVED_SDU_IND                 ((CsrBtAmpmPrim)(0x0000 + CSR_BT_AMPM_TEST_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_TEST_HCI_IND                          ((CsrBtAmpmPrim)(0x0001 + CSR_BT_AMPM_TEST_PRIM_UP_LOWEST))
#define CSR_BT_AMPM_TEST_PRIM_UP_HIGHEST                  ((CsrUint16)(0x0001 + CSR_BT_AMPM_TEST_PRIM_UP_LOWEST))

/*******************************************************************************
 * Shared structures
 *******************************************************************************/

/* AMP controller pair information element */
typedef struct
{
    CsrBtAmpAmpControllerType ampType;       /* Type of controller (use CSR_BT_AMP_AMP_CONTROLLER_TYPE_) */
    CsrBtAmpAmpStatus status;                /* Availability (use CSR_BT_AMP_AMP_STATUS_) */
    CsrUint32         caps;                  /* Reserved. Always zero.  */
    CsrBtAmpController localId;               /* Local AMP controller ID */
    CsrBtAmpController remoteId;              /* Remote AMP controller ID */
} CsrBtAmpmControllerEntry;

/*******************************************************************************
 * Primitive signal type definitions - APPLICATION INTERFACE
 *******************************************************************************/

/* Allows an upper layer task to register for receiving indications
 * from AMP Manager */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_REGISTER_REQ */
    CsrSchedQid       qid;                   /* Application handle */
    CsrUint32         flags;                 /* Flags (use CSR_BT_AMPM_FLAGS_) */
    CsrUint32         eventMask;             /* Event mask (use CSR_BT_AMPM_EVENTS_) */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address (can be NULL if btConnId is invalid) */
} CsrBtAmpmRegisterReq;

/* Confirmation of registration with AMP Manager */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_REGISTER_CFM */
    CsrUint32         eventMask;             /* Event mask in use */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address */
    CsrBtAmpmResult   resultCode;            /* Result code */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
} CsrBtAmpmRegisterCfm;

/* Unregister previously registered application from AMPM */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_DEREGISTER_REQ */
    CsrSchedQid       qid;                   /* Registered task queue */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address */
} CsrBtAmpmDeregisterReq;

/* Unregistration complete */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_DEREGISTER_CFM */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address */
    CsrBtAmpmResult   resultCode;            /* Result code */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
} CsrBtAmpmDeregisterCfm;

/* Request AMP controller discovery. Note: There is no confirm for
 * this signal - an IND is used instead */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_CONTROLLER_REQ */
    CsrSchedQid       qid;                   /* Application handle */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address */
} CsrBtAmpmControllerReq;

/* AMP controller discovery confirm */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_CONTROLLER_CFM | IND */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtDeviceAddr   addr;                  /* Bluetooth address these results relate to */
    CsrUint8          ampsCount;             /* Number of elements in amps */
    CsrBtAmpmControllerEntry *amps;          /* Pointer to array of AMP controllers */
    CsrBtAmpmResult   resultCode;            /* Result code */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
} CsrBtAmpmControllerCfm;

/* AMP controller discovery indication */
typedef CsrBtAmpmControllerCfm CsrBtAmpmControllerInd;

/* AMP move channel request */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_MOVE_REQ */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtAmpController localId;               /* Local AMP controller ID */
    CsrBtAmpController remoteId;              /* Remote AMP controller ID */
} CsrBtAmpmMoveReq;

/* AMP move channel response */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_MOVE_RES */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBool           accept;                /* Accept or reject move */
} CsrBtAmpmMoveRes;

/* AMP move channel indication */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_MOVE_IND */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtAmpController localId;               /* Local AMP controller ID */
} CsrBtAmpmMoveInd;

/* AMP move channel confirm */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_MOVE_CFM | MOVE_CMP_IND | AUTO_MOVE_CMP_IND */
    CsrBtConnId       btConnId;              /* Global Bluetooth connection ID */
    CsrBtAmpController localId;              /* Local AMP controller ID */
    CsrBtAmpmResult   resultCode;            /* Result code */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
} CsrBtAmpmMoveCfm;

/* AMP move channel complete indication */
typedef CsrBtAmpmMoveCfm CsrBtAmpmMoveCmpInd;

/* AMP automatic move channel complete indication */
typedef CsrBtAmpmMoveCfm CsrBtAmpmAutoMoveCmpInd;

/* Register app to receive indications when an AMP controller power on
   is needed, i.e. upon a move attempt when the controller is off - the
   registered app is responsible for powering on the local AMP controller */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_REGISTER_POWER_ON_REQ */
    CsrSchedQid       qid;                   /* Registered task queue */
} CsrBtAmpmRegisterPowerOnReq;

/* AMP controller power on indication */
typedef struct
{
    CsrBtAmpmPrim               type;       /* Identity: CSR_BT_AMPM_POWER_ON_IND */
    CsrBtAmpAmpControllerType   ampType;    /* Type of controller (use CSR_BT_AMP_AMP_CONTROLLER_TYPE_) */
    CsrBtAmpController          localId;    /* Local AMP controller ID */
    CsrBool                     localInit;  /* Set when triggered by a locally initiated move request */
} CsrBtAmpmPowerOnInd;

/* AMP controller power on response */
typedef struct
{
    CsrBtAmpmPrim               type;       /* Identity: CSR_BT_AMPM_POWER_ON_RES */
    CsrBtAmpAmpControllerType   ampType;    /* Type of controller (use CSR_BT_AMP_AMP_CONTROLLER_TYPE_) */
    CsrBtAmpController          localId;    /* Local AMP controller ID */
    CsrBool                     complete;   /* Power on completed or rejected/failed */
} CsrBtAmpmPowerOnRes;

/*******************************************************************************
 * Primitive signal type definitions - PAL INTERFACE
 *******************************************************************************/

/* Register a PAL with the AMP manager */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_REGISTER_REQ */
    CsrBtAmpmType     ampType;               /* AMP Type */
    CsrSchedQid       controlQid;            /* Task queue to receive HCI commands */
    CsrSchedQid       dataQid;               /* Task queue to receive outgoing ACL data */
    CsrUint32         flags;                 /* Flags. Reserved - must all be zero */
} CsrBtAmpmPalRegisterReq;

/* Confirmation of a PAL registration */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_REGISTER_REQ */
    CsrBtAmpmType     ampType;               /* Same type as in the request */
    CsrBtAmpmId       ampId;                 /* Assigned AMP ID for this controller */
    CsrBtAmpmResult   result;                /* Result code for the registration */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
    CsrSchedQid       controlQid;            /* Task queue to receive HCI events */
    CsrSchedQid       dataQid;               /* Task queue to receive incoming ACL data */
    CsrUint32         flags;                 /* Flags. Reserved - all are set to zero */
} CsrBtAmpmPalRegisterCfm;

/* Unregister the specified PAL named by the ampId */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_DEREGISTER_REQ */
    CsrBtAmpmId       ampId;                 /* Identifier of the AMP that is deregistering */
} CsrBtAmpmPalDeregisterReq;

/* Confirmation that a PAL has been unregistered. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_DEREGISTER_REQ */
    CsrBtAmpmId       ampId;                 /* AMP ID of controller that has been deregistered */
    CsrBtAmpmResult   result;                /* Result code for the deregistration */
    CsrBtSupplier     resultSupplier;        /* Result error code supplier */
} CsrBtAmpmPalDeregisterCfm;

/* AMP HCI event from the PAL to the AMP manager (ie. direction
 * dictates this is a request). The actual HCI event is in the 'data'
 * block, and uses the format(s) given in csr_bt_amp_hci.h. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_HCI_EVENT_REQ */
    CsrUint8          ampId;                 /* Target controller AMP ID */
    CsrUint16         length;                /* Length of 'data' block */
    CsrUint8         *data;                  /* HCI command data */
} CsrBtAmpmPalHciEventReq;

/* AMP HCI command sent from AMP manager to the PAL (ie. direction
 * dictates this is an indication). The actual HCI command is in the
 * 'data' block, and uses the format(s) given in csr_bt_amp_hci.h. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PAL_HCI_COMMAND_IND */
    CsrUint8          ampId;                 /* Target controller AMP ID */
    CsrUint16         length;                /* Length of 'data' block */
    CsrUint8         *data;                  /* HCI command data */
} CsrBtAmpmPalHciCommandInd;

/*******************************************************************************
 * Primitive signal type definitions - LOW LEVEL TEST INTERFACE
 *******************************************************************************/

/* Request to enable or disable timers within AMP Manager */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: AMPM_TIMER_CONTROL_REQ */
    CsrBool           enabled;               /* Enable or disable test control timers */
} CsrBtAmpmTestTimerControlReq;

/* Requests the AMP Manager to send the supplied SDU to the specified
 * Bluetooth device over the AMP manager channel. Allows invalid SDUs
 * to be sent to the specified destination as an aid to robustness and
 * IOP testing. Depending on the SDU sent it can also allow the sender
 * to create unexpected SDU sequences at the receiver of the
 * SDUs. However it is not possible to generate all sequences using
 * this method as there is no control over when the SDU will be
 * sent. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_SDU_REQ */
    CsrBtDeviceAddr   bdAddr;                /* Remote Bluetooth Address */
    CsrUint16         length;
    CsrUint8         *data;
} CsrBtAmpmTestSduReq;

/* Requests the AMP Manager to route received SDUs to the qid based on
 * the parameters of the primitive:
 * AMPM_RX_SDU_SEND_COPY: Sends a copy of all received SDUs to
 *    the qid whilst still handling the received SDU.
 * AMPM_RX_SDU_SEND_ORIGINAL: Sends all received SDUs to the
 *    qid without handling them
 * AMPM_RX_SDU_DISABLE: No received SDUs are sent to the qid */
#define CSR_BT_AMPM_CONTROL_RX_SDU_DISABLED        ((CsrBtAmpmControl)0x000) /* Let AMPM handle SDUs/HCIs */
#define CSR_BT_AMPM_CONTROL_RX_SDU_SEND_COPY       ((CsrBtAmpmControl)0x001) /* Sends a copy of SDU to tester */
#define CSR_BT_AMPM_CONTROL_RX_SDU_SEND_ORIGINAL   ((CsrBtAmpmControl)0x002) /* Sends original SDU to tester */
#define CSR_BT_AMPM_CONTROL_RX_HCI_SEND_ORIGINAL   ((CsrBtAmpmControl)0x004) /* Sends original HCI to tester */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_ROUTE_RECEIVED_SDU_REQ */
    CsrBtAmpmControl  rxSduControl;          /* How to route SDUs */
    CsrSchedQid       qid;                   /* Handle to receive routed SDUs */
} CsrBtAmpmTestRouteReceivedSduReq;

/* Requests the AMP Manager to cancel all physical link requests that
 * it generates. As it is not possible to correctly sequence the
 * sending of a cancel request via the CSR_BT_AMPM_TEST_SDU_REQ this
 * primitive provides the mechanism for generating it */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_CANCEL_CREATE_PHYSICAL_LINK_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
    CsrBool           sendAfterResponse;     /* Send cancel after phylink response */
    CsrBool           invalidResponderAmpId; /* Cancel with invalid AMP id */
    CsrUint32         delayAfterReq;         /* Send cancel this long after. 0 is immediately */
} CsrBtAmpmTestCancelCreatePhysicalLinkReq;

/* Requests the AMP Manager to ignore a specific AMP SDU request from
 * all devices */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_IGNORE_SDU_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
    CsrUint8          sduId;                 /* Ignore this opcode type, 0 means all */
    CsrUint8          ignoreTimes;           /* Times to ignore. Zero means infinite. */
} CsrBtAmpmTestIgnoreSduReq;

/* Requests the AMP Manager to change the key supplied in the HCI
 * CREATE PHYSICAL LINK CMD for the specified AMP type */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_CHANGE_AMP_KEY_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
    CsrBtAmpmType     ampType;               /* AMP controller type to change keys for */
    CsrUint8          changeTimes;           /* Times to change. Zero means infinite. */
} CsrBtAmpmTestChangeAmpKeyReq;

/* Request AMPM to not start physical link in order to test failure
 * scenarios. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_DONT_START_PHY_LINK_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
    CsrBtAmpmType     ampType;               /* AMP controller type to change behaviour for */
} CsrBtAmpmTestDontStartPhyLinkReq;

/* Request AMPM to not start logical link in order to test failure
 * scenarios. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_DONT_START_LOG_LINK_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
} CsrBtAmpmTestDontStartLogLinkReq;

/* Request AMPM to trigger a create physical link collision when an
   AMP create physical link request is received. */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_PHY_COLLISION_REQ */
    CsrBool           enabled;               /* Enabled or disables the behaviour */
} CsrBtAmpmTestPhyCollisionReq;

/* AMP Manager sends a received SDU to the SAP */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_SDU_REQ */
    CsrBtConnId       btConnId;              /* BT global connection ID */
    CsrBtDeviceAddr   bdAddr;                /* Remote Bluetooth Address */
    CsrUint16         length;                /* Length of 'data' */
    CsrUint8         *data;                  /* The raw SDU data */
} CsrBtAmpmTestReceivedSduInd;

/* AMPM Manager sends a get assoc request for testing purposes */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_GET_ASSOC_REQ */
    CsrUint8          signalId;              /* Signal identifier */
    CsrBtAmpmId       ampId;                 /* AMP controller ID */
    CsrBtDeviceAddr   bdAddr;                /* Remote Bluetooth Address */
} CsrBtAmpmTestGetAssocReq;

/* AMP Manager raw HCI command */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_HCI_REQ */
    CsrUint8          ampId;                 /* Target controller AMP ID */
    CsrUint16         length;                /* Length of 'data' block */
    CsrUint8         *data;                  /* HCI command data */
} CsrBtAmpmTestHciReq;

/* AMP Manager raw HCI event */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_TEST_HCI_IND */
    CsrUint8          ampId;                 /* Target controller AMP ID */
    CsrUint16         length;                /* Length of 'data' block */
    CsrUint8         *data;                  /* HCI command data */
} CsrBtAmpmTestHciInd;

/*******************************************************************************
 * Primitive signal type definitions - PROFILE HINT INTERFACE
 *******************************************************************************/

/* Profile hints about multiple sub-channels for a master */
typedef struct
{
    CsrBtAmpmPrim     type;                  /* Identity: CSR_BT_AMPM_PRF_MULTI_CONN_REQ */
    CsrBtConnId       master;                /* Master btConnId */
    CsrUint8          slavesCount;           /* Number of slaves */
    CsrBtConnId      *slaves;                /* Slave btConnIds */
} CsrBtAmpmPrfMultiConnReq;

#ifdef __cplusplus
}
#endif

#endif
