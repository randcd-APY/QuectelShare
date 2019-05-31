/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <net/if.h>
#include <linux/genetlink.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "nl80211_copy.h"
#include "qca-vendor.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <pthread.h>
/*************************************************************************************************************************************/

enum {
	CSM_ALL,
	CSM_DBG,
	CSM_INFO,
	CSM_ERR,
};

int csm_debug_level = CSM_INFO;

void csm_printf(int level, const char *fn, const char *fmt, ...)
{
	va_list ap;
	struct timeval tv;
	struct tm *tm;

	va_start(ap, fmt);

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	if ((level >= csm_debug_level) && (NULL != tm)) {
		printf("[%2d:%2d:%2d.%6lu]", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);
		printf("[%-40s]:", fn);
		vprintf(fmt, ap);
		printf("\n");
	}

	va_end(ap);
}

#define csm_loga(fmt, ...)	csm_printf(CSM_ALL, __func__, fmt, ##__VA_ARGS__)
#define csm_logd(fmt, ...)	csm_printf(CSM_DBG, __func__, fmt, ##__VA_ARGS__)
#define csm_logi(fmt, ...)	csm_printf(CSM_INFO, __func__, fmt, ##__VA_ARGS__)
#define csm_loge(fmt, ...)	csm_printf(CSM_ERR, __func__, fmt, ##__VA_ARGS__)

/*************************************************************************************************************************************/

struct csm_config
{
	/* sliding window size for tracking number of channel switches */
	unsigned int channel_switch_window_size;
	int sample_duration;
	/* Number of consecutive samples that meet channel / band switching criteria
	   before the channel / band switch will happen */
	unsigned int consecutive_sample_count;
	/* Max number of channel switches allowed in a channel switch window */
	unsigned int max_switch_count;
	/* Flag to indicate band switching is enabled/disabled */
	int use_both_24G_and_5G_channels;
	/* List of 2.4GHz channels to be used for channel switching */
	char *channels_24g;
	/* List of 5GHz channels to be used for channel switching */
	char *channels_5g;
	/* RTS PER threshold for channel switch */
	int rts_per_threshold;
	/* PPDU PER threshold for channel switch */
	int ppdu_per_threshold;
	int mpdu_per_threshold;
	/* RSSI ACK average threshold for channel switch */
	int rssi_threshold;
	/* MCS threhold used as a trigger in band switching */
	float low_mcs_threshold;
	/** 2.4GHZ Gain **/
	int gain_24g;
};

int csm_config_copy(char **dest, char *src)
{
	*dest = malloc(strlen(src));
	if (NULL == dest) {
		csm_loge("failed to malloc");
		return -1;
	}

	memset(*dest, 0, strlen(src));
	memcpy(*dest, src, strlen(src));

	return 0;
}

#define DEFAULT_MAX_SWITCH_COUNT		2
#define DEFAULT_SAMPLE_DURATION_SECS		1
#define DEFAULT_CONSECUTIVE_SAMPLE_COUNT	5
//DEFAULT_CHANNEL_SWITCH_WINDOW >= DEFAULT_SAMPLE_DURATION_SECS * DEFAULT_CONSECUTIVE_SAMPLE_COUNT * DEFAULT_MAX_SWITCH_COUNT
#define DEFAULT_CHANNEL_SWITCH_WINDOW		30
#define DEFAULT_RTS_PER_THRESHOLD		70
#define DEFAULT_PPDU_PER_THRESHOLD		30
#define DEFAULT_MPDU_PER_THRESHOLD		30
#define DEFAULT_RSSI_THRESHOLD			-80
#define DEFAULT_CHANNELS_5G			"149 153 157 161"
#define DEFAULT_CHANNELS_24G			"1 6 11"
#define DEFAULT_LOW_MCS_THRESHOLD		1.00
#define DEFAULT_24GHZ_GAIN			7

struct csm_config *csm_config_defaults(void)
{
	struct csm_config *conf;

	conf = malloc(sizeof(*conf));
	if (!conf) {
		csm_loge("failed to malloc");
		return NULL;
	}

	memset(conf, 0, sizeof(*conf));

	conf->channel_switch_window_size = DEFAULT_CHANNEL_SWITCH_WINDOW;
	conf->sample_duration = DEFAULT_SAMPLE_DURATION_SECS;
	conf->max_switch_count = DEFAULT_MAX_SWITCH_COUNT;
	conf->consecutive_sample_count = DEFAULT_CONSECUTIVE_SAMPLE_COUNT;
	conf->use_both_24G_and_5G_channels = 0;
	csm_config_copy(&conf->channels_24g, DEFAULT_CHANNELS_24G);
	csm_config_copy(&conf->channels_5g,DEFAULT_CHANNELS_5G);
	conf->rts_per_threshold = DEFAULT_RTS_PER_THRESHOLD;
	conf->ppdu_per_threshold = DEFAULT_PPDU_PER_THRESHOLD;
	conf->mpdu_per_threshold = DEFAULT_MPDU_PER_THRESHOLD;
	conf->rssi_threshold = DEFAULT_RSSI_THRESHOLD;
	conf->low_mcs_threshold = DEFAULT_LOW_MCS_THRESHOLD;
	conf->gain_24g = DEFAULT_24GHZ_GAIN;

	return conf;
}

int csm_config_fill(struct csm_config *conf, char *buf, char *pos, int line)
{
	if (0 == strcmp(buf, "channel_switch_window_size")) {
		conf->channel_switch_window_size = atoi(pos);
	} else if (0 == strcmp(buf, "sample_duration")) {
		conf->sample_duration = atoi(pos);
	} else if (0 == strcmp(buf, "max_switch_count")) {
		conf->max_switch_count = atoi(pos);
	} else if (0 == strcmp(buf, "consecutive_sample_count")) {
		conf->consecutive_sample_count = atoi(pos);
	} else if (0 == strcmp(buf, "use_both_24G_and_5G_channels")) {
		conf->use_both_24G_and_5G_channels = atoi(pos);
	} else if (0 == strcmp(buf, "channels_24G")) {
		csm_config_copy(&conf->channels_24g, pos);
	} else if (0 == strcmp(buf, "channels_5G")) {
		csm_config_copy(&conf->channels_5g, pos);
	} else if (0 == strcmp(buf, "rts_per_threshold")) {
		conf->rts_per_threshold = atoi(pos);
	} else if (0 == strcmp(buf, "ppdu_per_threshold")) {
		conf->ppdu_per_threshold = atoi(pos);
	} else if (0 == strcmp(buf, "mpdu_per_threshold")) {
		conf->mpdu_per_threshold = atoi(pos);
	} else if (0 == strcmp(buf, "rssi_threshold")) {
		conf->rssi_threshold = atoi(pos);
	} else if (0 == strcmp(buf, "low_mcs_threshold")) {
		conf->low_mcs_threshold = atof(pos);
	} else if (0 == strcmp(buf, "gain_24g")){
		conf->gain_24g = atoi(pos);
	}

	return 0;
}

struct csm_config *csm_config_read(const char *fname)
{
	FILE *f;
	struct csm_config *conf;
	char buf[1024], *pos;
	int line = 0;
	int errors = 0;

	f = fopen(fname, "r");
	if (NULL == f) {
		csm_loge("Failed to open config file %s\n", fname);
		return NULL;
	}

	conf = csm_config_defaults();
	if (NULL == f) {
		fclose(f);
		csm_loge("Failed to set csm config by default");
		return NULL;
	}

	while (fgets(buf, sizeof(buf), f)) {
		line++;

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

		pos = strchr(buf, '=');
		if (pos == NULL) {
			csm_loge("Line %d: invalid line '%s'", line, buf);
			errors++;
			continue;
		}
		*pos = '\0';
		pos++;
		errors += csm_config_fill(conf, buf, pos, line);
	}

	fclose(f);

	return conf;
}

void csm_config_dump(struct csm_config *conf)
{
	if (!conf) {
		csm_loge("null pointer");
		return;
	}

	csm_logi("*****csm_config_dump*****START*****");
	csm_logi("channel_switch_window_size:   %d",
				conf->channel_switch_window_size);
	csm_logi("sample_duration:              %d",
				conf->sample_duration);
	csm_logi("max_switch_count:             %d",
				conf->max_switch_count);
	csm_logi("consecutive_sample_count:     %d",
				conf->consecutive_sample_count);
	csm_logi("use_both_24G_and_5G_channels: %d",
				conf->use_both_24G_and_5G_channels);
	csm_logi("channels_24g:                 %s",
				conf->channels_24g);
	csm_logi("channels_5g:                  %s",
				conf->channels_5g);
	csm_logi("rts_per_threshold:            %d",
				conf->rts_per_threshold);
	csm_logi("ppdu_per_threshold:           %d",
				conf->ppdu_per_threshold);
	csm_logi("mpdu_per_threshold:           %d",
				conf->mpdu_per_threshold);
	csm_logi("rssi_threshold:               %d",
				conf->rssi_threshold);
	csm_logi("low_mcs_threshold:            %0.2f",
				conf->low_mcs_threshold);
	csm_logi("gain_24g:                     %d",
				conf->gain_24g);
	csm_logi("-----csm_config_dump-----END-------");
}

/*************************************************************************************************************************************/

struct csm_nl_80211_data {
	struct nl_sock *cmd_sock;
	struct genl_family *family_ptr;
	struct nl_cache *cache;
	int id;
	struct nl_cb *cmd_cb;
};

#define MAX_MCS_RATES 8

struct csm_ll_stats {
	unsigned int cur_channel;
	// Count of successful MPDU transmissions
	unsigned int mpdu_succ_cnt;
	// Count of failed MPDUs
	unsigned int mpdu_fail_cnt;
	// count of successful rts transmissions
	unsigned int rts_succ_cnt;
	// total number of rts failures
	unsigned int rts_fail_cnt;
	// number of successful ppdu tx
	unsigned int ppdu_succ_cnt;
	// ppdu failure count
	unsigned int ppdu_fail_cnt;
	//Channel busy time
	unsigned int cca_busy_time;
	unsigned int on_time;
	unsigned int rssi_ack_avg;
	unsigned int contention_time_avg;
	unsigned int mcs_rate_stats[MAX_MCS_RATES];
};

