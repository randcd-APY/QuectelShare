/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdlib.h>
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

#include "connx_file.h"


static int returnMappedFlags(ConnxFileOpenFlags mode)
{
    int ret = 0;

    ret |= (mode & CONNX_FILE_OPEN_FLAGS_APPEND) ? O_APPEND : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_READ_ONLY) ? O_RDONLY : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_READ_WRITE) ? O_RDWR : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_WRITE_ONLY) ? O_WRONLY : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_CREATE) ? O_CREAT : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_TRUNCATE) ? O_TRUNC : 0;
    ret |= (mode & CONNX_FILE_OPEN_FLAGS_EXCL) ? O_EXCL : 0;

    return ret;
}

static mode_t returnMappedPerms(ConnxFilePerms mode)
{
    mode_t ret = 0;

    /* note that read and write flags are the same for all user/group/others and execute flags are ignored on windows */
    ret |= ((mode & CONNX_FILE_PERMS_USER_READ) ? S_IRUSR : 0);
    ret |= ((mode & CONNX_FILE_PERMS_USER_WRITE) ? S_IWUSR : 0);
    ret |= ((mode & CONNX_FILE_PERMS_USER_EXECUTE) ? S_IXUSR : 0);
    ret |= ((mode & CONNX_FILE_PERMS_GROUP_READ) ? S_IRGRP : 0);
    ret |= ((mode & CONNX_FILE_PERMS_GROUP_WRITE) ? S_IWGRP : 0);
    ret |= ((mode & CONNX_FILE_PERMS_GROUP_EXECUTE) ? S_IXGRP : 0);
    ret |= ((mode & CONNX_FILE_PERMS_OTHERS_READ) ? S_IROTH : 0);
    ret |= ((mode & CONNX_FILE_PERMS_OTHERS_WRITE) ? S_IWOTH : 0);
    ret |= ((mode & CONNX_FILE_PERMS_OTHERS_EXECUTE) ? S_IXOTH : 0);

    return ret;
}

typedef struct
{
    int handle;
} ConnxFileInfo;

#define MAP_FILE_OPEN(result) (result != -1 ? CONNX_RESULT_SUCCESS : \
                               errno == EACCES ? CONNX_FILE_RESULT_NOT_ALLOWED : \
                               errno == EEXIST ? CONNX_FILE_RESULT_ALREAD_EXISTS : \
                               errno == ENOENT ? CONNX_FILE_RESULT_NOT_EXIST : \
                               CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileOpen(ConnxFileHandle *handle, const char *fileName, ConnxFileOpenFlags flags, ConnxFilePerms perms)
{
    int linFlags = returnMappedFlags(flags);
    mode_t mode = returnMappedPerms(perms);
    ConnxFileInfo *fsalHandle = NULL;
    int fd = -1;

    if (!handle || !fileName)
        return CONNX_RESULT_INVALID_PARAMETER;

    fsalHandle = malloc(sizeof(ConnxFileInfo));

    if (!fsalHandle)
        return CONNX_RESULT_OUT_OF_MEMEORY;

    fd = open((char *) fileName, linFlags, mode);

    if (fd >= 0)
    {
        fsalHandle->handle = fd;
        *handle = fsalHandle;
    }
    else
    {
        free(fsalHandle);
        *handle = NULL;
    }

    return MAP_FILE_OPEN(fd);
}

#define MAP_FILE_CLOSE(result) (result == 0 ? CONNX_RESULT_SUCCESS : \
                                CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileClose(ConnxFileHandle handle)
{
    int result;
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;

    if (!handle)
        return CONNX_RESULT_INVALID_PARAMETER;

    result = close(fsalHandle->handle);

    free(handle);

    return MAP_FILE_CLOSE(result);
}

