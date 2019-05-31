#ifndef CSR_BT_CM_PRIM_H__
#define CSR_BT_CM_PRIM_H__

/******************************************************************************
 Copyright (c) 2001-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "csr_bt_common.h"
#include "csr_bt_addr.h"
#include "rfcomm_prim.h"
#include "hci_prim.h"
#include "l2cap_prim.h"
#include "csr_bt_bnep_prim.h"
#include "dm_prim.h"
#include "csr_bt_result.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtCmPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the Connection Managers (CM) CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_CM_SUCCESS                                   ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_CM_COMMAND_DISALLOWED                        ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_CM_INTERNAL_ERROR                            ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_CM_CANCELLED                                 ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_CM_UNKNOWN_CONNECTION_IDENTIFIER             ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_CM_UNSPECIFIED_ERROR                         ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_CM_ALREADY_CONNECTING                        ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_CM_UNACCEPTABLE_PARAMETER                    ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_CM_NUMBER_OF_LM_EVENT_FILTERS_EXCEEDED       ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_CM_UNSUPPORTED_FEATURE                       ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_CM_REJECTED_DUE_TO_LIMITED_RESOURCES         ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_CM_NOTHING_TO_CANCEL                         ((CsrBtResultCode) (0x000B))
#define CSR_BT_RESULT_CODE_CM_REBOND_REJECTED_BY_APPLICATION            ((CsrBtResultCode) (0x000C))
#define CSR_BT_RESULT_CODE_CM_SYNCHRONOUS_CONNECTION_ALREADY_EXISTS     ((CsrBtResultCode) (0x000D))
#define CSR_BT_RESULT_CODE_CM_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE    ((CsrBtResultCode) (0x000E))
#define CSR_BT_RESULT_CODE_CM_INVALID_PCM_SLOT                          ((CsrBtResultCode) (0x000F))
#define CSR_BT_RESULT_CODE_CM_PCM_SLOT_BLOCKED                          ((CsrBtResultCode) (0x0010))
#define CSR_BT_RESULT_CODE_CM_SYNCHRONOUS_CONNECTION_LIMIT_EXCEEDED     ((CsrBtResultCode) (0x0011))
#define CSR_BT_RESULT_CODE_CM_SYNCHRONOUS_CONNECTION_ATTEMPT_FAILED     ((CsrBtResultCode) (0x0012))
#define CSR_BT_RESULT_CODE_CM_SYNCHRONOUS_CONNECTION_ALREADY_ACCEPTABLE ((CsrBtResultCode) (0x0013))
#define CSR_BT_RESULT_CODE_CM_MODE_CHANGE_REQUEST_OVERRULED             ((CsrBtResultCode) (0x0014))
#define CSR_BT_RESULT_CODE_CM_FLOW_CONTROL_VIOLATED                     ((CsrBtResultCode) (0x0015))
#define CSR_BT_RESULT_CODE_CM_BNEP_CONNECTION_LIMIT_EXCEEDED            ((CsrBtResultCode) (0x0016))
#define CSR_BT_RESULT_CODE_CM_TIMEOUT                                   ((CsrBtResultCode) (0x0017))
#define CSR_BT_RESULT_CODE_CM_AMP_LINK_LOSS_MOVE                        ((CsrBtResultCode) (0x0018))
#define CSR_BT_RESULT_CODE_CM_SERVER_CHANNEL_ALREADY_USED               ((CsrBtResultCode) (0x0019))

#define CSR_BT_ACTIVE_MODE               (0x0000)
#define CSR_BT_HOLD_MODE                 (0x0001)
#define CSR_BT_SNIFF_MODE                (0x0002)

#define CSR_BT_NO_SERVER                 (0xFF)

#define CSR_BT_MICROSEC2SEC              (1000000)

#define CSR_BT_EVENT_FILTER_ALL          (0)
#define CSR_BT_EVENT_FILTER_INQUIRY      (1)
#define CSR_BT_EVENT_FILTER_CONNECTIONS  (2)

#define CSR_BT_CM_CONTEXT_UNUSED         (0x0000)

#define CSR_BT_CM_ACCESS_CODE_GIAC       (0x9e8b33)
#define CSR_BT_CM_ACCESS_CODE_LIAC       (0x9e8b00)
#define CSR_BT_CM_ACCESS_CODE_HIGHEST    (0x9e8b3f)

/* A2DP */
/* A2DP bit rate */
#define CSR_BT_A2DP_BIT_RATE_UNKNOWN            ((CsrUint32) 0x00000000)
#define CSR_BT_A2DP_BIT_RATE_STREAM_SUSPENDED   ((CsrUint32) 0xFFFFFFFE)
#define CSR_BT_A2DP_BIT_RATE_STREAM_CLOSED      ((CsrUint32) 0xFFFFFFFF)

#ifndef EXCLUDE_CSR_BT_CME_BH_FEATURE
/* These definitions should match with CsrCmeCodecType in csr_cme_prim_8.h */
/* A2DP sampling rate */
#define CSR_BT_A2DP_SAMPLING_FREQ_16000     ((CsrUint8) 0x00)
#define CSR_BT_A2DP_SAMPLING_FREQ_32000     ((CsrUint8) 0x01)
#define CSR_BT_A2DP_SAMPLING_FREQ_44100     ((CsrUint8) 0x02)
#define CSR_BT_A2DP_SAMPLING_FREQ_48000     ((CsrUint8) 0x03)
#define CSR_BT_A2DP_SAMPLING_FREQ_UNKNOWN   ((CsrUint8) 0xFF)

/* A2DP codec location */
#define CSR_BT_A2DP_CODEC_LOCATION_OFF_CHIP ((CsrUint8) 0x00)
#define CSR_BT_A2DP_CODEC_LOCATION_ON_CHIP  ((CsrUint8) 0x01)
#define CSR_BT_A2DP_CODEC_LOCATION_UNKNOWN  ((CsrUint8) 0xFF)

/* A2DP codec type ; This is not based on bluetooth assigned numbers */
#define CSR_BT_A2DP_CODEC_TYPE_SBC          ((CsrUint8) 0x00)
#define CSR_BT_A2DP_CODEC_TYPE_APTX         ((CsrUint8) 0x01)
#define CSR_BT_A2DP_CODEC_TYPE_UNKNOWN      ((CsrUint8) 0xFF)
#endif /* EXCLUDE_CSR_BT_CME_BH_FEATURE */

typedef CsrUint32              CsrBtCmEventMask;
/* Defines for event that the application can subscribe for */
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_NONE                         ((CsrBtCmEventMask) 0x00000000)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_BLUECORE_INITIALIZED         ((CsrBtCmEventMask) 0x00000001)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION               ((CsrBtCmEventMask) 0x00000002)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_SYNCHRONOUS_CONNECTION       ((CsrBtCmEventMask) 0x00000004)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ROLE_CHANGE                  ((CsrBtCmEventMask) 0x00000008)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_MODE_CHANGE                  ((CsrBtCmEventMask) 0x00000010)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LSTO_CHANGE                  ((CsrBtCmEventMask) 0x00000020)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE                 ((CsrBtCmEventMask) 0x00000040)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_EXT_SYNC_CONNECTION          ((CsrBtCmEventMask) 0x00000080)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_REMOTE_FEATURES              ((CsrBtCmEventMask) 0x00000100)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_REMOTE_VERSION               ((CsrBtCmEventMask) 0x00000200)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_A2DP_BIT_RATE                ((CsrBtCmEventMask) 0x00000400)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_INQUIRY_PAGE_STATE           ((CsrBtCmEventMask) 0x00000800)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOW_ENERGY                   ((CsrBtCmEventMask) 0x00001000) /* only for BT4.0+ devices/stacks */
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ENCRYPT_CHANGE               ((CsrBtCmEventMask) 0x00002000)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOCAL_NAME_CHANGE            ((CsrBtCmEventMask) 0x00004000) /* only for BT2.1+ devices/stacks */
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_HIGH_PRIORITY_DATA           ((CsrBtCmEventMask) 0x00008000)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_BLUECORE_DEINITIALIZED       ((CsrBtCmEventMask) 0x00010000)
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LE_OWN_ADDR_TYPE_CHANGE      ((CsrBtCmEventMask) 0x00020000)

#define CSR_BT_CM_NUM_OF_CM_EVENTS                                  (0x00000013)
#define CSR_BT_CM_EVENT_MASK_RESERVER_VALUES_MASK                   (0x0003FFFF)

/* Backwards compatibility mapping */
#define CSR_BT_CM_EVENT_MASK_SUBSCRIBE_BLE_CONNECTION               (CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOW_ENERGY)

typedef CsrUint32 CsrBtCmEventMaskCond;
/* Condition for the eventmask */
#define CSR_BT_CM_EVENT_MASK_COND_NA                           ((CsrBtCmEventMaskCond) 0x00000000)
#define CSR_BT_CM_EVENT_MASK_COND_SUCCESS                      ((CsrBtCmEventMaskCond) 0x00000001)
#define CSR_BT_CM_EVENT_MASK_COND_UNKNOWN                      ((CsrBtCmEventMaskCond) 0xEFFFFFFF)
#define CSR_BT_CM_EVENT_MASK_COND_ALL                          ((CsrBtCmEventMaskCond) 0xFFFFFFFF)

#define CSR_BT_CM_ENC_TYPE_NONE                 DM_SM_ENCR_NONE
#define CSR_BT_CM_ENC_TYPE_LE_AES_CCM           DM_SM_ENCR_ON_LE_AES_CCM
#define CSR_BT_CM_ENC_TYPE_BREDR_E0             DM_SM_ENCR_ON_BREDR_EO
#define CSR_BT_CM_ENC_TYPE_BREDR_AES_CCM        DM_SM_ENCR_ON_BREDR_AES_CCM
#define CSR_BT_CM_ENC_TYPE_KEY_REFRESH          (CSR_BT_CM_ENC_TYPE_BREDR_AES_CCM + 1)


/* CSR_BT_CM_ACL_DETACH_REQ parameters */
typedef CsrUint32 CsrBtCmDetach;
#define CSR_BT_CM_ACL_DETACH_ALWAYS               ((CsrBtCmDetach) (0x00000000))    /* Always detach */
#define CSR_BT_CM_ACL_DETACH_EXCLUDE_L2CAP        ((CsrBtCmDetach) (0x00000001))    /* Do not detach if L2CAP connections are present */
#define CSR_BT_CM_ACL_DETACH_EXCLUDE_RFC          ((CsrBtCmDetach) (0x00000002))    /* Do not detach if RFCOMM connections are present */
#define CSR_BT_CM_ACL_DETACH_EXCLUDE_BNEP         ((CsrBtCmDetach) (0x00000004))    /* Do not detach if BNEP connections are present */
#define CSR_BT_CM_ACL_DETACH_EXCLUDE_LE           ((CsrBtCmDetach) (0x00000008))    /* Do not detach if a LE-over-BR/EDR connection is present */
#define CSR_BT_CM_ACL_DETACH_EXCLUDE_ALL          ((CsrBtCmDetach) (0xFFFFFFFF))    /* Only detach if no connections are present */

#define CSR_BT_ASSIGN_DYNAMIC_PSM      (0x0000)
#define CSR_BT_ACL_HANDLE_INVALID      HCI_HANDLE_INVALID

#define CSR_BT_CM_PRIORITY_NORMAL               (0xFFFF)
#define CSR_BT_CM_PRIORITY_HIGH                 (0x0001)
#define CSR_BT_CM_PRIORITY_HIGHEST              (0x0000)

#define CSR_BT_CM_ALWAYS_MASTER_DEVICES_CLEAR   DM_LP_WRITE_ALWAYS_MASTER_DEVICES_CLEAR
#define CSR_BT_CM_ALWAYS_MASTER_DEVICES_ADD     DM_LP_WRITE_ALWAYS_MASTER_DEVICES_ADD
#define CSR_BT_CM_ALWAYS_MASTER_DEVICES_DELETE  DM_LP_WRITE_ALWAYS_MASTER_DEVICES_DELETE

#define CSR_BT_CM_HANDLER_TYPE_SD               (0x00) /* handles service discovery type primitives */
#define CSR_BT_CM_HANDLER_TYPE_SC               (0x01) /* handles security type primitives */
#define CSR_BT_CM_HANDLER_TYPE_AMP              (0x02) /* handles AMP type primitives */
#define CSR_BT_CM_HANDLER_TYPE_LE               (0x03) /* handles low energy type primitives */

/* Piconet roles */
#define CSR_BT_CM_ROLE_MASTER                   (HCI_MASTER)
#define CSR_BT_CM_ROLE_SLAVE                    (HCI_SLAVE)

/* Low energy advertising types */
#define CSR_BT_CM_LE_ADVTYPE_CONNECTABLE_UNDIRECTED         (HCI_ULP_ADVERT_CONNECTABLE_UNDIRECTED)
#define CSR_BT_CM_LE_ADVTYPE_CONNECTABLE_DIRECTED_HIGH_DUTY (HCI_ULP_ADVERT_CONNECTABLE_DIRECTED_HIGH_DUTY)
#define CSR_BT_CM_LE_ADVTYPE_SCANNABLE                      (HCI_ULP_ADVERT_DISCOVERABLE)
#define CSR_BT_CM_LE_ADVTYPE_NONCONNECTABLE                 (HCI_ULP_ADVERT_NON_CONNECTABLE)

/* Low energy scan types */
#define CSR_BT_CM_LE_SCANTYPE_PASSIVE           (HCI_ULP_PASSIVE_SCANNING)
#define CSR_BT_CM_LE_SCANTYPE_ACTIVE            (HCI_ULP_ACTIVE_SCANNING)
#define CSR_BT_CM_LE_SCANTYPE_INITIATING        ((CsrUint8)0xFF)

/* Low energy advertising channel */
#define BT_CM_LE_CHANMAP_37                         (HCI_ULP_ADVERT_CHANNEL_37)
#define BT_CM_LE_CHANMAP_38                         (HCI_ULP_ADVERT_CHANNEL_38)
#define BT_CM_LE_CHANMAP_39                         (HCI_ULP_ADVERT_CHANNEL_39)
#define BT_CM_LE_CHANMAP_ALL                        (HCI_ULP_ADVERT_CHANNEL_ALL)

