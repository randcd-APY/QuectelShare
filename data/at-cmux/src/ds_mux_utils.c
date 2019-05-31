/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   U T I L S

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed
  for decoding and encoding MUX packets.

  DS MUX UTILS function could be invoked independently to generate
  mux frame

Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   sm     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/


#include <linux/kernel.h>
#include <linux/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <linux/filter.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <fcntl.h>

#include "ds_mux_main.h"

boolean ea_bit_length_field             = TRUE;
boolean frag_frame_check                = FALSE;
int rem_info_len = 0;

extern ds_mux_dlci_param_type*    ds_mux_dlci_conn[DS_MUX_MAX_LOGICAL_CONNECTION + 1];
extern ds_mux_smd_data_config     smd_data_fds;


/* CRC table to be used for FCS calculation
 * Inserted from 3GPP Spec 27.010 version 5
 */
static uint8  ds_mux_util_crc_table[DS_MUX_UTIL_CRC_TABLE_ELEMENTS] =
{   /* reversed, 8-bit, poly=0x07 */
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,
    0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

/*===========================================================================

FUNCTION DS_MUX_UART_SIGNAL_HANDLER

DESCRIPTION

  This function will handle the UART inactivity timer expire signal.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_uart_signal_handler(int sig)
{
  int ret = DS_MUX_SUCCESS_MACRO;
  switch(sig)
  {
    case SIG2:
      ret = ds_mux_uart_timer_ind();
    break;

    default:
    break;
  }
  if(ret == DS_MUX_FAILURE_MACRO)
    LOG_MSG_ERROR("ds_mux_timer_ind failed", 0, 0, 0);
}

void ds_mux_register_uart_timer_sig_handler()
{
  struct sigaction action;


  memset(&action, 0, sizeof(action));
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  action.sa_handler = ds_mux_uart_signal_handler;

  if (sigaction(SIG2, &action, NULL) == -1)
  {
    LOG_MSG_ERROR("sigaction failed", 0, 0, 0);
  }

}


/*===========================================================================

FUNCTION QCMAP_NL_ADDFD_MAP()

DESCRIPTION

  This function
  - maps the socket descriptor with the corresponding callback function
  - add the socket descriptor to the set of socket desc the listener thread
    listens on.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int ds_mux_addfd_map
(
  ds_mux_sk_fd_set_info_t      *fd_set_info,
  int                           fd,
  ds_mux_sock_thrd_fd_read_f   read_f,
  int                         max_fds
)
{
  ds_assert(fd_set_info != NULL);

  if( fd_set_info->num_fd < max_fds )
  {
/* -----------------------------------------------------------------------
  Add the fd to the fd set which the listener thread should listen on
------------------------------------------------------------------------ */
    FD_SET(fd, &(fd_set_info->fdset));

/* -----------------------------------------------------------------------
  Associate fd with the corresponding read function
------------------------------------------------------------------------ */
    fd_set_info->sk_fds[fd_set_info->num_fd].sk_fd = fd;
    fd_set_info->sk_fds[fd_set_info->num_fd].read_func = read_f;
    fd_set_info->num_fd++;
    LOGI("Adding FD to fd_set %d %d",fd,fd_set_info->num_fd);

/*-----------------------------------------------------------------------
  Increment the max socket desc number which the listener should listen
  if required
------------------------------------------------------------------------*/
    if(fd_set_info->max_fd < fd)
    {
      fd_set_info->max_fd = fd;
    }
  }
  else
  {
    LOGI("Exceeds maximum num of FD");
    return DS_MUX_FAILURE_MACRO;
  }

  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION QTI_PPP_CLEAR_FD()

DESCRIPTION

  This function
  - Removes the fd to the list of FD on which select call listens.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int ds_mux_clear_fd
(
   ds_mux_sk_fd_set_info_t *fd_set,
   int                     fd
)
{
  int i=0, j=0;
/*--------------------------------------------------------------------------
  Remove fd to fdmap array.
-------------------------------------------------------------------------- */
  for ( i = 0; i < fd_set->num_fd; i++ )
  {
     if ( fd_set->sk_fds[i].sk_fd == fd )
       break;
  }

  if ( i == fd_set->num_fd )
  {
    LOG_MSG_ERROR("Something wrong FD %d not found", fd, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  /* Clear the fd. */
  for ( j = i; j < ( fd_set->num_fd - 1); j++ )
  {
    fd_set->sk_fds[j].sk_fd = fd_set->sk_fds[j+1].sk_fd;
    fd_set->sk_fds[j].read_func = fd_set->sk_fds[j+1].read_func;
  }

  fd_set->sk_fds[fd_set->num_fd - 1].sk_fd = 0;
  fd_set->sk_fds[fd_set->num_fd - 1].read_func = NULL;
  fd_set->num_fd--;

/*--------------------------------------------------------------------------
  Re-calculate max_fd
--------------------------------------------------------------------------*/
  fd_set->max_fd = 0;
  for ( i = 0; i < fd_set->num_fd; i++ )
  {
     if ( fd_set->max_fd < fd_set->sk_fds[i].sk_fd )
       fd_set->max_fd = fd_set->sk_fds[i].sk_fd;
  }

  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION QCMAP_LISTENER_START()

DESCRIPTION

  This function
  - calls the select system call and listens data coming on
    various sockets and call the appropriate handler functions.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int ds_mux_listener_start
(
  ds_mux_sk_fd_set_info_t    * global_sk_fd_set
)
{
  int            i,ret;
  ds_mux_sk_fd_set_info_t    sk_fd_array;
  ds_mux_sk_fd_set_info_t*   sk_fd_set               = NULL;
/*-------------------------------------------------------------------------*/

  while(TRUE)
  {
    memset(&sk_fd_array, 0, sizeof(ds_mux_sk_fd_set_info_t));
    sk_fd_array = *global_sk_fd_set;
    sk_fd_set = &sk_fd_array;
    FD_ZERO(&sk_fd_set->fdset);
    for (i = 0; i < sk_fd_set->num_fd; i++ )
    {
/*--------------------------------------------------------------------------
Check if the fd is valid before setting it in FD_SET
---------------------------------------------------------------------------*/
     if (fcntl(sk_fd_set->sk_fds[i].sk_fd, F_GETFL) < 0 && errno == EBADF)
       continue;
      FD_SET(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
      LOGI("Listening FD set %d", sk_fd_set->sk_fds[i].sk_fd);
    }
    LOGI("Num FD set %d", sk_fd_set->num_fd);

/*--------------------------------------------------------------------------
    Call select system function which will listen to netlink events
    coming on socket which we would have opened during
    initialization
--------------------------------------------------------------------------*/
    if ((ret = select(sk_fd_set->max_fd+1,
                     &(sk_fd_set->fdset),
                     NULL,
                     NULL,
                     NULL)) < 0)
    {
      LOG_MSG_ERROR("qcmap select failed %d", errno, 0, 0);
      if (EINTR == errno)
      {
        LOG_MSG_ERROR("%s(): select() was interrupted, continuing", __func__, 0, 0);
        continue;
      }
    }
    else
    {
      for (i = 0; i < sk_fd_set->num_fd; i++ )
      {
        LOG_MSG_INFO1("qcmap select fd set", 0, 0, 0);
        if (FD_ISSET(sk_fd_set->sk_fds[i].sk_fd,
                     &(sk_fd_set->fdset)))
        {
          if(sk_fd_set->sk_fds[i].read_func)
          {
            LOG_MSG_INFO1(" read fd set %d", i, 0, 0);
            if (DS_MUX_SUCCESS_MACRO !=
               ((sk_fd_set->sk_fds[i].read_func)(sk_fd_set->sk_fds[i].sk_fd)))
            {
              LOG_MSG_ERROR("Error on read callback[%d] fd=%d",
                            i,
                            sk_fd_set->sk_fds[i].sk_fd,
                            0);
              FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
            }
            else
            {
              FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
            }
          }
          else
          {
            LOG_MSG_ERROR("No read function",0,0,0);
          }
        }
      }
    }
  }
  return DS_MUX_SUCCESS_MACRO;
}

void ds_mux_util_prepare_data_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  ds_mux_io_frame_type       *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  output_frame->dlci             = dlci;
  output_frame->frame_type       = frame_type;
  output_frame->command_response = FALSE;
  output_frame->poll_final       = FALSE;

}/* ds_mux_util_prepare_data_frame */

uint_32 ds_mux_util_convert_smd_to_dlci(int fd)
{
  if(fd == smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX])
    return DS_MUX_DLCI_ID_1;
  else if(fd == smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX])
    return DS_MUX_DLCI_ID_2;
  else if(fd == smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX])
    return DS_MUX_DLCI_ID_3;
  else
  {
    LOG_MSG_ERROR("Invalid DLCI-ID", 0, 0, 0);
    return 999;
  }
}

