/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifdef CONNX_UART_DEBUG
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif

#include <stdarg.h>
#include <linux/version.h>

/* Fix error: #warning "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE" [-Werror=cpp] */
/* Use deprecated macro for MTP_9X45 as glibc version in MTP_9X45 is older than 2.19 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* for CRTSCTS */
#endif
#else
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE /* for CRTSCTS */
#endif
#endif
#include <termios.h>

#include <poll.h>
#include <sys/ioctl.h>

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/prctl.h>

#include "connx_serial_com.h"
#include "connx_util.h"


#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
#define MSM_ENABLE_UART_CLOCK       0x5441  /* TIOCPMGET */
#define MSM_DISABLE_UART_CLOCK      0x5442  /* TIOCPMPUT */
#endif

#define QUEUE_SIZE             (9000)

static uint8_t UART0_RX_QUEUE[QUEUE_SIZE];
static uint32_t UART0_RX_in_PTR;
static uint32_t UART0_RX_out_PTR;
static uint32_t UART0_RX_numbers;

static pthread_mutex_t receiveMutex;
static pthread_t worker_thread;

/* Designates the reader and writer fds in a pipe(2) array. */
#define PIPE_READ   0
#define PIPE_WRITE  1

/* This maps the file descriptors into the pollfd struture. */
#define FD_UART     0
#define FD_CONTROL  1

typedef struct
{
    uint32_t    baud;
    speed_t     speed;
} UartBaudrateMap;


static struct pollfd pfd[2];
static int control;

static bool instInUse = false;
static bool instStarted = false;
static char instDevice[128];
static uint32_t curBaud = 0;
static uint32_t instBaud = 0;
static uint32_t instResetBaud = 0;
static ConnxUartDrvDataBits instDataBits = 0;
static ConnxUartDrvParity instParity = 0;
static ConnxUartDrvStopBits instStopBits = 0;
static bool instFlowControl = false;
static uint8_t instToken = 0;
static bool instUseToken = false;

static uint16_t rxBgint = 0xFFFF;
static ConnxUartDrvDataRx rxData = NULL;

/* [TODO] Only map some typical baudrate. */
static UartBaudrateMap baudrateMap[] =
{
    /* Baudrate,    Speed   */
    { 115200,       B115200  },
    { 460800,       B460800  },
    { 921600,       B921600  },
    { 2000000,      B2000000 },
    { 3000000,      B3000000 }
};

static void ConnxSchedBgintSet(uint16_t irq)
{
    CONNX_UNUSED(irq);

    ConnxUartDrvRx(NULL);
}

static speed_t baudrate_map(uint32_t baud)
{
    uint32_t index = 0;
    uint32_t count = CONNX_COUNT_OF(baudrateMap);
    UartBaudrateMap *ubm = NULL;

    for (index = 0; index < count; index++)
    {
        ubm = &baudrateMap[index];

        if (ubm->baud == baud)
            return ubm->speed;
    }

    return B115200;
}

/* THREAD FUNCTIONS */

