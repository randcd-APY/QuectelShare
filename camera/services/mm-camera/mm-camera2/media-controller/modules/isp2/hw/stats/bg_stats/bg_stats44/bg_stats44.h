/* bg_stats44.h
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __BG_STATS_44_H__
#define __BG_STATS_44_H__

/* mctl headers */
#include "modules.h"
#include "mct_event_stats.h"

/* isp headers */
#include "bg_stats_reg44.h"
#include "isp_sub_module_common.h"

/** bg_stats44_t:
 *
 *  @pcmd: hardware configuration
 *  @ispif_out_info: ispif out info
 *  @isp_out_info: isp out info
 *  @aec_config: aec config params
 *  @sensor_out_info: sensor out info
 *  @num_left_rgns: number of left regions
 *  @num_right_rgns: number of right regions
 *  @meta_enable: enable/disable bg stats in meta info
 **/
typedef struct {
  ISP_StatsBg_CfgCmdType     pcmd;
  ispif_out_info_t           ispif_out_info;
  isp_out_info_t             isp_out_info;
  aec_bg_config_t            bg_config;
  sensor_out_info_t          sensor_out_info;
  uint32_t                   num_left_rgns;
  uint32_t                   num_right_rgns;
  uint32_t                   meta_enable;
} bg_stats44_t;

boolean bg_stats44_trigger_update(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats46_stats_aec_config_update(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats46_stats_awb_config_update(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats44_streamoff(isp_sub_module_t *isp_sub_module, void *data);

boolean bg_stats44_init(isp_sub_module_t *isp_sub_module);

void bg_stats44_destroy(isp_sub_module_t *isp_sub_module);

boolean bg_stats44_set_stream_config(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats44_set_split_info(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats44_set_stripe_info(isp_sub_module_t *isp_sub_module,
  void *data);

boolean bg_stats44_set_meta_enable(mct_module_t *module,
  isp_sub_module_t *isp_sub_module, mct_event_t *event);

#endif /* __BG_STATS_44_H__ */
