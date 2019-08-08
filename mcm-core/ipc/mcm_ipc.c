/***************************************************************************************************
    @file
    mcm_ipc.c

    @brief
    Supports functions for handling IPC requests.

   Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential. 
***************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include "mcm_data_srv_mgr_v01.h"
#include "mcm_atcop_srv_mgr_v01.h"
#include "mcm_mobileap_srv_mgr_v01.h"
#include "mcm_loc_srv_mgr_v01.h"
#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "mcm_ipc.h"
#include "mcm_client_v01.h"
#include "mcm_constants.h"

#define TRUE 1
#define FALSE 0

static int mcm_ipc_get_service_id(int srv_name);
static int mcm_ipc_register_exit_func(mcm_ipc_srv_mgr_exit_func *srv_down_func);

pthread_t mcm_srv_manager_thread;

typedef struct mcm_ipc_srv_info_t
{
    int                    service_id;
    qmi_csi_service_handle service_handle;
    mcm_ipc_exit_func_cb   exit_func_cb;
}mcm_ipc_srv_info;

static mcm_ipc_srv_info mcm_ipc_service_cookie;
static volatile int mcm_ipc_service_ready = 0; // 0-not ready 1-ready -1-error ocurred

typedef struct {
    qmi_client_handle clnt;
}mcm_ipc_client_info_type;

mcm_ipc_srv_info mcm_ipc_srv_client_hdl[MCM_IPC_MAX_SERVICES];

/*=============================================================================
  CALLBACK FUNCTION mcm_ipc_service_connect_cb
=============================================================================*/
static qmi_csi_cb_error mcm_ipc_service_connect_cb
(
    qmi_client_handle         client_handle,
    void                      *service_cookie,
    void                      **connection_handle
)
{
   qmi_csi_cb_error              ret_val        = QMI_CSI_NO_ERR;
   mcm_ipc_srv_info             *ipc_srv_ptr    = NULL;
   mcm_ipc_client_info_type     *ipc_clnt_ptr   = NULL;

   printf("\n mcm_ipc_service_connect_cb ENTER client_handle:%x\n", client_handle);

   do
   {
       /* TODO validate input */
       if (NULL == service_cookie)
       {
           ret_val = QMI_CSI_INVALID_ARGS;
           break;
       }
       ipc_srv_ptr = (mcm_ipc_srv_info *)service_cookie;
       //mcm_srv_client_registered(mcm_server_ptr, client_handle);
       //TODO

       /* memory released at disconnect_cb */
       ipc_clnt_ptr = malloc(sizeof(mcm_ipc_client_info_type));

       if( NULL == ipc_clnt_ptr)
       {
           /* memory allocation failure */
           ret_val = QMI_CSI_NO_MEM;
           break;
       }

       ipc_clnt_ptr->clnt = client_handle;
       *connection_handle = ipc_clnt_ptr;
   }while(0);

   printf("\n mcm_ipc_service_connect_cb ret_val:%d EXIT \n", ret_val);
   return ret_val;
}

/*=============================================================================
  CALLBACK FUNCTION mcm_ipc_service_disconnect_cb
=============================================================================*/
static void mcm_ipc_service_disconnect_cb
(
    void                      *connection_handle,
    void                      *service_cookie
)
{
    mcm_ipc_srv_info           *ipc_srv_ptr  = NULL;
    mcm_ipc_client_info_type   *ipc_clnt_ptr = NULL;

    printf("\n mcm_ipc_service_disconnect_cb ENTER \n");

    do
    {
        if ( NULL == service_cookie ||
             NULL == connection_handle)
        {
            /* nothing needs to be done */
            break;
        }

        ipc_srv_ptr = (mcm_ipc_srv_info *)service_cookie;
        ipc_clnt_ptr = (mcm_ipc_client_info_type *)connection_handle;

        free(connection_handle);
    }while(0);

    printf("\n mcm_ipc_service_disconnect_cb EXIT \n");
    return;
}
//=============================================================================
// FUNCTION: ipc_exit_func_default
//
// DESCRIPTION:
// default exit function for all services
//
// @param[in]
//    None
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void ipc_exit_func_default(void)
{
   exit(0);
}

