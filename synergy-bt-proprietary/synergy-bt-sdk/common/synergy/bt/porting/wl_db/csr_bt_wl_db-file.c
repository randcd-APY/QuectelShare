/****************************************************************************

Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>

#include "csr_file.h"
#include "csr_bt_wl_db.h"
#include "csr_bt_util.h"
#include "csr_bt_common.h"
#include "csr_log.h"
#include "csr_pmem.h"
#include "csr_log_text.h"
#include "csr_bt_platform.h"
#include "csr_bt_addr.h"

#define LOG_TEXT_TASK_ID    ((CsrLogTextTaskId) CSR_BT_SC_IFACEQUEUE)
#define LOG_TEXT_SO_WLDB    0

#define CSR_BT_WL_DB_PERMS     (CSR_FILE_PERMS_USER_READ|CSR_FILE_PERMS_USER_WRITE)

#define csrBtWlMemFree(_pMem)  {     \
                    if((void*)_pMem) \
                    {                \
                      free(_pMem);   \
                      _pMem = NULL;  \
                    }                \
                }

CsrBtWlDbRecord wlHead;
CsrBtWlDbRecord wlTail;
CsrBtWlDbRecord *wlDbHead = &wlHead;
CsrBtWlDbRecord *wlDbTail = &wlTail;

void csrBtWlAddNode(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail, CsrBtWlDbRecord* _node)
{
    CsrBtWlDbRecord* curr = head;

    while(curr->next != tail)
    {
        curr=curr->next;
    }
    curr->next=_node;
    _node->prev = curr;
    _node->next = tail;
    tail->prev = _node;
}

CsrBool csrBtWlDelNode(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail, CsrBtWlDbRecord* _node)
{
    CsrBool res = FALSE;

    CsrBtWlDbRecord* curr=head->next;
    CsrBtWlDbRecord* temp1=head;
    CsrBtWlDbRecord* temp2=NULL;
    while(curr != tail)
    {
        if(CsrBtAddrEq(&curr->addr, &_node->addr))
        {
            temp2 = curr->next;
            temp1->next = temp2;
            temp2->prev = temp1;

            csrBtWlMemFree(curr);
            res = TRUE;
            return res;
        }
        else
        {
            temp1=temp1->next;
        }
        curr=curr->next;
    }

    return res;
}

void csrBtWlNodeFree(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail)
{
    CsrBtWlDbRecord* curr;
    CsrBtWlDbRecord* temp1;
    CsrBtWlDbRecord* temp2;
    if ((head == NULL) || (tail == NULL))
    {
        return;
    }

    curr = head->next;
    temp1=head;
    temp2=NULL;
    while(curr != tail)
    {
        temp2 = curr;
        temp1->next = curr->next;
        temp1->next->prev = temp1;

        curr=curr->next;

        csrBtWlMemFree(temp2);
    }
}

void csrBtWlDbprint(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail)
{
    CsrBtWlDbRecord* curr=head->next;
    while(curr!=tail)
    {
        CSR_LOG_TEXT_INFO((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "lap:%08x, uap:%02x, nap:%04x\n", curr->addr.addr.lap, curr->addr.addr.uap, curr->addr.addr.nap));
        curr=curr->next;
    }
}

CsrBool csrBtWlDbCheckEmpty()
{
    CsrResult res = TRUE;

    if(csrBtWlDbGetList() != -1)
    {
        if (wlDbHead ->next != wlDbTail)
        {
            res = FALSE;
        }

        csrBtWlNodeFree(wlDbHead, wlDbTail);
    }

    return res;
}
CsrInt32 csrBtWlDbGetList()
{
    CsrFileHandle *fileHandle;
    CsrSize read;
    CsrResult res;

    CsrBtWlDbRecord *dummyNodePtr;
    wlDbHead->prev = NULL;
    wlDbHead->next = wlDbTail;
    wlDbTail->prev = wlDbHead;
    wlDbTail->next = NULL;

    res = CsrFileOpen(&fileHandle,
                      CSR_BT_WL_DB_FILE_NAME,
                      CSR_FILE_OPEN_FLAGS_READ_ONLY,
                      CSR_BT_WL_DB_PERMS);

    /* Serious error, bail out */
    if(res != CSR_RESULT_SUCCESS)
    {
        CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Error File Open"));
        if (fileHandle)
        {
            CsrFileClose(fileHandle);
        }
        return -1;
    }

    res = CsrFileSeek(fileHandle, CSR_BT_WL_DB_FILE_VERSION_SIZE, CSR_SEEK_SET);
    if(res != CSR_RESULT_SUCCESS)
    {
        CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "FILE Seek error"));
        CsrFileClose(fileHandle);

        return -1;
    }

    do
    {
        dummyNodePtr = (CsrBtWlDbRecord*)CsrPmemZalloc(1*sizeof(CsrBtWlDbRecord));
        res = CsrFileRead(&dummyNodePtr->addr, sizeof(CsrBtTypedAddr), fileHandle, &read);

        if((res == CSR_RESULT_SUCCESS) &&
           (read == sizeof(CsrBtTypedAddr)))
        {
            csrBtWlAddNode(wlDbHead, wlDbTail, dummyNodePtr);

        }
        else
        {
            CSR_LOG_TEXT_INFO((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "File read fail!!! "));
            break;
        }
    }
    while((res == CSR_RESULT_SUCCESS) &&
          (read == sizeof(CsrBtTypedAddr)));
    CsrFileClose(fileHandle);
    csrBtWlDbprint(wlDbHead, wlDbTail);

    return TRUE;
}

