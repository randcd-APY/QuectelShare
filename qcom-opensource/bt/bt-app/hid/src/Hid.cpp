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

#include <list>
#include <map>
#include <iostream>
#include <string.h>
#include <hardware/bluetooth.h>
#include <hardware/hardware.h>
#include <hardware/bt_hh.h>
#include <algorithm>

#include "Hid.hpp"
#include "Gap.hpp"
#include "hardware/bt_rc_vendor.h"
#include "hardware/bt_hh_vendor.h"
using namespace std;
using std::list;
using std::string;

#define LOGTAG "HIDH"
#define LOGTAG_CTRL "HIDH_CTRL"

thread_t *hid_report_thread;
const char* threadName = "hid_report";
static const bt_bdaddr_t bd_addr_null= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
HidH *pHid = NULL;

using namespace std;
using std::list;
using std::string;

#ifdef __cplusplus
extern "C" {
#endif

void BtHidMsgHandler(void *msg) {
    ALOGD(LOGTAG " Process Event for BtHidMsgHandler");
    BtEvent* pEvent = NULL;
    BtEvent* pCleanupEvent = NULL;
    if(!msg) {
        printf("Msg is NULL, return.\n");
        return;
    }

    pEvent = ( BtEvent *) msg;
    switch(pEvent->event_id) {

        case PROFILE_API_START:
            ALOGD(LOGTAG " enable hid");
            if (pHid) {
                pHid->HandleEnableHID();
            }
            break;
        case PROFILE_API_STOP:
            ALOGD(LOGTAG " disable hid");
            if (pHid) {
                pHid->HandleDisableHID();
            }
            break;
        case HID_API_BONDED_HID_LIST:
            ALOGD(LOGTAG "add bonded list hid");
            if (pHid && !(pHid->isDeviceinHidList(pEvent->hid_profile_event.bd_addr))) {
                pHid->AddToHidList(HIDH_STATE_DISCONNECTED,&pEvent->hid_profile_event.bd_addr);
            }
            break;
        case HID_API_BONDED_LIST_REQ:
            ALOGD(LOGTAG "display bonded list hid");
            if (pHid) {
                pHid->HidPairedeDeviceList();
            }
            break;
        case HID_API_CONNECT_REQ:
            printf("HID_API_CONNECT_REQ\n");
            if(pHid){
                pHid->ProcessConnectRequest(pEvent);
            }
            break;
        case HID_API_DISCONNECT_REQ:
            printf("HID_API_DISCONNECT_REQ\n");
            if(pHid){
                pHid->ProcessDisconnectRequest(pEvent);
            }
            break;
        case HID_API_VIRTUAL_UNPLUG_REQ:
        case HID_API_GET_PROTOCOL_REQ:
        case HID_API_SET_PROTOCOL_REQ:
        case HID_API_GET_REPORT_REQ:
        case HID_API_SET_REPORT_REQ:
        if(pHid) {
            pHid->ProcessHidRequest(( BtEvent *) msg);
        }
            break;
        default:
            break;
    }
    delete pEvent;
}

#ifdef __cplusplus
}
#endif

static void connection_state_cb(bt_bdaddr_t *bd_addr, bthh_connection_state_t state) {
    BtEvent *pEvent = new BtEvent;
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    printf("Received Connection State Callback %d\n",state);

    memcpy(&pEvent->hid_profile_event.bd_addr, bd_addr, sizeof(bt_bdaddr_t));
    switch( state ) {
        case BTHH_CONN_STATE_CONNECTED:
        {
            printf("CONNECTED!!\n");
            if(!pHid->isDeviceinHidList(pEvent->hid_profile_event.bd_addr))
            {
                pHid->AddToHidList(HIDH_STATE_CONNECTED, &pEvent->hid_profile_event.bd_addr);
            }
        }
        break;
       case BTHH_CONN_STATE_CONNECTING:
        {
            //pHid->ChangeStateHidList(HIDH_STATE_CONNECTING,pEvent->hid_profile_event.bd_addr);
            printf("CONNECTING!!\n");
        }
        break;
        case BTHH_CONN_STATE_DISCONNECTING:
        {
            //printf("DISCONNECTING!!\n");
            pHid->ChangeStateHidList(HIDH_STATE_DISCONNECTING,pEvent->hid_profile_event.bd_addr);
        }
        break;
        case BTHH_CONN_STATE_DISCONNECTED:
        {
            printf("DISCONNECTED!!\n");
            if(pHid->isDeviceinHidList(pEvent->hid_profile_event.bd_addr))
            {
                pHid->RemoveHidList(pEvent->hid_profile_event.bd_addr);
            }
        }
        break;
        case BTHH_CONN_STATE_FAILED_MOUSE_FROM_HOST:
        case BTHH_CONN_STATE_FAILED_KBD_FROM_HOST:
        case BTHH_CONN_STATE_FAILED_TOO_MANY_DEVICES:
        case BTHH_CONN_STATE_FAILED_NO_BTHID_DRIVER:
        case BTHH_CONN_STATE_FAILED_GENERIC:
        case BTHH_CONN_STATE_UNKNOWN:
            printf("Unknown State of Hid Connection");
        break;
        default:
        break;
    }
    //PostMessage(THREAD_ID_HID, pEvent);
}

