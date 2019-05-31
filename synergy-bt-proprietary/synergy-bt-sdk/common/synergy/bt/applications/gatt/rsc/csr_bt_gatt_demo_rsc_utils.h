#ifndef CSR_BT_GATT_DEMO_RSC_UTILS_H_
#define CSR_BT_GATT_DEMO_RSC_UTILS_H_
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_types.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* returns size of valuepointer in returnValue */
CsrUint8 csrBtRscAppBuildMeasurementValue(CsrUint8 flags,
                                          CsrUint16 speed,
                                          CsrUint8 cadance,
                                          CsrUint16 stride,
                                          CsrUint32 distance,
                                          CsrUint8 **returnValue);


void CsrBtRscAppGetMeasurementValues(CsrUint8 *inputData,
                                     CsrBool *running,
                                     CsrBool *strideAvail,
                                     CsrBool *distanceAvail,
                                     CsrUint16 *speed,
                                     CsrUint8 *cadance,
                                     CsrUint16 *stride,
                                     CsrUint32 *distance);

void CsrBtRscAppGetScControlPointValues(CsrUint8 *inputData,
                                        CsrUint8 *opCode,
                                        CsrUint8 *sensorLoc,
                                        CsrUint8 *requestOpCode,
                                        CsrUint8 *responseValue,
                                        CsrUint8 *responseParam);
#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GATT_DEMO_RSC_UTILS_H_ */
