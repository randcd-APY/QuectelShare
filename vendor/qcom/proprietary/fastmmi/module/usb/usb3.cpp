/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

/**
* Defined supported command list here.And put into extra_cmd_list,
* so server could get it.
*/
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MODULE_NAME "USB3"
#define MODULE_AUTHOR "Qualcomm Technologies, Inc."
#define USBSPEED_DIR "/sys/devices/platform/soc/"
#define USBSPEED_PATH1 USBSPEED_DIR"%s.ssusb/%s.dwc3/udc/%s.dwc3/current_speed"
#define USBSPEED_PATH2 USBSPEED_DIR"%s.usb/msm_hsusb/udc/msm_hsusb/current_speed"
#define USBSPEED_PATH3 USBSPEED_DIR"%s.hsusb/%s.dwc3/udc/%s.dwc3/current_speed"

#define RUN_PID 0

static char us_buf[50] = { 0 };

/**
* Get the usb current_speed path
* @return, 0-success, -1 fail
*/
static int get_uspath(char *path) {
    DIR *dir = NULL;
    struct dirent *ptr = NULL;
    char addr[20] = { 0 };
    char last;
    int ret;
    if(path == NULL)
        return -1;
    dir = opendir(USBSPEED_DIR);
    if(!dir) {
        ALOGI("open dir error.\n");
        return -1;
    }
    while(ptr = readdir(dir)) {
        /**
        **For different chip, SOC address of USB controller is not same,
        **Its path also is not same,So, we need to get the soc address,
        **/
        ret = sscanf(ptr->d_name, "%[^.].ssus%[b]", addr, &last);
        if(ret  == 2) { //if sscanf can success, it will return 2(return 2 parameter)
            sprintf(path, USBSPEED_PATH1, addr, addr, addr);
            return 0;
        }else if(sscanf(ptr->d_name, "%[^.].us%[b]", addr, &last) == 2){
            sprintf(path, USBSPEED_PATH2, addr);
            return 0;
        }else if(sscanf(ptr->d_name, "%[^.].hsus%[b]", addr, &last) == 2){
            sprintf(path, USBSPEED_PATH3, addr, addr, addr);
            return 0;
        }
    }
    return -1;
}

static int us_read(void) {
    if(NULL == us_buf) {
        ALOGE("%s get sn error!\n", __FUNCTION__);
        return FAILED;
    }
    char usb_path[256] = { 0 };

    /* if get_uspath return Fail, read the default path from path_config_le.xml */
    if(get_uspath(usb_path) < 0) {
        ALOGI("use the path in xml");
        memset(usb_path, 0, sizeof(usb_path));
        strcpy(usb_path, get_value("current_speed"));
    }
    ALOGI("usb speed path: %s", usb_path);
    if(!check_file_exist(usb_path)) {
        ALOGE("%s the %s is not exist!\n", __FUNCTION__, get_value("current_speed"));
        return FAILED;
    }

    read_file(usb_path, us_buf, sizeof(us_buf));

    return SUCCESS;
}

/**
 * Defined case run in mmi mode,this mode support UI.
 * @return, 0 -success; -1
 */
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    char buf[50] = { 0 };
    int ret = FAILED;
    char speed[20] = { 0 };

    ret = us_read();
    if(ret != SUCCESS)
        return ret;
    memset(buf, 0, sizeof(buf));
    strlcpy(speed, params["speed"].c_str(), sizeof(speed));
    if(strlen(speed) <= 0) {
        strlcat(buf, "Speed Mode : ", sizeof(buf));
        strlcat(buf, us_buf, sizeof(buf));
        strlcat(buf, "\n", sizeof(buf));

        if(!strncmp(us_buf, "super", 5))
            ret = SUCCESS;
        else
            ret = FAILED;
    } else {
        if(!strcmp(params["speed"].c_str(), "superspeed")) {
            strlcat(buf, "Speed Mode : ", sizeof(buf));
            strlcat(buf, us_buf, sizeof(buf));
            strlcat(buf, "\n", sizeof(buf));

            if(!strncmp(us_buf, "super", 5))
                ret = SUCCESS;
            else
                ret = FAILED;
        } else if(!strcmp(params["speed"].c_str(), "high-speed")) {
            strlcat(buf, "Speed Mode : ", sizeof(buf));
            strlcat(buf, us_buf, sizeof(buf));
            strlcat(buf, "\n", sizeof(buf));

            if(!strncmp(us_buf, "high", 4))
                ret = SUCCESS;
            else
                ret = FAILED;
         } else {
            ALOGI("Arguments %s isn't supported!", params["speed"].c_str());
            ret = FAILED;
         }
     }

    module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);

    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    memset(us_buf, 0, sizeof(us_buf));

    /* init semaphore set the value 1 */
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    kill_thread(module->run_pid);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    ALOGI("%s start.", __FUNCTION__);
    int ret = -1;
    int i = -1;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    ret = module_run_pcba(module, params);

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
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .name = MODULE_NAME,
    .author = MODULE_AUTHOR,
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = RUN_PID,
};
