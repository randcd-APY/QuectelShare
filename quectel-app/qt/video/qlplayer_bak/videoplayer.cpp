#ifndef VIDEO_PLAYER_CPP
#define VIDEO_PLAYER_CPP
#include <QDebug>
#include <stdio.h>
#include "videoplayer.h"
#include <time.h>
#include <sys/time.h>

#define __STDC_CONSTANT_MACROS

//char* filename = "tc10.264";
//char* filename = "1080p.mp4";
//char* filename = "720p.mp4";

#define USE_H264BSF

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

VideoPlayer::VideoPlayer():pFormatCtx(NULL),
	isStop(false), 
	bAudPlay(false),
	audIndex(-1),
	vidIndex(-1)
{
	qDebug() <<"create VideoPlayer\n" << endl;
}

void VideoPlayer::initVideoPlayer()
{
	QMutexLocker locker(&mutex);
	vidPlayer = new VidPlayer(this);
	audPlayer = new AudPlayer();

	for (int i = 0; i < MAX_AV_PACKET; i++)
	{
		AVPacket *pkt = av_packet_alloc();
		emptyAvPktList << pkt;
	}

}

void VideoPlayer::_deinitVideoPlayer()
{
	for (int i = 0; i < emptyAvPktList.size(); i++)
	{
		AVPacket *pkt = emptyAvPktList.takeFirst();
		av_packet_free(&pkt);
	}

	delete vidPlayer;
	delete audPlayer;
}

void VideoPlayer::setVideoName(const char *videoName)
{
	memset(mVideoName, 0, sizeof(mVideoName));
	memcpy(mVideoName, videoName, strlen(videoName));
}

void VideoPlayer::setVidUpdateCb(screen_update_cb func)
{
	vidPlayer->screenUpdateCb = func;	
}

void VideoPlayer::setVidCleanCb(screen_clean_cb func)
{
	vidPlayer->screenCleanCb = func;
}

void VideoPlayer::stop()
{
	printf("videoplayer stop set\n");
	isStop = true;
}

bool VideoPlayer::_initVideoParam()
{
	pVidParam = pFormatCtx->streams[vidIndex]->codecpar; //得到一个指向视频流的上下文指针
	if (pVidParam == NULL)
	{
		qDebug("av param is null");
	}
    AVCodec* pVidCodec = avcodec_find_decoder(pVidParam->codec_id); //到该格式的解码器
    AVCodecContext* pVidCodecCtx = avcodec_alloc_context3(pVidCodec);
	avcodec_parameters_to_context(pVidCodecCtx, pVidParam);

	if (pVidCodecCtx == NULL) {
		qFatal("Cant't find the codec !"); //寻找解码器
		return false;
	}
	if (avcodec_open2(pVidCodecCtx, pVidCodec, NULL) < 0) { //打开解码器
		qFatal("Can't open the codec !");
		return false;
	}

	vidPlayer->initVidPlayer(pVidCodec, pVidCodecCtx);

	vidPlayer->start();  //开始解码并显示
	
	return true;

} 

bool VideoPlayer::_initAudioParam()
{
	AVCodecParameters* pAudParam = pFormatCtx->streams[audIndex]->codecpar; //得到一个指向视频流的上下文指针
   	pAudCodec = avcodec_find_decoder(pAudParam->codec_id); //到该格式的解码器
   	pAudCodecCtx = avcodec_alloc_context3(pAudCodec);
	avcodec_parameters_to_context(pAudCodecCtx, pAudParam);
	if (pAudCodec != NULL && avcodec_open2(pAudCodecCtx,pAudCodec,NULL) < 0) {
		qFatal("Can't open the audio decode");
		return false;
	}


	audPlayer->initAudPlayer(pAudCodec, pAudCodecCtx);
	audPlayer->start();
    	
	bAudPlay = true;
	return true;
    //16bit 44100 PCM 数据

}

void VideoPlayer::addAvPacket(AVPacket* avPkt)
{
	mutex.lock();
	emptyAvPktList << avPkt;
	mutex.unlock();
}

AVStream* VideoPlayer::getVidStream()
{
	return pVidStream;
}

void VideoPlayer::run()
{
	//added by tommy for redirection output
	//freopen("/dev/ttyHSL0","w",stdout);
	
	qDebug() << "video play run\n" << endl;

    int ret = -1;

	av_register_all();
    pFormatCtx = avformat_alloc_context();

	qDebug() << mVideoName << endl;

    if (avformat_open_input(&pFormatCtx, mVideoName, NULL, NULL) != 0) { //检查文件头部
        qFatal("Can't find the stream!");
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) { //查找流信息
        qFatal("Can't find the stream information !");
    }

    for (int i=0; i < pFormatCtx->nb_streams; ++i) //遍历各个流，找到第一个视频流,并记录该流的编码信息
    {
    	AVStream* stream = pFormatCtx->streams[i];
		if (vidIndex == -1 && stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {  //视频中可能有多个视频流，我们默认取第一个，第一个能播放的概率较大
			vidIndex = i;
			pVidStream = stream;
			continue;
		}

		if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audIndex = i;
			pAudStream = stream;
			continue;
		}
	}

	if (vidIndex == -1) {  //没有视频直接退出 不单独播放音频 
		qFatal("Don't find a video stream !");
		return ;
	}
	
	_initVideoParam();
	
	if (audIndex != -1)
	{
		_initAudioParam();	
	}

	int vidcount = 0;
	int audcount = 0;
	bool isEnd = false;
	AVPacket *pAvPacket;

#ifdef USE_H264BSF
	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif

	while (!isStop)
    {
		if (emptyAvPktList.isEmpty())
		{
			usleep(10);	  //休眠等待
			continue;
		}
		
		mutex.lock();
		pAvPacket = emptyAvPktList.takeFirst();
		mutex.unlock();

        if(av_read_frame(pFormatCtx, pAvPacket)>=0)
        {
            if(pAvPacket->stream_index==vidIndex)
            {

				double vClk = pAvPacket->pts * av_q2d(pVidStream->time_base);
#ifdef USE_H264BSF
				av_bitstream_filter_filter(h264bsfc, pVidStream->codec, NULL, &pAvPacket->data, &pAvPacket->size, pAvPacket->data, pAvPacket->size, pAvPacket->flags & AV_PKT_FLAG_KEY);
#endif
				
				vidPlayer->insertVidPkt(pAvPacket);
            } else if (pAvPacket->stream_index == audIndex){

				av_free_packet(pAvPacket);
				addAvPacket(pAvPacket);
			}

        }
        else
        {
			isEnd = true;
            break;
        }
	
    }

#ifdef USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif

	audPlayer->stop();
	audPlayer->wait();

	vidPlayer->stop();
	vidPlayer->wait();
	avformat_close_input(&pFormatCtx);
	avformat_free_context(pFormatCtx);

	_deinitVideoPlayer();	

	if (isEnd)
	{
		vidPlayer->screenCleanCb();
	}
}
#endif
