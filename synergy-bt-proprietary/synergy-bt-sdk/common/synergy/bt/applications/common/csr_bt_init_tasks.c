/****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_bt_declare_tasks.h"
#include <stdio.h>

/* Transport handler function. Set by the cmdline parser depending on
 * what transport the user selects */
schedEntryFunction_t CSR_TM_BLUECORE_TRANSPORT_INIT = NULL;

#ifdef CSR_HYDRA_SSD
CsrSchedQid CSR_SSD_IFACEQUEUE;
#endif

/* Initialise standard, mandatory Bluetooth tasks */
void CsrBtAppTaskInitBt(void *data)
{
#ifdef CSR_LOG_ENABLE
    static CsrBool logged;
    if (!logged)
    {
        CsrCharString text[100];

        snprintf(text, sizeof(text), "Synergy BT version - " CSR_BT_RELEASE_VERSION
                " Build number - '%x'"
#ifdef CSR_BT_RELEASE_TYPE_ENG
                " Engineering release"
#endif
#ifdef CSR_BT_RELEASE_TYPE_TEST
                " Test candidate"
#endif
                ,
                CSR_BT_VERSION_BUILD);

        CSR_LOG_TEXT(text);
        logged = TRUE;
    }
#endif

    CsrSchedRegisterTask(&DM_IFACEQUEUE, DM_INIT, DM_DEINIT, DM_TASK, "CSR_BT_DM", data, ID_STACK);
    CsrSchedRegisterTask(&DM_HCI_IFACEQUEUE, DM_HCI_INIT, DM_HCI_DEINIT, DM_HCI_TASK, "CSR_BT_DM_HCI", data, ID_STACK);
    CsrSchedRegisterTask(&L2CAP_IFACEQUEUE, L2CAP_INIT, L2CAP_DEINIT, L2CAP_TASK, "CSR_BT_L2CAP", data, ID_STACK);
    CsrSchedRegisterTask(&RFCOMM_IFACEQUEUE, RFCOMM_INIT, RFCOMM_DEINIT, RFCOMM_TASK, "CSR_BT_RFCOMM", data, ID_STACK);
    CsrSchedRegisterTask(&SDP_L2CAP_IFACEQUEUE, SDP_L2CAP_INIT, SDP_L2CAP_DEINIT, SDP_L2CAP_TASK, "CSR_BT_SDP_L2CAP", data, ID_STACK);
    CsrSchedRegisterTask(&SDP_IFACEQUEUE, SDP_INIT, SDP_DEINIT, SDP_TASK, "CSR_BT_SDP", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_CM_IFACEQUEUE, CSR_BT_CM_INIT, CSR_BT_CM_DEINIT, CSR_BT_CM_HANDLER, "CSR_BT_CM", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_SC_IFACEQUEUE, CSR_BT_SC_INIT, CSR_BT_SC_DEINIT, CSR_BT_SC_HANDLER, "CSR_BT_SC", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_SD_IFACEQUEUE, CSR_BT_SD_INIT, CSR_BT_SD_DEINIT, CSR_BT_SD_HANDLER, "CSR_BT_SD", data, ID_STACK);

#ifdef CSR_AMP_ENABLE
    CsrSchedRegisterTask(&CSR_BT_AMPM_IFACEQUEUE, CSR_BT_AMPM_INIT, CSR_BT_AMPM_DEINIT, CSR_BT_AMPM_HANDLER, "CSR_BT_AMPM", data, ID_STACK);
#endif /* CSR_AMP_ENABLE */
}

/* Initialise standard (mandatory) Framework tasks */
void CsrBtAppTaskInitFw(void *data)
{
#ifdef CSR_HYDRA_SSD
    CsrSchedRegisterTask(&CSR_SSD_IFACEQUEUE, CSR_SSD_INIT, CSR_SSD_DEINIT, CSR_SSD_HANDLER,"CSR_SSD", data, ID_STACK);
#endif

    CsrSchedRegisterTask(&CSR_BCCMD_IFACEQUEUE, CSR_BCCMD_INIT, CSR_BCCMD_DEINIT, CSR_BCCMD_HANDLER, "CSR_BCCMD", data, ID_STACK);
#ifdef CSR_USE_QCA_CHIP
    CsrSchedRegisterTask(&CSR_QVSC_IFACEQUEUE, CSR_QVSC_INIT, CSR_QVSC_DEINIT, CSR_QVSC_HANDLER, "CSR_QVSC", data, ID_STACK);
#endif
    CsrSchedRegisterTask(&CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_TRANSPORT_INIT, CSR_TM_BLUECORE_DEINIT, CSR_TM_BLUECORE_HANDLER, "CSR_TM_BLUECORE", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_HQ_IFACEQUEUE, CSR_HQ_INIT, CSR_HQ_DEINIT, CSR_HQ_HANDLER, "CSR_HQ", data, ID_STACK);
#ifdef CSR_USE_BLUECORE_CHIP
    CsrSchedRegisterTask(&CSR_AM_IFACEQUEUE, CSR_AM_INIT, CSR_AM_DEINIT, CSR_AM_HANDLER, "CSR_AM", data, ID_STACK);
#endif
    CsrSchedRegisterTask(&CSR_HCI_IFACEQUEUE, CSR_HCI_INIT, CSR_HCI_DEINIT, CSR_HCI_HANDLER, "CSR_HCI", data, ID_STACK);
}
