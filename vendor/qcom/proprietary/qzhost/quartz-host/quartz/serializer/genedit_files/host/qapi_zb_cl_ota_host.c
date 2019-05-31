/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "idlist.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_ota_host_mnl.h"
#include "qapi_zb_cl_ota_common.h"
#include "qapi_zb_cl_ota_host_cb.h"
#include "qapi_zb_cl_common.h"

qapi_Status_t _qapi_ZB_CL_OTA_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Client_Config_t *Client_Config, qapi_ZB_CL_OTA_Client_CB_t Event_CB, uint32_t CB_Param)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Handle event callback registration. */
    uint32_t qsCbParam = 0;
    CallbackInfo_t CallbackInfo;
    CallbackInfo.TargetID = TargetID;
    CallbackInfo.ModuleID = QS_MODULE_ZIGBEE;
    CallbackInfo.FileID = QAPI_ZB_CL_OTA_FILE_ID;
    CallbackInfo.CallbackID = QAPI_ZB_CL_OTA_CLIENT_CB_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = Event_CB;
    CallbackInfo.AppParam = (uint32_t)CB_Param;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_ZB_CL_OTA_Client_CB_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CB_Param = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (8 + CalcPackedSize_qapi_ZB_CL_Cluster_Info_t((qapi_ZB_CL_Cluster_Info_t *)Cluster_Info) + CalcPackedSize_qapi_ZB_CL_OTA_Client_Config_t((qapi_ZB_CL_OTA_Client_Config_t *)Client_Config) + (QS_POINTER_HEADER_SIZE * 3));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_CREATE_CLIENT_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ZB_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CB_Param);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Cluster_Info);

        if((qsResult == ssSuccess) && (Cluster_Info != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_Cluster_Info_t(&qsInputBuffer, (qapi_ZB_CL_Cluster_Info_t *)Cluster_Info);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Client_Config);

        if((qsResult == ssSuccess) && (Client_Config != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Config_t(&qsInputBuffer, (qapi_ZB_CL_OTA_Client_Config_t *)Client_Config);
        }

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)Cluster);
                    }

                    /* Update the event callback ID. */
                    if(qsRetVal == QAPI_OK)
                        qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)Cluster);
                    else
                        Callback_UnregisterByHandle(qsCbParam);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Client_Config_t *Client_Config, qapi_ZB_CL_OTA_Client_CB_t Event_CB, uint32_t CB_Param)
{
   return _qapi_ZB_CL_OTA_Create_Client(qsTargetId, ZB_Handle, Cluster, Cluster_Info, Client_Config, Event_CB, CB_Param);
}

qapi_Status_t _qapi_ZB_CL_OTA_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Server_Config_t *Server_Config, qapi_ZB_CL_OTA_Server_CB_t Event_CB, uint32_t CB_Param)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Handle event callback registration. */
    uint32_t qsCbParam = 0;
    CallbackInfo_t CallbackInfo;
    CallbackInfo.TargetID = TargetID;
    CallbackInfo.ModuleID = QS_MODULE_ZIGBEE;
    CallbackInfo.FileID = QAPI_ZB_CL_OTA_FILE_ID;
    CallbackInfo.CallbackID = QAPI_ZB_CL_OTA_SERVER_CB_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = Event_CB;
    CallbackInfo.AppParam = (uint32_t)CB_Param;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_ZB_CL_OTA_Server_CB_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CB_Param = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (8 + CalcPackedSize_qapi_ZB_CL_Cluster_Info_t((qapi_ZB_CL_Cluster_Info_t *)Cluster_Info) + CalcPackedSize_qapi_ZB_CL_OTA_Server_Config_t((qapi_ZB_CL_OTA_Server_Config_t *)Server_Config) + (QS_POINTER_HEADER_SIZE * 3));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_CREATE_SERVER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ZB_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CB_Param);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Cluster_Info);

        if((qsResult == ssSuccess) && (Cluster_Info != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_Cluster_Info_t(&qsInputBuffer, (qapi_ZB_CL_Cluster_Info_t *)Cluster_Info);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Server_Config);

        if((qsResult == ssSuccess) && (Server_Config != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_OTA_Server_Config_t(&qsInputBuffer, (qapi_ZB_CL_OTA_Server_Config_t *)Server_Config);
        }

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)Cluster);
                    }

                    /* Update the event callback ID. */
                    if(qsRetVal == QAPI_OK)
                        qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)Cluster);
                    else
                        Callback_UnregisterByHandle(qsCbParam);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Server_Config_t *Server_Config, qapi_ZB_CL_OTA_Server_CB_t Event_CB, uint32_t CB_Param)
{
   return _qapi_ZB_CL_OTA_Create_Server(qsTargetId, ZB_Handle, Cluster, Cluster_Info, Server_Config, Event_CB, CB_Param);
}

