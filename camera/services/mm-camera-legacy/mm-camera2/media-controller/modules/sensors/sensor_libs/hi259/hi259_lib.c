 /*============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#include <stdio.h>
#include "sensor_lib.h"

#define SENSOR_MODEL_NO_hi259 "hi259"
#define hi259_LOAD_CHROMATIX(n) \
  "libchromatix_"SENSOR_MODEL_NO_hi259"_"#n".so"

#undef DEBUG_INFO
//#define HI259_8909_DEBUG

#ifdef HI259_8909_DEBUG
#include <utils/Log.h>
#define SERR(fmt, args...) \
  ALOGE("%s:%d "fmt"\n", __func__, __LINE__, ##args)

#define DEBUG_INFO(fmt, args...) SERR(fmt, ##args)
#else
#define DEBUG_INFO(fmt, args...) do { } while (0)
#endif

#define SNAPSHOT_PARMS 1
#define PREVIEW_PARMS 1

static sensor_lib_t sensor_lib_ptr;

static struct msm_sensor_power_setting hi259_power_setting[] = {
	 {
	   .seq_type = SENSOR_GPIO,
	   .seq_val = SENSOR_GPIO_STANDBY,
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
	   .seq_type = SENSOR_VREG,
	   .seq_val = CAM_VIO,
	   .config_val = 0,
	   .delay = 5,
	 },
     /*
	 {
	   .seq_type = SENSOR_GPIO,
       .seq_val = SENSOR_GPIO_VIO,
       .config_val = GPIO_OUT_HIGH,
       .delay = 5,
  	 },
     */
	 {
	   .seq_type = SENSOR_VREG,
	   .seq_val = CAM_VANA,
	   .config_val = 0,
	   .delay = 5,
	 },
     /*
	 {
	   .seq_type = SENSOR_GPIO,
       .seq_val = SENSOR_GPIO_VANA,
       .config_val = GPIO_OUT_HIGH,
       .delay = 5,
  	 },
     */
	 {
	   .seq_type = SENSOR_CLK,
	   .seq_val = SENSOR_CAM_MCLK,
	   .config_val = 24000000,
	   .delay = 5,
	 },
	 {
	   .seq_type = SENSOR_GPIO,
	   .seq_val = SENSOR_GPIO_STANDBY,
	   .config_val = GPIO_OUT_LOW,
	   .delay = 10,
	 },
	 {
	   .seq_type = SENSOR_GPIO,
	   .seq_val = SENSOR_GPIO_RESET,
	   .config_val = GPIO_OUT_HIGH,
	   .delay = 2,
	 },
	 {
	   .seq_type = SENSOR_I2C_MUX,
	   .seq_val = 0,
	   .config_val = 0,
	   .delay = 1,
	 },
};

static struct msm_sensor_power_setting hi259_power_down_setting[] = {
 	 {
	   .seq_type = SENSOR_I2C_MUX,
	   .seq_val = 0,
	   .config_val = 0,
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
	   .seq_val = SENSOR_GPIO_STANDBY,
	   .config_val = GPIO_OUT_HIGH,
	   .delay = 1,
	 },
	 {
	   .seq_type = SENSOR_VREG,
	   .seq_val = CAM_VANA,
	   .config_val = 0,
	   .delay = 5,
	 },
     /*
	 {
	   .seq_type = SENSOR_GPIO,
       .seq_val = SENSOR_GPIO_VANA,
       .config_val = GPIO_OUT_HIGH,
       .delay = 5,
  	 },
     */
	 {
	   .seq_type = SENSOR_VREG,
	   .seq_val = CAM_VIO,
	   .config_val = 0,
	   .delay = 1,
	 },
     /*
	 {
	   .seq_type = SENSOR_GPIO,
       .seq_val = SENSOR_GPIO_VIO,
       .config_val = GPIO_OUT_HIGH,
       .delay = 5,
  	 },
     */
};

static struct msm_camera_sensor_slave_info sensor_slave_info = {
  /* Camera slot where this camera is mounted */
  .camera_id = CAMERA_1,
  /* sensor slave address */
  .slave_addr = 0x60,
  /*sensor i2c frequency*/
  .i2c_freq_mode = I2C_FAST_MODE,
  /* sensor address type */
  .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
  /* sensor id info*/
  .sensor_id_info = {
  /* sensor id register address */
  .sensor_id_reg_addr = 0x04,
  /* sensor id */
  .sensor_id = 0xE103,
  },
  /* power up / down setting */
  .power_setting_array = {
    .power_setting = hi259_power_setting,
    .size = ARRAY_SIZE(hi259_power_setting),
    .power_down_setting = hi259_power_down_setting,
    .size_down = ARRAY_SIZE(hi259_power_down_setting),
  },
};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = 1,
  .position = 1,
  .sensor_mount_angle = 270,
};

static sensor_output_t sensor_output = {
  .output_format = SENSOR_BAYER,
  .connection_mode = SENSOR_MIPI_CSI,
  .raw_output = SENSOR_10_BIT_DIRECT,
};

static struct msm_sensor_output_reg_addr_t output_reg_addr = {
  .x_output = 0x2a,
  .y_output = 0x28,
  .line_length_pclk = 0x4c, 
  .frame_length_lines = 0xff,
};

