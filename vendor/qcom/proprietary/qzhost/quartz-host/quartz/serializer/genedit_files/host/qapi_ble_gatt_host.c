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
#include "qapi_ble_gatt_host_mnl.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_gatt_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_atttypes_common.h"

int _qapi_BLE_GATT_Initialize(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CONNECTION_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ConnectionEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Connection_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 12;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_INITIALIZE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Initialize(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Initialize(qsTargetId, BluetoothStackID, Flags, ConnectionEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Cleanup(uint8_t TargetID, uint32_t BluetoothStackID)
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
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_CLEANUP_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

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
                        Callback_UnregisterByKey(MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_CONNECTION_EVENT_CALLBACK_T_CALLBACK_ID, (uint32_t)0);

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

int qapi_BLE_GATT_Cleanup(uint32_t BluetoothStackID)
{
   return _qapi_BLE_GATT_Cleanup(qsTargetId, BluetoothStackID);
}

int _qapi_BLE_GATT_Register_Connection_Events(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CONNECTION_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ConnectionEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Connection_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_REGISTER_CONNECTION_EVENTS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Register_Connection_Events(uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Register_Connection_Events(qsTargetId, BluetoothStackID, ConnectionEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Un_Register_Connection_Events(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t EventCallbackID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_UN_REGISTER_CONNECTION_EVENTS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&EventCallbackID);

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
                        Callback_UnregisterByKey(MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_CONNECTION_EVENT_CALLBACK_T_CALLBACK_ID, (uint32_t)EventCallbackID);

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

int qapi_BLE_GATT_Un_Register_Connection_Events(uint32_t BluetoothStackID, uint32_t EventCallbackID)
{
   return _qapi_BLE_GATT_Un_Register_Connection_Events(qsTargetId, BluetoothStackID, EventCallbackID);
}

int _qapi_BLE_GATT_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, uint32_t NumberOfServiceAttributeEntries, qapi_BLE_GATT_Service_Attribute_Entry_t *ServiceTable, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, qapi_BLE_GATT_Server_Event_Callback_t ServerEventCallback, uint32_t CallbackParameter)
{
    return(Mnl_qapi_BLE_GATT_Register_Service(TargetID, BluetoothStackID, ServiceFlags, NumberOfServiceAttributeEntries, ServiceTable, ServiceHandleGroupResult, ServerEventCallback, CallbackParameter));
}

int qapi_BLE_GATT_Register_Service(uint32_t BluetoothStackID, uint8_t ServiceFlags, uint32_t NumberOfServiceAttributeEntries, qapi_BLE_GATT_Service_Attribute_Entry_t *ServiceTable, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, qapi_BLE_GATT_Server_Event_Callback_t ServerEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Register_Service(qsTargetId, BluetoothStackID, ServiceFlags, NumberOfServiceAttributeEntries, ServiceTable, ServiceHandleGroupResult, ServerEventCallback, CallbackParameter);
}

void _qapi_BLE_GATT_Un_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID)
{
    Mnl_qapi_BLE_GATT_Un_Register_Service(TargetID, BluetoothStackID, ServiceID);
}

void qapi_BLE_GATT_Un_Register_Service(uint32_t BluetoothStackID, uint32_t ServiceID)
{
   _qapi_BLE_GATT_Un_Register_Service(qsTargetId, BluetoothStackID, ServiceID);
}

int _qapi_BLE_GATT_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint32_t DataLength, uint8_t *Data)
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
    qsSize = (12 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((DataLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_READ_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Data);

        if((qsResult == ssSuccess) && (Data != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Data, sizeof(uint8_t), DataLength);
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

int qapi_BLE_GATT_Read_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint32_t DataLength, uint8_t *Data)
{
   return _qapi_BLE_GATT_Read_Response(qsTargetId, BluetoothStackID, TransactionID, DataLength, Data);
}

int _qapi_BLE_GATT_Write_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_WRITE_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

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

int qapi_BLE_GATT_Write_Response(uint32_t BluetoothStackID, uint32_t TransactionID)
{
   return _qapi_BLE_GATT_Write_Response(qsTargetId, BluetoothStackID, TransactionID);
}

int _qapi_BLE_GATT_Execute_Write_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_EXECUTE_WRITE_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

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

int qapi_BLE_GATT_Execute_Write_Response(uint32_t BluetoothStackID, uint32_t TransactionID)
{
   return _qapi_BLE_GATT_Execute_Write_Response(qsTargetId, BluetoothStackID, TransactionID);
}

int _qapi_BLE_GATT_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t AttributeOffset, uint8_t ErrorCode)
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
    qsSize = 11;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_ERROR_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeOffset);

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

int qapi_BLE_GATT_Error_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t AttributeOffset, uint8_t ErrorCode)
{
   return _qapi_BLE_GATT_Error_Response(qsTargetId, BluetoothStackID, TransactionID, AttributeOffset, ErrorCode);
}

int _qapi_BLE_GATT_Handle_Value_Indication(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue)
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
    qsSize = (16 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((AttributeValueLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_HANDLE_VALUE_INDICATION_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, sizeof(uint8_t), AttributeValueLength);
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

int qapi_BLE_GATT_Handle_Value_Indication(uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue)
{
   return _qapi_BLE_GATT_Handle_Value_Indication(qsTargetId, BluetoothStackID, ServiceID, ConnectionID, AttributeOffset, AttributeValueLength, AttributeValue);
}

int _qapi_BLE_GATT_Handle_Value_Notification(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue)
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
    qsSize = (16 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((AttributeValueLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_HANDLE_VALUE_NOTIFICATION_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, sizeof(uint8_t), AttributeValueLength);
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

int qapi_BLE_GATT_Handle_Value_Notification(uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue)
{
   return _qapi_BLE_GATT_Handle_Value_Notification(qsTargetId, BluetoothStackID, ServiceID, ConnectionID, AttributeOffset, AttributeValueLength, AttributeValue);
}

boolean_t _qapi_BLE_GATT_Verify_Signature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint16_t AttributeOffset, uint32_t AttributeValueLength, uint8_t *AttributeValue, qapi_BLE_ATT_Authentication_Signature_t *ReceivedSignature, qapi_BLE_Encryption_Key_t *CSRK)
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
    boolean_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (14 + CalcPackedSize_qapi_BLE_ATT_Authentication_Signature_t((qapi_BLE_ATT_Authentication_Signature_t *)ReceivedSignature) + CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)CSRK) + (QS_POINTER_HEADER_SIZE * 3));

    qsSize = qsSize + ((AttributeValueLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_VERIFY_SIGNATURE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, sizeof(uint8_t), AttributeValueLength);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ReceivedSignature);

        if((qsResult == ssSuccess) && (ReceivedSignature != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_ATT_Authentication_Signature_t(&qsInputBuffer, (qapi_BLE_ATT_Authentication_Signature_t *)ReceivedSignature);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CSRK);

        if((qsResult == ssSuccess) && (CSRK != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(&qsInputBuffer, (qapi_BLE_Encryption_Key_t *)CSRK);
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
                        qsRetVal = FALSE;
                    }
                }
                else
                {
                    qsRetVal = FALSE;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = FALSE;
            }
        }
        else
        {
            qsRetVal = FALSE;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = FALSE;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

boolean_t qapi_BLE_GATT_Verify_Signature(uint32_t BluetoothStackID, uint32_t ServiceID, uint16_t AttributeOffset, uint32_t AttributeValueLength, uint8_t *AttributeValue, qapi_BLE_ATT_Authentication_Signature_t *ReceivedSignature, qapi_BLE_Encryption_Key_t *CSRK)
{
   return _qapi_BLE_GATT_Verify_Signature(qsTargetId, BluetoothStackID, ServiceID, AttributeOffset, AttributeValueLength, AttributeValue, ReceivedSignature, CSRK);
}

int _qapi_BLE_GATT_Service_Changed_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data)
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
    qsSize = (8 + CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t((qapi_BLE_GATT_Service_Changed_Data_t *)Service_Changed_Data) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_SERVICE_CHANGED_READ_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Service_Changed_Data);

        if((qsResult == ssSuccess) && (Service_Changed_Data != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_Service_Changed_Data_t(&qsInputBuffer, (qapi_BLE_GATT_Service_Changed_Data_t *)Service_Changed_Data);
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

int qapi_BLE_GATT_Service_Changed_Read_Response(uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data)
{
   return _qapi_BLE_GATT_Service_Changed_Read_Response(qsTargetId, BluetoothStackID, TransactionID, Service_Changed_Data);
}

int _qapi_BLE_GATT_Service_Changed_CCCD_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t CCCD)
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
    qsSize = 10;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_SERVICE_CHANGED_CCCD_READ_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&CCCD);

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

int qapi_BLE_GATT_Service_Changed_CCCD_Read_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t CCCD)
{
   return _qapi_BLE_GATT_Service_Changed_CCCD_Read_Response(qsTargetId, BluetoothStackID, TransactionID, CCCD);
}

int _qapi_BLE_GATT_Service_Changed_Indication(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data)
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
    qsSize = (8 + CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t((qapi_BLE_GATT_Service_Changed_Data_t *)Service_Changed_Data) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_SERVICE_CHANGED_INDICATION_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Service_Changed_Data);

        if((qsResult == ssSuccess) && (Service_Changed_Data != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_Service_Changed_Data_t(&qsInputBuffer, (qapi_BLE_GATT_Service_Changed_Data_t *)Service_Changed_Data);
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

int qapi_BLE_GATT_Service_Changed_Indication(uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data)
{
   return _qapi_BLE_GATT_Service_Changed_Indication(qsTargetId, BluetoothStackID, ConnectionID, Service_Changed_Data);
}

int _qapi_BLE_GATT_Exchange_MTU_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t RequestedMTU, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 14;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_EXCHANGE_MTU_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&RequestedMTU);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Exchange_MTU_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t RequestedMTU, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Exchange_MTU_Request(qsTargetId, BluetoothStackID, ConnectionID, RequestedMTU, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Discover_Services(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_DISCOVER_SERVICES_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&StartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&EndingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Discover_Services(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Discover_Services(qsTargetId, BluetoothStackID, ConnectionID, StartingHandle, EndingHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Discover_Services_By_UUID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_UUID_t *UUID, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (16 + CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)UUID) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_DISCOVER_SERVICES_BY_UUID_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&StartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&EndingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UUID);

        if((qsResult == ssSuccess) && (UUID != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, (qapi_BLE_GATT_UUID_t *)UUID);
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

int qapi_BLE_GATT_Discover_Services_By_UUID(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_UUID_t *UUID, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Discover_Services_By_UUID(qsTargetId, BluetoothStackID, ConnectionID, StartingHandle, EndingHandle, UUID, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Discover_Included_Services(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_DISCOVER_INCLUDED_SERVICES_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceStartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceEndingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Discover_Included_Services(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Discover_Included_Services(qsTargetId, BluetoothStackID, ConnectionID, ServiceStartingHandle, ServiceEndingHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Discover_Characteristics(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_DISCOVER_CHARACTERISTICS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceStartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceEndingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Discover_Characteristics(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Discover_Characteristics(qsTargetId, BluetoothStackID, ConnectionID, ServiceStartingHandle, ServiceEndingHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Discover_Characteristic_Descriptors(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t CharacteristicStartingHandle, uint16_t CharacteristicEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&CharacteristicStartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&CharacteristicEndingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Discover_Characteristic_Descriptors(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t CharacteristicStartingHandle, uint16_t CharacteristicEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Discover_Characteristic_Descriptors(qsTargetId, BluetoothStackID, ConnectionID, CharacteristicStartingHandle, CharacteristicEndingHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Read_Value_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 14;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_READ_VALUE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Read_Value_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Read_Value_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Read_Long_Value_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeOffset, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_READ_LONG_VALUE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Read_Long_Value_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeOffset, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Read_Long_Value_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, AttributeOffset, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Read_Value_By_UUID_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_UUID_t *AttributeUUID, uint16_t ServiceStartHandle, uint16_t ServiceEndHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (16 + CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)AttributeUUID) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_READ_VALUE_BY_UUID_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceStartHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ServiceEndHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeUUID);

        if((qsResult == ssSuccess) && (AttributeUUID != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, (qapi_BLE_GATT_UUID_t *)AttributeUUID);
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

int qapi_BLE_GATT_Read_Value_By_UUID_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_UUID_t *AttributeUUID, uint16_t ServiceStartHandle, uint16_t ServiceEndHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Read_Value_By_UUID_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeUUID, ServiceStartHandle, ServiceEndHandle, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Read_Multiple_Values_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t NumberOfHandles, uint16_t *AttributeHandleList, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (14 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((NumberOfHandles)*(2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_READ_MULTIPLE_VALUES_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&NumberOfHandles);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeHandleList);

        if((qsResult == ssSuccess) && (AttributeHandleList != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeHandleList, sizeof(uint16_t), NumberOfHandles);
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

int qapi_BLE_GATT_Read_Multiple_Values_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t NumberOfHandles, uint16_t *AttributeHandleList, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Read_Multiple_Values_Request(qsTargetId, BluetoothStackID, ConnectionID, NumberOfHandles, AttributeHandleList, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (16 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((AttributeLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_WRITE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, 1, AttributeLength);
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

int qapi_BLE_GATT_Write_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Write_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, AttributeLength, AttributeValue, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Write_Without_Response_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue)
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
    qsSize = (12 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((AttributeLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_WRITE_WITHOUT_RESPONSE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, 1, AttributeLength);
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

int qapi_BLE_GATT_Write_Without_Response_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue)
{
   return _qapi_BLE_GATT_Write_Without_Response_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, AttributeLength, AttributeValue);
}

int _qapi_BLE_GATT_Signed_Write_Without_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint32_t SignCounter, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_Encryption_Key_t *CSRK)
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
    qsSize = (16 + CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)CSRK) + (QS_POINTER_HEADER_SIZE * 2));

    qsSize = qsSize + ((AttributeLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_SIGNED_WRITE_WITHOUT_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&SignCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, 1, AttributeLength);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CSRK);

        if((qsResult == ssSuccess) && (CSRK != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(&qsInputBuffer, (qapi_BLE_Encryption_Key_t *)CSRK);
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

int qapi_BLE_GATT_Signed_Write_Without_Response(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint32_t SignCounter, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_Encryption_Key_t *CSRK)
{
   return _qapi_BLE_GATT_Signed_Write_Without_Response(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, SignCounter, AttributeLength, AttributeValue, CSRK);
}

int _qapi_BLE_GATT_Prepare_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, uint16_t AttributeValueOffset, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (18 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((AttributeLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_PREPARE_WRITE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeLength);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&AttributeValueOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)AttributeValue);

        if((qsResult == ssSuccess) && (AttributeValue != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)AttributeValue, 1, AttributeLength);
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

int qapi_BLE_GATT_Prepare_Write_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, uint16_t AttributeValueOffset, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Prepare_Write_Request(qsTargetId, BluetoothStackID, ConnectionID, AttributeHandle, AttributeLength, AttributeValueOffset, AttributeValue, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Execute_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, boolean_t CancelWrite, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_CLIENT_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ClientEventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Client_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_EXECUTE_WRITE_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CancelWrite);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

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

int qapi_BLE_GATT_Execute_Write_Request(uint32_t BluetoothStackID, uint32_t ConnectionID, boolean_t CancelWrite, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Execute_Write_Request(qsTargetId, BluetoothStackID, ConnectionID, CancelWrite, ClientEventCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Handle_Value_Confirmation(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t TransactionID)
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
    qsSize = 12;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_HANDLE_VALUE_CONFIRMATION_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

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

int qapi_BLE_GATT_Handle_Value_Confirmation(uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t TransactionID)
{
   return _qapi_BLE_GATT_Handle_Value_Confirmation(qsTargetId, BluetoothStackID, ConnectionID, TransactionID);
}

int _qapi_BLE_GATT_Start_Service_Discovery(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_SERVICE_DISCOVERY_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ServiceDiscoveryCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Service_Discovery_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (16 + (QS_POINTER_HEADER_SIZE * 1));

    for (qsIndex = 0; qsIndex < NumberOfUUID; qsIndex++)
    {
        qsSize = qsSize + CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&UUIDList[qsIndex]);
    }

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_START_SERVICE_DISCOVERY_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&NumberOfUUID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UUIDList);

        if((qsResult == ssSuccess) && (UUIDList != NULL))
        {
            for (qsIndex = 0; qsIndex < NumberOfUUID; qsIndex++)
            {
                if(qsResult == ssSuccess)
                    qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, &((qapi_BLE_GATT_UUID_t *)UUIDList)[qsIndex]);
            }
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

int qapi_BLE_GATT_Start_Service_Discovery(uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Start_Service_Discovery(qsTargetId, BluetoothStackID, ConnectionID, NumberOfUUID, UUIDList, ServiceDiscoveryCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Start_Service_Discovery_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Attribute_Handle_Group_t *DiscoveryHandleRange, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_GATT_SERVICE_DISCOVERY_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = ServiceDiscoveryCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Service_Discovery_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (16 + CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t((qapi_BLE_GATT_Attribute_Handle_Group_t *)DiscoveryHandleRange) + (QS_POINTER_HEADER_SIZE * 2));

    for (qsIndex = 0; qsIndex < NumberOfUUID; qsIndex++)
    {
        qsSize = qsSize + CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&UUIDList[qsIndex]);
    }

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_START_SERVICE_DISCOVERY_HANDLE_RANGE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&NumberOfUUID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)DiscoveryHandleRange);

        if((qsResult == ssSuccess) && (DiscoveryHandleRange != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsInputBuffer, (qapi_BLE_GATT_Attribute_Handle_Group_t *)DiscoveryHandleRange);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UUIDList);

        if((qsResult == ssSuccess) && (UUIDList != NULL))
        {
            for (qsIndex = 0; qsIndex < NumberOfUUID; qsIndex++)
            {
                if(qsResult == ssSuccess)
                    qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, &((qapi_BLE_GATT_UUID_t *)UUIDList)[qsIndex]);
            }
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

int qapi_BLE_GATT_Start_Service_Discovery_Handle_Range(uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Attribute_Handle_Group_t *DiscoveryHandleRange, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_GATT_Start_Service_Discovery_Handle_Range(qsTargetId, BluetoothStackID, ConnectionID, DiscoveryHandleRange, NumberOfUUID, UUIDList, ServiceDiscoveryCallback, CallbackParameter);
}

int _qapi_BLE_GATT_Stop_Service_Discovery(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_STOP_SERVICE_DISCOVERY_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

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

int qapi_BLE_GATT_Stop_Service_Discovery(uint32_t BluetoothStackID, uint32_t ConnectionID)
{
   return _qapi_BLE_GATT_Stop_Service_Discovery(qsTargetId, BluetoothStackID, ConnectionID);
}

int _qapi_BLE_GATT_Cancel_Transaction(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_CANCEL_TRANSACTION_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

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

int qapi_BLE_GATT_Cancel_Transaction(uint32_t BluetoothStackID, uint32_t TransactionID)
{
   return _qapi_BLE_GATT_Cancel_Transaction(qsTargetId, BluetoothStackID, TransactionID);
}

int _qapi_BLE_GATT_Query_Maximum_Supported_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t *MTU)
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
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_QUERY_MAXIMUM_SUPPORTED_MTU_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)MTU);

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
                        qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, (uint16_t *)MTU);
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

int qapi_BLE_GATT_Query_Maximum_Supported_MTU(uint32_t BluetoothStackID, uint16_t *MTU)
{
   return _qapi_BLE_GATT_Query_Maximum_Supported_MTU(qsTargetId, BluetoothStackID, MTU);
}

int _qapi_BLE_GATT_Change_Maximum_Supported_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t MTU)
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
    qsSize = 6;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_CHANGE_MAXIMUM_SUPPORTED_MTU_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&MTU);

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

int qapi_BLE_GATT_Change_Maximum_Supported_MTU(uint32_t BluetoothStackID, uint16_t MTU)
{
   return _qapi_BLE_GATT_Change_Maximum_Supported_MTU(qsTargetId, BluetoothStackID, MTU);
}

int _qapi_BLE_GATT_Query_Connection_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t *MTU)
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
    qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_QUERY_CONNECTION_MTU_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)MTU);

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
                        qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, (uint16_t *)MTU);
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

int qapi_BLE_GATT_Query_Connection_MTU(uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t *MTU)
{
   return _qapi_BLE_GATT_Query_Connection_MTU(qsTargetId, BluetoothStackID, ConnectionID, MTU);
}

int _qapi_BLE_GATT_Query_Connection_ID(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Type_t ConnectionType, qapi_BLE_BD_ADDR_t BD_ADDR, uint32_t *ConnectionID)
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
    qsSize = (8 + CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&BD_ADDR) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_QUERY_CONNECTION_ID_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(&qsInputBuffer, (int *)&ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(&qsInputBuffer, (qapi_BLE_BD_ADDR_t *)&BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ConnectionID);

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
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)ConnectionID);
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

int qapi_BLE_GATT_Query_Connection_ID(uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Type_t ConnectionType, qapi_BLE_BD_ADDR_t BD_ADDR, uint32_t *ConnectionID)
{
   return _qapi_BLE_GATT_Query_Connection_ID(qsTargetId, BluetoothStackID, ConnectionType, BD_ADDR, ConnectionID);
}

int _qapi_BLE_GATT_Query_Transaction_Opcode(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t *Opcode)
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
    qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_QUERY_TRANSACTION_OPCODE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Opcode);

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
                        qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)Opcode);
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

int qapi_BLE_GATT_Query_Transaction_Opcode(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t *Opcode)
{
   return _qapi_BLE_GATT_Query_Transaction_Opcode(qsTargetId, BluetoothStackID, TransactionID, Opcode);
}

int _qapi_BLE_GATT_Set_Queuing_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t MaximumNumberDataPackets, uint32_t QueuedDataPacketsThreshold, boolean_t DiscardOldest)
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
    qsSize = 16;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_SET_QUEUING_PARAMETERS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&MaximumNumberDataPackets);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&QueuedDataPacketsThreshold);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&DiscardOldest);

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

int qapi_BLE_GATT_Set_Queuing_Parameters(uint32_t BluetoothStackID, uint32_t MaximumNumberDataPackets, uint32_t QueuedDataPacketsThreshold, boolean_t DiscardOldest)
{
   return _qapi_BLE_GATT_Set_Queuing_Parameters(qsTargetId, BluetoothStackID, MaximumNumberDataPackets, QueuedDataPacketsThreshold, DiscardOldest);
}

int _qapi_BLE_GATT_Get_Queuing_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *MaximumNumberDataPackets, uint32_t *QueuedDataPacketsThreshold, boolean_t *DiscardOldest)
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
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 3));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_GET_QUEUING_PARAMETERS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)MaximumNumberDataPackets);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)QueuedDataPacketsThreshold);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)DiscardOldest);

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
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)MaximumNumberDataPackets);
                    }

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)QueuedDataPacketsThreshold);
                    }

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)DiscardOldest);
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

int qapi_BLE_GATT_Get_Queuing_Parameters(uint32_t BluetoothStackID, uint32_t *MaximumNumberDataPackets, uint32_t *QueuedDataPacketsThreshold, boolean_t *DiscardOldest)
{
   return _qapi_BLE_GATT_Get_Queuing_Parameters(qsTargetId, BluetoothStackID, MaximumNumberDataPackets, QueuedDataPacketsThreshold, DiscardOldest);
}

boolean_t _qapi_BLE_GATT_Query_Service_Range_Availability(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroup)
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
    boolean_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_QUERY_SERVICE_RANGE_AVAILABILITY_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceHandleGroup);

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
                        qsResult = PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleGroup);
                    }

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = FALSE;
                    }
                }
                else
                {
                    qsRetVal = FALSE;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = FALSE;
            }
        }
        else
        {
            qsRetVal = FALSE;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = FALSE;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

boolean_t qapi_BLE_GATT_Query_Service_Range_Availability(uint32_t BluetoothStackID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroup)
{
   return _qapi_BLE_GATT_Query_Service_Range_Availability(qsTargetId, BluetoothStackID, ServiceHandleGroup);
}
