/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_hids.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Human Interface Device Service
 * (HIDS) (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Human Interface Device Service (HIDS) programming interface defines the protocols and
 * procedures to be used to implement the Human Interface Device Service
 * capabilities.
 */

#ifndef __QAPI_BLE_HIDS_H__
#define __QAPI_BLE_HIDS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_hidstypes.h" /* QAPI HIDS prototypes.                    */

/**
 * @addtogroup qapi_ble_services
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER            (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_HIDS_ERROR_INVALID_BLUETOOTH_STACK_ID   (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_HIDS_ERROR_INSUFFICIENT_RESOURCES       (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_HIDS_ERROR_SERVICE_ALREADY_REGISTERED   (-1003)
/**< Service is already registered. */
#define QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID          (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_HIDS_ERROR_MALFORMATTED_DATA            (-1005)
/**< Malformatted data. */
#define QAPI_BLE_HIDS_ERROR_INDICATION_OUTSTANDING       (-1006)
/**< Indication outstanding. */
#define QAPI_BLE_HIDS_ERROR_UNKNOWN_ERROR                (-1007)
/**< Unknown error. */

   /* The follwoing defines the values of the Flags parameter that is   */
   /* provided in the qapi_BLE_HIDS_Initialize_Service() Function.      */
#define QAPI_BLE_HIDS_FLAGS_SUPPORT_MOUSE                        0x01
/**< HIDS supports mouse. */
#define QAPI_BLE_HIDS_FLAGS_SUPPORT_KEYBOARD                     0x02
/**< HIDS supports keyboard. */
#define QAPI_BLE_HIDS_FLAGS_BOOT_MOUSE_INPUT_REPORT_WRITABLE     0x04
/**< HIDS supports writable mouse input reports. */
#define QAPI_BLE_HIDS_FLAGS_BOOT_KEYBOARD_INPUT_REPORT_WRITABLE  0x08
/**< HIDS supports writable keyboard input reports. */

/**
 * Structure that represents the format of a Report Reference.
 *
 * The Report Type is an enumerated type that must be in the format
 * HIDS_REPORT_REFERENCE_REPORT_TYPE_XXX.
 */
typedef struct qapi_BLE_HIDS_Report_Reference_Data_s
{
   /**
    * Report ID.
    */
   uint8_t ReportID;

   /**
    * Report type.
    */
   uint8_t ReportType;
} qapi_BLE_HIDS_Report_Reference_Data_t;

#define QAPI_BLE_HIDS_REPORT_REFERENCE_DATA_SIZE         (sizeof(qapi_BLE_HIDS_Report_Reference_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Report_Reference_Data_t structure.
 */

   /* The following define an additional bit mask that may be set in the*/
   /* ReportType member of the qapi_BLE_HIDS_Report_Reference_Data_t    */
   /* structure.  This bit mask is only valid if the ReportType is set  */
   /* to QAPI_BLE_HIDS_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT and if */
   /* set specifies that the Input Report is writable (an optional      */
   /* feature).                                                         */
   /* * NOTE * This may ONLY be set when this structure is passed to    */
   /*          qapi_BLE_HIDS_Initialize_Service().                      */
#define QAPI_BLE_HIDS_REPORT_REFERENCE_SUPPORT_INPUT_WRITE_FEATURE      0x80
/**< HIDS report reference supports the input write feature. */

/**
 * Structure that represents the format of a HID Information.
 *
 * The Flags Type is an enumerated type that must be in the format
 * QAPI_BLE_HIDS_HID_INFORMATION_FLAGS_XXX.
 */
typedef struct qapi_BLE_HIDS_HID_Information_Data_s
{
   /**
    * HID Version.
    */
   uint16_t Version;

   /**
    * Country Code.
    */
   uint8_t  CountryCode;

   /**
    * Bitmask that specifies the capabilities of the device.
    */
   uint8_t  Flags;
} qapi_BLE_HIDS_HID_Information_Data_t;

#define QAPI_BLE_HIDS_INFORMATION_DATA_SIZE              (sizeof(qapi_BLE_HIDS_HID_Information_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_HID_Information_Data_t structure.
 */

/**
 * Enumeration that represents the valid Protocol Modes that are allowed
 * by HID.
 */
typedef enum
{
   QAPI_BLE_PM_BOOT_E,  /**< Boot protocol mode. */
   QAPI_BLE_PM_REPORT_E /**< Report protocol mode. */
} qapi_BLE_HIDS_Protocol_Mode_t;

/**
 * Enumeration that represents the valid Control Commands that may be
 * sent or received.
 */
typedef enum
{
   QAPI_BLE_PC_SUSPEND_E,      /**< Suspend. */
   QAPI_BLE_PC_EXIT_SUSPEND_E  /**< Exit suspend. */
} qapi_BLE_HIDS_Control_Point_Command_t;

/**
 * Enumeration that represents the valid report types that may be
 * dispatched in a Read or Write request to denote the report value that
 * the remote Host is trying to Read/Write.
 */
typedef enum
{
   QAPI_BLE_RT_REPORT_E,
   /**< Report. */
   QAPI_BLE_RT_BOOT_KEYBOARD_INPUT_REPORT_E,
   /**< Boot Keyboard input report. */
   QAPI_BLE_RT_BOOT_KEYBOARD_OUTPUT_REPORT_E,
   /**< Boot Keyboard output report. */
   QAPI_BLE_RT_BOOT_MOUSE_INPUT_REPORT
   /**< Boot Mouse input report. */
} qapi_BLE_HIDS_Report_Type_t;

/**
 * Enumeration that represents all the events generated by the HIDS
 * Profile.  These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * qapi_BLE_HIDS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_HIDS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_HIDS_SERVER_CLIENT_CONFIGURATION_UPDATE_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_HIDS_SERVER_GET_PROTOCOL_MODE_REQUEST_E,
   /**< Get protocol mode request event. */
   QAPI_BLE_ET_HIDS_SERVER_SET_PROTOCOL_MODE_REQUEST_E,
   /**< Set protocol mode request event. */
   QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_MAP_REQUEST_E,
   /**< Get report map request event. */
   QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_REQUEST_E,
   /**< Get report request event. */
   QAPI_BLE_ET_HIDS_SERVER_SET_REPORT_REQUEST_E,
   /**< Set report request event. */
   QAPI_BLE_ET_HIDS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E
   /**< Control point command indication event. */
} qapi_BLE_HIDS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an HIDS server when an HIDS client has sent a request to read an HIDS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HIDS_Client_Configuration_Read_Response() function.
 *
 * Only the following characteristic types may be returned
 * in this event:
 *
 *  @li QAPI_BLE_RT_REPORT_E (Input Report Type Only)
 *  @li QAPI_BLE_RT_BOOT_KEYBOARD_INPUT_REPORT_E
 *  @li QAPI_BLE_RT_BOOT_MOUSE_INPUT_REPORT
 *
 * The ReportReferenceData member is only valid if ReportType is set to
 * rtReport.
 */
