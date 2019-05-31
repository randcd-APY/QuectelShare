/* mesh_fusion_interface.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "stats_debug.h"
#include "mesh_fusion_interface.h"
#include "eis_dg/mesh_fusion.h"
#include <dlfcn.h>

typedef struct {
  int (*mesh_fusion)(void*,mesh_config_t*);
  int (*mesh_fusion_adapt_window)(void *, mesh_config_t*,
    mesh_fusion_win_t*, mesh_fusion_win_t*, mesh_fusion_win_t*);
  int (*mesh_fusion_init)(void**);
  int (*mesh_fusion_deinit)(void*);
} mesh_fusion_funct_ptr;

typedef struct {
  mesh_fusion_funct_ptr funct_ptr;
  void *mf_lib_ptr;
  void *mf;
  mesh_config_t config;
  boolean ldc_sv_fused;
  mesh_fusion_win_t sensor_win;
  mesh_fusion_win_t camif_win;
  mesh_fusion_win_t vfe_win;
} mesh_fusion_intf;

int mesh_fusion_intf_update_win_size(
  mesh_fusion_intf *mf_handler,
  is_info_t *is_info){

  //sensor win
  mf_handler->sensor_win.input_width =
    is_info->sensor_out_info.full_width;
  mf_handler->sensor_win.input_height =
    is_info->sensor_out_info.full_height;
  mf_handler->sensor_win.output_width =
    is_info->sensor_out_info.sensor_width;
  mf_handler->sensor_win.output_height =
    is_info->sensor_out_info.sensor_height;
  mf_handler->sensor_win.start_x =
    is_info->sensor_out_info.offset_x;
  mf_handler->sensor_win.start_y =
    is_info->sensor_out_info.offset_y;
  mf_handler->sensor_win.end_x =
    mf_handler->sensor_win.start_x +
    mf_handler->sensor_win.output_width;
  mf_handler->sensor_win.end_y =
    mf_handler->sensor_win.start_y +
    mf_handler->sensor_win.output_height;
  mf_handler->sensor_win.binning_en =
    is_info->sensor_out_info.binning_factor;
  mf_handler->sensor_win.hblank =
    is_info->sensor_out_info.ll_pck -
    mf_handler->sensor_win.output_width;
  mf_handler->sensor_win.vblank =
    is_info->sensor_out_info.fl_lines -
    mf_handler->sensor_win.output_height;
  IS_LOW("SEN_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    mf_handler->sensor_win.input_width,
    mf_handler->sensor_win.input_height,
    mf_handler->sensor_win.output_width,
    mf_handler->sensor_win.output_height,
    mf_handler->sensor_win.start_x,
    mf_handler->sensor_win.start_y,
    mf_handler->sensor_win.end_x,
    mf_handler->sensor_win.end_y,
    mf_handler->sensor_win.binning_en,
    mf_handler->sensor_win.hblank,
    mf_handler->sensor_win.vblank);

  //camif win
  mf_handler->camif_win.input_width =
    is_info->sensor_out_info.sensor_width;
  mf_handler->camif_win.input_height =
    is_info->sensor_out_info.sensor_height;
  mf_handler->camif_win.output_width =
    is_info->sensor_out_info.camif_width;
  mf_handler->camif_win.output_height =
    is_info->sensor_out_info.camif_height;
  mf_handler->camif_win.start_x =
    is_info->sensor_out_info.camif_crop_x;
  mf_handler->camif_win.start_y =
    is_info->sensor_out_info.camif_crop_y;
  mf_handler->camif_win.end_x =
    mf_handler->camif_win.start_x +
    mf_handler->camif_win.output_width;
  mf_handler->camif_win.end_y =
    mf_handler->camif_win.start_y +
    mf_handler->camif_win.output_height;
  mf_handler->camif_win.binning_en = 0;
  mf_handler->camif_win.hblank = 0;
  mf_handler->camif_win.vblank = 0;

  IS_LOW("CAMIF_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    mf_handler->camif_win.input_width,
    mf_handler->camif_win.input_height,
    mf_handler->camif_win.output_width,
    mf_handler->camif_win.output_height,
    mf_handler->camif_win.start_x,
    mf_handler->camif_win.start_y,
    mf_handler->camif_win.end_x,
    mf_handler->camif_win.end_y,
    mf_handler->camif_win.binning_en,
    mf_handler->camif_win.hblank,
    mf_handler->camif_win.vblank);

  //ISP win
  mf_handler->vfe_win.input_width =
    mf_handler->camif_win.output_width;
  mf_handler->vfe_win.input_height =
    mf_handler->camif_win.output_height;
  mf_handler->vfe_win.output_width =
    is_info->vfe_width[IS_VIDEO];
  mf_handler->vfe_win.output_height =
    is_info->vfe_height[IS_VIDEO];
  //TODO: Need to fill vfe x ,y based on crop For now think there is no crop but just scale
  mf_handler->vfe_win.start_x = 0;
  mf_handler->vfe_win.start_y = 0;
  mf_handler->vfe_win.end_x =
    mf_handler->vfe_win.start_x +
    mf_handler->vfe_win.input_width;
  mf_handler->vfe_win.end_y =
    mf_handler->vfe_win.start_y +
    mf_handler->vfe_win.input_height;
  mf_handler->vfe_win.binning_en = 0;
  mf_handler->vfe_win.hblank = 0;
  mf_handler->vfe_win.vblank = 0;

  IS_LOW("VFE_WIN: %d %d %d %d %d %d %d %d %d %d %d",
    mf_handler->vfe_win.input_width,
    mf_handler->vfe_win.input_height,
    mf_handler->vfe_win.output_width,
    mf_handler->vfe_win.output_height,
    mf_handler->vfe_win.start_x,
    mf_handler->vfe_win.start_y,
    mf_handler->vfe_win.end_x,
    mf_handler->vfe_win.end_y,
    mf_handler->vfe_win.binning_en,
    mf_handler->vfe_win.hblank,
    mf_handler->vfe_win.vblank);

  /*Call the update win API*/
  if(mf_handler->funct_ptr.mesh_fusion_adapt_window) {
    mf_handler->funct_ptr.mesh_fusion_adapt_window
      (mf_handler->mf,&mf_handler->config,&mf_handler->sensor_win,
      &mf_handler->camif_win, &mf_handler->vfe_win);
    IS_LOW("Called mesh fusion adapt win successfully");
  } else {
    IS_ERR("Invalid Function Pointer : mesh fusion");
    return 0;
  }
  return 1;
}



