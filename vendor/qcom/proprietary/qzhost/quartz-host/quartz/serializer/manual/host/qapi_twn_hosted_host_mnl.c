/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include <limits.h>
#include <poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "idlist.h"
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_twn.h"
#include "qapi_twn_hosted_common.h"
#include "qapi_twn_hosted_host_mnl.h"
#include "qapi_twn_hosted_host_cb.h"
#include "qapi_twn_common.h"

#define TWN_BORDER_ROUTER_PORT     49191
#define UDP_DATA_MTU               1280

typedef struct qapi_TWN_Socket_Context_s
{

   int               SocketFD;
   ThreadHandle_t    ThreadHandle;
   qapi_TWN_Handle_t TWN_Handle;
} qapi_TWN_Hosted_Context_t;

static Boolean_t                 Initialized;
static qapi_TWN_Hosted_Context_t Context;
static sa_family_t               Family;

static void *Receive_UDP_Data_Thread(void *Parameter);
static void Hosted_Event_Callback(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Event_Data_t *Event_Data, unsigned int CallbackParameter);

static void *Receive_UDP_Data_Thread(void *Parameter)
{
   int                           Result;
   qapi_Status_t                 Status;
   uint8_t                       Buffer[UDP_DATA_MTU];
   uint16_t                      Length;
   struct pollfd                 FD;
   struct sockaddr_storage       SocketAddress;
   socklen_t                     SocketLength;
   qapi_TWN_Hosted_Socket_Info_t SocketInfo;

   /* Configure poll. */
   FD.fd      = Context.SocketFD;
   FD.events  = POLLIN;
   FD.revents = 0;

   while(1)
   {
      /* Block until we have received data. */
      if((Result = poll(&FD, 1, -1)) > 0)
      {
         /* There is data that is ready to be read. */
         if(FD.revents & POLLIN)
         {
            /* Set the socket address length based on the
               address family. */
            if(Family == AF_INET)
            {
               SocketLength = sizeof(struct sockaddr_in);
            }
            else
            {
               SocketLength = sizeof(struct sockaddr_in6);
            }

            /* Receive the data. */
            if((Result = recvfrom(Context.SocketFD, Buffer, UDP_DATA_MTU, 0, (struct sockaddr *)&SocketAddress, &SocketLength)) > 0)
            {
               /* We do not need to check the socket length since we are
                  guaranteed the buffer can hold it. */

               /* Truncate the length if it exceeds the MTU size. */
               Length = ((uint16_t)Result > UDP_DATA_MTU) ? UDP_DATA_MTU : (uint16_t)Result;

               /* Convert to the socket info structure. */
               if(Family == AF_INET)
               {
                  /* Initialize the socket information. */
                  memset(&SocketInfo, 0, sizeof(SocketInfo));

                  SocketInfo.Family = (uint16_t)(((struct sockaddr_in *)&SocketAddress)->sin_family);
                  SocketInfo.Port   = (uint16_t)(((struct sockaddr_in *)&SocketAddress)->sin_port);
                  memcpy(&(SocketInfo.Address), (uint8_t *)&(((struct sockaddr_in *)&SocketAddress)->sin_addr), 4);
               }
               else
               {
                  SocketInfo.Family = (uint16_t)(((struct sockaddr_in6 *)&SocketAddress)->sin6_family);
                  SocketInfo.Port   = (uint16_t)(((struct sockaddr_in6 *)&SocketAddress)->sin6_port);
                  memcpy(&(SocketInfo.Address), (uint8_t *)&(((struct sockaddr_in6 *)&SocketAddress)->sin6_addr), 16);
               }

               /* We have received the encyrpted UDP data so we need to
                  send it via the serializer to Quartz. */
               Status = qapi_TWN_Hosted_Receive_UDP_Data(Context.TWN_Handle, &SocketInfo, Length, Buffer);
               if(Status != QAPI_OK)
               {
                  printf("qapi_TWN_Hosted_Receive_UDP_Data() failed.\n");
                  break;
               }
            }
         }
         else
         {
            /* Hang up event. */
            if(FD.revents & POLLHUP)
            {
               break;
            }
         }
      }

      /* An error has occured. */
      if(Result <= 0)
      {
         break;
      }
   }

   Context.ThreadHandle = NULL;

   close(Context.SocketFD);
   Context.SocketFD  = 0;

   return(NULL);
}

