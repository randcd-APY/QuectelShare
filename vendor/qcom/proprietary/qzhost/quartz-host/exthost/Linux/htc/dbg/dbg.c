/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <getopt.h>
#include <glob.h>           /* Including glob.h for glob() function used in find_pid() */
#include <signal.h>
#include <err.h>
#include <errno.h>
#include <sys/queue.h>
#include <pthread.h>
#include <assert.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include  "spi_regs.h"
#include  "htc_internal.h"

#include  "hif_spi_rw.h"
#include  "dbg.h"

#ifdef DEBUG_HTC
uint32_t    print_mask = HTC_PRINT_GRP0_HIF;
#else
uint32_t    print_mask = 0;
#endif

int open_gpio_value_file(const char *gpioName, GpioDir dir);

void htc_printf(char *fmt,...)
{
    va_list ap;

    va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

#ifdef  DEBUG_HTC
void HTCPrintf(uint32_t print, char *fmt,...)
{
    va_list ap;
    
    if ((print_mask & print) == 0)
        return;

    va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
#endif

void HTCPrintSetMask (int pos, uint32_t new_mask)
{
    uint32_t mask;

    if (pos == 0)
    {
        print_mask = new_mask;
        return;
    }

    if (pos > sizeof(uint32_t) * 8)
        return;

    mask = 1;
    mask <<= (pos -1);

    if (new_mask)
        print_mask |= mask;
    else
        print_mask &= ~mask;    
}

#ifdef  HTC_TOGGLE_ENABLE

/*
 *  GPIO toggling
 */

typedef struct gpio_toggle_s {
    uint32_t       gpio_num;
    uint32_t       status;
    char           gpioNumber[5];
    int            fd;
} gpio_toggle_t;

gpio_toggle_t *m_pToggle;

gpio_toggle_t *init_toggle(int gpio_pin_num)
{
    int   len;
    gpio_toggle_t *pToggle;
    
    pToggle = (gpio_toggle_t *)malloc(sizeof(gpio_toggle_t));
    if (pToggle == NULL)
        return NULL;
    
    memset(pToggle, 0, sizeof(*pToggle));
    
    len = snprintf(pToggle->gpioNumber, sizeof(pToggle->gpioNumber), "%d", gpio_pin_num);
    
    do {
        if (len > 3)
            break;
        
        printf("toggling pin=%s\n", pToggle->gpioNumber);
        
        export_gpio(pToggle->gpioNumber);
        set_gpio_direction(pToggle->gpioNumber, "out");
        pToggle->fd = open_gpio_value_file(pToggle->gpioNumber, DIR_OUTPUT);
    } while (0);
    
    if (pToggle->fd <= 0)
    {
        free (pToggle);
        pToggle = NULL;
        return NULL;    
    }
   
    return pToggle;
}

void deinit_toggle(gpio_toggle_t **ppToggle)
{
    gpio_toggle_t *pToggle = *ppToggle;
    
    if (pToggle != NULL)  
    {
        unexport_gpio(pToggle->gpioNumber);
        close(pToggle->fd);
        free(pToggle);
        *ppToggle = NULL;
    }
}

void toggle(void **p, int gpio_pin_num)
{
    char gpioPinState[3];
    gpio_toggle_t **ppToggle = (gpio_toggle_t **)p;
    gpio_toggle_t *pToggle;
    
    if (ppToggle == NULL)
        ppToggle = &m_pToggle;
    
    if (*ppToggle == NULL)
    {
        *ppToggle = init_toggle(gpio_pin_num);
    }
    pToggle = *ppToggle;
    if (pToggle == NULL)
        return;
    
    if (pToggle->status == 1)
        pToggle->status = 0;
    else
        pToggle->status = 1;
        
    assert(pToggle->fd >= 0);

    snprintf(gpioPinState, sizeof(gpioPinState), "%d", pToggle->status);
    (void) write(pToggle->fd, gpioPinState, 1);
}
#endif
