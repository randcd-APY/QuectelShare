/* imu_driver.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "imu_driver.h"

void init_imu_handle(imu_handle_type *imu_handle)
{
  if (imu_handle == NULL) {
    IS_ERR("Wrong parameter");
    return;
  }

  imu_handle->libPtr = dlopen("libsensor_imu.so", RTLD_NOW);
  if (!imu_handle->libPtr) {
      IS_ERR("ERROR: couldn't dlopen libsensor_imu.so: %s", dlerror());
      return;
  }

  *(void **)&imu_handle->sensor_imu_api_attitude_get_registered_imu_count =
    dlsym(imu_handle->libPtr,"sensor_imu_api_attitude_get_registered_imu_count");
  if(!imu_handle->sensor_imu_api_attitude_get_registered_imu_count) {
    IS_ERR("cannot load sensor_imu_api_attitude_get_registered_imu_count");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_imu_ids =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_imu_ids");
  if(!imu_handle->sensor_imu_attitude_api_get_imu_ids) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_imu_ids");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_imu_handle =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_imu_handle");
  if(!imu_handle->sensor_imu_attitude_api_get_imu_handle) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_imu_handle");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_version =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_version");
  if(!imu_handle->sensor_imu_attitude_api_get_version) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_version");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_initialize =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_initialize");
  if(!imu_handle->sensor_imu_attitude_api_initialize) {
    IS_ERR("cannot load sensor_imu_attitude_api_initialize");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_terminate =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_terminate");
  if(!imu_handle->sensor_imu_attitude_api_terminate) {
    IS_ERR("cannot load sensor_imu_attitude_api_terminate");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_imu_raw =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_imu_raw");
  if(!imu_handle->sensor_imu_attitude_api_get_imu_raw) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_imu_raw");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_mpu_driver_settings =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_mpu_driver_settings");
  if(!imu_handle->sensor_imu_attitude_api_get_mpu_driver_settings) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_mpu_driver_settings");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_wait_on_driver_init =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_wait_on_driver_init");
  if(!imu_handle->sensor_imu_attitude_api_wait_on_driver_init) {
    IS_ERR("cannot load sensor_imu_attitude_api_wait_on_driver_init");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_bias_compensated_imu =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_bias_compensated_imu");
  if(!imu_handle->sensor_imu_attitude_api_get_bias_compensated_imu) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_bias_compensated_imu");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_get_attitude =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_get_attitude");
  if(!imu_handle->sensor_imu_attitude_api_get_attitude) {
    IS_ERR("cannot load sensor_imu_attitude_api_get_attitude");
  }
  *(void **)&imu_handle->sensor_imu_attitude_api_is_flight_stack_enabled =
    dlsym(imu_handle->libPtr,"sensor_imu_attitude_api_is_flight_stack_enabled");
  if(!imu_handle->sensor_imu_attitude_api_is_flight_stack_enabled) {
    IS_ERR("cannot load sensor_imu_attitude_api_is_flight_stack_enabled");
  }
  *(void **)&imu_handle->
              sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix =
    dlsym(imu_handle->libPtr,
              "sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix");
  if(!imu_handle->
            sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix) {
    IS_ERR("cannot load sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix");
  }
}

void destroy_imu_handle(imu_handle_type *imu_handle)
{
  dlclose(imu_handle->libPtr);
}

void *select_imu_sensor(imu_handle_type *imu_handle)
{
  int16_t num_supported_imu = 0;
  int16_t num_filled_imu_ids = 0;
  int16_t i;
  int16_t dg_imu = 0;
  sensor_imu_id *imu_array;
  void *handle = NULL;

  if (imu_handle->sensor_imu_api_attitude_get_registered_imu_count) {
    imu_handle->sensor_imu_api_attitude_get_registered_imu_count(&num_supported_imu);
  }

  if(num_supported_imu) {
    imu_array = (sensor_imu_id *)malloc(sizeof(sensor_imu_id)*num_supported_imu);
    if (imu_array == NULL) {
      IS_ERR("Allocation error");
      return NULL;
    }
    memset(imu_array, 0x00, sizeof(sensor_imu_id));
    if (imu_handle->sensor_imu_attitude_api_get_imu_ids) {
      imu_handle->sensor_imu_attitude_api_get_imu_ids(imu_array,
                                               num_supported_imu,
                                               &num_filled_imu_ids);
    }
    if (num_filled_imu_ids) {
      for (i = 0; i < num_filled_imu_ids; i++) {
         if (imu_array[i].imu_type == SENSOR_IMU_TYPE_CAMERA_VIDEO) {
           dg_imu = imu_array[i].imu_id;
           break;
         }
      }
      if (i == num_filled_imu_ids) {
        for (i = 0; i < num_filled_imu_ids; i++) {
          if (imu_array[i].imu_type == SENSOR_IMU_TYPE_FLIGHT_PRIMARY) {
            dg_imu = imu_array[i].imu_id;
            break;
          }
        }
      }
      if (i == num_filled_imu_ids) {
        return NULL;
      }
      if (imu_handle->sensor_imu_attitude_api_get_imu_handle) {
        handle = imu_handle->sensor_imu_attitude_api_get_imu_handle(dg_imu);
      }
    } else {
      handle = NULL;
    }
  } else {
    handle = NULL;
  }
  return handle;
}

static void* imu_polling_thread(void *arg)
{
  imu_handle_type *imu_handle = (imu_handle_type *)arg;
  sensor_attitude poll_buffer[MAX_POLL_BUFFER_SIZE];
  int32_t returned_samples_count = 0;
  int32_t i;

  if (imu_handle->sensor_imu_attitude_api_initialize) {
     imu_handle->sensor_imu_attitude_api_initialize(imu_handle->handle,
        SENSOR_CLOCK_SYNC_TYPE_REALTIME);
  }

  if (imu_handle->sensor_imu_attitude_api_wait_on_driver_init) {
     imu_handle->sensor_imu_attitude_api_wait_on_driver_init(imu_handle->handle);
  }

  if (imu_handle->sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix) {
     memset(imu_handle->convert_matrix,0,sizeof(imu_handle->convert_matrix));
     imu_handle->sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix(
                                                                    imu_handle->handle,
                                                                    imu_handle->convert_matrix,
                                                                    9);
     IS_INFO("convert_matrix: %f %f %f %f %f %f %f %f %f",
       imu_handle->convert_matrix[0],imu_handle->convert_matrix[1],imu_handle->convert_matrix[2],
       imu_handle->convert_matrix[3],imu_handle->convert_matrix[4],imu_handle->convert_matrix[5],
       imu_handle->convert_matrix[6],imu_handle->convert_matrix[7],imu_handle->convert_matrix[8]);
 }


  do {

      if (imu_handle->polling_state == STATE_CLOSE) {
        break;
      }
      memset(poll_buffer, 0x00, sizeof(sensor_attitude) * MAX_POLL_BUFFER_SIZE);
      if (imu_handle->sensor_imu_attitude_api_get_attitude) {
        imu_handle->sensor_imu_attitude_api_get_attitude(imu_handle->handle,
                                                        &poll_buffer[0],
                                                        MAX_POLL_BUFFER_SIZE,
                                                       &returned_samples_count,
                                                        0);
      }

      if (returned_samples_count) {
          pthread_mutex_lock(&(imu_handle->circular_buf.buffer_lock));
          for (i = 0; i < returned_samples_count; i++) {
             imu_handle->circular_buf.samples[imu_handle->circular_buf.end_idx] =
                  poll_buffer[i];
             imu_handle->circular_buf.end_idx++;
             if (imu_handle->circular_buf.end_idx >= MAX_SAMPLES_DEPTH) {
                 imu_handle->circular_buf.end_idx = 0;
             }
             if (imu_handle->circular_buf.start_idx ==
                   imu_handle->circular_buf.end_idx)
                 imu_handle->circular_buf.start_idx++;
             if (imu_handle->circular_buf.start_idx >=  MAX_SAMPLES_DEPTH) {
                 imu_handle->circular_buf.start_idx = 0;
             }
          }
          pthread_cond_signal(&(imu_handle->circular_buf.new_sample_signal));
          pthread_mutex_unlock(&(imu_handle->circular_buf.buffer_lock));
          returned_samples_count = 0;
      }
      usleep(10000);
  } while (1);

  if (imu_handle->sensor_imu_attitude_api_terminate) {
     imu_handle->sensor_imu_attitude_api_terminate(imu_handle->handle);
  }
  return NULL;
}

static void* imu_thread_handler(void *arg)
{
  imu_obj_type *imu_obj = (imu_obj_type *)arg;
  imu_handle_type *imu_handle;
  uint32_t sample_idx = 0;
  struct timespec timeout;
  int32_t rc = 0;
  imu_cb_struct_type output_data;
  pthread_t  polling_thread_id;
  imu_request_type *stats_request = NULL;

  imu_handle = (imu_handle_type *)malloc(sizeof(imu_handle_type));
  if(imu_handle == NULL) {
    IS_ERR("Error in allocation");
    return NULL;
  }
  memset(imu_handle, 0x00, sizeof(imu_handle_type));
  imu_handle->circular_buf.samples =
     (sensor_attitude *)malloc(sizeof(sensor_attitude) * MAX_SAMPLES_DEPTH);
  if (imu_handle->circular_buf.samples == NULL) {
    IS_ERR("malloc failed");
    free(imu_handle);
    return NULL;
  }
  memset(imu_handle->circular_buf.samples, 0x00,
    sizeof(sensor_attitude) * MAX_SAMPLES_DEPTH);

  imu_handle->circular_buf.end_idx = 0;
  imu_handle->circular_buf.start_idx = 0;
  imu_handle->polling_state = STATE_RUNNING;
  pthread_mutex_init(&imu_handle->circular_buf.buffer_lock, NULL);
  pthread_cond_init(&(imu_handle->circular_buf.new_sample_signal), NULL);
  init_imu_handle(imu_handle);
  imu_handle->handle = NULL;
  imu_handle->handle = select_imu_sensor(imu_handle);
  if (imu_handle->handle == NULL) {
    IS_ERR(" IMU sensor select failed");
    goto EXIT;
  }
  pthread_create(&polling_thread_id, NULL, imu_polling_thread, imu_handle);
  memset(&output_data, 0x00, sizeof(imu_cb_struct_type));
  output_data.type = TYPE_START;

  if (imu_obj->data_callback)
    imu_obj->data_callback(imu_obj->cbkObj, &output_data);

  do {
    memset(&output_data, 0x00, sizeof(imu_cb_struct_type));

    pthread_mutex_lock(&(imu_obj->request_queue.mutex));
    if(MCT_QUEUE_IS_EMPTY(imu_obj->request_queue.q) == FALSE)
       stats_request = mct_queue_pop_head(imu_obj->request_queue.q);
    pthread_mutex_unlock(&(imu_obj->request_queue.mutex));

    if (stats_request == NULL) {

       pthread_mutex_lock(&imu_obj->new_request_mutex);
       pthread_cond_wait(&imu_obj->new_request_signal,
           &imu_obj->new_request_mutex);
       pthread_mutex_unlock(&imu_obj->new_request_mutex);
       pthread_mutex_lock(&(imu_obj->request_queue.mutex));
       stats_request = mct_queue_pop_head(imu_obj->request_queue.q);
       pthread_mutex_unlock(&(imu_obj->request_queue.mutex));
    }

    if (stats_request && stats_request->type == DATA_REQUEST) {
       pthread_mutex_lock(&(imu_handle->circular_buf.buffer_lock));
       sample_idx = imu_handle->circular_buf.start_idx;
       output_data.frame_id = stats_request->frame_id;
       output_data.type = TYPE_DATA;
       while (stats_request->t_end >
              imu_handle->circular_buf.samples[sample_idx].timestamp_in_us) {
           if(sample_idx == imu_handle->circular_buf.end_idx) {
               rc = clock_gettime(CLOCK_REALTIME, &timeout);
               if (rc < 0) {
                  IS_ERR("Error reading the real time clock");
                  break;
               } else {
                  if ((timeout.tv_nsec + SAMPLES_TIMEOUT) > SECOND_IN_NSECOND) {
                      timeout.tv_sec += 1;
                      timeout.tv_nsec = (timeout.tv_nsec + SAMPLES_TIMEOUT) -
                          SECOND_IN_NSECOND;
                  } else {
                      timeout.tv_nsec += SAMPLES_TIMEOUT;
                  }
               }
               rc = pthread_cond_timedwait(
                  &imu_handle->circular_buf.new_sample_signal,
                  &imu_handle->circular_buf.buffer_lock, &timeout);
               if (rc == ETIMEDOUT) {
                   IS_ERR("Samples collect timeuot");
                   break;
               }
           }

           if(imu_handle->circular_buf.samples[sample_idx].timestamp_in_us >=
                                              stats_request->t_start &&
             imu_handle->circular_buf.samples[sample_idx].timestamp_in_us <=
                                              stats_request->t_end) {
               uint32_t i,j;
               output_data.samples[output_data.sample_len].timestamp =
                  imu_handle->circular_buf.samples[sample_idx].timestamp_in_us;
               for (i = 0; i < 9; i++) {
                 output_data.samples[output_data.sample_len].sample[i] =
                   imu_handle->circular_buf.samples[sample_idx].rotation_matrix[i];
               }

               IS_LOW("original rotation_matrix: %f %f %f %f %f %f %f %f %f",
                 output_data.samples[output_data.sample_len].sample[0],
                 output_data.samples[output_data.sample_len].sample[1],
                 output_data.samples[output_data.sample_len].sample[2],
                 output_data.samples[output_data.sample_len].sample[3],
                 output_data.samples[output_data.sample_len].sample[4],
                 output_data.samples[output_data.sample_len].sample[5],
                 output_data.samples[output_data.sample_len].sample[6],
                 output_data.samples[output_data.sample_len].sample[7],
                 output_data.samples[output_data.sample_len].sample[8]);

               for (i = 0; i < 3; i++) {
                 for (j = 0; j < 3; j++) {
                   output_data.samples[output_data.sample_len].sample[i*3+j] =
                     output_data.samples[output_data.sample_len].sample[i*3]   * imu_handle->convert_matrix[j]   + \
                     output_data.samples[output_data.sample_len].sample[i*3+1] * imu_handle->convert_matrix[j+3] + \
                     output_data.samples[output_data.sample_len].sample[i*3+2] * imu_handle->convert_matrix[j+6];
                 }
               }

               IS_LOW("final rotation_matrix: %f %f %f %f %f %f %f %f %f",
                 output_data.samples[output_data.sample_len].sample[0],
                 output_data.samples[output_data.sample_len].sample[1],
                 output_data.samples[output_data.sample_len].sample[2],
                 output_data.samples[output_data.sample_len].sample[3],
                 output_data.samples[output_data.sample_len].sample[4],
                 output_data.samples[output_data.sample_len].sample[5],
                 output_data.samples[output_data.sample_len].sample[6],
                 output_data.samples[output_data.sample_len].sample[7],
                 output_data.samples[output_data.sample_len].sample[8]);

               output_data.sample_len++;
           }
           sample_idx++;
           if(sample_idx == MAX_SAMPLES_DEPTH)
              sample_idx = 0;
      }
      pthread_mutex_unlock(&(imu_handle->circular_buf.buffer_lock));
      if (imu_obj->data_callback)
          imu_obj->data_callback(imu_obj->cbkObj, &output_data);
      free(stats_request);
      stats_request = NULL;
    } else if (stats_request && stats_request->type == CLOSE_REQUEST) {
        free(stats_request);
        stats_request = NULL;
        break;
    }
  } while(1);
  /*Join polling thread */
  imu_handle->polling_state = STATE_CLOSE;
  pthread_join(polling_thread_id, NULL);
