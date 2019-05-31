/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
   @file qapi_twn_hosted.h
   @brief QAPI for the Thread Wireless Network.

   @addtogroup qapi_twn_hosted
   @{

   This API provides a control interface for the Thread Wireless Network.

   The API consists of functions to execute various commands and a callback
   mechanism for providing asynchronous events. The callback is registered when
   the interface is initialized via qapi_TWN_Initialize().

   @}
*/

#ifndef  __QAPI_TWN_HOSTED_H__ // [
#define  __QAPI_TWN_HOSTED_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

#include "qapi_twn.h"

/** @addtogroup qapi_twn_hosted
@{ */

/*-------------------------------------------------------------------------
 * Quartz Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

typedef enum
{
   QAPI_TWN_HOSTED_UDP_TRANSMIT_E,
} qapi_TWN_Hosted_Event_Type_t;

#define QAPI_TWN_HOSTED_AF_INET   2   /**< Address family is IPv4. */
#define QAPI_TWN_HOSTED_AF_INET6  3   /**< Address family is IPv6. */

typedef struct qapi_TWN_Hosted_Socket_Info_s
{
   uint16_t Family;
   uint16_t Port;
   uint8_t  Address[16];
} qapi_TWN_Hosted_Socket_Info_t;

typedef struct qapi_TWN_Hosted_UDP_Transmit_Data_s
{
   qapi_TWN_Hosted_Socket_Info_t  SocketInfo;
   uint16_t                       Length;
   uint8_t                       *Buffer;
} qapi_TWN_Hosted_UDP_Transmit_Data_t;

typedef struct qapi_TWN_Hosted_EventData_s
{
   qapi_TWN_Hosted_Event_Type_t Type;
   union
   {
      qapi_TWN_Hosted_UDP_Transmit_Data_t *qapi_TWN_Hosted_UDP_Transmit_Data;
   }
   Data;
} qapi_TWN_Hosted_Event_Data_t;

#define QAPI_TWN_HOSTED_EVENT_DATA_SIZE                  (sizeof(qapi_TWN_Hosted_Event_Data_t))

typedef void (*qapi_TWN_Hosted_Event_Callback_t)(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Event_Data_t *Event_Data, uint32_t CallbackParameter);

void TWN_Hosted_State_Changed_Callback(uint32_t Flags, void *Context);

/**
   @brief Starts the border agent service.

   @param[in] TWN_Handle    Handle of the TWN instance as provided by a
                            successful call to qapi_TWN_Initialize.
   @param[in] AddressFamily Address family to use for the external socket. This
                            can be AF_INET/INET6/INET_DUAL46.

   @param[in] DisplayName   Instance name registered in the MDNS record to be
                            used as a friendly display name.

   @param[in] HostName      Hostname registered in the MDNS record, as the FQDN
                            of this host (i.e., "thread-router.local.")

   @param[in] Interface     Network interface on which MDNS should run. This
                            should be either "wlan0" or "wlan1".

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Start_Border_Agent(qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface);

/**
   @brief Stops the border agent service.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Stop_Border_Agent(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Registers the callback used to transmit the UDP data back to the host.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Callback The user specified callback function.
   @param[in] Callback User specified callback parameter.

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERR_INVALID_PARAM  -- Invalid handle or bad timeout value.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Register_Event_Callback(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Event_Callback_t Callback, uint32_t CallbackParameter);

/**
   @brief Un-registers the callback used to transmit the UDP data back to the host.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERR_INVALID_PARAM  -- Invalid handle.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_UnRegister_Event_Callback(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Processes the UDP data that has been received over the
          UDP port, by the host.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] SocketInfo The source socket information.
   @param[in] Length  Length of the buffer.
   @param[in] Buffer  Holds the encrypted Dtls data.

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERR_INVALID_PARAM  -- Invalid handle.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Receive_UDP_Data(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Socket_Info_t *SocketInfo, uint16_t Length, uint8_t *Buffer);

/**
   @brief Enables Open Thread logging.

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Enable_Logging(void);

/**
   @brief Disables Open Thread logging.

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Hosted_Disable_Logging(void);

/** @} */

#endif // ] #ifndef __QAPI_TWN_HOSTED_H__
