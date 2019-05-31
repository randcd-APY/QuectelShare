/* imx241_lib.c
 *
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include "imx241_lib.h"

#define IMX241_VGA_RES

static sensor_lib_t sensor_lib_ptr;

static struct msm_sensor_power_setting imx241_power_setting[] = {
    {
            .seq_type = SENSOR_VREG,
            .seq_val = CAM_VANA,
            .config_val = 2850000,
            .delay = 2,
    },
   {
            .seq_type = SENSOR_VREG,
            .seq_val = CAM_VDIG,
            .config_val = 1200000,
            .delay = 2,
    },
    {
            .seq_type = SENSOR_VREG,
            .seq_val = CAM_VIO,
            .config_val = 1800000,
            .delay = 2,
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
            .delay = 1,
    },
     {
            .seq_type = SENSOR_GPIO,
            .seq_val = SENSOR_GPIO_RESET,
            .config_val = GPIO_OUT_LOW,
            .delay = 1,
    },
    {
            .seq_type = SENSOR_GPIO,
            .seq_val = SENSOR_GPIO_RESET,
            .config_val = GPIO_OUT_HIGH,
            .delay = 1,
    },
    {
            .seq_type = SENSOR_I2C_MUX,
            .seq_val = 0,
            .config_val = 0,
            .delay = 0,
    },
};


static struct msm_camera_sensor_slave_info sensor_slave_info = {
   .sensor_name = "imx241",
   /* Camera slot where this camera is mounted */
   .camera_id = CAMERA_0,
   /* sensor slave address */
   .slave_addr = 0x6E,
   /* sensor address type */
   .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
   /* sensor id info*/
   .sensor_id_info = {
   /* sensor id register address */
   .sensor_id_reg_addr = 0x0000,
   /* sensor id */
   .sensor_id = 0x241,
  },
  /* power up / down setting */
   .power_setting_array = {
      .power_setting = imx241_power_setting,
      .size = ARRAY_SIZE(imx241_power_setting),
   },
};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = 1,
  .position = 0,
  .sensor_mount_angle = 270,
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
    .global_gain_addr = 0x0205,
    .vert_offset = 5,
};

static sensor_aec_data_t aec_info = {
  .max_gain = IMX241_MAX_GAIN,
  .max_linecount = 65531,
};

//TBD
static sensor_lens_info_t default_lens_info = {
  .focal_length = 2.93,
  .pix_size = 1.4,
  .f_number = 2.8,
  .total_f_dist = 1.2,
  .hor_view_angle = 54.8,
  .ver_view_angle = 42.5,
};

/*
static sensor_physical_dimensions_t phy_info = {
  .pix_width = 1.12,
  .pix_height = 1.12,
};
*/

//TBD
static struct csi_lane_params_t csi_lane_params = {
    .csi_lane_assign = 0x4320,
    .csi_lane_mask = 0x7,
    .csi_if = 1,
    .csid_core = {0},
    .csi_phy_sel = 0,
};


