/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_VOICE_SS
#define CRI_VOICE_SS

#include "cri_voice.h"

cri_core_error_type cri_voice_get_call_forwarding_status_req_handler(cri_core_context_type cri_core_context,
                                                                     const cri_voice_get_call_forwarding_status_request_type* req_message,
                                                                     const void* user_data,
                                                                     cri_voice_request_get_call_forwarding_status_cb_type get_call_forwarding_status_cb);


cri_core_error_type cri_voice_set_call_forwarding_req_handler(cri_core_context_type cri_core_context,
                                                              const cri_voice_set_call_forwarding_request_type* req_message,
                                                              const void* user_data,
                                                              cri_voice_request_set_call_forwarding_cb_type set_call_forwarding_cb);

cri_core_error_type cri_voice_get_call_waiting_status_req_handler(cri_core_context_type cri_core_context,
                                                                  const void* user_data,
                                                                  cri_voice_request_get_call_waiting_status_cb_type get_call_waiting_status_cb);

cri_core_error_type cri_voice_set_call_waiting_req_handler(cri_core_context_type cri_core_context,
                                                           const cri_voice_set_call_waiting_request_type* req_message,
                                                           const void* user_data,
                                                           cri_voice_request_set_call_waiting_cb_type set_call_waiting_cb);

cri_core_error_type cri_voice_get_clir_req_handler(cri_core_context_type cri_core_context,
                                                   const void* user_data,
                                                   cri_voice_request_get_clir_cb_type get_clir_cb);

cri_core_error_type cri_voice_set_clir_req_handler(cri_voice_clir_type clir_type);

cri_core_error_type cri_voice_change_call_barring_password_req_handler(cri_core_context_type cri_core_context,
                                                                       const cri_voice_change_call_barring_password_request_type* req_message,
                                                                       const void* user_data,
                                                                       cri_voice_request_change_call_barring_password_cb_type change_call_barring_password_cb);

cri_core_error_type cri_voice_send_ussd_req_handler(cri_core_context_type cri_core_context,
                                                    const cri_voice_send_ussd_request_type* req_message,
                                                    const void* user_data,
                                                    cri_voice_request_send_ussd_cb_type send_ussd_cb);

cri_core_error_type cri_voice_cancel_ussd_req_handler(cri_core_context_type cri_core_context,
                                                      const void* user_data,
                                                      cri_voice_request_cancel_ussd_cb_type cancel_ussd_cb);

// USSD Indication
void cri_voice_ind_hdlr_ussd_ind(int qmi_service_client_id,
                                 const voice_ussd_ind_msg_v02* ussd_ind_ptr);

cri_voice_clir_type cri_voice_obtain_stored_clir();



/*
cri_core_error_type cri_voice_set_facility_lock_req_handler(cri_core_context_type cri_core_context,
                                                            const cri_voice_set_facility_lock_request_type* req_message,
                                                            const void* user_data,
                                                            cri_voice_request_set_facility_lock_cb_type set_facility_lock_cb);

*/


#endif
