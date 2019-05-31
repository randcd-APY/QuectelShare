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

#include "ipa_nat_drv.h"
#include "ipa_nat_drvi.h"

#include <sys/ioctl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_DMA_ENTRIES_FOR_ADD 2
#define MAX_DMA_ENTRIES_FOR_DEL 3

#define IPA_NAT_DEBUG_FILE_PATH "/sys/kernel/debug/ipa/ip4_nat"
#define IPA_NAT_TABLE_NAME "IPA NAT table"
#define IPA_NAT_INDEX_TABLE_NAME "IPA NAT index table"

struct ipa_nat_cache ipv4_nat_cache;
static pthread_mutex_t nat_mutex = PTHREAD_MUTEX_INITIALIZER;

static ipa_nat_pdn_entry pdns[IPA_MAX_PDN_NUM];
static int num_pdns = 0;

static int ipa_nati_create_table(struct ipa_nat_ip4_table_cache* nat_table,
	uint32_t public_ip_addr, uint16_t number_of_entries, uint8_t table_index);
static int ipa_nati_destroy_table(struct ipa_nat_ip4_table_cache* nat_table);
static void ipa_nati_create_table_dma_cmd_helpers(struct ipa_nat_ip4_table_cache* nat_table, uint8_t table_indx);
static int ipa_nati_post_ipv4_init_cmd(struct ipa_nat_ip4_table_cache* nat_table, uint8_t tbl_index);
static void ipa_nati_copy_second_index_entry_to_head(struct ipa_nat_ip4_table_cache* nat_table,
	ipa_table_iterator* index_table_iterator, struct ipa_ioc_nat_dma_cmd* cmd);

static int table_entry_is_valid(void* entry);
static uint16_t table_entry_get_next_index(void* entry);
static uint16_t table_entry_get_prev_index(void* entry, uint16_t entry_index, void* meta, uint16_t base_table_size);
static void table_entry_set_prev_index(void* entry, uint16_t entry_index, uint16_t prev_index,
	void* meta, uint16_t base_table_size);
static int table_entry_head_insert(void* entry, void* user_data, uint16_t* dma_command_data);
static int table_entry_tail_insert(void* entry, void* user_data);
static uint16_t table_entry_get_delete_head_dma_command_data(void* head, void* next_entry);

static int index_table_entry_is_valid(void* entry);
static uint16_t index_table_entry_get_next_index(void* entry);
static uint16_t index_table_entry_get_prev_index(void* entry, uint16_t entry_index,
	void* meta, uint16_t base_table_size);
static void index_table_entry_set_prev_index(void* entry, uint16_t entry_index, uint16_t prev_index,
	void* meta, uint16_t base_table_size);
static int index_table_entry_head_insert(void* entry, void* user_data, uint16_t* dma_command_data);
static int index_table_entry_tail_insert(void* entry, void* user_data);
static uint16_t index_table_entry_get_delete_head_dma_command_data(void* head, void* next_entry);

static ipa_table_entry_interface entry_interface = {
	table_entry_is_valid,
	table_entry_get_next_index,
	table_entry_get_prev_index,
	table_entry_set_prev_index,
	table_entry_head_insert,
	table_entry_tail_insert,
	table_entry_get_delete_head_dma_command_data
};

static ipa_table_entry_interface index_entry_interface = {
	index_table_entry_is_valid,
	index_table_entry_get_next_index,
	index_table_entry_get_prev_index,
	index_table_entry_set_prev_index,
	index_table_entry_head_insert,
	index_table_entry_tail_insert,
	index_table_entry_get_delete_head_dma_command_data
};

/* ------------------------------------------
 *	UTILITY FUNCTIONS START
 *--------------------------------------------*/

/**
 * dst_hash() - Find the index into ipv4 base table
 * @public_ip: [in] public_ip
 * @trgt_ip: [in] Target IP address
 * @trgt_port: [in]  Target port
 * @public_port: [in]  Public port
 * @proto: [in] Protocol (TCP/IP)
 * @size: [in] size of the ipv4 base Table
 *
 * This hash method is used to find the hash index of new nat
 * entry into ipv4 base table. In case of zero index, the
 * new entry will be stored into N-1 index where N is size of
 * ipv4 base table
 *
 * Returns: >0 index into ipv4 base table, negative on failure
 */
static uint16_t dst_hash(uint32_t public_ip, uint32_t trgt_ip,
	uint16_t trgt_port, uint16_t public_port,
	uint8_t proto, uint16_t size)
{
	uint16_t hash = ((uint16_t)(trgt_ip)) ^ ((uint16_t)(trgt_ip >> 16)) ^
		 (trgt_port) ^ (public_port) ^ (proto);

	IPADBG("public ip 0x%X\n", public_ip);
	IPADBG("trgt_ip: 0x%x trgt_port: 0x%x\n", trgt_ip, trgt_port);
	IPADBG("public_port: 0x%x\n", public_port);
	IPADBG("proto: 0x%x size: 0x%x\n", proto, size);

	if (ipv4_nat_cache.ipa_desc->ver >= IPA_HW_v4_0)
		hash ^= ((uint16_t)(public_ip)) ^
		((uint16_t)(public_ip >> 16));

	/*
	 * The size passed to hash function expected be power^2-1, while the actual size is power^2,
	 * actual_size = size + 1
	 */
	hash = (hash & size);

	/* If the hash resulted to zero then set it to maximum value as zero is unused entry in nat tables */
	if (hash == 0) {
		hash = size;
	}

	IPADBG("dst_hash returning value: %d\n", hash);
	return hash;
}

/**
 * src_hash() - Find the index into ipv4 index base table
 * @priv_ip: [in] Private IP address
 * @priv_port: [in]  Private port
 * @trgt_ip: [in]  Target IP address
 * @trgt_port: [in] Target Port
 * @proto: [in]  Protocol (TCP/IP)
 * @size: [in] size of the ipv4 index base Table
 *
 * This hash method is used to find the hash index of new nat
 * entry into ipv4 index base table. In case of zero index, the
 * new entry will be stored into N-1 index where N is size of
 * ipv4 index base table
 *
 * Returns: >0 index into ipv4 index base table, negative on failure
 */
