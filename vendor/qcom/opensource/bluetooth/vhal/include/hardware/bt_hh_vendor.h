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

#ifndef ANDROID_INCLUDE_BT_HH_VENDOR_H
#define ANDROID_INCLUDE_BT_HH_VENDOR_H

#define BT_PROFILE_HID_VENDOR_ID "hid_vendor"

__BEGIN_DECLS

/** Callback to send raw hid data .*/
typedef void (*  btvendor_raw_hid_data_callback)(uint8_t* rpt, uint16_t len,bool rpt_id_flag);

/** BT-HH vendor callback structure. */
typedef struct {
    /** set to sizeof(BtHhVendorCallbacks) */
    size_t      size;
    btvendor_raw_hid_data_callback raw_hid_data_cb;
} bthh_vendor_callbacks_t;

/** Represents the standard BT-HH Vendor interface. */
typedef struct {

    /** set to sizeof(BtHhVendorCallbacks) */
    size_t size;
    /**
     * Register the BtHh vendor callbacks
     */
    bt_status_t (*init_vendor)(bthh_vendor_callbacks_t* callbacks);

   /** Closes the hf client vendor interface. */
   void  (*cleanup_vendor)( void );
} bthh_vendor_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_HH_VENDOR_H */
