/* imu_driver.h
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <dlfcn.h>
#include <stdbool.h>
#include "sensor_imu_api.h"
#include "sensor_datatypes.h"
#include "stats_debug.h"
#include "mct_queue.h"

#define MAX_SAMPLES_DEPTH 200
#define MAX_SAMPLES_COUNT 200
#define MAX_POLL_BUFFER_SIZE 200
#define SAMPLES_TIMEOUT (30 * 1000 * 1000)
#define SECOND_IN_NSECOND (1 * 1000 * 1000 * 1000)

typedef enum {
  DATA_REQUEST,
  CLOSE_REQUEST,
} request_enum_type;

typedef enum {
  STATE_RUNNING,
  STATE_CLOSE,
} polling_state_enum_type;

typedef enum {
  TYPE_START = 1,
  TYPE_DATA,
} data_type_t;

typedef struct {
  sensor_attitude *samples;
  uint32_t         start_idx;
  uint32_t         end_idx;
  pthread_mutex_t  buffer_lock;
  pthread_cond_t   new_sample_signal;
} circular_buffer_type;

typedef struct {
  void                    *libPtr;
  circular_buffer_type     circular_buf;
  sensor_handle           *handle;
  float                    convert_matrix[9];
  polling_state_enum_type  polling_state;
  int16_t (*sensor_imu_api_attitude_get_registered_imu_count)( int16_t* imu_count );
  int16_t (*sensor_imu_attitude_api_get_imu_ids)( sensor_imu_id* imu_id_array,
                                                  int16_t  array_size,
                                                  int16_t* returned_elements_count);
  sensor_handle* (*sensor_imu_attitude_api_get_imu_handle)( int16_t imu_id);
  char* (*sensor_imu_attitude_api_get_version)(sensor_handle* handle );
  int16_t (*sensor_imu_attitude_api_initialize)(sensor_handle* handle,
                                            sensor_clock_sync_type type);
  int16_t (*sensor_imu_attitude_api_terminate)(sensor_handle* handle );
  int16_t (*sensor_imu_attitude_api_get_imu_raw)(sensor_handle* handle,
                                               sensor_imu*  dataArray,
                                               int32_t     max_count,
                                               int32_t*  returned_sample_count);
  int16_t (*sensor_imu_attitude_api_get_mpu_driver_settings)(sensor_handle*  handle,
                                                sensor_mpu_driver_settings* settings);
  int16_t (*sensor_imu_attitude_api_wait_on_driver_init)(sensor_handle* handle);
  int16_t (*sensor_imu_attitude_api_get_bias_compensated_imu)(sensor_handle* handle,
                                                             sensor_imu*  dataArray,
                                                             int32_t  max_count,
                                                             int32_t*  returned_imu_count,
                                                             uint8_t  block_on_no_data);
  int16_t (*sensor_imu_attitude_api_get_attitude)(sensor_handle*   handle,
                                                  sensor_attitude* dataArray,
                                                  int32_t  max_count,
                                                  int32_t*  returned_sample_count,
                                                  uint8_t  block_on_no_data);
  bool (*sensor_imu_attitude_api_is_flight_stack_enabled)(sensor_handle* handle);
  int16_t (*sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix)(
                                                  sensor_handle* handle,
                                                  float* rotation_matrix_3x3,
                                                  int16_t  size);
} imu_handle_type;

typedef struct {
  uint32_t            frame_id;
  request_enum_type   type;
  unsigned long long  t_start;
  unsigned long long  t_end;
} imu_request_type;

typedef struct {
  uint64_t timestamp;
  float sample[9];
} imu_ouptut_sample_t;

typedef struct {
  data_type_t type;
  uint32_t sample_len;
  uint32_t frame_id;
  imu_ouptut_sample_t samples[MAX_SAMPLES_COUNT];
} imu_cb_struct_type;

typedef void (*imu_data_cb)(void *port, imu_cb_struct_type *cb_event_data);

typedef struct {
   mct_queue_t       *q;
   pthread_mutex_t    mutex;
} request_queue_type;

typedef struct {
  imu_data_cb         data_callback;
  void               *cbkObj;
  request_queue_type  request_queue;
  pthread_cond_t      new_request_signal;
  pthread_mutex_t     new_request_mutex;
  pthread_t           thread_id;
  int                 session_started;
} imu_obj_type;

typedef struct {
  uint32_t            frame_id;
  unsigned long long  t_start;
  unsigned long long  t_end;
} stats_request_type;

void *imu_driver_init(void *port, imu_data_cb imu_callback);
void imu_driver_start_session(imu_obj_type *imu_obj);
void imu_driver_stop_session(imu_obj_type *imu_obj);
void imu_driver_deinit(imu_obj_type *imu_obj);
void imu_driver_request(imu_obj_type *imu_obj, stats_request_type *request);
