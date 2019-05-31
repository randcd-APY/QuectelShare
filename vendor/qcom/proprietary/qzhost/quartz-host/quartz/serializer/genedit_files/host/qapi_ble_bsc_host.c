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
#include "qapi_ble_bsc_host_mnl.h"
#include "qapi_ble_bsc_common.h"
#include "qapi_ble_bsc_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

int _qapi_BLE_BSC_Initialize(uint8_t TargetID, qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation, uint32_t Flags)
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
    qsSize = (4 + CalcPackedSize_qapi_BLE_HCI_DriverInformation_t((qapi_BLE_HCI_DriverInformation_t *)HCI_DriverInformation) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_INITIALIZE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)HCI_DriverInformation);

        if((qsResult == ssSuccess) && (HCI_DriverInformation != NULL))
        {
            qsResult = PackedWrite_qapi_BLE_HCI_DriverInformation_t(&qsInputBuffer, (qapi_BLE_HCI_DriverInformation_t *)HCI_DriverInformation);
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

int qapi_BLE_BSC_Initialize(qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation, uint32_t Flags)
{
   return _qapi_BLE_BSC_Initialize(qsTargetId, HCI_DriverInformation, Flags);
}

void _qapi_BLE_BSC_Shutdown(uint8_t TargetID, uint32_t BluetoothStackID)
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

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_SHUTDOWN_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                }
                else
                {
                    // Return type is void, no need to set any variables.
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                // Return type is void, no need to set any variables.
            }
        }
        else
        {
            // Return type is void, no need to set any variables.
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        // Return type is void, no need to set any variables.
    }

    FreeBufferList(&qsBufferList);

}

void qapi_BLE_BSC_Shutdown(uint32_t BluetoothStackID)
{
   _qapi_BLE_BSC_Shutdown(qsTargetId, BluetoothStackID);
}

char *_qapi_BLE_BSC_Query_Host_Version(uint8_t TargetID)
{
    return(Mnl_qapi_BLE_BSC_Query_Host_Version(TargetID));
}

char *qapi_BLE_BSC_Query_Host_Version()
{
   return _qapi_BLE_BSC_Query_Host_Version(qsTargetId);
}

int _qapi_BLE_BSC_RegisterDebugCallback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BSC_Debug_Callback_t DebugCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_BSC_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_BSC_DEBUG_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = DebugCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_BSC_Debug_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_REGISTERDEBUGCALLBACK_FUNCTION_ID, &qsInputBuffer, qsSize))
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

int qapi_BLE_BSC_RegisterDebugCallback(uint32_t BluetoothStackID, qapi_BLE_BSC_Debug_Callback_t DebugCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_BSC_RegisterDebugCallback(qsTargetId, BluetoothStackID, DebugCallback, CallbackParameter);
}

void _qapi_BLE_BSC_UnRegisterDebugCallback(uint8_t TargetID, uint32_t BluetoothStackID)
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

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_UNREGISTERDEBUGCALLBACK_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                }
                else
                {
                    // Return type is void, no need to set any variables.
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                // Return type is void, no need to set any variables.
            }
        }
        else
        {
            // Return type is void, no need to set any variables.
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        // Return type is void, no need to set any variables.
    }

    FreeBufferList(&qsBufferList);

}

void qapi_BLE_BSC_UnRegisterDebugCallback(uint32_t BluetoothStackID)
{
   _qapi_BLE_BSC_UnRegisterDebugCallback(qsTargetId, BluetoothStackID);
}

int _qapi_BLE_BSC_LockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID)
{
    return(Mnl_qapi_BLE_BSC_LockBluetoothStack(TargetID, BluetoothStackID));
}

int qapi_BLE_BSC_LockBluetoothStack(uint32_t BluetoothStackID)
{
   return _qapi_BLE_BSC_LockBluetoothStack(qsTargetId, BluetoothStackID);
}

void _qapi_BLE_BSC_UnLockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID)
{
    Mnl_qapi_BLE_BSC_UnLockBluetoothStack(TargetID, BluetoothStackID);
}

void qapi_BLE_BSC_UnLockBluetoothStack(uint32_t BluetoothStackID)
{
   _qapi_BLE_BSC_UnLockBluetoothStack(qsTargetId, BluetoothStackID);
}

int _qapi_BLE_BSC_StartTimer(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Timeout, qapi_BLE_BSC_Timer_Callback_t TimerCallback, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_BSC_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_BSC_TIMER_CALLBACK_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = TimerCallback;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_BSC_Timer_Callback_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 12;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_STARTTIMER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Timeout);

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

