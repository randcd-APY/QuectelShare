/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIG_DAEMON_MODE

#ifndef __QSUART_H__
#define __QSUART_H__

#include <stdint.h>

#define QS_UART_MAX_PORTS_E 2

uint32_t QsUartInit(char *device_Name, int32_t *handle);
uint32_t QsUartDeInit(int32_t handle);
uint32_t QsUartTransmit(int32_t handle, uint32_t len, char *buffer);

#endif /* __QSUART_H__ */

#endif /* not defined CONFIG_DAEMON_MODE */

