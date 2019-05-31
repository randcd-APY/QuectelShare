#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H
#include "vidplayer.h"
#include "audplayer.h"

#define MAX_AV_PACKET 20
class VideoPlayer : public QThread
{
	Q_OBJECT

private:
    bool _initAudioParam();
    bool _initVideoParam();
    void _deinitVideoPlayer();

private:
///container
	AVFormatContext* pFormatCtx;
	QList<AVPacket*> emptyAvPktList;  //use to add video and audio packet
	QMutex mutex;

///video
	VidPlayer* vidPlayer;
	AVCodecParameters* pVidParam;
	AVStream *pVidStream;

	int vidIndex;
///audio
	AudPlayer* audPlayer;
	AVCodecParameters* pAudParam;
	AVStream *pAudStream;
	AVCodec* pAudCodec;
	AVCodecContext* pAudCodecCtx;
	bool isStop;
	bool bAudPlay;
	int audIndex;

	char mVideoName[128];

public:
    VideoPlayer();
    void initVideoPlayer();
    void addAvPacket(AVPacket* avPkt);
    AVStream* getVidStream();
    void run();
    void setVideoName(const char *videoName);

    void setVidUpdateCb(screen_update_cb func);
    void setVidCleanCb(screen_clean_cb func);
    void stop();
};
#endif
