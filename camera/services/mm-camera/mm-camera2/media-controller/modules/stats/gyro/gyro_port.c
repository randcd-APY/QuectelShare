/* gyro_port.c
 *
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "mct_port.h"
#include "mct_stream.h"
#include "modules.h"
#include "stats_module.h"
#include "stats_event.h"
#include "aec.h"
#include "camera_dbg.h"
#include "sensor_intf.h"
#include "sensor_laser.h"
#include "dsps_hw_interface.h"
#ifdef FEATURE_GYRO_DSPS
 #include "dsps_hw.h"
#endif
#ifdef FEATURE_GYRO_IMU
 #include "imu_driver.h"
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC        1000000
#endif
#define Q8              (0x00000100)
#define BUF_SIZE        (4)
#ifndef MAX
#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#endif
#define SENSORS_MAX_STREAMS 2
#define SENSORS_PREVIEW 0
#define SENSORS_VIDEO 1

#define MAX_CALLBACK_REGISTER 10
static pthread_once_t cbRegister_init = PTHREAD_ONCE_INIT;
typedef struct _callbackPtr{
  void *ptr;
  int isValid;
}callbackPtr;

typedef struct _callbackRegister{
  callbackPtr callbackPtrArr[MAX_CALLBACK_REGISTER];
  pthread_mutex_t cbMutex;
}callbackRegister;

callbackRegister gCBRegister;
/* Default gravity vector report period is at 30fps*/
#define GRAVITY_VECTOR_DEF_REPORT_PERIOD (0.033 * (1 << 16))
#define GYRO_SAMPLE_RATE_AF 33
#define DEFAULT_GYRO_SAMPLE_RATE_EIS 200
#define GYRO_SAMPLE_RATE_DG 1600
#define LIGHT_SENSOR_REPORT_RATE 25
#define MIN_SAMPLE_LEN_THRESHOLD 1

typedef enum {
  ACCELEROMETER_TYPE        = (0x1 << 0),
  GYRO_TYPE                 = (0x1 << 1),
  MAGNETOMETER_TYPE         = (0x1 << 2),
  GRAVITY_TYPE              = (0x1 << 3),
  LINEAR_ACCELERATION_TYPE  = (0x1 << 4),
  TIME_OF_FLIGHT_TYPE       = (0x1 << 5),
  LIGHT_TYPE                = (0x1 << 6)
} gyro_port_sensor_type;

typedef enum {
  GYRO_PORT_STATE_CREATED = 1,
  GYRO_PORT_STATE_RESERVED,
  GYRO_PORT_STATE_LINKED,
  GYRO_PORT_STATE_UNLINKED,
  GYRO_PORT_STATE_UNRESERVED
} gyro_port_state_t;


/** _gyro_port_private_t:
 *    @reserved_id: session id | stream id
 *    @state: state of gyro port
 *    @sof: start of frame timestamp in microseconds.
 *    @frame_time: time when the last line of the sensor is read in
 *                 microseconds.  Does not include vertical blanking time
 *    @last_time: end time of the last gyro sample interval in microseconds
 *    @exposure_time: exposure time in seconds
 *    @gyro_stats: MCT_EVENT_MODULE_STATS_GYRO_STATS event body
 *    @dsps_handle: DSPS handle for access to gyro data
 *    @rs_interval_offset: maximum rolling shutter gyro interval offset
 *    @s3d_interval_offset: maximum shake 3D gyro interval offset
 *    @gyro_sample_rate: gyro sample rate in Hz
 *    @gyro_sample_rate_eis: gyro sample rate for EIS
 *    @is_type[]: IS type (DIS, EIS 2.0, etc.)
 *    @and_sens_handle: Android native sensor interface handle
 *    @sensor_list: List all availables sensors asigning one bit per sensor; type: gyro_port_sensor_type
 *    @dual_cam_sensor_info: To know if we are in Main Camera Mode or Auxillary
 *    @intra_peer_id: Intra peer Id
 *
 *  This structure defines gyro port's private variables
 **/
typedef struct _gyro_port_private {
  unsigned int reserved_id;
  gyro_port_state_t state;
  unsigned int frame_id[BUF_SIZE];
  unsigned long long sof[BUF_SIZE];
  unsigned int sof_event_identity;
  unsigned long long frame_time;
  unsigned long long last_time;
  float exposure_time[BUF_SIZE];
  mct_event_gyro_stats_t gyro_stats;
  void *dsps_handle;
  int64_t rs_interval_offset;
  int64_t s3d_interval_offset;
  uint32_t manual_gyro_sample_rate;
  uint32_t gyro_sample_rate;
  uint32_t max_gyro_sample_rate;
  uint32_t gyro_sample_rate_eis;
  cam_is_type_t is_type[SENSORS_MAX_STREAMS];
  void *and_sens_handle;
  unsigned int sensor_list;
  unsigned int dsps_sensor_list;
  unsigned int current_sof_id;
  boolean tof_intf_direct;
  enum camb_position_t sensor_position;
  void *laser_sns_hndl;
  callbackPtr *cbPtr;
  cam_sync_type_t      dual_cam_sensor_info;
  uint32_t             intra_peer_id;
  sensor_android_fmwk_mask sensor_intf_mask;
  void *imu_handle;
} gyro_port_private_t;

/** initCallbackRegisters:
 *
 * This function initalizes the mutex initalization of the call back registers
 **/
static void initCallbackRegisters(){
  IS_LOW("E");
  if(0 != pthread_mutex_init(&gCBRegister.cbMutex,NULL)) {
     IS_ERR("Failed to Initialize Callback Register");
  }
}

/** initalizeCallbackRegisters:
 *
 * This function can be calledn any no of times by any threads but
 * it ensures that the initialization happens only once
  *  Returns 0 on success and -error code  on failure
 **/
static int initalizeCallbackRegisters(){
  return pthread_once(&cbRegister_init, initCallbackRegisters);
}

/** registerCallback:
 *    @ptr: gyro port or any call back address
 *    @registeredPtr: Wrapper address for the ptr passed
 *
 *  This function registers the ptr and ensures  that multiple calls will be taken cared  of
 *
 *  Returns 0 on success and -1 on failure
 **/
static int registerCallback(void *ptr, callbackPtr **registeredPtr) {
  int retVal = -1;
  int cnt = 0;
  boolean isLockAcquired = FALSE;
  do{
    if(0 != pthread_mutex_lock(&gCBRegister.cbMutex)) {
       IS_ERR("Failed to Lock Mutex for Callback Register");
      break;
    }
    isLockAcquired = TRUE;
    for(cnt = 0; cnt < MAX_CALLBACK_REGISTER; cnt++){
      if((gCBRegister.callbackPtrArr[cnt].isValid &&
          gCBRegister.callbackPtrArr[cnt].ptr == ptr)
          || !gCBRegister.callbackPtrArr[cnt].isValid){
        gCBRegister.callbackPtrArr[cnt].ptr = ptr;
        gCBRegister.callbackPtrArr[cnt].isValid = TRUE;
        *registeredPtr = &gCBRegister.callbackPtrArr[cnt];
        retVal = 0;
         IS_LOW("for 0x%p as 0x%p cnt=%d",ptr ,(void*)*registeredPtr,cnt);
        break;
      }
    }
  }while(0);
  if(isLockAcquired && 0 != pthread_mutex_unlock(&gCBRegister.cbMutex)){
     IS_ERR("Failed to Unlock Mutex for Callback Register");
  }
  return retVal;
}

/** deRegisterCallback:
 *    @ptr: gyro port or any call back address
 *    @registeredPtr: Wrapper address for the ptr passed if we have the address
 *
 *  This function deregisters the ptr from the call back registry
 *
 *  Returns 0 on success and -1 on failure
 **/
static int deRegisterCallback(void *ptr, callbackPtr *registeredPtr){
  int retVal = -1;
  int cnt =0;
  boolean isLockAcquired = FALSE;

  do{
    if(0 != pthread_mutex_lock(&gCBRegister.cbMutex)) {
       IS_ERR("Failed to Lock Mutex for Callback Register");
      break;
    }
    isLockAcquired = TRUE;
    if(registeredPtr){
      registeredPtr->isValid = FALSE;
      registeredPtr->ptr = NULL;
      retVal =0;
    }else{
      for(cnt = 0; cnt < MAX_CALLBACK_REGISTER; cnt++){
        if(gCBRegister.callbackPtrArr[cnt].ptr == ptr){
          gCBRegister.callbackPtrArr[cnt].ptr = NULL;
          gCBRegister.callbackPtrArr[cnt].isValid = FALSE;
          retVal =0;
          break;
        }
      }
    }
  }while(0);
  if(isLockAcquired && 0 != pthread_mutex_unlock(&gCBRegister.cbMutex)){
     IS_ERR("Failed to Unlock Mutex for Callback Register");
  }
  return retVal;
}

