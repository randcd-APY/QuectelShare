/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_bdb.h
   @brief QAPI for the ZigBee base device behavior.

   @addtogroup qapi_zb_bdb
   @{

   This API provides definitions, commands related to the ZigBee Base Device
   Behavior (BDB) specification.

   @}
*/

#ifndef  __QAPI_ZB_BDB_H__ // [
#define  __QAPI_ZB_BDB_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb.h"

/** @addtogroup qapi_zb_bdb
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions are the ZigBee qapi_Status_t codes for the BDB
   layer of the ZigBee stack.  Consult qapi_zb.h for other status codes. */
#define QAPI_ZB_ERR_BDB_IN_PROGRESS                                     (QAPI_ZB_ERR(97))  /**< ZigBee BDB status indicating that commissioning is already in progress. */
#define QAPI_ZB_ERR_BDB_NOT_ADDRESS_ASSIGNMENT_CAPABLE                  (QAPI_ZB_ERR(98))  /**< ZigBee BDB status indicating that the device is not address assignment
                                                                                                capableduring touchlink. */
#define QAPI_ZB_ERR_BDB_NO_NETWORK                                      (QAPI_ZB_ERR(99))  /**< ZigBee BDB status indicating that no network was found. */
#define QAPI_ZB_ERR_BDB_TARGET_FAILURE                                  (QAPI_ZB_ERR(100)) /**< ZigBee BDB status indicating that a node has not joined the network when
                                                                                                requested during touchlink. */
#define QAPI_ZB_ERR_BDB_FORMATION_FAILURE                               (QAPI_ZB_ERR(101)) /**< ZigBee BDB status indicating that a network could not be formed. */
#define QAPI_ZB_ERR_BDB_NO_IDENTIFY_QUERY_RESPONSE                      (QAPI_ZB_ERR(102)) /**< ZigBee BDB status indicating that no response was received to an identify
                                                                                                query command. */
#define QAPI_ZB_ERR_BDB_NO_BINDING_TABLE_FULL                           (QAPI_ZB_ERR(103)) /**< ZigBee BDB status indicating that a new binding table entry could not be
                                                                                                created because the table was full. */
#define QAPI_ZB_ERR_BDB_NO_SCAN_RESPONSE                                (QAPI_ZB_ERR(104)) /**< ZigBee BDB status indicating that no response was received to an inter-PAN
                                                                                                scan request. */
#define QAPI_ZB_ERR_BDB_NOT_PERMITTED                                   (QAPI_ZB_ERR(105)) /**< ZigBee BDB status indicating that the operation was not permitted. */
#define QAPI_ZB_ERR_BDB_TCLK_EX_FAILURE                                 (QAPI_ZB_ERR(106)) /**< ZigBee BDB status indicating that the trust center link key exchange
                                                                                                procedure failed. */
#define QAPI_ZB_ERR_BDB_UNSUPPORTED_ATTRIBUTE                           (QAPI_ZB_ERR(109)) /**< ZigBee BDB status indicating that the specified attribute is not supported
                                                                                                by the device. */
#define QAPI_ZB_ERR_BDB_INVALID_INDEX                                   (QAPI_ZB_ERR(110)) /**< ZigBee BDB status indicating that a get or set request was issued with an
                                                                                                invalid attribute index. */
#define QAPI_ZB_ERR_BDB_INVALID_PARAMETER                               (QAPI_ZB_ERR(111)) /**< ZigBee BDB status indicating that an invalid parameter was specified. */

/* The following definitions represent bitmask values for the
   bdbCommissioningMode. */
