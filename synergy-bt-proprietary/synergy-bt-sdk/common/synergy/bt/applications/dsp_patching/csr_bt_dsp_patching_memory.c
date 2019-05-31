/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_dsp_patching_generic.h"

/* Extern the patch arrays and its length implemented in "/patch_files/*.c"*/
extern const CsrUint8 PB_109_GeminiA08_V8_pat[];
extern const CsrUint32 PB_109_GeminiA08_V8_pat_length[];

extern const CsrUint8 PB_132_B_128945_DaleAutoA07_V4_pat[];
extern const CsrUint32 PB_132_B_128945_DaleAutoA07_V4_pat_length[];

typedef struct
{
    CsrUint16 buildId;
    const CsrUint8 *patch;
    const CsrUint32 *patchLength;
} csrBtDspPatchBundles;

CsrUint32 CsrBtDspPatchArrayLength = 0;

static const csrBtDspPatchBundles csrBtDspPatchTable[] =
{
    /* Gemini A08: CSR8811, CSR8816, CSR8311*/
    { 8241, PB_109_GeminiA08_V8_pat, PB_109_GeminiA08_V8_pat_length},
    /* Dale Auto A07: CSR8350 */
    { 9079, PB_132_B_128945_DaleAutoA07_V4_pat, PB_132_B_128945_DaleAutoA07_V4_pat_length},
};

CsrResult CsrBtRead(void *handle, CsrUint32 offset, 
                    CsrUint16 lengthToRead, CsrUint8 *data)
{
    CsrUint8 *patchArray;

    if ((offset + lengthToRead) > CsrBtDspPatchArrayLength)
    {
        CSR_BT_DSP_PATCHING_LOG
        (
        "Segmentation fault: array index overflow (%d)", 
        (offset + lengthToRead)
        );
        return CSR_RESULT_FAILURE;
    }

    patchArray = (CsrUint8 *) handle;
    
    patchArray += offset;

    CsrMemCpy(data, patchArray, lengthToRead);

    return CSR_RESULT_SUCCESS;
}


static CsrUint8 *CsrBtParsePatchArray(CsrUint8  *patchArray, 
                                      CsrUint16  capability,
                                      CsrUint16  patchIndex, 
                                      CsrUint16 *patchCount,
                                      CsrUint16 *skippableCount, 
                                      CsrUint16 *patchLength)
{
    if (CsrBtValidatPatch(patchArray) != CSR_RESULT_SUCCESS)
    {/* Patch validation Failed */
        CSR_BT_DSP_PATCHING_LOG("Failed to validate the patch", 0);
        return NULL;
    }

    return CsrBtParsePatch(patchArray, capability, patchIndex, 
                            patchCount, skippableCount, patchLength);
}

CsrUint8 *configPatchAcquire(CsrUint16  buildId, 
                             CsrUint16  configBlockIndex,
                             CsrUint16 *configBlockCount, 
                             CsrUint16 *configBlockLength)
{
    CsrUint8  *patchArray;
    CsrUint8  *patch;
    CsrUint16  patchCount, skippableCount, patchLength, index;

    patchCount = skippableCount = patchLength = 0;
    *configBlockCount = *configBlockLength = 0;
#ifdef CSR_LOG_ENABLE
    if (configBlockIndex == 0)
    {
        CsrLogTextRegister(CsrSchedTaskQueueGet(), "DSP_PATCHING", 0, NULL);
    }
#endif
    for(index = 0; index < CSR_ARRAY_SIZE(csrBtDspPatchTable); index++)
    {
        if(csrBtDspPatchTable[index].buildId == buildId)
        {
            patchArray = (CsrUint8 *)csrBtDspPatchTable[index].patch;
            CsrBtDspPatchArrayLength = *(csrBtDspPatchTable[index].patchLength);
            patch = CsrBtParsePatchArray(patchArray, CSR_BT_DSP_CAPABILITY_ID_0, 
                                         configBlockIndex, &patchCount, 
                                         &skippableCount, &patchLength);
            if (patch != NULL)
            { /* Append configBlockCount/patchCount to the patch using little 
                 endian encoding*/
                CSR_COPY_UINT16_TO_LITTLE_ENDIAN(patchCount, patch);
                *configBlockCount = patchCount;
                *configBlockLength = patchLength;
            }
            return patch;
        }
    }
    return NULL;
}

CsrUint8 *operatorPatchAcquire(CsrUint16  buildId,CsrUint16 capability,
                               CsrUint16  itemIndex,CsrUint16 *itemCount,
                               CsrUint16 *skipCount,CsrUint16 *itemLength)
{
    CsrUint8  *patchArray;
    CsrUint8  *patch;
    CsrUint16  patchCount, skippableCount, patchLength, index;


    patchCount = skippableCount = patchLength = 0;

    for(index = 0; index < CSR_ARRAY_SIZE(csrBtDspPatchTable); index++)
    {
        if(csrBtDspPatchTable[index].buildId == buildId)
        {
            patchArray = (CsrUint8 *)csrBtDspPatchTable[index].patch;
            CsrBtDspPatchArrayLength = *(csrBtDspPatchTable[index].patchLength);
            patch = CsrBtParsePatchArray(patchArray, capability, itemIndex, 
                                    &patchCount, &skippableCount, &patchLength);

            if (itemIndex == 0)
            {
              /* This means the DSPM is trying to retrieve the item count and 
                 skip count */
                *itemCount = patchCount;
                *skipCount = skippableCount;
            }
            else
            {   /* Append the length of the patch, excluding the length field 
                   using little endian encoding */
                if (patch != NULL)
                {
                    CSR_COPY_UINT16_TO_LITTLE_ENDIAN((patchLength - 2), patch);
                }
                *itemLength = patchLength;
            }
            return patch;
        }
    }
    return NULL;
}

