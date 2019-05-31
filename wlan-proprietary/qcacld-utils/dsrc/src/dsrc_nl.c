/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * A segment of this code file includes code from the WPA Supplicant
 *
 * WPA Supplicant - driver interaction with Linux nl80211/cfg80211
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * Qualcomm Atheros, Inc. has chosen to take the WPA Supplicant code subject
 * to the BSD license and terms only.
 */

/**
 * @file dsrc_nl.c
 * @brief nl80211 layer for the test app.
 *
 * Implementation of the nl80211 layer to the test app. This acts as a wrapper
 * to any nl80211 related functions. Other code should only be aware of the
 * exposed APIs.
 */

#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/socket.h>
#include <netlink/attr.h>
#include <inttypes.h>
#include "nl80211_copy.h"
#include "dsrc_nl.h"
#include "dsrc_util.h"

unsigned int if_nametoindex(const char *ifname);

static int state_init(struct nl80211_state *state)
{
    /* Allocate the default callback */
    state->nl_cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (state->nl_cb == NULL) {
        printf("nl80211: Failed to allocate netlink callbacks\n");
        goto err;
    }

    state->nl = nl_socket_alloc();
    if (state->nl == NULL) {
        printf("nl80211: Failed to allocate socket\n");
        goto err;
    }

    /* Connect to the generic netlink socket */
    if (genl_connect(state->nl)) {
        printf("nl80211: Failed to connect to generic netlink socket\n");
        goto err;
    }

    state->nl80211_id = genl_ctrl_resolve(state->nl, "nl80211");
    if (state->nl80211_id < 0) {
        printf("nl80211: 'nl80211' generic netlink not found\n");
        goto err;
    }

    return 0;

err:
    if (state->nl)
        nl_socket_free(state->nl);
    nl_cb_put(state->nl_cb);
    state->nl_cb = NULL;
    return -1;
}

static void state_deinit(struct nl80211_state *state)
{
    if (state->nl) {
        nl_socket_free(state->nl);
        state->nl = NULL;
    }
    nl_cb_put(state->nl_cb);
    state->nl_cb = NULL;
}

/* Handle an ack sent from the host driver */
static int ack_handler(struct nl_msg *msg __attribute__((unused)),
	void *arg)
{
    int *err = arg;
    *err = 0;
    return NL_STOP;
}

/* Handle a finish response from the host driver */
static int finish_handler(struct nl_msg *msg __attribute__((unused)),
	void *arg)
{
    int *ret = arg;
    *ret = 0;
    return NL_SKIP;
}

/* Handle an error sent from the host driver */
static int error_handler(struct sockaddr_nl *nla __attribute__((unused)),
	struct nlmsgerr *err, void *arg)
{
    int *ret = arg;
    *ret = err->error;
    return NL_SKIP;
}

static int family_handler(struct nl_msg *msg, void *arg)
{
    struct family_data *res = arg;
    struct nlattr *tb[CTRL_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *mcgrp;
    int i;

    nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh, 0), NULL);
    if (!tb[CTRL_ATTR_MCAST_GROUPS])
        return NL_SKIP;

    nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], i) {
        struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];
        nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX, nla_data(mcgrp),
              nla_len(mcgrp), NULL);
        if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] ||
                !tb2[CTRL_ATTR_MCAST_GRP_ID] ||
            strncmp(nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]),
                   res->group,
                   nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME])) != 0)
            continue;
        res->id = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
        break;
    };

    return NL_SKIP;
}

static struct nla_policy qca_wlan_vendor_ocb_get_tsf_timer_resp[] = {
    [QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_HIGH] =
        { .type = NLA_U32 },
    [QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_LOW] =
        { .type = NLA_U32 },
};

/* Handle TSF timer response */
static int tsf_timer_handler(struct nl_msg *msg,
	void *arg __attribute__((unused)))
{
    struct nlattr *data[NL80211_ATTR_MAX + 1];
    struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_MAX + 1];
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct tsf_timer tsf;
    uint64_t tsf_value;

    if (nla_parse(data, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
            genlmsg_attrlen(gnlh, 0), NULL)) {
        printf("nla_parse failed for response\n");
        return -1;
    }

    if (!data[NL80211_ATTR_VENDOR_DATA]) {
        printf("There is no vendor data in the response\n");
        return -1;
    }

    if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_MAX,
            data[NL80211_ATTR_VENDOR_DATA],
            qca_wlan_vendor_ocb_get_tsf_timer_resp)) {
        printf("nla_parse failed for vendor_data\n");
        return -1;
    }

    if (!tb[QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_HIGH] ||
            !tb[QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_LOW]) {
        printf("TSF response fields are not present\n");
        return -1;
    }

    tsf.timer_high =
        nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_HIGH]);
    tsf.timer_low =
        nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_LOW]);

    tsf_value = tsf.timer_high;
    tsf_value = tsf_value << 32;
    tsf_value |= tsf.timer_low;

    printf("TSF timer value = %" PRIu64 "\n", tsf_value);

    return NL_SKIP;
}