/** checkIfCallbackIsValidAndLock:
 *    @registeredPtr: Wrapper address for the ptr passed if we have the address
 *
 *  This check if the callback is a valid registered address
 * Since its a memory address check the cost of calling this is
 * mutex lock+ address access+mutex unlock
 *
 *  Returns 0 on success and -1 on failure
 **/
static int checkIfCallbackIsValidAndLock(callbackPtr *registeredPtr) {
  int retVal = -1;
  boolean isLockAcquired = FALSE;

  do{
    if(0 != pthread_mutex_lock(&gCBRegister.cbMutex)) {
       IS_ERR("Failed to Lock Mutex for Callback Register");
      break;
    }

    isLockAcquired = TRUE;
    if(registeredPtr && registeredPtr->isValid){
      retVal = 0;
    }else{
      retVal = -1;
    }
  }while(0);
  if(isLockAcquired && (retVal != 0) && 0 != pthread_mutex_unlock(&gCBRegister.cbMutex)){
     IS_ERR("Failed to Unlock Mutex for Callback Register");
  }
  return retVal;
}

/** UnLockCallbackRegister:
 *
 *  This unlocks the mutex for the call back Register
 *
 *  Returns 0 on success and -1 on failure
 **/
static int UnLockCallbackRegister() {
  int retVal = 0;

  if(0 != pthread_mutex_unlock(&gCBRegister.cbMutex)){
     IS_ERR("Failed to Unlock Mutex for Callback Register");
    retVal = -1;
  }
  IS_LOW("Unlocking and retVal=%d",retVal);
  return retVal;
}

static void gyro_port_handle_light_data(
  mct_port_t *port,
  sensor_data_light_type *sensor_data,
  dsps_cb_data_light_t *dsps_data)
{
  mct_event_t stats_event;
  mct_event_light_sensor_update_t data;
  gyro_port_private_t *private =
    ((mct_port_t *)port)->port_private;

  STATS_MEMSET(&data, 0, sizeof(mct_event_light_sensor_update_t));
  if(NULL != sensor_data) {
    data.lux = sensor_data->lux;
  } else if(NULL != dsps_data) {
    data.lux = dsps_data->lux;
  }

  IS_LOW("Light lux :%f", data.lux);

  stats_event.type = MCT_EVENT_MODULE_EVENT;
  stats_event.identity = private->reserved_id;
  stats_event.direction = MCT_EVENT_UPSTREAM;
  stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_LIGHT_UPDATE;
  stats_event.u.module_event.module_event_data = &data;
  mct_port_send_event_to_peer(port, &stats_event);

}

static void gyro_port_handle_laser_data(
  mct_port_t *port,
  sensor_data_laser_type *input)
{
  tof_update_t data;
  mct_event_t stats_event;
  gyro_port_private_t *private =
    ((mct_port_t *)port)->port_private;

  IS_LOW("Input:\n frame-id: %d\n timestamp: %lld\n "
    "version: %f \ntype: %f \n distance: %f mm\n confidence: %f\n"
    "near_limit: %f\n far_limit: %f max_range = %d\n",
    private->current_sof_id,
    input->timestamp, input->version, input->type, input->distance,
    input->confidence, input->near_limit, input->far_limit,
    input->max_range);

  STATS_MEMSET(&data, 0, sizeof(tof_update_t));
  /* Fill laser data structure */
  data.frame_id = private->current_sof_id;
  data.timestamp = input->timestamp;
  data.distance =  input->distance;
  data.confidence =  input->confidence;
  data.near_limit =  input->near_limit;
  data.far_limit =  input->far_limit;
  data.max_distance = input->max_range;

  stats_event.type = MCT_EVENT_MODULE_EVENT;
  stats_event.identity = private->reserved_id;
  stats_event.direction = MCT_EVENT_UPSTREAM;
  stats_event.u.module_event.type = MCT_EVENT_MODULE_TOF_UPDATE;
  stats_event.u.module_event.module_event_data = &data;
  mct_port_send_event_to_peer(port, &stats_event);
}

/** sensor_laser_event_callback
 *    @port: gyro port
 *
 *  Callback received from laser sensor module.
 **/
static void sensor_laser_event_callback(
  void *cb_obj,
  sensor_laser_data_cb_t cb_event_data)
{
  callbackPtr *cbPtr = cb_obj;

  IS_HIGH("Received Laser Sensor Callback!");

  if(-1 == checkIfCallbackIsValidAndLock(cb_obj)){
   IS_HIGH("Laser Callback coming in for not registered CB 0x%p", cb_obj);
    return;
  }

  gyro_port_handle_laser_data((mct_port_t *)cbPtr->ptr, &cb_event_data.data);

  UnLockCallbackRegister();
}

/** gyro_port_use_dsps_iface:
 *
 *  Returns true if gyro port is configured to use Sensor1 API
 **/
inline static boolean gyro_port_use_dsps_iface(unsigned int mask)
{
  boolean rc = TRUE, use_android_intf = FALSE;

  use_android_intf = (SENSOR_ANDROID_FMWK_ENABLE_ALL == mask) ?
    TRUE : FALSE;


  /*Use android interface as default as per make file*/
#ifndef FEATURE_GYRO_DSPS
    rc = FALSE;
    IS_HIGH("Native sensor interface is default");
#else
    if(use_android_intf){
      IS_HIGH("Only use android sensor interface!");
      rc = FALSE;
    } else {
      IS_HIGH("Use DSPS interface as it supports");
      rc = TRUE;
    }
#endif
  return rc;
}

inline static boolean gyro_port_use_imu_iface()
{
  boolean rc = TRUE;

  /*Use android interface as default as per make file*/
#ifndef FEATURE_GYRO_IMU
    rc = FALSE;
    IS_HIGH("Native sensor interface is default");
#else
    IS_HIGH("Use IMU interface");
    rc = TRUE;
#endif
  return rc;
}

/** gyro_port_handle_sof_event:
 *    @port: gyro port
 *    @event: event object
 *    @private: gyro port private variables
 *
 * This function handles the SOF event.
 **/
static void gyro_port_handle_sof_event(mct_event_t *event, gyro_port_private_t *private)
{
  if(!(private->dsps_sensor_list & GYRO_TYPE)) {
    IS_LOW("Gyro not present. No futher processing");
    return;
  }
  mct_bus_msg_isp_sof_t *sof_event =
    (mct_bus_msg_isp_sof_t *)event->u.module_event.module_event_data;
  unsigned long long sof, eof;
  unsigned long long t_start; /* Start Time (microsec) */
  unsigned long long t_end;   /* End Time (microsec) */
  struct timeval tv;
  int i;

  if (gettimeofday(&tv, NULL) == 0) {
    IS_LOW("time = %llu, fid = %u, eid = %u",
      (((int64_t)tv.tv_sec * 1000000) + tv.tv_usec), sof_event->frame_id,
      event->identity);
  }

  i = sof_event->frame_id % BUF_SIZE;
  private->frame_id[i] = sof_event->frame_id;
  private->sof_event_identity = event->identity;
  private->current_sof_id = sof_event->frame_id;
  /* Calculate time interval */
  sof = (unsigned long long)sof_event->timestamp.tv_sec * USEC_PER_SEC
    + (unsigned long long)sof_event->timestamp.tv_usec;
  eof = sof + private->frame_time;

  /* In dual camera LPM case, camera may get suspended and then resumed at a much later time.  In such a case, the
   * time delta between current SOF and last_time can be huge.  Ensure that we don't ask for stale gyro data by
   * capping the starting interval time to be in the vicinity of the current SOF.  A starting interval time that
   * is too much in the past will anyway get rejected by sensor1. */
  if (sof > private->last_time + 4 * private->frame_time) {
    IS_HIGH("Detect %llu ms frame arrival gap, reset last_time to 0", (sof - private->last_time) / 1000);
    private->last_time = 0;
  }

  if (private->is_type[SENSORS_VIDEO] == IS_TYPE_EIS_DG) {
    t_start = sof;
    t_end = t_start + private->frame_time;
  } else {
  t_start = private->last_time ? private->last_time : sof - 15000;
  /* Below is the gyro interval request end time for EIS 2.0 */
  t_end  = MAX(eof + private->rs_interval_offset,
    (sof + eof) / 2 + private->s3d_interval_offset);
  }
  private->last_time = t_end;
  private->sof[i] = sof;
  IS_LOW("sof = %llu, frame_time = %llu, exp/2 = %f",
    sof, private->frame_time, (double)private->exposure_time[i] * USEC_PER_SEC / 2);

  IS_LOW("Post for gyro interval %llu, %llu, %llu",
    t_start, t_end, t_end - t_start);

  /* Request gyro samples from t_start to t_end */
  if (private->dsps_handle) {
    dsps_set_data_t dsps_set;
    dsps_set.msg_type = DSPS_GET_REPORT;
    dsps_set.sensor_type = DSPS_DATA_TYPE_GYRO;
    dsps_set.u.gyro.data.id = sof_event->frame_id;
    dsps_set.u.gyro.data.t_start = t_start;
    dsps_set.u.gyro.data.t_end = t_end;
    dsps_proc_set_params(private->dsps_handle, &dsps_set);
  } else if (private->and_sens_handle &&
    (private->sensor_list & GYRO_TYPE)) {
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_SET_DATA_CONFIG;
    sensor_set.sensor = SENSOR_TYPE_GYRO;
    sensor_set.u.data_range.id = sof_event->frame_id;
    sensor_set.u.data_range.t_start = t_start;
    sensor_set.u.data_range.t_end = t_end;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }
#ifdef FEATURE_GYRO_IMU
   else if (private->imu_handle != NULL) {
    stats_request_type request;
    request.frame_id = sof_event->frame_id;
    request.t_start = t_start;
    request.t_end = t_end;
    imu_driver_request(private->imu_handle, &request);
  }
#endif
}

