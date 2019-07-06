#include "music_thread.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

music_thread::music_thread()
{
	audio.rate = DefualtPlayRate;
	audio.device = DefaultOutputDevice;
	audio.channel = DefaultChannel;
	audio.volume = 10;
	m_buttonState = true;
}

void music_thread::threadStart()
{
	MainWindow * main_win = new MainWindow();
	str = main_win->Get_port();
    this->start();
    delete main_win;
}

void music_thread::threadPause()
{
	//qDebug(QString("pause :%1").arg(m_buttonState));
    //this->m_mutex.lock();
	this->m_buttonState = false;
    QL_Audio_stopback();
    //this->terminate();
    //this->wait();
	//qDebug()<<QString("pause");
}

void music_thread::threadResume()
{
	//qDebug()<<QString("resume :%1").arg(m_buttonState);
    //this->m_mutex.unlock();
	this->m_buttonState = true;
    QL_Audio_startback();
	//qDebug()<<QString("resume");
}

void music_thread::threadPR()
{
	if (m_buttonState)
	{
		threadPause();
	}
	else
	{
		threadResume();
	}

}

void music_thread::run()
{
	QString x("wav");
	QString z("mp3");
	QString y = str.right(3);
	if (QString::compare(y, x, Qt::CaseSensitive) == 0)
		audio.filetype = Wav;
	if (QString::compare(y, z, Qt::CaseSensitive) == 0)
	{
		audio.filetype = Mp3;
	}
	QByteArray ba;
	ba = str.toLatin1();
	QL_Audio_Playback(audio, ba.data());
}
