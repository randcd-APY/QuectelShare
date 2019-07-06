#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <syslog.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <algorithm>

#include "camera.h"
#include "camera_parameters.h"

using namespace camera;

class CameraCallBack : ICameraListener
{

public:
    #define QT_PREVIEW_WIDTH 640
    #define QT_PREVIEW_HEIGHT 480
    #define QT_PREVIEW_SIZE (QT_PREVIEW_WIDTH*QT_PREVIEW_HEIGHT*3/2)
    #define QT_RGBBUF_SIZE (QT_PREVIEW_WIDTH*QT_PREVIEW_HEIGHT*3)

    virtual void onPreviewFrame(ICameraFrame* frame);
    virtual void onVideoFrame(ICameraFrame* frame);
    virtual void onPictureFrame(ICameraFrame* frame);
	int takePicture(uint32_t num);

    int startCameraThread(int camId);
	int setParameters(int qwidth, int qheight);
	void NV21_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height);
	void rotateYUV240SP(char *src,char *des,int width,int height,int camId);
	int dumpToFile(uint8_t* data, uint32_t size, char* name, uint64_t timestamp);
	char rgbBuf[QT_RGBBUF_SIZE];

    ICameraDevice* camera_;
    CameraParams params_;
    ImageSize pSize_;
    ImageSize vSize_;
    ImageSize picSize_;
	bool paintFinish;

	bool isPicDone;
    pthread_cond_t cvPicDone;
    pthread_mutex_t mutexPicDone;
private:
#if 0
    ICameraDevice* camera_;
    CameraParams params_;
    ImageSize pSize_;
	ImageSize vSize_;
	ImageSize picSize_;
#endif

};
