/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef HTC_H
#define HTC_H

#include "htc_internal.h"

#define HTC_TARGET_INSTANCE_0  (0)
#define HTC_SPI_RX_BUF         (2)

int htc_init(void* pCxt);
void signal_htc(void* pCxt);
int htc_deinit(void* pCxt);
int32_t htc_iface_queue_init(HTC_IFACE_CXT_T* pIfaceCxt);
int32_t htc_add_header(uint8_t* buf, uint16_t length);
int htc_get_statistics(HTC_IFACE_CXT_T* pIfaceCxt, long *send_count, long *recv_count);


void event_sent_Handler(HTC_TARGET * target,
        HTC_ENDPOINT_ID ep, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo, void *context);
void event_received_Handler(HTC_TARGET * target,
        HTC_ENDPOINT_ID ep, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo, void *context);
extern HTC_TARGET *htcTargetInstance(int i);

void* spi_TXthread(void* arg);
void* spi_RXthread(void* arg);
#endif