/* Low energy master clock accuracy */
#define CSR_BT_CM_LE_CLOCKACCU_500PPM           (HCI_ULP_EV_CLOCK_ACCURACY_500PPM)
#define CSR_BT_CM_LE_CLOCKACCU_250PPM           (HCI_ULP_EV_CLOCK_ACCURACY_250PPM)
#define CSR_BT_CM_LE_CLOCKACCU_150PPM           (HCI_ULP_EV_CLOCK_ACCURACY_150PPM)
#define CSR_BT_CM_LE_CLOCKACCU_100PPM           (HCI_ULP_EV_CLOCK_ACCURACY_100PPM)
#define CSR_BT_CM_LE_CLOCKACCU_75PPM            (HCI_ULP_EV_CLOCK_ACCURACY_75PPM)
#define CSR_BT_CM_LE_CLOCKACCU_50PPM            (HCI_ULP_EV_CLOCK_ACCURACY_50PPM)
#define CSR_BT_CM_LE_CLOCKACCU_30PPM            (HCI_ULP_EV_CLOCK_ACCURACY_30PPM)
#define CSR_BT_CM_LE_CLOCKACCU_20PPM            (HCI_ULP_EV_CLOCK_ACCURACY_20PPM)

/* Low energy scan/advertising/connection modes */
#define CSR_BT_CM_LE_MODE_OFF                   (0x00) /* turn off */
#define CSR_BT_CM_LE_MODE_ON                    (0x01) /* turn on */
#define CSR_BT_CM_LE_MODE_MODIFY                (0x02) /* turn off, set params, and turn on again */
#define CSR_BT_CM_LE_MODE_CONTINUE              (0x04) /* set params, and continue */
#define CSR_BT_CM_LE_MODE_IGNORE                (0x40) /* internal in CM, do not use */
#define CSR_BT_CM_LE_MODE_COEX_NOTIFY           (0x80) /* GATT-to-CM coex notification. Do not use! */

#define CSR_BT_CM_LE_PARCHG_NONE                (0x00) /* do not change */
#define CSR_BT_CM_LE_PARCHG_PAR                 (0x01) /* change HCI (advertise) parameters */
#define CSR_BT_CM_LE_PARCHG_DATA_AD             (0x02) /* change AD data */
#define CSR_BT_CM_LE_PARCHG_DATA_SR             (0x04) /* change SR data */
#define CSR_BT_CM_LE_PARCHG_CONNPAR             (0x08) /* change HCI connection parameters */
#define CSR_BT_CM_LE_PARCHG_WHITELIST           (0x10) /* set whitelist */
#define CSR_BT_CM_LE_PARCHG_INTERNAL            (0x80) /* internal update flag */

#define CSR_BT_CM_LE_MAX_REPORT_LENGTH          (31)   /* maximum length of advertise/scan-response data */

#define CSR_BT_CM_LE_WHITELIST_ADD              (0x00)
#define CSR_BT_CM_LE_WHITELIST_CLEAR            (0x01)

#define CSR_BT_CM_LE_ADV_INTERVAL_NOCONN_MIN    (0x00A0) /* min for ADV_SCAN_IND and ADV_NONCONN_IND */
#define CSR_BT_CM_LE_ADV_INTERVAL_MIN           (0x0020) /* min allowed */
#define CSR_BT_CM_LE_ADV_INTERVAL_MAX           (0x4000) /* max allowed */
#define CSR_BT_CM_LE_ADV_INTERVAL_DEFAULT       (0x0800) /* min for ADV_SCAN_IND and ADV_NONCONN_IND */

/* LE Privacy : type defines for privacy mode & device flag */
typedef CsrUint8 CsrBtPrivacyMode;
typedef CsrUint8 CsrBtDeviceFlag;

/* LE Privacy Modes */
#define CSR_BT_NETWORK_PRIVACY_MODE             ((CsrBtPrivacyMode)0x00)
#define CSR_BT_DEVICE_PRIVACY_MODE              ((CsrBtPrivacyMode)0x01)

#define CSR_BT_CM_AFH_HOST_CHANNEL_CLASSIFICATION_SIZE        (10) /* From HCI spec */
#define CSR_BT_CM_AFH_HOST_CHANNEL_CLASSIFICATION_SIZE_LE     (5)  /* From HCI spec */
#define CSR_BT_CM_AFH_AFH_HOST_CHANNEL_CLASSIFICATION_BAD     (0)  /* From HCI spec */
#define CSR_BT_CM_AFH_AFH_HOST_CHANNEL_CLASSIFICATION_UNKNOWN (1)  /* From HCI spec */

typedef CsrPrim CsrBtCmPrim;

/*************************************************************************************
 Private configuration table entries
************************************************************************************/
#define CSR_BT_CM_AUTOPT_FALSE                                ((CsrUint16)0x0000)
#define CSR_BT_CM_AUTOPT_TRUE                                 ((CsrUint16)0x0001)
#define CSR_BT_CM_AUTOPT_AUTO_FCS_OFF_AMP                     ((CsrUint16)0x0081) /* 16 bit, exact, unused value */

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

#define CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST                                (0x0000)

#define CSR_BT_CM_SET_LOCAL_NAME_REQ                          ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_NAME_REQ                        ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SCO_CONNECT_REQ                             ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SCO_RENEGOTIATE_REQ                         ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SCO_DISCONNECT_REQ                          ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_CLASS_OF_DEVICE_REQ                   ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_MODE_CHANGE_REQ                          ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_DELETE_STORE_LINK_KEY_REQ                ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_REMOVE_DEVICE_REQ                        ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_SET_SEC_MODE_REQ                         ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_ACL_OPEN_REQ                             ((CsrBtCmPrim) (0x000A + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_AUTHENTICATE_REQ                         ((CsrBtCmPrim) (0x000B + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_ENCRYPTION_REQ                           ((CsrBtCmPrim) (0x000C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_ACL_CLOSE_REQ                            ((CsrBtCmPrim) (0x000D + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_SET_DEFAULT_SEC_LEVEL_REQ                ((CsrBtCmPrim) (0x000E + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_UNREGISTER_REQ                           ((CsrBtCmPrim) (0x000F + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_REGISTER_REQ                             ((CsrBtCmPrim) (0x0010 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_WRITE_LP_SETTINGS_REQ                    ((CsrBtCmPrim) (0x0011 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_SWITCH_ROLE_REQ                          ((CsrBtCmPrim) (0x0012 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ROLE_DISCOVERY_REQ                          ((CsrBtCmPrim) (0x0013 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_BD_ADDR_REQ                      ((CsrBtCmPrim) (0x0014 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_NAME_REQ                         ((CsrBtCmPrim) (0x0015 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ENABLE_DUT_MODE_REQ                         ((CsrBtCmPrim) (0x0016 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_SCAN_ENABLE_REQ                       ((CsrBtCmPrim) (0x0017 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_SCAN_ENABLE_REQ                        ((CsrBtCmPrim) (0x0018 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CONNECTABLE_REQ                             ((CsrBtCmPrim) (0x0019 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGE_TO_REQ                           ((CsrBtCmPrim) (0x001A + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_TX_POWER_LEVEL_REQ                     ((CsrBtCmPrim) (0x001B + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_GET_LINK_QUALITY_REQ                        ((CsrBtCmPrim) (0x001C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_RSSI_REQ                               ((CsrBtCmPrim) (0x001D + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_COD_REQ                               ((CsrBtCmPrim) (0x001E + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_COD_REQ                                ((CsrBtCmPrim) (0x001F + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_EXT_FEATURES_REQ                ((CsrBtCmPrim) (0x0020 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SET_AFH_CHANNEL_CLASS_REQ                   ((CsrBtCmPrim) (0x0021 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_AFH_CHANNEL_ASSESSMENT_MODE_REQ        ((CsrBtCmPrim) (0x0022 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_REQ       ((CsrBtCmPrim) (0x0023 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_EXT_FEATURES_REQ                 ((CsrBtCmPrim) (0x0024 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_AFH_CHANNEL_MAP_REQ                    ((CsrBtCmPrim) (0x0025 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_CLOCK_REQ                              ((CsrBtCmPrim) (0x0026 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_VERSION_REQ                      ((CsrBtCmPrim) (0x0027 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_FILTER_BDADDR_REQ                 ((CsrBtCmPrim) (0x0028 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_FILTER_COD_REQ                    ((CsrBtCmPrim) (0x0029 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CLEAR_EVENT_FILTER_REQ                      ((CsrBtCmPrim) (0x002A + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_IAC_REQ                                ((CsrBtCmPrim) (0x002B + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_IAC_REQ                               ((CsrBtCmPrim) (0x002C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_WRITE_CACHE_PARAMS_REQ                   ((CsrBtCmPrim) (0x002D + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_UPDATE_AND_CLEAR_CACHED_PARAM_REQ        ((CsrBtCmPrim) (0x002E + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_ENCRYPTION_STATUS_REQ                  ((CsrBtCmPrim) (0x002F + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGESCAN_SETTINGS_REQ                 ((CsrBtCmPrim) (0x0030 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGESCAN_TYPE_REQ                     ((CsrBtCmPrim) (0x0031 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_INQUIRYSCAN_SETTINGS_REQ              ((CsrBtCmPrim) (0x0032 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_INQUIRYSCAN_TYPE_REQ                  ((CsrBtCmPrim) (0x0033 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_MODE_SETTINGS_REQ                        ((CsrBtCmPrim) (0x0034 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_L2CA_MODE_SETTINGS_REQ                   ((CsrBtCmPrim) (0x0035 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_BNEP_MODE_SETTINGS_REQ                   ((CsrBtCmPrim) (0x0036 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_CHECK_SSR_REQ                            ((CsrBtCmPrim) (0x0037 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_BONDING_REQ                              ((CsrBtCmPrim) (0x0038 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_SEC_MODE_CONFIG_REQ                      ((CsrBtCmPrim) (0x0039 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_READ_LOCAL_OOB_DATA_REQ                  ((CsrBtCmPrim) (0x003A + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_READ_DEVICE_REQ                          ((CsrBtCmPrim) (0x003B + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_EN_ENABLE_ENHANCEMENTS_REQ                  ((CsrBtCmPrim) (0x003C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_EIR_UPDATE_MANUFACTURER_DATA_REQ            ((CsrBtCmPrim) (0x003D + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_WRITE_AUTO_FLUSH_TIMEOUT_REQ             ((CsrBtCmPrim) (0x003E + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_FAILED_CONTACT_COUNTER_REQ             ((CsrBtCmPrim) (0x003F + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_FEATURES_REQ                    ((CsrBtCmPrim) (0x0040 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_VOICE_SETTINGS_REQ                    ((CsrBtCmPrim) (0x0041 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_ACCESS_REQ                               ((CsrBtCmPrim) (0x0042 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ACL_DETACH_REQ                              ((CsrBtCmPrim) (0x0043 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ALWAYS_MASTER_DEVICES_REQ                   ((CsrBtCmPrim) (0x0044 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))  
#define CSR_BT_CM_DISABLE_DUT_MODE_REQ                        ((CsrBtCmPrim) (0x0045 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST)) 
#define CSR_BT_CM_SM_LE_SECURITY_REQ                          ((CsrBtCmPrim) (0x0046 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_SET_ENCRYPTION_KEY_SIZE_REQ              ((CsrBtCmPrim) (0x0047 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_INCOMING_SCO_REQ                            ((CsrBtCmPrim) (0x0048 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_HCI_QOS_SETUP_REQ                        ((CsrBtCmPrim) (0x0049 + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_AUTH_PAYLOAD_TIMEOUT_REQ              ((CsrBtCmPrim) (0x004A + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_ADD_DEVICE_REQ                           ((CsrBtCmPrim) (0x004B + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_RANDOM_ADDRESS_REQ                  ((CsrBtCmPrim) (0x004C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DM_PRIM_DOWNSTREAM_HIGHEST                                 (0x004C + CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST                                  (0x0100)

#define CSR_BT_CM_REGISTER_REQ                                ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CONNECT_REQ                                 ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CONNECT_ACCEPT_REQ                          ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_ACCEPT_CONNECT_REQ                   ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DISCONNECT_REQ                              ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ACCEPT_CONNECT_TIMEOUT                      ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_RFC_MODE_CHANGE_REQ                         ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CONNECT_EXT_REQ                             ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_REGISTER_REQ                           ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECT_REQ                            ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECT_ACCEPT_REQ                     ((CsrBtCmPrim) (0x000A + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CANCEL_CONNECT_ACCEPT_REQ              ((CsrBtCmPrim) (0x000B + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DISCONNECT_REQ                         ((CsrBtCmPrim) (0x000C + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_MODE_CHANGE_REQ                        ((CsrBtCmPrim) (0x000D + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_REGISTER_REQ                           ((CsrBtCmPrim) (0x000E + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_CONNECT_REQ                            ((CsrBtCmPrim) (0x000F + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_CONNECT_ACCEPT_REQ                     ((CsrBtCmPrim) (0x0010 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_CANCEL_CONNECT_ACCEPT_REQ              ((CsrBtCmPrim) (0x0011 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_MODE_CHANGE_REQ                        ((CsrBtCmPrim) (0x0012 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_SWITCH_ROLE_REQ                        ((CsrBtCmPrim) (0x0013 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_DISCONNECT_REQ                         ((CsrBtCmPrim) (0x0014 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_SEARCH_REQ                              ((CsrBtCmPrim) (0x0015 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_SERVICE_SEARCH_REQ                      ((CsrBtCmPrim) (0x0016 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_UUID128_SEARCH_REQ                      ((CsrBtCmPrim) (0x0017 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_OPEN_REQ                                ((CsrBtCmPrim) (0x0018 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_RFC_SEARCH_REQ                          ((CsrBtCmPrim) (0x0019 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_UUID128_RFC_SEARCH_REQ                  ((CsrBtCmPrim) (0x001A + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_RFC_EXTENDED_SEARCH_REQ                 ((CsrBtCmPrim) (0x001B + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_RELEASE_RESOURCES_REQ                   ((CsrBtCmPrim) (0x001C + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDS_REGISTER_REQ                            ((CsrBtCmPrim) (0x001D + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDS_UNREGISTER_REQ                          ((CsrBtCmPrim) (0x001E + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_PRIVATE_SWITCH_ROLE_REQ                     ((CsrBtCmPrim) (0x001F + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_LINK_POLICY_REQ                       ((CsrBtCmPrim) (0x0020 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_LINK_POLICY_REQ                        ((CsrBtCmPrim) (0x0021 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SWITCH_ROLE_REQ                             ((CsrBtCmPrim) (0x0022 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_MODE_CHANGE_REQ                             ((CsrBtCmPrim) (0x0023 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_LOCK_SM_QUEUE_REQ                        ((CsrBtCmPrim) (0x0024 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_PHYSICAL_LINK_STATUS_REQ                 ((CsrBtCmPrim) (0x0025 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_CM_SM_PRIM_DOWNSTREAM_HIGHEST                                 (0x0025 + CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST                                     (0x0200)

