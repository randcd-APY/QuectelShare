#ifndef CSR_FSAL_PRIM_H__
#define CSR_FSAL_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrFsalPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrFsalPrim;

typedef CsrUint32 CsrFsalSession;
typedef void *CsrFsalHandle;

#define CSR_FSAL_MAX_DATA_SIZE      0x10000       /* Maximum allowed number of bytes to read/write in a single operation */

#define CSR_FSAL_INVALID_HANDLE     NULL

/* File open flags (NB: only raw file access mode is supported) */
typedef CsrUint16 CsrFsalFileOpenFlags;
#define CSR_FSAL_OPEN_FLAGS_CREATE                  ((CsrFsalFileOpenFlags) 0x0001)  /* Set this flag if file should be created */
#define CSR_FSAL_OPEN_FLAGS_READ_ONLY               ((CsrFsalFileOpenFlags) 0x0002)  /* Set this flag if file should be opened as read only (if set this flag takes precedence over all other flags) */
#define CSR_FSAL_OPEN_FLAGS_WRITE_ONLY              ((CsrFsalFileOpenFlags) 0x0004)  /* Set this flag if file should be opened as write only */
#define CSR_FSAL_OPEN_FLAGS_READ_WRITE              ((CsrFsalFileOpenFlags) 0x0008)  /* Set this flag if file should be opened for both read and write */
#define CSR_FSAL_OPEN_FLAGS_APPEND                  ((CsrFsalFileOpenFlags) 0x0010)  /* Set this flag if file should be opened for append (if set the file pointer will be repositioned to the end of the file before every write operation) */
#define CSR_FSAL_OPEN_FLAGS_TRUNCATE                ((CsrFsalFileOpenFlags) 0x0020)  /* Set this flag if file should be truncated when opened */
#define CSR_FSAL_OPEN_FLAGS_EXCL                    ((CsrFsalFileOpenFlags) 0x0040)  /* Set this flag if file should not be created when it already exists */

/* File open permissions (only used if CSR_FSAL_OPEN_FLAGS_CREATE is set) */
typedef CsrUint16 CsrFsalFileOpenPerms;
#define CSR_FSAL_OPEN_PERMS_NOT_APPLICABLE          ((CsrFsalFileOpenPerms) 0x0000)  /* This flag should be used when the open perms is not applicable ie. when the CSR_FSAL_OPEN_FLAGS_CREATE flag is not used */
#define CSR_FSAL_OPEN_PERMS_READ                    ((CsrFsalFileOpenPerms) 0x0001)  /* Set this flag if reads are permitted */
#define CSR_FSAL_OPEN_PERMS_WRITE                   ((CsrFsalFileOpenPerms) 0x0002)  /* Set this flag if writes are permitted */

/*  File/directory mode properties */
typedef CsrUint16 CsrFsalMode;
#define CSR_FSAL_MODE_DIRECTORY                     ((CsrFsalMode) 0x0001)
#define CSR_FSAL_MODE_REGULAR_FILE                  ((CsrFsalMode) 0x0002)
#define CSR_FSAL_MODE_USER_READ_PERMISSION          ((CsrFsalMode) 0x0004)
#define CSR_FSAL_MODE_USER_WRITE_PERMISSION         ((CsrFsalMode) 0x0008)
#define CSR_FSAL_MODE_USER_EXECUTE                  ((CsrFsalMode) 0x0010)
#define CSR_FSAL_MODE_GROUP_READ_PERMISSION         ((CsrFsalMode) 0x0020)
#define CSR_FSAL_MODE_GROUP_WRITE_PERMISSION        ((CsrFsalMode) 0x0040)
#define CSR_FSAL_MODE_GROUP_EXECUTE                 ((CsrFsalMode) 0x0080)
#define CSR_FSAL_MODE_OTHERS_READ_PERMISSION        ((CsrFsalMode) 0x0100)
#define CSR_FSAL_MODE_OTHERS_WRITE_PERMISSION       ((CsrFsalMode) 0x0200)
#define CSR_FSAL_MODE_OTHERS_EXECUTE                ((CsrFsalMode) 0x0400)

