#ifndef CSR_FILE_H__
#define CSR_FILE_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_SEEK_SET 0
#define CSR_SEEK_CUR 1
#define CSR_SEEK_END 2

typedef void CsrFileHandle;
#define CSR_FILE_INVALID_HANDLE (NULL)

/* All file operations use these error codes        */
#define CSR_FILE_RESULT_FAILURE          ((CsrResult) 1)
#define CSR_FILE_RESULT_EOF              ((CsrResult) 2)
#define CSR_FILE_RESULT_READ_ONLY        ((CsrResult) 3)
#define CSR_FILE_RESULT_NOT_EXIST        ((CsrResult) 4)
#define CSR_FILE_RESULT_NOT_ALLOWED      ((CsrResult) 5)
#define CSR_FILE_RESULT_ALREAD_EXISTS    ((CsrResult) 6)
#define CSR_FILE_RESULT_NO_SPACE         ((CsrResult) 7)

/* File open flags */
typedef CsrUint16 CsrFileOpenFlags;
#define CSR_FILE_OPEN_FLAGS_CREATE        ((CsrFileOpenFlags) 0x0001)  /* Set this flag if file should be created */
#define CSR_FILE_OPEN_FLAGS_READ_ONLY     ((CsrFileOpenFlags) 0x0002)  /* Set this flag if file should be opened as read only (if set this flag takes precedence over all other flags) */
#define CSR_FILE_OPEN_FLAGS_WRITE_ONLY    ((CsrFileOpenFlags) 0x0004)  /* Set this flag if file should be opened as write only */
#define CSR_FILE_OPEN_FLAGS_READ_WRITE    ((CsrFileOpenFlags) 0x0008)  /* Set this flag if file should be opened for both read and write */
#define CSR_FILE_OPEN_FLAGS_APPEND        ((CsrFileOpenFlags) 0x0010)  /* Set this flag if file should be opened for append (if set the file pointer will be repositioned to the end of the file before every write operation) */
#define CSR_FILE_OPEN_FLAGS_TRUNCATE      ((CsrFileOpenFlags) 0x0020)  /* Set this flag if file should be truncated when opened */
#define CSR_FILE_OPEN_FLAGS_EXCL          ((CsrFileOpenFlags) 0x0040)  /* Set this flag if file should not be created when it already exists */

/* File open permissions (only used if CSR_FILE_OPEN_FLAGS_CREATE is set) */
typedef CsrUint16 CsrFilePerms;
#define CSR_FILE_PERMS_USER_READ          ((CsrFilePerms) 0x0001)  /* Set this flag if reads are permitted for user */
#define CSR_FILE_PERMS_USER_WRITE         ((CsrFilePerms) 0x0002)  /* Set this flag if writes are permitted for user */
#define CSR_FILE_PERMS_USER_EXECUTE       ((CsrFilePerms) 0x0004)  /* Set this flag if execute are permitted for user */
#define CSR_FILE_PERMS_GROUP_READ         ((CsrFilePerms) 0x0008)  /* Set this flag if reads are permitted for group */
#define CSR_FILE_PERMS_GROUP_WRITE        ((CsrFilePerms) 0x0010)  /* Set this flag if writes are permitted for group */
#define CSR_FILE_PERMS_GROUP_EXECUTE      ((CsrFilePerms) 0x0020)  /* Set this flag if execute are permitted for group */
#define CSR_FILE_PERMS_OTHERS_READ        ((CsrFilePerms) 0x0040)  /* Set this flag if reads are permitted for others */
#define CSR_FILE_PERMS_OTHERS_WRITE       ((CsrFilePerms) 0x0080)  /* Set this flag if writes are permitted for others */
#define CSR_FILE_PERMS_OTHERS_EXECUTE     ((CsrFilePerms) 0x0100)  /* Set this flag if execute are permitted for others */

CsrResult CsrFileOpen(CsrFileHandle **handle, const CsrUtf8String *fileName, CsrFileOpenFlags flags, CsrFilePerms perms);

CsrResult CsrFileClose(CsrFileHandle *handle);

CsrResult CsrFileWrite(const void *buffer, CsrSize bytesToWrite, CsrFileHandle *handle, CsrSize *written);

CsrResult CsrFileRead(void *buffer, CsrSize bytesToRead, CsrFileHandle *handle, CsrSize *bytesRead);

CsrResult CsrFileSeek(CsrFileHandle *handle, CsrInt32 offset, CsrInt32 relativeOffset);

CsrResult CsrFileFlush(CsrFileHandle *handle);

CsrResult CsrFileRemove(const CsrUtf8String *filename);

CsrResult CsrFileTell(CsrFileHandle *handle, CsrUint32 *position);

CsrResult CsrFileSetEndOfFile(CsrFileHandle *handle);

CsrResult CsrFileSetPerms(const CsrUtf8String *name, CsrFilePerms perms);  /* May also be used on directories */

CsrResult CsrFileRename(const CsrUtf8String *oldName, const CsrUtf8String *newName); /* NB: May also be used on directories */

#ifdef __cplusplus
}
#endif

#endif