typedef struct qapi_BLE_HIDS_Read_Client_Configuration_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                              InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                              ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                              TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t       ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                    RemoteDevice;

   /**
    * HIDS report type.
    */
   qapi_BLE_HIDS_Report_Type_t           ReportType;

   /**
    * Report reference data.
    */
   qapi_BLE_HIDS_Report_Reference_Data_t ReportReferenceData;
} qapi_BLE_HIDS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_HIDS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_HIDS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a HIDS server when a HIDS client has sent a request to write a HIDS
 * Characteristic's CCCD.
 *
 * Only the following characteristic types may be returned
 * in this event:
 *
 *  @li QAPI_BLE_RT_REPORT_E (Input Report Type Only)
 *  @li QAPI_BLE_RT_BOOT_KEYBOARD_INPUT_REPORT_E
 *  @li QAPI_BLE_RT_BOOT_MOUSE_INPUT_REPORT
 *
 * The ReportReferenceData member is only valid if ReportType is set to
 * rtReport.
 */
typedef struct qapi_BLE_HIDS_Client_Configuration_Update_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                              InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                              ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t       ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                    RemoteDevice;

   /**
    * HIDS report type.
    */
   qapi_BLE_HIDS_Report_Type_t           ReportType;

   /**
    * Report reference data.
    */
   qapi_BLE_HIDS_Report_Reference_Data_t ReportReferenceData;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                              ClientConfiguration;
} qapi_BLE_HIDS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_HIDS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_HIDS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Client_Configuration_Update_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HIDS server when an HIDS client has sent a request to read the
 * HIDS Protocol Mode.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HIDS_Get_Protocol_Mode_Response() function.
 */
