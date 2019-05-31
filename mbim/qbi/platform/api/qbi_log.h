/*!
  @file
  qbi_log.h

  @brief
  Brings in the appropriate platform-specific header file containing log macro
  definitions.
*/

/*=============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
10/08/12  hz   Add silent mode support
02/10/12  bd   Add support for Linux
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_LOG_H
#define QBI_LOG_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_os.h"

#ifdef _WIN32
  #include "qbi_log_wpp.h"
#else
  #include "qbi_log_qxdm.h"
#endif

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

typedef enum
{
  QBI_LOG_PKT_DIRECTION_RX,
  QBI_LOG_PKT_DIRECTION_TX
} qbi_log_pkt_direction_e;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_log_pkt
===========================================================================*/
/*!
    @brief Logs a MBIM message packet that is either being sent to the host
    (TX) or has been received from the host (RX)

    @details

    @param data
    @param len
*/
/*=========================================================================*/
void qbi_log_pkt
(
  uint32                  ctx_id,
  qbi_log_pkt_direction_e direction,
  const void             *data,
  uint32                  len
);

/*===========================================================================
  FUNCTION: qbi_log_silent_mode_get
===========================================================================*/
/*!
    @brief Check if silent mode is on

    @details

    @return boolean TRUE if silent mode is on, FALSE if otherwise
*/
/*=========================================================================*/
boolean qbi_log_silent_mode_get
(
  void
);

/*===========================================================================
  FUNCTION: qbi_log_silent_mode_set
===========================================================================*/
/*!
    @brief Set silent mode on or off

    @details

    @param boolean
*/
/*=========================================================================*/
void qbi_log_silent_mode_set
(
  boolean mode
);

#endif /* QBI_LOG_H */

