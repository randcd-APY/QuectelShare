/****************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_bt_gap_app_util.h"
#include "csr_bt_gap_app_sd.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_gap_app_ui_strings.h"
#include "csr_util.h"
#include "csr_formatted_io.h"
#include "csr_bt_gap_app_sd_event_handler.h"
#include "csr_bt_gap_app_ui_sef.h"
#include "csr_bt_gap_app_prim.h"

const static char *AddrTable[] = {
    "public",
    "private"
};

static void appAddServiceItemToUi(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
              CsrUieHandle backEventHandle, CsrBool availableServices, CsrUint16 icon, CsrUint16 *label)
{
    if (!availableServices)
    { /* Remove all items from the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                                */
        CsrUiMenuRemoveallitemsReqSend(displayHandle);
    }
    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_INVALID_KEY, icon, label,
                                   NULL, sk1EventHandle, CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
}

static void appAddSerialService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                SPP_SERVICE_ICON, TEXT_SPP_UCS2);
}

static void appAddLanAccessService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                LAN_ACCESS_USING_PPP_SERVICE_ICON, TEXT_LAN_ACCESS_USING_PPP_UCS2);
}

static void appAddDialUpService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        DIAL_UP_NETWOKING_SERVICE_ICON, TEXT_DIAL_UP_NETWOKING_UCS2);
}

static void appAddIrMcSyncService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    IR_MC_SYNC_SERVICE_ICON, TEXT_IR_MC_SYNC_UCS2);
}

static void appAddObjectPushService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                OPC_SERVICE_ICON, TEXT_OPC_UCS2);
}

static void appAddFileTransferService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                FTC_SERVICE_ICON, TEXT_FTC_UCS2);
}

static void appAddIrMcSyncCmdService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    IR_MC_SYNC_COMMAND_SERVICE_ICON, TEXT_IR_MC_SYNC_COMMAND_UCS2);
}

static void appAddHeadsetService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                    HS_SERVICE_ICON, TEXT_HS_UCS2);
}

static void appAddCordlessTelephonyService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    CORDLESS_TELEPHONY_SERVICE_ICON, TEXT_CORDLESS_TELEPHONY_UCS2);
}

static void appAddAudioSrcService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                AUDIO_SOURCE_SERVICE_ICON, TEXT_AUDIO_SOURCE_UCS2);
}

static void appAddAudioSinkService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    AUDIO_SINK_SERVICE_ICON, TEXT_AUDIO_SINK_UCS2);
}

static void appAddAvRemoteCtrlTargetService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                        AV_REMOTECONTROL_TARGET_SERVICE_ICON, TEXT_AV_REMOTECONTROL_TARGET_UCS2);
}

static void appAddAvdAudioDistService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                            ADVANCED_AUDIO_DIST_SERVICE_ICON, TEXT_ADVANCED_AUDIO_DIST_SER_UCS2);
}

static void appAddAvRemoteCtrlService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    AV_REMOTE_CONTROL_SERVICE_ICON, TEXT_AV_REMOTE_CONTROL_SER_UCS2);
}

static void appAddVideoConfService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    VIDEO_CONFERENCING_SERVICE_ICON, TEXT_VIDEO_CONFERENCING_UCS2);
}

static void appAddIntercomService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                            INTERCOM_SERVICE_ICON, TEXT_INTERCOM_UCS2);
}

static void appAddFaxService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                FAX_SERVICE_ICON, TEXT_FAX_UCS2);
}

static void appAddHeadsetAudioGWService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                HEADSET_AUDIO_GATEWAY_SERVICE_ICON, TEXT_HEADSET_AUDIO_GATEWAY_UCS2);
}

static void appAddWapService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                WAP_SERVICE_ICON, TEXT_WAP_UCS2);
}

static void appAddWapCliService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    WAP_CLIENT_SERVICE_ICON, TEXT_WAP_CLIENT_UCS2);
}

static void appAddPanuService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                PANU_SERVICE_ICON, TEXT_PANU_UCS2);
}

static void appAddNapService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                NAP_SERVICE_ICON, TEXT_NAP_UCS2);
}

static void appAddGnService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                    GN_SERVICE_ICON, TEXT_GN_UCS2);
}

static void appAddDirectPrintingService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                          DIRECT_PRINTING_SERVICE_ICON, TEXT_DIRECT_PRINTING_UCS2);
}

static void appAddRefPrintingService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                          REFERENCE_PRINTING_SERVICE_ICON, TEXT_REFERENCE_PRINTING_UCS2);
}

