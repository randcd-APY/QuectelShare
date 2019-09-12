#ifndef WAITTHREAD_H
#define WAITTHREAD_H

#include <QThread>
#include <QProcess>
#include <QDebug>

#include <ql-mcm-api/ql_in.h>

#define NETWORK_BEGINE_STEP             0
#define NETWORK_INIT_START              1
#define NETWORK_INIT_SUCCESSED          2
#define NETWORK_INIT_FAILED             3
#define NETWORK_ENABLE_START            4
#define NETWORK_ENABLE_SUCCESSED        5
#define NETWORK_ENABLE_FAILED           6
#define NETWORK_CONNECT_START           7
#define NETWORK_CONNECT_SUCCESSED       8
#define NETWORK_CONNECT_FAILED          9
#define NETWORK_CONNECT_BREAK           10
#define NETWORK_DISCONNECT_START        11
#define NETWORK_DISCONNECT_SUCCESSED    12
#define NETWORK_DISCONNECT_FAILED       13
#define NETWORK_BLINK_START             14
#define NETWORK_BLINK_END               15

class WaitThread : public QThread
{
    Q_OBJECT
public:
    explicit WaitThread(QObject *parent = nullptr);

    int initNetwork(void);
    int enableNetwork(void);
    int connectNetwork(void);

    int deinitNetwork(void);
    int disableNetwork(void);
    int disconnectNetwork(void);

    void setNetworkState(int state);
    void setNetworkAddress(QString address);

    int getDnsAddress(void);
    void setDnsAddress(QString address);

protected:
    virtual void run();

signals:
    void singalNetworkState(int state);
    void singalPingAvgTime(const QString t);
    void singalPrimaryDNS(const QString priDns);
    void singalSecondDNS(const QString secDns);

private:
    int m_mobap;
    int m_state;
    QString m_networkAddress;
    QString m_primaryDNS;
    QString m_secondDNS;
};

#endif // WAITTHREAD_H
