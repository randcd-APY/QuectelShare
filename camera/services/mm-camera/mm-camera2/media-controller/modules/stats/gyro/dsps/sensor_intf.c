/* sensor_intf.c
 *
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/

#ifdef _LE_CAMERA_
#undef _ANDROID_
#endif

#include "sensor_intf.h"
#ifdef _ANDROID_
#include "sensor_native.h"
#endif

#ifdef _ANDROID_
/*===========================================================================
 * FUNCTION      sensor_enable_req
 *    @sn_obj: handler of native interface
 *    @sensor: sensor type
 *
 * DESCRIPTION : Request to enable native sensor
 *=========================================================================*/
static sensor_return_type sensor_enable_req(sensor_native_type *sn_obj,
  sensor_type sensor)
{
  int rc = SENSOR_RET_SUCCESS;
  ASensorRef android_sensor = NULL;
  int32_t sample_rate = 0;
  boolean *enabled_success = NULL;

  switch (sensor) {
  case SENSOR_TYPE_GYRO: {
    if (sn_obj->gyro.present) {
      IS_LOW("Enable Gyro sensor and set default rate!");
      android_sensor = sn_obj->gyro.sensor;
      sample_rate = sn_obj->gyro.sample_rate;
      enabled_success = &sn_obj->gyro.enabled;
    } else {
      IS_ERR("Try to enable gyro but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_GRAVITY: {
    if (sn_obj->gravity.present) {
      IS_LOW("Enable gravity sensor and set default rate!");
      android_sensor = sn_obj->gravity.sensor;
      sample_rate = sn_obj->gravity.sample_rate;
      enabled_success = &sn_obj->gravity.enabled;
    } else {
      IS_ERR("Try to enable gravity but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_ACCELEROMETER: {
    if (sn_obj->accel.present) {
      IS_LOW("Enable Accel sensor and set default rate!");
      android_sensor = sn_obj->accel.sensor;
      sample_rate = sn_obj->accel.sample_rate;
      enabled_success = &sn_obj->accel.enabled;
    } else {
      IS_ERR("Try to enable accelerometer but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_LINEAR_ACCELERATION: {
    if (sn_obj->lin_accel.present) {
      IS_LOW("Enable linear acceleration sensor and set default rate!");
      android_sensor = sn_obj->lin_accel.sensor;
      sample_rate = sn_obj->lin_accel.sample_rate;
      enabled_success = &sn_obj->lin_accel.enabled;
    } else {
      IS_ERR("Try to enable linear acceleration but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_TIME_OF_FLIGHT: {
    if (sn_obj->laser.present) {
      IS_LOW("Enable laser sensor and set default rate!");
      android_sensor = sn_obj->laser.sensor;
      sample_rate = sn_obj->laser.sample_rate;
      enabled_success = &sn_obj->laser.enabled;
    } else {
      IS_ERR("Try to enabl laser sensor but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_LIGHT: {
    if (sn_obj->light.present) {
      IS_LOW("Enable light sensor and set default rate!");
      android_sensor = sn_obj->light.sensor;
      sample_rate = sn_obj->light.sample_rate;
      enabled_success = &sn_obj->light.enabled;
    } else {
      IS_ERR("Try to enable light but not present");
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  default:
    IS_ERR("Unkown sensor type: %d", sensor);
    rc = SENSOR_RET_BAD_PARAMS;
    break;
  }

  /* Calls to android sensor interface to perform operations */
  if (rc == SENSOR_RET_SUCCESS) {
    if (enabled_success) {
      /* enable the sensor only after eventqueue is created*/
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_enableSensor(sn_obj->event_queue, android_sensor);
      } else {
        rc = -1;
      }
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if (rc < 0) {
        IS_ERR("Fail to enable sensor");
      } else {
        pthread_mutex_lock(&(sn_obj->cb_mutex));
        *enabled_success = TRUE;
        pthread_mutex_unlock(&(sn_obj->cb_mutex));
        rc = ASensorEventQueue_setEventRate(sn_obj->event_queue, android_sensor,
          sample_rate);
        if (rc < 0) {
          IS_ERR("Fail to set event rate");
        }
      }
    } else {
      IS_ERR("Fail to init param enabled_success");
      rc = SENSOR_RET_BAD_PARAMS;
    }
  }

  if (rc < 0)
    rc = SENSOR_RET_FAILURE;

  return rc;
}

/*===========================================================================
 * FUNCTION      sensor_disable_req
 *    @sn_obj: handler of native interface
 *    @sensor: sensor type
 *
 * DESCRIPTION : Request to disable native sensor
 *=========================================================================*/
static sensor_return_type sensor_disable_req(sensor_native_type *sn_obj,
  sensor_type sensor)
{
  int rc = SENSOR_RET_SUCCESS;

  switch (sensor) {
  case SENSOR_TYPE_GYRO: {
    if (sn_obj->gyro.enabled) {
      IS_LOW("Disable Gyro Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->gyro.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->gyro.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->gyro.sensor);
      } else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_GRAVITY: {
    if (sn_obj->gravity.enabled) {
      IS_LOW("Disable Gravity Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->gravity.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->gravity.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->gravity.sensor);
      } else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_ACCELEROMETER: {
    if (sn_obj->accel.enabled) {
      IS_LOW("Disable Acceleroemter Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->accel.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->accel.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
         sn_obj->accel.sensor);
      } else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_LINEAR_ACCELERATION: {
    if (sn_obj->lin_accel.enabled) {
      IS_LOW("Disable Linear accel Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->lin_accel.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->lin_accel.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->lin_accel.sensor);
      } else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_TIME_OF_FLIGHT: {
    if (sn_obj->laser.enabled) {
      IS_LOW("Disable Laser Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->laser.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->laser.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
         sn_obj->laser.sensor);
      } else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }

    }
  }
  break;
  case SENSOR_TYPE_LIGHT: {
    if (sn_obj->light.enabled) {
      IS_LOW("Disable Light Sensor!");
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->light.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue && sn_obj->light.sensor) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->light.sensor);}
      else {
        IS_ERR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
    break;
  default:
    IS_ERR("Unkown sensor type: %d", sensor);
    rc = SENSOR_RET_BAD_PARAMS;
  break;
  }

  if (rc < 0)
    rc = SENSOR_RET_FAILURE;

  return rc;
}
#endif /* _ANDROID_ */


/*===========================================================================
 * FUNCTION      sensor_init
 *    @handle: Handler
 *    @cb_handle:Callback function of client
 *    @cb: Callback data fnction
 *
 * DESCRIPTION : initializaties the sensor interface
 *=========================================================================*/
sensor_return_type sensor_intf_init(
  void **handle,
  void *cb_hndl,
  sensor_data_cb cb)
{
  sensor_return_type rc = SENSOR_RET_SUCCESS;
#ifdef _ANDROID_
  /* just call native interface */
  rc = sensor_native_init(handle, cb_hndl, cb);
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
  STATS_UNUSED(cb_hndl);
  STATS_UNUSED(cb);
  rc = SENSOR_RET_FAILURE;
#endif
  return rc;
}

/*===========================================================================
 * FUNCTION      sensor_deinit
 *    @handle: Handler
 *
 * DESCRIPTION : Deinitializaties the sensor interface
 *=========================================================================*/
sensor_return_type sensor_intf_deinit(void *handle)
{
#ifdef _ANDROID_
  sensor_native_deinit(handle);
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
#endif
  return SENSOR_RET_SUCCESS;
}

/*===========================================================================
 * FUNCTION      sensor_set_params
 *    @handle: Handler
 *    @set_data : set/req data info
 *
 * DESCRIPTION : sets/config the sensor data.
 *=========================================================================*/
sensor_return_type sensor_set_params(
  void *handle, sensor_set_data_type set_data)
{
  sensor_return_type rc = SENSOR_RET_SUCCESS;
#ifdef _ANDROID_
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  if (!sn_obj->initialized) {
    IS_HIGH("sensor object not initialized!");
    return rc;
  }
  switch (set_data.msg_type) {
  case SENSOR_SET_DATA_CONFIG:
    if (set_data.sensor == SENSOR_TYPE_GYRO) {
      sensor_data_range_type data_range = set_data.u.data_range;
      pthread_mutex_lock(&(sn_obj->sof_req_mutex));
      if (sn_obj->prev_request_proc) {
        IS_LOW("sof_event :%d", data_range.id);
        sn_obj->sof_info.id = data_range.id;
        sn_obj->sof_info.t_start = data_range.t_start;
        sn_obj->sof_info.t_end = data_range.t_end;
        sn_obj->prev_request_proc = FALSE ;
      } else {
        IS_LOW("New sof_event :%d", data_range.id);
        sn_obj->sof_request = TRUE;
        sn_obj->sof_info_next.id = data_range.id;
        sn_obj->sof_info_next.t_start = data_range.t_start;
        sn_obj->sof_info_next.t_end = data_range.t_end;
      }
      pthread_mutex_unlock(&(sn_obj->sof_req_mutex));
      rc = SENSOR_RET_SUCCESS;
    }
    break;
  case SENSOR_ENABLE_REQ:
    rc = sensor_enable_req(sn_obj, set_data.sensor);
    break;
  case SENSOR_DISABLE_REQ:
    rc = sensor_disable_req(sn_obj, set_data.sensor);
    break;
  default:
    IS_ERR("Error not handle msg_type: %d", set_data.msg_type);
    rc = SENSOR_RET_BAD_PARAMS;
    break;
  }
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
  STATS_UNUSED(set_data);
  rc = SENSOR_RET_FAILURE;
#endif /* _ANDROID_ */
  return rc;
}
