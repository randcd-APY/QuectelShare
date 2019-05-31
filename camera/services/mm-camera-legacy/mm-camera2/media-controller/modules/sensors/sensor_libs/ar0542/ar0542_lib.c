/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

#include <stdio.h>
#include "sensor_lib.h"
#include <utils/Log.h>
#include <unistd.h>

#ifdef  AR0542_LIB_CDBG
#define CDBG_ERROR(fmt, args...)  ALOGE(fmt, ##args)
#else
#define CDBG_ERROR(fmt, args...)  do{}while(0)
#endif

#define SENSOR_MODEL_NO_AR0542 "ar0542"
#define AR0542_LOAD_CHROMATIX(n) \
  "libchromatix_"SENSOR_MODEL_NO_AR0542"_"#n".so"

#define AR0542_SNAPSHOT_SUPPORT 1
#define AR0542_PREVIEW_SUPPORT 1

static sensor_lib_t sensor_lib_ptr;

#define MIRROR_FLIP

static struct msm_sensor_power_setting power_setting[] = {
   {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_LOW,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_LOW,
    .delay = 2,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VIO,
    .config_val = GPIO_OUT_HIGH,
    .delay =0,
  },
   {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VDIG,
    .config_val = GPIO_OUT_HIGH,
    .delay = 3,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VANA,
    .config_val = GPIO_OUT_HIGH,
    .delay = 3,
  },
  {
    .seq_type = SENSOR_CLK,
    .seq_val = SENSOR_CAM_MCLK,
    .config_val = 24000000,
    .delay = 2,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VAF,
    .config_val = GPIO_OUT_HIGH,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_HIGH,
    .delay = 3,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_HIGH,
    .delay = 3,
  },
  {
    .seq_type = SENSOR_I2C_MUX,
    .seq_val = 0,
    .config_val = 0,
    .delay = 0,
  },
};

static struct msm_sensor_power_setting power_down_setting[] = {
  {
    .seq_type = SENSOR_I2C_MUX,
    .seq_val = 0,
    .config_val = 0,
    .delay = 0,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_CLK,
    .seq_val = SENSOR_CAM_MCLK,
    .config_val = 24000000,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VANA,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VDIG,
    .config_val = GPIO_OUT_HIGH,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_VIO,
    .config_val = GPIO_OUT_LOW,
    .delay =0,
  },

  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VAF,
    .config_val = 0,
    .delay = 5,
  },
};

static struct msm_camera_sensor_slave_info sensor_slave_info = {
  /* Camera slot where this camera is mounted */
  .camera_id = CAMERA_0,
  /* sensor slave address */
  .slave_addr = 0x6c,
  /* sensor i2c frequency*/
  .i2c_freq_mode = I2C_FAST_MODE,
  /* sensor address type */
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  /* sensor id info*/
  .sensor_id_info = {
  /* sensor id register address */
  .sensor_id_reg_addr = 0x0000,
  /* sensor id */
  .sensor_id = 0x4800,
  },
  /* power up / down setting */
  .power_setting_array = {
    .power_setting = power_setting,
    .size = ARRAY_SIZE(power_setting),
    .power_down_setting = power_down_setting,
    .size_down = ARRAY_SIZE(power_down_setting),
  },
  //.is_flash_supported = 1,

};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = CAMERA_MODE_2D_B,
  .position = BACK_CAMERA_B,
  .sensor_mount_angle = SENSOR_MOUNTANGLE_90,
};

static sensor_output_t sensor_output = {
  .output_format = SENSOR_BAYER,
  .connection_mode = SENSOR_MIPI_CSI,
  .raw_output = SENSOR_10_BIT_DIRECT,
};

static struct msm_sensor_output_reg_addr_t output_reg_addr = {
  .x_output = 0x034C,
  .y_output = 0x034E,
  .line_length_pclk = 0x300C,
  .frame_length_lines = 0x300A,
};

static struct msm_sensor_exp_gain_info_t exp_gain_info = {
  .coarse_int_time_addr      = 0x0202,
  .global_gain_addr          = 0x305E,
  .vert_offset               = 0,
};

static sensor_aec_data_t aec_info = {
  .max_gain = 24.0,
  .max_linecount = 0xffff,
};

static sensor_lens_info_t default_lens_info = {
  .focal_length = 3.37,
  .pix_size = 1.4,
  .f_number = 2.8,
  .total_f_dist = 1,
  .hor_view_angle = 56.3,
  .ver_view_angle = 47.3,
};

