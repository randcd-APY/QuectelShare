/*
 * Copyright (c) 2012-2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

#include <comdef.h>
#include "QCMAP_Client.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BTRS_HCI_PIPE_PATH "/usr/bin/btrspipe"
#define BTRS_HCI_CONNECT_STR "LE Connection Complete"
#define LOG_FILE "/tmp/btrsdaemon_log.txt"

#define LOG_TAG "BTRSDaemon"

struct timeval log_time;
#define LOGI(x...) if(bfileLogging) {gettimeofday(&log_time, NULL); fprintf(fLog, "%lu.%lu ",log_time.tv_sec,log_time.tv_usec); fprintf(fLog,"Info:"x);}
#define LOGSE(x...) if(bfileLogging){gettimeofday(&log_time, NULL); fprintf(fLog, "%lu.%lu ",log_time.tv_sec,log_time.tv_usec); fprintf(fLog,"SYSERROR:"x);}
#define LOGE(x...)  if(bfileLogging){gettimeofday(&log_time, NULL); fprintf(fLog, "%lu.%lu ",log_time.tv_sec,log_time.tv_usec); fprintf(fLog,"Error:"x);}

#define RV_SUCCESS 0
#define RV_FAILED -1

typedef struct btrs_sig_handler_s
{
  int sig;
  __sighandler_t handler;
} btrs_sig_handler_t;

btrs_sig_handler_t btrs_sig_handler_tbl[] =
{
  {SIGTERM, NULL},
  {SIGINT, NULL},
  {SIGQUIT, NULL},
  {SIGHUP, NULL}
};

#ifdef __cplusplus
extern "C"
{
#endif

int pipe_fd = 0;
FILE* fLog=NULL;
boolean bfileLogging = 0;
pid_t child_id = 0;
QCMAP_Client * pQCMAP_Client = NULL;

/*
 * Method to Start WLAN
 */
int QCMAPClient()
{
    qmi_error_type_v01 qmi_err_num;

    if(!pQCMAP_Client)
    {
       pQCMAP_Client = new QCMAP_Client(NULL);
    }

    if(!pQCMAP_Client)
    {
        LOGE("Could not create QCMAP_Client");
        return 0;

    }
    LOGI("Enabling Mobile AP\n");
    pQCMAP_Client->EnableMobileAP(&qmi_err_num);
    LOGI("Enabling WLAN \n");
    pQCMAP_Client->EnableWLAN(&qmi_err_num);
    LOGI("Enabling WWAN \n");
    pQCMAP_Client->ConnectBackHaul(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                                      &qmi_err_num);

    return 1;
}

/*
 * Method executed in child context
 * Configures advertising data and sniffs for hci messages
 */
static void run_child(void)
{
    system("echo > /usr/bin/btrspipe");
    system("/usr/bin/hciconfig hci0 leadv > /dev/null");
    system("/usr/bin/hcitool -i hci0 cmd 0x08 0x0008 1e 02 01 02 0c 09 52 65 6d 6f 74 65 53 74 61 72 74 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 >/dev/null");
    system("/usr/bin/hcidump > /usr/bin/btrspipe");
}

/*
 * Utility to create a named pipe given a path
 */
static int init_pipe_util(char const * const szPath, const int flags, int * pFd)
{
    int rv;

    LOGI("Init pipe %s\n", szPath);
    rv = mkfifo(szPath, 0666);
    if (rv == -1)
    {
        if (EEXIST != errno)
        {
            LOGSE("mkfifo: %s", strerror(errno));

            return RV_FAILED;
        }
        else
        {
            LOGI("FIFO %s already exists\n", szPath);
        }
    }

    LOGI("Opening FIFO %s \n", szPath);
    *pFd = open(szPath, flags);
    LOGI("Opened FIFO %s fd = %d\n", szPath, *pFd);
    if (-1 == *pFd)
    {
        LOGSE("init_pipe_util open(): %s", strerror(errno));
        return RV_FAILED;
    }
    else
    {
        LOGI("pipe opened\n");
    }

    return RV_SUCCESS;
}

/*
 * Reads from the given fd and returns when the string is
 * found in the stream or there was a read error
 */
static int detect_string_in_stream(const int fd, boolean * pbFound,
        char const * const pString) {
    char c;
    int count = 0;
    int const len = strlen(pString);
    *pbFound = FALSE;

    while(1)
    {
        int i;

        for (i = 0; i < len; i++)
        {
            while(1)
            {
                count = read(fd, &c, 1);

                if(count)
                    break;
                else
                    sleep(1);
            }
            if (-1 == count)
            {
                LOGSE("read in detect_string_in_stream: %s", strerror(errno));
                return RV_FAILED;
            }

            if (c == pString[i])
            {
                continue;
            }
            else
            {
                break;
            }
        }

        if (i == len)
        {
            // The string has been found
            *pbFound = TRUE;
            break;
        }
    }

    return RV_SUCCESS;
}


