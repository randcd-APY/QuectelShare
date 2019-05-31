/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/**
 * @file dsrc_config.h
 * @brief Prototypes for the dsrc_config APIs.
 *
 * Prototypes for the dsrc_config layer APIs. There is a method for each
 * DSRC control command.
 */

#ifndef DSRC_CONFIG_H
#define DSRC_CONFIG_H

#include "dsrc_dcc.h"
#include "dsrc_ocb.h"
#include <stdint.h>

#define OCB_CHANNEL_FLAG_DISABLE_RX_STATS_HDR	(1 << 0)

/** OCB channel information.
 *
 *  This is used when configuring the host driver. */
typedef struct dsrc_ocb_config_channel {
    uint32_t chan_freq; /**< The channel center frequency in MHz */
    uint32_t bandwidth; /**< The channel bandwidth in MHz (5, 10, or 20) */
    uint16_t flags; /**< Flags to be applied to this channel such as OCB_CHANNEL_FLAG_DISABLE_RX_STATS_HDR */
    uint8_t reserved[4]; /**< This must be 0 */
    struct dsrc_ocb_qos_params qos_params[DSRC_OCB_NUM_AC]; /**< QOS parameters */
    uint32_t max_pwr; /**< Maximum allowed transmit power on this channel */
    uint32_t min_pwr; /**< Minimum allowed transmit power on this channel */
} dsrc_ocb_config_channel_t;

/** OCB schedule information.
 *
 *  This is used when configuring the host driver.
 */
typedef struct dsrc_ocb_config_sched {
    uint32_t chan_freq; /**< The channel center frequency. This must correspond to a configured channel. */
    uint32_t total_duration; /**< The total amount of time (including guard interval) to tune to this channel in ms */
    uint32_t guard_interval; /**< The amount of time at the beginning of the interval to wait before transmitting */
} dsrc_ocb_config_sched_t;

/** TSF timer composition. */
struct tsf_timer {
    uint32_t timer_high; /**< The most significant 4 bytes of the TSF */
    uint32_t timer_low; /**< The least significant 4 bytes of the TSF */
};

/** Flag to indicate expiry time in TSF.
 *
 *  This flag is set if the expiry time in the TX control header is to be
 *  interpreted as an absolute TSF time. If this flag is not set, then the
 *  expiry time is interpreted as a relative time.
 */
#define OCB_CONFIG_FLAG_EXPIRY_TIME_IN_TSF	0x01

/** Flag to indicate raw mode.
 *
 *  This flag if set if the user app will transmit 802.11 frames and will
 *  received 802.11 frames. If this flag is not set, the user app will
 *  send 802.3 frames and the FW will convert them to 802.11 frames,
 *  and the FW will receive 802.11 frames and convert them to 802.3 frames
 *  before propagating them to the user app.
 */
#define OCB_CONFIG_FLAG_RAW_MODE		0x02

/**
 * Send a set config command and obtain response via nl80211.
 *
 * @param interface the OCB interface name
 * @param num_channels the number of OCB channels
 * @param chan the OCB channels info
 * @param ndl_chan the DCC channels info
 * @param sched_size the channel schedule size
 * @param sched the channel schedule info
 * @param num_active_states the number of active states
 * @param state_cfg the active state array info
 * @param flags configuration flags such as OCB_CONFIG_FLAG_EXPIRY_TIME_IN_TSF
 * @return result of the command execution
 */
int dsrc_ocb_set_config(char *interface, int num_channels,
    dsrc_ocb_config_channel_t *chan, dcc_ndl_chan *ndl_chan, int sched_size,
    dsrc_ocb_config_sched_t *sched, int num_active_states,
    dcc_ndl_active_state_config *state_cfg, unsigned int flags);

/**
 * Sends a DCC NDL update command via nl80211.
 *
 * @param interface the OCB interface name
 * @param num_channels the number of DCC channels
 * @param ndl_chan the list of DCC channel info
 * @param num_active_states the number of active states
 * @param state_cfg the active state array
 * @return result of the command execution
 */
int dsrc_ocb_dcc_ndl_update(char *interface, int num_channels,
    dcc_ndl_chan *ndl_chan, int num_active_states,
    dcc_ndl_active_state_config *state_cfg);

/**
 * Sends a DCC get stats command via nl80211 and print the results.
 *
 * @param interface the OCB interface name
 * @param num_channels the number of DCC channels
 * @param stats_request the stats request array
 * @return result of the command execution
 */
int dsrc_ocb_dcc_get_stats(char *interface, int num_channels,
                    dcc_channel_stats_request *stats_request);

/**
 * Enters a loop receiving DCC stats event messages and printing the stats.
 *
 * @param interface the OCB interface name
 */
void dsrc_ocb_dcc_stats_event(char *interface);

/**
 * Sends a DCC clear stats command via nl80211.
 *
 * @param interface the OCB interface name
 * @param dcc_stats_bitmap the DCC stats bitmap
 * @return result of the command execution
 */
int dsrc_ocb_dcc_clear_stats(char *interface, uint32_t dcc_stats_bitmap);

/**
 * Sends a get TSF timer command and print the timer value.
 *
 * @param interface the OCB interface name
 */
int dsrc_ocb_get_tsf_timer(char *interface);

/**
 * Enters a loop receiving VSA frames and printing the contents.
 *
 * @param interface the OCB interface name
 */
void dsrc_ocb_rx_vsa_frames(char *interface);


#endif /* DSRC_CONFIG_H */

