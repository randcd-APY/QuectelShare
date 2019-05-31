#ifndef DALSYS_H
#define DALSYS_H

/*
 * Copyright (c) 2015,2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*==================================================================================

                             EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file. Notice that
changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/dal/DALSys.h#2 $
==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "com_dtypes.h"
#include "DALSysTypes.h"
#include "string.h"

/*------------------------------------------------------------------------------
* Function declaration and documentation
*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*
  @brief Initialize/Register Module with the DALSYS library

  DAL Modules must invoke this API prior to any DALSYS usage.

  @param pCfg: DALSYS config struct pointer

  @return None
*/
void
DALSYS_InitMod(DALSYSConfig * pCfg);

/*
  @brief De-Initialize/De-Register Module with the DALSYS library

  DAL Modules can use this API once done using DALSYS.

  @param None
  @return None
*/
void
DALSYS_DeInitMod(void);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param DeviceId   : Desired Device Id
       phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                 DAL_SUCCESS
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps);

/*
  @brief Get the Property Value. User must pass a ptr to the PropVariable.
  The User must also initialize the name field, this API will populate the
  name and value. The value is a "union" and must be used depending upon
  the appropriate type.

  @param hDALProps   : DAL Propery Handle
       pszName     : Prop Name ( if searching for uint32 name, this MUST be
                                 set to NULL)
        dwId        : Prop Id   ( integer prop name)
        pDALProp    : Ptr to the Property variable

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                  uint32 dwId,
                   DALSYSPropertyVar *pDALPropVar);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DALSYS_H */
