#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavutil/opt.h"
#include <libavutil/log.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif
#endif
 
const char *inputFileName = NULL;
const char *outputFileName = NULL;
const char *method = NULL;
int frameWidth = 0;
int frameHeight = 0;
int bitRate = 0;
int frameTotal = 0;
FILE *pFileInput = NULL;
FILE *pFileOutput = NULL;

AVCodec *codec = NULL;
AVCodecContext *codecCtx = NULL;
AVFrame *frame = NULL;
AVPacket pkt;
uint8_t * bufferData;
static int parse_input_paramaters(int argc, char **argv)
{
	method = argv[1];
    inputFileName = argv[2];
    outputFileName = argv[3];

    pFileInput = fopen(inputFileName, "rb");
    if(NULL == pFileInput)
    {
        printf("open file ERR: [%s] \n", inputFileName);
        return -1;
    }

    pFileOutput = fopen(outputFileName, "wb+");
    if(NULL == pFileInput)
    {
        printf("open file ERR: [%s] \n", outputFileName);
        return -1;
    }

    frameWidth = atoi(argv[4]);
    frameHeight = atoi(argv[5]);
    bitRate = atoi(argv[6]);
    frameTotal = atoi(argv[7]);
    return 0;
}

static int read_yuv_data()
{
	int i = 0;
	uint8_t *pNV = bufferData + frameWidth*frameHeight;
	uint8_t *temp;
	fread(bufferData, frameWidth*frameHeight * 3 / 2, 1, pFileInput);
	memcpy(frame->data[0], bufferData, frameWidth*frameHeight);
	for (i = 0; i < frameWidth*frameHeight/2; i++)
	{
		if ((i%2)==0)
		{
			temp = frame->data[1]++;
			temp = pNV+i;
		} else
		{
			temp = frame->data[2]++;
			temp = pNV+i;
		}
	}

	return 0;
}


static FILE *fp_log;

#define LOG_BUF_PREFIX_SIZE 512
#define LOG_BUF_SIZE 1024
static char logBufPrefix[LOG_BUF_PREFIX_SIZE];
static char logBuffer[LOG_BUF_SIZE];
static pthread_mutex_t cb_av_log_lock;

typedef void (*ffmpeg_log_callback)(void *ptr, int level, const char *fmt, va_list vl);

static void log_callback_null(void *ptr, int level, const char *fmt, va_list vl){
    FILE *fp = (FILE*)fp_log;
    int cnt;
    pthread_mutex_lock(&cb_av_log_lock);
    cnt = snprintf(logBufPrefix, LOG_BUF_PREFIX_SIZE, "%s", fmt);
    cnt = vsnprintf(logBuffer, LOG_BUF_SIZE, logBufPrefix, vl);
    fprintf(fp, "%s", logBuffer);
    fflush(fp);
    pthread_mutex_unlock(&cb_av_log_lock);
}


int test_encode()
{
	int picture_size = 0;
	uint8_t* picture_buf;
	codec = avcodec_find_encoder(AV_CODEC_ID_H264);//查找编×××
    if(NULL == codec)
    {
        printf("find AV_CODEC_ID_H264 fail! \n");
        return -1;
    }

    //分配AVCodecContex实例
    codecCtx = avcodec_alloc_context3(codec);
    if(NULL == codecCtx)
    {
        printf("avcodec_alloc_context3  ERROR\n");
        return -1;
    }

    //设置编×××的参数
    codecCtx->width = frameWidth;//帧高
    codecCtx->height = frameHeight;
    codecCtx->bit_rate = bitRate;//比特率
    AVRational r = {1, 25};//设置帧率
    codecCtx->time_base = r;//设置帧率
    codecCtx->gop_size =12;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P; //AV_PIX_FMT_420P
    //av_opt_set(codecCtx->priv_data, "preset", "slow", 0);

    //打开编码器
    if(avcodec_open2(codecCtx, codec, NULL) < 0)
    {
        printf("avcodec_open2 ERR\n");
    }

    frame = av_frame_alloc();
    if(NULL == frame)
    {
        printf("av_frame_alloc err \n");
        return -1;
    }

    //分配AVframe及像素存储空间
    frame->width = codecCtx->width;
    frame->height = codecCtx->height;
    frame->format = codecCtx->pix_fmt;

	picture_size = avpicture_get_size(codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
	picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)frame, picture_buf, codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
	bufferData = (uint8_t *)av_malloc(codecCtx->width*codecCtx->height*3/2);

    int i;
    for(i = 0; i < frameTotal; i++)
    {
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;

        read_yuv_data();
        frame->pts = i;

        int got_packet;
        //int avcodec_encode_video2(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int *got_packet_ptr);
        if(avcodec_encode_video2(codecCtx, &pkt, (const AVFrame *)frame, &got_packet) < 0)
        {
            printf("avcodec_encodec_video2 ERR \n");
            return -1;
        }
        if(got_packet)
        {
            printf("Write packet of frame [%d], size=[%d] \n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, pFileOutput);
            av_packet_unref(&pkt);
        }
    }

    fclose(pFileInput);
    fclose(pFileOutput);
    avcodec_close(codecCtx);
    av_free(codecCtx);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);

	return 0;
}

int main(int argc, char **argv)
{
    if(parse_input_paramaters(argc, argv) == 0)
    {
		printf("method:%s \n", method);
        printf("inputFile:%s \n", inputFileName);
        printf("outputFile:%s \n", outputFileName);
        printf("Frame resolution::[%d*%d] \n", frameWidth, frameHeight);
        printf("freamToEncode=%d\n", frameTotal);
        printf("rate:%d \n", bitRate);
    }
    else
    {
        printf("init ERROR\n");
        return -1;
    }

	fp_log = fopen("/data/debug.log", "w+");
	if ( NULL == fp_log){
    	printf("cant open log file %s\n", "debug.log");
    	return 0;
	}
	ffmpeg_log_callback fptrLog = log_callback_null;
/* END */
	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_flags(AV_LOG_SKIP_REPEATED);
	av_log_set_callback(fptrLog);

    avcodec_register_all();//注册编解码组件

	if (!strcmp(method, "encode"))
	{
		test_encode();
	} else (!strcmp(method, "decode"))
	{
		printf("test decode");
	}
	return 0;
}

