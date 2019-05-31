#ifndef CSR_BT_HID_PARSER_H__
#define CSR_BT_HID_PARSER_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Masks
 */
#define CSR_BT_HID_ITEM_SIZE_MASK  0x03
#define CSR_BT_HID_ITEM_TYPE_MASK  0x0c
#define CSR_BT_HID_ITEM_TAG_MASK   0xfc

/*
 * Main tag types
 */
#define CSR_BT_HID_ITEM_INPUT            0x80
#define CSR_BT_HID_ITEM_OUTPUT           0x90
#define CSR_BT_HID_ITEM_BEGIN_COLLECTION 0xa0
#define CSR_BT_HID_ITEM_FEATURE          0xb0
#define CSR_BT_HID_ITEM_END_COLLECTION      0xc0

/*
 * Global tag types
 */
#define CSR_BT_HID_ITEM_USAGE_PAGE    0x04
#define CSR_BT_HID_ITEM_LOG_MIN       0x14
#define CSR_BT_HID_ITEM_LOG_MAX       0x24
#define CSR_BT_HID_ITEM_PHYS_MIN      0x34
#define CSR_BT_HID_ITEM_PHYS_MAX      0x44
#define CSR_BT_HID_ITEM_UNIT_EXPONENT 0x54
#define CSR_BT_HID_ITEM_UNIT          0x64
#define CSR_BT_HID_ITEM_REP_SIZE      0x74
#define CSR_BT_HID_ITEM_REP_ID        0x84
#define CSR_BT_HID_ITEM_REP_COUNT     0x94
#define CSR_BT_HID_ITEM_PUSH          0xa4
#define CSR_BT_HID_ITEM_POP           0xb4

/*
 * Local tag types
 */
#define CSR_BT_HID_ITEM_USAGE             0x08
#define CSR_BT_HID_ITEM_USAGE_MIN         0x18
#define CSR_BT_HID_ITEM_USAGE_MAX         0x28
#define CSR_BT_HID_ITEM_DESIGNATOR_INDEX  0x38
#define CSR_BT_HID_ITEM_DESIGNATOR_MIN    0x48
#define CSR_BT_HID_ITEM_DESIGNATOR_MAX    0x58
#define CSR_BT_HID_ITEM_STRING_INDEX      0x78
#define CSR_BT_HID_ITEM_STRING_MIN       0x88
#define CSR_BT_HID_ITEM_STRING_MAX        0x98
#define CSR_BT_HID_ITEM_DELIMITER         0xa8

/*
 * Main item data
 */
#define CSR_BT_HID_ITEM_DATA_CONSTANT        0x001
#define CSR_BT_HID_ITEM_DATA_VARIABLE        0x002
#define CSR_BT_HID_ITEM_DATA_RELATIVE        0x004
#define CSR_BT_HID_ITEM_DATA_WRAP        0x008
#define CSR_BT_HID_ITEM_DATA_NONLINEAR        0x010
#define CSR_BT_HID_ITEM_DATA_NO_PREFERRED      0x020
#define CSR_BT_HID_ITEM_DATA_NULL_STATE        0x040
#define CSR_BT_HID_ITEM_DATA_VOLATILE        0x080
#define CSR_BT_HID_ITEM_DATA_BUFFERED_BYTE     0x100

/*
 * HID collection types
 */
#define CSR_BT_HID_COLLECTION_TYPES            3
#define CSR_BT_HID_COLLECTION_PHYSICAL        0
#define CSR_BT_HID_COLLECTION_APPLICATION    1
#define CSR_BT_HID_COLLECTION_LOGICAL        2

/*
 * HID usage tables
 */
#define CSR_BT_HID_USAGE_PAGE        0xffff0000

#define CSR_BT_HID_UP_GENDESK             0x00010000
#define CSR_BT_HID_UP_SIM_CTRLS        0x00020000
#define CSR_BT_HID_UP_VR_CTRLS            0x00030000
#define CSR_BT_HID_UP_SPORT_CTRLS        0x00040000
#define CSR_BT_HID_UP_GAME_CTRLS        0x00050000
#define CSR_BT_HID_UP_GD_CTRLS            0x00060000
#define CSR_BT_HID_UP_KEYBOARD         0x00070000
#define CSR_BT_HID_UP_LED             0x00080000
#define CSR_BT_HID_UP_BUTTON             0x00090000
#define CSR_BT_HID_UP_ORDINAL             0x000a0000
#define CSR_BT_HID_UP_TELEPHONY_DEV            0x000b0000
#define CSR_BT_HID_UP_CONSUMER            0x000c0000
#define CSR_BT_HID_UP_DIGITIZER         0x000d0000
#define CSR_BT_HID_UP_PID             0x000f0000
#define CSR_BT_HID_UP_HPVENDOR            0xff7f0000
#define CSR_BT_HID_UP_MSVENDOR                 0xff000000

