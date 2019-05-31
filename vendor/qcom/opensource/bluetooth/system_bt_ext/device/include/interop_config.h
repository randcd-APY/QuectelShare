/******************************************************************************
 *
 *  Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <errno.h>
#include "device/include/interop_database.h"
#include "device/include/interop.h"


typedef struct {
  bt_bdaddr_t addr;
  size_t length;
  uint16_t max_lat;
  interop_feature_t feature;
} interop_hid_ssr_max_lat_t;

typedef enum {
    INTEROP_BL_TYPE_ADDR = 0,
    INTEROP_BL_TYPE_NAME,
    INTEROP_BL_TYPE_MANUFACTURE,
    INTEROP_BL_TYPE_VNDR_PRDT,
    INTEROP_BL_TYPE_SSR_MAX_LAT,

} interop_bl_type;

typedef enum {
    INTEROP_ENTRY_TYPE_STATIC = 1 << 0,
    INTEROP_ENTRY_TYPE_DYNAMIC = 1 << 1
} interop_entry_type;

typedef struct {
    interop_bl_type bl_type;
    interop_entry_type bl_entry_type;

    union {
        interop_addr_entry_t addr_entry;
        interop_name_entry_t name_entry;
        interop_manufacturer_t mnfr_entry;
        interop_hid_multitouch_t vnr_pdt_entry;
        interop_hid_ssr_max_lat_t ssr_max_lat_entry;
    } entry_type;

} interop_db_entry_t;

// API's for adding entries to dynamic interop database
void interop_database_add_addr(const uint16_t feature, const bt_bdaddr_t *addr, size_t length);
void interop_database_add_name(const uint16_t feature, const char *name);
void interop_database_add_manufacturer(const interop_feature_t feature, uint16_t manufacturer);
void interop_database_add_vndr_prdt(const interop_feature_t feature, uint16_t vendor_id, uint16_t product_id);

// API's for removing entries from dynamic interop database
bool interop_database_remove_addr(const interop_feature_t feature, const bt_bdaddr_t *addr);
bool interop_database_remove_name( const interop_feature_t feature, const char *name);
bool interop_database_remove_manufacturer( const interop_feature_t feature, uint16_t manufacturer);
bool interop_database_remove_vndr_prdt(const interop_feature_t feature, uint16_t vendor_id, uint16_t product_id);

// API's to match entries with in dynamic interop database
bool interop_database_match_addr(const interop_feature_t feature, const bt_bdaddr_t *addr);
bool interop_database_match_name( const interop_feature_t feature, const char *name);
bool interop_database_match_manufacturer(const interop_feature_t feature, uint16_t manufacturer);
bool interop_database_match_vndr_prdt(const interop_feature_t feature, uint16_t vendor_id, uint16_t product_id);
bool interop_database_match_addr_get_max_lat(const interop_feature_t feature,
          const bt_bdaddr_t *addr, uint16_t *max_lat);