static void appAddImagingService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                            IMAGING_SERVICE_ICON, TEXT_IMAGING_UCS2);
}

static void appAddImagingRspService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        IMAGING_RESPONDER_SERVICE_ICON, TEXT_IMAGING_RESPONDER_UCS2);
}

static void appAddImagingAutomaticArchService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                         IMAGING_AUTOMATIC_ARCHIVE_SERVICE_ICON, TEXT_IMAGING_AUTOMATIC_ARCHIVE_UCS2);
}

static void appAddImagingRefObjService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                         IMAGING_REFERENCE_OBJECTS_SERVICE_ICON, TEXT_IMAGING_REFERENCE_OBJECTS_UCS2);
}

static void appAddHandsfreeService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                        HF_SERVICE_ICON, TEXT_HF_UCS2);
}

static void appAddHandsfreeAudioGWService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                HANDSFREE_AUDIO_GATEWAY_SERVICE_ICON, TEXT_HANDSFREE_AUDIO_GATEWAY_UCS2);
}

static void appAddDirectPrintingRefObjService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                DIRECT_PRINT_REF_OBJ_SERVICE_ICON, TEXT_DIRECT_PRINT_REF_OBJ_SER_UCS2);
}

static void appAddReflectedUiService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        REFLECTED_UI_ICON, TEXT_REFLECTED_UI_UCS2);
}

static void appAddBasicPrintingService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    BASIC_PRINTING_ICON, TEXT_BASIC_PRINTING_UCS2);
}

static void appAddPrintingStatusService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                PRINTING_STATUS_ICON, TEXT_PRINTING_STATUS_UCS2);
}

static void appAddHidService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    HUMAN_INTERFACE_DEVICE_ICON, TEXT_HUMAN_INTERFACE_DEVICE_UCS2);
}

static void appAddHcrpService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                        HCRP_ICON, TEXT_HCRP_UCS2);
}

static void appAddHcrPrintService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                HCR_PRINT_SERVICE_ICON, TEXT_HCR_PRINT_SERVICE_UCS2);
}

static void appAddHcrScanCmdService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        HCR_SCAN_SERVICE_ICON, TEXT_HCR_SCAN_SERVICE_UCS2);
}

static void appAddCommonIsdnAccessService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                    COMMON_ISDN_ACCESS_SERVICE_ICON, TEXT_COMMON_ISDN_ACCESS_UCS2);
}

static void appAddVideoConfGwService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        VIDEO_CONFERENCING_GW_ICON, TEXT_VIDEO_CONFERENCING_GW_UCS2);
}

static void appAddUdiMtService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    UDI_MT_SERVICE_ICON, TEXT_UDI_MT_SERVICE_UCS2);
}

static void appAddUdiTaService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                    UDI_TA_SERVICE_ICON, TEXT_UDI_TA_SERVICE_UCS2);
}

static void appAddAudioVideoService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        AUDIO_VIDEO_SERVICE_ICON, TEXT_AUDIO_VIDEO_SERVICE_UCS2);
}

static void appAddSimAccessService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                            SIM_ACCESS_SERVICE_ICON, TEXT_SIM_ACCESS_SERVICE_UCS2);
}

static void appAddPbapPceService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        PHONEBOOK_ACCESS_PCE_ICON, TEXT_PHONEBOOK_ACCESS_PCE_UCS2);
}

static void appAddPbapPseService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        PHONEBOOK_ACCESS_PSE_ICON, TEXT_PHONEBOOK_ACCESS_PSE_UCS2);
}


