/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_aios.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Automation IO Service (AIOS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Automation IO Service (AIOS) programming interface defines the protocols and
 * procedures to be used to implement the Automation IO Service
 * capabilities.
 */

#ifndef __QAPI_BLE_AIOS_H__
#define __QAPI_BLE_AIOS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_aiostypes.h" /* QAPI AIOS prototypes.                    */

/**
 * @addtogroup qapi_ble_services
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_AIOS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
/**< Invalid Bluetooth stack ID. */
#define QAPI_BLE_AIOS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_AIOS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
/**< Service is already registered. */
#define QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID                    (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_AIOS_ERROR_MALFORMATTED_DATA                      (-1005)
/**< Malformatted data. */
#define QAPI_BLE_AIOS_ERROR_INSUFFICIENT_BUFFER_SPACE              (-1006)
/**< Insufficient buffer space. */

   /* Error codes return from qapi_BLE_AIOS_Initialize_Service() and    */
   /* qapi_BLE_AIOS_Initialize_Service_Handle_Range() functions if the  */
   /* qapi_BLE_AIOS_Initialize_Data_t structure parameter is invalid.   */
   /* * NOTE * Other error codes may also be returned.                  */
#define QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_ENTRIES              (-1007)
/**< Invalid number of characteristic entries. */
#define QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_INSTANCES            (-1008)
/**< Invalid number of characteristic instances. */
#define QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_TYPE_ALREADY_REGISTERED (-1009)
/**< Characteristic type has already been used. */
#define QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_PROPERTY_FLAG   (-1010)
/**< Invalid characteristic instance property flag. */
#define QAPI_BLE_AIOS_ERROR_INVALID_DESCRIPTOR_FLAG                (-1011)
/**< Invalid characteristic instance descriptor property flag. */
#define QAPI_BLE_AIOS_ERROR_INVALID_AGGREGATE_PROPERTY_FLAG        (-1012)
/**< Invalid aggregate characteristic property flag. */

   /* Error codes returned from AIOS API's in addition to common error  */
   /* codes above.                                                      */
#define QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE            (-1013)
/**< Invalid characteristic type. */
#define QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE                        (-1014)
/**< Invalid characteristic input/output type. */
#define QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID     (-1015)
/**< Invalid characteristic instance ID. */
#define QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND (-1016)
/**< Attribute handle information could not be found. */
#define QAPI_BLE_AIOS_ERROR_INVALID_CONDITION_CODE_VALUE           (-1017)
/**< Invalid condition code value. */
#define QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_MTU_SIZE_EXCEEDED       (-1018)
/**< GATT MTU size has been exceeded. */
#define QAPI_BLE_AIOS_ERROR_AGGREGATE_NOT_SUPPORTED                (-1019)
/**< Aggregate characteristic not supported. */
#define QAPI_BLE_AIOS_ERROR_AGGREGATE_CCCD_NOT_SUPPORTED           (-1020)
/**<
 * Aggregate client characteristic configuration descriptor (CCCD) is not
 * supported.
 */

/**
 * Enumeration that represents the AIOS characteristic types.
 */
typedef enum
{
   QAPI_BLE_ACT_DIGITAL_E,   /**< Digital characteristic.*/
   QAPI_BLE_ACT_ANALOG_E,    /**< Analog characteristic.*/
   QAPI_BLE_ACT_AGGREGATE_E  /**< Aggregate characteristic.*/
} qapi_BLE_AIOS_Characteristic_Type_t;

/**
 * Enumeration of the I/O type for an AIOS
 * characteristic.
 *
 * Digital and analog characteristics may only use the
 * QAPI_BLE_IO_INPUT_E and QAPI_BLE_IO_OUTOUT_E enumerations, since they
 * cannot be both an input and an output.
 *
 * The QAPI_BLE_IO_INPUT_OUTPUT_E will be set automatically for the
 * aggregate characteristic if the aggregate characteristic is made up of
 * digital and analog characterics that have both the QAPI_BLE_IO_INPUT_E
 * and QAPI_BLE_IO_OUTOUT_E enumerations. If the digital and analog
 * characteristics that make up the aggregate characteristic have either
 * the QAPI_BLE_IO_INPUT_E or QAPI_BLE_IO_OUTOUT_E enumeration, that
 * enumeration will be set for the aggregate characteristic.
 */
typedef enum
{
   QAPI_BLE_IO_INPUT_E,        /**< Input characteristic instance. */
   QAPI_BLE_IO_OUTOUT_E,       /**< Output characteristic instance. */
   QAPI_BLE_IO_INPUT_OUTPUT_E  /**< Input/output characteristic instance. */
} qapi_BLE_AIOS_IO_Type_t;

   /* The following defines the optional AIOS characteristic properties */
   /* that may be set for Digital/Analog Input characteristic instances.*/
   /* These bit mask values may be set for the                          */
   /* characteristic_Property_Flags field of the                        */
   /* qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure if the    */
   /* IOType field is set to QAPI_BLE_IO_INPUT_E.                       */
   /* ** NOTE ** If the IOType field of the                             */
   /*            qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure*/
   /*            is set to QAPI_BLE_IO_INPUT_E and any other GATT       */
   /*            characteristic Properties are detected other than the  */
   /*            ones below, the qapi_BLE_AIOS_Initialize_XXX() API's   */
   /*            will return the                                        */
   /*            QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_PROPERTY_.. */
   /*            .._FLAG error.                                         */
   /* * NOTE * The Read property is Mandatory for Digital/Analog        */
   /*          characteristic Input instances and will automatically be */
   /*          set when the service is initialized.                     */
   /* * NOTE * The QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_.. */
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_..     */
   /*          .._NOTIFY flags cannot be specified simultaneously.      */
   /* * NOTE * The QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_.. */
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_..     */
   /*          .._NOTIFY flags cannot be specified if the Aggregate     */
   /*          characteristic is supported.                             */
   /* * NOTE * A CCCD  */
   /*          will automatically be included when the service is       */
   /*          initialized if either the                                */
   /*          QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_..     */
   /*          .._INDICATE or                                           */
   /*          QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_..     */
   /*          .._NOTIFY flag is specified.                             */

#define QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_INDICATE  (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_INDICATE)
/**< The AIOS input characteristic instance supports the indicate
 * property.
 */
#define QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_NOTIFY    (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY)
/**< The AIOS input characteristic instance supports the notify
 * property.
 */

   /* The following defines the optional AIOS characteristic properties */
   /* that may be set for Digital/Analog Output characteristic          */
   /* instances.  These bit mask values may be set for the              */
   /* Characteristic_Property_Flags field of the                        */
   /* qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure if the    */
   /* IOType field is set to QAPI_BLE_IO_OUTPUT_E.                      */
   /* ** NOTE ** If the IOType field of the                             */
   /*            qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure*/
   /*            is set to QAPI_BLE_IO_INPUT_E and any other GATT       */
   /*            characteristic Properties are detected other than the  */
   /*            ones below, the qapi_BLE_AIOS_Initialize_XXX() API's   */
   /*            will return the                                        */
   /*            QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_PROPERTY_.. */
   /*            .._FLAG error.                                         */
   /* * NOTE * The QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..*/
   /*          .._WRITE and                                             */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_WRITE_*/
   /*          ..._RESPONSE properties are MANDATORY for Digital/Analog */
   /*          Output characteristic instances and at least one must be */
   /*          specified.                                               */
   /* * NOTE * The QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..*/
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._NOTIFY flags cannot be specified if                   */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._READ is not specified.                                */
   /* * NOTE * The QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..*/
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._NOTIFY flags cannot be specified simultaneously.      */
   /* * NOTE * The QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..*/
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._NOTIFY flags cannot be specified if the Aggregate     */
   /*          characteristic is supported.                             */
   /* * NOTE * A CCCD  */
   /*          will automatically be included when the service is       */
   /*          initialized if either the                                */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._INDICATE or                                           */
   /*          QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_..    */
   /*          .._NOTIFY flag is specified.                             */

#define QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_WRITE                   (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE)
/**< The AIOS output characteristic instance supports the write
 * property.
 */
#define QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_WRITE_WITHOUT_RESPONSE  (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE)
/**< The AIOS output characteristic instance supports the write without
 * response property.
 */
#define QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_READ                    (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ)
/**< The AIOS output characteristic instance supports the read
 * property.
 */
#define QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_INDICATE                (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_INDICATE)
/**< The AIOS output characteristic instance supports the indicate
 * property.
 */
#define QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_PROPERTY_FLAGS_NOTIFY                  (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY)
/**< The AIOS output characteristic instance supports the notify
 * property.
 */

   /* The following defines the optional AIOS aggregate characteristic  */
   /* properties that may be set.  These bit mask values may be set for */
   /* the Aggregate_Property_Flags field of the                         */
   /* qapi_BLE_AIOS_Initialize_Data_t structure.                        */
   /* * NOTE * The Read property is Mandatory for the Aggregate         */
   /*          characteristic and will automatically be set.            */
   /* * NOTE * The QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_.. */
   /*          .._INDICATE and                                          */
   /*          QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_PROPERTY_FLAGS_..     */
   /*          .._NOTIFY cannot be set simultaneously.                  */
   /* * NOTE * A CCCD  */
   /*          will automatically be included when the service is       */
   /*          initialized if either the                                */
   /*          QAPI_BLE_AIOS_AGGREGATE_PROPERTY_FLAGS_INDICATE or       */
   /*          QAPI_BLE_AIOS_AGGREGATE_PROPERTY_FLAGS_NOTIFY flag is    */
   /*          specified.                                               */

#define QAPI_BLE_AIOS_AGGREGATE_PROPERTY_FLAGS_INDICATE        (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_INDICATE)
/**< The AIOS aggregate characteristic supports the indicate property. */
#define QAPI_BLE_AIOS_AGGREGATE_PROPERTY_FLAGS_NOTIFY          (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY)
/**< The AIOS aggregate characteristic supports the notify property. */

   /* The following defines the descriptor flags that may be used to    */
   /* include optional descriptors for AIOS Digital/Analog              */
   /* characteristic instances.  These bit mask values may be set for   */
   /* the Descriptor_Flags field of the                                 */
   /* qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure.          */
   /* ** NOTE ** The QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALUE_TRIGGER_SETTING*/
   /*            and QAPI_BLE_AIOS_DESCRIPTOR_FLAG_TIME_TRIGGER_SETTING */
   /*            may not be specfied if the Indicate or Notify Property */
   /*            is not supported for the AIOS Digital/Analog           */
   /*            characteristic instance.  However, if the Aggregate    */
   /*            characteristic is supported, and it has either the     */
   /*            Notify or Indicate property then the                   */
   /*            QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALUE_TRIGGER_SETTING and*/
   /*            QAPI_BLE_AIOS_DESCRIPTOR_FLAG_TIME_TRIGGER_SETTING may */
   /*            be specified.                                          */
   /* * NOTE * The QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_PRESENTATION_FORMAT   */
   /*          may only be specified if there is one instance specified */
   /*          by Number_Of_Instances field of the                      */
   /*          qapi_BLE_AIOS_Characteristic_Entry_t structure.  The     */
   /*          Presentation Format descriptor will automatically be     */
   /*          included when the service is initialized if there is more*/
   /*          than one instance.                                       */
   /* * NOTE * The QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALUE_TRIGGER_SETTING  */
   /*          and QAPI_BLE_AIOS_DESCRIPTOR_FLAG_TIME_TRIGGER_SETTING   */
   /*          may NOT be included if the Use_Custom_Trigger field of   */
   /*          the qapi_BLE_AIOS_Characteristic_Instance_Entry_t        */
   /*          structure is TRUE.                                       */
   /* * NOTE * The QAPI_BLE_AIOS_DESCRIPTOR_FLAG_TIME_TRIGGER_SETTING   */
   /*          may not be specified if the                              */
   /*          QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALUE_TRIGGER_SETTING is   */
   /*          not specified.  This is required since the Time Trigger  */
   /*          Setting descriptor may be set such that a notification or*/
   /*          indication may only be sent once the Value Trigger       */
   /*          Setting descriptor's trigger condition has been triggered*/
   /*          a specified number of times.                             */
   /* * NOTE * Digital characteristic instances cannot use the optional */
   /*          QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALID_RANGE flag since this*/
   /*          flag is only valid for analog characteristic instances.  */

#define QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_PRESENTATION_FORMAT                 (0x01)
/**< The AIOS characteristic instance supports the presentation format
 * descriptor.
 */
#define QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_USER_DESCRIPTION                    (0x02)
/**< The AIOS characteristic instance supports the user description
 * descriptor.
 */
#define QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_VALUE_TRIGGER_SETTING               (0x04)
/**< The AIOS characteristic instance supports the value trigger setting
 * descriptor.
 */
#define QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_TIME_TRIGGER_SETTING                (0x08)
/**< The AIOS characteristic instance supports the time trigger setting
 * descriptor.
 */
#define QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_VALID_RANGE                         (0x10)
/**< The AIOS characteristic instance supports the valid range
 * descriptor.
 */

   /* The following defines the descriptor property flags that may be   */
   /* set for descriptors that are included for a Digital/Analog        */
   /* characteristic instance.  These bit mask values may be set for the*/
   /* Descriptor_Property_Flags field of the                            */
   /* qapi_BLE_AIOS_characteristic_Instance_Entry_t structure.          */
   /* * NOTE * The QAPI_BLE_AIOS_DESCRIPTOR_FLAG_USER_DESCRIPTION flag  */
   /*          must be specfied for the Descriptor_Flags field of the   */
   /*          qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure  */
   /*          for the                                                  */
   /*          QAPI_BLE_AIOS_DESCRIPTOR_PROPERTY_FLAGS_USER_DESCRIPTION_*/
   /*          .._WRITABLE flag to be valid.                            */
   /* * NOTE * If the                                                   */
   /*          QAPI_BLE_AIOS_DESCRIPTOR_PROPERTY_FLAGS_USER_DESCRIPTION_*/
   /*          .._WRITABLE is specified, and the above note holds, then */
   /*          the Extended Properties descriptor will automatically be */
   /*          included in the service when it is initialized, and the  */
   /*          'Write Auxiliries' bit will automatically be set.        */

#define QAPI_BLE_AIOS_DESCRIPTOR_PROPERTY_FLAGS_USER_DESCRIPTION_WRITABLE  (0x01)
/**< The AIOS characteristic user description descriptor supports the
 * write property.
 */

/**
 * Structure that defines the information that is needed to
 * initialize an Automation IO Service (AIOS) digital/analog characteristic instance.
 */
typedef struct qapi_BLE_AIOS_Characteristic_Instance_Entry_s
{
   /**
    * Specifies whether the AIOS
    * digital/analog characteristic instance is an
    * input (ioInput) or output (ioOutput) signal. This is
    * important since the Characteristic_Property_Flags must
    * be set by values from
    * QAPI_BLE_AIOS_OUTPUT_CHARACTERISTIC_XXX or
    * QAPI_BLE_AIOS_INPUT_CHARACTERISTIC_XXX, depending on the
    * enumeration selected for the IO_Type field.
    *
    * The QAPI_BLE_IO_INPUT_OUTPUT_E enumeration is not valid
    * for the IOType field since it is only informative and
    * used with the aggregate characteristic.
    */
   qapi_BLE_AIOS_IO_Type_t IO_Type;

   /**
    * Specifies the property
    * flags for the characteristic instance. Valid values depend on the
    * IO_Type field.
    */
   uint8_t                 Characteristic_Property_Flags;

   /**
    * Specifies the characteristic
    * descriptors that may be included for the characteristic instance.
    * Valid values have the form:
    * QAPI_BLE_AIOS_DESCRIPTOR_PROPERTY_FLAG_XXX.
    */
   uint8_t                 Descriptor_Flags;

   /**
    * Specifies the characteristic
    * descriptor properties that may be included if a characteristic
    * descriptor is included for the characteristic instance based on
    * the Descriptor_Flags field. Valid values have the form:
    * QAPI_BLE_AIOS_DESCRIPTOR_PROPERTY_FLAG_XXX.
    */
   uint8_t                 Descriptor_Property_Flags;

   /**
    * Allows the application to have a
    * custom condition that triggers a notification or
    * indication of a digital/analog characteristic instance if
    * set to TRUE. This also means that the
    * QAPI_BLE_AIOS_DESCRIPTOR_FLAG_VALUE_TRIGGER_SETTING and
    * QAPI_BLE_AIOS_DESCRIPTOR_FLAG_TIME_TRIGGER_SETTING values
    * for the Descriptor_Flags field cannot be specified, since
    * they are replaced by the custom condition.
    */
   boolean_t               Use_Custom_Trigger;
} qapi_BLE_AIOS_Characteristic_Instance_Entry_t;

#define QAPI_BLE_AIOS_CHARACTERISTIC_INSTANCE_ENTRY_SIZE       (sizeof(qapi_BLE_AIOS_Characteristic_Instance_Entry_t))
/**< Size of the #qapi_BLE_AIOS_Characteristic_Instance_Entry_t
 * structure.
 */

/**
 * Structure that defines the information that is needed to
 * initialize an AIOS digital/analog characteristic and all of its
 * instances.
 */
typedef struct qapi_BLE_AIOS_Characteristic_Entry_s
{
   /**
    * Enumeration that is used to identify
    * whether the AIOS characteristic instances are
    * digital (QAPI_BLE_ACT_DIGITAL_E) or
    * analog (QAPI_BLE_ACT_ANALOG_E).
    * The QAPI_BLE_ACT_AGGREGATE_E enumeration may not be used
    * for this field, since the aggregate is initialized
    * separately from digital/analog characteristic instances.
    *
    * The Type field must be unique for each
    * #qapi_BLE_AIOS_Characteristic_Entry_t structure included
    * in the Entries field of the
    * #qapi_BLE_AIOS_Initialize_Data_t structure, since all
    * digital/analog characteristic instances must be
    * initialized together.
    *
    * If the Type field is QAPI_BLE_ACT_DIGITAL_E, the
    * mandatory number of digital descriptors will
    * automatically be included when the service is initialized
    * for all digital characteristic instances.
    */
   qapi_BLE_AIOS_Characteristic_Type_t            Type;

   /**
    * Specifies the number of characteristic instances that are
    * supported for the spcified characteristic type.
    *
    * The presentation format descriptor will automatically be
    * included for each characteristic instance if the
    * Number_Of_Instances field is greater than 1. Otherwise, it
    * will be excluded for only one instance unless the
    * QAPI_BLE_AIOS_DESCRIPTOR_FLAGS_PRESENTATION_FORMAT flag is
    * specified in the Descriptor_Flags field of the
    * #qapi_BLE_AIOS_Characteristic_Instance_Entry_t structure.
    */
   uint16_t                                       Number_Of_Instances;

   /**
    * Pointer to the information that is needed to initialize each
    * characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Instance_Entry_t *Instances;
} qapi_BLE_AIOS_Characteristic_Entry_t;

#define QAPI_BLE_AIOS_CHARACTERISTIC_ENTRY_SIZE                (sizeof(qapi_BLE_AIOS_Characteristic_Entry_t))
/**< Size of the #qapi_BLE_AIOS_Characteristic_Entry_t
 * structure.
 */

/**
 * Structure that defines the information needed to
 * initialize the AIOS server.
 */
typedef struct qapi_BLE_AIOS_Initialize_Data_s
{
   /**
    * Number of unique characteristics.
    *
    * This field cannot be greater than 2,
    * since this structure will only allow a
    * #qapi_BLE_AIOS_Characteristic_Entry_t structure for a
    * digital characteristic and all of its instances or an
    * analog characteristic and all of its instances.
    */
   uint8_t                               Number_Of_Entries;

   /**
    * Pointer to the information that is needed to initialize each
    * unique characteristic and all of its instances.
    */
   qapi_BLE_AIOS_Characteristic_Entry_t *Entries;

   /**
    * Flag that indicates whether the aggregate characteristic is
    * supported.
    */
   boolean_t                             Aggregate_Supported;

   /**
    * Bitmask that specifies the characteristic properties
    * for the aggregate characteristic. The
    * Aggregate_Supported must be TRUE or this field will be
    * ignored. Valid values have the form
    * QAPI_BLE_AIOS_AGGREGATE_PROPERTY_XXX.
    */
   uint8_t                               Aggregate_Property_Flags;
} qapi_BLE_AIOS_Initialize_Data_t;

#define QAPI_BLE_AIOS_INITIALIZE_DATA_SIZE                     (sizeof(qapi_BLE_AIOS_Initialize_Data_t))
/**< Size of the #qapi_BLE_AIOS_Initialize_Data_t structure. */

/**
 * Structure that represents the format for a digital characteristic.
 *
 * A digital characteristic is repsented by a buffer for all
 * the digital signals supported by the digital characteristic instance.
 * The Buffer field is needed since a digital characteristic is made up
 * of digital signals (2-bit values in little-endian order) and may span
 * over many bytes depending on the number of digital signals that is
 * supported by the digital characteristic instance.
 *
 * Each digital signal (2-bit value) has the following definition:
 *
 *  @li 0b00 -- Inactive state
 *  @li 0b01 -- Active state
 *  @li 0b10 -- Tri-state (if supported by the AIOS server)
 *  @li 0b11 -- Unknown state
 *
 * If the Unknown state is received in a write operation (outputs only),
 * the AIOS server must not update the corresponding output. This value
 * may be sent in a read, notify, or indicate operation to indicate that
 * the AIOS server cannot report the value for this particular
 * digital characteristic instance.
 *
 * Each byte in the buffer contains four digital signals (2-bit values).
 * This can be represented by the following, where AA is the first
 * digital signal or least signifigant digital signal (2-bit value):
 *
 *        MSb    ... DD_CC_BB_AA ...    LSb
 *
 * Example:
 *
 * If the digital characteristic instance's Number Of Digitals
 * descriptor has five digital signals, the Length field will be 2, since a
 * single byte holds four digital signals (four 2-bit values). There is
 * another byte that contains one digital signal (at the least
 * signifigant 2-bit value that can be seen above as AA), with the
 * remaining bits (BB, CC, and DD) padded with zeros. The Number Of
 * Digitals Descriptor indicates that the remaining signals are not used,
 * however they should be padded with zeros to indicate the inactive
 * state.
 *
 * The AIOS server should call the
 * qapi_BLE_GATT_Change_Maximum_Supported_MTU() function before
 * initializing the service via a call to
 * qapi_BLE_AIOS_Initialize_XXX() to set an MTU that is large enough to
 * support the digital characteristic instances and the aggregate
 * characteristic. The AIOS client must call the
 * qapi_BLE_GATT_Exchange_MTU_Request() function to negotiate a higher
 * MTU once a connection has been established. The smaller supported
 * MTU by the AIOS server and AIOS client will always be the negotiated MTU.
 */
typedef struct qapi_BLE_AIOS_Digital_Characteristic_Data_s
{
  /**
   * Length of the Buffer field. This does not indicate the number of
   * digital signals. The number of digital signals can only be
   * determined from the Number Of Digitals descriptor that is mandatory
   * for each digital characteristic instance. The Length field can
   * be determined by taking the Number of Digitals and dividing
   * by four (adding an extra byte for the remainder). 
   *
   * The value of any
   * digital signals (2-bit values) that have been padded to the extra
   * byte for a digital characteristic instance must be included, however
   * they have no meaning since they are beyond the number of digital
   * signals supported for a digital characteristic instance. Therefore,
   * the AIOS client cannot read/write a digital characteristic instance
   * without first reading its Number Of Digitals descriptor to
   * determine the number of digital signals and any padded bits that
   * have no meaning.
   *
   * The Length field cannot be greater than (ATT_MTU-3). This
   * restriction is imposed by the service specification and it is the
   * application's responsibility to ensure that the Length field meets
   * this requirement. Otherwise, any API that sends a digital
   * characteristic instance will fail, and the error
   * QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_MTU_SIZE_EXCEEDED will be
   * returned.
   */
   uint16_t  Length;

   /**
    * Pointer to the buffer that holds the digital signals.
    */
   uint8_t  *Buffer;
} qapi_BLE_AIOS_Digital_Characteristic_Data_t;

#define QAPI_BLE_AIOS_DIGITAL_CHARACTERISTIC_DATA_SIZE         (sizeof(qapi_BLE_AIOS_Digital_Characteristic_Data_t))
/**< Size of the #qapi_BLE_AIOS_Digital_Characteristic_Data_t
 * structure.
 */

/**
 * Structure that defines the format for the AIOS aggregate
 * characteristic. The aggregate characteristic will be
 * represented by a buffer that holds all digital characteristic
 * instances (digital signals) and all analog characteristic instances
 * (analog signals) that are included in the aggregate characteristic.
 *
 * All digital characteristic instances included in the aggregate
 * characteristic must be formatted into the Buffer field first,
 * followed by all analog characteristic instances included in the
 * aggregate characteristic.
 *
 * All analog characteristic instances must be formatted in
 * little-endian order.
 *
 * All requirements for digital characteristic instances found in the
 * #qapi_BLE_AIOS_Digital_Characteristic_Data_t structure above must be
 * followed. This includes how digital characteristic instances are
 * formatted into the Buffer field for the aggregate characteristic.
 * Each digital characteristic instance included in the aggregate
 * characteristic will have its length added to the Length field. This
 * length includes the size needed to hold a digital characteristic
 * instance's digital signals (2-bit values in little-endian order) and
 * any padded 2-bit values that may span over multiple octets. The
 * Length field of the qapi_BLE_AIOS_Digital_Characteristic_Data_t
 * structure directly corresponds to the length that must be added for
 * each digital characteristic instance included in the aggregate
 * characteristic.
 *
 * Since a digital characteristic instance has a variable length and the
 * aggregate characteristic is made up of digital characteristic
 * instances, as well as analog characteristic instances that have the
 * read property, the AIOS server and AIOS client should use the Number
 * Of Digitals descriptor to determine the length of each digital
 * characteristic instance. The AIOS client cannot decode an aggregate
 * characteristic that has been read without first reading each digital
 * characteristic instance's Number Of Digitals descriptor before
 * issuing a read request for the aggregate characteristic. This is so
 * that the AIOS client can determine the length of each digital
 * characteristic instance included in the aggregate characteristic so
 * that the AIOS client can properly separate each digital
 * characteristic instance included in the aggregate characteristic. The
 * AIOS client should already know if the aggregate characteristic is
 * supported by the AIOS server and if a digital characteristic is
 * included the aggregate characteristic, since service discovery must be
 * performed before reading a characteristic or a descriptor. The
 * digital characteristic instance's properties are included in service
 * discovery, and the AIOS client can quickly determine if it is in the
 * aggregate by checking the read property.
 *
 * The AIOS server should call the
 * qapi_BLE_GATT_Change_Maximum_Supported_MTU() function prior to
 * initializing the service via a call to
 * qapi_BLE_AIOS_Initialize_XXX(), to set an MTU that is large enough to
 * support the digital characteristic instances and the Aggregate
 * characteristic.  The AIOS client must call the
 * qapi_BLE_GATT_Exchange_MTU_Request() function to negotiate a higher
 * MTU once a connection has been established. The smaller supported
 * MTU by the AIOS server and AIOS client will always be the negotiated
 * MTU.
 */
typedef struct qapi_BLE_AIOS_Aggregate_Characteristic_Data_s
{
   /**
    * Length of the Buffer field.
    *
    * The Length field cannot be greater than (ATT_MTU-3). This
    * restriction is imposed by the service specification and it is the
    * application's responsibility to ensure that the Length field meets
    * this requirement. Otherwise, any API that sends the aggregate
    * characteristic will fail, and the error
    * QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_MTU_SIZE_EXCEEDED will be
    * returned.
    */
   uint16_t  Length;

   /**
    * Pointer to the buffer that holds the digital and analog signals
    * that are included in the aggregate characteristic.
    */
   uint8_t  *Buffer;
} qapi_BLE_AIOS_Aggregate_Characteristic_Data_t;

#define QAPI_BLE_AIOS_AGGREGATE_CHARACTERISTIC_DATA_SIZE       (sizeof(qapi_BLE_AIOS_Aggregate_Characteristic_Data_t))
/**< Size of the #qapi_BLE_AIOS_Aggregate_Characteristic_Data_t
 * structure.
 */

/**
 * Union that represents the format for an AIOS characteristic.
 */
typedef union qapi_BLE_AIOS_Characteristic_Data_s
{
   /**
    * Data for an AIOS digital characteristic instance.
    */
   qapi_BLE_AIOS_Digital_Characteristic_Data_t   Digital;

   /**
    * Data for an AIOS analog characteristic instance.
    */
   uint16_t                                      Analog;

   /**
    * Data for the AIOS aggregate characteristic.
    */
   qapi_BLE_AIOS_Aggregate_Characteristic_Data_t Aggregate;
} qapi_BLE_AIOS_Characteristic_Data_t;

/**
 * Structure that represents the format for the Presentation Format
 * descriptor.
 */
typedef struct qapi_BLE_AIOS_Presentation_Format_Data_s
{
   /**
    * Determines how a value for a characteristic instance is
    * formatted.
    *
    * Valid values have the following form: 
    * QAPI_BLE_AIOS_XXX_PRESENTATION_FORMAT_XXX.
    */
   uint8_t  Format;

   /**
    * Used with integer data types to determine how the characteristic
    * instance value is further formatted. The actual value =
    * (characteristic instance value * 10^Exponent).
    */
   uint8_t  Exponent;

   /**
    * Unit of measurement for the characteristic instance.
    */
   uint16_t Unit;

   /**
    * Organization that is responsible for defining the enumerations
    * for the description field
    */
   uint8_t  NameSpace;

   /**
    * An enumerated value that is used to uniquely identify a
    * characteristic instance from another characteristic instance.
    * Digital characteristic instances and analog characteristic
    * instances that include the Presentation Format descriptor must
    * have independent description values. Values should start at 0x0001
    * and go upward. It is worth noting that the AIOS server will also
    * assign a unique ID for each digital or analog characteristic
    * instance that starts at zero. This ID will be used in AIOS server
    * events that contain the qapi_BLE_AIOS_Characteristic_Info_t
    * structure to identify the characteristic instance. This value will
    * be assigned implicitly in the order that characteristic instances
    * are initialized and will always correspond to 1 less than the
    * Description field of that characteristic instance's Presentation
    * Format descriptor's description field.
    */
   uint16_t Description;
} qapi_BLE_AIOS_Presentation_Format_Data_t;

#define QAPI_BLE_AIOS_PRESENTATION_FORMAT_DATA_SIZE            (sizeof(qapi_BLE_AIOS_Presentation_Format_Data_t))
/**< Size of the #qapi_BLE_AIOS_Presentation_Format_Data_t structure. */

/**
 * Structure that represents the format for the Valid Range descriptor
 * that may optionally be included for an analog characteristic
 * instance.
 */
typedef struct qapi_BLE_AIOS_Valid_Range_Data_s
{
   /**
    * Lower bound of the range.
    */
   uint16_t LowerBound;

    /**
    * Upper bound of the range.
    */
   uint16_t UpperBound;
} qapi_BLE_AIOS_Valid_Range_Data_t;

#define QAPI_BLE_AIOS_VALID_RANGE_DATA_SIZE                    (sizeof(qapi_BLE_AIOS_Valid_Range_Data_t))
/**< Size of the #qapi_BLE_AIOS_Valid_Range_Data_t structure. */

/**
 * Enumeration that represents the the possible Condition field of the
 * #qapi_BLE_AIOS_Value_Trigger_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_VTT_STATE_CHANGED_E                    = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_STATE_CHANGED,
   /**< State has changed. */
   QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_VALUE_E    = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_CROSSED_BOUNDARY_ANALOG_VALUE,
   /**< Analog value has crossed the boundary. */
   QAPI_BLE_VTT_ON_BOUDNARY_ANALOG_VALUE_E         = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_ON_BOUNDARY_ANALOG_VALUE,
   /**< Analog value is on the boundary. */
   QAPI_BLE_VTT_STATE_CHANGED_ANALOG_VALUE_E       = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_STATE_CHANGED_ANALOG_VALUE,
   /**< Analog value has changed. */
   QAPI_BLE_VTT_DIGITAL_STATE_CHANGED_BIT_MASK_E   = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_DIGITAL_STATE_CHANGED_BIT_MASK,
   /**< Bitmask for the digital signals whose state changed. */
   QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_INTERVAL_E = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_CROSSED_BOUNDARY_ANALOG_INTERVAL,
   /**< Analog interval has crossed the boundary. */
   QAPI_BLE_VTT_ON_BOUNDARY_ANALOG_INTERVAL_E      = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_ON_BOUNDARY_ANALOG_INTERVAL,
   /**< Analog interval is on the boundary. */
   QAPI_BLE_VTT_NO_VALUE_TRIGGER_E                 = QAPI_BLE_AIOS_VALUE_TRIGGER_SETTING_CONDITION_NO_VALUE_TRIGGER
   /**< No trigger. */
} qapi_BLE_AIOS_Value_Trigger_Type_t;

/**
 * Structure that represents the format for the Value Trigger Setting
 * descriptor that may be optionally included for a digital/analog
 * characteristic instance.
 */
typedef struct qapi_BLE_AIOS_Value_Trigger_Data_s
{
   qapi_BLE_AIOS_Value_Trigger_Type_t             Condition;
   /**< Condition that will cause a digital/analog characteristic
    * instance to be notified or indicated.
    */

   union
   {
      qapi_BLE_AIOS_Digital_Characteristic_Data_t BitMask;
      /**< AIOS digital characteristic instance data.
       */

      uint16_t                                    AnalogValue;
      /**< AIOS analog characteristic instance value.
       */

      qapi_BLE_AIOS_Valid_Range_Data_t            AnalogInterval;
      /**< AIOS analog characteristic instance valid range data.
       */
   }
   ComparisonValue;
   /**< Union that contains the data that is needed based on the
    * Condition field of the Value Trigger Setting data.
    */
} qapi_BLE_AIOS_Value_Trigger_Data_t;

#define QAPI_BLE_AIOS_VALUE_TRIGGER_DATA_SIZE                  (sizeof(qapi_BLE_AIOS_Value_Trigger_Data_t))
/**< Size of the #qapi_BLE_AIOS_Valid_Range_Data_t structure. */

/**
 * Structure that represents the format for the Time Trigger Interval or
 * UINT24 value in seconds. This structure may be included for the
 * ComparisonValue field of the #qapi_BLE_AIOS_Time_Trigger_Data_t
 * structure.
 */
typedef struct qapi_BLE_AIOS_Time_Interval_s
{
   uint8_t  Upper;
   /**< Upper value of the time interval.
    */

   uint16_t Lower;
   /**< Lower value of the time interval.
    */
} qapi_BLE_AIOS_Time_Interval_t;

#define QAPI_BLE_AIOS_TIME_INTERVAL_DATA_SIZE                  (sizeof(qapi_BLE_AIOS_Value_Trigger_Data_t))
/**< Size of the #qapi_BLE_AIOS_Time_Interval_t structure. */

/**
 * Enumeration that represents the the possible Condition field of the
 * #qapi_BLE_AIOS_Time_Trigger_Type_t structure.
 */
typedef enum
{
   QAPI_BLE_TTT_NO_TIME_BASED_TRIGGER_E              = QAPI_BLE_AIOS_TIME_TRIGGER_SETTING_CONDITION_NO_TIME_BASED_TRIGGERING_USED,
   /**< No time-based trigger. */
   QAPI_BLE_TTT_TIME_INTERVAL_IGNORE_VALUE_TRIGGER_E = QAPI_BLE_AIOS_TIME_TRIGGER_SETTING_CONDITION_TIME_INTERVAL_IGNORE_VALUE_TRIGGER,
   /**< Time trigger interval, but ignore the value trigger. */
   QAPI_BLE_TTT_TIME_INTERVAL_CHECK_VALUE_TRIGGER_E  = QAPI_BLE_AIOS_TIME_TRIGGER_SETTING_CONDITION_TIME_INTERVAL_CHECK_VALUE_TRIGGER,
   /**< Time trigger interval, but check the value trigger. */
   QAPI_BLE_TTT_COUNT_CHANGED_MORE_OFTEN_THAN_E      = QAPI_BLE_AIOS_TIME_TRIGGER_SETTING_CONDITION_COUNT_CHANGED_MORE_OFTEN_THAN
   /**< Count trigger. */
} qapi_BLE_AIOS_Time_Trigger_Type_t;

/**
 * Structure that represents the format for the Time Trigger Setting
 * descriptor that may be optionally included for a digital/analog
 * characteristic instance.
 */
typedef struct qapi_BLE_AIOS_Time_Trigger_Data_s
{
   qapi_BLE_AIOS_Time_Trigger_Type_t Condition;
   /**< Condition that will cause a digital/analog characteristic
    * instance to be notified or indicated.
    */

   union
   {
      qapi_BLE_AIOS_Time_Interval_t  TimeInterval;
      /**< Time interval that will trigger a notification or indication
       * for an AIOS characteristic based on the specified time interval.
       */

      uint16_t                       Count;
      /**< Specified count that will trigger a notification or indication
       * for an AIOS characteristic if it has changed a specified number
       * of times.
       */
   }
   ComparisonValue;
   /**< Union that contains the data that is needed based on the
    * Condition field of the Value Trigger Setting data.
    */
} qapi_BLE_AIOS_Time_Trigger_Data_t;

#define QAPI_BLE_AIOS_TIME_TRIGGER_DATA_SIZE                   (sizeof(qapi_BLE_AIOS_Value_Trigger_Data_t))
/**< Size of the #qapi_BLE_AIOS_Time_Trigger_Data_t structure. */

/**
 * Structure that represents the information that is needed to identify
 * an AIOS characteristic.
 */
typedef struct qapi_BLE_AIOS_Characteristic_Info_s
{
   qapi_BLE_AIOS_Characteristic_Type_t Type;
   /**< Identifies the AIOS characteristic type.
    *
    * If the Type field is set to QAPI_BLE_ACT_AGGREGATE_E, the
    * IOType field may be set to QAPI_BLE_IO_INPUT_E,
    * QAPI_BLE_IO_OUTPUT_E, or QAPI_BLE_IO_INPUT_OUTPUT_E depending on
    * the characteristic instance IOTypes that make up the aggregate
    * characteristic when the service was initialized.
    */

   qapi_BLE_AIOS_IO_Type_t             IOType;
   /**< Identifies if the AIOS characteristic is an input, output, or
    * input/output (only aggregate) characteristic.
    */

   uint32_t                            ID;
   /**< Identifies the AIOS characteristic instance.
    *
    * The ID field starts at 0 for all instances. The aggregate
    * characteristic will always have an ID of zero.
    */
} qapi_BLE_AIOS_Characteristic_Info_t;

#define QAPI_BLE_AIOS_CHARACTERISTIC_INFO_DATA_SIZE            (sizeof(qapi_BLE_AIOS_Characteristic_Info_t))
/**< Size of the #qapi_BLE_AIOS_Characteristic_Info_t structure. */

/**
 * Enumeration represents all the events generated by the AIOS
 * server. These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_AIOS_Event_Data_t structure.
 */
typedef enum qapi_BLE_AIOS_Event_Type_s
{
   QAPI_BLE_ET_AIOS_SERVER_READ_CHARACTERISTIC_REQUEST_E,
   /**< Read characteristic request event. */
   QAPI_BLE_ET_AIOS_SERVER_WRITE_CHARACTERISTIC_REQUEST_E,
   /**< Write characteristic request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_CCCD_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_AIOS_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_PRESENTATION_FORMAT_REQUEST_E,
   /**< Read User Description request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_USER_DESCRIPTION_REQUEST_E,
   /**< Read User Description request event. */
   QAPI_BLE_ET_AIOS_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E,
   /**< Write User Description request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_VALUE_TRIGGER_SETTING_REQUEST_E,
   /**< Read Value Trigger Setting request event. */
   QAPI_BLE_ET_AIOS_SERVER_WRITE_VALUE_TRIGGER_SETTING_REQUEST_E,
   /**< Write Value Trigger Setting request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_TIME_TRIGGER_SETTING_REQUEST_E,
   /**< Read Time Trigger Setting request event. */
   QAPI_BLE_ET_AIOS_SERVER_WRITE_TIME_TRIGGER_SETTING_REQUEST_E,
   /**< Write Time Trigger Setting request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_NUMBER_OF_DIGITALS_REQUEST_E,
   /**< Read Number of Digitals request event. */
   QAPI_BLE_ET_AIOS_SERVER_READ_VALID_RANGE_REQUEST_E,
   /**< Read Valid Range request event. */
   QAPI_BLE_ET_AIOS_SERVER_CONFIRMATION_E
   /**< Confirmation event. */
} qapi_BLE_AIOS_Event_Type_t;

/**
 * Structure that represent the format for the data that is dispatched to
 * an AIOS server when an AIOS client has sent a request to read an AIOS
 * characteristic instance.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Characteristic_Request_Data_s
{
   uint32_t                            InstanceID;
   /**< AIOS instance that dispatched the event.
    */

   uint32_t                            ConnectionID;
   /**< GATT connection ID for the connection with the AIOS client
    * that made the request.
    */

   uint32_t                            TransactionID;
   /**< GATT transaction ID for the request.
    */

   qapi_BLE_GATT_Connection_Type_t     ConnectionType;
   /**< GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */

   qapi_BLE_BD_ADDR_t                  RemoteDevice;
   /**< Bluetooth address of the AIOS client that made the request.
    */

   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
   /**< Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
} qapi_BLE_AIOS_Read_Characteristic_Request_Data_t;

#define QAPI_BLE_AIOS_READ_CHARACTERISTIC_REQUEST_DATA_SIZE    (sizeof(qapi_BLE_AIOS_Read_Characteristic_Request_Data_t))
/**< Size of the #qapi_BLE_AIOS_Read_Characteristic_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to write an
 * AIOS characteristic instance.
 *
 * This event can only be received for digital or analog characteristics
 * that are outputs. Digital or analog characteristics that are inputs
 * cannot be written.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Write_Characteristic_Request_Response() function.
 *  */
typedef struct qapi_BLE_AIOS_Write_Characteristic_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Structure that holds the AIOS characteristic data that has been
    * requested to be written.
    */
   qapi_BLE_AIOS_Characteristic_Data_t Data;
} qapi_BLE_AIOS_Write_Characteristic_Request_Data_t;