enum band
{
	BAND_24G = 0,
	BAND_5G,
};


struct chan_list
{
	unsigned int num_chans;
	unsigned int *chans;
};

struct csm_chan_data
{
	struct chan_list *chan_list_2g;
	struct chan_list *chan_list_5g;
	struct chan_list *cur_op_chan_list;
	unsigned int cur_op_chan_idx;
	enum band cur_op_band;
};

struct csm_chan_sw_win_data
{
	unsigned int chan_sw_win_size;
	unsigned int chan_sw_win_index;
	unsigned int  *chan_sw_win;

	// Count of consecutive times the channel switch criteria have been met
	unsigned int consecutive_chan_sw_count;

	// Count of consecutive times the band switch criteria have been met
	unsigned int consecutive_band_sw_count;
};

//golbal data structure
struct csm_data {
	//csm config file
	struct csm_config *conf;

	//interface msg
	char *iface_name;
	int iface_idx;

	//nl802111
	void *nl_80211;

	//ll_stats
	struct csm_ll_stats *ll_stats;
	struct csm_ll_stats *prev_ll_stats;

	//channel list
	struct csm_chan_data *chan_data;

	//channel switch window
	struct csm_chan_sw_win_data *chan_sw_win_data;

	pthread_t csm_monitor_thread;
	pthread_t csm_process_thread;
	pthread_mutex_t csm_stats_mutex;
	pthread_cond_t csm_stats_cv;

	int csm_process_pause;
	int csm_process_init;
	int csm_process_clean;
	int csm_process_connected;
};

static int csm_char_ch(char *buf, char ch)
{
        unsigned int match = 0;

        while(*buf)
        {
                if(ch == *buf++)
                        match++;
        }

        csm_logd("match: %d", match);
        return match;
}

static int csm_chan_list_parse(struct chan_list *chan_list, char *chan_list_str)
{
	char *str, *res;
	unsigned int i = 0;
	//duplicate chan_list_str when parsing in case split
	char *chan_list_str_dup;
	int chan_list_str_len;

	chan_list_str_len = strlen(chan_list_str);
	chan_list_str_dup = malloc(chan_list_str_len + 1);
	if(!chan_list_str_dup) {
		csm_loge("failed to malloc");
		return -1;
	}
	memcpy(chan_list_str_dup, chan_list_str, chan_list_str_len);
	chan_list_str_dup[chan_list_str_len] = '\0';

	chan_list->num_chans = csm_char_ch(chan_list_str_dup, ' ') + 1;

	chan_list->chans = malloc(sizeof(int)*(chan_list->num_chans));
	if (!chan_list->chans) {
		csm_loge("failed to malloc");
		return -1;
	}

	res = chan_list_str_dup;
	while ((str = strtok_r(res, " ", &res))) {
		csm_logd("channel num: %s", str);
		chan_list->chans[i++] = atoi(str);
	}

	free(chan_list_str_dup);

	return 0;
}

static int csm_chan_list_dump(struct chan_list *chan_list)
{
	unsigned int i;

	csm_logd("*****csm_chan_list_dump*****");
	csm_logd("num_chans:   %d", chan_list->num_chans);
	for (i = 0; i < chan_list->num_chans; i++)
		csm_logd("channel num: %d", chan_list->chans[i]);
	csm_logd("-----csm_chan_list_dump-----");

	return 0;
}

static int csm_chan_data_dump(struct csm_chan_data *chan_data)
{
	csm_logd("*****csm_chan_data_dump*****START*****");

	csm_logd("channel list 2g:");
	csm_chan_list_dump(chan_data->chan_list_2g);

	csm_logd("channel list 5g");
	csm_chan_list_dump(chan_data->chan_list_5g);


	csm_logd("cur_op_chan_idx: %d", chan_data->cur_op_chan_idx);
	csm_logd("cur_op_band:     %d", chan_data->cur_op_band);

	csm_logd("-----csm_chan_data_dump-----END-------");

	return 0;
}

struct freq_chan_info
{
	unsigned int freq;
	unsigned int chan;
};

static const struct freq_chan_info freq_chan_map[] = { {2412, 1}, {2417, 2},
	{2422, 3}, {2427, 4}, {2432, 5}, {2437, 6}, {2442, 7}, {2447, 8},
	{2452, 9}, {2457, 10}, {2462, 11}, {2467 ,12}, {2472, 13},
	{2484, 14}, {5180, 36}, {5200, 40}, {5220, 44},
	{5240, 48}, {5260, 52}, {5280, 56}, {5300, 60}, {5320, 64}, {5500, 100},
	{5520, 104}, {5540, 108}, {5560, 112}, {5580, 116}, {5600, 120},
	{5620, 124}, {5640, 128}, {5660, 132}, {5680, 136}, {5700, 140},
	{5720, 144}, {5745, 149}, {5765, 153}, {5785, 157}, {5805, 161},
	{5825, 165}, {5852, 170}, {5855, 171}, {5860, 172}, {5865, 173},
	{5870, 174}, {5875, 175}, {5880, 176}, {5885, 177}, {5890, 178},
	{5895, 179}, {5900, 180}, {5905, 181}, {5910, 182}, {5915, 183},
	{5920, 184} };

#define FREQ_CHAN_MAP_TABLE_SIZE (sizeof(freq_chan_map)/sizeof(freq_chan_map[0]))

static unsigned int csm_get_chan(unsigned int freq)
{
	int i;

	if (freq) {
		for(i = 0; i < FREQ_CHAN_MAP_TABLE_SIZE; i++) {
			if (freq == freq_chan_map[i].freq)
				return freq_chan_map[i].chan;
		}
	}

	return 0;
}

static int csm_get_chan_list_index(unsigned int chan,
	struct chan_list *chan_list)
{
	int i;

	if (chan) {
		for (i = 0; i < chan_list->num_chans; i++) {
			if (chan == chan_list->chans[i])
				return i;
		}
	}

	return -1;
}

int csm_chan_data_init(struct csm_data *csm_data)
{
	struct csm_chan_data *chan_data;
	struct csm_config *conf = csm_data->conf;
	struct csm_ll_stats *ll_stats = csm_data->ll_stats;

	csm_data->chan_data = malloc(sizeof(struct csm_chan_data));
	if (!csm_data->chan_data) {
		csm_loge("failed to malloc");
		return -1;
	}

	chan_data = csm_data->chan_data;

	//init channel list 2G
	chan_data->chan_list_2g = malloc(sizeof(struct chan_list));
	if (!chan_data->chan_list_2g)
	{
		csm_loge("failed to malloc");
		return -1;
	}
	csm_chan_list_parse(chan_data->chan_list_2g, conf->channels_24g);

	//init channel list 5G
	chan_data->chan_list_5g = malloc(sizeof(struct chan_list));
	if (!chan_data->chan_list_5g)
	{
		csm_loge("failed to malloc");
		return -1;
	}
	csm_chan_list_parse(chan_data->chan_list_5g, conf->channels_5g);

	if (ll_stats->cur_channel > 5000) {
		chan_data->cur_op_chan_list = chan_data->chan_list_5g;
		chan_data->cur_op_band = BAND_5G;
	} else {
		chan_data->cur_op_chan_list = chan_data->chan_list_2g;
		chan_data->cur_op_band = BAND_24G;
	}

	chan_data->cur_op_chan_idx = csm_get_chan_list_index(
				csm_get_chan(ll_stats->cur_channel),
				chan_data->cur_op_chan_list);

	csm_chan_data_dump(chan_data);

	return 0;
}

int csm_chan_data_deinit(struct csm_data *csm_data)
{
	if (csm_data->chan_data) {
		if (csm_data->chan_data->chan_list_2g)
			free(csm_data->chan_data->chan_list_2g);

		if (csm_data->chan_data->chan_list_5g)
			free(csm_data->chan_data->chan_list_5g);

		free(csm_data->chan_data);
		csm_data->chan_data = NULL;
	}

	return 0;
}

static int csm_chan_sw_win_data_dump(
	struct csm_chan_sw_win_data *chan_sw_win_data)
{
	int i = 0;

	csm_logd("*****csm_chan_sw_win_data_dump*****START*****");
	csm_logd("chan_sw_win_size:                 %d",
			chan_sw_win_data->chan_sw_win_size);
	csm_logd("chan_sw_win_index:                %d",
			chan_sw_win_data->chan_sw_win_index);
	for (i = 0; i < chan_sw_win_data->chan_sw_win_size; i++)
	csm_logd("chan_sw_win[%d]:                  %d",
			i, chan_sw_win_data->chan_sw_win[i]);
	csm_logd("consecutive_chan_sw_count:        %d",
			chan_sw_win_data->consecutive_chan_sw_count);
	csm_logd("consecutive_band_sw_count:        %d",
			chan_sw_win_data->consecutive_band_sw_count);
	csm_logd("-----csm_chan_sw_win_data_dump-----END-------");

	return 0;
}

int csm_chan_sw_win_data_init(struct csm_data *csm_data)
{
	struct csm_chan_sw_win_data *chan_sw_win_data;
	struct csm_config *conf = csm_data->conf;

	csm_data->chan_sw_win_data = malloc(sizeof(struct csm_chan_sw_win_data));
	if(!csm_data->chan_sw_win_data) {
		csm_loge("failed to malloc");
		return -1;
	}

	chan_sw_win_data = csm_data->chan_sw_win_data;

	chan_sw_win_data->chan_sw_win_size = (conf->channel_switch_window_size) /
						(conf->sample_duration);

	if ((conf->channel_switch_window_size) % (conf->sample_duration)) {
		chan_sw_win_data->chan_sw_win_size++;
	}

	chan_sw_win_data->chan_sw_win =
		malloc(sizeof(int)*(chan_sw_win_data->chan_sw_win_size));
	if (!chan_sw_win_data->chan_sw_win) {
		csm_loge("failed to malloc");
		return -1;
	}
	memset(chan_sw_win_data->chan_sw_win, 0,
		sizeof(int)*(chan_sw_win_data->chan_sw_win_size));

	chan_sw_win_data->chan_sw_win_index = 0;

	chan_sw_win_data->consecutive_chan_sw_count = 0;
	chan_sw_win_data->consecutive_band_sw_count = 0;

	csm_chan_sw_win_data_dump(chan_sw_win_data);

	return 0;
}