static void *workerThread(void *threadParameter)
{
    int running = 1;

    char name[] = "workerThread";
    prctl(PR_SET_NAME, (unsigned long)&name, 0, 0, 0);

    DPRINTF(("THREAD START\n"));

    while (running)
    {
        int ready;

        DPRINTF(("POLL: 0x%x 0x%x\n", pfd[FD_UART].events,
                 pfd[FD_CONTROL].events));
        /*
         * We wait for events on the file descriptors indefinitely
         * using the following scheme:
         *
         *  FD_UART:    POLLIN if buffer is not full, and POLLOUT if we
         *              have pending data to transmit.
         *  FD_CONTROL: Always, and only, POLLIN to receive rescheduling
         *              and termination requests.
         */
        ready = poll(pfd, 2, -1);

        DPRINTF(("ready %d 0x%x 0x%x\n", ready,
                 pfd[FD_UART].revents, pfd[FD_CONTROL].revents));

        if (ready > 0)
        {
            if (pfd[FD_UART].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                DPRINTF(("ERROR FD_UART FAILED\n"));
                running = 0;
            }

            if (pfd[FD_CONTROL].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                DPRINTF(("ERROR FD_CONTROL FAILED\n"));
                running = 0;
            }

            if (pfd[FD_CONTROL].revents & POLLIN)
            {
                char stop;

                DPRINTF(("FD_CONTROL\n"));

                /*
                 * If stop == 0 this is just a rescheduling signal
                 * but otherwise we treat it as a termination request.
                 */
                if ((read(pfd[FD_CONTROL].fd, &stop, 1) != 1) ||
                    (stop != 0))
                {
                    DPRINTF(("STOP RECEIVED\n"));

                    running = 0;
                }
                else
                {
                    DPRINTF(("RESCHEDULING\n"));
                }
            }

            if (pfd[FD_UART].revents & POLLIN)
            {
                ssize_t bytesRead;
                int iores;
                int iocount, bufferSpace;
                bool tokenDetected = false;

                pthread_mutex_lock(&receiveMutex);
                bufferSpace = (int) UART0_RX_numbers;
                pthread_mutex_unlock(&receiveMutex);

                bufferSpace = QUEUE_SIZE - bufferSpace;

                /* Find out how much data there is */
                iocount = 0;

                iores = ioctl(pfd[FD_UART].fd, FIONREAD, &iocount);

                if (iores)
                {
                    iocount = 0;
                }

                iocount = CONNX_MIN(iocount, (int) (QUEUE_SIZE - UART0_RX_in_PTR));
                iocount = CONNX_MIN(iocount, bufferSpace);

                DPRINTF(("READ: %u\n", iocount));
                /* Read in and wrap around the list */
                bytesRead = read(pfd[FD_UART].fd, &UART0_RX_QUEUE[UART0_RX_in_PTR], iocount);

                if (bytesRead > 0)
                {
                    DPRINTF(("BYTES READ: %u\n", bytesRead));

                    if (instUseToken)
                    {
                        int i;

                        for (i = 0; i < bytesRead; i++)
                        {
                            if (UART0_RX_QUEUE[UART0_RX_in_PTR + i] == instToken)
                            {
                                tokenDetected = true;
                                break;
                            }
                        }
                    }

                    UART0_RX_in_PTR += bytesRead;

                    if (UART0_RX_in_PTR >= QUEUE_SIZE)
                    {
                        UART0_RX_in_PTR = 0;
                    }

                    /* [QTI] Avoid to notify data received here. Instead, the event
                        will be notified after "UART0_RX_numbers" is updated. */
                }
                else
                {
                    bytesRead = 0;
                }

                /*
                 * Indicate that there is data in the buffer,
                 * and stop subscribing to UART read events if
                 * the buffer is full.
                 */
                pthread_mutex_lock(&receiveMutex);
                UART0_RX_numbers += bytesRead;

                if (UART0_RX_numbers >= QUEUE_SIZE)
                {
                    pfd[FD_UART].events &= ~POLLIN;
                }

                pthread_mutex_unlock(&receiveMutex);

                /* Notify data received in UART. */
                if (bytesRead)
                {
                    if (!instUseToken)
                    {
                        DPRINTF(("BGINT\n"));
                        ConnxSchedBgintSet(rxBgint);
                    }
                    else if (instUseToken && tokenDetected)
                    {
                        DPRINTF(("TOKEN\n"));
                        ConnxSchedBgintSet(rxBgint);
                    }
                }
            }
        }
    }

    DPRINTF(("THREAD STOP\n"));
    return NULL;
}

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
static void ConnxUartEnableClock(int fd, bool enable)
{
    DPRINTF(("fd: %d, enable: %x\n", fd, enable));

    if (enable)
    {
        /* Vote UART clock on. */
        ioctl(fd, MSM_ENABLE_UART_CLOCK);
    }
    else
    {
        /* Vote UART clock off. */
        ioctl(fd, MSM_DISABLE_UART_CLOCK);
    }
}
#endif

void *ConnxUartDrvOpen(const char *device,
                       uint32_t *baud,
                       uint8_t dataBits,
                       uint8_t parity,
                       uint8_t stopBits,
                       bool flowControl,
                       const uint8_t *token)
{
    if (!instInUse && (baudrate_map(*baud) != B0))
    {
        instInUse = true;

        curBaud = 0;
        instBaud = instResetBaud = *baud;
        instDataBits = dataBits;
        instParity = parity;
        instStopBits = stopBits;
        instFlowControl = flowControl;

        if (token != NULL)
        {
            instUseToken = true;
            instToken = *token;
        }
        else
        {
            instUseToken = false;
        }

        ConnxStrNCpyZero(instDevice, device, sizeof(instDevice));

        return &instInUse;
    }

    return NULL;
}

