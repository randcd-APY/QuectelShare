/*============================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#include <stdio.h>
#include "sensor_lib.h"
#include <utils/Log.h>

#define SENSOR_MODEL_NO_ov5675_d5v15b "ov5675_d5v15b"
#define ov5675_d5v15b_LOAD_CHROMATIX(n) \
  "libchromatix_"SENSOR_MODEL_NO_ov5675_d5v15b"_"#n".so"

static sensor_lib_t sensor_lib_ptr;

#undef DEBUG_INFO
#define OV5675_DEBUG
#ifdef OV5675_DEBUG
#include <utils/Log.h>
#define SERR(fmt, args...) \
    ALOGE("%s:%d "fmt"\n", __func__, __LINE__, ##args)
#define DEBUG_INFO(fmt, args...) SERR(fmt, ##args)
#else
#define DEBUG_INFO(fmt, args...) do { } while (0)
#endif


static struct msm_sensor_power_setting ov5675_d5v15b_power_setting[] = {
    {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VIO,
    .config_val = 0,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VANA,
    .config_val = 0,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_VREG,
    .seq_val = CAM_VDIG,
    .config_val = 0,
    .delay = 5,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_CUSTOM1,
    .config_val = GPIO_OUT_HIGH,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_LOW,
    .delay = 1,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_STANDBY,
    .config_val = GPIO_OUT_HIGH,
    .delay = 5,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_LOW,
    .delay = 5,
  },
  {
    .seq_type = SENSOR_GPIO,
    .seq_val = SENSOR_GPIO_RESET,
    .config_val = GPIO_OUT_HIGH,
    .delay = 10,
  },
  {
    .seq_type = SENSOR_CLK,
    .seq_val = SENSOR_CAM_MCLK,
    .config_val = 24000000,
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
  .camera_id = CAMERA_1,
  /* sensor slave address */
  .slave_addr = 0x6c,
  /* sensor i2c frequency*/
  .i2c_freq_mode = I2C_FAST_MODE,
  /* sensor address type */
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  /* sensor id info*/
  .sensor_id_info = {
    /* sensor id register address */
    .sensor_id_reg_addr = 0x300b,
    /* sensor id */
    .sensor_id = 0x5675,
  },
  /* power up / down setting */
  .power_setting_array = {
    .power_setting = ov5675_d5v15b_power_setting,
    .size = ARRAY_SIZE(ov5675_d5v15b_power_setting),
  },
  //.is_flash_supported = SENSOR_FLASH_SUPPORTED,
};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = CAMERA_MODE_2D_B,
  .position = FRONT_CAMERA_B,
  .sensor_mount_angle = SENSOR_MOUNTANGLE_270,
};

static sensor_output_t sensor_output = {
  .output_format = SENSOR_BAYER,
  .connection_mode = SENSOR_MIPI_CSI,
  .raw_output = SENSOR_10_BIT_DIRECT,
};

static struct msm_sensor_output_reg_addr_t output_reg_addr = {
  .x_output = 0x3808,
  .y_output = 0x380a,
  .line_length_pclk = 0x380c,
  .frame_length_lines = 0x380e,
};

static struct msm_sensor_exp_gain_info_t exp_gain_info = {
  .coarse_int_time_addr = 0x3500,
  .global_gain_addr = 0x3508,
  .vert_offset = 4,
};

static sensor_aec_data_t aec_info = {
  .max_gain = 15.5,
  .max_linecount = 65530,
};