#define CSR_BT_HID_USAGE                0x0000ffff

#define CSR_BT_HID_GD_POINTER            0x00010001
#define CSR_BT_HID_GD_MOUSE            0x00010002
#define CSR_BT_HID_GD_JOYSTICK            0x00010004
#define CSR_BT_HID_GD_GAMEPAD            0x00010005

/*
 * Long item tag
 */
#define CSR_BT_HID_ITEM_LONG    15

/*
 * Item types
 */
#define CSR_BT_HID_TYPE_MAIN      0x00
#define CSR_BT_HID_TYPE_GLOBAL      0x04
#define CSR_BT_HID_TYPE_LOCAL      0x08

/*
 * HID report types
 */
#define CSR_BT_HID_INPUT_REP    0
#define CSR_BT_HID_OUTPUT_REP    1
#define CSR_BT_HID_FEATURE_REP    2

#define CSR_BT_HID_REP_TYPES 3
#define CSR_BT_HID_MAX_COLLECTIONS 20
#define CSR_BT_HID_MAX_USAGES 1024
#define CSR_BT_HID_MAX_REP 10
#define CSR_BT_HID_MAX_ENTRYS 32

struct CsrBtHidStack
{
    struct CsrBtHidStack *next;
};

struct CsrBtHidCollection
{
    struct CsrBtHidStack *next;  /* MUST be 1st */
    unsigned type;
    unsigned usage;
};

struct CsrBtHidUsage
{
    CsrUint32 usage;
    CsrUint16 code;
    CsrUint8 type;
};

struct CsrBtHidEntry
{
    unsigned collectionType[CSR_BT_HID_COLLECTION_TYPES];
    struct CsrBtHidUsage *usage;
    unsigned maxusage;
    unsigned flags;
    unsigned reportSize;
    unsigned reportCount;
    unsigned reportType;
    CsrInt32 *value;
    CsrInt32 logicalMin;
    CsrInt32 logicalMax;
    CsrInt32 physicalMin;
    CsrInt32 physicalMax;
    CsrInt32 unitExp;
    unsigned unit;
    struct CsrBtHidReport *report;
    unsigned index;
};

struct CsrBtHidReport
{
    unsigned id;
    unsigned type;
    struct CsrBtHidEntry *entry[CSR_BT_HID_MAX_ENTRYS];
    unsigned maxEntry;
    unsigned size;
    struct CsrBtHidDevice *device;
};

struct CsrBtHidReportType
{
    CsrBool  usesIds;
    CsrUint8 maxReport;
    CsrUint8 reportIdTable[256];
    struct CsrBtHidReport *reportIndex[CSR_BT_HID_MAX_REP];
};

struct CsrBtHidDevice
{
    struct CsrBtHidReportType reportType[CSR_BT_HID_REP_TYPES];
};

/* Parser type */

struct CsrBtHidGlobal
{
    struct CsrBtHidStack *next;  /* MUST be 1st */
    CsrUint32 usagePage;
    CsrInt32 logicalMin;
    CsrInt32 logicalMax;
    CsrInt32 physicalMin;
    CsrInt32 physicalMax;
    CsrInt32 unitExp;
    CsrUint32 unit;
    CsrUint8 reportId;
    CsrUint32 reportSize;
    CsrUint32 reportCount;
};

struct CsrBtHidLocal
{
    CsrUint32 usage[CSR_BT_HID_MAX_USAGES]; /* usage array */
    CsrUint32 usageIndex;
    CsrUint32 usageMin;
    CsrUint32 delimiterDepth;
    CsrUint32 delimiterBranch;
};

struct CsrBtHidParser
{
    CsrUint8 *descPos;
    CsrUint8 *descEnd;
    CsrUint8 itemSize;
    CsrUint8 itemType;
    CsrUint8 itemTag;
    CsrUint32 itemData;
    struct CsrBtHidLocal local;
    struct CsrBtHidGlobal global;
    struct CsrBtHidStack *globalStack;
    struct CsrBtHidStack *collectionStack;
    struct CsrBtHidDevice *device;
};

struct CsrBtHidDevice *CsrBtHidParse(CsrUint8 *raw_hid, CsrUint16 raw_hid_size);
void CsrBtHidFree( struct CsrBtHidDevice *device);

#ifdef __cplusplus
}
#endif

#endif /* __HID_PARSER_H */