#define QAPI_BLE_AIOS_WRITE_CHARACTERISTIC_REQUEST_DATA_SIZE   (sizeof(qapi_BLE_AIOS_Write_Characteristic_Request_Data_t))
/**< Size of the #qapi_BLE_AIOS_Write_Characteristic_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * CCCD that is included
 * for an AIOS characteristic instance.
 *
 * Some of this structures fields will be required when responding to a
 * request using the qapi_BLE_AIOS_Read_CCCD_Request_Response()
 * function.
 */
typedef struct qapi_BLE_AIOS_Read_CCCD_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_AIOS_READ_CCCD_REQUEST_DATA_SIZE              (sizeof(qapi_BLE_AIOS_Read_CCCD_Request_Data_t))
/**< Size of the #qapi_BLE_AIOS_Read_CCCD_Request_Data_t structure. */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to write a
 * CCCD that is
 * included for an AIOS characteristic instance.
 *
 * Some of these structures fields will be required when responding to a
 * request using the qapi_BLE_AIOS_Write_CCCD_Request_Response()
 * function.
 */
typedef struct qapi_BLE_AIOS_Write_CCCD_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Configuration value that has been requested to be written.
    */
   uint16_t                            ClientConfiguration;
} qapi_BLE_AIOS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_AIOS_WRITE_CCCD_REQUEST_DATA_SIZE             (sizeof(qapi_BLE_AIOS_Write_CCCD_Request_Data_t))
/**< Size of the #qapi_BLE_AIOS_Write_CCCD_Request_Data_t structure. */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * Presentation Format Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Presentation_Format_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t;

