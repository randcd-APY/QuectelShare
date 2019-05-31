#ifndef CSR_BT_FILE_H__
#define CSR_BT_FILE_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/* File API wrappers */
typedef struct
{
    CsrUint32 st_size;
} CsrBtPosixStatBuffer;

extern CsrFileHandle *CsrBtFopen(const CsrCharString *filename, const char *mode);
extern CsrFileHandle *CsrBtFreopen(const CsrCharString *filename, const char *mode, CsrFileHandle *handle);
extern CsrInt32 CsrBtFclose(CsrFileHandle *handle);
extern CsrSize CsrBtFwrite(const CsrFileHandle *buffer, CsrSize size, CsrSize count, CsrFileHandle *handle);
extern CsrSize CsrBtFread(CsrFileHandle *buffer, CsrSize size, CsrSize count, CsrFileHandle *handle);
extern char *CsrBtFgets(char *buffer, CsrUint32 count, CsrFileHandle *handle);
extern CsrInt32 CsrBtFseek(CsrFileHandle *handle, CsrInt32 offset, CsrInt32 relativeOffset);
extern CsrInt32 CsrBtFflush(CsrFileHandle *handle);
extern CsrBool CsrBtSetEndOfFile(CsrFileHandle *handle);
extern CsrInt32 CsrBtDiskstat(CsrCharString *filename, CsrBtPosixStatBuffer *statData);
extern CsrInt32 CsrBtRemove(const char *filename);
extern CsrInt32 CsrBtFtell(CsrFileHandle *handle);
extern CsrUint32 CsrBtFileSize(CsrCharString *filename);
extern CsrBool CsrBtIsAtEof(CsrFileHandle *handle, CsrUint32 size);
extern CsrResult CsrBtCopy(const CsrCharString *srcName,const CsrCharString *destName);


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_FILE_H__ */

