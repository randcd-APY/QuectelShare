/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_hps.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth HTTP Proxy Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The HTTP Proxy Service (HPS) programming interface defines the
 * protocols and procedures to be used to implement the HTTP Proxy Service
 * capabilities.
 */

#ifndef __QAPI_BLE_HPS_H__
#define __QAPI_BLE_HPS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_hpstypes.h"  /* QAPI HPS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_HPS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_HPS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_HPS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_HPS_ERROR_INSUFFICIENT_BUFFER_SPACE              (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_HPS_ERROR_SERVICE_ALREADY_REGISTERED             (-1004)
/**< Service is already registered. */
#define QAPI_BLE_HPS_ERROR_INVALID_INSTANCE_ID                    (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_HPS_ERROR_MALFORMATTED_DATA                      (-1006)
/**< Malformatted data. */
#define QAPI_BLE_HPS_ERROR_INVALID_CHARACTERISTIC_TYPE            (-1007)
/**< Invalid Characteristic type. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by an HPS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_HPS_Client_Information_s
{
   /**
    * HPS URI attribute handle.
    */
   uint16_t  URI;

   /**
    * HPS Headers attribute handle.
    */
   uint16_t  Headers;

   /**
    * HPS Entity Body attribute handle.
    */
   uint16_t  Entity_Body;

   /**
    * HPS Control Point attribute handle.
    */
   uint16_t  Control_Point;

   /**
    * HPS Status Code attribute handle.
    */
   uint16_t  Status_Code;

   /**
    * HPS Status Code Client Characteristic Configuration Descriptor
    * (CCCD) attribute handle.
    */
   uint16_t  Status_Code_CCCD;

   /**
    * HPS HTTPS Security attribute handle.
    */
   uint16_t  HTTPS_Security;
} qapi_BLE_HPS_Client_Information_t;

#define QAPI_BLE_HPS_CLIENT_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_HPS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_HPS_Client_Information_t structure.
 */

/**
 * Structure that represents the HPS Status Code data.
 *
 * The Status_Code field should be set to the Status-Line of the first
 * line of the HTTP Response message.
 */
typedef struct qapi_BLE_HPS_HTTP_Status_Code_Data_s
{
   /**
    * HPS Status Code received in the HTTP response.
    */
   uint16_t Status_Code;

   /**
    * A bitmask of the form QAPI_BLE_HPS_DATA_STATUS_XXX, which can be
    * found in qapi_ble_hpstypes.h. This field is intended to indicate
    * that the HPS server has received the HTTP Headers and HTTP Entity
    * Body from the HTTP response information and stored it, making it
    * available for the HPS client to read the values after receiving the
    * notification.
    */
   uint8_t  Data_Status;
} qapi_BLE_HPS_HTTP_Status_Code_Data_t;

#define QAPI_BLE_HPS_HTTP_STATUS_CODE_DATA_SIZE                   (sizeof(qapi_BLE_HPS_HTTP_Status_Code_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_HTTP_Status_Code_Data_t structure.
 */

/**
 * Enumeration that represents the HPS characteristic that has been
 * read/written for the
 * QAPI_BLE_HPS_ET_SERVER_READ_HPS_CHARACTERISTIC_REQUEST_E and
 * QAPI_BLE_HPS_ET_SERVER_WRITE_HPS_CHARACTERISTIC_REQUEST_E events.
 */
typedef enum
{
   QAPI_BLE_HPS_CT_URI_E,             /**< URI. */
   QAPI_BLE_HPS_CT_HTTP_HEADERS_E,    /**< HTTP Headers. */
   QAPI_BLE_HPS_CT_HTTP_ENTITY_BODY_E /**< HTTP Entity Body. */
} qapi_BLE_HPS_Characteristic_Type_t;

/**
 * Enumeration that represents the possible values that may be set for
 * the HTTP Control Point request type.
 *
 * The QAPI_BLE_HPS_HTTP_CANCEL_REQUEST_E may only be used if an HTTP
 * request is currently oustanding.
 */
