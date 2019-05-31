/****************************************************************************

Copyright (c) 2009-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include "csr_bt_declare_tasks.h"

/* CSR scheduler task initialisation */
void CsrSchedTaskInit(void *data)
{
    /* Initialise standard tasks (see csr_bt_declare_tasks.c) */
    CsrBtAppTaskInitFw(data);
    CsrBtAppTaskInitBt(data);

    /* Synergy BT high-level protocols */
    CsrSchedRegisterTask(&CSR_BT_PAC_IFACEQUEUE, CSR_BT_PAC_INIT, CSR_BT_PAC_DEINIT, CSR_BT_PAC_HANDLER, "CSR_BT_PAC", data, ID_STACK);

    /* [QTI] Unify synergy scheduler into ID_STACK. */
#if !defined(EXCLUDE_TEST_MODULE) && (CSR_BT_RUN_TASK_TEST_TASK == 1)
    /* Application task and helpers */
    CsrSchedRegisterTask(&TESTQUEUE, CSR_BT_TEST_INIT, CSR_BT_TEST_DEINIT, CSR_BT_TEST_HANDLER, "CSR_BT_APP", data, ID_STACK);
#endif
}
