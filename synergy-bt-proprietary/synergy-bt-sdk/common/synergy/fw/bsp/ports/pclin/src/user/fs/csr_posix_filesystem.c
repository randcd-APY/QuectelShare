/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef BUILD_FOR_ANDROID
/* The macro "ANDRIOD" is obsolote for Android N-Car in MSM platform. */
#undef ANDROID
#endif

#include <time.h>
#ifdef ANDROID
#include <time64.h>
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "csr_synergy.h"
#include "csr_file.h"
#include "csr_dir.h"
#include "csr_util.h"
#include "csr_unicode.h"
#include "csr_pmem.h"

static int returnMappedFlags(CsrFileOpenFlags mode)
{
    int ret = 0;

    ret |= (mode & CSR_FILE_OPEN_FLAGS_APPEND) ? O_APPEND : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_READ_ONLY) ? O_RDONLY : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_READ_WRITE) ? O_RDWR : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_WRITE_ONLY) ? O_WRONLY : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_CREATE) ? O_CREAT : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_TRUNCATE) ? O_TRUNC : 0;
    ret |= (mode & CSR_FILE_OPEN_FLAGS_EXCL) ? O_EXCL : 0;

    return ret;
}

static mode_t returnMappedPerms(CsrFilePerms mode)
{
    mode_t ret = 0;

    /* note that read and write flags are the same for all user/group/others and execute flags are ignored on windows */
    ret |= ((mode & CSR_FILE_PERMS_USER_READ) ? S_IRUSR : 0);
    ret |= ((mode & CSR_FILE_PERMS_USER_WRITE) ? S_IWUSR : 0);
    ret |= ((mode & CSR_FILE_PERMS_USER_EXECUTE) ? S_IXUSR : 0);
    ret |= ((mode & CSR_FILE_PERMS_GROUP_READ) ? S_IRGRP : 0);
    ret |= ((mode & CSR_FILE_PERMS_GROUP_WRITE) ? S_IWGRP : 0);
    ret |= ((mode & CSR_FILE_PERMS_GROUP_EXECUTE) ? S_IXGRP : 0);
    ret |= ((mode & CSR_FILE_PERMS_OTHERS_READ) ? S_IROTH : 0);
    ret |= ((mode & CSR_FILE_PERMS_OTHERS_WRITE) ? S_IWOTH : 0);
    ret |= ((mode & CSR_FILE_PERMS_OTHERS_EXECUTE) ? S_IXOTH : 0);

    return ret;
}

typedef struct
{
    int handle;
} csrFileHandle;

#define MAP_FILE_OPEN(result) (result != -1 ? CSR_RESULT_SUCCESS : \
                               errno == EACCES ? CSR_FILE_RESULT_NOT_ALLOWED : \
                               errno == EEXIST ? CSR_FILE_RESULT_ALREAD_EXISTS : \
                               errno == ENOENT ? CSR_FILE_RESULT_NOT_EXIST : \
                               CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileOpen(CsrFileHandle **handle, const CsrUtf8String *fileName, CsrFileOpenFlags flags, CsrFilePerms perms)
{
    int linFlags = returnMappedFlags(flags);
    mode_t mode = returnMappedPerms(perms);
    csrFileHandle *fsalHandle = CsrPmemAlloc(sizeof(csrFileHandle));
    int fd;

    fd = open((char *) fileName, linFlags, mode);

    if (fd >= 0)
    {
        fsalHandle->handle = fd;
        *handle = fsalHandle;
    }
    else
    {
        CsrPmemFree(fsalHandle);
        *handle = NULL;
    }

    return MAP_FILE_OPEN(fd);
}

#define MAP_FILE_CLOSE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileClose(CsrFileHandle *handle)
{
    int result;
    csrFileHandle *fsalHandle = handle;
    result = close(fsalHandle->handle);
    CsrPmemFree(handle);
    return MAP_FILE_CLOSE(result);
}

