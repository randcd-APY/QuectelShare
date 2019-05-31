#ifndef CSR_FSAL_LIB_H__
#define CSR_FSAL_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_fsal_prim.h"
#include "csr_msg_transport.h"
#include "csr_fsal_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrFsalFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);
void CsrFsalFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalSessionCreateReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalSessionCreateReq *CsrFsalSessionCreateReq_struct(CsrSchedQid appHandle);

#define CsrFsalSessionCreateReqSend(_appHandle) { \
        CsrFsalSessionCreateReq *msg__; \
        msg__ = CsrFsalSessionCreateReq_struct(_appHandle); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalSessionDestroyReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalSessionDestroyReq *CsrFsalSessionDestroyReq_struct(CsrFsalSession sessionId);

#define CsrFsalSessionDestroyReqSend(_sessionId) { \
        CsrFsalSessionDestroyReq *msg__; \
        msg__ = CsrFsalSessionDestroyReq_struct(_sessionId); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileOpenReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileOpenReq *CsrFsalFileOpenReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *fileName,
    CsrFsalFileOpenFlags flags,
    CsrFsalFileOpenPerms perms);

#define CsrFsalFileOpenReqSend(_sessionId, _fileName, _flags, _perms) { \
        CsrFsalFileOpenReq *msg__; \
        msg__ = CsrFsalFileOpenReq_struct(_sessionId, _fileName, _flags, _perms); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileCloseReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileCloseReq *CsrFsalFileCloseReq_struct(CsrFsalHandle handle);

#define CsrFsalFileCloseReqSend(_handle) { \
        CsrFsalFileCloseReq *msg__; \
        msg__ = CsrFsalFileCloseReq_struct(_handle); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileSeekReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileSeekReq *CsrFsalFileSeekReq_struct(CsrFsalHandle handle,
    CsrInt32 offset,
    CsrFsalOrigin origin);

#define CsrFsalFileSeekReqSend(_handle, _offset, _origin) { \
        CsrFsalFileSeekReq *msg__; \
        msg__ = CsrFsalFileSeekReq_struct(_handle, _offset, _origin); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileReadReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileReadReq *CsrFsalFileReadReq_struct(CsrFsalHandle handle,
    CsrUint32 bytesToRead);

#define CsrFsalFileReadReqSend(_handle, _bytesToRead) { \
        CsrFsalFileReadReq *msg__; \
        msg__ = CsrFsalFileReadReq_struct(_handle, _bytesToRead); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileWriteReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileWriteReq *CsrFsalFileWriteReq_struct(CsrFsalHandle handle,
    CsrUint32 dataLen,
    CsrUint8 *data);

#define CsrFsalFileWriteReqSend(_handle, _dataLength, _data) { \
        CsrFsalFileWriteReq *msg__; \
        msg__ = CsrFsalFileWriteReq_struct(_handle, _dataLength, _data); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileTellReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileTellReq *CsrFsalFileTellReq_struct(CsrFsalHandle handle);

#define CsrFsalFileTellReqSend(_handle) { \
        CsrFsalFileTellReq *msg__; \
        msg__ = CsrFsalFileTellReq_struct(_handle); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalStatReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalStatReq *CsrFsalStatReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *path);

#define CsrFsalStatReqSend(_sessionId, _path) { \
        CsrFsalStatReq *msg__; \
        msg__ = CsrFsalStatReq_struct(_sessionId, _path); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRemoveReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRemoveReq *CsrFsalRemoveReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *path);

#define CsrFsalRemoveReqSend(_sessionId, _path) { \
        CsrFsalRemoveReq *msg__; \
        msg__ = CsrFsalRemoveReq_struct(_sessionId, _path); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRemoveRecursivelyReq
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRemoveRecursivelyReq *CsrFsalRemoveRecursivelyReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *dir);

#define CsrFsalRemoveRecursivelyReqSend(_sessionId, _dir) { \
        CsrFsalRemoveRecursivelyReq *msg__; \
        msg__ = CsrFsalRemoveRecursivelyReq_struct(_sessionId, _dir); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRenameReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRenameReq *CsrFsalRenameReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *oldPath,
    CsrUtf8String *newPath);

#define CsrFsalRenameReqSend(_sessionId, _oldPath, _newPath) { \
        CsrFsalRenameReq *msg__; \
        msg__ = CsrFsalRenameReq_struct(_sessionId, _oldPath, _newPath); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalPermissionsSetReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalPermissionsSetReq *CsrFsalPermissionsSetReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *path,
    CsrFsalPermissions permissions);

