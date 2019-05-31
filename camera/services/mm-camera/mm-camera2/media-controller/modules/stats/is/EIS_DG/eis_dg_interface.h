/* eis_dg_interface.h
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __EIS_DG_INTERFACE_H__
#define __EIS_DG_INTERFACE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "is_interface.h"
#include "stats_event.h"
#include "is_common_intf.h"
#include "stats_chromatix_wrapper.h"
#ifdef _ANDROID_
#include <cutils/properties.h>
#define DG_PROPERTY_MAX_LEN 96


#define IS_PROP_GET_MAT_DUMP(x) {\
  char prop[DG_PROPERTY_MAX_LEN];\
  property_get("persist.camera.is.eis_dg_dump", prop, "0");\
  x = atoi(prop);\
}

#define IS_PROP_GET_EIS_DG_TYPE(x) {\
  char prop[DG_PROPERTY_MAX_LEN];\
  property_get("persist.camera.is.eis_dg_type", prop, "2");\
  x = atoi(prop);\
}

#define IS_PROP_ENABLE_EIS_DG_LOG(x) {\
  char prop[DG_PROPERTY_MAX_LEN];\
  property_get("persist.camera.eis_dg_log", prop, "0");\
  x = atoi(prop);\
  }

#else
#define IS_PROP_GET_MAT_DUMP(x) {x = 0;}
#define IS_PROP_GET_EIS_DG_TYPE(x) {x = 2;}
#define IS_PROP_ENABLE_EIS_DG_LOG(x){X = 0;}
#endif


/** eis_dg_input_t:
 *    @frame_times: times associated frame (SOF, exposure, duration)
 *    @eis_dg_type: Used to specify correction type like Homography, RSC
 *    @gyro_data: pointer to Gyro data buffer
 */
typedef struct {
  frame_times_t frame_times;
  uint8_t eis_dg_type;
  mct_event_gyro_data_t *gyro_data;
} eis_dg_input_t;

int eis_dg_initialize(void **eis_dg_handle, is_init_data_t *init_data);
int eis_dg_process(void *eis_dg_handle, eis_dg_input_t *eis_dg_input,
  is_output_type *is_output);
int32_t eis_dg_deinitialize(void *eis3_handle, int32_t *frame_id);
int eis_dg_get_mesh_size(void *eis_dg_handle, uint32_t *mesh_x, uint32_t *mesh_y);


#ifdef __cplusplus
}
#endif


#endif //__EIS_DG_INTERFACE_H__

