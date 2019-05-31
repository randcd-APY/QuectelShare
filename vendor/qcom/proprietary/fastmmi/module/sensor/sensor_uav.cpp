/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "mmi_module.h"
#include "sensor.h"

/**
 * Defined case run in mmi mode,this mode support UI.
 *
 */

static const char *extra_cmd_list[] = {
};

#define KEY_SENSOR_SOCKET "/dev/socket/sensor_socket"

static int dev_count = 0;

static int cur_sensor_type = 0;
static char cur_sensor_name[32];

static pthread_mutex_t g_mutex;
static exec_cmd_t execmd;
static int g_sensor_sock = -1;

static sensor_t sensor_list[8] = {
     {SENSOR_TYPE_GYROSCOPE, "gyroscope", SENSOR_DATA_LENTH_3},
     {SENSOR_TYPE_ACCELEROMETER, "accelermeter", SENSOR_DATA_LENTH_3},
     {SENSOR_TYPE_ULTRASONIC, "ultrasonic", SENSOR_DATA_LENTH_10},
     {SENSOR_TYPE_BAROMETER, "barometer", SENSOR_DATA_LENTH_2},
     {SENSOR_TYPE_ESC, "esc", SENSOR_DATA_LENTH_10},
     {SENSOR_TYPE_COMPASS, "compass", SENSOR_DATA_LENTH_3},
     {SENSOR_TYPE_GPS, "gps", SENSOR_DATA_LENTH_10},
     {SENSOR_TYPE_PTZ, "ptz", SENSOR_DATA_LENTH_10}
};

static char const *get_sensor_name(int type) {
    switch (type) {
    case SENSOR_TYPE_GYROSCOPE:
        return "gyroscope";
    case SENSOR_TYPE_ACCELEROMETER:
        return "accelermeter";
    case SENSOR_TYPE_ULTRASONIC:
        return "ultrasonic";
    case SENSOR_TYPE_BAROMETER:
        return "barometer";
    case SENSOR_TYPE_ESC:
        return "esc";
    case SENSOR_TYPE_COMPASS:
        return "compass";
    case SENSOR_TYPE_GPS:
        return "gps";
    case SENSOR_TYPE_PTZ:
        return "ptz";
    }

    return "ukn";
}

static int get_sensor_type(const char *name) {
    int sensor_type = 0;

    int i = 0;

    for(i = 0; i < dev_count; i++) {
        if(!strncmp(name, sensor_list[i].name, strlen(sensor_list[i].name))) {
            sensor_type = sensor_list[i].type;
            break;
        }
    }

    return sensor_type;
}

int send_command(int fd, sensors_cmd * msg) {
    int ret = 0;

    if(fd > 0) {
        ret = write(fd, msg, sizeof(sensors_cmd));
    }
    return ret;
}

