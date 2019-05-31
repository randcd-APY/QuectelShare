/***************************************************************************
* Copyright (c) 2013-2017 Qualcomm Technologies, Inc.                      *
* All Rights Reserved.                                                     *
* Confidential and Proprietary - Qualcomm Technologies, Inc.               *
****************************************************************************/

#ifndef __IMG_META_H__
#define __IMG_META_H__

#include <unistd.h>
#include <math.h>
#include "img_common.h"
#include "img_thread_ops.h"
#include "img_mem_ops.h"
#include <stdbool.h>

/**
 * CONSTANTS and MACROS
 **/
/** IMG_MAX_FACES:
 *
 * Defines maximum supported faces
 *
 * Returns maximum supported faces
 **/
#define IMG_MAX_FACES 5
/** IMG_MAX_DCRF_FOV:
 *
 * Defines maximum FOVs needed by DCFR
 *
 * Returns maximum FOVs needed by DCFR
 **/
#define IMG_MAX_FOV 4

/** MAX_OPAQUE_DATA_SIZE:
 *
 * Opaque data size
 **/
#define MAX_OPAQUE_DATA_SIZE 128

/** IMG_MAX_HYST_REGIONS:
 *
 * Maximum number of hysteresis regions
 **/
#define IMG_MAX_HYST_REGIONS 4

/** img_stillmore_cfg_t
 *  @br_intensity: Controls amount of brightening. Range: 0 ~
 *                   1. Larger value means brighter output.
 *  @br_color: Controls amount of color after brightening.
 *             Range: 0 ~ 1. Larger value means more colorful
 *             output.
 *  @smoothing_intensity: Control amount of smoothing.
 *             Range: 0 ~ 1. Larger value means more smoother
 *             output.
 *
 *    Stillmore configuration
 **/
typedef struct {
  float br_intensity;
  float br_color;
  float smoothing_intensity;
} img_stillmore_cfg_t;

/** img_trigger_type_t:
 * @IMG_TRIGGER_INVALID: Trigger is invalid
 * @IMG_TRIGGER_LUX_IDX: Lux index based trigger
 * @IMG_TRIGGER_GAIN: Gain based trigger
 *
 * Enumerations of the trigger type
 */
typedef enum {
  IMG_TRIGGER_INVALID,
  IMG_TRIGGER_LUX_IDX,
  IMG_TRIGGER_GAIN,
} img_trigger_type_t;

/** img_seemore_trigger_t:
 *  @t_start: Start of the trigger
 *  @tnr_intensity: TNR intensity. Definition in seemore config
 *  @mot_det_sensitivity: Motion sensitivity. Definition in
 *                      seemore config
 *
 *  Seemore Trigger points and per region values
 */
typedef struct {
  float t_start;
  float tnr_intensity;
  float mot_det_sensitivity;
} img_seemore_trigger_t;

/** img_seemore_tuning_t:
 *  @type: trigger type
 *  @trigger: triggers for the regions
 *  @num_regions: number of regions. number of hysteresis points
 *              is num_regions - 1.
 *  @delta_hyst: delta hysteresis for all regions. End
 *                   trigger for each region can be calculated
 *                   by t_end = t_start + delta_hysteresis
 *
 *  Seemore tuning parameters
 */
typedef struct {
  img_trigger_type_t type;
  img_seemore_trigger_t trigger[IMG_MAX_HYST_REGIONS];
  uint32_t num_regions;
  float delta_hyst;
  bool tnr_custom_tuning_enable;
} img_seemore_tuning_t;

/** img_seemore_cfg_t
 *  @br_intensity: Controls amount of brightening. Range: 0 ~
 *                   1. Larger value means brighter output.
 *                   NOTE: only takes effect when
 *                   enableLTM == true.
 *  @br_color: Controls amount of color after brightening.
 *             Range: 0 ~ 1. Larger value means more colorful
 *             output. NOTE: only takes effect when
 *             enableLTM == true.
 *  @tnr_intensity:
 *             Controls amount of TNR. Range: 0 ~ 1.
 *             Larger value means more aggressive denoising.
 *             NOTE: only takes effect when enableTNR == true.
 *
 *  @mot_det_sensitivity:
 *            Controls sensitivity to motion. Range: 0 ~ 1.
 *            Larger value means more sensitive to motion
 *            NOTE: only takes effect when enableTNR == true.
 *
 *  @re_initialize: Flag to indicate whether the seemore needs
 *                to be restarted
 *
 *    Seemore configuration
 **/
