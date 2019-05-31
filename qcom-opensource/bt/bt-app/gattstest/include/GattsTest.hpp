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

#ifndef GATTSTEST_APP_H
#define GATTSTEST_APP_H

#pragma once

#include <hardware/bluetooth.h>

#include "osi/include/log.h"
#include "osi/include/thread.h"
#include <stdio.h>
#include "osi/include/config.h"
#include "ipc.h"
#include "GattsTest.hpp"
#include "Gatt.hpp"

#define GATTSTEST_MIN_CI           (100)
#define GATTSTEST_MAX_CI           (1000)

#define GATT_PROP_READ       (0x02)
#define GATT_PROP_WRITE      (0x08)

#define GATT_PERM_READ       (0x01)
#define GATT_PERM_WRITE      (0x10)



#define LOGTAG "GATTSTEST "


static char *arr_to_string(const uint8_t *v, int size, char *buf, int out_size)
{
int limit = size;
int i;

if (out_size > 0) {
*buf = '\0';
if (size >= 2 * out_size)
limit = (out_size - 2) / 2;

for (i = 0; i < limit; ++i)
snprintf(buf + 2 * i,200, "%02x", v[i]);

/* output buffer not enough to hold whole field fill with ...*/
if (limit < size)
snprintf(buf + 2 * i,200, "...");
}

fprintf(stdout,"\nconverted to %s \n",buf);
return buf;
}


/* Remote start profile support */
typedef struct {
    int event_id;
    bt_uuid_t server_uuid;
    bt_uuid_t client_uuid;
    bt_uuid_t service_uuid;
    bt_uuid_t characteristics_uuid;
    bt_uuid_t descriptor_uuid;
} GattsTestEnableEvent;


class Gatt;
class GattsTest {
    private:
        config_t *config;
        int wlan_state;
        bool isAdvertising;

        GattsTestEnableEvent attr;
        GattcRegisterAppEvent app_client_if;
        GattsRegisterAppEvent app_if;
        GattsServiceAddedEvent srvc_data;
        GattsCharacteristicAddedEvent char_data;
        GattsDescriptorAddedEvent desc_data;
        GattsConnectionEvent conn_data;
        GattsOpenEvent client_conn_data;
        btgatt_interface_t *gatt_interface;
        Gatt *app_gatt;
        bool isClientRegistered;
        bool isServerRegistered;

    public:
        GattsTest(btgatt_interface_t *, Gatt *);
        ~GattsTest();

        bool EnableGATTSTEST();
        bool DisableGATTSTEST();
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
        inline void SetGATTSTESTClientAppData(GattcRegisterAppEvent *event)
        {
            memset(&app_client_if, 0, sizeof(app_client_if));
            memcpy(&app_client_if, event, sizeof(GattcRegisterAppEvent));
        }
        inline GattcRegisterAppEvent* GetGATTSTESTClientAppData()
        {
            return &app_client_if;
        }
        inline void SetGATTSTESTAttrData(GattsTestEnableEvent *attrib)
        {
            memset (&attr, 0, sizeof(GattsTestEnableEvent));
            memcpy (&attr, attrib, sizeof(GattsTestEnableEvent));
        }
        inline GattsTestEnableEvent* GetGATTSTESTAttrData()
        {
            return &attr;
        }
        inline void SetGATTSTESTAppData(GattsRegisterAppEvent *event)
        {
            memset(&app_if, 0, sizeof(GattsRegisterAppEvent));
            memcpy(&app_if, event, sizeof(GattsRegisterAppEvent));
        }
        inline GattsRegisterAppEvent* GetGATTSTESTAppData()
        {
            return &app_if;
        }
        inline void SetGATTSTESTSrvcData(GattsServiceAddedEvent *event)
        {
            memset(&srvc_data, 0, sizeof(GattsServiceAddedEvent));
            memcpy(&srvc_data, event, sizeof(GattsServiceAddedEvent));
        }
        inline GattsServiceAddedEvent* GetGattsTestSrvcData()
        {
            return &srvc_data;
        }
        inline void SetGATTSTESTCharacteristicData(GattsCharacteristicAddedEvent
                *event)
        {
            memset(&char_data, 0, sizeof(GattsCharacteristicAddedEvent));
            memcpy(&char_data, event, sizeof(GattsCharacteristicAddedEvent));
        }
        inline GattsCharacteristicAddedEvent* GetGATTSTESTCharacteristicData()
        {
            return &char_data;
        }
        inline void SetGATTSTESTDescriptorData(GattsDescriptorAddedEvent *event)
        {
            memset(&desc_data, 0, sizeof(GattsDescriptorAddedEvent));
            memcpy(&desc_data, event, sizeof(GattsDescriptorAddedEvent));
        }
        inline GattsDescriptorAddedEvent* GetGATTSTESTDescriptorData()
        {
            return &desc_data;
        }
        inline void SetGATTSTESTConnectionData(GattsConnectionEvent *event)
        {
            memset(&conn_data, 0, sizeof(GattsConnectionEvent));
            memcpy(&conn_data, event, sizeof(GattsConnectionEvent));
        }
        inline GattsConnectionEvent* GetGATTSTESTConnectionData()
        {
            return &conn_data;
        }
        inline void SetGATTSTESTClientConnectionData(GattsOpenEvent *event)
        {
            memset(&client_conn_data, 0, sizeof(GattsOpenEvent));
            memcpy(&client_conn_data, event, sizeof(GattsOpenEvent));
        }
        inline GattsOpenEvent* GetGATTSTESTClientConnectionData()
        {
            return &client_conn_data;
        }

        bool SendResponse(GattsRequestWriteEvent *);
        bool CopyUUID(bt_uuid_t *);
        bool CopyClientUUID(bt_uuid_t *);
        bool CopyAlertServUUID(bt_uuid_t *);
        bool CopyAlertCharUUID(bt_uuid_t *);
        bool CopyAlertDescUUID(bt_uuid_t *);
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
        bool AddService(void);
        bool AddCharacteristics(void);
        bool AddDescriptor(void);
        bool StartService(void);
        bool StopService(void);
        bool DeleteService(void);
        bool HandleWlanOn(void);
        void CleanUp(int);
        bool getIsAdvertising();
        void setIsAdvertising(bool);
};
#endif

