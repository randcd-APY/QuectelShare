/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_unicode.h"
#include "csr_file.h"
#include "csr_dir.h"
#include "csr_util.h"
#include "csr_formatted_io.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"


#define CSR_BT_COPY_BUFFER_SIZE (4096)

/* this array represents the number of days in one non-leap year at
    the beginning of each month */
const int csrBtDaysToMonth[13] =
{
   0,31,59,90,120,151,181,212,243,273,304,334,365
};

/* Wraps old API flags to BT new ones */
static CsrFileOpenFlags csrBtFileOpenFlags(const char *mode)
{
    CsrUint8 index;

    struct {
        const char *mode;
        CsrFileOpenFlags flags;
    } FileOpenFlagsMap[] =
    {
        { "r",   CSR_FILE_OPEN_FLAGS_READ_ONLY },
        { "rb",  CSR_FILE_OPEN_FLAGS_READ_ONLY },

        { "rb+", CSR_FILE_OPEN_FLAGS_READ_WRITE },
        { "r+b", CSR_FILE_OPEN_FLAGS_READ_WRITE },
        { "r+",  CSR_FILE_OPEN_FLAGS_READ_WRITE },

        { "w",   CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE },
        { "wb",  CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE },

        { "wb+", CSR_FILE_OPEN_FLAGS_READ_WRITE|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE },
        { "w+b", CSR_FILE_OPEN_FLAGS_READ_WRITE|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE },
        { "w+",  CSR_FILE_OPEN_FLAGS_READ_WRITE|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_TRUNCATE },

        { "a",   CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_APPEND },
        { "ab",  CSR_FILE_OPEN_FLAGS_WRITE_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_APPEND },

        { "ab+", CSR_FILE_OPEN_FLAGS_READ_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_APPEND },
        { "a+b", CSR_FILE_OPEN_FLAGS_READ_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_APPEND },
        { "a+",  CSR_FILE_OPEN_FLAGS_READ_ONLY|CSR_FILE_OPEN_FLAGS_CREATE|CSR_FILE_OPEN_FLAGS_APPEND }
    };

    for (index = 0; index < CSR_ARRAY_SIZE(FileOpenFlagsMap); ++index)
    {
        if (CsrStrCmp(mode, FileOpenFlagsMap[index].mode) == 0)
        {
            return FileOpenFlagsMap[index].flags;
        }
    }

    return 0;
}

CsrFileHandle *CsrBtFopen(const CsrCharString *filename, const char *mode)
{
    CsrFileHandle *handle;
    CsrResult res;

    CsrFilePerms perms = CSR_FILE_PERMS_USER_READ|
                         CSR_FILE_PERMS_USER_WRITE;

    CsrFileOpenFlags flags = csrBtFileOpenFlags(mode);

    res = CsrFileOpen(&handle, (const CsrUtf8String *) filename, flags, perms);
    if (res == CSR_RESULT_SUCCESS)
    {
        return handle;
    }
    else
    {
        return NULL;
    }
}

void *CsrBtFreopen(const CsrCharString *filename, const char *mode, CsrFileHandle *handle)
{
    if (CsrBtFclose(handle) == 0)
    {
        return CsrBtFopen(filename, mode);
    }
    else
    {
        return NULL;
    }
}

CsrInt32 CsrBtFclose(CsrFileHandle *handle)
{
    return CsrFileClose(handle) == CSR_RESULT_SUCCESS ? 0 : -1;
}

CsrSize CsrBtFwrite(const void *buffer, CsrSize size, CsrSize count, CsrFileHandle *handle)
{
    CsrSize written;
    if (CsrFileWrite(buffer, size * count, handle, &written) == CSR_RESULT_SUCCESS)
    {
        return written;
    }
    else
    {
        return 0;
    }
}

CsrSize CsrBtFread(void *buffer, CsrSize size, CsrSize count, CsrFileHandle *handle)
{
    CsrSize read;
    CsrResult result = CsrFileRead(buffer, size * count, handle, &read);

    if ((result == CSR_RESULT_SUCCESS) || (result == CSR_FILE_RESULT_EOF))
    {
        return read;
    }
    else
    {
        return 0;
    }
}

