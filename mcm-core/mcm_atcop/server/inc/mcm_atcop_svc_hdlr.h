#ifndef _MCM_ATCOP_MGR_SVC_HDLR_H
#define _MCM_ATCOP_MGR_SVC_HDLR_H
/*===========================================================================

                         M C M _ A T C O P_ M G R _ S V C _ H D L R . H

DESCRIPTION

  The MCM ATCOP Service Handler Header File.

EXTERNALIZED FUNCTIONS

  mcm_atcop_mgr_svc_init()
    Register the MCM ATCOP service with QMUX for all applicable QMI links.

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/03/13    vm     Created module
===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include "stringl.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"

#include "comdef.h"
#include "customer.h"
#include "mcm_atcop_v01.h"

#define MCM_ATCOP_ENOERROR 0
#define MCM_ATCOP_SVC_MAX_CLIENT_HANDLES 5
#define SOFTAP_W_DUN_PATH "/etc/data/usb/softap_w_dun"

/*---------------------------------------------------------------------------
  MCM ATCOP Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_csi_service_handle service_handle;
  int                  num_clients;
  void*                client_handle_list[MCM_ATCOP_SVC_MAX_CLIENT_HANDLES];
  int                  qmi_instance;
} mcm_atcop_svc_state_info_type;


#endif /* _MCM_ATCOP_MGR_SVC_HDLR_H */
