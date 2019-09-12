/*!
  @file
  atfwd_daemon.c

  @brief
  ATFWD daemon which registers with QMI ATCOP service and forwards AT commands

*/

/*===========================================================================

Copyright (c) 2015, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/11/11  jaimel  ATFWD-daemon to register and forward AT commands to Apps
04/19/11  c_spotha Added default port changes

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_TAG "Atfwd_Daemon"
//#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
//#include "sendcmd.h"
#include <qmi_atcop_srvc.h>
#include <string.h>
#include <cutils/properties.h>
//#include "common_log.h"
#include "quectel_at_handle.h"
//#include "AtCmdFwd.h"
//#include "mdm_detect.h"

//#include "qmi-framework/inc/qmi_client.h"
//#include "qmi-framework/inc/qmi_client_instance_defs.h" 

#include "qmi_client.h"
#include "qmi_client_instance_defs.h" 

#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib.h"
#include "access_terminal_service_v01.h"

#define ATFWD_ATCOP_PORTS                 2
#define ATFWD_MAX_RETRY_ATTEMPTS          5
#define ATFWD_RETRY_DELAY                 5  /* Retry delay in sec */

//#define LOGI(...) fprintf(stderr, "I:" __VA_ARGS__)

//#define QUECTEL_AT_QAPSUB_FEATURE 
#define QUECTEL_FCT_TEST  

#define MAX_DIGITS 10
#define DEFAULT_QMI_PORT QMI_PORT_RMNET_SDIO_0
#define DEFAULT_SMD_PORT QMI_PORT_RMNET_0

#define ATFWD_DATA_PROP_BASEBAND            "ro.baseband"
#define ATFWD_DATA_PROP_BOARD_PLATFORM      "ro.board.platform"

#define ATFWD_DATA_TARGET_MSM         "msm"
#define ATFWD_DATA_TARGET_MSM8998     "msmcobalt"
#define ATFWD_DATA_TARGET_MSM8996     "msm8996"
#define ATFWD_DATA_TARGET_MSM8994     "msm8994"
#define ATFWD_DATA_TARGET_MSM8992     "msm8992"
#define ATFWD_DATA_TARGET_MSM8916     "msm8916"
#define ATFWD_DATA_TARGET_MSM8936     "msm8936"
#define ATFWD_DATA_TARGET_MSM8939     "msm8939"
#define ATFWD_DATA_TARGET_MSM8909     "msm8909"
#define ATFWD_DATA_TARGET_MSM8952     "msm8952"
#define ATFWD_DATA_TARGET_MSM8953     "msm8953"
#define ATFWD_DATA_TARGET_MSM8937     "msm8937"
#define ATFWD_DATA_TARGET_SVLTE1      "svlte1"
#define ATFWD_DATA_TARGET_SVLTE2A     "svlte2a"
#define ATFWD_DATA_TARGET_CSFB        "csfb"
#define ATFWD_DATA_TARGET_MDM         "mdm"
#define ATFWD_DATA_TARGET_MDM2        "mdm2" /* For Fusion 4.5 devices */
#define ATFWD_DATA_TARGET_SGLTE       "sglte"
#define ATFWD_DATA_TARGET_APQ         "apq" /* unused right now */

 /* For new MSM targets that use rmnet0 port - MSM8994, MSM8916, MSM8936,
    MSM8939, MSM8909, MSM8992, MSM8952 */
#define ATFWD_DATA_TARGET_MSM_NEW     "msm_new"

#define ATFWD_DATA_PROP_SIZE (PROPERTY_VALUE_MAX)
#define ATFWD_ATCOP_PORTS    2


typedef enum {
    INIT_QMI = 0,
    INIT_QMI_SRVC,
    INIT_ATFWD_SRVC,
    INIT_MAX
} atfwd_init_type_t;


/*qmi message library handle*/
int qmiHandle = QMI_INVALID_CLIENT_HANDLE;

/*===========================================================================

                           Global Variables

===========================================================================*/

int userData; //Extra user data sent by QMI
int qmiErrorCode; //Stores the QMI error codes
int userHandle; //Connection ID
int userHandleSMD; //Conn ID for SVLTE II , USB --> SMD --> 8k modem
qmi_atcop_abort_type abortType; //AT command abort type
const char *qmiPort = NULL;
const char *secondaryPort = NULL;
qmi_atcop_at_cmd_fwd_req_type atCmdFwdReqType[] = {
    { //AT command fwd type	// add for QFCT baron 20160328
        1, // Number of commands
        {
            { QMI_ATCOP_AT_CMD_NOT_ABORTABLE, "+QFCT"},
        }
    },
#if defined QUECTEL_QGMR_CMD
	{ //AT command fwd type //add for qgmr by klcib 20180224
        1, // Number of commands
        {
            { QMI_ATCOP_AT_CMD_NOT_ABORTABLE, "+QGMR"},
        }
    },
#endif
#if defined QUECTEL_QAPSUB_CMD
    { //AT command fwd type	// add for QAPSUB by ben,20180710
        1, // Number of commands
        {
            { QMI_ATCOP_AT_CMD_NOT_ABORTABLE, "+QAPSUB"},
        }
    },
#endif
#if defined QUECTEL_QAPCMD_CMD
    { //AT command fwd type	// add for QAPCMD by ben,20180710
        1, // Number of commands
        {
            { QMI_ATCOP_AT_CMD_NOT_ABORTABLE, "+QAPCMD"},
        }
    },
#endif
#if defined QUECTEL_QDEVINFO_CMD
    { //AT command fwd type	// add for QAPCMD by ben,20180710
        1, // Number of commands
        {
            { QMI_ATCOP_AT_CMD_NOT_ABORTABLE, "+QDEVINFO"},
        }
    },
#endif
};

qmi_idl_service_object_type qmi_at_svc_obj;
qmi_client_type             qmi_at_svc_client;
qmi_client_os_params        qmi_at_os_params;
qmi_client_type             qmi_at_notifier;
#define QMI_AT_CLIENT_INIT_TIMEOUT 4
#define MAX_QMI_AT_SVC_CONNECT_ATTEMPTS 2
#define QMI_ATFWD_SYNC_MSG_TIMEOUT 10000
#define QMI_AT_MAX_REQ_MSG_SIZE 512
#define QMI_AT_MAX_RESP_MSG_SIZE 512

//=============================================
// QMI AT RAW message helper functions

extern int qmi_atcop_reg_at_command_fwd_req_msg_helper
(
  qmi_atcop_at_cmd_fwd_req_type     *cmd_fwd_req,
  unsigned char                     *msg,
  int                               *msg_size
);

extern void qmi_atcop_srvc_indication_cb_helper
(
  unsigned long         msg_id,
  unsigned char         *rx_msg_buf,
  int                   rx_msg_len,
  qmi_atcop_indication_data_type    *ind_data
);

extern int qmi_atcop_fwd_at_cmd_resp_helper
(
  qmi_atcop_fwd_resp_at_resp_type   *at_resp,
  int                               *qmi_err_code,
  unsigned char                     *msg,
  int                               *msg_size
);

//=============================================

qmi_atcop_at_cmd_hndl_type commandHandle; //ATFWD request handle
qmi_atcop_at_cmd_fwd_ind_type request; //Input request string

//qmi_atcop_fwd_resp_status_type responseStatus; //ATFWD response status
qmi_atcop_fwd_resp_result_type responseResult; //ATFWD response result
qmi_atcop_fwd_resp_response_type responseType; //ATFWD response type
qmi_atcop_fwd_resp_at_resp_type atCmdResponse; //Actual ATFWD response