#define CSR_BT_CM_INQUIRY_REQ                                 ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_INQUIRY_REQ                          ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SCO_ACCEPT_CONNECT_REQ                      ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SCO_CANCEL_ACCEPT_CONNECT_REQ               ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_REQ                               ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_LINK_KEY_REQUEST_RES                     ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_AUTHORISE_RES                            ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_PIN_REQUEST_RES                          ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_UNREGISTER_REQ                              ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DATA_REQ                                    ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DATA_RES                                    ((CsrBtCmPrim) (0x000A + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CONTROL_REQ                                 ((CsrBtCmPrim) (0x000B + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_PORTNEG_RES                                 ((CsrBtCmPrim) (0x000C + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_EXTENDED_DATA_REQ                      ((CsrBtCmPrim) (0x000D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_EXTENDED_MULTICAST_DATA_REQ            ((CsrBtCmPrim) (0x000E + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_DISCONNECT_RES                         ((CsrBtCmPrim) (0x000F + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_CANCEL_SEARCH_REQ                       ((CsrBtCmPrim) (0x0010 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_ATTRIBUTE_REQ                           ((CsrBtCmPrim) (0x0011 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SDC_CLOSE_REQ                               ((CsrBtCmPrim) (0x0012 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_CONNECT_REQ                          ((CsrBtCmPrim) (0x0013 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_CANCEL_CONNECT_REQ                       ((CsrBtCmPrim) (0x0014 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_L2CA_CONNECT_REQ                     ((CsrBtCmPrim) (0x0015 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ALWAYS_SUPPORT_MASTER_ROLE_REQ              ((CsrBtCmPrim) (0x0016 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_READ_REMOTE_NAME_REQ                 ((CsrBtCmPrim) (0x0017 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_BONDING_CANCEL_REQ                       ((CsrBtCmPrim) (0x0018 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_IO_CAPABILITY_REQUEST_RES                ((CsrBtCmPrim) (0x0019 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_IO_CAPABILITY_REQUEST_NEG_RES            ((CsrBtCmPrim) (0x001A + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_USER_CONFIRMATION_REQUEST_RES            ((CsrBtCmPrim) (0x001B + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_USER_CONFIRMATION_REQUEST_NEG_RES        ((CsrBtCmPrim) (0x001C + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_USER_PASSKEY_REQUEST_RES                 ((CsrBtCmPrim) (0x001D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_USER_PASSKEY_REQUEST_NEG_RES             ((CsrBtCmPrim) (0x001E + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_SEND_KEYPRESS_NOTIFICATION_REQ           ((CsrBtCmPrim) (0x001F + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_REPAIR_RES                               ((CsrBtCmPrim) (0x0020 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_CANCEL_BNEP_CONNECT_REQ                     ((CsrBtCmPrim) (0x0021 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_ROLE_SWITCH_CONFIG_REQ                      ((CsrBtCmPrim) (0x0022 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_MASK_REQ                          ((CsrBtCmPrim) (0x0023 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_MODE_CHANGE_CONFIG_REQ                      ((CsrBtCmPrim) (0x0024 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_UNREGISTER_REQ                         ((CsrBtCmPrim) (0x0025 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_LINK_SUPERV_TIMEOUT_REQ               ((CsrBtCmPrim) (0x0026 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_MOVE_CHANNEL_REQ                            ((CsrBtCmPrim) (0x0027 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_MOVE_CHANNEL_RES                            ((CsrBtCmPrim) (0x0028 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECTIONLESS_DATA_REQ                ((CsrBtCmPrim) (0x0029 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LOGICAL_CHANNEL_TYPE_REQ                    ((CsrBtCmPrim) (0x002A + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_PORTNEG_REQ                                 ((CsrBtCmPrim) (0x002B + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_VERSION_REQ                     ((CsrBtCmPrim) (0x002C + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_REGISTER_HANDLER_REQ                        ((CsrBtCmPrim) (0x002D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_RES                               ((CsrBtCmPrim) (0x002E + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_ABORT_REQ                         ((CsrBtCmPrim) (0x002F + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_A2DP_BIT_RATE_REQ                           ((CsrBtCmPrim) (0x0030 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_GET_SECURITY_CONF_RES                       ((CsrBtCmPrim) (0x0031 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_DATA_BUFFER_EMPTY_REQ                       ((CsrBtCmPrim) (0x0032 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))  
#define CSR_BT_CM_LE_SCAN_REQ                                 ((CsrBtCmPrim) (0x0033 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_ADVERTISE_REQ                            ((CsrBtCmPrim) (0x0034 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_WHITELIST_SET_REQ                        ((CsrBtCmPrim) (0x0035 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_CONNPARAM_REQ                            ((CsrBtCmPrim) (0x0036 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_CONNPARAM_UPDATE_REQ                     ((CsrBtCmPrim) (0x0037 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_ACCEPT_CONNPARAM_UPDATE_RES              ((CsrBtCmPrim) (0x0038 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))  
#define CSR_BT_CM_SM_KEY_REQUEST_RES                          ((CsrBtCmPrim) (0x0039 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_EIR_FLAGS_REQ                               ((CsrBtCmPrim) (0x003A + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_ENCRYPTION_KEY_SIZE_REQ                ((CsrBtCmPrim) (0x003B + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_READ_ADVERTISING_CH_TX_POWER_REQ            ((CsrBtCmPrim) (0x003C + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_RECEIVER_TEST_REQ                        ((CsrBtCmPrim) (0x003D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_TRANSMITTER_TEST_REQ                     ((CsrBtCmPrim) (0x003E + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_TEST_END_REQ                             ((CsrBtCmPrim) (0x003F + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_UNLOCK_SM_QUEUE_REQ                      ((CsrBtCmPrim) (0x0040 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_GET_CONTROLLER_INFO_REQ                  ((CsrBtCmPrim) (0x0041 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_MAP_SCO_PCM_RES                             ((CsrBtCmPrim) (0x0042 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_GET_CHANNEL_INFO_REQ                   ((CsrBtCmPrim) (0x0043 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SET_AV_STREAM_INFO_REQ                      ((CsrBtCmPrim) (0x0044 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_REMOTE_USED_FEATURES_REQ            ((CsrBtCmPrim) (0x0045 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_LOCAL_SUPPORTED_FEATURES_REQ        ((CsrBtCmPrim) (0x0046 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_RESOLVING_LIST_SIZE_REQ             ((CsrBtCmPrim) (0x0047 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_PRIVACY_MODE_REQ                     ((CsrBtCmPrim) (0x0048 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_OWN_ADDRESS_TYPE_REQ                 ((CsrBtCmPrim) (0x0049 + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_STATIC_ADDRESS_REQ                   ((CsrBtCmPrim) (0x004A + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_PVT_ADDR_TIMEOUT_REQ                 ((CsrBtCmPrim) (0x004B + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_CM_SM_HOUSE_CLEANING                           ((CsrBtCmPrim) (0x004C + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST)) /* must be last */
#define CSR_BT_CM_DM_HOUSE_CLEANING                           ((CsrBtCmPrim) (0x004D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST)) /* must be last */
#define CSR_BT_CM_PRIM_DOWNSTREAM_HIGHEST                                    (0x004D + CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST                                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_CANCEL_ACCEPT_CONNECT_CFM                   ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_CONNECT_CFM                                 ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_CONNECT_ACCEPT_CFM                          ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_REGISTER_CFM                                ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_DISCONNECT_IND                              ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SCO_CONNECT_CFM                             ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SCO_DISCONNECT_IND                          ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SCO_ACCEPT_CONNECT_CFM                      ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_DATA_IND                                    ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_DATA_CFM                                    ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_CONTROL_IND                                 ((CsrBtCmPrim) (0x000A + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_RFC_MODE_CHANGE_IND                         ((CsrBtCmPrim) (0x000B + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_PORTNEG_IND                                 ((CsrBtCmPrim) (0x000C + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_PORTNEG_CFM                                 ((CsrBtCmPrim) (0x000D + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_RFC_PRIM_UPSTREAM_HIGHEST                                (0x000D + CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST                                 (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_SDC_SEARCH_IND                              ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_SEARCH_CFM                              ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_SERVICE_SEARCH_CFM                      ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_OPEN_CFM                                ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_ATTRIBUTE_CFM                           ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_CLOSE_IND                               ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_RELEASE_RESOURCES_CFM                   ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDS_REGISTER_CFM                            ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDS_UNREGISTER_CFM                          ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_SDC_PRIM_UPSTREAM_HIGHEST                                  (0x0008 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_CM_SDS_EXT_REGISTER_CFM                        ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDS_EXT_UNREGISTER_CFM                      ((CsrBtCmPrim) (0x000A + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_SDC_PRIM_EXT_UPSTREAM_HIGHEST                              (0x000A + CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST)


/* **** */

#define CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST                                 (0x0200 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_L2CA_REGISTER_CFM                           ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECT_CFM                            ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECT_ACCEPT_CFM                     ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CANCEL_CONNECT_ACCEPT_CFM              ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_CFM                               ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_IND                               ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DISCONNECT_IND                         ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_MODE_CHANGE_IND                        ((CsrBtCmPrim) (0x000A + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_DATA_ABORT_CFM                         ((CsrBtCmPrim) (0x000B + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_GET_CHANNEL_INFO_CFM                   ((CsrBtCmPrim) (0x000C + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_AMP_MOVE_IND                           ((CsrBtCmPrim) (0x000D + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_L2CA_PRIM_UPSTREAM_HIGHEST                                 (0x000D + CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST                                  (0x0300 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_BNEP_CONNECT_IND                            ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_CONNECT_ACCEPT_CFM                     ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_CANCEL_CONNECT_ACCEPT_CFM              ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_EXTENDED_DATA_IND                      ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_MODE_CHANGE_IND                        ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_SWITCH_ROLE_IND                        ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_DISCONNECT_IND                         ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BNEP_EXTENDED_DATA_CFM                      ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_BNEP_PRIM_UPSTREAM_HIGHEST                                 (0x0007 + CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST                             (0x0400 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_INQUIRY_RESULT_IND                          ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_INQUIRY_CFM                                 ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_NAME_CFM                        ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGE_TO_CFM                           ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_HIGHEST                              (0x0003 + CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST                                    (0x0500 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_SET_LOCAL_NAME_CFM                          ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_BD_ADDR_CFM                      ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_LINK_SUPERV_TIMEOUT_CFM               ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_NAME_CFM                         ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_TX_POWER_LEVEL_CFM                     ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_GET_LINK_QUALITY_CFM                        ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_RSSI_CFM                               ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_COD_CFM                               ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_COD_CFM                                ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_VERSION_CFM                      ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_VERSION_CFM                     ((CsrBtCmPrim) (0x000A + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_SCAN_ENABLE_CFM                        ((CsrBtCmPrim) (0x000B + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_SCAN_ENABLE_CFM                       ((CsrBtCmPrim) (0x000C + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_DM_PRIM_UPSTREAM_HIGHEST                                   (0x000C + CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST                                (0x0600 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_READ_REMOTE_EXT_FEATURES_CFM                ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SET_AFH_CHANNEL_CLASS_CFM                   ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_AFH_CHANNEL_ASSESSMENT_MODE_CFM        ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_CFM       ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_LOCAL_EXT_FEATURES_CFM                 ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_AFH_CHANNEL_MAP_CFM                    ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_CLOCK_CFM                              ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_HIGHEST                               (0x0006 + CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST)

/* **** */

#define CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST                                  (0x0700 + CSR_PRIM_UPSTREAM)

