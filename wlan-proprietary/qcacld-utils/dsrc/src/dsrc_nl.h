/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/**
 * @file dsrc_nl.h
 * @brief Prototypes for the nl80211 APIs.
 *
 * Prototypes for the nl80211 layer APIs. This is a wrapper to the complete
 * nl80211 sockets, so application developers do not need to be aware of any
 * nl80211 specific details. There is an API for each DSRC control command.
 */

#ifndef DSRC_NL_H
#define DSRC_NL_H

#include "dsrc_config.h"
#include <stdint.h>

/** nl80211 global state. */
struct nl80211_state {
    struct nl_cb *nl_cb;
    struct nl_sock *nl;
    int nl80211_id;
};

/** Multicast family data for nl80211. */
struct family_data {
    const char *group;
    int id;
};

/** QCA vendor ID for nl80211 vendor commands. */
#define QCA_NL80211_VENDOR_ID 0x001374

/** DSRC nl80211 vendor command IDs */
enum qca_nl80211_vendor_subcmds {
    QCA_NL80211_VENDOR_SUBCMD_OCB_SET_CONFIG = 92,
    QCA_NL80211_VENDOR_SUBCMD_OCB_SET_UTC_TIME = 93,
    QCA_NL80211_VENDOR_SUBCMD_OCB_START_TIMING_ADVERT = 94,
    QCA_NL80211_VENDOR_SUBCMD_OCB_STOP_TIMING_ADVERT = 95,
    QCA_NL80211_VENDOR_SUBCMD_OCB_GET_TSF_TIMER = 96,
    QCA_NL80211_VENDOR_SUBCMD_DCC_GET_STATS = 97,
    QCA_NL80211_VENDOR_SUBCMD_DCC_CLEAR_STATS = 98,
    QCA_NL80211_VENDOR_SUBCMD_DCC_UPDATE_NDL = 99,
    QCA_NL80211_VENDOR_SUBCMD_DCC_STATS_EVENT = 100,
};

/**
 * enum qca_wlan_vendor_attr_ocb_set_config - vendor subcmd to set ocb config
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_COUNT:
 *  number of channels in the configuration
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_SIZE: size of the schedule
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_ARRAY: array of channels
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_ARRAY:
 *  array of channels to be scheduled
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_CHANNEL_ARRAY:
 *  array of NDL channel information
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_ACTIVE_STATE_ARRAY:
 *  array of NDL active state configuration
 */
enum qca_wlan_vendor_attr_ocb_set_config {
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_COUNT,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_SIZE,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_CHANNEL_ARRAY,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_SCHEDULE_ARRAY,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_CHANNEL_ARRAY,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_NDL_ACTIVE_STATE_ARRAY,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_FLAGS,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_MAX =
        QCA_WLAN_VENDOR_ATTR_OCB_SET_CONFIG_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_ocb_set_utc_time - vendor subcmd to set UTC time
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_VALUE:
 *  the UTC time as an array of 10 bytes
 * @QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_ERROR:
 *  the time error as an array of 5 bytes
 */
enum qca_wlan_vendor_attr_ocb_set_utc_time {
    QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_VALUE,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_ERROR,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_MAX =
        QCA_WLAN_VENDOR_ATTR_OCB_SET_UTC_TIME_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_ocb_start_timing_advert - vendor subcmd to start
                               sending timing advert
                               frames
 * @QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_CHANNEL_FREQ:
 *  channel frequency on which to send the frames
 * @QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_REPEAT_RATE:
 *  number of times the frame is sent in 5 seconds
 */
enum qca_wlan_vendor_attr_ocb_start_timing_advert {
    QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_CHANNEL_FREQ,
    QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_REPEAT_RATE,
    QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_MAX =
        QCA_WLAN_VENDOR_ATTR_OCB_START_TIMING_ADVERT_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_ocb_stop_timing_advert - vendor subcmd to stop
 *                            timing advert
 * @QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_CHANNEL_FREQ:
 *  the channel frequency on which to stop the timing advert
 */
enum qca_wlan_vendor_attr_ocb_stop_timing_advert {
    QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_CHANNEL_FREQ,
    QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_MAX =
        QCA_WLAN_VENDOR_ATTR_OCB_STOP_TIMING_ADVERT_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_dcc_get_tsf_response - vendor subcmd to get TSF
 *                          timer value
 * @QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_HIGH:
 *      higher 32 bits of the timer
 * @QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_LOW:
 *      lower 32 bits of the timer
 */
enum qca_wlan_vendor_attr_ocb_get_tsf_resp {
    QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_HIGH,
    QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_TIMER_LOW,
    QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_MAX =
        QCA_WLAN_VENDOR_ATTR_OCB_GET_TSF_RESP_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_dcc_get_stats - vendor subcmd to get
 *                       dcc stats
 * @QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_CHANNEL_COUNT:
 *      the number of channels in the request array
 * @QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_REQUEST_ARRAY
 *      array of the channel and information being requested
 */
enum qca_wlan_vendor_attr_dcc_get_stats {
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_CHANNEL_COUNT,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_REQUEST_ARRAY,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_MAX =
        QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_dcc_get_stats_resp - response event from get
 *                        dcc stats
 * @QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_CHANNEL_COUNT:
 *      the number of channels in the request array
 * @QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_STATS_ARRAY
 *      array of the information being requested
 */
enum qca_wlan_vendor_attr_dcc_get_stats_resp {
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_CHANNEL_COUNT,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_STATS_ARRAY,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_MAX =
        QCA_WLAN_VENDOR_ATTR_DCC_GET_STATS_RESP_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_dcc_clear_stats - vendor subcmd to clear DCC stats
 * @QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_BITMAP:
 *      mask of the type of stats to be cleared
 */
enum qca_wlan_vendor_attr_dcc_clear_stats {
    QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_BITMAP,
    QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_MAX =
        QCA_WLAN_VENDOR_ATTR_DCC_CLEAR_STATS_AFTER_LAST - 1,
};

/**
 * enum qca_wlan_vendor_attr_ocb_set_config - vendor subcmd to update dcc
 * @QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_COUNT:
 *  number of channels in the configuration
 * @QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_ARRAY: the array of NDL
 *  channel info
 * @QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_ACTIVE_STATE_ARRAY: the array of
 *  NDL active states
 */
enum qca_wlan_vendor_attr_dcc_update_ndl {
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_INVALID = 0,
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_COUNT,
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_CHANNEL_ARRAY,
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_ACTIVE_STATE_ARRAY,
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_AFTER_LAST,
    QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_MAX =
        QCA_WLAN_VENDOR_ATTR_DCC_UPDATE_NDL_AFTER_LAST - 1,
};

#endif /* _DSRC_NL_H_ */