int mesh_fusion_intf_update_data(void *mf_handle, is_info_t *is_info){
  uint32_t i;
  int err = -1;
  //input validation
  if(!(mf_handle)) {
    IS_ERR("Invalid input: %p", mf_handle);
    return err;
  }

  dewarp_tuning_t *dewarp_tuning = &is_info->dewarp_tuning;
  if(dewarp_tuning->dgtc_tuning_data == NULL ||
    dewarp_tuning->ldc_tuning_data == NULL ||
    dewarp_tuning->custom_warpmap == NULL) {
    IS_ERR("dewarp tuning ptr is null %p %p %p",
      dewarp_tuning->dgtc_tuning_data,
      dewarp_tuning->ldc_tuning_data,
      dewarp_tuning->custom_warpmap);
    return err;
  }
  mesh_fusion_intf *mf_handler = (mesh_fusion_intf *)mf_handle;
  warpmap_t *ldc_mesh = &dewarp_tuning->ldc_tuning_data->ldc_mesh;
  warpmap_t *custom_mesh = &dewarp_tuning->custom_warpmap->cus_warpmap[0];
  //LDC
  mf_handler->config.ldc_mesh->mesh_vertices_num_x = ldc_mesh->block_w;
  mf_handler->config.ldc_mesh->mesh_vertices_num_y = ldc_mesh->block_h;
  mf_handler->config.ldc_mesh->input_width = ldc_mesh->input_width;
  mf_handler->config.ldc_mesh->input_height = ldc_mesh->input_height;
  mf_handler->config.ldc_mesh->output_width = ldc_mesh->output_width;
  mf_handler->config.ldc_mesh->output_height = ldc_mesh->output_height;
  mf_handler->config.ldc_mesh->type = ldc_mesh->type;
  IS_LOW("LDC data :%d %d %d %d %d %d",
    ldc_mesh->block_w,ldc_mesh->block_h,ldc_mesh->input_width,ldc_mesh->input_height,
    ldc_mesh->output_width,ldc_mesh->output_height);
  for (i = 0; i < 10; i++) {
    IS_LOW("tt = %u, tm = %f %f %f %f %f %f %f %f %f", ldc_mesh->type,
      ldc_mesh->mapping[i].RT[0],
      ldc_mesh->mapping[i].RT[1],
      ldc_mesh->mapping[i].RT[2],
      ldc_mesh->mapping[i].RT[3],
      ldc_mesh->mapping[i].RT[4],
      ldc_mesh->mapping[i].RT[5],
      ldc_mesh->mapping[i].RT[6],
      ldc_mesh->mapping[i].RT[7],
      ldc_mesh->mapping[i].RT[8]);
  }

  memcpy(mf_handler->config.ldc_mesh->mapping,ldc_mesh->mapping,
    sizeof(mf_handler->config.ldc_mesh->mapping));
  mf_handler->config.ldc_mesh->input_o_center_x = ldc_mesh->input_width/2;
  mf_handler->config.ldc_mesh->input_o_center_y = ldc_mesh->input_height/2;
  mf_handler->config.ldc_mesh->output_o_center_x = ldc_mesh->output_width/2;
  mf_handler->config.ldc_mesh->output_o_center_y = ldc_mesh->output_height/2;

  //custom
  mf_handler->config.super_view_mesh->mesh_vertices_num_x = custom_mesh->block_w;
  mf_handler->config.super_view_mesh->mesh_vertices_num_y = custom_mesh->block_h;
  mf_handler->config.super_view_mesh->input_width = custom_mesh->input_width;
  mf_handler->config.super_view_mesh->input_height = custom_mesh->input_height;
  mf_handler->config.super_view_mesh->output_width = custom_mesh->output_width;
  mf_handler->config.super_view_mesh->output_height = custom_mesh->output_height;
  mf_handler->config.super_view_mesh->type = custom_mesh->type;
  IS_LOW("superview data :%d %d %d %d %d %d",
    custom_mesh->block_w,custom_mesh->block_h,custom_mesh->input_width,custom_mesh->input_height,
    custom_mesh->output_width,custom_mesh->output_height);
  for (i = 0; i < 10; i++) {
    IS_LOW("tt = %u, tm = %f %f %f %f %f %f %f %f %f", custom_mesh->type,
      custom_mesh->mapping[i].RT[0],
      custom_mesh->mapping[i].RT[1],
      custom_mesh->mapping[i].RT[2],
      custom_mesh->mapping[i].RT[3],
      custom_mesh->mapping[i].RT[4],
      custom_mesh->mapping[i].RT[5],
      custom_mesh->mapping[i].RT[6],
      custom_mesh->mapping[i].RT[7],
      custom_mesh->mapping[i].RT[8]);
  }
  memcpy(mf_handler->config.super_view_mesh->mapping,custom_mesh->mapping,
    sizeof(mf_handler->config.super_view_mesh->mapping));
  mf_handler->config.super_view_mesh->input_o_center_x = custom_mesh->input_width/2;
  mf_handler->config.super_view_mesh->input_o_center_y = custom_mesh->input_height/2;
  mf_handler->config.super_view_mesh->output_o_center_x = custom_mesh->output_width/2;
  mf_handler->config.super_view_mesh->output_o_center_y = custom_mesh->output_height/2;

//EIS mesh config
  mf_handler->config.eis_mesh->mesh_vertices_num_x = is_info->num_mesh_x+1;
  mf_handler->config.eis_mesh->mesh_vertices_num_y = is_info->num_mesh_y+1;
  mf_handler->config.eis_mesh->input_width = is_info->vfe_width[IS_VIDEO];
  mf_handler->config.eis_mesh->input_height = is_info->vfe_height[IS_VIDEO];
  mf_handler->config.eis_mesh->output_width = is_info->width[IS_VIDEO];
  mf_handler->config.eis_mesh->output_height = is_info->height[IS_VIDEO];
  mf_handler->config.eis_mesh->type = 1;
  mf_handler->config.eis_mesh->input_o_center_x =
   mf_handler->config.eis_mesh->input_width/2;
  mf_handler->config.eis_mesh->input_o_center_y =
    mf_handler->config.eis_mesh->input_height/2;
  mf_handler->config.eis_mesh->output_o_center_x =
    mf_handler->config.eis_mesh->output_width/2;
  mf_handler->config.eis_mesh->output_o_center_y =
    mf_handler->config.eis_mesh->output_height/2;

  IS_LOW("input: vfe_width %d  vfe_height %d",is_info->vfe_width[IS_VIDEO],is_info->vfe_height[IS_VIDEO]);
  IS_LOW("output: width %d height %d",is_info->width[IS_VIDEO],is_info->height[IS_VIDEO]);
  //output mesh config
  mf_handler->config.output_mesh->mesh_vertices_num_x =is_info->num_mesh_x+1;
  mf_handler->config.output_mesh->mesh_vertices_num_y = is_info->num_mesh_y+1;
  mf_handler->config.output_mesh->input_width = is_info->vfe_width[IS_VIDEO];
  mf_handler->config.output_mesh->input_height = is_info->vfe_height[IS_VIDEO];
  mf_handler->config.output_mesh->output_width = is_info->width[IS_VIDEO];
  mf_handler->config.output_mesh->output_height = is_info->height[IS_VIDEO];
  mf_handler->config.output_mesh->type = 1;
  mf_handler->config.output_mesh->input_o_center_x =
    mf_handler->config.output_mesh->input_width/2;
  mf_handler->config.output_mesh->input_o_center_y =
    mf_handler->config.output_mesh->input_height/2;
  mf_handler->config.output_mesh->output_o_center_x =
    mf_handler->config.output_mesh->output_width/2;
  mf_handler->config.output_mesh->output_o_center_y =
    mf_handler->config.output_mesh->output_height/2;

  /*Update window*/
  if(mesh_fusion_intf_update_win_size(mf_handler,is_info)) {
    IS_LOW("Mesh fusion adapt win call success");
    err = 0;
  } else {
    IS_ERR("Error calling mesh fusion adapt win");
    return err;
  }

  /*Initialize ldc_sv_fused flag to FALSE*/
  mf_handler->ldc_sv_fused = FALSE;
  return err;

}