#define QAPI_BLE_AIOS_READ_PRESENTATION_FORMAT_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * User Description Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_User_Description_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_User_Description_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Starting offset of the User Description Descriptor that has
    * been requested to be read. This will be zero for GATT read
    * requests, however if the User Description Descriptor exceeds the
    * MTU requirements for the response, the AIOS client may issue a
    * GATT Read Long requests until the entire User Description
    * Descriptor has been read. The Offset field will be nonzero in
    * this case.
    */
   uint16_t                            Offset;
} qapi_BLE_AIOS_Read_User_Description_Request_Data_t;

#define QAPI_BLE_AIOS_READ_USER_DESCRIPTION_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Read_User_Description_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_User_Description_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to write a
 * User Description Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Write_User_Description_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Write_User_Description_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                             InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                             ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                             TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t      ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                   RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t  CharacteristicInfo;

   /**
    * Length of the User Description Descriptor value that has been
    * requested to be written.
    */
   uint16_t                             UserDescriptionLength;

   /**
    * User Description Descriptor value that has been
    * requested to be written.
    */
   uint8_t                             *UserDescription;
} qapi_BLE_AIOS_Write_User_Description_Request_Data_t;

#define QAPI_BLE_AIOS_WRITE_USER_DESCRIPTION_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Write_User_Description_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Write_User_Description_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * Value Trigger Setting Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_AIOS_READ_VALUE_TRIGGER_SETTING_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to write a
 * Value Trigger Setting Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Structure that holds the Value Trigger Setting data that has been
    * requested to be written.
    */
   qapi_BLE_AIOS_Value_Trigger_Data_t  ValueTriggerSetting;
} qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_AIOS_WRITE_VALUE_TRIGGER_SETTING_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * Time Trigger Setting Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_AIOS_READ_TIME_TRIGGER_SETTING_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to write a
 * Time Trigger Setting Descriptor that is included for an AIOS
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;

   /**
    * Structure that holds the Time Trigger Setting data that has been
    * requested to be written.
    */
   qapi_BLE_AIOS_Time_Trigger_Data_t   TimeTriggerSetting;
} qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t;