#define CSR_BT_CM_ENABLE_DUT_MODE_CFM                         ((CsrBtCmPrim) (0x0000 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_REJECT_RFC_CONNECTION_IND                   ((CsrBtCmPrim) (0x0001 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SDC_UUID128_SEARCH_IND                      ((CsrBtCmPrim) (0x0002 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ROLE_DISCOVERY_CFM                          ((CsrBtCmPrim) (0x0003 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_REMOTE_NAME_IND                        ((CsrBtCmPrim) (0x0004 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_LINK_POLICY_ERROR_IND                 ((CsrBtCmPrim) (0x0005 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_LINK_POLICY_CFM                        ((CsrBtCmPrim) (0x0006 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_CONNECTION_REJ_SECURITY_IND                 ((CsrBtCmPrim) (0x0007 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SCO_RENEGOTIATE_CFM                         ((CsrBtCmPrim) (0x0008 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SCO_RENEGOTIATE_IND                         ((CsrBtCmPrim) (0x0009 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_FILTER_BDADDR_CFM                 ((CsrBtCmPrim) (0x000C + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_FILTER_COD_CFM                    ((CsrBtCmPrim) (0x000D + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_CLEAR_EVENT_FILTER_CFM                      ((CsrBtCmPrim) (0x000E + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECTIONLESS_DATA_IND                ((CsrBtCmPrim) (0x000F + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_IAC_CFM                                ((CsrBtCmPrim) (0x0010 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_IAC_IND                               ((CsrBtCmPrim) (0x0011 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_ENCRYPTION_STATUS_CFM                  ((CsrBtCmPrim) (0x0012 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGESCAN_SETTINGS_CFM                 ((CsrBtCmPrim) (0x0013 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_PAGESCAN_TYPE_CFM                     ((CsrBtCmPrim) (0x0014 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_INQUIRYSCAN_SETTINGS_CFM              ((CsrBtCmPrim) (0x0015 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_INQUIRYSCAN_TYPE_CFM                  ((CsrBtCmPrim) (0x0016 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MAP_SCO_PCM_IND                             ((CsrBtCmPrim) (0x0017 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_EIR_UPDATE_MANUFACTURER_DATA_CFM            ((CsrBtCmPrim) (0x0019 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SM_REPAIR_IND                               ((CsrBtCmPrim) (0x001A + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ACL_DETACH_CFM                              ((CsrBtCmPrim) (0x001B + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_FAILED_CONTACT_COUNTER_CFM             ((CsrBtCmPrim) (0x001C + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SET_EVENT_MASK_CFM                          ((CsrBtCmPrim) (0x001D + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SYNC_CONNECT_IND                            ((CsrBtCmPrim) (0x001E + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SYNC_DISCONNECT_IND                         ((CsrBtCmPrim) (0x001F + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SYNC_RENEGOTIATE_IND                        ((CsrBtCmPrim) (0x0020 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ACL_CONNECT_IND                             ((CsrBtCmPrim) (0x0021 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ACL_DISCONNECT_IND                          ((CsrBtCmPrim) (0x0022 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SNIFF_SUB_RATING_IND                        ((CsrBtCmPrim) (0x0023 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MODE_CHANGE_IND                             ((CsrBtCmPrim) (0x0024 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ROLE_CHANGE_IND                             ((CsrBtCmPrim) (0x0025 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LSTO_CHANGE_IND                             ((CsrBtCmPrim) (0x0026 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BLUECORE_INITIALIZED_IND                    ((CsrBtCmPrim) (0x0027 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MODE_CHANGE_CONFIG_CFM                      ((CsrBtCmPrim) (0x0028 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MODE_CHANGE_CFM                             ((CsrBtCmPrim) (0x0029 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SWITCH_ROLE_CFM                             ((CsrBtCmPrim) (0x002A + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_VOICE_SETTINGS_CFM                    ((CsrBtCmPrim) (0x002B + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MOVE_CHANNEL_CFM                            ((CsrBtCmPrim) (0x002C + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MOVE_CHANNEL_IND                            ((CsrBtCmPrim) (0x002D + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_MOVE_CHANNEL_CMP_IND                        ((CsrBtCmPrim) (0x002E + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_L2CA_CONNECTIONLESS_DATA_CFM                ((CsrBtCmPrim) (0x002F + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LOGICAL_CHANNEL_TYPES_IND                   ((CsrBtCmPrim) (0x0030 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_EXT_SYNC_CONNECT_IND                        ((CsrBtCmPrim) (0x0031 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_REMOTE_FEATURES_IND                         ((CsrBtCmPrim) (0x0032 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_REMOTE_VERSION_IND                          ((CsrBtCmPrim) (0x0033 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_A2DP_BIT_RATE_IND                           ((CsrBtCmPrim) (0x0034 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_INQUIRY_PAGE_EVENT_IND                      ((CsrBtCmPrim) (0x0035 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_GET_SECURITY_CONF_IND                       ((CsrBtCmPrim) (0x0036 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ENCRYPT_CHANGE_IND                          ((CsrBtCmPrim) (0x0037 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_ALWAYS_MASTER_DEVICES_CFM                   ((CsrBtCmPrim) (0x0038 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_DATA_BUFFER_EMPTY_CFM                       ((CsrBtCmPrim) (0x0039 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_DISABLE_DUT_MODE_CFM                        ((CsrBtCmPrim) (0x003A + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_SCAN_CFM                                 ((CsrBtCmPrim) (0x003B + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_ADVERTISE_CFM                            ((CsrBtCmPrim) (0x003C + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_REPORT_IND                               ((CsrBtCmPrim) (0x003D + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LOCAL_NAME_CHANGE_IND                       ((CsrBtCmPrim) (0x003E + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_EVENT_ADVERTISING_IND                    ((CsrBtCmPrim) (0x003F + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_EVENT_SCAN_IND                           ((CsrBtCmPrim) (0x0040 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_EVENT_CONNECTION_IND                     ((CsrBtCmPrim) (0x0041 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_EIR_FLAGS_CFM                               ((CsrBtCmPrim) (0x0042 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_WHITELIST_SET_CFM                        ((CsrBtCmPrim) (0x0043 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_CONNPARAM_CFM                            ((CsrBtCmPrim) (0x0044 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_CONNPARAM_UPDATE_CMP_IND                 ((CsrBtCmPrim) (0x0045 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_ENCRYPTION_KEY_SIZE_CFM                ((CsrBtCmPrim) (0x0046 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_READ_ADVERTISING_CH_TX_POWER_CFM            ((CsrBtCmPrim) (0x0047 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_RECEIVER_TEST_CFM                        ((CsrBtCmPrim) (0x0048 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_TRANSMITTER_TEST_CFM                     ((CsrBtCmPrim) (0x0049 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_TEST_END_CFM                             ((CsrBtCmPrim) (0x004A + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_ACCEPT_CONNPARAM_UPDATE_IND              ((CsrBtCmPrim) (0x004B + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_HIGH_PRIORITY_DATA_IND                      ((CsrBtCmPrim) (0x004C + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_LOCK_SM_QUEUE_IND                        ((CsrBtCmPrim) (0x004D + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_GET_CONTROLLER_INFO_CFM                  ((CsrBtCmPrim) (0x004E + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_BLUECORE_DEINITIALIZED_IND                  ((CsrBtCmPrim) (0x004F + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_REMOTE_USED_FEATURES_CFM            ((CsrBtCmPrim) (0x0050 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_LOCAL_SUPPORTED_FEATURES_CFM        ((CsrBtCmPrim) (0x0051 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_WRITE_AUTH_PAYLOAD_TIMEOUT_CFM              ((CsrBtCmPrim) (0x0052 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_RANDOM_ADDRESS_CFM                  ((CsrBtCmPrim) (0x0053 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_READ_RESOLVING_LIST_SIZE_CFM             ((CsrBtCmPrim) (0x0054 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_PRIVACY_MODE_CFM                     ((CsrBtCmPrim) (0x0055 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_OWN_ADDRESS_TYPE_CFM                 ((CsrBtCmPrim) (0x0056 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_OWN_ADDRESS_TYPE_CHANGED_IND             ((CsrBtCmPrim) (0x0057 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_STATIC_ADDRESS_CFM                   ((CsrBtCmPrim) (0x0058 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_LE_SET_PVT_ADDR_TIMEOUT_CFM                 ((CsrBtCmPrim) (0x0059 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_CM_SEND_PRIM_UPSTREAM_HIGHEST                                 (0x0059 + CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_CM_DM_PRIM_DOWNSTREAM_COUNT                    (CSR_BT_CM_DM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_CM_DM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_CM_SM_PRIM_DOWNSTREAM_COUNT                    (CSR_BT_CM_SM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_CM_SM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_CM_PRIM_DOWNSTREAM_COUNT                       (CSR_BT_CM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_CM_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_CM_RFC_PRIM_UPSTREAM_COUNT                     (CSR_BT_CM_RFC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_RFC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_SDC_PRIM_UPSTREAM_COUNT                     (CSR_BT_CM_SDC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_SDC_PRIM_EXT_UPSTREAM_COUNT                 (CSR_BT_CM_SDC_PRIM_EXT_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_SDC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_L2CA_PRIM_UPSTREAM_COUNT                    (CSR_BT_CM_L2CA_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_L2CA_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_BNEP_PRIM_UPSTREAM_COUNT                    (CSR_BT_CM_BNEP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_BNEP_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_COUNT                 (CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_INQUIRY_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_DM_PRIM_UPSTREAM_COUNT                      (CSR_BT_CM_DM_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_DM_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_COUNT                  (CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_DM_1P2_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_CM_SEND_PRIM_UPSTREAM_COUNT                    (CSR_BT_CM_SEND_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_CM_SEND_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/*************************************************************************************
 Util. structure typedefs
************************************************************************************/

typedef struct
{
    CsrUint16                maxRemoteLatency;
    CsrUint16                minRemoteTimeout;
    CsrUint16                minLocalTimeout;
} CsrBtSsrSettingsDownstream;

typedef struct
{
    CsrBool                  valid;
    CsrUint16                maxTxLatency;
    CsrUint16                maxRxLatency;
    CsrUint16                minRemoteTimeout;
    CsrUint16                minLocalTimeout;
} CsrBtSsrSettingsUpstream;

/*************************************************************************************
 Primitive typedefs
************************************************************************************/
typedef struct 
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmStdCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             appHandle;
    CsrUint32               configMask;
    CsrUint24               inquiryAccessCode;
    CsrInt8                 inquiryTxPowerLevel;
} CsrBtCmInquiryReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmInquiryCfm;

#define CSR_BT_CM_INQUIRY_STATUS_NONE      (0x00000000)    /* Not specific status information available */
#define CSR_BT_CM_INQUIRY_STATUS_EIR       (0x00000001)    /* The result is from a device supporting extended inquiry */

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtClassOfDevice      classOfDevice;
    CsrInt8                 rssi;
    CsrUint8                eirDataLength;
    CsrUint8                *eirData;
    CsrUint32               status;
} CsrBtCmInquiryResultInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
} CsrBtCmCancelInquiryReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    CsrUtf8String                *friendlyName;
} CsrBtCmSetLocalNameReq;

typedef struct
{
    CsrBtCmPrim       type;
    CsrBtResultCode   resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSetLocalNameCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrUint8                  levelType;
} CsrBtCmReadTxPowerLevelReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrInt8                   powerLevel;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadTxPowerLevelCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           deviceAddr;
} CsrBtCmGetLinkQualityReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 linkQuality;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmGetLinkQualityCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
} CsrBtCmReadRssiReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrInt8                   rssi;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadRssiCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmReadRemoteNameReq;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUtf8String                *friendlyName;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmReadRemoteNameCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUtf8String                *friendlyName;
} CsrBtCmReadRemoteNameInd;

typedef struct
{
    CsrBtCmPrim         type;
    CsrSchedQid         appHandle;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtAddressType    addressType;
    CsrBtTransportType  transportType;
} CsrBtCmReadRemoteVersionReq;

typedef struct
{
    CsrBtCmPrim         type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtAddressType    addressType;
    CsrBtTransportType  transportType;
    CsrUint8            lmpVersion;
    CsrUint16           manufacturerName;
    CsrUint16           lmpSubversion;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtCmReadRemoteVersionCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
} CsrBtCmReadLocalBdAddrReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               phandle;
    CsrBtDeviceAddr           deviceAddr;
    CsrUint32                 flags;
    CsrBtAddressType          addressType;
} CsrBtCmAclDetachReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmAclDetachCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmReadLocalBdAddrCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
} CsrBtCmReadLocalNameReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUtf8String                *localName;
} CsrBtCmReadLocalNameCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    link_policy_settings_t  link_policy_settings;
    CsrUint8                 activePlayer;
} CsrBtCmDmWriteLpSettingsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                 activePlayer;
} CsrBtCmWriteClassOfDeviceReq;

#define CSR_BT_CM_SERVICE_CLASS_FILTER            (0xFFE000)
#define CSR_BT_CM_MAJOR_DEVICE_CLASS_FILTER       (0x001F00)
#define CSR_BT_CM_MINOR_DEVICE_CLASS_FILTER       (0x0000FC)

typedef CsrUint8 CsrBtCmUpdateFlags;
#define CSR_BT_CM_WRITE_COD_UPDATE_FLAG_SERVICE_CLASS       ((CsrBtCmUpdateFlags) 0x01)
#define CSR_BT_CM_WRITE_COD_UPDATE_FLAG_MAJOR_MINOR_CLASS   ((CsrBtCmUpdateFlags) 0x02)

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtCmUpdateFlags       updateFlags;
    CsrBtClassOfDevice         serviceClassOfDevice;
    CsrBtClassOfDevice         majorClassOfDevice;
    CsrBtClassOfDevice         minorClassOfDevice;
} CsrBtCmWriteCodReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteCodCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
} CsrBtCmReadCodReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint24                classOfDevice;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmReadCodCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    delete_all_flag_t       flag;
} CsrBtCmSmDeleteStoreLinkKeyReq;

typedef struct
{
    CsrBtCmPrim               type;
    dm_security_mode_t      mode;
    CsrUint8                 mode3Enc;
} CsrBtCmSmSetSecModeReq;

typedef struct
{
    CsrBtCmPrim               type;
    dm_security_level_t     seclDefault;
} CsrBtCmSmSetDefaultSecLevelReq;


typedef struct
{
    CsrBtCmPrim               type;
    dm_protocol_id_t        protocolId;
    CsrUint16                channel;
} CsrBtCmSmUnregisterReq;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 keyType;
    CsrUint8                 keyLength;
    CsrUint8                *key;
    CsrBtAddressType         addressType;
} CsrBtCmSmLinkKeyRequestRes;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 pinLength;
    CsrUint8                 pin[CSR_BT_PASSKEY_MAX_LEN];
} CsrBtCmSmPinRequestRes;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    dm_protocol_id_t         protocolId;
    CsrUint16                channel;
    CsrBool                  incoming;
    CsrUint16                authorisation;
    CsrBtAddressType         addressType;
} CsrBtCmSmAuthoriseRes;

typedef struct
{
    CsrBtCmPrim              type;
    dm_protocol_id_t         protocolId;
    CsrUint16                channel;
    CsrBool                  outgoingOk;
    dm_security_level_t      securityLevel;
    psm_t                    psm;
} CsrBtCmSmRegisterReq;