static uint16_t src_hash(uint32_t priv_ip, uint16_t priv_port,
	uint32_t trgt_ip, uint16_t trgt_port,
	uint8_t proto, uint16_t size)
{
	uint16_t hash =  ((uint16_t)(priv_ip)) ^ ((uint16_t)(priv_ip >> 16)) ^
		 (priv_port) ^
		 ((uint16_t)(trgt_ip)) ^ ((uint16_t)(trgt_ip >> 16)) ^
		 (trgt_port) ^ (proto);

	IPADBG("priv_ip: 0x%x priv_port: 0x%x\n", priv_ip, priv_port);
	IPADBG("trgt_ip: 0x%x trgt_port: 0x%x\n", trgt_ip, trgt_port);
	IPADBG("proto: 0x%x size: 0x%x\n", proto, size);

	/*
	 * The size passed to hash function expected be power^2-1, while the actual size is power^2,
	 * actual_size = size + 1
	 */
	hash = (hash & size);

	/* If the hash resulted to zero then set it to maximum value as zero is unused entry in nat tables */
	if (hash == 0) {
		hash = size;
	}

	IPADBG("src_hash returning value: %d\n", hash);
	return hash;
}

/**
 * ipa_nati_calc_ip_cksum() - Calculate the source nat IP checksum diff
 * @pub_ip_addr: [in] public ip address
 * @priv_ip_addr: [in]	Private ip address
 *
 * source nat ip checksum different is calculated as
 * public_ip_addr - private_ip_addr
 * Here we are using 1's complement to represent -ve number.
 * So take 1's complement of private ip addr and add it
 * to public ip addr.
 *
 * Returns: >0 ip checksum diff
 */
static uint16_t ipa_nati_calc_ip_cksum(uint32_t pub_ip_addr, uint32_t priv_ip_addr)
{
	uint16_t ret;
	uint32_t cksum = 0;

	IPADBG("\n");

	/* Add LSB(2 bytes) of public ip address to cksum */
	cksum += (pub_ip_addr & 0xFFFF);

	/* Add MSB(2 bytes) of public ip address to cksum
		and check for carry forward(CF), if any add it
	*/
	cksum += (pub_ip_addr>>16);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Calculate the 1's complement of private ip address */
	priv_ip_addr = (~priv_ip_addr);

	/* Add LSB(2 bytes) of private ip address to cksum
		 and check for carry forward(CF), if any add it
	*/
	cksum += (priv_ip_addr & 0xFFFF);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Add MSB(2 bytes) of private ip address to cksum
		 and check for carry forward(CF), if any add it
	*/
	cksum += (priv_ip_addr>>16);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Return the LSB(2 bytes) of checksum	*/
	ret = (uint16_t)cksum;
	IPADBG("return\n");
	return ret;
}

/**
 * ipa_nati_calc_tcp_udp_cksum() - Calculate the source nat TCP/UDP checksum diff
 * @pub_ip_addr: [in] public ip address
 * @pub_port: [in] public tcp/udp port
 * @priv_ip_addr: [in]	Private ip address
 * @priv_port: [in] Private tcp/udp prot
 *
 * source nat tcp/udp checksum is calculated as
 * (pub_ip_addr + pub_port) - (priv_ip_addr + priv_port)
 * Here we are using 1's complement to represent -ve number.
 * So take 1's complement of prviate ip addr &private port
 * and add it public ip addr & public port.
 *
 * Returns: >0 tcp/udp checksum diff
 */
static uint16_t ipa_nati_calc_tcp_udp_cksum(uint32_t pub_ip_addr,
	uint16_t pub_port,
	uint32_t priv_ip_addr,
	uint16_t priv_port)
{
	uint16_t ret = 0;
	uint32_t cksum = 0;

	IPADBG("\n");

	/* Add LSB(2 bytes) of public ip address to cksum */
	cksum += (pub_ip_addr & 0xFFFF);

	/* Add MSB(2 bytes) of public ip address to cksum
		and check for carry forward(CF), if any add it
	*/
	cksum += (pub_ip_addr>>16);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Add public port to cksum and
		 check for carry forward(CF), if any add it */
	cksum += pub_port;
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Calculate the 1's complement of private ip address */
	priv_ip_addr = (~priv_ip_addr);

	/* Add LSB(2 bytes) of private ip address to cksum
		 and check for carry forward(CF), if any add it
	*/
	cksum += (priv_ip_addr & 0xFFFF);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Add MSB(2 bytes) of private ip address to cksum
		 and check for carry forward(CF), if any add
	*/
	cksum += (priv_ip_addr>>16);
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* Calculate the 1's complement of private port */
	priv_port = (~priv_port);

	/* Add public port to cksum and
	 check for carry forward(CF), if any add it */
	cksum += priv_port;
	if (cksum >> 16) {
		cksum = (cksum & 0x0000FFFF);
		cksum += 1;
	}

	/* return the LSB(2 bytes) of checksum */
	ret = (uint16_t)cksum;
	IPADBG("return\n");
	return ret;
}

/* ------------------------------------------
		UTILITY FUNCTIONS END
--------------------------------------------*/

/* ------------------------------------------
	 Main Functions
--------------------------------------------**/

/**
 * ipa_nati_add_ipv4_tbl() - Adds a new IPv4 NAT table
 * @public_ip_addr: [in] public IPv4 address
 * @number_of_entries: [in] number of NAT entries
 * @table_handle: [out] handle of new IPv4 NAT table
 *
 * This function creates new IPv4 NAT table and posts IPv4 NAT init command to HW
 *
 * Returns:	0  On Success, negative on failure
 */
