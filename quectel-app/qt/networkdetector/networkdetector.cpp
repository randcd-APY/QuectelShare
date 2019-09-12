#include "networkdetector.h"

NetworkDetector::NetworkDetector(QObject *parent) : QObject(parent)
{
    setSwitch(false);
    p_waitThread = new WaitThread(this);
//    connect(p_waitThread, SIGNAL(finished()), p_waitThread, SLOT(deleteLater())); //不能有这个会导致m_mobap=0
    connect(p_waitThread, SIGNAL(singalPingAvgTime(QString)), this, SLOT(soltPingAvgTime(QString)));
    connect(p_waitThread, SIGNAL(singalNetworkState(int)), this, SLOT(setNetworkState(int)));
    connect(p_waitThread, SIGNAL(singalPrimaryDNS(QString)), this, SLOT(setPrimaryDNS(QString)));
    connect(p_waitThread, SIGNAL(singalSecondDNS(QString)), this, SLOT(setSecondDNS(QString)));
    setAddress("8.8.8.8");
    p_waitThread->setNetworkState(NETWORK_DISCONNECT_SUCCESSED);
    p_waitThread->start();
}

void NetworkDetector::switchNetworkState(bool state)
{
    if (state) {
        p_waitThread->setNetworkState(NETWORK_CONNECT_START);
    } else {
        p_waitThread->setNetworkState(NETWORK_DISCONNECT_START);
    }

    return;
}

void NetworkDetector::setAddress(QString address)
{
    if (m_address == address)
        return;
    m_address = address;
    emit addressChanged(m_address);
    if (p_waitThread != nullptr)
        p_waitThread->setNetworkAddress(m_address);
}

void NetworkDetector::setPrimaryDNS(QString dns)
{
    if (m_primaryDNS == dns)
        return;

    m_primaryDNS = "Primary DNS: ";
    m_primaryDNS.append(dns);
    emit primaryDNSChanged(m_primaryDNS);
}

void NetworkDetector::setSecondDNS(QString dns)
{
    if (m_secondDNS == dns)
        return;

    m_secondDNS = "Second DNS: ";
    m_secondDNS.append(dns);
    emit secondDNSChanged(m_secondDNS);
}

bool NetworkDetector::getSwitch() const
{
    return m_bswitch;
}

QString NetworkDetector::pingAvgTime()
{
    return m_pingAvgTime;
}

int NetworkDetector::networkState()
{
    return m_networkState;
}

QString NetworkDetector::getAddress()
{
    return m_address;
}

QString NetworkDetector::getPrimaryDNS()
{
    return m_primaryDNS;
}

QString NetworkDetector::getSecondDNS()
{
    return m_secondDNS;
}

void NetworkDetector::setSwitch(bool bswitch)
{
    if (m_bswitch == bswitch)
        return;

    m_bswitch = bswitch;
    emit switchChanged(m_bswitch);
}

void NetworkDetector::setPingAvgTime(QString time)
{
    if (m_pingAvgTime == time)
        return;
    m_pingAvgTime = time;
    emit pingAvgTimeChanged(m_pingAvgTime);
}

void NetworkDetector::setNetworkState(int state)
{
    if (m_networkState == state)
        return;
    m_networkState = state;
    emit networkStateChanged(m_networkState);
}

void NetworkDetector::soltPingAvgTime(QString t)
{
//    qDebug() << "peeta:" << t;
    setPingAvgTime(t);
}
