/**********************************************************************
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#include "module_imgbase.h"
#include "chromatix_iot.h"
#include "chromatix_metadata.h"
#include "chromatix.h"

/**
 *  Static functions
 **/
static int32_t module_lcac_client_created(imgbase_client_t *p_client);
static int32_t module_lcac_client_streamon(imgbase_client_t * p_client);
static int32_t module_lcac_client_destroy(imgbase_client_t *p_client);
static int32_t module_lcac_client_process_done(imgbase_client_t *p_client,
  img_frame_t *p_frame);
static boolean module_lcac_client_set_chromatix(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static boolean module_lcac_client_query_mod(mct_pipeline_cap_t *p_mct_cap,
  void *p_mod, unsigned int sessionid);
static boolean module_lcac_handle_buffer_divert(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled);
static int32_t module_lcac_process_param(mct_event_control_parm_t *ctrl_parm,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops);



static bool mIsTuningReceived = false;

/** g_caps:
 *
 *  Set the capabilities for LCAC module
*/
static img_caps_t g_caps = {
  .num_input = 1,
  .num_output = 0,
  .num_meta = 1,
  .inplace_algo = 1,
  .num_release_buf = 0,
  .num_overlap = 0,
};

/** g_params:
 *
 *  imgbase parameters
 **/
static module_imgbase_params_t g_params = {
  .imgbase_query_mod = module_lcac_client_query_mod,
  .imgbase_client_created = module_lcac_client_created,
  .imgbase_client_streamon = module_lcac_client_streamon,
  .imgbase_client_destroy = module_lcac_client_destroy,
  .imgbase_client_process_done = module_lcac_client_process_done,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_SET_CHROMATIX_PTR] =
      module_lcac_client_set_chromatix,
  .imgbase_handle_module_event[MCT_EVENT_MODULE_BUF_DIVERT] =
      module_lcac_handle_buffer_divert,
  .imgbase_handle_ctrl_parm[CAM_INTF_META_LCAC_YUV] =
    module_lcac_process_param,
  .exec_mode = IMG_EXECUTION_SW,
  .access_mode = IMG_ACCESS_READ_WRITE,
};

typedef struct {
  bool disable_preview;
  uint8_t lcac_mode;
} img_lcac_session_data_t;


typedef struct {
  img_lcac_session_data_t *p_session_data;
  uint8_t ignore_lcac;
  uint8_t prev_lcac;
  imgbase_client_t *p_client;
} img_lcac_client_t;

typedef struct {
  img_lcac_session_data_t session_data[MAX_IMGLIB_SESSIONS];
} img_lcacmod_priv_t;

/**
 * Function: module_lcac_client_query_mod
 *
 * Description: This function is used to query the imgbase module
 * info
 *
 * Arguments:
 *   @p_mct_cap: pipeline capability
 *   @p_mod: Module pointer
 *   @session_id: Session id
 *
 * Return values:
 *    true/false
 *
 * Notes: none
 **/
static boolean module_lcac_client_query_mod(mct_pipeline_cap_t *p_mct_cap,
  void *p_mod, unsigned int session_id)
{
  IMG_UNUSED(session_id);
  mct_pipeline_pp_cap_t *pp_cap =  NULL;
  if (!p_mct_cap || !p_mod) {
    IDBG_ERROR("Invalid pointers p_mct_cap %p p_mod %p",
      p_mct_cap, p_mod);
    return FALSE;
  }
  pp_cap = &p_mct_cap->pp_cap;
  pp_cap->feature_mask |= CAM_QCOM_FEATURE_LCAC;

  return TRUE;
}

