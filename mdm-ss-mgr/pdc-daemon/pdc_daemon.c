/*
 * ---------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "persistent_device_configuration_v01.h"
#include "pdc_daemon.h"


#define FIRMWARE_FILE "/etc/init.d/fw.tmp"
#define TRUE 1
#define FW_MAX_SIZE 300

#define LOG_TAG "PDCDAEMON"
#define LOGI(x...) if(bfileLog) fprintf(fLog,"Info:"x);


static FILE* fLog=NULL;
static int8_t bfileLog = 0;


static qmi_client_type pdc_client;
static qmi_client_type notifier;
static qmi_cci_os_signal_type os_signals;

static uint8_t pdc_qmi_initialized;
static pthread_mutex_t waitmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t waitcond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t ind_cond = PTHREAD_COND_INITIALIZER;
static pthread_t wait_thread,msg_thread;
static cb_ind_t  indication;




/*=========================================================================

    FUNCTION  pdc_daemon_exit
===========================================================================*/

/*!
 * @brief
 *
 *   Terminate pdc-daemon on sigterm.
 *
 *   @return
 *     None
 *
 */
/*=========================================================================*/
static void pdc_daemon_exit(int signum)
{
	LOGI("pdc daemon:exit\n");
	pthread_cond_signal(&waitcond);

}




/*=========================================================================

    FUNCTION  pdc_config_change_handler
===========================================================================*/

/*!
 * @brief
 *
 *   Handler function for config change indication from pdc service.
 *   QMI MSG handled: QMI_PDC_CONFIG_CHANGE_IND_V01
 *   QMI MSG posted to message thread:QMI_PDC_GET_SELECTED_CONFIG_REQ_V01
 *   @return
 *   None
 *
 */
/*=========================================================================*/

static void pdc_config_change_handler(void* ind_buff,uint32_t ind_buff_len)
{
	int rc=0;

        LOGI("pdc config change handler called\n");
	pdc_config_change_ind_msg_v01 *config_ind_buff;

	config_ind_buff =  (pdc_config_change_ind_msg_v01*)ind_buff;
	if(config_ind_buff->new_config_info.config_type != PDC_CONFIG_TYPE_MODEM_SW_V01 ||
	   config_ind_buff->new_config_info.config_id_len > PDC_CONFIG_ID_SIZE_MAX_V01){
		system("echo 'pdc config change error' > /dev/kmsg");
		return;
	}
	indication.config_id_len = config_ind_buff->new_config_info.config_id_len;
	memcpy(indication.config_id,
		config_ind_buff->new_config_info.config_id,
		config_ind_buff->new_config_info.config_id_len);

	indication.msg_id = QMI_PDC_GET_SELECTED_CONFIG_REQ_V01;
	indication.ind_received = 1;
        pthread_cond_signal(&ind_cond);
	LOGI("SIGNALLED SELCT CONFIG\n");
}




/*=========================================================================

    FUNCTION  pdc_get_selected_config_ind_handler
===========================================================================*/

/*!
 * @brief
 *
 *   Handler function for selected config indication from pdc service
 *   QMI MSG handled: QMI_PDC_GET_SELECTED_CONFIG_IND_V01
 *   QMI MSG posted to message thread:  QMI_PDC_GET_CONFIG_INFO_REQ_V01
 *
 *   @return
 *   None
 *
 */
/*=========================================================================*/

