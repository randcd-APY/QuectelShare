/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_pmem.h"
#include "csr_dspm_lower.h"
#include "csr_bt_dsp_patching.h"
#include "csr_bt_dsp_patching_generic.h"

CsrUint16 CsrBtDspNumCapabilities = 0;

void CsrBtDspPatchingInit(void)
{
    CsrDspmConfigAcquireFunctionRegister(configPatchAcquire);
    CsrDspmOperatorPatchAcquireFunctionRegister(operatorPatchAcquire);
}

CsrResult CsrBtReadUint16(void *handle, CsrUint32 offset, CsrUint16 *data)
{
    CsrResult res;
    CsrUint8 temp[2];

    res = CsrBtRead(handle, offset, 2, temp);
    if (res == CSR_RESULT_SUCCESS)
    {
        *data = CSR_GET_UINT16_FROM_PTR(temp);
    }
    return res;
}

CsrResult CsrBtReadUint32(void *handle, CsrUint32 offset, CsrUint32 *data)
{
    CsrResult res;
    CsrUint8 temp[4];
    
    res = CsrBtRead(handle, offset, 4, temp);
    if (res == CSR_RESULT_SUCCESS)
    {
        *data = CSR_GET_UINT32_FROM_PTR(temp);
    }
    return res;
}


CsrResult CsrBtValidatPatch(void *handle)
{

    if (CsrBtDspNumCapabilities == 0)
    {
        CsrUint16 fileVersion, stamp;
        CsrUint32 romVersion, configVersion, offset = 0;
        CsrUint8 data[CSR_BT_DSP_CONFIGURATION_FILE_STRUCT_SIZE];

        if (CsrBtRead(handle, offset, CSR_BT_DSP_CONFIGURATION_FILE_STRUCT_SIZE, data) != CSR_RESULT_SUCCESS)
        {
            return CSR_RESULT_FAILURE;
        }
        else
        {
            stamp = CSR_GET_UINT16_FROM_PTR(data + 0);
            /* Check the stamp says this as a configuration file */
            if (stamp != CSR_BT_DSP_CONFIG_STAMP)
            {
                return CSR_RESULT_FAILURE;
            }

            fileVersion = CSR_GET_UINT16_FROM_PTR(data + 2);
            romVersion = CSR_GET_UINT32_FROM_PTR(data + 4);
            configVersion = CSR_GET_UINT32_FROM_PTR(data + 8);
            CsrBtDspNumCapabilities = CSR_GET_UINT16_FROM_PTR(data + 12);
        }
    }
    return CSR_RESULT_SUCCESS;
    
}