static void Hosted_Event_Callback(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Event_Data_t *Event_Data, unsigned int CallbackParameter)
{
   struct sockaddr_storage  SocketAddress;
   socklen_t                SocketLength;

   if((TWN_Handle) && (Event_Data))
   {
      switch(Event_Data->Type)
      {
         case QAPI_TWN_HOSTED_UDP_TRANSMIT_E:
            /* Format the destination socket address. */
            if(Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Family == AF_INET)
            {
               ((struct sockaddr_in *)&SocketAddress)->sin_family = Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Family;
               ((struct sockaddr_in *)&SocketAddress)->sin_port   = Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Port;
               memcpy(&(((struct sockaddr_in *)&SocketAddress)->sin_addr.s_addr), Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Address, 4);
               SocketLength                                       = sizeof(struct sockaddr_in);
            }
            else
            {
               ((struct sockaddr_in6 *)&SocketAddress)->sin6_family = Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Family;
               ((struct sockaddr_in6 *)&SocketAddress)->sin6_port   = Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Port;
               memcpy(&(((struct sockaddr_in6 *)&SocketAddress)->sin6_addr), Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->SocketInfo.Address, 16);
               SocketLength                                         = sizeof(struct sockaddr_in6);
            }

            /* Send the UDP data over the socket. */
            sendto(Context.SocketFD,
                   Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->Buffer,
                   Event_Data->Data.qapi_TWN_Hosted_UDP_Transmit_Data->Length,
                   0,
                   (struct sockaddr *)&SocketAddress,
                   SocketLength);
            break;
         default:
            /* Prevent compiler warnings. */
            break;
      }
   }
}

qapi_Status_t Mnl_qapi_TWN_Hosted_Start_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   /* Handle event callback registration. */
   uint32_t qsCbParam;
   CallbackInfo_t CallbackInfo;
   uint32_t CallbackParameter;

   /* Socket. */
   struct sockaddr_storage SocketAddress;
   socklen_t SocketLength;

   /* If we are not initialized. */
   if(!Initialized)
   {
      /* Store the TWN handle so we can transmit UDP data over the serializer
         to Quartz.*/
      Context.TWN_Handle = TWN_Handle;

      /* Map the TWN address family type to Linux
         address family type. */
      switch(AddressFamily)
      {
         case QAPI_TWN_AF_INET:
            Family = AF_INET;
            break;
         case QAPI_TWN_AF_INET6:
            Family = AF_INET6;
            break;
         default:
            Family = AF_INET;
            break;
      }

      /* Open socket using the specified address family (domain). */
      if((Context.SocketFD = socket(Family, SOCK_DGRAM, 0)) >= 0)
      {
         /* Initialize the socket address. */
         memset(&SocketAddress, 0, sizeof(SocketAddress));

         /* Configure the socket address. */
         if(Family == AF_INET)
         {
            ((struct sockaddr_in *)&SocketAddress)->sin_family      = AF_INET;
            ((struct sockaddr_in *)&SocketAddress)->sin_port        = htons(TWN_BORDER_ROUTER_PORT);
            ((struct sockaddr_in *)&SocketAddress)->sin_addr.s_addr = htonl(INADDR_ANY);
            SocketLength                                            = sizeof(struct sockaddr_in);
         }
         else
         {
            ((struct sockaddr_in6 *)&SocketAddress)->sin6_family = AF_INET6;
            ((struct sockaddr_in6 *)&SocketAddress)->sin6_port   = htons(TWN_BORDER_ROUTER_PORT);
            ((struct sockaddr_in6 *)&SocketAddress)->sin6_addr   = in6addr_any;
            SocketLength                                         = sizeof(struct sockaddr_in6);
         }

         /* Bind the socket address. */
         if((bind(Context.SocketFD, (struct sockaddr *)(&SocketAddress), SocketLength)) >= 0)
         {
            /* Create the thread to receive UDP data. */
            if((Context.ThreadHandle = QSOSAL_CreateThread(Receive_UDP_Data_Thread, PTHREAD_STACK_MIN, NULL)) != NULL)
            {
               /* Register the host callback. */
               CallbackInfo.TargetID = TargetID;
               CallbackInfo.ModuleID = QS_MODULE_THREAD;
               CallbackInfo.FileID = QAPI_TWN_HOSTED_FILE_ID;
               CallbackInfo.CallbackID = QAPI_TWN_HOSTED_EVENT_CALLBACK_T_CALLBACK_ID;
               CallbackInfo.CallbackKey = 0;
               CallbackInfo.AppFunction = Hosted_Event_Callback;
               CallbackInfo.AppParam = (uint32_t)0;
               qsResult = Callback_Register(&qsCbParam, Host_qapi_TWN_Hosted_Event_Callback_t_Handler, &CallbackInfo);

               /* Override the callback parameter with the new one. */
               CallbackParameter = qsCbParam;

               /* Calculate size of packed function arguments. */
               qsSize = (12 + ((DisplayName != NULL) ? (strlen((const char *)DisplayName)+1) : 0) + ((HostName != NULL) ? (strlen((const char *)HostName)+1) : 0) + ((Interface != NULL) ? (strlen((const char *)Interface)+1) : 0) + (QS_POINTER_HEADER_SIZE * 3));

               if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TWN, QAPI_TWN_FILE_ID, QAPI_TWN_START_BORDER_AGENT_FUNCTION_ID, &qsInputBuffer, qsSize))
               {
                  /* Write arguments packed. */
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TWN_Handle);

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_int(&qsInputBuffer, (int *)&AddressFamily);

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)DisplayName);

                  if((qsResult == ssSuccess) && (DisplayName != NULL))
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, (void *)DisplayName, 1, (strlen((const char *)(DisplayName))+1));
                  }

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)HostName);

                  if((qsResult == ssSuccess) && (HostName != NULL))
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, (void *)HostName, 1, (strlen((const char *)(HostName))+1));
                  }

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Interface);

                  if((qsResult == ssSuccess) && (Interface != NULL))
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Interface, 1, (strlen((const char *)(Interface))+1));
                  }

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

                  if(qsResult == ssSuccess)
                  {
                     /* Send the command.*/
                     if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
                     {
                        if(qsOutputBuffer.Start != NULL)
                        {
                           /* Unpack returned values. */
                           if(qsResult == ssSuccess)
                              qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                           /* If an error has occured.*/
                           if(qsRetVal == QAPI_OK)
                           {
                              /* We are initialized. */
                              Initialized = TRUE;
                           }
                           else
                           {
                              /* Shutdown the UDP socket so poll will exit and cause the UDP
                                 Receive thread to close. */
                              shutdown(Context.SocketFD, SHUT_RDWR);
                           }

                           /* Set the return value to error if necessary. */
                           if(qsResult != ssSuccess)
                           {
                              qsRetVal = QAPI_ERROR;
                           }
                        }
                        else
                        {
                           qsRetVal = QAPI_ERROR;
                        }

                        /* Free the output buffer. */
                        FreePackedBuffer(&qsOutputBuffer);
                     }
                     else
                     {
                        qsRetVal = QAPI_ERROR;
                     }
                  }
                  else
                  {
                     qsRetVal = QAPI_ERROR;
                  }

                  /* Free the input buffer. */
                  FreePackedBuffer(&qsInputBuffer);
               }
               else
               {
                  qsRetVal = QAPI_ERR_NO_MEMORY;
               }

               FreeBufferList(&qsBufferList);
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }
         }
         else
         {
            perror("bind() failed.\n");

            close(Context.SocketFD);
            Context.SocketFD  = 0;

            qsRetVal = QAPI_ERROR;
         }
      }
      else
      {
         perror("socket() failed.\n");

         qsRetVal = QAPI_ERROR;
      }
   }
   else
   {
      qsRetVal = QAPI_ERROR;
   }

   return(qsRetVal);
}