static void pdc_get_selected_config_ind_handler(void* ind_buff,uint32_t buff_len)
{
	int rc=0;
	pdc_get_selected_config_ind_msg_v01 *config_ind_buff;
	config_ind_buff = (pdc_get_selected_config_ind_msg_v01*)ind_buff;

	LOGI("GET SELECTED CONFIG HANDLER\n");
	if(config_ind_buff->error != QMI_ERR_NONE ||
	   config_ind_buff->pending_config_id_valid != TRUE ||
	   config_ind_buff->pending_config_id_len > PDC_CONFIG_ID_SIZE_MAX_V01 ||
	   config_ind_buff->pending_config_id_len != indication.config_id_len)
	{
		LOGI("selected config ind error=%d\n",config_ind_buff->error)
		return;
	}

	LOGI("COMPARE INDICATION ID VS PENDING ID\n");
	if(memcmp(indication.config_id,config_ind_buff->pending_config_id,
		  indication.config_id_len) != 0)
	{
		LOGI("ind config id=%d : %d\n",indication.config_id,indication.config_id_len);
		LOGI("pending id=%d\n",config_ind_buff->pending_config_id);
		LOGI("pending idlen=%d\n",config_ind_buff->pending_config_id_len);
		return;
	}

	LOGI("UPDATE INDICATION ID\n");
	indication.config_id_len= config_ind_buff->pending_config_id_len;
	memcpy(indication.config_id,config_ind_buff->pending_config_id,
	       config_ind_buff->pending_config_id_len);

	indication.msg_id = QMI_PDC_GET_CONFIG_INFO_REQ_V01;
	indication.ind_received = 1;
	pthread_cond_signal(&ind_cond);
	LOGI("SIGNALLED GET CONFIG INFO MSG\n");

}




/*=========================================================================

    FUNCTION  pdc_get_config_ind_handler
===========================================================================*/

/*!
 * @brief
 *
 *   Handler function for get_config_info_indication from pdc service
 *   QMI MSG handled:  QMI_PDC_GET_CONFIG_INFO_IND_V01
 *   QMI MSG posted to message thread: None
 *
 *   @return
 *   None
 *
 */
/*=========================================================================*/

static void pdc_get_config_ind_handler(void* ind_buff,uint32_t ind_buff_len)
{
	int rc=0;
	FILE *fp;
	struct stat sb;
	uint8_t carrier_desc[PDC_CONFIG_DESC_SIZE_MAX_V01];
	uint8_t fwdir[FW_MAX_SIZE];
	char *fwpath="/firmware/image/";
	pdc_get_config_info_ind_msg_v01 *get_config_ind_buff;
	get_config_ind_buff = (pdc_get_config_info_ind_msg_v01*)ind_buff;

	LOGI("pdc get config ind handler called\n");
        if(get_config_ind_buff->error != QMI_ERR_NONE)
                return;

        LOGI("pdc get config check params\n");

        if(get_config_ind_buff->config_desc_valid == TRUE &&
           get_config_ind_buff->config_desc_len <= PDC_CONFIG_DESC_SIZE_MAX_V01)
        {
		LOGI("CONFIG DESCLEN =%d\n",get_config_ind_buff->config_desc_len);
		LOGI("CONFIG DESC = %s\n",get_config_ind_buff->config_desc);
                strlcpy(carrier_desc,get_config_ind_buff->config_desc,
                         get_config_ind_buff->config_desc_len+1);
		LOGI("CARRIER DESC=%s\n",carrier_desc);
		strlcpy(fwdir,fwpath,sizeof(fwdir));
		strlcat(fwdir,carrier_desc,sizeof(fwdir));
		if(stat(fwdir,&sb) == 0 && S_ISDIR(sb.st_mode))
		{
			LOGI("carrier firmware directory exists\n");
		}
		else
		{
			LOGI("carrier firmware directory doesnt exist\n");
			system("rm /etc/init.d/fw");
			system("rm -rf /lib/firmware/mba.*");
			system("rm -rf /lib/firmware/modem.*");
			goto out;
		}
		fp=fopen(FIRMWARE_FILE,"w");
		if(fp == NULL)
		{
		   system("echo 'null fw filepointer' > /dev/kmsg");
		   return;
		}
		rc = fprintf(fp,"%s",carrier_desc);
		if(rc < 0){
		   system("echo 'carrier desc write failed' > /dev/kmsg");
		   fclose(fp);
		   return;
	        }

		fclose(fp);
		system("fsync /etc/init.d/fw.tmp");
		system("mv /etc/init.d/fw.tmp /etc/init.d/fw");
		system("rm -rf /lib/firmware/mba.*");
                system("rm -rf /lib/firmware/modem.*");
        }
out: system("reboot");

}




