/**********************************************************************
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "module_imgbase.h"

#define WAIT_TIMEOUT 500000000
#define PTHREAD_COND_WAIT_TIME(cond_ptr, mutex_ptr, \
    timeout_ptr, ns, rc) \
  { \
    clock_gettime(CLOCK_REALTIME, (timeout_ptr)); \
    (timeout_ptr)->tv_nsec += ns;\
    if ((timeout_ptr)->tv_nsec > 999999999) { \
      (timeout_ptr)->tv_nsec -= 999999999; \
      (timeout_ptr)->tv_sec += 1; \
    } \
    rc = pthread_cond_timedwait(cond_ptr, \
      mutex_ptr, timeout_ptr);\
  }

typedef struct {
  uint32_t last_frameid;
  uint32_t flush_done;
  uint32_t      eis_num_mesh_x;
  uint32_t      eis_num_mesh_y;
  float transform_matrix[(IMG_MAX_IS_NUM_VERTICES) * 9];
} img_dewarp_config_t;


/** img_dewarpconfig_data_t:
 *   dewarp based parameters for ppeiscore module
 */
typedef struct {
  img_dewarp_config_t dewarp_cfg;
} img_dewarpconfig_data_t;


/** img_ppseicoremod_priv_t:
 *  @session_data: ppeiscore dewarp data
 *  ppseicore module private structure
 */
typedef struct {
  img_dewarpconfig_data_t session_data;
} img_ppeiscoremod_priv_t;


/**
 *  Static functions
 **/
static int32_t module_ppeiscore_client_created(imgbase_client_t *p_client);
static int32_t module_ppeiscore_client_destroy(imgbase_client_t *p_client);
static int32_t module_ppeiscore_client_streamon(imgbase_client_t * p_client);
static int32_t module_ppeiscore_client_prestreamoff( imgbase_client_t * p_client,
  uint32_t identity);
static int32_t module_ppeiscore_client_flush_done( imgbase_client_t * p_client,
  imgbase_stream_t *p_stream);
static int32_t module_ppeiscore_update_meta(imgbase_client_t *p_client,
  img_meta_t *p_meta);
static boolean module_ppeiscore_handle_stream_crop_event(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_ppeiscore_handle_isp_out_dim_event(uint32_t identity,
  mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_ppeiscore_handle_dis_config_event(uint32_t identity,
  mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_ppeiscore_handle_dis_update_event(uint32_t identity,
  mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_ppeiscore_handle_dewrap_update_event(uint32_t identity,
  mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_ppeiscore_handle_iface_request_pp_divert_event(
  uint32_t identity, mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static int32_t module_ppeiscore_handle_meta_stream_info(mct_event_control_parm_t
  *p_ctrl_parm, imgbase_client_t *p_client, img_core_ops_t *p_core_ops);


/** g_caps:
 *
 *  Set the capabilities for Ppeiscore (PPEISCORE) module
*/
static img_caps_t g_caps = {
  .num_input = 1,
  .num_output = 1,
  .num_meta = 1,
  .inplace_algo = 0,
  .num_release_buf = 0,
  .hold_meta = 1,
  .num_hold_outbuf = 1,
  .use_internal_bufs = 1,
  .internal_buf_cnt = 6,
};

/** g_params:
 *
 *  imgbase parameters
 **/
static module_imgbase_params_t g_params = {
  .imgbase_client_created = module_ppeiscore_client_created,
  .imgbase_client_destroy = module_ppeiscore_client_destroy,
  .imgbase_client_pre_streamoff = module_ppeiscore_client_prestreamoff,
  .imgbase_client_flush_done = module_ppeiscore_client_flush_done,
  .imgbase_client_streamon = module_ppeiscore_client_streamon,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_STREAM_CROP] =
    module_ppeiscore_handle_stream_crop_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_ISP_OUTPUT_DIM] =
    module_ppeiscore_handle_isp_out_dim_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_ISP_DIS_CONFIG] =
    module_ppeiscore_handle_dis_config_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_STATS_DIS_UPDATE] =
    module_ppeiscore_handle_dis_update_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_STATS_DEWARP_UPDATE] =
    module_ppeiscore_handle_dewrap_update_event,
  .imgbase_handle_module_event[ MCT_EVENT_MODULE_IFACE_REQUEST_PP_DIVERT] =
    module_ppeiscore_handle_iface_request_pp_divert_event,
  .imgbase_handle_ctrl_parm[CAM_INTF_META_STREAM_INFO] =
    module_ppeiscore_handle_meta_stream_info,
  .imgbase_client_update_meta = module_ppeiscore_update_meta,
  .exec_mode = IMG_EXECUTION_HW,
  .access_mode = IMG_ACCESS_READ_WRITE,
  .force_cache_op = FALSE,
};


/**
 * Function: module_ppeiscore_client_created
 *
 * Description: function called after client creation
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_ppeiscore_client_created(imgbase_client_t *p_client)
{
  IDBG_LOW("%s:%d: E", __func__, __LINE__);

  p_client->rate_control = TRUE;
  p_client->exp_frame_delay = 0LL;
  p_client->ion_fd = open("/dev/ion", O_RDONLY);
  p_client->before_cpp = TRUE;
  p_client->feature_mask = CAM_QTI_FEATURE_PPEISCORE;

  /* process ppeiscore only when bufs received on port where
    video stream is mapped */
  p_client->streams_to_process = ((1 << CAM_STREAM_TYPE_VIDEO) |( 1 <<
  CAM_STREAM_TYPE_SNAPSHOT));

  /* process ppeiscore on all frames regardless of if HAL3 has a
    request on it */
  p_client->process_all_frames = TRUE;

  /* send preferred stream mapping requesting for preview and
    video on the same port */
  p_client->set_preferred_mapping = TRUE;
  p_client->preferred_mapping_single.stream_num = 1;
  p_client->preferred_mapping_single.streams[0].max_streams_num = 2;
  p_client->preferred_mapping_single.streams[0].stream_mask =
    (1 << CAM_STREAM_TYPE_PREVIEW) | (1 << CAM_STREAM_TYPE_VIDEO);
  p_client->preferred_mapping_multi.stream_num = 1;
  p_client->preferred_mapping_multi.streams[0].max_streams_num = 2;
  p_client->preferred_mapping_multi.streams[0].stream_mask =
    (1 << CAM_STREAM_TYPE_VIDEO) | (1 << CAM_STREAM_TYPE_PREVIEW);

  /* Requires extra buffers . Will update after input from IS module*/
  p_client->isp_extra_native_buf += 0;

  /*Specifying that module will change dimension of buffer for consideration
       during output buffer operations */
  p_client->is_dim_change = TRUE;
  p_client->matrix_type = MATRIX_NONE;

  return IMG_SUCCESS;
}

