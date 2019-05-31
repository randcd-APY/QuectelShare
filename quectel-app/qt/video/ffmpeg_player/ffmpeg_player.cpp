#ifndef FFMPEG_PLAYER_CPP
#define FFMPEG_PLAYER_CPP
#include <QtDebug>
#include <stdio.h>
#include "ffmpeg_player.h"
#include <time.h>
#include <sys/time.h>

#define __STDC_CONSTANT_MACROS
extern "C" {
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

//char* filename = "tc10.264";
//char* filename = "1080p.mp4";
//char* filename = "720p.mp4";

static int difftimeval(const struct timeval *start, const struct timeval *end)
{
        double d;
        time_t s;
        suseconds_t u;

        s = end->tv_sec - start->tv_sec;
        u = end->tv_usec - start->tv_usec;
        //if (u < 0)
        //        --s;

        d = s;
        d *= 1000000;//1 ¿ = 10^6 ¿¿
        d += u;

        return d;
}

VideoPlayer::VideoPlayer():
	screenUpdateCb(NULL),
	screenCleanCb(NULL)
{
	qDebug() <<"create VideoPlayer\n" << endl;
}

void VideoPlayer::setVideoName(const char *videoName)
{
	memcpy(mVideoName, videoName, strlen(videoName));
}

void VideoPlayer::stop()
{
	isStop = true;
}

void VideoPlayer::run()
{
	//added by tommy for redirection output
	//freopen("/dev/ttyHSL0","w",stdout);
	
	qDebug() << "video play run\n" << endl;

    AVCodec *pCodec; //解码器指针
    AVCodecParameters* pAVParam; //ffmpeg解码类的类成员
    AVFrame* pAvFrame; //多媒体帧，保存解码后的数据帧
    AVFormatContext* pFormatCtx = NULL; //保存视频流的信息
    AVCodecContext* pCodecCtx;
    AVPacket* pAvPacket;
    AVCodec *decoder = NULL;
    int ret = -1;

	isStop = false;
    //av_log_set_level(AV_LOG_DEBUG);


    av_register_all(); //注册库中所有可用的文件格式和编码器

    //pFormatCtx = avformat_alloc_context();
    int videoindex = -1;
    int i = 0;

	qDebug() << mVideoName << endl;
	printf("mVideoName = %s\n", mVideoName);

    if (avformat_open_input(&pFormatCtx, mVideoName, NULL, NULL) != 0) { //检查文件头部
        qFatal("Can't find the stream!");
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) { //查找流信息
        qFatal("Can't find the stream information !");
    }

    for (i=0; i < pFormatCtx->nb_streams; ++i) //遍历各个流，找到第一个视频流,并记录该流的编码信息
    {
        AVStream *stream = pFormatCtx->streams[i];
		if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1) {
		qFatal("Don't find a video stream !");
		return ;
	}
	
	pAVParam = pFormatCtx->streams[videoindex]->codecpar; //得到一个指向视频流的上下文指针
    pCodec = avcodec_find_decoder(pAVParam->codec_id); //到该格式的解码器
    pCodecCtx = avcodec_alloc_context3(pCodec);

	avcodec_parameters_to_context(pCodecCtx, pAVParam);
	
	if (pCodec == NULL) {
		qFatal("Cant't find the decoder !"); //寻找解码器
		return ;
	}
	if (avcodec_open2(pCodecCtx,pCodec,NULL) < 0) { //打开解码器
		qFatal("Can't open the decoder !");
		return ;
	}

	/*FILE* fpRGB = fopen("rgb.yuv", "wb+");
	if (!fpRGB)
	{
		printf("文件打开失败！\n");
		return ;
	}

	char rgb_buffer[24] = "sssss";
    int rgb_num = fwrite(rgb_buffer, 1, 12,fpRGB);
	printf("rgb num = %d\n", rgb_num);
	fclose(fpRGB);*/



	/*pAvFrame = av_frame_alloc(); //分配帧存储空间
	pAvPacket = av_packet_alloc();

	av_init_packet(pAvPacket);

	AVFrame* pFrameYUV;
	pFrameYUV = av_frame_alloc();

	int yFrameSize= (size_t) (pFormatCtx->streams[0]->codecpar->width * pFormatCtx->streams[0]->codecpar->height);
	int uvFrameSize= yFrameSize>>2;

	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, 
		pCodecCtx->height, 4);
	uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer, AV_PIX_FMT_YUV420P,
		pCodecCtx->width, pCodecCtx->height, 4);

	struct SwsContext* pswscontext = NULL;
	sws_getCachedContext(pswscontext, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

	printf("open demo.yuv\n");
	FILE* fpYUV = fopen("demo.yuv", "wb+");
	if (!fpYUV)
	{
		printf("文件打开失败！\n");
		return ;
	}

	int count = 0;
	while (1)
	{
			int ret=av_read_frame(pFormatCtx, pAvPacket);
			if (pAvPacket->stream_index!=videoindex)
			{
				av_packet_unref(pAvPacket);
				continue;
			}

			if(ret!=0){
				log(ret,"av_read_frame");
				break;
			}
			ret=avcodec_send_packet(pCodecCtx, pAvPacket);
			if(ret!=0){
				log(ret,"avcodec_send_packet");
				break;
			}
			ret=avcodec_receive_frame(pCodecCtx, pAvFrame);
			if(ret==0){
				//sws_scale(pswscontext, (uint8_t const* const*)pAvFrame->data, pAvFrame->linesize, 0,
				//		pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
				int i = 0;
				printf("pCodecCtx->height = %d, pCodecCtx->width = %d\n", pCodecCtx->width, pCodecCtx->height);
				for (i = 0; i < pCodecCtx->height; i++)
				{
					fwrite(pAvFrame->data[0] + i * pAvFrame->linesize[0],1, pCodecCtx->width, fpYUV);
				}

				for (i = 0; i < pCodecCtx->height / 2; i++)
				{
					fwrite(pAvFrame->data[1] + i * pAvFrame->linesize[0] / 2,1, pCodecCtx->width / 2, fpYUV);
				}

				for (i = 0; i < pCodecCtx->height / 2; i++)
				{
					fwrite(pAvFrame->data[2] + i * pAvFrame->linesize[0] / 2,1, pCodecCtx->width / 2, fpYUV);
				}
		
				//fwrite(pAvFrame->data[0], 1, pAvFrame->linesize[0] * pCodecCtx->height, fpYUV);
				
				//fwrite(pAvFrame->data[0],1, pAvFrame->linesize[0] * 288, fpYUV);
				//fwrite(pAvFrame->data[1],1, pAvFrame->linesize[0] * 288 / 4, fpYUV);
				//fwrite(pAvFrame->data[2],1, pAvFrame->linesize[0] * 288 / 4, fpYUV);
			}else{
				log(ret,"avcodec_receive_frame");
			}
			av_packet_unref(pAvPacket);
			if (count++ > 800)
				break;
	}

	if (pFrameYUV != NULL)
		av_free(pFrameYUV);
	fclose(fpYUV);*/

	AVFrame *pFrameRGB;
	SwsContext *img_convert_ctx;
	pAvFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    pAvPacket = av_packet_alloc();
    av_init_packet(pAvPacket);
	int count = 0;
    uint8_t *out_buffer;


	printf("width = %d height = %d\n", pCodecCtx->width, pCodecCtx->height);
    out_buffer=new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height)];
    //avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

    int got_picture;
	struct timeval start, end;
	AVPixelFormat pixfmt;
	switch (pCodecCtx->pix_fmt)
	{
		case AV_PIX_FMT_YUVJ420P:
			pixfmt = AV_PIX_FMT_YUV420P;
			break;
		default:
			pixfmt = pCodecCtx->pix_fmt;
	}
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pixfmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

    while (!isStop)
    {
		gettimeofday(&start, NULL);
        if(av_read_frame(pFormatCtx, pAvPacket)>=0)
        {
            if(pAvPacket->stream_index==videoindex)
            {
                ret = avcodec_decode_video2(pCodecCtx, pAvFrame, &got_picture, pAvPacket);
				printf("got picture = %d, ret = %d\n", got_picture, ret);
                if(ret < 0)
                {
                    continue ;
                }
                if (got_picture)
                {
                    sws_scale(img_convert_ctx, (const uint8_t* const*)pAvFrame->data, pAvFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
					if (screenUpdateCb != NULL)
					{
						screenUpdateCb(pFrameRGB->data[0], pCodecCtx->width, pCodecCtx->height);
					}
					qDebug() << "count = " << count++ << endl;
                }
            }
            av_free_packet(pAvPacket);
			gettimeofday(&end, NULL);
			if (count > 500)
				break;
        }
        else
        {
            break;
        }
    }
	sws_freeContext(img_convert_ctx);
	free(out_buffer);
	if (pFrameRGB != NULL)
		av_free(pFrameRGB);

	if (pAvFrame != NULL)
		av_free(pAvFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	if (screenCleanCb != NULL)
		screenCleanCb();

}
#endif
