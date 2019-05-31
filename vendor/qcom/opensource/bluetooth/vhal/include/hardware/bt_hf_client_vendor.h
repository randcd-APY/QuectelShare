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

#ifndef ANDROID_INCLUDE_BT_HF_CLIENT_VENDOR_H
#define ANDROID_INCLUDE_BT_HF_CLIENT_VENDOR_H

#define BT_PROFILE_HANDSFREE_CLIENT_VENDOR_ID "handsfree_client_vendor"

__BEGIN_DECLS


/**
 * Vendor callback for sending cgmi indication to app
 */
typedef void (* bthf_client_cgmi_indication_vendor_callback) (const char *str);

/**
 * Vendor callback for sending cgmm indication to app
 */
typedef void (* bthf_client_cgmm_indication_vendor_callback) (const char *str);

/** BT-HF vendor callback structure. */
typedef struct {
    /** set to sizeof(BtHfClientVendorCallbacks) */
    size_t      size;
    bthf_client_cgmi_indication_vendor_callback   cgmi_vendor_cb;
    bthf_client_cgmm_indication_vendor_callback   cgmm_vendor_cb;
} bthf_client_vendor_callbacks_t;

/** Represents the standard BT-HF Vendor interface. */
typedef struct {

    /** set to sizeof(BtHfClientVendorInterface) */
    size_t size;
    /**
     * Register the BtHf vendor callbacks
     */
    bt_status_t (*init_vendor)(bthf_client_vendor_callbacks_t* callbacks);

   /** Closes the hf client vendor interface. */
   void  (*cleanup_vendor)( void );
} bthf_client_vendor_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_HF_CLIENT_VENDOR_H */
