#ifndef WMSTSCDMA_H
#define WMSTSCDMA_H

#ifdef FEATURE_CDSMS

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          W I R E L E S S    M E S S A G I N G   S E R V I C E S

             ------ CDMA Message Encoding and Decoding Services

GENERAL DESCRIPTION
  This source file contains the encoding and decoding functions of CDMA SMS
  messages.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmstscdma.h_v   1.4   10 Jul 2002 20:17:52   hqu  $
$Header: //components/rel/qbi.tn/2.0/wmsts/inc/wmstscdma.h#1 $ $DateTime: 2013/03/07 13:24:57 $ $Author: coresvc $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
06/20/07   PMD     Cleaned up file for internal functions - moved to wmspriv
01/17/07   HQ      Updated copyright info to include 2007.
11/14/06   HQ      Modified wms_ts_decode_CDMA_bd prototype.
05/03/06   PMD     Modified wms_ts_decode_CDMA_bd prototype
01/25/06   VK      Updated Copyright Info
08/05/05   Rex     Removed extern usage from functions
01/17/03   HQ      Added wms_ts_encode_CDMA_bd().
07/10/02   HQ      Moved some definitions out of FEATURE_CDSMS_RUIM.
06/22/02   HQ      Added encoding and decoding functions for RUIM SMS, address,
                   subaddress and CDMA templates.
06/15/02   HQ      Added wms_ts_decode_OTA().
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/16/02   HQ      Added support of Multimode WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
09/28/01   HQ      Mainlined FEATURE_NEWUASMS.
01/29/01   HQ      Properly feature-ized FEATURE_NEWUASMS.
10/09/00   HQ      Added handling of message tag.
08/25/00   vt      Moved uasmsx_encode_bearer_data() and
                   uasmsx_decode_bearer_data() to uasms.h
10/28/99   HQ      Added flag into TL translation function for indicating
                   MO or MT msg.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "wms.h"
#include "wmspriv.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
/*=========================================================================
FUNCTION
  wms_ts_convert_cl2tl

DESCRIPTION
  This function converts the SMS TL data from the client structure
  to the internal structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_ts_convert_cl2tl
(
  const wms_cdma_message_s_type       * cl_ptr,    /* IN */
  wms_tl_message_type                 * tl_ptr     /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_convert_tl2cl

DESCRIPTION
  This function converts the SMS TL data from the internal structure
  to the client structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_ts_convert_tl2cl
(
  const wms_tl_message_type          * tl_ptr,   /* IN */
  wms_cdma_message_s_type            * cl_ptr    /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_encode_CDMA_tl

DESCRIPTION
  This function encodes the SMS TL fields in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_encode_CDMA_tl
(
  const wms_tl_message_type          * tl_msg_ptr,
  const wms_raw_ts_data_s_type       * raw_bd_ptr,
  wms_OTA_message_type               * OTA_msg_ptr   /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_CDMA_tl

DESCRIPTION
  This function decodes the SMS TL fields in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_CDMA_tl
(
  const wms_OTA_message_type     * OTA_msg_ptr,   /* IN */
  wms_tl_message_type            * tl_ptr,        /* OUT */
  wms_raw_ts_data_s_type         * raw_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_AWISMS_tl

DESCRIPTION
  This function decodes the AWI SMS TL fields in Analog mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_AWISMS_tl
(
  const wms_OTA_message_type       * OTA_msg_ptr,
  wms_tl_message_type              * tl_msg_ptr,      /* OUT */
  wms_raw_ts_data_s_type           * raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_CDMA_OTA

DESCRIPTION
  This function decodes the raw CDMA message into the internal structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_CDMA_OTA
(
  const wms_OTA_message_type       * OTA_msg_ptr,   /* IN */
  wms_tl_message_type              * tl_msg_ptr     /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_encode_CDMA_OTA

DESCRIPTION
  This function encodes the SMS message from the internal structure to
  the OTA format in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type  wms_ts_encode_CDMA_OTA
(
  const wms_tl_message_type       * tl_msg_ptr,
  wms_OTA_message_type            * OTA_msg_ptr   /* IN-OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_encode_CDMA_bd

DESCRIPTION
  This function encodes the bearer data from the client structure to
  its raw format in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_encode_CDMA_bd
(
  const wms_client_bd_s_type                * cl_bd_ptr,       /* IN */
  wms_raw_ts_data_s_type                    * raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_CDMA_bd

DESCRIPTION
  This function decodes the raw CDMA bearer data into the client's structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

COMMENTS
  If is_unpacked_user_data = TRUE, then there should be no UDH.

=========================================================================*/
wms_status_e_type  wms_ts_decode_CDMA_bd
(
  const wms_raw_ts_data_s_type       * raw_bd_ptr,   /* IN */
  boolean                            is_unpacked_user_data, /* IN */
  boolean                            decode_other,    /* IN */
  wms_client_bd_s_type               * cl_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_encode_bearer_data

DESCRIPTION
  This function encodes the SMS bearer data from the client
  structure to the raw format.

DEPENDENCIES
  None

RETURN VALUE
  Encoding result. WMS_OK_S means encoding succeeded; otherwise it failed.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_encode_bearer_data
(
  const wms_client_bd_s_type                * cl_bd_ptr,       /* IN */
  wms_raw_ts_data_s_type                    * raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_bearer_data

DESCRIPTION
  This function decodes the SMS bearer data from the raw format
  to the client structure.

DEPENDENCIES
  None

RETURN VALUE
  Encoding result. WMS_OK_S means encoding succeeded; otherwise it failed.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_bearer_data
(
  const wms_raw_ts_data_s_type            * raw_bd_ptr,   /* IN */
  wms_client_bd_s_type                    * cl_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_OTA

DESCRIPTION
  This function decodes the SMS message from the raw over-the-air structure
  to the internal message structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_OTA
(
  const wms_OTA_message_type       * OTA_msg_ptr,
  wms_tl_message_type              * tl_msg_ptr  /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_cdma_decode_smsp

DESCRIPTION
  This function decodes the SMS Parameters for CDMA

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_decode_smsp
(
  const uint8               * data,
  uint8                     rec_len,
  wms_cdma_template_s_type  * tptr
);

/*=========================================================================
FUNCTION
  wms_ts_cdma_encode_smsp

DESCRIPTION
  This function encodes the SMS Parameters for CDMA

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_encode_smsp
(
  const wms_cdma_template_s_type  * tptr,
  uint8                           rec_len,
  uint8                           * data
);

/*=========================================================================
FUNCTION
  wms_ts_validate_bd_subparms

DESCRIPTION
  This function	checks whether all the mandatory fields of a given teleservice
  is present in the bearer data

DEPENDENCIES
  None

RETURN VALUE
  Validation result. WMS_OK_S means all the mandatory sub parms are present.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_validate_bd_subparms
(
  const wms_client_bd_s_type                    * cl_bd_ptr,     /* IN */
  const wms_teleservice_e_type                  ts_type         /* IN */
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_CDSMS */

#endif /* WMSTSCDMA_H */
