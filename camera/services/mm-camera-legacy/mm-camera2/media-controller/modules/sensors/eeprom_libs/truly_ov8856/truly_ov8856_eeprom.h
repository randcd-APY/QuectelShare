/* truly_ov8856_eeprom.h
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef  __truly_ov8856_EEPROM_H__
#define __truly_ov8856_EEPROM_H__

#include <stdio.h>
#include <string.h>
#include "eeprom.h"
#include "chromatix.h"
#include "sensor_common.h"

#undef DEBUG_INFO
/* #define truly_ov8856_DEBUG */
#ifdef truly_ov8856_DEBUG
#define DEBUG_INFO(fmt, args...) SERR(fmt, ##args)
#else
#define DEBUG_INFO(fmt, args...) do { } while (0)
#endif

#define BASE_ADDR 0x7010
#define LSC_FLAG_ADDR 0x7028

#define WB_OFFSET 5
#define WB_GROUP_SIZE 5
#define AWB_REG_SIZE 3

#define LSC_OFFSET (LSC_FLAG_ADDR-BASE_ADDR)
#define LSC_REG_SIZE 241

#define RG_RATIO_TYPICAL_VALUE 0x146
#define BG_RATIO_TYPICAL_VALUE 0x137

#define ABS(x)            (((x) < 0) ? -(x) : (x))

struct otp_struct {
  uint16_t flag;
  uint16_t module_integrator_id;
  uint16_t lens_id;
  uint16_t production_year;
  uint16_t production_month;
  uint16_t production_day;
  uint16_t rg_ratio;
  uint16_t bg_ratio;
  uint16_t lenc[240];
  uint16_t checksum;
} otp_data;

void truly_ov8856_get_calibration_items(void *e_ctrl);
void truly_ov8856_format_calibration_data(void *e_ctrl);
int truly_ov8856_get_raw_data(void *e_ctrl, void *data);

static eeprom_lib_func_t truly_ov8856_lib_func_ptr = {
  .get_calibration_items = truly_ov8856_get_calibration_items,
  .format_calibration_data = truly_ov8856_format_calibration_data,
  .do_af_calibration = NULL,
  .do_wbc_calibration = NULL,
  .do_lsc_calibration = NULL,
  .do_dpc_calibration = NULL,
  .get_dpc_calibration_info = NULL,
  .get_raw_data = truly_ov8856_get_raw_data,
};

#endif
