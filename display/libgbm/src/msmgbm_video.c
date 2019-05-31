//**************************************************************************************************
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#include <stdint.h>
#include <gbm_priv.h>
#include <msmgbm.h>
#include <msmgbm_common.h>

/**
 * Get the implementaion defined format based on usage flags.
 * @return - pixel_format to be used for BO
 *           returns input format if none of the combination matches.
 */
uint32_t GetVideoImplDefinedFormat(uint32_t usage_flags, uint32_t format)
{
    uint32_t pixel_format = format;

    if(!IsImplDefinedFormat(format))
        return pixel_format;

    if(usage_flags & GBM_BO_USAGE_VIDEO_ENCODER_QTI){
        pixel_format = GBM_FORMAT_NV12_ENCODEABLE;
    }

    return pixel_format;
}

/**
 * Get the ion allocation flags based on allocation flags.
 * @return - ion flags for BO allocation
 */
uint32_t GetVideoIonAllocFlags(uint32_t alloc_flags)
{
    uint32_t ion_flags = 0;

    return ion_flags;
}

/**
 * Get the ion heap id based on allocation flags.
 * @return - ion heap id for BO allocation
 */
uint32_t GetVideoIonHeapId(uint32_t alloc_flags)
{
    uint32_t ion_heap_id = 0;

    return ion_heap_id;
}

