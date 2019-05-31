/*
 * ---------------------------------------------------------------------------
 *  Copyright (c) 2015, 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */
#include <stdint.h>
#include <pthread.h>
#include "localdefs.h"
#include "time_genoff_i.h"

unsigned long long time_get_from_timetick(void)
{
	unsigned long long seconds = 0, ticks_qt = 0;
	unsigned long long cx32 = 0, cx32_xos = 0, xos = 0, timestamp = 0, ticks_1_25ms = 0;

#if defined __aarch64__ && __aarch64__ == 1
	asm volatile("mrs %0, cntvct_el0" : "=r" (ticks_qt));
#else
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (ticks_qt));
#endif
	xos = ticks_qt;
	cx32_xos = (xos * TIME_CONV_SCLK_CX32_DIVIDEND)/TIME_CONV_SCLK_CX32_DIVISOR;
	/* Add the number of 1.25ms units to the timestamp */
	cx32 += cx32_xos;
	ticks_1_25ms = cx32/TIME_CONV_CX32_PER_1p25MS;
	timestamp += (ticks_1_25ms << 16);
	/* Determine the remaining cx32s that we didn't add to the timestamp's upper part */
	cx32 -= (ticks_1_25ms * TIME_CONV_CX32_PER_1p25MS);
	/* Attach the cx32 back to end of the timestamp */
	timestamp |= cx32&0xffffULL;
	seconds = timestamp;

	return seconds;
}