/**
 * Function: module_ppeiscore_client_streamon
 *
 * Description: function called after stream on
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
static int32_t module_ppeiscore_client_streamon(imgbase_client_t * p_client)
{
  IDBG_LOW("%s:%d: E", __func__, __LINE__);
  int rc = IMG_ERR_GENERAL;
  p_client->processing_disabled = FALSE;

  module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;

  IDBG_MED(" p_client %p stream_cnt %d processing_disabled %d stream_on %d",
    p_client, p_client->stream_cnt, p_client->processing_disabled,
    p_client->stream_on);

  if (p_client->stream_on == 1) {
    /*Use the deault framproc callback*/
    rc = IMG_COMP_SET_PARAM(&p_client->comp, QIMG_PARAM_SET_LIB_CALLBACK, NULL);
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("Fail to set callback");
      return IMG_ERR_INVALID_OPERATION;
    }
  }

  return IMG_SUCCESS;
}


/**
 * Function: module_ppeiscore_client_prestreamoff
 *
 * Description: function called before streamoff
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
static int32_t module_ppeiscore_client_prestreamoff(
imgbase_client_t * p_client, uint32_t identity)
{
  int rc = IMG_SUCCESS;
  img_meta_t *p_meta = NULL;
  int stream_idx = 0;
  imgbase_stream_t *p_stream = NULL;
  struct timespec timeout;
  module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;

  IDBG_LOW(" E got it , queued %d !!", p_client->meta_hold_q.count);

  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);
  if (stream_idx < 0) {
    IDBG_ERROR("[%s] Cannot find stream mapped to idx %x",p_mod->name,
      p_client->divert_identity);
    goto error;
  }

  p_stream = &p_client->stream[stream_idx];

  p_client->processing_disabled = TRUE;
  p_stream->streamon = FALSE;

  if (p_client->meta_hold_q.count == 0 && p_stream->eis_config.flush_done) {
    IDBG_ERROR("No meta queued and flush complete!");
    return IMG_SUCCESS;
  }

  IDBG_LOW("id %x Stream to process type %d stream type %d ", identity,
    p_client->streams_to_process, p_stream->stream_info->stream_type);
  if (p_client->streams_to_process & (1 << p_stream->stream_info->stream_type)) {
    IDBG_LOW("Stream to process type %d ", p_stream->stream_info->stream_type);
  } else {
    IDBG_ERROR("Stream NOT to process type %d return", p_stream->stream_info->stream_type);
    return IMG_ERR_GENERAL;
  }

  //Flush the queued meta/frames
  while(p_client->meta_hold_q.count) {
    IDBG_LOW("Getting held meta from count %d", p_client->meta_hold_q.count);
    p_meta = img_q_dequeue(&p_client->meta_hold_q);
    if (!p_meta) {
      IDBG_ERROR("No meta queued");
      goto error;
    }
    if(p_client->meta_hold_q.count == 0) {
      //Mark the last frame, so that the streamoff can be unblocked
      p_stream->eis_config.last_frameid = p_meta->frame_id;
      p_meta->eis_cfg.last_frame = TRUE;
      IDBG_LOW("Populating the last frame_id %d", p_stream->eis_config.last_frameid);
    } else {
      p_meta->eis_cfg.last_frame = FALSE;
    }

    if (p_client->matrix_type == MATRIX_2DEWARP) {
      //Add event information to meta
      img_ppeiscoremod_priv_t *p_ppeiscoremod_priv =
        (img_ppeiscoremod_priv_t *)p_mod->mod_private;

      if (!p_ppeiscoremod_priv) {
          IDBG_ERROR("Invalid private module data");
          return 0;
      }
      img_dewarpconfig_data_t *p_dewarp_session_data =
          &(p_ppeiscoremod_priv->session_data);
      if (!p_dewarp_session_data) {
        IDBG_ERROR("Invalid priavte session data");
        return 0;
      }
      //Add event information to meta
      memcpy(p_meta->eis_cfg.transform_matrix,
        p_dewarp_session_data->dewarp_cfg.transform_matrix,
        sizeof(p_dewarp_session_data->dewarp_cfg.transform_matrix));
      p_meta->eis_cfg.num_mesh_x =
        p_dewarp_session_data->dewarp_cfg.eis_num_mesh_x;
      p_meta->eis_cfg.num_mesh_y =
        p_dewarp_session_data->dewarp_cfg.eis_num_mesh_y;
    } else {
      //Update the meta
      memcpy(p_meta->eis_cfg.transform_matrix, p_stream->eis_config.transform_matrix,
      sizeof(p_meta->eis_cfg.transform_matrix));
      p_meta->eis_cfg.num_mesh_x = p_stream->eis_config.eis_num_mesh_x;
      p_meta->eis_cfg.num_mesh_y = p_stream->eis_config.eis_num_mesh_y;
    }

    //queue the buffer
    rc = IMG_COMP_Q_META_BUF(&p_client->comp, p_meta);
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("Q_meta failed rc %d", rc);
      goto error;
    }
    p_client->num_meta_queued++;
    IDBG_LOW(" signalling frame %d p_client->num_meta_queued %d", p_meta->frame_id,
      p_client->num_meta_queued);
  }

  //Block streamoff till signalled
  memset(&timeout, 0, sizeof(timeout));
  PTHREAD_COND_WAIT_TIME(&(p_client->cond), &(p_client->mutex),
  &timeout, WAIT_TIMEOUT, rc);
  if (rc == ETIMEDOUT || rc == EINVAL) {
    IDBG_ERROR("Timed Out!!!  errno= %d \
      (ETIMEOUT %d, INVALID %d)\n", rc, ETIMEDOUT, EINVAL);
  }
  pthread_mutex_unlock(&(p_client->mutex));

  IDBG_LOW("After condition wait %d", rc);

  return IMG_SUCCESS;

  error:
  return IMG_ERR_GENERAL;
}

/**
 * Function: module_ppeiscore_client_flush_done
 *
 * Description: function called when flush is complete
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *   @p_stream - IMGLIB_BASE stream
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_ppeiscore_client_flush_done( imgbase_client_t * p_client,
  imgbase_stream_t *p_stream) {
  IDBG_LOW("Flush done indication!");
  p_stream->eis_config.flush_done = 1;
  pthread_cond_broadcast(&(p_client->cond));
  IDBG_LOW("Released streamoff!");

  return IMG_SUCCESS;
}


/**
 * Function: module_ppeiscore_client_destroy
 *
 * Description: function called before client is destroyed
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_ppeiscore_client_destroy(imgbase_client_t *p_client)
{
  IDBG_LOW("%s:%d: E", __func__, __LINE__);
  if (p_client->ion_fd >= 0) {
    close(p_client->ion_fd);
    p_client->ion_fd = -1;
  }
  return IMG_SUCCESS;
}


/**
 * Function: module_ppeiscore_handle_meta_stream_info
 *
 * Description: This function handles the Meta stream Info Ctrl
 * param
 *
 * Arguments:
 *   p_mod_event - pointer of module event
 *   p_client - pointer to imgbase client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
static int32_t module_ppeiscore_handle_meta_stream_info(mct_event_control_parm_t
  *p_ctrl_parm, imgbase_client_t *p_client, img_core_ops_t *p_core_ops)
{
  cam_stream_size_info_t *meta_stream_info;
  int rc = IMG_SUCCESS;
  uint32_t i = 0;
  bool video_4k = 0;

  IDBG_LOW("Got CAM_INTF_META_STREAM_INFO");

  if (!p_ctrl_parm || !p_client || !p_core_ops) {
    IDBG_ERROR(" Error input");
    return IMG_ERR_INVALID_INPUT;
  }

  meta_stream_info = (cam_stream_size_info_t *)(p_ctrl_parm->parm_data);
  if (!meta_stream_info) {
    IDBG_ERROR("Meta stream info is NULL");
    return IMG_ERR_INVALID_INPUT;
  }

  for (i = 0; i < meta_stream_info->num_streams; i++) {
    IDBG_LOW("META_STREAM_INFO type = %d w x h : %d x %d pp_mask = 0x%x feature_mask = 0x%x",
      meta_stream_info->type[i],
      meta_stream_info->stream_sizes[i].width,
    meta_stream_info->stream_sizes[i].height,
    meta_stream_info->postprocess_mask[i], p_client->feature_mask);
    if (meta_stream_info->type[i] == CAM_STREAM_TYPE_VIDEO) {
      video_4k = IS_4k_VIDEO(meta_stream_info->type[i],
        meta_stream_info->stream_sizes[i].width,
        meta_stream_info->stream_sizes[i].height,
        meta_stream_info->postprocess_mask[i]);
      break;
    }
  }

  if (video_4k) {
    IDBG_LOW("Video is 4k");
    /* send preferred stream mapping requesting for preview and
      video on the same port */
    p_client->set_preferred_mapping = TRUE;
    p_client->preferred_mapping_single.stream_num = 2;
    p_client->preferred_mapping_single.streams[0].max_streams_num = 1;
    p_client->preferred_mapping_single.streams[0].stream_mask =
      (1 << CAM_STREAM_TYPE_PREVIEW);
    p_client->preferred_mapping_single.streams[1].max_streams_num = 2;
    p_client->preferred_mapping_single.streams[1].stream_mask =
      (1 << CAM_STREAM_TYPE_VIDEO) | (1 << CAM_STREAM_TYPE_SNAPSHOT);

    p_client->preferred_mapping_multi.stream_num = 2;
    p_client->preferred_mapping_multi.streams[0].max_streams_num = 1;
    p_client->preferred_mapping_multi.streams[0].stream_mask =
      (1 << CAM_STREAM_TYPE_PREVIEW);
    p_client->preferred_mapping_multi.streams[1].max_streams_num = 2;
    p_client->preferred_mapping_multi.streams[1].stream_mask =
      (1 << CAM_STREAM_TYPE_VIDEO) | (1 << CAM_STREAM_TYPE_SNAPSHOT);
  }


  return rc;
}

