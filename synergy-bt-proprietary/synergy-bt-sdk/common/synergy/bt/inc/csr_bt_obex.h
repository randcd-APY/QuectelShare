#ifndef CSR_BT_CSR_BT_OBEX_H__
#define CSR_BT_CSR_BT_OBEX_H__

/****************************************************************************

Copyright (c) 2001-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "rfcomm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrUint8  CsrBtObexResponseCode;
typedef CsrUint16 CsrBtObexOperationStatusCode;

/* --------------------------------------------------------------
    Defines the IrOBEX Response Code values.

      These resonse codes are taken from the
      IrDA Object Exchange Protocol version 1.2
   --------------------------------------------------------------*/
#define CSR_BT_OBEX_CONTINUE_RESPONSE_CODE                                         ((CsrBtObexResponseCode) (0x10))
#define CSR_BT_OBEX_SUCCESS_RESPONSE_CODE                                          ((CsrBtObexResponseCode) (0x20))
#define CSR_BT_OBEX_CREATED_RESPONSE_CODE                                          ((CsrBtObexResponseCode) (0x21))
#define CSR_BT_OBEX_ACCEPTED_RESPONSE_CODE                                         ((CsrBtObexResponseCode) (0x22))
#define CSR_BT_OBEX_NON_AUTHORITATIVE_INFORMATION_RESPONSE_CODE                    ((CsrBtObexResponseCode) (0x23))
#define CSR_BT_OBEX_NO_CONTENT_RESPONSE_CODE                                       ((CsrBtObexResponseCode) (0x24))
#define CSR_BT_OBEX_RESET_CONTENT_RESPONSE_CODE                                    ((CsrBtObexResponseCode) (0x25))
#define CSR_BT_OBEX_PARTIAL_CONTENT_RESPONSE_CODE                                  ((CsrBtObexResponseCode) (0x26))
#define CSR_BT_OBEX_MULTIPLE_CHOICES_RESPONSE_CODE                                 ((CsrBtObexResponseCode) (0x30))
#define CSR_BT_OBEX_MOVED_PERMANENTLY_RESPONSE_CODE                                ((CsrBtObexResponseCode) (0x31))
#define CSR_BT_OBEX_MOVED_TEMPORARILY_RESPONSE_CODE                                ((CsrBtObexResponseCode) (0x32))
#define CSR_BT_OBEX_NOT_MODIFIED_RESPONSE_CODE                                     ((CsrBtObexResponseCode) (0x34))
#define CSR_BT_OBEX_USE_PROXY_RESPONSE_CODE                                        ((CsrBtObexResponseCode) (0x35))
#define CSR_BT_OBEX_BAD_REQUEST_RESPONSE_CODE                                      ((CsrBtObexResponseCode) (0x40))
#define CSR_BT_OBEX_UNAUTHORIZED_CODE                                              ((CsrBtObexResponseCode) (0x41))
#define CSR_BT_OBEX_PAYMENT_REQUIRED_RESPONSE_CODE                                 ((CsrBtObexResponseCode) (0x42))
#define CSR_BT_OBEX_FORBIDDEN_RESPONSE_CODE                                        ((CsrBtObexResponseCode) (0x43))
#define CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE                                        ((CsrBtObexResponseCode) (0x44))
#define CSR_BT_OBEX_METHOD_NOT_ALLOWED_RESPONSE_CODE                               ((CsrBtObexResponseCode) (0x45))
#define CSR_BT_OBEX_NOT_ACCEPTABLE_RESPONSE_CODE                                   ((CsrBtObexResponseCode) (0x46))
#define CSR_BT_OBEX_PROXY_AUTHENTICATION_REQUIRED_RESPONSE_CODE                    ((CsrBtObexResponseCode) (0x47))
#define CSR_BT_OBEX_REQUEST_TIME_OUT_RESPONSE_CODE                                 ((CsrBtObexResponseCode) (0x48))
#define CSR_BT_OBEX_CONFLICT_RESPONSE_CODE                                         ((CsrBtObexResponseCode) (0x49))
#define CSR_BT_OBEX_GONE_RESPONSE_CODE                                             ((CsrBtObexResponseCode) (0x4A))
#define CSR_BT_OBEX_LENGTH_REQUIRED_RESPONSE_CODE                                  ((CsrBtObexResponseCode) (0x4B))
#define CSR_BT_OBEX_PRECONDITION_FAILED_RESPONSE_CODE                              ((CsrBtObexResponseCode) (0x4C))
#define CSR_BT_OBEX_TOO_LARGE_RESPONSE_CODE                                        ((CsrBtObexResponseCode) (0x4D))
#define CSR_BT_OBEX_UNSUPPORTED_MEDIA_RESPONSE_CODE                                ((CsrBtObexResponseCode) (0x4F))
#define CSR_BT_OBEX_INTERNAL_SERVER_ERROR_RESPONSE_CODE                            ((CsrBtObexResponseCode) (0x50))
#define CSR_BT_OBEX_NOT_IMPLEMENTED_RESPONSE_CODE                                  ((CsrBtObexResponseCode) (0x51))
#define CSR_BT_OBEX_BAD_GATEWAY_RESPONSE_CODE                                      ((CsrBtObexResponseCode) (0x52))
#define CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE                              ((CsrBtObexResponseCode) (0x53))
#define CSR_BT_OBEX_GATEWAY_TIMEOUT                                                ((CsrBtObexResponseCode) (0x54))
#define CSR_BT_OBEX_HTTP_VERSION_NOT_SUPPORTED_RESPONSE_CODE                       ((CsrBtObexResponseCode) (0x55))
#define CSR_BT_OBEX_DATABASE_FULL_RESPONSE_CODE                                    ((CsrBtObexResponseCode) (0x60))
#define CSR_BT_OBEX_DATABASE_LOCKED_RESPONSE_CODE                                  ((CsrBtObexResponseCode) (0x61))

/* --------------------------------------------------------------
    Defines Intern used OBEX CsrBtResultCode.
   --------------------------------------------------------------*/
