/*!
  @file
  qbi_svc_atds_mbim.h

  @brief
  AT&T device service implementation, based on the "AT&T Windows 8
  Extended API Requirements - Joint Requirement Doc (JRD)" from AT&T.
  This device service allows host to set and query device/radio paremeters
  required for AT&T's acceptance.
*/

/*=============================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
09/01/12  cy   Initial release based on JRD v 1.6.1
=============================================================================*/

#ifndef QBI_SVC_ATDS_MBIM_H
#define QBI_SVC_ATDS_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of ATDS device service CIDs
*/
typedef enum {
  QBI_SVC_ATDS_CID_MIN = 0,

  QBI_SVC_ATDS_MBIM_CID_ATDS_SIGNAL             = 1,
  QBI_SVC_ATDS_MBIM_CID_ATDS_LOCATION           = 2,
  QBI_SVC_ATDS_MBIM_CID_ATDS_OPERATORS          = 3,
  QBI_SVC_ATDS_MBIM_CID_ATDS_RAT                = 4,
  QBI_SVC_ATDS_MBIM_CID_ATDS_REGISTER_STATE     = 9,
  QBI_SVC_ATDS_MBIM_CID_ATDS_PROJECTION_TABLES  = 10,
  QBI_SVC_ATDS_MBIM_CID_ATDS_RAT_DISPLAY        = 11,

  QBI_SVC_ATDS_CID_MAX
} qbi_svc_atds_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_ATDS_SIGNAL
    @{ */

#define QBI_SVC_ATDS_RSSI_MIN     (0)
#define QBI_SVC_ATDS_RSSI_MIN_DBM (-113)
#define QBI_SVC_ATDS_RSSI_MAX     (31)
#define QBI_SVC_ATDS_RSSI_MAX_DBM (-51)
#define QBI_SVC_ATDS_RSSI_UNKNOWN (99)
#define QBI_SVC_ATDS_RSSI_FACTOR  (2)

#define QBI_SVC_ATDS_BER_MIN     (0)
#define QBI_SVC_ATDS_BER_MAX     (7)
#define QBI_SVC_ATDS_BER_UNKNOWN (99)

#define QBI_SVC_ATDS_RSCP_MIN     (0)
#define QBI_SVC_ATDS_RSCP_MIN_DBM (-120)
#define QBI_SVC_ATDS_RSCP_MAX     (96)
#define QBI_SVC_ATDS_RSCP_MAX_DBM (-24)
#define QBI_SVC_ATDS_RSCP_UNKNOWN (255)
#define QBI_SVC_ATDS_RSCP_FACTOR  (1)

#define QBI_SVC_ATDS_ECNO_MIN     (0)
#define QBI_SVC_ATDS_ECNO_MIN_DB  (-24.5)
#define QBI_SVC_ATDS_ECNO_MAX     (49)
#define QBI_SVC_ATDS_ECNO_MAX_DB  (0)
#define QBI_SVC_ATDS_ECNO_UNKNOWN (255)
#define QBI_SVC_ATDS_ECNO_FACTOR  (0.5)

#define QBI_SVC_ATDS_RSRQ_MIN     (0)
#define QBI_SVC_ATDS_RSRQ_MIN_DBM (-19.5)
#define QBI_SVC_ATDS_RSRQ_MAX     (34)
#define QBI_SVC_ATDS_RSRQ_MAX_DBM (-2.5)
#define QBI_SVC_ATDS_RSRQ_UNKNOWN (255)
#define QBI_SVC_ATDS_RSRQ_FACTOR  (0.5)

#define QBI_SVC_ATDS_RSRP_MIN     (0)
#define QBI_SVC_ATDS_RSRP_MIN_DBM (-140)
#define QBI_SVC_ATDS_RSRP_MAX     (97)
#define QBI_SVC_ATDS_RSRP_MAX_DBM (-43)
#define QBI_SVC_ATDS_RSRP_UNKNOWN (255)
#define QBI_SVC_ATDS_RSRP_FACTOR  (1)

#define QBI_SVC_ATDS_RSSNR_MIN     (0)
#define QBI_SVC_ATDS_RSSNR_MIN_DBM (-4.5)
#define QBI_SVC_ATDS_RSSNR_MAX     (34)
#define QBI_SVC_ATDS_RSSNR_MAX_DBM (29.5)
#define QBI_SVC_ATDS_RSSNR_UNKNOWN (255)
#define QBI_SVC_ATDS_RSSNR_FACTOR  (1)

typedef PACK(struct){
  uint32 rssi;  /*!< RSSI */
  uint32 ber;   /*!< Channel bit error rate (in percent) */
  uint32 rscp;  /*!< Received signal code power  */
  uint32 ecno;  /*!< Ec/No */
  uint32 rsrq;  /*!< Reference signal received quality */
  uint32 rsrp;  /*!< Reference signal received power */
  uint32 rssnr; /*!< The signal to noise ratio for LTE */
} qbi_svc_atds_signal_info_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_LOCATION
    @{ */

/* MBIM_LOCATION_INFO */
typedef PACK(struct){
  uint32 lac;    /*!< location area code. 0 if not supported */
  uint32 tac;    /*!< the TAC value. 0 if not supported */
  uint32 cellid; /*!< cell identifier */
} qbi_svc_atds_location_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_OPERATORS
    @{ */

/* Values for PLMNMode field (replaces CellularClass in MBIM_PROVIDER) */
#define QBI_SVC_ATDS_PLMN_MODE_GSM         (0)
#define QBI_SVC_ATDS_PLMN_MODE_GSM_COMPACT (1)
#define QBI_SVC_ATDS_PLMN_MODE_UTRAN       (2)
#define QBI_SVC_ATDS_PLMN_MODE_GSM_EGPRS   (3)
#define QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSDPA (4)
#define QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSUPA (5)
#define QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSPA  (6)
#define QBI_SVC_ATDS_PLMN_MODE_LTE         (7)

/*! @note MBIM_PROVIDERS and MBIM_PROVIDER structures re-used for this CID */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_RAT
    @{ */

/* Radio access technology preference */
#define QBI_SVC_ATDS_RAT_PREF_AUTOMATIC (0)
#define QBI_SVC_ATDS_RAT_PREF_2G_ONLY   (1)
#define QBI_SVC_ATDS_RAT_PREF_3G_ONLY   (2)
#define QBI_SVC_ATDS_RAT_PREF_4G_ONLY   (3)

/* MBIM_RAT_INFO */
typedef PACK(struct) {
  uint32 mode_pref;
  /*! @note Followed by element_count instances of
      qbi_mbim_offset_size_pair_s, then DataBuffer containing element_count
      qbi_svc_atds_operator_s structures */
} qbi_svc_atds_rat_s;

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_PROJECTION_TABLES
    @{ */

/* MBIM_PROJECTION_TYPE */
#define QBI_SVC_ATDS_PROJECTION_TYPE_RSSI  (0)
#define QBI_SVC_ATDS_PROJECTION_TYPE_RSCP  (1)
#define QBI_SVC_ATDS_PROJECTION_TYPE_ECNO  (2)
#define QBI_SVC_ATDS_PROJECTION_TYPE_RSRP  (3)
#define QBI_SVC_ATDS_PROJECTION_TYPE_RSSNR (7)

/*! Size of the array of projection entries in MBIM_PROJECTION_TABLE */
#define QBI_SVC_ATDS_PROJECTION_TABLE_COUNT (6)

/*! Multiplication factor for coefficients (A and B) in projection entries, i.e.
    we need to divide the coefficients by this factor to get the intended
    fractional value */
#define QBI_SVC_ATDS_PROJECTION_FACTOR (1000)

typedef PACK(struct) {
  int32 bar_min;
  int32 a;
  int32 b;
} qbi_svc_atds_projection_entry_s;

/* MBIM_PROJECTION_TABLE */
typedef PACK(struct) {
  uint32 type;
  qbi_svc_atds_projection_entry_s entries[QBI_SVC_ATDS_PROJECTION_TABLE_COUNT];
} qbi_svc_atds_projection_table_s;

/* MBIM_PROJECTION_TABLES header info with count */
typedef PACK(struct) {
  uint32 count;
  /*! @note Followed by count instances of qbi_mbim_offset_size_pair_s, then
      DataBuffer containing qbi_mbim_atds_single_single_projection_table_s
      structures */
} qbi_svc_atds_projection_tables_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_ATDS_MBIM_H */

