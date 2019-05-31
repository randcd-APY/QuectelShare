/*******************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

DESCRIPTION:    Helper functions for freeing BlueStack primitives.
*******************************************************************************/

#ifndef _BLUESTACK_LIB_H
#define _BLUESTACK_LIB_H

#include "csr_synergy.h"

#include "hci_prim.h"
#include "l2cap_prim.h"
#include "l2caplib.h"
#include "rfcomm_prim.h"
#include "sds_prim.h"
#include "sdslib.h"
#include "dm_prim.h"
#include "dmlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      dm_free_primitive
 *
 *  DESCRIPTION
 *      Free downstream and upstream DM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

extern void dm_free_primitive(
    DM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      dm_free_upstream_primitive
 *
 *  DESCRIPTION
 *      Free upstream DM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

extern void dm_free_upstream_primitive(
    DM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      dm_free_downstream_primitive
 *
 *  DESCRIPTION
 *      Free downstream DM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void dm_free_downstream_primitive(
    DM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      dm_acl_free_downstream_primitive
 *
 *  DESCRIPTION
 *      Free downstream DM ACL primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void dm_acl_free_downstream_primitive(
    DM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdp_free_primitive
 *
 *  DESCRIPTION
 *      Free downstream and upstream SDP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdp_free_primitive(
    SDS_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdp_free_upstream_primitive
 *
 *  DESCRIPTION
 *      Free upstream SDP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdp_free_upstream_primitive(
    SDS_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdp_free_downstream_primitive
 *
 *  DESCRIPTION
 *      Free downstream SDP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdp_free_downstream_primitive(
    SDS_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      rfc_free_primitive
 *
 *  DESCRIPTION
 *      Free downstream and upstream RFCOMM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void rfc_free_primitive(
    RFCOMM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      rfc_free_upstream_primitive
 *
 *  DESCRIPTION
 *      Free upstream RFCOMM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void rfc_free_upstream_primitive(
    RFCOMM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      rfc_free_downstream_primitive
 *
 *  DESCRIPTION
 *      Free downstream RFCOMM primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void rfc_free_downstream_primitive(
    RFCOMM_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      l2cap_free_upstream_primitive
 *
 *  DESCRIPTION
 *      Free upstream L2CAP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void l2cap_free_upstream_primitive(
    L2CA_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      l2cap_free_downstream_primitive
 *
 *  DESCRIPTION
 *      Free downstream L2CAP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void l2cap_free_downstream_primitive(
    L2CA_UPRIM_T *p_uprim
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      l2cap_free_primitive
 *
 *  DESCRIPTION
 *      Free downstream and upstream L2CAP primitives, including any referenced memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void l2cap_free_primitive(
    L2CA_UPRIM_T *p_uprim
    );

#ifdef __cplusplus
}
#endif

#endif