typedef struct {
  float br_intensity;
  float br_color;
  bool enable_LTM;
  bool enable_TNR;
  float tnr_intensity;
  float mot_det_sensitivity;
  bool re_initialize;
} img_seemore_cfg_t;

/** paaf_sw_filter_type: SW stats filter type
 **/
typedef enum {
  PAAF_OFF = 0,
  PAAF_ON_IIR,
  PAAF_ON_FIR,
} paaf_sw_filter_type_t;

/** img_paaf_cfg_t
 *   @enable: Flag to enable/disable stats collection
 *   @frame_id: current frame id
 *   @roi: ROI of the stats collection..Tranlsated wrt
 *       processing stream.
 *   @coeffa: filterA coefficients
 *   @coeffb: filterB coefficients
 *   @coeff_len: length of coefficient table
 *   @sw_filter_type: SW filter type
 *   @FV_min
 *   @pixel_skip_cnt: Number of pixel to skip
 *   @cropped_window: Crop window
 *
 *  PAAF configuration
 **/
typedef struct {
  int8_t enable;
  int frame_id;
  img_rect_t roi;
  double coeffa[6];
  double coeffb[6];
  int coeff_fir[11];
  uint32_t coeff_len;
  paaf_sw_filter_type_t filter_type;
  double FV_min;
  int pixel_skip_cnt;
  img_rect_t cropped_roi;
} img_paaf_cfg_t;


/** img_paaf_result_t
 *  @frame_id: Id of the frame processed
 *  @fV: Focus value
 *
 *    Seemore configuration
 **/
typedef struct {
  int frame_id;
  double fV;
} img_paaf_result_t;

/** img_stats_status_t
*   @IMG_STATS_STATUS_VALID : Status (AF/AEC/AWB) status is
*                           valid
*   @IMG_STATS_STATUS_INVALID: Status (AF/AEC/AWB) status is
*                            invalid
*
*    Stats Status Info
**/
typedef enum {
  IMG_STATS_STATUS_INVALID = 0,
  IMG_STATS_STATUS_VALID,
} img_stats_status_t;

/** img_aec_info_t
*   @real_gain: real gain of the frame
*   @linecount: line count for the frame
*   @exp_time: exposure time
*   @lux_idx: lux index of the frame
*   @iso: ISO value
*   @hdr_gtm_gamma: gamma value for vhdr mode
*   @hdr_exp_time_ratio: exp time ratio for vhdr mode
*   @aec_status: AEC status
*   @l_linecount: long linecount
*   @s_linecount: short linecount
*
*    AEC info
**/
typedef struct {
  float real_gain;
  uint32_t linecount;
  float exp_time;
  float lux_idx;
  uint32_t iso;
  float hdr_gtm_gamma;
  float hdr_exp_time_ratio;
  img_stats_status_t aec_status;
  int32_t l_linecount;
  int32_t s_linecount;
} img_aec_info_t;

/** img_awb_info_t
*   @r_gain: Red channel gain
*   @g_gain: green channel gain
*   @b_gain: Blue channel gain
*   @color_temp: Color temperature
*   @ccm: Color Correction matrix
*   @ccm_size: Color Correction matrix size
*   @awb_status: AWB Status
*
*    AWB info
**/
typedef struct {
  float r_gain;
  float g_gain;
  float b_gain;
  uint32_t color_temp;
  float *ccm;
  int ccm_size;
  img_stats_status_t awb_status;
} img_awb_info_t;

/** img_af_info_t
*
*  @focused_roi: Focused AF ROI
*  @af_status: AF status
*  @lens_shift: Lens Position (um)
*  @object_distance: Estimated distance (cm)
*  @near_field: Nearest distance in the scene (cm)
*  @far_field: Farthest distance in the scene (cm)
*    AF info
**/
typedef struct {
  img_rect_t focused_roi;
  img_stats_status_t af_status;
  float lens_shift;
  float object_distance;
  float near_field;
  float far_field;
} img_af_info_t;

