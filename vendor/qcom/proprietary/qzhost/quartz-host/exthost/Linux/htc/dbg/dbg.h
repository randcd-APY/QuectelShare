/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _DBG_H_
#define _DBG_H_

enum {
    HTC_PRINT_GRP0_HIF=0x01,
    HTC_PRINT_GRP1=0x02,
    HTC_PRINT_GRP2=0x04,
    HTC_PRINT_GRP_WORK_TASK=0x04,
    HTC_PRINT_GRP3=0x08,
	 HTC_PRINT_GRP4=0x10,
	 HTC_PRINT_GRP5=0x20,
	 HTC_PRINT_GRP6=0x40,
	 HTC_PRINT_GRP7=0x80,
	 HTC_PRINT_GRP8=0x100,
	 HTC_PRINT_GRP9=0x200,
	HTC_PRINT_GRP10=0x400,
	HTC_PRINT_GRP11=0x800,
	HTC_PRINT_GRP12=0x1000,
	HTC_PRINT_GRP13=0x2000,
	HTC_PRINT_GRP14=0x4000,
	HTC_PRINT_GRP15=0x8000,
	HTC_PRINT_GRP16=0x10000,
	HTC_PRINT_GRP17=0x20000,
	HTC_PRINT_GRP0= 0x40000,

    HTC_PRINT_GRP_ERR= 0x80000000,    
};

#ifdef  DEBUG_HTC
void HTCPrintf(uint32_t print, char *fmt,...);
#else
#define HTCPrintf(print, fmt,...)
#endif

void HTCPrintSetMask (int pos, uint32_t new_mask);
void htc_printf(char *fmt,...);

#endif
