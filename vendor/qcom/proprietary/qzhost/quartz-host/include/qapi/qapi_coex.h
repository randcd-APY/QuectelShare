/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_coex
@{ */

/** @file qapi_coex.h
   @brief QAPI for coexistence.

   This file defines the public API for coexistence, allowing methods for
   setting and getting configuration of the coexistence manager for WLAN,
   Bluetooth LE, 802.15.4, and external devices.
*/

#ifndef __QAPI_COEX_H__ // [
#define __QAPI_COEX_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include <stdint.h>
#include "com_dtypes.h"
#include "qapi/qapi_status.h"
#include "qapi/qapi_wlan_base.h"
#include "qapi/qapi_ble_gap.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/**
 * Defines the flags used to set coexistence configuration.
*/
#define QAPI_COEX_CONFIG_FLAG_EXT_ENABLE           0x00000001
#define QAPI_COEX_CONFIG_FLAG_EXT_MASTER           0x00000002

/**
 * Defines the flags used to set concurrency configuration.
*/
#define QAPI_COEX_TX_TX_CONCUR_ENABLE              0x00000001
#define QAPI_COEX_TX_RX_CONCUR_ENABLE              0x00000002
#define QAPI_COEX_RX_RX_CONCUR_ENABLE              0x00000004

/**
 * Defines the maximum priority value of an individual transaction type.
*/
#define QAPI_COEX_MAXIMUM_PRIORITY_LEVEL_VALUE     62

/**
 * Defines the maximum length of the number of statistics
 * counters to be returned.
 */
#define QAPI_COEX_STATISTICS_DATA_LENGTH_MAXIMUM   32

/**
 * Defines the packet status types for coexistence statistics.
 */
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_COMPLETE              0x0001
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_STOMP                 0x0002
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_COMPLETE      0x0003
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_STOMP         0x0004
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_COMPLETE         0x0005
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_STOMP            0x0006
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_COMPLETE           0x0007
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_STOMP              0x0008
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_COMPLETE           0x0009
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_STOMP              0x000A
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_COMPLETE           0x000B
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_STOMP              0x000C
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_COMPLETE           0x000D
#define QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_STOMP              0x000E
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_COMPLETE         0x000F
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_STOMP            0x0010
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_COMPLETE     0x0011
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_STOMP        0x0012
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_COMPLETE         0x0013
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_STOMP            0x0014
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_COMPLETE         0x0015
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_STOMP            0x0016
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_COMPLETE          0x0017
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_STOMP             0x0018
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_COMPLETE          0x0019
#define QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_STOMP             0x001A

/**
 * Defines the flags to be set for the coexistence statistics
 * mask.
*/
#define QAPI_COEX_STATISTICS_MASK_BLE_SCAN_COMPLETE              0x00000001
#define QAPI_COEX_STATISTICS_MASK_BLE_SCAN_STOMP                 0x00000002
#define QAPI_COEX_STATISTICS_MASK_BLE_SCAN_CONNECT_COMPLETE      0x00000004
#define QAPI_COEX_STATISTICS_MASK_BLE_SCAN_CONNECT_STOMP         0x00000008
#define QAPI_COEX_STATISTICS_MASK_BLE_ADVERTISE_COMPLETE         0x00000010
#define QAPI_COEX_STATISTICS_MASK_BLE_ADVERTISE_STOMP            0x00000020
#define QAPI_COEX_STATISTICS_MASK_BLE_DATA_RX_COMPLETE           0x00000040
#define QAPI_COEX_STATISTICS_MASK_BLE_DATA_RX_STOMP              0x00000080
#define QAPI_COEX_STATISTICS_MASK_BLE_DATA_TX_COMPLETE           0x00000100
#define QAPI_COEX_STATISTICS_MASK_BLE_DATA_TX_STOMP              0x00000200
#define QAPI_COEX_STATISTICS_MASK_BLE_ISOC_RX_COMPLETE           0x00000400
#define QAPI_COEX_STATISTICS_MASK_BLE_ISOC_RX_STOMP              0x00000800
#define QAPI_COEX_STATISTICS_MASK_BLE_ISOC_TX_COMPLETE           0x00001000
#define QAPI_COEX_STATISTICS_MASK_BLE_ISOC_TX_STOMP              0x00002000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ED_SCAN_COMPLETE         0x00004000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ED_SCAN_STOMP            0x00008000
#define QAPI_COEX_STATISTICS_MASK_I15P4_BEACON_SCAN_COMPLETE     0x00010000
#define QAPI_COEX_STATISTICS_MASK_I15P4_BEACON_SCAN_STOMP        0x00020000
#define QAPI_COEX_STATISTICS_MASK_I15P4_DATA_RX_COMPLETE         0x00040000
#define QAPI_COEX_STATISTICS_MASK_I15P4_DATA_RX_STOMP            0x00080000
#define QAPI_COEX_STATISTICS_MASK_I15P4_DATA_TX_COMPLETE         0x00100000
#define QAPI_COEX_STATISTICS_MASK_I15P4_DATA_TX_STOMP            0x00200000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ACK_RX_COMPLETE          0x00400000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ACK_RX_STOMP             0x00800000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ACK_TX_COMPLETE          0x01000000
#define QAPI_COEX_STATISTICS_MASK_I15P4_ACK_TX_STOMP             0x02000000

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
 * Wireless protocol types supported, used to set the priority
 * of each mode in qapi_COEX_Configure()
*/
typedef enum {
   QAPI_COEX_WIRELESS_TYPE_INVALID,  /**< Invalid wireless type. */
   QAPI_COEX_WIRELESS_TYPE_BLE,      /**< Bluetooth Low Energy wireless type. */
   QAPI_COEX_WIRELESS_TYPE_I15P4,    /**< 802.15.4 wireless type. */
   QAPI_COEX_WIRELESS_TYPE_EXT,      /**< External interface wireless type. */
   QAPI_COEX_WIRELESS_TYPE_WLAN      /**< WLAN wireless type. */
} qapi_COEX_Wireless_Type_t;

/**
 * Structure which contains configuration information for
 * coexistence. Each priority level must have a unique wireless
 * type assigned to it with Priority_1 being the highest
 * priority level.
*/
typedef struct qapi_COEX_Config_Data_s {
   uint32                     config_Flags;
   /**< Configuration bitmask of the form QAPI_COEX_CONFIG_FLAG_XXX. */
   qapi_COEX_Wireless_Type_t  priority_1;   /**< The number 1 (highest) priority wireless type. */
   qapi_COEX_Wireless_Type_t  priority_2;   /**< The number 2 priority wireless type. */
   qapi_COEX_Wireless_Type_t  priority_3;   /**< The number 3 priority wireless type. */
   qapi_COEX_Wireless_Type_t  priority_4;   /**< The number 4 (lowest) priority wireless type. */
} qapi_COEX_Config_Data_t;

/**
 * Enumeration which contains the configurable priority types.
*/

typedef enum qapi_COEX_Priority_Type_s {
   QAPI_COEX_PRIORITY_BLE_ADVERTISE_E,       /**< BLE Advertise priority type. */
   QAPI_COEX_PRIORITY_BLE_SCAN_E,            /**< BLE Scan priority type. */
   QAPI_COEX_PRIORITY_BLE_DATA_REQUEST_E,    /**< BLE Data Request priority type. */
   QAPI_COEX_PRIORITY_BLE_DATA_ACTIVE_E,     /**< BLE Data Active priority type. */
   QAPI_COEX_PRIORITY_I15P4_RX_REQUEST_E,    /**< 802.15.4 Rx Request priority type. */
   QAPI_COEX_PRIORITY_I15P4_TX_REQUEST_E,    /**< 802.15.4 Tx Request priority type. */
   QAPI_COEX_PRIORITY_I15P4_RX_ACTIVE_E,     /**< 802.15.4 Rx Active priority type. */
   QAPI_COEX_PRIORITY_I15P4_TX_ACTIVE_E,     /**< 802.15.4 Tx Active priority type. */
   QAPI_COEX_PRIORITY_I15P4_ACK_E,           /**< 802.15.4 ACK priority type. */
   QAPI_COEX_PRIORITY_I15P4_ED_SCAN_E,       /**< 802.15.4 ED scan priority type. */
   QAPI_COEX_PRIORITY_I15P4_HOLD_REQUEST_E,  /**< 802.15.4 Hold request priority type. */
   QAPI_COEX_PRIORITY_EXT_LOW_REQUEST_E,     /**< External Low Request priority type. */
   QAPI_COEX_PRIORITY_EXT_HIGH_REQUEST_E,    /**< External High Request priority type. */
   QAPI_COEX_PRIORITY_EXT_LOW_ACTIVE_E,      /**< External Low Active priority type. */
   QAPI_COEX_PRIORITY_EXT_HIGH_ACTIVE_E      /**< External High Active priority type. */
} qapi_COEX_Priority_Type_t;


/**
 * @brief Enumeration which contains the event type to bind in CoEx module
 */
typedef enum qapi_COEX_Bind_Event_Type_s {
    QAPI_BLE_GAP_LE_EVENT = 0,               /**< BLE GAP connection event type. */
} qapi_COEX_Bind_Event_t;

/**
 * @brief Structure which contain the callback for
 *        binding purpose. The format of callback name
 *        will follow the _##qapi_COEX_Bind_Event_t##_CB
 *        The _DUMMY_CB can be refered for assignment.
 */
typedef struct{
    union {
        qapi_BLE_GAP_LE_Event_Callback_t _QAPI_BLE_GAP_LE_EVENT_CB;    /**< Bluetopia Event callback of BLE GAP */
        void*                            _DUMMY_CB;                    /**< Dummy callback for assignment */
    }cb;
} qapi_COEX_Bind_Callback_t;

/**
 * Structure which contains priority information for
 * coexistence.
*/

typedef struct qapi_COEX_Priority_Config_s {
   qapi_COEX_Priority_Type_t priority_Type;     /**< Priority type for the configuration. */
   uint8                     priority_Value;    /**< Priority value for the configuration. */
} qapi_COEX_Priority_Config_t;

/**
 * Structure which contains configuration information for
 * Bluetooth Low Energy coexistence.
*/
typedef struct qapi_COEX_BLE_Config_Data_s {
   uint32 flags;     /**< Flags for Bluetooth Low Energy Coex configuration. */
} qapi_COEX_BLE_Config_Data_t;

/**
 * Structure which contains configuration information for
 * 802.15.4 coexistence.
*/
typedef struct qapi_COEX_I15P4_Config_Data_s {
   uint32 flags;     /**< Flags for 802.15.4 Coex configuration. */
} qapi_COEX_I15P4_Config_Data_t;

/**
 * Structure which contains configuration information for
 * external device coexistence.
*/
typedef struct qapi_COEX_EXT_Config_Data_s {
   uint32 flags;     /**< Flags for external PTA configuration. */
} qapi_COEX_EXT_Config_Data_t;

/**
 * Enumeration which represents the valid WLAN coexistence
 * interface modes.
*/
typedef enum {
   QAPI_COEX_WLAN_MODE_PRIORITY_E,     /**< WLAN Coex Priority mode. */
   QAPI_COEX_WLAN_MODE_TX_RX_E         /**< WLAN Coex Rx/Tx mode. */
} qapi_COEX_WLAN_Mode_t;

/**
 * Structure which contains configuration information for WLAN
 * coexistence.
*/
typedef struct qapi_COEX_WLAN_Config_Data_s {
   uint32                flags;        /**< Flags for WLAN Coex configuration. */
   qapi_COEX_WLAN_Mode_t WLAN_Mode;    /**< Mode of the WLAN Coex. */
} qapi_COEX_WLAN_Config_Data_t;

/**
 * Structure which contains advanced configuration information
 * for coexistence.
*/
typedef struct qapi_COEX_Advanced_Config_Data_s {
   uint32                        config_Flags;
   /**< Configuration bitmask of the form QAPI_COEX_CONFIG_FLAG_XXX. */
   uint32                        concurrency_Flags;
   /**< Concurrency bitmask of the form QAPI_COEX_XX_XX_CONCUR_ENABLE. */
   qapi_COEX_BLE_Config_Data_t   BLE_Config_Data;
   /**< Bluetooth low energy Coex configuration data. */
   qapi_COEX_I15P4_Config_Data_t I15P4_Config_Data;
   /**< 802.15.4 Coex configuration data. */
   qapi_COEX_EXT_Config_Data_t   EXT_Config_Data;
   /**< External PTA Coex configuration data. */
   qapi_COEX_WLAN_Config_Data_t  WLAN_Config_Data;
   /**< WLAN Coex configuration data. */
   qapi_COEX_Priority_Config_t  *priority_Config_Data;
   /**< Pointer to an array of Coex priority configuration data type elements. */
   uint8                         priority_Config_Length;
   /**< Length of the priority configuration array. */
} qapi_COEX_Advanced_Config_Data_t;

/**
 * Structure which contains packet counter information for
 * coexistence statistics.
*/
typedef struct qapi_COEX_Statistics_Data_s
{
   uint16 packet_Status_Type;
   /**< Type of the packet status counter, of the form QAPI_COEX_PACKET_STATUS_TYPE_XXX. */
   uint32 packet_Status_Count;
   /**< Value of the packet status counter. */
} qapi_COEX_Statistics_Data_t;

typedef uint32 (*wlan_coex_callback_t)(uint8 deviceId,
                                       uint32 cbId,
                                       void *pApplicationContext,
                                       void  *payload,
                                       uint32 payload_Length);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
 * @brief This function configures the coexistence module.
 *
 * This function should be called to configure the coexistence
 * module after after initialization. The parameters passed into
 * this function will dictate how the coexistence module is
 * configured and prioritizes transactions. If advanced features
 * need to be configured, then qapi_COEX_Configure_Advanced()
 * should be used instead.
 *
 * @param config_Data  A pointer to a structure specifying
 *                     advanced configuration of the coexistence
 *                     module. Each priority level in the
 *                     structure must have a unique wireless
 *                     type assigned to it, with Priority_1
 *                     being the highest priority. For instance,
 *                     it will not be allowed to assign
 *                     QAPI_COEX_WIRELESS_TYPE_WLAN to both
 *                     Priority_1 and Priority_2.
 *
 * @see                qapi_COEX_Configure_Advanced()
 *
 * @return             The return value will provide the result
 *                     status of the configuration.
*/
qapi_Status_t qapi_COEX_Configure(const qapi_COEX_Config_Data_t *config_Data);

/**
 * @brief This function configures advanced parameters of the
 * coexistence module.
 *
 * This function should be called to configure the coexistence
 * module afterafter initialization. The parameters passed into
 * this function will dictate how the coexistence module is
 * configured and prioritizes transactions. If only basic
 * features need to be configured, then qapi_COEX_Configure()
 * should be used instead.
 *
 * @param advanced_Config_Data  A pointer to a structure specifying
 *                              advanced configuration of the
 *                              coexistence module. Each
 *                              priority level in the structure
 *                              must have a unique priority
 *                              value, no greater than
 *                              QAPI_COEX_MAXIMUM_PRIORITY_LEVEL.
 *
 * @see                         qapi_COEX_Configure()
 *
 * @return                      The return value will provide
 *                              the result status of the
 *                              configuration.
*/
qapi_Status_t qapi_COEX_Configure_Advanced(const qapi_COEX_Advanced_Config_Data_t *advanced_Config_Data);

/**
 * @brief This function enables coexistence statistics packet
 *        counters.
 *
 * This function should be called to enable or disable
 * coexistence statistics counters.
 *
 * @param[in] enable           A boolean value which enables coexistence
 *                         statistics when set to TRUE or disables
 *                         coexistence statistics when set to
 *                         FALSE. When set to FALSE, the value
 *                         of statistics_Mask is ignored.
 *
 * @param[in] statistics_Mask  A bitmask of the values to enable
 *                         counting, of the form
 *                         QAPI_COEX_STATISTICS_MASK_XXX.
 *                         When enable is set to FALSE, the
 *                         value of this parameter is ignored.
 *
 * @param[in] flags            Unused.
 *
 * @return                 The return value will provide the
 *                         result status of the enabling.
 *
 * @see                    qapi_COEX_Statistics_Get()
*/
qapi_Status_t qapi_COEX_Statistics_Enable(boolean enable, uint32 statistics_Mask, uint32 flags);

/**
 * @brief This function gets coexistence statistics packet
 *        counters.
 *
 * This function should be called to retrieve coexistence
 * statistics packet counters.
 *
 * @param[in] statistics_Data         A pointer to the buffer array
 *                                of statistics data the
 *                                function should copy into.
 *                                Upon function success, will
 *                                contain the retrieved data, with
 *                                the packet_Status_Type set to
 *                                a value of the form
 *                                QAPI_COEX_PACKET_STATUS_TYPE_XXX.
 *
 * @param[in] statistics_Data_Length  A pointer that provides the
 *                                number of elements in the
 *                                statistics_Data array, and
 *                                upon function success, will
 *                                also contain the number of
 *                                elements copied into the
 *                                buffer array.
 *
 * @param[in] statistics_Mask         A bitmask of the values to
 *                                retrieve, of the form
 *                                QAPI_COEX_STATISTICS_MASK_XXX.
 *
 * @param[in] reset                   A boolean value which resets
 *                                the statistics counters when
 *                                set to TRUE and does not reset
 *                                them when set to FALSE.
 *
 * @see                           qapi_COEX_Statistics_Enable()
 *
 * @return                        The return value will provide
 *                                the result status of the
 *                                statistics retrieval.
*/
qapi_Status_t qapi_COEX_Statistics_Get(qapi_COEX_Statistics_Data_t *statistics_Data, uint8 *statistics_Data_Length, uint32 statistics_Mask, boolean reset);

/**
 * @brief This function binds the common application layer callback like bluetooth. The CoEx module 
 *        will check the type of qapi_COEX_Bind_Event_t then choose the right strategy in CoEx to give
 *        better connection and throughput experience.
 * 
 * @param type                    current supported event type to bind
 * @param cb                      the original callback to bind
 * @return                        qapi_COEX_Bind_Callback_t 
 */
qapi_COEX_Bind_Callback_t qapi_Coex_Bind_Callback(qapi_COEX_Bind_Event_t type, void* cb);


/**
 * @brief This function wraps the qapi_Coex_Bind_Callback, check details in qapi_Coex_Bind_Callback.
 * 
 * @param type                    current supported event type to bind
 * @param callbback               the original callback to bind
 * @return                        A bind callback with the return type defined by parameter type
 */
#define QAPI_COEX_BIND_CB(type, callback) qapi_Coex_Bind_Callback(type, callback).cb._##type##_CB

/**
 * @brief This function retrieves the profile id  from the cached WLAN coex configuration.
 *
 *
 * @param Config                  A pointer to a configuration struct of type
 *                                qapi_WLAN_Coex_Current_Config containing a single
 *                                configuration option.
 *
 * @param ConfigToCache           Param of type qapi_WLAN_Coex_Config_t to specify which config to cache.
 *
 *
 * @return                        void
 */
uint32 qapi_Coex_WLAN_Cache_Get_Profile(qapi_WLAN_Coex_Current_Config *Config, qapi_WLAN_Coex_Config_t ConfigToCache);

/**
 * @brief This function returns if Coex configuration is in progress.
 *
 *
 * @param Config                  A pointer to a configuration struct of type
 *                                qapi_WLAN_Coex_Current_Config containing a single
 *                                configuration option.
 *
 * @param ConfigToCache           Param of type qapi_WLAN_Coex_Config_t to specify which config to cache.
 *
 *
 * @return                        void
 */
boolean qapi_Coex_WLAN_Cache_Is_Config_In_Progress(void);

/**
 * @brief This function returns if Coex set callback function is a success. The callback is called for 
 * certain wifi events that the coex middle ware can register.
 *
 *
 * @param callback                A pointer to a callback function
 *
 * @param application_Context     A void pointer to a context that will be used by the call back function
 *
 *
 * @return                        qapi_Status_t
 */
qapi_Status_t qapi_WLAN_Set_Coex_Callback (qapi_WLAN_Coex_Callback_t callback, const void *application_Context);


/** @} */

#endif // ] ifndef __QAPI_COEX_H__
