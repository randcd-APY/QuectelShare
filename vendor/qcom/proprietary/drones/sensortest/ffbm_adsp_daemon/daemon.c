/*
 * Copyright (c)2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include "hw_test.h"

#define CONFIG_FILE_PATH "/etc/mmi/sensor.config"
#define SOCKET_PATH  "/dev/socket/sensor_socket"
#define SOCKET_FOLDER  "/dev/socket/"
#define SENSOR_TEST_APP_PATH "/usr/tests/"

#define MAX_DEV_NUM  64
#define MAX_RESULT_STR_LEN  64

#define DEBUG_FLAG  0
#if DEBUG_FLAG
#define MSG(...) printf(__VA_ARGS__);
#else
#define MSG(...)
#endif
#define ERR(...) printf(__VA_ARGS__);

static struct hw_config config_table[MAX_DEV_NUM];
static socket_response_data res_data;

/* use below string in the config file */
static char *get_sensor_type_str(int sensor_type)
{
    switch(sensor_type) {
        case SENSOR_TYPE_ACCELEROMETER: return "accel";
        case SENSOR_TYPE_GYROSCOPE: return "gyro";
        case SENSOR_TYPE_COMPASS: return "mag";
        case SENSOR_TYPE_BAROMETER: return "baro";
        default: return "null";
    }
}

static int get_sensor_result_len(int sensor_type)
{
    switch(sensor_type) {
        case SENSOR_TYPE_ACCELEROMETER: return 3;
        case SENSOR_TYPE_GYROSCOPE: return 3;
        case SENSOR_TYPE_COMPASS: return 3;
        case SENSOR_TYPE_BAROMETER: return 1;
        default: return 0;
    }
}

static int parse_config_line(char *buf)
{
    const char *s = NULL;
    const char *e = NULL;
    size_t cpy_len;
    int unused_idx = 0;
    int sensor_type;
    int i;

    for(i = 0; i < MAX_DEV_NUM; i++) {
        if(!config_table[i].is_used) {
            unused_idx = i;
            break;
        }
    }

    /* sensor type */
    if((s = strstr(buf, "accel"))) {
        sensor_type = SENSOR_TYPE_ACCELEROMETER;
    } else if((s = strstr(buf, "gyro"))) {
        sensor_type = SENSOR_TYPE_GYROSCOPE;
    } else if((s = strstr(buf, "mag"))) {
        sensor_type = SENSOR_TYPE_COMPASS;
    } else if((s = strstr(buf, "baro"))) {
        sensor_type = SENSOR_TYPE_BAROMETER;
    } else {
        /* no sensor type was found */
        return -1;
    }

    config_table[unused_idx].sensor_type = sensor_type;

    /* sensor name */
    e = strstr(s, " ");
    if(e == NULL) {
        return -1;
    }
    while(e[0] == ' ')
        e++;

    s = e;
    e = strstr(s, " ");
    if(e == NULL) {
        return -1;
    } else {
        if(e - s >= SENSOR_NAME_LEN)
            cpy_len = SENSOR_NAME_LEN - 1;
        else
            cpy_len = e - s;

        strncpy(config_table[unused_idx].sensor_name, s, cpy_len);
        config_table[unused_idx].sensor_name[cpy_len] = '\0';
    }

    /* exec name with options*/
    while(e[0] == ' ')
        e++;

    if(strlen(e) >= EXEC_PATH_LEN) {
        return -1;
    } else {
        strncpy(config_table[unused_idx].exec_name, e, EXEC_PATH_LEN);
        config_table[unused_idx].exec_name[EXEC_PATH_LEN - 1] = '\0';
    }

    config_table[unused_idx].is_used = 1;

    return 0;
}

static int parse_config_file(void)
{
    int ret;
    int is_valid = 0;
    FILE *fp = NULL;
    size_t len = 0;
    ssize_t read_len;
    char *line = NULL;

    memset(config_table, 0, sizeof(struct hw_config) * MAX_DEV_NUM);

    fp = fopen(CONFIG_FILE_PATH, "r");
    if(!fp) {
        ERR("config file name was found\n");
        return -1;
    }

    while((read_len = getline(&line, &len, fp)) != -1) {
        ret = parse_config_line(line);
        /* config is valid */
        if (!ret)
            is_valid = 1;
    }

    fclose(fp);

    if (is_valid)
        return 0;
    else
        return -1;
}

