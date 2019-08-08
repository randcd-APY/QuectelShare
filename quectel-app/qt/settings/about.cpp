#include "about.h"

#include <ql_in.h>
#include <ql_power.h>
#include <stdio.h>
#include <sys/statfs.h>
#include <unistd.h>

About::About(QObject *parent) : QObject(parent)
{
    m_kernelVersion = "Linux version: 3.18.0";
    m_gccVersion = "GCC version: 6.4.1";
    m_compileTime = "UTC 2019";

    p_workerThread = new WorkerThread();
    p_workerThread->setAbout(this);
    connect(p_workerThread, &WorkerThread::finished, p_workerThread, &QObject::deleteLater);
//    connect(p_workerThread, &WorkerThread::resultReady, this, &About::slotUpdate_bpVersion);
    p_workerThread->start();

#if 1
    QFile file("/proc/version");
    if (!file.exists()) {
        qErrnoWarning("/proc/version is not exists!");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qErrnoWarning("/proc/version open failed!");
        return;
    }

    QByteArray str = file.readAll();
    QString qStr = QString(str);
#else
    QString qStr = QString("Linux version 3.18.71 (oe-user@oe-host) (gcc version 6.4.0 (GCC) ) #1 SMP PREEMPT Wed Jul 3 10:36:12 UTC 2019");
#endif
    QRegExp rx("");
    int pos;
    QString newStr;
    if (qStr.contains("Linux version ")) {
        rx.setPattern("\\d+.\\d+.\\d+");
        pos = rx.indexIn(qStr, 0);
        newStr = qStr.mid(pos);

        rx.setPattern("\\s");
        pos = rx.indexIn(newStr, 0);
        m_kernelVersion = QString("Linux version: ");
        m_kernelVersion.append(newStr.mid(0, pos + 1));
    //    qDebug(m_kernelVersion.toUtf8());
    } else {
        qErrnoWarning("/proc/version dosen't contains Linux version");
    }


    //get gcc version
    if (qStr.contains("gcc version ")) {
        rx.setPattern("gcc version ");
        pos = rx.indexIn(qStr, 0);
        newStr = qStr.mid(pos);

        rx.setPattern("\\d.\\d.\\d");
        pos = rx.indexIn(newStr, 0);
        newStr = newStr.mid(pos);

        rx.setPattern("\\s");
        pos = rx.indexIn(newStr, 0);

        m_gccVersion = QString("GCC version: ");
        m_gccVersion.append(newStr.mid(0, pos + 1));
    } else {
        qErrnoWarning("/proc/version dosen't contains gcc version");
    }

    //get compile time
    if (qStr.contains("SMP PREEMPT ")) {
        rx.setPattern("SMP PREEMPT ");
        pos = rx.indexIn(qStr, 0);
        newStr = qStr.mid(pos + 12);
//        qDebug(newStr.toUtf8());

        m_compileTime = QString("Compile Time: ");
        m_compileTime.append(newStr.trimmed());
//        qDebug(m_compileTime.toUtf8());
    } else if (qStr.contains("SMP ")) {
        rx.setPattern("SMP ");
        pos = rx.indexIn(qStr, 0);
        newStr = qStr.mid(pos + 4);
//        qDebug(newStr.toUtf8());

        m_compileTime = QString("Compile Time: ");
        m_compileTime.append(newStr);
//        qDebug(m_compileTime.toUtf8());
    } else {
        qErrnoWarning("/proc/version dosen't contains SMP [PREEMPT]");
    }

    /* flash */
    file.setFileName("/proc/quec_emmc_size");
    if (!file.exists()) {
        qErrnoWarning("/proc/quec_emmc_size is not exists!");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qErrnoWarning("/proc/quec_emmc_size open failed!");
        return;
    }

    str = file.readAll();
    qStr = QString(str);
    m_flashSize = "Flash Size: ";
    m_flashSize.append(qStr.trimmed());

    p_rtcFile = new QFile("/sys/class/rtc/rtc0/time");
    if (!p_rtcFile->exists()) {
        qErrnoWarning("/sys/class/rtc/rtc0/time is not exists!");
        p_rtcFile = nullptr;
        return;
    }

    if (!p_rtcFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qErrnoWarning("/sys/class/rtc/rtc0/time open failed!");
        return;
    }

    str = p_rtcFile->readAll();
    qStr = "RTC Time: ";
    qStr.append(QString(str).trimmed());
    m_rtcTime = qStr;
    qDebug() << "rtc Time: " << qStr;
    p_timer = new QTimer();
    connect(p_timer, SIGNAL(timeout()), this, SLOT(timerout_update()));
    p_timer->start(1000);
}