int csm_chan_sw_win_data_deinit(struct csm_data *csm_data)
{
	if (csm_data->chan_sw_win_data) {
		if (csm_data->chan_sw_win_data->chan_sw_win)
			free(csm_data->chan_sw_win_data->chan_sw_win);

		free(csm_data->chan_sw_win_data);
		csm_data->chan_sw_win_data = NULL;
	}

	return 0;
}

int csm_chan_sw_win_data_reset(struct csm_data *csm_data)
{
	struct csm_chan_sw_win_data *chan_sw_win_data;

	chan_sw_win_data = csm_data->chan_sw_win_data;
        memset(chan_sw_win_data->chan_sw_win, 0,
                sizeof(int)*(chan_sw_win_data->chan_sw_win_size));

        chan_sw_win_data->chan_sw_win_index = 0;

	csm_chan_sw_win_data_dump(chan_sw_win_data);

	return 0;
}

int csm_chan_sw_win_check_chan(struct csm_data *csm_data)
{
	struct csm_config *conf = csm_data->conf;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	csm_logd("consecutive_chan_sw_count >= conf->consecutive_sample_count : %s",
		(chan_sw_win_data->consecutive_chan_sw_count >= conf->consecutive_sample_count) ? "Meet" : "Not Meet");
	if (chan_sw_win_data->consecutive_chan_sw_count >=
			conf->consecutive_sample_count)
		return 0;
	else
		return -1;
}

int csm_chan_sw_win_check_band(struct csm_data *csm_data)
{
	struct csm_config *conf = csm_data->conf;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	csm_logd("consecutive_band_sw_count >= conf->consecutive_sample_count : %s",
		(chan_sw_win_data->consecutive_band_sw_count >= conf->consecutive_sample_count) ? "Meet" : "Not Meet");
	if (chan_sw_win_data->consecutive_band_sw_count >=
			conf->consecutive_sample_count)
		return 0;
	else
		return -1;
}

int csm_chan_sw_win_check_sw(struct csm_data *csm_data)
{
	struct csm_config *conf = csm_data->conf;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;
	int i, sum = 0;

	for(i = 0; i < chan_sw_win_data->chan_sw_win_size; i++) {
		sum += chan_sw_win_data->chan_sw_win[i];
	}

	csm_logd("switch count in period: %d", sum);
	if (sum < conf->max_switch_count) {
		csm_logd("Max Switch Count is NOT hit");
		return 0;
	}
	csm_logd("Max Switch Count is HIT now");
	return -1;

}

int csm_chan_sw_win_update(struct csm_data *csm_data, bool update)
{
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	chan_sw_win_data->chan_sw_win[chan_sw_win_data->chan_sw_win_index] = update;

	chan_sw_win_data->chan_sw_win_index =
			(chan_sw_win_data->chan_sw_win_index + 1) %
			(chan_sw_win_data->chan_sw_win_size);

	return 0;
}

int csm_nl_80211_init(struct csm_data *csm_data)
{
	int ret = 0;
        struct csm_nl_80211_data *nl_80211;

        if (!csm_data)
                return -1;

        csm_data->nl_80211 = (struct csm_nl_80211_data *)
                         malloc(sizeof(struct csm_nl_80211_data));
        if (!csm_data->nl_80211)
                return -1;

        nl_80211 = csm_data->nl_80211;
        nl_80211->cmd_cb = nl_cb_alloc(NL_CB_DEFAULT);
        if (!nl_80211->cmd_cb) {
                csm_loge("nl_cb_alloc() fail");
                goto fail;
        }

        nl_80211->cmd_sock = nl_socket_alloc_cb(nl_80211->cmd_cb);
        if (!nl_80211->cmd_sock) {
                csm_loge("nl_socket_alloc_cb() fail");
                goto fail;
        }

        if (genl_connect(nl_80211->cmd_sock)) {
                csm_loge("genl_connect() fail");
                ret = -ENOLINK;
                goto fail;
        }

        if (genl_ctrl_alloc_cache(nl_80211->cmd_sock, &(nl_80211->cache))) {
                csm_loge("genl_ctrl_alloc_cache() fail");
                goto fail;
        }

        nl_80211->family_ptr =
                        genl_ctrl_search_by_name(nl_80211->cache,"nl80211");
        if(!nl_80211->family_ptr) {
                csm_loge("genl_ctrl_search_by_name() fail");
                ret= -ENOENT;
                goto fail;
        }
        nl_80211->id = genl_ctrl_resolve(nl_80211->cmd_sock, "nl80211");
        if (nl_80211->id < 0) {
                csm_loge("genl_ctrl_resolve() fail");
                ret = -ENOENT;
                goto fail;
        }

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

int csm_nl_80211_deinit(struct csm_data *csm_data)
{
	struct csm_nl_80211_data *nl_80211 =
		(struct csm_nl_80211_data *)(csm_data->nl_80211);

        if (nl_80211->cmd_sock)
                nl_socket_free(nl_80211->cmd_sock);
        nl_cb_put(nl_80211->cmd_cb);
        if (nl_80211->cache)
                nl_cache_free(nl_80211->cache);
        if (nl_80211->id && nl_80211->family_ptr)
                genl_family_put(nl_80211->family_ptr);
        if (nl_80211)
                free(nl_80211);

	return 0;
}

static int csm_nl_80211_error_handler(struct sockaddr_nl *nla,
		struct nlmsgerr *error, void *cb_data)
{
        int *ret_val = (int *)cb_data;
        *ret_val = error->error;
        return NL_STOP;
}

/*static int csm_nl_80211_seq_check(struct nl_msg *msg, void *cb_data)
{
        return NL_OK;
}*/

static int csm_nl_80211_finish_handler(struct nl_msg *msg, void *cb_data)
{
        int *ret_val = (int *)cb_data;
        *ret_val = 0;
        return NL_SKIP;
}

static int csm_nl_80211_ack_handler(struct nl_msg *msg, void *cb_data)
{
        int *ret_val = (int *)cb_data;
        *ret_val = 0;
        return NL_STOP;
}

static int csm_nl_80211_cmd_init(struct nl_msg **msg, struct nl_cb **cb,
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

        nl_cb_err(*cb, NL_CB_CUSTOM, csm_nl_80211_error_handler, *cb_err);
        nl_cb_set(*cb, NL_CB_FINISH, NL_CB_CUSTOM, csm_nl_80211_finish_handler, *cb_err);
        nl_cb_set(*cb, NL_CB_ACK, NL_CB_CUSTOM, csm_nl_80211_ack_handler, *cb_err);
        if (cmd_handler)
                nl_cb_set(*cb, NL_CB_VALID, NL_CB_CUSTOM, cmd_handler, cb_data);

        return 0;
}

static void csm_nl_80211_cmd_deinit(struct nl_msg *msg, struct nl_cb *cb,
		int *cb_err)
{
	if (cb)
		nl_cb_put(cb);
	if (msg)
		nlmsg_free(msg);
	if (cb_err)
		free(cb_err);
}

static int csm_nl_80211_cmd_run(struct nl_sock *sock, struct nl_msg *msg,
		struct nl_cb *cb, int *cb_err)
{
        int ret = 0;

        if (msg == NULL || cb == NULL || cb_err == NULL)
                return -1;

        *cb_err = 1;
        if (nl_send_auto_complete(sock, msg) < 0) {
		csm_logd("%s nl_send_auto_complete failed", __func__);
                return -1;
	}

        while (*cb_err == 1)
                nl_recvmsgs(sock, cb);

        if (*cb_err == 0)
                ret = 0;
        else {
                ret = *cb_err;
		csm_logd("%s cb_err: %d", *cb_err);
	}

        return ret;
}

struct req_set_ll_stats
{
	unsigned int mpdu_size_threshold;
	unsigned int aggressive_stats_gathering;
};

int csm_nl_80211_set_ll_stats(struct csm_data *csm_data,
		struct req_set_ll_stats *req)
{
	struct csm_nl_80211_data *nl_80211;
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	int ret = 0, *cb_err;
	struct nlattr *data;

	if(!csm_data)
		return -1;

	nl_80211 = csm_data->nl_80211;

	if (csm_nl_80211_cmd_init(&msg, &cb, NULL, NULL, &cb_err)) {
		csm_loge("cmd msg alloc fail");
		return -1;
	}

	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);

	nla_put_u32(msg, NL80211_ATTR_IFINDEX,
					csm_data->iface_idx);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID,
					OUI_QCA);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
					QCA_NL80211_VENDOR_SUBCMD_LL_STATS_SET);

	if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA)) ||
		nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_SET_CONFIG_MPDU_SIZE_THRESHOLD,
					req->mpdu_size_threshold) ||
		nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_SET_CONFIG_AGGRESSIVE_STATS_GATHERING,
					req->aggressive_stats_gathering)) {
		ret = -ENOBUFS;
		goto err;
	}
	nla_nest_end(msg, data);

	ret = csm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err);

err:
	csm_logd("%s to SEND set ll stats CMD", ret ? "Failed" : "Succeed");
	csm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

int csm_set_ll_stats(struct csm_data *csm_data)
{
	struct req_set_ll_stats req = { /* mpdu_size_threshold        */ 128,
					/* aggressive_stats_gathering */ 1};

	return csm_nl_80211_set_ll_stats(csm_data, &req);
}

struct req_get_ll_stats
{
	unsigned int req_id;
	unsigned int req_mask;
};


