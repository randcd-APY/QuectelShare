/**********************************************************************
*  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include <cutils/properties.h>
#include <linux/media.h>
#include "mct_module.h"
#include "module_afs.h"
#include "mct_stream.h"
#include "modules.h"
#include "fd_chromatix.h"
#include <dlfcn.h>
#include <arm_neon.h>

#define PAAF_W_THRES 300
//#define TEST_AFS_IIR
//#define TEST_AFS_FIR
#define ROI_BOUNDARY_PIXELS 12
#define ROI_MIN_SIZE 180

#define NEON_OPTIMIZATION

/** afs_lib_info_t:
 *  @ptr: library handle
 *  @af_sw_stats_iir_float: function pointer for the AF stats
 *                        algorithm
 *  @af_sw_stats_fir_float: function pointer for the AF stats
 *                        algorithm
 **/
typedef struct {
  void *ptr;
  double (*af_sw_stats_iir_float) (uint8_t *img_orig,
    int N,
    int height,
    int width,
    int roi_h_offset,
    int rgn_width,
    int roi_v_offset,
    int rgn_height,
    float *coeffa,
    float *coeffb,
    int coeff_len,
    float FV_min
  );
  double (*af_sw_stats_iir_float_neon) (
    uint8_t *img_orig,
    int N,
    int height,
    int width,
    int roi_h_offset,
    int rgn_width,
    int roi_v_offset,
    int rgn_height,
    float32_t *coeffa,
    float32_t *coeffb,
    int coeff_len,
    float32_t FV_min
  );
  long (*FIR_frame) (uint8_t *img_orig,
    int height,
    int width,
    int roi_h_offset,
    int rgn_width,
    int roi_v_offset,
    int rgn_height,
    int *FIR,
    int coeff_len,
    double FV_min);
} afs_lib_info_t;

static afs_lib_info_t g_afs_lib;

/**
 * Function: module_afs_client_get_buf
 *
 * Description: get the buffer for AF sw stats
 *
 * Arguments:
 *   @p_client - afs client pointer
 *
 * Return values:
 *   buffer index
 *
 * Notes: none
 **/
static inline int module_afs_client_get_buf(afs_client_t *p_client)
{
  int32_t idx = p_client->buf_idx;
  p_client->buf_idx = (p_client->buf_idx + 1) % MAX_NUM_AFS_FRAMES;
  return idx;
}


/**
 * Function: module_afs_client_need_frame_crop
 *
 * Description: Determine if frame crop is needed.
 *
 * Arguments:
 *   @p_frame: afs frame
 *
 * Return values:
 *     bool
 *
 **/
boolean module_afs_client_need_frame_crop(img_frame_t *p_frame)
{
  if (IMG_WIDTH(p_frame) == 3840 || IMG_HEIGHT(p_frame) == 2160) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * Function: module_afs_client_destroy
 *
 * Description: This function is used to destroy the afs client
 *
 * Arguments:
 *   @p_client: afs client
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
void module_afs_client_destroy(afs_client_t *p_client)
{
  uint32_t i;
  img_frame_t *p_frame;
  int str_idx = 0;

  if (NULL == p_client) {
    return;
  }

  IDBG_MED("%s:%d] state %d", __func__, __LINE__, p_client->state);

  if (IMGLIB_STATE_STARTED == p_client->state) {
    module_afs_client_stop(p_client);
  }

  p_client->state = IMGLIB_STATE_IDLE;

  pthread_mutex_destroy(&p_client->mutex);
  pthread_mutex_destroy(&p_client->frame_algo_mutex);
  pthread_cond_destroy(&p_client->frame_algo_cond);

  for (str_idx = 0; str_idx < p_client->stream_cnt; str_idx++) {
    for (i = 0; i < MAX_NUM_AFS_FRAMES; i++) {
      p_frame = &p_client->stream[str_idx].p_frame[i];
      if (IMG_ADDR(p_frame)) {
        free(IMG_ADDR(p_frame));
        IMG_ADDR(p_frame) = NULL;
      }
    }
  }

  free(p_client);
  p_client = NULL;
  IDBG_MED("%s:%d] X", __func__, __LINE__);
}

/**
 * Function: module_afs_client_create
 *
 * Description: This function is used to create the afs client
 *
 * Arguments:
 *   @p_mct_mod: mct module pointer
 *   @p_port: mct port pointer
 *   @identity: identity of the stream
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_create(mct_module_t *p_mct_mod, mct_port_t *p_port,
  uint32_t identity, mct_stream_info_t *stream_info)
{
  afs_client_t *p_client = NULL;
  module_afs_t *p_mod = (module_afs_t *)p_mct_mod->module_private;

  p_client = (afs_client_t *)malloc(sizeof(afs_client_t));
  if (NULL == p_client) {
    IDBG_ERROR("%s:%d] client alloc failed", __func__, __LINE__);
    return IMG_ERR_NO_MEMORY;
  }

  /* initialize the variables */
  memset(p_client, 0x0, sizeof(afs_client_t));
  pthread_mutex_init(&p_client->mutex, NULL);
  pthread_mutex_init(&p_client->frame_algo_mutex, NULL);
  pthread_cond_init(&p_client->frame_algo_cond, NULL);
  p_client->state = IMGLIB_STATE_IDLE;
  p_client->stream[0].stream_info = stream_info;

  /* initialize if required */
  p_client->state = IMGLIB_STATE_INIT;

  p_client->stream[0].p_sinkport = p_port;
  p_client->stream[0].identity = identity;
  p_port->port_private = p_client;
  p_client->buf_idx = 0;
  p_client->p_mod = p_mod;
  p_client->active = TRUE;
  p_client->stream_cnt = 0;

  p_client->processing = FALSE;
  p_client->sync = FALSE;
  p_client->frame_crop = TRUE;
  p_client->buf_allocation_done = FALSE;
  p_client->frame_dim.width = 0;
  p_client->frame_dim.height = 0;
  p_client->cur_af_cfg.enable = 0;
  p_client->num_skip = 1;

  p_client->frame_skip_cnt = 0;
  p_client->use_af_tuning_trans = FALSE;
  p_client->af_tuning_trans_info.h_scale =
    p_client->af_tuning_trans_info.v_scale = 0.16f;

  IDBG_MED("%s:%d] port %p client %p X", __func__, __LINE__, p_port, p_client);
  return IMG_SUCCESS;
}

