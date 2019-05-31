/* s5k3m2xm_lib.c
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */


#include <stdio.h>
#include "sensor_lib.h"
#include "sensor.h"
#include "s5k3m2xm_lib.h"
#include "pdaf_api.h"
#include "pdaf_camif_api.h"

#define SENSOR_MODEL "s5k3m2xm"
#define SENSOR_BLACK_LEVEL 64
#define SENSOR_PDAF3_X_OFFSET 24
#define SENSOR_PDAF3_Y_OFFSET 24
#define FULL_WIDTH            4208
#define FULL_HEIGHT           3120

/*Enable this for supporting pdaf flip & mirror*/
//#define FLIP_MIRROR


PD_CAMIF_HANDLE              camif_handle = 0;
PD_HANDLE                       pd_handle = 0;

static sensor_lib_t sensor_lib_ptr;

static struct msm_sensor_power_setting power_setting[] = {
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VANA,
    .config_val = 0,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VDIG,
    .config_val = 0,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VIO,
    .config_val = 0,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VAF,
    .config_val = 0,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_CLK,
    .seq_val = SENSOR_CAM_MCLK,
    .config_val = 24000000,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_HIGH,
    .delay = 10,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_HIGH,
    .delay = 10,
  },
  {
    .seq_type = SENSOR_I2C_MUX,
    .seq_val = 0,
    .config_val = 0,
    .delay = 0,
  },
};

static struct msm_camera_sensor_slave_info sensor_slave_info = {
  /* Camera slot where this camera is mounted */
  .camera_id = CAMERA_0,
  /* sensor slave address */
  .slave_addr = 0x5A,
  /* sensor address type */
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  /* sensor id info*/
  .sensor_id_info = {
    /* sensor id register address */
    .sensor_id_reg_addr = 0x0000,
    /* sensor id */
    .sensor_id = 0x30D2,
  },
  /* power up / down setting */
  .power_setting_array = {
    .power_setting = power_setting,
    .size = ARRAY_SIZE(power_setting),
  },
};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = CAMERA_MODE_2D_B,
  .position = BACK_CAMERA_B,
  .sensor_mount_angle = SENSOR_MOUNTANGLE_360,
};

static sensor_output_t sensor_output = {
  .output_format = SENSOR_BAYER,
  .connection_mode = SENSOR_MIPI_CSI,
  .raw_output = SENSOR_10_BIT_DIRECT,
};

static struct msm_sensor_output_reg_addr_t output_reg_addr = {
  .x_output = 0x034C,
  .y_output = 0x034E,
  .line_length_pclk = 0x0342,
  .frame_length_lines = 0x0340,
};

static struct msm_sensor_exp_gain_info_t exp_gain_info = {
  .coarse_int_time_addr = 0x0202,
  .global_gain_addr = 0x0204,
  .vert_offset = S5K3M2XM_MAX_INTEGRATION_MARGIN,
};

static sensor_manual_exposure_info_t manual_exp_info = {
  .min_exposure_time = 10322,/*in nano sec = 1line*/
  .max_exposure_time = 86000000000,/*in nano sec = FFFF lines*/
  .min_iso = 100,
  .max_iso = 800,
};

static sensor_aec_data_t aec_info = {
  .max_gain = 8.0,
  .max_linecount = 65535 - S5K3M2XM_MAX_INTEGRATION_MARGIN,
};

static sensor_lens_info_t default_lens_info = {
  .focal_length = 3.68,
  .pix_size = 1.12,
  .f_number = 2.2,
  .total_f_dist = 1.2,
  .hor_view_angle = 77.1,
  .ver_view_angle = 77.1,
  .near_end_distance = 10,/*in cm*/
};

static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x4320,
  .csi_lane_mask = 0x1F,
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 0,
};

