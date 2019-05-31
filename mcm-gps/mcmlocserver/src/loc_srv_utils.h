//=============================================================================
// FILE: loc_srv_utils.h
//
// DESCRIPTION:
// Location server utils
//
// Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================

#ifndef LOC_SRV_UTILS_H
#define LOC_SRV_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOC_SRV_SUCCESS 0
#define LOC_SRV_FAILURE -1
#define LOC_SRV_TRUE 1
#define LOC_SRV_FALSE 0

#define LOC_SRV_NULL_CHECK(val)  if (val == NULL) return LOC_SRV_FAILURE;
#define LOC_SRV_RC_CHECK(val) if (val == LOC_SRV_FAILURE) return LOC_SRV_FAILURE;

#ifdef __cplusplus
}
#endif
#endif // LOC_SRV_UTILS_H