/** dsps_port_callback_gyro
 *    @port: gyro port
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives gyro samples from sensors.
 **/
static void dsps_port_callback_gyro(void *port, dsps_cb_data_gyro_t *input)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  unsigned int i;

  i = input->seq_no % BUF_SIZE;

  if ((private->frame_id[i] & 0x0FF) == input->seq_no) {
    mct_event_gyro_stats_t gyro_stats;
    mct_event_t gyro_stats_event;
    unsigned int j;

    gyro_stats.is_gyro_data.sof = private->sof[i];
    gyro_stats.is_gyro_data.frame_time = private->frame_time;
    gyro_stats.is_gyro_data.exposure_time = private->exposure_time[i];

    gyro_stats.is_gyro_data.frame_id = private->frame_id[i];

    /* Verify that DSPS API struct sizes match size used by gyro port */
    if (STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE < input->sample_len) {
      IS_ERR("error: sample_len mismatch");
      return;
    }
    gyro_stats.is_gyro_data.sample_len = input->sample_len;
    for (j = 0; j < input->sample_len; j++) {
      gyro_stats.is_gyro_data.sample[j] = input->sample[j];
       IS_LOW("fid = %u, gyro sample[%d]: %llu, %d, %d, %d", gyro_stats.is_gyro_data.frame_id, j,
        gyro_stats.is_gyro_data.sample[j].timestamp,
        gyro_stats.is_gyro_data.sample[j].value[0],
        gyro_stats.is_gyro_data.sample[j].value[1],
        gyro_stats.is_gyro_data.sample[j].value[2]);
    }

    if (input->sample_len > MIN_SAMPLE_LEN_THRESHOLD) {
      gyro_stats.q16_angle[0] = input->sample[1].value[0];
      gyro_stats.q16_angle[1] = input->sample[1].value[1];
      gyro_stats.q16_angle[2] = input->sample[1].value[2];
      gyro_stats.ts = input->sample[1].timestamp;
    }

    gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
    gyro_stats_event.identity = private->sof_event_identity;
    gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
    gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS;
    gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
    mct_port_send_event_to_peer(port, &gyro_stats_event);
  } else {
     IS_ERR("gyro_port queue overflow");
  }
}

/** dsps_port_callback_gravity
 *    @port: gyro port
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives gravity samples from sensors.
 **/
static void dsps_port_callback_gravity(void *port, dsps_cb_data_gravity_t *input)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  mct_event_gravity_vector_update_t data;
  mct_event_t stats_event;


   IS_LOW("Input data: Gravity (%f, %f, %f) Lin_Accel (%f, %f, %f)"
    "Accuracy: %d", input->gravity[0], input->gravity[1],
    input->gravity[2], input->lin_accel[0], input->lin_accel[1],
    input->lin_accel[2], input->accuracy);

  STATS_MEMCPY(data.gravity, sizeof(data.gravity),
    input->gravity, sizeof(input->gravity));
  STATS_MEMCPY(data.lin_accel, sizeof(data.lin_accel),
    input->lin_accel, sizeof(input->lin_accel));
  data.accuracy = input->accuracy;

  stats_event.type = MCT_EVENT_MODULE_EVENT;
  stats_event.identity = private->sof_event_identity;
  stats_event.direction = MCT_EVENT_UPSTREAM;
  stats_event.u.module_event.type = MCT_EVENT_MODULE_GRAVITY_VECTOR_UPDATE;
  stats_event.u.module_event.module_event_data = &data;
  mct_port_send_event_to_peer(port, &stats_event);
}

/** dsps_port_callback
 *    @port: dsps port
 *    @cb_data: callback data
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives sensor data from motion sensors.
 **/
static void dsps_port_callback(void *port, dsps_cb_data_t *cb_data)
{
  if (-1 == checkIfCallbackIsValidAndLock(port)) {
    IS_HIGH("DSPS Callback coming in for not registered CB 0x%p", port);
    return;
  }

  callbackPtr *cbPtr = port;
  void *gyroPort = cbPtr ->ptr;

  switch (cb_data->cb_type) {

  case DSPS_SENSOR_DATA: {
    switch (cb_data->type) {
    case DSPS_DATA_TYPE_GYRO: {
      dsps_port_callback_gyro(gyroPort, &cb_data->u.gyro);
    }
      break;
    case DSPS_DATA_TYPE_GRAVITY_VECTOR: {
      dsps_port_callback_gravity(gyroPort, &cb_data->u.gravity);
    }
      break;
    case DSPS_DATA_TYPE_LIGHT: {
      gyro_port_handle_light_data(gyroPort, NULL, &cb_data->u.light);
    }
      break;
    default:
      IS_ERR("DSPS Callback - data type (%d) not handled!",
        cb_data->type);
      break;
    }
    break;
  }
  case DSPS_SENSOR_LIST: {
    int i = 0;
    gyro_port_private_t *private = ((mct_port_t *)gyroPort)->port_private;
    private->dsps_sensor_list = 0;
    dsps_data_type *sensor = (dsps_data_type *)&cb_data->u.sensor_list.available_sensors;
    for (i = 0; i < cb_data->u.sensor_list.num_sensors; i++) {
      if (sensor[i] == DSPS_DATA_TYPE_ACCELEROMETER) {
        private->dsps_sensor_list |= ACCELEROMETER_TYPE;
        IS_HIGH("Available: ACCELEROMETER_TYPE");
      } else if (sensor[i] == DSPS_DATA_TYPE_GYRO) {
        private->dsps_sensor_list |= GYRO_TYPE;
        IS_HIGH("Available: GYRO_TYPE");
      } else if (sensor[i] == DSPS_DATA_TYPE_GRAVITY_VECTOR) {
        private->dsps_sensor_list |= GRAVITY_TYPE;
        IS_HIGH("Available: GRAVITY_TYPE");
      } else if (sensor[i] == DSPS_DATA_TYPE_LIGHT) {
        private->dsps_sensor_list |= LIGHT_TYPE;
        IS_HIGH("Available: LIGHT_TYPE");
      } else {
        IS_LOW("Sensor not needed");
      }
    }
    IS_LOW("Sensor list, num of sensors: %d, sensors: 0x%x",
      cb_data->u.sensor_list.num_sensors,
      private->dsps_sensor_list);
    break;
    }
  case DSPS_SENSOR_INFO: {
    gyro_port_private_t *private = ((mct_port_t *)gyroPort)->port_private;
    switch (cb_data->type) {
    case DSPS_DATA_TYPE_GYRO: {
      private->max_gyro_sample_rate = cb_data->u.sensor_info.max_sample_rate;
      IS_LOW("Max gyro rate: %d", private->max_gyro_sample_rate);
      break;
    }
    default:
      break;
    }
    break;
  }
    }
  UnLockCallbackRegister();
}

/** sensor_event_cb_sensor_data
 *    @port: gyro port
 *    @sensor_data: Information with the type of data and data itself
 *
 *  This is a helper to the sensor_event_callback to hanlde sensor data callback
 **/