/** img_meta_type_t
*    IMG_META_R_GAMMA: R gamma table
*    IMG_META_G_GAMMA: G gamma table
*    IMG_META_B_GAMMA: B gamma table
*    IMG_META_AEC_INFO: AEC info
*    IMG_META_AWB_INFO: AWB info
*    IMG_META_AF_INFO: AF info
*    IMG_META_OUTPUT_ROI: Output ROI
*    IMG_META_ZOOM_FACTOR: zoom factor
*    IMG_META_NO_ROT_FD_INFO: face detect info before rotation
*    IMG_META_FD_INFO: face detect info
*    IMG_META_ROTATION: rotation
*    IMG_META_FLIP: snapshot flip
*    IMG_META_MISC_DATA: misc data
*    IMG_META_CHROMAFLASH_CTRL: ctrl chroma flash
*    IMG_META_NUM_INPUT: number of input bufs
*    IMG_META_TP_CONFIG: trueportrait config
*    IMG_META_DCRF_RUNTIME_PARAM: dcrf runtime param
*    IMG_META_DCRF_RESULT: dcrf result
*    IMG_META_SW2D_OPS: SW2D frameop
*    IMG_META_OPAQUE_DATE: opaque data
*    IMG_META_STILLMORE_CFG: Stillmore configuration
*    IMG_META_SEEMORE_CFG: Seemore configuration
*    IMG_META_PAAF_CFG: PAAF configuration
*    IMG_META_PAAF_RESULT: PAAF result
*    IMG_META_SPATIAL_ALIGN_COMPUTE_CFG: Spatial Alignment Compute cfg
*    IMG_META_SPATIAL_ALIGN_COMPUTE_RESULT: Spatial Alignment Compute Result
*    IMG_META_BOKEH_CFG: Realtime Bokeh cfg
*    IMG_META_BOKEH_RESULT: Realtime Bokeh Result
*    IMG_META_BOKEH_DEPTH_MAP_CFG: Realtime Bokeh Depth Map cfg
*    IMG_META_BOKEH_DEPTH_MAP_RESULT: Realtime Bokeh Depth Map Result
*    IMG_META_USER_ZOOM_FACTOR: lens specific zoom factor
*    IMG_META_FOV_CONFIG: FOV configuration
*    Meta type
**/
typedef enum {
  IMG_META_R_GAMMA,
  IMG_META_G_GAMMA,
  IMG_META_B_GAMMA,
  IMG_META_AEC_INFO,
  IMG_META_AWB_INFO,
  IMG_META_AF_INFO,
  IMG_META_OUTPUT_ROI,
  IMG_META_ZOOM_FACTOR,
  IMG_META_FD_INFO,
  IMG_META_NO_ROT_FD_INFO,
  IMG_META_ROTATION,
  IMG_META_FLIP,
  IMG_META_MISC_DATA,
  IMG_META_CHROMAFLASH_CTRL,
  IMG_META_NUM_INPUT,
  IMG_META_TP_CONFIG,
  IMG_META_DCRF_RUNTIME_PARAM,
  IMG_META_DCRF_RESULT,
  IMG_META_SW2D_OPS,
  IMG_META_OPAQUE_DATA,
  IMG_META_STILLMORE_CFG,
  IMG_META_SEEMORE_CFG,
  IMG_META_PAAF_CFG,
  IMG_META_PAAF_RESULT,
  IMG_META_SPATIAL_TRANSFORM_CFG,
  IMG_META_SPATIAL_TRANSFORM_RESULT,
  IMG_META_SPATIAL_ALIGN_COMPUTE_CFG,
  IMG_META_SPATIAL_ALIGN_COMPUTE_RESULT,
  IMG_META_BOKEH_CFG,
  IMG_META_BOKEH_RESULT,
  IMG_META_BOKEH_DEPTH_MAP_CFG,
  IMG_META_BOKEH_DEPTH_MAP_RESULT,
  IMG_META_USER_ZOOM_FACTOR,
  IMG_META_FOV_CONFIG,
  IMG_META_MAX,
} img_meta_type_t;