int ipa_nati_add_ipv4_tbl(uint32_t public_ip_addr,
	uint16_t number_of_entries,
	uint32_t *tbl_hdl)
{
	int ret;
	struct ipa_nat_ip4_table_cache* nat_table;

	IPADBG("\n");

	*tbl_hdl = 0;

	if (ipv4_nat_cache.table_cnt >= IPA_NAT_MAX_IP4_TBLS) {
		IPAERR("Can't add addition NAT table. Maximum %d tables allowed\n", IPA_NAT_MAX_IP4_TBLS);
		return -EINVAL;
	}

	if (!ipv4_nat_cache.table_cnt) {
		ipv4_nat_cache.ipa_desc = ipa_descriptor_open();
		if (ipv4_nat_cache.ipa_desc == NULL) {
			IPAERR("failed to open IPA driver file descriptor\n");
			return -EIO;
		}
	}

	nat_table = &ipv4_nat_cache.ip4_tbl[ipv4_nat_cache.table_cnt];
	ret = ipa_nati_create_table(nat_table, public_ip_addr, number_of_entries, ipv4_nat_cache.table_cnt);
	if (ret) {
		IPAERR("unable to create nat table Error: %d\n", ret);
		goto failed_create_table;
	}

	/* Initialize the ipa hw with nat table dimensions */
	ret = ipa_nati_post_ipv4_init_cmd(nat_table, ipv4_nat_cache.table_cnt);
	if (ret) {
		IPAERR("unable to post nat_init command Error %d\n", ret);
		goto failed_post_init_cmd;
	}

	/* store the initial public ip address in the cached pdn table
		this is backward compatible for pre IPAv4 versions, we will always
		use this ip as the single PDN address
	*/
	pdns[0].public_ip = public_ip_addr;
	num_pdns++;

	/* Return table handle */
	++ipv4_nat_cache.table_cnt;
	*tbl_hdl = ipv4_nat_cache.table_cnt;

	IPADBG("return\n");
	return 0;

failed_post_init_cmd:
	ipa_nati_destroy_table(nat_table);
failed_create_table:
	if (!ipv4_nat_cache.table_cnt) {
		ipa_descriptor_close();
	}
	return ret;
}

/**
 * ipa_nati_create_table() - Creates a new IPv4 NAT table
 * @nat_table: [in] IPv4 NAT table
 * @public_ip_addr: [in] public IPv4 address
 * @number_of_entries: [in] number of NAT entries
 * @table_index: [in] the index of the IPv4 NAT table
 *
 * This function creates new IPv4 NAT table:
 * - Initializes table, index table, memory descriptor and table_dma_cmd_helpers structures
 * - Allocates the index expansion table meta data
 * - Allocates, maps and clears the memory for table and index table
 *
 * Returns:	0  On Success, negative on failure
 */
static int ipa_nati_create_table(struct ipa_nat_ip4_table_cache* nat_table,
	uint32_t public_ip_addr, uint16_t number_of_entries, uint8_t table_index)
{
	int ret, size;
	void* base_addr;

#ifdef IPA_ON_R3PC
	uint32_t nat_mem_offset = 0;
#endif

	IPADBG("\n");

	nat_table->public_addr = public_ip_addr;

	ipa_table_init(&nat_table->table, IPA_NAT_TABLE_NAME, sizeof(struct ipa_nat_rule), NULL, &entry_interface);

	ret = ipa_table_calculate_entries_num(&nat_table->table, number_of_entries);
	if (ret) {
		IPAERR("unable to calculate number of entries in nat table %d, while required by user %d\n",
			table_index, number_of_entries);
		return ret;
	}

	/* allocate memory for NAT index expansion table meta data */
	nat_table->index_expn_table_meta = (struct ipa_nat_indx_tbl_meta_info*)calloc(nat_table->table.expn_table_entries,
		sizeof(struct ipa_nat_indx_tbl_meta_info));
	if (nat_table->index_expn_table_meta == NULL) {
		IPAERR("Fail to allocate ipv4 index expansion table meta with size %d\n",
			nat_table->table.expn_table_entries * sizeof(struct ipa_nat_indx_tbl_meta_info));
		return -ENOMEM;
	}

	ipa_table_init(&nat_table->index_table, IPA_NAT_INDEX_TABLE_NAME, sizeof(struct ipa_nat_indx_tbl_rule),
		nat_table->index_expn_table_meta, &index_entry_interface);

	nat_table->index_table.table_entries = nat_table->table.table_entries;
	nat_table->index_table.expn_table_entries = nat_table->table.expn_table_entries;

	size = ipa_table_calculate_size(&nat_table->table);
	size += ipa_table_calculate_size(&nat_table->index_table);
	IPADBG("Nat Base and Index Table size: %d\n", size);

	ipa_mem_descriptor_init(&nat_table->mem_desc, IPA_NAT_DEV_NAME, size, table_index,
		IPA_IOC_ALLOC_NAT_TABLE, IPA_IOC_DEL_NAT_TABLE);

	ret = ipa_mem_descriptor_allocate_memory(&nat_table->mem_desc, ipv4_nat_cache.ipa_desc->fd);
	if (ret) {
		IPAERR("unable to allocate nat memory descriptor Error: %d\n", ret);
		goto bail_meta;
	}

	base_addr = nat_table->mem_desc.base_addr;

#ifdef IPA_ON_R3PC
	ret = ioctl(ipv4_nat_cache.ipa_desc->fd, IPA_IOC_GET_NAT_OFFSET, &nat_mem_offset);
	if (ret) {
		IPAERR("unable to post ant offset cmd Error: %d IPA fd %d\n", ret, ipv4_nat_cache.ipa_desc->fd);
		goto bail_mem_desc;
	}
	base_addr += nat_mem_offset;
#endif

	base_addr = ipa_table_calculate_addresses(&nat_table->table, base_addr);
	ipa_table_calculate_addresses(&nat_table->index_table, base_addr);

	ipa_table_reset(&nat_table->table);
	ipa_table_reset(&nat_table->index_table);

	ipa_nati_create_table_dma_cmd_helpers(nat_table, table_index);

	IPADBG("return\n");
	return 0;

#ifdef IPA_ON_R3PC
bail_mem_desc:
	ipa_mem_descriptor_delete(&nat_table->mem_desc, ipv4_nat_cache.ipa_desc->fd);
#endif
bail_meta:
	free(nat_table->index_expn_table_meta);
	memset(nat_table, 0, sizeof(*nat_table));
	return ret;
}

/**
 * ipa_nati_create_table_dma_cmd_helpers() - Creates dma_cmd_helpers for base and index tables in the
 *                                           received NAT table
 * @nat_table: [in] NAT table
 * @table_indx: [in] The index of the NAT table
 *
 * A DMA command helper helps to generate the DMA command for one specific field change. Each table has 3 different
 * types of field change: update_head, update_entry and delete_head. This function creates the helpers for base and
 * index tables and updates the tables correspondingly.
 */
