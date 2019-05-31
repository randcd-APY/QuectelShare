#ifndef CSR_BT_SC_DEMO_APP_H__
#define CSR_BT_SC_DEMO_APP_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_usr_config.h"
#include "csr_bt_util.h"
#include "csr_bt_sc_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Max number of profiles that can register incoming/outgoing security */
#define MAX_NUMBER_OF_SIMULTANEOUS_RFCOMM_CONNECTIONS               10
#define CSR_BT_SC_DEMO_MAX_NUM_OF_SIMULTANEOUS_L2CAP_CONNECTIONS    10
#define SC_APP_MAX_CONNECTIONS     (MAX_NUMBER_OF_SIMULTANEOUS_RFCOMM_CONNECTIONS+CSR_BT_SC_DEMO_MAX_NUM_OF_SIMULTANEOUS_L2CAP_CONNECTIONS)
#define SC_APP_INV_CONNECTION      ((CsrUint8) 0xFF)

#define SC_APP_NA                           (0x00)
#define SC_APP_MAIN_MENU                    (0x01)
#define SC_APP_SEC_MODE_MENU                (0x02)
#define SC_APP_SEC_LEVEL_MENU               (0x03)
#define SC_APP_SEC_IO_MENU                  (0x04)
#define SC_APP_SEC_AUTH_REQUIRE_MENU        (0x05)
#define SC_APP_SEC_EVENT_MASK_MENU          (0x06)
#define SC_APP_SEC_OOB_MENU                 (0x07)
#define SC_APP_SEC_AUTHORISE_MENU           (0x08)
#define SC_APP_SEC_INIT_PAIRING             (0x09)
#define SC_APP_SEC_PAIRING                  (0x0a)
#define SC_APP_SEC_ACP_PAIRING              (0x0b)

#define SC_APP_SEC_OOB_MAIN_MENU            (0x00)
#define SC_APP_SEC_OOB_SET_REM_HASHC_MENU   (0x01)
#define SC_APP_SEC_OOB_SET_REM_RANDR_MENU   (0x02)

typedef struct
{
    CsrPrim             type;
    CsrSchedQid              appHandle;
    CsrUint16           secLevel;
} ScAppDemoSecurityReq;

typedef struct
{
    CsrPrim             type;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} ScAppDemoSecurityCfm;

typedef void (*scAppInOutCallback)(void *pData);
typedef void* (*scAppGetMessage)(void *pData);

/* Set security related levels */
typedef struct
{
    char                *profileName;
    CsrSchedQid               ifaceQueue;
    CsrPrim              secPrimType;
    CsrPrim              msgSecOutType;
    CsrPrim              msgSecInType;

    /* Used as index into req parameter */
    CsrBool               securityIncoming;
    /* Incoming and outgoing security */
    ScAppDemoSecurityReq req[2];
} ScAppSecLvlInstData_t;

typedef struct
{
    CsrUint8             securityMode;
    CsrUint8             newSecurityMode;
    CsrUint8             ioCapability;
    CsrBool              enableSspDebugMode;
    CsrUint8             state;
    CsrUint8             subState;

    CsrBool              autoAcceptPairingAttempts;

    CsrUint8             minPasskeyLength;
    CsrUint8             inputLength;
    CsrUint8             input[SIZE_OOB_DATA*2];
    CsrUint8             authRequirements;
    CsrUint8             *oobHashCLocal;
    CsrUint8             *oobRandRLocal;
    CsrUint8             oobHashCRemote[SIZE_OOB_DATA];
    CsrUint8             oobRandRRemote[SIZE_OOB_DATA];
    CsrUint32            eventMask;
    CsrUint32            remoteAuthRequirements;
    CsrUint32            numericValue;
    CsrUint32            cod;
    CsrUint8             paired;
    CsrBool              accept;
    uuid16_t             serviceId;
    CsrPrim              msgType;
    CsrBool              proximityPairing;

    /* Parameters (set by demo app) */
    void                *recvMsgP;
    CsrBtAddressType     addressType;
    CsrBtDeviceAddr      deviceAddr;
    CsrUint8             deviceAddrStr[CSR_BT_DEVICE_ADDR_LEN];
    CsrBtDeviceName      deviceName;
    CsrBool              bondable;
    CsrBool              encrypted;

    /* Callback functions */
    scAppInOutCallback   fnInit;
    void                *pInitData;
    scAppInOutCallback   fnFini;
    void                *pFiniData;
    scAppInOutCallback   fnSend;
    void                *pSendData;
    scAppGetMessage      fnMsg;
    void                *pMsgData;

    CsrUint8             secLevelIdx;
    CsrUint8             secLevelCnt;
    ScAppSecLvlInstData_t secLevelData[SC_APP_MAX_CONNECTIONS];
} ScAppInstData_t;

typedef void (* scStateHandlerType)(ScAppInstData_t *scInstanceData);

extern const scStateHandlerType rejectPairingFunc[CSR_BT_SC_PRIM_UPSTREAM_COUNT];
extern const scStateHandlerType acceptPairingFunc[CSR_BT_SC_PRIM_UPSTREAM_COUNT];

