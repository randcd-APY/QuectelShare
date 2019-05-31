/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "scm_nl.h"

#include "nl80211_copy.h"

#include <net/if.h>
#include <linux/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <qcacld/wlan_nlink_common.h>

#define QCA_WLAN_VENDOR_ATTR_IFINDEX 4
#define QCA_OUI 0x001374

#define QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO 61
#define QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX 3

#define QCA_NL80211_VENDOR_SUBCMD_SETBAND 105
#define QCA_WLAN_VENDOR_ATTR_SETBAND_VALUE 12

#define QCA_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY 10
#define QCA_WLAN_VENDOR_ATTR_AVOID_FREQ_START_FREQ_LIST 1
#define QCA_WLAN_VENDOR_ATTR_AVOID_FREQ_END_FREQ_LIST 2

#define QCA_NL80211_VENDOR_SUBCMD_ACS_POLICY 116
#define QCA_WLAN_VENDOR_ATTR_ACS_DFS_MODE 1
#define QCA_WLAN_VENDOR_ATTR_ACS_CHANNEL_HINT 2

#define QCA_NL80211_VENDOR_SUBCMD_STA_CONNECT_ROAM_POLICY   117
#define QCA_WLAN_VENDOR_ATTR_STA_DFS_MODE 1
#define QCA_WLAN_VENDOR_ATTR_STA_SKIP_UNSAFE_CHAN 2

#define QCA_NL80211_VENDOR_SUBCMD_SET_SAP_CONFIG 118
#define QCA_WLAN_VENDOR_ATTR_SAP_CONFIG_CHAN 1

#define WLAN_SVC_WLAN_AUTO_SHUTDOWN_CANCEL_IND 0x10C

#define SCM_NL_80211_CMD_RUN 1
#define SCM_NL_80211_CMD_SUCCESS 0

#ifndef WLAN_SVC_FW_SHUTDOWN_IND
#define WLAN_SVC_FW_SHUTDOWN_IND 0x10E
#endif

#define for_each_nl_attr(attr_cur, nl_attr_list, attr_last) \
		for (attr_cur = (struct nlattr *)nla_data(nl_attr_list), \
			attr_last = nla_len(nl_attr_list); \
			nla_ok(attr_cur, attr_last); \
			attr_cur = nla_next(attr_cur, &(attr_last)))

/** enum qca_set_band: QCA Vendor Attr values for SET BAND Vendor command */
enum qca_set_band {
        QCA_SETBAND_AUTO,
        QCA_SETBAND_5G,
        QCA_SETBAND_2G,
};

/** struct scm_nl_80211_data: NL80211 Vendor netlink data structure */
struct scm_nl_80211_data
{
	struct nl_sock *cmd_sock;
	struct nl_sock *evt_sock;
	struct genl_family *family_ptr;
	struct nl_cache *cache;
	int id;
	struct nl_cb *cmd_cb;
	struct nl_cb *evt_cb;
	struct scm_evt_data *evt;
};

/** struct scm_nl_svc_data: Driver Netlink usersock data structure */
struct scm_nl_svc_data
{
        struct nlmsghdr *msg;
	struct scm_evt_data *evt;
};

struct scm_nl_80211_handler_args
{
	const char *group;
	int num;
};

static int svc_wlan_active = 0;

static void scm_nl_80211_evt_process_avoid_freq(void *cb_data,
					struct nlattr *nl_vendor_data_attr)
{
	struct scm_data *sd = (struct scm_data *)cb_data;
	struct wlan_if_data *tuf_wif;

	//Find if tuf is active
	tuf_wif = scm_wlan_get_wif(sd, NULL, WLAN_IF_ANY, WLAN_BAND_ANY, TUF_DEV);
	if (tuf_wif) {
		scm_logd("Force TUF Driver to update unsafe channel list");
		scm_nl_80211_avoid_freq(sd, tuf_wif->radio);
	}
}

int scm_nl_80211_evt_handler(struct nl_msg *msg, void *cb_data)
{
	int vendor_id, subcmd;
	struct genlmsghdr *gen_nlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *attr_list[NL80211_ATTR_MAX + 1];

	nla_parse(attr_list, NL80211_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
	      genlmsg_attrlen(gen_nlh, 0), NULL);

	if (gen_nlh->cmd != NL80211_CMD_VENDOR &&
				(!attr_list[NL80211_ATTR_VENDOR_ID] ||
				!attr_list[NL80211_ATTR_VENDOR_SUBCMD] ||
				!attr_list[NL80211_ATTR_VENDOR_DATA])) {
		scm_loge("Invalid Vendor Data or sub command ");
		return 0;
	}
        vendor_id = nla_get_u32(attr_list[NL80211_ATTR_VENDOR_ID]);
        subcmd = nla_get_u32(attr_list[NL80211_ATTR_VENDOR_SUBCMD]);

	if (vendor_id != QCA_OUI)
		return 0;

	switch (subcmd) {
	case QCA_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY:
		scm_logi("QCA_NL80211_VENDOR_SUBMCD_AVOID_FREQ");
		scm_nl_80211_evt_process_avoid_freq(cb_data,
					attr_list[NL80211_ATTR_VENDOR_DATA]);
		break;
	default:
		scm_loge("Unhandled event :%d", subcmd);
		break;
	}
	return NL_SKIP;
}


