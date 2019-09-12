#ifndef MCM_UTILS_H
#define MCM_UTILS_H

/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "comdef.h"
#include "qmi_client.h"
#include "log_util.h"

void* mcm_util_memory_alloc
(
    size_t size
);

void mcm_util_memory_free
(
    void** to_be_freed_memory_ptr
);

uint32_t convert_qmi_err_to_mcm
(
    qmi_client_error_type client_err
);


#endif

