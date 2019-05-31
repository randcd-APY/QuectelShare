/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <glob.h>           /* Including glob.h for glob() function used in find_pid() */
#include <signal.h>
#include <err.h>
#include <errno.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "htc_internal.h"
#include "spi_regs.h"
#include "spi_hw.h"
#include "hif_spi_rw.h"
#include "spi_regs.h"
#include "dbg.h"

static spi_config_t  spi_cfg = {
    .mode = QZ_SPI_MODE,
    .bytes_per_word = QZ_SPI_BITS_PER_WORD,
    .baudrate = QZ_SPI_SPEED_HZ,
};
static struct spi_ioc_transfer  spi_xfer[2];

htc_spi_ctxt_t    htc_spi_ctxt;

static void sleepDelayMicroseconds(int delay)
{
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = delay;
    select(0, NULL, NULL, NULL, &timeout);
}

int sleep_ms(uint32_t ms)
{
    struct timespec req, rem;

    req.tv_sec = ms / 1000;	        		/* seconds */
    req.tv_nsec = (ms % 1000) * 1000;       /* nanoseconds */

    nanosleep(&req, &rem);
    return 0;
}

uint16_t HW_GetMboxAddress(uint16_t mbox, uint16_t length)
{
    uint16_t address;

    if (mbox >= 4)
        mbox = 0;

    address = (HIF_MBOX_START_ADDR(mbox) + HIF_MBOX_WIDTH - length);

    return address;
}

/*
 * Function: open_gpio_value_file
 * Description:    open GPIO device file as read or write
 *                 GPIO device file must be created with export_gpio before this routine call
 *                 
 * Input: 
 *        gpioNumber:  GPIO number
 *        dir:         read or write
 * Returns:
 *        opened device file handler
 *
 */

int open_gpio_value_file(const char *gpioName, GpioDir dir)
{
    int fd = -1;
    char gpioDevicePath[GPIO_STRING_MAX+1] = "/sys/class/gpio/gpio";

    if (gpioName && *gpioName)
    {
        if (gpioName[0] == '/')
        { // Full path on command line overrides
            strncpy(gpioDevicePath, gpioName, sizeof(gpioDevicePath)-1);
        } else {
            strncat(gpioDevicePath, gpioName, sizeof(gpioDevicePath)-strlen(gpioDevicePath)-1);
            strncat(gpioDevicePath, "/value", sizeof(gpioDevicePath)-strlen(gpioDevicePath)-1);
        }
        fd = open(gpioDevicePath, (dir == DIR_INPUT ? O_RDONLY : O_WRONLY) | O_NONBLOCK);

        if (fd < 0)
        {
            perror("gpio open failed");
            return -1;
        }
        HTCPrintf(HTC_PRINT_GRP0, "Opened GPIO %s OK.\r\n", gpioName);
    }
    return fd;
}

/*
 * Function: export_gpio
 * Description:    export indicated GPIO to file system, so that GPIO can be accessed by 
 *                 file system API
 *                 after export, a device file(gpioN) will appear in /sys/class/gpio folder,
 *                 N is gpio number
 *                 
 * Input: 
 *        gpioNumber:     GPIO number
 * Returns:
 *        none
 *
 */

void export_gpio(const char *gpioNumber)
{
    int fd = -1;
    int length = strlen(gpioNumber);
    int returnLen;

    fd = open("/sys/class/gpio/export", O_WRONLY);

    if (fd == -1)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot open /sys/class/gpio/export.");
        return;
    }

    returnLen = write(fd, gpioNumber, length);
    if (returnLen < 0)
    {
        HTCPrintf(HTC_PRINT_GRP0, "GPIO-%s Cannot write to /sys/class/gpio/export.\r\n", gpioNumber);
    }

    close(fd);
}

void unexport_gpio(const char *gpioNumber)
{
    int fd = -1;
    int length = strlen(gpioNumber);
    int returnLen;

    fd = open("/sys/class/gpio/export", O_WRONLY);

    if (fd == -1)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot open /sys/class/gpio/unexport.");
        return;
    }

    returnLen = write(fd, gpioNumber, length);
    if (returnLen < 0)
    {
        HTCPrintf(HTC_PRINT_GRP0, "GPIO-%s Cannot write to /sys/class/gpio/export.\r\n", gpioNumber);
    }

    close(fd);
}

