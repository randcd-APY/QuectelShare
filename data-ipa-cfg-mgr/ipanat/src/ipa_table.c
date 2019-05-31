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
#include "ipa_table.h"
#include "ipa_nat_utils.h"

#include <errno.h>

/* --------- Handle Entry ID structure ---------
------------------------------------------------
|  3bits   |    12 bits       |     1 bit      |
------------------------------------------------
| reserved | index into table |  0 - base      |
|          |                  |  1 - expansion |
------------------------------------------------
*/
#define IPA_TABLE_TYPE_BITS 0x1
#define IPA_TABLE_TYPE_MASK 0x1

#define IPA_BASE_TABLE_PERCENTAGE      .8
#define IPA_EXPANSION_TABLE_PERCENTAGE .2

/*
 * The table number of entries is limited by Entry ID structure above. The base table max entries is limited by
 * index into table bits number. The table max entries number is (base table max entries / base table percentage)
 * IPA_TABLE_MAX_ENTRIES = 2^|index into table| / IPA_BASE_TABLE_PERCENTAGE
 */
#define IPA_TABLE_MAX_ENTRIES 5120

static int InsertHead(ipa_table* table, void* head, void* user_data, struct ipa_ioc_nat_dma_cmd* cmd);
static int InsertTail(ipa_table* table, void* head, void* user_data, uint16_t* index, struct ipa_ioc_nat_dma_cmd* cmd);
static void CreateNextIndexUpdateDmaCommand(ipa_table* table, struct ipa_ioc_nat_dma_cmd* cmd,
	ipa_table_iterator* iterator, uint16_t data);
static uint16_t MakeEntryHdl(ipa_table* tbl, uint16_t tbl_entry);
static uint16_t FindExpnTblFreeEntry(ipa_table* table, void** free_entry);
static int Get2PowerTightUpperBound(uint16_t num);
static int GetEvenTightUpperBound(uint16_t num);

void ipa_table_init(ipa_table* table, const char* table_name, int entry_size, void* meta,
	ipa_table_entry_interface* entry_interface)
{
	IPADBG("\n");
	strlcpy(table->name, table_name, IPA_RESOURCE_NAME_MAX);
	table->entry_size = entry_size;
	table->meta = meta;
	table->entry_interface = entry_interface;
	IPADBG("Created table %s with entry size %d\n", table->name, table->entry_size);
}

int ipa_table_calculate_entries_num(ipa_table* table, uint16_t number_of_entries)
{
	uint16_t table_entries, expn_table_entries;

	IPADBG("\n");

	if (number_of_entries > IPA_TABLE_MAX_ENTRIES)
	{
		IPAERR("Required number of %s entries %d exceeds the maximum %d\n",
			table->name, number_of_entries, IPA_TABLE_MAX_ENTRIES);
		return -EINVAL;
	}

	table_entries = Get2PowerTightUpperBound(number_of_entries * IPA_BASE_TABLE_PERCENTAGE);
	expn_table_entries = GetEvenTightUpperBound(number_of_entries * IPA_EXPANSION_TABLE_PERCENTAGE);

	if (table_entries + expn_table_entries > IPA_TABLE_MAX_ENTRIES)
	{
		IPAERR("Required number of %s entries %d (user provided %d) exceeds the maximum %d\n",
			table->name, table_entries + expn_table_entries, number_of_entries, IPA_TABLE_MAX_ENTRIES);
		return -EINVAL;
	}

	table->table_entries = table_entries;
	table->expn_table_entries = expn_table_entries;

	IPADBG("num of %s entries:%d\n", table->name, table->table_entries);
	IPADBG("num of %s expn entries:%d\n", table->name, table->expn_table_entries);
	return 0;
}

int ipa_table_calculate_size(ipa_table* table)
{
	int size = table->entry_size * (table->table_entries + table->expn_table_entries);

	IPADBG("%s size: %d\n", table->name, size);

	return size;
}

uint8_t* ipa_table_calculate_addresses(ipa_table* table, uint8_t* base_addr)
{
	uint8_t* result;

	IPADBG("\n");

	table->table_addr = base_addr;
	table->expn_table_addr = table->table_addr + table->entry_size * table->table_entries;

	IPADBG("Table %s addresses: table_addr %pK expn_table_addr %pK\n",
		table->name, table->table_addr, table->expn_table_addr);

	result = table->expn_table_addr + table->entry_size * table->expn_table_entries;
	IPADBG("return\n");
	return result;
}

