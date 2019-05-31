/******************************************************************************

                        IPACM_PERF_QMI.CPP

******************************************************************************/

/******************************************************************************

  @file    ipacm_perf_qmi.cpp
  @brief   Qmi module for ipacm_perf

  DESCRIPTION
  Implementation of ipacm_perf_qmi.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when        who       what, where, why
--------   -----      -------------------------------------------------------
03/05/15   shihuanl   Initial version

******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "ipacm_perf.h"
#include "ipacm_perf_qmi.h"
#include "ipacm_perf_rm.h"

ipacm_perf_qmi* ipacm_perf_qmi::p_instance = NULL;

unsigned int ipacm_perf_qmi::qmi_service_ipacm_perf_sockfd = 0;

ipacm_perf_qmi::ipacm_perf_qmi()
{
	qmi_wds_client_handle = NULL;

	qmi_ipacm_perf_msgr_inited = false;

	os_params = {0};

	ipacm_perf_qmi_service_sockfd = 0;

	if (IPACM_PERF_SUCCESS != ipacm_perf_msgr_init())
	{
		IPACM_PERF_ERR("ipacm_perf msgr service not inited\n");
	}
	if (IPACM_PERF_SUCCESS != ipacm_perf_reg_available_cb())
	{
		IPACM_PERF_ERR(" Failed: Service Available Callback register\n");
	}
	else
	{
		IPACM_PERF_DBG(" Service Available Callback registered\n");
	}
}

/* Register for service available callback */
int ipacm_perf_qmi::ipacm_perf_reg_available_cb(void)
{
	qmi_idl_service_object_type wds_qmi_service_object;
	qmi_client_error_type qmi_error;

	wds_qmi_service_object = wds_get_service_object_v01();
	if (wds_qmi_service_object == NULL)
	{
		IPACM_PERF_ERR("ipacm perf service object(wds) not available.\n");
		return IPACM_PERF_FAILURE;
	}
	qmi_error = qmi_client_notifier_init(wds_qmi_service_object,
					&ipacm_perf_cb.qmi_wds_os_params,
					&ipacm_perf_cb.qmi_wds_notifier);
	if (qmi_error < 0)
	{
		IPACM_PERF_ERR("qmi_client_notifier_init(nas) returned %d\n",
			qmi_error);
		return IPACM_PERF_FAILURE;
	}
	qmi_error =  qmi_client_register_notify_cb(ipacm_perf_cb.qmi_wds_notifier,
					ipacm_perf_qmi::qmi_wds_service_available_cb, NULL);

	if (qmi_error < 0)
	{
		IPACM_PERF_ERR("qmi_client_register_notify_cb(nas) returned %d\n",
			qmi_error);

		IPACM_PERF_ERR("Releasing QMI WDS notifier\n");

		qmi_error = qmi_client_release(ipacm_perf_cb.qmi_wds_notifier);
		ipacm_perf_cb.qmi_wds_notifier = NULL;

		if (qmi_error != QMI_NO_ERR)
		{
			IPACM_PERF_ERR("Cannot release WDS qmi_wds_notifier handle %d\n", qmi_error);			
		}
		return IPACM_PERF_FAILURE;
	}

	IPACM_PERF_DBG(" ipacm_perf_qmi Constructor : QMI WDS init successful\n");
	return IPACM_PERF_SUCCESS;
}
ipacm_perf_qmi::~ipacm_perf_qmi()
{
	if(qmi_wds_client_handle != NULL)
	{
		qmi_client_release(qmi_wds_client_handle);
	}
}

