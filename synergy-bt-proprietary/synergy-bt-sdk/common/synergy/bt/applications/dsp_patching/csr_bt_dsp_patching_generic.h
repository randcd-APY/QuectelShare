/******************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_result.h"
#include "csr_pmem.h"
#include "csr_log_text.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Value to indicate the configuration file*/
#define CSR_BT_DSP_CONFIG_STAMP 0x8EAD

/* DSP capability ID value 0 */
#define CSR_BT_DSP_CAPABILITY_ID_0 0

/* Configuration data structure header size except capability desc and datagram*/
#define CSR_BT_DSP_CONFIGURATION_FILE_STRUCT_SIZE 14

/* Capability desc structure size excluding capability ID size */
#define CSR_BT_DSP_CAPABILITY_DESC_STRUCTURE_SIZE 8

#ifdef CSR_LOG_ENABLE
#define CSR_BT_DSP_PATCHING_LOG(a,b) CSR_LOG_TEXT_WARNING((CsrSchedTaskQueueGet(), 0, (a), (b)))
#else
#define CSR_BT_DSP_PATCHING_LOG(a,b)
#endif

#define CSR_GET_UINT16_FROM_PTR(ptr) (((CsrUint16) ((CsrUint8 *) (ptr))[1]) | \
                                      ((CsrUint16) ((CsrUint8 *) (ptr))[0]) << 8)

#define CSR_GET_UINT32_FROM_PTR(ptr) (((CsrUint32) ((CsrUint8 *)(ptr))[3]) | \
                                      ((CsrUint32) ((CsrUint8 *)(ptr))[2]) << 8 | \
                                      ((CsrUint32) ((CsrUint8 *)(ptr))[1]) << 16 | \
                                      ((CsrUint32) ((CsrUint8 *)(ptr))[0]) << 24)
                                      
extern CsrUint8 *operatorPatchAcquire(CsrUint16  buildId,
                                      CsrUint16  capability,
                                      CsrUint16  itemIndex,
                                      CsrUint16 *itemCount,
                                      CsrUint16 *skipCount,
                                      CsrUint16 *itemLength);

extern CsrUint8 *configPatchAcquire(CsrUint16  buildId, 
                                    CsrUint16  configBlockIndex,
                                    CsrUint16 *configBlockCount, 
                                    CsrUint16 *configBlockLength);

extern CsrResult CsrBtRead(void      *handle, 
                           CsrUint32  offset, 
                           CsrUint16  lengthToRead, 
                           CsrUint8  *data);

CsrUint8 *CsrBtParsePatch(void      *handle,
                          CsrUint16  capability,
                          CsrUint16  itemIndex,
                          CsrUint16 *itemCount,
                          CsrUint16 *skipCount,
                          CsrUint16 *itemLength);
                          
CsrResult CsrBtValidatPatch(void *handle);

#ifdef __cplusplus
}
#endif