/*
 * Function: set_gpio_direction
 * Description:    set GPIO as input or output 
 *                 
 * Input: 
 *        gpioNumber:  GPIO number
 *        direction :  indicated GPIO as input(in) or output(out)
 * Returns:
 *        none
 *
 */

void set_gpio_direction(const char *gpioNumber, const char *direction)
{
    int fd = -1;
    char filename[GPIO_STRING_MAX];
    int length, loopCount = 0;

    length = strlen(direction);

    sprintf(filename, "/sys/class/gpio/gpio%s/direction", gpioNumber);

    while (fd == -1 && loopCount < GPIO_SETUP_MAX_LOOP_COUNT)
    {
        fd = open(filename, O_WRONLY);
        sleepDelayMicroseconds(GPIO_SETUP_SLEEP_US);
        loopCount++;
    }

    if (fd == -1)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot open %s.", filename);
        return;
    }

    if (write(fd, direction, length)<0)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot write to %s.", filename);
    }

    close(fd);
}

/*
 * Function: set_gpio_interrupt_edge
 * Description:    set GPIO interrupt edge trigger type
 *                 
 * Input: 
 *        gpioNumber:    GPIO number
 *        edge:          interrupt trigger type
 * Returns:
 *        none
 *
 */

void set_gpio_interrupt_edge(const char *gpioNumber, const char *edge)
{
    int fd = -1;
    char filename[GPIO_STRING_MAX];
    int length, loopCount = 0;

    length = strlen(edge);

    sprintf(filename, "/sys/class/gpio/gpio%s/edge", gpioNumber);

    while (fd == -1 && loopCount < GPIO_SETUP_MAX_LOOP_COUNT)
    {
        fd = open(filename, O_WRONLY);
        sleepDelayMicroseconds(GPIO_SETUP_SLEEP_US);
    }

    if (fd == -1)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot open %s.", filename);
        assert(0);
    }
    if (write(fd, edge, length)<0)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot write to %s.", filename);
    }
    close(fd);
}

/*
 * Function: setup_QzHost_interrupt
 * Description:    set up QCA402X Interrupt through GPIO interrupt
 *                 set interrupt type to falling-edge trigger
 *                 
 * Input: 
 *        none
 * Returns:
 *        0:           successful
 *       -1:           fails
 *
 */

int setup_QzHost_interrupt()
{
    char  str_intr[5];
    int   len;
    
    len = snprintf(str_intr, sizeof(str_intr), "%d",htc_spi_ctxt.spi_intr_pin_num);
    if (len > 4)
    {
        return -1;
    }
    export_gpio(str_intr);
    set_gpio_direction(str_intr, "in");
    set_gpio_interrupt_edge(str_intr, "falling");
    htc_spi_ctxt.qzIntFd = open_gpio_value_file(str_intr, DIR_INPUT);
    if (htc_spi_ctxt.qzIntFd < 0)
        return -1;

    return 0;
}

void deassertQzWake(void)
{
    assert(htc_spi_ctxt.qzWakeFd >= 0);

    uint8_t gpioPinState = '1';
    (void) write(htc_spi_ctxt.qzWakeFd, &gpioPinState, 1);
}

void assertQzWake(void)
{
    assert(htc_spi_ctxt.qzWakeFd >= 0);

    uint8_t gpioPinState = '0';
    (void) write(htc_spi_ctxt.qzWakeFd, &gpioPinState, 1);
}

/*
 *  set up Quartz PWD
 */
int setup_Qz_wakeup ()
{
    export_gpio(QZ_WAKE_GPIOn);
    set_gpio_direction(QZ_WAKE_GPIOn, "out");
    htc_spi_ctxt.qzWakeFd = open_gpio_value_file(QZ_WAKE_GPIOn, DIR_OUTPUT);
    if (htc_spi_ctxt.qzWakeFd < 0)
        return -1;

    deassertQzWake();

    return 0;
}

/*
 * Function: ccs_spi_read_host_setting
 * Description:    set up SPI HOST_CTRL_BYTE_SIZE register, and HOST_CTRL_CONFIG
 *                 register for host register read 
 * Input: 
 *        start_addr:     SPI host register start address
 *        len:            read length
 * Returns:
 *        16 bits register content
 *
 */