/* qmi callback function */
void ipacm_perf_qmi::wds_client_cb
(
	qmi_client_type user_handle,                    /* QMI user handle       */
	unsigned int    msg_id,                         /* Indicator message ID  */
	void           *ind_buf,                        /* Raw indication data   */
	unsigned int    ind_buf_len,                    /* Raw data length       */
	void           *ind_cb_data                     /* User call back handle */
)
{
	if(ind_buf == NULL)
	{
		IPACM_PERF_ERR("Failed to receive any data in qmi message\n");
		return;
	}

	IPACM_PERF_DBG("wds_client_cb: user_handle %X msg_id %d ind_buf_len %d\n",
		user_handle, msg_id, ind_buf_len);

	ipacm_perf_throughput data = {0};
	qmi_client_error_type qmi_error;
	void* dest;
	int dest_len = 0;

	switch(msg_id)
	{
	case QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01:
		IPACM_PERF_DBG("Receive QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01\n");
		dest = (void*)&data;
		dest_len = sizeof(data);
		qmi_error = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      dest,
                                      dest_len);
		if(qmi_error != QMI_NO_ERR)
		{
			IPACM_PERF_ERR("Failed to decode qmi message\n");
			return;
		}

		if(data.ul_configured_throughput_valid &&
			data.dl_configured_throughput_valid)
		{
			ipacm_perf_rm* ptr;
			ptr = ipacm_perf_rm::get_instance();
			if(ptr)
			{
				ptr->set_modem_configured_throughput(
						data.ul_configured_throughput,
						data.dl_configured_throughput);
			}
		}
		break;

	default:
		IPACM_PERF_ERR("Receive unexpected msg_id: %d\n", msg_id);
		break;
	}

	return;
}

