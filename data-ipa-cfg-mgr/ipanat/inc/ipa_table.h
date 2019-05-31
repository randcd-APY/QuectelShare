/*
 * Copyright (c) 2018 The Linux Foundation. All rights reserved.
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
#ifndef IPA_TABLE_H
#define IPA_TABLE_H

#include <stdint.h>
#include <linux/msm_ipa.h>

#define IPA_TABLE_INVALID_ENTRY 0x0

typedef enum
{
	IPA_NAT_BASE_TBL = 0,
	IPA_NAT_EXPN_TBL = 1,
	IPA_NAT_INDX_TBL = 2,
	IPA_NAT_INDEX_EXPN_TBL = 3,
	IPA_IPV6CT_BASE_TBL = 4,
	IPA_IPV6CT_EXPN_TBL = 5,
} ipa_table_dma_type;

typedef struct
{
	void* prev_entry;
	void* curr_entry;
	void* next_entry;
	uint16_t prev_index;
	uint16_t curr_index;
	uint16_t next_index;
} ipa_table_iterator;

typedef struct
{
	uint32_t offset;
	ipa_table_dma_type table_type;
	ipa_table_dma_type expn_table_type;
	uint8_t table_indx;
} ipa_table_dma_cmd_helper;

typedef int(*entry_validity_checker)(void* entry);
typedef uint16_t(*entry_next_index_getter)(void* entry);
typedef uint16_t(*entry_prev_index_getter)(void* entry, uint16_t entry_index, void* meta, uint16_t base_table_size);
typedef void(*entry_prev_index_setter)(void* entry, uint16_t entry_index, uint16_t prev_index,
	void* meta, uint16_t base_table_size);
typedef int(*entry_head_inserter)(void* entry, void* user_data, uint16_t* dma_command_data);
typedef int(*entry_tail_inserter)(void* entry, void* user_data);
typedef uint16_t(*entry_delete_head_dma_command_data_getter)(void* head, void* next_entry);

typedef struct
{
	entry_validity_checker entry_is_valid;
	entry_next_index_getter entry_get_next_index;
	entry_prev_index_getter entry_get_prev_index;
	entry_prev_index_setter entry_set_prev_index;
	entry_head_inserter entry_head_insert;
	entry_tail_inserter entry_tail_insert;
	entry_delete_head_dma_command_data_getter entry_get_delete_head_dma_command_data;
} ipa_table_entry_interface;

typedef struct
{
	char name[IPA_RESOURCE_NAME_MAX];
	int entry_size;

	uint8_t* table_addr;
	uint8_t* expn_table_addr;

	void* meta;

	ipa_table_entry_interface* entry_interface;

	ipa_table_dma_cmd_helper* update_head_dma_cmd_helper;
	ipa_table_dma_cmd_helper* update_entry_dma_cmd_helper;
	ipa_table_dma_cmd_helper* delete_head_dma_cmd_helper;

	uint16_t table_entries;
	uint16_t expn_table_entries;

	uint16_t cur_tbl_cnt;
	uint16_t cur_expn_tbl_cnt;
} ipa_table;

void ipa_table_init(ipa_table* table, const char* table_name, int entry_size, void* meta,
	ipa_table_entry_interface* entry_interface);

int ipa_table_calculate_entries_num(ipa_table* table, uint16_t number_of_entries);
int ipa_table_calculate_size(ipa_table* table);
uint8_t* ipa_table_calculate_addresses(ipa_table* table, uint8_t* base_addr);
void ipa_table_reset(ipa_table* table);
int ipa_table_add_entry(ipa_table* table, void* user_data, uint16_t* index,
	uint32_t* rule_hdl, struct ipa_ioc_nat_dma_cmd* cmd);
void ipa_table_create_delete_command(ipa_table* table, struct ipa_ioc_nat_dma_cmd* cmd, ipa_table_iterator* iterator);
void ipa_table_delete_entry(ipa_table* table, ipa_table_iterator* iterator, uint8_t is_prev_empty);
void ipa_table_erase_entry(ipa_table* table, uint16_t index);
int ipa_table_get_entry(ipa_table* table, uint32_t entry_handle, void** entry, uint16_t* entry_index);
void* ipa_table_get_entry_by_index(ipa_table* table, uint16_t index);

void ipa_table_dma_cmd_helper_init(ipa_table_dma_cmd_helper* dma_cmd_helper, uint8_t table_indx,
	ipa_table_dma_type table_type, ipa_table_dma_type expn_table_type, uint32_t offset);
void ipa_table_dma_cmd_generate(ipa_table_dma_cmd_helper* dma_cmd_helper,
	uint8_t is_expn, uint32_t entry_offset, uint16_t data, struct ipa_ioc_nat_dma_cmd* cmd);

int ipa_table_iterator_init(ipa_table_iterator* iterator, ipa_table* table, void* curr_entry, uint16_t curr_index);
int ipa_table_iterator_next(ipa_table_iterator* iterator, ipa_table* table);
int ipa_table_iterator_end(ipa_table_iterator* iterator, ipa_table* table, uint16_t head_index, void* head);
int ipa_table_iterator_is_head_with_tail(ipa_table_iterator* iterator);

#endif