int32_t ccs_spi_read_host_setting(int start_addr, int len)
{
    uint16_t    val, retry_count=256;
    int32_t     ret_val;

    val = 0;
    val = HOST_CTRL_BYTE_SIZE_INC_SET(val, ADDRESS_INC_YES);
    val = HOST_CTRL_BYTE_SIZE_BYTE_SIZE_SET(val, len);
    ccs_spiWriteRegisterCmd(SPI_REG_HOST_CTRL_BYTE_SIZE, val);
	
    HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", SPI_REG_HOST_CTRL_BYTE_SIZE, val);

    val = 0;
    val = HOST_CTRL_CONFIG_ADDRESS_SET(val,start_addr);
    val = HOST_CTRL_CONFIG_DIRECTION_SET(val, 0);
    val = HOST_CTRL_CONFIG_ENABLE_SET(val, 1);
    ccs_spiWriteRegisterCmd(SPI_REG_HOST_CTRL_CONFIG, val);

    HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", SPI_REG_HOST_CTRL_CONFIG, val);

    /*
     *  waiting for host register read done
     */
    ret_val = ccs_spiReadRegisterCmd(SPI_REG_SPI_STATUS, &val);
    HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", SPI_REG_SPI_STATUS, val);
    
    while ( !IsHostAceesDone(val) && (retry_count != 0))
    {
        sleep_ms(SPI_HOST_REG_ACCESS_DELAY);

        ret_val = ccs_spiReadRegisterCmd(SPI_REG_SPI_STATUS, &val);

        HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", SPI_REG_SPI_STATUS, val);
        retry_count--;
    }

    if (retry_count == 0)
        return  -1;

    return ret_val;
}

/*
 * Function: ccs_write_host_register
 * Description:    write SPI host register. 
 * Input: 
 *        start_addr:     SPI host register start address
 *        len:            written data length
 *        pbuff:          written data buffer
 * Returns:
 *        0
 *
 */

int32_t ccs_write_host_register(uint16_t start_addr, uint16_t len, uint8_t *pbuff)
{
    uint16_t	val;
    uint16_t	retry_count=256;

    val = 0;
    val = HOST_CTRL_BYTE_SIZE_INC_SET(val, ADDRESS_INC_YES);
    val = HOST_CTRL_BYTE_SIZE_BYTE_SIZE_SET(val, len);
    ccs_spiWriteRegisterCmd(SPI_REG_HOST_CTRL_BYTE_SIZE, val);
    HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", 
                                                    SPI_REG_HOST_CTRL_BYTE_SIZE, val);

    ccs_spiWrite(SPI_REG_HOST_CTRL_WR_PORT, pbuff, len);

    val = 0;
    val = HOST_CTRL_CONFIG_ADDRESS_SET(val,start_addr);
    val = HOST_CTRL_CONFIG_DIRECTION_SET(val, 1);
    val = HOST_CTRL_CONFIG_ENABLE_SET(val, 1);
    ccs_spiWriteRegisterCmd(SPI_REG_HOST_CTRL_CONFIG, val);
    HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", 
                                                         SPI_REG_HOST_CTRL_CONFIG, val);

    /*
     *  waiting for host register write done
     */
    do {
        ccs_spiReadRegisterCmd(SPI_REG_SPI_STATUS, &val);
        HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request Host reg addr=%0x val=0x%0x\n", 
                                                SPI_REG_SPI_STATUS, val);

        if (IsHostAceesDone(val))
            break;

        retry_count--;
        sleep_ms(SPI_HOST_REG_ACCESS_DELAY);	/* 500ms */
    } while (retry_count != 0);

    if (retry_count == 0)
    {
        HTCPrintf(HTC_PRINT_GRP8,  "ccs_write_host_register fail\n");
        return  -1;
    }
    return 0;
}

HIF_SPI_STATUS ccs_spi_read_host8(int start_addr, uint8_t *pval)
{
    int32_t     ret_val;
    uint8_t     val;

    ret_val = ccs_spi_read_host_setting(start_addr, 1);
    if (ret_val == -1)
        return HIF_SPI_ERROR;

    ccs_spiRead(SPI_REG_HOST_CTRL_RD_PORT, &val, 1);
    *pval = val;

    return HIF_SPI_OK;
}

uint8_t ccs_spi_write_host8(int start_addr, uint8_t val)
{
    int32_t     ret_val;

    ret_val = ccs_write_host_register(start_addr, 1, &val);

    return ret_val;
}

HIF_SPI_STATUS ccs_spi_read_host16(int start_addr, uint8_t *pval)
{
    int32_t     ret_val;
    uint16_t    val;

    ret_val = ccs_spi_read_host_setting(start_addr, 2);
    if (ret_val == -1)
        return HIF_SPI_ERROR;

    ccs_spiRead(SPI_REG_HOST_CTRL_RD_PORT, (uint8_t *)&val, 2);
    *((uint32_t *)pval) = SPI2CPU16(val);

    return HIF_SPI_OK;
}