/* register QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01 to WDS server */
int ipacm_perf_qmi::register_wds_indication
(
	qmi_client_type client_hdl
)
{
	if(client_hdl == NULL)
	{
		IPACM_PERF_ERR("Client handle is not valid\n");
		return IPACM_PERF_FAILURE;
	}

	qmi_client_error_type qmi_error;
	wds_indication_register_req_msg_v01 req;
	wds_indication_register_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.report_configured_throughput_valid = 1;
	req.report_configured_throughput = 1;

	/* Send report_configured_throughput registration */
	qmi_error = qmi_client_send_msg_sync(client_hdl,
                                QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                IPACM_PERF_QMI_MAX_TIMEOUT_MS);

	if (qmi_error != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
	{
		IPACM_PERF_ERR("Failed to register indication with qmi_error=%d result=%d\n",
				qmi_error, resp.resp.result);
		return IPACM_PERF_FAILURE;
	}

	return IPACM_PERF_SUCCESS;
}

/* Callback, invoked from QMI service context, for modem service available */
void ipacm_perf_qmi::qmi_wds_service_available_cb
(
	qmi_client_type                user_handle,
	qmi_idl_service_object_type    service_obj,
	qmi_client_notify_event_type   service_event,
	void                           *notify_cb_data
)
{
	ipacm_perf_qmi_service_buffer_t ipacm_perf_qmi_service_buffer;
	int numBytes=0, len;
	struct sockaddr_un qmi_service_ipacm_perf;

	IPACM_PERF_DBG(" qmi_wds_service_available_cb %d\n", service_event);
	switch (service_event)
	{
	  case QMI_CLIENT_SERVICE_COUNT_INC:
		qmi_service_ipacm_perf.sun_family = AF_UNIX;
		(void)strlcpy(qmi_service_ipacm_perf.sun_path,
			IPACM_PERF_QMI_SERVICE_UDS_FILE,
		  	sizeof(qmi_service_ipacm_perf.sun_path));
		len = strlen(qmi_service_ipacm_perf.sun_path) + sizeof(qmi_service_ipacm_perf.sun_family);

		ipacm_perf_qmi_service_buffer.qmi_service_id = IPACM_PERF_QMI_WDS_IN_SERVICE;

		if ((numBytes =
				sendto(qmi_service_ipacm_perf_sockfd, (void *)&ipacm_perf_qmi_service_buffer,
				sizeof(ipacm_perf_qmi_service_buffer_t), MSG_DONTWAIT,
				(struct sockaddr *)&qmi_service_ipacm_perf, len)) == -1)
		{
			IPACM_PERF_ERR("Send Failed from qmi_wds_service_available_cb context\n");
			return;
		}
		IPACM_PERF_DBG("Msg: IPACM_PERF_QMI_WDS_IN_SERVICE sent\n");
		break;
	  default:
		IPACM_PERF_DBG(" qmi_wds_service_available_cb: Invalid Event %d\n",
						service_event);
		break;
	  }
	  return;
}

/* Callback, invoked from QMI service context, for modem error */
void ipacm_perf_qmi::ipacm_perf_qmi_wds_error_cb
(
	qmi_client_type user_handle,
	qmi_client_error_type qmi_error,
	void *err_cb_data
)
{
	ipacm_perf_qmi_service_buffer_t ipacm_perf_qmi_service_buffer;
	int numBytes=0, len;
	struct sockaddr_un qmi_service_ipacm_perf;

	IPACM_PERF_DBG(" ipacm_perf_qmi_wds_error_cb\n");

	qmi_service_ipacm_perf.sun_family = AF_UNIX;
	(void)strlcpy(qmi_service_ipacm_perf.sun_path, IPACM_PERF_QMI_SERVICE_UDS_FILE, sizeof(qmi_service_ipacm_perf.sun_path));
	len = strlen(qmi_service_ipacm_perf.sun_path) + sizeof(qmi_service_ipacm_perf.sun_family);

	ipacm_perf_qmi_service_buffer.qmi_service_id = IPACM_PERF_QMI_WDS_NOT_IN_SERVICE;

	if ((numBytes =
			sendto(qmi_service_ipacm_perf_sockfd, (void *)&ipacm_perf_qmi_service_buffer,
			sizeof(ipacm_perf_qmi_service_buffer_t), 0,
			(struct sockaddr *)&qmi_service_ipacm_perf, len)) == -1)
	{
		IPACM_PERF_ERR("Send Failed from ipacm_perf_qmi_wds_error_cb context\n");
		return;
	}

	IPACM_PERF_DBG("Msg: IPACM_PERF_QMI_WDS_NOT_IN_SERVICE sent\n");
	return;

}

/*
 * Function gets invoked when the modem is crashed
 * Modem not in service, do necessary cleanup and register for service
 * available callback
 */
int ipacm_perf_qmi::ipacm_perf_qmi_wds_not_in_service(void)
{
	qmi_client_error_type qmi_error;

	IPACM_PERF_DBG(":\n");

	qmi_error = qmi_client_release(qmi_wds_client_handle);
	qmi_wds_client_handle = NULL;
	if (qmi_error != QMI_NO_ERR)
	{
		IPACM_PERF_ERR("Cannot release WDS client handle %d\n", qmi_error);
		return IPACM_PERF_FAILURE;
	}
	IPACM_PERF_DBG("Successfully deregistered WDS client handle\n");

	if (IPACM_PERF_SUCCESS != ipacm_perf_reg_available_cb())
	{
		IPACM_PERF_ERR(" Failed: Service Available Callback register\n");
		return IPACM_PERF_FAILURE;
	}
	IPACM_PERF_DBG(" Service Available Callback registered\n");
	return IPACM_PERF_SUCCESS;
}

/*
 * Function gets invoked when the modem serivce is available
 * Register for QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01 callback
 * Register for modem down error callback
 */
int ipacm_perf_qmi::ipacm_perf_qmi_wds_in_service(void)
{
	qmi_idl_service_object_type qmi_wds_service_object;
	qmi_client_error_type qmi_error;
	qmi_cci_os_signal_type qmi_wds_os_params;
	ipacm_perf_rm* rm_ptr = NULL;
	ipacm_perf_throughput data;

	IPACM_PERF_DBG(":\n");

	qmi_wds_client_handle = NULL;

	qmi_wds_service_object = wds_get_service_object_v01();
	if(qmi_wds_service_object == NULL)
	{
		IPACM_PERF_ERR("WDS service object is not available\n");
		return IPACM_PERF_FAILURE;
	}
	IPACM_PERF_DBG("Got WDS service object\n");

	IPACM_PERF_DBG("Releasing QMI WDS notifier\n");

	qmi_error = qmi_client_release(ipacm_perf_cb.qmi_wds_notifier);
	ipacm_perf_cb.qmi_wds_notifier = NULL;

	if (qmi_error != QMI_NO_ERR)
	{
		IPACM_PERF_ERR("Cannot release WDS qmi_wds_notifier handle %d\n", qmi_error);
		return IPACM_PERF_FAILURE;
	}

	IPACM_PERF_DBG("Start initing WDS qmi client with timeout: %d s\n", IPACM_PERF_QMI_MAX_TIMEOUT_MS/1000);
	qmi_error = qmi_client_init_instance(qmi_wds_service_object,
			QMI_CLIENT_INSTANCE_ANY,
			ipacm_perf_qmi::wds_client_cb,
			NULL,
			&qmi_wds_os_params,
			IPACM_PERF_QMI_MAX_TIMEOUT_MS,
			&qmi_wds_client_handle);
	if(qmi_error != QMI_NO_ERR)
	{
		IPACM_PERF_ERR("Failed to init WDS qmi client %d\n", qmi_error);
		if(qmi_wds_client_handle != NULL)
		{
			qmi_error = qmi_client_release(qmi_wds_client_handle);
			qmi_wds_client_handle = NULL;

			if (qmi_error != QMI_NO_ERR)
			{
				IPACM_PERF_ERR("Cannot release WDS client handle %d\n", qmi_error);
				return IPACM_PERF_FAILURE;
			}
		}
		return IPACM_PERF_FAILURE;
	}
	IPACM_PERF_DBG("WDS qmi client is inited with handle %d\n", qmi_wds_client_handle);

	if(register_wds_indication(qmi_wds_client_handle) == IPACM_PERF_FAILURE)
	{
		IPACM_PERF_ERR("Failed to register WDS indication\n");
		qmi_error = qmi_client_release(qmi_wds_client_handle);
		qmi_wds_client_handle = NULL;

		if (qmi_error != QMI_NO_ERR)
		{
			IPACM_PERF_ERR("Cannot release WDS client handle %d\n", qmi_error);
			return IPACM_PERF_FAILURE;
		}
	}

	/* Vote for low BW value*/
	rm_ptr = ipacm_perf_rm::get_instance();
	if (rm_ptr)
	{
		data = rm_ptr->get_modem_configured_throughput();
		IPACM_PERF_DBG("UL valid = %d; DL Valid = %d\n",
			data.ul_configured_throughput_valid,
			data.dl_configured_throughput_valid);
		IPACM_PERF_DBG("UL tput = %d; DL tput = %d\n",
			data.ul_configured_throughput,
			data.dl_configured_throughput);
		/* vote for low BW (100Mbps), if it is lesser */
		if (data.dl_configured_throughput >=
			IPACM_PERF_RM_NON_LTE_CAT16_BW_VOTE_MBPS);
		data.dl_configured_throughput =
			IPACM_PERF_RM_NON_LTE_CAT16_BW_VOTE_MBPS;
		rm_ptr->set_modem_configured_throughput(
						data.ul_configured_throughput,
						data.dl_configured_throughput);
	}

	/*-----------------------------------------------------------------------------
	  Register for err callback
	------------------------------------------------------------------------------*/
	(void) qmi_client_register_error_cb(qmi_wds_client_handle,
				ipacm_perf_qmi::ipacm_perf_qmi_wds_error_cb,
				NULL);
	return IPACM_PERF_SUCCESS;
}

/* Function decides to invoke in-service or not-in-service based on the message received */
void ipacm_perf_qmi::ipacm_perf_process_qmi_service_availabilty
(
  ipacm_perf_qmi_service_t  qmi_service_id
)
{
	IPACM_PERF_DBG(":\n");
	switch (qmi_service_id)
	{
		case IPACM_PERF_QMI_WDS_IN_SERVICE:
			if (ipacm_perf_qmi_wds_in_service() != IPACM_PERF_SUCCESS )
			{
				IPACM_PERF_ERR("Not able to Init QMI wds client\n");
			}
			break;

		case IPACM_PERF_QMI_WDS_NOT_IN_SERVICE:
			if (ipacm_perf_qmi_wds_not_in_service() != IPACM_PERF_SUCCESS )
			{
				IPACM_PERF_ERR("Not able to clean up QMI wds context during SSR\n");
			}
			break;

		default:
			IPACM_PERF_ERR("Invalid QMI service =%d\n",qmi_service_id);
			break;
	}
	return;
}

int ipacm_perf_qmi::create_socket(unsigned int *sockfd)
{

	IPACM_PERF_DBG(":\n");
	if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		IPACM_PERF_ERR("Error creating socket, errno: %d\n", errno);
		return IPACM_PERF_FAILURE;
	}

	if (fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
	{
		IPACM_PERF_ERR("Couldn't set Close on Exec, errno: %d\n", errno);/* Its Okay if it fails here */
	}

	return IPACM_PERF_SUCCESS;
}

int ipacm_perf_qmi::create_ipacm_perf_service_available_socket(void)
{
	int val, rval;
	struct sockaddr_un ipacm_perf_dsd;
	struct timeval rcv_timeo;
	int len;
	rval = create_socket(&ipacm_perf_qmi_service_sockfd);
	IPACM_PERF_DBG(":\n");
	if (rval == -1 || ipacm_perf_qmi_service_sockfd < 0)
	{
		IPACM_PERF_ERR("Failed to create ipacm_perf_qmi_service_sockfd\n");
		goto failure;
	}
	else if (ipacm_perf_qmi_service_sockfd >= FD_SETSIZE)
	{
		IPACM_PERF_ERR("ipacm_perf_qmi_service_sockfd fd=%d is exceeding range\n",
		ipacm_perf_qmi_service_sockfd);
		goto failure;
	}

	rcv_timeo.tv_sec = 0;
	rcv_timeo.tv_usec = 100000;
	setsockopt(ipacm_perf_qmi_service_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
	val = fcntl(ipacm_perf_qmi_service_sockfd, F_GETFL, 0);
	fcntl(ipacm_perf_qmi_service_sockfd, F_SETFL, val | O_NONBLOCK);
	/* set bit in ipacm_perf_qmi::os_params */
	FD_SET(ipacm_perf_qmi_service_sockfd, &(os_params.fds));
	os_params.max_fd = MAX(os_params.max_fd, ipacm_perf_qmi_service_sockfd);

	ipacm_perf_dsd.sun_family = AF_UNIX;
	(void)strlcpy(ipacm_perf_dsd.sun_path, IPACM_PERF_QMI_SERVICE_UDS_FILE, sizeof(ipacm_perf_dsd.sun_path));
	unlink(ipacm_perf_dsd.sun_path);
	len = strlen(ipacm_perf_dsd.sun_path) + sizeof(ipacm_perf_dsd.sun_family);
	if (bind(ipacm_perf_qmi_service_sockfd, (struct sockaddr *)&ipacm_perf_dsd, len) == -1)
	{
		IPACM_PERF_ERR("Error binding the socket, errno: %d\n", errno);
		goto socket_fail;
	}
success:
	return IPACM_PERF_SUCCESS;
socket_fail:
	close(ipacm_perf_qmi_service_sockfd);
failure:
	return IPACM_PERF_FAILURE;
}

/* Function init the msgr service, create server, client sockets */
int ipacm_perf_qmi::ipacm_perf_msgr_init(void)
{
	int rval;
	IPACM_PERF_DBG("ipacm_perf msgr service inited %d\n", qmi_ipacm_perf_msgr_inited);

	if (qmi_ipacm_perf_msgr_inited)
	{
		IPACM_PERF_DBG("ipacm_perf msgr service already initialized\n");
		goto success;
	}

	/* Create qmi service -> ipacm_perf client socket */
	if (create_socket(&qmi_service_ipacm_perf_sockfd) != IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_ERR("ipacm_perf_msgr_init error creating qmi_service_ipacm_perf_sockfd socket\n");
	}

	rval = create_ipacm_perf_service_available_socket();

	if (rval != IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_ERR("Unable to create ipacm_perf service available socket!\n");
		goto failure;
	}

	qmi_ipacm_perf_msgr_inited = true;
	IPACM_PERF_DBG("ipacm_perf msgr service initialized\n");
success:
	return IPACM_PERF_SUCCESS;
failure:
	return IPACM_PERF_FAILURE;
}

/*
 * Runs in the main function context in endless loop
 * It process the received message and handle it to respected service
 */
void ipacm_perf_qmi::ipacm_handle_msgr_service(ipacm_perf_qmi* qmi_ptr)
{
	int ret, index;
	fd_set master_fd_set;
	int nbytes;
	char buf[MAX_BUF_LEN];
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof(struct sockaddr_storage);
	ipacm_perf_qmi_service_buffer_t *ipacm_perf_qmi_service_buffer = NULL;
	IPACM_PERF_DBG(":\n");
	while(1)
	{
		master_fd_set = os_params.fds;
		ret = select(os_params.max_fd+1, &master_fd_set, NULL, NULL, NULL);
		if (ret < 0)
		{
			IPACM_PERF_ERR("Error in select, errno:%d\n", errno);
			if( errno == EINTR ) /*  Failure due to interruption, try again */
			{
				IPACM_PERF_ERR("Process interrupted, will try again on select\n");
				continue;
			}
			else
			{
				IPACM_PERF_ERR("Failure: Will stop the process\n");
				return;
			}
		}
		for (index = 0; index <= os_params.max_fd; index++)
		{
			if (FD_ISSET(index, &master_fd_set))
			{
				if (index == ipacm_perf_qmi_service_sockfd)
				{
					IPACM_PERF_DBG("\n\nReceived Message from registered SSR callback context\n\n\n");
					if ((nbytes = recvfrom(index, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0)
					{
						if ( nbytes == 0 )
						{
							IPACM_PERF_ERR("Completed full recv from nas context\n");
						}
						else
						{
							IPACM_PERF_ERR("recvfrom returned error, errno:%d\n", errno);
						}
					}
					else
					{
						ipacm_perf_qmi_service_buffer =  (ipacm_perf_qmi_service_buffer_t *)buf;
						qmi_ptr->ipacm_perf_process_qmi_service_availabilty(ipacm_perf_qmi_service_buffer->qmi_service_id);
					}
				}
			}
		}
	}
}

/* get ipacm_perf_qmi instance */
ipacm_perf_qmi* ipacm_perf_qmi::get_instance()
{
	if(ipacm_perf_qmi::p_instance == NULL)
	{
		ipacm_perf_qmi::p_instance = new ipacm_perf_qmi();
		IPACM_PERF_DBG("Qmi instance pointer: 0x%08x\n", ipacm_perf_qmi::p_instance);
	}
	return ipacm_perf_qmi::p_instance;
}

/* delete ipacm_perf_qmi instance */
void ipacm_perf_qmi::delete_instance()
{
	if(ipacm_perf_qmi::p_instance != NULL)
	{
		delete ipacm_perf_qmi::p_instance;
		IPACM_PERF_DBG("Qmi instance is deleted\n");
		ipacm_perf_qmi::p_instance = NULL;
	}
	return;
}
