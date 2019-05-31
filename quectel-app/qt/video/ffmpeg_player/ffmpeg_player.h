#ifndef FFMPEG_PLAYER_H
#define FFMPEG_PLAYER_H
#include <QThread>
typedef void (*screen_update_cb)(uint8_t *frameRGB, int width, int height);
typedef void (*screen_clean_cb)();
class VideoPlayer : public QThread
{
    Q_OBJECT
private:
    void run();
	char mVideoName[128];
	bool isStop;

public:
    VideoPlayer();
	screen_update_cb screenUpdateCb;
	screen_clean_cb screenCleanCb;  //clean screen, used video over
	void setVideoName(const char *videoName);

	void stop();
};
#endif