static struct msm_camera_i2c_reg_setting init_reg_setting[] = {
  {
    .reg_setting = init0_reg_array,
    .size = ARRAY_SIZE(init0_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 20,
  },

};
static struct sensor_lib_reg_settings_array init_settings_array = {
  .reg_settings = init_reg_setting,
  .size =ARRAY_SIZE(init_reg_setting),
};

static struct msm_camera_i2c_reg_array start_reg_array[] = {
  {0x6028, 0x4000},
  {0x602A, 0x0100},
#ifndef FLIP_MIRROR
  {0x6F12, 0x0100},
#else
  {0x6F12, 0x0103},
#endif
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_WORD_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
  {0x6028, 0x4000},
  {0x602A, 0x0100},
  {0x6F12, 0x0000},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_WORD_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupon_reg_array[] = {
  {0x0104, 0x01},
};

static struct msm_camera_i2c_reg_setting groupon_settings = {
  .reg_setting = groupon_reg_array,
  .size = ARRAY_SIZE(groupon_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupoff_reg_array[] = {
  {0x0104, 0x00},
};

static struct msm_camera_i2c_reg_setting groupoff_settings = {
  .reg_setting = groupoff_reg_array,
  .size = ARRAY_SIZE(groupoff_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_csid_vc_cfg s5k3m2xm_cid_cfg[] = {
  {0, CSI_RAW10, CSI_DECODE_10BIT},
  {1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params s5k3m2xm_csi_params = {
  .csid_params = {
    .lane_cnt = 4,
    .lut_params = {
      .num_cid = ARRAY_SIZE(s5k3m2xm_cid_cfg),
      .vc_cfg = {
         &s5k3m2xm_cid_cfg[0],
         &s5k3m2xm_cid_cfg[1],
      },
    },
  },
  .csiphy_params = {
    .lane_cnt = 4,
    .settle_cnt = 0x1B,
  },
};

static struct msm_camera_csi2_params *csi_params[] = {
  &s5k3m2xm_csi_params, /* RES 0*/
  &s5k3m2xm_csi_params, /* RES 1*/
  &s5k3m2xm_csi_params, /* RES 2*/
  &s5k3m2xm_csi_params, /* RES 3*/
  &s5k3m2xm_csi_params, /* RES 4*/
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
};

static struct sensor_pix_fmt_info_t s5k3m2xm_pix_fmt0_fourcc[] = {
  {V4L2_PIX_FMT_SGRBG10},
};

static struct sensor_pix_fmt_info_t s5k3m2xm_pix_fmt1_fourcc[] = {
  {MSM_V4L2_PIX_FMT_META},
};

static sensor_stream_info_t s5k3m2xm_stream_info[] = {
  {1, &s5k3m2xm_cid_cfg[0], s5k3m2xm_pix_fmt0_fourcc},
  {1, &s5k3m2xm_cid_cfg[1], s5k3m2xm_pix_fmt1_fourcc},
};

static sensor_stream_info_array_t s5k3m2xm_stream_info_array = {
  .sensor_stream_info = s5k3m2xm_stream_info,
  .size = ARRAY_SIZE(s5k3m2xm_stream_info),
};

static struct msm_camera_i2c_reg_setting res_settings[] = {
  {
    .reg_setting = res0_reg_array,
    .size = ARRAY_SIZE(res0_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 10,
  },
  {
    .reg_setting = res1_reg_array,
    .size = ARRAY_SIZE(res1_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 10,
  },
  {
    .reg_setting = res2_reg_array,
    .size = ARRAY_SIZE(res2_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 10,
  },
  {
    .reg_setting = res3_reg_array,
    .size = ARRAY_SIZE(res3_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 10,
  },
  {
    .reg_setting = res4_reg_array,
    .size = ARRAY_SIZE(res4_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 10,
  },
};

static struct sensor_lib_reg_settings_array res_settings_array = {
  .reg_settings = res_settings,
  .size = ARRAY_SIZE(res_settings),
};

static struct sensor_crop_parms_t crop_params[] = {
  {0, 0, 0, 0}, /* RES 0 */
  {0, 0, 0, 0}, /* RES 1 */
  {0, 0, 0, 0}, /* RES 2 */
  {0, 0, 0, 0}, /* RES 3 */
  {0, 0, 0, 0}, /* RES 4 */
};

static struct sensor_lib_crop_params_array crop_params_array = {
  .crop_params = crop_params,
  .size = ARRAY_SIZE(crop_params),
};

static struct sensor_lib_out_info_t sensor_out_info[] = {
  {/* 30 fps full size settings */
    .x_output = 4208,
    .y_output = 3120,
    .line_length_pclk = 4592,
    .frame_length_lines = 3188,
    .vt_pixel_clk = 440000000,
    .op_pixel_clk = 432000000,
    .binning_factor = 1,
    .max_fps = 30.056,
    .min_fps = 7.5,
    .mode = SENSOR_DEFAULT_MODE,
    .is_pdaf_supported = 1,
  },
  {/* 30 fps 4:3 */
    .x_output = 2104,
    .y_output = 1560,
    .line_length_pclk = 4592,
    .frame_length_lines = 3188,
    .vt_pixel_clk = 440000000,
    .op_pixel_clk = 432000000,
    .binning_factor = 1,
    .max_fps = 30.056,
    .min_fps = 7.5,
    .mode = SENSOR_DEFAULT_MODE,
    .is_pdaf_supported = 0,
  },
  {/* 60 fps 1080p*/
    .x_output = 1920,
    .y_output = 1080,
    .line_length_pclk = 4592,
    .frame_length_lines = 1596,
    .vt_pixel_clk = 440000000,
    .op_pixel_clk = 560000000,
    .binning_factor = 1,
    .max_fps = 60,
    .min_fps = 7.5,
    .mode = SENSOR_HFR_MODE,
    .is_pdaf_supported = 0,
  },
  {/* 90 fps 720p */
    .x_output = 1280,
    .y_output = 720,
    .line_length_pclk = 4592,
    .frame_length_lines = 1064,
    .vt_pixel_clk = 440000000,
    .op_pixel_clk = 560000000,
    .binning_factor = 1,
    .max_fps = 90,
    .min_fps = 7.5,
    .mode = SENSOR_HFR_MODE,
    .is_pdaf_supported = 0,
  },
  {/* 120 fps 720p */
    .x_output = 1280,
    .y_output = 720,
    .line_length_pclk = 4592,
    .frame_length_lines = 798,
    .vt_pixel_clk = 440000000,
    .op_pixel_clk = 560000000,
    .binning_factor = 1,
    .max_fps = 120,
    .min_fps = 7.5,
    .mode = SENSOR_HFR_MODE,
    .is_pdaf_supported = 0,
  },

};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t s5k3m2xm_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t s5k3m2xm_res_table = {
  .res_cfg_type = s5k3m2xm_res_cfg,
  .size = ARRAY_SIZE(s5k3m2xm_res_cfg),
};

static struct sensor_lib_chromatix_t s5k3m2xm_chromatix[] = {
  {
    .common_chromatix = S5K3M2XM_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = S5K3M2XM_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camera_snapshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camcorder_chromatix = S5K3M2XM_LOAD_CHROMATIX(default_video), /* RES0 */
    .liveshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(liveshot), /* RES0 */
  },
  {
    .common_chromatix = S5K3M2XM_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = S5K3M2XM_LOAD_CHROMATIX(preview), /* RES1 */
    .camera_snapshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(preview), /* RES1 */
    .camcorder_chromatix = S5K3M2XM_LOAD_CHROMATIX(default_video), /* RES1 */
    .liveshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(liveshot), /* RES1 */
  },
  {
    .common_chromatix = S5K3M2XM_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_60), /* RES2 */
    .camera_snapshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_60), /* RES2 */
    .camcorder_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_60), /* RES2 */
    .liveshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(liveshot), /* RES2 */
  },
  {
    .common_chromatix = S5K3M2XM_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_90), /* RES3 */
    .camera_snapshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_90), /* RES3 */
    .camcorder_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_90), /* RES3 */
    .liveshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(liveshot), /* RES3 */
  },
  {
    .common_chromatix = S5K3M2XM_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_120), /* RES4 */
    .camera_snapshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_120), /* RES4 */
    .camcorder_chromatix = S5K3M2XM_LOAD_CHROMATIX(hfr_120), /* RES4 */
    .liveshot_chromatix = S5K3M2XM_LOAD_CHROMATIX(liveshot), /* RES4 */
  },
};

static struct sensor_lib_chromatix_array s5k3m2xm_lib_chromatix_array = {
  .sensor_lib_chromatix = s5k3m2xm_chromatix,
  .size = ARRAY_SIZE(s5k3m2xm_chromatix),
};

/*===========================================================================
 * FUNCTION    - s5k3m2xm_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static uint16_t s5k3m2xm_real_to_register_gain(float gain)
{
  uint32_t reg_gain = 0;

  if (gain < S5K3M2XM_MIN_GAIN)
  {
      gain = S5K3M2XM_MIN_GAIN;
  }
  else if (gain > S5K3M2XM_MAX_GAIN)
  {
      gain = S5K3M2XM_MAX_GAIN;
  }

  reg_gain = (uint32_t)(gain * 32);

  return reg_gain;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static float s5k3m2xm_register_to_real_gain(uint16_t reg_gain)
{
  float gain;

  if(reg_gain > S5K3M2XM_MAX_GAIN_REG_VAL)
      reg_gain = S5K3M2XM_MAX_GAIN_REG_VAL;

  gain = (float) reg_gain / 32;

  return gain;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_calculate_exposure -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t s5k3m2xm_calculate_exposure(float real_gain,
  uint16_t line_count, sensor_exposure_info_t *exp_info)
{
  if (!exp_info)
  {
    return -1;
  }

  exp_info->reg_gain              = s5k3m2xm_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain      = s5k3m2xm_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain          = real_gain / exp_info->sensor_real_gain;
  exp_info->line_count            = line_count;
  exp_info->sensor_digital_gain   = 0x01;

  return 0;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t s5k3m2xm_fill_exposure_array(uint16_t gain,
  uint32_t line, uint32_t fl_lines, int32_t luma_avg, uint32_t fgain,
  struct msm_camera_i2c_reg_setting *reg_setting)
{
  uint16_t i = 0;
  uint16_t reg_count = 0;

  if (!reg_setting)
  {
    return -1;
  }

  for (i = 0; i < sensor_lib_ptr.groupon_settings->size; i++)
  {
      reg_setting->reg_setting[reg_count].reg_addr =
        sensor_lib_ptr.groupon_settings->reg_setting[i].reg_addr;
      reg_setting->reg_setting[reg_count].reg_data =
        sensor_lib_ptr.groupon_settings->reg_setting[i].reg_data;
      reg_count = reg_count + 1;
  }

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr->frame_length_lines;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr->frame_length_lines + 1;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF);
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF00) >> 8;
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF);
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF00) >> 8;
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr +  1;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF);
  reg_setting->reg_setting[reg_count].delay = 0;
  reg_count = reg_count + 1;

  for (i = 0; i < sensor_lib_ptr.groupoff_settings->size; i++)
  {
      reg_setting->reg_setting[reg_count].reg_addr =
        sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_addr;
      reg_setting->reg_setting[reg_count].reg_data =
        sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_data;
      reg_count = reg_count + 1;
  }

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;

  return 0;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_init_pdaf -
 *
 * DESCRIPTION:
 *==========================================================================*/
unsigned int s5k3m2xm_init_pdaf(void *arg1)
{
  pdaf_init_info_t              *s_pdaf = (pdaf_init_info_t *)arg1;
  pdaf_camif_init_param_t       input_camif;
  pdaf_init_param_t             input;
  struct sensor_lib_out_info_t  out_info;
  pdaf_calibration_param_t      *pdaf_1d_gain;
  pdaf_lib_version_t            version;
  pdaf_camif_lib_version_t      camif_lib_version;
  pdaf_sensor_orientation_t     sensor_orientation;
  PDAF_RET_t                    rc;

  SLOW("Enter");
  if(s_pdaf == NULL){
   SERR("s_pdaf is NULL");
   return SENSOR_FAILURE;
  }

  SLOW("PDAF calculate defocus, cur_res:%d",s_pdaf->cur_res);
  out_info = sensor_lib_ptr.out_info_array->out_info[s_pdaf->cur_res];

  if(!out_info.is_pdaf_supported) {
    SERR("PDAF not supported for this resolution");
    return SENSOR_SUCCESS;
  }

  PDAF_CAMIF_get_lib_version(&camif_lib_version);
  SLOW("major_version %d, minor_version %d",
       camif_lib_version.major_version,
       camif_lib_version.minor_version);

  PDAF_get_lib_version(&version);
  SLOW("major_version %d, minor_version %d",
       version.major_version,
       version.minor_version);

  /* set calibration data */
  if(s_pdaf->eeprom_data == NULL) {
   SERR("eeprom data is not avialable");
    return SENSOR_FAILURE;
   }

  pdaf_1d_gain = &((format_data_t *)(s_pdaf->eeprom_data))->pdafc_1d;
  memset(&input_camif, 0, sizeof(input_camif));
  memset(&input, 0, sizeof(input));

  out_info = sensor_lib_ptr.out_info_array->out_info[0];
  input_camif.camif_info.ori_image_height     = out_info.y_output;
  input_camif.camif_info.ori_image_width      = out_info.x_output;
  input_camif.camif_info.dual_vfe             = s_pdaf->isp_camif.dual_vfe;
  input_camif.camif_info.camif_left_buffer_width
   = s_pdaf->isp_camif.camif_left_buffer_width;
  input_camif.camif_info.camif_left_buffer_stride
   = s_pdaf->isp_camif.camif_left_buffer_stride;
  input_camif.camif_info.camif_right_buffer_width
   = s_pdaf->isp_camif.camif_right_buffer_width;
  input_camif.camif_info.camif_right_buffer_stride
   = s_pdaf->isp_camif.camif_right_buffer_stride;
  input_camif.camif_info.camif_buffer_height
   = s_pdaf->isp_camif.camif_buffer_height;
  input_camif.camif_info.image_overlap
   = s_pdaf->isp_camif.image_overlap;
  input_camif.camif_info.right_image_offset
   = s_pdaf->isp_camif.right_image_offset;
  input_camif.sensor_pattern           = SENSOR_PATTERN0;

  input.sensor_pattern     = SENSOR_PATTERN0;
  input.p_calibration_para = (void *)pdaf_1d_gain;
  input.ori_image_height   = out_info.y_output;
  input.ori_image_width    = out_info.x_output;
  input.black_level        = SENSOR_BLACK_LEVEL;
  input.cali_version       = CALIBRATION_VERSION_1D;
  input_camif.pixel_pattern =
   (((format_data_t *)(s_pdaf->eeprom_data))->pdafInfo);
  input.pixel_pattern = input_camif.pixel_pattern;

  out_info = sensor_lib_ptr.out_info_array->out_info[s_pdaf->cur_res];
  input_camif.binning_info.horizontal_resolution =
  input.binning_info.horizontal_resolution       = out_info.x_output;
  input_camif.binning_info.vertical_resolution   =
  input.binning_info.vertical_resolution         = out_info.y_output;
  input_camif.binning_info.binning_factor        =
  input.binning_info.binning_factor              = out_info.binning_factor;
  input_camif.binning_info.is_pdaf_support       =
  input.binning_info.is_pdaf_support             = out_info.is_pdaf_supported;

  camif_handle = PDAF_CAMIF_init(&input_camif);
  if (!camif_handle)
  {
   SERR("PDAF_CAMIF_init failed");
   return SENSOR_FAILURE;
  }
  pd_handle = PDAF_PhaseDetection_init(&input);
  if  (!pd_handle)
  {
   SERR("PDAF_PhaseDetection_init failed");
   return SENSOR_FAILURE;
  }

#ifndef FLIP_MIRROR
  sensor_orientation = CANONICAL_ORIENTATION;
#else
  sensor_orientation = MIRROR_AND_FLIP_ORIENTATION;
#endif

  rc = PDAF_CAMIF_sensor_orientation(camif_handle, sensor_orientation);
  if(rc != PDAF_LIB_OK)
  {
    SERR("PDAF CAMIF sensor orientation set failed");
    return SENSOR_FAILURE;
  }
  rc = PDAF_PhaseDetection_sensor_orientation(pd_handle, sensor_orientation);
  if(rc != PDAF_LIB_OK)
  {
    SERR("PDAF phase detection sensor orientation set failed");
    return SENSOR_FAILURE;
  }

  SLOW("Exit");
  return SENSOR_SUCCESS;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_pdaf_calculate_defocus -
 *
 * DESCRIPTION: PDAF calcuate defocus
 *==========================================================================*/
int s5k3m2xm_pdaf_calculate_defocus(__attribute__((unused))unsigned int * arg1,
  void * arg2, signed long (*dummy)(void *input, void *output)){
  pdaf_params_t                *s_pdaf = (pdaf_params_t *)arg2;
  int                          rc = PDAF_LIB_OK;
  unsigned int                 i = 0, j = 0;
  struct sensor_lib_out_info_t out_info;
  pdaf_camif_param_t           input_camif;
  PDAF_window_configure_t      win_config;
  pdaf_param_t                 input;
  pdaf_camif_output_data_t     output_camif;
  pdaf_output_data_t           output;

  SLOW("Enter");
  SLOW("PDAF calculate defocus, cur_res:%d",s_pdaf->cur_res);
  out_info = sensor_lib_ptr.out_info_array->out_info[s_pdaf->cur_res];

  if(!out_info.is_pdaf_supported) {
    SERR("PDAF not supported for this resolution");
    return 0;
  }

  s_pdaf->status = FALSE;
  memset(&input, 0, sizeof(input));
  memset(&input_camif, 0, sizeof(input_camif));
  memset(&output_camif, 0, sizeof(output_camif));
  memset(&output, 0, sizeof(output));

  input_camif.p_left = s_pdaf->pd_stats;
  input_camif.p_right = NULL;
  win_config.pdaf_sw_window_mode                         = FIXED_GRID_WINDOW;
  win_config.fixed_grid_window_configure.roi_offset_hori = 0.33333;
  win_config.fixed_grid_window_configure.roi_offset_ver  = 0.33334;
  win_config.fixed_grid_window_configure.roi_width       = 0.33335;
  win_config.fixed_grid_window_configure.roi_height      = 0.33334;
  win_config.fixed_grid_window_configure.window_number_hori = 1;
  win_config.fixed_grid_window_configure.window_number_ver  = 1;
  memcpy(&input_camif.window_configure, &win_config,
         sizeof(PDAF_window_configure_t));
  rc = PDAF_CAMIF_getPDAF(camif_handle, &input_camif, &output_camif);
  if (rc != PDAF_LIB_OK)
  {
   SERR("PDAF_CAMIF_getPDAF failed");
   return -rc;
  }

  input.camif_out = output_camif;
  /* do not have any info for ROI */
  input.image_analog_gain = s_pdaf->sensor_real_gain;
  input.defocus_confidence_th = 20;
  memcpy(&input.window_configure, &win_config,
       sizeof(PDAF_window_configure_t));
  rc = PDAF_PhaseDetection(pd_handle, &input, &output);
  if ( rc != PDAF_LIB_OK)
  {
   SERR("PDAF_PhaseDetection failed");
   return -rc;
  }
  memcpy(s_pdaf->defocus, output.defocus, sizeof(s_pdaf->defocus));
  s_pdaf->status = TRUE;
  s_pdaf->x_win_num =
   win_config.fixed_grid_window_configure.window_number_hori;
  s_pdaf->y_win_num =
   win_config.fixed_grid_window_configure.window_number_ver;
  s_pdaf->x_offset = s_pdaf->y_offset = SENSOR_PDAF_Y_OFFSET;

  SLOW("Exit");
  return rc;
}

/*===========================================================================
 * FUNCTION    - s5k3m2xm_deinit_pdaf -
 *
 * DESCRIPTION: PDAF release resource for API
 *==========================================================================*/
int s5k3m2xm_deinit_pdaf()
{
  int rc = PDAF_LIB_OK;

  SLOW("Enter");
  rc = PDAF_CAMIF_deinit(camif_handle);
  if ( rc != PDAF_LIB_OK)
  {
    SERR("PDAF CAMIF deinit failed %d", -rc);
  }
  rc = PDAF_PhaseDetection_deinit(pd_handle);
  if ( rc != PDAF_LIB_OK)
  {
    SERR("PDAF PhaseDetection deinit failed %d", -rc);
  }
  camif_handle = pd_handle = 0;
  SLOW("pd_handle %d, camif_handle %d", pd_handle, camif_handle);
  SLOW("Exit");
  return -rc;
}

static sensor_exposure_table_t s5k3m2xm_expsoure_tbl = {
  .sensor_calculate_exposure = s5k3m2xm_calculate_exposure,
  .sensor_fill_exposure_array = s5k3m2xm_fill_exposure_array,
};

static sensor_lib_t sensor_lib_ptr = {
  /* sensor slave info */
  .sensor_slave_info = &sensor_slave_info,
  /* Actuator Name */
  .actuator_name = "dw9761b",
  /* Eeprom name */
  .eeprom_name = "dw9761b",
  /* sensor init params */
  .sensor_init_params = &sensor_init_params,
  /* sensor output settings */
  .sensor_output = &sensor_output,
  /* sensor output register address */
  .output_reg_addr = &output_reg_addr,
  /* sensor exposure gain register address */
  .exp_gain_info = &exp_gain_info,
  /* manual_exp_info */
  .manual_exp_info = &manual_exp_info,
  /* sensor aec info */
  .aec_info = &aec_info,
  /* sensor snapshot exposure wait frames info */
  .snapshot_exp_wait_frames = 1,
  /* number of frames to skip after start stream */
  .sensor_num_frame_skip = 1,
  /* number of frames to skip after start HDR stream */
  .sensor_num_HDR_frame_skip = 2,
  /* sensor pipeline immediate delay */
  .sensor_max_pipeline_frame_delay = 1,
  /* sensor exposure table size */
  .exposure_table_size = 20,
  /* sensor lens info */
  .default_lens_info = &default_lens_info,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = s5k3m2xm_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(s5k3m2xm_cid_cfg),
  /* init settings */
  .init_settings_array = &init_settings_array,
  /* start settings */
  .start_settings = &start_settings,
  /* stop settings */
  .stop_settings = &stop_settings,
  /* group on settings */
  .groupon_settings = &groupon_settings,
  /* group off settings */
  .groupoff_settings = &groupoff_settings,
  /* resolution cfg table */
  .sensor_res_cfg_table = &s5k3m2xm_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &s5k3m2xm_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &s5k3m2xm_expsoure_tbl,
  /* chromatix array */
  .chromatix_array = &s5k3m2xm_lib_chromatix_array,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
   .sensorlib_pdaf_api =
  {
      /* PDAF init */
    .init = "s5k3m2xm_init_pdaf",
      /* PDAF calculate defocus */
    .deinit = "s5k3m2xm_deinit_pdaf",
      /* PDAF deinit */
    .calcdefocus = "s5k3m2xm_pdaf_calculate_defocus",
  },
};

/*===========================================================================
 * FUNCTION    - s5k3m2xm_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *s5k3m2xm_open_lib(void)
{
  return &sensor_lib_ptr;
}