void csm_ll_stats_dump(struct csm_ll_stats *stats)
{
	int i;

	if (NULL == stats) {
		csm_logd("NULL Pointer");
		return;
	}

	csm_logd("*****csm_ll_stats_dump*****START*****");
	csm_logd("cur_channel:         %d", stats->cur_channel);
	csm_logd("mpdu_succ_cnt:       %d", stats->mpdu_succ_cnt);
	csm_logd("mpdu_fail_cnt:       %d", stats->mpdu_fail_cnt);
	csm_logd("rts_succ_cnt:        %d", stats->rts_succ_cnt);
	csm_logd("rts_fail_cnt:        %d", stats->rts_fail_cnt);
	csm_logd("ppdu_succ_cnt:       %d", stats->ppdu_succ_cnt);
	csm_logd("ppdu_fail_cnt:       %d", stats->ppdu_fail_cnt);
	csm_logd("cca_busy_time:       %d", stats->cca_busy_time);
	csm_logd("on_time:             %d", stats->on_time);
	csm_logd("rssi_ack_avg:        %d", stats->rssi_ack_avg);
	csm_logd("contention_time_avg: %d", stats->contention_time_avg);
	for(i = 0; i < MAX_MCS_RATES; i++) {
	csm_logd("mcs_rate_stats[%d]:  %d", i, stats->mcs_rate_stats[i]);
	}
	csm_logd("-----csm_ll_stats_dump-----END-------");
}

struct rsp_ll_stats_radio
{
	int is_valid;

	unsigned int cur_channel;
	unsigned int cca_busy_time;
	unsigned int on_time;
};

struct rsp_ll_stats_iface
{
	int is_valid;

	unsigned int mpdu_succ_cnt;
	unsigned int mpdu_fail_cnt;
	unsigned int rts_succ_cnt;
	unsigned int rts_fail_cnt;
	unsigned int ppdu_succ_cnt;
	unsigned int ppdu_fail_cnt;
	unsigned int rssi_ack_avg;
	unsigned int contention_time_avg;
	unsigned int num_peers;
};

struct rsp_ll_stats_peer
{
	int is_valid;

	unsigned int mcs_rate_stats[MAX_MCS_RATES];
};

union rsp_get_ll_stats
{
	struct rsp_ll_stats_radio ll_stats_radio;
	struct rsp_ll_stats_iface ll_stats_iface;
	struct rsp_ll_stats_peer ll_stats_peer;
};

int csm_nl_80211_get_ll_stats_handler(struct nl_msg *msg, void *cb_data)
{
	union rsp_get_ll_stats *rsp = cb_data;
        struct nlattr *nl_attr_list[NL80211_ATTR_MAX + 1];
        struct genlmsghdr *gen_nlh = nlmsg_data(nlmsg_hdr(msg));
        struct nlattr *nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX+1];

        if (!rsp) {
                csm_loge("Invalid data ptr");
                goto end;
        }

        nla_parse(nl_attr_list, NL80211_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
                        genlmsg_attrlen(gen_nlh, 0), NULL);

        if (!nl_attr_list[NL80211_ATTR_VENDOR_DATA]) {
                csm_loge("No vendor data");
                goto end;
        } else {
                nla_parse(nl_vendor_data, QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX,
                        nla_data(nl_attr_list[NL80211_ATTR_VENDOR_DATA]),
                        nla_len(nl_attr_list[NL80211_ATTR_VENDOR_DATA]),
                        NULL);
        }

	if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_TYPE]) {
		csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_TYPE FOUND!!!");
		goto end;
	}

	switch (nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_TYPE])) {
		case QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_RADIO:
		{
			unsigned int num_channels;
			struct nlattr *ch_info;
			int rem;
			int channel = 0, on_time = 0, cca_busy_time = 0;

			csm_loga("*****QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_RADIO report*****");

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_RADIO_NUM_CHANNELS]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_RADIO_NUM_CHANNELS FOUND!!!");
				goto end;
			}
			num_channels = nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_RADIO_NUM_CHANNELS]);
			csm_loga("num_channels: %d", num_channels);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CH_INFO]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_CH_INFO FOUND!!!");
				goto end;
			}

			for (ch_info = (struct nlattr *) nla_data(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CH_INFO]),
				rem = nla_len(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CH_INFO]);
				nla_ok(ch_info, rem); ch_info = nla_next(ch_info, &(rem))) {
				struct nlattr *tb[ QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX+ 1];

				nla_parse(tb, QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX, (struct nlattr *) nla_data(ch_info), nla_len(ch_info), NULL);
				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_INFO_CENTER_FREQ]) {
					csm_logd("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_INFO_CENTER_FREQ FOUND!!!");
					goto end;
				}
				channel = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_INFO_CENTER_FREQ]);
				csm_loga("channel :%d", channel);

				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_ON_TIME]) {
					csm_logd("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_ON_TIME FOUND!!!");
					goto end;
				}
				on_time = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_ON_TIME]);
				csm_loga("on_time :%d", on_time);

				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_CCA_BUSY_TIME]) {
					csm_logd("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_CCA_BUSY_TIME FOUND!!!");
					goto end;
				}
				cca_busy_time = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_CHANNEL_CCA_BUSY_TIME]);
				csm_loga("cca_busy_time :%d", cca_busy_time);
			}

			rsp->ll_stats_radio.cur_channel = channel;
			rsp->ll_stats_radio.on_time = on_time;
			rsp->ll_stats_radio.cca_busy_time = cca_busy_time;

			rsp->ll_stats_radio.is_valid = 1;

			csm_loga("-----QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_RADIO report-----");
		}
		break;

		case QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_IFACE:
		{
			int max_contention_avg_time = 0;
			int mpdu_fail_cnt = 0;
			int mpdu_succ_cnt = 0;

			struct nlattr *wmm_info;
			int rem;

			csm_loga("*****QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_IFACE report*****");

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_INFO]) {
                                csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_INFO FOUND!!!");
                                goto end;
                        }

			for (wmm_info = (struct nlattr *) nla_data(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_INFO]),
				rem = nla_len(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_INFO]);
				nla_ok(wmm_info, rem); wmm_info = nla_next(wmm_info, &(rem))) {

				struct nlattr *tb[ QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX+ 1];

				int contention_avg_time;
				int mpdu_lost;
				int tx_mpdu;

				csm_loga("*****Parsing wmm info*****START*****");
                                nla_parse(tb, QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX, (struct nlattr *) nla_data(wmm_info), nla_len(wmm_info), NULL);

				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_CONTENTION_TIME_AVG]) {
					csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_CONTENTION_TIME_AVG FOUND!!!");
					goto end;
				}
				contention_avg_time = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_CONTENTION_TIME_AVG]);
				csm_loga("contention_avg_time: %d", contention_avg_time);
				if (contention_avg_time > max_contention_avg_time) {
					max_contention_avg_time = contention_avg_time;
				}

				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_MPDU_LOST]) {
                                        csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_MPDU_LOST FOUND!!!");
                                        goto end;
                                }
				mpdu_lost = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_MPDU_LOST]);
				csm_loga("mpdu_lost: %d", mpdu_lost);
				mpdu_fail_cnt += mpdu_lost;

				if (!tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_TX_MPDU]) {
                                        csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_TX_MPDU FOUND!!!");
                                        goto end;
                                }
                                tx_mpdu = nla_get_u32(tb[QCA_WLAN_VENDOR_ATTR_LL_STATS_WMM_AC_TX_MPDU]);
                                csm_loga("tx_mpdu: %d", tx_mpdu);
                                mpdu_succ_cnt += tx_mpdu;
				csm_loga("-----Parsing wmm info-----END-------");
			}

			rsp->ll_stats_iface.contention_time_avg = max_contention_avg_time;
			rsp->ll_stats_iface.mpdu_fail_cnt = mpdu_fail_cnt;
			rsp->ll_stats_iface.mpdu_succ_cnt = mpdu_succ_cnt;

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_SUCC_CNT]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_SUCC_CNT FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.rts_succ_cnt =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_SUCC_CNT]);
			csm_loga("rts_succ_cnt: %d", rsp->ll_stats_iface.rts_succ_cnt);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_FAIL_CNT]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_FAIL_CNT FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.rts_fail_cnt =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RTS_FAIL_CNT]);
			csm_loga("rts_fail_cnt: %d", rsp->ll_stats_iface.rts_fail_cnt);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_SUCC_CNT]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_SUCC_CNT FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.ppdu_succ_cnt =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_SUCC_CNT]);
			csm_loga("ppdu_succ_cnt: %d", rsp->ll_stats_iface.ppdu_succ_cnt);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_FAIL_CNT]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_FAIL_CNT FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.ppdu_fail_cnt =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_PPDU_FAIL_CNT]);
			csm_loga("ppdu_fail_cnt: %d", rsp->ll_stats_iface.ppdu_fail_cnt);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RSSI_ACK]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RSSI_ACK FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.rssi_ack_avg =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_RSSI_ACK]);
			csm_loga("rssi_ack_avg: %d", rsp->ll_stats_iface.rssi_ack_avg);

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_NUM_PEERS]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_NUM_PEERS FOUND!!!");
				goto end;
			}
			rsp->ll_stats_iface.num_peers =
					nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_IFACE_NUM_PEERS]);
			csm_loga("num_peers: %d", rsp->ll_stats_iface.num_peers);

			rsp->ll_stats_iface.is_valid = 1;

			csm_loga("-----QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_IFACE report-----");
		}
		break;

		case QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_PEERS:
		{
			struct nlattr *peer_info, *rate_info;
			int rem;
			int rate_mcs_idx, rate_tx_mpdu;

			csm_loga("*****Parsing QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_PEERS*****START*****");

			if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO]) {
				csm_loge("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO FOUND!!!");
				goto end;
			}

			for (peer_info = (struct nlattr *) nla_data(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO]),
				rem = nla_len(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO]);
				nla_ok(peer_info, rem); peer_info = nla_next(peer_info, &(rem))) {

				struct nlattr *tb1[QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX+1];

				csm_loga("*****Parsing peer info*****START*****");

				nla_parse(tb1, QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX, (struct nlattr *)nla_data(peer_info), nla_len(peer_info), NULL);

				if (!tb1[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO_RATE_INFO]) {
					csm_loga("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO_RATE_INFO FOUND!!!");
					goto end;
				}

				for (rate_info = (struct nlattr *) nla_data(tb1[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO_RATE_INFO]),
					rem = nla_len(tb1[QCA_WLAN_VENDOR_ATTR_LL_STATS_PEER_INFO_RATE_INFO]);
					nla_ok(rate_info, rem); rate_info = nla_next(rate_info, &(rem))) {

						struct nlattr *tb2[QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX+1];

						csm_loga("     *****Parsing rate info*****START*****");

						nla_parse(tb2, QCA_WLAN_VENDOR_ATTR_LL_STATS_MAX, (struct nlattr *)nla_data(rate_info), nla_len(rate_info), NULL);
						if (!tb2[QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_MCS_INDEX]) {
							csm_loga("!!!NO QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_MCS_INDEX FOUND!!!");
							goto end;
						}
						rate_mcs_idx = nla_get_u8(tb2[QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_MCS_INDEX]);
						if ((rate_mcs_idx < 0) || (rate_mcs_idx >= MAX_MCS_RATES)) {
							csm_loga("rate mac idx is illegal");
							goto end;
						}
						csm_loga("     rate_mcs_idx: %d", rate_mcs_idx);

						if (!tb2[QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_TX_MPDU]) {
							csm_loge("!!!QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_TX_MPDU FOUND!!!");
							goto end;
						}
						rate_tx_mpdu = nla_get_u32(tb2[QCA_WLAN_VENDOR_ATTR_LL_STATS_RATE_TX_MPDU]);
						csm_loga("     rate_tx_mpdu: %d", rate_tx_mpdu);

						rsp->ll_stats_peer.mcs_rate_stats[rate_mcs_idx] += rate_tx_mpdu;

						csm_loga("     -----Parsing rate info-----END-------");
					}

				csm_loga("-----Parsing peer info-----END-------");
			}

			rsp->ll_stats_peer.is_valid = 1;

			csm_loga("-----Parsing QCA_NL80211_VENDOR_SUBCMD_LL_STATS_TYPE_PEERS-----END-------");
		}
		break;

		default:
			csm_loge("Wrong LLStats subcmd received");
		break;
	}

