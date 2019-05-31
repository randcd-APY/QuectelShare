#ifndef CSR_BT_PROFILES_H__
#define CSR_BT_PROFILES_H__

/****************************************************************************

Copyright (c) 2001-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_mblk.h"
#include "bluetooth.h"
#include "l2cap_prim.h"
#include "csr_bt_usr_config.h"
#include "csr_bt_addr.h"

/* Ensure backwards compatibility with old csr_usr_config.h */
#ifndef CSR_BT_DEFAULT_LOW_POWER_MODES
#define CSR_BT_DEFAULT_LOW_POWER_MODES ((link_policy_settings_t)ENABLE_SNIFF)
#endif

#include "csr_bt_internet_primitives.h"
#include "hci_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************
 * MTU sizes
 ************************************************************************************/
#define CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE           0

/*************************************************************************************
                        Security level defines
************************************************************************************/
#define CSR_BT_SEC_NONE                           ((CsrUint16) 0x0000) /* No security */
#define CSR_BT_SEC_DEFAULT                        ((CsrUint16) 0x0001) /* Use default values as specified in csr_bt_usr_config.h */
#define CSR_BT_SEC_MANDATORY                      ((CsrUint16) 0x0002) /* Use profile mandatory settings */
#define CSR_BT_SEC_SPECIFY                        ((CsrUint16) 0x0004) /* Use specified security values */
#define CSR_BT_SEC_AUTHORISATION                  ((CsrUint16) 0x0010) /* Authorisation on incoming connections */
#define CSR_BT_SEC_AUTHENTICATION                 ((CsrUint16) 0x0020) /* Authentication on connections */
#define CSR_BT_SEC_ENCRYPTION                     ((CsrUint16) 0x0040) /* Encryption on connections */
#define CSR_BT_SEC_MITM                           ((CsrUint16) 0x0080) /* MITM protection on connections */
#define CSR_BT_SEC_SC                             ((CsrUint16) 0x0100) /* Secure connections */

/*************************************************************************************
                        SC Idle mode define
************************************************************************************/
#define CSR_BT_SEC_MODE_PAIRABLE                  ((CsrUint8) 0x00)
#define CSR_BT_SEC_MODE_NON_PAIRABLE              ((CsrUint8) 0x01)
#define CSR_BT_SEC_MODE_NON_BONDABLE              ((CsrUint8) 0x02)
#define CSR_BT_SEC_MODE_MAX                       ((CsrUint8) 0x03)

/*************************************************************************************
                        SC Event masks
************************************************************************************/
typedef CsrUint32                                  CsrBtScEventMask;
#define CSR_BT_SC_EVENT_MASK_NONE                  ((CsrUint32) 0x00000000)
#define CSR_BT_SC_EVENT_MASK_REBOND                ((CsrUint32) 0x00000001)
#define CSR_BT_SC_EVENT_MASK_PAIR                  ((CsrUint32) 0x00000002)
#define CSR_BT_SC_EVENT_MASK_AUTHORISE_CANCEL      ((CsrUint32) 0x00000004)
#define CSR_BT_SC_EVENT_MASK_AUTHORISE_EXTENDED    ((CsrUint32) 0x00000008)
#define CSR_BT_SC_EVENT_MASK_ADDRESS_MAPPED_IND    ((CsrUint32) 0x00000010)

#define CSR_BT_SC_EVENT_MASK_ALL                   ((CsrUint32) 0x0000001F)

#define CSR_BT_SC_PROTOCOL_RFCOMM                   SEC_PROTOCOL_RFCOMM
#define CSR_BT_SC_PROTOCOL_L2CAP                    SEC_PROTOCOL_L2CAP

/*************************************************************************************
                        Profile layer defines
************************************************************************************/
#define CSR_BT_DEVICE_ADDR_LEN                    (CsrUint8) (18)      /*  number of chars allocated for the BT device addr */
#define CSR_BT_PASSKEY_MAX_LEN                    HCI_MAX_PIN_LENGTH

/* Common profiles mask */
#define CSR_BT_CORDLESS_TELEPHONY_PROFILE                              (0x400200)
#define CSR_BT_INTERCOM_PROFILE                                        (0x400200)
#define CSR_BT_HEADSET_PROFILE                                         (0x200404)
#define CSR_BT_DIAL_UP_NETWORKING_PROFILE                              (0x400200)
#define CSR_BT_FAX_PROFILE                                             (0x400200)
#define CSR_BT_HANDS_FREE_PROFILE                                      (0x200408)
#define CSR_BT_PAN_PROFILE                                             (0x020000)

/* Major Service Mask */
#define CSR_BT_LIMITED_DISCOVERABLE_MODE_MAJOR_SERVICE_MASK            (0x002000)
#define CSR_BT_POSITIONING_MAJOR_SERVICE_MASK                          (0x010000)
#define CSR_BT_NETWORKING_MAJOR_SERVICE_MASK                           (0x020000)
#define CSR_BT_RENDERING_MAJOR_SERVICE_MASK                            (0x040000)
#define CSR_BT_CAPTURING_MAJOR_SERVICE_MASK                            (0x080000)
#define CSR_BT_OBJECT_TRANSFER_MAJOR_SERVICE_MASK                      (0x100000)
#define CSR_BT_AUDIO_MAJOR_SERVICE_MASK                                (0x200000)
#define CSR_BT_TELEPHONY_MAJOR_SERVICE_MASK                            (0x400000)
#define CSR_BT_INFORMATION_MAJOR_SERVICE_MASK                          (0x800000)
#define CSR_BT_SIGNIFICANT_MAJOR_SERVICE_MASK                          (0xFE2000)

/* Major Device Mask */
#define CSR_BT_MISCELLANEOUS_MAJOR_DEVICE_MASK                         (0x000000)
#define CSR_BT_COMPUTER_MAJOR_DEVICE_MASK                              (0x000100)
#define CSR_BT_PHONE_MAJOR_DEVICE_MASK                                 (0x000200)
#define CSR_BT_LAN_MAJOR_DEVICE_MASK                                   (0x000300)
#define CSR_BT_AV_MAJOR_DEVICE_MASK                                    (0x000400)
#define CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK                            (0x000500)
#define CSR_BT_IMAGING_MAJOR_DEVICE_MASK                               (0x000600)
#define CSR_BT_WEARABLE_MAJOR_DEVICE_MASK                              (0x000700)
#define CSR_BT_TOY_MAJOR_DEVICE_MASK                                   (0x000800)
#define CSR_BT_HEALTH_MAJOR_DEVICE_MASK                                (0x000900)
#define CSR_BT_UNCLASSIFIED_MAJOR_DEVICE_MASK                          (0x001F00)
#define CSR_BT_SIGNIFICANT_MAJOR_DEVICE_MASK                           (0x001F00)

/* Minor Device Mask under computer major class */
#define CSR_BT_DESKTOP_WORKSTATION_MINOR_DEVICE_MASK                   (0x000004)
#define CSR_BT_SERVER_CLASS_COMPUTER_MINOR_DEVICE_MASK                 (0x000008)
#define CSR_BT_LAPTOP_MINOR_DEVICE_MASK                                (0x00000C)
#define CSR_BT_HANDHELD_PC_PDA_MINOR_DEVICE_MASK                       (0x000010)
#define CSR_BT_PALM_SIZED_PC_PDA_MINOR_DEVICE_MASK                     (0x000014)
#define CSR_BT_WEARABLE_COMPUTER_MINOR_DEVICE_MASK                     (0x000018)
#define CSR_BT_UNCLASSIFIED_COMPUTER_MINOR_DEVICE_MASK                 (0x000000)

