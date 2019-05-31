/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This file contains the definition of the btm control block when
 *  BTM_DYNAMIC_MEMORY is used.
 *
 ******************************************************************************/

#include "bt_types.h"
#include "bt_target.h"
#include <string.h>
#include "btm_int.h"
#include "stack_config.h"

/* Global BTM control block structure
*/
#if BTM_DYNAMIC_MEMORY == FALSE
tBTM_CB  btm_cb;
#endif

/*******************************************************************************
**
** Function         btm_init
**
** Description      This function is called at BTM startup to allocate the
**                  control block (if using dynamic memory), and initializes the
**                  tracing level.  It then initializes the various components of
**                  btm.
**
** Returns          void
**
*******************************************************************************/
void btm_init (void)
{
    /* All fields are cleared; nonzero fields are reinitialized in appropriate function */
    memset(&btm_cb, 0, sizeof(tBTM_CB));
    btm_cb.page_queue = fixed_queue_new(SIZE_MAX);
    btm_cb.sec_pending_q = fixed_queue_new(SIZE_MAX);
    btm_cb.sec_collision_timer = alarm_new("btm.sec_collision_timer");
    btm_cb.pairing_timer = alarm_new("btm.pairing_timer");

#if defined(BTM_INITIAL_TRACE_LEVEL)
    btm_cb.trace_level = BTM_INITIAL_TRACE_LEVEL;
#else
    btm_cb.trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif
    /* Initialize BTM component structures */
    btm_inq_db_init();                  /* Inquiry Database and Structures */
    btm_acl_init();                     /* ACL Database and Structures */
    /* Security Manager Database and Structures */
    if (stack_config_get_interface()->get_pts_secure_only_mode())
        btm_sec_init(BTM_SEC_MODE_SC);
    else
        btm_sec_init(BTM_SEC_MODE_SP);
#if BTM_SCO_INCLUDED == TRUE
    btm_sco_init();                     /* SCO Database and Structures (If included) */
#endif

    btm_cb.sec_dev_rec = list_new(osi_free);

    btm_dev_init();                     /* Device Manager Structures & HCI_Reset */
}

void btm_free (void)
{
    if(btm_cb.page_queue)
    {
        fixed_queue_free(btm_cb.page_queue,NULL);
        btm_cb.page_queue =  NULL;
    }
    if(btm_cb.sec_pending_q)
    {
        fixed_queue_free(btm_cb.sec_pending_q,NULL);
        btm_cb.sec_pending_q =  NULL;
    }

    if (btm_cb.sec_collision_timer) {
        alarm_free(btm_cb.sec_collision_timer);
        btm_cb.sec_collision_timer = NULL;
    }
    if (btm_cb.pairing_timer) {
        alarm_free(btm_cb.pairing_timer);
        btm_cb.pairing_timer = NULL;
    }

    if (btm_cb.btm_inq_vars.remote_name_timer) {
        alarm_free(btm_cb.btm_inq_vars.remote_name_timer);
        btm_cb.btm_inq_vars.remote_name_timer = NULL;
    }

#if BTM_SCO_HCI_INCLUDED == TRUE
    {
        int i;
        for (i = 0; i < BTM_MAX_SCO_LINKS; i++) {
            if (btm_cb.sco_cb.sco_db[i].xmit_data_q) {
                fixed_queue_free(btm_cb.sco_cb.sco_db[i].xmit_data_q);
                btm_cb.sco_cb.sco_db[i].xmit_data_q = NULL;
            }
        }
    }
#endif

    if (btm_cb.sec_dev_rec) {
        list_free(btm_cb.sec_dev_rec);
        btm_cb.sec_dev_rec = NULL;
    }

    if (btm_cb.devcb.read_local_name_timer) {
        alarm_free(btm_cb.devcb.read_local_name_timer);
        btm_cb.devcb.read_local_name_timer = NULL;
    }
    if (btm_cb.devcb.read_rssi_timer) {
        alarm_free(btm_cb.devcb.read_rssi_timer);
        btm_cb.devcb.read_rssi_timer = NULL;
    }
    if (btm_cb.devcb.read_link_quality_timer) {
        alarm_free(btm_cb.devcb.read_link_quality_timer);
        btm_cb.devcb.read_link_quality_timer = NULL;
    }
    if (btm_cb.devcb.read_inq_tx_power_timer) {
        alarm_free(btm_cb.devcb.read_inq_tx_power_timer);
        btm_cb.devcb.read_inq_tx_power_timer = NULL;
    }
    if (btm_cb.devcb.qos_setup_timer) {
        alarm_free(btm_cb.devcb.qos_setup_timer);
        btm_cb.devcb.qos_setup_timer = NULL;
    }
    if (btm_cb.devcb.read_tx_power_timer) {
        alarm_free(btm_cb.devcb.read_tx_power_timer);
        btm_cb.devcb.read_tx_power_timer = NULL;
    }
}

