/*****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/


#include <sys/select.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_result.h"
#include "csr_prim_defs.h"

#include "csr_log_text_2.h"

#include "csr_ip_util.h"
#include "csr_ip_task.h"
#include "csr_ip_handler.h"

#ifdef CSR_IP_SUPPORT_TLS
#include "csr_ip_tls_util.h"
#endif

void CsrIpLock(void *arg)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    pthread_mutex_lock(&instanceData->instLock);
}

void CsrIpUnlock(void *arg)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrIpCommand(void *arg, CsrIpThreadCmd cmd)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    if (write(instanceData->controlPipe[1],
            (const char *) &cmd, 1) != 1)
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "CsrIpCommand: Failed to write to controlPipe: %u", cmd));
    }
}

void CsrIpFdNew(void *arg, int fd)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    if (fd > instanceData->maxfd)
    {
        instanceData->maxfd = fd;
    }
}

void CsrIpFdAdd(void *arg,
    int fd, CsrBool rset, CsrBool wset)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    if (rset)
    {
        FD_SET(fd, &instanceData->rsetAll);
    }
    if (wset)
    {
        FD_SET(fd, &instanceData->wsetAll);
    }

    CsrIpCommand(instanceData, THREAD_CMD_RESCHEDULE);
}

void CsrIpFdRemove(void *arg,
    int fd, CsrBool rset, CsrBool wset)
{
    CsrIpInstanceData *instanceData;

    instanceData = arg;

    if (rset)
    {
        FD_CLR(fd, &instanceData->rsetAll);
    }
    if (wset)
    {
        FD_CLR(fd, &instanceData->wsetAll);
    }

    if (fd == instanceData->maxfd)
    {
        /* If we've removed the highest fd, find the new max. */

        int i;

        fd = -1;

        for (i = 0;
             i < CSR_IP_MAX_SOCKETS;
             i++)
        {
            if (instanceData->socketInstance[i] != NULL)
            {
                if (instanceData->socketInstance[i]->socket > fd)
                {
                    fd = instanceData->socketInstance[i]->socket;
                }
            }
        }

#ifdef CSR_IP_SUPPORT_ETHER
        for (i = 0;
             i < CSR_IP_MAX_ETHERS;
             i++)
        {
            if (instanceData->etherInstance[i] != NULL)
            {
                if (instanceData->etherInstance[i]->fd > fd)
                {
                    fd = instanceData->etherInstance[i]->fd;
                }
            }
        }
#endif

#ifdef CSR_IP_SUPPORT_TLS
        fd = CSRMAX(fd, CsrIpTlsMaxFdGet(&instanceData->tlsInstance));
#endif

#ifdef CSR_IP_SUPPORT_IFCONFIG
        instanceData->maxfd = CSRMAX(instanceData->controlPipe[0],
            instanceData->rtSocketListen);
#else
        instanceData->maxfd = instanceData->controlPipe[0];
#endif
        instanceData->maxfd = CSRMAX(instanceData->maxfd, fd);
    }

    CsrIpCommand(instanceData, THREAD_CMD_REAP);
}

void CsrIpMessageForward(CsrSchedQid queue, CsrPrim prim, void *message)
{
    csrIpMessageContainer *container;

    container = CsrPmemAlloc(sizeof(*container));
    container->queue = queue;
    container->prim = prim;
    container->message = message;

    CsrSchedMessagePut(CSR_IP_IFACEQUEUE, CSR_IP_INTERNAL_PRIM, container);
}