ds_mux_frame_type_enum_type ds_mux_util_convert_frame_type
(
  ds_mux_subset_enum_type cmux_frame_type
)
{
  ds_mux_frame_type_enum_type return_frame;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( cmux_frame_type )
  {
    case DS_MUX_SUBSET_UIH:
      return_frame = DS_MUX_FRAME_TYPE_UTIL_UIH;
    break;

    case DS_MUX_SUBSET_UI:
      return_frame = DS_MUX_FRAME_TYPE_UTIL_UI;
    break;

    default:
     return_frame = DS_MUX_FRAME_TYPE_INVALID_FRAME;
    break;
  }
  return return_frame;

}/* ds_mux_util_convert_frame_type */

ds_mux_result_enum_type ds_mux_prepare_send_uih_ui_frame(int fd, char *buf, int_32 len)
{
  ds_mux_io_frame_type          msg_rsp_frame;
  int_32                        frame_length;
  int16                         ret = DS_MUX_SUCCESS;
  boolean                       fc;
  uint8                         dlci_id;
  ds_mux_dlci_param_type*       dlci_conn_param = NULL;

  memset(&msg_rsp_frame, 0, sizeof(ds_mux_io_frame_type));

  LOG_MSG_INFO1("Len: %d", len, 0, 0);
  dlci_id = ds_mux_util_convert_smd_to_dlci(fd);
  dlci_conn_param = ds_mux_get_dlci_conn_ptr(dlci_id);


  if(dlci_conn_param == NULL)
  {
    LOG_MSG_ERROR("Invalid DLCI-ID", 0, 0, 0);
    return DS_MUX_FAILURE;
  }

  if( (dlci_conn_param->frame_size_N1 - DS_MUX_FRM_SZ_EXPT_INFO_FLD) < len || len > DS_MUX_CMUX_MAX_FRAME_N1 )
  {
    LOG_MSG_ERROR("configured frame size %d is smaller than the actual frame size %d", dlci_conn_param->frame_size_N1, len + DS_MUX_FRM_SZ_EXPT_INFO_FLD, 0);
    return DS_MUX_FAILURE;
  }

  if(ds_mux_is_flow_enabled (DS_MUX_DLCI_ID_0))
  {
    int n = ds_mux_get_mux_state();
    LOG_MSG_INFO1("state %d", n, 0, 0);
    if(DS_MUX_STATE_IN_POWER_SAVE == ds_mux_get_mux_state())
    {
      LOG_MSG_INFO1("POWER SAVE", 0, 0, 0);
      if(DS_MUX_SUCCESS != ds_mux_init_wakeup_process(dlci_id))
      {
        LOG_MSG_ERROR("Could not initialize wakeup procedure", 0, 0, 0);
        return DS_MUX_FAILURE;
      }
  }
    if(ds_mux_is_flow_enabled (dlci_id))
    {

      msg_rsp_frame.information_ptr = (char *)calloc(1, dlci_conn_param->frame_size_N1 - DS_MUX_FRM_SZ_EXPT_INFO_FLD + DS_MUX_FEW_EXTRA_BYTES);
      if(msg_rsp_frame.information_ptr == NULL)
      {
        LOG_MSG_ERROR("Could-Not allocate memory %d", dlci_conn_param->frame_size_N1, 0, 0);
        return DS_MUX_FAILURE;
      }
      ds_mux_util_prepare_data_frame(dlci_id, ds_mux_util_convert_frame_type(dlci_conn_param->frame_type), &msg_rsp_frame);


      msg_rsp_frame.length = len;

      memcpy(msg_rsp_frame.information_ptr, buf, msg_rsp_frame.length);

      ret = ds_mux_io_transmit_msg_response(&msg_rsp_frame);

      if(ret != DS_MUX_SUCCESS)
      {
        LOG_MSG_ERROR("Could-not transmit the frame", 0, 0, 0)
      }

      LOG_MSG_INFO1("Successfully sent the frame to UART", 0, 0, 0);
    }
  }
  if(msg_rsp_frame.information_ptr != NULL)
  {
    LOG_MSG_INFO1("De-allocating the info ptr of the frame", 0, 0, 0);
    DS_MUX_MEM_FREE(msg_rsp_frame.information_ptr);
    msg_rsp_frame.information_ptr = NULL;
  }

  return ret;

}