typedef struct qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t;

#define QAPI_BLE_HIDS_GET_PROTOCOL_MODE_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a HIDS server when an HIDS client has sent a request to write the
 * HIDS Protocol Mode.
 */
typedef struct qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * HIDS Protocol Mode that has been requested to be written.
    */
   qapi_BLE_HIDS_Protocol_Mode_t   ProtocolMode;
} qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t;

#define QAPI_BLE_HIDS_SET_PROTOCOL_MODE_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a HIDS server when a HIDS client has sent a request to read the
 * HIDS report Map.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HIDS_Get_Report_Map_Response() function.
 */
typedef struct qapi_BLE_HIDS_Get_Report_Map_Request_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Report map offset.
    */
   uint16_t                        ReportMapOffset;
} qapi_BLE_HIDS_Get_Report_Map_Request_Data_t;

#define QAPI_BLE_HIDS_GET_REPORT_MAP_REQUEST_DATA_SIZE   (sizeof(qapi_BLE_HIDS_Get_Report_Map_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Get_Report_Map_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HIDS server when an HIDS client has sent a request to read a
 * HIDS report.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HIDS_Get_Report_Response() function.
 *
 * The ReportReferenceData member is only valid if ReportType is set to
 * QAPI_BLE_RT_REPORT_E.
 */
typedef struct qapi_BLE_HIDS_Get_Report_Request_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                              InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                              ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                              TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t       ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                    RemoteDevice;

   /**
    * Report offset.
    */
   uint16_t                              ReportOffset;

   /**
    * Report type.
    */
   qapi_BLE_HIDS_Report_Type_t           ReportType;

   /**
    * The report reference data.
    */
   qapi_BLE_HIDS_Report_Reference_Data_t ReportReferenceData;
} qapi_BLE_HIDS_Get_Report_Request_Data_t;

#define QAPI_BLE_HIDS_GET_REPORT_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_HIDS_Get_Report_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Get_Report_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HIDS server when an HIDS client has sent a request to write an
 * HIDS report.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HIDS_Set_Report_Response() function.
 *
 * The ReportReferenceData member is only valid if ReportType is set to
 * QAPI_BLE_RT_REPORT_E.
 *
 * The ReportReferenceData member is a report reference structure (only
 * valid if ReportType is set to QAPI_BLE_RT_REPORT_E) that contains the
 * Report ID and Report Type of the Report that is being written.
 */
typedef struct qapi_BLE_HIDS_Set_Report_Request_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                               InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                               ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                               TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t        ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                     RemoteDevice;

   /**
    * Report type.
    */
   qapi_BLE_HIDS_Report_Type_t            ReportType;

   /**
    * Report reference data.
    */
   qapi_BLE_HIDS_Report_Reference_Data_t  ReportReferenceData;

   /**
    * Report length.
    */
   uint32_t                               ReportLength;

   /**
    * Pointer to the report data.
    */
   uint8_t                               *Report;
} qapi_BLE_HIDS_Set_Report_Request_Data_t;

#define QAPI_BLE_HIDS_SET_REPORT_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_HIDS_Set_Report_Request_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Set_Report_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HIDS server when an HIDS client has sent a request to write the
 * HIDS Control Point.
 */