/** img_fd_info_t
*   @valid_faces_detected: valid faces detected
*   @fd_frame_width: fd frame width
*   @fd_frame_height: fd frame height
*   @valid_faces_detected: valid faces detected
*   @faceRollDir: face roll direction
*   @faceROIx: face start poistion x
*   @faceROIy: face start poistion y
*   @faceROIWidth: face width
*   @faceROIHeight: face height
*
*    face detection info
**/
typedef struct {
  uint32_t valid_faces_detected;
  uint32_t fd_frame_width;
  uint32_t fd_frame_height;
  int32_t faceRollDir[IMG_MAX_FACES];
  uint32_t faceROIx[IMG_MAX_FACES];
  uint32_t faceROIy[IMG_MAX_FACES];
  uint32_t faceROIWidth[IMG_MAX_FACES];
  uint32_t faceROIHeight[IMG_MAX_FACES];
} img_fd_info_t;

/** img_misc_t
*    @result: result of processing
*    @header_size: header size
*    @width: width
*    @height: height
*    @data: processing data
*
*    Imaging miscellaneous data structure
**/
typedef struct {
  uint32_t result;
  uint32_t header_size;
  uint32_t width;
  uint32_t height;
  uint8_t* data;
} img_misc_t;

/** img_chromaflash_ctrl_t
*    @deghost_enable: flag to enable/disable deghosting
*    @flash_weight: Controls contribution of flash input in the
*                 output. Larger value means more flash. Range:
*                 1 ~ 15.
*    @br_intensity: Controls amount of brightening.
*                Range: 0 ~ 1.
*                Larger value means brighter output.
*                  NOTE: 0 disables brightening entirely and
*                  reduces processing time
*
*    @contrast_enhancement: Controls amount of contrast
*               enhancement.
*               Range: 0 ~ 1
*               Larger value means better contrast output.
*               NOTE: 0 disables contrast enhancement and
*               reduces processing time
*
*    @sharpen_beta: Controls amount of sharpening.
*               Range: 0 ~ 1
*               NOTE: 0 means very less sharpening, 1 means
*               maximum sharpening
*
*    @br_color: Controls amount of color after brightening.
*              Range: 0 ~ 1.
*              Larger value means more colorful output.
*              NOTE: only takes effect when brightening is
*              enabled.
*
*    @max_amb_lux_idx: max lux index value of ambient frames
*             until which CF is applied
*
*    Parameters to control the chroma flash
**/
typedef struct {
  uint8_t deghost_enable;
  float flash_weight;
  float br_intensity;
  float contrast_enhancement;
  float sharpen_beta;
  float br_color;
  float max_amb_lux_idx;
} img_chromaflash_ctrl_t;

/** img_tp_effect_t
*    @TP_BOKEH: apply bokeh effect
*    @TP_HALO: apply halo effect
*    @TP_MOTIONBLUR: apply notion blur effect
*
*    TruePortrait effect types
**/
typedef enum {
  TP_BOKEH,
  TP_HALO,
  TP_MOTIONBLUR,
} img_tp_effect_t;

/** img_tp_config_t
*    @enable_bodymask: flag to enable bodymask
*    @enable_effects: flag to enable bodymask
*    @tp_effect: tp effect type
*    @intensity: intensity of the effect
*
*    Parameters for TruePortrait
**/
typedef struct {
  uint32_t enable_bodymask;
  uint32_t enable_effects;
  img_tp_effect_t effect;
  uint32_t intensity;
} img_tp_config_t;

/** img_rotation_t
*    @frame_rotation: frame rotation
*    @device_rotation: device rotation
*
*    rotation parameters
**/
typedef struct {
  int32_t frame_rotation;
  int32_t device_rotation;
} img_rotation_t;

/** img_fov_type_t
*    SENSOR_FOV: sensor FOV
*    ISPIF_FOV: ispif FOV
*    CAMIF_FOV: camif FOV
*    ISP_OUT_FOV: isp output FOV
*    STREAM_FUTURE_FOV: pending fov info
*
*    FOV type
**/
typedef enum {
  SENSOR_FOV = 0,
  ISPIF_FOV,
  CAMIF_FOV,
  ISP_OUT_FOV,
  STREAM_FUTURE_FOV
} img_fov_type_t;

