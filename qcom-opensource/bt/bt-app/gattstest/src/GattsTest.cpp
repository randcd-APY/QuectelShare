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

#include "Gatt.hpp"
#include "GattsTest.hpp"

#define LOGTAG "GATTSTEST "
#define UNUSED

GattsTest *gattstest = NULL;
int gattstestserverif, gattstestclientif;

class gattstestClientCallback : public BluetoothGattClientCallback
{
   public:
   void btgattc_client_register_app_cb(int status,int client_if,bt_uuid_t *uuid) {

        ALOGD(LOGTAG"gattServerRegisterAppCb\n ");

        GattcRegisterAppEvent event;
        event.event_id = GEN_GATT_EVENT;
        event.status = status;
        event.clientIf = client_if;
        memcpy(&event.app_uuid,uuid,sizeof(bt_uuid_t));
        gattstest->SetGATTSTESTClientAppData(&event);

        gattstest->ClientSetAdvData("Remote Start Profile");
        if (!gattstest->StartAdvertisement())
             gattstest->setIsAdvertising(1);
        ALOGD(LOGTAG, "isAdvertising %d",gattstest->getIsAdvertising() );
   }

   void btgattc_scan_result_cb(bt_bdaddr_t* bda, int rssi, uint8_t* adv_data) {
        UNUSED
   }

   void btgattc_open_cb(int conn_id, int status, int clientIf, bt_bdaddr_t* bda)
   {
        ALOGD(LOGTAG"btgattc_open_cb: conn_id = %d\n ", conn_id);
        GattsOpenEvent event;
        event.event_id = GEN_GATT_EVENT;
        event.conn_id = conn_id;
        event.clientIf = clientIf;
        memcpy(&event.bda, bda, sizeof(bt_bdaddr_t));
        event.status=status;

        if (gattstest) {
            gattstest->SetGATTSTESTClientConnectionData(&event);
        }else {
             fprintf(stdout, "(%s): Open With error (%d)\n", __FUNCTION__, status);
        }
   }

   void btgattc_close_cb(int conn_id, int status, int clientIf, bt_bdaddr_t* bda)
   {
       ALOGD(LOGTAG"btgattc_close_cb: conn_id = %d\n ", conn_id);
   }

   void btgattc_search_complete_cb(int conn_id, int status)
   {
        UNUSED
   }

   void btgattc_search_result_cb(int conn_id, btgatt_srvc_id_t *srvc_id)
   {
        UNUSED
   }

   void btgattc_get_characteristic_cb(int conn_id, int status,
                                     btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *char_id,
                                     int char_prop)
   {
        UNUSED
   }

   void btgattc_get_descriptor_cb(int conn_id, int status,
                                 btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *char_id,
                                 btgatt_gatt_id_t *descr_id)
   {
        UNUSED
   }

   void btgattc_get_included_service_cb(int conn_id, int status,
                                       btgatt_srvc_id_t *srvc_id, btgatt_srvc_id_t *incl_srvc_id)
   {
        UNUSED
   }

   void btgattc_register_for_notification_cb(int conn_id, int registered,
                                                int status, btgatt_srvc_id_t *srvc_id,
                                                btgatt_gatt_id_t *char_id)
   {
        UNUSED
   }

   void btgattc_notify_cb(int conn_id, btgatt_notify_params_t *p_data)
   {
        UNUSED
   }

   void btgattc_read_characteristic_cb(int conn_id, int status,
                                          btgatt_read_params_t *p_data)
   {
        UNUSED
   }

   void btgattc_write_characteristic_cb(int conn_id, int status,
                                           btgatt_write_params_t *p_data)
   {
        UNUSED
   }

   void btgattc_read_descriptor_cb(int conn_id, int status, btgatt_read_params_t *p_data)
   {
        UNUSED
   }

    void btgattc_write_descriptor_cb(int conn_id, int status, btgatt_write_params_t *p_data)
    {
        UNUSED
    }