typedef CsrUint16 CsrFsalPermissions;
#define CSR_FSAL_PERMISSION_USER_READ               ((CsrFsalPermissions) 0x0001)  /* Set this flag if reads are permitted for user */
#define CSR_FSAL_PERMISSION_USER_WRITE              ((CsrFsalPermissions) 0x0002)  /* Set this flag if writes are permitted for user */
#define CSR_FSAL_PERMISSION_USER_EXECUTE            ((CsrFsalPermissions) 0x0004)  /* Set this flag if execute are permitted for user */
#define CSR_FSAL_PERMISSION_GROUP_READ              ((CsrFsalPermissions) 0x0008)  /* Set this flag if reads are permitted for group */
#define CSR_FSAL_PERMISSION_GROUP_WRITE             ((CsrFsalPermissions) 0x0010)  /* Set this flag if writes are permitted for group */
#define CSR_FSAL_PERMISSION_GROUP_EXECUTE           ((CsrFsalPermissions) 0x0020)  /* Set this flag if execute are permitted for group */
#define CSR_FSAL_PERMISSION_OTHERS_READ             ((CsrFsalPermissions) 0x0040)  /* Set this flag if reads are permitted for others */
#define CSR_FSAL_PERMISSION_OTHERS_WRITE            ((CsrFsalPermissions) 0x0080)  /* Set this flag if writes are permitted for others */
#define CSR_FSAL_PERMISSION_OTHERS_EXECUTE          ((CsrFsalPermissions) 0x0100)  /* Set this flag if execute are permitted for others */

/*  File seek origins */
typedef CsrUint8 CsrFsalOrigin;
#define CSR_FSAL_SEEK_SET                           ((CsrFsalOrigin) 0) /* Beginning of file        */
#define CSR_FSAL_SEEK_CUR                           ((CsrFsalOrigin) 1) /* Current position in the file */
#define CSR_FSAL_SEEK_END                           ((CsrFsalOrigin) 2) /* End of file              */

/* All file operations (open/close/read/write/seek/tell) use these error codes, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_FILE_OP_FAILURE                    ((CsrResult) 1)
#define CSR_FSAL_FILE_OP_EOF                        ((CsrResult) 2)
#define CSR_FSAL_FILE_OP_READ_ONLY                  ((CsrResult) 3)
#define CSR_FSAL_FILE_OP_NOT_EXIST                  ((CsrResult) 4)
#define CSR_FSAL_FILE_OP_NOT_ALLOWED                ((CsrResult) 5)
#define CSR_FSAL_FILE_OP_ALREAD_EXISTS              ((CsrResult) 6)
#define CSR_FSAL_FILE_OP_NO_SPACE                   ((CsrResult) 7)

/* All Find operations use these error codes, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_DIR_OP_FAILURE                     ((CsrResult) 1)
#define CSR_FSAL_DIR_OP_NO_MORE_MATCHING_ENTRIES    ((CsrResult) 2)

/* Error codes define for the remove procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_DELETE_FAILURE                     ((CsrResult) 1)
#define CSR_FSAL_DELETE_READ_ONLY                   ((CsrResult) 2)
#define CSR_FSAL_DELETE_NOT_EXIST                   ((CsrResult) 3)
#define CSR_FSAL_DELETE_NOT_EMPTY                   ((CsrResult) 4)

/* Error codes define for the DIR make procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_DIR_MAKE_FAILURE                   ((CsrResult) 1)
#define CSR_FSAL_DIR_MAKE_EXIST                     ((CsrResult) 2)
#define CSR_FSAL_DIR_MAKE_INVALID_PATH              ((CsrResult) 3)

/* Error codes define for the DIR change procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_DIR_CHANGE_FAILURE                 ((CsrResult) 1)
#define CSR_FSAL_DIR_CHANGE_NOT_EXIST               ((CsrResult) 2)

/* Error codes define for the set permissions procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_SET_PERMISSIONS_FAILURE            ((CsrResult) 1)
#define CSR_FSAL_SET_PERMISSIONS_NOT_EXIST          ((CsrResult) 2)

/* Error codes define for the rename procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_RENAME_FAILURE                     ((CsrResult) 1)
#define CSR_FSAL_RENAME_NOT_EXIST                   ((CsrResult) 2)
#define CSR_FSAL_RENAME_NOT_ALLOWED                 ((CsrResult) 3)

/* Error codes define for the stat procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_FSAL_STAT_FAILURE                       ((CsrResult) 1)
#define CSR_FSAL_STAT_NOT_EXIST                     ((CsrResult) 2)

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
} CsrFsalTm;

typedef struct CsrFsalDirEntry_tag
{
    CsrUtf8String *name;                            /* Name of entry                                                */
    CsrFsalTm      time;                            /* Time of last write/modification of entry                     */
    CsrSize        size;                            /* 0 if not file                                                */
    CsrFsalMode    mode;                            /* mode                                                         */
} CsrFsalDirEntry;


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_FSAL_PRIM_DOWNSTREAM_LOWEST                       (0x0000)

#define CSR_FSAL_SESSION_CREATE_REQ             ((CsrFsalPrim) 0x0000 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_SESSION_DESTROY_REQ            ((CsrFsalPrim) 0x0001 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_OPEN_REQ                  ((CsrFsalPrim) 0x0002 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_CLOSE_REQ                 ((CsrFsalPrim) 0x0003 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_SEEK_REQ                  ((CsrFsalPrim) 0x0004 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_READ_REQ                  ((CsrFsalPrim) 0x0005 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_WRITE_REQ                 ((CsrFsalPrim) 0x0006 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_FILE_TELL_REQ                  ((CsrFsalPrim) 0x0007 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_STAT_REQ                       ((CsrFsalPrim) 0x0008 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_REMOVE_REQ                     ((CsrFsalPrim) 0x0009 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_RENAME_REQ                     ((CsrFsalPrim) 0x000A + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_PERMISSIONS_SET_REQ            ((CsrFsalPrim) 0x000B + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_DIR_MAKE_REQ                   ((CsrFsalPrim) 0x000C + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_DIR_CHANGE_REQ                 ((CsrFsalPrim) 0x000D + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_FIRST_REQ             ((CsrFsalPrim) 0x000E + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_NEXT_REQ              ((CsrFsalPrim) 0x000F + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_CLOSE_REQ             ((CsrFsalPrim) 0x0010 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_REMOVE_RECURSIVELY_REQ         ((CsrFsalPrim) 0x0011 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)

#define CSR_FSAL_PRIM_DOWNSTREAM_HIGHEST                      (0x0011 + CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_FSAL_PRIM_UPSTREAM_LOWEST                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_FSAL_SESSION_CREATE_CFM             ((CsrFsalPrim) 0x0000 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_SESSION_DESTROY_CFM            ((CsrFsalPrim) 0x0001 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_OPEN_CFM                  ((CsrFsalPrim) 0x0002 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_CLOSE_CFM                 ((CsrFsalPrim) 0x0003 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_SEEK_CFM                  ((CsrFsalPrim) 0x0004 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_READ_CFM                  ((CsrFsalPrim) 0x0005 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_WRITE_CFM                 ((CsrFsalPrim) 0x0006 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_FILE_TELL_CFM                  ((CsrFsalPrim) 0x0007 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_STAT_CFM                       ((CsrFsalPrim) 0x0008 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_REMOVE_CFM                     ((CsrFsalPrim) 0x0009 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_RENAME_CFM                     ((CsrFsalPrim) 0x000A + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_PERMISSIONS_SET_CFM            ((CsrFsalPrim) 0x000B + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_DIR_MAKE_CFM                   ((CsrFsalPrim) 0x000C + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_DIR_CHANGE_CFM                 ((CsrFsalPrim) 0x000D + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_FIRST_CFM             ((CsrFsalPrim) 0x000E + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_NEXT_CFM              ((CsrFsalPrim) 0x000F + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_DIR_FIND_CLOSE_CFM             ((CsrFsalPrim) 0x0010 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)
#define CSR_FSAL_REMOVE_RECURSIVELY_CFM         ((CsrFsalPrim) 0x0011 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)

