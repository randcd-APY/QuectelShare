#ifndef CSR_BT_GAP_APP_SD_H__
#define CSR_BT_GAP_APP_SD_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_gap_app_sd_event_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const CsrBtSdAppEventHandlerStructType * CsrBtGapCsrBtSdAppEventFunctionsPtr;

#ifndef DEFAULT_PP_RSSI_LEVEL
#define DEFAULT_PP_RSSI_LEVEL          -40  /* RSSI threshold level to use for Proximity Pairing */
#endif

#ifndef DEFAULT_PP_LE_RSSI_LEVEL
#define DEFAULT_PP_LE_RSSI_LEVEL       -80  /* RSSI threshold for Low Energy */
#endif

#ifndef DEFAULT_PP_INQ_TX_POWER
#define DEFAULT_PP_INQ_TX_POWER        -70  /* Inquiry Transmit Power Level to use for Proximity Pairing */
#endif

/* app SD states */
#define CSR_BT_GAP_SD_STATE_IDLE                                        0x00
#define CSR_BT_GAP_SD_STATE_SEARCHING                                   0x01
#define CSR_BT_GAP_SD_STATE_CANCEL_SEARCH                               0x02
#define CSR_BT_GAP_SD_STATE_DEVICE_SELECT_DOING_SEARCH                  0x03
#define CSR_BT_GAP_SD_STATE_DEVICE_SELECTED                             0x04

/*number of 11 services are 32 from 0 to 31 and 15 from 32 to 63 */
#define NUM_OF_11_00_63_SEVICES                         (32 + 15)
#define NUM_OF_12_00_31_SEVICES                         (7)
#define NUM_OF_13_00_31_SEVICES                         (6)
#define CSR_BT_GAP_SERVICE_11                               (0x00001100)
#define CSR_BT_GAP_SERVICE_12                               (0x00001200)
#define CSR_BT_GAP_SERVICE_13                               (0x00001300)

/* bt_man icon defines for services knownServices11_00_31                   */
#define SPP_SERVICE_ICON                                CSR_UI_ICON_DEVICE
#define LAN_ACCESS_USING_PPP_SERVICE_ICON               CSR_UI_ICON_DEVICE
#define DIAL_UP_NETWOKING_SERVICE_ICON                  CSR_UI_ICON_DEVICE
#define IR_MC_SYNC_SERVICE_ICON                         CSR_UI_ICON_DEVICE
#define OPC_SERVICE_ICON                                CSR_UI_ICON_VCARD
#define FTC_SERVICE_ICON                                CSR_UI_ICON_FOLDER_TRANSFER
#define IR_MC_SYNC_COMMAND_SERVICE_ICON                 CSR_UI_ICON_DEVICE
#define HS_SERVICE_ICON                                 CSR_UI_ICON_HEADSET_EARPLUG
#define CORDLESS_TELEPHONY_SERVICE_ICON                 CSR_UI_ICON_DEVICE
#define AUDIO_SOURCE_SERVICE_ICON                       CSR_UI_ICON_HEADSET
#define AUDIO_SINK_SERVICE_ICON                         CSR_UI_ICON_HEADSET
#define AV_REMOTECONTROL_TARGET_SERVICE_ICON            CSR_UI_ICON_REMOTECONTROL
#define ADVANCED_AUDIO_DIST_SERVICE_ICON                CSR_UI_ICON_DEVICE
#define AV_REMOTE_CONTROL_SERVICE_ICON                  CSR_UI_ICON_REMOTECONTROL
#define VIDEO_CONFERENCING_SERVICE_ICON                 CSR_UI_ICON_DEVICE
#define INTERCOM_SERVICE_ICON                           CSR_UI_ICON_DEVICE
#define FAX_SERVICE_ICON                                CSR_UI_ICON_DEVICE
#define HEADSET_AUDIO_GATEWAY_SERVICE_ICON              CSR_UI_ICON_HEADSET_EARPLUG
#define WAP_SERVICE_ICON                                CSR_UI_ICON_DEVICE
#define WAP_CLIENT_SERVICE_ICON                         CSR_UI_ICON_DEVICE
#define PANU_SERVICE_ICON                               CSR_UI_ICON_DEVICE
#define NAP_SERVICE_ICON                                CSR_UI_ICON_DEVICE
#define GN_SERVICE_ICON                                 CSR_UI_ICON_DEVICE
#define DIRECT_PRINTING_SERVICE_ICON                    CSR_UI_ICON_PRINTER
#define REFERENCE_PRINTING_SERVICE_ICON                 CSR_UI_ICON_PRINTER
#define IMAGING_SERVICE_ICON                            CSR_UI_ICON_CAMERA
#define IMAGING_RESPONDER_SERVICE_ICON                  CSR_UI_ICON_CAMERA
#define IMAGING_AUTOMATIC_ARCHIVE_SERVICE_ICON          CSR_UI_ICON_CAMERA
#define IMAGING_REFERENCE_OBJECTS_SERVICE_ICON          CSR_UI_ICON_CAMERA
#define HF_SERVICE_ICON                                 CSR_UI_ICON_HEADSET_EARPLUG
#define HANDSFREE_AUDIO_GATEWAY_SERVICE_ICON            CSR_UI_ICON_HEADSET_EARPLUG
#define DIRECT_PRINT_REF_OBJ_SERVICE_ICON               CSR_UI_ICON_PRINTER