int  scm_nl_80211_evt_fd_handler(struct scm_data *sd, int fd, void *arg)
{
	struct scm_nl_80211_data *nl_80211;
	if (!sd)
		return 0;

	nl_80211 = sd->nl_80211;
	scm_logd("NL80211 Event recieved");
	nl_recvmsgs(nl_80211->evt_sock, nl_80211->evt_cb);
	return 0;
}

int scm_nl_80211_error_handler(struct sockaddr_nl *nla,
				struct nlmsgerr *error, void *cb_data)
{
	int *ret_val = (int *)cb_data;
	*ret_val = error->error;
	return NL_STOP;
}

int scm_nl_80211_seq_check(struct nl_msg *msg, void *cb_data)
{
	//No action
	return NL_OK;
}

int scm_nl_80211_finish_handler(struct nl_msg *msg, void *cb_data)
{
	int *ret_val = (int *)cb_data;
	*ret_val = SCM_NL_80211_CMD_SUCCESS;
	return NL_SKIP;
}

int scm_nl_80211_ack_handler(struct nl_msg *msg, void *cb_data)
{
	int *ret_val = (int *)cb_data;
	*ret_val = SCM_NL_80211_CMD_SUCCESS;
	return NL_STOP;
}

static int scm_nl_80211_cmd_init(struct nl_msg **msg, struct nl_cb **cb,
			int (*cmd_handler) (struct nl_msg *msg, void *cb_data),
			void *cb_data, int **cb_err)
{
	*cb_err = malloc(sizeof(int));
	*msg  = nlmsg_alloc();
	if (!(*msg) || !(*cb_err))
		return -ENOMEM;

	*cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!(*cb)) {
		nlmsg_free(*msg);
		return -ENOMEM;
	}

	nl_cb_err(*cb, NL_CB_CUSTOM, scm_nl_80211_error_handler,
							*cb_err);
	nl_cb_set(*cb, NL_CB_FINISH, NL_CB_CUSTOM,
		 scm_nl_80211_finish_handler, *cb_err);
	nl_cb_set(*cb, NL_CB_ACK, NL_CB_CUSTOM,
		 scm_nl_80211_ack_handler, *cb_err);
	if (cmd_handler)
		nl_cb_set(*cb, NL_CB_VALID, NL_CB_CUSTOM, cmd_handler,
							 cb_data);

	return 0;
}

static int scm_nl_80211_cmd_run(struct nl_sock *sock, struct nl_msg *msg,
				struct nl_cb *cb, int *cb_err, int radio_idx)
{
	int ret = 0;

	if (msg == NULL || cb == NULL || cb_err == NULL)
		return -1;

	*cb_err = SCM_NL_80211_CMD_RUN;
	//QCMAP may do ifdown as per internal transitions so make sure if up
	scm_main_sys_cmd_res(NULL, 0, "ifconfig wlan%d up", radio_idx);
	if (nl_send_auto_complete(sock, msg) < 0)
		return -1;

	while (*cb_err == SCM_NL_80211_CMD_RUN)
		nl_recvmsgs(sock, cb);

	if (*cb_err == SCM_NL_80211_CMD_SUCCESS)
		ret = 0;
	else
		ret = *cb_err;

	return ret;
}

static void scm_nl_80211_cmd_deinit(struct nl_msg *msg, struct nl_cb *cb,
							 int *cb_err)
{
	if (cb)
		nl_cb_put(cb);
	if (msg)
		nlmsg_free(msg);
	if (cb_err)
		free(cb_err);
}

