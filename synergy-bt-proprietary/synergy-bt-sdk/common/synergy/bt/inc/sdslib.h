#ifndef _SDSLIB_H_
#define _SDSLIB_H_

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

FILE:              sdslib.h

DESCRIPTION:       SDS access library for building SDS downstream primitives.



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
 *      sds_register_req
 *
 *  DESCRIPTION
 *      Build and send an SDS_REGISTER_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sds_register_req(
    CsrSchedQid phandle,      /* routing handle */
    CsrUint8 *service_rec,   /* Pointer to service record data */
    CsrUint16 num_rec_bytes  /* Number of bytes in the service record data  */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sds_unregister_req
 *
 *  DESCRIPTION
 *      Build and send an SDS_UNREGISTER_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sds_unregister_req(
    CsrSchedQid phandle,      /* routing handle */
    CsrUint32 svc_rec_hndl   /* service record handle */
    );

/*----------------------------------------------------------------------------*
 *  NAME
 *      sds_config_req
 *
 *  DESCRIPTION
 *      Build and send an SDS_CONFIG_REQ primitive to SDP.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/

void sds_config_req(
    CsrSchedQid phandle,
    CsrUint16 mtu,
    CsrUint16 flags
    );

#ifdef __cplusplus
}
#endif


#endif

