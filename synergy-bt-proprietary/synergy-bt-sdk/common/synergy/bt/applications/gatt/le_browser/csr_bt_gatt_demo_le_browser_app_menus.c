/****************************************************************************

Copyright (c) 2011-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
*****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_gatt_demo_le_browser_app_strings.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_le_browser_app_ui_sef.h"
#include "csr_bt_gatt_demo_le_browser_app_menus.h"
#include "csr_bt_gatt_demo_le_browser_app_defines.h"

/****************************************************************************
    Commented code concerns features not yet completely implemented.
 ****************************************************************************/

void CsrBtGattLeBrowserUpdateDbElement(CsrBtLeBrowserAppInstData *inst, CsrUint16 level)
{
    CsrUint8 primServiceCount, characCount, descCount;

    inst->dbElement->currentPrimService = CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(inst->dbElement->dbPrimServices);
    for(primServiceCount = 0; primServiceCount < inst->selectedPrimService; primServiceCount++)
    {
        /* [QTI] Fix KW issue#831740 through adding the check "inst->dbElement->currentPrimService". */
        if (inst->dbElement->currentPrimService)
        {
            inst->dbElement->currentPrimService = inst->dbElement->currentPrimService->next;
        }
    }
    
    if(level >= CSR_BT_GATT_LE_BROWSER_LVL_1)
    {
        inst->dbElement->currentCharac = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(inst->dbElement->currentPrimService->characList);
        for(characCount = 0; characCount < inst->selectedCharac; characCount++)
        {
            inst->dbElement->currentCharac = inst->dbElement->currentCharac->next;
        }
    }

    if(level >= CSR_BT_GATT_LE_BROWSER_LVL_2)
    {
        inst->dbElement->currentDescr = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC_DESCR(inst->dbElement->currentCharac->descrList);
        for(descCount = 0; descCount < inst->selectedDescriptor; descCount++)
        {
            inst->dbElement->currentDescr = inst->dbElement->currentDescr->next;
        }
    }
    return;
}

/* Menu creating functions */
void commonShowPrimaryServicesMenu(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hPrimMenu);
    if(inst->dbElement->traversingDb)
    {
        CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(inst);
    }
    else
    {
        CsrUint16   primServiceElem;
        CsrUint8    listEntryCount = 0;
        inst->dbElement->currentPrimService = CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(inst->dbElement->dbPrimServices);
                
        /*Traverses list of primary services and adds a menu item for each element.*/
        while( inst->dbElement->currentPrimService != NULL )
        {
            if(inst->dbElement->currentPrimService->uuid.length == CSR_BT_GATT_LE_BROWSER_SHORT_UUID_LEN)
            {
                CsrBtUuid16 primService;
                CsrUtf16String *primServiceName;
                CsrUint32 menuItemLabelSize = sizeof(CsrUint8)*CSR_BT_GATT_LE_BROWSER_TXT_LEN_SHORT;		
                CsrUint8 *menuItemLabel = CsrPmemZalloc(menuItemLabelSize);  
        
                primService =  (CsrUint16)(inst->dbElement->currentPrimService->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
                primService += (CsrUint16)inst->dbElement->currentPrimService->uuid.uuid[CSR_BT_GATT_LOW_BYTE];

                snprintf((void*)menuItemLabel, menuItemLabelSize, "UUID: %04X", primService);

                primServiceName = CsrUtf82Utf16String((CsrUtf8String *) CsrBtGattAppUuid16ToCharName(primService));

                CsrUiMenuAdditemReqSend(csrUiVar->hPrimMenu, 
                                        CSR_UI_LAST,
                                        (CsrUint16)(CSR_BT_LE_BROWSER_APP_PRIMARY_SERVICES_KEY + listEntryCount),
                                        CSR_UI_ICON_BOX,
                                        primServiceName,
                                        CONVERT_TEXT_STRING_2_UCS2(menuItemLabel),
                                        csrUiVar->hSk1Event,
                                        0,
                                        csrUiVar->hBackEvent, 
                                        0);
                CsrPmemFree(menuItemLabel);

            }
            else
            {
                /* Handles odd sized UUID */

                CsrUint8 i;
                CsrUint8 totalSize = sizeof(CsrUint8) * (inst->dbElement->currentPrimService->uuid.length + 1) - 1;
                CsrUint8 actualSize = CSRMIN(inst->dbElement->currentPrimService->uuid.length, CSR_BT_UUID128_SIZE);
                CsrUint8 *menuItemSublabel = CsrPmemZalloc(totalSize);

                /* [QTI] Fix KW issue#32583 through adding the check. */
                for (i = 0; i < actualSize; i += 2)
                {
                    if (i < actualSize)
                    {
                        *(menuItemSublabel + i) = inst->dbElement->currentPrimService->uuid.uuid[i] + '0';
                    }

                    if ((i + 1) < actualSize)
                    {
                        *(menuItemSublabel + i + 1) = ':'; 
                    }
                }
                /* Adds the odd sized UUID to the list */
                CsrUiMenuAdditemReqSend(csrUiVar->hPrimMenu, 
                                        CSR_UI_FIRST,
                                        (CsrUint16)(CSR_BT_LE_BROWSER_APP_PRIMARY_SERVICES_KEY + listEntryCount),
                                        CSR_UI_ICON_NONE,
                                        CONVERT_TEXT_STRING_2_UCS2("Primary Service"),
                                        CONVERT_TEXT_STRING_2_UCS2(menuItemSublabel),
                                        csrUiVar->hSk1Event,
                                        0,
                                        csrUiVar->hBackEvent, 
                                        0);
                CsrPmemFree(menuItemSublabel);
            }
            inst->dbElement->currentPrimService = inst->dbElement->currentPrimService->next;
            listEntryCount++;
        }
        if(listEntryCount > 0)
        {
            CsrBtGattAppHandleLeBrowserCsrUiHidePopup(inst);
        }
        else
        {
            /* No primary services to be listed. */
            CsrBtGattAppHandleLeBrowserCsrUiHidePopup(inst); /* Hide one popup to show the next */
            CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupSet(inst, "Reading", "No primary services found.");
            CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupShow(inst);
        }

    }
    CsrUiUieShowReqSend(inst->csrUiVar.hPrimMenu,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);

}