#define QAPI_BLE_AIOS_WRITE_TIME_TRIGGER_SETTING_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * Number of Digitals Descriptor that is included for an AIOS Digital
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t;

#define QAPI_BLE_AIOS_READ_NUMBER_OF_DIGITALS_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has sent a request to read a
 * Valid Range Descriptor that is included for an AIOS Analog
 * characteristic instance.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_AIOS_Read_Valid_Range_Request_Response() function.
 */
typedef struct qapi_BLE_AIOS_Read_Valid_Range_Request_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Structure that holds the AIOS characteristic information that
    * identifies the AIOS characteristic instance.
    */
   qapi_BLE_AIOS_Characteristic_Info_t CharacteristicInfo;
} qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t;

#define QAPI_BLE_AIOS_READ_VALID_RANGE_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t))
/**<
 * Size of the #qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an AIOS server when an AIOS client has responded to an outstanding
 * indication that was previously sent by the AIOS server.
 */
typedef struct qapi_BLE_AIOS_Confirmation_Data_s
{
   /**
    * AIOS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the AIOS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the AIOS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the AIOS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;

   /**
    * Number of bytes that were written by the indication.
    */
   uint16_t                        BytesWritten;
} qapi_BLE_AIOS_Confirmation_Data_t;

#define QAPI_BLE_AIOS_CONFIRMATION_DATA_SIZE                   (sizeof(qapi_BLE_AIOS_Confirmation_Data_t))
/**< Size of the #qapi_BLE_AIOS_Confirmation_Data_t structure.  */

