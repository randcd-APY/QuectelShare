/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 */

/**
 * DOC: spectral_scan_fmt.h
 *
 * WLAN Host Device Driver Spectral Scan Implementation
 */

#ifndef _SPECTRAL_SCAN_FMT_H_
#define _SPECTRAL_SCAN_FMT_H_

#include <inttypes.h>
#ifndef __ATTRIB_PACK
#define __ATTRIB_PACK __attribute__((packed))
#endif

#define MAX_INTERF 10 /* 5 categories x (lower + upper) bands */
#define MAX_NUM_BINS 520
#define MAX_SPECTRAL_CHAINS  3

/**
 * struct INTERF_RSP_INFO - Interference source info
 * @interf_min_freq: Interference minimum frequency
 * @interf_max_freq: Interference maximum frequency
 * @interf_type: Interference type
 */
struct INTERF_RSP_INFO {
	uint16_t interf_min_freq;
	uint16_t interf_max_freq;
	uint8_t  interf_type;
} __ATTRIB_PACK;

/**
 * struct INTERF_SRC_RSP_INFO - Interference source info
 * @count: number of interference sources
 * @interf: interference info
 */
struct INTERF_SRC_RSP_INFO {
	uint16_t count;
	struct INTERF_RSP_INFO interf[MAX_INTERF];
} __ATTRIB_PACK;

/**
 * enum dcs_int_type - Interference type indicated by DCS
 * @SPECTRAL_DCS_INT_NONE:  No interference
 * @SPECTRAL_DCS_INT_CW:  CW interference
 * @SPECTRAL_DCS_INT_WIFI:  WLAN interference
 */
enum dcs_int_type {
	SPECTRAL_DCS_INT_NONE,
	SPECTRAL_DCS_INT_CW,
	SPECTRAL_DCS_INT_WIFI
};

/**
 * struct spectral_classifier_params - spectral classifier parameters
 * @spectral_20_40_mode:  Is AP in 20/40 mode?
 * @spectral_dc_index:    DC index
 * @spectral_dc_in_mhz:   DC in MHz
 * @upper_chan_in_mhz:    Upper channel in MHz
 * @lower_chan_in_mhz:    Lower channel in MHz
 */
struct spectral_classifier_params {
	int spectral_20_40_mode;
	int spectral_dc_index;
	int spectral_dc_in_mhz;
	int upper_chan_in_mhz;
	int lower_chan_in_mhz;
} __ATTRIB_PACK;

/**
 * struct spectral_samp_data - Spectral Analysis Messaging Protocol Data format
 * @spectral_data_len:        Indicates the bin size
 * @spectral_data_len_sec80:  Indicates the bin size for secondary 80 segment
 * @spectral_rssi:            Indicates RSSI
 * @spectral_rssi_sec80:      Indicates RSSI for secondary 80 segment
 * @spectral_combined_rssi:   Indicates combined RSSI from all antennas
 * @spectral_upper_rssi:      Indicates RSSI of upper band
 * @spectral_lower_rssi:      Indicates RSSI of lower band
 * @spectral_chain_ctl_rssi:  RSSI for control channel, for all antennas
 * @spectral_chain_ext_rssi:  RSSI for extension channel, for all antennas
 * @spectral_max_scale:       Indicates scale factor
 * @spectral_bwinfo:          Indicates bandwidth info
 * @spectral_tstamp:          Indicates timestamp
 * @spectral_max_index:       Indicates the index of max magnitude
 * @spectral_max_index_sec80: Indicates the index of max magnitude for secondary
 *                            80 segment
 * @spectral_max_mag:         Indicates the maximum magnitude
 * @spectral_max_mag_sec80:   Indicates the maximum magnitude for secondary 80
 *                            segment
 * @spectral_max_exp:         Indicates the max exp
 * @spectral_last_tstamp:     Indicates the last time stamp
 * @spectral_upper_max_index: Indicates the index of max mag in upper band
 * @spectral_lower_max_index: Indicates the index of max mag in lower band
 * @spectral_nb_upper:        Not Used
 * @spectral_nb_lower:        Not Used
 * @classifier_params:        Indicates classifier parameters
 * @bin_pwr_count:            Indicates the number of FFT bins
 * @lb_edge_extrabins:        Number of extra bins on left band edge
 * @rb_edge_extrabins:        Number of extra bins on right band edge
 * @bin_pwr_count_sec80:      Indicates the number of FFT bins in secondary 80
 *                            segment
 * @bin_pwr:                  Contains FFT magnitudes
 * @bin_pwr_sec80:            Contains FFT magnitudes for the secondary 80
 *                            segment
 * @interf_list:              List of interfernce sources
 * @noise_floor:              Indicates the current noise floor
 * @noise_floor_sec80:        Indicates the current noise floor for secondary 80
 *                            segment
 * @ch_width:                 Channel width 20/40/80/160 MHz
 */