static const CsrBtGapUpdateServiceType knownServices11_00_63Handlers[NUM_OF_11_00_63_SEVICES] =
{ /* Jump tabel to add services in the interval knownServices11_00_31 to the CSR_BT_GAP_REMOTE_SERVICES_UI      */
    /* Interval knownServices11_00_31                                                                       */
    appAddSerialService,                          /* SERIAL_PORT_SERVICE                                  */
    appAddLanAccessService,                       /* LAN_ACCESS_USING_PPP_SERVICE                         */
    appAddDialUpService,                          /* DIALUP_NETWORKING_SERVICE                            */
    appAddIrMcSyncService,                        /* IR_MC_SYNC_SERVICE                                   */
    appAddObjectPushService,                      /* OBEX_OBJECT_PUSH_SERVICE                             */
    appAddFileTransferService,                    /* OBEX_FILE_TRANSFER_SERVICE                           */
    appAddIrMcSyncCmdService,                     /* IR_MC_SYNC_COMMAND_SERVICE                           */
    appAddHeadsetService,                         /* HEADSET_SERVICE                                      */
    appAddCordlessTelephonyService,               /* CORDLESS_TELEPHONY_SERVICE                           */
    appAddAudioSrcService,                        /* AUDIO_SOURCE_SERVICE                                 */
    appAddAudioSinkService,                       /* AUDIO_SINK_SERVICE                                   */
    appAddAvRemoteCtrlTargetService,              /* AV_REMOTE_CONTROL_TARGET_SERVICE                     */
    appAddAvdAudioDistService,                    /* ADVANCED_AUDIO_DISTRIBUTION_SERVICE                  */
    appAddAvRemoteCtrlService,                    /* AV_REMOTE_CONTROL_SERVICE                            */
    appAddVideoConfService,                       /* VIDEO_CONFERENCING_SERVICE                           */
    appAddIntercomService,                        /* INTERCOM_SERVICE                                     */
    appAddFaxService,                             /* FAX_SERVICE                                          */
    appAddHeadsetAudioGWService,                  /* HEADSET_AUDIO_GATEWAY_SERVICE                        */
    appAddWapService,                             /* WAP_SERVICE                                          */
    appAddWapCliService,                          /* WAP_CLIENT_SERVICE                                   */
    appAddPanuService,                            /* PANU_SERVICE                                         */
    appAddNapService,                             /* NAP_SERVICE                                          */
    appAddGnService,                              /* GN_SERVICE                                           */
    appAddDirectPrintingService,                  /* DIRECT_PRINTING_SERVICE                              */
    appAddRefPrintingService,                     /* REFERENCE_PRINTING_SERVICE                           */
    appAddImagingService,                         /* IMAGING_SERVICE                                      */
    appAddImagingRspService,                      /* IMAGING_RESPONDER_SERVICE                            */
    appAddImagingAutomaticArchService,            /* IMAGING_AUTOMATIC_ARCHIVE_SERVICE                    */
    appAddImagingRefObjService,                   /* IMAGING_REFERENCED_OBJECTS_SERVICE                   */
    appAddHandsfreeService,                       /* HANDSFREE_SERVICE                                    */
    appAddHandsfreeAudioGWService,                /* HANDSFREE_AUDIO_GATEWAY_SERVICE                      */
    appAddDirectPrintingRefObjService,            /* DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE_SERVICE    */

    /* Interval knownServices11_32_63                                                                       */
    appAddReflectedUiService,                     /* REFLECTED_UI_SERVICE                                 */
    appAddBasicPrintingService,                   /* BASIC_PRINTING_SERVICE                               */
    appAddPrintingStatusService,                  /* PRINTING_STATUS_SERVICE                              */
    appAddHidService,                             /* HUMAN_INTERFACE_DEVICE_SERVICE_SERVICE               */
    appAddHcrpService,                            /* HARDCOPY_CABLE_REPLACEMENT_SERVICE                   */
    appAddHcrPrintService,                        /* HCR_PRINT_SERVICE                                    */
    appAddHcrScanCmdService,                      /* HCR_SCAN_SERVICE                                     */
    appAddCommonIsdnAccessService,                /* COMMON_ISDN_ACCESS_SERVICE                           */
    appAddVideoConfGwService,                     /* VIDEO_CONFERENCING_GW_SERVICE                        */
    appAddUdiMtService,                           /* UDI_MT_SERVICE                                       */
    appAddUdiTaService,                           /* UDI_TA_SERVICE                                       */
    appAddAudioVideoService,                      /* AUDIO_VIDEO_SERVICE                                  */
    appAddSimAccessService,                       /* SIM_ACCESS_SERVICE                                   */
    appAddPbapPceService,                         /* PHONEBOOK_ACCESS_PCE_SERVICE                         */
    appAddPbapPseService,                         /* PHONEBOOK_ACCESS_PSE_SERVICE                         */
};

static void appAddPnpInfoService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                PNP_INFORMATION_ICON, TEXT_PNP_INFORMATION_UCS2);
}

static void appAddGenericNetworkingService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                            GENERIC_NETWORKING_ICON, TEXT_GENERIC_NETWORKING_UCS2);
}

static void appAddGenericTransferService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                        GENERIC_FILE_TRANSFER_ICON, TEXT_GENERIC_FILE_TRANSFER_UCS2);
}

static void appAddGenericAudioService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        GENERIC_AUDIO_ICON, TEXT_GENERIC_AUDIO_UCS2);
}

static void appAddGenericTelephonyService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                GENERIC_TELEPHONY_ICON, TEXT_GENERIC_TELEPHONY_UCS2);
}

