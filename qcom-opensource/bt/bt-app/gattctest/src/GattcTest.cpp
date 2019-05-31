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
#include "GattcTest.hpp"
#include "utils.h"
#include <stdlib.h>

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

#define LOGTAG "GATTCTEST "
#define UNUSED

#define COPYMAXLEN 200

ServiceData *gattctestServData;
ServiceData *gattctestAlertData;

int testest;

int gattctestserverif, gattctestclientif;

GattcTest *gattctest = NULL;

bt_uuid_t client_uuid;
bt_uuid_t gen_uuid;

#define ADDRESS_STR_LEN 18
#define UUID_STR_LEN 37
#define HEX_VAL_STR_LEN 100

#define CHARID_STR_LEN UUID_STR_LEN + 3 + 11
#define SRVCID_STR_LEN UUID_STR_LEN + 3 + 11 + 1 + 11
#define desc_id_to_string gatt_id_to_string

#define MAX_NOTIFY_PARAMS_STR_LEN (SRVCID_STR_LEN + CHARID_STR_LEN \
+ ADDRESS_STR_LEN + HEX_VAL_STR_LEN + 60)
#define MAX_READ_PARAMS_STR_LEN (SRVCID_STR_LEN + CHARID_STR_LEN \
+ UUID_STR_LEN + HEX_VAL_STR_LEN + 80)


static char *uuid_to_string(const bt_uuid_t *uuid, char *buf)
{
    int shift = 0;
    int i = 16;
    int limit = 0;
    int j = 0;

    /* for bluetooth uuid only 32 bits */
    if (0 == memcmp(&uuid->uu, &(gen_uuid.uu),
    sizeof(bt_uuid_t) - 4)) {
    limit = 12;
    /* make it 16 bits */
    if (uuid->uu[15] == 0 && uuid->uu[14] == 0)
    i = 14;
    }

    while (i-- > limit) {
    if (i == 11 || i == 9 || i == 7 || i == 5) {
    buf[j * 2 + shift] = '-';
    shift++;
    }

    snprintf(buf + j * 2 + shift,COPYMAXLEN, "%02x", uuid->uu[i]);
    ++j;
    }

    return buf;
}


/* service_id formating function */
char *service_id_to_string(const btgatt_srvc_id_t *srvc_id, char *buf) {
    char uuid_buf[UUID_STR_LEN];
    snprintf(buf,COPYMAXLEN,"{%s,%d,%d}", uuid_to_string(&srvc_id->id.uuid,
    uuid_buf),srvc_id->id.inst_id,srvc_id->is_primary);
    return buf;
}


static char *gatt_id_to_string(const btgatt_gatt_id_t *char_id, char *buf)
{
    char uuid_buf[UUID_STR_LEN];

    snprintf(buf,COPYMAXLEN, "{%s,%d}", uuid_to_string(&char_id->uuid, uuid_buf),
    char_id->inst_id);
    return buf;
}

static char *arr_to_string(const uint8_t *v, int size, char *buf, int out_size)
{
    int limit = size;
    int i;

    if (out_size > 0) {
    *buf = '\0';
    if (size >= 2 * out_size)
    limit = (out_size - 2) / 2;

    for (i = 0; i < limit; ++i)
    snprintf(buf + 2 * i,COPYMAXLEN, "%02x", v[i]);

    /* output buffer not enough to hold whole field fill with ...*/
    if (limit < size)
    snprintf(buf + 2 * i,COPYMAXLEN, "...");
    }

    return buf;
}


static char *raw_data_to_string(const btgatt_unformatted_value_t *v,
    char *buf, int size)
{
    return arr_to_string(v->value, v->len, buf, size);
}

static char *read_param_to_string(const btgatt_read_params_t *data,
             char *buf)
{
    char srvc_id[SRVCID_STR_LEN];
    char char_id[CHARID_STR_LEN];
    char descr_id[UUID_STR_LEN];
    char value[HEX_VAL_STR_LEN];
/*    snprintf(buf,COPYMAXLEN, "{srvc_id=%s, char_id=%s, descr_id=%s, val=%s value_type=%d, status=%d}",
        service_id_to_string(&data->srvc_id, srvc_id),
        gatt_id_to_string(&data->char_id, char_id),
        desc_id_to_string(&data->descr_id, descr_id),
        raw_data_to_string(&data->value, value, 100),
        data->value_type, data->status);
*/
    return buf;
}


/*****************************/
class gattctestClientCallback : public BluetoothGattClientCallback
{
    private:
        bool AlertServiceMatches;
        bool srvcMatching;
    public:
        bool foundAlertService() {
             return AlertServiceMatches;  
        }

