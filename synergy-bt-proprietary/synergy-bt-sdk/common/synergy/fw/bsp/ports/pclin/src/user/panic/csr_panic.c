/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#include <stdlib.h>

#include "csr_synergy.h"
#include "csr_panic.h"
#include "platform/csr_panic_init.h"

static CsrPanicHandler panic = NULL;

void CsrPanicInit(CsrPanicHandler cb)
{
    panic = cb;
}

void CsrPanic(CsrUint8 tech, CsrUint16 reason, const char *p)
{
    if (panic)
    {
        panic(tech, reason, p);
    }
    exit(1);
}