//=============================================================================
// FUNCTION: mcm_ipc_service_handle_req_cb
//
// DESCRIPTION:
// Callback function to handle request from client
//
//=============================================================================
static qmi_csi_cb_error mcm_ipc_service_handle_req_cb
(
    void                  *connection_handle,
    qmi_req_handle         req_handle,
    int                    msg_id,
    void                  *req_c_struct,
    int                    req_c_struct_len,
    void                  *service_cookie
)
{
   qmi_csi_cb_error              ret_val        = QMI_CSI_NO_ERR;
   mcm_ipc_srv_info             *ipc_srv_ptr    = NULL;
   qmi_client_handle            *clnt_handle    = NULL;
   mcm_ipc_client_info_type     *ipc_clnt_ptr   = NULL;

   mcm_client_not_require_resp_msg_v01   not_require_resp_msg;

   int iter = 0;

   printf("\n mcm_ipc_service_handle_req_cb ENTER \n");

   do
   {
       if ( NULL == service_cookie)
       {
           ret_val = QMI_CSI_INVALID_ARGS;
           break;
       }

       if ( NULL == connection_handle)
       {
           ret_val = QMI_CSI_INVALID_HANDLE;
           break;
       }

       ipc_srv_ptr = (mcm_ipc_srv_info *)service_cookie;

       printf("\n requeset srv_id:%d\n", ipc_srv_ptr->service_id);

       if (MCM_DATA_SRV_MGR_UNLODE_REQ_V01 == msg_id ||
           MCM_ATCOP_SRV_MGR_UNLODE_REQ_V01 == msg_id ||
           MCM_LOC_SRV_MGR_UNLODE_REQ_V01 == msg_id ||
           MCM_MOBILEAP_SRV_MGR_UNLODE_REQ_V01 == msg_id)
       {
           for (iter = 0; iter < MCM_IPC_MAX_SERVICES; iter ++)
           {
               if( mcm_ipc_srv_client_hdl[iter].service_id == ipc_srv_ptr->service_id)
               {
                  memset(&not_require_resp_msg, 0, sizeof(mcm_client_not_require_resp_msg_v01));
                  not_require_resp_msg.response.error = MCM_SUCCESS_V01;
                  not_require_resp_msg.response.result = MCM_RESULT_SUCCESS_V01;

                  qmi_csi_send_resp(req_handle,msg_id,&not_require_resp_msg, sizeof(mcm_client_not_require_resp_msg_v01));
                  if ( NULL != mcm_ipc_srv_client_hdl[iter].exit_func_cb)
                  {
                      printf("\n exit func found calling....\n");
                      mcm_ipc_srv_client_hdl[iter].exit_func_cb();
                  }
                  else
                  {
                      printf("\n no exit function provided for this service, call default exit\n");
                      ipc_exit_func_default();
                  }
                  mcm_ipc_srv_client_hdl[iter].exit_func_cb = NULL;
                  mcm_ipc_srv_client_hdl[iter].service_handle = NULL;
                  mcm_ipc_srv_client_hdl[iter].service_id = 0;
                  break;
               }
           }
           if (iter == MCM_IPC_MAX_SERVICES)
           {
               printf("\n no service_id found break out");
               break;
           }
       }
   }while (0);

   printf("\n mcm_ipc_service_handle_req_cb EXIT ret_val:%d\n", ret_val);
   return ret_val;
}
//=============================================================================
// FUNCTION: mcm_ipc_get_service_id
//
// DESCRIPTION:
// query service_id from service enumeration value
//
//=============================================================================
int mcm_ipc_get_service_id(int srv_name)
{
    qmi_idl_service_object_type mcm_ipc_srv_obj = NULL;
    int service_id = 0;
    printf("\nmcm_ipc_get_service_id\n");
    switch (srv_name)
    {
        case MCM_DATA_V01:
            mcm_ipc_srv_obj = mcm_data_srv_mgr_get_service_object_v01();
            break;
        case MCM_ATCOP_V01:
            mcm_ipc_srv_obj = mcm_atcop_srv_mgr_get_service_object_v01();
            break;
        case MCM_MOBILEAP_V01:
            mcm_ipc_srv_obj = mcm_mobileap_srv_mgr_get_service_object_v01();
            break;
        case MCM_LOC_V01:
            mcm_ipc_srv_obj = mcm_loc_srv_mgr_get_service_object_v01();
           break;
   }
   qmi_idl_get_service_id( mcm_ipc_srv_obj, &service_id);
   printf("\nget service_id:%d\n", service_id);
   return service_id;
}
//=============================================================================
// FUNCTION: mcm_ipc_register_exit_func
//
// DESCRIPTION:
// Cache the exit function for each service
//
//=============================================================================
int mcm_ipc_register_exit_func
(
    mcm_ipc_srv_mgr_exit_func *srv_down_func
)
{
   int iter = 0;
   int iter_i = 0;
   int ret_val = 0;

   int service_id;
   do
   {
       if ( NULL == srv_down_func)
       {
          printf("NULL exit function\n");
          break;
       }
       printf("srv_id:%d",  srv_down_func->srv_id);
       service_id = mcm_ipc_get_service_id(srv_down_func->srv_id);
       for (iter = 0; iter < MCM_IPC_MAX_SERVICES; iter ++)
       {
          if (service_id == mcm_ipc_srv_client_hdl[iter].service_id)
          {
              printf("\n Found srv_id, break out");
              break;
          }
       }

       if (MCM_IPC_MAX_SERVICES == iter)
       {
           printf("\n not found srv_id\n");
           for (iter_i = 0; iter_i < MCM_IPC_MAX_SERVICES; iter_i ++ )
           {
              printf("\niter_i:%d service_id:%x", iter_i, mcm_ipc_srv_client_hdl[iter_i].service_id);
              if ( 0 == mcm_ipc_srv_client_hdl[iter_i].service_id)
              {
                 mcm_ipc_srv_client_hdl[iter_i].service_id = service_id;
                 mcm_ipc_srv_client_hdl[iter_i].exit_func_cb = srv_down_func->srv_exit_func;
                 ret_val = 1;
                 break;
              }
           }
       }
       else
       {
           mcm_ipc_srv_client_hdl[iter].exit_func_cb = srv_down_func->srv_exit_func;
           ret_val = 1;
       }
   }while(0);

   return ret_val;
}