/** img_fov_t
*    @input_width: Width of input window from which image is created
*    @input_height: Height of input window from which image is created
*    @offset_x: Offset in horizontel direction for fetching window wrt to input window
*    @offset_y: Offset in vertical direction for fetching window wrt to input window
*    @fetch_window_width: Width of the fetch(Crop) window
*    @fetch_window_height: Height of the fetch(Crop) window
*    @output_window_width: Output image width
*    @output_window_height: Output image heightt
*    @module: IMAGE_FOV_TYPE{ Sensor, CAMIF, ISPIF, ISP}
**/
typedef struct {
  uint32_t input_width;
  uint32_t input_height;
  uint32_t offset_x;
  uint32_t offset_y;
  uint32_t fetch_window_width;
  uint32_t fetch_window_height;
  uint32_t output_window_width;
  uint32_t output_window_height;
  img_fov_type_t module;
} img_fov_t;

/** img_dcrf_input_runtime_param_t
*    @lens_zoom_ratio:  Lens zoom ratio
*    @roi_of_main: ROI input params
*    @af_fps: AF FPS
*    @fov_params_main:  FOV information for Main sensor
*    @fov_params_aux: FOV information for Aux  sensor
**/
typedef struct {
  float lens_zoom_ratio;
  img_rect_t roi_of_main;
  uint32_t af_fps;
  img_fov_t fov_params_main[IMG_MAX_FOV];
  img_fov_t fov_params_aux[IMG_MAX_FOV];
} img_dcrf_input_runtime_param_t;

/** img_dcrf_output_result_t
 *    @frame_id: frame ID
 *    @timestamp: timestamp
 *    @distance_in_mm: distance in mm
 *    @confidence: result confidence
 *    @focused_roi: focused ROI
 *    @focused_x: focused x
 *    @focused_y: focused y
 *
 *    DCRF output result
 **/
typedef struct {
  uint32_t frame_id;
  uint64_t timestamp;
  uint32_t distance_in_mm;
  uint32_t confidence;
  uint32_t status;
  img_rect_t focused_roi;
  uint32_t focused_x;
  uint32_t focused_y;
} img_dcrf_output_result_t;

/** img_sw2d_ops_t
*    @SW2D_OPS_NONE: sw frame ops none
*    @SW2D_OPS_DOWNSCALE: downscale input frame
*    @SW2D_OPS_DEINTERLEAVE: deinterleave input frame
*
*    SW2D Frame Operations
**/
typedef enum {
  SW2D_OPS_NONE,
  SW2D_OPS_DOWNSCALE,
  SW2D_OPS_DEINTERLEAVE,
} img_sw2d_ops_t;

/** img_opaque_data_t
 *    @data: opaque data size
 *
 *    Opaque data for the 3rd party modules
 **/
typedef struct {
  uint8_t data[MAX_OPAQUE_DATA_SIZE];
} img_opaque_data_t;

/** img_fov_cfg_t
 *    @fov_params: fov config at different levels
 *
 *    fov configuration information
 **/
typedef struct {
  img_fov_t fov_params[STREAM_FUTURE_FOV + 1];
} img_fov_cfg_t;

typedef struct {
  float transform_matrix[(IMG_MAX_IS_NUM_VERTICES) * 9];
  uint32_t      num_mesh_x;
  uint32_t      num_mesh_y;
  uint32_t      last_frame;
} img_eis_data_t;

/** img_spatial_transform_cfg_t
 *    @fov_params: FOV settings for all modules
 *    @camera_role: Camera role
 *    @frameRate: Frame rate
 *    @low_power_mode: Low power mode enabled
 *    @is_master: Flag indicating if curent session is the
 *              master
 *
 *    Spatial Transform Config
 */
typedef struct {
  img_fov_t fov_params[IMG_MAX_FOV];
  img_camera_role_t camera_role;
  float frameRate;
  bool low_power_mode;
  bool is_master;
} img_spatial_transform_cfg_t;


 /** img_lpm_settings_t
 *    @camera_role: camera type of the chosen frame
 *    @low_power_mode: LPM enabled/disabled
 *
 *    Low Power mode settings
 */
typedef struct {
  img_camera_role_t camera_role;
  bool low_power_mode;
} img_lpm_cmd_t;