/**
 * Function: module_lcac_client_streamon
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
static int32_t module_lcac_client_streamon(imgbase_client_t * p_client)
{
  int rc = IMG_SUCCESS;
  int i, k, j;

  module_imgbase_t *p_mod =
    (module_imgbase_t *)p_client->p_mod;
  chromatix_LCAC_type *p_lcacmod_priv =
    (chromatix_LCAC_type *)p_mod->mod_private;
  FILE *cfg_fp = NULL;
  img_component_ops_t *p_comp = &p_client->comp;
  img_lib_config_t lib_config;

  if (p_lcacmod_priv) {
    if (!mIsTuningReceived) {
      cfg_fp = fopen("/data/misc/camera/lcac_cfg.txt", "rb");
      if(cfg_fp) {
        IDBG_INFO("Opened lcac_cfg.txt at /data/misc/camera/");
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->enable));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->peak_filter_kernel_size));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->local_chromaticity_threshold));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->saturated_luma));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->saturated_contrast));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->peak_threshold));
        fscanf(cfg_fp, "%d", &(p_lcacmod_priv->bleeding_threshold));
        for (i = 0; i < 8; i++) {
          fscanf(cfg_fp, "%d", &(p_lcacmod_priv->cac_correction_kernel[i]));
        }
        for (i = 0; i < 8; i++) {
          fscanf(cfg_fp, "%d", &(p_lcacmod_priv->contrast_threshold[i]));
        }
        for (i = 0; i < 8; i++) {
          fscanf(cfg_fp, "%d", &(p_lcacmod_priv->luma_threshold[i]));
        }
        for (k = 0; k < 2; k++) {
          for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
              fscanf(cfg_fp, "%d", &(p_lcacmod_priv->color_weight[k][i][j]));
            }
          }
        }
        fclose(cfg_fp);
      }
    }
    IDBG_INFO("enable: %d, peak_filter_size: %d, local_chrom_threshold:%d, saturated_luma:%d",
          p_lcacmod_priv->enable, p_lcacmod_priv->peak_filter_kernel_size,
              p_lcacmod_priv->local_chromaticity_threshold, p_lcacmod_priv->saturated_luma);
    lib_config.lib_param = IMG_ALGO_PRESTART;
    lib_config.lib_data = p_lcacmod_priv;
    if (p_comp->set_parm) {
        rc = IMG_COMP_SET_PARAM(p_comp, QIMG_PARAM_LIB_CONFIG,
          &lib_config);
        if (!IMG_SUCCEEDED(rc)) {
            IDBG_ERROR("%s %d: Set lib param for tuning value ptr "
             "setting was not successful", __func__, __LINE__);
        }
    } else {
        IDBG_ERROR("%s: p_comp->setparm is null",__func__);
    }
  }

  return rc;
}

/**
 * Function: module_lcac_process_param
 *
 * Description: Function used to handle control
 * param events
 *
 * Arguments:
 *   ctrl_parm - pointer to control param
 *   p_client - pointer to client
 *   p_core_ops - pointer to imgbase module ops
 *
 * Return values:
 *     IMG_SUCCESS/IMG_ERR_INVALID_INPUT
 *
 * Notes: none
 **/