typedef struct
{
    CsrBtCmPrim              type;
    CsrUint8                 theMode;
    CsrUint8                 thePlayer;
    CsrUint8                 theIndex;
    CsrBtDeviceAddr          deviceAddr;
} CsrBtCmDmModeChangeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmDmCheckSsrReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 role;
    CsrUint8                 activePlayer;
} CsrBtCmDmSwitchRoleReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmRoleDiscoveryReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 role;
} CsrBtCmRoleDiscoveryCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtUuid32                *serviceList;
    CsrUint8                 serviceListSize; /* Number of _items_ in serviceList, _not_ byte size */
    CsrBool                  extendedUuidSearch;
} CsrBtCmSdcSearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtUuid32                *serviceList;
    CsrUint8                 serviceListSize; /* Number of _items_ in serviceList, _not_ byte size */
} CsrBtCmSdcRfcSearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtUuid32                *serviceList;
    CsrUint8                 serviceListSize; /* Number of _items_ in serviceList, _not_ byte size */
} CsrBtCmSdcRfcExtendedSearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtUuid128               *serviceList;
    CsrUint8                 serviceListSize; /* Number of _items_ in serviceList, _not_ byte size */
} CsrBtCmSdcUuid128RfcSearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtUuid128               *serviceList;
    CsrUint8                 serviceListSize; /* Number of _items_ in serviceList, _not_ byte size */
} CsrBtCmSdcUuid128SearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSdcOpenReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSdcOpenCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtUuid32                service;
    CsrBtUuid32                *serviceHandleList;
    CsrUint16                serviceHandleListCount;  /* Number of _items_ in serviceHandleList, _not_ byte size */
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSdcSearchInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtUuid128               service;
    CsrBtUuid32                *serviceHandleList;
    CsrUint16                serviceHandleListCount; /* Number of _items_ in serviceHandleList, _not_ byte size */
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSdcUuid128SearchInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSdcSearchCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint16                uuidSetLength;
    CsrUint8                 *uuidSet;
} CsrBtCmSdcServiceSearchReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                recListLength; /* Number of _items_ in recList, _not_ byte size */
    CsrBtUuid32                *recList;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSdcServiceSearchCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtCmPrim               typeToCancel;
} CsrBtCmSdcCancelSearchReq;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtUuid32              serviceHandle;
    CsrUint16                attributeIdentifier;
    CsrUint16                upperRangeAttributeIdentifier;
    CsrUint16                maxBytesToReturn;
} CsrBtCmSdcAttributeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                 *attributeList;
    CsrUint16                attributeListSize;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSdcAttributeCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
} CsrBtCmSdcCloseReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmSdcCloseInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8           localServerChannel;
} CsrBtCmSdcReleaseResourcesReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr         deviceAddr;
} CsrBtCmSdcReleaseResourcesCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrUint8                *serviceRecord;
    CsrUint16               serviceRecordSize;
    CsrUint16               context;
} CsrBtCmSdsRegisterReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint32               serviceRecHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmSdsRegisterCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint32               serviceRecHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmSdsExtRegisterCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrUint32               serviceRecHandle;
    CsrUint16               context;
} CsrBtCmSdsUnregisterReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint32               serviceRecHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmSdsUnregisterCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint32               serviceRecHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmSdsExtUnregisterCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             appHandle;
    CsrUint16               manufacturerDataSettings;
    CsrUint8                manufacturerDataLength;
    CsrUint8                *manufacturerData;
} CsrBtCmEirUpdateManufacturerDataReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmEirUpdateManufacturerDataCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             appHandle;
    CsrUint8                eirFlags;
} CsrBtCmEirFlagsReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmEirFlagsCfm;

typedef struct
{
    CsrBtCmPrim             type; /* CSR_BT_CM_LE_WHITELIST_SET_REQ */
    CsrSchedQid             appHandle;
    CsrUint16               addressCount;
    CsrBtTypedAddr          *addressList;
} CsrBtCmLeWhitelistSetReq;

typedef struct
{
    CsrBtCmPrim             type; /* CSR_BT_CM_LE_WHITELIST_SET_CFM */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmLeWhitelistSetCfm;

typedef struct
{
    CsrBtCmPrim             type; /* CSR_BT_CM_LE_GET_CONTROLLER_INFO_REQ */
    CsrSchedQid             appHandle;
    CsrUint8                whiteListSize;
} CsrBtCmLeGetControllerInfoReq;

typedef struct
{
    CsrBtCmPrim             type; /* CSR_BT_CM_LE_GET_CONTROLLER_INFO_CFM */
    CsrUint8                whiteListSize;
    CsrUint32               leStatesUpper;
    CsrUint32               leStatesLower;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmLeGetControllerInfoCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             appHandle;
} CsrBtCmEnableDutModeReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8                stepNumber;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmEnableDutModeCfm;


typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             appHandle;
} CsrBtCmDisableDutModeReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmDisableDutModeCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrBool                 disableInquiryScan;
    CsrBool                 disablePageScan;
} CsrBtCmWriteScanEnableReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmWriteScanEnableCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
} CsrBtCmReadScanEnableReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8                scanEnable;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmReadScanEnableCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
} CsrBtCmReadIacReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint24               iac;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmReadIacCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrUint24               iac;
} CsrBtCmWriteIacReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint24                iac;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteIacInd;

#define CSR_BT_HCI_DEFAULT_LSTO                               (0x7D00)

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    CsrUint16                timeout;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmWriteLinkSupervTimeoutReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteLinkSupervTimeoutCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBool                  connectAble;
} CsrBtCmConnectableReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint16                pageTimeout;
} CsrBtCmWritePageToReq;
typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmWritePageToCfm;

typedef struct
{
    CsrBtCmPrim     type;
    CsrBtTypedAddr  typedAddr;  /* Bluetooth address of remote device */
    DM_SM_TRUST_T   trust;      /* Update trust level */
    DM_SM_KEYS_T    *keys;      /* Security keys and requirements */
} CsrBtCmSmAddDeviceReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
} CsrBtCmSmRemoveDeviceReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmAuthenticateReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmAclCloseReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  serverChannel;
    CsrSchedQid                    phandle;
    CsrUint16                 context;
} CsrBtCmCancelAcceptConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  serverChannel;
} CsrBtCmAcceptConnectTimeout;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8                serverChannel;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmCancelAcceptConnectCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    RFC_PORTNEG_VALUES_T    portPar;
    CsrUint16               context;
} CsrBtCmPortnegReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    RFC_PORTNEG_VALUES_T    portPar;
    CsrUint16               context;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmPortnegCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    RFC_PORTNEG_VALUES_T    portPar;
    CsrBool                 request;
    CsrUint16               context;
} CsrBtCmPortnegInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    RFC_PORTNEG_VALUES_T    portPar;
} CsrBtCmPortnegRes;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  appHandle;
    CsrUint8                localServerCh;
    CsrBtUuid32             serviceHandle;
    CsrUint16               profileMaxFrameSize;
    CsrBool                 requestPortPar;
    CsrBool                 validPortPar;
    RFC_PORTNEG_VALUES_T    portPar;
    dm_security_level_t     secLevel;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint16               context;
    CsrUint8                modemStatus;
    CsrUint8                breakSignal;
    CsrUint8                mscTimeout;
} CsrBtCmConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                  localServerCh;
    CsrUint8                  remoteServerCh;
    CsrUint16                profileMaxFrameSize;
    CsrBool                  requestPortPar;
    CsrBool                  validPortPar;
    RFC_PORTNEG_VALUES_T    portPar;
    dm_security_level_t     secLevel;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                modemStatus;
    CsrUint8                breakSignal;
    CsrUint8                mscTimeout;
} CsrBtCmConnectExtReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                   localServerCh;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtCmPrim               typeToCancel;
} CsrBtCmCancelConnectReq;


#define CSR_BT_CM_SERVER_CHANNEL_DONT_CARE      0

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               phandle;
    CsrUint16                 context;
    CsrUint8                  serverChannel;
} CsrBtCmRegisterReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrUint8                serverChannel;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmRegisterCfm;

typedef struct
{    CsrBtCmPrim              type;
    CsrUint8           serverChannel;
} CsrBtCmUnregisterReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16               profileMaxFrameSize;
    CsrBool                 validPortPar;
    RFC_PORTNEG_VALUES_T    portPar;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmConnectCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                serverChannel;
    CsrUint16               profileMaxFrameSize;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmConnectAcceptCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                timeout;
    CsrUint16                profileMaxFrameSize;
    CsrUint8           serverChannel;
    dm_security_level_t     secLevel;
    CsrSchedQid                     appHandle;
    CsrUint24                classOfDevice;
    uuid16_t                profileUuid;
    CsrUint16               context;
    CsrUint8                modemStatus;
    CsrUint8                breakSignal;
    CsrUint8                mscTimeout;
    CsrBtDeviceAddr         deviceAddr;
} CsrBtCmConnectAcceptReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16               context;
} CsrBtCmDisconnectReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrBool                 status;
    CsrBool                 localTerminated;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
    CsrUint16               context;
} CsrBtCmDisconnectInd;

typedef struct
{
    hci_pkt_type_t          audioQuality;
    CsrUint32                txBandwidth;
    CsrUint32                rxBandwidth;
    CsrUint16                maxLatency;
    CsrUint16                voiceSettings;
    CsrUint8                 reTxEffort;
} CsrBtCmScoCommonParms;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                 pcmSlot;
    CsrBool                  pcmReassign;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16                parmsLen;
    CsrBtCmScoCommonParms   *parms;
} CsrBtCmScoConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    hci_pkt_type_t          audioQuality;
    CsrUint32                txBandwidth;
    CsrUint32                rxBandwidth;
    CsrUint16                maxLatency;
    CsrUint16                voiceSettings;
    CsrUint8                 reTxEffort;
} CsrBtCmScoAcceptConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    hci_pkt_type_t          audioQuality;
    CsrUint16                maxLatency;
    CsrUint8                 reTxEffort;
} CsrBtCmScoRenegotiateReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
} CsrBtCmScoCancelAcceptConnectReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                linkType;
    CsrUint8                txInterval;
    CsrUint8                weSco;
    CsrUint16               rxPacketLength;
    CsrUint16               txPacketLength;
    CsrUint8                airMode;
    hci_connection_handle_t eScoHandle;
    CsrUint8                pcmSlot;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmScoConnectCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                linkType;
    CsrUint8                txInterval;
    CsrUint8                weSco;
    CsrUint16               rxPacketLength;
    CsrUint16               txPacketLength;
    CsrUint8                airMode;
    hci_connection_handle_t eScoHandle;
    CsrUint8                pcmSlot;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmScoAcceptConnectCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId              btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                 linkType;
} CsrBtCmMapScoPcmInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrSchedQid              appHandle;
    CsrBtConnId              btConnId;
    CsrUint8                 linkType;
    dm_protocol_id_t         protocolId;
} CsrBtCmIncomingScoReq;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtConnId              btConnId;                      /* Global Bluetooth connection ID */

    hci_error_t             acceptResponse;
    CsrUint16                parmsLen;
    CsrBtCmScoCommonParms   *parms;
    CsrUint8                 pcmSlot;
    CsrBool                  pcmReassign;
} CsrBtCmMapScoPcmRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    hci_connection_handle_t eScoHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmScoRenegotiateInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    hci_connection_handle_t eScoHandle;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmScoRenegotiateCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                    appHandle;
    CsrBtConnId               btConnId;                      /* Global Bluetooth connection ID */
} CsrBtCmScoDisconnectReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    hci_connection_handle_t eScoHandle;
    CsrBool                 status;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtCmScoDisconnectInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16              payloadLength;
    CsrUint8              *payload;
} CsrBtCmDataReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16              payloadLength;
    CsrUint8              *payload;
    CsrUint16               context;
} CsrBtCmDataInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16               context;
} CsrBtCmDataCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;                      /* Global Bluetooth connection ID */
} CsrBtCmDataRes;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtConnId              btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                 modemstatus;
    CsrUint8                 break_signal;
} CsrBtCmControlReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                modemstatus;
    CsrUint8                break_signal;
    CsrUint16               context;
} CsrBtCmControlInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmRejectRfcConnectionInd;

typedef struct
{
    CsrBtCmPrim       type;
    CsrUint8          mode;
    CsrUint16         length;
    CsrBtConnId       btConnId;                      /* Global Bluetooth connection ID */
    CsrBtResultCode   resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint16         context;
} CsrBtCmRfcModeChangeInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;                      /* Global Bluetooth connection ID */
    CsrUint8                  requestedMode;
    CsrBool                   forceSniff;
} CsrBtCmRfcModeChangeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId             btConnId;                      /* Global Bluetooth connection ID */
    CsrBtSniffSettings     *sniffSettings;
    CsrUint8                 sniffSettingsSize;
    CsrBtSsrSettingsDownstream *ssrSettings;
    CsrUint8                 ssrSettingsSize;
    CsrUint8                 lowPowerPriority;
} CsrBtCmDmModeSettingsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                    phandle;
    psm_t                     localPsm;
    CsrUint16                 mode_mask;        /*!< Mask of which L2CAP modes are acceptable, see the L2CA_MODE_MASK-defines */
    CsrUint16                 flags;           /*!< Register flags. */
    CsrUint16                 context;         
} CsrBtCmL2caRegisterReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    psm_t                   localPsm;
} CsrBtCmL2caUnregisterReq;

typedef struct
{
    CsrBtCmPrim               type;
    psm_t                   localPsm;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint16               context;
} CsrBtCmL2caCancelConnectAcceptCfm;

typedef struct
{
    CsrBtCmPrim               type;
    psm_t                   localPsm;
    CsrUint16                mode_mask;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint16                 context;
} CsrBtCmL2caRegisterCfm;

typedef struct
{
    CsrBtCmPrim               type;
    psm_t                     connectionlessPsm;
    CsrUint16                 length;
    CsrUint8                 *payload;
    CsrBtDeviceAddr           deviceAddr;
} CsrBtCmL2caConnectionlessDataReq;

typedef struct
{
    CsrBtCmPrim               type;
    psm_t                     connectionlessPsm;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmL2caConnectionlessDataCfm;

typedef struct
{
    CsrBtCmPrim               type;
    psm_t                     connectionlessPsm;
    CsrUint16                 length;
    CsrUint8                 *payload;
    CsrBtDeviceAddr           deviceAddr;
} CsrBtCmL2caConnectionlessDataInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  phandle;
    CsrBtDeviceAddr         addr;
    psm_t                   localPsm;
    psm_t                   remotePsm;
    dm_security_level_t     secLevel;
    CsrUint16               context;
    CsrUint16               conftabCount;
    CsrUint16              *conftab;
} CsrBtCmL2caConnectReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  phandle;
    CsrBtDeviceAddr         deviceAddr;
    psm_t                   localPsm;
} CsrBtCmCancelL2caConnectReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid                  phandle;
    psm_t                   localPsm;
    CsrUint24               classOfDevice;
    CsrBool                 primaryAcceptor;
    dm_security_level_t     secLevel;
    uuid16_t                profileUuid;
    CsrUint16               context;
    CsrUint16               conftabCount;
    CsrUint16              *conftab;
    CsrBtDeviceAddr         deviceAddr;
} CsrBtCmL2caConnectAcceptReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;
    psm_t                   localPsm;
    l2ca_mtu_t              mtu;
    l2ca_mtu_t              localMtu;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmL2caConnectCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;
    psm_t                   localPsm;
    psm_t                   remotePsm;
    l2ca_mtu_t              mtu;
    l2ca_mtu_t              localMtu;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               context;
} CsrBtCmL2caConnectAcceptCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;
    CsrUint16               length;
    CsrMblk                 *payload;
    CsrUint16               context;
} CsrBtCmL2caDataReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
} CsrBtCmL2caDataRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
} CsrBtCmL2caDataAbortReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
    CsrUint16                 context;
} CsrBtCmL2caDisconnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
    CsrBool                   localTerminated;
    CsrBtReasonCode           reasonCode;
    CsrBtSupplier             reasonSupplier;
    CsrUint16                 context;
} CsrBtCmL2caDisconnectInd;

