#ifndef AUD_PALYER_CPP
#define AUD_PALYER_CPP

#include "audplayer.h"

#define AUD_MAX_FRAME 10
//#define FFMPEG_OUT_AUDIO
#define MAX_AUDIO_FRAME_SIZE 48000

/*
static inline int audio_control_open(const struct hw_module_t* module, struct audio_device_t** device) {
    return module->methods->open(module,MSM_DUAL_AUDIO_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
}
int AudPlayer::channel_main() {
    int ret = -1;
	printf("Audio Channel Test Start\n");

	audio_module_t* module;
    if (hw_get_module(MSM_DUAL_AUDIO_HARDWARE_MODULE_ID, (hw_module_t const**)&module) == 0) {
        printf("MSM Audio module found\n");
        if (audio_control_open(&module->common, &audio_device) == 0) {
    	    printf("Audio device Opened Success\n");
        }else{
			printf("Audio device Opened Failed\n");
			return 0;
		}
    }else{
		printf("Not find Audio HAL err:[%s]\n",strerror(errno));
		return 0;
	}

	printf("Audio Channel Test Start\n");
	return 0;
}*/

AudPlayer::AudPlayer()
{
}

void AudPlayer::initAudPlayer(AVCodec* pCodec, AVCodecContext* pCodecCtx)
{
	QMutexLocker locker(&mutex);

	pAudCodec = pCodec;
    pAudCodecCtx = pCodecCtx;
	
//  //解压数据
    swrCtr =swr_alloc();

    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = pAudCodecCtx->sample_fmt;
    //输出采样格式16bit PCM
    //输入采样率
    int in_sample_rate = pAudCodecCtx->sample_rate;
    //输出采样率
    int out_sample_rate = 44100;
    //获取输入声道布局 如立体声 左声道
    uint64_t in_ch_layout = pAudCodecCtx->channel_layout;
    //输出声道 为立体声
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

	out_sample_fmt = AV_SAMPLE_FMT_S16;

	printf("in_sample_rate = %d out_ch_layout = %d\n", in_sample_rate, out_ch_layout);
	swr_alloc_set_opts(swrCtr,out_ch_layout,out_sample_fmt,out_sample_rate,in_ch_layout,in_sample_fmt,in_sample_rate,0,NULL);
    //初始化
    swr_init(swrCtr);

    //输出的声道个数
    out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

	bStop = false;

	//channel_main();
	//audio_device->init_channel(0);
}

void AudPlayer::insertAudPkt(AVPacket* audPkt)
{
	if (audPktList.isEmpty())
	{
		audPktList << audPkt;
		mutex.lock();
		condition.wakeOne();
		
		mutex.unlock();
	} else {
#ifdef AUD_FULL_CONT
		if (audPktList.size() > AUD_MAX_FRAME)
		{
			mutex.lock();
			fullCon.wait(&mutex);
			mutex.unlock();
		}
#endif
		audPktList << audPkt;
	}

}

void AudPlayer::stop()
{
	bStop = true;
	qDebug("audio stop");
	audPktList.clear();  //清除缓存帧
	mutex.lock();
	condition.wakeOne();
	qDebug("audio stop and wake");
	mutex.unlock();
}

void AudPlayer::run()
{
	int ret, got_frame;
    uint8_t* audio_buffer;

	AVFrame* pFrame;
	pFrame = av_frame_alloc();
	audio_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*4);
#ifdef FFMPEG_OUT_AUDIO
	FILE *audio_file = fopen("/data/misc/camera/test.pcm", "w+");
	if (audio_file == NULL)
	{
		qFatal("open audio file fail\n");
	}
#endif


	qDebug("audio player run bStop = %d\n", bStop);
	while(!bStop)
	{
		if (audPktList.isEmpty())
		{
			mutex.lock();
			condition.wait(&mutex);
			mutex.unlock();

			if (bStop)  //check stop command
				break;
		}	

		//qDebug("run audPktList size = %d\n", audPktList.size());
		lastPkt = currentPkt;
		currentPkt = audPktList.takeFirst();
		if (currentPkt == NULL)
		{
			continue;
		}

		//qDebug("audio currentPkt dts = %lld, pts = %lld", currentPkt->dts, currentPkt->pts);
		ret = avcodec_decode_audio4(pAudCodecCtx,pFrame,&got_frame, currentPkt);	
		if (ret < 0)
		{
			continue;
		}

		//printf("got audio got_frame = %d pFrame->nb_samples = %d\n", got_frame, pFrame->nb_samples);
		if (got_frame)
		{
			swr_convert(swrCtr,&audio_buffer,MAX_AUDIO_FRAME_SIZE*4, (const uint8_t **)pFrame->data, pFrame->nb_samples);
			int out_buffer_size =av_samples_get_buffer_size(NULL, out_channel_nb, pFrame->nb_samples, out_sample_fmt, 1);

			/*if(pFrame->nb_samples < 288){
					//framecnt++;
				if(audio_pcm!=NULL){
					audio_device->pcm_close(audio_pcm);
				}
				audio_pcm = audio_device->init_pcm(0, 0, 0, 0, 0, pFrame->nb_samples, 0);
				audio_device->write_buffer(audio_pcm, (char*)audio_buffer, out_buffer_size);
				audio_device->pcm_close(audio_pcm);
				audio_pcm = audio_device->init_pcm(0, 0, 0, 0, 0, 0, 0);
			}else{
				//if(frame->nb_samples < 288){
					//pp = audio_device->init_pcm(0, 0, 0, 0, 0, frame->nb_samples, 0);
				//}else{
				if (audio_pcm == NULL)
					audio_pcm = audio_device->init_pcm(0, 0, 0, 0, 0, pFrame->nb_samples, 0);
				audio_device->write_buffer(audio_pcm, (char*)audio_buffer, out_buffer_size);
			}*/
#ifdef FFMPEG_OUT_AUDIO 
			fwrite(audio_buffer, 1, out_buffer_size, audio_file);
#endif
		}
	
		av_packet_unref(currentPkt);
#ifdef AUD_FULL_CONT
		fullCon.wakeOne();
#endif
	}

#ifdef FFMPEG_OUT_AUDIO
	fclose(audio_file);
#endif

	if (pFrame != NULL)
		av_free(pFrame);
	avcodec_close(pAudCodecCtx);

	/*if (audio_device != NULL)
	{
		audio_device->stop_play();
		audio_device->release_channel(0);
		if(audio_pcm!=NULL)
			audio_device->pcm_close(audio_pcm);
	}*/
}
#endif