EXIT:
  free(imu_handle->circular_buf.samples);
  pthread_mutex_destroy(&(imu_handle->circular_buf.buffer_lock));
  pthread_cond_destroy(&(imu_handle->circular_buf.new_sample_signal));
  destroy_imu_handle(imu_handle);
  free(imu_handle);
  return NULL;
}


void *imu_driver_init(void *port, imu_data_cb imu_callback)
{
  imu_obj_type *imu_obj = NULL;

  imu_obj = (imu_obj_type *)malloc(sizeof(imu_obj_type));
  if (imu_obj == NULL) {
    IS_ERR("malloc error");
    return NULL;
  }

  imu_obj->session_started = 0;
  imu_obj->data_callback = imu_callback;
  imu_obj->cbkObj = port;
  pthread_mutex_init(&(imu_obj->request_queue.mutex), NULL);
  imu_obj->request_queue.q = (mct_queue_t*) malloc(sizeof(mct_queue_t));
  if(!imu_obj->request_queue.q) {
    IS_ERR("malloc failed");
    return NULL;
  }
  memset(imu_obj->request_queue.q, 0x00, sizeof(mct_queue_t));
  mct_queue_init(imu_obj->request_queue.q);
  pthread_mutex_init(&(imu_obj->new_request_mutex), NULL);
  pthread_cond_init(&(imu_obj->new_request_signal), NULL);

  return (void *)imu_obj;

}

