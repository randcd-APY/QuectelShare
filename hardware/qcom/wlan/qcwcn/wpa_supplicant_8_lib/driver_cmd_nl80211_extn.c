/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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
 *
 */

#include <netlink/object-api.h>
#include <linux/pkt_sched.h>
#include <dlfcn.h>
#include "driver_cmd_nl80211_extn.h"
#include "common.h"


int wpa_driver_oem_initialize(wpa_driver_oem_cb_table_t *oem_cb_table)
{
	wpa_driver_oem_get_cb_table_t *get_oem_table;
	wpa_driver_oem_cb_table_t *oem_cb_table_local = NULL;

	/* Return the callback table if it is already initialized*/
	if (oem_cb_table->wpa_driver_driver_cmd_oem_cb)
		return WPA_DRIVER_OEM_STATUS_SUCCESS;

#if __WORDSIZE == 64
	void* oem_handle = dlopen("/vendor/lib64/libwpa_drv_oem.so", RTLD_NOW);
#else
	void* oem_handle = dlopen("/vendor/lib/libwpa_drv_oem.so", RTLD_NOW);
#endif
	if (!oem_handle) {
		return WPA_DRIVER_OEM_STATUS_FAILURE;
	}

	get_oem_table = (wpa_driver_oem_get_cb_table_t *)dlsym(oem_handle,
						 "oem_generic_cb_table");
	if (!get_oem_table) {
		wpa_printf(MSG_ERROR, "%s: NULL oem callback table",
			   __FUNCTION__);
		return WPA_DRIVER_OEM_STATUS_FAILURE;
	}

	oem_cb_table_local = get_oem_table();

	if (!oem_cb_table_local ||
		!oem_cb_table_local->wpa_driver_driver_cmd_oem_cb) {
		wpa_printf(MSG_ERROR, "%s: oem module returned NULL table",
			   __FUNCTION__);
		return WPA_DRIVER_OEM_STATUS_FAILURE;
	}
	oem_cb_table->wpa_driver_driver_cmd_oem_cb =
                            oem_cb_table_local->wpa_driver_driver_cmd_oem_cb;
	wpa_printf(MSG_INFO, "%s: OEM lib initialized\n", __func__);

	return WPA_DRIVER_OEM_STATUS_SUCCESS;
}
