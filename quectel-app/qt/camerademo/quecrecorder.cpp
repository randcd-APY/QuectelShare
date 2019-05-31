#ifndef QUEC_RECORDER_THREAD_CPP
#define QUEC_RECORDER_THREAD_CPP
#include <QtDebug>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "quecrecorder.h"
#include "qcamera_mp4.h"

FILE *h264_file = NULL;
QuecRecorderThread::QuecRecorderThread()
	:eHandle(NULL)
{
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


void QuecRecorderThread::run()
{
	int length = 0;
	ICameraFrame *frame = NULL;
	unsigned char *eBuffer = NULL;
	struct timeval start, end;
	//int count = 0;
	while (!eThreadStop)
	{
		if (videoBufList.isEmpty())	
		{
			usleep(1000); //if buf list is empty, sleep 1ms	
			continue;
		}

		eBufMutex.lock();
		frame = videoBufList.takeFirst();
		eBufMutex.unlock();
		gettimeofday(&start, NULL);
        //eBuffer = omx_encoder_process(eHandle, (unsigned char *)frame->data, 640*480*3/2, frame->s.tv_sec * 1000000 + frame->ts.tv_nsec/1000, &length);
        eBuffer = omx_encoder_process(eHandle, (unsigned char *)frame->data, 640*480*3/2, frame->timeStamp, &length);

		if (eBuffer != NULL && length != 0)
		{
			//qDebug() << "h264 length = " << length << "time = " << (frame->ts.tv_sec%3600) * 1000000 + frame->ts.tv_nsec/1000 << endl;
			//qDebug("time = %lld", (frame->ts.tv_sec%3600) * 1000000 + frame->ts.tv_nsec/1000);
			qcamera_write_video(eBuffer, length, frame->timeStamp);
			//if (h264_file != NULL)
			//		fwrite(eBuffer, 1, length, h264_file);
			//qDebug() << "count = " << count++ << endl;
		}

		
	}
	/*unsigned char *eBuffer = NULL;
	unsigned char *readBuffer = (unsigned char *) malloc(640*480 * 2);
	int nBytesRead;
	int m_srcYuv_fd = open( "/data/misc/camera/640.yuv", O_RDONLY | O_LARGEFILE);
    if (m_srcYuv_fd <= 0)
    {
        //ALOGE("ENCODER TEST: open 640.yuv file\n");
        return ;
    }
    for (int i = 1; i < 2; i++)
    {
        memset(readBuffer, 0, 640*480*3);
        nBytesRead = read(m_srcYuv_fd, readBuffer, 640 * 480 * 3 / 2);
        omx_encoder_process(eHandle, (unsigned char *)frame->buffer, nBytesRead, frame->ts.tv_sec * 1000000 + frame->ts.tv_nsec/1000, &length);
		printf("abing run eHandle addr = %p\n", eHandle);
        //omx_encoder_process(eHandle, readBuffer, nBytesRead, i*1000000/30, &length);
    }*/
}

void* QuecRecorderThread::quecRecorderInit(unsigned char *outputName, int width, int height)
{
    struct EncoderInitParam eInitParam;
	eInitParam.width = width;
    eInitParam.height = height;
    eInitParam.bitrate = 1024*1024;
    eInitParam.framerate = 30;
    eInitParam.ratetype = 0;
    eInitParam.intraPeriod = 30;  // I帧间隔
    eInitParam.minQP = 15;  // QP最小值
    eInitParam.maxQP = 30;  // QP最大值
    eInitParam.codecProfileType = CODECPROFILE_AVC_Baseline;
    eInitParam.codecType = CODEC_AVC;
    eHandle = omx_encoder_init(eInitParam);
	eThreadStop = false;
	h264_file = fopen("/data/misc/camera/cam.h264", "w+");
	qcamera_create_mp4(outputName, width, height);
	/*int length = 0;
	unsigned char *eBuffer = NULL;
	unsigned char *readBuffer = (unsigned char *) malloc(640*480 * 2);
	int nBytesRead;
	int m_srcYuv_fd = open( "/data/misc/camera/640.yuv", O_RDONLY | O_LARGEFILE);
    if (m_srcYuv_fd <= 0)
    {
        return NULL;
    }
    for (int i = 1; i < 100; i++)
    {
        memset(readBuffer, 0, 640*480*3);
        nBytesRead = read(m_srcYuv_fd, readBuffer, 640 * 480 * 3 / 2);
        omx_encoder_process(eHandle, readBuffer, nBytesRead, i*1000000/30, &length);
    }*/

    /*unsigned char *readBuffer = (unsigned char *) malloc(eInitParam.width * eInitParam.height * 2);
    int length;
    int nBytesRead;
    int m_srcYuv_fd = open( "/data/misc/camera/640.yuv", O_RDONLY | O_LARGEFILE);
    if (m_srcYuv_fd <= 0)
    {
        ALOGE("ENCODER TEST: open 640.yuv file\n");
        return NULL;
    }
    for (OMX_S32 i = 1; i < 100; i++)
    {
        memset(readBuffer, 0, eInitParam.width*eInitParam.height*3);
        nBytesRead = read(m_srcYuv_fd, readBuffer, eInitParam.height * eInitParam.width * 3 / 2);
        //result = m_pFile->Read(readBuffer, 480, 640, &nBytesRead, 1, QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m);
        omx_encoder_process(eHandle, readBuffer, nBytesRead, i * 1000000 / 30, &length);
    }*/

    return eHandle;
}

void QuecRecorderThread::quecRecorderRelease()
{
	eThreadStop = true;
	qDebug() << "quecEncodeRelease call stop" << endl;
	this->wait();
	omx_encoder_release(eHandle);
	if (h264_file)
		fclose(h264_file);
	qcamera_close_mp4();

	qDebug() << "close h264 file" << endl;
}

void QuecRecorderThread::quecBufInsert(ICameraFrame * frame)
{
	eBufMutex.lock();
	videoBufList << frame;
	eBufMutex.unlock();
}
#endif