int scm_nl_80211_family_handler(struct nl_msg *msg, void *arg)
{

	struct scm_nl_80211_handler_args *grp =
				 (struct scm_nl_80211_handler_args *)arg;
	struct nlattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *gen_nlh =
				(struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *mcast_grp;
	int rem_mcast_grp;

	nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
	genlmsg_attrlen(gen_nlh, 0), NULL);

	if (!tb[CTRL_ATTR_MCAST_GROUPS])
		return NL_SKIP;

	for_each_nl_attr(mcast_grp, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcast_grp) {
		struct nlattr *tb_mcast_grp[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(tb_mcast_grp, CTRL_ATTR_MCAST_GRP_MAX,
					(struct nlattr *)nla_data(mcast_grp),
						nla_len(mcast_grp), NULL);

		if (!tb_mcast_grp[CTRL_ATTR_MCAST_GRP_NAME] ||
					!tb_mcast_grp[CTRL_ATTR_MCAST_GRP_ID])
			continue;

		if (strncmp((const char *)nla_data(
			tb_mcast_grp[CTRL_ATTR_MCAST_GRP_NAME]), grp->group,
			nla_len(tb_mcast_grp[CTRL_ATTR_MCAST_GRP_NAME])))
			continue;

		grp->num = nla_get_u32(tb_mcast_grp[CTRL_ATTR_MCAST_GRP_ID]);
			break;
	}
	return NL_SKIP;
}

int scm_nl_80211_get_multicast_id(struct nl_sock *sock, const char *family,
				const char *group)
{
	struct nl_cb *cb;
	struct nl_msg *msg;
	int ret = -1, *cb_err, ctrl_id;
	struct scm_nl_80211_handler_args grp = {group, -ENOENT};

	if (scm_nl_80211_cmd_init(&msg, &cb, scm_nl_80211_family_handler,
				 &grp, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}
	ctrl_id = genl_ctrl_resolve(sock, "nlctrl");
	genlmsg_put(msg, 0, 0, ctrl_id, 0, 0, CTRL_CMD_GETFAMILY, 0);
	NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);
	ret = scm_nl_80211_cmd_run(sock, msg, cb, cb_err, 0);
	if (ret == 0)
		ret = grp.num;
nla_put_failure:
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

int scm_nl_80211_deinit(struct scm_data *sd, int fd, void *arg)
{
	struct scm_nl_80211_data *nl_80211 = sd->nl_80211;

	scm_logd("Called");
	if (nl_80211->evt_sock)
		nl_socket_free(nl_80211->evt_sock);

	if (nl_80211->cmd_sock)
		nl_socket_free(nl_80211->cmd_sock);

	if (nl_80211->cache)
		nl_cache_free(nl_80211->cache);

	if (nl_80211->id && nl_80211->family_ptr)
		genl_family_put(nl_80211->family_ptr);

	nl_cb_put(nl_80211->cmd_cb);
	nl_cb_put(nl_80211->evt_cb);
	free(nl_80211);
	return 0;
}

int scm_nl_80211_add_mcast_attach(struct scm_data *sd)
{

	int ret = -1, mcast_id;
	struct scm_nl_80211_data *nl_80211 = sd->nl_80211;

	scm_logd("Setup NL80211 Event Monitor");

	nl_80211->evt_cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!nl_80211->evt_cb) {
		scm_loge("nl_cb_alloc() fail");
		return -ENOMEM;
	}

	nl_80211->evt_sock = nl_socket_alloc_cb(nl_80211->evt_cb);
	if (!nl_80211->evt_sock) {
		scm_loge("nl_socket_alloc_cb() fail");
		nl_cb_put( nl_80211->evt_cb);
		return -ENOMEM;
	}

	if (genl_connect(nl_80211->evt_sock)) {
		scm_loge("genl_connect() fail");
		ret = -ENOLINK;
		nl_cb_put( nl_80211->evt_cb);
		nl_socket_free(nl_80211->evt_sock);
		return ret;
	}

	mcast_id = scm_nl_80211_get_multicast_id(nl_80211->evt_sock,
						"nl80211", "vendor");
	if(mcast_id >= 0) {
		ret = nl_socket_add_membership(nl_80211->evt_sock, mcast_id);
	}

	nl_cb_set(nl_80211->evt_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM,
					scm_nl_80211_seq_check, NULL);
	nl_cb_set(nl_80211->evt_cb, NL_CB_VALID, NL_CB_CUSTOM,
					scm_nl_80211_evt_handler, sd);

	if (scm_main_evt_list_add(nl_socket_get_fd(nl_80211->evt_sock),
			 scm_nl_80211_evt_fd_handler, scm_nl_80211_deinit,
			 NULL, "NL80211", &nl_80211->evt))
		goto err;

	return ret;
err:
	nl_cb_put(nl_80211->evt_cb);
	nl_socket_free(nl_80211->evt_sock);
	return -1;
}

int scm_nl_80211_init(struct scm_data *sd)
{
	int ret = 0;
	struct scm_nl_80211_data *nl_80211;
	if (!sd)
		return -1;


	sd->nl_80211 = (struct scm_nl_80211_data *)
			 malloc(sizeof(struct scm_nl_80211_data));
	if (!sd->nl_80211)
		return -1;

	nl_80211 = sd->nl_80211;
	nl_80211->cmd_cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!nl_80211->cmd_cb) {
		scm_loge("nl_cb_alloc() fail");
		goto fail;
	}

	nl_80211->cmd_sock = nl_socket_alloc_cb(nl_80211->cmd_cb);
	if (!nl_80211->cmd_sock) {
		scm_loge("nl_socket_alloc_cb() fail");
		goto fail;
	}

	if (genl_connect(nl_80211->cmd_sock)) {
		scm_loge("genl_connect() fail");
		ret = -ENOLINK;
		goto fail;
	}

	if (genl_ctrl_alloc_cache(nl_80211->cmd_sock, &(nl_80211->cache))) {
		scm_loge("genl_ctrl_alloc_cache() fail");
		goto fail;
	}

	nl_80211->family_ptr =
			genl_ctrl_search_by_name(nl_80211->cache,"nl80211");
	if(!nl_80211->family_ptr) {
		scm_loge("genl_ctrl_search_by_name() fail");
		ret= -ENOENT;
		goto fail;
	}

	nl_80211->id = genl_ctrl_resolve(nl_80211->cmd_sock, "nl80211");
	if (nl_80211->id < 0) {
		scm_loge("genl_ctrl_resolve() fail");
		ret = -ENOENT;
		goto fail;
	}

	// add multicast membership for vendor evts
	ret = scm_nl_80211_add_mcast_attach(sd);
	if(ret < 0) {
		scm_loge("Vendor Event Monitor fail");
		goto fail;
	}
	scm_logd("Success");
	return 0;
fail:
	if (nl_80211->cmd_sock)
		nl_socket_free(nl_80211->cmd_sock);
	nl_cb_put(nl_80211->cmd_cb);
	if (nl_80211->cache)
		nl_cache_free(nl_80211->cache);
	if (nl_80211->id && nl_80211->family_ptr)
		genl_family_put(nl_80211->family_ptr);
	if (nl_80211)
		free(nl_80211);

	return ret;
}

