/*************************************************************************************
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 
**************************************************************************************/

#include "qmi_idl_lib.h"
#include "qmi_csi.h"

#include "mcm_srv.h"
#include "mcm_srv_internal.h"
#include "hlos_core.h"
#include "mcm_constants.h"
#include "mcm_service_object_v01.h"
#include "mcm_sim_v01.h"
#include "mcm_srv_audio.h"
#include "mcm_data_srv_mgr_v01.h"
#include "mcm_atcop_srv_mgr_v01.h"
#include "mcm_mobileap_srv_mgr_v01.h"
#include "mcm_loc_srv_mgr_v01.h"
#include "hlos_srv_mgr.h"
#include "signal.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

static pthread_t mcm_sim_thread;
static pthread_t mcm_srv_manager_thread;

static mcm_srv_client_hdl_info ril_service_cookie;
static mcm_srv_client_hdl_info sim_service_cookie;
static mcm_srv_client_hdl_info srv_manager_cookie;

qmi_client_type     user_handle[MCM_MAX_SRVS];
int                 user_handle_essential[MCM_MAX_SRVS];
int                 user_handle_expect[MCM_MAX_SRVS];

int snd_rcv_nfd[]= {-1,-1};
typedef struct {
    qmi_client_handle clnt;
}client_info_type;

typedef struct mcm_ril_msg_id_map_t
{
    int mcm_msg_id_min;
    int mcm_msg_id_max;
}mcm_ril_msg_id_map;

mcm_ril_msg_id_map mcm_ril_msg_id_map_tbl[] =
{
    {MCM_RIL_DMS_SERVICE_MIN,   MCM_RIL_DMS_SERVICE_MAX },   /* DMS     */
    {MCM_RIL_NAS_SERVICE_MIN,   MCM_RIL_NAS_SERVICE_MAX },   /* NAS     */
    {MCM_RIL_SMS_SERVCIE_MIN,   MCM_RIL_SMS_SERVICE_MAX },   /* SMS     */
    {MCM_RIL_VOICE_SERVICE_MIN, MCM_RIL_VOICE_SERVICE_MAX }, /* VOICE   */
    {MCM_SIM_SERVICE_MIN, MCM_SIM_SERVICE_MAX},              /* SIM     */
    {MCM_SRV_MANAGER_MIN, MCM_SRV_MANAGER_MAX}               /* SRV_MGR */
};
#define ESSENTIAL_STR "essential"
#define NON_ESSENTIAL_STR "non-essentia"
#define MCM_DATA_STR       "mcm_data"
#define MCM_ATCOP_STR      "mcm_atcop"
#define MCM_MOBILEAP_STR   "mcm_mobileap"
#define MCM_LOC_STR        "mcm_loc"
#define MCM_SIM_STR        "mcm_sim"
#define CONFIG_FILE        "/data/service_config.txt"
qmi_csi_cb_error mcm_ril_request_handler
(
    qmi_client_handle        *clnt_info,
    qmi_req_handle           req_handle,
    int                      msg_id,
    void                     *req_c_struct,
    int                      req_c_struct_len,
    void                     *service_cookie
)
{
    qmi_csi_cb_error  rc = QMI_CSI_CB_INTERNAL_ERR;
    int iter_msg_id;

    UTIL_LOG_MSG(" mcm_ril_request_handler ENTER msg_id:%p", msg_id);

    int msg_id_tbl_size = sizeof(mcm_ril_msg_id_map_tbl)/sizeof(mcm_ril_msg_id_map);
    for (iter_msg_id = 0; iter_msg_id < msg_id_tbl_size; iter_msg_id ++ )
    {
        if(msg_id >= mcm_ril_msg_id_map_tbl[iter_msg_id].mcm_msg_id_min &&
           msg_id <= mcm_ril_msg_id_map_tbl[iter_msg_id].mcm_msg_id_max)
        {
           UTIL_LOG_MSG(" RIL msg_id, breakout ");
           /* RIL MSG_ID break out */
           break;
        }
    }
    if (msg_id_tbl_size == iter_msg_id)
    {
        UTIL_LOG_MSG(" NOT RIL MSG_ID, EXIT");
        return rc;
    }

    switch (msg_id)
    {
        case MCM_DM_EVENT_REGISTER_REQ_V01:
        case MCM_NW_EVENT_REGISTER_REQ_V01:
        case MCM_VOICE_EVENT_REGISTER_REQ_V01:
        case MCM_SMS_EVENT_REGISTER_REQ_V01:
        case MCM_SIM_EVENT_REGISTER_REQ_V01:
            rc = mcm_srv_registration_event_request_handler(clnt_info,
                                                 req_handle,
                                                 msg_id,
                                                 req_c_struct,
                                                 req_c_struct_len,
                                                 service_cookie);
            break;
        case MCM_CLIENT_REQUIRE_REQ_V01:
        case MCM_CLIENT_NOT_REQUIRE_REQ_V01:
            rc = hlos_core_ril_request_handler_ex(clnt_info,
                                               sizeof(qmi_client_handle),
                                               req_handle,
                                               msg_id,
                                               req_c_struct,
                                               req_c_struct_len,
                                               service_cookie);
            break;
        default:
            rc = hlos_core_ril_request_handler(clnt_info,
                                               req_handle,
                                               msg_id,
                                               req_c_struct,
                                               req_c_struct_len,
                                               service_cookie);
            break;
    }

    return rc;
}

