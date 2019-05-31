#ifndef CSR_BT_XML_LIB_H__
#define CSR_BT_XML_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* xml tree header file */
#define CSR_BT_INVALID_TYPE            0x0000
#define CSR_BT_ROOT_ELEMENT_TYPE        0x0001
#define CSR_BT_CHILD_ELEMENT_TYPE        0x0002
#define CSR_BT_ELEMENT_ATTRIBUTE_TYPE    0x0003

typedef struct CsrBtElementAttributeTypeTag
{
    CsrUint16 type;
    CsrUint8 * name;
    CsrUint8 * value;
    struct CsrBtElementAttributeTypeTag * nextAttribute;
} CsrBtElementAttributeType;

typedef struct CsrBtChildElementTypeTag
{
    CsrUint16 type;
    CsrUint8 * name;
    CsrUint8 * value;
    CsrBtElementAttributeType * firstAttribute;
    struct CsrBtChildElementTypeTag * nextSiblingElement;
    struct CsrBtChildElementTypeTag * firstChildElement;
} CsrBtChildElementType;

typedef struct
{
    CsrUint16 type;
    CsrUint8 * name;
    CsrUint8 * value;
    CsrBtElementAttributeType * firstAttribute;
    CsrBtChildElementType * firstChildElement;
} CsrBtRootElementType;

typedef union
{
    CsrBtRootElementType rootElement;
    CsrBtChildElementType childElement;
    CsrBtElementAttributeType attribute;
} CsrBtElementTypes;

#define CSR_BT_ELEMENTS_PER_BUFFER        10
typedef struct CsrBtElementListTag
{
    CsrBtElementTypes elementArray[CSR_BT_ELEMENTS_PER_BUFFER];
    CsrUint16 currentIndex;
    struct CsrBtElementListTag * next;
} CsrBtElementList;

#ifdef CSR_BT_INSTALL_OBEX_UTIL_XML
extern void CsrBtFreeXmlTree(CsrBtElementList * xmlTree);
extern CsrBtElementList * CsrBtBuildXmlTree(CsrUint8 * xmlDocument, CsrUint32 docLength);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_XML_LIB_H__ */