/**
 * Structure that represents the container structure for holding all the
 * event data for an AIOS instance.
 */
typedef struct qapi_BLE_AIOS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_AIOS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * AIOS Read characteristic Request data.
       */
      qapi_BLE_AIOS_Read_Characteristic_Request_Data_t         *AIOS_Read_Characteristic_Request_Data;

      /**
       * AIOS Write characteristic Request data.
       */
      qapi_BLE_AIOS_Write_Characteristic_Request_Data_t        *AIOS_Write_Characteristic_Request_Data;

      /**
       * AIOS Read CCCD Request data.
       */
      qapi_BLE_AIOS_Read_CCCD_Request_Data_t                   *AIOS_Read_CCCD_Request_Data;

      /**
       * AIOS Write CCCD Request data.
       */
      qapi_BLE_AIOS_Write_CCCD_Request_Data_t                  *AIOS_Write_CCCD_Request_Data;

      /**
       * AIOS Read Presentation Format Request data.
       */
      qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t    *AIOS_Read_Presentation_Format_Request_Data;

      /**
       * AIOS Read User Descrption Request data.
       */
      qapi_BLE_AIOS_Read_User_Description_Request_Data_t       *AIOS_Read_User_Description_Request_Data;

      /**
       * AIOS Write User Descrption Request data.
       */
      qapi_BLE_AIOS_Write_User_Description_Request_Data_t      *AIOS_Write_User_Description_Request_Data;

      /**
       * AIOS Read Value Trigger Setting Request data.
       */
      qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t  *AIOS_Read_Value_Trigger_Setting_Request_Data;

      /**
       * AIOS Write Value Trigger Setting Request data.
       */
      qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *AIOS_Write_Value_Trigger_Setting_Request_Data;

      /**
       * AIOS Read Time Trigger Setting Request data.
       */
      qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t   *AIOS_Read_Time_Trigger_Setting_Request_Data;

      /**
       * AIOS Write Time Trigger Setting Request data.
       */
      qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t  *AIOS_Write_Time_Trigger_Setting_Request_Data;

      /**
       * AIOS Read Number of Digitals Request data.
       */
      qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t     *AIOS_Read_Number_Of_Digitals_Request_Data;

      /**
       * AIOS Read Number of Digitals Request data.
       */
      qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t            *AIOS_Read_Valid_Range_Request_Data;

      /**
       * AIOS Confirmation data.
       */
      qapi_BLE_AIOS_Confirmation_Data_t                        *AIOS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_AIOS_Event_Data_t;