#ifndef VFE_40
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x4320,
  .csi_lane_mask = 0x7,
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 0,
};
#else
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x4320,
  .csi_lane_mask = 0x7,
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 0,
};
#endif

static struct msm_camera_i2c_reg_array init_reg_array[] = {
  {0x301A, 0x0018, 0x00},
  {0x3064, 0xB800, 0x00},
  {0x31AE, 0x0202, 0x00},
  {0x316A, 0x8400, 0x00},
  {0x316C, 0x8400, 0x00},
  {0x316E, 0x8400, 0x00},
  {0x3EFA, 0x1A1F, 0x00},
  {0x3ED2, 0xD965, 0x00},
  {0x3ED8, 0x7F1B, 0x00},
  {0x3EDA, 0x2F11, 0x00},
  {0x3EE2, 0x0060, 0x00},
  {0x3EF2, 0xD965, 0x00},
  {0x3EF8, 0x797F, 0x00},
  {0x3EFC, 0x286F, 0x00},
  {0x3EFE, 0x2C01, 0x00},
  {0x3E00, 0x042F, 0x00},
  {0x3E02, 0xFFFF, 0x00},
  {0x3E04, 0xFFFF, 0x00},
  {0x3E06, 0xFFFF, 0x00},
  {0x3E08, 0x8071, 0x00},
  {0x3E0A, 0x7281, 0x00},
  {0x3E0C, 0x4011, 0x00},
  {0x3E0E, 0x8010, 0x00},
  {0x3E10, 0x60A5, 0x00},
  {0x3E12, 0x4080, 0x00},
  {0x3E14, 0x4180, 0x00},
  {0x3E16, 0x0018, 0x00},
  {0x3E18, 0x46B7, 0x00},
  {0x3E1A, 0x4994, 0x00},
  {0x3E1C, 0x4997, 0x00},
  {0x3E1E, 0x4682, 0x00},
  {0x3E20, 0x0018, 0x00},
  {0x3E22, 0x4241, 0x00},
  {0x3E24, 0x8000, 0x00},
  {0x3E26, 0x1880, 0x00},
  {0x3E28, 0x4785, 0x00},
  {0x3E2A, 0x4992, 0x00},
  {0x3E2C, 0x4997, 0x00},
  {0x3E2E, 0x4780, 0x00},
  {0x3E30, 0x4D80, 0x00},
  {0x3E32, 0x100C, 0x00},
  {0x3E34, 0x8000, 0x00},
  {0x3E36, 0x184A, 0x00},
  {0x3E38, 0x8042, 0x00},
  {0x3E3A, 0x001A, 0x00},
  {0x3E3C, 0x9610, 0x00},
  {0x3E3E, 0x0C80, 0x00},
  {0x3E40, 0x4DC6, 0x00},
  {0x3E42, 0x4A80, 0x00},
  {0x3E44, 0x0018, 0x00},
  {0x3E46, 0x8042, 0x00},
  {0x3E48, 0x8041, 0x00},
  {0x3E4A, 0x0018, 0x00},
  {0x3E4C, 0x804B, 0x00},
  {0x3E4E, 0xB74B, 0x00},
  {0x3E50, 0x8010, 0x00},
  {0x3E52, 0x6056, 0x00},
  {0x3E54, 0x001C, 0x00},
  {0x3E56, 0x8211, 0x00},
  {0x3E58, 0x8056, 0x00},
  {0x3E5A, 0x827C, 0x00},
  {0x3E5C, 0x0970, 0x00},
  {0x3E5E, 0x8082, 0x00},
  {0x3E60, 0x7281, 0x00},
  {0x3E62, 0x4C40, 0x00},
  {0x3E64, 0x8E4D, 0x00},
  {0x3E66, 0x8110, 0x00},
  {0x3E68, 0x0CAF, 0x00},
  {0x3E6A, 0x4D80, 0x00},
  {0x3E6C, 0x100C, 0x00},
  {0x3E6E, 0x8440, 0x00},
  {0x3E70, 0x4C81, 0x00},
  {0x3E72, 0x7C5F, 0x00},
  {0x3E74, 0x7000, 0x00},
  {0x3E76, 0x0000, 0x00},
  {0x3E78, 0x0000, 0x00},
  {0x3E7A, 0x0000, 0x00},
  {0x3E7C, 0x0000, 0x00},
  {0x3E7E, 0x0000, 0x00},
  {0x3E80, 0x0000, 0x00},
  {0x3E82, 0x0000, 0x00},
  {0x3E84, 0x0000, 0x00},
  {0x3E86, 0x0000, 0x00},
  {0x3E88, 0x0000, 0x00},
  {0x3E8A, 0x0000, 0x00},
  {0x3E8C, 0x0000, 0x00},
  {0x3E8E, 0x0000, 0x00},
  {0x3E90, 0x0000, 0x00},
  {0x3E92, 0x0000, 0x00},
  {0x3E94, 0x0000, 0x00},
  {0x3E96, 0x0000, 0x00},
  {0x3E98, 0x0000, 0x00},
  {0x3E9A, 0x0000, 0x00},
  {0x3E9C, 0x0000, 0x00},
  {0x3E9E, 0x0000, 0x00},
  {0x3EA0, 0x0000, 0x00},
  {0x3EA2, 0x0000, 0x00},
  {0x3EA4, 0x0000, 0x00},
  {0x3EA6, 0x0000, 0x00},
  {0x3EA8, 0x0000, 0x00},
  {0x3EAA, 0x0000, 0x00},
  {0x3EAC, 0x0000, 0x00},
  {0x3EAE, 0x0000, 0x00},
  {0x3EB0, 0x0000, 0x00},
  {0x3EB2, 0x0000, 0x00},
  {0x3EB4, 0x0000, 0x00},
  {0x3EB6, 0x0000, 0x00},
  {0x3EB8, 0x0000, 0x00},
  {0x3EBA, 0x0000, 0x00},
  {0x3EBC, 0x0000, 0x00},
  {0x3EBE, 0x0000, 0x00},
  {0x3EC0, 0x0000, 0x00},
  {0x3EC2, 0x0000, 0x00},
  {0x3EC4, 0x0000, 0x00},
  {0x3EC6, 0x0000, 0x00},
  {0x3EC8, 0x0000, 0x00},
  {0x3ECA, 0x0000, 0x00},
  {0x3170, 0x2150, 0x00},
  {0x317A, 0x0150, 0x00},
  {0x3ECC, 0x2200, 0x00},
  {0x3174, 0x0000, 0x00},
  {0x3176, 0x0000, 0x00},
  {0x30BC, 0x0384, 0x00},
  {0x30C0, 0x1220, 0x00},
  {0x30D4, 0x9200, 0x00},
  {0x30B2, 0xC000, 0x00},
  {0x3102, 0x0000, 0x00},
  {0x31B0, 0x00C4, 0x00},
  {0x31B2, 0x0064, 0x00},
  {0x31B4, 0x0E47, 0x00},
  {0x31B6, 0x0D24, 0x00},
  {0x31B8, 0x020E, 0x00},
  {0x31BA, 0x0710, 0x00},
  {0x31BC, 0x2A0D, 0x00},
  {0x31BE, 0xC007, 0x00},
  {0x305E, 0x112A, 0x00},
  {0x3ECE, 0x000A, 0x00},
  {0x0400, 0x0000, 0x00},
  {0x0404, 0x0010, 0x00},
  {0x0300, 0x0005, 0x00},
  {0x0302, 0x0001, 0x00},
  {0x0304, 0x0006, 0x00},
  {0x0306, 0x0089, 0x00},
  {0x0308, 0x000a, 0x00},
  {0x030A, 0x0001, 0x00},
};

