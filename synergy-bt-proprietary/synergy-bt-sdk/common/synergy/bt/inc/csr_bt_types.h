#ifndef CSR_BT_TYPES_H__
#define CSR_BT_TYPES_H__
/****************************************************************************

Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
    extern "C" {
#endif


/* Set to the most optimal type on the given platform that is 
   at least 8, 16 and 32 bit wide.
   e.g. using 32 bit types on a 32 bit processor will decrese 
   the code-size and increase the execution speed. */
#ifndef CSR_HAVE_FAST_INTEGERS

#define CsrUintFast8 CsrUint32 
#define CsrUintFast16 CsrUint32 
#define CsrUintFast32 CsrUint32 
#define CsrIntFast8 CsrInt32 
#define CsrIntFast16 CsrInt32 
#define CsrIntFast32 CsrInt32 

#endif

#ifdef __cplusplus
}
#endif


#endif

