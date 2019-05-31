/*============================================================================

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#ifndef __CSIPHY_H__
#define __CSIPHY_H__

#include <media/msm_cam_sensor.h>
#include "sensor_common.h"

/** sensor_csiphy_data_t:
*
*  @fd: CSIPHY v4l subdev fd
*  @cur_csiphy_params: store current CSIPHY params applied to hw
*  @csi_lane_params: store CSI lane params common to all resolutions
*
*  This struct store CSIPHY sub module information per session
**/
typedef struct {
  int32_t                    fd;
  struct sensor_csi_params   *cur_csi_params;
  camera_csi_params_t        *cur_camera_csi_params;
} sensor_csiphy_data_t;

#endif
