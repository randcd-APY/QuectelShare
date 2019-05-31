/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_input_menu.h"
#include "csr_pmem.h"
#include "csr_sched_init.h"

typedef struct
{
    CsrCharString key;
    CsrBtInputMenuKeyHandlerFunc keyHandler;
} CsrBtInputMenuKeyHandlers;

typedef struct CsrBtInputMenuKey
{
    void *pInst;
    CsrUint8 exitKey;
    CsrBtInputMenuStringType inputType;
    CsrUint8 keyOptions;
    CsrBtInputMenuKeyHandlers keyHandler[CSR_BT_INPUT_MENU_MAX_OPTIONS];
    CsrBtInputMenuStringHandlerFunc stringHandler;
    CsrCharString string[CSR_BT_MAX_PATH_LENGTH];
    CsrUint16 stringLen;
} CsrBtInputMenuKey;

static void csrBtInputMenuHandleKey(CsrBtInputMenuKey *keyInst, CsrUint8 cKey)
{
    CsrUint8 i;
    for (i = 0; i < keyInst->keyOptions; i++)
    {
        CsrBtInputMenuKeyHandlers* keyHandler = &keyInst->keyHandler[i];
        if (cKey == keyHandler->key)
        {
            keyHandler->keyHandler(keyInst->pInst);
            break;
        }
    }
}

static void csrBtInputMenuEraseChar(CsrBtInputMenuKey *keyInst)
{
    if (keyInst->stringLen > 0)
    {
        keyInst->stringLen--;
        printf("\b \b");
    }
}

static void csrBtInputMenuStringSend(CsrBtInputMenuKey *keyInst)
{
    keyInst->string[keyInst->stringLen] = '\0';
    keyInst->inputType = CSR_BT_INPUT_MENU_CHAR;
    keyInst->stringHandler(keyInst->pInst, keyInst->string);
    keyInst->stringLen = 0;
}

static void csrBtInputMenuWriteChar(CsrBtInputMenuKey* keyInst, CsrUint8 cKey)
{
    CsrBool add = FALSE;
    switch (keyInst->inputType)
    {
        case CSR_BT_INPUT_MENU_ADDRESS:
        if (cKey == ':')
        {
            add = TRUE;
            break;
        }
        /*Fallthrough*/
        case CSR_BT_INPUT_MENU_STRING_HEX:
        if ((cKey >= 'A' && cKey <= 'F') || (cKey >= 'a' || cKey <= 'f'))
        {
            add = TRUE;
            break;
        }
        /* Fallthrough */
        case CSR_BT_INPUT_MENU_STRING_NUM:
        if (cKey >= '0' && cKey <= '9')
        {
            add = TRUE;
        }
            break;
        case CSR_BT_INPUT_MENU_STRING:
        if ((cKey >= 32) && (cKey < 127))
        {
            /* Printable character */
            add = TRUE;
        }
            break;
        case CSR_BT_INPUT_MENU_STRING_ALPHA_NUMERIC:
        if (cKey >= '0' && cKey <= '9')
        {
            add = TRUE;
            break;
        }
        /* Fallthrough */
        case CSR_BT_INPUT_MENU_STRING_ALPHA:
        if ((cKey >= 'A' && cKey <= 'Z') || (cKey >= 'a' && cKey <= 'z'))
        {
            add = TRUE;
        }
            break;
    }

    if (add != FALSE)
    {
        keyInst->string[keyInst->stringLen] = cKey;
        keyInst->stringLen++;
        printf("%c", cKey);
        if (keyInst->stringLen >= CSR_BT_MAX_PATH_LENGTH)
        {
            keyInst->stringLen--;
            csrBtInputMenuStringSend(keyInst);
        }
    }
}

CsrBool CsrBtInputMenuAddOption(CsrBtInputMenuKey *keyInst,
                                CsrCharString key,
                                CsrCharString *description,
                                CsrBtInputMenuKeyHandlerFunc handler)
{
    if (keyInst->keyOptions < CSR_BT_INPUT_MENU_MAX_OPTIONS
        && handler != NULL
        && (key >= 32 && key < 127 && key != keyInst->exitKey))
    {
        printf("\t%c)\t%s\n", key, description);
        keyInst->inputType = CSR_BT_INPUT_MENU_CHAR;
        keyInst->keyHandler[keyInst->keyOptions].key = key;
        keyInst->keyHandler[keyInst->keyOptions].keyHandler = handler;
        keyInst->keyOptions++;
        return (TRUE);
    }
    return (FALSE);
}

void CsrBtInputMenuKeyClearOptions(CsrBtInputMenuKey *keyInst)
{
    keyInst->keyOptions = 0;
}

CsrBool CsrBtInputMenuGetString(CsrBtInputMenuKey *keyInst,
                                CsrBtInputMenuStringType stringType,
                                CsrCharString *description,
                                CsrBtInputMenuStringHandlerFunc func)
{
    switch (stringType)
    {
        case CSR_BT_INPUT_MENU_STRING:
        case CSR_BT_INPUT_MENU_STRING_NUM:
        case CSR_BT_INPUT_MENU_STRING_ALPHA:
        case CSR_BT_INPUT_MENU_STRING_ALPHA_NUMERIC:
        case CSR_BT_INPUT_MENU_STRING_HEX:
        case CSR_BT_INPUT_MENU_ADDRESS:
        keyInst->inputType = stringType;
        keyInst->stringHandler = func;
        keyInst->stringLen = 0;
        printf("\n\t%s:\t", description);
        return (TRUE);
        default:
        return (FALSE);
    }
}

CsrBtInputMenuKey *CsrBtInputMenuKeyInputInit(void *pInst, CsrUint8 exitKey)
{
    CsrBtInputMenuKey *keyInst = (CsrBtInputMenuKey *) CsrPmemZalloc(sizeof(CsrBtInputMenuKey));
    keyInst->pInst = pInst;
    keyInst->exitKey = exitKey;
    return (keyInst);
}

void CsrBtInputMenuKeyInputDeinit(CsrBtInputMenuKey *keyInst)
{
    CsrPmemFree(keyInst);
}

void CsrBtInputMenuKeyHandler(CsrBtInputMenuKey *keyInst,
                          CsrUint8 cKey)
{
    if (cKey == keyInst->exitKey)
    {
        printf("\nUser exit...\n");
#ifdef ENABLE_SHUTDOWN
        CsrSchedStop();
#else
        exit(0);
#endif
        return;
    }

    if (keyInst->inputType == CSR_BT_INPUT_MENU_CHAR)
    {
        csrBtInputMenuHandleKey(keyInst, cKey);
    }
    /* We are scanning string */
    else if (cKey == '\n')
    {
        /* Return pressed. Send accumulated string to application */
        csrBtInputMenuStringSend(keyInst);
    }
    else if (cKey == '\b')
    {
        csrBtInputMenuEraseChar(keyInst);
    }
    else
    {
        /* Add character to string */
        csrBtInputMenuWriteChar(keyInst, cKey);
    }
}