int mesh_fusion_intf_init(void **mf_handle) {
  void *mf_lib = NULL;
  *mf_handle = NULL;
  int err = -1, rc;
  //open the lib
  dlerror();
  mf_lib = dlopen("/usr/lib/libmesh_fusion.so", RTLD_LAZY);
  if (mf_lib == NULL) {
    err = -1;
    IS_LOW("Library Open Fail :%s", dlerror());
    return err;
  } else {
    IS_LOW("Library Open Success");
  }
  //allocate memory
  mesh_fusion_intf *mf_handler =
    (mesh_fusion_intf *)malloc(sizeof(mesh_fusion_intf));
  if(mf_handler == NULL) {
    IS_ERR("Memory allocation error!!");
    return err;
  }

  memset(mf_handler, 0, sizeof(mesh_fusion_intf));
  *mf_handle = (void *)mf_handler;

  //allocate memory for LDC_mesh
  mf_handler->config.ldc_mesh =
  (mesh_t*)malloc(sizeof(mesh_t));
  if(mf_handler->config.ldc_mesh == NULL) {
    IS_ERR("LDC mesh memory error!!");
    return err;
  }
  //allocate memory for superview_mesh
  mf_handler->config.super_view_mesh =
  (mesh_t*)malloc(sizeof(mesh_t));
  if(mf_handler->config.super_view_mesh == NULL) {
    IS_ERR("superview mesh memory error!!");
    return err;
  }
  //allocate memory for EIS_mesh
  mf_handler->config.eis_mesh =
  (mesh_t*)malloc(sizeof(mesh_t));
  if(mf_handler->config.eis_mesh == NULL) {
    IS_ERR("eis mesh memory error!!");
    return err;
  }
  //allocate memory for output_mesh
  mf_handler->config.output_mesh =
  (mesh_t*)malloc(sizeof(mesh_t));
  if(mf_handler->config.output_mesh == NULL) {
    IS_ERR("output mesh memory error!!");
    return err;
  }

  mf_handler->mf_lib_ptr = mf_lib;
  // get the function ptr
  dlerror();

  mf_handler->funct_ptr.mesh_fusion_init =
     (int(*)(void**))dlsym(mf_lib, "mesh_fusion_init");
  if(mf_handler->funct_ptr.mesh_fusion_init == NULL){
   IS_ERR("mesh fusion init API fail %s", dlerror());
   return err;
  }

  mf_handler->funct_ptr.mesh_fusion_deinit =
  (int(*)(void*))dlsym(mf_lib, "mesh_fusion_deinit");
  if(mf_handler->funct_ptr.mesh_fusion_deinit == NULL){
    IS_ERR("mesh fusion deinit API fail %s", dlerror());
    return err;
  }

  mf_handler->funct_ptr.mesh_fusion =
    (int(*)(void*,mesh_config_t*))dlsym(mf_lib, "mesh_fusion");
  if(mf_handler->funct_ptr.mesh_fusion == NULL){
    IS_ERR("mesh fusion API fail %s", dlerror());
    return err;
  }

  mf_handler->funct_ptr.mesh_fusion_adapt_window =
    (  int (*)(void *, mesh_config_t*,mesh_fusion_win_t*,mesh_fusion_win_t*,
    mesh_fusion_win_t*))dlsym(mf_lib, "mesh_fusion_adapt_window");
  if(mf_handler->funct_ptr.mesh_fusion_adapt_window == NULL){
    IS_ERR("mesh fusion adapt win API fail %s", dlerror());
    return err;
  }

  //Get the handler for mesh fusion
  if(mf_handler->funct_ptr.mesh_fusion_init) {
    rc = mf_handler->funct_ptr.mesh_fusion_init(&mf_handler->mf);
    IS_LOW("Called mesh fusion init successfully");
  } else {
    err = -1;
    IS_ERR("No valid function ptr : mesh fusion init");
    return err;
  }
  return 0;
}

