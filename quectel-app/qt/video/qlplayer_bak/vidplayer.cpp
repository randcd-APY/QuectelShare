#ifndef VID_PALYER_CPP
#define VID_PALYER_CPP

#include "vidplayer.h"
#include "videoplayer.h"
#include <time.h>
#include <sys/time.h>

#define VID_MAX_FRAME 10

#define FFMPEG_RECODER_VIDEO_BUFFER

FILE *h264_file = NULL;
#define HW_DECODE
#define HW_GPU
//#define C2D_POST
static void NV21_TO_RGB24(unsigned char *yuyv, unsigned char *rgb, int width, int height)
{
    const int nv_start = width * height ;
    int  i, j, index = 0, rgb_index = 0;
    int y, u, v;
    int r, g, b, nv_index = 0;


    for(i = 0; i <  height ; i++)
    {
        for(j = 0; j < width; j ++){
            //nv_index = (rgb_index / 2 - width / 2 * ((i + 1) / 2)) * 2;
            nv_index = i / 2  * width + j - j % 2;

            y = yuyv[rgb_index];
            u = yuyv[nv_start + nv_index ];
            v = yuyv[nv_start + nv_index + 1];


            r = y + 1.4*(v-128);//(140 * (v-128))/100;  //r
            g = y - (0.34*(u-128)) - (0.71*(v-128));//y - (34 * (u-128))/100 - (71 * (v-128))/100; //g
            b = y + 1.77*(u-128);//y + (177 * (u-128))/100; //b

            if(r > 255)   r = 255;
            if(g > 255)   g = 255;
            if(b > 255)   b = 255;
            if(r < 0)     r = 0;
            if(g < 0)     g = 0;
            if(b < 0)     b = 0;

            index = rgb_index % width + (height - i - 1) * width;
            rgb[index * 3+0] = r;
            rgb[index * 3+1] = g;
            rgb[index * 3+2] = b;
            rgb_index++;
        }
    }
}

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

VidPlayer::VidPlayer(void *vp)
{
	pVideoPlayer = vp;
}

void VidPlayer::initVidPlayer(AVCodec* pCodec, AVCodecContext* pCodecCtx)
{
	QMutexLocker locker(&mutex);

	pVidCodec = pCodec;
    pVidCodecCtx = pCodecCtx;
	bStop = false;

#ifdef FFMPEG_RECODER_VIDEO_BUFFER
	h264_file = fopen("/data/misc/camera/vid.h264", "w+");
#endif
}

void VidPlayer::insertVidPkt(AVPacket* vidPkt)
{
	mutex.lock();
	vidPktList << vidPkt;
	mutex.unlock();
	if (vidPktList.size() > VID_MAX_FRAME)
	{
		mutex.lock();
		fullCon.wait(&mutex);
		mutex.unlock();
	}
}

void VidPlayer::stop()
{
	bStop = true;
	mutex.lock();
	vidPktList.clear();  //清除缓存帧
	mutex.unlock();

}

