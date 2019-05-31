/*!
  @file
  qbi_qmi_defs.h

  @brief
  QMI-related definitions that are listed in the QMI ICD but not defined as a
  usable type in the IDL (e.g. those listed in comments).
*/

/*=============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
07/10/12  bd   Add invalid MCC/MNC definition, remove unused definitions
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_QMI_DEFS_H
#define QBI_QMI_DEFS_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "network_access_service_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/* Emergency mode reported by QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE and its
   associated indication */
#define QBI_QMI_NAS_EMERGENCY_MODE_OFF (0x00)
#define QBI_QMI_NAS_EMERGENCY_MODE_ON  (0x01)

/* Min and max valid values for GSM bit error rate */
#define QBI_QMI_NAS_GSM_BER_MIN (0)
#define QBI_QMI_NAS_GSM_BER_MAX (700)

/* Min and max valid values for CDMA 1X frame error rate */
#define QBI_QMI_NAS_CDMA_FER_MIN (1)
#define QBI_QMI_NAS_CDMA_FER_MAX (10000)

/*! Signed and unsigned (i.e. positive and negative) dBm values used by QMI NAS
    RSSI reporting to indicate that the radio interface has no service */
#define QBI_QMI_NAS_UNSIGNED_RSSI_NO_SRV (125)
#define QBI_QMI_NAS_SIGNED_RSSI_NO_SRV   (-125)

/* Network Status Bit Shift Values for QMI_NAS_PERFORM_NETWORK_SCAN */
#define QBI_QMI_NAS_NETWORK_IN_USE_STATUS_SHIFT    (0)
#define QBI_QMI_NAS_NETWORK_ROAMING_STATUS_SHIFT   (2)
#define QBI_QMI_NAS_NETWORK_FORBIDDEN_STATUS_SHIFT (4)
#define QBI_QMI_NAS_NETWORK_PREFERRED_STATUS_SHIFT (6)

/* Value of an unused digit in the ASCII MCC/MNC in
   nas_common_network_id_type_v01 */
#define QBI_QMI_NAS_MCC_MNC_UNUSED_DIGIT (0xFF)

/* Used in QMI_WDS_GET_PKT_STATISTICS to indicate the value is unknown */
#define QBI_QMI_WDS_PKT_STATS_UNKNOWN (0xFFFFFFFF)

/* UIM session index value for QMI_UIM_GET_CARD_STATUS indicating unknown or
   unavailable session */
#define QBI_QMI_UIM_SESSION_INDEX_UNKNOWN (0xFFFF)

/* Least significant byte of QBI_QMI_UIM_SESSION_INDEX_UNKNOWN, indicating that
   the application index is not known without hinting at the card index */
#define QBI_QMI_UIM_APP_INDEX_UNKNOWN (0xFF)

/* Ref ID for buffered proactive command when no terminal response is expected
   from host */
#define QBI_QMI_CAT_CMD_REF_ID_NO_TR      (0xFFFFFFFF)

/*! @todo Remove after QMI CAT support status words optional TLV in terminal
    response message */
/* Status words SW1 and SW2 indicating sucess */
#define QBI_QMI_CAT_STATUS_WORDS_SUCCESS  (0x9000)

/*! Value provided in record ID field in QMI_PBM_WRITE_RECORD_REQ indicating
    that the first available slot should be used for the new record */
#define QBI_QMI_PBM_RECORD_ID_FIRST_AVAILABLE (0)

/*! First valid record ID in a phonebook */
#define QBI_QMI_PBM_RECORD_ID_START (1)

/* Radio Access Technologies for QMI_NAS_PERFORM_NETWORK_SCAN */
#define QBI_QMI_NAS_RAT_GSM     (4)
#define QBI_QMI_NAS_RAT_UMTS    (5)
#define QBI_QMI_NAS_RAT_LTE     (8)
#define QBI_QMI_NAS_RAT_TDSCDMA (9)

/*! Factor to scale SNR values in QMI_NAS_GET_SIG_INFO */
#define QBI_QMI_NAS_SNR_FACTOR (10)

/*! Factor used to scale various values in QMI_NAS_CONFIG_SIG_INFO2, e.g. GSM
    RSSI is given in 0.1 dBm increments, so a dBm value must be multiplied by
    10 */
#define QBI_QMI_NAS_CONFIG_SIG_INFO2_FACTOR (10)