void commonShowCharacteristicsMenu(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{
    CsrUint8   listEntryCount;
    CsrBtGattAppDbCharacElement* currentCharac;
    CsrBtUuid16 uuid;
    CsrUtf16String *uuidName;
    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_0);
           
    uuid = (CsrUint16)(inst->dbElement->currentPrimService->uuid.uuid[1] << 8);
    uuid += (CsrUint16)inst->dbElement->currentPrimService->uuid.uuid[0];

    uuidName = CsrUtf82Utf16String((CsrUtf8String *) CsrBtGattAppUuid16ToCharName(uuid));

    CsrUiMenuSetReqSend(csrUiVar->hCharMenu,
                        uuidName,
                        TEXT_SELECT_UCS2,
                        NULL);
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hCharMenu);
            

            
    currentCharac = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(inst->dbElement->currentPrimService->characList);
    listEntryCount = 0;
    if(currentCharac == NULL)
    {
        /* No characteristics available */
        CsrUiMenuAdditemReqSend(csrUiVar->hCharMenu, 
                                CSR_UI_LAST,
                                1, 
                                CSR_UI_ICON_NONE,
                                CONVERT_TEXT_STRING_2_UCS2("No charac's found"),
                                NULL,
                                csrUiVar->hBackEvent,
                                0,
                                csrUiVar->hBackEvent, 
                                0);

    }
    else
    {

        while(currentCharac != NULL)
        {
            CsrBtUuid16 charac = 0; /* Start with unknown UUID assumption */
            CsrUtf16String *characName;
            CsrUint32   menuItemLabelSize = sizeof(CsrUint8)*(currentCharac->uuid.length + 1) + CSR_BT_GATT_LE_BROWSER_TXT_LEN_SHORT;
            CsrUint8   *menuItemLabel = CsrPmemZalloc(menuItemLabelSize);

            if(currentCharac->uuid.length == 2)
            {
                charac =  (CsrUint16)(currentCharac->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
                charac += (CsrUint16)currentCharac->uuid.uuid[CSR_BT_GATT_LOW_BYTE];

                snprintf((void*)menuItemLabel, menuItemLabelSize, "UUID: %04X", charac);
            }
            else
            {   /* To handle long UUIDs */
                CsrUint8   *longUuidLabel = CsrPmemZalloc(sizeof(CsrUint8)*(currentCharac->uuid.length + 1) - 1);
                CsrUint8ArrayToHex(currentCharac->uuid.uuid, (CsrCharString*)longUuidLabel, currentCharac->uuid.length);
                snprintf((void*)menuItemLabel, menuItemLabelSize, "UUID: %s", (char*)longUuidLabel);
                CsrPmemFree(longUuidLabel);
            }

            characName = CsrUtf82Utf16String((CsrUtf8String *) CsrBtGattAppUuid16ToCharName(charac));

            CsrUiMenuAdditemReqSend(csrUiVar->hCharMenu, 
                                    CSR_UI_LAST,
                                    (CsrUint16)(CSR_BT_LE_BROWSER_APP_CHARACTERISTICS_KEY + listEntryCount), 
                                    CSR_UI_ICON_BOX,
                                    characName,
                                    CONVERT_TEXT_STRING_2_UCS2(menuItemLabel),
                                    csrUiVar->hSk1Event,
                                    0,
                                    csrUiVar->hBackEvent, 
                                    0);

            currentCharac = currentCharac->next;
            listEntryCount++;
            CsrPmemFree(menuItemLabel);
        }
    }
    CsrUiUieShowReqSend(inst->csrUiVar.hCharMenu,
            CsrSchedTaskQueueGet(), 
            CSR_UI_INPUTMODE_AUTO, 
            CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
}

void commonShowDescriptorsMenu(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{

    CsrUint8   listEntryCount = 0;
    CsrBool     firstMark = FALSE;
    CsrUint16 textInsert = 0;
    CsrBtUuid16 uuid;
    CsrUtf16String *uuidName;
    CsrUint8  *propertyValue = CsrPmemZalloc(sizeof(CsrUint8) * 15);
    CsrUint8  *characValueStr;
    CsrUint32  characValueStrSize = 0;

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hDescMenu);

    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_2);

    uuid = (CsrUint16)(inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
    uuid += (CsrUint16)inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_LOW_BYTE];

    uuidName = CsrUtf82Utf16String((CsrUtf8String *) CsrBtGattAppUuid16ToCharName(uuid));

    CsrUiMenuSetReqSend(csrUiVar->hDescMenu,
                        uuidName,
                        TEXT_SELECT_UCS2,
                        TEXT_LE_BROWSER_APP_EDIT_UCS2);


    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST) == CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST)
    {
        CsrMemCpy((propertyValue + textInsert), "B", 1);
        textInsert += 1;
        firstMark = TRUE;
    }*/
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_READ) == CSR_BT_GATT_CHARAC_PROPERTIES_READ)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "R", sizeof("R"));
        textInsert += sizeof("R") - 1;
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "WC", sizeof("WC"));
        textInsert += sizeof("WC") - 1;
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "W", sizeof("W"));
        textInsert += sizeof("W") - 1;
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY) == CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "N", sizeof("N"));
        textInsert += sizeof("N") - 1;
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE) == CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "I", sizeof("I"));
        textInsert += sizeof("I") - 1;
        firstMark = TRUE;
    }
    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES) == CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
            textInsert += sizeof(",") - 1;
        }
        CsrMemCpy((propertyValue + textInsert), "WSC", sizeof("WSC"));
        textInsert += sizeof("WSC") - 1;
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES) == CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES)
    {
        CsrBtGattAppDbCharacDescrElement *dElem; 
        CsrBtUuid *newUuid = CsrBtGatt16BitTo128BitUuid(CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC);
        dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(inst->dbElement->currentCharac->descrList, newUuid);

        if(dElem  != NULL)
        {
            if(firstMark == TRUE)
            {
                CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
                textInsert += sizeof(",") - 1;
            }
            CsrMemCpy((propertyValue + textInsert), "E:", sizeof("E:"));
            textInsert += sizeof("E:") - 1;
            if(dElem->value != NULL)
            {
                if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE)
                {
                    CsrMemCpy((propertyValue + textInsert), "RW", sizeof("RW"));
                    textInsert += sizeof("RW") - 1;
                    firstMark = FALSE;
                }

                if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX)
                {
                    if(firstMark == FALSE)
                    {
                        CsrMemCpy((propertyValue + textInsert), ",", sizeof(","));
                        textInsert += sizeof(",") - 1;
                    }
                    CsrMemCpy((propertyValue + textInsert), "WA", sizeof("WA"));
                    textInsert += sizeof("WA") - 1;
                }
            }
        }
    }*/

    /* Allocated the memory by considering maximum of all the below conditions */
    characValueStrSize = sizeof(CsrUint8)*inst->dbElement->currentCharac->valueLength * 2 + 18;
    characValueStr = CsrPmemAlloc(characValueStrSize);
    if(inst->dbElement->currentCharac->valueLength)
    {
        CsrUint16 curUuid = (CsrUint16)(inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
        curUuid += (CsrUint16)inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_LOW_BYTE];
        if(curUuid == CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC)
        {
            float temp;
            CsrUint8 type = 'C'; /* Celcius */
            if(inst->dbElement->currentCharac->value[CSR_BT_GATT_LOW_BYTE] & CSR_BT_GATT_LE_BROWSER_TEMP_TYPE_F)
            {
                type = 'F'; /* Fahrenheit */
            }
            CsrMemCpy(&temp, &inst->dbElement->currentCharac->value[CSR_BT_GATT_HIGH_BYTE], sizeof(float));
            snprintf((void*)characValueStr, characValueStrSize, "Value: %.02f %c", temp, type);
        }
        else
        {
            CsrUint8* characValue = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8)*inst->dbElement->currentCharac->valueLength * 2 + 1 ); /* Each byte in characValue needs 2 bytes to write out */
            CsrUint8ArrayToHex(inst->dbElement->currentCharac->value, (CsrCharString*)characValue, inst->dbElement->currentCharac->valueLength);
            snprintf((void*)characValueStr, characValueStrSize, "Value: 0x%s", (char*)characValue);
            CsrPmemFree(characValue);
        }
    }
    else
    {
        snprintf((void*)characValueStr, characValueStrSize, "Value Unavailable");
    }

            
    CsrUiMenuAdditemReqSend(csrUiVar->hDescMenu, 
                            CSR_UI_LAST,
                            CSR_BT_LE_BROWSER_APP_CHARAC_VALUE_KEY, 
                            CSR_UI_ICON_BOX,
                            CONVERT_TEXT_STRING_2_UCS2(characValueStr),
                            CONVERT_TEXT_STRING_2_UCS2(propertyValue),
                            csrUiVar->hSk1Event,
                            csrUiVar->hSk2Event,
                            csrUiVar->hBackEvent, 
                            0);
    
    inst->dbElement->currentDescr = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC_DESCR(inst->dbElement->currentCharac->descrList);

    while(inst->dbElement->currentDescr != NULL)
    {
        /* [QTI] Fix KW issue#267148~267150. */
        CsrBtUuid16 charac = (CsrUint16)0x0000;
        CsrSize sizeLabel2 = sizeof(CsrUint8)*(inst->dbElement->currentDescr->uuid.length + 1) + CSR_BT_GATT_LE_BROWSER_TXT_LEN_SHORT;
        CsrUint8   *menuItemLabel2 = CsrPmemZalloc(sizeLabel2);
        CsrUint32  menuItemLabelSize = sizeof(CsrUint8)*(inst->dbElement->currentDescr->uuid.length + 1) - 1;
        CsrUint8   *menuItemLabel = CsrPmemZalloc(menuItemLabelSize);
        const CsrCharString *characName, *uuidTestName;
        if(inst->dbElement->currentCharac->uuid.length == CSR_BT_GATT_LE_BROWSER_SHORT_UUID_LEN)
        {
            charac =  (CsrUint16)(inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
            charac += (CsrUint16)inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_LOW_BYTE];
        }
        else
        {
            CsrUint8   *longUuidLabel = CsrPmemZalloc(sizeof(CsrUint8)*(inst->dbElement->currentCharac->uuid.length + 1) - 1);
                    
            CsrUint8ArrayToHex(inst->dbElement->currentDescr->uuid.uuid, (CsrCharString*)longUuidLabel, inst->dbElement->currentDescr->uuid.length);
            snprintf((void*)menuItemLabel, menuItemLabelSize, "UUID: %s", (char*)longUuidLabel);
            CsrPmemFree(longUuidLabel);
        }
        if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY) == CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY)
        {
            if(charac == CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC)
            {
                snprintf((void*)menuItemLabel2, sizeLabel2, " ");
            }
            else
            {
                if(inst->dbElement->currentDescr->value == NULL)
                {
                    snprintf((void*)menuItemLabel2, sizeLabel2, "Update: Off");
                }
                else
                {
                    snprintf((void*)menuItemLabel2, sizeLabel2, "Update: %s", (*inst->dbElement->currentDescr->value)? "On" : "Off");
                }
            }
        }
        else if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE) == CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE)
        {
            if(charac == CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC)
            {
                snprintf((void*)menuItemLabel2, sizeLabel2, " ");
            }
            else
            {
                if(inst->dbElement->currentDescr->value == NULL)
                {
                    snprintf((void*)menuItemLabel2, sizeLabel2, "Update: Off");
                }
                else
                {
                    snprintf((void*)menuItemLabel2, sizeLabel2, "Update: %s", (*inst->dbElement->currentDescr->value)? "On" : "Off");
                }
            }
        }
                
        characName = CsrBtGattAppUuid16ToCharName(charac);
        uuidTestName = CsrBtGattAppUuid16ToCharName(0); /* Unknown UUID */
        if(!CsrStrCmp(characName, uuidTestName))
        { /* Not an unknown UUID */
            snprintf((void*)menuItemLabel2, sizeLabel2, "UUID: %04X", charac);
        }

        if(inst->dbElement->currentCharac->uuid.length == CSR_BT_GATT_LE_BROWSER_SHORT_UUID_LEN)
        {
            if(charac == CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC)
            {
                /* [QTI] Fix KW issue#831774 through utilizing "snprintf" instead of "CsrMemCpy". */
                snprintf((void*)menuItemLabel2, sizeLabel2, "%s", (char*)inst->dbElement->currentDescr->value);
            }
            CsrPmemFree(menuItemLabel);
            CsrUiMenuAdditemReqSend(csrUiVar->hDescMenu,
                                    CSR_UI_LAST,
                                    (CsrUint16)(CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY + listEntryCount),
                                    CSR_UI_ICON_EMPTY,
                                    CsrUtf82Utf16String((CsrUtf8String *) characName),
                                    CONVERT_TEXT_STRING_2_UCS2(menuItemLabel2),
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 
                                    0);
        }
        else
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hDescMenu, 
                                    CSR_UI_LAST,
                                    (CsrUint16)(CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY + listEntryCount), 
                                    CSR_UI_ICON_EMPTY,
                                    CONVERT_TEXT_STRING_2_UCS2(menuItemLabel),
                                    CONVERT_TEXT_STRING_2_UCS2(menuItemLabel2),
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrPmemFree(menuItemLabel);
        }

        inst->dbElement->currentDescr = inst->dbElement->currentDescr->next;
        listEntryCount++;
        CsrPmemFree(menuItemLabel2);
    }
    
    CsrUiUieShowReqSend(inst->csrUiVar.hDescMenu,
            CsrSchedTaskQueueGet(), 
            CSR_UI_INPUTMODE_AUTO, 
            CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
    CsrPmemFree(characValueStr);
    CsrPmemFree(propertyValue);
}