#define CSR_FSAL_PRIM_UPSTREAM_HIGHEST                        (0x0011 + CSR_FSAL_PRIM_UPSTREAM_LOWEST)

#define CSR_FSAL_PRIM_DOWNSTREAM_COUNT     (CSR_FSAL_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_FSAL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_FSAL_PRIM_UPSTREAM_COUNT       (CSR_FSAL_PRIM_UPSTREAM_HIGHEST + 1 - CSR_FSAL_PRIM_UPSTREAM_LOWEST)

/* ---------- End of Primitives ----------*/

typedef struct
{
    CsrFsalPrim type;
    CsrSchedQid appHandle;
} CsrFsalSessionCreateReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *currentWorkingDir;
    CsrResult      result;
} CsrFsalSessionCreateCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
} CsrFsalSessionDestroyReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalSessionDestroyCfm;

typedef struct
{
    CsrFsalPrim          type;
    CsrFsalSession       sessionId;
    CsrFsalFileOpenFlags flags;
    CsrFsalFileOpenPerms permissions;
    CsrUtf8String       *fileName;
} CsrFsalFileOpenReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrFsalHandle  handle;
    CsrResult      result;
    CsrSize        size;
} CsrFsalFileOpenCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
} CsrFsalFileCloseReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrResult     result;
} CsrFsalFileCloseCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrInt32      offset;
    CsrFsalOrigin origin;
} CsrFsalFileSeekReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrResult     result;
} CsrFsalFileSeekCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrUint32     bytesToRead;
} CsrFsalFileReadReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrResult     result;
    CsrUint32     dataLen;
    CsrUint8     *data;
} CsrFsalFileReadCfm;


typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrUint32     dataLen;
    CsrUint8     *data;
} CsrFsalFileWriteReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrResult     result;
    CsrUint32     bytesWritten;
} CsrFsalFileWriteCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
} CsrFsalFileTellReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrUint32     position;
    CsrResult     result;
} CsrFsalFileTellCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *path;
} CsrFsalStatReq;

typedef struct
{
    CsrFsalPrim     type;
    CsrFsalSession  sessionId;
    CsrResult       result;
    CsrFsalDirEntry stat;
} CsrFsalStatCfm;


typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *path;
} CsrFsalRemoveReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalRemoveCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *dir;
} CsrFsalRemoveRecursivelyReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalRemoveRecursivelyCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *oldPath;
    CsrUtf8String *newPath;
} CsrFsalRenameReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalRenameCfm;

typedef struct
{
    CsrFsalPrim        type;
    CsrFsalSession     sessionId;
    CsrUtf8String     *path;
    CsrFsalPermissions permissions;
} CsrFsalPermissionsSetReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalPermissionsSetCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *dirName;
} CsrFsalDirMakeReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrResult      result;
} CsrFsalDirMakeCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *path;
} CsrFsalDirChangeReq;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *currentWorkingDir;
    CsrResult      result;
} CsrFsalDirChangeCfm;

typedef struct
{
    CsrFsalPrim    type;
    CsrFsalSession sessionId;
    CsrUtf8String *searchPattern;
} CsrFsalDirFindFirstReq;

typedef struct
{
    CsrFsalPrim     type;
    CsrFsalSession  sessionId;
    CsrFsalHandle   handle;
    CsrResult       result;
    CsrFsalDirEntry entry;
} CsrFsalDirFindFirstCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
} CsrFsalDirFindNextReq;

typedef struct
{
    CsrFsalPrim     type;
    CsrFsalHandle   handle;
    CsrFsalDirEntry entry;
    CsrResult       result;
} CsrFsalDirFindNextCfm;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
} CsrFsalDirFindCloseReq;

typedef struct
{
    CsrFsalPrim   type;
    CsrFsalHandle handle;
    CsrResult     result;
} CsrFsalDirFindCloseCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_FSAL_PRIM_H__ */