ds_mux_io_frame_type* ds_mux_util_alloc_frame
(

)
{
  ds_mux_io_frame_type*  frame_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  frame_ptr = (ds_mux_io_frame_type*)malloc(sizeof(ds_mux_io_frame_type));
  if ( NULL == frame_ptr )
  {
    LOG_MSG_ERROR("ds_mux_util_decode_input(): Failed: memory alloc for frame ",0,0,0);
    return NULL;
  }

  memset( frame_ptr, 0x0, sizeof(ds_mux_io_frame_type) );
  frame_ptr->information_ptr = NULL;
  return frame_ptr;

}/* ds_mux_util_alloc_frame */


/* Initial value of FCS according to spec.*/
#define DS_MUX_UTIL_FCS_INIT_VAL 0xFFu

ds_mux_result_enum_type ds_mux_utils_calc_fcs
(
  char*                          info_ptr,
  uint16                         info_len,
  uint8                          curr_fcs,
  uint8*                         fcs_field
)
{
  uint8                           temp_fcs      = curr_fcs;
  uint8                           octet_buf     = 0;

  char*                           info_ptr_curr = info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    1. Start FCS calculation over whole buffer
    2. Calculate one's complement
  -----------------------------------------------------------------------*/

  /* Step1 */
  while (info_len--)
  {
    temp_fcs = ds_mux_util_crc_table[temp_fcs ^ *info_ptr_curr++];
  }
  /* Step2 */
  temp_fcs = DS_MUX_UTIL_FCS_INIT_VAL - temp_fcs;

  *fcs_field = temp_fcs;

  return DS_MUX_SUCCESS;
}/* ds_mux_utils_calc_fcs */


ds_mux_result_enum_type ds_mux_util_stop_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  uint_32 stop_index;
  if(ea_bit_length_field)
    stop_index = DS_MUX_INFO_FLD_INDEX_WHEN_EA_IS_SET + rx_info->rx_frame->length + 1;
  else
    stop_index = DS_MUX_INFO_FLD_INDEX + + rx_info->rx_frame->length + 1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(rx_info == NULL || frame_info == NULL)
   {
     LOG_MSG_ERROR("Invalid params rx_info: %p, frame infp: %p ", rx_info, frame_info, 0);
     return DS_MUX_FAILURE;
   }

  /*-----------------------------------------------------------------------
    1. Pull out one character from item
    2. Move into START PARSED only when we receive F9 flag
       2.1 Otherthen F9 flag, Any char will be ignored and continue to search F9.
  -----------------------------------------------------------------------*/

   LOG_MSG_INFO1("Stop index: %X", frame_info[stop_index], 0, 0);

    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == frame_info[stop_index] )
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_STOP_PARSED;
      LOG_MSG_INFO1("Frame state set to FRAME_STOP_PARSED", 0, 0, 0);
    }
    else
    {
      result = DS_MUX_FAILURE;
      LOG_MSG_ERROR("NO stop Flag found for the frame", 0, 0, 0);
    }
  return result;
}


ds_mux_result_enum_type ds_mux_util_fcs_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from item
     2. Verify end flag (f9)
     3. Once END FLAG is verified store current pased frame into queue and set
        current frame pointer to NULL
     Note: Frame memory will be freed after processing in DS_MUX_IO
   -----------------------------------------------------------------------*/

    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == curr_char )
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_STOP_PARSED;
      LOG_MSG_INFO1("STOP FLAG found and parsed", 0, 0, 0);

    }
    else
    {
     LOG_MSG_ERROR("NO STOP FLAG FOUND", 0, 0, 0);
      result = DS_MUX_FAILURE;
    }

  return result;
}/* ds_mux_util_info_parsed */


ds_mux_result_enum_type ds_mux_util_info_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  int initial_index = 0;
  int fcs_index;

  if(ea_bit_length_field)
  {
    LOG_MSG_INFO1("EA IS SET", 0, 0, 0);
    fcs_index =  DS_MUX_INFO_FLD_INDEX_WHEN_EA_IS_SET + rx_info->rx_frame->length;
  }
  else
  {
    LOG_MSG_INFO1("EA is not set", 0, 0, 0);
    fcs_index = DS_MUX_INFO_FLD_INDEX + rx_info->rx_frame->length;
  }

   byte curr_char = frame_info[fcs_index];

   LOG_MSG_INFO1("fcs filed: %X", curr_char, 0, 0);
   LOG_MSG_INFO1("rx_info->rx_frame->fcs = %d", rx_info->rx_frame->fcs, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from item
     2. Get FCS value from FCS octet
     3. Compare FCS calculate on the frame with the FCS mentioned in the frame
   -----------------------------------------------------------------------*/
   LOG_MSG_INFO1("Frame FCS: %X , Calculated FCS : %X",curr_char , rx_info->rx_frame->fcs, 0);
    if ( curr_char != rx_info->rx_frame->fcs )
    {
      LOG_MSG_ERROR("Checksum failed", 0, 0, 0);
      //Check Sum failed
      result = DS_MUX_FAILURE;
    }
    else
    {
      LOG_MSG_INFO1("FCS VALIDATION SUCCESSFULL....FRAME IS CORRECT", 0, 0, 0);
    }
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_FCS_PARSED;
    LOG_MSG_INFO1("FCS parsed", 0, 0, 0);
  return result;
}/* ds_mux_util_info_parsed */

ds_mux_result_enum_type process_fragmented_frame(ds_mux_io_rx_info*         rx_info, int len_info_field, char *info_ptr)
{
  ds_mux_result_enum_type result = DS_MUX_SUCCESS;
  static int index = 0;

  if(NULL == rx_info || NULL == info_ptr)
  {
    LOG_MSG_ERROR("frame or information ptr is NULL rx_info: %p  info_ptr: %p",
                   rx_info, info_ptr, 0);
    return DS_MUX_FAILURE;
  }
  char* dest = rx_info->rx_frame->information_ptr +index;

  if(rx_info == NULL || info_ptr == NULL || len_info_field == 0)
  {
    LOG_MSG_ERROR("frame: %p  info_ptr: %p  legth of info field: %d", rx_info, info_ptr, len_info_field);
    result = DS_MUX_FAILURE;
  }

  memcpy(dest, info_ptr, len_info_field);
  index = index + len_info_field;

  if(rem_info_len == 0)
  {
    LOG_MSG_INFO1("Full frame parsed", 0, 0, 0);
    frag_frame_check = FALSE;
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_INFO_PARSED;
  }
  return result;
}