#define CSR_BT_RESULT_CODE_OBEX_SUCCESS                                             ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_OBEX_UNACCEPTABLE_PARAMETER                              ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_OBEX_CONNECT_CANCELLED                                   ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_OBEX_MAX_PACKET_LENGTH_TO_SMALL                          ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_OBEX_DIGEST_RESPONSE_HEADER_MISSING                      ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_OBEX_USERID_INVALID                                      ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_OBEX_PASSWORD_INVALID                                    ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_OBEX_CONNECTIONID_HEADER_MISSING                         ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_OBEX_TRANSPORT_CONNECTION_CLOSED_DIRECT                  ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_OBEX_AUTHENTICATION_NOT_SUPPORTED                        ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_OBEX_MULTIPLE_PUT_OPERATION_NOT_SUPPORTED                ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_OBEX_LOCAL_DEFINED_OBEX_HEADER_TOO_LARGE                 ((CsrBtResultCode) (0x000B))
#define CSR_BT_RESULT_CODE_OBEX_INCOMING_OBEX_PACKET_TOO_LARGE                      ((CsrBtResultCode) (0x000C))
#define CSR_BT_RESULT_CODE_OBEX_INVALID_OBEX_ABORT_RESPONSE_CODE                    ((CsrBtResultCode) (0x000D))
#define CSR_BT_RESULT_CODE_OBEX_INVALID_OBEX_DISCONNECT_RESPONSE_CODE               ((CsrBtResultCode) (0x000E))
#define CSR_BT_RESULT_CODE_OBEX_INTERNAL_ERROR                                      ((CsrBtResultCode) (0x000F))
#define CSR_BT_RESULT_CODE_OBEX_WHO_HEADER_MISSING                                  ((CsrBtResultCode) (0x0010))
#define CSR_BT_RESULT_CODE_OBEX_INCOMING_OBEX_PACKET_INVALID                        ((CsrBtResultCode) (0x0011))
#define CSR_BT_RESULT_CODE_OBEX_REQUEST_RECEIVED_IN_INVALID_STATE                   ((CsrBtResultCode) (0x0012))
#define CSR_BT_RESULT_CODE_OBEX_INVALID_TARGET                                      ((CsrBtResultCode) (0x0013))
#define CSR_BT_RESULT_CODE_OBEX_LOCAL_FEATURE_NOT_SUPPORTED                         ((CsrBtResultCode) (0x0014))
#define CSR_BT_RESULT_CODE_OBEX_SECONDARY_CHANNEL_ERROR                             ((CsrBtResultCode) (0x0015))
#define CSR_BT_RESULT_CODE_OBEX_RESPONSE_TIMEOUT                                    ((CsrBtResultCode) (0x0016))
#define CSR_BT_RESULT_CODE_OBEX_ABORTED_BY_LOCAL_DEVICE                             ((CsrBtResultCode) (0x0017))
#define CSR_BT_RESULT_CODE_OBEX_SRM_VIOLATION_BY_REMOTE_DEVICE                      ((CsrBtResultCode) (0x0018))

/* --------------------------------------------------------------
    Defines Intern used OBEX CsrBtObexOperationStatusCode.

    Note these are only used by the BPP profiles and are taken
    from the Basic Printing Profile vewrsion 1.2.

    At a minimum, the Printer shall be able to respond with at
    least three different error codes. (one successful code, one
    client error code, and one server error code).

    The Sender may choose to group specific error codes into one
    of three categories. (successful, client error, or server error
   --------------------------------------------------------------*/
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SUCCESSFUL_OK                                     ((CsrBtObexOperationStatusCode) (0x0000))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SUCCESSFUL_OK_IGNORED_OR_SUBSTITUTED_ATTRIBUTES   ((CsrBtObexOperationStatusCode) (0x0001))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SUCCESSFUL_OK_CONFLICTING_ATTRIBUTES              ((CsrBtObexOperationStatusCode) (0x0002))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_BAD_REQUEST                          ((CsrBtObexOperationStatusCode) (0x0400))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_FORBIDDEN                            ((CsrBtObexOperationStatusCode) (0x0401))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_NOT_AUTHENTICATED                    ((CsrBtObexOperationStatusCode) (0x0402))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_NOT_AUTHORIZED                       ((CsrBtObexOperationStatusCode) (0x0403))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_NOT_POSSIBLE                         ((CsrBtObexOperationStatusCode) (0x0404))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_TIMEOUT                              ((CsrBtObexOperationStatusCode) (0x0405))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_NOT_FOUND                            ((CsrBtObexOperationStatusCode) (0x0406))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_GONE                                 ((CsrBtObexOperationStatusCode) (0x0407))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_REQUEST_ENTITY_TOO_LARGE             ((CsrBtObexOperationStatusCode) (0x0408))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_REQUEST_VALUE_TOO_LONG               ((CsrBtObexOperationStatusCode) (0x0409))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_DOCUMENT_FORMAT_NOT_SUPPORTED        ((CsrBtObexOperationStatusCode) (0x040A))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_ATTRIBUTES_OR_VALUES_NOT_SUPPORTED   ((CsrBtObexOperationStatusCode) (0x040B))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_URI_SCHEME_NOT_SUPPORTED             ((CsrBtObexOperationStatusCode) (0x040C))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_CHARSET_NOT_SUPPORTED                ((CsrBtObexOperationStatusCode) (0x040D))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_CONFLICTING_ATTRIBUTES               ((CsrBtObexOperationStatusCode) (0x040E))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_COMPRESSION_NOT_SUPPORTED            ((CsrBtObexOperationStatusCode) (0x040F))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_COMPRESSION_ERROR                    ((CsrBtObexOperationStatusCode) (0x0410))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_DOCUMENT_FORMAT_ERROR                ((CsrBtObexOperationStatusCode) (0x0411))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_DOCUMENT_ACCESS_ERROR                ((CsrBtObexOperationStatusCode) (0x0412))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_CLIENT_ERROR_MEDIA_NOT_LOADED                     ((CsrBtObexOperationStatusCode) (0x0418))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_INTRERNAL_ERROR                      ((CsrBtObexOperationStatusCode) (0x0500))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_OPERATION_NOT_SUPPORTED              ((CsrBtObexOperationStatusCode) (0x0501))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_SERVICE_UNAVAILABLE                  ((CsrBtObexOperationStatusCode) (0x0502))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_VERSION_NOT_SUPPORTED                ((CsrBtObexOperationStatusCode) (0x0503))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_DEVICE_ERROR                         ((CsrBtObexOperationStatusCode) (0x0504))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_TEMPORARY_ERROR                      ((CsrBtObexOperationStatusCode) (0x0505))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_NOT_ACCEPTING_JOBS                   ((CsrBtObexOperationStatusCode) (0x0506))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_BUSY                                 ((CsrBtObexOperationStatusCode) (0x0507))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_JOB_CANCELED                         ((CsrBtObexOperationStatusCode) (0x0508))
#define CSR_BT_OBEX_OPERATION_STATUS_CODE_SERVER_ERROR_MULTIPLE_DOCUMENT_JOBS_NOT_SUPPORTED ((CsrBtObexOperationStatusCode) (0x0509))


