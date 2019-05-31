/*!
  @file
  qbi_log_qxdm.c

  @brief
  QXDM-specific logging functions
*/

/*=============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
04/02/13  bd   Truncate log packets >4KB due to new diag limitation
10/08/12  hz   Add silent mode support
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_log.h"

#include "log.h"
#include "log_codes.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

#define QBI_LOG_QXDM_MAX_SIZE (4092 - sizeof(log_hdr_type))

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*=============================================================================

  Private Variables

=============================================================================*/
static boolean qbi_msg_silent_mode = FALSE;

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
)
{
  log_code_type log_code;
  uint8 *log_buf;
/*-------------------------------------------------------------------------*/
  if (len > QBI_LOG_QXDM_MAX_SIZE)
  {
    QBI_LOG_W_2("Only logging first %d bytes of %d byte packet",
                QBI_LOG_QXDM_MAX_SIZE, len);
    len = QBI_LOG_QXDM_MAX_SIZE;
  }

  log_code = (log_code_type) LOG_QBI_RX_CONTEXT_0_C + (ctx_id * 2);
  if (direction == QBI_LOG_PKT_DIRECTION_TX)
  {
    log_code++;
  }

  if ((log_code > LOG_QBI_TX_CONTEXT_7_C) || (log_code < LOG_QBI_RX_CONTEXT_0_C))
  {
    QBI_LOG_E_3("Log code 0x%04x out of range - not logging packet! Used "
                "ctx_id %d direction %d", log_code, ctx_id, direction);
  }
  else
  {
    log_buf = (uint8 *) log_alloc(log_code, sizeof(log_hdr_type) + len);
    if (log_buf != NULL)
    {
      QBI_MEMSCPY((log_buf + sizeof(log_hdr_type)), len, data, len);
      log_commit(log_buf);
    }
  }
} /* qbi_log_pkt() */

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
)
{
/*-------------------------------------------------------------------------*/
  return qbi_msg_silent_mode;
} /* qbi_log_silent_mode_get */


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
)
{
  qbi_msg_silent_mode = mode;
} /* qbi_log_silent_mode_set */

