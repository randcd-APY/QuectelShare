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
#include "qapi_twn.h"
#include "qapi_twn_hosted_host_mnl.h"
#include "qapi_twn_hosted_common.h"
#include "qapi_twn_hosted_host_cb.h"
#include "qapi_twn_common.h"

qapi_Status_t _qapi_TWN_Hosted_Start_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface)
{
    return(Mnl_qapi_TWN_Hosted_Start_Border_Agent(TargetID, TWN_Handle, AddressFamily, DisplayName, HostName, Interface));
}

qapi_Status_t qapi_TWN_Hosted_Start_Border_Agent(qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface)
{
   return _qapi_TWN_Hosted_Start_Border_Agent(qsTargetId, TWN_Handle, AddressFamily, DisplayName, HostName, Interface);
}

qapi_Status_t _qapi_TWN_Hosted_Stop_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle)
{
    return(Mnl_qapi_TWN_Hosted_Stop_Border_Agent(TargetID, TWN_Handle));
}

qapi_Status_t qapi_TWN_Hosted_Stop_Border_Agent(qapi_TWN_Handle_t TWN_Handle)
{
   return _qapi_TWN_Hosted_Stop_Border_Agent(qsTargetId, TWN_Handle);
}

qapi_Status_t _qapi_TWN_Hosted_Receive_UDP_Data(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Socket_Info_t *SocketInfo, uint16_t Length, uint8_t *Buffer)
{
    return(Mnl_qapi_TWN_Hosted_Receive_UDP_Data(TargetID, TWN_Handle, SocketInfo, Length, Buffer));
}

qapi_Status_t qapi_TWN_Hosted_Receive_UDP_Data(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Socket_Info_t *SocketInfo, uint16_t Length, uint8_t *Buffer)
{
   return _qapi_TWN_Hosted_Receive_UDP_Data(qsTargetId, TWN_Handle, SocketInfo, Length, Buffer);
}

qapi_Status_t _qapi_TWN_Hosted_Enable_Logging(uint8_t TargetID)
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
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TWN, QAPI_TWN_HOSTED_FILE_ID, QAPI_TWN_HOSTED_ENABLE_LOGGING_FUNCTION_ID, &qsInputBuffer, qsSize))
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

qapi_Status_t qapi_TWN_Hosted_Enable_Logging()
{
   return _qapi_TWN_Hosted_Enable_Logging(qsTargetId);
}

qapi_Status_t _qapi_TWN_Hosted_Disable_Logging(uint8_t TargetID)
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
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TWN, QAPI_TWN_HOSTED_FILE_ID, QAPI_TWN_HOSTED_DISABLE_LOGGING_FUNCTION_ID, &qsInputBuffer, qsSize))
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

qapi_Status_t qapi_TWN_Hosted_Disable_Logging()
{
   return _qapi_TWN_Hosted_Disable_Logging(qsTargetId);
}
