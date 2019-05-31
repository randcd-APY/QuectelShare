/*****************************************************************************

Copyright (c) 2008-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary. 

*****************************************************************************/
#include <stdarg.h>
#include <linux/version.h>

#include "csr_synergy.h"

#ifdef CSR_UART_DEBUG
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif


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

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_serial_com.h"
#include "csr_transport.h"
#include "csr_util.h"

#include "platform/csr_serial_init.h"

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
#ifndef MSM_ENABLE_UART_CLOCK
#define MSM_ENABLE_UART_CLOCK      0x5441  /* TIOCPMGET */
#endif
#ifndef MSM_DISABLE_UART_CLOCK
#define MSM_DISABLE_UART_CLOCK     0x5442  /* TIOCPMPUT */
#endif
#endif

#define QUEUE_SIZE             (9000)

static CsrUint8 UART0_RX_QUEUE[QUEUE_SIZE];
static CsrUint32 UART0_RX_in_PTR;
static CsrUint32 UART0_RX_out_PTR;
static CsrUint32 UART0_RX_numbers;

static pthread_mutex_t receiveMutex;
static pthread_t worker_thread;

/* Designates the reader and writer fds in a pipe(2) array. */
#define PIPE_READ   0
#define PIPE_WRITE  1

/* This maps the file descriptors into the pollfd struture. */
#define FD_UART     0
#define FD_CONTROL  1

static struct pollfd pfd[2];
static int control;

static CsrBool instInUse;
static CsrBool instStarted;
static CsrCharString instDevice[128];
static CsrUint32 curBaud;
static CsrUint32 instBaud;
static CsrUint32 instResetBaud;
static CsrUartDrvDataBits instDataBits;
static CsrUartDrvParity instParity;
static CsrUartDrvStopBits instStopBits;
static CsrBool instFlowControl;
static CsrUint8 instToken;
static CsrBool instUseToken;

static CsrSchedBgint rxBgint = CSR_SCHED_BGINT_INVALID;
static CsrUartDrvDataRx rxData = NULL;

static speed_t baudrate_map(CsrUint32 baud)
{
    switch (baud)
    {
        case 110:
            return B110;
        case 300:
            return B300;
        case 1200:
            return B1200;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
#ifdef B230400
        case 230400:
            return B230400;
#endif
#ifdef B460800
        case 460800:
            return B460800;
#endif
#ifdef B500000
        case 500000:
            return B500000;
#endif
#ifdef B576000
        case 576000:
            return B576000;
#endif
#ifdef B921600
        case 921600:
            return B921600;
#endif
#ifdef B1000000
        case 1000000:
            return B1000000;
#endif
#ifdef B1152000
        case 1152000:
            return B1152000;
#endif
#ifdef B1500000
        case 1500000:
            return B1500000;
#endif
#ifdef B2000000
        case 2000000:
            return B2000000;
#endif
#ifdef B2500000
        case 2500000:
            return B2500000;
#endif
#ifdef B3000000
        case 3000000:
            return B3000000;
#endif
#ifdef B3500000
        case 3500000:
            return B3500000;
#endif
#ifdef B4000000
        case 4000000:
            return B4000000;
#endif
        default:
            break;
    }

    return B0;
}

/* THREAD FUNCTIONS */

static void *workerThread(void *threadParameter)
{
    int running = 1;

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
                CsrBool tokenDetected = FALSE;

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

                iocount = CSRMIN(iocount, (int) (QUEUE_SIZE - UART0_RX_in_PTR));
                iocount = CSRMIN(iocount, bufferSpace);

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
                                tokenDetected = TRUE;
                                break;
                            }
                        }
                    }

                    UART0_RX_in_PTR += bytesRead;

                    if (UART0_RX_in_PTR >= QUEUE_SIZE)
                    {
                        UART0_RX_in_PTR = 0;
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
                    if (!instUseToken)
                    {
                        DPRINTF(("BGINT\n"));
                        CsrSchedBgintSet(rxBgint);
                    }
                    else if (instUseToken && tokenDetected)
                    {
                        DPRINTF(("TOKEN\n"));
                        CsrSchedBgintSet(rxBgint);
                    }
                }
                else
                {
                    bytesRead = 0;
                }
            }
        }
    }

    DPRINTF(("THREAD STOP\n"));
    return NULL;
}

void *CsrUartDrvOpen(const CsrCharString *device,
    CsrUint32 *baud,
    CsrUint8 dataBits,
    CsrUint8 parity,
    CsrUint8 stopBits,
    CsrBool flowControl,
    const CsrUint8 *token)
{
    if (!instInUse && (baudrate_map(*baud) != B0))
    {
        instInUse = TRUE;

        curBaud = 0;
        instBaud = instResetBaud = *baud;
        instDataBits = dataBits;
        instParity = parity;
        instStopBits = stopBits;
        instFlowControl = flowControl;

        if (token != NULL)
        {
            instUseToken = TRUE;
            instToken = *token;
        }
        else
        {
            instUseToken = FALSE;
        }

        CsrStrNCpyZero(instDevice, device, sizeof(instDevice));

        return &instInUse;
    }

    return NULL;
}