void ipa_table_reset(ipa_table* table)
{
	IPADBG("memset %s table to 0, %pK\n", table->name, table->table_addr);
	memset(table->table_addr, 0, table->entry_size * table->table_entries);

	IPADBG("memset %s expn table to 0, %pK\n", table->name, table->expn_table_addr);
	memset(table->expn_table_addr, 0, table->entry_size * table->expn_table_entries);

	IPADBG("returning from ipa_table_reset()\n");
}

int ipa_table_add_entry(ipa_table* table, void* user_data, uint16_t* index,
	uint32_t* rule_hdl, struct ipa_ioc_nat_dma_cmd* cmd)
{
	int ret;
	void* head;

	IPADBG("\n");

	head = table->table_addr + *index * table->entry_size;

	/* check whether there is any collision */
	if (table->entry_interface->entry_is_valid(head))
		ret = InsertTail(table, head, user_data, index, cmd);
	else
		ret = InsertHead(table, head, user_data, cmd);
	if (ret)
		return ret;

	IPADBG("New Entry Index %d in %s\n", *index, table->name);

	if (rule_hdl != NULL)
		*rule_hdl = MakeEntryHdl(table, *index);
	IPADBG("return\n");
	return 0;
}

void ipa_table_create_delete_command(ipa_table* table, struct ipa_ioc_nat_dma_cmd* cmd,
	ipa_table_iterator* iterator)
{
	IPADBG("Delete below rule in %s\n", table->name);
	IPADBG("tbl_entry:%d\n", iterator->curr_index);

	if (iterator->prev_index == IPA_TABLE_INVALID_ENTRY)
	{
		uint16_t data;
		ipa_table_dma_cmd_helper* helper;

		if (iterator->next_index == IPA_TABLE_INVALID_ENTRY)
		{
			helper = table->update_head_dma_cmd_helper;
			data = 0;
		}
		else
		{
			helper = table->delete_head_dma_cmd_helper;
			data = table->entry_interface->
				entry_get_delete_head_dma_command_data(iterator->curr_entry, iterator->next_entry);
		}

		ipa_table_dma_cmd_generate(helper, FALSE, (uint8_t*)iterator->curr_entry - table->table_addr, data, cmd);
	}
	else
	{
		CreateNextIndexUpdateDmaCommand(table, cmd, iterator, iterator->next_index);
	}

	IPADBG("return\n");
}

void ipa_table_delete_entry(ipa_table* table, ipa_table_iterator* iterator, uint8_t is_prev_empty)
{
	IPADBG("\n");

	if (iterator->next_index != IPA_TABLE_INVALID_ENTRY)
	{
		/* Update the next entry's prev_index field with current entry's prev_index */
		table->entry_interface->entry_set_prev_index(iterator->next_entry, iterator->next_index, iterator->prev_index,
			table->meta, table->table_entries);
	}
	else if (is_prev_empty)
	{
		if (iterator->prev_entry == NULL)
		{
			IPAERR("failed to delete of an empty head %d while delete the next entry %d in %s",
				iterator->prev_index, iterator->curr_index, table->name);
		}
		else
		{
			/* delete an empty head rule after the whole tail was deleted */
			IPADBG("deleting the dead node %d for %s\n", iterator->prev_index, table->name);
			memset(iterator->prev_entry, 0, table->entry_size);
			--table->cur_tbl_cnt;
		}
	}

	ipa_table_erase_entry(table, iterator->curr_index);

	IPADBG("return\n");
}

void ipa_table_erase_entry(ipa_table* table, uint16_t index)
{
	IPADBG("\n");

	if (index < table->table_entries)
	{
		memset(table->table_addr + index * table->entry_size, 0, table->entry_size);
		--table->cur_tbl_cnt;
	}
	else
	{
		memset(table->expn_table_addr + (index - table->table_entries) * table->entry_size, 0, table->entry_size);
		--table->cur_expn_tbl_cnt;
	}

	IPADBG("return\n");
}

/**
 * ipa_table_get_entry() - returns a table entry according to the received entry handle
 * @table: [in] the table
 * @entry_handle: [in] entry handle
 * @entry: [out] the retrieved entry
 * @entry_index: [out] absolute index of the retrieved entry
 *
 * Parse the entry handle to retrieve the entry and its index
 *
 * Returns: 0 on success, negative on failure
 */
