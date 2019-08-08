/*
 * Copyright (c) 2014, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
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

static void *run_test(void *mod) {
	signal(SIGUSR1, signal_handler);
   
   /*  while(1)
    {
        ql_set_vibrator(1000);
        sleep(1);
        ql_set_vibrator(0);
        sleep(1);
    }  */
        sleep(1);
        ql_set_vibrator(1000);
        sleep(1);
        ql_set_vibrator(0);
        //sleep(1);
        sleep(1);
        ql_set_vibrator(1000);
        sleep(1);
        ql_set_vibrator(0);
    return NULL;
}

static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) 
{
    
  
    pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    return SUCCESS;
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