   void btgattc_execute_write_cb(int conn_id, int status)
   {
        UNUSED
   }

   void btgattc_remote_rssi_cb(int client_if,bt_bdaddr_t* bda, int rssi, int status)
   {
       UNUSED
   }

   void btgattc_advertise_cb(int status, int client_if)
   {
        UNUSED

   }

   void btgattc_configure_mtu_cb(int conn_id, int status, int mtu)
   {
        UNUSED
   }

   void btgattc_scan_filter_cfg_cb(int action, int client_if, int status, int filt_type, int avbl_space)
   {
        UNUSED
   }

   void btgattc_scan_filter_param_cb(int action, int client_if, int status, int avbl_space)
   {
        UNUSED
   }

   void btgattc_scan_filter_status_cb(int action, int client_if, int status)
   {
        UNUSED
   }

   void btgattc_multiadv_enable_cb(int client_if, int status)
   {
        UNUSED
   }

   void btgattc_multiadv_update_cb(int client_if, int status)
   {
        UNUSED
   }

    void btgattc_multiadv_setadv_data_cb(int client_if, int status)
   {
        UNUSED
   }

   void btgattc_multiadv_disable_cb(int client_if, int status)
   {
        UNUSED
   }

   void btgattc_congestion_cb(int conn_id, bool congested)
   {
        UNUSED
   }

   void btgattc_batchscan_cfg_storage_cb(int client_if, int status)
   {
        UNUSED
   }

   void btgattc_batchscan_startstop_cb(int startstop_action, int client_if, int status)
   {
        UNUSED

   }

   void btgattc_batchscan_reports_cb(int client_if, int status, int report_format,
        int num_records, int data_len, uint8_t *p_rep_data)
   {
        UNUSED
   }

   void btgattc_batchscan_threshold_cb(int client_if)
   {
        UNUSED
   }

   void btgattc_track_adv_event_cb(btgatt_track_adv_info_t *p_adv_track_info)
   {
        UNUSED
   }

   void btgattc_scan_parameter_setup_completed_cb(int client_if, btgattc_error_t status)
   {
        UNUSED
   }

};

class gattstestServerCallback :public BluetoothGattServerCallback
{

    public:

    void gattServerRegisterAppCb(int status, int server_if, bt_uuid_t *uuid) {

        ALOGD(LOGTAG"gattServerRegisterAppCb status is %d, serverif is %d \n ",
                status, server_if);

        if (status == BT_STATUS_SUCCESS) {
            GattsRegisterAppEvent rev;
            rev.event_id = GEN_GATT_EVENT;
            rev.server_if = server_if;
            memcpy(&rev.uuid, uuid, sizeof(bt_uuid_t));
            rev.status = status;
            ALOGD(LOGTAG" set gattstest data \n");
            gattstest->SetGATTSTESTAppData(&rev);
            gattstest->AddService();
        } else {
            fprintf (stdout,"(%s) Failed to registerApp, %d \n",__FUNCTION__, server_if);
        }
    }

    void btgatts_connection_cb(int conn_id, int server_if, int connected, bt_bdaddr_t *bda)
    {

        ALOGD(LOGTAG"btgatts_connection_cb  gattstest \n ");

        GattsConnectionEvent event;
        event.event_id = GEN_GATT_EVENT;
        event.conn_id = conn_id;
        event.server_if = server_if;
        event.connected = connected;
        memcpy(&event.bda, bda, sizeof(bt_bdaddr_t));

        if (gattstest) {
            gattstest->SetGATTSTESTConnectionData(&event);
            if (connected) {
                if(!gattstest->StopAdvertisement())
                    gattstest->setIsAdvertising(0);
            }
            ALOGD(LOGTAG, "isAdvertising %d connected %d",gattstest->getIsAdvertising(),connected);
        }

    }

