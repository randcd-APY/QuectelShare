/*
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "gst_crypto.h"

#define SECURE_COPY 1

#ifdef SECURE_COPY

#include <dlfcn.h>

#define SymOEMCryptoLib "libcontentcopy.so"
#define SymOEMCryptoInit "Content_Protection_Copy_Init"
#define SymOEMCryptoTerminate "Content_Protection_Copy_Terminate"
#define SymOEMCryptoCopy "Content_Protection_Copy"

gst_Content_Protection_Copy_Init m_fOEMCryptoInit;
gst_Content_Protection_Copy_Terminate m_fOEMCryptoTerminate;
gst_Content_Protection_Copy m_fOEMCryptoCopy;

int loadCryptoLib(){
    int result = 0;
    m_pLibHandle = dlopen(SymOEMCryptoLib, RTLD_NOW);
    if (m_pLibHandle == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        result = -1;
        return result;
    }
    m_fOEMCryptoInit = (gst_Content_Protection_Copy_Init *)dlsym(m_pLibHandle, SymOEMCryptoInit);
    if (m_fOEMCryptoInit == NULL) {
        result = -1;
    }

    if (result == -1) {
        m_fOEMCryptoTerminate = (gst_Content_Protection_Copy_Terminate)dlsym(m_pLibHandle, SymOEMCryptoTerminate);
        if (m_fOEMCryptoTerminate == NULL) {
            result = -1;
        }
    }

    if (result == 0) {
        m_fOEMCryptoCopy = (gst_Content_Protection_Copy)dlsym(m_pLibHandle, SymOEMCryptoCopy);
        if (m_fOEMCryptoCopy == NULL) {
            result = -1;
        }
    }

    if (result != 0) {
        unloadCryptoLib();
    }
    return result;
}

void unloadCryptoLib(){
    if (m_pLibHandle) {
        dlclose(m_pLibHandle);
        m_pLibHandle = NULL;
        m_pWVHandle = NULL;
    }
    m_fOEMCryptoInit = NULL;
    m_fOEMCryptoTerminate = NULL;
    m_fOEMCryptoCopy = NULL;
}

int copySecureBuf(struct QSEECOM_handle *qhandle, GstClientCopyDir sToUnsecure,unsigned char *nonSecureBuffer, unsigned long SecureBufFd, unsigned int nonSecureBufLen){
    int result =0;
    SampleClientResult result1 = SAMPLE_CLIENT_SUCCESS;
    uint32 nBytesCopied = 0;
    if (m_fOEMCryptoCopy == NULL) {
        return -1;
    }
    result = m_fOEMCryptoCopy(qhandle, nonSecureBuffer, nonSecureBufLen,
        SecureBufFd, 0, &nBytesCopied, 0);


    if ((result != SAMPLE_CLIENT_SUCCESS) || (nBytesCopied != nonSecureBufLen)) {
        return -1;
    }
    return result;
}

struct QSEECOM_handle* initCryptoLib(){
    int result=0;
    m_pLibHandle=NULL;
    m_pWVHandle=NULL;
    result = loadCryptoLib();
    if (result == 0) {
        if (m_fOEMCryptoInit) {
            result = m_fOEMCryptoInit(&m_pWVHandle);
        } else {
            result = -1;
        }
    }
    return m_pWVHandle;
}
#endif
