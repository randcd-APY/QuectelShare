/*
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/

 /*
  * Copyright (C) 2008 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include "mmi_module.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <cutils/log.h>
//#inlcude <linux/input.h>
//#include "InputEventReader.h"
#include <cutils/properties.h>
/*
#define	EVENT_TYPE_ACCEL_X	"ABS_X"
#define	EVENT_TYPE_ACCEL_Y	"ABS_Y"
#define	EVENT_TYPE_ACCEL_Z	"ABS_Z"

#define	EVENT_TYPE_GYRO_X	"ABS_RX"
#define	EVENT_TYPE_GYRO_Y	"ABS_RY"
#define	EVENT_TYPE_GYRO_Z	"ABS_RZ"

#define EVENT_TYPE_MAG_X	"ABS_X"
#define EVENT_TYPE_MAG_Y	"ABS_Y"
#define EVENT_TYPE_MAG_Z	"ABS_Z"

#define EVENT_TYPE_LIGHT		"ABS_MISC"
#define EVENT_TYPE_PROXIMITY		"ABS_DISTANCE"
*/
static char sensor_data_type[][16] = {
	"ABS_X",
	"ABS_Y",
	"ABS_Z",
	"ABS_RX",
	"ABS_RY",
	"ABS_RZ",
	"ABS_MISC",
	"ABS_DISTANCE",
};

/* struct input_event {
      char type;
      char code;
      unsigned int value;
}; */

int sensor_type=0;
char const*const SENSOR_DRI_FILE
        = "/sys/class/sensors";

static char *sensor_path[] = {
	"MPU6050-accel",
	"MPU6050-gyro",
	"mmc3416x-mag",
	"stk3x1x-light",
	"stk3x1x-proximity",
};
int has_acc = 0;
int has_compass = 0;
int has_gyro = 0;
int has_light = 0;
int has_proximity = 0;

static int get_sensor_type(const char *name) {
    int sensor_type = 0;

    if(!strncmp(name, "acc", 3))
        sensor_type = 1;
    else if(!strncmp(name, "mag", 3))
        sensor_type = 2;
    else if(!strncmp(name, "ori", 3))
        sensor_type = 3;
    else if(!strncmp(name, "gyr", 3))
        sensor_type = 4;
    else if(!strncmp(name, "lig", 3))
        sensor_type = 5;
    else if(!strncmp(name, "bar", 3))
        sensor_type = 6;
    else if(!strncmp(name, "tmp", 3))
        sensor_type = 7;
    else if(!strncmp(name, "pro", 3))
        sensor_type = 8;
    else if(!strncmp(name, "grv", 3))
        sensor_type = 9;
    else if(!strncmp(name, "lac", 3))
        sensor_type = 10;
    else if(!strncmp(name, "rot", 3))
        sensor_type = 11;
    else if(!strncmp(name, "hum", 3))
        sensor_type = 12;
    else if(!strncmp(name, "tam", 3))
        sensor_type = 13;
    else
        ALOGE("invalid device name: %s", name);

    return sensor_type;
}

static void get_sensors()
{
	char path[64], *sbuf;
	char *str = "type";
	int fd, id, ret;
	
	for (id = 0; sensor_path[id]!= NULL; id++)
	{
	     snprintf(path, sizeof(path), "/sys/class/sensors/%s", sensor_path[id]);
	     fd = open(path, O_RDONLY);
		 if (fd < 0)
	     {
	        printf("Quectel sensor_path : open failed\n");
	        break;
	     }
		 strcat(path,str);
         fd = open(path, O_RDONLY);		 
	     ret = read(fd, sbuf, 1);
		 if(ret == -1)
         {
            printf("Quectel sensor_path Read type Failed.\n");
            break;
         }
	     close(fd); 
		 switch (*sbuf) {
			 case 1:
			   has_acc = 1; 
			   break;
			 case 4:
			   has_gyro = 1; 
			   break;
			 case 5:
			   has_light = 1; 
			   break;
			 case 8:
			   has_proximity = 1; 
			   break;
			 case 2:
			   has_compass = 1; 
			   break;
		 } 
	}
}

int ql_get_sensor_list(char const* path)		
{
	if (strstr(SENSOR_DRI_FILE, path))
		get_sensors();
	return 0;
}

