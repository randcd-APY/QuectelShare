/***************************************************************************
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.                      *
* All Rights Reserved.                                                     *
* Confidential and Proprietary - Qualcomm Technologies, Inc.               *
***************************************************************************/

#include "module_qdc_common.h"

/** IMG_QDC_COMMON_GET_OUTPUT_INDEX:
 *
 *  @p_session: ptr to SAT session data
 *
 *   Returns index of the output stream
 */
#define IMG_QDC_COMMON_GET_OUTPUT_INDEX(p_session) ({ \
  uint32_t idx = 0, i; \
  if (p_session->output_size.num_streams > 1) { \
    for (i = 0; i < p_session->output_size.num_streams; i++) { \
      if(p_session->output_size.type[idx] == \
        p_session->process_stream_type) { \
        idx = i; \
        break; \
      } \
    } \
  } \
  idx; \
}) \

/** IMG_QDC_COMMON_CPY_ISP_STREAM_SIZE:
 *
 *  @p_dest: ptr to ISP stream size
 *  @p_src: ptr to ISP stream size
 *  @dest_idx: dest stream idx
 *  @src_idx: src stream idx
 *
 *  Copies ISP stream size for a perticular stream
 */
#define IMG_QDC_COMMON_CPY_ISP_STREAM_SIZE(p_dest, p_src, dest_idx, src_idx) ({ \
  (p_dest)->stream_sizes[dest_idx] = (p_src)->stream_sizes[src_idx]; \
  (p_dest)->type[dest_idx] = (p_src)->type[src_idx]; \
  (p_dest)->margins[dest_idx] = (p_src)->margins[src_idx]; \
  (p_dest)->stream_sz_plus_margin[dest_idx] = \
    (p_src)->stream_sz_plus_margin[src_idx]; \
  (p_dest)->stream_format[dest_idx] = (p_src)->stream_format[src_idx]; \
  (p_dest)->num_additional_buffers[dest_idx] = \
    (p_src)->num_additional_buffers[src_idx]; \
  (p_dest)->width_alignment = (p_src)->width_alignment; \
  (p_dest)->height_alignment = (p_src)->height_alignment; \
}) \

/** IMG_QDC_COMMON_PRINT_ISP_STREAM_SIZE:
 *
 *  @p_stream_size: ptr to ISP stream size
 *  @idx: dest stream idx
 *  @str: custom string
 *
 *  Copies ISP stream size for a perticular stream
 */
#define IMG_QDC_COMMON_PRINT_ISP_STREAM_SIZE(p_stream_size, idx, str) ({ \
  IDBG_INFO("%s: stream type %d stream sz wxh %dx%d, Margins width %f, height %f," \
    " sz_plus_margin wxh %dx%d, alignment wxh %dx%d," \
    "format %d", str, p_stream_sizes->type[isp_idx],\
    p_stream_sizes->stream_sizes[isp_idx].width,\
    p_stream_sizes->stream_sizes[isp_idx].height,\
    p_stream_sizes->margins[isp_idx].widthMargins, \
    p_stream_sizes->margins[isp_idx].heightMargins, \
    p_stream_sizes->stream_sz_plus_margin[isp_idx].width, \
    p_stream_sizes->stream_sz_plus_margin[isp_idx].height, \
    p_stream_sizes->width_alignment, \
    p_stream_sizes->height_alignment, \
    p_stream_sizes->stream_format[isp_idx]); \
}) \

/**
 ** IMG_GET_CAM_DATA
 *
 * Description: This method is used to get client cam info
 *
 * Arguments:
 *   @p_client: Imgbase client
 *   @p_base_sess_data: Ptr to base session data that will be
 *                    filled in
 *   @p_cam_info: client cam info that will be filled
 *
 * Return values:
 *     None
 *
 */
#define IMG_GET_CAM_DATA(p_client, p_base_sess_data, p_cam_info)({\
  module_imgbase_t *p_mod; \
  p_mod = (module_imgbase_t *)p_client->p_mod; \
  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id); \
  if (p_base_sess_data) { \
   p_cam_info = &p_base_sess_data->multi_cam_info; \
  } \
  else { \
    IDBG_ERROR("Base session data is NULL"); \
  } \
}) \

/**
 * Function: module_qdc_common_find_peer_clients
 *
 * Description: This method is used to find the peer clients
 * in multi camera mode.
 *
 * Arguments:
 *   @p_fp_data: imgbase client
 *   @p_input: input data
 *
 * Return values:
 *     true/false
 *
 * Notes: The functions checks for number of matching stream
 * type, width and height
 */
boolean module_qdc_common_find_peer_clients(void *p_imgbase_data,
  void *p_input)
{
  int i, j;
  boolean result = FALSE;
  int str_count;
  imgbase_client_t *p_client = (imgbase_client_t *)p_imgbase_data;
  imgbase_client_t *p_cur_client = (imgbase_client_t *)p_input;
  module_imgbase_t *p_mod = (module_imgbase_t *)p_cur_client->p_mod;

  imgbase_session_data_t *p_cur_sess_data =
    IMGBASE_SSP(p_mod, p_cur_client->session_id);
  if (NULL == p_cur_sess_data) {
    IDBG_ERROR("Imgbase session data is NULL");
    return IMG_ERR_INVALID_INPUT;
  }
  imgbase_multi_camera_info_t *multicam = &p_cur_sess_data->multi_cam_info;

  //Client cannot be its own peer
  if (p_cur_client == p_client) {
    return FALSE;
  }

  for (i = 0; i < multicam->num_peer_sessions; i++) {
    if (p_client->session_id == IMGLIB_SESSIONID(multicam->peer_session_ids[i])) {
      //Stream count is the min of the 2 clients
      str_count = MIN(p_cur_client->stream_cnt, p_client->stream_cnt);
      for (j = 0; j < str_count; j++) {
        if (((p_client->stream[j].stream_info->stream_type) ==
          (p_cur_client->stream[j].stream_info->stream_type)) &&
          ((p_client->stream[j].stream_info->dim.width ==
           p_cur_client->stream[j].stream_info->dim.width)) &&
          ((p_client->stream[j].stream_info->dim.height) ==
          (p_cur_client->stream[j].stream_info->dim.height))) {
          result = TRUE;
          break;
        }
      }
    }
    if (result)
      break;
  }
  return result;
}

/**
 * Function: module_qdc_common_client_destroy
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
int32_t module_qdc_common_client_destroy(imgbase_client_t *p_client)
{
  IDBG_MED(":E");
  if (p_client->p_private_data) {
    free(p_client->p_private_data);
    p_client->p_private_data = NULL;
  }
  IDBG_MED(": X");
  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_client_created
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
int32_t module_qdc_common_client_created(imgbase_client_t *p_client)
{
  module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;
  img_qdc_common_module_t *p_qdcc_mod = (img_qdc_common_module_t *)p_mod->mod_private;
  img_qdc_common_client_t *p_qdcc_client;

  IDBG_MED(":E");
  /* Create QDC_Common private client and set default params */
  p_qdcc_client = calloc(1, sizeof(img_qdc_common_client_t));
  if (!p_qdcc_client) {
    IDBG_ERROR("Failed, QDC_COMMON client is NULL");
    return IMG_ERR_NO_MEMORY;
  }

  p_qdcc_client->p_session_data =
    &p_qdcc_mod->session_data[p_client->session_id - 1];
  p_client->p_private_data = p_qdcc_client;
  p_qdcc_client->p_client = p_client;

  /* Set default params for imglib base client */
  p_client->before_cpp = p_mod->caps.before_cpp;
  p_client->feature_mask = p_mod->feature_mask;
  p_client->processing_disabled = FALSE;
  p_client->process_all_frames = TRUE;
  p_client->is_dim_change = p_mod->caps.will_dim_change;

  /* Update streams_to_process mask in the client */
  p_client->streams_to_process = p_mod->modparams.streams_to_process;
  IDBG_MED("Streams to process %d", p_client->streams_to_process);
  IDBG_MED("Does algo change dimension %d", p_client->is_dim_change);
  IDBG_MED("Is algo befor CPP %d", p_mod->caps.before_cpp);
  IDBG_MED("Session id %d", p_client->session_id);
  IDBG_MED(":X");
  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_client_streamon
 *
 * Description: function called after on streamon
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
int32_t module_qdc_common_client_streamon(imgbase_client_t *p_client)
{
  int rc = IMG_SUCCESS;
  img_core_ops_t *p_core_ops;
  module_imgbase_t *p_mod;
  img_component_ops_t *p_comp;
  imgbase_client_t *p_peer_client;
  imgbase_session_data_t *p_base_sess_data;
  bool is_master = FALSE;

  IDBG_MED(":E");
  p_mod = (module_imgbase_t *)p_client->p_mod;
  if (NULL == p_mod) {
    IDBG_ERROR("Imgbase module NULL");
    return IMG_ERR_INVALID_INPUT;
  }

  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    IDBG_ERROR("Imgbase session data is NULL");
    return IMG_ERR_INVALID_INPUT;
  }
  p_core_ops = &p_mod->core_ops;
  p_comp = &p_client->comp;
  p_peer_client = (imgbase_client_t *)p_client->p_peer_client;


  /* Bind the camera sessions */
  if (p_base_sess_data->multi_cam_info.is_linked) {
    if (!p_client->is_binded && p_client->p_peer_comp &&
      p_peer_client->stream_on) {
      if (p_core_ops->bind) {
        rc = IMG_COMP_BIND(p_core_ops, p_comp, p_client->p_peer_comp);
        if (IMG_ERROR(rc)) {
          IDBG_ERROR("%s:%d] Bind failed %d", __func__, __LINE__, rc);
          rc = IMG_ERR_GENERAL;
        } else {
          IDBG_MED("%s:%d] Bind comp %p peer_comp %p", p_comp,
            p_client->p_peer_comp);
          p_client->is_binded = TRUE;
          p_peer_client->is_binded = TRUE;
        }
      }
    }
    /* Set the master on the component */
    pthread_mutex_lock(&p_base_sess_data->q_mutex);
    is_master = (p_base_sess_data->multi_cam_info.cam_mode == CAM_MODE_PRIMARY)? 1:0;
    pthread_mutex_unlock(&p_base_sess_data->q_mutex);
    IDBG_MED("Session id %d, role %d is master %d", p_client->session_id,
      p_base_sess_data->multi_cam_info.cam_role, is_master);
    rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_IS_MASTER, (void *)(&is_master));
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("Set QIMG_PARAM_IS_MASTER failed rc %d", rc);
    }
  }
  IDBG_MED(":X");
  return rc;
}

/**
 * Function: module_qdc_common_link_peer
 *
 * Description: This function links peer clients on related
 *  sessions`
 *
 * Arguments:
 *   @p_client: pointer to imgbase client
 *   @identity: Idenity on which event was sent
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_link_peer(imgbase_client_t *p_client,
   uint32_t identity)
{
  module_imgbase_t *p_mod;
  mct_list_t *p_peer_list = NULL;

  if (!p_client) {
    IDBG_ERROR("Invalid input");
    return IMG_ERR_INVALID_INPUT;
  }
  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_peer_list = mct_list_find_custom(p_mod->imgbase_client, (void *)p_client,
    module_qdc_common_find_peer_clients);
  if (!p_peer_list) {
    IDBG_ERROR("Cannot find match peer client..");
    return IMG_ERR_GENERAL;
  }
  p_client->p_peer_client = p_peer_list->data;
  p_client->p_peer_comp = &((imgbase_client_t*)p_client->p_peer_client)->comp;
  ((imgbase_client_t*)p_client->p_peer_client)->p_peer_client = p_client;
  ((imgbase_client_t*)p_client->p_peer_client)->p_peer_comp = &p_client->comp;
  IDBG_MED("Session id %d, p_client %p linked to session id %d, p_client %p",
    p_client->session_id, p_client,
    ((imgbase_client_t*)p_client->p_peer_client)->session_id,
    (imgbase_client_t*)p_client->p_peer_client);
  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_update_all_clients
 *
 * Description: Callback function provided in mct_list_traverse used
 *  for updating multiple clients in the same session
 *
 * Arguments:
 *   @p_data: pointer to client structure.
 *   @p_user: Pointer to multi session flag.
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: This function holds client mutex.
 **/
static boolean module_qdc_common_update_all_clients(void *p_data, void *p_user)
{
  imgbase_session_data_t *p_base_sess_data;
  imgbase_multi_camera_info_t *multicam_info;
  module_imgbase_t *p_mod;
  img_qdc_common_multi_client_data_t *p_client_data =
    (img_qdc_common_multi_client_data_t *)p_user;
  imgbase_client_t *p_client = (imgbase_client_t *)p_data;
  int rc = IMG_SUCCESS;

  if (!p_client_data || !p_client) {
    return FALSE;
  }

  /* Session client need not be updated*/
  if (p_client->session_client) {
    return TRUE;
  }
  img_component_ops_t *p_comp = &p_client->comp;

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    IDBG_ERROR("Imgbase session data is NULL");
    return FALSE;
  }
  multicam_info = &p_base_sess_data->multi_cam_info;

  pthread_mutex_lock(&p_client->mutex);
  if (p_client_data->session_id == p_client->session_id) {
    switch (p_client_data->qdc_cmd) {
    case IMG_QDC_COMMON_ROLE_SWITCH: {
      bool *is_master = (bool *)p_client_data->data;
      IDBG_MED("Role switch. Setting comp of session %d as master",
        p_client->session_id);
      rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_IS_MASTER,
        (void *)(is_master));
      if (IMG_ERROR(rc)) {
       IDBG_ERROR("Set QIMG_PARAM_IS_MASTER rc %d", rc);
      }
      break;
    }
    case IMG_QDC_COMMON_LPM: {
      cam_dual_camera_perf_control_t *dual_cam_lpm =
        (cam_dual_camera_perf_control_t *)p_client_data->data;
      int8_t lpm_enable = dual_cam_lpm->enable;
      /* Check if lpm mode has changed */
      pthread_mutex_lock(&p_base_sess_data->q_mutex);
      if (multicam_info->low_power_mode == lpm_enable) {
        pthread_mutex_unlock(&p_base_sess_data->q_mutex);
        break;
      }
      pthread_mutex_unlock(&p_base_sess_data->q_mutex);
      if (lpm_enable) {
        switch(dual_cam_lpm->perf_mode) {
        case CAM_PERF_STATS_FPS_CONTROL:
        default: {
          IDBG_ERROR("LPM Enabled on session %d, Stop component",
            p_client->session_id);
          p_client->processing_disabled = TRUE;
          IDBG_ERROR("Calling Abort", p_client->session_id);
          rc = IMG_COMP_ABORT(p_comp, NULL);
          if (IMG_ERROR(rc))
            IDBG_ERROR("Abort failed on LPM enable %d on sessionid %d",
              rc, p_client->session_id);
          break;
          }
        }
      } else {
        switch(dual_cam_lpm->perf_mode) {
        case CAM_PERF_STATS_FPS_CONTROL:
        default:
          IDBG_ERROR("LPM Disabled on session %d, Start component",
            p_client->session_id);
          p_client->processing_disabled = FALSE;
          rc = IMG_COMP_START(p_comp, NULL);
          if (IMG_ERROR(rc)) {
            IDBG_ERROR("Comp start failed on LPM enable on session id %d",
              p_client->session_id );
          }
          break;
        }
      }
      break;
    }
    default:
      break;
    }
  }
  pthread_mutex_unlock(&p_client->mutex);
  return TRUE;
}