static struct msm_camera_i2c_reg_setting init_reg_setting[] = {
  {
    .reg_setting = INIT0_REG_ARRAY,
    .size = ARRAY_SIZE(INIT0_REG_ARRAY),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
};

static struct sensor_lib_reg_settings_array init_settings_array = {
  .reg_settings = init_reg_setting,
  .size = 1,
};

static struct msm_camera_i2c_reg_array start_reg_array[] = {
  {0x0100, 0x01},
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 10,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
    {0x0100, 0x00},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 10,
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

//TBD
static struct msm_camera_csid_vc_cfg imx241_cid_cfg[] = {
    {0, CSI_RAW10, CSI_DECODE_10BIT},
};

static struct msm_camera_csi2_params imx241_csi_params = {
    .csid_params = {
        .lane_cnt = 2,
        .lut_params = {
            .num_cid = ARRAY_SIZE(imx241_cid_cfg),
            .vc_cfg = {
                 &imx241_cid_cfg[0],
            },
        },
    },
    .csiphy_params = {
        .lane_cnt = 2,
        .settle_cnt = 0x15,
#ifndef VFE_40
        .combo_mode = 1,
#endif
    },
};
//TBD
static struct sensor_pix_fmt_info_t imx241_pix_fmt0_fourcc[] = {
  { V4L2_PIX_FMT_SRGGB10 },
};
//TBD
static struct sensor_pix_fmt_info_t imx241_pix_fmt1_fourcc[] = {
  { MSM_V4L2_PIX_FMT_META },
};
//TBD
static sensor_stream_info_t imx241_stream_info[] = {
  {1, &imx241_cid_cfg[0], imx241_pix_fmt0_fourcc},
};

static sensor_stream_info_array_t imx241_stream_info_array = {
  .sensor_stream_info = imx241_stream_info,
  .size = ARRAY_SIZE(imx241_stream_info),
};

static struct msm_camera_i2c_reg_setting res_settings[] = {
    /* RES 0 */
    {
        .reg_setting = RES0_REG_ARRAY,
        .size = ARRAY_SIZE(RES0_REG_ARRAY),
        .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
        .data_type = MSM_CAMERA_I2C_BYTE_DATA,
        .delay = 0,
    },
};

static struct sensor_lib_reg_settings_array res_settings_array = {
  .reg_settings = res_settings,
  .size = ARRAY_SIZE(res_settings),
};

static struct msm_camera_csi2_params *csi_params[] = {
  &imx241_csi_params,
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
};

static struct sensor_crop_parms_t crop_params[] = {
   {0, 0, 0, 0},
};

static struct sensor_lib_crop_params_array crop_params_array = {
  .crop_params = crop_params,
  .size = ARRAY_SIZE(crop_params),
};

static sensor_manual_exposure_info_t manual_exp_info = {
  .min_exposure_time = 16252,/*in nano sec = 1line*/
  .max_exposure_time = 1065075343,/*in nano sec = FFFF lines*/
  .min_iso = 100,
  .max_iso = 3200,
};

static struct sensor_lib_out_info_t sensor_out_info[] = {
   /* Res 0 */
  {
    .x_output = 2592,
    .y_output = 1944,
    .line_length_pclk = 1456,
    .frame_length_lines = 2051,
    .vt_pixel_clk = 89600000,
    .op_pixel_clk = 179200000,
    .binning_factor = 1,
    .max_fps = 30,
    .min_fps = 7.5,
    .mode = SENSOR_DEFAULT_MODE,
  },
};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t imx241_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t imx241_res_table = {
  .res_cfg_type = imx241_res_cfg,
  .size = ARRAY_SIZE(imx241_res_cfg),
};

static struct sensor_lib_chromatix_t imx241_chromatix[] = {
  {
    .common_chromatix = IMX241_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = IMX241_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camera_snapshot_chromatix = IMX241_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camcorder_chromatix = IMX241_LOAD_CHROMATIX(default_video), /* RES0 */
  },
};

static struct sensor_lib_chromatix_array imx241_lib_chromatix_array = {
  .sensor_lib_chromatix = imx241_chromatix,
  .size = ARRAY_SIZE(imx241_chromatix),
};

/*===========================================================================
 * FUNCTION    - imx241_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
//TBD
static uint16_t imx241_real_to_register_gain(float gain)
{
  uint16_t reg_gain;

  if(gain < 1.0) {
      gain = 1.0;
  }
  reg_gain = (uint16_t) (256 - (256 / gain));

  if (reg_gain < IMX241_MIN_AGAIN_REG_VAL) {
      reg_gain = IMX241_MIN_AGAIN_REG_VAL;
  } else if (reg_gain > IMX241_MAX_AGAIN_REG_VAL) {
      reg_gain = IMX241_MAX_AGAIN_REG_VAL;
  }

  return reg_gain;
}

/*===========================================================================
 * FUNCTION    - imx241_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
//TBD
static float imx241_register_to_real_gain(uint16_t reg_gain)
{
  float real_gain;

  if (reg_gain < IMX241_MIN_AGAIN_REG_VAL) {
      reg_gain = IMX241_MIN_AGAIN_REG_VAL;
  } else if (reg_gain > IMX241_MAX_AGAIN_REG_VAL) {
      reg_gain = IMX241_MAX_AGAIN_REG_VAL;
  }
  real_gain = 256 / (float)(256 - reg_gain);

  return real_gain;
}

/*===========================================================================
 * FUNCTION    - imx241_calculate_exposure -
 *
 * DESCRIPTION:
 *==========================================================================*/
//TBD
static int32_t imx241_calculate_exposure(float real_gain,
  uint16_t line_count, sensor_exposure_info_t *exp_info)
{
  if (!exp_info) {
    return -1;
  }
  exp_info->reg_gain = imx241_real_to_register_gain(real_gain);
  float sensor_real_gain = imx241_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain = real_gain / sensor_real_gain;
  exp_info->line_count = line_count;
  return 0;
}

/*===========================================================================
 * FUNCTION    - imx241_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
//TBD
static int32_t imx241_fill_exposure_array(uint16_t gain, uint32_t line,
  uint32_t fl_lines, int32_t luma_avg, uint32_t fgain,
  struct msm_camera_i2c_reg_setting* reg_setting)
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint16_t i = 0;

  if (!reg_setting) {
    return -1;
  }

  for (i = 0; i < sensor_lib_ptr.groupon_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.output_reg_addr->frame_length_lines;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.output_reg_addr->frame_length_lines + 1;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF);
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
	  sensor_lib_ptr.exp_gain_info->coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF00) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
	  sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF);
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF);
  reg_count++;

  for (i = 0; i < sensor_lib_ptr.groupoff_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 1;

  return rc;
}

static sensor_exposure_table_t imx241_expsoure_tbl = {
  .sensor_calculate_exposure = imx241_calculate_exposure,
  .sensor_fill_exposure_array = imx241_fill_exposure_array,
};

/*
static int32_t imx241_set_vertical_flip( int32_t flip, struct msm_camera_i2c_reg_setting* reg_setting )
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint32_t reg_data = 0;

  if (!reg_setting) {
    return -1;
  }

  if (flip)
     reg_data = 0x40 | ( 1 << 2 );
  else
     reg_data = 0x40;

  reg_setting->reg_setting[reg_count].reg_addr = 0x3820;
  reg_setting->reg_setting[reg_count++].reg_data = reg_data;

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;

  return rc;
}

static sensor_vertical_flip_table_t imx241_vertical_flip_tbl = {
   .sensor_set_vertical_flip = imx241_set_vertical_flip
};

static int32_t imx241_set_horizontal_mirror( int32_t mirror, struct msm_camera_i2c_reg_setting* reg_setting )
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint32_t reg_data = 0;

  if (!reg_setting) {
    return -1;
  }

  if (mirror)
     reg_data = 0x00 | ( 1 << 2 );
  else
     reg_data = 0x00;

  reg_setting->reg_setting[reg_count].reg_addr = 0x3821;
  reg_setting->reg_setting[reg_count++].reg_data = reg_data;

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;

  return rc;
}

static sensor_horizontal_mirror_table_t imx241_horizontal_mirror_tbl = {
   .sensor_set_horizontal_mirror = imx241_set_horizontal_mirror
};
*/

static sensor_lib_t sensor_lib_ptr = {
  /* sensor slave info */
  .sensor_slave_info = &sensor_slave_info,
  /* sensor init params */
  .sensor_init_params = &sensor_init_params,
  /* sensor output settings */
  .sensor_output = &sensor_output,
  /* sensor output register address */
  .output_reg_addr = &output_reg_addr,
  /* sensor exposure gain register address */
  .exp_gain_info = &exp_gain_info,
  /* sensor aec info */
  .aec_info = &aec_info,
  /* sensor snapshot exposure wait frames info */
  .snapshot_exp_wait_frames = 1,
  /* number of frames to skip after start stream */
  .sensor_num_frame_skip = 1,
   /* number of frames to skip after start HDR stream */
  .sensor_num_HDR_frame_skip = 2,
  /* sensor exposure table size */
  .exposure_table_size = 10,
  /* sensor lens info */
  .default_lens_info = &default_lens_info,
  /* sensor physical dimensions info */
//  .sensor_phy_info = &phy_info,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = imx241_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(imx241_cid_cfg),
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
  .sensor_res_cfg_table = &imx241_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &imx241_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &imx241_expsoure_tbl,
  /* manual_exp_info */
  .manual_exp_info = &manual_exp_info,
  /* manual sensor exposure table size */
  //.manual_exposure_table_size = 4,
  /* manual sensor gain table size */
  //.manual_gain_table_size = 2,

  /* maual exposure func table */
  //.manual_exposure_func_table = &imx241_manual_exposure_tbl,
  /* maual gain func table */
  //.manual_gain_func_table = &imx241_manual_gain_tbl,

     /* vertical_flip func table size */
  /*.vertical_flip_table_size = 1,*/

  /* vertical_flip_func table */
  /*.vertical_flip_func_table = &imx241_vertical_flip_tbl,*/

	     /* vertical_flip func table size */
  /*.horizontal_mirror_table_size = 1,*/

  /* vertical_flip_func table */
  /*.horizontal_mirror_func_table = &imx241_horizontal_mirror_tbl,*/

  /* chromatix array */
  .chromatix_array = &imx241_lib_chromatix_array,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
  /* ois info */
//  .ois_supported = 0,
};

/*===========================================================================
 * FUNCTION    - imx241_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *imx241_open_lib(void)
{
  return &sensor_lib_ptr;
}