/* Parse and display the DCC stats */
static int parse_dcc_stats_data(struct nlattr **data)
{
    struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_MAX + 1];
    uint32_t channel_count;
    dcc_ndl_stats_per_channel *stats;

    if (nla_parse_nested(tb, QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_MAX,
                         data[NL80211_ATTR_VENDOR_DATA],
                         NULL)) {
        printf("nla_parse failed for vendor_data\n");
        return -1;
    }

    if (!tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_CHANNEL_COUNT] ||
            !tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_STATS_ARRAY]) {
        printf("Stats fields are not present\n");
        return -1;
    }

    channel_count =
        nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_CHANNEL_COUNT]);
    stats = malloc(channel_count * sizeof(dcc_ndl_stats_per_channel));
    memcpy(stats,
        nla_data(tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_REQUEST_ARRAY]),
        nla_len(tb[QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_REQUEST_ARRAY]));

    /* Print the stats */
    printf("\n");
    dcc_print_stats(channel_count, stats);
    free(stats);

    return 0;
}

/* Handler for get_stats command */
static int dcc_stats_handler(struct nl_msg *msg,
	void *arg __attribute__((unused)))
{
    struct nlattr *data[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));

    if (nla_parse(data, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
            genlmsg_attrlen(gnlh, 0), NULL)) {
        printf("nla_parse failed for response\n");
        return -1;
    }

    if (!data[NL80211_ATTR_VENDOR_DATA]) {
        printf("There is no vendor data in the response\n");
        return -1;
    }

    if (parse_dcc_stats_data(data)) {
        printf("Failed to read the dcc stats\n");
    }

    return NL_SKIP;
}

/* Handler for the periodical stats event messages */
static int dcc_stats_event_handler(struct nl_msg *msg,
	void *arg __attribute__((unused)))
{
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    uint32_t vendor_id, subcmd;

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
        genlmsg_attrlen(gnlh, 0), NULL);

    if (gnlh->cmd == NL80211_CMD_VENDOR) {
        if (!tb[NL80211_ATTR_VENDOR_ID] ||
                !tb[NL80211_ATTR_VENDOR_SUBCMD]) {
            printf("No vendor ID or subcmd\n");
            return NL_SKIP;
        }

        vendor_id = nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]);
        subcmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);

        if (vendor_id == QCA_NL80211_VENDOR_ID &&
                subcmd == QCA_NL80211_VENDOR_SUBCMD_DCC_STATS_EVENT) {
            if (parse_dcc_stats_data(tb)) {
                printf("Failed to read the dcc stats\n");
            }
        } else {
            printf("Skipping non-QCA or non-stats event\n");
        }
    }

    return NL_SKIP;
}

static int send_and_recv_msgs(struct nl80211_state *state,
             struct nl_msg *msg,
             int (*valid_handler)(struct nl_msg *, void *),
             void *valid_data)
{
    struct nl_cb *cb;
    int err = -1;

    cb = nl_cb_clone(state->nl_cb);
    if (!cb)
        goto out;

    /* Send the message over to the host driver */
    err = nl_send_auto_complete(state->nl, msg);
    if (err < 0)
        goto out;

    err = 1;

    /* Set the callbacks to listen for a response */
    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

    if (valid_handler)
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
              valid_handler, valid_data);

    /* Loop waiting for a response */
    while (err > 0) {
        int res = nl_recvmsgs(state->nl, cb);
        if (res < 0)
            printf("nl80211: nl_recvmsgs failed: %d\n", res);
    }

out:
    nl_cb_put(cb);
    nlmsg_free(msg);
    return err;
}

static void *nl80211_cmd(struct nl80211_state *state,
                         struct nl_msg *msg, int flags, uint8_t cmd)
{
    return genlmsg_put(msg, 0, 0, state->nl80211_id, 0, flags, cmd, 0);
}

static int dsrc_nl_common(char *interface, struct nl80211_state *state,
                          struct nl_msg **msg, uint8_t subcmd)
{
    int if_idx;