AtCmd fwdcmd;
AtCmdResponse fwdresponse;

pthread_cond_t ctrCond; //Condition variable that will be used to indicate if a request has arrived
pthread_mutex_t ctrMutex; //Mutex that will be locked when a request is processed
int newRequest = 0; //New request indication
int regForPrimaryPort = 0; //Re registration indication for primary qmi port
int regForSecondaryPort = 0; //Re registration indication for secondary qmi port
int regForServiceUp = 0;
char target[ATFWD_DATA_PROP_SIZE] = {0}; // Stores target info

typedef struct atfwd_sig_handler_s
{
  int sig;
  __sighandler_t handler;
} atfwd_sig_handler_t;

/* All termination SIGNALS except SIGKILL
 * SIGKILL cannot be handled or ignored
*/
atfwd_sig_handler_t atfwd_sig_handler_tbl[] =
{
  {SIGTERM, NULL},
  {SIGINT, NULL},
  {SIGQUIT, NULL},
  {SIGHUP, NULL}
};

/*===========================================================================
  FUNCTION  parseInput
===========================================================================*/
/*!
@brief
  Parses the input request string and populates the AtCmd struct with the
  data to forward

@return
  None

*/
/*=========================================================================*/
void parseInput(void)
{
    int i;
    fwdcmd.opcode = request.op_code;
    fwdcmd.name = strdup((char *)request.at_name);
    fwdcmd.ntokens = request.num_tokens;
    fwdcmd.tokens = calloc(request.num_tokens, sizeof(char *));
    if(NULL != fwdcmd.tokens) {
        for (i = 0; i < request.num_tokens; i++) {
            fwdcmd.tokens[i] = strdup((char *)request.tokens[i]);
        }
    }
}

/*===========================================================================
  FUNCTION  exitDaemon
===========================================================================*/
/*!
@brief
  Utility method which handles when user presses CTRL+C

@return
  None

@note
  None
*/
/*=========================================================================*/
void exitDaemon(int sig)
{
    printf("Going to kill ATFWD daemon\n");
    (void)sig;
    unsigned int i=0;
    /* Note that the handler should ignore all the reg. signals
     * because they do not want to be interfered
     * while an ongoing signal is being processed
     */
    for(i=0; i<sizeof(atfwd_sig_handler_tbl)/sizeof(atfwd_sig_handler_t); i++) {
        signal(atfwd_sig_handler_tbl[i].sig, SIG_IGN);
    }
    int clientRelease = qmi_atcop_srvc_release_client (userHandle, &qmiErrorCode);
    if (clientRelease < 0) {
        printf("QMI client release error: %d\n", qmiErrorCode);
    }
    if (qmiHandle >= 0) {
        qmi_release(qmiHandle);
    }
    pthread_cond_destroy(&ctrCond);
    pthread_mutex_destroy(&ctrMutex);

    for(i=0; i<sizeof(atfwd_sig_handler_tbl)/sizeof(atfwd_sig_handler_t); i++) {
        printf("\natfwd_sig_handler_tbl[i].sig : %d", atfwd_sig_handler_tbl[i].sig);
        if (atfwd_sig_handler_tbl[i].sig == sig &&
            atfwd_sig_handler_tbl[i].handler != NULL) {
            /* call  default installed handler */
            printf("\ncall default handler [%p] for sig [%d]",
                  atfwd_sig_handler_tbl[i].handler,
                  atfwd_sig_handler_tbl[i].sig);
            (atfwd_sig_handler_tbl[i].handler)(sig);
            break;
        }
    }
    exit(0);
}

/*===========================================================================
  FUNCTION:  signalInit
===========================================================================*/
/*!
    @brief
    Signal specific initialization

    @return
    void
*/
/*=========================================================================*/
void signalInit(void)
{
    unsigned int i=0;
    __sighandler_t temp;

    for(i=0; i<sizeof(atfwd_sig_handler_tbl)/sizeof(atfwd_sig_handler_t); i++) {
        temp = atfwd_sig_handler_tbl[i].handler;
        atfwd_sig_handler_tbl[i].handler = signal(atfwd_sig_handler_tbl[i].sig,
                                                  exitDaemon);
        /* swap previous handler back if signal() was unsuccessful */
        if (SIG_ERR == atfwd_sig_handler_tbl[i].handler) {
            atfwd_sig_handler_tbl[i].handler = temp;
        }
    }
}

/*===========================================================================
  FUNCTION  is_supported_qcci

  From 8996 onwards ATFWD Daemon will work using QCCI. All earlier targets
  use legacy QCCI.

===========================================================================*/
bool is_supported_qcci(void)
{
   return true;
}

/*===========================================================================
  FUNCTION  sendSuccessResponse
===========================================================================*/
/*!
@brief
  Sends OK response to QMI.

@return
  None

@note
  None
*/
/*=========================================================================*/
void sendSuccessResponse(void)
{
    //responseStatus = QMI_ATCOP_SUCCESS;
    responseResult = QMI_ATCOP_RESULT_OK;
    responseType = QMI_ATCOP_RESP_COMPLETE;
    atCmdResponse.at_hndl = commandHandle;
    //atCmdResponse.status = responseStatus;
    atCmdResponse.result = responseResult;
    atCmdResponse.response = responseType;
    atCmdResponse.at_resp = NULL;
    if (qmi_atcop_fwd_at_cmd_resp(userHandle, &atCmdResponse, &qmiErrorCode) < 0) {
        printf("QMI response error: %d\n", qmiErrorCode);
    }
}

/*===========================================================================
  FUNCTION  sendResponse
===========================================================================*/
/*!
@brief
  Sends response to QMI.

@return
  None

@note
  None
*/
/*=========================================================================*/
void sendResponse(AtCmdResponse *response)
{
    unsigned char resp_msg[QMI_AT_MAX_RESP_MSG_SIZE];
    unsigned char qmi_msg_resp_buf[QMI_AT_MAX_RESP_MSG_SIZE];
    int resp_msg_len = 0;
    unsigned int qmi_msg_resp_buf_len = 0;
    qmi_client_error_type qmi_err = QMI_NO_ERR;

    if (!response) {
        printf("Have null response");
        return;
    }
    //responseStatus = QMI_ATCOP_SUCCESS;
    responseResult = response->result;
    responseType = QMI_ATCOP_RESP_COMPLETE;
    atCmdResponse.at_hndl = commandHandle;
    //atCmdResponse.status = responseStatus;
    atCmdResponse.result = responseResult;
    atCmdResponse.response = responseType;
    if ((response->result != QMI_ATCOP_RESULT_OK) && (request.cmee_val == 0)) {
        atCmdResponse.at_resp = NULL;
    } else {
        char *msg = NULL;
        unsigned long s3 = request.s3_val;
        unsigned long s4 = request.s4_val;
        if (response->response && response->response[0]) {
            // Need space for the end of line and carriage return chars (S3/S4)
            size_t l = 4*4 + strlen(response->response) + 1;
            msg = malloc(l);
            if(NULL != msg)
                snprintf(msg, l, "%c%c%s%c%c", (char)s3, \
                         (char)s4, response->response, (char)s3, (char)s4);
        }
        atCmdResponse.at_resp = (unsigned char *)msg;
    }
    if ( !is_supported_qcci() )
    {
        if (qmi_atcop_fwd_at_cmd_resp(userHandle, &atCmdResponse, &qmiErrorCode) < 0) {
            printf("QMI response error: %d\n", qmiErrorCode);
        }
    }
    else
    {
        memset(resp_msg, 0x00, QMI_AT_MAX_RESP_MSG_SIZE);
        memset(qmi_msg_resp_buf, 0x00, QMI_AT_MAX_RESP_MSG_SIZE);
        qmi_atcop_fwd_at_cmd_resp_helper(&atCmdResponse, &qmiErrorCode, resp_msg, &resp_msg_len);

        printf("sending QMI_AT_FWD_RESP_AT_CMD_RESP_V01 message\n");
        qmi_err = qmi_client_send_raw_msg_sync(qmi_at_svc_client,
                            QMI_AT_FWD_RESP_AT_CMD_RESP_V01,
                            resp_msg,
                            sizeof(resp_msg)-resp_msg_len,
                            qmi_msg_resp_buf,
                            sizeof(qmi_msg_resp_buf),
                            &qmi_msg_resp_buf_len,
                            QMI_ATFWD_SYNC_MSG_TIMEOUT);

        printf("qmi_client_send_raw_msg_sync returned: %d\n", qmi_err);
    }
    if(NULL != atCmdResponse.at_resp) {
        free(atCmdResponse.at_resp);
        atCmdResponse.at_resp = NULL;
    }
}