/**
 * Function: module_qdc_common_post_qdc_cmd
 *
 * Description: Helper function to post QDC CMD
 *
 * Arguments:
 *  p_client - pointer to
 *   imgbase client
 *  qdc_cmd - QDC Command
 *  p_data - Data to be posted
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
int module_qdc_common_post_qdc_cmd(imgbase_client_t *p_client,
  img_qdc_common_client_cmd_t qdc_cmd, void *p_data)
{
  module_imgbase_t *p_mod;
  img_qdc_common_multi_client_data_t client_data;
  int rc = IMG_SUCCESS;

  p_mod = (module_imgbase_t *)p_client->p_mod;
  memset(&client_data, 0, sizeof(img_qdc_common_multi_client_data_t));

  client_data.session_id = p_client->session_id;
  client_data.qdc_cmd = qdc_cmd;
  client_data.data = p_data;
  mct_list_traverse(p_mod->imgbase_client,
    module_qdc_common_update_all_clients, &client_data);

  return rc;
}

/**
 * Function: module_qdc_common_handle_ctrl_event
 *
 * Description: This function handles ctrl events
 *  that are overriden in the QDC_Common module
 *
 * Arguments:
 *  p_ctrl_event - pointer of ctrl event
 *  p_client - pointer to
 *   imgbase client
 *  p_core_ops - pointer to imgbase module ops
 *  is_evt_handled - flag indicating if base module handling is
 *  required
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_qdc_common_handle_ctrl_event(mct_event_control_t *p_ctrl_event,
  img_core_ops_t *p_core_ops, imgbase_client_t *p_client,
  boolean *is_evt_handled)
{
  module_imgbase_t *p_mod;
  imgbase_session_data_t *p_base_session_data;
  imgbase_multi_camera_info_t multicam_info;
  imgbase_multi_camera_info_t *p_multicam_info = &multicam_info;
  img_qdc_common_multi_client_data_t client_data;
  boolean fwd_event = TRUE;

  if (!p_ctrl_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;

  switch (p_ctrl_event->type) {
  case MCT_EVENT_CONTROL_HW_WAKEUP:
  case MCT_EVENT_CONTROL_HW_SLEEP: {
    IDBG_ERROR("MCT_EVENT_CONTROL_HW_SLEEP/MCT_EVENT_CONTROL_HW_WAKEUP");
    cam_dual_camera_perf_control_t *dual_cam_lpm =
      (cam_dual_camera_perf_control_t*)p_ctrl_event->control_event_data;
    if(dual_cam_lpm == NULL) {
        IDBG_ERROR("Invalid input for Dual Cam LPM Info");
        break;
    }
    IDBG_ERROR("MCT_EVENT_CONTROL_HW_SLEEP/MCT_EVENT_CONTROL_HW_WAKEUP lpm %d",
      dual_cam_lpm->enable);
    memset(&client_data, 0, sizeof(img_qdc_common_multi_client_data_t));
    client_data.session_id = p_client->session_id;
    client_data.qdc_cmd = IMG_QDC_COMMON_LPM;
    client_data.data = (void *)dual_cam_lpm;
    mct_list_traverse(p_mod->imgbase_client,
      module_qdc_common_update_all_clients, &client_data);
    *is_evt_handled = FALSE;

    break;
  }
  case MCT_EVENT_CONTROL_MASTER_INFO: {
    cam_dual_camera_master_info_t *dual_master_info =
      (cam_dual_camera_master_info_t *)p_ctrl_event->control_event_data;
    bool is_master = FALSE;
    if(dual_master_info == NULL) {
      IDBG_ERROR("Invalid input for DUAL CAM MASTER Info");
      break;
    }

    IMG_GET_CAM_DATA(p_client, p_base_session_data, p_multicam_info);
    if (p_multicam_info->cam_role == dual_master_info->mode) {
      IDBG_MED("No Change in master. Not updating");
      break;
    }
    is_master = (dual_master_info->mode == CAM_MODE_PRIMARY)? 1: 0;

    pthread_mutex_lock(&p_base_session_data->q_mutex);
    p_multicam_info->cam_mode = dual_master_info->mode;
    pthread_mutex_unlock(&p_base_session_data->q_mutex);

    module_qdc_common_post_qdc_cmd(p_client, IMG_QDC_COMMON_ROLE_SWITCH,
      (void*)&is_master);
    IDBG_HIGH("Master role switch event for session %d, cam role %d, is_master %d",
      p_client->session_id, p_multicam_info->cam_role, is_master);

    /* Update the peer client master to avoid syncronization issues */
    if (p_client->p_peer_client) {
      imgbase_client_t *p_peer_client = (imgbase_client_t *)p_client->p_peer_client;
      IMG_GET_CAM_DATA(p_client, p_base_session_data, p_multicam_info);

      pthread_mutex_lock(&p_base_session_data->q_mutex);
      p_multicam_info->cam_mode = dual_master_info->mode;
      pthread_mutex_unlock(&p_base_session_data->q_mutex);

      is_master = !is_master;
      module_qdc_common_post_qdc_cmd(p_peer_client,
        IMG_QDC_COMMON_ROLE_SWITCH, (void*)&is_master);
      IDBG_HIGH("Master role updated for session %d, cam role %d, is_master %d",
        p_peer_client->session_id, is_master);
    }
    *is_evt_handled = TRUE;
    break;
  }
  default:
    break;
  }

  return fwd_event;
}

/**
 * Function: module_qdc_common_handle_module_event
 *
 * Description: This function handles module events
 *  that are overriden in the QDC_Common module
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
 boolean module_qdc_common_handle_module_event(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  module_imgbase_t *p_mod;
  img_qdc_common_session_data_t *p_session_data;
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_module_t *p_qdcc_mod;
  boolean fwd_event = TRUE;

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_mod = (img_qdc_common_module_t *)p_mod->mod_private;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = p_qdcc_client->p_session_data;

  switch (p_mod_event->type) {
  case MCT_EVENT_MODULE_STATS_AF_UPDATE: {
    stats_update_t *stats_update =
      (stats_update_t *)(p_mod_event->module_event_data);
    IDBG_MED("MCT_EVENT_MODULE_STATS_AF_UPDATE af_status %d",
      stats_update->af_update.af_state);
    switch (p_mod->feature_mask) {
    case CAM_QTI_FEATURE_SAT:
      p_qdcc_client->qdc_cfg.sat_cfg.af_state =
        stats_update->af_update.af_state;
      break;
    case CAM_QTI_FEATURE_RTB:
      p_qdcc_client->qdc_cfg.rtb_cfg.af_state =
        stats_update->af_update.af_state;
      break;
    case CAM_QTI_FEATURE_SAC:
      p_qdcc_client->qdc_cfg.sac_cfg.af_state =
        stats_update->af_update.af_state;
      break;
    case CAM_QTI_FEATURE_RTBDM:
      p_qdcc_client->qdc_cfg.rtbdm_cfg.af_state =
        stats_update->af_update.af_state;
      break;
    default:
      IDBG_ERROR("Invalid feature mask");
      return fwd_event;
    }
    break;
  }
  case MCT_EVENT_MODULE_STATS_AEC_UPDATE: {
    module_imglib_common_meta_set_aec(&p_client->meta_data_list,
      p_mod_event->module_event_data, p_mod_event->current_frame_id);
    break;
  }
  case MCT_EVENT_MODULE_ISP_AWB_UPDATE: {
    module_imglib_common_meta_set_awb(&p_client->meta_data_list,
    p_mod_event->module_event_data, p_mod_event->current_frame_id);
    break;
  }
  case MCT_EVENT_MODULE_IMGLIB_AF_CONFIG: {
    mct_imglib_af_config_t *p_af_cfg;
    mct_imglib_af_focus_config_t *p_af_tuning;
    p_af_cfg = (mct_imglib_af_config_t *)p_mod_event->module_event_data;
    if (NULL == p_af_cfg || p_af_cfg->type != AF_CFG_FOCUS) {
      return fwd_event;
    }
    p_af_tuning = &(p_af_cfg->u.affocus);
    if (NULL == p_af_tuning) {
      IDBG_ERROR("Error AF Tuning CFG is NULL");
      return fwd_event;
    }
    p_session_data->af_tuning_cfg = *p_af_tuning;
    break;
  }
  case MCT_EVENT_MODULE_SET_DUAL_OTP_PTR : {
    cam_related_system_calibration_data_t *p_calib_data =
      (cam_related_system_calibration_data_t *)p_mod_event->module_event_data;
    IDBG_MED("%s:%d] MCT_EVENT_MODULE_SET_DUAL_OTP_PTR ", __func__, __LINE__);
    if (NULL == p_calib_data) {
      IDBG_ERROR("%s %d: calib_data is null", __func__, __LINE__);
      break;
    }
    memcpy(&p_qdcc_mod->system_calib_data, p_calib_data,
      sizeof(cam_related_system_calibration_data_t));
    break;
  }
  default:
    break;
  }
  return fwd_event;
}

/**
 * Function: module_qdc_common_handle_ctrl_parm
 *
 * Description: This function handles the ctrl parms
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
boolean module_qdc_common_handle_ctrl_parm(mct_event_control_parm_t
  *p_ctrl_parm, imgbase_client_t *p_client, img_core_ops_t *p_core_ops)
{
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_session_data_t *p_session_data;

  int rc = IMG_SUCCESS;
  boolean fwd_event = TRUE;

  if (!p_ctrl_parm || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = p_qdcc_client->p_session_data;

  switch (p_ctrl_parm->type) {
  case CAM_INTF_PARM_ZOOM:
    p_session_data->zoom_level = *((int32_t *)p_ctrl_parm->parm_data);
    IDBG_MED("CAM_INTF_PARM_ZOOM %d", p_session_data->zoom_level);
    break;
  default:
    break;
  }
  return rc;
}

/**
 * Function: module_qdc_common_post_lpm_message
 *
 * Description: Post LPM enable/disable to MCT if needed
 *
 * Arguments:
 *   @p_client - pointer to IMGLIB_BASE client
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_INVALID_INPUT
 *   IMG_ERR_EAGAIN
 *
 * Notes: none
 **/
int module_qdc_common_post_lpm_message(imgbase_client_t *p_client,
  img_lpm_cmd_t *p_lpm_cmd)
{
  int rc = IMG_SUCCESS;
  module_imgbase_t *p_mod;
  imgbase_session_data_t *p_base_sess_data;
  imgbase_session_data_t *p_peer_base_sess_data;
  imgbase_multi_camera_info_t *multicam_info;
  imgbase_multi_camera_info_t peer_multicam_info;
  imgbase_multi_camera_info_t *p_peer_multicam_info = &peer_multicam_info;
  uint32_t lmp_session_id;
  img_camera_role_t img_cam_role;
  imgbase_client_t *p_peer_client;
  bool post_msg = FALSE;

  /* get Current session info*/
  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    IDBG_ERROR("Imgbase session data is NULL");
    return IMG_ERR_INVALID_INPUT;
  }
  multicam_info = &p_base_sess_data->multi_cam_info;
  img_cam_role =
    module_imglib_common_get_img_camera_role(multicam_info->cam_role);

  IDBG_MED("Current session %d lmp mode %d, lmp mode from SAT %d"
    "for camera role %d", p_client->session_id,
    multicam_info->low_power_mode, p_lpm_cmd->low_power_mode,
    p_lpm_cmd->low_power_mode);

  /* Get the right sessionid to post message on */
  switch (multicam_info->perf_mode) {
  case CAM_PERF_STATS_FPS_CONTROL:
  default:
    if ((p_lpm_cmd->low_power_mode) &&
      (multicam_info->low_power_mode != p_lpm_cmd->low_power_mode)) {
      lmp_session_id = p_client->session_id;
      post_msg = TRUE;
    } else {
      /* When a session is in LPM in this mode, there is no result meta.
      LPM off will be passed on the active session */
      p_peer_client = (imgbase_client_t *)p_client->p_peer_client;
      if (!p_peer_client) {
        IDBG_ERROR("Peer client is NULL");
        return IMG_SUCCESS;
      }

      /* Get peer client info */
      IMG_GET_CAM_DATA(p_peer_client, p_peer_base_sess_data, p_peer_multicam_info);
      img_cam_role =
       module_imglib_common_get_img_camera_role(p_peer_multicam_info->cam_role);
      if ((img_cam_role == p_lpm_cmd->camera_role) &&
        (p_peer_multicam_info->low_power_mode != p_lpm_cmd->low_power_mode)){
        lmp_session_id = p_peer_client->session_id;
        post_msg = TRUE;
      }
    }
    break;
  }

  if (post_msg) {
    /* Post Bus message */
    mct_bus_msg_ctrl_request_frame_t req_frame_msg;
    if (p_lpm_cmd->low_power_mode == TRUE) {
      IDBG_HIGH("QDC requesting SLEEP on Session Id %d, camera role %d lpm %d",
        lmp_session_id, multicam_info->cam_role, p_lpm_cmd->low_power_mode);
      req_frame_msg.request_flag = FALSE;
      req_frame_msg.req_mode = FRM_REQ_MODE_NONE;
    } else {
      IDBG_HIGH("QDC requesting WAKEUP on Session Id %d, camera role %d lpm %d",
        lmp_session_id, multicam_info->cam_role, p_lpm_cmd->low_power_mode);
      req_frame_msg.request_flag = TRUE;
      req_frame_msg.req_mode = FRM_REQ_MODE_CONTINUOUS;
    }
    module_imglib_post_bus_msg(p_mod->parent_mod, lmp_session_id,
      MCT_BUS_MSG_CONTROL_REQUEST_FRAME, &req_frame_msg,
      sizeof(mct_bus_msg_ctrl_request_frame_t));
  }
  return rc;
}