/*=========================================================================

    FUNCTION  pdc_send_msg
===========================================================================*/

/*!
 * @brief
 *
 *   QMI-PDC request-response thread.
 *   Sends request messages to pdc service based on received indication.
 *
 *   @return
 *   None
 *
 */
/*=========================================================================*/
static void pdc_send_msg(void *data)
{
	int rc=0;
	pdc_get_selected_config_req_msg_v01 get_sel_req;
        pdc_get_selected_config_resp_msg_v01  get_sel_response;
	pdc_get_config_info_req_msg_v01 get_config_req;
        pdc_get_config_info_resp_msg_v01 get_config_response;

	while(1)
	{
		pthread_mutex_lock(&indication.lock);
		if(!indication.ind_received)
                        pthread_cond_wait(&ind_cond,&indication.lock);
		LOGI("GOING TO CALL REQ MES\n");
		switch(indication.msg_id)
		{
		  case QMI_PDC_GET_SELECTED_CONFIG_REQ_V01:
			LOGI("SEND MES SELECTED CONFIG\n");
			memset(&get_sel_req, 0 , sizeof(get_sel_req));
			memset(&get_sel_response, 0, sizeof(get_sel_response));
			get_sel_req.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
			rc = qmi_client_send_msg_sync(pdc_client,
                                      QMI_PDC_GET_SELECTED_CONFIG_REQ_V01,
                                      &get_sel_req,sizeof(get_sel_req),
                                      &get_sel_response,sizeof(get_sel_response),
                                      1000);
			LOGI("SEND MSG ERROR=%d : %x\n",rc,rc);
			break;
		  case QMI_PDC_GET_CONFIG_INFO_REQ_V01:
			LOGI("SEND MSG GET CONFIG INFO\n");
			memset(&get_config_req,0,sizeof(get_config_req));
			memset(&get_config_response,0,sizeof(get_config_response));
			get_config_req.new_config_info.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
			get_config_req.new_config_info.config_id_len = indication.config_id_len;
			memcpy(get_config_req.new_config_info.config_id,
			       indication.config_id,
                               indication.config_id_len);
			LOGI("SEND GET CONFIG INFO MSG\n");
			rc = qmi_client_send_msg_sync(pdc_client,
                                      QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                      &get_config_req,sizeof(get_config_req),
                                      &get_config_response,sizeof(get_config_response),
                                      1000);
			LOGI("SEND MSG ERROR=%d : %x\n",rc,rc);
			break;
		}
		if(QMI_NO_ERR == rc && get_sel_response.resp.error == 0)
		{
			system("echo 'pdc get config response success' > /dev/kmsg");
		}
		else
		{       LOGI("config resp failure=%d %x\n",get_sel_response.resp.error,
			     get_sel_response.resp.error);
			system("echo 'pdc get config response failure' > /dev/kmsg");
		}
		indication.ind_received = 0;
		pthread_mutex_unlock(&indication.lock);
	}

}




/*=========================================================================

    FUNCTION  pdc_config_ind_cb
===========================================================================*/

/*!
 * @brief
 *
 *   QMI-PDC client callback function for receiving config indications
 *
     @return
 *   None
 *
 */