ds_mux_result_enum_type ds_mux_util_len_parsed
(
  ds_mux_io_rx_info*  rx_info,
  char*               frame_info,
  int                 frame_len
)
{
  int             index_info;
  uint16          len_info_field;

  LOG_MSG_INFO1("Processing Information if Present", 0, 0, 0);
  LOG_MSG_INFO1("Frame info: HEX: %X  dec:%d", frame_len, frame_len, 0);

  if(ea_bit_length_field)
  {
     LOG_MSG_INFO1("EA bit is set in length field", 0, 0, 0);
     index_info = DS_MUX_INFO_FLD_INDEX_WHEN_EA_IS_SET;
     len_info_field = frame_len - (DS_MUX_MIN_FRAME_SZ);
     LOG_MSG_INFO1("LENGTH OF INFO FIELD is hex: %X dec:%d",len_info_field ,len_info_field, 0);
  }
  else
  {
    LOG_MSG_INFO1("EA bit is NOT set in length field", 0, 0, 0);
    index_info = DS_MUX_INFO_FLD_INDEX;
    len_info_field = frame_len - (DS_MUX_MIN_FRAME_SZ + 1);
  }

  char *info_ptr = frame_info + index_info;
  LOG_MSG_INFO1("*info_ptr_: %x %x %x",info_ptr[0] ,info_ptr[1], info_ptr[2]);


  int                             address_index                     = DS_MUX_ADDRESS_FLD_INDEX;
  char*                           address_ptr                       = frame_info + DS_MUX_ADDRESS_FLD_INDEX;
  int                             address_plus_cntl_field_len       = DS_MUX_CNTL_PLUS_ADDR_FLD_LEN;

  ds_mux_result_enum_type         result                            = DS_MUX_SUCCESS;
  uint_32                         rem_info_len                      = 0;
  uint8                           temp_fcs                          = 0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {

    //Info field is not present (e.g commands)
    LOG_MSG_INFO1("Length of information field is %d", rx_info->rx_frame->length, 0, 0);
    if ( 0 == rx_info->rx_frame->length)
    {
      LOG_MSG_INFO1("Info field is not present", 0, 0, 0);
      //commands doesnt have information field
      rx_info->rx_frame->information_ptr = NULL;
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_INFO_PARSED;
      break;
    }
    else
    {
       LOG_MSG_INFO1("Frame has info field", 0, 0, 0);
       if(rx_info->rx_frame->information_ptr == NULL)
       {
         LOG_MSG_INFO1("info field is NULL", 0, 0, 0);
         return DS_MUX_FAILURE;
       }
       if(len_info_field < rx_info->rx_frame->length )
       {
         frag_frame_check = TRUE;
         rem_info_len = rx_info->rx_frame->length - len_info_field;
         if(DS_MUX_SUCCESS != process_fragmented_frame(rx_info, len_info_field, info_ptr))
         LOG_MSG_ERROR("couldnot process the fragmented frame", 0, 0, 0);
       }
       else
       {
         memcpy(rx_info->rx_frame->information_ptr, info_ptr, rx_info->rx_frame->length);
         rx_info->curr_frame_state = DS_MUX_IO_FRAME_INFO_PARSED;
         LOG_MSG_INFO1("Info Field parsed", 0, 0, 0);
       }
    }

  }while(0);

    //We got the complete infromation hence we can do check sum on complete packet
    //Calculate FCS on info (Applicable for UIH frame only)
    if(DS_MUX_IO_FRAME_INFO_PARSED == rx_info->curr_frame_state)
    {
      LOG_MSG_INFO1("Calculating FCS for info filed if present", 0, 0, 0);
      if ( (0 != rx_info->rx_frame->length) && (DS_MUX_FRAME_TYPE_UTIL_UI == rx_info->rx_frame->frame_type) )
      {
        LOG_MSG_INFO1("Frame type is UI", 0, 0, 0);
        result = ds_mux_utils_calc_fcs(
              rx_info->rx_frame->information_ptr ,
              rx_info->rx_frame->length,
              rx_info->rx_frame->fcs ,
              &temp_fcs);

        rx_info->rx_frame->fcs = temp_fcs;
      }
      else if ((0 == rx_info->rx_frame->length))
      {
        //For both UI and UIH
        LOG_MSG_INFO1("Info filed is 0, No need to calculate FCS on info filed", 0, 0, 0);
        //as we are not calculating fcs on info, we should take one's complement of existing fcs
        /* One's complement */
        rx_info->rx_frame->fcs = DS_MUX_UTIL_FCS_INIT_VAL - rx_info->rx_frame->fcs;
        LOG_MSG_INFO1("rx_info->rx_frame->fcs = %X", rx_info->rx_frame->fcs, 0, 0);
      }
      else if (rx_info->rx_frame->frame_type == DS_MUX_FRAME_TYPE_UTIL_UIH)
      {
        LOG_MSG_INFO1("Frame type is UIH", 0, 0, 0);
        rx_info->rx_frame->fcs = DS_MUX_UTIL_FCS_INIT_VAL - rx_info->rx_frame->fcs;
        LOG_MSG_INFO1("rx_info->rx_frame->fcs = %X", rx_info->rx_frame->fcs, 0, 0);
      }
    }
  return result;
}/* ds_mux_util_len_parsed */

ds_mux_frame_type_enum_type ds_mux_util_get_frame_type_from_ctrl_octet
(
    uint8 ctrl_octet
)
{
  ds_mux_frame_type_enum_type ret_frame_type                  = DS_MUX_FRAME_TYPE_INVALID_FRAME;
  uint8                       ctrl_octet_wo_pf                = ctrl_octet & (~DS_MUX_UTIL_CTRL_OCTET_PF_MASK);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Convert CTRL Command into FRAME type
  -----------------------------------------------------------------------*/
  switch(ctrl_octet_wo_pf)
  {
    case DS_MUX_UTIL_CTRL_OCTET_SABM_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_SABM;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UA_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UA;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_DM_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_DM;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_DISC_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_DISC;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UIH_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UIH;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UI_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UI;
      break;
    }
    default:
    {
      break;
    }
  }

  return ret_frame_type;

}/* ds_mux_util_get_frame_type_from_ctrl_octet */


