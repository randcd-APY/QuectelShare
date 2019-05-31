#ifndef CSR_DIR_H__
#define CSR_DIR_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"
#include "csr_result.h"
#include "csr_time.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_DIR_PATH_SEP '/'

typedef CsrUint16 CsrDirMode;
#define CSR_DIR_MODE_DIRECTORY                 ((CsrDirMode) 0x0001)
#define CSR_DIR_MODE_REGULAR_FILE              ((CsrDirMode) 0x0002)
#define CSR_DIR_MODE_USER_READ_PERMISSION      ((CsrDirMode) 0x0004)
#define CSR_DIR_MODE_USER_WRITE_PERMISSION     ((CsrDirMode) 0x0008)
#define CSR_DIR_MODE_USER_EXECUTE              ((CsrDirMode) 0x0010)
#define CSR_DIR_MODE_GROUP_READ_PERMISSION     ((CsrDirMode) 0x0020)
#define CSR_DIR_MODE_GROUP_WRITE_PERMISSION    ((CsrDirMode) 0x0040)
#define CSR_DIR_MODE_GROUP_EXECUTE             ((CsrDirMode) 0x0080)
#define CSR_DIR_MODE_OTHERS_READ_PERMISSION    ((CsrDirMode) 0x0100)
#define CSR_DIR_MODE_OTHERS_WRITE_PERMISSION   ((CsrDirMode) 0x0200)
#define CSR_DIR_MODE_OTHERS_EXECUTE            ((CsrDirMode) 0x0400)

typedef struct
{
    CsrTime tm_sec;        /* Seconds: 0-59                                            */
    CsrTime tm_min;        /* Minutes: 0-59                                            */
    CsrTime tm_hour;       /* Hours since midnight: 0-23                               */
    CsrTime tm_mday;       /* Day of the month: 1-31                                   */
    CsrTime tm_mon;        /* Months *since* january: 0-11                             */
    CsrTime tm_year;       /* Years since 1900                                         */
    CsrTime tm_wday;       /* Days since Sunday (0-6)                                  */
    CsrTime tm_yday;       /* Days since Jan. 1: 0-365                                 */
    CsrTime tm_isdst;      /* +1 Daylight Savings Time, 0 No DST, 0xFFFF don't know    */
    CsrBool utcTime;       /* TRUE if given in UTC time, FALSE if given in local time  */
} CsrDirTm;

typedef struct
{
    CsrSize    size;  /* File size */
    CsrDirMode mode;  /* Mode */
    CsrDirTm   time;  /* Time of last write/modification */
} CsrDirEntryStat;

typedef struct
{
    CsrUtf8String *name;
    CsrSize        size;
    CsrDirMode     mode;
    CsrDirTm       time; /* Time of last write/modification */
} CsrDirFindStat;

typedef void CsrDirHandle;

/* All Directory operations use these error codes if it fails, success is CSR_RESULT_SUCCESS */
#define CSR_DIR_RESULT_FAILURE          ((CsrResult) 1)
#define CSR_DIR_RESULT_NOT_EXIST        ((CsrResult) 2)
#define CSR_DIR_RESULT_ALREADY_EXIST    ((CsrResult) 3)
#define CSR_DIR_RESULT_NOT_EMPTY        ((CsrResult) 4)
#define CSR_DIR_RESULT_INVALID_PATH     ((CsrResult) 5)

CsrResult CsrDirGetCurrentWorkingDir(CsrUtf8String **dirName);

CsrResult CsrDirStat(const CsrUtf8String *path, CsrDirEntryStat *fileStat);

CsrResult CsrDirMake(const CsrUtf8String *dirName);

CsrResult CsrDirRemove(const CsrUtf8String *dirName);

CsrResult CsrDirChange(const CsrUtf8String *dirName);

CsrDirHandle *CsrDirFindFirst(const CsrUtf8String *searchPattern, CsrDirFindStat *dirStat);

CsrResult CsrDirFindNext(CsrDirHandle *handle, CsrDirFindStat *dirStat);

CsrResult CsrDirFindClose(CsrDirHandle *handle);

#ifdef __cplusplus
}
#endif

#endif
