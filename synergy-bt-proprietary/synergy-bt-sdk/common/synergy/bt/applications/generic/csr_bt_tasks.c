/****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "csr_types.h"
#include "csr_sched_init.h"

#include "csr_bt_declare_tasks.h"

#include "csr_ui_task.h"
#include "csr_app_task.h"
#include "csr_app_main.h"
#include "csr_bt_gap_app_task.h"
#include "csr_bt_spp_app_task.h"
#include "csr_bt_pan_app_task.h"
#include "csr_bt_gnss_client_app_task.h"
#include "csr_bt_gnss_server_app_task.h"
#include "csr_bt_pac_app_task.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_hfg_app_task.h"
#include "csr_bt_hf_app_task.h"
#include "csr_bt_av_app_task.h"
#include "csr_bt_ftc_app_task.h"
#include "csr_bt_avrcp_app_task.h"
#include "csr_bt_fts_app_task.h"
#include "csr_bt_phdc_ag_app_task.h"
#include "csr_bt_phdc_mgr_app_task.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_bootstrap_generic.h"
#include "csr_bt_profiles.h"
#include "csr_tm_bluecore_task.h"
#include "csr_arg_search.h"
#include "csr_bt_log_tech_info_register.h"
#include "csr_ip_task.h"

void CsrAppMainUsage(void)
{
    CsrBtGapUsage();

#if NUM_AV_INST >= 1
    CsrBtAvAppUsage();
#endif
}
        
CsrResult CsrAppMain(void)
{
    return CSR_RESULT_SUCCESS;
}

/* CSR scheduler task initialisation */
void CsrSchedTaskInit(void *data)
{
    CsrUint8 address[6];
    CsrBtDeviceAddr localAddress;
    CsrBool isPhone = FALSE;    /* Default is carkit(handsfree) device. */
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    CsrCharString *parameter, *value;
#endif
#if (NUM_SPP_INST > 1) || (NUM_GATT_PROXC_INST > 1 || NUM_GATT_THERMC_INST > 1)
    CsrUint16 i = 0; /*used for SPP and Proximity demo */
#endif
#if ((CSR_FRW_VERSION_MAJOR >= 3) && (CSR_FRW_VERSION_MINOR >= 0) && (CSR_FRW_VERSION_FIXLEVEL >= 1)) || \
        ((CSR_FRW_VERSION_MAJOR >= 3) && (CSR_FRW_VERSION_MINOR > 0)) || \
            (CSR_FRW_VERSION_MAJOR > 3)
    /* [QTI] Device name set through pskey is only valid for CSR BlueCore chip, instead of QCA chip. */
#ifdef CSR_USE_BLUECORE_CHIP
    CsrUtf8String   *localDeviceName;
    if ((localDeviceName = (CsrUtf8String *)CsrAppMainBluecoreLocalDeviceNameGet()) != NULL) 
    {
        CsrBtBootstrapSetFriendlyName(NULL,localDeviceName);
    }
#endif
#endif

    /* [QTI] Omit to set interface/bitrate/frequency for QCA chip, because it's specific with BlueCore chip. */
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    if (CsrArgSearch(NULL, "--bt-dev", &parameter, &value) && (value != NULL))
    {
        isPhone = !CsrStrCmp(value, "phone") ? TRUE : FALSE;
    }   
#else
#ifdef CSR_USE_BLUECORE_CHIP
    CsrBtBootstrapSetInterface(NULL, CsrAppMainBluecoreTransportGet());
    CsrBtBootstrapSetBitRate(NULL, CsrAppMainBluecorePostBootstrapBaudrateGet());
    CsrBtBootstrapSetFrequency(NULL, (CsrUint16) (CsrAppMainBluecoreXtalFrequencyGet() & 0xFFFF));
#endif
#ifdef CSR_USE_QCA_CHIP
    CsrBtBootstrapSetQcFirmwareLog(CsrAppMainQcFrwHciLogGet());
#endif
#endif

    CsrAppMainBluecoreLocalDeviceBluetoothAddressGet(address);
    localAddress.nap = (address[0] << 8) | address[1];
    localAddress.uap = address[2];
    localAddress.lap = (address[3] << 16) | (address[4] << 8) | address[5];
    CsrBtBootstrapSetLocalBtAddress(NULL, &localAddress);

    if(CsrAppMainBluecorePsrFileGet())
    {
        CsrBtBootstrapParsePsrFile(NULL, CsrAppMainBluecorePsrFileGet());
    }

    if(CsrAppMainBluecorePsrStringGet())
    {
        CsrBtBootstrapParsePsrConstant(NULL, CsrAppMainBluecorePsrStringGet());
    }

    switch (CsrAppMainBluecoreTransportGet())
    {
#ifdef CSR_BT_APP_USE_SERIAL_COM
        case TRANSPORT_TYPE_BCSP:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_BCSP_INIT;
            break;
#endif
#ifdef CSR_BT_APP_USE_USB
        case TRANSPORT_TYPE_USB:
#ifdef CSR_BT_APP_HCI_SOCKET_TRANSPORT
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_HCI_SOCKET_INIT;
#else
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_USB_INIT;
#endif
            break;
#endif

#ifdef CSR_BT_APP_USE_SERIAL_COM
#ifdef CSR_H4_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_UART:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4_UART_INIT;
            break;
#endif
#ifdef CSR_H4DS_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_DS:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4DS_INIT;
            break;
#endif
#ifdef CSR_H4IBS_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_IBS:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4IBS_INIT;
            break;
#endif
#ifdef CSR_IBFP_H4I_SUPPORT
        case TRANSPORT_TYPE_H4_I:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4I_INIT;
            break;
#endif
#endif

        /* [QTI] Add BlueZ transport through socket. */
#ifdef CSR_HCI_SOCKET_TRANSPORT
        case TRANSPORT_TYPE_BLUEZ:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_HCI_SOCKET_INIT;
            break;
#endif

#ifdef CSR_BT_APP_USE_TYPE_A
        case TRANSPORT_TYPE_TYPE_A:
            CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_TYPE_A_INIT;
            break;
#endif
        default:
            break;
    }

    /* Initialise standard tasks (see csr_bt_declare_tasks.c) */
    CsrBtAppTaskInitFw(data);

    CsrBtAppTaskInitBt(data);

#ifndef EXCLUDE_CSR_BT_BSL_MODULE
    CsrSchedRegisterTask(&CSR_BT_BNEP_IFACEQUEUE, CSR_BT_BNEP_INIT, CSR_BT_BNEP_DEINIT, CSR_BT_BNEP_HANDLER, "CSR_BT_BNEP", data, ID_STACK);
#endif /* EXCLUDE_CSR_BT_BSL_MODULE */

#ifdef CSR_BT_LE_ENABLE
    CsrSchedRegisterTask(&ATT_IFACEQUEUE, ATT_INIT, ATT_DEINIT, ATT_TASK, "CSR_BT_ATT", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_GATT_IFACEQUEUE, CSR_BT_GATT_INIT, CSR_BT_GATT_DEINIT, CSR_BT_GATT_HANDLER, "CSR_BT_GATT", data, ID_STACK);



#if NUM_GATT_THERMS_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_THERM_SRV_IFACEQUEUE, CSR_BT_THERM_SRV_INIT, CSR_BT_THERM_SRV_DEINIT, CSR_BT_THERM_SRV_HANDLER, "CSR_BT_THERM_SRV", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_THERMS_APP_IFACEQUEUE, CsrBtThermsAppInit, CsrBtThermsAppDeinit, CsrBtThermsAppHandler, "CSR_BT_APP_THERMS", data, ID_STACK);
#endif

#if NUM_GATT_LE_BROWSER_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_LE_BROWSER_APP_IFACEQUEUE, CsrBtLeBrowserAppInit, CsrBtLeBrowserAppDeinit, CsrBtLeBrowserAppHandler, "CSR_BT_APP_LE_BROWSER", data, ID_STACK);
#if NUM_GATT_LE_BROWSER_INST > 1
    for (i = 1; i < NUM_GATT_LE_BROWSER_INST; i++)
    {
        CsrSchedRegisterTask(&CSR_BT_LE_BROWSER_APP_EXTRA_IFACEQUEUE, CsrBtLeBrowserAppInit, CsrBtLeBrowserAppDeinit, CsrBtLeBrowserAppHandler, "CSR_BT_APP_LE_BROWSER_EXTRA", data, ID_STACK);
    }
#endif
#endif

#if NUM_GATT_THERMC_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_THERMC_APP_IFACEQUEUE, CsrBtThermcAppInit, CsrBtThermcAppDeinit, CsrBtThermcAppHandler, "CSR_BT_APP_THERMC", data, ID_STACK);
#if NUM_GATT_THERMC_INST > 1
    for (i = 1; i < NUM_GATT_THERMC_INST; i++)
    {
        CsrSchedRegisterTask(&CSR_BT_THERMC_APP_EXTRA_IFACEQUEUE, CsrBtThermcAppInit, CsrBtThermcAppDeinit, CsrBtThermcAppHandler, "CSR_BT_APP_THERMC_EXTRA", data, ID_STACK);
    }
#endif
#endif

#if NUM_GATT_GENERIC_SERVER_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE, CsrBtGenericServerAppInit, CsrBtGenericServerAppDeinit, CsrBtGenericServerAppHandler, "CSR_BT_APP_GENERIC_SERVER", data, ID_STACK);
#endif

#if NUM_GATT_PROXS_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_PROX_SRV_IFACEQUEUE, CSR_BT_PROX_SRV_INIT, CSR_BT_PROX_SRV_DEINIT, CSR_BT_PROX_SRV_HANDLER, "CSR_BT_PROX_SRV", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_PROXS_APP_IFACEQUEUE, CsrBtProxsAppInit, CsrBtProxsAppDeinit, CsrBtProxsAppHandler, "CSR_BT_APP_PROXS", data, ID_STACK);
#endif

#if NUM_GATT_PROXC_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_PROXC_APP_IFACEQUEUE, CsrBtProxcAppInit, CsrBtProxcAppDeinit, CsrBtProxcAppHandler, "CSR_BT_APP_PROXC", data, ID_STACK);
#if NUM_GATT_PROXC_INST > 1
    for (i = 1; i < NUM_GATT_PROXC_INST; i++)
    {
        CsrSchedRegisterTask(&CSR_BT_PROXC_APP_EXTRA_IFACEQUEUE, CsrBtProxcAppInit, CsrBtProxcAppDeinit, CsrBtProxcAppHandler, "CSR_BT_APP_PROXC_EXTRA", data, ID_STACK);
    }
#endif
#endif

#if NUM_GATT_RSCS_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_RSCS_APP_IFACEQUEUE, CsrBtRscsAppInit, CsrBtRscsAppDeinit, CsrBtRscsAppHandler, "CSR_BT_APP_RSCS", data, ID_STACK);
#endif

#if NUM_GATT_RSCC_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_RSCC_APP_IFACEQUEUE, CsrBtRsccAppInit, CsrBtRsccAppDeinit, CsrBtRsccAppHandler, "CSR_BT_APP_RSCC", data, ID_STACK);
#if NUM_GATT_RSCC_INST > 1
    for (i = 1; i < NUM_GATT_RSCC_INST; i++)
    {
        CsrSchedRegisterTask(&CSR_BT_RSCC_APP_EXTRA_IFACEQUEUE, CsrBtRsccAppInit, CsrBtRsccAppDeinit, CsrBtRsccAppHandler, "CSR_BT_APP_RSCC_EXTRA", data, ID_STACK);
    }
#endif
#endif

#if NUM_GATT_HOGH_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_HOGH_IFACEQUEUE, CSR_BT_HOGH_INIT, CSR_BT_HOGH_DEINIT, CSR_BT_HOGH_HANDLER, "CSR_BT_HOGH", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_HOGH_APP_IFACEQUEUE, CsrBtHoghAppInit, CsrBtHoghAppDeinit, CsrBtHoghAppHandler, "CSR_BT_APP_HOGH", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_HOGD_APP_IFACEQUEUE, CsrBtHogdAppInit, CsrBtHogdAppDeinit, CsrBtHogdAppHandler, "CSR_BT_APP_HOGD", data, ID_STACK);
#endif

#endif /* CSR_BT_LE_ENABLE */

    /* Generic access profile - keep first to display at the top in the UI */
    CsrSchedRegisterTask(&CSR_BT_GAP_APP_IFACEQUEUE, CsrBtGapInit, CsrBtGapDeinit, CsrBtGapHandler, "CSR_BT_APP_GAP", data, ID_STACK);

    /* FW application*/
    /* [QTI] Register "CSR_APP" task in ID_STACK instead of ID_APP, because only 1 scheduler is supported. */
    CsrSchedRegisterTask(&CSR_APP_IFACEQUEUE, CSR_APP_INIT, CSR_APP_DEINIT, CSR_APP_HANDLER, "CSR_APP", data, ID_STACK);
    /* [QTI] Register "CSR_UI" task in ID_STACK instead of ID_APP, because only 1 scheduler is supported. */
    CsrSchedRegisterTask(&CSR_UI_IFACEQUEUE, CSR_UI_INIT, CSR_UI_DEINIT, CSR_UI_HANDLER, "CSR_UI", data, ID_STACK);

#if NUM_SPP_INST >= 1
    CsrSchedRegisterTask(&CSR_BT_SPP_IFACEQUEUE, CSR_BT_SPP_INIT, CSR_BT_SPP_DEINIT, CSR_BT_SPP_HANDLER, "CSR_BT_SPP", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_SPP_APP_IFACEQUEUE, CsrBtSppAppInit, CsrBtSppAppDeinit, CsrBtSppAppHandler, "CSR_BT_APP_SPP", data, ID_STACK);
#if NUM_SPP_INST > 1
    for(i=1;i<NUM_SPP_INST;i++)
    {
        CsrSchedRegisterTask(&CSR_BT_SPP_EXTRA_IFACEQUEUE, CSR_BT_SPP_INIT, CSR_BT_SPP_DEINIT, CSR_BT_SPP_HANDLER, "CSR_BT_SPP_EXTRA", data, ID_STACK);
        CsrSchedRegisterTask(&CSR_BT_SPP_APP_EXTRA_IFACEQUEUE, CsrBtSppAppInit, CsrBtSppAppDeinit, CsrBtSppAppHandler, "CSR_BT_APP_SPP_EXTRA", data, ID_STACK);
    }
#endif
#endif

#if NUM_PAN_INST >= 1
    CsrSchedRegisterTask(&CSR_IP_IFACEQUEUE, CSR_IP_INIT, CSR_IP_DEINIT, CSR_IP_HANDLER, "IP", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_BSL_IFACEQUEUE, CSR_BT_BSL_INIT, CSR_BT_BSL_DEINIT,CSR_BT_BSL_HANDLER, "CSR_BT_BSL", data, ID_STACK);
    CsrSchedRegisterTask(&CSR_BT_PAN_APP_IFACEQUEUE, CsrBtPanAppInit, CsrBtPanAppDeinit, CsrBtPanAppHandler, "CSR_BT_APP_PAN", data, ID_STACK);
#endif

#if NUM_HFG_INST >= 1
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    if (isPhone)
    {
        CsrSchedRegisterTask(&CSR_BT_HFG_IFACEQUEUE, CSR_BT_HFG_INIT, CSR_BT_HFG_DEINIT, CSR_BT_HFG_HANDLER, "CSR_BT_HFG", data, ID_STACK);
        CsrSchedRegisterTask(&CSR_BT_HFG_APP_IFACEQUEUE, CsrBtHfgAppInit, CsrBtHfgAppDeinit, CsrBtHfgAppHandler, "CSR_BT_APP_HFG", data, ID_STACK);
    }
#endif
#endif

#if NUM_HF_INST >= 1
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    if (!isPhone)
    {
        CsrSchedRegisterTask(&CSR_BT_HF_IFACEQUEUE, CSR_BT_HF_INIT, CSR_BT_HF_DEINIT, CSR_BT_HF_HANDLER, "CSR_BT_HF", data, ID_STACK);
        CsrSchedRegisterTask(&CSR_BT_HF_APP_IFACEQUEUE, CsrBtHfAppInit, CsrBtHfAppDeinit, CsrBtHfAppHandler, "CSR_BT_APP_HF", data, ID_STACK);
    }
#endif
#endif

#if NUM_AV_INST >= 1
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    /* if (isPhone) */
    {
        CsrSchedRegisterTask(&CSR_BT_AV_IFACEQUEUE, CSR_BT_AV_INIT, CSR_BT_AV_DEINIT, CSR_BT_AV_HANDLER, "CSR_BT_AV", data, ID_STACK);
        CsrSchedRegisterTask(&CSR_BT_AV_APP_IFACEQUEUE, CsrBtAvAppInit, CsrBtAvAppDeinit, CsrBtAvAppHandler, "CSR_BT_APP_AV", data, ID_STACK);
    }
#endif
#endif

#if NUM_PAC_INST >= 1
#if defined(USE_MDM_PLATFORM)|| defined(USE_IMX_PLATFORM)
    if (!isPhone)
    {
        CsrSchedRegisterTask(&CSR_BT_PAC_IFACEQUEUE, CSR_BT_PAC_INIT, CSR_BT_PAC_DEINIT, CSR_BT_PAC_HANDLER, "CSR_BT_PAC", data, ID_STACK);
        CsrSchedRegisterTask(&CSR_BT_PAC_APP_IFACEQUEUE, CsrBtPacAppInit, CsrBtPacAppDeinit, CsrBtPacAppHandler, "CSR_BT_APP_PAC", data, ID_STACK);
    }
#endif
#endif

#ifdef CSR_LOG_ENABLE
    CsrBtLogTechInfoRegister();
#endif
}