/* Minor Device Mask under phone major class */
#define CSR_BT_CELLULAR_MINOR_DEVICE_MASK                              (0x000004)
#define CSR_BT_CORDLESS_MINOR_DEVICE_MASK                              (0x000008)
#define CSR_BT_SMART_PHONE_MINOR_DEVICE_MASK                           (0x00000C)
#define CSR_BT_WIRED_MODEM_OR_VOICE_GATEWAY_MINOR_DEVICE_MASK          (0x000010)
#define CSR_BT_COMMON_ISDN_ACCESS_MINOR_DEVICE_MASK                    (0x000014)
#define CSR_BT_UNCLASSIFIED_PHONE_MINOR_DEVICE_MASK                    (0x000000)

/* Minor Device Mask under audio/video major class */
#define CSR_BT_UNCATEGORIZED_AV_MINOR_DEVICE_MASK                      (0x000000)
#define CSR_BT_WEARABLE_HEADSET_AV_MINOR_DEVICE_CLASS                  (0x000004)
#define CSR_BT_HANDSFREE_AV_MINOR_DEVICE_CLASS                         (0x000008)
#define CSR_BT_RESERVED_AV_MINOR_DEVICE_CLASS                          (0x00000C)
#define CSR_BT_MICROPHONE_AV_MINOR_DEVICE_CLASS                        (0x000010)
#define CSR_BT_LOUDSPEAKER_AV_MINOR_DEVICE_CLASS                       (0x000014)
#define CSR_BT_HEADPHONES_AV_MINOR_DEVICE_CLASS                        (0x000018)
#define CSR_BT_PORTABLE_AUDIO_AV_MINOR_DEVICE_CLASS                    (0x00001C)
#define CSR_BT_CAR_AUDIO_AV_MINOR_DEVICE_CLASS                         (0x000020)
#define CSR_BT_SETTOP_BOX_AV_MINOR_DEVICE_CLASS                        (0x000024)
#define CSR_BT_HIFI_AUDIO_AV_MINOR_DEVICE_CLASS                        (0x000028)
#define CSR_BT_VCR_AV_MINOR_DEVICE_CLASS                               (0x00002C)
#define CSR_BT_VIDEO_CAMERA_AV_MINOR_DEVICE_CLASS                      (0x000030)
#define CSR_BT_CAMCORDER_AV_MINOR_DEVICE_CLASS                         (0x000034)
#define CSR_BT_VIDEO_MONITOR_AV_MINOR_DEVICE_CLASS                     (0x000038)
#define CSR_BT_VIDEO_DISPLAY_AND_LOUDSPEAKER_AV_MINOR_DEVICE_CLASS     (0x00003C)
#define CSR_BT_VIDEO_CONFERENCING_AV_MINOR_DEVICE_CLASS                (0x000040)
#define CSR_BT_RESERVED1_AV_MINOR_DEVICE_CLASS                         (0x000044)
#define CSR_BT_GAME_TOY_AV_MINOR_DEVICE_CLASS                          (0x000048)

#define CSR_BT_HANDS_FREE_MINOR_DEVICE_MASK                            (0x000008)
/* Minor Device Mask under LAN major class */
#define CSR_BT_FULLY_AVAILABLE_MINOR_DEVICE_MASK                       (0x000000)
#define CSR_BT_ONE_TO_SEVENTEEN_PERCENT_MINOR_DEVICE_MASK              (0x000020)
#define CSR_BT_SEVENTEEN_TO_THIRTYTHREE_PERCENT_MINOR_DEVICE_MASK      (0x000040)
#define CSR_BT_THIRTYTHREE_TO_FIFTY_PERCENT_MINOR_DEVICE_MASK          (0x000060)
#define CSR_BT_FIFTY_TO_SIXTYSEVEN_PERCENT_MINOR_DEVICE_MASK           (0x000080)
#define CSR_BT_SIXTYSEVEN_TO_EIGHTYTHREE_PERCENT_MINOR_DEVICE_MASK     (0x0000A0)
#define CSR_BT_EIGHTYTHREE_TO_NINETYNINE_PERCENT_MINOR_DEVICE_MASK     (0x0000C0)
#define CSR_BT_NO_SERVICE_AVAILABLE_MINOR_DEVICE_MASK                  (0x0000E0)
#define CSR_BT_UNCLASSIFIED_LAN_MINOR_DEVICE_MASK                      (0x000000)

/* Minor Device Mask under Peripheral major Class (Keyboard/pointing device filed) */
#define CSR_BT_UNCATEGORISED_PERIPHERALS_MINOR_DEVICE_MASK             (0x000000)
#define CSR_BT_JOYSTICK_PERIPHERALS_MINOR_DEVICE_CLASS                 (0x000004)
#define CSR_BT_GAMEPAD_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS            (0x000008)
#define CSR_BT_REMOTE_CONTROL_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS     (0x00000C)
#define CSR_BT_SENSING_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS            (0x000010)
#define CSR_BT_DIGITIZER_TABLET_PERIPHERAL_MINOR_DEVICE_CLASS          (0x000014)
#define CSR_BT_CARD_READER_PERIPHERAL_MINOR_DEVICE_CLASS               (0x000018)

/* Minor Device Mask under Imaging major class */
#define CSR_BT_DISPLAY_IMAGING_MINOR_DEVICE_MASK                       (0x000010)
#define CSR_BT_CAMERA_IMAGING_MINOR_DEVICE_MASK                        (0x000020)
#define CSR_BT_SCANNER_IMAGING_MINOR_DEVICE_MASK                       (0x000040)
#define CSR_BT_PRINTER_IMAGING_MINOR_DEVICE_MASK                       (0x000080)

/* Minor Device Mask under Wearable major class */
#define CSR_BT_WRIST_WATCH_WEARABLE_MINOR_DEVICE_MASK                  (0x000004)
#define CSR_BT_PAGER_WEARABLE_MINOR_DEVICE_MASK                        (0x000008)
#define CSR_BT_JACKET_WEARABLE_MINOR_DEVICE_MASK                       (0x00000C)
#define CSR_BT_HELMET_WEARABLE_MINOR_DEVICE_MASK                       (0x000010)
#define CSR_BT_GLASSES_WATCH_WEARABLE_MINOR_DEVICE_MASK                (0x000014)

/* Minor Device Mask under Toy major class */
#define CSR_BT_ROBOT_TOY_MINOR_DEVICE_MASK                             (0x000004)
#define CSR_BT_VEHICLE_TOY_MINOR_DEVICE_MASK                           (0x000008)
#define CSR_BT_DOLL_ACTION_FIGURE_TOY_MINOR_DEVICE_MASK                (0x00000C)
#define CSR_BT_CONTROLLER_TOY_MINOR_DEVICE_MASK                        (0x000010)
#define CSR_BT_GAME_TOY_MINOR_DEVICE_MASK                              (0x000014)