       bool CpUUID(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
      {

        CHECK_PARAM(uuid_dest)
        CHECK_PARAM(uuid_src)

        for (int i = 0; i < 16; i++) {
         uuid_dest->uu[i] = uuid_src->uu[i];
        }
        return true;
}
    void btgattc_client_register_app_cb(int status,int client_if,bt_uuid_t *uuid) {

        fprintf(stdout,"gattctest btgattc_client_register_app_cb\n ");
        AlertServiceMatches = false;
        srvcMatching = false;

        if(!gattctest) {
            fprintf(stdout,"Client not initialized ... returning%s \n", __func__);
            return;
        }

        GattcRegisterAppEvent event;
        event.event_id = RSP_ENABLE_EVENT;
        event.status = status;
        event.clientIf = client_if;
        memcpy(&event.app_uuid,uuid,sizeof(bt_uuid_t));

        gattctest->SetGATTCTESTClientAppData(&event);
    }

    void btgattc_scan_result_cb(bt_bdaddr_t* bda, int rssi, uint8_t* adv_data) {
         bdstr_t bd_str;
         strlcpy(bd_str, bdaddr_empty, MAX_BD_STR_LEN);
         bdaddr_to_string(bda, &bd_str[0], sizeof(bd_str));
         fprintf(stdout,"btgattc_scan_result_cb %s \n ", bd_str);
    }

    void btgattc_open_cb(int conn_id, int status, int clientIf, bt_bdaddr_t* bda)
    {
         fprintf(stdout,"btgattc_open_cb  gattctest   status is %d\n ", status);

        GattcOpenEvent event;
        event.event_id = BTGATTC_OPEN_EVENT;
        event.conn_id = conn_id;
        event.status = status;
        event.clientIf = clientIf;
        memcpy(&event.bda, bda, sizeof(bt_bdaddr_t));

        if (gattctest) {
            gattctest->SetGATTCTESTConnectionData(&event);
            if (status == 0)
            {
                gattctest->SearchService(conn_id);
            } else {
             fprintf(stdout, "(%s): Open With error (%d)\n", __FUNCTION__, status);
            }
      }
    }

    void btgattc_close_cb(int conn_id, int status, int clientIf, bt_bdaddr_t* bda)
    {
        fprintf(stdout,"btgattc_close_cb  gattctest \n ");
        AlertServiceMatches = false;
        srvcMatching = false;
        if (gattctestAlertData != NULL ) {
             fprintf(stdout,"Diagnostic:(%s), freeing testAlert\n", __FUNCTION__);
            if (gattctestAlertData->srvc_id != NULL) {
                osi_free (gattctestAlertData->srvc_id);
                gattctestAlertData->srvc_id = NULL;
            }
            if (gattctestAlertData->char_id != NULL) {
                osi_free (gattctestAlertData->char_id);
                gattctestAlertData->char_id = NULL;
            }
            if (gattctestAlertData->descr_id != NULL) {
                osi_free (gattctestAlertData->descr_id);
                gattctestAlertData->descr_id = NULL;
            }
            osi_free(gattctestAlertData);
            gattctestAlertData = NULL;
        }
        if (gattctestServData != NULL ) {
             fprintf(stdout,"Diagnostic:(%s), freeing testServerData\n", __FUNCTION__);
            if (gattctestServData->srvc_id != NULL) {
                osi_free (gattctestServData->srvc_id);
                gattctestServData->srvc_id = NULL;
            }
            if (gattctestServData->char_id != NULL) {
                osi_free (gattctestServData->char_id);
                gattctestServData->char_id = NULL;
            }
            if (gattctestServData->descr_id != NULL) {
                osi_free (gattctestServData->descr_id);
                gattctestServData->descr_id = NULL;
            }
             osi_free(gattctestServData);
             gattctestServData = NULL;
        }
    }

    void btgattc_search_complete_cb(int conn_id, int status)
    {
         fprintf(stdout,"btgattc_search_complete_cb  conn_id %d, status %d \n ", conn_id,status);
        if(status == 0) {
           fprintf(stdout,"go for get db search\n");
           gattctest->app_gatt->get_gatt_db(conn_id);
        } else {
           fprintf(stdout,"btgattc_search_complete_cb no result\n");
        }
    }

    void btgattc_search_result_cb(int conn_id, btgatt_srvc_id_t *srvc_id)
    {
        char srvc_id_buf[(SRVCID_STR_LEN)];
        fprintf(stdout,"%s: conn_id=%d srvc_id=%s ++ \n", __func__, conn_id,service_id_to_string(srvc_id, srvc_id_buf));

        srvcMatching = MatchAlertServiceUUID(&srvc_id->id.uuid) ;
        if( srvcMatching == true) {
            if (gattctestServData == NULL ) {
                 fprintf(stdout,"Return: Could not allocate service data\n");
                return;
            }
            gattctestServData->conn_id = conn_id;
            gattctestServData->srvc_id = (btgatt_srvc_id_t*) osi_malloc(sizeof (btgatt_srvc_id_t));
            if(gattctestServData->srvc_id == NULL) {
                fprintf(stdout,"Could not allocate memory to gattctestServData->srvc_id\n");
                return;
            }
            memcpy(gattctestServData->srvc_id, srvc_id, sizeof (btgatt_srvc_id_t));

             fprintf(stdout,"%s: Matching Service UUID in Search CB--\n", __func__);
        } else {
             fprintf(stdout,"%s: Service UUID in Search CB doesnt match--\n", __func__);
        }
        fprintf(stdout,"%s: --\n", __func__);
    }


    bool CompareParams(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
    {
        CHECK_PARAM(uuid_dest)
        CHECK_PARAM(uuid_src)

        for (int i = 0; i < 16; i++) {
            if(uuid_dest->uu[i] != uuid_src->uu[i]){
             fprintf(stdout, "(%s) UUID Failed Matches\n",__FUNCTION__);
                return false;
        }
        }
         fprintf(stdout,"(%s) UUID Matches\n",__FUNCTION__);
        return true;
    }

    bool MatchAlertServiceUUID(bt_uuid_t *suuid)
    {
        CHECK_PARAM(suuid)
        bt_uuid_t uuid;
        uuid.uu[15] = 0x00;
        uuid.uu[14] = 0x00;
        uuid.uu[13] = 0x18;
        uuid.uu[12] = 0x02;
        uuid.uu[11] = 0x00;
        uuid.uu[10] =0x00;
        uuid.uu[9] = 0x10;
        uuid.uu[8] = 0x00;
        uuid.uu[7] =0x80;
        uuid.uu[6] = 0x00;
        uuid.uu[5] = 0x00;
        uuid.uu[4] = 0x80;
        uuid.uu[3] = 0x5f;
        uuid.uu[2] = 0x9b;
        uuid.uu[1] = 0x34;
        uuid.uu[0] = 0xfb;

	 fprintf(stdout,"(%s) Matching Service UUID\n", __FUNCTION__);
        return CompareParams(&uuid, suuid);
    }

    bool MatchAlertCharUUID(bt_uuid_t *suuid)
    {
        CHECK_PARAM(suuid)
        bt_uuid_t uuid;
        uuid.uu[15] = 0x00;
        uuid.uu[14] = 0x00;
        uuid.uu[13] = 0x2a;
        uuid.uu[12] = 0x06;
        uuid.uu[11] = 0x00;
        uuid.uu[10] =0x00;
        uuid.uu[9] = 0x10;
        uuid.uu[8] = 0x00;
        uuid.uu[7] =0x80;
        uuid.uu[6] = 0x00;
        uuid.uu[5] = 0x00;
        uuid.uu[4] = 0x80;
        uuid.uu[3] = 0x5f;
        uuid.uu[2] = 0x9b;
        uuid.uu[1] = 0x34;
        uuid.uu[0] = 0xfb;

	 fprintf(stdout, "(%s) Matching Char UUID\n", __FUNCTION__);
        return CompareParams(&uuid, suuid);
     }


    void btgattc_get_characteristic_cb(int conn_id, int status,
                                     btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *char_id,
                                     int char_prop)
    {
    char srvc_id_buf[SRVCID_STR_LEN];
    char char_id_buf[CHARID_STR_LEN];

     fprintf(stdout,"%s: conn_id=%d status=%d srvc_id=%s char_id=%s, char_prop=%x ++ \n",
       __func__, conn_id, status,
    service_id_to_string(srvc_id, srvc_id_buf),
    gatt_id_to_string(char_id, char_id_buf), char_prop);

    if(AlertServiceMatches!= true ) {
        gattctestAlertData = (ServiceData *) (osi_malloc(sizeof(ServiceData)));
        if (gattctestAlertData == NULL ) {
             fprintf(stdout, "Return: Could not allocate service data\n");
            return;
        }
         fprintf(stdout, "%s, got characteristics successfully\n",__func__);


         fprintf(stdout, "\Diagnostic: srvc_id->id.uuid is ++\n");
        for (int j = 0; j < sizeof(srvc_id->id.uuid); j++) {
              fprintf(stdout,  "%02x", srvc_id->id.uuid.uu[j]);
         }

         fprintf(stdout, "\Diagnostic: char_id->id.uuid is \n");
        for (int j = 0; j < sizeof(char_id->uuid); j++) {
              fprintf(stdout, "%02x", char_id->uuid.uu[j]);
        }

        AlertServiceMatches = MatchAlertCharUUID(&char_id->uuid)& MatchAlertServiceUUID(&srvc_id->id.uuid);
        if(AlertServiceMatches == true){
             fprintf(stdout, "Saving the Alert Level details\n");
            gattctestAlertData->conn_id = conn_id;

            gattctestAlertData->srvc_id = (btgatt_srvc_id_t*) osi_malloc(sizeof(btgatt_srvc_id_t));
            if (gattctestAlertData->srvc_id == NULL) {
                fprintf(stdout,"Could not allocate memory to gattctestAlertData->srvc_id \n");
                return;
            }
            memcpy(gattctestAlertData->srvc_id, srvc_id, sizeof(btgatt_srvc_id_t));

            gattctestAlertData->char_id = (btgatt_gatt_id_t*) osi_malloc(sizeof(btgatt_gatt_id_t));
            if (gattctestAlertData->char_id == NULL) {
                fprintf(stdout,"Could not allocate memory to gattctestAlertData->char_id \n");
                return;
            }
            memcpy(gattctestAlertData->char_id, char_id, sizeof(btgatt_gatt_id_t));
            fprintf(stdout, "%s, Return 1  -- \n", __func__);
            return;
            }
        } else {
             fprintf(stdout, "%s, All Characteristics fetched, no more characteristcs --\n", __func__);
        }
    }

    void btgattc_get_descriptor_cb(int conn_id, int status,
                                     btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *char_id,
                                     btgatt_gatt_id_t *descr_id)
    {
        char buf[UUID_STR_LEN];
        char srvc_id_buf[SRVCID_STR_LEN];
        char char_id_buf[CHARID_STR_LEN];

        gattctestServData->conn_id = conn_id;
        gattctestServData->srvc_id = (btgatt_srvc_id_t*) osi_malloc (sizeof(btgatt_srvc_id_t));
        if (gattctestServData->srvc_id == NULL) {
            fprintf(stdout,"Could not allocate memory to gattctestServData->srvc_id \n");
            return;
        }
        memcpy(gattctestServData->srvc_id,srvc_id, (sizeof(btgatt_srvc_id_t)));

        gattctestServData->char_id = (btgatt_gatt_id_t*)osi_malloc (sizeof(btgatt_gatt_id_t));
        if (gattctestServData->char_id == NULL) {
            fprintf(stdout,"Could not allocate memory to gattctestServData->char_id \n");
            return;
        }
        memcpy(gattctestServData->char_id, char_id, sizeof(btgatt_gatt_id_t));
        gattctestServData->descr_id = descr_id;

         fprintf(stdout,"%s: conn_id=%d status=%d srvc_id=%s char_id=%s, descr_id=%s\n",
               __func__, conn_id, status,
              service_id_to_string(srvc_id, srvc_id_buf),
              gatt_id_to_string(char_id, char_id_buf),
              desc_id_to_string(descr_id, buf));

        if(status == 0) {
             fprintf(stdout, "%s, got descriptor successfully\n", __func__);
            gattctest->app_gatt->get_descriptor(conn_id,srvc_id,char_id,descr_id);
        } else {
             fprintf(stdout,"%s, All Descriptors fetched, no more descriptors\n",__func__);
        }
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
        char buf[MAX_READ_PARAMS_STR_LEN];

         fprintf(stdout,"%s: conn_id=%d status=%d data=%s\n", __func__, conn_id,
                status, read_param_to_string(p_data, buf));
    }

    void btgattc_write_characteristic_cb(int conn_id, int status,
                                           btgatt_write_params_t *p_data)
    {
         fprintf(stdout,"btgattc_write_characteristic_cb status is %d conn_id %d \n ", status,conn_id);
    }

    void btgattc_read_descriptor_cb(int conn_id, int status, btgatt_read_params_t *p_data)
    {

    char buf[MAX_READ_PARAMS_STR_LEN];

     fprintf(stdout,"%s: conn_id=%d status=%d data=%s\n", __func__, conn_id,
           status, read_param_to_string(p_data, buf));

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

    void btgattc_get_included_service_cb(int conn_id, int status,
                                       btgatt_srvc_id_t *srvc_id, btgatt_srvc_id_t *incl_srvc_id)
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

    void btgattc_get_gatt_db_cb(int conn_id, btgatt_db_element_t *db, int count)
    {
          fprintf(stdout, "btgattc_get_gatt_db_cb conn_id is %d, count is %d\n", conn_id, count);

          for(int i = 0; i < count ; i++) {
             btgatt_db_element_t curr = db[i];
             fprintf(stdout,"curr type is %d\n", curr.type);
             switch(curr.type ) {

                case BTGATT_DB_PRIMARY_SERVICE:
                    srvcMatching = false;
                    fprintf(stdout," id is %d \n", curr.id);
                    fprintf(stdout," type is BTGATT_DB_PRIMARY_SERVICE  %d \n", curr.type);
                     fprintf(stdout," uuid is \n");
                    for (int j = 0; j < sizeof(curr.uuid); j++) {
                      fprintf(stdout,  "%02x", curr.uuid.uu[j]);
                    }
                    fprintf(stdout,"\n");
                    srvcMatching = MatchAlertServiceUUID(&curr.uuid) ;
                    if( srvcMatching == true) {
                         if (gattctestServData == NULL ) {
                              fprintf(stdout,"Return: Could not allocate service data\n");
                             return;
                         }
                         gattctestServData->conn_id = conn_id;
                         gattctestServData->srvc_id = (btgatt_srvc_id_t*) osi_malloc(sizeof (btgatt_srvc_id_t));
                         if (gattctestServData->srvc_id == NULL) {
                            fprintf(stdout,"Could not allocate memory to "
                                "gattctestServData->srvc_id \n");
                            return;
                         }
                         CpUUID(&gattctestServData->srvc_id->id.uuid, &(curr.uuid));
                         gattctestServData->srvc_id->id.inst_id = curr.id;

                          fprintf(stdout,"%s: Matching Service UUID in Search CB--\n", __func__);
                     } else {
                          fprintf(stdout, "%s: Service UUID in Search CB doesnt match--\n", __func__);
                     }

                      fprintf(stdout," \n");
                     fprintf(stdout," attribute handle is %d \n", curr.attribute_handle);
                      fprintf(stdout," start handle is %d \n", curr.start_handle);
                      fprintf(stdout," end handle is %d \n", curr.end_handle);
                      fprintf(stdout," properties are %d \n", curr.properties);

                     break;

             case BTGATT_DB_SECONDARY_SERVICE:

                     fprintf(stdout," id is %d \n", curr.id);
                      fprintf(stdout," type is BTGATT_DB_SECONDARY_SERVICE %d \n", curr.type);
                      fprintf(stdout," uuid is \n");
                     for (int j = 0; j < sizeof(curr.uuid); j++) {
                        fprintf(stdout,  "%02x", curr.uuid.uu[j]);
                        fprintf(stdout,  "%02x", curr.uuid.uu[j]);
                     }

                      fprintf(stdout," \n");
                      fprintf(stdout," attribute handle is %d \n", curr.attribute_handle);
                      fprintf(stdout," start handle is %d \n", curr.start_handle);
                     fprintf(stdout," end handle is %d \n", curr.end_handle);
                      fprintf(stdout," properties are %d \n", curr.properties);
                     break;

             case BTGATT_DB_CHARACTERISTIC:

                      fprintf(stdout," id is %d \n", curr.id);
                      fprintf(stdout," type is  BTGATT_DB_CHARACTERISTIC %d \n", curr.type);
                      fprintf(stdout," uuid is \n");
                     for (int j = 0; j < sizeof(curr.uuid); j++) {
                           fprintf(stdout,  "%02x", curr.uuid.uu[j]);
                     }
                     AlertServiceMatches = MatchAlertCharUUID(&curr.uuid) && srvcMatching;
                     if(AlertServiceMatches == true){
                          fprintf(stdout, "Saving the Alert Level details\n");
                         gattctestAlertData = (ServiceData *) (osi_malloc(sizeof(ServiceData)));
                         if (gattctestAlertData == NULL) {
                            fprintf(stdout,"Could not allocate memory to gattctestAlertData \n");
                            return;
                         }
                         gattctestAlertData->conn_id = conn_id;
                         gattctestAlertData->srvc_id = (btgatt_srvc_id_t*) osi_malloc(sizeof(btgatt_srvc_id_t));
                         gattctestAlertData->char_id = (btgatt_gatt_id_t*) osi_malloc(sizeof(btgatt_gatt_id_t));
                          fprintf(stdout," \n");
                          fprintf(stdout," attribute handle is %d \n", curr.attribute_handle);
                         gattctestAlertData->handle = curr.attribute_handle;
                          fprintf(stdout," start handle is %d \n", curr.start_handle);
                          fprintf(stdout," end handle is %d \n", curr.end_handle);
                          fprintf(stdout," properties are %d \n", curr.properties);
                     }
                      else {
                          fprintf(stdout, "%s, All Characteristics fetched, no more characteristcs --\n", __func__);
                     }
                     break;

             case BTGATT_DB_DESCRIPTOR:

                     fprintf(stdout," id is %d \n", curr.id);
                     fprintf(stdout," type is BTGATT_DB_DESCRIPTOR %d \n", curr.type);
                     fprintf(stdout," uuid is \n");
                    for (int j = 0; j < sizeof(curr.uuid); j++) {
                          fprintf(stdout,  "%02x", curr.uuid.uu[j]);
                    }

                     fprintf(stdout," \n");
                     fprintf(stdout," attribute handle is %d \n", curr.attribute_handle);
                     fprintf(stdout," start handle is %d \n", curr.start_handle);
                     fprintf(stdout," end handle is %d \n", curr.end_handle);
                     fprintf(stdout," properties are %d \n", curr.properties);
                    break;
          }
 
       }

   }

};

class gattctestServerCallback :public BluetoothGattServerCallback
{

    public:

    void gattServerRegisterAppCb(int status, int server_if, bt_uuid_t *uuid) {

        fprintf(stdout,"gattServerRegisterAppCb status is %d, serverif is %d \n ",
               status, server_if);

       if (status == BT_STATUS_SUCCESS)
       {
          GattsRegisterAppEvent rev;
          rev.event_id = RSP_ENABLE_EVENT;
          rev.server_if = server_if;
          memcpy(&rev.uuid, uuid,sizeof(bt_uuid_t));
          rev.status = status;
           fprintf(stdout," set gattctest data \n");
          gattctest->SetGATTCTESTAppData(&rev);
          gattctest->AddService();
       } else {
          fprintf (stdout,"(%s) Failed to registerApp, %d \n",__FUNCTION__, server_if);
       }
    }

    void btgatts_connection_cb(int conn_id, int server_if, int connected, bt_bdaddr_t *bda)
    {
        fprintf(stdout,"btgatts_connection_cb  gattctest \n ");
    }

    void btgatts_service_added_cb(int status, int server_if,
                                btgatt_srvc_id_t *srvc_id, int srvc_handle)
    {
        fprintf(stdout,"btgatts_service_added_cb \n");
       if (status == BT_STATUS_SUCCESS) {
          GattsServiceAddedEvent event;
           event.event_id =RSP_ENABLE_EVENT;
           event.status = status;
           event.server_if = server_if;
           memcpy(&event.srvc_id, srvc_id, sizeof(btgatt_srvc_id_t));
           event.srvc_handle = srvc_handle;
           gattctest->SetGATTCTESTSrvcData(&event);
           gattctest->AddCharacteristics();
       } else {
            fprintf(stdout, "(%s) Failed to Add_Service %d ",__FUNCTION__, server_if);
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
        fprintf(stdout,"btgatts_characteristic_added_cb \n");
       if (status == BT_STATUS_SUCCESS) {
           GattsCharacteristicAddedEvent event;
           event.event_id = RSP_ENABLE_EVENT;
           event.status = status;
           event.server_if = server_if;
           memcpy(&event.char_id, char_id, sizeof(bt_uuid_t));
           event.srvc_handle = srvc_handle;
           event.char_handle = char_handle;
           gattctest->SetGATTCTESTCharacteristicData(&event);
           gattctest->AddDescriptor();
       } else {
            fprintf(stdout, "(%s) Failed to Add Characteristics %d ",__FUNCTION__, server_if);
       }
    }

    void btgatts_descriptor_added_cb(int status, int server_if, bt_uuid_t *descr_id,
                                              int srvc_handle, int descr_handle)
    {
        fprintf(stdout,"btgatts_descriptor_added_cb \n");
       if (status == BT_STATUS_SUCCESS) {
           GattsDescriptorAddedEvent event;
           event.event_id = RSP_ENABLE_EVENT;
           event.status = status;
           event.server_if = server_if;
           memcpy(&event.descr_id, descr_id,sizeof(bt_uuid_t));
           event.srvc_handle = srvc_handle;
           event.descr_handle= descr_handle;
           gattctest->SetGATTCTESTDescriptorData(&event);
           gattctest->StartService();
        } else {
            fprintf(stdout, "(%s) Failed to add descriptor %d \n",__FUNCTION__, server_if);
        }
    }

    void btgatts_service_started_cb(int status, int server_if, int srvc_handle)
    {
        fprintf(stdout,"btgatts_service_started_cb \n");
      // gattctest->RegisterClient();
    }

    void btgatts_service_stopped_cb(int status, int server_if, int srvc_handle)
    {
        fprintf(stdout,"btgatts_service_stopped_cb \n");

      if (gattctest) {
          if (!status)
              gattctest->DeleteService();
      }
       fprintf(stdout,  "GATTCTEST Service stopped successfully, deleting the service");
    }

    void btgatts_service_deleted_cb(int status, int server_if, int srvc_handle)
    {
      fprintf(stdout,"btgatts_service_deleted_cb \n");

     if (gattctestAlertData != NULL ) {
          fprintf(stdout,"Diagnostic:(%s), freeing testAlert\n", __FUNCTION__);
         if (gattctestAlertData->srvc_id != NULL) {
             osi_free (gattctestAlertData->srvc_id);
	     gattctestAlertData->srvc_id = NULL;
	 }
         if (gattctestAlertData->char_id != NULL) {
	     osi_free (gattctestAlertData->char_id);
	     gattctestAlertData->char_id = NULL;
	 }
         free(gattctestAlertData);
	 gattctestAlertData = NULL;
      }
      if (gattctest) {
          if (!status) {
              gattctest->CleanUp(server_if);
              delete gattctest;
              gattctest = NULL;
          }
      }
       fprintf(stdout,"GATTCTEST Service stopped & Unregistered successfully\n");
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
        fprintf(stdout,"onCharacteristicWriteRequest \n");
       GattsRequestWriteEvent event;
       event.event_id = RSP_ENABLE_EVENT;
       event.conn_id = conn_id;
       event.trans_id = trans_id;
       memcpy(&event.bda, bda, sizeof(bt_uuid_t));
       event.attr_handle = attr_handle;
       event.offset = offset;
       event.length = length;
       event.need_rsp = need_rsp;
       event.is_prep = is_prep;
       event.value = value;
       gattctest->SendResponse(&event);
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

gattctestServerCallback *gattctestServerCb = NULL;
gattctestClientCallback *gattctestClientCb = NULL;

GattcTest::GattcTest(btgatt_interface_t *gatt_itf, Gatt* gatt)
{
     fprintf(stdout,"gattctest instantiated ");
    //gatt_interface = gatt_itf;
    gatt_interface = gatt->GetGattInterface();
    app_gatt = gatt;
    gattctestClientCb = new gattctestClientCallback;
    gattctestServerCb = new gattctestServerCallback;
}

GattcTest::~GattcTest()
{
     fprintf(stdout, "(%s) GATTCTEST DeInitialized\n",__FUNCTION__);
    delete(gattctestClientCb);
    delete(gattctestServerCb);
}

bool GattcTest::CopyUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    for (int i = 0; i < 16; i++) {
        uuid->uu[i] = 0x30;
    }
    return true;
}

bool GattcTest::CopyClientUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
    uuid->uu[0] = 0xff;
    for (int i = 1; i < 16; i++) {
        uuid->uu[i] = 0x30;
    }
    return true;
}

bool GattcTest::CopyGenUUID(bt_uuid_t *uuid)
{
    CHECK_PARAM(uuid)
     uuid->uu[0] = 0xfb;
     uuid->uu[1] = 0x34;
     uuid->uu[2] = 0x9b;
     uuid->uu[3] = 0x5f;
     uuid->uu[4] = 0x80;
     uuid->uu[5] =0x00;
     uuid->uu[6] = 0x00;
     uuid->uu[7] = 0x80;
     uuid->uu[8] =0x00;
     uuid->uu[9] = 0x10;
     uuid->uu[10] = 0x00;
     uuid->uu[11] = 0x00;
     uuid->uu[12] = 0x00;
     uuid->uu[13] = 0x00;
     uuid->uu[14] = 0x00;
     uuid->uu[15] = 0x00;

    return true;
}


bool GattcTest::CopyParams(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
{
    CHECK_PARAM(uuid_dest)
    CHECK_PARAM(uuid_src)

    for (int i = 0; i < 16; i++) {
        uuid_dest->uu[i] = uuid_src->uu[i];
    }
    return true;
}

bool GattcTest::MatchParams(bt_uuid_t *uuid_dest, bt_uuid_t *uuid_src)
{
    CHECK_PARAM(uuid_dest)
    CHECK_PARAM(uuid_src)

    for (int i = 0; i < 16; i++) {
        if(uuid_dest->uu[i] != uuid_src->uu[i]) {
             fprintf(stdout, "(%s) UUID Failed Matches\n",__FUNCTION__);
            return false;
        }
    }
     fprintf(stdout, "(%s) UUID Matches\n",__FUNCTION__);
    return true;
}

bool GattcTest::EnableGATTCTEST()
{
    fprintf(stdout, "(%s) Enable GATTCTEST Initiated \n",__FUNCTION__);
    CopyClientUUID(&client_uuid);
    CopyGenUUID(&gen_uuid);
    return gattctest->RegisterClient();
}

bool GattcTest::DisableGATTCTEST()
{
     fprintf(stdout, "(%s) Disable GATTCTEST Initiated",__FUNCTION__);

      if (gattctest) {
          UnregisterClient(GetGATTCTESTClientAppData()->clientIf);
          delete gattctest;
          gattctest = NULL;
      }
      return true;
}

bool GattcTest::RegisterApp()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bt_uuid_t server_uuid = GetGATTCTESTAttrData()->server_uuid;
    fprintf(stdout,"reg app addr is %d \n", GetGATTCTESTAttrData()->server_uuid);
    app_gatt->RegisterServerCallback(gattctestServerCb,&GetGATTCTESTAttrData()->server_uuid);
    return app_gatt->register_server(&server_uuid) == BT_STATUS_SUCCESS;
}

bool GattcTest::RegisterClient()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    app_gatt->RegisterClientCallback(gattctestClientCb,&client_uuid);
    return app_gatt->register_client(&client_uuid) == BT_STATUS_SUCCESS;
}

bool GattcTest::UnregisterClient(int client_if)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    app_gatt->UnRegisterClientCallback(client_if);
    return app_gatt->unregister_client(client_if) == BT_STATUS_SUCCESS;
}

bool GattcTest::ClientSetAdvData(char *str)
{
    bt_status_t        Ret;
    bool              SetScanRsp        = false;
    bool              IncludeName       = true;
    bool              IncludeTxPower    = false;
    int               min_conn_interval = RSP_MIN_CI;
    int               max_conn_interval = RSP_MAX_CI;

    return (app_gatt->set_adv_data(GetGATTCTESTClientAppData()->clientIf, SetScanRsp,
                                                IncludeName, IncludeTxPower, min_conn_interval,
                                                max_conn_interval, 0,strlen(str), str,
                                                strlen(str), str, 0,NULL) == BT_STATUS_SUCCESS);
}

void GattcTest::CleanUp(int server_if)
{
    UnregisterServer(server_if);
    UnregisterClient(GetGATTCTESTClientAppData()->clientIf);
}

bool GattcTest::UnregisterServer(int server_if)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "Gatt Interface Not present");
        return false;
    }
    app_gatt->UnRegisterServerCallback(server_if);
    return app_gatt->unregister_server(server_if) == BT_STATUS_SUCCESS;
}