uint16_t ccs_spi_write_host16(int start_addr, uint16_t val)
{
    int32_t     ret_val;

    ret_val = ccs_write_host_register(start_addr, 2, (uint8_t *)&val);
    return ret_val;
}

HIF_SPI_STATUS ccs_spi_read_host32(int start_addr, uint8_t *pval)
{
    uint32_t	val;
    int32_t     ret_val;

    ret_val = ccs_spi_read_host_setting(start_addr, 4);
    if (ret_val == -1)
        return HIF_SPI_ERROR;

    ccs_spiRead(SPI_REG_HOST_CTRL_RD_PORT, (uint8_t *)&val, 4);

    *((uint32_t *)pval) = SPI2CPU32(val);

    return HIF_SPI_OK;
}

uint32_t ccs_spi_write_host32(int start_addr, uint32_t val)
{
    int32_t     ret_val;

    ret_val = ccs_write_host_register(start_addr, 4, (uint8_t *)&val);
    return ret_val;
}

HIF_SPI_STATUS ccs_read_host_register(uint16_t reg_addr, uint16_t size, uint8_t *pval)
{
    HIF_SPI_STATUS   val;

    switch(size)
    {
        case   1:
            val = ccs_spi_read_host8(reg_addr, pval);
            break;
        case   2:
            val = ccs_spi_read_host16(reg_addr, pval);
            break;
        case   4:
            val = ccs_spi_read_host32(reg_addr, pval);
            break;
        default:
            val = ccs_spi_read_host_setting(reg_addr, size);
            if (val == -1)
                return HIF_SPI_ERROR;

            ccs_spiRead(SPI_REG_HOST_CTRL_RD_PORT, pval, size);
            HTCPrintf(HTC_PRINT_GRP8,  "ccs_read_host_register addr=%02X len=%d\n", SPI_REG_HOST_CTRL_RD_PORT, size);
            break;
    }

    return val;
}

int custom_spi_open(const char *spiDevice, spi_config_t *pconfig)
{
    uint32_t speed;
    int  ret;

    htc_spi_ctxt.qzSpiFd = open(spiDevice, O_RDWR); 
    if (htc_spi_ctxt.qzSpiFd < 0)
    {
        HTCPrintf(HTC_PRINT_GRP0, "Cannot open SPI device %s.", spiDevice);
        perror("spi open failed");
        return -1;
    }
    HTCPrintf(HTC_PRINT_GRP0, "Opened SPI device %s.\r\n", spiDevice);

    assert(ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_WR_MODE, &pconfig->mode) >= 0);
    assert(ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_RD_MODE, &pconfig->mode) >= 0);
    HTCPrintf(HTC_PRINT_GRP0, "SPI-Mode:%x\r\n", pconfig->mode);

    assert(ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_WR_BITS_PER_WORD, &pconfig->bytes_per_word) >= 0);
    assert(ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_RD_BITS_PER_WORD, &pconfig->bytes_per_word) >= 0);
    HTCPrintf(HTC_PRINT_GRP0, "SPI-Bits:%d\r\n", pconfig->bytes_per_word);

    ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    HTCPrintf(HTC_PRINT_GRP0, "Current max SPI-Speed:%d\r\n", speed);

    speed = pconfig->baudrate;
    ret = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        perror("can't set max speed hz");
        return -1;
    }

    ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    HTCPrintf(HTC_PRINT_GRP0, "Max SPI-Speed:%d\r\n", speed);

    return  0;
}

/*
 * Function: custom_spi_init
 * Description:    initialize SPI.
 *                 set up SPI interrupt
 *                 open SPI device file
 *                 check if target connection is OK
 * Input: 
 *        pConfig:     SPI config parameter
 * Returns:
 *        0:           successful
 *       -1:           fails
 *
 */

