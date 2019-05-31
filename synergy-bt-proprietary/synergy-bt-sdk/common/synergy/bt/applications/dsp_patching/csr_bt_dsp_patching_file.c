/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_file.h"
#include "csr_bt_dsp_patching_generic.h"


#define MACRO_EXP(X) MACRO_EXP_Q(X)
#define MACRO_EXP_Q(X)          #X
#define _PATPATH_ MACRO_EXP(CSR_BT_TOPDIR)"/applications/dsp_patching/patch_files/"

typedef struct
{
    CsrUint16      buildId;
    CsrCharString *patchFileName;
} csrBtDspPatchBundles;


static const csrBtDspPatchBundles csrBtDspPatchTable[] =
{
    /*Gemini A08:CSR8811,CSR8816,CSR8311*/
    { 8241, _PATPATH_"PB-109_GeminiA08_v8.pat"},
    /* Dale Auto A07: CSR8350 */    
    { 9079, _PATPATH_"PB-132_B-128945_DaleAutoA07_V4.pat"},
};

CsrResult CsrBtRead(void *handle, CsrUint32 offset, 
                    CsrUint16 lengthToRead, CsrUint8 *data)
{
    CsrResult res;
    CsrSize bytesRead;

    res = CsrFileSeek((CsrFileHandle*)handle, offset, CSR_SEEK_SET);
    if(res != CSR_RESULT_SUCCESS)
    {
        CSR_BT_DSP_PATCHING_LOG("Failed to seek the file of offset '%x'",offset);
        CSR_BT_DSP_PATCHING_LOG("File seek returns error code '%x'", res);
        return CSR_RESULT_FAILURE;
    }
    
    res = CsrFileRead(data,lengthToRead,(CsrFileHandle*)handle,&bytesRead);
    if (bytesRead != lengthToRead)
    {
        CSR_BT_DSP_PATCHING_LOG("Failed to read (%d) bytes in CsrBtRead()",lengthToRead);
        CSR_BT_DSP_PATCHING_LOG("File read returns error code '%x'", res);
        return CSR_RESULT_FAILURE;
    }
    return res;
}

static CsrUint8 *CsrBtParsePatchfile(CsrCharString *patchFileName,
                                     CsrUint16 capability,
                                     CsrUint16  patchIndex,
                                     CsrUint16 *patchCount,
                                     CsrUint16 *skippableCount,
                                     CsrUint16 *patchLength)
{
    CsrFileHandle *fileHandle;
    CsrUint8      *patch;
    CsrResult      res;

    res = CsrFileOpen(&fileHandle, 
                      (const CsrUtf8String *)patchFileName,
                      CSR_FILE_OPEN_FLAGS_READ_ONLY,
                      CSR_FILE_PERMS_USER_READ);
    if(res != CSR_RESULT_SUCCESS)
    { /* Failed to open the file */
        CSR_BT_DSP_PATCHING_LOG
        ("Failed to open the file (%s)",patchFileName);
        return NULL;
    }
    res = CsrBtValidatPatch(fileHandle);
    if (res != CSR_RESULT_SUCCESS)
    {/* Patch validation Failed */
        CSR_BT_DSP_PATCHING_LOG
        ("Failed to validate the pat file (%s)",patchFileName);
        CsrFileClose(fileHandle);
        return NULL;
    }

    patch = CsrBtParsePatch(fileHandle, capability, patchIndex, 
                            patchCount, skippableCount, patchLength);
                            
    CsrFileClose(fileHandle);

    return patch;

}

CsrUint8 *configPatchAcquire(CsrUint16  buildId, 
                             CsrUint16  configBlockIndex,
                             CsrUint16 *configBlockCount, 
                             CsrUint16 *configBlockLength)
{
    CsrCharString *patchFileName;
    CsrUint8      *patch;
    CsrUint16      patchCount, skippableCount, patchLength, index;
    
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
            patchFileName = csrBtDspPatchTable[index].patchFileName;
            patch = CsrBtParsePatchfile(patchFileName, CSR_BT_DSP_CAPABILITY_ID_0, 
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
    CsrCharString *patchFileName;
    CsrUint8      *patch;
    CsrUint16      patchCount, skippableCount, patchLength, index;

    patchCount = skippableCount = patchLength = 0;

    for(index=0; index<CSR_ARRAY_SIZE(csrBtDspPatchTable); index++)
    {
        if(csrBtDspPatchTable[index].buildId == buildId)
        {
            patchFileName = csrBtDspPatchTable[index].patchFileName;
            patch = CsrBtParsePatchfile(patchFileName, capability, itemIndex, 
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