int mesh_fusion_intf_process(void *mf_handle,is_output_type *is_output, int bitmask) {
  //input validation
  if(!(mf_handle)) {
    IS_ERR("Invalid input: %p", mf_handle);
    return -1;
  }
  int err = 0;
  uint32_t i;
  mesh_fusion_intf *mf_handler = (mesh_fusion_intf *)mf_handle;
  mf_handler->config.eis_en = bitmask & EIS_ENABLE;
  mf_handler->config.ldc_en = bitmask & LDC_ENABLE;
  mf_handler->config.sv_en  = bitmask & CUSTOM_ENABLE;

  if(!mf_handler->config.ldc_en && !mf_handler->config.sv_en) {
    IS_HIGH("no need to do fusion exit");
    return 0;
  }

  if(!mf_handler->config.eis_en) {
    IS_LOW("EIS is disabled");
    if(mf_handler->funct_ptr.mesh_fusion) {
      mf_handler->funct_ptr.mesh_fusion(mf_handler->mf,&mf_handler->config);
      IS_LOW("Called mesh fusion successfully");
      mf_handler->ldc_sv_fused = TRUE;
    } else {
      err = -1;
      IS_ERR("Invalid Function Pointer : mesh fusion");
      return err;
    }
  } else {
    memcpy(mf_handler->config.eis_mesh->mapping,is_output->transform_matrix,
      sizeof(mf_handler->config.eis_mesh->mapping));
     /*call the mesh fusion API*/
    if(mf_handler->funct_ptr.mesh_fusion) {
      mf_handler->funct_ptr.mesh_fusion(mf_handler->mf,&mf_handler->config);
      IS_LOW("Called mesh fusion successfully");
    } else {
      err = -1;
      IS_ERR("Invalid Function Pointer : mesh fusion");
      return err;
    }
  }

  //Copy the output matrix back to is_output
  memcpy(is_output->transform_matrix,mf_handler->config.output_mesh->mapping,
    sizeof(mf_handler->config.output_mesh->mapping));
  return err;
}

