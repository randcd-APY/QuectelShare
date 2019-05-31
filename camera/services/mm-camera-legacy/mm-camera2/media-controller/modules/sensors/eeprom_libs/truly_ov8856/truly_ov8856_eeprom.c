/* truly_ov8856_eeprom.c
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "truly_ov8856_eeprom.h"

struct msm_camera_i2c_reg_setting g_reg_setting;
struct msm_camera_i2c_reg_array g_reg_array[247];

/** truly_ov8856_get_calibration_items:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get calibration capabilities and mode items.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void truly_ov8856_get_calibration_items(void *e_ctrl)
{
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);
  e_items->is_insensor = TRUE;
  e_items->is_afc = FALSE;
  e_items->is_wbc = FALSE;
  e_items->is_lsc = FALSE;
  e_items->is_dpc = FALSE;
}

/** truly_ov8856_update_awb:
 *
 * Calculate and apply white balance calibration data
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void truly_ov8856_update_awb()
{
  uint16_t R_gain, G_gain, B_gain;
  uint16_t G_gain_R, G_gain_B ;
  uint16_t nR_G_gain, nB_G_gain, nG_G_gain;
  uint16_t nBase_gain;

  if (otp_data.flag & 0x40){
  SLOW("rg_ratio=0x%x,bg_ratio=0x%x",otp_data.rg_ratio,otp_data.bg_ratio) ;
  nR_G_gain = (RG_RATIO_TYPICAL_VALUE*1000) / otp_data.rg_ratio;
  nB_G_gain = (BG_RATIO_TYPICAL_VALUE*1000) / otp_data.bg_ratio;
  nG_G_gain = 1000;
  if (nR_G_gain < 1000 || nB_G_gain < 1000)
  {
    if (nR_G_gain < nB_G_gain)
      nBase_gain = nR_G_gain;
    else
      nBase_gain = nB_G_gain;
  }
  else
  {
    nBase_gain = nG_G_gain;
  }
  R_gain = 0x400 * nR_G_gain / (nBase_gain);
  B_gain = 0x400 * nB_G_gain / (nBase_gain);
  G_gain = 0x400 * nG_G_gain / (nBase_gain);

  SLOW("R_gain=0x%x,G_gain=0x%x,B_gain=0x%x",R_gain,G_gain,B_gain) ;

  if (R_gain > 0x400) {
    g_reg_array[g_reg_setting.size].reg_addr = 0x5019;
    g_reg_array[g_reg_setting.size].reg_data = R_gain >> 8;
    g_reg_setting.size++;
    g_reg_array[g_reg_setting.size].reg_addr = 0x501a;
    g_reg_array[g_reg_setting.size].reg_data = R_gain & 0x00ff;
    g_reg_setting.size++;
  }
  if (G_gain > 0x400) {
    g_reg_array[g_reg_setting.size].reg_addr = 0x501b;
    g_reg_array[g_reg_setting.size].reg_data = G_gain >> 8;
    g_reg_setting.size++;
    g_reg_array[g_reg_setting.size].reg_addr = 0x501c;
    g_reg_array[g_reg_setting.size].reg_data = G_gain & 0x00ff;
    g_reg_setting.size++;
  }
  if (B_gain > 0x400) {
    g_reg_array[g_reg_setting.size].reg_addr = 0x501d;
    g_reg_array[g_reg_setting.size].reg_data = B_gain >> 8;
    g_reg_setting.size++;
    g_reg_array[g_reg_setting.size].reg_addr = 0x501e;
    g_reg_array[g_reg_setting.size].reg_data = B_gain & 0x00ff;
    g_reg_setting.size++;
  }
 }
}

/** truly_ov8856_update_lsc:
 *
 * Calculate and apply lens shading calibration data
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void truly_ov8856_update_lsc(sensor_eeprom_data_t *e_ctrl)
{
     int i,temp;

     if (otp_data.flag & 0x10) {
        temp = (uint8_t)(e_ctrl->eeprom_params.buffer[507]);
        temp = 0x20 | temp;
        g_reg_array[g_reg_setting.size].reg_addr = 0x5000;
        g_reg_array[g_reg_setting.size].reg_data = temp;
        g_reg_setting.size++;

        for(i = 0; i < LSC_REG_SIZE -1; i++)
        {
           g_reg_array[g_reg_setting.size].reg_addr = 0x5900 + i;
           g_reg_array[g_reg_setting.size].reg_data = otp_data.lenc[i];
           g_reg_setting.size++;
        }
     }
 }

/** truly_ov8856_get_group_index:
 *    @mid: group index register value
 *
 * Get which group is used
 *
 * This function executes in eeprom module context
 *
 * Return: int to indicate group index.
 **/
static int truly_ov8856_get_group_index(uint8_t mid)
{
  int8_t group_index = -1 ;
  mid = mid & 0xF0;
  if((mid&0xC0) == 0x40){
    group_index = 0 ;
  }else if((mid&0x30) == 0x10){
    group_index = 1 ;
  }else{
    group_index = -1 ;
  }
  SLOW("%s:group_index:%d",__func__,group_index);
  return group_index ;
}

/** truly_ov8856_read_info:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Read the data structure of product information like product date
 *
 * This function executes in eeprom module context
 *
 * Return: int to indicate read information success or not.
 **/