typedef struct
{
    CsrBtCmPrim                type;
    CsrBtConnId                btConnId;
    CsrBtAmpController         remoteControl;   /*!< Remote controller ID */
    CsrBtAmpController         localControl;    /*!< Local controller ID */
} CsrBtCmMoveChannelReq;

typedef struct
{
    CsrBtCmPrim                type;
    CsrBtConnId                btConnId;
    CsrBtAmpController         localControl;    /*!< Local controller ID actually used */
    CsrBtDeviceAddr            deviceAddr;      /*!< Peer address */
    CsrBtResultCode            resultCode;
    CsrBtSupplier              resultSupplier;
} CsrBtCmMoveChannelCfm;

typedef CsrBtCmMoveChannelCfm CsrBtCmMoveChannelCmpInd;

typedef struct
{
    CsrBtCmPrim                type;
    CsrBtConnId                btConnId;
    CsrBtAmpController         localControl;    /*!< Local controller ID actually used */
    CsrBtDeviceAddr            deviceAddr;      /*!< Peer address */
} CsrBtCmMoveChannelInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
    CsrBool                   accept;
} CsrBtCmMoveChannelRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId               btConnId;
    CsrUint16                 length;
    CsrUint8                  *payload;
    CsrUint16                 context;
} CsrBtCmL2caDataInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtConnId              btConnId;
    CsrUint16                context;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmL2caDataCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId              btConnId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint16                context;
} CsrBtCmL2caDataAbortCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtConnId             btConnId;
    CsrUint8                requestedMode;
    CsrBool                 forceSniff;
} CsrBtCmL2caModeChangeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId              btConnId;
    CsrUint8                 mode;
    CsrUint16                length;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
    CsrUint16                context;
} CsrBtCmL2caModeChangeInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId             btConnId;
} CsrBtCmL2caCommonPrim;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmAclOpenReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBool                  encryptionMode;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmEncryptionReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    psm_t                   localPsm;
    CsrUint16               context;
} CsrBtCmL2caCancelConnectAcceptReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtConnId              btConnId;
    CsrBtSniffSettings        *sniffSettings;
    CsrUint8                 sniffSettingsSize;
    CsrBtSsrSettingsDownstream *ssrSettings;
    CsrUint8                 ssrSettingsSize;
    CsrUint8                 lowPowerPriority;
} CsrBtCmDmL2caModeSettingsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBool                  disableExtended;
    CsrBool                  manualBridge;
    CsrBool                  disableStack;
    CsrSchedQid                     phandle;
    CsrBtDeviceAddr            deviceAddr;
    psm_t                   localPsm;
} CsrBtCmBnepRegisterReq;

typedef struct
{
    CsrBtCmPrim               type;
    BNEP_CONNECT_REQ_FLAGS  flags;
    ETHER_ADDR              rem_addr; /* set msw to ETHER_UNKNOWN for passive */
    CsrUint16                profileMaxFrameSize;
    dm_security_level_t     secLevel;
} CsrBtCmBnepConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    ETHER_ADDR              rem_addr;
    CsrUint16                rem_uuid;
    CsrUint16                loc_uuid;
    CsrUint16                profileMaxFrameSize;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmBnepConnectInd;

typedef struct
{
    CsrBtCmPrim               type;
    ETHER_ADDR              rem_addr;
} CsrBtCmCancelBnepConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    BNEP_CONNECT_REQ_FLAGS  flags;
    ETHER_ADDR              rem_addr;
    dm_security_level_t     secLevel;
    CsrUint24                classOfDevice;
    CsrUint16                profileMaxFrameSize;
} CsrBtCmBnepConnectAcceptReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmBnepConnectAcceptCfm;

typedef struct
{
    CsrBtCmPrim               type;
} CsrBtCmBnepCancelConnectAcceptReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmBnepCancelConnectAcceptCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                idNot;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr; /* note may be multicast */
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtCmBnepExtendedMulticastDataReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr; /* note may be multicast */
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtCmBnepExtendedDataInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrUint16                etherType;
    ETHER_ADDR              dstAddr; /* note may be multicast */
    ETHER_ADDR              srcAddr; /* should be a PANU's address, but who knows? */
    CsrUint16                length;
    CsrUint8                 *payload;
} CsrBtCmBnepExtendedDataReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
} CsrBtCmBnepExtendedDataCfm;

typedef struct
{
    CsrUint16                type;
    CsrUint16                flags;
    CsrUint16                id;
} CsrBtCmBnepDisconnectReq;

typedef CsrBtCmBnepDisconnectReq CsrBtCmBnepDisconnectRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier     reasonSupplier;
} CsrBtCmBnepDisconnectInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrUint8                 requestedMode;
} CsrBtCmBnepModeChangeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrUint8                 mode;
    CsrUint16                length;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmBnepModeChangeInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                id;
    CsrUint8                 role;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmBnepSwitchRoleInd;

typedef struct
{
    CsrBtCmPrim                type;
    CsrUint16                  id;
    CsrBtSniffSettings         *sniffSettings;
    CsrUint8                  sniffSettingsSize;
    CsrBtSsrSettingsDownstream *ssrSettings;
    CsrUint8                 ssrSettingsSize;
    CsrUint8                 lowPowerPriority;
} CsrBtCmDmBnepModeSettingsReq;

typedef struct 
{
    CsrBtCmPrim         type;
    CsrSchedQid         appHandle;
    CsrBtDeviceAddr     deviceAddr;
    CsrBool             useDefaultQos;
} CsrBtCmDmHciQosSetupReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                 pageNum;
    CsrBtDeviceAddr            bd_addr;
} CsrBtCmReadRemoteExtFeaturesReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmReadRemoteFeaturesReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  pageNum;
    CsrUint8                  maxPageNum;
    CsrUint16                 extLmpFeatures[4];
    CsrBtDeviceAddr           bd_addr;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadRemoteExtFeaturesCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrUint8                  map[CSR_BT_CM_AFH_HOST_CHANNEL_CLASSIFICATION_SIZE];
} CsrBtCmSetAfhChannelClassReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmSetAfhChannelClassCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           bd_addr;
} CsrBtCmReadAfhChannelMapReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  mode;
    CsrUint8                  afhMap[10];
    CsrBtDeviceAddr           bd_addr;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadAfhChannelMapCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
} CsrBtCmReadAfhChannelAssessmentModeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  classMode;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadAfhChannelAssessmentModeCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrUint8                  classMode;
} CsrBtCmWriteAfhChannelAssessmentModeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmWriteAfhChannelAssessmentModeCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrUint8                  pageNum;
} CsrBtCmReadLocalExtFeaturesReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  pageNum;
    CsrUint8                  maxPageNum;
    CsrUint8                  extLmpFeatures[8];
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadLocalExtFeaturesCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrUint8                  whichClock;
    CsrBtDeviceAddr           bd_addr;
} CsrBtCmReadClockReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           bd_addr;
    CsrUint32                 clock;
    CsrUint16                 accuracy;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadClockCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
} CsrBtCmReadLocalVersionReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                  lmpVersion;
    hci_version_t             hciVersion;
    CsrUint16                 hciRevision;
    CsrUint16                 manufacturerName;
    CsrUint16                 lmpSubversion;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadLocalVersionCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           deviceAddr;
    CsrBool                   setupLinkPolicySetting;
    link_policy_settings_t    linkPolicySetting;
    CsrUint8                  sniffSettingsCount; /* Number of _items_ in sniffSettings, _not_ byte size */
    CsrBtSniffSettings       *sniffSettings;
    CsrUint8                  parkSettingsCount; /* Number of _items_ in parkSettings, _not_ byte size */
    CsrBtParkSettings        *parkSettings;
} CsrBtCmWriteLinkPolicyReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid               appHandle;
    CsrBtDeviceAddr           deviceAddr;
} CsrBtCmReadLinkPolicyReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrUint8                  actualMode;
    link_policy_settings_t    linkPolicySetting;
    CsrBtSniffSettings        sniffSettings;
    CsrBtParkSettings         parkSettings;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmReadLinkPolicyCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteLinkPolicyErrorInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBool                   cancelInitiated;
} CsrBtCmConnectionRejSecurityInd;

typedef struct
{
    CsrBtCmPrim               type;
} CsrBtCmSmHouseCleaning;

typedef struct
{
    CsrBtCmPrim               type;
} CsrBtCmDmHouseCleaning;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBool                  selectInquiryFilter; /* Select inquiry or connection filter */
    CsrUint8                 autoAccept;          /* Auto Accept flag, see hci.h for values */
    CsrBtDeviceAddr            address;
} CsrBtCmSetEventFilterBdaddrReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSetEventFilterBdaddrCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBool                  selectInquiryFilter; /* Select inquiry or connection filter */
    CsrUint8                 autoAccept;          /* Auto Accept flag, see hci.h for values */
    CsrUint24                cod;
    CsrUint24                codMask;
} CsrBtCmSetEventFilterCodReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSetEventFilterCodCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                 filter;
} CsrBtCmClearEventFilterReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmClearEventFilterCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                 activePlayer;
    CsrBtDeviceAddr            devAddr;
    CsrUint16                clockOffset;
    page_scan_mode_t        pageScanMode;
    page_scan_rep_mode_t    pageScanRepMode;
} CsrBtCmDmWriteCacheParamsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            devAddr;
} CsrBtCmDmUpdateAndClearCachedParamReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmCancelConnectReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBool                  alwaysSupportMasterRole;
} CsrBtCmAlwaysSupportMasterRoleReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmReadEncryptionStatusReq;

typedef struct
{
    CsrBtCmPrim       type;
    CsrUint16         encrypted;         /* Encrypt type ENCR_NONE, E0 (0x01) or AES-CCM (0x02)*/
    CsrBtResultCode   resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmReadEncryptionStatusCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint16                interval;
    CsrUint16                window;
} CsrBtCmWritePagescanSettingsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWritePagescanSettingsCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                 scanType;
} CsrBtCmWritePagescanTypeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWritePagescanTypeCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint16                interval;
    CsrUint16                window;
} CsrBtCmWriteInquiryscanSettingsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteInquiryscanSettingsCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrUint8                 scanType;
} CsrBtCmWriteInquiryscanTypeReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteInquiryscanTypeCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmCancelReadRemoteNameReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmBondingReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBool                   force;
    CsrBtAddressType          addressType;
} CsrBtCmSmBondingCancelReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint16                writeAuthEnable;
    CsrUint16                config;
} CsrBtCmSmSecModeConfigReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmCommonRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrUint8                  ioCapability;
    CsrUint8                  authenticationRequirements;
    CsrUint8                  oobDataPresent;
    CsrUint8                  *oobHashC;
    CsrUint8                  *oobRandR;
    CsrUint16                 keyDistribution;
} CsrBtCmSmIoCapabilityRequestRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    hci_error_t               reason;
} CsrBtCmSmIoCapabilityRequestNegRes;

typedef CsrBtCmSmIoCapabilityRequestNegRes CsrBtCmSmUserConfirmationRequestRes;
typedef CsrBtCmSmIoCapabilityRequestNegRes CsrBtCmSmUserConfirmationRequestNegRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrUint32                 numericValue;
} CsrBtCmSmUserPasskeyRequestRes;

typedef CsrBtCmSmUserPasskeyRequestRes CsrBtCmSmUserPasskeyRequestNegRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtTransportType        transportType;
} CsrBtCmSmReadLocalOobDataReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtAddressType          addressType;
    CsrBtTransportType        transportType;
    CsrUint8                  notificationType;
} CsrBtCmSmSendKeypressNotificationReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint16               repairId;
    CsrBtAddressType        addressType;
} CsrBtCmSmRepairInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint16                repairId;
    CsrBool                  accept;
    CsrBtAddressType         addressType;
} CsrBtCmSmRepairRes;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmSmReadDeviceReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint32                enhancements;
} CsrBtCmEnEnableEnhancementsReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint16                flushTo;
} CsrBtCmDmWriteAutoFlushTimeoutReq;

#define CSR_BT_CM_ROLE_SWITCH_DEFAULT                  (0x00000000) /* Try and do RS before SCO connections or if more than 1 ACL exists   */
#define CSR_BT_CM_ROLE_SWITCH_ALWAYS                   (0x00000001) /* Always do RS                                                        */
#define CSR_BT_CM_ROLE_SWITCH_NOT_BEFORE_SCO           (0x00000002) /* Don't do RS before SCO setup                                        */
#define CSR_BT_CM_ROLE_SWITCH_BEFORE_RNR               (0x00000003) /* Do RS before RNR                                                    */
#define CSR_BT_CM_ROLE_SWITCH_BEFORE_SCO               (0x00000004) /* Do RS before SCO setup                                              */
#define CSR_BT_CM_ROLE_SWITCH_NOT_BEFORE_RNR           (0x00000005) /* Don't do RS before RNR                                              */
#define CSR_BT_CM_ROLE_SWITCH_ALWAYS_ACL               (0x00000006) /* Do RS every time an ACL is establish                                */
#define CSR_BT_CM_ROLE_SWITCH_MULTIPLE_ACL             (0x00000007) /* Only do RS if more then 1 ACL exitst                                */

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint32                config;
} CsrBtCmRoleSwitchConfigReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrSchedQid                     appHandle;
} CsrBtCmReadFailedContactCounterReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint16                failedContactCount;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmReadFailedContactCounterCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     phandle;
    CsrUint32                eventMask;
    CsrUint32                conditionMask;
} CsrBtCmSetEventMaskReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint32                eventMask;
} CsrBtCmSetEventMaskCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint16                lsto;
} CsrBtCmLstoChangeInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 mode;
    CsrUint16                interval;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmModeChangeInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 role;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmRoleChangeInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrBool                  incoming;
    hci_connection_handle_t  syncHandle;
    CsrUint8                 linkType;
    CsrUint8                 txInterval;
    CsrUint8                 weSco;
    CsrUint16                rxPacketLength;
    CsrUint16                txPacketLength;
    CsrUint8                 airMode;
    hci_pkt_type_t           packetType;
    CsrUint32                txBdw;
    CsrUint32                rxBdw;
    CsrUint16                maxLatency;
    CsrUint8                 reTxEffort;
    CsrUint16                voiceSettings;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtCmSyncConnectInd;