    void btgatts_service_added_cb(int status, int server_if,
                                    btgatt_srvc_id_t *srvc_id, int srvc_handle)
    {
        ALOGD(LOGTAG"btgatts_service_added_cb \n");
        if (status == BT_STATUS_SUCCESS) {
            GattsServiceAddedEvent event;
            event.event_id = GEN_GATT_EVENT;
            event.status = status;
            event.server_if = server_if;
            memcpy(&event.srvc_id, srvc_id,sizeof(btgatt_srvc_id_t));
            event.srvc_handle = srvc_handle;
            gattstest->SetGATTSTESTSrvcData(&event);
            gattstest->AddCharacteristics();
        } else {
            ALOGD(LOGTAG "(%s) Failed to Add_Service %d ",__FUNCTION__, server_if);
        }
    }

    void btgatts_included_service_added_cb(int status, int server_if, int srvc_handle,
                                                int incl_srvc_handle)
    {
            UNUSED;
    }

    void btgatts_characteristic_added_cb(int status, int server_if, bt_uuid_t *char_id,
                                                    int srvc_handle, int char_handle)
    {
        ALOGD(LOGTAG"btgatts_characteristic_added_cb \n");
        if (status == BT_STATUS_SUCCESS) {
            GattsCharacteristicAddedEvent event;
            event.event_id = GEN_GATT_EVENT;
            event.status = status;
            event.server_if = server_if;
            memcpy(&event.char_id, char_id,sizeof(bt_uuid_t));
            event.srvc_handle = srvc_handle;
            event.char_handle = char_handle;
            gattstest->SetGATTSTESTCharacteristicData(&event);
            gattstest->AddDescriptor();
        } else {
            ALOGD(LOGTAG "(%s) Failed to Add Characteristics %d ",__FUNCTION__, server_if);
        }
    }

    void btgatts_descriptor_added_cb(int status, int server_if, bt_uuid_t *descr_id,
                                                int srvc_handle, int descr_handle)
    {
        ALOGD(LOGTAG"btgatts_descriptor_added_cb \n");
        if (status == BT_STATUS_SUCCESS) {
            GattsDescriptorAddedEvent event;
            event.event_id = GEN_GATT_EVENT;
            event.status = status;
            event.server_if = server_if;
            memcpy(&event.descr_id, descr_id,sizeof(bt_uuid_t));
            event.srvc_handle = srvc_handle;
            event.descr_handle= descr_handle;
            gattstest->SetGATTSTESTDescriptorData(&event);
            gattstest->StartService();
            } else {
            ALOGD(LOGTAG "(%s) Failed to add descriptor %d \n",__FUNCTION__, server_if);
            }
    }

    void btgatts_service_started_cb(int status, int server_if, int srvc_handle)
    {
        ALOGD(LOGTAG"btgatts_service_started_cb \n");
        gattstest->RegisterClient();
    }

    void btgatts_service_stopped_cb(int status, int server_if, int srvc_handle)
    {
        ALOGD(LOGTAG"btgatts_service_stopped_cb \n");

        if (gattstest) {
            if (!status)
                gattstest->DeleteService();
        }
        ALOGD(LOGTAG  "GATTSTEST Service stopped successfully, deleting the service\n");
    }

    void btgatts_service_deleted_cb(int status, int server_if, int srvc_handle)
    {
        ALOGD(LOGTAG"btgatts_service_deleted_cb \n");

        if (gattstest) {
            if (!status) {
                gattstest->CleanUp(server_if);
                delete gattstest;
                gattstest = NULL;
            }
        }
        ALOGD(LOGTAG"GATTSTEST Service stopped & Unregistered successfully\n");
    }

    void btgatts_request_read_cb(int conn_id, int trans_id, bt_bdaddr_t *bda, int attr_handle,
                                            int offset, bool is_long)
    {
        UNUSED;
    }