/*
 * Detects incoming  LE connection
 */
static int btrs_handler()
{

    boolean  bFound;
    int retVal = RV_SUCCESS;

    retVal = detect_string_in_stream(pipe_fd, &bFound, "LE Connection Complete");

    if( bFound &&  (retVal == RV_SUCCESS) )
    {
        return RV_SUCCESS;
    }
    else
    {
        RV_FAILED;
    }

}

/*
 * Function runs in the parent context
 * main loop of the daemon
 */
static int run_parent(void)
{
    int rv;

    rv = init_pipe_util(BTRS_HCI_PIPE_PATH, O_RDONLY, &(pipe_fd));
    if (RV_SUCCESS != rv)
    {
        LOGE("init_pipes failed");
        exit(EXIT_FAILURE);
    }

    do
    {
        while(1)
        {
            LOGI("Waiting for LE Connection\n");
            rv = btrs_handler();
            if (RV_SUCCESS != rv)
            {
                LOGE("btrs_handler failed. Bailing.\n");
                break;
            }
            system("echo Starting Remote Start of WLAN > /dev/kmsg");
            LOGI("BT LE connection detected\n");
            LOGI("Starting Remote Start sequence\n");
            QCMAPClient();
            LOGI("Remote Start complete\n");
            system("echo Completed Remote Start of WLAN > /dev/kmsg");
            system("echo > /usr/bin/btrspipe");

        }
    } while (0);

    close(pipe_fd);

    return rv;
}


/*
 * Signal Handler and cleanup Function
 */
void exitDaemon(int sig)
{
    LOGI("Going to kill btrs daemon\n");
    (void)sig;
    unsigned int i=0;
    /* Note that the handler should ignore all the reg. signals
     * because they do not want to be interfered
     * while an ongoing signal is being processed
     */
    for(i=0; i<sizeof(btrs_sig_handler_tbl)/sizeof(btrs_sig_handler_t); i++)
    {
        signal(btrs_sig_handler_tbl[i].sig, SIG_IGN);
    }

    system("/usr/bin/hciconfig hci0 noleadv > /dev/null");

    delete pQCMAP_Client;
    pQCMAP_Client = NULL;

    if(child_id)
    {
       pid_t pg_id = getpgid(child_id);
       if(pg_id)
       {
          killpg( pg_id, SIGTERM);
       }
       child_id=0;
    }

    if(pipe_fd)
       close(pipe_fd);
    pipe_fd = 0;
    remove(BTRS_HCI_PIPE_PATH);

    exit(0);
}


/*
 * Initializes Signal handler
 */
void signal_init(void)
{
    unsigned int i=0;
    __sighandler_t temp;

    LOGI("Initializing signal handler\n");
    for(i=0; i<sizeof(btrs_sig_handler_tbl)/sizeof(btrs_sig_handler_t); i++) {
        temp = btrs_sig_handler_tbl[i].handler;
        btrs_sig_handler_tbl[i].handler = signal(btrs_sig_handler_tbl[i].sig,
                                                  exitDaemon);
        /* swap previous handler back if signal() was unsuccessful */
        if (SIG_ERR == btrs_sig_handler_tbl[i].handler) {
            btrs_sig_handler_tbl[i].handler = temp;
        }
    }
}



/*
 * M A I N
 */
int main (int argc, char * argv[])
{
    int status;
    char * errStr;

    fLog = fopen(LOG_FILE, "r");
    if(fLog)
    {
        fclose(fLog);
        fLog = fopen(LOG_FILE, "w+");
        if(fLog)
        {
            bfileLogging = true;
            setbuf(fLog, NULL);
        }
    }

    signal_init();

    LOGI("Starting BTRSDaemon\n");
    child_id = fork();
    if (-1 == child_id) {
        LOGSE("fork(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (child_id)
    {
        /**
         * P A R E N T
         */
        LOGI("After fork in parent\n");

        run_parent();

        LOGI("Waiting for child to exit\n");
        waitpid(child_id, &status, 0);

        LOGI("Parent exiting\n");
        exit(EXIT_SUCCESS);
    }
    else
    {
        /**
         * C H I L D
         */
        LOGI("After fork in child\n");

        run_child();

        LOGI("Child exiting\n");
        exit(EXIT_SUCCESS);
    }

    if(fLog)
    {
        fclose(fLog);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