static void appAddUpnpService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        UPNP_SERVICE_ICON, TEXT_UPNP_SERVICE_UCS2);
}

static void appAddUpnpIpService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        UPNP_IP_SERVICE_ICON, TEXT_UPNP_IP_SERVICE_UCS2);
}

static const CsrBtGapUpdateServiceType knownServices12_00_31Handlers[NUM_OF_12_00_31_SEVICES] =
{ /* Jump tabel to add services in the interval knownServices12_00_31 to the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI      */

    appAddPnpInfoService,                         /* PNP_INFORMATION_SERVICE                              */
    appAddGenericNetworkingService,               /* GENERIC_NETWORKING_SERVICE                           */
    appAddGenericTransferService,                 /* GENERIC_FILE_TRANSFER_SERVICE                        */
    appAddGenericAudioService,                    /* GENERIC_AUDIO_SERVICE                                */
    appAddGenericTelephonyService,                /* GENERIC_TELEPHONY_SERVICE                            */
    appAddUpnpService,                            /* UPNP_SERVICE_SERVICE                                 */
    appAddUpnpIpService,                          /* UPNP_IP_SERVICE                                      */
};

static void appAddEsdpUpnpIpPanService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        ESDP_UPNP_IP_PAN_ICON, TEXT_ESDP_UPNP_IP_PAN_UCS2);
}

static void appAddEsdpUpnpIpLanService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        ESDP_UPNP_IP_LAP_ICON, TEXT_ESDP_UPNP_IP_LAP_UCS2);
}

static void appAddEsdpUpnpL2capService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                        ESDP_UPNP_L2CAP_ICON, TEXT_ESDP_UPNP_L2CAP_UCS2);
}

static void appAddVideoSrcService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                            VIDEO_SOURCE_ICON, TEXT_VIDEO_SOURCE_UCS2);
}

static void appAddVideoSnkService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                                VIDEO_SINK_ICON, TEXT_VIDEO_SINK_UCS2);
}

static void appAddVideoDistributionService(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices)
{
    appAddServiceItemToUi(displayHandle, sk1EventHandle, backEventHandle, availableServices,
                                                VIDEO_DISTRIBUTION_ICON, TEXT_VIDEO_DISTRIBUTION_UCS2);
}

static const CsrBtGapUpdateServiceType knownServices13_00_31Handlers[NUM_OF_13_00_31_SEVICES] =
{ /* Jump tabel to add services in the interval knownServices13_00_31 to the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI      */

    appAddEsdpUpnpIpPanService,                   /* ESDP_UPNP_IP_PAN_SERVICE                             */
    appAddEsdpUpnpIpLanService,                   /* ESDP_UPNP_IP_LAP_SERVICE                             */
    appAddEsdpUpnpL2capService,                   /* ESDP_UPNP_L2CAP_SERVICE                              */
    appAddVideoSrcService,                        /* VIDEO_SOURCE_SERVICE                                 */
    appAddVideoSnkService,                        /* VIDEO_SINK_SERVICE                                   */
    appAddVideoDistributionService,               /* VIDEO_DISTRIBUTION_SERVICE                           */
};

CsrBool AppUpdateRemoteServicesUi(CsrBtGapCsrUiType * csrUiVar, CsrUint32 infoLen, CsrUint8 *info)
{ /* Please note that currently only a limited number of services is check                  */
    CsrUieHandle displayHandle     = csrUiVar->displayesHandlers[CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI].displayHandle;
    CsrUieHandle sk1EventHandle    = csrUiVar->displayesHandlers[CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI].sk1EventHandle;
    CsrUieHandle backEventHandle   = csrUiVar->displayesHandlers[CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI].backEventHandle;
    CsrBool    availableServices = FALSE;
    CsrUint16    noOfServices;
    CsrUint32    * serviceList;

    noOfServices = CsrBtSdReadServiceList(info, infoLen, &serviceList);

    while(noOfServices > 0)
    {
        CsrUint8 serviceLsb;
        noOfServices--;

        serviceLsb = (CsrUint8) serviceList[noOfServices]-1; /*only take lsb of service and substract 1 to index the table correctly*/

        switch(serviceList[noOfServices] & 0xFFFFFF00)
        {
            case CSR_BT_GAP_SERVICE_11:
            {
                if ((serviceLsb < NUM_OF_11_00_63_SEVICES) && knownServices11_00_63Handlers[serviceLsb] != NULL)
                { /* Add the service to CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                           */
                    knownServices11_00_63Handlers[serviceLsb](displayHandle, sk1EventHandle,
                                                              backEventHandle, availableServices);

                    availableServices = TRUE;
                }
                else
                {
                    ;
                }
                break;
            }

            case CSR_BT_GAP_SERVICE_12:
            {
                if ((serviceLsb < NUM_OF_12_00_31_SEVICES) && knownServices12_00_31Handlers[serviceLsb] != NULL)
                { /* Add the service to CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                           */
                    knownServices12_00_31Handlers[serviceLsb](displayHandle, sk1EventHandle,
                                                              backEventHandle, availableServices);

                    availableServices = TRUE;
                }
                else
                {
                    ;
                }
                break;
            }

            case CSR_BT_GAP_SERVICE_13:
            {
                if ((serviceLsb < NUM_OF_13_00_31_SEVICES) && knownServices13_00_31Handlers[serviceLsb] != NULL)
                { /* Add the service to CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                           */
                    knownServices13_00_31Handlers[serviceLsb](displayHandle, sk1EventHandle,
                                                              backEventHandle, availableServices);

                    availableServices = TRUE;
                }
                else
                {
                    ;
                }
                break;
            }

            default:
            {
                break;
            }
        }

    }

    CsrPmemFree(serviceList);
    return (availableServices);
}

