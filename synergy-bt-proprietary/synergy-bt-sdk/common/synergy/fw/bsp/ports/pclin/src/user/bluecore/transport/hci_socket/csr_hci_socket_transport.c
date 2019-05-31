/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* For usleep() */
#endif
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <pthread.h>

#include <poll.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#if defined(USE_MDM_PLATFORM) || defined(USE_IMX_PLATFORM)
#include <bluetooth.h>
#include <hci.h>
#include <hci_lib.h>
#else
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif

#include <linux/version.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_transport.h"
#include "csr_tm_bluecore_transport.h"
#include "csr_tm_bluecore_private_lib.h"
#include "csr_arg_search.h"

#ifdef CSR_HCI_SOCKET_DEBUG
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif

#ifndef CSR_HCI_SOCKET_BUFSIZE
#define CSR_HCI_SOCKET_BUFSIZE   8192
#endif

#ifndef CSR_HCI_SOCKET_DEVICE
#define CSR_HCI_SOCKET_DEVICE    0
#endif

static int hciDevice = CSR_HCI_SOCKET_DEVICE;

#define PIPE_READ   0
#define PIPE_WRITE  1

/* This maps the file descriptors into the pollfd struture. */
#define FD_HCI_SOCKET    0
#define FD_CONTROL  1

#define OCF_HCI_RESET     (0x0003)
#define OGF_CTRL_BB       (0x03)
#define HCI_RESET_OPCODE  ((CsrUint16) ((OCF_HCI_RESET & 0x03ff) | (OGF_CTRL_BB << 10)))

#define EVENT_COMMAND_COMPLETE 0x0E

static struct pollfd pfd[2];
static int control;

static pthread_t rx_thread_id;
static CsrSchedBgint rxIrq;
static struct iovec rx[2];
static unsigned char chan;
static int waitreset;
static struct pollfd bpfd;
static MessageStructure msg;
static CsrBool isRawChannel = TRUE;

/*
 * Prototypes
 */
static CsrBool CsrTransportHciSocketStart(void);
static CsrBool CsrTransportHciSocketStop(void);
static CsrUint16 CsrTransportHciSocketQuery(void);
static void CsrTransportHciSocketMsgTx(void *arg);
static void CsrTransportHciSocketScoTx(void *scoData);
static CsrBool CsrTransportHciSocketDriverRestart(CsrUint8 unused);
static void CsrTransportHciSocketRestart(void);
static void CsrTransportHciSocketClose(void);

static CsrBool UseHciRawChannel(void);


static struct CsrTmBlueCoreTransport _CsrTransportHciSocketType =
{
    CsrTransportHciSocketStart,
    CsrTransportHciSocketStop,
    CsrTransportHciSocketQuery,
    CsrTransportHciSocketMsgTx,
    NULL, /* msgrx */
    CsrTransportHciSocketScoTx,
    CsrTransportHciSocketDriverRestart,
    CsrTransportHciSocketRestart,
    CsrTransportHciSocketClose,
    FALSE,
};

void *CsrBluecoreTransportHciSocket = &_CsrTransportHciSocketType;

static CsrUint8 nopPacket[] =
{
    0x0e, 0x04, 0x01, 0x00,
    0x00, 0x00
};

static CsrBool UseHciRawChannel(void)
{
    CsrCharString *parameter, *value;

    if (CsrArgSearch(NULL, "--platform", &parameter, &value) && (value != NULL))
    {
        /* MTP9X45 is based on Linux kernel v3.10, which limit that HCI socket in raw mode can only be used. */
        if (!CsrStrCmp(value, "MDM9640"))
            return TRUE;
    }

    return FALSE;
}