    void btgatts_request_write_cb(int conn_id, int trans_id, bt_bdaddr_t *bda, int attr_handle,
                                            int offset, int length, bool need_rsp, bool is_prep,
                                            uint8_t* value)
    {
        ALOGD(LOGTAG"onCharacteristicWriteRequest \n");
        GattsRequestWriteEvent event;
        event.event_id = GEN_GATT_EVENT;
        event.conn_id = conn_id;
        event.trans_id = trans_id;
        memcpy(&event.bda, bda, sizeof(bt_uuid_t));
        event.attr_handle = attr_handle;
        event.offset = offset;
        event.length = length;
        event.need_rsp = need_rsp;
        event.is_prep = is_prep;
    event.value = value;

        gattstest->SendResponse(&event);
    }
    void btgatts_request_exec_write_cb(int conn_id, int trans_id,
                                                    bt_bdaddr_t *bda, int exec_write)
    {
        UNUSED;
    }

    void btgatts_response_confirmation_cb(int status, int handle)
    {
        UNUSED;
    }

    void btgatts_indication_sent_cb(int conn_id, int status)
    {
        UNUSED;
    }

    void btgatts_congestion_cb(int conn_id, bool congested)
    {
        UNUSED;
    }

    void btgatts_mtu_changed_cb(int conn_id, int mtu)
    {
        UNUSED;
    }
};

gattstestServerCallback *gattstestServerCb = NULL;
gattstestClientCallback *gattstestClientCb = NULL;

GattsTest::GattsTest(btgatt_interface_t *gatt_itf, Gatt* gatt)
{
    ALOGD(LOGTAG"gattstest instantiated ");
    gatt_interface = gatt_itf;
    app_gatt = gatt;
    gattstestClientCb = new gattstestClientCallback;
    gattstestServerCb = new gattstestServerCallback;
    isClientRegistered = false;
    isServerRegistered = false;
    isAdvertising = false;
}


GattsTest::~GattsTest()
{
    if(gattstestClientCb != NULL) {
        delete(gattstestClientCb);
        gattstestClientCb = NULL;
    }
    if(gattstestServerCb != NULL) {
        delete(gattstestServerCb);
        gattstestServerCb = NULL;
    }
    ALOGD(LOGTAG "(%s) GATTSTEST DeInitialized\n",__FUNCTION__);
    isClientRegistered = false;
    isServerRegistered = false;
    isAdvertising = false;
}

bool GattsTest::CopyUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    for (int i = 0; i < 16; i++) {
        uuid->uu[i] = 0x30;
    }
    return true;
}

bool GattsTest::CopyClientUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    uuid->uu[0] = 0xff;
    for (int i = 1; i < 16; i++) {
        uuid->uu[i] = 0x30;
    }
    return true;
}

bool GattsTest::CopyAlertServUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    uuid->uu[15] = 0x00;
    uuid->uu[14] = 0x00;
    uuid->uu[13] = 0x18;
    uuid->uu[12] = 0x02;
    uuid->uu[11] = 0x00;
    uuid->uu[10] =0x00;
    uuid->uu[9] = 0x10;
    uuid->uu[8] = 0x00;
    uuid->uu[7] =0x80;
    uuid->uu[6] = 0x00;
    uuid->uu[5] = 0x00;
    uuid->uu[4] = 0x80;
    uuid->uu[3] = 0x5f;
    uuid->uu[2] = 0x9b;
    uuid->uu[1] = 0x34;
    uuid->uu[0] = 0xfb;

    return true;
}
bool GattsTest::CopyAlertCharUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    uuid->uu[15] = 0x00;
    uuid->uu[14] = 0x00;
    uuid->uu[13] = 0x2a;
    uuid->uu[12] = 0x06;
    uuid->uu[11] = 0x00;
    uuid->uu[10] =0x00;
    uuid->uu[9] = 0x10;
    uuid->uu[8] = 0x00;
    uuid->uu[7] =0x80;
    uuid->uu[6] = 0x00;
    uuid->uu[5] = 0x00;
    uuid->uu[4] = 0x80;
    uuid->uu[3] = 0x5f;
    uuid->uu[2] = 0x9b;
    uuid->uu[1] = 0x34;
    uuid->uu[0] = 0xfb;

    return true;
}

