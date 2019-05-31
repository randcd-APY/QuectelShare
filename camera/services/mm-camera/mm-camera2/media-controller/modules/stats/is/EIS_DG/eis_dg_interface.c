/* eis_dg_interface.cpp
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "stats_debug.h"
#include "eis_dg_interface.h"
#include "eis_dg/mvDGTC.h"
#include <dlfcn.h>
//#define PERP_MOUNT

#define START_FRAME_DUMP 4
#ifdef PROPERTY_VALUE_MAX
#undef PROPERTY_VALUE_MAX
#endif
#define PROPERTY_VALUE_MAX 32
#define S2US 1000000L
#define S2MS 1000
#define IMU_MAX_SAMPLE_RATE 1600
#define FBIS_MARGIN 1.10
#define RSC_MARGIN 1.15
#define MAX_Q16 65536
#define DEFAULT_EXP 30000
typedef struct {
  mvDG* (*init_get_handle)();
  void (*deinit)(mvDG*);
  int32_t (*start_session)(mvDG*,mvDGConfiguration*);
  int32_t (*stop_session)(mvDG*, int32_t*);
  void (*add_gyro_data)(mvDG*, int32_t, mvGyro*);
  void (*add_frame_times)(mvDG*, mvDGCameraFrameTimeInfo*);
  int32_t (*get_tranf_mat_number)(mvDG*, int32_t, TransfMatrixType, int32_t*,int32_t*);
  int32_t (*get_frame_transf_mat)(mvDG*, int32_t, TransfMatrixType, mvDGTransfMatrixArrayInfo*);
  int32_t (*mvDG_Adapt_window)(mvDG*, mvDGWindowInfo*, mvDGWindowInfo*, mvDGWindowInfo*);
} dgtc_funct_table;


typedef struct {
  mvDG* eis_dg;
  void* mvDG_lib_handle;
  dgtc_funct_table funct_pointers;
  mvDGConfiguration eis_dg_config;
  int32_t nTransfArrayW;
  int32_t nTransfArrayH;
  TransfMatrixType eis_dg_type;
  int32_t frame_id;
  mvGyro gyro_input[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
  mvDGWindowInfo sensor_win;
  mvDGWindowInfo camif_win;
  mvDGWindowInfo vfe_win;
  int mat_dump;
} eis_dg_interface_handle;

int eis_dg_get_rotation_matrix(float64_t rotation[][3],
  unsigned int camera_mount_angle, enum camb_position_t camera_position) {

  int err = 0, i, j;
  float temp;
  float rot_mat[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
  switch (camera_position) {
    case BACK_CAMERA_B:
      switch (camera_mount_angle) {
        case 270:
          rot_mat[0][0] = 1;
          rot_mat[1][1] = 1;
          rot_mat[2][2] = 1;
          break;
        case 180:
          rot_mat[0][1] = -1;
          rot_mat[1][0] = 1;
          rot_mat[2][2] = 1;
          break;
        case 90:
            rot_mat[0][0] = -1;
            rot_mat[1][1] = -1;
            rot_mat[2][2] = 1;
          break;
        case 0:
            rot_mat[0][1] = 1;
            rot_mat[1][0] = -1;
            rot_mat[2][2] = 1;
          break;
        }
      break;
    case FRONT_CAMERA_B:
      switch (camera_mount_angle) {
        case 270:
            rot_mat[0][0] = 1;
            rot_mat[1][1] = -1;
            rot_mat[2][2] = -1;
          break;
        case 180:
            rot_mat[0][1] = 1;
            rot_mat[1][0] = 1;
            rot_mat[2][2] = -1;
          break;
        case 90:
            rot_mat[0][0] = -1;
            rot_mat[1][1] = 1;
            rot_mat[2][2] = -1;
          break;
        case 0:
            rot_mat[0][1] = -1;
            rot_mat[1][0] = -1;
            rot_mat[2][2] = -1;
          break;
      }
      break;
    case AUX_CAMERA_B:
        switch (camera_mount_angle) {
          case 270:
              rot_mat[0][0] = 1;
              rot_mat[1][1] = 1;
              rot_mat[2][2] = 1;
            break;
          case 180:
              rot_mat[0][1] = -1;
              rot_mat[1][0] = 1;
              rot_mat[2][2] = 1;
            break;
          case 90:
              rot_mat[0][0] = -1;
              rot_mat[1][1] = -1;
              rot_mat[2][2] = 1;
            break;
          case 0:
              rot_mat[0][1] = 1;
              rot_mat[1][0] = -1;
              rot_mat[2][2] = 1;
            break;
            }
      break;
    default:
      err = -1;
      return err;
  }

//if camera is perpendicularly mounted as on IoT device.
#ifdef PERP_MOUNT
  switch (camera_position) {
    case BACK_CAMERA_B: {
      //rotate by 90deg around X axis in clock wise.
      //negate Z , swap Y,Z
      for(i =0;i<3;i++){
        temp = rot_mat[2][i];
        rot_mat[2][i] = rot_mat[1][i];
        rot_mat[1][i] = -temp;
      }
    }
    break;
    case FRONT_CAMERA_B: {
      //rotate by 90deg around X axis in anti clock wise.
      //swap Y,Z
      for(i =0;i<3;i++){
        temp = rot_mat[2][i];
        rot_mat[2][i] = rot_mat[1][i];
        rot_mat[1][i] = temp;
      }
    }
    break;
  }
#endif

  for (i=0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      rotation[i][j] = rot_mat[i][j];
    }
  }

  return err;
}


int eis_dg_fill_func_table(eis_dg_interface_handle *eis_dg_interface){
  if(eis_dg_interface == NULL){
    IS_ERR("invalid inut");
  return 0;
  }
  void *mvdg_lib = eis_dg_interface->mvDG_lib_handle;
  dlerror();
  eis_dg_interface->funct_pointers.init_get_handle =
    (mvDG*(*)())dlsym(mvdg_lib, "mvDG_Initialize");
  if(eis_dg_interface->funct_pointers.init_get_handle == NULL){
    IS_ERR("init_get_handle fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.deinit =
    (void (*)(mvDG*))dlsym(mvdg_lib, "mvDG_Deinitialize");
  if(eis_dg_interface->funct_pointers.deinit == NULL){
    IS_ERR("deinit fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.start_session =
    (int32_t (*)(mvDG*,mvDGConfiguration*))dlsym(mvdg_lib, "mvDG_StartSession");
  if(eis_dg_interface->funct_pointers.start_session == NULL){
    IS_ERR("start_session fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.stop_session =
    (int32_t (*)(mvDG*, int32_t*))dlsym(mvdg_lib, "mvDG_StopSession");
  if(eis_dg_interface->funct_pointers.stop_session == NULL){
    IS_ERR("stop_session fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.add_gyro_data =
    ( void (*)(mvDG*, int32_t, mvGyro*))dlsym(mvdg_lib, "mvDG_AddMultiGyro");
  if(eis_dg_interface->funct_pointers.add_gyro_data == NULL){
    IS_ERR("add_gyro_data fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.add_frame_times =
    (void (*)(mvDG*, mvDGCameraFrameTimeInfo*))dlsym(mvdg_lib, "mvDG_AddFrameTimeInfo");
  if(eis_dg_interface->funct_pointers.add_frame_times == NULL){
    IS_ERR("add_frame_times fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.get_tranf_mat_number =
    (int32_t (*)(mvDG*, int32_t, TransfMatrixType, int32_t*,int32_t*))dlsym(mvdg_lib, "mvDG_GetFrameTransfMatrixNumber");
  if(eis_dg_interface->funct_pointers.get_tranf_mat_number == NULL){
    IS_ERR("get_tranf_mat_number fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.get_frame_transf_mat =
    (int32_t (*)(mvDG*, int32_t, TransfMatrixType, mvDGTransfMatrixArrayInfo*))dlsym(mvdg_lib, "mvDG_GetFrameTransfMatrix");
  if(eis_dg_interface->funct_pointers.get_frame_transf_mat == NULL){
    IS_ERR("get_frame_transf_mat fail %s", dlerror());
    return 0;
  }

  eis_dg_interface->funct_pointers.mvDG_Adapt_window =
      (int32_t (*)(mvDG*, mvDGWindowInfo*, mvDGWindowInfo*, mvDGWindowInfo*))dlsym(mvdg_lib, "mvDG_Adapt_window");
  if(eis_dg_interface->funct_pointers.mvDG_Adapt_window == NULL) {
    IS_ERR("mvDG_adapt_window fail %s", dlerror());
    return 0;
  }

  return 1;
}



void eis_dg_get_camera_config(mvDGConfiguration *eis_dg_config, is_init_data_t *init_data) {

  int i,j;
  dewarp_tuning_t *dewarp_tune = (dewarp_tuning_t *)init_data->dewarp_tuning;
  eis_dg_config->FrameRateInHz = init_data->frame_cfg.frame_fps;
  //get rotation matrix relative to gyro axis
  eis_dg_get_rotation_matrix(eis_dg_config->rotation,
    init_data->sensor_mount_angle,
    init_data->camera_position);
  IS_LOW(" rotation matrix: {%f %f %f} {%f %f %f} {%f %f %f}, pos: %d mount angle: %d",
    eis_dg_config->rotation[0][0], eis_dg_config->rotation[0][1],eis_dg_config->rotation[0][2],
    eis_dg_config->rotation[1][0],eis_dg_config->rotation[1][1],eis_dg_config->rotation[1][2],
    eis_dg_config->rotation[2][0],eis_dg_config->rotation[2][1],eis_dg_config->rotation[2][2],
    init_data->camera_position,init_data->sensor_mount_angle);

    //Input Image Dimensions
    eis_dg_config->camera.pixelWidth = init_data->frame_cfg.vfe_output_width;
    eis_dg_config->camera.pixelHeight = init_data->frame_cfg.vfe_output_height;


    //Output Image Dimensions
    eis_dg_config->pxlWidth = init_data->frame_cfg.dis_frame_width;
    eis_dg_config->pxlHeight = init_data->frame_cfg.dis_frame_height;


    //Camera LDC calibration parameters: Need to read from eeprom data delivered by sensor.
    eis_dg_config->camera.principalPoint[0] =
      eis_dg_config->camera.pixelWidth/2;
    eis_dg_config->camera.principalPoint[1] =
      eis_dg_config->camera.pixelHeight/2;
    eis_dg_config->camera.focalLength[0] = 3100;
    eis_dg_config->camera.focalLength[1] = 3100;
    memset( eis_dg_config->camera.distortion, 0, sizeof( float64_t ) * 8 );
    eis_dg_config->camera.distortionModel = 0;

    //update tuning data.
    if(dewarp_tune != NULL) {
      eis_dg_config->ldc_r2_lut_size = dewarp_tune->ldc_tuning_data->ldc_1D_size;
      eis_dg_config->ldc_inv_r2_lut_size = dewarp_tune->ldc_tuning_data->ldc_inv_r2_lut_size;
      eis_dg_config->ldc_r2_lut = dewarp_tune->ldc_tuning_data->ldc_1D_LUT;
      eis_dg_config->ldc_inv_r2_lut = dewarp_tune->ldc_tuning_data->ldc_inv_r2_lut;
      eis_dg_config->ldc_mesh = (mvDG_mesh_t *)&dewarp_tune->ldc_tuning_data->ldc_mesh;
      IS_LOW("LDC 1D size :%f LDC inv 1d size :%f \n"
             "LDC 1D LUT: %f %f %f %f....%f %f %f %f \n"
             "Inv LDC 1D LUT: %f %f %f %f ...%f %f %f %f",
             eis_dg_config->ldc_r2_lut_size,
             eis_dg_config->ldc_inv_r2_lut_size,
             eis_dg_config->ldc_r2_lut[0],eis_dg_config->ldc_r2_lut[1],
             eis_dg_config->ldc_r2_lut[2],eis_dg_config->ldc_r2_lut[3],
             eis_dg_config->ldc_r2_lut[1020],eis_dg_config->ldc_r2_lut[1021],
             eis_dg_config->ldc_r2_lut[1022],eis_dg_config->ldc_r2_lut[1023],
             eis_dg_config->ldc_inv_r2_lut[0],eis_dg_config->ldc_inv_r2_lut[1],
             eis_dg_config->ldc_inv_r2_lut[2],eis_dg_config->ldc_inv_r2_lut[3],
             eis_dg_config->ldc_inv_r2_lut[1020],eis_dg_config->ldc_inv_r2_lut[1021],
             eis_dg_config->ldc_inv_r2_lut[1022],eis_dg_config->ldc_inv_r2_lut[1023]);
    } else {
      IS_ERR("Dewarp tuning null..1D LDC not configured");
    }

    //TODO:tuning or other dynamic config needs to be coming from upper layers.
    IS_PROP_ENABLE_EIS_DG_LOG(eis_dg_config->debug_print_en);
    eis_dg_config->fbis_margin = FBIS_MARGIN;
    eis_dg_config->rsc_margin = RSC_MARGIN;
    eis_dg_config->imu_data_sampling_rate = IMU_MAX_SAMPLE_RATE;
    //IMU calibration data
    eis_dg_config->imu_calib.gyro_unit = MAX_Q16;
    eis_dg_config->imu_calib.acc_unit = MAX_Q16;
    eis_dg_config->imu_calib.focal_length_in_mm = //2.6;
      init_data->sensor_out_info.focal_length;
    eis_dg_config->imu_calib.pixel_size_in_um = //3.24;
      init_data->sensor_out_info.pix_size;
    for (i=0; i < 3; i++) {
      for(j =0; j < 3; j++) {
        eis_dg_config->imu_calib.Rbc[i*3+j] = eis_dg_config->rotation[i][j];
      }
    }
}

int eis_dg_update_adapt_win(
  eis_dg_interface_handle *eis_dg_interface,
  is_vfe_window_t *vfe_win, long vfe_width, long vfe_height){
  int err = 0;
  //ISP win
  eis_dg_interface->vfe_win.input_width =
    eis_dg_interface->camif_win.output_width;
  eis_dg_interface->vfe_win.input_height =
    eis_dg_interface->camif_win.output_height;
  eis_dg_interface->vfe_win.output_width = vfe_width;
    eis_dg_interface->vfe_win.output_height = vfe_height;

  eis_dg_interface->vfe_win.start_x = vfe_win->vfe_start_x;
  eis_dg_interface->vfe_win.start_y = vfe_win->vfe_start_y;
  eis_dg_interface->vfe_win.end_x = vfe_win->scaler_output_w;
  eis_dg_interface->vfe_win.end_y = vfe_win->scaler_output_h;
  eis_dg_interface->vfe_win.binning_en = 0;
  eis_dg_interface->vfe_win.hblank = 0;
  eis_dg_interface->vfe_win.vblank = 0;
  IS_LOW("VFE_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->vfe_win.input_width,
    eis_dg_interface->vfe_win.input_height,
    eis_dg_interface->vfe_win.output_width,
    eis_dg_interface->vfe_win.output_height,
    eis_dg_interface->vfe_win.start_x,
    eis_dg_interface->vfe_win.start_y,
    eis_dg_interface->vfe_win.end_x,
    eis_dg_interface->vfe_win.end_y,
    eis_dg_interface->vfe_win.binning_en,
    eis_dg_interface->vfe_win.hblank,
    eis_dg_interface->vfe_win.vblank);
  IS_LOW("SEN_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->sensor_win.input_width,
    eis_dg_interface->sensor_win.input_height,
    eis_dg_interface->sensor_win.output_width,
    eis_dg_interface->sensor_win.output_height,
    eis_dg_interface->sensor_win.start_x,
    eis_dg_interface->sensor_win.start_y,
    eis_dg_interface->sensor_win.end_x,
    eis_dg_interface->sensor_win.end_y,
    eis_dg_interface->sensor_win.binning_en,
    eis_dg_interface->sensor_win.hblank,
    eis_dg_interface->sensor_win.vblank);
  IS_LOW("CAMIF_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->camif_win.input_width,
    eis_dg_interface->camif_win.input_height,
    eis_dg_interface->camif_win.output_width,
    eis_dg_interface->camif_win.output_height,
    eis_dg_interface->camif_win.start_x,
    eis_dg_interface->camif_win.start_y,
    eis_dg_interface->camif_win.end_x,
    eis_dg_interface->camif_win.end_y,
    eis_dg_interface->camif_win.binning_en,
    eis_dg_interface->camif_win.hblank,
    eis_dg_interface->camif_win.vblank);

  if(eis_dg_interface->funct_pointers.mvDG_Adapt_window) {
    err = eis_dg_interface->funct_pointers.mvDG_Adapt_window(
      eis_dg_interface->eis_dg,
      &eis_dg_interface->sensor_win,
      &eis_dg_interface->camif_win,
      &eis_dg_interface->vfe_win);
    IS_LOW("Adapt window for EIS success err %d",err);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : mvDG_Adapt_window()");
    free(eis_dg_interface);
    eis_dg_interface = NULL;
    return err;
  }
  return err;
}

void eis_dg_update_win_size(
  eis_dg_interface_handle *eis_dg_interface,
  is_init_data_t *init_data){

  //sensor win
  eis_dg_interface->sensor_win.input_width =
    init_data->sensor_out_info.full_width;
  eis_dg_interface->sensor_win.input_height =
    init_data->sensor_out_info.full_height;
  eis_dg_interface->sensor_win.output_width =
    init_data->sensor_out_info.sensor_width;
  eis_dg_interface->sensor_win.output_height =
    init_data->sensor_out_info.sensor_height;
  eis_dg_interface->sensor_win.start_x =
    init_data->sensor_out_info.offset_x;
  eis_dg_interface->sensor_win.start_y =
    init_data->sensor_out_info.offset_y;
  eis_dg_interface->sensor_win.end_x =
    eis_dg_interface->sensor_win.start_x +
    eis_dg_interface->sensor_win.output_width;
  eis_dg_interface->sensor_win.end_y =
    eis_dg_interface->sensor_win.start_y +
    eis_dg_interface->sensor_win.output_height;
  eis_dg_interface->sensor_win.binning_en =
    init_data->sensor_out_info.binning_factor;
  eis_dg_interface->sensor_win.hblank =
    init_data->sensor_out_info.ll_pck -
    eis_dg_interface->sensor_win.output_width;
  eis_dg_interface->sensor_win.vblank =
    init_data->sensor_out_info.fl_lines -
    eis_dg_interface->sensor_win.output_height;
  IS_LOW("SEN_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->sensor_win.input_width,
    eis_dg_interface->sensor_win.input_height,
    eis_dg_interface->sensor_win.output_width,
    eis_dg_interface->sensor_win.output_height,
    eis_dg_interface->sensor_win.start_x,
    eis_dg_interface->sensor_win.start_y,
    eis_dg_interface->sensor_win.end_x,
    eis_dg_interface->sensor_win.end_y,
    eis_dg_interface->sensor_win.binning_en,
    eis_dg_interface->sensor_win.hblank,
    eis_dg_interface->sensor_win.vblank);

  //camif win
  eis_dg_interface->camif_win.input_width =
    init_data->sensor_out_info.sensor_width;
  eis_dg_interface->camif_win.input_height =
    init_data->sensor_out_info.sensor_height;
  eis_dg_interface->camif_win.output_width =
    init_data->sensor_out_info.camif_width;
  eis_dg_interface->camif_win.output_height =
    init_data->sensor_out_info.camif_height;
  eis_dg_interface->camif_win.start_x =
    init_data->sensor_out_info.camif_crop_x;
  eis_dg_interface->camif_win.start_y =
    init_data->sensor_out_info.camif_crop_y;
  eis_dg_interface->camif_win.end_x =
    eis_dg_interface->camif_win.start_x +
    eis_dg_interface->camif_win.output_width;
  eis_dg_interface->camif_win.end_y =
    eis_dg_interface->camif_win.start_y +
    eis_dg_interface->camif_win.output_height;
  eis_dg_interface->camif_win.binning_en = 0;
  eis_dg_interface->camif_win.hblank = 0;
  eis_dg_interface->camif_win.vblank = 0;
  IS_LOW("CAMIF_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->camif_win.input_width,
    eis_dg_interface->camif_win.input_height,
    eis_dg_interface->camif_win.output_width,
    eis_dg_interface->camif_win.output_height,
    eis_dg_interface->camif_win.start_x,
    eis_dg_interface->camif_win.start_y,
    eis_dg_interface->camif_win.end_x,
    eis_dg_interface->camif_win.end_y,
    eis_dg_interface->camif_win.binning_en,
    eis_dg_interface->camif_win.hblank,
    eis_dg_interface->camif_win.vblank);

  //ISP win
  eis_dg_interface->vfe_win.input_width =
    eis_dg_interface->camif_win.output_width;
  eis_dg_interface->vfe_win.input_height =
    eis_dg_interface->camif_win.output_height;
  eis_dg_interface->vfe_win.output_width =
    init_data->frame_cfg.vfe_output_width;
  eis_dg_interface->vfe_win.output_height =
    init_data->frame_cfg.vfe_output_height;
  //TODO: Need to fill vfe x ,y based on crop For now think there is no crop but just scale
  eis_dg_interface->vfe_win.start_x = 0;
  eis_dg_interface->vfe_win.start_y = 0;
  eis_dg_interface->vfe_win.end_x =
    eis_dg_interface->vfe_win.start_x +
    eis_dg_interface->vfe_win.input_width;
  eis_dg_interface->vfe_win.end_y =
    eis_dg_interface->vfe_win.start_y +
    eis_dg_interface->vfe_win.input_height;
  eis_dg_interface->vfe_win.binning_en = 0;
  eis_dg_interface->vfe_win.hblank = 0;
  eis_dg_interface->vfe_win.vblank = 0;
  IS_LOW("VFE_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    eis_dg_interface->vfe_win.input_width,
    eis_dg_interface->vfe_win.input_height,
    eis_dg_interface->vfe_win.output_width,
    eis_dg_interface->vfe_win.output_height,
    eis_dg_interface->vfe_win.start_x,
    eis_dg_interface->vfe_win.start_y,
    eis_dg_interface->vfe_win.end_x,
    eis_dg_interface->vfe_win.end_y,
    eis_dg_interface->vfe_win.binning_en,
    eis_dg_interface->vfe_win.hblank,
    eis_dg_interface->vfe_win.vblank);
}



int eis_dg_initialize(void **eis_dg_handle, is_init_data_t *init_data) {

  /*Input validation*/
  *eis_dg_handle = NULL;
  if(init_data == NULL) {
    IS_ERR("Input data invalid!! EXIT!!");
    return -1;
  }
  int err = 0, rc = 0, x;

  void *mvdg_lib = NULL;
  dlerror();
  mvdg_lib = dlopen("/usr/lib/libmvDGTC.so", RTLD_LAZY);
  if (mvdg_lib == NULL) {
    err = -1;
    IS_ERR("Library Open Fail %s", dlerror());
    return err;
  } else {
    IS_LOW("Library Open Success");
  }

  //allocate memory
  eis_dg_interface_handle *eis_dg_interface =
    (eis_dg_interface_handle *)malloc(sizeof(eis_dg_interface_handle));
  if(eis_dg_interface == NULL) {
    IS_ERR("Memory allocation error!!");
    err = -1;
    return err;
  }

  eis_dg_interface->mvDG_lib_handle = mvdg_lib;

  mvDGConfiguration *eis_dg_config = &eis_dg_interface->eis_dg_config;

  //get the camera config
  eis_dg_get_camera_config(eis_dg_config, init_data);

  // FIll up window sizes
  eis_dg_update_win_size(eis_dg_interface, init_data);

  if(!eis_dg_fill_func_table(eis_dg_interface)){
    IS_ERR("Failed during func ptr!!" );
    err = -1;
    free(eis_dg_interface);
    eis_dg_interface = NULL;
    return err;
  }

  //Get handle
  if(eis_dg_interface->funct_pointers.init_get_handle) {
    eis_dg_interface->eis_dg = eis_dg_interface->funct_pointers.init_get_handle();
  } else {
    err = -1;
    IS_ERR("Not a valid function ptr : init_get_handle()");
    return err;
  }
  //Dump camera config
  IS_HIGH("DUMP: camera config");
  IS_HIGH("ipW:%d iP:%d opW:%d opH:%d framerate:%d PP:[%f %f] FL:[%f %f]"
    "dist coef:[%f %f %f %f %f %f %f %f] dist model:%d",
    eis_dg_config->camera.pixelWidth,
    eis_dg_config->camera.pixelHeight,
    eis_dg_config->pxlWidth,
    eis_dg_config->pxlHeight,
    eis_dg_config->FrameRateInHz,
    eis_dg_config->camera.principalPoint[0],
    eis_dg_config->camera.principalPoint[1],
    eis_dg_config->camera.focalLength[0],
    eis_dg_config->camera.focalLength[1],
    eis_dg_config->camera.distortion[0],
    eis_dg_config->camera.distortion[1],
    eis_dg_config->camera.distortion[2],
    eis_dg_config->camera.distortion[3],
    eis_dg_config->camera.distortion[4],
    eis_dg_config->camera.distortion[5],
    eis_dg_config->camera.distortion[6],
    eis_dg_config->camera.distortion[7],
    eis_dg_config->camera.distortionModel);

  //start session
  if(eis_dg_interface->funct_pointers.start_session) {
    rc = eis_dg_interface->funct_pointers.start_session(eis_dg_interface->eis_dg,eis_dg_config);
  } else {
    err = -1;
    IS_ERR("No valid function ptr : start_session");
    free(eis_dg_interface);
    eis_dg_interface = NULL;
    return err;
  }

  //get EIS DG transform type
  eis_dg_interface->eis_dg_type = (TransfMatrixType)(IS_PROP_GET_EIS_DG_TYPE(x));
  eis_dg_interface->frame_id = 1;

  //read the dewarp size from lib API
  if(eis_dg_interface->funct_pointers.get_tranf_mat_number) {
    err = eis_dg_interface->funct_pointers.get_tranf_mat_number(eis_dg_interface->eis_dg, eis_dg_interface->frame_id,
      eis_dg_interface->eis_dg_type, &eis_dg_interface->nTransfArrayW, &eis_dg_interface->nTransfArrayH);
   IS_LOW(" Type: %d Trans num X: %d Y: %d",
   eis_dg_interface->eis_dg_type,
   eis_dg_interface->nTransfArrayW,
   eis_dg_interface->nTransfArrayH);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : get_tranf_mat_number()");
    free(eis_dg_interface);
    eis_dg_interface = NULL;
    return err;
  }

  //Adapt window init.
  if(eis_dg_interface->funct_pointers.mvDG_Adapt_window) {
    err = eis_dg_interface->funct_pointers.mvDG_Adapt_window(
        eis_dg_interface->eis_dg,
        &eis_dg_interface->sensor_win,
        &eis_dg_interface->camif_win,
        &eis_dg_interface->vfe_win);
    IS_LOW("Adapt window for EIS success");
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : mvDG_Adapt_window()");
    free(eis_dg_interface);
    eis_dg_interface = NULL;
    return err;
  }

  IS_PROP_GET_MAT_DUMP(eis_dg_interface->mat_dump);
  //return handle
  *eis_dg_handle = (void *)eis_dg_interface;

  return rc;
}