bool GattcTest::StartAdvertisement()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
     fprintf(stdout,  "(%s) Listening on the interface (%d) ",__FUNCTION__,
            GetGATTCTESTAppData()->server_if);
    //SetDeviceState(WLAN_INACTIVE);
    return app_gatt->listen(GetGATTCTESTClientAppData()->clientIf, true);
}

bool GattcTest::StartScan()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGE(LOGTAG  "(%s) start scan",__FUNCTION__);
    return app_gatt->scan(true, GetGATTCTESTClientAppData()->clientIf);
}

bool GattcTest::StopScan()
{
     fprintf(stdout,"Stop scan GattInterface  =%p\n",GetGattInterface());
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGE(LOGTAG  "(%s) stop scan",__FUNCTION__);
     fprintf(stdout,"stopScan app_gatt =%p clientif =%d \n",app_gatt, GetGATTCTESTClientAppData()->clientIf);
    return app_gatt->scan(false, GetGATTCTESTClientAppData()->clientIf);
}

bool GattcTest::Connect(const bt_bdaddr_t *bd_addr)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGE(LOGTAG  "(%s) Connect",__FUNCTION__);
    return app_gatt->clientConnect(GetGATTCTESTClientAppData()->clientIf,bd_addr,true,GATT_TRANSPORT_LE);

}

