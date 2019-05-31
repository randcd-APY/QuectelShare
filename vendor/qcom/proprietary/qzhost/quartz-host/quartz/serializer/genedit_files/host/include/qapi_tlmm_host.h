/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_tlmm.h"

qapi_Status_t _qapi_TLMM_Get_Gpio_ID(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t *qapi_GPIO_ID);

qapi_Status_t _qapi_TLMM_Release_Gpio_ID(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config, qapi_GPIO_ID_t qapi_GPIO_ID);

qapi_Status_t _qapi_TLMM_Config_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, qapi_TLMM_Config_t *qapi_TLMM_Config);

qapi_Status_t _qapi_TLMM_Drive_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin, qapi_GPIO_Value_t value);

qapi_GPIO_Value_t _qapi_TLMM_Read_Gpio(uint8_t TargetID, qapi_GPIO_ID_t qapi_GPIO_ID, uint32_t pin);

qapi_GPIO_Value_t _qapi_TLMM_Get_Gpio_Outval(uint8_t TargetID, uint32_t pin);

qapi_Status_t _qapi_TLMM_Get_Gpio_Config(uint8_t TargetID, qapi_TLMM_Config_t *qapi_TLMM_Config);

qapi_Status_t _qapi_TLMM_Get_Total_Gpios(uint8_t TargetID, uint32_t *total_GPIOs);
