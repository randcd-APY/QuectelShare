/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
            
*****************************************************************************/
#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef BSP_SUPPORT_SDL
#include <pthread.h>
#include <SDL.h>
#endif
#include <execinfo.h>


#include "csr_types.h"
#include "csr_time.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_memstat.h"
#include "csr_macro.h"

#include "csr_transport.h"
#include "csr_msgconv.h"
#include "csr_converters.h"
#include "csr_ui_keycode.h"
#include "csr_ui_lower.h"
#include "csr_main.h"
#include "csr_app_lower.h"
#include "csr_app_main.h"
#include "csr_arg_search.h"
#include "csr_framework_ext.h"

#include "platform/csr_pmem_init.h"
#include "platform/csr_serial_init.h"
#include "platform/csr_panic_init.h"

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"
#include "csr_log_pcap.h"
#include "csr_log_cleartext.h"
#include "csr_log_fts.h"
#include "csr_log_btsnoop.h"
#include "csr_log_register.h"
#include "platform/csr_logtransport_init.h"
#endif


#include "csr_app_main_transport.h"
#include "csr_ui_lower_initialise.h"
#include "csr_key_input.h"

#include "csr_sched_init.h"

#ifdef USE_BT_AUDIO_SERVICE
#include "connx_peer_com.h"
#endif

#define _STRINGIFY(a) #a
#define STRINGIFY(a) _STRINGIFY(a)

/*The stack address for segment fault dump*/
#define STACK_SIZE 20

static void panic(CsrUint8 tech, CsrUint16 reason, const char *logtext)
{
    printf("PANIC: %s (tech %u, reason %u)\n", logtext, tech, reason);
}

#ifdef BSP_SUPPORT_SDL
static void *schedThread(void *schedInstance)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    CsrSched(schedInstance);
    SDL_PushEvent(&event);
    return NULL;
}

#endif

static void appExit(void)
{
#ifdef ENABLE_SHUTDOWN
    CsrSchedStop();
#else
    exit(0);
#endif
}

#ifdef CSR_MEMALLOC_PROFILING
static void leakDump(void *ptr, CsrSize sz,
    const CsrCharString *file, CsrUint32 line)
{
    printf("Leak: %p: %zu bytes, '%s:%u'\n", ptr, sz, file, line);
}

#endif

#ifdef CSR_LOG_ENABLE
static CsrCharString *logLevelTextString = NULL;
static CsrCharString *seekDelimiter(CsrCharString *str, CsrCharString delimiter)
{
    while ((*str != delimiter) && (*str != '\0'))
    {
        str += 1;
    }
    if (*str == '\0')
    {
        str = NULL;
    }
    else
    {
        *str = '\0';
        str += 1;
    }
    return str;
}

static CsrLogLevelText parseLogLevel(CsrCharString *str)
{
    CsrLogLevelText logLevel = CSR_LOG_LEVEL_TEXT_OFF;
    while (*str != '\0')
    {
        switch (*str)
        {
            case 'C':
                logLevel |= CSR_LOG_LEVEL_TEXT_CRITICAL;
                break;
            case 'E':
                logLevel |= CSR_LOG_LEVEL_TEXT_ERROR;
                break;
            case 'W':
                logLevel |= CSR_LOG_LEVEL_TEXT_WARNING;
                break;
            case 'I':
                logLevel |= CSR_LOG_LEVEL_TEXT_INFO;
                break;
            case 'D':
                logLevel |= CSR_LOG_LEVEL_TEXT_DEBUG;
                break;
            case 'A':
                logLevel |= CSR_LOG_LEVEL_TEXT_ALL;
                break;
            default:
                break;
        }
        str += 1;
    }
    return logLevel;
}

#ifdef CSR_ASYNC_LOG_TRANSPORT
static CsrUint32 asyncLogTransRingBufferSize = 0x80000;
#endif
#endif

void backTrace()
{
    int j, nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, STACK_SIZE);
    
    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
    would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        return;
    }

    for (j = 0; j < nptrs; j++)
	    printf("%d. %s\n", j, strings[j]);

    free(strings);
}

void terminationHandler (int signum)
{
	switch (signum)
	{
		case SIGPIPE:
		{
			perror("Ignore SIGPIPE");
			return;
		}

		case SIGHUP:
		{
			perror("SIGHUP");
			break;
		}

		case SIGINT:
		{
			perror("SIGINT");
			break;
		}

		case SIGTERM:
		{
			perror("SIGTERM");
			break;
		}
		case SIGSEGV:
		{
			perror("SIGSEGV got. Segment fault");
			backTrace();
			exit(0);
			break;
		}
		case SIGABRT:
		{
			perror("SIGABRT got. Process abort");
			backTrace();		   
			exit(0);
			break;
		}		 
		default:
		{
			perror("Unknown signal");
			break;
		}
	}

	if (signum == SIGHUP || signum == SIGINT || signum == SIGTERM || signum == SIGKILL)
	{
		perror("Stop Synergy scheduler");

		CsrSchedStop();
	}
}