bool GattcTest::Disconnect(const bt_bdaddr_t *bd_addr)
{
   if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    ALOGE(LOGTAG  "(%s) Disconnect",__FUNCTION__);
    return app_gatt->clientDisconnect(GetGATTCTESTConnectionData()->clientIf,bd_addr,GetGATTCTESTConnectionData()->conn_id);
}

bool GattcTest::SendAlert(int alert_level)
{
    char buf[UUID_STR_LEN];
    char srvc_id_buf[SRVCID_STR_LEN];
    char char_id_buf[CHARID_STR_LEN];

    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    if (gattctestClientCb->foundAlertService() == true) {


         fprintf(stdout, "sending alert now alert level =%d \n", alert_level);

        if (alert_level == LOW_ALERT ) {
             fprintf(stdout, "in LOW_ALERT %s\n", __func__);
            return gattctest->app_gatt->write_characteristic(gattctestAlertData->conn_id,gattctestAlertData->handle,1,2,0,"00");
        } else if (alert_level == MID_ALERT) {
             fprintf(stdout, "in MID_ALERT %s\n", __func__);
            return app_gatt->write_characteristic(gattctestAlertData->conn_id,gattctestAlertData->handle,1,2,0,"01");
        } else if (alert_level == HIGH_ALERT) {
             fprintf(stdout, "in HIGH_ALERT %s\n", __func__);
            return gattctest->app_gatt->write_characteristic(gattctestAlertData->conn_id,gattctestAlertData->handle,1,2,0,"02");
        }

    } else {
        fprintf(stdout," Matching Alert not found - dont send alert, try disc and connect again\n");
    }
    return false;
}

