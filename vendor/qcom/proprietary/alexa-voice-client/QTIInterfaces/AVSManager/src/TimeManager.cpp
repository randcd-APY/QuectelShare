/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <memory.h>
#include <arpa/inet.h>
#include <linux/rtc.h>
#include <cutils/properties.h>

#include "AVSManager/TimeManager.h"

/// Path to configuration file (from command line arguments).
std::string timeOffsetFilePath = "/data/time";
std::string timeOffsetFile = "/data/time/offset";

namespace alexaClientSDK {
namespace avsManager {

#define NTP_RETRY_COUNT 5
#define TIME2MS(time) (int64_t) ( (int64_t)1000*time.tv_sec + (int64_t)time.tv_usec/1000 );
#define NTP_PROP_MAX_VALUE 50

#define DUMP_SNTP_PACKET(header,pkt) printf(header " li_vn_mode=%x, stratum=%x poll=%x precision=%x " \
         "root_delay=%x root_dispersion=%x ref_id=%x ref_time=%x,%x orig_time=%x,%x recv_time=%x,%x tx_time=%x,%x.", \
         pkt.li_vn_mode, pkt.stratum, pkt.poll, pkt.precision, pkt.root_delay, pkt.root_dispersion, pkt.ref_id, \
         pkt.ref_time.sec, pkt.ref_time.fsec, pkt.orig_time.sec, pkt.orig_time.fsec, \
         pkt.recv_time.sec, pkt.recv_time.fsec, pkt.tx_time.sec, pkt.tx_time.fsec)

TimeManager::TimeManager()
{
    printf("TimeManager created.");
    rtcInit();
}

void TimeManager::handleSystemTime()
{
    int itr = 0;
    bool ntpStatus = false;
    bool syncTimeFromServer = true; // Force sync the time from server always

    if (syncTimeFromServer) {
        // Get NTP time
        printf("Get time from NTP server\n");
        while(itr < NTP_RETRY_COUNT) {
            ntpStatus = requestSntpTime();
            if(ntpStatus == false) {
                itr++;
                printf("Failed to get network time, retry: %d\n", itr);
                sleep(5);
            } else {
                break;
            }
        }

        if(ntpStatus) {
            printf("Update RTC time offset\n");
            setRtcTimeOffset();
        } else {
            printf("Failed to get network time after max retries...\n");
            return;
        }
    } else {
        printf("RTC offset valid. skip network request\n");
        readRtcTimeOffset();
        //TODO: calculate time using RTC and offset
    }

    //Update system time to network time.
    setSystemTime();
}

#define DUMP_SNTP_PACKET(header,pkt) printf(header " li_vn_mode=%x, stratum=%x poll=%x precision=%x " \
         "root_delay=%x root_dispersion=%x ref_id=%x ref_time=%x,%x orig_time=%x,%x recv_time=%x,%x tx_time=%x,%x\n", \
         pkt.li_vn_mode, pkt.stratum, pkt.poll, pkt.precision, pkt.root_delay, pkt.root_dispersion, pkt.ref_id, \
         pkt.ref_time.sec, pkt.ref_time.fsec, pkt.orig_time.sec, pkt.orig_time.fsec, \
         pkt.recv_time.sec, pkt.recv_time.fsec, pkt.tx_time.sec, pkt.tx_time.fsec)

void TimeManager::timevalDiff(timeval& result, const timeval& x, const timeval& y)
{
   int diff_sec;
   int diff_usec;

   diff_sec  = y.tv_sec  - x.tv_sec;
   diff_usec = y.tv_usec - x.tv_usec;

   /* in case usec is greater than 1 second */
   diff_sec += diff_usec / 1000000;
   diff_usec = diff_usec % 1000000;

   /* offset the negative value for usec */
   if (diff_usec > 0) {
      diff_sec  += 1;
      diff_usec -= 1000000;
   }

   result.tv_sec  = -diff_sec;
   result.tv_usec = -diff_usec;
}