#define MAP_FILE_WRITE(result, _bytesToWrite) (result == _bytesToWrite ? CSR_RESULT_SUCCESS : \
                                               errno == EBADF ? CSR_FILE_RESULT_NOT_ALLOWED : \
                                               errno == ENOSPC ? CSR_FILE_RESULT_NO_SPACE : \
                                               CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileWrite(const void *buffer, CsrSize bytesToWrite,
    CsrFileHandle *handle, CsrSize *written)
{
    csrFileHandle *fsalHandle = handle;
    *written = write(fsalHandle->handle, buffer, bytesToWrite);
    return MAP_FILE_WRITE(*written, bytesToWrite);
}

#define MAP_FILE_READ(result, _handle, _bytesToRead) ((result > 0) ? \
                                                      (result == _bytesToRead ? CSR_RESULT_SUCCESS : \
                                                       result == 0 ? CSR_FILE_RESULT_EOF : CSR_FILE_RESULT_FAILURE)) : \
    errno == EBADF ? CSR_FILE_RESULT_NOT_ALLOWED : \
    CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileRead(void *buffer, CsrSize bytesToRead,
    CsrFileHandle *handle, CsrSize *bytesRead)
{
    csrFileHandle *fsalHandle = handle;
    *bytesRead = read(fsalHandle->handle, buffer, bytesToRead);

    if (*bytesRead > 0)
    {
        return CSR_RESULT_SUCCESS;
    }
    else if (*bytesRead == 0)
    {
        return CSR_FILE_RESULT_EOF;
    }
    else
    {
        if (errno == EBADF)
        {
            return CSR_FILE_RESULT_NOT_ALLOWED;
        }
        else
        {
            return CSR_RESULT_FAILURE;
        }
    }
}

#define MAP_FILE_SEEK(result) (result >= 0 ? CSR_RESULT_SUCCESS : \
                               errno == EBADF ? CSR_FILE_RESULT_NOT_ALLOWED : \
                               CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileSeek(CsrFileHandle *handle, CsrInt32 offset, CsrInt32 relativeOffset)
{
    csrFileHandle *fsalHandle = handle;
    int o, result;

    switch (relativeOffset)
    {
        case CSR_SEEK_SET:
            o = SEEK_SET;
            break;

        case CSR_SEEK_CUR:
            o = SEEK_CUR;
            break;

        case CSR_SEEK_END:
            o = SEEK_END;
            break;

        default:
            o = SEEK_SET;
            break;
    }

    result = lseek(fsalHandle->handle, offset, o);
    return MAP_FILE_SEEK(result);
}

#define MAP_FILE_FLUSH(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                errno == EBADF ? CSR_FILE_RESULT_NOT_ALLOWED : \
                                CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileFlush(CsrFileHandle *handle)
{
    csrFileHandle *fsalHandle = handle;
    int result;
    result = fsync(fsalHandle->handle);
    return MAP_FILE_FLUSH(result);
}

#define MAP_FILE_SET_END_OF_FILE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                          CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileSetEndOfFile(CsrFileHandle *handle)
{
    csrFileHandle *fsalHandle = handle;
    off_t pos;
    int result = -1;

    pos = lseek(fsalHandle->handle, 0, SEEK_CUR);
    if (pos != (off_t) -1)
    {
        result = ftruncate(fsalHandle->handle, pos);
    }

    return MAP_FILE_SET_END_OF_FILE(result);
}

#define MAP_FILE_PERMS(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                errno == ENOENT ? CSR_FILE_RESULT_NOT_EXIST : \
                                CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileSetPerms(const CsrUtf8String *name, CsrFilePerms perms)  /* NB: May also be used on directories */
{
    mode_t mode;
    int result;

    mode = returnMappedPerms(perms);
    result = chmod((char *) name, mode);

    return MAP_FILE_PERMS(result);
}

#define MAP_FILE_REMOVE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                 errno == EACCES ? CSR_FILE_RESULT_NOT_ALLOWED : \
                                 errno == ENOENT ? CSR_FILE_RESULT_NOT_EXIST : \
                                 CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileRemove(const CsrUtf8String *fileName)
{
    int result;

    result = unlink((char *) fileName);

    return MAP_FILE_REMOVE(result);
}