static void ipa_nati_create_table_dma_cmd_helpers(struct ipa_nat_ip4_table_cache* nat_table, uint8_t table_indx)
{
	IPADBG("\n");

	/* Create helpers for base table*/
	ipa_table_dma_cmd_helper_init(&nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_FLAGS], table_indx,
		IPA_NAT_BASE_TBL, IPA_NAT_EXPN_TBL, nat_table->mem_desc.addr_offset + IPA_NAT_RULE_FLAG_FIELD_OFFSET);
	ipa_table_dma_cmd_helper_init(&nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_NEXT_INDEX], table_indx,
		IPA_NAT_BASE_TBL, IPA_NAT_EXPN_TBL, nat_table->mem_desc.addr_offset + IPA_NAT_RULE_NEXT_FIELD_OFFSET);
	ipa_table_dma_cmd_helper_init(&nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_PROTOCOL], table_indx,
		IPA_NAT_BASE_TBL, IPA_NAT_EXPN_TBL, nat_table->mem_desc.addr_offset + IPA_NAT_RULE_PROTO_FIELD_OFFSET);

	/* Create helpers for index table*/
	ipa_table_dma_cmd_helper_init(
		&nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_ENTRY],
		table_indx,
		IPA_NAT_INDX_TBL,
		IPA_NAT_INDEX_EXPN_TBL,
		nat_table->mem_desc.addr_offset + IPA_NAT_INDEX_RULE_NAT_INDEX_FIELD_OFFSET);
	ipa_table_dma_cmd_helper_init(
		&nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_NEXT_INDEX],
		table_indx,
		IPA_NAT_INDX_TBL,
		IPA_NAT_INDEX_EXPN_TBL,
		nat_table->mem_desc.addr_offset + IPA_NAT_INDEX_RULE_NEXT_FIELD_OFFSET);

	/* Update helpers for base table*/
	nat_table->table.update_head_dma_cmd_helper = &nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_FLAGS];
	nat_table->table.update_entry_dma_cmd_helper = &nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_NEXT_INDEX];
	nat_table->table.delete_head_dma_cmd_helper = &nat_table->table_dma_cmd_helpers[IPA_NAT_TABLE_PROTOCOL];

	/* Update helpers for index table*/
	nat_table->index_table.update_head_dma_cmd_helper = &nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_ENTRY];
	nat_table->index_table.update_entry_dma_cmd_helper =
		&nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_NEXT_INDEX];
	nat_table->index_table.delete_head_dma_cmd_helper =
		&nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_ENTRY];

	IPADBG("return\n");
}

static int ipa_nati_post_ipv4_init_cmd(struct ipa_nat_ip4_table_cache* nat_table, uint8_t tbl_index)
{
	struct ipa_ioc_v4_nat_init cmd;
	int ret;

	IPADBG("\n");

	cmd.tbl_index = tbl_index;

	cmd.ipv4_rules_offset = nat_table->mem_desc.addr_offset;
	cmd.expn_rules_offset = cmd.ipv4_rules_offset + (nat_table->table.table_entries * sizeof(struct ipa_nat_rule));
	cmd.index_offset = cmd.expn_rules_offset + (nat_table->table.expn_table_entries * sizeof(struct ipa_nat_rule));
	cmd.index_expn_offset =
		cmd.index_offset + (nat_table->index_table.table_entries * sizeof(struct ipa_nat_indx_tbl_rule));

	/* Driverr/HW expected base table size to be power^2-1 due to H/W hash calculation */
	cmd.table_entries = nat_table->table.table_entries - 1;
	cmd.expn_table_entries = nat_table->table.expn_table_entries;

	cmd.ip_addr = nat_table->public_addr;

	ret = ioctl(ipv4_nat_cache.ipa_desc->fd, IPA_IOC_V4_INIT_NAT, &cmd);
	if (ret) {
		IPAERR("unable to post init cmd Error: %d IPA fd %d\n", ret, ipv4_nat_cache.ipa_desc->fd);
		return ret;
	}

	IPADBG("Posted IPA_IOC_V4_INIT_NAT to kernel successfully\n");
	return 0;
}

int ipa_nati_del_ipv4_table(uint32_t tbl_hdl)
{
	struct ipa_nat_ip4_table_cache* nat_table = &ipv4_nat_cache.ip4_tbl[tbl_hdl - 1];
	int ret;

	IPADBG("\n");

	if (pthread_mutex_lock(&nat_mutex)) {
		IPAERR("unable to lock the nat mutex\n");
		return -EINVAL;
	}

	if (!nat_table->mem_desc.valid) {
		IPAERR("invalid table handle %d\n", tbl_hdl);
		ret = -EINVAL;
		goto unlock;
	}

	ret = ipa_nati_destroy_table(nat_table);
	if (ret) {
		IPAERR("unable to delete NAT table with handle %d\n", tbl_hdl);
		goto unlock;
	}

	if (!--ipv4_nat_cache.table_cnt) {
		ipa_descriptor_close();
	}

unlock:
	if (pthread_mutex_unlock(&nat_mutex)) {
		IPAERR("unable to unlock the nat mutex\n");
		return (ret) ? ret : -EPERM;
	}

	IPADBG("return\n");
	return ret;
}

static int ipa_nati_destroy_table(struct ipa_nat_ip4_table_cache* nat_table)
{
	int ret;

	IPADBG("\n");

	ret = ipa_mem_descriptor_delete(&nat_table->mem_desc, ipv4_nat_cache.ipa_desc->fd);
	if (ret)
		IPAERR("unable to delete NAT descriptor\n");

	free(nat_table->index_expn_table_meta);
	memset(nat_table, 0, sizeof(*nat_table));

	IPADBG("return\n");
	return ret;
}

int ipa_nati_query_timestamp(uint32_t  tbl_hdl,
	uint32_t  rule_hdl,
	uint32_t  *time_stamp)
{
	int ret;
	struct ipa_nat_ip4_table_cache* nat_table = &ipv4_nat_cache.ip4_tbl[tbl_hdl - 1];
	struct ipa_nat_rule *rule_ptr;

	IPADBG("\n");

	if (pthread_mutex_lock(&nat_mutex)) {
		IPAERR("unable to lock the nat mutex\n");
		return -EINVAL;
	}

	if (!nat_table->mem_desc.valid) {
		IPAERR("invalid table handle %d\n", tbl_hdl);
		ret = -EINVAL;
		goto unlock;
	}

	ret = ipa_table_get_entry(&nat_table->table, rule_hdl, (void**)&rule_ptr, NULL);
	if (ret) {
		IPAERR("unable to retrive the entry with handle=%d in NAT table with handle=%d\n", rule_hdl, tbl_hdl);
		goto unlock;
	}

	*time_stamp = rule_ptr->time_stamp;

unlock:
	if (pthread_mutex_unlock(&nat_mutex)) {
		IPAERR("unable to unlock the nat mutex\n");
		return (ret) ? ret : -EPERM;
	}

	IPADBG("return\n");
	return ret;
}