/**
 * Function: module_customeis_handle_stream_crop_event
 *
 * Description: This function handles stream crop event
 *
 * Arguments:
 *   p_mod_event - pointer of module event
 *   p_client - pointer to imgbase client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_ppeiscore_handle_stream_crop_event(uint32_t identity,
mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  // int rc = IMG_SUCCESS;
  boolean fwd_event = FALSE;
  //uint32_t identity;
  int stream_idx = 0;
  imgbase_stream_t *p_stream = NULL;
  mct_stream_info_t *stream_info = NULL;
  fwd_event = TRUE;

  IDBG_LOW("%s:%d: E", __func__, __LINE__);

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("%s:%d] Error input", __func__, __LINE__);
    return fwd_event;
  }

  mct_bus_msg_stream_crop_t *stream_crop =
      (mct_bus_msg_stream_crop_t *)p_mod_event->module_event_data;
  //identity = pack_identity(stream_crop->session_id, stream_crop->stream_id);
  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);

  if (stream_idx < 0) {
    IDBG_ERROR("%s %d] Cannot find stream mapped to idx %x", __func__, __LINE__,
      identity);
    return fwd_event;
  }

  mct_stream_info_t *input_stream_info = &p_client->isp_output_dim_stream_info;

  p_stream = &p_client->stream[stream_idx];

  if (NULL == p_stream) {
    IDBG_ERROR("%s %d] Cannot find stream mapped to client", __func__, __LINE__);
    return fwd_event;
  }

  stream_info = p_stream->stream_info;

  p_client->stream_crop = *stream_crop;
  p_client->stream_crop_valid = TRUE;

  IDBG_LOW("Before stream_crop.x=%d, stream_crop.y=%d, stream_crop.dx=%d,"
     " stream_crop.dy=%d, identity=0x%x",
     stream_crop->x, stream_crop->y, stream_crop->crop_out_x,
     stream_crop->crop_out_y, identity);

  if (input_stream_info->dim.width &&
    stream_info->dim.width) {
    stream_crop->x = (stream_crop->x * stream_info->dim.width) /
            input_stream_info->dim.width;
    stream_crop->y = (stream_crop->y * stream_info->dim.height) /
            input_stream_info->dim.height;
    stream_crop->crop_out_x = (stream_crop->crop_out_x *
        stream_info->dim.width) /
            input_stream_info->dim.width;
    stream_crop->crop_out_y = (stream_crop->crop_out_y *
        stream_info->dim.height) /
           input_stream_info->dim.height;
    }
  IDBG_LOW("After stream_crop.x=%d, stream_crop.y=%d, stream_crop.dx=%d,"
     " stream_crop.dy=%d, identity=0x%x",
     stream_crop->x, stream_crop->y, stream_crop->crop_out_x,
     stream_crop->crop_out_y, identity);

  return fwd_event;
}

/**
 * Function: module_customeis_handle_isp_out_dim_event
 *
 * Description: This function handles isp out dim event
 *
 * Arguments:
 *   p_mod_event - pointer of module event
 *   p_client - pointer to imgbase client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_ppeiscore_handle_isp_out_dim_event(uint32_t identity,
mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{

  IDBG_LOW("E");

  boolean fwd_event = FALSE;
  imgbase_stream_t *p_stream = NULL;
  int stream_idx = 0;
  *is_evt_handled = FALSE;
  mct_stream_info_t *stream_info = NULL;

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  stream_idx = module_imgbase_find_stream_by_identity(p_client,
                 identity);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x", identity);
    return FALSE;
  }

  p_stream = &p_client->stream[stream_idx];

  IDBG_LOW("dewarp_type %d and is_type %d",
    p_stream->stream_info->dewarp_type,p_stream->stream_info->is_type);

  if(p_stream->stream_info->dewarp_type > 0) {

    stream_info =
        (mct_stream_info_t *)(p_mod_event->module_event_data);

    p_client->isp_output_dim_stream_info = *stream_info;
    *(p_stream->stream_info) = *stream_info;
    IDBG_LOW("updated identity %x pe w x h = %d x %d", identity,
      p_stream->stream_info->dim.width ,p_stream->stream_info->dim.height);
  }

  if(p_stream->stream_info->dewarp_type > 0 &&
    p_stream->stream_info->is_type != IS_TYPE_EIS_3_0 &&
    p_stream->stream_info->is_type != IS_TYPE_EIS_DG) {
    fwd_event = TRUE;
  } else {
     IDBG_HIGH("Not Forwarding MCT_EVENT_MODULE_ISP_OUTPUT_DIM");
  }
  IDBG_LOW("X");
  return fwd_event;
}



/**
 * Function: ppeiscore_update_stream_dimensions
 *
 * Description: This function updates stream dimensions
 *
 * Arguments:
 *   stream - Stream
 *   dis_config - dis config event payload
 *
 * Return values:
 *     int32_t - error code
 *
 * Notes: none
 **/