static int scm_nl_80211_get_cnss_driver_idx_handler(struct nl_msg *msg,
							void *cb_data)
{
	uint8_t *cnss_driver_idx = cb_data;
	struct genlmsghdr *gen_nlh = (struct genlmsghdr *)
						nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *attr_list[NL80211_ATTR_MAX + 1], *nl_vendor_data_attr;
	// We are interested only in RADIO_INDEX
	struct nlattr *nl_attr_list[QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX + 1];

	nla_parse(attr_list, NL80211_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
	      genlmsg_attrlen(gen_nlh, 0), NULL);
	if (gen_nlh->cmd != NL80211_CMD_VENDOR &&
				!attr_list[NL80211_ATTR_VENDOR_DATA]) {
		scm_loge("Invalid Vendor Data or sub command ");
		return 0;
	}
	nl_vendor_data_attr = attr_list[NL80211_ATTR_VENDOR_DATA];
	nla_parse(nl_attr_list, QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX,
				(struct nlattr *) nla_data(nl_vendor_data_attr),
				nla_len(nl_vendor_data_attr), NULL);

	*cnss_driver_idx = SCM_RADIO_SUP_MAX + 1; //Invalid index
	if (!nl_attr_list[QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX])
		scm_loge("No Radio index\n");
	else
		*cnss_driver_idx = nla_get_u32(nl_attr_list[
				QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX]);
	return NL_SKIP;
}

int scm_nl_80211_get_cnss_driver_idx(struct scm_data *sd,
				struct wlan_radio_data *radio,
				uint8_t *cnss_driver_idx)
{
        struct nl_msg *msg;
	struct nl_cb *cb = NULL;
        struct nlattr *data;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	if (!sd || !radio)
		return -1;

	nl_80211 = sd->nl_80211;

	if (scm_nl_80211_cmd_init(&msg, &cb,
				scm_nl_80211_get_cnss_driver_idx_handler,
			 	cnss_driver_idx, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO);
        if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA)) ||
            nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_WIFI_INFO_RADIO_INDEX, 0)) {
                ret = -ENOBUFS;
		goto err;
        }
        nla_nest_end(msg, data);
	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
	scm_logd("CNSS driver Index: %d", *cnss_driver_idx);
err:
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
        return ret;
}

int scm_nl_80211_set_band(struct scm_data *sd, struct wlan_radio_data *radio,
			enum WLAN_BAND band)
{
        struct nl_msg *msg;
	struct nl_cb *cb = NULL;
        struct nlattr *data;
        enum qca_set_band qca_band;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	if (!sd || !radio)
		return -1;

	nl_80211 = sd->nl_80211;

	if (scm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}
        switch (band) {
        case WLAN_BAND_5G:
                qca_band = QCA_SETBAND_5G;
                break;
        case WLAN_BAND_2G:
                qca_band = QCA_SETBAND_2G;
                break;
        case WLAN_BAND_ANY:
        default:
                qca_band = QCA_SETBAND_AUTO;
                break;
        }
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_SETBAND);
        if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA)) ||
            nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_SETBAND_VALUE, qca_band)) {
                ret = -ENOBUFS;
		goto err;
        }
        nla_nest_end(msg, data);
	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
	if (ret == 0) {
		radio->band_restrict = band;
	}
err:
	scm_logi("%d on Radio %d: %s", band, radio->idx,
		ret == 0 ? "Success" : "Fail");
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
        return ret;
}

