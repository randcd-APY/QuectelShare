/*
 * Copyright (c) 2014,2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

static unordered_map < string, string > paras;

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static void light_on(bool enable) {
    if(enable) {
        if(!paras["type"].empty()) {
            if(!strncmp(paras["type"].c_str(), "flash_0", 7))
                write_file(get_value("flashlight_torch_0"), "200");
            else if(!strncmp(paras["type"].c_str(), "flash_1", 7))
                write_file(get_value("flashlight_torch_1"), "200");
        }
        write_file(get_value("leds_switch"), "1");
    } else {
        if(!paras["type"].empty()) {
            if(!strncmp(paras["type"].c_str(), "flash_0", 7))
                write_file(get_value("flashlight_torch_0"), "1");
            else if(!strncmp(paras["type"].c_str(), "flash_1", 7))
                write_file(get_value("flashlight_torch_1"), "1");
        }
        write_file(get_value("leds_switch"), "0");
    }
}

static void *run_test(void *mod) {
    int delay = 0;

    if(mod == NULL) {
        ALOGE("%s NULL for cb function ", __FUNCTION__);
        return NULL;
    }
    delay = atoi(paras["delay"].c_str());

    signal(SIGUSR1, signal_handler);

    while(1) {
        light_on(true);
        ((mmi_module_t *) mod)->cb_print(NULL, SUBCMD_MMI, "flashlight on", 13, PRINT);
        usleep(delay * 1000);
        light_on(false);
        ((mmi_module_t *) mod)->cb_print(NULL, SUBCMD_MMI, "flashlight off", 14, PRINT);
        usleep(delay * 1000);
    }

    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start ", __FUNCTION__);
    int ret = FAILED;

    paras = params;

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        ALOGE("%s:Can't create pthread: %s\n", __FUNCTION__, strerror(errno));
        return FAILED;
    } else {
        pthread_join(module->run_pid, NULL);
    }

    return ret;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start", __FUNCTION__);

    paras = params;
    light_on(true);
    module->cb_print(NULL, SUBCMD_PCBA, "flashlight on", 13, PRINT);

    return ERR_UNKNOW;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start ", __FUNCTION__);

    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

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

    light_on(false);
    pthread_kill(module->run_pid, SIGUSR1);
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
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    ALOGI("%s start.command : %s", __FUNCTION__, cmd);

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = module_run_pcba(module, params);
    else {
        ALOGE("%s Invalid command: %s  received ", __FUNCTION__, cmd);
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
    .name = "Flashlight",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
