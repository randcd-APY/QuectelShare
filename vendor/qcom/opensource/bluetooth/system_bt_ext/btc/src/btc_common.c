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

/******************************************************************************
 *
 *  This file contains action functions for the BT and WLAN coex.
 *
 ******************************************************************************/

#define LOG_TAG "bt_btc"

#include "bta_api.h"
#include "hcimsgs.h"
#include <cutils/log.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include "osi/include/thread.h"
#include "osi/include/reactor.h"
#include "osi/include/log.h"
#include "osi/include/fixed_queue.h"
#include "btc_common.h"
#include <string.h>
#include <errno.h>

#define INVALID_SOCK  (-1)
#define STACK_SOCKET_PATH  "/etc/bluetooth/qcom.btc.server"
#define HCI_SOCKET_PATH    "/etc/bluetooth/btc_hci"

static thread_t *btc_thread = NULL;
static int hci_sock = INVALID_SOCK;
static int hci_listen_sock = INVALID_SOCK;
static int stack_sock = INVALID_SOCK;
static int stack_listen_sock = INVALID_SOCK;
static fixed_queue_t *pkt_queue = NULL;
static reactor_object_t *stack_reactor = NULL;
static reactor_object_t *hci_reactor = NULL;
static reactor_object_t *hci_data_reactor = NULL;

static int local_sock_create(const char *path);
static void btc_stack_listen_handler(void *context);
static void btc_hci_listen_handler(void *context);
static void btc_process_client_command(char *hci_cmd, int len);
static void event_packet_ready(fixed_queue_t *queue, void *context);


void btc_init()
{
    btc_thread = thread_new("btc");
    if (!btc_thread) {
        LOG_ERROR("%s unable to create btc thread", __func__);
        return;
    }
    // initialize server sockets
    if ((stack_listen_sock = local_sock_create(STACK_SOCKET_PATH)) == -1) {
        LOG_ERROR("%s unable to create server socket for %s", __func__, STACK_SOCKET_PATH);
    } else {
        LOG_DEBUG("%s created server socket for %s", __func__, STACK_SOCKET_PATH);
        stack_reactor = reactor_register(
            thread_get_reactor(btc_thread),
            stack_listen_sock, NULL, btc_stack_listen_handler, NULL);
    }

    pkt_queue = fixed_queue_new(SIZE_MAX);
    if (!pkt_queue) {
        LOG_ERROR("%s unable to create pending command queue.", __func__);
    }
}

void btc_deinit()
{
    int status;

    LOG_VERBOSE("%s: BTC DE-INIT ", __func__);

    if (stack_sock > 0) {
        LOG_VERBOSE("BTC_Denit:stack sock id closing %d", stack_sock);
        close(stack_sock);
        stack_sock = INVALID_SOCK;
    }

    if (stack_listen_sock > 0) {
        LOG_VERBOSE("BTC_Denit: stack_listen_sock closing %d", stack_listen_sock);
        close(stack_listen_sock);
        stack_listen_sock = INVALID_SOCK;
    }

    if (stack_reactor) {
        reactor_unregister(stack_reactor);
        stack_reactor = NULL;
    }

    if (btc_thread) {
        // stop the btc thread
        thread_stop(btc_thread);
        thread_join(btc_thread);
    }
    if (pkt_queue) {
        fixed_queue_free(pkt_queue, NULL);
        pkt_queue = NULL;
    }
    // free the btc thread
    thread_free(btc_thread);
    btc_thread = NULL;
}

void btc_hci_init()
{
    if ((hci_listen_sock = local_sock_create(HCI_SOCKET_PATH)) == -1) {
        LOG_ERROR("%s unable to create server socket for %s", __func__, HCI_SOCKET_PATH);
    } else if (btc_thread) {
        LOG_DEBUG("%s created server socket for %s", __func__, HCI_SOCKET_PATH);
        hci_reactor = reactor_register(
            thread_get_reactor(btc_thread),
            hci_listen_sock, NULL, btc_hci_listen_handler, NULL);
        if (pkt_queue) {
            fixed_queue_register_dequeue(pkt_queue, thread_get_reactor(btc_thread),
                event_packet_ready, NULL);
        }
    }
}

void btc_hci_deinit()
{
    // close all sockets
    if (hci_sock > 0) {
        LOG_VERBOSE("BTC_Denit:HCI sock id closing %d", hci_sock);
        close(hci_sock);
        hci_sock = INVALID_SOCK;
    }

    if (hci_listen_sock > 0) {
        LOG_VERBOSE("BTC_Denit: hci_listen_sock closing %d", hci_listen_sock);
        close(hci_listen_sock);
        hci_listen_sock = INVALID_SOCK;
    }
    if (hci_reactor) {
        reactor_unregister(hci_reactor);
        hci_reactor = NULL;
    }
    if (hci_data_reactor) {
        reactor_unregister(hci_data_reactor);
        hci_data_reactor = NULL;
    }
}