/**
 * Function: afs_map_buffer
 *
 * Description: This method is used for updating the AFS
 *      buffer structure from MCT structure with AFS buffers
 *
 * Input parameters:
 *   @data - Index of the stream in the clients stream mapping
 *   @user_data - img buffer structure
 *
 * Return values:
 *     true/false
 *
 * Notes: none
 **/
boolean afs_map_buffer(void *data, void *user_data)
{
  mct_stream_map_buf_t *p_buf = (mct_stream_map_buf_t *)data;
  afs_client_t *p_client = (afs_client_t *)user_data;
  uint32_t idx = 0;
  int str_idx = 0;

  IDBG_MED("%s:%d] p_buf %p p_buf_info %p", __func__, __LINE__, p_buf,
    p_client);
  if (!p_buf || !p_client) {
    IDBG_ERROR("%s:%d failed", __func__, __LINE__);
    return TRUE;
  }

  /* Check the buffer count */
  if (p_client->buffer_cnt >= MAX_NUM_FRAMES)
    return TRUE;

  str_idx = module_afs_get_stream_by_id(p_client, p_client->event_identity);
  if (str_idx < 0) {
    IDBG_ERROR("%s:%d Invalid stream index", __func__, __LINE__);
    return FALSE;
  }

  idx = p_client->buffer_cnt;
  p_client->stream[str_idx].p_map_buf[idx] = *p_buf;
  IDBG_MED("%s:%d] buffer cnt %d idx %d addr %p", __func__, __LINE__,
    p_client->buffer_cnt, p_buf->buf_index,
    p_buf->buf_planes[0].buf);
  p_client->buffer_cnt++;
  return TRUE;
}

#if defined(TEST_AFS_IIR) || defined(TEST_AFS_FIR)
/**
 * Function: afs_set_test_param
 *
 * Description: This method is used for assigning test params
 *
 * Input parameters:
 *   @data - MCT stream buffer mapping
 *   @user_data - img buffer structure
 *
 * Return values:
 *     true/false
 *
 * Notes: none
 **/
void afs_set_test_param(afs_client_t *p_client, int32_t width, int32_t height)
{

#ifdef TEST_AFS_IIR
  p_client->cur_af_cfg.filter_type = 1;
  p_client->roi.size.width = width/6;
  p_client->roi.size.height = height/6;
  p_client->roi.pos.x = (double)(2.5/6.0) *
    (double)p_client->cur_af_cfg.roi.width;
  p_client->roi.pos.y = (double)(2.5/6.0) *
    (double)p_client->cur_af_cfg.roi.height;
#elif defined(TEST_AFS_FIR)
  p_client->cur_af_cfg.filter_type = 1;
  p_client->roi.size.width = width/6;
  p_client->roi.size.height = height/6;
  p_client->roi.pos.x = (double)(2.5/6.0) *
    (double)p_client->cur_af_cfg.roi.width;
  p_client->roi.pos.y = (double)(2.5/6.0) *
    (double)p_client->cur_af_cfg.roi.height;
#endif

#ifdef TEST_AFS_IIR
  double lcoeffa[] = {1.0, -1.758880, 0.930481, 1.0, -1.817633, 0.940604};
  double lcoeffb[] = {0.034788, 0.000000, -0.034788, 0.059808, 0.000000, -0.059808};
  uint32_t i;
  for (i = 0; i < 6; i++) {
    p_client->cur_af_cfg.coeffa[i] = lcoeffa[i];
    p_client->cur_af_cfg.coeffb[i] = lcoeffb[i];
  }
  p_client->cur_af_cfg.coeff_len = 6;
#endif

#ifdef TEST_AFS_FIR
  int coeff_fir[] = {-4, -4, 1, 2, 3, 4, 3, 2, 1, -4, -4};
  uint32_t k;
  for (k = 0; k < sizeof(coeff_fir)/sizeof(coeff_fir[0]); k++) {
    p_client->cur_af_cfg.coeff_fir[k] = coeff_fir[k];
  }
  p_client->cur_af_cfg.coeff_len = sizeof(coeff_fir)/sizeof(coeff_fir[0]);
#endif
}
#endif

/**
 * Function: module_afs_client_map_buffers
 *
 * Description: This function is used to map the buffers when
 * the stream is started
 *
 * Arguments:
 *   @p_client: afs client
 *   @event_identity: Identity of the current stream
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_map_buffers(afs_client_t *p_client,
  uint32_t event_identity)
{
  mct_stream_info_t *stream_info;
  uint32_t i = 0;
  img_frame_t *p_frame;
  stream_port_mapping_t *p_stream = NULL;
  int str_idx = 0;

  if (!(p_client))
    return IMG_ERR_INVALID_INPUT;

  if (p_client->state == IMGLIB_STATE_INIT ||
      p_client->state == IMGLIB_STATE_IDLE) {
     IDBG_ERROR("%s:%d] client not started", __func__, __LINE__);
     return IMG_SUCCESS;
  }

  /* More than one stream mapped to port,
     get stream index based on stream identity */
  str_idx = module_afs_get_stream_by_id(p_client, event_identity);
  if (str_idx < 0) {
    IDBG_ERROR("%s:%d Invalid stream index", __func__, __LINE__);
    return IMG_ERR_OUT_OF_BOUNDS;
  }

  p_stream = &p_client->stream[str_idx];
  if (!p_stream) {
    IDBG_ERROR("%s:%d] Can't find stream mapped to client",
      __func__, __LINE__);
    return IMG_ERR_INVALID_INPUT;
  }

  stream_info = p_stream->stream_info;
  p_client->buffer_cnt = 0;
  p_client->event_identity = event_identity;

  mct_list_traverse(stream_info->img_buffer_list, afs_map_buffer,
    p_client);

#ifdef SWITCH_BETWEEN_ASYNC_SYNC
  /* enable if async mode is not enabled by default
     and frame crop is needed */
  if (module_afs_client_need_frame_crop(&p_client->p_frame[0])) {
    p_client->sync = FALSE;
    p_client->frame_crop = TRUE;
  } else {
    p_client->sync = TRUE;
    p_client->frame_crop = FALSE;
  }
