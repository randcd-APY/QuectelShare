/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#ifdef ETHER_DEBUG
#include <stdio.h>
#endif

#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include <fcntl.h>

#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_tun.h>

#include "csr_types.h"
#include "csr_result.h"
#include "csr_log_text_2.h"

#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#include "csr_ip_ether_prim.h"
#include "csr_ip_ether_lib.h"

#include "csr_ip_ether_sef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ETHER_DEBUG
static void frameDump(CsrUint8 *destinationMac, CsrUint8 *sourceMac,
    CsrUint16 frameLength, CsrUint8 *frame, CsrBool tx)
{
    int i;

    printf("Stack %sing frame of size %u.\n", tx ? "transmitting" : "receiving",
        frameLength + 12);
    printf("%02x:%02x:%02x:%02x:%02x:%02x"
        " -> "
        "%02x:%02x:%02x:%02x:%02x:%02x",
        sourceMac[0],
        sourceMac[1],
        sourceMac[2],
        sourceMac[3],
        sourceMac[4],
        sourceMac[5],
        destinationMac[0],
        destinationMac[1],
        destinationMac[2],
        destinationMac[3],
        destinationMac[4],
        destinationMac[5]);

    for (i = 0;
         i < frameLength && i < 64;
         i++)
    {
        if ((i % 8) == 0)
        {
            printf("\n\t");
        }

        printf("%02x", frame[i]);

        if ((i % 2) == 1)
        {
            printf(" ");
        }
    }

    if (frameLength >= 64)
    {
        printf("\n\t...\n");
    }

    printf("\n");
}

#endif

static void etherFrameReceive(CsrUint8 *destinationMac, CsrUint8 *sourceMac,
    CsrInt16 rssi, CsrUint16 frameLength, CsrUint8 *frame,
    CsrIpEtherIpContext ipContext)
{
    CsrIpEtherInst *etherInstance;
    struct iovec iov[3];
    ssize_t res;

    etherInstance = (CsrIpEtherInst *) ipContext;

#ifdef ETHER_DEBUG
    frameDump(destinationMac, sourceMac, frameLength, frame, FALSE);
#endif

    iov[0].iov_base = destinationMac;
    iov[0].iov_len = 6;
    iov[1].iov_base = sourceMac;
    iov[1].iov_len = 6;
    iov[2].iov_base = frame;
    iov[2].iov_len = frameLength;

    /* Ignore failure because we can't do anything about it. */
    res = writev(etherInstance->fd, iov, 3);
    CSR_UNUSED(res);
}

static CsrBool tunAlloc(CsrIpEtherInst *etherInstance, CsrUint8 mac[6])
{
    CsrBool rv;
    int fd;

    fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0)
    {
        rv = FALSE;
    }
    else
    {
        int err;
        struct ifreq ifr;

        CsrMemSet(&ifr, 0, sizeof(ifr));

        ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

        if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
        }

        err = ioctl(fd, TUNSETIFF, (void *) &ifr);
        if (err < 0)
        {
            close(fd);
            rv = FALSE;
        }
        else
        {
            CsrMemCpy(&ifr.ifr_hwaddr.sa_data, mac, 6);
            ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

            if (ioctl(fd, SIOCSIFHWADDR, &ifr) < 0)
            {
                close(fd);
                rv = FALSE;
            }
            else
            {
                etherInstance->fd = fd;
                rv = TRUE;
            }
        }
    }

    return rv;
}

static void tunFree(CsrIpEtherInst *etherInstance)
{
    close(etherInstance->fd);
}

/*
 * CsrIpEtherInst utility functions
 */
static CsrIpEtherInst *etherAlloc(CsrIpInstanceData *instanceData,
    CsrUint32 *ifHandle)
{
    CsrIpEtherInst *etherInstance;
    CsrUint32 idx;

    for (idx = 0; idx < CSR_IP_MAX_ETHERS; idx++)
    {
        if (instanceData->etherInstance[idx] == NULL)
        {
            break;
        }
    }

    if (idx < CSR_IP_MAX_ETHERS)
    {
        etherInstance = CsrPmemAlloc(sizeof(*etherInstance));

        instanceData->etherInstance[idx] = etherInstance;
        etherInstance->inst = instanceData;
        etherInstance->reap = 0;
        *ifHandle = idx;
    }
    else
    {
        etherInstance = NULL;
    }

    return etherInstance;
}

