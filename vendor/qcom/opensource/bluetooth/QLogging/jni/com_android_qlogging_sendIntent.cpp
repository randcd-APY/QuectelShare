/*********************************************************************
*
* Copyright (c) 2015, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
************************************************************************/

#define LOG_TAG "QLoggingJni"
#include "hardware/bluetooth.h"
#include "utils/Log.h"
#include "utils/misc.h"
#include "cutils/properties.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"
#include "jni.h"
#include "logging.h"
#include "JNIHelp.h"

#include <string.h>
#include <pthread.h>

#include <sys/stat.h>
#include <fcntl.h>
#define HCI_GRP_HOST_CONT_BASEBAND_CMDS (0x03 << 10)
#define HCI_OPCODE_PACK(ogf, ocf) (unsigned short int)((ocf & 0x03ff)|(ogf << 10))
#define HCI_RESET                       (0x0003 | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
#define PAYLOAD_DATA_SIZE               765
#define PAYLOAD_LENGTH                  255

namespace android{

static const bt_interface_t *sBluetoothInterface = NULL;
static const btstacklog_interface_t *sBluetoothLogInterface = NULL;

jint HAL_Load(){
        jint err;
        hw_module_t* BT_HW_module;

        err = hw_get_module(BT_HARDWARE_MODULE_ID,(hw_module_t const**)&BT_HW_module);

        if (err == 0) {
            hw_device_t* stack;
            err = BT_HW_module->methods->open(BT_HW_module, BT_HARDWARE_MODULE_ID, &stack);
            if (err == 0) {
                    bluetooth_module_t* btStack = (bluetooth_module_t *)stack;
                    sBluetoothInterface = btStack->get_bluetooth_interface();
            } else {
               ALOGE("Loading BT Interface failed");
               return err;
            }
        }
        else {
             ALOGE("Error loading BT Module");
             return err;
        }
        return err;
}

static void setLogging(JNIEnv* env, jclass clazz,jstring jlog_layer, jint log_level, jint module)
{

    if (sBluetoothInterface == NULL)
    {
        if(HAL_Load()<0)
            ALOGE("HAL failed to load BT");
        else
            ALOGE("HAL loaded BT successfully");
    }

    if (sBluetoothInterface) {
        if ( (sBluetoothLogInterface = (btstacklog_interface_t *)
                sBluetoothInterface->get_profile_interface("LOG_ID")) == NULL) {
                ALOGE("Error loading Stack Log Interface");
        }
        else{
            uint8_t values[7] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06};
            const char *log_layer = env->GetStringUTFChars(jlog_layer, 0);
            const char *vsc_data = env->GetStringUTFChars(jlog_layer, 0);
            if ( module == 3 )
            {
                ALOGD("Setting SOC log levels.");
                uint8_t set_level = 0x00;
                int index = log_layer[0]-'0';
                if (log_level == 0)
                    set_level = 0xF;
                else
                    set_level = values[log_level-1];
                if ((index < 7) && (index >= 0))
                {
                    uint8_t array[3]={0x11,values[index],set_level};
                    sBluetoothInterface->hci_cmd_send(HCI_OPCODE_PACK(0x3F,0x17),array,3);
                }
            }
            else if( module == 4)
            {
                ALOGD("Setting all SOC log levels.");
                uint8_t enable_SOC[5] = {0x10,0x03,0x00,0x00,0x01};
                uint8_t disable_SOC[5] = {0x10,0x02,0x00,0x00,0x01};
                const char *all_log_levels = env->GetStringUTFChars(jlog_layer,0);
                if (all_log_levels[0] == 'F')/* Setting All Levels to Off.*/
                {
                   sBluetoothInterface->hci_cmd_send(HCI_OPCODE_PACK(0x3F,0x17),disable_SOC,5);
                }
                else
                {
                    /* Enable SOC Logs */
                    sBluetoothInterface->hci_cmd_send(HCI_OPCODE_PACK(0x3F,0x17),enable_SOC,5);
                    int index[7];
                    uint8_t param[15];
                    param[0]= 0x11;
                    for(int i=0;i<7;i++){
                        param[2*i+1] = values[i];
                        index[i]=all_log_levels[i]-'0';
                        if (index[i] == 0)
                            param[2*i+2] = 0xF;
                        else
                            param[2*i+2] = values[index[i]-1];
                    }
                    sBluetoothInterface->hci_cmd_send(HCI_OPCODE_PACK(0x3F,0x17),param,15);
                }
            }
            else if (module == 5)
            {
                ALOGD("Sending VS Commands.");
                uint8_t vsc_data_t[PAYLOAD_LENGTH] = {0};
                char str[PAYLOAD_DATA_SIZE] = {NULL};
                strlcpy(str,vsc_data,strlen(vsc_data)+1);
                int count_t = 0;
                char* token_pointer;
                char *tmp_token = NULL;
                token_pointer = strtok_r(str, ",", &tmp_token);
                while ((NULL != token_pointer) && (count_t < PAYLOAD_LENGTH))
                {
                    vsc_data_t[count_t] = (uint8_t)strtol(token_pointer,NULL,16);
                    token_pointer = strtok_r(NULL, ",", &tmp_token);
                    count_t++;
                }
                ALOGD("Total count:%d ",count_t );
                sBluetoothInterface->hci_cmd_send(HCI_OPCODE_PACK(0x3F,log_level),vsc_data_t,count_t);
            }
            else
            {
                ALOGD("Setting stack log levels.");
                sBluetoothLogInterface->setLog(log_layer,log_level);
            }
        }
    }
}

static JNINativeMethod sMethods[] = {
    {"setLogging", "(Ljava/lang/String;II)V", (void *) setLogging}
};

int register_com_android_qlogging_sendIntent(JNIEnv* env)
{
    return  jniRegisterNativeMethods(env, "com/android/qlogging/sendIntent",
                                    sMethods, NELEM(sMethods));

}

}/*namespace android*/


jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    JNIEnv *e;
    int status;

    // Check JNI version
    if (jvm->GetEnv((void **)&e, JNI_VERSION_1_6)) {
        ALOGE("Error matching JNI version");
        return JNI_ERR;
    }

    if ((status = android::register_com_android_qlogging_sendIntent(e)) < 0) {
        ALOGE("jni QLOGGING registration failure, status: %d", status);
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
