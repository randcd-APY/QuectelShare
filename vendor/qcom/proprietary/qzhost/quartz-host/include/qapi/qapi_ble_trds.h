/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_ble_services
@{
*/

/**
 * @file qapi_ble_trds.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Transport Discovery Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Transport Discovery Service (TRDS) programming interface defines
 * the protocols and procedures to be used to implement the Transport
 * Discovery Service capabilities.
 */

#ifndef __QAPI_BLE_TRDS_H__
#define __QAPI_BLE_TRDS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_trdstypes.h" /* QAPI TRDS prototypes.                    */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_TRDS_ERROR_INVALID_PARAMETER                     (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_TRDS_ERROR_INVALID_BLUETOOTH_STACK_ID            (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_TRDS_ERROR_INSUFFICIENT_RESOURCES                (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_TRDS_ERROR_SERVICE_ALREADY_REGISTERED            (-1003)
/**< Service already registered. */
#define QAPI_BLE_TRDS_ERROR_INVALID_INSTANCE_ID                   (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_TRDS_ERROR_MALFORMATTED_DATA                     (-1005)
/**< Malformatted data. */
#define QAPI_BLE_TRDS_ERROR_INSUFFICIENT_BUFFER_SPACE             (-1006)
/**< Insufficient buffer space. */
#define QAPI_BLE_TRDS_ERROR_INVALID_CCCD_TYPE                     (-1007)
/**< Invalid CCCD type. */
#define QAPI_BLE_TRDS_ERROR_INVALID_ATTRIBUTE_HANDLE              (-1008)
/**< Invalid attribute handle. */
#define QAPI_BLE_TRDS_ERROR_CONTROL_POINT_NOT_SUPPORTED           (-1009)
/**< Control Point not supported. */
#define QAPI_BLE_TRDS_ERROR_MTU_EXCEEDED                          (-1010)
/**< MTU exceeded. */

/**
 * This structure represents the data that is required to initialize the
 * Transport Discovery Service (a parameter for the
 * qapi_BLE_TRDS_Initialize_Service() and
 * qapi_BLE_TRDS_Initialize_Service_Handle_Range() functions).
 *
 * The Control_Point_Supported field indicates if the TDS Server supports
 * the TDS Control Point.  If this field is TRUE, then a Client
 * Characteristic Configuration Descriptor (CCCD) will automatically be
 * included so each TDS Client may configure the TDS Control Point for
 * indications.
 */
typedef struct qapi_BLE_TRDS_Initialize_Data_s
{
   /**
    * Flags if the Control Point Characteristic is supported by the TDS
    * Server.
    */
   boolean_t Control_Point_Supported;
} qapi_BLE_TRDS_Initialize_Data_t;

#define QAPI_BLE_TRDS_INITIALIZE_DATA_SIZE                        (sizeof(qapi_BLE_TRDS_Initialize_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Initialize_Data_t structure.
 */

/**
 * This structure contains the attribute handles that will need to be
 * cached by a TDS Client in order to only do service discovery once.
 */
typedef struct qapi_BLE_TRDS_Client_Information_s
{
   /**
    * The TDS Control Point attribute handle.
    */
   uint16_t Control_Point;

   /**
    * The TDS Control Point Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t Control_Point_CCCD;
} qapi_BLE_TRDS_Client_Information_t;

#define QAPI_BLE_TRDS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_TRDS_Client_Information_t))
/**<
 * Size of the qapi_BLE_TRDS_Client_Information_t structure.
 */

/**
 * This structure contains all of the per Client data that will need to
 * be stored by a TDS Server.
 *
 * This is ONLY required if the TDS Server supports the TDS Control
 * Point.
 */
typedef struct qapi_BLE_TRDS_Server_Information_s
{
   /**
    * The TDS Control Point Client Characteristic Configuration
    * Descriptor (CCCD).
    */
   uint16_t Control_Point_Configuration;
} qapi_BLE_TRDS_Server_Information_t;

#define QAPI_BLE_TRDS_SERVER_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_TRDS_Server_Information_t))
/**<
 * Size of the qapi_BLE_TRDS_Server_Information_t structure.
 */

/**
 * This enumeration represents the TDS Client Characteristic
 * Configuration Descriptor (CCCD) types.
 */
typedef enum
{
   QAPI_BLE_TRDS_CCT_CONTROL_POINT_E /**< CCT Control Point. */
} qapi_BLE_TRDS_CCCD_Type_t;

/**
 * This enumeration represents the TDS Roles.
 *
 * The Transport Role MUST be set at bits 0 and 1 of the Flags field in
 * the qapi_BLE_TRDS_Transport_Block_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_TRDS_TR_NOT_SPECIFIED_E,      /**< Not specified. */
   QAPI_BLE_TRDS_TR_SEEKER_ONLY_E,        /**< Seeker only. */
   QAPI_BLE_TRDS_TR_PROVIDER_ONLY_E,      /**< Provider only. */
   QAPI_BLE_TRDS_TR_SEEKER_AND_PROVIDER_E /**< Seeker and Provider. */
} qapi_BLE_TRDS_Transport_Role_t;

/**
 * This enumeration represents the TDS Transport State.
 *
 * The Transport Role MUST be set at bits 3 and 4 of the Flags field in
 * the qapi_BLE_TRDS_Transport_Block_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_TRDS_TS_OFF_E,
   /**< Transport state off. */
   QAPI_BLE_TRDS_TS_ON_E,
   /**< Transport state on. */
   QAPI_BLE_TRDS_TS_TEMPORARILY_UNAVAILABLE_E,
   /**< Transport state temporarily unavailable. */
   QAPI_BLE_TRDS_TS_RFU_E
   /**< Transport state RFU. */
} qapi_BLE_TRDS_Transport_State_t;

