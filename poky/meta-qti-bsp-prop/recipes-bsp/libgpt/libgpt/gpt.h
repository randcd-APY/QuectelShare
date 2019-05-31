/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */

#ifndef _LIBGPT_H_
#define _LIBGPT_H_

#include <uchar.h>

#define GPT_NAME_MAX (36)
#define GPT_HDR_SIGNATURE      (0x5452415020494645ULL)
#define GPT_HDR_SZ   (92)
#define GPT_PARTITION_ENTRY_SZ (128)

typedef struct {
    unsigned char data[16];
} guid_t;

typedef struct gpt_hdr {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t current_lba;
    uint64_t backup_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    guid_t   disk_guid;
    uint64_t partition_entries_start_lba;
    uint32_t number_of_partitions;
    uint32_t partition_entry_size;
    uint32_t partition_array_crc32;
    uint8_t  reserved_padding[420];
} __attribute__((packed)) gpt_hdr_t;

typedef struct gpt_partition_entry {
    guid_t   partition_type_guid;
    guid_t   unique_partition_guid;
    uint64_t first_lba;
    uint64_t last_lba;
    uint64_t attribute_flags;
    char16_t  partition_name[GPT_NAME_MAX];
} __attribute__((packed)) gpt_partition_entry_t;

typedef int gpt_header_id_t;

#define GPT_PRIMARY_HEADER_ID      0
#define GPT_SECONDARY_HEADER_ID    1
#define GPT_HEADER_ID_END          2


/*
 * libgpt_gptSanityCheck checks sanity of the GPT.
 *
 * Parameters
 *    gpt_id[IN]:         Identifier for GPT to be sanity checked.
 *
 * Return Value
 *    0 is returned in case of success, i.e., when GPT passes sanity check.
 *    -1 when an error is encountered, errno is set accordingly.
 */
int libgpt_gptSanityCheck(gpt_header_id_t gpt_id);


/*
 * Parameters
 *    device [IN]:         Block device where the partition should be searched.
 *    partition_name [IN]: Partition for ehich attribute is to be read.
 *    entry [IN/OUT]:      Must be alocated by Caller.
 *                         The allocated memory is filled with partition entry data.
 *
 * Return Value
 *    -1 when an error is encountered, errno is set accordingly.
 *    In case of success 'index' of the parttion entry in partition array is returned.
 */
int libgpt_getPartitionEntry(const char* device, const char16_t* partition_name,
                                                 gpt_partition_entry_t *entry);


/*
 * libgpt_getAttribute gets the attribute for the indicated partition.
 *
 * Parameters
 *    device [IN]:         Block device where the partition should be searched.
 *    partition_name [IN]: Partition for ehich attribute is to be read.
 *    attribs[IN/OUT]:     Must be alocated by Caller.
 *                         64 bit Attribute flag is copied in this.
 *
 * Return Value
 *    -1 when an error is encountered, errno is set accordingly.
 *    On success 'index' of the parttion entry in partition array is returned
 *    and attribs is filled.
 *
 */
int libgpt_getAttribute(const char* device, const char16_t* partition_name,
                                            uint64_t *attribs);


/*
 * libgpt_setAttribute sets the attribute for the indicated partition.
 *
 * Parameters
 *    device [IN]:         Block device where the partition should be searched.
 *    partition_name [IN]: Partition for ehich attribute is to be updated.
 *    attribs[IN]:         64 bit Attribute flag to be set for the partition
 *
 * Return Value
 *    -1 when an error is encountered, errno is set accordingly.
 *    On success 'index' of the parttion entry in partition array is returned
 *
 */
int libgpt_setAttribute(const char* device, const char16_t* partition_name,
                                            uint64_t attribs);

/*
 * libgpt_setTypeGUID sets the type GUID for the indicated partition.
 *
 * Parameters
 *    device [IN]:         Block device where the partition should be searched.
 *    partition_name [IN]: Partition for ehich attribute is to be updated.
 *    type_guid[IN]:       GUID to be set for the partition
 *
 * Return Value
 *    -1 when an error is encountered, errno is set accordingly.
 *    On success 'index' of the parttion entry in partition array is returned
 *
 */
int libgpt_setTypeGUID(const char* device, const char16_t* partition_name,
                                           guid_t *type_guid);

void libgpt_dbg_prints(int do_prints);
#endif //_LIBGPT_H_
