#ifndef DS_MUX_IO_H
#define DS_MUX_IO_H
/*===========================================================================

                                 D S   M U X   I O

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX IO. This file
  contains all the functions, definitions and data types needed
  for MUX input - output processing.

Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/


#include "ds_mux_types.h"

#define INFO_LENGTH_FIELD 1024



typedef enum
{
  DS_MUX_IO_FRAME_INIT     = 0,
  DS_MUX_IO_FRAME_START_PARSED,
  DS_MUX_IO_FRAME_ADDR_PARSED,
  DS_MUX_IO_FRAME_CTRL_PARSED,
  DS_MUX_IO_FRAME_LEN_PARSED,
  DS_MUX_IO_FRAME_INFO_PARSED,
  DS_MUX_IO_FRAME_FCS_PARSED,
  DS_MUX_IO_FRAME_STOP_PARSED
}ds_mux_io_parse_frame_state_enum_type;

/**
 *  Enum to identify the Frame Type
 **/
typedef enum
{
  DS_MUX_FRAME_TYPE_INVALID_FRAME =0,
  DS_MUX_FRAME_TYPE_UTIL_SABM,
  DS_MUX_FRAME_TYPE_UTIL_UA,
  DS_MUX_FRAME_TYPE_UTIL_DM,
  DS_MUX_FRAME_TYPE_UTIL_DISC,
  DS_MUX_FRAME_TYPE_UTIL_UIH,
  DS_MUX_FRAME_TYPE_UTIL_UI,
  DS_MUX_FRAME_TYPE_MAX
} ds_mux_frame_type_enum_type;


/**
 *  A mux frame will be identified with this structure
 **/
typedef struct
{
  ds_mux_frame_type_enum_type frame_type;
  uint8                       dlci;
  uint8                       control_field;
  boolean                     command_response;
  boolean                     poll_final;
  uint16                      length_wt_ea;
  uint16                      length;
  char*                       information_ptr;
  uint8                       fcs;
} ds_mux_io_frame_type;



typedef struct
{

  ds_mux_io_parse_frame_state_enum_type curr_frame_state;
  /* currnet frame state */

  ds_mux_io_frame_type* rx_frame;  /* pointer to frame*/

}ds_mux_io_rx_info;

#endif
