/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include "scm_wlan.h"
#include "scm_nl.h"
#ifdef FEATURE_DATA_LOG_QXDM
#include "ds_util.h"
#endif

#ifdef SCM_IPC_QMI_CLI
#include "scm_ipc_qmi.h"
#endif

/** enum scm_conf_file_param_index: SCM configuration file params */
enum scm_conf_file_param_index {
	SCM_LOG_LEVEL,
	SCM_DRV_INI_PATH,
	SCM_SAP_CFG_OVERRIDE,
	SCM_STA_CFG_OVERRIDE,
	SCM_RECFG_TIMING,
	SCM_SBS_SUP,
	SCM_RADIO_CNT,
	SCM_LOG_FILE,
	SCM_DS_LOG_DISABLE,
	SCM_PARAM_MAX,
};
enum {
	SCM_PARAM_TYPE_INT,
	SCM_PARAM_TYPE_STR
};

/** struct scm_conf_file_params_data: SCM config file param defaults */
struct scm_conf_file_params_data {
	char *name;
	int type;
	char *def;
	int min;
	int max;
} scm_conf_file_param [] = {
[SCM_DRV_INI_PATH] =
	{"drv_ini_path",         SCM_PARAM_TYPE_STR, "/lib/firmware/wlan/qcacld",   1, SCM_CONF_FILE_PATH_MAX},
[SCM_SAP_CFG_OVERRIDE] =
	{"sap_cfg_override",     SCM_PARAM_TYPE_INT, "0", 0, 0}, //Not supported
[SCM_STA_CFG_OVERRIDE] =
	{"sta_cfg_override",     SCM_PARAM_TYPE_INT, "0", 0, 0}, //Not supported
[SCM_RECFG_TIMING] =
	{"recfg_timing",         SCM_PARAM_TYPE_INT, "0", 0, 0}, //Not supported
[SCM_LOG_LEVEL] =
	{"log_level",            SCM_PARAM_TYPE_INT, "3", 1, 3},
[SCM_SBS_SUP] =
	{"sbs_sup",              SCM_PARAM_TYPE_INT, "0", 0, 0}, //Not supported
[SCM_RADIO_CNT] =
	{"radio_count",          SCM_PARAM_TYPE_INT, "2", 1, 2},
[SCM_LOG_FILE] =
	{"log_file",             SCM_PARAM_TYPE_STR, "",  1, SCM_CONF_FILE_PATH_MAX},
[SCM_DS_LOG_DISABLE] =
	{"ds_log_disable",       SCM_PARAM_TYPE_INT, "0", 0, 1},
};

static struct scm_data g_sd;
static FILE *scm_log_fp;
#ifdef FEATURE_DATA_LOG_QXDM
bool scm_ds_log = false;
bool scm_conf_ds_log_disable = false;
#endif

static int scm_main_set_config_param(int param_ind, char *val_str)
{
	int val = 0;

	if (scm_conf_file_param[param_ind].type == SCM_PARAM_TYPE_INT) {
		val = atoi(val_str);
		if (val < scm_conf_file_param[param_ind].min ||
			 val > scm_conf_file_param[param_ind].max)
			goto err;
	} else {
		if (strlen(val_str) <= scm_conf_file_param[param_ind].min ||
			strlen(val_str) > scm_conf_file_param[param_ind].max)
			goto err;
	}

	scm_logd("<Param>%s <Value>%s",
			scm_conf_file_param[param_ind].name, val_str);
	switch (param_ind) {
	case SCM_DRV_INI_PATH:
		scm_main_strlcpy(g_sd.wlan_drv_ini_path, val_str,
						SCM_CONF_FILE_PATH_MAX);
		break;
	case SCM_SAP_CFG_OVERRIDE:
		g_sd.sap_cfg_override = val;
		break;
	case SCM_STA_CFG_OVERRIDE:
		g_sd.sta_cfg_override = val;
		break;
	case SCM_RECFG_TIMING:
		g_sd.recfg_timing = val;
		break;
	case SCM_LOG_LEVEL:
		g_sd.log_lvl = val;
		break;
#ifdef FEATURE_DATA_LOG_QXDM
	case SCM_DS_LOG_DISABLE:
		scm_conf_ds_log_disable = val;
		break;
#endif
	case SCM_SBS_SUP:
		g_sd.sbs_sup = val;
		break;
	case SCM_RADIO_CNT:
		g_sd.radio_cnt = val;
		break;
	case SCM_LOG_FILE:
		scm_loge("%s",val_str);
		if (strlen(val_str)) {
			scm_log_fp = fopen(val_str, "a");
			if (!scm_log_fp)
				scm_loge("Log file open failed");
			else
				setlinebuf(scm_log_fp);
		}
		break;
	}
	return 0;
err:
	return -1;
}