static void updateSearchingUi(CsrBtGapInstData * inst,
                              CsrBtTypedAddr typedAddr, CsrBtClassOfDevice deviceClass,
                              CsrCharString *friendlyName, CsrUint32 deviceStatus)
{
    CsrUint16         * codLabel;
    CsrCharString     * deviceName;
    CsrUint16         codIcon = CSR_UI_ICON_NONE;
    CsrCharString     deviceNameType[CSR_BT_MAX_FRIENDLY_NAME_LEN + sizeof("[BR|LE] ") + sizeof(AddrTable)];

    /* Convert the friendly name of the remote device from utf8 to ucs2         */
    deviceName = CsrBtGapReturnCharStringDeviceName(typedAddr.addr, friendlyName);

#ifdef CSR_BT_LE_ENABLE
    if(CSR_BT_SD_STATUS_RADIO_LE & deviceStatus && CSR_BT_SD_STATUS_RADIO_BREDR & deviceStatus)
    {
        CsrSnprintf(deviceNameType, sizeof(deviceNameType), "[LE|BR] %s [%s]", deviceName, AddrTable[typedAddr.type]);
    }
    else if (CSR_BT_SD_STATUS_RADIO_LE & deviceStatus)
    {
        CsrSnprintf(deviceNameType, sizeof(deviceNameType), "[LE] %s [%s]", deviceName, AddrTable[typedAddr.type]);
    }
    else if (CSR_BT_SD_STATUS_RADIO_BREDR & deviceStatus)
    {
        CsrSnprintf(deviceNameType, sizeof(deviceNameType), "[BR] %s [%s]", deviceName, AddrTable[typedAddr.type]);
    }
    else
#endif
    {
        CsrSnprintf(deviceNameType, sizeof(deviceNameType), "%s [%s]", deviceName, AddrTable[typedAddr.type]);
    }

    /* Decode the Class of Device bit mask, to a Icon and a sublabel            */
    /* codLabel   = AppCodDecodeDeviceClass(deviceClass, &codIcon);*/
    codLabel = CsrBtGapDeviceAddr2UnicodeString(typedAddr.addr, deviceStatus);

    /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI                                           */
    CsrBtGapAddItem(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI, CSR_UI_LAST, inst->devicesInSearchList,
                                                            codIcon,
                                                            CONVERT_TEXT_STRING_2_UCS2(deviceNameType),
                                                            codLabel);

    CsrPmemFree(deviceName);
}


void appAddDevicesToSearchingUI(CsrBtGapInstData * inst, CsrBtTypedAddr typedAddr,
                                CsrBtClassOfDevice deviceClass, CsrCharString *friendlyName,
                                CsrUint32 deviceStatus)
{
    CsrUint16          key;
    CsrBool            secLevel;

    if (CsrBtGapDeviceAddrPresentInKeyList(inst->searchList, typedAddr,
                                        &key, &secLevel))
    { /* The device is already present on the UI, just ignore it                           */
        ;
    }
    else
    {
        CsrBool authorised = FALSE;

        if(CSR_BT_SD_STATUS_TRUSTED & deviceStatus)
        {
            authorised = TRUE;
        }

        /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI on the display                                    */
        updateSearchingUi(inst, typedAddr, deviceClass, friendlyName, deviceStatus);

        /* Insert the deviceAddr and the key in the searchList                              */
        CsrBtGapInsertDeviceInKeyList(&(inst->searchList), typedAddr, friendlyName,
                                      deviceStatus, inst->devicesInSearchList, authorised);

        /* This is the first result doing this search procedure                             */
        if (inst->devicesInSearchList == 0)
        {
            /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI menu on the display                           */
            CsrBtGapMenuSet(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI, TEXT_SEARCHING_UCS2, TEXT_SELECT_UCS2, NULL);

            /* Show the CSR_BT_GAP_SD_SEARCH_DEVICES_UI on the display                                  */
            CsrBtGapShowUi(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

            /* Hide the CSR_BT_GAP_DEFAULT_INFO_UI                                      */
            CsrBtGapHideUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI);

        }
        inst->devicesInSearchList++;
    }
}

