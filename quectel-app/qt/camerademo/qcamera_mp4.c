#include "qcamera_mp4.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef /* @abstract@ */ unsigned char  bool; 
#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#define STREAM_FRAME_RATE 25

AVFormatContext *m_pOc = NULL;
/*int waitkey = 1;
int vi = 0;
int ptsInc = 0;
int m_width = 0;  //width of frame
int m_height = 0; //height of frame
bool gotHeader = 0;*/

typedef struct mp4_muxer_param
{
	int waitkey;
	int videoIndex;
	int ptsInc;
	int width;
	int height;
	bool gotHeader;
	unsigned char outputName[256];	
};

struct mp4_muxer_param g_muxer_param;
#define EXTRADATA_SIZE 27
//#define STREAM_BACKUP_H
#define _ANDROID_LOG_
//#define QT

#ifdef _ANDROID_LOG_
	#include <utils/Log.h>

	#define QCAMERA_MSG_HIGH(fmt, ...) ALOGE("VT_HIGH %s::%d " fmt, __FUNCTION__, __LINE__,  ##__VA_ARGS__)
#else
	#define QCAMERA_MSG_HIGH(fmt, ...) printf
#endif

//add extradata, if there is no extradata, the mp4 video will not be drag
//unsigned char extradata[EXTRADATA_SIZE] = {0x0,  0x0,  0x0,  0x1,  0x67,  0x42,  0x80,  0x15,  0xda,  0x1,  0x40,  0x16,  0xe8,  0x6,  0xd0,  0xa1,  0x35,  0x0,  0x0,  0x0,  0x1,  0x68,  0xce,  0x6,  0xe2};
//unsigned char extradata[EXTRADATA_SIZE] = {0x0,  0x0,  0x0,  0x1,  0x67,  0x42,  0x80,  0x15,  0xda,  0x1,  0xe0,  0x08,  0x9f,  0x96,  0x01,  0xb4,  0x28,  0x4d,  0x40, 0x0,  0x0,  0x0, 0x1,  0x68,  0xce,  0x6,  0xe2};

unsigned char nal_prefix[4] = "\x00\x00\x00\x01";
//unsigned char extradata[EXTRADATA_SIZE] = { 0 };
int getVopType( const void *p, int len )
{
    if ( !p || 6 >= len )
        return -1;
    unsigned char *b = (unsigned char*)p;
    // Verify NAL marker
    if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
    {   b++;
		if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
			return -1;
    } // end if
    b += 3;
    // Verify VOP id
    if ( 0xb6 == *b )
    {   
		b++;
		return ( *b & 0xc0 ) >> 6;
    } // end if
    switch( *b )
    {  
		case 0x65 : return 0;
		case 0x61 : return 1;
		case 0x01 : return 2;
    } // end switch
    return -1;
}

/* Add an output stream */
AVStream *add_stream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;
    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!*codec)
    {
        QCAMERA_MSG_HIGH("could not find encoder for '%s' \n", avcodec_get_name(codec_id));
        exit(1);
    }
    st = avformat_new_stream(oc, *codec);
    if (!st)
    {
        QCAMERA_MSG_HIGH("could not allocate stream \n");
        exit(1);
    }
    st->id = oc->nb_streams-1;
    c = st->codec;
    g_muxer_param.videoIndex = st->index;
    switch ((*codec)->type)
    {
    case AVMEDIA_TYPE_VIDEO:
        QCAMERA_MSG_HIGH("AVMEDIA_TYPE_VIDEO\n");
        c->codec_id = AV_CODEC_ID_H264;
        c->width = g_muxer_param.width;
        c->height = g_muxer_param.height;
		//st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
        c->time_base.den = 25;
        c->time_base.num = 1;
		//c->time_base = st->time_base;
        c->gop_size = 1;
        c->pix_fmt = AV_PIX_FMT_NV21;
        //c->pix_fmt = AV_PIX_FMT_YUV420P;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
        {
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
        {
            c->mb_decision = 2;
        }
        break;
    default:
        break;
    }
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
    {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    return st;
}
void open_video(AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;
    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0)
    {
        QCAMERA_MSG_HIGH("could not open video codec");
       //exit(1);
    }
}