#define QAPI_ZB_BDB_COMMISSIONING_MODE_TOUCHLINK                        (0x01) /**< ZigBee BDB commissioning mode bitmask indicating touchlink. */
#define QAPI_ZB_BDB_COMMISSIONING_MODE_NETWORK_STEERING                 (0x02) /**< ZigBee BDB commissioning mode bitmask indicating network steering. */
#define QAPI_ZB_BDB_COMMISSIONING_MODE_NETWORK_FORMATION                (0x04) /**< ZigBee BDB commissioning mode bitmask indicating network formation. */
#define QAPI_ZB_BDB_COMMISSIONING_MODE_NETWORK_FINDING_AND_BINDING      (0x08) /**< ZigBee BDB commissioning mode bitmask indicating finding and binding. */
#define QAPI_ZB_BDB_COMMISSIONING_MODE_DEFAULT                          (QAPI_ZB_BDB_COMMISSIONING_MODE_NETWORK_FORMATION | QAPI_ZB_BDB_COMMISSIONING_MODE_NETWORK_STEERING)

/* The following definitions represent bitmask values for the
   bdbCommissioningCapability. */
#define QAPI_ZB_BDB_COMMISSIONING_CAPABILITY_NETWORK_STEERING           (0x01) /**< ZigBee BDB commissioning capability bitmask indicating network steering. */
#define QAPI_ZB_BDB_COMMISSIONING_CAPABILITY_NETWORK_FORMATION          (0x02) /**< ZigBee BDB commissioning capability bitmask indicating network formation. */
#define QAPI_ZB_BDB_COMMISSIONING_CAPABILITY_FINDING_AND_BINDING        (0x04) /**< ZigBee BDB commissioning capability bitmask indicating finding and binding. */
#define QAPI_ZB_BDB_COMMISSIONING_CAPABILITY_TOUCHLINK                  (0x08) /**< ZigBee BDB commissioning capability bitmask indicating Touchlink. */

#define QAPI_ZB_BDB_COMMISSIONING_DFAULT_GROUP_ID                       (0xFFFF) /**< ZigBee BDB default group ID for commissioning. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the ZigBee base device behavior (BDB) attribute IDs.
*/
typedef enum
{
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_COMMISSIONING_MODE_E               = 0x1001, /**< Commissioning mode. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_JOINING_NODE_EUI64_E               = 0x1002, /**< Joining node IEEE address. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_JOINING_NODE_NEW_TC_LINK_KEY_E     = 0x1003, /**< Joining node new trust center link key. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_JOIN_USES_INSTALL_CODE_KEY_E       = 0x1004, /**< Join use install code key. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_NODE_COMMISSIONING_CAPABILITY_E    = 0x1005, /**< Node commissioning capability. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_NODE_IS_ON_A_NETWORK_E             = 0x1006, /**< Node is on a network. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_NODE_JOIN_LINK_KEY_TYPE_E          = 0x1007, /**< Node join link key type. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_PRIMARY_CHANNEL_SET_E              = 0x1008, /**< Primary channel set. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_SCAN_DURATION_E                    = 0x1009, /**< Scan duration. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_SECONDARY_CHANNEL_SET_E            = 0x100A, /**< Secondary channel set. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_TC_LINK_KEY_EXCHANGE_ATTEMPS_E     = 0x100B, /**< Trust center link key exchange attempts. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_TC_LINK_KEY_EXCHANGE_ATTEMPS_MAX_E = 0x100C, /**< Trust center link key exchange maximum attempts. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_TC_LINK_KEY_EXCHANGE_METHOD_E      = 0x100D, /**< Trust center link key exchange method. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_TC_NODE_JOIN_TIMEOUT_E             = 0x100E, /**< Trust center node join timeout. */
   QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_TC_REQUIRE_KEY_EXCHANGE_E          = 0x100F  /**< Trust center require key exchange. */
} qapi_ZB_BDB_Attribute_ID_t;