static struct msm_sensor_exp_gain_info_t exp_gain_info = {
  .coarse_int_time_addr = 0x22,	// coarse int time read addr 0x20a2
  .global_gain_addr = 0x60,
  .vert_offset = 4,				
};

static sensor_aec_data_t aec_info = {
  .max_gain = 8.0,
  .max_linecount = 8000,	
};

static sensor_lens_info_t default_lens_info = {
  .focal_length = 3.16,
  .pix_size = 1.75,
  .f_number = 2.8,
  .total_f_dist = 1.2,
  .hor_view_angle = 56.0,
  .ver_view_angle = 42.0,
};

#ifdef VFE_40	//?
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x0004,
  .csi_lane_mask = 0x18,
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 0,
};
#else
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x0004,
  .csi_lane_mask = 0x18, // 0x03 = 1lane, 0x07 = 2lane
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 0,
};
#endif

static struct msm_camera_i2c_reg_array init_reg_array[] = {
{0x03, 0x00},
{0x01, 0x01},	// sleep on
{0x01, 0x03},	// soft reset
{0x01, 0x01},	// disable Reset

//////////////////
// BGR enable
//////////////////
{0x03, 0x02}, 
{0x1f, 0x01},  //bgr_en

//////////////////
// PLL Setting
//////////////////
// MIPI_4x = 360 , ISP_clk = 144Mhz , OPCLK = 72Mhz, PXL_clk = 72 Mhz
{0x03, 0x00}, 
{0x07, 0x05},  // pll enable & pre div setting       
{0x08, 0x5a},  // pll main div : 4x ~ 127x           
{0x09, 0x13},  // isp div : 2/5 mipi 4x div : 1/1 mipi 1x div : 1/5  
{0x07, 0x85},  // pll enable & pre div setting       
{0x07, 0x85},  // pll enable & pre div setting       
{0x07, 0x85},  // pll enable & pre div setting       
{0x0a, 0x80},  // pll_clk_sw select & clk inv option 
{0x07, 0xC5},  // pll enable & pre div setting  

//////////////////
// One Line = 1800 !!!
//////////////////

//////////////////
// Page 0x00
//////////////////
{0x03, 0x00},
{0x10, 0x00},
{0x11, 0x80},
{0x13, 0x01},   // frame skip option on
{0x14, 0x20},
{0x15, 0x81},	//group_retiming_enable[0]
{0x17, 0x10},
{0x1a, 0x00},
{0x1c, 0x00},
{0x1f, 0x00},
{0x20, 0x00},
{0x21, 0x10},
{0x22, 0x00},
{0x23, 0x0e},
{0x24, 0x00},
{0x25, 0x10},
{0x26, 0x00},
{0x27, 0x0e},
{0x28, 0x04},
{0x29, 0xb0},		//win_height 1200
{0x2a, 0x06},
{0x2b, 0x40},		//win_width 1600
{0x30, 0x00},
{0x31, 0x00},
{0x32, 0x00},
{0x33, 0x00},
{0x34, 0x00},
{0x35, 0x00},
{0x36, 0x00},
{0x37, 0x00},
{0x38, 0x02},
{0x39, 0x66},
{0x3a, 0x03},
{0x3b, 0x30},
{0x4c, 0x07},
{0x4d, 0x08},
{0x4e, 0x05},
{0x4f, 0x14},	//frame_length_line 1300
{0x54, 0x02},
{0x55, 0x03},
{0x56, 0x04},
{0x57, 0x40},
{0x58, 0x03},
{0x5c, 0x0a},
{0x60, 0x00},
{0x61, 0x00},
{0x62, 0x80},
{0x68, 0x03},
{0x69, 0x42},
{0x80, 0x00},
{0x81, 0x00},
{0x82, 0x00},
{0x83, 0x00},
{0x84, 0x06},
{0x85, 0x60},
{0x86, 0x04},
{0x87, 0xcc},
{0x88, 0x00},
{0x89, 0x00},
{0x8a, 0x02},
{0x8b, 0x66},
{0x90, 0x00},
{0x91, 0x02},
{0xa0, 0x01},
{0xa1, 0x40},
{0xa2, 0x40},
{0xa3, 0x40},
{0xa4, 0x40},
{0xe4, 0x10},
{0xe5, 0x00},

//////////////////
// Page 0x01
//////////////////
{0x03, 0x01},
{0x10, 0x21}, 	//2014.08.20.a
{0x11, 0x00},	//2016.02.03    0x00 --> 0x08 en_color_blc_ofs
{0x12, 0x3f}, 	//2014.10.23.a  adaptive region enable
{0x13, 0x08}, 	//2014.08.25.a
{0x14, 0x04}, 	//2014.08.20.a
{0x15, 0x01}, 	//2014.08.25.a
{0x16, 0x00},
{0x17, 0x02},	//check
{0x18, 0x00},	
{0x19, 0x00},
{0x20, 0x60},
{0x21, 0x00},
{0x22, 0x20},   
{0x23, 0x3c},   
{0x24, 0x5c},
{0x25, 0x00},
{0x26, 0x60},
{0x27, 0x07},
{0x28, 0x80},
{0x29, 0x00},
{0x2a, 0xff},
{0x2b, 0x20},
{0x2c, 0x80},
{0x2d, 0x80},
{0x2e, 0x80},
{0x2f, 0x80},
{0x85, 0x10},
///////////////////////////////////
// 2014.10.23 added
///////////////////////////////////
{0x30, 0x7b}, //blc adaptive time region setting
{0x31, 0x01},
{0x32, 0xfe},
{0x33, 0x00},
{0x34, 0x51},
{0x35, 0xb6},
{0x36, 0xfc},
{0x38, 0x66}, //man blc float
{0x39, 0x66},

{0x40, 0x00}, //man blc integer
{0x41, 0x00},
{0x42, 0x00},
{0x43, 0x00},
{0x44, 0x00},
{0x45, 0x00},

{0x48, 0x00},
{0x49, 0x00},
{0x4a, 0x00},
{0x4b, 0x00},
{0x4c, 0x00},
{0x4d, 0x00},

{0x50, 0x00},
{0x51, 0x00},
{0x52, 0x00},
{0x53, 0x00},
{0x54, 0x00},
{0x55, 0x00},

{0x58, 0x00},
{0x59, 0x00},
{0x5a, 0x00},
{0x5b, 0x00},
{0x5c, 0x00},
{0x5d, 0x00},
/////////////////////////////////////
{0x80, 0x00},
{0x81, 0x00},
{0x82, 0x00},
{0x83, 0x00},
{0x88, 0x20}, //2014.08.19.b
{0x8a, 0x30}, //2014.08.19.b
{0x8c, 0x00},
{0x90, 0x00},
{0x91, 0x60},
{0x92, 0x00},
{0x93, 0x60},

//////////////////
// Page 0x02
//////////////////
{0x03, 0x02},
{0x10, 0x00},
{0x11, 0x00},
{0x12, 0x70}, //2015.02.03
{0x13, 0x00},
{0x16, 0x00},
{0x17, 0x00},
{0x19, 0x00},
{0x1a, 0x10}, 
{0x1b, 0x00},
{0x1c, 0xc0},
{0x1d, 0x20},
{0x20, 0x04},
{0x21, 0x04},
{0x22, 0x06},	
{0x23, 0x10},
{0x24, 0x04},
{0x28, 0x00},
{0x29, 0x06},	
{0x2a, 0x00},
{0x2e, 0x00},
{0x2f, 0x2c},
{0x30, 0x00},
{0x31, 0x44},
{0x32, 0x02},
{0x33, 0x00},
{0x34, 0x00},
{0x35, 0x00},
{0x36, 0x06},
{0x37, 0xc0},
{0x38, 0x00},
{0x39, 0x32},
{0x3a, 0x02},
{0x3b, 0x04},
{0x3c, 0x04},
{0x3d, 0xfe},
{0x3e, 0x00},
{0x3f, 0x00},
{0x40, 0x00},
{0x41, 0x17},	
{0x42, 0x01},
{0x43, 0x25},
{0x47, 0x00},
{0x48, 0x9a},
{0x49, 0x24},
{0x4a, 0x0f},
{0x4b, 0x20},
{0x4c, 0x06},
{0x4d, 0xc0},
{0x50, 0xa9},
{0x51, 0x1c},
{0x52, 0x73},
{0x54, 0xc0},
{0x55, 0x40},
{0x56, 0x11},
{0x57, 0x00},
{0x58, 0x18},
{0x59, 0x16},
{0x5b, 0x00},
{0x62, 0x00},
{0x63, 0xc8},
{0x67, 0x3f},
{0x68, 0xc0},
{0x70, 0x03},
{0x71, 0xc7},
{0x72, 0x06}, //linemem_pattern_test
{0x73, 0x75}, //2014.10.23.a //2014.08.25.a
{0x74, 0x03},
{0x75, 0xc7},
{0x76, 0x05},
{0x77, 0x1d}, //2014.10.23.a //2014.08.25.a
{0xa0, 0x01},
{0xa1, 0x48},
{0xa2, 0x02},
{0xa3, 0xde},
{0xa4, 0x02},
{0xa5, 0xde},
{0xa6, 0x06},
{0xa7, 0xf0},
{0xb0, 0x02},
{0xb1, 0x0f},
{0xb2, 0x02},
{0xb3, 0xdb}, //2014.08.18 ramp_clk_msk_off1_1x_l
{0xb4, 0x03},
{0xb5, 0xc7},
{0xb6, 0x06},
{0xb7, 0xd7}, //2014.08.18 ramp_clk_msk_off2_1x_l
{0xc0, 0x02},
{0xc1, 0x0f},
{0xc2, 0x02},
{0xc3, 0xdd},
{0xc4, 0x03},
{0xc5, 0xc7},
{0xc6, 0x06},
{0xc7, 0xdb}, //2014.08.18 ramp_preset_off2_1x_l
{0xc8, 0x01},
{0xc9, 0x8e},
{0xca, 0x01},
{0xcb, 0x3e},
{0xcc, 0x03},
{0xcd, 0x1e},
{0xce, 0x02},
{0xcf, 0xe2},
{0xd0, 0x00}, //2014.10.23.a
{0xd1, 0x00}, //2014.10.23.a
{0xd2, 0x00}, //2014.10.23.a
{0xd3, 0x00}, //2014.10.23.a
{0xd4, 0x0c}, //2014.10.23.a
{0xd5, 0x00}, //2014.10.23.a
{0xe0, 0x1c}, //2014.10.23.a  NCP setting
{0xe1, 0x1c}, //2014.10.23.a
{0xe2, 0x1c}, //2014.10.23.a
{0xe3, 0x04}, //2014.10.23.a
{0xe4, 0x1c}, //2014.10.23.a
{0xe5, 0x01},
{0xe8, 0x00},
{0xe9, 0x00},
{0xea, 0x00}, //2014.10.23.a
{0xeb, 0x00}, //2014.10.23.a
{0xec, 0x00}, //2014.10.23.a
{0xed, 0x00}, //2014.10.23.a
 
{0xf0, 0x70},
{0xf1, 0x00},
{0xf2, 0x82},
{0xf3, 0x00},

//////////////////
// Page 0x03
//////////////////
{0x03, 0x03},
{0x10, 0x00},
{0x11, 0x80},
{0x12, 0x00},
{0x13, 0x02},
{0x14, 0x06},
{0x15, 0xeb},
{0x16, 0x06},
{0x17, 0xf5},
{0x18, 0x01},
{0x19, 0x02},
{0x1a, 0x06},
{0x1b, 0xda},
{0x1c, 0x01},
{0x1d, 0x02},
{0x1e, 0x06},
{0x1f, 0xda},
{0x20, 0x01},
{0x21, 0x02},
{0x22, 0x01},
{0x23, 0x34},
{0x24, 0x01},
{0x25, 0x02},
{0x26, 0xff}, //2014.10.23.a
{0x27, 0xff}, //2014.10.23.a
{0x28, 0x01},
{0x29, 0x02},
{0x2a, 0x01},
{0x2b, 0x3e},
{0x2c, 0x01},
{0x2d, 0x02},
{0x2e, 0x01},
{0x2f, 0x3e},
{0x30, 0x01},
{0x31, 0x48},
{0x32, 0x06},
{0x33, 0xda},
{0x34, 0x01},
{0x35, 0x48},
{0x36, 0x06},
{0x37, 0xda},
{0x38, 0x06},
{0x39, 0xf1},
{0x3a, 0x06},
{0x3b, 0xfb},
{0x3c, 0x00},
{0x3d, 0x0c},
{0x3e, 0x00},
{0x3f, 0x16},
{0x40, 0x00},
{0x41, 0x04},
{0x42, 0x00},
{0x43, 0x45},
{0x44, 0x00},
{0x45, 0x02},
{0x46, 0x00},
{0x47, 0x74},
{0x48, 0x00},
{0x49, 0x06},
{0x4a, 0x00},
{0x4b, 0x42},
{0x4c, 0x00},
{0x4d, 0x06},
{0x4e, 0x00},
{0x4f, 0x42},
{0x50, 0x00},
{0x51, 0x0a},
{0x52, 0x00},
{0x53, 0x32},
{0x54, 0x00},
{0x55, 0x0a},
{0x56, 0x00},
{0x57, 0x32},
{0x58, 0x00},
{0x59, 0x0a},
{0x5A, 0x00},
{0x5b, 0x32},
{0x60, 0x00},
{0x61, 0x04},
{0x62, 0x00},
{0x63, 0x12},
{0x64, 0x00},
{0x65, 0x04},
{0x66, 0x00},
{0x67, 0x12},
{0x68, 0x00},
{0x69, 0x04},
{0x6A, 0x00},
{0x6B, 0x4a},
{0x70, 0x00},
{0x71, 0xda},
{0x72, 0x06},
{0x73, 0xe0},
{0x74, 0x00},
{0x75, 0xe0},
{0x76, 0x00},
{0x77, 0xfc},
{0x78, 0x00},
{0x79, 0xe0},
{0x7A, 0x00},
{0x7B, 0xfc},
{0x7C, 0x06},
{0x7D, 0xdc},
{0x7E, 0x06},
{0x7F, 0xe4},
{0x80, 0x02},
{0x81, 0xe2},
{0x82, 0x03},
{0x83, 0x28},
{0x84, 0x02},
{0x85, 0xe2},
{0x86, 0x03},
{0x87, 0x28},
{0x88, 0x06},
{0x89, 0xdc},
{0x8A, 0x06},
{0x8B, 0xe4},
{0x90, 0x00},
{0x91, 0xd9},
{0x92, 0x06},
{0x93, 0xd9},
{0x94, 0x00},
{0x95, 0xd9},
{0x96, 0x06},
{0x97, 0xd9},
{0x98, 0x06},
{0x99, 0xd9},
{0x9a, 0x00},
{0x9b, 0xd9},
{0x9c, 0x06},
{0x9d, 0xd9},
{0x9e, 0x00},
{0x9f, 0xd9},
{0xa0, 0x00},
{0xa1, 0x05},
{0xa2, 0x00},
{0xa3, 0x13},
{0xa4, 0x00},
{0xa5, 0x05},
{0xa6, 0x00},
{0xa7, 0x13},
{0xa8, 0x00},
{0xa9, 0xde},
{0xaa, 0x00},
{0xab, 0xee},
{0xac, 0x00},
{0xad, 0xdd},
{0xae, 0x00},
{0xaf, 0xed},
{0xb0, 0x06},
{0xb1, 0xe1},
{0xb2, 0x00},
{0xb3, 0xff},
{0xb4, 0x06},
{0xb5, 0xe1},
{0xb6, 0x00},
{0xb7, 0xff},
{0xe0, 0x00},
{0xe1, 0xda},
{0xe2, 0x02},
{0xe3, 0xde},
{0xe4, 0x03},
{0xe5, 0x05},
{0xe6, 0x06},
{0xe7, 0xda},
{0xe8, 0x01},
{0xe9, 0x00},
{0xea, 0x02},
{0xeb, 0xde},
{0xec, 0x06},
{0xed, 0xfc},
{0xee, 0x00},
{0xef, 0x00},
{0xf6, 0x00},
{0xf7, 0x12},
{0xf8, 0x00},
{0xf9, 0x1c},

//////////////////
// Page 0x04
//////////////////
{0x03, 0x04},
{0x10, 0x02}, //2014.10.23.a ramp_dn manual
{0x11, 0x04}, 	//check
{0x12, 0x00},
{0x13, 0x00},
{0x14, 0x02},
{0x1a, 0x00},
{0x1b, 0x30},
{0x1c, 0x00},
{0x1d, 0xc0},
{0x1e, 0x44}, //2014.08.18 ramp_hold_count
{0x20, 0x00},
{0x21, 0x38},
{0x22, 0x00},
{0x23, 0x70},
{0x24, 0x00},
{0x25, 0xa8},
{0x26, 0x00},
{0x27, 0xc5},
{0x28, 0x01},
{0x29, 0x8a},
{0x2a, 0x02},
{0x2b, 0x4f},
{0x30, 0x01}, 
{0x31, 0x3c}, 
{0x32, 0x01}, 
{0x33, 0x3c}, 
{0x34, 0x01}, 
{0x35, 0x34}, 
{0x36, 0x01}, 
{0x37, 0x02}, 
{0x38, 0x01}, 
{0x39, 0x02}, 
{0x3a, 0x01}, 
{0x3b, 0x02}, 
{0x40, 0x01}, 
{0x41, 0x3e}, 
{0x42, 0x01}, 
{0x43, 0x3e}, 
{0x44, 0x01}, 
{0x45, 0x3e}, 
{0x46, 0x01}, 
{0x47, 0x02}, 
{0x48, 0x01}, 
{0x49, 0x02}, 
{0x4a, 0x01}, 
{0x4b, 0x02}, 
{0x50, 0x00},
{0x58, 0x00},
{0x59, 0xdc},
{0x5a, 0x06},
{0x5b, 0xe0},
{0x5c, 0x00},
{0x5d, 0xdc},
{0x5e, 0x06},
{0x5f, 0xe0},
{0x60, 0x00},
{0x61, 0x60},
{0x62, 0x00},
{0x63, 0x40},
{0x64, 0x00},
{0x65, 0x60},
{0x66, 0x00},
{0x67, 0x40},
{0x68, 0x00},
{0x69, 0x60},
{0x6a, 0x00},
{0x6b, 0x40},
{0x70, 0x18}, //2014.10.23.a analog_adapt_ctl
{0x71, 0x20}, //2014.10.23.a clamp
{0x72, 0x20}, 
{0x73, 0x00}, 
{0x80, 0x6f},
{0x81, 0x00},
{0x82, 0x2f},	
{0x83, 0x00},
{0x84, 0x19},	
{0x85, 0x01},
{0x86, 0x00},
{0x87, 0x00},
{0x90, 0x03},
{0x91, 0x06},
{0x92, 0x06},
{0x93, 0x06},
{0x94, 0x06},
{0x95, 0x00}, //2014.10.23.a ramp_vrst_ctl
{0x96, 0x40}, 
{0x97, 0x50}, 
{0x98, 0x70}, 
{0xa0, 0x06},
{0xa1, 0xda},
{0xa2, 0x06},
{0xa3, 0xda},
{0xa4, 0x06},
{0xa5, 0xda},
{0xa6, 0x01},
{0xa7, 0x02},
{0xa8, 0x01},
{0xa9, 0x02},
{0xaa, 0x01},
{0xab, 0x02},
 
{0xb0, 0x04},
{0xb1, 0x04},
{0xb2, 0x00},
{0xb3, 0x04},
{0xb4, 0x00},
{0xc0, 0x00},
{0xc1, 0x48},
{0xc2, 0x00},
{0xc3, 0x6e},
{0xc4, 0x00},
{0xc5, 0x4d},
{0xc6, 0x00},
{0xc7, 0x6c},
{0xc8, 0x00},
{0xc9, 0x4f},
{0xca, 0x00},
{0xcb, 0x6a},
{0xcc, 0x00},
{0xcd, 0x50},
{0xce, 0x00},
{0xcf, 0x68},
{0xd0, 0x07},
{0xd1, 0x00},
{0xd2, 0x03},
{0xd3, 0x03},
{0xe0, 0x00},
{0xe1, 0x10},
{0xe2, 0x67},
{0xe3, 0x00},

//////////////////
// Page 0x08
//////////////////
{0x03, 0x08},
{0x10, 0x07},
{0x20, 0x01},
{0x21, 0x00},
{0x22, 0x01},
{0x23, 0x00},
{0x24, 0x01},
{0x25, 0x00},
{0x26, 0x01},
{0x27, 0x00},
{0x28, 0x01},
{0x29, 0x00},
{0x2a, 0x01},
{0x2b, 0x00},
{0x2c, 0x01},
{0x2d, 0x00},
{0x2e, 0x01},
{0x2f, 0x00},
{0x30, 0x03},
{0x31, 0xff},
{0x32, 0x03},
{0x33, 0xff},
{0x34, 0x03},
{0x35, 0xff},
{0x36, 0x03},
{0x37, 0xff},
{0x40, 0x07},
{0x50, 0x01},
{0x51, 0x00},
{0x52, 0x01},
{0x53, 0x00},
{0x54, 0x0f},
{0x55, 0xff},

//////////////////
// Page 0x10
//////////////////
{0x03, 0x10},
{0x10, 0x00},
{0x11, 0x00},

//////////////////
// Page 0x20
//////////////////
{0x03, 0x20},
{0x10, 0x00},
{0x11, 0x05},
{0x12, 0x03},
{0x22, 0x01}, //2014.08.18 exptime_line_h
{0x23, 0x4d}, //2014.08.18 exptime_line_l
{0x26, 0xff},
{0x27, 0xff},
{0x29, 0x00},
{0x2a, 0x02},
{0x2b, 0x00},
{0x2c, 0x04},
{0x30, 0x00},
{0x31, 0x04},
/////////////////////////////
// 2014.10.23 added
/////////////////////////////
{0x40, 0x09}, //bandtime pixel
{0x41, 0x1e},
{0x42, 0x60},
/////////////////////////
{0x52, 0x0f},
{0x53, 0xf3},	//pga_max
 
{0x60, 0x0f}, 
{0x61, 0x00},

{0x64, 0x0f},	
{0x65, 0x00},	//ramp_gain_max

//////////////////
// MIPI Setting 
//////////////////
{0x03, 0x05},  // mode_page       
{0x39, 0x55},  // lvds_bias_ctl   
{0x4c, 0x20},  // hs_wakeup_size_h
{0x4d, 0x00},  // hs_wakeup_size_l
{0x4e, 0x40},  // mipi_int_time_h 
{0x4f, 0x00},  // mipi_int_time_l 
            
{0x11, 0x00},  // lvds_ctl_2
{0x14, 0x00},  // ser_out_ctl_1      
{0x16, 0x12},  //check // lvds_inout_ctl1 
{0x18, 0x80},  // lvds_inout_ctl3 
{0x19, 0x00},  // lvds_inout_ctl4 
{0x1a, 0xf0},  // lvds_time_ctl   
{0x24, 0x2b},  // long_packet_id  
// CLK Lane Timing Control //
{0x32, 0x1e},  // clk_zero_time   
{0x33, 0x0f},  // clk_post_time   
{0x34, 0x06},  // clk_prepare_time
{0x35, 0x05},  // clk_trail_time  
{0x36, 0x01},  // clk_tlpx_time_dp
{0x37, 0x08},  // clk_tlpx_time_dn
// DATA Lane Timing Control //
{0x1c, 0x01},  // tlpx_time_l_dp  
{0x1d, 0x09},  // tlpx_time_l_dn  
{0x1e, 0x0f},  // hs_zero_time    
{0x1f, 0x0b},  // hs_trail_time   
{0x30, 0x07},  // l_pkt_wc_h   1600 * 5 / 4 = 2000  (MIPI Data Byte)
{0x31, 0xd0},  // l_pkt_wc_l   1600 * 5 / 4 = 2000  (MIPI Data Byte)
            
{0x10, 0x1d},  // lvds_ctl_1      

/////////////////
// Sleep OFF
/////////////////
{0x03, 0x00},
{0x01, 0x01},		// hjj modify //sleep off

};