bool GattcTest::SearchService(int conn_id)
{
     if (GetGattInterface() == NULL) {
         ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
         return false;
     }
     ALOGE(LOGTAG  "(%s) SearchService",__FUNCTION__);

    if(!gattctestServData) { //To be freed up at disconnect/off.
        gattctestServData = (ServiceData *) (osi_malloc(sizeof(ServiceData)));
        memset(gattctestServData, 0, sizeof(ServiceData));
    }
    return app_gatt->search_service(conn_id, NULL);
}

bool GattcTest::SendResponse(GattsRequestWriteEvent *event)
{
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

    if(!strncasecmp((const char *)(event->value), "on", 2)) {
        response = 0;
    } else {
        response = -1;
    }

     fprintf(stdout, "(%s) Sending GATTCTEST response to write (%d) value (%s) State (%d)",__FUNCTION__,
            GetGATTCTESTAppData()->server_if, event->value,GetDeviceState());

    return app_gatt->send_response(event->conn_id, event->trans_id,
                                                         response, &att_resp);
}

bool GattcTest::HandleWlanOn()
{
    BtEvent *event = new BtEvent;
    CHECK_PARAM(event);
    event->event_id = SKT_API_IPC_MSG_WRITE;
    event->bt_ipc_msg_event.ipc_msg.type = BT_IPC_REMOTE_START_WLAN;
    event->bt_ipc_msg_event.ipc_msg.status = INITIATED;
    StopAdvertisement();
     fprintf(stdout, "(%s) Posting wlan start to main thread \n",__FUNCTION__);
    PostMessage (THREAD_ID_MAIN, event);
    return true;
}

