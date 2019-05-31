/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */

#ifndef __SPI_RW_H
#define __SPI_RW_H

#include   <stdio.h>
#include   <stdint.h>

#include   "hif_internal.h"
#include   "hif.h"

#define GPIO_STRING_MAX  256
#define GPIO_SETUP_MAX_LOOP_COUNT    10000
#define GPIO_SETUP_SLEEP_US          10

#define	IsHostCtrlWrtDone(v)     (((val) & SPI_STATUS_HOST_ACCESS_DONE_MSK) == SPI_STATUS_HOST_ACCESS_DONE_VAL)

typedef enum {
    HIF_SPI_OK = 0,
    HIF_SPI_ERROR
} HIF_SPI_STATUS;

typedef struct htc_spi_ctxt_s {
    int  qzChipSelectFd;
    int  qzChipSelectActiveLow;

    int  qzResetFd;
    int  qzResetActiveLow;

    int  qzWakeFd;
    int  qzWakeActiveLow;

    int  qzIntFd;

    int  qzSpiFd;
    int  halQzClearToTransmit;

    HIF_DEVICE  *device;

    
    int  halQzHostIntAssertedOnPowerup;
    pthread_t thread;
    int  spi_interrupt_init_flag;

    pthread_t rcv_thread;
    int  rcv_thread_init_flag;

    uint16_t  block_size;
    uint16_t  spi_intr_pin_num;
} htc_spi_ctxt_t;

extern htc_spi_ctxt_t    htc_spi_ctxt;

/* Round up a value (e.g. length) to a power of 2 (e.g. block size).  */
#define SPI_FIFO_ROUND_UP(value, pwrof2) (((value) + (pwrof2) - 1) & ~((pwrof2) - 1))

int32_t ccs_spiRead(uint16_t reg_addr, uint8_t *pdata, uint32_t len);
int32_t ccs_spiWrite(uint16_t reg_addr, uint8_t *pdata, uint32_t len);

int32_t ccs_write_host_register(uint16_t reg_addr, uint16_t size, uint8_t *pval);
uint32_t ccs_read_host_register(uint16_t reg_addr, uint16_t size, uint8_t *pval);

int custom_spi_init(HIF_SPI_INTERFACE_CONFIG *pConfig);
int custom_spi_deinit(void);

int	ccs_negotiate_block_size();

uint16_t htc_spi_read_reg(uint16_t reg_addr);
int32_t htc_write_spi_reg(uint16_t reg_addr, uint16_t val);

int32_t ccs_spiWriteFIFO(uint16_t reg_addr, uint8_t *pdata, uint32_t len);
int ccs_spiReadFIFO(uint16_t reg_addr, uint8_t *pdata, uint32_t len);
int setup_Qz_wakeup ();
void deassertQzWake(void);
void assertQzWake(void);
int sleep_ms(uint32_t ms);

void export_gpio(const char *gpioNumber);
void unexport_gpio(const char *gpioNumber);

int open_gpio_value_file(const char *gpioName, GpioDir dir);
void set_gpio_direction(const char *gpioNumber, const char *direction);

#endif