void ConnxUartDrvClose(void *handle)
{
    CONNX_UNUSED(handle);

    instInUse = false;
}

static bool uartDeviceConfigure(int fd, speed_t speed)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0)
    {
        perror("uartDeviceConfigure: tcgetattr failed");
        return false;
    }

    if ((cfsetispeed(&options, speed) != 0) ||
        (cfsetospeed(&options, speed) != 0))
    {
        perror("uartDeviceConfigure: set baud rate failed");
        return false;
    }

    options.c_cflag |= (CLOCAL | CREAD);

    if (instParity == 0)
    {
        options.c_cflag &= ~PARENB;
    }
    else if (instParity == 1)
    {
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
    }
    else if (instParity == 2)
    {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
    }
    else
    {
        return false;
    }

    if (instStopBits == 2)
    {
        options.c_cflag |= CSTOPB;
    }
    else if (instStopBits == 1)
    {
        options.c_cflag &= ~CSTOPB;
    }
    else
    {
        return false;
    }

    if (instDataBits == 8)
    {
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
    }
    else if (instDataBits == 7)
    {
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS7;
    }
    else
    {
        return false;
    }

    if (instFlowControl)
    {
        options.c_cflag |= CRTSCTS;
    }
    else
    {
        options.c_cflag &= ~CRTSCTS;
    }

    options.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("uartDeviceConfigure: tcsetattr failed");
        return false;
    }

    return true;
}

bool ConnxUartDrvReconfigure(void *handle, uint32_t *baud, bool flowControl)
{
    speed_t speed = baudrate_map(*baud);

    CONNX_UNUSED(handle);

    if (speed == B0)
    {
        return false;
    }

    instBaud = instResetBaud = *baud;
    instFlowControl = flowControl;

    if (instStarted)
    {
        return uartDeviceConfigure(pfd[FD_UART].fd, speed);
    }

    return true;
}

bool ConnxUartDrvStart(void *handle, uint8_t reset)
{
    int ret, fd;
    static int controlPipe[2];
    speed_t speed;

    CONNX_UNUSED(handle);

    DPRINTF(("UARTDRIVER START\n"));

    if (reset == 0)
    {
        curBaud = instBaud;
    }
    else
    {
        curBaud = instResetBaud;
    }

    DPRINTF(("curBaud: %d\n", curBaud));

    fd = open(instDevice, O_RDWR | O_NOCTTY);

    if (fd == -1)
    {
        perror("ConnxUartDrvStart: device open failed");
        return false;
    }

    if (fcntl(fd, F_SETFL, 0) != 0)
    {
        perror("ConnxUartDrvStart: fcntl failed");
        return false;
    }

    speed = baudrate_map(curBaud);
    DPRINTF(("speed: 0x%x\n", speed));

    if (speed == B0)
    {
        fprintf(stderr, "ConnxUartDrvStart: unsupported baud rate: %d\n", curBaud);
        return false;
    }

    if (!uartDeviceConfigure(fd, speed))
    {
        fprintf(stderr, "ConnxUartDrvStart: device configuration failed\n");
        return false;
    }

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
    /* [QTI] In MDM or MSM platform, UART clock should always be enabled when to open UART.
       The reason is that MDM/MSM UART driver internally has native low power mode, in which
       UART clock may be turned off/on dynamically after some timeout (such as 100ms) if UART
       is in idle status. If UART clock is turned off internally by UART driver, the packet
       from BT chip may well be lost. This is fitted with both H4 and H4-IBS transport. */
    ConnxUartEnableClock(fd, true);
#endif

    if (pipe(controlPipe) != 0)
    {
        perror("ConnxUartDrvStart: create control pipe failed");
        close(fd);

        return false;
    }

    /*
     * Set up poll structure to provide read events
     * from the UART fd and reader pipe.
     */
    pfd[FD_UART].fd = fd;
    pfd[FD_UART].events = POLLIN;
    pfd[FD_CONTROL].fd = controlPipe[PIPE_READ];
    pfd[FD_CONTROL].events = POLLIN;

    control = controlPipe[PIPE_WRITE];

    UART0_RX_in_PTR = UART0_RX_out_PTR = UART0_RX_numbers = 0x00;

    if (pthread_mutex_init(&receiveMutex, NULL) != 0)
    {
        perror("ConnxUartDrvStart: pthread_mutex_init failed");
        close(controlPipe[0]);
        close(controlPipe[1]);
        close(fd);

        return false;
    }

    ret = pthread_create(&worker_thread, NULL, workerThread, NULL);

    if (ret != 0)
    {
        perror("ConnxUartDrvStart: pthread_create failed");
        close(controlPipe[0]);
        close(controlPipe[1]);
        close(fd);

        pthread_mutex_destroy(&receiveMutex);

        return false;
    }

    instStarted = true;
    return true;
}

