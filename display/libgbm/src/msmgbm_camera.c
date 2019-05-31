//**************************************************************************************************
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#include <stdint.h>
#include <stddef.h>
#include <linux/msm_ion.h>
#include <linux/ion.h>
#include <gbm_priv.h>
#include <msmgbm.h>
#include <msmgbm_common.h>

/**
 * Get the implementaion defined format based on usage flags.
 * @return - pixel_format to be used for BO
 *           returns input format if none of the combination matches.
 */
uint32_t GetCameraImplDefinedFormat(uint32_t usage_flags, uint32_t format)
{
    uint32_t pixel_format = format;

    if((usage_flags & GBM_BO_USAGE_CAMERA_READ_QTI) &&
           (usage_flags & GBM_BO_USAGE_CAMERA_WRITE_QTI)){
        pixel_format = GBM_FORMAT_NV21_ZSL;
    }else if(usage_flags & GBM_BO_USAGE_CAMERA_READ_QTI){
        pixel_format = GBM_FORMAT_YCrCb_420_SP;
    }else if(usage_flags & GBM_BO_USAGE_CAMERA_WRITE_QTI){
        if(format == GBM_FORMAT_YCbCr_420_888){
            pixel_format = GBM_FORMAT_NV21_ZSL;
        }else{
            pixel_format = GBM_FORMAT_YCbCr_420_SP_VENUS;
        }
    }else if(usage_flags & GBM_BO_USAGE_HW_COMPOSER_QTI){
        pixel_format = GBM_FORMAT_RGBA8888;
    }

    return pixel_format;
}

/**
 * Get the ion allocation flags based on allocation flags.
 * @return - ion flags for BO allocation
 */
uint32_t GetCameraIonAllocFlags(uint32_t alloc_flags)
{
    uint32_t ion_flags = 0;

    if((alloc_flags & GBM_BO_USAGE_PROTECTED_QTI)
                    && (alloc_flags & GBM_BO_USAGE_CAMERA_WRITE_QTI)){
        if(alloc_flags & GBM_BO_USAGE_HW_COMPOSER_QTI)
            ion_flags |= ION_FLAG_CP_CAMERA_PREVIEW;
        else
            ion_flags |= ION_FLAG_CP_CAMERA;
    }

    return ion_flags;
}

/**
 * Get the ion heap id based on allocation flags.
 * @return - ion heap id for BO allocation
 */
uint32_t GetCameraIonHeapId(uint32_t alloc_flags)
{
    uint32_t ion_heap_id = 0;

    if((alloc_flags & GBM_BO_USAGE_PROTECTED_QTI) &&
                alloc_flags & GBM_BO_USAGE_CAMERA_WRITE_QTI){
            ion_heap_id = ION_HEAP(ION_SECURE_DISPLAY_HEAP_ID);
    }

    return ion_heap_id;
}