end:
	return NL_SKIP;
}
;

int csm_nl_80211_get_ll_stats(struct csm_data *csm_data,
		struct req_get_ll_stats *req,
		union rsp_get_ll_stats *rsp)
{
	struct csm_nl_80211_data *nl_80211;
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	int ret = 0, *cb_err;
	struct nlattr *data;

	if(!csm_data)
		return -1;

	nl_80211 = csm_data->nl_80211;

	if (csm_nl_80211_cmd_init(&msg, &cb, csm_nl_80211_get_ll_stats_handler,
					rsp, &cb_err)) {
		csm_loge("cmd msg alloc fail");
		return -1;
	}

	genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);

	nla_put_u32(msg, NL80211_ATTR_IFINDEX,
					csm_data->iface_idx);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_ID,
					OUI_QCA);
	nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
					QCA_NL80211_VENDOR_SUBCMD_LL_STATS_GET);

	if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA)) ||
		nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_GET_CONFIG_REQ_ID,
					req->req_id) ||
		nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_GET_CONFIG_REQ_MASK,
					req->req_mask)) {
		csm_loge("put attr failed");
		ret = -ENOBUFS;
		goto err;
	}
	nla_nest_end(msg, data);

	ret = csm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err);

err:
	csm_logd("%s to SEND get ll stats CMD", ret ? "Failed" : "Succeed");
	csm_nl_80211_cmd_deinit(msg, cb, cb_err);
	return ret;
}

struct csm_ll_stats *csm_get_ll_stats(struct csm_data *csm_data)
{
	struct req_get_ll_stats req;
	union rsp_get_ll_stats rsp;
	struct csm_ll_stats *stats;

	stats = malloc(sizeof(struct csm_ll_stats));
	if (NULL == stats) {
		csm_loge("Failed to malloc csm_ll_stats");
		goto end;
	}
	memset(stats, 0, sizeof(*stats));

	//request ll stats radio
	req.req_id = 1;
	req.req_mask = 1;
	memset(&rsp, 0, sizeof(rsp));
	rsp.ll_stats_radio.is_valid = 0;
	csm_nl_80211_get_ll_stats(csm_data, &req, &rsp);
	if (rsp.ll_stats_radio.is_valid) {

		stats->cur_channel = rsp.ll_stats_radio.cur_channel;
		stats->cca_busy_time = rsp.ll_stats_radio.cca_busy_time;
		stats->on_time = rsp.ll_stats_radio.on_time;

	} else {
		csm_logd("!!!ll stats radio is NOT valid!!!");
		goto end;
	}

	//request ll stats iface
	req.req_id = 1;
	req.req_mask = 2;
	memset(&rsp, 0, sizeof(rsp));
	rsp.ll_stats_iface.is_valid = 0;
	csm_nl_80211_get_ll_stats(csm_data, &req, &rsp);
	if (rsp.ll_stats_iface.is_valid) {

		stats->mpdu_succ_cnt = rsp.ll_stats_iface.mpdu_succ_cnt;
		stats->mpdu_fail_cnt = rsp.ll_stats_iface.mpdu_fail_cnt;
		stats->rts_succ_cnt = rsp.ll_stats_iface.rts_succ_cnt;
		stats->rts_fail_cnt = rsp.ll_stats_iface.rts_fail_cnt;
		stats->ppdu_succ_cnt = rsp.ll_stats_iface.ppdu_succ_cnt;
		stats->ppdu_fail_cnt = rsp.ll_stats_iface.ppdu_fail_cnt;
		stats->rssi_ack_avg =  rsp.ll_stats_iface.rssi_ack_avg;
		stats->contention_time_avg =  rsp.ll_stats_iface.contention_time_avg;

	} else {
		csm_logd("!!!ll stats iface is NOT valid!!!");
		goto end;
	}

	//request ll stats peers if peers exist
	if ( rsp.ll_stats_iface.num_peers) {
		req.req_id = 1;
		req.req_mask = 4;
		memset(&rsp, 0, sizeof(rsp));
		rsp.ll_stats_peer.is_valid = 0;
		csm_nl_80211_get_ll_stats(csm_data, &req, &rsp);
		if (rsp.ll_stats_peer.is_valid) {

			memcpy(stats->mcs_rate_stats, rsp.ll_stats_peer.mcs_rate_stats,
					sizeof(rsp.ll_stats_peer.mcs_rate_stats));

		} else {
			csm_logd("!!!ll stats peers is NOT valid!!!");
			goto end;
		}
	} else {
		csm_logd("!!!No peer found!!!");
		goto end;
	}

	return stats;

end:
	if (stats) {
		free(stats);
		stats = NULL;
	}
	return stats;
}

struct req_clr_ll_stats
{
	unsigned int req_mask;
	unsigned char stop_req;
};

struct rsp_clr_ll_stats
{
	unsigned int rsp_mask;
	unsigned int stop_rsp;
};

int csm_nl_80211_clr_ll_stats_handler(struct nl_msg *msg, void *cb_data)
{
	struct rsp_clr_ll_stats *rsp = cb_data;
	struct nlattr *nl_attr_list[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gen_nlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *nl_vendor_data[QCA_WLAN_VENDOR_ATTR_MAX+1];

	if (!rsp) {
		csm_loge("Invalid data ptr");
		goto end;
	}

	nla_parse(nl_attr_list, NL80211_ATTR_MAX, genlmsg_attrdata(gen_nlh, 0),
			genlmsg_attrlen(gen_nlh, 0), NULL);

	if (!nl_attr_list[NL80211_ATTR_VENDOR_DATA]) {
		csm_loge("No vendor data");
		goto end;
	} else {
		nla_parse(nl_vendor_data, QCA_WLAN_VENDOR_ATTR_MAX,
			nla_data(nl_attr_list[NL80211_ATTR_VENDOR_DATA]),
			nla_len(nl_attr_list[NL80211_ATTR_VENDOR_DATA]),
			NULL);
	}

	if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_RSP_MASK]) {
		csm_loge("No rsp mask");
		goto end;
	} else {
		rsp->rsp_mask =
			nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_RSP_MASK]);
	}

	if (!nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_STOP_RSP]) {
		csm_loge("No stop rsp");
		goto end;
	} else {
		rsp->stop_rsp =
			nla_get_u32(nl_vendor_data[QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_STOP_RSP]);
	}
end:
        return NL_SKIP;
}

int csm_nl_80211_clr_ll_stats(struct csm_data *csm_data,
                struct req_clr_ll_stats *req,
		struct rsp_clr_ll_stats *rsp)
{
        struct csm_nl_80211_data *nl_80211;
        struct nl_msg *msg;
        struct nl_cb *cb = NULL;
        int ret = 0, *cb_err;
        struct nlattr *data;

        if(!csm_data)
                return -1;

        nl_80211 = csm_data->nl_80211;

        if (csm_nl_80211_cmd_init(&msg, &cb, csm_nl_80211_clr_ll_stats_handler,
					rsp, &cb_err)) {
                csm_loge("cmd msg alloc fail");
                return -1;
        }

        genlmsg_put(msg, 0, 0, nl_80211->id, 0, 0, NL80211_CMD_VENDOR, 0);

        nla_put_u32(msg, NL80211_ATTR_IFINDEX,
                                        csm_data->iface_idx);
        nla_put_u32(msg, NL80211_ATTR_VENDOR_ID,
                                        OUI_QCA);
        nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
                                        QCA_NL80211_VENDOR_SUBCMD_LL_STATS_CLR);

        if (!(data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA)) ||
                nla_put_u32(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_REQ_MASK,
                                        req->req_mask) ||
                nla_put_u8(msg, QCA_WLAN_VENDOR_ATTR_LL_STATS_CLR_CONFIG_STOP_REQ,
                                        req->stop_req)) {
                ret = -ENOBUFS;
                goto err;
        }
        nla_nest_end(msg, data);

        ret = csm_nl_80211_cmd_run(nl_80211->cmd_sock, msg, cb, cb_err);

