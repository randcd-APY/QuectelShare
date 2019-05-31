/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SCM_MAIN_H__
#define __SCM_MAIN_H__

#include "comdef.h"

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define SCM_RADIO_SUP_MAX 2
#define SCM_IF_SUP_MAX 3
#define SCM_SAP_MAX 2
#define SCM_STA_MAX 1
#define SCM_CONF_FILE_PATH_MAX 80
#define SCM_SYS_CMD_MAX 256
#define SCM_WPA_SUP_EXT_CMD_ARGS 100
#define SCM_WLAN_SUP_SOCK_FILE "/var/run/scm_wlan_sup_sock_file"


#define MAX_OF(a,b)  ((a) > (b)) ? (a) : (b)

#define scm_sys_call_err(sc) scm_main_log(SCM_LOG_ERR, __func__, __LINE__, \
					 sc" failed: %d", errno)
#define scm_loge(fmt, ...) scm_main_log(SCM_LOG_ERR, __func__, __LINE__, \
					 fmt, ##__VA_ARGS__)
#define scm_logi(fmt, ...) scm_main_log(SCM_LOG_INFO, __func__, __LINE__, \
					 fmt, ##__VA_ARGS__)
#define scm_logd(fmt, ...) scm_main_log(SCM_LOG_DEBUG, __func__, __LINE__, \
					 fmt, ##__VA_ARGS__)
#define dbg_enter() scm_logd("Enter")
#define dbg_exit() scm_logd("Exit")
#define dbg_fn_hdr_enter() scm_logd("***** Enter *****\n")
#define dbg_fn_hdr_exit() scm_logd("***** Exit *****\n")

#define WLAN_IS_STA(wif_type) ((wif_type == WLAN_STA_1 || \
				wif_type == WLAN_STA_INT) ? 1 : 0)
#define WLAN_DRIVER_STR(status) (status == 1 ? "LOADED" : "UNLOAD")
#define WLAN_RADIO_STR(idx) (idx == ROME_DEV ? "ROM" : "TUF")
#define WLAN_BAND_STR(band) (band == WLAN_BAND_5G ? "5G" : \
                                (band == WLAN_BAND_2G ? "2G" : "AUTO"))

typedef uint8_t bool;

/** enum scm_log_lvl: Logging Level */
enum scm_log_lvl {
	SCM_LOG_ERR,
	SCM_LOG_INFO,
	SCM_LOG_DEBUG
};

/**
 * enum scm_recfg_timing: Reconfiguration priority values
 * @SCM_RECFG_AGGRESSIVE: Immediate reconfig after interface state change.
                          May involve Module unload if no active interface
 * @SCM_RECFG_DELAY: Reconfig after hysterisis delay
 * @SCM_RECFG_NONE: No Reconfig
 */
enum scm_recfg_timing {
	SCM_RECFG_AGGRESSIVE,
	SCM_RECFG_DELAY,
	SCM_RECFG_NONE
};

/** enum WLAN_DEV: WLAN device type */
enum WLAN_RADIO_DEV
{
	ROME_DEV,
	TUF_DEV,
	WLAN_RADIO_ANY,
	WLAN_RADIO_DEV_MAX = SCM_RADIO_SUP_MAX
};

/** enum wlan_drv_cfg: WLAN interface concurrency modes
 * @WLAN_DRV_CFG_1_STA_INT: Station only mode used for internal purposes
 * @WLAN_DRV_CFG_1_SAP - WLAN_DRV_CFG_3_SAP: WLAN Multi AP mode (CFG_X_SAP)
 * @WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE: WLAN 1 SAP and 1 STA mode
 * @WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE: WLAN 2 SAP and 1 STA mode
 * @WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE: WLAN 1 SAP and 1 STA. STA set in 4addr mode
 * @WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE: WLAN 2 SAP and 1 STA. STA set in 4addr mode
 */
enum WLAN_DRV_CFG
{
	WLAN_DRV_CFG_INVAL = 0,
	WLAN_DRV_CFG_1_STA_INT,
	WLAN_DRV_CFG_1_SAP,
	WLAN_DRV_CFG_2_SAP,
	WLAN_DRV_CFG_3_SAP,
	WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE,
	WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE,
	WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE,
	WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE,
	WLAN_DRV_CFG_MAX
};

/** enum wlan_if_type: WLAN Interface Type
 * @WLAN_SAP_1 to 3 - WLAN SAP concurrent interface in order
 * @WLAN_STA_1 - WLAN Station type. This index used for Rome in sd->wif[] array
 * @WLAN_STA_INT - WLAN_STA_INT is used for two purposes
 *		1. For STA only mode for CNE-QCMAP requirement
 *		2. As a index for TUF station in 5G SAP case in sd->wif[] array
 * @WLAN_SAP, WLAN_STA, WLAN_IF_ANY, WLAN_IF_NONE - For internal use as identifiers
 */
