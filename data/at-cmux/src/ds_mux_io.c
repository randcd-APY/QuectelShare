/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       D S   M U X   I O

  GENERAL DESCRIPTION
    This file contains all the functions, definitions and data types needed
    for MUX input - output processing.

Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/dev/dataservices.tx/1.0/sraghuve.dataservices.tx.1.0.sraghuve_SCM_Changes/tethering/qti_bridge/src/ds_mux.c#1 $
  $DateTime: 2017/08/30 01:35:03 $
  $Author: sraghuve $

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_main.h"

extern boolean                 frag_frame_check;
extern int                     rem_info_len;

ds_mux_result_enum_type ds_mux_io_transmit_cmd_response
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result =             DS_MUX_SUCCESS;
  char*                         tx_ptr =             NULL;
  int_32                        frame_length;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(frame->length <= INFO_FLD_LEN_IF_LEN_FLD_IS_ONE_BYTE)
  {
    frame_length = frame->length + DS_MUX_MIN_FRAME_SZ;
  }
  else
  {
    frame_length = frame->length + DS_MUX_MIN_FRAME_SZ + 1;
  }

  tx_ptr = (char *)calloc(1, DS_MUX_CTL_FRM_SZ + DS_MUX_FEW_EXTRA_BYTES);

  if(tx_ptr == NULL)
  {
    LOG_MSG_ERROR("Failed to allocate memory", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  //genereate MUX frames from input info
  if ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_cmd_input( tx_ptr, frame )))
  {
    ds_mux_uart_tty_send_msg(tx_ptr, frame_length);
  }
  //unable to transmit the data
  if ( DS_MUX_SUCCESS != result)
  {
    LOG_MSG_ERROR("unable to transmit UA frame to TE", 0, 0, 0);
  }

  if(tx_ptr != NULL)
  {
    LOG_MSG_INFO1("Deallocating tx_ptr", 0, 0, 0);
    DS_MUX_MEM_FREE(tx_ptr);
  }
  return result;
}/* ds_mux_io_transmit_cmd_response*/