/* Minor Device Mask under Health major class */
#define CSR_BT_UNDEFINED_HEALTH_MINOR_DEVICE_CLASS                     (0x000000)
#define CSR_BT_BPM_HEALTH_MINOR_DEVICE_CLASS                           (0x000004)
#define CSR_BT_THERMOMETER_HEALTH_MINOR_DEVICE_CLASS                   (0x000008)
#define CSR_BT_WEIGHING_SCALE_HEALTH_MINOR_DEVICE_CLASS                (0x00000C)
#define CSR_BT_GLUCOSE_METER_HEALTH_MINOR_DEVICE_CLASS                 (0x000010)
#define CSR_BT_PULSE_OXIM_HEALTH_MINOR_DEVICE_CLASS                    (0x000014)
#define CSR_BT_HRM_HEALTH_MINOR_DEVICE_CLASS                           (0x000018)
#define CSR_BT_DATA_DISPLAY_HEALTH_MINOR_DEVICE_CLASS                  (0x00001C)
#define CSR_BT_STEP_COUNTER_HEALTH_MINOR_DEVICE_CLASS                  (0x000020)

/* Page/Inquiry Scan defines */
#define CSR_BT_HCI_SCAN_ENABLE_OFF            HCI_SCAN_ENABLE_OFF          /* Default: No Page/Inquiry Scan */
#define CSR_BT_HCI_SCAN_ENABLE_INQ            HCI_SCAN_ENABLE_INQ          /* Inquiry Scan Only */
#define CSR_BT_HCI_SCAN_ENABLE_PAGE           HCI_SCAN_ENABLE_PAGE         /* Page Scan Only */
#define CSR_BT_HCI_SCAN_ENABLE_INQ_AND_PAGE   HCI_SCAN_ENABLE_INQ_AND_PAGE /* Page And Inquiry Scan */

/*************************************************************************************
                        Mandatory security settings
************************************************************************************/
/* Mandatory security levels for each profile, incoming connections */
#define CSR_BT_DUN_GW_MANDATORY_SECURITY_INCOMING                      (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_FAX_GW_MANDATORY_SECURITY_INCOMING                      (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_HANDSFREE_MANDATORY_SECURITY_INCOMING                   (CSR_BT_SEC_NONE)
#define CSR_BT_HANDSFREE_GW_MANDATORY_SECURITY_INCOMING                (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_IMAGING_RESPONDER_MANDATORY_SECURITY_INCOMING      (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_OBJECT_PUSH_MANDATORY_SECURITY_INCOMING            (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_IR_MC_SYNC_MANDATORY_SECURITY_INCOMING             (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_OBEX_MESSAGE_ACCESS_MANDATORY_SECURITY_INCOMING         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION | CSR_BT_SEC_MITM)
#define CSR_BT_OBEX_FILE_TRANSFER_MANDATORY_SECURITY_INCOMING          (CSR_BT_SEC_NONE)
#define CSR_BT_SIM_ACCESS_MANDATORY_SECURITY_INCOMING                  (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION | CSR_BT_SEC_MITM)
#define CSR_BT_SERIAL_PORT_MANDATORY_SECURITY_INCOMING                 (CSR_BT_SEC_NONE)
#define CSR_BT_PBAP_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_BPPS_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_NONE)
#define CSR_BT_AV_MANDATORY_SECURITY_INCOMING                          (CSR_BT_SEC_NONE)
#define CSR_BT_AV_RCP_MANDATORY_SECURITY_INCOMING                      (CSR_BT_SEC_NONE)
#define CSR_BT_HIDH_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_HIDD_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_CTP_MANDATORY_SECURITY_INCOMING                         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_ICP_MANDATORY_SECURITY_INCOMING                         (CSR_BT_SEC_NONE)
#define CSR_BT_PAN_MANDATORY_SECURITY_INCOMING                         (CSR_BT_SEC_NONE)
#define CSR_BT_HCRP_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_SYNCML_TRANSFER_MANDATORY_SECURITY_INCOMING        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_MCAP_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_NONE)
#define CSR_BT_HDP_MANDATORY_SECURITY_INCOMING                         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_GATT_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_GNSS_MANDATORY_SECURITY_INCOMING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)


/* Mandatory security levels for each profile, outgoing connections */
#define CSR_BT_HANDSFREE_MANDATORY_SECURITY_OUTGOING                   (CSR_BT_SEC_NONE)
#define CSR_BT_HANDSFREE_GW_MANDATORY_SECURITY_OUTGOING                (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_FILE_TRANSFER_MANDATORY_SECURITY_OUTGOING          (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_IR_MC_SYNC_MANDATORY_SECURITY_OUTGOING             (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_MESSAGE_ACCESS_MANDATORY_SECURITY_OUTGOING         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION | CSR_BT_SEC_MITM)
#define CSR_BT_OBEX_OBJECT_PUSH_MANDATORY_SECURITY_OUTGOING            (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_IMAGING_INITIATOR_MANDATORY_SECURITY_OUTGOING      (CSR_BT_SEC_NONE)
#define CSR_BT_OBEX_PRINTING_INITIATOR_MANDATORY_SECURITY_OUTGOING     (CSR_BT_SEC_NONE)
#define CSR_BT_SIM_ACCESS_MANDATORY_SECURITY_OUTGOING                  (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION | CSR_BT_SEC_MITM)
#define CSR_BT_SERIAL_PORT_MANDATORY_SECURITY_OUTGOING                 (CSR_BT_SEC_NONE)
#define CSR_BT_AV_MANDATORY_SECURITY_OUTGOING                          (CSR_BT_SEC_NONE)
#define CSR_BT_AV_RCP_MANDATORY_SECURITY_OUTGOING                      (CSR_BT_SEC_NONE)
#define CSR_BT_HIDH_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_HIDD_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_CTP_MANDATORY_SECURITY_OUTGOING                         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_ICP_MANDATORY_SECURITY_OUTGOING                         (CSR_BT_SEC_NONE)
#define CSR_BT_PAN_MANDATORY_SECURITY_OUTGOING                         (CSR_BT_SEC_NONE)
#define CSR_BT_PBAP_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_DUN_CLIENT_MANDATORY_SECURITY_OUTGOING                  (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_OBEX_SYNCML_TRANSFER_MANDATORY_SECURITY_OUTGOING        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_MCAP_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_NONE)
#define CSR_BT_HDP_MANDATORY_SECURITY_OUTGOING                         (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_GATT_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_AUTHENTICATION | CSR_BT_SEC_ENCRYPTION)
#define CSR_BT_GNSS_MANDATORY_SECURITY_OUTGOING                        (CSR_BT_SEC_NONE)



/*************************************************************************************
                        Primitive segmentation
************************************************************************************/
#define CSR_BT_CPL_PRIM_BASE                      (0x0100)

