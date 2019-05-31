/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include "qsHost.h"
#include "qapi_qsHost.h"

#ifdef CONFIG_DAEMON_MODE

uint32_t qapi_Qs_Set_Target_Id(uint8_t targetId)
{
    return QsSetTargetId(targetId);
}

uint32_t qapi_Qs_Init(char *serverName, uint16_t maxMsgSize, uint16_t maxMsgCnt)
{
    return QsInit(serverName, maxMsgSize, maxMsgCnt);
}

uint32_t qapi_Qs_DeInit()
{
    QsDeInit();
    return 0;
}

uint32_t qapi_Qs_Register_Cb(eventCb_t cb, void *parameter)
{
    return QsRegisterCb(cb, parameter);
}

#else

uint32_t qapi_Qs_Init(uint8_t module, uint32_t transport, char *device_Name)
{
    return QsInit(module, transport, device_Name);
}

uint32_t qapi_Qs_DeInit(uint8_t module)
{
    return QsDeInit(module);
}
#endif /* CONFIG_DAEMON_MODE*/
