#ifndef CSR_BT_PAC_LIB_H__
#define CSR_BT_PAC_LIB_H__

/******************************************************************************

Copyright (c) 2006-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_pac_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_PAC_IS_SUPPORT_FEATURE_DOWNLOAD(flag)   (CsrBool)((flag) & CSR_BT_PB_FEATURE_DOWNLOAD)
#define CSR_BT_PAC_IS_SUPPORT_FEATURE_BROWSING(flag)   (CsrBool)((flag) & CSR_BT_PB_FEATURE_BROWSING)
#define CSR_BT_PAC_IS_SUPPORT_REPOSITORY_PB(flag)      (CsrBool)((flag) & CSR_BT_PB_REPO_PHONE)
#define CSR_BT_PAC_IS_SUPPORT_REPOSITORY_SIM(flag)     (CsrBool)((flag) & CSR_BT_PB_REPO_SIM)

/* Common put_message function to reduce code size */
void CsrBtPacMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacConnectReqSend
 *
 *  DESCRIPTION
 *      Connect Request
 *      To connect to a PBAP Server, the application sends a Connect Request (CSR_BT_PAC_CONNECT_REQ) message to
 *      the PAC. In this message, the application should specify which device to connect to.
 *      On receiving the connect request, the PAC attempts to find PBAP server services on target device. If
 *      multiple PBAP servers are found on target device, the PAC attempts connection to first PBAP v1.2 server.
 *      If there are no PBAP v1.2 servers, the PAC attempts connection to all legacy PBAP servers, one by one,
 *      until an OBEX connection is established.
 *      During connection, the PAC exchanges its features, maximum packet size among other connection parameters
 *      with the PBAP server.
 *      The PAC replies to the application with Connect Confirm (CSR_BT_PAC_CONNECT_CFM) on completing connection
 *      procedure. The result of connection procedure is returned in resultCode member of connect confirm. The
 *      resultCode other than CSR_BT_OBEX_SUCCESS_RESPONSE_CODE is a failure
 *      The CsrBtPacConnecReqSend()library function is used to formulate and send Connect request to the PAC.
 *
 *  PARAMETERS
 *      appHandle:      The identity of the calling process. It is possible to initiate the procedure by any
 *                      higher layer process as the response is always returned to appHandle. The
 *                      PAC sends signals only to this task
 *      maxPacketSize:  Maximum OBEX packet size application wants to receive from the client side
 *      destination:    Remote device BD Address
 *      windowSize:     Controls how many packets the OBEX profile (and lower protocol layers) can cache on
 *                      the data receive side. If set to 0 the system auto-detects this value.
 *----------------------------------------------------------------------------*/
#define CsrBtPacConnectReqSend(_appHandle, _maxPacketSize,          \
                               _destination, _windowSize)           \
do{                                                                 \
    CsrBtPacConnectReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacConnectReq)); \
    msg__->type              = CSR_BT_PAC_CONNECT_REQ;              \
    msg__->appHandle         = _appHandle;                          \
    msg__->maxPacketSize     = _maxPacketSize;                      \
    msg__->destination       = _destination;                        \
    msg__->windowSize        = _windowSize;                         \
    CsrBtPacMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacAuthenticateResSend
 *
 *  DESCRIPTION
 *      Authentication Response
 *      This API is used to respond to authentication request initiated by PBAP server.
 *      The OBEX authentication is different from Service Level Security. The PAC does not support initiating
 *      OBEX authentication, however it still supports OBEX authentication if initiated by server. On
 *      receiving OBEX authentication request from server, the PAC notifies application with Authentication
 *      Indication (CSR_BT_PAC_AUTHENTICATION_IND). The Application is expected to respond with proper
 *      authentication information through Authentication Response (CSR_BT_PAC_AUTHENTICATION_RES)
 *
 *  PARAMETERS
 *      password:       Contains the response password of the OBEX authentication.
 *                      The application allocates this pointer.
 *      passwordLength: The length of the response password.
 *      userId:         Zero terminated string (ASCII) containing the user Id for the authentication. The
 *                      application allocates this pointer.
 *----------------------------------------------------------------------------*/
#define CsrBtPacAuthenticateResSend(_password, _passwordLength, _userId)\
do{                                                                     \
    CsrBtPacAuthenticateRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPacAuthenticateRes)); \
    msg__->type              = CSR_BT_PAC_AUTHENTICATE_RES;         \
    msg__->password          = _password;                           \
    msg__->passwordLength    = _passwordLength;                     \
    msg__->userId            = _userId;                             \
    CsrBtPacMsgTransport(msg__);                                    \
}while(0)

