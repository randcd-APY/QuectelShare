#ifndef CSR_BT_CMN_CSR_BT_LINKED_LIST_H__
#define CSR_BT_CMN_CSR_BT_LINKED_LIST_H__

/****************************************************************************

Copyright (c) 2009-2013 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_types.h"
#include "csr_pmem.h"
#include "csr_bt_profiles.h"
#include "csr_list.h"

#ifdef __cplusplus
extern "C" {
#endif

CsrCmnListElm_t *CsrCmnListSearchOffsetAddr(CsrCmnList_t *cmnList, size_t offset, CsrBtDeviceAddr *addr);

/*
 * backwards compatibility only -- do not use
 * for new code.  Instead use the csr_list
 * library from the Synergy Framework
 */


    /* A general linked list type, which can be used all over Synergy BT.
     */
    typedef struct CmnCsrBtLinkedListTag
    {
        void                         *data;
        CsrUint16                      dataLen;
        struct CmnCsrBtLinkedListTag  *nextEntry;
        struct CmnCsrBtLinkedListTag  *prevEntry;
    } CmnCsrBtLinkedListStruct;

    /* Defines the call back function and its arguments used by the function
       CsrBtUtilBllFindDataPointer.                                               */
    typedef CsrInt16 (*CmnBllCompareDataFuncType)(void * buf1, void * buf2);

#ifdef CSR_BT_INSTALL_CM_AFH
    /* Defines the call back function and its arguments used by the function
       CsrBtUtilBllForEachLinkedList.                                               */
    typedef CsrBool (*CmnBllForEachDataFuncType)(void * buf1, void * buf2);
#endif

    typedef void (*CmnBllFreeDataFunctype)(void *data);
/* **************************************************** */
/* **************************************************** */

    /* Create a new entry for the Synergy BT linked list, pointed to by oldBll.
       If oldBll is NULL, a new linked list is started,
       otherwise the new entry is added at the end of the linked list!
     */
    CmnCsrBtLinkedListStruct *CsrBtUtilBllCreateNewEntry(CmnCsrBtLinkedListStruct *oldBll,
                                                  void                  *data,
                                                  CsrUint16              dataLen);

    /* returns a pointer to the entry with index 'index' in the linked list of
       type CmnCsrBtLinkedListStruct, starting from *bll.
    */
    CmnCsrBtLinkedListStruct *CsrBtUtilBllGetEntryPtrFromIndexEx(CmnCsrBtLinkedListStruct *bll,
                                                        CsrUintFast16              index);

    /* returns the number of entries in the linked list of
       type CmnCsrBtLinkedListStruct, starting from *bll.
    */
    CsrUintFast16 CsrBtUtilBllGetNofEntriesEx(CmnCsrBtLinkedListStruct *bll);

    /* returns a pointer to the data from the entry with index 'index'
       in the linked list of
       type CmnCsrBtLinkedListStruct, starting from *bll.
    */

    void *CsrBtUtilBllGetDataPointerEx(CmnCsrBtLinkedListStruct *bll,
                                  CsrUintFast16              index,
                                  CsrUint16             *dataLen);

    /* Set the data and dataLen values in the entry in the linked list.
       If the index is not found, FALSE is returned, otherwise TRUE.
    */
    CsrBool CsrBtUtilBllSetDataPointerEx(CmnCsrBtLinkedListStruct *bll,
                                CsrUintFast16              index,
                                void                  *data,
                                CsrUint16              dataLen);

    /* Frees the data in the from the entry with index 'index'
       in the linked list of
       type CmnCsrBtLinkedListStruct, starting from *bll.
       Keeps the entry in the linked list.
    */
#if 0 /* UNUSED*/
    void CsrBtUtilBllFreeLinkedListDataEx(CmnCsrBtLinkedListStruct *bll,
                                             CsrUintFast16              index);
#endif
    /* Removes the entry in the bll linked list with index index.
       returns a pointer to the new linked list, where the entry is removed.
    */

    CmnCsrBtLinkedListStruct *CsrBtUtilBllFreeLinkedListEntryEx(CmnCsrBtLinkedListStruct *bll_p,
                                                           CsrUintFast16                 index,
                                                           CmnBllFreeDataFunctype  cmnBllFreeDataHandler);

    /* Removes the entry in the bll linked list which points to 'pData'.
       returns a pointer to the new linked list, where the entry is removed.
    */
    CmnCsrBtLinkedListStruct *CsrBtUtilBllFreeLinkedListEntryByDataPointer(CmnCsrBtLinkedListStruct *bll_p,
                                                                                  void *data_p,
                                                                                  CmnBllFreeDataFunctype  cmnBllFreeDataHandler);

    /* Frees the whole linked list structure
       and sets the linked list pointer to NULL.
     */
    void CsrBtUtilBllFreeLinkedList(CmnCsrBtLinkedListStruct **bll,
                                  CmnBllFreeDataFunctype  cmnBllFreeDataHandler);


#ifdef CSR_BT_INSTALL_CM_AFH
    /* Execute cmnBllForEachHandler for element in the list */
    void CsrBtUtilBllForEachLinkedList(CmnCsrBtLinkedListStruct *bll_pp,
                                  CmnBllForEachDataFuncType  cmnBllForEachHandler,
                                  void *data_p);
#endif
    void *CsrBtUtilBllFindDataPointer(CmnCsrBtLinkedListStruct         *bll,
                                    void                            *buf1,
                                    CmnBllCompareDataFuncType       cmnBllCompareDataHandler);

    /* Comparator for two CsrUint16 */
    CsrInt16 CsrBtUtilBllCompareDataUint16FuncType(void * buf1, void * buf2);

    void CsrBtUtilBllPfreeWrapper(void *ptr);

#ifdef __cplusplus
}
#endif

#endif