typedef enum
{
   QAPI_BLE_HPS_HTTP_GET_REQUEST_E           = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_GET_REQUEST,
   /**< HTTP get request. */
   QAPI_BLE_HPS_HTTP_HEAD_REQUEST_E          = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_HEAD_REQUEST,
   /**< HTTP head request. */
   QAPI_BLE_HPS_HTTP_POST_REQUEST_E          = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_POST_REQUEST,
   /**< HTTP post request. */
   QAPI_BLE_HPS_HTTP_PUT_REQUEST_E           = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_PUT_REQUEST,
   /**< HTTP put request. */
   QAPI_BLE_HPS_HTTP_DELETE_REQUEST_E        = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_DELETE_REQUEST,
   /**< HTTP delete request. */
   QAPI_BLE_HPS_HTTP_SECURE_GET_REQUEST_E    = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_GET_REQUEST,
   /**< HTTP secure get request. */
   QAPI_BLE_HPS_HTTP_SECURE_HEAD_REQUEST_E   = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_HEAD_REQUEST,
   /**< HTTP secure head request. */
   QAPI_BLE_HPS_HTTP_SECURE_POST_REQUEST_E   = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_POST_REQUEST,
   /**< HTTP secure post request. */
   QAPI_BLE_HPS_HTTP_SECURE_PUT_REQUEST_E    = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_PUT_REQUEST,
   /**< HTTP secure put request. */
   QAPI_BLE_HPS_HTTP_SECURE_DELETE_REQUEST_E = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_DELETE_REQUEST,
   /**< HTTP secure delete request. */
   QAPI_BLE_HPS_HTTP_CANCEL_REQUEST_E        = QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_CANCEL_REQUEST
   /**< HTTP cancel request. */
} qapi_BLE_HPS_HTTP_Control_Point_Request_Type_t;

/**
 * Enumeration that represents all the events generated by the HPS
 * service. These are used to determine the type of each event generated
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_HPS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_HPS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E,
   /**< Read Characteristic request event. */
   QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E,
   /**< Write Characteristic request event. */
   QAPI_BLE_HPS_ET_SERVER_PREPARE_CHARACTERISTIC_REQUEST_E,
   /**< Prepare Characteristic request event. */
   QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_E,
   /**< Write HTTP Control Point request event. */
   QAPI_BLE_HPS_ET_SERVER_READ_HTTPS_SECURITY_REQUEST_E,
   /**< Read HTTPS security request event. */
   QAPI_BLE_HPS_ET_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_E,
   /**< Read HTTP Status Code CCCD request event. */
   QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_E
   /**< Write HTTP Status Code CCCD request event. */
} qapi_BLE_HPS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to read an HPS
 * characteristic.
 *
 * This event should not be received while an HTTP request is outstanding,
 * since the HPS client should not read the URI, HTTP Headers, or the
 * HTTP Entity Body while an HTTP request is being executed. Since this
 * behavior is undefined, it is recommended that if an HTTP request is
 * currently outstanding, the request to read an HPS characteristic
 * identified by the Type field should be rejected with the ErrorCode
 * parameter of the qapi_BLE_HPS_Read_Characteristic_Request_Response()
 * function set to.
 * QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS.
 *
 * This event may be received after an HTTP request has been completed
 * (the HTTP Status Code has been notified to the HPS client). The HPS
 * client may wish to read the HTTP Headers and HTTP Entity Body that
 * will contain information from the HTTP response.
 *
 * The Offset field will be zero for the GATT Read Value request, which
 * is always sent first to read an HPS characteristic. However, if the
 * HPS characteristic cannot fit in the GATT Read Value response due to
 * the GATT Maximum Transmission Unit (MTU) constraints, the HPS
 * client must issue GATT Read Long Value requests until the entire HPS
 * characteristic has been read. If a GATT Read Long Value request has
 * been sent, the Offset field will be nonzero.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Read_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the HPS characteristic that has been requested to be
    * read.
    */
   qapi_BLE_HPS_Characteristic_Type_t Type;

   /**
    * Offset field is used to index the starting offset for an HPS
    * characteristic.
    */
   uint16_t                           Offset;
} qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_READ_CHARACTERISTIC_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to write an HPS
 * characteristic.
 *
 * The URI, HTTP Headers, and HTTP Entity Body must be written before an
 * HTTP request may be dispatched by the HPS server. An HPS client must
 * write a null (zero length) value if it does not wish to use a
 * particular HPS characteristic for the HTTP request.
 *
 * This event should not be received while an HTTP request is outstanding,
 * since the HPS client should not write the URI, HTTP Headers, or the
 * HTTP Entity Body while an HTTP request is being executed. Especially
 * since the HPS server will update the HTTP Headers and HTTP Entity
   Body with information received in the HTTP response. A
   subsequent write by an HPS client for the HTTP Headers
   or HTTP Entity Body may overwrite the response
   information that has been set by the HPS server. Since this
 * behavior is undefined, it is recommended that if an HTTP request is
 * currently outstanding, the request to read an HPS characteristic
 * identified by the Type field should be rejected with the ErrorCode
 * parameter of the qapi_BLE_HPS_Read_Characteristic_Request_Response()
 * function set to.
 * QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS.
 *
 * The HTTP Headers and HTTP Entity Body must be reconfigured by an HPS
 * client after an HTTP request has completed and before another HTTP
 * request may be dispatched, since the HPS server will update these
 * characteristics with information from the HTTP response.
 *
 * The BufferLength field may be zero. If this is the case, the Buffer
 * field will be NULL. This indicates that the HPS client has written a
 * NULL (zero length) value for this HPS characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Read_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the HPS characteristic that has been requested to be
    * written.
    */
   qapi_BLE_HPS_Characteristic_Type_t  Type;

   /**
    * Length of the buffer that contains the HPS characteristic data
    * that has been requested to be written.
    */
   uint16_t                            BufferLength;

   /**
    * Buffer that contains the HPS characteristic data
    * that has been requested to be written.
    */
   uint8_t                            *Buffer;
} qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_WRITE_CHARACTERISTIC_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to prepare data
 * that will be written at a later time for an HPS characteristic.
 *
 * This event is primarily provided to reject a GATT Prepare Write
 * request for optional security reasons, such as the HPS client has
 * insufficient authentication, authorization, or encryption. Therefore
 * we will not pass the prepared data up to the application until the
 * GATT Execute Write request has been received by the
 * HPS server, and the prepared writes are not cancelled. If the prepared
 * data is written, the QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E
 * event will be dispatched to the application. Otherwise, the prepared
 * data will be cleared.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Prepare_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the HPS characteristic that has been requested to be
    * written.
    */
   qapi_BLE_HPS_Characteristic_Type_t Type;
} qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_PREPARE_CHARACTERISTIC_REQUEST_DATA_SIZE (sizeof(qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to write the
 * HPS HTTP Control Point Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_HTTP_Control_Point_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                                       InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                                       ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t                ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                       TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                             RemoteDevice;

   /**
    * HTTP Control Point request type.
    */
   qapi_BLE_HPS_HTTP_Control_Point_Request_Type_t RequestType;
} qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t))
/**<
 * Size of the
 * #qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to read the
 * HPS HTTPS Security Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Read_HTTPS_Security_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_READ_HTTPS_SECURITY_REQUEST_DATA_SIZE (sizeof(qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to read the
 * HPS HTTP Status Code Characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Read_HTTP_Status_Code_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t))
/**<
 * Size of the
 * #qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HPS server when an HPS client has sent a request to write the
 * HPS HTTP Status Code Characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HPS_Write_HTTP_Status_Code_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_s
{
   /**
    * HPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the HPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * HPS HTTP Status Code CCCD value that has been requested to be written.
    */
   uint16_t                        ClientConfiguration;
} qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t;