/*===========================================================================
  FUNCTION  sendInvalidCommandResponse
===========================================================================*/
/*!
@brief
  Sends ERROR response to QMI.

@return
  None

@note
  None
*/
/*=========================================================================*/
void sendInvalidCommandResponse(void)
{
    unsigned char resp_msg[QMI_AT_MAX_RESP_MSG_SIZE];
    unsigned char qmi_msg_resp_buf[QMI_AT_MAX_RESP_MSG_SIZE];
    int resp_msg_len = 0;
    unsigned int qmi_msg_resp_buf_len =0;
    qmi_client_error_type qmi_err = QMI_NO_ERR;
    responseResult = QMI_ATCOP_RESULT_ERROR;
    responseType   = QMI_ATCOP_RESP_COMPLETE;

    atCmdResponse.at_hndl  = commandHandle;
    atCmdResponse.result   = responseResult;
    atCmdResponse.response = responseType;

    if (request.cmee_val == 0) {
        atCmdResponse.at_resp = NULL;
    } else {
        char *response;
        char s3Val[MAX_DIGITS];
        char s4Val[MAX_DIGITS];

        snprintf(s3Val, MAX_DIGITS, "%c", (char) request.s3_val);
        snprintf(s4Val, MAX_DIGITS, "%c", (char) request.s4_val);

        size_t respLen  = ((strlen(s3Val) * 2) + (strlen(s4Val) * 2) + 13 + 1) * sizeof(char);
        response = (char *)malloc(respLen);
        if (!response) {
            printf("No memory for generating invalid command response\n");
            atCmdResponse.at_resp = NULL;
        } else {
            snprintf(response, respLen, "%s%s+CME ERROR :2%s%s", s3Val, s4Val, s3Val, s3Val);
            atCmdResponse.at_resp = (unsigned char *)response;
        }
    }

    if (!is_supported_qcci() )
    {
        if (qmi_atcop_fwd_at_cmd_resp(userHandle, &atCmdResponse, &qmiErrorCode) < 0) {
            printf("QMI response error: %d\n", qmiErrorCode);
        }
    }
    else
    {
        memset(resp_msg, 0x00, QMI_AT_MAX_RESP_MSG_SIZE);
        memset(qmi_msg_resp_buf, 0x00, QMI_AT_MAX_RESP_MSG_SIZE);
        qmi_atcop_fwd_at_cmd_resp_helper(&atCmdResponse, &qmiErrorCode, resp_msg, &resp_msg_len);

        printf("sending QMI_AT_FWD_RESP_AT_CMD_RESP_V01 message\n");
        qmi_err = qmi_client_send_raw_msg_sync(qmi_at_svc_client,
                            QMI_AT_FWD_RESP_AT_CMD_RESP_V01,
                            resp_msg,
                            sizeof(resp_msg)-resp_msg_len,
                            qmi_msg_resp_buf,
                            sizeof(qmi_msg_resp_buf),
                            &qmi_msg_resp_buf_len,
                            QMI_ATFWD_SYNC_MSG_TIMEOUT);
        printf("qmi_client_send_raw_msg_sync returned: %d\n", qmi_err);
    }

    if (atCmdResponse.at_resp) {
        free(atCmdResponse.at_resp);
        atCmdResponse.at_resp = NULL;
    }
}

/*===========================================================================
  FUNCTION  sendCommand
===========================================================================*/
/*!
@brief
  Routine that will be invoked by QMI upon a request for AT command. It
  checks for the validity of the request and finally spawns a new thread to
  process the key press events.

@return
  None

@note
  None

*/
/*=========================================================================*/
static void atCommandCb(int userHandle, qmi_service_id_type serviceID,
                             void *userData, qmi_atcop_indication_id_type indicationID,
                             qmi_atcop_indication_data_type  *indicationData)
{
    printf("atCommandCb\n");
  //  QCRIL_NOTUSED(userHandle);
  //  QCRIL_NOTUSED(serviceID);
  //  QCRIL_NOTUSED(userData);

    /* Check if it's an abort request */
    if (indicationID == QMI_ATCOP_SRVC_ABORT_MSG_IND_TYPE) {
        printf("Received abort message from QMI\n");
    } else if (indicationID == QMI_ATCOP_SRVC_AT_FWD_MSG_IND_TYPE) {
        printf("Received AT command forward request\n");
        pthread_mutex_lock(&ctrMutex);
        commandHandle = indicationData->at_hndl;
        request = indicationData->at_cmd_fwd_type;
        parseInput();
        newRequest = 1;
        pthread_cond_signal(&ctrCond);
        pthread_mutex_unlock(&ctrMutex);
    }
}

 /* char* getTargetFromSysProperty()
{
    int ret = 0;
    struct dev_info devInfo;
    char msm_tgt[ATFWD_DATA_PROP_SIZE] = {0};

    memset(&devInfo, 0, sizeof(devInfo));
    
    ret = property_get(ATFWD_DATA_PROP_BASEBAND, target, "");

    
    if (!strcmp(ATFWD_DATA_TARGET_MDM, target)
            || !strcmp(ATFWD_DATA_TARGET_MDM2, target)) {
        ret = get_system_info(&devInfo);
        if (ret != 0) {
            printf("Could not retrieve ESOC info. get_system_info returns (%d)\n", ret);
        } else {
            printf("link name %s ", devInfo.mdm_list[0].mdm_link);
            if (!strcmp("HSIC", devInfo.mdm_list[0].mdm_link)) {
                strlcpy(target, ATFWD_DATA_TARGET_MDM, ATFWD_DATA_PROP_SIZE);
            } else {
                strlcpy(target, ATFWD_DATA_TARGET_MDM2, ATFWD_DATA_PROP_SIZE);
            }
        }
    } else if (!strcmp(ATFWD_DATA_TARGET_MSM, target)) {
       
        ret = property_get(ATFWD_DATA_PROP_BOARD_PLATFORM, msm_tgt, "");
        if (!strcmp(ATFWD_DATA_TARGET_MSM8994, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8992, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8916, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8936, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8939, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8909, msm_tgt) ||
                !strcmp(ATFWD_DATA_TARGET_MSM8952, msm_tgt)) {
            strlcpy(target, ATFWD_DATA_TARGET_MSM_NEW, ATFWD_DATA_PROP_SIZE);
        }
    }

    return target;
}   */

