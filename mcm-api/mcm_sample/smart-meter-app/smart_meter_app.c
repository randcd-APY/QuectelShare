/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif
#include "smart_meter_app.h"

/* variables used for shared memory segment */
int shmid;
char *shm;
key_t key;
key = 2601;

/* variables for send sms and voice call via MCM  */
mcm_client_handle_type hndl;

mcm_sms_send_mo_msg_req_msg_v01 sms_req;
mcm_sms_send_mo_msg_resp_msg_v01* sms_rsp;

mcm_voice_dial_req_msg_v01 dial_req;
mcm_voice_dial_resp_msg_v01* dial_rsp;

mcm_voice_hangup_req_msg_v01 hang_up_req;
mcm_voice_hangup_resp_msg_v01* hang_up_rsp;

uint32_t call_id =0;
int token_id = 0;
int voice_call_result = MAKE_CALL_FAILURE;
int send_sms_result = SEND_SMS_FAILURE;

sem_t sem_wait_for_callback;

/* indication callback function */
void ind_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *ind_c_struct,
  uint32_t                 ind_len)
{
    printf("**** ind_cb ENTER **** \n");
    printf("**** ind_cb EXIT **** \n");
}

/* asynchronous callback function */
void async_cb(
  mcm_client_handle_type   hndl,
  uint32_t                 msg_id,
  void                    *resp_c_struct,
  uint32_t                 resp_len,
  void                    *token_id)
{
    switch(msg_id)
    {

        case MCM_SMS_SEND_MO_MSG_RESP_V01:
            sms_rsp = (mcm_sms_send_mo_msg_resp_msg_v01*)resp_c_struct;
            printf("**** Async callback received for send sms call msg_id=%d, token_id = %d **** \n",msg_id,*(int*)token_id);

            if(sms_rsp->response.result != MCM_RESULT_SUCCESS_V01)
            {
                printf("**** Send SMS failed.Error code: %d **** \n", sms_rsp->response.error);
                send_sms_result=SEND_SMS_FAILURE;
            }
            else
            {
                send_sms_result=SEND_SMS_SUCCESS;
            }
            break;

        case MCM_VOICE_DIAL_RESP_V01:
            dial_rsp = (mcm_voice_dial_resp_msg_v01*)resp_c_struct;
            printf("**** Async callback received for voice dial call msg_id=%d, token_id = %d **** \n",msg_id,*(int*)token_id);

            if(dial_rsp->response.result != MCM_RESULT_SUCCESS_V01)
            {
                printf("**** Voice call failed.Error code: %d **** \n", dial_rsp->response.error);
                voice_call_result=MAKE_CALL_FAILURE;
            }

            if(dial_rsp->call_id_valid)
            {
                printf("**** Valid Call ID = %d **** \n", dial_rsp->call_id);
                call_id = dial_rsp->call_id;
                voice_call_result=MAKE_CALL_SUCCESS;
            }
            else
            {
                printf("**** Invalid Call ID = %d. validity flag = %d ****\n", dial_rsp->call_id,dial_rsp->call_id_valid);
                voice_call_result=MAKE_CALL_FAILURE;
            }
            break;

        case MCM_VOICE_HANGUP_RESP_V01:
            hang_up_rsp = (mcm_voice_hangup_resp_msg_v01*)resp_c_struct;
            printf("**** Async callback received for voice hang up call msg_id=%d, token_id = %d ****\n",msg_id,*(int*)token_id);

            if(hang_up_rsp->response.result != MCM_RESULT_SUCCESS_V01)
            {
                printf("**** Failure hanging up call. Error code: %d ****\n", hang_up_rsp->response.error);
            }
            else
            {
                printf("**** Call hang-up done **** \n");
            }
            break;

        default:
            printf("**** Unknown callback response **** \n");
            break;

     }

     sem_post(&sem_wait_for_callback);    /* release semaphore */
     printf("**** async_cb end **** \n");
}

