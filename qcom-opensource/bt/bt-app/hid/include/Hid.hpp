 /*
  * Copyright (c) 2017, The Linux Foundation. All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are
  * met:
  *  * Redistributions of source code must retain the above copyright
  *    notice, this list of conditions and the following disclaimer.
  *  * Redistributions in binary form must reproduce the above
  *    copyright notice, this list of conditions and the following
  *    disclaimer in the documentation and/or other materials provided
  *    with the distribution.
  *  * Neither the name of The Linux Foundation nor the names of its
  *    contributors may be used to endorse or promote products derived
  *    from this software without specific prior written permission.
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
  */

#ifndef HIDH_APP_H
#define HIDH_APP_H

#include <map>
#include <string>
#include <hardware/bluetooth.h>
#include <hardware/bt_rc.h>
#include <pthread.h>


#include "osi/include/thread.h"
#include "osi/include/reactor.h"
#include "osi/include/alarm.h"
#include "osi/include/config.h"
#include "ipc.h"
#include "utils.h"
#include "hardware/bt_rc_vendor.h"
#include "hardware/bt_hh.h"
#include <list>
#include "hardware/bt_hh_vendor.h"

using namespace std;
using std::list;
using std::string;

typedef enum {
    HIDH_STATE_IDLE = 0,
    HIDH_STATE_DISCONNECTED,
    HIDH_STATE_DISCONNECTING,
    HIDH_STATE_CONNECTING,
    HIDH_STATE_CONNECTED,
    HIDH_STATE_UNKNOWN
}HIDConnectiontState;

typedef struct HidDevice{
    HIDConnectiontState state;
    bt_bdaddr_t addr;
}HidDevice;

class HidH {
  private:
    config_t *config;
    const bt_interface_t * bluetooth_interface;
    const bthh_interface_t *sBluetoothHidInterface;
    /***  structure object for hh Vendor interface*/
    const bthh_vendor_interface_t *sBtHhVendorInterface;
    HIDConnectiontState mConnectionState;
    bt_bdaddr_t mConnectingDevice;
    bt_bdaddr_t mConnectedDevice;
  protected:
    list<HidDevice> hid_list;
  public:
    HidH(const bt_interface_t *bt_interface, config_t *config);
    ~HidH();
    void HandleEnableHID(void);
    void HandleDisableHID(void);
    void ProcessConnectRequest(BtEvent* pEvent);
    void ProcessDisconnectRequest(BtEvent* pEvent);
    void ProcessHidRequest(BtEvent* pEvent);
    void AddToHidList(HIDConnectiontState state, bt_bdaddr_t *addr);
    list<HidDevice>::iterator FindDeviceByAddr(list<HidDevice>& phidDev, bt_bdaddr_t dev);
    void HidPairedeDeviceList();
    void RemoveHidList(bt_bdaddr_t dev);
    bool isDeviceinHidList(bt_bdaddr_t dev);
    void ChangeStateHidList(HIDConnectiontState hidState, bt_bdaddr_t addr);
    pthread_mutex_t lock;
};
#endif