static void sendHciResetCmd(void)
{
    struct iovec tx[2];
    unsigned char oper;
    CsrUint8 buf[3];
    size_t bufLen;
    ssize_t res;

    DPRINTF(("SEND HCI RESET COMMAND\n"));

    oper = HCI_COMMAND_PKT;
    buf[0] = (CsrUint8) (HCI_RESET_OPCODE & 0xff);
    buf[1] = (CsrUint8) (HCI_RESET_OPCODE >> 8);
    buf[2] = 0x00; /* No parameters */
    bufLen = sizeof(buf);

    tx[0].iov_base = &oper;
    tx[0].iov_len = sizeof(oper);
    tx[1].iov_base = buf;
    tx[1].iov_len = bufLen;

    res = writev(pfd[FD_HCI_SOCKET].fd, tx, 2);

    DPRINTF(("WROTE %d\n", res));

    CSR_UNUSED(res);
}

static void hciSocketBgint(void *arg)
{
    int rv;
    ssize_t res;
    unsigned char stop = 0;

    DPRINTF(("HCI_SOCKET BGINT\n"));

    do
    {
        res = readv(bpfd.fd, rx, 2);

        DPRINTF(("READV RETURN %d\n", res));

        if (res > 0)
        {
            res -= sizeof(chan);

            msg.buflen = res;

            switch (chan)
            {
                case HCI_ACLDATA_PKT:
                    DPRINTF(("RX ACL PACKET\n"));
                    msg.chan = TRANSPORT_CHANNEL_ACL;
                    break;

                case HCI_EVENT_PKT:
                    DPRINTF(("RX HCI PACKET\n"));
                    msg.chan = TRANSPORT_CHANNEL_HCI;
                    break;

                case HCI_SCODATA_PKT:
                    DPRINTF(("RX SCO PACKET\n"));
                    msg.chan = TRANSPORT_CHANNEL_SCO;
                    break;

                default:
                    DPRINTF(("BOGUS PACKET TYPE %d\n", chan));
                    msg.chan = TRANSPORT_CHANNEL_HCI;
                    break;
            }

            if (waitreset)
            {
                if ((chan == HCI_EVENT_PKT) && (*(msg.buf) == EVENT_COMMAND_COMPLETE))
                {
                    CsrUint8 *buf = msg.buf;
                    CsrUint8 *opcode = buf + HCI_EVENT_HDR_SIZE + 1;

                    if ((opcode[0] == (CsrUint8) (HCI_RESET_OPCODE & 0xff)) &&
                        (opcode[1] == (CsrUint8) (HCI_RESET_OPCODE >> 8)))
                    {
                        /*
                         * Use number of HCI packets from
                         * COMMAND COMPLETE event as value
                         * in fake NOP event.
                         */
                        nopPacket[2] = *(buf + HCI_EVENT_HDR_SIZE);
                        msg.buf = nopPacket;
                        msg.buflen = sizeof(nopPacket);

                        CsrTmBlueCoreTransportMsgRx(CsrBluecoreTransportHciSocket, &msg);

#if defined(USE_MDM_PLATFORM) || defined(USE_IMX_PLATFORM)
                        /* [QTI] Simulate the 2nd HCI_NOP to make Synergy TM activated fully. */
                        CsrTmBlueCoreTransportMsgRx(CsrBluecoreTransportHciSocket, &msg);
#endif

                        /*
                         * Switch back to correct buffer.
                         * Stop waiting for reset complete.
                         */
                        msg.buf = buf;
                        waitreset = 0;
                    }
                    else
                    {
                        DPRINTF(("Discard HCI Event Completed, opcode: 0x%02x%02x\n", opcode[1], opcode[0]));
                    }
                }
                else
                {
                    /* Discard all packets but reset complete. */
                    DPRINTF(("Discard HCI Event: 0x%02x\n", *(msg.buf)));
                }
            }
            else
            {
                CsrTmBlueCoreTransportMsgRx(CsrBluecoreTransportHciSocket, &msg);
            }
        }

        /*
         * Check if there is more data that can be
         * read without blocking.  If there is an
         * error, we will break out of this loop,
         * and the reader thread will handle it.
         */
        rv = poll(&bpfd, 1, 0);
    } while ((rv > 0) && (bpfd.revents & POLLIN));

    /* Re-enable polling in reader thread. */
    pfd[FD_HCI_SOCKET].events = POLLIN;

    /*
     * Request the reader thread to reschedule
     * to start waiting for data on the BlueZ
     * socket.
     *
     * If write() fails, we try to limb on.
     */
    res = write(control, &stop, sizeof(stop));

    CSR_UNUSED(res);
}