void send_sms(int smart_meter_reading)
{
    printf("\n**** Sending SMS value = %d ****\n",smart_meter_reading);
    char message_content[MESSAGE_SIZE];
    snprintf(message_content,MESSAGE_SIZE,"%d",smart_meter_reading);
    strlcpy(sms_req.message_content, message_content, MCM_SMS_MAX_MO_MSG_LENGTH_V01 + 1);

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_SMS_SEND_MO_MSG_REQ_V01, &sms_req, sms_rsp, async_cb, &token_id);
    sem_wait(&sem_wait_for_callback);   /* wait for the callback */
}

int set_alarm()
{
    printf("\n**** Setting house alarm **** \n\n");
    return ALARM_SUCCESS;
}

void call_gas_company()
{
   printf("\n**** Calling gas company **** \n");

   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_VOICE_DIAL_REQ_V01, &dial_req, dial_rsp, async_cb, &token_id);
   sem_wait(&sem_wait_for_callback);    /* wait for the callback */
}

void hang_up_call()
{
    hang_up_req.call_id = call_id;
    printf("**** Hanging up call..call_id=%d **** \n",hang_up_req.call_id);

    MCM_CLIENT_EXECUTE_COMMAND_ASYNC(hndl, MCM_VOICE_HANGUP_REQ_V01, &hang_up_req, hang_up_rsp, async_cb, &token_id);
    sem_wait(&sem_wait_for_callback);    /* wait for the callback */
}

void send_meter_reading_to_server(smart_meter_values_struct smart_meter_values)
{
    printf("\n**** Inside function to send smart meter readings to the server at specific time interval ****\n");
    int delay_interval;
    int i;

    for (i=0;i<ARRAY_SIZE_VALUES_SMART_METER;i++)
    {
        if(smart_meter_values.meter_reading_static_values_arr[i]==-1)    /* -1 value indicates gas leak */
        {
            printf("\n**** Gas Leak detected **** \n");
            int alarm_result=set_alarm();    /* call function to set alarm */

            if (alarm_result==ALARM_SUCCESS)
                printf("**** In house alarm setting SUCCESS ****  \n");
            else if (alarm_result==ALARM_FAILURE)
                printf("**** In house alarm setting FAILED **** \n");

            call_gas_company();    /* call function to make a call to the gas company */

            if (voice_call_result==MAKE_CALL_SUCCESS)
            {
                printf("**** Call to gas company SUCCESS **** \n");
                printf("**** Waiting 20 seconds before hanging up the call **** \n");
                sleep(20);    /* wait for sometime before hanging up call */
                hang_up_call();
            }
            else if (voice_call_result==MAKE_CALL_FAILURE)
            {
                printf("**** Call to gas company FAILED **** \n");
            }
        }

        send_sms(smart_meter_values.meter_reading_static_values_arr[i]);    /* call function to send SMS with text as the smart meter reading */
        if(send_sms_result==SEND_SMS_SUCCESS)    /* SMS sent successfully as async callback received and semaphore is released */
        {
            printf("\n**** SMS SENT.. ****\n");
            if(i!=ARRAY_SIZE_VALUES_SMART_METER-1)
            {
                delay_interval=atoi(shm);    /* value of time interval read from shared memory */
                printf("\n**** Wait for %d seconds before sending next SMS ****\n",delay_interval);
                sleep(delay_interval);    /* sleep for certain seconds before sending next SMS */
            }
        }
        else if(send_sms_result==SEND_SMS_FAILURE)
        {
            printf("\n**** Problem sending SMS ****\n");
        }
     }
     printf("**** All smart meter readings sent ****\n\n");

}

int initialize_mcm_client()
{
    memset(&hndl, 0, sizeof(hndl));
    int init_result=mcm_client_init(&hndl, ind_cb, async_cb);    /* Initialize MCM client */

    return init_result;
}