static struct msm_camera_i2c_reg_setting init_reg_setting[] = {
  {
    .reg_setting = init_reg_array,
    .size = ARRAY_SIZE(init_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 2,
  },
};

static struct sensor_lib_reg_settings_array init_settings_array = {
  .reg_settings = init_reg_setting,
  .size = 1,
};

static struct msm_camera_i2c_reg_array start_reg_array[] = {
  {0x301A, 0x465c, 0x00},
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_WORD_DATA,
  .delay =2,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
  {0x301A, 0x4658, 0x00},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_WORD_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupon_reg_array[] = {
  {0x0104, 0x01, 0x00},
};

static struct msm_camera_i2c_reg_setting groupon_settings = {
  .reg_setting = groupon_reg_array,
  .size = ARRAY_SIZE(groupon_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupoff_reg_array[] = {
  {0x0104, 0x00, 0x00},
};

static struct msm_camera_i2c_reg_setting groupoff_settings = {
  .reg_setting = groupoff_reg_array,
  .size = ARRAY_SIZE(groupoff_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_csid_vc_cfg ar0542_cid_cfg[] = {
  {0, CSI_RAW10, CSI_DECODE_10BIT},
  {1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ar0542_csi_params = {
  .csid_params = {
    .lane_cnt = 2,
    .lut_params = {
      .num_cid = ARRAY_SIZE(ar0542_cid_cfg),
      .vc_cfg = {
         &ar0542_cid_cfg[0],
         &ar0542_cid_cfg[1],
      },
    },
  },
  .csiphy_params = {
    .lane_cnt = 2,
    .settle_cnt = 0xA,
#ifndef VFE_40
    .combo_mode = 1,
#endif
  },
};

static struct msm_camera_csi2_params *csi_params[] = {
#if AR0542_SNAPSHOT_SUPPORT
  &ar0542_csi_params, /* RES 0*/
#endif
#if AR0542_PREVIEW_SUPPORT
  &ar0542_csi_params, /* RES 1*/
#endif
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
};

static struct sensor_pix_fmt_info_t ar0542_pix_fmt0_fourcc[] = {
#ifdef MIRROR_FLIP
  { V4L2_PIX_FMT_SGRBG10 },
#else
  { V4L2_PIX_FMT_SGBRG10 },
#endif
};

static struct sensor_pix_fmt_info_t ar0542_pix_fmt1_fourcc[] = {
  { MSM_V4L2_PIX_FMT_META },
};

static sensor_stream_info_t ar0542_stream_info[] = {
  {1, &ar0542_cid_cfg[0], ar0542_pix_fmt0_fourcc},
  {1, &ar0542_cid_cfg[1], ar0542_pix_fmt1_fourcc},
};

static sensor_stream_info_array_t ar0542_stream_info_array = {
  .sensor_stream_info = ar0542_stream_info,
  .size = ARRAY_SIZE(ar0542_stream_info),
};
#if AR0542_SNAPSHOT_SUPPORT
static struct msm_camera_i2c_reg_array res0_reg_array[] = {
//Snapshot 2592*1944 14.8fps
  {0x3004, 0x0008, 0x00},/*x_addr_start*/
  {0x3008, 0x0A27, 0x00},/*x_addr_end*/
  {0x3002, 0x0008, 0x00},/*y_start_addr*/
  {0x3006, 0x079f, 0x00},/*y_addr_end*/
#ifdef MIRROR_FLIP
  {0x3040, 0x0041, 0x00},/*read_mode*/
#else
  {0x3040, 0xC041, 0x00},//read_mode
#endif
  {0x034C, 0x0A20, 0x00},/*x_output_size*/
  {0x034E, 0x0798, 0x00},/*y_output_size*/
  {0x300C, 0x0E70, 0x00},/*line_length_pck*/
  {0x300A, 0x07E5, 0x00},/*frame_length_lines*/
  {0x3012, 0x0EE4, 0x00},/*coarse_integration_time*/
  {0x3014, 0x0C8C, 0x00},/*fine_integration_time*/
  {0x3010, 0x00A0, 0x00},/*fine_correction*/
};
#endif

#if AR0542_PREVIEW_SUPPORT
static struct msm_camera_i2c_reg_array res1_reg_array[] = {
  //Preview 1296x972 30fps
  {0x3004, 0x0008, 0x00},//x_addr_start
  {0x3008, 0x0A25, 0x00},//x_addr_end
  {0x3002, 0x0008, 0x00},//y_start_addr
  {0x3006, 0x079D, 0x00},//y_addr_end
#ifdef MIRROR_FLIP
  {0x3040, 0x04C3, 0x00},//read_mode
#else
  {0x3040, 0xC4C3, 0x00},//read_mode
#endif
  {0x034C, 0x0510, 0x00},//x_output_size
  {0x034E, 0x03CC, 0x00},//y_output_size
  //{0x300C, 0x0CF0},//line_length_pck
  {0x300C, 0x0C4C, 0x00},//line_length_pck
  {0x300A, 0x04a1, 0x00},//frame_length_lines
  //{0x300A, 0x0457},//frame_length_lines

  //{0x3012, 0x0EE4},/*coarse_integration_time*/
  //{0x3014, 0x0C8C},/*fine_integration_time*/
  //{0x3010, 0x00A0},/*fine_correction*/
  {0x3012, 0x04a0, 0x00},/*coarse_integration_time*/
  {0x3014, 0x0908, 0x00},/*fine_integration_time*/
  {0x3010, 0x0184, 0x00},/*fine_correction*/
};
#endif

static struct msm_camera_i2c_reg_setting res_settings[] = {
  /*
   * Snapshot Setting
   */
#if AR0542_SNAPSHOT_SUPPORT
  {
    .reg_setting = res0_reg_array,
    .size = ARRAY_SIZE(res0_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 0,
  },
#endif

#if AR0542_PREVIEW_SUPPORT
  {
    .reg_setting = res1_reg_array,
    .size = ARRAY_SIZE(res1_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_WORD_DATA,
    .delay = 0,
  },
#endif
};

static struct sensor_lib_reg_settings_array res_settings_array = {
  .reg_settings = res_settings,
  .size = ARRAY_SIZE(res_settings),
};

static struct sensor_crop_parms_t crop_params[] = {
#if AR0542_SNAPSHOT_SUPPORT
  {0, 0, 0, 0}, /* RES 0 */
#endif
#if AR0542_PREVIEW_SUPPORT
  {0, 0, 0, 0}, /* RES 1 */
#endif
};

static struct sensor_lib_crop_params_array crop_params_array = {
  .crop_params = crop_params,
  .size = ARRAY_SIZE(crop_params),
};

static struct sensor_lib_out_info_t sensor_out_info[] = {
#if AR0542_SNAPSHOT_SUPPORT
  {
    .x_output = 0xA20,    /* 2592 */
    .y_output = 0x798,    /* 1944 */
    .line_length_pclk = 0x0E70,   /* 3696 */
    .frame_length_lines = 0x07E5,   /* 2021 */
    .vt_pixel_clk = 110400000,/* =14.8*3696*2021 */
    .op_pixel_clk = 110400000,
    .binning_factor = 0,
    .max_fps = 14.8,
    .min_fps = 7.5,
    .mode=SENSOR_DEFAULT_MODE,
  },
#endif

#if AR0542_PREVIEW_SUPPORT
  {
    .x_output = 0x0510,   /* 1296 */
    .y_output = 0x03CC,   /* 972  */
    .line_length_pclk = 0x0C4C,
    .frame_length_lines = 0x04a1,
    .vt_pixel_clk =  111910000,
    .op_pixel_clk =  111910000,
    .binning_factor = 0,
    .max_fps = 30,
    .min_fps = 7.5,
    .mode=SENSOR_DEFAULT_MODE,
  },
#endif
};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t ar0542_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t ar0542_res_table = {
  .res_cfg_type = ar0542_res_cfg,
  .size = ARRAY_SIZE(ar0542_res_cfg),
};

static struct sensor_lib_chromatix_t ar0542_chromatix[] = {
#if AR0542_SNAPSHOT_SUPPORT
  {
    .common_chromatix = AR0542_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = AR0542_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camera_snapshot_chromatix = AR0542_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camcorder_chromatix =AR0542_LOAD_CHROMATIX(default_video), /* RES0 */
    .liveshot_chromatix = AR0542_LOAD_CHROMATIX(liveshot), /* RES0 */
  },
#endif

#if AR0542_PREVIEW_SUPPORT
  {
    .common_chromatix = AR0542_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = AR0542_LOAD_CHROMATIX(preview), /* RES1 */
    .camera_snapshot_chromatix = AR0542_LOAD_CHROMATIX(preview), /* RES1 */
    .camcorder_chromatix = AR0542_LOAD_CHROMATIX(default_video), /* RES1 */
    .liveshot_chromatix = AR0542_LOAD_CHROMATIX(liveshot), /* RES1 */
  },
#endif
};

static struct sensor_lib_chromatix_array ar0542_lib_chromatix_array = {
  .sensor_lib_chromatix = ar0542_chromatix,
  .size = ARRAY_SIZE(ar0542_chromatix),
};

/*===========================================================================
 * FUNCTION    - ar0542_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static uint16_t ar0542_real_to_register_gain(float gain) {
  uint16_t reg_gain;
  uint16_t cg, asc1;
  if (gain > 24)
      gain = 24; /*Cap max gain to use only analog portion.*/
  if (gain < 1.3) /*Use 2nd stage as soon as possible*/
  {
    gain = gain;
    cg = 0x0;
    asc1 = 0x0;
  }
  else if (gain < 2)
  {
    gain = gain/1.3;
    cg = 0x0;
    asc1 = 0x100;
  }
  else if (gain < 2.6)
  {
    gain = gain/2;
    cg = 0x800;
    asc1 = 0x0;
  }
  else if (gain < 3)
  {
    gain = gain/2.6;
    cg = 0x800;
    asc1 = 0x100;
  }
    else if (gain < 4)
  {
    gain = gain/3;
    cg = 0x400;
    asc1 = 0x0;
  }
  else if (gain < 5.2)
  {
    gain = gain/4;
    cg = 0xc00;
    asc1 = 0x0;
  }
  else if (gain < 8)
  {
    gain = gain/5.2;
    cg = 0xc00;
    asc1 = 0x100;
  }
  else
  {
    gain = gain/8.0;
    cg = 0xc00;
    asc1 = 0x200;
  }
  reg_gain = (uint16_t)(gain * 32.0);
  reg_gain |= cg;
  reg_gain |= asc1;
  return reg_gain;
}

/*===========================================================================
 * FUNCTION    - ar0542_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static float ar0542_register_to_real_gain(uint16_t reg_gain)
{
  float gain;
  float multiplier1 = 0.0;
  float multiplier2 = 0.0;
  if(reg_gain > 0xF7F)
    reg_gain = 0xF7F;
  multiplier1 = (float)((reg_gain & 0x300)>> 8);
  multiplier2 = (float)((reg_gain & 0xc00)>> 10);
  if (multiplier1 == 0.0f)
    multiplier1 = 1.0f;
  else if (multiplier1 == 1.0f)
    multiplier1 = 1.3f;
  else if (multiplier1 == 2.0f)
    multiplier1 = 2.0f;

  if (multiplier2 == 0.0f)
    multiplier2 = 1.0f;
  else if (multiplier2 == 1.0f)
    multiplier2 = 3.0f;
  else if (multiplier2 == 2.0f)
    multiplier2 = 2.0f;
  else if (multiplier2 == 3.0f)
    multiplier2 = 4.0f;

  gain = (float)(reg_gain&0x7F)/32.0f;
  gain = multiplier1*multiplier2*gain;
  return gain;
}

/*===========================================================================
 * FUNCTION    - ar0542_calculate_exposure -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t ar0542_calculate_exposure(float real_gain,
  uint16_t line_count, sensor_exposure_info_t *exp_info)
{
  if (!exp_info) {
    return -1;
  }
  exp_info->reg_gain = ar0542_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain = ar0542_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain = real_gain / exp_info->sensor_real_gain;
  exp_info->line_count = line_count;
  exp_info->sensor_digital_gain = 0x1;
  return 0;
}

/*===========================================================================
 * FUNCTION    - ar0542_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t ar0542_fill_exposure_array(uint16_t gain,
  uint32_t line, uint32_t fl_lines, int32_t luma_avg, uint32_t fgain,
  struct msm_camera_i2c_reg_setting *reg_setting)
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint16_t i = 0;

  if (!reg_setting) {
    return -1;
  }

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr ;
  reg_setting->reg_setting[reg_count].reg_data = line ;
  reg_count++;

  gain= gain|0x1000;
  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = gain ;
  reg_count++;

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_WORD_DATA;
  reg_setting->delay = 0;

  return rc;
}

static sensor_exposure_table_t ar0542_expsoure_tbl = {
  .sensor_calculate_exposure = ar0542_calculate_exposure,
  .sensor_fill_exposure_array = ar0542_fill_exposure_array,
};

static sensor_lib_t sensor_lib_ptr = {
  .sensor_slave_info = &sensor_slave_info,
  .sensor_init_params = &sensor_init_params,
  /* sensor actuator name */
  .actuator_name = "dw9714",
  .eeprom_name = "common_ar0542",
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
  .sensor_num_HDR_frame_skip = 2,
  /* sensor pipeline immediate delay */
  .sensor_max_pipeline_frame_delay = 2,
  /* sensor exposure table size */
  .exposure_table_size = 2,
  /* sensor lens info */
  .default_lens_info = &default_lens_info,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = ar0542_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(ar0542_cid_cfg),
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
  .sensor_res_cfg_table = &ar0542_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &ar0542_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &ar0542_expsoure_tbl,
  /* chromatix array */
  .chromatix_array = &ar0542_lib_chromatix_array,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
  .sync_exp_gain = 1,
};

/*===========================================================================
 * FUNCTION    - ar0542_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *ar0542_open_lib(void)
{
  return &sensor_lib_ptr;
}

