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
#include "qapi_tlmm.h"
#include "qapi_tlmm_common.h"

qapi_Status_t _qapi_TLMM_Get_Gpio_ID(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t *qapi_GPIO_ID)
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
    qsSize = (CalcPackedSize_qapi_TLMM_Config_t((qapi_TLMM_Config_t *)qapi_TLMM_Config) + CalcPackedSize_16((uint16_t *)qapi_GPIO_ID) + (QS_POINTER_HEADER_SIZE * 2));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_GET_GPIO_ID_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)qapi_GPIO_ID);

        if((qsResult == ssSuccess) && (qapi_GPIO_ID != NULL))
        {
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)qapi_GPIO_ID);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)qapi_TLMM_Config);

        if((qsResult == ssSuccess) && (qapi_TLMM_Config != NULL))
        {
            qsResult = PackedWrite_qapi_TLMM_Config_t(&qsInputBuffer, (qapi_TLMM_Config_t *)qapi_TLMM_Config);
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
                        qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, (uint16_t *)qapi_GPIO_ID);
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

qapi_Status_t qapi_TLMM_Get_Gpio_ID(qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t *qapi_GPIO_ID)
{
   return _qapi_TLMM_Get_Gpio_ID(qsTargetId, qapi_TLMM_Config, qapi_GPIO_ID);
}

qapi_Status_t _qapi_TLMM_Release_Gpio_ID(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t qapi_GPIO_ID)
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
    qsSize = (2 + CalcPackedSize_qapi_TLMM_Config_t((qapi_TLMM_Config_t *)qapi_TLMM_Config) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_RELEASE_GPIO_ID_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&qapi_GPIO_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)qapi_TLMM_Config);

        if((qsResult == ssSuccess) && (qapi_TLMM_Config != NULL))
        {
            qsResult = PackedWrite_qapi_TLMM_Config_t(&qsInputBuffer, (qapi_TLMM_Config_t *)qapi_TLMM_Config);
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

qapi_Status_t qapi_TLMM_Release_Gpio_ID(qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t qapi_GPIO_ID)
{
   return _qapi_TLMM_Release_Gpio_ID(qsTargetId, qapi_TLMM_Config, qapi_GPIO_ID);
}

qapi_Status_t _qapi_TLMM_Config_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, qapi_TLMM_Config_t *qapi_TLMM_Config)
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
    qsSize = (2 + CalcPackedSize_qapi_TLMM_Config_t((qapi_TLMM_Config_t *)qapi_TLMM_Config) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_CONFIG_GPIO_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&qapi_GPIO_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)qapi_TLMM_Config);

        if((qsResult == ssSuccess) && (qapi_TLMM_Config != NULL))
        {
            qsResult = PackedWrite_qapi_TLMM_Config_t(&qsInputBuffer, (qapi_TLMM_Config_t *)qapi_TLMM_Config);
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

qapi_Status_t qapi_TLMM_Config_Gpio(qapi_GPIO_ID_t qapi_GPIO_ID, qapi_TLMM_Config_t *qapi_TLMM_Config)
{
   return _qapi_TLMM_Config_Gpio(qsTargetId, qapi_GPIO_ID, qapi_TLMM_Config);
}

qapi_Status_t _qapi_TLMM_Drive_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin, qapi_GPIO_Value_t value)
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
    qsSize = 10;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_DRIVE_GPIO_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&qapi_GPIO_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&pin);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(&qsInputBuffer, (int *)&value);

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

qapi_Status_t qapi_TLMM_Drive_Gpio(qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin, qapi_GPIO_Value_t value)
{
   return _qapi_TLMM_Drive_Gpio(qsTargetId, qapi_GPIO_ID, pin, value);
}

qapi_GPIO_Value_t _qapi_TLMM_Read_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin)
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
    qapi_GPIO_Value_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 6;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_READ_GPIO_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&qapi_GPIO_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&pin);

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
                        qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
            }
        }
        else
        {
            qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_GPIO_Value_t qapi_TLMM_Read_Gpio(qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin)
{
   return _qapi_TLMM_Read_Gpio(qsTargetId, qapi_GPIO_ID, pin);
}

qapi_GPIO_Value_t _qapi_TLMM_Get_Gpio_Outval(uint8_t TargetID, uint32_t pin)
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
    qapi_GPIO_Value_t qsRetVal = 0;

    /* Calculate size of packed function arguments. */
    qsSize = 4;

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_GET_GPIO_OUTVAL_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&pin);

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
                        qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
                    }
                }
                else
                {
                    qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
                }

                /* Free the output buffer. */
                FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
                qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
            }
        }
        else
        {
            qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
        }

        /* Free the input buffer. */
        FreePackedBuffer(&qsInputBuffer);
    }
    else
    {
        qsRetVal = QAPI_GPIO_INVALID_VALUE_E;
    }

    FreeBufferList(&qsBufferList);

    return(qsRetVal);
}

qapi_GPIO_Value_t qapi_TLMM_Get_Gpio_Outval(uint32_t pin)
{
   return _qapi_TLMM_Get_Gpio_Outval(qsTargetId, pin);
}

qapi_Status_t _qapi_TLMM_Get_Gpio_Config(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config)
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
    qsSize = (CalcPackedSize_qapi_TLMM_Config_t((qapi_TLMM_Config_t *)qapi_TLMM_Config) + (QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_GET_GPIO_CONFIG_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)qapi_TLMM_Config);

        if((qsResult == ssSuccess) && (qapi_TLMM_Config != NULL))
        {
            qsResult = PackedWrite_qapi_TLMM_Config_t(&qsInputBuffer, (qapi_TLMM_Config_t *)qapi_TLMM_Config);
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
                        qsResult = PackedRead_qapi_TLMM_Config_t(&qsOutputBuffer, &qsBufferList, (qapi_TLMM_Config_t *)qapi_TLMM_Config);
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

qapi_Status_t qapi_TLMM_Get_Gpio_Config(qapi_TLMM_Config_t *qapi_TLMM_Config)
{
   return _qapi_TLMM_Get_Gpio_Config(qsTargetId, qapi_TLMM_Config);
}

qapi_Status_t _qapi_TLMM_Get_Total_Gpios(uint8_t TargetID, uint32_t *total_GPIOs)
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
    qsSize = ((QS_POINTER_HEADER_SIZE * 1));

    if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_TLMM, QAPI_TLMM_FILE_ID, QAPI_TLMM_GET_TOTAL_GPIOS_FUNCTION_ID, &qsInputBuffer, qsSize))
    {
        /* Write arguments packed. */
        if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)total_GPIOs);

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
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)total_GPIOs);
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

qapi_Status_t qapi_TLMM_Get_Total_Gpios(uint32_t *total_GPIOs)
{
   return _qapi_TLMM_Get_Total_Gpios(qsTargetId, total_GPIOs);
}