/*=============================================================================
  CALLBACK FUNCTION mcm_srv_ril_service_connect_cb
=============================================================================*/
static qmi_csi_cb_error mcm_srv_ril_service_connect_cb
(
    qmi_client_handle         client_handle,
    void                      *service_cookie,
    void                      **connection_handle
)
{
    qmi_csi_cb_error          ret_val        = QMI_CSI_NO_ERR;
    mcm_srv_client_hdl_info  *mcm_server_ptr = NULL;
    client_info_type         *clnt_info = NULL;

    UTIL_LOG_MSG(" mcm_srv_ril_service_connect_cb ENTER client_handle:%x", client_handle);

    do
    {
        /* TODO validate input */
        if (NULL == service_cookie)
        {
            ret_val = QMI_CSI_INVALID_ARGS;
            break;
        }
        mcm_server_ptr = (mcm_srv_client_hdl_info *)service_cookie;
        mcm_srv_client_registered(mcm_server_ptr, client_handle);

        /* memory released at disconnect_cb */
        clnt_info = malloc(sizeof(client_info_type));

        if( NULL == clnt_info)
        {
            /* memory allocation failure */
            ret_val = QMI_CSI_NO_MEM;
            break;
        }

        clnt_info->clnt = client_handle;
        *connection_handle = clnt_info;
    }while(0);

    UTIL_LOG_MSG(" mcm_srv_ril_service_connect_cb ret_val:%d EXIT ");
    return ret_val;
}


/*=============================================================================
  CALLBACK FUNCTION mcm_srv_ril_service_disconnect_cb
=============================================================================*/
static void mcm_srv_ril_service_disconnect_cb
(
    void                      *connection_handle,
    void                      *service_cookie
)
{
    mcm_srv_client_hdl_info  *mcm_server_ptr = NULL;
    client_info_type         *clnt_info = NULL;

    UTIL_LOG_MSG(" mcm_srv_ril_service_disconnect_cb ENTER ");

    do
    {
        if ( NULL == service_cookie ||
             NULL == connection_handle)
        {
            /* nothing needs to be done */
            break;
        }

        mcm_server_ptr = (mcm_srv_client_hdl_info *)service_cookie;
        clnt_info = (client_info_type *)connection_handle;

        mcm_srv_client_unregistered(mcm_server_ptr, clnt_info->clnt);

        free(connection_handle);
    }while(0);

    UTIL_LOG_MSG(" mcm_srv_ril_service_disconnect_cb EXIT ");
    return;
}

