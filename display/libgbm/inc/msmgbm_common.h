//**************************************************************************************************
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#ifndef MSMGBM_COMMON_H_
#define MSMGBM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

bool IsImplDefinedFormat(uint32_t format);
uint32_t GetImplDefinedFormat(uint32_t usage_flags, uint32_t format);
uint32_t GetIonAllocFlags(uint32_t alloc_flags);
uint32_t GetIonHeapId(uint32_t alloc_flags);

/*camera specific functions*/
uint32_t GetCameraImplDefinedFormat(uint32_t usage_flags, uint32_t format);
uint32_t GetCameraIonAllocFlags(uint32_t alloc_flags);
uint32_t GetCameraIonHeapId(uint32_t alloc_flags);

/*video specific functions*/
uint32_t GetVideoImplDefinedFormat(uint32_t usage_flags, uint32_t format);
uint32_t GetVideoIonAllocFlags(uint32_t alloc_flags);
uint32_t GetVideoIonHeapId(uint32_t alloc_flags);

#ifdef __cplusplus
}
#endif

#endif //MSMGBM_COMMON_H_
