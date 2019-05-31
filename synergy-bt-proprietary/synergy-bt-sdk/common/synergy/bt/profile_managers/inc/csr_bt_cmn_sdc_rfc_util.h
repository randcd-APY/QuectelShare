#ifndef CSR_BT_CMN_SDC_RFC_UTIL_H__
#define CSR_BT_CMN_SDC_RFC_UTIL_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_result.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_cm_private_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_cmn_sdr_tagbased_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines the states                                                           */
#define CMN_SDC_RFC_IDLE_STATE                          0x00
#define CMN_SDC_RFC_SEARCH_STATE                        0x01
#define CMN_SDC_RFC_ATTR_STATE                          0x02
#define CMN_SDC_RFC_CLOSE_SEARCH_STATE                  0x03
#define CMN_SDC_RFC_CANCEL_STATE                        0x04
#define CMN_SDC_RFC_REGISTER_STATE                      0x05
#define CMN_SDC_RFC_SELECT_SERVICE_RECORD_HANDLE_STATE  0x06
#define CMN_SDC_RFC_CONNECT_STATE                       0x07
#define CMN_SDC_RFC_PORTNEG_PENDING_STATE               0x08

/* Defines the maximum number of different uuid that can be requested in one
   request                                                                      */
#define CMN_SDC_MAX_NUM_OF_UUID                     0xff


/* Defines the called back function and its arguments which is called when the
   Cm Rfc Connect procedure demands the profile to select a service handle or
   a list of service handles. When this function is called the profile must
   either called the function CsrBtUtilRfcConSetServiceHandleIndexList to select
   one a more service handle(s), or  CsrBtUtilRfcConCancel to cancel the procedure.
   Please note the sdpTagList pointer is own by the cmn sdc rfc function and
must therefore NOT be CsrPmemFree                                                     */
typedef void(*CmnRfcConSelectServiceHandleFuncType)(void                    * instData,
                                                    void                    * cmSdcRfcInstData,
                                                    CsrBtDeviceAddr            deviceAddr,
                                                    CsrUint8           serverChannel,
                                                    CsrUint16                entriesInSdpTaglist,
                                                    CmnCsrBtLinkedListStruct * sdpTagList);

/* Defines the called back function and its arguments which is called when the
   Cm Rfc connect procedure demands the profile to respond to a CSR_BT_CM_PORTNEG_IND
   message. When this function is called the profile must either called the
   function CsrBtUtilRfcConSetPortPar, or CsrBtUtilRfcConCancel to cancel the procedure.
   Please note the cmSdcRfcInstData pointer is own by the cmn sdc rfc function
   and must therefore NOT be CsrPmemFree                                              */
typedef void(*CsrBtUtilRfcConSetPortParFuncType)(void                     * instData,
                                                 CsrBtDeviceAddr          deviceAddr,
                                                 CsrUint8            serverChannel,
                                                 RFC_PORTNEG_VALUES_T     portPar,
                                                 CsrBool                   request);

/* Defines the called back function and its arguments which is called when the
   Cm Rfc connect procedure is finish. Please note that the sdpTag pointer
   is NOT own by the cmn sdc rfc function and MUST therefore be CsrPmemFree           */
typedef void (*CmnRfcConResultFuncType)(void                        * instData,
                                        CsrUint8               localServerCh,
                                        CsrUint32                    btConnId,
                                        CsrBtDeviceAddr                deviceAddr,
                                        CsrUint16                    maxFrameSize,
                                        CsrBool                      validPortPar,
                                        RFC_PORTNEG_VALUES_T        portPar,
                                        CsrBtResultCode             resultCode,
                                        CsrBtSupplier          resultSupplier,
                                        CmnCsrBtLinkedListStruct     * sdpTag);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtUtilRfcConStart
 *
 *  DESCRIPTION
 *      This function will start a CM RFC SEARCH and RFCOMM connect operation
 *
 *    PARAMETERS
 *      *instData               : The overloaded void pointer to the
 *                                profiles own instance data
 *
 *      *cmSdcRfcInstData       : The overloaded void pointer to this
 *                                library private instance data
 *
 *      *sdpTag                 : The info that the profile must read from the
 *                                peer device service record
 *
 *      deviceAddr              : The Bluetooth address of the device to
 *                                connect to
 *
 *      secLevel                : Sets up the security level for new outgoing
 *                                connection
 *
 *      requestPortPar          : If TRUE, this command is a request for the remote
 *                                device to report its current port parameter values.
 *                                Please note that this parameter is only valid if
 *                                the *portPar is not NULL
 *
 *      *portPar                : If *portPar is not NULL it is requested to set port
 *                                parameters doing the connect procedure. If *portPar
 *                                is set to NULL the value of requestPortPar is not
 *                                important. Please note the normally this parameter
 *                                is set to NULL.
 *
 *      mtu                     : Specify MTU size for the RFCOMM connection
 *
 *      modemStatus             : Modem signal values
 *
 *      mscTimeout              : Time in msec that the RFC shall wait for MSC at connection
 *                                time.
 *----------------------------------------------------------------------------*/