char *CsrBtFgets(char *buffer, CsrUint32 count, CsrFileHandle *handle)
{
    CsrSize        read = '\0';
    char        *dummy = "";
    CsrResult   res = CSR_RESULT_SUCCESS;
    CsrUint32   i = 0;

    for (i = 0; i < count; i++)
    {
        res = CsrFileRead(&buffer[i], (CsrSize)1, handle, (CsrSize *) &read);

        if (res == CSR_RESULT_SUCCESS)
        {
            if (buffer[i] == '\r' || buffer[i] == '\n')
            {
                CsrInt32    pos = 0;

                buffer[i] = '\0';
                i = count; /* to stop for-loop */
                pos = CsrBtFtell(handle);
                CsrFileRead(&dummy[0], (CsrSize)1, handle, (CsrSize *) &read); /* Read an extra byte to get both the LF and CR char */
                if (!(dummy[0] == '\r' || dummy[0] == '\n'))
                {
                    /* If the next character is not CR or LF, set the file pointer back to the previous position */
                    CsrBtFseek(handle, pos, CSR_SEEK_SET);
                }
            }
        }
        else if (res == CSR_FILE_RESULT_EOF)
        {
            buffer[i] = '\0';
            if (i == 0)
            {
                return NULL;
            }
            else
            {
                i = count; /* to stop for-loop */
            }
        }
        else
        {
            return NULL;
        }
    }

    return buffer;
}

CsrInt32 CsrBtFseek(CsrFileHandle *handle, CsrInt32 offset, CsrInt32 relativeOffset)
{
    return CsrFileSeek(handle, offset, relativeOffset) == CSR_RESULT_SUCCESS ? 0 : -1;
}

CsrInt32 CsrBtFflush(CsrFileHandle *handle)
{
    return CsrFileFlush(handle) == CSR_RESULT_SUCCESS ? 0 : -1;
}

CsrBool CsrBtSetEndOfFile(CsrFileHandle *handle)
{
    return CsrFileSetEndOfFile(handle) == CSR_RESULT_SUCCESS;
}

CsrInt32 CsrBtDiskstat(CsrCharString *filename, CsrBtPosixStatBuffer *statData)
{
    CsrFileHandle *handle = CsrBtFopen(filename, "rb");
    if (handle)
    {
        if (CsrBtFseek(handle, 0, CSR_SEEK_END) == 0)
        {
            CsrInt32 size = CsrBtFtell(handle);
            if (size >= 0)
            {
                statData->st_size = (CsrUint32) size;
                (void) CsrBtFclose(handle);
                return 0;
            }
        }
        /* [QTI] Fix KW issue#831572/831573 through moving the statement into this code block. */
        (void) CsrBtFclose(handle);
    }

    return -1;
}

CsrUint32 CsrBtFileSize(CsrCharString *filename)
{
    CsrBtPosixStatBuffer stat;
    if(CsrBtDiskstat(filename, &stat) == 0)
    {
        return stat.st_size;
    }
    else
    {
        return 0;
    }
}

CsrInt32 CsrBtRemove(const char *filename)
{
    return CsrFileRemove((const CsrUtf8String *) filename) == CSR_RESULT_SUCCESS ? 0 : -1;
}

CsrInt32 CsrBtFtell(CsrFileHandle *handle)
{
    CsrUint32 position;
    if (CsrFileTell(handle, &position) == CSR_RESULT_SUCCESS)
    {
        return (CsrInt32) position;
    }
    else
    {
        return -1;
    }
}

