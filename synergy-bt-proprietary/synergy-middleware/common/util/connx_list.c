/*****************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary. 
            
*****************************************************************************/

#include <stdlib.h>
#include "connx_list.h"

void ConnxCmnListInit(ConnxCmnList_t *cmnList, uint32_t listId, ConnxCmnListAddFunc_t addFunc, ConnxCmnListRemoveFunc_t removeFunc)
{
    cmnList->first = NULL;
    cmnList->last = NULL;
    cmnList->count = 0;
    cmnList->listId = listId;
    cmnList->addFunc = addFunc;
    cmnList->removeFunc = removeFunc;
}

void ConnxCmnListDeinit(ConnxCmnList_t *cmnList) /* Free the entire list */
{
    ConnxCmnListElm_t *element, *nextElement;

    for (element = cmnList->first; element; element = nextElement)
    {
        nextElement = element->next;
        ConnxCmnListElementRemove(cmnList, element);
    }

    cmnList->listId = 0;
    cmnList->first = NULL;
    cmnList->last = NULL;
    cmnList->count = 0;
}

ConnxCmnListElm_t *ConnxCmnListElementGetFirst(ConnxCmnList_t *cmnList)
{
    if (!cmnList)
    {
        return NULL;
    }
    else
    {
        return cmnList->first;
    }
}

ConnxCmnListElm_t *ConnxCmnListElementGetLast(ConnxCmnList_t *cmnList)
{
    if (!cmnList)
    {
        return NULL;
    }
    else
    {
        return cmnList->last;
    }
}

ConnxCmnListElm_t *ConnxCmnListElementAddFirst(ConnxCmnList_t *cmnList, size_t size) /* Add an element in the beginning of the list */
{
    ConnxCmnListElm_t *element = malloc(size);

    /* [QTI] Fix KW#7296473/7296479 "NULL dereferenced". */
    if (!element)
        return NULL;

    element->next = cmnList->first;
    element->prev = NULL;

    if (cmnList->first) /* This is not the first element in the list */
    {
        cmnList->first->prev = element;
    }
    else /* This is the first element in the list */
    {
        cmnList->last = element;
    }

    cmnList->first = element;
    cmnList->count++;

    if (cmnList->addFunc)
    {
        cmnList->addFunc(element);
    }

    return element;
}

ConnxCmnListElm_t *ConnxCmnListElementAddLast(ConnxCmnList_t *cmnList, size_t size) /* Add an element in the end of the list */
{
    ConnxCmnListElm_t *element = malloc(size);

    /* [QTI] Fix KW#7296474/7296480 "NULL dereferenced". */
    if (!element)
        return NULL;

    element->next = NULL;
    element->prev = cmnList->last;

    if (cmnList->last) /* An element is already present in the list */
    {
        cmnList->last->next = element;
    }
    else /* This is the first element in the list */
    {
        cmnList->first = element;
    }

    cmnList->last = element;
    cmnList->count++;

    if (cmnList->addFunc)
    {
        cmnList->addFunc(element);
    }

    return element;
}

void ConnxCmnListElementRemove(ConnxCmnList_t *cmnList, ConnxCmnListElm_t *element)
{
    /* [QTI] Fix KW#7296475/7296481 "Suspicious dereference of pointer 'element' before NULL check". */
    if (!element)
        return;

    if (element->prev) /* This is not the first element in the list */
    {
        element->prev->next = element->next;
    }
    else /* This is the first element in the list */
    {
        cmnList->first = element->next;
    }

    if (element->next) /* This is not the last element in the list */
    {
        element->next->prev = element->prev;
    }
    else /* This is the last element in the list */
    {
        cmnList->last = element->prev;
    }

    cmnList->count--;

    if (cmnList->removeFunc) /* Call associated function on the element to remove */
    {
        cmnList->removeFunc(element);
    }
    
    if (element != NULL)
    {
        free(element);
    }
}

ConnxCmnListElm_t *ConnxCmnListGetFromIndex(ConnxCmnList_t *cmnList, uint32_t index) /* Get an element from the 'index' of an element (starting from the beginning of the list) */
{
    ConnxCmnListElm_t *elem = NULL;

    for (elem = cmnList->first; elem; elem = elem->next, index--) /* Skip to the correct element */
    {
        if (index == 0)
        {
            break;
        }
    }

    return elem;
}