typedef struct qapi_BLE_HIDS_Control_Point_Command_Data_s
{
   /**
    * HIDS instance that dispatched the event.
    */
   uint32_t                               InstanceID;

   /**
    * GATT connection ID for the connection with the HIDS client
    * that made the request.
    */
   uint32_t                               ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HIDS client.
    */
   qapi_BLE_GATT_Connection_Type_t        ConnectionType;

   /**
    * Bluetooth address of the HIDS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                     RemoteDevice;

   /**
    * HIDS Control Point command.
    */
   qapi_BLE_HIDS_Control_Point_Command_t ControlPointCommand;
} qapi_BLE_HIDS_Control_Point_Command_Data_t;

#define QAPI_BLE_HIDS_CONTROL_POINT_COMMAND_DATA_SIZE    (sizeof(qapi_BLE_HIDS_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Control_Point_Command_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for an HIDS instance.
 */
typedef struct qapi_BLE_HIDS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_HIDS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * HIDS Read CCCD data.
       */
      qapi_BLE_HIDS_Read_Client_Configuration_Data_t   *HIDS_Read_Client_Configuration_Data;

      /**
       * HIDS Write CCCD data.
       */
      qapi_BLE_HIDS_Client_Configuration_Update_Data_t *HIDS_Client_Configuration_Update_Data;

      /**
       * HIDS Get Protocol Mode Request data.
       */
      qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t   *HIDS_Get_Protocol_Mode_Request_Data;

      /**
       * HIDS Set Protocol Mode Request data.
       */
      qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t   *HIDS_Set_Protocol_Mode_Request_Data;

      /**
       * HIDS Get Report Map Request data.
       */
      qapi_BLE_HIDS_Get_Report_Map_Request_Data_t      *HIDS_Get_Report_Map_Data;

      /**
       * HIDS Get Report Request data.
       */
      qapi_BLE_HIDS_Get_Report_Request_Data_t          *HIDS_Get_Report_Request_Data;

      /**
       * HIDS Set Report Request data.
       */
      qapi_BLE_HIDS_Set_Report_Request_Data_t          *HIDS_Set_Report_Request_Data;

      /**
       * HIDS Control Point Command data.
       */
      qapi_BLE_HIDS_Control_Point_Command_Data_t       *HIDS_Control_Point_Command_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_HIDS_Event_Data_t;

#define QAPI_BLE_HIDS_EVENT_DATA_SIZE                    (sizeof(qapi_BLE_HIDS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_HIDS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * HIDS Event Callback. This function will be called whenever a define
 * HIDS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an qapi_BLE_HIDS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the L2CAP Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another Data Buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another HIDS Event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for events that can only be
 * satisfied by receiving other Bluetooth Stack Events. A Deadlock
 * will occur because other callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  HIDS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the HIDS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_HIDS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_HIDS_Event_Data_t *HIDS_Event_Data, uint32_t CallbackParameter);

   /* HIDS server API.                                                  */

