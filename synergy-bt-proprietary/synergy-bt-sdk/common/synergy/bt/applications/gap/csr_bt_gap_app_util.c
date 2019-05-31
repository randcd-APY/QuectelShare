/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "csr_types.h"
#include "csr_app_prim.h"
#include "csr_bt_gap_app_util.h"
#include "csr_bt_gap_app_ui_sef.h"
#include "csr_bt_util.h"
#include "csr_pmem.h"
#include "csr_formatted_io.h"
#include "csr_bt_sd_prim.h"

CsrUint16 * CsrBtGapDeviceAddr2UnicodeString(CsrBtDeviceAddr deviceAddr, CsrUint32 deviceStatus)
{
    CsrCharString bdAddrStr[16+12];

    if((CSR_BT_SD_STATUS_RADIO_LE & deviceStatus) && !(CSR_BT_SD_STATUS_RADIO_BREDR & deviceStatus))
    {
        /* LE only */
        CsrSnprintf(bdAddrStr, CSR_ARRAY_SIZE(bdAddrStr), "%04x:%02x:%06x (%s)", deviceAddr.nap, deviceAddr.uap, deviceAddr.lap, ((deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? "Random":"Public" ));
    }
    else
    {
        CsrSnprintf(bdAddrStr, CSR_ARRAY_SIZE(bdAddrStr), "%04x:%02x:%06x", deviceAddr.nap, deviceAddr.uap, deviceAddr.lap);
    }
    return (CONVERT_TEXT_STRING_2_UCS2(bdAddrStr));
}

CsrCharString * CsrBtGapDeviceAddr2CharString(CsrBtDeviceAddr deviceAddr)
{
    CsrCharString bdAddrStr[16];
    CsrSnprintf(bdAddrStr, CSR_ARRAY_SIZE(bdAddrStr), "%04x:%02x:%06x", deviceAddr.nap, deviceAddr.uap, deviceAddr.lap);

    return (CsrStrDup(bdAddrStr));
}

CsrUint16 * CsrBtGapReturnUcs2DeviceName(CsrBtDeviceAddr deviceAddr, const CsrUint8 * friendlyName)
{
    CsrUint16 * deviceName;

    if (friendlyName && CsrStrLen((char *)friendlyName) > 0)
    { /* Convert the friendly name to a unicodeString                       */
        deviceName =CONVERT_TEXT_STRING_2_UCS2(friendlyName);
    }
    else
    { /* The friendly name is invalid convert the Bluetooth device address
         to a unicodeString                                                 */
        deviceName = CsrBtGapDeviceAddr2UnicodeString(deviceAddr, 0);
    }
    return deviceName;

}

CsrCharString * CsrBtGapReturnCharStringDeviceName(CsrBtDeviceAddr deviceAddr, const CsrCharString * friendlyName)
{
    CsrCharString * deviceName;

    if (friendlyName && CsrStrLen((char *)friendlyName) > 0)
    { /* Convert the friendly name to a unicodeString                       */
        deviceName = CsrStrDup(friendlyName);
    }
    else
    { /* The friendly name is invalid convert the Bluetooth device address
         to a unicodeString                                                 */
        deviceName = CsrBtGapDeviceAddr2CharString(deviceAddr);
    }
    return deviceName;

}

CsrUint16 * CsrBtGapReturnDeviceNameWithDeviceAddr(CsrBtDeviceAddr deviceAddr, const CsrUint8 * friendlyName)
{
    CsrUint16 *ucs2Str;
    char *asciiDeviceName;
    CsrSize asciiDeviceNameLength = CsrStrLen((char *)friendlyName) + 20;

    asciiDeviceName = CsrPmemAlloc(asciiDeviceNameLength);

    CsrSnprintf(asciiDeviceName, asciiDeviceNameLength, "%s\n(%04x:%02x:%06x)",friendlyName, deviceAddr.nap, deviceAddr.uap, deviceAddr.lap);

    ucs2Str = CONVERT_TEXT_STRING_2_UCS2(asciiDeviceName);
    CsrPmemFree(asciiDeviceName);

    return ucs2Str;
}


void CsrBtGapAddItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 position,
                  CsrUint16 key, CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel)
{ /* Add a Menu item to an UI                                               */
    CsrUieHandle menu, sk1Handle, sk2Handle, backHandle, delHandle;

    menu        = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiMenuAdditemReqSend(menu, position, key, icon, label, sublabel,
                                sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtGapSetItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 key,
                                CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel)
{ /* Set a Menu item to an UI                                               */
    CsrUieHandle menu, sk1Handle, sk2Handle, backHandle, delHandle;

    menu        = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiMenuSetitemReqSend(menu, key, icon, label, sublabel,
                    sk1Handle, sk2Handle, backHandle, delHandle);
}


void CsrBtGapSetDialog(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a Dialog Menu on the UI                                            */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiDialogSetReqSend(dialog, heading, message, textSK1, textSK2,
                            sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtGapSetInputDialog(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a input Dialog Menu on the UI                                      */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiInputdialogSetReqSend(dialog, heading, message, icon, text, textLength, keyMap,
                            textSK1, textSK2, sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtGapMenuSet(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 *heading,
                  CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set the Menu on the UI                                                 */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuSetReqSend(displayHandle, heading, textSK1, textSK2);
}


void CsrBtGapShowUi(CsrBtGapInstData * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority)
{ /* Show a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiUieShowReqSend(displayHandle, CsrSchedTaskQueueGet(), inputMode, priority);
}

void CsrBtGapHideUi(CsrBtGapInstData * inst, CsrUint8 UiIndex)
{ /* Hide a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiUieHideReqSend(displayHandle);
}

void CsrBtGapDisplaySetInputMode(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 inputMode)
{ /* Set the display mode on a given display                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiDisplaySetinputmodeReqSend(displayHandle, inputMode);

}

void CsrBtGapMenuRemoveAllItems(CsrBtGapInstData * inst, CsrUint8 UiIndex)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuRemoveallitemsReqSend(displayHandle);
}

void CsrBtGapMenuRemoveItem(CsrBtGapInstData * inst, CsrUint8 UiIndex, CsrUint16 key)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuRemoveitemReqSend(displayHandle, key);
}

static void appClearDeviceListType(CsrBtGapDeviceListType * deviceElement)
{ /* Initialize the BtManDeviceListType                                     */
    deviceElement->csrUiKey          = CSR_BT_GAP_INVALID_KEY;
    deviceElement->authorised       = FALSE;
    CsrBtBdAddrZero(&(deviceElement->deviceAddr));
}

static void appClearDeviceKeyList(CsrBtGapDeviceKeyListType * deviceKeyElement)
{ /* Initialize the BtManDeviceKeyListType                                  */
    CsrUint16 i;

    for (i = 0; i < DEVICE_LIST_POOL_SIZE; i++)
    {
        appClearDeviceListType(&(deviceKeyElement->deviceListEntryArray[i]));
    }
    deviceKeyElement->nextQueuePool               = NULL;
    deviceKeyElement->numberOfMessagesInThisPool  = 0;
}

/* Public util functions which is associated to the CsrBtGapDeviceKeyListType
   which is a linket list                                                   */
void CsrBtGapInsertDeviceInKeyList(CsrBtGapDeviceKeyListType  ** theList,
                                   CsrBtTypedAddr typedAddr, char *deviceName,
                                   CsrUint32 deviceStatus, CsrUint16 key, CsrBool authorised)
{ /* Insert a new device address into the given linked list                 */
    CsrBtGapDeviceKeyListType  * theElement;
    CsrBtGapDeviceListType     * deviceElement;

    theElement = *theList;

    if (theElement)
    { /* Has at least one element in the linked list                        */
        CsrBtGapDeviceKeyListType  * previousElement;

        do
        { /* Goes thought the linked list                                   */
            if (theElement->numberOfMessagesInThisPool < DEVICE_LIST_POOL_SIZE)
            { /* There is an empty entry in this pool                       */
                CsrUint16    i;

                for ( i = 0; i < DEVICE_LIST_POOL_SIZE; i++)
                { /* Goes though the element to find an empty entry         */
                    if (CsrBtBdAddrEqZero(&(theElement->deviceListEntryArray[i].deviceAddr)))
                    { /* Insert the device in the element and return        */
                        deviceElement = &(theElement->deviceListEntryArray[i]);
                        theElement->numberOfMessagesInThisPool++;
                        CsrBtBdAddrCopy(&(deviceElement->deviceAddr), &(typedAddr.addr));
                        deviceElement->deviceAddrType = typedAddr.type;
                        CsrUtf8StrNCpyZero(deviceElement->deviceName, (CsrUtf8String *) (deviceName ? deviceName : ""), sizeof(CsrBtDeviceName));
                        deviceElement->csrUiKey      = key;
                        deviceElement->deviceStatus  = deviceStatus;
                        deviceElement->authorised    = authorised;
                        return;
                    }
                }
            }
            previousElement   = theElement;
            theElement        = theElement->nextQueuePool;
        }while (theElement);

        /* All elements in the linked list were filled, allocate a new
          element to the linked list                                        */
        theElement                        = (CsrBtGapDeviceKeyListType *) CsrPmemAlloc(sizeof(CsrBtGapDeviceKeyListType));
        previousElement->nextQueuePool    = theElement;
    }
    else
    { /* No element in the linked list. Allocate one                        */
        theElement  = (CsrBtGapDeviceKeyListType *) CsrPmemAlloc(sizeof(CsrBtGapDeviceKeyListType));
        *theList    = theElement;
    }
    /* Initialize the new allocated element                                 */
    appClearDeviceKeyList(theElement);

    /* Insert the device in the new allocated element                       */
    theElement->numberOfMessagesInThisPool++;
    deviceElement = &(theElement->deviceListEntryArray[0]);
    CsrBtBdAddrCopy(&(deviceElement->deviceAddr), &(typedAddr.addr));
    deviceElement->deviceAddrType = typedAddr.type;
    CsrUtf8StrNCpyZero(deviceElement->deviceName, (CsrUtf8String *) (deviceName ? deviceName : ""), sizeof(CsrBtDeviceName));
    deviceElement->csrUiKey      = key;
    deviceElement->deviceStatus  = deviceStatus;
    deviceElement->authorised    = authorised;
}

CsrBool CsrBtGapReturnDeviceAddrFromKeyList(CsrBtGapDeviceKeyListType  * theList, CsrUint16 key,
                                            CsrBtTypedAddr *typedAddr, CsrBtDeviceName *deviceName,
                                            CsrUint32 *deviceStatus)
{ /* Return the deviceAddr that this linked to the key                      */
    CsrBtGapDeviceKeyListType  * theElement = theList;

    if (theElement)
    { /* Has at least one element in the linked list                        */
        CsrUint16                i;

        do
        { /* Goes thought the linked list                                   */
            for ( i = 0; i < DEVICE_LIST_POOL_SIZE; i++)
            {
                if (key == theElement->deviceListEntryArray[i].csrUiKey)
                { /* Found a key that match. Return the deviceAddr it is
                     linked to and return TRUE                          */
                     CsrBtBdAddrCopy(&typedAddr->addr, &(theElement->deviceListEntryArray[i].deviceAddr));
                     typedAddr->type = theElement->deviceListEntryArray[i].deviceAddrType;
                     CsrUtf8StrNCpyZero(*deviceName, theElement->deviceListEntryArray[i].deviceName, sizeof(CsrBtDeviceName));
                     *deviceStatus = theElement->deviceListEntryArray[i].deviceStatus;
                    return TRUE;
                }
            }

            theElement        = theElement->nextQueuePool;

        } while (theElement);
    }
    /* Did not found any key that match, return FALSE                       */
    return FALSE;
}

CsrBool CsrBtGapDeviceAddrPresentInKeyList(CsrBtGapDeviceKeyListType  * theList, CsrBtTypedAddr typedAddr,
                                       CsrUint16 *key, CsrBool *secLevel)
{ /* Return the key, secLevel, and headsetPriority that in linked
     to the deviceAddr                                                      */
    CsrBtGapDeviceKeyListType  * theElement = theList;

    if (theElement)
    { /* Has at least one element in the linked list                        */
        CsrUint16                i;

        do
        {
            for ( i = 0; i < DEVICE_LIST_POOL_SIZE; i++)
            {
                if ((CsrBtBdAddrEq(&(typedAddr.addr), &(theElement->deviceListEntryArray[i].deviceAddr))) &&
                    (typedAddr.type == theElement->deviceListEntryArray[i].deviceAddrType))
                { /* Found a deviceAddr that match. Return the key,
                     secLevel, headsetPriority  it is linked to
                     and return TRUE                                    */
                    *key             = theElement->deviceListEntryArray[i].csrUiKey;
                    *secLevel        = theElement->deviceListEntryArray[i].authorised;
                    return TRUE;
                }
            }

            theElement        = theElement->nextQueuePool;

        } while (theElement);
    }
    /* Did not found any deviceAddr that match, return FALSE                */
    return FALSE;
}

void CsrBtGapRemoveDeviceInKeyList(CsrBtGapDeviceKeyListType  ** theList)
{ /* Remove all elements from the linked list                               */
    CsrBtGapDeviceKeyListType  * currentElement;
    CsrBtGapDeviceKeyListType  * nextElement;

    nextElement = *theList;

    if (nextElement)
    {
        do
        {
            currentElement = nextElement;
            nextElement    = currentElement->nextQueuePool;
            CsrPmemFree(currentElement);
        } while (nextElement);
    }
    *theList = NULL;
}