static void get_report_cb(bt_bdaddr_t *bd_addr, bthh_status_t hh_status, uint8_t *rpt_data, int rpt_size) {
    bdstr_t str;
    int i;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    //printf("Report length:: %d Status %d\n",rpt_size,hh_status);
    if((hh_status != BTHH_OK) || (rpt_size ==0 ) || (rpt_size > 15))/*Handle only Release Event, check for size*/
        return;
}

static void handshake_cb(bt_bdaddr_t *bd_addr, bthh_status_t hh_status){
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    if(hh_status !=   BTHH_OK )
        return;
    bdaddr_to_string(bd_addr, str, 18);
    fprintf(stdout, "handshake callback for device %s\n", str);
    ALOGD(LOGTAG "handshake for device %s ",str);
}

static void protocol_mode_cb(bt_bdaddr_t *bd_addr, bthh_status_t hh_status,bthh_protocol_mode_t mode) {
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    if(hh_status !=   BTHH_OK )
        return;
    bdaddr_to_string(bd_addr, str, 18);
    fprintf(stdout, "Protocol Mode for device %s\n", str);
    ALOGD(LOGTAG "Done! Protocol mode for device %s is: %d",str,mode);
    printf("Done! Protocol mode for device %s is: %d",str,mode);
}

static void virtual_unplug_cb(bt_bdaddr_t *bd_addr, bthh_status_t hh_status) {
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    BtEvent *pEvent = new BtEvent;
    pEvent->hid_profile_event.event_id = HID_API_DISCONNECT_REQ;
    printf("virtual unplug");
    if(hh_status !=   BTHH_OK )
        return;
    bdaddr_to_string(bd_addr, str, 18);
    fprintf(stdout, "virtual unplug callback for device %s\n", str);
    ALOGD(LOGTAG "Initiating disconnect for device %s ",str);
    PostMessage(THREAD_ID_HID, pEvent);
}

static bthh_callbacks_t sBluetoothHidCallbacks = {
    sizeof(sBluetoothHidCallbacks),
    connection_state_cb,
    NULL,
    protocol_mode_cb,
    NULL,
    get_report_cb,
    virtual_unplug_cb,
    handshake_cb
};

