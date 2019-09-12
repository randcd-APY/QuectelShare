#include "WlanThread.h"

static QStringList g_hotspotList;
static QString g_networkEventFlag = "none";

WlanThread::WlanThread(QObject *parent) : QThread(parent)
{
    isEnableNetwork = 1;
    p_hotspotHash = new QHash<QString, int>();
    p_networkListHash = new QHash<int, QString>();
}

void WlanThread::onScanResults(char *buff)
{
//    printf("this is wifi list:\n");
//    printf("\n%s\n",buff);

    QByteArray str(buff);
    QString qStr = QString(str);
    g_hotspotList = qStr.split("\n", QString::SkipEmptyParts);
}

void WlanThread::onEventCallBack(char *event)
{
    printf("\nrecieve event:%s.\n",event);
    QByteArray str(event);
    QString qStr = QString(str);
    if (qStr.contains("WPS-AP-AVAILABLE")) {
        g_networkEventFlag = "available";
    } else if (qStr.contains("DISCONNECTED")) {
        g_networkEventFlag = "disconnected";
    } else if (qStr.contains("CTRL-EVENT-CONNECTED")) {
        g_networkEventFlag = "connected";
    } else
        g_networkEventFlag = "none";
}

QHash<QString, int> *WlanThread::getHotspotHash()
{
    return p_hotspotHash;
}

QHash<int, QString> *WlanThread::getNetworkListHash()
{
    return p_networkListHash;
}

int WlanThread::enableNetwork()
{
    isEnableNetwork = 1;
    return QL_WIFI_enableNetwork();
}

int WlanThread::disableNetwork()
{
    isEnableNetwork = 0;
    return QL_WIFI_disableNetwork();
}

int WlanThread::selectEnableNetwork(QString network)
{
    int id;

    if (p_networkListHash->isEmpty())
        return -1;

    QHash<int, QString>::const_iterator it;
    for (it = p_networkListHash->constBegin(); it != p_networkListHash->constEnd(); it++) {
        if (it.value().compare(network) == 0) {
            id = it.key();
            return QL_WIFI_select_enable(id);
        }
    }

    return -2;
}

int WlanThread::selectDisableNetwork(QString network)
{
    int id;

    if (p_networkListHash->isEmpty())
        return 0;

    QHash<int, QString>::const_iterator it;
    for (it = p_networkListHash->constBegin(); it != p_networkListHash->constEnd(); it++) {
        if (it.value().compare(network) == 0) {
            id = it.key();
            return QL_WIFI_select_disable(id);
        }
    }

    return -1;
}

int WlanThread::startScanHotspot()
{
    QL_WIFI_scan();

    return 0;
}

int WlanThread::stopScanHotspot()
{
    QL_WIFI_abort_scan();

    return 0;
}

int WlanThread::connectHotspot(QString ssid, QString psk)
{
    char *pSsid;
    char *pPsk;

//    qDebug() << "ssid: " << ssid << "current: " << getCurrentHotspotName();
    if (ssid.compare(getCurrentHotspotName()) == 0)
        return 0;

    QByteArray ba = ssid.toUtf8();
    pSsid = ba.data();

    QByteArray bb = psk.toUtf8();
    pPsk = bb.data();

    QL_WIFI_connect(pSsid, pPsk);
//    QL_WIFI_save_config();
    return 0;
}

int WlanThread::disconnectHotspot()
{
    QL_WIFI_disconnect();

    return 0;
}

QString WlanThread::getCurrentHotspotName()
{
    return m_currentHotspotName;
}

void WlanThread::setCurrentHotspotName(QString name)
{
    if (m_currentHotspotName == name)
        return;

    m_currentHotspotName = name;
    emit signalCurrentHotspotChanged(name);
}

void WlanThread::run()
{
    int times = 0;
    int ret = -1;
    char buff[4096] = {0};

    ret = QL_WIFI_init();
    if (ret != 0)
        qDebug() << "QL_WIFI_init failed";
    QL_WIFI_set_scan_results_listener(onScanResults);
    QL_WIFI_set_event_listener(onEventCallBack);
    QL_WIFI_scan();

    QString midStr = QString("(\\S+)\\s+(\\d+)\\s+-(\\d+)\\s+(\\S+)\\s+(\\S+)");
    QRegExp rx(midStr);
    while(1) {
        if (QThread::currentThread()->isInterruptionRequested() ) {
            return;
        }

        while(!isEnableNetwork) {
            QL_WIFI_abort_scan();
            QL_WIFI_disableNetwork();

            if (!p_hotspotHash->isEmpty()) {
                p_hotspotHash->clear();
            }

            emit signalScanNewHotspot();

            if (!p_networkListHash->isEmpty()) {
                p_networkListHash->clear();
            }

            sleep(1);
        }

        if (++times >= 5) {
            times = 0;
            QL_WIFI_scan();
        }

        QL_WIFI_list_network(buff, sizeof(buff));
        QByteArray str(buff);
        QString qStr = QString(str);

        QStringList networkList = qStr.split("\n", QString::SkipEmptyParts);

        QString Str1 = QString("(\\d+)\\s+(\\S+)\\s+(\\S+)\\s+\\[(\\S+)\\]");
        QString Str2 = QString("(\\d+)\\s+(\\S+)\\s+(\\S+)");
        QRegExp rx1(Str1);
        QRegExp rx2(Str2);
        if (networkList.size() > 0) {
            for (int i = 0; i < networkList.size(); ++i) {
                int pos = networkList.at(i).indexOf(rx1);
                if (pos >= 0) {
                    if (rx1.cap(4).contains("CURRENT"))
                        setCurrentHotspotName(rx1.cap(2));
                    if (rx1.cap(4).contains("DISABLED")) {
                        if (rx1.cap(2).compare(getCurrentHotspotName()) == 0)
                            setCurrentHotspotName("");
                    }
                    p_networkListHash->insert(rx1.cap(1).toInt(nullptr, 10), rx1.cap(2));
                } else {
                    pos = networkList.at(i).indexOf(rx2);
                    if (pos >= 0) {
                        p_networkListHash->insert(rx2.cap(1).toInt(nullptr, 10), rx2.cap(2));
                    }
                }
            }
            networkList.clear();
        }

        if (g_hotspotList.size() > 0) {
            for (int i = 0; i < g_hotspotList.size(); ++i) {
                int pos = g_hotspotList.at(i).indexOf(rx);
                if (pos >= 0) {
//                    qDebug() << rx.matchedLength();
//                    qDebug() << rx.capturedTexts();
                    p_hotspotHash->insert(rx.cap(5), -rx.cap(3).toInt(nullptr, 10));
                }
            }
            emit signalScanNewHotspot();
            g_hotspotList.clear();

//            QHash<QString, int>::const_iterator it;//迭代器的使用,it一开始指向的是第0个元素之前的位置
//            for(it = p_hotspotHash->constBegin(); it != p_hotspotHash->constEnd(); ++it){
//                qDebug() << it.key() << " : " << it.value();
//            }
        }

        if (g_networkEventFlag.compare("none") != 0) {
            emit signalNetworkEvent(g_networkEventFlag);
            g_networkEventFlag = "none";
        }

        sleep(1);
    }

    QL_WIFI_abort_scan();
}