static void csrBtSdReadDeviceInfoCfmFunc
        (   void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode, /* the result of the read device info operation                                 */
            CsrBtDeviceAddr            deviceAddr,                 /* the bluetooth address of the device                                          */
            CsrBtClassOfDevice         deviceClass,                /* the class of device of the selected device                                   */
            CsrUint32                infoLen,                    /* the length of the data present in the info pointer                           */
            CsrUint8                 *info,                      /* additional information about the selected device                             */
            CsrUint32                deviceStatus)              /* the status of the device (non-paired/paired/which pairing)                   */
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) instData;
    CsrUint8 *friendlyName = NULL;

    if (supplier == CSR_BT_SUPPLIER_SD && resultCode == CSR_BT_RESULT_CODE_SD_SUCCESS)
    { /* The services were retrive with success, and at least one service has been
         added to the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI by the function
         BtManUpdateRemoteServicesUi                                                    */
         if(!AppUpdateRemoteServicesUi(&inst->csrUiVar, infoLen, info))
         {
            CsrBtGapMenuRemoveAllItems(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI);
            CsrBtGapAddItem(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI, CSR_UI_FIRST, CSR_BT_GAP_INVALID_KEY, CSR_UI_ICON_NONE,
                CONVERT_TEXT_STRING_2_UCS2("No services stored, try to read available services"), NULL);
         }

         CsrBtSdDecodeFriendlyName(info, infoLen, &friendlyName);

         CsrBtGapMenuSet(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI, CONVERT_TEXT_STRING_2_UCS2((char*) friendlyName), TEXT_OK_UCS2,NULL);

        /* Show the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI on the display                            */
        CsrBtGapShowUi(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
        CsrBtGapHideUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI);
    }
    else
    { /* The services could not be read or no services is available. Generate the pop-up
         UI that telling the user that the the services could not be read.              */

        /* Update CSR_BT_GAP_DEFAULT_INFO_UI                                          */
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_NO_DEVICES_FOUND_UCS2,
                                        NULL, TEXT_OK_UCS2, NULL);

        CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
    }
}

static void csrBtSdSearchResultIndFunc(
    void               *instData,              /* the applications own instance data                               */
    CsrBtDeviceAddr     deviceAddr,            /* the bluetooth address of the device                              */
    CsrBtAddressType    addressType,           /* the address type of the bluestooth address                                   */
    CsrBtClassOfDevice  deviceClass,           /* the class of device of the selected device                       */
    CsrUint8            rssi,                  /* the rssi level of the device                                     */
    CsrUint32           infoLen,               /* the length of the data present in the info pointer               */
    CsrUint8           *info,                  /* additional information about the selected device                 */
    CsrUint32           deviceStatus           /* the status of the device (non-paired/paired/which pairing)       */
    )
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) instData;

    if (inst->sdVar.state == CSR_BT_GAP_SD_STATE_SEARCHING)
    { /* The applicatoin is in the middle of a search procedure                           */

        CsrUint8 *friendlyName;
        CsrBtTypedAddr typedAddr;

        typedAddr.addr = deviceAddr;
        typedAddr.type = addressType;
        CsrBtSdDecodeFriendlyName(info, infoLen, &friendlyName);

        if ((inst->scVar.proximityPairing) && (inst->proximityPairingHandle != 0xFFFF))
        {
            CsrBool authorised = FALSE;

            if(CSR_BT_SD_STATUS_TRUSTED & deviceStatus)
            {
                authorised = TRUE;
            }

            /* Insert the deviceAddr and the key in the searchList                              */
            CsrBtGapInsertDeviceInKeyList(&(inst->searchList), typedAddr, (char *)friendlyName,
                                          deviceStatus, inst->devicesInSearchList, authorised);
            inst->devicesInSearchList++;
            if (inst->performBonding)
            {/* Start bonding! */
                CsrBtScBondReqSend(CsrSchedTaskQueueGet(), deviceAddr);
                inst->performBonding = FALSE;
            }
            else
            {/* Send confirm */
                CsrBtGapAppPairProximityDeviceCfm *prim = (CsrBtGapAppPairProximityDeviceCfm *)CsrPmemAlloc(sizeof(CsrBtGapAppPairProximityDeviceCfm));
            
                prim->type         = CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM;
                prim->deviceAddr   = deviceAddr;
                prim->friendlyName = CsrUtf8StrDup(friendlyName);
                prim->resultCode   = CSR_BT_GAP_APP_SUCCESS;

                CsrSchedMessagePut(inst->proximityPairingHandle, CSR_BT_GAP_APP_PRIM, prim);
                inst->proximityPairingHandle = 0xFFFF;
            }
            inst->sdVar.state = CSR_BT_GAP_SD_STATE_CANCEL_SEARCH;
            CsrBtSdCancelSearchReqSend(CsrSchedTaskQueueGet());

        }
        else
        {
            /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI, the UI which showing remote devices found the
            search/inquiry procedure                                                             */
            appAddDevicesToSearchingUI(inst, typedAddr, deviceClass, (CsrCharString *) friendlyName, deviceStatus);
        }
        CsrPmemFree(friendlyName);
    }
    else
    { /* The application is not in the middle of a search procedure. This signal must
         be crossing, e.g. the user has requested to cancel the search procedure, but the
         Service Discovery module has sent a CsrBtSdSearchResultInd just before it receives a
         CsrBtSdCancelSearchReq. Just ignore it                                                      */
        ;
    }
    CsrPmemFree(info);
    CSR_UNUSED(rssi);
}

