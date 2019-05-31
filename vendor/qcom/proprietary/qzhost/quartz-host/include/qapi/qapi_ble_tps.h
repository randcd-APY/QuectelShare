/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_tps.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Transmit Power Service (TPS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Transmit Power Service programming interface defines the protocols and
 * procedures to be used to implement the Automation IO Service capabilities.
 */

#ifndef __QAPI_BLE_TPS_H__
#define __QAPI_BLE_TPS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_tpstypes.h"  /* QAPI TPS prototypes.                     */

/**
 * @addtogroup qapi_ble_services
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_TPS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_TPS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_TPS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_TPS_ERROR_SERVICE_ALREADY_REGISTERED    (-1003)
/**< Service is already registered. */
#define QAPI_BLE_TPS_ERROR_INVALID_INSTANCE_ID           (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_TPS_ERROR_MALFORMATTED_DATA             (-1005)
/**< Malformatted data. */
#define QAPI_BLE_TPS_ERROR_UNKNOWN_ERROR                 (-1007)
/**< Unknown error. */

#define QAPI_BLE_TPS_TX_POWER_LEVEL_SIZE                 (sizeof(int8_t))
/**< Size of the TX Power level. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a TPS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_TPS_Client_Information_s
{
   /**
    * TPS Power Level attribute handle.
    */
   uint16_t Tx_Power_Level;
} qapi_BLE_TPS_Client_Information_t;

#define QAPI_BLE_TPS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_TPS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_TPS_Client_Information_t structure.
 */

   /* TPS Server API.                                                   */

/**
 * @brief
 * Initializes a TPS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one TPS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered TPS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of the TPS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_TPS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Initialize_Service(uint32_t BluetoothStackID, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a TPS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_TPS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one TPS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered TPS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer that, on input, holds
 *                                       the handle range to store the
 *                                       service in GATT and, on output,
 *                                       contains the handle range for
 *                                       where the service is stored in
 *                                       GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of the TPS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_TPS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Transmit Power Service (TPS) Instance.
 *
 * @details
 * After this function is called, no other Device Information Service
 * function can be called until after a successful call to either of the
 * qapi_BLE_TPS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_TPS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_TPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_TPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the TPS service that is registered with a
 * call to either the qapi_BLE_TPS_Initialize_Service() or the
 * qapi_BLE_TPS_Initialize_Service_Handle_Range() function.
 *
 * @return      Positive, nonzero if successful (represents the number of attributes
 *              for TPS).
 *
 * @return      Zero for failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the Tx power level.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_TPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Tx_Power_Level      Transmit power level that will be
 *                                 set if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_TPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_TPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Set_Tx_Power_Level(uint32_t BluetoothStackID, uint32_t InstanceID, int8_t Tx_Power_Level);

/**
 * @brief
 * Queries the Tx power level.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_TPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Tx_Power_Level     Pointer that will hold the transmit
 *                                 power level on successful return from
 *                                 this function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_TPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_TPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_TPS_Query_Tx_Power_Level(uint32_t BluetoothStackID, uint32_t InstanceID, int8_t *Tx_Power_Level);

/**
 * @}
 */

#endif

