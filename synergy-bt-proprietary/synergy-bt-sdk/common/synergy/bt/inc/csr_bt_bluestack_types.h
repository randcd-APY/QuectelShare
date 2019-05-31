#ifndef CSR_BT_BLUESTACK_TYPES_H__
#define CSR_BT_BLUESTACK_TYPES_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif


/* BlueStack support types */
#define PARAM_UNUSED CSR_UNUSED
typedef CsrSchedQid phandle_t;
typedef CsrTime TIME;

#define PHANDLE_TO_QUEUEID(phdl) (phdl)
#define QUEUEID_TO_PHANDLE(qid)  (qid)

/* Compile time assert
 *
 * Evaluate an expression at compile time and force the compilation to
 * abort if the expression is false. expr is the expression to
 * evaluate. msg is a symbol name to try to get into the error message
 * (works only on some compilers).
 */
#define CSR_BT_COMPILE_TIME_ASSERT(expr, msg) struct compile_time_assert_ ## msg { \
    int compile_time_assert_ ## msg [1 - (!(expr))*2]; \
}

#ifdef __cplusplus
}
#endif

#endif
