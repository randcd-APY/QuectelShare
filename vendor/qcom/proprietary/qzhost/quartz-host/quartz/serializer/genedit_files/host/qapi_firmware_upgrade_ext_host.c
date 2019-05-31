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
#include "qapi_firmware_upgrade_ext.h"
#include "qapi_firmware_upgrade_ext_common.h"

uint8_t _qapi_Fw_Upgrade_Get_Active_FWD(uint8_t TargetID, uint32_t *fwd_boot_type, uint32_t *valid_fwd)
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
    uint8_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = ((QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_GET_ACTIVE_FWD_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)fwd_boot_type);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)valid_fwd);

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

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)fwd_boot_type);
                    }

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)valid_fwd);
                    }

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

uint8_t qapi_Fw_Upgrade_Get_Active_FWD(uint32_t *fwd_boot_type, uint32_t *valid_fwd)
{
   return _qapi_Fw_Upgrade_Get_Active_FWD(qsTargetId, fwd_boot_type, valid_fwd);
}

qapi_Status_t _qapi_Fw_Upgrade_Reboot_System(uint8_t TargetID)
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

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_REBOOT_SYSTEM_FUNCTION_ID, &qsInputBuffer, qsSize))
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

qapi_Status_t qapi_Fw_Upgrade_Reboot_System()
{
   return _qapi_Fw_Upgrade_Reboot_System(qsTargetId);
}

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Done(uint8_t TargetID, uint32_t result, uint32_t flags)
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
    qapi_Fw_Upgrade_Status_Code_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 8;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_DONE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&result);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&flags);

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
                        qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Done(uint32_t result, uint32_t flags)
{
   return _qapi_Fw_Upgrade_Done(qsTargetId, result, flags);
}

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Init(uint8_t TargetID, uint32_t flags)
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
    qapi_Fw_Upgrade_Status_Code_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_HOST_INIT_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&flags);

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
                        qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Init(uint32_t flags)
{
   return _qapi_Fw_Upgrade_Host_Init(qsTargetId, flags);
}

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Deinit(uint8_t TargetID)
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
    qapi_Fw_Upgrade_Status_Code_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_HOST_DEINIT_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                        qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Deinit()
{
   return _qapi_Fw_Upgrade_Host_Deinit(qsTargetId);
}

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Write(uint8_t TargetID, char *buffer, int32_t len)
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
    qapi_Fw_Upgrade_Status_Code_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = (4 + (QS_POINTER_HEADER_SIZE * 1));

    qsSize = qsSize + ((len)*(1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_HOST_WRITE_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&len);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)buffer);

        if((qsResult == ssSuccess) && (buffer != NULL))
        {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)buffer, sizeof(char), len);
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
                        qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Write(char *buffer, int32_t len)
{
   return _qapi_Fw_Upgrade_Host_Write(qsTargetId, buffer, len);
}

qapi_Fw_Upgrade_State_t _qapi_Fw_Upgrade_Get_State(uint8_t TargetID)
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
    qapi_Fw_Upgrade_State_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_GET_STATE_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                        qsRetVal = QAPI_FW_UPGRADE_STATE_NOT_START_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_STATE_NOT_START_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_STATE_NOT_START_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_STATE_NOT_START_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_STATE_NOT_START_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_State_t qapi_Fw_Upgrade_Get_State()
{
   return _qapi_Fw_Upgrade_Get_State(qsTargetId);
}

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Get_Status(uint8_t TargetID)
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
    qapi_Fw_Upgrade_Status_Code_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 0;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_FW_UPGRADE_GET_STATUS_FUNCTION_ID, &qsInputBuffer, qsSize))
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
                        qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
            }
        }
        else
        {
            qsRetVal = QAPI_FW_UPGRADE_ERROR_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Get_Status()
{
   return _qapi_Fw_Upgrade_Get_Status(qsTargetId);
}

uint32_t _qapi_Get_FW_Ver(uint8_t TargetID, qapi_FW_Ver_t *ver)
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
    qsSize = ((QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_FIRMWARE_UPGRADE_EXT, QAPI_FIRMWARE_UPGRADE_EXT_FILE_ID, QAPI_GET_FW_VER_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ver);

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
                        qsResult = PackedRead_qapi_FW_Ver_t(&qsOutputBuffer, &qsBufferList, (qapi_FW_Ver_t *)ver);
                    }

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

uint32_t qapi_Get_FW_Ver(qapi_FW_Ver_t *ver)
{
   return _qapi_Get_FW_Ver(qsTargetId, ver);
}
