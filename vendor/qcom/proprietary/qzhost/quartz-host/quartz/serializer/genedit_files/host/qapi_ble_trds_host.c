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
#include "qapi_ble.h"
#include "qapi_ble_trds_common.h"
#include "qapi_ble_trds_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_trdstypes_common.h"

int _qapi_BLE_TRDS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID)
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
    int qsRetVal = 0;

    /* Handle event callback registration. */
    uint32_t qsCbParam = 0;
    CallbackInfo_t CallbackInfo;
    CallbackInfo.TargetID = TargetID;
    CallbackInfo.ModuleID = QS_MODULE_BLE;
    CallbackInfo.FileID = QAPI_BLE_TRDS_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_TRDS_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = EventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_TRDS_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (12 + CalcPackedSize_qapi_BLE_TRDS_Initialize_Data_t((qapi_BLE_TRDS_Initialize_Data_t *)InitializeData) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_INITIALIZE_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Service_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)InitializeData);

        if((qsResult == ssSuccess) && (InitializeData != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_TRDS_Initialize_Data_t(&qsInputBuffer, (qapi_BLE_TRDS_Initialize_Data_t *)InitializeData);
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
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)ServiceID);
                    }

                    /* Update the event callback ID. */
                    if(qsRetVal >= 0)
                        qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)qsRetVal);
                    else
                        Callback_UnregisterByHandle(qsCbParam);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID)
{
   return _qapi_BLE_TRDS_Initialize_Service(qsTargetId, BluetoothStackID, Service_Flags, InitializeData, EventCallback, CallbackParameter, ServiceID);
}

int _qapi_BLE_TRDS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange)
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
    int qsRetVal = 0;

    /* Handle event callback registration. */
    uint32_t qsCbParam = 0;
    CallbackInfo_t CallbackInfo;
    CallbackInfo.TargetID = TargetID;
    CallbackInfo.ModuleID = QS_MODULE_BLE;
    CallbackInfo.FileID = QAPI_BLE_TRDS_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_TRDS_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = EventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_TRDS_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (12 + CalcPackedSize_qapi_BLE_TRDS_Initialize_Data_t((qapi_BLE_TRDS_Initialize_Data_t *)InitializeData) + CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t((qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange) + (QS_POINTER_HEADER_SIZE * 3));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_INITIALIZE_SERVICE_HANDLE_RANGE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Service_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)InitializeData);

        if((qsResult == ssSuccess) && (InitializeData != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_TRDS_Initialize_Data_t(&qsInputBuffer, (qapi_BLE_TRDS_Initialize_Data_t *)InitializeData);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceHandleRange);

        if((qsResult == ssSuccess) && (ServiceHandleRange != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsInputBuffer, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange);
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
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)ServiceID);
                    }

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange);
                    }

                    /* Update the event callback ID. */
                    if(qsRetVal >= 0)
                        qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)qsRetVal);
                    else
                        Callback_UnregisterByHandle(qsCbParam);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return _qapi_BLE_TRDS_Initialize_Service_Handle_Range(qsTargetId, BluetoothStackID, Service_Flags, InitializeData, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange);
}

int _qapi_BLE_TRDS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_CLEANUP_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    /* Un-register the callback. */
                    if(qsRetVal >= 0)
                        Callback_UnregisterByKey(MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_EVENT_CALLBACK_T_CALLBACK_ID, (uint32_t)InstanceID);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID)
{
   return _qapi_BLE_TRDS_Cleanup_Service(qsTargetId, BluetoothStackID, InstanceID);
}

uint32_t _qapi_BLE_TRDS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID)
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
    uint32_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_QUERY_NUMBER_ATTRIBUTES_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

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
                        qsRetVal = 0;
                    }
                }
                else
                {
                    qsRetVal = 0;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = 0;
            }
        }
        else
        {
            qsRetVal = 0;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = 0;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

uint32_t qapi_BLE_TRDS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID)
{
   return _qapi_BLE_TRDS_Query_Number_Attributes(qsTargetId, BluetoothStackID, InstanceID);
}

int _qapi_BLE_TRDS_Write_Control_Point_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 13;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_WRITE_CONTROL_POINT_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ErrorCode);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Write_Control_Point_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode)
{
   return _qapi_BLE_TRDS_Write_Control_Point_Request_Response(qsTargetId, BluetoothStackID, InstanceID, TransactionID, ErrorCode);
}