int eis_dg_get_mesh_size(void *eis_dg_handle, uint32_t *mesh_x, uint32_t *mesh_y) {
  //input validation
  if(!(eis_dg_handle && mesh_x && mesh_y)) {
    IS_ERR("Invalid input: %p %p %p", eis_dg_handle, mesh_x,mesh_y);
    return -1;
  }

  eis_dg_interface_handle *eis_dg_interface = (eis_dg_interface_handle *)eis_dg_handle;
  //mesh size is one less than vertices
  *mesh_x = eis_dg_interface->nTransfArrayW - 1;
  *mesh_y = eis_dg_interface->nTransfArrayH - 1;
  return 0;
}


int eis_dg_process(void *eis_dg_handle, eis_dg_input_t *eis_dg_input,
  is_output_type *is_output) {

  //input validation
  if(!(eis_dg_handle && eis_dg_input && is_output)) {
    IS_ERR("Invalid input: %p %p %p", eis_dg_handle, eis_dg_input,is_output);
    return -1;
  }

  FILE* file;
  char filename[128];

  uint32_t num_gyro_samples, i, j, k;
  int32_t frame_id;
  mvDGTransfMatrixArrayInfo rTransfArray;
  int err = 0;
  eis_dg_interface_handle *eis_dg_interface = (eis_dg_interface_handle *)eis_dg_handle;
  mvGyro *Gptr = eis_dg_interface->gyro_input;
  mct_event_gyro_data_t *gyro_data = eis_dg_input->gyro_data;

  mvDGCameraFrameTimeInfo eis_dg_frame_times;

  for(i =0;i < gyro_data->sample_len; i++) {
    Gptr[i].time_usec = gyro_data->sample[i].timestamp;
    Gptr[i].gyro_rad_sec[0] = (float32_t)(gyro_data->sample[i].value[0]);
    Gptr[i].gyro_rad_sec[1] = (float32_t)(gyro_data->sample[i].value[1]);
    Gptr[i].gyro_rad_sec[2] = (float32_t)(gyro_data->sample[i].value[2]);
    IS_LOW("gyro sample[%d], %llu, %f, %f, %f", i,
      Gptr[i].time_usec,
      Gptr[i].gyro_rad_sec[0],
      Gptr[i].gyro_rad_sec[1],
      Gptr[i].gyro_rad_sec[2]);
  }
  num_gyro_samples = gyro_data->sample_len;


  /*Pass the gyro data to the mvDGTC lib*/
  if((void*)(eis_dg_interface->funct_pointers.add_gyro_data)) {
    eis_dg_interface->funct_pointers.add_gyro_data(eis_dg_interface->eis_dg,
      num_gyro_samples, Gptr);
    IS_LOW("Add gyro data successfully");
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : add_gyro_data");
    return err;
  }

  /*update image frame timing info*/
  eis_dg_frame_times.time_usec = eis_dg_input->frame_times.sof;
  eis_dg_frame_times.durationTime_usec = eis_dg_input->frame_times.frame_time;
  eis_dg_frame_times.exposureTime_usec = (int)(eis_dg_input->frame_times.exposure_time*S2US);
  if(eis_dg_frame_times.exposureTime_usec == 0) {
    eis_dg_frame_times.exposureTime_usec = DEFAULT_EXP;
  }
  eis_dg_frame_times.lineReadoutTime_usec = eis_dg_input->frame_times.linereadout_time/S2MS;
  frame_id = is_output->frame_id;
  eis_dg_frame_times.frameid = frame_id; //this frame is gyro arrival fid!!!

  IS_LOW("Frame id:%d sof :%lld exp_time: %lld dur_time: %lld l_rd_time:%f",
    eis_dg_frame_times.frameid,
    eis_dg_frame_times.time_usec,
    eis_dg_frame_times.exposureTime_usec,
    eis_dg_frame_times.durationTime_usec,
    eis_dg_frame_times.lineReadoutTime_usec);


  /*Pass the frame timing info and frame id to mvDGTC lib*/
 if(eis_dg_interface->funct_pointers.add_frame_times) {
    eis_dg_interface->funct_pointers.add_frame_times(eis_dg_interface->eis_dg,
      &eis_dg_frame_times);
  } else {
    err = -1;
    IS_ERR("Invalid Function pointer : add_frame_times()");
    return err;
  }

  rTransfArray.frameid = frame_id;
  rTransfArray.transfMatrixType = eis_dg_interface->eis_dg_type;
  rTransfArray.gridW = eis_dg_interface->nTransfArrayW;
  rTransfArray.gridH = eis_dg_interface->nTransfArrayH;
  rTransfArray.transfArray = (mvPose3DR *)is_output->transform_matrix;

  if(eis_dg_interface->funct_pointers.get_frame_transf_mat) {
   if(num_gyro_samples > 0) {
     err = eis_dg_interface->funct_pointers.get_frame_transf_mat(eis_dg_interface->eis_dg,
       frame_id, eis_dg_interface->eis_dg_type, &rTransfArray);
   }
      IS_LOW("gyro_samples : %d got warp map correction for frame :%d",
        num_gyro_samples,rTransfArray.frameid);
    for (i = 0; i < 10; i++) {
      IS_LOW("tt = %u, tm = %f %f %f %f %f %f %f %f %f", eis_dg_interface->eis_dg_type,
        rTransfArray.transfArray[i].matrix[0][0],
        rTransfArray.transfArray[i].matrix[0][1],
        rTransfArray.transfArray[i].matrix[0][2],
        rTransfArray.transfArray[i].matrix[1][0],
        rTransfArray.transfArray[i].matrix[1][1],
        rTransfArray.transfArray[i].matrix[1][2],
        rTransfArray.transfArray[i].matrix[2][0],
        rTransfArray.transfArray[i].matrix[2][1],
        rTransfArray.transfArray[i].matrix[2][2]);
    }
  //dump matrices based on setprop
    if(eis_dg_interface->mat_dump > 0) {
      if(frame_id >= START_FRAME_DUMP &&
         frame_id < START_FRAME_DUMP + eis_dg_interface->mat_dump){
        snprintf(filename, sizeof(filename), "/data/misc/camera/filename_%d.txt",frame_id);
        file = fopen(filename,"w");
        if(file == NULL) {
          IS_ERR("Failed to open file %s",filename);
          return -1;
        }
        perror("EIS_DG");
        for(i =0 ; i < rTransfArray.gridH ; i++){
          for(j =0; j< rTransfArray.gridW; j++){
            for(k =0; k<9; k++){
              fprintf(file,"%f ", rTransfArray.transfArray[i*rTransfArray.gridW +j].matrix[k/3][k%3]);
            }
            fprintf(file, "\n");
          }
          fprintf(file, "\n\n\n");
        }
      }
    }
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : get_frame_transf_mat()");
    return err;
  }
  if(err == 0) {
    /*Processing Successful. Update is_output struct.
        Transf matrix dim are according to EIS3.0 currently*/
    is_output->has_output = TRUE;
   return 1;
  } else {
    return 0;
  }
  return err;
}


int32_t eis_dg_deinitialize(void *eis_dg_handle, int32_t *frame_id) {
 /*input validation*/
 if(!eis_dg_handle) {
  IS_ERR("invalid input %p", eis_dg_handle);
  return -1;
 }
  eis_dg_interface_handle *eis_dg_interface = (eis_dg_interface_handle *)eis_dg_handle;
  int32_t err;

  if(eis_dg_interface->funct_pointers.stop_session) {
    err = eis_dg_interface->funct_pointers.stop_session(eis_dg_interface->eis_dg, frame_id);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : stop_session()");
  }

  if(err != -1 && eis_dg_interface->funct_pointers.deinit) {
    eis_dg_interface->funct_pointers.deinit(eis_dg_interface->eis_dg);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : deinit()");
  }

  if(eis_dg_handle) {
    free(eis_dg_handle);
  }

  return err;
}
