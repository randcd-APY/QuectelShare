#ifndef WLANTHREAD_H
#define WLANTHREAD_H

#include <QThread>
#include <QProcess>
#include <QDebug>

#include <wifi/ql_wifi.h>

class WlanThread : public QThread
{
    Q_OBJECT
public:
    explicit WlanThread(QObject *parent = nullptr);

//    void getScanNewHotspot();
    static void onScanResults(char *buff);
    static void onEventCallBack(char *event);

    QHash<QString, int> *getHotspotHash(void);
    QHash<int, QString> *getNetworkListHash(void);

    int enableNetwork(void);
    int disableNetwork(void);
    int selectEnableNetwork(QString network);
    int selectDisableNetwork(QString network);
    int startScanHotspot();
    int stopScanHotspot();
    int connectHotspot(QString ssid, QString psk);
    int disconnectHotspot();

    QString getCurrentHotspotName(void);
    void setCurrentHotspotName(QString name);

protected:
    virtual void run();

signals:
    void signalScanNewHotspot();
    void signalNetworkEvent(QString event);
    void signalCurrentHotspotChanged(QString name);

public slots:

private:
//    ModelManager* p_modelMgr;
    QHash<QString, int> *p_hotspotHash;
    QHash<int, QString> *p_networkListHash;
    QString m_currentHotspotName;
    int isEnableNetwork;
};

#endif // WLANTHREAD_H