//=============================================================================
// FUNCTION: mcm_ipc_srv_manager_init
//
// DESCRIPTION:
// thread that starts srv_mgr for the input service
//=============================================================================
int *mcm_ipc_srv_manager_init(void *srv_id)
{
    qmi_csi_os_params os_params,os_params_in;
    fd_set fds;
    void *sp;

    int iter_srv = 0;
    int service_id;
    int a = ((int) srv_id);
    printf("\n\n TEST service_id:%x\n\n", a);

    int ret_val = 1;
    int ret_val_i = 1;
    qmi_idl_service_object_type ipc_srv_obj = NULL;

    qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
    mcm_ipc_service_ready = 0;
    do
    {
        switch(a)
        {
            case MCM_DATA_V01:
                ipc_srv_obj = mcm_data_srv_mgr_get_service_object_v01();
                break;
            case MCM_ATCOP_V01:
                ipc_srv_obj = mcm_atcop_srv_mgr_get_service_object_v01();
                break;
            case MCM_MOBILEAP_V01:
                ipc_srv_obj = mcm_mobileap_srv_mgr_get_service_object_v01();
                break;
            case MCM_LOC_V01:
                ipc_srv_obj = mcm_loc_srv_mgr_get_service_object_v01();
                break;
            default:
                ret_val_i = 0;
                break;
        }
        if (TRUE != ret_val_i)
        {
            ret_val = FALSE;
            break;
        }

        printf("\n\n TEST TEST TEST mcm_ipc_srv_mgr_start ENTER");
        rc = qmi_csi_register(ipc_srv_obj,
                              mcm_ipc_service_connect_cb,
                              mcm_ipc_service_disconnect_cb,
                              mcm_ipc_service_handle_req_cb,
                              &mcm_ipc_service_cookie,
                              &os_params,
                              &mcm_ipc_service_cookie.service_handle);

        if (rc != QMI_CSI_NO_ERR)
        {
            sp = NULL;
            printf("\n TEST TEST mcm_ipc_srv_mgr_start failed rc:%d\n", rc);
            ret_val = FALSE;
        }
        sp = mcm_ipc_service_cookie.service_handle;
        qmi_idl_get_service_id( ipc_srv_obj, &service_id);
        mcm_ipc_service_cookie.service_id = service_id;

        printf("\n SERVICE:%x IS UP !!\n", service_id);
        mcm_ipc_service_ready = 1;
        while(1)
        {
            fds = os_params.fds;
            select(os_params.max_fd+1, &fds, NULL, NULL, NULL);
            qmi_csi_handle_event(sp, &os_params);
        }
        qmi_csi_unregister(sp);

    }while (0);

    mcm_ipc_service_ready = -1;

    return ret_val;
}
//=============================================================================
// FUNCTION: mcm_ipc_srv_mgr_start
//
// DESCRIPTION:
// Start srv_mgr thread for input service and cache its input exit function
//=============================================================================
int mcm_ipc_srv_mgr_start( mcm_ipc_srv_mgr_exit_func *srv_down_func)
{
   int err_code = 0;
   int a;
   a = srv_down_func->srv_id;
   err_code = pthread_create(&mcm_srv_manager_thread,
                             NULL,
                             mcm_ipc_srv_manager_init,
                             (void *)a);
   printf("mcm_ipc_srv_mgr_start: ERROR: 0x%x", err_code);
   mcm_ipc_register_exit_func(srv_down_func);
   return err_code;
}