int ipa_table_get_entry(ipa_table* table, uint32_t entry_handle, void** entry, uint16_t* entry_index)
{
	uint8_t is_expn_tbl;
	uint16_t index = 0;

	IPADBG("\n");

	/* Retrieve the table type */
	is_expn_tbl = entry_handle & IPA_TABLE_TYPE_MASK;

	/* Retrieve the table entry */
	index = (entry_handle >> IPA_TABLE_TYPE_BITS);

	if (is_expn_tbl)
	{
		if (index >= table->expn_table_entries)
		{
			IPAERR("invalid entry handle %d for %s\n", entry_handle, table->name);
			return -EINVAL;
		}

		*entry = table->expn_table_addr + index * table->entry_size;
		index += table->table_entries;
	}
	else
	{
		if (index >= table->table_entries)
		{
			IPAERR("invalid entry handle %d for %s\n", entry_handle, table->name);
			return -EINVAL;
		}

		*entry = table->table_addr + index * table->entry_size;
	}

	if (entry_index != NULL)
		*entry_index = index;

	IPADBG("return\n");
	return 0;
}

void* ipa_table_get_entry_by_index(ipa_table* table, uint16_t index)
{
	void* result;

	IPADBG("\n");
	if (!index || index >= table->table_entries + table->expn_table_entries)
	{
		IPAERR("Invalid index %d for %s\n", index, table->name);
		return NULL;
	}

	result = (index >= table->table_entries) ?
		table->expn_table_addr + (index - table->table_entries) * table->entry_size :
		table->table_addr + index * table->entry_size;

	IPADBG("return\n");
	return result;
}

void ipa_table_dma_cmd_helper_init(ipa_table_dma_cmd_helper* dma_cmd_helper, uint8_t table_indx,
	ipa_table_dma_type table_type, ipa_table_dma_type expn_table_type, uint32_t offset)
{
	IPADBG("\n");
	dma_cmd_helper->offset = offset;
	dma_cmd_helper->table_indx = table_indx;
	dma_cmd_helper->table_type = table_type;
	dma_cmd_helper->expn_table_type = expn_table_type;
	IPADBG("return\n");
}

void ipa_table_dma_cmd_generate(ipa_table_dma_cmd_helper* dma_cmd_helper,
	uint8_t is_expn, uint32_t entry_offset, uint16_t data, struct ipa_ioc_nat_dma_cmd* cmd)
{
	struct ipa_ioc_nat_dma_one* dma = &cmd->dma[cmd->entries++];

	IPADBG("\n");
	dma->table_index = dma_cmd_helper->table_indx;

	/* DMA parameter base_addr is the table type (see the IPA architecture document) */
	dma->base_addr = (is_expn) ? dma_cmd_helper->expn_table_type : dma_cmd_helper->table_type;

	dma->offset = dma_cmd_helper->offset + entry_offset;
	dma->data = data;

	IPADBG("Updating field with index: %u, base_addr: 0x%x, value: 0x%x offset: 0x%x\n",
		dma->table_index, dma->base_addr, dma->data, dma->offset);
}

int ipa_table_iterator_init(ipa_table_iterator* iterator, ipa_table* table, void* curr_entry, uint16_t curr_index)
{
	IPADBG("\n");

	if (!table->entry_interface->entry_is_valid(curr_entry))
	{
		IPAERR("Invalid (not enabled) rule %d in %s\n", curr_index, table->name);
		return -EINVAL;
	}

	memset(iterator, 0, sizeof(ipa_table_iterator));
	iterator->curr_entry = curr_entry;
	iterator->curr_index = curr_index;

	iterator->prev_index = table->entry_interface->entry_get_prev_index(curr_entry,
		curr_index, table->meta, table->table_entries);
	iterator->next_index = table->entry_interface->entry_get_next_index(curr_entry);

	if (iterator->prev_index != IPA_TABLE_INVALID_ENTRY)
	{
		iterator->prev_entry = ipa_table_get_entry_by_index(table, iterator->prev_index);
		if (iterator->prev_entry == NULL)
		{
			IPAERR("Failed to retrieve the entry in index %d for %s\n", iterator->prev_index, table->name);
			return -EPERM;
		}
	}

	if (iterator->next_index != IPA_TABLE_INVALID_ENTRY)
	{
		iterator->next_entry = ipa_table_get_entry_by_index(table, iterator->next_index);
		if (iterator->next_entry == NULL)
		{
			IPAERR("Failed to retrieve the entry in index %d for %s\n", iterator->next_index, table->name);
			return -EPERM;
		}
	}

	IPADBG("Created iterator: prev_index=%d curr_index=%d next_index=%d\n",
		iterator->prev_index, iterator->curr_index, iterator->next_index);
	IPADBG("                  prev_entry=%pK curr_entry=%pK next_entry=%pK\n",
		iterator->prev_entry, iterator->curr_entry, iterator->next_entry);

	return 0;
}