enum WLAN_IF_TYPE
{
	WLAN_SAP_1,
	WLAN_SAP_2,
	WLAN_SAP_3,
	// Add new SAP no before here
	WLAN_STA_1,
	WLAN_STA_INT,
	WLAN_IF_TYPE_MAX,
	WLAN_SAP,
	WLAN_STA,
	WLAN_IF_ANY,
	WLAN_IF_NONE,
};

/** enum wlan_if_control: WLAN Interfac Control
 * @WLAN_IF_CTRL_INVAL: SCM State machine table No action equivalent
 * @WLAN_IF_STOP, WLAN_IF_START, WLAN_IF_RESTART: Comes from SCM client. Also used for indications
 * @WLAN_IF_START_CMD: To Start an interface but dont send QMI indication
 * @WLAN_IF_STOP_CMD: To stop an interface but dont send QMI indication
 * @WLAN_IF_STA_SUSPEND: To stop Rome stat by wpa_cli disconnect instead of kill supplicant
 * @WLAN_IF_STA_RESUME: For Rome Sta to resume using wpa_Cli reconnect
 * @WLAN_IF_STA_STOP_ON_DFS: Stop Sta only if SAP 5G is operating in DFS ch.
 * @WLAN_IF_STA_START_NO_DFS: Start Sta only if SAP 5G is not operating in DFS ch.
 */
enum WLAN_IF_CTRL
{
	WLAN_IF_CTRL_INVAL = 0,
	WLAN_IF_STOP,
	WLAN_IF_START,
	WLAN_IF_RESTART,
	WLAN_IF_START_CMD,
	WLAN_IF_STOP_CMD,
	WLAN_IF_STA_SUSPEND,
	WLAN_IF_STA_RESUME,
	WLAN_IF_STA_STOP_ON_DFS,
	WLAN_IF_STA_START_NO_DFS,
	WLAN_IF_CTRL_MAX
};

/** enum wlan_ctrl: WLAN system control */
enum WLAN_SYS_CTRL
{
	WLAN_SYS_CTRL_INVAL = 0,
	WLAN_DISABLE,
	WLAN_ENABLE,
	WLAN_SYS_CTRL_MAX,
};

/** enum wlan_sta_state: Station connect state */
enum WLAN_STATION_STATUS
{
	WLAN_STATION_STATUS_INVAL = 0,
	WLAN_STA_CONNECT,
	WLAN_STA_DISCONNECT,
	WLAN_STATION_STATUS_MAX,
};

/** enum wlan_ind_type: WLAN indication types to client */
enum WLAN_IND_TYPE
{
	WLAN_IND_INVAL = 0,
	WLAN_DYN_RECFG_IND,
	WLAN_STA_STATE_IND,
	WLAN_SYS_CTRL_IND,
	WLAN_IND_TYPE_MAX
};

/** enum scm_op_band: Operating band */
enum WLAN_BAND
{
	WLAN_BAND_2G = 0,
	WLAN_BAND_5G,
	WLAN_BAND_ANY,
	WLAN_BAND_MAX = WLAN_BAND_ANY
};

/** struct scm_list: Common Linked list data */
struct scm_list
{
	void *data;
	struct scm_list *next;
};

/**
 * struct scm_evt_data: Polling event data
 * @fd: Event Socket / file descriptor
 * @cb_process: Callback for processing event
 * @cb_close: Callback on event deinit
 * @cb_data: Pointer to Event callback data
 * @name: Event Name for debugging
 */
struct scm_data;
typedef int (*scm_evt_cb) (struct scm_data *scm_data, int fd, void *);
struct scm_evt_data {
	int fd;
	scm_evt_cb cb_process;
	scm_evt_cb cb_close;
	void *cb_data;
	char *name;
};

/**
 * struct wlan_radio_data: WLAN Radio Device data
 *
 * @drv_name: WLAN driver modules name
 * @band_sup: Supported Bands
 * @idx: Radio Dev enum index
 * @pri_if_num: Primary wlan interface ID of this driver
 * @cnss_driver_idx: CNSS diag module assigned driver index
 * @drv_loaded: driver modules load status
 * @if_bitmap: Active interfaces of this driver
 * @band_restrict: Current active operating band
 * @ssr_drv_restart: restart driver or not when ssr happens. Curretly, only be
 * 		     used for TUF driver restart check when ROME ssr happens.
 * @sap_dfs_setup: Current sap dfs setup mode
 */
struct wlan_radio_data {
        char *drv_name;
        uint8_t band_sup;
	enum WLAN_RADIO_DEV idx;
	uint8_t pri_if_num;
	uint8_t cnss_driver_idx;
	uint8_t drv_loaded;
	uint32_t if_bitmap;
	enum WLAN_BAND band_restrict;
	bool ssr_drv_restart;
	int sap_dfs_setup;
};

