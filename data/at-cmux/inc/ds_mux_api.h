#ifndef _DS_MUX_API_H_
#define _DS_MUX_API_H_

/******************************************************************************
                                   D S   M U X   T A S K

                                   H E A D E R   F I L E

  DESCRIPTION
    This is the external header file for the MUX Task. This file
    contains all the functions, definitions and data types needed for other
    tasks to interface to the MUX Task.


  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_mux_types.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
ds_mux_result_enum_type  ds_mux_passive_init
(
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_oprt_mode
(
  ds_mux_mode_enum_type    mode,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_subset
(
  ds_mux_subset_enum_type  subset,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_port_speed
(
  ds_mux_port_speed_enum_type    port_speed,
  dlci_cmux_param_type          *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_N1
(
  uint16                   frame_size,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T1
(
  uint16                   acknowledgement_timer,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_N2
(
  uint8                    num_re_transmissions,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T2
(
  uint16                   response_timer,
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_T3
(
  uint16                    wake_up_timer,
  dlci_cmux_param_type     *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_K
(
  uint8                    window_size,
  dlci_cmux_param_type    *dlci_param
);

#endif /* DS_MUX_API_H */