int ql_enable_sensor(int num)
{
	int fd, ret;
	char *wbuf = "1";
	switch (num) {
		case 1:
			fd = open("/sys/class/sensors/MPU6050-accel/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor acc open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor acc write failed\n");
				break;
			}
			close(fd); 
			break;
		case 4:
			fd = open("/sys/class/sensors/MPU6050-gyro/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor gyro open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor gyro write failed\n");
				break;
			}
			close(fd); 
			break;
		case 2:
			fd = open("/sys/class/sensors/mmc3416x-mag/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor mag open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor mag write failed\n");
				break;
			}
			close(fd); 
			break;
		case 5:
			fd = open("/sys/class/sensors/stk3x1x-light/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor light open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor light write failed\n");
				break;
			}
			close(fd); 
			break;
		case 8:
			fd = open("/sys/class/sensors/stk3x1x-proximity/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor proximity open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor proximity write failed\n");
				break;
			}
			close(fd); 
			break;
	}
	return ret;
}

int ql_disable_sensor(int num)
{
	int fd, ret;
	char *wbuf = "0";
	switch (num) {
		case 1:
			fd = open("/sys/class/sensors/MPU6050-accel/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor acc open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor acc write failed\n");
				break;
			}
			close(fd); 
			break;
		case 4:
			fd = open("/sys/class/sensors/MPU6050-gyro/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor gyro open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor gyro write failed\n");
				break;
			}
			close(fd); 
			break;
		case 2:
			fd = open("/sys/class/sensors/mmc3416x-mag/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor mag open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor mag write failed\n");
				break;
			}
			close(fd); 
			break;
		case 5:
			fd = open("/sys/class/sensors/stk3x1x-light/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor light open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor light write failed\n");
				break;
			}
			close(fd); 
			break;
		case 8:
			fd = open("/sys/class/sensors/stk3x1x-proximity/enable", O_RDWR);
		    if (fd < 0)
			{
				printf("Quectel ql_enable_sensor proximity open failed\n");
				break;
			}
			ret = write(fd, wbuf, 1);
		    if (ret < 0)
			{
				printf("Quectel ql_enable_sensor proximity write failed\n");
				break;
			}
			close(fd); 
			break;
	}
	return ret;
}