CsrUint8 *CsrBtParsePatch(void *handle, CsrUint16 capability,
                          CsrUint16  itemIndex,CsrUint16 *itemCount,
                          CsrUint16 *skipCount,CsrUint16 *itemLength)
{
    CsrUint16 index, count;
    CsrUint16 capabilityId;
    CsrUint16 numSkippable, offsetSkippable, numNonSkippable, OffsetNonSkippable;
    CsrUint8 *patch = NULL;
    CsrUint16 patchLength;
    CsrUint32 offset;

    *itemCount = *skipCount = *itemLength = 0;
    offset = CSR_BT_DSP_CONFIGURATION_FILE_STRUCT_SIZE;
    
    /* Loop and find the capapbility requested by DSP Manager */
    for (count = 0; count < CsrBtDspNumCapabilities; count++)
    {
        if (CsrBtReadUint16(handle, offset, (CsrUint16 *)&capabilityId) != CSR_RESULT_SUCCESS)
        {
            return NULL;
        }
        offset += 2;
        /* Check the capabilityID with the recieved one */
        if (capabilityId != capability)
        {   /* Looks like capabilityID not matched */
            /* Skip the next 8 bytes to get to the next capability ID */
            offset += CSR_BT_DSP_CAPABILITY_DESC_STRUCTURE_SIZE;
        }
        else
        {
            /* Looks like the capability matches */
            CsrUint8 data[CSR_BT_DSP_CAPABILITY_DESC_STRUCTURE_SIZE];

            if (CsrBtRead(handle, offset, CSR_BT_DSP_CAPABILITY_DESC_STRUCTURE_SIZE, data) != CSR_RESULT_SUCCESS)
            {
                return NULL;
            }
            else
            {
                numSkippable = CSR_GET_UINT16_FROM_PTR(data + 0);
                offsetSkippable = CSR_GET_UINT16_FROM_PTR(data + 2);
                numNonSkippable = CSR_GET_UINT16_FROM_PTR(data + 4);
                OffsetNonSkippable = CSR_GET_UINT16_FROM_PTR(data + 6);
                offset += CSR_BT_DSP_CAPABILITY_DESC_STRUCTURE_SIZE;
                break;
            }
        }
    }
    
    if (count == CsrBtDspNumCapabilities)
    {/*DSPM requested capability not found */
        CSR_BT_DSP_PATCHING_LOG
        ("CapabilityID recieved not found (%d)",capability);
        return NULL;
    }

    /* Update itemCount and skipCount, itemLength would be updated later */
    *itemCount = numSkippable + numNonSkippable;
    *skipCount = numSkippable;

    /* For capabilityId other than '0', the itemIndex '0' meaning differs with 
       the capabilityId '0' with itemIndex '0' see csr_dspm_lower.h */
    /* The below code under condition (capabilityId != 0) is to make unique 
       functionality for Framework patching and operator patching */
    if (capabilityId != 0)
    {
        if (itemIndex == 0)
        {
            /* This means the DSPM is trying to retrieve the item count and skip 
               count, The values already updated */
            return NULL;
        }
        else
        {
            itemIndex--;
        }
    }
    
    /* Check whether we recieved the valid itemIndex or not, recieved itemIndex
       should be less than the total no of skippable & non skippable patches */
    if (itemIndex >= (numSkippable + numNonSkippable))
    { /* DSPM requested the invalid itemindex */
        CSR_BT_DSP_PATCHING_LOG
        ("ItemIndex recieved is out of range (%d)",itemIndex);
        return NULL;
    }
    
    /* Finally, we found the capability requested by DSP manager, let pull 
       patch from .pat file. before that Set the offset based on requested 
       itemIndex (skippable or nonSkippable) */
    if (itemIndex < numSkippable)
    {
        offset = offsetSkippable;
        index = 0;
    }
    else
    {
        offset = OffsetNonSkippable;
        index = numSkippable;
    }


    /* Set the offset to retrieve the patch requested */
    for (; index < itemIndex; index++)
    {
        if (CsrBtReadUint16(handle, offset, (CsrUint16 *)&patchLength) != CSR_RESULT_SUCCESS)
        {
            return NULL;
        }
        offset += (patchLength + 2);
    }
    
    /* Found the requested patch, extract the length and patch from .PAT file */
    if (itemIndex == index)
    {
        CsrUint8 temp;
        
        if (CsrBtReadUint16(handle, offset, (CsrUint16 *)&patchLength) != CSR_RESULT_SUCCESS)
        {
            return NULL;
        }
        offset += 2;

        /* Allocate the memory of size of the patch plus 2 bytes for 
           patchLength/patchCount. For 'capability 0' the first 2 bytes shall 
           contain the patchCount. For 'capability X' (non-zero) the first 2 bytes 
           shall contain the patchLength */
        patch = CsrPmemZalloc(patchLength + 2);

        if (CsrBtRead(handle, offset, patchLength, patch + 2) != CSR_RESULT_SUCCESS)
        {
            CsrPmemFree(patch);
            return NULL;
        }

        /* Convert patch in to little endian format */
        for (index = 0; index < (patchLength + 2); index+=2)
        {
            temp = patch[index];
            patch[index] = patch[index + 1];
            patch[index + 1] = temp;
        }
        *itemLength = (CsrUint16)patchLength + (CsrUint16)sizeof(CsrUint16);
    }
    return patch;
}



