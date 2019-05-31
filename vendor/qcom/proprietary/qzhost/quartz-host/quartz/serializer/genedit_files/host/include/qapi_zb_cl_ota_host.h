/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_ota.h"

qapi_Status_t _qapi_ZB_CL_OTA_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Client_Config_t *Client_Config, qapi_ZB_CL_OTA_Client_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_OTA_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Server_Config_t *Server_Config, qapi_ZB_CL_OTA_Server_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_OTA_Populate_Attributes(uint8_t TargetID, qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

qapi_Status_t _qapi_ZB_CL_OTA_Client_Discover(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_Addr_Mode_t DstAddrMode, qapi_ZB_Addr_t DstAddress);

qapi_Status_t _qapi_ZB_CL_OTA_Client_Reset(uint8_t TargetID, qapi_ZB_Cluster_t Cluster);

qapi_Status_t _qapi_ZB_CL_OTA_Client_Upgrade_End(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_Status_t Status);

qapi_Status_t _qapi_ZB_CL_OTA_Query_Next_Image(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition, uint8_t Field_Control, uint16_t Hardware_Version);

qapi_Status_t _qapi_ZB_CL_OTA_Image_Block_Start_Transfer(uint8_t TargetID, qapi_ZB_Cluster_t Cluster);

qapi_Status_t _qapi_ZB_CL_OTA_Image_Notify(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *Send_Info, uint8_t Notify_Payload_Type, uint8_t Jitter, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition);

qapi_Status_t _qapi_ZB_CL_OTA_Image_Block_Response(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, const qapi_ZB_CL_OTA_Image_Block_Response_t *ResponseData);