int qapi_BLE_BSC_StartTimer(uint32_t BluetoothStackID, uint32_t Timeout, qapi_BLE_BSC_Timer_Callback_t TimerCallback, uint32_t CallbackParameter)
{
   return _qapi_BLE_BSC_StartTimer(qsTargetId, BluetoothStackID, Timeout, TimerCallback, CallbackParameter);
}

int _qapi_BLE_BSC_StopTimer(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TimerID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_STOPTIMER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TimerID);

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
                        Callback_UnregisterByKey(MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_TIMER_CALLBACK_T_CALLBACK_ID, (uint32_t)TimerID);

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

int qapi_BLE_BSC_StopTimer(uint32_t BluetoothStackID, uint32_t TimerID)
{
   return _qapi_BLE_BSC_StopTimer(qsTargetId, BluetoothStackID, TimerID);
}

boolean_t _qapi_BLE_BSC_QueryActiveConnections(uint8_t TargetID, uint32_t BluetoothStackID)
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
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_QUERYACTIVECONNECTIONS_FUNCTION_ID, &qsInputBuffer, qsSize))
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

boolean_t qapi_BLE_BSC_QueryActiveConnections(uint32_t BluetoothStackID)
{
   return _qapi_BLE_BSC_QueryActiveConnections(qsTargetId, BluetoothStackID);
}

int _qapi_BLE_BSC_ScheduleAsynchronousCallback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BSC_AsynchronousCallbackFunction_t AsynchronousCallbackFunction, uint32_t CallbackParameter)
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
    CallbackInfo.FileID = QAPI_BLE_BSC_FILE_ID;
    CallbackInfo.CallbackID = QAPI_BLE_BSC_ASYNCHRONOUSCALLBACKFUNCTION_T_CALLBACK_ID;
    CallbackInfo.CallbackKey = 0;
    CallbackInfo.AppFunction = AsynchronousCallbackFunction;
    CallbackInfo.AppParam = (uint32_t)CallbackParameter;
    qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_BSC_AsynchronousCallbackFunction_t_Handler, &CallbackInfo);

    /* Override the callback parameter with the new one. */
    CallbackParameter = qsCbParam;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_SCHEDULEASYNCHRONOUSCALLBACK_FUNCTION_ID, &qsInputBuffer, qsSize))
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

int qapi_BLE_BSC_ScheduleAsynchronousCallback(uint32_t BluetoothStackID, qapi_BLE_BSC_AsynchronousCallbackFunction_t AsynchronousCallbackFunction, uint32_t CallbackParameter)
{
   return _qapi_BLE_BSC_ScheduleAsynchronousCallback(qsTargetId, BluetoothStackID, AsynchronousCallbackFunction, CallbackParameter);
}

int _qapi_BLE_BSC_GetTxPower(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Connection, int8_t *TxPower)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_GETTXPOWER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Connection);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)TxPower);

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
                        qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)TxPower);
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

int qapi_BLE_BSC_GetTxPower(uint32_t BluetoothStackID, boolean_t Connection, int8_t *TxPower)
{
   return _qapi_BLE_BSC_GetTxPower(qsTargetId, BluetoothStackID, Connection, TxPower);
}

int _qapi_BLE_BSC_SetTxPower(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Connection, int8_t TxPower)
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
    qsSize = 9;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_SETTXPOWER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Connection);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&TxPower);

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

int qapi_BLE_BSC_SetTxPower(uint32_t BluetoothStackID, boolean_t Connection, int8_t TxPower)
{
   return _qapi_BLE_BSC_SetTxPower(qsTargetId, BluetoothStackID, Connection, TxPower);
}

int _qapi_BLE_BSC_Set_FEM_Control_Override(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Enable, uint16_t FEM_Ctrl_0_1, uint16_t FEM_Ctrl_2_3)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_BSC_FILE_ID, QAPI_BLE_BSC_SET_FEM_CONTROL_OVERRIDE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Enable);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&FEM_Ctrl_0_1);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&FEM_Ctrl_2_3);

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

int qapi_BLE_BSC_Set_FEM_Control_Override(uint32_t BluetoothStackID, boolean_t Enable, uint16_t FEM_Ctrl_0_1, uint16_t FEM_Ctrl_2_3)
{
   return _qapi_BLE_BSC_Set_FEM_Control_Override(qsTargetId, BluetoothStackID, Enable, FEM_Ctrl_0_1, FEM_Ctrl_2_3);
}
