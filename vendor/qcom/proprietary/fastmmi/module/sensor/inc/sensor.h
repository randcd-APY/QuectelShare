/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __SENSOR__
#define __SENSOR__

/*command type*/
#define COMMAND_TYPE_ENABLE          1
#define COMMAND_TYPE_DISABLE         2
#define COMMAND_TYPE_GETDATA         3

/*sensor type*/
#define SENSOR_TYPE_GYROSCOPE            1
#define SENSOR_TYPE_ACCELEROMETER        2
#define SENSOR_TYPE_ULTRASONIC           3
#define SENSOR_TYPE_BAROMETER            4
#define SENSOR_TYPE_ESC                  5
#define SENSOR_TYPE_COMPASS              6
#define SENSOR_TYPE_GPS                  7
#define SENSOR_TYPE_PTZ                  8

#define SENSOR_DATA_LENTH_0        0
#define SENSOR_DATA_LENTH_2        2
#define SENSOR_DATA_LENTH_3        3
#define SENSOR_DATA_LENTH_6        6
#define SENSOR_DATA_LENTH_7        7
#define SENSOR_DATA_LENTH_10       10

typedef struct sensors_event {
    int type;
    float data[SENSOR_DATA_LENTH_10];
} sensors_event_t;

//define the sensor list structure
typedef struct sensor {
    int type;
    const char *name;
    int len;
} sensor_t;

typedef struct{
    int cmd_type;
    int sensor_type;
} sensors_cmd;

typedef struct {
    int sensor_type;
    int result;
    int quantity;
    float data[SENSOR_DATA_LENTH_3];
} sensors_data;



#endif
