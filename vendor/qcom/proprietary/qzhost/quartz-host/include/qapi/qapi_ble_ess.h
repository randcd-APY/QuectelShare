/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_ess.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Environmental Sensing Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Environmental Sensing Service (ESS) programming interface defines
 * the protocols and procedures to be used to implement the Environmental
 * Sensing Service capabilities.
 */

#ifndef __QAPI_BLE_ESS_H__
#define __QAPI_BLE_ESS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_esstypes.h"  /* QAPI ESS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */

   /* The following error codes are common error codes.                 */
#define QAPI_BLE_ESS_ERROR_INVALID_PARAMETER                              (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_ESS_ERROR_INVALID_BLUETOOTH_STACK_ID                     (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_ESS_ERROR_INSUFFICIENT_RESOURCES                         (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_ESS_ERROR_SERVICE_ALREADY_REGISTERED                     (-1003)
/**< Service is already registered. */
#define QAPI_BLE_ESS_ERROR_INVALID_INSTANCE_ID                            (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_ESS_ERROR_MALFORMATTED_DATA                              (-1005)
/**< Malformatted data. */
#define QAPI_BLE_ESS_ERROR_INSUFFICIENT_BUFFER_SPACE                      (-1006)
/**< Insufficient buffer space. */
#define QAPI_BLE_ESS_ERROR_UNKNOWN_ERROR                                  (-1007)
/**< Unknown error. */

   /* The following error codes may be returned by                      */
   /* ESS_Initialize_Service() or ESS_Initialize_Service_Handle_Range() */
   /* if the qapi_BLE_ESS_Initialize_Data_t structure parameter and sub */
   /* structures are not correctly configured.                          */
#define QAPI_BLE_ESS_ERROR_INVALID_NUMBER_OF_CHARACTERISTIC_TYPE_ENTRIES  (-1008)
/**< Invalid number of ESS characteristic type entries. */
#define QAPI_BLE_ESS_ERROR_DUPLICATE_CHARACTERISTIC_TYPE                  (-1009)
/**< A duplicate ESS characteristic type has been found. */
#define QAPI_BLE_ESS_ERROR_INVALID_NUMBER_OF_TRIGGER_SETTING_INSTANCES    (-1010)
/**< Invalid number of Trigger Setting Characteristic instances. */
#define QAPI_BLE_ESS_ERROR_MISSING_NOTIFY_CHARACTERISTIC_PROPERTY_FLAG    (-1011)
/**< Missing notify Characteristic instance property flag. */
#define QAPI_BLE_ESS_ERROR_MISSING_ES_MEASUREMENT_DESCRIPTOR_FLAG         (-1012)
/**< Missing ES Measurement Descriptor flag. */
#define QAPI_BLE_ESS_ERROR_MISSING_ES_TRIGGER_SETTING_DESCRIPTOR_FLAG     (-1013)
/**< Missing ES Trigger Setting Descriptor flag. */
#define QAPI_BLE_ESS_ERROR_DESCRIPTOR_VALUE_CHANGED_NOT_REGISTERED        (-1014)
/**< Descriptor Value Changed Characteristic is not registered. */
#define QAPI_BLE_ESS_ERROR_NUMBER_OF_INSTANCES_CHECK_FAILURE              (-1015)
/**< Invalid number of Characteristic instances. */

   /* The following error codes may be returned by response, write, and */
   /* decode functions.                                                 */
   /* * NOTE * If QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_TYPE_   */
   /*          -AND_CONDITION is returned, this means that the condition*/
   /*          is between (QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E */
   /*          <= condition <=                                          */
   /*          QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E)*/
   /*          and an ESS characteristic Type has been specified that   */
   /*          cannot be used for the corresponding condition. See the  */
   /*          qapi_BLE_ESS_ES_Trigger_Setting_Data_t structure for more*/
   /*          information.                                             */
#define QAPI_BLE_ESS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND         (-1016)
/**< Attribute handle information was not found. */
#define QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_INSTANCE            (-1017)
/**< Invalid ES Trigger Setting instance. */
#define QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_CONDITION           (-1018)
/**< Invalid ES Trigger Setting condition. */
#define QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_TYPE_AND_CONDITION  (-1019)
/**< Invalid ES Trigger Setting type and condition. */
#define QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_UNIT                (-1020)
/**< Invalid ES Trigger Setting unit type. */
#define QAPI_BLE_ESS_ERROR_INVALID_CHARACTERISTIC_TYPE                    (-1021)
/**< Invalid Characteristic type. */
#define QAPI_BLE_ESS_ERROR_INVALID_UUID_TYPE                              (-1022)
/**< Invalid GATT UUID Type. */
#define QAPI_BLE_ESS_ERROR_MISSING_EXTENDED_PROPERTIES                    (-1023)
/**< Missing Exteneded Properties Descriptor. */

/**
 * Enumeration of the ESS characteristic types.
 */
typedef enum
{
   QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E,
   /**< Apparent Wind Direction. */
   QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E,
   /**< Apparent Wind Speed. */
   QAPI_BLE_ESS_ECT_DEW_POINT_E,
   /**< Dew Point. */
   QAPI_BLE_ESS_ECT_ELEVATION_E,
   /**< Elevation. */
   QAPI_BLE_ESS_ECT_GUST_FACTOR_E,
   /**< Gust Factor. */
   QAPI_BLE_ESS_ECT_HEAT_INDEX_E,
   /**< Heat Index. */
   QAPI_BLE_ESS_ECT_HUMIDITY_E,
   /**< Humidity. */
   QAPI_BLE_ESS_ECT_IRRADIANCE_E,
   /**< Irradiance. */
   QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E,
   /**< Pollen concentration. */
   QAPI_BLE_ESS_ECT_RAIN_FALL_E,
   /**< Rain fall. */
   QAPI_BLE_ESS_ECT_PRESSURE_E,
   /**< Pressure. */
   QAPI_BLE_ESS_ECT_TEMPERATURE_E,
   /**< Temperature. */
   QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E,
   /**< True Wind Direction. */
   QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E,
   /**< True Wind Speed. */
   QAPI_BLE_ESS_ECT_UV_INDEX_E,
   /**< UV Index. */
   QAPI_BLE_ESS_ECT_WIND_CHILL_E,
   /**< Wind Chill. */
   QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E,
   /**< Barometric Pressure Trend. */
   QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E,
   /**< Magnetic Declination. */
   QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E,
   /**< Magnetic Flux Density 2D. */
   QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E
   /**< Magnetic Flux Density 3D. */
} qapi_BLE_ESS_Characteristic_Type_t;

/**
 * Structure that represents the unsigned/signed 24bit integer.
 */
typedef struct qapi_BLE_ESS_Int_24_Data_s
{
   /**
    * Lower 16 bits.
    */
   uint16_t Lower;

   /**
    * Upper 8 bits.
    */
   uint8_t  Upper;
} qapi_BLE_ESS_Int_24_Data_t;

