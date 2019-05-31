#ifndef CSR_BT_CSR_BT_PLATFORM_H__
#define CSR_BT_CSR_BT_PLATFORM_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_file.h"
#include "csr_dir.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__
#define RETURN_KEY      0x0A
#define BACKSPACE_KEY   0x7f
#define CLS             "clear"
#else
#define RETURN_KEY      0x0D
#define BACKSPACE_KEY   0x08
#define CLS             "cls"
#endif

#define ESCAPE_KEY      0x1B

extern int _kbhit(void);
extern int _getch(void);
extern void system_cls(void);

extern char baudRate[10];

/* Keyboard press encapsulated in a Synergy BT message */
#define KEY_MESSAGE     0xABBA
typedef struct
{
    CsrUint16 type;
    CsrUint8 key;
} CsrBtKeyPress_t;


struct dirent
{
    char *d_name;
};

typedef struct
{
    CsrDirHandle        *handle;
    CsrDirFindStat      info;
    struct dirent       result; /* d_name null if first time */
    char                *name;  /* Actual name */
} DIR;


#if defined(__linux__)
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <linux/limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if 0
#include <dirent.h>
#endif

#include <errno.h>

typedef struct stat CsrBtStat_t;

#if 0
typedef struct
{
    int    st_mode;    /* protection */
    int    st_size;    /* total size, in bytes */
    int    st_ctime;   /* time of last status change */
} CsrBtStat_t;
#endif


#define CSRBTTEXT(t)         t
#define CSR_BT_MAX_PATH_LENGTH PATH_MAX
extern int CsrBtAccess(const char *path, int amode);
#define CsrBtCTime           ctime
#define CsrBtChar            char
extern int CsrBtChdir(const char *path);
extern int CsrBtCloseDir(DIR *dir);
#define CsrBtFTell           ftell
extern char *CsrBtGetCwd(char *buf, size_t size);
#define CsrBtIsSpace         isspace
extern int CsrBtMkDir(const char *path);
extern DIR *CsrBtOpenDir(const char *name);
#define CsrBtPrintf          printf
extern struct dirent *CsrBtReadDir(DIR *dir);
extern int CsrBtRmDir(const char *path);
#define CsrBtSScanf          sscanf
#define CsrBtSleep(x)        sleep((x))
#define CsrBtSprintf         sprintf
extern int CsrBtStat(const char *path, CsrBtStat_t *buf);
#define CsrBtStrNICmp        strnicmp
#define CsrBtStrRChr         strrchr
#define CsrBtVSPrintf        vsprintf
#define PATH_SEP "/"


#elif defined(__arm)

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define CSR_BT_OBEX_DUMMY_FILE_SIZE (100*1024*1024)

typedef struct
{
    int    st_mode;    /* protection */
    int    st_size;    /* total size, in bytes */
    int    st_ctime;   /* time of last status change */
} CsrBtStat_t;

#define CSR_BT_MAX_PATH_LENGTH  128
#define S_IFDIR     0040000
#define S_IFREG     0100000
#define S_IWUSR     00200

#define EACCES      0x1
#define ENOTEMPTY   0x2

extern DIR *CsrBtOpenDir(const char *name);
extern char *CsrBtGetCwd(char *buf, size_t size);
extern int CsrBtAccess(const char *path, int amode);
extern int CsrBtChdir(const char *path);
extern int CsrBtCloseDir(DIR *dir);
extern int CsrBtMkDir(const char *path);
extern int CsrBtRmDir(const char *path);
extern int CsrBtStat(const char *path, CsrBtStat_t *buf);
extern struct dirent *CsrBtReadDir(DIR *dir);


#define PATH_SEP "/"
#define CSRBTTEXT(t)         t
#define CsrBtChar            char
#define CsrBtSprintf         sprintf
#define CsrBtPrintf          printf
#define CsrBtVSPrintf        vsprintf
#define CsrBtStrRChr         strrchr
#define CsrBtStrNICmp        strnicmp
#define CsrBtSScanf          sscanf
#define CsrBtIsSpace         isspace
#define CsrBtCTime           ctime
#define CsrBtFTell           ftell
#define CsrBtSleep(x)        sleep((x))

#else /* WINDOWS */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <io.h>
#include <errno.h>

#ifndef _DEP_BUILD_
#include <direct.h>
#endif

#include "csr_types.h"

#define CSR_BT_MAX_PATH_LENGTH _MAX_PATH
#define PATH_SEP "/"

#define S_IWUSR S_IWRITE

typedef struct
{
    int    st_mode;    /* protection */
    int    st_size;    /* total size, in bytes */
    int    st_ctime;   /* time of last status change */
} CsrBtStat_t;

extern DIR                  *CsrBtOpenDir(const char *name);
extern int                  CsrBtCloseDir(DIR *dir);
extern struct dirent        *CsrBtReadDir(DIR *dir);

#define CSRBTTEXT(t)        L##t
#define CsrBtChar            wchar_t
#define CsrBtPrintf          printf
#define CsrBtSprintf         sprintf
#define CsrBtVSPrintf        vsprintf
extern char *CsrBtGetCwd(char *buf, size_t size);
extern int CsrBtChdir(const char *path);
extern int CsrBtMkDir(const char *path);
extern int CsrBtRmDir(const char *path);
#define CsrBtStrRChr         strrchr
#define CsrBtStrNICmp        strnicmp
#define CsrBtSScanf          sscanf
extern int CsrBtStat(const char *path, CsrBtStat_t *buf);
extern int CsrBtAccess(const char *path, int amode);
#define CsrBtIsSpace         isspace
#define CsrBtCTime           ctime
#define CsrBtWStat           _wstat
#define CsrBtFreeStr2Utf8    CsrPmemFree
#define CsrBtFTell           ftell
#define CsrBtSleep(x)        Sleep((x)/100)

#endif

#ifdef __cplusplus
}
#endif

#endif