static int32_t module_lcac_process_param(mct_event_control_parm_t *ctrl_parm,
  imgbase_client_t *p_client, img_core_ops_t *p_core_ops)
{
  int rc = IMG_SUCCESS;
  img_lcac_client_t *p_lcac_client;
  img_lcac_session_data_t *p_session_data;

  if (!ctrl_parm || !p_client || !p_core_ops) {
    rc = IMG_ERR_INVALID_INPUT;
    IDBG_ERROR("Invalid Input!!!");
    return rc;
  }

  p_lcac_client = (img_lcac_client_t *)p_client->p_private_data;
  if (!p_lcac_client) {
    rc = IMG_ERR_INVALID_INPUT;
    IDBG_ERROR("Invalid llvd client!!!");
    return rc;
  }

  p_session_data = p_lcac_client->p_session_data;
  if (!p_session_data) {
    rc = IMG_ERR_INVALID_INPUT;
    IDBG_ERROR("Invalid session handle %d",p_client->session_id);
    return rc;
  }

  switch (ctrl_parm->type) {
    case CAM_INTF_META_LCAC_YUV: {
      if (p_lcac_client->ignore_lcac) {
        IDBG_MED("LCAC mode Ignored");
        break;
      }
      p_session_data->lcac_mode = *((cam_lac_yuv_mode *)ctrl_parm->parm_data);
      IDBG_MED("Updated LCAC Mode %d", p_session_data->lcac_mode);
      if ( CAM_LCAC_YUV_MODE_ON ==
        p_session_data->lcac_mode) {
        p_client->processing_disabled = FALSE;
        IDBG_ERROR("LCAC Enabled!!!");
      } else {
        if (p_lcac_client->prev_lcac==
          CAM_LCAC_YUV_MODE_ON) {
          IDBG_MED("LCAC mode disabled, flush triggered");
          module_imgbase_client_flush(p_client);
        }
        p_client->processing_disabled = TRUE;
        IDBG_MED("LCAC Disabled!!!");
      }
      p_lcac_client->prev_lcac= p_session_data->lcac_mode;
      break;
    }
    default:
      IDBG_MED("default case, type %d", ctrl_parm->type);
      break;
  }
  return rc;
}


/**
 * Function: module_lcac_client_set_chromatix
 *
 * Description: This function handles module event
 * for setting chromatix ptr
 *
 * Arguments:
 *   @identity: identity of the event
 *   @p_mod_event - pointer of module event
 *   @p_client - pointer to imgbase client
 *   @p_core_ops - pointer to imgbase module ops
 *   @is_evt_handled: output data to check if the event is
 *                  handled or not.
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
static boolean module_lcac_client_set_chromatix(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  modulesChromatix_t *chromatix_param =
    (modulesChromatix_t*)p_mod_event->module_event_data;
  module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;
  chromatix_LCAC_type *p_lcacmod_priv =
    (chromatix_LCAC_type *)p_mod->mod_private;

  if (p_mod->feature_mask == CAM_QCOM_FEATURE_LCAC &&
          chromatix_param) {
    chromatix_LCAC_type *p_tuning_ptr =
          (chromatix_LCAC_type *)&(((chromatix_iot_parms_type *)chromatix_param
                                    ->chromatixPostProcPtr)->chromatix_LCAC_data);
    memcpy(p_lcacmod_priv, p_tuning_ptr, sizeof(chromatix_LCAC_type));
  }
  *is_evt_handled=true;
  mIsTuningReceived = true;
  return true;
}

/**
 * Function: module_lcac_handle_buffer_divert
 *
 * Description: Function used to handle buffer divert
 * module event to process frames based on noise
 * reduction mode.
 *
 * Arguments:
 *   @identity: identity of the event
 *   @p_mod_event - pointer of module event
 *   @p_client - pointer to imgbase client
 *   @p_core_ops - pointer to imgbase module ops
 *   @is_evt_handled: output data to check if the event is
 *                  handled or not.
 *
 * Return values:
 *     TRUE/FALSE
 *
 * Notes: none
 **/
