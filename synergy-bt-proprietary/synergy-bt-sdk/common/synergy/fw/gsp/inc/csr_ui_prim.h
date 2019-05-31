#ifndef CSR_UI_PRIM_H__
#define CSR_UI_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_ui_task.h"
#include "csr_ui_keycode.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrUiPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrUiPrim;
typedef CsrUint16 CsrUieHandle;
typedef CsrUint16 CsrUieType;

/* UIE types */
#define CSR_UI_UIETYPE_EVENT           0
#define CSR_UI_UIETYPE_MENU            1
#define CSR_UI_UIETYPE_DIALOG          2
#define CSR_UI_UIETYPE_INPUTDIALOG     3
#define CSR_UI_UIETYPE_IDLESCREEN      4

/* Input modes */
#define CSR_UI_INPUTMODE_AUTO          0 /* Key presses are filtered by UI, rest discarded */
#define CSR_UI_INPUTMODE_AUTOPASS      1 /* Key presses are filtered by UI, rest sent to application */
#define CSR_UI_INPUTMODE_BLOCK         2 /* All key presses are discarded */
#define CSR_UI_INPUTMODE_PASS          3 /* All key presses are sent to application */

/* Default action */
#define CSR_UI_DEFAULTACTION           0

/* Menu Item Positions */
#define CSR_UI_LAST                    0
#define CSR_UI_FIRST                   1

/* Priority Boundaries */
#define CSR_UI_LOWESTPRIORITY          0
#define CSR_UI_HIGHESTPRIORITY       255

/* Valid values for state of headset in CSR_UI_STATUS_HEADSET_SET_REQ */
#define CSR_UI_STATUS_HEADSET_PRESERVE                    -1          /* Retain the current state */
#define CSR_UI_STATUS_HEADSET_NOT_PRESENT                  0          /* Set to not present */
#define CSR_UI_STATUS_HEADSET_PRESENT                      1          /* Set to present (no indicator) */
#define CSR_UI_STATUS_HEADSET_PRESENT_INDICATOR(value)    (2 + value) /* Set to present with indicator (range: 0-100) */

/* Include icon defines from auto generated file */
#include "csr_ui_icon_index.h"

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_UI_PRIM_DOWNSTREAM_LOWEST                              (0x0000)

#define CSR_UI_UIE_CREATE_REQ                         ((CsrUiPrim) (0x0000 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_SET_REQ                           ((CsrUiPrim) (0x0001 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_GET_REQ                           ((CsrUiPrim) (0x0002 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_ADDITEM_REQ                       ((CsrUiPrim) (0x0003 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_REMOVEITEM_REQ                    ((CsrUiPrim) (0x0004 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_REMOVEALLITEMS_REQ                ((CsrUiPrim) (0x0005 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_SETITEM_REQ                       ((CsrUiPrim) (0x0006 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_GETITEM_REQ                       ((CsrUiPrim) (0x0007 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_SETCURSOR_REQ                     ((CsrUiPrim) (0x0008 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_MENU_GETCURSOR_REQ                     ((CsrUiPrim) (0x0009 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_EVENT_SET_REQ                          ((CsrUiPrim) (0x000A + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_EVENT_GET_REQ                          ((CsrUiPrim) (0x000B + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_DIALOG_SET_REQ                         ((CsrUiPrim) (0x000C + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_DIALOG_GET_REQ                         ((CsrUiPrim) (0x000D + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_INPUTDIALOG_SET_REQ                    ((CsrUiPrim) (0x000E + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_INPUTDIALOG_GET_REQ                    ((CsrUiPrim) (0x000F + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_IDLESCREEN_SET_REQ                     ((CsrUiPrim) (0x0010 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_IDLESCREEN_GET_REQ                     ((CsrUiPrim) (0x0011 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_UIE_SHOW_REQ                           ((CsrUiPrim) (0x0012 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_UIE_HIDE_REQ                           ((CsrUiPrim) (0x0013 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_DISPLAY_SETINPUTMODE_REQ               ((CsrUiPrim) (0x0014 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_DISPLAY_GETHANDLE_REQ                  ((CsrUiPrim) (0x0015 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_STATUS_BATTERY_SET_REQ                 ((CsrUiPrim) (0x0016 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_STATUS_RADIOMETER_SET_REQ              ((CsrUiPrim) (0x0017 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_STATUS_HEADSET_SET_REQ                 ((CsrUiPrim) (0x0018 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_STATUS_BLUETOOTH_SET_REQ               ((CsrUiPrim) (0x0019 + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_STATUS_WIFI_SET_REQ                    ((CsrUiPrim) (0x001A + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_UIE_DESTROY_REQ                        ((CsrUiPrim) (0x001B + CSR_UI_PRIM_DOWNSTREAM_LOWEST))
#define CSR_UI_PRIM_DOWNSTREAM_HIGHEST                             (0x001B + CSR_UI_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_UI_PRIM_UPSTREAM_LOWEST                                (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_UI_UIE_CREATE_CFM                         ((CsrUiPrim) (0x0000 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_MENU_GET_CFM                           ((CsrUiPrim) (0x0001 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_MENU_GETITEM_CFM                       ((CsrUiPrim) (0x0002 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_MENU_GETCURSOR_CFM                     ((CsrUiPrim) (0x0003 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_EVENT_GET_CFM                          ((CsrUiPrim) (0x0004 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_DIALOG_GET_CFM                         ((CsrUiPrim) (0x0005 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_INPUTDIALOG_GET_CFM                    ((CsrUiPrim) (0x0006 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_IDLESCREEN_GET_CFM                     ((CsrUiPrim) (0x0007 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_DISPLAY_GETHANDLE_CFM                  ((CsrUiPrim) (0x0008 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_KEYDOWN_IND                            ((CsrUiPrim) (0x0009 + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_EVENT_IND                              ((CsrUiPrim) (0x000A + CSR_UI_PRIM_UPSTREAM_LOWEST))
#define CSR_UI_PRIM_UPSTREAM_HIGHEST                               (0x000A + CSR_UI_PRIM_UPSTREAM_LOWEST)