#define QAPI_BLE_AIOS_EVENT_DATA_SIZE                          (sizeof(qapi_BLE_AIOS_Event_Data_t))
/**< Size of the #qapi_BLE_AIOS_Event_Data_t structure. */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * AIOS Event Callback. This function will be called whenever a define
 * AIOS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in a #qapi_BLE_AIOS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the AIOS Event Data
 * only in the context of this callback. If the caller requires
 * the Data for a longer period of time, the callback function
 * must copy the data into another Data Buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted however,
 * that if the same Callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the User does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another AIOS Event will not be processed while this
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
 * @param[in]  AIOS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User defined value that will be
 *                                   received with the AIOS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_AIOS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_AIOS_Event_Data_t *AIOS_Event_Data, uint32_t CallbackParameter);

   /* AIOS Server API.                                                  */

/**
 * @brief
 * Initializes an AIOS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one AIOS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @note1hang
 * It is the application's responsibilty to meet the size
 * constraints for each digital characteristic instance
 * and the aggregate characteristic. Both sizes must be
 * under (ATT_MTU-3). This function will not check the
 * size constraints since no connection should be
 * established prior to calling this function. APIs that
 * send these characteristics to an AIOS client will check
 * to make sure that the size constraint is met. If this
 * constraint is not met, all APIs that send a
 * digital characteristic instance or the Aggregate
 * characteristic will FAIL with the error
 * QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_MTU_SIZE_EXCEEDED.
 *
 * @note1hang
 * The AIOS server should call the
 * qapi_BLE_GATT_Change_Maximum_Supported_MTU() function prior to
 * initializing the service via a call to
 * qapi_BLE_AIOS_Initialize_XXX(), to set an MTU that is large
 * enough to support the digital characteristic instances
 * and the aggregate characteristic. The AIOS client must
 * call the qapi_BLE_GATT_Exchange_MTU_Request() function to
 * negotiate a higher MTU once a connection has been
 * established. The smaller supported MTU by the AIOS
 * Server and AIOS client will always be the negotiated
 * MTU.
 *
 * @note1hang
 * The InitializeData parameter and substructures must be
 * valid. This API will fail if the structure pointed to
 * by the InitializeData parameter is configured
 * incorrectly. Since there is too much information to
 * cover here, see the #qapi_BLE_AIOS_Initialize_Data_t
 * structure for more information about
 * configuring this structure.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags        Flags that are used to register AIOS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_aiostypes.h and have the
 *                                  form QAPI_BLE_AIOS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData       Pointer that contains the information
 *                                  needed to configure AIOS. This parameter
 *                                  must be configured correctly.
 *
 * @param[in]  EventCallback        The AIOS Event Callback that will
 *                                  receive AIOS server events.
 *
 * @param[in]  CallbackParameter    A user-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered AIOS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero value if successful. The return value will
 *              be the Service Instance ID of AIOS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                    QAPI_BLE_AIOS_ERROR_INSUFFICIENT_RESOURCES \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                    QAPI_BLE_AIOS_ERROR_MAXIMUM_NUMBER_OF_INSTANCES_REACHED \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_ENTRIES \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_INSTANCES \n
 *                    QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_TYPE_ALREADY_REGISTERED \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_PROPERTY_FLAG \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_DESCRIPTOR_FLAG \n
 *                    QAPI_BLE_AIOS_ERROR_INVALID_AGGREGATE_PROPERTY_FLAG \n
 *                    QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                    QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                    QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                    QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                    QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_AIOS_Initialize_Data_t *InitializeData, qapi_BLE_AIOS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an AIOS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_AIOS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one AIOS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @note1hang
 * It is the application's responsibilty to meet the size
 * constraints for each digital characteristic instance
 * and the aggregate characteristic. Both sizes must be
 * under (ATT_MTU-3). This function will not check the
 * size constraints since no connection should be
 * established prior to calling this function. APIs that
 * send these characteristics to an AIOS client will check
 * to make sure that the size constraint is met. If this
 * constraint is not met, all APIs that send a
 * digital characteristic instance or the Aggregate
 * characteristic will fail with the error
 * QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_MTU_SIZE_EXCEEDED.
 *
 * @note1hang
 * The AIOS server should call the
 * qapi_BLE_GATT_Change_Maximum_Supported_MTU() function prior to
 * initializing the service via a call to
 * qapi_BLE_AIOS_Initialize_XXX() to set an MTU that is large
 * enough to support the digital characteristic instances
 * and the aggregate characteristic. The AIOS client must
 * call the qapi_BLE_GATT_Exchange_MTU_Request() function to
 * negotiate a higher MTU once a connection has been
 * established. The smaller supported MTU by the AIOS
 * server and AIOS client will always be the negotiated
 * MTU.
 *
 * @note1hang
 * The InitializeData parameter and substructures must be
 * valid. This API will fail if the structure pointed to
 * by the InitializeData parameter is configured
 * incorrectly. Since there is too much information to
 * cover here, see the #qapi_BLE_AIOS_Initialize_Data_t
 * structure for more information about
 * configuring this structure.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_AIOS_Initialize().
 *
 * @param[in]  Service_Flags        Flags that are used to register AIOS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_aiostypes.h and have the
 *                                  form QAPI_BLE_AIOS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData       Pointer that contains the information
 *                                  needed to configure AIOS. This parameter
 *                                  must be configured correctly.
 *
 * @param[in]  EventCallback        AIOS event callback that will
 *                                  receive AIOS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered AIOS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer that, on input, holds
 *                                       the handle range to store the
 *                                       service in GATT, and on output,
 *                                       contains the handle range for
 *                                       where the service is stored in
 *                                       GATT.
 *
 * @return      Positive, nonzero value if successful. The return value will
 *              be the Service Instance ID of AIOS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_MAXIMUM_NUMBER_OF_INSTANCES_REACHED \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_ENTRIES \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_NUMBER_OF_INSTANCES \n
 *                 QAPI_BLE_AIOS_ERROR_CHARACTERISTIC_TYPE_ALREADY_REGISTERED \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_PROPERTY_FLAG \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_DESCRIPTOR_FLAG \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_AGGREGATE_PROPERTY_FLAG \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED \n
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_AIOS_Initialize_Data_t *InitializeData, qapi_BLE_AIOS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an AIOS instance.
 *
 * @details
 * After this function is called, no other AIOS
 * function can be called until after a successful call to either of the
 * qapi_BLE_AIOS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the AIOS service that is registered with a
 * call to either the qapi_BLE_AIOS_Initialize_Service() or the
 * qapi_BLE_AIOS_Initialize_Service_Handle_Range() function.
 *
 * @details
 * This function will return a variable number of attributes depending
 * on how the service was configured.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Positive non-zero if successful (represents the number of attributes
 *              for AIOS). \n
 *              Zero for failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The ClientConfiguration parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_AIOS_ERROR_CODE_SUCCESS.
 * Otherwise it will be ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  ClientConfiguration   Current value of the CCCD that will be sent
 *                                   if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, uint16_t ClientConfiguration);

/**
 * @brief
 * Responds to a write
 * request from an AIOS client for an AIOS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's Presentation
 * Format descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The PresentationFormatData parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it may excluded
 * (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  PresentationFormatData   Pointer to the current value of
 *                                      the Presentation Format
 *                                      Descriptor that will be sent if
 *                                      the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Presentation_Format_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Presentation_Format_Data_t *PresentationFormatData);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's User
 * Description descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The UserDescription parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it may excluded
 * (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 * request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  UserDescription      Pointer to the current value of the
 *                                  User Description that will  be sent
 *                                  if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_User_Description_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, char *UserDescription);

/**
 * @brief
 * Responds to a write
 * request from an AIOS client for an AIOS characteristic's User
 * Description descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Write_User_Description_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's Value
 * Trigger Setting descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The ValueTriggerData parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it may excluded
 * (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  ValueTriggerData     Pointer to the current value of the
 *                                  Value Trigger Setting that will be
 *                                  sent if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Value_Trigger_Data_t *ValueTriggerData);

/**
 * @brief
 * Rresponds to a write
 * request from an AIOS client for an AIOS characteristic's Value
 * Trigger Setting descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's Time
 * Trigger Setting descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The TimeTriggerData parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it may excluded
 * (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  TimeTriggerData      Pointer to the current value of the
 *                                  Time Trigger Setting that will be
 *                                  sent if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Time_Trigger_Data_t *TimeTriggerData);

/**
 * @brief
 * Responds to a write
 * request from an AIOS client for an AIOS characteristic's Time
 * Trigger Setting descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid Time from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the Time that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS digital characteristic's Number of
 * Digitals descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The NumberOfDigitals parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it will be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  NumberOfDigitals     Number of digitals that will be
 *                                  sent if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, uint8_t NumberOfDigitals);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS analog characteristic's Valid
 * Range descriptor.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The Valid Range parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it may be
 * excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  ValidRangeData       Pointer to the Valid Range that
 *                                  will be sent if the request was
 *                                  successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Valid_Range_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Valid_Range_Data_t *ValidRangeData);

/**
 * @brief
 * Responds to a read
 * request from an AIOS client for an AIOS characteristic's value.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The CharacteristicData parameter is only required if the ErrorCode
 * parameter is QPAI_BLE_AIOS_ERROR_CODE_SUCCESS. Otherwise it will be
 * ignored.
 *
 * @note1hang
 * All contraints for the #qapi_BLE_AIOS_Digital_Characteristic_Data_t
 * structure must be met if this function is responding to a request
 * to read a digital characteristic's value.
 *
 * @note1hang
 * All contraints for the
 * #qapi_BLE_AIOS_Aggregate_Characteristic_Data_t structure must be met
 * if this function is responding to a request to read an Aggregate
 * characteristic's value.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in] ConnectionID         GATT connection ID for the
 *                                 request.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  CharacteristicData   Pointer to the characteristic data
 *                                  that will be sent if the request was
 *                                  successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Read_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Characteristic_Data_t *CharacteristicData);

/**
 * @brief
 * Responds to a write
 * request from an AIOS client for an AIOS characteristic's value.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_AIOS_ERROR_CODE_XXX from qapi_ble_aiostypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Write_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo);

/**
 * @brief
 * Sends a notification
 * for an AIOS characteristic's value to an AIOS client.
 *
 * @details
 * This function sends a notification, but provides no guarantee
 * that the AIOS client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the AIOS
 * characteristic that is going to be notified has been previously
 * configured for notifications. An AIOS client must have written
 * the AIOS characteristic's CCCD to enable notifications.
 *
 * @note1hang
 * All contraints for the #qapi_BLE_AIOS_Digital_Characteristic_Data_t
 * structure must be met if this function is to send the Digital
 * characteristic's value.
 *
 * @note1hang
 * All contraints for the
 * #qapi_BLE_AIOS_Aggregate_Characteristic_Data_t structure must be
 * met if this function is to send the aggregate characteristic's
 * value.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the notification.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  CharacteristicData   Pointer to the
 *                                  characteristic data that will
 *                                  be sent in the notification.
 *
 * @return      Positive non-zero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Notify_Characteristic(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Characteristic_Data_t *CharacteristicData);

/**
 * @brief
 * Sends an indication for an
 * AIOS characteristic's value to an AIOS client.
 *
 * @details
 * This function sends an indication, but provides a guarantee that the
 * AIOS client will receive the value, since the AIOS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the AIOS
 * characteristic that is going to be notified has been previously
 * configured for indications. An AIOS client must have written
 * the AIOS characteristic's CCCD to enable indications.
 *
 * @note1hang
 * All contraints for the #qapi_BLE_AIOS_Digital_Characteristic_Data_t
 * structure must be met if this function is to send the Digital
 * characteristic's value.
 *
 * @note1hang
 * All contraints for the
 * #qapi_BLE_AIOS_Aggregate_Characteristic_Data_t structure must be
 * met if this function is to send the aggregate characteristic's
 * value.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_AIOS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  CharacteristicInfo   Pointer that contains the
 *                                  information about the characteristic
 *                                  for the request.
 *
 * @param[in]  CharacteristicData   Pointer to the
 *                                  characteristic data that will
 *                                  be sent in the indication.
 *
 * @return      Positive non-zero if successful (represents the
 *              GATT transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_IO_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_TYPE \n
 *                 QAPI_BLE_AIOS_ERROR_INVALID_CHARACTERISTIC_INSTANCE_ID \n
 *                 QAPI_BLE_AIOS_ERROR_ATTRIBUTE_HANDLE_INFORMATION_NOT_FOUND \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Indicate_Characteristic(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_AIOS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_AIOS_Characteristic_Data_t *CharacteristicData);

   /* AIOS client API                                                   */