/**
 * Function: module_qdc_common_sat_handle_meta_buf_done
 *
 * Description: Function to handle meta buf done for SAT feature
 *
 *
 * Arguments:
 *   p_client - pointer to imgbase client
 *   p_meta - pointer to metadata
 *   p_mod - pointer to base module
 *   p_qdc_client - pointer to qdc common client
 *   p_session_client - pointer to session stream client
 *   p_stream - pointer to session stream
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
int module_qdc_common_sat_handle_meta_buf_done(imgbase_client_t *p_client,
  img_meta_t *p_meta, module_imgbase_t *p_mod)
{
  int rc = IMG_ERR_EAGAIN;
  uint32_t i;
  img_qdc_common_client_t *p_qdc_client = NULL;
  img_qdc_common_session_data_t *p_session_data = NULL;
  img_spatial_transform_result_t *p_sat_result = NULL;
  imgbase_stream_t *p_stream = NULL;
  cam_sac_output_info_t l_output_info;
  cam_dual_camera_role_t cam_master_role = CAM_ROLE_DEFAULT;
  cam_dual_camera_role_t cam_recommended_role;
  mct_bus_msg_stream_crop_t l_stream_crop;
  imgbase_session_data_t *p_base_sess_data;
  img_camera_role_t cam_role;

  p_qdc_client = (img_qdc_common_client_t *)p_client->p_private_data;

  if ((p_qdc_client == NULL) || (p_qdc_client->p_session_data == NULL)) {
    IDBG_ERROR("client %p ", p_qdc_client);
    goto end;
  }

  p_session_data =
    (img_qdc_common_session_data_t *)p_qdc_client->p_session_data;

  /* Check if the result has been updated indicating a framedrop */
  if ((p_qdc_client->qdc_result.sat_result.frameid == p_meta->frame_id) &&
    (p_qdc_client->qdc_result.sat_result.chosen_frame ==
    IMG_CAM_ROLE_INVALID)) {
    IDBG_MED("Frameid %d dropped. No result to handle",
      p_meta->frame_id);
    goto end;
  }

  p_sat_result = (img_spatial_transform_result_t *)img_get_meta(p_meta,
    IMG_META_SPATIAL_TRANSFORM_RESULT);
  if (!p_sat_result) {
    IDBG_ERROR("QDC meta result is null");
    goto end;
  }

  /* Save SAT result */
  p_qdc_client->qdc_result.sat_result.frameid = p_meta->frame_id;

  memset(&l_output_info, 0, sizeof(cam_sac_output_info_t));
  if (p_sat_result->is_camera_role_valid) {
    p_qdc_client->qdc_result.sat_result.chosen_frame =
      p_sat_result->camera_role;
    cam_master_role =
    module_imglib_common_get_cam_role(p_sat_result->camera_role);
    l_output_info.is_master_preview_valid = 1;
    l_output_info.master_preview = cam_master_role;
    l_output_info.is_master_3A_valid = 1;
    l_output_info.master_3A = cam_master_role;
  }

  if (p_sat_result->is_recommended_role_valid) {
    cam_recommended_role =
    module_imglib_common_get_cam_role(p_sat_result->recommended_role);
    l_output_info.is_master_hint_valid = 1;
    l_output_info.master_hint = cam_recommended_role;
  }

  if (p_sat_result->is_output_shift_valid) {
    l_output_info.is_output_shift_valid = 1;
    l_output_info.output_shift.shift_horz = p_sat_result->output_shift.x;
    l_output_info.output_shift.shift_vert = p_sat_result->output_shift.y;
    l_output_info.reference_res_for_output_shift.width =
      p_sat_result->ref_res_output_shift.width;
    l_output_info.reference_res_for_output_shift.height =
      p_sat_result->ref_res_output_shift.height;
  }

  if (p_sat_result->is_focus_roi_shift_valid) {
    l_output_info.is_focus_roi_shift_valid = 1;
    l_output_info.focus_roi_shift.shift_horz = p_sat_result->focus_roi_shift.x;
    l_output_info.focus_roi_shift.shift_vert = p_sat_result->focus_roi_shift.y;
    l_output_info.reference_res_for_focus_roi_shift.width =
      p_sat_result->ref_res_focus_roi_shift.width;
    l_output_info.reference_res_for_focus_roi_shift.height =
      p_sat_result->ref_res_focus_roi_shift.height;
  }

  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    /* Error printed in the macro*/
    return IMG_ERR_INVALID_INPUT;
  }

  cam_role =
    module_imglib_common_get_img_camera_role(
      p_base_sess_data->multi_cam_info.cam_role);

  if ((cam_master_role != CAM_ROLE_DEFAULT) && (cam_master_role == cam_role)) {
    for (i = 0; i < p_client->stream_cnt; i++) {
      p_stream = &p_client->stream[i];
      /* if crop requested by SAT algo send crop event downstream*/
      if (p_sat_result->is_output_crop_request_valid &&
        p_client->stream_crop_valid) {
        l_stream_crop = p_client->stream_crop;
        IDBG_MED("Crop before, x %d, y %d, w %d, h %d",
          l_stream_crop.x, l_stream_crop.y,
          l_stream_crop.crop_out_x,
          l_stream_crop.crop_out_y);
          l_stream_crop.x = p_sat_result->output_crop.pos.x;
          l_stream_crop.y = p_sat_result->output_crop.pos.y;
          l_stream_crop.crop_out_x = p_sat_result->output_crop.size.width;
          l_stream_crop.crop_out_y = p_sat_result->output_crop.size.height;
        IDBG_MED("Crop after, x %d, y %d, w %d, h %d",
          l_stream_crop.x, l_stream_crop.y,
          l_stream_crop.crop_out_x,
          l_stream_crop.crop_out_y);
        IDBG_MED("Sending event on identity %x, stream type %p",
          p_stream->identity, p_stream->isp_stream_info.stream_type);
        rc = mod_imgbase_send_event(p_stream->identity, FALSE,
          MCT_EVENT_MODULE_STREAM_CROP, l_stream_crop);
        if (!rc) {
          IDBG_ERROR("Send crop event failed");
          return IMG_ERR_EAGAIN;
        }
      }
    }
  }

  /* post output info to MCT Bus */
  module_imglib_post_bus_msg(p_mod->parent_mod, p_client->session_id,
    MCT_BUS_MSG_DC_SAC_OUTPUT_INFO, &l_output_info,
    sizeof(cam_sac_output_info_t));

  /* Send LPM enable/disable */
  if (p_sat_result->is_lpm_cmd_valid) {
    module_qdc_common_post_lpm_message(p_client, &p_sat_result->lpm_cmd);
  }

  end:
  return rc;
}

/**
 * Function: module_qdc_common_sac_handle_meta_buf_done
 *
 * Description: Function to handle meta buf done for SAC feature
 *
 *
 * Arguments:
 *   p_client - pointer to imgbase client
 *   p_meta - pointer to metadata
 *   p_mod - pointer to base module
 *   p_qdc_client - pointer to qdc common client
 *   p_session_client - pointer to session stream client
 *   p_stream - pointer to session stream
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
int module_qdc_common_sac_handle_meta_buf_done(imgbase_client_t *p_client,
  img_meta_t *p_meta, module_imgbase_t *p_mod)
{
  int rc = IMG_ERR_EAGAIN;

  img_qdc_common_client_t *p_qdc_client = NULL;
  img_qdc_common_session_data_t *p_session_data = NULL;
  img_spatial_align_compute_result_t *p_sac_result = NULL;
  mct_list_t *p_client_list = NULL;
  imgbase_client_t *p_session_client =NULL;
  imgbase_stream_t *p_stream = NULL;

  cam_dual_camera_role_t cam_master_role, cam_recommended_role;
  cam_sac_output_info_t l_output_info;

  p_qdc_client = (img_qdc_common_client_t *)p_client->p_private_data;

  if ((p_qdc_client == NULL) || (p_qdc_client->p_session_data == NULL)) {
    IDBG_ERROR("client %p", p_qdc_client);
    goto end;
  }

  p_session_data =
    (img_qdc_common_session_data_t *)p_qdc_client->p_session_data;

  p_sac_result = (img_spatial_align_compute_result_t *)img_get_meta(p_meta,
    IMG_META_SPATIAL_ALIGN_COMPUTE_RESULT);
  if (!p_sac_result) {
    IDBG_ERROR("QDC meta result is null");
    goto end;
  }

  p_client_list = mct_list_find_custom(p_mod->imgbase_client,
    &p_client->session_id, module_imgbase_find_session_str_client);
  if (!p_client_list) {
    IDBG_ERROR("Cannot find session str client. Cannot send QDC"
      "output to CPP");
    goto end;
  }

  p_session_client = (imgbase_client_t *)p_client_list->data;
  p_stream = &p_session_client->stream[0];
  if (NULL == p_stream) {
    IDBG_ERROR("Cannot find stream");
    goto end;
  }

  /* Save SAT result */
  p_qdc_client->qdc_result.sac_result.frameid = p_meta->frame_id;

  memset(&l_output_info, 0, sizeof(cam_sac_output_info_t));
  if (p_sac_result->is_camera_role_valid) {
    p_qdc_client->qdc_result.sac_result.chosen_frame =
      p_sac_result->camera_role;
    cam_master_role =
    module_imglib_common_get_cam_role(p_sac_result->camera_role);
    l_output_info.is_master_preview_valid = 1;
    l_output_info.master_preview = cam_master_role;
    l_output_info.is_master_3A_valid = 1;
    l_output_info.master_3A = cam_master_role;
  }

  if (p_sac_result->is_recommended_role_valid) {
    cam_recommended_role =
    module_imglib_common_get_cam_role(p_sac_result->recommended_role);
    l_output_info.is_master_hint_valid = 1;
    l_output_info.master_hint = cam_recommended_role;
  }

  if (p_sac_result->is_output_shift_valid) {
    l_output_info.is_output_shift_valid = 1;
    l_output_info.output_shift.shift_horz = p_sac_result->output_shift.x;
    l_output_info.output_shift.shift_vert = p_sac_result->output_shift.y;
    l_output_info.reference_res_for_output_shift.width =
      p_sac_result->ref_res_output_shift.width;
    l_output_info.reference_res_for_output_shift.height =
      p_sac_result->ref_res_output_shift.height;

    rc = mod_imgbase_send_event(p_stream->identity, FALSE,
      MCT_EVENT_MODULE_DUALCAM_SHIFT_OFFSET, l_output_info);
    if (!rc) {
      IDBG_ERROR("Send shift event failed");
      return IMG_ERR_EAGAIN;
    }
  }

  if (p_sac_result->is_focus_roi_shift_valid) {
    l_output_info.is_focus_roi_shift_valid = 1;
    l_output_info.focus_roi_shift.shift_horz = p_sac_result->focus_roi_shift.x;
    l_output_info.focus_roi_shift.shift_vert = p_sac_result->focus_roi_shift.y;
    l_output_info.reference_res_for_focus_roi_shift.width =
      p_sac_result->ref_res_focus_roi_shift.width;
    l_output_info.reference_res_for_focus_roi_shift.height =
      p_sac_result->ref_res_focus_roi_shift.height;
  }

  if (p_sac_result->is_alignmet_ready_flag_valid) {
    l_output_info.is_ready_status_valid = 1;
    l_output_info.ready_status = p_sac_result->alignmet_ready_flag;
  }
  /* post output info to MCT Bus */
  module_imglib_post_bus_msg(p_mod->parent_mod, p_client->session_id,
    MCT_BUS_MSG_DC_SAC_OUTPUT_INFO, &l_output_info,
    sizeof(cam_sac_output_info_t));


  /* Send LPM enable/disable */
  if (p_sac_result->is_lpm_cmd_valid) {
    module_qdc_common_post_lpm_message(p_client, &p_sac_result->lpm_cmd);
  }

  end:
  return rc;
}

/**
 * Function: module_qdc_common_rtb_handle_meta_buf_done
 *
 * Description: Function to handle meta buf done for RTB feature
 *
 *
 * Arguments:
 *   p_client - pointer to imgbase client
 *   p_meta - pointer to metadata
 *   p_mod - pointer to base module
 *   p_qdc_client - pointer to qdc common client
 *   p_session_client - pointer to session stream client
 *   p_stream - pointer to session stream
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
int module_qdc_common_rtb_handle_meta_buf_done(imgbase_client_t *p_client,
  img_meta_t *p_meta, module_imgbase_t *p_mod)
{
  IMG_UNUSED(p_client);
  IMG_UNUSED(p_mod);
  int rc = IMG_ERR_EAGAIN;
  img_qdc_common_client_t *p_qdc_client = NULL;
  img_realtime_bokeh_result_t *p_rtb_result = NULL;


  p_qdc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  if (!p_qdc_client) {
    IDBG_ERROR("null client %p ", p_qdc_client);
    goto end;
  }

  /* Check if the result has been updated indicating a framedrop */
  if ((p_qdc_client->qdc_result.rtb_result.frameid == p_meta->frame_id) &&
    (p_qdc_client->qdc_result.rtb_result.chosen_frame ==
    IMG_CAM_ROLE_INVALID)) {
    IDBG_MED("Frameid %d dropped. No result to handle",
      p_meta->frame_id);
    goto end;
  }

  p_rtb_result = (img_realtime_bokeh_result_t *)img_get_meta(p_meta,
    IMG_META_BOKEH_RESULT);
  if (!p_rtb_result) {
    IDBG_ERROR("QDC meta result is null");
    goto end;
  }

  /* Save RTB result */
  p_qdc_client->qdc_result.rtb_result.frameid = p_meta->frame_id;

  if (p_rtb_result->is_camera_role_valid) {
    p_qdc_client->qdc_result.sac_result.chosen_frame =
      p_rtb_result->camera_role;
  }

  end:
  return rc;
}

/**
 * Function: module_qdc_common_rtbdm_handle_meta_buf_done
 *
 * Description: Function to handle meta buf done for RTBDM
 * feature
 *
 *
 * Arguments:
 *   p_client - pointer to imgbase client
 *   p_meta - pointer to metadata
 *   p_mod - pointer to base module
 *   p_qdc_client - pointer to qdc common client
 *   p_session_client - pointer to session stream client
 *   p_stream - pointer to session stream
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
int module_qdc_common_rtbdm_handle_meta_buf_done(imgbase_client_t *p_client,
  img_meta_t *p_meta, module_imgbase_t *p_mod)
{
  IMG_UNUSED(p_client);
  IMG_UNUSED(p_mod);
  int rc = IMG_ERR_EAGAIN;
  img_qdc_common_client_t *p_qdc_client = NULL;
  img_realtime_bokeh_depth_map_result_t *p_rtbdm_result = NULL;


  p_qdc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  if (!p_qdc_client) {
    IDBG_ERROR("null client %p ", p_qdc_client);
    goto end;
  }

  p_rtbdm_result = (img_realtime_bokeh_depth_map_result_t *)img_get_meta(
    p_meta, IMG_META_BOKEH_DEPTH_MAP_RESULT);
  if (!p_rtbdm_result) {
    IDBG_ERROR("QDC meta result is null");
    goto end;
  }

  /* Save RTB result */
  p_qdc_client->qdc_result.rtbdm_result.frameid = p_meta->frame_id;

  if (p_rtbdm_result->is_depthmap_ready_flag) {
    p_qdc_client->qdc_result.rtbdm_result.depth_map_ready =
      p_rtbdm_result->depth_map_ready_flag;
  }

  end:
  return rc;
}

/**
 * Function: module_qdc_common_handle_meta_buf_done
 *
 * Description:
 *
 *
 * Arguments:
 *   p_client - pointer to imgbase client
 *   p_event - pointer to event
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/

int module_qdc_common_handle_meta_buf_done(imgbase_client_t *p_client,
  img_event_t *p_event)
{
  int rc = IMG_ERR_EAGAIN;
  img_meta_t *p_meta = NULL;
  module_imgbase_t *p_mod = NULL;

  if ((NULL == p_event) || (NULL == p_client)) {
    IDBG_ERROR("Error Input");
    return IMG_ERR_INVALID_INPUT;
  }

  p_meta = p_event->d.p_meta;
  if (!p_meta) {
    IDBG_ERROR("QDC result pointer is null");
    goto end;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  if (!p_mod) {
    IDBG_ERROR("null base module");
    goto end;
  }

  switch (p_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    module_qdc_common_sat_handle_meta_buf_done(p_client, p_meta, p_mod);
    break;
  case CAM_QTI_FEATURE_SAC:
    module_qdc_common_sac_handle_meta_buf_done(p_client, p_meta, p_mod);
    break;
  case CAM_QTI_FEATURE_RTB:
    module_qdc_common_rtb_handle_meta_buf_done(p_client, p_meta, p_mod);
    break;
  case CAM_QTI_FEATURE_RTBDM:
    module_qdc_common_rtbdm_handle_meta_buf_done(p_client, p_meta, p_mod);
    break;
  }
end:
  return rc;
}


/**
 * Function: module_qdc_common_handle_set_stream_config
 *
 * Description: This function handles the set stream config
 * module event and saves the sensor output
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
boolean module_qdc_common_handle_set_stream_config(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  module_imgbase_t *p_mod;
  img_qdc_common_session_data_t *session_data;
  sensor_out_info_t *p_sensor_info;
  img_qdc_common_client_t *p_qdcc_client;

  boolean fwd_event = TRUE;
  IDBG_MED(":E");
  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_sensor_info = (sensor_out_info_t *)p_mod_event->module_event_data;
  if (!p_sensor_info) {
    IDBG_ERROR("Error input %p", p_sensor_info);
    return fwd_event;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  session_data = p_qdcc_client->p_session_data;

  IDBG_MED("[%s]MCT_EVENT_MODULE_SET_STREAM_CONFIG, w = %u, h = %u",
    p_mod->name, p_sensor_info->dim_output.width,
    p_sensor_info->dim_output.height);

  session_data->sensor_max_dim.width = p_sensor_info->dim_output.width;
  session_data->sensor_max_dim.height = p_sensor_info->dim_output.height;
  session_data->sensor_crop.first_line = p_sensor_info->request_crop.first_line;
  session_data->sensor_crop.last_line = p_sensor_info->request_crop.last_line;
  session_data->sensor_crop.first_pixel = p_sensor_info->request_crop.first_pixel;
  session_data->sensor_crop.last_pixel = p_sensor_info->request_crop.last_pixel;
  IDBG_MED(":X");
  return fwd_event;
}

/**
 * Function: module_qdc_common_fill_fov_params
 *
 * Description: This function is used to fill in the
 *   ISP and CAMIF crop/scale params
 *
 * Arguments:
 *   @p_client - pointer to imgbase client
 *   @p_meta: pointer to the image meta
 *
 * Return values:
 *     None
 *
 * Notes: none
 **/
