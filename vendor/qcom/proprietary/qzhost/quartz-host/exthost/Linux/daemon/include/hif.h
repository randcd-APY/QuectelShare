/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef HIF_H
#define HIF_H

int32_t hif_UartInit(void* cxt, HTC_IFACE_CXT_T* pIfaceCxt, cfg_ini* cfg, uint8_t index);
int32_t hif_UartDeinit(HTC_IFACE_CXT_T* pIfaceCxt);

int32_t htc_spi_init(void* pCxt, HTC_IFACE_CXT_T* pIfaceCxt, cfg_ini* cfg, uint8_t Id);
int32_t htc_spi_deinit(HTC_IFACE_CXT_T* pIfaceCxt);

#endif
