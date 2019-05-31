#ifndef CSR_BT_RFC_PROXY_LIB_H__
#define CSR_BT_RFC_PROXY_LIB_H__

#include "csr_synergy.h"

/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
    Public Defines
 *============================================================================*/
/* None */

/*============================================================================*
    Public Data Types
 *============================================================================*/
/* None */

/*============================================================================*
    Public Data
 *============================================================================*/
/* None */

/*============================================================================*
    Public Functions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcInitReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_INIT_REQ primitive to RFCOMM, not overriding the
 *      default parameters.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcInitReqSend( CsrSchedQid phandle );

/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcRegisterReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_REGISTER_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcRegisterReqSend(
    CsrSchedQid phandle,
    CsrUint8   flags,
    CsrUint16  context,
    CsrUint8   loc_serv_chan_req
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcUnregisterReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_UNREGISTER_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcUnregisterReqSend(CsrUint8 loc_serv_chan);


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcClientConnectReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_CLIENT_CONNECT_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcClientConnectReqSend(
    CsrSchedQid   phandle,
    BD_ADDR_T   *p_bd_addr,
    CsrUint8     rem_serv_chan,
    CsrUint8     flags,
    CsrUint16    context,
    CsrUint16    client_security_chan,
    CsrUint16    max_payload_size,
    CsrUint8     priority,
    CsrUint16    total_credits,
    l2ca_controller_t   remote_l2cap_control,
    l2ca_controller_t   local_l2cap_control,
    CsrUint16    p_reserved_length,
    CsrUint16    *p_reserved,
    CsrUint8     modem_signal,
    CsrUint8     break_signal,
    CsrUint16    msc_timeout);


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcServerConnectResSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_SERVER_CONNECT_RES primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcServerConnectResSend(
        CsrUint8     flags,
        CsrUint16    conn_id,
        RFC_RESPONSE_T   response,
        CsrUint16    max_payload_size,
        CsrUint8     priority,
        CsrUint16    total_credits,
        l2ca_controller_t   remote_l2cap_control,
        l2ca_controller_t   local_l2cap_control,
        CsrUint8     modem_signal,
        CsrUint8     break_signal,
        CsrUint16    msc_timeout);

/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcDisconnectReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_DISCONNECT_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcDisconnectReqSend(
    CsrUint16 connId
    );


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcPortnegReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_PORTNEG_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcPortnegReqSend(
    CsrUint16 connId,
    CsrBool request,
    RFC_PORTNEG_VALUES_T *p_port_pars     /* Can be NULL */
    );


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcPortnegResSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_PORTNEG_RES primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcPortnegResSend(
    CsrUint16 connId,
    RFC_PORTNEG_VALUES_T *p_port_pars     /* Must be supplied */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcModemStatusReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_MODEM_STATUS_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcModemStatusReqSend(
    CsrUint16 connId,
    CsrUint8 modem_signal,
    CsrUint8 break_signal
    );


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcFcReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_FCON_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcFcReqSend(
    BD_ADDR_T *p_bd_addr,
    RFC_FC_T  fc
    );


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcDatawriteReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_DATAWRITE_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void RfcDatawriteReqSend(
    CsrUint16 connId,
    CsrUint16 rx_credits,
    CsrUint16 payload_length,
    CsrUint8 *payload
    );


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcDatareadRspSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_DATAREAD_RSP primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcDatareadRspSend(CsrUint16 conn_id);



/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcLinestatusReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_LINE_STATUS_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcLinestatusReqSend(CsrUint16 conn_id,
                        CsrUint8 error_flag,
                        CsrUint8 line_status);


/*----------------------------------------------------------------------------*
 *  NAME
 *      RfcTestReqSend
 *
 *  DESCRIPTION
 *      Build and send an RFC_TEST_REQ primitive to RFCOMM.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void RfcTestReqSend(BD_ADDR_T  *p_bd_addr,
                  CsrUint16 test_data_length,
                  CsrUint8 *test_data);

#ifdef __cplusplus
}
#endif

#endif /* ndef _RFC_PROXY_LIB_H */
