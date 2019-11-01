/*************************************************************************************
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 
**************************************************************************************/

#include "hlos_srv_mgr.h"
#include "hlos_core.h"
#include "mcm_client_v01.h"
#include "mcm_data_srv_mgr_v01.h"
#include "mcm_atcop_srv_mgr_v01.h"
#include "mcm_loc_srv_mgr_v01.h"
#include "mcm_mobileap_srv_mgr_v01.h"
#include "mcm_service_object_v01.h"
#include "mcm_srv_internal.h"

/* each entry records how many clients uses this service */
char service_ref_counts[MCM_MAX_SRVS];

static int hlos_srv_mgr_get_srv_handle(int srv_id, qmi_client_type *user_handle);
static int hlos_srv_start_srv_if_required(int srv_id);
static int hlos_srv_mgr_stop_srv_if_required(int srv_id);

//=============================================================================
// FUNCTION: hlos_srv_mgr_get_srv_handle
//
// DESCRIPTION:
// Query qmi handle for input service
//=============================================================================
int hlos_srv_mgr_get_srv_handle(int srv_id, qmi_client_type *user_handle)
{
    qmi_client_error_type         client_err = QMI_NO_ERR;
    qmi_idl_service_object_type   service_obj = NULL;
    qmi_service_info              info[10];
    qmi_client_os_params          os_params;

    int ret_val = TRUE;
    int ret_val_i = TRUE;
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    UTIL_LOG_MSG("hlos_srv_mgr_get_srv_handle ENTER srv_id:%x", srv_id);
    do
    {
        switch (srv_id)
        {
            case MCM_DATA_V01:
                service_obj = mcm_data_srv_mgr_get_service_object_v01();
                break;
            case MCM_LOC_V01:
                service_obj = mcm_loc_srv_mgr_get_service_object_v01();
                break;
            case MCM_MOBILEAP_V01:
                service_obj = mcm_mobileap_srv_mgr_get_service_object_v01();
                break;
            case MCM_ATCOP_V01:
                service_obj = mcm_atcop_srv_mgr_get_service_object_v01();
                break;
            case MCM_SIM_V01:
                service_obj = mcm_sim_get_service_object_v01();
                break;
            default:
                UTIL_LOG_MSG("\n unknown srv_id:%d", srv_id);
                ret_val_i = FALSE;
                break;
        }
        if (TRUE != ret_val_i)
        {
            ret_val = FALSE;
            break;
        }

        qmi_service_info    *service_info = NULL;
        uint32_t               num_services, num_entries;
        uint32_t               retry_count = 0;
        uint32_t               retry_timeout = 300;
        int rc = QMI_NO_ERR;
        service_info = &info[0];

        /* Check if the service is up, if not wait on a signal */
        while( retry_count < 2)
        {
            rc = qmi_client_get_service_list( service_obj, NULL, NULL, &num_services);
            UTIL_LOG_MSG("\n qmi_client_get_service_list rc:%d, num_services:%d\n", rc, num_services);

            if(rc == QMI_NO_ERR)
                break;

            /* wait for server to come up */
            UTIL_LOG_MSG("\n wait for %d milli seconds for server to come up\n",retry_timeout);
            QMI_CCI_OS_SIGNAL_WAIT(&os_params, retry_timeout);
            retry_count++;
            retry_timeout = 100;
            UTIL_LOG_MSG("\n Get service list retry - %d\n",retry_count);
        }

        if ( QMI_NO_ERR != rc)
        {
            ret_val = FALSE;
            break;
        }
        num_entries = num_services;
        /* The server has come up, store the information in info variable */
        rc = qmi_client_get_service_list( service_obj, service_info, &num_entries, &num_services);
        UTIL_LOG_MSG("\n qmi_client_get_service_list() returned %d num_entries = %d num_services = %d\n", rc, num_entries, num_services);

        rc = qmi_client_init(service_info, service_obj, /*ind_cb*/NULL, NULL, NULL, user_handle);

        if ( QMI_NO_ERR != rc)
        {
            UTIL_LOG_MSG("\n qmi_client_init failed exit rc:%d\n", rc);
            ret_val = FALSE;
            break;
        }
    }while(FALSE);

    if (FALSE == ret_val)
    {
        UTIL_LOG_MSG("\nfailed to get service handle");
        *user_handle = NULL;
    }
    UTIL_LOG_MSG("hlos_srv_mgr_get_srv_handle EXIT user_handle:%x", *user_handle);
    return ret_val;
}