int32_t ppeiscore_update_stream_dimensions(
  imgbase_stream_t *stream, isp_dis_config_info_t *dis_config)
{
  uint32_t i = 0;
  int color_fmt = 0;
  if (!stream || !dis_config) {
    IDBG_ERROR("failed stream_params %p, dis_config %p",
       stream, dis_config);
    return -EFAULT;
  }

  IDBG_LOW("B id %x stream op %dX%d plane[0] %dX%d len %d"
    " plane[1] %dX%d len %d frame_len %d\n",
    stream->identity,
    stream->stream_info->dim.width,
    stream->stream_info->dim.height,
    stream->stream_info->buf_planes.plane_info.mp[0].stride,
    stream->stream_info->buf_planes.plane_info.mp[0].scanline,
    stream->stream_info->buf_planes.plane_info.mp[0].len,
    stream->stream_info->buf_planes.plane_info.mp[1].stride,
    stream->stream_info->buf_planes.plane_info.mp[1].scanline,
    stream->stream_info->buf_planes.plane_info.mp[1].len,
    stream->stream_info->buf_planes.plane_info.frame_len);
  stream->stream_info->dim.width = dis_config->width;
  stream->stream_info->dim.height = dis_config->height;

  stream->stream_info->buf_planes.plane_info.frame_len = 0;
  color_fmt =
    module_imglib_common_get_venus_color_format(stream->stream_info->fmt);

  stream->stream_info->buf_planes.plane_info.frame_len =
    VENUS_BUFFER_SIZE(color_fmt, stream->stream_info->dim.width,
      stream->stream_info->dim.height);

  for (i = 0; i < stream->stream_info->buf_planes.plane_info.num_planes; i++) {
    if (i == 0) { // Y Plane
      stream->stream_info->buf_planes.plane_info.mp[i].stride =
        VENUS_Y_STRIDE(color_fmt, stream->stream_info->dim.width);
      stream->stream_info->buf_planes.plane_info.mp[i].scanline =
        VENUS_Y_SCANLINES(color_fmt, stream->stream_info->dim.height);
      if (stream->stream_info->fmt == CAM_FORMAT_YUV_420_NV12_UBWC) {
        stream->stream_info->buf_planes.plane_info.mp[i].len =
          (uint32_t)(MSM_MEDIA_ALIGN(
                    (stream->stream_info->buf_planes.plane_info.mp[i].stride *
                     stream->stream_info->buf_planes.plane_info.mp[i].scanline),
                     4096) +
          stream->stream_info->buf_planes.plane_info.mp[i].meta_len);
      } else {
        stream->stream_info->buf_planes.plane_info.mp[i].len =
          (stream->stream_info->buf_planes.plane_info.mp[i].stride *
          stream->stream_info->buf_planes.plane_info.mp[i].scanline);
      }
    } else {
      stream->stream_info->buf_planes.plane_info.mp[i].stride =
        VENUS_UV_STRIDE(color_fmt, stream->stream_info->dim.width);
      stream->stream_info->buf_planes.plane_info.mp[i].scanline =
        VENUS_UV_SCANLINES(color_fmt, stream->stream_info->dim.height);

    stream->stream_info->buf_planes.plane_info.mp[i].len =
        stream->stream_info->buf_planes.plane_info.frame_len -
        stream->stream_info->buf_planes.plane_info.mp[0].len;
    }
  }

  IDBG_LOW(",A id %x stream op %dX%d plane[0] %dX%d len %d"
    " plane[1] %dX%d len %d frame_len %d\n",
    stream->identity,
    stream->stream_info->dim.width,
    stream->stream_info->dim.height,
    stream->stream_info->buf_planes.plane_info.mp[0].stride,
    stream->stream_info->buf_planes.plane_info.mp[0].scanline,
    stream->stream_info->buf_planes.plane_info.mp[0].len,
    stream->stream_info->buf_planes.plane_info.mp[1].stride,
    stream->stream_info->buf_planes.plane_info.mp[1].scanline,
    stream->stream_info->buf_planes.plane_info.mp[1].len,
    stream->stream_info->buf_planes.plane_info.frame_len);
  return 0;
}

