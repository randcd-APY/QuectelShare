/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_FILE_H_
#define _CONNX_FILE_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONNX_SEEK_SET  0
#define CONNX_SEEK_CUR  1
#define CONNX_SEEK_END  2

typedef void *ConnxFileHandle;
#define CONNX_FILE_INVALID_HANDLE           (NULL)

/* All file operations use these error codes        */
#define CONNX_FILE_RESULT_BASE              (0xF000)

#define CONNX_FILE_RESULT_FAILURE           ((ConnxResult) (CONNX_FILE_RESULT_BASE + 1))
#define CONNX_FILE_RESULT_EOF               ((ConnxResult) (CONNX_FILE_RESULT_BASE + 2))
#define CONNX_FILE_RESULT_READ_ONLY         ((ConnxResult) (CONNX_FILE_RESULT_BASE + 3))
#define CONNX_FILE_RESULT_NOT_EXIST         ((ConnxResult) (CONNX_FILE_RESULT_BASE + 4))
#define CONNX_FILE_RESULT_NOT_ALLOWED       ((ConnxResult) (CONNX_FILE_RESULT_BASE + 5))
#define CONNX_FILE_RESULT_ALREAD_EXISTS     ((ConnxResult) (CONNX_FILE_RESULT_BASE + 6))
#define CONNX_FILE_RESULT_NO_SPACE          ((ConnxResult) (CONNX_FILE_RESULT_BASE + 7))

/* File open flags */
typedef uint16_t ConnxFileOpenFlags;
#define CONNX_FILE_OPEN_FLAGS_CREATE        ((ConnxFileOpenFlags) 0x0001)  /* Set this flag if file should be created */
#define CONNX_FILE_OPEN_FLAGS_READ_ONLY     ((ConnxFileOpenFlags) 0x0002)  /* Set this flag if file should be opened as read only (if set this flag takes precedence over all other flags) */
#define CONNX_FILE_OPEN_FLAGS_WRITE_ONLY    ((ConnxFileOpenFlags) 0x0004)  /* Set this flag if file should be opened as write only */
#define CONNX_FILE_OPEN_FLAGS_READ_WRITE    ((ConnxFileOpenFlags) 0x0008)  /* Set this flag if file should be opened for both read and write */
#define CONNX_FILE_OPEN_FLAGS_APPEND        ((ConnxFileOpenFlags) 0x0010)  /* Set this flag if file should be opened for append (if set the file pointer will be repositioned to the end of the file before every write operation) */
#define CONNX_FILE_OPEN_FLAGS_TRUNCATE      ((ConnxFileOpenFlags) 0x0020)  /* Set this flag if file should be truncated when opened */
#define CONNX_FILE_OPEN_FLAGS_EXCL          ((ConnxFileOpenFlags) 0x0040)  /* Set this flag if file should not be created when it already exists */

/* File open permissions (only used if CONNX_FILE_OPEN_FLAGS_CREATE is set) */
typedef uint16_t ConnxFilePerms;
#define CONNX_FILE_PERMS_USER_READ          ((ConnxFilePerms) 0x0001)  /* Set this flag if reads are permitted for user */
#define CONNX_FILE_PERMS_USER_WRITE         ((ConnxFilePerms) 0x0002)  /* Set this flag if writes are permitted for user */
#define CONNX_FILE_PERMS_USER_EXECUTE       ((ConnxFilePerms) 0x0004)  /* Set this flag if execute are permitted for user */
#define CONNX_FILE_PERMS_GROUP_READ         ((ConnxFilePerms) 0x0008)  /* Set this flag if reads are permitted for group */
#define CONNX_FILE_PERMS_GROUP_WRITE        ((ConnxFilePerms) 0x0010)  /* Set this flag if writes are permitted for group */
#define CONNX_FILE_PERMS_GROUP_EXECUTE      ((ConnxFilePerms) 0x0020)  /* Set this flag if execute are permitted for group */
#define CONNX_FILE_PERMS_OTHERS_READ        ((ConnxFilePerms) 0x0040)  /* Set this flag if reads are permitted for others */
#define CONNX_FILE_PERMS_OTHERS_WRITE       ((ConnxFilePerms) 0x0080)  /* Set this flag if writes are permitted for others */
#define CONNX_FILE_PERMS_OTHERS_EXECUTE     ((ConnxFilePerms) 0x0100)  /* Set this flag if execute are permitted for others */


ConnxResult ConnxFileOpen(ConnxFileHandle *handle, const char *fileName, ConnxFileOpenFlags flags, ConnxFilePerms perms);

ConnxResult ConnxFileClose(ConnxFileHandle handle);

ConnxResult ConnxFileWrite(const void *buffer, size_t bytesToWrite, ConnxFileHandle handle, size_t *written);

ConnxResult ConnxFileRead(void *buffer, size_t bytesToRead, ConnxFileHandle handle, size_t *bytesRead);

ConnxResult ConnxFileFlush(ConnxFileHandle handle);

ConnxResult ConnxFileSeek(ConnxFileHandle handle, int32_t offset, int32_t relativeOffset);

ConnxResult ConnxFileTell(ConnxFileHandle handle, uint32_t *position);

ConnxResult ConnxFileGetSize(const char *fileName, uint32_t *size);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_FILE_H_ */