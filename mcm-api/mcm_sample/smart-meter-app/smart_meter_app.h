/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "mcm_client.h"
#include "mcm_sms_v01.h"
#include "mcm_common_v01.h"
#include "mcm_voice_v01.h"

#define ARRAY_SIZE_VALUES_SMART_METER 10

#define SEND_SMS_SUCCESS 1
#define SEND_SMS_FAILURE 0

#define MAKE_CALL_SUCCESS 1
#define MAKE_CALL_FAILURE 0

#define ALARM_SUCCESS 1
#define ALARM_FAILURE 0


#define SHMSZ 27
#define MESSAGE_SIZE 20

typedef struct
{
    int meter_reading_static_values_arr[ARRAY_SIZE_VALUES_SMART_METER];
}smart_meter_values_struct;

typedef struct
{
    char emergency[MCM_MAX_PHONE_NUMBER_V01+1];
    char server[MCM_MAX_PHONE_NUMBER_V01+1];
    int time_interval;
}smart_meter_config_struct;

void file_read_static_values_smart_meter(char *,smart_meter_values_struct *);
void config_file_read_intialize_variables_smart_meter(char *,smart_meter_config_struct *);

