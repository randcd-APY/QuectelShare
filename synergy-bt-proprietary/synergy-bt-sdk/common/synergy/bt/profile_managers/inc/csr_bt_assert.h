#ifndef CSR_BT_CSR_BT_ASSERT_H__
#define CSR_BT_CSR_BT_ASSERT_H__

/****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_panic.h"
#include "csr_pmem.h"
#include "csr_bt_tasks.h"
#include "csr_bt_profiles.h"
#include "csr_bt_util.h"
#include "csr_exceptionhandler.h"

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXCLUDE_CSR_EXCEPTION_HANDLER_MODULE

#define CSR_BT_GENERAL_WARNING(task, prim, type, str)                 \
    do {                                                              \
        CsrGeneralWarning(task, (prim), (CsrUint16) (type), (str)); \
    } while(0)

#define CSR_BT_STATE_EVENT_EXCEPTION(task, prim, type, state)          \
    do {                                                             \
        CsrStateEventException(task, (prim), (CsrUint16) (type), (CsrUint16) (state)); \
    } while(0)

#define CSR_BT_GENERAL_EXCEPTION(task, prim, type, str)                 \
    do {                                                              \
        CsrGeneralException(task, (prim), (CsrUint16) (type), (str)); \
    } while(0)

#define CSR_BT_STATE_EVENT_EXCEPTION_ON(task, cond, prim, type, state)  \
    do {                                                              \
        if ((cond))                                                   \
            CsrStateEventException(task, (prim), (type), (state));   \
    } while(0)

#define CSR_BT_GENERAL_EXCEPTION_ON(task, cond, prim, type, str)        \
    do {                                                              \
        if ((cond))                                                   \
            CsrStateEventException(task, (prim), (type), (str));     \
    } while(0)

#else
#define CSR_BT_GENERAL_WARNING(a, b, c, d)
#define CSR_BT_STATE_EVENT_EXCEPTION(a, b, c, d)
#define CSR_BT_GENERAL_EXCEPTION(a, b, c, d)
#define CSR_BT_STATE_EVENT_EXCEPTION_ON(a, b, c, d, e)
#define CSR_BT_GENERAL_EXCEPTION_ON(a, b, c, d, e)
#endif

#define CSR_BT_ASSERT_RET_VAL(cond, retval)                           \
    do {                                                            \
        if (!(cond))                                                \
        {                                                           \
            CSR_BT_GENERAL_EXCEPTION(NULL, 0, 0, #cond);              \
            return (retval);                                        \
        }                                                           \
    } while(0)

#define CSR_BT_ASSERT_RET(cond)                                       \
    do {                                                            \
        if (!(cond))                                                \
        {                                                           \
            CSR_BT_GENERAL_EXCEPTION(NULL, 0, 0, #cond);              \
            return;                                                 \
        }                                                           \
    } while(0)

#define CSR_BT_RET_VAL_IF_FAIL(cond, retval)                          \
    do {                                                            \
        if (!(cond))                                                \
        {                                                           \
            return (retval);                                        \
        }                                                           \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif /* ndef _CM_MAIN_H */
