/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

/* Note: this is an auto-generated file. */

#ifndef CSR_ENV_LIB_H__
#define CSR_ENV_LIB_H__

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_util.h"
#include "csr_msg_transport.h"

#include "csr_lib.h"

#include "csr_env_prim.h"
#include "csr_env_task.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  CsrEnvFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_ENV upstream message. Does not
 *      free the message itself, and can only be used for upstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_ENV upstream message
 *----------------------------------------------------------------------------*/
void CsrEnvFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  CsrEnvFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_ENV downstream message. Does not
 *      free the message itself, and can only be used for downstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_ENV downstream message
 *----------------------------------------------------------------------------*/
void CsrEnvFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 * Enum to string functions
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * CsrPrim Type toString function.
 * Converts a message type to the String name of the Message
 *----------------------------------------------------------------------------*/
const CsrCharString *CsrEnvPrimTypeToString(CsrPrim msgType);

/*----------------------------------------------------------------------------*
 * Lookup arrays for PrimType name Strings
 *----------------------------------------------------------------------------*/
extern const CsrCharString *CsrEnvUpstreamPrimNames[CSR_ENV_PRIM_UPSTREAM_COUNT];
extern const CsrCharString *CsrEnvDownstreamPrimNames[CSR_ENV_PRIM_DOWNSTREAM_COUNT];

/*******************************************************************************

  NAME
    CsrCleanupIndSend

  DESCRIPTION

  PARAMETERS
    queue   - Destination Task Queue
    phandle -

*******************************************************************************/
#define CsrCleanupInd_struct(phandle__) (CsrCleanupInd *) CsrMessageCsrUint16_struct(CSR_ENV_PRIM, CSR_CLEANUP_IND, phandle__)
#define CsrCleanupIndSendTo(dst, phandle__) \
    { \
        CsrCleanupInd *msg__ = CsrCleanupInd_struct(phandle__); \
        CsrSchedMessagePut(dst, CSR_ENV_PRIM, msg__); \
    }
#define CsrCleanupIndSend(dst, phandle__) \
    CsrCleanupIndSendTo(dst, phandle__)


#ifdef __cplusplus
}
#endif

#endif /* CSR_ENV_LIB_H__ */
