/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       D S   M U X   T A S K

  GENERAL DESCRIPTION
    This file contains all the functions, definitions and data types needed
    for other tasks to interface to the MUX Task.

    Command definitions for the MUX Task, and functions
    that should be used by other tasks to send commands to the MUX
    Task are defined here.

Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/10/17   sm     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_types.h"
#include "ds_mux_main.h"

extern ds_mux_state_info_type       ds_mux_state_info;


void ds_mux_set_mux_state
(
  ds_mux_state_enum_type  state
)
{
    ds_mux_state_info.ds_mux_state  = state;
}/*ds_mux_set_mux_state*/

ds_mux_result_enum_type  ds_mux_set_oprt_mode
(
  ds_mux_mode_enum_type    mode,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the operating mode", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( DS_MUX_MODE_BASIC == mode )
  {

    dlci_param->operating_mode = mode;
    dlci_param->mask          |= DS_MUX_SET_MODE;
    status                     = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_oprt_mode(): "
                         "mode %d mask %d status %d",
                         mode, dlci_param->mask, status);
  return status;
}/*ds_mux_set_oprt_mode*/

ds_mux_result_enum_type  ds_mux_set_subset
(
  ds_mux_subset_enum_type  subset,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the subset ", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if (subset <= DS_MUX_SUBSET_I)
  {

    dlci_param->subset         = subset;
    dlci_param->mask          |= DS_MUX_SET_SUBSET;
    status                     = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_subset(): "
                         "subset %d mask %d status %d",
                         subset, dlci_param->mask, status);
  return status;
}/*ds_mux_set_subset*/

ds_mux_result_enum_type  ds_mux_set_port_speed
(
  ds_mux_port_speed_enum_type  port_speed,
  dlci_cmux_param_type        *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Port Speed", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( DS_MUX_PHY_PORT_SPEED_INVALID < port_speed &&
       port_speed <= DS_MUX_PHY_PORT_SPEED_6)
  {

    dlci_param->port_speed     = port_speed;
    dlci_param->mask          |= DS_MUX_SET_PORT_SPEED;
    status                     = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_port_speed(): "
                         "speed %d mask %d status %d",
                         port_speed, dlci_param->mask, status);
  return status;
}/*ds_mux_set_port_speed*/

ds_mux_result_enum_type  ds_mux_set_N1
(
  uint16                   frame_size,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Frame Size N1", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( 0 < frame_size &&
       frame_size <= DS_MUX_CMUX_MAX_FRAME_N1 )
  {

    dlci_param->frame_size_N1  = frame_size;
    dlci_param->mask          |= DS_MUX_SET_FRAME_SIZE_N1;
    status                     = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_N1(): "
                         "size %d mask %d status %d",
                         frame_size, dlci_param->mask, status);
  return status;
}/*ds_mux_set_N1*/

ds_mux_result_enum_type  ds_mux_set_T1
(
  uint16                   acknowledgement_timer,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Acknowledge Timer T1 ", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( 0 < acknowledgement_timer &&
       acknowledgement_timer <= DS_MUX_CMUX_MAX_TX_T1 )
  {

    dlci_param->response_timer_T1  = acknowledgement_timer;
    dlci_param->mask              |= DS_MUX_SET_ACK_TIMER_T1;
    status                         = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_T1(): "
                         "timer %d mask %d status %d",
                         acknowledgement_timer, dlci_param->mask, status);
  return status;
}/*ds_mux_set_T1*/


ds_mux_result_enum_type  ds_mux_set_N2
(
  uint8                    num_re_transmissions,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Max no of retransmissions N2", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( num_re_transmissions <= DS_MUX_CMUX_MAX_N2 )
  {

    dlci_param->re_transmissions_N2  = num_re_transmissions;
    dlci_param->mask                |= DS_MUX_SET_RE_TRIES_N2;
    status                           = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_N2(): "
                         "re tries %d mask %d status %d",
                         num_re_transmissions, dlci_param->mask, status);
  return status;
}/*ds_mux_set_N2*/


ds_mux_result_enum_type  ds_mux_set_T2
(
  uint16                   response_timer,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the response Timer T2", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( 1 < response_timer &&
       response_timer <= DS_MUX_CMUX_MAX_TX_T2 )
  {

    dlci_param->response_timer_T2    = response_timer;
    dlci_param->mask                |= DS_MUX_SET_RESP_TIMER_T2;
    status                           = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_T2(): "
                         "resp timer %d mask %d status %d",
                         response_timer, dlci_param->mask, status);
  return status;
}/*ds_mux_set_T2*/

ds_mux_result_enum_type  ds_mux_set_T3
(
  uint16                    wake_up_timer,
  dlci_cmux_param_type     *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Wakeup Timer T3", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }

  if ( 0 < wake_up_timer &&
         wake_up_timer <= DS_MUX_CMUX_MAX_TX_T3 )
  {

    dlci_param->wake_up_timer_T3     = wake_up_timer;
    dlci_param->mask                |= DS_MUX_SET_WAKEUP_TIMER_T3;
    status                           = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_T3(): "
                         "wake up timer %d mask %d status %d",
                         wake_up_timer, dlci_param->mask, status);
  return status;
}/*ds_mux_set_T3*/

ds_mux_result_enum_type  ds_mux_set_K
(
  uint8                    window_size,
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type status  = DS_MUX_INVALID_PARAM;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("DLCI PARAMETER IS NULL. Could not set the Window size K", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  if ( DS_MUX_WINDOW_SIZE_INVALID < window_size &&
           window_size <= DS_MUX_WINDOW_SIZE_7 )
  {

    dlci_param->window_size_k        = window_size;
    dlci_param->mask                |= DS_MUX_SET_WINDOW_SIZE_K;
    status                           = DS_MUX_SUCCESS;
  }
  LOG_MSG_INFO2("ds_mux_set_K(): "
                         "window size %d mask %d status %d",
                         window_size, dlci_param->mask, status);
  return status;
}/*ds_mux_set_K*/

ds_mux_result_enum_type   ds_mux_passive_init
(
  dlci_cmux_param_type    *dlci_param
)
{
  ds_mux_result_enum_type result  = DS_MUX_FAILURE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == dlci_param )
  {
    LOG_MSG_ERROR("Passive Init could not be done", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }

  if ( DS_MUX_STATE_OPENED != ds_mux_get_mux_state() )
  {
    LOG_MSG_ERROR("INVALID MUX STATE", 0, 0, 0);
    return DS_MUX_INVALID_MUX_STATE;
  }

  result = ds_muxi_passive_init(dlci_param);

  return result;
}/*ds_mux_passive_init*/