static void *hciSocketReaderThread(void *arg)
{
    int running;

    DPRINTF(("THREAD STARTED\n"));

    /* Reset controller state. */
    sendHciResetCmd();

    /* Start running the thread. */
    running = 1;

    /* Wait for reset completion. */
    waitreset = 1;

    while (running)
    {
        int ready;

        DPRINTF(("POLL: 0x%x 0x%x\n", pfd[FD_HCI_SOCKET].events,
                 pfd[FD_CONTROL].events));
        /*
         * We wait for POLLIN events on the file descriptors
         * indefinitely since we are only reading from this
         * thread.  We check for error events before processing
         * POLLIN events.
         */
        ready = poll(pfd, 2, -1);

        DPRINTF(("ready %d 0x%x 0x%x\n", ready,
                 pfd[FD_HCI_SOCKET].revents, pfd[FD_CONTROL].revents));

        if (ready > 0)
        {
            if (pfd[FD_HCI_SOCKET].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                DPRINTF(("ERROR FD_HCI_SOCKET FAILED\n"));
                running = 0;
            }

            if (pfd[FD_CONTROL].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                DPRINTF(("ERROR FD_CONTROL FAILED\n"));
                running = 0;
            }

            if (pfd[FD_CONTROL].revents & POLLIN)
            {
                ssize_t res;
                unsigned char stop;

                res = read(pfd[FD_CONTROL].fd, &stop, sizeof(stop));
                if (res != sizeof(stop))
                {
                    DPRINTF(("CONTROL ERROR\n"));
                }
                else if (stop == 1)
                {
                    /* This is a termination request. */
                    DPRINTF(("STOP RECEIVED\n"));
                    running = 0;
                }
                else
                {
                    /* This is a rescheduling request. */
                }
            }

            if (pfd[FD_HCI_SOCKET].revents & POLLIN)
            {
                DPRINTF(("FD_HCI_SOCKET POLLIN\n"));
                /*
                 * Don't poll on the BlueZ socket until the bgint
                 * has been executed which will re-enable polling again.
                 */
                pfd[FD_HCI_SOCKET].events = 0;
                CsrSchedBgintSet(rxIrq);
            }
        }
    }

    DPRINTF(("THREAD TERMINATING\n"));

    return NULL;
}

void CsrTransportHciSocketConfigure(const char *device)
{
    int devno;

    if ((sscanf(device, "hci%d", &devno) == 1) ||
        (sscanf(device, "%d", &devno) == 1))
    {
        hciDevice = devno;
    }
}

