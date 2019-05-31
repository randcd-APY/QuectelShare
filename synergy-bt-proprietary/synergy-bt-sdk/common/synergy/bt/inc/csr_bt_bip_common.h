#ifndef CSR_BT_BIP_COMMON_H__
#define CSR_BT_BIP_COMMON_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_BIP_MAX_AUTH_USERID_LENGTH                CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH
#define CSR_BT_BIP_MAX_ENCODING_ATTRIBUTE_LENGTH         (10)
#define CSR_BT_BIP_MAX_PIXEL_ATTRIBUTE_LENGTH            (20)
#define CSR_BT_BIP_MAX_SIZE_ATTRIBUTE_LENGTH             (8)
#define CSR_BT_BIP_MAX_TRANSFORMATION_ATTRIBUTE_LENGTH   (20)
#define CSR_BT_BIP_MAX_CREATED_ATTRIBUTE_LENGTH          (35)
#define CSR_BT_BIP_MAX_CONTENT_TYPE_ATTRIBUTE_LENGTH     (25)
#define CSR_BT_BIP_MAX_CHARSET_ATTRIBUTE_LENGTH          (15)
#define CSR_BT_BIP_MAX_NAME_ATTRIBUTE_LENGTH             (20)

#define CSR_BT_IMAGE_PUSH_FEATURE                        (0x00)
#define CSR_BT_AUTO_ARCHIVE_FEATURE                      (0x02)
#define CSR_BT_REMOTE_CAMERA_FEATURE                     (0x04)

#define CSR_BT_GET_CAPABILITIES_MASK                     (0x00000001)
#define CSR_BT_PUT_IMAGE_MASK                            (0x00000002)
#define CSR_BT_PUT_LINKED_ATTACHMENT_MASK                (0x00000004)
#define CSR_BT_PUT_LINKED_THUMBNAIL_MASK                 (0x00000008)
#define CSR_BT_GET_IMAGE_LIST_MASK                       (0x00000020)
#define CSR_BT_GET_IMAGE_PROPERTIES_MASK                 (0x00000040)
#define CSR_BT_GET_IMAGE_MASK                            (0x00000080)
#define CSR_BT_GET_LINKED_THUMBNAIL_MASK                 (0x00000100)
#define CSR_BT_GET_LINKED_ATTACHMENT_MASK                (0x00000200)
#define CSR_BT_DELETE_IMAGE_MASK                         (0x00000400)
#define CSR_BT_START_ARCHIVE                             (0x00002000)
#define CSR_BT_GET_MONITORING_IMAGE_MASK                 (0x00004000)
#define CSR_BT_GET_STATUS                                (0x00010000)

#ifdef __cplusplus
}
#endif


#endif