/**
 * This structure represents the format for the TDS Transport Block data.
 *
 * It is the application's responsibility to make sure that the length of
 * the TDS Transport Block data does not exceed the maximum size of the
 * advertising data.  It is worth noting that multiple TDS Transport
 * Blocks may be included in the advertising data.
 *
 * If a TDS Transport Block is incomplete (meaning it CANNOT all fit in
 * the advertising data), then bit 2 of the Flags field MUST be set to
 * indicate that the Transport Block is incomplete.
 */
typedef struct qapi_BLE_TRDS_Transport_Block_Data_s
{
   /**
    * The TDS Organization ID.
    */
   uint8_t  Organization_ID;

   /**
    * The TDS Flags.
    */
   uint8_t  Flags;

   /**
    * The TDS Transport data length.
    */
   uint8_t  Transport_Data_Length;

   /**
    * The TDS Transport data.
    */
   uint8_t *Transport_Data;
} qapi_BLE_TRDS_Transport_Block_Data_t;

#define QAPI_BLE_TRDS_TRANSPORT_BLOCK_DATA_SIZE                   (sizeof(qapi_BLE_TRDS_Transport_Block_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Transport_Block_Data_t structure.
 */

/**
 * This enumeration represents the valid values that may be set for the
 * Op_Code field of the qapi_BLE_TRDS_RACP_Request_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_TRDS_CPRT_ACTIVATE_TRANSPORT_E = QAPI_BLE_TRDS_CONTROL_POINT_OP_CODE_ACTIVATE_TRANSPORT
   /**< Activate Transport op code. */
} qapi_BLE_TRDS_Control_Point_Request_Type_t;

/**
 * This structure represents the format for the TDS Control Point Request
 * data that may be sent in a TDS Control Point Request.
 *
 * The maximum size of the Parameter_Length field is dependent on the
 * negotiated Maximum Transmission Unit (MTU) size.  Two octets MUST be
 * subtracted from the MTU size for the Mandatory Op_Code and
 * Organization_ID fields.  The remaining length may be used for the
 * Parameter_Length field.
 */
