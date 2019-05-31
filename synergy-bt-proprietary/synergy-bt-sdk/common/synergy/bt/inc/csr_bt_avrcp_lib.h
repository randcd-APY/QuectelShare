#ifndef CSR_BT_AVRCP_LIB_H__
#define CSR_BT_AVRCP_LIB_H__

/******************************************************************************

Copyright (c) 2004-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtAvrcpMsgTransport(void* msg);

/** \file csr_bt_avrcp_lib.h */
void CsrBtCopyBackwards(CsrBtAvrcpUid dst, CsrBtAvrcpUid src, CsrUint16 len);

#define CSR_BT_AVRCP_UID_COPY(dest, src)           (CsrBtCopyBackwards((dest), (src), sizeof(CsrBtAvrcpUid)))
#define CSR_BT_AVRCP_FEATURE_MASK_RESET(mask)      (CsrMemSet(&(mask), 0, sizeof(CsrBtAvrcpMpFeatureMask))
#define CSR_BT_AVRCP_FEATURE_MASK_COPY(dest, src)  (CsrMemCpy((dest), (src), sizeof(CsrBtAvrcpMpFeatureMask)))

void CsrBtAvrcpConfigRoleNoSupport(CsrBtAvrcpRoleDetails *details);

void CsrBtAvrcpConfigRoleSupport(CsrBtAvrcpRoleDetails          *details,
                                 CsrBtAvrcpConfigRoleMask       roleConfig,
                                 CsrBtAvrcpConfigSrVersion      srAvrcpVersion,
                                 CsrBtAvrcpConfigSrFeatureMask  srFeatures,
                                 CsrCharString                    *providerName,
                                 CsrCharString                    *serviceName);

#define CsrBtAvrcpConfigReqSend(_phandle, _globalConfig, _mtu, _tgConfig, _ctConfig){ \
        CsrBtAvrcpConfigReq *msg = pnew(CsrBtAvrcpConfigReq);           \
        msg->type          = CSR_BT_AVRCP_CONFIG_REQ;                   \
        msg->phandle       = _phandle;                                  \
        msg->globalConfig  = _globalConfig;                             \
        msg->tgDetails     = _tgConfig;                                 \
        msg->ctDetails     = _ctConfig;                                 \
        msg->mtu           = _mtu;                                      \
        msg->uidCount      = 0;                                         \
        CsrBtAvrcpMsgTransport(msg);}