//=============================================================================
// FUNCTION: hlos_srv_mgr_get_service
//
// DESCRIPTION:
// Query service and send indication for current available services
//=============================================================================
int hlos_srv_mgr_get_service(int mask) //-1 means pre_load_service
{
   UTIL_LOG_MSG("hlos_srv_mgr_get_service ENTER");

   int ret_val = TRUE;

   do
   {
      if (-1 == mask)
      {
         if ( NULL == user_handle[DATA_SRV])
         {
             hlos_srv_mgr_get_srv_handle(MCM_DATA_V01, &user_handle[DATA_SRV]);
             if ( (TRUE == user_handle_essential[DATA_SRV]) &&
                  (NULL == user_handle[DATA_SRV]))
             {
                 /* if essential service failes to start */
                 ret_val = FALSE;
                 break;
             }
         }
         if ( NULL == user_handle[LOC_SRV])
         {
            hlos_srv_mgr_get_srv_handle(MCM_LOC_V01, &user_handle[LOC_SRV]);
            if ( (TRUE == user_handle_essential[LOC_SRV]) &&
                 (NULL == user_handle[LOC_SRV]))
            {
                ret_val = FALSE;
                break;
            }
         }
         if ( NULL == user_handle[ATCOP_SRV])
         {
            hlos_srv_mgr_get_srv_handle(MCM_ATCOP_V01, &user_handle[ATCOP_SRV]);
            if ( (TRUE == user_handle_essential[ATCOP_SRV]) &&
                 (NULL == user_handle[ATCOP_SRV]))
            {
               ret_val = FALSE;
               break;
            }
         }
         if ( NULL == user_handle[MOBILEAP_SRV])
         {
            hlos_srv_mgr_get_srv_handle(MCM_MOBILEAP_V01, &user_handle[MOBILEAP_SRV]);
            if ( (TRUE == user_handle_essential[MOBILEAP_SRV]) &&
                 (NULL == user_handle[MOBILEAP_SRV]))
            {
               ret_val = FALSE;
               break;
            }
         }
         if ( NULL == user_handle[UIM_SRV])
         {
            hlos_srv_mgr_get_srv_handle(MCM_SIM_V01, &user_handle[UIM_SRV]);
            if ( ( TRUE == user_handle_essential[UIM_SRV]) &&
                 ( NULL == user_handle[UIM_SRV]))
            {
               ret_val = FALSE;
               break;
            }
         }
      }
      else
      {
         if ((( NULL == user_handle[DATA_SRV]) ||
              ((NULL != user_handle[DATA_SRV]) && (FALSE == user_handle_expect[DATA_SRV])))
           && ( MCM_DATA_V01 & mask))
         {
             hlos_srv_mgr_get_srv_handle(MCM_DATA_V01, &user_handle[DATA_SRV]);
             user_handle_expect[DATA_SRV] = TRUE;
         }
         if ((( NULL == user_handle[LOC_SRV]) ||
              ((NULL != user_handle[LOC_SRV]) && (FALSE == user_handle_expect[LOC_SRV])))
           && ( MCM_LOC_V01 & mask))
         {
             hlos_srv_mgr_get_srv_handle(MCM_LOC_V01, &user_handle[LOC_SRV]);
             user_handle_expect[LOC_SRV] = TRUE;
         }
         if ((( NULL == user_handle[ATCOP_SRV] )||
              ((NULL != user_handle[ATCOP_SRV]) && (FALSE == user_handle_expect[ATCOP_SRV])))
           && ( MCM_ATCOP_V01 & mask))
         {
             hlos_srv_mgr_get_srv_handle(MCM_ATCOP_V01, &user_handle[ATCOP_SRV]);
             user_handle_expect[ATCOP_SRV] = TRUE;
         }
         if ((( NULL == user_handle[MOBILEAP_SRV])||
              ((NULL != user_handle[MOBILEAP_SRV]) && (FALSE == user_handle_expect[MOBILEAP_SRV])))
           && ( MCM_MOBILEAP_V01 & mask))
         {
             hlos_srv_mgr_get_srv_handle(MCM_MOBILEAP_V01, &user_handle[MOBILEAP_SRV]);
             user_handle_expect[MOBILEAP_SRV] = TRUE;
         }
         if ((( NULL == user_handle[UIM_SRV]) ||
              ((NULL != user_handle[UIM_SRV]) &&( FALSE == user_handle_expect[UIM_SRV])))
           && ( MCM_SIM_V01 & mask))
         {
             hlos_srv_mgr_get_srv_handle(MCM_SIM_V01, &user_handle[UIM_SRV]);
             user_handle_expect[UIM_SRV] = TRUE;
         }
      }
   }while(FALSE);

   UTIL_LOG_MSG("mcm_srv_manager_get_service EXIT");
   return ret_val;
}

