#ifndef MCM_UIM_RESPONSE_H
#define MCM_UIM_RESPONSE_H
/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "cri_core.h"
#include "hlos_core.h"

/*===========================================================================

                            INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mcm_uim_response_cb

===========================================================================*/
/*
    @brief
    Function will be called after a async response from QMI is posted
    to the MCM server context to handle individual SIM related responses.

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_response_cb
(
  cri_core_context_type                cri_core_context,
  cri_core_error_type                  cri_core_error,
  hlos_core_hlos_request_data_type   * hlos_cb_data_ptr,
  void                               * cri_resp_data_ptr
);

#endif /* MCM_UIM_RESPONSE_H */
