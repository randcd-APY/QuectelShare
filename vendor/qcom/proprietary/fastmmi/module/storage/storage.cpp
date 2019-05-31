/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <sys/statfs.h>
#include "mmi_module.h"

#define UFS_SYS_PATH "/sys/block/sda"
#define SDCARD1_PATH "/storage/sdcard1"
/** Sdcard resource */
#define DEV_MMCBLK1 "/dev/block/mmcblk1"
#define SYS_MMCBLK0_SIZE "/sys/class/block/mmcblk0/size"
#define DEV_BOOTDEVICE "/dev/block/bootdevice"
#define PROC_PARTITIONS "/proc/partitions"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/

static bool is_ufs() {
    char tmp[256] = { 0 };

    int len = readlink(UFS_SYS_PATH, tmp, sizeof(tmp));

    if(len < 0) {
        ALOGI("read bootdevice fail");
        return false;
    }

    if(strstr(tmp, "ufs") != NULL)
        return true;
    else
        return false;
}

/*get the volume of ufs rom*/
static bool ufs_info(double *ufs_size) {
    int ret;
    int ufs = 0;
    FILE *f = NULL;
    char tmp[256] = { 0 };
    struct {
        int major;
        int minor;
        int blocks;
        char name[256];
    } proc_partition;

    if(!is_ufs() || (ufs_size == NULL))
        return false;

    *ufs_size = 0;
    f = fopen(PROC_PARTITIONS,"r");
    if(f == NULL) {
        ALOGI("open partitions failed.");
        return false;
    }

    /* Cycle until the size of sda got or end of file reached */
    while(fgets(tmp, 256, f)) {
        memset(&proc_partition, 0, sizeof(proc_partition));
        if(tmp[0] == '\n' || !strstr(tmp, "sd"))
            continue;
        /**
           major minor   #blocks   name
           8     0      28307456  sda
           8     2      32768     sda1
        **/
        ret = sscanf(tmp, "%d%d%d%s", &proc_partition.major, &proc_partition.minor,
                  &proc_partition.blocks, proc_partition.name);
        /*it the sscanf can get 4 value, we think it is correct format*/
        if(ret != 4)
            continue;
        if(strstr(proc_partition.name, "sd") && isalpha(proc_partition.name[2])
                  && proc_partition.name[3] == '\0') {
            *ufs_size += (double)proc_partition.blocks;
             ufs = 1;
        }
    }

    if(ferror(f)) {
        ALOGI("read partitions failed.");
        *ufs_size = 0;
    }

    if(!ufs)
       return false;
    else
       return true;
}

static int flash_info(char *buf, int size, int64_t * target_size, const char *type) {
    int ret = FAILED;
    char tmp[256] = { 0 };
    double total_size = 0;

    if(buf == NULL || target_size == NULL)
        return ret;

    if(!strcmp(type, "ufs")) {
        /**It is UFS device*/
        if(ufs_info(&total_size)) {
            ALOGI("UFS device found");
            total_size = total_size / (1024 * 1024);
            snprintf(buf, size, "ufs capacity = %4.3f G \n", total_size);
            strlcat(buf, "ufs = ufs deteced \n", size);
            *target_size = (int) total_size;
            ret = SUCCESS;
        } else {
            strlcpy(buf, "ufs = not deteced \n", size);
            ret = FAILED;
        }
    } else {
        /**It is EMMC device*/
        if(!is_ufs() && !read_file(SYS_MMCBLK0_SIZE, tmp, sizeof(tmp))) {
            total_size = (((double) atoi(tmp) * BLOCK_SIZE)) / SIZE_1G;
            snprintf(buf, size, "emmc capacity = %4.3f G \n", total_size);
            strlcat(buf, "emmc = deteced \n", size);
            *target_size = (int) total_size;
            ret = SUCCESS;
        } else {
            strlcpy(buf, "emmc = not deteced \n", size);
            ret = FAILED;
        }
    }
    return ret;
}

static int sdcard_info(char *buf, int size, int64_t * target_size) {
    struct statfs st;
    int ret = FAILED, n;
    char tmp[SIZE_8K] = { 0 };
    bool found = false;
    char num[32] = { 0 };
    double total_size = 0;
    char mmcblk[8] = { 0 };

    if(buf == NULL || target_size == NULL)
        return ret;

    memset(mmcblk, 0, sizeof(mmcblk));
    if(is_ufs()) {
        /**
          179        0   15159296 mmcblk0
          179        1   15155200 mmcblk0p
        **/
        ALOGI("Internal storage type is UFS.");
        strcpy(mmcblk, "mmcblk0");
    } else {
        /**
          179       32       4096 mmcblk0rpmb
          179       64    7761920 mmcblk1
          179       65    7757824 mmcblk1p1
        **/
        ALOGI("Internal storage type is EMMC.");
        strcpy(mmcblk, "mmcblk1");
    }

    /* Check sdcard and its size */
    if(!read_file(PROC_PARTITIONS, tmp, sizeof(tmp))) {
        char *p = tmp;
        char *ptr;

        while(*p != '\0') { /*print every line of scan result information */
            ptr = tmp;
            while(*p != '\n' && *p != '\0') {
                *ptr++ = *p++;
            }

            p++;
            *ptr = '\0';

            ptr = strstr(tmp, mmcblk);
            if(ptr != NULL) {

                char *x = ptr;

                while(isspace(*x) || !isdigit(*x)) {
                    x--;
                }
                n = 0;
                while(*x) {
                    if(isspace(*x) || !isdigit(*x)) {
                        ALOGI("end of digit");
                        break;
                    }
                    x--;
                    n++;
                }

                strlcpy(num, x + 1, n + 1);
                found = true;
                break;
            }
        }
        if(found) {
            total_size = (double) string_to_long(num) / SIZE_1M;
            snprintf(buf, size, "sdcard_capacity = %4.3f G  \n", total_size);
            strlcat(buf, "sdcard = detected \n", size);
            *target_size = (int) total_size;
            ret = SUCCESS;
          } else {
            ALOGI("sdcard not found ");
            strlcpy(buf, "sdcard = not deteced \n", size);
        }

    } else {
        ALOGI("sdcard not found ");
        strlcpy(buf, "sdcard = not deteced \n", size);
    }

    return ret;
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
    int64_t target_size = 0, min_limit = 0, max_limit = 0;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    ALOGI("%s start.command : %s", __FUNCTION__, cmd);

    /**Run test */
    if(!strcmp(params["type"].c_str(), "emmc") || !strcmp(params["type"].c_str(), "ufs")) {
        ret = flash_info(buf, sizeof(buf), &target_size, params["type"].c_str());
    } else if(!strcmp(params["type"].c_str(), "sdcard")) {
        ret = sdcard_info(buf, sizeof(buf), &target_size);
    } else {
        ALOGE("FFBM STORAGE: Unknow storage device");
        return FAILED;
    }
    ALOGI("%s start.command : target_size:%llu", __FUNCTION__, target_size);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        min_limit = string_to_long(params[KEY_MIN_LIMINT]);
        max_limit = string_to_long(params[KEY_MAX_LIMINT]);
        if(max_limit != 0) {
            if(target_size > min_limit && target_size < max_limit)
                ret = SUCCESS;
        }
        ALOGI("%s target_size : %llu limit[%llu,%llu] GB", __FUNCTION__, target_size, min_limit, max_limit);
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
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
    .name = "Storage",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = 0,
};