bool GattsTest::CopyAlertDescUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    uuid->uu[15] = 0x00;
    uuid->uu[14] = 0x00;
    uuid->uu[13] = 0x2a;
    uuid->uu[12] = 0x07;
    uuid->uu[11] = 0x00;
    uuid->uu[10] =0x00;
    uuid->uu[9] = 0x10;
    uuid->uu[8] = 0x00;
    uuid->uu[7] =0x80;
    uuid->uu[6] = 0x00;
    uuid->uu[5] = 0x00;
    uuid->uu[4] = 0x80;
    uuid->uu[3] = 0x5f;
    uuid->uu[2] = 0x9b;
    uuid->uu[1] = 0x34;
    uuid->uu[0] = 0xfb;


    return true;
}


bool GattsTest::CopyParams(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
{
    CHECK_PARAM(uuid_dest)
    CHECK_PARAM(uuid_src)

    for (int i = 0; i < 16; i++) {
        uuid_dest->uu[i] = uuid_src->uu[i];
    }
    return true;
}

bool GattsTest::MatchParams(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
{
    CHECK_PARAM(uuid_dest)
    CHECK_PARAM(uuid_src)

    for (int i = 0; i < 16; i++) {
        if(uuid_dest->uu[i] != uuid_src->uu[i])
            return false;
    }
    ALOGD(LOGTAG "(%s) UUID Matches",__FUNCTION__);
    return true;
}

bool GattsTest::EnableGATTSTEST()
{
    ALOGD(LOGTAG "(%s) Enable GATTSTEST Initiated \n",__FUNCTION__);

    GattsTestEnableEvent rev;
    rev.event_id = GEN_GATT_EVENT;// change it later
    CopyAlertCharUUID(&rev.characteristics_uuid);
    CopyAlertDescUUID(&rev.descriptor_uuid);
    CopyUUID(&rev.server_uuid);
    CopyClientUUID(&rev.client_uuid);
    CopyAlertServUUID(&rev.service_uuid);

    ALOGD(LOGTAG" set gattstest data \n");
    SetGATTSTESTAttrData(&rev);
    return RegisterApp();
}

bool GattsTest::DisableGATTSTEST()
{
    ALOGD(LOGTAG "(%s) Disable GATTSTEST Initiated",__FUNCTION__);
    return StopService();
}

bool GattsTest::RegisterApp()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bt_uuid_t server_uuid = GetGATTSTESTAttrData()->server_uuid;
    ALOGD(LOGTAG"reg app addr is %d \n", GetGATTSTESTAttrData()->server_uuid);
    app_gatt->RegisterServerCallback(gattstestServerCb,&GetGATTSTESTAttrData()->server_uuid);
    isServerRegistered = (app_gatt->register_server(&server_uuid) == BT_STATUS_SUCCESS);
    return isServerRegistered;
}

bool GattsTest::RegisterClient()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bt_uuid_t client_uuid = GetGATTSTESTAttrData()->client_uuid;
    app_gatt->RegisterClientCallback(gattstestClientCb,&GetGATTSTESTAttrData()->client_uuid);
    isClientRegistered =(app_gatt->register_client(&client_uuid) == BT_STATUS_SUCCESS);
    return isClientRegistered;
}

bool GattsTest::UnregisterClient(int client_if)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    app_gatt->UnRegisterClientCallback(client_if);
    return app_gatt->unregister_client(client_if) == BT_STATUS_SUCCESS;
}

bool GattsTest::ClientSetAdvData(char *str)
{
    bt_status_t        Ret;
    bool              SetScanGattsTest        = false;
    bool              IncludeName       = true;
    bool              IncludeTxPower    = false;
    bool              dataset;
    int               min_conn_interval = GATTSTEST_MIN_CI;
    int               max_conn_interval = GATTSTEST_MAX_CI;

    dataset = app_gatt->set_adv_data(GetGATTSTESTClientAppData()->clientIf, SetScanGattsTest,
                                                IncludeName, IncludeTxPower, min_conn_interval,
                                                max_conn_interval, 0,strlen(str), str,
                                                strlen(str), str, 0,NULL);
    return dataset;
}

