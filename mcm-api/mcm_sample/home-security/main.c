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
#include <sys/select.h>
#include <pthread.h>
#include <stdlib.h>

#include "mcm_client.h"
#include "mcm_common_v01.h"
#include "mcm_voice_v01.h"

#include "homesecurity.h"
#include "config_file_handling.h"
#include "data_call_handler.h"
#include "client.h"

/* structure holding variables to be set on reading the configuration file */
home_security_config_struct home_security_config;

/* What mode is the system currently in? */
int currMode = SYSTEM_OFF;
int alarm_type = SYSTEM_OFF;

/* If the system is currently counting down or not */
int countdownActive = FALSE;

/* Which countdown thread is active */
int threadNum = 0;

/* Is the call active? */
int callActive = FALSE;

/* MCM variables for voice calls */
mcm_client_handle_type voice_hndl_dial;
mcm_client_handle_type voice_hndl_hang_up;
mcm_voice_dial_req_msg_v01 dial_req;
mcm_voice_dial_resp_msg_v01* dial_rsp;
mcm_voice_hangup_req_msg_v01 hang_up_req;
mcm_voice_hangup_resp_msg_v01* hang_up_rsp;
int voice_token_id = 0;
uint32_t call_id;

/* Indication callback for voice */
void voice_ind_cb(
  mcm_client_handle_type   voice_hndl,
  uint32_t                 msg_id,
  void                    *ind_c_struct,
  uint32_t                 ind_len)
{
   printf("voice_ind_cb ENTER\n");
   printf("voice_ind_cb EXIT\n");
}

/* Asynchronous callback for voice */
void voice_async_cb(
  mcm_client_handle_type   voice_hndl,
  uint32_t                 msg_id,
  void                    *resp_c_struct,
  uint32_t                 resp_len,
  void                    *voice_token_id)
{
   printf("voice_async_cb ENTER\n");
   switch(msg_id)
	{
		/* The callback after dialing */
		case MCM_VOICE_DIAL_RESP_V01:

			dial_rsp = (mcm_voice_dial_resp_msg_v01*)resp_c_struct;

			if(dial_rsp->response.result != MCM_RESULT_SUCCESS_V01)
			{
				printf("Voice call failed.Error code: %d\n", dial_rsp->response.error);
				callActive = FALSE;
			}

			if(dial_rsp->call_id_valid)
			{
				printf("Valid Call ID = %d \n", dial_rsp->call_id);
				call_id = dial_rsp->call_id;
				callActive = TRUE;
			}
			else
			{
				printf("Invalid Call ID = %d \n", dial_rsp->call_id);
				printf("Call ID Validity = %d \n", dial_rsp->call_id_valid);
				callActive = FALSE;
			}
			break;
		/* The callback after hanging up */
		case MCM_VOICE_HANGUP_RESP_V01:
			hang_up_rsp = (mcm_voice_hangup_resp_msg_v01*)resp_c_struct;
			if(hang_up_rsp->response.result != MCM_RESULT_SUCCESS_V01)
			{
				printf("Failure hanging up call. Error code: %d\n", hang_up_rsp->response.error);
			}
			else
			{
				printf("Voice call ended.\n");
				callActive = FALSE;
			}
	}
   printf("voice_async_cb EXIT\n");
}

/* Sends a string to a server notifying the alarm has been sounded */
void soundAlarm(void)
{
	printf("Alarm has been sounded.\n");
	/* Send alarm notification to server */
	if(alarm_type == SYSTEM_DOOR){
		send_string(home_security_config.host_name, "ALARM! A door was opened!");
	}
	else if(alarm_type == SYSTEM_MOTION){
		send_string(home_security_config.host_name, "ALARM! Motion detector activated!");
	}
	else{
		send_string(home_security_config.host_name, "The alarm is going off!");
	}
}