/*-------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullPbReqSend
 *
 *  DESCRIPTION
 *      Pull Phonebook Request
 *      The Pull Phonebook Request (CSR_BT_PAC_PULL_PB_REQ) downloads an entire phone book folder from the
 *      server. It is useful for devices with relatively large storage capacity that download and store the
 *      entire phone book locally. This operation can be requested from any PBAP virtual folder.
 *      The Pull Phonebook Request contains target folder path, filter criteria among other directives for
 *      PBAP server. In response to this request, server sends phonebook objects as vCard object file in
 *      sequential order of vCard handles.
 *      If target folder path is illegal or not supported by server, the PAC drops request and responds to
 *      application with CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE as responseCode in Pull Phonebook confirmation.
 *      All other Pull Phonebook request parameters are validated and refined before requesting from server. Thus,
 *      parameter values that are illegal or are not supported are masked in resulting OBEX GET requests to server
 *
 *  PARAMETERS
 *      ucs2name:       A null terminated 16 bit Unicode big-endian text string (UCS-2BE)
 *                      containing the file name of the object.
 *      src:            Indicate whether phone book source should be the internal memory
 *                      (CSR_BT_PAC_SRC_PHONE) or the SIM (CSR_BT_PAC_SRC_SIM).
 *      filter:         Bitmap of vCard properties to be included in phonebook objects in requested property
 *                      is available in concerned contact vCards.
 *                      Some of the properties are mandatory depending upon vCard format type (format)
 *                      requested. If none of bits are set, all supported properties are assumed to be set.
 *                      PAC drops unsupported vCard properties from this parameter before passing it to server.
                        Only attributes N and TEL are mandatory to be supported by the PBAP
 *                      server.
 *                              filter[0] contains bit 0 - 7,
 *                              filter[1] contains bit 8 - 15,
 *                              filter[2] contains bit 16 - 23,
 *                              filter[3] contains bit 24 - 31,
 *                              filter[4] contains bit 32 - 39,
 *                              filter[5] contains bit 40 - 47,
 *                              filter[6] contains bit 48 - 55 and
 *                              filter[7] contains bit 56 - 63.
 *      format:         Indicate the requested format:
 *                              vCard 2.1 (CSR_BT_PAC_FORMAT_VCARD2_1) or
 *                              vCard 3.0 (CSR_BT_PAC_FORMAT_VCARD3:0)
 *      maxListCnt:     Used for indicating the maximum number of entries of the phone book PAC
 *                      can recevie from PAS.
 *                      maxlistCnt = 0, indicates PAC wants to know the actual number of indexes
                        used in the phone book of interest.
 *                      When maxListCnt is zero, PAS ignores all other application parameters that may be
 *                      present in the request. The response will not contain any body header(phone book object).
 *
 *      listStartOffset:    Indicates server the offset in requested phonebook to send vCard objects from.
 *                      The first vCard handle cannot be smaller than this value.
 *      srmpOn:         Suspends the Single Response Mode if TRUE. It is applicable only if peer device supports SRM.
 *----------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullPbReqSend(_ucs2name, _src, _filter, _format, _maxLstCnt, _listStartOffset, _srmpOn) \
    CsrBtPacPullPbReqSendEx(_ucs2name, _src, _filter, _format, _maxLstCnt, _listStartOffset,            \
                            CSR_BT_PB_DO_NOT_RESET_NEW_MISSED_CALL, NULL, 0, _srmpOn)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullPbReqSendEx
 *
 *  DESCRIPTION
 *      Extended Pull Phonebook Request
 *      The Extended Pull Phonebook request has been introduced to accommodate new features of PBAP v1.2.
 *
 *  PARAMETERS
 *      ucs2name:          A null terminated 16 bit Unicode big-endian text string (UCS-2BE) containing the file
 *                         name of the object.
 *      src:               Indicate whether phone book source should be the internal memory
 *                         (CSR_BT_PAC_SRC_PHONE) or the SIM (CSR_BT_PAC_SRC_SIM).
 *      filter:            Bitmap of vCard properties to be included in phonebook objects in requested property
 *                         is available in concerned contact vCards.
 *                         Some of the properties are mandatory depending upon vCard format type (format)
 *                         requested. If none of bits are set, all supported properties are assumed to be set.
 *                         PAC drops unsupported vCard properties from this parameter before passing it to server.
 *                         Only attributes N and TEL are mandatory to be supported by the PBAP
 *                         server.
 *                             filter[0] contains bit 0 - 7,
 *                             filter[1] contains bit 8 - 15,
 *                             filter[2] contains bit 16 - 23,
 *                             filter[3] contains bit 24 - 31,
 *                             filter[4] contains bit 32 - 39,
 *                             filter[5] contains bit 40 - 47,
 *                             filter[6] contains bit 48 - 55 and
 *                             filter[7] contains bit 56 - 63.
 *      format:            Indicate the requested format:
 *                             vCard 2.1 (CSR_BT_PAC_FORMAT_VCARD2_1) or
 *                             vCard 3.0 (CSR_BT_PAC_FORMAT_VCARD3:0)
 *      maxListCnt:        Used for indicating the maximum number of entries of the phone book PAC
 *                         can recevie from PAS.
 *                         maxlistCnt = 0, indicates PAC wants to know the actual number of indexes
 *                         used in the phone book of interest.
 *                         When maxListCnt is zero, PAS ignores all other application parameters that may be
 *                         present in the request. The response will not contain any body header(phone book object).
 *
 *      listStartOffset:        Indicates server the offset in requested phonebook to send vCard objects from.
 *                              The first vCard handle cannot be smaller than this value.
 *      resetNewMissedCalls:    Resets number of new missed calls in server database. Normally this parameter is
 *                              applicable only for MCH folder. However if Enhanced Missed Calls is supported,
 *                              this parameter is valid for CCH folder as well.
 *      vCardSelector:          Bitmap of vCard properties for selecting the vCards. Only the vCards containing these
 *                              properties would be considered for response by server.
 *                              PAC drops unsupported vCard properties from this parameter before passing it to server.
 *      vCardSelectorOperator:  This determines which logic to be used when multiple bits of the
 *                              vCardSelector application parameter are set. A bit value of 0 indicates
 *                              that (OR) logic would be used, a bit value of 1 indicates that AND logic
 *                              would be used.
 *      srmpOn:                 Temporarily suspends the Single Response Mode if TRUE.
 *                              It is applicable only if peer device supports SRM.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullPbReqSendEx(_ucs2name, _src, _filter, _format, _maxLstCnt,  \
                                _listStartOffset, _resetNewMissedCalls,         \
                                _vCardSelector,_vCardSelectorOperator, _srmpOn) \
do{                                                                             \
        CsrBtPacPullPbReq *msg__     = CsrPmemZalloc(sizeof(CsrBtPacPullPbReq));\
        msg__->type                  = CSR_BT_PAC_PULL_PB_REQ;                  \
        msg__->ucs2name              = _ucs2name;                               \
        msg__->src                   = _src;                                    \
        if(_filter != NULL)                                                     \
        {                                                                       \
            CsrMemCpy(msg__->filter, _filter, sizeof(msg__->filter));           \
        }                                                                       \
        msg__->format                = _format;                                 \
        msg__->maxListCnt            = _maxLstCnt;                              \
        msg__->listStartOffset       = _listStartOffset;                        \
        msg__->srmpOn                = _srmpOn;                                 \
        msg__->resetNewMissedCalls   = _resetNewMissedCalls;                    \
        if(_vCardSelector != NULL)                                              \
        {                                                                       \
            CsrMemCpy(msg__->vCardSelector,                                     \
                      _vCardSelector,                                           \
                      sizeof(msg__->vCardSelector));                            \
            msg__->vCardSelectorOperator = _vCardSelectorOperator;              \
        }                                                                       \
        CsrBtPacMsgTransport(msg__);                                            \
}while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullPbResSend
 *
 *  DESCRIPTION
 *      Pull Phonebook Response
 *      The library function CsrBtPacPullPbResSend() is used to acknowledge the Pull Phonebook indications.
 *      It also indicates to PAC that application is ready to receive the next part of the phone book.
 *
 *  PARAMETERS
 *      srmpOn:         Temporarily suspends the Single Response Mode if TRUE.
 *                      It is applicable only if peer device supports SRM.
 *                      This parameter is effective only if preceding request and responses during current
 *                      Phonebook Download operation had set srmpOn parameter.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullPbResSend(_srmpOn)                                      \
do{                                                                         \
        CsrBtPacPullPbRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPacPullPbRes)); \
        msg__->type   = CSR_BT_PAC_PULL_PB_RES;                             \
        msg__->srmpOn = _srmpOn;                                            \
        CsrBtPacMsgTransport(msg__);                                        \
}while(0)


/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacSetFolderReqSend
 *
 *  DESCRIPTION
 *      Set Folder Request
 *      The Set Folder Request (CSR_BT_PAC_SET_FOLDER_REQ) changes the current folder on the server to a folder
 *      specified with the name parameter. This navigates down in the directory hierarchy on the server.
 *      The Set Folder Confirm (CSR_BT_PAC_SET_FOLDER_CFM) primitive gives the result of the change folder
 *      operation. The result can contain error codes corresponding to the reason for failure if the folder
 *      does not exist or if the server does not permit this operation.
 *
 *  PARAMETERS
 *      ucs2name:   A null terminated 16-bit Unicode big-endian text string (UCS-2BE) containing the
 *                  (file) name of the object.
 *                  The CsrUtf82Ucs2String function can convert a null terminated UTF8 text string into a null
 *                  terminated UCS2 text string.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacSetFolderReqSend(_ucs2name)             \
do{                                                     \
        CsrBtPacSetFolderReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacSetFolderReq)); \
        msg__->type     = CSR_BT_PAC_SET_FOLDER_REQ;    \
        msg__->ucs2name = _ucs2name;                    \
        CsrBtPacMsgTransport(msg__);                    \
}while(0)


/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacSetBackFolderReqSend
 *
 *  DESCRIPTION
 *      Set Back Folder Request
 *      The Set Back Folder Request (CSR_BT_PAC_SET_BACK_FOLDER_REQ) sets the current folder back to the
 *      parent folder. The Set Back Folder Confirm (CSR_BT_PAC_SET_BACK_FOLDER_CFM) signal gives the result
 *      of the operation. If the current folder is the root folder, the confirm signal has
 *      CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE as result code.
 *
 *  PARAMETERS
 *         NONE
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacSetBackFolderReqSend()                  \
do{                                                     \
        CsrBtPacSetBackFolderReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacSetBackFolderReq)); \
        msg__->type = CSR_BT_PAC_SET_BACK_FOLDER_REQ;   \
        CsrBtPacMsgTransport(msg__);                    \
}while(0)


/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacSetRootFolderReqSend
 *
 *  DESCRIPTION
 *      Set Root Folder Request
 *      The Set Root Folder Request (CSR_BT_PAC_SET_ROOT_FOLDER_REQ) sets the current folder back to the root
 *      folder. The PAC responds with Set Root Folder Confirm (CSR_BT_PAC_SET_ROOT_FOLDER_CFM) on completing
 *      this operation. The result in the confirmation message can contain error codes corresponding to the
 *      reason for the failure on the server. Normally, this operation does not fail.
 *
 *  PARAMETERS
 *      NONE 
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacSetRootFolderReqSend()                  \
do{                                                     \
        CsrBtPacSetRootFolderReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacSetRootFolderReq)); \
        msg__->type = CSR_BT_PAC_SET_ROOT_FOLDER_REQ;   \
        CsrBtPacMsgTransport(msg__);                    \
}while(0)


/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullVcardListReqSend
 *
 *  DESCRIPTION
 *      Phonebook Listing Request
 *      The Phonebook Listing Request (CSR_BT_PAC_PULL_VCARD_LIST_REQ) enables the PAC to retrieve a list
 *      that represents the content of a given phone book object. The list can be customised so that it
 *      contains only the entries of interest and/or is sorted in the order of interest.
 *
 *      The Phonebook Listing Request contains relative path to target folder, filter criteria among other
 *      directives for PBAP server. In response to this request, the PBAP server sends listing output in the
 *      order specified in Phonebook Listing Request.
 *
 *      The PAC passes listing output, if received from PBAP server, to application through Phonebook Listing
 *      Indication (CSR_BT_PAC_PULL_VCARD_LIST_IND). The Application should acknowledge each Phonebook
 *      Listing indication through a Phonebook Listing Response (CSR_BT_PAC_PULL_VCARD_LIST_RES).
 *      When all listing output has been passed on to the application, the PAC ends the Phonebook Listing
 *      operation by sending Phonebook Listing Confirm (CSR_BT_PAC_PULL_VCARD_LIST_CFM) to the application.
 *      The Phonebook Listing confirmation contains application information such as phonebook size, new
 *      missed calls count and folder version besides a response code.
 *
 *      If target folder path is illegal or not supported by server, the PAC drops request and responds to
 *      the application with CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE as responseCode in Phonebook Listing confirm.
 *      All other Phonebook Listing request parameters are validated and refined before requesting from server.
 *      Thus, parameter values, which are illegal or that are not supported are masked in resulting
 *      OBEX GET requests to the server.
 *
 *
 *
 *  PARAMETERS
 *      ucs2name:           A NULL terminated 16-bit Unicode big-endian text string (UCS-2BE) containing target
 *                          folder path. Folder path is relative to current folder.
 *                          A NULL value signifies the current folder.
 *     order:               Used for indicating which sorting order shall be used for the vCard-listing.
 *                          The order is always ascendant. There are three possible sorting orders:
 *                             - Alphabetical (CSR_BT_PAC_ORDER_ALPHABETICAL),
 *                             - Indexed      (CSR_BT_PAC_SEARCH_ATT_NUMBER) or
 *                             - Phonetic     (CSR_BT_PAC_ORDER_PHONETICAL).
 *      searchVal:          Null terminated UTF-8 big-endian text string. The parameter indicates which
 *                          vCards shall be contained in the vCard-listing
 *                          Attribute described by searchAtt parameter is matched with this value.
 *                          Matching algorithm is server implementation dependant.
 *      searchAtt:          Indicate which attributes should be used when searching. There are three
 *                          possible attributes:
 *                           - Name (CSR_BT_PAC_SEARCH_ATT_NAME),
 *                           - Number (CSR_BT_PAC_SEARCH_ATT_NUMBER) or
 *                           - Sound (CSR_BT_PAC_SEARCH_ATT_SOUND).
 *      maxListCnt:         This parameter indicates the maximum number of entries of the vCard-listing.
 *                          When the application sets maxListCnt to zero it signifies that it wants to
 *                          know the actual used number of indexes in the phone book of interest (i.e.
 *                          indexes that correspond to non-null entries).
 *                          When maxListCnt is zero the PBAP server ignores all other
 *                          application parameters that may be present in the request.
 *                          The response will not contain any body header (phone book object).
 *      listStartOffset:    Indicates server the offset in requested phonebook to send vCard objects from.
 *                          The first vCard handle cannot be smaller than this value.
 *      srmpOn:             Temporarily suspends the Single Response Mode if TRUE. It is applicable only if
 *                          peer device supports SRM.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullVcardListReqSend(ucs2name, order, searchVal, searchAtt, maxListCnt, \
                                     listStartOffset, srmpOn)                           \
    CsrBtPacPullVcardListReqSendEx(ucs2name, order, searchVal, searchAtt, maxListCnt,   \
                                   listStartOffset, CSR_BT_PB_DO_NOT_RESET_NEW_MISSED_CALL, \
                                   NULL, 0, srmpOn)


/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullVcardListReqSendEx
 *
 *  DESCRIPTION
 *      Extended Phonebook Listing Request
 *      The Extended Pull Phonebook request has been introduced to accommodate new features of PBAP v1.2.
 *
 *  PARAMETERS
 *      ucs2name:           A NULL terminated 16-bit Unicode big-endian text string (UCS-2BE) containing target
 *                          folder path. Folder path is relative to current folder.
 *                          A NULL value signifies the current folder.
 *     order:               Used for indicating which sorting order shall be used for the vCard-listing.
 *                          The order is always ascendant. There are three possible sorting orders:
 *                             - Alphabetical (CSR_BT_PAC_ORDER_ALPHABETICAL),
 *                             - Indexed      (CSR_BT_PAC_SEARCH_ATT_NUMBER) or
 *                             - Phonetic     (CSR_BT_PAC_ORDER_PHONETICAL).
 *      searchVal:          Null terminated UTF-8 big-endian text string. The parameter indicates which
 *                          vCards shall be contained in the vCard-listing
 *                          Attribute described by searchAtt parameter is matched with this value.
 *                          Matching algorithm is server implementation dependant.
 *      searchAtt:          Indicate which attributes should be used when searching. There are three
 *                          possible attributes:
 *                           - Name (CSR_BT_PAC_SEARCH_ATT_NAME),
 *                           - Number (CSR_BT_PAC_SEARCH_ATT_NUMBER) or
 *                           - Sound (CSR_BT_PAC_SEARCH_ATT_SOUND).
 *      maxListCnt:         This parameter indicates the maximum number of entries of the vCard-listing.
 *                          When the application sets maxListCnt to zero it signifies that it wants to
 *                          know the actual used number of indexes in the phone book of interest (i.e.
 *                          indexes that correspond to non-null entries).
 *                          When maxListCnt is zero the PBAP server ignores all other
 *                          application parameters that may be present in the request.
 *                          The response will not contain any body header (phone book object).
 *      listStartOffset:    Indicates server the offset in requested phonebook to send vCard objects from.
 *                          The first vCard handle cannot be smaller than this value.
 *      resetNewMissedCalls:    Resets number of new missed calls in server database.
 *                              Normally this parameter is applicable only for MCH folder. However if
 *                              Enhanced Missed Calls is supported, this parameter is valid for CCH folder
 *                              as well.
 *      vCardSelector:          Bitmap of vCard properties for selecting the vCards. Only the vCards
 *                              containing these properties would be considered for response by server.
 *                              PAC drops unsupported vCard properties from this parameter before passing it
 *                              to server.
 *      vCardSelectorOperator:  This determines which logic to be used when multiple bits of the vCardSelector
 *                              application parameter are set. A bit value of 0 indicates that (OR) logic would
 *                              be used, a bit value of 1 indicates that AND logic would be used.
 *      srmpOn:                 Temporarily suspends the Single Response Mode if TRUE. It is applicable only if
 *                              peer device supports SRM.
 *---------------------------------------------------------------------------------------------------------*/
 void CsrBtPacPullVcardListReqSendEx(CsrUcs2String *ucs2name,
                                     CsrBtPacOrderType order,
                                     CsrUint8 *searchVal,
                                     CsrBtPacSearchAtt searchAtt,
                                     CsrUint16 maxListCnt,
                                     CsrUint16 listStartOffset,
                                     CsrUint8 resetNewMissedCalls,
                                     CsrUint8 *vCardSelector,
                                     CsrUint8 vCardSelectorOperator,
                                     CsrBool srmpOn);

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullVcardListResSend
 *
 *  DESCRIPTION
 *      Phonebook Listing Response
 *      The CsrBtPacPullVcardListResSend()library function acknowledges the Phonebook Listing indications.
 *      It also indicates to the PAC that application is ready to receive the next part of the listing output.
 *
 *  PARAMETERS
 *      srmpOn:         Temporarily suspends the Single Response Mode if TRUE. It is applicable only if
 *                      peer device supports SRM.
 *                      This parameter is effective only if preceding request and responses during current
 *                      Phonebook Listing operation had set srmpOn parameter.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullVcardListResSend(_srmpOn)           \
 do{                                                    \
        CsrBtPacPullVcardListRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPacPullVcardListRes)); \
        msg__->type = CSR_BT_PAC_PULL_VCARD_LIST_RES;   \
        msg__->srmpOn = _srmpOn;                        \
        CsrBtPacMsgTransport(msg__);                    \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullVcardEntryReqSend
 *
 *  DESCRIPTION
 *      Pull vCard Request
 *      The Application can use Pull vCard Request (CSR_BT_PAC_PULL_VCARD_ENTRY_REQ) to retrieve a single
 *      phone book entry from the PBAP server. The Pull vCard request specifies name of contact vCard entry,
 *      vCard format and vCard property filter.
 *
 *      The PAC passes vCard entry, if received from PBAP server, to the application through Pull vCard
 *      Indication (CSR_BT_PAC_PULL_VCARD_ENTRY_IND). The Application should acknowledge each Pull vCard
 *      indication through a Pull vCard Response (CSR_BT_PAC_PULL_VCARD_ENTRY_RES).
 *
 *      When all of vCard has been passed on to the application, the PAC ends the Contact Download operation
 *      by sending Pull vCard Confirm (CSR_BT_PAC_PULL_VCARD_ENTRY_CFM) to the application. The  Pull vCard
 *      Confirm contains database identifier besides a response code.
 *
 *      The Contacts can be referenced through UID property of contact vCards if X-BT-UID vCard Property and
 *      Referencing Contacts are supported by both PBAP Client and Server. To reference a contact, the
 *      application should format the requested object name as 'X-BT-UID:<<UID>>', where <<UID>> stands for
 *      X-BT-UID property of contact vCard.
 *      For example, if the UID of contact is A1A2A3A4B1B2C1C2D1D2E1E2E3E4E5E6, object name should be
 *      X-BT-UID:A1A2A3A4B1B2C1C2D1D2E1E2E3E4E5E6.
 *      Note: Contacts can be referenced from any folder, unlike normal Contact Download operation, which
 *      can only be requested from a PBAP contact folder like root/telecom/pb.
 *
 *
 *  PARAMETERS
 *      ucs2name:   A null terminated 16 bit Unicode big-endian text string (UCS-2BE) containing the
 *                  (file) name of the object.
 *                  A vCard entry name consists of vCard handle returned in listing output followed by
 *                  extension. For example, vCard entry file name corresponding to vCard handle "1" would
 *                  be "1.vcf".
 *                  For cross-referencing contacts, object name should be formatted like: X-BT-UID:<<UID>>.
 *      filter:     Bitmap of vCard properties to be included in phonebook objects in requested property
 *                  is available in concerned contact vCards.
 *                  Some of the properties are mandatory depending upon vCard format type (format)
 *                  requested. If none of bits are set, all supported properties are assumed to be set.
 *                  PAC drops unsupported vCard properties from this parameter before passing it to server.
 *                  Only the attributes N and TEL are mandatory to be supported by the PBAP server.
 *      format:     Indicate the requested format:
 *                          vCard 2.1 (CSR_BT_PAC_FORMAT_VCARD2_1) or
 *                          vCard 3.0 (CSR_BT_PAC_FORMAT_VCARD3:0)
 *      srmpOn:     Temporarily suspends the Single Response Mode if TRUE.
 *                  It is applicable only if peer device supports SRM.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullVcardEntryReqSend(_ucs2name, _filter, _format, _srmpOn) \
 do{                                                                        \
        CsrBtPacPullVcardEntryReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacPullVcardEntryReq)); \
        msg__->type = CSR_BT_PAC_PULL_VCARD_ENTRY_REQ;                      \
        msg__->ucs2name= _ucs2name;                                         \
        CsrMemCpy(msg__->filter, _filter, sizeof(msg__->filter));           \
        msg__->format = _format;                                            \
        msg__->srmpOn = _srmpOn;                                            \
        CsrBtPacMsgTransport(msg__);                                        \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacPullVcardEntryResSend
 *
 *  DESCRIPTION
 *      Pull Phonebook Response
 *      The CsrBtPacPullVcardEntryResSend()library function acknowledges the Pull vCard indications.
 *      It also indicates to the PAC that application is ready receive the next part of the contact vCard.
 *
 *  PARAMETERS
 *      srmpOn:         Suspends the Single Response Mode if TRUE.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacPullVcardEntryResSend(_srmpOn)              \
 do{                                                        \
        CsrBtPacPullVcardEntryRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPacPullVcardEntryRes)); \
        msg__->type   = CSR_BT_PAC_PULL_VCARD_ENTRY_RES;    \
        msg__->srmpOn = _srmpOn;                            \
        CsrBtPacMsgTransport(msg__);                        \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacAbortReqSend
 *
 *  DESCRIPTION
 *      Abort Request
 *      The Application can use Abort Request (CSR_BT_PAC_ABORT_REQ) to abort an ongoing object transfer
 *      operation. The PAC responds to this request with an Abort Confirm (CSR_BT_PAC_ABORT_CFM). In case
 *      an operation is aborted, the PAC does not confirm the operation request.
 *
 *      This API is used when the application decides to terminate a multi-packet operation (such as GET/PUT)
 *      before it normally ends.
 *      The CSR_BT_PAC_ABORT_CFM indicates that the server has received the abort response and the server
 *      is now resynchronized with the client.
 *      If the server does not respond the Abort Request or sends response with a response code different
 *      from CSR_BT_OBEX_SUCCESS_RESPONSE_CODE, the profile will disconnect the Bluetooth connection and
 *      send a disconnect indication to application.
 *
 *  PARAMETERS
 *      None
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacAbortReqSend()              \
 do{                                        \
        CsrBtPacAbortReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacAbortReq)); \
        msg__->type = CSR_BT_PAC_ABORT_REQ; \
        CsrBtPacMsgTransport(msg__);        \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacDisconnectReqSend
 *
 *  DESCRIPTION
 *      Disconnect Request
 *      The Application uses Disconnect Request (CSR_BT_PAC_DISCONNECT_REQ) to disconnect current connection.
 *      The PAC informs the application of disconnection with a Disconnect Indication (CSR_BT_PAC_DISCONNECT_IND).
 *      Either side, PBAP client or server, can trigger disconnection. It can also happen due to other reasons
 *      such as link loss.
 *
 *      Note: Operation timeout also results in disconnection. The CSR_BT_OBEX_DISCONNECT_TIMEOUT (defined in
 *      csr_bt_usr_config.h) defines timeout period for all OBEX profiles.
 *
 *  PARAMETERS
 *      theNormalDisconnect:    Whether to disconnect gracefully or immediately. Valid values are:
 *                                  TRUE - Graceful disconnect involving OBEX disconnection followed by
 *                                          transport disconnection.
 *                                  FALSE - Immediate disconnection of transport.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacDisconnectReqSend(_normalDisconnect)\
 do{                                                \
        CsrBtPacDisconnectReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacDisconnectReq)); \
        msg__->type = CSR_BT_PAC_DISCONNECT_REQ;    \
        msg__->normalDisconnect = _normalDisconnect;\
        CsrBtPacMsgTransport(msg__);                \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Cancel Connect Request
 *      The Application can cancel an on-going connection using Cancel Connect Request
 *      (CSR_BT_PAC_CANCEL_CONNECT_REQ). The PAC responds with Connect Confirm with an appropriate result
 *      code. The Cancel Connect Request should be used only during connection procedure.
 *
 *    PARAMETERS
 *      None
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacCancelConnectReqSend()                  \
 do{                                                    \
        CsrBtPacCancelConnectReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPacCancelConnectReq)); \
        msg__->type = CSR_BT_PAC_CANCEL_CONNECT_REQ;    \
        CsrBtPacMsgTransport(msg__);                    \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Security Request
 *      This API sets the default security settings for new incoming/outgoing connections.
 *      Applications that wish to change the enforcement to a specific profile security level, i.e.
 *      authentication, encryption and/or authorisation, can use this API to set up the security level for
 *      new connections. Note that this API is for the local device only and can be used from within any
 *      state.
 *      The CSR_BT_SECURITY_OUT_REQ signal sets up the security level for new outgoing connections. Already
 *      established and pending connections are not altered. Note that authorisation should not be used for
 *      outgoing connections as that may be confusing for the user - there is really no point in requesting
 *      an outgoing connection and afterwards having to authorise as they are both locally-only decided
 *      procedures.
 *      Note, that any attempts to set security to a less secure level than the mandatory security level will
 *      be rejected. See csr_bt_profiles.h for mandatory security settings. The default settings used by
 *      CSR Synergy Bluetooth are set to require authentication and encryption.
 *      Note that if MITM protection is requested and the remote device does not have the required IO
 *      capabilities, pairing/bonding will fail and connections to the remote device cannot be made.
 *
 *  PARAMETERS
 *      appHandle:      Application handle to which the confirm message is sent.
 *      secLevel:       Minimum incoming security level requested by the application. The application
 *                      must specify one of the following values:
 *                          CSR_BT_SEC_DEFAULT: Uses the default security settings.
 *                          CSR_BT_SEC_MANDATORY: Uses the mandatory security settings.
 *                          CSR_BT_SEC_SPECIFY: Specifies new security settings.
 *                      If CSR_BT_SEC_SPECIFY is set, the following values can be OR'ed additionally,
 *                      i.e., combinations of these values can be used:
 *                          CSR_BT_SEC_AUTHORISATION: Requires authorisation.
 *                          CSR_BT_SEC_AUTHENTICATION: Requires authentication.
 *                          CSR_BT_SEC_SEC_ENCRYPTION: Requires encryption (implies authentication).
 *                          CSR_BT_SEC_MITM: Requires MITM protection (implies encryption).
 *                      The PAC mandates authentication and encryption (CSR_BT_PBAP_MANDATORY_SECURITY_OUTGOING).
 *                      A default security level (CSR_BT_PBAP_DEFAULT_SECURITY_OUTGOING) is defined in
 *                      csr_bt_usr_config.h.
 *---------------------------------------------------------------------------------------------------------*/
#define CsrBtPacSecurityOutReqSend(_appHandle, _secLevel)   \
 do{                                                        \
        CsrBtPacSecurityOutReq *msg;                        \
        msg = (CsrBtPacSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtPacSecurityOutReq)); \
        msg->type = CSR_BT_PAC_SECURITY_OUT_REQ;            \
        msg->appHandle = _appHandle;                        \
        msg->secLevel = _secLevel;                          \
        CsrBtPacMsgTransport(msg);                          \
 }while(0)

/*----------------------------------------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPacFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT PAC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass:     Must be CSR_BT_PAC_PRIM,
 *      msg:            The message received from Synergy BT PAC
 *---------------------------------------------------------------------------------------------------------*/
void CsrBtPacFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif

