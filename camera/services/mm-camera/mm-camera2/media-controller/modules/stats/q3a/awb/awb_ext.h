/* awb_ext.h
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __AWB_EXT_H__
#define __AWB_EXT_H__

#include "awb_port.h"
#include "awb.h"
#include "stats_util.h"

/**
 * Custom output mask.
 */
typedef enum {
  AWB_OUTPUT_CUSTOM_MAX
} awb_ext_output_mask_type;

/**
 * Custom output structure. External core should define
 * the parameters here.
 */
typedef struct {
  uint32_t mask;
  /* Add parameters here */
} awb_ext_output_t;


typedef struct _awb_ext_param {
  awb_ext_output_t custom_output;
} awb_ext_param_t;


boolean awb_port_ext_is_extension_required(void **awb_libptr,
  cam_position_t cam_position, boolean *use_default_algo);
boolean awb_port_ext_update_func_table(void *ptr);
void * awb_port_ext_load_function(awb_ops_t *awb_ops, void *awb_libptr,
  cam_position_t cam_position, boolean use_default_algo);
void awb_port_ext_unload_function(awb_port_private_t *private);



#endif /*__AWB_EXT_H__*/
