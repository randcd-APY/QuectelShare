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
#include "qapi_ble_bls_common.h"
#include "qapi_ble_bls_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_blstypes_common.h"

int _qapi_BLE_BLS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID)
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
    CallbackInfo.FileID = QAPI_BLE_BLS_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_BLS_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = EventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_BLS_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_INITIALIZE_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

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

int qapi_BLE_BLS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID)
{
   return _qapi_BLE_BLS_Initialize_Service(qsTargetId, BluetoothStackID, EventCallback, CallbackParameter, ServiceID);
}

int _qapi_BLE_BLS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange)
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
    CallbackInfo.FileID = QAPI_BLE_BLS_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_BLS_EVENT_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = EventCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_BLS_Event_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = (8 + CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t((qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_INITIALIZE_SERVICE_HANDLE_RANGE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

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

int qapi_BLE_BLS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   return _qapi_BLE_BLS_Initialize_Service_Handle_Range(qsTargetId, BluetoothStackID, EventCallback, CallbackParameter, ServiceID, ServiceHandleRange);
}

int _qapi_BLE_BLS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_CLEANUP_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                        Callback_UnregisterByKey(MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_EVENT_CALLBACK_T_CALLBACK_ID, (uint32_t)InstanceID);

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

int qapi_BLE_BLS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID)
{
   return _qapi_BLE_BLS_Cleanup_Service(qsTargetId, BluetoothStackID, InstanceID);
}

unsigned int _qapi_BLE_BLS_Query_Number_Attributes(uint8_t TargetID)
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
    unsigned int qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_QUERY_NUMBER_ATTRIBUTES_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
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

unsigned int qapi_BLE_BLS_Query_Number_Attributes()
{
   return _qapi_BLE_BLS_Query_Number_Attributes(qsTargetId);
}

int _qapi_BLE_BLS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration)
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
    qsSize = 14;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_READ_CLIENT_CONFIGURATION_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&ClientConfiguration);

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

int qapi_BLE_BLS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration)
{
   return _qapi_BLE_BLS_Read_Client_Configuration_Response(qsTargetId, BluetoothStackID, InstanceID, TransactionID, ClientConfiguration);
}

int _qapi_BLE_BLS_Indicate_Blood_Pressure_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Measurement_Data)
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
    qsSize = (12 + CalcPackedSize_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t((qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *)Measurement_Data) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_INDICATE_BLOOD_PRESSURE_MEASUREMENT_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Measurement_Data);

        if((qsResult == ssSuccess) && (Measurement_Data != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(&qsInputBuffer, (qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *)Measurement_Data);
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

int qapi_BLE_BLS_Indicate_Blood_Pressure_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Measurement_Data)
{
   return _qapi_BLE_BLS_Indicate_Blood_Pressure_Measurement(qsTargetId, BluetoothStackID, InstanceID, ConnectionID, Measurement_Data);
}

int _qapi_BLE_BLS_Notify_Intermediate_Cuff_Pressure(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Intermediate_Cuff_Pressure)
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
    qsSize = (12 + CalcPackedSize_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t((qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *)Intermediate_Cuff_Pressure) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_NOTIFY_INTERMEDIATE_CUFF_PRESSURE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Intermediate_Cuff_Pressure);

        if((qsResult == ssSuccess) && (Intermediate_Cuff_Pressure != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(&qsInputBuffer, (qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *)Intermediate_Cuff_Pressure);
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

int qapi_BLE_BLS_Notify_Intermediate_Cuff_Pressure(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Intermediate_Cuff_Pressure)
{
   return _qapi_BLE_BLS_Notify_Intermediate_Cuff_Pressure(qsTargetId, BluetoothStackID, InstanceID, ConnectionID, Intermediate_Cuff_Pressure);
}

int _qapi_BLE_BLS_Set_Blood_Pressure_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_SET_BLOOD_PRESSURE_FEATURE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&SupportedFeatures);

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

int qapi_BLE_BLS_Set_Blood_Pressure_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures)
{
   return _qapi_BLE_BLS_Set_Blood_Pressure_Feature(qsTargetId, BluetoothStackID, InstanceID, SupportedFeatures);
}

int _qapi_BLE_BLS_Query_Blood_Pressure_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_QUERY_BLOOD_PRESSURE_FEATURE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)SupportedFeatures);

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
                        qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, (uint16_t *)SupportedFeatures);
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

int qapi_BLE_BLS_Query_Blood_Pressure_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures)
{
   return _qapi_BLE_BLS_Query_Blood_Pressure_Feature(qsTargetId, BluetoothStackID, InstanceID, SupportedFeatures);
}

int _qapi_BLE_BLS_Decode_Blood_Pressure_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *BloodPressureMeasurement)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BLS_FILE_ID, QAPI_BLE_BLS_DECODE_BLOOD_PRESSURE_MEASUREMENT_FUNCTION_ID, &qsInputBuffer, qsSize))
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
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)BloodPressureMeasurement);

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
                        qsResult = PackedRead_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *)BloodPressureMeasurement);
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

int qapi_BLE_BLS_Decode_Blood_Pressure_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *BloodPressureMeasurement)
{
   return _qapi_BLE_BLS_Decode_Blood_Pressure_Measurement(qsTargetId, ValueLength, Value, BloodPressureMeasurement);
}
