/*!
  @file
  qbi_nv_store.h

  @brief
  Non-volatile storage platform abstraction layer

  @details
  This file presents two different sets of APIs - one for configuration items
  and another for files. Configuration items are fixed-length structures that
  are generally expected to be small, though there is no explicit upper bound
  on their size. Files can be variable length, and are generally expected to be
  larger than configuration items. The platform-specific implementation may use
  this distinction to store each type separately, for example to utilize a more
  space-efficient storage method for the smaller configuration items.
*/

/*=============================================================================

  Copyright (c) 2012-2014, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
06/26/18  mm   Removed persistent and added non persistent cache for DSSA
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
01/05/18  ha   Add NV for pcie_enabled item
08/18/17  rv   Add NV for Single PDP item
07/27/17  rv   Add function to delete item
07/18/17  rv   Add item to maintain QBI version
06/28/17  rv   Add item for operator config
03/22/17  rv   Add item for executor_slot config
04/02/14  hz   Add item for SMS storage preference
11/18/13  bd   Combine PLMN name and SPN per 3GPP 22.101 A.4 (NV configurable)
09/04/13  bd   Store PROVISIONED_CONTEXTS ContextType in NV layer
06/24/13  bd   Add API to cache encrypted PIN1
06/06/13  bd   Add file read API for ERI support
01/22/13  bd   Add item for last firmware update session info
11/21/12  bd   Add runtime DeviceType configuration
06/01/12  bd   Rework REGISTER_STATE storage to only maintain net_sel_pref
02/03/12  bd   Added module
=============================================================================*/

#ifndef QBI_NV_STORE_H
#define QBI_NV_STORE_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_util.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Configuration items saved to non-volatile storage */
typedef enum {
  /*! @brief Network selection preference maintained by QBI
      @details As not all manual network selection information is maintained by
      the modem across power cycle, QBI must store this information and provide
      it to the modem when needed. Data structure:
      qbi_svc_bc_nas_net_sel_pref_cfg_s */
  QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF = 0,

  /*! @brief MBIM DeviceType setting
      @details The DeviceType value that is sent to the host in
      MBIM_CID_DEVICE_CAPS. The platform-specific NV store layer may provide a
      default value if there is no value configured at runtime.
      Data structure: uint32 */
  QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE = 1,

  /*! @brief Information about last firmware update session
      @details Maintains information about the result of the last firmware
      update session. Data structure: platform-dependent. See qbi_fwupd platform
      implementation. */
  QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO = 2,

  /*! @brief Microsoft Firmware ID
      @details Maintains the Firmware ID used with the Microsoft Firmware ID
      device service. Platform support of this configuration item is optional.
      Data structure: uint8[16] (UUID) */
  QBI_NV_STORE_CFG_ITEM_FIRMWARE_ID = 3,

  /*! @brief Whether to concatenate the PLMN name and SPN in REGISTER_STATE
      @details If enabled, QBI will follow the recommendation in 3GPP TS 22.101
      Annex A.4, and return both network name sources in ProviderName. Data
      structure: uint32 */
  QBI_NV_STORE_CFG_ITEM_CONCAT_PLMN_NAME_SPN = 4,

  /*! @brief Call type parameter to use in QMI_WDS_START_NETWORK_INTERFACE_REQ
      @details Default is 0 (laptop call). Data structure: uint32 (cast to
      wds_call_type_enum_v01) */
  QBI_NV_STORE_CFG_ITEM_WDS_CALL_TYPE = 5,

  /*! @brief Whether to restrict the data class reported in DEVICE_CAPS to only
      RATs appearing in the current band preference
      @details Default is 0 (do not limit to band preference, instead relying on
      the device capabilities reported by the modem). Data structure: uint32 */
  QBI_NV_STORE_CFG_ITEM_LIMIT_DEV_CAP_TO_BAND_PREF = 6,

  /*! @brief SMS storage preference maintained by QBI
      @details QBI uses both NV and UIM storage types by default to present a
      single logical storage to host. If set to WMS_STORAGE_TYPE_UIM_V01, only
      UIM storage will be used assuming modem is also configured
      to use UIM storage only. If set to other values, default SMS storage
      behavior remains. Data structure: wms_storage_type_enum_v01 */
  QBI_NV_STORE_CFG_ITEM_SMS_STORAGE_PREF = 7,

  /*! @brief Provisioned context owner maintained by QBI
  @details Provisioned context profiles can be modem defined or HLOS 
  defined/modified. QBI uses this NV store to identify the owner and populate 
  parameters - source, roaming, media type and enable flag accordingly.*/
  QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA = 8,

  /*! @brief Executor slot configuration maintened by QBI
  @details This NV maintains operator specific configuration. This is used 
  to keep track enable/disable for class1/class2 specific APN*/ 
  QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG = 9,

  /*! @brief Build Version maintened by QBI
  @details This NV maintains version for the builds.*/ 
  QBI_NV_STORE_CFG_ITEM_QBI_VERSION = 10,

  /*! @brief Single PDP support maintened by QBI
  @details This NV maintains operator specific single PDP support flag.*/
  QBI_NV_STORE_CFG_ITEM_SINGLE_PDP_SUPPORT = 11,

  /*! @brief Is platform AND OS
  @details This NV acts as a flag to describe if the platform is Andromeda OS.*/
  QBI_NV_STORE_CFG_ITEM_SINGLE_WDS = 12,

  /*! @brief Enable SSR
  @details This NV is used to enable the SSR functionality on Linux platform
  and on windows this is disabled as we have different mechanism to handle
  the SSR */
  QBI_NV_STORE_CFG_ITEM_SSR_ENABLE = 13,

  QBI_NV_STORE_CFG_ITEM_END
} qbi_nv_store_cfg_item_e;