void VidPlayer::run()
{
	int ret;
	int count = 0;
	struct timeval start, end;
	VideoPlayer *vp = (VideoPlayer*)pVideoPlayer;
	double lastClk = 0, currentClk = 0, diffClk;  //记录帧需要的显示时间
	struct timeval lastTime, currentTime, frameTime, frameTimeNext;  //记录当前显示时间
	int diffTime = 0;
	gettimeofday(&lastTime, NULL);
	unsigned char *frame;
#ifndef HW_DECODE
	AVFrame* pAvFrame;
	pAvFrame = av_frame_alloc();
	AVFrame *pFrameRGB;
	SwsContext *img_convert_ctx;
    pFrameRGB = av_frame_alloc();
    uint8_t *out_buffer;

    out_buffer=new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB24, pVidCodecCtx->width, pVidCodecCtx->height)];
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_BGR24, pVidCodecCtx->width, pVidCodecCtx->height);
    int got_frame;
	AVPixelFormat pixfmt;
	switch (pVidCodecCtx->pix_fmt)
	{
		case AV_PIX_FMT_YUVJ420P:
			pixfmt = AV_PIX_FMT_YUV420P;
			break;
		default:
			pixfmt = pVidCodecCtx->pix_fmt;
	}
    img_convert_ctx = sws_getContext(pVidCodecCtx->width, pVidCodecCtx->height, pixfmt, pVidCodecCtx->width, pVidCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
	
#else
	void *pHandle = omx_decoder_init(CODEC_AVC);
	int width, height, stride, sliceHeight;
	uint8_t *out = NULL;
#ifdef C2D_POST
	struct PostProcInitParam postProcInitParam;
    postProcInitParam.srcWidth = 1280;
    postProcInitParam.srcHeight = 720;
    postProcInitParam.dstWidth = 1280;
    postProcInitParam.dstHeight = 720;
    postProcInitParam.inputColorFormat = OMX_COLOR_FormatYUV422PackedPlanar;
    postProcInitParam.outputColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;

    qcamera::QCameraC2dPostProc *qcameraC2dPostProc = (qcamera::QCameraC2dPostProc *)omx_c2dpostproc_init(postProcInitParam);
#endif
	unsigned char *rgb_buffer = (unsigned char *)av_malloc(pVidCodecCtx->width*pVidCodecCtx->height*3);
#endif

#ifdef HW_GPU
	convert = new GLYuvToRgb(pVidCodecCtx->width, pVidCodecCtx->height);
	convert->initializeGL();
#endif

	while(!bStop)
	{
		if (vidPktList.isEmpty())
		{
			usleep(10 * 1000);
			continue;
		}	

		mutex.lock();
		currentPkt = vidPktList.takeFirst();
		mutex.unlock();
		if (currentPkt == NULL || currentPkt->data == NULL)
		{
			continue;
		}

#ifdef HW_DECODE

#ifdef FFMPEG_RECODER_VIDEO_BUFFER
        fwrite(currentPkt->data, 1, currentPkt->size, h264_file);
#endif
	
        out = omx_decoder_process(pHandle, currentPkt->data, currentPkt->size, currentPkt->pts, &width, &height, &stride, &sliceHeight);
        if (out != NULL && screenUpdateCb != NULL)
		{

            //sws_scale(img_convert_ctx, (const uint8_t* const*)out, 0, 0, pVidCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
			//NV21_TO_RGB24(out, rgb_buffer, pVidCodecCtx->width, pVidCodecCtx->height);  //效率太低
			//printf("width = %d  height = %d  stride = %d sliceHeight = %d\n", width, height, stride, sliceHeight);
			//currentClk = currentPkt->pts * av_q2d(vp->getVidStream()->time_base);
			gettimeofday(&currentTime, NULL);
			if (lastClk != 0)
			{
				diffTime = difftimeval(&lastTime, &currentTime); 
				diffClk = currentClk - lastClk;
				//qDebug("diffTime = %d vClk = %f pts = %d lastClk = %f diffClk = %d",diffTime, currentClk, currentPkt->pts, lastClk, (int)(diffClk*1000000));
				if (diffTime < (int)(diffClk*1000000))
				{
					//qDebug("diff time = %d\n", (int)(diffClk*1000000) - diffTime);
					//usleep((int)(diffClk *1000000) - diffTime);
					usleep(35*1000);
				}
			}
			//gettimeofday(&frameTime, NULL);
			frame = convert->slotYuv(out, stride, sliceHeight);
			//gettimeofday(&frameTimeNext, NULL);
			screenUpdateCb(frame, stride, sliceHeight);  //不带yuv->rgb转换 30fps
			lastTime = currentTime;
			lastClk = currentClk;
		}
#else
		ret = avcodec_decode_video2(pVidCodecCtx, pAvFrame, &got_frame, currentPkt);
        if(ret < 0)
        {
           continue ;
        }

        if (got_frame)
        {
            sws_scale(img_convert_ctx, (const uint8_t* const*)pAvFrame->data, pAvFrame->linesize, 0, pVidCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
			if (screenUpdateCb != NULL)
			{
				screenUpdateCb(pFrameRGB->data[0], pVidCodecCtx->width, pVidCodecCtx->height);
			}
		}

#endif

		av_free(currentPkt->data); //不释放 会有内存泄漏
		av_free_packet(currentPkt);
		vp->addAvPacket(currentPkt);
		fullCon.wakeOne();
	}

#ifdef FFMPEG_RECODER_VIDEO_BUFFER
	fclose(h264_file);
#endif

#ifndef HW_DECODE
	sws_freeContext(img_convert_ctx);
	free(out_buffer);
	if (pFrameRGB != NULL)
		av_free(pFrameRGB);

	if (pAvFrame != NULL)
		av_free(pAvFrame);
#else
	omx_decoder_release(pHandle);
	free(rgb_buffer);
#endif
	avcodec_close(pVidCodecCtx);

#ifdef HW_GPU
	delete convert;
#endif

}
#endif