static CsrBool CsrTransportHciSocketStart(void)
{
    int sock;
    int res;
    int attempts;
    struct sockaddr_hci sa;
    struct hci_filter flt;
    int controlPipe[2];

    isRawChannel = UseHciRawChannel();

    rxIrq = CsrSchedBgintReg(hciSocketBgint, NULL, "hci_socket rx");
    if (rxIrq == CSR_SCHED_BGINT_INVALID)
    {
        DPRINTF(("NO FREE BGINTS\n"));

        return FALSE;
    }

    rx[0].iov_base = &chan;
    rx[0].iov_len = sizeof(chan);
    rx[1].iov_base = CsrPmemAlloc(CSR_HCI_SOCKET_BUFSIZE);
    rx[1].iov_len = CSR_HCI_SOCKET_BUFSIZE;

    sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);

    if (sock == -1)
    {
        DPRINTF(("SOCKET FAILED, errno=%d\n", errno));
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }

    DPRINTF(("TRYING TO SET DEVICE DOWN\n"));

    for (attempts = 100; attempts > 0; attempts--)
    {
        res = ioctl(sock, HCIDEVDOWN, hciDevice);
        if (res == 0)
        {
            break;
        }
        else
        {
            usleep(10 * 1000);
        }
    }

    if ((attempts == 0) && (res != 0))
    {
        DPRINTF(("IOCTL HCIDEVDOWN FAILED, dev %u, errno=%d\n", hciDevice, errno));
        close(sock);
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }
    else
    {
        DPRINTF(("DEVICE SET DOWN\n"));
    }

    memset(&flt, 0, sizeof(flt));

    flt.type_mask |= (1 << HCI_ACLDATA_PKT);
    flt.type_mask |= (1 << HCI_SCODATA_PKT);
    flt.type_mask |= (1 << HCI_EVENT_PKT);
    flt.event_mask[0] = ~0L;
    flt.event_mask[1] = ~0L;

    res = setsockopt(sock, SOL_HCI, HCI_FILTER, &flt, sizeof(flt));
    if (res == -1)
    {
        DPRINTF(("setsockopt SOL_HCI HCI_FILTER FAILED, errno=%d\n", errno));
        close(sock);
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }

    memset(&sa, 0, sizeof(sa));
    sa.hci_family = AF_BLUETOOTH;
    sa.hci_dev = hciDevice;

    if (isRawChannel)
    {
        /* MTP9X45 is based on Linux kernel v3.10, which limit to utilize HCI raw channel. */
        sa.hci_channel = HCI_CHANNEL_RAW;
    }
    else
    {
        /* MTP9X50/MTP9X07 are based on Linux kernel v3.18 or higher, which can utilize HCI user channel. */
        sa.hci_channel = HCI_CHANNEL_USER;
    }

    if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) != 0)
    {
        DPRINTF(("UNABLE TO BIND TO DEVICE %u, errno=%d\n", hciDevice, errno));
        close(sock);
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }

    /* [QTI] BlueZ in MDM 9x50 doesn't support HCISETRAW. But BlueZ hciattach 
       can be started up with the input parametr "-r" to set "HCI_RAW". This can
       avoid BlueZ handling HCI_EVENT internally. At the same time, "HCIDEVUP"
       should not be called. */
    if (isRawChannel)
    {
        /* Switch to RAW mode to make BlueZ leave the device alone. */
        if (ioctl(sock, HCISETRAW, 1) < 0)
        {
            DPRINTF(("IOCTL HCISETRAW FAILED, dev %u, errno=%d\n", hciDevice, errno));
            close(sock);
            CsrSchedBgintUnreg(rxIrq);
            CsrPmemFree(rx[1].iov_base);
            return FALSE;
        }
        
        res = ioctl(sock, HCIDEVUP, hciDevice);
        if (res != 0)
        {
            DPRINTF(("IOCTL HCIDEVUP FAILED, dev %u, errno=%d\n", hciDevice, errno));
            close(sock);
            CsrSchedBgintUnreg(rxIrq);
            CsrPmemFree(rx[1].iov_base);
            return FALSE;
        }
        else
        {
            DPRINTF(("DEVICE SET UP\n"));
        }
    }

    /* Create pipe used for signalling stop to the thread. */
    if (pipe(controlPipe) != 0)
    {
        DPRINTF(("UNABLE TO CREATE PIPE, errno=%d\n", errno));
        close(sock);
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }

    /*
     * Set up poll structure to provide read events
     * from the BlueZ fd and reader pipe.
     */
    pfd[FD_HCI_SOCKET].fd = sock;
    pfd[FD_HCI_SOCKET].events = POLLIN;
    pfd[FD_CONTROL].fd = controlPipe[PIPE_READ];
    pfd[FD_CONTROL].events = POLLIN;

    control = controlPipe[PIPE_WRITE];

    /* Set message buffer and fixed fields. */
    msg.dex = 0;
    msg.buf = rx[1].iov_base;

    /* Enable polling in bgint handler. */
    bpfd = pfd[FD_HCI_SOCKET];
    bpfd.events = POLLIN;

    DPRINTF(("STARTING HCI_SOCKET THREAD\n"));

    res = pthread_create(&rx_thread_id, NULL, hciSocketReaderThread, NULL);
    if (res != 0)
    {
        DPRINTF(("UNABLE TO START HCI_SOCKET THREAD\n"));
        close(sock);
        close(controlPipe[PIPE_READ]);
        close(controlPipe[PIPE_WRITE]);
        CsrSchedBgintUnreg(rxIrq);
        CsrPmemFree(rx[1].iov_base);
        return FALSE;
    }

    return TRUE;
}