bool ConnxUartDrvStop(void *handle)
{
    void *thread_res;
    char stop = 1;

    CONNX_UNUSED(handle);

    DPRINTF(("UARTDRIVER STOP\n"));

    if (!instStarted)
    {
        return true;
    }

    instStarted = false;

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
    /* [QTI] Vote UART clock off. */
    ConnxUartEnableClock(pfd[FD_UART].fd, false);
#endif

    /* Stop worker thread */
    if (write(control, &stop, 1) != 1)
    {
        perror("ConnxUartDrvStop: could not signal stop");
        return false;
    }
    else
    {
        DPRINTF(("STOPPING THREAD\n"));

        if (pthread_join(worker_thread, &thread_res) != 0)
        {
            perror("ConnxUartDrvStop: worker thread join error");
        }
        else
        {
            DPRINTF(("THREAD STOPPED\n"));
        }
    }

    close(pfd[FD_UART].fd);
    close(pfd[FD_CONTROL].fd);
    close(control);

    pthread_mutex_destroy(&receiveMutex);

    return true;
}

void ConnxUartDrvRtsSet(void *handle, bool state)
{
    CONNX_UNUSED(handle);

    int bit = TIOCM_RTS;

    if (ioctl(pfd[FD_UART].fd, state ? TIOCMBIS : TIOCMBIC, &bit) == -1)
    {
        perror("ConnxUartDrvRtsSet(): ioctl failed");
    }
}

uint32_t ConnxUartDrvGetTxSpace(void *handle)
{
    CONNX_UNUSED(handle);

    return QUEUE_SIZE;
}

bool ConnxUartDrvTx(void *handle, const uint8_t *data, uint32_t dataLength, uint32_t *numSent)
{
    ssize_t res;
    bool rv;

    CONNX_UNUSED(handle);

    DPRINTF(("TX %u\n", dataLength));

    res = write(pfd[FD_UART].fd, data, dataLength);

    if ((res >= 0) &&
        ((uint32_t) res == dataLength))
    {
        DPRINTF(("TX COMPLETE\n"));
        *numSent = dataLength;
        rv = true;
    }
    else
    {
        DPRINTF(("TX FAILURE\n"));
        rv = false;
    }

    return rv;
}

uint32_t ConnxUartDrvGetRxAvailable(void *handle)
{
    uint32_t rxBytes;

    CONNX_UNUSED(handle);

    pthread_mutex_lock(&receiveMutex);
    rxBytes = UART0_RX_numbers;
    pthread_mutex_unlock(&receiveMutex);

    return QUEUE_SIZE - rxBytes;
}

void ConnxUartDrvRx(void *handle)
{
    uint32_t bytesConsumed;
    uint32_t noOfBytes;
    bool reschedule = false;

    CONNX_UNUSED(handle);

    if (!rxData)
    {
        return;
    }

    DPRINTF(("RX\n"));
    pthread_mutex_lock(&receiveMutex);
    noOfBytes = UART0_RX_numbers;
    pthread_mutex_unlock(&receiveMutex);

    if (noOfBytes > (QUEUE_SIZE - UART0_RX_out_PTR))
    {
        noOfBytes = (QUEUE_SIZE - UART0_RX_out_PTR);
    }

    bytesConsumed = rxData(&UART0_RX_QUEUE[UART0_RX_out_PTR],
                           noOfBytes);

    DPRINTF(("CONSUMED %u\n", bytesConsumed));
    UART0_RX_out_PTR = UART0_RX_out_PTR + bytesConsumed;

    if (UART0_RX_out_PTR >= QUEUE_SIZE)
    {
        UART0_RX_out_PTR = 0x00; /* Buffer start from beginning*/
    }

    /*
     * Adjust buffer data counter and, if disabled, enable UART
     * read events if disabled, because there is free space now.
     */
    pthread_mutex_lock(&receiveMutex);
    UART0_RX_numbers = UART0_RX_numbers - bytesConsumed;
    noOfBytes = UART0_RX_numbers;

    if ((pfd[FD_UART].events & POLLIN) == 0)
    {
        DPRINTF(("RESCHEDULE\n"));
        pfd[FD_UART].events |= POLLIN;
        reschedule = true;
    }

    pthread_mutex_unlock(&receiveMutex);

    if (reschedule)
    {
        char stop = 0;
        ssize_t res = write(control, &stop, 1);
        CONNX_UNUSED(res);
    }

    /* If buffer is not empty, reschedule bgint. */
    /* [QTI] Need to double check "bytesConsumed" so as to avoid recursion. */
    if ((noOfBytes > 0) && (bytesConsumed > 0))
    {
        DPRINTF(("BGINT\n"));
        ConnxSchedBgintSet(rxBgint);
    }
}

