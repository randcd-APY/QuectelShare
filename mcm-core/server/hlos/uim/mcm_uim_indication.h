#ifndef MCM_UIM_INDICATION_H
#define MCM_UIM_INDICATION_H
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

                            INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  mcm_uim_indication_cb

===========================================================================*/
/*
    @brief
    This function will be called after an unsolicited event indication
    is posted to the MCM server to process individual events

    @return
    None
*/
/*=========================================================================*/
void mcm_uim_indication_cb
(
  unsigned long         message_id,
  void                * ind_data_ptr,
  int                   ind_data_len
);

#endif /* MCM_UIM_INDICATION_H */