QString About::kernelVersion()
{
    return m_kernelVersion;
}

void About::setKernelVersion(QString kernelVersion)
{
    m_kernelVersion = kernelVersion;
}

QString About::gccVersion()
{
    return m_gccVersion;
}

void About::setGccVersion(QString gccVersion)
{
    m_gccVersion = gccVersion;
}

QString About::compileTime()
{
    return m_compileTime;
}

void About::setCompileTime(QString compileTime)
{
    m_compileTime = compileTime;
}

#define ATC_REQ_CMD_MAX_LEN     512
#define ATC_RESP_CMD_MAX_LEN    512
QString About::bpVersion()
{
    return m_bpVersion;
}

void About::setBpVersion(QString bpVersion)
{
    if (m_bpVersion == bpVersion)
        return;

    m_bpVersion = bpVersion;
    emit bpVersionChanged(m_bpVersion);
}

int About::getbpVersion4Thread()
{
    int ret = 0;
    static int flag = 10;
    static atc_client_handle_type h_atc = 0;
    char atc_cmd_req[ATC_REQ_CMD_MAX_LEN] = {0};
    char atc_cmd_resp[ATC_RESP_CMD_MAX_LEN] = {0};

    if (flag == 10) {
        ret = QL_ATC_Client_Init(&h_atc);
        printf("QL_ATC_Client_Init ret=%d with h_atc=0x%x\n", ret, h_atc);
    }

    if (flag < 0)
        return 0;
    else {
        flag--;
    }

    memset(atc_cmd_req,  0, sizeof(atc_cmd_req));
    memset(atc_cmd_resp, 0, sizeof(atc_cmd_resp));

    strcpy((char *)atc_cmd_req, "ati");
    ret = QL_ATC_Send_Cmd(h_atc, atc_cmd_req, atc_cmd_resp, ATC_RESP_CMD_MAX_LEN);
    printf("QL_ATC_Send_Cmd \"%s\" ret=%d with resp=\n%s\n", atc_cmd_req, ret, atc_cmd_resp);
    if (ret < 0) {
        setBpVersion("BP Version: Unknown");
        return ret;
    }

    QString qStr = QString(atc_cmd_resp);
    QStringList list = qStr.split("\r\n", QString::QString::SkipEmptyParts);

    QRegExp rx;
    QString newStr;
    int pos;
    for (int i = 0; i < list.size();i++) {
        qDebug() << list.at(i);
        if (list.at(i).contains("Revision:")) {

            QString rxStr = QString("Revision:(.*)");
            rx.setPattern(rxStr);
            pos = list.at(i).indexOf(rx);
            if (pos >= 0) {
                newStr = "BP Version: ";
                newStr.append(rx.cap(1).trimmed());
                setBpVersion(newStr);
            }
            break;
        }
    }

    return 0;
}

QString About::getIMEI()
{
    return m_IMEI;
}

void About::setIMEI(QString IMEI)
{
    if (m_IMEI == IMEI)
        return;

    m_IMEI = IMEI;
    emit IMEIChanged(m_IMEI);
}

QString About::getMEID()
{
    return m_MEID;
}