/*! Variable length files saved to non-volatile storage */
typedef enum {
  /*! Verizon-specific Enhanced Roaming Indicator (ERI), binary file format */
  QBI_NV_STORE_FILE_ID_ERI_VZW = 0,

  /*! Sprint-specific Enhanced Roaming Indicator (ERI), ASCII text format */
  QBI_NV_STORE_FILE_ID_ERI_SPR = 1,

  /*! Sprint-specific international roaming indicator list, ASCII text format */
  QBI_NV_STORE_FILE_ID_INTL_ROAM_SPR = 2,

  QBI_NV_STORE_FILE_ID_END
} qbi_nv_store_file_id_e;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_read
===========================================================================*/
/*!
    @brief Read a configuration item from non-volatile storage

    @details

    @param item
    @param data
    @param data_len

    @return boolean TRUE if entire configuration item read successfully,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_read
(
  qbi_ctx_s              *ctx,
  qbi_nv_store_cfg_item_e item,
  void                   *data,
  uint32                  data_len
);

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_write
===========================================================================*/
/*!
    @brief Write a configuration item to non-volatile storage

    @details

    @param ctx
    @param item
    @param data
    @param data_len

    @return boolean TRUE if entire configuration item written successfully,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_write
(
  qbi_ctx_s              *ctx,
  qbi_nv_store_cfg_item_e item,
  void                   *data,
  uint32                  data_len
);

/*===========================================================================
  FUNCTION: qbi_nv_store_encrypted_pin1_save
===========================================================================*/
/*!
    @brief Saves an encrypted PIN1 value to a platform-dependent temporary
    storage area

    @details
    Intended to support silent PIN re-verification after modem subsystem
    restart. Implementation of this function is optional and may be a no-op
    for platforms where PIN re-verification is not supported/not applicable
    (e.g. MDMs).

    @param ctx
    @param enc_pin1_data
    @param enc_pin1_len
*/
/*=========================================================================*/
void qbi_nv_store_encrypted_pin1_save
(
  qbi_ctx_s   *ctx,
  const uint8 *enc_pin1_data,
  uint32       enc_pin1_len
);

/*===========================================================================
  FUNCTION: qbi_nv_store_file_read
===========================================================================*/
/*!
    @brief Reads the complete contents of a file into a buffer

    @details

    @param ctx
    @param file_id
    @param buf Pointer to an initialized buffer to use for storing the file
    contents. If this function returns TRUE, the caller is responsible for
    freeing the memory associated with the buffer.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_nv_store_file_read
(
  qbi_ctx_s             *ctx,
  qbi_nv_store_file_id_e file_id,
  qbi_util_buf_s        *buf
);

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_delete
===========================================================================*/
/*!
    @brief Delete the configuration item

    @details

    @param ctx
    @param qbi_nv_store_cfg_item_e item
 
    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_delete
(
  qbi_ctx_s             *ctx,
  qbi_nv_store_cfg_item_e item
);

#endif /* QBI_NV_STORE_H */