#define CSR_BT_DG_PRIM                            (0x0100)
#define CSR_BT_DUNC_PRIM                          (0x0101)
#define CSR_BT_SC_PRIM                            (0x0102)
#define CSR_BT_CM_PRIM                            (0x0104)
#define CSR_BT_SDS_PRIM                           (0x0105)
#define CSR_BT_OPS_PRIM                           (0x0106)
#define CSR_BT_OPC_PRIM                           (0x0107)
#define CSR_BT_AT_PRIM                            (0x0108)
#define CSR_BT_IWU_PRIM                           (0x0109)
#define CSR_BT_SYNCS_PRIM                         (0x010A)
#define CSR_BT_SPP_PRIM                           (0x010B)
#define CSR_BT_FAX_PRIM                           (0x010C)
#define CSR_BT_HFG_PRIM                           (0x010D)
#define CSR_BT_HF_PRIM                            (0x010E)
#define CSR_BT_FTS_PRIM                           (0x0110)
#define CSR_BT_BNEP_PRIM                          (0x0111)
#define CSR_BT_BSL_PRIM                           (0x0112)
#define CSR_BT_BIPS_PRIM                          (0x0113)
#define CSR_BT_BIPC_PRIM                          (0x0114)
#define CSR_BT_FTC_PRIM                           (0x0115)
#define CSR_BT_SMLC_PRIM                          (0x0116)
#define CSR_BT_SMLS_PRIM                          (0x0117)
#define CSR_BT_PPP_PRIM                           (0x0118)
#define CSR_BT_BPPC_PRIM                          (0x0119)
#define CSR_BT_AV_PRIM                            (0x011A)
#define CSR_BT_AVRCP_PRIM                         (0x011B)
#define CSR_BT_SAPS_PRIM                          (0x011C)
#define CSR_BT_SAPC_PRIM                          (0x011D)
#define CSR_BT_SD_PRIM                            (0x011E)
#define CSR_BT_HIDH_PRIM                          (0x011F)
#define CSR_BT_HIDD_PRIM                          (0x0120)
#define CSR_BT_PAS_PRIM                           (0x0121)
#define CSR_BT_PAC_PRIM                           (0x0122)
#define CSR_BT_BPPS_PRIM                          (0x0123)
#define CSR_BT_LSL_PRIM                           (0x0124)
#define CSR_BT_HCRP_PRIM                          (0x0125)
#define CSR_BT_JSR82_PRIM                         (0x0126)
#define CSR_BT_SDC_PRIM                           (0x0128)
#define CSR_BT_SYNCC_PRIM                         (0x0129)
#define CSR_BT_MCAP_PRIM                          (0x012A)
#define CSR_BT_HDP_PRIM                           (0x012B)
#define CSR_BT_MAPC_PRIM                          (0x012C)
#define CSR_BT_MAPS_PRIM                          (0x012D)
#define CSR_BT_AMPM_PRIM                          (0x012E)
#define CSR_BT_PHDC_MGR_PRIM                      (0x012F)
#define CSR_BT_PHDC_AG_PRIM                       (0x0130)
#define CSR_BT_GATT_PRIM                          (0x0131)
#define CSR_BT_PROX_SRV_PRIM                      (0x0132)
#define CSR_BT_THERM_SRV_PRIM                     (0x0133)
#define CSR_BT_GENERIC_SRV_PRIM                   (0x0134)
#define CSR_BT_GNSS_CLIENT_PRIM                   (0x0135)
#define CSR_BT_GNSS_SERVER_PRIM                   (0x0136)
#define CSR_BT_ASM_PRIM                           (0x0137)
#define CSR_BT_AVRCP_IMAGING_PRIM                 (0x0138)
#define CSR_BT_HOGH_PRIM                          (0x0139)
#define CSR_BT_NUMBER_OF_CSR_BT_EVENTS            (0x0139 - CSR_BT_CPL_PRIM_BASE + 1)

#define CSR_BT_HCI_PRIM HCI_PRIM
#define CSR_BT_DM_PRIM DM_PRIM
#define CSR_BT_L2CAP_PRIM L2CAP_PRIM
#define CSR_BT_RFCOMM_PRIM RFCOMM_PRIM
#define CSR_BT_SDP_PRIM SDP_PRIM
#define CSR_BT_ATT_PRIM ATT_PRIM
#define SDS_PRIM CSR_BT_SDS_PRIM
#define SDC_PRIM CSR_BT_SDC_PRIM

/* CSR_BT_USER_PRIM used by 'user' for e.g. testing purposes */
#define CSR_BT_USER_PRIM                          (0x1000)

/* AMP controller ID */
typedef l2ca_controller_t CsrBtAmpController;
#define CSR_BT_AMP_CONTROLLER_BREDR               (L2CA_AMP_CONTROLLER_BT)
#define CSR_BT_AMP_CONTROLLER_UNKNOWN             (L2CA_AMP_CONTROLLER_UNKNOWN)

/* Commonly used types in CPL */
typedef BD_ADDR_T                          CsrBtDeviceAddr;
typedef unsigned char                      CsrBtDeviceLinkkey[SIZE_LINK_KEY];
typedef CsrUint32                          CsrBtClassOfDevice;
typedef CsrUint16                          CsrBtCplTimer;
typedef CsrUint8                           CsrBtMuxId;

/* Connection identifier */
typedef CsrUint32                          CsrBtConnId;
#define CSR_BT_CONN_ID_INVALID             ((CsrBtConnId) 0x00000000)
#define CSR_BT_CONN_ID_GET_MASK            ((CsrBtConnId) 0x0000FFFF)
#define CSR_BT_CONN_ID_L2CAP_TECH_MASK     ((CsrBtConnId) 0x00010000)
#define CSR_BT_CONN_ID_RFCOMM_TECH_MASK    ((CsrBtConnId) 0x00020000)
#define CSR_BT_CONN_ID_LE_TECH_MASK        ((CsrBtConnId) 0x00040000)
#define CSR_BT_CONN_ID_LE_SECURITY         ((CsrBtConnId) 0x00080000)
#define CSR_BT_CONN_ID_CANCELLED           ((CsrBtConnId) 0x80000000)

#define CSR_BT_CONN_ID_IS_RFC(c)           ((c) & CSR_BT_CONN_ID_RFCOMM_TECH_MASK)
#define CSR_BT_CONN_ID_IS_L2CA(c)          ((c) & CSR_BT_CONN_ID_L2CAP_TECH_MASK)
#define CSR_BT_CONN_ID_IS_LE(c)            ((c) & CSR_BT_CONN_ID_LE_TECH_MASK)
#define CSR_BT_CONN_ID_IS_LE_SECURITY(c)   ((c) & CSR_BT_CONN_ID_LE_SECURITY)

/* PAN roles */
typedef CsrUint16                           CsrBtBslPanRole;
#define CSR_BT_BSL_NO_ROLE                 ((CsrBtBslPanRole) (0x00) )
#define CSR_BT_BSL_NAP_ROLE                ((CsrBtBslPanRole) (0x01) )
#define CSR_BT_BSL_GN_ROLE                 ((CsrBtBslPanRole) (0x02) )
#define CSR_BT_BSL_PANU_ROLE               ((CsrBtBslPanRole) (0x04) )

/* UUID types - 16 bit UUID is defined in bluetooth.h */
typedef uuid16_t                            CsrBtUuid16;
typedef CsrUint32                           CsrBtUuid32;
typedef CsrUint8                            CsrBtUuid128[16];

/* Type/length of combined UUID */
typedef CsrUint8                            CsrBtUuidSize;
#define CSR_BT_UUID16_SIZE                  ((CsrBtUuidSize)2)
#define CSR_BT_UUID32_SIZE                  ((CsrBtUuidSize)4)
#define CSR_BT_UUID128_SIZE                 ((CsrBtUuidSize)16)

