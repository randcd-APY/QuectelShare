#ifndef QCMAP_SOCKSV5_CONFIGURE_H_
#define QCMAP_SOCKSV5_CONFIGURE_H_

/*====================================================

FILE:  QCMAP_SOCKSv5_Configure.h

SERVICES:
   QCMAP SOCKSv5 Configuration Specific Implementation

=====================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/

/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse
  chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------
  03/30/17   jt         initial SOCKSv5 support
==========================================================*/

/* RFC 1928 min and max private auth values */
#define MIN_PRIVATE_AUTH_VALUE 0x80
#define MAX_PRIVATE_AUTH_VALUE 0xFE

#define IPV4 4
#define IPV6 6
#define TEMP_SOCKSV5_CFG "/tmp/qti_socksv5_cfg.tmp"

#include "pugixml/pugixml.hpp"
#include "qualcomm_mobile_access_point_msgr_v01.h"

namespace QC_SOCKSv5_Configure
{
    bool SetSOCKSv5ProxyAuth(qmi_error_type_v01 *qmi_err_num, unsigned char auth_method);
    bool EditSOCKSv5ProxyLANIface(qmi_error_type_v01 *qmi_err_num, char *lan_iface);
    bool AddSOCKSv5ProxyUnameAssoc(qmi_error_type_v01 *qmi_err_num, char *uname,
                                   unsigned int service_no);
    bool DeleteSOCKSv5ProxyUnameAssoc(qmi_error_type_v01 *qmi_err_num, char *uname);
    bool GetSOCKSv5ProxyConfig(qmi_error_type_v01 *qmi_err_num,
                               qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01 *resp_msg);
};

#endif