#define QAPI_BLE_HPS_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t))
/**<
 * Size of the
 * qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * HPS server event data for an HPS instance.
 */
typedef struct qapi_BLE_HPS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_HPS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;

   union
   {
      /**
       * HPS Read Characteristic event data.
       */
      qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t         *HPS_Server_Read_Characteristic_Request_Data;

      /**
       * HPS Write Characteristic event data.
       */
      qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t        *HPS_Server_Write_Characteristic_Request_Data;

      /**
       * HPS Prepare Characteristic event data.
       */
      qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t      *HPS_Server_Prepare_Characteristic_Request_Data;

      /**
       * HPS Write HTTP Control Point event data.
       */
      qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t    *HPS_Server_Write_HTTP_Control_Point_Request_Data;

      /**
       * HPS Read HTTPS Security event data.
       */
      qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t         *HPS_Server_Read_HTTPS_Security_Request_Data;

      /**
       * HPS Read HTTP Status Code CCCD event data.
       */
      qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t  *HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data;

      /**
       * HPS Write HTTP Status Code Client CCCD event data.
       */
      qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_HPS_Event_Data_t;

#define QAPI_BLE_HPS_EVENT_DATA_SIZE                              (sizeof(qapi_BLE_HPS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_HPS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for an
 * HPS instance event callback. This function will be called
 * whenever an HPS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the HPS instance event data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e,. this
 * function does not have be reentrant). It should be noted however,
 * that if the same event callback is installed more than once, the
 * event callbacks will be called serially. Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another HPS instance event will not be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for HPS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock will occur because other event callbacks will not be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  HPS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter when the HPS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_HPS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_HPS_Event_Data_t *HPS_Event_Data, uint32_t CallbackParameter);

   /* HPS server API.                                                   */

