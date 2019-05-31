#ifndef CSR_BT_AVRCP_APP_PRIM_H__
#define CSR_BT_AVRCP_APP_PRIM_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_prim_defs.h"
#include "csr_sched.h"

#include "csr_pmem.h"
#include "csr_msg_transport.h"
#include "csr_bt_avrcp_app_task.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim CsrBtAvrcpAppPrim;
typedef CsrUint8 CsrBtAvrcpAppCmd;

#define CSR_BT_AVRCP_APP_PLAY_REQ        ((CsrBtAvrcpAppCmd) 0x00)
#define CSR_BT_AVRCP_APP_PAUSE_REQ       ((CsrBtAvrcpAppCmd) 0x01)


#define CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST                          (0x0000)
#define CSR_BT_AVRCP_APP_ACTIVATE_REQ                                             \
            ((CsrBtAvrcpAppPrim) (0x0000 + CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_CONNECT_REQ                                              \
            ((CsrBtAvrcpAppPrim) (0x0001 + CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_DISCONNECT_REQ                                           \
            ((CsrBtAvrcpAppPrim) (0x0002 + CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_PLAY_PAUSE_REQ                                  \
            ((CsrBtAvrcpAppPrim) (0x0003 + CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_HIGHEST                                  \
            ((CsrBtAvrcpAppPrim) (0x0003 + CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST))
            
#define CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_COUNT                                    \
            (CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - \
             CSR_BT_AVRCP_APP_PRIM_DOWNSTREAM_LOWEST)


#define CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST        (0x0000 + CSR_PRIM_UPSTREAM)
#define CSR_BT_AVRCP_APP_CONNECT_IND                                              \
            ((CsrBtAvrcpAppPrim) (0x0000 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_DISCONNECT_IND                                           \
            ((CsrBtAvrcpAppPrim) (0x0001 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_ACTIVATE_CFM                                              \
            ((CsrBtAvrcpAppPrim) (0x0002 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_CONNECT_CFM                                              \
            ((CsrBtAvrcpAppPrim) (0x0003 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_DISCONNECT_CFM                                           \
            ((CsrBtAvrcpAppPrim) (0x0004 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_PLAY_PAUSE_CFM                                  \
            ((CsrBtAvrcpAppPrim) (0x0005 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_APP_PRIM_UPSTREAM_HIGHEST                                    \
            ((CsrBtAvrcpAppPrim) (0x0005 + CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_APP_PRIM_UPSTREAM_COUNT                                      \
            (CSR_BT_AVRCP_APP_PRIM_UPSTREAM_HIGHEST + 1 - \
             CSR_BT_AVRCP_APP_PRIM_UPSTREAM_LOWEST)


typedef struct
{
    CsrBtAvrcpAppPrim   type;
    CsrSchedQid         pHandle;
    CsrBtAvrcpAppCmd    playPauseCmd;
}CsrBtAvrcpAppPlayPauseReq;

#define CsrBtAvrcpAppPlayPauseReqSend(_pHandle, _playPauseCmd)\
{\
    CsrBtAvrcpAppPlayPauseReq* msg = (CsrBtAvrcpAppPlayPauseReq*)CsrPmemAlloc(sizeof(CsrBtAvrcpAppPlayPauseReq));\
    msg->type = CSR_BT_AVRCP_APP_PLAY_PAUSE_REQ;\
    msg->pHandle = _pHandle;\
    msg->playPauseCmd = _playPauseCmd;\
    CsrMsgTransport(CSR_BT_AVRCP_APP_IFACEQUEUE, CSR_BT_AVRCP_APP_PRIM, msg);\
}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AVRCP_APP_PRIM_H__ */