#define MAP_FILE_RENAME(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                 errno == EACCES ? CSR_FILE_RESULT_NOT_ALLOWED : \
                                 errno == ENOENT ? CSR_FILE_RESULT_NOT_EXIST : \
                                 CSR_FILE_RESULT_FAILURE)

CsrResult CsrFileRename(const CsrUtf8String *oldName, const CsrUtf8String *newName) /* NB: May also be used on directories */
{
    int result;

    result = rename((char *) oldName, (char *) newName);

    return MAP_FILE_RENAME(result);
}

CsrResult CsrFileTell(CsrFileHandle *handle, CsrUint32 *position)
{
    off_t rv;
    csrFileHandle *fsalHandle = handle;

    rv = lseek(fsalHandle->handle, (off_t) 0, SEEK_CUR);

    *position = (CsrUint32) rv;

    return CSR_RESULT_SUCCESS;
}

#define MAP_DIR_STAT(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                              errno == ENOENT ? CSR_DIR_RESULT_NOT_EXIST : \
                              CSR_DIR_RESULT_FAILURE)

static CsrDirMode convertModeToDirMode(mode_t st_mode)
{
    CsrDirMode mode;

    /* Files that are not directories are files. */
    mode = S_ISDIR(st_mode) ? CSR_DIR_MODE_DIRECTORY :
           CSR_DIR_MODE_REGULAR_FILE;
    mode |= st_mode & S_IRUSR ? CSR_DIR_MODE_USER_READ_PERMISSION : 0;
    mode |= st_mode & S_IWUSR ? CSR_DIR_MODE_USER_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXUSR ? CSR_DIR_MODE_USER_EXECUTE : 0;
    mode |= st_mode & S_IRGRP ? CSR_DIR_MODE_GROUP_READ_PERMISSION : 0;
    mode |= st_mode & S_IWGRP ? CSR_DIR_MODE_GROUP_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXGRP ? CSR_DIR_MODE_GROUP_EXECUTE : 0;
    mode |= st_mode & S_IROTH ? CSR_DIR_MODE_OTHERS_READ_PERMISSION : 0;
    mode |= st_mode & S_IWOTH ? CSR_DIR_MODE_OTHERS_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXOTH ? CSR_DIR_MODE_OTHERS_EXECUTE : 0;

    return mode;
}

#ifdef ANDROID
static void convertToDirTm(CsrDirTm *out, time64_t *in)
#else
static void convertToDirTm(CsrDirTm *out, time_t *in)
#endif
{
    struct tm *calTime;

    /* Fill out time in UTC */
#ifdef ANDROID
    calTime = gmtime64(in);
#else
    calTime = gmtime(in);
#endif

    /* [QTI] Fix KW issue#835858 through adding the check "calTime". */
    if (calTime)
    {
        out->tm_sec = calTime->tm_sec;
        out->tm_min = calTime->tm_min;
        out->tm_hour = calTime->tm_hour;
        out->tm_mday = calTime->tm_mday;
        out->tm_mon = calTime->tm_mon;
        out->tm_year = calTime->tm_year;
        out->tm_wday = calTime->tm_wday;
        out->tm_yday = calTime->tm_yday;
        out->tm_isdst = calTime->tm_isdst;
    }
    
    out->utcTime = TRUE;
}

CsrResult CsrDirGetCurrentWorkingDir(CsrUtf8String **dirName)
{
    char buf[MAXPATHLEN];

    if (getcwd(buf, MAXPATHLEN) != NULL)
    {
        *dirName = (CsrUtf8String *) CsrStrDup((CsrCharString *) buf);

        return CSR_RESULT_SUCCESS;
    }
    else /* it failed */
    {
        *dirName = NULL;

        return CSR_DIR_RESULT_FAILURE;
    }
}

