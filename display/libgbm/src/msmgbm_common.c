//**************************************************************************************************
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <linux/msm_ion.h>
#include <linux/ion.h>
#include <gbm_priv.h>
#include <msmgbm.h>
#include <msmgbm_common.h>

bool IsImplDefinedFormat(uint32_t format)
{
    if((format == GBM_FORMAT_YCbCr_420_888) ||
           (format == GBM_FORMAT_IMPLEMENTATION_DEFINED))
        return true;
    else
        return false;
}

uint32_t GetDefaultImplDefinedFormat(uint32_t usage_flags, uint32_t format)
{
    uint32_t pixel_format = format;

    if(usage_flags & GBM_BO_USAGE_UBWC_ALIGNED_QTI){
        pixel_format = GBM_FORMAT_YCbCr_420_SP_VENUS_UBWC;
    }

    return pixel_format;
}

uint32_t GetImplDefinedFormat(uint32_t usage_flags, uint32_t format)
{
    uint32_t pixel_format = format;


    pixel_format = GetDefaultImplDefinedFormat(usage_flags, pixel_format);
    pixel_format = GetCameraImplDefinedFormat(usage_flags, pixel_format);
    pixel_format = GetVideoImplDefinedFormat(usage_flags, pixel_format);

    /*default if no valid format is set by camera/video*/
    if(IsImplDefinedFormat(pixel_format))
        pixel_format = GBM_FORMAT_NV21_ZSL;

    LOG(LOG_DBG,"%s: format 0x%x\n", __func__, pixel_format);

    return pixel_format;
}

static uint32_t GetDefaultIonAllocFlags(uint32_t alloc_flags)
{
    uint32_t ion_flags = 0;

    /*set heap specific flags*/
    if(alloc_flags & GBM_BO_ALLOC_SECURE_HEAP_QTI){
            ion_flags |= ION_FLAG_CP_PIXEL;
    }else if((alloc_flags & GBM_BO_ALLOC_SECURE_DISPLAY_HEAP_QTI) &&
		    !(alloc_flags & GBM_BO_USAGE_CAMERA_WRITE_QTI)){
            /*check for secure display*/
            ion_flags |= ION_FLAG_CP_SEC_DISPLAY;
    }

    /*check if it is secure allocation*/
    if(alloc_flags & GBM_BO_USAGE_PROTECTED_QTI){
        ion_flags |= ION_FLAG_SECURE;
    }

    /*check if uncached buffer is requested*/
    if(!(alloc_flags & GBM_BO_USAGE_UNCACHED_QTI)){
        ion_flags |= ION_FLAG_CACHED;
    }

    return ion_flags;
}

uint32_t GetIonAllocFlags(uint32_t alloc_flags)
{
    uint32_t ion_flags = 0;

    ion_flags |= GetDefaultIonAllocFlags(alloc_flags);
    ion_flags |= GetCameraIonAllocFlags(alloc_flags);
    ion_flags |= GetVideoIonAllocFlags(alloc_flags);

    LOG(LOG_DBG,"%s: ion_flags 0x%x\n", __func__, ion_flags);

    return ion_flags;
}

static uint32_t GetDefaultIonHeapId(uint32_t alloc_flags)
{
    uint32_t ion_heap_id = 0;

    if(alloc_flags & GBM_BO_ALLOC_SECURE_HEAP_QTI){
        ion_heap_id = ION_HEAP(ION_SECURE_HEAP_ID);
    }else if(alloc_flags & GBM_BO_ALLOC_SECURE_DISPLAY_HEAP_QTI){
        ion_heap_id = ION_HEAP(ION_SECURE_DISPLAY_HEAP_ID);
    }else if(alloc_flags & GBM_BO_ALLOC_ADSP_HEAP_QTI){
        ion_heap_id = ION_HEAP(ION_ADSP_HEAP_ID);
    }else if(alloc_flags & GBM_BO_ALLOC_CAMERA_HEAP_QTI){
        ion_heap_id = ION_HEAP(ION_CAMERA_HEAP_ID);
    }else if(alloc_flags & GBM_BO_ALLOC_IOMMU_HEAP_QTI){
	/*IOMMU_HEAP is deprecated, use ION_SYSTEM_HEAP_ID*/
        ion_heap_id = ION_HEAP(ION_SYSTEM_HEAP_ID);
    }else if(alloc_flags & GBM_BO_ALLOC_MM_HEAP_QTI){
        ion_heap_id = ION_HEAP(ION_CP_MM_HEAP_ID);
    }else{
        ion_heap_id = ION_HEAP(ION_SYSTEM_HEAP_ID);
    }

    return ion_heap_id;
}

uint32_t GetIonHeapId(uint32_t alloc_flags)
{
    uint32_t ion_heap_id = 0;

    ion_heap_id |= GetDefaultIonHeapId(alloc_flags);
    ion_heap_id |= GetCameraIonHeapId(alloc_flags);
    ion_heap_id |= GetVideoIonHeapId(alloc_flags);

    LOG(LOG_DBG,"%s: ion_heap_id 0x%x\n", __func__, ion_heap_id);

    return ion_heap_id;
}