void imu_driver_start_session(imu_obj_type *imu_obj)
{
  if (imu_obj->session_started) {
    return;
  }
  pthread_create(&imu_obj->thread_id, NULL, imu_thread_handler,
    (void *)imu_obj);
  imu_obj->session_started = 1;
}

void imu_driver_stop_session(imu_obj_type *imu_obj)
{
  imu_request_type *close_request;
  if (!imu_obj->session_started) {
    return;
  }
  close_request = (imu_request_type *)malloc(sizeof(imu_request_type));
  if (close_request == NULL) {
      IS_ERR("Fail to allocate");
      return;
  }
  memset(close_request, 0x00, sizeof(imu_request_type));
  close_request->type = CLOSE_REQUEST;
  pthread_mutex_lock(&(imu_obj->request_queue.mutex));
  mct_queue_push_tail(imu_obj->request_queue.q, close_request);
  pthread_mutex_unlock(&(imu_obj->request_queue.mutex));
  pthread_mutex_lock(&(imu_obj->new_request_mutex));
  pthread_cond_signal(&(imu_obj->new_request_signal));
  pthread_mutex_unlock(&(imu_obj->new_request_mutex));
  pthread_join(imu_obj->thread_id, NULL);
  imu_obj->session_started = 0;
}

void imu_driver_deinit(imu_obj_type *imu_obj)
{
  mct_queue_free(imu_obj->request_queue.q);
  pthread_mutex_destroy(&(imu_obj->request_queue.mutex));
  pthread_mutex_destroy(&(imu_obj->new_request_mutex));
  pthread_cond_destroy(&(imu_obj->new_request_signal));
  free(imu_obj);
  return;
}

void imu_driver_request(imu_obj_type *imu_obj, stats_request_type *request)
{
  imu_request_type *sof_request;

  if (!imu_obj || !request) {
      IS_ERR("Incorrect input parameters");
      return;
  }

  sof_request = (imu_request_type *)malloc(sizeof(imu_request_type));
  if (sof_request == NULL) {
      IS_ERR("Fail to allocate");
      return;
  }
  memset(sof_request, 0x00, sizeof(imu_request_type));

  sof_request->type = DATA_REQUEST;
  sof_request->frame_id = request->frame_id;
  sof_request->t_start = request->t_start;
  sof_request->t_end = request->t_end;

  pthread_mutex_lock(&(imu_obj->request_queue.mutex));
  mct_queue_push_tail(imu_obj->request_queue.q, sof_request);
  pthread_mutex_unlock(&(imu_obj->request_queue.mutex));

  pthread_mutex_lock(&(imu_obj->new_request_mutex));
  pthread_cond_signal(&(imu_obj->new_request_signal));
  pthread_mutex_unlock(&(imu_obj->new_request_mutex));

  return;
}
