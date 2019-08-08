/***************************************************************************************************
    @file
    cri_core.c

    @brief
    Implements functions supported in cri_core.h.

    Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

//qmi headers
#include "qmi_idl_lib.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"

// cri headers
#include "cri_core.h"
#include "core_handler.h"
#include "cri_rule_handler.h"

//#include "circuit_switched_video_telephony_v01.h"
//#include "cri_csvt_core.h"

#include "cri_nas.h"
#include "cri_wms.h"
#include "cri_dms.h"
#include "cri_uim_core.h"

#include "voice_service_v02.h"
#include "cri_voice_core.h"

#define CRI_CORE_MAX_TRIES_FOR_QMI_INIT (10)
#define CRI_CORE_DELAY_FOR_QMI_INIT (1)

#define CRI_CORE_MAX_LOG_CONTEXT_BUFFER_SIZE (256)
#define CRI_CORE_MAX_HLOS_TOKEN_ID (0xFFFFFFFF) //32 bits
#define CRI_CORE_CRI_HLOS_TOKEN_ID_POS (33)
#define CRI_CORE_MIN_CRI_TOKEN_ID (0x0000001)
#define CRI_CORE_MAX_CRI_TOKEN_ID (0xFFFF) //16 bits
#define CRI_CORE_MAX_CRI_SUBSCRIPTION_ID (0xF) //4 bits
#define CRI_CORE_MAX_CRI_SUBSCRIPTION_ID_BITS (4)
#define CRI_CORE_CRI_SUBSCRIPTION_ID_POS (29)

#define CRI_QMI_CLIENT_SERVICE_REGISTRATION_TIMEOUT (4) //4 seconds

typedef enum
{
    CRI_CORE_QMI_SERVICE_NOT_CONNECTED,
    CRI_CORE_QMI_SERVICE_CONNECTED,
    CRI_CORE_QMI_SERVICE_NOT_AVAILABLE,
    CRI_CORE_QMI_SERVICE_AVAILABLE,
    CRI_CORE_QMI_SERVICE_MAX
} cri_core_qmi_service_conection_state;

typedef struct cri_core_qmi_client_list
{
    int num_of_active_clients;
    int max_active_clients;
}cri_core_qmi_client_list;

typedef struct cri_core_qmi_client_info_type
{
    int is_valid;
    int qmi_client_handle;
    qmi_sys_event_rx_hdlr sys_event_rx_hdlr;
}cri_core_qmi_client_info_type;

typedef struct cri_core_qmi_service_client_info_type
{
    int is_valid;
    qmi_client_type user_handle;
    qmi_idl_service_object_type service_obj;
    qmi_service_id_type service_id;
    cri_core_subscription_id_type subscription_id;
    hlos_ind_cb_type hlos_ind_cb;
    cri_core_qmi_service_conection_state client_state;
    qmi_client_type notifier;
}cri_core_qmi_service_client_info_type;

static cri_core_gen_operational_status_type cri_core_op_status;
static cri_core_qmi_client_list client_info;
static cri_core_qmi_client_info_type qmi_client_info;
static cri_core_qmi_service_client_info_type qmi_service_client_info[CRI_CORE_MAX_CLIENTS];
static cri_core_token_id_type cri_core_token_id;
static char cri_core_log_context_buffer[CRI_CORE_MAX_LOG_CONTEXT_BUFFER_SIZE];

ssr_srv_down_cb srv_down_cb = NULL;
ssr_srv_up_cb srv_up_cb = NULL;




/***************************************************************************************************
    @function
    cri_core_get_qmi_port

    @brief
    Retrieves the QMI port that needs to be used for the target under use.

    @param[in]
        none

    @param[out]
        none

    @retval
    string containing the port name
***************************************************************************************************/
static char* cri_core_get_qmi_port();





/***************************************************************************************************
    @function
    cri_core_qmi_client_sys_event_handler

    @brief
    Handles framework specific notifications that would be delivered by the QMI framework.

    @param[in]
        event_id
            event id of the framework specific notification that is being delivered
        event_info
            information of the framework specific notification
        user_data
            user callback data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void cri_core_qmi_client_sys_event_handler(qmi_sys_event_type event_id,
                                                  const qmi_sys_event_info_type *event_info,
                                                  void *user_data);





/***************************************************************************************************
    @function
    cri_core_qmi_service_unsol_ind_cb

    @brief
    Handles QMI service indications.

    @param[in]
        user_handle
            QMI user handle of the QMI client that is receiving the indication
        message_id
            message id of the QMI indication that is being handled
        ind_buf
            encoded data of the QMI indication
        ind_buf_len
            length of the encoded data of the QMI indication
        ind_cb_data
            user indication callback data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void cri_core_qmi_service_unsol_ind_cb(qmi_client_type user_handle,
                                              unsigned long message_id,
                                              unsigned char *ind_buf,
                                              int ind_buf_len,
                                              void *ind_cb_data);





/***************************************************************************************************
    @function
    cri_core_qmi_service_async_resp_cb

    @brief
    Handles QMI async responses.

    @param[in]
        user_handle
            QMI user handle of the QMI client that is receiving the async response
        message_id
            message id of the QMI async response that is being handled
        resp_message
            decoded data of the QMI async response
        resp_message_len
            length of the decoded data of the QMI async response
        resp_cb_data
            user callback data that was specified while sending the async request
        transport_error
            error encountered while sending the async request to the QMI service server on modem

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
static void cri_core_qmi_service_async_resp_cb(qmi_client_type user_handle,
                                               unsigned long message_id,
                                               void *resp_message,
                                               int resp_message_len,
                                               void *resp_cb_data,
                                               qmi_client_error_type transport_error);





/***************************************************************************************************
    @function
    cri_core_generate_cri_token_id

    @brief
    Generates CRI internal token id.

    @param[in]
        none

    @param[out]
        none

    @retval
    CRI internal token id
***************************************************************************************************/
static cri_core_token_id_type cri_core_generate_cri_token_id();






/***************************************************************************************************
    @function
    cri_core_get_qmi_port

    @implementation detail
    None.
***************************************************************************************************/
char* cri_core_get_qmi_port()
{
    char *qmi_port;

    qmi_port = NULL;

    qmi_port = QMI_PORT_RMNET_0;

    return qmi_port;
}

/***************************************************************************************************
    @function
    cri_core_qmi_client_sys_event_handler

    @implementation detail
    None.
***************************************************************************************************/
void cri_core_qmi_client_sys_event_handler(qmi_sys_event_type event_id,
                                           const qmi_sys_event_info_type *event_info,
                                           void *user_data)
{
    //TODO : Handler SSR and call Front end's handler - qmi_client_info.sys_event_rx_hdlr
}

/***************************************************************************************************
    @function
    cri_core_start

    @implementation detail
    None.
***************************************************************************************************/
int cri_core_start()
{
    int err_code;

    err_code = ENOMEM;

    err_code = cri_rule_handler_init();

    UTIL_LOG_MSG("error %d",
                 err_code);

    return err_code;
}

