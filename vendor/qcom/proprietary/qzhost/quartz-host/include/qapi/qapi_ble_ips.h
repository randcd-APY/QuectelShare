/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_ips.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Indoor Positioning Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Indoor Positioning Service (IPS) programming interface defines the
 * protocols and procedures to be used to implement the Indoor
 * Positioning Service capabilities.
 */

#ifndef __QAPI_BLE_IPS_H__
#define __QAPI_BLE_IPS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_ipstypes.h"  /* QAPI IPS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_IPS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_IPS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_IPS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_IPS_ERROR_INSUFFICIENT_BUFFER_SPACE              (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_IPS_ERROR_SERVICE_ALREADY_REGISTERED             (-1004)
/**< Service is already registered. */
#define QAPI_BLE_IPS_ERROR_INVALID_INSTANCE_ID                    (-1005)
/**< Invalid instance ID. */
#define QAPI_BLE_IPS_ERROR_MALFORMATTED_DATA                      (-1006)
/**< Malformatted data. */
#define QAPI_BLE_IPS_ERROR_FEATURE_NOT_SUPPORTED                  (-1007)
/**< Feature is not supported. */
#define QAPI_BLE_IPS_ERROR_INVALID_CONFIGURATION                  (-1008)
/**< Invalid configuration. */
#define QAPI_BLE_IPS_ERROR_INVALID_STRING_LENGTH                  (-1009)
/**< Invalid string length. */
#define QAPI_BLE_IPS_ERROR_READ_TX_POWER_FAILED                   (-1010)
/**< Could not read the transmit power. */
#define QAPI_BLE_IPS_ERROR_LOCATION_NAME_NOT_CONFIGURED           (-1011)
/**< Location name is not configured. */
#define QAPI_BLE_IPS_ERROR_NO_PENDING_AUTHORIZATION_REQUEST       (-1012)
/**< No pending authorization request. */

   /* IPS Common API Structures, Enums, and Constants.                  */

/**
 * Enumeration that represents the characteristic types.
 *
 * The QAPI_BLE_IPS_CT_EXTENDED_PROPERTIES_E is a  Characteristic
 * descriptor.
 */
typedef enum
{
   QAPI_BLE_IPS_CT_IP_CONFIGURATION_E,
   /**< IP Configuration. */
   QAPI_BLE_IPS_CT_LATITUDE_E,
   /**< Latitude. */
   QAPI_BLE_IPS_CT_LONGITUDE_E,
   /**< Longitude. */
   QAPI_BLE_IPS_CT_LOCAL_NORTH_COORDINATE_E,
   /**< Local North Coordinate. */
   QAPI_BLE_IPS_CT_LOCAL_EAST_COORDINATE_E,
   /**< Local East Coordinate. */
   QAPI_BLE_IPS_CT_FLOOR_NUMBER_E,
   /**< Floor number. */
   QAPI_BLE_IPS_CT_ALTITUDE_E,
   /**< Altitude. */
   QAPI_BLE_IPS_CT_UNCERTAINTY_E,
   /**< Uncertainty. */
   QAPI_BLE_IPS_CT_LOCATION_NAME_E,
   /**< Location name. */
   QAPI_BLE_IPS_CT_EXTENDED_PROPERTIES_E
   /**< Extended Properties. */
} qapi_BLE_IPS_Characteristic_Type_t;

   /* IPS Server API Structures, Enums, and Constants.                  */

   /* The following defines the flags that may be used to initialize the*/
   /* service with optional IPS Characteristics. These flags make up the*/
   /* bit mask for the QAPI_BLE_IPS_Characteristic_Flags field of the   */
   /* qapi_BLE_IPS_Initialize_Data_t structure for the                  */
   /* QAPI_BLE_IPS_Initialize_XXX() API's used to register IPS.         */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLAGS_LOCAL_COORDINATES                 (0x01)