int _qapi_BLE_TRDS_Indicate_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (12 + CalcPackedSize_qapi_BLE_TRDS_Control_Point_Response_Data_t((qapi_BLE_TRDS_Control_Point_Response_Data_t *)ResponseData) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_INDICATE_CONTROL_POINT_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ResponseData);

        if((qsResult == ssSuccess) && (ResponseData != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_TRDS_Control_Point_Response_Data_t(&qsInputBuffer, (qapi_BLE_TRDS_Control_Point_Response_Data_t *)ResponseData);
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
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Indicate_Control_Point_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData)
{
   return _qapi_BLE_TRDS_Indicate_Control_Point_Response(qsTargetId, BluetoothStackID, InstanceID, ConnectionID, ResponseData);
}

int _qapi_BLE_TRDS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type, uint16_t Configuration)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 19;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_READ_CCCD_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ErrorCode);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&Configuration);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type, uint16_t Configuration)
{
   return _qapi_BLE_TRDS_Read_CCCD_Request_Response(qsTargetId, BluetoothStackID, InstanceID, TransactionID, ErrorCode, Type, Configuration);
}

int _qapi_BLE_TRDS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 17;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_WRITE_CCCD_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ErrorCode);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type)
{
   return _qapi_BLE_TRDS_Write_CCCD_Request_Response(qsTargetId, BluetoothStackID, InstanceID, TransactionID, ErrorCode, Type);
}

int _qapi_BLE_TRDS_Format_Control_Point_Request(uint8_t TargetID, qapi_BLE_TRDS_Control_Point_Request_Data_t *RequestData, uint32_t BufferLength, uint8_t *Buffer)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + CalcPackedSize_qapi_BLE_TRDS_Control_Point_Request_Data_t((qapi_BLE_TRDS_Control_Point_Request_Data_t *)RequestData) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_FORMAT_CONTROL_POINT_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BufferLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)RequestData);

        if((qsResult == ssSuccess) && (RequestData != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_TRDS_Control_Point_Request_Data_t(&qsInputBuffer, (qapi_BLE_TRDS_Control_Point_Request_Data_t *)RequestData);
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
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)Buffer, sizeof(uint8_t), BufferLength);
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Format_Control_Point_Request(qapi_BLE_TRDS_Control_Point_Request_Data_t *RequestData, uint32_t BufferLength, uint8_t *Buffer)
{
   return _qapi_BLE_TRDS_Format_Control_Point_Request(qsTargetId, RequestData, BufferLength, Buffer);
}

int _qapi_BLE_TRDS_Decode_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 2));

    qsSize = qsSize + ((ValueLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_DECODE_CONTROL_POINT_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Value);

        if((qsResult == ssSuccess) && (Value != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Value, sizeof(uint8_t), ValueLength);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ResponseData);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_qapi_BLE_TRDS_Control_Point_Response_Data_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_TRDS_Control_Point_Response_Data_t *)ResponseData);
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Decode_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData)
{
   return _qapi_BLE_TRDS_Decode_Control_Point_Response(qsTargetId, ValueLength, Value, ResponseData);
}

int _qapi_BLE_TRDS_Format_Transport_Block(uint8_t TargetID, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData, uint32_t BufferLength, uint8_t *Buffer)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + CalcPackedSize_qapi_BLE_TRDS_Transport_Block_Data_t((qapi_BLE_TRDS_Transport_Block_Data_t *)TransportBlockData) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_FORMAT_TRANSPORT_BLOCK_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BufferLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)TransportBlockData);

        if((qsResult == ssSuccess) && (TransportBlockData != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_TRDS_Transport_Block_Data_t(&qsInputBuffer, (qapi_BLE_TRDS_Transport_Block_Data_t *)TransportBlockData);
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
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)Buffer, sizeof(uint8_t), BufferLength);
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Format_Transport_Block(qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData, uint32_t BufferLength, uint8_t *Buffer)
{
   return _qapi_BLE_TRDS_Format_Transport_Block(qsTargetId, TransportBlockData, BufferLength, Buffer);
}

int _qapi_BLE_TRDS_Decode_Transport_Block(uint8_t TargetID, uint32_t RemainingLength, uint8_t *Buffer, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData)
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
    int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 2));

    qsSize = qsSize + ((RemainingLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_TRDS_FILE_ID, QAPI_BLE_TRDS_DECODE_TRANSPORT_BLOCK_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&RemainingLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

        if((qsResult == ssSuccess) && (Buffer != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Buffer, sizeof(uint8_t), RemainingLength);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)TransportBlockData);

        if(qsResult == ssSuccess)
        {
            /* Send the command. */
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
                if(qsOutputBuffer.Start != NULL)
                {
                    /* Unpack returned values. */
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_qapi_BLE_TRDS_Transport_Block_Data_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_TRDS_Transport_Block_Data_t *)TransportBlockData);
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int qapi_BLE_TRDS_Decode_Transport_Block(uint32_t RemainingLength, uint8_t *Buffer, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData)
{
   return _qapi_BLE_TRDS_Decode_Transport_Block(qsTargetId, RemainingLength, Buffer, TransportBlockData);
}