#define CsrFsalPermissionsSetReqSend(_sessionId, _path, _permissions) { \
        CsrFsalPermissionsSetReq *msg__; \
        msg__ = CsrFsalPermissionsSetReq_struct(_sessionId, _path, _permissions); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirMakeReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirMakeReq *CsrFsalDirMakeReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *dirName);

#define CsrFsalDirMakeReqSend(_sessionId, _dirName) { \
        CsrFsalDirMakeReq *msg__; \
        msg__ = CsrFsalDirMakeReq_struct(_sessionId, _dirName); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirChangeReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirChangeReq *CsrFsalDirChangeReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *path);

#define CsrFsalDirChangeReqSend(_sessionId, _path) { \
        CsrFsalDirChangeReq *msg__; \
        msg__ = CsrFsalDirChangeReq_struct(_sessionId, _path); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindFirstReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindFirstReq *CsrFsalDirFindFirstReq_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *searchPattern);

#define CsrFsalDirFindFirstReqSend(_sessionId, _searchPattern) { \
        CsrFsalDirFindFirstReq *msg__; \
        msg__ = CsrFsalDirFindFirstReq_struct(_sessionId, _searchPattern); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindNextReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindNextReq *CsrFsalDirFindNextReq_struct(CsrFsalHandle handle);

#define CsrFsalDirFindNextReqSend(_h) { \
        CsrFsalDirFindNextReq *msg__; \
        msg__ = CsrFsalDirFindNextReq_struct(_h); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindCloseReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindCloseReq *CsrFsalDirFindCloseReq_struct(CsrFsalHandle handle);

#define CsrFsalDirFindCloseReqSend(_h) { \
        CsrFsalDirFindCloseReq *msg__; \
        msg__ = CsrFsalDirFindCloseReq_struct(_h); \
        CsrMsgTransport(CSR_FSAL_IFACEQUEUE, CSR_FSAL_PRIM, msg__);}

/* FROM HERE UPSTREAM PRIMS */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalSessionCreateCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalSessionCreateCfm *CsrFsalSessionCreateCfm_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *currentWorkingDir,
    CsrResult result);

#define CsrFsalSessionCreateCfmSend(_appHandle, _sessionId, _currentWorkingDir, _result) { \
        CsrFsalSessionCreateCfm *msg__; \
        msg__ = CsrFsalSessionCreateCfm_struct(_sessionId, _currentWorkingDir, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalSessionDestroyCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalSessionDestroyCfm *CsrFsalSessionDestroyCfm_struct(
    CsrFsalSession sessionId,
    CsrResult result);

#define CsrFsalSessionDestroyCfmSend(_appHandle, _sessionId, _result) { \
        CsrFsalSessionDestroyCfm *msg__; \
        msg__ = CsrFsalSessionDestroyCfm_struct(_sessionId, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileOpenCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileOpenCfm *CsrFsalFileOpenCfmSend_struct(
    CsrFsalSession sessionId,
    CsrFsalHandle handle,
    CsrResult result,
    CsrUint32 size);

#define CsrFsalFileOpenCfmSend(_appHandle, _sessionId, _handle, _result, _size) { \
        CsrFsalFileOpenCfm *msg__; \
        msg__ = CsrFsalFileOpenCfmSend_struct(_sessionId, _handle, _result, _size); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileCloseCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileCloseCfm *CsrFsalFileCloseCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result);

#define CsrFsalFileCloseCfmSend(_appHandle, _handle, _result) { \
        CsrFsalFileCloseCfm *msg__; \
        msg__ = CsrFsalFileCloseCfmSend_struct(_handle, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileSeekCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileSeekCfm *CsrFsalFileSeekCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result);

#define CsrFsalFileSeekCfmSend(_appHandle, _handle, _result) { \
        CsrFsalFileSeekCfm *msg__; \
        msg__ = CsrFsalFileSeekCfmSend_struct(_handle, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileReadCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileReadCfm *CsrFsalFileReadCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result,
    CsrUint32 dataLen,
    CsrUint8 *data);

#define CsrFsalFileReadCfmSend(_appHandle, _handle, _result, _dataLen, _data) { \
        CsrFsalFileReadCfm *msg__; \
        msg__ = CsrFsalFileReadCfmSend_struct(_handle, _result, _dataLen, _data); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileWriteCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileWriteCfm *CsrFsalFileWriteCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result,
    CsrUint32 bytesWritten);

#define CsrFsalFileWriteCfmSend(_appHandle, _handle, _result, _bytesWritten) { \
        CsrFsalFileWriteCfm *msg__; \
        msg__ = CsrFsalFileWriteCfmSend_struct(_handle, _result, _bytesWritten); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalFileTellCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalFileTellCfm *CsrFsalFileTellCfmSend_struct(
    CsrFsalHandle handle,
    CsrUint32 position,
    CsrResult result);

