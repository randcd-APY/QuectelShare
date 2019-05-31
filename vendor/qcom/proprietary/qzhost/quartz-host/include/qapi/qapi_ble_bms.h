/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_bms.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Bond Management Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Bond Management Service (BMS) programming interface defines the
 * protocols and procedures to be used to implement the Bond Management
 * Service capabilities.
 */

#ifndef __QAPI_BLE_BMS_H__
#define __QAPI_BLE_BMS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_bmstypes.h"  /* QAPI BMS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_BMS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_BMS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_BMS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_BMS_ERROR_INSUFFICIENT_BUFFER_SPACE     (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_BMS_ERROR_SERVICE_ALREADY_REGISTERED    (-1004)
/**< Service is already registered. */
#define QAPI_BLE_BMS_ERROR_INVALID_INSTANCE_ID           (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_BMS_ERROR_INVALID_FEATURE_FLAGS         (-1006)
/**< Invalid feature flags. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a BMS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_BMS_Client_Information_s
{
   /**
    * BMS control point attribute handle.
    */
   uint16_t BM_Control_Point;

   /**
    * BMS feature attribute handle.
    */
   uint16_t BM_Feature;
} qapi_BLE_BMS_Client_Information_t;

#define QAPI_BLE_BMS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_BMS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_BMS_Client_Information_t structure.
 */

/**
 * Enumeration of the values that may be set as the value
 * for the CommandType field of the BM Control Point Format Data.
 */
typedef enum
{
   QAPI_BLE_BMC_DELETE_BOND_REQUESTING_BREDR_LE_E  = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE,
   /**< Delete the BR/EDR and LE bonds for the requesting device. */
   QAPI_BLE_BMC_DELETE_BOND_REQUESTING_BREDR_E     = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_BREDR,
   /**< Delete the BR/EDR bond for the requesting device. */
   QAPI_BLE_BMC_DELETE_BOND_REQUESTING_LE_E        = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_LE,
   /**< Delete the LE bond for the requesting device. */
   QAPI_BLE_BMC_DELETE_ALL_BONDS_BREDR_LE_E        = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_BREDR_LE,
   /**< Delete all BR/EDR and LE bonds. */
   QAPI_BLE_BMC_DELETE_ALL_BONDS_BREDR_E           = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_BREDR,
   /**< Delete all BR/EDR bonds. */
   QAPI_BLE_BMC_DELETE_ALL_BONDS_LE_E              = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_LE,
   /**< Delete all LE bonds. */
   QAPI_BLE_BMC_DELETE_ALL_OTHER_BONDS_BREDR_LE_E  = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_BREDR_LE,
   /**<
    * Delete all BR/EDR and LE bonds other than the requesting
    * device.
    */
   QAPI_BLE_BMC_DELETE_ALL_OTHER_BONDS_BREDR_E     = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_BREDR,
   /**<
    * Delete all BR/EDR bonds other than the requesting
    * device.
    */
   QAPI_BLE_BMC_DELETE_ALL_OTHER_BONDS_LE_E        = QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_LE
   /**<
    * Delete all LE bonds other than the requesting device.
    */
} qapi_BLE_BMS_BMCP_Command_Type_t;

/**
 * Structure that represents the BMS BM Control Point request data.
 */
typedef struct qapi_BLE_BMS_BM_Control_Point_Format_Data_s
{
   /**
    * BMS command type.
    */
   qapi_BLE_BMS_BMCP_Command_Type_t CommandType;

   /**
    * Authorization code length.
    */
   uint32_t                         AuthorizationCodeLength;

   /**
    * Pointer to the authorization code.
    */
   uint8_t                         *AuthorizationCode;
} qapi_BLE_BMS_BM_Control_Point_Format_Data_t;

#define QAPI_BLE_BMS_BM_CONTROL_POINT_FORMAT_DATA_SIZE   (sizeof(qapi_BLE_BMS_BM_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_BMS_BM_Control_Point_Format_Data_t structure.
 */

/**
 * Enumerations of all the events generated by the BMS
 * service. These are used to determine the type of each event generated
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_BMS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_BMS_BM_CONTROL_POINT_COMMAND_E /**< BMS Control Point request event. */
} qapi_BLE_BMS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a BMS server when a BMS client has sent a request to write a BMS
 * Control Point Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_BMS_BM_Control_Point_Response() function.
 */
typedef struct qapi_BLE_BMS_BM_Control_Point_Command_Data_s
{
   /**
    * BMS instance that dispatched the event.
    */
   uint32_t                                    InstanceID;

   /**
    * GATT connection ID for the connection with the BMS client
    * that made the request.
    */
   uint32_t                                    ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                    TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BMS client.
    */
   qapi_BLE_GATT_Connection_Type_t             ConnectionType;

   /**
    * Bluetooth address of the BMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                          RemoteDevice;

   /**
    * BMS Control Point data that has been requested to be written.
    */
   qapi_BLE_BMS_BM_Control_Point_Format_Data_t FormatData;
} qapi_BLE_BMS_BM_Control_Point_Command_Data_t;

