/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_cl_scenes_common.h"
#include "qapi_zb_cl_common.h"

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Extension_Field_Set_t(qapi_ZB_CL_Scenes_Extension_Field_Set_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_EXTENSION_FIELD_SET_T_MIN_PACKED_SIZE;

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Add_Scene_t(qapi_ZB_CL_Scenes_Add_Scene_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_ADD_SCENE_T_MIN_PACKED_SIZE;

        if(Structure->SceneName != NULL)
        {
            qsResult += (strlen((const char *)Structure->SceneName)+1);
        }

        if(Structure->ExtensionFieldSets != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Extension_Field_Set_t(&((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Add_Scene_Response_t(qapi_ZB_CL_Scenes_Add_Scene_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_ADD_SCENE_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_View_Scene_Response_t(qapi_ZB_CL_Scenes_View_Scene_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_VIEW_SCENE_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->SceneName != NULL)
        {
            qsResult += (strlen((const char *)Structure->SceneName)+1);
        }

        if(Structure->ExtensionFieldSets != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Extension_Field_Set_t(&((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(qapi_ZB_CL_Scenes_Remove_Scene_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_REMOVE_SCENE_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_REMOVE_ALL_SCENES_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Store_Scene_Response_t(qapi_ZB_CL_Scenes_Store_Scene_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_STORE_SCENE_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->SceneList != NULL)
        {
            qsResult += (Structure->SceneCount);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Copy_Scene_t(qapi_ZB_CL_Scenes_Copy_Scene_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_COPY_SCENE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(qapi_ZB_CL_Scenes_Copy_Scene_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_COPY_SCENE_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Client_Event_Data_t(qapi_ZB_CL_Scenes_Client_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Command_Complete_t((qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ADD_SCENE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Add_Scene_Response_t((qapi_ZB_CL_Scenes_Add_Scene_Response_t *)&Structure->Data.Add_Scene_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_VIEW_SCENE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_View_Scene_Response_t((qapi_ZB_CL_Scenes_View_Scene_Response_t *)&Structure->Data.View_Scene_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_SCENE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Remove_Scene_Response_t((qapi_ZB_CL_Scenes_Remove_Scene_Response_t *)&Structure->Data.Remove_Scene_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_ALL_SCENES_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t((qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *)&Structure->Data.Remove_All_Scenes_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_STORE_SCENE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Store_Scene_Response_t((qapi_ZB_CL_Scenes_Store_Scene_Response_t *)&Structure->Data.Store_Scene_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_GET_SCENE_MEMBERSHIP_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t((qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *)&Structure->Data.Get_Scene_Membership_Response);
                break;
            case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COPY_SCENE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Scenes_Copy_Scene_Response_t((qapi_ZB_CL_Scenes_Copy_Scene_Response_t *)&Structure->Data.Copy_Scene_Response);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Scenes_Server_Event_Data_t(qapi_ZB_CL_Scenes_Server_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_SCENES_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t((qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Extension_Field_Set_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Extension_Field_Set_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Extension_Field_Set_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Add_Scene_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Add_Scene_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Add_Scene_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsEnhanced);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TransitionTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ExtensionFieldCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->SceneName);

         if((qsResult == ssSuccess) && (Structure->SceneName != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SceneName, 1, (strlen((const char *)(Structure->SceneName))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ExtensionFieldSets);

         if((qsResult == ssSuccess) && (Structure->ExtensionFieldSets != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Scenes_Extension_Field_Set_t(Buffer, &((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Add_Scene_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Add_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Add_Scene_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsEnhanced);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_View_Scene_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_View_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_View_Scene_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsEnhanced);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TransitionTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ExtensionFieldCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->SceneName);

         if((qsResult == ssSuccess) && (Structure->SceneName != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SceneName, 1, (strlen((const char *)(Structure->SceneName))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ExtensionFieldSets);

         if((qsResult == ssSuccess) && (Structure->ExtensionFieldSets != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Scenes_Extension_Field_Set_t(Buffer, &((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Remove_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Store_Scene_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Store_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Store_Scene_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Capacity);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->SceneList);

         if((qsResult == ssSuccess) && (Structure->SceneList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SceneList, sizeof(uint8_t), Structure->SceneCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Copy_Scene_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Copy_Scene_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Copy_Scene_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CopyAllScenes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupIdFrom);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneIdFrom);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupIdTo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneIdTo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Copy_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupIdFrom);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SceneIdFrom);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Client_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Command_Complete_t(Buffer, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ADD_SCENE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Add_Scene_Response_t(Buffer, (qapi_ZB_CL_Scenes_Add_Scene_Response_t *)&Structure->Data.Add_Scene_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_VIEW_SCENE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_View_Scene_Response_t(Buffer, (qapi_ZB_CL_Scenes_View_Scene_Response_t *)&Structure->Data.View_Scene_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_SCENE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(Buffer, (qapi_ZB_CL_Scenes_Remove_Scene_Response_t *)&Structure->Data.Remove_Scene_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_ALL_SCENES_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(Buffer, (qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *)&Structure->Data.Remove_All_Scenes_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_STORE_SCENE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Store_Scene_Response_t(Buffer, (qapi_ZB_CL_Scenes_Store_Scene_Response_t *)&Structure->Data.Store_Scene_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_GET_SCENE_MEMBERSHIP_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(Buffer, (qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *)&Structure->Data.Get_Scene_Membership_Response);
                     break;
                 case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COPY_SCENE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(Buffer, (qapi_ZB_CL_Scenes_Copy_Scene_Response_t *)&Structure->Data.Copy_Scene_Response);
                     break;
                 default:
                     break;
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Scenes_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Scenes_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Scenes_Server_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Data_t(Buffer, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                     break;
                 default:
                     break;
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Extension_Field_Set_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Extension_Field_Set_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_EXTENSION_FIELD_SET_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), Structure->Length);
            }
        }
        else
            Structure->Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Add_Scene_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Add_Scene_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_ADD_SCENE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsEnhanced);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TransitionTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ExtensionFieldCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->SceneName = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->SceneName == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SceneName, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->SceneName = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ExtensionFieldSets = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Scenes_Extension_Field_Set_t)*(Structure->ExtensionFieldCount)));

            if(Structure->ExtensionFieldSets == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Scenes_Extension_Field_Set_t(Buffer, BufferList, &((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
                }
            }
        }
        else
            Structure->ExtensionFieldSets = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Add_Scene_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Add_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_ADD_SCENE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsEnhanced);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_View_Scene_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_View_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_VIEW_SCENE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsEnhanced);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TransitionTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ExtensionFieldCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->SceneName = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->SceneName == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SceneName, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->SceneName = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ExtensionFieldSets = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Scenes_Extension_Field_Set_t)*(Structure->ExtensionFieldCount)));

            if(Structure->ExtensionFieldSets == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->ExtensionFieldCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Scenes_Extension_Field_Set_t(Buffer, BufferList, &((qapi_ZB_CL_Scenes_Extension_Field_Set_t *)Structure->ExtensionFieldSets)[qsIndex]);
                }
            }
        }
        else
            Structure->ExtensionFieldSets = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Remove_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_REMOVE_SCENE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_REMOVE_ALL_SCENES_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Store_Scene_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Store_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_STORE_SCENE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Capacity);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->SceneList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->SceneCount)));

            if(Structure->SceneList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SceneList, sizeof(uint8_t), Structure->SceneCount);
            }
        }
        else
            Structure->SceneList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Copy_Scene_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Copy_Scene_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_COPY_SCENE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CopyAllScenes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupIdFrom);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneIdFrom);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupIdTo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneIdTo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Copy_Scene_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_COPY_SCENE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupIdFrom);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SceneIdFrom);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Command_Complete_t(Buffer, BufferList, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ADD_SCENE_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Add_Scene_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Add_Scene_Response_t *)&Structure->Data.Add_Scene_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_VIEW_SCENE_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_View_Scene_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_View_Scene_Response_t *)&Structure->Data.View_Scene_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_SCENE_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Remove_Scene_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Remove_Scene_Response_t *)&Structure->Data.Remove_Scene_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_ALL_SCENES_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t *)&Structure->Data.Remove_All_Scenes_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_STORE_SCENE_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Store_Scene_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Store_Scene_Response_t *)&Structure->Data.Store_Scene_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_GET_SCENE_MEMBERSHIP_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t *)&Structure->Data.Get_Scene_Membership_Response);
                    break;
                case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COPY_SCENE_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Scenes_Copy_Scene_Response_t(Buffer, BufferList, (qapi_ZB_CL_Scenes_Copy_Scene_Response_t *)&Structure->Data.Copy_Scene_Response);
                    break;
                default:
                    break;
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Scenes_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Scenes_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_SCENES_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Data_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                    break;
                default:
                    break;
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
