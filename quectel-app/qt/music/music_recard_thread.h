#ifndef MUSIC_RECARD_THREAD_H
#define MUSIC_RECARD_THREAD_H

#include <QThread>
#include <QMutex>
#include "music_select.h"
#include <QMainWindow>
#include "qlaudio_api.h"
#include <QTime>

class music_recard_thread : public QThread
{
public:
    music_recard_thread();
    void run();

public slots:
    void threadPause();
    void threadStart();
    void threadPR();
    void threadResume();
	void threadStop();

private:
    QMutex m_mutex;
    bool m_buttonState;
    QString music_path;
};

#endif // MUSIC_RECARD_THREAD_H