void setSignalHandler()
{
    struct sigaction actNew, actOld;
    
    sigemptyset (&actNew.sa_mask);
    actNew.sa_handler   = terminationHandler;
    actNew.sa_flags     = 0;

    /* SIGPIPE */
    sigaction (SIGPIPE, NULL, &actOld);    
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGPIPE, &actNew, NULL);
    }    

    /* SIGHUP */
    sigaction (SIGHUP, NULL, &actOld);
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGHUP, &actNew, NULL);
    }

    /* SIGINT */
    sigaction (SIGINT, NULL, &actOld);
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGINT, &actNew, NULL);
    }

    /* SIGTERM */
    sigaction (SIGTERM, NULL, &actOld);
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGTERM, &actNew, NULL);
    }

    /* SIGSEGV */
    sigaction (SIGSEGV, NULL, &actOld);
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGSEGV, &actNew, NULL);
    }

    /* SIGABRT */
    sigaction (SIGABRT, NULL, &actOld);
    if (actOld.sa_handler != SIG_IGN)
    {
        sigaction (SIGABRT, &actNew, NULL);
    }    
}

CsrResult CsrMain(void)
{
    CsrCharString *parameter;
    CsrCharString *value;
    void *schedInstance;
    CsrUint16 i;
    CsrResult result;
    CsrBool guiEnable = TRUE;
    CsrBool tuiEnable = FALSE;
    CsrBool tuiNoCls = FALSE;
#ifdef BSP_SUPPORT_SDL
    pthread_t schedThreadHandle;
#endif

    if (!CsrArgSearchValidate())
    {
        printf("Invalid command line, use --help for usage.\n");
        return CSR_RESULT_FAILURE;
    }

    if (CsrArgSearch(NULL, "--help", NULL, NULL))
    {
        printf("Command line parameters:\n");
        printf("--log-cleartext-stdout [format] - Enable clear text logging to stdout\n");
        printf("--log-cleartext-stdout-schedevents - Enable sched events on the clear text logging\n");
        printf("--log-pcap-file [filename] - Enable Wireshark PCAP logging to file\n");
        printf("--log-pcap-live - Enable Wireshark PCAP live logging\n");
        printf("--log-btsnoop-file [filename] - Enable Btsnoop logging to file\n");
        printf("--log-fts-live [ftspath] - Enable FTS live logging\n");
        printf("--log-level <0xXXXXXXXX:0xXXXXXXXX:0xXXXXXXXX> - Configure log level for environment:task:logtext\n");
        printf("--log-level-text [origin].[suborigin].[<CEWIDA>] - Configure log level text. Use : to separate multiple arguments\n");
        printf("--log-async-ringbuffer [size] - Set Async log transport Ring buffer Size\n");
        printf("--tui - Enable Text User Interface (TUI)\n");
        printf("--tui-nocls - Disable clear screen between each redraw in TUI\n");
        CsrAppMainTransportUsage();
        CsrAppMainUsage();
        return CSR_RESULT_SUCCESS;
    }

    /* Disable output buffering */
    setbuf(stdout, NULL);

    /* Init Pmem */
    CsrPmemInit();
#ifdef CSR_MEMALLOC_PROFILING
    CsrMemStatInit();
#endif

    /* Install panic handler */
    CsrPanicInit(panic);

    /* Initialise Transport */
    result = CsrAppMainTransportInitialise();
    if (result != CSR_RESULT_SUCCESS)
    {
        return result;
    }

    /* Initialise Log */
#ifdef CSR_LOG_ENABLE
    CsrLogInit(8);
#endif

    /* Cleartext logging to stdout */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOGTRANSPORT_HAVE_STDOUT)
    if (CsrArgSearch(NULL, "--log-cleartext-stdout", &parameter, &value))
    {
        void *ltClearTextStdout;
        CsrLog *logClearTextStdOut;
        CsrBool schedEvents = CsrArgSearch(NULL, "--log-cleartext-stdout-schedevents", NULL, NULL) != NULL;

        ltClearTextStdout = CsrLogTransportStdoutOpen();
        logClearTextStdOut = CsrLogCleartextCreate(ltClearTextStdout, value, schedEvents);

        if (ltClearTextStdout && logClearTextStdOut)
        {
            CsrLogFormatInstRegister(logClearTextStdOut);
        }
        else
        {
            printf("Unable to open stdout log transport using format: %s\n", value ? value : CSR_LOG_CLEARTEXT_FORMAT);
            return CSR_RESULT_FAILURE;
        }
    }
