/*****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_app.h"
#include "csr_bt_log_tech_info_register.h"

#define _STRINGIFY(a) #a
#define STRINGIFY(a) _STRINGIFY(a)

/* We support 4 transports: pcap, BTsnoop, Live-FTS, Live-WS */
#define CSR_BT_APP_MAX_LOG_TRANSPORTS 4

#define DEFAULT_WIRESHARK_LOG   (DEFAULT_SYNERGY_LOG_PATH "pcap.cap")

#define DEFAULT_BT_SNOOP_LOG    (DEFAULT_SYNERGY_LOG_PATH "hci.cfa")


/* Log transport/instance containters for fw2+ */
static void *ltPcap = NULL;
static CsrLog *logPcap = NULL;
static void *ltBtsnoop = NULL;
static CsrLog *logBtsnoop = NULL;
#ifdef CSR_LOGTRANSPORT_HAVE_FTSPIPE
static void *ltFts = NULL;
static CsrLog *logFts = NULL;
#endif
#ifdef CSR_LOGTRANSPORT_HAVE_WSPIPE
static void *ltWs = NULL;
static CsrLog *logWs = NULL;
#endif

void CsrBtAppLogInit()
{
    CsrLogInit(CSR_BT_APP_MAX_LOG_TRANSPORTS);
#ifdef CSR_LOGTRANSPORT_HAVE_FILE
    {
        CsrCharString fileStr[128];
        CsrTime logTimeValLow;
        CsrTime logTimeValHigh;

        logTimeValLow = CsrTimeGet(&logTimeValHigh);

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
        CsrSnprintf(fileStr, 128, DEFAULT_WIRESHARK_LOG);
#else
        CsrSnprintf(fileStr, 128, "csr-log-%08lu%08lu-pcap.cap", logTimeValHigh, logTimeValLow);
#endif

        ltPcap = CsrLogTransportFileOpen(fileStr);
        /* [QTI] Fix KW issue#83499 through adding the check "ltPcap". */
        if (!ltPcap)
        {
            printf("Error: Could not open file named: %s\n", fileStr);
            return;
        }

        logPcap = CsrLogPcapCreate(ltPcap);
        CsrLogFormatInstRegister(logPcap);

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
        CsrSnprintf(fileStr, 128, DEFAULT_BT_SNOOP_LOG);
#else
        CsrSnprintf(fileStr, 128, "csr-log-%08lu%08lu-btsnoop.log", logTimeValHigh, logTimeValLow);
#endif

        ltBtsnoop = CsrLogTransportFileOpen(fileStr);
        /* [QTI] Fix KW issue#834991 through adding the check "ltBtsnoop". */
        if (!ltBtsnoop)
        {
            printf("Error: Could not open file named: %s\n", fileStr);
            return;
        }

        logBtsnoop = CsrLogBtsnoopCreate(ltBtsnoop);
        CsrLogFormatInstRegister(logBtsnoop);
    }
#endif /* HAVE_FILE */
}

/* Log deinitialisation */
void CsrBtAppLogDeinit()
{
    /* Log deinitilisation for framework 2.x and up */
    if(ltBtsnoop)
    {
        CsrLogTransportFileClose(ltBtsnoop);
    }
    if(ltPcap)
    {
        CsrLogTransportFileClose(ltPcap);
    }
#ifdef CSR_LOGTRANSPORT_HAVE_WSPIPE
    if(ltWs)
    {
        CsrLogTransportWSPipeClose(ltWs);
    }
#endif
#ifdef CSR_LOGTRANSPORT_HAVE_FTSPIPE
    if(ltFts)
    {
        CsrLogTransportFtsPipeClose(ltFts);
    }
#endif
    CsrLogDeinit();
}

/* Enable Wireshark live logging */
void CsrBtAppLogWireshark()
{
#ifdef CSR_LOGTRANSPORT_HAVE_WSPIPE
    ltWs = CsrLogTransportWSPipeOpen(NULL, NULL);
    if (ltWs)
    {
        logWs = CsrLogPcapCreate(ltWs);
        CsrLogFormatInstRegister(logWs);
    }
    else
#endif
    {
        printf("Error: WSPipe log transport failed\n");
        exit(1);
    }
}

/* Enable FTS live logging */
void CsrBtAppLogFrontline()
{
#ifdef CSR_LOGTRANSPORT_HAVE_FTSPIPE
    CsrCharString *filename = "c:\\Program Files\\Frontline Test System II\\Frontline FTS4BT " STRINGIFY(FTS_VER);
    ltFts = CsrLogTransportFtsPipeOpen(filename);
    if (ltFts)
    {
        logFts = CsrLogFtsCreate(ltFts);
        CsrLogFormatInstRegister(logFts);
    }
    else
    {
        printf("Error: FTSPipe log transport failed on %s\n", filename);
        exit(1);
    }
#else
    {
        printf("Error: FTSPipe log transport failed\n");
        exit(1);
    }
#endif
}

/* Apply log levels */
void CsrBtAppLogApply()
{
    CsrLogLevelEnvironmentSet(logEnvLevel);
    CsrLogLevelTaskSetAll(logTaskLevel);
    CsrLogLevelTextSetAll(logTextLevel);

    CsrLogTechInfoRegister();
    CsrBtLogTechInfoRegister();
}
