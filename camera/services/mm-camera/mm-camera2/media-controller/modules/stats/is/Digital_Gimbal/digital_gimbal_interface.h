/* digital_gimbal_interface.h
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __DG_INTERFACE_H__
#define __DG_INTERFACE_H__
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
  property_get("persist.camera.is.dg_dump", prop, "0");\
  x = atoi(prop);\
}

#define IS_PROP_GET_DG_TYPE(x) {\
  char prop[DG_PROPERTY_MAX_LEN];\
  property_get("persist.camera.is.dg_type", prop, "2");\
  x = atoi(prop);\
}

#define IS_PROP_ENABLE_DG_LOG(x) {\
  char prop[DG_PROPERTY_MAX_LEN];\
  property_get("persist.camera.dg_log", prop, "0");\
  x = atoi(prop);\
  }

#else
#define IS_PROP_GET_MAT_DUMP(x) {x = 0;}
#define IS_PROP_GET_DG_TYPE(x) {x = 2;}
#define IS_PROP_ENABLE_DG_LOG(x){X = 0;}
#endif


/** digital_gimbal_input_t:
 *    @frame_times: times associated frame (SOF, exposure, duration)
 *    @eis_dg_type: Used to specify correction type like Homography, RSC
 *    @imu_data: pointer to IMU data buffer
 */
typedef struct {
  frame_times_t frame_times;
  uint8_t dg_type;
  mct_event_imu_stats_t *imu_data;
} digital_gimbal_input_t;

int digital_gimbal_deinitialize(void **dg_handle, int32_t *frame_id);
int digital_gimbal_initialize(void **dg_handle, is_init_data_t *init_data);
int digital_gimbal_process(void *eis_dg_handle,
  digital_gimbal_input_t *eis_dg_input, is_output_type *is_output);
int digital_gimbal_get_mesh_size(void *eis_dg_handle, uint32_t *mesh_x, uint32_t *mesh_y);


#ifdef __cplusplus
}
#endif


#endif //__DG_INTERFACE_H__

