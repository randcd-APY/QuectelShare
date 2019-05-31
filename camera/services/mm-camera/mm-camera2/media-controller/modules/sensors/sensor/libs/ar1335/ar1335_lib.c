/* imx274_lib.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <math.h>
#include "ar1335_lib.h"
#include "debug_lib.h"

/**
 * FUNCTION: sensor_real_to_register_gain
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/

static uint32_t gain_table[] =
{0x2010, 0x2014, 0x2018, 0x201C, 0x2020, 0x2022, 0x2024, 0x2026,
0x2028, 0x202A, 0x202C, 0x202E, 0x2030, 0x2031, 0x2032, 0x2033,
0x2034, 0x2035, 0x2036, 0x2037, 0x2038, 0x2039, 0x203A, 0x203B,
0x203C, 0x203D, 0x203E, 0x203F, 0x213F, 0x223F, 0x233F, 0x243F,
0x253F, 0x263F, 0x273F, 0x28BF, 0x29BF, 0x2ABF, 0x2BBF, 0x2CBF,
0x2DBF, 0x2EBF, 0x2FBF, 0x30BF, 0x31BF, 0x32BF, 0x33BF, 0x34BF,
0x35BF, 0x36BF, 0x37BF, 0x393F, 0x3A3F, 0x3B3F, 0x3C3F, 0x3D3F,
0x3E3F, 0x3F3F, 0x403F, 0x413F, 0x423F, 0x433F, 0x443F, 0x453F,
0x463F, 0x473F, 0x48BF, 0x49BF, 0x4ABF, 0x4BBF, 0x4CBF, 0x4DBF,
0x4EBF, 0x4FBF, 0x50BF, 0x51BF, 0x52BF, 0x53BF, 0x54BF, 0x55BF,
0x56BF, 0x57BF, 0x593F, 0x5A3F, 0x5B3F, 0x5C3F, 0x5D3F, 0x5E5F,
0x5F3F, 0x603F, 0x613F, 0x623F, 0x633F};

static unsigned int ar1335_real_to_register_gain(float real_gain, unsigned int *applied_total_gain)
{
  unsigned int reg_gain = 0;
  if (real_gain < AR1335_MIN_GAIN) {
    real_gain = AR1335_MIN_GAIN;
    SHIGH("AR1335 Requested gain(%f) is lower than the sensor minimum(%d)",
      real_gain, AR1335_MIN_GAIN);
  }
  else if (real_gain > AR1335_MAX_GAIN) {
    real_gain = AR1335_MAX_GAIN;
  }

  *applied_total_gain  = (unsigned int)(4.0*real_gain);
  reg_gain = gain_table[*applied_total_gain - 4];

  return reg_gain;
}

/**
 * FUNCTION: ar1335_digital_gain_calc
 *
 * DESCRIPTION: Calcuate the sensor digital gain
 **/
static unsigned int ar1335_digital_gain_calc(float sensor_real_gain)
{
    float sensor_dig_gain = AR1335_MIN_DIGITAL_GAIN;
    unsigned int reg_dig_gain = 1;

    if(sensor_real_gain > AR1335_MAX_ANALOG_GAIN)
    {
        sensor_dig_gain = sensor_real_gain / AR1335_MAX_ANALOG_GAIN;
    }
    else
    {
        sensor_dig_gain = AR1335_MIN_DIGITAL_GAIN;
    }

    if(sensor_dig_gain > AR1335_MAX_DIGITAL_GAIN)
    {
        sensor_dig_gain = AR1335_MAX_DIGITAL_GAIN;
    }

    reg_dig_gain = (unsigned int)(sensor_dig_gain * 256);

    return reg_dig_gain;

}


/**
 * FUNCTION: sensor_calculate_exposure
 *
 * DESCRIPTION: Calcuate the sensor exposure
 **/

static int sensor_calculate_exposure(float real_gain,
  unsigned int line_count, sensor_exposure_info_t* exp_info, float s_real_gain){

    unsigned int applied_total_gain = 0;
    float sensor_total_gain = 1.0;
    if (!exp_info)
    {
      return -1;
    }

    exp_info->reg_gain = ar1335_real_to_register_gain(real_gain, &applied_total_gain);
    sensor_total_gain = (float)applied_total_gain/4.0;
    exp_info->sensor_digital_gain =
              ar1335_digital_gain_calc(sensor_total_gain);
    exp_info->sensor_real_dig_gain =
      (float)exp_info->sensor_digital_gain / 256;
    exp_info->sensor_real_gain = sensor_total_gain / exp_info->sensor_real_dig_gain;
    exp_info->digital_gain =
      real_gain /(exp_info->sensor_real_gain * exp_info->sensor_real_dig_gain);
    exp_info->line_count = line_count;

    return 0;
}

/**
 * FUNCTION: sensor_fill_exposure_array
 *
 * DESCRIPTION: Fill the sensor exposure array
 **/

static int sensor_fill_exposure_array(unsigned int gain,
  unsigned int digital_gain, unsigned int line,
  unsigned int fl_lines, int luma_avg, unsigned int hdr_param,
  struct camera_i2c_reg_setting* reg_setting,
  __attribute__((unused)) unsigned int s_gain,
  __attribute__((unused)) int s_linecount,
  __attribute__((unused)) int hdr_mode) {

    int rc = 0;
    unsigned short reg_count = 0;
    unsigned short i = 0;
    int shr_dol1 = 0;
    int shr_dol2 = 0;

    if (!reg_setting)
    {
      return -1;
    }

    SERR("[%s][%d] gain:%d, line:%d, fl_lines:%d", __func__, __LINE__, gain, line, fl_lines);
    gain = (gain & 0xFFFF);

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
    reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0x0000FF00) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr.frame_length_lines + 1;
    reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0x000000FF);
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr;
    reg_setting->reg_setting[reg_count].reg_data = (line & 0x0000FF00) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.coarse_int_time_addr + 1;
    reg_setting->reg_setting[reg_count].reg_data = (line & 0x000000FF);
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr;
    reg_setting->reg_setting[reg_count].reg_data = (gain & 0x0000FF00) >> 8;
    reg_setting->reg_setting[reg_count].delay = 0;
    reg_count = reg_count + 1;

    reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info.global_gain_addr + 1;
    reg_setting->reg_setting[reg_count].reg_data = (gain & 0x000000FF);
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