ds_mux_result_enum_type ds_mux_util_addr_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{
  ds_mux_result_enum_type            result                      = DS_MUX_SUCCESS;
  int                                control_index               = DS_MUX_CNTL_FLD_INDEX;
  byte                               curr_char                   = frame_info[control_index];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from item
     2. Get Frame type from control octet
     3. Calculate poll/final bit
     4. Move into Next step
     5. Calculate FCS
   -----------------------------------------------------------------------*/
    LOG_MSG_INFO1("Processing Control field", 0, 0, 0);
    LOG_MSG_INFO1("address filed: %X", curr_char, 0, 0);
    /*Step 2*/
    rx_info->rx_frame->frame_type = ds_mux_util_get_frame_type_from_ctrl_octet(curr_char);
    LOG_MSG_INFO1("Frame Type is: %d",rx_info->rx_frame->frame_type , 0, 0);

    if ( DS_MUX_FRAME_TYPE_INVALID_FRAME == rx_info->curr_frame_state)
    {
      LOG_MSG_ERROR("ds_mux_util_addr_parsed(): Invalid frame type ",0,0,0);
      result = DS_MUX_FAILURE;
    }
    else
    {
      /*Step 3*/
      rx_info->rx_frame->poll_final = ( curr_char & DS_MUX_UTIL_CTRL_OCTET_PF_MASK ) ? TRUE : FALSE;
      //Store Ctrl byte
      rx_info->rx_frame->control_field = curr_char;
      /*Step 4*/
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_CTRL_PARSED;
      /*Step 5*/
      rx_info->rx_frame->fcs = ds_mux_utils_calc_fcs_per_char (curr_char, rx_info->rx_frame->fcs );
      LOG_MSG_INFO1("temp fcs %X", rx_info->rx_frame->fcs, 0, 0);
    }
  return result;
}/* ds_mux_util_addr_parsed */



ds_mux_result_enum_type ds_mux_util_ctrl_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{

  uint16                          len_index           = DS_MUX_LEN_FLD_INDEX;
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  byte                            curr_char           = frame_info[len_index]; //length is one byte

  len_index = len_index+1;
  byte                            ext_len_char        = frame_info[len_index];//if length is 2 byte
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     1. Pull out one character from item
     2. Length can be in 1 or 2 byte based on EA bit
        2.1 if EA bit is 1 then length is in 1 byte
        2.2 if EA bit is 0 thne length is in 2 byte
     3. Calculate length from one or two byte
     4. Move into Next step
     5. Calculate FCS on length bytes
   -----------------------------------------------------------------------*/

  LOG_MSG_INFO1("Processing LENGTH field", 0, 0, 0);
  LOG_MSG_INFO1("length filed: %X", curr_char, 0, 0);

    /*Step 2*/
    if ( 1 == (curr_char & DS_MUX_UTIL_OCTET_EA_MASK))
    {
      LOG_MSG_INFO1("EA bit is set in the length field.", 0, 0, 0);
      rx_info->rx_frame->length_wt_ea = curr_char;
      /*Step 2.1*/
      rx_info->rx_frame->length = (uint16)(curr_char >> 1);
      /*Step 5*/

        rx_info->rx_frame->fcs = ds_mux_utils_calc_fcs_per_char (curr_char, rx_info->rx_frame->fcs);
        LOG_MSG_INFO1("temp fcs %X", rx_info->rx_frame->fcs, 0, 0);

      rx_info->curr_frame_state = DS_MUX_IO_FRAME_LEN_PARSED;
      ea_bit_length_field = TRUE;
    }
    else
    {
        LOG_MSG_INFO1("EA bit is not set in the length field", 0, 0, 0);
        /*Step 2.2*/
        rx_info->rx_frame->length = (uint16)(ext_len_char);
        rx_info->rx_frame->length <<= 7;
        rx_info->rx_frame->length |= (uint16)(curr_char >> 1);
        /*Step 4*/
        rx_info->curr_frame_state = DS_MUX_IO_FRAME_LEN_PARSED;
        /* Check against MAX length */
        if ( rx_info->rx_frame->length > DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN )
        {
         LOG_MSG_ERROR("Frame length is greater than 2^15. Not possible!!!", 0, 0, 0);
          result  = DS_MUX_FAILURE;
        }
        /*Step 5*/
        if(rx_info->rx_frame->frame_type == DS_MUX_FRAME_TYPE_UTIL_UI)
        {
          LOG_MSG_INFO1("Frame  type is UI", 0, 0, 0);
          rx_info->rx_frame->fcs = ds_mux_utils_calc_fcs_per_char (
                                               curr_char, rx_info->rx_frame->fcs);
        //Calculate fcs on second length byte
          rx_info->rx_frame->fcs = ds_mux_utils_calc_fcs_per_char (
                                               ext_len_char, rx_info->rx_frame->fcs );
        }
        ea_bit_length_field = FALSE;
      }
  return result;
}/* ds_mux_util_ctrl_parsed */


