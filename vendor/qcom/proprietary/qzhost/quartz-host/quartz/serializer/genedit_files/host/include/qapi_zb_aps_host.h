/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_aps.h"

qapi_Status_t _qapi_ZB_APS_Register_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Event_CB_t APS_Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_APS_Unregister_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Event_CB_t APS_Event_CB);

qapi_Status_t _qapi_ZB_APSDE_Data_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSDE_Data_Request_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Bind_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Bind_Data_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Unbind_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Bind_Data_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Get_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t *AIBAttributeLength, void *AIBAttributeValue);

qapi_Status_t _qapi_ZB_APSME_Set_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t AIBAttributeLength, const void *AIBAttributeValue);

qapi_Status_t _qapi_ZB_APSME_Add_Group_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Group_Data_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Remove_Group_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Group_Data_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Remove_All_Groups_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

qapi_Status_t _qapi_ZB_APSME_Transport_Key_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Transport_Key_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Update_Device_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Update_Device_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Remove_Device_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Remove_Device_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Request_Key_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Request_Key_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Switch_Key_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Switch_Key_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Verify_Key_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Verify_Key_t *RequestData);

qapi_Status_t _qapi_ZB_APSME_Confirm_Key_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Confirm_Key_t *RequestData);

qapi_Status_t _qapi_ZB_APS_Add_Endpoint(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Add_Endpoint_t *RequestData);

qapi_Status_t _qapi_ZB_APS_Remove_Endpoint(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

qapi_Status_t _qapi_ZB_APS_Create_Test_Endpoint(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, void **TestEndpoint, uint16_t ProfileID);

qapi_Status_t _qapi_ZB_APS_Destroy_Test_Endpoint(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, void *TestEndpoint);

qapi_Status_t _qapi_ZB_APS_Test_Endpoint_Data_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, void *TestEndpoint, const qapi_ZB_APSDE_Data_Request_t *RequestData, const uint16_t RspClusterID);

qapi_Status_t _qapi_ZB_APS_Add_Device_Key_Pair_Set(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Key_Pair_t *RequestData);

qapi_Status_t _qapi_ZB_APS_Get_Device_Key_Pair_Set(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t PartnerAddr, qapi_ZB_Key_Type_t KeyType, qapi_ZB_APS_Get_Key_Confirm_t *ConfirmData);

qapi_Status_t _qapi_ZB_APS_Remove_Device_Key_Pair_Set(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t PartnerAddr, qapi_ZB_Key_Type_t KeyType);

qapi_Status_t _qapi_ZB_APS_ADD_Fragmented_Data_Drop(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t BlockNumber);

qapi_Status_t _qapi_ZB_APS_Clear_Fragmented_Data_Drop(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle);

qapi_Status_t _qapi_ZB_APS_Hash_Install_Code(uint8_t TargetID, const uint8_t *InstallCode, uint8_t *LinkKey);

qapi_Status_t _qapi_ZB_APS_Add_Link_Key(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t ExtAddr, const uint8_t *LinkKey);

qapi_Status_t _qapi_ZB_APS_Set_Update_Device_Key_Id(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_zb_Key_ID_t SecurityMethod);