void module_qdc_common_fill_fov_params(imgbase_client_t *p_client,
  img_fov_t *p_fov_cfg_arr)
{
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_session_data_t *p_session_data;
  IDBG_MED(":E");
  if (!p_client || !p_fov_cfg_arr) {
    IDBG_ERROR("Invalid input %p %p", p_client, p_fov_cfg_arr);
    return;
  }

  /* Get Session Data */
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = p_qdcc_client->p_session_data;

  module_imglib_common_fill_fov_params(
    &p_session_data->sensor_max_dim,
    &p_session_data->sensor_crop,
    p_client->stream_crop_valid,
    &p_client->stream_crop,
    p_client->isp_output_dim_stream_info_valid,
    &p_client->isp_output_dim_stream_info,
    NULL,
    p_fov_cfg_arr, 4);

  IDBG_MED(":X");
  return;
}

/**
 * Function: module_qdc_common_update_meta
 *
 * Description: This function is used to called when the base
 *   module updates the metadata
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
int32_t module_qdc_common_update_meta(imgbase_client_t *p_client,
  img_meta_t *p_meta)
{
  int rc = IMG_SUCCESS;
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_session_data_t *p_session_data;
  module_imgbase_t *p_base_mod;
  imgbase_multi_camera_info_t multicam_info;
  imgbase_multi_camera_info_t *p_multicam_info = &multicam_info;
  imgbase_session_data_t *p_base_session_data;

  img_dual_cam_common_cfg_t l_qdcc_cfg;
  img_camera_role_t *p_cam_role = NULL;
  img_fov_t *p_fov = NULL;
  void *p_cfg = NULL;
  uint32_t meta_type;

  int l_af_status;
  img_af_info_t l_af_info;
  float zoom_factor = 0;
  float user_zoom_factor = 0; // wide and tele specific zoom factor ;

  if (!p_client || !p_meta) {
    IDBG_ERROR("Invalid input %p %p", p_client, p_meta);
    rc = IMG_ERR_INVALID_INPUT;
    return rc;
  }

  p_base_mod = (module_imgbase_t *)p_client->p_mod;

  /* Get Session Data */
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = p_qdcc_client->p_session_data;
  IMG_GET_CAM_DATA(p_client, p_base_session_data, p_multicam_info);
  IDBG_MED("Role %d", p_multicam_info->cam_role)

  /* Set CFG meta */
  memset(&l_qdcc_cfg, 0, sizeof(img_dual_cam_common_cfg_t));

  switch (p_base_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    p_cam_role = &l_qdcc_cfg.sat_cfg.camera_role;
    p_fov = &l_qdcc_cfg.sat_cfg.fov_params[0];
    p_cfg = &l_qdcc_cfg.sat_cfg;
    meta_type = IMG_META_SPATIAL_TRANSFORM_CFG;
    l_af_status = p_qdcc_client->qdc_cfg.sat_cfg.af_state;
    /* Fill LPM mode for current session*/
    pthread_mutex_lock(&p_base_session_data->q_mutex);
    l_qdcc_cfg.sat_cfg.low_power_mode = p_multicam_info->low_power_mode;
    l_qdcc_cfg.sat_cfg.is_master =
      (p_multicam_info->cam_mode == CAM_MODE_PRIMARY)? 1:0;
    pthread_mutex_unlock(&p_base_session_data->q_mutex);
    break;
  case CAM_QTI_FEATURE_SAC:
    p_cam_role = &l_qdcc_cfg.sac_cfg.camera_role;
    p_fov = &l_qdcc_cfg.sac_cfg.fov_params[0];
    p_cfg = &l_qdcc_cfg.sac_cfg;
    meta_type = IMG_META_SPATIAL_ALIGN_COMPUTE_CFG;
    l_af_status = p_qdcc_client->qdc_cfg.sac_cfg.af_state;
    /* Fill LPM mode for current session*/
    pthread_mutex_lock(&p_base_session_data->q_mutex);
    l_qdcc_cfg.sac_cfg.low_power_mode = p_multicam_info->low_power_mode;
    l_qdcc_cfg.sac_cfg.is_master =
      (p_multicam_info->cam_mode == CAM_MODE_PRIMARY)? 1:0;
    pthread_mutex_unlock(&p_base_session_data->q_mutex);
    break;
  case CAM_QTI_FEATURE_RTB:
    p_cam_role = &l_qdcc_cfg.rtb_cfg.camera_role;
    p_fov = &l_qdcc_cfg.rtb_cfg.fov_params[0];
    p_cfg = &l_qdcc_cfg.rtb_cfg;
    pthread_mutex_lock(&p_base_session_data->q_mutex);
    l_qdcc_cfg.rtb_cfg.is_master =
      (p_multicam_info->cam_mode == CAM_MODE_PRIMARY)? 1:0;
    pthread_mutex_unlock(&p_base_session_data->q_mutex);
    meta_type = IMG_META_BOKEH_CFG;
    l_af_status = p_qdcc_client->qdc_cfg.rtb_cfg.af_state;
    break;
  case CAM_QTI_FEATURE_RTBDM:
    p_cam_role = &l_qdcc_cfg.rtbdm_cfg.camera_role;
    p_fov = &l_qdcc_cfg.rtbdm_cfg.fov_params[0];
    p_cfg = &l_qdcc_cfg.rtbdm_cfg;
    pthread_mutex_lock(&p_base_session_data->q_mutex);
    l_qdcc_cfg.rtbdm_cfg.is_master =
      (p_multicam_info->cam_mode == CAM_MODE_PRIMARY)? 1:0;
    pthread_mutex_unlock(&p_base_session_data->q_mutex);
    meta_type = IMG_META_BOKEH_DEPTH_MAP_CFG;
    l_af_status = p_qdcc_client->qdc_cfg.rtbdm_cfg.af_state;
    break;
  default:
    IDBG_ERROR("Invalid feature mask");
    return IMG_ERR_INVALID_INPUT;
  }

  if (p_cam_role) {
    /* Fill current camera role */
    *p_cam_role =
      module_imglib_common_get_img_camera_role(p_multicam_info->cam_role);
  }

  if (p_fov) {
    module_qdc_common_fill_fov_params(p_client, p_fov);
  }

  if (p_cfg) {
    rc = img_set_meta(p_meta, meta_type, p_cfg);
    if (rc != IMG_SUCCESS) {
      IDBG_ERROR("Error updating meta cfg %d rc %d", meta_type,
        rc);
      goto error;
    }
  }

  /* Set Zoom factor */
  zoom_factor = module_imgbase_get_zoom_ratio(p_base_mod->p_mct_mod,
    p_client->session_id, p_session_data->zoom_level);
  IDBG_ERROR("zoom_factor %f for id %d for zoom level %d",
    zoom_factor, p_client->session_id,
    p_session_data->zoom_level);
  rc = img_set_meta(p_meta, IMG_META_ZOOM_FACTOR, &zoom_factor);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("%s:%d] rc %d", __func__, __LINE__, rc);
    goto error;
  }

  /* Set wide and tele specific user zoom factor */
  user_zoom_factor = module_imgbase_get_zoom_ratio(p_base_mod->p_mct_mod,
    p_client->session_id, p_session_data->user_zoom_level);
  IDBG_MED("User zoom_factor %f for id %d for user zoom level %d",
    user_zoom_factor, p_client->session_id,
    p_session_data->user_zoom_level);
  rc = img_set_meta(p_meta, IMG_META_USER_ZOOM_FACTOR, &user_zoom_factor);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("%s:%d] rc %d", __func__, __LINE__, rc);
    goto error;
  }


  /* Set AF meta */
  l_af_info.object_distance = p_session_data->af_tuning_cfg.object_distance_cm;
  l_af_info.lens_shift = p_session_data->af_tuning_cfg.lens_shift_um;
  l_af_info.near_field = p_session_data->af_tuning_cfg.near_field_cm;
  l_af_info.far_field = p_session_data->af_tuning_cfg.far_field_cm;

  switch (l_af_status) {
    case CAM_AF_STATE_PASSIVE_FOCUSED:
    case CAM_AF_STATE_FOCUSED_LOCKED: {
      l_af_info.af_status = IMG_STATS_STATUS_VALID;
      break;
    }
    default : {
      l_af_info.af_status = IMG_STATS_STATUS_INVALID;
      break;
    }
  }

  l_af_info.focused_roi.pos.x = p_session_data->af_tuning_cfg.roi.left;
  l_af_info.focused_roi.pos.y = p_session_data->af_tuning_cfg.roi.top;
  l_af_info.focused_roi.size.width = p_session_data->af_tuning_cfg.roi.width;
  l_af_info.focused_roi.size.height= p_session_data->af_tuning_cfg.roi.height;

  rc = img_set_meta(p_meta, IMG_META_AF_INFO, &l_af_info);
  if (rc != IMG_SUCCESS) {
    IDBG_ERROR("Error updating meta IMG_META_AF_INFO %d", rc);
    goto error;
  }

error:
  return rc;
}

/**
 * Function: module_qdc_common_init_params
 *
 * Description: This function is used to init parameters
 *
 * Arguments:
 *   p_client - Imgbase client
 *   p_params - init params
 *
 * Return values:
 *     true/false
 *
 * Notes: none
 **/
boolean module_qdc_common_init_params(imgbase_client_t *p_client,
  img_init_params_t *p_params)
{
  imgbase_session_data_t *p_base_session_data;
  module_imgbase_t *p_mod;
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_module_t *p_qdcc_mod;
  img_qdc_common_session_data_t *p_session_data;
  uint32_t i;

  IDBG_MED(":E");
  if (!p_client || !p_params) {
    IDBG_ERROR("Invalid Input %p %p", p_client, p_params);
    return false;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_mod = (img_qdc_common_module_t *)p_mod->mod_private;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_base_session_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_session_data) {
    /* Error printed in the macro*/
    return false;
  }
  p_session_data =
    (img_qdc_common_session_data_t *)p_qdcc_client->p_session_data;

  if (p_base_session_data->multi_cam_info.is_linked) {
    p_params->multicam_dim_info.cam_role =
      module_imglib_common_get_img_camera_role(
        p_base_session_data->multi_cam_info.cam_role);

    switch (p_qdcc_mod->qdc_algo_flavor) {
    case IMG_QDC_QTI_SOL:
      p_params->multicam_dim_info.sensor_calib_data.p_data =
        &p_qdcc_mod->system_calib_data;
      p_params->multicam_dim_info.sensor_calib_data.data_size =
        sizeof(cam_related_system_calibration_data_t);
      break;
    case IMG_QDC_OEM_SOL:
      p_params->multicam_dim_info.sensor_calib_data.p_data=
        p_qdcc_mod->system_calib_data.dc_otp_params;
      p_params->multicam_dim_info.sensor_calib_data.data_size=
        p_qdcc_mod->system_calib_data.dc_otp_size;
      break;
    }

    p_params->multicam_dim_info.focal_length =
      p_base_session_data->focal_length;
    p_params->multicam_dim_info.pix_pitch =
      p_base_session_data->pix_size;

    for (i = 0; i < p_session_data->input_size.num_streams; i++) {
      if (p_session_data->input_size.type[i] ==
        p_session_data->process_stream_type) {
        // set default dimension values

        p_params->multicam_dim_info.input_margin.w_margin =
          QIMG_CALC_MARGIN(
          p_session_data->input_size.stream_sz_plus_margin[i].width,
          p_session_data->output_size.stream_sz_plus_margin[i].width);

        p_params->multicam_dim_info.input_margin.h_margin =
          QIMG_CALC_MARGIN(
          p_session_data->input_size.stream_sz_plus_margin[i].height,
          p_session_data->output_size.stream_sz_plus_margin[i].height);

        p_params->multicam_dim_info.input_pad.w_pad =
          p_session_data->input_size.width_alignment;
        p_params->multicam_dim_info.input_pad.h_pad =
          p_session_data->input_size.height_alignment;

        p_params->multicam_dim_info.input_size.width =
          p_session_data->input_size.stream_sz_plus_margin[i].width;
        p_params->multicam_dim_info.input_size.height =
          p_session_data->input_size.stream_sz_plus_margin[i].height;
        p_params->multicam_dim_info.input_size.stride =
          QIMG_CEILINGN(
          p_session_data->input_size.stream_sz_plus_margin[i].width,
          p_session_data->input_size.width_alignment);
        p_params->multicam_dim_info.input_size.scanline =
          QIMG_CEILINGN(
          p_session_data->input_size.stream_sz_plus_margin[i].height,
          p_session_data->input_size.height_alignment);

        p_params->multicam_dim_info.output_size.width =
          p_session_data->output_size.stream_sz_plus_margin[i].width;
        p_params->multicam_dim_info.output_size.height =
          p_session_data->output_size.stream_sz_plus_margin[i].height;
        p_params->multicam_dim_info.output_size.stride =
          QIMG_CEILINGN(
          p_session_data->output_size.stream_sz_plus_margin[i].width,
          p_session_data->output_size.width_alignment);
        p_params->multicam_dim_info.output_size.scanline =
          QIMG_CEILINGN(
          p_session_data->output_size.stream_sz_plus_margin[i].height,
          p_session_data->output_size.height_alignment);

        if (p_session_data->output_size_changed[i] == true) {
          p_params->multicam_dim_info.output_size_changed = true;
          p_params->multicam_dim_info.orignal_output_size.width =
           p_session_data->orignal_output_size.stream_sz_plus_margin[i].width;
          p_params->multicam_dim_info.orignal_output_size.height =
           p_session_data->orignal_output_size.stream_sz_plus_margin[i].height;
          p_params->multicam_dim_info.orignal_output_size.stride =
           QIMG_CEILINGN(
           p_session_data->orignal_output_size.stream_sz_plus_margin[i].width,
           p_session_data->orignal_output_size.width_alignment);
          p_params->multicam_dim_info.output_size.scanline =
           QIMG_CEILINGN(
           p_session_data->orignal_output_size.stream_sz_plus_margin[i].height,
           p_session_data->orignal_output_size.height_alignment);
        }
        module_imglib_common_get_frame_fmt(
          p_session_data->input_size.stream_format[i],
          &p_params->multicam_dim_info.format);
        break;
      }
    }
  }

  IDBG_MED(":X");
  return TRUE;
}

/**
 * Function: module_qdc_common_query_mod
 *
 * Description: This function is used to query module caps
 *
 * Arguments:
 *   @p_mct_cap - capababilities
 *   @p_mod - pointer to the module
 *
 * Return values:
 *   true/false
 *
 * Notes: none
 **/
