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
#include "mcm_loc_v01.h"
#include "gps_handler.h"

/* Handler and request/response variables */
mcm_client_handle_type hndl;
mcm_loc_set_indications_resp_msg_v01 *rsp;
mcm_loc_set_position_mode_resp_msg_v01 *set_position_rsp;
mcm_loc_start_nav_resp_msg_v01 *start_nav_rsp;
mcm_loc_stop_nav_resp_msg_v01 *stop_nav_rsp;

int gps_token_id = 0;

sem_t sem_wait_for_callback;

sem_t sem_wait_for_loc_indication;

/* Callback that periodically reports the GPS location */
void gps_ind_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *ind_c_struct,
  uint32_t                 ind_len)
{
    printf("**** gps_ind_cb start msg_id = %d ****\n",msg_id);
    mcm_loc_location_info_ind_msg_v01 *ind = (mcm_loc_location_info_ind_msg_v01*)ind_c_struct;

    mcm_gps_location_t_v01 *mylocation;
    mylocation = (mcm_gps_location_t_v01*)&ind->location;

    printf("**** Latitude = %f ****\n",mylocation->latitude);
	lat = mylocation->latitude;
    printf("**** Longitude = %f ****\n",mylocation->longitude);
	lon = mylocation->longitude;
    printf("**** Accuracy= %f ****\n",mylocation->accuracy);
    printf("**** gps_ind_cb end ****\n");

    sem_post(&sem_wait_for_loc_indication);
}

/* Callback for starting and ending services */
void gps_async_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *resp_c_struct,
  uint32_t                 resp_len,
  void                    *gps_token_id)
{
    printf("**** gps_async_cb start msg_id = %d ****\n",msg_id);
    switch(msg_id)
    {
        case MCM_LOC_SET_INDICATIONS_RESP_V01:
            printf("**** gps_async_cb received for set location indications ****\n");
            break;

        case MCM_LOC_SET_POSITION_MODE_RESP_V01:
            printf("**** gps_async_cb received for set position mode ****\n");
            break;

        case MCM_LOC_START_NAV_RESP_V01:
            printf("**** gps_async_cb received for start navigation ****\n");
            break;

        case MCM_LOC_STOP_NAV_RESP_V01:
            printf("**** gps_async_cb received for stop navigation ****\n");
            break;
    }
    printf("**** gps_async_cb end ****\n");
    sem_post(&sem_wait_for_callback);    /* release semaphore */
}

/* Set what we want callbacks for */
void set_indications()
{
    printf("**** Inside function for set location indications ****\n");
    mcm_loc_set_indications_req_msg_v01 req;
    rsp = malloc(sizeof(mcm_loc_set_indications_resp_msg_v01));
    memset(rsp, 0, sizeof(mcm_loc_set_indications_resp_msg_v01));

    // Set Indications
    req.register_location_info_ind = 1;
    req.register_status_info_ind = 0;
    req.register_sv_info_ind = 0;
    req.register_nmea_info_ind = 0;
    req.register_capabilities_info_ind = 0;
    req.register_utc_time_req_ind = 0;
    req.register_xtra_data_req_ind = 0;
    req.register_agps_data_conn_cmd_req_ind = 0;
    req.register_ni_notify_user_response_req_ind = 0;

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_LOC_SET_INDICATIONS_REQ_V01, &req, rsp,gps_async_cb, &gps_token_id);
    sem_wait(&sem_wait_for_callback);
}

void set_position_mode()
{
    printf("**** Inside function for set position mode ****\n");
    mcm_loc_set_position_mode_req_msg_v01 set_position_req;

    set_position_rsp = malloc(sizeof(mcm_loc_set_position_mode_resp_msg_v01));
    memset(set_position_rsp, 0, sizeof(mcm_loc_set_position_mode_resp_msg_v01));

    /* verify below values */
    set_position_req.mode=0;
    set_position_req.recurrence=0;
    set_position_req.min_interval=60;
    set_position_req.preferred_accuracy=0;
    set_position_req.preferred_time=0;

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_LOC_SET_POSITION_MODE_REQ_V01, &set_position_req, set_position_rsp,gps_async_cb, &gps_token_id);
    sem_wait(&sem_wait_for_callback);
}

void start_navigation()
{
    printf("**** Inside function for start navigation ****\n");
    mcm_loc_start_nav_req_msg_v01 start_nav_req;

    start_nav_rsp = malloc(sizeof(mcm_loc_start_nav_resp_msg_v01));
    memset(start_nav_rsp, 0, sizeof(mcm_loc_start_nav_resp_msg_v01));

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_LOC_START_NAV_REQ_V01, &start_nav_req, start_nav_rsp,gps_async_cb, &gps_token_id);
    sem_wait(&sem_wait_for_callback);
}

void stop_navigation()
{
    printf("**** Inside function for stop navigation ****\n");
    mcm_loc_start_nav_req_msg_v01 stop_nav_req;

    stop_nav_rsp = malloc(sizeof(mcm_loc_stop_nav_resp_msg_v01));
    memset(stop_nav_rsp, 0, sizeof(mcm_loc_stop_nav_resp_msg_v01));

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_LOC_STOP_NAV_REQ_V01, &stop_nav_req, stop_nav_rsp,gps_async_cb, &gps_token_id);
    sem_wait(&sem_wait_for_callback);
}

/* Set up GPS connection and start getting coordinates */
int get_current_location()
{
    memset(&hndl, 0, sizeof(hndl));
    int init_result=mcm_client_init(&hndl, gps_ind_cb, gps_async_cb);

    sem_init(&sem_wait_for_callback, 0, 0);    /* initialize semaphore */
    sem_init(&sem_wait_for_loc_indication, 0, 0);    /* initialize semaphore */

    if (init_result == MCM_RESULT_SUCCESS_V01)
    {
        printf ("MCM client init is successful so making sync calls\n\n");

        set_indications();    /* 1. Register for indications (Location) */

        set_position_mode();   /* 2. Set position mode */

        start_navigation();    /* 3. Start navigation */

        sem_wait(&sem_wait_for_loc_indication);

        stop_navigation();    /* 4. Stop navigation */

		return GPS_SUCCESS;
    }

    printf ("MCM client init failed\n");
	return GPS_FAIL;
}

/* Stop getting coordinates */
int gps_tear_down()
{
    printf("Inside gps_tear_down function\n");
    int release_result=mcm_client_release(hndl);
    if(release_result != MCM_RESULT_SUCCESS_V01)
    {
        printf (" In gps_tear_down function.Problems releasing client handle\n");
		return GPS_FAIL;
    }
	return GPS_SUCCESS;
}