void btc_capture (const BT_HDR *buffer, serial_data_type_t type)
{
    if (pkt_queue && (hci_sock > 0)) {
        BT_HDR *btc_buf = (BT_HDR *)osi_malloc(buffer->len + buffer->offset + sizeof(BT_HDR) + 1 );
        memcpy(btc_buf, buffer, sizeof(BT_HDR));
        btc_buf->len = buffer->len + 1;
        btc_buf->offset = buffer->offset;
        btc_buf->data[buffer->offset] = (uint8_t )type; // store the type
        memcpy(&btc_buf->data[buffer->offset + 1], &buffer->data[buffer->offset], buffer->len);

        fixed_queue_enqueue(pkt_queue, btc_buf);
    }
}

static int local_sock_create(const char *path)
{
    int local_sock, length;
    struct sockaddr_un addr;

    local_sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (local_sock < 0) {
        LOG_ERROR("Failed to create Local Socket 1 (%s)", strerror(errno));
        return INVALID_SOCK;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strlcpy(addr.sun_path, path, sizeof(addr.sun_path));
    unlink(path);
    if (bind(local_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        LOG_ERROR("Failed to create Local Socket (%s)", strerror(errno));
        return INVALID_SOCK;
    }

    if (listen(local_sock, 1) < 0) {
        LOG_ERROR("Local socket listen failed (%s)", strerror(errno));
        close(local_sock);
        return INVALID_SOCK;
    }
    return local_sock;
}

static void btc_hci_data_handler(void *context)
{
    int len;
    char event[BTC_MAX_LEN] = { 0 };

    if (hci_sock != INVALID_SOCK) {
        len = recv(hci_sock, event, BTC_MAX_LEN, 0);

        if (len <= 0) {
            LOG_ERROR("Not able to receive msg to remote dev: %s", strerror(errno));
            reactor_unregister(hci_data_reactor);
            close(hci_sock);
            hci_sock = INVALID_SOCK;
            hci_data_reactor = NULL;
        } else {
            LOG_VERBOSE("Success: reading from HCI socket %d", len);
            btc_process_client_command(event, len);
        }
    }
}

static void btc_hci_listen_handler(void *context)
{
    struct sockaddr_un cliaddr;
    int length = sizeof(cliaddr);

    if (hci_sock == INVALID_SOCK) {
        hci_sock = accept(hci_listen_sock,
                  (struct sockaddr*)&cliaddr, ( socklen_t*)&length);
        if (hci_sock == INVALID_SOCK) {
            LOG_ERROR("%s error accepting LOCAL socket: %s",
                  __func__, strerror(errno));
        } else {
            LOG_VERBOSE(" %s  accepted hci_sock is %d\n", __func__, hci_sock);
            hci_data_reactor = reactor_register
                           (thread_get_reactor(btc_thread),
                           hci_sock, NULL, btc_hci_data_handler, NULL);
        }
    } else {
        LOG_ERROR(" %s Accepting and closing the next connection \n", __func__);
        int accept_socket = accept(hci_listen_sock,
                       (struct sockaddr*)&cliaddr, ( socklen_t*)&length);
        if (accept_socket)
            close(accept_socket);
    }
}

static void btc_stack_listen_handler(void *context)
{
    struct sockaddr_un cliaddr;
    int length = sizeof(cliaddr);

    if (stack_sock == INVALID_SOCK) {
        stack_sock = accept(stack_listen_sock,
                    (struct sockaddr*)&cliaddr, ( socklen_t*)&length);
        if (stack_sock == INVALID_SOCK) {
            LOG_ERROR("%s error accepting LOCAL socket: %s",
                  __func__, strerror(errno));
        } else {
            LOG_DEBUG(" %s  accepted stack_sock is %d\n", __func__, stack_sock);
        }
    } else {
        LOG_ERROR(" %s Accepting and closing the next connection .\n", __func__);
        int accept_socket = accept(stack_listen_sock,
                       (struct sockaddr*)&cliaddr, ( socklen_t*)&length);
        if (accept_socket)
            close(accept_socket);
    }
}

static void update_stack_events(void *context)
{
    int status = -1;
    btc_event_t event,  orig_event = (btc_event_t)context;

    if (stack_sock > 0) {
        event = htonl(orig_event);
        status = write(stack_sock, &event, sizeof(event));
    }
    if (BLUETOOTH_ON == orig_event) {
        LOG_DEBUG(" %s starting abtfilter \n", __func__);
        property_set_bt("bluetooth.isEnabled", "true");
    } else if (BLUETOOTH_OFF == orig_event) {
        LOG_DEBUG(" %s stopping abtfilter \n", __func__);
        property_set_bt("bluetooth.isEnabled", "false");
    }
}

void btc_post_msg(btc_event_t event)
{
    if (btc_thread) {
        thread_post(btc_thread, update_stack_events, (void*)event);
    }
}

static void btc_send_cmd_complete(char *hci_evt, int len)
{
    int status = -1;

    switch (hci_evt[EVT_OGF_OFFSET]) {
        case OGF_HOST_CTL:
            if (OCF_SET_AFH_HOST_CHANNEL_CLASSIFICATION == hci_evt[EVT_OCF_OFFSET]) {
                LOG_VERBOSE("btc_process_event%x OGF %x OCF%x", hci_evt[1],
                        hci_evt[5], hci_evt[4]);
            } else {
                return;
            }
            break;
        case OGF_LINK_POLICY:
            if (OCF_ROLE_DISCOVERY != hci_evt[EVT_OCF_OFFSET]) {
                return;
            } else {
                LOG_VERBOSE("%s: read  OCF_ROLE_DISCOVERY ", __func__);
            }
            break;
        case OGF_INFO_PARAM:
            if (OCF_READ_BD_ADDR == hci_evt[EVT_OCF_OFFSET]) {
                LOG_VERBOSE("btc_process_event   %x OGF %x OCF%x",
                        hci_evt[1], hci_evt[5], hci_evt[4]);
                LOG_VERBOSE("%s: Read BD_ADDRESS ", __func__);
            } else {
                return;
            }
            break;
        default:
            return;
    }

    if (hci_sock > 0) {
        status = write(hci_sock, hci_evt, len);
        if (status < 0) {
            LOG_ERROR("sock write failed");
        }
    }
}

static void btc_process_event(char *hci_evt, int len)
{
    int status = -1;

    switch (hci_evt[EVT_CODE_OFFSET]) {
        case EVT_INQUIRY_COMPLETE:
        case EVT_PIN_CODE_REQ:
        case EVT_LINK_KEY_NOTIFY:
        case EVT_CONN_COMPLETE:
        case EVT_CONN_REQUEST:
        case EVT_SYNC_CONN_COMPLETE:
        case EVT_READ_REMOTE_FEATURES_COMPLETE:
        case EVT_READ_REMOTE_VERSION_COMPLETE:
        case EVT_DISCONN_COMPLETE:
        case EVT_ROLE_CHANGE:
            LOG_VERBOSE("%s EventCode : 0x%02X", __func__, hci_evt[EVT_CODE_OFFSET]);

#if (BLE_INCLUDED == TRUE)
        case BLE_META_EVENT:
            switch(hci_evt[BLE_SUB_EVT_OFFSET]) {
                case EVT_BLE_CONN_COMPLETE:
                case EVT_BLE_ADV_PKT_RPT:
                case EVT_BLE_LL_CONN_PARAM_UPD:
                case EVT_BLE_READ_REMOTE_FEAT_CMPL:
                case EVT_BLE_LTK_REQ:
                    LOG_VERBOSE("%s LE eventCode : 0x%02X LE SubEventCode : 0x%02X", __func__,
                        hci_evt[EVT_CODE_OFFSET], hci_evt[BLE_SUB_EVT_OFFSET]);
                default:
                    break;
            }
#endif
            if (hci_sock > 0) {
                status = write(hci_sock, hci_evt, len);
                if (status < 0) {
                    LOG_VERBOSE("sock write failed");
                }
            }
            break;
        case EVT_CMD_COMPLETE:
            btc_send_cmd_complete(hci_evt, len);
            break;
        default:
            break;
    }
}

static void btc_process_client_command(char *hci_cmd, int len)
{
    int fd = INVALID_SOCK;
    UINT16 hci_handle = 0;

    if (hci_cmd[CMD_OGF_OFFSET] == OGF_INFO_PARAM) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_READ_BD_ADDR) {
            LOG_VERBOSE("HCI  READ_BD_ADDR Command  %x", hci_cmd[2]);
            BTA_DmHciRawCommand(HCI_READ_BD_ADDR, hci_cmd[3], (UINT8*)&hci_cmd[3], NULL);
        } else {
            return;
        }
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_HOST_CTL) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_SET_AFH_HOST_CHANNEL_CLASSIFICATION) {
            LOG_VERBOSE("SET_AFH_CLASS %x", hci_cmd[2]);
            BTA_DmHciRawCommand(HCI_SET_AFH_CHANNELS, hci_cmd[3], (UINT8*)&hci_cmd[4], NULL);
        } else {
            return;
        }
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_LINK_CTL) {
        if ((hci_cmd[CMD_OCF_OFFSET] == OCF_READ_REMOTE_VERSION)) {
            LOG_VERBOSE(" BTC OCF_READ_REMOTE_VERSION OGF  %x %x %x %x %x %x ",
                  hci_cmd[0], hci_cmd[1], hci_cmd[2],
                  hci_cmd[3], hci_cmd[4], hci_cmd[5]);
            BTA_DmHciRawCommand(HCI_READ_RMT_VERSION_INFO, hci_cmd[3],
                        (UINT8*)&hci_cmd[4], NULL);
        } else {
            return;
        }
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_LINK_POLICY) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_ROLE_DISCOVERY) {
            LOG_VERBOSE("HCI OCF_ROLE_DISCOVERY Command  %x", hci_cmd[2]);
            BTA_DmHciRawCommand(HCI_ROLE_DISCOVERY, hci_cmd[3], (UINT8*)&hci_cmd[4], NULL);
        } else {
            return;
        }
