#ifndef _PCM_H_
#define _PCM_H_
/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <alsa/asoundlib.h>

#define PCM_DEVICE_NAME_MAX_LENGTH 31

struct pcm {
	snd_pcm_t *handle;
	char device[32];
	snd_pcm_uframes_t buffer_size;
	snd_pcm_uframes_t chunk_size;
	snd_pcm_stream_t stream;
	int running;
    unsigned int sample_rate;
};

struct pcm *pcm_open_dev(const char *device, unsigned int type, unsigned int sample_rate, unsigned int latency);
void pcm_close_dev(struct pcm *pcm);
void pcm_start(struct pcm *pcm);
void pcm_write_data(struct pcm *pcm, void *buffer, int count);
ssize_t pcm_read_data(struct pcm *pcm, void *buffer, int count);
long pcm_get_delay(struct pcm *pcm,  snd_pcm_uframes_t frames, struct timespec *tstamp);
int set_sample_rate(unsigned int value);
void set_recv_out_volume(int percent);
void inhibit_alsa_err();

#endif /* _PCM_H_ */