#define QAPI_BLE_ESS_INT_24_DATA_SIZE                                   (sizeof(qapi_BLE_ESS_Int_24_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Int_24_Data_t structure.
 */

/**
 * Enumeration of all the values for the ESS
 * Characteristic: Barometric Pressure Trend.
 */
typedef enum
{
   QAPI_BLE_ESS_BPT_UNKNOWN_E,
   /**< Unknown. */
   QAPI_BLE_ESS_BPT_CONTINUOUSLY_FALLING_E,
   /**< Continuously falling. */
   QAPI_BLE_ESS_BPT_CONTINUOUSLY_RISING_E,
   /**< Continuously rising. */
   QAPI_BLE_ESS_BPT_FALLING_THEN_STEADY_E,
   /**< Falling then steady. */
   QAPI_BLE_ESS_BPT_RISING_THEN_STEADY_E,
   /**< Rising then steady. */
   QAPI_BLE_ESS_BPT_FALLING_BEFORE_LESSER_RISE_E,
   /**< Falling before lesser rise. */
   QAPI_BLE_ESS_BPT_FALLING_BEFORE_GREATER_RISE_E,
   /**< Falling before greater rise. */
   QAPI_BLE_ESS_BPT_RISING_BEFORE_GREATER_FALL_E,
   /**< Rising before greater fall. */
   QAPI_BLE_ESS_BPT_RISING_BEFORE_LESSER_FALL_E,
   /**< Rising before lesser fall. */
   QAPI_BLE_ESS_BPT_STEADY_E
   /**< Steady. */
} qapi_BLE_ESS_Barometric_Pressure_Trend_t;

/**
 * Structure that represents the format for the Magnetic Flux Density 2D
 * data.
 */
typedef struct qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_s
{
   /**
    * X Axis.
    */
   int16_t X_Axis;

   /**
    * Y Axis.
    */
   int16_t Y_Axis;
} qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t;

#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_DATA_SIZE                 (sizeof(qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t structure.
 */

/**
 * Structure that represents the format for the Magnetic Flux Density 3D
 * data.
 */
typedef struct qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_s
{
   /**
    * X Axis.
    */
   int16_t  X_Axis;

   /**
    * Y Axis.
    */
   int16_t  Y_Axis;

   /**
    * Z Axis.
    */
   int16_t  Z_Axis;
} qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t;

#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_DATA_SIZE                 (sizeof(qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t structure.
 */

/**
 * Union that represents the format for all ESS characteristic data
 * types.
 */
typedef union qapi_BLE_ESS_Characteristic_Data_s
{
   /**
    * Apparent Wind Direction.
    */
   uint16_t                                     Apparent_Wind_Direction;

   /**
    * Apparent Wind Speed.
    */
   uint16_t                                     Apparent_Wind_Speed;

   /**
    * Dew Point.
    */
   int8_t                                       Dew_Point;

   /**
    * Elevation.
    */
   qapi_BLE_ESS_Int_24_Data_t                   Elevation;

   /**
    * Gust Factor.
    */
   uint8_t                                      Gust_Factor;

   /**
    * Heat Index.
    */
   int8_t                                       Heat_Index;

   /**
    * Humidity.
    */
   uint16_t                                     Humidity;

   /**
    * Irradiance.
    */
   uint16_t                                     Irradiance;

   /**
    * Pollen Concentration.
    */
   qapi_BLE_ESS_Int_24_Data_t                   Pollen_Concentration;

   /**
    * Rainfall.
    */
   uint16_t                                     Rainfall;

   /**
    * Pressure.
    */
   uint32_t                                     Pressure;

   /**
    * Temperature.
    */
   int16_t                                      Temperature;

   /**
    * True Wind Direction.
    */
   uint16_t                                     True_Wind_Direction;

   /**
    * True Wind Speed.
    */
   uint16_t                                     True_Wind_Speed;

   /**
    * UV Index.
    */
   uint8_t                                      UV_Index;

   /**
    * Wind Chill.
    */
   int8_t                                       Wind_Chill;

   /**
    * Barometric Pressure Trend.
    */
   qapi_BLE_ESS_Barometric_Pressure_Trend_t     Barometric_Pressure_Trend;

   /**
    * Magnetic Declination.
    */
   uint16_t                                     Magnetic_Declination;

   /**
    * Magnetic Flux Density 2D.
    */
   qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t Magnetic_Flux_Density_2D;

   /**
    * Magnetic Flux Density 3D.
    */
   qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t Magnetic_Flux_Density_3D;
} qapi_BLE_ESS_Characteristic_Data_t;

   /* The following defines the optional properties for each ESS        */
   /* Characteristic.                                                   */
   /* * NOTE * If this flag is not set then the                         */
   /*          QAPI_BLE_ESS_DESCRIPTOR_FLAGS_ES_TRIGGER_SETTING may not */
   /*          be set since the ES Trigger Setting and ES Configuration */
   /*          descriptors CANNOT be included if the ESS characteristic */
   /*          does not have the Notify property.                       */
#define QAPI_BLE_ESS_CHARACTERISTIC_PROPERTY_FLAGS_NOTIFY                        (0x01)
/**< ESS characteristic supports the notify property. */

   /* The following defines the optional descriptors that may be added  */
   /* for each ESS characteristic.                                      */
   /* * NOTE * The QAPI_BLE_ESS_CHARACTERISTIC_DESCRIPTOR_FLAGS_        */
   /*          -ES_MEASUREMENT is ONLY OPTIONAL if there is only one    */
   /*          instance of a given ESS characteristic Type. This flag is*/
   /*          MANDATORY if multiple instances of the same ESS          */
   /*          Characteristic Type are registered with the service. If  */
   /*          multiple instances are present, ESS will fail to register*/
   /*          if multiple instances do not have this flag.             */
   /* * NOTE * The QAPI_BLE_ESS_CHARACTERISTIC_DESCRIPTOR_FLAGS_        */
   /*          -ES_TRIGGER_SETTING SHALL only be set if the             */
   /*          QAPI_BLE_ESS_CHARACTERISTIC_PROPERTY_FLAGS_NOTIFY is set */
   /*          for the Characteristic_Property_Flags field of the       */
   /*          qapi_BLE_ESS_Characteristic_Instance_Entry_t structure.  */
   /* * NOTE * If there are multiple instances (up to 3 instances       */
   /*          maximum) of the ES Trigger Setting descriptor for an ESS */
   /*          Characteristic instance (Set by the                      */
   /*          Trigger_Setting_Instances field of the                   */
   /*          qapi_BLE_ESS_Characteristic_Instance_Entry_t structure), */
   /*          then the ES Configuration descriptor will automatically  */
   /*          be included.                                             */