boolean module_ppeiscore_handle_dis_config_event(uint32_t identity,
mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  boolean fwd_event = TRUE;
  *is_evt_handled = TRUE;
  int32_t stream_idx;
  uint32_t i = 0 ;
  img_component_ops_t *p_comp = NULL;
  imgbase_stream_t *p_stream = NULL;
  mct_stream_info_t *stream_info = NULL;
  int rc = IMG_SUCCESS;

  isp_dis_config_info_t *dis_config =
    (isp_dis_config_info_t *) (p_mod_event->module_event_data);
  if (!dis_config){
    IDBG_ERROR("failed\n");
    return fwd_event;
  }

  IDBG_LOW(" MCT_EVENT_MODULE_ISP_DIS_CONFIG , sid = %u, strid = %x, "
    " col_num = %u, row_num = %u, w = %u, h = %u",
    dis_config->session_id, dis_config->stream_id,
    dis_config->col_num, dis_config->row_num,
    dis_config->width, dis_config->height);

  //Update the internal stream dimensions i.e this modules o/p dimensions
  stream_idx = module_imgbase_find_stream_by_identity(p_client,
    identity);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x")
    return IMG_ERR_NOT_FOUND;
  }

  p_stream = &p_client->stream[stream_idx];
  if (NULL == p_stream) {
    IDBG_ERROR(" Cannot find stream mapped to client");
    return IMG_ERR_NOT_FOUND;
  }
  IDBG_LOW("B Strm Info dim %dX%d strXsca %dX%d \n", p_stream->stream_info->dim.width,
    p_stream->stream_info->dim.height, p_stream->stream_info->buf_planes.plane_info.mp[0].stride,
    p_stream->stream_info->buf_planes.plane_info.mp[0].scanline,
    p_stream->stream_info->buf_planes.plane_info.frame_len);

  //Update internal stream dimension information
  ppeiscore_update_stream_dimensions(p_stream, dis_config);

  IDBG_LOW("A Strm Info dim %dX%d strXsca %dX%d \n", p_stream->stream_info->dim.width,
    p_stream->stream_info->dim.height, p_stream->stream_info->buf_planes.plane_info.mp[0].stride,
    p_stream->stream_info->buf_planes.plane_info.mp[0].scanline,
    p_stream->stream_info->buf_planes.plane_info.frame_len);

  //Send ISP output dim event to CPP
  stream_info = p_stream->stream_info;

  mct_stream_info_t new_out_stream_info;
  new_out_stream_info.fmt = p_stream->isp_stream_info.fmt;
  new_out_stream_info.stream_type = stream_info->stream_type;
  new_out_stream_info.dim.width = stream_info->dim.width;
  new_out_stream_info.dim.height = stream_info->dim.height;
  new_out_stream_info.buf_planes.plane_info.num_planes =
    stream_info->buf_planes.plane_info.num_planes;
  new_out_stream_info.buf_planes.plane_info.frame_len = 0;

  IDBG_LOW("stream type %d, width %d, height %d num planes %d",
    stream_info->stream_type, new_out_stream_info.dim.width,
    new_out_stream_info.dim.height,
    new_out_stream_info.buf_planes.plane_info.num_planes);

  for (i = 0; i < stream_info->buf_planes.plane_info.num_planes; i++) {
    new_out_stream_info.buf_planes.plane_info.mp[i].stride =
      stream_info->buf_planes.plane_info.mp[i].stride;
    new_out_stream_info.buf_planes.plane_info.mp[i].scanline =
      stream_info->buf_planes.plane_info.mp[i].scanline;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset =
      stream_info->buf_planes.plane_info.mp[i].offset;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset_x =
      stream_info->buf_planes.plane_info.mp[i].offset;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset_y =
      stream_info->buf_planes.plane_info.mp[i].offset;
    new_out_stream_info.buf_planes.plane_info.mp[i].len =
      stream_info->buf_planes.plane_info.mp[i].stride *
      stream_info->buf_planes.plane_info.mp[i].scanline;
      //stream_info->buf_planes.plane_info.mp[i].len;
    new_out_stream_info.buf_planes.plane_info.frame_len +=
      stream_info->buf_planes.plane_info.mp[i].len;
    IDBG_LOW("new plane %d, stride %d, scanline %d,"
      "offset_x %d, offset_y %d, len %d, offset:%d", i,
      new_out_stream_info.buf_planes.plane_info.mp[i].stride,
      new_out_stream_info.buf_planes.plane_info.mp[i].scanline,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset_x,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset_y,
      new_out_stream_info.buf_planes.plane_info.mp[i].len,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset);
  }

  if(p_stream->stream_info->dewarp_type == 0 &&
    p_stream->stream_info->is_type == IS_TYPE_EIS_3_0) {
    p_client->matrix_type = MATRIX_EIS_3_0;
    IDBG_LOW("matrix_type %d", p_client->matrix_type);
    p_comp = &p_client->comp;
    if (p_comp) {
      img_lib_config_t lib_config;
      lib_config.lib_data = (void *)&(p_client->matrix_type);
      lib_config.lib_param = IMG_ALGO_PRESTART;
      rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_LIB_CONFIG,
        &lib_config);
      if (IMG_ERROR(rc)) {
        IDBG_ERROR("Fail to map stream buffers");
        rc = IMG_ERR_INVALID_OPERATION;
      }
    }
  }

  rc = mod_imgbase_send_event(identity, FALSE,
    MCT_EVENT_MODULE_ISP_OUTPUT_DIM, new_out_stream_info);

  if (!rc) {
    IDBG_ERROR(" Sending ISP OUT DIM failed %d", rc);
    return IMG_ERR_GENERAL;
  }

  return fwd_event;
}