qapi_Status_t Mnl_qapi_TWN_Hosted_Stop_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   /* If we are initialized. */
   if(Initialized)
   {
      /* Calculate size of packed function arguments. */
      qsSize = 4;

      if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TWN, QAPI_TWN_FILE_ID, QAPI_TWN_STOP_BORDER_AGENT_FUNCTION_ID, &qsInputBuffer, qsSize))
      {
         /* Write arguments packed. */
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TWN_Handle);

         if(qsResult == ssSuccess)
         {
            /* Send the command.*/
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
               if(qsOutputBuffer.Start != NULL)
               {
                  /* Unpack returned values. */
                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                  /* If the function was successful. */
                  if(qsRetVal >= 0)
                  {
                     /* Un-register the hosted event callback. */
                     Callback_UnregisterByKey(MODULE_TWN, QAPI_TWN_FILE_ID, QAPI_TWN_HOSTED_EVENT_CALLBACK_T_CALLBACK_ID, (uint32_t)0);

                     /* Shutdown the UDP socket so poll will exit and cause the UDP
                        Receive thread to close. */
                     shutdown(Context.SocketFD, SHUT_RDWR);

                     /* Flag that we are no longer initialized. */
                     Initialized = FALSE;
                  }

                  /* Set the return value to error if necessary. */
                  if(qsResult != ssSuccess)
                  {
                     qsRetVal = QAPI_ERROR;
                  }
               }
               else
               {
                  qsRetVal = QAPI_ERROR;
               }

               /* Free the output buffer. */
               FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }
         }
         else
         {
            qsRetVal = QAPI_ERROR;
         }

         /* Free the input buffer. */
         FreePackedBuffer(&qsInputBuffer);
      }
      else
      {
         qsRetVal = QAPI_ERR_NO_MEMORY;
      }

      FreeBufferList(&qsBufferList);
   }

   return(qsRetVal);
}

qapi_Status_t Mnl_qapi_TWN_Hosted_Receive_UDP_Data(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Socket_Info_t *SocketInfo, uint16_t Length, uint8_t *Buffer)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (6 + CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(SocketInfo) + (QS_POINTER_HEADER_SIZE * 1) + Length);

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TWN, QAPI_TWN_HOSTED_FILE_ID, QAPI_TWN_HOSTED_RECEIVE_UDP_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TWN_Handle);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_qapi_TWN_Hosted_Socket_Info_t(&qsInputBuffer, SocketInfo);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&Length);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

      if((qsResult == ssSuccess) && (Buffer != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Buffer, sizeof(uint8_t), Length);
      }

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_ERR_NO_MEMORY;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

