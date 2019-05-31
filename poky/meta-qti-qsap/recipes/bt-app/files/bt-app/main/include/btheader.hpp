/******************************************************************************
 *
 *  Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/


#ifndef BT_APP_HPP
#define BT_APP_HPP

#include "osi/include/thread.h"
#include "osi/include/reactor.h"
#include "osi/include/alarm.h"
#include "osi/include/config.h"
#include "gap/include/Gap.hpp"
#include <hardware/bluetooth.h>
#include "include/ipc.h"
#include "utils.h"

#include <cutils/sockets.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

/**
 * @file Main.hpp
 * @brief Main header file for the BT application
*/

/**
 * Maximum argument length
 */
#define COMMAND_ARG_SIZE     200

/**
 * Maximum command length
 */
#define COMMAND_SIZE        200

/**
 * Maximum arguments count
 */
#define MAX_ARGUMENTS        20 //TODO

/**
 * Maximum sub-arguments count
 */
#define MAX_SUB_ARGUMENTS    10


#define BTM_MAX_LOC_BD_NAME_LEN     248

/**
 * Macro used to find the total commands number
 */
#define  NO_OF_COMMANDS(x)  (sizeof(x) / sizeof((x)[0]))

 /**
 * The Configuration options
 */
 /**
const char *BT_SOCKET_ENABLED      = "BtSockInputEnabled";
const char *BT_ENABLE_DEFAULT      = "BtEnableByDefault";
const char *BT_USER_INPUT          = "UserInteractionNeeded";
const char *BT_A2DP_SINK_ENABLED   = "BtA2dpSinkEnable";
const char *BT_A2DP_SOURCE_ENABLED = "BtA2dpSourceEnable";
const char *BT_HFP_CLIENT_ENABLED  = "BtHfClientEnable";
const char *BT_HFP_AG_ENABLED      = "BtHfpAGEnable";
const char *BT_AVRCP_ENABLED       = "BtAvrcpEnable";
const char *BT_ENABLE_EXT_POWER    = "BtEnableExtPower";
const char *BT_ENABLE_FW_SNOOP     = "BtEnableFWSnoop";
const char *BT_ENABLE_SOC_LOG      = "BtEnableSocLog";
const char *BT_HID_ENABLED         = "BtHidEnable";
*/
/**
 * The Configuration file path
 */
 /**
const char *CONFIG_FILE_PATH       = "/etc/bluetooth/bt_app.conf"; */


/**
 * To track user command status
 */
extern bool a2dpsink_connected;
typedef enum  {
    COMMAND_NONE = 0,
    COMMAND_INPROGRESS,
    COMMAND_COMPLETE,
} CommandStatus;

/**
 * To track user command status
 */
typedef struct {
    CommandStatus enable_cmd;
    CommandStatus enquiry_cmd;
    CommandStatus stop_enquiry_cmd;
    CommandStatus disable_cmd;
    CommandStatus pairing_cmd;
} UiCommandStatus;

int BtInit(void);
int BtDeinit(void);
bool GAP_BT_ENABLE(void);
bool GAP_BT_DISABLE(void);
bool GAP_START_PAIR(void);
bool A2DPSink_CONNECT(void);
bool A2DPSink_DISCONNECT(void);
bool AVRCP_PLAY(void);
bool AVRCP_PAUSE(void);
bool AVRCP_STOP(void);
bool AVRCP_FASTFORWARD(void);
bool AVRCP_REWIND(void);
bool AVRCP_FORWARD(void);
bool AVRCP_BACKWARD(void);
bool AVRCP_VOL_UP(void);
bool AVRCP_VOL_DOWN(void);
#endif
