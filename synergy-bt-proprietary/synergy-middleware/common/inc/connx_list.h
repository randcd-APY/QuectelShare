#ifndef CONNX_LIST_H__
#define CONNX_LIST_H__

/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* **************************************************** */
/* **************************************************** */
/* General linked list handling */
/* **************************************************** */
/* **************************************************** */

typedef struct ConnxCmnListElmTag
{
    struct ConnxCmnListElmTag *next;
    struct ConnxCmnListElmTag *prev;
} ConnxCmnListElm_t;

typedef void (*ConnxCmnListRemoveFunc_t)(ConnxCmnListElm_t *elem);
typedef void (*ConnxCmnListAddFunc_t)(ConnxCmnListElm_t *elem);
typedef uint32_t (*ConnxCmnListSizeofFunc_t)(ConnxCmnListElm_t *elem);
typedef void * (*ConnxCmnListSerializeFunc_t)(ConnxCmnListElm_t *elem);

typedef struct ConnxCmnListDataElmTag
{
    struct ConnxCmnListDataElmTag *next;
    struct ConnxCmnListDataElmTag *prev;
    void                        *data;
} ConnxCmnListDataElm_t;

typedef struct ConnxCmnListTag
{
    ConnxCmnListElm_t       *first;
    ConnxCmnListElm_t       *last;
    uint32_t              count;
    uint32_t              listId;
    ConnxCmnListAddFunc_t    addFunc;         /* Pointer to function which will be called after adding a new element - NULL if no special handling */
    ConnxCmnListRemoveFunc_t removeFunc;      /* Pointer to function for freeing an element - NULL if no special handling */
} ConnxCmnList_t;

typedef int32_t (*ConnxCmnListSortFunc_t)(ConnxCmnListElm_t *elem1, ConnxCmnListElm_t *elem2);
typedef bool (*ConnxCmnListSearchFunc_t)(ConnxCmnListElm_t *elem, void *value);
typedef void (ConnxCmnListIterateFunc_t)(ConnxCmnListElm_t *elem, void *data);
typedef bool (ConnxCmnListIterateAllowRemoveFunc_t)(ConnxCmnListElm_t *elem, void *data);

void ConnxCmnListInit(ConnxCmnList_t *cmnList, uint32_t listId, ConnxCmnListAddFunc_t addFunc, ConnxCmnListRemoveFunc_t removeFunc);
void ConnxCmnListDeinit(ConnxCmnList_t *cmnList);

ConnxCmnListElm_t *ConnxCmnListElementGetFirst(ConnxCmnList_t *cmnList);
ConnxCmnListElm_t *ConnxCmnListElementGetLast(ConnxCmnList_t *cmnList);
ConnxCmnListElm_t *ConnxCmnListElementAddFirst(ConnxCmnList_t *cmnList, size_t size);
ConnxCmnListElm_t *ConnxCmnListElementAddLast(ConnxCmnList_t *cmnList, size_t size);

void ConnxCmnListElementRemove(ConnxCmnList_t *cmnList, ConnxCmnListElm_t *element);

ConnxCmnListElm_t *ConnxCmnListGetFromIndex(ConnxCmnList_t *cmnList, uint32_t index);
void ConnxCmnListIterate(ConnxCmnList_t *cmnList, ConnxCmnListIterateFunc_t iterateFunc, void *data);

void ConnxCmnListIterateAllowRemove(ConnxCmnList_t *cmnList, ConnxCmnListIterateAllowRemoveFunc_t iterateFunc, void *data);

ConnxCmnListElm_t *ConnxCmnListSearch(ConnxCmnList_t *cmnList, ConnxCmnListSearchFunc_t searchFunc, void *value);
void ConnxCmnListSort(ConnxCmnList_t *cmnList, ConnxCmnListSortFunc_t sortFunc);

ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint8(ConnxCmnList_t *cmnList, size_t offset, uint8_t value);
ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint16(ConnxCmnList_t *cmnList, size_t offset, uint16_t value);
ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint32(ConnxCmnList_t *cmnList, size_t offset, uint32_t value);

#define ConnxCmnListGetFirst(list)           ((list)->first)
#define ConnxCmnListGetLast(list)            ((list)->last)
#define ConnxCmnListGetCount(list)           ((list)->count)
#define ConnxCmnListNext(elm)                ((elm) = ((ConnxCmnListElm_t *) (elm)->next))

#ifdef __cplusplus
}
#endif

#endif