/********************* defines which may be adjusted ***************************/
#define CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH  20


/* Max length for the different type's defined, including zero terminator */
#define CSR_BT_OBEX_MAX_TYPE_LENGTH                                 0x0017

#define CSR_BT_NO_BODY_HEADER                                       0xFFFFFFFF

/* Types */
#define CSR_BT_UNKNOWN_TYPE                                         0x00
#define CSR_BT_VCARD_TYPE                                           0x01
#define CSR_BT_VCAL_TYPE                                            0x03
#define CSR_BT_VNOTE_TYPE                                           0x05
#define CSR_BT_VMESSAGE_TYPE                                        0x06
#define CSR_BT_FOLDER_LISTING_TYPE                                  0x07
#define CSR_BT_IMG_GET_CAPABILITIES                                 0x08
#define CSR_BT_IMG_IMAGE                                            0x09
#define CSR_BT_IMG_LINKED_THUMBNAIL                                 0x0A
#define CSR_BT_IMG_LINKED_ATTACHMENT                                0x0B
#define CSR_BT_IMG_IMAGE_LISTING                                    0x0C
#define CSR_BT_IMG_IMAGE_PROPERTIES                                 0x0D
#define CSR_BT_IMG_ARCHIVE                                          0x0E
#define CSR_BT_IMG_STATUS                                           0x0F
#define CSR_BT_IMG_MONITORING                                       0x10
#define CSR_BT_SOAP_STRING                                          0x11
#define CSR_BT_PB_PHONE_BOOK_TYPE                                   0x12
#define CSR_BT_PB_VCARD_LISTING_TYPE                                0x13
#define CSR_BT_PB_VCARD_TYPE                                        0x14
#define CSR_BT_OTHER_FORMAT_TYPE                                    0xFF
#define CSR_BT_ANY_TYPE                                             0xFF

/* OPP client and server type */
#define CSR_BT_NO_TYPE_SUPPORT                                      0x0000
#define CSR_BT_VCARD_2_1_SUPPORT                                    0x0001
#define CSR_BT_VCARD_3_0_SUPPORT                                    0x0002
#define CSR_BT_VCAL_1_0_SUPPORT                                     0x0004
#define CSR_BT_ICAL_2_0_SUPPORT                                     0x0008
#define CSR_BT_VNOTE_SUPPORT                                        0x0010
#define CSR_BT_VMESSAGE_SUPPORT                                     0x0020
#define CSR_BT_OTHER_TYPE_SUPPORT                                   0x8000
#define CSR_BT_ANY_TYPE_SUPPORT                                     0xFFFF

/* MAP client and server type */
typedef CsrUint8 CsrBtMapMesSupport;
#define CSR_BT_MAP_NO_TYPE_SUPPORT                                  ((CsrBtMapMesSupport) 0x00)
#define CSR_BT_MAP_EMAIL_SUPPORT                                    ((CsrBtMapMesSupport) 0x01)
#define CSR_BT_MAP_SMS_GSM_SUPPORT                                  ((CsrBtMapMesSupport) 0x02)
#define CSR_BT_MAP_SMS_CDMA_SUPPORT                                 ((CsrBtMapMesSupport) 0x04)
#define CSR_BT_MAP_MMS_SUPPORT                                      ((CsrBtMapMesSupport) 0x08)
#define CSR_BT_MAP_IM_SUPPORT                                       ((CsrBtMapMesSupport) 0x10)
#define CSR_BT_MAP_ANY_TYPE_SUPPORT                                 ((CsrBtMapMesSupport) 0xFF)

