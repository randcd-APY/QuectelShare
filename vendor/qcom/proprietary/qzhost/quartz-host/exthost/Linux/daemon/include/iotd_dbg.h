/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _IOTD_DBG_H_
#define _IOTD_DBG_H_


void iotd_log(int level, char* format, ...);
int32_t dbg_init(void* pCxt);

#define IOTD_SYSLOG
#define DBG_STDOUT

#define LOG_LEVEL_VERBOSE          (0x03)
#define LOG_LEVEL_DEFAULT          (0x01)   


#define LOG_TYPE_CRIT         (0x01) /*Print errors*/
#define LOG_TYPE_WARN         (0x02) /*Print warnings and errors*/
#define LOG_TYPE_INFO         (0x03) /*Print info, warnings and errors*/
#define LOG_TYPE_VERBOSE      (0x04) 

#define IOTD_LOG   iotd_log

#endif
