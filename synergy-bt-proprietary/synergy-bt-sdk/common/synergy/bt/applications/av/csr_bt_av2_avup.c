/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_app_lib.h"
#include "csr_bt_av_app_util.h"
#include "csr_bt_av_app_ui_sef.h"

/**************************************************************************************************
 * handleAvPrim
 **************************************************************************************************/
void handleAvPrim(av2instance_t *instData, void *msg)
{
    CsrBtAvPrim    *primType;

    primType = (CsrBtAvPrim *) msg;
    switch (*primType)
    {
        case CSR_BT_AV_DEACTIVATE_CFM:
            {
                CsrBtAvDeactivateCfm *prim;
                prim = (CsrBtAvDeactivateCfm*)msg;
                handleAvDeactivateCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_REGISTER_STREAM_HANDLE_CFM:
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Application stream handle has been registered\n");
                break;
            }

        case CSR_BT_AV_ACTIVATE_CFM:
            {
                CsrBtAvActivateCfm *prim;
                prim = (CsrBtAvActivateCfm*)msg;
                handleAvActivateCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_STREAM_DATA_IND:
            {
                CsrBtAvStreamDataInd *prim;
                prim = (CsrBtAvStreamDataInd*)msg;
                handleAvStreamDataInd(instData, prim);
                break;
            }

        case CSR_BT_AV_QOS_IND:
            {
                CsrBtAvQosInd *prim;
                prim = (CsrBtAvQosInd*)msg;
                handleAvQosInd(instData, prim);
                break;
            }

        case CSR_BT_AV_CONNECT_IND:
            {
                CsrBtAvConnectInd *prim;
                prim = (CsrBtAvConnectInd*)msg;
                handleAvConnectInd(instData, prim);
                break;
            }

        case CSR_BT_AV_CONNECT_CFM:
            {
                CsrBtAvConnectCfm *prim;
                prim = (CsrBtAvConnectCfm*)msg;
                handleAvConnectCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_DISCOVER_CFM:
            {
                CsrBtAvDiscoverCfm *prim;
                prim = (CsrBtAvDiscoverCfm*)msg;
                handleAvDiscoverCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_DISCOVER_IND:
            {
                CsrBtAvDiscoverInd *prim;
                prim = (CsrBtAvDiscoverInd*)msg;
                handleAvDiscoverInd(instData, prim);
                break;
            }

        case CSR_BT_AV_GET_CAPABILITIES_CFM:
            {
                CsrBtAvGetCapabilitiesCfm *prim;
                prim = (CsrBtAvGetCapabilitiesCfm*)msg;
                handleAvGetCapabilitiesCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_STATUS_IND:
            {
                CsrBtAvStatusInd *prim;
                prim = (CsrBtAvStatusInd*)msg;
                handleAvStatusInd(instData, prim);
                break;
            }

        case CSR_BT_AV_GET_ALL_CAPABILITIES_IND:
        case CSR_BT_AV_GET_CAPABILITIES_IND:
            {
                CsrBtAvGetCapabilitiesInd *prim;
                prim = (CsrBtAvGetCapabilitiesInd*)msg;
                handleAvGetCapabilitiesInd(instData, prim);
                break;
            }

        case CSR_BT_AV_SET_CONFIGURATION_CFM:
            {
                CsrBtAvSetConfigurationCfm *prim;
                prim = (CsrBtAvSetConfigurationCfm*)msg;
                handleAvSetConfigurationCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_SET_CONFIGURATION_IND:
            {
                CsrBtAvSetConfigurationInd *prim;
                prim = (CsrBtAvSetConfigurationInd*)msg;
                handleAvSetConfigurationInd(instData, prim);
                break;
            }

        case CSR_BT_AV_OPEN_CFM:
            {
                CsrBtAvOpenCfm *prim;
                prim = (CsrBtAvOpenCfm*)msg;
                handleAvOpenCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_OPEN_IND:
            {
                CsrBtAvOpenInd *prim;
                prim = (CsrBtAvOpenInd*)msg;
                handleAvOpenInd(instData, prim);
                break;
            }

        case CSR_BT_AV_START_CFM:
            {
                CsrBtAvStartCfm *prim;
                prim = (CsrBtAvStartCfm*)msg;
                handleAvStartCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_START_IND:
            {
                CsrBtAvStartInd *prim;
                prim = (CsrBtAvStartInd*)msg;
                handleAvStartInd(instData, prim);

                break;
            }

        case CSR_BT_AV_CLOSE_CFM:
            {
                CsrBtAvCloseCfm *prim;
                prim = (CsrBtAvCloseCfm*)msg;
                handleAvCloseCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_CLOSE_IND:
            {
                CsrBtAvCloseInd *prim;
                prim = (CsrBtAvCloseInd*)msg;
                handleAvCloseInd(instData, prim);
                break;
            }

        case CSR_BT_AV_SUSPEND_CFM:
            {
                CsrBtAvSuspendCfm *prim;
                prim = (CsrBtAvSuspendCfm*)msg;
                handleAvSuspendCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_SUSPEND_IND:
            {
                CsrBtAvSuspendInd *prim;
                prim = (CsrBtAvSuspendInd*)msg;
                handleAvSuspendInd(instData, prim);
                break;
            }

        case CSR_BT_AV_RECONFIGURE_CFM:
            {
                CsrBtAvReconfigureCfm *prim;
                prim = (CsrBtAvReconfigureCfm *) msg;
                handleAvReconfigureCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_RECONFIGURE_IND:
            {
                CsrBtAvReconfigureInd *prim;
                prim = (CsrBtAvReconfigureInd*)msg;
                handleAvReconfigureInd(instData, prim);
                break;
            }

        case CSR_BT_AV_GET_CONFIGURATION_CFM:
            {
                CsrBtAvGetConfigurationCfm *prim;
                prim = (CsrBtAvGetConfigurationCfm*)msg;
                handleAvGetConfigurationCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_GET_CONFIGURATION_IND:
            {
                CsrBtAvGetConfigurationInd *prim;
                prim = (CsrBtAvGetConfigurationInd*)msg;
                handleAvGetConfigurationInd(instData, prim);
                break;
            }

        case CSR_BT_AV_ABORT_CFM:
            {
                CsrBtAvAbortCfm *prim;
                prim = (CsrBtAvAbortCfm*)msg;
                handleAvAbortCfm(instData, prim);
                break;
            }

        case CSR_BT_AV_ABORT_IND:
            {
                CsrBtAvAbortInd *prim;
                prim = (CsrBtAvAbortInd*)msg;
                handleAvAbortInd(instData, prim);
                break;
            }

        case CSR_BT_AV_DISCONNECT_IND:
            {
                CsrBtAvDisconnectInd *prim;
                prim = (CsrBtAvDisconnectInd*)msg;
                handleAvDisconnectInd(instData, prim);
                break;
            }

        case CSR_BT_AV_STREAM_MTU_SIZE_IND:
            {
                CsrBtAvStreamMtuSizeInd *prim;
                prim = (CsrBtAvStreamMtuSizeInd*)msg;
                handleAvStreamMtuSizeInd(instData, prim);
                break;
            }
        case CSR_BT_AV_SECURITY_CONTROL_IND:
            {
                CsrBtAvSecurityControlInd *prim;
                prim = (CsrBtAvSecurityControlInd*)msg;
                handleAvSecurityControlInd(instData, prim);
                break;
            }
        case CSR_BT_AV_SECURITY_CONTROL_CFM:
            {
                CsrBtAvSecurityControlCfm *prim;
                prim = (CsrBtAvSecurityControlCfm*)msg;
                handleAvSecurityControlCfm(instData, prim);
                break;
            }
        case CSR_BT_AV_DELAY_REPORT_IND:
            {
                CsrBtAvDelayReportInd *prim;
                prim = (CsrBtAvDelayReportInd *)msg;
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Delay report message received. Value: %d msecs\n", prim->delay/10);
                
                CsrBtAvDelayReportResAcpSend(prim->shandle, prim->tLabel);
                break;
            }
        case CSR_BT_AV_GET_CHANNEL_INFO_CFM:
            {
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtAvGetChannelInfoCfm *prim = (CsrBtAvGetChannelInfoCfm *)msg;

                if (prim->resultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
                {
                    instData->avCon[instData->currentConnection].remoteCid = prim->remoteCid;
                    instData->avCon[instData->currentConnection].aclHandle = prim->aclHandle;
                }
#endif
                break;
            }
        default:
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Unhandled AV primitive: 0x%04x,\n", *primType);
            }
    }
}

