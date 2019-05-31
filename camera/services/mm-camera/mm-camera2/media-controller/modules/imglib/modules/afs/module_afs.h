/**********************************************************************
* Copyright (c) 2013,2015 Qualcomm Technologies, Inc. All Rights Reserved. *
* Qualcomm Technologies Proprietary and Confidential.                 *
**********************************************************************/

#ifndef __MODULE_AFS_H__
#define __MODULE_AFS_H__

#include "img_common.h"
#include "module_imglib_common.h"
#include "camera_dbg.h"
#include "chromatix.h"
#include "chromatix_3a.h"
#define IMGLIB_MOD_NAME "afs"
#define MAX_AFS_STATIC_PORTS 4
#define MAX_NUM_FRAMES 20
#define MAX_NUM_AFS_FRAMES 4
#define MAX_AFS_STREAMS 4

/** MODULE_MASK:
 *
 * Mask to enable dynamic logging
 **/
#undef MODULE_MASK
#define MODULE_MASK IMGLIB_AFS_SHIFT

/** stream_port_mapping_t
 *   @stream_info: pointer to hold stream info
 *   @identity: MCT session/stream identity
 *   @p_sinkport: sink port associated with the stream
 *   @p_srcport: source port associated with the stream
 *   @p_frame: Frame buffers
 *   @p_map_buf: Buffers from MCT
 *   @stream_off: Flag to indicate if streamoff arrived
 *     for current stream
 *   @in_dim: AFS input dim associated with stream
 *
 *   afs stream-based params
 **/
typedef struct {
  mct_stream_info_t *stream_info;
  uint32_t identity;
  mct_port_t *p_sinkport;
  mct_port_t *p_srcport;
  img_frame_t p_frame[MAX_NUM_AFS_FRAMES];
  mct_stream_map_buf_t p_map_buf[MAX_NUM_FRAMES];
  uint8_t stream_off;
  img_dim_t in_dim;
} stream_port_mapping_t;

/** afs_client_t
 *   @mutex: client lock
 *   @frame_algo_mutex: frame algo lock
 *   @frame_algo_cond: signal to notify frame_algo is done
 *   @state: state of AFS detection client
 *   @main_dim: variable to store the sensor dimension
 *   @out_trans_info: translation from camif -> preview
 *   @streamon: false to indicate of streamon is issued
 *   @p_mod: module pointer
 *   @frame_skip_cnt: frame skip count
 *   @current_count: current count
 *   @buffer_cnt: Number of frames in current stream
 *   @buf_idx: current buffer index
 *   @crop_info: crop info
 *   @active: flag to indicate whether the client is active
 *   @camif_trans_info: translation for camif
 *   @sync: synchronous handling of frames
 *   @processing: flag to indicate whether PAAF is in progress
 *   @num_skip: number of frames to skip
 *   @cropped_window: dimension of cropped frame
 *   @frame_crop: flag to indicate if frame crop is enabled
 *   @buf_allocation_done: flag to indicate whether buf for
 *     frame is allocated
 *   @frame_dim: dimension of the frame passed to frame algo
 *   @stream: maping for all streams and ports connecting them
 *   @stream_cnt: number of streams mapped to client
 *   @event_identity: identity that current port event came with
 *   @stream_mask: mask of all streams associated with the client
 *   @active_str_identity: Identity of the stream on which AFS
 *                        is enabled/disabled
 *   @ref_count - Reference count of the number of streams that
 *    have been streamed on.
 *   @buf_divert_identity : Identity of the stream in which
 *                        buffer divert was recieved.
 *
 *   afs client structure
 **/
typedef struct {
  pthread_mutex_t mutex;
  pthread_mutex_t frame_algo_mutex;
  pthread_cond_t  frame_algo_cond;
  imglib_state_t state;
  img_size_t main_dim;
  img_trans_info_t out_trans_info;
  int8_t streamon;
  void *p_mod;
  uint32_t frame_skip_cnt;
  uint32_t current_count;
  img_rect_t crop_info;
  uint32_t buffer_cnt;
  int buf_idx;
  int8_t active;
  mct_imglib_swaf_config_t cur_af_cfg;
  img_trans_info_t camif_trans_info;
  boolean use_af_tuning_trans;
  img_trans_info_t af_tuning_trans_info;
  int frame_id;
  int32_t sync;
  int32_t processing;
  img_rect_t roi;
  int num_skip;
  img_rect_t cropped_window;
  boolean frame_crop;
  boolean buf_allocation_done;
  img_size_t frame_dim;
  stream_port_mapping_t stream[MAX_AFS_STREAMS];
  int stream_cnt;
  uint32_t event_identity;
  uint32_t stream_mask;
  uint32_t active_str_identity;
  int ref_count;
  uint32_t buf_divert_identity;
} afs_client_t;

/** afs_session_params_t
 *   @session_id: Session id for which parameters are stored
 *   @valid_params: Valid parameters Yes/No
 *   @msg_thread: message thread
 *
 *   Structure which holds session based parameters
 **/
typedef struct {
  uint32_t session_id;
  boolean valid_params;
  mod_imglib_msg_th_t msg_thread;
  struct {
  } param;
} afs_session_params_t;

/** module_afs_t
 *   @client_cnt: Variable to hold the number of afs
 *              clients
 *   @module_type: Hold last updated module type
 *   @mutex: client lock
 *   @cond: conditional variable for the client
 *   @lib_ref_count: reference count for afs library access
 *   @afs_client: list of afs clients
 *   @session_parms: List of session based parameters
 *   @parent_mod: mct parent module
 *
 *   afs module structure
 **/
typedef struct {
  int client_cnt;
  mct_module_type_t module_type;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int lib_ref_count;
  mct_list_t *afs_client;
  mct_list_t *session_parms;
  mct_module_t *parent_mod;
} module_afs_t;

/**
 * afs client APIs
 **/
int module_afs_get_stream_by_id(afs_client_t *p_client,
  uint32_t identity);

int module_afs_get_stream_by_type(afs_client_t *p_client,
  cam_stream_type_t stream_type);

int module_afs_client_create(mct_module_t *p_mct_mod, mct_port_t *p_port,
  uint32_t identity, mct_stream_info_t *stream_info);

int module_afs_client_start(afs_client_t *p_client);

int module_afs_client_stop(afs_client_t *p_client);

int module_afs_load();

void module_afs_unload();

void module_afs_client_destroy(afs_client_t *p_client);

int module_afs_client_map_buffers(afs_client_t *p_client, uint32_t event_identity);

int module_afs_client_handle_buffer(afs_client_t *p_client,
  uint32_t buf_idx, uint32_t frame_id, int32_t *p_frame_idx,
  isp_buf_divert_t *isp_buf, uint32_t processing_identity);

int module_afs_client_unmap_buffers(afs_client_t *p_client, uint32_t identity);

int module_afs_client_handle_ctrl_parm(afs_client_t *p_client,
  mct_event_control_parm_t *param);

int module_afs_client_set_scale_ratio(afs_client_t *p_client,
  mct_bus_msg_stream_crop_t *stream_crop, uint32_t event_idx);

int module_afs_handle_streamon(afs_client_t *p_client, uint32_t identity);

int module_afs_handle_streamoff(afs_client_t *p_client, uint32_t identity);

void module_afs_client_process(void *p_userdata, void *data);

void module_afs_client_update_cfg(afs_client_t *p_client, uint32_t event_identity);

#endif //__MODULE_afs_H__
