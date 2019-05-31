/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_common.h"
#include "qapi_zb_aps.h"

qapi_Status_t Mnl_qapi_ZB_APSME_Get_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t *AIBAttributeLength, void *AIBAttributeValue);
qapi_Status_t Mnl_qapi_ZB_APSME_Set_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t AIBAttributeLength, const void *AIBAttributeValue);
qapi_Status_t Mnl_qapi_ZB_APS_Create_Test_Endpoint(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, void **TestEndpoint, uint16_t ProfileID);
qapi_Status_t Mnl_qapi_ZB_APS_Test_Endpoint_Data_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, void *TestEndpoint, const qapi_ZB_APSDE_Data_Request_t *RequestData, const uint16_t RspClusterID);
