/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#define MAKE_CALL_SUCCESS 1
#define MAKE_CALL_FAILURE 0

int set_up_call(int8_t ip_family,char *apn_name,char *user_name,char *password,int8_t tech_pref,int8_t umts_profile,int8_t cdma_profile);
int stop_data_call(void);