//=============================================================================
// FUNCTION: hlos_srv_start_srv_if_required
//
// DESCRIPTION:
// Handler start the service
//=============================================================================
int hlos_srv_start_srv_if_required(int srv_id)
{
   pid_t pid;
   int ret_val = TRUE;
   UTIL_LOG_MSG("hlos_srv_start_srv_if_required ENTER");

   switch (srv_id)
   {
       case UIM_SRV:
           if (service_ref_counts[UIM_SRV] == 1)
           {
               mcm_srv_sim_start();
           }
           break;

       case DATA_SRV:
           if (service_ref_counts[DATA_SRV] == 1)
           {
               pid=fork();
               if (pid==0)
               { /* child process */
                   char *argv[] = {DATA_EXEC_STR_ARG, NULL};
                   execv(DATA_EXEC_STR,argv);
                   exit(127); /* only if execv fails */
               }
           }
           break;

       case ATCOP_SRV:
           if (service_ref_counts[ATCOP_SRV] == 1)
           {
              pid = fork();
              if (pid == 0)
              { /* child process */
                  char *argv[] = {ATCOP_EXEC_STR_ARG, NULL};
                  execv(ATCOP_EXEC_STR, argv);
                  exit(127);
              }
           }
           break;

       case LOC_SRV:
           if (service_ref_counts[LOC_SRV] == 1)
           {
              pid = fork();
              if (pid == 0)
              { // child process
               char *argv[] = {LOC_EXEC_STR_ARG, NULL};
               execv(LOC_EXEC_STR, argv);
               exit(127);
              }
           }
           break;
       case MOBILEAP_SRV:
           if (service_ref_counts[MOBILEAP_SRV] == 1)
           {
              pid = fork();
              if (pid == 0)
              { /* child process */
                  char *argv[] = {MOBILEAP_EXEC_STR_ARG, QCMAP_DEFAULT_CONFIG, "d", NULL};
                  execv(MOBILEAP_EXEC_STR, argv);
                  exit(127);
              }
           }
           break;
       default:
           ret_val = FALSE;
           break;
   }

   UTIL_LOG_MSG("hlos_srv_start_srv_if_required EXIT ret_val:%d", ret_val);
   return ret_val;
}
//=============================================================================
// FUNCTION: hlos_srv_mgr_require_handle
//
// DESCRIPTION:
// Handler for require request
//=============================================================================
void hlos_srv_mgr_require_handle(void *event_data)
{
    cri_core_error_type                  ret_val = CRI_ERR_GENERAL_V01;
    cri_core_context_type                cri_core_context;
    hlos_core_hlos_request_data_type    *hlos_core_hlos_request_data = NULL;
    mcm_client_require_req_msg_v01      *mcm_require_req = NULL;
    mcm_client_require_resp_msg_v01      mcm_require_resp;
    qmi_client_handle                   *client_info = NULL;
    qmi_mcm_client_info                 *client_info_ptr = NULL;
    int iter = 0;

    UTIL_LOG_MSG("hlos_srv_mgr_require_handle ENTER");
    memset(&mcm_require_resp, NIL, sizeof(mcm_client_require_resp_msg_v01));
    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_require_req = (mcm_client_require_req_msg_v01*)hlos_core_hlos_request_data->data)) ||
            (NULL ==
             (client_info = (qmi_client_handle *)(hlos_core_hlos_request_data->data + sizeof(mcm_client_require_req_msg_v01))))
           )
        {
            UTIL_LOG_MSG("Invalid input parameter");
            break;
        }

        UTIL_LOG_MSG("hlos_srv_mgr_require_handle qmi_client_handle:%x", *client_info);

        if ( NIL == mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("require is EMPTY no action");
            break;
        }
        else
        {
            UTIL_LOG_MSG("require service:%x", mcm_require_req->require_service);

            for (iter = 0; iter < MCM_MAX_SRVS; iter++)
            {
                UTIL_LOG_MSG("\nuser_handle[%d]:%x service_ref_count[%d]:%d\n",
                             iter, user_handle[iter], iter, service_ref_counts[iter]);
            }
        }
        client_info_ptr = mcm_srv_find_client(*client_info, MCM_SRV_MANAGER);

        if (client_info_ptr == NULL)
        {
            UTIL_LOG_MSG("NULL client!");
            break;
        }

        UTIL_LOG_MSG("client:%x made the request", client_info_ptr->client_handle);

        for (iter = 0; iter < MCM_MAX_SRVS; iter++)
        {
            UTIL_LOG_MSG("ref_counts[%d]:%d", iter, client_info_ptr->ref_counts[iter]);
        }

        if ( MCM_SIM_V01 & mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("Request to start SIM");
            if (++client_info_ptr->ref_counts[UIM_SRV] == 1)
            {
                /* add this client to global ref counts */
                service_ref_counts[UIM_SRV]++;
                hlos_srv_start_srv_if_required(UIM_SRV);
            }
        }
        if ( MCM_DATA_V01 & mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("Request to start DATA");
            if (++client_info_ptr->ref_counts[DATA_SRV] == 1)
            {
               /* add this client to global ref counts */
               service_ref_counts[DATA_SRV] ++;
               hlos_srv_start_srv_if_required(DATA_SRV);
            }
        }
        if ( MCM_ATCOP_V01 & mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("Request to start ATCOP");
            if (++client_info_ptr->ref_counts[ATCOP_SRV] == 1)
            {
               /* add this client to global ref counts */
               service_ref_counts[ATCOP_SRV]++;
               hlos_srv_start_srv_if_required(ATCOP_SRV);
            }
        }
        if ( MCM_LOC_V01 & mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("Request to start LOC");
            if (++client_info_ptr->ref_counts[LOC_SRV] == 1)
            {
               /* add this client to global ref counts */
               service_ref_counts[LOC_SRV]++;
               hlos_srv_start_srv_if_required(LOC_SRV);
            }
        }
        if ( MCM_MOBILEAP_V01 & mcm_require_req->require_service)
        {
            UTIL_LOG_MSG("Request to start MOBILEAP");
            if (++client_info_ptr->ref_counts[MOBILEAP_SRV] == 1)
            {
               /* add this client to global ref counts */
               service_ref_counts[MOBILEAP_SRV]++;
               hlos_srv_start_srv_if_required(MOBILEAP_SRV);
            }
        }
        ret_val = CRI_ERR_NONE_V01;
    }while(FALSE);

    /* Update qmi handle for each service */
    /** add by tyler.kuang@20180503 start : check whether the services is ready */
    do {
        int all_ready = 0;
        int retry_cnt = (10*1000)/100; /* waiting 10 secs */
        while(retry_cnt>0 && !all_ready) {
            usleep(100000);
            retry_cnt --;
            all_ready = 1;
            if ( MCM_SIM_V01 & mcm_require_req->require_service) {
                if(!mcm_get_service_is_ready(MCM_SIM_SERVICE))  {
                    all_ready = 0;
                    continue;
                }
            }
            if ( MCM_DATA_V01 & mcm_require_req->require_service) {
                if(!mcm_get_service_is_ready(MCM_DATA_SERVICE))  {
                    all_ready = 0;
                    continue;
                }
            }
            if ( MCM_ATCOP_V01 & mcm_require_req->require_service) {
                if(!mcm_get_service_is_ready(MCM_ATCOP_SERVICE))  {
                    all_ready = 0;
                    continue;
                }
            }
            if ( MCM_LOC_V01 & mcm_require_req->require_service) {
                if(!mcm_get_service_is_ready(MCM_LOC_SERVICE))  {
                    all_ready = 0;
                    continue;
                }
            }
            if ( MCM_MOBILEAP_V01 & mcm_require_req->require_service) {
                if(!mcm_get_service_is_ready(MCM_MOBILEAP_SERVICE))  {
                    all_ready = 0;
                    continue;
                }
            }
        }
    } while(0);
    /** add by tyler.kuang@20180503 end */
    hlos_srv_mgr_get_service(mcm_require_req->require_service);

    if(NULL != hlos_core_hlos_request_data)
    {
        if (CRI_ERR_NONE_V01 != ret_val)
        {
            mcm_require_resp.response.result = MCM_RESULT_FAILURE_V01;
        }
        else
        {
            mcm_require_resp.response.result = MCM_RESULT_SUCCESS_V01;
        }
        mcm_require_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_require_resp,
                                sizeof(mcm_require_resp));
    }

    UTIL_LOG_MSG("hlos_client_require_handle EXIT ret_val:%d", ret_val);
}

