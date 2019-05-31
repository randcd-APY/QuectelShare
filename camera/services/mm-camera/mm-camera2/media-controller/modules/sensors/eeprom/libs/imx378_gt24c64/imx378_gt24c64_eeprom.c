/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "imx378_gt24c64_eeprom.h"

/** print_matrix:
 *    @paramlist: address of pointer to
 *                   chromatix struct
 *
 * Prints out debug logs
 *
 * This function executes in module sensor context
 *
 * Return:
 * void
 */
static void print_matrix(__attribute__((unused))float* paramlist)
{
  int j =0;
  for (j=0; j < MESH_HWROLLOFF_SIZE; j = j + 17) {
   SLOW("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f,%.2f, %.2f, %.2f,"
     "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f",
     paramlist[j], paramlist[j+1], paramlist[j+2], paramlist[j+3],
     paramlist[j+4], paramlist[j+5], paramlist[j+6], paramlist[j+7],
     paramlist[j+8], paramlist[j+9], paramlist[j+10], paramlist[j+11],
     paramlist[j+12], paramlist[j+13], paramlist[j+14], paramlist[j+15],
     paramlist[j+16]);
  }
}

/** imx378_gt24c64_eeprom_get_calibration_items:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get calibration capabilities and mode items.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void imx378_gt24c64_eeprom_get_calibration_items(void *e_ctrl)
{
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);

  e_items->is_wbc = awb_present ? TRUE : FALSE;
  e_items->is_lsc = lsc_present ? TRUE : FALSE;
  e_items->is_afc = FALSE;
  e_items->is_dpc = FALSE;
  e_items->is_insensor = FALSE;
  e_items->is_ois = FALSE;

  SLOW("is_wbc:%d,is_afc:%d,is_lsc:%d,is_dpc:%d,is_insensor:%d,\
      is_ois:%d",e_items->is_wbc,e_items->is_afc,
    e_items->is_lsc,e_items->is_dpc,e_items->is_insensor,
    e_items->is_ois);
}

/** imx378_gt24c64_checksum:
 *    @buff: address of page buffer
 *
 *
 **/
static unsigned char imx378_gt24c64_checksum(unsigned char *buf,
unsigned int first, unsigned int last, unsigned int position)
{
  unsigned char retval = 0;
  unsigned int i = 0, sum = 0;

  SDBG("Enter");
  for(i = first; i <= last; i++){
    sum += buf[i];
  }
  if(sum % 255 == buf[position]){
    SLOW("Checksum good, pos:%d, sum:%d buf: %d" , position,
      sum,buf[position]);
    retval = 1;
  }
  SDBG("Exit");
  return retval;
}

/** imx378_gt24c64_eeprom_format_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void imx378_gt24c64_eeprom_format_wbdata(
        sensor_eeprom_data_t *e_ctrl)
{
  awb_data_t    *wb, *golden_wb;
  float         r_over_gr, b_over_gr, gb_over_gr;
  float         golden_r_over_gr, golden_b_over_gr, golden_gb_over_gr;
  int           i;

  SLOW("Enter");

  /* Checksum for whole eeprom page */
  if (imx378_gt24c64_checksum(e_ctrl->eeprom_params.buffer,
    AWB_5100K_OFFSET, AWB_5100K_LAST_BYTE, AWB_5100K_CHECKSUM))
  {
    awb_present = TRUE;

    /* Get AWB data */
    wb = (awb_data_t *)(e_ctrl->eeprom_params.buffer + AWB_5100K_OFFSET);
    golden_wb = (awb_data_t *)(e_ctrl->eeprom_params.buffer + AWB_5100K_GOLD_OFFSET);

    r_over_gr =
      ((float)((wb->r_over_gr_h << 8) | wb->r_over_gr_l)) / QVALUE;
    b_over_gr =
      ((float)((wb->b_over_gr_h << 8) | wb->b_over_gr_l)) / QVALUE;
    gb_over_gr =
      ((float)((wb->gb_over_gr_h << 8) | wb->gb_over_gr_l)) / QVALUE;

    golden_r_over_gr =
      ((float)((golden_wb->r_over_gr_h << 8) | golden_wb->r_over_gr_l)) / QVALUE;
    golden_b_over_gr =
      ((float)((golden_wb->b_over_gr_h << 8) | golden_wb->b_over_gr_l)) / QVALUE;
    golden_gb_over_gr =
      ((float)((golden_wb->gb_over_gr_h << 8) | golden_wb->gb_over_gr_l)) / QVALUE;

    SLOW("AWB : r/gr = %f", r_over_gr);
    SLOW("AWB : b/gr = %f", b_over_gr);
    SLOW("AWB : gb/gr = %f", gb_over_gr);
    SLOW("AWB Golden : r/gr = %f", golden_r_over_gr);
    SLOW("AWB Golden : b/gr = %f", golden_b_over_gr);
    SLOW("AWB Golden : gb/gr = %f", golden_gb_over_gr);

    for (i = 0; i < AGW_AWB_MAX_LIGHT; i++) {
      e_ctrl->eeprom_data.wbc.r_over_g[i] = r_over_gr;
      e_ctrl->eeprom_data.wbc.b_over_g[i] = b_over_gr;
    }
    e_ctrl->eeprom_data.wbc.gr_over_gb = 1.0f / gb_over_gr;

  } else {
    SERR("failed: AWB checksum");
  }
  SLOW("Exit");
}