#ifdef STREAM_BACKUP_H
FILE* bak_file = NULL;
#endif
void init_params()
{
	g_muxer_param.waitkey = 1;
	g_muxer_param.videoIndex = 0;
	g_muxer_param.ptsInc = 0;
	g_muxer_param.width = 0;  //width of frame
	g_muxer_param.height = 0; //height of frame
	g_muxer_param.gotHeader = false;
	memset(g_muxer_param.outputName, 0, sizeof(g_muxer_param.outputName));
}

int qcamera_create_mp4(unsigned char* outputName, int width, int height)
{
    int ret = 0; // 成功返回0，失败返回1
    AVOutputFormat *fmt;
    AVCodec *video_codec;
    AVStream *m_pVideoSt = NULL;

	init_params();
	memcpy(g_muxer_param.outputName, outputName, strlen(outputName));
    av_register_all();

#ifdef STREAM_BACKUP_H
	char* bak_file_name = "/data/misc/camera/bak.h264";
	bak_file = fopen(bak_file_name, "w+");
#endif
    avformat_alloc_output_context2(&m_pOc, NULL, NULL, outputName);
    if (!m_pOc)
    {
        avformat_alloc_output_context2(&m_pOc, NULL, "mpeg", outputName);
    }
    if (!m_pOc)
    {
	printf("failed to find output context\n");
        return 1;
    }

    fmt = m_pOc->oformat;
    if (fmt->video_codec == AV_CODEC_ID_NONE)
    {
	printf("video codec is none\n");
	return 1;
    }
    
	g_muxer_param.width = width;
	g_muxer_param.height = height;
	m_pVideoSt = add_stream(m_pOc, &video_codec, fmt->video_codec);
    if (m_pVideoSt)
    {
        open_video(video_codec, m_pVideoSt);
    }

	return ret;
}

int qcamera_write_header(unsigned char *header, int len)
{
	AVStream *m_pVideoSt = NULL;	
	int ret = 0, j = 0;
	m_pVideoSt = m_pOc->streams[m_pOc->nb_streams-1];
	if (m_pVideoSt == NULL)
	{
		return -1;
	}

/*add extradata*/
	m_pVideoSt->codec->extradata_size = len;
	m_pVideoSt->codec->extradata = (unsigned char *)malloc(m_pVideoSt->codec->extradata_size);
	for (;j < m_pVideoSt->codec->extradata_size; j++)
	{
		m_pVideoSt->codec->extradata[j] = *(header+j);
	}

    av_dump_format(m_pOc, 0, g_muxer_param.outputName, 1);
    /* open the output file, if needed */
    if (!(m_pOc->oformat->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&m_pOc->pb, g_muxer_param.outputName, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            QCAMERA_MSG_HIGH("could not open %s\n", g_muxer_param.outputName);
            return 1;
        }
    }
    /* Write the stream header, if any */
    ret = avformat_write_header(m_pOc, NULL);
    if (ret < 0)
    {
        QCAMERA_MSG_HIGH("Error occurred when opening output file");
        return 1;
    }

	return ret;
}

int findNextNalStartCode(unsigned char *data, size_t length) {
    int *res = NULL;
    int index = 0;
    while(length - index> 3)
    {
        if (!memcmp(data+index, nal_prefix, 4))
        {
            break;
        }
        index++;
    }

    return length - index > 3? index : -1;
}


int parseAVCCodecSpecificData(unsigned char *data, int len)
{
    int bytesLeft = len;
    unsigned char *tmp = data;
    int nextNalPpsStart = 0;
    int nextNalPpsEnd = 0;
	bool gotPps = false;
	bool gotSps = false;
	
    if (!memcmp(nal_prefix, tmp, 4)) {
        if (!gotSps && ((*(tmp + 4)) & 0x1F) == 0x7)
        {
            gotSps = true;
        }

		if (gotSps)
		{
        	nextNalPpsStart = findNextNalStartCode(tmp + 4, bytesLeft - 4);
		}

        if (nextNalPpsStart > 0 && ((*(tmp + 8 + nextNalPpsStart)) & 0x1F) == 0x8)
        {
            gotPps = true;

        }
		if (gotPps)
		{
        	nextNalPpsEnd = findNextNalStartCode(tmp + 8 + nextNalPpsStart, bytesLeft - 8 - nextNalPpsStart);  //maybe this frame have no data like 0000000165
		}
    }

	if (gotSps && gotPps && nextNalPpsEnd > 0)
	{
		return nextNalPpsStart + 8 + nextNalPpsEnd;
	} else if (gotSps && gotPps && nextNalPpsEnd < 0)
	{
		
		return len;
	}

	return -1;
}

