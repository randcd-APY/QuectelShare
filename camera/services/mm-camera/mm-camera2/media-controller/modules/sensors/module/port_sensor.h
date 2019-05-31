/* port_sensor.h
 *
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __PORT_SENSOR_H__
#define __PORT_SENSOR_H__

#include "sensor_common.h"

#define SENSOR_MEASURE_FPS 0

boolean port_sensor_create(void *data, void *user_data);
boolean port_sensor_handle_stream_on(mct_module_t *module, mct_event_t *event,
  sensor_bundle_info_t *bundle_info);

#endif /* __PORT_SENSOR_H__ */