#define QAPI_BLE_ESS_DESCRIPTOR_FLAGS_ES_MEASUREMENT                             (0x01)
/**< ESS characteristic supports the ES Measurement Descriptor. */
#define QAPI_BLE_ESS_DESCRIPTOR_FLAGS_ES_TRIGGER_SETTING                         (0x02)
/**< ESS characteristic supports the ES Trigger Setting Descriptor. */
#define QAPI_BLE_ESS_DESCRIPTOR_FLAGS_USER_DESCRIPTION                           (0x04)
/**< ESS characteristic supports the User Description Descriptor. */
#define QAPI_BLE_ESS_DESCRIPTOR_FLAGS_VALID_RANGE                                (0x08)
/**< ESS characteristic supports the Valid Range Descriptor. */

   /* The following defines the optional descriptor properties.         */
   /* * NOTE * If the QAPI_BLE_ESS_DESCRIPTOR_PROPERTY_FLAGS_WRITE_     */
   /*          -S_TRIGGER_SETTING property is set and an ESS            */
   /*          Characteristic instance has multiple ES Trigger Setting  */
   /*          descriptors, then the ES Configuration descriptor that is*/
   /*          automatically included for multiple ES Trigger Setting   */
   /*          descriptors will also have the write property.           */
#define QAPI_BLE_ESS_DESCRIPTOR_PROPERTY_FLAGS_WRITE_ES_TRIGGER_SETTING          (0x01)
/**<
 * ESS characteristic's ES Trigger Setting Descriptor supports the write
 * property.
 */
#define QAPI_BLE_ESS_DESCRIPTOR_PROPERTY_FLAGS_WRITE_USER_DESCRIPTION            (0x02)
/**<
 * ESS characteristic's User Description Descriptor supports the write
 * property.
 */

/**
 * Structure that represents the information needed to register an ESS
 * Characteristic instance.
 *
 * All instances share the same ESS characteristic Type, so it is
 * implicitly known by the #qapi_BLE_ESS_Characteristic_Entry_t parent
 * structure.
 *
 * Trigger_Setting_Instances is only valid if the
 * QAPI_BLE_ESS_CHARACTERISTIC_DESCRIPTOR_FLAGS_ES_TRIGGER_SETTING bit
 * of the Descriptor_Flags is valid.
 *
 * Each ESS characteristic instance may have up to 3 ES Trigger Setting
 * descriptor instances. The Trigger_Setting_Instances field must be
 * between (1-3).
 *
 * If multiple ES Trigger Setting descriptor instances are specified by
 * the Trigger_Setting_Instances field, the ES Configuration
 * descriptor will automatically be registered for this ESS
 * characteristic instance.
 */
typedef struct qapi_BLE_ESS_Characteristic_Instance_Entry_s
{
   /**
    * Bitmask that defines the optional properties that may be
    * included for an ESS characteristic instance.
    */
   uint8_t Characteristic_Property_Flags;

   /**
    * Bitmask that defines the optional Characteristic Descriptors
    * that may be included for an ESS characteristic instance.
    */
   uint8_t Descriptor_Flags;

   /**
    * Bitmask that defines the optional Characteristic Descriptors
    * properties that are included for the ESS characteristic instance.
    */
   uint8_t Descriptor_Property_Flags;

   /**
    * Defines the number of instances for the optional Trigger Setting
    * Characteristic Descriptor.
    */
   uint8_t Trigger_Setting_Instances;
} qapi_BLE_ESS_Characteristic_Instance_Entry_t;

#define QAPI_BLE_ESS_CHARACTERISTIC_INSTANCE_ENTRY_SIZE                 (sizeof(qapi_BLE_ESS_Characteristic_Instance_Entry_t))
/**<
 * Size of the #qapi_BLE_ESS_Characteristic_Instance_Entry_t structure.
 */

/**
 * Structure that represents the information needed to register an ESS
 * Characteristic and all of it's instances.
 *
 * The Type field identifies the type of the ESS characteristic (must be
 * UNIQUE, see the #qapi_BLE_ESS_Initialize_Data_t structure for more
 * information).
 *
 * The Number_Of_Instances field identifies how many ESS characteristic
 * instances need to be registered with the service for the ESS
 * Characteristic Type.
 */
typedef struct qapi_BLE_ESS_Characteristic_Entry_s
{
   /**
    * Identifies the ESS characteristic type for all ESS characteristic
    * instances.
    */
   qapi_BLE_ESS_Characteristic_Type_t            Type;

   /**
    * Number of instances to include for the ESS characteristic.
    */
   uint16_t                                      Number_Of_Instances;

   /**
    * Pointer to the information that is needed to configure each ESS
    * Characteristic instance.
    */
   qapi_BLE_ESS_Characteristic_Instance_Entry_t *Instances;
} qapi_BLE_ESS_Characteristic_Entry_t;

#define QAPI_BLE_ESS_CHARACTERISTIC_ENTRY_SIZE                          (sizeof(qapi_BLE_ESS_Characteristic_Entry_t))
/**<
 * Size of the #qapi_BLE_ESS_Characteristic_Entry_t structure.
 */

/**
 * Structure that is used by the ESS_Initialize_Service() and
 * ESS_Initialize_Service_Handle_Range() functions to allow the ESS
 * Server to specify the optional ESS characteristics, descriptors, and
 * properties to include when the service is registered.
 *
 * This structure will also include the optional Descriptor Value Changed
 * Characteristic (conditions apply, see below).
 *
 * Each qapi_BLE_ESS_Characteristic_Entry_t structure must have a unique
 * Type field for each structure in the Characteristics field. This is
 * required to enforce that all instances for an ESS characteristic Type
 * will be registered sequentially. If a duplicate Type field is found
 * the service will fail to register.
 *
 * The Number_Of_Entries field must be at least 1 and is the number of
 * #qapi_BLE_ESS_Characteristic_Entry_t structures that the Entries field
 * holds. This directly corresponds to the total number of unique ESS
 * characteristics Types (a maximum of 20).
 *
 * The Descriptor_Value_Changed field indicates whether the Descriptor
 * Value Changed Characteristic will be registered. A CCCD
 * CCCD will be automatically
 * registered with the service for this characteristic if TRUE.
 *
 * The Descriptor_Value_Changed must be TRUE if the User Description
 * descriptor has the write property for any ESS characteristic.
 *
 * If the Descriptor_Value_Changed is FALSE, it is the ESS server's
 * responsibility to follow the specification and not change the ES
 * Measurement, ES Trigger Setting, ES Configuration, or Characteristic
 * User Description descriptors if they are present, since they are
 * stored in the application. If these descriptors are present, they
 * should be initialized once, and before or immediately following a
 * call to either of the Initialize_Service_XXX() APIs.
 *
 * If the Descriptor_Value_Changed is FALSE, the
 * qapi_BLE_ESS_Notify_Characteristic(),
 * qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Response(),
 * and
 * qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Response()
 * functions will fail, since they are dependedent on the Descriptor Value
 * Changed Characteristic.
 */
