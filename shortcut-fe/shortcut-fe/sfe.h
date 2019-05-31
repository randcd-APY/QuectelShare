/*
 * sfe.h
 *	Shortcut forwarding engine.
 *
 * Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Select whether we "hook" in below or above the Ethernet bridge layer.
 *
 * XXX - note that hooking below the bridge (set this value to 0) will
 * not currently work completely cleanly within Linux.  In order to make
 * this work properly we need to resync stats to Linux.  Arguably if we
 * want to do this we also need to validate that the source MAC address
 * of any packets is actually correct too.  Right now we're relying on
 * the bridge layer to do this sort of thing for us.
 */

#include <linux/ipc_logging.h>

#define SFE_HOOK_ABOVE_BRIDGE 0
#define MAX_INTF_LEN 50
#define SFE_MAX_CMD_LEN 100
#define SFE_CONFIG_MARK 1

#define WLAN_INTF_LEN 5
#define ECM_INTF_LEN 4
#define ETH_INTF_LEN 4

#define WLAN_INTF1 "wlan0"
#define WLAN_INTF2 "wlan1"
#define WLAN_INTF3 "wlan2"
#define WLAN_INTF4 "wlan3"
#define ECM_INTF "ecm0"
#define ETH_INTF "eth0"
#define IPCLOG_STATE_PAGES 2
#define __FILENAME__ (strrchr(__FILE__, '/') ? \
	strrchr(__FILE__, '/') + 1 : __FILE__)


#ifndef SFE_SUPPORT_IPV6
#define SFE_SUPPORT_IPV6
#endif

/*
 * Debug output verbosity level.
 */
#define DEBUG_LEVEL 2
static void *ipc_sfe_log_ctxt;
static void *ipc_sfe_log_ctxt_low;

enum {
	ERROR_LEVEL = 1,
	WARN_LEVEL = 2,
	INFO_LEVEL = 3,
	TRACE_LEVEL = 4,
};

static int debug_level = WARN_LEVEL;

#define DEBUG_ERROR(s, ...) \
do { \
	if (debug_level >= ERROR_LEVEL) { \
		printk("%s: %s[%u]: ERROR:", \
		__FILENAME__, __func__, __LINE__); \
		printk(s, ##__VA_ARGS__); \
		if (ipc_sfe_log_ctxt) { \
			ipc_log_string(ipc_sfe_log_ctxt, \
			"%s: %s[%u]: ERROR:" s, __FILENAME__, \
			__func__, __LINE__, ##__VA_ARGS__); \
		} \
	} \
} while (0)

#define DEBUG_WARN(s, ...) \
do { \
	if (debug_level >= WARN_LEVEL) { \
		printk("%s: %s[%u]: WARN:", \
		__FILENAME__, __func__, __LINE__); \
		printk(s, ##__VA_ARGS__); \
		if (ipc_sfe_log_ctxt) { \
			ipc_log_string(ipc_sfe_log_ctxt, \
			"%s: %s[%u]: WARN:" s, __FILENAME__, \
			__func__, __LINE__, ##__VA_ARGS__); \
		} \
	} \
} while (0)

#define DEBUG_INFO(s, ...) \
do { \
	if (debug_level >= INFO_LEVEL) { \
		printk("%s: %s[%u]: INFO:", \
		__FILENAME__, __func__, __LINE__); \
		printk(s, ##__VA_ARGS__); \
		if (ipc_sfe_log_ctxt) { \
			ipc_log_string(ipc_sfe_log_ctxt, \
			"%s: %s[%u]: INFO:" s, __FILENAME__, \
			__func__, __LINE__, ##__VA_ARGS__);\
		} \
	} \
} while (0)

#define DEBUG_TRACE(s, ...) \
do { \
	if (debug_level >= TRACE_LEVEL) { \
		printk("%s: %s[%u]: TRACE:", \
		__FILENAME__, __func__, __LINE__); \
		printk(s, ##__VA_ARGS__); \
		if (ipc_sfe_log_ctxt) { \
			ipc_log_string(ipc_sfe_log_ctxt, \
			"%s: %s[%u]: TRACE:" s, __FILENAME__, \
			__func__, __LINE__, ##__VA_ARGS__);\
		} \
	} \
} while (0)

#define IPC_DEBUG(s, ...) \
do { \
	if (ipc_sfe_log_ctxt) { \
		ipc_log_string(ipc_sfe_log_ctxt, \
		"%s: %s[%u]: IPC DEBUG:" s, __FILENAME__, \
		__func__, __LINE__, ##__VA_ARGS__);\
	} \
} while (0)

#define DEBUG_INFO_LOW(s, ...) \
	do { \
		if (ipc_sfe_log_ctxt_low) { \
			ipc_log_string(ipc_sfe_log_ctxt_low, \
			"%s: %s[%u]: INFO LOW:" s, __FILENAME__, \
			__func__, __LINE__, ##__VA_ARGS__);\
		} \
	} while (0)

#define DEBUG_TRACE_LOW(s, ...) \
do { \
	if (ipc_sfe_log_ctxt_low) { \
		ipc_log_string(ipc_sfe_log_ctxt_low, \
		"%s: %s[%u]: TRACE LOW:" s, __FILENAME__, \
		__func__, __LINE__, ##__VA_ARGS__);\
	} \
} while (0)

#define IPC_DEBUG_LOW(s, ...) \
do { \
	if (ipc_sfe_log_ctxt_low) { \
		ipc_log_string(ipc_sfe_log_ctxt_low, \
		"%s: %s[%u]: IPC DEBUG LOW:" s, __FILENAME__, \
		__func__, __LINE__, ##__VA_ARGS__);\
	} \
} while (0)


#ifdef CONFIG_NF_FLOW_COOKIE
typedef int (*flow_cookie_set_func_t)(u32 protocol, __be32 src_ip, __be16 src_port,
				      __be32 dst_ip, __be16 dst_port, u16 flow_cookie);
/*
 * sfe_register_flow_cookie_cb
 *	register a function in SFE to let SFE use this function to configure flow cookie for a flow
 *
 * Hardware driver which support flow cookie should register a callback function in SFE. Then SFE
 * can use this function to configure flow cookie for a flow.
 * return: 0, success; !=0, fail
 */
int sfe_register_flow_cookie_cb(flow_cookie_set_func_t cb);

/*
 * sfe_unregister_flow_cookie_cb
 *	unregister function which is used to configure flow cookie for a flow
 *
 * return: 0, success; !=0, fail
 */
int sfe_unregister_flow_cookie_cb(flow_cookie_set_func_t cb);

typedef int (*sfe_ipv6_flow_cookie_set_func_t)(u32 protocol, __be32 src_ip[4], __be16 src_port,
						__be32 dst_ip[4], __be16 dst_port, u16 flow_cookie);

/*
 * sfe_ipv6_register_flow_cookie_cb
 *	register a function in SFE to let SFE use this function to configure flow cookie for a flow
 *
 * Hardware driver which support flow cookie should register a callback function in SFE. Then SFE
 * can use this function to configure flow cookie for a flow.
 * return: 0, success; !=0, fail
 */
int sfe_ipv6_register_flow_cookie_cb(sfe_ipv6_flow_cookie_set_func_t cb);

/*
 * sfe_ipv6_unregister_flow_cookie_cb
 *	unregister function which is used to configure flow cookie for a flow
 *
 * return: 0, success; !=0, fail
 */
int sfe_ipv6_unregister_flow_cookie_cb(sfe_ipv6_flow_cookie_set_func_t cb);

#endif /*CONFIG_NF_FLOW_COOKIE*/
