#include "waitthread.h"

WaitThread::WaitThread(QObject *parent) : QThread(parent)
{
    m_mobap = -1;
    m_primaryDNS = "";
    m_secondDNS = "";
    setNetworkState(NETWORK_BEGINE_STEP);
}

int WaitThread::initNetwork()
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    for (int i = 0; i < 5; i++) {
        setNetworkState(NETWORK_INIT_START);
        ret = QL_MCM_MobileAP_Init(&h_mobap);
        printf("QL_MCM_MobileAP_Init ret = %d, h_mobap=0x%X\n", ret, h_mobap);
        if (ret != 0) {
            setNetworkState(NETWORK_INIT_FAILED);
            msleep(100);
            continue;
        } else {
            m_mobap = h_mobap;
            setNetworkState(NETWORK_INIT_SUCCESSED);
            return 0;
        }
    }

    return -1;
}

int WaitThread::deinitNetwork()
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    h_mobap = m_mobap;
    for (int i = 0; i < 5; i++) {
//        setNetworkState(NETWORK_INIT_START);
        ret = QL_MCM_MobileAP_Deinit(h_mobap);
        printf("QL_MCM_MobileAP_Deinit ret = %d, h_mobap=0x%X\n", ret, h_mobap);
        if (ret != 0) {
//            setNetworkState(NETWORK_INIT_FAILED);
            msleep(100);
            continue;
        } else {
            m_mobap = -1;
//            setNetworkState(NETWORK_INIT_SUCCESSED);
            return 0;
        }
    }

    return -1;
}

int WaitThread::enableNetwork()
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    h_mobap = m_mobap;
    for (int i = 0; i < 5; i ++) { //enable
        setNetworkState(NETWORK_ENABLE_START);
        ret = QL_MCM_MobileAP_Enable(h_mobap);
        printf("QL_MCM_MobileAP_Enable ret = %d\n", ret);
        if (ret != 0) {
            setNetworkState(NETWORK_ENABLE_FAILED);
            msleep(100);
            continue;
        } else {
            setNetworkState(NETWORK_ENABLE_SUCCESSED);
            return 0;
        }
    }

    return -1;
}

int WaitThread::disableNetwork()
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    h_mobap = m_mobap;
    for (int i = 0; i < 5; i ++) { //enable
//        setNetworkState(NETWORK_ENABLE_START);
        ret = QL_MCM_MobileAP_Disable(h_mobap);
        printf("QL_MCM_MobileAP_Disable ret = %d\n", ret);
        if (ret != 0) {
//            setNetworkState(NETWORK_ENABLE_FAILED);
            msleep(100);
            continue;
        } else {
//            setNetworkState(NETWORK_ENABLE_SUCCESSED);
            return 0;
        }
    }

    return -1;
}

int WaitThread::connectNetwork(void)
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    m_primaryDNS = "";
    m_secondDNS = "";

    h_mobap = m_mobap;
    for (int i = 0; i < 5; ++i) {
        setNetworkState(NETWORK_CONNECT_START);
        ret = QL_MCM_MobileAP_ConnectBackhaul(h_mobap, E_QL_MOBILEAP_IP_TYPE_V4);
        printf("QL_MCM_MobileAP_ConnectBackhaul h_mobap = %d ret = %d\n", h_mobap, ret);
        if (ret != 0) {
            setNetworkState(NETWORK_CONNECT_FAILED);
            msleep(100);
            continue;
        } else {
            setNetworkState(NETWORK_CONNECT_SUCCESSED);
            return 0;
        }
    }

    return -1;
}

int WaitThread::disconnectNetwork(void)
{
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    h_mobap = m_mobap;
    for (int i = 0; i < 5; ++i) {
        setNetworkState(NETWORK_DISCONNECT_START);
        ret = QL_MCM_MobileAP_DisconnectBackhaul(h_mobap, E_QL_MOBILEAP_IP_TYPE_V4);
        printf("QL_MCM_MobileAP_DisconnectBackhaul h_mobap = %d ret = %d\n", h_mobap, ret);
        if (ret != 0) {
            setNetworkState(NETWORK_DISCONNECT_FAILED);
            msleep(100);
            continue;
        } else {
            setNetworkState(NETWORK_DISCONNECT_SUCCESSED);
            emit singalPingAvgTime("Stop Ping");
            return 0;
        }
    }

    return -1;
}

void WaitThread::setNetworkState(int state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit singalNetworkState(m_state);
    return;
}

void WaitThread::setNetworkAddress(QString address)
{
    m_networkAddress = address;
}