int ipa_table_iterator_next(ipa_table_iterator* iterator, ipa_table* table)
{
	IPADBG("\n");

	iterator->prev_entry = iterator->curr_entry;
	iterator->prev_index = iterator->curr_index;
	iterator->curr_entry = iterator->next_entry;
	iterator->curr_index = iterator->next_index;

	iterator->next_index = table->entry_interface->entry_get_next_index(iterator->curr_entry);
	if (iterator->next_index == IPA_TABLE_INVALID_ENTRY)
	{
		iterator->next_entry = NULL;
	}
	else
	{
		iterator->next_entry = ipa_table_get_entry_by_index(table, iterator->next_index);
		if (iterator->next_entry == NULL)
		{
			IPAERR("Failed to retrieve the entry in index %d for %s\n", iterator->next_index, table->name);
			return -EPERM;
		}
	}

	IPADBG("Iterator moved to: prev_index=%d curr_index=%d next_index=%d\n",
		iterator->prev_index, iterator->curr_index, iterator->next_index);
	IPADBG("                   prev_entry=%pK curr_entry=%pK next_entry=%pK\n",
		iterator->prev_entry, iterator->curr_entry, iterator->next_entry);
	return 0;
}

int ipa_table_iterator_end(ipa_table_iterator* iterator, ipa_table* table, uint16_t head_index, void* head)
{
	IPADBG("\n");

	memset(iterator, 0, sizeof(ipa_table_iterator));
	iterator->prev_index = head_index;
	iterator->prev_entry = head;

	for (;;)
	{
		iterator->curr_index = table->entry_interface->entry_get_next_index(iterator->prev_entry);
		if (iterator->curr_index == IPA_TABLE_INVALID_ENTRY)
		{
			break;
		}
		if (iterator->prev_index == iterator->curr_index)
		{
			IPAERR("Error: Prev index:%d and next:%d index should not be same in %s\n",
				iterator->prev_index, iterator->curr_index, table->name);
			return -EINVAL;
		}
		iterator->prev_entry =
			table->expn_table_addr + (iterator->curr_index - table->table_entries) * table->entry_size;
		iterator->prev_index = iterator->curr_index;
	}

	IPADBG("return\n");
	return 0;
}

int ipa_table_iterator_is_head_with_tail(ipa_table_iterator* iterator)
{
	return iterator->prev_index == IPA_TABLE_INVALID_ENTRY && iterator->next_index != IPA_TABLE_INVALID_ENTRY;
}

static int InsertHead(ipa_table* table, void* head, void* user_data, struct ipa_ioc_nat_dma_cmd* cmd)
{
	int ret;
	uint16_t dma_command_data;

	IPADBG("\n");
	ret = table->entry_interface->entry_head_insert(head, user_data, &dma_command_data);
	if (ret)
	{
		IPAERR("unable to insert a new entry to the head in %s\n", table->name);
		return ret;
	}

	ipa_table_dma_cmd_generate(table->update_head_dma_cmd_helper,
		FALSE, (uint8_t*)head - table->table_addr, dma_command_data, cmd);
	++table->cur_tbl_cnt;
	IPADBG("return\n");
	return 0;
}

