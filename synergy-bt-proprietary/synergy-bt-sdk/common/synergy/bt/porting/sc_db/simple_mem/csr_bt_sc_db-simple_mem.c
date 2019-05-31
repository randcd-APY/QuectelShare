/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_sc_db.h"
#include "csr_bt_util.h"
#include "csr_bt_common.h"

static CsrBtScDbDeviceRecord deviceVariable;
static CsrBtScDbDeviceRecord deviceVariable1;

CsrBool CsrBtScDbRead(CsrBtDeviceAddr *theAddr, CsrBtScDbDeviceRecord *theRecord)
{ /* Should return TRUE and the information about the requested device, if the device is not present in the data base return FALSE */
    if(bd_addr_eq(theAddr, &deviceVariable.deviceAddr) )
    {
        *theRecord = deviceVariable;
        return(TRUE);
    }
    else
    {
        deviceVariable1.linkkeyValid = FALSE;
        *theRecord = deviceVariable1;
        return(FALSE);
    }
}

void CsrBtScDbWrite(CsrBtDeviceAddr *theAddr, CsrBtScDbDeviceRecord *theRecord)
{ /* Should write information about into the data base, if the device already exist, the present entry should be updated */
    CSR_UNUSED(theAddr);
    deviceVariable = *theRecord;
}

void CsrBtScDbRemove(CsrBtDeviceAddr *theAddr)
{ /* SHould remove the device from the data base */
    CSR_UNUSED(theAddr);
    bd_addr_zero( &(deviceVariable.deviceAddr) );
    deviceVariable.linkkeyValid = FALSE;
}

CsrBool CsrBtScDbReadFirst(CsrBtScDbDeviceRecord *theRecord)
{ /* Should return TRUE and the first valid device if it exist, is no device exist it return FALSE */
    CSR_UNUSED(theRecord);
    return(FALSE);
}

CsrBool CsrBtScDbReadNext(CsrBtScDbDeviceRecord *theRecord)
{ /* Should return TRUE and the next valid device in line if it exist, is no device exist it return FALSE */
    CSR_UNUSED(theRecord);
    return(FALSE);
}
