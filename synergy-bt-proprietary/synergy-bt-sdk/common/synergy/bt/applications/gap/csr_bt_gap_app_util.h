#ifndef CSR_BT_GAP_APP_UTIL_H__
#define CSR_BT_GAP_APP_UTIL_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #4 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_util.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

    CsrUint16 * CsrBtGapDeviceAddr2UnicodeString(CsrBtDeviceAddr deviceAddr, CsrUint32 deviceStatus);
CsrUint16 * CsrBtGapReturnUcs2DeviceName(CsrBtDeviceAddr deviceAddr, const CsrUint8 * friendlyName);
CsrCharString * CsrBtGapReturnCharStringDeviceName(CsrBtDeviceAddr deviceAddr, const CsrCharString * friendlyName);
CsrUint16 * CsrBtGapReturnDeviceNameWithDeviceAddr(CsrBtDeviceAddr deviceAddr, const CsrUint8 * friendlyName);


void CsrBtGapRestoreSavedMessages(CsrBtGapInstData * instData);

void CsrBtGapAddItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 position,
                  CsrUint16 key, CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtGapSetItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 key,
                                CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtGapSetDialog(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtGapSetInputDialog(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtGapMenuSet(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 *heading,
                  CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtGapShowUi(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority);

void CsrBtGapHideUi(CsrBtGapInstData * inst, CsrUint8 UiIndex);

void CsrBtGapDisplaySetInputMode(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 inputMode);

void CsrBtGapMenuRemoveAllItems(CsrBtGapInstData * inst, CsrUint8 UiIndex);

void CsrBtGapMenuRemoveItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 key);

void CsrBtGapInsertDeviceInKeyList(CsrBtGapDeviceKeyListType  ** theList,
                                   CsrBtTypedAddr typedAddr, char *deviceName,
                                   CsrUint32 deviceStatus,
                                   CsrUint16 key, CsrBool authorised);

CsrBool CsrBtGapReturnDeviceAddrFromKeyList(CsrBtGapDeviceKeyListType  * theList, CsrUint16 key,
                                            CsrBtTypedAddr *typedAddr, CsrBtDeviceName *deviceName,
                                            CsrUint32 *deviceStatus);

CsrBool CsrBtGapDeviceAddrPresentInKeyList(CsrBtGapDeviceKeyListType  * theList, CsrBtTypedAddr typedAddr,
                                           CsrUint16 *key, CsrBool *secLevel);

void CsrBtGapRemoveDeviceInKeyList(CsrBtGapDeviceKeyListType  ** theList);




#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GAP_APP_UTIL_H__ */