int scm_nl_80211_get_dfs_range_handler(struct nl_msg *msg, void *cb_data)
{
	struct wlan_freq_range_arr *dfs_range_arr = cb_data;
	struct nlattr *nl_attr_list[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gen_nlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *nl_attr;
	int nl_attr_last, cnt = 0;
	char *cc;
	uint32_t start_freq, end_freq, reg_flags;
	static struct nla_policy nl_80211_reg_policy[NL80211_REG_RULE_ATTR_MAX + 1] = {
		[NL80211_ATTR_REG_RULE_FLAGS]           = { .type = NLA_U32 },
		[NL80211_ATTR_FREQ_RANGE_START]         = { .type = NLA_U32 },
		[NL80211_ATTR_FREQ_RANGE_END]           = { .type = NLA_U32 },
		[NL80211_ATTR_FREQ_RANGE_MAX_BW]        = { .type = NLA_U32 },
		[NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN]  = { .type = NLA_U32 },
		[NL80211_ATTR_POWER_RULE_MAX_EIRP]      = { .type = NLA_U32 },
		[NL80211_ATTR_DFS_CAC_TIME]             = { .type = NLA_U32 },
	};

	if (!dfs_range_arr) {
		scm_loge("Invalid data ptr");
		return NL_SKIP;
	}

	nla_parse(nl_attr_list, NL80211_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
					genlmsg_attrlen(gen_nlh, 0), NULL);

	if (!nl_attr_list[NL80211_ATTR_REG_RULES]) {
		scm_loge("No reg rules\n");
		goto end;
	}

	if (!nl_attr_list[NL80211_ATTR_REG_ALPHA2]) {
		scm_loge("No alpha2\n");
		goto end;
	} else {
		cc = nla_data(nl_attr_list[NL80211_ATTR_REG_ALPHA2]);
	}
	scm_logd("Country Code: %c%c", cc[0], cc[1]);

	for_each_nl_attr(nl_attr, nl_attr_list[NL80211_ATTR_REG_RULES],
								 nl_attr_last) {
		struct nlattr *reg_attr_list[NL80211_REG_RULE_ATTR_MAX + 1];

		nla_parse(reg_attr_list, NL80211_REG_RULE_ATTR_MAX,
				 nla_data(nl_attr), nla_len(nl_attr),
				 nl_80211_reg_policy);

		reg_flags = nla_get_u32(
				reg_attr_list[NL80211_ATTR_REG_RULE_FLAGS]);
		start_freq = nla_get_u32(
				reg_attr_list[NL80211_ATTR_FREQ_RANGE_START]);
		end_freq = nla_get_u32(
				reg_attr_list[NL80211_ATTR_FREQ_RANGE_END]);

		if (reg_flags & NL80211_RRF_DFS) {
			if (cnt >= dfs_range_arr->len)
				goto end;
			dfs_range_arr->freq_range[cnt].start_freq = start_freq / 1000;
			dfs_range_arr->freq_range[cnt].end_freq = end_freq / 1000;
			scm_logd("DFS %d - %d",
				dfs_range_arr->freq_range[cnt].start_freq,
				dfs_range_arr->freq_range[cnt].end_freq);
			cnt++;
		}
	}
end:
	dfs_range_arr->len = cnt;
	return NL_SKIP;
}

int scm_nl_80211_get_dfs_range(struct scm_data *sd,
				struct wlan_radio_data *radio,
				struct wlan_freq_range_arr *dfs_range_arr)
{
        struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];
	int ret, *cb_err;

	if (!sd)
		return -1;
	nl_80211 = sd->nl_80211;

	if (scm_nl_80211_cmd_init(&msg, &cb, scm_nl_80211_get_dfs_range_handler,
					dfs_range_arr, &cb_err)) {
		scm_loge("msg alloc fail");
		return -1;
	}
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_GET_REG, 0);
	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));

	ret =scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
	if (ret < 0) {
		scm_loge("Fail");
	}

	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

int scm_nl_80211_set_sap_ch(struct scm_data *sd,
				struct wlan_if_data *sap_wif,
				int ch)
{
        struct nl_msg *msg;
	struct nl_cb *cb = NULL;
        struct nlattr *data;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	scm_logd("ENTER");

	if (!sd) {
		scm_loge("invalid pointer sd: %p", sd);
		return -EINVAL;
	}

	if (!sap_wif) {
		scm_loge("invalid pointer sta_wif ");
		return -EINVAL;
	}

	nl_80211 = sd->nl_80211;
	if (scm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}

	scm_logd("if idx %d, chan %d", sap_wif->num, ch);
	snprintf(if_name, IFNAMSIZ, "wlan%d", sap_wif->num);
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_SET_SAP_CONFIG);

	if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA))) {
		ret = -ENOBUFS;
		scm_loge("fail: QCA_NL80211_VENDOR_SUBCMD_SET_SAP_CONFIG");
		goto err;
	}

	nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_SAP_CONFIG_CHAN, ch);
	nla_nest_end(msg, data);

	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							sap_wif->num);
	scm_logi("Force SCC Ch %d on %s wlan%d: %s", ch,
		scm_wlan_if_type_str(sap_wif->type), sap_wif->num,
		ret == 0 ? "Success" : "Fail");
err:
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
        return ret;
}

int scm_nl_80211_set_sta_roam_policy(struct scm_data *sd,
					struct wlan_radio_data *radio,
					int dfs_setup, int unsafe_ch_skip)
{
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	struct nlattr *data;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	scm_logd("Enter");

	if (!sd) {
		scm_loge("invalid pointer sd: %p", sd);
		return -EINVAL;
	}

	if (!radio) {
		scm_loge("invalid pointer radio data");
		return -EINVAL;
	}

	nl_80211 = sd->nl_80211;
	if (scm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}

	scm_logd("Radio %d, dfs_setup %d, unsafe_ch_skip %d",
			radio->idx, dfs_setup, unsafe_ch_skip);

	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_STA_CONNECT_ROAM_POLICY);

	if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA))) {
		ret = -ENOBUFS;
		goto err;
	}

	nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_STA_DFS_MODE, dfs_setup);
	nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_STA_SKIP_UNSAFE_CHAN, unsafe_ch_skip);
	nla_nest_end(msg, data);

	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
err:
	scm_logd("ret %d", ret);
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
        return ret;
}

