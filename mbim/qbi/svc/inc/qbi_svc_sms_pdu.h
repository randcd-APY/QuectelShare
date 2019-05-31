/*!
  @file
  qbi_svc_sms_pdu.h

  @brief
  SMS PDU transcoding functions
*/

/*=============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/13/12  bd   Added module
=============================================================================*/

#ifndef QBI_SVC_SMS_PDU_H
#define QBI_SVC_SMS_PDU_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Enums

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_cdma_to_gw
===========================================================================*/
/*!
    @brief Convert an SMS from GSM/WCDMA PDU format to CDMA PDU format

    @details
    As QBI only needs to convert from CDMA to GW format when reading a
    stored CDMA message, this function assumes the target GW TPDU format is
    for a mobile terminated (MT) SMS.

    In-place conversion is supported (i.e. gw_pdu and cdma_pdu can point
    to the same buffer).

    @param cdma_pdu Buffer containing the SMS in CDMA PDU format
    @param cdma_pdu_size Size of CDMA PDU in bytes
    @param gw_pdu Buffer to populate with SMS in GSM PDU format
    @param gw_pdu_buf_size Size of the GSM PDU buffer in bytes

    @return uint32 Returns 0 on failure, or number of bytes set in the
    gsm_pdu buffer on success.
*/
/*=========================================================================*/
uint32 qbi_svc_sms_pdu_cdma_to_gw
(
  const uint8 *cdma_pdu,
  uint32       cdma_pdu_size,
  uint8       *gw_pdu,
  uint32       gw_pdu_buf_size
);

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_gw_to_cdma
===========================================================================*/
/*!
    @brief Convert an SMS from GSM/WCDMA PDU format to CDMA PDU format

    @details
    As QBI only needs to convert from GW to CDMA format when sending an SMS
    message, this function assumes the GW TPDU format is for a mobile
    originated (MO) SMS.

    In-place conversion is supported (i.e. gw_pdu and cdma_pdu can point
    to the same buffer).

    @param gsm_pdu Buffer containing the SMS in GSM/WCDMA PDU format
    @param gsm_pdu_size Size of GSM/WCDMA PDU in bytes
    @param cdma_pdu Buffer to populate with SMS in CDMA PDU format
    @param cdma_pdu_size Size of the CDMA PDU buffer in bytes

    @return uint32 Returns 0 on failure, or number of bytes set in the
    cdma_pdu buffer on success.
*/
/*=========================================================================*/
uint32 qbi_svc_sms_pdu_gw_to_cdma
(
  const uint8 *gsm_pdu,
  uint32       gsm_pdu_size,
  uint8       *cdma_pdu,
  uint32       cdma_pdu_size
);

#endif /* QBI_SVC_SMS_PDU_H */