bool GattcTest::StopAdvertisement()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
     fprintf(stdout, "(%s) Stopping listen on the interface (%d) \n",__FUNCTION__,
            GetGATTCTESTClientAppData()->clientIf);
    return app_gatt->listen(GetGATTCTESTClientAppData()->clientIf, false);
}

bool GattcTest::AddService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    btgatt_srvc_id_t srvc_id;
    srvc_id.id.inst_id = 0;   // 1 instance
    srvc_id.is_primary = 1;   // Primary addition
    srvc_id.id.uuid = GetGATTCTESTAttrData()->service_uuid;
    return app_gatt->add_service(GetGATTCTESTAppData()->server_if, &srvc_id,4)
                                                        ==BT_STATUS_SUCCESS;
}

bool GattcTest::DisconnectServer()
{
    int server_if = GetGATTCTESTConnectionData()->clientIf;
    bt_bdaddr_t bda;
    memcpy(&bda, &(GetGATTCTESTConnectionData()->bda),sizeof(bt_bdaddr_t));
    int conn_id = GetGATTCTESTConnectionData()->conn_id;
     fprintf(stdout,  "(%s) Disconnecting interface (%d), connid (%d) ",__FUNCTION__,
            server_if, conn_id);
    return app_gatt->serverDisconnect(server_if, &bda, conn_id) == BT_STATUS_SUCCESS;
}

