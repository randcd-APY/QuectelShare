/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 * Not a contribution
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vr_int.h"

// List thermal configs in format {name, algo_type}
// This list is manually synced with the thermal config

#define THERMAL_CONFIG_SYNCED 0

#if THERMAL_CONFIG_SYNCED

#define NUM_NON_VR_CONFIGS 4
static char *non_vr_thermal_configs[NUM_NON_VR_CONFIGS][2] =
    {{"SKIN-HIGH-FLOOR",     "ss"},
     {"SKIN-MID-FLOOR",      "ss"},
     {"SKIN-LOW-FLOOR",      "ss"},
     {"VIRTUAL-SS-GPU-SKIN", "ss"}};

#define NUM_VR_CONFIGS 1
static char *vr_thermal_configs[NUM_VR_CONFIGS][2] =
    {{"VR-EMMC",     "monitor"}};

int load_thermal_cfg_info(thermal_cfg_info_t *non_vr, thermal_cfg_info_t *vr) {

    if(!non_vr || !vr)
        return -1;

    non_vr->num_cfgs = NUM_NON_VR_CONFIGS;
    non_vr->cfgs = &non_vr_thermal_configs[0][0];

    vr->num_cfgs = NUM_VR_CONFIGS;
    vr->cfgs = &vr_thermal_configs[0][0];

    return 0;
}

#endif