err:
	csm_logd("%s to SEND clr ll stats CMD", ret ? "Failed" : "Succeed");
        csm_nl_80211_cmd_deinit(msg, cb, cb_err);
        return ret;
}

int csm_clr_ll_stats(struct csm_data *csm_data)
{
	struct req_clr_ll_stats req = {0xff, 0};
	struct rsp_clr_ll_stats rsp;

	csm_nl_80211_clr_ll_stats(csm_data, &req, &rsp);

	if ((rsp.rsp_mask == req.req_mask) &&
		(rsp.stop_rsp == req.stop_req)) {
		return 0;
	}

	return -1;
}
/*************************************************************************************************************************************/
int csm_reset_stats(struct csm_data *csm_data)
{
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;
	char buf[256];

	//Send down a iwpriv command to clear the stats in Driver
	snprintf(buf, sizeof(buf), "iwpriv %s clearStats 0",
					csm_data->iface_name);
	csm_logd("execute command: %s", buf);
	if(!system(buf)) {
		csm_logd("clear stats in Driver");
	}

	//Send down a netlink command to clear the stats in Firmware
	if(!csm_clr_ll_stats(csm_data)) {
		csm_logd("clear stats in Fw");
	}

	free(csm_data->ll_stats);
	csm_data->ll_stats = NULL;

	free(csm_data->prev_ll_stats);
	csm_data->prev_ll_stats = NULL;

	chan_sw_win_data->consecutive_chan_sw_count = 0;
	chan_sw_win_data->consecutive_band_sw_count = 0;

	return 0;
}

int csm_check_sta_ecsa_capable(struct csm_data *csm_data)
{
	char buf[256];
	FILE *f;
	int read_line_cnt = 3;

	snprintf(buf, sizeof(buf), "iwpriv %s get_sta_info", csm_data->iface_name);
	csm_loga("execute command: %s", buf);

	f = popen(buf, "r");
	if (NULL == f) {
		csm_loge("failed to run command");
		return -1;
	}

	while (read_line_cnt--) {
		if (!fgets(buf, sizeof(buf), f)) {
			csm_loge("failed to get respone");
			return -1;
		}

		csm_loga("*****respone*****START*****");
		csm_loga(buf);
		csm_loga("-----respone-----END-------");

		if (strstr(buf, "ecsa=1"))
		{
			csm_logd("ecsa=1 found");
			pclose(f);
			return 0;
		}
	}

	pclose(f);
	return -1;
}

int csm_get_next_best_chan(struct csm_chan_data *chan_data)
{
	unsigned int chan;

	chan_data->cur_op_chan_idx =
		(chan_data->cur_op_chan_idx + 1) %
		(chan_data->cur_op_chan_list->num_chans);

	chan = chan_data->cur_op_chan_list->chans[chan_data->cur_op_chan_idx];

	csm_logd("chan: %d", chan);

	return chan;
}

#define CHANNEL_SWITCH_TIME_MICRO_SECS 500000
int csm_change_chan(struct csm_data *csm_data)
{
	char buf[256];
	unsigned int channel_num;

	channel_num = csm_get_next_best_chan(csm_data->chan_data);

	snprintf(buf, sizeof(buf), "iwpriv %s setChanChange %d",
			csm_data->iface_name, channel_num);
	csm_loga("execute command: %s", buf);
	if (!system(buf))
	{
		csm_loga("succeed to execute");
		csm_logd("Change channel to %d", channel_num);

		csm_chan_sw_win_update(csm_data, 1);

		//To account for channel switch time, before fetching stats again from the firmware
		usleep(CHANNEL_SWITCH_TIME_MICRO_SECS);

		csm_reset_stats(csm_data);

		return 0;
	}

	csm_logd("failed to execute");

	return -1;
}

int csm_change_band(struct csm_data *csm_data)
{
	struct csm_chan_data *chan_data;

	chan_data = csm_data->chan_data;

	chan_data->cur_op_band = !chan_data->cur_op_band;

	if ( BAND_5G == chan_data->cur_op_band) {
		csm_logd("Change band to 5G");
		chan_data->cur_op_chan_list = chan_data->chan_list_5g;
	} else {
		csm_logd("Change band to 2G");
		chan_data->cur_op_chan_list = chan_data->chan_list_2g;
	}

	chan_data->cur_op_chan_idx = -1;

	csm_chan_sw_win_data_reset(csm_data);

	csm_change_chan(csm_data);

	return 0;
}

int csm_update_ll_stats(struct csm_data *csm_data)
{
	struct csm_ll_stats *stats = NULL;

	stats = csm_get_ll_stats(csm_data);

	if (stats) {
		csm_logd("succeed to update");
		csm_data->ll_stats = stats;

		return 0;
	}
	else {
		csm_logd("failed to update");
		return -1;
	}
}

int csm_update_prev_ll_stats(struct csm_data *csm_data)
{
	//!!!Memory Leak!!!
	if (csm_data->prev_ll_stats)
		free(csm_data->prev_ll_stats);

	csm_data->prev_ll_stats = csm_data->ll_stats;

	csm_data->ll_stats = NULL;

	return 0;
}


static void csm_update_consecutive_chan_sw_trigger_counts(
	struct csm_data *csm_data,
	int cur_rssi_ack_avg,
	int cur_ppdu_per,
	int cur_rts_per)
{
	struct csm_config *conf = csm_data->conf;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	bool trigger_channel_switch;

	trigger_channel_switch = ((cur_rssi_ack_avg > conf->rssi_threshold) &&
					(cur_ppdu_per > conf->ppdu_per_threshold ||
					cur_rts_per > conf->rts_per_threshold));

	csm_logd("*****channel switch criterion******");
	csm_logd("cur_rssi_ack_avg > conf->rssi_threshold : %s",
			(cur_rssi_ack_avg > conf->rssi_threshold) ? "Meet" : "Not Meet");
	csm_logd("(cur_ppdu_per > conf->ppdu_per_threshold || cur_rts_per > conf->rts_per_threshold) : %s",
			(cur_ppdu_per > conf->ppdu_per_threshold || cur_rts_per > conf->rts_per_threshold) ? "Meet" : "Not Meet");
	csm_logd("trigger_channel_switch : %s",
			trigger_channel_switch ? "Meet" : "Not Meet");
	csm_logd("-----channel switch criterion-----");

	if (trigger_channel_switch) {
		chan_sw_win_data->consecutive_chan_sw_count++;
	} else {
		chan_sw_win_data->consecutive_chan_sw_count = 0;
	}

	csm_logd("consecutive_chan_sw_count: %d", chan_sw_win_data->consecutive_chan_sw_count);
}

static void csm_update_consecutive_band_sw_trigger_counts(
	struct csm_data *csm_data,
	int cur_rssi_ack_avg,
	int cur_ppdu_per,
	int cur_rts_per,
	int cur_mpdu_per,
	float mcs_avg)
{
	struct csm_config *conf = csm_data->conf;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	bool trigger_band_switch;

	if (!conf->use_both_24G_and_5G_channels) {
		// Band switching is disabled
		csm_logd("Band switching is NOT supported");
		return;
	}

	//TODO: Take speed into account when available
	if (csm_data->chan_data->cur_op_band == BAND_24G) {
		trigger_band_switch =
			(cur_rssi_ack_avg - conf->gain_24g > conf->rssi_threshold) &&
			(cur_ppdu_per > conf->ppdu_per_threshold ||
			 cur_rts_per > conf->rts_per_threshold);

		csm_logd("*****Band switching criterion on 2.4G*****");
		csm_logd("(cur_rssi_ack_avg - conf->gain_24g > conf->rssi_threshold) : %s",
				(cur_rssi_ack_avg - conf->gain_24g > conf->rssi_threshold) ? "Meet" : "Not Meet");
		csm_logd("(cur_ppdu_per > conf->ppdu_per_threshold || cur_rts_per > conf->rts_per_threshold) : %s",
				(cur_ppdu_per > conf->ppdu_per_threshold || cur_rts_per > conf->rts_per_threshold) ? "Meet" : "Not Meet");
		csm_logd("trigger_band_switch : %s",
				trigger_band_switch ? "Meet" : "Not Meet");
		csm_logd("-----Band switching criterion on 2.4G-----");
	} else {
		trigger_band_switch =
			(cur_rssi_ack_avg + conf->gain_24g > conf->rssi_threshold) &&
			(mcs_avg >= 0 &&  mcs_avg < conf->low_mcs_threshold && cur_mpdu_per > conf->mpdu_per_threshold) &&
			!chan_sw_win_data->consecutive_chan_sw_count;

		csm_logd("*****Band switching criterion on 5G*****");
		csm_logd("(cur_rssi_ack_avg + conf->gain_24g > conf->rssi_threshold) : %s",
				(cur_rssi_ack_avg + conf->gain_24g > conf->rssi_threshold) ? "Meet" : "Not Meet");
		csm_logd("(mcs_avg >= 0 && mcs_avg < conf->low_mcs_threshold && cur_mpdu_per > conf->mpdu_per_threshold) : %s",
				(mcs_avg >= 0 && mcs_avg < conf->low_mcs_threshold && cur_mpdu_per > conf->mpdu_per_threshold) ? "Meet" : "Not Meet");
		csm_logd("!chan_sw_win_data->consecutive_chan_sw_count : %s",
				!chan_sw_win_data->consecutive_chan_sw_count ? "Meet" : "Not Meet");
		csm_logd("trigger_band_switch : %s",
				trigger_band_switch ? "Meet" : "Not Meet");
		csm_logd("-----Band switching criterion on 5G-----");
	}

	// Either reset or increment the number of times the band switch condition is set
	if (trigger_band_switch) {
		chan_sw_win_data->consecutive_band_sw_count++;
	} else {
		chan_sw_win_data->consecutive_band_sw_count = 0;
	}

	csm_logd("consecutive_band_sw_count: %d", chan_sw_win_data->consecutive_band_sw_count);
}