#define CsrFsalFileTellCfmSend(_appHandle, _handle, _position, _result) { \
        CsrFsalFileTellCfm *msg__; \
        msg__ = CsrFsalFileTellCfmSend_struct(_handle, _position, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalStatCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalStatCfm *CsrFsalStatCfmSend_struct(
    CsrFsalSession sessionId,
    CsrResult result,
    CsrFsalDirEntry *stat);

#define CsrFsalStatCfmSend(_appHandle, _sessionId, _result, _stat) { \
        CsrFsalStatCfm *msg__; \
        msg__ = CsrFsalStatCfmSend_struct(_sessionId, _result, _stat); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRemoveCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRemoveCfm *CsrFsalRemoveCfmSend_struct(
    CsrResult result,
    CsrFsalSession sessionId);

#define CsrFsalRemoveCfmSend(_appHandle, _result, _sessionId) { \
        CsrFsalRemoveCfm *msg__; \
        msg__ = CsrFsalRemoveCfmSend_struct(_result, _sessionId); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRemoveRecursivelyCfm
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRemoveRecursivelyCfm *CsrFsalRemoveRecursivelyCfmSend_struct(
    CsrResult result,
    CsrFsalSession sessionId);

#define CsrFsalRemoveRecursivelyCfmSend(_appHandle, _result, _sessionId) { \
        CsrFsalRemoveRecursivelyCfm *msg__; \
        msg__ = CsrFsalRemoveRecursivelyCfmSend_struct(_result, _sessionId); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalRenameCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalRenameCfm *CsrFsalRenameCfmSend_struct(
    CsrFsalSession sessionId,
    CsrResult result);

#define CsrFsalRenameCfmSend(_appHandle, _sessionId, _result) { \
        CsrFsalRenameCfm *msg__; \
        msg__ = CsrFsalRenameCfmSend_struct(_sessionId, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalPermissionsSetCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalPermissionsSetCfm *CsrFsalPermissionsSetCfmSend_struct(
    CsrFsalSession sessionId,
    CsrResult result);

#define CsrFsalPermissionsSetCfmSend(_appHandle, _sessionId, _result) { \
        CsrFsalPermissionsSetCfm *msg__; \
        msg__ = CsrFsalPermissionsSetCfmSend_struct(_sessionId, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirMakeCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirMakeCfm *CsrFsalDirMakeCfmSend_struct(
    CsrFsalSession sessionId,
    CsrResult result);

#define CsrFsalDirMakeCfmSend(_appHandle, _sessionId, _result) { \
        CsrFsalDirMakeCfm *msg__; \
        msg__ = CsrFsalDirMakeCfmSend_struct(_sessionId, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirChangeCfm
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirChangeCfm *CsrFsalDirChangeCfmSend_struct(
    CsrFsalSession sessionId,
    CsrUtf8String *currentWorkingDir,
    CsrResult result);

#define CsrFsalDirChangeCfmSend(_appHandle, _sessionId, _currentWorkingDir, _result) { \
        CsrFsalDirChangeCfm *msg__; \
        msg__ = CsrFsalDirChangeCfmSend_struct(_sessionId, _currentWorkingDir, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindFirstCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindFirstCfm *CsrFsalDirFindFirstCfmSend_struct(
    CsrFsalSession sessionId,
    CsrFsalHandle handle,
    CsrUint8 result,
    CsrFsalDirEntry *entry);

#define CsrFsalDirFindFirstCfmSend(_appHandle, _sessionId, _handle, _result, _entry) { \
        CsrFsalDirFindFirstCfm *msg__; \
        msg__ = CsrFsalDirFindFirstCfmSend_struct(_sessionId, _handle, _result, _entry); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindNextCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindNextCfm *CsrFsalDirFindNextCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result,
    CsrFsalDirEntry *entry);

#define CsrFsalDirFindNextCfmSend(_appHandle, _handle, _result, _entry) { \
        CsrFsalDirFindNextCfm *msg__; \
        msg__ = CsrFsalDirFindNextCfmSend_struct(_handle, _result, _entry); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFsalDirFindCloseCfmSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrFsalDirFindCloseCfm *CsrFsalDirFindCloseCfmSend_struct(
    CsrFsalHandle handle,
    CsrResult result);
#define CsrFsalDirFindCloseCfmSend(_appHandle, _handle, _result) { \
        CsrFsalDirFindCloseCfm *msg__; \
        msg__ = CsrFsalDirFindCloseCfmSend_struct(_handle, _result); \
        CsrMsgTransport(_appHandle, CSR_FSAL_PRIM, msg__);}


#ifdef __cplusplus
}
#endif

#endif