#define CSR_UI_PRIM_DOWNSTREAM_COUNT     (CSR_UI_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_UI_PRIM_DOWNSTREAM_LOWEST)
#define CSR_UI_PRIM_UPSTREAM_COUNT       (CSR_UI_PRIM_UPSTREAM_HIGHEST + 1 - CSR_UI_PRIM_UPSTREAM_LOWEST)

/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrUiPrim   type;
    CsrSchedQid phandle;
    CsrUieType  elementType;
} CsrUiUieCreateReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle handle;
    CsrUieType   elementType;
} CsrUiUieCreateCfm;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    menu;
    CsrUtf16String *heading;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
} CsrUiMenuSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle menu;
} CsrUiMenuGetReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    handle;
    CsrUtf16String *heading;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
} CsrUiMenuGetCfm;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    menu;
    CsrUint16       position;
    CsrUint16       key;
    CsrUint16       icon;
    CsrUtf16String *label;
    CsrUtf16String *sublabel;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiMenuAdditemReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle menu;
    CsrUint16    key;
} CsrUiMenuRemoveitemReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle menu;
} CsrUiMenuRemoveallitemsReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    menu;
    CsrUint16       key;
    CsrUint16       icon;
    CsrUtf16String *label;
    CsrUtf16String *sublabel;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiMenuSetitemReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle menu;
    CsrUint16    key;
} CsrUiMenuGetitemReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    handle;
    CsrUint16       key;
    CsrUint16       icon;
    CsrUtf16String *label;
    CsrUtf16String *sublabel;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiMenuGetitemCfm;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle menu;
    CsrUint16    key;
} CsrUiMenuSetcursorReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle menu;
} CsrUiMenuGetcursorReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle handle;
    CsrUint16    key;
} CsrUiMenuGetcursorCfm;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle event;
    CsrUint16    inputMode;
    CsrSchedQid  listener;
} CsrUiEventSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle event;
} CsrUiEventGetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle handle;
    CsrUint16    inputMode;
    CsrSchedQid  listener;
} CsrUiEventGetCfm;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    dialog;
    CsrUtf16String *heading;
    CsrUtf16String *message;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiDialogSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle dialog;
} CsrUiDialogGetReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    handle;
    CsrUtf16String *heading;
    CsrUtf16String *message;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiDialogGetCfm;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    inputDialog;
    CsrUtf16String *heading;
    CsrUtf16String *message;
    CsrUint16       icon;
    CsrUtf16String *text;
    CsrUint16       textLength;
    CsrUint8        keyMap;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiInputdialogSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle inputDialog;
} CsrUiInputdialogGetReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    handle;
    CsrUtf16String *text;
} CsrUiInputdialogGetCfm;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    idleScreen;
    CsrUtf16String *heading;
    CsrUtf16String *text;
    CsrUint16       textLength;
    CsrUtf16String *textSK1;
    CsrUtf16String *textSK2;
    CsrUieHandle    actionSK1;
    CsrUieHandle    actionSK2;
    CsrUieHandle    actionBACK;
    CsrUieHandle    actionDEL;
} CsrUiIdlescreenSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrSchedQid  phandle;
    CsrUieHandle idleScreen;
} CsrUiIdlescreenGetReq;

typedef struct
{
    CsrUiPrim       type;
    CsrUieHandle    handle;
    CsrUtf16String *text;
} CsrUiIdlescreenGetCfm;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle element;
    CsrSchedQid  listener;
    CsrUint16    inputMode;
    CsrUint16    priority;
} CsrUiUieShowReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle element;
} CsrUiUieHideReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle element;
    CsrUint16    inputMode;
} CsrUiDisplaySetinputmodeReq;

typedef struct
{
    CsrUiPrim   type;
    CsrSchedQid phandle;
} CsrUiDisplayGethandleReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle handle;
} CsrUiDisplayGethandleCfm;

typedef struct
{
    CsrUiPrim type;
    CsrUint8  percentage;
} CsrUiStatusBatterySetReq;

typedef struct
{
    CsrUiPrim type;
    CsrUint8  percentage;
} CsrUiStatusRadiometerSetReq;

typedef struct
{
    CsrUiPrim type;
    CsrInt8   headset1;
    CsrInt8   headset2;
} CsrUiStatusHeadsetSetReq;

typedef struct
{
    CsrUiPrim type;
    CsrBool   active;
} CsrUiStatusBluetoothSetReq;

typedef struct
{
    CsrUiPrim type;
    CsrBool   active;
} CsrUiStatusWifiSetReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle element;
} CsrUiUieDestroyReq;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle displayElement;
    CsrUint16    key;
} CsrUiKeydownInd;

typedef struct
{
    CsrUiPrim    type;
    CsrUieHandle event;
    CsrUieHandle displayElement;
    CsrUint16    key;
    CsrUint16    inputMode;
} CsrUiEventInd;

#ifdef __cplusplus
}
#endif

#endif