typedef CsrBtCmSyncConnectInd CsrBtCmSyncRenegotiateInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrBool                  incoming;
    hci_connection_handle_t  syncHandle;
    CsrUint8                 linkType;
    CsrUint8                 txInterval;
    CsrUint8                 weSco;
    CsrUint8                 reservedSlots;
    CsrUint16                rxPacketLength;
    CsrUint16                txPacketLength;
    CsrUint8                 airMode;
    hci_pkt_type_t           packetType;
    CsrUint32                txBdw;
    CsrUint32                rxBdw;
    CsrUint16                maxLatency;
    CsrUint8                 reTxEffort;
    CsrUint16                voiceSettings;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtCmExtSyncConnectInd;


typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    hci_connection_handle_t syncHandle;
    hci_reason_t            reason;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSyncDisconnectInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrUint8                  incoming;
    CsrBtClassOfDevice        cod;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
    CsrUint16                 aclConnHandle;
} CsrBtCmAclConnectInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr           deviceAddr;
    CsrBtReasonCode           reasonCode;
    CsrBtSupplier             reasonSupplier;
} CsrBtCmAclDisconnectInd;

typedef struct
{
    CsrBtCmPrim               type;
} CsrBtCmBluecoreInitializedInd;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtCmBluecoreDeinitializedInd;

typedef struct
{
    CsrBtCmPrim              type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint16                maxTxLatency;
    CsrUint16                maxRxLatency;
    CsrUint16                minRemoteTimeout;
    CsrUint16                minLocalTimeout;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtCmSniffSubRatingInd;

typedef struct
{
    CsrBtCmPrim               type; /* Identity: CSR_BT_CM_LOCAL_NAME_CHANGE_IND */
    CsrUtf8String            *localName;
} CsrBtCmLocalNameChangeInd;

/* Low Energy COEX event. Subscribe via the
 * CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOW_ENERGY event mask */
typedef struct
{
    CsrBtCmPrim               type;        /* Identity */
    CsrUint8                  event;       /* State, use CSR_BT_CM_LE_MODE_... */
    CsrUint8                  advType;     /* Type of advertising, use CSR_BT_CM_LE_ADVTYPE_ */
    CsrUint16                 intervalMin; /* Minimum advertising interval (in slots, i.e. x * 0.625ms) */
    CsrUint16                 intervalMax; /* Maximum advertising interval (in slots, i.e. x * 0.625ms) */
    CsrUint8                  channelMap;  /* Advertising channel map, use CSR_BT_CM_LE_CHANMAP_ */
} CsrBtCmLeEventAdvertisingInd;

/* Low Energy COEX event. Subscribe via the
 * CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOW_ENERGY event mask */

typedef struct
{
    CsrBtCmPrim               type;      /* Identity */
    CsrUint8                  event;     /* State, use CSR_BT_CM_LE_MODE_... */
    CsrUint8                  scanType;  /* Type of scanning, use CSR_BT_CM_LE_SCANTYPE_ */
    CsrUint16                 interval;  /* Scan interval (in slots, i.e. x * 0.625ms) */
    CsrUint16                 window;    /* Scan window (in slots, i.e. x * 0.625ms) */
} CsrBtCmLeEventScanInd;

/* Low Energy COEX event. Subscribe via the
 * CSR_BT_CM_EVENT_MASK_SUBSCRIBE_LOW_ENERGY event mask */
typedef struct
{
    CsrBtCmPrim               type;       /* Identity */
    CsrUint8                  event;      /* State, use CSR_BT_CM_LE_MODE_... */
    CsrBtTypedAddr            deviceAddr; /* Peer device address */
    CsrUint8                  role;       /* Role, use CSR_BT_CM_ROLE_... */
    CsrUint16                 interval;   /* Connection interval (in slots, i.e. x * 0.625ms) */
    CsrUint16                 timeout;    /* Supervision timeout (in 10ms units) */
    CsrUint16                 latency;    /* Connection latency (in slots, i.e. x * 0.625ms) */
    CsrUint8                  accuracy;   /* Clock accurary, use CSR_BT_CM_LE_CLOCKACCU_ */
} CsrBtCmLeEventConnectionInd;

#define CSR_BT_CM_MODE_CHANGE_DISABLE  (0) /* Synergy BT is controlling all low power handling. Default setting */
#define CSR_BT_CM_MODE_CHANGE_ENABLE   (1) /* The Application is controlling all low power handling       */

typedef struct
{
    CsrBtCmPrim              type;
    CsrSchedQid              phandle;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint32                config;
} CsrBtCmModeChangeConfigReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmModeChangeConfigCfm;

typedef struct
{
    CsrBtCmPrim             type;
    CsrSchedQid             phandle;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                mode;
    CsrBool                 forceSniffSettings;
    CsrBtSniffSettings      sniffSettings;
} CsrBtCmModeChangeReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                mode;
    CsrUint16               interval;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmModeChangeCfm;


typedef CsrUint32  CsrBtLogicalChannelType;
#define CSR_BT_NO_ACTIVE_LOGICAL_CHANNEL        ((CsrBtLogicalChannelType)0x00000000)
#define CSR_BT_ACTIVE_DATA_CHANNEL              ((CsrBtLogicalChannelType)0x00000001)
#define CSR_BT_ACTIVE_CONTROL_CHANNEL           ((CsrBtLogicalChannelType)0x00000002)
#define CSR_BT_ACTIVE_STREAM_CHANNEL            ((CsrBtLogicalChannelType)0x00000004)

typedef struct
{
    CsrBtCmPrim     type;
    CsrBtLogicalChannelType     logicalChannelTypeMask;
    CsrBtDeviceAddr deviceAddr;
    CsrBtConnId     btConnId;                      /* Global Bluetooth connection ID */
}CsrBtCmLogicalChannelTypeReq;

typedef struct
{
    CsrBtCmPrim                 type;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtLogicalChannelType     logicalChannelTypeMask;
    CsrUint8                    numberOfGuaranteedLogicalChannels;
} CsrBtCmLogicalChannelTypesInd;


typedef CsrUint32 CsrBtCmRoleType;
#define CSR_BT_CM_SWITCH_ROLE_TYPE_INVALID     ((CsrBtCmRoleType) 0x00)
#define CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT     ((CsrBtCmRoleType) 0x01)
#define CSR_BT_CM_SWITCH_ROLE_TYPE_MAX         ((CsrBtCmRoleType) 0x01)

typedef struct
{
    CsrBtCmPrim               type;
    CsrSchedQid                     appHandle;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 role;
    CsrBtCmRoleType          roleType;
    CsrUint32                config;     /*! < RFU */
} CsrBtCmSwitchRoleReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 role;
    CsrBtCmRoleType          roleType;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmSwitchRoleCfm;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                 role;
    CsrUint16                id;
} CsrBtCmBnepSwitchRoleReq;