qapi_Status_t _qapi_ZB_CL_OTA_Populate_Attributes(uint8_t TargetID, qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    /* Create a temporary variable for the length of AttributeList. */
    uint32_t qsTmp_AttributeCount = 0;
    if (AttributeCount != NULL)
        qsTmp_AttributeCount = *AttributeCount;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + CalcPackedSize_8((uint8_t *)AttributeCount) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_POPULATE_ATTRIBUTES_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Server);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeCount);

        if((qsResult == ssSuccess) && (AttributeCount != NULL))
        {
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)AttributeCount);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeList);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)AttributeCount);
                    }

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE) && (AttributeCount != NULL))
                    {
                        if(qsTmp_AttributeCount > *AttributeCount)
                            qsTmp_AttributeCount = *AttributeCount;
                        for (qsIndex = 0; qsIndex < qsTmp_AttributeCount; qsIndex++)
                        {
                            if(qsResult == ssSuccess)
                                qsResult = PackedRead_qapi_ZB_CL_Attribute_t(&qsOutputBuffer, &qsBufferList, &((qapi_ZB_CL_Attribute_t *)AttributeList)[qsIndex]);
                        }
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList)
{
   return _qapi_ZB_CL_OTA_Populate_Attributes(qsTargetId, Server, AttributeCount, AttributeList);
}

qapi_Status_t _qapi_ZB_CL_OTA_Client_Discover(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_Addr_Mode_t DstAddrMode, qapi_ZB_Addr_t DstAddress)
{
    return(Mnl_qapi_ZB_CL_OTA_Client_Discover(TargetID, Cluster, DstAddrMode, DstAddress));
}

qapi_Status_t qapi_ZB_CL_OTA_Client_Discover(qapi_ZB_Cluster_t Cluster, qapi_ZB_Addr_Mode_t DstAddrMode, qapi_ZB_Addr_t DstAddress)
{
   return _qapi_ZB_CL_OTA_Client_Discover(qsTargetId, Cluster, DstAddrMode, DstAddress);
}

qapi_Status_t _qapi_ZB_CL_OTA_Client_Reset(uint8_t TargetID, qapi_ZB_Cluster_t Cluster)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_CLIENT_RESET_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Client_Reset(qapi_ZB_Cluster_t Cluster)
{
   return _qapi_ZB_CL_OTA_Client_Reset(qsTargetId, Cluster);
}

qapi_Status_t _qapi_ZB_CL_OTA_Client_Upgrade_End(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_Status_t Status)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_CLIENT_UPGRADE_END_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Status);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Client_Upgrade_End(qapi_ZB_Cluster_t Cluster, qapi_Status_t Status)
{
   return _qapi_ZB_CL_OTA_Client_Upgrade_End(qsTargetId, Cluster, Status);
}