void setup_voice_sms(smart_meter_config_struct smart_meter_config)
{
    printf("**** Inside voice call and SMS set-up function. ****\n");

    memset(&hndl, 0, sizeof(hndl));
    int init_result=mcm_client_init(&hndl, ind_cb, async_cb);    /* Initialize MCM client */

    /* Set-up for SMS */
    sms_req.message_format = 1;
    strlcpy(sms_req.destination, smart_meter_config.server, MCM_SMS_MAX_ADDR_LENGTH_V01 + 1);

    sms_rsp = malloc(sizeof(mcm_sms_send_mo_msg_resp_msg_v01));
    if(sms_rsp!=0)
    {
        memset(sms_rsp, 0, sizeof(mcm_sms_send_mo_msg_resp_msg_v01));
    }
    /* Set-up for voice */
    dial_req.address_valid=1;
    strlcpy(dial_req.address,smart_meter_config.emergency, MCM_MAX_PHONE_NUMBER_V01 + 1);
    dial_req.call_type_valid = 1;
    dial_req.call_type = MCM_VOICE_CALL_TYPE_VOICE_V01;
    dial_req.uusdata_valid = 0;

    dial_rsp = malloc(sizeof(mcm_voice_dial_resp_msg_v01));
    if(dial_rsp!=0)
    {
        memset(dial_rsp, 0, sizeof(mcm_voice_dial_resp_msg_v01));
    }
    
    hang_up_rsp = malloc(sizeof(mcm_voice_hangup_resp_msg_v01));
    if(hang_up_rsp!=0)
    {
        memset(hang_up_rsp, 0, sizeof(mcm_voice_hangup_resp_msg_v01));
    }
    sem_init(&sem_wait_for_callback, 0, 0);    /* initialize semaphore */
}

void tear_down()
{
    int release_result=mcm_client_release(hndl);
    if(release_result!=0)
    {
        printf ("**** In tear_down function.Problems releasing MCM client handle ****\n");
    }
    printf("**** MMCM client hndl released.. Program end****\n");
    free(sms_rsp);
    free(dial_rsp);
    free(hang_up_rsp);
}

void create_shared_memory_segment(smart_meter_config_struct smart_meter_config)
{
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)    /* Create the segment */
    {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)     /* attach the segment to our data space */
    {
        perror("shmat");
        exit(1);
    }
    snprintf(shm,sizeof(shm),"%d",smart_meter_config.time_interval);    /* Put time interval into the memory */

    printf ("**** Shared memory segment created.. Has time interval = %d seconds read from configuration file ****\n",atoi(shm));
}

int main()
{
    char config_filename[]="smart_meter_config.txt";    /* Name of the configuration file to be read */

    smart_meter_config_struct smart_meter_config;    /* structure holding variables to be set on reading the configuration file */
    config_file_read_intialize_variables_smart_meter(config_filename,&smart_meter_config); /* call function to read configuration file */

    printf ("**** Phone Number for server= %s ****\n",smart_meter_config.server);
    printf ("**** Phone Number for emergency (call for gas leak)= %s ****\n",smart_meter_config.emergency);
    printf ("**** Time Interval = %d (seconds)****\n",smart_meter_config.time_interval);

    char text_filename[]="smart_meter_values_text.txt";    /* TEXT file (having static values of meter reading) */
    smart_meter_values_struct smart_meter_values;    /* structure holding values on reading text file */

    file_read_static_values_smart_meter(text_filename,&smart_meter_values);    /* call function to read the text file */

    printf("**** Smart meter values read from text file **** \n");
    int i;
    for (i=0;i<ARRAY_SIZE_VALUES_SMART_METER;i++)
    {
        printf ("%d\n" ,smart_meter_values.meter_reading_static_values_arr[i]);
    }
    printf("\n--------------------------------------------------------------------\n");

    create_shared_memory_segment(smart_meter_config);    /* call function to create a shared memory and write the time interval read from configuration file */

    int mcm_init_result=initialize_mcm_client();    /* call function to Initialize MCM client */
    if(mcm_init_result==0)    /* mcm_client_init returns 0 on success */
    {
        printf("**** MCM client init success. Sending smart meter readings ****\n");
        setup_voice_sms(smart_meter_config);    /* call function to set-up request and response objects for voice and SMS */
        send_meter_reading_to_server(smart_meter_values);    /* call function to send meter readings to server */
        tear_down();    /* Release MCM client handle */
    }
    else
    {
        printf("**** MCM client init failed so voice call,SMS functionality won't work ****\n");
    }

    return 0;
}
