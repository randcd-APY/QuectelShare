/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#pragma once

#include <stdint.h>
#include "sensor_datatypes.h"
#include <vector>

#if defined(__GNUC__) && defined(BUILDING_SO)
/// MACRO enables function to be visible in shared-library case.
#define IMU_API __attribute__ ((visibility ("default")))
#else
/// MACRO empty for non-shared-library case.
#define IMU_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_IMU_AND_ATTITUDE_API_VERSION "sensor-imu-2.0"

/**
 * Handle to represent the implementation of the sensor_imu_attitude_api
 */
#ifdef ARM_BASED
typedef int sensor_handle;
#else //arm_based
#ifndef ROS_BASED
typedef class VirtualIMU sensor_handle;
#else //ROS_BASED
typedef class VirtualIMU_ROS sensor_handle;
#endif //ROS_BASED
#endif //ARM_BASED
/** 
 * API to get the number of the IMU's supported by the server.  Note, this will provide
 * only the IMU's that are register with the server at the time of calling this API.
 * If other IMU's are added later, one needs to call this again.
 * Also it is possible that the caller has to wait for some time till there is a register IMU handler.
 * @param imu_count
 *    This provides the number of IMU's registered with the server.
 * @return 
 *   0 = success
 *  otherwise = failure
 **/
IMU_API int16_t sensor_imu_api_attitude_get_registered_imu_count( int16_t* imu_count );

/**
 * API to get the array of for different IMU's supported by the IMU server on dsp
 * @param imu_id_array
 *    This is an output array. The array of IMU's supported by the Server.  This is a structure defined by 
 *    @sensor_imu_id.
 * @param array_size
 *    The max size of the array that the server can fill upto.  This should be atleast the 
 *    number of the IMU's reported by the @sensor_imu_api_attitude_get_register_imu_count api.
 * @param returned_element_count
 *    Specifies the number of imu'id's returned.
 * @return 
 *    0 = success
 *  otherwise = failure;
 */
IMU_API int16_t sensor_imu_attitude_api_get_imu_ids
( 
  sensor_imu_id* imu_id_array, 
  int16_t  array_size, 
  int16_t* returned_elements_count
);

/**
 * This returns the handle to the sensor_imu_attitude_api instance for imu_id 0;
 * NOTE: THIS IS AN DEPRECATED API. USE THE API @sensor_imu_attitude_api_get_imu_handle
 * THIS API will be removed in future releases.
 * Use this handle to invoke the api's.
 * @return sensor_handle* 
 *  pointer to the api handle.  
 *  0 = failure.
 *  anyother value = success.
 */
IMU_API sensor_handle* sensor_imu_attitude_api_get_instance() __attribute__( (deprecated) );

/**
 * This returns the handle to the sensor_imu_attitude_api instance for the specified 
 * imu_id. The imu_id should be less than the imu_count returned by @sensor_imu_attitude_api_get_num_imu_count
 * If the imu_id is not in the valid range the return handle is null/0.
 * Use this handle to invoke the api's.
 * @param imu_id
 *   the imu_id returned from get_imu_ids call.
 * @return sensor_handle* 
 *  pointer to the api handle.  
 *  0 = failure.
 *  anyother value = success.
 */
IMU_API sensor_handle* sensor_imu_attitude_api_get_imu_handle( int16_t imu_id );

/**
 * NOTE: This API is deprecated. use @sensor_imu_attitude_api_get_server_version instead.
 * API to get the version of the imu and attitude API.  This should match the 
 * SENSOR_IMU_AND_ATTITUDE_API_VERSION
 */
IMU_API char* sensor_imu_attitude_api_get_version( sensor_handle* handle ) __attribute__( (deprecated) );

/*
 * API to get the version of IMU server.
 */
IMU_API char* sensor_imu_attitude_api_get_server_version();

