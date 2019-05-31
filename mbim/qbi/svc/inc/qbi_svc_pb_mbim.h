/*!
  @file
  qbi_svc_pb_mbim.h

  @brief
  Phonebook device service definitions provided by the MBIM specification, e.g.
  InformationBuffer structures, enums, etc.
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
01/31/12  bd   Added module
=============================================================================*/

#ifndef QBI_SVC_PB_MBIM_H
#define QBI_SVC_PB_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of Phonebook device service CIDs
*/
typedef enum {
  QBI_SVC_PB_CID_MIN = 0,

  QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION = 1,
  QBI_SVC_PB_MBIM_CID_PHONEBOOK_READ          = 2,
  QBI_SVC_PB_MBIM_CID_PHONEBOOK_DELETE        = 3,
  QBI_SVC_PB_MBIM_CID_PHONEBOOK_WRITE         = 4,

  QBI_SVC_PB_CID_MAX
} qbi_svc_pb_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_PHONEBOOK_CONFIGURATION
    @{ */

/* MBIM_PHONEBOOK_STATE */
#define QBI_SVC_PB_STATE_NOT_INITIALIZED (0)
#define QBI_SVC_PB_STATE_INITIALIZED     (1)

/* MBIM_PHONEBOOK_CONFIGURATION_INFO */
typedef PACK(struct) {
  uint32 state;
  uint32 total_entries;
  uint32 used_entries;
  uint32 max_num_length;
  uint32 max_name_length;
} qbi_svc_pb_configuration_info_q_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_READ
    @{ */

/* MBIM_PHONEBOOK_FLAG */
#define QBI_SVC_PB_FILTER_FLAG_ALL   (0)
#define QBI_SVC_PB_FILTER_FLAG_INDEX (1)

/* MBIM_PHONEBOOK_ENTRY */
typedef PACK(struct) {
  uint32 index;
  qbi_mbim_offset_size_pair_s number;
  qbi_mbim_offset_size_pair_s name;
  /*! @note Followed by DataBuffer containing phone number and name */
} qbi_svc_pb_entry_s;

/* MBIM_PHONEBOOK_READ_REQ */
typedef PACK(struct) {
  uint32 filter_flag;
  uint32 filter_index;
} qbi_svc_pb_read_q_req_s;

/* MBIM_PHONEBOOK_READ_INFO */
typedef PACK(struct) {
  uint32 element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
      then DataBuffer containing element_count qbi_svc_pb_entry_s structures */
} qbi_svc_pb_read_q_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_DELETE
    @{ */

/* MBIM_SET_PHONEBOOK_DELETE */
typedef PACK(struct) {
  uint32 filter_flag;
  uint32 filter_index;
} qbi_svc_pb_delete_s_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_WRITE
    @{ */

/* MBIM_PHONEBOOK_WRITE_FLAG */
#define QBI_SVC_PB_WRITE_FLAG_SAVE_UNUSED (0)
#define QBI_SVC_PB_WRITE_FLAG_SAVE_INDEX  (1)

/* MBIM_PHONEBOOK_INDEX_NONE */
#define QBI_SVC_PB_INDEX_NONE (0)

/* MBIM_SET_PHONEBOOK_WRITE */
typedef PACK(struct) {
  uint32 save_flag;
  uint32 save_index;
  qbi_mbim_offset_size_pair_s number;
  qbi_mbim_offset_size_pair_s name;
  /*! @note Followed by DataBuffer containing phone number and name */
} qbi_svc_pb_write_s_req_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_PB_MBIM_H */