    if (state_init(state))
        return -1;

    /* Get the interface index number */
    if_idx = if_nametoindex(interface);
    if (!if_idx) {
        printf("nl80211: %s interface not found!\n", interface);
        return -1;
    }

    *msg = nlmsg_alloc();
    if (!*msg) {
        printf("nl80211: Failed to allocate netlink message!\n");
        return -1;
    }

    /* Prepare the vendor command */
    if (!nl80211_cmd(state, *msg, 0, NL80211_CMD_VENDOR) ||
            nla_put_u32(*msg, NL80211_ATTR_IFINDEX,
                if_nametoindex(interface)) ||
            nla_put_u32(*msg, NL80211_ATTR_VENDOR_ID, QCA_NL80211_VENDOR_ID) ||
            nla_put_u32(*msg, NL80211_ATTR_VENDOR_SUBCMD,
                subcmd)) {
        nlmsg_free(*msg);
        return -1;
    }

    return 0;
}

int dsrc_ocb_set_config(char *interface, int num_channels,
    dsrc_ocb_config_channel_t *chan, dcc_ndl_chan *ndl_chan, int sched_size,
    dsrc_ocb_config_sched_t *sched, int num_active_states,
    dcc_ndl_active_state_config *state_cfg, unsigned int flags)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;
    int i;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_OCB_SET_CONFIG);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_COUNT,
        num_channels);
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_SIZE,
        sched_size);
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_ARRAY,
        num_channels*sizeof(*chan), chan);
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_ARRAY,
        sched_size*sizeof(*sched), sched);

    if (ndl_chan != NULL) {
        /* put in the DCC array */
        nla_put(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_CHANNEL_ARRAY,
            num_channels*sizeof(*ndl_chan), ndl_chan);
        /* put in the active state array */
        if (state_cfg != NULL) {
            num_active_states = 0;
            for (i = 0; i < num_channels; i++) {
                num_active_states += DCC_NDL_NUM_ACTIVE_STATE_GET(&ndl_chan[i]);
            }
            nla_put(msg,
                QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_ACTIVE_STATE_ARRAY,
                num_active_states*sizeof(*state_cfg), state_cfg);
        }
    }

    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_FLAGS, flags);

    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_dcc_ndl_update(char *interface, int num_channels,
                           dcc_ndl_chan *ndl_chan, int num_active_states,
                           dcc_ndl_active_state_config *state_cfg)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1, i;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_DCC_UPDATE_NDL);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;

    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_COUNT,
        num_channels);

    /* put in the DCC array */
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_ARRAY,
        num_channels*sizeof(dcc_ndl_chan), ndl_chan);
    num_active_states = 0;
    for (i = 0; i < num_channels; i++) {
        num_active_states += DCC_NDL_NUM_ACTIVE_STATE_GET(&ndl_chan[i]);
    }
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_ACTIVE_STATE_ARRAY,
        num_active_states*sizeof(dcc_ndl_active_state_config), state_cfg);

    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command %d!\n", rc);
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_dcc_get_stats(char *interface, int num_channels,
                    dcc_channel_stats_request *stats_request)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_DCC_GET_STATS);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_CHANNEL_COUNT,
        num_channels);
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_REQUEST_ARRAY,
        num_channels*sizeof(dcc_channel_stats_request), stats_request);
    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, dcc_stats_handler, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_dcc_clear_stats(char *interface, uint32_t dcc_stats_bitmap)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_DCC_CLEAR_STATS);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_BITMAP,
        dcc_stats_bitmap);
    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_get_tsf_timer(char *interface)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_OCB_GET_TSF_TIMER);
    if (rc) {
        return rc;
    }

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, tsf_timer_handler, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_set_utc_time(char *interface, uint8_t time_value[DSRC_OCB_UTC_TIME_LEN],
    uint8_t time_error[DSRC_OCB_UTC_TIME_ERROR_LEN])
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_OCB_SET_UTC_TIME);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_VALUE, DSRC_OCB_UTC_TIME_LEN,
        time_value);
    nla_put(msg, QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_ERROR,
        DSRC_OCB_UTC_TIME_ERROR_LEN, time_error);
    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_start_timing_advert(char *interface, uint32_t channel_freq,
    uint32_t repeat_rate)
{
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_OCB_START_TIMING_ADVERT);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_CHANNEL_FREQ,
        channel_freq);
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_REPEAT_RATE,
        repeat_rate);
    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