/* Flexible UUID type */
typedef struct
{
    CsrBtUuidSize   length;          /* no. valid bytes in the UUID, use CSR_BT_UUIDxx_SIZE */
    CsrBtUuid128    uuid;            /* little endian, ie LSB in [0], MSB in [15] */
} CsrBtUuid;

#define CSR_BT_UUID_GET_16(x)        ((CsrUint16)((x).uuid[0]|((x).uuid[1]<<8)))
#define CSR_BT_UUID_GET_32(x)        ((CsrUint32)((x).uuid[0]|((x).uuid[1]<<8)|((x).uuid[2]<<16)|((x).uuid[3]<<24)))

typedef struct
{
    CsrUint16 max_interval;          /* Max sniff interval */
    CsrUint16 min_interval;          /* Min sniff interval */
    CsrUint16 attempt;               /* Sniff attempt */
    CsrUint16 timeout;               /* Sniff timeout */
} CsrBtSniffSettings;

/* Park state is deprecated, this structure is still provided to avoid
 * the API break with the existing API's */
typedef struct
{
    CsrUint16 max_interval;          /* Max park interval */
    CsrUint16 min_interval;          /* Min park interval */

    /* The amount of time for which the ACL link should be idle before
     * being parked. DM will try to keep the link active until there has
     * been no ACL data in either direction for this amount of time.
    */
    CsrUint32 park_idle_time;        /* Microseconds */
} CsrBtParkSettings;

/* L2CAP psm channels for different services */
#define CSR_BT_ICP_PSM                            ((psm_t) 0x05)
#define CSR_BT_CTP_PSM                            ((psm_t) 0x07)
#define CSR_BT_PAN_BNEP_PSM                       ((psm_t) 0x0F)
#define CSR_BT_HID_CTRL_PSM                       ((psm_t) 0x11)
#define CSR_BT_HID_INTR_PSM                       ((psm_t) 0x13)
#define CSR_BT_AVCTP_PSM                          ((psm_t) 0x17)
#define CSR_BT_AVDTP_PSM                          ((psm_t) 0x19)
#define CSR_BT_AVCTP_BROWSING_PSM                 ((psm_t) 0x1B)
#define CSR_BT_GATT_PSM                           ((psm_t) 0x1F)

/* UUIDs for specified protocols */
#define CSR_BT_PROTOCOL_AVCTP_UUID                ((uuid16_t) 0x0017)
#define CSR_BT_PROTOCOL_AVDTP_UUID                ((uuid16_t) 0x0019)
#define CSR_BT_PROTOCOL_MCAP_CONTROL_UUID         ((uuid16_t) 0x001E)
#define CSR_BT_PROTOCOL_MCAP_DATA_UUID            ((uuid16_t) 0x001F)

/* UUIDs for specified service classes */
#define CSR_BT_OBEX_SYNCML_TRANSFER_UUID          ((uuid16_t) 0x0000)

#define CSR_BT_SPP_PROFILE_UUID                   ((uuid16_t) 0x1101)
#define CSR_BT_LAP_PROFILE_UUID                   ((uuid16_t) 0x1102)
#define CSR_BT_DUN_PROFILE_UUID                   ((uuid16_t) 0x1103)
#define CSR_BT_OBEX_IR_MC_SYNC_SERVICE_UUID       ((uuid16_t) 0x1104)
#define CSR_BT_OBEX_OBJECT_PUSH_SERVICE_UUID      ((uuid16_t) 0x1105)
#define CSR_BT_OBEX_FILE_TRANSFER_UUID            ((uuid16_t) 0x1106)
#define CSR_BT_HS_PROFILE_UUID                    ((uuid16_t) 0x1108)
#define CSR_BT_CTP_PROFILE_UUID                   ((uuid16_t) 0x1109)
#define CSR_BT_AUDIO_SOURCE_UUID                  ((uuid16_t) 0x110A)
#define CSR_BT_AUDIO_SINK_UUID                    ((uuid16_t) 0x110B)
#define CSR_BT_AV_REMOTE_CONTROL_TARGET_UUID      ((uuid16_t) 0x110C)
#define CSR_BT_ADVANCED_AUDIO_PROFILE_UUID        ((uuid16_t) 0x110D)
#define CSR_BT_AV_REMOTE_CONTROL_UUID             ((uuid16_t) 0x110E)
#define CSR_BT_AV_REMOTE_CONTROL_CONTROLLER_UUID  ((uuid16_t) 0x110F)
#define CSR_BT_ICP_PROFILE_UUID                   ((uuid16_t) 0x1110)
#define CSR_BT_FAX_PROFILE_UUID                   ((uuid16_t) 0x1111)
#define CSR_BT_HEADSET_AG_SERVICE_UUID            ((uuid16_t) 0x1112)
#define CSR_BT_PAN_PANU_PROFILE_UUID              ((uuid16_t) 0x1115)
#define CSR_BT_PAN_NAP_PROFILE_UUID               ((uuid16_t) 0x1116)
#define CSR_BT_PAN_GN_PROFILE_UUID                ((uuid16_t) 0x1117)
#define CSR_BT_DIRECT_PRINTING_UUID               ((uuid16_t) 0x1118)
#define CSR_BT_REFERENCE_PRINTING_UUID            ((uuid16_t) 0x1119)
#define CSR_BT_OBEX_IMAGING_UUID                  ((uuid16_t) 0x111A)
#define CSR_BT_OBEX_IMAGING_RESPONDER_UUID        ((uuid16_t) 0x111B)
#define CSR_BT_OBEX_IMAGING_AUTOMATIC_ARCHIVE_UUID ((uuid16_t) 0x111C)
#define CSR_BT_HF_PROFILE_UUID                    ((uuid16_t) 0x111E)
#define CSR_BT_HFG_PROFILE_UUID                   ((uuid16_t) 0x111F)
#define CSR_BT_DIRECT_PRINTING_REFERENCE_OBJ_UUID ((uuid16_t) 0x1120)
#define CSR_BT_BASIC_PRINTING_UUID                ((uuid16_t) 0x1122)
#define CSR_BT_PRINTING_STATUS_UUID               ((uuid16_t) 0x1123)
#define CSR_BT_HID_PROFILE_UUID                   ((uuid16_t) 0x1124)
#define CSR_BT_HCR_PROFILE_UUID                   ((uuid16_t) 0x1125)
#define CSR_BT_SIM_ACCESS_PROFILE_UUID            ((uuid16_t) 0x112D)
#define CSR_BT_OBEX_PBA_CLIENT_PROFILE_UUID       ((uuid16_t) 0x112E)
#define CSR_BT_OBEX_PBA_SERVER_PROFILE_UUID       ((uuid16_t) 0x112F)
#define CSR_BT_OBEX_PBAP_PROFILE_UUID             ((uuid16_t) 0x1130)
#define CSR_BT_HS_12_PROFILE_UUID                 ((uuid16_t) 0x1131)
#define CSR_BT_OBEX_MESSAGE_ACCESS_SERVER_UUID         ((uuid16_t) 0x1132)
#define CSR_BT_OBEX_MESSAGE_NOTIFICATION_SERVER_UUID   ((uuid16_t) 0x1133)
#define CSR_BT_OBEX_MESSAGE_ACCESS_PROFILE_UUID        ((uuid16_t) 0x1134)
#define CSR_BT_PNP_INFORMATION_UUID               ((uuid16_t) 0x1200)
#define CSR_BT_VIDEO_SOURCE_UUID                  ((uuid16_t) 0x1303)
#define CSR_BT_VIDEO_SINK_UUID                    ((uuid16_t) 0x1304)
#define CSR_BT_VIDEO_DISTRIBUTION_UUID            ((uuid16_t) 0x1305)
#define CSR_BT_HDP_PROFILE_UUID                   ((uuid16_t) 0x1400)
#define CSR_BT_HDP_SOURCE_UUID                    ((uuid16_t) 0x1401)
#define CSR_BT_HDP_SINK_UUID                      ((uuid16_t) 0x1402)
#define CSR_BT_PUBLIC_BROWSE_GROUP_UUID           ((uuid16_t) 0x1002)
#define CSR_BT_GENERIC_ACCESS_PROFILE_UUID        ((uuid16_t) 0x1800)
#define CSR_BT_GENERIC_ATTRIBUTE_PROFILE_UUID     ((uuid16_t) 0x1801)
#define CSR_BT_GNSS_PROFILE_UUID                  ((uuid16_t) 0x1135)
#define CSR_BT_GNSS_SERVER_UUID                   ((uuid16_t) 0x1136)




