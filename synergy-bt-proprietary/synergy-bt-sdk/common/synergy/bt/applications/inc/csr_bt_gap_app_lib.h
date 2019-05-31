#ifndef CSR_BT_GAP_APP_LIB_H__
#define CSR_BT_GAP_APP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_msg_transport.h"
#include "csr_bt_profiles.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bt_gap_app_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_BT_GAP_APP_GET_SELECTED_DEVICE_REQ

   Description
       Ask the GAP task what is the currently selected device

 * -------------------------------------------------------------------- */
CsrBtGapAppGetSelectedDeviceReq *CsrBtGapAppGetSelectedDeviceReq_struct(CsrSchedQid pHandle);
#define CsrBtGapAppGetSelectedDeviceReqSend(_pHandle){\
    CsrBtGapAppGetSelectedDeviceReq *msg;\
    msg = CsrBtGapAppGetSelectedDeviceReq_struct(_pHandle);\
    CsrMsgTransport(CSR_BT_GAP_APP_IFACEQUEUE,CSR_BT_GAP_APP_PRIM,msg);\
}

/* --------------------------------------------------------------------
   Name
       CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_REQ

   Description
       Ask the GAP task to perform a proximity search and pair to the found device

 * -------------------------------------------------------------------- */
CsrBtGapAppPairProximityDeviceReq *CsrBtGapAppPairProximityDeviceReq_struct(CsrSchedQid pHandle,
                                                                            CsrBtClassOfDevice  classOfDevice,
                                                                            CsrBtClassOfDevice  classOfDeviceMask,
                                                                            CsrBool performBonding);
#define CsrBtGapAppPairProximityDeviceReqSend(_pHandle, _cod, _codm, _pb){\
    CsrBtGapAppPairProximityDeviceReq *msg;\
    msg = CsrBtGapAppPairProximityDeviceReq_struct(_pHandle, _cod, _codm, _pb);\
    CsrMsgTransport(CSR_BT_GAP_APP_IFACEQUEUE,CSR_BT_GAP_APP_PRIM,msg);\
}


#ifdef __cplusplus
}
#endif

#endif