boolean module_qdc_common_query_mod(mct_pipeline_cap_t *p_mct_cap, void* p_mod, unsigned int sessionid )
{
  mct_pipeline_imaging_cap_t *p_cap = NULL;
  module_imgbase_t *p_base_mod = (module_imgbase_t *) p_mod;
  mct_pipeline_sensor_cap_t *p_sensor_cap = NULL;
  img_qdc_common_module_t *p_qdc_data = NULL;

  if (!p_mct_cap || !p_mod) {
    IDBG_ERROR("Error Input %p %p", p_mct_cap, p_mod);
    return FALSE;
  }

  p_qdc_data = (img_qdc_common_module_t *)p_base_mod->mod_private;
  if (!p_qdc_data) {
    IDBG_ERROR("Private data null");
    return FALSE;
  }

  p_cap = &p_mct_cap->imaging_cap;
  p_sensor_cap = &p_mct_cap->sensor_cap;
  if (!p_sensor_cap) {
     IDBG_ERROR("Error in sensor cap %p", p_sensor_cap);
     return FALSE;
  }
  //TBD: Check for sessionid-1, if this is valid
  p_qdc_data->session_data[sessionid-1].pix_pitch = p_sensor_cap->pix_size;
  p_qdc_data->session_data[sessionid-1].focal_length = p_sensor_cap->focal_length;

  switch (p_base_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    p_cap->feature_mask |= CAM_QTI_FEATURE_SAT;
    if (p_qdc_data->qdc_algo_flavor == IMG_QDC_QTI_SOL) {
      p_cap->avail_spatial_align_solns |= CAM_SPATIAL_ALIGN_QTI;
    } else {
      p_cap->avail_spatial_align_solns |= CAM_SPATIAL_ALIGN_OEM;
    }
    break;
  case CAM_QTI_FEATURE_RTB:
    p_cap->feature_mask |= CAM_QTI_FEATURE_RTB;
    break;
  case CAM_QTI_FEATURE_SAC:
    p_cap->feature_mask |= CAM_QTI_FEATURE_SAC;
    if (p_qdc_data->qdc_algo_flavor == IMG_QDC_QTI_SOL) {
      p_cap->avail_spatial_align_solns |= CAM_SPATIAL_ALIGN_QTI;
    } else {
      p_cap->avail_spatial_align_solns |= CAM_SPATIAL_ALIGN_OEM;
    }
    break;
  case CAM_QTI_FEATURE_RTBDM:
    p_cap->feature_mask |= CAM_QTI_FEATURE_RTBDM;
    break;
  default:
    IDBG_ERROR("Invalid feature mask");
    return FALSE;
  }

  return TRUE;
}

/**
 * Function: module_qdc_common_handle_stream_crop_event
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
boolean module_qdc_common_handle_stream_crop_event(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  IMG_UNUSED(is_evt_handled);
  int stream_idx = 0;
  imgbase_stream_t *p_stream = NULL;
  mct_stream_info_t *stream_info = NULL;
  boolean fwd_event = TRUE;
  mct_stream_info_t *input_stream_info = NULL;
  img_qdc_common_client_t *p_qdcc_client = NULL;
  img_qdc_common_session_data_t *p_session_data = NULL;
  module_imgbase_t *p_mod = NULL;

  IDBG_LOW("E");

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = p_qdcc_client->p_session_data;

  mct_bus_msg_stream_crop_t *stream_crop =
    (mct_bus_msg_stream_crop_t *)p_mod_event->module_event_data;
  /* Save stream crop info from ISP before updating*/
  p_client->stream_crop = *stream_crop;
  p_client->stream_crop_valid = TRUE;

  p_session_data->user_zoom_level = p_client->stream_crop.user_zoom;

  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);
  if (stream_idx < 0) {
    IDBG_ERROR("Cannot find stream mapped to idx %x", identity);
    return fwd_event;
  }

  input_stream_info = &p_client->isp_output_dim_stream_info;

  p_stream = &p_client->stream[stream_idx];
  if (NULL == p_stream) {
    IDBG_ERROR("Cannot find stream mapped to client");
    return fwd_event;
  }

  stream_info = p_stream->stream_info;

  IDBG_MED("Before stream_crop.x=%d, stream_crop.y=%d, stream_crop.dx=%d,"
    " stream_crop.dy=%d, identity=0x%x",
    stream_crop->x, stream_crop->y, stream_crop->crop_out_x,
    stream_crop->crop_out_y, identity);

  switch (p_mod->algo_caps.crop_caps) {
  case IMG_CROP_UPDATED:
    fwd_event = FALSE;
    break;
  case IMG_CROP_APPLIED:
    stream_crop->x = 0;
    stream_crop->y = 0;
    stream_crop->crop_out_x = stream_info->dim.width;
    stream_crop->crop_out_y = stream_info->dim.height;
    break;
  case IMG_CROP_NOT_APPLIED:
    if (input_stream_info->dim.width && stream_info->dim.width) {
      stream_crop->x = (stream_crop->x * stream_info->dim.width) /
        input_stream_info->dim.width;
      stream_crop->crop_out_x = (stream_crop->crop_out_x *
        stream_info->dim.width) / input_stream_info->dim.width;
    }

    if (input_stream_info->dim.height && stream_info->dim.height) {
      stream_crop->y = (stream_crop->y * stream_info->dim.height) /
        input_stream_info->dim.height;
      stream_crop->crop_out_y = (stream_crop->crop_out_y *
        stream_info->dim.height) / input_stream_info->dim.height;
    }
    break;
  }

  IDBG_LOW("After stream_crop.x=%d, stream_crop.y=%d, stream_crop.dx=%d,"
    " stream_crop.dy=%d, identity=0x%x crop fwd %d",
    stream_crop->x, stream_crop->y, stream_crop->crop_out_x,
    stream_crop->crop_out_y, identity, fwd_event);

  return fwd_event;
}

/**
 * Function: module_qdc_common_update_output_buffer_dim
 *
 * Description: This function updates the output buffer size
 *
 * Arguments:
 *  @p_client - pointer to imgbase client
 *  @p_stream - pointer to imgbase stream struct
 *
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_update_output_buffer_dim(imgbase_client_t *p_client,
  imgbase_stream_t *p_stream)
{
  module_imgbase_t *p_mod = NULL;
  img_qdc_common_client_t *p_qdcc_client;
  img_qdc_common_session_data_t *p_session_data;
  int color_fmt, out_idx;
  mct_stream_info_t *p_stream_info;
  uint32_t i;

  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_stream_info = p_stream->stream_info;
  p_mod = (module_imgbase_t *)p_client->p_mod;

  p_session_data = (img_qdc_common_session_data_t *)
    p_qdcc_client->p_session_data;
  if (p_session_data == NULL) {
    IDBG_ERROR("Error! p_session %p for client %p",
      p_session_data, p_qdcc_client);
    return IMG_ERR_GENERAL;
  }

  if (!p_session_data->output_size.num_streams) {
    IDBG_ERROR("Error! Output stream size not available");
    return IMG_ERR_GENERAL;
  }

  /* Get Index of the stream for output size */
  out_idx = IMG_QDC_COMMON_GET_OUTPUT_INDEX(p_session_data);

  p_stream_info->dim.width =
    p_session_data->output_size.stream_sz_plus_margin[out_idx].width;
  p_stream_info->dim.height =
    p_session_data->output_size.stream_sz_plus_margin[out_idx].height;
  p_stream_info->fmt = p_stream->isp_stream_info.fmt;
  p_stream_info->buf_planes.plane_info.frame_len = 0;

  for (i = 0; i < p_stream_info->buf_planes.plane_info.num_planes; i++) {
    if (i == 0) { // Y Plane
      p_stream_info->buf_planes.plane_info.mp[i].stride =
        QIMG_CEILINGN(p_stream_info->dim.width,
        p_session_data->output_size.width_alignment);
      p_stream_info->buf_planes.plane_info.mp[i].scanline =
        QIMG_CEILINGN(p_stream_info->dim.height,
        p_session_data->output_size.height_alignment);

      p_stream_info->buf_planes.plane_info.mp[i].len =
        (p_stream_info->buf_planes.plane_info.mp[i].stride *
        p_stream_info->buf_planes.plane_info.mp[i].scanline);
    } else {
      p_stream_info->buf_planes.plane_info.mp[i].stride =
        QIMG_CEILINGN(p_stream_info->dim.width,
        p_session_data->output_size.width_alignment);
      p_stream_info->buf_planes.plane_info.mp[i].scanline =
        QIMG_CEILINGN(p_stream_info->dim.height/2,
        p_session_data->output_size.height_alignment);

      p_stream_info->buf_planes.plane_info.mp[i].len =
        (p_stream_info->buf_planes.plane_info.mp[i].stride *
        p_stream_info->buf_planes.plane_info.mp[i].scanline);
    }
    p_stream_info->buf_planes.plane_info.frame_len +=
      p_stream_info->buf_planes.plane_info.mp[i].len;
  }

  if (p_mod->algo_caps.core_type == IMG_CORE_GPU) {
    color_fmt =
      module_imglib_common_get_venus_color_format(p_stream->isp_stream_info.fmt);
    p_stream_info->buf_planes.plane_info.frame_len =
      VENUS_BUFFER_SIZE(color_fmt,
      p_stream_info->buf_planes.plane_info.mp[0].stride,
      p_stream_info->buf_planes.plane_info.mp[0].scanline);

    for (i = 0; i < p_stream_info->buf_planes.plane_info.num_planes; i++) {
      if (i == 0) { // Y Plane
        p_stream_info->buf_planes.plane_info.mp[i].stride =
          VENUS_Y_STRIDE(color_fmt,
          p_stream_info->buf_planes.plane_info.mp[0].stride);
        p_stream_info->buf_planes.plane_info.mp[i].scanline =
          VENUS_Y_SCANLINES(color_fmt,
          p_stream_info->buf_planes.plane_info.mp[0].scanline);
        if (p_stream_info->fmt == CAM_FORMAT_YUV_420_NV12_UBWC) {
          p_stream_info->buf_planes.plane_info.mp[i].len =
            (uint32_t)(MSM_MEDIA_ALIGN(
            (p_stream_info->buf_planes.plane_info.mp[i].stride *
            p_stream_info->buf_planes.plane_info.mp[i].scanline), 4096) +
            p_stream_info->buf_planes.plane_info.mp[i].meta_len);
        } else {
          p_stream_info->buf_planes.plane_info.mp[i].len =
            (p_stream_info->buf_planes.plane_info.mp[i].stride *
            p_stream_info->buf_planes.plane_info.mp[i].scanline);
        }
      } else {
        p_stream_info->buf_planes.plane_info.mp[i].stride =
          VENUS_UV_STRIDE(color_fmt,
          p_stream_info->buf_planes.plane_info.mp[0].stride);
        p_stream_info->buf_planes.plane_info.mp[i].scanline =
          VENUS_UV_SCANLINES(color_fmt,
          p_stream_info->buf_planes.plane_info.mp[0].scanline);

        p_stream_info->buf_planes.plane_info.mp[i].len =
          p_stream_info->buf_planes.plane_info.frame_len -
          p_stream_info->buf_planes.plane_info.mp[0].len;
      }
    }
  }

  IDBG_LOW("After Update id %x stream op %dX%d plane[0] %dX%d len %d"
    " plane[1] %dX%d len %d frame_len %d\n",
    p_stream->identity,
    p_stream_info->dim.width, p_stream_info->dim.height,
    p_stream_info->buf_planes.plane_info.mp[0].stride,
    p_stream_info->buf_planes.plane_info.mp[0].scanline,
    p_stream_info->buf_planes.plane_info.mp[0].len,
    p_stream_info->buf_planes.plane_info.mp[1].stride,
    p_stream_info->buf_planes.plane_info.mp[1].scanline,
    p_stream_info->buf_planes.plane_info.mp[1].len,
    p_stream_info->buf_planes.plane_info.frame_len);

  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_send_new_isp_out_dim_event
 *
 * Description: This function send out updated output buffer
 * size to downstream modules
 *
 * Arguments:
 *  @p_client - pointer to imgbase client
 *  @p_stream - pointer to imgbase stream struct
 *
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_send_new_isp_out_dim_event(
  imgbase_client_t *p_client,
  imgbase_stream_t *p_stream)
{
  mct_stream_info_t new_out_stream_info;
  mct_stream_info_t *p_stream_info;
  int rc = IMG_SUCCESS;
  uint32_t i;

  if (!p_client || !p_stream) {
    IDBG_ERROR("Invalid Input");
    return IMG_ERR_INVALID_INPUT;
  }

  p_stream_info = p_stream->stream_info;

  /* Update new stream info structure */
  memset(&new_out_stream_info, 0, sizeof(mct_stream_info_t));
  new_out_stream_info.fmt = p_stream_info->fmt;
  new_out_stream_info.stream_type = p_stream_info->stream_type;
  new_out_stream_info.identity = p_stream_info->identity;
  new_out_stream_info.dim.width = p_stream_info->dim.width;
  new_out_stream_info.dim.height = p_stream_info->dim.height;
  new_out_stream_info.buf_planes.plane_info.num_planes =
    p_stream_info->buf_planes.plane_info.num_planes;

  IDBG_LOW("stream type %d,identity %x width %d, height %d num planes %d fmt %d",
    new_out_stream_info.stream_type, new_out_stream_info.identity,
    new_out_stream_info.dim.width, new_out_stream_info.dim.height,
    new_out_stream_info.buf_planes.plane_info.num_planes, new_out_stream_info.fmt);

  for (i = 0; i < p_stream_info->buf_planes.plane_info.num_planes; i++) {
    new_out_stream_info.buf_planes.plane_info.mp[i].stride =
      p_stream_info->buf_planes.plane_info.mp[i].stride;
    new_out_stream_info.buf_planes.plane_info.mp[i].scanline =
      p_stream_info->buf_planes.plane_info.mp[i].scanline;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset =
      p_stream_info->buf_planes.plane_info.mp[i].offset;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset_x =
      p_stream_info->buf_planes.plane_info.mp[i].offset_x;
    new_out_stream_info.buf_planes.plane_info.mp[i].offset_y =
      p_stream_info->buf_planes.plane_info.mp[i].offset_y;
    new_out_stream_info.buf_planes.plane_info.mp[i].len =
      p_stream_info->buf_planes.plane_info.mp[i].stride *
      p_stream_info->buf_planes.plane_info.mp[i].scanline;
    new_out_stream_info.buf_planes.plane_info.frame_len +=
      p_stream_info->buf_planes.plane_info.mp[i].len;

    IDBG_LOW("New ISP out dim struct: plane %d, stride %d, scanline %d,"
      "offset_x %d, offset_y %d, len %d, offset:%d", i,
      new_out_stream_info.buf_planes.plane_info.mp[i].stride,
      new_out_stream_info.buf_planes.plane_info.mp[i].scanline,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset_x,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset_y,
      new_out_stream_info.buf_planes.plane_info.mp[i].len,
      new_out_stream_info.buf_planes.plane_info.mp[i].offset);
  }
  /* Send new ISP_OUTPUT_EVENT downstream */
  rc = mod_imgbase_send_event(p_stream_info->identity, FALSE,
    MCT_EVENT_MODULE_ISP_OUTPUT_DIM, new_out_stream_info);
  if (!rc) {
    IDBG_ERROR(" Sending ISP OUT DIM failed %d", rc);
    return IMG_ERR_GENERAL;
  }
  return rc;
}