char* getDefaultPort(void)
{
    char *defaultQmiPort = NULL;

    if (!strncmp(ATFWD_DATA_TARGET_MSM, target,strlen(target)) ||
        !strncmp(ATFWD_DATA_TARGET_SGLTE, target, strlen(target))) {
        /* use smd port */
        defaultQmiPort = QMI_PORT_RMNET_1;
    } else if (!strncmp(ATFWD_DATA_TARGET_MSM_NEW, target, strlen(target))) {
        /* 8994, 8992, 8916, 8936, 8939, 8909 and 8952 would use first conn id */
        defaultQmiPort = QMI_PORT_RMNET_0;
    } else if (!strncmp(ATFWD_DATA_TARGET_SVLTE1, target, strlen(target)) ||
               !strncmp(ATFWD_DATA_TARGET_SVLTE2A, target, strlen(target)) ||
               !strncmp(ATFWD_DATA_TARGET_CSFB, target, strlen(target))) {
        /* use sdio port */
        defaultQmiPort = QMI_PORT_RMNET_SDIO_0;
    } else if (!strncmp(ATFWD_DATA_TARGET_MDM, target, strlen(target))) {
        /* use usb port */
        defaultQmiPort = QMI_PORT_RMNET_USB_0;
    } else if (!strncmp(ATFWD_DATA_TARGET_MDM2, target, strlen(target))) {
        /* use MHI port for fusion 4.5 devices */
        defaultQmiPort = QMI_PORT_RMNET_MHI_0;
    } else {
        /* do not set defaultQmiPort for any thing else right now
         * as we don't know */
        printf("defaultQmiPort left as-is to %s", defaultQmiPort);
    }

    return defaultQmiPort;
}

void initAtCopServiceByPort(const char *port, int *handle) {
    int retryCnt;
    for (retryCnt = 1; retryCnt <= ATFWD_MAX_RETRY_ATTEMPTS; ++retryCnt) {
        *handle = qmi_atcop_srvc_init_client(port, atCommandCb, NULL , &qmiErrorCode);
        printf("qmi_atcop_srvc_init_client - QMI Err code %d , handle %d", qmiErrorCode, *handle);
        if (*handle < 0 || qmiErrorCode != 0) {
            printf("Could not register with the QMI Interface on port %s. " \
                    "The QMI error code is %d", port, qmiErrorCode);
            // retry after yielding..
            sleep(retryCnt * ATFWD_RETRY_DELAY);
            continue;
        }
        break;
    }
    return;
}

void stopSelf(void) {
    printf("Stop the daemon....");
   // property_set("radio.atfwd.start", "false");
    return;
}

/*===========================================================================
  FUNCTION  registerATCommands

  Register all AT commands with the QMI AT service.
  Note that client initialization is already done as part of qmi_client_init_
  instance.

===========================================================================*/
bool registerATCommands(void)
{
    int i=0;
    int nCommands=0;
    int num_errors=0;
    qmi_client_error_type qmi_err = QMI_NO_ERR;
    unsigned char req_msg[QMI_AT_MAX_REQ_MSG_SIZE];
    unsigned char resp_msg[QMI_AT_MAX_RESP_MSG_SIZE];
    unsigned int actual_resp_len = 0;
    int req_msg_len = 0;

    nCommands = sizeof(atCmdFwdReqType) / sizeof(atCmdFwdReqType[0]);
    printf("Trying to register %d commands:\n", nCommands);

    for ( i=0; i< nCommands; i++)
    {
        printf("cmd%d: %s\n", i, atCmdFwdReqType[i].qmi_atcop_at_cmd_fwd_req_type[0].at_cmd_name);

        memset( req_msg , 0x00, QMI_AT_MAX_REQ_MSG_SIZE);
        memset( resp_msg, 0x00, QMI_AT_MAX_RESP_MSG_SIZE);

        qmi_atcop_reg_at_command_fwd_req_msg_helper(&atCmdFwdReqType[i], req_msg, &req_msg_len);

        printf("sending QMI_AT_REG_AT_CMD_FWD_REQ_V01 message\n");
        qmi_err = qmi_client_send_raw_msg_sync(qmi_at_svc_client,
                                    QMI_AT_REG_AT_CMD_FWD_REQ_V01,
                                    req_msg,
                                    sizeof(req_msg)-req_msg_len,
                                    resp_msg,
                                    sizeof(resp_msg),
                                    &actual_resp_len,
                                    QMI_ATFWD_SYNC_MSG_TIMEOUT);

        if ( qmi_err != QMI_NO_ERR)
        {
            printf("Could not register AT command : %s with the QMI Interface - Err code:%d\n",
                     atCmdFwdReqType[i].qmi_atcop_at_cmd_fwd_req_type[0].at_cmd_name, qmi_err);
            num_errors++;
            qmi_err = QMI_NO_ERR;
        }
    }

    if ( num_errors == nCommands )
    {
        printf("AT commands registration failure... Release client handle.");
        qmi_client_release(qmi_at_svc_client);
        return false;
    }

    printf("Registered AT Commands event handler\n");
    return true;
}

void initAtcopServiceAndRegisterCommands(const char *port, int *handle) {
    int i, nErrorCnt, nCommands;
    i = nErrorCnt = 0;

    initAtCopServiceByPort(port, handle);

    if (*handle > 0) {
        nCommands = sizeof(atCmdFwdReqType) / sizeof(atCmdFwdReqType[0]);
        printf("Trying to register %d commands:\n", nCommands);
        for (i = 0; i < nCommands ; i++) {
            printf("cmd%d: %s\n", i, atCmdFwdReqType[i].qmi_atcop_at_cmd_fwd_req_type[0].at_cmd_name);

            qmiErrorCode = 0;
            int registrationStatus = qmi_atcop_reg_at_command_fwd_req(*handle, \
                                              &atCmdFwdReqType[i], &qmiErrorCode);
            printf("qmi_atcop_reg_at_command_fwd_req: %d", qmiErrorCode);
            if (registrationStatus < 0 || qmiErrorCode != 0) {
                printf("Could not register AT command : %s with the QMI Interface - Err code:%d\n",
                     atCmdFwdReqType[i].qmi_atcop_at_cmd_fwd_req_type[0].at_cmd_name, qmiErrorCode);
                nErrorCnt++;
                qmiErrorCode = 0;
            }
        }

        if(nErrorCnt == nCommands) {
            printf("AT commands registration failure... Release client handle: %d\n", *handle);
            qmi_atcop_srvc_release_client(*handle, &qmiErrorCode);
            *handle = -1;
            return;
        }
    } else {
        printf("ATcop Service Init failed\n");
        return;
    }

    printf("Registered AT Commands event handler\n");
    return;
}

/*===========================================================================
  FUNCTION  at_fwd_sys_up_handler

  Call back to be registered with qmi_client_notifier_init .
  When service is available back on the modem, initiate a new client
  registration.

===========================================================================*/
/* static void at_fwd_sys_up_handler
(
 qmi_client_type                user_handle,
 qmi_idl_service_object_type    service_obj,
 qmi_client_notify_event_type   service_event,
 void                           *notify_cb_data
)
{
   // QCRIL_NOTUSED(user_handle);
   // QCRIL_NOTUSED(service_obj);
   // QCRIL_NOTUSED(notify_cb_data);
    printf("Recvd QMI AT notification. service_event: %d", (int) service_event);
    switch ( service_event)
    {
    case QMI_CLIENT_SERVICE_COUNT_INC:
        printf("QMI AT service (%d) is back up.", (int) service_event);

        // There is only one port when using QMI_CLIENT_INIT_INSTANCE_ANY
        // reusing regForPrimaryPort for signalling.
        pthread_mutex_lock(&ctrMutex);
        regForPrimaryPort = 1;
        pthread_cond_signal(&ctrCond);
        pthread_mutex_unlock(&ctrMutex);
        printf("regForPrimaryPort signalled.");
        break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
        printf("QMI AT service (%d) is down.", (int) service_event);
        //no need to release again. QCCI error_cb should have provided
        //this info already.
        break;
    default:
        printf("Recvd unsupported event (%d) from QCCI notifier.", (int) service_event);
        break;
    }
} */