/*=============================================================================
  CALLBACK FUNCTION mcm_srv_ril_service_handle_req_cb
=============================================================================*/
static qmi_csi_cb_error mcm_srv_ril_service_handle_req_cb
(
    void                     *connection_handle,
    qmi_req_handle           req_handle,
    int                      msg_id,
    void                     *req_c_struct,
    int                      req_c_struct_len,
    void                     *service_cookie
)
{
    qmi_csi_cb_error          ret_val        = QMI_CSI_NO_ERR;
    mcm_srv_client_hdl_info  *mcm_server_ptr = NULL;
    qmi_client_handle        *clnt_handle    = NULL;
    client_info_type         *clnt_info      = NULL;

    int iter_srv;
    int iter_client;

    UTIL_LOG_MSG(" mcm_srv_ril_service_handle_req_cb ENTER ");

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

        mcm_server_ptr = (mcm_srv_client_hdl_info *)service_cookie;

        /* check for valid service_id and client_handle */
        for (iter_srv = 0; iter_srv < MCM_MAX_SERVICES; iter_srv ++)
        {
            if (mcm_srv_client_hdl[iter_srv].service_id ==
                mcm_server_ptr->service_id)
            {
               UTIL_LOG_MSG(" SERVICE_ID FOUND iter_srv:%d !!", iter_srv);
               break;
            }
        }

        if (iter_srv == MCM_MAX_SERVICES)
        {
            /* service not found, return */
            ret_val = QMI_CSI_INVALID_ARGS;
            break;
        }

        clnt_info = (client_info_type *)connection_handle;
        for (iter_client = 0; iter_client < MCM_MAX_CLIENTS; iter_client ++)
        {
            if (mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle ==
                clnt_info->clnt)
            {
                UTIL_LOG_MSG(" CLIENT FOUND !! iter_client:%d client_handle:%x",
                             mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle);
                break;
            }
        }
        if (iter_client == MCM_MAX_CLIENTS)
        {
            /* client_handle not found, return */
            ret_val = QMI_CSI_INVALID_HANDLE;
            break;
        }

        ret_val = mcm_ril_request_handler(&(clnt_info->clnt),
                                 req_handle,
                                 msg_id,
                                 req_c_struct,
                                 req_c_struct_len,
                                 service_cookie);
        printf("\nmcm_srv_ril_service_handle_req_cb client_handle:%x\n", clnt_info->clnt);
    }while (0);
    UTIL_LOG_MSG(" mcm_srv_ril_service_handle_req_cb EXIT ret_val:%d", ret_val);
    return ret_val;
}


void *mcm_uim_service_init(void)
{
    char buf[10];
    qmi_csi_os_params   uim_os_params,os_params_in;
    fd_set              fds;
    int                 service_id;
    void                *uim_sp = NULL;
    qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

    UTIL_LOG_MSG("\n\n mcm_uim_service_init uim_sp:%x\n", uim_sp);

    qmi_idl_service_object_type mcm_sim_service_object = mcm_sim_get_service_object_v01();

    /* start UIM service */
    rc = qmi_csi_register(mcm_sim_service_object,
                          mcm_srv_ril_service_connect_cb,
                          mcm_srv_ril_service_disconnect_cb,
                          mcm_srv_ril_service_handle_req_cb,
                          &sim_service_cookie,
                          &uim_os_params,
                          &sim_service_cookie.service_handle);

    if (rc != QMI_CSI_NO_ERR)
    {
        uim_sp = NULL;
        UTIL_LOG_MSG("\n mcm_uim_service_init failed rc:%d\n", rc);
        return -1;
    }
    else
    {
        uim_sp = sim_service_cookie.service_handle;
        qmi_idl_get_service_id( mcm_sim_service_object, &service_id);
        sim_service_cookie.service_id = service_id;
        UTIL_LOG_MSG("\n\nstart uim_sp:%x\n", uim_sp);

        uim_os_params.max_fd = snd_rcv_nfd[SENDING_THREAD_FD] > uim_os_params.max_fd ? snd_rcv_nfd[SENDING_THREAD_FD] : uim_os_params.max_fd;
    }

    mcm_srv_client_registered(&sim_service_cookie, NULL);
    qmi_util_log("\n SERVICE:%x IS UP !!\n", service_id);

    while(1)
    {
        fds = uim_os_params.fds;
        FD_SET(snd_rcv_nfd[SENDING_THREAD_FD], &fds);
        select(uim_os_params.max_fd+1, &fds, NULL, NULL, NULL);

        if(FD_ISSET(snd_rcv_nfd[SENDING_THREAD_FD], &fds))
        {
           int n = read(snd_rcv_nfd[SENDING_THREAD_FD], buf, sizeof(buf));
           if ( 0 == strcmp(buf, SEND_STR))
           {
              UTIL_LOG_MSG("\nstring match\n");
              break;
           }
        }
        os_params_in.fds = fds;
        qmi_csi_handle_event(uim_sp, &os_params_in);
    }

    /* unregister UIM service from QMI Framework */
    qmi_csi_unregister(uim_sp);
    return NULL;
} /* mcm_uim_service_init */