int WaitThread::getDnsAddress()
{
    char command[128];
    struct in_addr addr;
    QL_MOBAP_IPV4_WWAN_CONFIG_INFO_T    t_wwan_cfg = {0};
    E_QL_ERROR_CODE_T ret;
    mobap_client_handle_type h_mobap;

    if (!m_primaryDNS.isNull() && !m_primaryDNS.isEmpty())
        return -2;

    h_mobap = m_mobap;
    for (int i = 0; i < 5; ++i) {
        ret = QL_MCM_MobileAP_GetIPv4WWANCfg(h_mobap, &t_wwan_cfg);
        printf("QL_MCM_MobileAP_GetIPv4WWANCfg ret = %d, IPV4 Config Info:...\n", ret);
        if(ret == 0) {
            addr.s_addr = ntohl(t_wwan_cfg.v4_addr);
            printf("v4_addr_valid:%d v4_addr:%s\n",t_wwan_cfg.v4_addr_valid,inet_ntoa(addr));
            addr.s_addr = ntohl(t_wwan_cfg.v4_prim_dns_addr);
            if (m_primaryDNS.compare(QString(inet_ntoa(addr))) == 0)
                return 0;
            m_primaryDNS = QString(inet_ntoa(addr));
            emit singalPrimaryDNS(m_primaryDNS);
            printf("v4_prim_dns_addr_valid:%d v4_prim_dns_addr:%s\n",t_wwan_cfg.v4_prim_dns_addr_valid, m_primaryDNS);
            snprintf(command, sizeof(command), "echo 'nameserver %s' > /etc/resolv.conf",inet_ntoa(addr));
            system(command);

            addr.s_addr = ntohl(t_wwan_cfg.v4_sec_dns_addr);
            m_secondDNS = QString(inet_ntoa(addr));
            emit singalSecondDNS(m_secondDNS);
            printf("v4_sec_dns_addr_valid:%d v4_sec_dns_addr:%s\n",t_wwan_cfg.v4_sec_dns_addr_valid, inet_ntoa(addr));
            snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf", inet_ntoa(addr));
            system(command);
            return 0;
        } else {
            msleep(100);
            continue;
        }
    }

    return -1;
}

void WaitThread::setDnsAddress(QString address)
{
    m_primaryDNS = address;
}

void WaitThread::run()
{
    int fault_times = 0;
    QString program = "/bin/ping";
    QString tempAddress = "8.8.8.8";
    QStringList arguments;
    arguments << "-c" << "2" << tempAddress;

    QRegExp rx("");
    QProcess *myProcess = new QProcess(this);
    myProcess->setProcessChannelMode(QProcess::MergedChannels);
    while (1) {
        if (m_state == NETWORK_BEGINE_STEP) { //dial-up
            if (initNetwork() == 0) {
                if (enableNetwork() != 0) {
                    m_mobap = -1;
                    m_state = NETWORK_BEGINE_STEP;
                }
            } else {
                m_mobap = -1;
                m_state = NETWORK_BEGINE_STEP;
            }
        } else if (m_state == NETWORK_DISCONNECT_START) { //disconnect
            if (disconnectNetwork() != 0) {
                sleep(1);
                continue;
            }
        }

        if (m_state == NETWORK_ENABLE_SUCCESSED || m_state == NETWORK_CONNECT_START) { //connect
            if (connectNetwork() != 0) {
                m_mobap = -1;
                m_state = NETWORK_BEGINE_STEP;
            }
        }

        if (m_state != NETWORK_CONNECT_SUCCESSED) {
            sleep(1);
            continue;
        }

        if (m_networkAddress.compare(tempAddress) != 0) {
            tempAddress = m_networkAddress;
            arguments.clear();
            arguments << "-c" << "2" << m_networkAddress;
        }

        if (fault_times == 0)
            emit singalNetworkState(NETWORK_BLINK_START);
        myProcess->start(program, arguments);
        if (QThread::currentThread()->isInterruptionRequested() ) {
//            qDebug() << "peeta: isInterruptionRequested 1";
            return;
        }
        if (myProcess->waitForFinished(5000)) {
            if (QThread::currentThread()->isInterruptionRequested() ) {
//                qDebug() << "peeta: isInterruptionRequested 2";
                return;
            }
            QByteArray str = myProcess->readAll();
//            qDebug() << str;
            QString qStr = QString(str);
            QStringList list = qStr.split("\n", QString::QString::SkipEmptyParts);
            for (int i = 0; i < list.size();i++) {
                if (list.at(i).contains("min/avg/max")) {
                    rx.setPattern("(.*) = (.*)/(.*)/(.*)\\sms");
                    int pos = qStr.indexOf(rx);
                    if (pos >= 0) {
//                            qDebug() << "fulinux" << rx.cap(3);
                        emit singalPingAvgTime("RTT: " + rx.cap(3) + " ms");
                        emit singalNetworkState(NETWORK_BLINK_END);
                        fault_times = 0;
                        getDnsAddress();
                    }
                } else if (list.at(i).contains("unreachable")) {
                    emit singalPingAvgTime("Ping: Unreachable");
                    emit singalNetworkState(NETWORK_CONNECT_BREAK);
                    fault_times++;
//                    if (fault_times++ == 3) { //fix network
//                        disconnectNetwork();
//                        setNetworkState(NETWORK_CONNECT_START);
//                        break;
//                    }
//                    if (fault_times > 10){ //fix network
//                        fault_times = 0;
//                        if (disconnectNetwork() == 0) {
//                            if (disableNetwork() == 0) {
//                                deinitNetwork();
//                            }
//                        }
//                        m_networkAddress = "8.8.8.8";
//                        tempAddress = m_networkAddress;
//                        arguments.clear();
//                        arguments << "-c" << "2" << tempAddress;
//                        setNetworkState(NETWORK_BEGINE_STEP);
//                        break;
//                    }
                    break;
                }
            }
        } else {
            emit singalPingAvgTime("Ping: Timeout/Unreachable");
            emit singalNetworkState(NETWORK_CONNECT_BREAK);
            fault_times++;
            if (fault_times > 10) {
                m_primaryDNS = "";
                m_secondDNS = "";
            }
//            if (fault_times++ > 10){ //fix network
//                fault_times = 0;
//                if (disconnectNetwork() == 0) {
//                    if (disableNetwork() == 0) {
//                        deinitNetwork();
//                    }
//                }
//                m_networkAddress = "8.8.8.8";
//                tempAddress = m_networkAddress;
//                arguments.clear();
//                arguments << "-c" << "2" << tempAddress;
//                setNetworkState(NETWORK_BEGINE_STEP);
//                continue;
//            }
        }

        sleep(1);
    }
}