void CsrUartDrvClose(void *handle)
{
    instInUse = FALSE;
}

static CsrBool uartDeviceConfigure(int fd, speed_t speed)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0)
    {
        perror("uartDeviceConfigure: tcgetattr failed");
        return FALSE;
    }

    if ((cfsetispeed(&options, speed) != 0) ||
        (cfsetospeed(&options, speed) != 0))
    {
        perror("uartDeviceConfigure: set baud rate failed");
        return FALSE;
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
        return FALSE;
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
        return FALSE;
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
        return FALSE;
    }

    if (instFlowControl)
    {
        options.c_cflag |= CRTSCTS;
    }
    else
    {
        options.c_cflag &= ~CRTSCTS;
        options.c_cflag |= CLOCAL;
    }

    options.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("uartDeviceConfigure: tcsetattr failed");
        return FALSE;
    }

    return TRUE;
}

CsrBool CsrUartDrvReconfigure(void *handle, CsrUint32 *baud, CsrBool flowControl)
{
    speed_t speed = baudrate_map(*baud);

    if (speed == B0)
    {
        return FALSE;
    }

    instBaud = instResetBaud = *baud;
    instFlowControl = flowControl;

    if (instStarted)
    {
        return uartDeviceConfigure(pfd[FD_UART].fd, speed);
    }

    return TRUE;
}

CsrBool CsrUartDrvStart(void *handle, CsrUint8 reset)
{
    int ret, fd;
    static int controlPipe[2];
    speed_t speed;

    DPRINTF(("UARTDRIVER START\n"));

    if (reset == 0)
    {
        curBaud = instBaud;
    }
    else
    {
        curBaud = instResetBaud;
    }

    fd = open(instDevice, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        perror("CsrUartDrvStart: device open failed");
        return FALSE;
    }
    if (fcntl(fd, F_SETFL, 0) != 0)
    {
        perror("CsrUartDrvStart: fcntl failed");
        return FALSE;
    }

    speed = baudrate_map(curBaud);
    if (speed == B0)
    {
        fprintf(stderr, "CsrUartDrvStart: unsupported baud rate: %d\n", curBaud);
        return FALSE;
    }

    if (!uartDeviceConfigure(fd, speed))
    {
        fprintf(stderr, "CsrUartDrvStart: device configuration failed\n");
        return FALSE;
    }

    if (pipe(controlPipe) != 0)
    {
        perror("CsrUartDrvStart: create control pipe failed");
        close(fd);

        return FALSE;
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
        perror("CsrUartDrvStart: pthread_mutex_init failed");
        close(controlPipe[0]);
        close(controlPipe[1]);
        close(fd);

        return FALSE;
    }

    ret = pthread_create(&worker_thread, NULL, workerThread, NULL);
    if (ret != 0)
    {
        perror("CsrUartDrvStart: pthread_create failed");
        close(controlPipe[0]);
        close(controlPipe[1]);
        close(fd);

        pthread_mutex_destroy(&receiveMutex);

        return FALSE;
    }

    instStarted = TRUE;
    return TRUE;
}

