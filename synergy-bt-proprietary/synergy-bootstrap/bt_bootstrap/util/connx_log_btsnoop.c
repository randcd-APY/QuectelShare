/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>

#include "connx_log_btsnoop.h"
#include "connx_file.h"
#include "connx_time.h"


#define CONNX_HAVE_64_BIT_INTEGERS

#define BTSNOOP_MAGIC_STRING            "btsnoop"

/* Total size for the string "btsnoop" */
#define BTSNOOP_MAGIC_SIZE              8

#define BTSNOOP_VERSION_NUMBER          0x1

#define BTSNOOP_DATA_LINK_TYPE          0x03E9  /* 1001 */

#define BTSNOOP_HEADER_SIZE             (sizeof(BtsnoopRecordHeader))

#define BTSNOOP_RECORD_SIZE(len)        (sizeof(BtsnoopRecordHeader) + (len))
#define BTSNOOP_RECORD_PAYLOAD(rec)     (((uint8_t *)(rec)) + sizeof(BtsnoopRecordHeader))

#define BTSNOOP_FLAGS_HCI               0x02
#define BTSNOOP_FLAGS_RECEIVED          0x01

#define BSI_GET_FILE_NAME(inst)         ((inst)->fileName)
#define BSI_GET_FILE_HANDLE(inst)       ((inst)->fileHandle)


typedef struct
{
    char               *fileName;       /* BT snoop log file. */
    ConnxFileHandle     fileHandle;     /* File handle for BT snoop log. */
} BtsnoopInstance;

#pragma pack(push, 1)

typedef struct
{
    uint8_t     magic[BTSNOOP_MAGIC_SIZE];
    uint32_t    version_number;
    uint32_t    data_link_type;
} BtsnoopHeader;

typedef struct
{
    uint32_t    original_length;
    uint32_t    included_length;
    uint32_t    packet_flags;
    uint32_t    cumulative_drops;
    uint32_t    timestamp_high;
    uint32_t    timestamp_low;
} BtsnoopRecordHeader;

#pragma pack(pop)


/* "btsnoop" */
static const uint8_t btsnoop_magic[BTSNOOP_MAGIC_SIZE] = { 0x62, 0x74, 0x73, 0x6E, 0x6F, 0x6F, 0x70, 0x00 };

static void BtsnoopGetUtcTime(uint32_t *high, uint32_t *low);
static uint32_t BtsnoopConvertFlags(bool received);
static bool BtsnoopWrite(BtsnoopInstance *inst, const void *data, uint32_t dataLength, size_t *sizeWritten);
static bool BtsnoopWriteHeader(BtsnoopInstance *inst);


static void BtsnoopGetUtcTime(uint32_t *high, uint32_t *low)
{
#ifdef CONNX_HAVE_64_BIT_INTEGERS
    uint64_t msecs = 0;
    /* Make it relative to midnight, 1st Jan 2000 */
    msecs |= ((uint64_t) 0x00dcddb3) << 32;
    msecs |= 0x0f2f8000;

    *low = ConnxTimeGet(high);

    msecs += ((uint64_t) * high) << 32 | *low;

    *high = (uint32_t) (msecs >> 32);
    *low = (uint32_t) (msecs);
#else
    *high = 0;
    *low = 0;
#endif
}

static uint32_t BtsnoopConvertFlags(bool received)
{
    uint32_t flags = 0x00;

    flags |= BTSNOOP_FLAGS_HCI;

    if (received)
    {
        flags |= BTSNOOP_FLAGS_RECEIVED;
    }

    return flags;
}

static bool BtsnoopWrite(BtsnoopInstance *inst, const void *data, uint32_t dataLength, size_t *sizeWritten)
{
    ConnxFileHandle fileHandle;
    ConnxResult result;

    if (!inst || !data || !dataLength)
        return false;

    fileHandle = BSI_GET_FILE_HANDLE(inst);

    result = ConnxFileWrite(data, dataLength, fileHandle, sizeWritten);

    return IS_CONNX_RESULT_SUCCESS(result) ? true : false;
}

