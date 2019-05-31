/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdio.h>
#include <stdlib.h>

#include "packagetracking.h"
#include "client.h"
#include "data_call_handler.h"
#include "gps_handler.h"

double lat = UNKNOWN_LOC;
double lon = UNKNOWN_LOC;

/* Write to a file for our server to read */
void sendToServer(void)
{
	if(lat == UNKNOWN_LOC || lon == UNKNOWN_LOC){
		printf("Location is unknown.\n");
		return;
	}
	char buf[MAXDATASIZE];
	sprintf(buf, "%f, %f", lat, lon);
	send_string("10.45.146.243", buf);
}

int main(void)
{
	/* Report the location periodically forever */
	int data_call_result = set_up_call(0,"" ,"" ,"",0,0,0);

	if(!data_call_result){
		printf("Data call failed. Exiting\n");
		stop_data_call();
		return 0;
	}

	get_current_location();

	printf("About to start sending data!\n");

	while(1){
		sendToServer();

		struct timeval t;

		t.tv_sec = updateTime;
		t.tv_usec = 0;

		/* Perform a blocking wait for the specified time */
		select(0, NULL, NULL, NULL, &t);
	}

	gps_tear_down();
	stop_data_call();
}