/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static const mmi_module_t *g_module = NULL;
static exec_cmd_t_1 execmd;
static pthread_mutex_t g_mutex;

#define KEY_WORD_CHANNEL "Channel:"
#define KEY_WORD_ESSID "ESSID:"
#define KEY_WORD_COMPLETED "COMPLETED"
#define KEY_WORD_SIGNAL_LEVEL "Signal level"
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

static int start_test(char *buf, uint32_t size) {
    char result[500 * SIZE_1K] = { 0 };
    char tmp[256] = { 0 };
    wifi_info wifi_result;
    int ret = FAILED;
    bool found = false;

    if(!check_file_exist(get_value("wifi_driver")) || !check_file_exist(get_value("wifi_ifconfig"))
       || !check_file_exist(get_value("wifi_iwlist")))
        return FAILED;

    load_driver();
    config_wlan();

    int loop = 0;
    system("/usr/sbin/wpa_supplicant -B -iwlan0 -Dnl80211 -c/etc/mmi/wpa_supplicant.conf -O/var/run/wpa_supplicant -ddd &");
    sleep(2);	
    system("wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan");
    sleep(2);
	
for(loop = 0; loop < 6; loop ++)
	{
	    memset(result, 0 ,SIZE_1K);
	    memset(buf, 0 ,SIZE_1K);
           memset(tmp, 0 ,256);
	    sleep(5);
	    char *args[] = {(char *)"/usr/sbin/wpa_cli", (char *)"-i", (char *)"wlan0", (char *)"-p", (char *)"/var/run/wpa_supplicant", (char *)"stat", NULL};
	    execmd.cmd = "/usr/sbin/wpa_cli";
	    execmd.params = args;
	    execmd.pid = -1;
	    execmd.result = result;
	    execmd.size = sizeof(result);
	    ret = exe_cmd_1(cb_function, &execmd);


	    if(ret != SUCCESS)
	        return FAILED;

	char *p = result;
	char *ptr;

	while(*p != '\0') { 		/*print every line of scan result information */
		ptr = tmp;
		while(*p != '\n' && *p != '\0') {
			*ptr++ = *p++;
		}

		p++;
		*ptr++ = '\n';
		*ptr = '\0';
		strlcat(buf, tmp, size);
		ptr = strstr(tmp, KEY_WORD_COMPLETED);
		if(ptr != NULL) {
			//ptr = strstr(tmp, ":");
			//ptr++;
			//snprintf(tmp, sizeof(tmp), "ESSID: = %s \n", ptr);
			//strlcat(buf, tmp, size);
			found = true;
			unload_driver();
			return SUCCESS;
		}

	}
    }
unload_driver();
return FAILED;
/*

    char *args[4] = { (char *) get_value("wifi_iwlist"), (char *) "wlan0", (char *) "scanning", NULL };
    execmd.cmd = (char *) get_value("wifi_iwlist");
    execmd.params = args;
    execmd.pid = -1;
    execmd.result = result;
    execmd.size = sizeof(result);
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; (j < sizeof(args) / sizeof(char *)) && (NULL != args[j]); j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(cb_function, &execmd);
    unload_driver();

    if(ret != SUCCESS) {
        ALOGE("command:'%s' exec failed", execmd.cmd);
        return FAILED;
    }

    char *p = result;
    char *ptr;
    char *ptr_end;
*/
  //  while(*p != '\0') {         /*print every line of scan result information */
    /*    ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

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

    if(found) {
        ALOGI("wifi test pass, get wifi info: %s", buf);
    } else {
        ALOGE("wifi test fail");
    }

    return found ? SUCCESS : FAILED;
*/
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module init start for module:[%s]", module->name);

    g_module = module;
    pthread_mutex_init(&g_mutex, NULL);

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

    kill_proc(execmd.pid);
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
        ALOGE("NULL point received");
        return FAILED;
    }
    ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
    g_module = module;

    pthread_mutex_lock(&g_mutex);
    ret = start_test(buf, sizeof(buf));

   // ALOGI("module:[%s] test %s", module->name, MMI_TEST_RESULT(ret));
    pthread_mutex_unlock(&g_mutex);

   /*  if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    } else {
        ALOGE("Received invalid command: %s", MMI_STR(cmd));
    } */

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
    .name = "Wifi",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