void scm_main_get_conf_param_val(char *conf_file, char *param, char *val, int len)
{
	char *nl;

	if (!conf_file || !param || !val)
		return;
	scm_main_sys_cmd_res(val, len,
		"cat %s | grep -v \"#\" | grep %s | awk -F '=' '{print $2}'",
							conf_file, param);
	nl = strchr(val, '\n');
	if (nl)
		*nl='\0';
}

static int scm_main_parse_conf_file(char *conf_file)
{
	FILE *fp;
	char buf[512];
	int i;

	scm_loge("%s", conf_file);

	fp = fopen(conf_file, "r");
	if (!fp) {
		scm_loge("Open Failed");
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp)) {
		buf[sizeof(buf) - 1] = '\0';
		if (buf[0] == '\0' || buf[0] == '#')
			continue;

		for (i = 0; i < SCM_PARAM_MAX; i++) {
			char *val_str, *nl;
			if (strstr(buf, scm_conf_file_param[i].name) == NULL)
				continue;

			val_str = strstr(buf, "=");
			if (!val_str)
				goto err;
			val_str++;

			nl = strchr(val_str, '\n');
			if (nl)
				*nl='\0';
			if (scm_main_set_config_param(i, val_str))
				goto err;
		}
	}
	return 0;
err:
	scm_loge("Invalid Param value for %s", scm_conf_file_param[i].name);
	return -1;
}

static void scm_main_init_def(void)
{
	int i;

	for (i = 0; i < SCM_PARAM_MAX; i++) {
		scm_main_set_config_param(i, scm_conf_file_param[i].def);
	}

}
void scm_main_exit()
{
	struct scm_list *list;
	struct scm_evt_data **evt;

	scm_wlan_disable(&g_sd);
	list = g_sd.evt_list;
	while (list != NULL) {
		evt = (struct scm_evt_data **)&list->data;
		list = list->next;
		scm_main_evt_list_del(evt);
	}
	if (g_sd.ipc.priv_close_cb)
		(g_sd.ipc.priv_close_cb)(&g_sd);
}

static void scm_main_sig_handler(int signal)
{
	scm_loge("Signal %d", signal);

	switch (signal) {
	case SIGTERM:
	case SIGINT:
		scm_main_exit();
		exit(0);
		break;
	default:
		scm_loge("Signal %d Not handled", signal);
		break;
	}
}

void scm_main_log(int lvl, const char *fn, int line, const char *fmt, ...)
{
	FILE *fp = stderr;
	char buf[512];
	va_list argp;
	if (lvl > g_sd.log_lvl)
		return;
	if (scm_log_fp)
		fp = scm_log_fp;

	va_start(argp, fmt);
	vsnprintf(buf, sizeof(buf), fmt, argp);
	va_end(argp);

#ifdef FEATURE_DATA_LOG_QXDM
	if (scm_ds_log) {
		int scm_ds_log_lvl[] = {MSG_LEGACY_ERROR, MSG_LEGACY_HIGH,
								MSG_LEGACY_LOW};
		if (scm_ds_log_lvl[lvl] & MSG_BUILD_MASK_MSG_SSID_WLAN_CP) {
			const msg_const_type scm_ds_msg = {
				{line, MSG_SSID_WLAN_CP, scm_ds_log_lvl[lvl]},
				buf, fn};
			msg_sprintf(&scm_ds_msg);
		}
	} else
#endif
	fprintf(fp, "%04d:%s: %s \n", line, fn, buf);


	return;
}

int scm_main_strlcpy(char *dst, const char *src, int dst_len)
{
	int i = 0;
	if (!dst || !src) {
		if (dst)
			dst[0] = '\0';
		goto end;
	}
	//Reserve last byte for NULL
	dst_len--;
	for (i = 0; (i < dst_len) && *src; i++)
		*dst++ = *src++;
	*dst = '\0';
end:
	return i;
}

