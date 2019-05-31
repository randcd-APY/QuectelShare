#ifndef CSR_BT_HFG_MENU_UTILS_H__
#define CSR_BT_HFG_MENU_UTILS_H__
/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "csr_synergy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_UNDEFINED                 0xFF

void toggleActivation(hfgInstance_t *inst);
void hfgActivate(hfgInstance_t *inst);
void startHfgConnecting(hfgInstance_t *inst, CsrBtHfgConnection type);
void startCancelConnect(hfgInstance_t *inst);
void startDisconnecting(hfgInstance_t *inst);
void toggleHfgConnections(hfgInstance_t *inst);
void startToggleRfShield(hfgInstance_t *inst);
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
void handleAtCmdChoiceMenu(hfgInstance_t *inst, CsrUint8 key);
#endif
void toggleAudio(hfgInstance_t *inst);
void startRinging(hfgInstance_t *inst, char *ringNumber);
void startAnswer(hfgInstance_t *inst);
void startReject(hfgInstance_t *inst);
void startPutOnHold(hfgInstance_t *inst);
void startAnswerCallOnHold(hfgInstance_t *inst);
void startRejectCallOnHold(hfgInstance_t *inst);
void displayFeatures(hfgInstance_t *inst);
void handleCallAlterMenu(hfgInstance_t *inst, CsrUint8 key);
void displayCallOptions(hfgInstance_t *inst);
void handleAdpcmMenu(hfgInstance_t* inst, CsrUint8 key);
void toggleRoam(hfgInstance_t *inst);
void toggleCellRegStatus(hfgInstance_t *inst);
void toggleSignal(hfgInstance_t *inst, CsrBool inc);
void toggleBattery(hfgInstance_t *inst, CsrBool inc);
void toggleMic(hfgInstance_t *inst, CsrBool inc);
void toggleSpeaker(hfgInstance_t *inst, CsrBool inc);
void toggleInband(hfgInstance_t *inst, CsrBool on);
void toggleVoiceRecog(hfgInstance_t *inst, CsrBool on);
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
void handleScoInput(hfgInstance_t* inst, CsrUint8 key);
#endif
CsrUint32 extractNumberFromString(CsrUint8 *string, CsrUint32 len);
void startOutgoingCall(hfgInstance_t *inst, char *ringNumber);

#ifdef __cplusplus
extern "C" }
#endif


#endif

