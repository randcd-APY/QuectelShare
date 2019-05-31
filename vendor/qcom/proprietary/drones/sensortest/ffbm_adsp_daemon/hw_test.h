/*
 * Copyright (c)2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SENSOR_HW_TEST_H
#define _SENSOR_HW_TEST_H

/*sensor type*/
#define SENSOR_TYPE_NONE                 0
#define SENSOR_TYPE_GYROSCOPE            1
#define SENSOR_TYPE_ACCELEROMETER        2
#define SENSOR_TYPE_ULTRASONIC           3
#define SENSOR_TYPE_BAROMETER            4
#define SENSOR_TYPE_ESC                  5
#define SENSOR_TYPE_COMPASS              6
#define SENSOR_TYPE_GPS                  7
#define SENSOR_TYPE_PTZ                  8

/*command type*/
#define COMMAND_TYPE_ENABLE             1
#define COMMAND_TYPE_DISABLE            2
#define COMMAND_TYPE_GETDATA            3

#define SENSOR_NAME_LEN    16
#define EXEC_PATH_LEN      32

/*
 * sensor_type: should be one of the following in the config file,
 *              "accel","gyro", "mag", "baro", "esc"
 *              could support more in the future.
 * sensor_name:
 * exec_name  : test app or dynamic lib name
 * is_lib     : is dynamic lib? could support dynamic lib later
 * is_used    :
 */
struct hw_config {
    int sensor_type;
    char sensor_name[SENSOR_NAME_LEN];
    char exec_name[EXEC_PATH_LEN];
    char is_lib;
    char is_used;
};

/*
 * command type: enable or disable or get_data
 * sensor type : one of "accel","gyro", "mag", "baro", "esc"
 */
typedef struct {
    int cmd_type;
    int sensor_type;
} socket_request_data;

/*
 * sensor_type : one of "accel","gyro", "mag", "baro", "esc"
 * result      : test result, 0: success or 1: fail
 * data_len    : data len written to FFBM socket, should be 1 - 3
 * value       : return the sensor data; for example: gyro sensor,
 *               it should return X Y Z axis information
 */
typedef struct {
    int sensor_type;
    int result;
    int data_len;
    float value[3];
} socket_response_data;

#endif