static void raw_hid_data_cb(uint8_t* rpt, uint16_t len,bool rpt_id_flag){
    ALOGD(LOGTAG " raw_hid_data_cb ");
    uint8_t *rpt_data;
    uint8_t rpt_id;
    uint8_t rpt_len;

    static bool toggle_play_pause = false;
    static bool toggle_mute = false;
    if (rpt_id_flag) {
        ALOGD(LOGTAG " raw_hid_data_cb : report contains report_id.");
        rpt_id = *(rpt);
        rpt_len = len-1;
        rpt_data = (uint8_t *)malloc((rpt_len)*sizeof(uint8_t));
        if (rpt_data == NULL) {
            ALOGD(LOGTAG " raw_hid_data_cb Memory not allocated");
            return;
        }
        memcpy(rpt_data,&rpt[1],(rpt_len)*sizeof(uint8_t));
        for (int i=0;i<rpt_len;i++)
            ALOGD(LOGTAG "raw_hid_data_cb : data at idx %d is %d",i,*(rpt_data+i));
    }
    else{
        rpt_len = len;
        ALOGD(LOGTAG "raw_hid_data_cb :report does not contain report_id.");
        rpt_data = (uint8_t *)malloc((rpt_len)*sizeof(uint8_t));
        if (rpt_data == NULL) {
             ALOGD(LOGTAG " raw_hid_data_cb Memory not allocated");
             return;
        }
        memcpy(rpt_data,&rpt[1],rpt_len*sizeof(uint8_t));
        for (int i=0;i<rpt_len;i++)
            ALOGD(LOGTAG "raw_hid_data_cb : data at idx %d is %d",i,*(rpt_data+i));
    }

    if(*(rpt_data) == 0 && *(rpt_data+1) == 0 && *(rpt_data+2) == 0){
        ALOGD(LOGTAG " raw_hid_data_cb : KEYPRESS RELEASE");
        fprintf(stdout,"KEYPRESS RELEASE");
    }
    else if (*(rpt_data) == 16 && *(rpt_data+1) == 0 && *(rpt_data+2) == 0){
        if(!toggle_play_pause){
            toggle_play_pause = true;
            ALOGD(LOGTAG " raw_hid_data_cb : PLAY");
            fprintf(stdout,"PLAY");
        }else{
            toggle_play_pause = false;
            ALOGD(LOGTAG " raw_hid_data_cb : PAUSE");
            fprintf(stdout,"PAUSE");
        }
    }
    else if(*(rpt_data) == 32 && *(rpt_data+1) == 0 && *(rpt_data+2) == 0){
        if(!toggle_mute){
            ALOGD(LOGTAG " raw_hid_data_cb : MUTE");
            fprintf(stdout,"MUTE");
            toggle_mute = true;
        }
        else{
            ALOGD(LOGTAG " raw_hid_data_cb : UNMUTE");
            fprintf(stdout,"UNMUTE");
            toggle_mute = false;
        }
    }
    else if(*(rpt_data) == 128 && *(rpt_data+1) == 0 && *(rpt_data+2) == 0){
        ALOGD(LOGTAG " raw_hid_data_cb : VOL DOWN");
        fprintf(stdout,"VOL DOWN");
    }
    else if(*(rpt_data) == 64 && *(rpt_data+1) == 0 && *(rpt_data+2) == 0){
        ALOGD(LOGTAG " raw_hid_data_cb : VOL UP");
        fprintf(stdout,"VOL UP");
    }
    else{
        ALOGD(LOGTAG " raw_hid_data_cb : Unknown Cmd");
    }
}


static bthh_vendor_callbacks_t sBtHhVendorCallbacks = {
    sizeof(sBtHhVendorCallbacks),
    raw_hid_data_cb,
};

/*Constructor and Destructor Structure of the Class*/
HidH::HidH(const bt_interface_t *bt_interface, config_t *config)
{
    ALOGD(LOGTAG "(%s) Starting Up HID Instance",__FUNCTION__);
    pthread_mutex_init(&this->lock, NULL);
    this->sBluetoothHidInterface = NULL;
    this->bluetooth_interface = bt_interface;
    this->config = config;
    hid_report_thread = thread_new (threadName);
    hid_list.clear();

}

HidH::~HidH()
{
    ALOGD(LOGTAG  "(%s) Cleaning up HID Interface",__FUNCTION__);
    hid_list.clear();
    pthread_mutex_destroy(&lock);
}

/*****************************************************************/
void HidH::HandleEnableHID(void) {
    BtEvent *pEvent = new BtEvent;
    ALOGD(LOGTAG "%s HandleEnableHID ",__func__);
    if (bluetooth_interface != NULL)
    {
        // HID Initialization
        sBluetoothHidInterface = (bthh_interface_t *)bluetooth_interface->
                get_profile_interface(BT_PROFILE_HIDHOST_ID);

        if (sBluetoothHidInterface != NULL) {
            sBluetoothHidInterface->init(&sBluetoothHidCallbacks);
        }

        // Vendor interface
        sBtHhVendorInterface = (bthh_vendor_interface_t *)bluetooth_interface->
                                get_profile_interface(BT_PROFILE_HID_VENDOR_ID);
        if (sBtHhVendorInterface != NULL) {
            ALOGD(LOGTAG" Added vendor inteface in HidH");
            sBtHhVendorInterface->init_vendor(&sBtHhVendorCallbacks);
        }
        else{
            ALOGD(LOGTAG " vendor interface NULL");
        }

        ALOGD(LOGTAG "%s Enable HID  ",__func__);
        pEvent->profile_start_event.event_id = PROFILE_EVENT_START_DONE;
        pEvent->profile_start_event.profile_id = PROFILE_ID_HID;
        pEvent->profile_start_event.status = true;
        PostMessage(THREAD_ID_GAP, pEvent);
    }
}

