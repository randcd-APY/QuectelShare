#ifndef AUD_PLAYER_H
#define AUD_PLAYER_H
#include <QThread>
#include <QList>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}

class AudPlayer : public QThread
{
	Q_OBJECT

private:
	//struct audio_device_t *audio_device;
	//struct pcm *audio_pcm;
	AVCodec* pAudCodec;
	AVCodecContext* pAudCodecCtx;
    struct SwrContext *swrCtr = NULL;
    enum AVSampleFormat out_sample_fmt;
    int out_channel_nb;

	QList<AVPacket*> audPktList;
	bool bStop;

	QMutex mutex;
    QWaitCondition condition; //empty condition
    QWaitCondition fullCon; //empty condition

	AVPacket* lastPkt;
	AVPacket* currentPkt;

public:
	AudPlayer();
	void initAudPlayer(AVCodec* pCodec, AVCodecContext* pCodecCtx);
	void insertAudPkt(AVPacket* pkt);
	//int channel_main();
	void run();
	void stop();

};
#endif