void commonShowCharacValuePopup(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{
    CsrUint8 textInsert = 0;
    CsrUint16  loopCount;
            
    CsrUint8  *propertyValue = CsrPmemAlloc(sizeof(CsrUint8)*CSR_BT_GATT_LE_BROWSER_TXT_LEN_MEDIUM);
    CsrUint32 characValueSize = sizeof(CsrUint8)*CSR_BT_GATT_LE_BROWSER_TXT_LEN_LONG;
    CsrUint8  *characValue = CsrPmemAlloc(characValueSize);
    CsrBool firstMark = FALSE;
    
    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST) == CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST)
    {
        CsrMemCpy((propertyValue + textInsert), "Broadcast", sizeof("Broadcast"));
        textInsert += (sizeof("Broadcast") - 1);
        firstMark = TRUE;
    }*/
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_READ) == CSR_BT_GATT_CHARAC_PROPERTIES_READ)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Read", sizeof("Read"));
        textInsert += (sizeof("Read") - 1);
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Write Command", sizeof("Write Command"));
        textInsert += (sizeof("Write Command") - 1);
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Write", sizeof("Write"));
        textInsert += (sizeof("Write") - 1);
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY) == CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Notify", sizeof("Notify"));
        textInsert += (sizeof("Notify") - 1);
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE) == CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Indicate", sizeof("Indicate"));
        textInsert += (sizeof("Indicate") - 1);
        firstMark = TRUE;
    }
    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES) == CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES)
    {
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Write Sign Command", sizeof("Write Sign Command"));
        textInsert += (sizeof("Write Sign Command") - 1);
        firstMark = TRUE;
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES) == CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES)
    {
        CsrBtUuid *uuid = CsrBtGatt16BitTo128BitUuid(CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC);
        CsrBtGattAppDbCharacDescrElement *dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(inst->dbElement->currentCharac->descrList, uuid);
        if(firstMark == TRUE)
        {
            CsrMemCpy((propertyValue + textInsert), ",", 1);
            textInsert += 1;
        }
        CsrMemCpy((propertyValue + textInsert), "Ext:", sizeof("Ext:"));
        textInsert += (sizeof("Ext:") - 1);
        firstMark = TRUE;

        if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE)
        {
            CsrMemCpy((propertyValue + textInsert), "Reliable Write", sizeof("Reliable Write"));
            textInsert += (sizeof("Reliable Write") - 1);
            firstMark = FALSE;
        }
        if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX)
        {
            if(firstMark == FALSE)
            {
                CsrMemCpy((propertyValue + textInsert), ",Writable Aux", sizeof(",Writable Aux"));
                textInsert += (sizeof(",Writable Aux") - 1);
            }
            else
            {
                CsrMemCpy((propertyValue + textInsert), "Writable Aux", sizeof("Writable Aux"));
                textInsert += (sizeof("Writable Aux") - 1);
            }
        }
    }*/

            
    if(inst->dbElement->currentCharac->valueLength)
    {
        CsrUint16 curUuid = (CsrUint16)(inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_HIGH_BYTE] << CSR_BT_GATT_LE_BROWSER_BYTELEN);
        curUuid += (CsrUint16)inst->dbElement->currentCharac->uuid.uuid[CSR_BT_GATT_LOW_BYTE];
        if(curUuid == CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC)
        {
            float temp;
            CsrUint8 type = 'C'; /* Celcius */
            if(inst->dbElement->currentCharac->value[CSR_BT_GATT_LOW_BYTE] & CSR_BT_GATT_LE_BROWSER_TEMP_TYPE_F)
            {
                type = 'F'; /* Fahrenheit */
            }
                    
            CsrMemCpy(&temp, &inst->dbElement->currentCharac->value[CSR_BT_GATT_HIGH_BYTE], sizeof(float));
            snprintf((void*)characValue, characValueSize, "Value: %.02f %c\nProperties: %s", temp, type, (char*)propertyValue);
        }
        else
        {
            if(inst->dbElement->currentCharac->valueLength <=4)
            {
                /* Limited to a valueLength of 4 bytes  */
                CsrUint32 currentValue = 0;
                for(loopCount = 0; loopCount < inst->dbElement->currentCharac->valueLength ; loopCount++)
                {
                    currentValue = (currentValue << 8) + *(inst->dbElement->currentCharac->value + loopCount);
                }
                    
                if(inst->dbElement->currentCharac->value[inst->dbElement->currentCharac->valueLength - 1] != '\0')
                {
                    CsrUint8* newValueStr = (CsrUint8*)CsrPmemZalloc(inst->dbElement->currentCharac->valueLength + 1);
                    CsrMemCpy(newValueStr, inst->dbElement->currentCharac->value, inst->dbElement->currentCharac->valueLength);
                    snprintf((void*)characValue, characValueSize, "Value: 0x%0*X\nProperties: %s\nValueStr: %s", inst->dbElement->currentCharac->valueLength*2, 
                                                                                        currentValue, 
                                                                                        (char*)propertyValue,
                                                                                        (char*)newValueStr);
                    CsrPmemFree(newValueStr);
                }
                else
                {
                    snprintf((void*)characValue, characValueSize, "Value: 0x%0*X\nProperties: %s\nValueStr: %s", inst->dbElement->currentCharac->valueLength*2, 
                                                                                        currentValue, 
                                                                                        (char*)propertyValue,
                                                                                        (char*)inst->dbElement->currentCharac->value);
                }
            }
            else
            {

                if(inst->dbElement->currentCharac->value[inst->dbElement->currentCharac->valueLength - 1] != '\0')
                {
                    CsrUint8* newValueStr = (CsrUint8*)CsrPmemZalloc(inst->dbElement->currentCharac->valueLength + 1);
                    CsrMemCpy(newValueStr, inst->dbElement->currentCharac->value, inst->dbElement->currentCharac->valueLength);
                    snprintf((void*)characValue, characValueSize, "Value: NaN\nProperties: %s\nValueStr: %s", (char*)propertyValue, (char*)newValueStr);
                    CsrPmemFree(newValueStr);
                }
                else
                {
                    snprintf((void*)characValue, characValueSize, "Value: NaN\nProperties: %s\nValueStr: %s", (char*)propertyValue, (char*)inst->dbElement->currentCharac->value);
                }
            }
        }
    }
    else
    {
        snprintf((void*)characValue, characValueSize, "Value unavailable\nProperties: %s", (char*)propertyValue);
    }

    CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupSet(inst, "Characteristic value", (char*)characValue); 
    CsrPmemFree(characValue);
    CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupShow(inst);
    CsrPmemFree(propertyValue);

}