#define QAPI_BLE_BMS_BM_CONTROL_POINT_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_BMS_BM_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_BMS_BM_Control_Point_Command_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a BMS instance.
 */
typedef struct qapi_BLE_BMS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_BMS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * BMS Control Point Request data.
       */
      qapi_BLE_BMS_BM_Control_Point_Command_Data_t *BMS_BM_Control_Point_Command_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_BMS_Event_Data_t;

#define QAPI_BLE_BMS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_BMS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_BMS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * BMS Event Callback. This function will be called whenever a define
 * BMS event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in a #qapi_BLE_BMS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the BMS Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It needs to be noted however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another BMS event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for events that can only be
 * satisfied by receiving other Bluetooth Stack Events. A Deadlock
 * will occur because other callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack on which the
 *                                  event occurred.
 *
 * @param[in]  BMS_Event_Data       Pointer to a structure that contains
 *                                  information about the event that has
 *                                  occurred.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the BMS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_BMS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_BMS_Event_Data_t *BMS_Event_Data, uint32_t CallbackParameter);

   /* BMS server API.                                                   */

/**
 * @brief
 * Initializes a BMS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one BMS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register BMS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_bmstypes.h and have the form
 *                                  QAPI_BLE_BMS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        BMS Event Callback that will
 *                                  receive BMS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered BMS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of BMS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a BMS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_BMS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 * Only one BMS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register BMS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_bmstypes.h and have the form
 *                                  QAPI_BLE_BMS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        BMS Event Callback that will
 *                                  receive BMS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered BMS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer that, on input, holds
 *                                       the handle range to store the
 *                                       service in GATT, and on output,
 *                                       contains the handle range for
 *                                       where the service is stored in
 *                                       GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of BMS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Bond Management Service (BMS) instance.
 *
 * @details
 * After this function is called, no other BMS
 * function can be called until after a successful call to either of the
 * qapi_BLE_BMS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_BMS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BMS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the BMS service that is registered with a
 * call to qapi_BLE_BMS_Initialize_Service() or
 * qapi_BLE_BMS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a BMS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the supported BMS features for the BMS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * BMS_BM_FEATURE_FLAG_XXX.
 *
 * This function must be called after the BMS service is registered with
 * a successful call to qapi_BLE_BMS_Initialize_XXX() in order to set the default
 * features of the BMS server.
 *
 * @note1hang
 * This function will clear any SupportedFeatures that do not correspond
 * to the transport being used by the service. The error
 * QAPI_BLE_BMS_ERROR_INVALID_FEATURE_FLAGS will be returned if all the
 * bits are cleared for the SupportedFeatures.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the BMS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BMS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Set_BM_Features(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

/**
 * @brief
 * Gets the supported BMS features for the BMS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * BMS_BM_FEATURE_FLAG_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @param[out]  SupportedFeatures   Pointer to a bitmask that will
 *                                  hold the supported features of the
 *                                  BMS server if this function is
 *                                  successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BMS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Query_BM_Features(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

/**
 * @brief
 * Responds to a BM Control Point Command
 * received from a BMS client.
 *
 * @details
 * This function is primarily provided to allow a way to reject a BM Control
 * Point Command. This could be rejected for improper authentication,
 * authorization, or encryption. Otherwise this function should send the
 * ResponseCode BMS_ERROR_CODE_SUCCESS. See qapi_ble_bmstypes.h for other
 * BMS error codes and qapi_ble_atttypes.h for all other ATT error codes
 * that may be used.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ResponseCode        Response code to indicate if the
 *                                 request was accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_BM_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ResponseCode);

   /* BMS Client API.                                                   */

/**
 * @brief
 * Formats the BMS BM Control Point
 * Command into a user-specified buffer, for a GATT write request, that will be sent
 * to the BMS server. This function may also be used to determine the size of the
 * buffer to hold the formatted data (see below).
 *
 * @details
 * If the BufferLength parameter is NULL, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data. The BMS
 * client may use this size to allocate a buffer necessary to hold the formatted
 * data.
 *
 * @param[in]  FormatData      Pointer to the BM Control Point Command
 *                             data that will be formatted into the
 *                             user-specified buffer.
 *
 * @param[in]  BufferLength    Length of the user-specified buffer.
 *
 * @param[out] Buffer          User-specified buffer that will hold
 *                             the formatted data if this function is
 *                             successful.
 *
 * @return      Positive nonzero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the FormatData is formatted into the specified buffer.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BMS_Format_BM_Control_Point_Command(qapi_BLE_BMS_BM_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/** @}
 */

#endif