/*=========================================================================*/
static void pdc_config_ind_cb(qmi_client_type handle, unsigned long msg_id,
                unsigned char *buffer, int buffer_len, void *indication_data)
{
	int rc;
	uint32_t dec_payload_len=0;
	void* dec_payload_buffer=NULL;

	LOGI("pdc config ind mes_id=%x :  %d\n",msg_id,msg_id);

	pthread_mutex_lock(&indication.lock);
	LOGI("pdc config ind cb took indication lock\n");

	rc = qmi_idl_get_message_c_struct_len(pdc_get_service_object_v01(),
					      QMI_IDL_INDICATION,
					      msg_id,&dec_payload_len);
	LOGI("idl message error=%d\n",rc)
	if(QMI_NO_ERR == rc)
	{
		LOGI("pdc config ind cb dec payload alloc\n");
		dec_payload_buffer = malloc(dec_payload_len);
		if(dec_payload_buffer != NULL)
		{
			memset(dec_payload_buffer,0,dec_payload_len);
		}
		else
		{
			system("echo 'pdc config ind cb null buffer' > /dev/kmsg");
			goto error;
		}
		LOGI("pdc config ind cb message decode\n");
		rc = qmi_client_message_decode(handle,QMI_IDL_INDICATION,msg_id,
					       buffer,buffer_len,dec_payload_buffer,
					       (int)dec_payload_len);
		if(QMI_NO_ERR == rc)
		{
		  LOGI("pdc config ind cb msg_id action\n");
		  switch(msg_id)
		  {
		    case  QMI_PDC_CONFIG_CHANGE_IND_V01:
			  LOGI("pdc config ind cb config_change_msgid\n");
			  pdc_config_change_handler(dec_payload_buffer,dec_payload_len);
			  break;
		    case  QMI_PDC_GET_SELECTED_CONFIG_IND_V01:
			  LOGI("PDC GET SELECTED CONFIG IND MSG\n");
			  pdc_get_selected_config_ind_handler(dec_payload_buffer,dec_payload_len);
			  break;
		    case  QMI_PDC_GET_CONFIG_INFO_IND_V01:
			  LOGI("pdc config ind cb get_config_change_msgid\n");
			  pdc_get_config_ind_handler(dec_payload_buffer,dec_payload_len);
                          break;
		 }
		}
		LOGI("FREE PAYLOAD BUFFER\n");
		free(dec_payload_buffer);
	}
	LOGI("pdc config ind cb done\n");
error:	pthread_mutex_unlock(&indication.lock);
	return;
}




/*=========================================================================

    FUNCTION  pdc_qmi_init
===========================================================================*/

/*!
 * @brief
 *
 *   Handles QMI-PDC client initialization.
 *   Registers client control point for pdc service indications.
 *
 *   @return
 *   Success : 0
 *   Failure: -EINVAL
 *
 */
/*=========================================================================*/
static int pdc_qmi_init(void)
{
	int i = 0, rc = 0, retry_count=5;
	unsigned int num_entries = 1, num_services=0;
	qmi_cci_os_signal_type os_params;
	qmi_idl_service_object_type pdc_service_object;
	qmi_service_info info[10];
	pdc_indication_register_req_msg_v01 ind_req;
	pdc_indication_register_resp_msg_v01 ind_response;

	LOGI("%s: Initiallizing QMI \n", __func__);
	pdc_service_object = pdc_get_service_object_v01();
	if (!pdc_service_object){
		LOGI("%s: pdc_get_service_object_v01 failed\n",
				__func__);
                system("echo 'no pdc service object' > /dev/kmsg");
		return -EINVAL;
	}

        LOGI("calling qmi notifier init\n");
	rc = qmi_client_notifier_init(pdc_service_object, &os_params,
								&notifier);
        LOGI("calling qmi notifieer init done\n");


	/* Check if the pdc service is up, if not wait on a signal */
	/* max wait time = 5 secs */
	while(1){
                LOGI("calling qmi get service list\n");
		rc = qmi_client_get_service_list( pdc_service_object, NULL,
				NULL, &num_services);
		LOGI("%s: qmi_client_get_service_list returned %d"
				"num_services %d\n", __func__, rc,
				num_services);
		if(QMI_NO_ERR == rc)
			break;

		if(!retry_count){
			LOGI("%s:retry count exceeds limit"
					"QMI init failed continuing without"
					"server\n", __func__);
                       system("echo 'retry count failed' > /dev/kmsg");
                       return -EINVAL;
		}

		sleep(1);

		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 1000);
		retry_count--;
	}

	/* The pdc service has come up, save the information in info variable */
	rc = qmi_client_get_service_list(pdc_service_object, info,
			&num_entries, &num_services);
	if (QMI_NO_ERR != rc){
		LOGI("%s: qmi_client_get_service_list returned %d"
				"num_service %d num_entries %d\n", __func__,
				rc, num_services, num_entries);
		qmi_client_release(notifier);
                system("echo 'get service list failed' > /dev/kmsg");
		return -EINVAL;
	}

        LOGI("calling qmi pdc client init\n");
	rc = qmi_client_init(&info[0], pdc_service_object, pdc_config_ind_cb,
			NULL, &os_signals, &pdc_client);
	if (QMI_NO_ERR != rc){
		LOGI("%s: Client init failed %d\n", __func__, rc);
                system("echo 'qmi pdc init failed' > /dev/kmsg");
		qmi_client_release(notifier);
		return -EINVAL;
	}

	LOGI("calling qmi pdc init success initial\n");
	memset(&ind_req, 0 , sizeof(ind_req));
	memset(&ind_response, 0, sizeof(ind_response));

	ind_req.reg_config_change_valid = TRUE;
	ind_req.reg_config_change = 0x01;

	rc = qmi_client_send_msg_sync(pdc_client,
				      QMI_PDC_INDICATION_REGISTER_REQ_V01,
				      &ind_req,sizeof(ind_req),
				      &ind_response,sizeof(ind_response),
				      1000);
	if(QMI_NO_ERR == rc && ind_response.resp.error == 0 &&
	   ind_response.resp.result == 0)
	{
		LOGI("pdc indication register success\n");
	}
	else
	{
		system("echo 'pdc indication register failure' > /dev/kmsg");
		qmi_client_release(notifier);
		return -EINVAL;
	}
	/* release the notifier after qmi client initialization */
	qmi_client_release(notifier);
        LOGI("calling qmi pdc init success\n");
	/* QMI initialization done */

	pdc_qmi_initialized = 1;
        LOGI("returning to main\n");
	return 0;
}