/*! Maximum allowed profile index, derived from size of profile index TLV */
#define QBI_QMI_WDS_PROFILE_INDEX_MAX (255)

/* Values used in QMI_NAS_GET_ERR_RATE_RESP to identify unknown error rate */
#define QBI_QMI_NAS_ERROR_RATE_UNKNOWN_3GPP  (0xFF)
#define QBI_QMI_NAS_ERORR_RATE_UNKNOWN_3GPP2 (0xFFFF)

/* WCDMA band preference values in QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE */
#define QBI_QMI_NAS_BAND_PREF_WCDMA_XIX_850 ((nas_band_pref_mask_type_v01) 0x1000000000000000ull)
#define QBI_QMI_NAS_BAND_PREF_WCDMA_XI_1500 ((nas_band_pref_mask_type_v01) 0x2000000000000000ull)

/*! All 3GPP2 band classes reported by QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE */
#define QBI_QMI_NAS_BAND_PREF_FAMILY_3GPP2 \
  (QMI_NAS_BAND_CLASS_0_A_SYSTEM_V01    | \
   QMI_NAS_BAND_CLASS_0_B_AB_GSM850_V01 | \
   QMI_NAS_BAND_CLASS_1_ALL_BLOCKS_V01  | \
   QMI_NAS_BAND_CLASS_2_PLACEHOLDER_V01 | \
   QMI_NAS_BAND_CLASS_3_A_SYSTEM_V01    | \
   QMI_NAS_BAND_CLASS_4_ALL_BLOCKS_V01  | \
   QMI_NAS_BAND_CLASS_5_ALL_BLOCKS_V01  | \
   QMI_NAS_BAND_CLASS_6_V01  | \
   QMI_NAS_BAND_CLASS_7_V01  | \
   QMI_NAS_BAND_CLASS_8_V01  | \
   QMI_NAS_BAND_CLASS_9_V01  | \
   QMI_NAS_BAND_CLASS_10_V01 | \
   QMI_NAS_BAND_CLASS_11_V01 | \
   QMI_NAS_BAND_CLASS_12_V01 | \
   QMI_NAS_BAND_CLASS_14_V01 | \
   QMI_NAS_BAND_CLASS_15_V01 | \
   QMI_NAS_BAND_CLASS_16_V01 | \
   QMI_NAS_BAND_CLASS_17_V01 | \
   QMI_NAS_BAND_CLASS_18_V01 | \
   QMI_NAS_BAND_CLASS_19_V01)

/*! All GSM band classes reported by QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE */
#define QBI_QMI_NAS_BAND_PREF_FAMILY_GSM \
  (QMI_NAS_GSM_DCS_1800_BAND_V01 | \
   QMI_NAS_E_GSM_900_BAND_V01    | \
   QMI_NAS_P_GSM_900_BAND_V01    | \
   QMI_NAS_GSM_BAND_450_V01      | \
   QMI_NAS_GSM_BAND_480_V01      | \
   QMI_NAS_GSM_BAND_750_V01      | \
   QMI_NAS_GSM_BAND_850_V01      | \
   QMI_NAS_GSM_BAND_RAILWAYS_900_BAND_V01 | \
   QMI_NAS_GSM_BAND_PCS_1900_BAND_V01)

/*! All WCDMA band classes reported by QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE */
#define QBI_QMI_NAS_BAND_PREF_FAMILY_WCDMA \
  (QMI_NAS_WCDMA_EU_J_CH_IMT_2100_BAND_V01 | \
   QMI_NAS_WCDMA_US_PCS_1900_BAND_V01  | \
   QMI_NAS_EU_CH_DCS_1800_BAND_V01     | \
   QMI_NAS_WCDMA_US_1700_BAND_V01      | \
   QMI_NAS_WCDMA_US_850_BAND_V01       | \
   QMI_NAS_WCDMA_JAPAN_800_BAND_V01    | \
   QMI_NAS_WCDMA_EU_2600_BAND_V01      | \
   QMI_NAS_WCDMA_EU_J_900_BAND_V01     | \
   QMI_NAS_WCDMA_J_1700_BAND_V01       | \
   QBI_QMI_NAS_BAND_PREF_WCDMA_XIX_850 | \
   QBI_QMI_NAS_BAND_PREF_WCDMA_XI_1500)

/*=============================================================================

  Typedefs

=============================================================================*/

#endif /* QBI_QMI_DEFS_H */

