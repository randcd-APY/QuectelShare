/*
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __GST_CRYPTO_H_
#define __GST_CRYPTO_H_

#include "content_protection_copy.h"
#include "QSEEComAPI.h"


typedef enum GstClientCopyDir {
    GstCLIENT_COPY_NONSECURE_TO_SECURE = 0,
    GstCLIENT_COPY_SECURE_TO_NONSECURE,
    GstCLIENT_COPY_INVALID_DIR
}GstClientCopyDir; //Taken from content_protection_copy.h



typedef SampleClientResult (*gst_Content_Protection_Copy_Init)(struct QSEECom_handle **);
typedef SampleClientResult (*gst_Content_Protection_Copy_Terminate)(struct QSEECom_handle **);
typedef SampleClientResult (*gst_Content_Protection_Copy)(struct QSEECom_handle *,
    /*GST_U8*/ uint8 *, const uint32, uint32, uint32, uint32 *, GstClientCopyDir);

struct QSEECOM_handle* initCryptoLib();
void unloadCryptoLib();
int loadCryptoLib();
int copySecureBuf(struct QSEECOM_handle *qhandle, GstClientCopyDir sToUnsecure,unsigned char *nonSecureBuffer, unsigned long nonSecureBufFd, unsigned int nonSecureBufLen);

void *m_pLibHandle;

struct QSEECom_handle *m_pWVHandle;
gst_Content_Protection_Copy_Init m_fOEMCryptoInit;
gst_Content_Protection_Copy_Terminate m_fOEMCryptoTerminate;
gst_Content_Protection_Copy m_fOEMCryptoCopy;

#endif /* __GST_CRYPTO_H_ */
