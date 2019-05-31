/******************************************************************************

  @file	 pdc_daemon.h

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __PDC_DAEMON_H__
#define __PDC_DAEMON_H__



#define strlcpy g_strlcpy
#define strlcat g_strlcat

typedef struct ind_pdc {
	int8_t ind_received;
	uint32_t msg_id;
        uint32_t config_id_len;
	uint8_t config_id[255];
        pthread_mutex_t lock;
} cb_ind_t;




#endif
