/******************************************************************************

Copyright (c) 2013,2016, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************/
#ifndef BTC_COMMON_H
#define BTC_COMMON_H

#include "bt_hci_bdroid.h"
#include "hci/include/hci_hal.h"

/*****************************************************************************/

typedef unsigned short uint16_t;

#define HCI_COMMAND                                     0x01
#define HCI_EVT                                         0x04
#define OCF_INQUIRY                                     0x01
#define OCF_INQUIRY_CANCEL                              0x02
#define OCF_PERIODIC_INQUIRY                            0x03
#define OCF_OCF_CREATE_CONN                             0x05
#define OCF_READ_BD_ADDR                                0x09
#define OCF_READ_REMOTE_FEATURES                        0x1B
#define OCF_READ_REMOTE_VERSION                         0x1D
#define OCF_SET_AFH_HOST_CHANNEL_CLASSIFICATION         0x3F
#define OCF_RESET                                       0x03
#define OCF_ROLE_DISCOVERY                              0x09
#define EVT_INQUIRY_COMPLETE                            0x01
#define EVT_CONN_COMPLETE                               0x03
#define EVT_CONN_REQUEST                                0x04
#define EVT_DISCONN_COMPLETE                            0x05
#define EVT_READ_REMOTE_FEATURES_COMPLETE               0x0B
#define EVT_READ_REMOTE_VERSION_COMPLETE                0x0C
#define EVT_CMD_COMPLETE                                0x0E
#define EVT_ROLE_CHANGE                                 0x12
#define EVT_PIN_CODE_REQ                                0x16
#define EVT_LINK_KEY_NOTIFY                             0x18
#define EVT_SYNC_CONN_COMPLETE                          0x2C

#if (BLE_INCLUDED == TRUE)

#define OGF_BLE_CONTROLLER_CMD                          0x20
#define OCF_BLE_SET_EVT_MASK                            0x01
#define OCF_BLE_READ_BUFFER_SIZE                        0x02
#define OCF_BLE_LOCAL_SUPPORTED_FEAT                    0x03
#define OCF_BLE_SET_RAND_ADDR                           0x05
#define OCF_BLE_READ_REMOTE_USED_FEAT                   0x16
#define BLE_META_EVENT                                  0x3e
#define EVT_BLE_CONN_COMPLETE                           0x01
#define EVT_BLE_ADV_PKT_RPT                             0x02
#define EVT_BLE_LL_CONN_PARAM_UPD                       0x03
#define EVT_BLE_READ_REMOTE_FEAT_CMPL                   0x04
#define EVT_BLE_LTK_REQ                                 0x05
#define BLE_SUB_EVT_OFFSET                              0x03

#endif

#define OGF_LINK_CTL                                    0x04
#define OGF_LINK_POLICY                                 0x08
#define OGF_HOST_CTL                                    0x0c
#define OGF_INFO_PARAM                                  0x10
#define BTC_STACK_TO_HC_HCI_CMD                         0x2000
#define BTC_HC_TO_STACK_HCI_EVT                         0x1000
#define BTC_EVT_MASK                                    0xFF00
#define BTC_MAX_LEN                                     0xFF

#define EVT_OGF_OFFSET                                  0x05
#define EVT_OCF_OFFSET                                  0x04
#define EVT_CODE_OFFSET                                 0x01
#define CMD_OGF_OFFSET                                  0x02
#define CMD_OCF_OFFSET                                  0x01

typedef enum {
    BLUETOOTH_NONE = 0x00,
    BLUETOOTH_ON = 0x20,
    BLUETOOTH_OFF = 0x21,
    BLUETOOTH_DISCOVERY_STARTED = 0x22,
    BLUETOOTH_DISCOVERY_FINISHED = 0x23,
    BLUETOOTH_DEVICE_CONNECTED = 0x24,
    BLUETOOTH_DEVICE_DISCONNECTED = 0x25,
    BLUETOOTH_HEADSET_CONNECTED = 0x40,
    BLUETOOTH_HEADSET_DISCONNECTED = 0x41,
    BLUETOOTH_HEADSET_AUDIO_STREAM_STARTED = 0x42,
    BLUETOOTH_HEADSET_AUDIO_STREAM_STOPPED = 0x43,
    BLUETOOTH_AUDIO_SINK_CONNECTED = 0x60,
    BLUETOOTH_AUDIO_SINK_DISCONNECTED = 0x61,
    BLUETOOTH_SINK_STREAM_STARTED = 0x62,
    BLUETOOTH_SINK_STREAM_STOPPED = 0x63,
    BLUETOOTH_INPUT_DEVICE_CONNECTED = 0x80,
    BLUETOOTH_INPUT_DEVICE_DISCONNECTED = 0x81
}  btc_event_t;

void btc_init(void);
void btc_deinit(void);
void btc_hci_init(void);
void btc_hci_deinit(void);
void btc_post_msg(btc_event_t event);
void btc_capture (const BT_HDR *buffer, serial_data_type_t type);
#endif /*BTC_COMMON_H*/
