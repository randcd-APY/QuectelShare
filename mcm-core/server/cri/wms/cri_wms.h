/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_WMS
#define CRI_WMS

#include "utils_common.h"
#include "cri_core.h"
#include "wireless_messaging_service_v01.h"

#define CRI_WMS_MT_PP_SMS_IND 0x0001
#define CRI_WMS_MT_CB_SMS_IND 0x0002
#define CRI_WMS_MT_CB_CMAS_SMS_IND 0x0003


#define CRI_WMS_DESTINATION_NUMBER_MAX_LEN (30)
#define CRI_WMS_MESSAGE_CONTENT_MAX_LEN (160)
#define CRI_WMS_CMAS_MESSAGE_MAX_LEN (160)

typedef enum
{
    CRI_WMS_MESSAGE_CLASS_0 = 0,
    CRI_WMS_MESSAGE_CLASS_1 = 1,
    CRI_WMS_MESSAGE_CLASS_2 = 2,
    CRI_WMS_MESSAGE_CLASS_3 = 3,
    CRI_WMS_MESSAGE_CLASS_NONE = 4
}cri_wms_message_class;

typedef enum
{
    CRI_WMS_CMAE_CATEGORY_INVALID = -1,
    CRI_WMS_CMAE_CATEGORY_GEO = 0x00,
    CRI_WMS_CMAE_CATEGORY_MET = 0x01,
    CRI_WMS_CMAE_CATEGORY_SAFETY = 0x02,
    CRI_WMS_CMAE_CATEGORY_SECURITY = 0x03,
    CRI_WMS_CMAE_CATEGORY_RESCUE = 0x04,
    CRI_WMS_CMAE_CATEGORY_FIRE = 0x05,
    CRI_WMS_CMAE_CATEGORY_HEALTH = 0x06,
    CRI_WMS_CMAE_CATEGORY_ENV = 0x07,
    CRI_WMS_CMAE_CATEGORY_TRANSPORT = 0x08,
    CRI_WMS_CMAE_CATEGORY_INFRA = 0x09,
    CRI_WMS_CMAE_CATEGORY_CBRNE = 0x0A,
    CRI_WMS_CMAE_CATEGORY_OTHER = 0x0B
} cri_wms_cmae_category;

typedef enum
{
    CRI_WMS_CMAE_RESPONSE_TYPE_INVALID = -1,
    CRI_WMS_CMAE_RESPONSE_TYPE_SHELTER = 0x00,
    CRI_WMS_CMAE_RESPONSE_TYPE_EVACUATE = 0x01,
    CRI_WMS_CMAE_RESPONSE_TYPE_PREPARE = 0x02,
    CRI_WMS_CMAE_RESPONSE_TYPE_EXECUTE = 0x03,
    CRI_WMS_CMAE_RESPONSE_TYPE_MONITOR = 0x04,
    CRI_WMS_CMAE_RESPONSE_TYPE_AVOID = 0x05,
    CRI_WMS_CMAE_RESPONSE_TYPE_ASSESS = 0x06,
    CRI_WMS_CMAE_RESPONSE_TYPE_NONE = 0x07
} cri_wms_cmae_response_type;

typedef enum
{
    CRI_WMS_CMAE_SEVERITY_INVALID = -1,
    CRI_WMS_CMAE_SEVERITY_EXTREME = 0,
    CRI_WMS_CMAE_SEVERITY_SEVERE = 1
} cri_wms_cmae_severity;

typedef enum
{
    CRI_WMS_CMAE_URGENCY_INVALID = -1,
    CRI_WMS_CMAE_URGENCY_IMMEDIATE = 0,
    CRI_WMS_CMAE_URGENCY_EXPECTED = 1
} cri_wms_cmae_urgency;

typedef enum
{
    CRI_WMS_CMAE_CERTAINTY_INVALID = -1,
    CRI_WMS_CMAE_CERTAINTY_OBSERVED = 0,
    CRI_WMS_CMAE_CERTAINTY_LIKELY = 1
} cri_wms_cmae_certainty;