void About::setMEID(QString MEID)
{
    if (m_MEID == MEID)
        return;

    m_MEID = MEID;
    emit MEIDChanged(m_MEID);
}

int About::getIMEIandMEID4Thread()
{
    int ret = 0;
    dm_client_handle_type h_dm = 0;

    ret = QL_MCM_DM_Client_Init(&h_dm);
//    printf("QL_MCM_DM_Client_Init ret = %d with h_dm= %d\n", ret, h_dm);
    ql_dm_device_serial_numbers_t dm_device_serial_numbers;
    memset(&dm_device_serial_numbers, 0, sizeof(dm_device_serial_numbers));
    ret = QL_MCM_DM_GetSerialNumbers(h_dm,&dm_device_serial_numbers);
    if (ret < 0) {
//        setIMEI("");
//        setMEID("");
        printf("QL_MCM_DM_GetSerialNumbers fail ret = %d\n", ret);
        return ret;
    } else {
//        printf("SerialNumbers  imei:%s      meid:%s\n",
//               dm_device_serial_numbers.imei,dm_device_serial_numbers.meid);
        setIMEI("IMEI: " + QString(dm_device_serial_numbers.imei));
        setMEID("MEID: " + QString(dm_device_serial_numbers.meid));
    }

    return ret;
}

QString About::memTotal()
{
    return m_memTotal;
}

void About::setMemTotal(QString memTotal)
{
    if (m_memTotal == memTotal)
        return;

    m_memTotal = memTotal;
    emit memTotalChanged(m_memTotal);
}

QString About::memAvailable()
{
    return m_memAvailable;
}

void About::setMemAvailable(QString memAvailable)
{
    if (m_memAvailable == memAvailable)
        return;

    m_memAvailable = memAvailable;
    emit memAvailableChanged(m_memAvailable);
}

int About::getMemInfos4Thread()
{
    QFile file;

    /* Memory */
    file.setFileName("/proc/meminfo");
    if (!file.exists()) {
        qErrnoWarning("/proc/meminfo is not exists!");
        return -1;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qErrnoWarning("/proc/version open failed!");
        return -2;
    }

    QByteArray str = file.readAll();
    QString qStr = QString(str);
    QStringList list = qStr.split("\n", QString::QString::SkipEmptyParts);
    for (int i = 0; i < list.size();i++) {
        if (list.at(i).contains("MemTotal:")) {
            setMemTotal(list.at(i));
            break;
        }
    }

    for (int i = 0; i < list.size();i++) {
        if (list.at(i).contains("MemAvailable:")) {
            setMemAvailable(list.at(i));
            break;
        }
    }

    return 0;
}

QString About::flashSize()
{
    return m_flashSize;
}

void About::setFlashSize(QString flashSize)
{
    if (m_flashSize == flashSize)
        return;
    m_flashSize = flashSize;
    emit flashSizeChanged(m_flashSize);
}

QString About::powerStatus()
{
    return m_powerStatus;
}

void About::setPowerStatus(QString powerStatus)
{
    if (m_powerStatus == powerStatus)
        return;
    m_powerStatus = powerStatus;
    emit powerStatusChanged(m_powerStatus);
}

QString About::batteryCapacity()
{
    return m_batteryCapacity;
}

void About::setBatteryCapacity(QString batteryCapacity)
{
    if (m_batteryCapacity == batteryCapacity)
        return;
    m_batteryCapacity = batteryCapacity;
    emit batteryCapacityChanged(m_batteryCapacity);
}

