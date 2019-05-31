/*******************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*******************************************************************************/

#ifndef _L2CAPLIB_H_
#define _L2CAPLIB_H_

#include "csr_synergy.h"

#include "csr_bt_tasks.h"
#include "csr_bt_bluestack_types.h"
#include "bluetooth.h"
#include "l2cap_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Send primitive to L2CAP */
#define L2CA_PutMsg(prim) CsrSchedMessagePut(L2CAP_IFACEQUEUE, L2CAP_PRIM, (prim))

/* Managment helpers for L2CA_CONFIG_T structures */
extern void L2CA_FreeConfigPtrs(L2CA_CONFIG_T *config);
extern void L2CA_CullConfigPtrs(L2CA_CONFIG_T *config);
extern void L2CA_MergeConfigPtrs(L2CA_CONFIG_T *target, L2CA_CONFIG_T *source);
/*lint -sem(L2CA_FreePrimitive, custodial(1)) */
extern void L2CA_FreePrimitive(L2CA_UPRIM_T *prim);

#if defined(INSTALL_STREAM_MODULE) || defined(INSTALL_ATT_MODULE)
extern CsrBool L2CA_GetBdAddrFromCid(CsrUint16 cid,
                                    TYPED_BD_ADDR_T *addrt);
#endif

#if defined(INSTALL_STREAM_MODULE) || defined(TRAPSET_BLUESTACK) || defined(INSTALL_SM_MODULE)
extern CsrBool L2CA_GetTpBdAddrFromCid(CsrUint16 cid,
                                    TP_BD_ADDR_T *tpaddrt);
#endif

#ifndef DISABLE_L2CAP_CONNECTION_FSM_SUPPORT
extern void L2CA_RegisterReq(psm_t psm,
                             CsrSchedQid phandle,
                             CsrUint16 mode_mask,
                             CsrUint16 flags,
                             CsrUint16 reg_ctx);
extern void L2CA_UnRegisterReq(psm_t psm_local,
                               CsrSchedQid phandle);
extern void L2CA_ConnectReq(const BD_ADDR_T *p_bd_addr,
                            psm_t psm_local,
                            psm_t psm_remote,
                            CsrUint16 con_ctx,
                            DM_SM_SERVICE_T *substitute_security_service);
extern void L2CA_ConnectRsp(l2ca_cid_t cid,
                            l2ca_identifier_t identifier,
                            l2ca_conn_result_t response,
                            CsrUint16 con_ctx);
extern void L2CA_ConfigReq(l2ca_cid_t cid,
                           l2ca_options_t options, l2ca_options_t hints,
                           CsrUint8 unknownLength, CsrUint8 *unknown,
                           l2ca_mtu_t mtu, l2ca_timeout_t flush_to,
                           L2CA_QOS_T *qos,
                           L2CA_FLOW_T *flow,
                           l2ca_fcs_t fcs,
                           L2CA_FLOWSPEC_T *flowspec,
                           l2ca_window_size_t ext_window);
extern void L2CA_ConfigReqCs(l2ca_cid_t cid,
                             L2CA_CONFIG_T *config);
extern void L2CA_ConfigRsp(l2ca_cid_t cid,
                           l2ca_identifier_t identifier,
                           l2ca_conf_result_t response,
                           l2ca_options_t options,
                           l2ca_options_t hints,
                           CsrUint8 unknownLength,
                           CsrUint8 *unknown,
                           l2ca_mtu_t mtu,
                           l2ca_timeout_t flush_to,
                           L2CA_QOS_T *qos,
                           L2CA_FLOW_T *flow,
                           l2ca_fcs_t fcs,
                           L2CA_FLOWSPEC_T *flowspec,
                           l2ca_window_size_t ext_window);
extern void L2CA_ConfigRspCs(l2ca_cid_t cid,
                             l2ca_identifier_t identifier,
                             l2ca_conf_result_t response,
                             L2CA_CONFIG_T *config);
extern void L2CA_MulticastReq(l2ca_cid_t *cids, CsrUint16 length, void *p_data);
extern void L2CA_DisconnectReq(l2ca_cid_t cid);
extern void L2CA_DisconnectCtxReq(CsrUint16 con_ctx);
extern void L2CA_DisconnectRsp(l2ca_identifier_t identifier, l2ca_cid_t cid);
extern void L2CA_PingReq(const BD_ADDR_T *p_bd_addr, CsrSchedQid phandle, void *p_data,
                         CsrUint16 length, CsrUint16 req_ctx, l2ca_conflags_t flags);
extern void L2CA_GetInfoReq(const BD_ADDR_T *p_bd_addr, CsrSchedQid phandle,
                            CsrUint16 info_type,CsrUint16 req_ctx, l2ca_conflags_t flags);
/*lint -sem(L2CA_AutoConnectReq, custodial(9)) */
extern void L2CA_AutoConnectReq(l2ca_cid_t           cid,
                                psm_t                psm_local,
                                const BD_ADDR_T     *p_bd_addr,
                                psm_t                psm,
                                CsrUint16             con_ctx,
                                l2ca_controller_t    remote_control,
                                l2ca_controller_t    local_control,
                                CsrUint16             conftab_length,
                                CsrUint16            *conftab);