static int truly_ov8856_read_info(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t mid,temp,reg_val;
  int i = 0, group_index,addr_offset = 0,group_offset=8;
  int temp1;

  SLOW("Enter");

  mid = (uint8_t)(e_ctrl->eeprom_params.buffer[0]);
  group_index=truly_ov8856_get_group_index(mid);
  if(group_index==-1){
    SERR("%s:invalid or empty opt data",__func__) ;
    return -1 ;
  }
  addr_offset = group_offset*group_index ;

  otp_data.flag = 0xC0; // valid info and AWB in OTP
  otp_data.module_integrator_id =
    (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 1]);
  otp_data.lens_id =
    (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 2]);
  otp_data.production_year =
    (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 3]);
  otp_data.production_month =
    (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 4]);
  otp_data.production_day =
    (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 5]);

  SLOW("module id = %d lens id =%d year = %d month = %d day = %d",
   otp_data.module_integrator_id,otp_data.lens_id,otp_data.production_year,
   otp_data.production_month,otp_data.production_day);

  return 0;
}

/** truly_ov8856_read_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Read the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: int to indicate read white balance success or not.
 **/
static int truly_ov8856_read_wbdata(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t mid ;
  uint8_t temp ;
  uint8_t reg_val  ;
  int addr_offset = -1 ;
  int group_index  = -1 ;
  int group_offset = 8 ;
  SLOW("Enter");
  mid = (uint8_t)(e_ctrl->eeprom_params.buffer[0]);

  group_index=truly_ov8856_get_group_index(mid);
  if(group_index==-1){
    SERR("%s:invalid or empty awb data",__func__) ;
    return -1 ;
  }

  addr_offset =WB_OFFSET+group_index*group_offset ;
  temp = (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 3]);
  SLOW("temp=0x%x",temp);

  reg_val = (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 1]);
  SLOW("reg_val=0x%x",reg_val);
  otp_data.rg_ratio = (reg_val<<2) + ((temp>>6)&0x03) ;

  reg_val = (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 2]);
  SLOW("reg_val=0x%x",reg_val);
  otp_data.bg_ratio = (reg_val<<2) + ((temp>>4)&0x03) ;

  return 0;
}

/** truly_ov8856_read_lscdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Read the data structure of lens shading calibration
 *
 * This function executes in eeprom module context
 *
 * Return: int to indicate read lens shading success or not.
 **/
static int truly_ov8856_read_lscdata(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t mid ;
  int addr_offset = -1 ;
  int group_index  = -1 ;
  int group_offset = 241 ;
  int i;
  int temp1;
  int checksum2=0;

  SLOW("Enter");
  mid = (uint8_t)(e_ctrl->eeprom_params.buffer[LSC_FLAG_ADDR - BASE_ADDR]);

  group_index=truly_ov8856_get_group_index(mid);
  if(group_index==-1){
    SERR("%s:invalid or empty awb data",__func__) ;
    return -1 ;
  }

   addr_offset =LSC_OFFSET+group_index*group_offset ;

   for(i = 0; i < LSC_REG_SIZE-1; i++){
  otp_data.lenc[i] = (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 1 + i]);
  checksum2 += otp_data.lenc[i];
  SLOW("otp_data.lenc[%d]=0x%x",i,otp_data.lenc[i]);
   }
   checksum2 = (checksum2)%255 +1;
   otp_data.checksum = (uint8_t)(e_ctrl->eeprom_params.buffer[addr_offset + 1 + 240]);
   if(otp_data.checksum == checksum2){
        otp_data.flag |= 0x10;
   }
   return 0;
}

/** truly_ov8856_format_wb_lsc_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance and lens shading calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void truly_ov8856_format_wb_lsc_data(sensor_eeprom_data_t *e_ctrl)
{
  SLOW("Enter");
  int rc = 0;

  rc = truly_ov8856_read_wbdata(e_ctrl);
  if(rc < 0){
    SERR("read wbdata failed");
    return ;
  }
  rc = truly_ov8856_read_lscdata(e_ctrl);
  if(rc < 0){
    SERR("read lscdata failed");
    return ;
  }
  truly_ov8856_update_awb();
  truly_ov8856_update_lsc(e_ctrl);

  SLOW("Exit");
}

/** truly_ov8856_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
void truly_ov8856_format_calibration_data(void *e_ctrl) {
  SLOW("Enter");
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  uint8_t *data = ectrl->eeprom_params.buffer;

  g_reg_setting.addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  g_reg_setting.data_type = MSM_CAMERA_I2C_BYTE_DATA;
  g_reg_setting.reg_setting = &g_reg_array[0];
  g_reg_setting.size = 0;
  g_reg_setting.delay = 0;
  truly_ov8856_read_info(e_ctrl) ;
  truly_ov8856_format_wb_lsc_data(ectrl);
  SLOW("Exit");
}

/** truly_ov8856_get_raw_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *    @data: point to the destination msm_camera_i2c_reg_setting
 *
 * Get the all the calibration registers setting of the sensor
 *
 * This function executes in eeprom module context.
 *
 * Return: void.
 **/
int truly_ov8856_get_raw_data(void *e_ctrl, void *data) {
  if (e_ctrl && data)
    memcpy(data, &g_reg_setting, sizeof(g_reg_setting));
  else
    SERR("failed Null pointer");
  return 0;
}

/** truly_ov8856_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* truly_ov8856_eeprom_open_lib(void) {
  return &truly_ov8856_lib_func_ptr;
}
