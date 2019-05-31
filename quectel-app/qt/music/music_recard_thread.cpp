#include "music_recard_thread.h"

music_recard_thread::music_recard_thread()
{
    m_buttonState = true;
}

void music_recard_thread::threadResume()
{
    this->m_mutex.unlock();
    this->m_buttonState = true;
}

void music_recard_thread::threadPR()
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

void music_recard_thread::threadPause()
{
    this->m_mutex.lock();
    this->m_buttonState = false;
}

void music_recard_thread::threadStart()
{
    this->start();

}

void music_recard_thread::threadStop()
{
	this->quit();
	this->wait();
}

void music_recard_thread::run()
{
    struct Audio_Record record;
    record.device = DefaultInputDevice;
    record.channel = DefaultChannel;
    record.rate   = DefaultRecordRate;
    QL_Audio_Record(record);
}