/* MAP supported features */
typedef CsrUint32 CsrBtMapSupportedFeatures;
#define CSR_BT_MAP_SF_NONE                                    ((CsrBtMapSupportedFeatures) 0x00000000)
#define CSR_BT_MAP_SF_NOTIFICATION_REG                        ((CsrBtMapSupportedFeatures) 0x00000001)
#define CSR_BT_MAP_SF_NOTIFICATION                            ((CsrBtMapSupportedFeatures) 0x00000002)
#define CSR_BT_MAP_SF_BROWSING                                ((CsrBtMapSupportedFeatures) 0x00000004)
#define CSR_BT_MAP_SF_UPLOADING                               ((CsrBtMapSupportedFeatures) 0x00000008)
#define CSR_BT_MAP_SF_DELETE                                  ((CsrBtMapSupportedFeatures) 0x00000010)
#define CSR_BT_MAP_SF_INSTANCE_INFO                           ((CsrBtMapSupportedFeatures) 0x00000020)
#define CSR_BT_MAP_SF_EXT_EVENT_REPORT_1V1                    ((CsrBtMapSupportedFeatures) 0x00000040)
#define CSR_BT_MAP_SF_EXT_EVENT_REPORT_1V2                    ((CsrBtMapSupportedFeatures) 0x00000080)
#define CSR_BT_MAP_SF_MSG_FORMAT_1V1                          ((CsrBtMapSupportedFeatures) 0x00000100)
#define CSR_BT_MAP_SF_MSG_LISTING_FORMAT_1V1                  ((CsrBtMapSupportedFeatures) 0x00000200)
#define CSR_BT_MAP_SF_PERSISTENT_HANDLES                      ((CsrBtMapSupportedFeatures) 0x00000400)
#define CSR_BT_MAP_SF_DATABASE_IDENTIFIER                     ((CsrBtMapSupportedFeatures) 0x00000800)
#define CSR_BT_MAP_SF_FOLDER_VERSION_COUNTER                  ((CsrBtMapSupportedFeatures) 0x00001000)
#define CSR_BT_MAP_SF_CONVERSATION_VERSION_COUNTER            ((CsrBtMapSupportedFeatures) 0x00002000)
#define CSR_BT_MAP_SF_PARTICIPANT_PRESENCE_CHANGE_NOTI        ((CsrBtMapSupportedFeatures) 0x00004000)
#define CSR_BT_MAP_SF_PARTICIPANT_CHAT_STATE_CHANGE_NOTI      ((CsrBtMapSupportedFeatures) 0x00008000)
#define CSR_BT_MAP_SF_PBAP_CONTACT_CROSS_REFERENCE            ((CsrBtMapSupportedFeatures) 0x00010000)
#define CSR_BT_MAP_SF_NOTIFICATION_FILTERING                  ((CsrBtMapSupportedFeatures) 0x00020000)
#define CSR_BT_MAP_SF_UTC_OFFSET_TIMESTAMP_FORMAT             ((CsrBtMapSupportedFeatures) 0x00040000)
#define CSR_BT_MAP_SF_SUPP_FEATURES_IN_CONNECT_REQ            ((CsrBtMapSupportedFeatures) 0x00080000) /* To be ignored by (client) applications */
#define CSR_BT_MAP_SF_CONVERSATION_LISTING                    ((CsrBtMapSupportedFeatures) 0x00100000)
#define CSR_BT_MAP_SF_OWNER_STATUS                            ((CsrBtMapSupportedFeatures) 0x00200000)

#define CSR_BT_MAP_SUPPORTED_FEATURE_ALL                            ((CsrBtMapSupportedFeatures) 0x003FFFFF)

/* Default features as from specification */
#define CSR_BT_MAP_SF_DEFAULT                                       ((CsrBtMapSupportedFeatures) 0x0000001F)


typedef CsrUint32 CsrBtMapMesParms;
#define CSR_BT_MAP_PARAMETERS_ALL                                   ((CsrBtMapMesParms) 0x00000000)
#define CSR_BT_MAP_PARAMETERS_SUBJECT                               ((CsrBtMapMesParms) 0x00000001)
#define CSR_BT_MAP_PARAMETERS_DATETIME                              ((CsrBtMapMesParms) 0x00000002)
#define CSR_BT_MAP_PARAMETERS_SENDER_NAME                           ((CsrBtMapMesParms) 0x00000004)
#define CSR_BT_MAP_PARAMETERS_SENDER_ADDRESSING                     ((CsrBtMapMesParms) 0x00000008)
#define CSR_BT_MAP_PARAMETERS_RECIPIENT_NAME                        ((CsrBtMapMesParms) 0x00000010)
#define CSR_BT_MAP_PARAMETERS_RECIPIENT_ADRESSING                   ((CsrBtMapMesParms) 0x00000020)
#define CSR_BT_MAP_PARAMETERS_TYPE                                  ((CsrBtMapMesParms) 0x00000040)
#define CSR_BT_MAP_PARAMETERS_SIZE                                  ((CsrBtMapMesParms) 0x00000080)
#define CSR_BT_MAP_PARAMETERS_RECEPTION_STATUS                      ((CsrBtMapMesParms) 0x00000100)
#define CSR_BT_MAP_PARAMETERS_TEXT                                  ((CsrBtMapMesParms) 0x00000200)
#define CSR_BT_MAP_PARAMETERS_ATTACHMENT_SIZE                       ((CsrBtMapMesParms) 0x00000400)
#define CSR_BT_MAP_PARAMETERS_PRIORITY                              ((CsrBtMapMesParms) 0x00000800)
#define CSR_BT_MAP_PARAMETERS_READ                                  ((CsrBtMapMesParms) 0x00001000)
#define CSR_BT_MAP_PARAMETERS_SENT                                  ((CsrBtMapMesParms) 0x00002000)
#define CSR_BT_MAP_PARAMETERS_PROTECTED                             ((CsrBtMapMesParms) 0x00004000)
#define CSR_BT_MAP_PARAMETERS_REPLYTO_ADDRESSING                    ((CsrBtMapMesParms) 0x00008000)
#define CSR_BT_MAP_PARAMETERS_DELIVERY_STATUS                       ((CsrBtMapMesParms) 0x00010000)
#define CSR_BT_MAP_PARAMETERS_CONVERSATION_ID                       ((CsrBtMapMesParms) 0x00020000)
#define CSR_BT_MAP_PARAMETERS_CONVERSATION_NAME                     ((CsrBtMapMesParms) 0x00040000)
#define CSR_BT_MAP_PARAMETERS_DIRECTION                             ((CsrBtMapMesParms) 0x00080000)
#define CSR_BT_MAP_PARAMETERS_ATTACHMENT_MIME                       ((CsrBtMapMesParms) 0x00100000)

