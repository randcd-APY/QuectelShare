/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_bdb.h"

qapi_Status_t _qapi_ZB_BDB_Get_Endpoint_Commissioning_Status(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

qapi_Status_t _qapi_ZB_BDB_Set_Endpoint_Commissioning_Status(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint, qapi_Status_t NewStatus);

qapi_Status_t _qapi_ZB_BDB_Get_Endpoint_Commissioning_Group_ID(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint, uint16_t *GroupID);

qapi_Status_t _qapi_ZB_BDB_Get_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_BDB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t *AttributeLength, uint8_t *AttributeValue);

qapi_Status_t _qapi_ZB_BDB_Set_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_BDB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t AttributeLength, const uint8_t *AttributeValue);