char ql_get_sensor_data(int num, int buf[3])
{
	int fd, ret;
	//char buf_one[64], buf_x[64], buf_y[64], buf_z[64];
	struct input_event  event;
	struct pollfd fds[1];
	
	switch (num) {
		case 1:
			fd = open("/dev/input/event4", O_RDONLY);
		    if (fd < 0)
			{
				printf("Quectel ql_get_sensor_data acc open failed\n");
				break;
			}
			fds[0].fd      = fd;
			fds[0].events  = POLLIN;
			while(1)
			{
				ret = poll(fds, 1, 5000);
				if(ret  == 0)
				{
					printf("Quectel ql_get_sensor_data acc time out\n");
				}
				else
				{
					read(fd, &event, sizeof(event));
					//printf("Quectel ql_get_sensor_data acc = %x\n", buffer);
				}
				if (event.code == sensor_data_type[0][0]) {
					buf[0] = event.value;
				} else if (event.code == sensor_data_type[1][0]) {
					buf[1] = event.value;
				} else if (event.code == sensor_data_type[2][0]) {
					buf[2] = event.value;
				}
				//return buf;
			}
		case 2:
			fd = open("/dev/input/event5", O_RDONLY);
		    if (fd < 0)
			{
				printf("Quectel ql_get_sensor_data gyro open failed\n");
				break;
			}
			fds[0].fd      = fd;
			fds[0].events  = POLLIN;
			while(1)
			{
				ret = poll(fds, 1, 5000);
				if(ret  == 0)
				{
					printf("Quectel ql_get_sensor_data gyro time out\n");
				}
				else
				{
					read(fd, &event, sizeof(event));
					//printf("Quectel ql_get_sensor_data gyro = %x\n", buffer);
				}
				if (event.code == sensor_data_type[3][0]) {
					buf[0] = event.value;
				} else if (event.code == sensor_data_type[4][0]) {
					buf[1] = event.value;
				} else if (event.code == sensor_data_type[5][0]) {
					buf[2] = event.value;
				}
				//return buf;
			}
		case 4:
			fd = open("/dev/input/event1", O_RDONLY);
		    if (fd < 0)
			{
				printf("Quectel ql_get_sensor_data mag open failed\n");
				break;
			}
			fds[0].fd      = fd;
			fds[0].events  = POLLIN;
			while(1)
			{
				ret = poll(fds, 1, 5000);
				if(ret  == 0)
				{
					printf("Quectel ql_get_sensor_data mag time out\n");
				}
				else
				{
					read(fd, &event, sizeof(event));
					//printf("Quectel ql_get_sensor_data mag = %x\n", buffer);
				}
				if (event.code == sensor_data_type[0][0]) {
					buf[0] = event.value;
				} else if (event.code == sensor_data_type[1][0]) {
					buf[1] = event.value;
				} else if (event.code == sensor_data_type[2][0]) {
					buf[2] = event.value;
				}
				//return buf;
			}
		case 5:
			fd = open("/dev/input/event5", O_RDONLY);
		    if (fd < 0)
			{
				printf("Quectel ql_get_sensor_data light open failed\n");
				break;
			}
			fds[0].fd      = fd;
			fds[0].events  = POLLIN;
			while(1)
			{
				ret = poll(fds, 1, 5000);
				if(ret  == 0)
				{
					printf("Quectel ql_get_sensor_data light time out\n");
				}
				else
				{
					printf("Quectel ql_get_sensor_data light\n");
					read(fd, &event, sizeof(event));
				/* 	read(fd, a, sizeof(a));
					printf("Quectel ql_get_sensor_data light %x\n",a); */
					//strcpy(buf[0],a);
				//	break;
				//	printf("Quectel ql_get_sensor_data light = %s\n", event.value);
				}
			//	if (event.code == sensor_data_type[6][0])
					buf[0] = event.value;
				//return buf;
				break;
			}
			break;
		case 8:
			fd = open("/dev/input/event3", O_RDONLY);
		    if (fd < 0)
			{
				printf("Quectel ql_get_sensor_data proximity open failed\n");
				break;
			}
			fds[0].fd      = fd;
			fds[0].events  = POLLIN;
			while(1)
			{
				ret = poll(fds, 1, 5000);
				if(ret  == 0)
				{
					printf("Quectel ql_get_sensor_data proximity time out\n");
				}
				else
				{
					read(fd, &event, sizeof(event));
					//printf("Quectel ql_get_sensor_data proximity = %x\n", buffer);
				}
			    if (event.code == sensor_data_type[7][0])
					buf[0] = event.value;
				//return buf;
			}
	}
	return 0;
}

static void *run_test(void *mod) {
    mmi_module_t *module = (mmi_module_t *) mod;
	signal(SIGUSR1, signal_handler);
    int buf[3];
	char sensor_buf[256]={0};
    while(1)
    {
    ql_get_sensor_data(sensor_type,buf);
    ALOGI("THE LIGHT SENSOR DATA=%d",buf[0]);
	if(buf[0]<1000)
	{
		snprintf(sensor_buf, sizeof(sensor_buf), "the light sensor num= %d pass", buf[0]);
	}
	else
	{
		snprintf(sensor_buf, sizeof(sensor_buf), "the light sensor num= %d", buf[0]);
	}
	
	module->cb_print(NULL, SUBCMD_MMI, sensor_buf, strlen(sensor_buf), PRINT_DATA);
	usleep(1000);
    } 
    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("run mmi start for module:[%s]", module->name);
	int ret = FAILED;
	sensor_type=get_sensor_type(params["type"].c_str());
    ql_enable_sensor(sensor_type);
	pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
	ret=SUCCESS;
	return ret;
}

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module init start for module:[%s]", module->name);

    ALOGI("module init finished for module:[%s]", module->name);
    return SUCCESS;
}


static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module deinit start for module:[%s]", module->name);

    ALOGI("module deinit finished for module:[%s]", module->name);
    return SUCCESS;
}


static int32_t module_stop(const mmi_module_t * module) {
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module stop start for module:[%s]", module->name);
	kill_thread(module->run_pid);
    ALOGI("module stop finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) 
{
    int ret = FAILED;
    if(!module || !cmd) 
    {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
    if(!strcmp(cmd, SUBCMD_MMI))
    {
        ret = module_run_mmi(module, params);
    }
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
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