/*lint -sem(L2CA_AutoConnectRsp, custodial(6)) */
extern void L2CA_AutoConnectRsp(l2ca_identifier_t    identifier,
                                l2ca_cid_t           cid,
                                l2ca_conn_result_t   response,
                                CsrUint16             con_ctx,
                                CsrUint16             conftab_length,
                                CsrUint16            *conftab);

extern void L2CA_GetChannelInfoReq(l2ca_cid_t cid);
#endif

extern void L2CA_DataWriteReqEx(l2ca_cid_t cid, CsrUint16 length, void *p_data, CsrUint16 context);
#define L2CA_DataWriteReq(cid, length, p_data) (L2CA_DataWriteReqEx((cid), (length), (p_data), 0))
extern void L2CA_DataReadRsp(l2ca_cid_t cid, CsrUint16 packets);
extern void L2CA_RawDataReq(l2ca_cid_t cid, CsrUint16 length, void *p_data, CsrUint16 raw_length, CsrUint16 flush_to);
extern void L2CA_ExRawModeReq(l2ca_cid_t cid, l2ca_raw_t mode, CsrSchedQid phandle);
extern void L2CA_RawModeReq(l2ca_cid_t cid,CsrBool cid_rx, CsrBool acl_rx, CsrSchedQid phandle);
extern void L2CA_DataWriteAbortReq(l2ca_cid_t cid);

#ifdef INSTALL_L2CAP_ENHANCED_SUPPORT
extern void L2CA_BusyReq(l2ca_cid_t cid, CsrBool busy);
#endif /* ENHANCED_SUPPORT */

#ifdef INSTALL_L2CAP_FIXED_CHANNEL_BASE_SUPPORT
#ifdef INSTALL_L2CAP_FIXED_CHANNEL_SUPPORT
extern void L2CA_RegisterFixedCidReq(CsrSchedQid phandle,
                                     l2ca_cid_t fixed_cid,
                                     L2CA_CONFIG_T *config,
                                     CsrUint16 reg_ctx);
extern void L2CA_MapFixedCidReq(const TYPED_BD_ADDR_T *addrt,
                                l2ca_cid_t fixed_cid,
                                CsrUint16 con_ctx,
                                l2ca_conflags_t flags);
extern void L2CA_MapFixedCidRsp(l2ca_cid_t cid,
                                CsrUint16 con_ctx,
                                l2ca_conflags_t flags);
#endif

extern void L2CA_UnmapFixedCidReq(l2ca_cid_t cid);

#if defined(INSTALL_L2CAP_CONNLESS_SUPPORT) || defined(INSTALL_L2CAP_UCD_SUPPORT)
extern void L2CA_MapConnectionlessCidReq(const BD_ADDR_T *p_bd_addr,
                                         psm_t cl_local_psm,
                                         psm_t cl_remote_psm,
                                         CsrUint16 con_ctx,
                                         l2ca_conflags_t flags);

extern void L2CA_MapConnectionlessCidRsp(l2ca_cid_t cid,
                                         CsrUint16 con_ctx,
                                         psm_t ucd_remote_psm,
                                         l2ca_conflags_t flags);
#endif /* INSTALL_L2CAP_CONNLESS_SUPPORT || INSTALL_L2CAP_UCD_SUPPORT */
#endif /* FIXED_CHANNEL_SUPPORT */

/* BR/EDR may also support flowspecs */
#ifdef INSTALL_L2CAP_FLOWSPEC_SUPPORT
extern L2CA_FLOWSPEC_T *L2CA_AllocFlowspec(void);
#endif

#ifdef INSTALL_L2CAP_QOS_SUPPORT
extern L2CA_QOS_T *L2CA_AllocQoS(void);
#endif

#ifdef INSTALL_AMP_SUPPORT
extern void L2CA_CreateChannelReq(psm_t               psm_local,
                                  const BD_ADDR_T    *bd_addr,
                                  psm_t               psm,
                                  CsrUint16            con_ctx,
                                  l2ca_controller_t   remote_control,
                                  l2ca_controller_t   local_control,
                                  DM_SM_SERVICE_T *substitute_security_service);
extern void L2CA_CreateChannelRsp(l2ca_identifier_t   identifier,
                                  l2ca_cid_t          cid,
                                  l2ca_conn_result_t  response,
                                  CsrUint16            con_ctx);
extern void L2CA_MoveChannelReq(l2ca_cid_t          cid,
                                l2ca_controller_t   remote_control,
                                l2ca_controller_t   local_control);
extern void L2CA_MoveChannelRsp(l2ca_identifier_t   identifier,
                                l2ca_cid_t          cid,
                                l2ca_move_result_t  response);
#endif /* AMP_SUPPORT */


#ifdef __cplusplus
}
#endif
#endif /* _L2CAPLIB_H_ */
