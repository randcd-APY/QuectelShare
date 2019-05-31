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
#include "qapi_wlan.h"
#include "qapi_wlan_prof_common.h"

int8_t _qapi_Prof_Cmd_Handler(uint8_t TargetID, qapi_Prof_Cmd_t cmd, qapi_Prof_Stats_t *data)
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
    int8_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + CalcPackedSize_qapi_Prof_Stats_t((qapi_Prof_Stats_t *)data) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_WLAN, QAPI_WLAN_PROF_FILE_ID, QAPI_PROF_CMD_HANDLER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(&qsInputBuffer, (int *)&cmd);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)data);

        if((qsResult == ssSuccess) && (data != NULL))
        {
            qsResult = PackedWrite_qapi_Prof_Stats_t(&qsInputBuffer, (qapi_Prof_Stats_t *)data);
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
                        qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)&qsRetVal);

                    /* Set the return value to error if necessary. */
                    if(qsResult != ssSuccess)
                    {
                        qsRetVal = QAPI_WLAN_ERROR;
                    }
                }
                else
                {
                    qsRetVal = QAPI_WLAN_ERROR;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_WLAN_ERROR;
            }
        }
        else
        {
            qsRetVal = QAPI_WLAN_ERROR;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_WLAN_ERR_NO_MEMORY;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

int8_t qapi_Prof_Cmd_Handler(qapi_Prof_Cmd_t cmd, qapi_Prof_Stats_t *data)
{
   return _qapi_Prof_Cmd_Handler(qsTargetId, cmd, data);
}