int ipa_nati_modify_pdn(struct ipa_ioc_nat_pdn_entry *entry)
{
	IPADBG("\n");

	if (entry->public_ip == 0)
		IPADBG("PDN %d public ip will be set  to 0\n", entry->pdn_index);

	if (ioctl(ipv4_nat_cache.ipa_desc->fd, IPA_IOC_NAT_MODIFY_PDN, entry)) {
		IPAERR("unable to call modify pdn icotl\nindex %d, ip 0x%X, src_metdata 0x%X, dst_metadata 0x%X IPA fd %d\n",
			entry->pdn_index, entry->public_ip, entry->src_metadata, entry->dst_metadata, ipv4_nat_cache.ipa_desc->fd);
		return -EIO;
	}

	pdns[entry->pdn_index].public_ip = entry->public_ip;
	pdns[entry->pdn_index].dst_metadata = entry->dst_metadata;
	pdns[entry->pdn_index].src_metadata = entry->src_metadata;

	IPADBG("posted IPA_IOC_NAT_MODIFY_PDN to kernel successfully and stored in cache\n index %d, ip 0x%X, src_metdata 0x%X, dst_metadata 0x%X\n",
		entry->pdn_index, entry->public_ip, entry->src_metadata, entry->dst_metadata);
	return 0;
}

int ipa_nati_get_pdn_index(uint32_t public_ip, uint8_t *pdn_index)
{
	int i = 0;

	for(i = 0; i < (IPA_MAX_PDN_NUM - 1); i++) {
		if(pdns[i].public_ip == public_ip) {
			IPADBG("ip 0x%X matches PDN index %d\n", public_ip, i);
			*pdn_index = i;
			return 0;
		}
	}

	IPAERR("ip 0x%X does not match any PDN\n", public_ip);
	return -EIO;
}

int ipa_nati_alloc_pdn(ipa_nat_pdn_entry *pdn_info, uint8_t *pdn_index)
{
	ipa_nat_pdn_entry zero_test;
	struct ipa_ioc_nat_pdn_entry pdn_data;
	int i, ret;

	IPADBG("alloc PDN  for ip %d\n", pdn_info->public_ip);

	memset(&zero_test, 0, sizeof(zero_test));

	if(num_pdns >= (IPA_MAX_PDN_NUM - 1)) {
		IPAERR("exceeded max num of PDNs\n");
		return -EIO;
	}

	for(i = 0; i < (IPA_MAX_PDN_NUM - 1); i++) {
		if(pdns[i].public_ip == pdn_info->public_ip)
		{
			IPADBG("found the same pdn in index %d\n", i);
			*pdn_index = i;
			if((pdns[i].src_metadata != pdn_info->src_metadata) ||
				(pdns[i].dst_metadata != pdn_info->dst_metadata))
			{
				IPAERR("WARNING: metadata values don't match! [%d, %d], [%d, %d]\n\n",
					pdns[i].src_metadata, pdn_info->src_metadata,
					pdns[i].dst_metadata, pdn_info->dst_metadata);
			}
			return 0;
		}
		if(!memcmp((pdns + i), &zero_test, sizeof(ipa_nat_pdn_entry)))
		{
			IPADBG("found an empty pdn in index %d\n", i);
			break;
		}
	}

	if(i >= (IPA_MAX_PDN_NUM - 1))
	{
		IPAERR("couldn't find an empty entry while num is %d\n",
			num_pdns);
		return -EIO;
	}

	pdn_data.pdn_index = i;
	pdn_data.public_ip = pdn_info->public_ip;
	pdn_data.src_metadata = pdn_info->src_metadata;
	pdn_data.dst_metadata = pdn_info->dst_metadata;

	ret = ipa_nati_modify_pdn(&pdn_data);
	if(!ret)
	{
		num_pdns++;
		*pdn_index = i;
	}

	return ret;
}

int ipa_nati_get_pdn_cnt(void)
{
	return num_pdns;
}

int ipa_nati_dealloc_pdn(uint8_t pdn_index)
{
	ipa_nat_pdn_entry zero_test;
	struct ipa_ioc_nat_pdn_entry pdn_data;
	int ret;

	IPADBG(" trying to deallocate PDN index %d\n", pdn_index);

	if(!num_pdns)
	{
		IPAERR("pdn table is already empty\n");
		return -EIO;
	}

	memset(&zero_test, 0, sizeof(zero_test));

	if(!memcmp((pdns + pdn_index), &zero_test, sizeof(ipa_nat_pdn_entry)))
	{
		IPAERR("pdn entry is a zero entry\n");
		return -EIO;
	}

	IPADBG("PDN in index %d has ip 0x%X\n", pdn_index, pdns[pdn_index].public_ip);

	pdn_data.pdn_index = pdn_index;
	pdn_data.src_metadata = 0;
	pdn_data.dst_metadata = 0;
	pdn_data.public_ip = 0;

	ret = ipa_nati_modify_pdn(&pdn_data);
	if(ret)
	{
		IPAERR("failed modifying PDN\n");
		return -EIO;
	}

	memset((pdns + pdn_index), 0, sizeof(ipa_nat_pdn_entry));
	num_pdns--;
	IPADBG("successfully removed pdn from index %d\n", pdn_index);
	return 0;
}

static int ipa_nati_post_ipv4_dma_cmd(struct ipa_ioc_nat_dma_cmd* cmd)
{
	IPADBG("\n");

	if (ioctl(ipv4_nat_cache.ipa_desc->fd, IPA_IOC_TABLE_DMA_CMD, cmd)) {
		IPAERR("unable to call dma icotl IPA fd %d\n", ipv4_nat_cache.ipa_desc->fd);
		return -EIO;
	}
	IPADBG("posted IPA_IOC_TABLE_DMA_CMD to kernel successfully\n");
	return 0;
}

