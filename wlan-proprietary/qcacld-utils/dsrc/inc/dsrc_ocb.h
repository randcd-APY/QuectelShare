/*******************************************************************************
 * Copyright (c) 2015 Qualcomm Atheros, Inc. All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 ******************************************************************************/
#ifndef DSRC_OCB_H
#define DSRC_OCB_H

#include <stdint.h>

/**
 * @file
 * @brief OCB per-packet data headers.
 */

#define PACKED __attribute__ ((packed))

/** The size of the utc time in bytes. */
#define DSRC_OCB_UTC_TIME_LEN (10)
/** The size of the utc time error in bytes. */
#define DSRC_OCB_UTC_TIME_ERROR_LEN (5)

/*!
 @brief These values can be used in the datarate field of the tx control header.
 @details
    This table shows the data rates yield in the context of 20Mhz bandwidth
    channel. Data rate yields will be proportionally down in the context of
    10MHz and 5Mhz bandwidth channels.<br>
       | VALUE   |  DATA RATE (mbps)  |  MODULATION | CODING RATE |<br>
       | :-----: | :----------------: | :----------:|:----------: |<br>
       | 0       |  6                 |  BPSK       | 1/2         |<br>
       | 1       |  9                 |  BPSK       | 3/4         |<br>
       | 2       |  12                |  QPSK       | 1/2         |<br>
       | 3       |  18                |  QPSK       | 3/4         |<br>
       | 4       |  24                |  16-QAM     | 1/2         |<br>
       | 5       |  36                |  16-QAM     | 3/4         |<br>
       | 6       |  48                |  64-QAM     | 1/2         |<br>
       | 7       |  54                |  64-QAM     | 3/4         |<br>

       For example a value 2 on 10Mhz bandwidth will result in 6Mbps data rate.
*/
enum dsrc_ocb_ofdm_datarate {
    dsrc_ocb_ofdm_datarate_6_mbps = 0,
    dsrc_ocb_ofdm_datarate_9_mbps = 1,
    dsrc_ocb_ofdm_datarate_12_mbps = 2,
    dsrc_ocb_ofdm_datarate_18_mbps = 3,
    dsrc_ocb_ofdm_datarate_24_mbps = 4,
    dsrc_ocb_ofdm_datarate_36_mbps = 5,
    dsrc_ocb_ofdm_datarate_48_mbps = 6,
    dsrc_ocb_ofdm_datarate_54_mbps = 7,
    dsrc_ocb_ofdm_datarate_max = 7,
};

/*!
 * @brief TX control header
 * @details
 *  When sending an OCB packet, the user application has
 *  the option of including the following struct following an ethernet header
 *  with the proto field set to 0x8151. This struct includes various TX
 *  paramaters including the TX power and datarate.
 */
struct dsrc_ocb_tx_ctrl_hdr {
    /*! The version must be 1. */
    uint16_t version;
    /*! length - The length of this structure */
    uint16_t length;
    /*! channel_freq - The center frequency for the packet */
    uint16_t channel_freq;

    /*! flags */
    union {
        /** a convenient structure of all the flags */
        struct {
            uint16_t
                /*! bit 0: if set, tx pwr spec is valid */
                valid_pwr:            1,
                /*! bit 1: if set, tx MCS mask spec is valid */
                valid_datarate:       1,
                /*! bit 2: if set, tx retries spec is valid */
                valid_retries:        1,
                /*! bit 3: if set, chain mask is valid */
                valid_chain_mask:     1,
                /*! bit 4: if set, tx expire TSF spec is valid*/
                valid_expire_tsf:     1,
                /*! bit 5: if set, TID is valid */
                valid_tid:            1,
                /*! bits 15:6 - unused, set to 0x0 */
                reserved0_15_6:      10;
        } b;
        /*! the union of all flags */
        uint16_t all_flags;
    } flags;

    /*! TX expiry time (TSF) LSBs */
    uint32_t expire_tsf_lo;

    /*! TX expiry time (TSF) MSBs */
    uint32_t expire_tsf_hi;

    /*!
     * Specify what power the tx frame needs to be transmitted at.
     * The power a signed (two's complement) value is in units of 0.5 dBm.
     * The value needs to be appropriately sign-extended when extracting
     * the value from the message and storing it in a variable that is
     * larger than A_INT8.
     * If the transmission uses multiple tx chains, this power spec is
     * the total transmit power, assuming incoherent combination of
     * per-chain power to produce the total power.
     */
    int8_t pwr;

    /*!
     * The desired modulation and coding scheme.
     * @see enum dsrc_ocb_ofdm_datarate
     */
    uint8_t datarate;

    /*!
     * Specify the maximum number of transmissions, including the
     * initial transmission, to attempt before giving up if no ack
     * is received.
     * If the tx rate is specified, then all retries shall use the
     * same rate as the initial transmission.
     * If no tx rate is specified, the target can choose whether to
     * retain the original rate during the retransmissions, or to
     * fall back to a more robust rate.
     */
    uint8_t retry_limit;

    /*! specify which chains to transmit from. */
    uint8_t chain_mask;

    /*! Extended Traffic ID (0-15) */
    uint8_t ext_tid;

    /*! This is to ensure that the size of the structure is a multiple of 4. */
    uint8_t reserved[3];
} PACKED;