/**
 * @brief
 * Initializes an HIDS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one HIDS server may be open at a time, per the Bluetooth Stack ID.
 *
 * The ServiceIDList parameter must contain valid service IDs of services that
 * have already been registered with GATT
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack via a
 *                                   call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                 Flags that are used to register HIDS
 *                                   for the specified transport. These
 *                                   flags can be found in
 *                                   qapi_ble_hidstypes.h and have the
 *                                   form QAPI_BLE_HIDS_FLAGS_XXX.
 *
 * @param[in]  HIDInformation        Pointer that contains the
 *                                   information about the HIDS.
 *
 * @param[in]  NumIncludedServices   Number of services that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  ServiceIDList         List of services that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  NumExternalReportReferences   Number of UUIDs that
 *                                           will be referenced for the
 *                                           HIDS server.
 *
 * @param[in]  ReferenceUUID         UUIDs that will be referenced
 *                                   for the HIDS server.
 *
 * @param[in]  NumReports            Number of reports that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  ReportReference       Reports that will be included
 *                                   for the HIDS server.
 *
 * @param[in]  EventCallback         HIDS Event Callback that will
 *                                   receive HIDS server events.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the specified
 *                                   EventCallback parameter.
 *
 * @param[out]  ServiceID            Unique GATT service ID of the
 *                                   registered HIDS service returned
 *                                   from
 *                                   qapi_BLE_GATT_Register_Service()
 *                                   API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HIDS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Initialize_Service(uint32_t BluetoothStackID, uint8_t Flags, qapi_BLE_HIDS_HID_Information_Data_t *HIDInformation, uint32_t NumIncludedServices, uint32_t *ServiceIDList, uint32_t NumExternalReportReferences, qapi_BLE_GATT_UUID_t *ReferenceUUID, uint32_t NumReports, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReference, qapi_BLE_HIDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an HIDS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_HIDS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one HIDS server may be open at a time, per the Bluetooth Stack ID.
 *
 * The ServiceIDList parameter must contain valid ServiceIDs of services that
 * have already been registered with GATT
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack via a
 *                                   call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                 Flags that are used to register HIDS
 *                                   for the specified transport. These
 *                                   flags can be found in
 *                                   qapi_ble_hidstypes.h and have the
 *                                   form QAPI_BLE_HIDS_FLAGS_XXX.
 *
 * @param[in]  HIDInformation        Pointer that contains the
 *                                   information about the HIDS.
 *
 * @param[in]  NumIncludedServices   Number of services that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  ServiceIDList         List of services that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  NumExternalReportReferences   Number of UUIDs that
 *                                           will be referenced for the
 *                                           HIDS server.
 *
 * @param[in]  ReferenceUUID         UUIDs that will be referenced
 *                                   for the HIDS server.
 *
 * @param[in]  NumReports            Number of reports that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  ReportReference       Reports that will be included
 *                                   for the HIDS server.
 *
 * @param[in]  EventCallback         HIDS Event Callback that will
 *                                   receive HIDS server events.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the specified
 *                                   EventCallback parameter.
 *
 * @param[out]  ServiceID            Unique GATT Service ID of the
 *                                   registered HIDS service returned
 *                                   from the
 *                                   qapi_BLE_GATT_Register_Service()
 *                                   API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer that, on input, holds
 *                                       the handle range to store the
 *                                       service in GATT, and on output,
 *                                       contains the handle range for
 *                                       where the service is stored in
 *                                       GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HIDS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint8_t Flags, qapi_BLE_HIDS_HID_Information_Data_t *HIDInformation, uint32_t NumIncludedServices, uint32_t *ServiceIDList, uint32_t NumExternalReportReferences, qapi_BLE_GATT_UUID_t *ReferenceUUID, uint32_t NumReports, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReference, qapi_BLE_HIDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Human Interface Device Service (HIDS) Instance.
 *
 * @details
 * After this function is called, no other Device Information Service
 * function can be called until after a successful call to either of the
 * qapi_BLE_HIDS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the HIDS service that is registered with a
 * call to either the qapi_BLE_HIDS_Initialize_Service() or the
 * qapi_BLE_HIDS_Initialize_Service_Handle_Range() function.
 *
 * @details
 * This function will return a variable number of attributes depending
 * on how the service was configured.
 *
 * @param[in]  Flags                 Flags used to control what the HIDS
 *                                   server supports (these have the form
 *                                   QAPI_BLE_HIDS_FLAGS_SUPPORT_XXX).
 *
 * @param[in]  NumIncludedServices   Number of services that will be
 *                                   included for the HIDS server.
 *
 * @param[in]  NumExternalReportReferences   Number of UUIDs that are
 *                                           referenced for the HIDS server.
 *
 * @param[in]  NumReports            Number of reports that are
 *                                   included for the HIDS server.
 *
 * @param[in]  ReportReference       Reports that are included for
 *                                   the HIDS server.
 *
 * @return      Positive nonzero if successful (represents the number of attributes
 *              for HIDS).
 *
 * @return      Zero for failure.
 */
QAPI_BLE_DECLARATION uint32_t QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Query_Number_Attributes(uint8_t Flags, uint32_t NumIncludedServices, uint32_t NumExternalReportReferences, uint32_t NumReports, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReference);

