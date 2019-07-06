#include "music_recard_thread.h"
#include "mainwindow.h"

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
    MainWindow * main_win = new MainWindow();
    music_path = main_win->get_dir();
    if (music_path == NULL)
    {
        QMessageBox::information(NULL, "Title", "please add music directory", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        this->start();
    }
    delete main_win;
}

void music_recard_thread::threadStop()
{
    QL_Record_Stop();
    //this->terminate();
    //this->wait();
}

void music_recard_thread::run()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str_time = time.toString("yyyy-MM-dd_hh:mm:ss_ddd"); //设置显示格式
    QString str_file = music_path + "/" + str_time;
    QByteArray ba;
    ba = str_file.toLatin1();
    struct Audio_Record record;
    record.device = DefaultInputDevice;
    record.channel = DefaultChannel;
    record.rate = DefaultRecordRate;
    QL_Audio_Record(record, ba.data());

}
