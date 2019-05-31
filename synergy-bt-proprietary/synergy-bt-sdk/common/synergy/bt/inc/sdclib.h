#ifndef _SDCLIB_H_
#define _SDCLIB_H_

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

FILE:              sdclib.h

DESCRIPTION:       SDC access library for building SDC downstream primitives.



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
 *      sdc_service_search_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_SERVICE_SEARCH_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_service_search_req(
    CsrSchedQid phandle,          /* routing handle */
    BD_ADDR_T *p_bd_addr,       /* remote device */
    CsrUint16 size_srch_pttrn,   /* size of search pattern */
    CsrUint8 *srch_pttrn,        /* pointer to the search pattern */
    CsrUint16 max_num_recs       /* maximum records to return */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_service_attribute_req
 *
 *  DESCRIPTION
 *      Build and send a SDC_SERVICE_ATTRIBUTE_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_service_attribute_req(
    CsrSchedQid phandle,          /* routing handle */
    BD_ADDR_T *p_bd_addr,       /* remote device */
    CsrUint32 svc_rec_hndl,      /* remote service handle */
    CsrUint16 size_attr_list,    /* size of attribute list */
    CsrUint8 *attr_list,         /* pointer to the attribute list */
    CsrUint16 max_num_attr       /* maximum bytes per response */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_service_search_attribute_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_SERVICE_SEARCH_ATTRIBUTE_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_service_search_attribute_req(
    CsrSchedQid phandle,          /* routing handle */
    BD_ADDR_T *p_bd_addr,       /* remote device */
    CsrUint16 size_srch_pttrn,   /* size of search pattern */
    CsrUint8 *srch_pttrn,        /* pointer to the search pattern */
    CsrUint16 size_attr_list,    /* size of attribute list */
    CsrUint8 *attr_list,         /* pointer to the attribute list */
    CsrUint16 max_num_attr       /* maximum bytes per response */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_terminate_primitive_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_TERMINATE_PRIMITIVE_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_terminate_primitive_req(
    CsrSchedQid phandle           /* routing handle */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_open_search_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_OPEN_SEARCH_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_open_search_req(
    CsrSchedQid phandle,          /* routing handle */
    BD_ADDR_T *p_bd_addr        /* remote device */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_close_search_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_CLOSE_SEARCH_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_close_search_req(
    CsrSchedQid phandle           /* routing handle */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sdc_config_req
 *
 *  DESCRIPTION
 *      Build and send an SDC_CONFIG_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sdc_config_req(
    CsrSchedQid phandle,
    CsrUint16 mtu,
    CsrUint16 flags
    );

#ifdef __cplusplus
}
#endif


#endif