static void csrBtSdCloseSearchIndFunc(
    void                    *instData,              /* the applications own instance data                               */
    CsrBtSupplier supplier, CsrBtResultCode resultCode                  /* reports the reason why the search has been stopped               */
    )
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) instData;

    switch (inst->sdVar.state)
    { /* Find out why the search procedure has stopped                                          */
        case CSR_BT_GAP_SD_STATE_SEARCHING :
        { /*  The search procedure has stopped, either because it has search for the time given
              by the applicatoin, or an error has occured doing the search                */

            if (inst->devicesInSearchList == 0)
            { /* No devices were found doing the search procedure, e.g current UI present is
                 CSR_BT_GAP_SD_STATE_SEARCHING                                                 */

                if ((inst->scVar.proximityPairing) && (inst->proximityPairingHandle != 0xFFFF))
                {
                    CsrBtGapAppPairProximityDeviceCfm *prim = (CsrBtGapAppPairProximityDeviceCfm *)CsrPmemAlloc(sizeof(CsrBtGapAppPairProximityDeviceCfm));
            
                    prim->type         = CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM;
                    CsrMemSet(&prim->deviceAddr, 0, sizeof(CsrBtDeviceAddr));
                    prim->friendlyName = NULL;
                    prim->resultCode   = CSR_BT_GAP_APP_NO_DEVICE_FOUND;

                    CsrSchedMessagePut(inst->proximityPairingHandle, CSR_BT_GAP_APP_PRIM, prim);
                    inst->proximityPairingHandle = 0xFFFF;
                    inst->devicesInSearchList = 0;
                    CsrBtGapRemoveDeviceInKeyList(&inst->searchList);
                    CsrBtGapMenuRemoveAllItems(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);
                }
                else
                {
                    CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SEARCH_UCS2,
                                                TEXT_SD_NO_DEVICES_FOUND_UCS2, TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                           */
                    CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
                }
            }
            else
            { /* At least one device were found doing the search procedure,
                 e.g current UI present is CSR_BT_GAP_SD_SEARCH_DEVICES_UI                                  */

                /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI menu on the display                           */
                CsrBtGapMenuSet(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI, TEXT_FOUND_DEVICES_UCS2,
                                                            TEXT_SELECT_UCS2, TEXT_SEARCH_UCS2);
            }
            inst->sdVar.state = CSR_BT_GAP_SD_STATE_IDLE;
            break;
        }
        case CSR_BT_GAP_SD_STATE_CANCEL_SEARCH :
        { /* The Search procedure has been cancel by the user                                   */
            if (inst->devicesInSearchList == 0)
            { /* No device is found, e.g current UI present is CSR_BT_GAP_DEFAULT_INFO_UI.
                 Hide CSR_BT_GAP_DEFAULT_INFO_UI                                            */
                CsrBtGapHideUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI);
            }
            else
            { /* At least one device is found doing the search procedure, e.g current UI present
                 is CSR_BT_GAP_SD_SEARCH_DEVICES_UI. Hide the CSR_BT_GAP_SD_SEARCH_DEVICES_UI                           */
                CsrBtGapHideUi(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);
            }
            inst->sdVar.state = CSR_BT_GAP_SD_STATE_IDLE;
            break;
        }
        case CSR_BT_GAP_SD_STATE_DEVICE_SELECT_DOING_SEARCH :
        { /*  The search procedure has stopped, either because it has search for the time given
              by the applicatoin, or an error has occured doing the search                */
            CsrUint16         * deviceName;

            /* Convert the friendly name of the remote device from utf8 to ucs2         */
            deviceName = CsrBtGapReturnDeviceNameWithDeviceAddr(inst->selectedDeviceAddr, inst->selectedDeviceName);

            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, NULL,
                                            deviceName, TEXT_OK_UCS2, NULL);

            CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

            CsrBtGapHideUi(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);

            inst->sdVar.state = CSR_BT_GAP_SD_STATE_IDLE;
            break;
        }
        default :
        {
            break;
        }
    }
    CSR_UNUSED(resultCode);
    CSR_UNUSED(supplier);
}