/**
 * @brief
 * Parses a value received from a
 * remote AIOS server interpreting it as a Presentation Format characteristic
 * descriptor.
 *
 * @details
 * This function should be used to decode the Presentation Format
 * characteristic descriptor received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            AIOS server.
 *
 * @param[in]  Value          Value received from the
 *                            AIOS server.
 *
 * @param[out]  PresentationFormatData    Pointer that will hold the
 *                                        Presentation Format descriptor if
 *                                        this function returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Decode_Presentation_Format(uint32_t ValueLength, uint8_t *Value, qapi_BLE_AIOS_Presentation_Format_Data_t *PresentationFormatData);

/**
 * @brief
 * Formats the AIOS Value Trigger
 * Setting characteristic descriptor into a user-specified buffer, for a GATT
 * Write request, that will be sent to the AIOS server. This function may also
 * be used to determine the size of the buffer to hold the formatted data.
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data. The
 * AIOS client may use this size to allocate a buffer necessary to hold the
 * formatted data..
 *
 * @param[in]  ValueTriggerData      Pointer to the Value Trigger
 *                                   Setting characteristic Descriptor
 *                                   data that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in]  BufferLength          Length of the user-specified
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Positive nonzero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the ValueTriggerData is formatted into the specified buffer.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Format_Value_Trigger_Setting(qapi_BLE_AIOS_Value_Trigger_Data_t *ValueTriggerData, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received from
 * a remote AIOS server interpreting it as a Value Trigger Setting
 * characteristic descriptor.
 *
 * @details
 * This function should be used to decode the Value Trigger Setting
 * characteristic descriptor received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            AIOS server.
 *
 * @param[in]  Value          Value received from the
 *                            AIOS server.
 *
 * @param[out]  ValueTriggerData    Pointer that will hold the
 *                                  Presentation Format descriptor
 *                                  if this function returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Decode_Value_Trigger_Setting(uint32_t ValueLength, uint8_t *Value, qapi_BLE_AIOS_Value_Trigger_Data_t *ValueTriggerData);

/**
 * @brief
 * Formats the AIOS Time Trigger
 * Setting characteristic descriptor into a user-specified buffer, for a GATT
 * Write request, that will be sent to the AIOS server. This function may also
 * be used to determine the size of the buffer to hold the formatted data (see
 * below).
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data. The
 * AIOS client may use this size to allocate a buffer necessary to hold the
 * formatted data.
 *
 * @param[in]  TimeTriggerData       Pointer to the Time Trigger
 *                                   Setting characteristic Descriptor
 *                                   data that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in]  BufferLength          Length of the user-specified
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Positive non-zero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the TimeTriggerData is formatted into the specified buffer.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Format_Time_Trigger_Setting(qapi_BLE_AIOS_Time_Trigger_Data_t * TimeTriggerData, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received from
 * a remote AIOS server interpreting it as a Time Trigger Setting
 * characteristic descriptor.
 *
 * @details
 * This function should be used to decode the Time Trigger Setting
 * characteristic descriptor received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the time received from the
 *                            AIOS server.
 *
 * @param[in]  Value          Time received from the
 *                            AIOS server.
 *
 * @param[out]  TimeTriggerData    Pointer that will hold the
 *                                 Presentation Format descriptor
 *                                 if this function returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Decode_Time_Trigger_Setting(uint32_t ValueLength, uint8_t *Value, qapi_BLE_AIOS_Time_Trigger_Data_t *TimeTriggerData);

/**
 * @brief
 * Parses a value received from
 * a remote AIOS server interpreting it as a Valid Range characteristic
 * descriptor.
 *
 * @details
 * This function should be used to decode the Valid Range
 * characteristic descriptor received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the time received from the
 *                            AIOS server.
 *
 * @param[in]  Value          Time received from the
 *                            AIOS server.
 *
 * @param[out]  ValidRangeData    Pointer that will hold the Valid
 *                                Range descriptor if this function
 *                                returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_AIOS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_AIOS_Decode_Valid_Range(uint32_t ValueLength, uint8_t *Value, qapi_BLE_AIOS_Valid_Range_Data_t *ValidRangeData);

/** @}
 */

#endif