typedef enum
{
    CRI_WMS_MESSAGE_PROTOCOL_INVALID = -1,
    CRI_WMS_MESSAGE_PROTOCOL_CDMA = 0x00,
    CRI_WMS_MESSAGE_PROTOCOL_WCDMA = 0x01
} cri_wms_message_protocol;

typedef struct cri_cmae_record_type_0_t
{
    char message_content[CRI_WMS_CMAS_MESSAGE_MAX_LEN];
    uint8_t message_content_len;
} cri_wms_cmae_record_type_0;

typedef struct cri_cmae_record_type_1_t
{
    cri_wms_cmae_category category;
    cri_wms_cmae_response_type response;
    cri_wms_cmae_severity severity;
    cri_wms_cmae_urgency urgency;
    cri_wms_cmae_certainty certainty;
} cri_wms_cmae_record_type_1;

typedef struct cri_wms_cmae_expire_type_t
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} cri_wms_cmae_expire_type;

typedef struct cri_wms_cmae_record_type_2_t
{
    uint16_t id;
    uint8_t alert_handling;
    cri_wms_cmae_expire_type expire;
    uint8_t language;
} cri_wms_cmae_record_type_2;

typedef struct cri_wms_mt_pp_sms_type
{
    uint32_t transaction_id;
    wms_message_format_enum_v01 format;
    char source_number[CRI_WMS_DESTINATION_NUMBER_MAX_LEN];
    char message_content[CRI_WMS_MESSAGE_CONTENT_MAX_LEN];
    uint8_t message_class_valid;
    cri_wms_message_class message_class;
    uint8_t send_ack;
} cri_wms_mt_pp_sms_type;

typedef struct cri_wms_mt_cb_sms_type
{
    wms_message_format_enum_v01 format;
    char message_content[CRI_WMS_MESSAGE_CONTENT_MAX_LEN];
} cri_wms_mt_cb_sms_type;

typedef struct cri_wms_mt_cb_cmas_sms_type
{
    uint8_t type_0_record_valid;
    cri_wms_cmae_record_type_0 type_0_record;

    uint8_t type_1_record_valid;
    cri_wms_cmae_record_type_1 type_1_record;

    uint8_t type_2_record_valid;
    cri_wms_cmae_record_type_2 type_2_record;
} cri_wms_mt_cb_cmas_sms_type;

typedef enum
{
    CRI_WMS_MO_PP_SMS_TYPE_NOT_CONCATENATED = 0,
    CRI_WMS_MO_PP_SMS_TYPE_CONCATENATED = 1
} cri_wms_mo_pp_sms_type;

qmi_error_type_v01 cri_wms_init_client(
    hlos_ind_cb_type hlos_ind_cb
);
void cri_wms_release_client(
    int qmi_service_client_id
);
void cri_wms_async_resp_handler(
    int qmi_service_client_id,
    unsigned long message_id,
    void *resp_data,
    int resp_data_len,
    cri_core_context_type cri_core_context
);
void cri_wms_unsol_ind_handler(
    int qmi_service_client_id,
    unsigned long message_id,
    void *ind_data,
    int ind_data_len
);
qmi_error_type_v01 cri_wms_send_gw_sms(
    cri_core_context_type cri_core_context,
    char *destination_number,
    char *message_content,
    int message_content_len,
    void *hlos_cb_data,
    hlos_resp_cb_type hlos_resp_cb,
    cri_wms_mo_pp_sms_type concatenated,
    int seg_number,
    int total_segments
);

qmi_error_type_v01 cri_wms_send_cdma_sms(
    cri_core_context_type cri_core_context,
    char *destination_number,
    char *message_content,
    int message_content_len,
    void *hlos_cb_data,
    hlos_resp_cb_type hlos_resp_cb
);

qmi_error_type_v01 cri_wms_send_ack(
    uint32_t transaction_id,
    cri_wms_message_protocol message_protocol,
    void *hlos_cb_data,
    hlos_resp_cb_type hlos_resp_cb
);




#endif