static struct msm_camera_i2c_reg_setting init_reg_setting[] = {
  {
    .reg_setting = init_reg_array,
    .size = ARRAY_SIZE(init_reg_array),
    .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,		// WORD -> BYTE
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,		// WORD -> BYTE
    .delay = 0,
  },
};

static struct sensor_lib_reg_settings_array init_settings_array = {
  .reg_settings = init_reg_setting,
  .size = 1,
};

static struct msm_camera_i2c_reg_array start_reg_array[] = {
   {0x03, 0x00},	
   {0x01, 0x00},
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 10,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
   {0x03, 0x00},	
   {0x01, 0x01},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 10,
};

static struct msm_camera_i2c_reg_array groupon_reg_array[] = {
  {0x03, 0x00},	
  {0x1f, 0x01},
};

static struct msm_camera_i2c_reg_setting groupon_settings = {
  .reg_setting = groupon_reg_array,
  .size = ARRAY_SIZE(groupon_reg_array),
  .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_i2c_reg_array groupoff_reg_array[] = {
  {0x03, 0x00},	
  {0x1f, 0x00},
};

static struct msm_camera_i2c_reg_setting groupoff_settings = {
  .reg_setting = groupoff_reg_array,
  .size = ARRAY_SIZE(groupoff_reg_array),
  .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
};

static struct msm_camera_csid_vc_cfg hi259_cid_cfg[] = {
  {0, CSI_RAW10, CSI_DECODE_10BIT},
  {1, CSI_EMBED_DATA, CSI_DECODE_10BIT},
};

// 259 use 1 lane
static struct msm_camera_csi2_params hi259_csi_params = {
  .csid_params = {
    .lane_cnt = 1,									// 2 --> 1								
    .lut_params = {
      .num_cid = ARRAY_SIZE(hi259_cid_cfg),
      .vc_cfg = {
         &hi259_cid_cfg[0],
         &hi259_cid_cfg[1],
      },
    },
  },
  .csiphy_params = {
    .lane_cnt = 1,									// 2 --> 1
    .settle_cnt = 0x18,
	.combo_mode= 1,
  },
};

static struct sensor_pix_fmt_info_t rgb10[] = {
  {V4L2_PIX_FMT_SBGGR10},
};

static struct sensor_pix_fmt_info_t meta[] = {
  {MSM_V4L2_PIX_FMT_META},
};

static sensor_stream_info_t hi259_stream_info[] = {
  {1, &hi259_cid_cfg[0], rgb10},
  {1, &hi259_cid_cfg[1], meta},
};

static sensor_stream_info_array_t hi259_stream_info_array = {
  .sensor_stream_info = hi259_stream_info,
  .size = ARRAY_SIZE(hi259_stream_info),
};

static struct msm_camera_i2c_reg_array res0_reg_array[] = {
   {0x03, 0x00},	
   {0x01, 0x01}, //hjj modify
};

static struct msm_camera_i2c_reg_array res1_reg_array[] = {
   {0x03, 0x00},	
   {0x01, 0x01}, //hjj modify
};

static struct msm_camera_i2c_reg_setting res_settings[] = {
{  //capture
    .reg_setting = res0_reg_array,
    .size = ARRAY_SIZE(res0_reg_array),
    .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
  {//preview
    .reg_setting = res1_reg_array,
    .size = ARRAY_SIZE(res1_reg_array),
    .addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
  },
};

static struct sensor_lib_reg_settings_array res_settings_array = {
  .reg_settings = res_settings,
  .size = ARRAY_SIZE(res_settings),
};

static struct msm_camera_csi2_params *csi_params[] = {
  &hi259_csi_params, /* RES 0*/
  &hi259_csi_params, /* RES 1*/
  //&hi259_csi_params, /* RES 2*/
  //&hi259_csi_params, /* RES 3*/
  //&hi259_csi_params, /* RES 4*/
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
};

static struct sensor_crop_parms_t crop_params[] = {
  {0, 0, 0, 0}, /* RES 0 */
  {0, 0, 0, 0}, /* RES 1 */
  //{0, 0, 0, 0}, /* RES 2 */
  //{0, 0, 0, 0}, /* RES 3 */
  //{0, 0, 0, 0}, /* RES 4 */
};

static struct sensor_lib_crop_params_array crop_params_array = {
  .crop_params = crop_params,
  .size = ARRAY_SIZE(crop_params),
};

static struct sensor_lib_out_info_t sensor_out_info[] = {
//#if SNAPSHOT_PARMS
  {
    .x_output = 1600,
    .y_output = 1200,
  .line_length_pclk = 1800,
  .frame_length_lines = 1300, //1236, 
  .vt_pixel_clk = 72000000,
  .op_pixel_clk = 72000000,
  .binning_factor = 1,
  .max_fps = 30.0,
  .min_fps = 7.5,
  .mode = SENSOR_DEFAULT_MODE,
  },
//#endif

//#if PREVIEW_PARMS
  {
        .x_output = 1600,
        .y_output = 1200,
        .line_length_pclk = 1800,
        .frame_length_lines = 1300,
        .vt_pixel_clk = 72000000,
        .op_pixel_clk = 72000000,
        .binning_factor = 1,
        .min_fps = 7.5,
        .max_fps = 30,

  .mode = SENSOR_DEFAULT_MODE,
  },
//#endif
};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t hi259_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t hi259_res_table = {
  .res_cfg_type = hi259_res_cfg,
  .size = ARRAY_SIZE(hi259_res_cfg),
};

static struct sensor_lib_chromatix_t hi259_chromatix[] = {
  {
    .common_chromatix = hi259_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = hi259_LOAD_CHROMATIX(preview), // RES0 */
    .camera_snapshot_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES0 */
    .camcorder_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES0 */
    .liveshot_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES0 */
  },
  {
    .common_chromatix = hi259_LOAD_CHROMATIX(common),
    .camera_preview_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES2 */
    .camera_snapshot_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES2 */
    .camcorder_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES2 */
    .liveshot_chromatix = hi259_LOAD_CHROMATIX(preview), /* RES1 */
  },
};

static struct sensor_lib_chromatix_array hi259_lib_chromatix_array = {
  .sensor_lib_chromatix = hi259_chromatix,
  .size = ARRAY_SIZE(hi259_chromatix),
};

/*===========================================================================
 * FUNCTION    - hi259_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static uint16_t hi259_real_to_register_gain(float gain)
{
   uint16_t reg_gain;

#if 1 // hjj modify
    if(gain < 1.0) {
        gain = 1.0;
    } else if (gain > 8.0) {
        gain = 8.0;
    }

    reg_gain = (uint16_t)(512.0/gain-34);
#else
	if (gain < 1.0)
		gain = 1.0;
   if (gain > 8.0) 
      gain = 8.0;
    //reg_gain = (uint16_t)((256/gain - 14.6) * 2);
	reg_gain = (uint16_t)256/gain-32;
#endif
   return reg_gain;

}

/*===========================================================================
 * FUNCTION    - hi259_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static float hi259_register_to_real_gain(uint16_t reg_gain)
{
    float real_gain;
#if 1 // hjj modify
    if (reg_gain > 0x1DE)
        reg_gain = 0x1DE;

    real_gain = (float)512.0/(reg_gain + 34);
#else
    //real_gain = (float)256.0 / ((reg_gain * 0.5) + 14.6);
	  if (reg_gain > 0xe0)
        reg_gain = 0xe0;
    real_gain = (float)256.0/(reg_gain + 32);
#endif
    return real_gain;

}

/*===========================================================================
 * FUNCTION    - hi259_calculate_exposure -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t hi259_calculate_exposure(float real_gain,
  uint16_t line_count, sensor_exposure_info_t *exp_info)
{
  if (!exp_info) {
    return -1;
  }
  exp_info->reg_gain = hi259_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain = hi259_register_to_real_gain(exp_info->reg_gain);
  exp_info->digital_gain = real_gain / exp_info->sensor_real_gain;
  exp_info->line_count = line_count;
  exp_info->sensor_digital_gain = 0x1;
  return 0;
}

/*===========================================================================
 * FUNCTION    - hi259_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t hi259_fill_exposure_array(uint16_t gain, uint32_t line,
  uint32_t fl_lines, int32_t luma_avg, uint32_t fgain,
  struct msm_camera_i2c_reg_setting* reg_setting)
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint16_t i = 0;

  if (!reg_setting) {
    return -1;
  }

  DEBUG_INFO("%s:gain=%d,line=%d,fl_lines=%d",__func__,
    gain,line,fl_lines) ;
  for (i = 0; i < sensor_lib_ptr.groupon_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->reg_setting[reg_count].reg_addr = 0x03;
  reg_setting->reg_setting[reg_count].reg_data = 0x00;
  reg_setting->reg_setting[reg_count].reg_addr = sensor_lib_ptr.output_reg_addr->frame_length_lines;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr = sensor_lib_ptr.output_reg_addr->frame_length_lines + 1;
  reg_setting->reg_setting[reg_count].reg_data = fl_lines & 0xFF;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr = 0x03;
  reg_setting->reg_setting[reg_count].reg_data = 0x20;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = (line & 0xFF00) >> 8;
  reg_count = reg_count + 1;
  
    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = line & 0xFF;
  reg_count = reg_count + 1;


  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain >> 1) & 0xFF; //hjj modify // gain & 0xFE;
  reg_count = reg_count + 1;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = gain & 0x01;
  reg_count = reg_count + 1;

  for (i = 0; i < sensor_lib_ptr.groupoff_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_BYTE_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;
  DEBUG_INFO("%s reg_count:%d",__func__,reg_count);
  return rc;
}

static sensor_exposure_table_t hi259_expsoure_tbl = {
  .sensor_calculate_exposure = hi259_calculate_exposure,
  .sensor_fill_exposure_array = hi259_fill_exposure_array,
};

static sensor_lib_t sensor_lib_ptr = {
  /* sensor slave info */
  .sensor_slave_info = &sensor_slave_info,
  /* sensor init params */
  .sensor_init_params = &sensor_init_params,
  /* sensor actuator name */
  // .actuator_name = "dw9718",
  /* module eeprom name */
  /*.eeprom_name = "truly_cm9887qr",*/
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
  .sensor_num_frame_skip = 2,
  /* number of frames to skip after start HDR stream */
  .sensor_num_HDR_frame_skip = 2,
  /* sensor exposure table size */
  .exposure_table_size = 23,//reg_count
  /* sensor lens info */
  .default_lens_info = &default_lens_info,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = hi259_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(hi259_cid_cfg),
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
  .sensor_res_cfg_table = &hi259_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &hi259_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &hi259_expsoure_tbl,
  /* chromatix array */
  .chromatix_array = &hi259_lib_chromatix_array,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
};

/*===========================================================================
 * FUNCTION    - hi259_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *hi259_open_lib(void)
{
  return &sensor_lib_ptr;
}