static void at_fwd_sys_up_handler
(
 qmi_client_type                user_handle,
 qmi_idl_service_object_type    service_obj,
 qmi_client_notify_event_type   service_event,
 void                           *notify_cb_data
)
{
    LOGI("Recvd QMI AT notification. service_event: %d", (int) service_event);
    switch ( service_event)
    {
    case QMI_CLIENT_SERVICE_COUNT_INC:
        LOGI("QMI AT service is back up: %d", (int) service_event);
        // There is only one port when using QMI_CLIENT_INIT_INSTANCE_ANY
        // reusing regForPrimaryPort for signalling.
        pthread_mutex_lock(&ctrMutex);
        regForPrimaryPort = 1;
        pthread_cond_signal(&ctrCond);
        pthread_mutex_unlock(&ctrMutex);
        LOGI("regForPrimaryPort signalled.");
        break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
        LOGI("QMI AT service is down: %d", (int) service_event);
        //no need to release again. QCCI error_cb should have provided
        //this info already.
        break;
    default:
        LOGI("Recvd unsupported event from QCCI notifier: %d", (int) service_event);
        break;
    }
}

/*===========================================================================
  FUNCTION  at_fwd_sys_down_evt_handlers

  Call back to be registered with qmi_client_register_error_cb.
  When service terminates on the modem, release qmi_client.

===========================================================================*/
/* static void at_fwd_sys_down_evt_handler
(
  qmi_client_type       clnt,
  qmi_client_error_type error,
  void                 *error_cb_data
)
{
  QCRIL_NOTUSED(clnt);
  QCRIL_NOTUSED(error_cb_data);

  printf("ATFWD sys down event recvd with error: %d", error);

  pthread_mutex_lock(&ctrMutex);
  regForServiceUp = 1;
  pthread_cond_signal(&ctrCond);
  pthread_mutex_unlock(&ctrMutex);
  printf("regForServiceUp signalled.");
} */

static void at_fwd_sys_down_evt_handler
(
  qmi_client_type       clnt,
  qmi_client_error_type error,
  void                 *error_cb_data
)
{
  int srv_id = (intptr_t) error_cb_data;
  LOGI("ATFWD sys down event recvd. srv_id:%d", srv_id);
  pthread_mutex_lock(&ctrMutex);
  regForServiceUp = 1;
  pthread_cond_signal(&ctrCond);
  pthread_mutex_unlock(&ctrMutex);
  LOGI("regForServiceUp signalled.");
}

/*===========================================================================
  FUNCTION  qmi_at_unsol_ind_cb

  New indication callback to be provided with qmi_client_init_instance.
  Note. This function uses helper functions from legacy qcci to process
  raw indication data. Once AT QMI qcci service files are ready, use
  decode function instead.

===========================================================================*/
void qmi_at_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
   // QCRIL_NOTUSED(user_handle);
   // QCRIL_NOTUSED(ind_cb_data);
    qmi_atcop_indication_data_type ind_data;
    printf("qmi_at_unsol_ind_cb: ind id:%d \n", msg_id);

    switch ( msg_id )
    {
    case QMI_AT_ABORT_AT_CMD_IND_V01:
        printf("Received abort message from QMI\n"); //nothing to do
        break;
    case QMI_AT_FWD_AT_CMD_IND_V01:
        printf("Received AT command forward request from modem\n");
        pthread_mutex_lock(&ctrMutex);
        qmi_atcop_srvc_indication_cb_helper (
            msg_id,
            ind_buf,
            ind_buf_len,
            &ind_data
            );
        commandHandle = ind_data.at_hndl;
        request = ind_data.at_cmd_fwd_type;
        parseInput();
        newRequest = 1;
        pthread_cond_signal(&ctrCond);
        pthread_mutex_unlock(&ctrMutex);
        printf("ctrCond signalling complete.\n");
        break;
    }
}

static void atfwdSysEventHandler (qmi_sys_event_type event_id,
        const qmi_sys_event_info_type *event_info, void *user_data) {

    //QCRIL_NOTUSED(user_data);
    int ret;
    const char* devId;

    if (event_info && event_info->qmi_modem_service_ind.dev_id) {
        devId = event_info->qmi_modem_service_ind.dev_id;
        printf("Received sys_event: %d from QMI devId: %s\n", (int) event_id, devId);
        if ((qmiPort && !strcmp(qmiPort, devId))
                || (secondaryPort && !strcmp(secondaryPort, devId))) {
            switch (event_id) {
                case QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND:
                    printf("Modem Out Of Service --> Release ATCOP service client handles, if any\n");
                    if (qmiPort && !strcmp(qmiPort, devId) && (userHandle > 0)) {
                        qmiErrorCode = 0;
                        ret = qmi_atcop_srvc_release_client(userHandle, &qmiErrorCode);
                        printf("release_client returns %d, qmiErroCode = %d for qmiPort: %s & "
                                "userHandle: %d\n", ret, qmiErrorCode, qmiPort, userHandle);
                        userHandle = -1;
                    }
                    if (secondaryPort && !strcmp(secondaryPort, devId) && (userHandleSMD > 0)) {
                        qmiErrorCode = 0;
                        ret = qmi_atcop_srvc_release_client(userHandleSMD, &qmiErrorCode);
                        printf("release_client returns %d, qmiErroCode = %d for secondaryPort: %s & "
                            "userHandleSMD: %d\n", ret, qmiErrorCode, secondaryPort, userHandleSMD);
                        userHandleSMD = -1;
                    }
                    break;

                case QMI_SYS_EVENT_MODEM_IN_SERVICE_IND:
                    printf("Modem In Service --> Notify Main loop to init ATCOP service client\n");
                    if (qmiPort && !strcmp(qmiPort, devId)) {
                        pthread_mutex_lock(&ctrMutex);
                        regForPrimaryPort = 1;
                        pthread_cond_signal(&ctrCond);
                        pthread_mutex_unlock(&ctrMutex);
                    }
                    if (secondaryPort && !strcmp(secondaryPort, devId)) {
                        pthread_mutex_lock(&ctrMutex);
                        regForSecondaryPort = 1;
                        pthread_cond_signal(&ctrCond);
                        pthread_mutex_unlock(&ctrMutex);
                    }

                    break;

                default:
                    printf("Unhandled SYS event (%d) from QMI....\n", (int) event_id);
                    break;
            }
        }
    } else {
        printf("Ignoring sys_event: %d from QMI with NO event_info/dev_id\n", (int) event_id);
    }
}