/** img_spatial_transform_result_t
 *    @is_camera_role_valid: flag to check if camera_role
 *      variable is set.
 *    @camera_role: camera type of the chosen frame.
 *    @is_recommended_role_valid: flag to check if
 *       recommended_role variable is set.
 *    @recommended_role: Indicates which frame is recommended by
 *      the lib. INVALID indicates no specific recommendation.
 *    @is_output_shift_valid: flag to check if output_shift
 *      variable is set.
 *    @output_shift: Shift of the output frame
 *    @ref_res_output_shift: reference resolution on
 *      which output shift was applied.
 *    @is_focus_roi_shift_valid: flag to check if
 *      focus_roi_shift variable is set
 *    @focus_roi_shift: Shift required for af roi.
 *    @ref_res_focus_roi_shift: reference resolution
 *       on which roi shift was applied.
 *    @is_output_crop_request_valid: flag to check if
 *      output_crop parameter is set
 *    @is_lpm_cmd_valid: flag to check if low_power_mode
 *      variable is set
 *    @low_power_mode: LPM enabled/disabled
 *
 *    Spatial alignment transform result
 */
typedef struct {
  bool is_camera_role_valid;
  img_camera_role_t camera_role;

  bool is_recommended_role_valid;
  img_camera_role_t recommended_role;

  bool is_output_shift_valid;
  img_pixel_t output_shift;
  img_size_t ref_res_output_shift;

  bool is_focus_roi_shift_valid;
  img_pixel_t focus_roi_shift;
  img_size_t ref_res_focus_roi_shift;

  bool is_output_crop_request_valid;
  img_rect_t output_crop;

  bool is_lpm_cmd_valid;
  img_lpm_cmd_t lpm_cmd;
} img_spatial_transform_result_t;

/** img_spatial_alignment_compute_cfg_t
 *    @fov_params: FOV settings for all modules
 *    @camera_role: Camera role
 *    @frameRate: Frame rate
 *    @low_power_mode: Low power mode enabled
 *    @is_master: Flag indicating if curent session is the
 *              master
 *
 *    Spatial alignment compute config
 */
typedef struct {
  img_fov_t fov_params[IMG_MAX_FOV];
  img_camera_role_t camera_role;
  float frameRate;
  bool low_power_mode;
  bool is_master;
} img_spatial_align_compute_cfg_t;

/** img_spatial_alignment_compute_result_t
 *    @is_camera_role_valid: flag to check if camera_role
 *      variable is set.
 *    @camera_role: camera type of the chosen frame.
 *    @is_recommended_role_valid: flag to check if
 *       recommended_role variable is set.
 *    @recommended_role: Indicates which frame is recommended by
 *      the lib. INVALID indicates no specific recommendation.
 *    @is_output_shift_valid: flag to check if output_shift
 *      variable is set.
 *    @output_shift: Shift of the output frame
 *    @ref_res_output_shift: reference resolution on
 *      which output shift was applied.
 *    @is_focus_roi_shift_valid: flag to check if
 *      focus_roi_shift variable is set
 *    @focus_roi_shift: Shift required for af roi.
 *    @ref_res_focus_roi_shift: reference resolution
 *       on which roi shift was applied.
 *    @is_lpm_cmd_valid: flag to check if low_power_mode
 *      variable is set
 *    @low_power_mode: LPM enabled/disabled
 *    @is_proj_transform_valid: flag to check of
 *      proj_transform_matrix variable is set.
 *    @proj_transform_matrix: tramsform matrix
 *    @is_alignmet_ready_flag_valid: flag to check of
 *      alignmet_ready_flag
 *    @alignmet_ready_flag: alignment ready flag
 *
 *    Spatial alignment compute result
 */
typedef struct {
  bool is_camera_role_valid;
  img_camera_role_t camera_role;

  bool is_recommended_role_valid;
  img_camera_role_t recommended_role;

  bool is_output_shift_valid;
  img_pixel_t output_shift;
  img_size_t ref_res_output_shift;

  bool is_focus_roi_shift_valid;
  img_pixel_t focus_roi_shift;
  img_size_t ref_res_focus_roi_shift;

  bool is_lpm_cmd_valid;
  img_lpm_cmd_t lpm_cmd;

  bool is_proj_transform_valid;
  float proj_transform_matrix[9];

  bool is_alignmet_ready_flag_valid;
  bool alignmet_ready_flag;
} img_spatial_align_compute_result_t;

/** img_realtime_bokeh_cfg_t
 *    @fov_params: FOV settings for all modules
 *    @camera_role: Camera role
 *    @is_master: Flag indicating if curent session is the
 *              master
 *
 *    Realtime bokeh config
 */