static CsrBool CsrTransportHciSocketStop(void)
{
    void *thread_res;
    int ret;
    ssize_t res;
    unsigned char stop = 1;

    /* Stop reader thread */
    res = write(control, &stop, 1);

    CSR_UNUSED(res);

    ret = pthread_join(rx_thread_id, &thread_res);
    if (ret != 0)
    {
        perror("CsrTransportHciSocketStop: reader thread join error");
    }

    close(pfd[FD_HCI_SOCKET].fd);
    close(pfd[FD_CONTROL].fd);
    close(control);

    CsrSchedBgintUnreg(rxIrq);
    CsrPmemFree(rx[1].iov_base);

    return TRUE;
}

static CsrUint16 CsrTransportHciSocketQuery(void)
{
    return TRANSPORT_TYPE_USB;
}

static void transportHciSocketTx(void *buf, size_t bufLen, CsrUint8 chan)
{
    ssize_t res;
    struct iovec tx[2];
    unsigned char oper;

    DPRINTF(("transportHciSocketTx %p len %u chan %u\n",
             buf, bufLen, chan));

    switch (chan)
    {
        case TRANSPORT_CHANNEL_ACL:
            oper = HCI_ACLDATA_PKT;
            break;

        case TRANSPORT_CHANNEL_HCI:
            oper = HCI_COMMAND_PKT;
            break;

        case TRANSPORT_CHANNEL_SCO:
            oper = HCI_SCODATA_PKT;
            break;
    }

    tx[0].iov_base = &oper;
    tx[0].iov_len = sizeof(oper);
    tx[1].iov_base = buf;
    tx[1].iov_len = bufLen;

    res = writev(pfd[FD_HCI_SOCKET].fd, tx, 2);

    DPRINTF(("WROTE %d\n", res));

    CSR_UNUSED(res);

    CsrPmemFree(buf);
}

static void CsrTransportHciSocketMsgTx(void *arg)
{
    MessageStructure *txMsg;
    TXMSG *msg;

    DPRINTF(("CsrTransportHciSocketMsgTx %p\n", arg));
    msg = (TXMSG *) arg;

    txMsg = (MessageStructure *) &msg->m;
    transportHciSocketTx(txMsg->buf, txMsg->buflen, msg->chan);
    CsrPmemFree(msg);
}

static void CsrTransportHciSocketScoTx(void *scoData)
{
    CsrUint32 scoLen;

    scoLen = (CsrUint32) (((CsrUint8 *) scoData)[2] + 3);

    transportHciSocketTx(scoData, scoLen, TRANSPORT_CHANNEL_SCO);
}

static CsrBool CsrTransportHciSocketDriverRestart(CsrUint8 unused)
{
    DPRINTF(("CsrTransportHciSocketDriverRestart %u\n", unused));

    if (CsrTransportHciSocketStop())
    {
        if (CsrTransportHciSocketStart())
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void CsrTransportHciSocketRestart(void)
{
}

static void CsrTransportHciSocketClose(void)
{
    CsrTmBluecoreResetIndSend();
}

void CsrTmBlueCoreHciSocketInit(void **gash)
{
    CsrTmBlueCoreTransportInit(gash, CsrBluecoreTransportHciSocket);
}
