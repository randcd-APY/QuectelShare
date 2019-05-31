/* mesh_fusion_interface.h
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __MESH_FUSION_INTERFACE_H__
#define __MESH_FUSION_INTERFACE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "is_common_intf.h"
#include "is.h"

int mesh_fusion_intf_init(void **mf_handle);
int mesh_fusion_intf_update_data(void *mf_handle, is_info_t *is_info);
int mesh_fusion_intf_process(void *mf_handle,is_output_type *is_output, int bitmask);
int mesh_fusion_intf_deinit(void *mf_handle);


#ifdef __cplusplus
}
#endif

#endif //__MESH_FUSION_INTERFACE_H__