/* Defines a maximum time value and an infinite time value */
#define CSR_BT_INFINITE_TIME                      (0)             /* Defines an infinite time value */
#define CSR_BT_UNLIMITED                          (0)             /* Defines an unlimited number */

/* Feature mask */
/* byte 0 */
#define CSR_BT_EXT_FEAT_3_SLOT_PACKETS                 0x0001
#define CSR_BT_EXT_FEAT_5_SLOT_PACKETS                 0x0002
#define CSR_BT_EXT_FEAT_ENCRYPTION                     0x0004
#define CSR_BT_EXT_FEAT_SLOT_OFFSET                    0x0008
#define CSR_BT_EXT_FEAT_TIMING_ACCURACY                0x0010
#define CSR_BT_EXT_FEAT_ROLE_SWITCH                    0x0020
#define CSR_BT_EXT_FEAT_HOLD_MODE                      0x0040
#define CSR_BT_EXT_FEAT_SNIFF_MODE                     0x0080

/* byte 1 */
#define CSR_BT_EXT_FEAT_PARK_STATE                     0x0100
#define CSR_BT_EXT_FEAT_POWER_CONTROL_REQ              0x0200
#define CSR_BT_EXT_FEAT_CQDDR                          0x0400
#define CSR_BT_EXT_FEAT_SCO_LINK                       0x0800
#define CSR_BT_EXT_FEAT_HV2_PACKETS                    0x1000
#define CSR_BT_EXT_FEAT_HV3_PACKETS                    0x2000
#define CSR_BT_EXT_FEAT_MU_LAW_LOG_SYNCHRONOUS_DATA    0x4000
#define CSR_BT_EXT_FEAT_A_LAW_LOG_SYNCHRONOUS_DATA     0x8000

/* byte 2 */
#define CSR_BT_EXT_FEAT_CVSD_SYNCHRONOUS_DATA          0x0001
#define CSR_BT_EXT_FEAT_PAGING_PARAMETER_NEGOTIATION   0x0002
#define CSR_BT_EXT_FEAT_POWER_CONTROL                  0x0004
#define CSR_BT_EXT_FEAT_TRANSPARENT_SYNCHRONOUS_DATA   0x0008
#define CSR_BT_EXT_FEAT_FLOW_CONTROL_LAG_LSB           0x0010
#define CSR_BT_EXT_FEAT_FLOW_CONTROL_LAG_MB            0x0020
#define CSR_BT_EXT_FEAT_FLOW_CONTROL_LAG_MSB           0x0040
#define CSR_BT_EXT_FEAT_BROADCAST_ENCRYPTION           0x0080

/* byte 3 */
/* reserved                                            0x0100 */
#define CSR_BT_EXT_FEAT_EDR_ACL_2_MBPS_MODE            0x0200
#define CSR_BT_EXT_FEAT_EDR_ACL_3_MBPS_MODE            0x0400
#define CSR_BT_EXT_FEAT_ENHANCED_INQUIRY_SCAN          0x0800
#define CSR_BT_EXT_FEAT_INTERLACED_INQUIRY_SCAN        0x1000
#define CSR_BT_EXT_FEAT_INTERLACED_PAGE_SCAN           0x2000
#define CSR_BT_EXT_FEAT_RSSI_WITH_INQUIRY_RESULTS      0x4000
#define CSR_BT_EXT_FEAT_EV3_PACKETS                    0x8000

/* byte 4 */
#define CSR_BT_EXT_FEAT_EV4_PACKETS                    0x0001
#define CSR_BT_EXT_FEAT_EV5_PACKETS                    0x0002
/* reserved                                            0x0004 */
#define CSR_BT_EXT_FEAT_AFH_CAPABLE_SLAVE              0x0008
#define CSR_BT_EXT_FEAT_AFH_CLASSIFICATION_SLAVE       0x0010
/* reserved                                            0x0020 */
/* reserved                                            0x0040 */
#define CSR_BT_EXT_FEAT_3_SLOT_EDR_ACL_PACKETS         0x0080

/* byte 5 */
#define CSR_BT_EXT_FEAT_5_SLOT_EDR_ACL_PACKETS         0x0100
/* reserved                                            0x0200 */
/* reserved                                            0x0400 */
#define CSR_BT_EXT_FEAT_AFH_CAPABLE_MASTER             0x0800
#define CSR_BT_EXT_FEAT_AFH_CLASSIFICATION_MASTER      0x1000
#define CSR_BT_EXT_FEAT_EDR_ESCO_2_MBPS_MODE           0x2000
#define CSR_BT_EXT_FEAT_EDR_ESCO_3_MBPS_MODE           0x4000
#define CSR_BT_EXT_FEAT_3_SLOT_ENHANCED_DATA_RATE      0x8000

/* byte 6 */
/* reserved                                            0x0001 */
/* reserved                                            0x0002 */
/* reserved                                            0x0004 */
/* reserved                                            0x0008 */
/* reserved                                            0x0010 */
/* reserved                                            0x0020 */
/* reserved                                            0x0040 */
/* reserved                                            0x0080 */

/* byte 7 */
/* reserved                                            0x0100 */
/* reserved                                            0x0200 */
/* reserved                                            0x0400 */
/* reserved                                            0x0800 */
/* reserved                                            0x1000 */
/* reserved                                            0x2000 */
/* reserved                                            0x4000 */
#define CSR_BT_EXT_FEAT_EXTENDED_FEATURES              0x8000

#define CSR_BT_DCE                                        (1)
#define CSR_BT_DTE                                        (2)

#define CSR_BT_MODEM_CTS_MASK                             0x01
#define CSR_BT_MODEM_RTS_MASK                             0x02
#define CSR_BT_MODEM_DSR_MASK                             0x04
#define CSR_BT_MODEM_DTR_MASK                             0x08
#define CSR_BT_MODEM_RI_MASK                              0x10
#define CSR_BT_MODEM_DCD_MASK                             0x20

