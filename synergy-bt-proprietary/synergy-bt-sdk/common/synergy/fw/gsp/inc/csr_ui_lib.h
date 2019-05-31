#ifndef CSR_UI_LIB_H__
#define CSR_UI_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_ui_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiUieCreateReqSend
 *
 *  DESCRIPTION
 *      Use to create a new UIE
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiUieCreateReq *CsrUiUieCreateReq_struct(CsrSchedQid phandle, CsrUieType elementType);
#define CsrUiUieCreateReqSend(_appH, _elementT) { \
        CsrUiUieCreateReq *msg__; \
        msg__ = CsrUiUieCreateReq_struct(_appH, _elementT); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuSetReq *CsrUiMenuSetReq_struct(CsrUieHandle menu, CsrUtf16String *heading, CsrUtf16String *textSK1, CsrUtf16String *textSK2);
#define CsrUiMenuSetReqSend(_menu, _heading, _textSK1, _textSK2) { \
        CsrUiMenuSetReq *msg__; \
        msg__ = CsrUiMenuSetReq_struct(_menu, _heading, _textSK1, _textSK2); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuGetReq *CsrUiMenuGetReq_struct(CsrSchedQid phandle, CsrUieHandle menu);
#define CsrUiMenuGetReqSend(_appH, _menu) { \
        CsrUiMenuGetReq *msg__; \
        msg__ = CsrUiMenuGetReq_struct(_appH, _menu); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuAdditemReqSend
 *
 *  DESCRIPTION
 *      Add a single item to a menu
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuAdditemReq *CsrUiMenuAdditemReq_struct(CsrUieHandle menu, CsrUint16 position, CsrUint16 key, CsrUint16 icon, CsrUtf16String *label, CsrUtf16String *sublabel, CsrUieHandle actionSK1, CsrUieHandle actionSK2, CsrUieHandle actionBACK, CsrUieHandle actionDEL);
#define CsrUiMenuAdditemReqSend(_menu, _position, _key, _icon, _label, _sublabel, _actionSK1, _actionSK2, _actionB, _actionD) { \
        CsrUiMenuAdditemReq *msg__; \
        msg__ = CsrUiMenuAdditemReq_struct(_menu, _position, _key, _icon, _label, _sublabel, _actionSK1, _actionSK2, _actionB, _actionD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuRemoveitemReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuRemoveitemReq *CsrUiMenuRemoveitemReq_struct(CsrUieHandle menu, CsrUint16 key);
#define CsrUiMenuRemoveitemReqSend(_menu, _key) { \
        CsrUiMenuRemoveitemReq *msg__; \
        msg__ = CsrUiMenuRemoveitemReq_struct(_menu, _key); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuRemoveallitemsReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuRemoveallitemsReq *CsrUiMenuRemoveallitemsReq_struct(CsrUieHandle menu);
#define CsrUiMenuRemoveallitemsReqSend(_menu) { \
        CsrUiMenuRemoveallitemsReq *msg__; \
        msg__ = CsrUiMenuRemoveallitemsReq_struct(_menu); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuSetitemReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuSetitemReq *CsrUiMenuSetitemReq_struct(CsrUieHandle menu, CsrUint16 key, CsrUint16 icon, CsrUtf16String *label, CsrUtf16String *sublabel, CsrUieHandle actionSK1, CsrUieHandle actionSK2, CsrUieHandle actionBACK, CsrUieHandle actionDEL);
#define CsrUiMenuSetitemReqSend(_menu, _key, _icon, _label, _sublabel, _actionSK1, _actionSK2, _actionB, _actionD) { \
        CsrUiMenuSetitemReq *msg__; \
        msg__ = CsrUiMenuSetitemReq_struct(_menu, _key, _icon, _label, _sublabel, _actionSK1, _actionSK2, _actionB, _actionD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuGetitemReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuGetitemReq *CsrUiMenuGetitemReq_struct(CsrSchedQid phandle, CsrUieHandle menu, CsrUint16 key);
#define CsrUiMenuGetitemReqSend(_appH, _menu, _key) { \
        CsrUiMenuGetitemReq *msg__; \
        msg__ = CsrUiMenuGetitemReq_struct(_appH, _menu, _key); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuSetcursorReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuSetcursorReq *CsrUiMenuSetcursorReq_struct(CsrUieHandle menu, CsrUint16 key);
#define CsrUiMenuSetcursorReqSend(_menu, _key) { \
        CsrUiMenuSetcursorReq *msg__; \
        msg__ = CsrUiMenuSetcursorReq_struct(_menu, _key); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiMenuGetcursorReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiMenuGetcursorReq *CsrUiMenuGetcursorReq_struct(CsrSchedQid phandle, CsrUieHandle menu);
#define CsrUiMenuGetcursorReqSend(_phandle, _menu) { \
        CsrUiMenuGetcursorReq *msg__; \
        msg__ = CsrUiMenuGetcursorReq_struct(_phandle, _menu); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiEventSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiEventSetReq *CsrUiEventSetReq_struct(CsrUieHandle event, CsrUint16 inputMode, CsrSchedQid listener);
#define CsrUiEventSetReqSend(_event, _inputM, _listener) { \
        CsrUiEventSetReq *msg__; \
        msg__ = CsrUiEventSetReq_struct(_event, _inputM, _listener); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiEventGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiEventGetReq *CsrUiEventGetReq_struct(CsrSchedQid phandle, CsrUieHandle event);
#define CsrUiEventGetReqSend(_appH, _event) { \
        CsrUiEventGetReq *msg__; \
        msg__ = CsrUiEventGetReq_struct(_appH, _event); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiDialogSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiDialogSetReq *CsrUiDialogSetReq_struct(CsrUieHandle dialog, CsrUtf16String *heading, CsrUtf16String *message, CsrUtf16String *textSK1, CsrUtf16String *textSK2, CsrUieHandle actionSK1, CsrUieHandle actionSK2, CsrUieHandle actionBACK, CsrUieHandle actionDEL);
#define CsrUiDialogSetReqSend(_dialog, _heading, _message, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD) { \
        CsrUiDialogSetReq *msg__; \
        msg__ = CsrUiDialogSetReq_struct(_dialog, _heading, _message, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiDialogGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiDialogGetReq *CsrUiDialogGetReq_struct(CsrSchedQid phandle, CsrUieHandle dialog);
#define CsrUiDialogGetReqSend(_appH, _dialog) { \
        CsrUiDialogGetReq *msg__; \
        msg__ = CsrUiDialogGetReq_struct(_appH, _dialog); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiInputdialogSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiInputdialogSetReq *CsrUiInputdialogSetReq_struct(CsrUieHandle inputDialog, CsrUtf16String *heading, CsrUtf16String *message, CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength, CsrUint8 keyMap, CsrUtf16String *textSK1, CsrUtf16String *textSK2, CsrUieHandle actionSK1, CsrUieHandle actionSK2, CsrUieHandle actionBACK, CsrUieHandle actionDEL);
#define CsrUiInputdialogSetReqSend(_inputD, _heading, _message, _icon, _text, _textL, _keyM, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD) { \
        CsrUiInputdialogSetReq *msg__; \
        msg__ = CsrUiInputdialogSetReq_struct(_inputD, _heading, _message, _icon, _text, _textL, _keyM, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiInputdialogGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiInputdialogGetReq *CsrUiInputdialogGetReq_struct(CsrSchedQid phandle, CsrUieHandle inputDialog);
#define CsrUiInputdialogGetReqSend(_appH, _inputD) { \
        CsrUiInputdialogGetReq *msg__; \
        msg__ = CsrUiInputdialogGetReq_struct(_appH, _inputD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiIdlescreenSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiIdlescreenSetReq *CsrUiIdlescreenSetReq_struct(CsrUieHandle inputDialog, CsrUtf16String *heading, CsrUint16 *text, CsrUint16 textLength, CsrUtf16String *textSK1, CsrUtf16String *textSK2, CsrUieHandle actionSK1, CsrUieHandle actionSK2, CsrUieHandle actionBACK, CsrUieHandle actionDEL);
#define CsrUiIdlescreenSetReqSend(_idleS, _heading, _text, _textL, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD) { \
        CsrUiIdlescreenSetReq *msg__; \
        msg__ = CsrUiIdlescreenSetReq_struct(_idleS, _heading, _text, _textL, _textSK1, _textSK2, _actionSK1, _actionSK2, _actionB, _actionD); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiIdlescreenGetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiIdlescreenGetReq *CsrUiIdlescreenGetReq_struct(CsrSchedQid phandle, CsrUieHandle idleScreen);
#define CsrUiIdlescreenGetReqSend(_appH, _idleS) { \
        CsrUiIdlescreenGetReq *msg__; \
        msg__ = CsrUiIdlescreenGetReq_struct(_appH, _idleS); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiUieShowReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiUieShowReq *CsrUiUieShowReq_struct(CsrUieHandle element, CsrSchedQid listener, CsrUint16 inputMode, CsrUint16 priority);
#define CsrUiUieShowReqSend(_element, _listener, _inputM, _priority) { \
        CsrUiUieShowReq *msg__; \
        msg__ = CsrUiUieShowReq_struct(_element, _listener, _inputM, _priority); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiUieHideReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiUieHideReq *CsrUiUieHideReq_struct(CsrUieHandle element);
#define CsrUiUieHideReqSend(_element) { \
        CsrUiUieHideReq *msg__; \
        msg__ = CsrUiUieHideReq_struct(_element); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiDisplaySetinputmodeReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiDisplaySetinputmodeReq *CsrUiDisplaySetinputmodeReq_struct(CsrUieHandle element, CsrUint16 inputMode);
#define CsrUiDisplaySetinputmodeReqSend(_element, _inputM) { \
        CsrUiDisplaySetinputmodeReq *msg__; \
        msg__ = CsrUiDisplaySetinputmodeReq_struct(_element, _inputM); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiDisplayGethandleReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiDisplayGethandleReq *CsrUiDisplayGethandleReq_struct(CsrSchedQid phandle);
#define CsrUiDisplayGethandleReqSend(_appH) { \
        CsrUiDisplayGethandleReq *msg__; \
        msg__ = CsrUiDisplayGethandleReq_struct(_appH); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiStatusBatterySetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiStatusBatterySetReq *CsrUiStatusBatterySetReq_struct(CsrUint8 percentage);
#define CsrUiStatusBatterySetReqSend(_percentage) { \
        CsrUiStatusBatterySetReq *msg__; \
        msg__ = CsrUiStatusBatterySetReq_struct(_percentage); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiStatusRadiometerSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiStatusRadiometerSetReq *CsrUiStatusRadiometerSetReq_struct(CsrUint8 percentage);
#define CsrUiStatusRadiometerSetReqSend(_percentage) { \
        CsrUiStatusRadiometerSetReq *msg__; \
        msg__ = CsrUiStatusRadiometerSetReq_struct(_percentage); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiStatusHeadsetSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiStatusHeadsetSetReq *CsrUiStatusHeadsetSetReq_struct(CsrInt8 headset1, CsrInt8 headset2);
#define CsrUiStatusHeadsetSetReqSend(_headset1, _headset2) { \
        CsrUiStatusHeadsetSetReq *msg__; \
        msg__ = CsrUiStatusHeadsetSetReq_struct(_headset1, _headset2); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiStatusBluetoothSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiStatusBluetoothSetReq *CsrUiStatusBluetoothSetReq_struct(CsrBool active);
#define CsrUiStatusBluetoothSetReqSend(_active) { \
        CsrUiStatusBluetoothSetReq *msg__; \
        msg__ = CsrUiStatusBluetoothSetReq_struct(_active); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiStatusWifiSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiStatusWifiSetReq *CsrUiStatusWifiSetReq_struct(CsrBool active);
#define CsrUiStatusWifiSetReqSend(_active) { \
        CsrUiStatusWifiSetReq *msg__; \
        msg__ = CsrUiStatusWifiSetReq_struct(_active); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiUieDestroyReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrUiUieDestroyReq *CsrUiUieDestroyReq_struct(CsrUieHandle element);
#define CsrUiUieDestroyReqSend(_element) { \
        CsrUiUieDestroyReq *msg__; \
        msg__ = CsrUiUieDestroyReq_struct(_element); \
        CsrSchedMessagePut(CSR_UI_IFACEQUEUE, CSR_UI_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrUiFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR UI upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_UI_PRIM,
 *      msg:          The message received from CSR UI
 *----------------------------------------------------------------------------*/
void CsrUiFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