typedef struct qapi_BLE_TRDS_Control_Point_Request_Data_s
{
   /**
    * The request op code.
    */
   qapi_BLE_TRDS_Control_Point_Request_Type_t  Op_Code;

   /**
    * The organization ID.
    */
   uint8_t                                     Organization_ID;

   /**
    * The parameter length.
    */
   uint16_t                                    Parameter_Length;

   /**
    * The parameter.
    */
   uint8_t                                    *Parameter;
} qapi_BLE_TRDS_Control_Point_Request_Data_t;

#define QAPI_BLE_TRDS_CONTROL_POINT_REQUEST_DATA_SIZE             (sizeof(qapi_BLE_TRDS_Control_Point_Request_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Control_Point_Request_Data_t structure.
 */

/**
 * This enumeration represents the valid values that may be set for the
 * Result_Code field of the qapi_BLE_TRDS_Control_Point_Response_Data_t
 * structure.
 */
typedef enum
{
   QAPI_BLE_TRDS_TCP_SUCCESS_E                     = QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_SUCCESS,
   /**< Success result code. */
   QAPI_BLE_TRDS_TCP_OPCODE_NOT_SUPPORTED_E        = QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_OP_CODE_NOT_SUPPORTED,
   /**< Opcode not supported result code. */
   QAPI_BLE_TRDS_TCP_INVALID_PARAMETER_E           = QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_INVALID_PARAMETER,
   /**< Invalid parameter result code. */
   QAPI_BLE_TRDS_TCP_UNSUPPORTED_ORGANIZATION_ID_E = QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_UNSUPPORTED_ORGANIZATION_ID,
   /**< Unsupported organization ID result code. */
   QAPI_BLE_TRDS_TCP_OPERATION_FAILED_E            = QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_OPERATION_FAILED
   /**< Operation failed result code. */
} qapi_BLE_TRDS_Control_Point_Result_Code_t;

/**
 * This structure represents the format for the TDS Control Point
 * Response data that may be sent in a TDS Control Point Response.
 *
 * The maximum size of the Parameter_Length field is dependent on the
 * negotiated Maximum Transmission Unit (MTU) size.  Two octets MUST be
 * subtracted from the MTU size for the Mandatory Op_Code and
 * Organization_ID fields.  The remaining length may be used for the
 * Parameter_Length field.
 */
typedef struct qapi_BLE_TRDS_Control_Point_Response_Data_s
{
   /**
    * The request op code.
    */
   qapi_BLE_TRDS_Control_Point_Request_Type_t  Request_Op_Code;

   /**
    * The result code.
    */
   qapi_BLE_TRDS_Control_Point_Result_Code_t   Result_Code;

   /**
    * The parameter length.
    */
   uint16_t                                    Parameter_Length;

   /**
    * The parameter.
    */
   uint8_t                                    *Parameter;
} qapi_BLE_TRDS_Control_Point_Response_Data_t;

#define QAPI_BLE_TRDS_CONTROL_POINT_RESPONSE_DATA_SIZE            (sizeof(qapi_BLE_TRDS_Control_Point_Response_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Control_Point_Response_Data_t structure.
 */

/**
 * This enumeration represents all the events generated by the TDS for
 * the TDS Server. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * qapi_BLE_TRDS_Event_Data_t structure.
 */
typedef enum qapi_BLE_TRDS_Event_Type_s
{
   QAPI_BLE_TRDS_ET_SERVER_WRITE_CONTROL_POINT_REQUEST_E,
   /**< Write Control Point request event */
   QAPI_BLE_TRDS_ET_SERVER_READ_CCCD_REQUEST_E,
   /**< Read CCCD request event */
   QAPI_BLE_TRDS_ET_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write CCCD request event */
   QAPI_BLE_TRDS_ET_SERVER_CONFIRMATION_E
   /**< Confirmation event */
} qapi_BLE_TRDS_Event_Type_t;