int scm_main_sys_cmd_res(char *res, int res_len, char *cmd, ...)
{
	FILE *pf;
	char cmd_buf[SCM_SYS_CMD_MAX];
	va_list argp;

	if (res)
		*res ='\0';


	va_start(argp, cmd);
	vsnprintf(cmd_buf, sizeof(cmd_buf), cmd, argp);
	va_end(argp);

	pf = popen(cmd_buf, "r");
	if (!pf)
		return -1;


	while (res && res_len > 0) {
		if (fgets(res + strlen(res), res_len - 1, pf) == NULL)
			break;
		res_len -= strlen(res);
	}
	pclose(pf);

	scm_logd("<CMD>%s <RES>%s", cmd_buf, res);
	return 0;
}

void scm_main_kill_exe(int pid)
{
	int retry = 0;
	char res[32];
#define SCM_KILL_RETRY_LIMIT 5

	// kill with SIGTERM first and if pid still exists kill with SIGKILL
	scm_main_sys_cmd_res(NULL, 0, "kill -%d %d", SIGTERM, pid);
	while (retry++ < SCM_KILL_RETRY_LIMIT) {
		usleep(300000);
		scm_main_sys_cmd_res(res, 32, "ps -o pid | grep %d", pid);
		if (res[0] == '\0')
			break;
	}

	if (retry >= SCM_KILL_RETRY_LIMIT) {
		scm_logd("Force kill %d", pid);
		scm_main_sys_cmd_res(NULL, 0, "kill -%d %d", SIGKILL, pid);
		retry = 0;
		while (retry++ < SCM_KILL_RETRY_LIMIT) {
			usleep(100000);
			scm_main_sys_cmd_res(res, 32, "ps -o pid | grep %d", pid);
			if (res[0] == '\0')
				break;
		}
	}
	if (retry >= SCM_KILL_RETRY_LIMIT)
		scm_loge("FATAL: Failed to kill %d", pid);
	else
		scm_loge("Killed %d", pid);
}

int scm_main_list_add(struct scm_list **scm_list_head, void **data, int data_len)
{
	struct scm_list *list;

	list = (struct scm_list *) malloc(sizeof(struct scm_list));
	if (!list)
		return -ENOMEM;

	*data = malloc(data_len);
	if (!*data){
		free(list);
		return -ENOMEM;
	}

	list->data = *data;
	if ((*scm_list_head) == NULL) {
		(*scm_list_head) = list;
		(*scm_list_head)->next = NULL;
	} else {
		list->next = (*scm_list_head);
		(*scm_list_head) = list;
	}

	memset(*data, 0, data_len);
	return 0;
}

int scm_main_list_del(struct scm_list **scm_list_head, void **data)
{
	struct scm_list *list, *prev_entry;

	if ((*scm_list_head) == NULL)
		return -ENOENT;

	list = (*scm_list_head);
	if ((*scm_list_head)->data == *data) {
		(*scm_list_head) = list->next;
		goto rel;
	}

	prev_entry = list;
	while (list != NULL) {
		if (list->data == *data) {
			prev_entry->next = list->next;
			goto rel;
		} else {
			prev_entry = list;
			list = list->next;
		}
	}

	return -ENOENT;
rel:
	free(list->data);
	*data = NULL;
	free(list);
	return 0;
}

int scm_main_list_check_data(struct scm_list **scm_list_head, void *entry)
{
	struct  scm_list *list;

	list = *scm_list_head;

	while (list != NULL) {
		if (list->data == entry)
			return 0;
		else
			list = list->next;
	}
	return -ENOENT;
}

int scm_main_list_check_list(struct scm_list **scm_list_head, void *entry)
{
	struct  scm_list *list;

	list = *scm_list_head;

	while (list != NULL) {
		if (list == entry)
			return 0;
		else
			list = list->next;
	}
	return -ENOENT;
}

/**
 * scm_main_evt_list_add() - Add entry to Main Loop Event Polling list
 * @evt_fd: Event file descriptor
 * @evt_cb_process: pointer to event process callback
 * @evt_cb_close: pointer to event close callback
 * @evt_cb_data: pointer to event callback data
 * @evt_name: pointer to event name
 * @evt: Return data struct of created event
 *
 * Add event to main loop event monitoring
 *
 * Return: 0 on success, error number otherwise.
 */