bool GattcTest::DeleteService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bool status = false;
    int srvc_handle = GetGATTCTESTSrvcData()->srvc_handle;
    return app_gatt->delete_service(GetGATTCTESTAppData()->server_if,
                                                            srvc_handle) == BT_STATUS_SUCCESS;
}

bool GattcTest::AddCharacteristics()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }
    bt_uuid_t char_uuid;
    CopyParams(&char_uuid, &(GetGATTCTESTSrvcData()->srvc_id.id.uuid));
    int srvc_handle = GetGATTCTESTSrvcData()->srvc_handle;
    int server_if = GetGATTCTESTSrvcData()->server_if;
     fprintf(stdout,  "(%s) Adding Characteristics server_if (%d), srvc_handle (%d) \n",
            __FUNCTION__, server_if,srvc_handle);
    return app_gatt->add_characteristic(server_if, srvc_handle, &char_uuid,
                                                            GATT_PROP_WRITE, GATT_PERM_WRITE)
                                                            ==BT_STATUS_SUCCESS;
}

bool GattcTest::AddDescriptor(void)
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    bt_uuid_t desc_uuid;
    desc_uuid = GetGATTCTESTAttrData()->descriptor_uuid;
    int srvc_handle = GetGATTCTESTSrvcData()->srvc_handle;
    return app_gatt->add_descriptor(GetGATTCTESTAppData()->server_if,
                                                        srvc_handle, &desc_uuid,
                                                        GATT_PERM_READ) == BT_STATUS_SUCCESS;
}

bool GattcTest::StartService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    int srvc_handle = GetGATTCTESTSrvcData()->srvc_handle;
    return app_gatt->start_service(GetGATTCTESTAppData()->server_if,
                                                        srvc_handle, GATT_TRANSPORT_LE)
                                                        == BT_STATUS_SUCCESS;
}

bool GattcTest::StopService()
{
    if (GetGattInterface() == NULL) {
        ALOGE(LOGTAG  "(%s) Gatt Interface Not present",__FUNCTION__);
        return false;
    }

    int srvc_handle = GetGATTCTESTSrvcData()->srvc_handle;
    return app_gatt->stop_service(GetGATTCTESTAppData()->server_if,
                                                        srvc_handle) == BT_STATUS_SUCCESS;
}