/**
 * Function: module_ppeiscore_handle_dis_update_event
 *
 * Description: This function handles dis update event
 *
 * Arguments:
 *   p_mod_event - pointer of module event
 *   p_client - pointer to imgbase client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_ppeiscore_handle_dis_update_event(uint32_t identity,
mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  int rc = IMG_SUCCESS;
  img_meta_t *p_meta = NULL;
  img_meta_t *p_peek_meta = NULL;
  int stream_idx = 0;
  imgbase_stream_t *p_stream = NULL;

  boolean fwd_event = FALSE;
  *is_evt_handled = TRUE;

   is_update_t *is_update =
    (is_update_t *)(p_mod_event->module_event_data);
  if(!is_update) {
    IDBG_ERROR("%s:%d, failed\n", __func__, __LINE__);
    return FALSE;
  }

  stream_idx = module_imgbase_find_stream_by_identity(p_client,
                 is_update->id);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x", is_update->id);
    return FALSE;
  }

  p_stream = &p_client->stream[stream_idx];

  IDBG_LOW("Got DIS update event for id %x frameid %d ",identity, is_update->frame_id);
  IDBG_LOW("Matrix 1 : %f %f %f | %f %f %f | %f %f %f ",
    is_update->transform_matrix[0], is_update->transform_matrix[1],is_update->transform_matrix[2],
    is_update->transform_matrix[3], is_update->transform_matrix[4],is_update->transform_matrix[5],
    is_update->transform_matrix[6], is_update->transform_matrix[7],is_update->transform_matrix[8]);
  IDBG_LOW("Matrix 2 : %f %f %f | %f %f %f | %f %f %f ",
    is_update->transform_matrix[9], is_update->transform_matrix[10],is_update->transform_matrix[11],
    is_update->transform_matrix[12], is_update->transform_matrix[13],is_update->transform_matrix[14],
    is_update->transform_matrix[15], is_update->transform_matrix[16],is_update->transform_matrix[17]);

  if (img_q_count(&p_client->meta_hold_q) > 0) {
    p_peek_meta = img_q_peek(&p_client->meta_hold_q);
    if (!p_peek_meta) {
      IDBG_ERROR("No meta queued in peek");
      goto error;
    }
  } else {
    module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;
    if (CAM_HAL_V3 == p_mod->hal_version) {
      // For HAL3, this is not an error. There could be possibility that there
      // is no frame request for this frame_id, so we skipped frame and then
      // getting DIS update event.
      IDBG_MED("No meta queued in peek");
      return fwd_event;
    }
  }

  if (!p_peek_meta) {
      IDBG_ERROR("No meta queued in peek");
      goto error;
  }

  IDBG_LOW("peek event frame id %d meta frameid %d meta count %d ",
    is_update->frame_id, p_peek_meta->frame_id, p_client->meta_hold_q.count);

  //Error handling for mismatched event
  if(p_peek_meta->frame_id < is_update->frame_id) {
    IDBG_ERROR("MISMATCH Frame queued, but matrix for future frame. Need to flush");
    while(p_peek_meta->frame_id < is_update->frame_id) {
      //dequeue the buffer
      p_meta = img_q_dequeue(&p_client->meta_hold_q);
      if (!p_meta) {
        IDBG_ERROR("No meta queued");
        goto error;
      }

      //Update meta informaiton
      memcpy(p_meta->eis_cfg.transform_matrix, p_stream->eis_config.transform_matrix,
        sizeof(p_meta->eis_cfg.transform_matrix));
      p_meta->eis_cfg.num_mesh_x = p_stream->eis_config.eis_num_mesh_x;
      p_meta->eis_cfg.num_mesh_y = p_stream->eis_config.eis_num_mesh_y;
      p_meta->eis_cfg.last_frame = FALSE;

      //queue the buffer
      rc = IMG_COMP_Q_META_BUF(&p_client->comp, p_meta);
      if (IMG_ERROR(rc)) {
        IDBG_ERROR("Q_meta failed rc %d", rc);
        goto error;
      }
      p_client->num_meta_queued++;

      //Update peek to decide additional flush needed or not
      p_peek_meta = img_q_peek(&p_client->meta_hold_q);
      if (!p_peek_meta) {
        IDBG_ERROR("No meta queued in peek");
        goto error;
      }
      IDBG_LOW("flush peek event frame id %d meta frameid %d",
        is_update->frame_id, p_peek_meta->frame_id);
    }
    if(p_peek_meta->frame_id == is_update->frame_id) {
      IDBG_LOW("Flush complete. State restored.");
    } else {
      IDBG_LOW("Flush. State Error.event frame id %d meta frameid %d",
        is_update->frame_id, p_peek_meta->frame_id);
      goto error;
    }
  } else if (p_peek_meta->frame_id > is_update->frame_id) {
    IDBG_ERROR("MISMATCH Old matrix. Ignore event, since no frame for it!");
    //Update matrix information since its latest
    memcpy(p_stream->eis_config.transform_matrix, is_update->transform_matrix,
      is_update->num_matrices * sizeof(float) * NUM_MATRIX_ELEMENTS);
    goto error;
  } else {
    IDBG_LOW("MATCH Matrix for the frame queued.");
  }

  IDBG_LOW("Getting held meta from count %d", p_client->meta_hold_q.count);
  p_meta = img_q_dequeue(&p_client->meta_hold_q);
  if (!p_meta) {
    IDBG_ERROR("No meta queued");
    goto error;
  }

  //Modify meta buffer
  IDBG_LOW("Event frame id %d meta frameid %d", is_update->frame_id,
    p_meta->frame_id);

  //Add event information to meta
  memcpy(p_meta->eis_cfg.transform_matrix, is_update->transform_matrix,
    is_update->num_matrices * sizeof(float) * NUM_MATRIX_ELEMENTS);
  //Store for dropped/streamoff cases
  memcpy(p_stream->eis_config.transform_matrix, is_update->transform_matrix,
    is_update->num_matrices * sizeof(float) * NUM_MATRIX_ELEMENTS);
  p_meta->eis_cfg.num_mesh_x = p_stream->eis_config.eis_num_mesh_x;
  p_meta->eis_cfg.num_mesh_y = p_stream->eis_config.eis_num_mesh_y;
  p_meta->eis_cfg.last_frame = FALSE;

  //queue the buffer
  rc = IMG_COMP_Q_META_BUF(&p_client->comp, p_meta);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("Q_meta failed rc %d", rc);
    goto error;
  }
  p_client->num_meta_queued++;
  IDBG_LOW(" p_client->num_meta_queued %d", p_client->num_meta_queued);

  return fwd_event;

  error:
  IDBG_ERROR("Error path!");
  return fwd_event;
}

/**
 * Function: module_ppeiscore_handle_dewrap_update_event
 *
 * Description: This function handles dewarp event
 *
 * Arguments:
 *   p_mod_event - pointer of module event
 *   p_client - pointer to imgbase client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_ppeiscore_handle_dewrap_update_event(uint32_t identity,
  mct_event_module_t *p_mod_event,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  img_component_ops_t *p_comp = NULL;
  int rc = IMG_SUCCESS;
  boolean fwd_event = FALSE;
  *is_evt_handled = TRUE;
  imgbase_stream_t *p_stream = NULL;
  int stream_idx = 0;

  module_imgbase_t *p_mod;
  if (!p_client) {
    IDBG_ERROR("Error input");
    return FALSE;
  }
  p_mod = (module_imgbase_t *)p_client->p_mod;

  dewarp_update_t *dewarp_update =
    (dewarp_update_t *)(p_mod_event->module_event_data);
  if(!dewarp_update) {
    IDBG_ERROR("%s:%d, failed\n", __func__, __LINE__);
    return FALSE;
  }

  img_ppeiscoremod_priv_t *p_ppeiscoremod_priv =
    (img_ppeiscoremod_priv_t *)p_mod->mod_private;

  if (!p_ppeiscoremod_priv) {
    IDBG_ERROR("Invalid private module data");
    return FALSE;
  }

  img_dewarpconfig_data_t *p_dewarp_session_data =
    &(p_ppeiscoremod_priv->session_data);

  if (!p_dewarp_session_data) {
    IDBG_ERROR("Invalid priavte session data");
    return FALSE;
  }

  stream_idx = module_imgbase_find_stream_by_identity(p_client,
                 dewarp_update->id);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x", dewarp_update->id);
    return FALSE;
  }

  p_stream = &p_client->stream[stream_idx];

  IDBG_LOW("Got Dewarp update event for id %x",identity);
  IDBG_LOW("Matrix 1 : %f %f %f | %f %f %f | %f %f %f ",
    dewarp_update->transform_matrix[0],
    dewarp_update->transform_matrix[1],
    dewarp_update->transform_matrix[2],
    dewarp_update->transform_matrix[3],
    dewarp_update->transform_matrix[4],
    dewarp_update->transform_matrix[5],
    dewarp_update->transform_matrix[6],
    dewarp_update->transform_matrix[7],
    dewarp_update->transform_matrix[8]);
  IDBG_LOW("Matrix 2 : %f %f %f | %f %f %f | %f %f %f ",
    dewarp_update->transform_matrix[9],
    dewarp_update->transform_matrix[10],
    dewarp_update->transform_matrix[11],
    dewarp_update->transform_matrix[12],
    dewarp_update->transform_matrix[13],
    dewarp_update->transform_matrix[14],
    dewarp_update->transform_matrix[15],
    dewarp_update->transform_matrix[16],
    dewarp_update->transform_matrix[17]);

  if(p_stream->stream_info->dewarp_type > 0 ||
    p_stream->stream_info->is_type == IS_TYPE_EIS_DG) {
    p_client->matrix_type = MATRIX_2DEWARP;
    IDBG_LOW("matrix_type %d", p_client->matrix_type);
    p_comp = &p_client->comp;
    if (p_comp) {
      img_lib_config_t lib_config;
      lib_config.lib_data = &(p_client->matrix_type);
      lib_config.lib_param = IMG_ALGO_PRESTART;
      rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_LIB_CONFIG,
        &lib_config);
      if (IMG_ERROR(rc)) {
        IDBG_ERROR("Fail to map stream buffers");
        rc = IMG_ERR_INVALID_OPERATION;
      }
    }
  }

  memcpy(p_dewarp_session_data->dewarp_cfg.transform_matrix,
    dewarp_update->transform_matrix,
    dewarp_update->num_matrices * sizeof(float) * NUM_MATRIX_ELEMENTS);
  p_dewarp_session_data->dewarp_cfg.eis_num_mesh_x = IMG_MAX_IS_MESH_X;
  p_dewarp_session_data->dewarp_cfg.eis_num_mesh_y = IMG_MAX_IS_MESH_Y;

  return fwd_event;
}

boolean module_ppeiscore_handle_iface_request_pp_divert_event(
  uint32_t identity, mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  int rc = IMG_SUCCESS;
  boolean fwd_event = TRUE;
  int stream_idx = 0;
  imgbase_stream_t *p_stream = NULL;

  IDBG_LOW("PP divert request event for %x", identity);
  *is_evt_handled = FALSE;

  mct_event_is_config_t is_config;
  memset(&is_config, 0, sizeof(mct_event_is_config_t));

  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);

  if (stream_idx < 0) {
    IDBG_ERROR("%s %d] Cannot find stream mapped to idx %x", __func__, __LINE__,
      identity);
    return IMG_ERR_GENERAL;
  }

  p_stream = &p_client->stream[stream_idx];
  rc = mod_imgbase_send_event(identity, TRUE,
    MCT_EVENT_MODULE_IS_CONFIG, is_config);

  if (!rc) {
    IDBG_ERROR("%s:%d] Sending ISP OUT DIM failed %d",
    __func__, __LINE__, rc);
    return IMG_ERR_GENERAL;
  }
  memset(&p_stream->eis_config, 0, sizeof(img_eis_config_t));
  p_stream->eis_config.eis_num_mesh_x = is_config.num_mesh_x;
  p_stream->eis_config.eis_num_mesh_y = is_config.num_mesh_y;
  IDBG_LOW("Got IS input %f %d %d %d", is_config.margin_value,
        is_config.num_buffers, is_config.num_mesh_x, is_config.num_mesh_y);
  //Update the number of extra native bufs needed.

  if((p_stream->stream_info->is_type == IS_TYPE_EIS_3_0 ||
    p_stream->stream_info->is_type == IS_TYPE_EIS_DG) &&
    p_stream->stream_info->dewarp_type == 0) {
      p_client->isp_extra_native_buf += is_config.num_buffers;
  }
  IDBG_LOW("isp_extra_native_buf %d",p_client->isp_extra_native_buf);

  pp_buf_divert_request_t *divert_request;
  divert_request =
   (pp_buf_divert_request_t *)(p_mod_event->module_event_data);
  divert_request->buf_alignment = 128;

  return fwd_event;
}

/**
 * Function: module_ppeiscore_update_meta
 *
 * Description: This function is used to called when the base
 *                       module updates the metadata
 *
 * Arguments:
 *   @p_client - pointer to imgbase client
 *   @p_meta: pointer to the image meta
 *
 * Return values:
 *     error values
 *
 * Notes: none
 **/