/*----------------------------------------------------------------------------*
*  NAME
*      handleScKeybPrim
*
*  DESCRIPTION
*      Handler for key prims. The app must only pass on these prims if:
*
*      1) The SC main menu has been entered or
*      2) The SC app demo has requested control over the keyboard using
*         the callback given in scAppSetFnInit()
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void handleScKeybPrim(ScAppInstData_t *instData);

/*----------------------------------------------------------------------------*
*  NAME
*      handleScPrim
*
*  DESCRIPTION
*      Handler for all SC primitives
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void handleScPrim(ScAppInstData_t *instData);

/*----------------------------------------------------------------------------*
*  NAME
*      handleScSecLevelPrim
*
*  DESCRIPTION
*      App can pass on XXX_SECURITY_IN_CFM/XXX_SECURITY_OUT_CFM to
*      the SC app demo if the security primitives has been registered
*      by calling scAppSetSecLevelPrim().
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void handleScSecLevelPrim(ScAppInstData_t *instData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppInitInstanceData
*
*  DESCRIPTION
*      Initialise SC app internal instance data.
*
*      Note: Must be called before passing on key/SC prims.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppInitInstanceData(ScAppInstData_t *instData);
void scAppDeinitInstanceData(ScAppInstData_t *instData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetBondingAddr
*
*  DESCRIPTION
*      Set the deviceAddr of the remote device with which we wish
*      to bond.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetBondingAddr(ScAppInstData_t *instData,
                         CsrBtDeviceAddr    deviceAddr);

void scAppSetBondingTypedAddr(ScAppInstData_t *instData,
                              CsrBtAddressType addressType,
                              CsrBtDeviceAddr deviceAddr);
/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetFnInit
*
*  DESCRIPTION
*      The SC App demo is requesting control over the input (keyboard).
*      This is done in order to enter passkey/pin/etc.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetFnInit(ScAppInstData_t *instData,
                         scAppInOutCallback     fnInit,
                         void                   *pInitData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetFnFini
*
*  DESCRIPTION
*      The SC App demo is releasing control over the input (keyboard).
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetFnFini(ScAppInstData_t *instData,
                         scAppInOutCallback     fnFini,
                         void                   *pFiniData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetSecLevelSend
*
*  DESCRIPTION
*
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetSecLevelSend(ScAppInstData_t *instData,
                                    scAppInOutCallback     fnSend,
                                    void                   *pSendData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetFnGetMessage
*
*  DESCRIPTION
*      The SC App demo is requesting the app to take ownership of
*      the current active message. The app must _never_ access the
*      message after the ownership has been transferred.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetFnGetMessage(ScAppInstData_t *instData,
                                     scAppGetMessage        fnMsg,
                                     void                   *pMsgData);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetSecLevelCM
*
*  DESCRIPTION
*      Apps using the CM directly must use this function to register
*      a profile. The security level for the given profile can then
*      be changed/set by entering the SC configuration menu.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetSecLevelCM(ScAppInstData_t *instData,
                                  CsrUint8                secLevelIdx,
                                  char                   *profileName);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetGetLevelCM
*
*  DESCRIPTION
*      The security level associated by the given secLevelIdx will be
*      returned.
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
CsrUint16 scAppGetSecLevelCM(ScAppInstData_t *instData,
                                      CsrUint8                secLevelIdx,
                                      CsrBool                 securityIncoming);

#define SC_APP_SET_SEC_LEVEL_NA     ((CsrUint16) 0xFF)

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetSecLevelPrim
*
*  DESCRIPTION
*
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSetSecLevelPrim(ScAppInstData_t *instData,
                                    CsrUint8                secLevelIdx,
                                    char                   *profileName,
                                    CsrPrim           primType,
                                    CsrUint16               ifaceQueue,
                                    CsrPrim           msgSecInType,
                                    CsrPrim           msgSecOutType);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSendProfileSecurityLevelReq
*
*  DESCRIPTION
*
*
*  RETURNS/MODIFIES
*
*----------------------------------------------------------------------------*/
void scAppSendProfileSecurityLevelReq(void *arg);

/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetMinimumPasskeylength
*
*  DESCRIPTION
*     Used to apply a minimum length requirement to the passkey length
*     The default is one.
*
*  RETURNS/MODIFIES
*    the minPasskeyLength of the instData
*----------------------------------------------------------------------------*/
void scAppSetMinimumPasskeylength(ScAppInstData_t *instData,
                                  CsrUint8          minPasskeyLength);


/*----------------------------------------------------------------------------*
*  NAME
*      scAppSetProximityPairing
*
*  DESCRIPTION
*     Used to toggle proximity pairing on/off. When Proximity Pairing is on,
*     all incoming pairing requests will be auto accepted without further
*     user interaction.
*
*  RETURNS/MODIFIES
*    the proximityPairing boolean of the instData
*----------------------------------------------------------------------------*/
void scAppSetProximityPairing(ScAppInstData_t   *instData,
                              CsrBool           proximityPairingOn);


void scAppSaveState(CsrUint8 *prevState, CsrUint8 *curState, CsrUint8 newState);
void scAppRestoreState(CsrUint8 *prevState, CsrUint8 *curState);

#ifdef __cplusplus
}
#endif

#endif
