/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
 */

#ifndef GATTCTEST_APP_H
#define GATTCTEST_APP_H

#pragma once

#include <hardware/bluetooth.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "osi/include/log.h"
#include "osi/include/thread.h"
#include "osi/include/config.h"
#include "ipc.h"
#include "Gatt.hpp"

#define RSP_MIN_CI           (100)
#define RSP_MAX_CI           (1000)

#define GATT_PROP_READ       (0x02)
#define GATT_PROP_WRITE      (0x08)

#define GATT_PERM_READ       (0x01)
#define GATT_PERM_WRITE      (0x10)

#define LOGTAG "GATTCTEST "

typedef enum {
   LOW_ALERT=0,
   MID_ALERT,
   HIGH_ALERT,
}AlertLevel;

typedef struct{
    int conn_id;
    int handle;
    btgatt_srvc_id_t *srvc_id;
    btgatt_gatt_id_t *char_id;
    btgatt_gatt_id_t *descr_id;
    bt_bdaddr_t* bda;
} ServiceData;


class Gatt;
class GattcTest {
    private:
        config_t *config;
        int wlan_state;

        RspEnableEvent attr;
        GattcRegisterAppEvent app_client_if;
        GattsRegisterAppEvent app_if;
        GattsServiceAddedEvent srvc_data;
        GattsCharacteristicAddedEvent char_data;
        GattsDescriptorAddedEvent desc_data;
        GattsConnectionEvent conn_data;
        GattcOpenEvent client_conn_data;
        btgatt_interface_t *gatt_interface;
     public:
        Gatt *app_gatt;
/***************************/

    public:
        GattcTest(btgatt_interface_t *, Gatt *);
        ~GattcTest();

        bool EnableGATTCTEST();
        bool DisableGATTCTEST();
        inline btgatt_interface_t* GetGattInterface()
        {
            return gatt_interface;
        }
        inline int GetDeviceState()
        {
            fprintf(stdout, "(%s) WLAN Current State (%d) \n",__FUNCTION__, wlan_state);
            return wlan_state;
        }
        inline void SetDeviceState(int currentstate)
        {
            fprintf(stdout, "(%s) WLAN Prev State (%d) New State(%d) \n"
                   ,__FUNCTION__, wlan_state, currentstate);
            wlan_state = currentstate;
        }
        inline void SetGATTCTESTClientAppData(GattcRegisterAppEvent *event)
        {
            memset(&app_client_if, 0, sizeof(app_client_if));
            memcpy(&app_client_if, event, sizeof(GattcRegisterAppEvent));
        }
        inline GattcRegisterAppEvent* GetGATTCTESTClientAppData()
        {
            return &app_client_if;
        }
        inline void SetGATTCTESTAttrData(RspEnableEvent *attrib)
        {
            memset (&attr, 0, sizeof(RspEnableEvent));
            memcpy (&attr, attrib, sizeof(RspEnableEvent));
        }
        inline RspEnableEvent* GetGATTCTESTAttrData()
        {
            return &attr;
        }
        inline void SetGATTCTESTAppData(GattsRegisterAppEvent *event)
        {
            memset(&app_if, 0, sizeof(GattsRegisterAppEvent));
            memcpy(&app_if, event, sizeof(GattsRegisterAppEvent));
        }
        inline GattsRegisterAppEvent* GetGATTCTESTAppData()
        {
            return &app_if;
        }
        inline void SetGATTCTESTSrvcData(GattsServiceAddedEvent *event)
        {
            memset(&srvc_data, 0, sizeof(GattsServiceAddedEvent));
            memcpy(&srvc_data, event, sizeof(GattsServiceAddedEvent));
        }
        inline GattsServiceAddedEvent* GetGATTCTESTSrvcData()
        {
            return &srvc_data;
        }
        inline void SetGATTCTESTCharacteristicData(GattsCharacteristicAddedEvent
                *event)
        {
            memset(&char_data, 0, sizeof(GattsCharacteristicAddedEvent));
            memcpy(&char_data, event, sizeof(GattsCharacteristicAddedEvent));
        }
        inline GattsCharacteristicAddedEvent* GetGATTCTESTCharacteristicData()
        {
            return &char_data;
        }
        inline void SetGATTCTESTDescriptorData(GattsDescriptorAddedEvent *event)
        {
            memset(&desc_data, 0, sizeof(GattsDescriptorAddedEvent));
            memcpy(&desc_data, event, sizeof(GattsDescriptorAddedEvent));
        }
        inline GattsDescriptorAddedEvent* GetGATTCTESTDescriptorData()
        {
            return &desc_data;
        }
        inline void SetGATTCTESTConnectionData(GattcOpenEvent*event)
        {
            memset(&client_conn_data, 0, sizeof(GattcOpenEvent));
            memcpy(&client_conn_data, event, sizeof(GattcOpenEvent));
        }
        inline GattcOpenEvent* GetGATTCTESTConnectionData()
        {
            return &client_conn_data;
        }
        bool SendResponse(GattsRequestWriteEvent *);
        bool CopyUUID(bt_uuid_t *);
        bool CopyClientUUID(bt_uuid_t *);
        bool CopyGenUUID(bt_uuid_t *);
        bool ClientSetAdvData(char *);
        bool CopyParams(bt_uuid_t *, bt_uuid_t *);
        bool MatchParams(bt_uuid_t *, bt_uuid_t *);
        bool RegisterApp(void);
        bool DisconnectServer(void);
        bool UnregisterServer(int);
        bool RegisterClient(void);
        bool UnregisterClient(int);
        bool StartAdvertisement(void);
        bool StopAdvertisement(void);
        bool StartScan(void);
        bool StopScan(void);
        bool Connect(const bt_bdaddr_t *);
        bool Disconnect(const bt_bdaddr_t *);
        bool SendAlert(int);
        bool SearchService(int);
        bool AddService(void);
        bool AddCharacteristics(void);
        bool AddDescriptor(void);
        bool StartService(void);
        bool StopService(void);
        bool DeleteService(void);
        bool HandleWlanOn(void);
        void CleanUp(int);
};
#endif