static void sensor_event_cb_sensor_data(void *port,
  sensor_callback_data_type* sensor_data)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  switch (sensor_data->type) {
    case SENSOR_TYPE_GYRO: {
      mct_event_t gyro_stats_event;
      mct_event_gyro_stats_t gyro_stats;
      sensor_gyro_buffer_data_type *gyro_data = NULL;
      unsigned int i, j;
      uint8_t frame_id;
      gyro_data = &sensor_data->u.gyro;
      frame_id = gyro_data->seqnum;
      i = frame_id % BUF_SIZE;
      IS_LOW("Received gyro samples");
      if((private->frame_id[i] & 0x0FF) == frame_id) {
        gyro_stats.is_gyro_data.sof = private->sof[i];
        gyro_stats.is_gyro_data.frame_time = private->frame_time;
        gyro_stats.is_gyro_data.exposure_time = private->exposure_time[i];

        gyro_stats.is_gyro_data.frame_id = private->frame_id[i];

        if (STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE < gyro_data->sample_len) {
          IS_ERR("error: size mismatch");
          return;
        }
        gyro_stats.is_gyro_data.sample_len = gyro_data->sample_len;
        /* Fill data used mainly by IS */
        for (j = 0; j < gyro_data->sample_len; j++) {
          gyro_stats.is_gyro_data.sample[j] = gyro_data->sample[j];
          IS_LOW("gyro sample[%d], %llu, %d, %d, %d", j,
          gyro_stats.is_gyro_data.sample[j].timestamp,
          gyro_stats.is_gyro_data.sample[j].value[0],
          gyro_stats.is_gyro_data.sample[j].value[1],
          gyro_stats.is_gyro_data.sample[j].value[2]);
        }
        /* Fill data used mainly by AF */
        if (gyro_data->sample_len > MIN_SAMPLE_LEN_THRESHOLD) {
          gyro_stats.q16_angle[0] = gyro_data->sample[1].value[0];
          gyro_stats.q16_angle[1] = gyro_data->sample[1].value[1];
          gyro_stats.q16_angle[2] = gyro_data->sample[1].value[2];
          gyro_stats.ts = gyro_data->sample[1].timestamp;
        }
        gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
        gyro_stats_event.identity = private->reserved_id ;
        gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
        gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS;
        gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
        mct_port_send_event_to_peer(port, &gyro_stats_event);
      } else {
         IS_ERR("gyro_port queue overflow");
      }
      break;
    }
    case SENSOR_TYPE_GRAVITY: {
      mct_event_gravity_vector_update_t data;
      mct_event_t stats_event;
      IS_LOW("Input data: Gravity (%f, %f, %f)",
        sensor_data->u.gravity.x,
        sensor_data->u.gravity.y,
        sensor_data->u.gravity.z);

      STATS_MEMSET(&data, 0, sizeof(mct_event_gravity_vector_update_t));
     /* As algo is tuned with that of DSPS data; change the axes in
      * accordance  with that of DSPS. x<-->Y  ;  Z -> -Z */
      data.gravity[0] = sensor_data->u.gravity.y;
      data.gravity[1] = sensor_data->u.gravity.x;
      data.gravity[2] = -sensor_data->u.gravity.z;

      stats_event.type = MCT_EVENT_MODULE_EVENT;
      stats_event.identity = private->sof_event_identity;
      stats_event.direction = MCT_EVENT_UPSTREAM;
      stats_event.u.module_event.type = MCT_EVENT_MODULE_GRAVITY_VECTOR_UPDATE;
      stats_event.u.module_event.module_event_data = &data;
      mct_port_send_event_to_peer(port, &stats_event);
    }
      break;
    case SENSOR_TYPE_ACCELEROMETER: {
      /* TODO: Need to create an appropraite event for accelerometer
       * based on requirement*/
    }
      break;
    case SENSOR_TYPE_LINEAR_ACCELERATION: {
      /* TODO: Need to handle linear accelerometer based on requirement */
    }
      break;
    case SENSOR_TYPE_TIME_OF_FLIGHT: {
      IS_HIGH("Received Laser Sensor Data!");
      gyro_port_handle_laser_data((mct_port_t *)port, &sensor_data->u.laser);
    }
      break;
    case SENSOR_TYPE_LIGHT: {
      IS_INFO("Received Light Sensor Data!");
      gyro_port_handle_light_data((mct_port_t *)port, &sensor_data->u.light, NULL);
    }
      break;
    default:
      IS_ERR("Sensor Callback - data type (%d) not handled!", sensor_data->type);
      break;
  }
}

/** sensor_event_callback
 *    @port: gyro port
 *    @type: Sensor type like gyroscope or Accelerometer
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the Android sensor interface layer.
 *  It gets called when android interface layer receives sensor samples from sensors.
 **/
static void sensor_event_callback(
  void *port,
  sensor_cb_struct_type cb_event_data)
{
  void *gyroPort = NULL;
  gyro_port_private_t *private = NULL;
  callbackPtr *cbPtr = port;

  if(-1 == checkIfCallbackIsValidAndLock(port)){
   IS_HIGH("DSPS Callback coming in for not registered CB 0x%p",port);
    return;
  }

  gyroPort = cbPtr ->ptr;

  private = ((mct_port_t *)gyroPort)->port_private;
  switch (cb_event_data.type) {
  case SENSOR_LIST: {
    int i = 0;
    private->sensor_list = 0;
    sensor_type *sensor = (sensor_type *)&cb_event_data.u.sensor_list.available_sensors;
    for (i = 0; i < cb_event_data.u.sensor_list.num_of_sensors; i++) {
      if (sensor[i] == SENSOR_TYPE_ACCELEROMETER) {
        private->sensor_list |= ACCELEROMETER_TYPE;
        IS_HIGH("Available: ACCELEROMETER_TYPE");
      } else if (sensor[i] == SENSOR_TYPE_GYRO) {
        private->sensor_list |= GYRO_TYPE;
        IS_HIGH("Available: GYRO_TYPE");
      } else if (sensor[i] == SENSOR_TYPE_GRAVITY) {
        private->sensor_list |= GRAVITY_TYPE;
        IS_HIGH("Available: GRAVITY_TYPE");
      } else if (sensor[i] == SENSOR_TYPE_LINEAR_ACCELERATION) {
        private->sensor_list |= LINEAR_ACCELERATION_TYPE;
        IS_HIGH("Available: LINEAR_ACCELERATION_TYPE");
      } else if (sensor[i] == SENSOR_TYPE_TIME_OF_FLIGHT) {
        private->sensor_list |= TIME_OF_FLIGHT_TYPE;
        IS_HIGH("Available: TIME_OF_FLIGHT_TYPE");
      } else if (sensor[i] == SENSOR_TYPE_LIGHT) {
        private->sensor_list |= LIGHT_TYPE;
        IS_HIGH("Available: LIGHT_TYPE");
      } else {
        IS_LOW("Sensor not needed");
      }
    }
    IS_LOW("Sensor list, num of sensors: %d, sensors: 0x%x",
      cb_event_data.u.sensor_list.num_of_sensors,
      private->sensor_list);
  }
    break;
  case SENSOR_DATA: {
    sensor_event_cb_sensor_data(gyroPort, &cb_event_data.u.sensor_data);
  }
    break;
  default:
    IS_ERR("Not valid cb type");
    break;
  }
  UnLockCallbackRegister();
}

#ifdef FEATURE_GYRO_IMU
static void imu_event_callback(
  void *port,
  imu_cb_struct_type *cb_event_data)
{
  callbackPtr *cbPtr = port;
  void *gyroPort = cbPtr ->ptr;;
  gyro_port_private_t *private = ((mct_port_t *)gyroPort)->port_private;
  unsigned int i, j, k;

  if (cb_event_data->type == TYPE_START) {
    private->dsps_sensor_list |= GYRO_TYPE;
  }

  if (cb_event_data->type == TYPE_DATA) {
    i = cb_event_data->frame_id % BUF_SIZE;
    if (private->frame_id[i] == cb_event_data->frame_id) {
      mct_event_imu_stats_t imu_stats;
      mct_event_t gyro_stats_event;
      unsigned int j;

      imu_stats.sof = private->sof[i];
      imu_stats.frame_time = private->frame_time;
      imu_stats.exposure_time = private->exposure_time[i];

      imu_stats.frame_id = private->frame_id[i];

      /* Verify that IMU API struct sizes match size used by gyro port */
      if (STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE < cb_event_data->sample_len) {
        IS_ERR("error: sample_len mismatch");
        return;
      }
      imu_stats.sample_len = cb_event_data->sample_len;
      for (j = 0; j < cb_event_data->sample_len; j++) {
        imu_stats.sample[j].timestamp = cb_event_data->samples[j].timestamp;
        for (k = 0; k < 9; k++) {
          imu_stats.sample[j].rotation_matrix[k] =
            cb_event_data->samples[j].sample[k];
        }
      }

      gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
      gyro_stats_event.identity = private->sof_event_identity;
      gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
      gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_IMU_STATS;
      gyro_stats_event.u.module_event.module_event_data = &imu_stats;
      mct_port_send_event_to_peer(gyroPort, &gyro_stats_event);
    } else {
       IS_ERR("gyro_port queue overflow");
    }
  }
}
#endif

/** gyro_port_init_sensors:
 *    @port: gyro port
 *
 * Initialize motion sensor framework.
 *
 *  Returns TRUE on success.
 **/
