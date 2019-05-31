/* imx377_lib.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <stdio.h>
#include <string.h>
#include "eeprom_lib.h"
#include "debug_lib.h"
#include "pdaf_lib.h"
#include "imx377_lib.h"

/**
 * FUNCTION: imx377_real_to_register_gain
 *
 * DESCRIPTION: Calculate sensor analog gain register value
 **/
static unsigned int imx377_real_to_register_gain(float real_gain)
{
  unsigned int reg_gain = 0;

  if (real_gain < REAL_GAIN_SATURATION)
    real_gain = REAL_GAIN_PROTECTION;
  if (real_gain > REAL_A_GAIN_MAX)
    real_gain = REAL_A_GAIN_MAX;
  reg_gain = (unsigned int)(2048.0f - 2048.0f / real_gain);

  return reg_gain;
}

/**
 * FUNCTION: imx377_register_to_real_gain
 *
 * DESCRIPTION: Calculate real gain from analog register value
 **/
static float imx377_register_to_real_gain(unsigned int reg_gain)
{
  float gain;

  if (reg_gain > REGISTER_A_GAIN_MAX)
    reg_gain = REGISTER_A_GAIN_MAX;
  gain = 2048.0f / (2048.0f - (float)reg_gain);

  return gain;
}

/**
 * FUNCTION: imx377_real_to_digital_gain
 *
 * DESCRIPTION: Calculate sensor analog gain register value
 **/
static unsigned int imx377_real_to_digital_gain(float real_gain)
{
  unsigned int reg_gain = 0;
  /* 0 db */
  if (real_gain < REAL_A_GAIN_MAX)
    reg_gain = 0;
  /* 6 db : X2 */
  else if( real_gain < REAL_A_GAIN_MAX * 1.99)
    reg_gain = 1;
  /* 12 db : X4 */
  else if( real_gain < REAL_A_GAIN_MAX * 3.98)
    reg_gain = 2;
  /* 18 db : X8 */
  else
    reg_gain = 3;

  return reg_gain;
}

/**
 * FUNCTION: imx377_digital_gain_to_real
 *
 * DESCRIPTION: Calculate digital gain to real gain
 **/
static float imx377_digital_gain_to_real( unsigned int reg_gain)
{
  float real_gain = 0;

  if (reg_gain == 0)
    real_gain = 1.0;
  else if( reg_gain == 1)
    real_gain = 1.99;
  else if( reg_gain == 2)
    real_gain = 3.98;
  else if( reg_gain == 3)
    real_gain = REAL_D_GAIN_MAX;

  return real_gain;
}

/**
 * FUNCTION: sensor_calculate_exposure
 *
 * DESCRIPTION: Calculate the sensor exposure
 **/
int sensor_calculate_exposure(float real_gain,
  unsigned int line_count, sensor_exposure_info_t *exp_info, float s_real_gain)
{
    if (!exp_info)
    {
      return -1;
    }

    exp_info->line_count = line_count;
    /* (1) sensor digital gain */
    exp_info->sensor_digital_gain = imx377_real_to_digital_gain(real_gain);
    exp_info->sensor_real_dig_gain =
      imx377_digital_gain_to_real(exp_info->sensor_digital_gain);

    /* (2) sensor analog gain */
    exp_info->reg_gain =
      imx377_real_to_register_gain(real_gain / exp_info->sensor_real_dig_gain);
    exp_info->sensor_real_gain = imx377_register_to_real_gain(exp_info->reg_gain);

    /* (3) isp gain */
    exp_info->digital_gain =
     real_gain / (exp_info->sensor_real_gain * exp_info->sensor_real_dig_gain);

    exp_info->s_reg_gain = imx377_real_to_register_gain(s_real_gain);
    return 0;
}

/**
 * FUNCTION: imx377_fill_exposure_array
 *
 * DESCRIPTION: Fill the sensor exposure array
 **/
static int sensor_fill_exposure_array(unsigned int gain,
  unsigned int digital_gain, unsigned int line, unsigned int fl_lines,
  __attribute__((unused)) int luma_avg,
  __attribute__((unused)) unsigned int hdr_param,
  struct camera_i2c_reg_setting* reg_setting,
  __attribute__((unused)) unsigned int s_gain,
  __attribute__((unused)) signed int s_linecount,
  __attribute__((unused)) int hdr_mode)
{
    unsigned short i = 0;
    unsigned short reg_count = 0;

    if (!reg_setting)
    {
      return -1;
    }

    for (i = 0; i < sensor_lib_ptr.groupon_settings.size; i++)
    {
        reg_setting->reg_setting[reg_count].reg_addr =
        sensor_lib_ptr.groupon_settings.reg_setting_a[i].reg_addr;
        reg_setting->reg_setting[reg_count].reg_data =
        sensor_lib_ptr.groupon_settings.reg_setting_a[i].reg_data;
        reg_count = reg_count + 1;
    }

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines;
    reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF);
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines + 1;
    reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines + 2;
    reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0x0F0000) >> 16;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr = REG_DIGITAL_GAIN_R;
    reg_setting->reg_setting[reg_count].reg_data = digital_gain;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr;
    reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF);
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr + 1;
    reg_setting->reg_setting[reg_count].reg_data = (gain & 0x700) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;


    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr;
    reg_setting->reg_setting[reg_count].reg_data = ((fl_lines-line) & 0xFF);
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr + 1;
    reg_setting->reg_setting[reg_count].reg_data = ((fl_lines-line) & 0xFF00) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    for (i = 0; i < sensor_lib_ptr.groupoff_settings.size; i++)
    {
        reg_setting->reg_setting[reg_count].reg_addr =
        sensor_lib_ptr.groupoff_settings.reg_setting_a[i].reg_addr;
        reg_setting->reg_setting[reg_count].reg_data =
        sensor_lib_ptr.groupoff_settings.reg_setting_a[i].reg_data;
        reg_count = reg_count + 1;
    }

    reg_setting->size = reg_count;
    reg_setting->addr_type = CAMERA_I2C_WORD_ADDR;
    reg_setting->data_type = CAMERA_I2C_BYTE_DATA;
    reg_setting->delay = 0;

    return 0;
}

/**
 * FUNCTION: sensor_open_lib
 *
 * DESCRIPTION: Open sensor library and returns data pointer
 **/
void *sensor_open_lib(void)
{
  return &sensor_lib_ptr;
}