int custom_spi_init(HIF_SPI_INTERFACE_CONFIG *pConfig)
{
    uint32_t   i;
    int    iRet;

    HTCPrintf(HTC_PRINT_GRP8,  "custom_spi_init\n");

    htc_spi_ctxt.qzSpiFd = -1;
    htc_spi_ctxt.qzWakeFd = -1;
    htc_spi_ctxt.qzIntFd = -1;

    htc_spi_ctxt.block_size = pConfig->block_size;
    htc_spi_ctxt.spi_intr_pin_num = pConfig->spi_intr_pin_num;
    
    iRet = setup_Qz_wakeup ();
    if (iRet < 0)
        goto done;

    iRet = setup_QzHost_interrupt();
    if (iRet < 0)
    {
        goto done;
    }
    spi_cfg.baudrate = pConfig->baud;

    iRet = custom_spi_open((char *)pConfig->dev, &spi_cfg);
    if (iRet != 0)
        goto done;

    for (i=0; i < 10000; i++)
    {
        uint16_t  reg_addr, val;

        reg_addr = SPI_REG_WRBUF_SPC_AVAL;
        val = htc_spi_read_reg(reg_addr);

        if ((val != SPI_TEST_OK) && ((i % 10) == 0))
        {
            HTCPrintf(HTC_PRINT_GRP8,  "reg:%x val:0x%x\r\n", reg_addr, val);
            HTCPrintf(HTC_PRINT_GRP8,  "SPI %s\r\n", ((val == SPI_TEST_OK)?"works fine":"not good"));
        }
        if (val == SPI_TEST_OK)
        {
            HTCPrintf(HTC_PRINT_GRP8,  "SPI is OK i=%d\n", i);
            return 0;
        }
    }

    printf("SPI test fails\n");

done:

    if (htc_spi_ctxt.qzSpiFd > 0)
    {
        close(htc_spi_ctxt.qzSpiFd);
    }
    if (htc_spi_ctxt.qzWakeFd > 0)
    {
        close(htc_spi_ctxt.qzWakeFd);
    }
    if (htc_spi_ctxt.qzIntFd > 0)
    {
        close(htc_spi_ctxt.qzIntFd);
    }


    HTCPrintf(HTC_PRINT_GRP8,  "SPI connection fails\n");
    return -1;
}

int custom_spi_deinit(void)
{
    char  str_intr[5];
    int   len;
    
    len = snprintf(str_intr, sizeof(str_intr), "%d",htc_spi_ctxt.spi_intr_pin_num);
    if (len > 4)
    {
        return -1;
    }

    if (htc_spi_ctxt.qzSpiFd > 0)
    {
        close(htc_spi_ctxt.qzSpiFd);
        htc_spi_ctxt.qzSpiFd = -1;
    }
    if (htc_spi_ctxt.qzWakeFd > 0)
    {
        close(htc_spi_ctxt.qzWakeFd);
        htc_spi_ctxt.qzWakeFd = -1;
    }
    if (htc_spi_ctxt.qzIntFd > 0)
    {
        close(htc_spi_ctxt.qzIntFd);
        htc_spi_ctxt.qzIntFd = -1;
    }
    
    unexport_gpio(QZ_WAKE_GPIOn);
    unexport_gpio(str_intr);

    return 0;
}

/*
 * Function: ccs_spiReadRegisterCmd
 * Description:    read SPI 16 bits register. 
 * Input: 
 *        reg_addr:     SPI register address
 *        reg_val:      16 bits return value buffer
 * Returns:
 *        16 bits register content
 *
 */

uint16_t ccs_spiReadRegisterCmd(uint16_t reg_addr, uint16_t *reg_val)
{
    int32_t    iRet;
    uint16_t   cmd;
    uint16_t   out_val, in_val;

    cmd = reg_addr | SPI_CMD_REG_READ;
    out_val = CPU2SPI16(cmd);

    spi_xfer[0].tx_buf        = (uint32_t)&out_val;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = 0;
    spi_xfer[1].rx_buf        = (uint32_t)&in_val;
    spi_xfer[1].len           = 2;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
        perror("SPI transfer fail");

    *reg_val = SPI2CPU16(in_val);

    return 0;
}

int32_t ccs_spiRead(uint16_t reg_addr, uint8_t *pdata, uint32_t len)
{
    int32_t    iRet;
    uint16_t   cmd;
    uint16_t   spi_cmd_val;

    cmd = reg_addr | SPI_CMD_REG_READ;
    spi_cmd_val = CPU2SPI16(cmd);

    spi_xfer[0].tx_buf        = (uint32_t)&spi_cmd_val;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = 0;
    spi_xfer[1].rx_buf        = (uint32_t)pdata;
    spi_xfer[1].len           = len;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
        perror("SPI transfer fail");

    return 0;
}

uint16_t htc_spi_read_reg(uint16_t reg_addr)
{
    uint16_t    result;

    ccs_spiRead(reg_addr, (uint8_t *)&result, 2);

    return SPI2CPU16(result);
}