int32_t module_ppeiscore_update_meta(imgbase_client_t *p_client,
  img_meta_t *p_meta)
{

  int rc = IMG_SUCCESS;
  imgbase_stream_t *p_stream = NULL;
  int stream_idx = 0;

  module_imgbase_t *p_mod;
  if (!p_client) {
    IDBG_ERROR("Error input");
    return 0;
  }
  p_mod = (module_imgbase_t *)p_client->p_mod;

  IDBG_LOW("matrix_type %d, Getting held meta from count %d",
    p_client->matrix_type, p_client->meta_hold_q.count);

  stream_idx = module_imgbase_find_stream_by_identity(p_client,
                 p_client->divert_identity);

  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x",
    p_client->divert_identity);
    return 0;
  }

  p_stream = &p_client->stream[stream_idx];

  if (p_client->matrix_type == MATRIX_2DEWARP) {
    p_meta = img_q_dequeue(&p_client->meta_hold_q);
    if (!p_meta) {
      IDBG_ERROR("No meta queued");
      return 0;
    }

    //Modify meta buffer
    IDBG_LOW("meta frameid %d", p_meta->frame_id);

    //Add event information to meta
    img_ppeiscoremod_priv_t *p_ppeiscoremod_priv =
      (img_ppeiscoremod_priv_t *)p_mod->mod_private;

    if (!p_ppeiscoremod_priv) {
        IDBG_ERROR("Invalid private module data");
        return 0;
    }
    img_dewarpconfig_data_t *p_dewarp_session_data =
        &(p_ppeiscoremod_priv->session_data);
    if (!p_dewarp_session_data) {
      IDBG_ERROR("Invalid priavte session data");
      return 0;
    }
    //Add event information to meta
    memcpy(p_meta->eis_cfg.transform_matrix,
      p_dewarp_session_data->dewarp_cfg.transform_matrix,
      sizeof(p_dewarp_session_data->dewarp_cfg.transform_matrix));
    p_meta->eis_cfg.num_mesh_x =
      p_dewarp_session_data->dewarp_cfg.eis_num_mesh_x;
    p_meta->eis_cfg.num_mesh_y =
      p_dewarp_session_data->dewarp_cfg.eis_num_mesh_y;
    p_meta->eis_cfg.last_frame = FALSE;

    //queue the buffer
    rc = IMG_COMP_Q_META_BUF(&p_client->comp, p_meta);
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("Q_meta failed rc %d", rc);
      return 0;
    }
    p_client->num_meta_queued++;
    IDBG_LOW(" p_client->num_meta_queued %d", p_client->num_meta_queued);
  }

  return 0;
}
/**
 * Function: module_ppeiscore_deinit
 *
 * Description: Function used to deinit Ppeiscore module
 *
 * Arguments:
 *   p_mct_mod - MCTL module instance pointer
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_ppeiscore_deinit(mct_module_t *p_mct_mod)
{
  IDBG_LOW("E");

  module_imgbase_t *p_mod = NULL;
  img_ppeiscoremod_priv_t *p_ppeiscorepmod_priv = NULL;

  if(!p_mct_mod) {
    IDBG_ERROR("Invalid module %p", p_mct_mod);
    return;
  }

  p_mod = (module_imgbase_t *)p_mct_mod->module_private;
  if (!p_mod) {
    IDBG_ERROR("Invalid base module %p", p_mod);
    return;
  }

  p_ppeiscorepmod_priv = p_mod->mod_private;
  if (p_ppeiscorepmod_priv) {
    free(p_ppeiscorepmod_priv);
    p_ppeiscorepmod_priv = NULL;
  }

  module_imgbase_deinit(p_mct_mod);
}

/** module_ppeiscore_init:
 *
 *  Arguments:
 *  @name - name of the module
 *
 * Description: Function used to initialize the SeeMore module
 *
 * Return values:
 *     MCTL module instance pointer
 *
 * Notes: none
 **/