#endif

  if ((p_client->stream[str_idx].in_dim.width <= 0) ||
    (p_client->stream[str_idx].in_dim.height <= 0)) {
    IDBG_ERROR("%s:%d] Invalid ISP output dimension", __func__, __LINE__);
    return IMG_ERR_INVALID_INPUT;
  }

  for (i = 0; i < MAX_NUM_AFS_FRAMES; i++) {
    p_frame = &p_client->stream[str_idx].p_frame[i];
    p_frame->frame_cnt = 1;
    p_frame->idx = i;
    p_frame->info.width = p_client->stream[str_idx].in_dim.width;
    p_frame->info.height = p_client->stream[str_idx].in_dim.height;
    p_frame->frame[0].plane_cnt = 1;
    p_frame->frame[0].plane[0].width =
      p_client->stream[str_idx].in_dim.width;
    p_frame->frame[0].plane[0].height =
      p_client->stream[str_idx].in_dim.height;
    p_frame->frame[0].plane[0].plane_type = PLANE_Y;
    p_frame->frame[0].plane[0].length =
      p_client->stream[str_idx].in_dim.width *
      p_client->stream[str_idx].in_dim.height;

    /* Async mode, need to alloc memory for the memcpy of frame */
    if (!p_client->sync) {
      p_frame->frame[0].plane[0].addr =
        malloc((size_t)p_frame->frame[0].plane[0].length);
      if (NULL == IMG_ADDR(p_frame)) {
        IDBG_ERROR("%s:%d] buffer create failed", __func__, __LINE__);
        return IMG_ERR_NO_MEMORY;
      }
      p_client->buf_allocation_done = TRUE;

      IDBG_MED("%s:%d] map buf %p, dim %d %d", __func__, __LINE__,
        p_frame->frame[0].plane[0].addr, p_frame->frame[0].plane[0].width,
        p_frame->frame[0].plane[0].height);
    }
  }
  IDBG_MED("%s:%d] dim %dx%d", __func__, __LINE__,
    p_client->stream[str_idx].in_dim.width,
    p_client->stream[str_idx].in_dim.height);
  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_unmap_buffers
 *
 * Description: This function is used to unmap the buffers when
 * the stream is started
 *
 * Arguments:
 *   @p_client: afs client
 *   @identity: on which identity to unmap the buffers
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_unmap_buffers(afs_client_t *p_client, uint32_t identity)
{
  uint32_t i = 0;
  img_frame_t *p_frame;
  int str_idx = 0;

  str_idx = module_afs_get_stream_by_id(p_client, identity);
  if (str_idx < 0) {
    IDBG_MED("%s:%d] Invalid identity 0x%x", __func__, __LINE__, identity);
    return IMG_ERR_INVALID_INPUT;
  }

  for (i = 0; i < MAX_NUM_AFS_FRAMES; i++) {
    p_frame = &p_client->stream[str_idx].p_frame[i];
    if (!p_client->sync) {
      if (IMG_ADDR(p_frame)) {
        free(IMG_ADDR(p_frame));
        IMG_ADDR(p_frame) = NULL;
      }
    }
    IMG_ADDR(p_frame) = NULL;
  }

  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_set_scale_ratio
 *
 * Description: Set face detection scale ratio.
 *
 * Arguments:
 *   @p_client: afs client
 *   @stream_crop: Stream crop event structure
 *
 * Return values:
 *     imaging error values
 *
 * Notes: Calculate just scale ratio which we assume that will be applied
 *  to get preview resolution.
 **/
int module_afs_client_set_scale_ratio(afs_client_t *p_client,
  mct_bus_msg_stream_crop_t *stream_crop, uint32_t event_idx)
{
  int32_t str_idx = 0;
  uint32_t in_width, in_height;

  if (!(p_client) || !stream_crop) {
    IDBG_MED("%s:%d] Error parameters are null", __func__, __LINE__);
    return IMG_ERR_INVALID_INPUT;
  }

  str_idx  = module_afs_get_stream_by_id(p_client, event_idx);
  if (str_idx < 0) {
    IDBG_ERROR("%s %d]: Invalid stream index", __func__, __LINE__);
    return IMG_ERR_INVALID_INPUT;
  }

  /* ISP output dim associated with stream */
  in_width = p_client->stream[str_idx].in_dim.width;
  in_height = p_client->stream[str_idx].in_dim.height;

  if (in_width <=0 || in_height <=0) {
    IDBG_MED("%s:%d] Error %p %d %d",
      __func__, __LINE__,
      stream_crop,
      in_width,
      in_height);
    return IMG_SUCCESS;
  }

  if (!stream_crop->crop_out_x || !stream_crop->crop_out_y) {
    p_client->crop_info.pos.x = 0;
    p_client->crop_info.pos.y = 0;
    p_client->crop_info.size.width = in_width;
    p_client->crop_info.size.height = in_height;
  } else {
    p_client->crop_info.pos.x = (int32_t)stream_crop->x;
    p_client->crop_info.pos.y = (int32_t)stream_crop->y;
    p_client->crop_info.size.width = (int32_t)stream_crop->crop_out_x;
    p_client->crop_info.size.height= (int32_t)stream_crop->crop_out_y;
  }
  IDBG_MED("%s:%d] [AFS_CAMIF] Crop(%d %d %d %d)",
    __func__, __LINE__,
    stream_crop->x,
    stream_crop->y,
    stream_crop->crop_out_x,
    stream_crop->crop_out_y);

  if ((stream_crop->width_map > 0) &&
    (stream_crop->height_map > 0)) {
    p_client->camif_trans_info.h_scale =
      (float)in_width /
      (float)stream_crop->width_map;
    p_client->camif_trans_info.v_scale =
      (float)in_height/
      (float)stream_crop->height_map;
    p_client->use_af_tuning_trans = FALSE;
  } else {
    p_client->use_af_tuning_trans = TRUE;
  }
  p_client->camif_trans_info.h_offset =
    stream_crop->x_map;
  p_client->camif_trans_info.v_offset =
    stream_crop->y_map;

  IDBG_MED("%s:%d] [AFS_CAMIF] Map(%d %d %d %d)",
    __func__, __LINE__,
    stream_crop->width_map,
    stream_crop->height_map,
    stream_crop->x_map,
    stream_crop->y_map);
  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_start
 *
 * Description: This function is used to start the afs
 *              client
 *
 * Arguments:
 *   @p_client: afs client
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_start(afs_client_t *p_client)
{
  IDBG_MED("%s:%d] E", __func__, __LINE__);

  if (p_client->state != IMGLIB_STATE_INIT) {
    IDBG_ERROR("%s:%d] invalid state %d",
      __func__, __LINE__, p_client->state);
    return IMG_ERR_INVALID_OPERATION;
  }

  p_client->state = IMGLIB_STATE_STARTED;
  IDBG_MED("%s:%d] X", __func__, __LINE__);

  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_stop
 *
 * Description: This function is used to stop the afs
 *              client
 *
 * Arguments:
 *   @p_client: afs client
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_stop(afs_client_t *p_client)
{
  p_client->state = IMGLIB_STATE_INIT;
  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_check_boundary
 *
 * Description: validates the boundary of the AFS cordiantes
 *
 * Arguments:
 *   @p_rect - AFS input cordinates
 *   @width - Preview width
 *   @height - Preview height
 *
 * Return values:
 *   TRUE/FALSE
 *
 * Notes: none
 **/
static inline boolean module_afs_client_check_boundary(
  img_rect_t *p_rect, int width, int height)
{
  if ((p_rect->pos.x < 0) || (p_rect->pos.y < 0) ||
    (p_rect->size.width <= 0) || (p_rect->size.height <= 0) ||
    ((p_rect->pos.x + p_rect->size.width) > width) ||
    ((p_rect->pos.y + p_rect->size.height) > height)) {
    IDBG_LOW("%s:%d] x %d y %d, roi_w %d, roi_h %d, w %d, h %d",
      __func__, __LINE__, p_rect->pos.x, p_rect->pos.y,
      p_rect->size.width, p_rect->size.height, width, height);
    /* out of boundary */
    IMG_PRINT_RECT(p_rect);
    return FALSE;
  }
  IMG_PRINT_RECT(p_rect);
  return TRUE;
}

/**
 * Function: module_afs_client_default_roi_to_center
 *
 * Description: in error case, default AFS ROI to center of
 *              output frame
 *
 * Arguments:
 *   @roi - AFS ROI coords
 *   @width - Preview width
 *   @height - Preview height
 *   @h_scale - af tuning h_clip ratio
 *   @v_scale - af tuning v_clip ratio
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
static inline void module_afs_client_default_roi_to_center(
  img_rect_t *roi, int width, int height, float h_scale, float v_scale)
{
  roi->size.width = (int)((float)width * h_scale);
  roi->size.height = (int)((float)width * v_scale);
  roi->pos.x = (width - roi->size.width) / 2;
  roi->pos.y = (height - roi->size.height) / 2;

  IDBG_MED("%s:%d] [AS_SW_DBG] center roi (%d %d %d %d), "
    "h_scale %f, v_scale %f", __func__, __LINE__,
    roi->pos.x, roi->pos.y, roi->size.width,
    roi->size.height, h_scale, v_scale);
}

/**
 * Function: module_afs_client_grow_roi
 *
 * Description: increase the size of the ROI
 *
 * Arguments:
 *   @roi - AFS ROI coords
 *   @length - New ROI's length and width
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
static inline void module_afs_client_grow_roi(
  img_rect_t *roi, int length)
{
    roi->pos.x = roi->pos.x - ((length - roi->size.width) / 2);
    roi->pos.y = roi->pos.y - ((length - roi->size.height) / 2);
    roi->size.width = length;
    roi->size.height = length;
}

/**
 * Function: module_afs_client_fix_out_of_boundary
 *
 * Description: If the ROI is partly out of boundary,
 *              translate x and y coords to within the preview
 *              frame. Will still need to do a boundary check
 *              after this tranlsation and reject the ROI if it
 *              cannot be contained in the preview frame.
 *
 * Arguments:
 *   @p_rect - AFS input cordinates
 *   @width - Preview width
 *   @height - Preview height
 *
 * Return values:
 *   TRUE/FALSE
 *
 * Notes: none
 **/
static inline boolean module_afs_client_fix_out_of_boundary(
  img_rect_t *p_rect, int width, int height)
{
  if ((p_rect->pos.x > width) || (p_rect->pos.y > height) ||
    ((p_rect->pos.x + p_rect->size.width) < 0) ||
    ((p_rect->pos.y + p_rect->size.height) < 0)) {
    /* ROI window completely out of boundary */
    return FALSE;
  }

  if (p_rect->pos.x < 0) {
    p_rect->pos.x = 0;
  }

  if (p_rect->pos.y < 0) {
    p_rect->pos.y = 0;
  }

  if ((p_rect->pos.x + p_rect->size.width) >= width) {
    p_rect->pos.x = width - p_rect->size.width - 1;
  }

  if ((p_rect->pos.y + p_rect->size.height) >= height) {
    p_rect->pos.y = height - p_rect->size.height - 1;
  }

  return TRUE;
}

/**
 * Function: module_afs_client_get_skip
 *
 * Description: This function is to get the skip count
 *
 * Arguments:
 *   @width: image width
 *   @height: image height
 *   @coeff_len: determines filter order
 *
 * Return values:
 *     skip count
 *
 * Notes: none
 **/
int32_t module_afs_client_get_skip(uint32_t width, uint32_t height,
  uint32_t coeff_len)
{
  /* if coeff_len is 3, lower order filter is used
     and pixel skip is not needed*/
  if (coeff_len == 3) {
    return 1;
  }

  float threshold = 304.0;
  int32_t num_skip = (int32_t)(MAX(ceil((float)width / threshold),
    ceil((float)height / threshold)));

  return num_skip;
}

/**
 * Function: module_afs_client_set_frame_crop
 *
 * Description: Set the roi for frame crop
 *
 * Arguments:
 *   @cropped_window - to store the cropped coords
 *   @p_orig_roi - AFS roi cordinates
 *   @width - Preview width
 *   @height - Preview height
 *   @pixel_skip - number of pixels to skip. 1 = no skip
 *
 * Return values:
 *   TRUE/FALSE
 *
 * Notes: none
 **/
static inline boolean module_afs_client_set_frame_crop(
  img_rect_t *cropped_window, img_rect_t *p_orig_roi,
  int width, int height, int pixel_skip)
{
  if (p_orig_roi->size.width == 0 || p_orig_roi->size.height == 0) {
    return FALSE;
  }

  cropped_window->pos.x = p_orig_roi->pos.x - ROI_BOUNDARY_PIXELS;
  cropped_window->pos.y = p_orig_roi->pos.y - ROI_BOUNDARY_PIXELS;
  cropped_window->size.width =
    p_orig_roi->size.width + (2 * ROI_BOUNDARY_PIXELS);
  cropped_window->size.height =
    p_orig_roi->size.height + (2 * ROI_BOUNDARY_PIXELS);

  p_orig_roi->pos.x = ROI_BOUNDARY_PIXELS / pixel_skip;
  p_orig_roi->pos.y = ROI_BOUNDARY_PIXELS / pixel_skip;
  p_orig_roi->size.width = p_orig_roi->size.width / pixel_skip;
  p_orig_roi->size.height = p_orig_roi->size.height / pixel_skip;

  if (!module_afs_client_fix_out_of_boundary(cropped_window, width, height)) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Function: afs_client_update_cfg
 *
 * Description: This function is to get the skip count
 *
 * Arguments:
 *   @p_client: paaf client
 *   @event_identity : Event Idenitity
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_afs_client_update_cfg(afs_client_t *p_client,
  uint32_t event_identity)
{
  int32_t width, height;
  int str_idx = 0;
#ifdef SWITCH_BETWEEN_ASYNC_SYNC
  int i;
  img_frame_t *p_frame;
#endif

  mct_imglib_swaf_config_t *cfg = &p_client->cur_af_cfg;
  img_trans_info_t *camif_trans = &p_client->camif_trans_info;

  if (!p_client->cur_af_cfg.enable)
    return;

  /* More than one stream mapped to port,
    get stream index based on stream identity */
  str_idx = module_afs_get_stream_by_id(p_client, event_identity);
  if (str_idx < 0) {
    IDBG_ERROR("%s:%d Invalid stream index", __func__, __LINE__);
    return;
  }

  /* ISP output dim associated with stream */
  width = p_client->stream[str_idx].in_dim.width;
  height = p_client->stream[str_idx].in_dim.height;

  IDBG_HIGH("%s:%d] [AS_SW_DBG] %dx%d (%d %d %d %d)",
    __func__, __LINE__, width, height,
    cfg->roi.left, cfg->roi.top, cfg->roi.width, cfg->roi.height);

  if (!p_client->use_af_tuning_trans) {
    /* Calculate AFS client ROI position based on vfe map scale */
    p_client->roi.pos.x = (int32_t)IMG_TRANSLATE(cfg->roi.left,
      camif_trans->h_scale, camif_trans->h_offset);
    p_client->roi.pos.y = (int32_t)IMG_TRANSLATE(cfg->roi.top,
      camif_trans->v_scale, camif_trans->v_offset);

    IDBG_MED("%s:%d] [AS_SW_DBG] roi_x %d, h_scale %f, h_off %d. "
      "roi_y %d, v_scale %f, v_off %d",
      __func__, __LINE__,
      p_client->roi.pos.x, camif_trans->h_scale, camif_trans->h_offset,
      p_client->roi.pos.y, camif_trans->v_scale, camif_trans->v_offset);

    /* Calculate AFS client ROI size */
    p_client->roi.size.width = (int32_t)IMG_TRANSLATE2(cfg->roi.width,
      camif_trans->h_scale, 0);
    p_client->roi.size.height = (int32_t)IMG_TRANSLATE2(cfg->roi.height,
      camif_trans->v_scale, 0);

    IDBG_MED("%s:%d] [AS_SW_DBG] roi_w %d roi_h %d", __func__, __LINE__,
      p_client->roi.size.width, p_client->roi.size.height);
  } else {
    /* vfe map is invalid, default to center ROI based on af_tuning ratio */
    IDBG_ERROR("%s:%d] [AS_SW_DBG] Invalid VFE w and h map. "
      "Set to center ROI", __func__, __LINE__);
    module_afs_client_default_roi_to_center(&p_client->roi,
      width, height,
      p_client->af_tuning_trans_info.h_scale,
      p_client->af_tuning_trans_info.v_scale);
  }

  /* Grow ROI window if ROI is too small */
  if (p_client->roi.size.width < ROI_MIN_SIZE ||
    p_client->roi.size.height < ROI_MIN_SIZE) {
    module_afs_client_grow_roi(&p_client->roi, ROI_MIN_SIZE);
  }

  if (!module_afs_client_fix_out_of_boundary(&p_client->roi, width, height)) {
    IDBG_ERROR("%s:%d] [AS_SW_DBG] ROI window completely out of boundary. "
      "Set to center ROI", __func__, __LINE__);
    module_afs_client_default_roi_to_center(&p_client->roi,
      width, height,
      p_client->af_tuning_trans_info.h_scale,
      p_client->af_tuning_trans_info.v_scale);
  }

  /* If ROI is out of boundary, default to center ROI */
  if (!module_afs_client_check_boundary(&p_client->roi, width, height)) {
    IDBG_ERROR("%s:%d] [AS_SW_DBG] ROI out of boundary. Set to center ROI",
      __func__, __LINE__);
    module_afs_client_default_roi_to_center(&p_client->roi,
      width, height,
      p_client->af_tuning_trans_info.h_scale,
      p_client->af_tuning_trans_info.v_scale);
  }

#if defined(TEST_AFS_IIR) || defined(TEST_AFS_FIR)
  /* Hard Code params for Testing */
  afs_set_test_param(p_client, width, height);
#endif

  /* Ensure Even width and Height */
  p_client->roi.size.width &= (~3);
  p_client->roi.size.height &= (~3);

  /* get num of pixels skip for filtering */
  p_client->num_skip = module_afs_client_get_skip(
    (uint32_t)p_client->roi.size.width,
    (uint32_t)p_client->roi.size.height,
    p_client->cur_af_cfg.coeff_len);
  if (!p_client->num_skip)
    p_client->num_skip = 1;

#ifdef SWITCH_BETWEEN_ASYNC_SYNC
  /* enable if async mode is not enabled by default
     and frame crop is needed */
  if (module_afs_client_need_frame_crop(&p_client->p_frame[0])) {
    p_client->sync = FALSE;
    p_client->frame_crop = TRUE;
  } else {
    p_client->sync = TRUE;
    p_client->frame_crop = FALSE;
  }

  /* malloc of bufs required in async mode since not done
     in client_create*/
  if (!p_client->sync && !p_client->buf_allocation_done) {
    for (i = 0; i < MAX_NUM_AFS_FRAMES; i++) {
      p_frame = &p_client->stream[str_idx].p_frame[i];
      p_frame->frame[0].plane[0].addr =
        malloc(p_frame->frame[0].plane[0].length);
      if (NULL == IMG_ADDR(p_frame)) {
        IDBG_ERROR("%s:%d] buffer create failed", __func__, __LINE__);
        return;
      }
    }
    p_client->buf_allocation_done = TRUE;
  }
#endif

  if (p_client->frame_crop) {
    if (!module_afs_client_set_frame_crop(&p_client->cropped_window,
      &p_client->roi, width, height, p_client->num_skip)) {
      IDBG_ERROR("%s:%d] [AS_SW_DBG] frame crop out of boundary, "
        "set to center ROI", __func__, __LINE__);
      module_afs_client_default_roi_to_center(&p_client->roi,
        width, height, p_client->af_tuning_trans_info.h_scale,
        p_client->af_tuning_trans_info.v_scale);
      module_afs_client_set_frame_crop(&p_client->cropped_window,
        &p_client->roi, width, height, p_client->num_skip);
    }
  }

  IDBG_HIGH("%s:%d] [AS_SW_DBG] ROI(%d %d %d %d), cropped frame"
    "(%d %d %d %d), coe_len %d skip %d",
    __func__, __LINE__,
    p_client->roi.pos.x,
    p_client->roi.pos.y,
    p_client->roi.size.width,
    p_client->roi.size.height,
    p_client->cropped_window.pos.x,
    p_client->cropped_window.pos.y,
    p_client->cropped_window.size.width,
    p_client->cropped_window.size.height,
    p_client->cur_af_cfg.coeff_len,
    p_client->num_skip);
}

/**
 * Function: module_afs_client_process
 *
 * Description: This function is for processing the algorithm
 *
 * Arguments:
 *   @p_userdata: client
 *   @data: function parameter
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
void module_afs_client_process(void *p_userdata, void *data)
{
  afs_client_t *p_client = (afs_client_t *)p_userdata;
  img_frame_t *p_frame = (img_frame_t *)data;
  mct_imglib_swaf_config_t af_cfg;
  double fV = 0.0;
  uint32_t coeff_len;
  uint32_t i;
  float l_FV_min;
  int str_idx;
  mct_event_t mct_event;
  struct timeval start_time, end_time;

  IDBG_LOW("%s:%d] E", __func__, __LINE__);
  if (!p_userdata || !p_frame) {
    IDBG_ERROR("%s:%d] invalid input", __func__, __LINE__);
    return;
  }

  if (!g_afs_lib.ptr) {
    IDBG_ERROR("%s:%d] invalid operation", __func__, __LINE__);
    return;
  }

  pthread_mutex_lock(&p_client->mutex);
  if (IMGLIB_STATE_PROCESSING != p_client->state) {
    IDBG_MED("%s:%d] not in proper state", __func__, __LINE__);
    pthread_mutex_unlock(&p_client->mutex);
    return;
  }
  pthread_mutex_unlock(&p_client->mutex);

  af_cfg = p_client->cur_af_cfg;

  if (!af_cfg.enable) {
    IDBG_MED("%s:%d] not enabled", __func__, __LINE__);
    return;
  }

  /* use sink port from current stream to send event*/
  str_idx = module_afs_get_stream_by_id(p_client, p_client->buf_divert_identity);
  if (str_idx < 0) {
    IDBG_ERROR("%s:%d] Invalid Stream index", __func__, __LINE__);
    return;
  }

  if (p_client->stream[str_idx].stream_off == TRUE) {
    IDBG_ERROR("%s:%d streamoff already unmap buffers, return", __func__, __LINE__);
    return;
  }

#ifdef AFS_GET_AF_FILTER
  af_sw_filter_type afs_filter_type;
  memset(&mct_event, 0x0, sizeof(mct_event_t));
  mct_event.u.module_event.type = MCT_EVENT_MODULE_GET_AF_SW_STATS_FILTER_TYPE;
  mct_event.u.module_event.module_event_data = (void *)&afs_filter_type;
  mct_event.type = MCT_EVENT_MODULE_EVENT;
  mct_event.identity = p_client->buf_divert_identity;
  mct_event.direction = MCT_EVENT_UPSTREAM;
  mct_port_send_event_to_peer(p_client->stream[str_idx].p_sinkport, &mct_event);
  IDBG_MED("%s:%d] filter type : %d", __func__, __LINE__,
    afs_filter_type);
#else
  af_sw_filter_type afs_filter_type =
    p_client->cur_af_cfg.filter_type;
  IDBG_MED("%s:%d] filter_type : %d", __func__, __LINE__,
    afs_filter_type);
#endif

  if (afs_filter_type != AFS_OFF) {

#ifdef TEST_AFS_FRAME
    IMG_DUMP_TO_FILE("/data/misc/camera/paaf.yuv", IMG_ADDR(p_frame),
      IMG_Y_LEN(p_frame));
#endif

#ifdef TEST_AFS_CROP_FRAME
    IMG_DUMP_TO_FILE("/data/misc/camera/paaf_crop.yuv", IMG_ADDR(p_frame),
      (p_client->cropped_window.size.height / p_client->num_skip) *
      (p_client->cropped_window.size.width / p_client->num_skip));
#endif

    if (p_client->frame_crop) {
      p_client->frame_dim.width = (p_client->cropped_window.size.width /
        p_client->num_skip);
      p_client->frame_dim.height = (p_client->cropped_window.size.height /
        p_client->num_skip);
    } else {
      p_client->frame_dim.width = (int)IMG_WIDTH(p_frame);
      p_client->frame_dim.height = (int)IMG_HEIGHT(p_frame);
    }

    if (p_client->frame_dim.height == 0 || p_client->frame_dim.height == 0) {
      IDBG_ERROR("%s:%d] AFS frame dimensions not valid", __func__, __LINE__);
      return;
    }

    pthread_mutex_lock(&p_client->frame_algo_mutex);
    if (afs_filter_type == AFS_ON_FIR &&
      p_client->stream[str_idx].stream_off == FALSE) {
      p_client->processing = TRUE;

      IMG_DBG_TIMER_START(start_time);
      fV = g_afs_lib.FIR_frame(IMG_ADDR(p_frame),
        p_client->frame_dim.height,
        p_client->frame_dim.width,
        p_client->roi.pos.x,
        p_client->roi.size.width,
        p_client->roi.pos.y,
        p_client->roi.size.height,
        af_cfg.coeff_fir,
        (int)af_cfg.coeff_len,
        af_cfg.FV_min);
      IMG_DBG_TIMER_END(start_time, end_time, "AFS", IMG_TIMER_MODE_MS);

    } else if (afs_filter_type == AFS_ON_IIR &&
      p_client->stream[str_idx].stream_off == FALSE) {
      p_client->processing = TRUE;

      coeff_len = sizeof(af_cfg.coeffa)/sizeof(af_cfg.coeffa[0]);

      float l_coeffa[coeff_len];
      float l_coeffb[coeff_len];
      for (i = 0; i < coeff_len; i++) {
        l_coeffa[i] = (float)af_cfg.coeffa[i];
        l_coeffb[i] = (float)af_cfg.coeffb[i];
      }
      l_FV_min = (float)af_cfg.FV_min;

      IMG_DBG_TIMER_START(start_time);
#ifdef NEON_OPTIMIZATION
      fV = g_afs_lib.af_sw_stats_iir_float_neon(IMG_ADDR(p_frame),
        1,
        p_client->frame_dim.height,
        p_client->frame_dim.width,
        p_client->roi.pos.x,
        p_client->roi.size.width,
        p_client->roi.pos.y,
        p_client->roi.size.height,
        l_coeffa,
        l_coeffb,
        (int)af_cfg.coeff_len,
        l_FV_min);
#else
      fV = g_afs_lib.af_sw_stats_iir_float(IMG_ADDR(p_frame),
        p_client->num_skip,
        p_client->frame_dim.height,
        p_client->frame_dim.width,
        p_client->roi.pos.x,
        p_client->roi.size.width,
        p_client->roi.pos.y,
        p_client->roi.size.height,
        l_coeffa,
        l_coeffb,
        (int)af_cfg.coeff_len,
        l_FV_min);
#endif
      IMG_DBG_TIMER_END(start_time, end_time, "AFS", IMG_TIMER_MODE_MS);
    }
    p_client->processing = FALSE;
    pthread_cond_signal(&p_client->frame_algo_cond);
    pthread_mutex_unlock(&p_client->frame_algo_mutex);

    /* create MCT event and send */
    if (fV > 0) {
      mct_imglib_af_output_t af_out;
      memset(&mct_event, 0x0, sizeof(mct_event_t));
      memset(&af_out, 0x0, sizeof(mct_imglib_af_output_t));
      af_out.frame_id = p_client->frame_id;
      af_out.fV = fV;
      IDBG_MED("%s:%d] AFS_FV frameID:%d fv:%f filter %d", __func__, __LINE__,
        af_out.frame_id, af_out.fV, afs_filter_type);
      mct_event.u.module_event.type = MCT_EVENT_MODULE_IMGLIB_AF_OUTPUT;
      mct_event.u.module_event.module_event_data = (void *)&af_out;
      mct_event.type = MCT_EVENT_MODULE_EVENT;
      mct_event.identity = p_client->buf_divert_identity;
      mct_event.direction = MCT_EVENT_UPSTREAM;
      mct_port_send_event_to_peer(p_client->stream[str_idx].p_sinkport,
       &mct_event);
    }

  } else {
    p_client->processing = FALSE;
  }
  IDBG_LOW("%s:%d] X", __func__, __LINE__);
  return;
}


/**
 * Function: module_afs_client_crop_frame
 *
 * Description: crop frame based on the crop params
 *
 * Arguments:
 *   @out: destination buf
 *   @in: source buf
 *   @in_width: width of source frame
 *   @in_height: height of source frame
 *   @crop: amount to crop
 *   @pixel_skip: number of pixels to skip. 1 = no skip
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_crop_frame(uint8_t *out, uint8_t *in, int in_width,
  int in_height, img_rect_t crop, int pixel_skip)
{
  int i, j;
  uint8_t *inputAddr = in + (crop.pos.y * in_width) + crop.pos.x;
  uint8_t *outputAddr = out;
  uint32_t size = 0;

  if ((crop.size.height > in_height) || (crop.size.width > in_width)) {
    return IMG_ERR_INVALID_INPUT;
  }

  if (pixel_skip < 2) {
    /* no pixel skip, memcpy row-by-row to output buf*/
    for (i = 0; i < crop.size.height; i++) {
      memcpy(outputAddr, inputAddr, (size_t)crop.size.width);
      size += (uint32_t)crop.size.width;
      inputAddr += in_width;
      outputAddr += crop.size.width;
    }
  } else {
    /* skip alternate pixels */
    for(i = 0; i < (crop.size.height / pixel_skip); i++) {
      for(j = 0; j < (crop.size.width / pixel_skip); j++) {
        *outputAddr = *inputAddr;
        inputAddr += pixel_skip;
        outputAddr++;
      }
      inputAddr += (in_width * pixel_skip) - crop.size.width;
    }
  }

  return IMG_SUCCESS;
}

/**
 * Function: module_afs_client_handle_buffer
 *
 * Description: This function is for handling the buffers
 *            sent from the peer modules
 *
 * Arguments:
 *   @p_client: afs client
 *   @buf_idx: index of the buffer to be processed
 *   @frame_id: frame id
 *   @p_frame_idx: pointer to frame index
 *   @isp_buf: Input buffer
 *   @processing_identity: Identity of stream to be processed
 *
 * Return values:
 *     imaging error values
 *
 * Notes: none
 **/
int module_afs_client_handle_buffer(afs_client_t *p_client,
  uint32_t buf_idx, uint32_t frame_id, int32_t *p_frame_idx,
  isp_buf_divert_t *isp_buf, uint32_t processing_identity)
{
  img_frame_t *p_frame;
  int rc = IMG_SUCCESS;
  int img_idx = 0;
  mct_stream_map_buf_t *p_map_buf;
  int str_idx = 0;
  uint32_t in_width = 0;
  uint32_t in_height = 0;

  *p_frame_idx = -1;
  p_client->frame_id = (int)frame_id;
  /* process the frame only in IMGLIB_STATE_PROCESSING state */
  pthread_mutex_lock(&p_client->mutex);
  if (TRUE != p_client->active) {
    pthread_mutex_unlock(&p_client->mutex);
    return IMG_SUCCESS;
  }

  if (IMGLIB_STATE_PROCESSING != p_client->state) {
    pthread_mutex_unlock(&p_client->mutex);
    return IMG_SUCCESS;
  }

  IDBG_MED("%s:%d] count %d", __func__, __LINE__, p_client->current_count);

  if (0 != p_client->current_count) {
    IDBG_LOW("%s:%d] Skip frame", __func__, __LINE__);
    p_client->current_count =
      (p_client->current_count + 1)%(p_client->frame_skip_cnt + 1);
    pthread_mutex_unlock(&p_client->mutex);
    return IMG_SUCCESS;
  }
  p_client->current_count =
    (p_client->current_count + 1)%(p_client->frame_skip_cnt+1);

  img_idx = module_afs_client_get_buf(p_client);
  IDBG_MED("%s:%d] img_idx %d", __func__, __LINE__, img_idx);


  if(!isp_buf->native_buf) {
    /* check if MCT buffers are mapped */
    if ((buf_idx >= p_client->buffer_cnt)
      || (img_idx >= MAX_NUM_AFS_FRAMES)) {
      IDBG_ERROR("%s:%d] invalid buffer index %d img_idx %d. buffer_cnt %d",
        __func__, __LINE__, buf_idx, img_idx, p_client->buffer_cnt);
      pthread_mutex_unlock(&p_client->mutex);
      return IMG_ERR_OUT_OF_BOUNDS;
    }
  }
  /* More than one stream mapped to port,
    get stream index based on the current active stream identity */
  str_idx  = module_afs_get_stream_by_id(p_client, processing_identity);
  if (str_idx < 0) {
    IDBG_ERROR("%s:%d Invalid stream index", __func__, __LINE__);
    pthread_mutex_unlock(&p_client->mutex);
    return IMG_ERR_OUT_OF_BOUNDS;
  }

  p_frame = &p_client->stream[str_idx].p_frame[img_idx];
  if (isp_buf->native_buf) {
    unsigned long *plane_addr = (unsigned long *)isp_buf->vaddr;

    /* ISP output dim associated with stream */
    in_width = p_client->stream[str_idx].in_dim.width;
    in_height = p_client->stream[str_idx].in_dim.height;

    IDBG_MED("%s:%d] map buf %p %dx%d, isp buf %p", __func__, __LINE__,
      p_frame->frame[0].plane[0].addr, in_width, in_height,
      (uint8_t *)plane_addr[0]);

    if (!p_frame->frame[0].plane[0].addr) {
      IDBG_ERROR("%s:%d Error cannot find mapped buffer", __func__, __LINE__);
      pthread_mutex_unlock(&p_client->mutex);
      return IMG_ERR_GENERAL;
    }

    if (p_client->frame_crop) {
      rc = module_afs_client_crop_frame(p_frame->frame[0].plane[0].addr,
        (uint8_t *)plane_addr[0],
        in_width,
        in_height,
        p_client->cropped_window,
        p_client->num_skip);
      IDBG_LOW("%s:%d] native buf frame crop successful",
        __func__, __LINE__);
    }
  } else {
    p_map_buf = &p_client->stream[str_idx].p_map_buf[buf_idx];
    if (p_client->frame_crop) {
      rc = module_afs_client_crop_frame(p_frame->frame[0].plane[0].addr,
        p_map_buf->buf_planes[0].buf,
        in_width,
        in_height,
        p_client->cropped_window,
        p_client->num_skip);
      IDBG_LOW("%s:%d] non-native buf frame crop successful",
        __func__, __LINE__);
    }
  }

  *p_frame_idx = img_idx;
  /* return the buffer */
  pthread_mutex_unlock(&p_client->mutex);

  return rc;
}

/**
 * Function: module_afs_client_handle_ctrl_parm
 *
 * Description: This function is used to handle the ctrl
 *             commands passed from the MCTL
 *
 * Arguments:
 *   @p_ctrl_event: pointer to mctl ctrl events
 *   @p_client: afs client
 *
 * Return values:
 *     error/success
 *
 * Notes: none
 **/
int module_afs_client_handle_ctrl_parm(afs_client_t *p_client,
  mct_event_control_parm_t *param)
{
  IMG_UNUSED(p_client);

  int status = IMG_SUCCESS;

  if (NULL == param)
    return status;

  IDBG_LOW("%s:%d] param %d", __func__, __LINE__, param->type);
  switch(param->type) {
  default:
    break;
  }
  return status;
}

/**
 * Function: module_afs_load
 *
 * Description: This function is used to load frame
 *             algo library
 *
 * Input parameters:
 *   none
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
int module_afs_load()
{
  if (g_afs_lib.ptr) {
    IDBG_ERROR("%s:%d] library already loaded", __func__, __LINE__);
    return IMG_SUCCESS;
  }

  g_afs_lib.ptr = dlopen("libmmcamera2_frame_algorithm.so", RTLD_NOW);
  if (!g_afs_lib.ptr) {
    IDBG_ERROR("%s:%d] Error opening Frame algo library", __func__, __LINE__);
    return IMG_ERR_NOT_FOUND;
  }

  *(void **)&(g_afs_lib.af_sw_stats_iir_float) =
    dlsym(g_afs_lib.ptr, "af_sw_stats_iir_float");
  if (!g_afs_lib.af_sw_stats_iir_float) {
    IDBG_ERROR("%s:%d] Error linking af_sw_stats_iir_float",
      __func__, __LINE__);
    dlclose(g_afs_lib.ptr);
    g_afs_lib.ptr = NULL;
    return IMG_ERR_NOT_FOUND;
  }
  *(void **)&(g_afs_lib.af_sw_stats_iir_float_neon) =
    dlsym(g_afs_lib.ptr, "af_sw_stats_iir_float_neon");
  if (!g_afs_lib.af_sw_stats_iir_float_neon) {
    IDBG_ERROR("%s:%d] Error linking af_sw_stats_iir_float_neon",
      __func__, __LINE__);
    dlclose(g_afs_lib.ptr);
    g_afs_lib.ptr = NULL;
    return -1;
  }
  *(void **)&(g_afs_lib.FIR_frame) =
    dlsym(g_afs_lib.ptr, "FIR_frame");
  if (!g_afs_lib.FIR_frame) {
    IDBG_ERROR("%s:%d] Error linking FIR_frame",
      __func__, __LINE__);
    dlclose(g_afs_lib.ptr);
    g_afs_lib.ptr = NULL;
    return IMG_ERR_NOT_FOUND;
  }
  IDBG_HIGH("%s:%d] Frame algo library loaded successfully", __func__, __LINE__);
  return IMG_SUCCESS;
}

/**
 * Function: module_afs_unload
 *
 * Description: This function is used to unload frame
 *             algo library
 *
 * Input parameters:
 *   none
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_afs_unload()
{
  int rc = 0;
  IDBG_HIGH("%s:%d] ptr %p", __func__, __LINE__, g_afs_lib.ptr);
  if (g_afs_lib.ptr) {
    rc = dlclose(g_afs_lib.ptr);
    if (rc < 0)
      IDBG_HIGH("%s:%d] error %s", __func__, __LINE__, dlerror());
    g_afs_lib.ptr = NULL;
  }
}