static boolean module_lcac_handle_buffer_divert(uint32_t identity,
  mct_event_module_t *p_mod_event, imgbase_client_t *p_client,
  img_core_ops_t *p_core_ops, boolean *is_evt_handled)
{
  module_imgbase_t *p_mod;
  img_lcac_client_t *p_lcac_client;
  img_lcac_session_data_t *p_session_data;
  uint8_t lcac_mode;
  isp_buf_divert_t *p_buf_divert;

  if (!p_mod_event || !p_client || !p_core_ops) {
    IDBG_ERROR("%s:%d] Error input", __func__, __LINE__);
    return FALSE;
  }

  p_mod = (module_imgbase_t *)p_client->p_mod;
  p_lcac_client = (img_lcac_client_t *)p_client->p_private_data;
  p_buf_divert = (isp_buf_divert_t *)p_mod_event->module_event_data;
  if (!p_mod || !p_lcac_client || !p_buf_divert) {
    IDBG_ERROR("Invalid module or client or event!!!");
    return FALSE;
  }

  p_session_data = p_lcac_client->p_session_data;
  if (!p_session_data) {
    IDBG_ERROR("Invalid session handle %d",p_client->session_id);
    return FALSE;
  }

  // handles only noise reduction part,  rest buffer handling in imgbase
  *is_evt_handled = FALSE;

  // ignore bayer buffer processing, return inplace ack
  if(p_buf_divert->bayerdata == TRUE) {
    p_buf_divert->ack_flag = TRUE;
    *is_evt_handled = TRUE;
    return FALSE;
  }

  //handle noise reduction mode incase of LE and HAL3
  if (CAM_HAL_V3 != p_mod->hal_version || p_lcac_client->ignore_lcac) {
    IDBG_MED("hal_version %d ignore_lcac %d",
      p_mod->hal_version, p_lcac_client->ignore_lcac);
    return TRUE;
  }

  //get noise reduction mode
  lcac_mode = p_session_data->lcac_mode;
  IDBG_MED("lcac_mode %d", lcac_mode);

  if (CAM_LCAC_YUV_MODE_ON == lcac_mode) {
    p_client->processing_disabled = FALSE;
  } else {
    if (p_lcac_client->prev_lcac ==
      CAM_LCAC_YUV_MODE_ON) {
      IDBG_MED("LCAC mode disabled, flush triggered");
      module_imgbase_client_flush(p_client);
    }
    p_client->processing_disabled = TRUE;
  }
  p_lcac_client->prev_lcac= lcac_mode;
  return TRUE;
}


/**
 * Function: module_lcac_client_created
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
int32_t module_lcac_client_created(imgbase_client_t *p_client)
{
  int i;
  IDBG_MED("%s:%d: E", __func__, __LINE__);
  bool disable_preview = 0;
  img_lcac_client_t *p_lcac_client;
  module_imgbase_t *p_mod =
    (module_imgbase_t *)p_client->p_mod;
  img_lcacmod_priv_t *p_lcacmod_priv =
    (img_lcacmod_priv_t *)p_mod->mod_private;

 /* alloc llvd client private data */
  p_lcac_client = calloc(1, sizeof(img_lcac_client_t));
  if (!p_lcac_client) {
    IDBG_ERROR("%s:%d] lcac client data alloc failed", __func__, __LINE__);
    return IMG_ERR_NO_MEMORY;
  }

  /* update llvd client priv data */
  p_lcac_client->p_session_data =
    &p_lcacmod_priv->session_data[p_client->session_id - 1];
  p_client->p_private_data = p_lcac_client;
  p_lcac_client->p_client = p_client;

  disable_preview =
    p_lcacmod_priv->session_data[p_client->session_id - 1].disable_preview;

#ifdef _LE_CAMERA_
  p_lcac_client->ignore_lcac= FALSE;
  p_client->processing_disabled = TRUE;
#else
  p_lcac_client->ignore_lcac= TRUE;
  p_client->processing_disabled = FALSE;
#endif

  IDBG_ERROR("ignore_lcac %d processing_disabled %d",
    p_lcac_client->ignore_lcac, p_client->processing_disabled);

  p_client->rate_control = TRUE;
  p_client->exp_frame_delay = 0LL;
  p_client->ion_fd = open("/dev/ion", O_RDONLY);
  p_client->before_cpp = TRUE;
  p_client->feature_mask = CAM_QCOM_FEATURE_LCAC;
  p_client->processing_disabled = TRUE;
  p_client->streams_to_process = 1 << CAM_STREAM_TYPE_VIDEO;
  p_client->process_all_frames = TRUE;
  p_client->output_stream_mask = 0;
  for(i = 0; i < CAM_STREAM_TYPE_MAX; i++) {
    if (CAM_STREAM_TYPE_ANALYSIS == i)
      continue;
    p_client->output_stream_mask |= 1 << i;
  }
  IDBG_INFO("output_stream_mask %x", p_client->output_stream_mask);

  return IMG_SUCCESS;
}