/**< IPS is configured to use the local coordinates. */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLAGS_FLOOR_NUMBER                      (0x02)
/**< IPS is configured to include the Floor Number Characteristic. */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLAGS_ALTITUDE                          (0x04)
/**< IPS is configured to include the Altitude Characteristic. */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLAGS_UNCERTAINTY                       (0x08)
/**< IPS is configured to include the Uncertainty Characteristic. */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLAGS_LOCATION_NAME                     (0x10)
/**< IPS is configured to include the Location Name Characteristic. */

   /* The following defines the optional IPS Characteristic properties  */
   /* that may be set for IPS Characteristics. These flags make up the  */
   /* bit mask for the QAPI_BLE_IPS_Characteristic_Property_Flags       */
   /* parameter of the qapi_BLE_IPS_Initialize_Data_t structure for the */
   /* QAPI_BLE_IPS_Initialize_XXX() API's used to register IPS.         */
   /* * NOTE * The QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_XXX Flags */
   /*          will enable the IPS Characteristic property for all      */
   /*          supported characteristics (This includes any optional IPS*/
   /*          Characteristic defined by the                            */
   /*          QAPI_BLE_IPS_Characteristic_Flags field of the           */
   /*          qapi_BLE_IPS_Initialize_Data_t structure).               */
   /* * NOTE * QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_              */
   /*          -WRITE_WITHOUT_RESPONSE and                              */
   /*          QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_              */
   /*          -RELIABLE_WRITE can only be included if                  */
   /*          QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE is set. */
   /*          If QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE is   */
   /*          NOT SET then the other Flags will be IGNORED if set.     */
   /* * NOTE * If QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_           */
   /*          -RELIABLE_WRITE is set then an Extended Properties       */
   /*          descriptor will be present for each support              */
   /*          characteristic. The 'reliable write bit' will also be    */
   /*          automaticlly set since it is required to be set for      */
   /*          reliable writes.                                         */
#define QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE                   (0x01)
/**< All IPS Characteristics are writable. */
#define QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE_WITHOUT_RESPONSE  (0x02)
/**< All IPS Characteristics are writable without a response. */
#define QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_RELIABLE_WRITE          (0x04)
/**< All IPS Characteristics support a reliable write. */

/**
 * Structure that represents the data that is required to initialize the
 * Object Transfer Service (a parameter for the
 * qapi_BLE_IPS_Initialize_Service() and
 * qapi_BLE_IPS_Initialize_Service_Handle_Range() functions).
 *
 * The Enable_Authorized_Device_Write field configures the IPS server to
 * only allow authorized IPS clients to write IPS characteristics. In
 * order to use this feature, the
 * QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE flag must be set in
 * the QAPI_BLE_IPS_Characteristic_Properties_Flags field. This feature
 * will also affect the
 * QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_WRITE_WITHOUT_RESPONSE and
 * QAPI_BLE_IPS_CHARACTERISTIC_PROPERTY_FLAGS_RELIABLE_WRITE flags if
 * they are also specified in the
 * QAPI_BLE_IPS_Characteristic_Properties_Flags field.
 *
 * If Enable_Authorized_Device_Write is TRUE, an
 * QAPI_BLE_IPS_ET_SERVER_AUTHORIZATION_REQUEST_E event will be
 * dispatched when a request to write an IPS characteristic value or
 * prepare an IPS characteristic to be written (reliabled write or write
 * long) is received. If the IPS client that made the request is
 * authorized via the qapi_BLE_IPS_Authorization_Request_Response()
 * function, the QAPI_BLE_IPS_ET_SERVER_CHARACTERISTIC_UPDATED_E
 * event will be dispatched when the IPS characteristic(s) have been
 * written.
 */
typedef struct qapi_BLE_IPS_Initialize_Data_s
{
   /**
    * Controls the mandatory/optional IPS characteristics that are
    * included for the IPS instance based on the needs of the IPS server.
    */
   uint32_t   IPS_Characteristic_Flags;

   /**
    * Controls the optional properties for IPS characteristics that are
    * included for the IPS instance.
    */
   uint32_t   IPS_Characteristic_Property_Flags;

   /**
    * Flags if the IPS server requires authorization before an IPS
    * characteristic may be written.
    */
   boolean_t  Enable_Authorized_Device_Write;
} qapi_BLE_IPS_Initialize_Data_t;