bool TimeManager::requestSntpTime()
{
    sockaddr_in server = sockaddr_in();
    int socketFd = {};
    SntpPacket request = SntpPacket();
    SntpPacket response = SntpPacket();
    struct timeval sendTime, recvTime, diffTime;
    char ntpServer[NTP_PROP_MAX_VALUE] = "\0";
    std::string ntpServerHostName = "";

    // DNS query on XTRA time server
    printf("Resolving hostname for SNTP server %s\n", ntpServerHostName.c_str());

#define SNTP_PORT_NUM 123
    server.sin_family = AF_INET;
    server.sin_port = htons(SNTP_PORT_NUM);

#ifndef MIN
    #define MIN(a,b) ((a) < (b) ? a : b)
#endif
    property_get("ro.qc.sdk.fwk.ntpServer", ntpServer, "\0");

    ntpServerHostName = ntpServer;
    if(ntpServerHostName.empty())
    {
        printf("Please configure ntpServer in build.prop and then restart the target... \n");
        return false;
    }
    printf("######## Value of ro.qc.sdk.fwk.ntpServer = %s\n", ntpServerHostName.c_str());

    struct hostent *host = gethostbyname(ntpServerHostName.c_str());
    if (host != NULL) {
        size_t len = MIN((size_t)host->h_length, sizeof(server.sin_addr));
        memcpy(&server.sin_addr, host->h_addr_list[0], len);

    } else if (!inet_aton(ntpServerHostName.c_str(), &server.sin_addr)) {
        printf("DNS resolution on %s failed\n", ntpServerHostName.c_str());
        return false;
    }

    // open the socket
    printf("Connecting to SNTP server: %s\n", inet_ntoa(server.sin_addr));
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketFd < 0) {
       printf("socket failed\n");
       return false;
    }

    // Set timestamp among other things so server knows
#define SNTP_VN_NUM        4
#define SNTP_CLIENT_MODE   3
    gettimeofday(&sendTime, NULL);
    request.li_vn_mode = (SNTP_VN_NUM <<3) + SNTP_CLIENT_MODE;

    request.tx_time.sec = htonl((unsigned int)sendTime.tv_sec
            + (unsigned int)(70 * 365 + 17) * (24 * 60 * 60));

    request.tx_time.fsec = htonl((unsigned int)(4294967296.
            * (double)sendTime.tv_usec/1000000. +.5));

    // Send SNTP Request
    DUMP_SNTP_PACKET("request", request);
    sendto(socketFd,
         (void *)&request,
         sizeof(request),
         0,
         (struct sockaddr *)&server,
         sizeof(server));

    // Wait SNTP Resp
#define SNTP_TIMEOUT_SEC   5
#define SNTP_TIMEOUT_USEC  0
#define SNTP_RECV_BUF_SIZE 128
    sockaddr_in dst = sockaddr_in();
    socklen_t dstLen = sizeof(dst);
    char recvBuf[SNTP_RECV_BUF_SIZE];
    fd_set rfds;
    timeval timeout = {.tv_sec = SNTP_TIMEOUT_SEC, .tv_usec = SNTP_TIMEOUT_USEC};

    FD_ZERO(&rfds);
    FD_SET(socketFd, &rfds);

    printf("select Sntp socket with timeout - %ld, %ld \n", timeout.tv_sec, timeout.tv_usec);
    if(!select(socketFd + 1, &rfds, NULL, NULL, &timeout)) {
        printf("SNTP server receive time out\n");
        close(socketFd);
        return false;
    }

    size_t len = recvfrom(socketFd,
            recvBuf,
            sizeof(recvBuf),
            0,
            (sockaddr *)&dst,
            &dstLen);

    if(ntohs(dst.sin_port) != SNTP_PORT_NUM) {
         printf("receive error\n");
         close(socketFd);
         return false;
    }

    if(len < sizeof(response)) {
        printf("Packet size error\n");
    }

    memcpy(&response, recvBuf, sizeof(response));

    DUMP_SNTP_PACKET("response", response);

#define SNTP_LI(x)   ( (x>>6) & 3 )
#define SNTP_VN(x)   ( (x>>3) & 7 )
#define SNTP_MODE(x) (  x     & 7 )
#define SNTP_SERVER_MODE   4
    if(SNTP_VN(response.li_vn_mode) != SNTP_VN_NUM
         ||SNTP_MODE(response.li_vn_mode) != SNTP_SERVER_MODE)
    {
        printf("Illegal packet\n");
    }

    // receive time
    gettimeofday(&recvTime, NULL);
    close(socketFd);

    // get the difference between client receive time and client send time
    timevalDiff(diffTime, recvTime, sendTime);

    int64_t clientSendTime = TIME2MS(sendTime);
    int64_t clientRecvTime = TIME2MS(recvTime);
    int64_t timeDiffInMillis = TIME2MS(diffTime);
    int64_t responseTime = (int64_t)(clientSendTime + timeDiffInMillis);