void csm_update_consecutive_sw_trigger_counts(
        struct csm_data *csm_data,
        int cur_rssi_ack_avg,
        unsigned int cur_ppdu_per,
        unsigned int cur_rts_per,
        int cur_mpdu_per,
        float mcs_avg)
{
	csm_logd("*****key metrics*****");
	csm_logd("cur_rssi_ack_avg: %d", cur_rssi_ack_avg);
	csm_logd("cur_ppdu_per: %d", cur_ppdu_per);
	csm_logd("cur_rts_per: %d", cur_rts_per);
	csm_logd("cur_mpdu_per: %d", cur_mpdu_per);
	csm_logd("mcs_avg: %0.2f", mcs_avg);
	csm_logd("-----key metrics-----");

	csm_update_consecutive_chan_sw_trigger_counts(csm_data,
		cur_rssi_ack_avg, cur_ppdu_per, cur_rts_per);
	csm_update_consecutive_band_sw_trigger_counts(csm_data,
		cur_rssi_ack_avg, cur_ppdu_per, cur_rts_per, cur_mpdu_per, mcs_avg);
}

#define NOISE_FLOOR 96
void csm_process(struct csm_data *csm_data)
{
	struct csm_ll_stats *ll_stats = csm_data->ll_stats;
	struct csm_ll_stats *prev_ll_stats = csm_data->prev_ll_stats;
	struct csm_chan_sw_win_data *chan_sw_win_data =
					csm_data->chan_sw_win_data;

	int cur_mpdu_per = 0,
	    cur_medium_utilization,
	    cur_rssi_ack_avg,

	    cur_rts_per = 0,
	    cur_ppdu_per = 0,
	    cur_on_time = 0,
	    cur_cca_busy_time = 0,
	    cur_mpdu_succ_cnt = 0,
	    cur_mpdu_fail_cnt = 0,
	    cur_rts_succ_cnt = 0,
	    cur_rts_fail_cnt = 0,
	    cur_ppdu_succ_cnt = 0,
	    cur_ppdu_fail_cnt = 0,
	    weighted_sum = 0,
	    total_num_ppdus = 0,
	    mcs_rate_i_num_ppdus = 0;

	float mcs_avg = -1;

	int i;

	if (NULL == prev_ll_stats) {
		csm_logd("init prev ll stats");
		csm_update_prev_ll_stats(csm_data);
		return;
	}

	cur_on_time = abs(ll_stats->on_time - prev_ll_stats->on_time);
	cur_cca_busy_time = abs(ll_stats->cca_busy_time - prev_ll_stats->cca_busy_time);
	cur_medium_utilization = (cur_cca_busy_time * 100) / cur_on_time;

	cur_mpdu_fail_cnt = abs(ll_stats->mpdu_fail_cnt -  prev_ll_stats->mpdu_fail_cnt);
	cur_mpdu_succ_cnt = abs(ll_stats->mpdu_succ_cnt - prev_ll_stats->mpdu_succ_cnt);
	if ((cur_mpdu_succ_cnt + cur_mpdu_fail_cnt) > 0) {
		cur_mpdu_per = (cur_mpdu_fail_cnt * 100) / (cur_mpdu_succ_cnt + cur_mpdu_fail_cnt);
	}

	cur_rts_succ_cnt = abs(ll_stats->rts_succ_cnt - prev_ll_stats->rts_succ_cnt);
	cur_rts_fail_cnt = abs(ll_stats->rts_fail_cnt - prev_ll_stats->rts_fail_cnt);
	if ((cur_rts_fail_cnt + cur_rts_succ_cnt) > 0) {
		cur_rts_per = (cur_rts_fail_cnt * 100) / (cur_rts_fail_cnt + cur_rts_succ_cnt);
	}

	cur_ppdu_succ_cnt = abs(ll_stats->ppdu_succ_cnt - prev_ll_stats->ppdu_succ_cnt);
	cur_ppdu_fail_cnt = abs(ll_stats->ppdu_fail_cnt - prev_ll_stats->ppdu_fail_cnt);
	if ((cur_ppdu_fail_cnt + cur_ppdu_succ_cnt) > 0) {
		cur_ppdu_per = (cur_ppdu_fail_cnt * 100) / (cur_ppdu_fail_cnt + cur_ppdu_succ_cnt);
	}

	cur_rssi_ack_avg = ll_stats->rssi_ack_avg - NOISE_FLOOR;

	for (i = 0; i < MAX_MCS_RATES; i++) {
		mcs_rate_i_num_ppdus = abs(ll_stats->mcs_rate_stats[i] - prev_ll_stats->mcs_rate_stats[i]);
		total_num_ppdus += mcs_rate_i_num_ppdus;
		weighted_sum += (i * mcs_rate_i_num_ppdus);
	}
	if (total_num_ppdus > 0) {
		 mcs_avg = (float)(weighted_sum) / total_num_ppdus;
	}

	csm_update_consecutive_sw_trigger_counts(csm_data,
		cur_rssi_ack_avg, cur_ppdu_per, cur_rts_per, cur_mpdu_per, mcs_avg);

	csm_loga("*****ll stats delta*****START*****");
	csm_loga("cur_channel:               %d", ll_stats->cur_channel);
	csm_loga("rssi_ack_avg:              %d", cur_rssi_ack_avg);
	csm_loga("rts_s:                     %d", cur_rts_succ_cnt);
	csm_loga("rts_f:                     %d", cur_rts_fail_cnt);
	csm_loga("total_rts_cnt:             %d", ll_stats->rts_succ_cnt);
	csm_loga("rts_per:                   %d", cur_rts_per);
	csm_loga("ppdu_s:                    %d", cur_ppdu_succ_cnt);
	csm_loga("ppdu_f:                    %d", cur_ppdu_fail_cnt);
	csm_loga("ppdu_per:                  %d", cur_ppdu_per);
	csm_loga("mpdu_s:                    %d", cur_mpdu_succ_cnt);
	csm_loga("mpdu_f:                    %d", cur_mpdu_fail_cnt);
	csm_loga("mpdu_per:                  %d", cur_mpdu_per);
	csm_loga("cur_mu:                    %d", cur_medium_utilization);
	csm_loga("total_ppdus:               %d", total_num_ppdus);
	csm_loga("mcs_avg:                   %0.2f", mcs_avg);
	csm_loga("consecutive_chan_sw_count: %d", chan_sw_win_data->consecutive_chan_sw_count);
	csm_loga("consecutive_band_sw_count: %d", chan_sw_win_data->consecutive_band_sw_count);
	csm_loga("-----ll stats delta-----END-------");

	if (csm_data->chan_data->cur_op_band == BAND_24G) {
		if (!csm_chan_sw_win_check_sw(csm_data)) {
			/* If the channel change condition has been met for at least the previous
			   consecutive sample counts, change the channel */
			if (!csm_chan_sw_win_check_chan(csm_data)) {
				csm_change_chan(csm_data);
				return;
			}
		} else {
			/* channel switch count has exceeded and band switching is enabled */
			if (!csm_chan_sw_win_check_band(csm_data)) {
				/* Set list of channels to 5GHz channels and change the channel */
				csm_change_band(csm_data);
				return;
			}

		}
	} else {
		/* We are in 5GHz band */
		if (!csm_chan_sw_win_check_band(csm_data)) {
			/* Set list of channels to 2.4GHz channels and change the channel */
			csm_change_band(csm_data);
			return;
		}

		/* Check, if we have to switch the channe */
		if (!csm_chan_sw_win_check_sw(csm_data)) {
			/* If the channel change condition has been met for at least the previous
			   consecutive sample counts, change the channel */
			if (!csm_chan_sw_win_check_chan(csm_data)) {
				csm_change_chan(csm_data);
				return;
			}
		}
	}

	/* No channel switch */
	csm_logd("No Channel Switch");

	csm_chan_sw_win_update(csm_data, 0);

	csm_update_prev_ll_stats(csm_data);
}


/*************************************************************************************************************************************/
void *csm_process_handler(void *arg)
{
	struct csm_data *csm_data = (struct csm_data *)arg;

	int duration = csm_data->conf->sample_duration;

	while (!csm_data->csm_process_clean) {
		csm_logi("pthread debug: csm_process_handler running");
		pthread_mutex_lock(&csm_data->csm_stats_mutex);
		while (csm_data->csm_process_pause)
			pthread_cond_wait(&csm_data->csm_stats_cv, &csm_data->csm_stats_mutex);
		pthread_mutex_unlock(&csm_data->csm_stats_mutex);

		if (!csm_update_ll_stats(csm_data)) {
			csm_ll_stats_dump(csm_data->ll_stats);

			if (csm_data->csm_process_connected) {
				csm_process(csm_data);
			} else {
				csm_data->csm_process_connected = 1;
				//Init csm channel list
				csm_chan_data_init(csm_data);
				//Init csm channel switch window
				csm_chan_sw_win_data_init(csm_data);
			}
		}

		sleep(duration);
	}
	csm_logi("pthread debug: csm_process_handler exit");
	pthread_exit(0);
	return (void *)NULL;
}

int csm_iface_idx_init(struct csm_data *csm_data)
{
	int iface_idx = 0;

	iface_idx = if_nametoindex(csm_data->iface_name);
	if(!iface_idx) {
		csm_loge("%s doesn't exist", csm_data->iface_name);
		return -1;
	}
	csm_data->iface_idx = iface_idx;
	csm_logd("%s iface_idx: %d", csm_data->iface_name, csm_data->iface_idx);

	return 0;
}

int csm_iface_idx_deinit(struct csm_data *csm_data)
{
	csm_data->iface_idx = 0;

	return 0;
}