typedef struct qapi_BLE_ESS_Initialize_Data_s
{
   /**
    * Identifies the number of unique ESS characteristic types.
    */
   uint8_t                              Number_Of_Entries;

   /**
    * Pointer to the information for each ESS characteristic type that
    * is needed to each ESS characteristic and all of its instances.
    */
   qapi_BLE_ESS_Characteristic_Entry_t *Entries;

   /**
    * Controls whether the Descriptor Value Changed Characteristic is
    * included.
    */
   boolean_t                            Descriptor_Value_Changed;
} qapi_BLE_ESS_Initialize_Data_t;

#define QAPI_BLE_ESS_INITIALIZE_DATA_SIZE                               (sizeof(qapi_BLE_ESS_Initialize_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Initialize_Data_t structure.
 */

/**
 * Structure that represents the ESS Environmental Sensing Measurement
 * data.
 *
 * Definitions for these fields may be found in qapi_ble_esstypes.h. See
 * the qapi_BLE_ESS_ES_Measurement_t structure.
 */
typedef struct qapi_BLE_ESS_ES_Measurement_Data_s
{
   /**
    * Bitmask that controls the optional features and indicates the
    * optional fields included for an Environmental Sensing Measurement.
    */
   uint16_t                   Flags;

   /**
    * Sampling Function.
    */
   uint8_t                    Sampling_Function;

   /**
    * Measurement Period.
    */
   qapi_BLE_ESS_Int_24_Data_t Measurement_Period;

   /**
    * Update Interval.
    */
   qapi_BLE_ESS_Int_24_Data_t Update_Interval;

   /**
    * Application.
    */
   uint8_t                    Application;

   /**
    * Measurement Uncertainty.
    */
   uint8_t                    Measurement_Uncertainty;
} qapi_BLE_ESS_ES_Measurement_Data_t;

#define QAPI_BLE_ESS_ES_MEASUREMENT_DATA_SIZE                           (sizeof(qapi_BLE_ESS_ES_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_ES_Measurement_Data_t structure.
 */

/**
 * Enumeration of the ESS Trigger Setting instance. An ESS
 * Characteristic may have up to 3 instances of this descriptor.
 */
typedef enum
{
   QAPI_BLE_ESS_TSI_TRIGGER_SETTING_0_E, /**< Instance one. */
   QAPI_BLE_ESS_TSI_TRIGGER_SETTING_1_E, /**< Instance two. */
   QAPI_BLE_ESS_TSI_TRIGGER_SETTING_2_E  /**< Instance three. */
} qapi_BLE_ESS_ES_Trigger_Setting_Instance_t;

/**
 * Enumeration of the values for the Condition field of the
 * #qapi_BLE_ESS_ES_Trigger_Setting_Data_t structure.
 *
 * ESS characteristics that hold multiple values (Barometric Pressure
 * Trend, Magnetic Flux 2D/3D) cannot use the conditions where conditions
 * are between (QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E <= condition
 * <= QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E)
 */
typedef enum
{
   QAPI_BLE_ESS_TSC_TRIGGER_INACTIVE_E                      = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_TRIGGER_INACTIVE,
   /**< Trigger is inactive. */
   QAPI_BLE_ESS_TSC_FIXED_TIME_INTERVAL_E                   = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_FIXED_TIME_INTERVAL,
   /**< Fixed time interval trigger. */
   QAPI_BLE_ESS_TSC_NO_LESS_THAN_SPECIFIED_TIME_E           = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_NO_LESS_THAN_SPECIFIED_TIME,
   /**< No less than specified time trigger. */
   QAPI_BLE_ESS_TSC_VALUE_CHANGED_E                         = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_VALUE_CHANGED,
   /**< Value changed trigger. */
   QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E             = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_LESS_THAN_SPECIFIED_VALUE,
   /**< Less than specified value trigger. */
   QAPI_BLE_ESS_TSC_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE_E    = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE,
   /**< Less than or equal to specified value trigger. */
   QAPI_BLE_ESS_TSC_GREATER_THAN_SPECIFIED_VALUE_E          = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_GREATER_THAN_SPECIFIED_VALUE,
   /**< Greater than specified value trigger. */
   QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE,
   /**< Greater than or equal to specified value trigger. */
   QAPI_BLE_ESS_TSC_EQUALS_SPECIFIED_VALUE_E                = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_EQUAL_TO_SPECIFIED_VALUE,
   /**< Equals specified value trigger. */
   QAPI_BLE_ESS_TSC_NOT_EQUAL_SPECIFIED_VALUE_E             = QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_NOT_EQUAL_TO_SPECIFIED_VALUE
   /**< Not equal to specified value trigger. */
} qapi_BLE_ESS_ES_Trigger_Setting_Condition_t;

/**
 * Structure that represents the ESS Environmental Sensing Trigger
 * Setting data.
 *
 * The Condition field identifies the condition that will trigger a
 * notification. Multiple instances of this descriptor will depend on
 * the ES Configuration descriptor to determine if multiple trigger
 * conditions should be ANDed/ORed together to trigger a notification for
 * the ESS characteristic.
 *
 * The QAPI_BLE_ESS_TSC_TRIGGER_INACTIVE_E and
 * QAPI_BLE_ESS_TSC_VALUE_CHANGED_E conditions have no Operand.
 *
 * The QAPI_BLE_ESS_TSC_FIXED_TIME_INTERVAL_E and
 * QAPI_BLE_ESS_TSC_NO_LESS_THAN_SPECIFIED_TIME_E conditions will use
 * the Seconds Operand.
 *
 * ESS Barometric Pressure Trend, ESS Magnetic Flux 2D, and ESS Magnetic
 * Flux 3D, and future ESS characteristics that have multiple fields
 * cannot use the specified value conditions where conditions are between
 * QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E <= condition <=
 * QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E). If the
 * ESS characteristic Type and Condition fields are set so that this
 * requirement does not hold, APIs that use this structure will
 * return
 * QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_TYPE_AND_CONDITION.
 *
 * All other Operands depend on the ESS characteristic type for the ESS
 * characteristic instance that includes the ES Trigger Setting
 * Descriptor instance.
 */
typedef struct qapi_BLE_ESS_ES_Trigger_Setting_Data_s
{
   /**
    * Controls the condition that will trigger notifications.
    */
   qapi_BLE_ESS_ES_Trigger_Setting_Condition_t Condition;
   union
   {
      /**
       * seconds.
       */
      qapi_BLE_ESS_Int_24_Data_t Seconds;

      /**
       * Apparent Wind Direction.
       */
      uint16_t                   Apparent_Wind_Direction;

      /**
       * Apparent Wind Speed.
       */
      uint16_t                   Apparent_Wind_Speed;

      /**
       * Dew Point.
       */
      int8_t                     Dew_Point;

      /**
       * Elevation.
       */
      qapi_BLE_ESS_Int_24_Data_t Elevation;

      /**
       * Gust Factor.
       */
      uint8_t                    Gust_Factor;

      /**
       * Heat Index.
       */
      int8_t                     Heat_Index;

      /**
       * Humidity.
       */
      uint16_t                   Humidity;

      /**
       * Irradiance.
       */
      uint16_t                   Irradiance;

      /**
       * Pollen Concentration.
       */
      qapi_BLE_ESS_Int_24_Data_t Pollen_Concentration;

      /**
       * Rainfall.
       */
      uint16_t                   Rainfall;

      /**
       * Pressure.
       */
      uint32_t                   Pressure;

      /**
       * Temperature.
       */
      int16_t                    Temperature;

      /**
       * True Wind Direction.
       */
      uint16_t                   True_Wind_Direction;

      /**
       * True Wind Speed.
       */
      uint16_t                   True_Wind_Speed;

      /**
       * UV Index.
       */
      uint8_t                    UV_Index;

      /**
       * Wind Chill.
       */
      int8_t                     Wind_Chill;

      /**
       * Magnetic Declination.
       */
      uint16_t                   Magnetic_Declination;
   }
   /**
    * Optional operand that may be included for certain conditions.
    */
   Operand;
} qapi_BLE_ESS_ES_Trigger_Setting_Data_t;