static int sensor_enable(int sensor_type, int delay, bool enable) {
    int err = SUCCESS;
    struct timeval timeout = {3,0}; //timeout of socket recieve data
#ifdef ENABLE_SENSOR_FUNC
    sensors_cmd buffer;
    int response = 0;

    memset(&buffer, 0, sizeof(sensors_cmd));//initialize the sensor data buffer
    buffer.cmd_type = (enable == true ? COMMAND_TYPE_ENABLE : COMMAND_TYPE_DISABLE);
    buffer.sensor_type = sensor_type;
    buffer.data_quantity = SENSOR_DATA_LENTH_0;

    send_command(g_sensor_sock, &buffer);

    response = (int *) zmalloc(sizeof(int));
    if(response == NULL){
        ALOGE("%s:out of memory, abort the current request:(%s)\n", __FUNCTION__, strerror(errno));
        err = -1;
    }

    setsockopt(g_sensor_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    if((recv_size = recv(g_sensor_sock, &response, sizeof(int), MSG_WAITALL)) < 0) {
         ALOGE("%s:Recv fail: %s, received size:%d\n", __FUNCTION__, strerror(errno), recv_size);
         err = -1;
    }

    ALOGI("%s:Received one enable message from server[%d],recv_size=%d", __FUNCTION__, response, recv_size);

    if(response == -1)
        err = -1;
#endif
    return err;
}

int read_sensor_data(float* ptr,int ptrlen,int sensor_type){
    int err = SUCCESS;
    sensors_cmd send;
    sensors_data response;
    int recv_size = 0;
    struct timeval timeout = {250,0}; //timeout of socket recieve data

    ALOGI("%s: start!", __FUNCTION__);
    memset(&send, 0, sizeof(sensors_cmd)); //initialize the sensor cmd buffer
    memset(&response, 0, sizeof(sensors_data)); //initialize the sensor data buffer

    send.cmd_type = COMMAND_TYPE_GETDATA;
    send.sensor_type = sensor_type;

    ALOGI("%s: g_sensor_sock = %d", __FUNCTION__, g_sensor_sock);
    err = send_command(g_sensor_sock, &send); //send command to FFBM sensor daemon
    if(err <= 0){
        ALOGI("%s fail to send command to g_sensor_sock", __FUNCTION__);
        return FAILED;
    }
    ALOGI("%s: send command to g_sensor_sock successfully", __FUNCTION__);

    setsockopt(g_sensor_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    if((recv_size = recv(g_sensor_sock, &response, sizeof(sensors_data), MSG_WAITALL)) < 0) {
         ALOGE("%s:Recv fail: %s, received size:%d\n", __FUNCTION__, strerror(errno), recv_size);
         return FAILED;
    }

    ALOGI("%s:Received one data message from server[%d,%d,%d,%5.5f,%5.5f,%5.5f],recv_size=%d",
              __FUNCTION__, response.sensor_type, response.result, response.quantity,
              response.data[0], response.data[1], response.data[2], recv_size);
    if(response.result == 0){
        ptr[0] = response.data[0];
        ptr[1] = response.data[1];
        ptr[2] = response.data[2];
        err = SUCCESS;
    }else{
        err = FAILED;
        ALOGI("%s: sensor data can not got by FFBM sensor DAEMON = %d!", __FUNCTION__, err);
    }
    return err;
}

static int test_event(sensors_event_t * event, unordered_map < string, string > &params) {
    int err = FAILED;
    double x_min, x_max, y_min, y_max, z_min, z_max, v_min, v_max;

    if(event != NULL) {
        switch (event->type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_GYROSCOPE:

            x_min = atof(params["x_min_limit"].c_str());
            x_max = atof(params["x_max_limit"].c_str());
            y_min = atof(params["y_min_limit"].c_str());
            y_max = atof(params["y_max_limit"].c_str());
            z_min = atof(params["z_min_limit"].c_str());
            z_max = atof(params["z_max_limit"].c_str());

            ALOGI("%s: limitation for(%s):x(%5.5f,%5.5f),y(%5.5f,%5.5f),z(%5.5f,%5.5f). data(%5.5f,%5.5f,%5.5f)\n",
                __FUNCTION__, get_sensor_name(event->type), x_min, x_max, y_min, y_max, z_min, z_max, event->data[0],
                    event->data[1], event->data[2]);
            if(inside_float(event->data[0], x_min, x_max) && inside_float(event->data[1], y_min, y_max)
                && inside_float(event->data[2], z_min, z_max))
                err = SUCCESS;
            break;

        case SENSOR_TYPE_BAROMETER: //barometer unit is Pa
            x_min = atof(params["x_min_limit"].c_str());
            x_max = atof(params["x_max_limit"].c_str());

            ALOGI("%s: limitation for(%s):x(%6.5f,%6.5f). data(%6.5f)\n",
                __FUNCTION__, get_sensor_name(event->type), x_min, x_max, event->data[0]);
            if(inside_float(event->data[0], x_min, x_max))
                err = SUCCESS;
            break;

        case SENSOR_TYPE_ULTRASONIC:
        case SENSOR_TYPE_ESC:
        case SENSOR_TYPE_COMPASS:
        case SENSOR_TYPE_GPS:
        case SENSOR_TYPE_PTZ:
            err = FAILED;
            ALOGE("%s: FFBM SENSOR: Current, This sensor test isn't supported... returning\n", __FUNCTION__);
            break;
        default:
            ALOGE("%s: FFBM SENSOR: Data received, but sensor is unknown... returning\n", __FUNCTION__);
            break;
        }
    }
    return err;
}

static void print_event(sensors_event_t * event, const mmi_module_t * mod, bool is_pcba) {
    int err = FAILED;
    char print_buf[256] = {0};

    if(event == NULL || mod == NULL)
        return;

    switch (event->type) {
    case SENSOR_TYPE_GYROSCOPE:
    case SENSOR_TYPE_ACCELEROMETER:
        snprintf(print_buf,
                 sizeof(print_buf),
                 "%s\nx = %5.5f\ny = %5.5f\nz = %5.5f\n", (char *) get_sensor_name(event->type),
                 event->data[0], event->data[1], event->data[2]);
        break;

    case SENSOR_TYPE_BAROMETER: ////barometer unit is Pa
        snprintf(print_buf,
                 sizeof(print_buf),
                 "%s\nx = %5.5f\ny = %5.5f\n", (char *) get_sensor_name(event->type), event->data[0], event->data[1]);
        break;

    case SENSOR_TYPE_GPS:
    case SENSOR_TYPE_ULTRASONIC:
    case SENSOR_TYPE_COMPASS:
    case SENSOR_TYPE_ESC:
    case SENSOR_TYPE_PTZ:
        err = FAILED;
        ALOGE("%s: FFBM SENSOR: Current, This sensor test isn't supported... returning\n", __FUNCTION__);
        break;
    default:
        ALOGE("%s: FFBM SENSOR: Data received, but sensor is unknown... returning", __FUNCTION__);
        break;
    }

    if(!is_pcba)
        mod->cb_print(cur_sensor_name, SUBCMD_MMI, print_buf, strlen(print_buf), PRINT_DATA);
    else
        mod->cb_print(cur_sensor_name, SUBCMD_PCBA, print_buf, strlen(print_buf), PRINT_DATA);

}

static int get_sensor_data(int sensor_type, sensors_event_t * event) {
    int err = FAILED;

    if(sensor_type == 0)
        return err;
    event->type = sensor_type;
    memset(event->data, 0, sizeof(float) * 10);
    for(int i = 0; i < dev_count; i++) {
        if(sensor_list[i].type == sensor_type) {
            err = read_sensor_data(event->data, sensor_list[i].len, sensor_list[i].type);
            if(err == SUCCESS) {
                ALOGI("%s: Run sensor : %s success\n", __FUNCTION__, sensor_list[i].name);
            } else {
                ALOGE("%s: Run sensor : %s fail\n", __FUNCTION__, sensor_list[i].name);
            }
            break;
        }
    }
    return err;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s: start:%s\n", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s: NULL point  received \n", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
 * Defined case run in PCBA mode, fully automatically.
 *
 */
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start\n", __FUNCTION__);

    int ret = FAILED, tried = 3;
    sensors_event_t event;

    pthread_mutex_lock(&g_mutex);
    cur_sensor_type = get_sensor_type(params["type"].c_str());
    ALOGI("%s: cur_sensor_type = %d\n", __FUNCTION__, cur_sensor_type);
    strlcpy(cur_sensor_name, params[KEY_MODULE_NAME].c_str(), sizeof(cur_sensor_name));

    /**Test several data, if any of them correct will returen succesful*/
    while(tried-- > 0) {
        if(get_sensor_data(cur_sensor_type, &event) == SUCCESS){
            ret = test_event(&event, params);
            if(ret == SUCCESS)
                break;
        }
    }

    ALOGI("%s: finished\n", __FUNCTION__);
    print_event(&event, module, true);

    pthread_mutex_unlock(&g_mutex);
    ALOGI("%s: pthread_mutex_unlock\n", __FUNCTION__);

    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s: start sensor\n", __FUNCTION__);

    if(module == NULL) {
        ALOGE("%s: NULL point  received\n ", __FUNCTION__);
        return FAILED;
    }

    pthread_mutex_init(&g_mutex, NULL);
    dev_count = sizeof(sensor_list) / sizeof(sensor_t);
    g_sensor_sock = connect_server(KEY_SENSOR_SOCKET);
    ALOGE("%s: connect socket = %d\n ", __FUNCTION__, g_sensor_sock);
    if(g_sensor_sock < 0)
         return FAILED;
    cur_sensor_type = get_sensor_type(params["type"].c_str());
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    ALOGI("%s: start.\n", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s: NULL point  received\n", __FUNCTION__);
        return FAILED;
    }
    //sensor_enable(cur_sensor_type, 0, false);

    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    ALOGI("%s: start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s: NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    pthread_kill(module->run_pid, SIGUSR1);
    ALOGI("%s: FFBM MMI test thread exit, disable the sensor(%s) unlock\n", __FUNCTION__,
               get_sensor_name(cur_sensor_type));
    //sensor_enable(cur_sensor_type, 0, false);

    return SUCCESS;
}

/**
 * Before call Run function, caller should call module_init first to initialize the module.
 * the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
 *
 */
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    int ret = FAILED;

    if(!module || !cmd) {
        ALOGE("%s: NULL point received", __FUNCTION__);
        return FAILED;
    }
    ALOGI("%s: start.command : %s", __FUNCTION__, cmd);

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA)){
        ret = module_run_pcba(module, params);
    }else {
        ALOGE("%s: Invalid command: %s  received.", __FUNCTION__, cmd);
        ret = FAILED;
    }

    /** Default RUN mmi*/
    return ret;
}

/**
 * Methods must be implemented by module.
 */
static struct mmi_module_methods_t module_methods = {
    .module_init = module_init,
    .module_deinit = module_deinit,
    .module_run = module_run,
    .module_stop = module_stop,
};

/**
 * Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
 * and the fields of this data structure must be initialize in strictly sequence as definition,
 * please don't change the sequence as g++ not supported in CPP file.
 */
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = 1,
    .version_minor = 0,
    .name = "Sensor",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};

