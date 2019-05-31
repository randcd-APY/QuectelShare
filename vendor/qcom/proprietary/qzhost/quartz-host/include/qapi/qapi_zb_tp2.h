/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_tp2.h
   @brief QAPI for the ZigBee test endpoint.

   @addtogroup qapi_zb_tp2
   @{

   This API provides definitions, commands, and events related to the ZigBee
   test endpoint 2. These APIs are provided for certification testing and are
   not used in normal operation.

   @}
*/
#ifndef __QAPI_ZB_TP2_H__
#define __QAPI_ZB_TP2_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_tp2
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define QAPI_ZB_TP2_COMMAND_ID_TRANSMIT_COUNTED_PACKETS                         (0x0001) /**< TP2 command ID for the transmit counted packets command. */
#define QAPI_ZB_TP2_COMMAND_ID_RESET_PACKET_COUNT                               (0x0002) /**< TP2 command ID for the reset packet count command. */
#define QAPI_ZB_TP2_COMMAND_ID_RETRIEVE_PACKET_COUNT                            (0x0003) /**< TP2 command ID for the retrieve packet counted command. */
#define QAPI_ZB_TP2_COMMAND_ID_PACKET_COUNT_RESPONSE                            (0x0004) /**< TP2 command ID for the packet count response command. */
#define QAPI_ZB_TP2_COMMAND_ID_BUFFER_TEST_REQUEST                              (0x001C) /**< TP2 command ID for the buffer test request command. */
#define QAPI_ZB_TP2_COMMAND_ID_BUFFER_TEST_GROUP_REQUEST                        (0x001D) /**< TP2 command ID for the buffer test group request command. */
#define QAPI_ZB_TP2_COMMAND_ID_BUFFER_TEST_RESPONSE                             (0x0054) /**< TP2 command ID for the buffer test response command. */
#define QAPI_ZB_TP2_COMMAND_ID_BUFFER_TEST_GROUP_RESPONSE                       (0x0055) /**< TP2 command ID for the buffer test group response command. */
#define QAPI_ZB_TP2_COMMAND_ID_ROUTE_DISCOVERY_REQUEST                          (0x1000) /**< TP2 command ID for the route discovery Request command. */
#define QAPI_ZB_TP2_COMMAND_ID_ROUTE_DISCOVERY_RESPONSE                         (0x1001) /**< TP2 command ID for the route discovery response command. */
#define QAPI_ZB_TP2_COMMAND_ID_FREEFORM_REQUEST                                 (0xA0A8) /**< TP2 command ID for the freeform request command. */
#define QAPI_ZB_TP2_COMMAND_ID_FREEFORM_RESPONSE                                (0xE000) /**< TP2 command ID for the freeform response command. */
#define QAPI_ZB_TP2_COMMAND_ID_FREEFORM_NO_DATA_RESPONSE                        (0xE001) /**< TP2 command ID for the freeform no data response command. */
#define QAPI_ZB_TP2_COMMAND_ID_BROADCAST_REQUEST                                (0xF000) /**< TP2 command ID for the broadcast request command. */
#define QAPI_ZB_TP2_COMMAND_ID_BROADCAST_ALL_DEVICES_RESPONSE                   (0xF001) /**< TP2 command ID for the broadcast all device response command. */
#define QAPI_ZB_TP2_COMMAND_ID_BROADCAST_RX_ON_WHEN_IDLE_DEVICES_RESPONSE       (0xF00A) /**< TP2 command ID for the broadcast rx on when idle devices response command. */
#define QAPI_ZB_TP2_COMMAND_ID_BROADCAST_ROUTERS_RESPONSE                       (0xF00E) /**< TP2 command ID for the broadcast routers response command. */

#define QAPI_ZB_TP2_PROFILE_ID                                                  (0x7F01) /**< Profile ID used for TP2. */

#define QAPI_ZB_TP2_ENDPOINT_TEST_DRIVER                                        (0x01) /**< Typical endpoint for a TP2 test driver device. */
#define QAPI_ZB_TP2_ENDPOINT_DEVICE_UNDER_TEST                                  (0xF0) /**< Typical endpoint for a TP2 device under test. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct qapi_ZB_TP2_Handle_s *qapi_ZB_TP2_Handle_t;

/**
   Enumeration of the TP2 buffer test response status.
*/
typedef enum
{
   QAPI_ZB_TP2_BUFFER_TEST_RESPONSE_STATUS_SUCCESS_E = 0x00, /**< Indicates success. */
   QAPI_ZB_TP2_BUFFER_TEST_RESPONSE_STATUS_FAILED_E  = 0x01, /**< Indicates sending the buffer test response failed on its first attempt. */
} qapi_ZB_TP2_Buffer_Test_Response_Status_t;

