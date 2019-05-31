/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_COMMON_H__
#define __GST_QAHW_COMMON_H__

G_BEGIN_DECLS

#define QAHW_SINK_PCM_CAPS \
        "audio/x-raw, " \
        "format = (string) {S16LE,S16BE,S24LE,S24BE}, " \
        "layout = (string) interleaved, " \
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]"

#define QAHW_SINK_OFFLOAD_CAPS \
        "audio/mpeg, mpegversion = (int) 1, layer = (int) 3," \
        " parsed = (boolean) TRUE; " \
        "audio/mpeg, mpegversion = (int) 2, stream-format = (string)" \
        " {adts,raw}, framed = (boolean) TRUE;" \
        "audio/mpeg, mpegversion = (int) 4, stream-format = (string)" \
        " {adts,raw}, framed = (boolean) TRUE, base-profile = (string) lc; " \
        "audio/x-vorbis, framed=(boolean)TRUE"

#define QAHW_SINK_FLAC_CAPS \
        "audio/x-flac, " \
        "rate = (int) [ 1, MAX ], " "channels = (int) { 1, 2 }"

#define QAHW_SINK_ALAC_CAPS \
        "audio/x-alac, " \
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]"

#define QAHW_SINK_WMA_CAPS \
        "audio/x-wma, " \
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]"

#define QAHW_SINK_CAPS QAHW_SINK_PCM_CAPS  ";" QAHW_SINK_OFFLOAD_CAPS ";" QAHW_SINK_ALAC_CAPS ";" QAHW_SINK_FLAC_CAPS ";" QAHW_SINK_WMA_CAPS

G_END_DECLS

#endif /* __GST_QAHW_COMMON_H__ */