static sensor_lens_info_t default_lens_info = {
  .focal_length = 3.16,
  .pix_size = 1.12,
  .f_number = 2.4,
  .total_f_dist = 1.2,
  .hor_view_angle = 56.0,
  .ver_view_angle = 42.0,
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

static struct msm_camera_i2c_reg_array init_reg_array0[] = {
   {0x0103, 0x01, 0x00},
};

static struct msm_camera_i2c_reg_array init_reg_array1[] = {
/*@@ init
;;FORMAT_1_2592X1944_30FPS_MIPI_2_LANE_900Mbps
;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
;; FPS= Sysclk/(HTS*2*VTS)
;;max exposure is (VTS-4)/2
;;exposure time is twice as before. Ex. [3501,3502]=0040 means 8 Tline exposure.
*/
   {0x0300, 0x05, 0x00},
   {0x0302, 0x96, 0x00},
   {0x0303, 0x00, 0x00},
   {0x3002, 0x21, 0x00},
   {0x3107, 0x01, 0x00},
   {0x3501, 0x20, 0x00},
   {0x3503, 0x0c, 0x00},
   {0x3508, 0x03, 0x00},
   {0x3509, 0x00, 0x00},
   {0x3600, 0x66, 0x00},
   {0x3602, 0x30, 0x00},
   {0x3610, 0xa5, 0x00},
   {0x3612, 0x93, 0x00},
   {0x3620, 0x80, 0x00},
   {0x3642, 0x0e, 0x00},
   {0x3661, 0x00, 0x00},
   {0x3662, 0x10, 0x00},
   {0x3664, 0xf3, 0x00},
   {0x3665, 0x9e, 0x00},
   {0x3667, 0xa5, 0x00},
   {0x366e, 0x55, 0x00},
   {0x366f, 0x55, 0x00},
   {0x3670, 0x11, 0x00},
   {0x3671, 0x11, 0x00},
   {0x3672, 0x11, 0x00},
   {0x3673, 0x11, 0x00},
   {0x3714, 0x24, 0x00},
   {0x371a, 0x3e, 0x00},
   {0x3733, 0x10, 0x00},
   {0x3734, 0x00, 0x00},
   {0x373d, 0x24, 0x00},
   {0x3764, 0x20, 0x00},
   {0x3765, 0x20, 0x00},
   {0x3766, 0x12, 0x00},
   {0x37a1, 0x14, 0x00},
   {0x37a8, 0x1c, 0x00},
   {0x37ab, 0x0f, 0x00},
   {0x37c2, 0x04, 0x00},
   {0x37cb, 0x00, 0x00},
   {0x37cc, 0x00, 0x00},
   {0x37cd, 0x00, 0x00},
   {0x37ce, 0x00, 0x00},
   {0x37d8, 0x02, 0x00},
   {0x37d9, 0x08, 0x00},
   {0x37dc, 0x04, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0x04, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x07, 0x00},
   {0x3807, 0xb3, 0x00},
   {0x3808, 0x0a, 0x00},
   {0x3809, 0x20, 0x00},
   {0x380a, 0x07, 0x00},
   {0x380b, 0x98, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x07, 0x00},
   {0x380f, 0xd0, 0x00},
   {0x3811, 0x10, 0x00},
   {0x3813, 0x0c, 0x00},
   {0x3814, 0x01, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x01, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x88, 0x00},
   {0x3821, 0x01, 0x00},
   {0x3832, 0x04, 0x00},
   {0x3c80, 0x08, 0x00},
   {0x3c82, 0x00, 0x00},
   {0x3c83, 0xb1, 0x00},
   {0x3c8c, 0x10, 0x00},
   {0x3c8d, 0x00, 0x00},
   {0x3c90, 0x00, 0x00},
   {0x3c91, 0x00, 0x00},
   {0x3c92, 0x00, 0x00},
   {0x3c93, 0x00, 0x00},
   {0x3c94, 0x00, 0x00},
   {0x3c95, 0x00, 0x00},
   {0x3c96, 0x00, 0x00},
   {0x3c97, 0x00, 0x00},
   {0x4001, 0xe0, 0x00},
   {0x4008, 0x02, 0x00},
   {0x4009, 0x0d, 0x00},
   {0x400f, 0x80, 0x00},
   {0x4013, 0x02, 0x00},
   {0x4040, 0x00, 0x00},
   {0x4041, 0x07, 0x00},
   {0x404c, 0x50, 0x00},
   {0x404e, 0x20, 0x00},
   {0x4500, 0x06, 0x00},
   {0x4503, 0x00, 0x00},
   {0x450a, 0x04, 0x00},
   {0x4809, 0x04, 0x00},
   {0x480c, 0x12, 0x00},
   {0x4819, 0x70, 0x00},
   {0x4825, 0x32, 0x00},
   {0x4826, 0x32, 0x00},
   {0x482a, 0x06, 0x00},
   {0x4833, 0x08, 0x00},
   {0x4837, 0x0d, 0x00},
   {0x5000, 0x77, 0x00},
   {0x5b00, 0x01, 0x00},
   {0x5b01, 0x10, 0x00},
   {0x5b02, 0x01, 0x00},
   {0x5b03, 0xdb, 0x00},
   {0x5b05, 0x6c, 0x00},
   {0x5e10, 0xfc, 0x00},
   {0x3500, 0x00, 0x00},
   {0x3501, 0x3E, 0x00},
   {0x3502, 0x60, 0x00},
   {0x3503, 0x08, 0x00},
   {0x3508, 0x04, 0x00},
   {0x3509, 0x00, 0x00},
   {0x3832, 0x48, 0x00},
   {0x3c90, 0x00, 0x00},
   {0x5780, 0x3e, 0x00},
   {0x5781, 0x0f, 0x00},
   {0x5782, 0x44, 0x00},
   {0x5783, 0x02, 0x00},
   {0x5784, 0x01, 0x00},
   {0x5785, 0x01, 0x00},
   {0x5786, 0x00, 0x00},
   {0x5787, 0x04, 0x00},
   {0x5788, 0x02, 0x00},
   {0x5789, 0x0f, 0x00},
   {0x578a, 0xfd, 0x00},
   {0x578b, 0xf5, 0x00},
   {0x578c, 0xf5, 0x00},
   {0x578d, 0x03, 0x00},
   {0x578e, 0x08, 0x00},
   {0x578f, 0x0c, 0x00},
   {0x5790, 0x08, 0x00},
   {0x5791, 0x06, 0x00},
   {0x5792, 0x00, 0x00},
   {0x5793, 0x52, 0x00},
   {0x5794, 0xa3, 0x00},
   {0x4003, 0x40, 0x00},
};

static struct msm_camera_i2c_reg_setting init_reg_setting[] = {
  {
    .reg_setting = init_reg_array0,
    .size = ARRAY_SIZE(init_reg_array0),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 50,
  },
  {
    .reg_setting = init_reg_array1,
    .size = ARRAY_SIZE(init_reg_array1),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
};

static struct sensor_lib_reg_settings_array init_settings_array = {
  .reg_settings = init_reg_setting,
  .size = 2,
};

static struct msm_camera_i2c_reg_array start_reg_array[] = {
    {0x0100, 0x01, 0x00},
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
    {0x0100, 0x00, 0x00},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupon_reg_array[] = {
  {0x3208, 0x00, 0x00},
};

static struct msm_camera_i2c_reg_setting groupon_settings = {
  .reg_setting = groupon_reg_array,
  .size = ARRAY_SIZE(groupon_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupoff_reg_array[] = {
  {0x3208, 0x10, 0x00},
  {0x3208, 0xA0, 0x00},
};

static struct msm_camera_i2c_reg_setting groupoff_settings = {
  .reg_setting = groupoff_reg_array,
  .size = ARRAY_SIZE(groupoff_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_csid_vc_cfg ov5675_d5v15b_cid_cfg[] = {
  {0, CSI_RAW10, CSI_DECODE_10BIT},
  {1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ov5675_d5v15b_csi_params = {
  .csid_params = {
    .lane_cnt = 2,
    .lut_params = {
      .num_cid = ARRAY_SIZE(ov5675_d5v15b_cid_cfg),
      .vc_cfg = {
         &ov5675_d5v15b_cid_cfg[0],
         &ov5675_d5v15b_cid_cfg[1],
      },
    },
  },
  .csiphy_params = {
    .lane_cnt = 2,
    .settle_cnt = 0x1b,//0x1b,
  },
};

static struct sensor_pix_fmt_info_t ov5675_d5v15b_pix_fmt0_fourcc[] = {
  { V4L2_PIX_FMT_SBGGR10 },
};

static struct sensor_pix_fmt_info_t ov5675_d5v15b_pix_fmt1_fourcc[] = {
  { MSM_V4L2_PIX_FMT_META },
};

static sensor_stream_info_t ov5675_d5v15b_stream_info[] = {
  {1, &ov5675_d5v15b_cid_cfg[0], ov5675_d5v15b_pix_fmt0_fourcc},
  {1, &ov5675_d5v15b_cid_cfg[1], ov5675_d5v15b_pix_fmt1_fourcc},
};

static sensor_stream_info_array_t ov5675_d5v15b_stream_info_array = {
  .sensor_stream_info = ov5675_d5v15b_stream_info,
  .size = ARRAY_SIZE(ov5675_d5v15b_stream_info),
};

static struct msm_camera_i2c_reg_array res0_reg_array[] = {
/*
@@ 2592X1944_30FPS_MIPI_2_LANE_900Mbps
;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
*/
   {0x3662, 0x10, 0x00},
   {0x3714, 0x24, 0x00},
   {0x371a, 0x3e, 0x00},
   {0x37c2, 0x04, 0x00},
   {0x37d9, 0x08, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0x04, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x07, 0x00},
   {0x3807, 0xb3, 0x00},
   {0x3808, 0x0a, 0x00},
   {0x3809, 0x20, 0x00},
   {0x380a, 0x07, 0x00},
   {0x380b, 0x98, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x07, 0x00},
   {0x380f, 0xd0, 0x00},
   {0x3811, 0x10, 0x00},
   {0x3813, 0x0c, 0x00},
   {0x3814, 0x01, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x01, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x88, 0x00},
   {0x3821, 0x01, 0x00},
   {0x4008, 0x02, 0x00},
   {0x4009, 0x0d, 0x00},
   {0x4041, 0x07, 0x00},
   {0x3501, 0x3E, 0x00},
   {0x3502, 0x60, 0x00},
};

static struct msm_camera_i2c_reg_array res1_reg_array[] = {
/*
@@ FORMAT_1296x972_30FPS_MIPI_2_LANE_900Mbps
;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
*/
   {0x3662, 0x08, 0x00},
   {0x3714, 0x28, 0x00},
   {0x371a, 0x3e, 0x00},
   {0x37c2, 0x14, 0x00},
   {0x37d9, 0x04, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0x00, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x07, 0x00},
   {0x3807, 0xb7, 0x00},
   {0x3808, 0x05, 0x00},
   {0x3809, 0x10, 0x00},
   {0x380a, 0x03, 0x00},
   {0x380b, 0xcc, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x07, 0x00},
   {0x380f, 0xd0, 0x00},
   {0x3811, 0x08, 0x00},
   {0x3813, 0x08, 0x00},
   {0x3814, 0x03, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x03, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x8b, 0x00},
   {0x3821, 0x01, 0x00},
   {0x4008, 0x00, 0x00},
   {0x4009, 0x07, 0x00},
   {0x4041, 0x03, 0x00},
   {0x3501, 0x3E, 0x00},
   {0x3502, 0x60, 0x00},
};

static struct msm_camera_i2c_reg_array res2_reg_array[] = {
/*
@@ FORMAT_1280x720_60FPS_MIPI_2_LANE_900Mbps
;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
*/
   {0x3662, 0x08, 0x00},
   {0x3714, 0x28, 0x00},
   {0x371a, 0x3e, 0x00},
   {0x37c2, 0x14, 0x00},
   {0x37d9, 0x04, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0xf4, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x06, 0x00},
   {0x3807, 0xb3, 0x00},
   {0x3808, 0x05, 0x00},
   {0x3809, 0x00, 0x00},
   {0x380a, 0x02, 0x00},
   {0x380b, 0xd0, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x03, 0x00},
   {0x380f, 0xe8, 0x00},
   {0x3811, 0x10, 0x00},
   {0x3813, 0x08, 0x00},
   {0x3814, 0x03, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x03, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x8b, 0x00},
   {0x3821, 0x01, 0x00},
   {0x4008, 0x00, 0x00},
   {0x4009, 0x07, 0x00},
   {0x4041, 0x03, 0x00},
   {0x3501, 0x1F, 0x00},
   {0x3502, 0x20, 0x00},
};

static struct msm_camera_i2c_reg_array res3_reg_array[] = {
/* @@ FORMAT_640X480_90FPS_MIPI_2_LANE_900Mbps
   ;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
*/
   {0x3662, 0x08, 0x00},
   {0x3714, 0x24, 0x00},
   {0x371a, 0x3f, 0x00},
   {0x37c2, 0x24, 0x00},
   {0x37d9, 0x04, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0x10, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x07, 0x00},
   {0x3807, 0xaf, 0x00},
   {0x3808, 0x02, 0x00},
   {0x3809, 0x80, 0x00},
   {0x380a, 0x01, 0x00},
   {0x380b, 0xe0, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x02, 0x00},
   {0x380f, 0x9a, 0x00},
   {0x3811, 0x08, 0x00},
   {0x3813, 0x02, 0x00},
   {0x3814, 0x07, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x07, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x8d, 0x00},
   {0x3821, 0x00, 0x00},
   {0x4008, 0x00, 0x00},
   {0x4009, 0x03, 0x00},
   {0x4041, 0x03, 0x00},
   {0x3501, 0x0f, 0x00},
   {0x3502, 0xa0, 0x00},
};

static struct msm_camera_i2c_reg_array res4_reg_array[] = {
/*
@@ FORMAT_640X480_120FPS_MIPI_2_LANE_900Mbps
;; Sysclk 90Mhz, MIPI2_900Mbps/Lane,
*/
   {0x3662, 0x08, 0x00},
   {0x3714, 0x24, 0x00},
   {0x371a, 0x3f, 0x00},
   {0x37c2, 0x24, 0x00},
   {0x37d9, 0x04, 0x00},
   {0x3800, 0x00, 0x00},
   {0x3801, 0x00, 0x00},
   {0x3802, 0x00, 0x00},
   {0x3803, 0x10, 0x00},
   {0x3804, 0x0a, 0x00},
   {0x3805, 0x3f, 0x00},
   {0x3806, 0x07, 0x00},
   {0x3807, 0xaf, 0x00},
   {0x3808, 0x02, 0x00},
   {0x3809, 0x80, 0x00},
   {0x380a, 0x01, 0x00},
   {0x380b, 0xe0, 0x00},
   {0x380c, 0x02, 0x00},
   {0x380d, 0xee, 0x00},
   {0x380e, 0x02, 0x00},
   {0x380f, 0x14, 0x00},
   {0x3811, 0x08, 0x00},
   {0x3813, 0x02, 0x00},
   {0x3814, 0x07, 0x00},
   {0x3815, 0x01, 0x00},
   {0x3816, 0x07, 0x00},
   {0x3817, 0x01, 0x00},
   {0x381e, 0x02, 0x00},
   {0x3820, 0x8d, 0x00},
   {0x3821, 0x00, 0x00},
   {0x4008, 0x00, 0x00},
   {0x4009, 0x03, 0x00},
   {0x4041, 0x03, 0x00},
   {0x3501, 0x0F, 0x00},
   {0x3502, 0x80, 0x00},
};
static struct msm_camera_i2c_reg_setting res_settings[] = {
  {
    .reg_setting = res0_reg_array,
    .size = ARRAY_SIZE(res0_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  {
    .reg_setting = res1_reg_array,
    .size = ARRAY_SIZE(res1_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  {
    .reg_setting = res2_reg_array,
    .size = ARRAY_SIZE(res2_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  {
    .reg_setting = res3_reg_array,
    .size = ARRAY_SIZE(res3_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  {
    .reg_setting = res4_reg_array,
    .size = ARRAY_SIZE(res4_reg_array),
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
  &ov5675_d5v15b_csi_params, /* RES 0*/
  &ov5675_d5v15b_csi_params, /* RES 1*/
  &ov5675_d5v15b_csi_params, /* RES 2*/
  &ov5675_d5v15b_csi_params, /* RES 3*/
  &ov5675_d5v15b_csi_params, /* RES 4*/
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
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
  {
    .x_output = 2592,
    .y_output = 1944,
    .line_length_pclk = 750,
    .frame_length_lines = 2000,
    .vt_pixel_clk = 45000000,
    .op_pixel_clk = 180000000,
    .binning_factor = 1,
    .max_fps = 30.0,
    .min_fps = 7.5,
    .mode = SENSOR_DEFAULT_MODE,
  },
  {
    .x_output = 1296,
    .y_output = 972,
    .line_length_pclk = 750,
    .frame_length_lines = 2000,
    .vt_pixel_clk = 45000000,
    .op_pixel_clk = 180000000,
    .binning_factor = 2,
    .max_fps = 30.0,
    .min_fps = 7.5,
    .mode = SENSOR_DEFAULT_MODE,
  },
  {
    .x_output = 1280,
    .y_output = 720,
    .line_length_pclk = 750,
    .frame_length_lines = 1000,
    .vt_pixel_clk = 45000000,
    .op_pixel_clk = 180000000,
    .binning_factor = 1,
    .max_fps = 60.0,
    .min_fps = 60.0,
    .mode = SENSOR_HFR_MODE,
  },
  {
    .x_output = 640,
    .y_output = 480,
    .line_length_pclk = 750,
    .frame_length_lines = 666,
    .vt_pixel_clk = 45000000,
    .op_pixel_clk = 180000000,
    .binning_factor = 1,
    .max_fps = 90.0,
    .min_fps = 90.0,
    .mode = SENSOR_HFR_MODE,
  },
  {
    .x_output = 640,
    .y_output = 480,
    .line_length_pclk = 750,
    .frame_length_lines = 532,
    .vt_pixel_clk = 45000000,
    .op_pixel_clk = 180000000,
    .binning_factor = 1,
    .max_fps = 120.0,
    .min_fps = 120.0,
    .mode = SENSOR_HFR_MODE,
  },
};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t ov5675_d5v15b_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t ov5675_d5v15b_res_table = {
  .res_cfg_type = ov5675_d5v15b_res_cfg,
  .size = ARRAY_SIZE(ov5675_d5v15b_res_cfg),
};

static struct sensor_lib_chromatix_t ov5675_d5v15b_chromatix[] = {
  {
    .common_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(zsl), /* RES0 */
    .camera_snapshot_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(snapshot), /* RES0 */
    .camcorder_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(default_video), /* RES0 */
    .liveshot_chromatix =  ov5675_d5v15b_LOAD_CHROMATIX(liveshot), /* RES0 */
  },
  {
    .common_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(preview), /* RES1 */
    .camera_snapshot_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(preview), /* RES1 */
    .camcorder_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(default_video), /* RES1 */
    .liveshot_chromatix =  ov5675_d5v15b_LOAD_CHROMATIX(liveshot), /* RES1 */
  },
  {
    .common_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_60fps), /* RES2 */
    .camera_snapshot_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_60fps), /* RES2 */
    .camcorder_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_60fps), /* RES2 */
    .liveshot_chromatix =  ov5675_d5v15b_LOAD_CHROMATIX(liveshot), /* RES2 */
  },
  {
    .common_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_90fps), /* RES3 */
    .camera_snapshot_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_90fps), /* RES3 */
    .camcorder_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_90fps), /* RES3 */
    .liveshot_chromatix =  ov5675_d5v15b_LOAD_CHROMATIX(liveshot), /* RES3 */
  },
  {
    .common_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_120fps), /* RES4 */
    .camera_snapshot_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_120fps), /* RES4 */
    .camcorder_chromatix = ov5675_d5v15b_LOAD_CHROMATIX(hfr_120fps), /* RES4 */
    .liveshot_chromatix =  ov5675_d5v15b_LOAD_CHROMATIX(liveshot), /* RES4 */
  },
};

static struct sensor_lib_chromatix_array ov5675_d5v15b_lib_chromatix_array = {
  .sensor_lib_chromatix = ov5675_d5v15b_chromatix,
  .size = ARRAY_SIZE(ov5675_d5v15b_chromatix),
};

/*===========================================================================
 * FUNCTION    - ov5675_d5v15b_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static uint16_t ov5675_d5v15b_real_to_register_gain(float gain)
{
  uint16_t reg_gain;

  if (gain < 1.0) {
      gain = 1.0;
  } else if (gain > 15.5) {
      gain = 15.5;
  }
  gain = (gain) * 128.0;
  reg_gain = (uint16_t) gain;

  return reg_gain;
}

/*===========================================================================
 * FUNCTION    - ov5675_d5v15b_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static float ov5675_d5v15b_register_to_real_gain(uint16_t reg_gain)
{
  float real_gain;

  if (reg_gain < 0x80) {
      reg_gain = 0x80;
  } else if (reg_gain > 0x7C0) {
      reg_gain = 0x7C0;
  }
  real_gain = (float) reg_gain / 128.0;

  return real_gain;
}

/*===========================================================================
 * FUNCTION    - ov5675_d5v15b_calculate_exposure -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t ov5675_d5v15b_calculate_exposure(float real_gain,
  uint16_t line_count, sensor_exposure_info_t *exp_info)
{
  if (!exp_info) {
    return -1;
  }
  exp_info->reg_gain = ov5675_d5v15b_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain = ov5675_d5v15b_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain = real_gain / exp_info->sensor_real_gain;
  exp_info->line_count = line_count;
  exp_info->sensor_digital_gain = 0x1;
  return 0;
}

/*===========================================================================
 * FUNCTION    - ov5675_d5v15b_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t ov5675_d5v15b_fill_exposure_array(uint16_t gain, uint32_t line,
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
  DEBUG_INFO("%s:gain=%d,line=%d,fl_lines=%d",__func__,gain,line,fl_lines);

  line = line/2;

  if(line < 4)
  {
   line = 4;
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
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xffff) >> 12;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0x0fff) >> 4;
  reg_count++;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 2;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0x0f) << 4;
  reg_count++;



  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0x7FF) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr + 1;
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
  reg_setting->delay = 0;

  return rc;
}

static sensor_exposure_table_t ov5675_d5v15b_expsoure_tbl = {
  .sensor_calculate_exposure = ov5675_d5v15b_calculate_exposure,
  .sensor_fill_exposure_array = ov5675_d5v15b_fill_exposure_array,
};

static sensor_lib_t sensor_lib_ptr = {
  /* sensor actuator name */
  .actuator_name = "\0",
  /* sensor slave info */
  .sensor_slave_info = &sensor_slave_info,
  /* sensor init params */
  .sensor_init_params = &sensor_init_params,
  /* sensor output settings */
  .sensor_output = &sensor_output,
  /* sensor eeprom name */
  .eeprom_name = "\0",
  /* sensor output register address */
  .output_reg_addr = &output_reg_addr,
  /* sensor exposure gain register address */
  .exp_gain_info = &exp_gain_info,
  /* sensor aec info */
  .aec_info = &aec_info,
  /* sensor snapshot exposure wait frames info */
  .snapshot_exp_wait_frames = 2,
  /* number of frames to skip after start stream */
  .sensor_num_frame_skip = 2,
  /* number of frames to skip after start HDR stream */
  .sensor_num_HDR_frame_skip = 2,
  /* sensor pipeline immediate delay */
  .sensor_max_pipeline_frame_delay = 2,
  /* sensor exposure table size */
  .exposure_table_size = 10,
  /* sensor lens info */
  .default_lens_info = &default_lens_info,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = ov5675_d5v15b_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(ov5675_d5v15b_cid_cfg),
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
  .sensor_res_cfg_table = &ov5675_d5v15b_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &ov5675_d5v15b_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &ov5675_d5v15b_expsoure_tbl,
  /* chromatix array */
  .chromatix_array = &ov5675_d5v15b_lib_chromatix_array,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
  .sync_exp_gain = 1,
};

/*===========================================================================
 * FUNCTION    - ov5675_d5v15b_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *ov5675_d5v15b_open_lib(void)
{
  return &sensor_lib_ptr;
}