/* Counts down after an event has occured to give time for the user to enter their code. */
void *doCountdown(void* threadID)
{
	/* Wakes up after a specified time to see if the alarm code was entered */
	struct timeval t;

	int * ti = (int*)threadID;

	t.tv_sec = home_security_config.wait_time;
	t.tv_usec = 0;

	/* Performs the actual blocking wait */
	select(0, NULL, NULL, NULL, &t);

	if(countdownActive == TRUE && threadNum == *ti){
		soundAlarm();
		countdownActive = FALSE;
	}else{
		printf("Code entered before countdown completed.\n");
	}

	return 0;
}

/* Start the countdown for the alarm after an event occurs */
void startCountdown(int alarmReason)
{
	if(countdownActive == TRUE){
		printf("Another event occured, but the countdown was already active!\n");
	}else{
		printf("Code must be entered within %d seconds\n", home_security_config.wait_time);
		alarm_type = alarmReason;
		threadNum++;
		countdownActive = TRUE;
		pthread_t thread;
		void * ptr = malloc(sizeof(int));
		if(ptr == NULL){
			return;
		}
		*((int*)ptr) = threadNum;
		pthread_create(&thread, NULL, doCountdown, ptr);
	}
}

/* Calls 911 (in our case a number in the config file) */
void panicButton(void)
{
	if(callActive){
		printf("Call to 911 already active!\n");
	}
	printf("Panic Button Pressed! Calling 911!\n");

	memset(&voice_hndl_dial, 0, sizeof(voice_hndl_dial));
	int init_result=mcm_client_init(&voice_hndl_dial, voice_ind_cb, voice_async_cb);

	dial_req.address_valid=1;
	strncpy(dial_req.address,home_security_config.phone_number, MCM_MAX_PHONE_NUMBER_V01 + 1);

	dial_req.call_type_valid = 1;
	dial_req.call_type = MCM_VOICE_CALL_TYPE_VOICE_V01;

	dial_req.uusdata_valid = 0;

	dial_rsp = malloc(sizeof(mcm_voice_dial_resp_msg_v01));
	if(dial_rsp == NULL){
		return;
	}
	memset(dial_rsp, 0, sizeof(mcm_voice_dial_resp_msg_v01));

	MCM_CLIENT_EXECUTE_COMMAND_ASYNC(voice_hndl_dial, MCM_VOICE_DIAL_REQ_V01, &dial_req, dial_rsp, voice_async_cb, &voice_token_id);
}

/* Hangs up our voice call */
void dontPanic(void){
	memset(&voice_hndl_hang_up, 0, sizeof(voice_hndl_hang_up));
	mcm_client_init(&voice_hndl_hang_up, voice_ind_cb, voice_async_cb);
	hang_up_req.call_id = call_id;
	hang_up_rsp = malloc(sizeof(mcm_voice_hangup_resp_msg_v01));
	if(hang_up_rsp == NULL){
		return;
	}
	memset(hang_up_rsp, 0, sizeof(mcm_voice_hangup_resp_msg_v01));
	MCM_CLIENT_EXECUTE_COMMAND_ASYNC(voice_hndl_hang_up, MCM_VOICE_HANGUP_REQ_V01, &hang_up_req, hang_up_rsp, voice_async_cb, &voice_token_id);
}

/* Validates the security code */
void validateCode(void)
{
	printf("Code entered is valid. Alarm cancelled.\n");
	countdownActive = FALSE;
}

