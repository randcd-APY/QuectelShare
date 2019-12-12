/*
 *      Copyright:  (C) 2019 quectel
 *                  All rights reserved.
 *
 *       Filename:  time_daemon.c
 *        Version:  1.0.0(2019年12月04日)
 *         Author:  Peeta <peeta.chen@quectel.com>
 *      ChangeLog:  1, Release initial version on "2019年12月04日 09时53分31秒"
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/timex.h>
#include <sys/un.h>
#include <linux/rtc.h>
#include <errno.h>
#include <signal.h>

#define DELTA_TIME_FILE "ats_1"

#define SEC_TO_MSEC(s)  ((s) * 1000ULL)
#define MSEC_TO_SEC(s)  ((s) / 1000ULL)
#define USEC_TO_MSEC(s) ((s) / 1000ULL)
#define NSEC_TO_MSEC(s) ((s) / 1000000ULL)

typedef enum time_persistant_operation {
    TIME_READ_MEMORY,
    TIME_WRITE_MEMORY
}time_persistant_opr_type;

static int ntp_synced(void)
{
    struct timex txc = {};

    if (adjtimex(&txc) < 0) 
        return -1;

    if (txc.status & STA_UNSYNC)
        return -2;

    return 0;
}

static int rtc_get(int64_t *msecs)
{
    int rc, fd;
    time_t secs = 0;
    struct tm rtc_tm;

    fd = open("/dev/rtc0", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Daemon:%s: Unable to read from RTC device\n", __func__);
        goto fail_rtc;
    }

    rc = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (rc < 0) {
        printf("Daemon: %s: Unable to read from RTC device\n", __func__);
        goto fail_rtc;
    }

#if 0
    printf("Daemon:%s: Time read from RTC -- MM/DD/YY HH:MM:SS"
            "%d/%d/%d %d:%d:%d\n", __func__, rtc_tm.tm_mon,
            rtc_tm.tm_mday, rtc_tm.tm_year, rtc_tm.tm_hour,
            rtc_tm.tm_min, rtc_tm.tm_sec);
#endif

    /* Convert the time to UTC and then to milliseconds and store it */
    secs = mktime(&rtc_tm);
    secs += rtc_tm.tm_gmtoff;
    if (secs < 0) {
        fprintf(stderr, "Daemon: Invalid RTC seconds = %llx\n", secs);
        goto fail_rtc;
    }

    *msecs = SEC_TO_MSEC(secs);

    close(fd);
    return 0;

fail_rtc:
    close(fd);
    return -EINVAL;
}

static int time_persistant_memory_opr(const char *file_name,
        time_persistant_opr_type rd_wr, int64_t *data_ptr)
{
    char fname[120];
    int fd;

    /* location where offset is to be stored */
    snprintf(fname, 100, "%s/%s", "/data/time", file_name);
    //printf("Daemon:Opening File: %s\n", fname);

    switch(rd_wr) {
        case TIME_READ_MEMORY:
            //printf("Daemon:%s: Read operation\n", __func__);
            if ((fd = open(fname, O_RDONLY)) < 0) {
                printf("Daemon: Unable to open file for read\n");
                goto fail_operation;
            }

            if (read(fd, (int64_t *)data_ptr, sizeof(int64_t)) < 0) {
                fprintf(stderr, "Daemon:%s:Error reading from file\n", __func__);
                close(fd);
                goto fail_operation;
            }

            break;
        case TIME_WRITE_MEMORY:
            //printf("Daemon:%s: write operation\n", __func__);
            if ((fd = open(fname, O_RDWR | O_SYNC)) < 0) {
                printf("Daemon:Unable to open file,creating file\n");
                if (fd = open(fname, O_CREAT | O_RDWR | O_SYNC, 0644) < 0) {
                    fprintf(stderr, "Daemon:%s:Daemon:Unable to create file, exiting\n", __func__);
                    goto fail_operation;
                }
                close(fd);

                if ((fd = open(fname, O_RDWR | O_SYNC)) < 0) {
                    fprintf(stderr, "Daemon:Unable to open file,creating file\n");
                    goto fail_operation;
                }
            }

            if (write(fd, (int64_t *)data_ptr, sizeof(int64_t)) < 0) {
                fprintf(stderr, "Daemon:%s:Daemon:Unable to create file, exiting\n", __func__);

                close(fd);
                goto fail_operation;
            }
            break;
        default:
            return -EINVAL;
    }

    close(fd);
    return 0;

fail_operation:
    return -EINVAL;
}

#define TIME_STATUS_DO_NOTHING   0
#define TIME_STATUS_STORE_ANYHOW   1

int main (int argc, char **argv)
{
    int ret;
    int while_times = 1;
    int status = TIME_STATUS_DO_NOTHING;
    int64_t rtc_msecs;
    int64_t system_msecs;
    int64_t offset_ms = 0;
    int64_t last_offset_ms = 0;
    int64_t delta_ms = 0;
    struct timeval stime;

    while (1) {
        if ((ret = rtc_get(&rtc_msecs)) != 0)
            return ret;

        if (ntp_synced() == 0 || status == TIME_STATUS_STORE_ANYHOW) {
            gettimeofday(&stime, NULL);

            system_msecs = SEC_TO_MSEC(stime.tv_sec);
            //printf ("system_msecs = %llxms\n", system_msecs);

            system_msecs += USEC_TO_MSEC(stime.tv_usec);
            //printf ("system_msecs = %llxms\n", system_msecs);

            offset_ms = system_msecs - rtc_msecs;
            //printf ("offset_ms = %llxms\n", offset_ms);
            if (MSEC_TO_SEC(offset_ms) != MSEC_TO_SEC(last_offset_ms)) {
                time_persistant_memory_opr(DELTA_TIME_FILE, TIME_WRITE_MEMORY, &offset_ms);
                printf("Stored system time.\n");
                last_offset_ms = offset_ms;
            }

            status = TIME_STATUS_DO_NOTHING;
        }

        ret = time_persistant_memory_opr(DELTA_TIME_FILE, TIME_READ_MEMORY, &offset_ms);
        if (ret != 0) {
            last_offset_ms = 0;
            status = TIME_STATUS_STORE_ANYHOW;
            continue;
        }

        gettimeofday(&stime, NULL);

        system_msecs = SEC_TO_MSEC(stime.tv_sec);
        //printf ("system_msecs = %llxms\n", system_msecs);

        system_msecs += USEC_TO_MSEC(stime.tv_usec);
        //printf ("system_msecs = %llxms\n", system_msecs);

        delta_ms = system_msecs - rtc_msecs;
        if (MSEC_TO_SEC(delta_ms - offset_ms) != 0) {
            //printf ("offset_ms = %llxms\n", offset_ms);

            system_msecs = rtc_msecs + offset_ms;

            stime.tv_sec = MSEC_TO_SEC(system_msecs);
            //printf ("stime.tv_sec = %llxms\n", stime.tv_sec);

            stime.tv_usec = (system_msecs % 1000ULL) * 1000ULL;
            //printf ("stime.tv_usec = %llxms\n", stime.tv_usec);

            settimeofday(&stime,NULL);
            printf("Updated system time.\n");
        }

        sleep(while_times);

        if (ntp_synced() != 0)
            while_times = 1;
        else
            while_times *= 2;
    }

    return 0;
} /* ----- End of main() ----- */
