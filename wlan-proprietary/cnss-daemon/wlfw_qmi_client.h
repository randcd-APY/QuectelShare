/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WLFW_QMI_CLIENT_H__
#define __WLFW_QMI_CLIENT_H__

enum wlfw_svc_flag {
	SVC_START,
	SVC_RECONNECT,
	SVC_DISCONNECTED,
	SVC_EXIT,
};

#ifdef ICNSS_QMI
int wlfw_start(enum wlfw_svc_flag flag);
int wlfw_stop(enum wlfw_svc_flag flag);
#else
static inline int wlfw_start(enum wlfw_svc_flag flag)
{
	flag;
	return 0;
}
static inline int wlfw_stop(enum wlfw_svc_flag flag)
{
	flag;
	return 0;
}
#endif /* ICNSS_QMI */
#endif /* __WLFW_QMI_CLIENT_H__ */