/**
 * @brief RX stats header
 * @details
 *  When receiving an OCB packet, the RX stats is sent to the user application
 *  so that the user application can do processing based on the RX stats.
 *  This structure will be preceded by an ethernet header with
 *  the proto field set to 0x8152. This struct includes various RX
 *  paramaters including RSSI, data rate, and center frequency.
 */
struct dsrc_ocb_rx_stats_hdr {
    /*! The version must be 1. */
    uint16_t version;
    /*! The length of this structure */
    uint16_t length;
    /*! The center frequency for the packet */
    uint16_t channel_freq;
    /*! Combined Received signal strength indicator (RSSI) from all chains */
    int16_t rssi_cmb;
    /*! Received signal strength indicator (RSSI)
     *  for chains 0 through 3 (for 20 MHz bandwidth) */
    int16_t rssi[4];
    /*! Timestamp in TSF units */
    uint32_t tsf32;
    /*! Timestamp in microseconds */
    uint32_t timestamp_microsec;
    /*!
     * The modulation and coding scheme index of the received signal.
     * @see enum dsrc_ocb_ofdm_datarate
     */
    uint8_t datarate;
    /*! Submicrosecond portion of the timestamp */
    uint8_t timestamp_submicrosec;
    /*! Extended Traffic ID (TID) of the received signal */
    uint8_t ext_tid;
    /*! This ensure the size of the structure is a multiple of 4 */
    uint8_t reserved;
} PACKED;

#define DSRC_OCB_AC_BE  0     /**< Best Effort access class for EDCA */
#define DSRC_OCB_AC_BK  1     /**< BacKground access class for EDCA */
#define DSRC_OCB_AC_VI  2     /**< VIdeo access class for EDCA */
#define DSRC_OCB_AC_VO  3     /**< VOice access class for EDCA */

/**
 * @brief Number of access classes.
 */
#define DSRC_OCB_NUM_AC 4

/**
 * @brief Achieve EDCA priority QOS scheme as described in 802.11.
 * @details
 *   Members provide the Arbitration Inter-frame Spacing number,
 *   Contention Window min-max settings.
 */
struct dsrc_ocb_qos_params {
    uint8_t aifsn;   /**< AIFSN parameters. Range: 2 to 15 */
    uint8_t cwmin;   /**< CWmin in exponential form. Range: 1 to 10 */
    uint8_t cwmax;   /**< CWmax in exponential form. Range: 1 to 10 */
};

/**
 * @brief Max number of channels for struct dsrc_ocb_sched
 */
#define DSRC_OCB_CHANNEL_MAX 5

/**
 * @brief DEPRECATED
 * @details
 *   Channel description for OCB communications.
 */
struct dsrc_ocb_channel {
    uint32_t channel_freq;           /**< Channel center frequency (MHz) */
    uint32_t duration;               /**< Channel Duration (ms) */
    uint32_t start_guard_interval;   /**< Start Guard Interval (ms) */
    uint32_t channel_bandwidth;      /**< channel bandwidth as MHz (eg: 5, 10, 20, 40) */
    uint32_t tx_power;               /**< max Transmit Power (dBm) allowed */
    uint32_t tx_rate;                /**< Transmit Data Rate (mbit) */
    struct dsrc_ocb_qos_params qos_params[DSRC_OCB_NUM_AC]; /**< EDCA parameters as described by 802.11 for OCB operations */
    uint32_t reserved;               /**< This must be 0 */
};

/**
 * @brief DEPRECATED
 * @details
 *   Channel scheduling information.
 */
struct dsrc_ocb_sched {
    uint32_t num_channels;           /**< number of channels in the array channels[] */
    struct dsrc_ocb_channel channels[DSRC_OCB_CHANNEL_MAX]; /**< channels array */
    uint32_t reserved;               /**< This must be 0 */
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * DEPRECATED. Set the dsrc channel scheduler parameters.
 * Use dsrc_ocb_set_config instead.
 *
 * @param interface Interface name
 * @param schedule schedule parameters
 * @return 0 on success. Negative value on failure
 */
int dsrc_ocb_set_schedule(char *interface, struct dsrc_ocb_sched *schedule);

/*!
 * Set UTC time for use with Timing advertisement.
 *
 * @param interface the OCB interface name
 * @param time_value the UTC time as a 10-byte value
 * @param time_error The 5-byte time error as specified in 802.11p spec.
 *                   An estimate of error in time_value.
 * @return result of the command execution
 */
int dsrc_ocb_set_utc_time(char *interface,
                          uint8_t time_value[DSRC_OCB_UTC_TIME_LEN],
                          uint8_t time_error[DSRC_OCB_UTC_TIME_ERROR_LEN]);

/**
 * Start sending a time advertisement on a given channel and the rate.
 *
 * @param interface the OCB interface name
 * @param channel_freq The channel frequency on which to send the TA frames.
 *                     This must be the channel already scheduled using
 *                     dsrc_ocb_set_schedule().
 * @param repeat_rate The number of times in 5 seconds to send the frames
 * @return result 0 on success. Negative value on failure.
 */
int dsrc_ocb_start_timing_advert(char *interface, uint32_t channel_freq,
                                 uint32_t repeat_rate);

/**
 * Stop time advertisement on a given channel.
 *
 * @param interface the OCB interface name
 * @param channel_freq the channel frequency on which to stop sending TA frames
 * @return result of the command execution
 */
int dsrc_ocb_stop_timing_advert(char *interface, uint32_t channel_freq);

#ifdef __cplusplus
}
#endif

#endif /* !DSRC_OCB_H */