/**
   Enumeration of the TP2 freeform type.
*/
typedef enum
{
   QAPI_ZB_TP2_FREEFORM_TYPE_8BIT_INTEGER_E     = 0x00, /**< Freeform Request type of 8-bit integer value. */
   QAPI_ZB_TP2_FREEFORM_TYPE_CHARACTER_STRING_E = 0x01, /**< Freeform Request type of character string. */
   QAPI_ZB_TP2_FREEFORM_TYPE_COORDINATES_E      = 0x02, /**< Freeform Request type of coordinates. */
   QAPI_ZB_TP2_FREEFORM_TYPE_16BIT_INTEGER_E    = 0x03, /**< Freeform Request type of 16-bit integer value. */
   QAPI_ZB_TP2_FREEFORM_TYPE_NO_DATA_E          = 0x04, /**< Freeform Request type of no data value. */
   QAPI_ZB_TP2_FREEFORM_TYPE_RELATIVE_TIME_E    = 0x05, /**< Freeform Request type of relative time value. */
   QAPI_ZB_TP2_FREEFORM_TYPE_ABSOLUTE_TIME_E    = 0x06  /**< Freeform Request type of absolute time value. */
} qapi_ZB_TP2_Freeform_Type_t;

/**
   Enumeration of the TP2 broadcast request type.
*/
typedef enum
{
   QAPI_ZB_TP2_BROADCAST_TYPE_ALL_DEVICES_E             = 0x00, /**< Broadcast request type of broadcast to all devices. */
   QAPI_ZB_TP2_BROADCAST_TYPE_RX_ON_WHEN_IDLE_DEVICES_E = 0x01, /**< Broadcast request type of broadcast to all RxOnWhenIdle devices. */
   QAPI_ZB_TP2_BROADCAST_TYPE_ROUTERS_E                 = 0x02  /**< Broadcast request type of broadcast to all routers and the coordinator. */
} qapi_ZB_TP2_Broadcast_Type_t;

/**
   Enumeration of the events generated by the TP2 test profile.
*/
typedef enum
{
   QAPI_ZB_TP2_EVENT_TYPE_COMMAND_COMPLETE_E,         /**< Indicates a Command Complete event. */
   QAPI_ZB_TP2_EVENT_TYPE_COUNTED_PACKETS_COMPLETE_E, /**< Indicates a TP2 transmit counted packets command has completed. */
   QAPI_ZB_TP2_EVENT_TYPE_COUNTED_PACKET_E,           /**< Indicates a TP2 counted packet has been received. */
   QAPI_ZB_TP2_EVENT_TYPE_PACKET_COUNT_RESPONSE_E,    /**< Indicates a packet count response was received. */
   QAPI_ZB_TP2_EVENT_TYPE_BUFFER_TEST_RESPONSE_E,     /**< Indicates a TP2 buffer test response event. */
   QAPI_ZB_TP2_EVENT_TYPE_ROUTE_DISCOVERY_RESPONSE_E, /**< Indicates a TP2 route discovery response event. */
   QAPI_ZB_TP2_EVENT_TYPE_FREEFORM_RESPONSE_E,        /**< Indicates a TP2 freeform response event. */
   QAPI_ZB_TP2_EVENT_TYPE_BROADCAST_RESPONSE_E        /**< Indicates a TP2 broadcast response event. */
} qapi_ZB_TP2_Event_Type_t;

/**
   Information for the TP2 transmit counted packets command.
*/
typedef struct qapi_ZB_TP2_Transmit_Counted_Packets_s
{
   /** Starting sequence number for the counted packets. This is typically zero. */
   uint16_t  StartCount;

   /** Total number of counted packets to send. */
   uint16_t  SendCount;

   /** Interval between counted packets being sent in milliseconds. */
   uint32_t  Interval;

   /** Length of the payload for the counted packets. */
   uint8_t   Length;

   /** Payload of the counted packets. This may be set to NULL if the length is
       zero. */
   uint8_t  *Payload;
} qapi_ZB_TP2_Transmit_Counted_Packets_t;

/**
   Structure that represents the information for a TP2 counted packet.
*/
typedef struct qapi_ZB_TP2_CountedPacketsComplete_s
{
   /** Status result of the transmit counted packets command. */
   qapi_Status_t Status;

   /** Sequence counter of the last packet sent. */
   uint16_t      FinalCount;
} qapi_ZB_TP2_Counted_Packets_Complete_t;