/**
 * Initializes the sensor api. If the flight stack is running
 * on the ADSP, this initializes the communication channel between the apps process
 * and the adsp. The clock type is used to handle the sychronization of the ADSP and
 * the apps processor. 
 * @param handle
 *   This handle should be obtained by calling the function @sensor_imu_attitude_api_get_imu_handle
 *   This represents the IMU instance to be initialized.
 * @param type
 *  The clock type of the apps process to use for the time synchronization between 
 *  apps processor and the adsp.
 * @param 
 * @return 
 *    0 = success
 *    other = failure.
 */
IMU_API int16_t sensor_imu_attitude_api_initialize
( 
    sensor_handle*         handle, 
    sensor_clock_sync_type type 
);

/**
 * Api to allow for cleanup of the API.  This should be called at shutdown of the 
 * client application. 
 * @return 
 *   0 = success
 *   other = failure
 */
IMU_API int16_t sensor_imu_attitude_api_terminate( sensor_handle* handle );

/**
 * Gets the raw imu data.  For the ADSP implementation
 * this is a blocking call, if there this no data.  
 * On Adsp the numbers of samples bufferred is 100.  If the 
 * buffer is full the oldest data is overwritten.
 * @param dataArray 
 *    Pointer pre-allocated sensor_imu array to get the imu data.
 * @param max_count
 *    The maximum size of the pre-allocated array
 * @param returned_sample_count
 *     the number of imu's samples returned.
 * @return 
 *     0 = success
 *     other = failure;
 * @Note: The ImuData is in  the raw IMU Frame of the eagle board.
 */
IMU_API int16_t sensor_imu_attitude_api_get_imu_raw
( 
    sensor_handle* handle,
    sensor_imu*    dataArray, 
    int32_t        max_count, 
    int32_t*       returned_sample_count 
);

/**
 * This returns Mpu Driver settings.  If the driver is not initialized,
 * the sample rates will be set to -1.
 * @param settings
 *  The setting to be returned.
 * @return int32_t
 *  0 = on success;
 *  otherwise = failure;
 */
IMU_API int16_t sensor_imu_attitude_api_get_mpu_driver_settings
( 
    sensor_handle*              handle, 
    sensor_mpu_driver_settings* settings 
);

/**
 * Method to wait on driver initializtion if needed.
 * @Note: Call this method before calling the sensor_imu_attitude_api_get_imu_raw() 
 * to make sure the driver is initialized.
 * @return int32_t
 * 0 = sucess
 * otherwise = failure
 */
IMU_API int16_t sensor_imu_attitude_api_wait_on_driver_init( sensor_handle* handle );

/**
 * This returns the BiasCompensated IMU based on the flight stack that is
 * being run.  Set the value of "block_on_no_data" to > 0 if api should block until 
 * data is available. 
 * The amount of buffering on the adsp is 100 samples.  If the buffer is full, 
 * the oldest data is overwritten. 
 * **NOTE:** In case there is no flight stack running and the "block_on_no_data" > 0 
 * this will block forever. 
 * **NOTE** This can return a value of zero samples under the following conditions:
 *   * There are not IMU samples
 *   * There is not flight stack running to generate the data
 *   * The implementation of the flight stack is not reporting the data to be 
 *     sent to the Apps process. In this case contact the flight stack vendor/implementor
 *     to enable this. 
 * @param dataArray
 *     A pre-allocated pointer to the sensor_imu array to get the imu from the adsp.
 * @param max_count
 *    The maximum size for the dataArray.
 * @param returned_imu_count
 *     the number of imu's samples returned.
 * @param block_on_no_data
 *    > 0 == true
 *    = 0 == false 
 * @return 
 *     0 = success
 *     other = failure;
 * @Note: The Bias Compensated IMU is in the raw IMU Frame of the snapdragon flight board.
 **/
IMU_API int16_t sensor_imu_attitude_api_get_bias_compensated_imu
( 
  sensor_handle* handle,
  sensor_imu*    dataArray, 
  int32_t        max_count, 
  int32_t*       returned_imu_count,
  uint8_t        block_on_no_data
);

