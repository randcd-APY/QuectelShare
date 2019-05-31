#ifndef CSR_BT_SPP_COMMON_H__
#define CSR_BT_SPP_COMMON_H__

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CSR_BT_INSTALL_SPP_EXTENDED

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSppMapSecurityInLevel
 *
 *  DESCRIPTION
 *      The Application uses this function to convert the SC incoming security 
 *      level to bluestack DM incoming security level.
 *
 *  PARAMETERS
 *      secInputLevel:  SC incoming security level.
 *
 *  RETURN
 *      Returns the DM incoming security level.
 *----------------------------------------------------------------------------*/
dm_security_level_t CsrBtSppMapSecurityInLevel(CsrUint16 secInputLevel);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSppMapSecurityOutLevel
 *
 *  DESCRIPTION
 *      The Application uses this function to convert the SC outgoing security 
 *      level to bluestack DM outgoing security level.
 *
 *  PARAMETERS
 *      secOutputLevel:  SC outgoing security level.
 *
 *  RETURN
 *      Returns the DM outgoing security level.
 *----------------------------------------------------------------------------*/
dm_security_level_t CsrBtSppMapSecurityOutLevel(CsrUint16 secOutputLevel);

#endif /* CSR_BT_INSTALL_SPP_EXTENDED */

#ifdef __cplusplus
}
#endif

#endif