void tryInit (atfwd_init_type_t type, int *result) {
    int retryCnt = 1;
    qmi_client_error_type client_err = QMI_NO_ERR;
    int num_retries = 0;

    for (; retryCnt <= ATFWD_MAX_RETRY_ATTEMPTS; retryCnt++) {
        printf("\n");
        printf("\n TrtyInit: retryCnt: %d",retryCnt );
        qmiErrorCode = 0;
        switch (type) {
        case INIT_QMI:
            if (!is_supported_qcci())
            {
                qmiHandle = qmi_init(atfwdSysEventHandler, NULL);
                *result = qmiHandle;
            }
            else
            {
                printf("Using QCCI. Skipping qmi_init");
                *result=1; //to support backwards compatibility
            }
            break;
        case INIT_QMI_SRVC:
            if ( !is_supported_qcci())
            {
                *result = qmi_connection_init(qmiPort, &qmiErrorCode);
            }
            else
            {
                 do {
                     if ( num_retries != 0) {
                         sleep(1);
                         printf("qmi_client_init_instance status retry : %d", num_retries);
                     }

                     printf("\n qmi_client_init_instance....");
                     client_err = qmi_client_init_instance(qmi_at_svc_obj,
                                                   QMI_CLIENT_INSTANCE_ANY,
                                                   qmi_at_unsol_ind_cb,
                                                   NULL,
                                                   &qmi_at_os_params,
                                                   (int) QMI_AT_CLIENT_INIT_TIMEOUT,
                                                   &qmi_at_svc_client);
                     num_retries++;
                 } while ( (client_err != QMI_NO_ERR) && (num_retries < MAX_QMI_AT_SVC_CONNECT_ATTEMPTS) );

                 printf("qmi_client_init_instance status: %d, num_retries: %d, retryCnt: %d",
                      (int) client_err, num_retries, retryCnt);

                 // Only qmi_client_init_instance failure will be treated as catastrophic
                 // failure of error_cb and notifier funtions will result in ATFdeamon
                 // not working after SSR.
                 *result = client_err;

                 if ( client_err == QMI_NO_ERR )
                 {
                     client_err = qmi_client_register_error_cb(qmi_at_svc_client,
                                                               at_fwd_sys_down_evt_handler,
                                                               NULL);
                     printf("qmi_client_register_error_cb status: %d", (int) client_err);
                     qmi_client_release(qmi_at_notifier);
                     qmi_at_notifier = NULL;
                 }
                }//end qcci client init
                break;
            case INIT_ATFWD_SRVC:
                //*result = initializeAtFwdService();
		*result = 0;
                break;
            default:
                printf("Invalid type %d", type);
                return;
        }
        printf("result : %d \t ,Init step :%d \t ,qmiErrorCode: %d", *result, type, qmiErrorCode);
        if (*result >= 0 && qmiErrorCode == 0) {
            break;
        }
        printf("\n Sleeping...");
        sleep(retryCnt * ATFWD_RETRY_DELAY);
        printf("\n Out of sleep...");
    }
    printf("\n Back to main.");

    return;
}

int isNewCommandAvailable(void) {
    if (newRequest || regForPrimaryPort || regForSecondaryPort || regForServiceUp ) {
        return 1;
    }
    return 0;
}

void freeAtCmdResponse(AtCmdResponse *response) {
    if (!response) return;
        if (response->response)
            free(response->response);
    free(response);
}

/*=========================================================================
  FUNCTION:  regForServiceUpEvent

===========================================================================*/
/*!
@brief
  Register for service up events.

*/
/*=========================================================================*/
void regForServiceUpEvent(void)
{
    qmi_client_error_type client_err = QMI_NO_ERR;

    // as service down received, release existing client handle.
    printf("Release QMI AT client");
    qmi_client_release(qmi_at_svc_client);
    qmi_at_svc_client = NULL;

    // Register for service up.
    if ( qmi_at_notifier == NULL )
    {
        client_err =  qmi_client_notifier_init(qmi_at_svc_obj,
                                               &qmi_at_os_params,
                                               &qmi_at_notifier
                                               );
        printf("qmi_client_notifier_init status: %d", (int) client_err);

        client_err = qmi_client_register_notify_cb(qmi_at_notifier,
                                                   at_fwd_sys_up_handler,
                                                   NULL);

        printf("qmi_client_register_notify_cb status: %d", (int) client_err);
    }
    regForServiceUp = 0;
}

#ifdef QUECTEL_FCT_TEST

#define ARRARY_SIZE(a) (sizeof(a)/sizeof(a[0]))

//#define QUECTEL_FCT_TEST_DEBUG // for print debug log info
#ifdef QUECTEL_FCT_TEST_DEBUG
#define DGprintf(fmt,args...) printf(fmt,##args) 
#else
#define DGprintf(fmt, args...) 
#endif 
typedef struct{
	const char *name; // item name
	int result; // -1: fail  0: none  1: success
}fct_item_type;
fct_item_type fct_items_all[]=
//fct_item_type fct_items[]=
{
        {"FLASHLIGHT",0},
	{"KEY", 0},
        {"VIBRATOR",0},
	{"HANDSET PLAY", 0},
	{"CAMERA MAIN", 0},
	{"HEADSET LOOP", 0},
	{"CAMERA FRONT",0},
	{"SPEAKER LOOP", 0},
        {"LIGHT SENSOR",0},
	{"SDCARD", 0},
	{"EMMC", 0},
        {"SIMCARD1",0},
        {"SIMCARD2",0},
	{"WIFI", 0},
	{"BLUETOOTH", 0},
        {"GPS",0},
};

 char* set_response_buf(fct_item_type *fct_items, int num)
{
	int i,offset=0;
	int test_num=0, success_num=0,fail_num=0,null_num=0;
	char *resp_buf=NULL;
	if(NULL == resp_buf)
	{
		resp_buf = (char *)malloc(RESP_BUF_SIZE);
		if(resp_buf == NULL)
		{
			LOGI("%s:%d No Memory\n", __func__, __LINE__);
			return resp_buf; // error
		}
		memset(resp_buf, 0, RESP_BUF_SIZE);
	}
	for(i=0; i<num; i++)
	{
		if(fct_items[i].result !=0 )
			test_num ++;
		if(fct_items[i].result == 1)
			success_num ++;
		if(fct_items[i].result == 0)
			null_num ++;
		if(fct_items[i].result == -1)
			fail_num ++;
	}
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "+QFCT: %d,%d,%d",(success_num==num)?1:0, num, test_num );
	if((success_num==num) || (test_num == 0))
	{
		return resp_buf; // all fct items pass or not test
	}
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\r\n");
	
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "Success_num = %d,Fail_num = %d,Null_num = %d",success_num, fail_num, null_num );
	
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "\r\n");
	offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "%d:%s,",num-1,fct_items[num-1].name);
	switch(fct_items[num-1].result)
	{
		case -1:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "fail");
			break;
		case 0:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "null");
			break;
			
		case 1:
			offset += snprintf(resp_buf+offset, (RESP_BUF_SIZE-offset), "pass");
			break;
	}

	LOGI("%s:%d: RESP_BUF_SIZE:%d offset:%d \n", __FILE__, __LINE__,RESP_BUF_SIZE, offset);
	//printf("%s:%d: RESP_BUF_SIZE:%d offset:%d \n", __FILE__, __LINE__,RESP_BUF_SIZE, offset);
#if  1// for debug
//	DGprintf("<<<<<<<<<<<< respbuf >>>>>>>>>>>>>>\n");
//	DGprintf("%s", resp_buf);
//	DGprintf("<<<<<<<<<<<< respbuf end >>>>>>>>>>>>>>\n");
#endif

	return resp_buf;
}
char* get_string_from_two_char(char *src, char *dest, int size, char start, char end)
{
	char *p=NULL;
	char *q=NULL;
	int i=0;
	if(NULL==src || NULL==dest)
		return NULL;
	memset(dest, 0, size);
	if((p=strchr(src,start)) && (q=strchr(src,end)))
	{
		p++; // skip start char, from next char
		for(i=0;i<size&&p!=q;i++)
		{
			*dest++=*p;
			p++;
			
		}
	}
	else
	{
		return NULL;
	}
	return dest;
	
}

