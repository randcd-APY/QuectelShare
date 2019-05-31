/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_gap_app_lib.h"
#include "csr_pmem.h"

/* --------------------------------------------------------------------
   Name
       CSR_BT_GAP_APP_GET_SELECTED_DEVICE_REQ

   Description
       Ask the GAP task what is the currently selected device

 * -------------------------------------------------------------------- */
CsrBtGapAppGetSelectedDeviceReq *CsrBtGapAppGetSelectedDeviceReq_struct(CsrSchedQid pHandle)
{
    CsrBtGapAppGetSelectedDeviceReq *prim;

    prim = (CsrBtGapAppGetSelectedDeviceReq *) CsrPmemAlloc(sizeof(CsrBtGapAppGetSelectedDeviceReq));
    prim->type = CSR_BT_GAP_APP_GET_SELECTED_DEVICE_REQ;
    prim->phandle = pHandle;

    return prim;
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
                                                                            CsrBool performBonding)
{
    CsrBtGapAppPairProximityDeviceReq *prim;

    prim                    = (CsrBtGapAppPairProximityDeviceReq *) CsrPmemAlloc(sizeof(CsrBtGapAppPairProximityDeviceReq));
    prim->type              = CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_REQ;
    prim->phandle           = pHandle;
    prim->classOfDevice     = classOfDevice;
    prim->classOfDeviceMask = classOfDeviceMask;
    prim->performBonding    = performBonding;
    return prim;
}