CsrResult CsrDirStat(const CsrUtf8String *path, CsrDirEntryStat *fileStat)
{
    int result;
    struct stat temp;

    result = stat((char *) path, &temp);

    if (result == 0)
    {
        /* Fill out time in UTC */
#ifdef ANDROID
        time64_t realTime;

        realTime = temp.st_mtime;
        convertToDirTm(&fileStat->time, &realTime);
#else
        convertToDirTm(&fileStat->time, &temp.st_mtime);
#endif

        /* Convert mode */
        fileStat->mode = convertModeToDirMode(temp.st_mode);

        /* File size */
        fileStat->size = temp.st_size;
    }
    return MAP_DIR_STAT(result);
}

#define MAP_DIR_MAKE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                              errno == EEXIST ? CSR_DIR_RESULT_ALREADY_EXIST : \
                              errno == ENOENT ? CSR_DIR_RESULT_NOT_EXIST : \
                              CSR_DIR_RESULT_FAILURE)

CsrResult CsrDirMake(const CsrUtf8String *dirName)
{
    int result;

    result = mkdir((char *) dirName, S_IRUSR | S_IWUSR | S_IXUSR);

    return MAP_DIR_MAKE(result);
}

#define MAP_DIR_REMOVE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                errno == ENOTEMPTY ? CSR_DIR_RESULT_NOT_EMPTY : \
                                errno == ENOENT ? CSR_DIR_RESULT_INVALID_PATH : \
                                CSR_DIR_RESULT_FAILURE)

CsrResult CsrDirRemove(const CsrUtf8String *dirName)
{
    int result;

    result = rmdir((char *) dirName);

    return MAP_DIR_REMOVE(result);
}

#define MAP_DIR_CHANGE(result) (result == 0 ? CSR_RESULT_SUCCESS : \
                                errno == ENOENT ? CSR_DIR_RESULT_NOT_EXIST : \
                                CSR_DIR_RESULT_FAILURE)

CsrResult CsrDirChange(const CsrUtf8String *dirName)
{
    int result;

    result = chdir((char *) dirName);

    return MAP_DIR_CHANGE(result);
}

typedef struct
{
    DIR           *handle;
    char          *path;
    CsrUtf8String *pattern;
} csrDir;

static CsrDirMode convertAttribToDirMode(unsigned short st_mode)
{
    CsrDirMode mode;

    mode = S_ISDIR(st_mode) ? CSR_DIR_MODE_DIRECTORY : CSR_DIR_MODE_REGULAR_FILE;
    mode |= st_mode & S_IRUSR ? CSR_DIR_MODE_USER_READ_PERMISSION : 0;
    mode |= st_mode & S_IWUSR ? CSR_DIR_MODE_USER_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXUSR ? CSR_DIR_MODE_USER_EXECUTE : 0;
    mode |= st_mode & S_IRGRP ? CSR_DIR_MODE_GROUP_READ_PERMISSION : 0;
    mode |= st_mode & S_IWGRP ? CSR_DIR_MODE_GROUP_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXGRP ? CSR_DIR_MODE_GROUP_EXECUTE : 0;
    mode |= st_mode & S_IROTH ? CSR_DIR_MODE_OTHERS_READ_PERMISSION : 0;
    mode |= st_mode & S_IWOTH ? CSR_DIR_MODE_OTHERS_WRITE_PERMISSION : 0;
    mode |= st_mode & S_IXOTH ? CSR_DIR_MODE_OTHERS_EXECUTE : 0;

    return mode;
}

