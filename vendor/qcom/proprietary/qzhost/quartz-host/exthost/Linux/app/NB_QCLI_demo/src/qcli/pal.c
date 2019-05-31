/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qapi_types.h"
#include "qapi_qsHost.h"

#include "pal.h"
#include "qcli.h"

#include "qsOSAL.h"

#include "qapi/qapi.h"
#include "qapi/qapi_status.h"
#include "qsConfig.h"

#include "spple_demo.h"
#include "zigbee_demo.h"
#include "debug_demo.h"
#include "hmi_demo.h"
#include "thread_demo.h"
#include "fwupgrade_demo.h"
#include "coex_demo.h"

#include "qsCallback.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define PAL_RECIEVE_BUFFER_SIZE                         (128)

#define PAL_THREAD_STACK_SIZE                           (3072)
#define PAL_THREAD_PRIORITY                             (10)

/* The following is a simple macro to facilitate printing strings directly
   to the console. As it uses the sizeof operator on the size of the string
   provided, it is intended to be used with string literals and will not
   work correctly with pointers.
*/
#define PAL_CONSOLE_WRITE_STRING_LITERAL(__String__)    do { PAL_Console_Write(sizeof(__String__) - 1, (__String__)); } while(0)

#define MSGQ_MSG_SIZE                                   (2000)
#define MSGQ_MAX_NUM_MSG                                (15) 

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct PAL_Context_s
{
   char    Rx_Buffer[PAL_RECIEVE_BUFFER_SIZE];
   qbool_t Initialized;
   qbool_t Exit;
} PAL_Context_t;


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

static PAL_Context_t PAL_Context;

/*-------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

static void Initialize_Samples(void);
static void Cleanup_Samples(void);
static qbool_t PAL_Initialize(void);

/*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/

/**
   @brief This function is responsible for initializing the sample
          applications.
*/
static void Initialize_Samples(void)
{
   Callback_Initialize();

   Initialize_SPPLE_Demo();
   Initialize_ZigBee_Demo();
   Initialize_Debug_Demo();
   Initialize_HMI_Demo();
   Initialize_Thread_Demo();
   Initialize_Firmware_Upgrade_Demo();
   Initialize_Coex_Demo();
}

/**
   @brief This function is responsible for initializing the sample
          applications.
*/
static void Cleanup_Samples(void)
{
   Cleanup_SPPLE_Demo();
   Cleanup_ZigBee_Demo();
   Cleanup_Debug_Demo();
   Cleanup_HMI_Demo();
   Cleanup_Thread_Demo();
   Cleanup_Firmware_Upgrade_Demo();
   Cleanup_Coex_Demo();

   Callback_Shutdown();
}

/**
   @brief This function is used to initialize the Platform, predominately
          the console port.

   @return
    - true if the platform was initialized successfully.
    - false if initialization failed.
*/
static qbool_t PAL_Initialize(void)
{
   memset(&PAL_Context, 0, sizeof(PAL_Context));

   return(true);
}

void ServerEventCb(uint32_t eventId, void *param)
{
    switch(eventId) {
    case QAPI_QS_EVENT_TARGET_ASSERT:
        PAL_CONSOLE_WRITE_STRING_LITERAL("Warning: Target assert and Server exit.");
        PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
        PAL_Exit();
        break;
    case QAPI_QS_EVENT_SERVER_EXIT:
        PAL_CONSOLE_WRITE_STRING_LITERAL("Warning: Server exit.");
        PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
        PAL_Exit();
        break;
    default:
        break;
    }
}

void InitializeModules(char *DeviceName)
{
#ifdef CONFIG_DAEMON_MODE
    qapi_Qs_Init("/iotdq", MSGQ_MSG_SIZE, MSGQ_MAX_NUM_MSG);
    /* register server event callback to monitor server exception */
    qapi_Qs_Register_Cb(ServerEventCb, 0);
#else
#ifdef QS_WPAN_INIT_BLE_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_BLE, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_HMI_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_HMI, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_TWN_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_THREAD, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_ZB_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_ZIGBEE, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_HERH_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_HERH, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_COEX_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_COEX, QAPI_QS_UART_E, DeviceName);
#endif
#ifdef QS_WPAN_INIT_FWUPGRADE_THREAD
     qapi_Qs_Init(QAPI_QS_MODULE_FWUPGRADE, QAPI_QS_UART_E, DeviceName);
#endif
#endif /* CONFIG_DAEMON_MODE */
       return;
}