static int parse_pipe_result(int idx, const char *buf)
{
    char *sensor_type_str = NULL;
    char ret_buf[MAX_RESULT_STR_LEN];
    const char *s = NULL;
    const char *e = NULL;
    const char *r = NULL;
    int result_len;

    /* search the result string.
     * make the result string in below format:
     * sensor_type  [x:val y:val z:val]
     *
     * for example:
     * baro  [pressure: 123]
     * accel [x:123, y:456, z: 789]
     * gyro  [x:123 y:456 z:789]
     *
     * note: using ':' to search the value
     ***************************************/
    sensor_type_str = get_sensor_type_str(config_table[idx].sensor_type);
    r = strstr(buf, sensor_type_str);
    if(r == NULL) {
        return -1;
    }

    s = strstr(r, "[");
    e = strstr(r, "]");
    if(!s || !e) {
        return -1;
    }
    if((e - s) == 1) {
        return -1;
    }
    if((e - s) > MAX_RESULT_STR_LEN) {
        return -1;
    }
    s++;
    strncpy(ret_buf, s, e-s);
    ret_buf[e-s] = '\0';

    result_len = get_sensor_result_len(config_table[idx].sensor_type);
    switch(result_len) {
        case 1:
            s = strstr(ret_buf, ":");
            res_data.value[0] = strtof(s+1, NULL);
            break;
        case 2:
            s = strstr(ret_buf, ":");
            res_data.value[0] = strtof(s+1, &e);
            s = strstr(e, ":");
            res_data.value[1] = strtof(s+1, NULL);
            break;
        case 3:
            s = strstr(ret_buf, ":");
            res_data.value[0] = strtof(s+1, &e);
            s = strstr(e, ":");
            res_data.value[1] = strtof(s+1, &e);
            s = strstr(e, ":");
            res_data.value[2] = strtof(s+1, NULL);
            break;
        default:
            break;
    }

    res_data.result = 0;
    res_data.data_len = result_len;

    return 0;
}

static int exec_cmd(socket_request_data *req_data)
{
    int sensor_type = req_data->sensor_type;
    int idx = MAX_DEV_NUM;
    FILE *fp = NULL;
    char data_buf[256];
    char exec_buf[64] = SENSOR_TEST_APP_PATH;
    int parse_result;
    int i;
    int ret = -1;

    if(sensor_type == SENSOR_TYPE_NONE)
        return -1;

    switch(req_data->cmd_type) {
        case COMMAND_TYPE_ENABLE:
            break;

        case COMMAND_TYPE_GETDATA:
            for(i = 0; i < MAX_DEV_NUM; i++) {
                if((config_table[i].is_used) &&
                        (config_table[i].sensor_type == sensor_type)) {
                    idx = i;
                    break;
                }
            }

            if(idx == MAX_DEV_NUM) {
                ERR("sensor was not found in config table\n");
                return -1;
            }

            strcat(exec_buf, config_table[idx].exec_name);

            /* open pipeline */
            fp = popen(exec_buf, "r");
            parse_result = -1;
            if(fp) {
                res_data.sensor_type = sensor_type;
                res_data.result = 1;
                while(fgets(data_buf, sizeof(data_buf), fp) != NULL) {
                    parse_result = parse_pipe_result(idx, data_buf);

                    /* find the result then stop*/
                    if(!parse_result) {
                        ret = 0;
                        break;
                    }
                }

                pclose(fp);
            } else {
                ERR("sensor test app not found\n");
                return -1;
            }
            break;

         case COMMAND_TYPE_DISABLE:
             break;

         default:
             break;
    }

    return ret;
}

int main(int argc, char **argv)
{
    int srv_fd;
    int clt_fd;
    int ret;
    struct sockaddr_un clt_addr;
    struct sockaddr_un srv_addr;
    socket_request_data req_data;
    socklen_t addr_size = sizeof(clt_addr);

    ret = parse_config_file();
    if(ret) {
        ERR("parse config file failed\n");
        return -1;
    }

    if(NULL == opendir(SOCKET_FOLDER)){
        ret = mkdir(SOCKET_FOLDER, 0755);
        if(ret)
            return -1;
    }

    srv_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if(srv_fd < 0) {
        ERR("create socket failed\n");
        return -1;
    }

    /* local socket */
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, SOCKET_PATH);

    ret = unlink(srv_addr.sun_path);
    if(ret != 0 && errno != ENOENT) {
        ret = -1;
        goto err_1;
    }

    ret = bind(srv_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if(ret < 0) {
        goto err_2;
    }
    chmod(srv_addr.sun_path, (mode_t) 0666);

    ret = listen(srv_fd, 8);
    if(ret < 0) {
        goto err_2;
    }

    /* connect socket */
    while(1) {
        clt_fd = accept(srv_fd, (struct sockaddr *)&clt_addr, &addr_size);
        if(clt_fd < 0) {
            MSG("socket accept error %d:\n", errno);
            continue;
        } else {
            break;
        }
    }

    while (1) {
        memset(&req_data, 0, sizeof(req_data));
        ret = TEMP_FAILURE_RETRY(recv(clt_fd, &req_data, sizeof(req_data), MSG_WAITALL));

        if(exec_cmd(&req_data)) {
            res_data.sensor_type = req_data.sensor_type;
            res_data.result = 1;
        }
        write(clt_fd, &res_data, sizeof(res_data));
    }

    ret = 0;

    close(clt_fd);
err_2:
    unlink(SOCKET_PATH);
err_1:
    close(srv_fd);
    return ret;
}