/* write h264 data to mp4 file

 * 创建mp4文件返回2；写入数据帧返回0 */
void qcamera_write_video(void* data, int nLen, double nTimeStamp)
{
	int ret;
    if ( 0 > g_muxer_param.videoIndex )
    {
       QCAMERA_MSG_HIGH("videoIndex less than 0");
        return ;
    }

	if (!g_muxer_param.gotHeader)
	{
		int dataEnd = parseAVCCodecSpecificData(data, nLen);
		if (dataEnd > 0)
		{
			qcamera_write_header(data, dataEnd);
			g_muxer_param.gotHeader = true;
		} else 
		{
			QCAMERA_MSG_HIGH("wait extradata...\n");
			return ;
		}

	}

#ifdef STREAM_BACKUP_H
	fwrite(data, 1, nLen, bak_file);
#endif

    AVStream *pst = m_pOc->streams[ g_muxer_param.videoIndex ];
    // Init packet
    AVPacket pkt;
    // 我的添加，为了计算pts
    av_init_packet( &pkt );
    //pkt.flags |= ( 0 >= getVopType( data, nLen ) ) ? AV_PKT_FLAG_KEY : 0;
    pkt.flags = AV_PKT_FLAG_KEY;
    pkt.stream_index = pst->index;
    pkt.data = (uint8_t*)data;
    pkt.size = nLen; 

	//pkt.buf = NULL;
    // Wait for key frame
    /*if ( waitkey )
	{
        if ( 0 == ( pkt.flags & AV_PKT_FLAG_KEY ) )
            return ;
        else
            waitkey = 0;
	}*/
    pkt.pts = av_rescale_q((g_muxer_param.ptsInc++)*2, pst->codec->time_base,pst->time_base);
	/*double cal = 0;  //fixed framerate
    cal = (1000000/STREAM_FRAME_RATE);
	pkt.pts=(double)(ptsInc++ * cal) / (double)(av_q2d(pst->codec->time_base)*AV_TIME_BASE);
	pkt.pts=(double)(nTimeStamp) / (double)(av_q2d(pst->codec->time_base)*AV_TIME_BASE);
	pkt.pts = nTimeStamp/(double)(av_q2d(STREAM_FRAME_RATE));*/
	//printf("pkt pts = %f time = %f", pkt.pts, nTimeStamp);
	//pkt.pts=(double)(nTimeStamp) / (double)(av_q2d(pst->codec->time_base)*AV_TIME_BASE);

	//pkt.pts = nTimeStamp / 10;  //i don't know why
	pkt.dts = pkt.pts;
    //pkt.pts=av_rescale_q_rnd(pkt.pts, pst->codec->time_base,pst->codec->time_base,(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    //pkt.dts=av_rescale_q_rnd(pkt.dts, pst->codec->time_base,pst->codec->time_base, (AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    //pkt.duration = av_rescale_q(pkt.duration,pst->codec->time_base, pst->codec->time_base);
    pkt.pos = -1;

    ret = av_interleaved_write_frame( m_pOc, &pkt );

    if (ret < 0)
    {
        QCAMERA_MSG_HIGH("cannot write frame");
		return ;
    }
}

void qcamera_close_mp4()
{
    g_muxer_param.waitkey = -1;
	g_muxer_param.videoIndex = -1;
	g_muxer_param.ptsInc = 0;

#ifdef STREAM_BACKUP_H
	fclose(bak_file);
#endif
    if (m_pOc)
	{
        av_write_trailer(m_pOc);
	}
    if (m_pOc && !(m_pOc->oformat->flags & AVFMT_NOFILE))
	{
        avio_close(m_pOc->pb);
	}

    if (m_pOc)
    {
        avformat_free_context(m_pOc);
        m_pOc = NULL;
    }
}

#ifdef __cplusplus
}
#endif