/**
 * This structure represents the format for the data that is dispatched
 * to a TDS Server when a TDS Client has sent a request to write the TRDS
 * Control Point Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_TRDS_Write_Control_Point_Request_Response() function.
 */
typedef struct qapi_BLE_TRDS_Write_Control_Point_Request_Data_s
{
   /**
    * The TDS instance that dispatched the event.
    */
   uint32_t                                   InstanceID;

   /**
    * The GATT Connection ID for the connection with the TDS Client that
    * made the request.
    */
   uint32_t                                   ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the TDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t            ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                                   TransactionID;

   /**
    * The Bluetooth address of the TDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                         RemoteDevice;

   /**
    * The TDS Control Point request data.
    */
   qapi_BLE_TRDS_Control_Point_Request_Data_t RequestData;
} qapi_BLE_TRDS_Write_Control_Point_Request_Data_t;

#define QAPI_BLE_TRDS_WRITE_CONTROL_POINT_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_TRDS_Write_Control_Point_Request_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Write_Control_Point_Request_Data_t
 * structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a TDS Server when a TDS Client has sent a request to read a TDS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD).
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_TRDS_Read_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_TRDS_Read_CCCD_Request_Data_s
{
   /**
    * The TDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the TDS Client that
    * made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the TDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the TDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Identifies the requested CCCD based on the TDS Characteristic type.
    */
   qapi_BLE_TRDS_CCCD_Type_t       Type;
} qapi_BLE_TRDS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_TRDS_READ_CCCD_REQUEST_DATA_SIZE                 (sizeof(qapi_BLE_TRDS_Read_CCCD_Request_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Read_CCCD_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a TDS Server when a TDS Client has sent a request to write a TDS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD).
 *
 * Some of this structures fields will be required when responding to a
 * request using the qapi_BLE_TRDS_Write_CCCD_Request_Response()
 * function.
                                 */
typedef struct qapi_BLE_TRDS_Write_CCCD_Request_Data_s
{
   /**
    * The TDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the TDS Client that
    * made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the TDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the TDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Identifies the requested CCCD based on the TDS Characteristic type.
    */
   qapi_BLE_TRDS_CCCD_Type_t       Type;

   /**
    * The Client Characteristic Configuration Descriptor (CCCD) value
    * that has been requested to be written.
    */
   uint16_t                        Configuration;
} qapi_BLE_TRDS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_TRDS_WRITE_CCCD_REQUEST_DATA_SIZE                (sizeof(qapi_BLE_TRDS_Write_CCCD_Request_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Write_CCCD_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to the TDS Server when a TDS Client has responded to an outstanding
 * indication that was previously sent by the TDS Server.
 */
typedef struct qapi_BLE_TRDS_Confirmation_Data_s
{
   /**
    * The TDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the TDS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the TDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the TDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * The status of the outstanding indication. Values will be of the
    * form QAPI_BLE_GATT_CONFIRMATION_STATUS_XXX and can be found in
    * qapi_ble_gatt.h.
    */
   uint8_t                         Status;

   /**
    * The number of bytes successfully indicated.
    */
   uint16_t                        BytesWritten;
} qapi_BLE_TRDS_Confirmation_Data_t;

#define QAPI_BLE_TRDS_CONFIRMATION_DATA_SIZE                      (sizeof(qapi_BLE_TRDS_Confirmation_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Confirmation_Data_t structure.
 */

/**
 * This structure represents the container structure for holding all the
 * TDS Server event data for a TDS instance.
 */
typedef struct qapi_BLE_TRDS_Event_Data_s
{
   /**
    * The event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_TRDS_Event_Type_t Event_Data_Type;

   /**
    * The total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;

   union
   {
      /**
       * OTS Write TDS Control Point event data.
       */
      qapi_BLE_TRDS_Write_Control_Point_Request_Data_t *TRDS_Write_Control_Point_Request_Data;

      /**
       * TDS Read Client Characteristic Configuration Descriptor
       * (CCCD) event data.
       */
      qapi_BLE_TRDS_Read_CCCD_Request_Data_t           *TRDS_Read_CCCD_Request_Data;

      /**
       * TDS Write Client Characteristic Configuration Descriptor
       * (CCCD) event data.
       */
      qapi_BLE_TRDS_Write_CCCD_Request_Data_t          *TRDS_Write_CCCD_Request_Data;

      /**
       * TDS Confirmation event data.
       */
      qapi_BLE_TRDS_Confirmation_Data_t                *TRDS_Confirmation_Data;
   }
   /**
    * The event data.
    */
   Event_Data;
} qapi_BLE_TRDS_Event_Data_t;

#define QAPI_BLE_TRDS_EVENT_DATA_SIZE                             (sizeof(qapi_BLE_TRDS_Event_Data_t))
/**<
 * Size of the qapi_BLE_TRDS_Event_Data_t structure.
 */

/**
 * @brief
 * The following declared type represents the prototype function for a
 * TDS instance event callback.  This function will be called
 * whenever a TDS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller is free to use the contents of the TDS instance event data
 * ONLY in the context of this callback.  If the caller requires
 * the data for a longer period of time, then the callback function
 * MUST copy the data into another data buffer.
 *
 * This function is guaranteed NOT to be invoked more than once
 * simultaneously for the specified installed callback (i.e. this
 * function DOES NOT have be reentrant).  It needs to be noted however,
 * that if the same event callback is installed more than once, then the
 * event callbacks will be called serially.  Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does NOT own.  Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another TDS instance event will NOT be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function MUST NOT Block and wait for TDS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock WILL occur because other event callbacks will NOT be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  TRDS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User defined value that was supplied
 *                                   as an input parameter when the TDS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_TRDS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_TRDS_Event_Data_t *TRDS_Event_Data, uint32_t CallbackParameter);

   /* TDS Server API.                                                   */

/**
 * @brief
 * This function initializes a TDS instance (TDS Server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only 1 TDS instance may be initialized at a time, per Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_TRDS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly a TDS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register TDS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      A pointer to the data that is used to
 *                                 configure the TDS Server.
 *
 * @param[in]  EventCallback       The TDS event callback that will
 *                                 receive TDS instance events.
 *
 * @param[in]  CallbackParameter   A user-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT Service ID of the
 *                                 registered TDS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the TDS instance ID of the TDS Server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the TDS
 *              Instance ID.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * This function initializes a TDS instance (TDS Server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_TRDS_Initialize_Service(), this function allows the
 * application to select a attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only 1 TDS instance may be initialized at a time, per Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_TDS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly a TDS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, then all fields of the ServiceHandleRange parameter MUST
 * be initialized to zero. The qapi_BLE_TRDS_Query_Number_Attributes()
 * function may be used, after initializing a TDS instance, to determine
 * the attribute handle range for the TDS instance.
 *
 * @param[in]  BluetoothStackID   Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags      Flags that are used to
 *                                 register TDS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData     A pointer to the data that is used to
 *                                 configure the TDS Server.
 *
 * @param[in]  EventCallback      The TDS event callback that will
 *                                 receive TDS instance events.
 *
 * @param[in]  CallbackParameter  A user-defined value that will be received
 *                                 with the specified EventCallback parameter.
 *
 * @param[out] ServiceID          Unique GATT Service ID of the
 *                                 registered TDS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @param[in,out]  ServiceHandleRange  A pointer that on input, holds
 *                                      the handle range to store the
 *                                      service in GATT, and on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the TDS instance ID of the TDS Server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the TDS
 *              Instance ID.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * This function is responsible for cleaning up and freeing all resources
 * associated with a TDS Instance (TDS Server).
 *
 * @details
 * After this function is called, no other TDS
 * function can be called until after a successful call to either of the
 * qapi_BLE_TRDS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for querying the number of attributes
 * that are contained in a TDS instance that is registered with a call
 * to either of the qapi_BLE_TRDS_Initialize_XXX() functions.
 *
 * @details
 * None.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @return   Positive, non-zero, number of attributes for the
 *           registered TDS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION uint32_t QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a TDS Client, for the TDS Control Point (TCP)
 * Characteristic.
 *
 * This function does NOT indicate the result of the procedure. If the
 * request is accepted, then it indicates that the Procedure has
 * started. An indication MUST be sent by the TDS Server once the RACP
 * Procedure has completed. The
 * qapi_BLE_TRDS_Indicate_Control_Point_Response() function MUST be used to
 * indicate the result of the Procedure.
 *
 * @details
 * This function is primarily provided to allow a way to reject the
 * Control Point write request when the Control Point Client
 * Characteristic Configuration Descriptor (CCCD) has not been configured
 * for indications, the TDS Client does not have proper Authentication,
 * Authorization, or Encryption to write the Control Point, or a Control
 * Point request is already in progress.  All other reasons should return
 * QAPI_BLE_TRDS_ERROR_CODE_SUCCESS for the ErrorCode and then call the
 * qapi_BLE_TRDS_Indicate_Control_Point_Response() to indicate the
 * response  once the procedure has completed.
 *
 * The ErrorCode parameter MUST be a valid value from qapi_ble_trdstypes.h
 * (TDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Write_Control_Point_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * This function is responsible for indicating the TDS Control Point
 * Procedure result to a TDS Client.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * TDS Client will receive the value, since the TDS Client MUST confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the Control
 * Point Client Characteristic Configuration Descriptor (CCCD) has been
 * previously configured for indications. A TDS Client MUST have written
 * the TDS Control Point Characteristic's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @param[in]  ConnectionID        The GATT connection ID for the
 *                                 connection with the TDS Client.
 *
 * @param[in]  ResponseData        A pointer to the RACP response data
 *                                 that will be sent in the indication.
 *
 * @return      Positive non-zero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Indicate_Control_Point_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData);

/**
 * @brief
 * This function is responsible for responding to a read request,
 * received from a TDS Client, for a TDS Characteristic's Client
 * Characteristic Configuration Descriptor (CCCD).
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_trdstypes.h (TDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter may be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the Client Characteristic
 *                                 Configuration descriptor (CCCD).
 *
 * @param[in]  Configuration       The Client Characteristic
 *                                 Configuration descriptor (CCCD) that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type, uint16_t Configuration);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a TDS Client, for a TDS Characteristic's Client
 * Characteristic Configuration Descriptor (CCCD).
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_trdstypes.h (TDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the TDS instance.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the Client Characteristic
 *                                 Configuration descriptor (CCCD).
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type);

   /* TDS Client API.                                                   */

/**
 * @brief
 * The following function is responsible for formatting a TDS
 * Control Point Characteristic into a buffer, for a GATT Write
 * request, that will be sent to the TDS Server.
 *
 * This function may also be used to determine the size of the buffer to
 * hold the formatted data (see details).
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be
 * excluded (NULL), and this function will return a positive non-zero
 * value, which represents the size of the buffer needed to hold the
 * formatted data.  The TDS Client may use this size to allocate a buffer
 * necessary to hold the formatted data.
 *
 * @param[in]  RequestData    A pointer to the TDS Control Point
 *                            data.
 *
 * @param[in]  BufferLength   The length of the user specified buffer.
 *
 * @param[out]  Buffer        The user specified buffer.
 *
 * @return      If this function is used to format the user specified
 *              buffer, then zero will be returned for success.
 *
 * @return      If this function is used to determine the size of the
 *              user specified buffer to hold the formatted data, then a
 *              positive non-zero value will be returned for success,
 *              which represents the minimum length required for the user
 *              specified buffer to hold the formatted data.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Format_Control_Point_Request(qapi_BLE_TRDS_Control_Point_Request_Data_t *RequestData, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * The following function is responsible for parsing a value received in
 * a GATT Indication from a remote TDS Server interpreting it as the TDS
 * Control Point Procedure result.
 *
 * @details
 * None.
 *
 * @param[in]  ValueLength   The length of the Value received from the
 *                           TDS Server.
 *
 * @param[in]  Value         The Value received from the
 *                           TDS Server.
 *
 * @param[out]  ResponseData   A pointer that will hold the
 *                             TDS Control Point response
 *                             data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Decode_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData);

   /* TDS Server and Client API.                                        */

/**
 * @brief
 * The following function is responsible for formatting a TDS Transport
 * Block into a buffer, that will be included in the advertising data.
 *
 * This function may also be used to determine the size of the buffer to
 * hold the formatted data (see details).
 *
 * @details
 * The Buffer and BufferLength parameters should be set to the location
 * in the advertising data for the TDS Transport Block and the remaining
 * length of the advertising data. This way the caller does NOT have to
 * allocate memory for the user specified buffer. The advertising data is
 * a fixed size that MUST not be exceeded.
 *
 * If the BufferLength parameter is 0, the Buffer parameter may be
 * excluded (NULL), and this function will return a positive non-zero
 * value, which represents the size of the buffer needed to hold the
 * formatted data.  The TDS Client may use this size to allocate a buffer
 * necessary to hold the formatted data.
 *
 * This function may be called repeatedly to format multiple TDS
 * Transport Blocks into the buffer.
 *
 * It is the application's responsibility to make sure that the size of
 * the advertising data is NOT exceeded.
 *
 * @param[in]  TransportBlockData    A pointer to the TDS Transport Block
 *                                   data.
 *
 * @param[in]  BufferLength   The length of the user specified buffer.
 *
 * @param[out]  Buffer        The user specified buffer.
 *
 * @return      If this function is used to format the user specified
 *              buffer, then zero will be returned for success.
 *
 * @return      If this function is used to determine the size of the
 *              user specified buffer to hold the formatted data, then a
 *              positive non-zero value will be returned for success,
 *              which represents the minimum length required to format
 *              the TDS Transport Block into the user specified buffer.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Format_Transport_Block(qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * The following function is responsible for parsing a value received in
 * an advertising report from a remote device interpreting it as a TDS
 * Transport Block.
 *
 * @details
 * If the Transport_Data_Length field of the
 * qapi_BLE_TRDS_Transport_Block_Data_t struture is non-zero on
 * successful return from this function, then the Transport Data has been
 * received in the TDS Transport Block.  The Transport_Data field MUST be
 * copied on successful return from this function if the application
 * needs to store the value.  This pointer is ONLY VALID, while the
 * Buffer parameter is Valid since it points to the location of the
 * Transport Data.
 *
 * This function may be called repeatedly to decode multiple TDS
 * Transport Blocks.  The TDS Transport Blocks are located one after the
 * other, and the positive return value for success may be used to update
 * the remaining length and location in the advertising data for the next
 * TDS Transport Block to decode.
 *
 * @param[in]  RemainingLength   The remaining length of the TDS
 *                               Transport Block data.
 *
 * @param[in]  Buffer            A pointer to the location of the TDS
 *                               Transport Block data.
 *
 * @param[out]  TransportBlockData   A pointer that will hold the
 *                                   TDS Transport Block data if
 *                                   this function is successful.
 *
 * @return      A positive non-zero value, which represents the length of
 *              data decoded from the user specified buffer. This may be
 *              used to decode another TDS Transport Block.
 *
 * @return      An error code if negative. TDS error codes can be
 *              found in qapi_ble_trds.h (QAPI_BLE_TRDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TRDS_Decode_Transport_Block(uint32_t RemainingLength, uint8_t *Buffer, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData);

#endif

/**
 *  @}
 */