/**
 * Function: module_qdc_common_handle_isp_output_dim_event
 *
 * Description: This function handles the isp output dim event.
 * Updates and sends out a new ISP
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
boolean module_qdc_common_handle_isp_output_dim_event(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  boolean fwd_event = FALSE;
  module_imgbase_t *p_mod;
  img_qdc_common_module_t *p_qdcc_mod;
  *is_evt_handled = TRUE;
  int rc = IMG_SUCCESS, stream_idx;
  imgbase_stream_t *p_stream;
  img_qdc_common_client_t *p_qdcc_client;
  mct_stream_info_t *stream_info;
  img_component_ops_t *p_comp;
  img_init_params_t *p_params;

  IDBG_LOW(":E");
  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error invalid input");
    return fwd_event;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_comp = &p_client->comp;
  p_qdcc_mod = (img_qdc_common_module_t *)p_mod->mod_private;
  p_qdcc_client = (img_qdc_common_client_t*)p_client->p_private_data;
  p_params = &p_client->comp_init_params;

  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);
  if (stream_idx < 0) {
    IDBG_ERROR("Cannot find stream mapped to idx %x", identity);
    return fwd_event;
  }

  p_stream = &p_client->stream[stream_idx];

  stream_info = (mct_stream_info_t *)(p_mod_event->module_event_data);
  if (stream_info) {
    p_client->isp_output_dim_stream_info = *stream_info;
    p_client->isp_output_dim_stream_info_valid = TRUE;
    p_stream->isp_stream_info = *stream_info;

    IDBG_MED("MCT_EVENT_MODULE_ISP_OUTPUT_DIM %x, %dX%d",
      identity, stream_info->dim.width,
      stream_info->dim.height);
  } else {
    IDBG_MED("MCT_EVENT_MODULE_ISP_OUTPUT_DIM %x invalid",
      identity);
  }

  /* Update the internal stream dimensions i.e SAT module's o/p dim */
  stream_idx = module_imgbase_find_stream_by_identity(p_client, identity);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x. Not updating dim")
    return IMG_ERR_INVALID_INPUT;
  }
  p_stream = &p_client->stream[stream_idx];
  if (NULL == p_stream) {
    IDBG_ERROR(" Cannot find stream mapped to client");
    return IMG_ERR_INVALID_INPUT;
  }

  IDBG_LOW("IN stream info id %x stream op %dX%d plane[0] %dX%d"
    "len %d" " plane[1] %dX%d len %d frame_len %d\n",
    p_stream->identity, p_stream->isp_stream_info.dim.width,
    p_stream->isp_stream_info.dim.height,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[0].stride,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[0].scanline,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[0].len,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[1].stride,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[1].scanline,
    p_stream->isp_stream_info.buf_planes.plane_info.mp[1].len,
    p_stream->isp_stream_info.buf_planes.plane_info.frame_len);

  IDBG_LOW("Out stream info before update id %x stream op %dX%d plane[0] %dX%d"
    "len %d" " plane[1] %dX%d len %d frame_len %d\n",
    p_stream->identity, p_stream->stream_info->dim.width,
    p_stream->stream_info->dim.height,
    p_stream->stream_info->buf_planes.plane_info.mp[0].stride,
    p_stream->stream_info->buf_planes.plane_info.mp[0].scanline,
    p_stream->stream_info->buf_planes.plane_info.mp[0].len,
    p_stream->stream_info->buf_planes.plane_info.mp[1].stride,
    p_stream->stream_info->buf_planes.plane_info.mp[1].scanline,
    p_stream->stream_info->buf_planes.plane_info.mp[1].len,
    p_stream->stream_info->buf_planes.plane_info.frame_len);

  rc = module_qdc_common_update_output_buffer_dim(p_client, p_stream);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("Error: Not updating ISP OUT dimensions");
    fwd_event = TRUE;
  }

  module_qdc_common_send_new_isp_out_dim_event(p_client, p_stream);

  p_params->multicam_dim_info.input_size.width =
    p_stream->isp_stream_info.dim.width;
  p_params->multicam_dim_info.input_size.height =
    p_stream->isp_stream_info.dim.height;
  p_params->multicam_dim_info.input_size.stride =
    p_stream->isp_stream_info.buf_planes.plane_info.mp[0].stride;
  p_params->multicam_dim_info.input_size.scanline =
    p_stream->isp_stream_info.buf_planes.plane_info.mp[0].scanline;

  p_params->multicam_dim_info.output_size.width =
    p_stream->stream_info->dim.width;
  p_params->multicam_dim_info.output_size.height =
    p_stream->stream_info->dim.height;
  p_params->multicam_dim_info.output_size.stride =
    p_stream->stream_info->buf_planes.plane_info.mp[0].stride;
  p_params->multicam_dim_info.output_size.scanline =
    p_stream->stream_info->buf_planes.plane_info.mp[0].scanline;

  IDBG_MED("Update init params for session id %d ", p_client->session_id);
  rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_INIT_UPDATE,
    (void *)(p_params));
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("ERROR during set param for QIMG_PARAM_INIT_UPDATE rc %d", rc);
  }

  IDBG_MED(":X");
  return fwd_event;
}

/**
 * Function: module_qdc_common_handle_buffer_divert_ack
 *
 * Description: This function handles the buffer divert
 *   ack recieved for SAT module
 *
 * Arguments:
 *   @identity: Event Identity
 *   @p_mod_event - pointer of module event
 *   @p_client - pointer to imgbase client
 *   @p_core_ops - pointer to imgbase module ops
 *   @is_evt_handled - Flag indicating if base module event
 *                   handling is required
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
boolean module_qdc_common_handle_buffer_divert_ack(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  boolean fwd_event = TRUE;
  isp_buf_divert_ack_t *p_ack;
  imgbase_client_t *p_peer_client;
  boolean rc = 0;

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("Error input");
    return fwd_event;
  }

  p_ack = (isp_buf_divert_ack_t *)p_mod_event->module_event_data;
  if (!p_ack) {
    IDBG_ERROR("Invalid Input");
    return fwd_event;
  }

  IDBG_MED("ACK recieved on p_ack identity 0x%x, p_ack buf_idx 0x%x",
    p_ack->identity, p_ack->buf_identity);
  p_peer_client = (imgbase_client_t *)p_client->p_peer_client;

  /* Check if ack belongs to peer session */
  if ((IMGLIB_SESSIONID(p_ack->buf_identity) != p_client->session_id) &&
    (IMGLIB_SESSIONID(p_ack->buf_identity) == p_peer_client->session_id)) {
  /* Update Identity in the ack struc to divert identity and free internal buf */
    p_ack->identity = p_peer_client->divert_identity;
    rc = module_imgbase_client_handle_buffer_ack(p_peer_client,
      IMG_EVT_ACK_FREE_INTERNAL_BUF, p_ack);
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("[IMG_BUF_DBG_OUT] Error, Free internal buffer failed %d", rc);
    }
    fwd_event = false;
    *is_evt_handled = TRUE;
  } else {
    /*Ack is for the right session, let base module handle*/
    *is_evt_handled = FALSE;
  }
  return fwd_event;
}

/**
 * Function: module_qdc_common_do_in_place_ack
 *
 * Description: Helper function to set in place ack
 *
 * Arguments:
 *   @p_client - pointer to IMGLIB_BASE client
 *   @p_frame - output buffer
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_INVALID_INPUT
 *
 * Notes: none
 **/