int scm_nl_80211_set_sap_acs_policy(struct scm_data *sd,
				struct wlan_radio_data *radio,
				int dfs_setup, int acs_ch_hint)
{
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	struct nlattr *data;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	scm_logd("Enter");

	if (!sd) {
		scm_loge("invalid pointer sd: %p", sd);
		return -EINVAL;
	}

	if (!radio) {
		scm_loge("invalid pointer to radio data");
		return -EINVAL;
	}

	nl_80211 = sd->nl_80211;
	if (scm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		scm_loge("cmd msg alloc fail");
		return -1;
	}

	scm_logd("if idx %d, dfs_setup %d, acs_ch_hint %d",
			radio->pri_if_num, dfs_setup, acs_ch_hint);

	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_ACS_POLICY);

	if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA))) {
		ret = -ENOBUFS;
		scm_loge("fail: QCA_WLAN_VENDOR_ATTR_ACS_DFS_MODE");
		goto err;
	}

	nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_ACS_DFS_MODE, dfs_setup);
	nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_ACS_CHANNEL_HINT, acs_ch_hint);
	nla_nest_end(msg, data);

	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
	radio->sap_dfs_setup = dfs_setup;
err:
	scm_logd("ret %d", ret);
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

int scm_nl_80211_avoid_freq(struct scm_data *sd, struct wlan_radio_data *radio)
{
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	int ret = 0, *cb_err;
	struct scm_nl_80211_data *nl_80211;
	char if_name[IFNAMSIZ];

	scm_logd("Enter");

	if (!sd) {
		scm_loge("Invalid pointer sd: %p", sd);
		return -EINVAL;
	}

	if (!radio) {
		scm_loge("Invalid pointer radio");
		return -EINVAL;
	}

	nl_80211 = sd->nl_80211;
	if (scm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		scm_loge("Cmd msg alloc fail");
		return -1;
	}

	snprintf(if_name, IFNAMSIZ, "wlan%d", radio->pri_if_num);
	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(if_name));
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, QCA_OUI);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY);

	ret = scm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err,
							 radio->idx);
	scm_logd("ret %d", ret);
	scm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

/******************************************************************************/
                               /* NL_SVC Handling */
/******************************************************************************/

static inline int scm_wake_lock(void)
{
	return system("echo wlan_services > /sys/power/wake_lock");
}

static inline int scm_wake_unlock(void)
{
	return system("echo wlan_services > /sys/power/wake_unlock");
}

static void scm_nl_svc_lte_coex_handler(struct scm_data *sd, int cnss_driver_idx)
{
	int i, sta_dfs_cfg = DFS_ENABLE, sap_dfs_cfg = DFS_ENABLE,
					recfg_ch = SCM_SAP_ACS_HINT_RESET;

	scm_logd("CNSS driver Index: %d", cnss_driver_idx);
	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (sd->wif[i] && sd->wif[i]->radio->cnss_driver_idx ==
							 cnss_driver_idx) {
			// STA no longer allowed to operate in unsafe channels due to SCC
			// Assumption to reconnect STA is STA-AP is always in SCC
			if (WLAN_IS_STA(sd->wif[i]->type)) {
				scm_main_sys_cmd_res(NULL, 0,
						"wpa_cli -i wlan%d disconnect",
						sd->wif[i]->num);
				scm_nl_80211_set_sta_roam_policy(sd,
						sd->wif[i]->radio, sta_dfs_cfg,
						SCM_STA_UNSAFE_CH_SKIP);
				scm_main_sys_cmd_res(NULL, 0,
						"wpa_cli -i wlan%d reconnect",
						sd->wif[i]->num);
			} else {
				//If multi STA AP mode
				if (sd->wif[i]->band == WLAN_BAND_5G &&
							sd->wif[WLAN_STA_1]) {
					sta_dfs_cfg = DFS_DISABLE;
				}
				scm_wlan_dynamic_if_ctrl(sd, sd->wif[i],
					WLAN_IF_STOP, sd->wif[i]->radio->idx);
				usleep(500000); // Sleep hostapd to clear
			}
		}
	}

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (sd->wif[i] && sd->wif[i]->radio->cnss_driver_idx ==
							 cnss_driver_idx) {
			if (!WLAN_IS_STA(sd->wif[i]->type)) {
				//If multi STA AP mode
				if (sd->wif[i]->band == WLAN_BAND_5G &&
							sd->wif[WLAN_STA_1]) {
					sap_dfs_cfg = DFS_DEPRIORITY;
				}

				scm_nl_80211_set_sap_acs_policy(sd,
						sd->wif[i]->radio,
						sap_dfs_cfg, recfg_ch);

				scm_wlan_dynamic_if_ctrl(sd, sd->wif[i],
					WLAN_IF_START, sd->wif[i]->radio->idx);
				//For next SAP on same radio force SCC.
				recfg_ch = sd->wif[i]->act_ch;
			}
		}
	}
}

static void scm_nl_svc_dp_msg_handler(struct scm_data *sd,
						unsigned short type, void *data)
{
	scm_wlan_process_dp_msg(type, data);
}

