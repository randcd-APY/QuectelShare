/****************************************************************************

Copyright (c) 2001 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifdef INSTALL_ATT_MODULE

#ifndef __ATTLIB_H__
#define __ATTLIB_H__

#include "csr_synergy.h"

#include "csr_bt_bluestack_types.h"
#include "bluetooth.h"
#include "att_prim.h"

#ifdef __cplusplus
extern "C" {
#endif


void attlib_access_rsp(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    att_result_t result,
    CsrUint16 size_value,
    CsrUint8 *value,
    ATT_UPRIM_T **pp_prim
    );

void attlib_add_db_req(
    CsrSchedQid phandle,
    CsrUint16 size_db,
    CsrUint16 *db,
    ATT_UPRIM_T **pp_prim
    );

#ifdef BUILD_FOR_HOST
void attlib_add_req(
    CsrSchedQid phandle,
    att_attr_t *attrs,
    ATT_UPRIM_T **pp_prim
    );
#endif /* BUILD_FOR_HOST */

void attlib_connect_req(
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    L2CA_CONNECTION_T connection,
    l2ca_conflags_t flags,
    ATT_UPRIM_T **pp_prim
    );

#ifdef INSTALL_ATT_BREDR
void attlib_connect_rsp(
    CsrSchedQid phandle,
    CsrUint16 cid,
    l2ca_conn_result_t  response,
    ATT_UPRIM_T **pp_prim
    );
#endif

void attlib_disconnect_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    ATT_UPRIM_T **pp_prim
    );

void attlib_exchange_mtu_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 mtu,
    ATT_UPRIM_T **pp_prim
    );

void attlib_exchange_mtu_rsp(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 server_mtu,
    ATT_UPRIM_T **pp_prim
    );

void attlib_execute_write_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 flags,
    ATT_UPRIM_T **pp_prim
    );

void attlib_find_by_type_value_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 start,
    CsrUint16 end,
    CsrUint16 uuid,
    CsrUint16 size_value,
    CsrUint8 *value,
    ATT_UPRIM_T **pp_prim
    );

void attlib_find_info_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 start,
    CsrUint16 end,
    ATT_UPRIM_T **pp_prim
    );

void attlib_handle_value_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    CsrUint16 flags,
    CsrUint16 size_value,
    CsrUint8 *value,
    ATT_UPRIM_T **pp_prim
    );

void attlib_handle_value_rsp(
    CsrSchedQid phandle,
    CsrUint16 cid,
    ATT_UPRIM_T **pp_prim
    );

void attlib_prepare_write_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    CsrUint16 offset,
    CsrUint16 size_value,
    CsrUint8 *value,
    ATT_UPRIM_T **pp_prim
    );

void attlib_read_blob_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    CsrUint16 offset,
    ATT_UPRIM_T **pp_prim
    );

void attlib_read_by_group_type_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 start,
    CsrUint16 end,
    att_uuid_type_t group_type,
    CsrUint32 *group,
    ATT_UPRIM_T **pp_prim
    );

void attlib_read_by_type_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 start,
    CsrUint16 end,
    att_uuid_type_t uuid_type,
    CsrUint32 *uuid,
    ATT_UPRIM_T **pp_prim
    );

void attlib_read_multi_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 size_handles,
    CsrUint16 *handles,
    ATT_UPRIM_T **pp_prim
    );

void attlib_read_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    ATT_UPRIM_T **pp_prim
    );

void attlib_register_req(
    CsrSchedQid phandle,
    ATT_UPRIM_T **pp_prim
    );

#ifdef BUILD_FOR_HOST
void attlib_remove_req(
    CsrSchedQid phandle,
    CsrUint16 start,
    CsrUint16 end,
    ATT_UPRIM_T **pp_prim
    );
#endif /* BUILD_FOR_HOST */

void attlib_unregister_req(
    CsrSchedQid phandle,
    ATT_UPRIM_T **pp_prim
    );

void attlib_write_req(
    CsrSchedQid phandle,
    CsrUint16 cid,
    CsrUint16 handle,
    CsrUint16 flags,
    CsrUint16 size_value,
    CsrUint8 *value,
    ATT_UPRIM_T **pp_prim
    );

void attlib_free(
    ATT_UPRIM_T *p_prim
    );

#ifdef __cplusplus
}
#endif

#endif  /* __ATTLIB_H__ */
#endif  /* INSTALL_ATT_MODULE */