/**
   Structure that represents the information for a TP2 counted packet.
*/
typedef struct qapi_ZB_TP2_Counted_Packet_s
{
   /** Sequence counter of the packet. */
   uint16_t  SequenceCount;

   /** Length of the payload. */
   uint8_t   Length;

   /** Payload of the counted packet. */
   uint8_t  *Payload;
} qapi_ZB_TP2_Counted_Packet_t;

/**
   Structure that represents the information for a TP2 packet count response.
*/
typedef struct qapi_ZB_TP2_Packet_Count_Response_s
{
   /** Status result of the retrieve packet count command. */
   qapi_Status_t Status;

   /** Packet count from the remote device. */
   uint16_t      PacketCount;
} qapi_ZB_TP2_Packet_Count_Response_t;

/**
   Structure that represents the information for a TP2 buffer test response.
*/
typedef struct qapi_ZB_TP2_Buffer_Test_Response_s
{
   /** Flag indicating if this is a buffer test group response. */
   qbool_t                                    IsGroup;

   /** Status reported from the remote device. */
   qapi_ZB_TP2_Buffer_Test_Response_Status_t  Status;

   /** Length of the payload. */
   uint8_t                                    Length;

   /** Payload of the response. */
   uint8_t                                   *Payload;
} qapi_ZB_TP2_Buffer_Test_Response_t;

/**
   Structure that represents the information for a TP2 route discovery response.
*/
typedef struct qapi_ZB_TP2_Route_Discovery_Response_s
{
   /** Status of the route discovery.*/
   qapi_Status_t  Status;
} qapi_ZB_TP2_Route_Discovery_Response_t;

/**
   Structure that represents the information for a TP2 freeform response.
*/
typedef struct qapi_ZB_TP2_Freeform_Response_s
{
   /** Type of freeform packet requested.*/
   qapi_ZB_TP2_Freeform_Type_t  RequestType;

   /** Flag indicating if the response was valid (i.e., the command, payload and
       length match the type specified). */
   qbool_t                      Valid;

   /** Length of the payload. */
   uint8_t                      Length;

   /** Payload of the response.  */
   uint8_t                     *Payload;
} qapi_ZB_TP2_Freeform_Response_t;

/**
   Structure that represents the information for a TP2 freeform response.
*/
typedef struct qapi_ZB_TP2_Broadcast_Response_s
{
   /** Broadcast request type.*/
   qapi_ZB_TP2_Broadcast_Type_t RequestType;

   /** Flag indicating if the response was valid (i.e., the command and
       destination address match the type specified. */
   qbool_t                      Valid;
} qapi_ZB_TP2_Broadcast_Response_t;

/**
   Structure representing the data for an event from the TP2 profile.
*/
typedef struct qapi_ZB_TP2_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_TP2_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t          CommandComplete;

      /** Information for tx counted packets complete events. */
      qapi_ZB_TP2_Counted_Packets_Complete_t CountedPacketsComplete;

      /** Information for counted packet events. */
      qapi_ZB_TP2_Counted_Packet_t           CountedPacket;

      /** Information for packet count response events. */
      qapi_ZB_TP2_Packet_Count_Response_t    PacketCountResponse;

      /** Information for buffer test response events. */
      qapi_ZB_TP2_Buffer_Test_Response_t     BufferTestResponse;

      /** Information for route discovery response events. */
      qapi_ZB_TP2_Route_Discovery_Response_t RouteDiscoveryResponse;

      /** Information for freeform response events. */
      qapi_ZB_TP2_Freeform_Response_t        FreeformResponse;

      /** Information for broadcast response events. */
      qapi_ZB_TP2_Broadcast_Response_t       BroadcastResponse;

   } Data;
} qapi_ZB_TP2_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          events from a TP2 endpoint.

   @param ZB_Handle  Handle of the ZigBee instance was returned by a successful
                     call to qapi_ZB_Initialize().
   @param Event_Data Information for the event.
   @param CB_Param   User specified parameter for the callback function.