static void csrBtSdReadAvailableServicesCfmFunc
        (   void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode, /* the result of the read device info operation                                 */
            CsrBtDeviceAddr            deviceAddr,                 /* the bluetooth address of the device                                          */
            CsrBtClassOfDevice         deviceClass,                /* the class of device of the selected device                                   */
            CsrUint32                infoLen,                    /* the length of the data present in the info pointer                           */
            CsrUint8                 *info,                      /* additional information about the selected device                             */
            CsrUint32                deviceStatus)              /* the status of the device (non-paired/paired/which pairing)                   */
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) instData;
    CsrUint8 *friendlyName = NULL;

    if (supplier == CSR_BT_SUPPLIER_SD && resultCode == CSR_BT_RESULT_CODE_SD_SUCCESS && AppUpdateRemoteServicesUi(&inst->csrUiVar, infoLen, info))
    { /* The services were retrive with success, and at least one service has been
         added to the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI by the function
         BtManUpdateRemoteServicesUi                                                    */

         CsrBtSdDecodeFriendlyName(info, infoLen, &friendlyName);

         CsrBtGapMenuSet(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI, CONVERT_TEXT_STRING_2_UCS2((char*) friendlyName), TEXT_OK_UCS2,NULL);

        /* Show the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI on the display                            */
        CsrBtGapShowUi(inst, CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
        CsrBtGapHideUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI);
    }
    else
    { /* The services could not be read or no services is available. Generate the pop-up
         UI that telling the user that the the services could not be read.              */

        /* Update CSR_BT_GAP_DEFAULT_INFO_UI                                          */
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_READ_SERVICES_UCS2,
                                        CONVERT_TEXT_STRING_2_UCS2("Failed to find any useful services in the peer device"), TEXT_OK_UCS2, NULL);

        CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
    }
}


/*
Setup of the functions.
The table below shall be configured for each application that can receive
messages from the CSR_BT SD.
If some of the messages will never occur in the specific application the function reference
can be set to "NULL" in the table.
*/
static const struct CsrBtSdAppEventHandlerStructType csrBtSdAppEventFunctions =
{
    csrBtSdReadDeviceInfoCfmFunc,                    /* CSR_BT_SD_READ_DEVICE_INFO_CFM              */
    csrBtSdSearchResultIndFunc,                      /* CSR_BT_SD_SEARCH_RESULT_IND                 */
    csrBtSdCloseSearchIndFunc,                       /* CSR_BT_SD_CLOSE_SEARCH_IND                  */
    NULL,                                           /* CSR_BT_SD_ACTIVATE_SEARCH_AGENT_CFM         */
    NULL,                                           /* CSR_BT_SD_DEACTIVATE_SEARCH_AGENT_CFM       */
    csrBtSdReadAvailableServicesCfmFunc,             /* CSR_BT_SD_READ_AVAILABLE_SERVICES_CFM       */
    NULL,       /* CSR_BT_SD_CANCEL_READ_AVAILABLE_SERVICES_CFM*/
    NULL,                    /* CSR_BT_SD_READ_DEVICE_LIST_IND              */
    NULL,                    /* CSR_BT_SD_READ_DEVICE_LIST_CFM              */
};

/* setup the global functional pointer that shall be overloaded to the event function */
const CsrBtSdAppEventHandlerStructType *CsrBtGapCsrBtSdAppEventFunctionsPtr = &csrBtSdAppEventFunctions;