int csm_init(struct csm_data *csm_data)
{
	/* Iface idx init */
	csm_iface_idx_init(csm_data);

	/* Nl80211 Init */
	if (!csm_nl_80211_init(csm_data)) {
		csm_logd("Succeed to init NL80211");
	} else {
		csm_loge("Failed to init NL80211");
		return -1;
	}

	/* Set ll stats */
	csm_set_ll_stats(csm_data);

	/* Initialize mutex and condition variable objects */
	pthread_mutex_init(&csm_data->csm_stats_mutex, NULL);
	pthread_cond_init(&csm_data->csm_stats_cv, NULL);
	csm_data->csm_process_pause = 0;
	csm_data->csm_process_init = 0;
	csm_data->csm_process_clean = 0;
	csm_data->csm_process_connected = 0;

	pthread_create(&csm_data->csm_process_thread, NULL, &csm_process_handler, csm_data);

	csm_logi("Succeed to init CSM");

	return 0;
}

int csm_process_pause_reset(struct csm_data *csm_data)
{
        csm_chan_data_deinit(csm_data);

        csm_chan_sw_win_data_deinit(csm_data);

        if(csm_data->ll_stats) {
                free(csm_data->ll_stats);
                csm_data->ll_stats = NULL;
        }

        if(csm_data->prev_ll_stats) {
                free(csm_data->prev_ll_stats);
                csm_data->prev_ll_stats = NULL;
        }

	return 0;
}

int csm_deinit(struct csm_data *csm_data)
{
	csm_iface_idx_deinit(csm_data);

	csm_nl_80211_deinit(csm_data);

	csm_chan_data_deinit(csm_data);

	csm_chan_sw_win_data_deinit(csm_data);

	if(csm_data->ll_stats) {
		free(csm_data->ll_stats);
		csm_data->ll_stats = NULL;
	}

	if(csm_data->prev_ll_stats) {
		free(csm_data->prev_ll_stats);
		csm_data->prev_ll_stats = NULL;
	}

	pthread_mutex_destroy(&csm_data->csm_stats_mutex);
	pthread_cond_destroy(&csm_data->csm_stats_cv);

	csm_data->csm_process_pause = 0;
	csm_data->csm_process_init = 0;
	csm_data->csm_process_clean = 0;
	csm_data->csm_process_connected = 0;

	return 0;
}

int csm_is_iface_up_and_running(char *iface_name)
{
	int status = 0;
	int sockfd;
	struct ifreq ifr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
	    return -1;
	}

	memset(&ifr, 0, sizeof ifr);

	strncpy(ifr.ifr_name, iface_name, IFNAMSIZ);

	/* Get interface flags of wlan0 */
	status = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	if (status < 0) {
		csm_loge("ioctl failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	close(sockfd);

	/* Check the interface flags */
	if (ifr.ifr_flags & IFF_RUNNING) {
		csm_logd("%s is up and running", iface_name);
		return 0;
	}

	csm_loge("%s is NOT up and running", iface_name);
	return -1;
}

/* Process interface events. More specifically, events and actions are:
 * IFACE UP/DOWN (RTM_NEWLINK) event: Unpause/Pause stats collection.
 *                                    On IFACE_UP, handles dwm initliazation
 *                                    as well, if it is not already done.
 * IFACE REMOVED (RTM_DELLINK) event: Triggers clean up
 */
#define IFF_LOWER_UP	0x10000		/* driver signals L1 up		*/
static int process_netlink_interface_event(struct nl_msg *msg, void *arg)
{
	struct ifinfomsg *ifi;
	struct nlmsghdr * hdr = nlmsg_hdr(msg);

	int attrlen;
	struct rtattr *attr;
	char ifname[IFNAMSIZ + 1];

	char* buf = (char *)NLMSG_DATA(hdr) + NLMSG_ALIGN(sizeof(struct ifinfomsg));
	int len = NLMSG_PAYLOAD(hdr, sizeof(struct ifinfomsg));

	struct csm_data *csm_data = (struct csm_data *)arg;

	attrlen = len;
	attr = (struct rtattr *) buf;
	/* Extract the interface name associated with the event */
	while (RTA_OK(attr, attrlen)) {
		switch (attr->rta_type) {
			case IFLA_IFNAME:
			if (RTA_PAYLOAD(attr) > IFNAMSIZ)
				break;
			memcpy(ifname, RTA_DATA(attr), RTA_PAYLOAD(attr));
			ifname[RTA_PAYLOAD(attr)] = '\0';
			break;
		}
		attr = RTA_NEXT(attr, attrlen);
	}

	if (memcmp(ifname, csm_data->iface_name, strlen(csm_data->iface_name)) == 0) {
		if (hdr->nlmsg_type == RTM_NEWLINK) {
			ifi = (struct ifinfomsg *)nlmsg_data (hdr);
			csm_logi("NETLINK::%s\n", (ifi->ifi_flags & IFF_LOWER_UP) ? "Up" : "Down");
			if (ifi->ifi_flags & IFF_LOWER_UP) {
				/* If initialization is not already done */
				if (!csm_data->csm_process_init) {
					/* Initialize csm */
					csm_init(csm_data);
					csm_data->csm_process_init = 1;
				}

				/* Unpaused stats collection */
				if (csm_data->csm_process_pause) {
					csm_data->csm_process_pause = 0;
					pthread_cond_signal(&csm_data->csm_stats_cv);
					csm_logi("Interface is up. Unpaused stats collection");
				}
			} else {
				/* Interface is down */
				if (csm_data->csm_process_init) {
					/* Pause stats collection */
					csm_logi("Interface is down. Pausing stats collection");
					csm_data->csm_process_pause = 1;

					/* dis-connected and reset */
					csm_data->csm_process_connected = 0;

					csm_process_pause_reset(csm_data);
				}
			}
		} else if (hdr->nlmsg_type == RTM_DELLINK) {
			ifi = (struct ifinfomsg *)nlmsg_data(hdr);
			csm_logi("Interface removed\n");
			csm_data->csm_process_clean = 1;
			/* Wake-up stats collection thread which could be sleeping because of previous ifdown wlan0 event
			* There is no harm here in sending a signal even if it is not sleeping.
			* This is to wake up stats collection thread when these events (ordered by time) occur.
			* Event1: wlan0 down (ifconfig wlan0 down or hostapd is killed), makes stats collection thread wait for unpause event
			* Event2: rmmod wlan.ko
			*/
			if (csm_data->csm_process_pause) {
				csm_data->csm_process_pause = 0;
				pthread_cond_signal(&csm_data->csm_stats_cv);
			}
		}
	}

	return 0;
}

/* Thread function to listen for interface up/down events */
void *csm_monitor_handler(void *arg)
{
	struct nl_sock *sk;
	struct csm_data *csm_data = (struct csm_data *)arg;

	/* Allocate a new socket */
	sk = nl_socket_alloc();

	if (sk == NULL) {
	    csm_loge("Failed to create nelink socket");
	    return NULL;
	}

	/* Notifications do not use sequence numbers, disable sequence number checking */
	nl_socket_disable_seq_check(sk);

	/* Register a callback function, which will be called for each notification received */
	nl_socket_modify_cb(sk, NL_CB_VALID, NL_CB_CUSTOM, process_netlink_interface_event, arg);

	/* Connect to routing netlink protocol */
	nl_connect(sk, NETLINK_ROUTE);

	/* Subscribe to link notifications group */
	nl_socket_add_memberships(sk, RTNLGRP_LINK, 0);

	while (true) {
		csm_logi("pthread debug: csm_monitor_handler running");
		/* Initialize csm, if the wlan interface is already up and running (In a way
		 * confirm that hostapd is running). Otherwise, wait until wlan0 is up and handle
		 * the initialization in the interface events handler (process_netlink_csm_monitor).
		 */
		if(!csm_is_iface_up_and_running(csm_data->iface_name)) {
			csm_logi("Interface is up and running");
			csm_init(csm_data);
			csm_data->csm_process_init = 1;
		} else {
			/* If there is no wlan0, delay the initialization until it is up */
			csm_loge("Interface is not up: csm_daemon initialization deferred");
		}

		/* Event loop tracking interface events (handled in process_netlink_csm_monitor()).
		 * Exits only on interface removal. So, if there is no interface on startup, it would
		 * wait here until wlan0 is created and removed */
		while (!csm_data->csm_process_clean) {
		    nl_recvmsgs_default(sk);
		}

		csm_logi("Waiting for other threads to finish");
		pthread_join(csm_data->csm_process_thread, NULL);

		/* Reset state */
		csm_logi("csm_deinit");
		csm_deinit(csm_data);
	}

	pthread_exit(0);
	return (void *)NULL;
}

/*************************************************************************************************************************************/
int optind = 1;
int optopt;
char *optarg;

void csm_main_exit(int signum)
{
	exit(0);
}

int main(int argc, char *argv[])
{
	int c;
	char *iface = NULL;
	char *conf_file = NULL;

	struct csm_data csm_data;

	memset(&csm_data, 0, sizeof(csm_data));

	signal(SIGINT, csm_main_exit);

	csm_logi("***ChannelSwitchManager***");
	for(;;) {
		c = getopt(argc, argv, "i:c:d");
		if (c < 0)
			break;

		switch (c) {
		case 'i':
			iface = optarg;
			break;
		case 'c':
			conf_file = optarg;
			break;
		case 'd':
			csm_debug_level--;
			break;
		default:
			break;
		}
	}

	if (iface) {
		csm_data.iface_name = iface;
	} else {
		csm_loge("iface missed");
		exit(0);
	}

	if (conf_file) {
		struct csm_config *conf;

		csm_logd("conf_file: %s", conf_file);
		conf = csm_config_read(conf_file);
		if (conf) {
			csm_logi("Succeed to load conf");
			csm_config_dump(conf);
			csm_data.conf = conf;
		} else {
			csm_logi("Failed to load conf");
		}
	} else {
		csm_loge("conf_file missed");
		exit(0);
	}

	/* create csm monitor thread */
	pthread_create(&csm_data.csm_monitor_thread, NULL, &csm_monitor_handler, &csm_data);
	pthread_join(csm_data.csm_monitor_thread, NULL);

	exit(0);
}