typedef struct
{
    CsrBtCmPrim               type;
    CsrUint8                 role;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtCmPrivateSwitchRoleReq;

typedef struct
{
    dm_prim_t               type;
    CsrSchedQid                     phandle;
    CsrUint16                voiceSettings;
} CsrBtCmWriteVoiceSettingsReq;

typedef struct
{
    dm_prim_t               type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtCmWriteVoiceSettingsCfm;

typedef struct
{
    dm_prim_t     type;
    CsrSchedQid   phandle;
    CsrBtTpdAddrT tpAddrt;
    CsrUint16     authPayloadTimeout;
} CsrBtCmWriteAuthPayloadTimeoutReq;

typedef struct
{
    dm_prim_t        type;
    CsrBtTpdAddrT    tpAddrt;
    CsrBtResultCode  resultCode;
    CsrBtSupplier    resultSupplier;
} CsrBtCmWriteAuthPayloadTimeoutCfm;

typedef struct
{
    CsrBtCmPrim             type;
} CsrBtCmSmAccessReq;

typedef struct
{
    CsrBtCmPrim             type; /* Identity: CSR_BT_CM_REGISTER_HANDLER_REQ */
    CsrUint8                handlerType;
    CsrSchedQid             handle;
    CsrUint32               flags;
} CsrBtCmRegisterHandlerReq;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                remoteLmpFeatures[8];
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmRemoteFeaturesInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                lmpVersion;
    CsrUint16               manufacturerName;
    CsrUint16               lmpSubversion;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtCmRemoteVersionInd;

typedef struct
{
    CsrBtCmPrim             type;
    CsrBtDeviceAddr         deviceAddr;     /* address of the remote device*/
    CsrUint8                streamIdx;      /* unique Id for the stream in question */
    CsrUint32               bitRate;        /* bit rate used */
} CsrBtCmA2dpBitRateInd;
typedef CsrBtCmA2dpBitRateInd CsrBtCmA2dpBitRateReq;

#ifndef EXCLUDE_CSR_BT_CME_BH_FEATURE
typedef struct
{
    CsrBtCmPrim     type;
    CsrUint16       aclHandle; /* Identifies the ACL Link */
    CsrUint16       l2capConnectionId; /* Identifies the local L2CAP channel ID */
    CsrUint16       bitRate; /* Identifies the bit rate of the codec in kbps */
    CsrUint16       sduSize; /* Identifies the L2CAP MTU negotiated for av */
    CsrUint8        period; /* Identifies the period in ms of codec data being available for transmission */
    CsrUint8        role; /* Identifies the local device role, source or sink */
    CsrUint8        samplingFreq; /* Identifies the sampling frequency of audio codec used */
    CsrUint8        codecType; /* Identifies the codec type e.g. SBC/aptX etc */
    CsrUint8        codecLocation; /* Identifies the location of the codec on/off-chip*/
    CsrUint8        streamIndex; /* unique id for a stream */
    CsrBool         start; /* identifies start/stop of av stream */
} CsrBtCmSetAvStreamInfoReq;
#endif /* EXCLUDE_CSR_BT_CME_BH_FEATURE */

typedef CsrUint8 CsrBtCmInquiryEventType;
#define CSR_BT_CM_INQUIRY_TYPE_START            ((CsrBtCmInquiryEventType) 0x00) /* Inquiry ongoing */
#define CSR_BT_CM_INQUIRY_TYPE_STOP             ((CsrBtCmInquiryEventType) 0x01) /* Inquiry not ongoing */

typedef CsrUint8 CsrBtCmPagingEventType;
#define CSR_BT_CM_PAGE_TYPE_START            ((CsrBtCmPagingEventType) 0x00)     /* Paging ongoing */
#define CSR_BT_CM_PAGE_TYPE_STOP             ((CsrBtCmPagingEventType) 0x01)     /* Paging not ongoing */

typedef struct
{
    CsrBtCmPrim                   type;
    CsrBtCmInquiryEventType       inquiry;
    CsrBtCmPagingEventType        paging;
} CsrBtCmInquiryPageEventInd;

typedef struct
{
    CsrBtCmPrim                   type;
    CsrUint8                      lmpVersion;
} CsrBtCmGetSecurityConfInd;

typedef struct
{
    CsrBtCmPrim                   type;
    CsrUint16                     options;
    dm_security_mode_t            securityMode;
    dm_security_level_t           securityLevelDefault;
    CsrUint16                     config;
    CsrUint16                     writeAuthEnable;
    CsrUint8                      mode3enc;
    CsrUint8                      leErCount; /* count either 0 or 8 */
    CsrUint16                    *leEr; /* count either 0 or 8 */
    CsrUint8                      leIrCount; /* count either 0 or 8 */
    CsrUint16                    *leIr; /* count either 0 or 8 */
    CsrUint16                     leSmDivState;
    CsrUint32                     leSmSignCounter;
} CsrBtCmGetSecurityConfRes;

typedef struct
{
    CsrBtCmPrim                   type;
    CsrBtDeviceAddr               deviceAddr;
    CsrBtAddressType              deviceAddrType;
    CsrBtTransportType            transportType;
    CsrUint16                     encryptType;
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmEncryptChangeInd;

typedef struct
{
    dm_prim_t               type;
    CsrSchedQid             phandle;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint16               operation;
} CsrBtCmAlwaysMasterDevicesReq;

typedef struct
{
    CsrBtCmPrim         type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtCmAlwaysMasterDevicesCfm;

typedef struct
{
    dm_prim_t               type; /* CSR_BT_CM_SM_LE_SECURITY_REQ */
    CsrBtTypedAddr          addr;  
    CsrUint16               context;
    CsrUint16               securityRequirements;
    CsrUint16               l2caConFlags;
} CsrBtCmSmLeSecurityReq;

typedef struct
{
    dm_prim_t               type; /* CSR_BT_CM_SM_SET_ENCRYPTION_KEY_SIZE_REQ */
    CsrUint8                minKeySize;
    CsrUint8                maxKeySize;
} CsrBtCmSmSetEncryptionKeySizeReq;

typedef struct
{
    CsrBtCmPrim         type;
    CsrBtConnId         btConnId;                      /* Global Bluetooth connection ID */
} CsrBtCmDataBufferEmptyReq;

typedef struct
{
    CsrBtCmPrim         type;
    CsrUint16           context;
} CsrBtCmDataBufferEmptyCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_SCAN_REQ */
    CsrSchedQid                   appHandle;
    CsrUint16                     context;
    CsrUint8                      mode; /* use CSR_BT_CM_LE_MODE_ */
    CsrUint8                      scanType;
    CsrUint16                     scanInterval;
    CsrUint16                     scanWindow;
    CsrUint8                      scanningFilterPolicy;
    CsrUint8                      filterDuplicates;
    CsrUint16                     addressCount;
    CsrBtTypedAddr                *addressList;
} CsrBtCmLeScanReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_SCAN_CFM */
    CsrUint16                     context;  
    CsrUint8                      scanMode; /* use CSR_BT_CM_LE_MODE_ */
    CsrBool                       whiteListEnable; /* TRUE enable, FALSE disable */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeScanCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_ADVERTISE_REQ */
    CsrSchedQid                   appHandle;
    CsrUint16                     context;
    CsrUint8                      mode; /* use CSR_BT_CM_LE_MODE_ */
    CsrUint8                      paramChange; /* use CSR_BT_CM_LE_PARCHG_ */ 
    CsrUint8                      advertisingDataLength; /* max 31 */
    CsrUint8                     *advertisingData;
    CsrUint8                      scanResponseDataLength; /* max 31 */
    CsrUint8                     *scanResponseData;
    CsrUint16                     advIntervalMin;
    CsrUint16                     advIntervalMax;
    CsrUint8                      advertisingType;
    CsrUint8                      advertisingChannelMap;
    CsrUint8                      advertisingFilterPolicy;
    CsrUint16                     whitelistAddrCount;
    CsrBtTypedAddr               *whitelistAddrList;
} CsrBtCmLeAdvertiseReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_ADVERTISE_CFM */
    CsrUint16                     context;
    CsrUint8                      advMode; /* use CSR_BT_CM_LE_MODE_ */
    CsrUint8                      advType;
    CsrBool                       whiteListEnable; /* TRUE enable, FALSE disable */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeAdvertiseCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_REPORT_IND */
    CsrUint8                      eventType;
    CsrBtTypedAddr                address; /* current address */
    CsrBtTypedAddr                permanentAddress; /* permanent address (resolved) */
    CsrUint8                      lengthData;
    CsrUint8                      data[CSR_BT_CM_LE_MAX_REPORT_LENGTH];
    CsrInt8                       rssi;
} CsrBtCmLeReportInd;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_SM_KEY_REQUEST_RES */
    CsrBtDeviceAddr               address;
    CsrUint16                     securityRequirements;
    CsrUint16                     keyType;
    CsrUint16                     keySize;
    CsrUint8                     *key; /* points to DM_SM_KEY structure */
    CsrBtAddressType              addressType;
} CsrBtCmSmKeyRequestRes;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_CONNPARAM_REQ */
    CsrSchedQid                   appHandle;
    CsrUint16                     scanInterval;            /* These six parameters correspond to */
    CsrUint16                     scanWindow;              /* those in HCI_ULP_CREATE_CONNECTION_T */
    CsrUint16                     connIntervalMin;         /* and are used to fill that primitive */
    CsrUint16                     connIntervalMax;         /* when it is created. If they are not */
    CsrUint16                     connLatency;             /* set explicitly here then the Device */
    CsrUint16                     supervisionTimeout;      /* Manager will choose defaults. */
    CsrUint16                     connLatencyMax;          /* The 3 parameters are used to define the conditions */
    CsrUint16                     supervisionTimeoutMin;   /* to accept a  */
    CsrUint16                     supervisionTimeoutMax;   /* L2CA_CONNECTION_PAR_UPDATE_REQ_T*/
} CsrBtCmLeConnparamReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_CONNPARAM_CFM */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeConnparamCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_CONNPARAM_UPDATE_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                address;
    CsrUint16                     connIntervalMin;
    CsrUint16                     connIntervalMax;
    CsrUint16                     connLatency;
    CsrUint16                     supervisionTimeout;
    CsrUint16                     minimumCeLength;
    CsrUint16                     maximumCeLength;
} CsrBtCmLeConnparamUpdateReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_CONNPARAM_UPDATE_CMP_IND */
    CsrBtTypedAddr                address;
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeConnparamUpdateCmpInd;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_ACCEPT_CONNPARAM_UPDATE_IND */
    CsrBtTypedAddr                address; /* Bluetooth device address  of remote device */
    l2ca_identifier_t             l2caSignalId; /* L2CAP Signal ID */
    CsrUint16                     connIntervalMin; /* Minimum allowed connection interval */
    CsrUint16                     connIntervalMax; /* Maximum allowed connection interval */
    CsrUint16                     connLatency; /* connection slave latency */
    CsrUint16                     supervisionTimeout; /* link timeout value */
} CsrBtCmLeAcceptConnparamUpdateInd;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_ACCEPT_CONNPARAM_UPDATE_RES */
    CsrBtTypedAddr                address; /* Bluetooth device address  of remote device */
    l2ca_identifier_t             l2caSignalId; /* L2CAP Signal ID */
    CsrUint16                     connIntervalMin; /* Minimum allowed connection interval */
    CsrUint16                     connIntervalMax; /* Maximum allowed connection interval */
    CsrUint16                     connLatency; /* connection slave latency */
    CsrUint16                     supervisionTimeout; /* link timeout value */
    CsrBool                       accept; /* TRUE - if parameters are acceptable, FALSE - non-acceptable  */ 
} CsrBtCmLeAcceptConnparamUpdateRes;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_READ_ENCRYPTION_KEY_SIZE_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                address;
    CsrUint16                     context;
} CsrBtCmReadEncryptionKeySizeReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_READ_ENCRYPTION_KEY_SIZE_CFM */
    CsrBtTypedAddr                address;
    CsrUint16                     keySize;
    CsrUint16                     context;
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmReadEncryptionKeySizeCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_READ_ADVERTISING_CH_TX_POWER_REQ */
    CsrSchedQid                   appHandle;
    CsrUint16                     context;
} CsrBtCmReadAdvertisingChTxPowerReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_READ_ADVERTISING_CH_TX_POWER_CFM */
    CsrInt8                       txPower; /* Tx power in dBm */
    CsrUint16                     context;
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmReadAdvertisingChTxPowerCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_RECEIVER_TEST_REQ */
    CsrSchedQid                   appHandle;
    CsrUint8                      rxFrequency; /* n=(f-2402)/2, range 0x00-0x27 */
} CsrBtCmLeReceiverTestReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_RECEIVER_TEST_CFM */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;   
} CsrBtCmLeReceiverTestCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_TRANSMITTER_TEST_REQ */
    CsrSchedQid                   appHandle;
    CsrUint8                      txFrequency; /* n=(f-2402)/2, range 0x00-0x27 */
    CsrUint8                      lengthOfTestData; /* range 0x00-0x25 */
    CsrUint8                      packetPayload; /* Pattern code, see BT4.0 volume 2 part E section 7.8.30 */
} CsrBtCmLeTransmitterTestReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_TRANSMITTER_TEST_CFM */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;   
} CsrBtCmLeTransmitterTestCfm;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_TEST_END_REQ */
    CsrSchedQid                   appHandle;
} CsrBtCmLeTestEndReq;

typedef struct
{
    CsrBtCmPrim                   type; /* Identity: CSR_BT_CM_LE_TEST_END_CFM */
    CsrUint16                     numberOfPackets; /* number of packets Rx'ed, 0 for Tx */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;   
} CsrBtCmLeTestEndCfm;


typedef struct
{
    CsrBtCmPrim                   type;            /* Identity: CSR_BT_CM_L2CA_GET_CHANNEL_INFO_REQ */
    CsrBtConnId                   btConnId;        /* Global Bluetooth connection ID */
    CsrSchedQid                   appHandle;       /* ID of the app to answer to */
} CsrBtCmL2caGetChannelInfoReq;

typedef struct
{
    CsrBtCmPrim                   type;            /* Identity: CSR_BT_CM_L2CA_GET_CHANNEL_INFO_CFM */
    CsrBtConnId                   btConnId;        /* Global Bluetooth connection ID */
    CsrUint16                     aclHandle;       /* ACL connection ID */
    CsrUint16                     remoteCid;       /* remote CID */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;   
} CsrBtCmL2caGetChannelInfoCfm;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_PHYSICAL_LINK_STATUS_REQ */
    CsrBtTypedAddr                address;  /* Peer address */
    CsrBool                       radioType;/* TRUE = LE, FALSE = BREDR */
    CsrBool                       status;   /* TRUE = connected, FALSE = disconnected */   
} CsrBtCmLePhysicalLinkStatusReq;

typedef struct
{
    CsrBtCmPrim                   type;        /* Identity: CSR_BT_CM_HIGH_PRIORITY_DATA_IND */
    CsrBtDeviceAddr               deviceAddr;  /* Peer address */
    CsrBool                       start;       /* TRUE = start sending high priority data, FALSE = stopped sending high priority data */
} CsrBtCmHighPriorityDataInd;

typedef struct
{
    CsrBtCmPrim                   type;            /* Identity: CSR_BT_CM_L2CA_AMP_MOVE_IND */
    CsrBtConnId                   btConnId;        /* Global Bluetooth connection ID */
    CsrBtAmpController            localControl;    /*!< Local controller ID actually used */  
    CsrUint16                     context;
} CsrBtCmL2caAmpMoveInd;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_LOCK_SM_QUEUE_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                address;  /* Peer address */
} CsrBtCmLeLockSmQueueReq;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_UNLOCK_SM_QUEUE_REQ */
    CsrBtTypedAddr                address;  /* Peer address */
} CsrBtCmLeUnlockSmQueueReq;

typedef CsrBtCmLeUnlockSmQueueReq CsrBtCmLeLockSmQueueInd;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_READ_REMOTE_USED_FEATURES_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                address;  /* Peer Address */
}CsrBtCmLeReadRemoteUsedFeaturesReq;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_READ_REMOTE_USED_FEATURES_CFM */
    CsrBtTypedAddr                address;  /* Peer Address */
    CsrUint8                      remoteLeFeatures[8];
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
}CsrBtCmLeReadRemoteUsedFeaturesCfm;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_READ_LOCAL_SUPPORTED_FEATURES_REQ */
    CsrSchedQid                   appHandle;
}CsrBtCmLeReadLocalSupportedFeaturesReq;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_READ_LOCAL_SUPPORTED_FEATURES_CFM */
    CsrUint8                      localLeFeatures[8];
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
}CsrBtCmLeReadLocalSupportedFeaturesCfm;

typedef struct
{
    CsrBtCmPrim                   type;     /* Identity: CSR_BT_CM_LE_READ_RESOLVING_LIST_SIZE_REQ */
    CsrSchedQid                   appHandle;
} CsrBtCmLeReadResolvingListSizeReq;

typedef struct
{
    CsrBtCmPrim                   type;              /* Identity: CSR_BT_CM_LE_READ_RESOLVING_LIST_SIZE_CFM */
    CsrUint8                      resolvingListSize; /* Size of controller's resolving list */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeReadResolvingListSizeCfm;

typedef struct
{
    CsrBtCmPrim                   type;              /* Identity: CSR_BT_CM_LE_SET_PRIVACY_MODE_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                peerIdAddress;     /* Peer Identity Address info */
    CsrBtPrivacyMode              privacyMode;       /* Privacy mode to be set either Network[0x00] or Device[0x01] */
} CsrBtCmLeSetPrivacyModeReq;

typedef CsrBtCmStdCfm CsrBtCmLeSetPrivacyModeCfm;

typedef struct
{
    CsrBtCmPrim                   type;          /* Identity CSR_BT_CM_LE_SET_OWN_ADDRESS_TYPE_REQ */
    CsrSchedQid                   appHandle;
    CsrBtOwnAddressType           ownAddressType;/* Own address type[0x00, 0x01, 0x02, 0x03] to be used during GAP procedures */
} CsrBtCmLeSetOwnAddressTypeReq;

typedef CsrBtCmStdCfm CsrBtCmLeSetOwnAddressTypeCfm;

typedef struct
{
    CsrBtCmPrim                   type;         /* Identity */
    CsrBtOwnAddressType           addressType;  /* Own address type[0x00, 0x01, 0x02, 0x03] */
} CsrBtCmLeOwnAddressTypeChangedInd;

typedef struct
{
    CsrBtCmPrim                   type;         /* Identity: CSR_BT_CM_LE_SET_PVT_ADDR_TIMEOUT_REQ */
    CsrSchedQid                   appHandle;
    CsrUint16                     timeout;      /* Private address timeout value. Range allowed : 0x0001(1 sec) - 0xA1B8(~11.5 hours) */
} CsrBtCmLeSetPvtAddrTimeoutReq;

typedef CsrBtCmStdCfm CsrBtCmLeSetPvtAddrTimeoutCfm;

typedef struct
{
    CsrBtCmPrim                   type;          /* Identity: CSR_BT_CM_LE_SET_STATIC_ADDRESS_REQ */
    CsrSchedQid                   appHandle;
    CsrBtDeviceAddr               staticAddress; /* Static address to be used locally for the current power cycle */
} CsrBtCmLeSetStaticAddressReq;

typedef CsrBtCmStdCfm CsrBtCmLeSetStaticAddressCfm;

typedef struct
{
    CsrBtCmPrim                   type;          /* Identity: CSR_BT_CM_LE_READ_RANDOM_ADDRESS_REQ */
    CsrSchedQid                   appHandle;
    CsrBtTypedAddr                idAddress;     /* Peer Identity Address info used to retrieve current local/peer RPA */
    CsrBtDeviceFlag               flag;          /* Flag which defines RPA to be retrieved either for local(0x01) or peer(0x02) device */
} CsrBtCmLeReadRandomAddressReq;

typedef struct
{
    CsrBtCmPrim                   type;       /* Identity: CSR_BT_CM_LE_READ_RANDOM_ADDRESS_CFM */
    CsrBtTypedAddr                idAddress;  /* Peer Identity Address asked for */
    CsrUint8                      flag;       /* RPA is whether for Local or peer device */
    CsrBtDeviceAddr               rpa;        /* Retrieved current local/peer device Resolvable Private address */
    CsrBtResultCode               resultCode;
    CsrBtSupplier                 resultSupplier;
} CsrBtCmLeReadRandomAddressCfm;

#ifdef __cplusplus
}
#endif

#endif /* ifndef _CM_PRIM_H */