/*
 * Internal interface
 */
void csrIpEtherFree(CsrIpInstanceData *instanceData, CsrUint32 ifHandle)
{
    CsrIpEtherInst *etherInstance;

    etherInstance = instanceData->etherInstance[ifHandle];

    tunFree(etherInstance);
    CsrPmemFree(etherInstance);

    instanceData->etherInstance[ifHandle] = NULL;
}

void csrIpEtherRemove(CsrIpInstanceData *instanceData, CsrUint32 ifHandle)
{
    CsrIpEtherInst *etherInstance;
    CsrIpEtherIfRemoveCfm *confirm;
    CsrSchedQid qid;

    etherInstance = instanceData->etherInstance[ifHandle];

    qid = etherInstance->qid;

    csrIpEtherFree(instanceData, ifHandle);

    confirm = CsrIpEtherIfRemoveCfm_struct(ifHandle, CSR_RESULT_SUCCESS);

    CsrIpMessageForward(qid, CSR_IP_ETHER_PRIM, confirm);
}

void csrIpEtherHandleEthers(CsrIpInstanceData *instanceData,
    int *ready, fd_set *rset)
{
    int i, readyCount;

    for (i = 0, readyCount = *ready;
         (readyCount > 0) && (i < CSR_IP_MAX_ETHERS);
         i++)
    {
        CsrIpEtherInst *etherInstance;

        etherInstance = instanceData->etherInstance[i];

        if ((etherInstance != NULL) && FD_ISSET(etherInstance->fd, rset))
        {
            ssize_t dataLen;

            readyCount--;

            dataLen = read(etherInstance->fd, instanceData->rxBuffer,
                IP_RX_BUFFER_SIZE);

            if (dataLen > 0)
            {
                CsrUint8 *frame;

                frame = instanceData->rxBuffer;

#ifdef ETHER_DEBUG
                frameDump(&frame[0], &frame[6], dataLen - 12, &frame[12],
                    TRUE);
#endif

                (void) etherInstance->frameTxFunction(&frame[0], &frame[6],
                    dataLen - 12, &frame[12], etherInstance->ifContext);
            }
            else
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                      "ether%u: read() failed: %d\n", i, dataLen));
            }
        }
    }

    *ready = readyCount;
}

/*
 * Message handlers follow
 */

void CsrIpEtherIfAddReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfAddReq *request;
    CsrIpEtherInst *etherInstance;
    CsrUint32 ifHandle;
    CsrResult rv;
    CsrIpEtherEncapsulation encapsulation;
    CsrIpEtherFrameRxFunction frameRxFunction;
    CsrIpEtherIpContext ipContext;

    request = instanceData->msg;

    CsrIpLock(instanceData);

    etherInstance = etherAlloc(instanceData, &ifHandle);

    if (etherInstance != NULL)
    {
        if (tunAlloc(etherInstance, request->mac))
        {
            etherInstance->qid = request->appHandle;
            etherInstance->ifType = request->ifType;
            etherInstance->ifContext = request->ifContext;
            etherInstance->maxTxUnit = request->maxTxUnit;
            etherInstance->encapsulation = request->encapsulation;
            etherInstance->frameTxFunction = request->frameTxFunction;
            CsrMemCpy(etherInstance->mac, request->mac,
                sizeof(etherInstance->mac));

            encapsulation = CSR_IP_ETHER_ENCAPSULATION_ETHERNET;
            frameRxFunction = etherFrameReceive;
            ipContext = etherInstance;
            rv = CSR_RESULT_SUCCESS;
            CsrIpFdNew(instanceData, etherInstance->fd);
        }
        else
        {
            csrIpEtherFree(instanceData, ifHandle);

            ifHandle = 0;
            encapsulation = 0;
            frameRxFunction = NULL;
            ipContext = NULL;
            rv = CSR_IP_ETHER_RESULT_NO_MORE_INTERFACES;
        }
    }
    else
    {
        ifHandle = 0;
        encapsulation = 0;
        frameRxFunction = NULL;
        ipContext = NULL;
        rv = CSR_IP_ETHER_RESULT_NO_MORE_INTERFACES;
    }

    CsrIpUnlock(instanceData);

    CsrIpEtherIfAddCfmSend(request->appHandle, ifHandle,
        rv, encapsulation, frameRxFunction, ipContext);
}

void CsrIpEtherIfRemoveReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfRemoveReq *request;

    request = instanceData->msg;

    if (request->ifHandle < CSR_IP_MAX_ETHERS)
    {
        CsrIpLock(instanceData);

        if (instanceData->etherInstance[request->ifHandle] != NULL)
        {
            CsrIpEtherInst *etherInstance;

            etherInstance = instanceData->etherInstance[request->ifHandle];

            etherInstance->reap = 1;
            CsrIpFdRemove(instanceData, etherInstance->fd, TRUE, FALSE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "ether%u: remove: interface not found\n", request->ifHandle));
        }

        CsrIpUnlock(instanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "ether%u: remove: interface not found\n", request->ifHandle));
    }
}

void CsrIpEtherIfLinkUpReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfLinkUpReq *request;

    request = instanceData->msg;

    if (request->ifHandle < CSR_IP_MAX_ETHERS)
    {
        CsrIpLock(instanceData);

        if (instanceData->etherInstance[request->ifHandle] != NULL)
        {
            CsrIpEtherInst *etherInstance;

            etherInstance = instanceData->etherInstance[request->ifHandle];

            CsrIpFdAdd(instanceData, etherInstance->fd, TRUE, FALSE);
            CsrIpEtherIfLinkUpCfmSend(etherInstance->qid, request->ifHandle,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "ether%u: link up: interface not found\n", request->ifHandle));
        }

        CsrIpUnlock(instanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "ether%u: link up: interface not found\n", request->ifHandle));
    }
}

void CsrIpEtherIfLinkDownReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfLinkDownReq *request;

    request = instanceData->msg;

    if (request->ifHandle < CSR_IP_MAX_ETHERS)
    {
        CsrIpLock(instanceData);

        if (instanceData->etherInstance[request->ifHandle] != NULL)
        {
            CsrIpEtherInst *etherInstance;

            etherInstance = instanceData->etherInstance[request->ifHandle];

            CsrIpFdRemove(instanceData, etherInstance->fd, TRUE, FALSE);
            CsrIpEtherIfLinkDownCfmSend(etherInstance->qid, request->ifHandle,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "ether%u: link down: interface not found\n", request->ifHandle));
        }

        CsrIpUnlock(instanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "ether%u: link down: interface not found\n", request->ifHandle));
    }
}

void CsrIpEtherIfFlowControlPauseReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfFlowControlPauseReq *request;

    request = instanceData->msg;

    if (request->ifHandle < CSR_IP_MAX_ETHERS)
    {
        CsrIpLock(instanceData);

        if (instanceData->etherInstance[request->ifHandle] != NULL)
        {
            CsrIpEtherInst *etherInstance;

            etherInstance = instanceData->etherInstance[request->ifHandle];
            CsrIpFdRemove(instanceData, etherInstance->fd, TRUE, FALSE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "ether%u: flow control pause: interface not found\n", request->ifHandle));
        }

        CsrIpUnlock(instanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "ether%u: flow control pause: interface not found\n", request->ifHandle));
    }
}

void CsrIpEtherIfFlowControlResumeReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpEtherIfFlowControlResumeReq *request;

    request = instanceData->msg;

    if (request->ifHandle < CSR_IP_MAX_ETHERS)
    {
        CsrIpLock(instanceData);

        if (instanceData->etherInstance[request->ifHandle] != NULL)
        {
            CsrIpEtherInst *etherInstance;

            etherInstance = instanceData->etherInstance[request->ifHandle];
            CsrIpFdAdd(instanceData, etherInstance->fd, TRUE, FALSE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "ether%u: flow control resume: interface not found\n", request->ifHandle));
        }

        CsrIpUnlock(instanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "ether%u: flow control resume: interface not found\n", request->ifHandle));
    }
}

#ifdef __cplusplus
}
#endif
