#ifndef QUEC_RECORDER_THREAD_H
#define QUEC_RECORDER_THREAD_H

#include <QThread>
#include <QList>
#include <QMutex>

#include "ql_omx_video.h"

#include "camera.h"
#include "camera_parameters.h"

using namespace camera;

class QuecRecorderThread : public QThread
{
    Q_OBJECT

private:
	void *eHandle;	
	void onPreviewFrame(ICameraFrame* frame);
	QList<ICameraFrame*> videoBufList;
	QMutex eBufMutex;
	bool eThreadStop;
    void run();

public:
	QuecRecorderThread();
	void* quecRecorderInit(unsigned char *ouputName, int width, int height);  //encoder handle
	void quecRecorderRelease();
	void quecBufInsert(ICameraFrame* iFrame);
};
#endif