void *mcm_srv_manager_init(void)
{
    qmi_csi_os_params os_params;
    fd_set fds;
    void *sp;
    int service_id;

    qmi_idl_service_object_type mcm_srv_manager_object = mcm_client_get_service_object_v01();

    qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

    UTIL_LOG_MSG(" mcm_srv_manager_init ENTER ");

    rc = qmi_csi_register(mcm_srv_manager_object,
                          mcm_srv_ril_service_connect_cb,
                          mcm_srv_ril_service_disconnect_cb,
                          mcm_srv_ril_service_handle_req_cb,
                          &srv_manager_cookie,
                          &os_params,
                          &srv_manager_cookie.service_handle);

    if (rc != QMI_CSI_NO_ERR)
    {
        sp = NULL;
        UTIL_LOG_MSG(" mcm_srv_manager_init failed rc:%d", rc);
        return -1;
    }
    else
    {
        sp = srv_manager_cookie.service_handle;
        qmi_idl_get_service_id( mcm_srv_manager_object, &service_id);
        srv_manager_cookie.service_id = service_id;
    }

    mcm_srv_client_registered(&srv_manager_cookie, NULL);
    UTIL_LOG_MSG(" SERVICE:%x IS UP !!", service_id);

    while(1)
    {
        fds = os_params.fds;
        select(os_params.max_fd+1, &fds, NULL, NULL, NULL);
        qmi_csi_handle_event(sp, &os_params);
    }
    qmi_csi_unregister(sp);
    return NULL;
} /* mcm_srv_manager_init */

int *mcm_srv_ril_service_init(void)
{
    qmi_csi_os_params os_params;
    fd_set fds;
    void *sp;
    int service_id;

    qmi_idl_service_object_type mcm_srv_ril_service_object = mcm_ril_service_get_service_object_v01();

    qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
    UTIL_LOG_MSG(" mcm_srv_ril_service_init ");

    rc = qmi_csi_register(mcm_srv_ril_service_object,
                        mcm_srv_ril_service_connect_cb,
                        mcm_srv_ril_service_disconnect_cb,
                        mcm_srv_ril_service_handle_req_cb,
                        &ril_service_cookie,
                        &os_params,
                        &ril_service_cookie.service_handle);

    if (rc != QMI_CSI_NO_ERR)
    {
        sp = NULL;
        UTIL_LOG_MSG(" mcm_srv_ril_service_init failed rc:%d", rc);
        return -1;
    }
    else
    {
        sp = ril_service_cookie.service_handle;
        qmi_idl_get_service_id( mcm_srv_ril_service_object, &service_id);
        ril_service_cookie.service_id = service_id;
    }
    mcm_srv_client_registered(&ril_service_cookie, NULL);

    UTIL_LOG_MSG(" SERVICE:%x IS UP !!", service_id);

    while(1)
    {
        fds = os_params.fds;
        select(os_params.max_fd+1, &fds, NULL, NULL, NULL);
        qmi_csi_handle_event(sp, &os_params);
    }
    qmi_csi_unregister(sp);
    return NULL;
}

