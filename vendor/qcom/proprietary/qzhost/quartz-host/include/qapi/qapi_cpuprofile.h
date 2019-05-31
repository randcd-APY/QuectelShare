/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_CPUPROFILE_H_
#define _QAPI_CPUPROFILE_H_

#include <stdint.h>

#if defined(ENABLE_PER_FN_PROFILING)

int qapi_Cpu_Profile_Per_Fn_Profiling_Enable(uint32_t addr, uint16_t port);
int qapi_Cpu_Profile_Per_Fn_Profiling_Start(
    uint32_t sampling_period_in_us,
    uint32_t number_of_top_cpu_hoggers_to_print,
    uint32_t profiling_time_in_seconds
    );
int qapi_Cpu_Profile_Per_Fn_Profiling_Stop();

#endif // ENABLE_PER_FN_PROFILING

#endif /* _QAPI_CPUPROFILE_H_ */