static int InsertTail(ipa_table* table, void* head, void* user_data, uint16_t* index, struct ipa_ioc_nat_dma_cmd* cmd)
{
	int ret;
	ipa_table_iterator iterator;

	IPADBG("\n");

	ret = ipa_table_iterator_end(&iterator, table, *index, head);
	if (ret)
	{
		IPAERR("Error: Failed to reach the end of the list in index %d in %s\n", *index, table->name);
		return ret;
	}

	iterator.curr_index = FindExpnTblFreeEntry(table, &iterator.curr_entry);
	if (iterator.curr_index == IPA_TABLE_INVALID_ENTRY)
	{
		IPAERR("Expansion table is full in %s\n", table->name);
		return -EINVAL;
	}

	ret = table->entry_interface->entry_tail_insert(iterator.curr_entry, user_data);
	if (ret)
	{
		IPAERR("unable to insert a new entry to the tail in %s\n", table->name);
		return ret;
	}

	table->entry_interface->entry_set_prev_index(iterator.curr_entry, iterator.curr_index, iterator.prev_index,
		table->meta, table->table_entries);

	CreateNextIndexUpdateDmaCommand(table, cmd, &iterator, iterator.curr_index);

	++table->cur_expn_tbl_cnt;
	*index = iterator.curr_index;
	IPADBG("return\n");
	return 0;
}

static void CreateNextIndexUpdateDmaCommand(ipa_table* table, struct ipa_ioc_nat_dma_cmd* cmd,
	ipa_table_iterator* iterator, uint16_t data)
{
	uint8_t* table_addr;
	uint8_t is_expn;

	IPADBG("\n");

	if (iterator->prev_index >= table->table_entries)
	{
		table_addr = table->expn_table_addr;
		is_expn = TRUE;
	}
	else
	{
		table_addr = table->table_addr;
		is_expn = FALSE;
	}
	ipa_table_dma_cmd_generate(table->update_entry_dma_cmd_helper, is_expn,
		(uint8_t*)iterator->prev_entry - table_addr, data, cmd);

	IPADBG("return\n");
}

/**
 * MakeEntryHdl() - makes an entry handle
 * @tbl_hdl: [in] tbl - the table
 * @tbl_entry: [in] tbl_entry - table entry
 *
 * Calculate the entry handle which will be returned to client
 *
 * Returns: >0 table entry handle
 */
static uint16_t MakeEntryHdl(ipa_table* tbl, uint16_t tbl_entry)
{
	uint16_t entry_hdl = 0;

	IPADBG("\n");

	if (tbl_entry >= tbl->table_entries)
	{
		/* Update the index into table */
		entry_hdl = tbl_entry - tbl->table_entries;
		entry_hdl = (entry_hdl << IPA_TABLE_TYPE_BITS);
		/* Update the table type mask */
		entry_hdl = (entry_hdl | IPA_TABLE_TYPE_MASK);
	}
	else
	{
		entry_hdl = tbl_entry;
		entry_hdl = (entry_hdl << IPA_TABLE_TYPE_BITS);
	}

	IPADBG("return %d\n", entry_hdl);
	return entry_hdl;
}

/* returns expn table entry absolute index */
static uint16_t FindExpnTblFreeEntry(ipa_table* table, void** free_entry)
{
	uint16_t i;
	uint8_t* entry;

	IPADBG("\n");

	for (i = 0, entry = table->expn_table_addr; i < table->expn_table_entries; ++i, entry += table->entry_size)
	{
		if (!table->entry_interface->entry_is_valid(entry))
		{
			IPADBG("new expansion table entry index %d in %s\n", i, table->name);
			*free_entry = entry;
			return i + table->table_entries;
		}
	}

	IPAERR("%s expansion table is full.\nCurrent table entries: %d & expansion table entries: %d\n\n",
		table->name, table->cur_tbl_cnt, table->cur_expn_tbl_cnt);
	return 0;
}

/**
 * Get2PowerTightUpperBound() - Returns the tight upper bound which is a power of 2
 * @num: [in] given number
 *
 * Returns the tight upper bound for a given number which is power of 2
 *
 * Returns: the tight upper bound which is power of 2
 */
static int Get2PowerTightUpperBound(uint16_t num)
{
	uint16_t tmp = num, prev = 0, curr = 2;

	if (num == 0)
		return 2;

	while (tmp != 1)
	{
		prev = curr;
		curr <<= 1;
		tmp >>= 1;
	}

	return (num == prev) ? prev : curr;
}

/**
 * GetEvenTightUpperBound() - Returns the tight upper bound which is an even number
 * @num: [in] given number
 *
 * Returns the tight upper bound for a given number which is an even number
 *
 * Returns: the tight upper bound which is an even number
 */
static int GetEvenTightUpperBound(uint16_t num)
{
	if (num == 0)
		return 2;

	return (num % 2) ? num + 1 : num;
}

