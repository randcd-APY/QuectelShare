/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "mmi_module.h"

#define QPNP_VIBRATOR_ENABLE  "/sys/class/timed_output/vibrator/enable"

static const char str_vibrator_on[] = "Vibrator on\n";
static const char str_vibrator_off[] = "Vibrator off\n";
static bool _switch_running = false;

static int32_t module_stop(const mmi_module_t * module);

int ql_set_vibrator( int value)	
{
	int len = 0;
	int fd;
	char buf[10];

	fd = open(QPNP_VIBRATOR_ENABLE, O_WRONLY|O_NONBLOCK);
	if (fd < 0){
	   ALOGE("open vibrator failed\n");
	   return -1;
	 }
	
	sprintf(buf, "%d", value);
	len = write(fd, buf, strlen(buf));
	 if(len != strlen(buf)) {        
	   	ALOGE("write vibrator failed\n");        
		close(fd);       
		return -1;    
	}
	close(fd);  
	return 0;
}

static void *run_test(void *mod) {
    signal(SIGUSR1, signal_handler);

    if(_switch_running){
        while(1) {
            ql_set_vibrator(5000);
            //usleep(800 * 1000);
        }
    }else{
        while(1) {
            ql_set_vibrator(200);
            usleep(800 * 1000);
        }
    }
    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("run mmi start for module:[%s]", module->name);

    int ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);

    if(ret < 0) {
        ALOGE("Can't create thread, error=%s\n", strerror(errno));
        return FAILED;
    } else {
        ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);
        pthread_join(module->run_pid, NULL);
    }

    ALOGI("run mmi finished for module:[%s]", module->name);
    return ret;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("Vibrator %s start", __FUNCTION__);
    int _fd = -1;
    char gpio_status[8] = {0,};
    long ret = -1;
    int retry = 0;
    long diff[2] = {0,};
   
    _switch_running = true;

    ALOGI("[%s] vibrator start to create thread for automation", __FUNCTION__);
    _fd = open("/sys/class/power_supply/battery/mpp4_voltage", O_RDONLY );
    if(_fd<0){
        ALOGI("[%s] vibrator open adc failed fd is %d", __FUNCTION__, _fd);
        return FAILED;
    } 
    read(_fd, gpio_status, 7);
    gpio_status[7] = '\0';
    ALOGI("[%s] vibrator not running fd=%d,adc_char=%s", __FUNCTION__,_fd,gpio_status);
    ret = atol(gpio_status);
    diff[0] = abs(ret-1800000); 
    ALOGI("[%s] vibrator not running adc=%ld,diff[0]=%ld", __FUNCTION__,ret,diff[0]);
    if((diff[0]<200000)){
        ALOGI("[%s] vibrator not running test successful", __FUNCTION__);
    }else{
        ALOGI("[%s] vibrator not running test FAILED", __FUNCTION__);
    }
    close(_fd);
    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        ALOGE("[%s]:Vibrator can't create pthread: %s", __FUNCTION__, strerror(errno));
        return FAILED;
    }
    sleep(1);

    _fd = open("/sys/class/power_supply/battery/mpp4_voltage", O_RDONLY );
    if(_fd<0){
        ALOGI("[%s] vibrator open adc failed fd is %d", __FUNCTION__, _fd);
        return FAILED;
    }
    while(retry<10){
        memset(gpio_status,0,7); 
        read(_fd, &gpio_status[0], 7);
        gpio_status[7] = '\0';
        ret = atol(gpio_status);
        diff[1] = ret; 
        ALOGI("[%s] vibrator running retry=%d adc=%ld,diff[1]=%ld", __FUNCTION__,retry,ret,diff[1]);
        if((diff[1]<1500000 && diff[1]>700000)){
            ALOGI("[%s] vibrator running test successful", __FUNCTION__);
            close(_fd);
            break;
        }
        close(_fd);
        retry++;
        _fd = open("/sys/class/power_supply/battery/mpp4_voltage", O_RDONLY );
        if(_fd<0){
            ALOGI("[%s] vibrator open adc failed fd is %d", __FUNCTION__, _fd);
        }
    }   
    
    module_stop(module);
    if((diff[0] <= 200000) && (retry<10)){
        ret = SUCCESS;
    }else{
        ret = FAILED;
    }
    ALOGI("[%s] Vibrator Test Result is %s", __FUNCTION__, ret == SUCCESS?"PASS":"FAILED");
    return ret;

}

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

    ALOGI("vibrator off");
    ql_set_vibrator(0);

    ALOGI("module start to stop finished for module:[%s],thread id=%lu", module->name, module->run_pid);
    kill_thread(module->run_pid);

    ALOGI("module stop finished for module:[%s]", module->name);
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
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = module_run_pcba(module, params);
    else {
        ALOGE("Received invalid command: %s", MMI_STR(cmd));
        ret = FAILED;
    }

    ALOGI("module run finished for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
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
    .name = "Vibrator",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