*/
typedef void (*qapi_ZB_TP2_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_TP2_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a tp2 endpoint.

   @param[in]  ZB_Handle  Handle of the ZigBee instance was returned by a
                          successful call to qapi_ZB_Initialize().
   @param[out] TP2_Handle Pointer to where the newly created endpoint handle is
                          to be written.
   @param[in]  Config     Configuration information for the endpoint.
   @param[in]  Event_CB   Callback function to register.
   @param[in]  CB_Param   Callback parameter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Create(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_TP2_Handle_t *TP2_Handle, uint8_t Endpoint, qapi_ZB_CL_Device_ID_t DeviceID, qapi_ZB_TP2_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Destroys a test endpoint created with qapi_ZB_TP2_Create().

   @param[in] TP2_Handle Handle of endpoint to be destroyed.
*/
void qapi_ZB_TP2_Destroy(qapi_ZB_TP2_Handle_t TP2_Handle);

/**
   @brief Sets the tx options for the tp2 instance.

   These Tx options are used both for packets sent by the cluster and to
   validate packets received by the cluster (such as the minimum security
   level).

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] TxOptions  Tx options for the clsuter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Set_Tx_Options(qapi_ZB_TP2_Handle_t TP2_Handle, uint8_t TxOptions);

/**
   @brief Gets the tx options for the tp2 instance.

   @param[in]  TP2_Handle Handle of endpoint.
   @param[out] TxOptions  Tx options for the clsuter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Get_Tx_Options(qapi_ZB_TP2_Handle_t TP2_Handle, uint8_t *TxOptions);

/**
   @brief Sets the value of the discover route flag used when sending packets
          for the specified tp2 instance.

   @param[in] TP2_Handle    Handle of endpoint.
   @param[in] DiscoverRoute Value of the discover route flag.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Set_Discover_Route(qapi_ZB_TP2_Handle_t TP2_Handle, qbool_t DiscoverRoute);

/**
   @brief Gets the value of the discover route flag for the specified tp2
          instance.

   @param[in]  TP2_Handle    Handle of endpoint.
   @param[out] DiscoverRoute Value of the discover route flag.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Get_Discover_Route(qapi_ZB_TP2_Handle_t TP2_Handle, qbool_t *DiscoverRoute);

/**
   @brief Sets the radius used when sending packets for the specified tp2
          instance.

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] Radius     Radius used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Set_Radius(qapi_ZB_TP2_Handle_t TP2_Handle, uint8_t Radius);

/**
   @brief Sets the radius for the specified tp2 instance.

   @param[in]  TP2_Handle Handle of endpoint.
   @param[out] Radius     Radius used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Get_Radius(qapi_ZB_TP2_Handle_t TP2_Handle, uint8_t *Radius);

/**
   @brief Sets the radius used when sending packets for the specified tp2
          instance.

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] ProfileID  Profile ID used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Set_ProfileID(qapi_ZB_TP2_Handle_t TP2_Handle, uint16_t ProfileID);

/**
   @brief Sets the radius for the specified tp2 instance.

   @param[in]  TP2_Handle Handle of endpoint.
   @param[out] ProfileID  Profile ID used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Get_ProfileID(qapi_ZB_TP2_Handle_t TP2_Handle, uint16_t *ProfileID);

/**
   @brief Sends a series of counted packets at the specified interval.

   @param[in] TP2_Handle  Handle of endpoint.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] RequestData Information for the transmit counted packets command.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Transmit_Counted_Packets(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_TP2_Transmit_Counted_Packets_t *RequestData);

/**
   @brief Sends a retrieve packet count.

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] SendInfo   General send information for the command packet.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Retrieve_Packet_Count(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a reset packet count.

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] SendInfo   General send information for the command packet.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Reset_Packet_Count(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a buffer test request or buffer test group request.

   @param[in] TP2_Handle Handle of endpoint.
   @param[in] SendInfo   General send information for the command packet. A
                         buffer test group request will be sent if the
                         destination address mode is
                         QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E.
   @param[in] Length     Length of the buffer to request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Buffer_Test_Request(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t Length);

/**
   @brief Sends a route discovery request.

   @param[in] TP2_Handle  Handle of endpoint.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] DstAddrMode Mode of the address field being requested as either
                          a network or group address.
   @param[in] NWKAddress  Network or group address to find a route for.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Route_Discovery_Request(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_Addr_Mode_t DstAddrMode, uint16_t NWKAddress, uint8_t Radius);

/**
   @brief Sends a freeform request.

   @param[in] TP2_Handle  Handle of endpoint.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] RequestType Type of freeform request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Freeform_Request(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_TP2_Freeform_Type_t RequestType);

/**
   @brief Sends a broadcast request.

   @param[in] TP2_Handle  Handle of endpoint.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] RequestType Type of broadcast being requested.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_TP2_Send_Broadcast_Request(qapi_ZB_TP2_Handle_t TP2_Handle, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_TP2_Broadcast_Type_t RequestType);

#endif // ] #ifndef __QAPI_ZB_TP2_H__

/** @} */

