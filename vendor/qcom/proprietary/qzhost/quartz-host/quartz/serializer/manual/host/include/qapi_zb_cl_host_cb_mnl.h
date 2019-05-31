/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl.h"

void Mnl_Host_qapi_ZB_CL_Custom_Cluster_Event_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer);
uint8_t *zb_cl_HERH_Create_GetScene_Rsp(qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Result, uint32_t *DataLength);
uint8_t *zb_cl_HERH_Create_SetScene_Rsp(qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Result, uint32_t *DataLength);
uint8_t *zb_cl_HERH_Create_ReadAttr_Rsp(qapi_ZB_CL_Attr_Custom_Read_t *Result, uint32_t *DataLength);
uint8_t *zb_cl_HERH_Create_WriteAttr_Rsp(qapi_ZB_CL_Attr_Custom_Write_t *Result, uint32_t *DataLength);

