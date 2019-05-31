/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static const mmi_module_t *g_module = NULL;
static int g_pid = -1;

static pthread_mutex_t g_mutex;

#define KEY_WORD_CHANNEL "Channel:"
#define KEY_WORD_ESSID "ESSID:"
#define KEY_WORD_SIGNAL_LEVEL "Signal level"
#define KEY_WORD_SSID "SSID"
#define KEY_WORD_SIGNAL "signal"
#define KEY_WORD_FREQ "freq"
#define DRIVER_PATH "/usr/lib/modules/%s/extra/wlan.ko"

typedef struct {
    char channel[64];
    char essid[64];
    char signal_level[128];
} wifi_info;

static void cb_function(char *str, int size) {
    if(g_module != NULL)
        g_module->cb_print(NULL, SUBCMD_MMI, str, size, PRINT);
}

static char *get_driver_path(char* path) {
    char kv_tmp[256] = {0};

    if (NULL == path)
	    return NULL;

    system("uname -r > /tmp/kernelversion");
     if(read_file("/tmp/kernelversion", kv_tmp, sizeof(kv_tmp))) {
        ALOGE("%s read /tmp/kernelversion error!", __FUNCTION__);
    }
    sprintf(path, DRIVER_PATH, kv_tmp);
    ALOGE("%s *******%s******\n", __FUNCTION__, path);
    return path;
}
static void load_driver() {
    char temp[256] = { 0 };
    char driver_path[256] = { 0 };

    get_driver_path(driver_path);
    if (NULL == driver_path)
	    return;

    snprintf(temp, sizeof(temp), "insmod %s", driver_path);
    system(temp);

    ALOGI("exec '%s' to load wifi driver", temp);
}

static void unload_driver() {
    system("rmmod wlan");

    ALOGI("exec 'rmmod wlan' to unload wifi driver");
}

static void config_wlan() {
    char temp[256] = { 0 };
    snprintf(temp, sizeof(temp), "%s wlan0 up", get_value("wifi_ifconfig"));
    system(temp);

    ALOGI("exec '%s' to config wifi", temp);
}

static int exec_wifi_cmd(exec_cmd_t *cmd, char *result, int result_size) {
    int ret = FAILED;
    char *args[4];
    int i = 0;

    if(check_file_exist(get_value("wifi_iw"))) {
        args[i++] = (char *) get_value("wifi_iw");
        args[i++] = (char *) "wlan0";
        args[i++] = (char *) "scan";
        args[i] = (char *) NULL;
        cmd->cmd = (char *) get_value("wifi_iw");
    } else if(check_file_exist(get_value("wifi_iwlist"))) {
        args[i++] = (char *) get_value("wifi_iwlist");
        args[i++] = (char *) "wlan0";
        args[i++] = (char *) "scanning";
        args[i] = (char *) NULL;
        cmd->cmd = (char *) get_value("wifi_iwlist");
    } else {
        ALOGE("Cannot find WIFI tool to scan device");
        return ret;
    }
    cmd->params = args;
    cmd->pid = &g_pid;
    cmd->exit_str = NULL;
    cmd->result = result;
    cmd->size = result_size;
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; (j < sizeof(args) / sizeof(char *)) && (NULL != args[j]); j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        ALOGI("exec command:'%s', args:%s", cmd->cmd, temp_agrs);
    }
    ret = exe_cmd(cb_function, cmd);
    return ret;
}

static bool parse_exec_result(char *result, char *buf, uint32_t size) {
    wifi_info wifi_result;
    char tmp[256] = { 0 };
    char *p = result;
    char *ptr;
    char *ptr_end;
    bool found = false;

    while(*p != '\0') {         /*print every line of scan result information */
        ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

        if(check_file_exist(get_value("wifi_iw"))) {
            if(strstr(tmp, KEY_WORD_FREQ) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.channel, 0, sizeof(wifi_result.channel));
                snprintf(wifi_result.channel, sizeof(wifi_result.channel), "Channel:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_SIGNAL) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.signal_level, 0, sizeof(wifi_result.signal_level));
                snprintf(wifi_result.signal_level, sizeof(wifi_result.signal_level), "Signal level:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_SSID) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.essid, 0, sizeof(wifi_result.essid));
                snprintf(wifi_result.essid, sizeof(wifi_result.essid), "AP=ESSID:%s;", ptr);
                found = true;
                strlcat(buf, wifi_result.essid, size);
                strlcat(buf, wifi_result.channel, size);
                strlcat(buf, wifi_result.signal_level, size);
                strlcat(buf, "\n", size);
            }
        } else if(check_file_exist(get_value("wifi_iwlist"))) {
            if(strstr(tmp, KEY_WORD_CHANNEL) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.channel, 0, sizeof(wifi_result.channel));
                snprintf(wifi_result.channel, sizeof(wifi_result.channel), "Channel:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_ESSID) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.essid, 0, sizeof(wifi_result.essid));
                snprintf(wifi_result.essid, sizeof(wifi_result.essid), "AP=ESSID:%s;", ptr);
                found = true;
            } else if(strstr(tmp, KEY_WORD_SIGNAL_LEVEL) != NULL) {
                ptr = strstr(tmp, KEY_WORD_SIGNAL_LEVEL);
                ptr_end = strstr(tmp, "Noise level");
                memset(wifi_result.signal_level, 0, sizeof(wifi_result.signal_level));
                snprintf(wifi_result.signal_level, (ptr_end - ptr), "Signal level:%s;", (ptr + 13));
                strlcat(buf, wifi_result.essid, size);
                strlcat(buf, wifi_result.channel, size);
                strlcat(buf, wifi_result.signal_level, size);
                strlcat(buf, "\n", size);
            }
        }
    }

    if(found) {
        ALOGI("wifi test pass, get wifi info: %s", buf);
    } else {
        ALOGE("wifi test fail");
    }

    return found;
}

static int start_test(char *buf, uint32_t size) {
    char result[800 * SIZE_1K] = { 0 };
    char tmp[256] = { 0 };
    char driver_path[256] = { 0 };
    char *args[4];
    int ret = FAILED;
    bool found = false;
    int i = 0;
    exec_cmd_t execmd;

    get_driver_path(driver_path);
    if (NULL == driver_path)
	    return FAILED;

    if(!check_file_exist(driver_path) || !check_file_exist(get_value("wifi_ifconfig"))
       || (!check_file_exist(get_value("wifi_iwlist")) && !check_file_exist(get_value("wifi_iw"))))
        return FAILED;

    load_driver();
    config_wlan();
    ret = exec_wifi_cmd(&execmd, result, sizeof(result));
    unload_driver();

    if(ret != SUCCESS) {
        ALOGE("command:'%s' exec failed", execmd.cmd);
        return FAILED;
    }

    found = parse_exec_result(result, buf, size);

    return found ? SUCCESS : FAILED;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start ", __FUNCTION__);

    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    ALOGI("module init start for module:[%s]", module->name);

    g_module = module;
    pthread_mutex_init(&g_mutex, NULL);

    ALOGI("module init finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module deinit start for module:[%s]", module->name);

    ALOGI("module deinit finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module stop start for module:[%s]", module->name);

    kill_proc(g_pid);
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
    char buf[SIZE_1K] = { 0 };

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    ALOGI("%s start.command :waiting ... %s", __FUNCTION__, cmd);
    g_module = module;

    pthread_mutex_lock(&g_mutex);
    ret = start_test(buf, sizeof(buf));
    pthread_mutex_unlock(&g_mutex);

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
    .version_major = 1,
    .version_minor = 0,
    .name = "Wifi",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = 0,
};