#define CSR_BT_LINK_STATUS_DISCONNECTED                   (0xAB)
#define CSR_BT_LINK_STATUS_MAX_RECONNECT_TIME             (0xCA)
#define CSR_BT_LINK_STATUS_CONNECTED                      (CSR_BT_ACTIVE_MODE)
#define CSR_BT_LINK_STATUS_HOLD                           (CSR_BT_HOLD_MODE)
#define CSR_BT_LINK_STATUS_SNIFF                          (CSR_BT_SNIFF_MODE)

#define CSR_BT_MASTER_ROLE                                ((CsrUint8) 0)
#define CSR_BT_SLAVE_ROLE                                 ((CsrUint8) 1)
#define CSR_BT_UNDEFINED_ROLE                             ((CsrUint8) 2)

/* CSR_BT_PCM_DONT_MAP is used only when CSR_USE_DSPM is defined */
#define CSR_BT_PCM_DONT_CARE                              (0xFF)
#define CSR_BT_PCM_DONT_MAP                               (0xFE)

/* Extended Inquiry Response - types from specification */
#define CSR_BT_EIR_DATA_END                                 (0x00)
#define CSR_BT_EIR_DATA_TYPE_FLAGS                          (0x01)
#define CSR_BT_EIR_DATA_TYPE_MORE_16_BIT_UUID               (0x02)
#define CSR_BT_EIR_DATA_TYPE_COMPLETE_16_BIT_UUID           (0x03)
#define CSR_BT_EIR_DATA_TYPE_MORE_32_BIT_UUID               (0x04)
#define CSR_BT_EIR_DATA_TYPE_COMPLETE_32_BIT_UUID           (0x05)
#define CSR_BT_EIR_DATA_TYPE_MORE_128_BIT_UUID              (0x06)
#define CSR_BT_EIR_DATA_TYPE_COMPLETE_128_BIT_UUID          (0x07)
#define CSR_BT_EIR_DATA_TYPE_SHORT_LOCAL_NAME               (0x08)
#define CSR_BT_EIR_DATA_TYPE_COMPLETE_LOCAL_NAME            (0x09)
#define CSR_BT_EIR_DATA_TYPE_TX_POWER                       (0x0A)
#define CSR_BT_EIR_DATA_TYPE_SM_TK_VALUE                    (0x10)
#define CSR_BT_EIR_DATA_TYPE_SM_OOB                         (0x11)
#define CSR_BT_EIR_DATA_TYPE_SLAVE_CONN_INTERVAL_RANGE      (0x12)
#define CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_16_BIT_UUID  (0x14)
#define CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_128_BIT_UUID (0x15)
#define CSR_BT_EIR_DATA_TYPE_SERV_DATA                      (0x16)
#define CSR_BT_EIR_DATA_TYPE_PUBLIC_TARGET_ADDRESS          (0x17)
#define CSR_BT_EIR_DATA_TYPE_RANDOM_TARGET_ADDRESS          (0x18)
#define CSR_BT_EIR_DATA_TYPE_APPEARANCE                     (0x19)
#define CSR_BT_EIR_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA     (0xFF)

/* Proprietary CSR extensions for TAG-values */
#define CSR_BT_TAG_SDR_ENTRY                               (0x0100)
#define CSR_BT_TAG_SDR_ATTRIBUTE_ENTRY                     (0x0101)

/* EIR flag bits */
#define CSR_BT_EIR_FLAG_DATA_LEN                           (0x01)
#define CSR_BT_EIR_FLAG_LE_LIMITED_DISCOVERABLE            ((CsrUint8)0x01)
#define CSR_BT_EIR_FLAG_LE_GENERAL_DISCOVERABLE            ((CsrUint8)0x02)
#define CSR_BT_EIR_FLAG_LE_BREDR_NOT_SUPPORTED             ((CsrUint8)0x04)
#define CSR_BT_EIR_FLAG_SIMUL_BREDR_LE_CONTROLLER          ((CsrUint8)0x08)
#define CSR_BT_EIR_FLAG_SIMUL_BREDR_LE_HOST                ((CsrUint8)0x10)

/* Simple Pairing specific */
#define CSR_BT_SP_OOB_OPTIONAL_DATA_LEN                    (0x0B)
#define CSR_BT_SP_OOB_BD_ADDR                              (0x0C)
#define CSR_BT_SP_OOB_CLASS_OF_DEVICE                      (0x0D)
#define CSR_BT_SP_OOB_HASH_C                               (0x0E)
#define CSR_BT_SP_OOB_RANDOMIZER_R                         (0x0F)
#define CSR_BT_SP_OOB_PIN_LENGTH                           (0x10)
#define CSR_BT_SP_OOB_PIN                                  (0x11)

/* Extended EIR tags */
#define CSR_BT_EIR_EXTENDED_TAG_MIN                        (0xF0)
#define CSR_BT_EIR_EXTENDED_TAG_MAX                        (0xFE)
#define CSR_BT_EIR_DATA_TYPE_MANUFACTURER_SPECIFIC         (0xFF)

/* Note cannot remove these defines (even that their are define twice) 
   as it may be consider as an API change */
#define CSR_BT_EIR_DATA_TYPE_SLAVE_CONN_INTERVAL           CSR_BT_EIR_DATA_TYPE_SLAVE_CONN_INTERVAL_RANGE
#define CSR_BT_EIR_DATA_TYPE_SERVICE_LIST_16               CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_16_BIT_UUID
#define CSR_BT_EIR_DATA_TYPE_SERVICE_LIST_128              CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_128_BIT_UUID
#define CSR_BT_EIR_DATA_TYPE_SERVICE_DATA                  CSR_BT_EIR_DATA_TYPE_SERV_DATA


/* Misc defines related to EIR */
#define CSR_BT_EIR_TAG_HEADER_SIZE                         (2)
#define CSR_BT_EIR_TYPE_TAG_SIZE                           (1)
#define CSR_BT_EIR_LENGTH_TAG_SIZE                         (1) 
#define CSR_BT_EIR_DATA_MAX_SIZE                           (240)
#define CSR_BT_EIR_DATA_TYPE_APPEARANCE_SIZE               (2)
#define CSR_BT_EIR_DATA_TYPE_DEVICE_ADDRESS_SIZE           (6)
#define CSR_BT_EIR_DATA_TYPE_SLAVE_CONN_INT_RANGE_SIZE     (4) 

#define CSR_BT_EIR_MANUFACTURER_NOT_AVAILABLE              (0x00)
#define CSR_BT_EIR_MANUFACTURER_PRIORITY_LOW               (0x01)
#define CSR_BT_EIR_MANUFACTURER_PRIORITY_HIGH              (0x02)
#define CSR_BT_EIR_MANUFACTURER_DATA_MAX_SIZE              (CSR_BT_EIR_DATA_MAX_SIZE - CSR_BT_EIR_TAG_HEADER_SIZE)

#define CSR_BT_EIR_UUID16_SIZE                             (2)
#define CSR_BT_EIR_UUID32_SIZE                             (4)
#define CSR_BT_EIR_UUID128_SIZE                            (16)
#define CSR_BT_EIR_NO_OF_UUID_TYPES                        (3)

