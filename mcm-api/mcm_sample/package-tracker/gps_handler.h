/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#define GPS_SUCCESS 1
#define GPS_FAIL 0

#define UNKNOWN_LOC 1000.0

extern double lat;
extern double lon;

int get_current_location(void);
int gps_tear_down(void);