int32_t ccs_spiWrite(uint16_t reg_addr, uint8_t *pdata, uint32_t len)
{
    int32_t    iRet;
    uint16_t   cmd;
    uint16_t   spi_cmd_val;

    cmd = reg_addr | SPI_CMD_REG_WRITE;
    spi_cmd_val = CPU2SPI16(cmd);

    spi_xfer[0].tx_buf        = (uint32_t)&spi_cmd_val;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = (uint32_t)pdata;
    spi_xfer[1].rx_buf        = 0;
    spi_xfer[1].len           = len;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
        perror("SPI transfer fail");

    return 0;
}

int32_t htc_write_spi_reg(uint16_t reg_addr, uint16_t val)
{
    uint16_t spi_val;

    spi_val = CPU2SPI16(val);
    return ccs_spiWrite(reg_addr, (uint8_t *)&spi_val, 2);
}

/*
 * Function: ccs_spiReadFIFO
 * Description:    read SPI FIFO. 
 * Input: 
 *        reg_addr:     SPI register address
 *        pdata:        data buffer
 *        len:          data length
 * Returns:
 *        0
 *
 */

int ccs_spiReadFIFO(uint16_t reg_addr, uint8_t *pdata, uint32_t len)
{
    int32_t    iRet;
    uint16_t   cmd;
    uint16_t   spi_cmd;

    cmd = reg_addr | SPI_CMD_MAILBOX_READ;
    spi_cmd = CPU2SPI16(cmd);

    spi_xfer[0].tx_buf        = (uint32_t)&spi_cmd;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = 0;
    spi_xfer[1].rx_buf        = (uint32_t)pdata;
    spi_xfer[1].len           = len;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
    {
        printf("len=%d\n", len);
        perror("FIFO read fail");
    }

    return 0;
}

/*
 * Function: ccs_spiWriteRegisterCmd
 * Description:    write value to SPI 16 bits register. 
 * Input: 
 *        reg_addr:     SPI register address
 *        reg_val:      new 16 bits register value
 * Returns:
 *        0
 *
 */

uint16_t ccs_spiWriteRegisterCmd(uint16_t reg_addr, uint16_t reg_val)
{
    int32_t     iRet;
    uint16_t    cmd;
    uint16_t    out_cmd, out_val;

    cmd = reg_addr | SPI_CMD_REG_WRITE;
    out_cmd = CPU2SPI16(cmd);
    out_val = CPU2SPI16(reg_val);

    spi_xfer[0].tx_buf        = (uint32_t)&out_cmd;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = (uint32_t)&out_val;
    spi_xfer[1].rx_buf        = 0;
    spi_xfer[1].len           = 2;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
        perror("SPI transfer fail");

    return 0;
}

/*
 * Function: ccs_spiWriteFIFO
 * Description:    write data to SPI FIFO. 
 * Input: 
 *        reg_addr:     SPI register address
 *        pdata:        data buffer
 *        len:          data length
 * Returns:
 *        0
 *
 */

int32_t ccs_spiWriteFIFO(uint16_t reg_addr, uint8_t *pdata, uint32_t len)
{
    int32_t     iRet;
    uint16_t    cmd;
    uint16_t    spi_cmd;

    cmd = reg_addr | SPI_CMD_MAILBOX_WRITE;
    spi_cmd = CPU2SPI16(cmd);
    spi_xfer[0].tx_buf        = (uint32_t)&spi_cmd;
    spi_xfer[0].rx_buf        = 0;
    spi_xfer[0].len           = 2;
    spi_xfer[0].delay_usecs   = 0;
    spi_xfer[0].speed_hz      = spi_cfg.baudrate;
    spi_xfer[0].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[0].cs_change     = 1;

    spi_xfer[1].tx_buf        = (uint32_t)pdata;
    spi_xfer[1].rx_buf        = 0;
    spi_xfer[1].len           = len;
    spi_xfer[1].delay_usecs   = 0;
    spi_xfer[1].speed_hz      = spi_cfg.baudrate;
    spi_xfer[1].bits_per_word = QZ_SPI_BITS_PER_WORD;
    spi_xfer[1].cs_change     = 0;

    iRet = ioctl(htc_spi_ctxt.qzSpiFd, SPI_IOC_MESSAGE(2), spi_xfer);
    if (iRet < 0)
    {
        printf("len=%d\n", len);
        perror("FIFO write fail");
    }

    return 0;
}