#define SECSINSEVENTYYEARS 2208988800ll         // from 1 Jan 1900 to 1970 (start of Unix time)
#define PACKETTIME(time) (((int64_t)ntohl(time.sec) - SECSINSEVENTYYEARS) * 1000) \
        + (((int64_t)ntohl(time.fsec) * 1000L) / 0x100000000L);
    int64_t originateTime = PACKETTIME(response.orig_time);
    int64_t serverReceiveTime = PACKETTIME(response.recv_time);
    int64_t serverTransmitTime = PACKETTIME(response.tx_time);

    // round trip time in milliseconds
    int64_t roundTripTime = (int64_t)(timeDiffInMillis - (serverTransmitTime - serverReceiveTime));
    int64_t clockOffset = (int64_t)(((serverReceiveTime - originateTime) + (serverTransmitTime - responseTime))/2);

    mCachedNtpTime.uncertainty = (uint64_t)roundTripTime/2;
    mCachedNtpTime.utcTime = (int64_t)(responseTime + clockOffset);
    mCachedNtpTime.reference = clientRecvTime;

    std::cout << "Sntp time: " << std::endl;
    std::cout << "utcTime: " << mCachedNtpTime.utcTime << std::endl;
    std::cout << "uncertainty: " << mCachedNtpTime.uncertainty << std::endl;

    return true;
}

bool TimeManager::isTimeOffsetAvailable()
{
    bool rc = true;

    if (access(timeOffsetFilePath.c_str(), F_OK) != -1) {
        if (access(timeOffsetFile.c_str(), F_OK) != -1) {
            printf("Offset file exists...\n");
        } else {
            printf("Unable to find offset file...\n");
            rc = false;
        }
    } else {
        printf("Unable to find offset directory...\n");
        rc = false;
    }

    return rc;
}

bool TimeManager::setRtcTimeOffset()
{
    int fd;

    if (access(timeOffsetFilePath.c_str(), F_OK) != -1) {
        printf("Offset directory exists...\n");
    } else {
        printf("Create offset directory...\n");
        mkdir(timeOffsetFilePath.c_str(), 0700);
    }

    printf("Opening File: %s\n", timeOffsetFile.c_str());

    if ((fd = open(timeOffsetFile.c_str(), O_RDWR | O_SYNC)) < 0) {
        printf("Unable to open file, creating file...\n");
        if ((fd = open(timeOffsetFile.c_str(),
                       O_CREAT | O_RDWR | O_SYNC,  0666)) < 0) {
            printf("Unable to create file, exiting\n");
            return false;
        }
    }

    m_rtcTimeOffset = mCachedNtpTime.utcTime - m_rtcTimeInMsec;

    if (write(fd, (uint64_t *)&m_rtcTimeOffset, sizeof(uint64_t)) < 0) {
        printf("%s:Error writing to file\n", __func__);
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

bool TimeManager::readRtcTimeOffset()
{
  int fd;

  printf("%s:Read RTC time offset \n", __func__);
  if ((fd = open(timeOffsetFile.c_str(), O_RDONLY)) < 0) {
    printf("Unable to open file for read\n");
    return false;
  }
  if (read(fd, (uint64_t *)&m_rtcTimeOffset, sizeof(uint64_t)) < 0) {
    printf("%s:Error reading from file\n", __func__);
    close(fd);
    return false;
  }

  close(fd);
  return true;
}

bool TimeManager::setSystemTime()
{
    struct timeval tv;

    //Update system time
    tv.tv_sec = (time_t) (MSEC_TO_SEC(m_rtcTimeInMsec + m_rtcTimeOffset));
    tv.tv_usec = 0;
    printf("%s: Set clock to sec=%ld usec=%ld\n",
                         __func__, tv.tv_sec, tv.tv_usec);
    if (settimeofday(&tv, NULL) != 0) {
        printf("%s: Unable to set clock to sec=%ld"
            "usec=%ld\n", __func__, tv.tv_sec,
            tv.tv_usec);
        return false;
    }

    return true;
}

int TimeManager::rtcInit()
{
    int fd, rc = 0;
    struct tm rtc_tm;
    time_t secs = 0;

    memset(&rtc_tm, 0, sizeof(struct tm));

    do {
        fd = open("/dev/rtc0", O_RDONLY);
        if(fd < 0) {
            printf("%s: Unable to open RTC device\n", __func__);
            rc = -EINVAL;
            break;
        }

        rc = ioctl(fd, RTC_RD_TIME, &rtc_tm);
        if(rc < 0) {
            printf("%s: Unable to read from RTC device\n",
                        __func__);
            close(fd);
            rc = -EINVAL;
            break;
        }

        printf("%s: Time read from RTC -- year = %d, month = %d,"
            "day = %d\n", __func__, rtc_tm.tm_year, rtc_tm.tm_mon,
            rtc_tm.tm_mday);

        close(fd);

        /* Convert the time to UTC and then to milliseconds and store it */
        secs = mktime(&rtc_tm);
        secs += rtc_tm.tm_gmtoff;
        if(secs < 0) {
            printf("Invalid RTC seconds = %ld\n", secs);
            rc = -EINVAL;
            break;
        }

        m_rtcTimeInMsec = SEC_TO_MSEC(secs);
        printf("Value read from RTC seconds = %lld\n", m_rtcTimeInMsec);
    }while(false);

    return rc;
}

} // end of namespace
} // end of namespace