void csrBtWlDbGet(CsrBtWlDbRecord** head, CsrBtWlDbRecord** tail)
{
    CsrInt32 res = csrBtWlDbGetList();
    if (res != -1)
    {
        *head = wlDbHead;
        *tail = wlDbTail;
    }
    else
    {
        *head = NULL;
        *tail = NULL;
    }
}

void csrBtWlInsertNode(CsrBtWlDbRecord        *node)
{
    /*1st del this node if exist */
    csrBtWlDelNode(wlDbHead, wlDbTail, node);

    /*2nd insert node*/
    csrBtWlAddNode(wlDbHead, wlDbTail, node);
}

CsrBool csrBtWlDbCheckAddrExist(CsrBtTypedAddr *address)
{
    CsrResult res = FALSE;

    if(csrBtWlDbGetList() != -1)
    {
        CsrBtWlDbRecord* curr=wlDbHead->next;
        CsrBtWlDbRecord* temp1=wlDbHead;
        while(curr != wlDbTail)
        {
            if(CsrBtAddrEq(&curr->addr, address))
            {
                res = TRUE;
                return res;
            }
            else
            {
                temp1=temp1->next;
            }
            curr=curr->next;
        }
    }

    csrBtWlNodeFree(wlDbHead, wlDbTail);

    return res;
}

void csrBtWlDbUpdate(CsrBtTypedAddr *address)
{
    /* Should write information about a device into the data base, if
     * the device already exist, its entry should be updated */
    CsrFileHandle *fileHandle;
    CsrResult res;
    CsrSize read;
    CsrFileOpenFlags flags;
    CsrBtWlDbRecord* curr = (CsrBtWlDbRecord*)CsrPmemZalloc(1*sizeof(CsrBtWlDbRecord));

    if(csrBtWlDbGetList() == -1)
    {
        /* Creating new database */
        flags = CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE;
    }
    else
    {
        /* Existing database */
        flags = CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_TRUNCATE;
    }
    CsrBtAddrCopy(&curr->addr, address);

    csrBtWlInsertNode(curr);

    /* Open file for write */
    res = CsrFileOpen(&fileHandle,
                      CSR_BT_WL_DB_FILE_NAME,
                      flags,
                      CSR_BT_WL_DB_PERMS);

    if(res == CSR_RESULT_SUCCESS)
    {
        res = CsrFileSeek(fileHandle,
                          0,
                          CSR_SEEK_SET);
        if(res != CSR_RESULT_SUCCESS)
        {
            CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Seek %04X:%02X:%06X",
                                  address->addr.nap, address->addr.uap, address->addr.lap));
        }

        curr = wlDbHead->next;
        while (curr != wlDbTail)
        {
            res = CsrFileWrite(&curr->addr, sizeof(CsrBtTypedAddr), fileHandle, &read);
            if(res != CSR_RESULT_SUCCESS || read != sizeof(CsrBtTypedAddr))
            {
                CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Write %04X:%02X:%06X",
                                      address->addr.nap, address->addr.uap, address->addr.lap));
                break;
            }

            curr = curr->next;
        }
        CsrFileFlush(fileHandle);
        CsrFileClose(fileHandle);

        csrBtWlDbprint(wlDbHead, wlDbTail);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Open %04X:%02X:%06X", address->addr.nap, address->addr.uap, address->addr.lap));
    }

    csrBtWlNodeFree(wlDbHead, wlDbTail);
}

CsrResult CsrBtWlDbRemove(CsrBtTypedAddr *addr)
{
    CsrResult res = CSR_RESULT_SUCCESS;

    CsrFileHandle *fileHandle;
    CsrFileOpenFlags flags;
    CsrSize read;
    CsrBtWlDbRecord rec;
    CsrBtWlDbRecord* curr  = &rec;
    CsrMemSet(curr, 0, sizeof(CsrBtWlDbRecord));
    CsrBtAddrCopy(&curr->addr, addr);

    if(csrBtWlDbGetList() == -1)
    {
        res = CSR_RESULT_FAILURE;
    }
    else
    {
        /* Existing database */
        flags = CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_TRUNCATE;

        csrBtWlDelNode(wlDbHead, wlDbTail, curr);

        /* Open file for write */
        res = CsrFileOpen(&fileHandle,
                          CSR_BT_WL_DB_FILE_NAME,
                          flags,
                          CSR_BT_WL_DB_PERMS);

        if(res == CSR_RESULT_SUCCESS)
        {
            res = CsrFileSeek(fileHandle,
                              0,
                              CSR_SEEK_SET);
            if(res != CSR_RESULT_SUCCESS)
            {
                CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Seek %04X:%02X:%06X",
                                      addr->addr.nap, addr->addr.uap, addr->addr.lap));
            }

            curr = wlDbHead->next;
            while (curr != wlDbTail)
            {
                res = CsrFileWrite(&curr->addr, sizeof(CsrBtTypedAddr), fileHandle, &read);
                if(res != CSR_RESULT_SUCCESS || read != sizeof(CsrBtTypedAddr))
                {
                    CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Write %04X:%02X:%06X",
                                          addr->addr.nap, addr->addr.uap, addr->addr.lap));
                    break;
                }

                curr = curr->next;
            }
            CsrFileFlush(fileHandle);
            CsrFileClose(fileHandle);

            csrBtWlDbprint(wlDbHead, wlDbTail);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((LOG_TEXT_TASK_ID, LOG_TEXT_SO_WLDB, "Open %04X:%02X:%06X", addr->addr.nap, addr->addr.uap, addr->addr.lap));
        }
    }

    csrBtWlNodeFree(wlDbHead, wlDbTail);

    return res;
}