boolean gyro_port_init_sensors(mct_port_t *port)
{
  gyro_port_private_t *private = (gyro_port_private_t *)port->port_private;
  boolean rc = TRUE;

  /* Init sensor interface */
  if (gyro_port_use_dsps_iface(private->sensor_intf_mask)){
    IS_HIGH("Init DSPS intf");
    private->dsps_handle = dsps_proc_init(private->cbPtr, dsps_port_callback);
    if (NULL != private->dsps_handle) {
      rc = TRUE;
    } else {
      rc = FALSE;
      IS_ERR("DSPS fail to init");
    }
  }
#ifdef FEATURE_GYRO_IMU
  LOGE("Try to initialize IMU");
  /* Init IMU interface */
  if(gyro_port_use_imu_iface()) {
    private->imu_handle = imu_driver_init(private->cbPtr, imu_event_callback);
    if(private->imu_handle == NULL) {
      IS_ERR("Error initializing IMU sensor");
      rc = FALSE;
    } else {
      rc = TRUE;
    }
  }
#else
  private->imu_handle = NULL;
#endif

  /* Try using Android Sensor Interface if
     - dsps interface isn't enabled, or
     - IMU interface isn't enabled, or
     - android sensor interface is enabled through setprop
   */
  if ((SENSOR_ANDROID_FMWK_DISABLE != private->sensor_intf_mask) ||
    ((NULL == private->dsps_handle) && !gyro_port_use_imu_iface())) {
    /* DSPS not available and IMU is not available, try using Android sensor interface */
    /* Initialization done here to make sure sensors start before requesting
           to enable at dsps_port_enable_disable_request() */
    IS_HIGH("Init Android sensor intf");
    rc = sensor_intf_init(&private->and_sens_handle, private->cbPtr,
      sensor_event_callback);
    if ((private->and_sens_handle == NULL) || (SENSOR_RET_SUCCESS != rc)) {
      IS_ERR("Error initializing Native Sensor!");
      private->and_sens_handle = NULL;
      rc = FALSE;
    } else {
      rc = TRUE;
    }
  }

  return rc;
}

/** gyro_port_deinit_sensors
 *    @private: gyro port private struct
 *
 *  Deinit sensors modules (DSPS and Android sensors API)
 **/
static void gyro_port_deinit_sensors(gyro_port_private_t *private)
{
  /*deinit Android sensorService here */
  if (NULL != private->and_sens_handle) {
    sensor_intf_deinit(private->and_sens_handle);
    IS_LOW("Deinited Android sensor_Service");
    private->and_sens_handle = NULL;
  } else {
    IS_HIGH("No android native handler");
  }
  /* Deinit DSPS if not already deinit during stream off*/
  if(NULL != private->dsps_handle){
    dsps_proc_deinit(private->dsps_handle);
    private->dsps_handle = NULL;
    IS_HIGH("DSPS intf deinit in Gyro port deinit");
  }

  /* deinitialize the laser sensor */
  if (NULL != private->laser_sns_hndl) {
    IS_HIGH("Deinit Laser Sensor Module!");
    sensor_laser_deinit(private->laser_sns_hndl);
    private->laser_sns_hndl = NULL;
  }

#ifdef FEATURE_GYRO_IMU
  if (NULL != private->imu_handle) {
    imu_driver_deinit(private->imu_handle);
    private->imu_handle = NULL;
  }
#endif
}

/** dsps_port_handle_set_is_enable
 *    @port: gyro port
 *    @is_enable: IS enable indication
 *
 * This function adjusts the gyro sampling rate based on use case to save
 * power.  When gyro-based IS is not active, gyro sampling rate is set low.
 * When gyro-based IS is active, the gyro sampling rate is set high.  Changing
 * the sampling rate requires a restart of gyro services.  General worse case
 * delay may be up to 40 ms.  However, this function executes during camera to
 * camcorder mode transitions (vice versa) and during the enabling/disabling of
 * IS from the camcorder menu which results in a restarting of the camera
 * pipeline.  In these contexts, the delay should not be too noticeable.
 *
 * If gyro restart to change sampling rate fails, leave gravity vector running.
**/
int dsps_port_handle_set_is_enable(gyro_port_private_t *private,
  int32_t is_enable)
{
  int rc = 0;
  uint32_t new_gyro_sample_rate = 0;

  if (private->dsps_handle == NULL) {
    /* DSPS services failed earlier at init or enable, return "no error" */
    return rc;
  }

  if(!(private->dsps_sensor_list & GYRO_TYPE)) {
    IS_LOW("No need to handle the is_enable event as no gyro is present.");
    return rc;
  }

  if (is_enable) {
    /* Change gyro sample rate only for IS technologies that require gyro */
    if ((private->gyro_sample_rate == GYRO_SAMPLE_RATE_AF ||
          private->gyro_sample_rate == private->manual_gyro_sample_rate) &&
        ((private->is_type[SENSORS_PREVIEW] == IS_TYPE_EIS_2_0 ||
          private->is_type[SENSORS_PREVIEW] == IS_TYPE_EIS_3_0) ||
         (private->is_type[SENSORS_VIDEO] == IS_TYPE_EIS_2_0 ||
          private->is_type[SENSORS_VIDEO] == IS_TYPE_EIS_3_0))) {
      new_gyro_sample_rate = private->gyro_sample_rate_eis;
    }

    if(private->is_type[SENSORS_VIDEO] == IS_TYPE_EIS_DG) {
      new_gyro_sample_rate = GYRO_SAMPLE_RATE_DG;
    } else if(private->is_type[SENSORS_VIDEO] == IS_TYPE_DIG_GIMB) {
      new_gyro_sample_rate = GYRO_SAMPLE_RATE_DG;
    }
  } else {
    if (private->gyro_sample_rate == private->gyro_sample_rate_eis ||
        private->gyro_sample_rate == GYRO_SAMPLE_RATE_DG) {
      if (private->manual_gyro_sample_rate == 0) {
        new_gyro_sample_rate = GYRO_SAMPLE_RATE_AF;
      } else {
        new_gyro_sample_rate = private->manual_gyro_sample_rate;
      }
    }
  }

  if (new_gyro_sample_rate != 0) {
    dsps_set_data_t dsps_set_data;

    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc == 0) {
      dsps_set_data.msg_type = DSPS_ENABLE_REQ;
      dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
      if (new_gyro_sample_rate <= private->max_gyro_sample_rate) {
        dsps_set_data.u.gyro.gyro_sample_rate = new_gyro_sample_rate;
      } else {
        IS_HIGH("Gyro Sample Rate set to Max. Allowed! Max rate = %d", private->max_gyro_sample_rate);
        dsps_set_data.u.gyro.gyro_sample_rate = private->max_gyro_sample_rate;
        new_gyro_sample_rate = private->max_gyro_sample_rate;
      }
      rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
      if (rc == 0) {
        private->gyro_sample_rate = new_gyro_sample_rate;
         IS_HIGH("Set gyro sample rate to %u Hz", new_gyro_sample_rate);
      } else {
         IS_ERR("Gyro enable req failed");
      }
    } else {
       IS_ERR("Gyro disable req failed");
    }
  }
  return rc;
}

/** gyro_port_enable_tof_direct_access
 *    @port: gyro port
 *
 *  Enable direct access of laser data
**/
static int gyro_port_enable_tof_direct_access(mct_port_t *port)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);
  int rc = 0;

  /* If direct laser interface to vendor is enabled, initialize laser sensor
     separately, otherwise it'll be initalized as part of android sensor
     interface */
  if(private->tof_intf_direct &&
   (NULL == private->laser_sns_hndl)) {
  /* Note: Check if this is back camera. Currently we only support a single
       back sensor for laser AF. For front we do not initialize laser sensor.*/
    IS_LOW("Sensor position: %d", private->sensor_position);
    if (BACK_CAMERA_B == private->sensor_position){
      sensor_laser_ret_type ret;
      IS_HIGH("Init Laser Sensor Interface!");
      ret = sensor_laser_init(&private->laser_sns_hndl, private->cbPtr,
        sensor_laser_event_callback);
      if (SENSOR_LASER_RET_SUCCESS != ret) {
        IS_ERR("Failure initializing laser sensor. Error: %d", ret);
        rc = FALSE;
      } else {
        rc = TRUE;
      }
    }
  }

  return rc;
}

/** gyro_port_enable_sensor_dsps_intf
 *    @port: gyro port
 *
 *  Enable sensors through dsps interface
**/
static void gyro_port_enable_sensor_dsps_intf(mct_port_t *port)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);
  dsps_set_data_t dsps_set_data;
  int rc = 0;

  /* Enable gyro if it's not enabled through android interface
       Check for the presence of gyro before enabling */
  if (!(SENSOR_ANDROID_FMWK_GYRO & private->sensor_intf_mask) &&
       (private->dsps_sensor_list & GYRO_TYPE)) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_ENABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
    dsps_set_data.u.gyro.gyro_sample_rate = private->gyro_sample_rate;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_ERR("Error enabling gyro!");
    } else {
      IS_HIGH("Set gyro sample rate to %u Hz",
        private->gyro_sample_rate);
    }
  } else {
    IS_HIGH("Gyro not Present")
  }

  /* Enable gravity vector. Check for its presence first. */
  if (!(SENSOR_ANDROID_FMWK_GRAVITY_VECTOR & private->sensor_intf_mask) &&
       (private->dsps_sensor_list & GRAVITY_TYPE)) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_ENABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    dsps_set_data.u.gravity.report_period =
      GRAVITY_VECTOR_DEF_REPORT_PERIOD;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_ERR("Error enabling gravity vector!");
    } else {
      IS_HIGH("Enable gravity vector with report period: %d",
        dsps_set_data.u.gravity.report_period);
    }
  } else {
    IS_HIGH("Gravity not present");
  }

  /* Enable light sensor. Check for its presence first. */
  if (!(SENSOR_ANDROID_FMWK_LIGHT & private->sensor_intf_mask) &&
       (private->dsps_sensor_list & LIGHT_TYPE)) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_ENABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_LIGHT;
    dsps_set_data.u.light.sample_rate = LIGHT_SENSOR_REPORT_RATE;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_ERR("Error enabling Light!");
    } else {
      IS_HIGH("Enable Light Sensor");
    }
  } else {
    IS_HIGH("Light not present");
  }
}