/**
 * Function: module_lcac_client_process_done
 *
 * Description: function called after frame is processed
 *
 * Arguments:
 *   @p_client - IMGLIB_BASE client
 *   @p_frame: output frame
 *
 * Return values:
 *   IMG_SUCCESS
 *   IMG_ERR_GENERAL
 *
 * Notes: none
 **/
int32_t module_lcac_client_process_done(imgbase_client_t *p_client,
  img_frame_t *p_frame)
{
  module_imgbase_t *p_mod = (module_imgbase_t *)p_client->p_mod;
  int rc = IMG_SUCCESS;

  if (p_mod->modparams.force_cache_op == TRUE) {
    rc = img_cache_ops_external(IMG_ADDR(p_frame),
      IMG_FRAME_LEN(p_frame), 0, IMG_FD(p_frame),
      IMG_CACHE_CLEAN_INV, p_mod->ion_fd);

    if (rc == IMG_SUCCESS) {
      imgbase_buf_t *p_imgbase_buf = p_frame->private_data;
      p_imgbase_buf->buf_divert.buffer_access = 0;
    }
  }

  return rc;
}

/**
 * Function: module_lcac_client_destroy
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
int32_t module_lcac_client_destroy(imgbase_client_t *p_client)
{
  IDBG_MED("%s:%d: E", __func__, __LINE__);

  if (p_client->ion_fd >= 0) {
    close(p_client->ion_fd);
    p_client->ion_fd = -1;
  }

  return IMG_SUCCESS;
}

/**
 * Function: module_lcac_deinit
 *
 * Description: Function used to deinit LCAC module
 *
 * Arguments:
 *   p_mct_mod - MCTL module instance pointer
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_lcac_deinit(mct_module_t *p_mct_mod)
{
  module_imgbase_t *p_mod;
  chromatix_LCAC_type *p_lcacmod_priv;

  p_mod = (module_imgbase_t *)p_mct_mod->module_private;
  if (!p_mod) {
    IDBG_ERROR("%s:%d] Invalid base module %p", __func__, __LINE__, p_mod);
    return;
  }
  p_lcacmod_priv = p_mod->mod_private;
  if (p_lcacmod_priv) {
    free(p_lcacmod_priv);
  }

  mIsTuningReceived = false;

  module_imgbase_deinit(p_mct_mod);
}

/** module_lcac_init:
 *
 *  Arguments:
 *  @name - name of the module
 *
 * Description: Function used to initialize the LCAC module
 *
 * Return values:
 *     MCTL module instance pointer
 *
 * Notes: none
 **/
mct_module_t *module_lcac_init(const char *name)
{
  chromatix_LCAC_type *p_lcacmod_priv = calloc(1, sizeof(chromatix_LCAC_type));
  if (!p_lcacmod_priv) {
    IDBG_ERROR("%s:%d] lcac private data alloc failed!!!", __func__, __LINE__);
    return NULL;
  }
  return module_imgbase_init(name,
    IMG_COMP_GEN_FRAME_PROC,
    "qcom.gen_frameproc",
    p_lcacmod_priv,
    &g_caps,
    "libmmcamera_lcac.so",
    CAM_QCOM_FEATURE_LCAC,
    &g_params);
}

/** module_lcac_set_parent:
 *
 *  Arguments:
 *  @p_parent - parent module pointer
 *
 * Description: This function is used to set the parent pointer
 *              of the LCAC module
 *
 * Return values:
 *     none
 *
 * Notes: none
 **/
void module_lcac_set_parent(mct_module_t *p_mct_mod, mct_module_t *p_parent)
{
  return module_imgbase_set_parent(p_mct_mod, p_parent);
}