/**
 * @brief
 * Initializes an HPS instance (HPS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only one HPS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register HPS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback       HPS event callback that will
 *                                 receive HPS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered HPS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 function.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the HPS instance ID of the HPS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the HPS
 *              Instance ID.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_HPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an HPS instance (HPS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_HPS_Initialize_Service(), this function allows the
 * application to select a attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only one HPS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, all fields of the ServiceHandleRange parameter must
 * be initialized to zero. The qapi_BLE_HPS_Query_Number_Attributes()
 * function may be used after initializing an HPS instance to determine
 * the attribute handle range for the HPS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register HPS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback       HPS event callback that will
 *                                 receive HPS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered HPS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 function.
 *
 * @param[in,out]  ServiceHandleRange   Pointer that, on input, holds
 *                                      the handle range to store the
 *                                      service in GATT, and on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the HPS Instance ID of the HPS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the HPS
 *              instance ID.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_HPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an HPS instance (HPS server).
 *
 * @details
 * After this function is called, no other HPS
 * function can be called until after a successful call to either of the
 * qapi_BLE_HPS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that will
 *                                 be cleaned up.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in an HPS instance that is registered with a call
 * to either of the qapi_BLE_HPS_Initialize_XXX() functions.
 *
 * @return   Positive, nonzero, number of attributes for the
 *           registered HPS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Query_Number_Attributes(void);

/**
 * @brief
 * Responds to a read request,
 * received from an HPS client, for the HPS URI, HTTP Headers, or Entity
 * Body Characteristic.
 *
 * @details
 * This function should not be called while an HTTP request is
 * outstanding, since the HPS client should not read the URI, HTTP
 * Headers, or the HTTP Entity Body while an HTTP request is being
 * executed. Since this behavior is undefined, it is recommended that
 * if an HTTP request is currently outstanding and a request has been
 * dispatched to read the URI, HTTP Headers, or HTTP Entity Body
 * characteristics, it should be rejected with the ErrorCode parameter
 * set to QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS.
 *
 * If the BufferLength parameter is set to zero, this function will send a
 * zero length response for the HPS charactersitic. The Buffer parameter
 * may be excluded (NULL). This simply means that the value has a NULL
 * (zero length) value. This does not mean that the HPS client has not
 * written the value, since the HPS client may write a NULL (zero length)
 * value for an HPS characteristic to indicate that this particular
 * characteristic should not be used for an HTTP request.
 *
 * If this function is called to respond to a GATT Read Long Value
 * request (the Offset field of the
 * #qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t structure will
 * be nonzero), the BufferLength parameter should be set to the
 * remaining length of the data for the HPS characteristic and the Buffer
 * parameter should be set to the location of the HPS characteristic data
 * indexed by the specified Offset.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the BufferLength parameter will be ignored
 * and the Buffer parameter may be excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Type                Identifies the HPS characteristic.
 *
 * @param[in]  BufferLength        Length of the buffer that
 *                                 will be sent if the request is
 *                                 accepted. A zero length value is
 *                                 permitted if the request is accepted.
 *
 * @param[in]  Buffer              Buffer will be sent if the request
 *                                 is accepted. A NULL value is permitted
 *                                 if the request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Read_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_HPS_Characteristic_Type_t Type, uint16_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Responds to a write request,
 * received from an HPS client for the HPS URI, HTTP Headers, or Entity
 * Body Characteristic.
 *
 * @details
 * This function should not be called while an HTTP request is
 * outstanding, since the HPS client should not write the URI, HTTP
 * Headers, or the HTTP Entity Body while an HTTP request is being
 * executed, especially since the HPS server will update the HTTP
 * Headers and HTTP Entity Body with information received in the HTTP
 * response. A subsequent write by an HPS client for the HTTP Headers or
 * HTTP Entity Body may overwrite the response information that has been
 * set by the HPS server. Since this behavior is undefined, it is
 * recommended that, if an HTTP request is currently outstanding and a
 * request has been dispatched to write the URI, HTTP Headers, or HTTP
 * Entity Body characteristics, it should be rejected with the ErrorCode
 * parameter set to
 * QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the HPS characteristic.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Write_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_HPS_Characteristic_Type_t Type);

/**
 * @brief
 * Responds to a request received
 * from an HPS client to prepare the HPS URI, HTTP Headers, or Entity
 * Body Characteristic data for a write.
 *
 * @details
 * This function should not be called while an HTTP request is
 * outstanding, since the HPS client should not prepare/write the URI,
 * HTTP Headers, or the HTTP Entity Body while an HTTP request is being
 * executed, especially since the HPS server will update the HTTP Headers
 * and HTTP Entity Body with information received in the HTTP response.
 * A subsequent write by an HPS client for the HTTP Headers or HTTP
 * Entity Body may overwrite the response information that has been set
 * by the HPS server. Since this behavior is undefined, it is
 * recommended that, if an HTTP request is currently outstanding and a
 * request has been dispatched to prepare/write the URI, HTTP Headers, or
 * HTTP Entity Body characteristics, it should be rejected with the
 * ErrorCode parameter set to
 * QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS.
 *
 * This event is primarily provided to reject a GATT Prepare Write
 * request for optional security reasons, such as the HPS client has
 * insufficient authentication, authorization, or encryption. This is
 * required if the HPS server needs additional security. Therefore, we
 * will not pass the prepared data up to the application until the
 * GATT Execute Write request has been received by the HPS server, and
 * the prepared writes are not cancelled. If the prepared data is
 * written, the QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E
 * event will be dispatched to the application. Otherwise, the prepared
 * data will be cleared.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Type                Identifies the HPS characteristic.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Prepare_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_HPS_Characteristic_Type_t Type);

/**
 * @brief
 * Responds to a write request,
 * received from an HPS client for the HPS HTTP Control Point (HCP)
 * Characteristic.
 *
 * @details
 * This function is primarily provided to allow a way to reject an HTTP
 * Control Point request when the HTTP Status Code CCCD has not been
 * configured for notifications, an outstanding HTTP Control Point
 * operation is already in progress, the HTTP Server certificate for an
 * HTTPS request is invalid, or the HTTP Control Point request has an
 * invalid value. The completion of the HTTP Control Point request is
 * determined by the notification of the HPS HTTP Status Code via the
 * qapi_BLE_HPS_Notify_HTTP_Status_Code() function.
 *
 * The HPS server should only dispatch an HTTP request if the ErrorCode
 * parameter is set to QAPI_BLE_HPS_ERROR_CODE_SUCCESS.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_HTTP_Control_Point_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Notifies the HPS HTTP Status Code
 * Characteristic to an HPS client.
 *
 * @details
 * This function is primarily provided to inform the HPS client that the
 * HTTP response has been received by the HPS server and the HPS server
 * has set the HTTP Headers and HTTP Entity Body with the required
 * information from the HTTP response. This informs the HPS client that
 * the HTTP request has been completed. This does not mean that it was
 * successful. The Status_Code field of the
 * #qapi_BLE_HPS_HTTP_Status_Code_Data_t structure will indicate this.
 * After receiving the notification, the HPS client may read the HTTP
 * response information that has been previously set by the HPS server.
 *
 * This function sends a notification, which provides no guarantee that
 * the HPS client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the HPS
 * HTTP Status Code Characteristic's CCCD
 * has been previously configured for notifications. An
 * HPS client must have written the HPS HTTP Status Code Characteristic's
 * CCCD to enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the HPS client.
 *
 * @param[in]  HTTPStatusCode      Pointer to the HPS HTTP Status Code
 *                                 that will be sent in the notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Notify_HTTP_Status_Code(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HPS_HTTP_Status_Code_Data_t *HTTPStatusCode);

/**
 * @brief
 * Respondsg to a read request,
 * received from an HPS client, for the HPS HTTPS Security
 * Characteristic.
 *
 * @details
 * The requested HTTPS Security must be set before an HTTPS request is
 * dispatched by the HPS server. This means that the HPS client should
 * be able to read the value before an HTTP request is dispatched. This
 * is because the HTTP Server certificate should be validated before an
 * HTTPS request is dispatched by the HPS server.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the CertificateValid parameter will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  CertificateValid    Indicates whether the HTTPS Certificate is
 *                                 valid.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Read_HTTPS_Security_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, boolean_t CertificateValid);

/**
 * @brief
 * Responds to a read request
 * received from an HPS client for the HPS HTTP Status Code's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whther the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Configuration       CCCD that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Read_HTTP_Status_Code_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Configuration);

/**
 * @brief
 * Respondsg to a write request,
 * received from an HPS client, for the HPS HTTP Status Code's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_hpstypes.h
 * (HPS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the HPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Write_HTTP_Status_Code_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

   /* HPS client API.                                                   */

/**
 * @brief
 * Parses a value received in
 * a GATT Notification from a remote HPS server, interpreting it as the
 * HPS HTTP Status Code Characteristic.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           HPS server.
 *
 * @param[in]  Value         Value received from the
 *                           HPS server.
 *
 * @param[out]  StatusCodeData   Pointer that will hold the
 *                               HPS HTTP Status Code Characteristic
 *                               if this function is
 *                               successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. HPS error codes can be
 *              found in qapi_ble_hps.h (QAPI_BLE_HPS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HPS_Decode_HTTP_Status_Code(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HPS_HTTP_Status_Code_Data_t *StatusCodeData);

/**
 * @}
 */

#endif

