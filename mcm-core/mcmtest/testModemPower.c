/******************************************************************************
  @file    testModemPower.c
  @brief

  DESCRIPTION


  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#include "mcm_client.h"
#include "mcm_dm_v01.h"


#define TRUE 1
#define FALSE 0

pthread_t client1_thread_id;
pthread_t client2_thread_id;

mcm_client_handle_type      client1_handle;
mcm_client_handle_type      client2_handle;

int response_received = FALSE;
int ind_received = FALSE;

typedef void (*mcm_client_async_cb)
(
  mcm_client_handle_type hndl,
  uint32_t                 msg_id,
  void                  *resp_c_struct,
  uint32_t                 resp_len,
  void                  *token_id
);



void mcm_set_power_resp_cb
(
   unsigned int       msg_id,
   void              *resp_c_struct,
   uint32_t             resp_len,
   void              *token_id
)
{
   printf("mcm_client2_async_cb msg_id - %d\n", msg_id);
   response_received = TRUE;
}


void mcm_client2_async_cb
(
   unsigned int       msg_id,
   void              *resp_c_struct,
   uint32_t             resp_len,
   void              *token_id
)
{
   printf("mcm_client2_async_cb msg_id - %d\n", msg_id);
   response_received = TRUE;
}

void mcm_client2_ind_cb
(
   unsigned int      msg_id,
   void             *ind_c_struct,
   uint32_t             ind_len
)
{
   printf("mcm_client2_ind_cb msg_id - %d\n", msg_id);
}

void mcm_client1_async_cb
(
   unsigned int       msg_id,
   void              *resp_c_struct,
   uint32_t             resp_len,
   void              *token_id
)
{
   printf("mcm_client1_async_cb msg_id - %d\n", msg_id);
   response_received = TRUE;
}

void mcm_client1_ind_cb
(
   unsigned int      msg_id,
   void             *ind_c_struct,
   uint32_t            ind_len
)
{
   printf("mcm_client1_ind_cb msg_id - %d\n", msg_id);
   ind_received = TRUE;
}

void mcm_test_dm_offline()

{
   int *token_ptr;
   mcm_dm_set_power_req_msg_v01 req_msg;
   mcm_dm_set_power_resp_msg_v01 *resp_msg;

   token_ptr = (int*)malloc(sizeof(int));

   resp_msg = (mcm_dm_set_power_resp_msg_v01*) malloc(sizeof(*resp_msg));

   req_msg.radio_power = MCM_DM_RADIO_MODE_OFF_V01;
   response_received = FALSE;
   ind_received = FALSE;

   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(client1_handle,
                                    MCM_DM_SET_POWER_REQ_V01,
                                    &req_msg,
                                    resp_msg,
                                    mcm_set_power_resp_cb,
                                    token_ptr);

   printf("token id from mcm framework - %d\n",*token_ptr);

}


void mcm_test_dm_online()

{
   int *token_ptr;
   mcm_dm_set_power_req_msg_v01 req_msg;
   mcm_dm_set_power_resp_msg_v01 *resp_msg;

   token_ptr = (int*)malloc(sizeof(int));

   resp_msg = (mcm_dm_set_power_resp_msg_v01*) malloc(sizeof(*resp_msg));

   req_msg.radio_power = MCM_DM_RADIO_ON_V01;
   response_received = FALSE;
   ind_received = FALSE;

   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(client1_handle,
                                    MCM_DM_SET_POWER_REQ_V01,
                                    &req_msg,
                                    resp_msg,
                                    NULL,
                                    token_ptr);

   printf("token id from mcm framework - %d\n",*token_ptr);

}


void mcm_register_dm_indication()
{
   mcm_dm_event_register_req_msg_v01 req_msg;
   mcm_dm_event_register_req_msg_v01 resp_msg;
   req_msg.register_power_event_valid = TRUE;
   req_msg.register_power_event = TRUE;

   MCM_CLIENT_EXECUTE_COMMAND_SYNC(client1_handle,
                                 MCM_DM_EVENT_REGISTER_REQ_V01,
                                 &req_msg,
                                 &resp_msg);

}

int start_mcm_client1()
{

   printf("Client1 started\n");
   mcm_client_init(&client1_handle,mcm_client1_async_cb,mcm_client1_ind_cb);
   printf("Client1 handle - %d\n",client1_handle);

   mcm_register_dm_indication();
   mcm_test_dm_offline();
   while(!response_received)
   {
   }
   printf("received response for offline request\n");
   while(ind_received)
   {
   }
   printf("received indication for offline request\n");

   mcm_test_dm_online();

   printf("received response for online request\n");

   while(response_received)
   {
   }
   while(!ind_received)
   {
   }
   printf("received indication for online request\n");

}


int start_mcm_client2()
{
   mcm_client_init(&client1_handle,mcm_client2_async_cb,mcm_client2_ind_cb);

   mcm_test_dm_offline();
   while(!response_received)
   {
   }
   printf("received response for offline request\n");

   mcm_test_dm_online();

   printf("received response for online request\n");

   while(response_received)
   {
   }

}

int main(int argc, char *argv[])
{

    printf("\n*************************** start testModemPower ******************************\n");

    pthread_create(&client1_thread_id, NULL, start_mcm_client1,NULL);

    pthread_join(client1_thread_id,NULL);
    pthread_create(&client2_thread_id, NULL, start_mcm_client2,NULL);
    pthread_join(client2_thread_id,NULL);

    printf("\n*************************** End of testModemPower *****************************\n");

}