/**
 * This returns the flight attitude based on the flight stack that is
 * being run.  Set the value of "block_on_no_data" to > 0 if api should block until 
 * data is available. 
 * The amount of buffering on the adsp is 100 samples.  If the buffer is full, 
 * the oldest data is overwritten. 
 * **NOTE:** In case there is no flight stack running and the "block_on_no_data" > 0 
 * this will block forever. 
 * **NOTE** This can return a value of zero samples under the following conditions:
 *   * There are no sensor_attitude samples
 *   * There is not flight stack running to generate the data
 *   * The implementation of the flight stack is not reporting the data to be 
 *     sent to the Apps process. In this case contact the flight stack vendor/implementor
 *     to enable this. 
 * @param dataArray 
 *     A pre-allocated pointer to the sensor_attitude array to get the attitude.
 * @param max_count
 *    The maximum size for the dataArray array.
 * @param returned_sample_count
 *    The number of sensor_attitude samples returned.
 * @param block_on_no_data
 *    > 0 == true
 *    = 0 == false
 * @return 
 *     0 = success
 *     other = failure;
 * @Note: The IMU data that is part of the data type is in the raw IMU Frame for the snapdragon flight board.
 **/
IMU_API int16_t sensor_imu_attitude_api_get_attitude
(  
  sensor_handle*   handle,
  sensor_attitude* dataArray, 
  int32_t          max_count, 
  int32_t*         returned_sample_count,
  uint8_t          block_on_no_data
);

/**
 * This will check and provide if the flight stack is enbled on the 
 * Use this function to check before calling any of the following
 * methods:
 *  sensor_imu_attitude_api_get_attitude_buffered()
 *  sensor_imu_attitude_api_get_bias_compensated_imu()
 * @return 
 *  true if flight is enabled.
 */
IMU_API bool sensor_imu_attitude_api_is_flight_stack_enabled( sensor_handle* handle );

/**
 * This will provide a utility method to get the synchornization offset between Apps and DSP
 * in nanoseconds.
 * This offset used as follows to update the timestamp from DSP
 * Apps_UpdatedTimestamp = DSP_rawtimestamp + sync_offset
 * @param clock_type
 *   This represents the clock type used for offset computation. ie either Monotonic or Realtime.
 * @param handle
 *   The IMU handle for which the offset is needed. 
 * @param sync_offset
 *  the offset to return in nanoseconds;
 * @return 
 *   0 = success
 *   other = failure.
 */
IMU_API int16_t sensor_imu_attitude_api_get_dsp_synchornization_offset
( 
  sensor_handle* handle, 
  sensor_clock_sync_type* clock_type, 
  int64_t* sync_offset_ns 
);

/**
 * This function will provide the rotation matrix from the IMU frame to the
 * Body Frame. The IMU frame is dependent on the IMU that is installed
 * on the board.  The Body frame is defined as follows:
 *
 *                              Z
 *                               ^                                  
 *                                \     
 *                                 \   
 *                                  \ 
 *    <-- Front(hires Cam)   x <-----+
 *                                   |
 *                                   v
 *                                   y
 *
 *  z is the upward perpendicular the frame for the eagle board.
 *
 * @param rotation_matrix_3x3
 *   The 3x3 matrix for the Rotation.
 * @param size
 *  the size of the rotation_matrix_3x3 array.
 * @note: The values are in this order:
 * [(0,0),(0,1),(0,2),(1,0),(1,1),(1,2),(2,0),(2,1),(2,2)]
 * Multiple the IMUVector(x,y,z) By the rotationMatrix to get the body frame co-ordinates.
 * If the rotation matrix is not set, this returns an error.
 * @return 
 *  0  = success
 *  -1 = rotation matrix not set
 * other values error;
 */
IMU_API int16_t sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix
(
    sensor_handle* handle, 
    float*         rotation_matrix_3x3, 
    int16_t        size 
);

#ifdef __cplusplus
}
#endif