void HidH::HandleDisableHID(void) {
    ALOGD(LOGTAG_CTRL " HandleDisableHID ");
   if (sBluetoothHidInterface != NULL) {
       sBluetoothHidInterface->cleanup();
       sBluetoothHidInterface = NULL;
   }
   if (sBtHhVendorInterface != NULL) {
        sBtHhVendorInterface->cleanup_vendor();
         sBtHhVendorInterface = NULL;
   }
   BtEvent *pEvent = new BtEvent;
   pEvent->profile_stop_event.event_id = PROFILE_EVENT_STOP_DONE;
   pEvent->profile_stop_event.profile_id = PROFILE_ID_HID;
   pEvent->profile_stop_event.status = true;
   PostMessage(THREAD_ID_GAP, pEvent);
}

void HidH::ProcessConnectRequest(BtEvent* pEvent) {
    if(isDeviceinHidList(pEvent->hid_profile_event.bd_addr)) //Device in HID List
    {
        list<HidDevice>::iterator iter;
        iter = FindDeviceByAddr(hid_list, pEvent->hid_profile_event.bd_addr);
        printf("Check State Issuing Connect request \n");
        if(iter != hid_list.end() && iter->state!= HIDH_STATE_CONNECTED){
            if (sBluetoothHidInterface != NULL) {
                printf("Send Connect\n");
                sBluetoothHidInterface->connect(&pEvent->hid_profile_event.bd_addr);
            }
        }
        printf("No Connect as State is Already Connected\n");
        //check the state ..if not Connected then issue connect
    }
    else
    {
        printf("Not in Hid List..Issue Connect\n");
        if (sBluetoothHidInterface != NULL) {
            sBluetoothHidInterface->connect(&pEvent->hid_profile_event.bd_addr);
        }
    }
}

void HidH::ProcessDisconnectRequest(BtEvent* pEvent) {
    if(isDeviceinHidList(pEvent->hid_profile_event.bd_addr)) //Device in HID List
    {
        list<HidDevice>::iterator iter;
        iter = FindDeviceByAddr(hid_list, pEvent->hid_profile_event.bd_addr);
        printf("Check State Issuing Disconnect request \n");
        if(iter != hid_list.end() && iter->state!= HIDH_STATE_DISCONNECTED){
            if (sBluetoothHidInterface != NULL) {
                sBluetoothHidInterface->disconnect(&pEvent->hid_profile_event.bd_addr);
            }
        }
        printf("No Disconnect as State is Already Connected\n");
        //check the state ..if not Connected then issue connect
    }
    else
    {
        printf("Not in Hid List..Issue Disconnect");
        if (sBluetoothHidInterface != NULL) {
            sBluetoothHidInterface->disconnect(&pEvent->hid_profile_event.bd_addr);
        }
    }
}

void HidH::ProcessHidRequest(BtEvent* pEvent){
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    if(isDeviceinHidList(pEvent->hid_profile_event.bd_addr)) //Device in HID List
    {
        list<HidDevice>::iterator iter;
        iter = FindDeviceByAddr(hid_list, pEvent->hid_profile_event.bd_addr);
        if(iter != hid_list.end() && iter->state== HIDH_STATE_CONNECTED){
            switch(pEvent->event_id){
                case HID_API_VIRTUAL_UNPLUG_REQ:
                    if (sBluetoothHidInterface != NULL) {
                        sBluetoothHidInterface->virtual_unplug(&pEvent->hid_profile_event.bd_addr);
                    }
                    bdaddr_to_string(&pEvent->hid_profile_event.bd_addr, str, 18);
                    fprintf(stdout, "Handling Virtual Unplug for HID device %s\n", str);
                    ALOGD(LOGTAG "Sending Disconnect for device %s", str);
                    break;
                case HID_API_GET_PROTOCOL_REQ:
                    printf("Get Protocol Mode to %d ",pEvent->hid_profile_event.protocolMode);
                    if (sBluetoothHidInterface != NULL) {
                        sBluetoothHidInterface->get_protocol(&pEvent->hid_profile_event.bd_addr,(bthh_protocol_mode_t)pEvent->hid_profile_event.protocolMode);
                    }
                    break;
                case HID_API_SET_PROTOCOL_REQ:
                    printf("Set Protocol Mode to %d ",pEvent->hid_profile_event.protocolMode);
                    if (sBluetoothHidInterface != NULL) {
                        sBluetoothHidInterface->set_protocol(&pEvent->hid_profile_event.bd_addr,(bthh_protocol_mode_t)pEvent->hid_profile_event.reportType);
                    }
                    break;
                case HID_API_GET_REPORT_REQ:
                    if (sBluetoothHidInterface != NULL) {
                    sBluetoothHidInterface->get_report(&pEvent->hid_profile_event.bd_addr,(bthh_report_type_t)pEvent->hid_profile_event.reportType   \
                            ,pEvent->hid_profile_event.reportID,pEvent->hid_profile_event.bufSize);
                    }
                    bdaddr_to_string(&pEvent->hid_profile_event.bd_addr, str, 18);
                    fprintf(stdout, "Handling GET RPT for HID device %s\n", str);
                    ALOGD(LOGTAG "Sending GET Report for device%s", str);
                    break;
                case HID_API_SET_REPORT_REQ:
                    printf("HID_API_SET_REPORT_REQ %s\n" ,pEvent->hid_profile_event.report);
                    if (sBluetoothHidInterface != NULL) {
                        sBluetoothHidInterface->set_report(&pEvent->hid_profile_event.bd_addr,(bthh_report_type_t)pEvent->hid_profile_event.reportType,pEvent->hid_profile_event.report);
                    }
                    bdaddr_to_string(&pEvent->hid_profile_event.bd_addr, str, 18);
                    fprintf(stdout, "Handling SET RPT for HID device %s\n", str);
                    ALOGD(LOGTAG "Sending SET Report for device%s", str);
                    break;
            }

       }
   }
}