#define QAPI_BLE_IPS_INITIALIZE_DATA_SIZE                         (sizeof(qapi_BLE_IPS_Initialize_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Initialize_Data_t structure.
 */

   /* The following defines the flags that may be used indicate the     */
   /* characteristics that were read or updated.                        */
#define QAPI_BLE_IPS_CHARACTERISTIC_INDOOR_POSITIONING_CONFIG   (0x0001)
/**< The IP Configuration has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_LATITUDE                    (0x0002)
/**< The Latitude has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_LONGITUDE                   (0x0004)
/**< The Longitude has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_LOCAL_NORTH_COORDINATE      (0x0008)
/**< The Local North Coordinate has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_LOCAL_EAST_COORDINATE       (0x0010)
/**< The Local East Coordinate has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_FLOOR_NUMBER                (0x0020)
/**< The Floor Number has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_ALTITUDE                    (0x0040)
/**< The Altitude has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_UNCERTAINTY                 (0x0080)
/**< The Uncertainty has been read/updated.  */
#define QAPI_BLE_IPS_CHARACTERISTIC_LOCATION_NAME               (0x0100)
/**< The Location Name has been read/updated.  */

/**
 * Enumeration that represents all the events generated by the IPS
 * service. These are used to determine the type of each event generated
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_IPS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_IPS_ET_SERVER_CHARACTERISTIC_READ_E,
   /**< Characteristic has been read event. */
   QAPI_BLE_IPS_ET_SERVER_CHARACTERISTIC_UPDATED_E,
   /**< Characteristic has been updated event. */
   QAPI_BLE_IPS_ET_SERVER_AUTHORIZATION_REQUEST_E
   /**< Authorization request event. */
} qapi_BLE_IPS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an IPS server when an IPS client has read an IPS characteristic.
 *
 * This event is only informative, and may be ignored by the application.
 */
typedef struct qapi_BLE_IPS_Server_Characteristic_Read_Data_s
{
   /**
    * IPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the IPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the IPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the IPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the IPS characteristic that has been read.
    */
   qapi_BLE_IPS_Characteristic_Type_t Type;
} qapi_BLE_IPS_Server_Characteristic_Read_Data_t;

#define QAPI_BLE_IPS_SERVER_CHARACTERISTIC_READ_DATA_SIZE         (sizeof(qapi_BLE_IPS_Server_Characteristic_Read_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Server_Characteristic_Read_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an IPS server when an IPS client has updated an
 * IPS characteristic.
 *
 * More than one IPS characteristic may have been updated. This is
 * identified by the Characteristic field.
 */
typedef struct qapi_BLE_IPS_Server_Characteristic_Updated_Data_s
{
   /**
    * IPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the IPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the IPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the IPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Bitmask of the form QAPI_BLE_IPS_CHARACTERISTIC_XXX, where XXX
    * indicates the IPS characteristic that has been updated. Since
    * multiple IPS characteristics may have been updated (reliable
    * write), the Characteristic field allows the IPS server to determine
    * the IPS characteristics that have been updated.
    */
   uint16_t                        Characteristic;
} qapi_BLE_IPS_Server_Characteristic_Updated_Data_t;

#define QAPI_BLE_IPS_SERVER_CHARACTERISTIC_UPDATED_DATA_SIZE      (sizeof(qapi_BLE_IPS_Server_Characteristic_Updated_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Server_Characteristic_Updated_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an IPS server when an IPS client has prepared data for an IPS
 * characteristic that will be written at a later time.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_IPS_Authorization_Request_Response() function.
 *
 * If the IPS client that made the request is authorized via the
 * qapi_BLE_IPS_Authorization_Request_Response(), the
 * QAPI_BLE_IPS_ET_SERVER_CHARACTERISTIC_UPDATED_E event will be
 * dispatched when the IPS characteristic(s) have been written.
 */
