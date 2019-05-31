#ifndef CSR_HCI_LIB_H__
#define CSR_HCI_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_hci_prim.h"
#include "csr_msg_transport.h"
#ifdef CSR_BTM_TASK
#include "csr_btm_task.h"
#define CSR_HCI_IFACEQUEUE CSR_BTM_IFACEQUEUE
#else
#include "csr_hci_task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_HCI_REGISTER_EVENT_HANDLER_REQ

   Description
        Register HCI event handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciRegisterEventHandlerReq *CsrHciRegisterEventHandlerReq_struct(CsrSchedQid queueId);
#define CsrHciRegisterEventHandlerReqSend(_queueId){ \
        CsrHciRegisterEventHandlerReq *__msg; \
        __msg = CsrHciRegisterEventHandlerReq_struct(_queueId); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_REGISTER_ACL_HANDLER_REQ

   Description
        Register HCI ACL handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciRegisterAclHandlerReq *CsrHciRegisterAclHandlerReq_struct(CsrSchedQid queueId, CsrUint16 aclHandle);
#define CsrHciRegisterAclHandlerReqSend(_queueId, _aclHandle){ \
        CsrHciRegisterAclHandlerReq *__msg; \
        __msg = CsrHciRegisterAclHandlerReq_struct(_queueId, _aclHandle); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_UNREGISTER_ACL_HANDLER_REQ

   Description
        Unregister HCI ACL handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciUnregisterAclHandlerReq *CsrHciUnregisterAclHandlerReq_struct(CsrSchedQid queueId, CsrUint16 aclHandle);
#define CsrHciUnregisterAclHandlerReqSend(_queueId, _aclHandle){ \
        CsrHciUnregisterAclHandlerReq *__msg; \
        __msg = CsrHciUnregisterAclHandlerReq_struct(_queueId, _aclHandle); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_REGISTER_SCO_HANDLER_REQ

   Description
        Register HCI SCO handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciRegisterScoHandlerReq *CsrHciRegisterScoHandlerReq_struct(CsrSchedQid queueId, CsrUint16 scoHandle);
#define CsrHciRegisterScoHandlerReqSend(_queueId, _scoHandle){ \
        CsrHciRegisterScoHandlerReq *__msg; \
        __msg = CsrHciRegisterScoHandlerReq_struct(_queueId, _scoHandle); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_UNREGISTER_SCO_HANDLER_REQ

   Description
        Unregister HCI SCO handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciUnregisterScoHandlerReq *CsrHciUnregisterScoHandlerReq_struct(CsrSchedQid queueId, CsrUint16 scoHandle);
#define CsrHciUnregisterScoHandlerReqSend(_queueId, _scoHandle){ \
        CsrHciUnregisterScoHandlerReq *__msg; \
        __msg = CsrHciUnregisterScoHandlerReq_struct(_queueId, _scoHandle); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_REGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_REQ

   Description
        Register Vendor Specific HCI event handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciRegisterVendorSpecificEventHandlerReq *CsrHciRegisterVendorSpecificEventHandlerReq_struct(CsrSchedQid queueId, CsrUint8 channel);
#define CsrHciRegisterVendorSpecificEventHandlerReqSend(_queueId, _channel){ \
        CsrHciRegisterVendorSpecificEventHandlerReq *__msg; \
        __msg = CsrHciRegisterVendorSpecificEventHandlerReq_struct(_queueId, _channel); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_UNREGISTER_VENDOR_SPECIFIC_EVENT_HANDLER_REQ

   Description
        Unregister Vendor Specific HCI event handler with the CsrHci task

 * -------------------------------------------------------------------- */
CsrHciUnregisterVendorSpecificEventHandlerReq *CsrHciUnregisterVendorSpecificEventHandlerReq_struct(CsrSchedQid queueId, CsrUint8 channel);
#define CsrHciUnregisterVendorSpecificEventHandlerReqSend(_queueId, _channel){ \
        CsrHciUnregisterVendorSpecificEventHandlerReq *__msg; \
        __msg = CsrHciUnregisterVendorSpecificEventHandlerReq_struct(_queueId, _channel); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_COMMAND_REQ

   Description
        Send HCI command

 * -------------------------------------------------------------------- */
CsrHciCommandReq *CsrHciCommandReq_struct(CsrUint16 payloadLength, CsrUint8 *payload);
#define CsrHciCommandReqSend(_payloadLength, _payload){ \
        CsrHciCommandReq *__msg; \
        __msg = CsrHciCommandReq_struct(_payloadLength, _payload); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_ACL_DATA_REQ

   Description
        Send HCI ACL data

 * -------------------------------------------------------------------- */
CsrHciAclDataReq *CsrHciAclDataReq_struct(CsrUint16 handlePlusFlags, CsrMblk *data);
#define CsrHciAclDataReqSend(_handlePlusFlags, _data){ \
        CsrHciAclDataReq *__msg; \
        __msg = CsrHciAclDataReq_struct(_handlePlusFlags, _data); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_SCO_DATA_REQ

   Description
        Send HCI SCO data

 * -------------------------------------------------------------------- */
CsrHciScoDataReq *CsrHciScoDataReq_struct(CsrUint16 handlePlusFlags, CsrMblk *data);
#define CsrHciScoDataReqSend(_handlePlusFlags, _data){ \
        CsrHciScoDataReq *__msg; \
        __msg = CsrHciScoDataReq_struct(_handlePlusFlags, _data); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HCI_VENDOR_SPECIFIC_COMMAND_REQ

   Description
        Send Vendor Specific HCI command data

 * -------------------------------------------------------------------- */
CsrHciVendorSpecificCommandReq *CsrHciVendorSpecificCommandReq_struct(CsrUint8 channel, CsrMblk *data);
#define CsrHciVendorSpecificCommandReqSend(_channel, _data){ \
        CsrHciVendorSpecificCommandReq *__msg; \
        __msg = CsrHciVendorSpecificCommandReq_struct(_channel, _data); \
        CsrMsgTransport(CSR_HCI_IFACEQUEUE, CSR_HCI_PRIM, __msg); \
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrHciFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR HCI upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_HCI_PRIM,
 *      message:      The message received from CSR HCI
 *----------------------------------------------------------------------------*/
void CsrHciFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
