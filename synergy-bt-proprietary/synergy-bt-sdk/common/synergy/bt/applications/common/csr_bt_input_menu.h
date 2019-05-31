#ifndef CSR_BT_INPUT_MENU_H_
#define CSR_BT_INPUT_MENU_H_

/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_bt_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximium number of input menu items */
#define CSR_BT_INPUT_MENU_MAX_OPTIONS           32


/* Function callback type for character input menu item */
typedef void (*CsrBtInputMenuKeyHandlerFunc)(void *opaque);

/* Function callback type for string input menu item */
typedef void (*CsrBtInputMenuStringHandlerFunc)(void *opaque, CsrCharString *string);

/* Datatype for input menu instance */
typedef struct CsrBtInputMenuKey CsrBtInputMenuKey;

/* Datatype for type of input string */
typedef CsrUint8 CsrBtInputMenuStringType;


/* Possible values for input string types */
#define CSR_BT_INPUT_MENU_CHAR                  ((CsrBtInputMenuStringType) 0)
#define CSR_BT_INPUT_MENU_STRING                ((CsrBtInputMenuStringType) 1)
#define CSR_BT_INPUT_MENU_STRING_NUM            ((CsrBtInputMenuStringType) 2)
#define CSR_BT_INPUT_MENU_STRING_ALPHA          ((CsrBtInputMenuStringType) 3)
#define CSR_BT_INPUT_MENU_STRING_ALPHA_NUMERIC  ((CsrBtInputMenuStringType) 4)
#define CSR_BT_INPUT_MENU_STRING_HEX            ((CsrBtInputMenuStringType) 5)
#define CSR_BT_INPUT_MENU_ADDRESS               ((CsrBtInputMenuStringType) 6)


/* Creates new input menu instance
 *
 * pInst :      Opaque value sent in callback functions. Normally application instance
 * exitkey :    ASCII code character input to act as exit key. Normally ESCAPE (0x1B) character */
CsrBtInputMenuKey *CsrBtInputMenuKeyInputInit(void *pInst, CsrUint8 exitKey);

/* Destroys input menu instance */
void CsrBtInputMenuKeyInputDeinit(CsrBtInputMenuKey *keyInst);

/* Adds one menu item to the menu
 *
 * keyInst:     Input menu instance
 * key:         ASCII alpha-numeric character to act as selection key for menu item
 * description: Menu item description
 * handler:     Callback function for the menu item selection
 *
 * returns TRUE is input menu item was created successfully */
CsrBool CsrBtInputMenuAddOption(CsrBtInputMenuKey *keyInst,
                                CsrCharString key,
                                CsrCharString *description,
                                CsrBtInputMenuKeyHandlerFunc handler);

/* Clears all menu items from menu
 *
 * keyInst:     Input menu instance */
void CsrBtInputMenuKeyClearOptions(CsrBtInputMenuKey *keyInst);

/* Scans string input
 *
 * keyInst:     Input menu instance
 * stringType:  Type of string expected from user
 * description: User description to print on screen
 * func:        Callback function for the string input
 *
 * returns TRUE if parameters are valid */
CsrBool CsrBtInputMenuGetString(CsrBtInputMenuKey *keyInst,
                                CsrBtInputMenuStringType stringType,
                                CsrCharString *description,
                                CsrBtInputMenuStringHandlerFunc func);

/* Key handler function. Applications are supposed to direct input characters into this function
 *
 * keyInst:     Input menu instance
 * cKey:        Input character */
void CsrBtInputMenuKeyHandler(CsrBtInputMenuKey *keyInst,
                              CsrUint8 cKey);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_INPUT_MENU_H_ */
