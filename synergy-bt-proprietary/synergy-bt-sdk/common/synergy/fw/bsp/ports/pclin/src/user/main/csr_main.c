/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"
#include "csr_main.h"
#include "csr_arg_search.h"

int main(int argc, char *argv[])
{
    CsrArgSearchInit((CsrUint32) argc, (CsrCharString **) argv);
    return (int) CsrMain();
}
