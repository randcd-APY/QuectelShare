/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include "../eeprom_util/eeprom.h"
#include "eeprom_lib.h"
#include "eeprom_util.h"
#include "debug_lib.h"
#include "sensor_lib.h"


#define QVALUE                1024.0

#define MODULE_INFO_OFFSET    0x0002
#define MODULE_INFO_LAST_BYTE 0x000B
#define MODULE_INFO_CHECKSUM  0x0001

#define AWB_3100K_OFFSET      0x0010
#define AWB_3100K_LAST_BYTE   0x001B
#define AWB_3100K_CHECKSUM    0x001C

#define AWB_4000K_OFFSET      0x001D
#define AWB_4000K_LAST_BYTE   0x0028
#define AWB_4000K_CHECKSUM    0x0029

#define AWB_5100K_OFFSET      0x002A
#define AWB_5100K_GOLD_OFFSET 0x0030
#define AWB_5100K_LAST_BYTE   0x0035
#define AWB_5100K_CHECKSUM    0x0036

#define MESH_HWROLLOFF_SIZE (17 * 13)

#define LSC_R_OFFSET          0x0040
#define LSC_GR_OFFSET         (LSC_R_OFFSET  + 2 * MESH_HWROLLOFF_SIZE)
#define LSC_GB_OFFSET         (LSC_GR_OFFSET + 2 * MESH_HWROLLOFF_SIZE)
#define LSC_B_OFFSET          (LSC_GB_OFFSET + 2 * MESH_HWROLLOFF_SIZE)
#define LSC_LAST_BYTE         0x0727
#define LSC_CHECKSUM          0x0728

typedef struct {
  unsigned char cal_version;
  unsigned char year;
  unsigned char month;
  unsigned char day;
  unsigned char module_id;
  unsigned char lens_id;
  unsigned char vcm_id;
  unsigned char vcm_driver_id;
  unsigned char color_temp;
  unsigned char mirror_flip;
} module_info_t;

typedef struct {
  unsigned char r_over_gr_l;
  unsigned char r_over_gr_h;
  unsigned char b_over_gr_l;
  unsigned char b_over_gr_h;
  unsigned char gb_over_gr_l;
  unsigned char gb_over_gr_h;
} awb_data_t;

static unsigned char awb_present = 0;
static unsigned char lsc_present = 0;

static void imx378_gt24c64_eeprom_get_calibration_items(void *e_ctrl);
static void imx378_gt24c64_eeprom_format_calibration_data(void *e_ctrl);
static int imx378_gt24c64_eeprom_get_raw_data(void *e_ctrl, void *data);

static eeprom_lib_func_t imx378_gt24c64_eeprom_lib_func_ptr = {
  .get_calibration_items    = imx378_gt24c64_eeprom_get_calibration_items,
  .format_calibration_data  = imx378_gt24c64_eeprom_format_calibration_data,
  .do_af_calibration        = eeprom_autofocus_calibration,
  .do_wbc_calibration       = eeprom_whitebalance_calibration,
  .do_lsc_calibration       = NULL,
  .get_raw_data             = NULL,
  .get_ois_raw_data         = NULL,
  .eeprom_info =
  {
    .power_setting_array =
    {
      .power_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 0,
        },
      },
      .size = 1,
      .power_down_setting_a =
      {
        {
          .seq_type = CAMERA_POW_SEQ_VREG,
          .seq_val = CAMERA_VIO,
          .config_val = 0,
          .delay = 0,
        },
      },
      .size_down = 1,
    },
    .i2c_freq_mode = SENSOR_I2C_MODE_FAST,
    .mem_map_array =
    {
      .memory_map =
      {
        {
          .slave_addr = 0xa0,
          .mem_settings =
          {
            { 0x0, CAMERA_I2C_WORD_ADDR,
              2048, CAMERA_I2C_BYTE_DATA, CAMERA_I2C_OP_READ, 0 },
          },
          .memory_map_size = 1,
        },
      },
      .size_map_array = 1,
    },
  },
};


