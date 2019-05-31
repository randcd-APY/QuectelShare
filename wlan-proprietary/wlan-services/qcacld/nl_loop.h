/**
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __NL_LOOP_H_
#define __NL_LOOP_H_

#include <qcacld/wlan_nlink_common.h>

typedef void (*nl_loop_ind_handler)(unsigned short type, void *user_data);

int nl_loop_init(void);
int nl_loop_deinit(void);
int nl_loop_register(int ind, nl_loop_ind_handler ind_handler, void *user_data);
int nl_loop_unregister(int ind);

int nl_loop_terminate(void);
int nl_loop_run(void);

#endif  /* __NL_LOOP_H_ */