CsrBool CsrBtIsAtEof(CsrFileHandle *handle, CsrUint32 size)
{
    CsrInt32 t;
    t = CsrBtFtell(handle);
    if((t < 0) || (t < (CsrInt32)size))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int CsrBtChdir(const char *path)
{
    if (CsrDirChange((CsrUtf8String *) path) == CSR_RESULT_SUCCESS)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int CsrBtMkDir(const char *path)
{
    if (CsrDirMake((CsrUtf8String *) path) == CSR_RESULT_SUCCESS)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int CsrBtRmDir(const char *path)
{
    if (CsrDirRemove((CsrUtf8String *) path) == CSR_RESULT_SUCCESS)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

char *CsrBtGetCwd(char *buf, size_t size)
{
    CsrUtf8String *path;

    if (CsrDirGetCurrentWorkingDir(&path) == CSR_RESULT_SUCCESS)
    {
        CsrUtf8StrNCpyZero((CsrUtf8String *) buf, path, size);
        CsrPmemFree(path);

        return ((char *) buf);
    }

    return NULL;
}


static int CsrBtDateToInt(CsrDirTm datetime)
{

   int iday;
   int val;
   CsrUint8 monthIdx = (CsrUint8)datetime.tm_mon;
   CsrUint8 dayNr    = (CsrUint8)datetime.tm_mday;
   /* tm_year is the number of years since 1900 and we want a date in seconds since Jan 1. 1970 */
   if (monthIdx > 0)
   {
       monthIdx--;
   }
   if (dayNr > 0)
   {
       dayNr--;
   }

   iday = 365 * (datetime.tm_year - 70) + csrBtDaysToMonth[monthIdx] + dayNr;
   iday = iday + (datetime.tm_year - 69) / 4;
   if ((datetime.tm_mon > 2) && ((datetime.tm_year % 4) == 0))
   {
      iday++;
   }
   val = datetime.tm_sec + 60 * datetime.tm_min + 3600 * (datetime.tm_hour + 24 * iday);
   return val;
}


int CsrBtStat(const char *path, CsrBtStat_t *buf)
{
    CsrDirEntryStat dirEntryStat;

    if (CsrDirStat((CsrUtf8String *) path, &dirEntryStat) == CSR_RESULT_SUCCESS)
    {
        buf->st_mode = 0;
        if (dirEntryStat.mode & CSR_DIR_MODE_DIRECTORY)
        {
            buf->st_mode |= S_IFDIR;
        }
        if (dirEntryStat.mode & CSR_DIR_MODE_REGULAR_FILE)
        {
            buf->st_mode |= S_IFREG;
        }
        if (dirEntryStat.mode & CSR_DIR_MODE_USER_WRITE_PERMISSION)
        {
            buf->st_mode |= S_IWUSR;
        }
        buf->st_size = dirEntryStat.size;
        buf->st_ctime = CsrBtDateToInt(dirEntryStat.time);
        return 0;
    }

    return -1;
}

int CsrBtAccess(const char *path, int amode)
{
    CsrDirEntryStat dirEntryStat;

    if (CsrDirStat((CsrUtf8String *) path, &dirEntryStat) == CSR_RESULT_SUCCESS)
    {
        if ((amode & 2) && !(dirEntryStat.mode & CSR_DIR_MODE_USER_WRITE_PERMISSION))
        {
            return -1;
        }
        if ((amode & 4) && !(dirEntryStat.mode & CSR_DIR_MODE_USER_READ_PERMISSION))
        {
            return -1;
        }
        return 0;
    }

    return -1;
}

DIR *CsrBtOpenDir(const char *name)
{
    DIR *dir = NULL;
    CsrSize dir_name_size = 0;

    if(name && name[0])
    {
        size_t base_length = CsrStrLen(name);
        const char *all;

        /* Append wildcard search pattern */
        if(name[base_length - 1] == '\\' || name[base_length - 1] == '/')
        {
            all = "*";
        }
        else
        {
            all = "";
        }

        dir_name_size = base_length + CsrStrLen(all) + 1;

        /* Build directory name and pattern */
        if((dir = CsrPmemAlloc(sizeof *dir)) != 0 &&
           (dir->name = CsrPmemAlloc(dir_name_size)) != 0)
        {
            CsrStrLCpy(dir->name, name, dir_name_size);
            CsrStrLCat(dir->name, all, dir_name_size);
            if((dir->handle = CsrDirFindFirst((CsrUtf8String *) dir->name, &dir->info)) != NULL)
            {
                dir->result.d_name = 0;
            }
            else
            {
                /* Rollback */
                CsrPmemFree(dir->name);
                CsrPmemFree(dir);
                dir = NULL;
            }
        }
        else
        {
             /* Rollback */
            CsrPmemFree(dir);
            dir   = NULL;
        }
    }

    return dir;
}

struct dirent *CsrBtReadDir(DIR *dir)
{
    struct dirent *result = NULL;

    if(dir && (dir->handle != NULL))
    {
        if((dir->result.d_name == NULL) ||
           (CsrDirFindNext(dir->handle, &dir->info) == CSR_RESULT_SUCCESS))
        {
            result         = &dir->result;
            result->d_name = (char *) dir->info.name;
        }
    }

    return result;
}

int CsrBtCloseDir(DIR *dir)
{
    CsrResult result = CSR_RESULT_FAILURE;

    if(dir)
    {
        if(dir->handle != NULL)
        {
            result = CsrDirFindClose(dir->handle);
        }
        CsrPmemFree(dir->name);
        CsrPmemFree(dir);
    }

    return ((result == CSR_RESULT_SUCCESS) ? 0 : -1);
}


CsrResult CsrBtCopy(const CsrCharString *srcName,const CsrCharString *destName)
{
    CsrFileHandle *srcHandle;
    CsrBool dirCopy = FALSE;
    CsrCharString *dirPath = NULL;

    /* Ensure that the src-file exists and can be opened for read */
    srcHandle = CsrBtFopen(srcName,"rb");

    if ( srcHandle == NULL)
    {
        DIR *dir;
        /* Could be a directory so lets check */
        dir = CsrBtOpenDir(srcName);

        if (dir == NULL)
        {
            return(CSR_DIR_RESULT_NOT_EXIST);
        }
        else
        {
            dirCopy = TRUE;
            CsrBtCloseDir(dir);
            /* Do nothing */
        }
    }
    else
    {
        dirCopy = FALSE;
        CsrBtFclose(srcHandle);
    }

    if (dirCopy)
    {
        DIR *dir;

        /* Check if dest dir exists */
        dir = CsrBtOpenDir(destName);

        if (dir == NULL)
        {
            if (CsrBtMkDir(destName) == 0)
            {
                /* Do nothing */
            }
            else
            {
                return(CSR_DIR_RESULT_INVALID_PATH);
            }
        }
        else
        {
            /* Do nothing */
        }


        dirPath = CsrPmemZalloc(CSR_BT_MAX_PATH_LENGTH);
        CsrStrLCat(dirPath, srcName, CSR_BT_MAX_PATH_LENGTH);
        CsrStrLCat(dirPath, PATH_SEP, CSR_BT_MAX_PATH_LENGTH);
        dir = CsrBtOpenDir(dirPath);

        if (dir == NULL)
        {
            return(CSR_DIR_RESULT_INVALID_PATH);
        }
        else
        {
            struct dirent *file;
            CsrResult res=0;

            while((file = CsrBtReadDir(dir)) != NULL)
            {
                CsrCharString *newSrc = NULL;
                CsrCharString *newDest = NULL;

                if ((CsrStrCmp(file->d_name,"..") == 0) || (CsrStrCmp(file->d_name,".")== 0))
                {
                    /* Do nothing */
                    res = 0;
                }
                else
                {
                    newSrc = CsrPmemZalloc(CSR_BT_MAX_PATH_LENGTH);
                    newDest = CsrPmemZalloc(CSR_BT_MAX_PATH_LENGTH);

                    CsrStrLCat(newSrc, srcName, CSR_BT_MAX_PATH_LENGTH);
                    CsrStrLCat(newSrc, PATH_SEP, CSR_BT_MAX_PATH_LENGTH);
                    CsrStrLCat(newSrc, file->d_name, CSR_BT_MAX_PATH_LENGTH);

                    CsrStrLCat(newDest, destName, CSR_BT_MAX_PATH_LENGTH);
                    CsrStrLCat(newDest, PATH_SEP, CSR_BT_MAX_PATH_LENGTH);
                    CsrStrLCat(newDest, file->d_name, CSR_BT_MAX_PATH_LENGTH);

                    res = CsrBtCopy(newSrc,newDest);

                    CsrPmemFree(newSrc);
                    CsrPmemFree(newDest);
                }

                if (res != 0)
                {
                    return(res);
                }
                else
                {
                    /* Proceed */
                }
            }
        }
    }
    else
    {
        CsrFileHandle *srcFile;
        CsrFileHandle *destFile;
        CsrSize byteRead;
        CsrUint8 *buffer;

        srcFile = CsrBtFopen(srcName,"rb");
        destFile = CsrBtFopen(destName,"wb");

        /* [QTI] Fix KW issue#831588 ~ 831590 through adding the check "srcFile" and "destFile". */
        if (srcFile && destFile)
        {
            buffer = CsrPmemAlloc(CSR_BT_COPY_BUFFER_SIZE);
            
            while((byteRead = CsrBtFread(buffer,CSR_BT_COPY_BUFFER_SIZE,1,srcFile)) > 0)
            {
                CsrBtFwrite(buffer,byteRead,1,destFile);
            }
            
            CsrBtFclose(srcFile);
            CsrBtFclose(destFile);
        }
    }

    return(0);
}
