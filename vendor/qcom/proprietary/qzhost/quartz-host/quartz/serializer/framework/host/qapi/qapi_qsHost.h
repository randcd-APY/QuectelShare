/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QAPI_QSHOST_H__
#define __QAPI_QSHOST_H__

#include <stdint.h>

#define QAPI_QS_OK             (0x00)
#define QAPI_QS_ERR            (0x01)

#define QAPI_QS_UART_E         (0x00)
#define QAPI_QS_SPI_E          (0x01)
#define QAPI_QS_SDIO_E         (0x02)

#define QAPI_QS_MODULE_WLAN         (0x00)
#define QAPI_QS_MODULE_BLE          (0x01)
#define QAPI_QS_MODULE_HMI          (0x02)
#define QAPI_QS_MODULE_ZIGBEE       (0x03)
#define QAPI_QS_MODULE_THREAD       (0x04)
#define QAPI_QS_MODULE_COEX         (0x05)
#define QAPI_QS_MODULE_HERH         (0x06)
#define QAPI_QS_MODULE_FWUPGRADE    (0x07)
#define QAPI_QS_MODULE_TLMM         (0x08)
#define QAPI_QS_MODULE_MAX          (0x09)
#define QAPI_QS_HELLO_REQ_E         (0x0E)
#define QAPI_QS_HELLO_RESP_E        (0x0F)

#define QAPI_QS_EVENT_SERVER_EXIT     (0x01<<0)
#define QAPI_QS_EVENT_TARGET_ASSERT   (0x01<<1)

typedef void (* WlanCb_t)(uint8_t dev_Id, uint32_t cb_Id, void *app_Cxt, void  *buf, uint32_t buf_Len);
typedef void (* WpanCb_t)(uint8_t module, uint8_t fileId, uint16_t eventId, uint32_t buf_Len, uint8_t *buf);
typedef void (* eventCb_t)(uint32_t eventId, void *param);

#ifdef CONFIG_DAEMON_MODE

/* Sets target device ID.
 * DO NOT include this function in customer documentation.
 * This is to be used only by TABI for internal testing.

  @param targetId    Target ID to be set.

  @return 0 for success, non-zero for failure.
*/
uint32_t qapi_Qs_Set_Target_Id(uint8_t targetId);

/* Applications should use this QAPI to initialize the target for hosted mode.

  @param serverName   Name of daemon server queue.
  @param maxMsgSize   Maximum possible message size.
  @param maxMsgCnt    Maximum possible message count.

  @return 0 for success, non-zero for failure.
*/
uint32_t qapi_Qs_Init(char *serverName, uint16_t maxMsgSize, uint16_t maxMsgCnt);

/* Applications should use this QAPI to deinitialize hosted mode feature */
uint32_t qapi_Qs_DeInit();

/* Applications should use this QAPI to register event callback.

  @param cb           Event callback.
  @param parameter    Parameter to pass the callback.

  @return 0 for success, non-zero for failure.
*/
uint32_t qapi_Qs_Register_Cb(eventCb_t cb, void *parameter);

#else

/* Applications must initialize each required module, transport will be initialized internally.
 * device_Name: Name of the actual device to be used for transport
 * Multiple modules can use the same device */
uint32_t qapi_Qs_Init(uint8_t module, uint32_t transport, char *device_Name);

/* Whenever applications send a QAPI request to set a callback function for asynchronous event, this function must be
 * called to set a  callback locally on host for the given module.
 * WLAN module must have callback of type WlanCb_t
 * BLE must have a callback of type WpanCb_t */
uint32_t qapi_Qs_DeInit(uint8_t module);

#endif /* CONFIG_DAEMON_MODE */
#endif /* __QAPI_QSHOST_H__ */