extern CsrBool CsrBtUtilRfcConStart(void                    * instData,
                                      void                    * cmSdcRfcInstData,
                                      CmnCsrBtLinkedListStruct * sdpTag,
                                      CsrBtDeviceAddr            deviceAddr,
                                      dm_security_level_t     secLevel,
                                      CsrBool                  requestPortPar,
                                      RFC_PORTNEG_VALUES_T     * portPar,
                                      CsrUint16                mtu,
                                      CsrUint8                 modemStatus,
                                      CsrUint8                 mscTimeout);



/* Function to cancel the CM RFC connect procedure search                       */
extern CsrBool CsrBtUtilRfcConCancel(void *instData, void *cmSdcRfcInstData);


/* Function to select one or more service record handle(s) doing the CM RFC
   connect procedure. Please note that in the case that nofServiceHandleIndicis
   > 1 then will this library try to connect to serviceHandleIndexList[0] first,
   if this fails then serviceHandleIndexList[1] and so on. In this way the
   profile gives priority of which service recod handle it perfers              */
extern CsrBool CsrBtUtilRfcConSetServiceHandleIndexList(void           * instData,
                                                       void           * cmSdcRfcInstData,
                                                       CsrUint16       * serviceHandleIndexList,
                                                       CsrUint16       nofServiceHandleIndicis);

#ifdef CSR_BT_INSTALL_SDC_SET_PORT_PAR
/* Function to respond a portneg ind msg received doing the CM RFC connect
   procedure                                                                    */
extern CsrBool CsrBtUtilRfcConSetPortPar(void * cmSdcRfcInstData, RFC_PORTNEG_VALUES_T portPar);
#endif

/* Function to register the callback functions that is needed to used the Cm Rfc
   connect procedure. Note by setting the callback function of type
   CmnRfcConSelectServiceHandleFuncType to NULL the service record handle will
   be auto selected by the CM RFC connect procedure. Also note that by setting
   the callback function of type CsrBtUtilRfcConSetPortParFuncType to NULL the portPar
   will be automatic return with default parameters by the CM RFC connect
   procedure                                                                    */
extern void * CsrBtUtilSdpRfcConInit(CmnRfcConSelectServiceHandleFuncType selectServiceHandler,
                                     CsrBtUtilRfcConSetPortParFuncType    setPortParHandler,
                                     CmnRfcConResultFuncType              rfcConResultHandler,
                                     CsrSchedQid                               appHandle);

/* Function to does the same thing as CsrBtUtilSdpRfcConInit. The different part
   is that the instIdentifier is return in the CM upstream messages. Note this
   is an CsrUint8 because in the CM_SDC_XXX message the instIdentifier is the
   local server channel                                                         */
extern void * CsrBtUtilSdpRfcInit(CmnRfcConSelectServiceHandleFuncType selectServiceHandler,
                                  CsrBtUtilRfcConSetPortParFuncType    setPortParHandler,
                                  CmnRfcConResultFuncType              rfcConResultHandler,
                                  CsrSchedQid                               appHandle,
                                  CsrUint8                             instIdentifier);


/* Function to handle upstream CM messages, in the group
   NUM_OF_CM_BASIC_PLUS_SDC_SEND_PRIMS, doing the Cm Rfc Connect procedure      */
extern CsrBool CsrBtUtilRfcConCmMsgHandler(void * instData,
                                          void * cmSdcRfcInstData,
                                          void * msg);