void commonShowEditMenu(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hEditMenu);

    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_2);

    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST) == CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST)
    {
        CsrUint8  *subMenuLabel = CsrPmemZalloc(sizeof(CsrUint8)*50);
        if(inst->broadcasting == FALSE)
        {
            sprintf((void*)subMenuLabel, "Status: Off");
        }
        else
        {
            sprintf((void*)subMenuLabel, "Status: %s", (*inst->dbElement->currentDescr->value)? "On" : "Off");
        }
        
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_BROADCAST_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Broadcast"),
                                CONVERT_TEXT_STRING_2_UCS2(subMenuLabel),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
    }*/
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_READ) == CSR_BT_GATT_CHARAC_PROPERTIES_READ)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_READ_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Read Value"),
                                CONVERT_TEXT_STRING_2_UCS2("  "),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_WRITECOMMAND_RAW_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Write Command"),
                                CONVERT_TEXT_STRING_2_UCS2("Input raw data"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_WRITECOMMAND_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Write Command"),
                                CONVERT_TEXT_STRING_2_UCS2("Input string"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_WRITE_RAW_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Write"),
                                CONVERT_TEXT_STRING_2_UCS2("Input raw data"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_WRITE_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Write"),
                                CONVERT_TEXT_STRING_2_UCS2("Input string"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY) == CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY)
    {
        CsrUint32 subMenuLabelSize = sizeof(CsrUint8)*50;
        CsrUint8  *subMenuLabel = CsrPmemZalloc(subMenuLabelSize);
        if(inst->dbElement->currentDescr->value == NULL)
        {
            snprintf((void*)subMenuLabel, subMenuLabelSize, "Status: Off");
        }
        else
        {
            snprintf((void*)subMenuLabel, subMenuLabelSize, "Status: %s", (*inst->dbElement->currentDescr->value)? "On" : "Off");
        }

        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_NOTIFY_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Toggle Notify"),
                                CONVERT_TEXT_STRING_2_UCS2(subMenuLabel),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
        CsrPmemFree(subMenuLabel);
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE) == CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE)
    {
        CsrUint32 subMenuLabelSize = sizeof(CsrUint8)*12;
        CsrUint8  *subMenuLabel = CsrPmemZalloc(subMenuLabelSize);
        if(inst->dbElement->currentDescr->value == NULL)
        {
            snprintf((void*)subMenuLabel, subMenuLabelSize, "Status: Off");
        }
        else
        {
            snprintf((void*)subMenuLabel, subMenuLabelSize, "Status: %s", (*inst->dbElement->currentDescr->value)? "On" : "Off");
        }

        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_INDICATE_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Toggle Indicate"),
                                CONVERT_TEXT_STRING_2_UCS2(subMenuLabel),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
        CsrPmemFree(subMenuLabel);
    }
    /*if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES) == CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_AUTH_WRITES_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Auth Sign Write"),
                                CONVERT_TEXT_STRING_2_UCS2("Input string"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                CSR_UI_LAST,
                                CSR_BT_LE_BROWSER_APP_AUTH_WRITES_RAW_KEY, 
                                CSR_UI_ICON_BOX,
                                CONVERT_TEXT_STRING_2_UCS2("Auth Sign Write"),
                                CONVERT_TEXT_STRING_2_UCS2("Input raw data"),
                                csrUiVar->hSk1Event,
                                0,
                                csrUiVar->hBackEvent, 
                                0);
    }
    if((inst->dbElement->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES) == CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES)
    {*/
        /* List of available extended properties may be extended */
        /*CsrBtUuid* uuid = CsrBtGatt16BitTo128BitUuid(CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC);

        CsrBtGattAppDbCharacDescrElement *dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(inst->dbElement->currentCharac->descrList, uuid);

        if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE)
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                    CSR_UI_LAST,
                                    CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_RAW_KEY, 
                                    CSR_UI_ICON_BOX,
                                    CONVERT_TEXT_STRING_2_UCS2("Ext:Reliable Write"),
                                    CONVERT_TEXT_STRING_2_UCS2("Input raw data"),
                                    csrUiVar->hSk1Event,
                                    0,
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                    CSR_UI_LAST,
                                    CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_KEY, 
                                    CSR_UI_ICON_BOX,
                                    CONVERT_TEXT_STRING_2_UCS2("Ext:Reliable Write"),
                                    CONVERT_TEXT_STRING_2_UCS2("Input string"),
                                    csrUiVar->hSk1Event,
                                    0,
                                    csrUiVar->hBackEvent, 
                                    0);
        }
        if((*dElem->value & CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX) == CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX)
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hEditMenu, 
                                    CSR_UI_LAST,
                                    CSR_BT_LE_BROWSER_APP_EXT_WRITABLE_AUX_KEY, 
                                    CSR_UI_ICON_BOX,
                                    CONVERT_TEXT_STRING_2_UCS2("Ext:Writable AUX"),
                                    CONVERT_TEXT_STRING_2_UCS2("  "),
                                    csrUiVar->hSk1Event,
                                    0,
                                    csrUiVar->hBackEvent, 
                                    0);
        }
        CsrPmemFree(uuid);
    }*/
    CsrUiUieShowReqSend(inst->csrUiVar.hEditMenu,
            CsrSchedTaskQueueGet(), 
            CSR_UI_INPUTMODE_AUTO, 
            CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
}

void commonShowDescrValuePopup(CsrBtLeBrowserAppInstData *inst, CsrBtLeBrowserAppUiInstData *csrUiVar)
{
    CsrSize size = sizeof(CsrUint8)*20;
    CsrUint8   *popUpLabel = CsrPmemZalloc(size);
    CsrUint16 uuid;
    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_2);
    uuid = inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_HIGH_BYTE];
    uuid = (uuid << CSR_BT_GATT_LE_BROWSER_BYTELEN) + inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_LOW_BYTE];
    if(uuid == CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC)
    {
        /* [QTI] Fix KW issue#831775 through utilizing "snprintf" instead of "CsrMemCpy". */
        snprintf((void*)popUpLabel, size, "%s", (char*)inst->dbElement->currentDescr->value);
    }
    else
    {
        if(inst->dbElement->currentDescr->value != NULL)
        {
            snprintf((void*)popUpLabel, size, "Value: %0*X",inst->dbElement->currentDescr->valueLength, *inst->dbElement->currentDescr->value);
        }
        else
        {
            snprintf((void*)popUpLabel, size, "Value: 0");
        }
    }
    CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupSet(inst, "Descriptor", (char*)popUpLabel);
    CsrPmemFree(popUpLabel);
    CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupShow(inst);
}



void CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(CsrBtLeBrowserAppInstData *inst, CsrUint16 menuState)
{
    switch(menuState)
    {
        case CSR_BT_LE_BROWSER_PRIMARY_SERVICES_MENU_STATE:
            {
                commonShowPrimaryServicesMenu(inst, &inst->csrUiVar);
                break;
            }
        case CSR_BT_LE_BROWSER_CHARAC_MENU_STATE:
            {
                commonShowCharacteristicsMenu(inst, &inst->csrUiVar);
                break;
            }
        case CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE:
            {
                commonShowDescriptorsMenu(inst, &inst->csrUiVar);
                break;
            }
        case CSR_BT_LE_BROWSER_CHARAC_VALUE_MENU_STATE:
            {
                commonShowCharacValuePopup(inst, &inst->csrUiVar);
                break;
            }
        case CSR_BT_LE_BROWSER_EDIT_MENU_STATE:
            {
                commonShowEditMenu(inst, &inst->csrUiVar);
                break;
            }
        case CSR_BT_LE_BROWSER_VALUE_MENU_STATE:
            {
                /* Currently no need to show the descriptor value if it is not a user description */
                commonShowDescrValuePopup(inst, &inst->csrUiVar);
                break;
            }
        default:
            {
                /* Unknown menu state */
            }
    }
}

