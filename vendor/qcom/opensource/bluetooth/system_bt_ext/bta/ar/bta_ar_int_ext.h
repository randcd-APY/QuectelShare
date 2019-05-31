/******************************************************************************
 *
 *  Copyright (C) 2008-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  This is the private interface file for the BTA audio/video registration
 *  module.
 *
 ******************************************************************************/
#ifndef BTA_AR_INT_EXT_H
#define BTA_AR_INT_EXT_H

typedef struct {
    UINT8           allocated;
    BD_ADDR         bd_addr;                      /* remote BD address */
    //UINT8           avdtp_connect_ind_mask;       /* Connection Indication Mask  */
    //UINT8           avdtp_disconnect_ind_mask;    /* DisConnection Indication Mask  */
    UINT8           sep_type;                     /* Sep Type of remote */
} tBTA_AR_AVDTP_CONNECTION_INFO;

tBTA_AR_AVDTP_CONNECTION_INFO avdtp_connection_info[10];
#define BTA_AV_RC_PROFILE_SINK  0x02
#define BTA_AV_RC_PROFILE_SRC  0x01

#define BTA_AR_EXT_AV_MASK      0x01
#define BTA_AR_EXT_AVK_MASK     0x02

#define AVDT_AR_EXT_CONNECT_IND_EVT     0x01
#define AVDT_AR_EXT_DISCONNECT_IND_EVT  0x02
#define AVDT_AR_EXT_CONNECT_REQ_EVT     0x04
#define AVDT_AR_EXT_DISCONNECT_REQ_EVT  0x08

void bta_ar_ext_init(void);
int get_index(BD_ADDR bd_addr);
tBTA_AR_AVDTP_CONNECTION_INFO* alloc_device(BD_ADDR bd_addr);
void dealloc_ar_device_info(BD_ADDR bd_addr);
void update_avdtp_connection_info(BD_ADDR bd_addr, UINT8 event, UINT8 sep_type);
UINT8 get_remote_sep_type(BD_ADDR bd_addr);

#endif /* BTA_AR_INT_EXT_H */