int ipa_nati_add_ipv4_rule(uint32_t tbl_hdl,
	const ipa_nat_ipv4_rule *clnt_rule,
	uint32_t *rule_hdl)
{
	int ret;
	struct ipa_nat_ip4_table_cache* nat_table = &ipv4_nat_cache.ip4_tbl[tbl_hdl - 1];
	struct ipa_nat_rule* rule;
	uint16_t new_entry_index, new_index_tbl_entry_index;
	uint32_t new_entry_handle;
	struct ipa_ioc_nat_dma_cmd* cmd;

	IPADBG("\n");

	if (clnt_rule->protocol == IPA_NAT_INVALID_PROTO_FIELD_CMP) {
		IPAERR("invalid parameter protocol=%d\n", clnt_rule->protocol);
		return -EINVAL;
	}

	/* verify that the rule's PDN is valid */
	if (clnt_rule->pdn_index >= IPA_MAX_PDN_NUM || pdns[clnt_rule->pdn_index].public_ip == 0) {
		IPAERR("invalid parameters, pdn index %d, public ip = 0x%X\n",
			clnt_rule->pdn_index, pdns[clnt_rule->pdn_index].public_ip);
		return -EINVAL;
	}

	if (pthread_mutex_lock(&nat_mutex)) {
		IPAERR("unable to lock the nat mutex\n");
		return -EINVAL;
	}

	if (!nat_table->mem_desc.valid) {
		IPAERR("invalid table handle %d\n", tbl_hdl);
		ret = -EINVAL;
		goto unlock;
	}

	cmd = (struct ipa_ioc_nat_dma_cmd *)calloc(1, sizeof(struct ipa_ioc_nat_dma_cmd) +
		MAX_DMA_ENTRIES_FOR_ADD * sizeof(struct ipa_ioc_nat_dma_one));
	if (cmd == NULL) {
		IPAERR("unable to allocate memory for Talbe DMA command\n");
		ret = -ENOMEM;
		goto unlock;
	}
	cmd->entries = 0;

	new_entry_index = dst_hash(pdns[clnt_rule->pdn_index].public_ip, clnt_rule->target_ip, clnt_rule->target_port,
		clnt_rule->public_port, clnt_rule->protocol, nat_table->table.table_entries - 1);

	ret = ipa_table_add_entry(&nat_table->table, (void*)clnt_rule, &new_entry_index, &new_entry_handle, cmd);
	if (ret) {
		IPAERR("failed to add a new NAT entry\n");
		goto fail_add_entry;
	}

	new_index_tbl_entry_index = src_hash(clnt_rule->private_ip,
		clnt_rule->private_port,
		clnt_rule->target_ip,
		clnt_rule->target_port,
		clnt_rule->protocol,
		nat_table->table.table_entries - 1);

	ret = ipa_table_add_entry(&nat_table->index_table, &new_entry_index, &new_index_tbl_entry_index, NULL, cmd);
	if (ret) {
		IPAERR("failed to add a new NAT index entry\n");
		goto fail_add_index_entry;
	}

	rule = ipa_table_get_entry_by_index(&nat_table->table, new_entry_index);
	if (rule == NULL) {
		IPAERR("Failed to retrieve the entry in index %d for NAT table with handle=%d\n", new_entry_index, tbl_hdl);
		ret = -EPERM;
		goto bail;
	}

	rule->indx_tbl_entry = new_index_tbl_entry_index;

	IPADBG("new entry:%d, new index entry: %d\n", new_entry_index, new_index_tbl_entry_index);
	ret = ipa_nati_post_ipv4_dma_cmd(cmd);
	if (ret) {
		IPAERR("unable to post dma command\n");
		goto bail;
	}
	free(cmd);

	if (pthread_mutex_unlock(&nat_mutex)) {
		IPAERR("unable to unlock the nat mutex\n");
		return -EPERM;
	}

	*rule_hdl = new_entry_handle;

	IPADBG("return\n");
	return 0;

bail:
	ipa_table_erase_entry(&nat_table->index_table, new_index_tbl_entry_index);
fail_add_index_entry:
	ipa_table_erase_entry(&nat_table->table, new_entry_index);
fail_add_entry:
	free(cmd);
unlock:
	if (pthread_mutex_unlock(&nat_mutex))
		IPAERR("unable to unlock the nat mutex\n");
	return ret;
}