typedef struct {
  img_fov_t fov_params[IMG_MAX_FOV];
  img_camera_role_t camera_role;
  bool is_master;
} img_realtime_bokeh_cfg_t;

/** img_realtime_bokeh_result_t
 *    @is_camera_role_valid: flag to check if camera_role
 *      variable is set.
 *    @camera_role: camera type of the chosen frame.
 *
 *    Real time bokeh result
 */
typedef struct {
  bool is_camera_role_valid;
  img_camera_role_t camera_role;
} img_realtime_bokeh_result_t;

/** img_realtime_bokeh_depth_map_cfg_t
 *    @fov_params: FOV settings for all modules
 *    @camera_role: Camera role
 *    @is_master: Flag indicating if curent session is the
 *      master
 *
 *    Realtime bokeh depth map config
 */
typedef struct {
  img_fov_t fov_params[IMG_MAX_FOV];
  img_camera_role_t camera_role;
  bool is_master;
} img_realtime_bokeh_depth_map_cfg_t;

/** img_realtime_bokeh_depth_map_result_t
 *    @is_depthmap_ready_flag: flag to check if
 *      depth_map_ready_flag variable is set.
 *    @depth_map_ready_flag: depth map ready
 *
 *    Real time bokeh depth map result
 */
typedef struct {
  bool is_depthmap_ready_flag;
  bool depth_map_ready_flag;
} img_realtime_bokeh_depth_map_result_t;

/** img_dual_cam_common_cfg_t
 *    @sat_cfg: spatial alignment transform config
 *    @sac_cfg: spatial alignment compute config
 *    @rtb_cfg: realtime bokeh config
 *    @rtbdm_cfg: realtime bokeh depth map config
 *
 *    Dual camera common config union
 */
typedef union {
  img_spatial_transform_cfg_t sat_cfg;
  img_spatial_align_compute_cfg_t sac_cfg;
  img_realtime_bokeh_cfg_t rtb_cfg;
  img_realtime_bokeh_depth_map_cfg_t rtbdm_cfg;
} img_dual_cam_common_cfg_t;

/** img_dual_cam_common_result_t
 *    @sat_result: spatial alignment compute result
 *    @sac_result: spatial alignment compute result
 *    @rtb_result: realtime bokeh depth map result
 *    @rtbdm_result: realtime bokeh depth map result
 *
 *    Dual camera common result union
 */
typedef union {
  img_spatial_transform_result_t sat_result;
  img_spatial_align_compute_result_t sac_result;
  img_realtime_bokeh_result_t rtb_result;
  img_realtime_bokeh_depth_map_result_t rtbdm_result;
} img_dual_cam_common_result_t;

/** img_meta_t
*    @valid: check if meta is valid
*    @gamma_R: R gamma table
*    @gamma_G: G gamma table
*    @gamma_B: B gamma table
*    @aec_info: AEC information
*    @awb_info: AWB information
*    @af_info: AF information
*    @output_crop: output crop info
*    @zoom_factor: zoom factor 1x-6x, apps zoom factor
*    @user_zoom_factor: wide and tele lens specific zoom factor
*    @no_rot_fd_info: face detect info before rotation
*    @fd_info: face detect info
*    @rotation: rotation requested for snapshot frame
*    @snapshot_flip: snapshot flip
*    @misc_data: pointer to misc data buffer
*    @frame_id: frame id
*    @cf_ctrl: chroma flash control
*    @num_input: number of input bufs
*    @tp_config: trueportrait config
*    @dcrf_runtime_params: DCRF runtime params
*    @dcrf_result: DCRF result
*    @sw2d_operation: sw2d frame operation
*    @opaque_data: opaque data for 3rd party modules
*    @stillmore_cfg: Stillmore configuration
*    @seemore_cfg: Seemore configuration
*    @paaf_cfg: PAAF config
*    @paaf_result: PAAF result
*    @qdcc_cfg: QDC_COMMON Config
*    @qdcc_result: QDC_COMMON Result
*    @fov_cfg: common fov information
*    @userdata: Pointer to piggy back
*               userdata for async callback
*    Imaging metadata structure
**/
typedef struct {
  int valid[IMG_META_MAX];

  img_gamma_t gamma_R;
  img_gamma_t gamma_G;
  img_gamma_t gamma_B;
  img_aec_info_t aec_info;
  img_awb_info_t awb_info;
  img_af_info_t af_info;
  img_rect_t output_crop;
  float zoom_factor;
  float user_zoom_factor;
  img_fd_info_t no_rot_fd_info;
  img_fd_info_t fd_info;
  img_rotation_t rotation;
  int32_t snapshot_flip;
  img_misc_t misc_data;
  uint32_t frame_id;
  img_chromaflash_ctrl_t cf_ctrl;
  uint8_t num_input;
  img_tp_config_t tp_config;
  img_dcrf_input_runtime_param_t dcrf_runtime_params;
  img_dcrf_output_result_t dcrf_result;
  img_sw2d_ops_t sw2d_operation;
  img_opaque_data_t opaque_data;
  img_stillmore_cfg_t stillmore_cfg;
  img_seemore_cfg_t seemore_cfg;
  img_paaf_cfg_t paaf_cfg;
  img_paaf_result_t paaf_result;
  img_eis_data_t eis_cfg;
  img_dual_cam_common_cfg_t qdcc_cfg;
  img_dual_cam_common_result_t qdcc_result;
  img_fov_cfg_t fov_cfg;
  void *userdata;
} img_meta_t;