mct_module_t *module_ppeiscore_init(const char *name)
{
  IDBG_LOW("E");

  img_ppeiscoremod_priv_t *p_pperiscore_priv_data = NULL;
  p_pperiscore_priv_data = calloc(1, sizeof(img_ppeiscoremod_priv_t ));

  if(!p_pperiscore_priv_data) {
    IDBG_ERROR("Unable allocate module private data");
    return NULL;
  }

  memset(&(p_pperiscore_priv_data->session_data), 0x0,
    sizeof(p_pperiscore_priv_data->session_data));

  return module_imgbase_init(name,
    IMG_COMP_GEN_FRAME_PROC,
    "qcom.gen_frameproc",
    p_pperiscore_priv_data,
    &g_caps,
    "libmmcamera_ppeiscore.so",
    CAM_QTI_FEATURE_PPEISCORE,
    &g_params);
}


/** module_ppeiscore_set_parent:
 *
 *  Arguments:
 *  @p_parent - parent module pointer
 *
 * Description: This function is used to set the parent pointer
 *              of the PPEISCORE (SeeMore) module
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_ppeiscore_set_parent(mct_module_t *p_mct_mod, mct_module_t *p_parent)
{
  IDBG_LOW("%s:%d: E", __func__, __LINE__);
  return module_imgbase_set_parent(p_mct_mod, p_parent);
}