int module_qdc_common_do_in_place_ack(imgbase_client_t *p_client,
  img_frame_t *p_frame)
{
  isp_buf_divert_t *p_buf_divert;
  imgbase_buf_t *p_imgbase_buf;

  if ((NULL == p_frame) || (NULL == p_client)) {
    IDBG_ERROR("Error Input");
    return IMG_ERR_INVALID_INPUT;
  }

  /* Retrieve divert info from private data */
  p_imgbase_buf = (imgbase_buf_t *)p_frame->private_data;
  if (!p_imgbase_buf) {
    IDBG_ERROR("Error, img_base buffer null");
    return IMG_ERR_INVALID_INPUT;
  }
  p_buf_divert = &p_imgbase_buf->buf_divert;
  p_buf_divert->ack_flag = TRUE;

  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_forward_buffer_on_peer_session
 *
 * Description: Forward buffer on peer session
 *
 * Arguments:
 *   @p_frame - pointer to the frame
 *   @p_client - pointer to IMGLIB_BASE client
 *
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_INVALID_INPUT
 *
 * Notes: none
 **/
int module_qdc_common_forward_buffer_on_peer_session(img_frame_t *p_frame,
  imgbase_client_t *p_client)
{
  imgbase_buf_t *p_imgbase_buf;
  isp_buf_divert_t *p_buf_divert;
  imgbase_client_t *p_peer_client;
  module_imgbase_t *p_mod;
  imgbase_stream_t *p_stream;
  boolean rc = FALSE;
  int ret = IMG_SUCCESS;
  int stream_idx;

  if ((NULL == p_frame) || (NULL == p_client)) {
    IDBG_ERROR("Error Invalid Input");
    return IMG_ERR_INVALID_INPUT;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_peer_client = (imgbase_client_t *)p_client->p_peer_client;
  if (!p_peer_client) {
    IDBG_ERROR("Error Invalid Input, Peer client is null");
    return IMG_ERR_INVALID_INPUT;
  }

  /* Retrieve divert info from private data */
  p_imgbase_buf = (imgbase_buf_t *)p_frame->private_data;
  if (!p_imgbase_buf) {
    IDBG_ERROR("[%s] Error, img_base buff null", p_mod->name);
    return IMG_ERR_INVALID_INPUT;
  }

  p_buf_divert = &p_imgbase_buf->buf_divert;

  IDBG_MED("Sending buffer with buffer_identity 0x%x, divert identity 0x%x on"
    "peer_client on divert identity ox%x", p_buf_divert->identity,
    p_client->divert_identity, p_peer_client->divert_identity);

  stream_idx = module_imgbase_find_stream_by_identity(p_peer_client,
    p_peer_client->divert_identity);
  if (stream_idx < 0) {
    IDBG_ERROR("[%s] Cannot find stream mapped to idx %x",
      p_mod->name, p_peer_client->divert_identity);
    goto end;
  }

  p_stream = &p_peer_client->stream[stream_idx];
  IDBG_MED("[IMG_BUF_DBG_OUT_f%d_b%d_i%x] send buf_div timestamp %llu",
    p_buf_divert->buffer.sequence, p_frame->idx, p_peer_client->divert_identity,
    p_imgbase_buf->buf_divert.buffer.timestamp.tv_sec
    * 1000000LL + p_imgbase_buf->buf_divert.buffer.timestamp.tv_usec);

  // Send buffer divert downstream
  p_buf_divert->ack_flag = FALSE;
  rc = mod_imgbase_send_event(p_peer_client->divert_identity, FALSE,
    MCT_EVENT_MODULE_BUF_DIVERT, *p_buf_divert);
  if (!rc) {
    IDBG_ERROR("Error, send divert event failed %d", rc);
    rc = IMG_ERR_GENERAL;
    goto end;
  }

  /* if no inplace ack was issued for divert, proceed */
  if (!p_buf_divert->ack_flag) {
    rc = IMG_SUCCESS;
    goto end;
  }

  IDBG_MED("[%s] [IMG_BUF_DBG_%s_f%d_b%d_i%x divert_idx %x] inplace buf_ack",
    p_mod->name,
    "OUT",
    p_buf_divert->buffer.sequence,
    p_buf_divert->buffer.index,
    p_buf_divert->identity,
    p_peer_client->divert_identity)

  if (p_mod->caps.use_internal_bufs) {
    isp_buf_divert_ack_t l_buf_divert_ack;
    memset(&l_buf_divert_ack, 0, sizeof(isp_buf_divert_ack_t));
    l_buf_divert_ack.buf_idx = p_buf_divert->buffer.index;
    l_buf_divert_ack.is_buf_dirty = 1;
    l_buf_divert_ack.identity = p_client->divert_identity;
    l_buf_divert_ack.buf_identity = p_buf_divert->identity;
    l_buf_divert_ack.frame_id = p_buf_divert->buffer.sequence;
    l_buf_divert_ack.channel_id = p_buf_divert->channel_id;
    l_buf_divert_ack.meta_data = p_buf_divert->meta_data;
    l_buf_divert_ack.buffer_access = p_buf_divert->buffer_access;
    rc = module_imgbase_client_handle_buffer_ack(p_client,
      IMG_EVT_ACK_FREE_INTERNAL_BUF, &l_buf_divert_ack);
    if (IMG_ERROR(rc)) {
      IDBG_ERROR("[%s] [IMG_BUF_DBG_OUT] Error, internal buf failed %d",
        p_mod->name, rc);
    }
  } else {
    //Todo: Need to handle this case if needed
    IDBG_ERROR("In place Ack on Non internal buffer not handled, Fatal");
    goto end;
  }

end:
  if (!p_mod->caps.use_internal_bufs && p_imgbase_buf) {
    free(p_imgbase_buf);
    p_imgbase_buf = NULL;
  }
  return ret;
}

/**
 * Function: module_qdc_common_handle_output_buf_done
 *
 * Description: Handle output buffer done event
 *
 * Arguments:
 *   @p_client - pointer to IMGLIB_BASE client
 *   @p_event - pointer to the event
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_INVALID_INPUT
 *   IMG_ERR_EAGAIN
 *
 * Notes: none
 **/
int module_qdc_common_handle_output_buf_done(imgbase_client_t *p_client,
  img_event_t *p_event)
{
  img_frame_t *p_frame;
  module_imgbase_t *p_mod;
  img_qdc_common_client_t *p_qdcc_client;
  imgbase_session_data_t *p_base_sess_data;
  img_camera_role_t cam_role;
  img_camera_role_t result_camera_role;
  uint32_t result_frameid;

  int rc = IMG_SUCCESS;

  if ((NULL == p_event) || (NULL == p_client)) {
    IDBG_ERROR("Error Input");
    return IMG_ERR_INVALID_INPUT;
  }

  p_frame = p_event->d.p_frame;
  if (!p_frame) {
    IDBG_ERROR("Output frame is null");
    goto end;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    /* Error printed in the macro*/
    return IMG_ERR_INVALID_INPUT;
  }

  switch (p_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    result_camera_role = p_qdcc_client->qdc_result.sat_result.chosen_frame;
    result_frameid = p_qdcc_client->qdc_result.sat_result.frameid;
    break;
  case CAM_QTI_FEATURE_RTB:
    /* Todo: HACK camara role to always be IMG_CAM_ROLE_WIDE, should get from meta data*/
    result_camera_role = IMG_CAM_ROLE_WIDE;
    result_frameid = p_qdcc_client->qdc_result.rtb_result.frameid;
    break;
  default:
    IDBG_ERROR("Invalid feature mask");
    return IMG_ERR_GENERAL;
  }

  cam_role =
    module_imglib_common_get_img_camera_role(p_base_sess_data->multi_cam_info.cam_role);

  if (p_mod->caps.use_internal_bufs) {
    IDBG_MED("Session id %d Current camera session type %d, chosen frame %d,"
      "Current session mode %d", p_client->session_id,
      p_base_sess_data->multi_cam_info.cam_role,
      result_camera_role,
      p_base_sess_data->multi_cam_info.cam_mode);

    /* Invalid result, Drop frame */
    if (result_camera_role == IMG_CAM_ROLE_INVALID) {
      IDBG_HIGH("Dropping frame sessionid %d frameid %d",
        p_client->session_id, result_frameid);
      /* Do buffer done on current session frame */
      module_qdc_common_do_in_place_ack(p_client, p_frame);
      rc = IMG_ERR_EAGAIN;
      goto end;
    }

    if (result_camera_role == cam_role) {
      pthread_mutex_lock(&p_base_sess_data->q_mutex);
      if(CAM_MODE_PRIMARY == p_base_sess_data->multi_cam_info.cam_mode) {
        /* Chosen frame belongs to the current session. Forward it downstream */
        IDBG_MED("Forward frame downstream sessionid %d", p_client->session_id);
        rc = IMG_ERR_EAGAIN;
        pthread_mutex_unlock(&p_base_sess_data->q_mutex);
      } else {
        /* Do buffer done on current session frame */
        pthread_mutex_unlock(&p_base_sess_data->q_mutex);
        module_qdc_common_do_in_place_ack(p_client, p_frame);
        rc = IMG_ERR_EAGAIN;
      }
    } else {
      pthread_mutex_lock(&p_base_sess_data->q_mutex);
      if (CAM_MODE_PRIMARY != p_base_sess_data->multi_cam_info.cam_mode) {
        /* Best case: Just do a buffer done. Dont forward */
        IDBG_MED("Do buffer done on sessionid %d", p_client->session_id);
        pthread_mutex_unlock(&p_base_sess_data->q_mutex);
        module_qdc_common_do_in_place_ack(p_client, p_frame);
        rc = IMG_ERR_EAGAIN;
      } else {
        /* Right buffer, wrong session. Send it on right session */
        pthread_mutex_unlock(&p_base_sess_data->q_mutex);
        module_qdc_common_forward_buffer_on_peer_session(p_frame, p_client);
        IDBG_MED("Frame from current session %d forwaded on peer session id %d",
          p_client->session_id,
          ((imgbase_client_t *)p_client->p_peer_client)->session_id);
        rc = IMG_SUCCESS;
      }
    }
  } else {
  /* Execute base module event handling */
  rc = IMG_ERR_EAGAIN;
  }
end:
  return rc;
}

/**
 * Function: module_qdc_common_handle_frame_drop
 *
 * Description: Error Event handler for SAT module
 *
 * Arguments:
 *   @p_client - pointer to IMGLIB_BASE client
 *   @p_event - pointer to the event
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_INVALID_INPUT
 *   IMG_ERR_EAGAIN
 *
 * Notes: none
 **/
int module_qdc_common_handle_frame_drop(imgbase_client_t *p_client,
  img_event_t *p_event)
{
  int rc = IMG_SUCCESS;
  uint32_t frame_id;
  module_imgbase_t *p_mod;
  img_qdc_common_client_t *p_qdcc_client;

  if ((NULL == p_event) || (NULL == p_client)) {
    IDBG_ERROR("Error Input");
    return IMG_ERR_INVALID_INPUT;
  }
  frame_id = p_event->d.frameid;
  IDBG_ERROR("Dropping frame %d sessionid %d",
    frame_id, p_client->session_id);

  /* Update result for frameid to be invalid */
  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;

  switch (p_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    p_qdcc_client->qdc_result.sat_result.frameid = frame_id;
    p_qdcc_client->qdc_result.sat_result.chosen_frame = IMG_CAM_ROLE_INVALID;
    break;
  case CAM_QTI_FEATURE_RTB:
    p_qdcc_client->qdc_result.rtb_result.frameid = frame_id;
    p_qdcc_client->qdc_result.rtb_result.chosen_frame = IMG_CAM_ROLE_INVALID;
    break;
  default:
    IDBG_ERROR("Invalid feature mask");
    return IMG_ERR_GENERAL;
  }

  return rc;
}

/**
 * Function: module_qdc_common_save_isp_stream_sizes
 *
 * Description: This function saves the ISP stream sizes and
 * updates it based on SAT algo requirments.
 *
 * Arguments:
 *   @p_client: pointer to imgbase client
 *   @p_stream_sizes: ISP stream sizes
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_save_isp_stream_sizes(imgbase_client_t *p_client,
  sensor_isp_stream_sizes_t *p_stream_sizes)
{
  uint32_t rc = IMG_SUCCESS;

  module_imgbase_t *p_mod = NULL;
  img_qdc_common_client_t *p_qdcc_client = NULL;
  img_qdc_common_session_data_t *p_session_data = NULL;
  img_component_ops_t *p_comp = NULL;
  imgbase_session_data_t *p_base_sess_data = NULL;

  img_multicam_dim_info_t dim_info;
  img_lib_config_t lib_config;
  uint32_t isp_idx = 0;
  uint32_t img_idx = 0;
  img_camera_mode_t l_cam_mode;

  if (!p_client || !p_stream_sizes) {
    IDBG_ERROR("Invalid input %p %p", p_client, p_stream_sizes);
    return IMG_ERR_INVALID_INPUT;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data =
    (img_qdc_common_session_data_t *)p_qdcc_client->p_session_data;
  p_comp = &p_mod->comp_ops;
  p_base_sess_data = IMGBASE_SSP(p_mod, p_client->session_id);
  if (NULL == p_base_sess_data) {
    /* Error printed in the macro*/
    return IMG_ERR_INVALID_INPUT;
  }

  switch (p_mod->feature_mask) {
  case CAM_QTI_FEATURE_SAT:
    l_cam_mode = IMG_CAM_MODE_PREVIEW;
    for (isp_idx = 0; isp_idx < p_stream_sizes->num_streams; isp_idx++) {
      if (p_stream_sizes->type[isp_idx] == CAM_STREAM_TYPE_VIDEO) {
        l_cam_mode = IMG_CAM_MODE_VIDEO;
        break;
      }
    }
  break;
  case CAM_QTI_FEATURE_RTB:
    l_cam_mode = IMG_CAM_MODE_BOKEH;
  break;
  default:
    l_cam_mode = IMG_CAM_MODE_DEFAULT;
  }

  memset(&p_session_data->orignal_output_size, 0x0,
    sizeof(sensor_isp_stream_sizes_t));
  memset(&p_session_data->output_size, 0x0,
    sizeof(sensor_isp_stream_sizes_t));
  memset(&p_session_data->input_size, 0x0,
    sizeof(sensor_isp_stream_sizes_t));

  for (isp_idx = 0; isp_idx < p_stream_sizes->num_streams; isp_idx++) {
    /* Save stream size for streams that have module feature mask enabled*/
    if (p_stream_sizes->postprocess_mask[isp_idx] & p_mod->feature_mask) {
      IMG_QDC_COMMON_CPY_ISP_STREAM_SIZE(&p_session_data->orignal_output_size,
        p_stream_sizes, img_idx, isp_idx);
      IMG_QDC_COMMON_CPY_ISP_STREAM_SIZE(&p_session_data->output_size,
        p_stream_sizes, img_idx, isp_idx);
      IMG_QDC_COMMON_PRINT_ISP_STREAM_SIZE(p_stream_sizes, isp_idx, "Before");

      memset(&dim_info, 0x0, sizeof(img_multicam_dim_info_t));
      dim_info.cam_role =
        module_imglib_common_get_img_camera_role(p_base_sess_data->
        multi_cam_info.cam_role);
      dim_info.output_size.width =
        p_stream_sizes->stream_sz_plus_margin[isp_idx].width;
      dim_info.output_size.height =
        p_stream_sizes->stream_sz_plus_margin[isp_idx].height;
      dim_info.output_size.stride = dim_info.output_size.width;
      dim_info.output_size.scanline = dim_info.output_size.height;
      dim_info.cam_mode = l_cam_mode;

      lib_config.lib_data = &dim_info;
      lib_config.lib_param = IMG_ALGO_IN_FRAME_DIM;
      rc = IMG_COMP_GET_PARAM(p_comp, QIMG_PARAM_SRC_DIM, &lib_config);
      if (!IMG_ERROR(rc)) {
        if (dim_info.output_size_changed) {
          p_session_data->output_size_changed[img_idx] = true;
          p_session_data->output_size.stream_sz_plus_margin[img_idx].width =
            dim_info.output_size.width;
          p_session_data->output_size.stream_sz_plus_margin[img_idx].height =
            dim_info.output_size.height;
        }
        p_stream_sizes->margins[isp_idx].widthMargins +=
          dim_info.input_margin.w_margin;
        p_stream_sizes->margins[isp_idx].heightMargins +=
          dim_info.input_margin.h_margin;
        p_stream_sizes->stream_sz_plus_margin[isp_idx].width =
          dim_info.input_size.width;
        p_stream_sizes->stream_sz_plus_margin[isp_idx].height =
          dim_info.input_size.height;
        if (p_stream_sizes->width_alignment < dim_info.input_pad.w_pad)
          p_stream_sizes->width_alignment = dim_info.input_pad.w_pad;
        if (p_stream_sizes->height_alignment < dim_info.input_pad.h_pad)
          p_stream_sizes->height_alignment = dim_info.input_pad.h_pad;
      } else {
        IDBG_ERROR("get param for margin failed");
      }

      IMG_QDC_COMMON_PRINT_ISP_STREAM_SIZE(p_stream_sizes, isp_idx, "After");
      IMG_QDC_COMMON_CPY_ISP_STREAM_SIZE(&p_session_data->input_size,
        p_stream_sizes, img_idx, isp_idx);
      img_idx++;
    }
  }

  if (!IMG_ERROR(rc)) {
    p_session_data->orignal_output_size.num_streams = img_idx;
    p_session_data->output_size.num_streams = img_idx;
    p_session_data->input_size.num_streams = img_idx;
  }

  IDBG_MED("img_idx %d", img_idx);
  IDBG_MED("num input streams %d", p_session_data->input_size.num_streams);
  IDBG_MED("num output streams %d", p_session_data->output_size.num_streams);

  return rc;
}

/**
 * Function: module_qdc_common_set_process_stream_type
 *
 * Description: This function is used to set the stream type to
 * process.
 *
 * Arguments:
 *   @p_client: pointer to imgbase client
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_set_process_stream_type(imgbase_client_t *p_client)
{
  uint32_t rc = IMG_SUCCESS;

  img_qdc_common_client_t *p_qdcc_client = NULL;
  img_qdc_common_session_data_t *p_session_data = NULL;

  uint32_t streams_mask = 0x0;
  uint32_t img_idx = 0;

  if (!p_client) {
    IDBG_ERROR("Invalid input %p", p_client);
    return IMG_ERR_INVALID_INPUT;
  }

  p_qdcc_client = (img_qdc_common_client_t *)p_client->p_private_data;
  p_session_data = (img_qdc_common_session_data_t *)p_qdcc_client->p_session_data;

  if (p_session_data->output_size.num_streams == 0) {
    p_session_data->process_stream_type = CAM_STREAM_TYPE_MAX;
  }

  for (img_idx = 0; img_idx < p_session_data->output_size.num_streams;
    img_idx++) {
    switch (p_session_data->output_size.type[img_idx]) {
    case CAM_STREAM_TYPE_VIDEO:
      streams_mask |= IMG_2_MASK(CAM_STREAM_TYPE_VIDEO);
      break;
    case CAM_STREAM_TYPE_PREVIEW:
      streams_mask |= IMG_2_MASK(CAM_STREAM_TYPE_PREVIEW);
      break;
    case CAM_STREAM_TYPE_CALLBACK:
      streams_mask |= IMG_2_MASK(CAM_STREAM_TYPE_CALLBACK);
      break;
    case CAM_STREAM_TYPE_ANALYSIS:
      streams_mask |= IMG_2_MASK(CAM_STREAM_TYPE_ANALYSIS);
      break;
    default:
      break;
    }
  }

  IDBG_MED("stream mask 0x%x", streams_mask);
  if (streams_mask & IMG_2_MASK(CAM_STREAM_TYPE_VIDEO)) {
    p_session_data->process_stream_type = CAM_STREAM_TYPE_VIDEO;
  } else if (streams_mask & IMG_2_MASK(CAM_STREAM_TYPE_PREVIEW)) {
    p_session_data->process_stream_type = CAM_STREAM_TYPE_PREVIEW;
  } else if (streams_mask & IMG_2_MASK(CAM_STREAM_TYPE_CALLBACK)) {
    p_session_data->process_stream_type = CAM_STREAM_TYPE_CALLBACK;
  } else if (streams_mask & IMG_2_MASK(CAM_STREAM_TYPE_ANALYSIS)) {
    p_session_data->process_stream_type = CAM_STREAM_TYPE_ANALYSIS;
  }
  IDBG_MED("num streams = %d", p_session_data->output_size.num_streams);
  IDBG_MED("process stream type %d", p_session_data->process_stream_type);

  return rc;
}

/**
 * Function: module_qdc_common_sensor_query_out_size
 *
 * Description: This function to handle sensor query output size event
 *
 * Arguments:
 *   @p_client: pointer to imgbase client
 *   @p_event: pointer to event
 *   @p_stream_sizes: pointer to stream sizes
 *   @p_fwd_event: pointer to forward event flag
 *
 * Return values:
 *     IMG_SUCCESS
 *     IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_qdc_common_sensor_query_out_size(imgbase_client_t *p_client,
  mct_event_t *p_event, sensor_isp_stream_sizes_t *p_stream_sizes,
  boolean *p_fwd_event)
{
  int32_t stream_idx;
  uint32_t rc = IMG_SUCCESS;
  imgbase_stream_t *p_stream = NULL;

  IDBG_MED(":E");
  if (!p_fwd_event) {
    IDBG_ERROR("pointer to forward event flag is null");
    return IMG_ERR_INVALID_INPUT;
  }
  *p_fwd_event = FALSE;

  if (!p_client || !p_event || !p_stream_sizes) {
    IDBG_ERROR("Invalid input %p %p %p", p_client, p_event,
      p_stream_sizes);
    return IMG_ERR_INVALID_INPUT;
  }

  if (p_event->direction == MCT_EVENT_UPSTREAM) {
    IDBG_ERROR("Invalid event direction");
    return IMG_ERR_INVALID_OPERATION;
  }

  stream_idx = module_imgbase_find_stream_by_identity(p_client,
    p_event->identity);
  if (stream_idx < 0) {
    IDBG_ERROR(" Cannot find stream mapped to idx %x");
    return IMG_ERR_NOT_FOUND;
  }

  p_stream= &p_client->stream[stream_idx];
  /* p_stream is used in macro mod_imgbase_send_event*/
  rc = mod_imgbase_send_event(p_event->identity, FALSE,
    MCT_EVENT_MODULE_SENSOR_QUERY_OUTPUT_SIZE, *p_stream_sizes);
  if (!rc) {
    IDBG_ERROR("Send event failed");
    return IMG_ERR_GENERAL;
  }
  if (p_stream_sizes->num_streams > MAX_NUM_STREAMS) {
    IDBG_ERROR("Invalid number of streams");
    return IMG_ERR_INVALID_INPUT;
  }

  rc = module_qdc_common_save_isp_stream_sizes(p_client, p_stream_sizes);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("Error %d", rc);
    return rc;
  }

  rc = module_qdc_common_set_process_stream_type(p_client);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("Error %d", rc);
    return rc;
  }

  IDBG_MED(":X");
  return IMG_SUCCESS;
}

