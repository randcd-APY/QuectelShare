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
#include "qapi_HERH.h"
#include "qapi_HERH_common.h"
#include "qapi_HERH_common.h"

void _qapi_HEHR_Host_Data(uint8_t TargetID, HERH_Handle_t HERH_Handle, uint32_t DataLength, const uint8_t *Data)
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
    qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((DataLength)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_HERH, QAPI_HERH_FILE_ID, QAPI_HEHR_HOST_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&HERH_Handle);

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

void qapi_HEHR_Host_Data(HERH_Handle_t HERH_Handle, uint32_t DataLength, const uint8_t *Data)
{
   _qapi_HEHR_Host_Data(qsTargetId, HERH_Handle, DataLength, Data);
}