list<HidDevice>::iterator HidH::FindDeviceByAddr(list<HidDevice>& phidDev, bt_bdaddr_t dev) {
    bdstr_t str, devstr;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    strlcpy(devstr, bdaddr_empty, MAX_BD_STR_LEN);
    bdaddr_to_string(&dev, devstr, 18);
    list<HidDevice>::iterator p = hid_list.begin();
    while(p != hid_list.end()) {
        bdaddr_to_string(&p->addr, str, 18);
        //printf(" BT FindDeviceByAddr:: %s  State:: %d\n",str,p->state);
        if (!strcmp(str,devstr))
        {
            break;
        }
        ++p;
    }
    return p;
}

bool HidH::isDeviceinHidList(bt_bdaddr_t dev) {
    bdstr_t str, devstr;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    strlcpy(devstr, bdaddr_empty, MAX_BD_STR_LEN);
    bdaddr_to_string(&dev, devstr, 18);
    if(hid_list.size()!= 0)
    {
        list<HidDevice>::iterator p = hid_list.begin();
        while(p != hid_list.end()) {
            bdaddr_to_string(&p->addr, str, 18);
                if (!strcmp(str,devstr))
                {
                    return true;
                }
            ++p;
        }
    }
    return false;
}

void HidH::AddToHidList(HIDConnectiontState state, bt_bdaddr_t *addr)
{
    HidDevice p;
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    memcpy(&p.addr,addr,sizeof(bt_bdaddr_t));
    p.state = state;
    hid_list.push_back(p);
    bdaddr_to_string(&p.addr, str, 18);
    printf("Pushed paired Device %s to HID List to state %d\n",str,p.state);
}

void HidH::HidPairedeDeviceList()
{
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    if(hid_list.empty())
    {
        printf("Hid List Empty!\n");
        return;
    }
    printf("Size of HID List %d\n",hid_list.size());
    list<HidDevice>::iterator p = hid_list.begin();
    while(p != hid_list.end()) {
        bdaddr_to_string(&p->addr, str, 18);
        printf(" BT Address:: %s  State:: %d\n",str,p->state);
        ++p;
    }
}

void HidH::RemoveHidList(bt_bdaddr_t dev)
{
    bdstr_t str;
    strlcpy(str, bdaddr_empty, MAX_BD_STR_LEN);
    if(!pHid->isDeviceinHidList(dev)) //false
    {
        //printf("Device Not in HID List\n");
        return;
    }
    else
    {
        list<HidDevice>::iterator p = FindDeviceByAddr(hid_list ,dev);
        if(p != hid_list.end())
        {
            bdaddr_to_string(&p->addr, str, 18);
            //printf("RemoveHidList Addr:: %s \n",str);
            hid_list.erase(p);
        }
    }
}

void HidH::ChangeStateHidList(HIDConnectiontState hidState, bt_bdaddr_t addr)
{
    list<HidDevice>::iterator iter;
    iter = FindDeviceByAddr(pHid->hid_list, addr);
    if(iter != pHid->hid_list.end()){
        iter->state = hidState;
    }
}