int main()
{
	/* Name of the configuration file to be read */
	char config_filename1[]="home_security_config.txt";

	/* call function to read config file */
	config_file_read_intialize_variables_home_security(config_filename1,&home_security_config);

	printf("Server ID read = %s\n", home_security_config.host_name);
	printf("Phone Number = %s\n", home_security_config.phone_number);
	printf("Wait time = %d\n", home_security_config.wait_time);
	printf("Port = %d\n", home_security_config.port);

	int pipe_fd;		// file descriptor for fifo
	char buf[MAX_BUF];	// buffer to store read data in
	int read_count;		//numbef of characters read

	printf("Home Security: System turning on\n");

	/* delete the fifo if it already exists */
	printf("Unlinking\n");
	if(unlink(FIFO_NAME) == -1) {
		if (errno != ENOENT) {
            printf("unlink failed: %s\n", strerror(errno));
        }
        /* if the file does not exist to be deleted, that is ok */
    }

	/* create fifo with rw for owner and group */
	printf("mknodding\n");
	if(mknod(FIFO_NAME, S_IFIFO | 0660,0) == -1) {
		//if(mkfifo(FIFO_NAME, 0666) == -1) {
        printf("mknod failed: %s\n", strerror(errno));
        return 1;
    }

    /* set fifo to user rw, group w */
    printf("chmodding\n");
	if(chmod(FIFO_NAME, 0620) == -1) {
		printf("chmod failed: %s\n", strerror(errno));
		return 1;
	}

	/* Initialize MCM data service */
	int data_call_result = set_up_call(0,"" ,"" ,"",0,0,0);

	if(!data_call_result){
		printf("Data call failed. Exiting\n");
		stop_data_call();
		return 0;
	}

	/* read and open forever */
	while(1) {

		/* open fifo for reading */
		//printf("opening\n");
		pipe_fd = open(FIFO_NAME, O_RDONLY);
		if(pipe_fd == -1) {
			printf("open failed: %s\n", strerror(errno));
			stop_data_call();
			return 1;
		}

		/* read until an error or halted by user */
		while(1) {

			/* reset the buffer to prevent matching multiple times */
			memset(buf,0,MAX_BUF);

			/* blocking read from fifo */
			//printf("about to read\n");
			read_count = read(pipe_fd,buf,MAX_BUF-1);
			if(read_count <= 0) {
				//printf("read failed: %s\n", strerror(errno));
				break;
			}

			//printf("read: read_count %d, read: %s\n",read_count,buf);

			if(strncmp(buf,MODE_OFF,strlen(MODE_OFF)) == 0) {
				currMode = SYSTEM_OFF;
				printf("System is now on mode off\n");
			}else if(strncmp(buf,MODE_STAY,strlen(MODE_STAY)) == 0) {
				currMode = SYSTEM_STAY;
				printf("System is now on mode stay\n");
			}else if(strncmp(buf,MODE_AWAY,strlen(MODE_AWAY)) == 0) {
				currMode = SYSTEM_AWAY;
				printf("System is now on mode away\n");
			}else if(strncmp(buf,EVENT_DOOR,strlen(EVENT_DOOR)) == 0) {
				if(currMode == SYSTEM_STAY || currMode == SYSTEM_AWAY){
					printf("Door was opened. Please enter code!\n");
					startCountdown(SYSTEM_DOOR);
				}
			}else if(strncmp(buf,EVENT_MOTION,strlen(EVENT_MOTION)) == 0) {
				if(currMode == SYSTEM_AWAY){
					printf("Something moved. Please enter code!\n");
					startCountdown(SYSTEM_MOTION);
				}
			}else if(strncmp(buf,EVENT_PANIC,strlen(EVENT_PANIC)) == 0) {
				panicButton();
			}else if(strncmp(buf,EVENT_DONT_PANIC,strlen(EVENT_DONT_PANIC)) == 0) {
				dontPanic();
			}else if(strncmp(buf,EVENT_CODE,strlen(EVENT_CODE)) == 0) {
				validateCode();
			}else if(strncmp(buf,MODE_SHUTDOWN,strlen(MODE_SHUTDOWN)) == 0) {
				printf("Home Security: Shutting down system\n");

				/* close FIFO */
				printf("closing fifo\n");
				close(pipe_fd);
				stop_data_call();
				return 0;
			}
		}

		/* close FIFO */
		//printf("closing fifo\n");
		close(pipe_fd);
	}

	stop_data_call();
}