/**
 * @brief
 * Responds to a read
 * request from an HIDS client for a HIDS characteristic's CCCD.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Client_Configuration   Current value of the CCCD
 *                                    that will be sent
 *                                    in the response.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

/**
 * @brief
 * Responds to a read
 * request from an HIDS client for the current Protocol Mode.
 *
 * @details
 * If ErrorCode is 0, this function
 * will respond to the Get Protocol Mode Request successfully. If
 * ErrorCode is nonzero, this function will respond with an error
 * with the error code set to the parameter.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CurrentProtocolMode   Enumeration for the current
 *                                   protocol mode that will be sent if
 *                                   the request is successful. This
 *                                   will be ignored if the request is
 *                                   rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Get_Protocol_Mode_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_HIDS_Protocol_Mode_t CurrentProtocolMode);

/**
 * @brief
 * Responds to a read
 * request from an HIDS client for an HIDS report Map.
 *
 * @details
 * If ErrorCode is 0, this function
 * will respond to the Get Protocol Mode Request successfully. If
 * ErrorCode is nonzero, this function will respond with an error
 * with the error code set to the parameter.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  ReportMapLength     Lngth of the report that will
 *                                 be sent in the response if the
 *                                 request is successful. This will be
 *                                 ignored if the request is rejected.
 *
 * @param[in]  ReportMap           Pointer the Report Map that will
 *                                 be sent if the request is
 *                                 successful. This may be exlucded
 *                                 (NULL) if the request is rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Get_Report_Map_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint32_t ReportMapLength, uint8_t *ReportMap);

/**
 * @brief
 * Responds to a read
 * request from an HIDS client for an HIDS report.
 *
 * @details
 * If ErrorCode is 0, this function
 * will respond to the Get Protocol Mode Request successfully. If
 * ErrorCode is nozero, this function will respond with an error
 * with the error code set to the parameter.
 *
 * The ReportReferenceData member is only valid if ReportType is set
 * to QAPI_BLE_RT_REPORT_E.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ReportType          Enumeration for the Report Type
 *                                 that has been requested by the
 *                                 client.
 *
 * @param[in]  ReportReferenceData   Pointer to the report reference
 *                                   data. See note above.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  ReportLength        Length of the report that will
 *                                 be sent in the response if the
 *                                 request is successful. This will be
 *                                 ignored if the request is rejected.
 *
 * @param[in]  Report              Pointer the Report that will
 *                                 be sent if the request is
 *                                 successful. This may be exlucded
 *                                 (NULL) if the request is rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Get_Report_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, qapi_BLE_HIDS_Report_Type_t ReportType, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReferenceData, uint8_t ErrorCode, uint32_t ReportLength, uint8_t *Report);

/**
 * @brief
 * Responds to a write
 * request from an HIDS client for an HIDS report.
 *
 * @details
 * If ErrorCode is 0, this function
 * will respond to the Get Protocol Mode Request successfully. If
 * ErrorCode is nonzero, this function will respond with an error
 * with the error code set to the parameter.
 *
 * The ReportReferenceData member is only valid if ReportType is set
 * to QAPI_BLE_RT_REPORT_E.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ReportType          Enumeration for the Report Type
 *                                 that has been requested by the
 *                                 client.
 *
 * @param[in]  ReportReferenceData   Pointer to the report reference
 *                                   data. See note above.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Set_Report_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, qapi_BLE_HIDS_Report_Type_t ReportType, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReferenceData, uint8_t ErrorCode);

/**
 * @brief
 * Responds to a write
 * request from an HIDS client for an HIDS report.
 *
 * @details
 * Only the following characteristic types may be returned
 * in this event:
 *
 *  QAPI_BLE_RT_REPORT_E (Input Report Type Only)
 *  QAPI_BLE_RT_BOOT_KEYBOARD_INPUT_REPORT_E
 *  QAPI_BLE_RT_BOOT_MOUSE_INPUT_REPORT
 *
 * If the ReportType is QAPI_BLE_RT_REPORT_E, the ReportReferenceData
 * must be valid and point to a Report Reference structure of the Input
 * Report to be notified. Otherwise, the ReportReferenceData parameter can
 * be NULL.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HIDS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 request.
 *
 * @param[in]  ReportType          Enumeration for the Report Type
 *                                 that is going to be notified.
 *
 * @param[in]  ReportReferenceData   Pointer to the report reference
 *                                   data that is going to be
 *                                   notified. See note above.
 *
 * @param[in]  InputReportLength   Length of the input report that
 *                                 will be notified.
 *
 * @param[in]  InputReportData     Pointer to the input report that
 *                                 will be notified.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HIDS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Notify_Input_Report(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HIDS_Report_Type_t ReportType, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReferenceData, uint16_t InputReportLength, uint8_t *InputReportData);

   /* HIDS Client API.                                                  */