void ConnxCmnListIterate(ConnxCmnList_t *cmnList, ConnxCmnListIterateFunc_t iterateFunc, void *data)
{
    ConnxCmnListElm_t *elem;

    for (elem = cmnList->first; elem; elem = elem->next)
    {
        iterateFunc(elem, data);
    }
}

void ConnxCmnListIterateAllowRemove(ConnxCmnList_t *cmnList, ConnxCmnListIterateAllowRemoveFunc_t iterateRemoveFunc, void *data)
{
    ConnxCmnListElm_t *elem = cmnList->first;
    ConnxCmnListElm_t *nextElem;

    while (elem)
    {
        if (iterateRemoveFunc(elem, data)) /* Element should be removed - save link to next element */
        {
            nextElem = elem->next;
            ConnxCmnListElementRemove(cmnList, elem);
            elem = nextElem;
        }
        else /* Element should not be removed - existing element is valid */
        {
            elem = elem->next;
        }
    }
}

ConnxCmnListElm_t *ConnxCmnListSearch(ConnxCmnList_t *cmnList, ConnxCmnListSearchFunc_t searchFunc, void *data)
{
    ConnxCmnListElm_t *currentElem;

    for (currentElem = cmnList->first; currentElem; currentElem = currentElem->next)
    {
        if (searchFunc(currentElem, data) == true)
        {
            break;
        }
    }

    return currentElem;
}

static void csrCmnListMove(ConnxCmnList_t *cmnList, ConnxCmnListElm_t *dst, ConnxCmnListElm_t *src) /* Will insert src before dst */
{   /* Update old links around src */
    if (src->prev)
    {
        src->prev->next = src->next;
    }
    else /* The entry to move is the first in the list */
    {
        cmnList->first = src->next;
    }

    if (src->next)
    {
        src->next->prev = src->prev;
    }
    else /* The entry to move is the last in the list */
    {
        cmnList->last = src->prev;
    }

    /* Update new links around src */
    src->next = dst;
    src->prev = dst->prev;

    /* Update links around dst */
    if (dst->prev)
    {
        dst->prev->next = src;
    }
    else /* dst is the first in the list */
    {
        cmnList->first = src;
    }
    dst->prev = src;
}

void ConnxCmnListSort(ConnxCmnList_t *cmnList, ConnxCmnListSortFunc_t sortFunc) /* Using insertion sort */
{
    ConnxCmnListElm_t *currentElem = cmnList->first, *nextElem, *tmpElem;

    while (currentElem && currentElem->next)
    {
        nextElem = currentElem->next;

        if (sortFunc(currentElem, nextElem) > 0) /* nextElem should be moved - (currentElem > nextElem) */
        {
            currentElem = currentElem->next;
            /* Find the correct place to insert the element */
            for (tmpElem = cmnList->first; tmpElem; tmpElem = tmpElem->next)
            {
                if (sortFunc(nextElem, tmpElem) <= 0) /* Insert element before tmpElem - (nextElem <= tmpElem) */
                {
                    csrCmnListMove(cmnList, tmpElem, nextElem);
                    break;
                }
            }
        }
        else
        {
            currentElem = currentElem->next;
        }
    }
}

ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint8(ConnxCmnList_t *cmnList, size_t offset, uint8_t value)
{
    ConnxCmnListElm_t *currentElem;

    for (currentElem = cmnList->first; currentElem; currentElem = currentElem->next)
    {
        if (*(uint8_t *) (((int8_t *) currentElem) + offset) == value)
        {
            return currentElem;
        }
    }

    return NULL;
}

ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint16(ConnxCmnList_t *cmnList, size_t offset, uint16_t value)
{
    ConnxCmnListElm_t *currentElem;

    for (currentElem = cmnList->first; currentElem; currentElem = currentElem->next)
    {
        if (*(uint16_t *) (((int8_t *) currentElem) + offset) == value)
        {
            return currentElem;
        }
    }

    return NULL;
}

ConnxCmnListElm_t *ConnxCmnListSearchOffsetUint32(ConnxCmnList_t *cmnList, size_t offset, uint32_t value)
{
    ConnxCmnListElm_t *currentElem;

    for (currentElem = cmnList->first; currentElem; currentElem = currentElem->next)
    {
        if (*(uint32_t *) (((int8_t *) currentElem) + offset) == value)
        {
            return currentElem;
        }
    }

    return NULL;
}