typedef CsrUint8 CsrBtMapMesTypes;
#define CSR_BT_MAP_TYPE_ALL                                         ((CsrBtMapMesTypes) 0x00)
#define CSR_BT_MAP_TYPE_NO_SMS_GSM                                  ((CsrBtMapMesTypes) 0x01)
#define CSR_BT_MAP_TYPE_NO_SMS_CDMA                                 ((CsrBtMapMesTypes) 0x02)
#define CSR_BT_MAP_TYPE_NO_EMAIL                                    ((CsrBtMapMesTypes) 0x04)
#define CSR_BT_MAP_TYPE_NO_MSS                                      ((CsrBtMapMesTypes) 0x08)
#define CSR_BT_MAP_TYPE_NO_MMS                                      ((CsrBtMapMesTypes) 0x08) /* CSR_BT_MAP_TYPE_NO_MSS should have been called CSR_BT_MAP_TYPE_NO_MMS */
#define CSR_BT_MAP_TYPE_NO_IM                                       ((CsrBtMapMesTypes) 0x10)

typedef CsrUint8 CsrBtMapReadStatus;
#define CSR_BT_MAP_STATUS_NO_FILTERING                              ((CsrBtMapReadStatus) 0x00)
#define CSR_BT_MAP_STATUS_UNREAD                                    ((CsrBtMapReadStatus) 0x01)
#define CSR_BT_MAP_STATUS_READ                                      ((CsrBtMapReadStatus) 0x02)

typedef CsrUint8 CsrBtMapPriority;
#define CSR_BT_MAP_PRIORITY_NO_FILTERING                            ((CsrBtMapPriority) 0x00)
#define CSR_BT_MAP_PRIORITY_HIGH                                    ((CsrBtMapPriority) 0x01)
#define CSR_BT_MAP_PRIORITY_NON_HIGH                                ((CsrBtMapPriority) 0x02)

typedef CsrUint8 CsrBtMapNewMessage;
#define CSR_BT_MAP_NEW_MESSAGE_OFF                                  ((CsrBtMapNewMessage) 0x00)
#define CSR_BT_MAP_NEW_MESSAGE_ON                                   ((CsrBtMapNewMessage) 0x01)

typedef CsrUint8 CsrBtMapAttachment;
#define CSR_BT_MAP_ATTACHMENT_OFF                                   ((CsrBtMapAttachment) 0x00)
#define CSR_BT_MAP_ATTACHMENT_ON                                    ((CsrBtMapAttachment) 0x01)

typedef CsrUint8 CsrBtMapCharset;
#define CSR_BT_MAP_CHARSET_NATIVE                                   ((CsrBtMapCharset) 0x00)
#define CSR_BT_MAP_CHARSET_UTF8                                     ((CsrBtMapCharset) 0x01)

typedef CsrUint8 CsrBtMapFracReq;
#define CSR_BT_MAP_FRACTION_REQ_NOT_USED                            ((CsrBtMapFracReq) 0xFF)
#define CSR_BT_MAP_FRACTION_REQ_FIRST                               ((CsrBtMapFracReq) 0x00)
#define CSR_BT_MAP_FRACTION_REQ_NEXT                                ((CsrBtMapFracReq) 0x01)

typedef CsrUint8 CsrBtMapFracDel;
#define CSR_BT_MAP_FRACTION_DEL_NOT_SPECIFIED                       ((CsrBtMapFracDel) 0xFF)
#define CSR_BT_MAP_FRACTION_DEL_MORE                                ((CsrBtMapFracDel) 0x00)
#define CSR_BT_MAP_FRACTION_DEL_LAST                                ((CsrBtMapFracDel) 0x01)

typedef CsrUint8 CsrBtMapStatusInd;
#define CSR_BT_MAP_STATUS_IND_READ                                  ((CsrBtMapStatusInd) 0x00)
#define CSR_BT_MAP_STATUS_IND_DELETE                                ((CsrBtMapStatusInd) 0x01)
#define CSR_BT_MAP_STATUS_IND_EXTENDED_DATA                         ((CsrBtMapStatusInd) 0x02)

typedef CsrUint8 CsrBtMapStatusVal;
#define CSR_BT_MAP_STATUS_VAL_NO                                    ((CsrBtMapStatusVal) 0x00)
#define CSR_BT_MAP_STATUS_VAL_YES                                   ((CsrBtMapStatusVal) 0x01)

typedef CsrUint8 CsrBtMapTrans;
#define CSR_BT_MAP_TRANSPARENT_NOT_SPECIFIED                        ((CsrBtMapTrans) 0xFF)
#define CSR_BT_MAP_TRANSPARENT_OFF                                  ((CsrBtMapTrans) 0x00)
#define CSR_BT_MAP_TRANSPARENT_ON                                   ((CsrBtMapTrans) 0x01)

typedef CsrUint8 CsrBtMapRetry;
#define CSR_BT_MAP_RETRY_NOT_SPECIFIED                              ((CsrBtMapRetry) 0xFF)
#define CSR_BT_MAP_RETRY_OFF                                        ((CsrBtMapRetry) 0x00)
#define CSR_BT_MAP_RETRY_ON                                         ((CsrBtMapRetry) 0x01)

