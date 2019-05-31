/*
Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __STATS_EVENT_H__
#define __STATS_EVENT_H__
#include "mct_event_stats.h"

/* Size base in recomended size by the hardware API */
#define STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE (100)
#define STATS_GROUP_0 0
#define STATS_GROUP_1 1


/** gyro_samples_t
 * @value: samples gyro sample (x,y,z), rad/sec, Q16
 * @timestamp: realtime [usec]
 *
 * Base structure for gyro samples
 **/
typedef struct _gyro_samples{
  int32_t value[3];
  uint64_t timestamp;
} gyro_samples_t;

/** mct_event_gyro_data_t
 *    @frame_id:      the id of frame
 *    @ready:         indicate whether the data is ready
 *    @sample_len:    indicate the number of samples in @sample
 *    @sample:        the array of sample
 *    @sof:           the timestamp of SOF
 *    @frame_time:    the duration of frame
 *    @exposure_time: the expoure value
 *
 *  This structure is used to store gyro data
 *
 **/
typedef struct _mct_event_gyro_data {
  uint32_t frame_id;
  int      ready;
  uint32_t sample_len;
  gyro_samples_t sample[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
  uint64_t sof;
  uint64_t frame_time;
  float    exposure_time;
} mct_event_gyro_data_t;

/** mct_event_gyro_stats_t
 *    @q16_angle:  the angle for x, y, z
 *    @ts:         the timestamp
 *
 *  This structure is used to store gyro data
 *
 **/
typedef struct _mct_event_gyro_stats {
  int q16_angle[3];
  uint64_t ts;
  mct_event_gyro_data_t is_gyro_data;
} mct_event_gyro_stats_t;

/** _mct_event_gravity_vector_enable
 *  @enable: boolean flag - true if enable false if disable.
 *  @report_period: how often to get gravity data, units of
 *    seconds, Q16
 *  @sample_rate_valid: true if sampling rate is passed.
 *  @sample_rate: sample rate in Hz, Q16. If less than report
 *    period, it is set to report rate.
 *
 *  This structure is used to enable/disable gravity vector.
 *
 **/
typedef struct _mct_event_gravity_vector_enable {
  boolean enable;
  uint32_t report_period;
  uint8_t sample_rate_valid;
  uint32_t sample_rate;
} mct_event_gravity_vector_enable_t;

/** _mct_event_light_sensor_update
 *  @lux:  light sensor value is returned in SI lux units.
 *
 *  This structure is used to store light sensor data
 *
 **/
typedef struct _mct_event_light_sensor_update {
  float lux;
} mct_event_light_sensor_update_t;

typedef struct _mct_event_stats_ext_t {
  pthread_mutex_t*        stats_mutex;
  int32_t                 ref_count;
  mct_event_stats_isp_t*  stats_data;
} mct_event_stats_ext_t;

typedef struct {
  uint32_t is_gyro_sampling_rate;
  uint32_t rs_interval_offset;
  uint32_t s3d_interval_offset;
} mct_event_sns_config_request_t;

typedef struct {
  uint64_t timestamp;
  float rotation_matrix[9];
} mct_event_imu_sample_t;

typedef struct {
  uint32_t frame_id;
  uint32_t sample_len;
  mct_event_imu_sample_t sample[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
  uint64_t sof;
  uint64_t frame_time;
  float    exposure_time;
} mct_event_imu_stats_t;

#endif /* __STATS_EVENT_H__ */
