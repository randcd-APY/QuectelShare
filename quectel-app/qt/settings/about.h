#ifndef ABOUT_H
#define ABOUT_H

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QProcess>

class About;

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    void setAbout(class About *about);

protected:
    virtual void run();

signals:
    void resultReady(const QString &s);

private:
    About *p_about;
};

class About : public QObject
{
    Q_OBJECT
    //属性声明
    Q_PROPERTY(QString kernelVersion READ getKernelVersion WRITE setKernelVersion NOTIFY kernelVersionChanged)
    Q_PROPERTY(QString gccVersion READ gccVersion WRITE setGccVersion)
    Q_PROPERTY(QString compileTime READ compileTime WRITE setCompileTime)
    Q_PROPERTY(QString bpVersion READ bpVersion WRITE setBpVersion NOTIFY bpVersionChanged)
    Q_PROPERTY(QString IMEI READ getIMEI WRITE setIMEI NOTIFY IMEIChanged)
    Q_PROPERTY(QString MEID READ getMEID WRITE setMEID NOTIFY MEIDChanged)
    Q_PROPERTY(QString IMEI2 READ getIMEI2 WRITE setIMEI2 NOTIFY IMEI2Changed)
    Q_PROPERTY(QString WIFIMAC READ getWIFIMAC WRITE setWIFIMAC NOTIFY WIFIMACChanged)
    Q_PROPERTY(QString BTMAC READ getBTMAC WRITE setBTMAC NOTIFY BTMACChanged)
    Q_PROPERTY(QString memTotal READ memTotal WRITE setMemTotal NOTIFY memTotalChanged)
    Q_PROPERTY(QString memAvailable READ memAvailable WRITE setMemAvailable NOTIFY memAvailableChanged)
    Q_PROPERTY(QString flashSize READ flashSize WRITE setFlashSize NOTIFY flashSizeChanged)
    Q_PROPERTY(QString powerStatus READ powerStatus WRITE setPowerStatus NOTIFY powerStatusChanged)
    Q_PROPERTY(QString batteryCapacity READ batteryCapacity WRITE setBatteryCapacity NOTIFY batteryCapacityChanged)
    Q_PROPERTY(QString sdTotalSize READ sdTotalSize WRITE setSDTotalSize NOTIFY sdTotalSizeChanged)
    Q_PROPERTY(QString sdAvailableSize READ sdAvailableSize WRITE setSDAvailableSize NOTIFY sdAvailableSizeChanged)
    Q_PROPERTY(QString rtcTime READ rtcTime WRITE setRtcTime NOTIFY rtcTimeChanged)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)

public:
    explicit About(QObject *parent = nullptr);

    QString getRtcTime(void);

    QString getKernelVersion();
    void setKernelVersion(QString kernelVersion);

    QString gccVersion();
    void setGccVersion(QString gccVersion);

    QString compileTime();
    void setCompileTime(QString compileTime);

    QString bpVersion();
    void setBpVersion(QString bpVersion);

    int getbpVersion4Thread();

    QString getIMEI();
    void setIMEI(QString IMEI);

    QString getIMEI2();
    void setIMEI2(QString IMEI2);

    QString getMEID();
    void setMEID(QString MEID);

    QString getWIFIMAC();
    void setWIFIMAC(QString WIFIMAC);

    QString getBTMAC();
    void setBTMAC(QString BTMAC);

    int getIMEIandMEID4Thread();

    int getIMEI24Thread();

    int getWIFIMAC4Thread();

    int getBTMAC4Thread();

    QString memTotal();
    void setMemTotal(QString memTotal);

    QString memAvailable();
    void setMemAvailable(QString memAvailable);
    int getMemInfos4Thread();

    QString flashSize();
    void setFlashSize(QString flashSize);

    QString powerStatus();
    void setPowerStatus(QString powerStatus);

    QString batteryCapacity();
    void setBatteryCapacity(QString batteryCapacity);
    int getPowerInfos4Thread();

    QString sdTotalSize();
    void setSDTotalSize(QString sdTotalSize);

    QString sdAvailableSize();
    void setSDAvailableSize(QString sdAvailableSize);
    int getSDInfos4Thread();

    QString rtcTime();
    void setRtcTime(QString rtcTime);

    int brightness();
    void setBrightness(int brightness);
    int getBrightness4Thread();

signals:
    void bpVersionChanged(QString bpVersion);

    void kernelVersionChanged(QString kernelVersion);

    void IMEIChanged(QString IMEI);

    void MEIDChanged(QString IMEI);

    void IMEI2Changed(QString IMEI2);

    void WIFIMACChanged(QString WIFIMAC);

    void BTMACChanged(QString BTMAC);

    void memTotalChanged(QString memTotal);

    void memAvailableChanged(QString memAvailable);

    void flashSizeChanged(QString flashSize);

    void powerStatusChanged(QString powerStatus);

    void batteryCapacityChanged(QString batteryCapacity);

    void sdTotalSizeChanged(QString sdTotalSize);

    void sdAvailableSizeChanged(QString sdAvailableSize);

    void rtcTimeChanged(QString rtcTime);

    void brightnessChanged(int brightness);
public slots:
    void timerout_update();
//    void slotUpdate_bpVersion(QString bpVersion);

private:
    QString m_kernelVersion;
    QString m_gccVersion;
    QString m_compileTime;
    QString m_bpVersion;
    QString m_IMEI;
    QString m_MEID;
    QString m_IMEI2;
    QString m_WIFIMAC;
    QString m_BTMAC;
    QString m_memTotal;
    QString m_memAvailable;
    QString m_flashSize;
    QString m_powerStatus;
    QString m_batteryCapacity;
    QString m_sdTotalSize;
    QString m_sdAvailableSize;
    QString m_rtcTime;
    int m_brightness;

    QFile *p_rtcFile;
    QTimer *p_timer;
    WorkerThread *p_workerThread;
};

#endif // ABOUT_H
