/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#ifndef ANDROID_INCLUDE_BT_HF_VENDOR_H
#define ANDROID_INCLUDE_BT_HF_VENDOR_H

#define BT_PROFILE_HANDSFREE_VENDOR_ID "handsfree_vendor"

__BEGIN_DECLS

/* BIND type*/
typedef enum
{
   BTHF_VENDOR_BIND_SET,
   BTHF_VENDOR_BIND_READ,
   BTHF_VENDOR_BIND_TEST
} bthf_vendor_bind_type_t;

typedef enum {
    BTHF_VENDOR_HF_INDICATOR_STATE_DISABLED = 0,
    BTHF_VENDOR_HF_INDICATOR_STATE_ENABLED
} bthf_vendor_hf_indicator_status_t;

typedef enum {
    BTHF_VENDOR_VOIP_CALL_NETWORK_TYPE_MOBILE = 0,
    BTHF_VENDOR_VOIP_CALL_NETWORK_TYPE_WIFI
} bthf_vendor_voip_call_network_type_t;

typedef enum {
    BTHF_VENDOR_VOIP_STATE_STOPPED = 0,
    BTHF_VENDOR_VOIP_STATE_STARTED
} bthf_vendor_voip_state_t;

/** Vendor callback for HF indicators (BIND)
 */
typedef void (* bthf_bind_cmd_vendor_callback)(char* hf_ind, bthf_vendor_bind_type_t type, bt_bdaddr_t *bd_addr);

/** Vendor callback for HF indicator value (BIEV)
 */
typedef void (* bthf_biev_cmd_vendor_callback)(char* hf_ind_val, bt_bdaddr_t *bd_addr);

/** BT-HF Vendor callback structure. */
typedef struct {
    /** set to sizeof(BtHfVendorCallbacks) */
    size_t      size;
    bthf_bind_cmd_vendor_callback          bind_cmd_vendor_cb;
    bthf_biev_cmd_vendor_callback          biev_cmd_vendor_cb;
} bthf_vendor_callbacks_t;

/** Represents the standard BT-HF Vendor interface. */
typedef struct {
    /** set to sizeof(BtHfVendorInterface) */
    size_t          size;
    /**
     * Register the BtHf Vendor callbacks
     */
    bt_status_t (*init_vendor)( bthf_vendor_callbacks_t* callbacks);

    /** Response for BIND READ command and activation/deactivation of  HF indicator */
    bt_status_t (*bind_response_vendor) (int anum, bthf_vendor_hf_indicator_status_t status,
                                  bt_bdaddr_t *bd_addr);

    /** Response for BIND TEST command */
    bt_status_t (*bind_string_response_vendor) (const char* result, bt_bdaddr_t *bd_addr);

    /** Sends connectivity network type used by Voip currently to stack */
    bt_status_t (*voip_network_type_wifi_vendor) (bthf_vendor_voip_state_t is_voip_started,
                                           bthf_vendor_voip_call_network_type_t is_network_wifi);

   /** Closes the hf vednor interface. */
   void (*cleanup_vendor)(void);
} bthf_vendor_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_HF_VENDOR_H */