/**
 * Function: module_qdc_common_sat_custom_session_config
 *
 * Description: Function to customize session configuration for
 *   SAT
 *
 * Arguments:
 *   @p_caps: pointer img caps
 *   @p_params: pointer img params
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
void module_qdc_common_sat_custom_session_config(img_caps_t *p_caps,
  module_imgbase_params_t *p_params)
{
  p_params->streams_to_process = IMG_2_MASK(CAM_STREAM_TYPE_VIDEO) |
    IMG_2_MASK(CAM_STREAM_TYPE_PREVIEW);
}

/**
 * Function: module_qdc_common_sac_custom_session_config
 *
 * Description: Function to customize session configuration for
 *   SAC
 *
 * Arguments:
 *   @p_caps: pointer img caps
 *   @p_params: pointer img params
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
void module_qdc_common_sac_custom_session_config(img_caps_t *p_caps,
  module_imgbase_params_t *p_params)
{
  p_params->streams_to_process = IMG_2_MASK(CAM_STREAM_TYPE_ANALYSIS);

  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_STREAM_CROP] = NULL;
  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_ISP_OUTPUT_DIM] = NULL;
  p_params->imgbase_client_event_handler[QIMG_EVT_IMG_OUT_BUF_DONE] = NULL;
  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_BUF_DIVERT_ACK] = NULL;
  p_params->imgbase_client_event_handler[QIMG_EVT_FRAME_DROP] = NULL;
  p_params->exec_mode = IMG_EXECUTION_SW;
}

/**
 * Function: module_qdc_common_rtb_custom_session_config
 *
 * Description: Function to customize session configuration for
 *   RTB
 *
 * Arguments:
 *   @p_caps: pointer img caps
 *   @p_params: pointer img params
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
void module_qdc_common_rtb_custom_session_config(
  img_caps_t *p_caps, module_imgbase_params_t *p_params)
{
  p_params->streams_to_process = IMG_2_MASK(CAM_STREAM_TYPE_PREVIEW);
  p_params->imgbase_client_event_handler[QIMG_EVT_META_BUF_DONE] = NULL;
}

/**
 * Function: module_qdc_common_rtbdm_custom_session_config
 *
 * Description: Function to customize session configuration for
 *   RTBDM
 *
 * Arguments:
 *   @p_caps: pointer img caps
 *   @p_params: pointer img params
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
void module_qdc_common_rtbdm_custom_session_config(
  img_caps_t *p_caps, module_imgbase_params_t *p_params)
{
  p_params->streams_to_process = IMG_2_MASK(CAM_STREAM_TYPE_ANALYSIS);

  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_STREAM_CROP] = NULL;
  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_ISP_OUTPUT_DIM] = NULL;
  p_params->imgbase_client_event_handler[QIMG_EVT_IMG_OUT_BUF_DONE] = NULL;
  p_params->imgbase_handle_module_event[MCT_EVENT_MODULE_BUF_DIVERT_ACK] = NULL;
  p_params->imgbase_client_event_handler[QIMG_EVT_FRAME_DROP] = NULL;
  p_params->exec_mode = IMG_EXECUTION_SW;
}

/** g_params:
 *
 *  imgbase parameters
 **/
static module_imgbase_params_t g_params = {
  .imgbase_client_created = module_qdc_common_client_created,
  .imgbase_client_destroy = module_qdc_common_client_destroy,
  .imgbase_client_init_params = module_qdc_common_init_params,
  .imgbase_client_update_meta = module_qdc_common_update_meta,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_STREAM_CROP] =
    module_qdc_common_handle_stream_crop_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_ISP_OUTPUT_DIM] =
    module_qdc_common_handle_isp_output_dim_event,
  .imgbase_client_event_handler[QIMG_EVT_IMG_OUT_BUF_DONE] =
    module_qdc_common_handle_output_buf_done,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_BUF_DIVERT_ACK] =
    module_qdc_common_handle_buffer_divert_ack,
  .imgbase_client_event_handler[QIMG_EVT_FRAME_DROP] =
    module_qdc_common_handle_frame_drop,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_SET_STREAM_CONFIG] =
    module_qdc_common_handle_set_stream_config,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_STATS_AF_UPDATE] =
    module_qdc_common_handle_module_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_IMGLIB_AF_CONFIG] =
    module_qdc_common_handle_module_event,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_SET_DUAL_OTP_PTR] =
    module_qdc_common_handle_module_event,
  .imgbase_client_event_handler[QIMG_EVT_META_BUF_DONE] =
    module_qdc_common_handle_meta_buf_done,
  .imgbase_client_streamon = module_qdc_common_client_streamon,
  .imgbase_client_link_peer = module_qdc_common_link_peer,
  .imgbase_handle_ctrl_event[MCT_EVENT_CONTROL_MASTER_INFO] =
    module_qdc_common_handle_ctrl_event,
  .imgbase_handle_ctrl_event[MCT_EVENT_CONTROL_HW_SLEEP] =
    module_qdc_common_handle_ctrl_event,
  .imgbase_handle_ctrl_event[MCT_EVENT_CONTROL_HW_WAKEUP] =
    module_qdc_common_handle_ctrl_event,
  .imgbase_handle_ctrl_parm[CAM_INTF_PARM_ZOOM] =
    module_qdc_common_handle_ctrl_parm,
  .imgbase_query_mod = module_qdc_common_query_mod,
  .imgbase_client_sensor_query_out_size =
    module_qdc_common_sensor_query_out_size,
  .exec_mode = IMG_EXECUTION_SW_HW,
  .access_mode = IMG_ACCESS_READ_WRITE,
  .force_cache_op = 0,
};

/** g_sat_lib_flavors_table:
 *
 *  sat library flavors table
 **/
static img_qdc_lib_info_t g_sat_lib_flavors_table[] = {
  {
    "libmmcamera_sat_dummy_lib.so",
    IMG_QDC_QTI_SOL,
  },
};

/** g_sac_lib_flavors_table:
 *
 *  sac library flavors table
 **/
static img_qdc_lib_info_t g_sac_lib_flavors_table[] = {
  {
    "libmmcamera_sac_lib.so",
    IMG_QDC_QTI_SOL,
  },
};

/** g_rtb_lib_flavors_table:
 *
 *  rtb library flavors table
 **/
static img_qdc_lib_info_t g_rtb_lib_flavors_table[] = {
  {
    "libmmcamera_dummyalgo.so",
    IMG_QDC_QTI_SOL,
  },
};

/** g_rtbdm_lib_flavors_table:
 *
 *  rtbdm library flavors table
 **/
static img_qdc_lib_info_t g_rtbdm_lib_flavors_table[] = {
  {
    "libmmcamera_dummyalgo.so",
    IMG_QDC_QTI_SOL,
  },
};

/** g_qdc_features_table:
 *
 *  qdc features table
 **/
static img_qdc_feature_info_t g_qdc_features_table[] = {
  {
    CAM_QTI_FEATURE_SAT,
    g_sat_lib_flavors_table,
    sizeof(g_sat_lib_flavors_table) / sizeof(img_qdc_lib_info_t),
  },
  {
    CAM_QTI_FEATURE_SAC,
    g_sac_lib_flavors_table,
    sizeof(g_sac_lib_flavors_table) / sizeof(img_qdc_lib_info_t),
  },
  {
    CAM_QTI_FEATURE_RTB,
    g_rtb_lib_flavors_table,
    sizeof(g_rtb_lib_flavors_table) / sizeof(img_qdc_lib_info_t),
  },
  {
    CAM_QTI_FEATURE_RTBDM,
    g_rtbdm_lib_flavors_table,
    sizeof(g_rtbdm_lib_flavors_table) / sizeof(img_qdc_lib_info_t),
  },
};

/** module_qdc_common_feature_init:
 *
 * Description: Function used to initialize the QDC feature
 * based on feature mask and return library name
 *
 *  Arguments:
 *  @feature_idx - feature index
 *  @feature_mask - feature mask
 *
 * Return values:
 *   Feature library name
 *
 * Notes: none
 **/
char *module_qdc_common_get_feature_lib(uint8_t feature_idx,
  cam_feature_mask_t feature_mask, img_qdc_flavor_t *p_flavor)
{
  img_qdc_lib_info_t *p_lib_info_arr;
  uint8_t lib_idx;
  char *p_lib_name = NULL;

  p_lib_info_arr = g_qdc_features_table[feature_idx].p_libs_arr;
  // get the index for the last entry in lib array
  lib_idx = g_qdc_features_table[feature_idx].arr_size - 1;

  p_lib_name = p_lib_info_arr[lib_idx].p_lib_name;
  if (p_lib_name == NULL) {
    IDBG_ERROR("error, lib name null");
    return NULL;
  }

  switch (p_lib_info_arr[lib_idx].lib_flavor) {
  case IMG_QDC_OEM_SOL: {
    IDBG_MED("plugging in OEM solution");
    *p_flavor = IMG_QDC_OEM_SOL;
    break;
  }
  case IMG_QDC_QTI_SOL: {
    IDBG_MED("plugging in QTI solution");
    *p_flavor = IMG_QDC_QTI_SOL;
    break;
  }
  default:
    return NULL;
  }

  return p_lib_name;
}

/** module_qdc_common_init:
 *
 * Description: Function used to initialize the QDC Common
 * module
 *
 *  Arguments:
 *  @name - name of the module
 *
 * Return values:
 *     MCTL module instance pointer
 *
 * Notes: none
 **/
mct_module_t *module_qdc_common_init(const char *name)
{
  char prop[PROPERTY_VALUE_MAX];
  mct_module_t *p_mct_mod = NULL;
  module_imgbase_t *p_mod = NULL;
  img_qdc_common_module_t *p_qdcc_mod = NULL;
  img_caps_t *p_g_caps = NULL;
  char *lib_name = NULL;
  cam_feature_mask_t feature_mask;
  uint8_t i;
  uint8_t num_features;
  uint8_t feature_idx = 0;
  img_qdc_flavor_t qdc_algo_flavor;
  uint32_t rc = IMG_SUCCESS;

  if (name == NULL) {
    IDBG_ERROR("Invalid input NULL");
    return NULL;
  }

  IDBG_MED("Init module %s", name);

  /* Identify module feature by name*/
  if (strcmp(name, "sat") == 0) {
    feature_mask = CAM_QTI_FEATURE_SAT;
  } else if (strcmp(name, "sac") == 0) {
    feature_mask = CAM_QTI_FEATURE_SAC;
  } else if(strcmp(name, "rtb") == 0) {
    feature_mask = CAM_QTI_FEATURE_RTB;
  } else if (strcmp(name, "rtbdm") == 0) {
    feature_mask = CAM_QTI_FEATURE_RTBDM;
  } else {
    IDBG_ERROR("Invalid module name %s", name);
    return NULL;
  }

  /* Get number of features supported by qdc_common */
  num_features = sizeof(g_qdc_features_table) / sizeof(img_qdc_feature_info_t);
  if (!num_features) {
    IDBG_ERROR("Error feature table empty");
    return NULL;
  }

  /* Find feature index to feature in features table*/
  for (i = 0; i < num_features; i++) {
    if (feature_mask == g_qdc_features_table[i].feature_mask) {
      feature_idx = i;
      break;
    }
  }
  if (i == num_features) {
    IDBG_ERROR("Error cant find feature in feature table");
    return NULL;
  }

  /* get feature library name from features table*/
  lib_name = module_qdc_common_get_feature_lib(feature_idx, feature_mask,
    &qdc_algo_flavor);
  if (lib_name == NULL) {
    return NULL;
  }

  /* Based on feature mask configure module caps and parameters*/
  switch (feature_mask) {
  case CAM_QTI_FEATURE_SAT: {
    property_get("persist.camera.sat.enable", prop, "0");
    if (!atoi(prop)) {
      return NULL;
    }
    IDBG_MED("FEATURE mask is CAM_QTI_FEATURE_SAT");
    module_qdc_common_sat_custom_session_config(&g_sat_caps, &g_params);
    p_g_caps = &g_sat_caps;
    break;
  }
  case CAM_QTI_FEATURE_SAC: {
    property_get("persist.camera.sac.enable", prop, "0");
    if (!atoi(prop)) {
      return NULL;
    }
    IDBG_MED("FEATURE mask is CAM_QTI_FEATURE_SAC");
    module_qdc_common_sac_custom_session_config(&g_sac_caps, &g_params);
    p_g_caps = &g_sac_caps;
    break;
  }
  case CAM_QTI_FEATURE_RTB: {
    property_get("persist.camera.rtb.enable", prop, "0");
    if (!atoi(prop)) {
      return NULL;
    }
    IDBG_MED("FEATURE mask is CAM_QTI_FEATURE_RTB");
    module_qdc_common_rtb_custom_session_config(&g_rtbdm_caps, &g_params);
    p_g_caps = &g_rtb_caps;
    break;
  }
  case CAM_QTI_FEATURE_RTBDM: {
    property_get("persist.camera.rtbdm.enable", prop, "0");
    if (!atoi(prop)) {
      return NULL;
    }
    IDBG_MED("FEATURE mask is CAM_QTI_FEATURE_RTBDM");
    module_qdc_common_rtbdm_custom_session_config(&g_rtbdm_caps, &g_params);
    p_g_caps = &g_rtbdm_caps;
    break;
  }
  default:
    IDBG_ERROR("Invalid feature mask");
    return NULL;
  }

  p_qdcc_mod = calloc(1, sizeof(img_qdc_common_module_t));
  if (!p_qdcc_mod) {
    IDBG_ERROR("%s:%d] Failed", __func__, __LINE__);
    return NULL;
  }
  pthread_mutex_init(&p_qdcc_mod->session_data->session_mutex, NULL);
  p_qdcc_mod->qdc_algo_flavor = qdc_algo_flavor;

  p_mct_mod = module_imgbase_init(name,
    IMG_COMP_MULTI_FRAME_PROC,
    "qti.multi_frameproc",
    p_qdcc_mod,
    p_g_caps,
    lib_name,
    feature_mask,
    &g_params);

  if ((p_mct_mod == NULL) || (p_mct_mod->module_private == NULL)) {
    IDBG_ERROR("mct module %p", p_mct_mod);
    if (p_mct_mod) {
      IDBG_ERROR("imgbase module %p", p_mct_mod->module_private);
    }
    goto module_init_failed;
  }

  p_mod = (module_imgbase_t *)p_mct_mod->module_private;
  rc = IMG_COMP_CREATE(&p_mod->core_ops, &p_mod->comp_ops);
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("comp create failed %d, failed to update capabilities", rc);
    goto comp_create_failed;
  }

  rc = IMG_COMP_GET_PARAM(&p_mod->comp_ops,
    QIMG_PARAM_ALGO_CAPS, (void *)(&p_mod->algo_caps));
  if (IMG_ERROR(rc)) {
    IDBG_ERROR("failed to get capabilities %d", rc);
    goto get_caps_failed;
  }

  IDBG_ERROR(" Algo Capabilities Crop caps %d,"
    "core type %d, access mode %d, exec mode %d",
    p_mod->algo_caps.crop_caps,
    p_mod->algo_caps.core_type,
    p_mod->algo_caps.buff_access_mode,
    p_mod->algo_caps.exec_mode);

  p_mod->modparams.access_mode = p_mod->algo_caps.buff_access_mode;
  p_mod->modparams.exec_mode = p_mod->algo_caps.exec_mode;

  return p_mct_mod;

get_caps_failed:
comp_create_failed:
  module_imgbase_deinit(p_mct_mod);

module_init_failed:
  pthread_mutex_destroy(&p_qdcc_mod->session_data->session_mutex);
  free(p_qdcc_mod);

  return NULL;
}


/**
 * Function: module_qdc_common_deinit
 *
 * Description: Function used to deinit QDC Common module
 *
 * Arguments:
 *   p_mct_mod - MCTL module instance pointer
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_qdc_common_deinit(mct_module_t *p_mct_mod)
{
  IDBG_MED(":E");
  module_imgbase_t *p_mod = NULL;
  img_qdc_common_module_t *p_qdcc_mod = NULL;

  p_mod = p_mct_mod->module_private;
  if (p_mod && p_mod->mod_private) {
    p_qdcc_mod = p_mod->mod_private;
    pthread_mutex_destroy(&p_qdcc_mod->session_data->session_mutex);
    free(p_qdcc_mod);
  }
  module_imgbase_deinit(p_mct_mod);
  IDBG_MED(":X");
}


/** module_qdc_common_set_parent:
 *
 * Description: This function is used to set the parent pointer
 *   of the QDC Common module
 *
 *  Arguments:
 *  @p_parent - parent module pointer
 *
 * Return values:
 *   none
 *
 * Notes: none
 **/
void module_qdc_common_set_parent(mct_module_t *p_mct_mod, mct_module_t *p_parent)
{
  return module_imgbase_set_parent(p_mct_mod, p_parent);
}