/** imx378_gt24c64_eeprom_format_lensshading:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of lens shading correction calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void imx378_gt24c64_eeprom_format_lsc(sensor_eeprom_data_t *e_ctrl)
{
  unsigned short i, light, grid_size;
  unsigned char  *a_r_gain, *a_gr_gain, *a_gb_gain, *a_b_gain, *higher_bits;
  unsigned char  *temp;
  unsigned char  flag;
  float          gain;

  SDBG("Enter");

  /* Checksum for lsc page */
  if (!imx378_gt24c64_checksum(e_ctrl->eeprom_params.buffer,
    LSC_R_OFFSET, LSC_LAST_BYTE, LSC_CHECKSUM))
  {
    SERR("failed: LSC checksum");
    return;
  }

  lsc_present = TRUE;

  a_r_gain = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_R_OFFSET]);
  a_gr_gain = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_GR_OFFSET]);
  a_gb_gain = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_GB_OFFSET]);
  a_b_gain = (unsigned char *) &(e_ctrl->eeprom_params.buffer[LSC_B_OFFSET]);

  for (i = 0; i < MESH_HWROLLOFF_SIZE; i++) {
    for(light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
      e_ctrl->eeprom_data.lsc.lsc_calib[light].r_gain[i] =
        (float) (a_r_gain[0] | (a_r_gain[1] << 8));
      e_ctrl->eeprom_data.lsc.lsc_calib[light].gr_gain[i] =
        (float) (a_gr_gain[0] | (a_gr_gain[1] << 8));
      e_ctrl->eeprom_data.lsc.lsc_calib[light].gb_gain[i] =
        (float) (a_gb_gain[0] | (a_gb_gain[1] << 8));
      e_ctrl->eeprom_data.lsc.lsc_calib[light].b_gain[i] =
        (float) (a_b_gain[0] | (a_b_gain[1] << 8));
    }
    a_r_gain += 2;
    a_gr_gain += 2;
    a_gb_gain += 2;
    a_b_gain += 2;
  }

  for(light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
    e_ctrl->eeprom_data.lsc.lsc_calib[light].mesh_rolloff_table_size
      = MESH_HWROLLOFF_SIZE;
  }

  for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
    SLOW("MESH R MATRIX %d", light);
    print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[light].r_gain);
    SLOW("MESH GR MATRIX %d", light);
    print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[light].gr_gain);
    SLOW("MESH GB MATRIX %d", light);
    print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[light].gb_gain);
    SLOW("MESH B MATRIX %d", light);
    print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[light].b_gain);
  }

  SDBG("Exit");
}

/** imx378_gt24c64_eeprom_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
static void imx378_gt24c64_eeprom_format_calibration_data(void *e_ctrl)
{
  sensor_eeprom_data_t * ctrl = (sensor_eeprom_data_t *)e_ctrl;
  module_info_t        *module_info;
  int i;

  awb_present = FALSE;
  lsc_present = FALSE;

  if(ctrl != NULL)
    SLOW("Total bytes in OTP buffer: %d", ctrl->eeprom_params.num_bytes);
  else {
    SERR("failed, e_ctrl is NULL");
    return;
  }

  if (!ctrl->eeprom_params.buffer || !ctrl->eeprom_params.num_bytes) {
    SERR("Buff pointer %p buffer size %d", ctrl->eeprom_params.buffer,
      ctrl->eeprom_params.num_bytes);
    return;
  }

  /* Checksum for whole eeprom page */
  if (!imx378_gt24c64_checksum(ctrl->eeprom_params.buffer,
    MODULE_INFO_OFFSET, MODULE_INFO_LAST_BYTE, MODULE_INFO_CHECKSUM))
  {
    SERR("failed: checksum");
    return;
  }

  /* Print module info */
   module_info = (module_info_t *)
     (ctrl->eeprom_params.buffer + MODULE_INFO_OFFSET);

   SLOW("Module ID : 0x%x", module_info->module_id);
   SLOW("Y/M/D : %d/%d/%d",
     module_info->year, module_info->month, module_info->day);

  imx378_gt24c64_eeprom_format_wbdata(ctrl);
  imx378_gt24c64_eeprom_format_lsc(ctrl);
}


/** imx378_gt24c64_eeprom_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* imx378_gt24c64_eeprom_open_lib(void) {
  return &imx378_gt24c64_eeprom_lib_func_ptr;
}
