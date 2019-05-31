/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_hmi.h"

qapi_Status_t _qapi_HMI_Initialize(uint8_t TargetID, uint32_t *Interface_ID, qapi_HMI_Event_CB_t HMI_Event_CB, uint32_t CB_Param, qbool_t LatchNVM);

void _qapi_HMI_Shutdown(uint8_t TargetID, uint32_t Interface_ID);

qapi_Status_t _qapi_HMI_Get_Max_MSDU_Length(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MCPS_Data_Request_t *RequestData, uint8_t *MaxMSDULength);

qapi_Status_t _qapi_HMI_MCPS_Data_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MCPS_Data_Request_t *RequestData);

qapi_Status_t _qapi_HMI_MCPS_Purge_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t MSDUHandle, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Associate_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Associate_Request_t *RequestData);

qapi_Status_t _qapi_HMI_MLME_Associate_Response(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Associate_Response_t *ResponseData);

qapi_Status_t _qapi_HMI_MLME_Disassociate_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Disassociate_Request_t *RequestData);

qapi_Status_t _qapi_HMI_MLME_Get_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t *PIBAttributeLength, uint8_t *PIBAttributeValue, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Orphan_Response(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Orphan_Response_t *ResponseData);

qapi_Status_t _qapi_HMI_MLME_Reset_Request(uint8_t TargetID, uint32_t Interface_ID, qapi_HMI_Reset_Level_t ResetLevel, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Rx_Enable_Request(uint8_t TargetID, uint32_t Interface_ID, qbool_t DeferPermit, uint32_t RxOnTime, uint32_t RxOnDuration, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Scan_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Scan_Request_t *RequestData);

uint32_t _qapi_HMI_15p4_Scan_Duration_To_Milliseconds(uint8_t TargetID, uint8_t ScanDuration);

qapi_Status_t _qapi_HMI_MLME_Set_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t PIBAttributeLength, const void *PIBAttributeValue, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Start_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Start_Request_t *RequestData, uint8_t *Status);

qapi_Status_t _qapi_HMI_MLME_Poll_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_MLME_Poll_Request_t *RequestData);

qapi_Status_t _qapi_HMI_VS_Auto_Poll_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_VS_Auto_Poll_Request_t *RequestData, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_DUT_Enable_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_DUT_Tx_Test_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_VS_DUT_Tx_Test_Request_t *RequestData, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_DUT_Rx_Test_Request(uint8_t TargetID, uint32_t Interface_ID, const qapi_HMI_VS_DUT_Rx_Test_Request_t *RequestData, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_DUT_Rx_Stat_Request(uint8_t TargetID, uint32_t Interface_ID, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *ConfirmData);

qapi_Status_t _qapi_HMI_VS_DUT_Cca_Scan_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t Channel, uint8_t Page, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_DUT_Test_End_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_Enable_BBIF(uint8_t TargetID, uint32_t Interface_ID, qbool_t Enable, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_NVM_Get_Request(uint8_t TargetID, uint32_t Interface_ID, uint16_t NVMTag, uint16_t *NVMLength, uint8_t *NVMValue, uint8_t *Status);

qapi_Status_t _qapi_HMI_VS_NVM_Set_Request(uint8_t TargetID, uint32_t Interface_ID, uint16_t NVMTag, uint16_t NVMLength, const uint8_t *NVMValue, uint8_t *Status);