int ipa_nati_del_ipv4_rule(uint32_t tbl_hdl, uint32_t rule_hdl)
{
	struct ipa_nat_ip4_table_cache* nat_table = &ipv4_nat_cache.ip4_tbl[tbl_hdl - 1];
	struct ipa_ioc_nat_dma_cmd* cmd;
	ipa_table_iterator table_iterator, index_table_iterator;
	struct ipa_nat_rule* table_rule;
	struct ipa_nat_indx_tbl_rule* index_table_rule;
	uint16_t index;
	int ret;

	IPADBG("\n");

	if (pthread_mutex_lock(&nat_mutex)) {
		IPAERR("unable to lock the nat mutex\n");
		return -EINVAL;
	}

	if (!nat_table->mem_desc.valid) {
		IPAERR("invalid table handle %d\n", tbl_hdl);
		ret = -EINVAL;
		goto unlock;
	}

	ret = ipa_table_get_entry(&nat_table->table, rule_hdl, (void**)&table_rule, &index);
	if (ret) {
		IPAERR("unable to retrive the entry with handle=%d in NAT table with handle=%d\n", rule_hdl, tbl_hdl);
		goto unlock;
	}

	ret = ipa_table_iterator_init(&table_iterator, &nat_table->table, table_rule, index);
	if (ret) {
		IPAERR("unable to create iterator which points to the entry %d in NAT table with handle=%d\n", index, tbl_hdl);
		goto unlock;
	}

	index = table_rule->indx_tbl_entry;
	index_table_rule = (struct ipa_nat_indx_tbl_rule*)ipa_table_get_entry_by_index(&nat_table->index_table, index);
	if (index_table_rule == NULL) {
		IPAERR("unable to retrieve the entry in index %d in NAT index table with handle=%d\n", index, tbl_hdl);
		ret = -EPERM;
		goto unlock;
	}

	ret = ipa_table_iterator_init(&index_table_iterator, &nat_table->index_table, index_table_rule, index);
	if (ret) {
		IPAERR("unable to create iterator which points to the entry %d in NAT index table with handle=%d\n",
			index, tbl_hdl);
		goto unlock;
	}

	cmd = (struct ipa_ioc_nat_dma_cmd *)calloc(1, sizeof(struct ipa_ioc_nat_dma_cmd) +
		MAX_DMA_ENTRIES_FOR_DEL * sizeof(struct ipa_ioc_nat_dma_one));
	if (cmd == NULL) {
		IPAERR("unable to allocate memory for Talbe DMA command\n");
		ret = -ENOMEM;
		goto unlock;
	}
	cmd->entries = 0;

	ipa_table_create_delete_command(&nat_table->index_table, cmd, &index_table_iterator);

	if (ipa_table_iterator_is_head_with_tail(&index_table_iterator)) {
		ipa_nati_copy_second_index_entry_to_head(nat_table, &index_table_iterator, cmd);

		/* Iterate to the next entry which should be deleted */
		ret = ipa_table_iterator_next(&index_table_iterator, &nat_table->index_table);
		if (ret) {
			IPAERR("unable to move the iterator to the next entry (points to the entry %d in NAT index table)\n",
				index);
			goto fail;
		}
	}

	ipa_table_create_delete_command(&nat_table->table, cmd, &table_iterator);

	ret = ipa_nati_post_ipv4_dma_cmd(cmd);
	if (ret) {
		IPAERR("unable to post dma command\n");
		goto fail;
	}

	if (!ipa_table_iterator_is_head_with_tail(&table_iterator)) {
		/* The entry can be deleted */
		uint8_t is_prev_empty = (table_iterator.prev_entry != NULL &&
			((struct ipa_nat_rule*)table_iterator.prev_entry)->protocol == IPA_NAT_INVALID_PROTO_FIELD_CMP);
		ipa_table_delete_entry(&nat_table->table, &table_iterator, is_prev_empty);
	}

	ipa_table_delete_entry(&nat_table->index_table, &index_table_iterator, FALSE);
	if (index_table_iterator.curr_index >= nat_table->index_table.table_entries)
		nat_table->index_expn_table_meta[index_table_iterator.curr_index - nat_table->index_table.table_entries].
			prev_index = IPA_TABLE_INVALID_ENTRY;

fail:
	free(cmd);

unlock:
	if (pthread_mutex_unlock(&nat_mutex)) {
		IPAERR("unable to unlock the nat mutex\n");
		return (ret) ? ret : -EPERM;
	}

	IPADBG("return\n");
	return ret;
}

static void ipa_nati_copy_second_index_entry_to_head(struct ipa_nat_ip4_table_cache* nat_table,
	ipa_table_iterator* index_table_iterator, struct ipa_ioc_nat_dma_cmd* cmd)
{
	uint16_t index;
	struct ipa_nat_rule* table;
	struct ipa_nat_indx_tbl_rule* index_table_rule = (struct ipa_nat_indx_tbl_rule*)index_table_iterator->next_entry;

	IPADBG("\n");

	/* The DMA command for field tbl_entry already added by the index_table.ipa_table_create_delete_command() */
	ipa_table_dma_cmd_generate(&nat_table->table_dma_cmd_helpers[IPA_NAT_INDEX_TABLE_NEXT_INDEX], FALSE,
		(uint8_t*)index_table_iterator->curr_entry - nat_table->index_table.table_addr, index_table_rule->next_index, cmd);

	/* Change the indx_tbl_entry field in the related table rule */
	if (index_table_rule->tbl_entry < nat_table->table.table_entries) {
		index = index_table_rule->tbl_entry;
		table = (struct ipa_nat_rule*)nat_table->table.table_addr;
	} else {
		index = index_table_rule->tbl_entry - nat_table->table.table_entries;
		table = (struct ipa_nat_rule*)nat_table->table.expn_table_addr;
	}
	table[index].indx_tbl_entry = index_table_iterator->curr_index;

	IPADBG("return\n");
}

static int table_entry_is_valid(void* entry)
{
	struct ipa_nat_rule* rule = (struct ipa_nat_rule*)entry;

	IPADBG("\n");

	return rule->enable;
}

static uint16_t table_entry_get_next_index(void* entry)
{
	uint16_t result;
	struct ipa_nat_rule* rule = (struct ipa_nat_rule*)entry;

	IPADBG("\n");

	result = rule->next_index;

	IPADBG("Next entry of %pK is %d\n", entry, result);
	return result;
}

static uint16_t table_entry_get_prev_index(void* entry, uint16_t entry_index, void* meta, uint16_t base_table_size)
{
	uint16_t result;
	struct ipa_nat_rule* rule = (struct ipa_nat_rule*)entry;

	IPADBG("\n");

	result = rule->prev_index;

	IPADBG("Previous entry of %d is %d\n", entry_index, result);
	return result;
}

static void table_entry_set_prev_index(void* entry, uint16_t entry_index, uint16_t prev_index,
	void* meta, uint16_t base_table_size)
{
	struct ipa_nat_rule* rule = (struct ipa_nat_rule*)entry;

	IPADBG("Previous entry of %d is %d\n", entry_index, prev_index);

	rule->prev_index = prev_index;

	IPADBG("return\n");
}

static int table_entry_copy_from_user(void* entry, void* user_data)
{
	uint32_t pub_ip_addr;
	struct ipa_nat_rule* nat_entry = (struct ipa_nat_rule*)entry;
	const ipa_nat_ipv4_rule* user_rule = (const ipa_nat_ipv4_rule*)user_data;

	IPADBG("\n");

	pub_ip_addr = pdns[user_rule->pdn_index].public_ip;

	nat_entry->private_ip = user_rule->private_ip;
	nat_entry->private_port = user_rule->private_port;
	nat_entry->protocol = user_rule->protocol;
	nat_entry->public_port = user_rule->public_port;
	nat_entry->target_ip = user_rule->target_ip;
	nat_entry->target_port = user_rule->target_port;
	nat_entry->pdn_index = user_rule->pdn_index;

	nat_entry->ip_chksum = ipa_nati_calc_ip_cksum(pub_ip_addr, user_rule->private_ip);

	if (IPPROTO_TCP == nat_entry->protocol || IPPROTO_UDP == nat_entry->protocol) {
		nat_entry->tcp_udp_chksum = ipa_nati_calc_tcp_udp_cksum(
			pub_ip_addr,
			user_rule->public_port,
			user_rule->private_ip,
			user_rule->private_port);
	}

	IPADBG("return\n");
	return 0;
}

