#ifndef MUSIC_THREAD_H
#define MUSIC_THREAD_H

#include <QThread>
#include <QMutex>
#include "music_select.h"
#include <QMainWindow>
#include "qlaudio_api.h"
//#include "qahw_api.h"
//#include "qahw_defs.h"

class music_thread : public QThread
{
public:
	music_thread();
	void run();
	QString name; //添加一个 name 对象


	public slots:
	void threadPause();
	void threadStart();
	void threadPR();
	void threadResume();

private:
	QMutex m_mutex;
	QString str;
	bool m_buttonState;
	Audio_Playback audio;

};

#endif // MUSIC_THREAD_H
