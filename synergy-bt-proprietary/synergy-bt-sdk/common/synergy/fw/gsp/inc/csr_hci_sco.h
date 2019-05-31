#ifndef CSR_HCI_SCO_H__
#define CSR_HCI_SCO_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"


#ifdef __cplusplus
extern "C" {
#endif

#define NO_SCO_HANDLE   (255)

typedef void (*CsrHciScoHandlerFuncType)(CsrUint8 *);

typedef struct CsrHciScoHandleStructure
{
    CsrUint16                        scoHandle;
    CsrHciScoHandlerFuncType         scoHandlerFunc;
    struct CsrHciScoHandleStructure *next;
} CsrHciScoHandleStructure;

CsrBool CsrHciRegisterScoHandle(CsrUint16 theScoHandle,
    CsrHciScoHandlerFuncType theFunctionPtr);
void CsrHciSendScoData(CsrUint8 *theData);
void CsrHciDeRegisterScoHandle(CsrUint16 theScoHandle);
void CsrHciInitScoHandle(void);
CsrBool CsrHciLookForScoHandle(const CsrUint8 *theBuf);

#ifdef __cplusplus
}
#endif

#endif