int About::getPowerInfos4Thread()
{
    /* power */
    char buf[512];
    int ret = QL_Power_Info("battery", buf);
    if (ret < 0) {
        qErrnoWarning("QL_Power_Info failed!");
        return ret;
    }
//    fprintf(stdout, "%s\n", buf);
    int pos;
    QRegExp rx;
    QString newStr;
    QString qStr = QString(buf);
    QStringList list = qStr.split("\n", QString::QString::SkipEmptyParts);
    for (int i = 0; i < list.size();i++) {
        if (list.at(i).contains("POWER_SUPPLY_STATUS")) {
            newStr = QString("POWER_SUPPLY_STATUS=(.*)");
            rx.setPattern(newStr);
            pos = list.at(i).indexOf(rx);
            if (pos >= 0) {
                newStr = "Power Supply Status: ";
                newStr.append(rx.cap(1).trimmed());
                setPowerStatus(newStr);
            }
        } else if (list.at(i).contains("POWER_SUPPLY_CAPACITY")) {
            newStr = QString("POWER_SUPPLY_CAPACITY=(.*)");
            rx.setPattern(newStr);
            pos = list.at(i).indexOf(rx);
            if (pos >= 0) {
                newStr = "Battery Capacity: ";
                newStr.append(rx.cap(1).trimmed());
                setBatteryCapacity(newStr);
            }
        }
    }

    return 0;
}

QString About::sdTotalSize()
{
    return m_sdTotalSize;
}

void About::setSDTotalSize(QString sdTotalSize)
{
    if (m_sdTotalSize == sdTotalSize)
        return;
    m_sdTotalSize = sdTotalSize;
    emit sdTotalSizeChanged(m_sdTotalSize);
}

QString About::sdAvailableSize()
{
    return m_sdAvailableSize;
}

void About::setSDAvailableSize(QString sdAvailableSize)
{
    if (m_sdAvailableSize == sdAvailableSize)
        return;
    m_sdAvailableSize = sdAvailableSize;
    emit sdAvailableSizeChanged(m_sdAvailableSize);
}

int About::getSDInfos4Thread()
{
    /* sdcard */
    QString qStr;
    struct statfs diskInfo;

    if (access("/dev/mmcblk1p1", F_OK)) {
        printf("sdcard is absent\n");
        setSDTotalSize("");
        setSDAvailableSize("");
        return -1;
    } else {
        statfs("/mnt/sdcard", &diskInfo);
        unsigned long long blocksize = diskInfo.f_bsize;
        unsigned long long totalsize = blocksize * diskInfo.f_blocks;
//        printf("Total size = %llu Bytes\n", totalsize);
        qStr = "SD Total Size: ";
        qStr.append(QString(QString::number(totalsize, 10)));
        qStr.append("Bytes");
        setSDTotalSize(qStr);

//        unsigned long long freeDisk = diskInfo.f_bfree * blocksize;
        unsigned long long availableDisk = diskInfo.f_bavail * blocksize;
//        printf("Free size = %llu Bytes\n", availableDisk);
        qStr = "SD Available Size: ";
        qStr.append(QString(QString::number(availableDisk, 10)));
        qStr.append("Bytes");
        setSDAvailableSize(qStr);
    }
    return 0;
}

QString About::rtcTime()
{
    return m_rtcTime;
}

void About::setRtcTime(QString rtcTime)
{
    if (m_rtcTime == rtcTime)
        return;
    m_rtcTime = rtcTime;
    emit rtcTimeChanged(m_rtcTime);
}

void About::timerout_update()
{
    if (p_rtcFile != nullptr) {
        p_rtcFile->seek(0);
        QString str = p_rtcFile->readAll();
        QString qStr = "RTC Time: ";
        qStr.append(QString(str).trimmed());
        setRtcTime(qStr);
    } else
        qDebug() << "p_rtcFile is nullptr";
}

//void About::slotUpdate_bpVersion(QString bpVersion)
//{
//    setBpVersion(bpVersion);
//}

void WorkerThread::setAbout(About *about)
{
    p_about = about;
}

void WorkerThread::run()
{
    int ret;
    forever {
        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }
        ret = p_about->getMemInfos4Thread();
        ret = p_about->getPowerInfos4Thread();
        ret = p_about->getSDInfos4Thread();
        ret = p_about->getIMEIandMEID4Thread();
        ret = p_about->getbpVersion4Thread();

        QThread::sleep(3);
    }
}
