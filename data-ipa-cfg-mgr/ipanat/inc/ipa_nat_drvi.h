/*
 * Copyright (c) 2013 - 2018 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IPA_NAT_DRVI_H
#define IPA_NAT_DRVI_H

#include "ipa_table.h"
#include "ipa_mem_descriptor.h"
#include "ipa_nat_utils.h"

/*======= IMPLEMENTATION related data structures and functions ======= */

#define IPA_NAT_MAX_IP4_TBLS   1

#define IPA_NAT_RULE_FLAG_FIELD_OFFSET        18
#define IPA_NAT_RULE_NEXT_FIELD_OFFSET        8
#define IPA_NAT_RULE_PROTO_FIELD_OFFSET       22

#define IPA_NAT_INDEX_RULE_NEXT_FIELD_OFFSET       2
#define IPA_NAT_INDEX_RULE_NAT_INDEX_FIELD_OFFSET  0

#define IPA_NAT_FLAG_ENABLE_BIT  1

#define IPA_NAT_INVALID_PROTO_FIELD_VALUE 0xFF00
#define IPA_NAT_INVALID_PROTO_FIELD_CMP   0xFF

typedef enum {
	IPA_NAT_TABLE_FLAGS,
	IPA_NAT_TABLE_NEXT_INDEX,
	IPA_NAT_TABLE_PROTOCOL,
	IPA_NAT_INDEX_TABLE_ENTRY,
	IPA_NAT_INDEX_TABLE_NEXT_INDEX,
	IPA_NAT_TABLE_DMA_CMD_MAX
} ipa_nat_table_dma_cmd_type;

/*------------------------  NAT Table Entry  -----------------------------------------

  ------------------------------------------------------------------------------------
  |   7    |    6    |   5    |    4    |     3        |  2   |    1    |    0      |
  ------------------------------------------------------------------------------------
  |             Target IP(4B)           |             Private IP(4B)                 |
  ------------------------------------------------------------------------------------
  |Target Port(2B)   | Private Port(2B) | Public Port(2B)     | Next Index(2B)       |
  ------------------------------------------------------------------------------------
  | Proto   |      TimeStamp(3B)        |       Flags(2B)     | IP check sum Diff(2B)|
  | (1B)    |                           |EN|Redirect|Resv     |                      |
  ------------------------------------------------------------------------------------
  | TCP/UDP checksum |PDN info|Reserved |    SW Specific Parameters(4B)              |
  |    diff (2B)     |  (1B)  |  (1B)   |                                            |
  ------------------------------------------------------------------------------------

  Dont change below structure definition.
  It should be same as above(little endian order)
  -------------------------------------------------------------------------------*/
struct ipa_nat_rule {
	uint64_t private_ip:32;
	uint64_t target_ip:32;

	uint64_t next_index:16;
	uint64_t public_port:16;
	uint64_t private_port:16;
	uint64_t target_port:16;

	uint64_t ip_chksum:16;
	uint64_t rsvd1:14;
	uint64_t redirect:1;
	uint64_t enable:1;
	uint64_t time_stamp:24;
	uint64_t protocol:8;

	/*--------------------------------------------------
	32 bit sw_spec_params is interpreted as follows
	------------------------------------
	|     16 bits     |     16 bits    |
	------------------------------------
	|  index table    |  prev index    |
	|     entry       |                |
	------------------------------------
	--------------------------------------------------*/
	uint64_t prev_index:16;
	uint64_t indx_tbl_entry:16;
	uint64_t rsvd2:8;
	/*-----------------------------------------
	8 bit PDN info is interpreted as following
	------------------------------------
	|     4 bits      |     4 bits     |
	------------------------------------
	|  PDN index      |    reserved    |
	|                 |                |
	------------------------------------
	-------------------------------------------*/
	uint64_t rsvd3:4;
	uint64_t pdn_index:4;
	uint64_t tcp_udp_chksum:16;
};

/*
	---------------------------------------
	|         1        |         0        |
	---------------------------------------
	|               Flags(2B)             |
	|Enable|Redirect|Resv                 |
	---------------------------------------
*/
typedef struct {
	uint32_t rsvd1:14;
	uint32_t redirect:1;
	uint32_t enable:1;
} ipa_nat_flags;

struct ipa_nat_indx_tbl_rule {
	uint16_t tbl_entry;
	uint16_t next_index;
};

struct ipa_nat_indx_tbl_meta_info {
	uint16_t prev_index;
};

struct ipa_nat_ip4_table_cache {
	uint32_t public_addr;
	ipa_mem_descriptor mem_desc;
	ipa_table table;
	ipa_table index_table;
	struct ipa_nat_indx_tbl_meta_info *index_expn_table_meta;
	ipa_table_dma_cmd_helper table_dma_cmd_helpers[IPA_NAT_TABLE_DMA_CMD_MAX];
};

struct ipa_nat_cache {
	ipa_descriptor* ipa_desc;
	struct ipa_nat_ip4_table_cache ip4_tbl[IPA_NAT_MAX_IP4_TBLS];
	uint8_t table_cnt;
};

int ipa_nati_add_ipv4_tbl(uint32_t public_ip_addr,
				uint16_t number_of_entries,
				uint32_t *table_hanle);

int ipa_nati_del_ipv4_table(uint32_t tbl_hdl);

int ipa_nati_query_timestamp(uint32_t  tbl_hdl,
				uint32_t  rule_hdl,
				uint32_t  *time_stamp);

int ipa_nati_modify_pdn(struct ipa_ioc_nat_pdn_entry *entry);

int ipa_nati_get_pdn_index(uint32_t public_ip, uint8_t *pdn_index);

int ipa_nati_alloc_pdn(ipa_nat_pdn_entry *pdn_info, uint8_t *pdn_index);

int ipa_nati_get_pdn_cnt(void);

int ipa_nati_dealloc_pdn(uint8_t pdn_index);

int ipa_nati_add_ipv4_rule(uint32_t tbl_hdl,
				const ipa_nat_ipv4_rule *clnt_rule,
				uint32_t *rule_hdl);

int ipa_nati_del_ipv4_rule(uint32_t tbl_hdl,
				uint32_t rule_hdl);

#endif /* #ifndef IPA_NAT_DRVI_H */