/** img_frame_bundle_t
 *   @p_input: input frame pointers
 *   @p_output: output frame pointers
 *   @p_meta: meta pointer
 *
 *   Imaging frame bundle
 **/
typedef struct {
  img_frame_t *p_input[IMG_MAX_INPUT_FRAME];
  img_frame_t *p_output[IMG_MAX_OUTPUT_FRAME];
  img_meta_t *p_meta[IMG_MAX_META_FRAME];
} img_frame_bundle_t;

/** img_msg_type_t
 *   @IMG_MSG_BUNDLE: frame bundle
 *   @IMG_MSG_FRAME: image frame
 *   @IMG_MSG_META: meta frame
 *   @IMG_MSG_WAIT_FOR_START: message to indicate that
 *                           component is waiting for start
 *                           signal
 *   @IMG_MSG_WAIT_FOR_ABORT: message to indicate that
 *                           component is waiting for abort
 *                           signal
 *   @IMG_MSG_KILL: message to indicate Kill
 *
 *   Imaging message type
 **/
typedef enum {
  IMG_MSG_BUNDLE,
  IMG_MSG_FRAME,
  IMG_MSG_META,
  IMG_MSG_WAIT_FOR_START,
  IMG_MSG_WAIT_FOR_ABORT,
  IMG_MSG_KILL,
  IMG_MSG_NONE,
} img_msg_type_t;

/** img_msg_t
 *   @p_input: input frame pointers
 *   @p_sender: pointer to the sender comp
 *   @p_output: output frame pointers
 *   @p_meta: meta pointer
 *
 *   Imaging message information
 **/
typedef struct {
  img_msg_type_t type;
  void *p_sender;
  union {
    img_frame_bundle_t bundle;
    img_frame_t *p_frame;
    img_meta_t *p_meta;
    uint32_t new_state;
  };
} img_msg_t;

/**
 * Function: img_get_meta
 *
 * Description: This macro is to get the meta value from
 *               metadata if present
 *
 * Arguments:
 *   @p_meta_data : meta buffer
 *   @meta_type: meta type
 *
 * Return values:
 *     meta buffer value pointer
 *
 * Notes: none
 **/
void *img_get_meta(void *p_meta_data, uint32_t meta_type);

/**
 * Function: img_set_meta
 *
 * Description: This macro is to set the meta value in metadata
 *
 * Arguments:
 *   @p_meta_data : meta buffer
 *   @meta_type: meta type
 *   @val: pointer to new value
 *
 * Returns standard image lib codes
 *
 * Notes: none
 **/
int32_t img_set_meta(void *p_meta_data, uint32_t meta_type, void* val);

/** img_dump_meta
 *    @img_meta: metadata handler
 *    @file_name: file name prefix
 *
 * Saves specified metadata
 *
 * Returns nothing
 **/
void img_dump_meta(void *p_meta_data, const char* file_name);

#endif //__IMG_META_H__