static CsrBool csrNextMatch(csrDir *dir, CsrDirFindStat *dirStat)
{
    struct dirent *dirent;

    while ((dirent = readdir(dir->handle)) != NULL)
    {
        if (fnmatch((char *) dir->pattern, dirent->d_name, FNM_NOESCAPE) == 0)
        {
            /* Match! */
            char *fileName;
            int result;
            struct stat temp;
            CsrSize dirPathLength;
            CsrSize length;

            dirPathLength = CsrStrLen(dir->path);
            /* [QTI] Fix KW issue#835867 through allocating 3 more bytes for "fileName",
               so as to avoid buffer overflow. */
            length = dirPathLength + CsrStrLen(dirent->d_name) + 4;
            fileName = CsrPmemAlloc(length);
            CsrStrLCpy(fileName, dir->path, length);
            CsrStrLCat(fileName, dirent->d_name, length);
            result = stat(fileName, &temp);
            CsrPmemFree(fileName);

            if (result != 0)
            {
                continue;
            }
            else
            {
                /* Fill out time in UTC */
#ifdef ANDROID
                time64_t realTime;

                realTime = temp.st_mtime;
                convertToDirTm(&dirStat->time, &realTime);
#else
                convertToDirTm(&dirStat->time, &temp.st_mtime);
#endif
                dirStat->name = (CsrUtf8String *) CsrStrDup((CsrCharString *) dirent->d_name);

                /* Convert mode */
                dirStat->mode = convertAttribToDirMode(temp.st_mode);

                dirStat->size = temp.st_size;

                break;
            }
        }
    }

    if (dirent == NULL)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

CsrDirHandle *CsrDirFindFirst(const CsrUtf8String *searchPattern, CsrDirFindStat *dirStat)
{
    csrDir *dir;
    char *tmp, *sep, *pattern, *dirname;

    tmp = CsrStrDup((char *) searchPattern);

    sep = strrchr(tmp, '/');
    if (sep != NULL)
    {
        *sep = '\0';
        dirname = tmp;
        pattern = CsrStrDup(sep + 1);
    }
    else
    {
        dirname = ".";
        pattern = CsrStrDup(tmp);
    }

    dir = CsrPmemAlloc(sizeof(*dir));

    dir->pattern = (CsrUtf8String *) pattern;

    /* Things are easy if we're working on an absolute path. */
    if (dirname[0] == '/')
    {
        CsrSize sz;

        /* [QTI] Fix KW issue#835868 through allocating 3 more bytes for "dir->path",
           so as to avoid buffer overflow. */
        sz = CsrStrLen(dirname) + sizeof('/') + 4;

        dir->path = CsrPmemAlloc(sz);
        CsrStrLCpy(dir->path, dirname, sz);
        CsrStrLCat(dir->path, "/", sz);
    }
    else
    {
        /* Ugh, we have to reconstruct the path */
        dir->path = CsrPmemAlloc(MAXPATHLEN);

        if (getcwd(dir->path, MAXPATHLEN) == NULL)
        {
            CsrPmemFree(dir->path);
            CsrPmemFree(dir->pattern);
            CsrPmemFree(dir);

            return NULL;
        }
        else
        {
            /* Append "/", input path, and "/" */
            CsrStrLCat(dir->path, "/", MAXPATHLEN);
            CsrStrLCat(dir->path, dirname, MAXPATHLEN);
            CsrStrLCat(dir->path, "/", MAXPATHLEN);
        }
    }

    /*
     * At this point we no longer need tmp -- `dirname'
     * is no longer needed.
     */
    CsrPmemFree(tmp);

    dir->handle = opendir(dir->path);

    if ((dir->handle == NULL) || (csrNextMatch(dir, dirStat) != TRUE))
    {
        if (dir->handle != NULL)
        {
            closedir(dir->handle);
        }
        CsrPmemFree(dir->path);
        CsrPmemFree(dir->pattern);
        CsrPmemFree(dir);
        dir = NULL;
    }

    return dir;
}

#define MAP_DIR_FIND_NEXT(result) (result == TRUE ? CSR_RESULT_SUCCESS : \
                                   CSR_DIR_RESULT_FAILURE)

CsrResult CsrDirFindNext(CsrDirHandle *handle, CsrDirFindStat *dirStat)
{
    CsrBool result;
    csrDir *dir = handle;

    result = csrNextMatch(dir, dirStat);

    return MAP_DIR_FIND_NEXT(result);
}

CsrResult CsrDirFindClose(CsrDirHandle *handle)
{
    csrDir *dir = handle;

    closedir(dir->handle);

    CsrPmemFree(dir->path);
    CsrPmemFree(dir->pattern);
    CsrPmemFree(dir);

    return CSR_RESULT_SUCCESS;
}
