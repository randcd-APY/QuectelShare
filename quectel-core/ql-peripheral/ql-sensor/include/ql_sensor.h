#ifndef _QL_GPIO_H_
#define _QL_GPIO_H_

#include <stdint.h>
#include <sys/types.h>

#define EV_SYN 0x00
#define ABS_X  0x00
#define ABS_Y  0x01
#define ABS_Z  0x02
#define ABS_RX 0x03
#define ABS_RY 0x04
#define ABS_RZ 0x05

#define GRAVITY_EARTH     (9.80665f)
#define CONVERT_MAG	      (1.0f/16.0f)
#define GYROSCOPE_CONVERT (M_PI / (180 * 16.4))

#define MAX_INPUT_EVENTS 256

struct input_event {
	struct timeval time;
	uint16_t type;
	uint16_t code;
	long value;
};

int QL_Get_Sensor_List(char *list);
int QL_Enable_Sensor(const char *name, int enable);
int QL_Get_Sensor_Data(const char *name, struct input_event *value);

#endif