#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_DATA_SIZE                       (sizeof(qapi_BLE_ESS_ES_Trigger_Setting_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_ES_Trigger_Setting_Data_t structure.
 */

/**
 * Structure that represents the the ESS Valid Range data.
 *
 * The Lower and Upper fields of this structure must access the same
 * union element.
 */
typedef struct qapi_BLE_ESS_Valid_Range_Data_s
{
   /**
    * The lower ESS characteristic value.
    */
   qapi_BLE_ESS_Characteristic_Data_t Lower;

   /**
    * The upper ESS characteristic value.
    */
   qapi_BLE_ESS_Characteristic_Data_t Upper;
} qapi_BLE_ESS_Valid_Range_Data_t;

#define QAPI_BLE_ESS_VALID_RANGE_DATA_SIZE                              (sizeof(qapi_BLE_ESS_Valid_Range_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Valid_Range_Data_t structure.
 */

/**
 * Structure that represents the ESS Descriptor Value Changed
 * Characteristic.
 *
 * The Flags field is a bitmask and takes bit values of the form
 * QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_XXX, where XXX indicates
 * the Flag. These can be found in qapi_ble_esstypes.h.
 *
 * Only the 16 and 128 UUID Types may be used for the UUID field.
 */
typedef struct qapi_BLE_ESS_Descriptor_Value_Changed_Data_s
{
   /**
    * Bitmask that identifies the source of the change as well as the
    * Characteristic Dscriptor.
    */
   uint16_t             Flags;

   /**
    * UUID of the ESS characteristic instance whose descriptors
    * changed.
    */
   qapi_BLE_GATT_UUID_t UUID;
} qapi_BLE_ESS_Descriptor_Value_Changed_Data_t;

#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_DATA_SIZE                 (sizeof(qapi_BLE_ESS_Descriptor_Value_Changed_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Descriptor_Value_Changed_Data_t structure.
 */

/**
 * Structure that represents the Characteristic information field that
 * will be included in events below to identify the type of ESS
 * characteristic as well as the instance. This allows the ESS server to
 * store/retrieve the corresponding characteristic or descriptor value
 * for the specified ESS characteristic instance.
 *
 * This structure will only be used for APIs and events that are for ESS
 * characteristic instances (not the Descriptor Value Changed
 * characteristic).
 *
 * Only the 16 and 128 UUID Types may be used for the UUID field.
 */
typedef struct qapi_BLE_ESS_Characteristic_Info_s
{
   /**
    * Identifies the ESS characteristic Type.
    */
   qapi_BLE_ESS_Characteristic_Type_t Type;

   /**
    * Identifies the ESS characteristic instance. Instances start at zero
    * and increment for each instance.
    */
   uint32_t                           ID;
} qapi_BLE_ESS_Characteristic_Info_t;

#define QAPI_BLE_ESS_CHARACTERISTIC_INFO_SIZE                           (sizeof(qapi_BLE_ESS_Characteristic_Info_t))
/**<
 * Size of the #qapi_BLE_ESS_Characteristic_Info_t structure.
 */

/**
 * Enumeration of all the events generated by the ESS
 * service. These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_ESS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ESS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E,
   /**< Read Characteristic request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_EXTENDED_PROPERTIES_REQUEST_E,
   /**< Read Exnteded Properties request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_CCCD_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ESS_ET_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_ES_MEASUREMENT_REQUEST_E,
   /**< Read ES Measurement request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_ES_TRIGGER_SETTING_REQUEST_E,
   /**< Read ES Trigger Setting request event. */
   QAPI_BLE_ESS_ET_SERVER_WRITE_ES_TRIGGER_SETTING_REQUEST_E,
   /**< Write ES Trigger Setting request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_ES_CONFIGURATION_REQUEST_E,
   /**< Read ES Configuration request event. */
   QAPI_BLE_ESS_ET_SERVER_WRITE_ES_CONFIGURATION_REQUEST_E,
   /**< Write ES Configuration request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_USER_DESCRIPTION_REQUEST_E,
   /**< Read User Description request event. */
   QAPI_BLE_ESS_ET_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E,
   /**< Write User Description request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_VALID_RANGE_REQUEST_E,
   /**< Read Valid Range request event. */
   QAPI_BLE_ESS_ET_SERVER_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E,
   /**< Read Descriptor Value Changed CCCD request event. */
   QAPI_BLE_ESS_ET_SERVER_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E,
   /**< Write Descriptor Value Changed CCCD request event. */
   QAPI_BLE_ESS_ET_SERVER_CONFIRMATION_E
   /**< Confirmation event. */
} qapi_BLE_ESS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_Characteristic_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * Characteristic value that has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_ESS_Read_Characteristic_Request_Data_t;