/*=========================================================================

    FUNCTION  pdc_wait_threadfunc
===========================================================================*/

/*!
 * @brief
 *
 *   QMI-PDC client wait thread.
 *   @return
 *   None
 *
 */
/*=========================================================================*/
static void pdc_wait_threadfunc(void *param)
{
	pthread_mutex_lock(&waitmutex);
	pthread_cond_wait(&waitcond, &waitmutex);

	LOGI("pdc daemon: exit\n");
	pthread_mutex_unlock(&waitmutex);
	pthread_mutex_destroy(&waitmutex);
	pthread_detach(pthread_self());
	pthread_exit(NULL);

}


int main(void)
{
	int ret;
	struct sigaction sa;
        sa.sa_handler = pdc_daemon_exit;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	fLog = fopen("/data/logs/pdclg.txt", "w+");
	if(fLog)
	{
		bfileLog = 1;
		setbuf(fLog, NULL);
	}

	if (sigaction(SIGTERM, &sa, NULL) == -1)
		LOGI("pdc daemon: sigaction graceful shutdown disabled\n");

	LOGI("indication lock init\n");
	if (pthread_mutex_init(&indication.lock, NULL)) {
                system("echo 'pdc indication mutex init failed' > /dev/kmsg");
                return -EINVAL;
        }

        LOGI("calling qmi init\n");
	ret = pdc_qmi_init();
	if (ret)
	{
		system("echo 'pdc daemon: qmi init failed' > /dev/kmsg");
		goto out;
	}

	ret = pthread_create(&msg_thread, NULL, (void *)pdc_send_msg, NULL);
        if (ret < 0) {
                system("echo 'cannot create thread pdc msg thread' > /dev/kmsg");
                goto out;
        }

        LOGI("wait thread create\n");
	ret = pthread_create(&wait_thread, NULL, (void *)pdc_wait_threadfunc,
			NULL);
	if( ret < 0 ) {
		system("echo 'pdc-daemon: sleep thread creation failure' > /dev/kmsg");
		goto out;
	}

	LOGI("calling wait thread join\n");
	ret = pthread_join(wait_thread, NULL);

out:
	if (pdc_qmi_initialized) {
		qmi_client_release(pdc_client);
	}
	return 0;


}