int mesh_fusion_intf_deinit(void *mf_handle) {
    //input validation
  if(!(mf_handle)) {
    IS_ERR("Invalid input: %p %p %p", mf_handle);
    return -1;
  }
  mesh_fusion_intf *mf_handler = (mesh_fusion_intf*)mf_handle;

   /*call the mesh fusion deinit API*/
  if(mf_handler->funct_ptr.mesh_fusion_deinit) {
    mf_handler->funct_ptr.mesh_fusion_deinit(mf_handler->mf);
    IS_LOW("Called mesh fusion deinit successfully");
  } else {
    IS_ERR("Invalid Function Pointer : mesh fusion deinit");
  }
  //free up the memory
  if(mf_handler){
    if(mf_handler->config.ldc_mesh) {
      free(mf_handler->config.ldc_mesh);
      mf_handler->config.ldc_mesh = NULL;
    }
    if(mf_handler->config.eis_mesh) {
      free(mf_handler->config.eis_mesh);
      mf_handler->config.eis_mesh = NULL;
    }
    if(mf_handler->config.super_view_mesh) {
      free(mf_handler->config.super_view_mesh);
      mf_handler->config.super_view_mesh = NULL;
    }
    if(mf_handler->config.output_mesh) {
      free(mf_handler->config.output_mesh);
      mf_handler->config.output_mesh = NULL;
    }
    free(mf_handler);
    mf_handler = NULL;
  }
  return 0;
}