void GattsTest::CleanUp(int server_if)
{
    int client_if = GetGATTSTESTClientAppData()->clientIf ;
    ALOGD(LOGTAG "(%s) unregistering serverif(%d) & ClientIf(%d)\n",__FUNCTION__, server_if,client_if);
    if (isServerRegistered)
        UnregisterServer(server_if);
    if (isClientRegistered)
        UnregisterClient(client_if);
    ALOGD(LOGTAG "(%s) unregistered serverif(%d) & ClientIf(%d)\n",__FUNCTION__, server_if,client_if);
}

bool GattsTest::UnregisterServer(int server_if)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "Gatt Interface Not present");
        return false;
    }
    app_gatt->UnRegisterServerCallback(server_if);
    return app_gatt->unregister_server(server_if) == BT_STATUS_SUCCESS;
}

bool GattsTest::StartAdvertisement()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGD(LOGTAG  "(%s) Listening on the interface (%d) ",__FUNCTION__,
            GetGATTSTESTClientAppData()->clientIf);
    //SetDeviceState(WLAN_INACTIVE);
    return app_gatt->listen(GetGATTSTESTClientAppData()->clientIf, true);
}

bool GattsTest::SendResponse(GattsRequestWriteEvent *event)
{
    char val[5];
    unsigned char *p;
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present \n",__FUNCTION__);
        return false;
    }
    CHECK_PARAM(event)
    btgatt_response_t att_resp;
    int response = -1;
    memset(att_resp.attr_value.value,0,BTGATT_MAX_ATTR_LEN);
    memcpy(att_resp.attr_value.value, event->value, event->length);
    att_resp.attr_value.handle = event->attr_handle;
    att_resp.attr_value.offset = event->offset;
    att_resp.attr_value.len = event->length;
    att_resp.attr_value.auth_req = 0;


    ALOGD(LOGTAG "(%s) Sending GATTSTEST response to write (%d) ",__FUNCTION__,
        GetGATTSTESTAppData()->server_if);

    p = (unsigned char *)att_resp.attr_value.value;
    if (p[0] == 0x00) {
         response = 0;
         ALOGI(LOGTAG"low alert written \n");
    } else if (p[0] == 0x01) {
         response = 0;
         ALOGI(LOGTAG"mid alert written \n");
    } else if (p[0] == 0x02) {
         response = 0;
         ALOGI(LOGTAG"high alert written \n");
    } else {
         ALOGI(LOGTAG"default alert written \n");
    }
    return app_gatt->send_response(event->conn_id, event->trans_id,
                                                         response, &att_resp);
}

bool GattsTest::HandleWlanOn()
{
    BtEvent *event = new BtEvent;
    CHECK_PARAM(event);
    event->event_id = SKT_API_IPC_MSG_WRITE;
    event->bt_ipc_msg_event.ipc_msg.type = BT_IPC_REMOTE_START_WLAN;
    event->bt_ipc_msg_event.ipc_msg.status = INITIATED;
    StopAdvertisement();
    ALOGD(LOGTAG "(%s) Posting wlan start to main thread \n",__FUNCTION__);
    PostMessage (THREAD_ID_MAIN, event);
    return true;
}

bool GattsTest::StopAdvertisement()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGD(LOGTAG "(%s) Stopping listen on the interface (%d) \n",__FUNCTION__,
            GetGATTSTESTClientAppData()->clientIf);
    return app_gatt->listen(GetGATTSTESTClientAppData()->clientIf, false);
}