/**
   Enumeration of the valid values for the bdbNodeJoinLinkKeyType attribute.
*/
typedef enum
{
   QAPI_ZB_BDB_NODE_JOIN_LINK_KEY_TYPE_DEFAULT_GLOBAL_TRUST_CENTER_E        = 0x00,  /**< Default global Trust Center link key. */
   QAPI_ZB_BDB_NODE_JOIN_LINK_KEY_TYPE_DISTRIBUTED_SECURITY_GLOBAL_E        = 0x01,  /**< Distributed security global link key. */
   QAPI_ZB_BDB_NODE_JOIN_LINK_KEY_TYPE_INSTALL_CODE_DERIVED_PRECONFIGURED_E = 0x02,  /**< Install code derived preconfigured link key. */
   QAPI_ZB_BDB_NODE_JOIN_LINK_KEY_TYPE_TOUCHLINK_PRECONFIGURED_E            = 0x03   /**< Touchlink preconfigured link key. */
} qapi_ZB_BDB_Node_Join_Link_Key_Type_t;

/**
   Enumeration of the valid values for the bdbTCLinkKeyExchangeMethod attribute.
*/
typedef enum
{
   QAPI_ZB_BDB_TC_LINK_KEY_EXCHANGE_METHOD_APS_REQUEST_KEY_E                = 0x00, /**< APS request key. */
   QAPI_ZB_BDB_TC_LINK_KEY_EXCHANGE_METHOD_CERTIFICATE_BASED_KEY_EXCHANGE_E = 0x01  /**< Certificate-based key exchange (CBKE). */
} qapi_ZB_BDB_TC_Link_Key_Exchange_Method_t;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Gets the commissioning status of the specified endpoint.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Endpoint  Endpoint whose commissioning status is being read.

   @return
     The current status of commissioning on the endpoint.
*/
qapi_Status_t qapi_ZB_BDB_Get_Endpoint_Commissioning_Status(qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

/**
   @brief Sets the commissioning status of the specified endpoint.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Endpoint  Endpoint whose commissioning status is being set.
   @param[in] NewStatus New commissioning status.

   @return
     The current status of commissioning on the endpoint.
*/
qapi_Status_t qapi_ZB_BDB_Set_Endpoint_Commissioning_Status(qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint, qapi_Status_t NewStatus);

/**
   @brief Gets the commissioning group ID of an endpoint.

   @param[in]  ZB_Handle Handle of the ZigBee instance.
   @param[in]  Endpoint  Endpoint whose commissioning group ID is being read.
   @param[out] GroupID   Pointer to where the commissioning group ID will be
                         stored upon successful return.

   @return
     The current status of commissioning on the endpoint.
*/
qapi_Status_t qapi_ZB_BDB_Get_Endpoint_Commissioning_Group_ID(qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint, uint16_t *GroupID);

/**
   @brief Implements the ZigBee BDB-GET.request primitive.

   @param[in]     ZB_Handle       Handle of the ZigBee instance.
   @param[in]     AttributeId     Identifier of the requested attribute.
   @param[in]     AttributeIndex  Attribute index to be read (when applicable).
   @param[in,out] AttributeLength Pointer to where the length of the requested
                                  attribute is located. The initial value of
                                  this parameter should be set to the actual
                                  length of the AttributeValue buffer. Upon
                                  successful return (or an error of
                                  QAPI_ERR_BOUNDS), this value will be set to
                                  the actual length of the attribute.
   @param[out]    AttributeValue  Pointer to the buffer where the requested
                                  attribute will be stored upon success.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_BDB_Get_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_BDB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t *AttributeLength, uint8_t *AttributeValue);

/**
   @brief Implements the ZigBee BDB-SET.request primitive.

   @param[in] ZB_Handle       Handle of the ZigBee instance.
   @param[in] AttributeId     Identifier of the attribute being set.
   @param[in] AttributeIndex  Index to be written (when applicable).
   @param[in] AttributeLength Length of the AttributeValue buffer.
   @param[in] AttributeValue  Buffer containing the attribute to be set.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_BDB_Set_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_BDB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t AttributeLength, const uint8_t *AttributeValue);

#endif // ] #ifndef __QAPI_ZB_BDB_H__

/** @} */