#define QAPI_BLE_ESS_READ_CHARACTERISTIC_REQUEST_DATA_SIZE              (sizeof(qapi_BLE_ESS_Read_Characteristic_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_Characteristic_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_CCCD_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * Characteristic that includes the CCCD that has been requested to be
    * read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_ESS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_ESS_READ_CCCD_REQUEST_DATA_SIZE                        (sizeof(qapi_BLE_ESS_Read_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to write an ESS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Write_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Write_CCCD_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * Characteristic that includes the CCCD that has been requested to be
    * written.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_ESS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_ESS_WRITE_CCCD_REQUEST_DATA_SIZE                       (sizeof(qapi_BLE_ESS_Write_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Write_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's ES Measurement Descriptor.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_ES_Measurement_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_ES_Measurement_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the ES Measurement Descriptor that
    * has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t;

#define QAPI_BLE_ESS_READ_ES_MEASUREMENT_REQUEST_DATA_SIZE              (sizeof(qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's ES Trigger Setting Descriptor instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                                   InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                                   ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t            ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                   TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                         RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the ES Trigger Setting Descriptor that
    * has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t         CharacteristicInfo;

   /**
    * Identifies the ES Trigger Setting Descriptor instance that has been
    * requested to be read.
    */
   qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance;
} qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_ESS_READ_ES_TRIGGER_SETTING_REQUEST_DATA_SIZE          (sizeof(qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to write an ESS
 * characteristic's ES Trigger Setting Descriptor instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                                   InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                                   ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t            ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                   TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                         RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the ES Trigger Setting Descriptor that
    * has been requested to be written.
    */
   qapi_BLE_ESS_Characteristic_Info_t         CharacteristicInfo;

   /**
    * Identifies the ES Trigger Setting Descriptor instance that has been
    * requested to be written.
    */
   qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance;

   /**
    * ES Trigger Setting Descriptor data that has been
    * requested to be written.
    */
   qapi_BLE_ESS_ES_Trigger_Setting_Data_t     TriggerSetting;
} qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_ESS_WRITE_ES_TRIGGER_SETTING_REQUEST_DATA_SIZE         (sizeof(qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's ES Configuration Descriptor.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_ES_Configuration_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_ES_Configuration_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the ES Configuration Descriptor that
    * has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t;

#define QAPI_BLE_ESS_READ_ES_CONFIGURATION_REQUEST_DATA_SIZE            (sizeof(qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to write an ESS
 * characteristic's ES Configuration Descriptor.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Write_ES_Configuration_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Write_ES_Configuration_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the ES Configuration Descriptor that
    * has been requested to be written.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;

   /**
    * ES Configuration Descriptor value that has been requested to
    * be written.
    */
   uint8_t                            Configuration;
} qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t;

#define QAPI_BLE_ESS_WRITE_ES_CONFIGURATION_REQUEST_DATA_SIZE           (sizeof(qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's User Description Descriptor.
 *
 * The application should set the starting offset to read for
 * the User Description Descriptor to the value of the
 * UserDescriptionOffset field. This will be nonzero if the entire User
 * Description Descriptor could not be sent in the previous call to
 * qapi_BLE_ESS_Read_User_Description_Request_Response().
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_User_Description_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_User_Description_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the User Description Descriptor that
    * has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Identifies the starting offset for the User Description Descriptor
    * that has been requested to be read.
    */
   uint16_t                           UserDescriptionOffset;
} qapi_BLE_ESS_Read_User_Description_Request_Data_t;

#define QAPI_BLE_ESS_READ_USER_DESCRIPTION_REQUEST_DATA_SIZE            (sizeof(qapi_BLE_ESS_Read_User_Description_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_User_Description_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to write an ESS
 * characteristic's User Description Descriptor.
 *
 * It is the application's responsiblity to ensure the User Description
 * Descriptor is NULL terminated if it is going to be displayed as a
 * character string.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Write_User_Description_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Write_User_Description_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the User Description Descriptor that
    * has been requested to be written.
    */
   qapi_BLE_ESS_Characteristic_Info_t  CharacteristicInfo;

   /**
    * Length of the User Description Descriptor.
    */
   uint16_t                            UserDescriptionLength;

   /**
    * Pointer to the User Description Descriptor.
    */
   uint8_t                            *UserDescription;
} qapi_BLE_ESS_Write_User_Description_Request_Data_t;

#define QAPI_BLE_ESS_WRITE_USER_DESCRIPTION_REQUEST_DATA_SIZE           (sizeof(qapi_BLE_ESS_Write_User_Description_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Write_User_Description_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read an ESS
 * characteristic's Valid Range Descriptor.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_Valid_Range_Request_Response() function.
 */
typedef struct qapi_BLE_ESS_Read_Valid_Range_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the ESS characteristic type and instance for the ESS
    * characteristic that includes the Valid Range Descriptor that
    * has been requested to be read.
    */
   qapi_BLE_ESS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_ESS_Read_Valid_Range_Request_Data_t;

#define QAPI_BLE_ESS_READ_VALID_RANGE_REQUEST_DATA_SIZE                 (sizeof(qapi_BLE_ESS_Read_Valid_Range_Request_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Read_Valid_Range_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to read the ESS
 * Descriptor Value Changed CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Response()
 * function.
 */
typedef struct qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t;

#define QAPI_BLE_ESS_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_SIZE   (sizeof(qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t))
/**<
 * Size of the
 * #qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ESS server when an ESS client has sent a request to write the
 * ESS Descriptor Value Changed CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Response()
 * function.
 */
typedef struct qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                        ClientConfiguration;
} qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t;

#define QAPI_BLE_ESS_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t))
/**<
 * Size of the
 * #qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to the ESS server when an ESS client has responded to an outstanding
 * indication that was previously sent by the ESS server.
 */
typedef struct qapi_BLE_ESS_Confirmation_Data_s
{
   /**
    * ESS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the ESS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ESS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the ESS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the outstanding indication. Values will be of the
    * form QAPI_BLE_GATT_CONFIRMATION_STATUS_XXX and can be found in
    * qapi_ble_gatt.h.
    */
   uint8_t                         Status;
} qapi_BLE_ESS_Confirmation_Data_t;

#define QAPI_BLE_ESS_CONFIRMATION_DATA_SIZE                             (sizeof(qapi_BLE_ESS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * ESS server event data for an ESS instance.
 */
typedef struct qapi_BLE_ESS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_ESS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint8_t                   Event_Data_Size;

   union
   {
      /**
       * ESS Read ESS characteristic instance event data.
       */
      qapi_BLE_ESS_Read_Characteristic_Request_Data_t                 *ESS_Read_Characteristic_Request_Data;

      /**
       * ESS Read Client Characteristic Configuration Descriptor event
       * data.
       */
      qapi_BLE_ESS_Read_CCCD_Request_Data_t                           *ESS_Read_CCCD_Request_Data;

      /**
       * ESS Write Client Characteristic Configuration Descriptor event
       * data.
       */
      qapi_BLE_ESS_Write_CCCD_Request_Data_t                          *ESS_Write_CCCD_Request_Data;

      /**
       * ESS Read ES Measurement Descriptor event data.
       */
      qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t                 *ESS_Read_ES_Measurement_Request_Data;

      /**
       * ESS Read ES Trigger Setting Descriptor event data.
       */
      qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t             *ESS_Read_ES_Trigger_Setting_Request_Data;

      /**
       * ESS Write ES Trigger Setting Descriptor event data.
       */
      qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t            *ESS_Write_ES_Trigger_Setting_Request_Data;

      /**
       * ESS Read ES Configuration Descriptor event data.
       */
      qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t               *ESS_Read_ES_Configuration_Request_Data;

      /**
       * ESS Write ES Configuration Descriptor event data.
       */
      qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t              *ESS_Write_ES_Configuration_Request_Data;

      /**
       * ESS Read User Description Descriptor event data.
       */
      qapi_BLE_ESS_Read_User_Description_Request_Data_t               *ESS_Read_User_Description_Request_Data;

      /**
       * ESS Write User Description Descriptor event data.
       */
      qapi_BLE_ESS_Write_User_Description_Request_Data_t              *ESS_Write_User_Description_Request_Data;

      /**
       * ESS Read Valid Range Descriptor event data.
       */
      qapi_BLE_ESS_Read_Valid_Range_Request_Data_t                    *ESS_Read_Valid_Range_Request_Data;

      /**
       * ESS Read Descriptor Value Changed CCCD event data.
       */
      qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t  *ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data;

      /**
       * ESS Write Descriptor Value Changed CCCD event data.
       */
      qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data;

      /**
       * ESS Confirmation event data.
       */
      qapi_BLE_ESS_Confirmation_Data_t                                *ESS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_ESS_Event_Data_t;

#define QAPI_BLE_ESS_EVENT_DATA_SIZE                             (sizeof(qapi_BLE_ESS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_ESS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for an
 * ESS instance event callback. This function will be called
 * whenever an ESS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller may use the contents of the ESS instance event data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It needs to be noted, however,
 * that if the same event callback is installed more than once, the
 * event callbacks will be called serially. Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another ESS instance event will not be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for ESS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock will occur because other event callbacks will not be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  ESS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User defined value that was supplied
 *                                   as an input parameter when the ESS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_ESS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_ESS_Event_Data_t *ESS_Event_Data, uint32_t CallbackParameter);

   /* ESS server API's.                                                 */

/**
 * @brief
 * Initializes an ESS instance (ESS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only one ESS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the #qapi_BLE_ESS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, an ESS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  ServiceFlags        Flags that are used to
 *                                 register ESS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the ESS server.
 *
 * @param[in]  EventCallback       ESS event callback that will
 *                                 receive ESS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered ESS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 function.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the ESS instance ID of the ESS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the ESS
 *              instance ID.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Initialize_Service(uint32_t BluetoothStackID, uint32_t ServiceFlags, qapi_BLE_ESS_Initialize_Data_t *InitializeData, qapi_BLE_ESS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an ESS instance (ESS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_ESS_Initialize_Service(), this function allows the
 * application to select an attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only one ESS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the #qapi_BLE_ESS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, an ESS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, all fields of the ServiceHandleRange parameter must
 * be initialized to zero. The qapi_BLE_ESS_Query_Number_Attributes()
 * function may be used after initializing an ESS instance to determine
 * the attribute handle range for the ESS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  ServiceFlags        Flags that are used to
 *                                 register ESS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the ESS server.
 *
 * @param[in]  EventCallback       ESS event callback that will
 *                                 receive ESS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT Service ID of the
 *                                 registered ESS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @param[in,out]  ServiceHandleRange   Pointer that, on input, holds
 *                                      the handle range to store the
 *                                      service in GATT, and on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the ESS instance ID of the ESS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the ESS
 *              instance ID.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t ServiceFlags, qapi_BLE_ESS_Initialize_Data_t *InitializeData, qapi_BLE_ESS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an ESS Instance (ESS server).
 *
 * @details
 * After this function is called, no other ESS
 * function can be called until after a successful call to either of the
 * qapi_BLE_ESS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that will
 *                                 be cleaned up.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in an ESS instance that is registered with a call
 * to either of the qapi_BLE_ESS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance.
 *
 * @return   Positive, nonzero, number of attributes for the
 *           registered ESS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Responds to a read request,
 * received from an ESS client for an ESS characteristic instance's
 * value.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the CharacteristicData parameter may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo  Identifies the ESS characteristic
 *                                 type and instance.
 *
 * @param[in]  CharacteristicData  ESS characteristic value that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's
 * CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  Configuration        The CCCD that
 *                                  will be sent if the request is
 *                                  accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint16_t Configuration);

/**
 * @brief
 * Responds to a write request
 * received from an ESS client for an ESS characteristic instance's
 * CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Allows the ESS server to query the
 * Extended Properties Descriptor for an ESS characteristic instance.
 *
 * @details
 * If the error code QAPI_BLE_ESS_ERROR_MISSING_EXTENDED_PROPERTIES is
 * returned, the Extended Properties descriptor does not exist for
 * the specified ESS characteristic instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[out]  ExtendedProperties   Pointer that will hold the
 *                                   Extended Properties Descriptor if
 *                                   this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 *
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Query_Extended_Properties(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint16_t *ExtendedProperties);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's
 * ES Measurement Descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the MeasurementData parameter may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  MeasurementData      Pointer to the ES Measurement
 *                                  Descriptor data that will be sent if
 *                                  the request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_ES_Measurement_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's
 * ES Trigger Setting Descriptor instance.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the TriggerSetting parameter may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  Instance             Identifies the ES Trigger Setting
 *                                  instance that has been requested.
 *
 * @param[in]  TriggerSetting       Pointer to the ES Trigger Setting
 *                                  Descriptor data that will be sent if
 *                                  the request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting);

/**
 * @brief
 * Responds to a write request
 * received from an ESS client for an ESS characteristic instance's
 * ES Trigger Setting Descriptor instance.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  Instance             Identifies the ES Trigger Setting
 *                                  instance that has been requested.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's ES
 * Configuration Descriptor instance.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  Configuration        ES Configuration Descriptor that
 *                                  will be sent if the request is
 *                                  accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_ES_Configuration_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint8_t Configuration);

/**
 * @brief
 * Responds to a write request
 * received from an ESS client for an ESS characteristic instance's ES
 * Configuration Descriptor instance.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Write_ES_Configuration_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's User
 * Description Descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the UserDescription parameter may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo  Identifies the ESS characteristic
 *                                 type and instance.
 *
 * @param[in]  UserDescription     Pointer to the User Description
 *                                 Descriptor that will be sent if the
 *                                 request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_User_Description_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, char *UserDescription);

/**
 * @brief
 * Responds to a write request
 * received from an ESS client for an ESS characteristic instance's User
 * Description Descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Write_User_Description_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for an ESS characteristic instance's
 * Valid Range Descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the ValidRange parameter may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  ValidRange           Pointer to the Valid Range
 *                                  Descriptor that will be sent if the
 *                                  request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_Valid_Range_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Valid_Range_Data_t *ValidRange);

/**
 * @brief
 * Responds to a read request
 * received from an ESS client for the ESS Descriptor Value Changed
 * Characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Configuration        CCCD that
 *                                  will be sent if the request is
 *                                  accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Configuration);

/**
 * @brief
 * Responds to a write request,
 * received from an ESS client for the ESS Descriptor Value Changed
 * Characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from qapi_ble_esstypes.h
 * (ESS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted/rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Notifies an ESS characteristic
 * instance value to an ESS client.
 *
 * @details
 * This function sends a notification, which provides no guarantee that
 * the ESS client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the ESS
 * Characteristic instance's CCCD has been previously configured for notifications. An
 * ESS client must have written the ESS characteristic instance's CCCD to
 * enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the ESS client.
 *
 * @param[in]  CharacteristicInfo   Identifies the ESS characteristic
 *                                  type and instance.
 *
 * @param[in]  CharacteristicData   Pointer to the ESS characteristic
 *                                  instance's data that will be sent in
 *                                  the notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Notify_Characteristic(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

/**
 * @brief
 * Indicates the Descriptor Value
 * Changed Characteristic to an ESS client.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * ESS client will receive the value, since the ESS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the ESS
 * Descriptor Value Changed Characteristic's CCCD has been previously configured for
 * indications. An ESS client must have written the ESS characteristic
 * instance's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the ESS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the ESS client.
 *
 * @param[in]  DescriptorValueChanged   Pointer to the ESS Descriptor
 *                                      Value Changed data
 *                                      that will be sent in the
 *                                      indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Indicate_Descriptor_Value_Changed(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *DescriptorValueChanged);

   /* ESS client API's.                                                 */

/**
 * @brief
 * Parses a value received in
 * a GATT read response or notification from a remote ESS server,
 * interpreting it as an ESS characteristic's value.
 *
 * @details
 * The Type parameter must be set and valid prior to calling this
 * function based on the attribute handle so the ESS characteristic can
 * be correctly decoded for the specified Type. The ESS client can
 * determine the correct Type to assign based on the ESS characteristic
 * UUID used to store the attribute handle during service discovery.
 * Therefore, the ESS client must store the ESS characteristic type for
 * each ESS characteristic instance along with the attribute handle
 * during service discovery.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           ESS server.
 *
 * @param[in]  Value         Value received from the
 *                           ESS server.
 *
 * @param[in]  Type          Identifies the ESS characteristic type.
 *
 * @param[out]  CharacteristicData   Pointer that will hold the ESS
 *                                   Characteristic instance data if this
 *                                   function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Decode_Characteristic(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

/**
 * @brief
 * Parses a value received in
 * a GATT read response from a remote ESS server interpreting
 * it as an ESS characteristic instance's ES Measurement Descriptor
 * value.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           ESS server.
 *
 * @param[in]  Value         Value received from the
 *                           ESS server.
 *
 * @param[out]  MeasurementData   Pointer that will hold the ESS
 *                                Characteristic instance data if this
 *                                function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Decode_ES_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_ES_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Formats the ES Trigger
 * Setting Descriptor into a user-specified buffer. This
 * function may also be used to determine the minimum length of the
 * user-specified buffer that is needed to hold the formatted data.
 *
 * @details
 * The Type parameter must be set and valid prior to calling this
 * function based on the attribute handle so the ESS characteristic can
 * be correctly decoded for the specified Type. The ESS client can
 * determine the correct Type to assign based on the ESS characteristic
 * UUID used to store the attribute handle during service discovery.
 * Therefore, the ESS client must store the ESS characteristic type for
 * each ESS characteristic instance along with the attribute handle
 * during service discovery.
 *
 * If the BufferLength parameter is set to zero, the
 * Buffer parameter may be excluded (NULL), and this
 * function will return the minimum length required for the
 * user-specified buffer. The caller may use this value to
 * allocate a buffer to hold the formatted data.
 *
 * @param[in]  Type                Identifies the ESS characteristic
 *                                 type.
 *
 * @param[in]  TriggerSetting      Pointer to the ESS trigger setting
 *                                 data.
 *
 * @param[in]  BufferLength        Length of the user-specified buffer.
 *
 * @param[in]  Buffer              User-specified buffer.
 *
 * @return      Zero if the ES Trigger Setting Descriptor
 *              is successfully formatted into the user-specified buffer.
 *
 * @return      Positive, nonzero value that represents the minimum length of the
 *              user-specified buffer to hold the formatted data.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Format_ES_Trigger_Setting_Request(qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received in
 * a GATT read response from a remote ESS server interpreting
 * it as an ESS characteristic instance's ES Trigger Setting Descriptor
 * instance's value.
 *
 * @details
 * The Type parameter must be set and valid prior to calling this
 * function based on the attribute handle so the ESS characteristic can
 * be correctly decoded for the specified Type. The ESS client can
 * determine the correct Type to assign based on the ESS characteristic
 * UUID used to store the attribute handle during service discovery.
 * Therefore, the ESS client must store the ESS characteristic type for
 * each ESS characteristic instance along with the attribute handle
 * during service discovery.
 *
 * ESS Barometric Pressure Trend, ESS Magnetic Flux 2D, and ESS Magnetic
 * Flux 3D, and future ESS characteristics that have multiple fields
 * cannot use the specified value conditions where conditions are between
 * QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E <= condition <=
 * QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E). If the
 * ESS characteristic Type and Condition fields are set so that this
 * requirement does not hold, APIs that use this structure will
 * return
 * QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_TYPE_AND_CONDITION.
 *
 * If the error code
 * QAPI_BLE_ESS_ERROR_INVALID_ES_TRIGGER_SETTING_CONDITION is returned,
 * this means that the ESS client has received a Reserved for Future
 * Use (RFU) condition. The ESS client needs to be tolerant of receiving
 * RFU conditions, however this function will still return an error code.
   The Operand field of the TriggerSetting parameter will
   not be valid.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           ESS server.
 *
 * @param[in]  Value         Value received from the
 *                           ESS server.
 *
 * @param[in]  Type          Identifies the ESS characteristic
 *                           type.
 *
 * @param[out]  TriggerSetting   Pointer that will hold the ESS
 *                               Trigger Setting Descriptor instance's
 *                               data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Decode_ES_Trigger_Setting(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting);

/**
 * @brief
 * Parses a value received in
 * a GATT read response from a remote ESS server, interpreting
 * it as an ESS characteristic instance's Valid Range Descriptor value.
 *
 * @details
 * The Type parameter must be set and valid prior to calling this
 * function based on the attribute handle so the ESS characteristic can
 * be correctly decoded for the specified Type. The ESS client can
 * determine the correct Type to assign based on the ESS characteristic
 * UUID used to store the attribute handle during service discovery.
 * Therefore, the ESS client must store the ESS characteristic type for
 * each ESS characteristic instance along with the attribute handle
 * during service discovery.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           ESS server.
 *
 * @param[in]  Value         Value received from the
 *                           ESS server.
 *
 * @param[in]  Type          Identifies the ESS characteristic
 *                           type.
 *
 * @param[out]  ValidRange   Pointer that will hold the ESS Valid Range
 *                           Descriptor if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Decode_Valid_Range(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Valid_Range_Data_t *ValidRange);

/**
 * @brief
 * Parses a value received in
 * a GATT indication from a remote ESS server interpreting
 * it as the ESS Descriptor Value Changed Characteristic value.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           ESS server.
 *
 * @param[in]  Value         Value received from the
 *                           ESS server.
 *
 * @param[out]  DescriptorValueChanged   Pointer that will hold the ESS
 *                                       Descriptor Value Changed
 *                                       Characteristic if this function
 *                                       is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. ESS error codes can be
 *              found in qapi_ble_ess.h (QAPI_BLE_ESS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ESS_Decode_Descriptor_Value_Changed(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *DescriptorValueChanged);

#endif

/**
 *  @}
 */
