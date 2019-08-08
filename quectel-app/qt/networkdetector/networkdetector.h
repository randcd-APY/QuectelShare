#ifndef NETWORKDETECTOR_H
#define NETWORKDETECTOR_H

#include <QObject>

#include "waitthread.h"

class NetworkDetector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool bswitch READ getSwitch WRITE setSwitch NOTIFY switchChanged)
    Q_PROPERTY(QString pingAvgTime READ pingAvgTime WRITE setPingAvgTime NOTIFY pingAvgTimeChanged)
    Q_PROPERTY(int networkState READ networkState WRITE setNetworkState NOTIFY networkStateChanged)
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
public:
    explicit NetworkDetector(QObject *parent = nullptr);

    bool getSwitch() const;
    QString pingAvgTime();
    int networkState();
    QString getAddress();

    Q_INVOKABLE void switchNetworkState(bool state);

signals:
    void switchChanged(bool bswitch);
    void pingAvgTimeChanged(QString time);
    void networkStateChanged(int state);
    void addressChanged(QString address);

public slots:
    void setSwitch(bool bswitch);
    void setPingAvgTime(QString time);
    void setNetworkState(int state);
    void setAddress(QString address);

    void soltPingAvgTime(QString t);

private:
    bool m_bswitch;
    QString m_pingAvgTime;
    int m_networkState;
    QString m_address;

    WaitThread *p_waitThread;
};

#endif // NETWORKDETECTOR_H
