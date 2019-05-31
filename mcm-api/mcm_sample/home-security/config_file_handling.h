/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MCM_MAX_PHONE_NUMBER_V01 82 /* taken from mcm_common_v01.h */
#define MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01 255 /* taken from  mcm_loc_v01.h (Do we really need 255 for server address?) */

#define ARRAY_SIZE_VALUES_SMART_METER 10

typedef struct
{
	char host_name[MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01 + 1];
	char phone_number[MCM_MAX_PHONE_NUMBER_V01+1];
	int wait_time;
	int port;
} home_security_config_struct;

void config_file_read_intialize_variables_home_security(char *,home_security_config_struct *);