#define MAP_FILE_WRITE(result, _bytesToWrite) (result == _bytesToWrite ? CONNX_RESULT_SUCCESS : \
                                               errno == EBADF ? CONNX_FILE_RESULT_NOT_ALLOWED : \
                                               errno == ENOSPC ? CONNX_FILE_RESULT_NO_SPACE : \
                                               CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileWrite(const void *buffer, size_t bytesToWrite,
                           ConnxFileHandle handle, size_t *written)
{
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;
    size_t actual_written = 0;

    if (!buffer || !bytesToWrite || !handle)
        return CONNX_RESULT_INVALID_PARAMETER;

    actual_written = write(fsalHandle->handle, buffer, bytesToWrite);

    if (written)
        *written = actual_written;

    return MAP_FILE_WRITE(actual_written, bytesToWrite);
}

#define MAP_FILE_READ(result, _handle, _bytesToRead) ((result > 0) ? \
                                                      (result == _bytesToRead ? CONNX_RESULT_SUCCESS : \
                                                       result == 0 ? CONNX_FILE_RESULT_EOF : CONNX_FILE_RESULT_FAILURE)) : \
                                                       errno == EBADF ? CONNX_FILE_RESULT_NOT_ALLOWED : \
                                                       CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileRead(void *buffer, size_t bytesToRead,
                          ConnxFileHandle handle, size_t *bytesRead)
{
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;
    size_t actual_read = 0;

    if (!buffer || !bytesToRead || !handle)
        return CONNX_RESULT_INVALID_PARAMETER;

    actual_read = read(fsalHandle->handle, buffer, bytesToRead);

    if (bytesRead)
    {
        *bytesRead = actual_read;
    }

    if (actual_read > 0)
    {
        return CONNX_RESULT_SUCCESS;
    }
    else if (actual_read == 0)
    {
        return CONNX_FILE_RESULT_EOF;
    }
    else
    {
        if (errno == EBADF)
        {
            return CONNX_FILE_RESULT_NOT_ALLOWED;
        }
        else
        {
            return CONNX_RESULT_FAIL;
        }
    }
}

#define MAP_FILE_FLUSH(result) (result == 0 ? CONNX_RESULT_SUCCESS : \
                                errno == EBADF ? CONNX_FILE_RESULT_NOT_ALLOWED : \
                                CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileFlush(ConnxFileHandle handle)
{
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;
    int result;

    if (!handle)
        return CONNX_RESULT_INVALID_PARAMETER;

    result = fsync(fsalHandle->handle);

    return MAP_FILE_FLUSH(result);
}

#define MAP_FILE_SEEK(result) (result >= 0 ? CONNX_RESULT_SUCCESS : \
                               errno == EBADF ? CONNX_FILE_RESULT_NOT_ALLOWED : \
                               CONNX_FILE_RESULT_FAILURE)

ConnxResult ConnxFileSeek(ConnxFileHandle handle, int32_t offset, int32_t relativeOffset)
{
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;
    int o, result;

    switch (relativeOffset)
    {
        case CONNX_SEEK_SET:
            o = SEEK_SET;
            break;

        case CONNX_SEEK_CUR:
            o = SEEK_CUR;
            break;

        case CONNX_SEEK_END:
            o = SEEK_END;
            break;

        default:
            o = SEEK_SET;
            break;
    }

    result = lseek(fsalHandle->handle, offset, o);
    return MAP_FILE_SEEK(result);
}

ConnxResult ConnxFileTell(ConnxFileHandle handle, uint32_t *iposition)
{
    off_t rv;
    ConnxFileInfo *fsalHandle = (ConnxFileInfo *)handle;

    if (!handle)
        return CONNX_RESULT_INVALID_PARAMETER;

    rv = lseek(fsalHandle->handle, (off_t) 0, SEEK_CUR);

    if (iposition != NULL)
        *iposition = (uint32_t) rv;

    return CONNX_RESULT_SUCCESS;
}

ConnxResult ConnxFileGetSize(const char *fileName, uint32_t *size)
{
    FILE *fd;

    if (!fileName || !size)
        return CONNX_RESULT_INVALID_PARAMETER;

    fd = fopen (fileName, "r");

    if (!fd)
        return CONNX_FILE_RESULT_NOT_EXIST;

    fseek (fd, 0, SEEK_END);

    *size = ftell(fd);

    fclose(fd);

    return CONNX_RESULT_SUCCESS;
}