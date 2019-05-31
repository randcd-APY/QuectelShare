#ifndef VID_PLAYER_H
#define VID_PLAYER_H
#include <QObject>  //包含该文件，使用信号和槽
#include <QThread>
#include <QList>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include "glyuvtorgb.h"
#include "ql_omx_video.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
}
#endif

typedef void (*screen_update_cb)(uint8_t *frameRGB, int width, int height);
typedef void (*screen_clean_cb)();
class VidPlayer : public QThread
{
	Q_OBJECT

private:
	void* pVideoPlayer;
	AVCodec* pVidCodec;
	AVCodecContext* pVidCodecCtx;

	QList<AVPacket*> vidPktList;
    bool bStop;

    QMutex mutex;
    QWaitCondition condition; //empty condition
    QWaitCondition fullCon; //empty condition

    AVPacket* lastPkt;
	AVPacket* currentPkt;
	GLYuvToRgb *convert;

public:
	VidPlayer(void *vp);
	void initVidPlayer(AVCodec* pCodec, AVCodecContext* pCodecCtx);
	void insertVidPkt(AVPacket* pkt);
	void run();
	void stop();

	screen_update_cb screenUpdateCb;
    screen_clean_cb screenCleanCb;  //clean screen, used video over

};
#endif