static void scm_nl_svc_auto_shutdown_handler(struct scm_data *sd,
						 int cnss_driver_idx, int ind)
{
	scm_logd("CNSS driver Index: %d", cnss_driver_idx);
	if (ind == WLAN_SVC_WLAN_AUTO_SHUTDOWN_CANCEL_IND) {
		svc_wlan_active |= (1 << cnss_driver_idx);
		return;
	}

	svc_wlan_active &= ~(1 << cnss_driver_idx);
	if (!svc_wlan_active) {
		scm_main_cli_indication(sd, NULL, WLAN_SYS_CTRL_IND,
					 WLAN_DISABLE);
		scm_wlan_disable(sd);
	}
}

void scm_nl_svc_reset_auto_shutdown(int cnss_driver_idx)
{
        svc_wlan_active &= ~(1 << cnss_driver_idx);
}

static void scm_nl_svc_fw_shutdown_handler(struct scm_data *sd, int cnss_driver_idx)
{
	int i;
	extern struct wlan_radio_data wlan_radio_data[SCM_RADIO_SUP_MAX];

	scm_logd("CNSS driver Index: %d", cnss_driver_idx);

	if (cnss_driver_idx == wlan_radio_data[TUF_DEV].cnss_driver_idx) {
		scm_logd("No action for TUF FW shutdown event");
		return;
	} else {
		scm_logd("Stop TUF Dev due to ROME FW shutdown event");
		for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
			if (!sd->wif[i])
				continue;
			if (sd->wif[i]->radio->idx == TUF_DEV) {
				//STOP_CMD here to avoid duplicate event to QCMAP
				scm_wlan_dynamic_if_ctrl(sd, sd->wif[i],
					WLAN_IF_STOP_CMD, sd->wif[i]->radio->idx);
			}
		}

		if (wlan_radio_data[TUF_DEV].drv_loaded) {
			scm_wlan_unload_module(sd, TUF_DEV);
			wlan_radio_data[TUF_DEV].ssr_drv_restart = true;
		}
		else {
			wlan_radio_data[TUF_DEV].ssr_drv_restart = false;
		}
	}
}

static bool scm_nl_restart_driver_unload(struct scm_data *sd, int radio_idx)
{
	int i;
	bool radio_used = false;

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (!sd->wif[i])
			continue;
		if (sd->wif[i]->radio->idx == radio_idx) {
			radio_used = true;
			scm_wlan_dynamic_if_ctrl(sd, sd->wif[i],
					WLAN_IF_STOP, sd->wif[i]->radio->idx);
		}
	}

	if (radio_used == true)
		scm_wlan_unload_module(sd, radio_idx);

	return radio_used;
}

static void scm_nl_restart_driver_load(struct scm_data *sd, int radio_idx)
{
	int i;

	if (scm_wlan_load_module(sd, radio_idx)) {
		scm_loge("%s: Driver reload fail", radio_idx == ROME_DEV ? "Rome" : "Tuf");
		return;
	}

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (!sd->wif[i])
			continue;
		if (sd->wif[i]->radio->idx == radio_idx) {
			scm_wlan_dynamic_if_ctrl(sd, sd->wif[i],
					WLAN_IF_START, sd->wif[i]->radio->idx);
		}
	}
}

static void scm_nl_svc_fw_crash_handler(struct scm_data *sd, int cnss_driver_idx)
{
	bool tuf_drv_load_needed = true;
	extern struct wlan_radio_data wlan_radio_data[SCM_RADIO_SUP_MAX];

	scm_logd("CNSS driver Index: %d", cnss_driver_idx);
	if (wlan_radio_data[ROME_DEV].cnss_driver_idx == cnss_driver_idx) {
		tuf_drv_load_needed = scm_nl_restart_driver_unload(sd, TUF_DEV);
		if(scm_nl_restart_driver_unload(sd, ROME_DEV))
			scm_nl_restart_driver_load(sd, ROME_DEV);

		scm_logd("ROME SSR, Restart TUF if in use..");
		if (wlan_radio_data[TUF_DEV].ssr_drv_restart
			&& tuf_drv_load_needed)
			scm_nl_restart_driver_load(sd, TUF_DEV);
	} else {
		if(scm_nl_restart_driver_unload(sd, TUF_DEV))
			scm_nl_restart_driver_load(sd, TUF_DEV);
	}
}

