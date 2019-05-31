#ifndef _IPACM_PERF_QMI_H_
#define _IPACM_PERF_QMI_H_
/******************************************************************************

                        IPACM_PERF_QMI.H

******************************************************************************/

/******************************************************************************

  @file ipacm_perf_qmi.h
  @brief ipacm_perf_qmi header file.

  DESCRIPTION
  Header file of ipacm_perf_qmi.

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
#include <stdint.h>
#include "qmi_client.h"
#include "wireless_data_service_v01.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"


#define IPACM_PERF_QMI_MAX_TIMEOUT_MS 60000

#define MAX_BUF_LEN 256

#define IPACM_PERF_QMI_SERVICE_UDS_FILE "/data/misc/ipa/ipacm_perf_qmi_service_file"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

/*---------------------------------------------------------------------------
  IPACM_PERF QMI SERVICE
----------------------------------------------------------------------------*/

typedef enum
{
	IPACM_PERF_QMI_WDS_IN_SERVICE = 0,
	IPACM_PERF_QMI_WDS_NOT_IN_SERVICE,
} ipacm_perf_qmi_service_t;

typedef struct qcmap_qmi_service_buffer_s {
	ipacm_perf_qmi_service_t  qmi_service_id;
} ipacm_perf_qmi_service_buffer_t;

typedef struct
{
    /* QMI WDS service info */
	qmi_client_type           qmi_wds_notifier;
	qmi_cci_os_signal_type    qmi_wds_os_params;
} ipacm_perf_cb_t;


class ipacm_perf_qmi
{
public:

	/* get ipacm_perf_qmi instance */
	static ipacm_perf_qmi* get_instance();

	/* delete ipacm_perf_qmi instance */
	static void delete_instance();

	/* Runs in the main function context in endless loop */
	void ipacm_handle_msgr_service(ipacm_perf_qmi* qmi_ptr);

private:

	ipacm_perf_qmi();

	~ipacm_perf_qmi();

	static ipacm_perf_qmi* p_instance;

	qmi_client_type qmi_wds_client_handle;

	ipacm_perf_cb_t ipacm_perf_cb;

	qmi_csi_os_params os_params;

	/* Server socket */
	unsigned int ipacm_perf_qmi_service_sockfd;

	/* Client socket */
	static unsigned int qmi_service_ipacm_perf_sockfd;

	boolean qmi_ipacm_perf_msgr_inited;

	/* Function init the msgr service, create server, client sockets */
	int ipacm_perf_msgr_init(void);

	int create_socket(unsigned int *sockfd);

	int create_ipacm_perf_service_available_socket(void);

	/* Register for service available callback */
	int ipacm_perf_reg_available_cb(void);

	/* Function decides to invoke in-service or not-in-service based on the message received */
	void ipacm_perf_process_qmi_service_availabilty
	(
	  ipacm_perf_qmi_service_t  qmi_service_id
	);

	/* Function gets invoked when the modem serivce is available */
	int ipacm_perf_qmi_wds_in_service(void);

	/* Function gets invoked when the modem is crashed */
	int ipacm_perf_qmi_wds_not_in_service(void);

	/* qmi callback functions */
	static void wds_client_cb
	(
		qmi_client_type user_handle,                    /* QMI user handle       */
		unsigned int    msg_id,                         /* Indicator message ID  */
		void           *ind_buf,                        /* Raw indication data   */
		unsigned int    ind_buf_len,                    /* Raw data length       */
		void           *ind_cb_data                     /* User call back handle */
	);

	/* Callback, invoked from QMI service context, for modem error */
	static void ipacm_perf_qmi_wds_error_cb (qmi_client_type user_handle,
		qmi_client_error_type qmi_error,
		void *err_cb_data);

	/* Callback, invoked from QMI service context, for modem service available */
	static void qmi_wds_service_available_cb
	(
		qmi_client_type			user_handle,
		qmi_idl_service_object_type	service_obj,
		qmi_client_notify_event_type	service_event,
		void				*notify_cb_data
	);

	/* register QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01 to WDS server */
	int register_wds_indication
	(
		qmi_client_type client_hdl
	);

};

#endif /* _IPACM_PERF_QMI_H_ */