static int table_entry_head_insert(void* entry, void* user_data, uint16_t* dma_command_data)
{
	int ret;

	IPADBG("\n");

	ret = table_entry_copy_from_user(entry, user_data);
	if (ret) {
		IPAERR("unable to copy from user a new entry\n");
		return ret;
	}

	*dma_command_data = 0;
	((ipa_nat_flags*)dma_command_data)->enable = IPA_NAT_FLAG_ENABLE_BIT;

	IPADBG("return\n");
	return 0;
}

static int table_entry_tail_insert(void* entry, void* user_data)
{
	int ret;

	IPADBG("\n");

	ret = table_entry_copy_from_user(entry, user_data);
	if (ret) {
		IPAERR("unable to copy from user a new entry\n");
		return ret;
	}

	((struct ipa_nat_rule*)entry)->enable = IPA_NAT_FLAG_ENABLE_BIT;

	IPADBG("return\n");
	return 0;
}

static uint16_t table_entry_get_delete_head_dma_command_data(void* head, void* next_entry)
{
	IPADBG("\n");
	return IPA_NAT_INVALID_PROTO_FIELD_VALUE;
}

static int index_table_entry_is_valid(void* entry)
{
	struct ipa_nat_indx_tbl_rule* rule = (struct ipa_nat_indx_tbl_rule*)entry;

	IPADBG("\n");

	return (rule->tbl_entry)? 1 : 0;
}

static uint16_t index_table_entry_get_next_index(void* entry)
{
	uint16_t result;
	struct ipa_nat_indx_tbl_rule* rule = (struct ipa_nat_indx_tbl_rule*)entry;

	IPADBG("\n");

	result = rule->next_index;

	IPADBG("Next entry of %pK is %d\n", entry, result);
	return result;
}

static uint16_t index_table_entry_get_prev_index(void* entry, uint16_t entry_index,
	void* meta, uint16_t base_table_size)
{
	uint16_t result = 0;
	struct ipa_nat_indx_tbl_meta_info* index_expn_table_meta = (struct ipa_nat_indx_tbl_meta_info*)meta;

	IPADBG("\n");

	if (entry_index >= base_table_size)
		result = index_expn_table_meta[entry_index - base_table_size].prev_index;

	IPADBG("Previous entry of %d is %d\n", entry_index, result);

	return result;
}

static void index_table_entry_set_prev_index(void* entry, uint16_t entry_index, uint16_t prev_index,
	void* meta, uint16_t base_table_size)
{
	struct ipa_nat_indx_tbl_meta_info* index_expn_table_meta = (struct ipa_nat_indx_tbl_meta_info*)meta;

	IPADBG("Previous entry of %d is %d\n", entry_index, prev_index);

	if (entry_index >= base_table_size) {
		index_expn_table_meta[entry_index - base_table_size].prev_index = prev_index;
	} else if (prev_index != IPA_TABLE_INVALID_ENTRY) {
		IPAERR("Base table entry %d can't has prev entry %d, but only %d",
			entry_index, prev_index, IPA_TABLE_INVALID_ENTRY);
	}

	IPADBG("return\n");
}

static int index_table_entry_head_insert(void* entry, void* user_data, uint16_t* dma_command_data)
{
	IPADBG("\n");
	*dma_command_data = *((uint16_t*)user_data);
	IPADBG("return\n");
	return 0;
}

static int index_table_entry_tail_insert(void* entry, void* user_data)
{
	IPADBG("\n");
	((struct ipa_nat_indx_tbl_rule*)entry)->tbl_entry = *((uint16_t*)user_data);
	IPADBG("return\n");
	return 0;
}

static uint16_t index_table_entry_get_delete_head_dma_command_data(void* head, void* next_entry)
{
	uint16_t result;
	struct ipa_nat_indx_tbl_rule* rule = (struct ipa_nat_indx_tbl_rule*)next_entry;

	IPADBG("\n");

	result = rule->tbl_entry;

	IPADBG("return\n");
	return result;
}

/* ========================================================
						Debug functions
	 ========================================================*/

void ipa_nat_dump_ipv4_table(uint32_t tbl_hdl)
{
	int table_empty = TRUE;
	uint16_t i, rule_id;
	struct ipa_nat_ip4_table_cache* nat_table;
	struct ipa_nat_indx_tbl_meta_info zero_entry = {0};

	if (tbl_hdl == IPA_TABLE_INVALID_ENTRY || tbl_hdl > IPA_NAT_MAX_IP4_TBLS) {
		IPAERR("invalid parameter table handle %d\n", tbl_hdl);
		return;
	}

	if (pthread_mutex_lock(&nat_mutex)) {
		IPAERR("unable to lock the nat mutex\n");
		return;
	}

	nat_table = &ipv4_nat_cache.ip4_tbl[tbl_hdl - 1];
	if (!nat_table->mem_desc.valid) {
		IPAERR("invalid table handle %d\n", tbl_hdl);
		goto unlock;
	}

	/* Prevents interleaving with kernel printouts. Flush doesn't help. */
	sleep(1);
	ipa_read_debug_info(IPA_NAT_DEBUG_FILE_PATH);

	printf("ipaNatTable Expansion Index Table Meta Data:\n");
	for (i = 0, rule_id = nat_table->table.table_entries; i < nat_table->table.expn_table_entries; ++i, ++rule_id) {
		if (!memcmp(&zero_entry, &nat_table->index_expn_table_meta[i], sizeof(zero_entry)))
			continue;

		table_empty = FALSE;
		printf("\tEntry_Index=%d\n\t\tPrev_Index=%d\n", rule_id, nat_table->index_expn_table_meta[i].prev_index);
	}

	if (table_empty)
		printf("\tEmpty\n");

	printf("\n");
	sleep(1);

unlock:
	if (pthread_mutex_unlock(&nat_mutex)) {
		IPAERR("unable to unlock the nat mutex\n");
	}
}

