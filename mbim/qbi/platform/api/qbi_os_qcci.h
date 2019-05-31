/*!
  @file
  qbi_os_qcci.h

  @brief
  This file provides abstraction for operating system and platform specific
  QCCI-related operations.
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
02/11/13  bd   Add cross-platform support for QCCI setup, teardown, instance
=============================================================================*/

#ifndef QBI_OS_QCCI_H
#define QBI_OS_QCCI_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_os.h"

#include "qmi_client.h"
#include "qmi_cci_target_ext.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_os_qcci_os_params_init
===========================================================================*/
/*!
    @brief Initializes OS-specific parameters for use in QCCI synchronous
    APIs such as qmi_client_init()

    @details

    @param os_params
*/
/*=========================================================================*/
void qbi_os_qcci_os_params_init
(
  qmi_client_os_params *os_params
);

/*===========================================================================
  FUNCTION: qbi_os_qcci_get_service_instance
===========================================================================*/
/*!
    @brief Retrieves the appropriate service instance value to use with
    qmi_client_get_service_instance()

    @details

    @param ctx

    @return qmi_service_instance
*/
/*=========================================================================*/
qmi_service_instance qbi_os_qcci_get_service_instance
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_os_qcci_setup
===========================================================================*/
/*!
    @brief Performs any OS/platform specific setup required before the QCCI
    APIs can be used

    @details
    To be called as part of MBIM_OPEN processing before any QCCI APIs are
    called.

    @param ctx

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_qcci_teardown
*/
/*=========================================================================*/
boolean qbi_os_qcci_setup
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_os_qcci_teardown
===========================================================================*/
/*!
    @brief Performs OS/platform specific teardown and resource cleanup

    @details
    To be called as part of MBIM_CLOSE processing, at a point where QCCI APIs
    will not be called until after qbi_os_qcci_setup() is called again.

    @param ctx

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_qcci_setup
*/
/*=========================================================================*/
boolean qbi_os_qcci_teardown
(
  const qbi_ctx_s *ctx
);

#endif /* QBI_OS_QCCI_H */