/**
 * @brief
 * Parses a value received from
 * a remote HIDS server interpreting it as HID Information.
 *
 * @details
 * This function should be used to decode the HID Information received in
 * a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HIDS server.
 *
 * @param[in]  Value          Value received from the
 *                            HIDS server.
 *
 * @param[out]  HIDSHIDInformation   Pointer that will hold the
 *                                   HID Information if this function
 *                                   returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Decode_HID_Information(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HIDS_HID_Information_Data_t *HIDSHIDInformation);

/**
 * @brief
 * Parses a value received from
 * a remote HIDS server interpreting it as a Report Reference value.
 *
 * @details
 * This function should be used to decode the Report Reference received in
 * a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HIDS server.
 *
 * @param[in]  Value          Value received from the
 *                            HIDS server.
 *
 * @param[out]  ReportReferenceData   Pointer that will hold the
 *                                    Report Reference data if this
 *                                    function returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Decode_Report_Reference(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HIDS_Report_Reference_Data_t *ReportReferenceData);

/**
 * @brief
 * Parses a value received from
 * a remote HIDS server interpreting it as a External Report Reference
 * value.
 *
 * @details
 * This function should be used to decode the Report Reference received in
 * a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HIDS server.
 *
 * @param[in]  Value          Value received from the
 *                            HIDS server.
 *
 * @param[out]  ExternalReportReferenceUUID   A pointer that will hold
 *                                            the External Report data if
 *                                            this function returns
 *                                            success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Decode_External_Report_Reference(uint32_t ValueLength, uint8_t *Value, qapi_BLE_GATT_UUID_t *ExternalReportReferenceUUID);

/**
 * @brief
 * Formats an HIDS Protocol
 * Mode into a user-specified buffer.
 *
 * @details
 * The BufferLength and Buffer parameter must point to a buffer of at
 * least QAPI_BLE_HIDS_PROTOCOL_MODE_VALUE_LENGTH in size.
 *
 * @param[in]  ProtocolMode    Enumeration that will format the
 *                             Protocol into a user-specified buffer.
 *
 * @param[in]  BufferLength    Length of the buffer that will receive
 *                             the formatted data if this function is
 *                             successful.
 *
 * @param[out]  Buffer         The buffer that will receive the
 *                             formatted data if this function is
 *                             succesful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Format_Protocol_Mode(qapi_BLE_HIDS_Protocol_Mode_t ProtocolMode, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Formats an HIDS Control
 * Point Command into a user-specified buffer.
 *
 * @details
 * The BufferLength and Buffer parameter must point to a buffer of at
 * least QAPI_BLE_HIDS_CONTROL_POINT_VALUE_LENGTH in size.
 *
 * @param[in]  Command         Enumeration that will format the
 *                             Control Point Command into a
 *                             user-specified buffer.
 *
 * @param[in]  BufferLength    Length of the buffer that will receive
 *                             the formatted data if this function is
 *                             successful.
 *
 * @param[out]  Buffer         Buffer that will receive the
 *                             formatted data if this function is
 *                             succesful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HIDS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HIDS_Format_Control_Point_Command(qapi_BLE_HIDS_Control_Point_Command_t Command, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @}
 */

#endif