/** gyro_port_disable_sensor_dsps_intf
 *    @port: gyro port
 *
 *  Disable sensors through dsps interface
**/
static void gyro_port_disable_sensor_dsps_intf(mct_port_t *port)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);
  dsps_set_data_t dsps_set_data;
  int rc = 0;

  if (!(SENSOR_ANDROID_FMWK_GYRO & private->sensor_intf_mask) &&
       private->dsps_sensor_list & GYRO_TYPE) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_HIGH("Error disabling gyro!");
    } else {
      IS_LOW("Gyro sensor disabled!");
    }
  }

  if (!(SENSOR_ANDROID_FMWK_GRAVITY_VECTOR & private->sensor_intf_mask) &&
       private->dsps_sensor_list & GRAVITY_TYPE) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_HIGH("Error disabling gravity vector!");
    } else {
      IS_LOW("Gravity vector disabled!");
    }
  }

  if (!(SENSOR_ANDROID_FMWK_LIGHT & private->sensor_intf_mask) &&
       private->dsps_sensor_list & LIGHT_TYPE) {
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_LIGHT;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      IS_HIGH("Error disabling light sensor!");
    } else {
      IS_LOW("Light sensor disabled!");
    }
  }
}

/** gyro_port_enable_sensor_android_intf
 *    @port: gyro port
 *
 *  Enable sensors through Android sensor interface
**/
static void gyro_port_enable_sensor_android_intf(mct_port_t *port)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);
  int rc = 0;

  IS_HIGH("Enable Android API sensors. sensor_list: 0x%x",
    private->sensor_list);

  if (((SENSOR_ANDROID_FMWK_ENABLE_ALL & private->sensor_intf_mask) ||
    (SENSOR_ANDROID_FMWK_GYRO & private->sensor_intf_mask)) &&
    (private->sensor_list & GYRO_TYPE)) {
    /* Enable Gyro sensor */
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_ENABLE_REQ;
    sensor_set.sensor = SENSOR_TYPE_GYRO;
    sensor_set.u.sample_rate = private->gyro_sample_rate;
    rc = sensor_set_params(private->and_sens_handle, sensor_set);
    if (rc) {
      IS_ERR("Error enabling gyro!");
    } else {
      IS_HIGH("Set gyro sample rate to %u Hz",
        private->gyro_sample_rate);
    }
  }

  if (((SENSOR_ANDROID_FMWK_ENABLE_ALL & private->sensor_intf_mask) ||
    (SENSOR_ANDROID_FMWK_LIGHT & private->sensor_intf_mask)) &&
    (private->sensor_list & LIGHT_TYPE)) {
    /* Enable Light sensor */
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_ENABLE_REQ;
    sensor_set.sensor = SENSOR_TYPE_LIGHT;
    rc = sensor_set_params(private->and_sens_handle, sensor_set);
    if (rc) {
      IS_ERR("Error enabling Light!");
    } else {
      IS_HIGH("Set Light Sensor");
    }
  }

  if (((SENSOR_ANDROID_FMWK_ENABLE_ALL & private->sensor_intf_mask) ||
    (SENSOR_ANDROID_FMWK_GRAVITY_VECTOR & private->sensor_intf_mask)) &&
    (private->sensor_list & GRAVITY_TYPE)) {
    /* Enable Gravity sensor */
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_ENABLE_REQ;
    sensor_set.sensor = SENSOR_TYPE_GRAVITY;
    sensor_set.u.sample_rate = 0; /* min sample rate */
    rc = sensor_set_params(private->and_sens_handle, sensor_set);
    if (rc) {
      IS_ERR("Error enabling gravity vector!");
    } else {
      IS_HIGH("Set gravity sample rate to %u",
        sensor_set.u.sample_rate);
    }
  }

  /* Number of checks here:
     - Android sensor framework is enabled - for only laser or all sensors
     - Access of laser data is enabled through android interface instead
       of direct access
     - this is for back camera
     - laser sensor has been detected
  */
  if((FALSE == private->tof_intf_direct) &&
    (BACK_CAMERA_B == private->sensor_position) &&
    ((SENSOR_ANDROID_FMWK_ENABLE_ALL & private->sensor_intf_mask) ||
    (SENSOR_ANDROID_FMWK_LASER & private->sensor_intf_mask)) &&
    (private->sensor_list & TIME_OF_FLIGHT_TYPE)) {
    /* Enable tof sensor */
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_ENABLE_REQ;
    sensor_set.sensor = SENSOR_TYPE_TIME_OF_FLIGHT;
    sensor_set.u.sample_rate = 0; /* min sample rate */
    rc = sensor_set_params(private->and_sens_handle, sensor_set);
    if (rc) {
      IS_ERR("Error enabling TOF sensor!");
    } else {
      IS_HIGH("Set TOF sample rate to %u (minimum rate)",
        sensor_set.u.sample_rate);
    }
  }
}

/** gyro_port_disable_sensor_android_intf
 *    @port: gyro port
 *
 *  Disable sensors through Android sensor interface
**/
static void gyro_port_disable_sensor_android_intf(mct_port_t *port)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);

  IS_HIGH("Android sensor API Deinit");
  sensor_set_data_type sensor_set;
  sensor_set.msg_type = SENSOR_DISABLE_REQ;

  if (private->sensor_list & GYRO_TYPE) {
    sensor_set.sensor = SENSOR_TYPE_GYRO;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }

  if (private->sensor_list & GRAVITY_TYPE) {
    sensor_set.sensor = SENSOR_TYPE_GRAVITY;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }

  if (private->sensor_list & ACCELEROMETER_TYPE){
    sensor_set.sensor = SENSOR_TYPE_ACCELEROMETER;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }

  if (private->sensor_list & MAGNETOMETER_TYPE) {
    sensor_set.sensor = SENSOR_TYPE_MAGNETOMETER;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }

  if (private->sensor_list & LINEAR_ACCELERATION_TYPE) {
   sensor_set.sensor = SENSOR_TYPE_LINEAR_ACCELERATION;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }

  if (private->sensor_list & LIGHT_TYPE) {
    sensor_set.sensor = SENSOR_TYPE_LIGHT;
    sensor_set_params(private->and_sens_handle, sensor_set);
  }
}

/** gyro_port_enable_disable_sensor_request
 *    @port: gyro port
 *    @enable: indicates whether to enable or disable android sensor or DSPS services
 *
 *  At the start, gyro and gravity vector enablements must both both succeed.
 *  succeed.  Otherwise, DSPS or android sensors are deinited.
**/
void gyro_port_enable_disable_sensor_request(mct_port_t *port, boolean enable)
{
  gyro_port_private_t *private =
    (gyro_port_private_t *)(port->port_private);

  if (NULL != private->dsps_handle) {
    if (enable) {
      gyro_port_enable_sensor_dsps_intf(port);
    } else {
      gyro_port_disable_sensor_dsps_intf(port);
    }
  }

  if (NULL != private->and_sens_handle) {
    if (enable) {
      gyro_port_enable_sensor_android_intf(port);
    } else {
      gyro_port_disable_sensor_android_intf(port);
    }
  }

  /* Enable TOF direct access if required.
     It'll be disabled at deinit only (currently)*/
  if(enable) {
    gyro_port_enable_tof_direct_access(port);
  }
}

/** dsps_port_enable_gravity_vector:
 *    @port: dsps port
 *    @input: input parameter
 *
 *  This function enables/disables gravity vector.
 *
 *  Returns TRUE on success.
 **/
int dsps_port_enable_gravity_vector(void *port, void *input)
{
  int rc = 0;
  mct_event_gravity_vector_enable_t *param =
    (mct_event_gravity_vector_enable_t *)input;
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  dsps_set_data_t dsps_set_data;

  if (private->dsps_handle == NULL) {
     IS_ERR("DSPS not initialized!");
    return -1;
  }

  if (param->enable) { /* enable gravity vector */
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_ENABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    dsps_set_data.u.gravity.report_period = param->report_period;
    dsps_set_data.u.gravity.sample_rate_valid =
      param->sample_rate_valid;
    dsps_set_data.u.gravity.sample_rate = param->sample_rate;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
       IS_ERR("DSPS enable req failed");
    }
  } else { /* disable gravity vector */
    STATS_MEMSET(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
       IS_ERR("DSPS enable req failed");
    }
  }

  return rc;
} /* dsps_port_enable_gravity_vector */

