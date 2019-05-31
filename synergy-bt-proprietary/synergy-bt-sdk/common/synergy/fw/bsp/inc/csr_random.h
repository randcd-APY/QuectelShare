#ifndef CSR_RANDOM_H__
#define CSR_RANDOM_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************

    NAME
        CsrRandomSeed

    DESCRIPTION
        Return a seeded state for use as argument to CsrRandom. The seed may be
        based on the current time, so care must be taken to place this call at a
        strategic location that is likely to be triggered at a semi-random time.

    RETURNS
        A seeded state for use as argument to CsrRandom. May be NULL depending
        on the implementation. Must be freed by CsrPmemFree after use.

*******************************************************************************/
void *CsrRandomSeed(void);

/*******************************************************************************

    NAME
        CsrRandom

    DESCRIPTION
        Return a random number (uniform distribution). Before calling this
        function a seeded state must be obtained by calling CsrRandomSeed. The
        return value from CsrRandomSeed is passed as argument to CsrRandom any
        number of times to receive a sequence of random numbers. When no more
        random numbers are needed, the state is freed by a call to CsrPmemFree.

    PARAMETERS
        randomState - The seeded state returned by CsrRandomSeed.

    RETURNS
        A random number (uniform distribution)

*******************************************************************************/
CsrUint32 CsrRandom(void *randomState);

#ifdef __cplusplus
}
#endif

#endif