//=============================================================================
// FUNCTION: mcm_srv_manager_preload_service_required
//
// DESCRIPTION:
// Check if service_config.txt file exists
//
// @param[in]
//    None
//
//
// @param[out]
//    None
//
// @return
//
//    TRUE - file exists
//    FALSE - file does not exist
//=============================================================================
int mcm_srv_manager_preload_service_required(void)
{
    FILE *fp;
    int ret_val = FALSE;
    fp = fopen(CONFIG_FILE, "r");

    if (NULL != fp)
    {
        /* file exists */
        ret_val = TRUE;
    }
    return ret_val;
}
mcm_srv_mgr_start_services(char *buffer_str, int is_essential)
{
    pid_t pid = 0;

    if (0 ==strncmp(buffer_str, MCM_DATA_STR, strlen(MCM_DATA_STR)))
    {
        UTIL_LOG_MSG("\n mcm_data found\n");
        user_handle_essential[DATA_SRV] = is_essential;
        service_ref_counts[DATA_SRV]++;
        pid=fork();
        if (pid==0)
        { /* child process */
            char *argv[] = {DATA_EXEC_STR_ARG, NULL};
            execv(DATA_EXEC_STR,argv);
            exit(127); /* only if execv fails */
        }
    }
    if ( 0 == strncmp(buffer_str, MCM_ATCOP_STR, strlen(MCM_ATCOP_STR)))
    {
        UTIL_LOG_MSG("\n mcm_atcop found \n");
        user_handle_essential[ATCOP_SRV] = is_essential;
        service_ref_counts[ATCOP_SRV]++;
        pid = fork();
        if (pid == 0)
        { /* child process */
            char *argv[] = {ATCOP_EXEC_STR_ARG, NULL};
            execv(ATCOP_EXEC_STR, argv);
            exit(127);
        }
    }
    if ( 0 == strncmp(buffer_str, MCM_SIM_STR, strlen(MCM_SIM_STR)))
    {
        UTIL_LOG_MSG("\n mcm_sim found\n");
        user_handle_essential[UIM_SRV] = is_essential;
        service_ref_counts[UIM_SRV]++;
        mcm_srv_sim_start();
    }
    if ( 0 == strncmp(buffer_str, MCM_MOBILEAP_STR, strlen(MCM_MOBILEAP_STR)))
    {
        UTIL_LOG_MSG("\n mcm_mobileap found\n");
        user_handle_essential[MOBILEAP_SRV] = is_essential;
        service_ref_counts[MOBILEAP_SRV]++;
        pid = fork();
        if (pid == 0)
        { /* child process */
            static char *argv[] = {MOBILEAP_EXEC_STR, QCMAP_DEFAULT_CONFIG, "d", NULL};
            execv(MOBILEAP_EXEC_STR, argv);
            exit(127);
        }
    }
    if ( 0 == strncmp(buffer_str, MCM_LOC_STR, strlen(MCM_LOC_STR)))
    {
        UTIL_LOG_MSG("\n mcm_loc found\n");
        user_handle_essential[LOC_SRV] = is_essential;
        service_ref_counts[LOC_SRV]++;
        pid = fork();
        if (pid == 0)
        { /* child process */
            char *argv[] = {LOC_EXEC_STR, NULL};
            execv(LOC_EXEC_STR, argv);
            exit(127);
        }
    }
}
//=============================================================================
// FUNCTION: mcm_srv_manager_start_preload_service
//
// DESCRIPTION:
// Handler for pre-loading services stored from service_config.txt
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
void mcm_srv_manager_start_preload_service(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *buffer1 = NULL;
    char *buffer2 = NULL;
    char *buffer_essential = NULL;
    char *buffer_non_essential = NULL;

    UTIL_LOG_MSG("\nmcm_srv_manager_start_preload_service ENTER\n");
    fp = fopen(CONFIG_FILE, "r");
    if (fp != NULL)
    {
        while ((read = getline(&line, &len, fp)) != -1)
        {
            UTIL_LOG_MSG("Retrieved line of length %d :\n", read);
            buffer1 = strstr(line, ESSENTIAL_STR);
            buffer2 = strstr(line, NON_ESSENTIAL_STR);
            if (NULL != buffer1)
            {
                UTIL_LOG_MSG("buffer1:%s\n", buffer1);
                buffer_essential = strchr(buffer1, 'm');
                if (NULL != buffer_essential) {
                    UTIL_LOG_MSG("buffer_essential:%s\n", buffer_essential);
                    mcm_srv_mgr_start_services(buffer_essential, TRUE);
                }
            }/*if (NULL != buffer1)*/

            if (NULL != buffer2)
            {
                UTIL_LOG_MSG("\nbuffer2:%s\n", buffer2);
                buffer_non_essential = strchr(buffer2, 'm');
                if ( NULL != buffer_non_essential)
                {
                    UTIL_LOG_MSG("buffer_not_essential:%s\n", buffer_non_essential);
                    mcm_srv_mgr_start_services(buffer_non_essential, FALSE);
                }
            }/* if (NULL != buffer2) */
        }/* while */
        free(line);
    }/* fp != NULL */
    else
    {
       UTIL_LOG_MSG("\n no service_config.txt found");
    }
    UTIL_LOG_MSG("\nmcm_srv_manager_start_preload_service END\n");
}
//=============================================================================
// FUNCTION: mcm_srv_sim_start
//
// DESCRIPTION:
// Start UIM service thread
//
// @param[in]
//    None
//
//
// @param[out]
//    None
//
// @return
//
//    None
//=============================================================================
void mcm_srv_sim_start(void)
{
    int err_code = ENOMEM;

    err_code = pthread_create(&mcm_sim_thread,
                              NULL,
                              mcm_uim_service_init,
                              NULL);

    UTIL_LOG_MSG("mcm_srv_sim_start: ERROR: 0x%x", err_code);
}
//=============================================================================
// FUNCTION: mcm_srv_manager_start
//
// DESCRIPTION:
// start thread for service manager
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
void mcm_srv_manager_start(void)
{
    int err_code = ENOMEM;

    err_code = pthread_create(&mcm_srv_manager_thread,
                              NULL,
                              mcm_srv_manager_init,
                              NULL);
    UTIL_LOG_MSG("mcm_srv_manager_start: ERROR: 0x%x", err_code);
}