typedef struct qapi_BLE_IPS_Server_Authorization_Request_Data_s
{
   /**
    * IPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the IPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the IPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the IPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the IPS characteristic that has been prepared.
    */
   qapi_BLE_IPS_Characteristic_Type_t Type;
} qapi_BLE_IPS_Server_Authorization_Request_Data_t;

#define QAPI_BLE_IPS_SERVER_AUTHORIZATION_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_IPS_Server_Authorization_Request_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Server_Authorization_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * IPS server event data for an IPS instance.
 */
typedef struct qapi_BLE_IPS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_IPS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;

   union
   {
      /**
       * IPS Characteristic Read event data.
       */
      qapi_BLE_IPS_Server_Characteristic_Read_Data_t    *IPS_Server_Characteristic_Read_Data;

      /**
       * IPS Characteristic Updated event data.
       */
      qapi_BLE_IPS_Server_Characteristic_Updated_Data_t *IPS_Server_Characteristic_Updated_Data;

      /**
       * IPS Characteristic Authorization Request event data.
       */
      qapi_BLE_IPS_Server_Authorization_Request_Data_t  *IPS_Server_Authorization_Request_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_IPS_Event_Data_t;

#define QAPI_BLE_IPS_EVENT_DATA_SIZE                              (sizeof(qapi_BLE_IPS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Event_Data_t structure.
 */

   /* IPS client API Structures, Enums, and Constants.                  */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by an IPS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_IPS_Client_Information_s
{
   /**
    * IPS IP Configuration attribute handle.
    */
   uint16_t Indoor_Positioning_Configuration;

   /**
    * IPS IP Configuration Extended Properties Descriptor attribute
    * handle.
    */
   uint16_t Indoor_Positioning_Configuration_Extended_Properties;

   /**
    * IPS Latitude attribute handle.
    */
   uint16_t Latitude;

   /**
    * IPS Latitude Extended Properties Descriptor attribute handle.
    */
   uint16_t Latitude_Extended_Properties;

   /**
    * IPS Longitude attribute handle.
    */
   uint16_t Longitude;

   /**
    * IPS Longitude Extended Properties Descriptor attribute handle.
    */
   uint16_t Longitude_Extended_Properties;

   /**
    * IPS Local North Coordinate attribute handle.
    */
   uint16_t Local_North_Coordinate;

   /**
    * IPS Local North Coordinate Extended Properties Descriptor
    * attribute handle.
    */
   uint16_t Local_North_Coordinate_Extended_Properties;

   /**
    * IPS Local East Coordinate attribute handle.
    */
   uint16_t Local_East_Coordinate;

   /**
    * IPS Local East Coordinate Extended Properties Descriptor
    * attribute handle.
    */
   uint16_t Local_East_Coordinate_Extended_Properties;

   /**
    * IPS Floor Number attribute handle.
    */
   uint16_t Floor_Number;

   /**
    * IPS Floor Number Extended Properties Descriptor attribute
    * handle.
    */
   uint16_t Floor_Number_Extended_Properties;

   /**
    * IPS Altitude attribute handle.
    */
   uint16_t Altitude;

   /**
    * IPS Altitude Extended Properties Descriptor attribute handle.
    */
   uint16_t Altitude_Extended_Properties;

   /**
    * IPS Uncertainty attribute handle.
    */
   uint16_t Uncertainty;

   /**
    * IPS Uncertainty Extended Properties Descriptor attribute
    * handle.
    */
   uint16_t Uncertainty_Extended_Properties;

   /**
    * IPS Location Name attribute handle.
    */
   uint16_t Location_Name;

   /**
    * IPS Location Name Extended Properties Descriptor attribute
    * handle.
    */
   uint16_t Location_Name_Extended_Properties;
} qapi_BLE_IPS_Client_Information_t;

#define QAPI_BLE_IPS_CLIENT_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_IPS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_IPS_Client_Information_t structure.
 */

/**
 * Structure that represents that data that may be received in the IPS
 * Broadcase data.
 *
 * Decoded does not mean that the encoded values have been decoded,
 * simply that multi-octet values have been converted from
 * Little-Endian.
 *
 * The Flags field is a bitmask of the form
 * QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_XXX, which can be found in
 * qapi_ble_ipstypes.h.
 */
typedef struct qapi_BLE_IPS_Broadcast_Data_s
{
   /**
    * Bitmask that indicates the feature and optional fields that are
    * included for the IPS Broadcast data.
    */
   uint8_t Flags;

   /**
    * Latitude.
    */
   int32_t Latitude;

   /**
    * Longitude.
    */
   int32_t Longitude;

   /**
    * Local North Coordinate.
    */
   int16_t LocalNorthCoordinate;

   /**
    * Local East Coordinate.
    */
   int16_t LocalEastCoordinate;

   /**
    * Transmit power included for the advertising packet.
    */
   uint8_t TxPower;

   /**
    * Floor Number.
    */
   uint8_t FloorNumber;

   /**
    * Altitude.
    */
   int16_t Altitude;

   /**
    * Uncertainty.
    */
   uint8_t Uncertainty;
} qapi_BLE_IPS_Broadcast_Data_t;

#define QAPI_BLE_IPS_BROADCAST_DATA_SIZE                          (sizeof(qapi_BLE_IPS_Broadcast_Data_t))
/**<
 * Size of the #qapi_BLE_IPS_Broadcast_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for an
 * IPS instance event callback. This function will be called
 * whenever an IPS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the IPS instance event data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted, however,
 * that if the same event callback is installed more than once, the
 * event callbacks will be called serially. Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another IPS instance event will not be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for IPS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock will occur because other event callbacks will not be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  IPS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User- defined value that was supplied
 *                                   as an input parameter when the IPS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_IPS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_IPS_Event_Data_t *IPS_Event_Data, uint32_t CallbackParameter);

   /* IPS server API.                                                   */

/**
 * @brief
 * Initializes an IPS instance (IPS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only one IPS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_IPS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, an IPS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register IPS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the IPS server.
 *
 * @param[in]  EventCallback       IPS event callback that will
 *                                 receive IPS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered IPS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 function.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the IPS instance ID of the IPS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the IPS
 *              Instance ID.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_IPS_Initialize_Data_t *InitializeData, qapi_BLE_IPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an IPS instance (IPS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_IPS_Initialize_Service(), this function allows the
 * application to select an attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only one IPS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the #qapi_BLE_IPS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, an IPS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, all fields of the ServiceHandleRange parameter must
 * be initialized to zero. The qapi_BLE_IPS_Query_Number_Attributes()
 * function may be used after initializing an IPS instance to determine
 * the attribute handle range for the IPS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register IPS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the IPS server.
 *
 * @param[in]  EventCallback       IPS event callback that will
 *                                 receive IPS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered IPS instance returned from
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
 *              be the IPS Instance ID of the IPS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the IPS
 *              instance ID.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_IPS_Initialize_Data_t *InitializeData, qapi_BLE_IPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an IPS instance (IPS server).
 *
 * @details
 * After this function is called, no other IPS
 * function can be called until after a successful call to either of the
 * qapi_BLE_IPS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in an IPS instance that is registered with a call
 * to either of the qapi_BLE_IPS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @return   Positive, nonzero, number of attributes for the
 *           registered IPS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Sets the IPS Indoor Positioning
 * Configuration Characteristic.
 *
 * @details
 * The Configuration parameter is a bitmask made up of bits of the form
 * QAPI_BLE_IPS_CONFIGURATION_FLAG_XXX from qapi_ble_ipstypes.h. The
 * Configuration may be zero.
 *
 * If any bits for optional service features that are not included in the
 * service or reserved for future use bits are set this function will
 * return QAPI_BLE_IPS_ERROR_INVALID_CONFIGURATION.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  Configuration       Bitmask for the IP configuration.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Indoor_Positioning_Configuration(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Configuration);

/**
 * @brief
 * Queries the IPS Indoor Positioning
 * Configuration Characteristic.
 *
 * @details
 * The Configuration parameter is a bitmask made up of bits of the form
 * QAPI_BLE_IPS_CONFIGURATION_FLAG_XXX from qapi_ble_ipstypes.h. The
 * Configuration may be zero.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  Configuration      Pointer that will hold the IP
 *                                 configuration if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Indoor_Positioning_Configuration(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Configuration);

/**
 * @brief
 * Sets the IPS Latitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  Latitude            Latitude.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Latitude(uint32_t BluetoothStackID, uint32_t InstanceID, int32_t Latitude);

/**
 * @brief
 * Queries the IPS Latitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  Latitude           Pointer that will hold the IPS
 *                                 Latitude if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Latitude(uint32_t BluetoothStackID, uint32_t InstanceID, int32_t *Latitude);

/**
 * @brief
 * Sets the IPS Longitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  Longitude           Longitude.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Longitude(uint32_t BluetoothStackID, uint32_t InstanceID, int32_t Longitude);

/**
 * @brief
 * Queries the IPS Longitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  Longitude          Pointer that will hold the IPS
 *                                 Longitude if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Longitude(uint32_t BluetoothStackID, uint32_t InstanceID, int32_t *Longitude);

/**
 * @brief
 * Sets the IPS Local North
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  LocalCoordinate     Local Coordinate.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Local_North_Coordinate(uint32_t BluetoothStackID, uint32_t InstanceID, int16_t LocalCoordinate);

/**
 * @brief
 * Queries the IPS Local North
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  LocalCoordinate    Pointer that will hold the IPS
 *                                 Local Coordinate if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Local_North_Coordinate(uint32_t BluetoothStackID, uint32_t InstanceID, int16_t *LocalCoordinate);

/**
 * @brief
 * Sets the IPS Local East
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  LocalCoordinate     Local Coordinate.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Local_East_Coordinate(uint32_t BluetoothStackID, uint32_t InstanceID, int16_t LocalCoordinate);

/**
 * @brief
 * Queries the IPS Local East
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  LocalCoordinate    Pointer that will hold the IPS
 *                                 Local Coordinate if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Local_East_Coordinate(uint32_t BluetoothStackID, uint32_t InstanceID, int16_t *LocalCoordinate);

/**
 * @brief
 * Sets the IPS Floor Number
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  FloorNumber         Floor Number.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Floor_Number(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t FloorNumber);

/**
 * @brief
 * Queries the IPS Floor Number
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  FloorNumber        Pointer that will hold the IPS
 *                                 Floor Number if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Floor_Number(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *FloorNumber);

/**
 * @brief
 * Sets the IPS Altitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  Altitude            Altitude.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Altitude(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t Altitude);

/**
 * @brief
 * Queries the IPS Altitude
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  Altitude           Pointer that will hold the IPS
 *                                 Altitude if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Altitude(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *Altitude);

/**
 * @brief
 * Sets the IPS Uncertainty
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  Uncertainty         Uncertainty.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Uncertainty(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Uncertainty);

/**
 * @brief
 * Queries the IPS Uncertainty
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[out]  Uncertainty        Pointer that will hold the IPS
 *                                 Uncertainty if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Uncertainty(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Uncertainty);

/**
 * @brief
 * Sets the IPS Location Name
 * Characteristic.
 *
 * @details
 * The LocationName parameter must be less than or equal
 * to BTPS_CONFIGURATION_IPS_MAXIMUM_SUPPORTED_STRING_LENGTH in size,
 * including the NULL terminator. Otherwise, the error code
 * QAPI_BLE_IPS_ERROR_INVALID_STRING_LENGTH will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  LocationName        Character string for the Locataion
 *                                 Name.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Set_Location_Name(uint32_t BluetoothStackID, uint32_t InstanceID, char *LocationName);

/**
 * @brief
 * Queries the IPS Location Name
 * Characteristic.
 *
 * @details
 * The LocationName parameter must be large enough to hold the IPS
 * Location Name and the NULL terminator that this function will
 * automatically set. The IPS Location Name cannot be greater than
 * BTPS_CONFIGURATION_IPS_MAXIMUM_SUPPORTED_STRING_LENGTH in size.
 * Otherwise, the error code QAPI_BLE_IPS_ERROR_INVALID_STRING_LENGTH will
 * be returned.
 *
 * If LocationName has not been previuosly set, the error code
 * QAPI_BLE_IPS_ERROR_LOCATION_NAME_NOT_CONFIGURED will be returned. Use
 * qapi_BLE_IPS_Set_Location_Name() to configure the Location Name.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in]  LocationNameLength  Length of the Location Name.
 *
 * @param[out]  LocationName       Character string for the Location
 *                                 Name that will be returned if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Query_Location_Name(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t LocationNameLength, char *LocationName);

/**
 * @brief
 * Responds to a request received
 * from an IPS client for an authorization request to write an IPS
 * Characteristic.
 *
 * @details
 * This event is primarily provided to reject a GATT Prepare Write
 * request for optional security reasons, such as the IPS client has
 * insufficient authentication, authorization, or encryption. This is
 * required if the IPS server needs additional security. Therefore, we
 * will not pass the prepared data up to the application until the
 * GATT Execute Write request has been received by the IPS server, and
 * the prepared writes are not cancelled. If the prepared data is
 * written, the QAPI_BLE_IPS_ET_SERVER_CHARACTERISTIC_UPDATED_E
 * event will be dispatched to the application. Otherwise, the prepared
 * data will be cleared.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  NotAuthorized       Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Authorization_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, boolean_t NotAuthorized);

/**
 * @brief
 * Formats the IPS Broadcast
 * data into a buffer that will be included in the advertising data.
 *
 * This function may also be used to determine the size of the buffer to
 * hold the formatted data.
 *
 * @details
 * The BufferLength parameter is mandatory (cannot be NULL). This
 * parameter cannot be greater than
 * QAPI_BLE_IPS_MAXIMUM_BROADCAST_DATA_SIZE.
 *
 * If the BufferLength parameter is 0, the Buffer parameter may be
 * excluded (NULL), and this function will return a positive nonzero
 * value, which represents the size of the buffer needed to hold the
 * formatted data. The IPS client may use this size to allocate a buffer
 * necessary to hold the formatted data.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the IPS instance.
 *
 * @param[in,out]  BufferLength    Length of the user specified
 *                                 buffer on input, and the size of the
 *                                 formatted buffer on output.
 *
 * @param[out]  Buffer             User-specified buffer.
 *
 * @return      If this function is used to format the user specified
 *              buffer, zero will be returned for success.
 *
 * @return      If this function is used to determine the size of the
 *              user specified buffer to hold the formatted data, a
 *              positive nonzero value will be returned for success,
 *              which represents the minimum length required for the user-
 *              specified buffer to hold the formatted data.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Format_Broadcasting_Data(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *BufferLength, uint8_t *Buffer);

   /* IPS Client API.                                                   */

/**
 * @brief
 * Parses a value received in
 * an advertising data report, interpreting it as the IPS Broadcast data.
 *
 * @details
 * This function Expects the AD_Data_Buffer and AD_Data_Length fields as
 * the Buffer and BufferLength parameters from the
 * #qapi_BLE_GAP_LE_Advertising_Data_Entry_t structure. This structure
 * stores the mandatory Length and AD Type fields separately from the
 * rest of the broadcast information that is in the Buffer.
 *
 * This function cannot be called if the AD_Data_Length field is 1. This
 * indicates that only the AD type Indoor Positioning Service has been
 * included. The Flags field has also been omitted, since no other data
 * has been included.
 *
 * @param[in]  BufferLength   Length of the value received from the
 *                            IPS server.
 *
 * @param[in]  Buffer         Value received from the
 *                            IPS server.
 *
 * @param[out]  BroadcastData   Pointer that will hold the
 *                              IPS Broadcast data if this function is
 *                              successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. IPS error codes can be
 *              found in qapi_ble_ips.h (QAPI_BLE_IPS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_IPS_Decode_Broadcasting_Data(uint8_t BufferLength, uint8_t *Buffer, qapi_BLE_IPS_Broadcast_Data_t *BroadcastData);

/**
 * @}
 */

#endif