/** gyro_port_event:
 *    @port: gyro port
 *    @event: event
 *
 *  This function handles events for the gyro port.
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_event(mct_port_t *port, mct_event_t *event)
{
  boolean rc = TRUE;
  gyro_port_private_t *private;

  /* sanity check */
  if (!port || !event)
    return FALSE;

  private = (gyro_port_private_t *)(port->port_private);
  if (!private)
    return FALSE;

  /* sanity check: ensure event is meant for port with same identity*/
  if ((private->reserved_id & 0xFFFF0000) != (event->identity & 0xFFFF0000))
  {
    return FALSE;
  }

  switch (MCT_EVENT_DIRECTION(event)) {
  case MCT_EVENT_DOWNSTREAM: {
    switch (event->type) {
    case MCT_EVENT_CONTROL_CMD: {
      mct_event_control_t *control = &event->u.ctrl_event;

      switch (control->type) {
      case MCT_EVENT_CONTROL_SOF:
        gyro_port_handle_sof_event(event, private);
        break;
      case MCT_EVENT_CONTROL_STREAMON:
        private->last_time = 0;
        break;
      case MCT_EVENT_CONTROL_STREAMOFF:
        break;
      case MCT_EVENT_CONTROL_LINK_INTRA_SESSION: {
        cam_sync_related_sensors_event_info_t *link_param = NULL;
        uint32_t                               peer_identity = 0;
        link_param = (cam_sync_related_sensors_event_info_t *)
          (event->u.ctrl_event.control_event_data);
        peer_identity = link_param->related_sensor_session_id;
        private->intra_peer_id = peer_identity;
        private->dual_cam_sensor_info = link_param->type;
      }
        break;
      case MCT_EVENT_CONTROL_UNLINK_INTRA_SESSION: {
        private->dual_cam_sensor_info = CAM_TYPE_STANDALONE;
        private->intra_peer_id = 0;
      }
        break;
      case MCT_EVENT_CONTROL_SET_PARM: {
        stats_set_params_type *stats_parm = control->control_event_data;

        if (stats_parm->param_type == STATS_SET_IS_PARAM &&
          stats_parm->u.is_param.type == IS_SET_PARAM_IS_ENABLE) {
          if ((SENSOR_ANDROID_FMWK_ENABLE_ALL & private->sensor_intf_mask) ||
            (SENSOR_ANDROID_FMWK_GYRO & private->sensor_intf_mask)) {
             IS_HIGH("Android Intf enabled for gyro! EIS currently not supported!");
             rc = FALSE;
          } else {
            rc = dsps_port_handle_set_is_enable(private,
              stats_parm->u.is_param.u.is_enable);
          }
        } else if (stats_parm->param_type == STATS_SET_COMMON_PARAM &&
          stats_parm->u.common_param.type == COMMON_SET_PARAM_STREAM_ON_OFF) {
          stats_common_set_parameter_t *common_param =
            &(stats_parm->u.common_param);
          IS_LOW("COMMON_SET_PARAM_STREAM_ON_OFF %d",
            common_param->u.stream_on);
          if (common_param->u.stream_on) {
            /* Initialize sensor framework if it's not been done before */
            if ((NULL == private->dsps_handle) &&
              (NULL == private->and_sens_handle) &&
              (NULL == private->imu_handle)) {
              IS_HIGH("Initializing Motion Sensor Framework!!!");
              gyro_port_init_sensors(port);
            }
            IS_LOW("Enable Sensors!");
            gyro_port_enable_disable_sensor_request(port, TRUE);
          } else {
            IS_LOW("Disable Sensors!");
            gyro_port_enable_disable_sensor_request(port, FALSE);
          }
        }
      }
        break;

      default:
        break;
      }
    } /* case MCT_EVENT_CONTROL_CMD */
      break;

    case MCT_EVENT_MODULE_EVENT: {
      mct_event_module_t *mod_event = &event->u.module_event;

      IS_LOW("Module event type %d", mod_event->type);
      switch (mod_event->type) {
      case MCT_EVENT_MODULE_START_STOP_STATS_THREADS: {
        uint8_t *start_flag = (uint8_t*)(mod_event->module_event_data);
        IS_HIGH("MCT_EVENT_MODULE_START_STOP_STATS_THREADS start_flag: %d",
        *start_flag);
        if(*start_flag && private->dsps_handle != NULL && (private->dsps_sensor_list & GYRO_TYPE)) {
          dsps_get_sensor_caps(private->dsps_handle, DSPS_DATA_TYPE_GYRO);
        } else {
          IS_HIGH("DSPS Interface not inited");
        }
      }
        break;

      case MCT_EVENT_MODULE_STATS_AEC_UPDATE: {
        aec_update_t *aec_update_data;
        int i;

        aec_update_data = (aec_update_t *)mod_event->module_event_data;
        i = aec_update_data->frame_id % BUF_SIZE;
        private->exposure_time[i] = aec_update_data->exp_time;
      }
        break;

      case MCT_EVENT_MODULE_SET_STREAM_CONFIG: {
        sensor_out_info_t *sensor_info;
        sensor_info = (sensor_out_info_t *)mod_event->module_event_data;
        private->frame_time = (unsigned long long)
          ((sensor_info->ll_pck * sensor_info->dim_output.height) /
           (sensor_info->vt_pixel_clk / USEC_PER_SEC));
        private->sensor_position = sensor_info->position;
        IS_LOW("frame_time: %llu sensor_position: %d",
          private->frame_time, private->sensor_position);
      }
        break;

      case MCT_EVENT_MODULE_SET_CHROMATIX_WRP: {
        /* TODO Get values from chromatix structure when IS parameters are available
           with 305 header.  For now, hard-code with default values */
        private->rs_interval_offset = 2500;
        private->s3d_interval_offset = 2500;
      }
        break;

      case MCT_EVENT_MODULE_SNS_CONFIG_REQUEST: {
        mct_event_sns_config_request_t *sns_config_request;

        sns_config_request = (mct_event_sns_config_request_t *)mod_event->module_event_data;
        private->gyro_sample_rate_eis = sns_config_request->is_gyro_sampling_rate;
        IS_HIGH("MCT_EVENT_MODULE_SNS_CONFIG_REQUEST, EIS gyro sample rate = %u", private->gyro_sample_rate_eis);
      }
        break;

      case MCT_EVENT_MODULE_GRAVITY_VECTOR_ENABLE: {
        rc = dsps_port_enable_gravity_vector(port,
          mod_event->module_event_data);
      }
        break;

      default:
        break;
      }
    } /* case MCT_EVENT_MODULE_EVENT */
      break;

    default:
      break;
    } /* switch (event->type) */

  } /* case MCT_EVENT_TYPE_DOWNSTREAM */
      break;

  default:
    break;
  } /* switch (MCT_EVENT_DIRECTION(event)) */

  return rc;
}


/** gyro_port_ext_link:
 *    @identity: session id | stream id
 *    @port: gyro port
 *    @peer: For gyro sink port, peer is most likely stats port
 *
 *  Sets gyro port's external peer port.
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_ext_link(unsigned int identity,
  mct_port_t *port, mct_port_t *peer)
{
  boolean rc = FALSE;
  gyro_port_private_t *private;
  mct_event_t event;

   IS_LOW("Enter");
  if (strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return FALSE;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return FALSE;

  MCT_OBJECT_LOCK(port);
  switch (private->state) {
  case GYRO_PORT_STATE_RESERVED:
    if ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {
      rc = TRUE;
    }
    break;
  case GYRO_PORT_STATE_UNLINKED:
    if ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {
      rc = TRUE;
    }
    break;
  case GYRO_PORT_STATE_CREATED:
    /* If gyro module requires a thread, indicate here. */
    rc = TRUE;
    break;

  case GYRO_PORT_STATE_LINKED:
    if ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {
      rc = TRUE;
    }
    #ifdef FEATURE_GYRO_IMU
    if(private->is_type[SENSORS_VIDEO] == IS_TYPE_DIG_GIMB) {
      imu_driver_start_session(private->imu_handle);
    }
    #endif
    break;

  default:
    break;
  }

  if (rc == TRUE) {
    /* If gyro module requires a thread and the port state above warrants one,
       create the thread here */
    private->state = GYRO_PORT_STATE_LINKED;
    MCT_PORT_PEER(port) = peer;
    MCT_OBJECT_REFCOUNT(port) += 1;
  }

  MCT_OBJECT_UNLOCK(port);
  mct_port_add_child(identity, port);
   IS_LOW("rc=%d", rc);
  return rc;
}