#define CsrBtAvrcpConfigReqExtSend(_phandle, _globalConfig, _mtu, _tgConfig, _ctConfig, _uidCount){ \
        CsrBtAvrcpConfigReq *msg = pnew(CsrBtAvrcpConfigReq);           \
        msg->type          = CSR_BT_AVRCP_CONFIG_REQ;                   \
        msg->phandle       = _phandle;                                  \
        msg->globalConfig  = _globalConfig;                             \
        msg->tgDetails     = _tgConfig;                                 \
        msg->ctDetails     = _ctConfig;                                 \
        msg->mtu           = _mtu;                                      \
        msg->uidCount      = _uidCount;                                 \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from a
 *        remote device.
 *
 *    PARAMETERS
 *        phandle:            application handle
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpActivateReqSend(_maxIncoming){                        \
        CsrBtAvrcpActivateReq *msg = pnew(CsrBtAvrcpActivateReq);       \
        msg->type              = CSR_BT_AVRCP_ACTIVATE_REQ;             \
        msg->maxIncoming       = _maxIncoming;                          \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpDeactivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to deactivate a service and make in inaccessible from
 *        other devices.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpDeactivateReqSend(){                                  \
        CsrBtAvrcpDeactivateReq *msg = pnew(CsrBtAvrcpDeactivateReq);   \
        msg->type              = CSR_BT_AVRCP_DEACTIVATE_REQ;           \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpConnectReqSend
 *
 *  DESCRIPTION
 *        An CSR_BT_AVRCP_CONNECT_REQ will initiate a connection towards a device specified
 *        by the Bluetooth device address. The AVRCP will send an CSR_BT_AVRCP_CONNECT_IND back
 *        to the initiator with the result of the connection attempt.
 *
 *    PARAMETERS
 *        phandle:            application handle
 *        deviceAddr:         address of device to connect to
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpConnectReqSend(_deviceAddr){                  \
        CsrBtAvrcpConnectReq *msg = pnew(CsrBtAvrcpConnectReq); \
        msg->type              = CSR_BT_AVRCP_CONNECT_REQ;      \
        msg->deviceAddr        = _deviceAddr;                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCancelConnectReqSend
 *
 *  DESCRIPTION
 *        An CSR_BT_AVRCP_CANCEL_CONNECT_REQ will attempt to cancel an outgoing connection
 *        establishment
 *
 *    PARAMETERS
 *        deviceAddr:         address of device to cancel the connection to
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCancelConnectReqSend(_deviceAddr){                    \
        CsrBtAvrcpCancelConnectReq *msg = pnew(CsrBtAvrcpCancelConnectReq); \
        msg->type              = CSR_BT_AVRCP_CANCEL_CONNECT_REQ;       \
        msg->deviceAddr        = _deviceAddr;                           \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpDisconnectReqSend
 *
 *  DESCRIPTION
 *      Request for disconnect of connection previously established.
 *
 *    PARAMETERS
 *        connectionId        Unique connection ID
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpDisconnectReqSend(_connectionId){                     \
        CsrBtAvrcpDisconnectReq *msg = pnew(CsrBtAvrcpDisconnectReq);   \
        msg->type              = CSR_BT_AVRCP_DISCONNECT_REQ;           \
        msg->connectionId      = _connectionId;                         \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpSecurityInReq
 *      AvrcpSecurityOutReq
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *    PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpSecurityInReqSend(_appHandle, _secLevel, _config) {   \
        CsrBtAvrcpSecurityInReq *msg = pnew(CsrBtAvrcpSecurityInReq);   \
        msg->type = CSR_BT_AVRCP_SECURITY_IN_REQ;                       \
        msg->phandle = _appHandle;                                      \
        msg->secLevel = _secLevel;                                      \
        msg->config = _config;                                          \
        CsrBtAvrcpMsgTransport(msg);}

#define CsrBtAvrcpSecurityOutReqSend(_appHandle, _secLevel, _config) {  \
        CsrBtAvrcpSecurityOutReq *msg = (CsrBtAvrcpSecurityOutReq*)pnew(CsrBtAvrcpSecurityOutReq); \
        msg->type = CSR_BT_AVRCP_SECURITY_OUT_REQ;                      \
        msg->phandle = _appHandle;                                      \
        msg->secLevel = _secLevel;                                      \
        msg->config = _config;                                          \
        CsrBtAvrcpMsgTransport(msg);}

#define CSR_BT_AVRCP_LIB_GFI_HEADER_OFFSET             (11)
#define CSR_BT_AVRCP_LIB_GFI_HEADER_SIZE               (8)
#define CSR_BT_AVRCP_LIB_GFI_ITEM_TYPE_INDEX           (0)
#define CSR_BT_AVRCP_LIB_GFI_ITEM_TYPE_SIZE            (1)
#define CSR_BT_AVRCP_LIB_GFI_ITEM_LENGTH_INDEX         (1)
#define CSR_BT_AVRCP_LIB_GFI_ITEM_LENGTH_SIZE          (2)
#define CSR_BT_AVRCP_LIB_GFI_ITEM_HEADER_SIZE          (CSR_BT_AVRCP_LIB_GFI_ITEM_TYPE_SIZE + CSR_BT_AVRCP_LIB_GFI_ITEM_LENGTH_SIZE)

/* The below defines are for Media Player Item as define by spec.
 * It can be used by GFI related library functions
 */
#define CSR_BT_AVRCP_LIB_GFI_MP_PLAYER_ID_INDEX        (3)
#define CSR_BT_AVRCP_LIB_GFI_MP_MAJOR_TYPE_INDEX       (5)
#define CSR_BT_AVRCP_LIB_GFI_MP_SUB_TYPE_INDEX         (6)
#define CSR_BT_AVRCP_LIB_GFI_MP_PLAY_STATUS_INDEX      (10)
#define CSR_BT_AVRCP_LIB_GFI_MP_FEATURE_MASK_INDEX     (11)
#define CSR_BT_AVRCP_LIB_GFI_MP_CHARSET_INDEX          (27)
#define CSR_BT_AVRCP_LIB_GFI_MP_NAME_LEN_INDEX         (29)
#define CSR_BT_AVRCP_LIB_GFI_MP_NAME_INDEX             (31)
#define CSR_BT_AVRCP_LIB_GFI_MP_PART_SIZE              (31) /* + MP name */

/* The below defines are for Folder Item as define by spec.
 * It can be used by GFI related library functions
 */
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_UID_INDEX          (3)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_TYPE_INDEX         (11)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_PLAYABLE_INDEX     (12)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_CHARSET_INDEX      (13)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_NAME_LEN_INDEX     (15)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_NAME_INDEX         (17)
#define CSR_BT_AVRCP_LIB_GFI_FOLDER_PART_SIZE          (17) /* + folder name*/

/* The below defines are for Media Element Item as define by spec.
 * It can be used by GFI related library functions
 */
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_UID_INDEX           (3)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_TYPE_INDEX          (11)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_CHARSET_INDEX       (12)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_NAME_LEN_INDEX      (14)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_NAME_INDEX          (16)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_COUNT_SIZE      (1)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_PART_SIZE           (16) /* media name and attributes */

/* The below defines are for Attribute Value Entry as define by spec.
 * It can be used by GFI and GIA related
 */
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE       (8)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_ID_INDEX        (0)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_CHARSET_INDEX   (4)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_VAL_LEN_INDEX   (6)
#define CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_VAL_INDEX       (8)

/** Get Items Attributes Header related */
#define CSR_BT_AVRCP_LIB_GIA_HEADER_OFFSET             (8)


#ifndef EXCLUDE_CSR_BT_AVRCP_CT_MODULE
/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPassThroughReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPassThroughReqSend(_phandle, _connId, _opId, _state) { \
        CsrBtAvrcpCtPassThroughReq *msg = pnew(CsrBtAvrcpCtPassThroughReq); \
        msg->type          = CSR_BT_AVRCP_CT_PASS_THROUGH_REQ;          \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->operationId   = _opId;                                     \
        msg->state         = _state;                                    \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtGetPlayStatusReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_AVRCP_CT_13_AND_HIGHER
#define CsrBtAvrcpCtGetPlayStatusReqSend(_phandle, _connId) {           \
        CsrBtAvrcpCtGetPlayStatusReq *msg = pnew(CsrBtAvrcpCtGetPlayStatusReq); \
        msg->type          = CSR_BT_AVRCP_CT_GET_PLAY_STATUS_REQ;       \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPassThroughReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtNotiRegisterReqSend(_phandle, _connId, _notiMask, _playbackInterval, _configMask) { \
        CsrBtAvrcpCtNotiRegisterReq *msg = pnew(CsrBtAvrcpCtNotiRegisterReq); \
        msg->type          = CSR_BT_AVRCP_CT_NOTI_REGISTER_REQ;         \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->notiMask      = _notiMask;                                 \
        msg->config        = _configMask;                               \
        msg->playbackInterval = _playbackInterval;                      \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtInformDispCharSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtInformDispCharSetReqSend(_phandle, _connId, _charsetCount, _charset) { \
        CsrBtAvrcpCtInformDispCharsetReq *msg = pnew(CsrBtAvrcpCtInformDispCharsetReq); \
        msg->type          = CSR_BT_AVRCP_CT_INFORM_DISP_CHARSET_REQ;   \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->charsetCount  = _charsetCount;                             \
        msg->charset       = _charset;                                  \
        CsrBtAvrcpMsgTransport(msg);}


/* CT PAS helper functions (for parsing attribute and value text) */
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_OFFSET          (13)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_MIN_HEADER_SIZE (5)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_MIN_PART_SIZE   (4)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_NUM_INDEX       (0)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_ID_INDEX        (1)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_CS_INDEX        (2)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_STR_LEN_INDEX   (4)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_STR_INDEX       (5)

#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_OFFSET          (13)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_MIN_HEADER_SIZE (5)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_MIN_PART_SIZE   (4)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_NUM_INDEX       (0)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_ID_INDEX        (1)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_CS_INDEX        (2)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_STR_LEN_INDEX   (4)
#define CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_STR_INDEX       (5)

CsrBool CsrBtAvrcpCtLibPasAttribTxtGet(CsrUint16 pasLen, CsrUint8 *pas, CsrUint16 *index, CsrBtAvrcpPasAttId *attId, CsrBtAvrcpCharSet *charset, CsrUint8 *attTxtLen, CsrUint8 **attTxt);
CsrBool CsrBtAvrcpCtLibPasValueTxtGet(CsrUint16 pasLen, CsrUint8 *pas, CsrUint16 *index, CsrBtAvrcpPasValId *valId, CsrBtAvrcpCharSet *charset, CsrUint8 *valTxtLen, CsrUint8 **valTxt);


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasAttIdReqSend(_phandle, _connId) {                \
        CsrBtAvrcpCtPasAttIdReq *msg = pnew(CsrBtAvrcpCtPasAttIdReq);   \
        msg->type          = CSR_BT_AVRCP_CT_PAS_ATT_ID_REQ;            \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasAttTxtReqSend(_phandle, _connId, _attribIdCount, _attribId) { \
        CsrBtAvrcpCtPasAttTxtReq *msg = pnew(CsrBtAvrcpCtPasAttTxtReq); \
        msg->type          = CSR_BT_AVRCP_CT_PAS_ATT_TXT_REQ;           \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->attIdCount = _attribIdCount;                               \
        msg->attId      = _attribId;                                    \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasAttTxtResSend(_connId, _proceed) {               \
        CsrBtAvrcpCtPasAttTxtRes *msg = pnew(CsrBtAvrcpCtPasAttTxtRes); \
        msg->type          = CSR_BT_AVRCP_CT_PAS_ATT_TXT_RES;           \
        msg->connectionId  = _connId;                                   \
        msg->proceed       = _proceed;                                  \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasValIdReqSend(_phandle, _connId, _attribId) {     \
        CsrBtAvrcpCtPasValIdReq *msg = pnew(CsrBtAvrcpCtPasValIdReq);   \
        msg->type          = CSR_BT_AVRCP_CT_PAS_VAL_ID_REQ;            \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->attId      = _attribId;                                    \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasValTxtReqSend(_phandle, _connId, _attribId, _valIdCount, _valId) { \
        CsrBtAvrcpCtPasValTxtReq *msg = pnew(CsrBtAvrcpCtPasValTxtReq); \
        msg->type          = CSR_BT_AVRCP_CT_PAS_VAL_TXT_REQ;           \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->attId      = _attribId;                                    \
        msg->valIdCount    = _valIdCount;                               \
        msg->valId         = _valId;                                    \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasValTxtResSend(_connId, _proceed) {               \
        CsrBtAvrcpCtPasValTxtRes *msg = pnew(CsrBtAvrcpCtPasValTxtRes); \
        msg->type          = CSR_BT_AVRCP_CT_PAS_VAL_TXT_RES;           \
        msg->connectionId  = _connId;                                   \
        msg->proceed       = _proceed;                                  \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasCurrentReqSend(_phandle, _connId, _attribIdCount, _attribId) { \
        CsrBtAvrcpCtPasCurrentReq *msg = pnew(CsrBtAvrcpCtPasCurrentReq); \
        msg->type          = CSR_BT_AVRCP_CT_PAS_CURRENT_REQ;           \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->attIdCount    = _attribIdCount;                            \
        msg->attId         = _attribId;                                 \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtPas
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPasSetReqSend(_phandle, _connId, _attValPairCount, _attValPair) { \
        CsrBtAvrcpCtPasSetReq *msg = pnew(CsrBtAvrcpCtPasSetReq);       \
        msg->type          = CSR_BT_AVRCP_CT_PAS_SET_REQ;               \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->attValPair    = _attValPair;                               \
        msg->attValPairCount = _attValPairCount;                        \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtSetAddressedPlayerReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtSetAddressedPlayerReqSend(_phandle, _connId, _playerId) { \
        CsrBtAvrcpCtSetAddressedPlayerReq *msg = pnew(CsrBtAvrcpCtSetAddressedPlayerReq); \
        msg->type              = CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_REQ; \
        msg->phandle           = _phandle;                              \
        msg->connectionId      = _connId;                               \
        msg->playerId          = _playerId;                             \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpCtSetBrowsedPlayerReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtSetBrowsedPlayerReqSend(_phandle, _connId, _playerId) { \
        CsrBtAvrcpCtSetBrowsedPlayerReq *msg = pnew(CsrBtAvrcpCtSetBrowsedPlayerReq); \
        msg->type              = CSR_BT_AVRCP_CT_SET_BROWSED_PLAYER_REQ; \
        msg->phandle           = _phandle;                              \
        msg->connectionId      = _connId;                               \
        msg->playerId          = _playerId;                             \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtGetFolderItemsReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtGetFolderItemsReqSend(_phandle, _connId, _scope, _startItem, _endItem, _attributeMask) { \
        CsrBtAvrcpCtGetFolderItemsReq *msg = pnew(CsrBtAvrcpCtGetFolderItemsReq); \
        msg->type          = CSR_BT_AVRCP_CT_GET_FOLDER_ITEMS_REQ;      \
        msg->connectionId  = _connId;                                   \
        msg->phandle       = _phandle;                                  \
        msg->scope         = _scope;                                    \
        msg->startItem     = _startItem;                                \
        msg->endItem       = _endItem;                                  \
        msg->attributeMask = _attributeMask;                            \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      Helper functions
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/

CsrBool CsrBtAvrcpCtLibGfiNextGet(CsrUint16 *index,
                                  CsrUint16 itemsLen,
                                  CsrUint8 *items,
                                  CsrBtAvrcpItemType *itemType);

CsrBool CsrBtAvrcpCtLibGfiMpGet(CsrUint16 *index,
                                CsrUint16 itemsLen,
                                CsrUint8 *items,
                                CsrUint16 *playerId,
                                CsrBtAvrcpFolderType *majorType,
                                CsrBtAvrcpMpTypeSub *subType,
                                CsrBtAvrcpPlaybackStatus *playbackStatus,
                                CsrBtAvrcpMpFeatureMask *featureMask,
                                CsrBtAvrcpCharSet *charset,
                                CsrUint16 *playerNameLen,
                                CsrUint8 **playerName);

CsrBool CsrBtAvrcpCtLibGfiFolderGet(CsrUint16 *index,
                                    CsrUint16 itemsLen,
                                    CsrUint8 *items,
                                    CsrBtAvrcpUid *folderUid,
                                    CsrBtAvrcpFolderType *folderType,
                                    CsrBtAvrcpFolderPlayableType *playableType,
                                    CsrBtAvrcpCharSet *charset,
                                    CsrUint16 *folderNameLen,
                                    CsrUint8 **folderName);

CsrBool CsrBtAvrcpCtLibGfiMediaGet(CsrUint16 *index,
                                   CsrUint16 itemsLen,
                                   CsrUint8 *items,
                                   CsrBtAvrcpUid *mediaUid,
                                   CsrBtAvrcpMediaType *mediaType,
                                   CsrBtAvrcpCharSet *charset,
                                   CsrUint16 *mediaNameLen,
                                   CsrUint8 **mediaName,
                                   CsrUint8 *attributeCount);

CsrBool CsrBtAvrcpCtLibGfiMediaAttributeNextGet(CsrUint16 *index,
                                                CsrUint16 *attIndex,
                                                CsrUint16 itemsLen,
                                                CsrUint8 *items,
                                                CsrBtAvrcpItemMediaAttributeId *attribId);

CsrBool CsrBtAvrcpCtLibGfiMediaAttributeGet(CsrUint16 maxData,
                                            CsrUint16 *attIndex,
                                            CsrUint16 itemsLen,
                                            CsrUint8 *items,
                                            CsrBtAvrcpItemMediaAttributeId *attribId,
                                            CsrBtAvrcpCharSet *charset,
                                            CsrUint16 *attLen,
                                            CsrUint8 **att);

CsrBool CsrBtAvrcpCtLibItemsAttributeGet(CsrUint16 maxData,
                                         CsrUint16 *attIndex,
                                         CsrUint16 itemsLen,
                                         CsrUint8 *items,
                                         CsrBtAvrcpItemMediaAttributeId *attribId,
                                         CsrBtAvrcpCharSet *charset,
                                         CsrUint16 *attLen,
                                         CsrUint8 **att);

#define CsrBtAvrcpCtLibElementsAttributeGet(maxData, attIndex, itemsLen, items, attribId, charset, attLen, att) \
        CsrBtAvrcpCtLibGfiMediaAttributeGet(maxData, attIndex, itemsLen, items, attribId, charset, attLen, att)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtChangePathReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtChangePathReqSend(_phandle, _connId, _uidCounter, _folderDir, _folderUid) { \
        CsrBtAvrcpCtChangePathReq *msg = pnew(CsrBtAvrcpCtChangePathReq); \
        msg->type          = CSR_BT_AVRCP_CT_CHANGE_PATH_REQ;           \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->folderDir     = _folderDir;                                \
        msg->uidCounter    = _uidCounter;                               \
        CsrMemCpy(msg->folderUid, _folderUid, sizeof(CsrBtAvrcpUid));   \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtGetAttributesReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtGetAttributesReqSend(_phandle, _connId, _scope, _uid, _uidCounter, _attributeMask) { \
        CsrBtAvrcpCtGetAttributesReq *msg = pnew(CsrBtAvrcpCtGetAttributesReq); \
        msg->type          = CSR_BT_AVRCP_CT_GET_ATTRIBUTES_REQ;        \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->scope         = _scope;                                    \
        msg->uidCounter    = _uidCounter;                               \
        msg->attributeMask = _attributeMask;                            \
        CsrMemCpy(msg->uid, _uid, sizeof(CsrBtAvrcpUid));               \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtGetAttributesResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtGetAttributesResSend(_connId, _proceed) {           \
        CsrBtAvrcpCtGetAttributesRes *msg = pnew(CsrBtAvrcpCtGetAttributesRes); \
        msg->type          = CSR_BT_AVRCP_CT_GET_ATTRIBUTES_RES;        \
        msg->connectionId  = _connId;                                   \
        msg->proceed       = _proceed;                                  \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtPlayReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtPlayReqSend(_phandle, _connId, _scope, _uidCounter, _uid) { \
        CsrBtAvrcpCtPlayReq *msg = pnew(CsrBtAvrcpCtPlayReq);           \
        msg->type          = CSR_BT_AVRCP_CT_PLAY_REQ;                  \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->scope         = _scope;                                    \
        msg->uidCounter    = _uidCounter;                               \
        CsrMemCpy(msg->uid, _uid, sizeof(CsrBtAvrcpUid));               \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtAddToNowPlayingReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtAddToNowPlayingReqSend(_phandle, _connId, _scope, _uidCounter, _uid) { \
        CsrBtAvrcpCtAddToNowPlayingReq *msg = pnew(CsrBtAvrcpCtAddToNowPlayingReq); \
        msg->type          = CSR_BT_AVRCP_CT_ADD_TO_NOW_PLAYING_REQ;    \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->scope         = _scope;                                    \
        msg->uidCounter    = _uidCounter;                               \
        CsrMemCpy(msg->uid, _uid, sizeof(CsrBtAvrcpUid));               \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtSearchReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtSearchReqSend(_phandle, _connId, _text) {           \
        CsrBtAvrcpCtSearchReq *msg = pnew(CsrBtAvrcpCtSearchReq);       \
        msg->type          = CSR_BT_AVRCP_CT_SEARCH_REQ;                \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->text          = _text;                                     \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtSetVolumeReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtSetVolumeReqSend(_phandle, _connId, _volume) {      \
        CsrBtAvrcpCtSetVolumeReq *msg = pnew(CsrBtAvrcpCtSetVolumeReq); \
        msg->type          = CSR_BT_AVRCP_CT_SET_VOLUME_REQ;            \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->volume        = _volume;                                   \
        CsrBtAvrcpMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtInformBatteryStatusReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtInformBatteryStatusReqSend(_phandle, _connId, _batStatus) { \
        CsrBtAvrcpCtInformBatteryStatusReq *msg = pnew(CsrBtAvrcpCtInformBatteryStatusReq); \
        msg->type          = CSR_BT_AVRCP_CT_INFORM_BATTERY_STATUS_REQ; \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->batStatus     = _batStatus;                                \
        CsrBtAvrcpMsgTransport(msg);}

#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtUnitInfoCmdReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtUnitInfoCmdReqSend(_phandle, _connId, _pDatalen, _pData) { \
        CsrBtAvrcpCtUnitInfoCmdReq *msg  = pnew(CsrBtAvrcpCtUnitInfoCmdReq); \
        msg->type         = CSR_BT_AVRCP_CT_UNIT_INFO_CMD_REQ;          \
        msg->connectionId = _connId;                                    \
        msg->phandle      = _phandle;                                   \
        msg->pDataLen     = _pDatalen;                                  \
        msg->pData        = _pData;                                     \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtSubUnitInfoCmdReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtSubUnitInfoCmdReqSend(_phandle, _connId, _pDatalen, _pData) { \
        CsrBtAvrcpCtSubUnitInfoCmdReq *msg  = pnew(CsrBtAvrcpCtSubUnitInfoCmdReq); \
        msg->type         = CSR_BT_AVRCP_CT_SUB_UNIT_INFO_CMD_REQ;      \
        msg->connectionId = _connId;                                    \
        msg->phandle      = _phandle;                                   \
        msg->pDataLen     = _pDatalen;                                  \
        msg->pData        = _pData;                                     \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpCtGetTotalNumberOfItemsReqSend
 *
 *  DESCRIPTION
 *      CsrBtAvrcpCtGetTotalNumberOfItemsReq msg can be used to retrieve the total number 
 *      of items in a folder prior to retrieving a listing of the contents of a browsed folder and can 
 *      be used by Controller to:
 *          - find the total number of Media players on Target
 *          - find total number of media element items in a folder (media file system) on Target
 *          - find total number of media element items in Search Results list on Target
 *          - find total number of media element items in Queue/Now Playing list of addressed 
 *             Media player on Target
 *
 *    PARAMETERS
 *      connId:     Unique number for identifying the specific connection 
 *      scope:      The scope of browsing(MPL/MP VFS/SEARCH/NPL)
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpCtGetTotalNumberOfItemsReqSend(_phandle, _connId, _scope) { \
        CsrBtAvrcpCtGetTotalNumberOfItemsReq *msg = pnew(CsrBtAvrcpCtGetTotalNumberOfItemsReq); \
        msg->type          = CSR_BT_AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS_REQ; \
        msg->phandle       = _phandle;                                  \
        msg->connectionId  = _connId;                                   \
        msg->scope         = _scope;                                \
        CsrBtAvrcpMsgTransport(msg);}

#endif

#ifndef EXCLUDE_CSR_BT_AVRCP_TG_MODULE
/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgPassThroughRes
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgPassThroughResSend(_connId, _msgId, _status) {      \
        CsrBtAvrcpTgPassThroughRes *msg = pnew(CsrBtAvrcpTgPassThroughRes); \
        msg->type          = CSR_BT_AVRCP_TG_PASS_THROUGH_RES;          \
        msg->connectionId  = _connId;                                   \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgGetPlayStatusResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgGetPlayStatusResSend(_connId, _songLength, _songPosition, _playStatus, _msgId, _status) { \
        CsrBtAvrcpTgGetPlayStatusRes *msg = pnew(CsrBtAvrcpTgGetPlayStatusRes); \
        msg->type          = CSR_BT_AVRCP_TG_GET_PLAY_STATUS_RES;       \
        msg->connectionId  = _connId;                                   \
        msg->songLength    = _songLength;                               \
        msg->songPosition  = _songPosition;                             \
        msg->playStatus    = _playStatus;                               \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgNotiRes
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgNotiResSend(_connId, _notiId, _notiData, _status, _msgId) { \
        CsrBtAvrcpTgNotiRes *msg = pnew(CsrBtAvrcpTgNotiRes);           \
        msg->type          = CSR_BT_AVRCP_TG_NOTI_RES;                  \
        msg->connectionId  = _connId;                                   \
        msg->notiId        = _notiId;                                   \
        msg->status        = _status;                                   \
        msg->msgId         = _msgId;                                    \
        CsrMemCpy(msg->notiData, _notiData,CSR_BT_AVRCP_TG_NOTI_MAX_SIZE); \
        CsrBtAvrcpMsgTransport(msg);}

void CsrBtAvrcpTgNotiPlaybackStatusRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrBtAvrcpPlaybackStatus playbackStatus);
void CsrBtAvrcpTgNotiTrackRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrBtAvrcpUid uid);
void CsrBtAvrcpTgNotiTrackStartEndRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrBool start);
void CsrBtAvrcpTgNotiPlaybackPositionRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrUint32 pos);
void CsrBtAvrcpTgNotiBatStatusRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrBtAvrcpBatteryStatus batStatus);
void CsrBtAvrcpTgNotiSystemStatusRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrBtAvrcpSystemStatus sysStatus);
void CsrBtAvrcpTgNotiNowPlayingRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId);
void CsrBtAvrcpTgNotiUidsRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrUint16 uidCounter);
void CsrBtAvrcpTgNotiVolumeRes(CsrUint8 connId, CsrBtAvrcpStatus status, CsrUint32 msgId, CsrUint8 volume);

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgNotiReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgNotiReqSend(_playerId, _notiId, _notiData) {        \
        CsrBtAvrcpTgNotiReq *msg = pnew(CsrBtAvrcpTgNotiReq);           \
        msg->type          = CSR_BT_AVRCP_TG_NOTI_REQ;                  \
        msg->playerId      = _playerId;                                 \
        msg->notiId        = _notiId;                                   \
        CsrMemCpy(msg->notiData, _notiData,CSR_BT_AVRCP_TG_NOTI_MAX_SIZE); \
        CsrBtAvrcpMsgTransport(msg);}

void CsrBtAvrcpTgNotiPlaybackStatusReq(CsrUint32 playerId, CsrUint8 playbackStatus);
void CsrBtAvrcpTgNotiTrackReq(CsrUint32 playerId, CsrBtAvrcpUid uid);
void CsrBtAvrcpTgNotiTrackStartEndReq(CsrUint32 playerId, CsrBool start);
void CsrBtAvrcpTgNotiPlaybackPositionReq(CsrUint32 playerId, CsrUint32 pos);
void CsrBtAvrcpTgNotiBatStatusReq(CsrUint32 playerId, CsrBtAvrcpBatteryStatus batStatus);
void CsrBtAvrcpTgNotiSystemStatusReq(CsrUint32 playerId, CsrBtAvrcpSystemStatus sysStatus);
void CsrBtAvrcpTgNotiNowPlayingReq(CsrUint32 playerId);
void CsrBtAvrcpTgNotiUidsReq(CsrUint32 playerId, CsrUint16 uidCounter);
void CsrBtAvrcpTgNotiVolumeReq(CsrUint32 playerId, CsrUint8 volume);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgPasCurrentResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgPasCurrentResSend(_connId, _msgId, _pasCount, _pas, _status) { \
        CsrBtAvrcpTgPasCurrentRes *msg = pnew(CsrBtAvrcpTgPasCurrentRes); \
        msg->type          = CSR_BT_AVRCP_TG_PAS_CURRENT_RES;           \
        msg->connectionId  = _connId;                                   \
        msg->msgId         = _msgId;                                    \
        msg->attValPairCount      = _pasCount;                          \
        msg->attValPair           = _pas;                               \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgPasSetReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgPasSetReqSend(_phandle, _playerId, _changedPasCount, _changedPas) { \
        CsrBtAvrcpTgPasSetReq *msg = pnew(CsrBtAvrcpTgPasSetReq);       \
        msg->type          = CSR_BT_AVRCP_TG_PAS_SET_REQ;               \
        msg->phandle       = _phandle;                                  \
        msg->playerId      = _playerId;                                 \
        msg->attValPair    = _changedPas;                               \
        msg->attValPairCount = _changedPasCount;                        \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgPasSetResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgPasSetResSend(_connId, _msgId, _status) {           \
        CsrBtAvrcpTgPasSetRes *msg = pnew(CsrBtAvrcpTgPasSetRes);       \
        msg->type          = CSR_BT_AVRCP_TG_PAS_SET_RES;               \
        msg->connectionId  = _connId;                                   \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}


#define CSR_BT_AVRCP_TG_LIB_PAS_INVALID_INDEX      (0xFFFF)

#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_COUNT_IDX      (0)

/* Relative to the beginning of an attribute */
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_SIZE           (5)
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_ID_IDX         (0)
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_LEN_IDX        (1)
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_VAL_COUNT_IDX  (3)
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_TXT_LEN_IDX    (4)
#define CSR_BT_AVRCP_TG_LIB_PAS_ATT_TXT_IDX        (5)

/* Relative to the beginning of a value */
#define CSR_BT_AVRCP_TG_LIB_PAS_VAL_SIZE           (2)
#define CSR_BT_AVRCP_TG_LIB_PAS_VAL_ID_IDX         (0)
#define CSR_BT_AVRCP_TG_LIB_PAS_VAL_TXT_LEN_IDX    (1)
#define CSR_BT_AVRCP_TG_LIB_PAS_VAL_TXT_IDX        (2)

/* TG PAS helper functions */
CsrUint16 CsrBtAvrcpTgLibPasAttribGet(CsrUint16 pasLen, CsrUint8 *pas, CsrUint8 attId);
CsrUint16 CsrBtAvrcpTgLibPasValueGet(CsrUint16 pasLen, CsrUint8 *pas, CsrUint8 attId, CsrUint8 valId);
CsrUint16 CsrBtAvrcpTgLibPasValueFirstGet(CsrUint16 pasLen, CsrUint8 *pas, CsrUint16 attIndex);
CsrBool CsrBtAvrcpTgLibPasAttribNext(CsrUint16 pasLen, CsrUint8 *pas, CsrUint16 *attIndex);
CsrBool CsrBtAvrcpTgLibPasValueNext(CsrUint16 pasLen, CsrUint8 *pas, CsrUint16 attIdIndex, CsrUint16 *valIndex);
CsrUint8 CsrBtAvrcpTgLibPasAttribCount(CsrUint16 pasLen, CsrUint8 *pas);
CsrUint8 CsrBtAvrcpTgLibPasValueCount(CsrUint16 pasLen, CsrUint8 *pas, CsrUint8 attId);

#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
void CsrBtAvrcpTgLibPasAttribAdd(CsrUint16 *pasLen, CsrUint8 **pas, CsrBtAvrcpPasAttId attId, const CsrUtf8String *attTxt);
void CsrBtAvrcpTgLibPasValueAdd(CsrUint16 *pasLen, CsrUint8 **pas, CsrBtAvrcpPasAttId attId, CsrBtAvrcpPasValId valId, const CsrUtf8String *valTxt);
#else
#define CsrBtAvrcpTgLibPasAttribAdd(pasLen, pas, attId, attTxt)
#define CsrBtAvrcpTgLibPasValueAdd(pasLen, pas, attId, valId, valTxt)
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgMpRegisterReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgMpRegisterReqSend(_playerHandle, _notificationMask, _configMask, _pasLen, _pas, _majorType, _subType, _featureMask, _playerName) { \
        CsrBtAvrcpTgMpRegisterReq *msg = pnew(CsrBtAvrcpTgMpRegisterReq); \
        msg->type              = CSR_BT_AVRCP_TG_MP_REGISTER_REQ;       \
        msg->playerHandle      = _playerHandle;                         \
        msg->notificationMask  = _notificationMask;                     \
        msg->configMask        = _configMask;                           \
        msg->pasLen            = _pasLen;                               \
        msg->pas               = _pas;                                  \
        msg->majorType         = _majorType;                            \
        msg->subType           = _subType;                              \
        msg->playerName        = _playerName;                           \
        CSR_BT_AVRCP_FEATURE_MASK_COPY(msg->featureMask, _featureMask); \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgMpUnregisterReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgMpUnregisterReqSend(_phandle, _playerId) {          \
        CsrBtAvrcpTgMpUnregisterReq *msg = pnew(CsrBtAvrcpTgMpUnregisterReq); \
        msg->type              = CSR_BT_AVRCP_TG_MP_UNREGISTER_REQ;     \
        msg->phandle           = _phandle;                              \
        msg->playerId          = _playerId;                             \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgSetAddressedPlayerReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgSetAddressedPlayerReqSend(_phandle, _playerId, _uidCounter) { \
        CsrBtAvrcpTgSetAddressedPlayerReq *msg = pnew(CsrBtAvrcpTgSetAddressedPlayerReq); \
        msg->type              = CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_REQ; \
        msg->phandle           = _phandle;                              \
        msg->playerId          = _playerId;                             \
        msg->uidCounter        = _uidCounter;                           \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgSetAddressedPlayerRes
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgSetAddressedPlayerResSend(_connId, _playerId, _uidCounter, _msgId, _status) { \
        CsrBtAvrcpTgSetAddressedPlayerRes *msg = pnew(CsrBtAvrcpTgSetAddressedPlayerRes); \
        msg->type              = CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_RES; \
        msg->uidCounter        = _uidCounter;                           \
        msg->connectionId      = _connId;                               \
        msg->msgId             = _msgId;                                \
        msg->playerId          = _playerId;                             \
        msg->status            = _status;                               \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      AvrcpTgSetBrowsedPlayerRes
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgSetBrowsedPlayerResSend(_connId, _playerId, _uidCounter, _itemsCount, _folderDepth, _folderNamesLen, _folderNames, _msgId, _status) { \
        CsrBtAvrcpTgSetBrowsedPlayerRes *msg = pnew(CsrBtAvrcpTgSetBrowsedPlayerRes); \
        msg->type              = CSR_BT_AVRCP_TG_SET_BROWSED_PLAYER_RES; \
        msg->uidCounter        = _uidCounter;                           \
        msg->itemsCount        = _itemsCount;                           \
        msg->folderDepth       = _folderDepth;                          \
        msg->folderNamesLen    = _folderNamesLen;                       \
        msg->folderNames       = _folderNames;                          \
        msg->msgId             = _msgId;                                \
        msg->playerId          = _playerId;                             \
        msg->status            = _status;                               \
        msg->connectionId      = _connId;                               \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgGetTotalNumberOfItemsResSend
 *
 *  DESCRIPTION
 *      Response for get total number of items indication.
 *
 *  PARAMETERS
 *      connId:     The unique connection ID of the AVRCP connection
 *      noOfItems:  Total number of media items in the response message
 *      uidCounter: The current UID counter
 *      msgId:      Unique message ID for associating indications and responses
 *      status:     Indicates whether the operation succeeded or failed
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgGetTotalNumberOfItemsResSend(_connId, _noOfItems, _uidCounter, _msgId, _status) { \
        CsrBtAvrcpTgGetTotalNumberOfItemsRes *msg = pnew(CsrBtAvrcpTgGetTotalNumberOfItemsRes); \
        msg->type          = CSR_BT_AVRCP_TG_GET_TOTAL_NUMBER_OF_ITEMS_RES;      \
        msg->connectionId  = _connId;                                   \
        msg->noOfItems     = _noOfItems;                                \
        msg->uidCounter    = _uidCounter;                               \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgGetFolderItemsResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgGetFolderItemsResSend(_connId, _itemCount, _uidCounter, _itemsLen, _items, _msgId, _status) { \
        CsrBtAvrcpTgGetFolderItemsRes *msg = pnew(CsrBtAvrcpTgGetFolderItemsRes); \
        msg->type          = CSR_BT_AVRCP_TG_GET_FOLDER_ITEMS_RES;      \
        msg->connectionId  = _connId;                                   \
        msg->itemsCount    = _itemCount;                                \
        msg->uidCounter    = _uidCounter;                               \
        msg->itemsLen      = _itemsLen;                                 \
        msg->items         = _items;                                    \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}



CsrBool CsrBtAvrcpTgLibGfiFolderAdd(CsrUint16 maxData,
                                    CsrUint16 *itemsLen,
                                    CsrUint8 **items,
                                    CsrBtAvrcpUid *folderUid,
                                    CsrBtAvrcpFolderType folderType,
                                    CsrBtAvrcpFolderPlayableType playableType,
                                    CsrBtAvrcpCharSet charset,
                                    CsrCharString *folderName);

CsrBool CsrBtAvrcpTgLibGfiMediaAdd(CsrUint16 maxData,
                                   CsrUint16 *index,
                                   CsrUint8 **data,
                                   CsrUint16 *mediaIndex,
                                   CsrBtAvrcpUid *mediaUid,
                                   CsrBtAvrcpMediaType mediaType,
                                   CsrBtAvrcpCharSet charset,
                                   CsrCharString *mediaName);

CsrBool CsrBtAvrcpTgLibGfiMediaAttributeAdd(CsrUint16 maxData,
                                            CsrUint16 *itemsLen,
                                            CsrUint8 **items,
                                            CsrUint16 mediaIndex,
                                            CsrBtAvrcpItemMediaAttributeId attribId,
                                            CsrBtAvrcpCharSet charset,
                                            CsrUint16 attLen,
                                            CsrUint8 *att);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgChangePathResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgChangePathResSend(_connId, _itemsCount, _msgId, _status) { \
        CsrBtAvrcpTgChangePathRes *msg = pnew(CsrBtAvrcpTgChangePathRes); \
        msg->type          = CSR_BT_AVRCP_TG_CHANGE_PATH_RES;           \
        msg->connectionId  = _connId;                                   \
        msg->itemsCount    = _itemsCount;                               \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgGetAttributesResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgGetAttributesResSend(_connId, _attribCount, _attribDataLen, _attribData, _msgId, _status) { \
        CsrBtAvrcpTgGetAttributesRes *msg = pnew(CsrBtAvrcpTgGetAttributesRes); \
        msg->type          = CSR_BT_AVRCP_TG_GET_ATTRIBUTES_RES;        \
        msg->connectionId  = _connId;                                   \
        msg->attribCount   = _attribCount;                              \
        msg->attribDataLen = _attribDataLen;                            \
        msg->attribData    = _attribData;                               \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgPlayResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgPlayResSend(_connId, _uid, _scope, _msgId, _status) { \
        CsrBtAvrcpTgPlayRes *msg = pnew(CsrBtAvrcpTgPlayRes);           \
        msg->type          = CSR_BT_AVRCP_TG_PLAY_RES;                  \
        msg->connectionId  = _connId;                                   \
        msg->scope         = _scope;                                    \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrMemCpy(msg->uid, _uid, sizeof(CsrBtAvrcpUid));               \
        CsrBtAvrcpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgAddToNowPlayingResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgAddToNowPlayingResSend(_connId, _msgId, _status) {  \
        CsrBtAvrcpTgAddToNowPlayingRes *msg = pnew(CsrBtAvrcpTgAddToNowPlayingRes); \
        msg->type          = CSR_BT_AVRCP_TG_ADD_TO_NOW_PLAYING_RES;    \
        msg->connectionId  = _connId;                                   \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgSearchResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgSearchResSend(_connId, _uidCounter, _numberOfItems, _msgId, _status) { \
        CsrBtAvrcpTgSearchRes *msg = pnew(CsrBtAvrcpTgSearchRes);       \
        msg->type          = CSR_BT_AVRCP_TG_SEARCH_RES;                \
        msg->connectionId  = _connId;                                   \
        msg->uidCounter    = _uidCounter;                               \
        msg->numberOfItems = _numberOfItems;                            \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpTgSetVolumeResSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *       FIXME
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpTgSetVolumeResSend(_connId, _volume, _msgId, _status) { \
        CsrBtAvrcpTgSetVolumeRes *msg = pnew(CsrBtAvrcpTgSetVolumeRes); \
        msg->type          = CSR_BT_AVRCP_TG_SET_VOLUME_RES;            \
        msg->connectionId  = _connId;                                   \
        msg->volume        = _volume;                                   \
        msg->msgId         = _msgId;                                    \
        msg->status        = _status;                                   \
        CsrBtAvrcpMsgTransport(msg);}

#endif

/* Accumulates payload from multiple packets into a single buffer */
CsrBool CsrBtAvrcpCtLibAppendPayload(CsrUint8 **buffer,
                                     CsrUint16 *bufferLen,
                                     CsrUint8 *rxData,
                                     CsrUint16 rxDataLen,
                                     CsrUint16 rxDataOffset);

void CsrBtAvrcpUtilFreeRoleDetails(CsrBtAvrcpRoleDetails *ptr);
void CsrBtAvrcpFreeRemoteFeaturesInd(CsrBtAvrcpRemoteFeaturesInd **prim);

void CsrBtAvrcpFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif
