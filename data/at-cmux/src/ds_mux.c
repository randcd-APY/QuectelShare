/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for MUX processing

  DS MUX 27.010 core protocol and logic is implemented in this file

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

#include "ds_mux_main.h"
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <sys/ioctl.h>

/* used to set the parameters received from ATFWD */
at_fwd_cmux_cmd_params_struct*                        cmux_params = NULL;//done

/* used in passive init*/
dlci_cmux_param_type*                                 ds_mux_cmux_params = NULL;//done

/* To mark where it is MUX mode or not. Initially it is not in MUX mode*/
boolean                                               ds_mux_bridge_active = FALSE; //done

/* Response to AT_CMND Buffer */
char*                                                 at_resp_buf = NULL; //done

/* Response callback when at cmd comes fromatfwd deamon */
ds_mux_to_atfwd_response_callback                     at_params_resp_cb = NULL; //done

set_baud_rate                                         set_baud_cb = NULL;

boolean                                               at_cmux_eq_params = FALSE; //done

struct sigevent                                       sev;

int                                                   baud_rate;

boolean                                               msc_resp_rcvd = FALSE;

boolean                                               avoid_enter_mux_mode = FALSE;

ds_mux_timer_common_data*                             t2_timer_data = NULL;

ds_mux_timer_common_data*                             psc_timer_data = NULL;

int                                                   ipr_fd;
ds_mux_dlci_param_type*                               ds_mux_dlci_conn[DS_MUX_MAX_LOGICAL_CONNECTION + 1] = {NULL, NULL, NULL, NULL};

extern boolean                                        ea_bit_length_field;
extern ds_mux_smd_tty_config*                         smd_tty_info;
extern ds_mux_sk_info_t                               sk_info_bw_atfwd_ds_mux;
extern ds_mux_state_info_type                         ds_mux_state_info;
extern ds_mux_smd_data_config                         smd_data_fds;
extern unsigned int                                   timer_ds_mux_sockfd;
extern unsigned int                                   timer_client_sockfd;
extern tty_config*                                    tty_info;

void ds_mux_open_ipr_file()
{

  LOGI("Entering function %s ", __func__);
  ipr_fd = open("/data/ipr", O_CREAT|O_RDWR, DS_MUX_PERM_RDWR);

  if(-1 == ipr_fd)
  {
    LOGI("Could-not create file: errno:%d ", errno);
    return;
  }
  LOGI("Successfully created ipr file fd: %d",ipr_fd);
}

void ds_mux_read_ipr_file(int mode)
{
  int n_bytes = 0;
  char ipr_buff[DS_MUX_IPR_BUFF_SZ];

  memset(ipr_buff, 0, DS_MUX_IPR_BUFF_SZ);

  if(ipr_fd > 0)
  {
    n_bytes = read(ipr_fd , ipr_buff, DS_MUX_IPR_BUFF_SZ);
  }

  if(n_bytes == 0 || ipr_fd < 0)
  {
    if(ipr_fd < 0)
    {
      LOGI("Setting to default baud-rate since failed to open or create /data/ipr");
    }
    if(mode == OPEN_USB_MODE)
    {
      baud_rate = DEFAULT_BAUD_RATE_USB_115200;
    }
    else if(mode == OPEN_UART_MODE)
    {
      baud_rate = DEFAULT_BAUD_RATE_UART_9600;
    }
    else
    {
      LOGI("Invalid mode. It is neither USB nor UART mode: %d", mode);
    }
  }
  else if (n_bytes > 0)
  {
    baud_rate = atoi(ipr_buff);
    LOGI("n_bytes %d baud_rate: %d", n_bytes, baud_rate);

    lseek(ipr_fd, 0, SEEK_SET);
  }
  else
  {
    LOGI("Could-not read file: errno:%d ", errno);
  }
}

void ds_mux_write_ipr_file()
{
  int n_bytes = 0;
  char ipr_buff[DS_MUX_IPR_BUFF_SZ];

  memset(ipr_buff, 0, DS_MUX_IPR_BUFF_SZ);

  snprintf(ipr_buff, DS_MUX_IPR_BUFF_SZ, "%d", baud_rate);

  LOG_MSG_INFO1("ipr_buff %s", ipr_buff, 0, 0);

  if(ipr_fd > 0)
  {
    n_bytes =  write (ipr_fd, ipr_buff, sizeof(ipr_buff));
    fsync(ipr_fd);

    if(n_bytes < 0)
    {
      LOG_MSG_ERROR("writing baud rate to /data/ipr failed errno: %d", errno, 0, 0);
    }
    else
    {
      LOG_MSG_INFO1("no of bytes written = %d", n_bytes, 0, 0);
    }
    lseek(ipr_fd, 0, SEEK_SET);
  }
  else
  {
    LOGI("Invalid ipr_fd: %d ", ipr_fd);
  }
}

ds_mux_dlci_param_type* ds_mux_get_dlci_conn_ptr
(
  uint_32                   dlci_id
)
{
  ds_mux_dlci_param_type  *dlci_conn = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( IS_DLCI_INVALID( dlci_id) )
  {
    LOG_MSG_ERROR( "ds_mux_get_dlci_conn_ptr unable to find dlci_id %d", dlci_id, 0, 0);
    return NULL;
  }

  if(ds_mux_dlci_conn == NULL)
  {
    LOG_MSG_ERROR("No DCLIs are allocated", 0, 0, 0);
    return NULL;
  }

  dlci_conn = (ds_mux_dlci_param_type *)(ds_mux_dlci_conn[dlci_id]);

  if(dlci_conn != NULL)
  {

    LOG_MSG_INFO1("dlci_id: %d fcon mask: %d dlci state =%d",
                   dlci_conn->dlci_id,
                   dlci_conn->flow_control_mask,
                   dlci_conn->state);
    LOG_MSG_INFO1("frame_type: %d priority: %d frame size: %d",
                   dlci_conn->frame_type,
                   dlci_conn->priprity,
                   dlci_conn->frame_size_N1);
    LOG_MSG_INFO1("T1: %d N2 retransmissions: %d N2 curr transmissions: %d",
                  dlci_conn->response_timer_T1,
                  dlci_conn->re_transmissions_N2,
                  dlci_conn->curr_transmissions_N2);
    LOG_MSG_INFO1("wake_up_timer_T3: %d window sz: %d cltype: %d",
                   dlci_conn->wake_up_timer_T3,
                   dlci_conn->window_size_k,
                   dlci_conn->cl_type);

    LOG_MSG_INFO1("t2_timer_id: %d", dlci_conn->t2_timer_data.timer_id, 0, 0);
  }

  if (  NULL == dlci_conn || dlci_conn->dlci_id != dlci_id)
  {
    LOG_MSG_ERROR( "ds_mux_get_dlci_conn_ptr invalid dlci_id %d dlci conn %x", dlci_id, dlci_conn, 0);

    return NULL;
  }

  return dlci_conn;
}/* ds_mux_get_dlci_conn_ptr */



boolean ds_mux_is_initiator
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds_mux_state_info.is_initiator;
}/* ds_mux_is_initiator */

boolean   ds_mux_is_flow_enabled
(
  uint8                           dlci_id
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
    LOG_MSG_ERROR("dlci_conn_param is NULL", 0, 0, 0);
    return FALSE;
  }

  if ( DS_MUX_FLOW_DISABLE_MIN != dlci_conn_param->flow_control_mask)
  {
    LOG_MSG_ERROR("FLow is disabled", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}/*ds_mux_is_flow_enabled*/

ds_mux_cl_type_enum_type ds_mux_get_cl_type
(
  uint8                       dlci
)
{

  ds_mux_cl_type_enum_type       cl_type = DS_MUX_CL_TYPE_2;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci);

  if(NULL != dlci_conn_param)
  {
    cl_type = dlci_conn_param->cl_type;
  }
  return cl_type;
}/* ds_mux_get_cl_type*/

boolean ds_mux_Is_dlci_connected(uint8 dlci_id)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1("checking DLCI connected or not", 0, 0, 0);
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if(dlci_conn_param ==  NULL)
  {
    LOG_MSG_ERROR("dlci conn NULL", 0, 0, 0);
    return FALSE;
  }
  else if(dlci_conn_param->state == DS_MUX_DLCI_CONNECTED)
  {
   LOG_MSG_INFO1("Dlci Connected", 0, 0, 0);
    return TRUE;
  }
  LOG_MSG_INFO1("DLCI STATE: %d", dlci_conn_param->state, 0, 0);
  return FALSE;

}

void ds_mux_prepare_and_send_msc_message
(
  uint8                                  dlci_id,
  uint8                                  fc_flag,
  uint8                                  rtc_asserted_flag,
  uint8                                  cd_asserted_flag,
  uint8                                  ic_asserted_flag,
  uint8                                  rtr_asserted_flag
)
{
  ds_mux_io_frame_type*                  output_frame   = NULL;
  uint8                                  msg_type       = DS_MUX_DLCI_FRAME_VAL_MSC | DS_MUX_DLCI_MSG_TYPE_CR_MASK;
  byte                                   msg_len        = DS_MUX_DLCI_MSC_VAL_LEN;
  uint8                                  signal_frame_value;
  ds_mux_dlci_param_type*                dlci_conn_param = NULL;
  int                                    idx             = 0;
  int                                    len             = 0;
  uint8                                  dlci_id_ea_cr;
  int                                    ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
  {
    LOG_MSG_ERROR( "ds_mux_prepare_and_send_msc_message DLCI CONN ptr is %x ", dlci_conn_param,0,0);
    return;
  }

  do
  {
    /*-----------------------------------------------------------------------
          1. Allocate default frame
          2. Prepare frame
          3. Insert message type (Message type already have EA and CR bit set)
          4. Insert message length (Message len is 2, 1 DLCI Byte, 1 Signal Byte)
          5. Prepare DLCI ID (Last 2 bits will be 1 (including EA bit)) and insert DLCI ID
          6. Set all signal bits based on input parameters.and insert signal byte
          7. transmit the raw frame to DS MUX IO
    -----------------------------------------------------------------------*/
    /* Step 1 */
    ds_mux_dlci_conn[dlci_id]->curr_msg_frame = ds_mux_util_alloc_frame();

    if ( NULL == ds_mux_dlci_conn[dlci_id]->curr_msg_frame)
    {
      LOG_MSG_ERROR("Could-not allocate Memory", 0, 0, 0);
      return;
    }

    ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr = (char *)calloc(1, (DS_MUX_CTL_FRM_SZ-DS_MUX_MIN_FRAME_SZ));

    if(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr == NULL)
    {
      LOG_MSG_ERROR("Could-not allocate Memory", 0, 0, 0);
      DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame);
      ds_mux_dlci_conn[dlci_id]->curr_msg_frame = NULL;
      return;
    }

    output_frame = (ds_mux_dlci_conn[dlci_id])->curr_msg_frame;

    if(NULL == output_frame)
    {
      LOG_MSG_ERROR("Output frame is NULL", 0, 0, 0);
      return;
    }

    /* Step 2 */
    ds_mux_util_prepare_data_frame( DS_MUX_DLCI_ID_0, DS_MUX_FRAME_TYPE_UTIL_UIH, output_frame);

    /* Step 3 */
    output_frame->information_ptr[idx++] = msg_type;
    len++;

    /* step 4 */
    output_frame->information_ptr[idx++] = msg_len;
    len++;

    /* Step 5 */
    dlci_id_ea_cr = dlci_id << 2;
    dlci_id_ea_cr |= 0x3;

    output_frame->information_ptr[idx++] = dlci_id_ea_cr;
    len++;

    /* Step 6 */
    signal_frame_value = (DS_MUX_DLCI_MSG_TYPE_EA_MASK |
                         (fc_flag << 1) |
                         (rtc_asserted_flag << 2) |
                         (rtr_asserted_flag << 3)|
                         (ic_asserted_flag  << 6)|
                         (cd_asserted_flag << 7));

    output_frame->information_ptr[idx] = signal_frame_value;
    len++;

    output_frame->length = len;

    LOG_MSG_INFO1( "ds_mux_prepare_and_send_msc_message signal byte %d ", signal_frame_value,0,0);

  /* Step 7 */
  /*-------------------------------------------------------------------------
    If Flow is disabled then don't start the timer as Host would not receive the cmd due
    to flow control. DS MUX do re- transmission again once flow become enabled and
    will start the timer
   -------------------------------------------------------------------------*/
    LOG_MSG_INFO1("transmitting Frame and starting timer", 0, 0, 0);
    if ( DS_MUX_FLOW_DISABLED != ds_mux_io_transmit_msg_cmd( output_frame ))
    {
      if ( 0 != dlci_conn_param->t2_timer_data.timer_id )
      {
        LOG_MSG_INFO1("T2 timer val %d",dlci_conn_param->response_timer_T2 , 0, 0);
        dlci_conn_param->t2_timer_data.is_running = TRUE;
        ret = ds_mux_timer_start(&(dlci_conn_param->t2_timer_data),
                            dlci_conn_param->response_timer_T2, DS_MUX_T2_TIMER);
        if(ret != DS_MUX_SUCCESS_MACRO)
        {
          LOG_MSG_ERROR("Failed to start timer", 0, 0, 0);
          break;
        }
        dlci_conn_param->curr_transmissions_N2 = 1;
        return;
      }
      else
      {
        LOG_MSG_ERROR("Timer is Invalid", 0, 0, 0);
        break;
      }
    }
    else
    {
      LOG_MSG_ERROR("FLOW DISABLED. Could not transmit MSC", 0, 0, 0);
      break;
    }

  }while(0);

  //handaling Failure case.
  if(dlci_conn_param->curr_msg_frame != NULL && output_frame->information_ptr != NULL)
  {
    LOG_MSG_INFO1("Deallocating output frame and its information ptr", 0, 0, 0);
    DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr);
    ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr = NULL;
    DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame);
    ds_mux_dlci_conn[dlci_id]->curr_msg_frame = NULL;
    return;
  }
}

ds_mux_result_enum_type ds_mux_logical_process_rx_data_frames
(
  ds_mux_io_frame_type    *frame,
  int frame_len
)
{

  ds_mux_result_enum_type       result      = DS_MUX_SUCCESS;
  ds_mux_cl_type_enum_type      cl_type     = DS_MUX_CL_TYPE_INVALID;
  char*                         rx_data_ptr = NULL;
  uint8                         cl2_v24_val;
  uint16                        index       =0;
  int16                         ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == frame )
  {
    LOG_MSG_ERROR("frame is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  if( NULL == frame->information_ptr)
  {
    LOG_MSG_ERROR("frame->information_ptr is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  if(ea_bit_length_field)
  {
     LOG_MSG_INFO1("EA bit set in length field", 0, 0, 0);
     if(frame_len == DS_MUX_MIN_FRAME_SZ )
     {
       LOG_MSG_ERROR("EA bit is set, info field is not present", 0, 0, 0);
       return DS_MUX_FAILURE;
     }
  }
  else
  {
    if(frame_len == DS_MUX_MIN_FRAME_SZ +1)
    {
      LOG_MSG_ERROR("EA bit is NOT set, info field is not present", 0, 0, 0);
      return DS_MUX_FAILURE;
    }
  }

  /*-----------------------------------------------------------------------
    1 Get CL type
      1.1 If CL is type 2 then process signal byte and remove it from the data
    2 Enque the data into Rx Item
  -----------------------------------------------------------------------*/
  rx_data_ptr = frame->information_ptr;

  cl_type = ds_mux_get_cl_type( frame->dlci );

  LOG_MSG_INFO1("cltype is : %d", cl_type, 0, 0);

  if ( DS_MUX_CL_TYPE_2 == cl_type)
  {
    cl2_v24_val = rx_data_ptr[index++];
    //Processing the V24 signals received in the MSC command.
    ds_mux_logical_process_v24_signal( frame->dlci, cl2_v24_val);

  }

  result = send_msg_ds_mux_to_smd( frame->dlci, rx_data_ptr, frame->length);
  rx_data_ptr = NULL;

  if(result != DS_MUX_SUCCESS)
  {
    result = DS_MUX_FAILURE;
    LOG_MSG_ERROR("Failed to write on SMD", 0, 0, 0);
  }
  return result;
}/* ds_mux_logical_process_rx_data_frames*/



ds_mux_result_enum_type ds_mux_process_rx_frag_data_frames
(
  ds_mux_io_frame_type    *frame,
  int frame_len
)
{

  ds_mux_result_enum_type       result                     = DS_MUX_SUCCESS;
  uint8                         cl4_ctl_header             = (uint8)CL_FOUR_CTL_HDR_ENTIRE_FRAME;
  ds_mux_dlci_param_type*       dlci_conn_param            = NULL;
  char*                         pkt_head_ptr               = NULL;
  uint16                        index                      = 0;
  static uint16                 index_pkt_head_ptr         = 0;
  char*                         dest                       = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == frame ||
       NULL == frame->information_ptr || frame->length == 0 )
  {
     LOG_MSG_ERROR("ds_mux_process_rx_frag_data_frames(): Invalid pkt, frame %x ", frame, 0, 0);
     return DS_MUX_FAILURE;
  }

  if(ea_bit_length_field)
  {
     if(frame_len == DS_MUX_MIN_FRAME_SZ)
     LOG_MSG_ERROR("EA bit is set, info field is not present", 0, 0, 0);
     return DS_MUX_FAILURE;
  }
  else
  {
    if(frame_len == DS_MUX_MIN_FRAME_SZ + 1)
    LOG_MSG_ERROR("EA bit is NOT set, info field is not present", 0, 0, 0);
    DS_MUX_FAILURE;
  }

  pkt_head_ptr = frame->information_ptr;
  dlci_conn_param = ds_mux_get_alloc_conn_ptr(frame->dlci);

  if ( NULL == dlci_conn_param )
  {
    LOG_MSG_ERROR("ds_mux_process_rx_frag_data_frames(): Invalid conn ptr ", 0, 0, 0);
    return  DS_MUX_FAILURE;
  }

  cl4_ctl_header = pkt_head_ptr[index++];
  switch ( cl4_ctl_header )
  {
    case CL_FOUR_CTL_HDR_ENTIRE_FRAME:
    {
      /*-----------------------------------------------------------------------
        Free old items
      -----------------------------------------------------------------------*/
      if ( NULL != dlci_conn_param->pending_rx_cl4_data_ptr )
      {
        DS_MUX_MEM_FREE(ds_mux_dlci_conn[frame->dlci]->pending_rx_cl4_data_ptr);
        ds_mux_dlci_conn[frame->dlci]->pending_rx_cl4_data_ptr = NULL;
      }
      result = send_msg_ds_mux_to_smd( frame->dlci, pkt_head_ptr, frame->length );

    }
    break;

    case CL_FOUR_CTL_HDR_FIRST_FRAME:
    {
      /*-----------------------------------------------------------------------
        Free old pending items and build up new chain
       -----------------------------------------------------------------------*/
      if ( NULL != dlci_conn_param->pending_rx_cl4_data_ptr )
      {
        LOG_MSG_ERROR("Error: ds_mux_process_rx_frag_data_frames(): dropping ptr ", 0, 0, 0);

        DS_MUX_MEM_FREE(ds_mux_dlci_conn[frame->dlci]->pending_rx_cl4_data_ptr);
        ds_mux_dlci_conn[frame->dlci]->pending_rx_cl4_data_ptr = NULL;
      }
      //Update the first frame
      dlci_conn_param->pending_rx_cl4_data_ptr = pkt_head_ptr;
      index_pkt_head_ptr = index_pkt_head_ptr + frame->length;

    }
      break;

    case CL_FOUR_CTL_HDR_MIDDLE_FRAME:
    {
      /*-----------------------------------------------------------------------
        If there is no previous frame then discard middle frame otherwise append the middle frame
       -----------------------------------------------------------------------*/
      if ( NULL == dlci_conn_param->pending_rx_cl4_data_ptr )
      {
        LOG_MSG_ERROR("Error: ds_mux_process_rx_frag_data_frames(): dropping ptr ",0, 0, 0);
        return DS_MUX_FAILURE;
      }
      dest = dlci_conn_param->pending_rx_cl4_data_ptr + index_pkt_head_ptr;
      if(frame->length > 0)
        memcpy( dest, pkt_head_ptr, frame->length);
      else
        LOG_MSG_ERROR("information field length is 0", 0, 0, 0);

      index_pkt_head_ptr = index_pkt_head_ptr + frame->length;

    }
    break;
    case CL_FOUR_CTL_HDR_LAST_FRAME:
    {
      /*-----------------------------------------------------------------------
        If there is no previous frame then discard last frame
      -----------------------------------------------------------------------*/
      if ( NULL == dlci_conn_param->pending_rx_cl4_data_ptr && pkt_head_ptr != NULL)
      {
        LOG_MSG_ERROR("ds_mux_process_rx_frag_data_frames(): dropping ptr ",0 ,0, 0);
        return DS_MUX_FAILURE;
      }

      dest = dlci_conn_param->pending_rx_cl4_data_ptr + index_pkt_head_ptr;
      memcpy( dest, pkt_head_ptr, frame->length);
      index_pkt_head_ptr = index_pkt_head_ptr + frame->length;
      result = send_msg_ds_mux_to_smd( frame->dlci, pkt_head_ptr, frame->length );

      dlci_conn_param->pending_rx_cl4_data_ptr = NULL;
    }
    break;
    default:
    {
      LOG_MSG_ERROR("ds_mux_process_rx_frag_data_frames(): dropping ptr ",0, 0, 0);
      result = DS_MUX_FAILURE;
      break;
    }
  }
  return result;
}/* ds_mux_process_rx_frag_data_frames*/



void  ds_mux_enable_flow_mask
(
  uint8                           dlci_id,
  ds_mux_flow_ctl_mask_enum_type  fc_msk
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
    return;
  }

  LOG_MSG_INFO2( "ds_mux_enable_flow_mask dlci_id %d fc_msk  %x current flow mask %x",
      dlci_id, fc_msk, dlci_conn_param->flow_control_mask);

  dlci_conn_param->flow_control_mask  &= (~fc_msk);

  //Flow is beaing enabled on this DLCI hence
  if ( DS_MUX_FLOW_DISABLE_MIN != dlci_conn_param->flow_control_mask)
  {
    if ( 0 == dlci_id)
    {
      //ds_mux_dlci_0_flow_enabled( dlci_conn_param );
    }
    else
    {
      //ds_mux_dlci_n_flow_enabled( dlci_conn_param );
    }
  }
}/* ds_mux_enable_flow_mask */



boolean ds_mux_is_dlci_0_connected
(
  void
)
{
  boolean                 result          = TRUE;
  ds_mux_dlci_param_type *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dlci_conn_param = ds_mux_get_alloc_conn_ptr(DS_MUX_DLCI_ID_0);

  if ( NULL == dlci_conn_param ||
       DS_MUX_DLCI_CONNECTED != dlci_conn_param->state )
  {
    result = FALSE;
  }

  return result;
}/* ds_mux_is_dlci_0_connected */


void ds_mux_set_is_initiator
(
  boolean  is_initiator
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2( " ds_mux_set_is_initiator  %d", is_initiator, 0, 0);
  ds_mux_state_info.is_initiator  = is_initiator;

}/*ds_mux_set_is_initiator*/

void ds_mux_prepare_ua_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_util_prepare_cmd_frame( input_frame->dlci, DS_MUX_FRAME_TYPE_UTIL_UA,
    FALSE, input_frame->poll_final, output_frame );

  output_frame->length_wt_ea = input_frame->length_wt_ea;

  return ;
}/* ds_mux_prepare_ua_response */

ds_mux_result_enum_type ds_mux_process_ua_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1  Fetch DLCI connection info from DLCI ID
    2. Validate final bit (Final Bit should set to 1)
  -----------------------------------------------------------------------*/
  do
  {
    dlci_conn_param = (ds_mux_dlci_param_type *)ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      result = DS_MUX_FAILURE;
      break ;
    }

    if (0 == frame->poll_final)
    {
      result = DS_MUX_FAILURE;
      break ;
    }

  }while(0);

  return result;
}/* ds_mux_process_ua_cmd_frames*/



ds_mux_result_enum_type ds_mux_process_sabm_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type*       dlci_conn_param  = NULL;
  ds_mux_io_frame_type          ua_rsp_frame;

  uint8                          fc_flag           = 0;
  uint8                          rtc_asserted_flag = 0;
  uint8                          cd_asserted_flag  = 0;
  uint8                          ic_asserted_flag  = 0;
  uint8                          rtr_asserted_flag = 1;

  memset (&ua_rsp_frame, 0, sizeof(ds_mux_io_frame_type));
  ua_rsp_frame.information_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //Host should set poll bit to 1
    if (FALSE == frame->poll_final )
    {
      LOG_MSG_ERROR("Poll is is not set", 0, 0, 0);
      result = DS_MUX_FAILURE;
      break;
    }

    //Get DLCI connection ptr
    dlci_conn_param = ds_mux_get_alloc_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      LOG_MSG_ERROR("dlci_conn_param is NULL", 0, 0, 0);
      result = DS_MUX_FAILURE;
      break ;
    }

    //If DLCI is already connected no Action will be taken.
    if (DS_MUX_DLCI_CONNECTED == dlci_conn_param->state )
    {
      LOG_MSG_INFO1("DLCI is already connected so no action is taken", 0, 0, 0);
      result = DS_MUX_SUCCESS;
      break ;
    }

    LOG_MSG_INFO1("frame->dlci: %d", frame->dlci, 0, 0);
    if ( DS_MUX_DLCI_ID_0 == frame->dlci)
    {
       LOG_MSG_INFO1("Set SABM initiator as FALSE", 0, 0, 0);
       ds_mux_set_is_initiator(FALSE);
    }
    else
    {
      //DLCI0 should be connected before any new logical connection
      if(FALSE == ds_mux_is_dlci_0_connected() )
      {
        LOG_MSG_ERROR("Received SABM for DLCI %d before SABM for DLCI 0", frame->dlci, 0, 0);
        result = DS_MUX_FAILURE;
        break ;
      }
    }

    //Set state to connected
    dlci_conn_param->state = DS_MUX_DLCI_CONNECTED;

    //Prepare UA response for SABM command
    LOG_MSG_INFO1("Preparing UA response", 0, 0, 0);
    ds_mux_prepare_ua_response(frame, &ua_rsp_frame);

    //Send UA to host transmit &ua_rsp_frame
    LOG_MSG_INFO1("Transmitting response to HOST", 0, 0, 0);
    ds_mux_io_transmit_cmd_response(&ua_rsp_frame);

    LOG_MSG_INFO1("@DLCIb %d", dlci_conn_param->dlci_id, 0, 0);
    //We do not send MSC command for DLC0
    if(dlci_conn_param->dlci_id != 0)
    {
      ds_mux_prepare_and_send_msc_message( dlci_conn_param->dlci_id,
                                           fc_flag,
                                           rtc_asserted_flag,
                                           cd_asserted_flag,
                                           ic_asserted_flag,
                                           rtr_asserted_flag);
    }

  }while(0);

  LOG_MSG_INFO2( " ds_mux_process_sabm_cmd_frames result %d dlci id %d ", result, frame->dlci, 0);
  return result;
}/* ds_mux_process_sabm_cmd_frames*/


void ds_mux_prepare_msg_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  output_frame->length       = input_frame->length;
  output_frame->length_wt_ea = input_frame->length_wt_ea;
  LOG_MSG_INFO1("ip len:%d ip_len_ea: %d", input_frame->length, input_frame->length_wt_ea, 0);
  /*-----------------------------------------------------------------------
   Prepare ouput frame from input frame.Length and info field will be updated as par of message processing
  -----------------------------------------------------------------------*/
  ds_mux_util_prepare_cmd_frame( input_frame->dlci, input_frame->frame_type,
    FALSE, input_frame->poll_final, output_frame );

  return ;
}/* ds_mux_prepare_msg_response */

ds_mux_result_enum_type ds_mux_get_msg_type
(
  byte                          msg_byte,
  ds_mux_msg_type_enum_type    *msg_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch ((uint_32)(msg_byte & (0xff ^ DS_MUX_DLCI_MSG_TYPE_CR_MASK) ))
   {
      case DS_MUX_DLCI_FRAME_VAL_PN:
         *msg_type =  DS_MUX_MSG_TYPE_VAL_PN;
          break;
      case DS_MUX_DLCI_FRAME_VAL_PSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_PSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_CLD:
         *msg_type = DS_MUX_MSG_TYPE_VAL_CLD;
          break;
      case DS_MUX_DLCI_FRAME_VAL_TEST:
         *msg_type = DS_MUX_MSG_TYPE_VAL_TEST;
          break;
      case DS_MUX_DLCI_FRAME_VAL_FCON:
         *msg_type = DS_MUX_MSG_TYPE_VAL_FCON;
          break;
      case DS_MUX_DLCI_FRAME_VAL_FCOFF:
         *msg_type = DS_MUX_MSG_TYPE_VAL_FCOFF;
          break;
      case DS_MUX_DLCI_FRAME_VAL_MSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_MSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_NSC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_NSC;
          break;
      case DS_MUX_DLCI_FRAME_VAL_RPN:
         *msg_type = DS_MUX_MSG_TYPE_VAL_RPN;
          break;
      case DS_MUX_DLCI_FRAME_VAL_RLS:
         *msg_type = DS_MUX_MSG_TYPE_VAL_RLS;
          break;
      case DS_MUX_DLCI_FRAME_VAL_SNC:
         *msg_type = DS_MUX_MSG_TYPE_VAL_SNC;
          break;
      default:
        result           = DS_MUX_FAILURE;
   }
  LOG_MSG_INFO2( " ds_mux_get_msg_type function msg type %d msg_byte %d ", *msg_type, msg_byte, 0);
  return result;
}/* ds_mux_get_msg_type */

int ds_mux_timer_start(ds_mux_timer_data_t* timer_data, uint16 timer_val, ds_mux_timer_type type )
{

  struct itimerspec its;

  /* Converting Mili-seconds to Nano-seconds */
  int_32 timer_value;
  int_32 nano_timer_val = 0;

  memset(&its, 0, sizeof(struct itimerspec));

  switch(type)
  {

    case DS_MUX_T2_TIMER:
      /* Converting Mili-seconds to Nano-seconds */
      timer_value = timer_val * pow(10, 7);
      its.it_value.tv_sec = 0;
      its.it_value.tv_nsec = timer_value;
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec =0;
    break;

    case DS_MUX_PSC_TIMER:
      its.it_value.tv_sec = timer_val;
      its.it_value.tv_nsec = 0;
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec =0;
    break;

    case DS_MUX_UART_INACTIVITY_TIMER:
      LOG_MSG_INFO1("starting uart inactivity timer",0,0,0);
      /* Converting Mili-seconds to seconds */
      timer_val = ds_mux_convert_mili_to_sec(timer_val);
      if(timer_val <= 1)
      {
        /* Converting seconds to nanoseconds */
        nano_timer_val = ds_mux_convert_sec_to_nano(timer_val);
      }
      its.it_value.tv_sec = timer_val;
      its.it_value.tv_nsec = nano_timer_val;
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec =0;
    break;

    default:
      LOG_MSG_ERROR("Invalid timer type", 0, 0, 0);
      return DS_MUX_FAILURE_MACRO;

  }

  if (timer_settime(timer_data->timer_id,
                    0,
                    &its,
                    NULL) == -1)
  {
    LOG_MSG_ERROR("failed to start timer ", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  LOGI("Timer %d set successfully", type);

  return DS_MUX_SUCCESS_MACRO;
}

int ds_mux_timer_stop(ds_mux_timer_data_t* t2_timer_data)
{

  struct itimerspec its;

  memset(&its, 0, sizeof(struct itimerspec));

  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec =0;

  if (timer_settime(t2_timer_data->timer_id,
                    0,
                    &its,
                    NULL) == -1)
  {
    LOG_MSG_ERROR("failed to stop timer ", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  LOG_MSG_INFO1("Timer Stopped", 0, 0, 0);
  return DS_MUX_SUCCESS_MACRO;
}


void ds_mux_timer_cb
(
  int            sig,
  siginfo_t*     si,
  void*          uc
)
{
  ds_mux_timer_common_data*      common_timer_data = (ds_mux_timer_common_data *)si->si_value.sival_ptr;
  uint8*                         dlci_id;
  ds_mux_dlci_param_type*        dlci_conn_param = NULL;
  ds_mux_dlci_param_type*        dlci_conn_dlci0 = NULL;
  int16                          ret = DS_MUX_SUCCESS_MACRO;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("ds_mux_timer_cb %d", common_timer_data->timer_id_type, 0, 0);

  switch(common_timer_data->timer_id_type)
  {
    case DS_MUX_T2_TIMER:
      dlci_id = (uint8 *)common_timer_data->user_data;
      LOG_MSG_INFO1("T2 timer case: dlci_id %d", *dlci_id , 0, 0);
      dlci_conn_param = ds_mux_get_dlci_conn_ptr(*dlci_id);
      if(dlci_conn_param != NULL)
      {
        dlci_conn_param->t2_timer_data.is_running = FALSE;
      }
      ret = ds_mux_t2_timer_ind(*dlci_id);
    break;

    case DS_MUX_PSC_TIMER:
       LOG_MSG_INFO1("PSC timer case", 0, 0, 0);
       dlci_conn_dlci0 = ds_mux_get_dlci_conn_ptr(DS_MUX_DLCI_ID_0);
       if(dlci_conn_dlci0 != NULL)
       {
         dlci_conn_dlci0->psc_timer_data.is_running = FALSE;
       }
       else
       {
         LOG_MSG_ERROR("dlci_conn0 NULL", 0, 0, 0);
       }
       ds_mux_set_mux_state(DS_MUX_STATE_WAKEUP_FLAG_SENT);
    break;
  }

  if(ret == DS_MUX_FAILURE_MACRO)
    LOG_MSG_ERROR("ds_mux_timer_ind failed", 0, 0, 0);

}

int ds_mux_t2_timer_ind
(
  uint32              user_data                          /* User Data */
)
{
  ds_mux_timer_common_data* common_timer_data = NULL;
  int16 numBytes=0, len;
  struct sockaddr_un timer_ds_mux;

  common_timer_data = calloc(1, sizeof(ds_mux_timer_common_data));
  if(NULL == common_timer_data)
  {
    LOG_MSG_ERROR("could-not allocate memory to common_timer_data", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  timer_ds_mux.sun_family = AF_UNIX;
  strlcpy(timer_ds_mux.sun_path, SERVER_TIMER_FILE, sizeof(SERVER_TIMER_FILE));
  len = strlen(timer_ds_mux.sun_path) + sizeof(timer_ds_mux.sun_family);

  common_timer_data->user_data = (void *)&user_data;

  if ((numBytes = sendto(timer_client_sockfd, (void *)common_timer_data, sizeof(ds_mux_timer_common_data), 0,
          (struct sockaddr *)&timer_ds_mux, len)) == -1)
  {
    LOG_MSG_ERROR("Sendto failed:errno: %d coud not send from timer context to DS_MUX context", errno, 0, 0);
    if(NULL != common_timer_data)
    {
      DS_MUX_MEM_FREE(common_timer_data);
      common_timer_data = NULL;
    }
    return DS_MUX_FAILURE_MACRO;
  }

  if(NULL != common_timer_data)
  {
    DS_MUX_MEM_FREE(common_timer_data);
    common_timer_data = NULL;
  }
  return DS_MUX_SUCCESS_MACRO;
}

int ds_mux_process_t2_timer_ind
(
  int fd
)
{
  struct sockaddr_storage their_addr;
  socklen_t addr_len = sizeof(struct sockaddr_storage);
  int16 nbytes = 0;
  ds_mux_timer_common_data *common_timer_data = NULL;
  ds_mux_dlci_param_type*        dlci_conn_param = NULL;
  uint32*                         dlci_ptr;
  int16                          ret;
  uint8 dlci_id;
  char buf[MAX_BUF_LEN_TIMER_IPC];

  LOG_MSG_INFO1("Received Message from timer_context",0,0,0);
  if ( (nbytes = recvfrom(fd, buf, MAX_BUF_LEN_TIMER_IPC-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
  {
    if ( nbytes == 0 )
    {
      LOG_MSG_INFO1("Completed full recv from ds_mux_timer_sockfd context", 0, 0, 0);
    }
    else
    {
      LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
    }
  }
  else
  {
    common_timer_data = (ds_mux_timer_data_t *)buf;

      /*-------------------------------------------------------------------------
      1. Re-transmit the packet if curr_transmission is less then N2 (Max Re- transmission )
      2. If Flow is disabled then don't start the timer as Host would not receive the cmd due
         to flow control. DS MUX do re- transmission again once flow become enabled and
         will start the timer
    -------------------------------------------------------------------------*/
    dlci_ptr = (uint_32*)(common_timer_data->user_data);
    LOG_MSG_INFO1("returned to main context: %d", *dlci_ptr, 0, 0);
    dlci_conn_param = ds_mux_get_dlci_conn_ptr(*dlci_ptr);


    if ( NULL == dlci_conn_param || DS_MUX_DLCI_CONNECTED != dlci_conn_param->state)
    {
      LOG_MSG_ERROR( "ds_mux_timer_cb invalid DLCI conn %x state ", dlci_conn_param, 0, 0);
      return;
    }
    dlci_id = dlci_conn_param->dlci_id;
    /*Step 1*/
    if ( dlci_conn_param->curr_transmissions_N2 <= dlci_conn_param->re_transmissions_N2)
    {
      LOG_MSG_INFO1("MSC %d", msc_resp_rcvd, 0, 0);
      /*Step 2*/
      if(msc_resp_rcvd == FALSE && dlci_conn_param->curr_msg_frame != NULL)
      {
        if ( DS_MUX_FLOW_DISABLED != ds_mux_io_transmit_msg_cmd( dlci_conn_param->curr_msg_frame))
        {
          dlci_conn_param->t2_timer_data.is_running = TRUE;
          ret = ds_mux_timer_start( &(dlci_conn_param->t2_timer_data),
                              dlci_conn_param->response_timer_T2, DS_MUX_T2_TIMER );
         if(ret != DS_MUX_SUCCESS_MACRO)
         {
           LOG_MSG_ERROR("Failed to start timer", 0, 0, 0);
           return DS_MUX_FAILURE_MACRO;
         }
          dlci_conn_param->curr_transmissions_N2++;
        }
        else
        {
          LOG_MSG_ERROR("Flow is Disabled so cannot transmit the MSC", 0, 0, 0);
          if(dlci_conn_param->curr_msg_frame != NULL && dlci_conn_param->curr_msg_frame->information_ptr != NULL)
          {
            LOG_MSG_INFO1("Deallocating output frame and its information ptr", 0, 0, 0);
            DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr);
            ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr = NULL;
            DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame);
            ds_mux_dlci_conn[dlci_id]->curr_msg_frame = NULL;
          }
          return DS_MUX_FAILURE_MACRO;
        }
      }
      else
      {
        LOG_MSG_INFO1("MSC response is received Correctly or MSC Cmd: %p", dlci_conn_param->curr_msg_frame, 0, 0);
      }
    }
    else
    {
      //We should get response by now, else raising alarm
      LOG_MSG_ERROR( "ds_mux_timer_cb unable to recv msg ack from host %d", dlci_conn_param->curr_transmissions_N2, 0, 0);
      if(dlci_conn_param->curr_msg_frame != NULL && dlci_conn_param->curr_msg_frame->information_ptr != NULL)
      {
        LOG_MSG_INFO1("Deallocating output frame and its information ptr", 0, 0, 0);
        DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr);
        ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr = NULL;
        DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame);
        ds_mux_dlci_conn[dlci_id]->curr_msg_frame = NULL;
      }
    }
  }
  return DS_MUX_SUCCESS_MACRO;
}
int ds_mux_timer_listener_init
(
  ds_mux_sk_fd_set_info_t*   sk_fdset,
  ds_mux_sock_thrd_fd_read_f read_f,
  int                        max_fds
)
{
  if (create_server_socket(&timer_ds_mux_sockfd, SERVER_TIMER_FILE) != DS_MUX_SUCCESS_MACRO)
  {
    LOGI("creating timer_ds_mux_sockfd socket fails");
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Timer Socket Creation SUCCESS!!!");
  }

  if( ds_mux_addfd_map(sk_fdset,timer_ds_mux_sockfd, read_f, max_fds) ==
                                                            DS_MUX_FAILURE_MACRO)
  {
    LOGI("cannot add DSMUX atfwd socket for reading");
    close(timer_ds_mux_sockfd);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Timer Socket mapped to fn SUCCESS!!!");
  }
  return DS_MUX_SUCCESS_MACRO;
}


int ds_mux_listen_frm_timer
(
)
{
  if(ds_mux_timer_listener_init(&sk_fdset, ds_mux_process_t2_timer_ind, DS_MUX_MAX_NUM_OF_FD) == DS_MUX_SUCCESS_MACRO)
  {
    LOGI("Socket to receive timer msgs succeeds");
  }
  else
  {
    LOGI("Socket to receive timer msgs Fails");
    return DS_MUX_FAILURE_MACRO;
  }

  return DS_MUX_SUCCESS_MACRO;
}

void ds_mux_listen_msgs_frm_t2_timer()
{
  int ret;
  ret = ds_mux_listen_frm_timer();
  if (DS_MUX_SUCCESS_MACRO != ret)
  {
     LOGI("unable to listen msgs from Timer, errno: %d",ret, 0, 0);
  }
}

void ds_mux_create_timer_client_socket()
{
  int ret;
  struct sockaddr_un client;

  ret = create_socket(&timer_client_sockfd);
  if(ret == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Failed to create the client timer socket");
  }
  else
  {
    LOGI("Sucessfully created CLIENT timer socket ");
  }

}

ds_mux_result_enum_type ds_mux_timer_alloc
(
  ds_mux_dlci_param_type* dlci_conn,
  t2_timer_handler        ds_mux_timer_expire_handler
)
{
  struct sigaction sa;

  if(DS_MUX_DLCI_ID_0 == dlci_conn->dlci_id)
  {
    t2_timer_data = (ds_mux_timer_common_data *)calloc(1, sizeof(ds_mux_timer_common_data));
    if(NULL == t2_timer_data )
    {
      LOG_MSG_ERROR("Failed to allocate memory t2_timer_data: %p", t2_timer_data, 0, 0);
      return DS_MUX_FAILURE;
    }

    t2_timer_data->user_data = calloc(1,sizeof(uint8));
    if(t2_timer_data->user_data == NULL)
    {
      LOG_MSG_ERROR("Could-not alocate memory. t2_timer_data->user_data: %p", t2_timer_data->user_data, 0, 0);
      if(t2_timer_data != NULL)
      {
        DS_MUX_MEM_FREE(t2_timer_data);
        t2_timer_data = NULL;
      }
      return DS_MUX_FAILURE;
    }

    psc_timer_data = (ds_mux_timer_common_data *)calloc(1, sizeof(ds_mux_timer_common_data));
    if(NULL == psc_timer_data )
    {
      LOG_MSG_ERROR("Failed to allocate memory psc_timer_data: %p", psc_timer_data, 0, 0);
      return DS_MUX_FAILURE;
    }
  }

  memset(&sa, 0, sizeof(struct sigaction));
  memset(&sev, 0, sizeof(struct sigevent));
  memset(&(dlci_conn->t2_timer_data), 0, sizeof(ds_mux_timer_data_t));

  memcpy(t2_timer_data->user_data, &(dlci_conn->dlci_id), sizeof(uint8));
  t2_timer_data->timer_id_type = DS_MUX_T2_TIMER;

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = (void *)(t2_timer_data);

  /* We are creating T2 timer for each DLCI.*/
  if (timer_create(CLOCKID, &sev, &(dlci_conn->t2_timer_data.timer_id))== -1)
  {
    LOG_MSG_ERROR(" Failed to create T2 timer i.e. Response Timer",0,0,0);
    return DS_MUX_FAILURE;
  }

   /*============== Allocating PSC timer=====================*/

  /*
    PSC timer is created in the dlci_con_dlci_id0 and for remaining DLCI,
    the same PSC timer is used
  */

  psc_timer_data->timer_id_type = DS_MUX_PSC_TIMER;

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = (void *)(psc_timer_data);

  if (timer_create(CLOCKID, &sev, &(dlci_conn->psc_timer_data.timer_id))== -1)
  {
    LOG_MSG_ERROR(" Failed to create PSC i.e. power saving Timer",0,0,0);
    return DS_MUX_FAILURE;
  }

  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = ds_mux_timer_expire_handler;

  sigemptyset(&sa.sa_mask);
  if (sigaction(SIG, &sa, NULL) == -1)
  {
    LOG_MSG_ERROR("T2 timer, Sigaction action failed .Error in registering signal handler", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  return DS_MUX_SUCCESS;
}

void ds_mux_timer_dealloc()
{
  LOG_MSG_INFO1("De-allocating Timer", 0, 0, 0);

  if(t2_timer_data != NULL)
  {
    if(t2_timer_data->user_data != NULL)
    {
      DS_MUX_MEM_FREE(t2_timer_data->user_data);
      t2_timer_data->user_data = NULL;
    }
    DS_MUX_MEM_FREE(t2_timer_data);
    t2_timer_data = NULL;
  }

  if(psc_timer_data != NULL)
  {
    if(psc_timer_data->user_data != NULL)
    {
      DS_MUX_MEM_FREE(psc_timer_data->user_data);
      psc_timer_data->user_data = NULL;
    }
    DS_MUX_MEM_FREE(psc_timer_data);
    psc_timer_data = NULL;
  }

}

void ds_mux_inititialize_dlci_conn
(
  ds_mux_dlci_param_type  *dlci_conn,
  uint_32                   dlci_id
)
{
  int                                       timer_ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == dlci_conn)
  {
    LOG_MSG_ERROR( "ds_mux_inititialize_dlci_conn  DLCI conn ptr is NULL ",0, 0, 0);
    return;
  }
  /*-----------------------------------------------------------------------
    Initialize default values if CMUX params are NULL
  -----------------------------------------------------------------------*/
  if ( NULL == ds_mux_cmux_params)
  {
    LOG_MSG_INFO1("ds_mux_cmux_params is NULL. Filling dlci_conn with default values", 0, 0, 0);
    ds_mux_update_dlci_params (dlci_conn,
                               DS_MUX_SUBSET_UIH, DS_MUX_CL_TYPE_1,
                               DS_MUX_DEFAULT_DLCI_PRIORITY,
                               DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1,
                               DS_MUX_CMUX_DEFAULT_FRAME_N1,
                               DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
                               DS_MUX_WINDOW_SIZE_2);
    dlci_conn->response_timer_T2 = DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2;
    dlci_conn->wake_up_timer_T3  = DS_MUX_CMUX_DEFAULT_TIMER_T3;
  }
  else
  {
    LOG_MSG_INFO1("ds_mux_cmux_params is present. So NOT filling dlci_conn with default values", 0, 0, 0);
    ds_mux_update_dlci_params (dlci_conn,
                               ds_mux_cmux_params->subset,
                               DS_MUX_CL_TYPE_1,
                               DS_MUX_DEFAULT_DLCI_PRIORITY,
                               ds_mux_cmux_params->response_timer_T1,
                               ds_mux_cmux_params->frame_size_N1,
                               ds_mux_cmux_params->re_transmissions_N2,
                               ds_mux_cmux_params->window_size_k);

    dlci_conn->response_timer_T2 = ds_mux_cmux_params->response_timer_T2;
    dlci_conn->wake_up_timer_T3  = ds_mux_cmux_params->wake_up_timer_T3;
  }

  dlci_conn->dlci_id               = (uint8)dlci_id;
  dlci_conn->state                 = DS_MUX_DLCI_INITIALIZED;
  dlci_conn->pending_rx_cl4_data_ptr = NULL;
  dlci_conn->flow_control_mask = DS_MUX_FLOW_DISABLE_MIN;

  if(DS_MUX_SUCCESS != ds_mux_timer_alloc(dlci_conn, ds_mux_timer_cb))
  {
    LOG_MSG_ERROR("Error in allocating timer", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO2( "ds_mux_inititialize_dlci_conn initialized dlci_id  %d ", dlci_id, 0, 0);

}/* ds_mux_inititialize_dlci_conn */

ds_mux_result_enum_type  ds_mux_deinitiatize_frame(ds_mux_io_rx_info** parsed_rx_frame_info)
{
  ds_mux_io_rx_info* deref_ptr = *parsed_rx_frame_info;
  if(deref_ptr == NULL)
  {
    LOG_MSG_ERROR("Cannot free the frame info, frame is already NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }
  if(deref_ptr->rx_frame != NULL)
  {
    if(deref_ptr->rx_frame->information_ptr != NULL)
    {
      LOG_MSG_INFO1("Deallocating Information ptr of frame", 0, 0, 0);
      DS_MUX_MEM_FREE(deref_ptr->rx_frame->information_ptr);
      deref_ptr->rx_frame->information_ptr == NULL;
    }
    else
    {
      LOG_MSG_ERROR("info ptr is already NULL", 0, 0, 0);
    }
    LOG_MSG_INFO1("Deallocating the whole frame", 0, 0, 0);
    DS_MUX_MEM_FREE(deref_ptr->rx_frame);
    deref_ptr->rx_frame = NULL;
  }
  else
  {
    LOG_MSG_ERROR("Rx Frame is NULL", 0, 0, 0);
  }

  LOG_MSG_INFO1("Deallocating the parsed frame", 0, 0, 0);
  DS_MUX_MEM_FREE(deref_ptr);
  *parsed_rx_frame_info = NULL;
  return DS_MUX_SUCCESS;
}



ds_mux_dlci_param_type*  ds_mux_get_alloc_conn_ptr
(
  uint_32                   dlci_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( IS_DLCI_INVALID( dlci_id) )
  {
    LOG_MSG_ERROR( "ds_mux_get_alloc_conn_ptr Invalid DLCI ID %d", dlci_id,0, 0);
    return NULL;
  }

  /*-----------------------------------------------------------------------
     If DLCI is not allocate then allocate otherwise return previously allocated pointer
  -----------------------------------------------------------------------*/
  LOG_MSG_INFO1("*DLCI ID* : %d", dlci_id, 0, 0);
  if ( NULL == ds_mux_dlci_conn[dlci_id])
  {
    LOG_MSG_INFO1("dlci_conn DOSE-NOT exist for %d. Allocating and initializing dlci_conn",
                   dlci_id, 0, 0);
    ds_mux_dlci_conn[dlci_id] = (ds_mux_dlci_param_type*)malloc(sizeof(ds_mux_dlci_param_type));
    if(ds_mux_dlci_conn[dlci_id] == NULL)
    {
       LOG_MSG_ERROR("Could not allocate memory for ds_mux_dlci_conn", 0, 0, 0);
       return NULL;
    }
    memset(ds_mux_dlci_conn[dlci_id], 0, sizeof(ds_mux_dlci_param_type));
    ds_mux_inititialize_dlci_conn( ds_mux_dlci_conn[dlci_id],  dlci_id);

    LOG_MSG_INFO1( " ds_mux_get_alloc_conn_ptr Allocating dlci id %d conn ptr %x ",
      dlci_id, ds_mux_dlci_conn[dlci_id],0);

  }
  else
  {
    LOG_MSG_INFO1("dlci_conn ALREADY EXIST for %d.Returning it", dlci_id, 0, 0);
  }
  return ds_mux_dlci_conn[dlci_id];
}/* ds_mux_get_alloc_conn_ptr */

ds_mux_result_enum_type ds_mux_get_msg_frame_type
(
  uint8    input_frame_type,
  ds_mux_subset_enum_type*   output_fram_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (input_frame_type)
  {
     case 0:
        *output_fram_type = DS_MUX_SUBSET_UIH;
        break;
     case 1:
        *output_fram_type =  DS_MUX_SUBSET_UI;
        break;
     default:
        LOG_MSG_ERROR("input frame type is INVALID", 0, 0, 0);
        result   = DS_MUX_FAILURE;
  }

  LOG_MSG_INFO2( " ds_mux_get_msg_frame_type input frame type %d output fram type %d ",
    input_frame_type, *output_fram_type, 0);

  return result;
}/* ds_mux_get_msg_frame_type */

ds_mux_result_enum_type ds_mux_get_msg_cl_type
(
  uint8    input_cl_type,
  ds_mux_cl_type_enum_type*   output_cl_type
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (input_cl_type)
  {
    case 0:
      *output_cl_type = DS_MUX_CL_TYPE_1;
    break;
    case 1:
      *output_cl_type = DS_MUX_CL_TYPE_2;
    break;
    case 2:
      *output_cl_type = DS_MUX_CL_TYPE_3;
    break;
    case 3:
      *output_cl_type = DS_MUX_CL_TYPE_4;
    break;
    default:
        result   = DS_MUX_FAILURE;
  }

  LOG_MSG_INFO2( " ds_mux_get_msg_frame_type input_cl_type %d output_cl_type %d ",
    input_cl_type, *output_cl_type, 0);

  return result;
}/* ds_mux_get_msg_frame_type */

void ds_mux_update_dlci_params
(
  ds_mux_dlci_param_type     *dlci_conn,
  ds_mux_subset_enum_type     frame_type,
  ds_mux_cl_type_enum_type    cl_type,
  uint8                       priprity,
  uint16                      response_timer_T1,
  uint16                      frame_size_N1,
  uint8                       re_transmissions_N2,
  uint8                       window_size_k
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dlci_conn->frame_type          = frame_type;
  dlci_conn->cl_type             = cl_type;
  dlci_conn->priprity            = priprity;
  dlci_conn->response_timer_T1   = response_timer_T1;
  dlci_conn->frame_size_N1       = frame_size_N1;
  dlci_conn->re_transmissions_N2 = re_transmissions_N2;
  dlci_conn->window_size_k       = window_size_k;

  LOG_MSG_INFO2( "ds_mux_update_dlci_params frame type %d "
                          "cl type %d priority %d",
                           dlci_conn->frame_type,
                           dlci_conn->cl_type,
                           dlci_conn->priprity );
  LOG_MSG_INFO2("ds_mux_update_dlci_params frame type T1: %d, N1: %d, N2: %d",
                           dlci_conn->response_timer_T1,
                           dlci_conn->frame_size_N1,
                           dlci_conn->re_transmissions_N2 );

}/* ds_mux_update_dlci_params*/

ds_mux_result_enum_type ds_mux_process_test_msg_resp
(
  void
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported
  }while(0);


  return result;
}/* ds_mux_process_test_msg_resp */

ds_mux_result_enum_type ds_mux_process_fc_on_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_FCON;
  char*                         resp_msg         = NULL;
  char*                         tx_pkt           = NULL;
  uint8                         dlci_id          = DS_MUX_DLCI_ID_1;
  uint16                        len              = 0;
  uint16                        idx              = 0;

  /*-----------------------------------------------------------------------
      Process FLow Control ON  command and prepare FCON message response.
      Enable flow on all DLCIs. 5.4.6.3.5	Flow Control On Command (FCon)
      The flow control command is used to handle the aggregate flow. When either entity
      is able to receive new information it transmits this command.
    -----------------------------------------------------------------------*/
  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp: %d msg_rsp_frame: %d", send_msg_resp, msg_rsp_frame, 0);
    return DS_MUX_FAILURE;
  }
  do
  {

    if(msg_rsp_frame->information_ptr != NULL)
    {
      memset(msg_rsp_frame->information_ptr, 0, DS_MUX_CTL_FRM_SZ-DS_MUX_MIN_FRAME_SZ);
      msg_rsp_frame->information_ptr[idx++]= msg_type;
      len++;
      msg_rsp_frame->information_ptr[idx]= msg_len;
      len++;
      LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);

      for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
      {
        ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_FCON );
      }
    }
  }while(0);

  msg_rsp_frame->length = len;
  *send_msg_resp = TRUE;
  return result;
}/*ds_mux_process_fc_on_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_fc_on_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem flow control  is not supported
  }while(0);


  return result;
}/* ds_mux_process_fc_on_msg_resp */

ds_mux_result_enum_type ds_mux_process_fc_off_msg_resp
(
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem flow control  is not supported
  }while(0);

  return result;
}/* ds_mux_process_fc_off_msg_resp */

ds_mux_result_enum_type ds_mux_process_fc_off_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_FCOFF;
  char*                         resp_msg         = NULL;
  char*                         tx_pkt           = NULL;
  uint8                         dlci_id          = DS_MUX_DLCI_ID_1;
  uint16                        len              = 0;
  uint16                        idx              = 0;

  /*-----------------------------------------------------------------------
      Process FLow Control OFF  command and prepare FCOFF message response.
      Disable flow on all DLCIs. except DLCI 0 5.4.6.3.6	Flow Control Off Command (FCoff)
      The flow control command is used to handle the aggregate flow. When either entity is
      not able to receive information it transmits the FCoff command.
    -----------------------------------------------------------------------*/

  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp:%p msg_rsp_frame:%p", send_msg_resp, msg_rsp_frame, 0);
    return DS_MUX_FAILURE;
  }

  do
  {

    if(msg_rsp_frame->information_ptr != NULL)
    {
      memset(msg_rsp_frame->information_ptr, 0, DS_MUX_CTL_FRM_SZ-DS_MUX_MIN_FRAME_SZ);
      msg_rsp_frame->information_ptr[idx++]= msg_type;
      len++;
      msg_rsp_frame->information_ptr[idx]= msg_len;
      len++;
      LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);

      for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
      {
        ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_FCON );
      }
    }
  }while(0);

  msg_rsp_frame->length = len;
  *send_msg_resp = TRUE;

  return result;
}/*ds_mux_process_fc_off_msg_cmd*/


ds_mux_result_enum_type ds_mux_handle_remote_fc_enable
(
  uint8                         dlci_id
)
{
  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;

  /*-----------------------------------------------------------------------------------------
    This Function will be invoked when remote mux/host send flow control enable in MSC message or
    flow control enable in first Byte of data(CL- 2.).
    Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
    -------------------------------------------------------------------------------------------*/
  ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_MSC );

  return result;
}/* ds_mux_logical_handle_remote_fc_enable */

ds_mux_result_enum_type ds_mux_handle_remote_fc_disable
(
  uint8                         dlci_id
)
{
  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;

  /*------------------------------------------------------------------------------------------------
    This Function will be invoked when remote mux/host send flow controldisaable in MSC message or
    flow control disable in first Byte of data(CL- 2.).
    Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
    --------------------------------------------------------------------------------------------*/
  ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_MSC );

  return result;
}/* ds_mux_logical_handle_remote_fc_disable */



ds_mux_result_enum_type  ds_mux_logical_process_v24_signal
(
  uint8                  dlci_id,
  uint8                  v24_sig
)
{

  ds_mux_result_enum_type       result          = DS_MUX_SUCCESS;
  boolean                       recv_enable_fc  = FALSE;
  boolean                       recv_enable_rtc = FALSE;
  boolean                       recv_enable_dv  = FALSE;
  boolean                       recv_enable_ic  = FALSE;

  int                           sig = 0;

  /*-----------------------------------------------------------------------
    1.  Parse V24 signal byte and check signal bit
    2.  If signal changed then update the signal status and invoke client call back (if registered  )

        Supported Bits:-

        Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
        Bit 3. Ready To Communicate (RTC). The bit is set to 1 when the device is ready to
               communicate.
        Bit 7. Incoming call indicator (IC). The bit is set to 1 to indicate an incoming call.
        Bit 8. Data Valid (DV). The bit is set to 1 to indicate that valid data is being sent
    -----------------------------------------------------------------------*/
  do
  {
    if ( 0 == (v24_sig & DS_MUX_DLCI_MSG_TYPE_EA_MASK))
    {
       DS_MUX_SET_ERROR_AND_BREAK(result);
       LOG_MSG_ERROR("EA mask not set. Its error", 0, 0, 0);
    }

    /* Step 1 */
    if ( (v24_sig & DS_MUX_DLCI_MSC_FC_MASK) )
    {
      recv_enable_fc = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_RTC_MASK) )
    {
      recv_enable_rtc = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_IC_MASK) )
    {
      recv_enable_ic = TRUE;
    }
    if ( (v24_sig & DS_MUX_DLCI_MSC_DV_MASK) )
    {
      recv_enable_dv = TRUE;
    }

    if ( TRUE == recv_enable_fc)
    {
      LOG_MSG_INFO1("MSC FC SET", 0, 0, 0);
      ds_mux_handle_remote_fc_enable( dlci_id);
    }
    else
    {
      LOG_MSG_INFO1("MSC FC is NOT SET", 0, 0, 0);
      ds_mux_handle_remote_fc_disable( dlci_id);
    }

    LOG_MSG_INFO1("dlci id : %d  v24_sig: %d", dlci_id, v24_sig, 0);
    if( TRUE == recv_enable_rtc)
    {
      LOG_MSG_INFO1("Setting DTR HIGH", 0, 0, 0);
      sig = sig | TIOCM_DTR;
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);
    }
    else
    {
       LOG_MSG_INFO1("setting DTR LOW", 0, 0, 0);

          sig = sig | (~ TIOCM_DTR);
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);

    }

    if( TRUE == recv_enable_ic)
    {
      LOG_MSG_INFO1("Setting Incoming call indicator to high", 0, 0, 0);
      sig = sig | TIOCM_CD;
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);
    }
    else
    {
      LOG_MSG_INFO1("Setting incoming call indicator is LOW", 0, 0, 0);
      sig = sig | TIOCM_CD;
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);
    }

    if( TRUE == recv_enable_dv)
    {
      LOG_MSG_INFO1("Setting DATA VALID bit to HIGH", 0, 0, 0);
      sig = sig | TIOCM_RI;
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);

    }
    else
    {
      LOG_MSG_INFO1("Setting DATA valid bit to low", 0, 0, 0);
      sig = sig | TIOCM_RI;
      if(dlci_id == DS_MUX_DLCI_ID_1)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_2)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], TIOCMSET, (void*)&sig);
      else if(dlci_id == DS_MUX_DLCI_ID_3)
        ioctl(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], TIOCMSET, (void*)&sig);

    }
  }while(0);

  return result;

}

ds_mux_result_enum_type ds_mux_process_msc_msg_resp
(
  char*                         msg_ptr_ptr,
  byte                          msg_len
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         dlci_id          = DS_MUX_DLCI_ID_0;
  ds_mux_dlci_param_type       *dlci_conn_param  = NULL;
  uint8                         msg_info[DS_MUX_DLCI_MSC_VAL_LEN];
  uint8                         msg_info_idx = 0;
  uint16                        idx=0;
  uint16                        cmd_idx = 0;
  uint16                        idx_msg_info = 0;
  byte                          msg_length;
  uint8                         dlci_id_ea_cr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
      Process MSC message response and send outstanding message from queue
    -----------------------------------------------------------------------*/
    if(msg_ptr_ptr == NULL)
    {
      LOG_MSG_ERROR("msg_ptr_ptr is NULL", 0, 0, 0);
      return DS_MUX_FAILURE;
    }
    //Verify message length
    if( DS_MUX_DLCI_MSC_VAL_LEN != msg_len )
    {
      LOG_MSG_ERROR("MSC response len is incorrect", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

    //Pullout DLCI ID
    idx=idx+2;
    dlci_id_ea_cr = msg_ptr_ptr[idx++];
    LOG_MSG_INFO1("DLCI ID with ea n cr bit %X", dlci_id_ea_cr, 0, 0);


    //Pull out remaining bytes
    msg_info[msg_info_idx++] = msg_ptr_ptr[idx++];
    msg_info[msg_info_idx] = msg_ptr_ptr[idx++];

    //Remove EA and CR bit
    dlci_id = dlci_id_ea_cr >> 2;
    LOG_MSG_INFO1("dlci id  %d", dlci_id, 0, 0);

    if(IS_DLCI_INVALID(dlci_id))
    {
      LOG_MSG_ERROR("Invalid DLCI ID ", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

    dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

    if ( NULL == dlci_conn_param )
    {
      LOG_MSG_ERROR("DLCI_CONN is NULL", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

    if(dlci_conn_param->curr_msg_frame != NULL)
    {
      if(dlci_conn_param->curr_msg_frame->information_ptr != NULL)
      {
        //To get DLCI ID
        cmd_idx = cmd_idx+2;
        if( (dlci_conn_param->curr_msg_frame->information_ptr[cmd_idx]) == dlci_id_ea_cr)
        {
          LOG_MSG_INFO1("dlci_ea_cr is correct in MSC resp %d  dlci_id_ea_cr: %d",
                        dlci_conn_param->curr_msg_frame->information_ptr[cmd_idx], dlci_id_ea_cr, 0);
        }
        else
        {
          LOG_MSG_ERROR("DLCI ID is NOT correct in MSC resp %d  dlci_id_ea_cr: %d",
                        dlci_conn_param->curr_msg_frame->information_ptr[cmd_idx], dlci_id_ea_cr, 0);
          return DS_MUX_FAILURE;
        }

        cmd_idx++;

        if(dlci_conn_param->curr_msg_frame->information_ptr[cmd_idx] == msg_info[0])
        {
          LOG_MSG_INFO1("is_running: %d", dlci_conn_param->t2_timer_data.is_running, 0, 0);
          if(dlci_conn_param->t2_timer_data.is_running == TRUE)
          {
            ds_mux_timer_stop( &dlci_conn_param->t2_timer_data);
          }
          LOG_MSG_INFO1("MSC resp's signal byte is correct", 0, 0, 0);
          //Mark as MSC reponse is received.
          msc_resp_rcvd = TRUE;
        }
        else
        {
          LOG_MSG_INFO1("Signal byte is INCORRECT in MSC Resp", 0, 0, 0);
          return DS_MUX_FAILURE;
        }
      }
      else
      {
        LOG_MSG_ERROR("MSC command does-not have info field", 0, 0, 0);
        return DS_MUX_FAILURE;
      }
    }
    else
    {
      LOG_MSG_INFO1("MSC command not present", 0, 0, 0);
    }
  }while(0);

  return result;
}/*ds_mux_process_msc_msg_resp */

ds_mux_result_enum_type ds_mux_process_msc_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_MSC;
  uint8                         dlci_id          = 0;
  uint8                         v24_sig          = 0;
  uint8                         break_sig        = 0;
  uint8                         index_msg_ptr_ptr = 0;
  uint8                         len                =0;
  uint8                         idx                =0;
  uint8                         msg_ptr_ptr_idx    =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp:%p msg_rsp_frame: %p", send_msg_resp, msg_rsp_frame, 0);
    return DS_MUX_FAILURE;
  }

  if(msg_rsp_frame->information_ptr == NULL)
  {
    LOG_MSG_ERROR("msg_rsp_frame->informartion_ptr: %p", msg_rsp_frame->information_ptr, 0, 0);
    return DS_MUX_FAILURE;
  }
  do
  {
    msg_rsp_frame->information_ptr[idx++]= msg_type;
    len++;
    msg_rsp_frame->information_ptr[idx++]= msg_len;
    len++;
    LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);

    //Verify length
    if ( 2 != msg_len && 3 != msg_len)
    {
      LOG_MSG_ERROR("msg_len of MSC command is neither 2 or 3 bytes", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

    //increment the index by 2 so that u get DLCI ID in the info field
    msg_ptr_ptr_idx = msg_ptr_ptr_idx +2;
    dlci_id = msg_ptr_ptr[msg_ptr_ptr_idx++];

    //Insert DLCI ID
    msg_rsp_frame->information_ptr[idx++]= dlci_id;
    len++;

    //Remove Lower two bits from DLCI ID
    dlci_id = dlci_id >> 2;

    LOG_MSG_INFO1("length is %d", msg_len, 0, 0);
    //Pull out V24 signal byte
    if ( 2 == msg_len)
    {
      v24_sig = msg_ptr_ptr[msg_ptr_ptr_idx++];

      msg_rsp_frame->information_ptr[idx++]= v24_sig;
      len++;

    }
    else if ( 3 == msg_len )
    {

      v24_sig = msg_ptr_ptr[msg_ptr_ptr_idx++];

      msg_rsp_frame->information_ptr[idx++]= v24_sig;
      len++;

      break_sig = msg_ptr_ptr[msg_ptr_ptr_idx++];
      msg_rsp_frame->information_ptr[idx++]= break_sig;
      len++;

    }

    //Process DLCI signal byte
    ds_mux_logical_process_v24_signal( dlci_id, v24_sig);


  }while(0);

  LOG_MSG_INFO1( " ds_mux_process_msc_msg_cmd dlci id %d v24 signal byte %d len %d",
      dlci_id, v24_sig, msg_len);

  LOG_MSG_INFO1(" ds_mux_process_msc_msg_cmd result: %d", result,0,0);

  if ( DS_MUX_FAILURE == result)
  {
    *send_msg_resp = FALSE;
  }
  else
  {
     msg_rsp_frame->length = len;
    *send_msg_resp = TRUE;
  }

  return result;
}/*ds_mux_process_msc_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_test_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_TEST;
  char*                         dst              = NULL;
  uint8                         len              = 0;
  uint8                         idx              = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp:%d  msg_resp_frame:%d", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  do
  {

    if(msg_rsp_frame->information_ptr != NULL)
    {
      if(msg_ptr_ptr != NULL)
      {
        msg_rsp_frame->information_ptr[idx++]= msg_type;
        len++;
        msg_rsp_frame->information_ptr[idx++]= msg_len;
        len++;
        LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);

        dst = msg_rsp_frame->information_ptr + idx;
        memcpy(dst, msg_ptr_ptr, msg_len);
        len = len + msg_len;
        idx = idx + msg_len - 1;
      }
      else
      {
        msg_rsp_frame->information_ptr[idx++]= msg_type;
        len++;
        msg_rsp_frame->information_ptr[idx]= msg_len;
        len++;
        LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);
      }
    }
    else
    {
      LOG_MSG_ERROR("information field is NULL", 0, 0, 0);
      return DS_MUX_FAILURE;
    }

  }while(0);

   msg_rsp_frame->length = len;
  *send_msg_resp = TRUE;

  return result;
}

ds_mux_result_enum_type ds_mux_process_cld_msg_resp
(
  void
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported
  }while(0);


  return result;
}/* ds_mux_process_psc_msg_resp */


ds_mux_result_enum_type ds_mux_process_cld_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result            = DS_MUX_SUCCESS;
  uint8                         msg_type          = DS_MUX_DLCI_FRAME_VAL_CLD;
  uint8                         len                =0;
  uint8                         idx                =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
      Process CLD message command and prepare CLD message response.
      Note: CLD processing will be done after sending CLD response in process rx message
    -----------------------------------------------------------------------*/
  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp: %p msg_resp_frame: %p", 0, 0, 0);
    return DS_MUX_FAILURE;
  }
  do
  {

    if(msg_rsp_frame->information_ptr != NULL)
    {
      memset(msg_rsp_frame->information_ptr, 0, DS_MUX_CTL_FRM_SZ-DS_MUX_MIN_FRAME_SZ);
      msg_rsp_frame->information_ptr[idx++]= msg_type;
      len++;
      msg_rsp_frame->information_ptr[idx]= msg_len;
      len++;
      LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);
    }

  }while(0);

  msg_rsp_frame->length = len;
  *send_msg_resp = TRUE;
  return result;
}/*ds_mux_process_cld_msg_cmd*/


ds_mux_result_enum_type ds_mux_process_psc_msg_resp
(
  void
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    //TBD::  modem initiated power save is not supported

  }while(0);

  return result;
}/* ds_mux_process_psc_msg_resp */

ds_mux_result_enum_type ds_mux_init_wakeup_process(uint8 dlci)
{
  uint8                         dlci_id;
  ds_mux_dlci_param_type *      dlci_conn_param = NULL;
  ds_mux_dlci_param_type *      dlci_conn_param_dlci0 = NULL;
  uint8                         wakeup_flag[6] = {0xF9,0xF9,0xF9,0xF9,0xF9,0xF9};
  int ret;

  for(dlci_id = DS_MUX_DLCI_ID_1; dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++)
  {
    ds_mux_enable_flow_mask(dlci_id, DS_MUX_FLOW_DISABLE_PSC);
  }
    /*Wake up flag of 6 bytes is sent. Any number of flag greater than 1 can be sent.*/
  ds_mux_uart_tty_send_msg(wakeup_flag, 6);
  ds_mux_set_mux_state(DS_MUX_WAIT_FOR_WAKEUP_FLAG);

  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci);
  if(dlci_conn_param == NULL)
  {
    LOG_MSG_ERROR("dlciconn is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  dlci_conn_param_dlci0 = ds_mux_get_dlci_conn_ptr(DS_MUX_DLCI_ID_0);
  if(dlci_conn_param_dlci0 == NULL)
  {
    LOG_MSG_ERROR("dlciconn is NULL", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  //start timer
  dlci_conn_param_dlci0->psc_timer_data.is_running = TRUE;
  ret = ds_mux_timer_start(&dlci_conn_param_dlci0->psc_timer_data, dlci_conn_param->wake_up_timer_T3, DS_MUX_PSC_TIMER);
  if(ret != DS_MUX_SUCCESS_MACRO)
  {
    LOG_MSG_ERROR("Failed to start timer", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  return DS_MUX_SUCCESS;

}

ds_mux_result_enum_type ds_mux_process_psc_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result            = DS_MUX_SUCCESS;
  uint8                         msg_type          = DS_MUX_DLCI_FRAME_VAL_PSC;
  uint8                         len               = 0;
  uint8                         idx               = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
      Process PSC message command and prepare PSC message response.
      Note: PSC processing will be done after sending PSC response in process rx message
    -----------------------------------------------------------------------*/
    if(send_msg_resp == NULL || msg_rsp_frame == NULL)
    {
      LOG_MSG_ERROR("send_msg_resp: %d  msg_rsp_frame: %p", 0, 0, 0);
      return DS_MUX_FAILURE;
    }

    //Insert Message type
    msg_rsp_frame->information_ptr[idx++] = msg_type;
    len++;

    msg_rsp_frame->information_ptr[idx] = msg_len;
    len++;

  }while(0);

  msg_rsp_frame->length = len;
  *send_msg_resp = TRUE;

  return result;
}/*ds_mux_process_psc_msg_cmd*/



ds_mux_result_enum_type ds_mux_process_pn_msg_cmd
(
  char*                         msg_ptr_ptr,
  byte                          msg_len,
  ds_mux_io_frame_type*         msg_rsp_frame,
  boolean*                      send_msg_resp
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type*       dlci_conn_param  = NULL;
  uint8                         msg_type         = DS_MUX_DLCI_FRAME_VAL_PN;
  uint8                         dlci_id          = 0;
  uint8                         oct2             = 0;
  uint8                         priority_oct3    = 0;
  uint8                         T1_oct4          = 0;
  uint8                         N1_oct5          = 0;
  uint8                         N1_oct6          = 0;
  uint16                        N1               = 0;
  uint8                         N2_oct7          = 0;
  uint8                         K_oct8           = 0;
  ds_mux_subset_enum_type       req_frame_type   = DS_MUX_SUBSET_MIN;
  ds_mux_cl_type_enum_type      req_cl_type      = DS_MUX_CL_TYPE_INVALID;
  uint8                         index_msg_ptr_ptr= 0;
  uint8                         len              = 0;
  uint8                         idx              = 0;

  if(send_msg_resp == NULL || msg_rsp_frame == NULL)
  {
    LOG_MSG_ERROR("send_msg_resp:%p, msg_rsp_frame: %p", send_msg_resp, msg_rsp_frame, 0);
    return DS_MUX_FAILURE;
  }

  do
  {

    if(msg_rsp_frame->information_ptr != NULL)
    {
      msg_rsp_frame->information_ptr[idx++]= msg_type;
      len++;
      msg_rsp_frame->information_ptr[idx++]= msg_len;
      len++;
      LOG_MSG_INFO1("len: %d msg_type: %d", msg_len, msg_type, 0);

      /* Right shift one bit to remove EA*/
      msg_len = msg_len >> 1;
      //Validate length
      LOG_MSG_INFO1("msg_len: %d", msg_len, 0, 0);
      if ( DS_MUX_PN_MSG_LEN != msg_len)
        DS_MUX_SET_ERROR_AND_BREAK(result);

      //Parse DLCI
      //To get the dlci_type index_msg_ptr_ptr is incremented by 2
      index_msg_ptr_ptr = index_msg_ptr_ptr +2;
      dlci_id = msg_ptr_ptr[index_msg_ptr_ptr];
      LOG_MSG_INFO1("dlci_id: %d", dlci_id, 0, 0);

      //Insert DLCI ID
      msg_rsp_frame->information_ptr[idx++] = dlci_id;
      len++;

      //Remove Highest two bits
      dlci_id = dlci_id & 0x3F;
      LOG_MSG_INFO1("dlci_id : %d", dlci_id, 0, 0);

      dlci_conn_param = ds_mux_get_alloc_conn_ptr(dlci_id);
      //If DLCI is already connected then we should ignore parameter  negotition.
      if ( NULL == dlci_conn_param || dlci_conn_param->state != DS_MUX_DLCI_INITIALIZED)
      {
        LOG_MSG_ERROR("dlci_conn_param: %p", dlci_conn_param ,0, 0);
        DS_MUX_SET_ERROR_AND_BREAK(result);
      }

      //Frame type and Covergence layer type
      index_msg_ptr_ptr++;
      oct2 = msg_ptr_ptr[index_msg_ptr_ptr];

      LOG_MSG_INFO1("oct2: %X", oct2, 0, 0);


      msg_rsp_frame->information_ptr[idx++] = oct2;
      len++;

      if ( DS_MUX_SUCCESS != ds_mux_get_msg_frame_type( (oct2 & 0x0F), &req_frame_type))
        DS_MUX_SET_ERROR_AND_BREAK(result)
      if ( DS_MUX_SUCCESS != ds_mux_get_msg_cl_type( oct2>>4, &req_cl_type))
        DS_MUX_SET_ERROR_AND_BREAK(result)

      //Priority
      index_msg_ptr_ptr++;
      priority_oct3 = msg_ptr_ptr[index_msg_ptr_ptr];

      LOG_MSG_INFO1("prio: %X", priority_oct3, 0, 0);

      msg_rsp_frame->information_ptr[idx++] = priority_oct3;
      len++;

      //Acknowledgement timer
      index_msg_ptr_ptr++;
      T1_oct4 = msg_ptr_ptr[index_msg_ptr_ptr];

      LOG_MSG_INFO1("t1 %x", T1_oct4, 0, 0);

      msg_rsp_frame->information_ptr[idx++] = T1_oct4;
      len++;

      //maximum frame size
      index_msg_ptr_ptr++;
      N1_oct5 = msg_ptr_ptr[index_msg_ptr_ptr];

      LOG_MSG_INFO1("N1 %x", N1_oct5, 0, 0);

      msg_rsp_frame->information_ptr[idx++] = N1_oct5;
      len++;

      //N1
      index_msg_ptr_ptr++;
      N1_oct6 = msg_ptr_ptr[index_msg_ptr_ptr];


      msg_rsp_frame->information_ptr[idx++] = N1_oct6;
      len++;

      N1 = N1_oct6 << 8 | N1_oct5;

      //maximum number of retransmissions
      index_msg_ptr_ptr++;
      N2_oct7 = msg_ptr_ptr[index_msg_ptr_ptr];

      msg_rsp_frame->information_ptr[idx++] = N2_oct7;
      len++;


      //Window size
      index_msg_ptr_ptr++;
      K_oct8 = msg_ptr_ptr[index_msg_ptr_ptr];


      msg_rsp_frame->information_ptr[idx] = K_oct8;
      len++;

      LOG_MSG_INFO1("k %x", K_oct8, 0, 0);

      ds_mux_update_dlci_params (dlci_conn_param,
                                 req_frame_type,
                                 req_cl_type,
                                 priority_oct3,
                                 T1_oct4,
                                 N1,
                                 N2_oct7,
                                 K_oct8);

      LOG_MSG_INFO1("dlci_conn_param: %p,ds_mux_dlci_conn[dlci_id]: %p",
                    dlci_conn_param,
                    ds_mux_dlci_conn[dlci_id],
                    0);
    }
    else
    {
      LOG_MSG_ERROR("Information field is NULL", 0, 0, 0);
      return DS_MUX_FAILURE;
    }
  }while(0);

  LOG_MSG_INFO2("ds_mux_process_pn_msg_cmd result %d dlci id %d ", result, dlci_id, 0);

  if ( DS_MUX_SUCCESS != result)
  {
    LOG_MSG_ERROR("Failed to process PN command", 0, 0, 0);
    *send_msg_resp = FALSE;
  }
  else
  {
    LOG_MSG_INFO1("Prepared information field of Parameter-negotiation response", 0, 0, 0);
    msg_rsp_frame->length = len;
    *send_msg_resp = TRUE;
  }
  return result;
}/*ds_mux_process_pn_msg_cmd*/

ds_mux_result_enum_type ds_mux_process_pn_msg_resp
(
  char*                         msg_ptr_ptr,
  byte                          msg_len
)
{
  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  uint8                         dlci_id          = 0;
  ds_mux_dlci_param_type       *dlci_conn_param  = NULL;
  uint8                         index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  msg_len = msg_len >> 1;
  do
  {
    if ( DS_MUX_PN_MSG_LEN != msg_len)
      DS_MUX_SET_ERROR_AND_BREAK(result)

    dlci_id = msg_ptr_ptr[index];

    //Reduce the count as DLCI has taken out
    msg_len = msg_len - 1;

    /*--------------------------------------------------------------------------------
      Note: We are not taking care of re-negotiation and Expecting host has accpeted
      the requested parameters Take out remining params
     -------------------------------------------------------------------------------*/

    //Remove Highest two bits
    dlci_id = dlci_id & 0x3F;

    dlci_conn_param = ds_mux_get_alloc_conn_ptr(dlci_id);

    if ( NULL == dlci_conn_param )
    {
      LOG_MSG_ERROR("DLCI param is NULL", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

  }while(0);

  return result;
}/*ds_mux_process_pn_msg_resp*/

void  ds_mux_disable_flow_mask
(
    uint8                        dlci_id,
  ds_mux_flow_ctl_mask_enum_type fc_msk
)
{
  ds_mux_dlci_param_type        *dlci_conn_param = NULL;
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);

  if ( NULL == dlci_conn_param )
  {
    return;
  }

  LOG_MSG_INFO1("ds_mux_disable_flow_mask dlci_id %d fc_msk  %x current flow mask %x",
                dlci_id, fc_msk,
                dlci_conn_param->flow_control_mask);

  dlci_conn_param->flow_control_mask |= fc_msk;

}/* ds_mux_disable_flow_mask */

ds_mux_result_enum_type ds_mux_process_rx_msgs_frames
(
  ds_mux_io_frame_type*  frame
)
{

  ds_mux_result_enum_type       result           = DS_MUX_SUCCESS;
  char*                         msg_ptr_ptr      = NULL;
  boolean                       processing_msg   = TRUE;
  byte                          msg_byte         = 0;
  byte                          msg_len_with_ea  = 0;
  ds_mux_msg_type_enum_type     msg_type         = DS_MUX_MSG_TYPE_VAL_INVALID;
  boolean                       is_command       = FALSE;
  ds_mux_io_frame_type          msg_rsp_frame;
  boolean                       send_msg_resp   =  FALSE;
  uint8                         dlci_id;
  int                           index=0;


  /*---------------------------------------------------------------------------------------
     DLCI 0 is control channel, UI and UIH frames are used for sending messages over DLCI 0
   ----------------------------------------------------------------------------------------*/
  if ( (NULL == frame )|| (DS_MUX_DLCI_ID_0 != frame->dlci) ||
       (NULL == frame->information_ptr ))
  {
    return DS_MUX_FAILURE;
  }

  /*-----------------------------------------------------------------------
    Initialize message response frame. info field will be updated as part of message processing
   -----------------------------------------------------------------------*/
  memset(&msg_rsp_frame, 0, sizeof(ds_mux_io_frame_type));
  msg_rsp_frame.information_ptr = NULL;

 //The information pointer is deallocated after sending the frame to UART in FN: ds_mux_io_transmit_msg_response
  msg_rsp_frame.information_ptr = (char *)calloc(1, (DS_MUX_CTL_FRM_SZ - DS_MUX_MIN_FRAME_SZ + DS_MUX_FEW_EXTRA_BYTES));


  if(msg_rsp_frame.information_ptr == NULL)
  {
   LOG_MSG_ERROR("Could-Not allocate memory %d", 0, 0, 0);
   return DS_MUX_FAILURE;
  }

  ds_mux_prepare_msg_response( frame, &msg_rsp_frame);

  msg_ptr_ptr = frame->information_ptr;

  if(msg_rsp_frame.information_ptr == NULL)
   LOG_MSG_INFO1("NULL ino ptr", 0, 0, 0);

  /*---------------------------------------------------------------------------------------
      Loop till no data left or processing aborted due to error or processing is complete
   ------------------------------------------------------------------------------------------*/
  if (TRUE == processing_msg)
  {
    do
    {
     /*--------------------------------------------------------------------------
       1  Pull out first byte and convert message byte into DS MUX message type
          1.1 Check if this is command message or response message
       2. Pull out the length byte (All supported messgae has length 1 byte
          only hence not supporting extnd lenght)
       3. Info byte can be optional if length byte is 0
    -----------------------------------------------------------------------------*/
       msg_byte = ( byte )msg_ptr_ptr[index];
       LOG_MSG_INFO1("msg_byte: %d", msg_byte, 0, 0);

      if ( DS_MUX_SUCCESS != ds_mux_get_msg_type( msg_byte, &msg_type) )
      {
        LOG_MSG_ERROR("could not obtain msg_type", 0, 0, 0);
        result = DS_MUX_FAILURE;
        processing_msg = FALSE;
        break;
      }

       is_command = ( DS_MUX_MSG_RESP != (msg_byte & DS_MUX_DLCI_MSG_TYPE_CR_MASK)) ? TRUE : FALSE;
       index++; //fetch next index
       msg_len_with_ea =(byte) msg_ptr_ptr[index];
       LOG_MSG_INFO2(" ds_mux_process_rx_msgs_frames is_command %d msg_type %d len (with EA) %d",
                    is_command, msg_type, msg_len_with_ea);

     /*--------------------------------------------------------------------------------------
     Message processing can be done in three ways
     1. Message can be processed Synchronously. Generate the response messages and
        send them at end of this function call
     2. Message will be processed Asynchronously(later).Generate the response messages and
        send them at end of this function call to the host.
     3. Message processing and response message generation will happen Asynchronously(later).
     ------------------------------------------------------------------------------------------*/
       switch ( msg_type)
       {
         case DS_MUX_MSG_TYPE_VAL_PN:
         {
           LOG_MSG_INFO1("Case: received PN command", 0, 0, 0);
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_pn_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_pn_msg_resp( msg_ptr_ptr, msg_len_with_ea );
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_PSC:
         {
           LOG_MSG_INFO1("CASE: PSC", 0, 0, 0);
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_psc_msg_cmd( msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_psc_msg_resp();
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_CLD:
         {
           LOG_MSG_INFO1("CASE: CLD", 0, 0, 0);
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_cld_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_cld_msg_resp();
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_TEST:
         {
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_test_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_test_msg_resp();
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_FCON:
         {
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_fc_on_msg_cmd( msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_fc_on_msg_resp();
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_FCOFF:
         {
           if ( DS_MUX_MSG_CMD == is_command )
           {
             result = ds_mux_process_fc_off_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
             result = ds_mux_process_fc_off_msg_resp();
             send_msg_resp = FALSE;
           }
         }
         break;

         case DS_MUX_MSG_TYPE_VAL_MSC:
         {
           if ( DS_MUX_MSG_CMD == is_command )
           {
            LOG_MSG_INFO1("Processing MSC command", 0, 0, 0);
            result = ds_mux_process_msc_msg_cmd(  msg_ptr_ptr, msg_len_with_ea, &msg_rsp_frame, &send_msg_resp);
           }
           else
           {
              LOG_MSG_INFO1("Processing MSC reponse", 0, 0, 0);
              result = ds_mux_process_msc_msg_resp( msg_ptr_ptr, msg_len_with_ea );
              send_msg_resp = FALSE;
           }
         }
         break;

         default:
         {
           //unsupported messages
           result = DS_MUX_FAILURE;
           processing_msg = FALSE;
           break;
         }
       }

       LOG_MSG_INFO2( " ds_mux_process_rx_msgs_frames send msg resp %d ", send_msg_resp, 0, 0);

       if ( TRUE == send_msg_resp)
       {
         /*-----------------------------------------------------------------------
          In case of failure, return FAILURE. Success case: FREE the info ptr
      -----------  -------  -----------------------------------------------------*/

         if( DS_MUX_SUCCESS != ds_mux_io_transmit_msg_response ( &msg_rsp_frame ))
         {
           result = DS_MUX_FAILURE;
         }
         else
         {
          /*-----------------------------------------------------------------
            Flow controlling all DLCI including DLCI 0 (Control channel)
            Hence disabling flow after sending msg resp
          ------------------------------------------------------------------*/
           if ( DS_MUX_MSG_TYPE_VAL_PSC == msg_type && TRUE == is_command)
           {

             for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
             {
               ds_mux_disable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PSC );
               /*---------------------------------------------------------------------
                  Whenever this mask is set we should preform wakeup procedure once
                  wake up then clear the mask
                --------------------------------------------------     ---------------*/
             }
             ds_mux_set_mux_state( DS_MUX_STATE_IN_POWER_SAVE );
           }
           else if( DS_MUX_MSG_TYPE_VAL_CLD == msg_type && TRUE == is_command )
           {
             ds_mux_close_down_mux();
             processing_msg = FALSE;
             result         = DS_MUX_CLD_PROCESSED;
           }
         }

         //Reset Message field
         if(msg_rsp_frame.information_ptr != NULL)
         {
             DS_MUX_MEM_FREE(msg_rsp_frame.information_ptr);
             msg_rsp_frame.information_ptr = NULL;
         }
         else
         {
           LOG_MSG_ERROR("Information field in a frame is NULL", 0, 0, 0);
           result = DS_MUX_FAILURE;
         }
         msg_rsp_frame.length = 0;
         send_msg_resp = FALSE;
       }
    }while(0);
  }

  /*-----------------------------------------------------------------------
     Free the item if messages parsing failed
   -----------------------------------------------------------------------*/
  if(DS_MUX_CLD_PROCESSED == result)
  {
    LOG_MSG_INFO1("CLD handled", 0, 0, 0);
  }
  else if ( DS_MUX_SUCCESS != result )
  {
    LOG_MSG_ERROR("Could not handle the message", 0, 0, 0);
  }

  return result;
}/* ds_mux_process_rx_msgs_frames*/

void ds_mux_de_inititialize_dlci_conn
(
  ds_mux_dlci_param_type  *dlci_conn
)
{
  LOG_MSG_INFO1( "Deinitializing", 0, 0, 0);

  uint8 dlci_id;

  if ( NULL == dlci_conn)
  {
    LOG_MSG_ERROR( "ds_mux_de_inititialize_dlci_conn  DLCI conn ptr is NULL ",0, 0, 0);
  }
  else
  {
   /*-------------------------------------------------------------------------
      Re-set into global DLCI connection array
   -------------------------------------------------------------------------*/
     LOG_MSG_INFO1( "De-initialized the dlci_conn  %d",dlci_conn->dlci_id, 0, 0);
     dlci_id = dlci_conn->dlci_id;
     if(ds_mux_dlci_conn[dlci_id]->curr_msg_frame != NULL)
     {
       if(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr != NULL)
       {
         DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr);
         ds_mux_dlci_conn[dlci_id]->curr_msg_frame->information_ptr = NULL;
        }
        DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]->curr_msg_frame);
        ds_mux_dlci_conn[dlci_id]->curr_msg_frame = NULL;
      }
      if(ds_mux_dlci_conn[dlci_id] != NULL)
      {
        DS_MUX_MEM_FREE(ds_mux_dlci_conn[dlci_id]);
        ds_mux_dlci_conn[dlci_id] = NULL;
      }
  }
}/* ds_mux_de_inititialize_dlci_conn*/


void ds_mux_disconnect_dlci
(
  ds_mux_dlci_param_type  *dlci_conn
)
{

  LOG_MSG_INFO2( " ds_mux_disconnect_dlci dlci id %d ", dlci_conn->dlci_id, 0, 0);

  /*-----------------------------------------------------------------------

    1.close all fds
    2. De-Initilize DLCI connection (Free outstanding msg and command) and Free the DLCI-conn
  -----------------------------------------------------------------------*/

  /*Step 2*/
  ds_mux_de_inititialize_dlci_conn( dlci_conn );

  return ;
}/* ds_mux_disconnect_dlci */


ds_mux_result_enum_type ds_mux_process_dm_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{
  ds_mux_result_enum_type result             = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type *dlci_conn_param    = NULL;

  /*-----------------------------------------------------------------------
    1. Get DLCI connection pointer
    2. Disconnect DLCI(Free up resouces)
  -----------------------------------------------------------------------*/
  do
  {
    dlci_conn_param = (ds_mux_dlci_param_type *) ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      result = DS_MUX_FAILURE;
      break ;
    }
    ds_mux_disconnect_dlci( dlci_conn_param );

    LOG_MSG_INFO1( " ds_mux_process_dm_cmd_frames dlci id %d ", frame->dlci, 0, 0);

  }while(0);

  return result;
}/* ds_mux_process_dm_cmd_frames*/

void ds_mux_prepare_dm_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_util_prepare_cmd_frame(input_frame->dlci,
                                DS_MUX_FRAME_TYPE_UTIL_DM,
                                FALSE,
                                input_frame->poll_final,
                                output_frame);

  output_frame->length = input_frame->length;

  LOG_MSG_INFO1("ip len: %X len_ea %x", input_frame->length, input_frame->length_wt_ea, 0);

  output_frame->length_wt_ea = input_frame->length_wt_ea;

  return;
}/* ds_mux_prepare_ua_response */

void ds_muxi_deinit(void)
{
  uint8 index = 0;
  ds_mux_set_mux_state(DS_MUX_STATE_CLOSED);
  ds_mux_set_is_initiator(FALSE);

  for ( index = 1; index <= DS_MUX_MAX_LOGICAL_CONNECTION; index++)
  {
    if ( NULL != ds_mux_dlci_conn[index])
    {
      ds_mux_de_inititialize_dlci_conn( ds_mux_dlci_conn[index]);
      DS_MUX_MEM_FREE( ds_mux_dlci_conn[index]);
      ds_mux_dlci_conn[index] = NULL;
    }
  }

  if ( NULL != ds_mux_cmux_params)
  {
     DS_MUX_MEM_FREE(ds_mux_cmux_params);
     ds_mux_cmux_params = NULL;
  }

  LOG_MSG_INFO2( "ds_muxi_deinit done ", 0, 0, 0);

}/*ds_muxi_deinit*/


void ds_mux_close_down_mux
(
  void
)
{
  ds_mux_dlci_param_type      *dlci_conn_param = NULL;
  int16                        ret;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2( "ds_mux_close_down_mux Closing down MUX functionality", 0, 0, 0);

  /*-----------------------------------------------------------------------
    1.  Make sure ALL DLCIs should be closed. ( Spec 27.010 5.8.2 Close-down procedure)
    2.  Close down DLCI 0 (Free all resources associated with  DLCI 0 )
    3.  Notify client about MUX closing
  -----------------------------------------------------------------------*/
  /*Step 1*/

   LOG_MSG_INFO1("Closed all SMD DATA channels", 0, 0, 0);

   ds_mux_clear_fd(&sk_fdset, smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX]);
   ds_mux_clear_fd(&sk_fdset, smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX]);
   ds_mux_clear_fd(&sk_fdset, smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX]);

   close(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX]);
   close(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX]);
   close(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX]);

   ds_muxi_deinit();

  /*Step 2*/
   dlci_conn_param = (ds_mux_dlci_param_type *) ds_mux_get_dlci_conn_ptr( DS_MUX_DLCI_ID_0 );

   if ( NULL != dlci_conn_param )
   {
     ds_mux_disconnect_dlci( dlci_conn_param );
   }

   if(smd_tty_info !=NULL)
   {
     ret = ds_mux_set_dtr_high(smd_tty_info->smd_fd);

     if(ret == DS_MUX_FAILURE_MACRO)
     {
       LOG_MSG_ERROR("Couldnot set DTR to high", 0, 0, 0);
     }
     else
     {
       LOG_MSG_INFO1("DTR bit set to high", 0, 0, 0);
     }
   }

   ds_mux_timer_dealloc();
   ds_mux_bridge_active = FALSE;
   LOG_MSG_INFO2( " ds_mux_close_down_mux MUX Closed down ", 0, 0, 0);
   LOG_MSG_INFO1("Returned to Legacy AT command mode", 0, 0, 0);

}/*ds_mux_close_down_mux*/

ds_mux_result_enum_type ds_mux_process_disc_cmd_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type result          = DS_MUX_SUCCESS;
  ds_mux_dlci_param_type* dlci_conn_param = NULL;
  ds_mux_io_frame_type    rsp_frame;

  /*-----------------------------------------------------------------------
    1  IF connection is not valid
      1.1. Send DM message to host
    2 IF connection is valid
      2.1 Prepare and Send UA to host
    . 2.2 Disconnect the DLCI if Connection is valid
  -----------------------------------------------------------------------*/
  memset(&rsp_frame, 0, sizeof(ds_mux_io_frame_type));
  rsp_frame.information_ptr = NULL;
  do
  {
    dlci_conn_param = (ds_mux_dlci_param_type *)ds_mux_get_dlci_conn_ptr(frame->dlci);

    if ( NULL == dlci_conn_param)
    {
      ds_mux_prepare_dm_response(frame , &rsp_frame);
      ds_mux_io_transmit_cmd_response(&rsp_frame);
      LOG_MSG_INFO2( "ds_mux_process_disc_cmd_frames Sending DM dlci id %d ", frame->dlci, 0, 0);

    }
    else
    {
      ds_mux_prepare_ua_response(frame , &rsp_frame);
      result = ds_mux_io_transmit_cmd_response(&rsp_frame);
      if(result != DS_MUX_SUCCESS)
      {
        LOG_MSG_ERROR("Couldnot Transmit the UA frame reponse to the DM command", 0, 0, 0);
      }
      else
      {
        LOG_MSG_INFO2( " ds_mux_process_disc_cmd_frames sent UA dlci id %d ", frame->dlci, 0, 0);
        ds_mux_disconnect_dlci( dlci_conn_param );
      }

      if ( DS_MUX_DLCI_ID_0 == frame->dlci )
      {
        LOG_MSG_INFO2( "ds_mux_process_disc_cmd_frames: Received DISC "
                       "for DLCI%d, treating it as CLD command ", frame->dlci, 0, 0);
        ds_mux_close_down_mux();
        result         = DS_MUX_CLD_PROCESSED;
      }
    }
  }while(0);

  return result;
}/* ds_mux_process_disc_cmd_frames*/

ds_mux_result_enum_type ds_mux_process_dlci_rx_ctl_frames
(
  ds_mux_io_frame_type    *frame
)
{

  ds_mux_result_enum_type       result = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //DLCI Messages are sent as UI or UIH frames
  switch ( frame->frame_type )
  {
    case DS_MUX_FRAME_TYPE_UTIL_UIH:
    case DS_MUX_FRAME_TYPE_UTIL_UI:
    {
      LOG_MSG_INFO1(" Case is UIH UI", 0, 0, 0);
      result = ds_mux_process_rx_msgs_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_UTIL_SABM:
    {
      LOG_MSG_INFO1(" Case is SABM", 0, 0, 0);
      if((frame->dlci > 0 && ds_mux_Is_dlci_connected(DS_MUX_DLCI_ID_0) )|| frame->dlci == 0)
      {
        result = ds_mux_process_sabm_cmd_frames( frame );
      }
      else
      {
        LOG_MSG_ERROR("DLCI ID is not yet connected: dlci %d", frame->dlci, 0, 0);
        result = DS_MUX_FAILURE;
      }
    }
    break;
    case DS_MUX_FRAME_TYPE_UTIL_UA:
    {
      LOG_MSG_INFO1(" Case is UIH UA", 0, 0, 0);
      result = ds_mux_process_ua_cmd_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_UTIL_DM:
    {
      LOG_MSG_INFO1(" Case is DM", 0, 0, 0);
      result = ds_mux_process_dm_cmd_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_UTIL_DISC:
    {
      LOG_MSG_INFO1(" Case is DISC", 0, 0, 0);
      result = ds_mux_process_disc_cmd_frames( frame );
    }
    break;
    case DS_MUX_FRAME_TYPE_INVALID_FRAME:
      LOG_MSG_INFO1(" Case is INVALID frame", 0, 0, 0);
      break;
    default:
    {
      result = DS_MUX_FAILURE;
    }
  }

  return result;
}/* ds_mux_process_dlci_rx_ctl_frames*/



dlci_cmux_param_type* ds_mux_get_cmux_params(void)
{
  return ds_mux_cmux_params;
}

int ds_mux_enter_mux_mode(void)
{
  int ret;
  ret = ds_mux_smd_ch_open();

  if(ret == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_INFO1("Could-Not open SMD channels", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    ds_mux_bridge_active = TRUE;
    ds_mux_set_mux_state(DS_MUX_STATE_OPENED);
    LOG_MSG_INFO1("Entered MUX mode %d and MUX state is opened is:%d",
                   ds_mux_bridge_active,
                   ds_mux_state_info.ds_mux_state,
                   0);
    if(smd_tty_info !=NULL)
    {
      ret = ds_mux_set_dtr_low(smd_tty_info->smd_fd);
    }
    if(ret == DS_MUX_FAILURE_MACRO)
    {
      LOG_MSG_ERROR("Couldnot set DTR to low", 0, 0, 0)
    }
    else
    {
      LOG_MSG_INFO1("DTR bit set to LOW ", 0, 0, 0);
    }
  }
  return DS_MUX_SUCCESS_MACRO;
}


void ds_mux_send_resp_for_atcmd(boolean atcmd_resp_value, char* resp_buf)
{
  int ret;
  ds_mux_at_cmd_response response;
  char *send_buffer = NULL;

  send_buffer = (char *)malloc(AT_CMD_AND_RESPONSE_SIZE);
  if(send_buffer == NULL)
  {
    LOG_MSG_ERROR("malloc failed.Could not allocate memory for send buffer.", 0, 0, 0);
    return;
  }
  memset(send_buffer, 0, AT_CMD_AND_RESPONSE_SIZE);

  LOG_MSG_INFO1("Sending Response for AT command",0, 0, 0);

  memset(&response, 0, sizeof(ds_mux_at_cmd_response));


  if(atcmd_resp_value)
  {
    response.errorCode = OK;
    LOG_MSG_INFO1("Sending response OK", 0, 0, 0);

    if(resp_buf != NULL)
    {
      memcpy(response.resp_buf, resp_buf, AT_CMD_PARAM_SIZE);
    }
    memcpy(send_buffer, &response, sizeof(ds_mux_at_cmd_response));
  }
  else
  {
    response.errorCode = ERROR;
    LOG_MSG_INFO1("Sending Response ERROR", 0, 0, 0);
    memcpy(send_buffer, &response, sizeof(ds_mux_at_cmd_response));
  }
  ret = ds_mux_send_msg(sk_info_bw_atfwd_ds_mux.sk_fd, send_buffer);
  if(ret != DS_MUX_SUCCESS_MACRO)
  {
    LOG_MSG_ERROR("could not send response to AT FWD", 0, 0, 0);
  }
  else
  {
    LOG_MSG_INFO1("Sent Response to ATFWD", 0, 0, 0);
  }

  if(send_buffer != NULL)
  {
    LOG_MSG_INFO1("Dealllocating Send Buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(send_buffer);
    send_buffer = NULL;
  }
  LOG_MSG_INFO1("at_cmux_eq_params: %d ", at_cmux_eq_params, 0, 0);

  /* If we get AT+CMUX=VALUE at_cmux_eq_params is set. if we receive AT+IPR command avoid_enter_mux_mode is set.
     We enter into MUX mode when at_cmux_eq_params is set. After receiving AT+IPR in MUX mode
     we shoud not enter into MUX mode. So an extra check (avoid_enter_mux_mode == FALSE) is added. */
  if(at_cmux_eq_params == TRUE && avoid_enter_mux_mode == FALSE)
  {
    LOG_MSG_INFO1("Entering MUX mode. at_cmux_eq_params: %d avoid_enter_mux_mode: %d", at_cmux_eq_params, avoid_enter_mux_mode, 0);
    ret = ds_mux_enter_mux_mode();
  }
  if(ret == DS_MUX_FAILURE_MACRO)
    LOG_MSG_ERROR("could not enter MUX mode: %d",ds_mux_bridge_active, 0, 0);

}


void ds_mux_to_atfwd_response_cb_reg(ds_mux_to_atfwd_response_callback resp_cb)
{
  LOG_MSG_INFO1("sending AT command response from DS_MUX to ATFWD ",0,0,0);

  at_params_resp_cb = resp_cb;

}

void ds_mux_get_supported_baud_rates(void)
{

  LOG_MSG_INFO1("get suppoted baud rates()", 0, 0, 0);

  at_resp_buf = (char*)malloc(DS_MUX_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    LOG_MSG_ERROR(" Not able to allocate memory for at_resp_buf", 0, 0, 0);
    return;
  }

  memset (at_resp_buf,0, DS_MUX_MAX_AT_TOKENS);

  snprintf(at_resp_buf, DS_MUX_MAX_AT_TOKENS,
           "\n \r+IPR:300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800");

  at_params_resp_cb(TRUE, at_resp_buf);

  if(at_resp_buf != NULL)
  {
    LOG_MSG_INFO1("Deallocating AT response buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(at_resp_buf);
    at_resp_buf = NULL;
  }

}

/*==============================================================================
FUNCTION qti_bridge_get_supported_cmux_params()

DESCRIPTION
  This function gets the supported CMUX parameters values

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_mux_get_supported_cmux_params(void)
{
  at_resp_buf = (char*)malloc(DS_MUX_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    LOG_MSG_ERROR("qti_bridge_get_supported_cmux_params"
                      " Not able to allocate memory for at_resp_buf", 0, 0, 0);
    return;
  }

  memset (at_resp_buf,0, DS_MUX_MAX_AT_TOKENS);

  snprintf(at_resp_buf, DS_MUX_MAX_AT_TOKENS,
           "\n \r+CMUX:(0),(0-2),(1-6),(1-32786),(1-255),(0-100),(2-255),(1-255),(1-7)");

  at_params_resp_cb(TRUE, at_resp_buf);

  if(at_resp_buf != NULL)
  {
    LOG_MSG_INFO1("Deallocating AT response buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(at_resp_buf);
    at_resp_buf = NULL;
  }

}


ds_mux_result_enum_type   ds_mux_initialize_cmux_param
(
  dlci_cmux_param_type    *cmux_param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_mux_cmux_params = (dlci_cmux_param_type* )malloc(sizeof(dlci_cmux_param_type));

  if ( NULL == ds_mux_cmux_params)
  {
    return DS_MUX_FAILURE;
  }
  ds_mux_cmux_params->operating_mode = ( ( 0 != (cmux_param->mask & DS_MUX_SET_MODE)  ) ?
    cmux_param->operating_mode : DS_MUX_MODE_BASIC );

  ds_mux_cmux_params->subset = ( ( 0 != (cmux_param->mask & DS_MUX_SET_SUBSET)  ) ?
    cmux_param->subset : DS_MUX_SUBSET_UIH );

  ds_mux_cmux_params->port_speed = ( ( 0 != (cmux_param->mask & DS_MUX_SET_PORT_SPEED)  ) ?
    cmux_param->port_speed : DS_MUX_PHY_PORT_SPEED_1 );

  ds_mux_cmux_params->frame_size_N1 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_FRAME_SIZE_N1)  ) ?
    cmux_param->frame_size_N1 : DS_MUX_CMUX_DEFAULT_FRAME_N1 ) ;

  ds_mux_cmux_params->response_timer_T1 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_ACK_TIMER_T1)  ) ?
    cmux_param->response_timer_T1 : DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1 );

  ds_mux_cmux_params->re_transmissions_N2 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_RE_TRIES_N2)  ) ?
    cmux_param->re_transmissions_N2 : DS_MUX_CMUX_DEFAULT_MAX_TX_N2 );

  ds_mux_cmux_params->response_timer_T2 = ( ( 0 != (cmux_param->mask & DS_MUX_SET_RESP_TIMER_T2)  ) ?
    cmux_param->response_timer_T2 : DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2 );

  ds_mux_cmux_params->wake_up_timer_T3 =  ( ( 0 != (cmux_param->mask & DS_MUX_SET_WAKEUP_TIMER_T3)  ) ?
    cmux_param->wake_up_timer_T3 : DS_MUX_CMUX_DEFAULT_TIMER_T3 );

  ds_mux_cmux_params->window_size_k = ( ( 0 != (cmux_param->mask & DS_MUX_SET_WINDOW_SIZE_K)  ) ?
    cmux_param->window_size_k : DS_MUX_WINDOW_SIZE_2 );

  ds_mux_cmux_params->mask = DS_MUX_SET_ALL_CMUX_FIELDS;

  return DS_MUX_SUCCESS;

}/*ds_mux_initialize_cmux_param*/


ds_mux_state_enum_type ds_mux_get_mux_state
(
  void
)
{
  return ds_mux_state_info.ds_mux_state;
}/* ds_mux_get_mux_state */

ds_mux_result_enum_type   ds_muxi_passive_init
(
  dlci_cmux_param_type    *cmux_param
)
{
  ds_mux_result_enum_type   result  = DS_MUX_SUCCESS;
  ds_mux_state_enum_type    ds_mux_prev_state = ds_mux_get_mux_state();

  LOG_MSG_INFO2( "Entered ds_muxi_passive_init ", 0,0,0);

  if ( NULL == cmux_param )
  {
   LOG_MSG_ERROR("CMUX PARAMETER is NULL", 0, 0, 0);
    return DS_MUX_NULL_PARAM;
  }
  do
  {
    /*-----------------------------------------------------------------------
       Initialize  CMUX param.
    -----------------------------------------------------------------------*/
    if ( DS_MUX_SUCCESS != ds_mux_initialize_cmux_param( cmux_param ) )
    {
      LOG_MSG_ERROR("Could-Not initialize the CMUX parameter", 0, 0, 0);
      result = DS_MUX_FAILURE;
      break;
    }

    LOG_MSG_INFO2( "ds_muxi_passive_init Success  ",0,0,0);

    return result;

  }while(0);

  LOG_MSG_ERROR("ds_muxi_passive_init(): Failed ",0,0,0);

  /*-----------------------------------------------------------------------
    1.  De-allocate all resources
    2.  Set DS MUX state to previous state
    3.  return result error
  -----------------------------------------------------------------------*/
  //Passive init has been failed.
  if ( NULL != ds_mux_cmux_params )
  {
    LOG_MSG_ERROR("Passive init failed and Deallocating the ds_mux_cmux_params", 0, 0, 0);
    DS_MUX_MEM_FREE(ds_mux_cmux_params);
    ds_mux_cmux_params = NULL;
    result = DS_MUX_FAILURE;
    ds_mux_set_mux_state(ds_mux_prev_state);
  }

  return result;
}/*ds_muxi_passive_init*/

void ds_mux_get_current_baud_rate()
{

  at_resp_buf = (char*)malloc(DS_MUX_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    LOG_MSG_ERROR(" Not able to allocate memory for at_resp_buf", 0, 0, 0);
    return;
  }

  memset (at_resp_buf,0,DS_MUX_MAX_AT_TOKENS);
  snprintf(at_resp_buf,DS_MUX_MAX_AT_TOKENS,
           "\n \r+IPR: %d",
           baud_rate);

  at_params_resp_cb(TRUE, at_resp_buf);

  if(at_resp_buf != NULL)
  {
    LOG_MSG_INFO1("Deallocating AT response buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(at_resp_buf);
    at_resp_buf = NULL;
  }
}



/*==============================================================================
FUNCTION ds_mux_validate_cmux_params()

DESCRIPTION
  This function validates all DS MUX AT command parameters for their values
  received from AT-FWD and stores them if all are valid

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_mux_get_current_cmux_params(void)
{
  dlci_cmux_param_type *curr_cmux_param_ptr;

  LOG_MSG_INFO1("qti_bridge_get_current_cmux_params()", 0, 0, 0);

  at_resp_buf = (char*)malloc(DS_MUX_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    LOG_MSG_ERROR("qti_bridge_get_current_cmux_params"
                       " Not able to allocate memory for at_resp_buf", 0, 0, 0);
    return;
  }

  memset (at_resp_buf,0,DS_MUX_MAX_AT_TOKENS);

  // If MUX mode is active fetch from the CMUX params global variable
  if (ds_mux_bridge_active)
  {
    if ((curr_cmux_param_ptr = ds_mux_get_cmux_params()) != NULL)
    {
      snprintf(at_resp_buf,DS_MUX_MAX_AT_TOKENS,
           "\n \r+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d",
           curr_cmux_param_ptr->operating_mode,curr_cmux_param_ptr->subset,
           curr_cmux_param_ptr->port_speed, curr_cmux_param_ptr->frame_size_N1,
           curr_cmux_param_ptr->response_timer_T1,
           curr_cmux_param_ptr->re_transmissions_N2,
           curr_cmux_param_ptr->response_timer_T2,
           curr_cmux_param_ptr->wake_up_timer_T3,
           curr_cmux_param_ptr->window_size_k);
    }
    else
    {
      snprintf(at_resp_buf,DS_MUX_MAX_AT_TOKENS,
           "\n \r+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d",
           DS_MUX_MODE_BASIC,DS_MUX_SUBSET_MIN,
           DS_MUX_PHY_PORT_SPEED_1, DS_MUX_CMUX_DEFAULT_FRAME_N1,
           DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1,DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
           DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2,DS_MUX_CMUX_DEFAULT_TIMER_T3,
           DS_MUX_WINDOW_SIZE_2);
    }
  }
  else
  {
    LOG_MSG_INFO1("MUX Mode not ON returning default params", 0, 0, 0);

    snprintf(at_resp_buf,DS_MUX_MAX_AT_TOKENS,
           "\n \r+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d",
           DS_MUX_MODE_BASIC,DS_MUX_SUBSET_MIN,
           DS_MUX_PHY_PORT_SPEED_1, DS_MUX_CMUX_DEFAULT_FRAME_N1,
           DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1,DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
           DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2,DS_MUX_CMUX_DEFAULT_TIMER_T3,
           DS_MUX_WINDOW_SIZE_2);
  }

  at_params_resp_cb(TRUE, at_resp_buf);

  if(at_resp_buf != NULL)
  {
    LOG_MSG_INFO1("Deallocating AT response buffer", 0, 0, 0);
    DS_MUX_MEM_FREE(at_resp_buf);
    at_resp_buf = NULL;
  }
}

int set_baud_rate_cb()
{
   int ret = DS_MUX_SUCCESS_MACRO;
   struct termios2 ntio;

   if(tty_info == NULL )
   {
     LOG_MSG_INFO1("Cannot set baud rate, UART fd NOT present", 0, 0, 0);
     return;
   }

   ret = ioctl(tty_info->fd, TCGETS2, &ntio);
   if(ret == -1)
   {
     LOG_MSG_INFO1("IOCTL FAILED errno %d", errno, 0, 0);
   }
   else
   {
     LOG_MSG_INFO1("IOCTL SUCCESS", 0, 0, 0);
   }
   ntio.c_cflag &= ~CBAUD;
   ntio.c_cflag |= BOTHER | CREAD;
   ntio.c_ispeed = baud_rate;
   ntio.c_ospeed = baud_rate;

   ret = ioctl(tty_info->fd, TCSETS2, &ntio);
   if(ret == DS_MUX_FAILURE_MACRO)
   {
     LOG_MSG_INFO1("IOCTL FAILED errno: %d", errno, 0, 0);
   }
   else
   {
     LOG_MSG_INFO1("IOCTL SUCCESS", 0, 0, 0);
   }
   return ret;
}


void reg_baud_cb(set_baud_rate set_baud_rate_cb)
{
  set_baud_cb = set_baud_rate_cb;
}

boolean ds_mux_validate_and_set_baud_rate(char token[MAX_TOKENS][MAX_TOKEN_SIZE+1])
{
  boolean ret = TRUE;

  uint8 idx = 0;
  int i, j, len, ret1, baud_rate_value;
  struct termios2 ntio;
  char temp[MAX_TOKEN_SIZE+1] = {0};

  LOG_MSG_INFO1("validating IPR value()", 0, 0, 0);

  /* Extract the baud rate */
  strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
  len = strlen(temp);
  baud_rate_value = (ds_mux_at_fwd_mode_enum_type) ds_mux_string_to_int(token, len, idx);

  if(baud_rate_value == BAUD_RATE_300 || baud_rate_value == BAUD_RATE_600 || baud_rate_value == BAUD_RATE_1200 ||
     baud_rate_value == BAUD_RATE_2400 || baud_rate_value == BAUD_RATE_4800 || baud_rate_value == BAUD_RATE_9600 ||
     baud_rate_value == BAUD_RATE_19200 || baud_rate_value == BAUD_RATE_38400 || baud_rate_value == BAUD_RATE_57600 ||
     baud_rate_value == BAUD_RATE_115200 || baud_rate_value == BAUD_RATE_230400 || baud_rate_value == BAUD_RATE_460800)
    {
      LOG_MSG_INFO1("Received a valid baud rate %d", baud_rate_value, 0, 0);
      baud_rate = baud_rate_value;
      ds_mux_write_ipr_file();
    }
    else
    {
      /* baud_rate already contains the default baud rate value. It is set in main function. */
      LOG_MSG_INFO1("baud rate received is incorrect baud_rate: %d", baud_rate_value, 0, 0);
      ret = FALSE;
    }

  return ret;

}

boolean ds_mux_validate_cmux_params(void)
{
  boolean ret = TRUE;

  LOG_MSG_INFO1("ds_mux_validate_cmux_params()", 0, 0, 0);

  if( ds_mux_set_oprt_mode((ds_mux_mode_enum_type )cmux_params->operating_mode,
                            (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
  {
    ret = FALSE;
    LOG_MSG_ERROR("cmux_params invalid operating mode:%d",
                            cmux_params->operating_mode,0,0);
  }

  if(cmux_params->subset)
  {
    if( ds_mux_set_subset((ds_mux_subset_enum_type)cmux_params->subset,
                          (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
      ret = FALSE;
      LOG_MSG_ERROR("cmux_params invalid subset:%d", cmux_params->subset,0,0);
    }
  }
  if(cmux_params->port_speed)
  {
    if( ds_mux_set_port_speed((ds_mux_port_speed_enum_type)cmux_params->port_speed,
                              (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
      ret = FALSE;
      LOG_MSG_ERROR("cmux_params invalid port speed:%d", cmux_params->port_speed,0,0);
    }
  }

  if(cmux_params->frame_size_N1)
  {
    if( ds_mux_set_N1(cmux_params->frame_size_N1,
                      (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params invalid frame_size_N1:%d", cmux_params->frame_size_N1,0,0);
    }
  }

  if(cmux_params->response_timer_T1)
  {
    if( ds_mux_set_T1(cmux_params->response_timer_T1,
                      (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params invalid response_timer_T1:%d", cmux_params->response_timer_T1,0,0);
    }
  }

  if (cmux_params->re_transmissions_N2)
  {
    if( ds_mux_set_N2(cmux_params->re_transmissions_N2,
                      (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params re_transmissions_N2:%d", cmux_params->re_transmissions_N2,0,0);
    }
  }

  if(cmux_params->response_timer_T2)
  {
    if( ds_mux_set_T2(cmux_params->response_timer_T2,
                       (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params response_timer_T2:%d", cmux_params->response_timer_T2,0,0);
    }
  }

  if(cmux_params->wake_up_timer_T3)
  {
    if( ds_mux_set_T3(cmux_params->wake_up_timer_T3,
                      (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params wake_up_timer_T3:%d", cmux_params->wake_up_timer_T3,0,0);
    }
  }

  if(cmux_params->window_size_k)
  {
     if( ds_mux_set_K(cmux_params->window_size_k,
                      (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
     {
       ret = FALSE;
       LOG_MSG_ERROR("cmux_params window_size_k:%d", cmux_params->window_size_k,0,0);
     }
  }

  LOG_MSG_INFO1("Validation of CMUX parameters Completed", 0, 0, 0);

  return ret;
}

int ds_mux_string_to_int(char token[MAX_TOKENS][MAX_TOKEN_SIZE+1], int len, int idx)
{
  int j=0, int_num=0;

  for(j=0; j<len; j++)
  {
    int_num = int_num * 10 + ( token[idx][j] - '0' );
  }
  return int_num;

}

/*==============================================================================
FUNCTION ds_mux_validate_cmux_params()

DESCRIPTION
  This function validates all DS MUX AT command parameters for their values
  received from AT-FWD and stores them if all are valid

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/



void ds_mux_tokenize_cmux_input(char token[MAX_TOKENS][MAX_TOKEN_SIZE+1], int ntokens)
{
  uint8 idx = 0;
  uint8 buf_idx = 0;
  char buffer[8];
  int value = 0;
  int i, j,len;
  char temp[MAX_TOKEN_SIZE+1] = {0};

  LOG_MSG_INFO1("tokenize_cmux_input()", 0, 0, 0);

  if(cmux_params == NULL)
  {
    LOG_MSG_ERROR("cmux_params is NULL. Memory is allocated aloocated to it. Pls check", 0, 0, 0);
    return;
  }

  // Extract the transparency (operating mode)
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->operating_mode = (ds_mux_at_fwd_mode_enum_type) ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract the subset
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->subset = (ds_mux_at_fwd_subset_enum_type)ds_mux_string_to_int(token, len, idx);
    idx++;
  }


  //Extract the Port speed
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->port_speed = (ds_mux_at_fwd_port_speed_enum_type)ds_mux_string_to_int(token,
                                                                                       len,
                                                                                       idx);
    idx++;
  }

  //Extract N1
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->frame_size_N1 = (uint16)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract T1
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->response_timer_T1 = (uint16)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract N2
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->re_transmissions_N2 = (uint8)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract T2
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->response_timer_T2 = (uint16)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract T3
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->wake_up_timer_T3 = (uint_32)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  //Extract K
  if(idx < ntokens)
  {
    strlcpy(temp, token[idx], MAX_TOKEN_SIZE+1);
    len = strlen(temp);
    cmux_params->window_size_k = (uint8)ds_mux_string_to_int(token, len, idx);
    idx++;
  }

  LOG_MSG_INFO1("tokenize complete", 0, 0, 0);
}

void ds_mux_validate_atfwd_cmux_params(ds_mux_at_cmd_request* atcmd_ptr)
{
  int ret;
  ds_mux_result_enum_type result;
  if(atcmd_ptr == NULL)
  {
    LOG_MSG_ERROR ("atcmd_ptr is NULL",0, 0, 0 );
    return;
  }

  if( (strncasecmpp(atcmd_ptr->name, "+CMUX", strlen("+CMUX")) ) == 0 ||
       (strncasecmpp(atcmd_ptr->name, "+IPR", strlen("+IPR"))  ) ==0 )
  {
     ds_mux_to_atfwd_response_cb_reg(ds_mux_send_resp_for_atcmd);
     LOG_MSG_INFO1("Received : %s " , atcmd_ptr->name, 0, 0);
     LOG_MSG_INFO1 ("Registered for at-command response",
                       0, 0, 0 );
  }
  else
  {
    LOG_MSG_ERROR("We didnot receive +CMUX or +IPRcommand we received: %s", atcmd_ptr->name, 0, 0);
    return;
  }


  if(  (strncasecmpp(atcmd_ptr->name, "+CMUX", strlen("+CMUX"))) == 0)
  {
    if(atcmd_ptr->opcode == OP_NA)
    {
      LOG_MSG_ERROR ("Received AT+CMUX with Name only which is not supported",
                         0, 0, 0 );
      if(at_params_resp_cb != NULL)
      {
        LOG_MSG_INFO1("sending response ", 0, 0, 0);
        at_cmux_eq_params = FALSE;
        at_params_resp_cb(FALSE, at_resp_buf);
      }
      else
      {
        LOG_MSG_ERROR("at_params_resp_cb is NULL", 0, 0, 0);
        return;
      }
    }
    else if(atcmd_ptr->opcode == (OP_NA | OP_EQ| OP_AR) )
    {
      LOG_MSG_INFO1("Received AT+CMUX = <parameters>",0,0,0);

      cmux_params = (at_fwd_cmux_cmd_params_struct*)malloc(sizeof(at_fwd_cmux_cmd_params_struct));

      if (cmux_params == NULL)
      {
        LOG_MSG_ERROR ("ds_mux validate_atfwd_cmux_params: Not able to"
                         " allocate memory for cmux_params", 0, 0, 0 );
        return;
      }
      memset (cmux_params, 0, sizeof(at_fwd_cmux_cmd_params_struct));
      ds_mux_tokenize_cmux_input(atcmd_ptr->token, atcmd_ptr->ntokens);

      ret = ds_mux_validate_cmux_params();

      if (ret)
      {
        if(at_params_resp_cb)
        {
          LOG_MSG_INFO1("sending response", 0, 0, 0);
          at_cmux_eq_params = TRUE;
          at_params_resp_cb(TRUE, at_resp_buf);
        }
        else
        {
          LOG_MSG_ERROR("at_params_resp_cb is NULL", 0, 0, 0);
          return;
        }
        result = ds_mux_passive_init((dlci_cmux_param_type *)cmux_params);
        if(result != DS_MUX_SUCCESS)
        {
          LOG_MSG_ERROR("Passive Init failed", 0, 0, 0);
        }
        else
        {
          LOG_MSG_INFO1("Passive Init Successfull", 0, 0, 0);
        }
      }
      else
      {
        LOG_MSG_ERROR("VALIDATION OF +CMUX COMMAND FAILED.", 0, 0, 0);
        at_params_resp_cb(FALSE, at_resp_buf);
      }

      if(cmux_params != NULL)
      {
        LOG_MSG_INFO1("Deallocating cmux_params", 0, 0, 0);
        DS_MUX_MEM_FREE(cmux_params);
        cmux_params = NULL;
      }
      else
      {
        LOG_MSG_ERROR("CMUX PARAMS already NULL", 0, 0, 0);
      }
    }
    else if(atcmd_ptr->opcode == (OP_NA | OP_QU))
    {
      LOG_MSG_INFO1("Received AT+CMUX?", 0, 0, 0);
      at_cmux_eq_params = FALSE;
      ds_mux_get_current_cmux_params();
    }
    else if (atcmd_ptr->opcode == (OP_NA | OP_EQ |
                                    OP_QU))
    {
      LOG_MSG_INFO1("Received AT+CMUX=?", 0, 0, 0);
      at_cmux_eq_params = FALSE;
      ds_mux_get_supported_cmux_params();
    }
  }
  else if( (strncasecmpp(atcmd_ptr->name, "+IPR", strlen("+IPR"))) == 0 )
  {
    if(atcmd_ptr->opcode == OP_NA)
    {
      LOG_MSG_ERROR ("Received AT+CMUX with Name only which is not supported",
                         0, 0, 0 );
      if(at_params_resp_cb != NULL)
      {
        avoid_enter_mux_mode = TRUE;
        LOG_MSG_INFO1("sending response ", 0, 0, 0);
        at_params_resp_cb(FALSE, at_resp_buf);
      }
      else
      {
        LOG_MSG_ERROR("at_params_resp_cb is NULL", 0, 0, 0);
        return;
      }
    }
    else if(atcmd_ptr->opcode == (OP_NA | OP_EQ| OP_AR) )
    {
      LOG_MSG_INFO1("Received AT+IPR = <value>",0,0,0);

      ret = ds_mux_validate_and_set_baud_rate(atcmd_ptr->token);
      reg_baud_cb(set_baud_rate_cb);

      if (ret)
      {
        if(at_params_resp_cb)
        {

          LOG_MSG_INFO1("SETTING avoid_enter_mux_mode", 0, 0, 0);
          avoid_enter_mux_mode = TRUE;

          LOG_MSG_INFO1("sending response", 0, 0, 0);
          at_params_resp_cb(TRUE, at_resp_buf);
        }
        else
        {
          LOG_MSG_ERROR("at_params_resp_cb is NULL", 0, 0, 0);
          return;
        }
      }
      else
      {
        LOG_MSG_ERROR("VALIDATION OF +IPR Failed. Invalid baud rate", 0, 0, 0);
        avoid_enter_mux_mode = TRUE;
        at_params_resp_cb(FALSE, at_resp_buf);
      }
    }
    else if(atcmd_ptr->opcode == (OP_NA | OP_QU))
    {
      LOG_MSG_INFO1("Received AT+IPR?", 0, 0, 0);
      avoid_enter_mux_mode = TRUE;
      ds_mux_get_current_baud_rate();
    }
    else if (atcmd_ptr->opcode == (OP_NA | OP_EQ |
                                   OP_QU))
    {
      LOG_MSG_INFO1("Received AT+CMUX=?", 0, 0, 0);
      avoid_enter_mux_mode = TRUE;
      ds_mux_get_supported_baud_rates();
    }
    else
    {
      LOG_MSG_ERROR("Something is going wrong. It should not reach this statement", 0, 0, 0);
      return;
    }
  }
}