//=============================================================================
// FUNCTION: process_exit_handler
//
// DESCRIPTION:
// Handler for exited process
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
void process_exit_handler(int signal)
{
   int child_status;

   UTIL_LOG_MSG("\n child_process_die_handler ENTER received signal:%d\n", signal);

   switch (signal)
   {
       case SIGCHLD:
           /* remove PID entries */
           wait(&child_status);
           UTIL_LOG_MSG("\n child_status:%d", child_status);
           break;
       default:
           break;
   }
   UTIL_LOG_MSG("\nchild_process_die_handler EXIT");
}

//=============================================================================
// FUNCTION: main
//
// DESCRIPTION:
// main entry for mcm_ril_service
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
int main(void)
{
    int ret_val;

    ret_val = Diag_LSM_Init(NULL);
    if ( !ret_val )
    {
        UTIL_LOG_MSG("Failed to initialize DIAG interface for RIL service login");
        exit(1);
    }

    UTIL_LOG_MSG("Initializing audio... ");
    mcm_srv_snd_init();
    UTIL_LOG_MSG("Done.\n");

    mcm_srv_qmi_client_init();

    ret_val  = socketpair (AF_UNIX, SOCK_STREAM, 0, snd_rcv_nfd);

    if ( -1 == ret_val)
    {
       UTIL_LOG_MSG("Failed to create socket\n");
       exit(1);
    }
    /* start srv_manager service */
    mcm_srv_manager_start();

    /* zombie process handler*/
    signal(SIGCHLD, process_exit_handler);

    if( TRUE == mcm_srv_manager_preload_service_required())
    {
        mcm_srv_manager_start_preload_service();
        sleep(1);
        /* query handle from other services */

        if( FALSE == hlos_srv_mgr_get_service(TRUE))
        {
            /* essential service not start properly, abort !!! */
            exit(0);
        }
    }
    // RIL SERVICE
    mcm_srv_ril_service_init();
    return 0;
}