/* bt_man icon defines for services knownServices11_32_63                   */
#define REFLECTED_UI_ICON                               CSR_UI_ICON_DEVICE
#define BASIC_PRINTING_ICON                             CSR_UI_ICON_PRINTER
#define PRINTING_STATUS_ICON                            CSR_UI_ICON_PRINTER
#define HUMAN_INTERFACE_DEVICE_ICON                     CSR_UI_ICON_DEVICE
#define HCRP_ICON                                       CSR_UI_ICON_PRINTER
#define HCR_PRINT_SERVICE_ICON                          CSR_UI_ICON_PRINTER
#define HCR_SCAN_SERVICE_ICON                           CSR_UI_ICON_SCANNER
#define COMMON_ISDN_ACCESS_SERVICE_ICON                 CSR_UI_ICON_DEVICE
#define VIDEO_CONFERENCING_GW_ICON                      CSR_UI_ICON_DEVICE
#define UDI_MT_SERVICE_ICON                             CSR_UI_ICON_DEVICE
#define UDI_TA_SERVICE_ICON                             CSR_UI_ICON_DEVICE
#define AUDIO_VIDEO_SERVICE_ICON                        CSR_UI_ICON_CAMCORDER
#define SIM_ACCESS_SERVICE_ICON                         CSR_UI_ICON_DEVICE
#define PHONEBOOK_ACCESS_PCE_ICON                       CSR_UI_ICON_DEVICE
#define PHONEBOOK_ACCESS_PSE_ICON                       CSR_UI_ICON_DEVICE

/* bt_man icon defines for services knownServices12_00_31                   */
#define PNP_INFORMATION_ICON                            CSR_UI_ICON_DEVICE
#define GENERIC_NETWORKING_ICON                         CSR_UI_ICON_DEVICE
#define GENERIC_FILE_TRANSFER_ICON                      CSR_UI_ICON_DEVICE
#define GENERIC_AUDIO_ICON                              CSR_UI_ICON_DEVICE
#define GENERIC_TELEPHONY_ICON                          CSR_UI_ICON_DEVICE
#define UPNP_SERVICE_ICON                               CSR_UI_ICON_DEVICE
#define UPNP_IP_SERVICE_ICON                            CSR_UI_ICON_DEVICE

/* bt_man icon defines for services knownServices13_00_31                   */
#define ESDP_UPNP_IP_PAN_ICON                           CSR_UI_ICON_DEVICE
#define ESDP_UPNP_IP_LAP_ICON                           CSR_UI_ICON_DEVICE
#define ESDP_UPNP_L2CAP_ICON                            CSR_UI_ICON_DEVICE
#define VIDEO_SOURCE_ICON                               CSR_UI_ICON_DEVICE
#define VIDEO_SINK_ICON                                 CSR_UI_ICON_DEVICE
#define VIDEO_DISTRIBUTION_ICON                         CSR_UI_ICON_DEVICE

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GAP_APP_SD_H__ */