int dsrc_ocb_stop_timing_advert(char *interface, uint32_t channel_freq) {
    struct nl80211_state state;
    struct nl_msg *msg = NULL;
    struct nlattr *data;
    int rc = -1;

    rc = dsrc_nl_common(interface, &state, &msg,
        QCA_NL80211_VENDOR_SUBCMD_OCB_STOP_TIMING_ADVERT);
    if (rc) {
        return rc;
    }

    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (!data)
        goto nla_put_failure;
    NLA_PUT_U32(msg, QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_CHANNEL_FREQ,
        channel_freq);
    nla_nest_end(msg, data);

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(&state, msg, NULL, NULL);
    msg = NULL;
    if (rc) {
        printf("nl80211: Error sending nl80211 command!\n");
        goto nla_put_failure;
    }

    rc = 0;

nla_put_failure:
    if (msg)
        nlmsg_free(msg);
    state_deinit(&state);

    return rc;
}

static int dsrc_nl_get_multicast_id(struct nl80211_state *state,
                                    const char *family, const char *group)
{
    struct nl_msg *msg;
    int ret = -1;
    struct family_data res = {group, -1};

    msg = nlmsg_alloc();
    if (!msg)
        return -1;
    genlmsg_put(msg, 0, 0, genl_ctrl_resolve(state->nl, "nlctrl"),
            0, 0, CTRL_CMD_GETFAMILY, 0);
    NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);

    ret = send_and_recv_msgs(state, msg, family_handler, &res);
    msg = NULL;
    if (ret == 0)
        ret = res.id;

nla_put_failure:
    nlmsg_free(msg);
    return ret;
}