/* Bluetooth versions as defined by the specification */
#define CSR_BT_BLUETOOTH_VERSION_1P0                       (0x00)
#define CSR_BT_BLUETOOTH_VERSION_1P1                       (0x01)
#define CSR_BT_BLUETOOTH_VERSION_1P2                       (0x02)
#define CSR_BT_BLUETOOTH_VERSION_2P0                       (0x03)
#define CSR_BT_BLUETOOTH_VERSION_2P1                       (0x04)
#define CSR_BT_BLUETOOTH_VERSION_3P0                       (0x05)
#define CSR_BT_BLUETOOTH_VERSION_4P0                       (0x06)
#define CSR_BT_BLUETOOTH_VERSION_4P1                       (0x07)
#define CSR_BT_BLUETOOTH_VERSION_4P2                       (0x08)
#define CSR_BT_BLUETOOTH_VERSION_5P0                       (0x09)

/* Misc defines from the Bluetooth specification */
#define CSR_BT_RSSI_INVALID                                (-128)

#ifndef CSR_BT_MAX_FRIENDLY_NAME_LEN
#define CSR_BT_MAX_FRIENDLY_NAME_LEN                       (50)
#endif /* CSR_BT_MAX_FRIENDLY_NAME_LEN */

/* Name of the peer device, add 1 to ensure that we can add a zero char */
typedef CsrUtf8String    CsrBtDeviceName[CSR_BT_MAX_FRIENDLY_NAME_LEN + 1];

typedef struct
{
    CsrBtDeviceAddr      deviceAddr;
    CsrBtAddressType     addrType;
    CsrBtDeviceName      friendlyName;
    CsrBtClassOfDevice   classOfDevice;
    CsrUint32            knownServices11_00_31;
    CsrUint32            knownServices11_32_63;
    CsrUint32            knownServices12_00_31;
    CsrUint32            knownServices13_00_31;
    CsrBool              authorised;
} CsrBtDevicePropertiesType;

#define CSR_BT_MAX_NUM_OF_LANGUAGE_ELEMENTS    3
typedef struct
{
    CsrUint16            nationalLanguageId;
    CsrUint16            characterEncoding;
    CsrUint16            attributeId;
} CsrBtLanguageElement;

/* status codes used for profiles with data and control path split support */
#define CSR_BT_DATA_PATH_STATUS_OPEN        0
#define CSR_BT_DATA_PATH_STATUS_CLOSED      1
#define CSR_BT_DATA_PATH_STATUS_LOST        2


/* Extended invalid attribute handle use by GATT ServiceChanged
   procedure. Note the handle of the Client Configuration 
   Descriptor under the Service Change Charateristic cannot be 
   less than 4, as the GATT Service and the Service Change 
   Charateristic and the Service Change Charateristic Value 
   shall be located before the Value handle of the Client 
   Configuration descriptor */
#define CSR_BT_GATT_SERVICE_CHANGED_NOT_FOUND                                        0x00000000 /* Reserve Value Shall not be changed */
#define CSR_BT_GATT_SERVICE_CHANGED_NOT_SUPPORTED                                    0x00010000 /* Reserve Value Shall not be changed */
#define CSR_BT_GATT_SERVICE_CHANGED_VALUE_HANDLE_MASK                                0x0000FFFF
#define CSR_BT_GATT_SERVICE_CHANGED_DESCRIPTOR_HANDLE_MASK                           0xFFFF0000

#define CSR_BT_GATT_SERVICE_CHANGED_GET_VALUE_HANDLE(_serviceHdl)                    ((CsrUint16)(CSR_BT_GATT_SERVICE_CHANGED_VALUE_HANDLE_MASK & _serviceHdl))
#define CSR_BT_GATT_SERVICE_CHANGED_GET_DESCRIPTOR_HANDLE(_serviceHdl)               ((CsrUint16)((CSR_BT_GATT_SERVICE_CHANGED_DESCRIPTOR_HANDLE_MASK & _serviceHdl) >> 16))
#define CSR_BT_GATT_SERVICE_CHANGED_CREATE_SERVICE_HANDLE(_descriptorHdl, _valueHdl) ((CsrUint32)((_descriptorHdl << 16) | _valueHdl))

/*************************************************************************************
    Defines Appearance AD types values. 
    The values are composed of a category (10-bits) and sub-categories (6-bits). 
    and can be found in assigned numbers
************************************************************************************/
#define CSR_BT_APPEARANCE_UNKNOWN                                   ((CsrUint16)0x0000)
#define CSR_BT_APPEARANCE_GENERIC_PHONE                             ((CsrUint16)0x0040)
#define CSR_BT_APPEARANCE_GENERIC_COMPUTER                          ((CsrUint16)0x0080)
#define CSR_BT_APPEARANCE_GENERIC_WATCH                             ((CsrUint16)0x00C0)
#define CSR_BT_APPEARANCE_SPORTS_WATCH                              ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_WATCH | 0x0001))
#define CSR_BT_APPEARANCE_GENERIC_CLOCK                             ((CsrUint16)0x0100)
#define CSR_BT_APPEARANCE_GENERIC_DISPLAY                           ((CsrUint16)0x0140)
#define CSR_BT_APPEARANCE_GENERIC_REMOTE_CONTROL                    ((CsrUint16)0x0180)
#define CSR_BT_APPEARANCE_GENERIC_EYE_GLASSES                       ((CsrUint16)0x01C0)
#define CSR_BT_APPEARANCE_GENERIC_TAG                               ((CsrUint16)0x0200)
#define CSR_BT_APPEARANCE_GENERIC_KEYRING                           ((CsrUint16)0x0240)
#define CSR_BT_APPEARANCE_GENERIC_MEDIA_PLAYER                      ((CsrUint16)0x0280)
#define CSR_BT_APPEARANCE_GENERIC_BARCODE_SCANNER                   ((CsrUint16)0x02C0)
#define CSR_BT_APPEARANCE_GENERIC_THERMOMETER                       ((CsrUint16)0x0300)
#define CSR_BT_APPEARANCE_THERMOMETER_EAR                           ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_THERMOMETER | 0x0001))
#define CSR_BT_APPEARANCE_GENERIC_HEART_RATE_SENSOR                 ((CsrUint16)0x0340)
#define CSR_BT_APPEARANCE_HEART_RATE_BELT                           ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HEART_RATE_SENSOR | 0x0001))
#define CSR_BT_APPEARANCE_GENERIC_BLOOD_PRESSURE                    ((CsrUint16)0x0380)
#define CSR_BT_APPEARANCE_BLOOD_PRESSURE_ARM                        ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_BLOOD_PRESSURE | 0x0001))
#define CSR_BT_APPEARANCE_BLOOD_PRESSURE_WRIST                      ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_BLOOD_PRESSURE | 0x0002))
#define CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE            ((CsrUint16)0x03C0)
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_KEYBOARD           ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0001))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_MOUSE              ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0002))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_JOYSTICK           ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0003))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_GAMEPAD            ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0004))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_DIGITIZER_TABLET   ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0005))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_CARD_READER        ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0006))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_DIGITAL_PEN        ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0007))
#define CSR_BT_APPEARANCE_HUMAN_INTERFACE_DEVICE_BARCODE_SCANNER    ((CsrUint16)(CSR_BT_APPEARANCE_GENERIC_HUMAN_INTERFACE_DEVICE | 0x0008))
#define CSR_BT_APPEARANCE_GENERIC_GLUCOSE_METER                     ((CsrUint16)0x0400)

#ifdef __cplusplus
}
#endif

#endif