/**
 * struct wlan_if_data: WLAN Interface data
 *
 * @num: Interface id used as interface number suffice to wlan
 * @type: Interface type
 * @act_ch: Operating channel
 * @band: operating band
 * @radio: Pointer to radio device on which interface is operating
 * @d_pid: WLAN interface daemon PID
 * @d_cfg: WLAN Interface daemon config file struct
 * @evt: Associated WPA Supplicant monitor event
 * @sup_mon: WPA Supplicant monitor socket
 */
struct wlan_if_data {
        uint8_t num;
        enum WLAN_IF_TYPE type;
	uint8_t act_ch;
	enum WLAN_BAND band;
        struct wlan_radio_data *radio;
        int d_pid;
	struct wlan_daemon_cfg *d_cfg;
	struct scm_evt_data *evt;
	struct wpa_ctrl *sup_mon;
};

/**
 * struct scm_ipc_data: SCM server - Client IPC data
 * @num_cli: Number of clients
 * @cli_list; Client linked list head
 * @priv: Non unix socket IPC (QMI) private data
 * @evt: Pointer to Client-Server IPC event data
 * @indication_cb: Indication callback
 * @priv_evt_process_cb: Non unix socket IPC (QMI) evt process callback
 * @priv_close_cb: Non unix socket IPC (QMI) close callback
 */
struct scm_ipc_data {
	int num_cli;
	struct scm_list *cli_list;
	void *priv;
	struct scm_evt_data *evt;
	void (*indication_cb)(struct scm_data *sd, struct wlan_if_data *wif,
                                        uint8_t ind_type, uint8_t ind_val);
	void (*priv_evt_process_cb)(struct scm_data *sd);
	void (*priv_close_cb)(struct scm_data *sd);
};

/**
 * Struct scm_data: SCM application private data
 *
 * @evt_fds: Poling event file desc set
 * @evt_max_fd: Used for Select event polling
 * @evt_list: linked list of Events
 * @nl_80211: NL80211 data struct for qcacld vendor cmd / event handling
 * @nl_svc: Driver Netlink usersock data
 * @ipc: Client server ipc data
 * @wlan_state: WLAN enabled state
 * @wlan_cfg: WLAN concurrency config
 * @wlan_if_cnt: WLAN concurrent Interface count
 * @wif: Array of supported WLAN Interface type data
 * @radio_cnt: Number of WLAN radio in platform
 * @sbs_sup: Platform single band simultaneous operation support
 * @sap_cfg_override: SAP hostapd.conf override for internal optimization
 * @sta_cfg_override: STA wpa_supplicant override for BSSID/ Network selection
 * @recfg_timing: Reconfiguration aggresiveness
 * @log_lvl: Logging level
 * @drv_mod_path: Driver Module path in rootfs
 * @drv_ini_path: Driver INI config file path in rootfs
 */
struct scm_data {
	fd_set evt_fds;
	int evt_max_fd;
	struct scm_list *evt_list;

	void *nl_80211;
	void *nl_svc;

	struct scm_ipc_data ipc;
	bool wlan_state;
	int wlan_cfg;
	int wlan_if_cnt;
        struct wlan_if_data *wif[WLAN_IF_TYPE_MAX];
	struct wlan_if_data *act_sta_wif;

	int radio_cnt;
	int sbs_sup;
	int sap_cfg_override;
	int sta_cfg_override;
	int recfg_timing;
	int log_lvl;
	char wlan_drv_ini_path[SCM_CONF_FILE_PATH_MAX];
};

void scm_main_log(int lvl, const char *fn, int line, const char *fmt, ...);
int scm_main_strlcpy(char *dst, const char *src, int len);
int scm_main_sys_cmd_res(char *res, int res_len, char *cmd, ...);
void scm_main_kill_exe(int pid);
int scm_main_list_add(struct scm_list **scm_list_head, void **data, int data_len);
int scm_main_list_del(struct scm_list **scm_list_head, void **data);
int scm_main_list_check_data(struct scm_list **scm_list_head, void *data);
int scm_main_list_check_list(struct scm_list **scm_list_head, void *list);
int scm_main_evt_list_add(int evt_fd, scm_evt_cb evt_cb_process,
			scm_evt_cb evt_cb_close, void *evt_cb_data,
			char *evt_name, struct scm_evt_data **evt);
void scm_main_evt_list_del(struct scm_evt_data **evt);
int scm_main_ipc_sock_init(struct scm_data *sd, const char *socket_file,
                        scm_evt_cb evt_cb_process, int flags);
void scm_main_get_conf_param_val(char *conf_file, char *param, char *val, int len);
void scm_main_cli_indication(struct scm_data *sd, struct wlan_if_data *wif,
                                        uint8_t ind_type, uint8_t ind_val);
#endif