qapi_Status_t _qapi_ZB_CL_OTA_Query_Next_Image(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition, uint8_t Field_Control, uint16_t Hardware_Version)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (7 + CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)Image_Definition) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_QUERY_NEXT_IMAGE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Field_Control);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&Hardware_Version);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Image_Definition);

        if((qsResult == ssSuccess) && (Image_Definition != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(&qsInputBuffer, (qapi_ZB_CL_OTA_Image_Definition_t *)Image_Definition);
        }

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Query_Next_Image(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition, uint8_t Field_Control, uint16_t Hardware_Version)
{
   return _qapi_ZB_CL_OTA_Query_Next_Image(qsTargetId, Cluster, Image_Definition, Field_Control, Hardware_Version);
}

qapi_Status_t _qapi_ZB_CL_OTA_Image_Block_Start_Transfer(uint8_t TargetID, qapi_ZB_Cluster_t Cluster)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_IMAGE_BLOCK_START_TRANSFER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Image_Block_Start_Transfer(qapi_ZB_Cluster_t Cluster)
{
   return _qapi_ZB_CL_OTA_Image_Block_Start_Transfer(qsTargetId, Cluster);
}

qapi_Status_t _qapi_ZB_CL_OTA_Image_Notify(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *Send_Info, uint8_t Notify_Payload_Type, uint8_t Jitter, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (6 + CalcPackedSize_qapi_ZB_CL_General_Send_Info_t((qapi_ZB_CL_General_Send_Info_t *)Send_Info) + CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)Image_Definition) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_IMAGE_NOTIFY_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Notify_Payload_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Send_Info);

        if((qsResult == ssSuccess) && (Send_Info != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_General_Send_Info_t(&qsInputBuffer, (qapi_ZB_CL_General_Send_Info_t *)Send_Info);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Image_Definition);

        if((qsResult == ssSuccess) && (Image_Definition != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(&qsInputBuffer, (qapi_ZB_CL_OTA_Image_Definition_t *)Image_Definition);
        }

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Image_Notify(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *Send_Info, uint8_t Notify_Payload_Type, uint8_t Jitter, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition)
{
   return _qapi_ZB_CL_OTA_Image_Notify(qsTargetId, Cluster, Send_Info, Notify_Payload_Type, Jitter, Image_Definition);
}

qapi_Status_t _qapi_ZB_CL_OTA_Image_Block_Response(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, const qapi_ZB_CL_OTA_Image_Block_Response_t *ResponseData)
{
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    BufferListEntry_t *qsBufferList = NULL;
    SerStatus_t        qsResult = ssSuccess;
    uint32_t           qsIndex = 0;
    uint16_t           qsSize = 0;
    Boolean_t          qsPointerValid = FALSE;

    UNUSED(qsIndex);
    UNUSED(qsPointerValid);

    /* Return value. */
    qapi_Status_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)APSDEData) + CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)ZCLHeader) + CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Response_t((qapi_ZB_CL_OTA_Image_Block_Response_t *)ResponseData) + (QS_POINTER_HEADER_SIZE * 3));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_IMAGE_BLOCK_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)APSDEData);

        if((qsResult == ssSuccess) && (APSDEData != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(&qsInputBuffer, (qapi_ZB_APSDE_Data_Indication_t *)APSDEData);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ZCLHeader);

        if((qsResult == ssSuccess) && (ZCLHeader != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_Header_t(&qsInputBuffer, (qapi_ZB_CL_Header_t *)ZCLHeader);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ResponseData);

        if((qsResult == ssSuccess) && (ResponseData != NULL))
        {
            qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Block_Response_t(&qsInputBuffer, (qapi_ZB_CL_OTA_Image_Block_Response_t *)ResponseData);
        }

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Status_t qapi_ZB_CL_OTA_Image_Block_Response(qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, const qapi_ZB_CL_OTA_Image_Block_Response_t *ResponseData)
{
   return _qapi_ZB_CL_OTA_Image_Block_Response(qsTargetId, Cluster, APSDEData, ZCLHeader, ResponseData);
}