uint32_t ConnxUartDrvLowLevelTransportTxBufLevel(void *handle)
{
    CONNX_UNUSED(handle);

    return 0;
}

uint32_t ConnxUartDrvLowLevelTransportRx(void *handle, uint32_t dataLength, uint8_t *data)
{
    uint32_t rxCount, rxLeft;

    CONNX_UNUSED(handle);

    pthread_mutex_lock(&receiveMutex);
    rxCount = UART0_RX_numbers;
    pthread_mutex_unlock(&receiveMutex);

    if (rxCount > dataLength)
    {
        rxCount = dataLength;
    }

    if (rxCount > (QUEUE_SIZE - UART0_RX_out_PTR))
    {
        rxCount = (QUEUE_SIZE - UART0_RX_out_PTR);
    }

    memcpy(data, &UART0_RX_QUEUE[UART0_RX_out_PTR], rxCount);

    UART0_RX_out_PTR = UART0_RX_out_PTR + rxCount;

    if (UART0_RX_out_PTR >= QUEUE_SIZE)
    {
        UART0_RX_out_PTR = 0x00; /* Buffer start from beginning*/
    }

    pthread_mutex_lock(&receiveMutex);
    UART0_RX_numbers -= rxCount;
    rxLeft = UART0_RX_numbers;
    pthread_mutex_unlock(&receiveMutex);

    /* If buffer is not empty initialize a new bg_int */
    if (rxLeft > 0)
    {
        ConnxSchedBgintSet(rxBgint);
    }

    return rxCount;
}

uint32_t ConnxUartDrvGetBaudrate(void *handle)
{
    CONNX_UNUSED(handle);

    return curBaud;
}

void ConnxUartDrvRegister(void *handle, ConnxUartDrvDataRx rxDataFn, uint16_t rxBgintHdl)
{
    CONNX_UNUSED(handle);

    rxData = rxDataFn;
    rxBgint = rxBgintHdl;
}

void *ConnxUartDrvInitialise(const char *device,
                             uint32_t baud, uint32_t resetBaud,
                             ConnxUartDrvDataBits dataBits,
                             ConnxUartDrvParity parity,
                             ConnxUartDrvStopBits stopBits,
                             bool flowControl,
                             const uint8_t *token)
{
    void *handle = NULL;
    uint8_t dataBits_, parity_, stopBits_;

    dataBits_ = (dataBits == CONNX_UART_DRV_DATA_BITS_8) ? 8 : 7;
    parity_ = (parity == CONNX_UART_DRV_PARITY_EVEN) ? 2 : 0;
    stopBits_ = (stopBits == CONNX_UART_DRV_STOP_BITS_2) ? 2 : 1;

    handle = ConnxUartDrvOpen(device, &baud, dataBits_, parity_, stopBits_, flowControl, token);

    if (handle != NULL)
    {
        instResetBaud = resetBaud;
    }

    DPRINTF(("device: %s, baud: %d, resetBaud: %d, dataBits: %d, parity: %d, stopBits: %d, flowControl: %x\n",
             device, baud, resetBaud, dataBits_, parity_, stopBits_, flowControl));

    return handle;
}

void ConnxUartDrvDeinitialise(void *handle)
{
    CONNX_UNUSED(handle);

    instInUse = false;
}

uint32_t ConnxUartDrvBaudrateActualGet(uint32_t wantedBaudrate)
{
    return wantedBaudrate;
}