int scm_main_evt_list_add(int evt_fd, scm_evt_cb evt_cb_process,
				scm_evt_cb evt_cb_close, void *evt_cb_data,
				char *evt_name, struct scm_evt_data **evt)
{

	if (scm_main_list_add(&g_sd.evt_list, (void **) evt,
						sizeof(struct scm_evt_data)))
		return -1;

	if (!(*evt))
		return -1;

	(*evt)->fd = evt_fd;
	(*evt)->cb_close = evt_cb_close;
	(*evt)->cb_data = evt_cb_data;
	(*evt)->cb_process = evt_cb_process;
	(*evt)->name = evt_name;
	g_sd.evt_max_fd = MAX_OF(g_sd.evt_max_fd, evt_fd);
	FD_SET(evt_fd, &g_sd.evt_fds);
	scm_logd("Add evt: %p %s", (*evt), (*evt)->name);

	return 0;
}

/**
 * scm_main_evt_list_del() - Remove event from Main loop event polling list
 * @evt: pointer to Event to be removed
 *
 * Return: 0 on success, error number otherwise.
 */
void scm_main_evt_list_del(struct scm_evt_data **evt)
{
	int find_max_fd = 0;

	scm_logd("Remove evt: %p %s", (*evt), (*evt)->name);
	FD_CLR((*evt)->fd, &g_sd.evt_fds);
	if ((*evt)->fd != g_sd.evt_max_fd)
		find_max_fd = 1;;

	if ((*evt)->cb_close)
		(*evt)->cb_close(&g_sd, (*evt)->fd, (*evt)->cb_data);

	scm_main_list_del(&g_sd.evt_list, (void **)evt);
	if (find_max_fd) {
		struct scm_list *list;
		struct scm_evt_data *ev;

		g_sd.evt_max_fd = 0;
		list = g_sd.evt_list;
		while (list != NULL) {
			ev = (struct scm_evt_data *)list->data;
			g_sd.evt_max_fd = MAX_OF(g_sd.evt_max_fd, ev->fd);
			list = list->next;
		}
	}
}

/**
 * scm_main_cli_indication: Indicate SCM client about wlan event
 * @sd: Pointer to scm_data
 * @wif: Pointer to WLAN Interface data
 * @ind_type: Indication type
 * @ind_val: Indication value
 *
 * Return: None
 */
void scm_main_cli_indication(struct scm_data *sd, struct wlan_if_data *wif,
                                        uint8_t ind_type, uint8_t ind_val)
{
	if (sd->ipc.indication_cb)
		(sd->ipc.indication_cb)(sd, wif, ind_type, ind_val);
}

/**
 * scm_main_ipc_evt_handler() - Unix socket IPC event handler
 * @sd: pointer to scm data struct
 * @fd: Unix named socket file descriptor
 * @cb_data: pointer to callback data
 *
 * Return: 0 on success, error number otherwise.
 */
int scm_main_ipc_evt_handler(struct scm_data *sd, int fd, void *cb_data)
{
	return 0;
}

/**
 * scm_main_ipc_evt_close() - Unix socket IPC event close
 * @sd: pointer to scm data struct
 * @fd: Unix named socket file descriptor
 * @cb_data: pointer to callback data
 *
 * Return: 0 on success, error number otherwise.
 */
int scm_main_ipc_evt_close(struct scm_data *sd, int fd, void *cb_data)
{
	close(fd);
	return 0;
}

/**
 * scm_main_ipc_sock_init() - Thread IPC init
 * @sd: pointer to scm data struct
 * @socket_file: UNIX named socket path
 * @evt_cb_process: Associated event for name socket
 * @flags: File desc flags
 *
 * This function is used for socket IPC init for SCM threads and main loop
 *
 * Return: 0 on success, error number otherwise.
 */