void CleanupModules(void)
{
#ifdef CONFIG_DAEMON_MODE
    qapi_Qs_DeInit();

#else

#ifdef QS_WPAN_INIT_BLE_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_BLE);
#endif
#ifdef QS_WPAN_INIT_HMI_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_HMI);
#endif
#ifdef QS_WPAN_INIT_TWN_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_THREAD);
#endif
#ifdef QS_WPAN_INIT_ZB_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_ZIGBEE);
#endif
#ifdef QS_WPAN_INIT_HERH_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_HERH);
#endif
#ifdef QS_WPAN_INIT_COEX_THREAD
     qapi_Qs_DeInit(QAPI_QS_MODULE_COEX);
#endif
#ifdef QS_WPAN_INIT_FWUPGRADE_THREAD
    qapi_Qs_DeInit(QAPI_QS_MODULE_FWUPGRADE);
#endif
#endif /* CONFIG_DAEMON_MODE */

}

/**
   @brief The main entry of the Linux demo.
*/
int main(int argc, char* argv[])
{
   uint32_t InputLength;
    char *dev = "/dev/ttyUSB0";
//   if(argc > 1)
//   {
      InitializeModules(dev);

      memset(&PAL_Context, 0, sizeof(PAL_Context_t));

      /* Initialize the platform. */
      if(PAL_Initialize())
      {
         /* Initiailze the CLI. */
         if(QCLI_Initialize())
         {
            /* Initialize the samples. */
            Initialize_Samples();

            /* Display the initialize command list. */
            QCLI_Display_Command_List();

            PAL_Context.Initialized = true;
            PAL_Context.Exit        = false;

            /* Loop waiting for received data. */
            while(!PAL_Context.Exit)
            {
               InputLength = 0;
               /* Wait for data to be received. */
               if(fgets(PAL_Context.Rx_Buffer, PAL_RECIEVE_BUFFER_SIZE, stdin) != NULL)
               {
                  /* Determin the length of the input. */
                  InputLength = strlen(PAL_Context.Rx_Buffer);

                  /* Put the PAL_INPUT_END_OF_LINE_CHARACTER at the end of the input
                  string. */
                  PAL_Context.Rx_Buffer[InputLength] = PAL_INPUT_END_OF_LINE_CHARACTER;

                  /* Send the next buffer's data to QCLI for processing. */
                  QCLI_Process_Input_Data((InputLength + 1), PAL_Context.Rx_Buffer);

                  /* Reset the receive buffer for next command. */
                  memset(PAL_Context.Rx_Buffer, 0, PAL_RECIEVE_BUFFER_SIZE);
               }
            }

            Cleanup_Samples();
         }

         QCLI_Shutdown();
      }

      CleanupModules();
//   }
  // else
    //  printf("Usage:  QCLIHost [Device File ex. /dev/TTYUSB1]\n");

   return(0);
}

/**
   @brief This function is used to write a buffer to the console. Note
          that when this function returns, all data from the buffer will
          be written to the console or buffered locally.

   @param Length is the length of the data to be written.
   @param Buffer is a pointer to the buffer to be written to the console.
*/
void PAL_Console_Write(uint32_t Length, const char *Buffer)
{

   if((Length != 0) && (Buffer != NULL) && (PAL_Context.Initialized))
   {
      //xxx Make sure the buffer contains formatted string.
      printf("%s",Buffer);
      fflush(stdout);
   }
}

/**
   @brief This function indicates to the PAL layer that the application
          should exit.
*/
void PAL_Exit(void)
{
   PAL_CONSOLE_WRITE_STRING_LITERAL("Exiting...");
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

   PAL_Context.Exit = true;
}

/**
   @brief This function indicates to the PAL layer that the application
          should reset. For embedded applications this is typically a reboot.
*/
void PAL_Reset(void)
{
   PAL_CONSOLE_WRITE_STRING_LITERAL("Resetting...");
   PAL_CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

   PAL_Context.Exit = true;
}