#endif

    /* Wireshark File Logging */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOGTRANSPORT_HAVE_FILE)
    if (CsrArgSearch(NULL, "--log-pcap-file", &parameter, &value))
    {
        void *ltPcapFile;
        CsrCharString *filename = NULL;

        if (value != NULL)
        {
            filename = value;
        }
        else
        {
            filename = "pcap.cap";
        }

#if defined(CSR_ASYNC_LOG_TRANSPORT)
        /* Use Async log file Transport*/
        if (CsrArgSearch(NULL, "--log-async-ringbuffer", &parameter, &value))
        {
            if (value == NULL)
            {
                printf("No value specified for parameter %s\n", parameter);
                return CSR_RESULT_FAILURE;
            }
            asyncLogTransRingBufferSize = CsrStrToInt(value);
        }
        ltPcapFile = CsrLogTransportFileAsyncOpen(filename, asyncLogTransRingBufferSize);
#else 
        /* Use Sync log file Transport*/
        ltPcapFile = CsrLogTransportFileOpen(filename);
#endif

        if (ltPcapFile)
        {
            CsrLog *logPcapFile = CsrLogPcapCreate(ltPcapFile);
            CsrLogFormatInstRegister(logPcapFile);
        }
        else
        {
            printf("Unable to open Wireshark log transport on file %s\n", filename);
            return CSR_RESULT_FAILURE;
        }
    }
#endif

    /* Wireshark Live Logging */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOGTRANSPORT_HAVE_WSPIPE)
    if (CsrArgSearch(NULL, "--log-pcap-live", &parameter, &value))
    {
        void *ltPcapLive = CsrLogTransportWSPipeOpen(NULL, NULL);
        if (ltPcapLive)
        {
            CsrLog *logPcapPipe = CsrLogPcapCreate(ltPcapLive);
            CsrLogFormatInstRegister(logPcapPipe);
        }
        else
        {
            printf("Unable to open Wireshark Pipe log transport\n");
            return CSR_RESULT_FAILURE;
        }
    }
#endif

    /* Btsnoop File Logging */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOGTRANSPORT_HAVE_FILE)
    if (CsrArgSearch(NULL, "--log-btsnoop-file", &parameter, &value))
    {
        void *ltBtsnoopFile;
        CsrCharString *filename = NULL;

        if (value != NULL)
        {
            filename = value;
        }
        else
        {
            filename = "btsnoop.log";
        }

#if defined(CSR_ASYNC_LOG_TRANSPORT)
        /* Use Async log file Transport*/
        if (CsrArgSearch(NULL, "--log-async-ringbuffer", &parameter, &value))
        {
            if (value == NULL)
            {
                printf("No value specified for parameter %s\n", parameter);
                return CSR_RESULT_FAILURE;
            }
            asyncLogTransRingBufferSize = CsrStrToInt(value);
        }
        ltBtsnoopFile = CsrLogTransportFileAsyncOpen(filename, asyncLogTransRingBufferSize);
#else 
        /* Use Sync log file Transport*/
        ltBtsnoopFile = CsrLogTransportFileOpen(filename);
#endif
        if (ltBtsnoopFile)
        {
            CsrLog *logBtsnoopFile = CsrLogBtsnoopCreate(ltBtsnoopFile);
            CsrLogFormatInstRegister(logBtsnoopFile);
        }
        else
        {
            printf("Unable to open Btsnoop log transport on file %s\n", filename);
            return CSR_RESULT_FAILURE;
        }
    }
#endif

    /* FTS Live Logging */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOGTRANSPORT_HAVE_FTSPIPE)
    if (CsrArgSearch(NULL, "--log-fts-live", &parameter, &value))
    {
        void *ltFtsLive;
        CsrCharString *filename = NULL;

        if (value != NULL)
        {
            filename = value;
        }
        else
        {
            filename = "c:\\Program Files\\Frontline Test System II\\Frontline FTS4BT " STRINGIFY(FTS_VER);
        }

        ltFtsLive = CsrLogTransportFtsPipeOpen(filename);

        if (ltFtsLive)
        {
            CsrLog *logFtsLive = CsrLogFtsCreate(ltFtsLive);
            CsrLogFormatInstRegister(logFtsLive);
        }
        else
        {
            printf("Unable to open FTS Pipe log transport on %s\n", filename);
            return CSR_RESULT_FAILURE;
        }
    }
#endif

    /* Application Specific Initialisation */
    result = CsrAppMain();
    if (result != CSR_RESULT_SUCCESS)
    {
        return result;
    }

#ifdef CSR_LOG_ENABLE
    {
        CsrLogTechInfoRegister();
    }
#endif