/** gyro_port_unlink:
 *  @identity: session id | stream id
 *  @port: gyro port
 *  @peer: gyro port's peer port (probably stats port)
 *
 *  This funtion unlinks the gyro port from its peer.
 **/
static void gyro_port_unlink(unsigned int identity,
  mct_port_t *port, mct_port_t *peer)
{
  gyro_port_private_t *private;

   IS_HIGH("Enter");
  if (!port || !peer || MCT_PORT_PEER(port) != peer)
    return;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return;

  MCT_OBJECT_LOCK(port);
  if (private->state == GYRO_PORT_STATE_LINKED &&
      (private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {

    MCT_OBJECT_REFCOUNT(port) -= 1;
    if (!MCT_OBJECT_REFCOUNT(port)) {
      private->state = GYRO_PORT_STATE_UNLINKED;
      #ifdef FEATURE_GYRO_IMU
      if(private->is_type[SENSORS_VIDEO] == IS_TYPE_DIG_GIMB) {
        imu_driver_stop_session(private->imu_handle);
      }
      #endif
    }
  }
  MCT_OBJECT_UNLOCK(port);
  mct_port_remove_child(identity, port);

   IS_HIGH("Exit");
  return;
}


/** gyro_port_set_caps:
 *    @port: port object whose caps are to be set
 *    @caps: this port's capability.
 *
 *  Function overwrites a ports capability.
 *
 *  Returns TRUE if it is valid source port.
 **/
static boolean gyro_port_set_caps(mct_port_t *port,
  mct_port_caps_t *caps)
{
   IS_LOW("Enter");
  if (strcmp(MCT_PORT_NAME(port), "gyro_sink"))
    return FALSE;

  port->caps = *caps;
  return TRUE;
}


/** gyro_port_check_caps_reserve:
 *    @port: this interface module's port
 *    @peer_caps: the capability of peer port which wants to match
 *                interface port
 *    @stream_info: stream information
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_check_caps_reserve(mct_port_t *port, void *caps,
  void *stream_info)
{
  boolean rc = FALSE;
  mct_port_caps_t *port_caps;
  gyro_port_private_t *private;
  mct_stream_info_t *strm_info = (mct_stream_info_t *)stream_info;

  IS_LOW("Enter");
  MCT_OBJECT_LOCK(port);
  if (!port || !caps || !strm_info ||
      strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    IS_LOW("Exit unsucessful");
    goto reserve_done;
  }

  port_caps = (mct_port_caps_t *)caps;
  /* Change later to SOF? */
  if (port_caps->port_caps_type != MCT_PORT_CAPS_STATS) {
    rc = FALSE;
    goto reserve_done;
  }

  private = (gyro_port_private_t *)port->port_private;
  if (!private) {
    rc = FALSE;
    goto reserve_done;
  }

  switch (private->state) {
  case GYRO_PORT_STATE_LINKED:
    if ((private->reserved_id & 0xFFFF0000) ==
        (strm_info->identity & 0xFFFF0000)) {
      if (strm_info->stream_type == CAM_STREAM_TYPE_VIDEO) {
        private->is_type[SENSORS_VIDEO] = strm_info->is_type;
      } else if (strm_info->stream_type == CAM_STREAM_TYPE_PREVIEW) {
        private->is_type[SENSORS_PREVIEW] = strm_info->is_type;
      }
      rc = TRUE;
    }
    break;

  case GYRO_PORT_STATE_CREATED:
  case GYRO_PORT_STATE_UNRESERVED:
    private->reserved_id = strm_info->identity;
    private->state = GYRO_PORT_STATE_RESERVED;
    rc = TRUE;
    break;

  case GYRO_PORT_STATE_RESERVED:
    if ((private->reserved_id & 0xFFFF0000) ==
        (strm_info->identity & 0xFFFF0000))
      rc = TRUE;
    break;

  default:
    break;
  }

reserve_done:
  MCT_OBJECT_UNLOCK(port);
  return rc;
}


/** gyro_port_check_caps_unreserve:
 *    @port: this port object to remove the session/stream
 *    @identity: session+stream identity
 *
 *  This function frees the identity from port's children list.
 *
 *  Returns FALSE if the identity does not exist.
 **/
static boolean gyro_port_check_caps_unreserve(mct_port_t *port,
  unsigned int identity)
{
  boolean rc = FALSE;
  gyro_port_private_t *private;

  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return FALSE;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return FALSE;

  if (private->state == GYRO_PORT_STATE_UNRESERVED)
    return TRUE;

  MCT_OBJECT_LOCK(port);
  if ((private->state == GYRO_PORT_STATE_UNLINKED ||
       private->state == GYRO_PORT_STATE_RESERVED) &&
      ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000))) {

    if (!MCT_OBJECT_REFCOUNT(port)) {
      private->state = GYRO_PORT_STATE_UNRESERVED;
      private->reserved_id = (private->reserved_id & 0xFFFF0000);
    }
    rc = TRUE;
  }

unreserve_done:
  MCT_OBJECT_UNLOCK(port);
  return rc;
}


/** gyro_port_init:
 *    @port: gyro port
 *    @session_id: session id
 *    @dsps_handle: DSPS handle
 *
 *  This function initializes the gyro port's internal variables.
 *
 *  Returns TRUE on success.
 **/
boolean gyro_port_init(mct_port_t *port, unsigned int session_id)
{
  mct_port_caps_t caps;
  gyro_port_private_t *private;
  IS_HIGH("Enter ");

  if (port == NULL || strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    return FALSE;
  }

  private = (void *)calloc(1, sizeof(gyro_port_private_t));
  if (!private) {
    return FALSE;
  }
  initalizeCallbackRegisters();

  GYRO_PORT_GET_GYRO_SAMPLE_RATE(private->manual_gyro_sample_rate);

  private->reserved_id = session_id;
  private->state = GYRO_PORT_STATE_CREATED;
  if (private->manual_gyro_sample_rate == 0) {
    private->gyro_sample_rate = GYRO_SAMPLE_RATE_AF;
  } else {
    private->gyro_sample_rate = private->manual_gyro_sample_rate;
  }
  private->gyro_sample_rate_eis = DEFAULT_GYRO_SAMPLE_RATE_EIS;
  private->dual_cam_sensor_info = CAM_TYPE_STANDALONE;
  port->port_private = private;
  port->direction = MCT_PORT_SINK;
  caps.port_caps_type = MCT_PORT_CAPS_STATS;
  caps.u.stats.flag = MCT_PORT_CAP_STATS_GYRO;

  mct_port_set_event_func(port, gyro_port_event);
   IS_LOW("gyro_port_event addr = %p", gyro_port_event);
  /* Accept default int_link function */
  mct_port_set_ext_link_func(port, gyro_port_ext_link);
  mct_port_set_unlink_func(port, gyro_port_unlink);
  mct_port_set_set_caps_func(port, gyro_port_set_caps);
  mct_port_set_check_caps_reserve_func(port, gyro_port_check_caps_reserve);
  mct_port_set_check_caps_unreserve_func(port, gyro_port_check_caps_unreserve);

  if (port->set_caps) {
    port->set_caps(port, &caps);
  }

  if(-1 == registerCallback(port,&private->cbPtr)){
    IS_ERR("Failed to Register Gyro Port Handler");
    return FALSE;
  }

  /* Read setprop mask to read if sensor needs to be enabled through android sensor
        framework */
  STATS_GYRO_USE_ANDROID_API(private->sensor_intf_mask);

  /* Read which interface to use for TOF */
  STATS_TOF_INTERFACE_CHOICE(private->tof_intf_direct);
  IS_LOW("TOF interface selected: %d", private->tof_intf_direct);

  gyro_port_init_sensors(port);
  IS_HIGH("EXIT ");

  return TRUE;
}


/** gyro_port_deinit:
 *    @port: gyro port
 *
 * This function frees the gyro port's memory.
 **/
void gyro_port_deinit(mct_port_t *port)
{
  IS_HIGH("E");
  gyro_port_private_t *private;
  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    return;
  }

  IS_HIGH("Deinit Gyro port");
  private = port->port_private;
  gyro_port_deinit_sensors(private);

  if(-1 == deRegisterCallback((void*)port,private->cbPtr)){
    IS_ERR("Failed to Register Gyro Port Handler");
  }
  if (port->port_private) {
    free(port->port_private);
  }
  IS_HIGH("X");
}


/** gyro_port_find_identity:
 *    @port: gyro port
 *    @identity: session id | stream id
 *
 * This function checks for the port with a given session.
 *
 * Returns TRUE if the port is found.
 **/
boolean gyro_port_find_identity(mct_port_t *port, unsigned int identity)
{
  gyro_port_private_t *private;

  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return TRUE;

  private = port->port_private;

  if (private) {
    return ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000) ?
            TRUE : FALSE);
  }

  return FALSE;
}

