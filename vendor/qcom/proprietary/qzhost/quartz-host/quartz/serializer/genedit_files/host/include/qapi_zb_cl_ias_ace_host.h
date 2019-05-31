/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_ias_ace.h"

qapi_Status_t _qapi_ZB_CL_IASACE_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASACE_Client_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_IASACE_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASACE_Server_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_IASACE_Populate_Attributes(uint8_t TargetID, qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Arm(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ZoneID, qapi_ZB_CL_IASACE_Arm_Mode_t ArmMode, const uint8_t *ArmCode);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Bypass(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t NumberOfZones, const uint8_t *ZoneIDList, const uint8_t *ArmCode);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Emergency(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Fire(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Panic(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Get_ZoneID_Map(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Info(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ZoneID);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Get_Panel_Status(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Get_Bypassed_Zone_List(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

qapi_Status_t _qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Status(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_IASACE_Get_Zone_Status_t *Request);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Set_Arm_Code(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const uint8_t *ArmCode);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Get_Arm_Code(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ArmCodeLength, uint8_t *ArmCode);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Set_Panel_Status(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_IASACE_Panel_Status_Info_t *PanelStatus);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Get_Panel_Status(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Panel_Status_Info_t *PanelStatus);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Add_Zone(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_IASACE_Zone_Table_Entry_t *Request);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Delete_Zone(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ZoneID);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Set_Zone_Status(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, uint16_t ZoneStatus, uint8_t AudibleNotification, qapi_ZB_CL_IASACE_Alarm_Status_t AlarmStatus);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Set_Bypass_Permit(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qbool_t Permit);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Set_Bypass(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qbool_t Bypass, qapi_ZB_CL_IASACE_Bypass_Result_t *BypassResult);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Find_Zone_By_ID(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *ZoneInfo, uint16_t *ZoneStatus);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Find_Zone_By_Address(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint64_t ZoneAddress, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *ZoneInfo, uint16_t *ZoneStatus);

qapi_Status_t _qapi_ZB_CL_IASACE_Server_Get_Zone_List(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, uint8_t *ListSize, uint8_t *ZoneList);