void dsrc_ocb_dcc_stats_event(char *interface)
{
    struct nl80211_state state;
    struct nl_sock *sock = NULL;
    struct nl_cb *cb;
    int err = 1, ret;

    /* Get the interface index number */
    if (!if_nametoindex(interface)) {
        printf("nl80211: %s interface not found!\n", interface);
        return;
    }

    if (state_init(&state))
        return;

    cb = nl_cb_clone(state.nl_cb);
    if (!cb)
        goto out;
    nl_cb_set(cb, NL_CB_MSG_IN, NL_CB_CUSTOM, dcc_stats_event_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

    sock = nl_socket_alloc_cb(cb);
    if (sock == NULL) {
        printf("nl80211: Failed to allocate netlink callback\n");
        goto out;
    }

    if (genl_connect(sock)) {
        printf("nl80211: Failed to connect to generic netlink\n");
        goto out;
    }

    ret = dsrc_nl_get_multicast_id(&state, "nl80211", "vendor");
    if (ret >= 0) {
        ret = nl_socket_add_membership(sock, ret);
    }
    if (ret < 0) {
        printf("nl80211: Could not add event multicast membership\n");
        goto out;
    }

    /* Loop waiting for a response */
    while (err > 0) {
        nl_recvmsgs(sock, cb);
    }

out:
    if (sock) {
        nl_socket_free(sock);
    }
    nl_cb_put(cb);
    state_deinit(&state);
}

static int dsrc_ocb_register_for_action_frames(struct nl80211_state *state,
                                              char *interface)
{
    struct nl_msg *msg;
    int rc;

    msg = nlmsg_alloc();
    if (!msg) {
        printf("nl80211: Failed to allocate netlink message!\n");
        return -1;
    }

    /* Prepare the vendor command */
    if (!nl80211_cmd(state, msg, 0, NL80211_CMD_REGISTER_FRAME) ||
            nla_put_u32(msg, NL80211_ATTR_IFINDEX,
                if_nametoindex(interface)) ||
            nla_put_u8(msg, NL80211_ATTR_FRAME_MATCH, 127)) {
        nlmsg_free(msg);
        return -1;
    }

    /* Send the message and obtain the response */
    rc = send_and_recv_msgs(state, msg, NULL, NULL);
    msg = NULL;
    if (rc)
        printf("Failed to register for action frames: %d\n", rc);

    return rc;
}

static void print_wifi_header(const ieee80211_hdr_t *wifi_header)
{
    printf("Wi-Fi header (802.11)\n");
    printf("  Frame Control:\n");
    printf("    Protocol Version: %d\n", (wifi_header->frame_control) & 0x03);
    printf("    Type: %d\n", (wifi_header->frame_control >> 2) & 0x03);
    printf("    Subtype: %d\n", (wifi_header->frame_control >> 4) & 0x0f);
    printf("    To DS: %d\n", (wifi_header->frame_control >> 8) & 0x01);
    printf("    From DS: %d\n", (wifi_header->frame_control >> 9) & 0x1);
    printf("    More Flag: %d\n", (wifi_header->frame_control >> 10) & 0x01);
    printf("    Retry: %d\n", (wifi_header->frame_control >> 11) & 0x01);
    printf("    Pwr Mgr: %d\n", (wifi_header->frame_control >> 12) & 0x01);
    printf("    More Data: %d\n", (wifi_header->frame_control >> 13) & 0x01);
    printf("    WEP: %d\n", (wifi_header->frame_control >> 14) & 0x01);
    printf("    Order: %d\n", (wifi_header->frame_control >> 15) & 0x01);
    printf("  Duration/ID: %d\n", wifi_header->duration);
    printf("  Address 1 (dest): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_1[0], wifi_header->address_1[1],
           wifi_header->address_1[2], wifi_header->address_1[3],
           wifi_header->address_1[4], wifi_header->address_1[5]);
    printf("  Address 2 (source): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_2[0], wifi_header->address_2[1],
           wifi_header->address_2[2], wifi_header->address_2[3],
           wifi_header->address_2[4], wifi_header->address_2[5]);
    printf("  Address 3 (BSSID): %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_header->address_3[0], wifi_header->address_3[1],
           wifi_header->address_3[2], wifi_header->address_3[3],
           wifi_header->address_3[4], wifi_header->address_3[5]);
    printf("  Sequence/Control: %d\n", wifi_header->sequence);
}

static int rx_vsa_frames_handler(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
        genlmsg_attrlen(gnlh, 0), NULL);

    if (gnlh->cmd == NL80211_CMD_FRAME) {
        printf("NL80211_CMD_FRAME\n");

        if (tb[NL80211_ATTR_WIPHY])
            printf("Wiphy index: %d\n", nla_get_u32(tb[NL80211_ATTR_WIPHY]));
        else
            printf("Wiphy index: not present\n");


        if (tb[NL80211_ATTR_WDEV])
            printf("Wdev ID: %llx\n", nla_get_u64(tb[NL80211_ATTR_WDEV]));
        else
            printf("Wdev ID: not present\n");

        if (tb[NL80211_ATTR_WIPHY_FREQ])
            printf("Frequency: %d\n", nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]));
        else
            printf("Frequency: not presenet\n");

        if (tb[NL80211_ATTR_RX_SIGNAL_DBM])
            printf("Signal strength (mBm): %d\n", nla_get_u32(tb[
                                                  NL80211_ATTR_RX_SIGNAL_DBM]));
        else
            printf("Signal strength (mBm): not present\n");

        if (tb[NL80211_ATTR_FRAME]) {
            int length = nla_len(tb[NL80211_ATTR_FRAME]);
            uint8_t *data = nla_data(tb[NL80211_ATTR_FRAME]);
            uint8_t *data_end = data + length;
            int i;

            printf("Frame:");
            for (i = 0; i < length; i++) {
                if (i % 16 == 0)
                    printf("\n    ");
                printf("%02x ", data[i]);
            }

            printf("\n");

            print_wifi_header((const ieee80211_hdr_t *) data);
            data += sizeof(ieee80211_hdr_t);

            printf("Category: %x\n", *data);
            data++;
            printf("Organization Identifier: %x %x %x\n", data[0], data[1],
                   data[2]);
            data += 3;
            printf("Vendor Specific Content:");
            length = data_end - data;
            for (i = 0; i < length; i++) {
                if (i % 16 == 0)
                    printf("\n    ");
                printf("%02x ", data[i]);
            }
            printf("\n");
            printf("\n");
        } else {
            printf("Frame: not present\n");
        }
    } else {
        printf("Unknown netlink command. gnlh->cmd == %d\n", gnlh->cmd);
    }

    return NL_SKIP;
}

void dsrc_ocb_rx_vsa_frames(char *interface)
{
    struct nl80211_state state;
    struct nl_cb *cb;
    int err = 1;

    /* Get the interface index number */
    if (!if_nametoindex(interface)) {
        printf("nl80211: %s interface not found!\n", interface);
        return;
    }

    if (state_init(&state))
        return;

    cb = nl_cb_clone(state.nl_cb);
    if (!cb)
        goto out;
    nl_cb_set(cb, NL_CB_MSG_IN, NL_CB_CUSTOM, rx_vsa_frames_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

    dsrc_ocb_register_for_action_frames(&state, interface);

    /* Loop waiting for a response */
    while (err > 0) {
        nl_recvmsgs(state.nl, cb);
    }

out:
    nl_cb_put(cb);
    state_deinit(&state);
}