int mcm_ipc_get_service_is_ready(void)
{
    return mcm_ipc_service_ready;
}


//=============================================================================
// FUNCTION: mcm_get_service_flag_file
//
// DESCRIPTION:
// get flag file path 
//
//=============================================================================
const char * mcm_get_service_flag_file(int srv_id)
{
    const char *path = NULL;

    switch(srv_id) {
        case MCM_DATA_SERVICE:
            path = MCM_DATA_SERVICE_READY_FILE;
            break;
        case MCM_ATCOP_SERVICE:
            path = MCM_ATCOP_SERVICE_READY_FILE;
            break;
        case MCM_MOBILEAP_SERVICE:
            path = MCM_MOBILEAP_SERVICE_READY_FILE;
            break;
        case MCM_LOC_SERVICE:
            path = MCM_LOC_SERVICE_READY_FILE;
            break;
        case MCM_SIM_SERVICE:
            path = MCM_SIM_SERVICE_READY_FILE; 
            break;
        case MCM_RIL_SERVICE: /** RIL service is ready */
            path = MCM_SERVICE_READY_FILE;
            break;
        default:
            break;
    }

    return path;
}

//=============================================================================
// FUNCTION: mcm_set_service_reday
//
// DESCRIPTION:
// mask whether service is ready
//
//=============================================================================
void mcm_set_service_ready(int srv_id, int is_ready)
{
    const char *path = NULL;

    path = mcm_get_service_flag_file(srv_id);

    if(path == NULL) {
        return;
    }

    if(is_ready) {
        FILE *fp = fopen(path, "w");
        if(fp) {
            fputs("1\n", fp);
            fflush(fp);
            fclose(fp);
        }
    }
    else {
        unlink(path);
    }
}

//=============================================================================
// FUNCTION: mcm_get_service_is_ready
//
// DESCRIPTION:
// check whether service is ready
//
//=============================================================================
int mcm_get_service_is_ready(int srv_id)
{
    const char *path = NULL;

    path = mcm_get_service_flag_file(srv_id);

    if(path == NULL) {
        return 0;
    }

    if(!access(path, F_OK)) {
        return 1;
    }

    return 0;
}
/** add by tyler.kuang@20180502 end */