uint8 ds_mux_utils_calc_fcs_per_char
(
  uint8                          octet_buf,
  uint8                          curr_fcs
)
{
  uint8                           temp_fcs;
  LOG_MSG_INFO1("Calculating FCS", 0, 0, 0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  temp_fcs = ds_mux_util_crc_table[curr_fcs ^ octet_buf];

  return temp_fcs;
}

ds_mux_result_enum_type ds_mux_util_flag_parsed
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  // To go to address field in a frame.
  uint16                          index               = DS_MUX_ADDRESS_FLD_INDEX;
  byte                            curr_char           = frame_info[index];


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     1. Pull out one character from item
     2. Stay into START PARSED  when we receive F9 flag again
     3. Generate DLCI ID
     4. EA bit should set to 1 as we are suppoting MAX DLCI ID till 63 only
     5. Calculate C/R bit. Initiator should set command bit to 1
     6. Generate DLCI ID from remaining 6 bits( remove EA and CR bit by rightshift)
     7. Move into Next step
     8. Calculate FCS
   -----------------------------------------------------------------------*/
      LOG_MSG_INFO1("Processing Address Field", 0, 0, 0);
      LOG_MSG_INFO1("frame_info[%d] = %X curr_char : %X", index, frame_info[index], curr_char);

      if ( 0 == (curr_char & DS_MUX_UTIL_OCTET_EA_MASK))
      {
         LOG_MSG_ERROR("ds_mux_util_decode_input(): EA bit is NOT set in address",0 ,0, 0);
         result   = DS_MUX_FAILURE;
      }
      else
      {
        LOG_MSG_INFO1("EA bit is set in address field", 0, 0, 0);
        /*Step 5*/
        rx_info->rx_frame->command_response = ( curr_char & DS_MUX_UTIL_ADDR_OCTET_CR_MASK ) ?
        TRUE : FALSE;

        /*Step 6*/
        rx_info->rx_frame->dlci = (curr_char >> 2);
        LOG_MSG_INFO1("DLCI ID: %d", rx_info->rx_frame->dlci, 0, 0);
        /*Step 7*/
        rx_info->curr_frame_state = DS_MUX_IO_FRAME_ADDR_PARSED;
         /*Step 8*/
        rx_info->rx_frame->fcs = ds_mux_utils_calc_fcs_per_char(curr_char,
                                                                DS_MUX_UTIL_FCS_INIT_VAL);
        LOG_MSG_INFO1("temp fcs %X", rx_info->rx_frame->fcs, 0, 0);
      }
  return result;
}/* ds_mux_util_flag_parsed */


ds_mux_result_enum_type ds_mux_util_start_parsed
(
 ds_mux_io_rx_info* rx_info,
 char* frame_info
)
{
  ds_mux_result_enum_type         result          = DS_MUX_SUCCESS;
  uint16                          index           = DS_MUX_START_FLAG_FLD_INDEX;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("Processing the START flag", 0, 0, 0);

   if(rx_info == NULL || frame_info == NULL)
   {
     LOG_MSG_ERROR("Invalid params rx_info: %p, frame infp: %p ", rx_info, frame_info, 0);
     return DS_MUX_FAILURE;
   }

   LOG_MSG_INFO1("frame_info[0] = %X", frame_info[0], 0, 0);

  /*-----------------------------------------------------------------------
    1. Pull out one character from item
    2. Move into START PARSED only when we receive F9 flag
       2.1 Otherthen F9 flag, Any char will be ignored and continue to search F9.
  -----------------------------------------------------------------------*/

    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == frame_info[index] )
    {
      LOG_MSG_INFO1("START flag found in the frame", 0, 0, 0);
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_START_PARSED;
    }
    else
    {
      LOG_MSG_ERROR("Fame start dosent contain 0XF9", 0, 0, 0);
      result = DS_MUX_FAILURE;
    }
  return result;
}/* ds_mux_util_addr_parsed */

ds_mux_result_enum_type ds_mux_util_process_power_save_flag
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info,
  int packet_len
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  uint_32                         cnt                 = 0;
  uint8                           flag_byte           = 0;
  boolean                         wake_up_flag_found  = FALSE;
  uint16                          index               = 0;
  uint16                          cnt_flag            = 0;
  ds_mux_dlci_param_type*         dlci_conn_dlci0     = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rx_info == NULL || frame_info == NULL)
  {
    LOG_MSG_ERROR("Invalid params rx_info: %p, frame infp: %p ", rx_info, frame_info, 0);
    return DS_MUX_FAILURE;
  }

  if ( 0 != packet_len )
  {
    for ( cnt = 0; cnt < 3; cnt++)
    {
      flag_byte = frame_info[index++];
      if (DS_MUX_UTIL_START_STOP_FLAG_CODE == flag_byte)
      {
        cnt_flag++;
        if( cnt_flag == 3 )
        {
          wake_up_flag_found = TRUE;
          break;
        }
      }
     }
  }

  if(cnt_flag != 3)
  {
    LOG_MSG_ERROR("Incorrect PowerSaveFlag:There is no sequence of flags in the frame", 0, 0, 0);
    result = DS_MUX_FAILURE;
  }
  /*-----------------------------------------------------------------------
     Send same wake up sequnce to host
   -----------------------------------------------------------------------*/
  if (TRUE  == wake_up_flag_found)
  {
    dlci_conn_dlci0 = ds_mux_get_dlci_conn_ptr(DS_MUX_DLCI_ID_0);
    if(NULL == dlci_conn_dlci0)
    {
      LOG_MSG_ERROR("dlci_conn0 is NULL", 0, 0, 0);
      return DS_MUX_FAILURE;
    }
    LOG_MSG_INFO1("Hi %d %d",dlci_conn_dlci0->psc_timer_data.is_running, 0, 0);
    if(dlci_conn_dlci0->psc_timer_data.is_running)
    {
      ds_mux_timer_stop(&dlci_conn_dlci0->psc_timer_data);
      dlci_conn_dlci0->psc_timer_data.is_running = FALSE;
    }
    ds_mux_uart_tty_send_msg(frame_info, packet_len );
    ds_mux_set_mux_state(DS_MUX_STATE_WAKEUP_FLAG_SENT);
  }

  return result;

}/* ds_mux_util_process_power_save_flag */

ds_mux_result_enum_type ds_mux_util_process_frame_after_wake_up
(
  ds_mux_io_rx_info* rx_info,
  char* frame_info,
  int packet_len

)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  uint_32                         cnt                 = 0;
  uint8                           flag_byte           = 0;
  boolean                         valid_frame_found   = FALSE;
  uint8                           dlci_id;
  uint16                          index               = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rx_info == NULL || frame_info == NULL)
  {
    LOG_MSG_ERROR("Invalid params rx_info: %p, frame infp: %p ", rx_info, frame_info, 0);
    return DS_MUX_INVALID_PARAM;
  }

  if ( 0 != packet_len )
  {
    for ( cnt = 0; cnt < packet_len; cnt++)
    {
      flag_byte = frame_info[index++];
      if (DS_MUX_UTIL_START_STOP_FLAG_CODE != flag_byte)
      {
        valid_frame_found = TRUE;
        break;
      }
    }
  }
  /*-----------------------------------------------------------------------
     Found character other then F9, Try to decode the frame. If it is valid frame then
     come out from powersave
   -----------------------------------------------------------------------*/
  if (TRUE == valid_frame_found)
  {
    result = ds_mux_util_decode_input(rx_info, frame_info);

    if ( DS_MUX_SUCCESS == result)
    {
      ds_mux_set_mux_state(DS_MUX_STATE_OPENED);

      for(dlci_id = DS_MUX_DLCI_ID_1;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
      {
        ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PSC );

      }
    }
    else
    {
      LOG_MSG_ERROR("ds_mux_util_decode_input(): DS MUX is expecting valid frame", 0, 0, 0);
      result = DS_MUX_FAILURE;
    }
  }
  else //send wake up sync
  {
    ds_mux_uart_tty_send_msg( frame_info, packet_len );
  }
  return result;
}/* ds_mux_util_process_power_save_flag */

