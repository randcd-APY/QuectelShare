/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>

#include "mcm_client.h"
#include "mcm_common_v01.h"
#include "mcm_data_v01.h"
#include "data_call_handler.h"

/* Variables for MCM messages */
mcm_client_handle_type hndl;
mcm_data_start_data_call_rsp_msg_v01* rsp;
mcm_data_stop_data_call_rsp_msg_v01* stop_rsp;

/* MCM variables for call state and id */
uint32_t call_id =0;
int call_result = MAKE_CALL_SUCCESS;
int token_id = 0;

/* Allows us to wait for the result before continuing on */
sem_t sem_wait_for_callback;

/* Should not be used, but needed by the MCM API */
void ind_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *ind_c_struct,
  uint32_t                 ind_len)
{
    printf("**** ind_cb ENTER ****\n\n");
    printf("**** ind_cb EXIT ****\n");
}

/* Callback for all of our API calls */
void async_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *resp_c_struct,
  uint32_t                 resp_len,
  void                    *token_id)
{
    switch(msg_id)
    {
		// Result of starting the data call
        case MCM_DATA_START_DATA_CALL_RSP_V01:
            printf("**** Async callback received for start data call msg_id=%d, token_id = %d **** \n",msg_id,*(int*)token_id);
            rsp = (mcm_data_start_data_call_rsp_msg_v01*)resp_c_struct;

            if(!rsp->call_id_valid)
            {
                printf("**** Invalid Call Status ****\n");
                call_result = MAKE_CALL_FAILURE;
            }
            if(rsp->vce_reason_valid)
            {
                printf("**** Call Ended Abnormally.Reason ID: %d ****", rsp->vce_reason.call_end_reason_code);
                call_result = MAKE_CALL_FAILURE;
            }
            call_id = rsp->call_id;
            printf (" **** Call Id = %d.. Validity flag = %d **** \n", call_id, rsp->call_id_valid);
            break;
		// Result of ending the data call
        case MCM_DATA_STOP_DATA_CALL_RSP_V01:
            printf("**** Async callback received for stop data call msg_id=%d, token_id = %d **** \n",msg_id,*(int*)token_id);
            stop_rsp = (mcm_data_stop_data_call_rsp_msg_v01*)resp_c_struct;
            printf("**** Data call stopped ****\n");
            break;
		// Something unexpected happened
        default:
            printf("**** Unknown callback response.. ****\n");
            break;
    }
	/* release semaphore */
    sem_post(&sem_wait_for_callback);
    printf("**** async_cb end ****\n");
}

/* Need to add route for data to make it out to the world */
int add_route()
{
    char cmd[50];
    strlcpy(cmd, "ip route add default dev rmnet0", sizeof(cmd));
    printf("\n**** Command to add route = %s ****\n", cmd);

    return system(cmd);
}

/* Set up the data call */
int set_up_call(
  int8_t 	ip_family,
  char 		*apn_name,
  char 		*user_name,
  char 		*password,
  int8_t 	tech_pref,
  int8_t 	umts_profile,
  int8_t 	cdma_profile)
{
    printf("**** Inside data call set-up function\n ****");

	// Initialize our handler
    memset(&hndl, 0, sizeof(hndl));
    int init_result=mcm_client_init(&hndl, ind_cb, async_cb);

	// Set up all the needed variables in our request
    mcm_data_start_data_call_req_msg_v01 req;
    req.ip_family_valid=1;
    req.ip_family = ip_family;
    req.apn_name_valid=1;
    strlcpy(req.apn_name, apn_name, MCM_DATA_MAX_APN_LEN_V01);
    req.user_name_valid=1;
    strlcpy(req.user_name, user_name, MCM_DATA_MAX_USERNAME_LEN_V01);
    req.password_valid=1;
    strlcpy(req.password, password, MCM_DATA_MAX_PASSWORD_LEN_V01);
    req.tech_pref_valid = 1;
    req.tech_pref = tech_pref;
    req.umts_profile_valid = 1;
    req.umts_profile = umts_profile;
    req.cdma_profile_valid = 1;
    req.cdma_profile = cdma_profile;

	/* allocate memory for start data call response message */
    rsp = malloc(sizeof(mcm_data_start_data_call_rsp_msg_v01));
    if(rsp == NULL){
		return MAKE_CALL_FAILURE;
	}
	memset(rsp, 0, sizeof(mcm_data_start_data_call_rsp_msg_v01));

	/* allocate memory for stop data call response message */
    stop_rsp = malloc(sizeof(mcm_data_stop_data_call_rsp_msg_v01));
    if(stop_rsp == NULL){
		return MAKE_CALL_FAILURE;
	}
	memset(stop_rsp, 0, sizeof(mcm_data_stop_data_call_rsp_msg_v01));

	/* initialize semaphore */
    sem_init(&sem_wait_for_callback, 0, 0);

    if (init_result == MCM_RESULT_SUCCESS_V01)
    {
        printf ("**** MCM client init is successful.Starting data call *****\n\n");
        MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_DATA_START_DATA_CALL_REQ_V01, &req, rsp, async_cb, &token_id);
        sem_wait(&sem_wait_for_callback);    /* wait for the callback */

		/* function called to add route for data transfer */
        int add_route_result = add_route();
        if(add_route_result == -1)
        {
            printf("**** Problem adding route. Data transfer might not work *****\n");
        }
        else
        {
            printf("****Route added successfully. add_route_result = %d ****\n",add_route_result);
        }
    }

    else
    {
        printf ("**** MCM client init failed ****\n");
        call_result = MAKE_CALL_FAILURE;
    }

    return call_result;
}

/*  Release the handler */
void tear_down(mcm_client_handle_type hndl)
{
        int release_result=mcm_client_release(hndl);
        if(release_result!=0)
        {
                printf ("**** In tear_down function.Problems releasing client handle ****\n");
        }
        printf("**** Mcm client hndl released ****\n");
}

/* Stop the data call */
int stop_data_call(void)
{
    mcm_data_stop_data_call_req_msg_v01 stop_req;
    stop_req.call_id = call_id;
    printf("**** Call ID set inside stop data call request = %d ****\n",stop_req.call_id);

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_DATA_STOP_DATA_CALL_REQ_V01, &stop_req, stop_rsp, async_cb, &token_id);
    sem_wait(&sem_wait_for_callback);    /* wait on same semaphore */

    printf("**** Data call stopped successfully ****\n");
    tear_down(hndl);    /* releasing the handle */
}