typedef CsrUint8 CsrBtMapTagId;
#define CSR_BT_MAP_TAG_ID_MAX_LIST_COUNT                            ((CsrBtMapTagId) 0x01)
#define CSR_BT_MAP_TAG_ID_START_OFFSET                              ((CsrBtMapTagId) 0x02)
#define CSR_BT_MAP_TAG_ID_FILTER_MESSAGE_TYPE                       ((CsrBtMapTagId) 0x03)
#define CSR_BT_MAP_TAG_ID_FILTER_PERIOD_BEGIN                       ((CsrBtMapTagId) 0x04)
#define CSR_BT_MAP_TAG_ID_FILTER_PERIOD_END                         ((CsrBtMapTagId) 0x05)
#define CSR_BT_MAP_TAG_ID_FILTER_READ_STATUS                        ((CsrBtMapTagId) 0x06)
#define CSR_BT_MAP_TAG_ID_FILTER_RECIPIENT                          ((CsrBtMapTagId) 0x07)
#define CSR_BT_MAP_TAG_ID_FILTER_ORIGINATOR                         ((CsrBtMapTagId) 0x08)
#define CSR_BT_MAP_TAG_ID_FILTER_PRIORITY                           ((CsrBtMapTagId) 0x09)
#define CSR_BT_MAP_TAG_ID_ATTACHMENT                                ((CsrBtMapTagId) 0x0A)
#define CSR_BT_MAP_TAG_ID_TRANSPARENT                               ((CsrBtMapTagId) 0x0B)
#define CSR_BT_MAP_TAG_ID_RETRY                                     ((CsrBtMapTagId) 0x0C)
#define CSR_BT_MAP_TAG_ID_NEW_MESSAGE                               ((CsrBtMapTagId) 0x0D)
#define CSR_BT_MAP_TAG_ID_NOTIFICATION_STATUS                       ((CsrBtMapTagId) 0x0E)
#define CSR_BT_MAP_TAG_ID_MAS_INSTANCE_ID                           ((CsrBtMapTagId) 0x0F)
#define CSR_BT_MAP_TAG_ID_PARAMETER_MASK                            ((CsrBtMapTagId) 0x10)
#define CSR_BT_MAP_TAG_ID_FOLDER_LISTING_SIZE                       ((CsrBtMapTagId) 0x11)
#define CSR_BT_MAP_TAG_ID_MESSAGES_LISTING_SIZE                     ((CsrBtMapTagId) 0x12)
#define CSR_BT_MAP_TAG_ID_SUBJECT_LENGTH                            ((CsrBtMapTagId) 0x13)
#define CSR_BT_MAP_TAG_ID_CHARSET                                   ((CsrBtMapTagId) 0x14)
#define CSR_BT_MAP_TAG_ID_FRACTION_REQUEST                          ((CsrBtMapTagId) 0x15)
#define CSR_BT_MAP_TAG_ID_FRACTION_DELIVER                          ((CsrBtMapTagId) 0x16)
#define CSR_BT_MAP_TAG_ID_STATUS_INDICATOR                          ((CsrBtMapTagId) 0x17)
#define CSR_BT_MAP_TAG_ID_STATUS_VALUE                              ((CsrBtMapTagId) 0x18)
#define CSR_BT_MAP_TAG_ID_MSE_TIME                                  ((CsrBtMapTagId) 0x19)
#define CSR_BT_MAP_TAG_ID_DATABASE_ID                               ((CsrBtMapTagId) 0x1A)
#define CSR_BT_MAP_TAG_ID_CONV_LIST_VER_COUNTER                     ((CsrBtMapTagId) 0x1B)
#define CSR_BT_MAP_TAG_ID_PRESENCE_AVAILABILITY                     ((CsrBtMapTagId) 0x1C)
#define CSR_BT_MAP_TAG_ID_PRESENCE_TEXT                             ((CsrBtMapTagId) 0x1D)
#define CSR_BT_MAP_TAG_ID_LAST_ACTIVTY                              ((CsrBtMapTagId) 0x1E)
#define CSR_BT_MAP_TAG_ID_FILTER_LAST_ACTIVITY_BEGIN                ((CsrBtMapTagId) 0x1F)
#define CSR_BT_MAP_TAG_ID_FILTER_LAST_ACTIVITY_END                  ((CsrBtMapTagId) 0x20)
#define CSR_BT_MAP_TAG_ID_CHAT_STATE                                ((CsrBtMapTagId) 0x21)
#define CSR_BT_MAP_TAG_ID_CONVERSATION_ID                           ((CsrBtMapTagId) 0x22)
#define CSR_BT_MAP_TAG_ID_FOLDER_VER_COUNTER                        ((CsrBtMapTagId) 0x23)
#define CSR_BT_MAP_TAG_ID_FILTER_MSG_HANDLE                         ((CsrBtMapTagId) 0x24)
#define CSR_BT_MAP_TAG_ID_NOTI_FILTER_MASK                          ((CsrBtMapTagId) 0x25)
#define CSR_BT_MAP_TAG_ID_CONV_PARAMETER_MASK                       ((CsrBtMapTagId) 0x26)
#define CSR_BT_MAP_TAG_ID_OWNER_UCI                                 ((CsrBtMapTagId) 0x27)
#define CSR_BT_MAP_TAG_ID_EXTENDED_DATA                             ((CsrBtMapTagId) 0x28)
#define CSR_BT_MAP_TAG_ID_MAP_SUPPORTED_FEATURES                    ((CsrBtMapTagId) 0x29)

typedef CsrUint8 CsrBtMapTagIdLength;
#define CSR_BT_MAP_TAG_ID_LENGTH_MAX_LIST_COUNT                     ((CsrBtMapTagIdLength) 0x02)
#define CSR_BT_MAP_TAG_ID_LENGTH_START_OFFSET                       ((CsrBtMapTagIdLength) 0x02)
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_MESSAGE_TYPE                ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_PERIOD_BEGIN                ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_PERIOD_END                  ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_READ_STATUS                 ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_RECIPIENT                   ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_ORIGINATOR                  ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_PRIORITY                    ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_ATTACHMENT                         ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_TRANSPARENT                        ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_RETRY                              ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_NEW_MESSAGE                        ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_NOTIFICATION_STATUS                ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_MAS_INSTANCE_ID                    ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_PARAMETER_MASK                     ((CsrBtMapTagIdLength) 0x04)
#define CSR_BT_MAP_TAG_ID_LENGTH_FOLDER_LISTING_SIZE                ((CsrBtMapTagIdLength) 0x02)
#define CSR_BT_MAP_TAG_ID_LENGTH_MESSAGES_LISTING_SIZE              ((CsrBtMapTagIdLength) 0x02)
#define CSR_BT_MAP_TAG_ID_LENGTH_SUBJECT_LENGTH                     ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_CHARSET                            ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_FRACTION_REQUEST                   ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_FRACTION_DELIVER                   ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_STATUS_INDICATOR                   ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_STATUS_VALUE                       ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_MSE_TIME                           ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_DATABASE_ID                        ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_CONV_LIST_VER_COUNTER              ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_PRESENCE                           ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_PRESENCE_TEXT                      ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_LAST_ACTIVTY                       ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_LAST_ACTIVITY_BEGIN         ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_LAST_ACTIVITY_END           ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_CHAT_STATE                         ((CsrBtMapTagIdLength) 0x01)
#define CSR_BT_MAP_TAG_ID_LENGTH_CONVERSATION_ID                    ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FOLDER_VER_COUNTER                 ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_FILTER_MSG_HANDLE                  ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_NOTI_FILTER_MASK                   ((CsrBtMapTagIdLength) 0x04)
#define CSR_BT_MAP_TAG_ID_LENGTH_CONV_PARAMETER_MASK                ((CsrBtMapTagIdLength) 0x04)
#define CSR_BT_MAP_TAG_ID_LENGTH_OWNER_UCI                          ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_EXTENDED_DATA                      ((CsrBtMapTagIdLength) 0x00) /* variable */
#define CSR_BT_MAP_TAG_ID_LENGTH_MAP_SUPPORTED_FEATURES             ((CsrBtMapTagIdLength) 0x04)

