/*!
  @file
  qbi_svc_auth_mbim.h

  @brief
  Authentication device service definitions provided by the MBIM specification,
  e.g. InformationBuffer structures, enums, etc.
*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/20/12  hz   Initial release based on MBIM v1.0
=============================================================================*/

#ifndef QBI_SVC_AUTH_MBIM_H
#define QBI_SVC_AUTH_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of AUTH device service CIDs
*/
typedef enum {
  QBI_SVC_AUTH_CID_MIN = 0,

  QBI_SVC_AUTH_MBIM_CID_AKA_AUTH  = 1,
  QBI_SVC_AUTH_MBIM_CID_AKAP_AUTH = 2,
  QBI_SVC_AUTH_MBIM_CID_SIM_AUTH  = 3,

  QBI_SVC_AUTH_CID_MAX
} qbi_svc_auth_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

#define QBI_SVC_AUTH_RAND_LEN       (16)
#define QBI_SVC_AUTH_AUTN_LEN       (16)
#define QBI_SVC_AUTH_RES_LEN        (16)
#define QBI_SVC_AUTH_IK_LEN         (16)
#define QBI_SVC_AUTH_CK_LEN         (16)
#define QBI_SVC_AUTH_AUTS_LEN       (14)
#define QBI_SVC_AUTH_SRES_LEN       (4)
#define QBI_SVC_AUTH_KC_LEN         (8)

/*! @addtogroup MBIM_CID_AKA_AUTH
    @{ */
/* MBIM_AKA_AUTH_REQ request */
typedef PACK(struct){
  uint8 rand[QBI_SVC_AUTH_RAND_LEN];
  uint8 autn[QBI_SVC_AUTH_AUTN_LEN];
} qbi_svc_auth_aka_auth_q_req_s;

/* MBIM_AKA_AUTH_INFO response */
typedef PACK(struct){
  uint8  res[QBI_SVC_AUTH_RES_LEN];
  uint32 res_len;
  uint8  ik[QBI_SVC_AUTH_IK_LEN];
  uint8  ck[QBI_SVC_AUTH_CK_LEN];
  uint8  auts[QBI_SVC_AUTH_AUTS_LEN];
} qbi_svc_auth_aka_auth_info_rsp_s;
/*! @} */

/*! @addtogroup MBIM_CID_AKAP_AUTH
    @{ */
/* MBIM_AKAP_AUTH_REQ request */
typedef PACK(struct){
  uint8 rand[QBI_SVC_AUTH_RAND_LEN];
  uint8 autn[QBI_SVC_AUTH_AUTN_LEN];
  qbi_mbim_offset_size_pair_s network_name;
  /*! @note Followed by DataBuffer containing NetworkName */
} qbi_svc_auth_akap_auth_q_req_s;

/* MBIM_AKAP_AUTH_INFO response */
typedef PACK(struct){
  uint8  res[QBI_SVC_AUTH_RES_LEN];
  uint32 res_len;
  uint8  ik[QBI_SVC_AUTH_IK_LEN];
  uint8  ck[QBI_SVC_AUTH_CK_LEN];
  uint8  auts[QBI_SVC_AUTH_AUTS_LEN];
} qbi_svc_auth_akap_auth_info_rsp_s;
/*! @} */

/*! @addtogroup MBIM_CID_SIM_AUTH
    @{ */
/* MBIM_SIM_AUTH_REQ request */
typedef PACK(struct){
  uint8  rand1[QBI_SVC_AUTH_RAND_LEN];
  uint8  rand2[QBI_SVC_AUTH_RAND_LEN];
  uint8  rand3[QBI_SVC_AUTH_RAND_LEN];
  uint32 n;
} qbi_svc_auth_sim_auth_q_req_s;

/* MBIM_SIM_AUTH_INFO response */
typedef PACK(struct){
  uint8 sres1[QBI_SVC_AUTH_SRES_LEN];
  uint8 kc1[QBI_SVC_AUTH_KC_LEN];
  uint8 sres2[QBI_SVC_AUTH_SRES_LEN];
  uint8 kc2[QBI_SVC_AUTH_KC_LEN];
  uint8 sres3[QBI_SVC_AUTH_SRES_LEN];
  uint8 kc3[QBI_SVC_AUTH_KC_LEN];
  uint32 n;
} qbi_svc_auth_sim_auth_info_rsp_s;
/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_AUTH_MBIM_H */

