/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#ifdef BUILD_FOR_ANDROID
#include "cutils/log.h"
#endif

#include "connx_log.h"


#if defined (ENABLE_CONNX_LOG) || defined (_DEBUG)

#define BPR     16

#define BUFFER_COUNT    1024


typedef struct
{
    pthread_mutex_t     lock;
    uint32_t            outputMask;
    uint32_t            outputMethod;
    FILE               *outputFile;
    bool                initialized;
    char                outputBuffer[BUFFER_COUNT];
} ConnxDebugZones;

static ConnxDebugZones   zoneData;


static void Output()
{
    if ((zoneData.outputMethod & OUTPUT_MODE_FILE) && zoneData.outputFile)
    {
        fprintf(zoneData.outputFile, "%s \n", zoneData.outputBuffer);
    }

    if (zoneData.outputMethod & OUTPUT_MODE_CONSOLE)
    {
#ifdef BUILD_FOR_ANDROID
        ALOGD("%s \n", zoneData.outputBuffer);
#else
        printf("%s \n", zoneData.outputBuffer);
#endif
    }
}

void DebugOut(uint32_t mask, char *format, ...)
{
    size_t len = 0;
    int size_remained = 0;
    va_list arglist;

    if ((!(mask & zoneData.outputMask)) || !zoneData.initialized)
    {
        return;
    }

    pthread_mutex_lock(&zoneData.lock);

    if (mask & DEBUG_ERROR)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[ERROR]");
    }
    else if (mask & DEBUG_WARN)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[WARN]");
    }
    else if (mask & DEBUG_MESSAGE)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[MESSAGE]");
    }
    else if (mask & DEBUG_OUTPUT)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[OUTPUT]");
    }
    else if (mask & DEBUG_BT_ERROR)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[ERROR][BT]");
    }
    else if (mask & DEBUG_BT_WARN)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[WARN][BT]");
    }
    else if (mask & DEBUG_BT_MESSAGE)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[BT]");
    }
    else if (mask & DEBUG_BT_OUTPUT)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[BT]");
    }
    else if (mask & DEBUG_HFG_ERROR)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[ERROR][HFG]");
    }
    else if (mask & DEBUG_HFG_WARN)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[WARN][HFG]");
    }
    else if (mask & DEBUG_HFG_MESSAGE)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[HFG]");
    }
    else if (mask & DEBUG_HFG_OUTPUT)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[HFG]");
    }
    else if (mask & DEBUG_IPC_ERROR)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[ERROR][IPC]");
    }
    else if (mask & DEBUG_IPC_WARN)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[WARN][IPC]");
    }
    else if (mask & DEBUG_IPC_MESSAGE)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[IPC]");
    }
    else if (mask & DEBUG_IPC_OUTPUT)
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[IPC]");
    }
    else
    {
        snprintf((char *) zoneData.outputBuffer, sizeof(zoneData.outputBuffer), "%s ", "[MULTI]");
    }

    len = strlen(zoneData.outputBuffer);

    size_remained = sizeof(zoneData.outputBuffer) - len - 1;

    if (size_remained > 0)
    {
        va_start(arglist, format);
        vsnprintf((char *) &zoneData.outputBuffer[len], size_remained, format, arglist);
        va_end(arglist);
    }

    Output();

    pthread_mutex_unlock(&zoneData.lock);
}

void DumpBuff(uint32_t mask, const uint8_t *buffer, uint32_t bufferSize)
{
    char szLine[5 + 7 + 2 + 4 * BPR];
    char *p = NULL;
    uint32_t i = 0;

    if (!(mask & zoneData.outputMask) || !zoneData.initialized || !buffer || !bufferSize)
    {
        return;
    }

    for (i = 0 ; i < bufferSize ; i += BPR)
    {
        int bpr = bufferSize - i;
        int j;

        if (bpr > BPR)
            bpr = BPR;

        snprintf(szLine, sizeof(szLine), "%04x ", i);
        p = szLine + strlen (szLine);

        for (j = 0 ; j < bpr ; ++j)
        {
            char c = (buffer[i + j] >> 4) & 0xf;

            if (c > 9) c += 'a' - 10;
            else c += '0';

            *p++ = c;
            c = buffer[i + j] & 0xf;

            if (c > 9) c += 'a' - 10;
            else c += '0';

            *p++ = c;
            *p++ = ' ';
        }

        for ( ; j < BPR ; ++j)
        {
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
        }

        *p++ = ' ';
        *p++ = ' ';
        *p++ = ' ';
        *p++ = '|';
        *p++ = ' ';
        *p++ = ' ';
        *p++ = ' ';

        for (j = 0 ; j < bpr ; ++j)
        {
            char c = buffer[i + j];

            if ((c < ' ') || (c >= 127))
                c = '.';

            *p++ = c;
        }

        for ( ; j < BPR ; ++j)
        {
            *p++ = ' ';
        }

        *p++ = '\0';

        DebugOut(mask, TEXT("%s"), szLine);
    }
}

void DebugInitialize(uint32_t outputMode, uint32_t outputMask, const char *outputFilename)
{
    memset(&zoneData, 0, sizeof(ConnxDebugZones));

    pthread_mutex_init(&zoneData.lock, NULL);
    pthread_mutex_lock(&zoneData.lock);

    zoneData.initialized  = true;
    zoneData.outputMethod = outputMode;
    zoneData.outputMask   = outputMask;

    if (zoneData.outputMethod & OUTPUT_MODE_FILE)
    {
        if (outputFilename)
        {
            zoneData.outputFile = fopen((char *) outputFilename, "w");
        }
        else
        {
            zoneData.outputMethod &= ~OUTPUT_MODE_FILE;
        }
    }

    pthread_mutex_unlock(&zoneData.lock);
}

void DebugFlushBuffers(void)
{
    pthread_mutex_lock(&zoneData.lock);

    fflush(zoneData.outputFile);

    pthread_mutex_unlock(&zoneData.lock);
}

void DebugUninitialize(void)
{
    if (zoneData.initialized)
    {
        pthread_mutex_lock(&zoneData.lock);

        if (zoneData.outputFile)
        {
            fflush(zoneData.outputFile);
            fclose(zoneData.outputFile);
        }

        zoneData.outputFile   = NULL;
        zoneData.outputMethod = 0;
        zoneData.initialized  = false;

        pthread_mutex_unlock(&zoneData.lock);
        pthread_mutex_destroy(&zoneData.lock);
    }
}

#endif /* ENABLE_CONNX_LOG || _DEBUG */
