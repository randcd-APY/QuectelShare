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
 *  This is the implementation for the audio/video registration module.
 *
 ******************************************************************************/

#include <string.h>
#include "bt_types.h"
#include "bta_ar_int_ext.h"
#include "btm_api.h"

/* AV control block */
/* structure to keep track of AVDTP_CONNECT_IND and AVDTP_DISCONNECT_IND */


void bta_ar_ext_init(void)
{
    /* initialize control block */
    memset(&avdtp_connection_info, 0, 10*sizeof(tBTA_AR_AVDTP_CONNECTION_INFO));
}
int get_index(BD_ADDR bd_addr)
{
    tBTA_AR_AVDTP_CONNECTION_INFO* p_conn_info = &avdtp_connection_info[0];
    for(int i = 0; i < AVDT_NUM_LINKS; i++,p_conn_info++)
    {
        if (p_conn_info->allocated && !memcmp(bd_addr, p_conn_info->bd_addr, sizeof(BD_ADDR)))
            return i;
    }
    /* we have not found this BD address  */
    return -1;
}

tBTA_AR_AVDTP_CONNECTION_INFO* alloc_device(BD_ADDR bd_addr)
{
    tBTA_AR_AVDTP_CONNECTION_INFO* p_conn_info = &avdtp_connection_info[0];
    for(int i = 0; i < AVDT_NUM_LINKS; i++,p_conn_info++)
    {
        if (!p_conn_info->allocated)
        {
            memcpy(p_conn_info->bd_addr, bd_addr,  sizeof(BD_ADDR));
            p_conn_info->allocated = 1;
            APPL_TRACE_DEBUG(" device allocated index = %d ",i);
            return p_conn_info;
        }
    }
    return NULL;
}
void dealloc_ar_device_info(BD_ADDR bd_addr)
{
    tBTA_AR_AVDTP_CONNECTION_INFO* p_conn_info = &avdtp_connection_info[0];
    for(int i = 0; i < AVDT_NUM_LINKS; i++,p_conn_info++)
    {
        if (p_conn_info->allocated && !memcmp(bd_addr, p_conn_info->bd_addr, sizeof(BD_ADDR)))
        {
            memset(p_conn_info, 0,  sizeof(tBTA_AR_AVDTP_CONNECTION_INFO));
            APPL_TRACE_DEBUG(" device deallocated index = %d ",i);
            return;
        }
    }
}
/* this function only updates database */
void update_avdtp_connection_info(BD_ADDR bd_addr, UINT8 event, UINT8 sep_type)
{
    tBTA_AR_AVDTP_CONNECTION_INFO* p_conn_info =  NULL;
    int index = get_index(bd_addr);
    APPL_TRACE_DEBUG("%s BD_ADDR [%x] [%x] [%x] [%x] [%x] [%x] event %d index %d sep_mask = %d",
        __FUNCTION__,bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3],bd_addr[4], bd_addr[5],
        event, index, sep_type);
    if((event == AVDT_AR_EXT_CONNECT_IND_EVT) || (event == AVDT_AR_EXT_CONNECT_REQ_EVT))
    {
        /* if there is no entry for this BD right now */
        if(index == -1)
        {
            p_conn_info = alloc_device(bd_addr);
            if (p_conn_info == NULL)
            {
                APPL_TRACE_DEBUG(" Not enough space for this device ");
                return;
            }
            p_conn_info->sep_type |= sep_type;
        }
        else if(index < AVDT_NUM_LINKS)
        {
            avdtp_connection_info[index].sep_type |= sep_type;
            return;
        }
    }
    if ((event == AVDT_AR_EXT_DISCONNECT_IND_EVT) || (event == AVDT_AR_EXT_DISCONNECT_REQ_EVT))
    {
        if(index == -1)
        {
            APPL_TRACE_DEBUG(" Device not found ");
            return;
        }
        else if(index < AVDT_NUM_LINKS)
        {
            avdtp_connection_info[index].sep_type &= (~sep_type);
            return;
        }
    }
}
UINT8 get_remote_sep_type(BD_ADDR bd_addr)
{
    int index = get_index(bd_addr);
    if (index == -1)
        return 0;
    return avdtp_connection_info[index].sep_type;
}

