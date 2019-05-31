/* digital_gimbal_interface.cpp
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "stats_debug.h"
#include "digital_gimbal_interface.h"
#include "DG/API/mvDGTC.h"
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
  mvDGTC* (*init_get_handle)();
  void (*deinit)(mvDGTC*);
  int32_t (*start_session)(mvDGTC*,mvDGConfiguration*);
  int32_t (*stop_session)(mvDGTC*, int32_t*);
  void (*add_imu_data)(mvDGTC*, uint32_t, mvAttitude*);
  void (*add_frame_times)(mvDGTC*, mvDGCameraFrameTimeInfo*);
  int32_t (*get_tranf_mat_number)(mvDGTC*, int32_t, TransfMatrixType, int32_t*,int32_t*);
  int32_t (*get_frame_transf_mat)(mvDGTC*, int32_t, TransfMatrixType, mvDGTransfMatrixArrayInfo*);
  /*int32_t (*mvDG_Adapt_window)(mvDGTC*, mvDGWindowInfo*, mvDGWindowInfo*, mvDGWindowInfo*);*/
} dgtc_funct_table;


typedef struct {
  mvDGTC* dgHandle;
  void* mvDG_lib_handle;
  dgtc_funct_table funct_pointers;
  mvDGConfiguration dg_config;
  int32_t nTransfArrayW;
  int32_t nTransfArrayH;
  TransfMatrixType dg_type;
  int32_t frame_id;
  mvAttitude imu_input[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
  /*mvDGWindowInfo sensor_win;
  mvDGWindowInfo camif_win;
  mvDGWindowInfo vfe_win;*/
  int mat_dump;
} dg_interface_handle;

int dg_get_rotation_matrix(float64_t rotation[][3],
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


int dg_fill_func_table(dg_interface_handle *dg_interface){
  if(dg_interface == NULL){
    IS_ERR("invalid inut");
  return 0;
  }
  void *mvdg_lib = dg_interface->mvDG_lib_handle;
  dlerror();
  dg_interface->funct_pointers.init_get_handle =
    (mvDGTC*(*)())dlsym(mvdg_lib, "mvDGTC_Initialize");
  if(dg_interface->funct_pointers.init_get_handle == NULL){
    IS_ERR("init_get_handle fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.deinit =
    (void (*)(mvDGTC*))dlsym(mvdg_lib, "mvDGTC_Deinitialize");
  if(dg_interface->funct_pointers.deinit == NULL){
    IS_ERR("deinit fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.start_session =
    (int32_t (*)(mvDGTC*,mvDGConfiguration*))dlsym(mvdg_lib, "mvDGTC_StartSession");
  if(dg_interface->funct_pointers.start_session == NULL){
    IS_ERR("start_session fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.stop_session =
    (int32_t (*)(mvDGTC*, int32_t*))dlsym(mvdg_lib, "mvDGTC_StopSession");
  if(dg_interface->funct_pointers.stop_session == NULL){
    IS_ERR("stop_session fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.add_imu_data =
    ( void (*)(mvDGTC*, int32_t, mvGyro*))dlsym(mvdg_lib, "mvDGTC_AddAttitudes");
  if(dg_interface->funct_pointers.add_imu_data == NULL){
    IS_ERR("add_imu_data fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.add_frame_times =
    (void (*)(mvDGTC*, mvDGCameraFrameTimeInfo*))dlsym(mvdg_lib, "mvDGTC_AddFrameTimeInfo");
  if(dg_interface->funct_pointers.add_frame_times == NULL){
    IS_ERR("add_frame_times fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.get_tranf_mat_number =
    (int32_t (*)(mvDGTC*, int32_t, TransfMatrixType, int32_t*,int32_t*))dlsym(mvdg_lib, "mvDGTC_GetFrameTransfMatrixNumber");
  if(dg_interface->funct_pointers.get_tranf_mat_number == NULL){
    IS_ERR("get_tranf_mat_number fail %s", dlerror());
    return 0;
  }

  dg_interface->funct_pointers.get_frame_transf_mat =
    (int32_t (*)(mvDGTC*, int32_t, TransfMatrixType, mvDGTransfMatrixArrayInfo*))dlsym(mvdg_lib, "mvDGTC_GetFrameTransfMatrix");
  if(dg_interface->funct_pointers.get_frame_transf_mat == NULL){
    IS_ERR("get_frame_transf_mat fail %s", dlerror());
    return 0;
  }

 /* dg_interface->funct_pointers.mvDG_Adapt_window =
      (int32_t (*)(mvDG*, mvDGWindowInfo*, mvDGWindowInfo*, mvDGWindowInfo*))dlsym(mvdg_lib, "mvDG_Adapt_window");
  if(dg_interface->funct_pointers.mvDG_Adapt_window == NULL) {
    IS_ERR("mvDG_adapt_window fail %s", dlerror());
    return 0;
  }*/

  return 1;
}



void dg_get_camera_config(mvDGConfiguration *dg_config, is_init_data_t *init_data) {

  int i,j;
  dewarp_tuning_t *dewarp_tune = (dewarp_tuning_t *)init_data->dewarp_tuning;

  IS_INFO("pos: %d mount angle: %d",
    init_data->camera_position,init_data->sensor_mount_angle);

  //get rotation matrix relative to gyro axis
  dg_get_rotation_matrix(dg_config->rotationin,
    init_data->sensor_mount_angle,
    init_data->camera_position);

  //Input Image Dimensions
  dg_config->camera.pixelWidth = init_data->frame_cfg.vfe_output_width;
  dg_config->camera.pixelHeight = init_data->frame_cfg.vfe_output_height;
  dg_config->camera.memoryStride = init_data->frame_cfg.vfe_output_stride;
  dg_config->camera.uvOffset = init_data->frame_cfg.vfe_output_scanline - init_data->frame_cfg.vfe_output_height;

  //Camera LDC calibration parameters: Need to read from eeprom data delivered by sensor.
  dg_config->camera.principalPoint[0] = dg_config->camera.pixelWidth/2;
  dg_config->camera.principalPoint[1] = dg_config->camera.pixelHeight/2;
  dg_config->camera.focalLength[0] = dg_config->camera.focalLength[1] =
      (dg_config->camera.pixelWidth + dg_config->camera.pixelHeight) / 2;
  memset(dg_config->camera.distortion, 0, sizeof( float64_t ) * 8 );
  dg_config->camera.distortionModel = 0;

  //Output Image Dimensions
  dg_config->pxlWidth = init_data->frame_cfg.dis_frame_width;
  dg_config->pxlHeight = init_data->frame_cfg.dis_frame_height;
  dg_config->rowMemoryStride = init_data->frame_cfg.dis_frame_stride;
  dg_config->uvOffset = init_data->frame_cfg.dis_frame_scanline - init_data->frame_cfg.dis_frame_height;

  dg_config->FrameRateInHz = init_data->frame_cfg.frame_fps;

  {
    char value[PROPERTY_VALUE_MAX];
    property_get("persist.camera.dg.p2l", value, "0");
    bool dg_p2l_enabled = (bool) atoi(value);

    if (dg_p2l_enabled) {
      IS_INFO("P2L enabled.");
      dg_config->rotationin[0][0] = 0.0;
      dg_config->rotationin[0][1] = 0.0;
      dg_config->rotationin[0][2] = 1.0;
      dg_config->rotationin[1][0] = -1.0;
      dg_config->rotationin[1][1] = 0.0;
      dg_config->rotationin[1][2] = 0.0;
      dg_config->rotationin[2][0] = 0.0;
      dg_config->rotationin[2][1] = -1.0;
      dg_config->rotationin[2][2] = 0.0;

      dg_config->rotationout[0][0] = 0.0;
      dg_config->rotationout[0][1] = 0.0;
      dg_config->rotationout[0][2] = 1.0;
      dg_config->rotationout[1][0] = 0.0;
      dg_config->rotationout[1][1] = -1.0;
      dg_config->rotationout[1][2] = 0.0;
      dg_config->rotationout[2][0] = 1.0;
      dg_config->rotationout[2][1] = 0.0;
      dg_config->rotationout[2][2] = 0.0;
    }else {
      IS_INFO("P2L disabled.");
      dg_config->rotationin[0][0] = 1.0;
      dg_config->rotationin[0][1] = 0.0;
      dg_config->rotationin[0][2] = 0.0;
      dg_config->rotationin[1][0] = 0.0;
      dg_config->rotationin[1][1] = 1.0;
      dg_config->rotationin[1][2] = 0.0;
      dg_config->rotationin[2][0] = 0.0;
      dg_config->rotationin[2][1] = 0.0;
      dg_config->rotationin[2][2] = 1.0;

      dg_config->rotationout[0][0] = 1.0;
      dg_config->rotationout[0][1] = 0.0;
      dg_config->rotationout[0][2] = 0.0;
      dg_config->rotationout[1][0] = 0.0;
      dg_config->rotationout[1][1] = 1.0;
      dg_config->rotationout[1][2] = 0.0;
      dg_config->rotationout[2][0] = 0.0;
      dg_config->rotationout[2][1] = 0.0;
      dg_config->rotationout[2][2] = 1.0;
    }
  }

  dg_config->zoomFactor = 1.0;
  dg_config->cameraDownTilt = 0.0;//3.14*15/180;
  dg_config->useGyro = false;

  //Dump camera config
  IS_INFO("DUMP: camera config");
  IS_INFO("ipW:%d ipH:%d ipStride:%d ipOffset:%d",
      dg_config->camera.pixelWidth,
      dg_config->camera.pixelHeight,
      dg_config->camera.memoryStride,
      dg_config->camera.uvOffset);
  IS_INFO("principalPoint:[%f %f] focalLength:[%f %f]",
      dg_config->camera.principalPoint[0],
      dg_config->camera.principalPoint[1],
      dg_config->camera.focalLength[0],
      dg_config->camera.focalLength[1]);
  IS_INFO("distortion:[%f %f %f %f %f %f %f %f] dist model:%d",
      dg_config->camera.distortion[0],
      dg_config->camera.distortion[1],
      dg_config->camera.distortion[2],
      dg_config->camera.distortion[3],
      dg_config->camera.distortion[4],
      dg_config->camera.distortion[5],
      dg_config->camera.distortion[6],
      dg_config->camera.distortion[7],
      dg_config->camera.distortionModel);

  IS_INFO("opW:%d opH:%d opStride:%d opOffset:%d framerate:%d",
      dg_config->pxlWidth,
      dg_config->pxlHeight,
      dg_config->rowMemoryStride,
      dg_config->uvOffset,
      dg_config->FrameRateInHz);
  IS_INFO("rotationin matrix: {%f %f %f} {%f %f %f} {%f %f %f}",
      dg_config->rotationin[0][0], dg_config->rotationin[0][1], dg_config->rotationin[0][2],
      dg_config->rotationin[1][0], dg_config->rotationin[1][1], dg_config->rotationin[1][2],
      dg_config->rotationin[2][0], dg_config->rotationin[2][1], dg_config->rotationin[2][2]);
  IS_INFO("rotationout matrix: {%f %f %f} {%f %f %f} {%f %f %f}",
      dg_config->rotationout[0][0], dg_config->rotationout[0][1], dg_config->rotationout[0][2],
      dg_config->rotationout[1][0], dg_config->rotationout[1][1], dg_config->rotationout[1][2],
      dg_config->rotationout[2][0], dg_config->rotationout[2][1], dg_config->rotationout[2][2]);

  IS_INFO("zoomFactor:%f cameraDownTilt:%f useGyro:%d",
      dg_config->zoomFactor,dg_config->cameraDownTilt,dg_config->useGyro);

/*
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
    IS_PROP_ENABLE_EIS_DG_LOG(dg_config->debug_print_en);
    dg_config->fbis_margin = FBIS_MARGIN;
    dg_config->rsc_margin = RSC_MARGIN;
    dg_config->imu_data_sampling_rate = IMU_MAX_SAMPLE_RATE;
    //IMU calibration data
    dg_config->imu_calib.gyro_unit = MAX_Q16;
    dg_config->imu_calib.acc_unit = MAX_Q16;
    dg_config->imu_calib.focal_length_in_mm = //2.6;
      init_data->sensor_out_info.focal_length;
    dg_config->imu_calib.pixel_size_in_um = //3.24;
      init_data->sensor_out_info.pix_size;
    for (i=0; i < 3; i++) {
      for(j =0; j < 3; j++) {
        dg_config->imu_calib.Rbc[i*3+j] = dg_config->rotationin[i][j];
      }
    }
*/
}

void dg_update_win_size(
  dg_interface_handle *dg_interface,
  is_init_data_t *init_data){

}

int digital_gimbal_deinitialize(void **dg_handle, int32_t *frame_id) {
  int err = 0;

  IS_LOW("E");

  /*input validation*/
  if(!dg_handle) {
    IS_ERR("invalid input %p", dg_handle);
  }
  dg_interface_handle *dg_interface = (dg_interface_handle *)(*dg_handle);

  if(dg_interface->funct_pointers.stop_session) {
    err = dg_interface->funct_pointers.stop_session(dg_interface->dgHandle, frame_id);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : stop_session()");
  }

  if(dg_interface->funct_pointers.deinit) {
    dg_interface->funct_pointers.deinit(dg_interface->dgHandle);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : deinit()");
  }

  if(dg_interface->mvDG_lib_handle){
    dlclose(dg_interface->mvDG_lib_handle);
  }

  if(dg_interface) {
    free(dg_interface);
  }

  IS_LOW("X");

  return err;
}

int digital_gimbal_initialize(void **dg_handle, is_init_data_t *init_data) {
  int err = 0, x;
  void *mvdg_lib = NULL;

  IS_LOW("E");

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
  dg_interface_handle *dg_interface =
    (dg_interface_handle *)calloc(1, sizeof(dg_interface_handle));
  if(dg_interface == NULL) {
    IS_ERR("Memory allocation error!!");
    err = -1;
    return err;
  }

  dg_interface->mvDG_lib_handle = mvdg_lib;

  if(!dg_fill_func_table(dg_interface)){
    IS_ERR("Failed during func ptr!!" );
    err = -1;
    free(dg_interface);
    dg_interface = NULL;
    return err;
  }

  //Get handle
  if(dg_interface->funct_pointers.init_get_handle) {
    dg_interface->dgHandle = dg_interface->funct_pointers.init_get_handle();
  } else {
    err = -1;
    IS_ERR("Not a valid function ptr : init_get_handle()");
    return err;
  }

  mvDGConfiguration *dg_config = &dg_interface->dg_config;

  //get the camera config
  dg_get_camera_config(dg_config, init_data);

  // FIll up window sizes
  dg_update_win_size(dg_interface, init_data);

  //start session
  if(dg_interface->funct_pointers.start_session) {
    err = dg_interface->funct_pointers.start_session(dg_interface->dgHandle,dg_config);
  } else {
    err = -1;
    IS_ERR("No valid function ptr : start_session");
    free(dg_interface);
    dg_interface = NULL;
    return err;
  }

  //get EIS DG transform type
  dg_interface->dg_type = (TransfMatrixType)(IS_PROP_GET_DG_TYPE(x));
  dg_interface->frame_id = 1;

  //read the dewarp size from lib API
  if(dg_interface->funct_pointers.get_tranf_mat_number) {
    err = dg_interface->funct_pointers.get_tranf_mat_number(dg_interface->dgHandle, dg_interface->frame_id,
      dg_interface->dg_type, &dg_interface->nTransfArrayW, &dg_interface->nTransfArrayH);
    IS_INFO(" Type: %d Trans num X: %d Y: %d",
    dg_interface->dg_type,
    dg_interface->nTransfArrayW,
    dg_interface->nTransfArrayH);
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : get_tranf_mat_number()");
    free(dg_interface);
    dg_interface = NULL;
    return err;
  }

  //Adapt window init.
  /*if(dg_interface->funct_pointers.mvDG_Adapt_window) {
    err = dg_interface->funct_pointers.mvDG_Adapt_window(
        dg_interface->dgHandle,
        &dg_interface->sensor_win,
        &dg_interface->camif_win,
        &dg_interface->vfe_win);
    IS_LOW("Adapt window for EIS success");
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : mvDG_Adapt_window()");
    free(dg_interface);
    dg_interface = NULL;
    return err;
  }*/

  IS_PROP_GET_MAT_DUMP(dg_interface->mat_dump);
  //return handle
  *dg_handle = (void *)dg_interface;

  IS_LOW("X");

  return err;
}

int digital_gimbal_get_mesh_size(void *dg_handle, uint32_t *mesh_x, uint32_t *mesh_y) {
  //input validation
  if(!(dg_handle && mesh_x && mesh_y)) {
    IS_ERR("Invalid input: %p %p %p", dg_handle, mesh_x,mesh_y);
    return -1;
  }

  dg_interface_handle *dg_interface = (dg_interface_handle *)dg_handle;
  //mesh size is one less than vertices
  *mesh_x = dg_interface->nTransfArrayW - 1;
  *mesh_y = dg_interface->nTransfArrayH - 1;
  return 0;
}


int digital_gimbal_process(void *dg_handle, digital_gimbal_input_t *dg_input,
  is_output_type *is_output) {

  //input validation
  if(!(dg_handle && dg_input && is_output)) {
    IS_ERR("Invalid input: %p %p %p", dg_handle, dg_input,is_output);
    return -1;
  }

  FILE* file;
  char filename[128];

  uint32_t num_gyro_samples, i, j, k;
  int32_t frame_id;
  mvDGTransfMatrixArrayInfo rTransfArray;
  int err = 0;
  dg_interface_handle *dg_interface = (dg_interface_handle *)dg_handle;
  mvAttitude *IMUptr = dg_interface->imu_input;
  mct_event_imu_stats_t *imu_data = dg_input->imu_data;

  mvDGCameraFrameTimeInfo dg_frame_times;

  IS_LOW("DGTC begin: frame id %d, sample len %d",
    is_output->frame_id,imu_data->sample_len);

  for(i =0;i < imu_data->sample_len; i++) {
    IMUptr[i].time_usec = imu_data->sample[i].timestamp;
    IMUptr[i].rotation[0] = imu_data->sample[i].rotation_matrix[0];
    IMUptr[i].rotation[1] = imu_data->sample[i].rotation_matrix[1];
    IMUptr[i].rotation[2] = imu_data->sample[i].rotation_matrix[2];
    IMUptr[i].rotation[3] = imu_data->sample[i].rotation_matrix[3];
    IMUptr[i].rotation[4] = imu_data->sample[i].rotation_matrix[4];
    IMUptr[i].rotation[5] = imu_data->sample[i].rotation_matrix[5];
    IMUptr[i].rotation[6] = imu_data->sample[i].rotation_matrix[6];
    IMUptr[i].rotation[7] = imu_data->sample[i].rotation_matrix[7];
    IMUptr[i].rotation[8] = imu_data->sample[i].rotation_matrix[8];
    IS_LOW("IMU sample[%d], %llu, %f, %f, %f, %f, %f, %f, %f, %f, %f", i,
      IMUptr[i].time_usec,
      IMUptr[i].rotation[0],
      IMUptr[i].rotation[1],
      IMUptr[i].rotation[2],
      IMUptr[i].rotation[3],
      IMUptr[i].rotation[4],
      IMUptr[i].rotation[5],
      IMUptr[i].rotation[6],
      IMUptr[i].rotation[7],
      IMUptr[i].rotation[8]);
  }
  num_gyro_samples = imu_data->sample_len;

  /*Pass the gyro data to the mvDGTC lib*/
  if((void*)(dg_interface->funct_pointers.add_imu_data)) {
    dg_interface->funct_pointers.add_imu_data(dg_interface->dgHandle,
      num_gyro_samples, IMUptr);
    IS_LOW("Add gyro data successfully");
  } else {
    err = -1;
    IS_ERR("Invalid Function Pointer : add_imu_data");
    return err;
  }

  /*update image frame timing info*/
  dg_frame_times.time_usec = dg_input->frame_times.sof/* * S2MS */;
  dg_frame_times.durationTime_usec = dg_input->frame_times.frame_time;
  dg_frame_times.exposureTime_usec = (int)(dg_input->frame_times.exposure_time*S2US);
  if(dg_frame_times.exposureTime_usec == 0) {
    dg_frame_times.exposureTime_usec = DEFAULT_EXP;
  }
  dg_frame_times.lineReadoutTime_usec = dg_input->frame_times.linereadout_time/S2MS;
  frame_id = is_output->frame_id;
  dg_frame_times.frameid = frame_id; //this frame is gyro arrival fid!!!

  IS_INFO("Frame id:%d sof :%lld exp_time: %lld dur_time: %lld l_rd_time:%f",
    dg_frame_times.frameid,
    dg_frame_times.time_usec,
    dg_frame_times.exposureTime_usec,
    dg_frame_times.durationTime_usec,
    dg_frame_times.lineReadoutTime_usec);


  /*Pass the frame timing info and frame id to mvDGTC lib*/
 if(dg_interface->funct_pointers.add_frame_times) {
    dg_interface->funct_pointers.add_frame_times(dg_interface->dgHandle,
      &dg_frame_times);
  } else {
    err = -1;
    IS_ERR("Invalid Function pointer : add_frame_times()");
    return err;
  }

  rTransfArray.frameid = frame_id;
  rTransfArray.transfMatrixType = dg_interface->dg_type;
  rTransfArray.gridW = dg_interface->nTransfArrayW;
  rTransfArray.gridH = dg_interface->nTransfArrayH;
  rTransfArray.transfArray = (mvPose3DR *)is_output->transform_matrix;

  if(dg_interface->funct_pointers.get_frame_transf_mat) {
    if(num_gyro_samples > 0) {
      err = dg_interface->funct_pointers.get_frame_transf_mat(dg_interface->dgHandle,
         frame_id, dg_interface->dg_type, &rTransfArray);
      IS_INFO("matrix calc: %d", err);
    }
    IS_LOW("gyro_samples : %d got warp map correction for frame :%d",
        num_gyro_samples,rTransfArray.frameid);
    for (i = 0; i < 10; i++) {
      IS_LOW("tt = %u, tm = %f %f %f %f %f %f %f %f %f", dg_interface->dg_type,
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
    if(dg_interface->mat_dump > 0) {
      if(frame_id >= START_FRAME_DUMP &&
         frame_id < START_FRAME_DUMP + dg_interface->mat_dump){
        sprintf(filename, "/data/misc/camera/filename_%d.txt",frame_id);
        file = fopen(filename,"w");
        perror("DG");
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
  //if(err == 0) {
    /*Processing Successful. Update is_output struct.
        Transf matrix dim are according to Digital Gimbal spec currently*/
    is_output->has_output = TRUE;
   return 1;
  //} else {
  //  return 0;
  //}
  return err;
}
