/******************************************************************************
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_TIMEMANAGER_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_TIMEMANAGER_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <memory.h>
#include <arpa/inet.h>

namespace alexaClientSDK {
namespace avsManager {

#define SEC_TO_MSEC(s)  ((s) * 1000ULL)
#define MSEC_TO_SEC(s)  ((s) / 1000ULL)
#define USEC_TO_MSEC(s)  ((s) / 1000ULL)
#define NSEC_TO_MSEC(s)  ((s) / 1000000ULL)

typedef struct {
    uint64_t utcTime;
    uint64_t uncertainty;
    uint64_t reference;
} SntpTime;

typedef struct {
    uint32_t sec;
    uint32_t fsec;
} SntpTimeStamp;

typedef struct {
   uint8_t       li_vn_mode;       /* leap indicator, version and mode */
   uint8_t       stratum;          /* peer stratum */
   uint8_t       poll;             /* peer poll interval */
   int8_t        precision;        /* peer clock precision */
   uint32_t      root_delay;       /* distance to primary clock */
   uint32_t      root_dispersion;  /* clock dispersion */
   uint32_t      ref_id;           /* reference clock ID */
   SntpTimeStamp ref_time;         /* time peer clock was last updated */
   SntpTimeStamp orig_time;        /* originate time stamp */
   SntpTimeStamp recv_time;        /* receive time stamp */
   SntpTimeStamp tx_time;          /* transmit time stamp */
} SntpPacket;

class TimeManager {
public:
    TimeManager();
    inline ~TimeManager(){}

    void handleSystemTime();

private:
    int  rtcInit();
    bool requestSntpTime();
    bool isTimeOffsetAvailable();
    void timevalDiff(timeval& result,
           const timeval& x, const timeval& y);
    bool setRtcTimeOffset();
    bool readRtcTimeOffset();
    bool setSystemTime();

    SntpTime mCachedNtpTime;
    uint64_t m_rtcTime;
    int64_t m_rtcTimeInMsec;
    uint64_t m_rtcTimeOffset;
};

} // end of namespace
} // end of namespace

#endif //AVSMANAGER_INCLUDE_AVSMANAGER_TIMEMANAGER_H_