ds_mux_result_enum_type ds_mux_io_validate_frame
(
  ds_mux_io_frame_type    *frame
)
{
  if(frame == NULL)
  {
    LOG_MSG_ERROR("Frame is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if ( IS_DLCI_INVALID( frame->dlci) )
    {
     LOG_MSG_ERROR("Invalid DLCI-ID in the frame", 0, 0, 0);
      break;
    }

    if ( DS_MUX_FRAME_TYPE_INVALID_FRAME == frame->frame_type )
    {
      LOG_MSG_ERROR("Invalid Frame Type", 0, 0, 0);
      break;
    }

    if ( frame->length > DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN)
    {
      LOG_MSG_ERROR("Frame size is greater than 2^15", 0, 0, 0);
      break;
    }

    return DS_MUX_SUCCESS;

  }while(0);

  LOG_MSG_ERROR("ds_mux_io_validate_frame(): Invalid frame dlci id %d frame type %d len %d",
    frame->dlci, frame->frame_type, frame->length );

  return DS_MUX_FAILURE;
}/* ds_mux_io_validate_frame*/

ds_mux_result_enum_type ds_mux_io_transmit_msg_response
(
  ds_mux_io_frame_type    *frame
)
{
  ds_mux_result_enum_type      result                = DS_MUX_SUCCESS;
  char*                        tx_ptr                = NULL;
  int_32                       frame_length;
  ds_mux_dlci_param_type*      dlci_conn_param       = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(frame == NULL)
  {
    LOG_MSG_ERROR("NULL frame received", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  if(DS_MUX_FRAME_TYPE_INVALID_FRAME != frame->frame_type)
  {
    if(DS_MUX_FRAME_TYPE_UTIL_UIH == frame->frame_type ||
       DS_MUX_FRAME_TYPE_UTIL_UI == frame->frame_type)
     {
       dlci_conn_param = ds_mux_get_dlci_conn_ptr(frame->dlci);
       if(dlci_conn_param ==  NULL)
       {
         LOG_MSG_ERROR("dlci conn NULL", 0, 0, 0);
         return DS_MUX_FAILURE;
       }
       tx_ptr = (char *)calloc(1, dlci_conn_param->frame_size_N1 + DS_MUX_FEW_EXTRA_BYTES);
     }
     else
     {
       tx_ptr = (char *)calloc(1, (DS_MUX_CTL_FRM_SZ + DS_MUX_FEW_EXTRA_BYTES));
     }
  }
  if(tx_ptr == NULL)
  {
    LOG_MSG_ERROR("malloc: Could not allocate memory errno:%d", errno, 0, 0);
    return DS_MUX_FAILURE;
  }

  if ( FALSE == ds_mux_is_flow_enabled (DS_MUX_DLCI_ID_0) && tx_ptr != NULL)
  {
    LOG_MSG_INFO1("Flow is disabled, cannot transmit the frame", 0, 0, 0);
    DS_MUX_MEM_FREE(tx_ptr);
    tx_ptr = NULL;
    return DS_MUX_FLOW_DISABLED;
  }

  if(frame->length <= INFO_FLD_LEN_IF_LEN_FLD_IS_ONE_BYTE)
  {
     frame_length = frame->length + DS_MUX_MIN_FRAME_SZ;
  }
  else
  {
     frame_length = frame->length + DS_MUX_MIN_FRAME_SZ + 1;
  }

  if ( ( NULL != frame->information_ptr ) &&
       ( DS_MUX_SUCCESS == ( result = ds_mux_util_encode_cmd_input( tx_ptr, frame ))))
  {
      LOG_MSG_INFO1("frame->length: %d", frame->length, 0, 0);
      ds_mux_uart_tty_send_msg(tx_ptr, frame_length);
  }

  if(tx_ptr != NULL)
  {
    LOG_MSG_INFO1("De-allocating the frame. tx_ptr", 0, 0, 0);
    DS_MUX_MEM_FREE(tx_ptr);
    tx_ptr = NULL;
  }

  return result;
}/* ds_mux_io_transmit_msg_response*/

ds_mux_result_enum_type ds_mux_io_transmit_msg_cmd
(
  ds_mux_io_frame_type           *output_frame
)
{
  ds_mux_result_enum_type         result             = DS_MUX_SUCCESS;
  char*                           frame_buf          = NULL;
  int_32                          frame_length;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(output_frame == NULL)
  {
    LOG_MSG_ERROR("Output frame is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  frame_buf = (char *)calloc(1, DS_MUX_CTL_FRM_SZ + DS_MUX_FEW_EXTRA_BYTES);
  if(frame_buf == NULL)
  {
    LOG_MSG_ERROR("Could not allocate memory", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  if ( FALSE == ds_mux_is_flow_enabled (DS_MUX_DLCI_ID_0) && frame_buf != NULL)
  {
    LOG_MSG_ERROR( "ds_mux_io_transmit_msg_cmd DLCI 0 is flow controlled",0,0,0);
    DS_MUX_MEM_FREE(frame_buf);
    frame_buf = NULL;
    return DS_MUX_FLOW_DISABLED;
  }

  if(output_frame->length < INFO_FLD_LEN_IF_LEN_FLD_IS_ONE_BYTE)
  {
    frame_length = output_frame->length + DS_MUX_MIN_FRAME_SZ;
  }
  else
  {
    frame_length = output_frame->length + DS_MUX_MIN_FRAME_SZ + 1;
  }
  /*-----------------------------------------------------------------------
   Original packet will be freed after message response. Creating DUP copy of information DSM item.
   UART will be freeing the DUP copy of this DSM item    .
    -----------------------------------------------------------------------*/
  result = ds_mux_util_encode_cmd_input( frame_buf, output_frame );

  if ( DS_MUX_SUCCESS ==  result)
  {
    send_msg_from_ds_mux_to_uart(frame_buf, frame_length);
  }

  LOG_MSG_INFO1("Deallocating Frame buf", 0, 0, 0);
  if(frame_buf != NULL)
  {
    LOG_MSG_INFO1("Deallocating frame buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(frame_buf);
    frame_buf = NULL;
  }
  return result;
}/* ds_mux_io_transmit_msg_cmd */


ds_mux_result_enum_type ds_mux_io_process_rx_frames
(
  ds_mux_io_rx_info        *rx_info,
  int frame_len
)
{
  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  boolean                       ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Retrieved all parsed frame
  -------------------------------------------------------------------------*/
  if(rx_info == NULL)
  {
    LOG_MSG_ERROR("Received NULL rx_info", 0, 0, 0);
    return DS_MUX_FAILURE;
  }
  if ( DS_MUX_SUCCESS == ds_mux_io_validate_frame( rx_info->rx_frame))
  {

    /*-------------------------------------------------------------------------
      Control packets
        All control packet will be routed to DS MUX for processing.
        DS MUX will generate response message if required.
        Item will be freed by DS MUX
      Data packets
        All non-fragmented data packets will routed to logical interface

        All fragmented packets(Convergence layer 4) will be routed to
        DS MUX for further processing

      Frame pointer will be freed here
    -------------------------------------------------------------------------*/
    if ( DS_MUX_DLCI_ID_0 == rx_info->rx_frame->dlci )
    {
      result = ds_mux_process_dlci_rx_ctl_frames( rx_info->rx_frame );
    }
    else
    {
      switch ( rx_info->rx_frame->frame_type)
      {
        case DS_MUX_FRAME_TYPE_UTIL_UIH:
        case DS_MUX_FRAME_TYPE_UTIL_UI:
        {
          LOG_MSG_INFO1("CASE UIH UI", 0, 0, 0);
          ret = ds_mux_Is_dlci_connected(rx_info->rx_frame->dlci);
          if(ret == FALSE)
          {
            LOG_MSG_INFO1("Dlci is not yet connected", 0, 0, 0);
            return DS_MUX_FAILURE;
          }
          LOG_MSG_INFO1("dlci id rx frame: %d", rx_info->rx_frame->dlci, 0, 0);
          if ( DS_MUX_CL_TYPE_4 == ds_mux_get_cl_type( rx_info->rx_frame->dlci ))
          {
            LOG_MSG_INFO1("Convergence layer is four", 0, 0, 0);
            result = ds_mux_process_rx_frag_data_frames( rx_info->rx_frame, frame_len );
          }
          else
          {
            LOG_MSG_INFO1("Convergence layer is NOT four. We got a full frame", 0, 0, 0);
            LOG_MSG_INFO1("rx_info->rx_frame %p", rx_info->rx_frame, 0,0 );
            result = ds_mux_logical_process_rx_data_frames( rx_info->rx_frame, frame_len );
          }
        }
        break;
        case DS_MUX_FRAME_TYPE_UTIL_SABM:
        case DS_MUX_FRAME_TYPE_UTIL_UA:
        case DS_MUX_FRAME_TYPE_UTIL_DM:
        case DS_MUX_FRAME_TYPE_UTIL_DISC:
        {
          LOG_MSG_INFO1("CASE SABM, UA, DM, DISC", 0, 0, 0);
          result = ds_mux_process_dlci_rx_ctl_frames( rx_info->rx_frame );
        }
        break;
        case DS_MUX_FRAME_TYPE_INVALID_FRAME:
        default:
        {
          result = DS_MUX_FAILURE;
        }
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("Validation of frame failed. Therefore not a valid frame", 0, 0, 0);
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_io_process_rx_frames */

ds_mux_result_enum_type ds_mux_util_decode_input
(
  ds_mux_io_rx_info* parsed_rx_frame_info,
  char* frame_info,
  int len
)
{
  boolean                         processing_frames   = TRUE;
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type*         dlci_conn_param     = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("Decoding The received Frame", 0, 0, 0);

  if ( ( NULL == parsed_rx_frame_info || NULL == frame_info) )
  {
    LOG_MSG_ERROR("ds_mux_util_decode_input(): Invalid params %p", parsed_rx_frame_info, 0, 0);
    return DS_MUX_INVALID_PARAM;
  }

  /*-----------------------------------------------------------------------
     1. Loop till no data left or processing aborted due to error or processing is complete
   -----------------------------------------------------------------------*/
  while( TRUE == processing_frames )
  {
    switch ( parsed_rx_frame_info->curr_frame_state )
    {
      case DS_MUX_IO_FRAME_INIT:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_start_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_INIT: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_START_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_flag_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_START: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_ADDR_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_addr_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_ADDR: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_CTRL_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_ctrl_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_CTRL: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
        else
        {
         if(DS_MUX_FRAME_TYPE_INVALID_FRAME != parsed_rx_frame_info->rx_frame->frame_type)
          {
            if(DS_MUX_FRAME_TYPE_UTIL_UIH == parsed_rx_frame_info->rx_frame->frame_type ||
               DS_MUX_FRAME_TYPE_UTIL_UI == parsed_rx_frame_info->rx_frame->frame_type)
            {
              dlci_conn_param = ds_mux_get_dlci_conn_ptr(parsed_rx_frame_info->rx_frame->dlci);
              if(dlci_conn_param ==  NULL)
              {
                LOG_MSG_ERROR("dlci conn NULL", 0, 0, 0);
                return DS_MUX_FAILURE;
              }
              if(dlci_conn_param->frame_size_N1 < len)
              {
                LOG_MSG_ERROR("configured frame size %d is smaller than the actual frame size %d", dlci_conn_param->frame_size_N1, len, 0);
                return DS_MUX_FAILURE;
              }
              parsed_rx_frame_info->rx_frame->information_ptr = (char *)calloc(1, dlci_conn_param->frame_size_N1 - DS_MUX_FRM_SZ_EXPT_INFO_FLD + DS_MUX_FEW_EXTRA_BYTES);
            }
            else
            {
              if(len > DS_MUX_CTL_FRM_SZ)
              {
                LOG_MSG_ERROR("control frame length is greater than 31 bytes: %d", len, 0, 0);
              }
              parsed_rx_frame_info->rx_frame->information_ptr = (char *)calloc(1, (DS_MUX_CTL_FRM_SZ - DS_MUX_MIN_FRAME_SZ + DS_MUX_FEW_EXTRA_BYTES));
            }

            if(parsed_rx_frame_info->rx_frame->information_ptr == NULL)
            {
              LOG_MSG_ERROR("Could-not allocate memory for info ptr and deallocating the frame",0,0,0);
              ds_mux_deinitiatize_frame(&parsed_rx_frame_info);
              return DS_MUX_FAILURE;
            }
          }
        }
        break;

      case DS_MUX_IO_FRAME_LEN_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_len_parsed( parsed_rx_frame_info, frame_info, len)))
        {
          LOG_MSG_ERROR("CASE_LEN: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
      break;

      case DS_MUX_IO_FRAME_INFO_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_info_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_INFO: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
      break;

      case DS_MUX_IO_FRAME_FCS_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_stop_parsed( parsed_rx_frame_info, frame_info)))
        {
          LOG_MSG_ERROR("CASE_STOP: Frame Could not be processed because of FAILURE", 0, 0, 0);
          processing_frames   = FALSE;
        }
        else
        {
          processing_frames = TRUE;
          LOG_MSG_INFO1("Frame Processed", 0, 0, 0);
        }
      break;
      case DS_MUX_IO_FRAME_STOP_PARSED:
        LOG_MSG_INFO1("Full frame parsed", 0, 0, 0);
        processing_frames = FALSE;
      break;
      default :
      {
      }
      break;
    }/* End of switch case */

  }
  //In case of error:: Reset the current frame state and  free the current frame
  if ( DS_MUX_SUCCESS != result)
  {
    LOG_MSG_ERROR("Could-not process the frame.", 0, 0, 0);
    parsed_rx_frame_info->curr_frame_state = DS_MUX_IO_FRAME_INIT;
  }
  return result;
}

ds_mux_result_enum_type ds_mux_io_rx_frame_hdlr
(
  char* frame,
  int len
)
{
  ds_mux_io_rx_info*             parsed_rx_frame_info = NULL;
  ds_mux_result_enum_type result = DS_MUX_SUCCESS;
  int info_len = 0;
  int mux_state;

  LOG_MSG_INFO1("Executing frame handler", 0, 0, 0);

  if(frame == NULL)
  {
    LOG_MSG_ERROR("Pointer pointed to Frame is NULL", 0,0,0);
    return DS_MUX_FAILURE;
  }

  if(frag_frame_check == TRUE && (DS_MUX_STATE_OPENED == ds_mux_get_mux_state() ))
  {

    LOG_MSG_INFO1("It is a Fragmented frame and MUX state is OPENED", 0, 0, 0);
    /*---------------------------------------------------------------------
       The frame contains the remaining info field length and FCS and
       STOP_FLAG field Or it may be completely info field.
    ----------------------------------------------------------------------*/
    if(rem_info_len == len)
    {
      LOG_MSG_INFO1("Remaining info len is equal to len", 0, 0, 0);
      if(frame[len] == DS_MUX_UTIL_START_STOP_FLAG_CODE)
      {
        info_len = len-2;
      }
      else
      {
        info_len = len;
      }
    }
    rem_info_len = rem_info_len - info_len;
    if(DS_MUX_SUCCESS != process_fragmented_frame(parsed_rx_frame_info, info_len , frame))
    {
      result = DS_MUX_FAILURE;
      LOG_MSG_ERROR("Couldnot process the fragmented frame", 0, 0, 0)
    }
  }
  else
  {
    LOG_MSG_INFO1("We got a FULL FRAME", 0, 0, 0);
    if(len < DS_MUX_MIN_FRAME_SZ)
    {
      LOG_MSG_ERROR("Frame size is smaller than minimun frame size:%d", DS_MUX_MIN_FRAME_SZ, 0, 0);
      return DS_MUX_FAILURE;
    }
    else
    {
      LOG_MSG_INFO1("Our frame size is correct", 0, 0, 0);
    }
    /*Parsed_frame_info is deallocated after sending the frame to UART in FN:
      ds_mux_uart_tty_send_msg*/
    parsed_rx_frame_info = (ds_mux_io_rx_info *)malloc(sizeof(ds_mux_io_rx_info));
    if(parsed_rx_frame_info == NULL)
    {
      LOG_MSG_ERROR("Could-not allocate memory",0,0,0);
      return DS_MUX_FAILURE;
    }
    memset(parsed_rx_frame_info, 0, sizeof(ds_mux_io_rx_info));
    parsed_rx_frame_info->rx_frame = NULL;
    LOG_MSG_INFO1("Allocated memory successfully for Frame ptr and frame state ", 0, 0, 0);

    parsed_rx_frame_info->curr_frame_state = DS_MUX_IO_FRAME_INIT;
    parsed_rx_frame_info->rx_frame = (ds_mux_io_frame_type *) malloc (sizeof (ds_mux_io_frame_type));

    if(parsed_rx_frame_info->rx_frame == NULL)
    {
      LOG_MSG_ERROR("Could-not allocate memory for info ptr and deallocating the frame",0,0,0);
      ds_mux_deinitiatize_frame(&parsed_rx_frame_info);
      return DS_MUX_FAILURE;
    }
    memset(parsed_rx_frame_info->rx_frame, 0, sizeof(ds_mux_io_frame_type));
  }

  do
  {
    /*-----------------------------------------------------------------------
         3. Convert Rx item into frames (Invoke utils decode API )
             3.1 A item can have multiple frames
             3.2 It can have partial frame at the end and remaining part could come next
         4. Fetch frames from frame queue and process
            4.1. Contol packets will be routed to DS MUX
            4.2. ( Convergence layer 4) Fragmented packets will be routed to DS MUX
            4.3. Data packets will be routed to logical iface
     -----------------------------------------------------------------------*/
  // TBD call different decoding APIs for normal, modem psc or cld recv mode
  mux_state =  ds_mux_get_mux_state();

  LOG_MSG_INFO1("mux state is: %d",mux_state , 0, 0);
   switch( ds_mux_get_mux_state() )
   {
     case DS_MUX_STATE_OPENED:
     {
        LOG_MSG_INFO1("processing CASE_MUX_STATE_OPENED" , 0, 0, 0);
        result = ds_mux_util_decode_input( parsed_rx_frame_info , frame, len);
     }
     break;

     case DS_MUX_STATE_PSC_RCVD:
     {

     }
     break;

     case DS_MUX_STATE_IN_POWER_SAVE:
     {
        result = ds_mux_util_process_power_save_flag( parsed_rx_frame_info, frame, len);
     }
     break;

     case DS_MUX_STATE_WAKEUP_FLAG_SENT:
     {
       result = ds_mux_util_process_frame_after_wake_up( parsed_rx_frame_info, frame, len);
     }
     break;

     case DS_MUX_STATE_CLD_RCVD:
     {

     }
     break;
     default:
     break;
  }

   /*-----------------------------------------------------------------------
      ERROR:.  Unable to parse theitem. Aborting current item processing
      Note:item supposed to free in decodeder API.
    -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS != result)
    {
      LOG_MSG_ERROR("ds_mux_io_rx_frame_hdlr(): Aborting Rx processing for %p", frame, 0, 0);
      ds_mux_deinitiatize_frame(&parsed_rx_frame_info);
      return result;
    }
    /*Step 4*/
     /* No need to process the fame which contains only flags */
      if(ds_mux_get_mux_state() != DS_MUX_STATE_WAKEUP_FLAG_SENT)
      {
        result = ds_mux_io_process_rx_frames(parsed_rx_frame_info, len);
      }

    if(DS_MUX_CLD_PROCESSED == result)
    {
      LOG_MSG_INFO1("Processed CLD result: %d", 0, 0, 0);
    }
    else if ( DS_MUX_SUCCESS != result)
    {
      LOG_MSG_ERROR("ds_mux_io_rx_frame_hdlr():  Rx frame processing  "
                     " result %d ", result, 0, 0);
    }
    /*Free the frame in both success and failure case*/
    ds_mux_deinitiatize_frame(&parsed_rx_frame_info);
  }while(0);
  return result;
}/* ds_mux_io_rx_sig_hdlr */