CsrBool CsrUartDrvStop(void *handle)
{
    void *thread_res;
    char stop = 1;

    DPRINTF(("UARTDRIVER STOP\n"));

    if (!instStarted)
    {
        return TRUE;
    }
    instStarted = FALSE;

    /* Stop worker thread */
    if (write(control, &stop, 1) != 1)
    {
        perror("CsrUartDrvStop: could not signal stop");
        return FALSE;
    }
    else
    {
        DPRINTF(("STOPPING THREAD\n"));
        if (pthread_join(worker_thread, &thread_res) != 0)
        {
            perror("CsrUartDrvStop: worker thread join error");
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

    return TRUE;
}

void CsrUartDrvRtsSet(void *handle, CsrBool state)
{
    int bit = TIOCM_RTS;
    if (ioctl(pfd[FD_UART].fd, state ? TIOCMBIS : TIOCMBIC, &bit) == -1)
    {
        perror("CsrUartDrvRtsSet(): ioctl failed");
    }
}

CsrUint32 CsrUartDrvGetTxSpace(void *handle)
{
    return QUEUE_SIZE;
}

CsrBool CsrUartDrvTx(void *handle, const CsrUint8 *data, CsrUint32 dataLength, CsrUint32 *numSent)
{
    ssize_t res;
    CsrBool rv = FALSE;

    DPRINTF(("TX %u\n", dataLength));

    res = write(pfd[FD_UART].fd, data, dataLength);

    if (res >= 0)
    {
        *numSent = res;
        if ((CsrUint32) res == dataLength)
        {
            DPRINTF(("TX COMPLETE\n"));
            rv = TRUE;
        }
    }
    else
    {
        DPRINTF(("TX FAILURE\n"));
        *numSent = 0;
    }

    return rv;
}

CsrUint32 CsrUartDrvGetRxAvailable(void *handle)
{
    CsrUint32 rxBytes;

    pthread_mutex_lock(&receiveMutex);
    rxBytes = UART0_RX_numbers;
    pthread_mutex_unlock(&receiveMutex);

    return QUEUE_SIZE - rxBytes;
}

void CsrUartDrvRx(void *handle)
{
    CsrUint32 bytesConsumed;
    CsrUint32 noOfBytes;
    CsrBool reschedule = FALSE;

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
        reschedule = TRUE;
    }
    pthread_mutex_unlock(&receiveMutex);

    if (reschedule)
    {
        char stop = 0;
        ssize_t res = write(control, &stop, 1);
        CSR_UNUSED(res);
    }

    /* If buffer is not empty, reschedule bgint. */
    if (noOfBytes > 0)
    {
        DPRINTF(("BGINT\n"));
        CsrSchedBgintSet(rxBgint);
    }
}

CsrUint32 CsrUartDrvLowLevelTransportTxBufLevel(void *handle)
{
    return 0;
}

CsrUint32 CsrUartDrvLowLevelTransportRx(void *handle, CsrUint32 dataLength, CsrUint8 *data)
{
    CsrUint32 rxCount, rxLeft;
    CsrBool reschedule = FALSE;

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
    if ((pfd[FD_UART].events & POLLIN) == 0)
    {
        pfd[FD_UART].events |= POLLIN;
        reschedule = TRUE;
    }
    pthread_mutex_unlock(&receiveMutex);

    if (reschedule)
    {
        char stop = 0;
        ssize_t res = write(control, &stop, 1);
        CSR_UNUSED(res);
    }

    /* If buffer is not empty initialize a new bg_int */
    if (rxLeft > 0)
    {
        CsrSchedBgintSet(rxBgint);
    }
    return rxCount;
}

CsrBool CsrUartIsInitBaudOverrideSupported(void *handle)
{
    return FALSE;
}

CsrUint32 CsrUartDrvGetBaudrate(void *handle)
{
    return curBaud;
}

CsrUint32 CsrUartDrvGetResetBaudrate(void *handle)
{
    return instResetBaud;
}

void CsrUartDrvSetResetBaudrate(CsrUint32 baud)
{
    instResetBaud = baud;
}

void CsrUartDrvRegister(void *handle, CsrUartDrvDataRx rxDataFn, CsrSchedBgint rxBgintHdl)
{
    rxData = rxDataFn;
    rxBgint = rxBgintHdl;
}

/* DEPRECATED: This function is provided for backward compatibility, please
   use CsrUartDrvOpen in new code. */
void *CsrUartDrvInitialise(const CsrCharString *device,
    CsrUint32 baud, CsrUint32 resetBaud,
    CsrUartDrvDataBits dataBits,
    CsrUartDrvParity parity,
    CsrUartDrvStopBits stopBits,
    CsrBool flowControl,
    const CsrUint8 *token)
{
    void *handle = NULL;
    CsrUint8 dataBits_, parity_, stopBits_;

    dataBits_ = (dataBits == CSR_UART_DRV_DATA_BITS_8) ? 8 : 7;
    parity_ = (parity == CSR_UART_DRV_PARITY_EVEN) ? 2 : 0;
    stopBits_ = (stopBits == CSR_UART_DRV_STOP_BITS_2) ? 2 : 1;

    handle = CsrUartDrvOpen(device, &baud, dataBits_, parity_, stopBits_, flowControl, token);

    if (handle != NULL)
    {
        instResetBaud = resetBaud;
    }

    return handle;
}

/* DEPRECATED: This function is provided for backward compatibility, please
   use CsrUartDrvClose in new code */
void CsrUartDrvDeinitialise(void *handle)
{
    instInUse = FALSE;
}

CsrUint32 CsrUartDrvBaudrateActualGet(CsrUint32 wantedBaudrate)
{
    return wantedBaudrate;
}

void CsrUartDrvEnableClock(void *handle, CsrBool enable)
{
#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
    CsrInt32 fd;
    CsrInt32 rc;

    CSR_UNUSED(handle);

    fd = open(instDevice, O_RDWR | O_NOCTTY);

    if (fd == -1)
    {
        DPRINTF(("Fail to open '%s', errno = %d\n", instDevice, errno));
        return;
    }

    if (enable)
    {
        /* Enable UART clock. */
        rc = ioctl(fd, MSM_ENABLE_UART_CLOCK);
    }
    else
    {
        /* Disable UART clock. */
        rc = ioctl(fd, MSM_DISABLE_UART_CLOCK);
    }

    if (rc == -1)
    {
        DPRINTF(("Fail to control UART clock, errno = %d (%s)\n", errno, strerror(errno)));
    }

    close(fd);
#endif
}