#if (BLE_INCLUDED == TRUE)
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_BLE_CONTROLLER_CMD) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_BLE_READ_REMOTE_USED_FEAT) {
            LOG_VERBOSE("BLE Read Remote Used Features Command");
            BTA_DmHciRawCommand (HCI_BLE_READ_REMOTE_FEAT, hci_cmd[3], (UINT8 *)&hci_cmd[4], NULL);
        } else {
            return;
        }
#endif
    } else {
        LOG_VERBOSE("Not processing this command OGF  %x", hci_cmd[2]);
        return;
    }
}

static void btc_process_stack_command(char *hci_cmd, int len)
{
    int fd = INVALID_SOCK;
    int status = -1;

    if (hci_cmd[CMD_OGF_OFFSET] == OGF_LINK_CTL) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_INQUIRY) {
            LOG_VERBOSE("INQUIRY %x", hci_cmd[2]);
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_INQUIRY_CANCEL) {
            LOG_VERBOSE("INQUIRY CAN %x", hci_cmd[2]);
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_PERIODIC_INQUIRY) {
            LOG_VERBOSE("PERI INQUIRY %x", hci_cmd[2]);
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_OCF_CREATE_CONN) {
            LOG_VERBOSE("CREATE CONN %x", hci_cmd[2]);
        } else {
            return;
        }
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_HOST_CTL) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_RESET) {
            LOG_VERBOSE("RESET %x", hci_cmd[2]);
        } else {
            return;
        }
