/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef MML_CMN_H
#define MML_CMN_H

typedef enum _service_id
{
    IOTD_SID_QAPI_CTRL=0,
    IOTD_SID_QAPI_MSG,
} SERVICE_ID_T;

typedef struct _mml_hdr
{
    unsigned char service_qid;
    unsigned char rsvd;
    unsigned short length;
} MML_HDR_T;

#endif