/***************************************************************************************************
    @function
    cri_core_cri_client_init

    @implementation detail
    Initializes required services based on service id.
***************************************************************************************************/
qmi_error_type_v01 cri_core_cri_client_init(cri_core_cri_client_init_info_type *client_init_info)
{
    qmi_error_type_v01 ret_val;
    qmi_error_type_v01 client_init_error;
    int iter_client_init;
    int services_num;

    ret_val = QMI_ERR_INTERNAL_V01;
    client_init_error = QMI_ERR_NONE_V01;
    iter_client_init = NIL;
    services_num = NIL;

    if(client_init_info)
    {
        //TODO: Need to call cri_core_create_qmi_client when we move to CRF completely

        services_num = client_init_info->number_of_cri_services_to_be_initialized;
        for(iter_client_init = 0; iter_client_init < services_num; iter_client_init++)
        {
            switch(client_init_info->service_info[iter_client_init].cri_service_id)
            {
                /*case QMI_CSVT_SERVICE:
                    client_init_error = cri_csvt_utils_init_client(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                    UTIL_LOG_MSG("service initialization, service id %d,  error %d",
                                 client_init_info->service_info[iter_client_init].cri_service_id,
                                 client_init_error);
                    break;
                */
                case QMI_CRI_NAS_SERVICE:
                    client_init_error = cri_nas_init_client(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                    UTIL_LOG_MSG("service initialization, service id %d,  error %d",
                                 client_init_info->service_info[iter_client_init].cri_service_id,
                                 client_init_error);
                    break;

                case QMI_CRI_VOICE_SERVICE:
                     client_init_error = cri_voice_core_init(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                     UTIL_LOG_MSG("service initialization, service id %d, error %d",
                                  client_init_info->service_info[iter_client_init].cri_service_id,
                                  client_init_error);
                     break;

                 case QMI_CRI_DMS_SERVICE:
                     client_init_error = cri_dms_init_client(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                     UTIL_LOG_MSG("service initialization, service id %d, error %d",
                                  client_init_info->service_info[iter_client_init].cri_service_id,
                                  client_init_error);
                     break;

                case QMI_CRI_WMS_SERVICE:
                    client_init_error = cri_wms_init_client(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                    UTIL_LOG_MSG("service initialization, service id %d,  error %d",
                                 client_init_info->service_info[iter_client_init].cri_service_id,
                                 client_init_error);
                    break;

                case QMI_CRI_DSD_SERVICE:
                    client_init_error = cri_data_init_client(
                                            client_init_info->service_info
                                            [iter_client_init].hlos_ind_cb
                                        );
                    UTIL_LOG_MSG("service initialization, service id %d,  error %d",
                                 client_init_info->service_info[iter_client_init].cri_service_id,
                                 client_init_error);
                    break;

                 case QMI_CRI_SIM_SERVICE:
                     client_init_error = cri_uim_init_client(
                                           client_init_info->service_info[iter_client_init].hlos_ind_cb);
                     UTIL_LOG_MSG("service initialization, service id %d, error %d",
                                  client_init_info->service_info[iter_client_init].cri_service_id,
                                  client_init_error);
                     break;


                default:
                    UTIL_LOG_MSG("service to be initialized unhandled, service id %d",
                                 client_init_info->service_info[iter_client_init].cri_service_id);
                    client_init_error = QMI_ERR_INTERNAL_V01;
                    break;
            }
        }

        if(services_num == iter_client_init)
        {
            ret_val = QMI_ERR_NONE_V01;
        }
        else
        {
            UTIL_LOG_MSG("all requested services have been initialized");
        }
    }

    return ret_val;
}

/***************************************************************************************************
    @function
    cri_core_cri_client_release

    @implementation detail
    Releases the initialized service based on service id.
***************************************************************************************************/
void cri_core_cri_client_release()
{
    int iter_client_release;

    iter_client_release = NIL;

    for(iter_client_release = 0; iter_client_release < CRI_CORE_MAX_CLIENTS; iter_client_release++)
    {
        if(TRUE == qmi_service_client_info[iter_client_release].is_valid)
        {
            switch(qmi_service_client_info[iter_client_release].service_id)
            {
                /*case QMI_CSVT_SERVICE:
                    cri_csvt_utils_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;
                */
                case QMI_CRI_NAS_SERVICE:
                    cri_nas_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                case QMI_CRI_DMS_SERVICE:
                    cri_dms_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                case QMI_CRI_WMS_SERVICE:
                    cri_wms_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                case QMI_CRI_DSD_SERVICE:
                    cri_data_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                case QMI_CRI_VOICE_SERVICE:
                    cri_voice_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                case QMI_CRI_SIM_SERVICE:
                    cri_uim_release_client(iter_client_release);
                    UTIL_LOG_MSG("service release, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;

                default:
                    UTIL_LOG_MSG("service to be released unhandled, service id %d",
                                 qmi_service_client_info[iter_client_release].service_id);
                    break;
            }
        }
    }

    //TODO: Need to call cri_core_release_qmi_client when we move to CRF completely
}

/***************************************************************************************************
    @function
    cri_core_create_qmi_client

    @implementation detail
    Makes CRI_CORE_MAX_TRIES_FOR_QMI_INIT attempts with a time period of
    CRI_CORE_DELAY_FOR_QMI_INIT second(s) to intialize the QMI framework.
***************************************************************************************************/
int cri_core_create_qmi_client(qmi_sys_event_rx_hdlr sys_event_rx_hdlr)
{
    int qmi_client_handle;
    int attempt_number;

    qmi_client_handle = QMI_INTERNAL_ERR;
    attempt_number = NIL;

    do
    {
      if (NIL != attempt_number)
      {
          sleep(CRI_CORE_DELAY_FOR_QMI_INIT);
      }
     // qmi_client_handle  = qmi_init(cri_core_qmi_client_sys_event_handler , NULL);
      qmi_client_handle = 1;
      attempt_number++;
    } while((QMI_NO_ERR > qmi_client_handle) &&
            (attempt_number < CRI_CORE_MAX_TRIES_FOR_QMI_INIT));

    if (QMI_NO_ERR != qmi_client_handle)
    {
        UTIL_LOG_MSG("qmi client initialization failure");
    }
    else
    {
        qmi_client_info.is_valid = TRUE;
        qmi_client_info.qmi_client_handle = qmi_client_handle;
        qmi_client_info.sys_event_rx_hdlr = sys_event_rx_hdlr;
        UTIL_LOG_MSG("qmi client initialization success");
    }

    return qmi_client_handle;
}

/***************************************************************************************************
    @function
    cri_core_release_qmi_client

    @implementation detail
    None.
***************************************************************************************************/
int cri_core_release_qmi_client(int qmi_client_handle)
{
    int ret_code;

    ret_code = QMI_INTERNAL_ERR;

    if(TRUE == qmi_client_info.is_valid &&
       qmi_client_info.qmi_client_handle == qmi_client_handle)
    {
        ret_code = QMI_NO_ERR;
        qmi_client_info.is_valid = FALSE;
        //qmi_release(qmi_client_info.qmi_client_handle);
        UTIL_LOG_MSG("qmi client release success");
    }
    else
    {
        UTIL_LOG_MSG("qmi client release failure");
    }

    return ret_code;
}

/***************************************************************************************************
    @function
    cri_core_create_qmi_service_client

    @implementation detail
    Initializes the required service based on service id.
***************************************************************************************************/
int cri_core_create_qmi_service_client(qmi_service_id_type service_id,
                                       hlos_ind_cb_type hlos_ind_cb)
{
    int iter_client;
    int is_allocation_success;
    int max_retry = 20;
    qmi_client_error_type client_allocation_err;
    char *qmi_port;

    iter_client = NIL;
    is_allocation_success = FALSE;
    client_allocation_err = QMI_NO_ERR;
    qmi_port = cri_core_get_qmi_port();
    qmi_client_os_params os_params;

    do
    {
        for(iter_client = 0; iter_client < CRI_CORE_MAX_CLIENTS; iter_client++)
        {
            if(FALSE == qmi_service_client_info[iter_client].is_valid)
            {
                memset(&qmi_service_client_info[iter_client],
                       NIL,
                       sizeof(qmi_service_client_info[iter_client]));
                is_allocation_success = TRUE;
                break;
            }
        }

        if(FALSE == is_allocation_success || NULL == hlos_ind_cb)
        {
            UTIL_LOG_MSG("qmi service client initialization failure, service id %d",
                         service_id);
            iter_client = -1;
            break;
        }

        qmi_service_client_info[iter_client].client_state = CRI_CORE_QMI_SERVICE_NOT_CONNECTED;
        qmi_service_client_info[iter_client].hlos_ind_cb = hlos_ind_cb;
        switch(service_id)
        {
            /*case QMI_CSVT_SERVICE:
              qmi_service_client_info[iter_client].service_obj = csvt_get_service_object_v01();
              break;
            */
            case QMI_CRI_NAS_SERVICE:
              qmi_service_client_info[iter_client].service_obj = nas_get_service_object_v01();
              break;

            case QMI_CRI_DMS_SERVICE:
            qmi_service_client_info[iter_client].service_obj = dms_get_service_object_v01();
            break;

            case QMI_CRI_VOICE_SERVICE:
            qmi_service_client_info[iter_client].service_obj = voice_get_service_object_v02();
            break;

            case QMI_CRI_WMS_SERVICE:
              qmi_service_client_info[iter_client].service_obj = wms_get_service_object_v01();
              break;

            case QMI_CRI_DSD_SERVICE:
              qmi_service_client_info[iter_client].service_obj = dsd_get_service_object_v01();
              break;

            case QMI_CRI_SIM_SERVICE:
              qmi_service_client_info[iter_client].service_obj = uim_get_service_object_v01();
              break;

            default:
                is_allocation_success = FALSE;
                break;
        }

        if(FALSE == is_allocation_success)
        {
            iter_client = -1;
            break;
        }
        qmi_service_client_info[iter_client].service_id = service_id;

        do
        {
            UTIL_LOG_MSG("Trying to connect, service id %d,cri core client id %d", service_id, iter_client);
            client_allocation_err = qmi_client_init_instance(qmi_service_client_info[iter_client].service_obj,
                                                             QMI_CLIENT_INSTANCE_ANY,
                                                             cri_core_qmi_service_unsol_ind_cb,
                                                             NULL,
                                                             &os_params,
                                                             CRI_QMI_CLIENT_SERVICE_REGISTRATION_TIMEOUT,
                                                             &qmi_service_client_info[iter_client].user_handle );

            if(client_allocation_err == QMI_NO_ERR)
            {
                //SUCCESS
                break;
            }
            max_retry--;
            UTIL_LOG_MSG("qmi service client initialization failure, service id %d, retries left: %d",
                               service_id, max_retry);
            usleep(1000*200); // Sleep for 200 milliseconds
        } while(max_retry);

        if(max_retry == 0)
        {
            //Failure after all retries. Reset
            iter_client = -1;
            UTIL_LOG_MSG("qmi service client initialization failure after multiple retries, service id %d, retries left: %d", service_id, max_retry);
        }
        else
        {
            UTIL_LOG_MSG("qmi service client initialization success, service %d,cri core client id %d, qmi userhandle %p",
                         service_id,
                         iter_client,
                         qmi_service_client_info[iter_client].user_handle);
            qmi_service_client_info[iter_client].client_state = CRI_CORE_QMI_SERVICE_CONNECTED;
            qmi_service_client_info[iter_client].is_valid = TRUE;
        }
    }while(0);

    return iter_client;
}

/***************************************************************************************************
    @function
    cri_core_release_qmi_service_client

    @implementation detail
    Invalidates the service client specific cache before releasing the client.
***************************************************************************************************/
int cri_core_release_qmi_service_client(int qmi_service_client_id)
{
    int ret_code;

    ret_code = QMI_INTERNAL_ERR;

    if(qmi_service_client_id >= NIL &&
       qmi_service_client_id < CRI_CORE_MAX_CLIENTS &&
       TRUE == qmi_service_client_info[qmi_service_client_id].is_valid)
    {
        //TODO specific cleanup for cache
        ret_code = QMI_NO_ERR;
        qmi_service_client_info[qmi_service_client_id].is_valid = FALSE;
        qmi_client_release(qmi_service_client_info[qmi_service_client_id].user_handle);
    }
    else
    {
        UTIL_LOG_MSG("qmi service client release failure, cri core client id %d",
                     qmi_service_client_id);
    }

    return ret_code;
}

/***************************************************************************************************
    @function
    cri_core_qmi_service_unsol_ind_cb

    @implementation detail
    Allocates memory for and copies the indication data since the indication data is freed
    by QMI framework once the QMI framework thread returns.
***************************************************************************************************/
void cri_core_qmi_service_unsol_ind_cb(qmi_client_type user_handle,
                                       unsigned long message_id,
                                       unsigned char *ind_buf,
                                       int ind_buf_len,
                                       void *ind_cb_data)
{
    cri_core_cri_message_data_type *cri_core_cri_message_data;

    cri_core_cri_message_data = NULL;

    UTIL_LOG_MSG("qmi service indication received, message id %d, qmi userhandle %p",
                 message_id,
                 user_handle);
    cri_core_cri_message_data = util_memory_alloc(sizeof(*cri_core_cri_message_data));
    if(cri_core_cri_message_data)
    {
        cri_core_cri_message_data->cri_message_category = IND;
        cri_core_cri_message_data->user_handle = user_handle;
        cri_core_cri_message_data->event_id = message_id;
        cri_core_cri_message_data->data = util_memory_alloc(ind_buf_len);
        if(cri_core_cri_message_data->data)
        {
            cri_core_cri_message_data->data_len = ind_buf_len;
            memcpy(cri_core_cri_message_data->data,
                   ind_buf,
                   cri_core_cri_message_data->data_len);
        }
        cri_core_cri_message_data->cb_data = ind_cb_data;
        core_handler_add_event(CORE_HANDLER_CRI_MESSAGE,
                               cri_core_cri_message_data);
    }
}

/***************************************************************************************************
    @function
    cri_core_qmi_service_async_resp_cb

    @implementation detail
    None.
***************************************************************************************************/
void cri_core_qmi_service_async_resp_cb(qmi_client_type user_handle,
                                        unsigned long message_id,
                                        void *resp_message,
                                        int resp_message_len,
                                        void *resp_cb_data,
                                        qmi_client_error_type transport_error)
{

    UTIL_LOG_MSG("In cri_core_qmi_service_async_resp_cb: resp_data pointer:%x | length: %d | message_id: %x\n",
                  resp_message, resp_message_len, message_id);

    cri_core_cri_message_data_type *cri_core_cri_message_data;

    cri_core_cri_message_data = NULL;

    UTIL_LOG_MSG("qmi service async resp received, message id %d, qmi userhandle %p",
                 message_id,
                 user_handle);
    cri_core_cri_message_data = util_memory_alloc(sizeof(*cri_core_cri_message_data));
    if(cri_core_cri_message_data)
    {
        cri_core_cri_message_data->cri_message_category = RESP;
        cri_core_cri_message_data->user_handle = user_handle;
        cri_core_cri_message_data->event_id = message_id;
        cri_core_cri_message_data->data = resp_message;
        cri_core_cri_message_data->data_len = resp_message_len;
        cri_core_cri_message_data->cb_data = resp_cb_data;
        cri_core_cri_message_data->transport_error = transport_error;
        core_handler_add_event(CORE_HANDLER_CRI_MESSAGE,
                               cri_core_cri_message_data);
    }
}

/***************************************************************************************************
    @function
    cri_core_unsol_ind_handler

    @implementation detail
    Runs in the context of the core thread.
***************************************************************************************************/
void cri_core_unsol_ind_handler(void *event_data)
{
    int iter_client;
    qmi_client_error_type err_code;
    cri_core_cri_message_data_type *core_handler_qmi_service_unsol_ind_data;
    uint32_t decoded_payload_len;
    void* decoded_payload;

    iter_client = NIL;
    err_code = QMI_NO_ERR;
    core_handler_qmi_service_unsol_ind_data = (cri_core_cri_message_data_type*) event_data;
    decoded_payload_len = NIL;
    decoded_payload = NULL;

    UTIL_LOG_MSG("entry");

    if(core_handler_qmi_service_unsol_ind_data)
    {
        for(iter_client = 0; iter_client < CRI_CORE_MAX_CLIENTS; iter_client++)
        {
            if(TRUE == qmi_service_client_info[iter_client].is_valid &&
               (core_handler_qmi_service_unsol_ind_data->user_handle ==
               qmi_service_client_info[iter_client].user_handle))
            {
                UTIL_LOG_MSG("indication, msg id %d, service id %d being processed",
                             core_handler_qmi_service_unsol_ind_data->event_id,
                             qmi_service_client_info[iter_client].service_id);
                qmi_idl_get_message_c_struct_len(qmi_service_client_info[iter_client].service_obj,
                                                 QMI_IDL_INDICATION,
                                                 core_handler_qmi_service_unsol_ind_data->event_id,
                                                 &decoded_payload_len);

                if(decoded_payload_len)
                {
                    decoded_payload = util_memory_alloc(decoded_payload_len);
                }

                if ( decoded_payload || !decoded_payload_len )
                {
                  if( decoded_payload_len )
                  {
                    err_code = qmi_client_message_decode(
                                   qmi_service_client_info[iter_client].user_handle,
                                   QMI_IDL_INDICATION,
                                   core_handler_qmi_service_unsol_ind_data->event_id,
                                   core_handler_qmi_service_unsol_ind_data->data,
                                   core_handler_qmi_service_unsol_ind_data->data_len,
                                   decoded_payload,
                                   (int)decoded_payload_len
                               );
                  }
                  UTIL_LOG("unsol indication event_id - %x\n",core_handler_qmi_service_unsol_ind_data->event_id);
                  if (QMI_NO_ERR == err_code)
                  {
                      //TODO: pass SUB ID to ind handler since we do not have context

                      switch(qmi_service_client_info[iter_client].service_id)
                      {
                            /*case QMI_CSVT_SERVICE:
                                cri_csvt_core_unsol_ind_handler(
                                    iter_client,
                                    core_handler_qmi_service_unsol_ind_data->event_id,
                                    decoded_payload,
                                    decoded_payload_len
                                    );
                            break;
                            */

                            case QMI_CRI_NAS_SERVICE:
                                cri_nas_unsol_ind_handler(
                                    iter_client,
                                    core_handler_qmi_service_unsol_ind_data->event_id,
                                    decoded_payload,
                                    decoded_payload_len
                                    );
                                break;
                            case QMI_CRI_VOICE_SERVICE:
                                cri_voice_core_unsol_ind_handler(iter_client,
                                    core_handler_qmi_service_unsol_ind_data->event_id,
                                    decoded_payload,
                                    decoded_payload_len
                                    );
                            break;

                            case QMI_CRI_DMS_SERVICE:
                                cri_dms_unsol_ind_handler(iter_client,
                                    core_handler_qmi_service_unsol_ind_data->event_id,
                                    decoded_payload,
                                    decoded_payload_len
                                    );
                            break;

                          case QMI_CRI_WMS_SERVICE:
                            cri_wms_unsol_ind_handler(
                                iter_client,
                                core_handler_qmi_service_unsol_ind_data->event_id,
                                decoded_payload,
                                decoded_payload_len
                            );
                            break;

                          case QMI_CRI_DSD_SERVICE:
                            cri_data_unsol_ind_handler(
                                iter_client,
                                core_handler_qmi_service_unsol_ind_data->event_id,
                                decoded_payload,
                                decoded_payload_len
                            );
                            break;

                            case QMI_CRI_SIM_SERVICE:
                                cri_uim_core_unsol_ind_handler(iter_client,
                                    core_handler_qmi_service_unsol_ind_data->event_id,
                                    decoded_payload,
                                    decoded_payload_len
                                    );
                            break;

                          default:
                              UTIL_LOG_MSG("service id %d indications unhandled",
                                           qmi_service_client_info[iter_client].service_id);
                              break;
                      }

                  }
                  else
                  {
                      UTIL_LOG_MSG("indication decoding failed, error %d",
                                   err_code);
                  }

                  if(decoded_payload)
                  {
                      util_memory_free((void**) &decoded_payload);
                  }
                }
                break;
            }
        }

        if(core_handler_qmi_service_unsol_ind_data->data)
        {
            util_memory_free((void**) &core_handler_qmi_service_unsol_ind_data->data);
        }

        core_handler_remove_event(core_handler_qmi_service_unsol_ind_data);
        util_memory_free((void**) &core_handler_qmi_service_unsol_ind_data);
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    cri_core_async_resp_handler

    @implementation detail
    Runs in the context of the core thread.
    Frees up QMI response message data.
***************************************************************************************************/
void cri_core_async_resp_handler(void *event_data)
{
    int iter_client;
    cri_core_cri_message_data_type *core_handler_qmi_service_async_resp_data;
    cri_core_context_type cri_core_context;

    UTIL_LOG_MSG("entry");

    iter_client = NIL;
    core_handler_qmi_service_async_resp_data = (cri_core_cri_message_data_type*) event_data;
    cri_core_context = NIL;

    if(core_handler_qmi_service_async_resp_data)
    {
        if(core_handler_qmi_service_async_resp_data->cb_data)
        {
            cri_core_context = *((cri_core_context_type*)
                                 core_handler_qmi_service_async_resp_data->cb_data);
        }

        for(iter_client = 0; iter_client < CRI_CORE_MAX_CLIENTS; iter_client++)
        {
            if(TRUE == qmi_service_client_info[iter_client].is_valid &&
               (core_handler_qmi_service_async_resp_data->user_handle ==
                qmi_service_client_info[iter_client].user_handle))
            {
                UTIL_LOG_MSG("async resp, %s, msg id %d, service id %d being processed",
                             cri_core_create_loggable_context(cri_core_context),
                             core_handler_qmi_service_async_resp_data->event_id,
                             qmi_service_client_info[iter_client].service_id);

                /*
                TODO:
                Backend Call service specific async resp handler with
                core_handler_qmi_service_async_resp_data's data and so on
                Call service specific async resp handler should
                1) Process the response, retrieve error code,
                    convert data to hlos format data (if needed)
                2) Call cri_rule_handler_rule_check with processed data(if any)
                */

                switch(qmi_service_client_info[iter_client].service_id)
                {
                    /*case QMI_CSVT_SERVICE:
                        cri_csvt_core_async_resp_handler(
                            iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                      break;
                     */
                    case QMI_CRI_NAS_SERVICE:
                        cri_nas_async_resp_handler(
                            iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                      break;

                    case QMI_CRI_VOICE_SERVICE:
                        cri_voice_core_async_resp_handler(
                            iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                       break;

                    case QMI_CRI_DMS_SERVICE:
                        cri_dms_async_resp_handler(iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                    break;

                    case QMI_CRI_WMS_SERVICE:
                      cri_wms_async_resp_handler(
                          iter_client,
                          core_handler_qmi_service_async_resp_data->event_id,
                          core_handler_qmi_service_async_resp_data->data,
                          core_handler_qmi_service_async_resp_data->data_len,
                          cri_core_context
                      );
                      break;

                    case QMI_CRI_DSD_SERVICE:
                        cri_data_async_resp_handler(
                            iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                      break;

                    case QMI_CRI_SIM_SERVICE:
                        cri_uim_core_async_resp_handler(iter_client,
                            core_handler_qmi_service_async_resp_data->event_id,
                            core_handler_qmi_service_async_resp_data->data,
                            core_handler_qmi_service_async_resp_data->data_len,
                            cri_core_context
                        );
                    break;

                    default:
                        UTIL_LOG_MSG("service id %d async responses unhandled",
                                     qmi_service_client_info[iter_client].service_id);
                        break;
                }
                break;
            }
        }

        if(core_handler_qmi_service_async_resp_data->data &&
           core_handler_qmi_service_async_resp_data->data_len)
        {
            util_memory_free((void**) &core_handler_qmi_service_async_resp_data->data);
        }

        if(core_handler_qmi_service_async_resp_data->cb_data)
        {
            util_memory_free((void**) &core_handler_qmi_service_async_resp_data->cb_data);
        }

        core_handler_remove_event(core_handler_qmi_service_async_resp_data);
        util_memory_free((void**) &core_handler_qmi_service_async_resp_data);
    }

    UTIL_LOG_MSG("exit");
}

/***************************************************************************************************
    @function
    cri_core_qmi_send_msg_sync

    @implementation detail
    None.
***************************************************************************************************/
qmi_error_type_v01 cri_core_qmi_send_msg_sync(int qmi_service_client_id,
                                              unsigned long message_id,
                                              void *req_message,
                                              int req_message_len,
                                              void *resp_message,
                                              int resp_message_len,
                                              int timeout_secs)
{
    qmi_client_error_type transport_error;
    qmi_error_type_v01 err_code;

    UTIL_LOG_MSG("entry");

    transport_error = QMI_INTERNAL_ERR;
    err_code = QMI_ERR_INTERNAL_V01;

    if(qmi_service_client_id >= NIL && qmi_service_client_id < CRI_CORE_MAX_CLIENTS &&
       TRUE == qmi_service_client_info[qmi_service_client_id].is_valid)
    {
        transport_error = qmi_client_send_msg_sync(
                              qmi_service_client_info[qmi_service_client_id].user_handle,
                              message_id,
                              req_message,
                              req_message_len,
                              resp_message,
                              resp_message_len,
                              timeout_secs * 1000
                          );

        UTIL_LOG_MSG("sync req, msg id %d, service id %d, transp error %d",
                     message_id,
                     qmi_service_client_info[qmi_service_client_id].service_id,
                     transport_error);

        if(QMI_NO_ERR == transport_error)
        {
            err_code = QMI_ERR_NONE_V01;
        }

        /*
        TODO:
        This function needs to be called from a CRI service handler (Voice, NAS....)
        CRI service handler should process the response, retrieve error code,
        convert data to hlos format data (if needed)
        */
    }

    UTIL_LOG_MSG("exit");
    return err_code;
}

/***************************************************************************************************
    @function
    cri_core_qmi_send_msg_async

    @implementation detail
    None.
***************************************************************************************************/
qmi_error_type_v01 cri_core_qmi_send_msg_async(cri_core_context_type cri_core_context,
                                               int qmi_service_client_id,
                                               unsigned long message_id,
                                               void *req_message,
                                               int req_message_len,
                                               int resp_message_len,
                                               void *hlos_cb_data,
                                               hlos_resp_cb_type hlos_resp_cb,
                                               int timeout_secs,
                                               cri_rule_handler_user_rule_info_type *user_rule_info
                                               )
{
    cri_core_token_id_type cri_core_token_id;
    cri_core_context_type temp_cri_core_context;
    qmi_client_error_type transport_error;
    qmi_txn_handle txn_handle;
    struct timeval qmi_async_req_timeval;
    cri_rule_handler_rule_info_type cri_rule_handler_rule_info;
    qmi_error_type_v01 err_code;
    void *resp_message;
    cri_core_context_type *context_cb_data;

    UTIL_LOG_MSG("entry");

    cri_core_token_id = NIL;
    temp_cri_core_context = NIL;
    transport_error = QMI_INTERNAL_ERR;
    err_code = QMI_ERR_INTERNAL_V01;
    txn_handle = NIL;
    memset(&qmi_async_req_timeval,
           NIL,
           sizeof(qmi_async_req_timeval));
    memset(&cri_rule_handler_rule_info,
           NIL,
           sizeof(cri_rule_handler_rule_info));
    resp_message = NULL;
    context_cb_data = NULL;

    qmi_async_req_timeval.tv_sec = timeout_secs;

    if(qmi_service_client_id >= NIL && qmi_service_client_id < CRI_CORE_MAX_CLIENTS &&
       TRUE == qmi_service_client_info[qmi_service_client_id].is_valid)
    {
        if(resp_message_len)
        {
            resp_message = util_memory_alloc(resp_message_len);
        }

        if(NIL == resp_message_len || NULL != resp_message)
        {
            context_cb_data = util_memory_alloc(sizeof(cri_core_context_type));
            if(NULL != context_cb_data)
            {
                cri_core_token_id = cri_core_generate_cri_token_id();
                temp_cri_core_context = cri_core_generate_context_using_cri_token_id(
                                            cri_core_context,
                                            cri_core_token_id
                                        );

                *context_cb_data = temp_cri_core_context;
                cri_rule_handler_rule_info.core_rule_info.context = temp_cri_core_context;
                cri_rule_handler_rule_info.core_rule_info.qmi_resp_data = resp_message;
                cri_rule_handler_rule_info.core_rule_info.qmi_cb_data = context_cb_data;
                cri_rule_handler_rule_info.core_rule_info.hlos_resp_cb = hlos_resp_cb;
                cri_rule_handler_rule_info.core_rule_info.hlos_resp_cb_data = hlos_cb_data;
                if(user_rule_info)
                {
                    memcpy(&cri_rule_handler_rule_info.user_rule_info,
                           user_rule_info,
                           sizeof(cri_rule_handler_rule_info.user_rule_info));
                }


                transport_error = qmi_client_send_msg_async(
                                      qmi_service_client_info[qmi_service_client_id].user_handle,
                                      message_id,
                                      req_message,
                                      req_message_len,
                                      resp_message,
                                      resp_message_len,
                                      cri_core_qmi_service_async_resp_cb,
                                      (void*) context_cb_data,
                                      &txn_handle
                                  );

                if(QMI_NO_ERR == transport_error)
                {
                    if(ESUCCESS == cri_rule_handler_rule_add(&cri_rule_handler_rule_info,
                                                             &qmi_async_req_timeval))
                    {
                        err_code = QMI_ERR_NONE_V01;
                    }
                }
            }

            if(QMI_ERR_NONE_V01 != err_code)
            {
                if(resp_message)
                {
                    util_memory_free((void**) &resp_message);
                }

                if(context_cb_data)
                {
                    util_memory_free((void**) &context_cb_data);
                }
            }
            UTIL_LOG_MSG("async req, msg id %d, service id %d, %s, error %d",
                         message_id,
                         qmi_service_client_info[qmi_service_client_id].service_id,
                         cri_core_create_loggable_context(temp_cri_core_context),
                         err_code);
        }
    }

    UTIL_LOG_MSG("exit");
    return err_code;
}

/***************************************************************************************************
    @function
    cri_core_retrieve_hlos_ind_cb

    @implementation detail
    None.
***************************************************************************************************/
hlos_ind_cb_type cri_core_retrieve_hlos_ind_cb(int qmi_service_client_id)
{
    hlos_ind_cb_type hlos_ind_cb;

    UTIL_LOG_MSG("entry");

    hlos_ind_cb = NULL;

    if(qmi_service_client_id >= NIL && qmi_service_client_id < CRI_CORE_MAX_CLIENTS &&
       TRUE == qmi_service_client_info[qmi_service_client_id].is_valid)
    {
        hlos_ind_cb = qmi_service_client_info[qmi_service_client_id].hlos_ind_cb;
    }

    UTIL_LOG_MSG("exit");

    return hlos_ind_cb;
}

/***************************************************************************************************
    @function
    cri_core_generate_cri_token_id

    @implementation detail
    Resets the CRI token id to CRI_CORE_MIN_CRI_TOKEN_ID once it reaches CRI_CORE_MAX_CRI_TOKEN_ID.
***************************************************************************************************/
cri_core_token_id_type cri_core_generate_cri_token_id()
{
    cri_core_token_id++;

    if(CRI_CORE_MAX_CRI_TOKEN_ID == cri_core_token_id)
    {
        cri_core_token_id = CRI_CORE_MIN_CRI_TOKEN_ID;
    }

    return cri_core_token_id;
}

/***************************************************************************************************
    @function
    cri_core_generate_context_using_subscription_id__hlos_token_id

    @implementation detail
    Context is 64 bits wide.
    HLOS token id uses the most significant 32 bits.
    Subscription id uses bit 32 to bit 29.
***************************************************************************************************/
cri_core_context_type cri_core_generate_context_using_subscription_id__hlos_token_id(
                            cri_core_subscription_id_type subscription_id,
                            cri_core_hlos_token_id_type hlos_token_id)
{
    cri_core_context_type cri_core_context;

    memset(&cri_core_context, NIL, sizeof(cri_core_context));
    cri_core_context = hlos_token_id;
    cri_core_context <<= CRI_CORE_MAX_CRI_SUBSCRIPTION_ID_BITS;
    cri_core_context |= (subscription_id & CRI_CORE_MAX_CRI_SUBSCRIPTION_ID);
    cri_core_context <<= (CRI_CORE_CRI_SUBSCRIPTION_ID_POS - 1);

    return cri_core_context;
}

/***************************************************************************************************
    @function
    cri_core_generate_context_using_cri_token_id

    @implementation detail
    Context is 64 bits wide.
    HLOS token id uses the most significant 32 bits.
    Subscription id uses bit 32 to bit 29.
    CRI token id uses the least significant 16 bits.
***************************************************************************************************/
cri_core_context_type cri_core_generate_context_using_cri_token_id(
                            cri_core_context_type cri_core_context,
                            cri_core_token_id_type cri_core_token_id)
{
    return (cri_core_context | (cri_core_token_id & CRI_CORE_MAX_CRI_TOKEN_ID));
}

/***************************************************************************************************
    @function
    cri_core_retrieve_cri_token_id_from_context

    @implementation detail
    None.
***************************************************************************************************/
void cri_core_retrieve_cri_token_id_from_context(cri_core_context_type cri_core_context,
                                                 cri_core_token_id_type *cri_core_token_id)
{
    if(cri_core_token_id)
    {
        *cri_core_token_id = (cri_core_context & CRI_CORE_MAX_CRI_TOKEN_ID);
    }
}

/***************************************************************************************************
    @function
    cri_core_retrieve_subscription_id__hlos_token_id_from_context

    @implementation detail
    None.
***************************************************************************************************/
void cri_core_retrieve_subscription_id__hlos_token_id_from_context(
           cri_core_context_type cri_core_context,
           cri_core_subscription_id_type *subscription_id,
           cri_core_hlos_token_id_type *hlos_token_id)
{
    if(subscription_id && hlos_token_id)
    {
        *subscription_id = ((cri_core_context >> (CRI_CORE_CRI_SUBSCRIPTION_ID_POS - 1)) &
                            CRI_CORE_MAX_CRI_SUBSCRIPTION_ID);
        *hlos_token_id = ((cri_core_context >> (CRI_CORE_CRI_HLOS_TOKEN_ID_POS - 1)) &
                          CRI_CORE_MAX_HLOS_TOKEN_ID);
    }
}

/***************************************************************************************************
    @function
    cri_core_create_loggable_context

    @implementation detail
    Uses a global buffer to store the loggable context. Hence it is NOT thread safe and has to be
    called only in the context of core thread.
***************************************************************************************************/
char* cri_core_create_loggable_context(cri_core_context_type cri_core_context)
{
    cri_core_subscription_id_type subscription_id;
    cri_core_hlos_token_id_type hlos_token_id;
    cri_core_token_id_type cri_core_token_id;

    subscription_id= NIL;
    hlos_token_id = NIL;
    cri_core_token_id = NIL;

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(cri_core_context,
                                                                  &subscription_id,
                                                                  &hlos_token_id);

    cri_core_retrieve_cri_token_id_from_context(cri_core_context,
                                                &cri_core_token_id);

    memset(cri_core_log_context_buffer,
           NIL,
           sizeof(cri_core_log_context_buffer));

    snprintf(cri_core_log_context_buffer,
             sizeof(cri_core_log_context_buffer),
             "context 0x%016llx (hlos token id %u, sub id %u, cri token id %u)",
             cri_core_context, hlos_token_id, subscription_id, cri_core_token_id);

    return cri_core_log_context_buffer;
}

/***************************************************************************************************
    @function
    cri_core_retrieve_err_code

    @implementation detail
    None.
***************************************************************************************************/
qmi_error_type_v01 cri_core_retrieve_err_code(qmi_error_type_v01 transport_error,
                                              qmi_response_type_v01* resp_err)
{
    qmi_error_type_v01 err_code;

    err_code = QMI_ERR_NONE_V01;

    if(resp_err)
    {
        switch(transport_error)
        {
            case QMI_ERR_NONE_V01:
                switch (resp_err->result)
                {
                    case QMI_RESULT_SUCCESS_V01:
                        err_code = QMI_ERR_NONE_V01;
                        break;

                    case QMI_RESULT_FAILURE_V01:
                        switch (resp_err->error)
                        {
                            case QMI_ERR_NONE_V01:
                            case QMI_ERR_NO_EFFECT_V01:
                                err_code = QMI_ERR_NONE_V01;
                                break;

                            case QMI_ERR_MALFORMED_MSG_V01:
                                err_code = CRI_ERR_MALFORMED_MSG_V01;
                                break;

                            case QMI_ERR_NO_MEMORY_V01:
                                err_code = CRI_ERR_NO_MEMORY_V01;
                                break;

                            case QMI_ERR_CLIENT_IDS_EXHAUSTED_V01:
                                err_code = CRI_ERR_CLIENT_IDS_EXHAUSTED_V01;
                                break;

                            case QMI_ERR_UNABORTABLE_TRANSACTION_V01:
                                err_code = CRI_ERR_UNABORTABLE_TRANSACTION_V01;
                                break;

                            case QMI_ERR_INVALID_CLIENT_ID_V01:
                                err_code = CRI_ERR_INVALID_CLIENT_ID_V01;
                                break;

                            case QMI_ERR_INVALID_HANDLE_V01:
                                err_code = CRI_ERR_INVALID_HANDLE_V01;
                                break;

                            case QMI_ERR_INVALID_PROFILE_V01:
                                err_code = CRI_ERR_INVALID_PROFILE_V01;
                                break;

                            case QMI_ERR_NO_NETWORK_FOUND_V01:
                                err_code = CRI_ERR_NO_NETWORK_FOUND_V01;
                                break;

                            case QMI_ERR_OUT_OF_CALL_V01:
                                err_code = CRI_ERR_OUT_OF_CALL_V01;
                                break;

                            case QMI_ERR_NOT_PROVISIONED_V01:
                                err_code = CRI_ERR_NOT_PROVISIONED_V01;
                                break;

                            case QMI_ERR_MISSING_ARG_V01:
                                err_code = CRI_ERR_MISSING_ARG_V01;
                                break;

                            case QMI_ERR_ARG_TOO_LONG_V01:
                                err_code = CRI_ERR_ARG_TOO_LONG_V01;
                                break;

                            case QMI_ERR_INVALID_TX_ID_V01:
                                err_code = CRI_ERR_INVALID_TX_ID_V01;
                                break;

                            case QMI_ERR_DEVICE_IN_USE_V01:
                                err_code = CRI_ERR_DEVICE_IN_USE_V01;
                                break;

                            case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
                                err_code = CRI_ERR_OP_DEVICE_UNSUPPORTED_V01;
                                break;

                            case QMI_ERR_NO_FREE_PROFILE_V01:
                                err_code = CRI_ERR_NO_FREE_PROFILE_V01;
                                break;

                            case QMI_ERR_INVALID_PDP_TYPE_V01:
                                err_code = CRI_ERR_INVALID_PDP_TYPE_V01;
                                break;

                            case QMI_ERR_INVALID_TECH_PREF_V01:
                                err_code = CRI_ERR_INVALID_TECH_PREF_V01;
                                break;

                            case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
                                err_code = CRI_ERR_OP_NETWORK_UNSUPPORTED_V01;
                                break;

                            case QMI_ERR_ABORTED_V01:
                                err_code = CRI_ERR_ABORTED_V01;
                                break;

                            case QMI_ERR_FDN_RESTRICT_V01:
                                err_code = CRI_ERR_FDN_RESTRICT_V01;
                                break;

                            case QMI_ERR_INTERNAL_V01:
                                err_code = QMI_ERR_INTERNAL_V01;
                                break;

                            default:
                                err_code = QMI_ERR_INTERNAL_V01;
                                break;
                        }
                        break;

                    default:
                        err_code = QMI_ERR_INTERNAL_V01;
                        break;
                }
                break;

            default:
                err_code = QMI_ERR_INTERNAL_V01;
                break;
        }
    }
    else
    {
        err_code = QMI_ERR_INTERNAL_V01;
    }

    return err_code;
}

void cri_core_register_for_service_down(ssr_srv_down_cb cb)
{
    int iter_client;
    qmi_client_error_type client_err = QMI_NO_ERR;

    do
    {
        if(NULL == cb)
        {
            break;
        }

        srv_down_cb = cb;
        client_info.num_of_active_clients = 0;
        for ( iter_client = 0; iter_client < CRI_CORE_MAX_CLIENTS; iter_client ++ )
        {
            //Do not register down events for services which are not included for SSR.
            if ( (qmi_service_client_info[iter_client].client_state == CRI_CORE_QMI_SERVICE_CONNECTED) &&
                 (qmi_service_client_info[iter_client].is_valid))
            {
                client_err = qmi_client_register_error_cb(qmi_service_client_info[iter_client].user_handle,
                                                          cri_core_service_down_event,
                                                          (void*)(intptr_t)iter_client);

                UTIL_LOG_MSG("error call back registration res - %d index - %d", client_err, iter_client);
                if( client_err == QMI_NO_ERR )
                {
                    client_info.num_of_active_clients++;
                }
            }
        }

        client_info.max_active_clients       = client_info.num_of_active_clients;
        UTIL_LOG_MSG("Max active clients - %d", client_info.max_active_clients);
    }while(FALSE);
}

void cri_core_service_down_event
(
  qmi_client_type       clnt,
  qmi_client_error_type error,
  void                 *cb_data
)
{
    int srv_index;
    cri_core_service_down_event_data_type *cri_core_service_down_event_data = NULL;

    cri_core_service_down_event_data = util_memory_alloc(sizeof(*cri_core_service_down_event_data));

    if(cri_core_service_down_event_data)
    {
        srv_index = (intptr_t)cb_data;
        UTIL_LOG_MSG("cri_core_service_down_event %d", (int) srv_index);
        cri_core_service_down_event_data->event_id = SRV_DOWN;
        cri_core_service_down_event_data->clnt = clnt;
        cri_core_service_down_event_data->error = error;
        cri_core_service_down_event_data->cb_data = cb_data;
        core_handler_add_event(CORE_HANDLER_SERVICE_DOWN,
                               cri_core_service_down_event_data);
    }
}

void cri_core_service_down_event_hdlr(void *event_data)
{
    cri_core_service_down_event_data_type *cri_core_service_down_event_data = NULL;
    int srv_index;

    do
    {
        if(NULL == event_data)
        {
            break;
        }

        cri_core_service_down_event_data = (cri_core_service_down_event_data_type *)event_data;
        srv_index = (intptr_t) cri_core_service_down_event_data->cb_data;

        UTIL_LOG_MSG("cri_core_service_down_event_hdlr %d", (int) srv_index);

        if (client_info.num_of_active_clients)
        {
            if (qmi_service_client_info[srv_index].client_state == CRI_CORE_QMI_SERVICE_CONNECTED)
            {
                qmi_service_client_info[srv_index].client_state = CRI_CORE_QMI_SERVICE_NOT_AVAILABLE;
                client_info.num_of_active_clients--;
            }

            UTIL_LOG_MSG("number of active clients %d", client_info.num_of_active_clients);
            if (!client_info.num_of_active_clients)
            {
                cri_core_cri_client_release();
                UTIL_LOG_MSG( "SUSPENDED" );
                cri_core_set_operational_status( CRI_CORE_GEN_OPERATIONAL_STATUS_SUSPENDED );
                cri_dms_utils_clear_operating_mode();
                if(srv_down_cb)
                {
                    srv_down_cb();
                }
            }
        }
    }while(FALSE);
}

void cri_core_register_for_service_up(ssr_srv_up_cb cb)
{
    int iter_client;
    int rc = 0;

    do
    {
        if(NULL == cb)
        {
            break;
        }

        srv_up_cb = cb;
        for (iter_client = 0; iter_client < CRI_CORE_MAX_CLIENTS; iter_client++)
        {
            //Do not register up events for services which are not included for SSR.
            if (qmi_service_client_info[iter_client].client_state == CRI_CORE_QMI_SERVICE_NOT_AVAILABLE)
            {
                if (!qmi_service_client_info[iter_client].notifier)
                {
                    rc = qmi_client_notifier_init(qmi_service_client_info[iter_client].service_obj,
                                              NIL,
                                              &qmi_service_client_info[iter_client].notifier);
                    UTIL_LOG_MSG("qmi_client_notifier_init return %d", (int) rc);
                }

                if (!rc)
                {
                    rc = qmi_client_register_notify_cb(qmi_service_client_info[iter_client].notifier,
                                              cri_core_service_up_event,
                                              (void*)(intptr_t)iter_client);
                    UTIL_LOG_MSG("qmi_client_register_notify_cb %d", (int) rc);
                }
            }
        }
    }while(FALSE);
}

void cri_core_service_up_event
(
  qmi_client_type  clnt,
  qmi_idl_service_object_type   svc_obj,
  qmi_client_notify_event_type  service_event,
  void                         *cb_data
)
{
    int srv_index;
    cri_core_service_up_event_data_type *cri_core_service_up_event_data = NULL;

    cri_core_service_up_event_data = util_memory_alloc(sizeof(*cri_core_service_up_event_data));

    if(cri_core_service_up_event_data)
    {
        srv_index = (intptr_t)cb_data;
        UTIL_LOG_MSG("cri_core_service_up_event %d", (int) srv_index);
        cri_core_service_up_event_data->event_id = SRV_UP;
        cri_core_service_up_event_data->clnt = clnt;
        cri_core_service_up_event_data->svc_obj = svc_obj;
        cri_core_service_up_event_data->service_event = service_event;
        cri_core_service_up_event_data->cb_data = cb_data;
        core_handler_add_event(CORE_HANDLER_SERVICE_UP,
                               cri_core_service_up_event_data);
    }
}

void cri_core_service_up_event_hdlr(void *event_data)
{
    cri_core_service_up_event_data_type *cri_core_service_up_event_data = NULL;
    int rc;
    qmi_service_info info;

    do
    {
        if(NULL == event_data)
        {
            break;
        }

        cri_core_service_up_event_data = (cri_core_service_up_event_data_type *)event_data;
        UTIL_LOG_MSG("cri_core_service_up_event_hdlr %"PRIdPTR" %d",
                             (intptr_t) cri_core_service_up_event_data->cb_data, cri_core_service_up_event_data->service_event);

        if (cri_core_service_up_event_data->service_event == QMI_CLIENT_SERVICE_COUNT_INC)
        {
            if (qmi_service_client_info[(intptr_t)cri_core_service_up_event_data->cb_data].client_state == CRI_CORE_QMI_SERVICE_NOT_AVAILABLE)
            {
                UTIL_LOG_MSG("Before: number of active clients %d", client_info.num_of_active_clients);
                if (QMI_NO_ERR == (rc = qmi_client_get_service_instance(cri_core_service_up_event_data->svc_obj,
                                                          QMI_CLIENT_INSTANCE_ANY,
                                                          &info)))
                {
                    qmi_service_client_info[(intptr_t)cri_core_service_up_event_data->cb_data].client_state = CRI_CORE_QMI_SERVICE_AVAILABLE;
                    qmi_client_release(qmi_service_client_info[(intptr_t)cri_core_service_up_event_data->cb_data].notifier);
                    qmi_service_client_info[(intptr_t)cri_core_service_up_event_data->cb_data].notifier = NULL;
                    client_info.num_of_active_clients++;
                }
                else
                {
                    UTIL_LOG_MSG("qmi_client_get_service_instance return  %x %d", cri_core_service_up_event_data->svc_obj, rc);
                }

                UTIL_LOG_MSG("After: number of active clients %d", client_info.num_of_active_clients);
                UTIL_LOG_MSG("max_active_clients %d", client_info.max_active_clients);
                if (client_info.num_of_active_clients == client_info.max_active_clients)
                {
                    if(srv_up_cb)
                    {
                        srv_up_cb();
                    }
                }
            }
        }
    }while(FALSE);
}

void cri_core_set_operational_status( cri_core_gen_operational_status_type status )
{
    cri_core_op_status = status;
}

cri_core_gen_operational_status_type cri_core_get_operational_status(void)
{
    return cri_core_op_status;
}

uint8_t cri_core_mcm_ssr_resume(void)
{
    uint8_t status = FALSE;

    if(CRI_CORE_GEN_OPERATIONAL_STATUS_RESUMED == cri_core_get_operational_status())
    {
        status = TRUE;
    }

    return status;
}