#if (BLE_INCLUDED == TRUE)
    } else if (hci_cmd[CMD_OGF_OFFSET] == OGF_BLE_CONTROLLER_CMD) {
        if (hci_cmd[CMD_OCF_OFFSET] == OCF_BLE_SET_EVT_MASK) {
            LOG_VERBOSE("BLE Set Event Mask Command");
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_BLE_READ_BUFFER_SIZE) {
            LOG_VERBOSE("BLE Read Buffer Size Command");
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_BLE_LOCAL_SUPPORTED_FEAT) {
            LOG_VERBOSE("BLE Local Supported Feature Command");
        } else if (hci_cmd[CMD_OCF_OFFSET] == OCF_BLE_SET_RAND_ADDR) {
            LOG_VERBOSE("BLE Set Random Address Command");
        } else {
            return;
        }
#endif
    } else {
        return;
    }

    if (hci_sock > 0) {
        status = write(hci_sock, hci_cmd, len);
    }
}

static void event_packet_ready(fixed_queue_t *queue, void *context)
{
    BT_HDR *buffer = fixed_queue_dequeue(queue);
    const uint8_t *p = NULL;

    if ((hci_sock == INVALID_SOCK) || !buffer) {
        return;
    }

    p = buffer->data + buffer->offset;

    switch (buffer->event & MSG_EVT_MASK) {
        case MSG_HC_TO_STACK_HCI_EVT:
            btc_process_event((char*)p, buffer->len);
            break;
        case MSG_STACK_TO_HC_HCI_CMD:
            btc_process_stack_command((char*)p, buffer->len);
            break;
        default:
            LOG_VERBOSE("%s :Default case", __func__);
            break;
    }
    osi_free(buffer);
}
