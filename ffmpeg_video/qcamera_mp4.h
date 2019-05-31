#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"

typedef /* @abstract@ */ unsigned char  bool;

struct mp4_muxer_param
{
	int waitkey;
	int vi;
	int ptsInc;
	int width;
	int height;
	bool gotHeader;
	unsigned char outputName[256];	
};

int qcamera_create_mp4(unsigned char *outputName, int width, int height);

void qcamera_write_video(void* data, int len, double nTimeStamp);

void qcamera_close_mp4();

#ifdef __cplusplus
}
#endif