#ifdef CSR_LOG_ENABLE
    if (CsrArgSearch(NULL, "--log-level", &parameter, &value))
    {
        CsrLogLevelEnvironment logLevelEnv;
        CsrLogLevelTask logLevelTask;
        CsrLogLevelText logLevelText;

        if (value == NULL)
        {
            printf("No value specified for parameter --log-level\n");
            return CSR_RESULT_FAILURE;
        }

        if (sscanf(value, "0x%8x:0x%8x:0x%8x", &logLevelEnv, &logLevelTask, &logLevelText) == 3)
        {
            CsrLogLevelEnvironmentSet(logLevelEnv);
            CsrLogLevelTaskSetAll(logLevelTask);
            CsrLogLevelTextSetAll(logLevelText);
        }
        else
        {
            printf("Invalid value '%s' for parameter %s (format: '0xXXXXXXXX:0xXXXXXXXX:0xXXXXXXXX')\n", value, parameter);
            return CSR_RESULT_FAILURE;
        }
    }
    if (CsrArgSearch(NULL, "--log-level-text", &parameter, &value))
    {
        CsrCharString *logLevelTextTmp;
        CsrCharString *origin = NULL;
        CsrCharString *subOrigin = NULL;
        CsrCharString *logLevel = NULL;

        if (value == NULL)
        {
            printf("No value specified for parameter --log-level-text\n");
            return CSR_RESULT_FAILURE;
        }

        logLevelTextString = logLevelTextTmp = CsrStrDup(value);

        do
        {
            origin = logLevelTextTmp;
            logLevelTextTmp = seekDelimiter(logLevelTextTmp, '.');
            if (logLevelTextTmp != NULL)
            {
                subOrigin = logLevelTextTmp;
                logLevelTextTmp = seekDelimiter(logLevelTextTmp, '.');
                if (logLevelTextTmp != NULL)
                {
                    logLevel = logLevelTextTmp;
                    logLevelTextTmp = seekDelimiter(logLevelTextTmp, ':');
                    CsrLogLevelTextSet(origin, subOrigin, parseLogLevel(logLevel));
                }
                else
                {
                    printf("Invalid value '%s' for parameter %s\n", value, parameter);
                    return CSR_RESULT_FAILURE;
                }
            }
            else
            {
                printf("Invalid value '%s' for parameter %s\n", value, parameter);
                return CSR_RESULT_FAILURE;
            }
        } while (logLevelTextTmp);
    }
#endif

    if (CsrArgSearch(NULL, "--tui", &parameter, &value))
    {
        tuiEnable = TRUE;
    }

    if (CsrArgSearch(NULL, "--tui-nocls", &parameter, &value))
    {
        tuiNoCls = TRUE;
    }

    setSignalHandler();

    /* Init the CSR_UI lower interface */
    guiEnable = CsrUiLowerInitialise(guiEnable, tuiEnable, tuiNoCls);

    /* Init the CSR_APP lower interface */
    CsrAppExitFunctionRegister(appExit);

#ifdef USE_BT_AUDIO_SERVICE
    InitPeerComInstance();
#endif
    /* Init Scheduler */
    for (i = 0; i < CSR_SCHEDULER_INSTANCES; i++)
    {
        schedInstance = CsrSchedInit(i, CSR_THREAD_PRIORITY_NORMAL, 0);
    }

    /* Activate Key Input */
    
    if (CsrArgSearch(NULL, "--use-key", &parameter, &value))
    {
        if ((value == NULL) || (!CsrStrCmp(value, "1")))
            CsrKeyInputActivate();
    }
    
#ifdef BSP_SUPPORT_SDL
    if (guiEnable)
    {
        /* Start the scheduler (in a separate thread) */
        pthread_create(&schedThreadHandle, NULL, schedThread, schedInstance);

        /* Main message loop */
        CsrUiLowerEventLoop();

        /* Stop the scheduler */
        pthread_join(schedThreadHandle, NULL);
    }
    else
#endif
    {
        /* Start scheduler */
        CsrSched(schedInstance);
    }

    /* Deactivate Key Input */
    if (CsrArgSearch(NULL, "--use-key", &parameter, &value))
    {
        if ((value == NULL) || (!CsrStrCmp(value, "1")))
            CsrKeyInputDeactivate();
    }

#ifdef USE_BT_AUDIO_SERVICE
    DeinitPeerComInstance();
#endif

    /* Deinit Scheduler */
    CsrSchedDeinit(schedInstance);

    /* Deinitialise Log */
#ifdef CSR_LOG_ENABLE
    CsrLogDeinit();
#ifdef ENABLE_SHUTDOWN
    CsrMsgConvDeinit();
#endif
    CsrPmemFree(logLevelTextString);
#endif

    /* Deinitialise Transport */
    CsrAppMainTransportDeinitialise();

    /* Deinit Pmem */
#ifdef CSR_MEMALLOC_PROFILING
    CsrMemStatDeinit(leakDump);
#endif
    CsrPmemDeinit();

    return CSR_RESULT_SUCCESS;
}
