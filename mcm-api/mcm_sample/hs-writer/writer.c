/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "homesecurity.h"

int main() {
    int pipe_fd = -1;
    char buf[MAX_BUF];
    int write_count = -1;

	while(1){

		int menuOption = 0;

		do{
			printf("\nHome Security System: Select an Option:\n");
			printf("  1. Set Mode Off\n");
			printf("  2. Set Mode Stay\n");
			printf("  3. Set Mode Away\n");
			printf("  4. Open a Door\n");
			printf("  5. Trigger Motion Sensor\n");
			printf("  6. Press Panic Button\n");
			printf("  7. Hang up call\n");
			printf("  8. Enter Security code\n");
			printf("  9. Shutdown system\n");
			printf(" 10. Exit Menu\n");

			char in[5];
			fgets(in, 5, stdin);
			menuOption = atoi(in);
		}while(menuOption < 0 || menuOption > 10);

		/* Open connection to the security app */
		pipe_fd = open(FIFO_NAME, O_WRONLY);
		if (pipe_fd == -1) {
			return 0;
		}

		memset(buf, 0, MAX_BUF);
		switch(menuOption){
			case 1:
				printf("Setting Mode to Off\n");
				strncpy(buf, MODE_OFF, strlen(MODE_OFF));
				break;
			case 2:
				printf("Setting Mode to Stay\n");
				strncpy(buf, MODE_STAY, strlen(MODE_STAY));
				break;
			case 3:
				printf("Setting Mode to Away\n");
				strncpy(buf, MODE_AWAY, strlen(MODE_AWAY));
				break;
			case 4:
				printf("Opening a door...\n");
				strncpy(buf, EVENT_DOOR, strlen(EVENT_DOOR));
				break;
			case 5:
				printf("Something is moving...\n");
				strncpy(buf, EVENT_MOTION, strlen(EVENT_MOTION));
				break;
			case 6:
				printf("Pressing the Panic Button...\n");
				strncpy(buf, EVENT_PANIC, strlen(EVENT_PANIC));
				break;
			case 7:
				printf("Hanging up the call...\n");
				strncpy(buf, EVENT_DONT_PANIC, strlen(EVENT_DONT_PANIC));
				break;
			case 8:
				printf("Entering Security Code...1...2...3...4\n");
				strncpy(buf, EVENT_CODE, strlen(EVENT_CODE));
				break;
			case 9:
				printf("Goodbye\n");
				strncpy(buf, MODE_SHUTDOWN, strlen(MODE_SHUTDOWN));
				break;
			case 10:
				return 1;
		}
		strcat(buf, "\0");

		write_count = write(pipe_fd, buf, strlen(buf));
		if (write_count < strlen(buf)) {
			return 0;
		}

		/* Close connection to the file. */
		close(pipe_fd);

		if(menuOption == 9){
			break;
		}
	}
	return 1;
}