int scm_main_ipc_sock_init(struct scm_data *sd, const char *socket_file,
			scm_evt_cb evt_cb_process, int flags)
{
	int sock_fd;
	struct sockaddr_un ipc;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd < 0) {
		scm_sys_call_err("socket()");
		return -1;
	}

	if (fcntl(sock_fd, F_SETFL, flags) < 0) {
		scm_sys_call_err("fcntl()");
		return -1;
	}


	ipc.sun_family = AF_UNIX;
	scm_main_strlcpy(ipc.sun_path, socket_file, strlen(socket_file));
	unlink(ipc.sun_path);
	if (bind(sock_fd, (struct sockaddr *)&ipc,
			strlen(ipc.sun_path) + sizeof(ipc.sun_family)) < 0) {
		scm_sys_call_err("bind()");
		return -1;
	}
	scm_main_evt_list_add(sock_fd, evt_cb_process, scm_main_ipc_evt_close,
					 NULL, "Thread IPC", &sd->ipc.evt);

	scm_loge("Success");
	return 0;
}

/**
 * main() - Smart Connection Manager main loop
 * @argc: Program arg count
 * @argv[]: Array of program arg params
 *
 * Return: 0 on success, error number otherwise.
 */
int main(int argc, char *argv[])
{
	int dmon = 0, c, ret = -1;
	fd_set sel_fd_set;
	char *conf_file = NULL;

	signal(SIGTERM, scm_main_sig_handler);
	signal(SIGINT, scm_main_sig_handler);
	signal(SIGUSR1, SIG_IGN);

	while (1) {
		c = getopt(argc, argv, "Bdnc:");

		if (c < 0)
			break;

		switch (c) {
		case 'B':
			dmon = 1;
			break;
		case 'c':
			conf_file = optarg;
			break;
		case 'h':
		default:
			fprintf(stderr, "Usage: %s [options]\n"
				"   -B: Run as a daemon\n"
				"   -c: <path/file> Config file\n"
				"   -h: Usage help\n"
				, argv[0]);
			exit(0);
			break;
		}
	}

	if (dmon && daemon(0, 0)) {
		scm_sys_call_err("daemon");
		exit(EXIT_FAILURE);
	}

	scm_main_init_def();

	if (conf_file)
		scm_main_parse_conf_file(conf_file);

#ifdef FEATURE_DATA_LOG_QXDM
	/*Initialize the Diag for QXDM logs*/
	if (scm_conf_ds_log_disable == false) {
		scm_ds_log = Diag_LSM_Init(NULL);
		if (scm_ds_log == false)
			scm_loge("Diag_LSM_Init fail");
	}
#endif
	// Kill legacy wlan_services used for single wifi
	ret = system("killall -9 wlan_services");
	scm_logi("wlan_services killed, ret=%d, errno=%d", ret, errno);
	// Create tmp folder for SCM runtime files
	ret = system("mkdir -p /var/run/scm/etc/misc/wifi");
	scm_logi("mkdir for SCM runtime files, ret=%d, errno=%d", ret, errno);

#ifdef SCM_IPC_QMI_CLI
	if (scm_ipc_qmi_init(&g_sd))
		goto err;
#endif
	if (scm_nl_80211_init(&g_sd))
		goto err;

	if (scm_main_ipc_sock_init(&g_sd, SCM_WLAN_SUP_SOCK_FILE,
			scm_main_ipc_evt_handler, (O_CLOEXEC | O_NONBLOCK)))
		goto err;

	if (scm_nl_svc_init(&g_sd))
		goto err;

	ret = 0;
	scm_loge("Event loop start");
	while (1) {
		struct scm_list *list;
		struct scm_evt_data *evt;

		sel_fd_set = g_sd.evt_fds;
		ret = select(g_sd.evt_max_fd + 1, &sel_fd_set, NULL, NULL,
									NULL);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			else {
				scm_sys_call_err("select");
				return -1;
			}
		}

		if (g_sd.ipc.priv_evt_process_cb)
			(g_sd.ipc.priv_evt_process_cb)(&g_sd);

		list = g_sd.evt_list;
		while (list != NULL) {
			//Evt entries can be removed on evt process. So check
			if (scm_main_list_check_list(&g_sd.evt_list, list) < 0) {
				list = g_sd.evt_list;
				continue;
			}
			evt = list->data;
			if (evt && FD_ISSET(evt->fd, &sel_fd_set)) {
				//Clear Evt on processsing start
				FD_CLR(evt->fd, &sel_fd_set);
				evt->cb_process(&g_sd, evt->fd, evt->cb_data);
			}
			list = list->next;
		}
	}
err:
	scm_main_exit();
	return ret;
}