void ds_mux_util_prepare_cmd_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     is_command,
  boolean                     is_final,
  ds_mux_io_frame_type       *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(output_frame == NULL)
  {
    LOG_MSG_ERROR("Output frame is NULL", 0, 0, 0);
    return;
  }

  output_frame->dlci             = dlci;
  output_frame->frame_type       = frame_type;

  if ( TRUE == is_command )
  {
    LOG_MSG_INFO1("It is a command", 0, 0, 0);
    output_frame->command_response = (TRUE == ds_mux_is_initiator()) ? TRUE : FALSE;
  }
  else
  {
    //Incase of response; Responder will set bit 1
    LOG_MSG_INFO1("It is not initialtor.we are sending response", 0, 0, 0);
    output_frame->command_response = (TRUE == ds_mux_is_initiator()) ? FALSE : TRUE;
  }
  if (TRUE == is_final)
  {
    LOG_MSG_INFO1("Final bit is set", 0, 0, 0);
    output_frame->poll_final       = TRUE;
  }
  else
  {
    LOG_MSG_INFO1("Poll bit is set", 0, 0, 0);
    output_frame->poll_final       = FALSE;
  }

}/* ds_mux_util_prepare_cmd_frame */

ds_mux_result_enum_type ds_mux_util_generate_control_field
(
  uint8*                      control_fld_ptr,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     pf_bit
)
{
  ds_mux_result_enum_type     result    = DS_MUX_SUCCESS;

  LOG_MSG_INFO1("Generating Control Field", 0, 0, 0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(frame_type)
  {
    case DS_MUX_FRAME_TYPE_UTIL_SABM:
    {
       LOG_MSG_INFO1("Frame is SABM", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_SABM_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UA:
    {
       LOG_MSG_INFO1("Frame is UA", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UA_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_DM:
    {
       LOG_MSG_INFO1("Frame is DM", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_DM_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_DISC:
    {
       LOG_MSG_INFO1("Frame is DISC", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_DISC_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UIH:
    {
       LOG_MSG_INFO1("Frame is UIH", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UIH_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UI:
    {
      LOG_MSG_INFO1("Frame is UI", 0, 0, 0);
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UI_CODE_WO_PF ;
      break;
    }
    default:
    {
      result    = DS_MUX_FAILURE;
      break;
    }
  }

  if ( DS_MUX_SUCCESS == result && 1 == pf_bit)
  {
    LOG_MSG_INFO1("Inserting PF bit in control field", 0, 0, 0);
    *control_fld_ptr |= DS_MUX_UTIL_CTRL_OCTET_PF_MASK;
  }

  return result;
} /* ds_mux_util_generate_control_field() */

ds_mux_result_enum_type ds_mux_util_insert_len
(
  char*                      item_ptr,
  uint16                     length,
  uint_32*                   index
)
{
  uint8                           len_field     = 0;
  uint8                           len_sec_octet = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( length <= 127 )
  {
    // Insert EA bit
    len_field   = length << 1;
    len_field  |= DS_MUX_UTIL_OCTET_EA_MASK;

    //Insert length byte at back
    item_ptr[(*index)++] = len_field;
  }
  else if (length <= DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN )
  {
    len_sec_octet = (uint8)(length >> 7);
    len_field     = (uint8)((length << 1) & (0x00fe));
    item_ptr[(*index)++] = len_field;
    item_ptr[(*index)++] = len_sec_octet;
  }
  else
  {
    LOG_MSG_ERROR("ds_mux_util_insert_len(): Invalid len %d", length, 0, 0);
    return DS_MUX_FAILURE;
  }
  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_len*/



ds_mux_result_enum_type ds_mux_util_generate_fcs
(
  ds_mux_frame_type_enum_type    frame_type,
  uint16                         frame_info_len,
  uint8*                         fcs_field,
  char*                          item_ptr
)
{
  uint16                          total_pkt_len = 2;// Initialized with 2 (Address + ctrl byte) and length
  uint8                           index = DS_MUX_ADDRESS_FLD_INDEX;
  char*                           cal_fcs_frm = item_ptr + index;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // Calculate bytes for length
  if ( frame_info_len < 127)
  {
    //Length is 1 byte long
    LOG_MSG_INFO1("length field is 1 byte", 0, 0, 0);
    total_pkt_len += 1;
  }
  else
  {
    //Length is two byte long
    LOG_MSG_INFO1("length field is 2 byte", 0, 0, 0);
    total_pkt_len += 2;
  }

  /*-----------------------------------------------------------------------
    In case of UI frame include info length as well
  -----------------------------------------------------------------------*/
  if( DS_MUX_FRAME_TYPE_UTIL_UI == frame_type )
  {
    total_pkt_len += frame_info_len;
  }

  if ( 0 != total_pkt_len &&  DS_MUX_FAILURE ==  ds_mux_utils_calc_fcs(
              cal_fcs_frm, total_pkt_len, DS_MUX_UTIL_FCS_INIT_VAL, fcs_field) )
  {
    return DS_MUX_FAILURE;
  }

  return DS_MUX_SUCCESS;
}/* ds_mux_util_generate_fcs */


ds_mux_result_enum_type ds_mux_util_insert_addr
(
  uint8*                   addr_field,
  uint8                    dlci,
  boolean                  command_response
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("Inserting Address field", 0, 0, 0);

  if (IS_DLCI_INVALID(dlci))
  {
    LOG_MSG_ERROR("Invalid DLCI ID", 0, 0, 0);
    return  DS_MUX_FAILURE;
  }

  *addr_field = dlci << 2;

  if ( TRUE == command_response)
  {
    LOG_MSG_INFO1("Inserting C/R bit in addr_field", 0, 0, 0);
    *addr_field |= DS_MUX_UTIL_ADDR_OCTET_CR_MASK;
  }

  *addr_field |= DS_MUX_UTIL_OCTET_EA_MASK;

  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_addr*/



ds_mux_result_enum_type ds_mux_util_encode_cmd_input
(
  char                     *item_ptr,
  ds_mux_io_frame_type     *frame
)
{
  ds_mux_result_enum_type         result           = DS_MUX_SUCCESS;
  uint_32                         index            = 0;
  uint8                           ctl_field        = 0;
  uint8                           addr_field       = 0;
  uint8                           fcs_field        = 0;
  uint16                          length           = 0;
  char*                           info_ptr         = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1("Preparing Frame", 0, 0, 0);
  do
  {
    if(NULL == item_ptr || NULL == frame)
    {
      LOG_MSG_INFO1("Invalid params", 0, 0, 0);
      result = DS_MUX_FAILURE;
      break;
    }

    /*-----------------------------------------------------------------------
      Order of API calls should be retained
    -----------------------------------------------------------------------*/

    item_ptr[index++] = DS_MUX_UTIL_START_STOP_FLAG_CODE;

    if ( DS_MUX_SUCCESS != ds_mux_util_insert_addr ( &addr_field,
                                                     frame->dlci,
                                                     frame->command_response))
      DS_MUX_SET_ERROR_AND_BREAK(result);

    item_ptr[index++] = addr_field;

    LOG_MSG_INFO1("Frame_type: %d", frame->frame_type, 0, 0);

    if ( DS_MUX_SUCCESS != ds_mux_util_generate_control_field( &ctl_field,
                                                               frame->frame_type,
                                                               frame->poll_final))
    {
      LOG_MSG_ERROR("Couldnot insert control field", 0, 0, 0);
      return DS_MUX_FAILURE;
    }
    LOG_MSG_INFO1("control field is %X", ctl_field, 0, 0);
    item_ptr[index++] = ctl_field;

    if ( DS_MUX_FAILURE == ds_mux_util_insert_len (item_ptr, frame->length, &index))
    {
      LOG_MSG_ERROR("Could-not insert length field", 0, 0, 0);
      DS_MUX_SET_ERROR_AND_BREAK(result);
    }

   LOG_MSG_INFO1("Frame len here:", frame->length, 0, 0);
    if(frame->length > 0)
    {
      LOG_MSG_INFO1("info field is: %d", frame->length, 0, 0);
      info_ptr = item_ptr + index;
      if(frame->information_ptr != NULL)
      {
        LOG_MSG_INFO1("Infomation field in transmit frame is filled", 0, 0, 0);
        memcpy(info_ptr, frame->information_ptr, frame->length );
        index = index + frame->length;
      }
    }

    if ( DS_MUX_SUCCESS != ds_mux_util_generate_fcs( frame->frame_type,
                                                     frame->length,
                                                     &fcs_field, item_ptr) )
    {
      LOG_MSG_ERROR("FCS failed", 0, 0, 0);
      return DS_MUX_FAILURE;
    }

    item_ptr[index++] = fcs_field;

    item_ptr[index] = DS_MUX_UTIL_START_STOP_FLAG_CODE;


    //TBD: read spec and Add extra F9 at the end for making len multiple of 4

  }while( 0 );

  return result;
}/* ds_mux_util_encode_cmd_input */

ds_mux_result_enum_type ds_mux_util_encode_msg_input
(
  char*                     item_ptr,
  ds_mux_io_frame_type*     frame
)
{
  ds_mux_result_enum_type         result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == item_ptr || NULL == frame)
    {
      result = DS_MUX_FAILURE;
      break;
    }
    /*-----------------------------------------------------------------------
      Order of API calls should be retained
    -----------------------------------------------------------------------*/
    result = ds_mux_util_encode_cmd_input( item_ptr, frame);
  }while( 0 );

  return result;
}/* ds_mux_util_encode_msg_input*/

int_32 ds_mux_convert_mili_to_sec(int_32 timer_val)
{
  timer_val = timer_val * pow(10, -3);
  return timer_val;
}

int_32 ds_mux_convert_sec_to_nano(int_32 timer_val)
{
  int_32 nano_timer_val = timer_val * pow(10, 9);
  return nano_timer_val;
}

static __inline int cmp_char_i(unsigned char c1, unsigned char c2)
{
  /* Convert UC to LC */
  if (('A' <= c1) && ('Z' >= c1))
  {
      c1 = c1 - 'A' + 'a';
  }
  if (('A' <= c2) && ('Z' >= c2))
  {
    c2 = c2 - 'A' + 'a';
  }
  return (c1 - c2);
}

int strncasecmpp(const char * s1, const char * s2, size_t n)
{
  unsigned char c1, c2;
  int diff;
  if (n > 0)
  {
    do
    {
      c1 = (unsigned char)(*s1++);
      c2 = (unsigned char)(*s2++);
      diff = cmp_char_i(c1, c2);
      if (0 != diff)
      {
        return diff;
      }
      if ('\0' == c1)
      {
        break;
      }
    } while (--n);
  }
  return 0;
}

int create_socket(unsigned int *sockfd)
{

  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Error creating socket, errno: %d", errno);
    return DS_MUX_FAILURE_MACRO;
  }

  if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOGI("Couldn't set Close on Exec, errno: %d", errno);
    close(*sockfd);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}

 int create_server_socket
(
  ds_mux_sk_info_t*  sk_info,
  char*              file_path
)
{
  int                  *p_sk_fd;
  int                   len;
  struct                sockaddr_un server;

   ds_assert(sk_info != NULL);

   p_sk_fd = &(sk_info->sk_fd);

/*--------------------------------------------------------------------------
  Open socket
+---------------------------------------------------------------------------*/
  if ((*p_sk_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOGI("Cannot open socket errno: %d", errno);
    return DS_MUX_FAILURE_MACRO;
  }
  memset (&server, 0, sizeof (server));
  server.sun_family = AF_UNIX;
  strlcpy (server.sun_path, file_path, sizeof(server.sun_path));
  unlink(server.sun_path);

/*-------------------------------------------------------------------------
  Bind socket to receive messages from atfwd
--------------------------------------------------------------------------*/
  len = strlen(server.sun_path) + sizeof(server.sun_family);
  if (bind (*p_sk_fd, (struct sockaddr *)&server, len) <0)
  {
    LOGI("Cannot bind socket errno: %d", errno);
    close(*p_sk_fd);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}
