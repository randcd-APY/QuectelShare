/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_bt_avrcp_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_avrcp.h"
#include "csr_bt_avrcp_app_util.h"
#include "csr_bt_avrcp_app_ui_sef.h"


/**************************************************************************************************
 * startConnectingAvrcp
 **************************************************************************************************/

#include "csr_synergy.h"
void startConnectingAvrcp(avrcpinstance_t *instData)
{
    instData->avrcpConnectPending = FALSE;
    if (instData->avrcpCon[instData->currentConnection].state == DaAvStateAvrcpDisconnected)
    {
        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2("Connecting AVRCP...\n") , TEXT_OK_UCS2, NULL);
        /* Setup an AVRCP connection from AV sink (AVRCP controller) */
        CsrBtAvrcpConnectReqSend(instData->remoteAddr[instData->currentConnection]);

        instData->avrcpCon[instData->currentConnection].state = DaAvStateAvrcpConnecting;
    }
    else
    {
        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2("ERROR: AVRCP connection could not be established\n") , TEXT_OK_UCS2, NULL);
    }



    /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
}


/**************************************************************************************************
 * startDisconnect
 **************************************************************************************************/

void startAvrcpDisconnect(avrcpinstance_t *instData)
{
    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
            CONVERT_TEXT_STRING_2_UCS2("Disconnecting AVRCP - please wait...\n") , TEXT_OK_UCS2, NULL);
    CsrBtAvrcpDisconnectReqSend(instData->avrcpCon[instData->currentConnection].connectionId);

    /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
}

/**************************************************************************************************
 * startActivate
 **************************************************************************************************/
void startAVRCPActivate(avrcpinstance_t *instData)
{

    CsrBtAvrcpRoleDetails tgFeatures;
    CsrBtAvrcpRoleDetails ctFeatures;

    if ((instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT) ||
        (instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_ONLY))
    {
        CsrBtAvrcpConfigRoleSupport(&tgFeatures,                                         /* Pointer to details */
                               CSR_BT_AVRCP_CONFIG_ROLE_STANDARD,                        /* Role config */
                               instData->avrcpVersion,                                   /* AVRCP version */
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT1_PLAY_REC |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT2_MON_AMP |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT3_TUNER |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT4_MENU |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_PAS |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_BROWSING |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_MULTIPLE_MP |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART,                     /* Features */
                                    (CsrCharString*)CsrStrDup("CSR"),                                         /* Provider name */
                                    (CsrCharString*)CsrStrDup("AVRCP TG"));                                   /* Service name */
    }
    else
    {
        CsrBtAvrcpConfigRoleNoSupport(&tgFeatures);
    }

    if ((instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT) ||
        (instData->avrcpConfig == DA_AV_CONFIG_AVRCP_CT_ONLY))
    {
        CsrBtAvrcpConfigRoleSupport(&ctFeatures,                                         /* Pointer to details */
                               CSR_BT_AVRCP_CONFIG_ROLE_STANDARD,                        /* Role config */
                               instData->avrcpVersion,                                   /* AVRCP version */
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT1_PLAY_REC |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT2_MON_AMP |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT3_TUNER |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT4_MENU |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_BROWSING |
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG_PROP|
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG|
                               CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG_THUMB,                     /* Features */
                                    (CsrCharString*)CsrStrDup("CSR"),                                         /* Provider name */
                                    (CsrCharString*)CsrStrDup("AVRCP CT"));                                   /* Service name */
    }
    else
    {
        CsrBtAvrcpConfigRoleNoSupport(&ctFeatures);
    }

    CsrBtAvrcpConfigReqSend(instData->CsrSchedQid,              /* Phandle */
                       CSR_BT_AVRCP_CONFIG_GLOBAL_STANDARD,    /* Global configuration */
                       instData->maxMtu,                        /* MTU */
                       tgFeatures,                  /* Target specific configuration */
                       ctFeatures);                 /* Controller specific configuration */

}