//=============================================================================
// FUNCTION: hlos_srv_mgr_stop_srv_if_required
//
// DESCRIPTION:
// Handler for stopping the service if ref-counts reaches zero
//=============================================================================
int hlos_srv_mgr_stop_srv_if_required(int srv_id)
{
    mcm_data_srv_mgr_unload_req_msg_v01       *mcm_data_unload_req = NULL;
    mcm_data_srv_mgr_unload_resp_msg_v01       mcm_data_unload_resp;

    mcm_atcop_srv_mgr_unload_req_msg_v01      *mcm_atcop_unload_req = NULL;
    mcm_atcop_srv_mgr_unload_resp_msg_v01      mcm_atcop_unload_resp;

    mcm_loc_srv_mgr_unload_req_msg_v01        *mcm_loc_unload_req = NULL;
    mcm_loc_srv_mgr_unload_resp_msg_v01        mcm_loc_unload_resp;

    mcm_mobileap_srv_mgr_unload_req_msg_v01   *mcm_mobileap_unload_req = NULL;
    mcm_mobileap_srv_mgr_unload_resp_msg_v01   mcm_mobileap_unload_resp;

   int ret_val = TRUE;
   int ret_val_i = QMI_ERR_GENERAL_V01;

   UTIL_LOG_MSG("hlos_srv_mgr_stop_srv_if_required ENTER");
   switch (srv_id)
   {
       case UIM_SRV:
           if (service_ref_counts[UIM_SRV] == 0)
           {
               write(snd_rcv_nfd[RECEIVING_THREAD_FD], SEND_STR, sizeof(SEND_STR));
               qmi_client_release(user_handle[UIM_SRV]);
               ret_val_i = QMI_ERR_NONE_V01;
           }
           else
           {
               /* service can be started from service_config.txt OR
                  to be used by other clients, dont' release it */
               ret_val_i = QMI_ERR_NONE_V01;
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           break;

       case DATA_SRV:
           if ( service_ref_counts[DATA_SRV] == 0)
           {
               memset(&mcm_data_unload_resp, NIL, sizeof(mcm_data_srv_mgr_unload_resp_msg_v01));
               mcm_data_unload_req = util_memory_alloc(sizeof(mcm_data_srv_mgr_unload_req_msg_v01));
               ret_val_i = qmi_client_send_msg_sync(user_handle[DATA_SRV],
                                                    MCM_DATA_SRV_MGR_UNLODE_REQ_V01,
                                                    mcm_data_unload_req,
                                                    sizeof(mcm_data_srv_mgr_unload_req_msg_v01),
                                                    &mcm_data_unload_resp,
                                                    sizeof(mcm_data_srv_mgr_unload_resp_msg_v01),
                                                    1000);
               qmi_client_release(user_handle[DATA_SRV]);
           }
           else
           {
               /* service can be started from service_config.txt OR
                  to be used by other clients, dont' release it */
               ret_val_i = QMI_ERR_NONE_V01;
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           break;

       case ATCOP_SRV:
           if ( service_ref_counts[ATCOP_SRV] == 0)
           {
               memset(&mcm_atcop_unload_resp, NIL, sizeof(mcm_atcop_srv_mgr_unload_resp_msg_v01));
               mcm_atcop_unload_req = util_memory_alloc(sizeof(mcm_atcop_srv_mgr_unload_req_msg_v01));
               ret_val_i = qmi_client_send_msg_sync(user_handle[ATCOP_SRV],
                                                    MCM_ATCOP_SRV_MGR_UNLODE_REQ_V01,
                                                    mcm_atcop_unload_req,
                                                    sizeof(mcm_atcop_srv_mgr_unload_req_msg_v01),
                                                    &mcm_atcop_unload_resp,
                                                    sizeof(mcm_atcop_srv_mgr_unload_resp_msg_v01),
                                                    1000);
               qmi_client_release(user_handle[ATCOP_SRV]);
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           else
           {
               /* service can be started from service_config.txt OR
                  to be used by other clients, dont' release it */
               ret_val_i = QMI_ERR_NONE_V01;
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           break;

       case LOC_SRV:
           if ( service_ref_counts[LOC_SRV] == 0)
           {
               memset(&mcm_loc_unload_resp, NIL, sizeof(mcm_loc_srv_mgr_unload_resp_msg_v01));
               mcm_loc_unload_req = util_memory_alloc(sizeof(mcm_loc_srv_mgr_unload_req_msg_v01));
               ret_val_i = qmi_client_send_msg_sync(user_handle[LOC_SRV],
                                                    MCM_LOC_SRV_MGR_UNLODE_REQ_V01,
                                                    mcm_loc_unload_req,
                                                    sizeof(mcm_loc_srv_mgr_unload_req_msg_v01),
                                                    &mcm_loc_unload_resp,
                                                    sizeof(mcm_loc_srv_mgr_unload_resp_msg_v01),
                                                    1000);
               qmi_client_release(user_handle[LOC_SRV]);
           }
           else
           {
               /* service can be started from service_config.txt OR
                  to be used by other clients, dont' release it */
               ret_val_i = QMI_ERR_NONE_V01;
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           break;

       case MOBILEAP_SRV:
           if ( service_ref_counts[MOBILEAP_SRV] == 0)
           {
               memset(&mcm_mobileap_unload_resp, NIL, sizeof(mcm_mobileap_srv_mgr_unload_resp_msg_v01));
               mcm_mobileap_unload_req = util_memory_alloc(sizeof(mcm_mobileap_srv_mgr_unload_req_msg_v01));
               ret_val_i = qmi_client_send_msg_sync(user_handle[MOBILEAP_SRV],
                                                    MCM_MOBILEAP_SRV_MGR_UNLODE_REQ_V01,
                                                    mcm_mobileap_unload_req,
                                                    sizeof(mcm_mobileap_srv_mgr_unload_req_msg_v01),
                                                    &mcm_mobileap_unload_resp,
                                                    sizeof(mcm_mobileap_srv_mgr_unload_resp_msg_v01),
                                                    1000);
               qmi_client_release(user_handle[MOBILEAP_SRV]);
           }
           else
           {
               /* service can be started from service_config.txt OR
                  to be used by other clients, dont' release it */
               ret_val_i = QMI_ERR_NONE_V01;
               UTIL_LOG_MSG("service:%d is started from config file OR used by other clients, can not be unloaded", srv_id);
           }
           break;

       default:
           break;
   }
   ret_val = (ret_val_i != QMI_ERR_NONE)? FALSE:TRUE;
   
   UTIL_LOG_MSG("hlos_srv_mgr_stop_srv_if_required EXIT ret_val:%d", ret_val);
   return ret_val;
}
//=============================================================================
// FUNCTION: hlos_srv_mgr_not_require_handle
//
// DESCRIPTION:
// Handler for not require request
//=============================================================================
void hlos_srv_mgr_not_require_handle(void *event_data)
{
    cri_core_context_type                    cri_core_context;
    hlos_core_hlos_request_data_type        *hlos_core_hlos_request_data = NULL;
    mcm_client_not_require_req_msg_v01      *mcm_not_require_req = NULL;
    mcm_client_not_require_resp_msg_v01      mcm_not_require_resp;
    cri_core_error_type                      ret_val = CRI_ERR_GENERAL_V01;

    qmi_client_handle                       *client_info = NULL;
    qmi_mcm_client_info                     *client_info_ptr = NULL;
    int iter = 0;

    UTIL_LOG_MSG("hlos_srv_mgr_not_require_handle ENTER");
    memset(&mcm_not_require_resp, NIL, sizeof(mcm_client_not_require_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if ( NULL == hlos_core_hlos_request_data ||
             (NULL ==
              (mcm_not_require_req = (mcm_client_not_require_req_msg_v01*)hlos_core_hlos_request_data->data)) ||
             (NULL ==
              (client_info = (qmi_client_handle *)(hlos_core_hlos_request_data->data + sizeof(mcm_client_not_require_req_msg_v01))))
           )
        {
            UTIL_LOG_MSG("Invalid input parameter");
            break;
        }
        if ( NIL == mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("not require is EMPTY no action");
            break;
        }
        else
        {
            UTIL_LOG_MSG("not require service:%x", mcm_not_require_req->not_require_service);
            
            for (iter = 0; iter < MCM_MAX_SRVS; iter++)
            {
                UTIL_LOG_MSG("user_handle[%d]:%x service_ref_count[%d]:%d",
                             iter, user_handle[iter], iter, service_ref_counts[iter]);
            }
        }

        client_info_ptr = mcm_srv_find_client(*client_info, MCM_SRV_MANAGER);

        if (client_info_ptr == NULL)
        {
            UTIL_LOG_MSG("NULL client!");
            break;
        }
    
        UTIL_LOG_MSG("client:%x made the request", client_info_ptr->client_handle);

        for (iter = 0; iter < MCM_MAX_SRVS; iter++)
        {
            UTIL_LOG_MSG("ref_counts[%d]:%d", iter, client_info_ptr->ref_counts[iter]);
        }

        if (MCM_SIM_V01 & mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("Request to stop SIM");
            if(client_info_ptr->ref_counts[UIM_SRV] == 0)
            {
                UTIL_LOG_MSG("Client has no prior request on SIM");
                break;
            }
            else if ( --client_info_ptr->ref_counts[UIM_SRV] == 0)
            {
               service_ref_counts[UIM_SRV]--;
               user_handle_expect[UIM_SRV] = FALSE;
               if( FALSE == hlos_srv_mgr_stop_srv_if_required(UIM_SRV))
               {
                  break;
               }
            }
        }
        if ( MCM_DATA_V01 & mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("Request to stop DATA");
            if (client_info_ptr->ref_counts[DATA_SRV] == 0)
            {
                UTIL_LOG_MSG("Client has no prior request on DATA");
                break;
            }
            else if (--client_info_ptr->ref_counts[DATA_SRV] == 0)
            {
               service_ref_counts[DATA_SRV]--;
               user_handle_expect[DATA_SRV] = FALSE;
               if( FALSE == hlos_srv_mgr_stop_srv_if_required(DATA_SRV))
               {
                  break;
               }
            }
        }
        if ( MCM_ATCOP_V01 & mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("Request to stop ATCOP");
            if( client_info_ptr->ref_counts[ATCOP_SRV] == 0)
            {
                UTIL_LOG_MSG("Client has no prior request on ATCOP");
                break;
            }
            else if (--client_info_ptr->ref_counts[ATCOP_SRV] == 0)
            {
                service_ref_counts[ATCOP_SRV]--;
                user_handle_expect[ATCOP_SRV] = FALSE;
                if( FALSE == hlos_srv_mgr_stop_srv_if_required(ATCOP_SRV))
                {
                   break;
                }
            }
        }
        if ( MCM_LOC_V01 & mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("Request to stop LOC ");
            if(client_info_ptr->ref_counts[LOC_SRV] == 0)
            {
                UTIL_LOG_MSG("Client has no prior request on LOC");
                break;
            }
            else if (--client_info_ptr->ref_counts[LOC_SRV] == 0)
            {
                service_ref_counts[LOC_SRV]--;
                user_handle_expect[LOC_SRV] = FALSE;
                if( FALSE == hlos_srv_mgr_stop_srv_if_required(LOC_SRV))
                {
                   break;
                }
            }
        }
        if ( MCM_MOBILEAP_V01 & mcm_not_require_req->not_require_service)
        {
            UTIL_LOG_MSG("Request to stop MOBILEAP");
            if ( client_info_ptr->ref_counts[MOBILEAP_SRV] == 0)
            {
                UTIL_LOG_MSG("Client has no prior request on MOBILEAP");
                break;
            }
            else if (--client_info_ptr->ref_counts[MOBILEAP_SRV] == 0)
            {
                service_ref_counts[MOBILEAP_SRV]--;
                user_handle_expect[MOBILEAP_SRV] = FALSE;
                if( FALSE == hlos_srv_mgr_stop_srv_if_required(MOBILEAP_SRV))
                {
                   break;
                }
            }
        }
        ret_val = CRI_ERR_NONE_V01;
    }while(FALSE);

    /* Update qmi handle for each service */
    sleep(1);
    hlos_srv_mgr_get_service(mcm_not_require_req->not_require_service);

    if (NULL != hlos_core_hlos_request_data)
    {
        /* TODO need mapping from qmi to HLOS */
        mcm_not_require_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
        if (CRI_ERR_NONE_V01 != ret_val)
        {
            mcm_not_require_resp.response.result = MCM_RESULT_FAILURE_V01;
        }
        else
        {
            mcm_not_require_resp.response.result = MCM_RESULT_SUCCESS_V01;
        }
        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_not_require_resp,
                                sizeof(mcm_not_require_resp));
    }

    UTIL_LOG_MSG("hlos_srv_mgr_not_require_handle EXIT ret_val:%d", ret_val);

}