bool GattsTest::AddService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    btgatt_srvc_id_t srvc_id;
    srvc_id.id.inst_id = 0;   // 1 instance
    srvc_id.is_primary = 1;   // Primary addition
    srvc_id.id.uuid = GetGATTSTESTAttrData()->service_uuid;
    return app_gatt->add_service(GetGATTSTESTAppData()->server_if, &srvc_id,4)
                                                        ==BT_STATUS_SUCCESS;
}

bool GattsTest::DisconnectServer()
{
    int status;
    int server_if = GetGATTSTESTConnectionData()->server_if;
    bt_bdaddr_t bda;
    memcpy(&bda, &(GetGATTSTESTConnectionData()->bda),sizeof(bt_bdaddr_t));
    int server_conn_id = GetGATTSTESTConnectionData()->conn_id;
    ALOGD(LOGTAG  "(%s) Disconnecting server interface (%d), connid (%d) ",__FUNCTION__,
         server_if, server_conn_id);
    status = app_gatt->serverDisconnect(server_if, &bda, server_conn_id);

    int client_if = GetGATTSTESTClientConnectionData()->clientIf;
    bt_bdaddr_t client_bda;
    memcpy(&client_bda, &(GetGATTSTESTClientConnectionData()->bda),sizeof(bt_bdaddr_t));
    int client_conn_id =  GetGATTSTESTClientConnectionData()->conn_id;
    if (gattstest) {
       ALOGD(LOGTAG,  "(%s) Disconnecting client interface (%d), connid (%d) ",__FUNCTION__,
          client_if, client_conn_id);
       status = app_gatt->clientDisconnect(client_if, &client_bda, client_conn_id);
    }

    if (status == BT_STATUS_SUCCESS)
        return true;
    else
        return false;
}

bool GattsTest::DeleteService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bool status = false;
    int srvc_handle = GetGattsTestSrvcData()->srvc_handle;
    return app_gatt->delete_service(GetGATTSTESTAppData()->server_if,
                                                            srvc_handle) == BT_STATUS_SUCCESS;
}

bool GattsTest::AddCharacteristics()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bt_uuid_t char_uuid;
    char_uuid = GetGATTSTESTAttrData()->characteristics_uuid;
    int srvc_handle = GetGattsTestSrvcData()->srvc_handle;
    int server_if = GetGattsTestSrvcData()->server_if;
    ALOGD(LOGTAG  "(%s) Adding Characteristics server_if (%d), srvc_handle (%d) \n",
            __FUNCTION__, server_if,srvc_handle);
    return app_gatt->add_characteristic(server_if, srvc_handle, &char_uuid,
                                                            GATT_PROP_WRITE, GATT_PERM_WRITE)
                                                            ==BT_STATUS_SUCCESS;
}

bool GattsTest::AddDescriptor(void)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    bt_uuid_t desc_uuid;
    desc_uuid = GetGATTSTESTAttrData()->descriptor_uuid;
    int srvc_handle = GetGattsTestSrvcData()->srvc_handle;
    return app_gatt->add_descriptor(GetGATTSTESTAppData()->server_if,
                                                        srvc_handle, &desc_uuid,
                                                        GATT_PERM_READ) == BT_STATUS_SUCCESS;
}

bool GattsTest::StartService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    int srvc_handle = GetGattsTestSrvcData()->srvc_handle;
    return app_gatt->start_service(GetGATTSTESTAppData()->server_if,
                                                        srvc_handle, GATT_TRANSPORT_LE)
                                                        == BT_STATUS_SUCCESS;
}

bool GattsTest::StopService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    int srvc_handle = GetGattsTestSrvcData()->srvc_handle;
    return app_gatt->stop_service(GetGATTSTESTAppData()->server_if,
                                                        srvc_handle) == BT_STATUS_SUCCESS;
}

bool GattsTest::getIsAdvertising()
{
    return isAdvertising;
}

void GattsTest::setIsAdvertising(bool value){
    ALOGD(LOGTAG "setIsAdvertising %d ", value);
    isAdvertising = value;
}
