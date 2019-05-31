#ifndef CSR_LIST_H__
#define CSR_LIST_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* **************************************************** */
/* **************************************************** */
/* General linked list handling */
/* **************************************************** */
/* **************************************************** */

typedef struct CsrCmnListElmTag
{
    struct CsrCmnListElmTag *next;
    struct CsrCmnListElmTag *prev;
} CsrCmnListElm_t;

typedef void (*CsrCmnListRemoveFunc_t)(CsrCmnListElm_t *elem);
typedef void (*CsrCmnListAddFunc_t)(CsrCmnListElm_t *elem);
typedef CsrUint32 (*CsrCmnListSizeofFunc_t)(CsrCmnListElm_t *elem);
typedef void * (*CsrCmnListSerializeFunc_t)(CsrCmnListElm_t *elem);

typedef struct CsrCmnListDataElmTag
{
    struct CsrCmnListDataElmTag *next;
    struct CsrCmnListDataElmTag *prev;
    void                        *data;
} CsrCmnListDataElm_t;

typedef struct CsrCmnListTag
{
    CsrCmnListElm_t       *first;
    CsrCmnListElm_t       *last;
    CsrUint32              count;
    CsrUint32              listId;
    CsrCmnListAddFunc_t    addFunc;         /* Pointer to function which will be called after adding a new element - NULL if no special handling */
    CsrCmnListRemoveFunc_t removeFunc;      /* Pointer to function for freeing an element - NULL if no special handling */
} CsrCmnList_t;

typedef CsrInt32 (*CsrCmnListSortFunc_t)(CsrCmnListElm_t *elem1, CsrCmnListElm_t *elem2);
typedef CsrBool (*CsrCmnListSearchFunc_t)(CsrCmnListElm_t *elem, void *value);
typedef void (CsrCmnListIterateFunc_t)(CsrCmnListElm_t *elem, void *data);
typedef CsrBool (CsrCmnListIterateAllowRemoveFunc_t)(CsrCmnListElm_t *elem, void *data);

void CsrCmnListInit(CsrCmnList_t *cmnList, CsrUint32 listId, CsrCmnListAddFunc_t addFunc, CsrCmnListRemoveFunc_t removeFunc);
void CsrCmnListDeinit(CsrCmnList_t *cmnList);

CsrCmnListElm_t *CsrCmnListElementGetFirst(CsrCmnList_t *cmnList);
CsrCmnListElm_t *CsrCmnListElementGetLast(CsrCmnList_t *cmnList);
CsrCmnListElm_t *CsrCmnListElementAddFirst(CsrCmnList_t *cmnList, CsrSize size);
CsrCmnListElm_t *CsrCmnListElementAddLast(CsrCmnList_t *cmnList, CsrSize size);

void CsrCmnListElementRemove(CsrCmnList_t *cmnList, CsrCmnListElm_t *element);

CsrCmnListElm_t *CsrCmnListGetFromIndex(CsrCmnList_t *cmnList, CsrUint32 index);
void CsrCmnListIterate(CsrCmnList_t *cmnList, CsrCmnListIterateFunc_t iterateFunc, void *data);

void CsrCmnListIterateAllowRemove(CsrCmnList_t *cmnList, CsrCmnListIterateAllowRemoveFunc_t iterateFunc, void *data);

CsrCmnListElm_t *CsrCmnListSearch(CsrCmnList_t *cmnList, CsrCmnListSearchFunc_t searchFunc, void *value);
void CsrCmnListSort(CsrCmnList_t *cmnList, CsrCmnListSortFunc_t sortFunc);

CsrCmnListElm_t *CsrCmnListSearchOffsetUint8(CsrCmnList_t *cmnList, CsrSize offset, CsrUint8 value);
CsrCmnListElm_t *CsrCmnListSearchOffsetUint16(CsrCmnList_t *cmnList, CsrSize offset, CsrUint16 value);
CsrCmnListElm_t *CsrCmnListSearchOffsetUint32(CsrCmnList_t *cmnList, CsrSize offset, CsrUint32 value);

#define CsrCmnListGetFirst(list)           ((list)->first)
#define CsrCmnListGetLast(list)            ((list)->last)
#define CsrCmnListGetCount(list)           ((list)->count)
#define CsrCmnListNext(elm)                ((elm) = ((CsrCmnListElm_t *) (elm)->next))

#ifdef __cplusplus
}
#endif

#endif
