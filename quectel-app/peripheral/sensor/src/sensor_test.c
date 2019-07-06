/**
 * @file sensor_test.c
 * @brief Quectel SC20 Module about sensor example.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "ql_sensor.h"

static char sensor_name[64];

static void *QL_Read_Thread(__attribute__((unused))void *data)
{
	int i, count;
	float value;
	struct input_event *event;
	struct input_event events[MAX_INPUT_EVENTS];

	while (1) {
		count = QL_Get_Sensor_Data(sensor_name, events);
		if (count < 0) {
			fprintf(stderr, "QL_Get_Sensor_Data failed\n");
			return NULL;
		}
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (event->type == EV_SYN)
				continue;
			if (!strcmp(sensor_name, "light"))
				fprintf(stderr, "Lux [%d]\n", event->value);
			else if (!strcmp(sensor_name, "prox"))
				fprintf(stderr, "Dis [%d]\n", event->value);
			else if (!strcmp(sensor_name, "mag")) {
				value = event->value * CONVERT_MAG;
				if (event->code == ABS_X)
					fprintf(stderr, "X [%f]\n", value);
				if (event->code == ABS_Y)
					fprintf(stderr, "Y [%f]\n", value);
				if (event->code == ABS_Z)
					fprintf(stderr, "Z [%f]\n", value);
			} else if (!strcmp(sensor_name, "accel")) {
				value = event->value * (GRAVITY_EARTH / 16384);
				if (event->code == ABS_X)
					fprintf(stderr, "X [%f]\n", value);
				if (event->code == ABS_Y)
					fprintf(stderr, "Y [%f]\n", value);
				if (event->code == ABS_Z)
					fprintf(stderr, "Z [%f]\n", value);
			} else if (!strcmp(sensor_name, "gyro")) {
				value = event->value * GYROSCOPE_CONVERT * -1;
				if (event->code == ABS_RX)
					fprintf(stderr, "X [%f]\n", value);
				if (event->code == ABS_RY)
					fprintf(stderr, "Y [%f]\n", value);
				if (event->code == ABS_RZ)
					fprintf(stderr, "Z [%f]\n", value);
			}
		}
	}

    return NULL;
}

int main(int argc, char *argv[])
{
	int ret;
	pthread_t pid;
	int wait_time;
	char sensor_list[64];

	if (argc < 2) {
		fprintf(stderr, "usage: %s [list] | [type] [wait_time]\n", argv[0]);
		return -1;
	}

	memset(sensor_name, 0, sizeof(sensor_name));
	strcpy(sensor_name, argv[1]);
	if (!strcmp(argv[1], "list")) {
		memset(sensor_list, 0, sizeof(sensor_list));
		ret = QL_Get_Sensor_List(sensor_list);
		if (ret <0) {
			fprintf(stderr, "%s: QL_Get_Sensor_List failed\n", argv[0]);
			return -1;
		}
		fprintf(stdout, "%s: sensor list [%s]\n", argv[0], sensor_list);
	} else {
		sscanf(argv[2], "%d", &wait_time);
		fprintf(stdout, "%s: wait sensor data time [%d]\n", argv[0], wait_time);
		ret = QL_Enable_Sensor(argv[1], 1);
		if (ret < 0) {
			fprintf(stderr, "%s: enable sensor [%s] failed\n", argv[0], argv[1]);
			return -1;
		}
		ret = pthread_create((pthread_t *)&pid, NULL, QL_Read_Thread, NULL);
	    if (ret < 0) {
	        fprintf(stderr, "%s: create read thread failed\n", argv[0]);
	        return -1;
		}
		sleep(wait_time);
		ret = QL_Enable_Sensor(argv[1], 0);
		if (ret < 0) {
			fprintf(stderr, "%s: disable sensor [%s] failed\n", argv[0], argv[1]);
			return -1;
		}
	}

	return 0;
}
