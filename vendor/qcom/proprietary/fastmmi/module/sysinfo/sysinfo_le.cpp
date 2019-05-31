/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <cutils/properties.h>

/**
* Defined supported command list here.And put into extra_cmd_list,
* so server could get it.
*/
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MODULE_NAME "SystemInfo"
#define MODULE_AUTHOR "Qualcomm Technologies, Inc."
#define RUN_PID 0

static char g_chipname[1024] = { 0 };
static char g_buildversion[1024] = { 0 };

static const char *extra_cmd_list[] = {
};

static int32_t read_chipname(const mmi_module_t * module, unordered_map < string, string > &params);

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start: ", __FUNCTION__);
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
    ALOGI("%s start: ", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t read_chipname(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start: ", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    int ret = -1;
    ret = read_file(get_value("chipname"), g_chipname, sizeof(g_chipname));
    if(ret != 0) {
        ALOGE("%s read chip name error", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("[hwversion] %s start", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    /* bzero global variable */
    memset(g_chipname, 0, sizeof(g_chipname));
    //memset(g_buildversion, 0, sizeof(g_buildversion));
    //property_get("ro.build.version.release", g_buildversion, "");
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
    ALOGI("[hwversion] %s start.", __FUNCTION__);
    int ret = -1;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    char buf[1024] = { 0 };
    char format_buf[256] = { 0 };
    int run_flag = -1;

    /**MMI version info*/
    snprintf(format_buf, sizeof(format_buf), "MMI = %s\n", MMI_VERSION);
    strlcat(buf, format_buf, sizeof(buf));

    /** SW version info **/
    //property_get("ro.build.version.release", g_buildversion, "");
    //snprintf(format_buf, sizeof(format_buf), "Build Version = %s\n", g_buildversion);
    //strlcat(buf, format_buf, sizeof(buf));

    /* Chip name info */
    if(!check_file_exist(get_value("chipname"))) {
        ALOGE("%s chipname is not exsit\n", __FUNCTION__);
    }else{
        ret = read_chipname(module, params);
        if(FAILED == ret) {
            strlcat(buf, "no chipname info\n", sizeof(buf));
        } else {
            snprintf(format_buf, sizeof(format_buf), "ChipName = %s\n", g_chipname);
            strlcat(buf, format_buf, sizeof(buf));
        }
    }

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
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
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .name = MODULE_NAME,
    .author = MODULE_AUTHOR,
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = RUN_PID,
};
