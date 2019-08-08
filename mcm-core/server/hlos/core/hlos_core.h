
/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef HLOS_CORE
#define HLOS_CORE

#include "qmi_idl_lib.h"
#include "utils_common.h"
#include "cri_core.h"
#include "qmi_csi.h"
#include "common_v01.h"
#include "mcm_common_v01.h"

typedef struct hlos_core_hlos_request_data_type
{
    unsigned long event_id;
    void *token_id;
    void *data;
    size_t data_len;
}hlos_core_hlos_request_data_type;

typedef enum hlos_core_response_pipe_category_type
{
    RILD_PIPE = 1,
    IMS_PIPE
}hlos_core_response_pipe_category_type;

typedef struct hlos_core_request_type
{
    uint32_t clnt_token;
}hlos_core_request_type;

typedef struct hlos_core_resp_type
{
    qmi_response_type_v01 resp;
}hlos_core_resp_type;




/***************************************************************************************************
    @function
    hlos_core_send_response

    @brief
    Sends the response to HLOS for a received request.

    @param[in]
        hlos_core_response_pipe_category
            HLOS pipe that needs to be used for sending response
        cri_core_error
            calculated error code for the response
        hlos_core_hlos_request_data
            HLOS related information used for sending the response
        payload
            payload of the response that needs to be sent
        payload_len
            length of the payload

    @param[out]
        none

    @retval
        none
***************************************************************************************************/
void hlos_core_send_response(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                            uint32_t cri_core_error,
                            hlos_core_hlos_request_data_type *hlos_core_hlos_request_data,
                            void *payload,
                            size_t payload_len);

void hlos_core_send_response_handler(cri_core_error_type               cri_core_error,
                                     hlos_core_hlos_request_data_type *hlos_core_hlos_request_data,
                                     void                             *payload,
                                     size_t                            payload_len);

void hlos_core_send_sim_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                                   uint32_t msg_id,
                                   void *ind_data,
                                   size_t ind_data_len);

void hlos_core_send_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                                uint32_t msg_id,
                                void *ind_data,
                                size_t ind_data_len);

void hlos_core_send_srv_mgr_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                                uint32_t msg_id,
                                void *ind_data,
                                size_t ind_data_len);
/***************************************************************************************************
    @function
    hlos_core_get_token_id_value

    @brief
    Retrieves the token id value from the token id a specific HLOS.

    @param[in]
        token_id
            HLOS specific token id

    @param[out]
        none

    @retval
        value of the HLOS specific token id
***************************************************************************************************/
cri_core_hlos_token_id_type hlos_core_get_token_id_value(void *token_id);

/***************************************************************************************************
    @function
    hlos_core_sim_client_init

    @brief
    Initializes SIM QMI Clients.

    @retval
        error code
***************************************************************************************************/
uint32_t hlos_core_sim_client_init();

/***************************************************************************************************
    @function
    hlos_core_ril_client_init

    @brief
    Initializes RIL QMI Clients.

    @retval
        error code
***************************************************************************************************/
uint32_t hlos_core_ril_client_init();

/***************************************************************************************************
    @function
    hlos_ril_request_handler

    @brief
    Handles requests from MCM client.

    @retval
      returns true if the request is handled, false otherwise
***************************************************************************************************/

uint32_t hlos_core_ril_request_handler( void            *clnt_info,
                                        qmi_req_handle  req_handle,
                                        int             msg_id,
                                        void            *req_c_struct,
                                        int             req_c_struct_len,
                                        void            *service_handle);
uint32_t hlos_core_ril_request_handler_ex( void            *clnt_info,
                                        int              clnt_info_len,
                                  qmi_req_handle  req_handle,
                                  int             msg_id,
                                  void            *data,
                                  int             data_len,
                                  void            *service_handle);
mcm_error_t_v01 hlos_map_qmi_sim_error_to_mcm_error(qmi_error_type_v01 qmi_sim_error);

uint32_t hlos_map_qmi_ril_error_to_mcm_error(int qmi_ril_error);

void hlos_core_mcm_ssr_init(void);
void hlos_core_mcm_ssr_resume(void);
void hlos_core_register_for_service_down(ssr_srv_down_cb hlos_cb_srv_down_complete);
void hlos_cb_srv_down_complete(void);
void hlos_core_register_for_service_up_event(ssr_srv_up_cb hlos_cb_srv_up_complete);
void hlos_cb_srv_up_complete(void);
#endif