static bool BtsnoopWriteHeader(BtsnoopInstance *inst)
{
    BtsnoopHeader header;
    BtsnoopHeader *hdr = &header;
    uint32_t version_number = BTSNOOP_VERSION_NUMBER;
    uint32_t data_link_type = BTSNOOP_DATA_LINK_TYPE;

    memcpy(hdr->magic, btsnoop_magic, BTSNOOP_MAGIC_SIZE);

    CONNX_COPY_UINT32_TO_BIG_ENDIAN(version_number, &hdr->version_number);

    CONNX_COPY_UINT32_TO_BIG_ENDIAN(data_link_type, &hdr->data_link_type);

    return BtsnoopWrite(inst, &header, sizeof(BtsnoopHeader), NULL);
}

bool BtsnoopWriteRecord(ConnxHandle handle, bool received, const void *data, size_t dataLength)
{
    BtsnoopInstance *inst = (BtsnoopInstance *)handle;
    BtsnoopRecordHeader *record;
    uint8_t *payload = NULL;
    uint32_t total_size = BTSNOOP_RECORD_SIZE(dataLength);
    uint32_t flags = BtsnoopConvertFlags(received);
    uint32_t time_high = 0;
    uint32_t time_low = 0;
    uint32_t orig_len = dataLength;
    uint32_t incl_len = orig_len;
    uint32_t cum_drops = 0;

    if (!handle || !data || !dataLength)
        return false;

    record = (BtsnoopRecordHeader *)malloc(total_size);

    if (!record)
        return false;

    BtsnoopGetUtcTime(&time_high, &time_low);

    CONNX_COPY_UINT32_TO_BIG_ENDIAN(orig_len, &record->original_length);
    CONNX_COPY_UINT32_TO_BIG_ENDIAN(incl_len, &record->included_length);
    CONNX_COPY_UINT32_TO_BIG_ENDIAN(flags, &record->packet_flags);
    CONNX_COPY_UINT32_TO_BIG_ENDIAN(cum_drops, &record->cumulative_drops);
    CONNX_COPY_UINT32_TO_BIG_ENDIAN(time_high, &record->timestamp_high);
    CONNX_COPY_UINT32_TO_BIG_ENDIAN(time_low, &record->timestamp_low);

    payload = BTSNOOP_RECORD_PAYLOAD(record);

    memcpy(payload, data, dataLength);

    return BtsnoopWrite(inst, record, total_size, NULL);
}

/* ------------------------------------------------------------------------------------------------------ */

ConnxHandle BtsnoopCreate(const char *fileName)
{
    BtsnoopInstance *inst = NULL;
    ConnxResult result = 0;
    ConnxFileHandle fileHandle = NULL;
    ConnxFileOpenFlags flags = CONNX_FILE_OPEN_FLAGS_CREATE | CONNX_FILE_OPEN_FLAGS_READ_WRITE | CONNX_FILE_OPEN_FLAGS_TRUNCATE;
    ConnxFilePerms perms = CONNX_FILE_PERMS_USER_READ | CONNX_FILE_PERMS_USER_WRITE;

    if (!fileName)
        return NULL;

    inst = (BtsnoopInstance *)malloc(sizeof(BtsnoopInstance));

    if (!inst)
        return NULL;

    memset(inst, 0, sizeof(BtsnoopInstance));

    do
    {
        inst->fileName = ConnxStrDup(fileName);

        if (!inst->fileName)
        {
            break;
        }

        result = ConnxFileOpen(&fileHandle, fileName, flags, perms);

        if (!IS_CONNX_RESULT_SUCCESS(result))
        {
            break;
        }

        inst->fileHandle = fileHandle;

        BtsnoopWriteHeader(inst);
    }
    while (0);

    if (IS_CONNX_RESULT_SUCCESS(result))
    {
        return (ConnxHandle)inst;
    }
    else
    {
        ConnxFileClose(inst->fileHandle);
        free(inst->fileName);

        free(inst);
        return NULL;
    }
}

void BtsnoopDestroy(ConnxHandle handle)
{
    BtsnoopInstance *inst = (BtsnoopInstance *)handle;

    if (!inst)
        return;

    if (inst->fileHandle)
    {
        ConnxFileFlush(inst->fileHandle);
        ConnxFileClose(inst->fileHandle);
        inst->fileHandle = NULL;
    }

    if (inst->fileName)
    {
        free(inst->fileName);
        inst->fileName = NULL;
    }

    free(inst);
}