/* Function to check if the CsrBtUtilRfcConCmMsgHandler is able to handle the
   incoming CM message. This function will return TRUE if it can handle the
   message otherwise FALSE                                                      */
extern CsrBool CsrBtUtilRfcConVerifyCmMsg(void *msg);


/* Defines the called back function and its arguments which is called when the
   Cm Sdp search procedure is finish. Please note that the sdpTagList pointer
   is NOT own by the cmn sdc rfc function and MUST therefore be CsrPmemFree           */
typedef void (*CmnSdcResultFuncType)(void                     * instData,
                                     CmnCsrBtLinkedListStruct * sdpTagList,
                                     CsrBtDeviceAddr          deviceAddr,
                                     CsrBtResultCode          resultCode,
                                     CsrBtSupplier      resultSupplier);


/* Function to register the callback function that is needed to used the Cm SDP
   search procedure.                                                            */
extern void * CsrBtUtilSdcInit(CmnSdcResultFuncType sdcResultHandler,
                               CsrSchedQid                  appHandle);

/* Function to deregister the callback functions again. Note that this
   function must only be called when the Cm SDP search
   procedure is idle                                                            */
extern void CsrBtUtilSdcRfcDeinit(void ** cmSdcRfcInstData);

/* Function to start the Cm SDP search procedure                                */
extern CsrBool CsrBtUtilSdcSearchStart(void               * instData,
                                void                     * cmSdcRfcInstData,
                                CmnCsrBtLinkedListStruct * sdpTag,
                                CsrBtDeviceAddr            deviceAddr);

/* Function to cancel a SDP search                                              */
extern CsrBool CsrBtUtilSdcSearchCancel(void *instData, void *cmSdcRfcInstData);

/* Function to handle upstream CM messages, in the group
   NUM_OF_CM_PROFILE_SDC_SEND_PRIMS, doing the Cm SDP search                    */
extern CsrBool CsrBtUtilSdcCmMsgHandler(void * instData,
                                       void * cmSdcRfcInstData,
                                       void * msg);


/* Function to check if the CsrBtUtilSdcCmMsgHandler is able to handle the
   incoming CM message. This function will return TRUE if it can handle the
   message otherwise FALSE                                                      */
extern CsrBool CsrBtUtilSdcVerifyCmMsg(void *msg);

/* Instance data used intern by cmn_sdp_connect_util functions                  */
typedef struct
{
    CsrUint8                 state;
    CsrBtDeviceAddr          deviceAddr;
    CsrUintFast16            numOfSdrOutEntries;
    CsrUintFast16            numOfSdrAttr;
    CsrUintFast16            sdrEntryIndex;
    CsrUintFast16            sdrAttrIndex;
    CsrUint16                uuidType;
    CsrUint16                maxFrameSize;
    CsrBtUuid32              serviceHandle;
    dm_security_level_t      secLevel;
    CsrUint8                 localServerCh;
    CsrUint32                btConnId;
    CsrBool                  reqPortPar;
    CsrBool                  validPortPar;
    CsrBool                  obtainServer;
    CsrBool                  reqControl;
    RFC_PORTNEG_VALUES_T     portPar;
    CsrUintFast16            numOfServiceHandleIndicis;
    CsrUint16                * serviceHandleIndexList;
    CmnCsrBtLinkedListStruct * sdpInTagList;
    CmnCsrBtLinkedListStruct * sdpOutTagList;
    CsrUint8                 modemStatus;
    CsrUint8                 breakSignal;
    CsrUint8                 scTimeout;
} CmnSdcRfcPriInstType;

typedef struct
{
    CsrSchedQid                                  appHandle;
    CmnSdcResultFuncType                    searchResultFunc;
    CmnRfcConSelectServiceHandleFuncType    selectServiceHandleFunc;
    CsrBtUtilRfcConSetPortParFuncType       setPortParFunc;
    CmnRfcConResultFuncType                 connectResultFunc;
    CsrUint8                                instId;
    CmnSdcRfcPriInstType                    * privateInst;
} CmnSdcRfcInstType;

typedef CsrBool (*CmnSdcRfcType)(void *instData, CmnSdcRfcInstType *inst,
                                CmnSdcRfcPriInstType *priInst, void * msg);


#ifdef __cplusplus
}
#endif

#endif /*_CMN_SDP_RFC_UTIL_H */