typedef CsrUint8 CsrBtMapPresence;
#define CSR_BT_MAP_PRESENCE_UNKNOWN                                 ((CsrBtMapPresence) 0x00)
#define CSR_BT_MAP_PRESENCE_OFFLINE                                 ((CsrBtMapPresence) 0x01)
#define CSR_BT_MAP_PRESENCE_ONLINE                                  ((CsrBtMapPresence) 0x02)
#define CSR_BT_MAP_PRESENCE_AWAY                                    ((CsrBtMapPresence) 0x03)
#define CSR_BT_MAP_PRESENCE_DO_NOT_DISTURB                          ((CsrBtMapPresence) 0x04)
#define CSR_BT_MAP_PRESENCE_BUSY                                    ((CsrBtMapPresence) 0x05)
#define CSR_BT_MAP_PRESENCE_IN_A_MEETING                            ((CsrBtMapPresence) 0x06)

typedef CsrUint8 CsrBtMapChatState;
#define CSR_BT_MAP_CHAT_STATE_UNKNOWN                               ((CsrBtMapChatState) 0x00)
#define CSR_BT_MAP_CHAT_STATE_INACTIVE                              ((CsrBtMapChatState) 0x01)
#define CSR_BT_MAP_CHAT_STATE_ACTIVE                                ((CsrBtMapChatState) 0x02)
#define CSR_BT_MAP_CHAT_STATE_COMPOSING                             ((CsrBtMapChatState) 0x03)
#define CSR_BT_MAP_CHAT_STATE_PAUSED_COMPOSING                      ((CsrBtMapChatState) 0x04)
#define CSR_BT_MAP_CHAT_STATE_GONE                                  ((CsrBtMapChatState) 0x05)

typedef CsrUint32 CsrBtMapConvParams;
#define CSR_BT_MAP_CONV_PARAMS_ALL                                  ((CsrBtMapConvParams) 0x00000000)
#define CSR_BT_MAP_CONV_PARAMS_CONVERSATION_NAME                    ((CsrBtMapConvParams) 0x00000001)
#define CSR_BT_MAP_CONV_PARAMS_CONVERSATION_LAST_ACTIVITY           ((CsrBtMapConvParams) 0x00000002)
#define CSR_BT_MAP_CONV_PARAMS_CONVERSATION_READ_STATUS             ((CsrBtMapConvParams) 0x00000004)
#define CSR_BT_MAP_CONV_PARAMS_CONVERSATION_VERSION_COUNTER         ((CsrBtMapConvParams) 0x00000008)
#define CSR_BT_MAP_CONV_PARAMS_CONVERSATION_SUMMARY                 ((CsrBtMapConvParams) 0x00000010)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANTS                         ((CsrBtMapConvParams) 0x00000020)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_UCI                      ((CsrBtMapConvParams) 0x00000040)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_DISPLAY_NAME             ((CsrBtMapConvParams) 0x00000080)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_CHAT_STATE               ((CsrBtMapConvParams) 0x00000100)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_LAST_ACTIVITY            ((CsrBtMapConvParams) 0x00000200)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_X_BT_UID                 ((CsrBtMapConvParams) 0x00000400)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_NAME                     ((CsrBtMapConvParams) 0x00000800)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_PRESENCE_AVAILABILITY    ((CsrBtMapConvParams) 0x00001000)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_PRESENCE_TEXT            ((CsrBtMapConvParams) 0x00002000)
#define CSR_BT_MAP_CONV_PARAMS_PARTICIPANT_PRIORITY                 ((CsrBtMapConvParams) 0x00004000)

typedef CsrUint32 CsrBtMapNotificationFilterMask;
#define CSR_BT_MAP_NOTIFY_NEW_MESSAGE                               ((CsrBtMapNotificationFilterMask) 0x00000001)
#define CSR_BT_MAP_NOTIFY_MESSAGE_DELETED                           ((CsrBtMapNotificationFilterMask) 0x00000002)
#define CSR_BT_MAP_NOTIFY_MESSAGE_SHIFT                             ((CsrBtMapNotificationFilterMask) 0x00000004)
#define CSR_BT_MAP_NOTIFY_SENDING_SUCCESS                           ((CsrBtMapNotificationFilterMask) 0x00000008)
#define CSR_BT_MAP_NOTIFY_SENDING_FAILURE                           ((CsrBtMapNotificationFilterMask) 0x00000010)
#define CSR_BT_MAP_NOTIFY_DELIVERY_SUCCESS                          ((CsrBtMapNotificationFilterMask) 0x00000020)
#define CSR_BT_MAP_NOTIFY_DELIVERY_FAILURE                          ((CsrBtMapNotificationFilterMask) 0x00000040)
#define CSR_BT_MAP_NOTIFY_MEMORY_FULL                               ((CsrBtMapNotificationFilterMask) 0x00000080)
#define CSR_BT_MAP_NOTIFY_MEMORY_AVAILABLE                          ((CsrBtMapNotificationFilterMask) 0x00000100)
#define CSR_BT_MAP_NOTIFY_READ_STATUS_CHANGED                       ((CsrBtMapNotificationFilterMask) 0x00000200)
#define CSR_BT_MAP_NOTIFY_CONVERSATION_CHANGED                      ((CsrBtMapNotificationFilterMask) 0x00000400)
#define CSR_BT_MAP_NOTIFY_PARTICIPANT_PRESENCE_CHANGED              ((CsrBtMapNotificationFilterMask) 0x00000800)
#define CSR_BT_MAP_NOTIFY_PARTICIPANT_CHAT_STATE_CHANGED            ((CsrBtMapNotificationFilterMask) 0x00001000)
#define CSR_BT_MAP_NOTIFY_MESSAGE_EXTENDED_DATA_CHANGED             ((CsrBtMapNotificationFilterMask) 0x00002000)
#define CSR_BT_MAP_NOTIFY_MESSAGE_REMOVED                           ((CsrBtMapNotificationFilterMask) 0x00004000)
#define CSR_BT_MAP_NOTIFY_MESSAGE_ALL                               ((CsrBtMapNotificationFilterMask) 0x00007FFF)

/* SYNC client and server type */
typedef CsrUint16 CsrBtSyncStores;
#define CSR_BT_SYNC_NO_TYPE_SUPPORT                                 ((CsrBtSyncStores) 0x0000)
#define CSR_BT_SYNC_PHONEBOOK_SUPPORT                               ((CsrBtSyncStores) 0x0001)
#define CSR_BT_SYNC_CALENDAR_SUPPORT                                ((CsrBtSyncStores) 0x0002)
#define CSR_BT_SYNC_NOTE_SUPPORT                                    ((CsrBtSyncStores) 0x0004)
#define CSR_BT_SYNC_MESSAGE_SUPPORT                                 ((CsrBtSyncStores) 0x0008)
#define CSR_BT_SYNC_ANY_TYPE_SUPPORT                                ((CsrBtSyncStores) 0xFFFF)

/* Excluding zero termination */
#define CSR_BT_IMG_HANDLE_LENGTH                                    7

/* Defines the permission bits the application can used in OBEX Set Object Permissions Action Command*/
typedef CsrUint32 CsrBtObexPermissionsMask;
#define CSR_BT_OBEX_USER_PERMISSIONS_READ_MASK                      ((CsrBtObexPermissionsMask) 0x00010000)
#define CSR_BT_OBEX_USER_PERMISSIONS_WRITE_MASK                        ((CsrBtObexPermissionsMask) 0x00020000)
#define CSR_BT_OBEX_USER_PERMISSIONS_DELETE_MASK                    ((CsrBtObexPermissionsMask) 0x00040000)
#define CSR_BT_OBEX_USER_PERMISSIONS_MODIFY_MASK                    ((CsrBtObexPermissionsMask) 0x00800000)
#define CSR_BT_OBEX_GROUP_PERMISSIONS_READ_MASK                        ((CsrBtObexPermissionsMask) 0x00000100)
#define CSR_BT_OBEX_GROUP_PERMISSIONS_WRITE_MASK                    ((CsrBtObexPermissionsMask) 0x00000200)
#define CSR_BT_OBEX_GROUP_PERMISSIONS_DELETE_MASK                    ((CsrBtObexPermissionsMask) 0x00000400)
#define CSR_BT_OBEX_GROUP_PERMISSIONS_MODIFY_MASK                    ((CsrBtObexPermissionsMask) 0x00008000)
#define CSR_BT_OBEX_OTHER_PERMISSIONS_READ_MASK                        ((CsrBtObexPermissionsMask) 0x00000001)
#define CSR_BT_OBEX_OTHER_PERMISSIONS_WRITE_MASK                    ((CsrBtObexPermissionsMask) 0x00000002)
#define CSR_BT_OBEX_OTHER_PERMISSIONS_DELETE_MASK                   ((CsrBtObexPermissionsMask) 0x00000004)
#define CSR_BT_OBEX_OTHER_PERMISSIONS_MODIFY_MASK                    ((CsrBtObexPermissionsMask) 0x00000080)


/* prototypes */
extern int    CsrBtStrICmp(char * string1, char * string2);
extern int    CsrBtStrICmpLength(char * string1, char * string2, CsrUint16 theLength);

extern CsrUint8 *CsrBtObexLittle2BigEndian(CsrUint8 *szName);
extern CsrUint8 *CsrBtObexLittle2BigEndianN(CsrUint8 *szName, int count);

#ifdef CSR_BT_INSTALL_OBEX_STRING_DESCRIPTION
extern CsrUint16 CsrBtReturnImgDescriptionLength(char * encoding, char * pixel, char * size, char * maxSize, char * transformation);
extern CsrUint16 CsrBtReturnImgAttachDescriptionLength(char * contentType, char * charset, char * name, char * size, char * created);
extern CsrUint16 CsrBtReturnImgHandlesDescriptionLength(char * created, char * modified, char * encoding, char * pixel);
extern CsrUint8 * CsrBtBuildImgDescriptorHeader(char * encoding, char * pixel, char * size, char * maxSize,
                                             char * transformation, CsrUint16 descriptorLength);
extern CsrUint8 * CsrBtBuildImgAttachmentDescriptorHeader(char * contentType, char * charset, char * name,
                                                       char * size, char * created, CsrUint16 descriptorLength);
extern CsrUint8 * CsrBtBuildImgHandlesDescriptorHeader(char * created, char * modified, char * encoding, char * pixel, CsrUint16 descriptorLength);

extern CsrUint16 CsrBtReturnImgListLength(CsrUint16 nofImagesInList);

extern CsrUint8 *CsrBtBuildImgListHeader(CsrUint16 nbImageHandles, CsrUint16 imageListLength, CsrUint16 *currentLength);
extern CsrUint8 * CsrBtBuildImgListObject(CsrUint8 *imageListString,
                      char imageHandlesString[9],
                      char createdFileTimesString[18], CsrUint16 *currentLength);

extern CsrUint8 *  CsrBtBuildImgListTerminator(CsrUint8 *imageListString);
#endif



#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_OBEX_H__ */