struct samp_msg_data_info {
	int16_t spectral_data_len;
	int16_t spectral_data_len_sec80;
	int16_t spectral_rssi;
	int16_t spectral_rssi_sec80;
	int8_t spectral_combined_rssi;
	int8_t spectral_upper_rssi;
	int8_t spectral_lower_rssi;
	int8_t spectral_chain_ctl_rssi[MAX_SPECTRAL_CHAINS];
	int8_t spectral_chain_ext_rssi[MAX_SPECTRAL_CHAINS];
	uint8_t spectral_max_scale;
	int16_t spectral_bwinfo;
	int32_t spectral_tstamp;
	int16_t spectral_max_index;
	int16_t spectral_max_index_sec80;
	int16_t spectral_max_mag;
	int16_t spectral_max_mag_sec80;
	uint8_t spectral_max_exp;
	int32_t spectral_last_tstamp;
	int16_t spectral_upper_max_index;
	int16_t spectral_lower_max_index;
	uint8_t spectral_nb_upper;
	uint8_t spectral_nb_lower;
	struct spectral_classifier_params classifier_params;
	uint16_t bin_pwr_count;
	/*
	 * For 11ac chipsets prior to AR900B version 2.0, a max of 512 bins are
	 * delivered.  However, there can be additional bins reported for
	 * AR900B version 2.0 and QCA9984 as described next:
	 *
	 * AR900B version 2.0: An additional tone is processed on the right
	 * hand side in order to facilitate detection of radar pulses out to
	 * the extreme band-edge of the channel frequency.
	 * Since the HW design processes four tones at a time,
	 * this requires one additional Dword to be added to the
	 * search FFT report.
	 *
	 * QCA9984: When spectral_scan_rpt_mode=2, i.e 2-dword summary +
	 * 1x-oversampled bins (in-band) per FFT,
	 * then 8 more bins (4 more on left side and 4 more on right side)
	 * are added.
	 */
	uint8_t lb_edge_extrabins;
	uint8_t rb_edge_extrabins;
	uint16_t bin_pwr_count_sec80;
	uint8_t bin_pwr[MAX_NUM_BINS];
	uint8_t bin_pwr_sec80[MAX_NUM_BINS];
	struct INTERF_SRC_RSP_INFO interf_list;
	int16_t noise_floor;
	int16_t noise_floor_sec80;
	uint32_t ch_width;
}__ATTRIB_PACK;

#define SPECTRAL_DCS_INT_NONE    0
#define SPECTRAL_DCS_INT_CW      1
#define SPECTRAL_DCS_INT_WIFI    2

#define SAMP_SIGNATURE 0xdeadbeef



/**
 * struct spectral_samp_msg - Spectral SAMP message
 * @signature:          Validates the SAMP message
 * @freq:               Operating frequency in MHz
 * @vhtop_ch_freq_seg1: VHT Segment 1 centre frequency in MHz
 * @vhtop_ch_freq_seg2: VHT Segment 2 centre frequency in MHz
 * @freq_loading:       How busy was the channel
 * @dcs_enabled:        Whether DCS is enabled
 * @int_type:           Interference type indicated by DCS
 * @macaddr:            Indicates the device interface
 * @samp_data:          SAMP Data
 */
struct spectral_samp_msg_info {
	uint32_t signature;
	uint16_t freq;
	uint16_t vhtop_ch_freq_seg1;
	uint16_t vhtop_ch_freq_seg2;
	uint16_t freq_loading;
	uint16_t dcs_enabled;
	enum dcs_int_type int_type;
	uint8_t macaddr[6];
	struct samp_msg_data_info samp_data;
} __ATTRIB_PACK;

#endif /* _SPECTRAL_SCAN_FMT_H_ */