int read_file(const char *filepath, char *buf, int size){
    int fd, len;

    fd = open(filepath, O_RDONLY);
    if(fd == -1){
        LOGI("[%s]:file(%s) open fail, error=%s\n", __FUNCTION__, filepath, strerror(errno));
        return -1;
    }

    len = read(fd, buf, size - 1);
    if(len > 0){
        if(buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else
            buf[len] = '\0';
    }

    close(fd);
    return 0;
}

void quec_qfct_handle(AtCmdResponse *response)
{
	#define FCT_RESULT_FILE	 "/data/FTM_AP/mmi.res"
	int total_items;
	char line_text[64] = {0};
	FILE *fp = NULL;
	char mmi_res_name[32] = {0};
	int  i,offset=-1;
    int items_length = ARRARY_SIZE(fct_items_all);
	LOGI("quec_qfct_handle start\n");
    total_items = items_length;
    fct_item_type fct_items[total_items];        
      
    for(i=0; i<total_items; i++)
    {
        fct_items[i].name = fct_items_all[i].name;
        fct_items[i].result = 0;
    }
    
	if((fp=fopen(FCT_RESULT_FILE, "r")) == NULL)
	{
		LOGI("open file:%s failed!\n", FCT_RESULT_FILE);
		
		if((response->response = set_response_buf(fct_items, total_items)) == NULL )
		{
			response->result = 0; // error
			LOGI("%s:%d open file %s failed!\n", __func__, __LINE__, FCT_RESULT_FILE);
		}
		response->result = 1;
		return;
	}
	// get line from file
	while(fgets(line_text, 64, fp)!=NULL)	
	{
	LOGI("Line: %s \n",line_text);
		if(strchr(line_text, '[') && strchr(line_text, ']')) // [name]
		{
			offset = -1;
			if(get_string_from_two_char(line_text, mmi_res_name, 32, '[',']') == NULL)
			{
				LOGI("error!\n");
				continue;
			}
			else
			{
				LOGI("get name:%s\n", mmi_res_name);
			}
			for(i=0; i<total_items; i++)
			{
				LOGI("fct_item.name:%s\n",fct_items[i].name);
				if(strcasecmp(mmi_res_name, fct_items[i].name)==0)
				{
					LOGI("Match\n");
					offset = i;
					break;
				}
				else
				{
					LOGI("Not Match.\n");
				}
			}
			
		}
		else if(strstr(line_text, "Result"))  // item result
		{
			if(offset>=0)
			{
				if(strstr(line_text, "pass"))
					fct_items[offset].result = 1;
				else if(strstr(line_text, "fail"))
					fct_items[offset].result = -1;
			}
		}
	}
	if(fp)
		fclose(fp);
	
	if((response->response = set_response_buf(fct_items, ARRARY_SIZE(fct_items))) == NULL)
	{
		response->result = 0;
		return;
	}
	response->result = 1; // success
	return;
	
}

#endif

/*===========================================================================
  the tool of process operation
===========================================================================*/
pid_t getProcessPidByName(const char *proc_name)
{
    FILE *fp;
    char buf[100];
    char cmd[200] = {'\0'};
    pid_t pid = -1;
    sprintf(cmd, "pidof %s", proc_name);
    if((fp = popen(cmd, "r")) != NULL)
    {
        if(fgets(buf, 255, fp) != NULL)
        {
            pid = atoi(buf);
        }
    }
    printf("pid = %d \n", pid);
    pclose(fp);
    return pid;
}

bool is_proc_exist(int pid)
{
    return kill(pid, 0) == 0;
}

void kill_proc(int pid) 
{
    int stat;
    if(pid > 0 && is_proc_exist(pid))
   {
        kill(pid, SIGKILL);
        waitpid(pid, &stat, 0);
        if(WIFSIGNALED(stat))
            printf("Child process(pid=%d) received signal(%d) and exit\n", pid, WTERMSIG(stat));

    }
}

/*===========================================================================
  start and stop wifi 
===========================================================================*/
void start_wifi()
{
  system("ptt_socket_app -v -d -f &");
}

void stop_wifi()
{
    int pid=-1;
    pid=getProcessPidByName("ptt_socket_app");
    kill_proc(pid);
}

/*===========================================================================
  start and stop bluetooth
===========================================================================*/
void start_bluetooth()
{
  system("ftmdaemon -n &");
}

void stop_bluetooth()
{
    int pid=-1;
    pid=getProcessPidByName("ftmdaemon");
    kill_proc(pid);
}

/*===========================================================================
  FUNCTION  sendit
===========================================================================*/
/*!
@brief
     Invokes a Remote Procedure Call (RPC) to Android's Window Manager Service
     Window Manager service returns 0 if the call is successful
@return
  Returns 1 if the key press operation was successful; 0 otherwise

@note
  None.
*/
/*=========================================================================*/
AtCmdResponse *sendit(const AtCmd *cmd)
{
   	printf("\nATFWD: cmd->name : %s\n",cmd->name);
    AtCmdResponse *result;
	result = malloc(RESP_BUF_SIZE);
    result->response = malloc(256 * sizeof(char));
    if (!cmd) return NULL;
#ifdef QUECTEL_QGMR_CMD                            //
		if (strcasecmp(cmd->name, "+QGMR") == 0) {
			quec_qgmr_handle(cmd, result);      
		}
#endif
#ifdef QUECTEL_QAPSUB_CMD
		if (strcasecmp(cmd->name, "+QAPSUB") == 0) {
			quec_qapsub_handle(cmd, result);
		}
#endif
#ifdef QUECTEL_QAPCMD_CMD
		if (strcasecmp(cmd->name, "+QAPCMD") == 0) {
			quec_qapcmd_handle(cmd, result);
		}
#endif
#ifdef QUECTEL_QDEVINFO_CMD
		if (strcasecmp(cmd->name, "+QDEVINFO") == 0) {
			quec_qdevinfo_handle(cmd, result);
		}
#endif
#ifdef QUECTEL_FCT_TEST
		if (strcasecmp(cmd->name, "+QFCT") == 0) 
        {
            if(NULL != cmd->tokens)
		    {   
                if(cmd->ntokens == 0 || cmd->tokens[0] == NULL)
                {
                    LOGI("ATFWD AtCmdFwd Tokens[0] is NULL");
                    quec_qfct_handle(result);
                }
                else if(0 == strncmp("wifi-start",cmd->tokens[0],strlen("wifi-start")))
                {
                    printf("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
                // property_set("wifi.p_socket_app", "true");
                    start_wifi();
                    result->result = 1; // success
                }
                else if(0 == strncmp("wifi-end",cmd->tokens[0],strlen("wifi-end")))
                {
                    printf("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
                //  property_set("wifi.p_socket_app", "false");
                    stop_wifi();
                    result->result = 1; // success
                }
                else if(0 == strncmp("ble-start",cmd->tokens[0],strlen("ble-start")))
                {	
                    printf("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
                // property_set("bt.start", "true");
                    start_bluetooth();
                    result->result = 1; // success
                }else if(0 == strncmp("ble-end",cmd->tokens[0],strlen("ble-end")))
                {
                    printf("ATFWD AtCmdFwd:%s",cmd->tokens[0]);
                //  property_set("bt.start", "false");
                    stop_bluetooth();
                    result->result = 1; // success
                }
                else
                {
                    printf("ATFWD AtCmdFwd QFCT");
                    quec_qfct_handle(result);
                }	
            }
            else
            {
                LOGI("ATFWD AtCmdFwd Tokens is NULL");
                quec_qfct_handle(result);		
            }	
		}
#else
    result = gAtCmdFwdService->processCommand(*cmd);
#endif

    return result;
}


/*=========================================================================
  FUNCTION:  main

===========================================================================*/
/*!
@brief
  Initialize the QMI connection and register the ATFWD event listener.
  argv[1] if provided, gives the name of the qmi port to open.
  Default is "rmnet_sdio0".

*/
/*=========================================================================*/
int main (int argc, char **argv)
{
    AtCmdResponse *response;
    int i, connectionResult, initType;

    userHandle = userHandleSMD = -1;
    i = connectionResult = 0;

    printf("*** Starting ATFWD-daemon *** \n");
 //   (void) getTargetFromSysProperty();

    if ( !is_supported_qcci() )
    {
        if (!strncmp(ATFWD_DATA_TARGET_APQ, target,
                     strlen(target))) {
            printf("APQ baseband : Explicitly stopping ATFWD service....\n");
            stopSelf();
            return -1;
        }

        if (argc >= 2) {
            qmiPort = argv[1];
        } else {
            qmiPort = getDefaultPort();
            if( NULL == qmiPort ) {
                qmiPort = DEFAULT_QMI_PORT;
            }
        }

        if (argc >= 3) {
            secondaryPort = argv[2];
        } else if (!strncmp(ATFWD_DATA_TARGET_SVLTE2A, target, strlen(target))) {
           /* For SVLTE type II targets, Modem currently exposes two ATCOP ports.
            * One bridged from USB to SDIO, directly talking to 9k modem
            * Another bridged from USB to SMD, directly talking to 8k
            * Therefore given this modem architecture, ATFWD-daemon needs to
            * listen to both the modems( 8k & 9K).
            * Register with 8k modem
            */
            secondaryPort = DEFAULT_SMD_PORT;
        } else if (!strncmp(ATFWD_DATA_TARGET_SGLTE, target, strlen(target))) {
            // For SGLTE targets, Register with the SMUX port.
            secondaryPort = QMI_PORT_RMNET_SMUX_0;
        }
    }

    printf("init all signals\n");
    //signalInit();

    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&ctrMutex, &attr);
    pthread_cond_init(&ctrCond, NULL);

    printf("Explicitly disbling qmux \n");
    qmi_cci_qmux_xport_unregister(QMI_CLIENT_QMUX_RMNET_INSTANCE_0);
    qmi_cci_qmux_xport_unregister(QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0);
    qmi_cci_qmux_xport_unregister(QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0);
    qmi_cci_qmux_xport_unregister(QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0);
    printf("Disabling QMUX complete...\n");

    //Get QMI service object
    printf("getting at svc obj for access terminal QMI svc\n");
    qmi_at_svc_obj = at_get_service_object_v01();

    for (initType = INIT_QMI; initType != INIT_MAX; initType++) {
        connectionResult = 0;
        tryInit (initType, &connectionResult);
        printf(" tryinit complete with connectresult: %d\n", connectionResult);
        if (connectionResult < 0)
        {
            if ( !is_supported_qcci() )
            {
              if (qmiHandle >= 0) {
                  qmi_release(qmiHandle);
              }
            }
            else
            {
                printf("Release qmi_client...\n");
                qmi_client_release(qmi_at_svc_client);
                qmi_at_svc_client = NULL;
            }

            stopSelf();
            return -1;
        }
    }

    if ( !is_supported_qcci() )
    {
        if (qmiPort) {
            printf("ATFWD --> QMI Port : %s\n" , qmiPort);
            initAtcopServiceAndRegisterCommands(qmiPort, &userHandle);
            regForPrimaryPort = 0;
        }

        if (secondaryPort) {
            printf("ATFWD --> secondaryPort : %s\n" , secondaryPort);
            initAtcopServiceAndRegisterCommands(secondaryPort, &userHandleSMD);
            regForSecondaryPort = 0;
        }

        if(userHandle < 0 && userHandleSMD < 0) {
            printf("Could not register userhandle(s) with both 8k and 9k modems -- bail out");
            if (qmiHandle >= 0) {
                qmi_release(qmiHandle);
            }
            stopSelf();
            return -1;
        }
    }
    else
    {
       if (!registerATCommands())
       {
        stopSelf();
        return -1;
       }
    }

    while (1) {
        pthread_mutex_lock(&ctrMutex);
        while (!isNewCommandAvailable()) {
            printf("Waiting for ctrCond");
            pthread_cond_wait(&ctrCond, &ctrMutex);
            printf("Recieved ctrCond: p: %d, S:%d, nr: %d",regForPrimaryPort, regForSecondaryPort, newRequest );
        }

        if ( !is_supported_qcci() )
        {
            if (regForPrimaryPort == 1) {
                if (qmiPort) {
                    printf("Rcvd pthread notification for primary QMI port registration");
                    initAtcopServiceAndRegisterCommands(qmiPort, &userHandle);
                } else {
                    printf("Notification for primary QMI port registration when NOT valid, ignore...");
                }
                regForPrimaryPort = 0;
            }

            if (regForSecondaryPort == 1) {
                if (secondaryPort) {
                    printf("Rcvd pthread notification for secondary QMI port registration");
                    initAtcopServiceAndRegisterCommands(secondaryPort, &userHandleSMD);
                } else {
                    printf("Notification for secondary QMI port registration when NOT valid, ignore...");
                }
                regForSecondaryPort = 0;
            }
            if(userHandle < 0 && userHandleSMD < 0)
            {
                printf("userhandle(s) for both 8k and 9k modems NOT valid -- bail out");
                if (qmiHandle >= 0)
                {
                    qmi_release(qmiHandle);
                }
                stopSelf();
                return -1;
            }
        }
        else
        {
            if ( regForPrimaryPort == 1)
            {
                printf("Registering for primary port (QCCI).");
                connectionResult = 0;
                tryInit (INIT_QMI_SRVC, &connectionResult);
                printf(" init result: %d\n", connectionResult);
                if (connectionResult < 0)
                {
                    printf("Release qmi_client...\n");
                    qmi_client_release(qmi_at_svc_client);
                    qmi_at_svc_client = NULL;
                    stopSelf();
                    return -1;
                }
                if (!registerATCommands())
                {
                    printf("Register for primary port (QCCI) failed.");
                    stopSelf();
                    return -1;
                }
                regForPrimaryPort=0;
            }

            if ( regForServiceUp == 1 )
            {
                regForServiceUpEvent();
            }
        }

        if (newRequest == 1) {
            printf("pthread notified for new request; sending response.");
            response = sendit(&fwdcmd);
            if (response == NULL) {
                printf("Response processing complete. Invalid cmd resp.");
                sendInvalidCommandResponse();
                printf("Invalid response sending complete.");
            } else {
                printf("Response processing complete. Sending response.");
                sendResponse(response);
                printf("Send response complete.");
            }

            if (fwdcmd.name) free(fwdcmd.name);
            if (fwdcmd.tokens) {
                for (i = 0; i < fwdcmd.ntokens; i++) {
                    free(fwdcmd.tokens[i]);
                }
                free(fwdcmd.tokens);
            }
            freeAtCmdResponse(response);
            newRequest = 0;
            printf("New request processing complete.");
        }

        pthread_mutex_unlock(&ctrMutex);
    }

    return 0;
}