void scm_nl_svc_evt_process(struct scm_data *sd, struct nlmsghdr *nlh)
{
	struct sAniHdr *ani_hdr;
	ani_hdr = (struct sAniHdr *) NLMSG_DATA(nlh);
	int radio = 0;
	int msg_len;
	struct radio_index_tlv *radio_info;
	void *p_data = NULL;

	scm_logi("\n type %d, len %d", ani_hdr->type, ani_hdr->length);

	msg_len =  sizeof (*ani_hdr) + ani_hdr->length;

	/*
	 * The radio index is added at the end of the nl event in tlv format.
	 * -------------------------------------------------------------------------------------------------------------
	 * |nlmsghdr|wlan svc event type|wlan svc event len|svc event body|radio index type|radio index len|radio index|
	 * -------------------------------------------------------------------------------------------------------------
	 */
	if ((msg_len + sizeof(*radio_info)) <= nlh->nlmsg_len) {
		radio_info = (struct radio_index_tlv *) (((char *) ani_hdr) +
				msg_len);
		scm_logi("\n radio_info->type  %x", radio_info->type);
		if (radio_info->type == WLAN_SVC_WLAN_RADIO_INDEX) {
			scm_logi("\n radio_info->radio  %d",
					radio_info->radio);
			radio = radio_info->radio;
		}
	}

	scm_wake_lock();

	switch (ani_hdr->type) {
		case WLAN_SVC_WLAN_AUTO_SHUTDOWN_IND:
		case WLAN_SVC_WLAN_AUTO_SHUTDOWN_CANCEL_IND:
			scm_nl_svc_auto_shutdown_handler(sd,
					radio,
					ani_hdr->type);
			break;
		case WLAN_SVC_FW_SHUTDOWN_IND:
			scm_nl_svc_fw_shutdown_handler(sd, radio);
			break;
		case WLAN_SVC_FW_CRASHED_IND:
			scm_nl_svc_fw_crash_handler(sd, radio);
			break;
		case WLAN_SVC_LTE_COEX_IND:
			scm_nl_svc_lte_coex_handler(sd, radio);
			break;
		case WLAN_MSG_WLAN_TP_IND:
		case WLAN_MSG_WLAN_TP_TX_IND:
			p_data = (char *)ani_hdr + sizeof(struct sAniHdr);
			scm_nl_svc_dp_msg_handler(sd, ani_hdr->type, p_data);
			break;
		case WLAN_SVC_DFS_CAC_START_IND:
		case WLAN_SVC_DFS_CAC_END_IND:
		case WLAN_SVC_DFS_RADAR_DETECT_IND:
		default:
			break;
	}
	scm_wake_unlock();
}


int scm_nl_svc_evt_handler(struct scm_data *sd, int evt_fd,  void *arg)
{
	struct sockaddr_nl dest_addr;
	struct iovec iov;
	struct msghdr msg;
	struct nlmsghdr *nlh;
	int ret = 0, nlmsg_len_orig;
	struct scm_nl_svc_data *nl_svc = sd->nl_svc;

	nlh = nl_svc->msg;
	nlmsg_len_orig = nl_svc->msg->nlmsg_len;

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* Kernel */
	dest_addr.nl_groups = WLAN_NLINK_MCAST_GRP_ID;

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	ret = recvmsg(evt_fd, &msg, 0);

	if (ret < 0) {
		scm_sys_call_err("recvmsg");
		return -1;
	}

	while(NLMSG_OK(nlh, ret)) {
		if (nlh->nlmsg_type == WLAN_NL_MSG_SVC) {
			scm_nl_svc_evt_process(sd, nlh);
			break;
		}
		nlh = NLMSG_NEXT(nlh, ret);
	}
	nl_svc->msg->nlmsg_len = nlmsg_len_orig;
	return 0;
}

int scm_nl_svc_deinit(struct scm_data *sd, int evt_fd,  void *arg)
{
	struct scm_nl_svc_data *nl_svc = sd->nl_svc;

	scm_logd("Called");
	if (nl_svc->msg)
		free(nl_svc->msg);
	if (evt_fd)
		close(evt_fd);
	free(nl_svc);
	return 0;
}

int scm_nl_svc_init(struct scm_data *sd)
{
	struct sockaddr_nl sa;
	int ret = 0, sock_fd = 0;
	struct scm_nl_svc_data *nl_svc;

	if (!sd)
		return -1;

	sd->nl_svc = (struct scm_nl_svc_data *)
			 malloc(sizeof(struct scm_nl_svc_data));
	if (!sd->nl_svc)
		return -1;

	nl_svc = sd->nl_svc;
	nl_svc->msg = (struct nlmsghdr *)
			 malloc(NLMSG_SPACE(WLAN_NL_MAX_PAYLOAD));
	if (nl_svc->msg == NULL) {
		scm_sys_call_err("malloc");
		goto err;
	}
	memset(nl_svc->msg, 0, NLMSG_SPACE(WLAN_NL_MAX_PAYLOAD));
	nl_svc->msg->nlmsg_len = NLMSG_SPACE(WLAN_NL_MAX_PAYLOAD);
	nl_svc->msg->nlmsg_pid = getpid();

	sock_fd = socket(AF_NETLINK, SOCK_RAW, WLAN_NLINK_PROTO_FAMILY);
	if (sock_fd < 0) {
		scm_sys_call_err("socket");
		goto err;
	}

	if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0) {
		scm_sys_call_err("fcntl()");
		goto err;
	}

	memset (&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = WLAN_NLINK_MCAST_GRP_ID;
	sa.nl_pid = getpid();

	ret = bind(sock_fd, (struct sockaddr*)&sa, sizeof(sa));
	if (ret < 0) {
		scm_sys_call_err("bind");
		goto err;
	}

	if (scm_main_evt_list_add(sock_fd, scm_nl_svc_evt_handler,
			 scm_nl_svc_deinit, NULL, "NL_SVC", &nl_svc->evt))
		goto err;
	scm_logd("Success");
	return 0;

err:
	if (nl_svc->msg)
		free(nl_svc->msg);

	if (sock_fd > 0)
		close(sock_fd);
	if (nl_svc)
		free(nl_svc);

	return ret;
}


