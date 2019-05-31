/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gatt_demo_db_utils.h"

CsrUint8 csrBtRscAppBuildMeasurementValue(CsrUint8 flags, CsrUint16 speed, CsrUint8 cadance, CsrUint16 stride, CsrUint32 distance, CsrUint8 **returnValue)
{
    CsrUint8 *value = *returnValue;
    CsrUint8 size = 1+2+1; /* flags+speed+cadance */
    CsrUint8 offset = size;

    if(flags & 0x01) /* stride should be set in data */
    {
        size += 2;  /*16bit*/
    }
    if(flags & 0x02) /* distance should be set in data */
    {
        size += 4; /*32bit*/
    }
    if(value != NULL) /*we need to clear the return pointer*/
    {
        CsrPmemFree(value);
    }
    value = CsrPmemZalloc(size);
    value[0] = flags;
    value[1] = (CsrUint8)(speed & 0x00FF);
    value[2] = (CsrUint8)((speed & 0xFF00) >> 8);
    value[3] = cadance;
    if(flags & 0x01) /* stride should be set in data */
    {
        value[offset++] = (CsrUint8)(stride & 0x00FF);
        value[offset++] = (CsrUint8)((stride & 0xFF00) >> 8);
    }
    if(flags & 0x02) /* distance should be set in data */
    {
        value[offset++] = (CsrUint8)((distance & 0x000000FF));
        value[offset++] = (CsrUint8)((distance & 0x0000FF00) >> 8);
        value[offset++] = (CsrUint8)((distance & 0x00FF0000) >> 16);
        value[offset++] = (CsrUint8)((distance & 0xFF000000) >> 24);
    }
    *returnValue = value;
    return size;
}

void CsrBtRscAppGetMeasurementValues(CsrUint8 *inputData, CsrBool *running,  CsrBool *strideAvail, CsrBool *distanceAvail, CsrUint16 *speed, CsrUint8 *cadance, CsrUint16 *stride, CsrUint32 *distance)
{
    CsrUint8 offset = 4;
    *strideAvail = (CsrBool)(inputData[0] & 0x01);
    *distanceAvail = (CsrBool)(inputData[0] & 0x02);
    *running = (CsrBool)(inputData[0] & 0x04); /*0 = walking, 1 = running */
    *speed   = (CsrUint16)((inputData[1]))|inputData[2]<<8;
    *cadance = inputData[3];

    if(*strideAvail)
    {
        *stride = (CsrUint16)((inputData[offset])|(inputData[offset+1]<<8));
        offset += 2;            
    }
    if(*distanceAvail)
    {
        *distance = (CsrUint32)((inputData[offset])|(inputData[offset+1]<<8)|(inputData[offset+2]<16)|(inputData[offset+3]<<24));
    }
}

