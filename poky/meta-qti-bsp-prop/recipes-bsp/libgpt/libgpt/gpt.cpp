/*
 * ---------------------------------------------------------------------------
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */

/*
 *                        GUID Partition Table
 *
 *         -------------------------------------------------
 * LBA 0   |                  Protective MBR               |
 *         -------------------------------------------------
 * LBA 1   |                Primary GPT Header             |
 *         -------------------------------------------------
 * LBA 2   |  Entry 1  | Entry 2   | Entry 3   | Entry 4   |
 * LBA 3   |                                               |
 *         .                                               .
 *         .                                               .
 *         .                                               .
 * LBA 33  | Entry 125 | Entry 126 | Entry 127 | Entry 128 |
 *         -------------------------------------------------
 * LBA 34  .                                               .
 *         .                                               .
 *         .                                               .
 *         .                   Partitions                  .
 *         .                                               .
 *         .                                               .
 *         .                                               .
 * LBA -34 .                                               .
 *         -------------------------------------------------
 * LBA -33 |  Entry 1  | Entry 2   | Entry 3   | Entry 4   |
 *         .                                               .
 *         .                                               .
 * LBA  -2 | Entry 125 | Entry 126 | Entry 127 | Entry 128 |
 *         -------------------------------------------------
 * LBA  -1 |               Secondary GPT Header            |
 *         -------------------------------------------------
 *
 */
#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <stdlib.h>

#include "gpt.h"

static int _dbg_prints = 0;

#ifdef DBG_PRINTS
#define DBG_ERR(format, ...)    if(_dbg_prints)fprintf(stderr, format, __VA_ARGS__)
#else
#define DBG_ERR(format, ...)
#endif

#define PROTECTIVE_MBR_LBA_SIZE (512)

#define GPT_HEADER_SIGNATURE    (0x5452415020494645ULL)

static int strcmp16(const char16_t *s1, const char16_t *s2)
{
   if(NULL == s1 && NULL != s2)
      return -1;
   if(NULL != s1 && NULL == s2)
      return 1;
   if(NULL == s1 && NULL == s2)
      return 0;

   while((*s1 == *s2) && *s1 && *s2) {
      s1++;
      s2++;
   }

   return (*s1-*s2);
}

/*
 * A8h reflected is 15h, i.e. 10101000 <--> 00010101
 */
static int reflect(int data, int len)
{
    int ref = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (data & 0x1) {
            ref |= (1 << ((len - 1) - i));
        }
        data = (data >> 1);
    }
    return ref;
}

static uint32_t calculate_crc32(void *buf, uint32_t len)
{
    uint32_t i, j;
    uint32_t byte_length = 8;       /*length of unit (i.e. byte) */
    int msb = 0;
    int polynomial = 0x04C11DB7;    /* IEEE 32bit polynomial */
    unsigned int regs = 0xFFFFFFFF; /* init to all ones */
    int regs_mask = 0xFFFFFFFF;     /* ensure only 32 bit answer */
    int regs_msb = 0;
    unsigned int reflected_regs;
    for (i = 0; i < len; i++) {
        int data_byte = *((uint8_t *)buf + i);
        data_byte = reflect(data_byte, 8);
        for (j = 0; j < byte_length; j++) {
            msb = data_byte >> (byte_length - 1); /* get MSB */
            msb &= 1;                             /* ensure just 1 bit */
            regs_msb = (regs >> 31) & 1;          /* MSB of regs */
            regs = regs << 1;                     /* shift regs for CRC-CCITT */
            if (regs_msb ^ msb) {                 /* MSB is a 1 */
                regs = regs ^ polynomial;         /* XOR with generator poly */
            }
            regs = regs & regs_mask;              /* Mask off excess upper bits */
            data_byte <<= 1;                      /* get to next bit */
        }
    }
    regs = regs & regs_mask;
    reflected_regs = reflect(regs, 32) ^ 0xFFFFFFFF;
    return reflected_regs;
}

static uint64_t get_lba_offset(uint64_t lba)
{
    return lba * 512;
}

static int64_t get_disk_num_lbas(const char* device)
{
   int fd = open(device, O_RDONLY);
   uint64_t size_bytes = 0;

   if (-1 == fd) {
      DBG_ERR("%s: open() failed for %s (%s)\n",__func__, device, strerror(errno));
      return -1;
   }

   int ret = ioctl(fd, BLKGETSIZE64, &size_bytes);

   close(fd);

   if (-1 == ret) {
      DBG_ERR("%s: ioctl(BLKGETSIZE64) failed for %s (%s)\n",
               __func__, device, strerror(errno));
      return -1;
   }

   return (size_bytes/512);
}

static int open_block_dev(const char* device, int is_write)
{
    int oflag = is_write ? O_RDWR : O_RDONLY;
    int fd = open (device, oflag);

    if (-1 == fd) {
        DBG_ERR("%s: open() failed for %s (%s)\n",__func__, device, strerror(errno));
        return -1;
    }

    return fd;
}

static ssize_t read_block_dev(const char* device, off64_t offset,
                                                  size_t size,
                                                  void *buf)
{
    ssize_t read_bytes;
    ssize_t ret = -1;
    int fd = open_block_dev(device, 0);

    if (fd < 0)
        return -1;

    if (-1 == lseek64(fd, offset, SEEK_SET)) {
        close(fd);
        return -1;
    }

    do {
        read_bytes = read(fd, buf, size);
    } while (-1 == read_bytes && EINTR == errno);

    if (-1 == read_bytes)
        ret = -1;
    else
        ret = read_bytes;

    close(fd);
    return ret;
}

static ssize_t write_block_dev(const char* device, off64_t offset,
                               size_t size, void *buf)
{
    ssize_t written_bytes;
    ssize_t ret = 0;
    int fd = open_block_dev(device, 1);

    if (-1 == fd)
        return -1;

    if (-1 == lseek64(fd, offset, SEEK_SET)) {
        close(fd);
        return -1;
    }

    do {
        written_bytes = write(fd, buf, size);
    } while(-1 == written_bytes && EINTR == errno);

    if (-1 == written_bytes)
        ret = -1;
    else
        ret = written_bytes;

    close(fd);
    return ret;
}

static int write_gpt_part_entry(const char *device, gpt_header_id_t hdr_id,
                                gpt_partition_entry_t *entry, unsigned index)
{
   uint64_t gpt_hdr_start_offset, gpt_parts_entry_start_offset;
   gpt_partition_entry_t *partition_entry_array = NULL;
   gpt_hdr_t gpt_header;
   gpt_partition_entry_t *part_entry;
   int ret = -1;

   /* Set offsets as per the valid GPT */
   if (hdr_id == GPT_PRIMARY_HEADER_ID) {
      gpt_hdr_start_offset = get_lba_offset(1);
      gpt_parts_entry_start_offset = get_lba_offset(2);
   } else {
      int64_t n_lbas = get_disk_num_lbas(device);
      if(n_lbas == -1)
         return -1;
      gpt_hdr_start_offset = get_lba_offset(n_lbas - 1);
      gpt_parts_entry_start_offset = get_lba_offset(n_lbas - 33);
   }

   /* Read Header */
   ret = read_block_dev(device, gpt_hdr_start_offset, sizeof(gpt_hdr_t), &gpt_header);

   if(ret == -1) {
      DBG_ERR("libgpt: %s: header read failed %s\n",__func__,strerror(errno));
      return -1;
   }

   /* Zero out the header_crc32 */
   gpt_header.header_crc32 = 0;
   /* Zero out the partition_array_crc32 */
   gpt_header.partition_array_crc32 = 0;

   /* Allocate for entries */
   partition_entry_array =
         (gpt_partition_entry_t *) malloc(sizeof(gpt_partition_entry_t) *
                                          gpt_header.number_of_partitions);

   if(partition_entry_array == NULL) {
      DBG_ERR("libgpt: %s: malloc() error\n",__func__);
      return -1;
   }

   /* Read all partition entries from GPT into the allocated array */
   ret = read_block_dev(device, gpt_parts_entry_start_offset,
                        sizeof(gpt_partition_entry_t) * gpt_header.number_of_partitions,
                        partition_entry_array);

   if(ret == -1) {
      DBG_ERR("libgpt: %s: partition array read failed %s\n",
               __func__,strerror(errno));
      free(partition_entry_array);
      return -1;
   }

   /* memcpy created entry at the desired index */
   part_entry = partition_entry_array + index;
   memcpy(part_entry, entry, sizeof(gpt_partition_entry_t));

   /* Populate partition_array_crc32 using the partition array */
   gpt_header.partition_array_crc32 =
            calculate_crc32(partition_entry_array,
                            gpt_header.number_of_partitions *
                            sizeof(gpt_partition_entry_t));

   /* Populate header_crc32 from the gpt_hdr */
   gpt_header.header_crc32 = calculate_crc32(&gpt_header,
                                             gpt_header.header_size);

   ret = write_block_dev(device, gpt_parts_entry_start_offset,
                         sizeof(gpt_partition_entry_t) * gpt_header.number_of_partitions,
                         partition_entry_array);

   free(partition_entry_array);

   if (ret == -1) {
      DBG_ERR("libgpt: %s: partition array write failed for GPT %d (%s)\n",
               __func__, hdr_id, strerror(errno));
      return -1;
   }

   ret = write_block_dev(device, gpt_hdr_start_offset,
                         sizeof(gpt_hdr_t), &gpt_header);

   if (ret == -1) {
      DBG_ERR("libgpt: %s: GPT header write failed for GPT %d (%s)\n",
               __func__, hdr_id, strerror(errno));
      return -1;
   }

   return 0;
}

int libgpt_gptSanityCheck(const char* device, gpt_header_id_t gpt_id)
{
   uint64_t gpt_hdr_start_offset, gpt_parts_entry_start_offset;
   gpt_hdr_t gpt_header;
   int ret = -1;
   gpt_partition_entry_t *partition_entry_array = NULL;

   /* Set offsets as per the valid GPT. */
   if (gpt_id == GPT_PRIMARY_HEADER_ID) {
      gpt_hdr_start_offset = get_lba_offset(1);
      gpt_parts_entry_start_offset = get_lba_offset(2);
   } else if (gpt_id == GPT_SECONDARY_HEADER_ID) {
      int64_t n_lbas = get_disk_num_lbas(device);
      if(n_lbas == -1)
         return -1;
      gpt_hdr_start_offset = get_lba_offset(n_lbas - 1);
      gpt_parts_entry_start_offset = get_lba_offset(n_lbas - 33);
   } else {
      DBG_ERR("libgpt: %s: invalid GPT header ID\n", __func__);
      errno = EINVAL;
      return -1;
   }

   /* Read the GPT Header. */
   ret = read_block_dev(device, gpt_hdr_start_offset, sizeof(gpt_hdr_t), &gpt_header);

   if((ret != -1) && (gpt_header.signature == GPT_HDR_SIGNATURE) &&
      (gpt_header.header_size == GPT_HDR_SZ) &&
      (gpt_header.partition_entry_size == GPT_PARTITION_ENTRY_SZ)) {

      /* Allocate for entries */
      partition_entry_array = (gpt_partition_entry_t *)
                               malloc(sizeof(gpt_partition_entry_t) *
                                      gpt_header.number_of_partitions);

      if(partition_entry_array == NULL) {
         DBG_ERR("libgpt: %s: malloc() error\n",__func__);
         return -1;
      }

      /* Read all partition entries from GPT into the allocated array */
      ret = read_block_dev(device, gpt_parts_entry_start_offset,
                           sizeof(gpt_partition_entry_t) * gpt_header.number_of_partitions,
                           partition_entry_array);

      if(ret == -1) {
         DBG_ERR("libgpt: %s: partition array read failed %s\n",
                  __func__, strerror(errno));
         free(partition_entry_array);
         return -1;
      }

      /* Calculate and compare CRC32 */
      if (gpt_header.partition_array_crc32 ==
             calculate_crc32(partition_entry_array,
                             gpt_header.number_of_partitions *
                             sizeof(gpt_partition_entry_t)))
         ret = 0;
      else
         ret = -1;
   }
   return ret;
}

int libgpt_getPartitionEntry(const char* device, const char16_t* partition_name,
                                                 gpt_partition_entry_t *entry)
{
   int ret = -1;
   gpt_header_id_t hdr_id;
   gpt_hdr_t gpt_header;
   gpt_partition_entry_t partition_entry;
   uint64_t gpt_hdr_start_offset, gpt_parts_entry_start_offset;
   uint32_t partition_iter = 0;

   if((device == NULL) || (partition_name == NULL)) {
      DBG_ERR("%s: invalid parameter\n", __func__);
      errno = EINVAL;
      return -1;
   }

   /*
      Determine the usable GPT header.
      First check primary GPT, if primary GPT is invalid use secondary GPT.
    */
   for (hdr_id = GPT_PRIMARY_HEADER_ID; hdr_id < GPT_HEADER_ID_END; hdr_id++) {
      if(libgpt_gptSanityCheck(device, hdr_id) == 0)
         break;
   }

   if (hdr_id >= GPT_HEADER_ID_END) {
      DBG_ERR("libgpt: %s no valid GPT found for %s.\n", __func__, device);
      errno = EAGAIN;
      return -1;
   }

   /* Set offsets as per the valid GPT. */
   if (hdr_id == GPT_PRIMARY_HEADER_ID) {
      gpt_hdr_start_offset = get_lba_offset(1);
      gpt_parts_entry_start_offset = get_lba_offset(2);
   } else if (hdr_id == GPT_SECONDARY_HEADER_ID) {
      int64_t n_lbas = get_disk_num_lbas(device);
      if(n_lbas == -1)
         return -1;
      gpt_hdr_start_offset = get_lba_offset(n_lbas - 1);
      gpt_parts_entry_start_offset = get_lba_offset(n_lbas - 33);
   } else {
      DBG_ERR("libgpt: %s: invalid GPT header ID\n", __func__);
      errno = EINVAL;
      return -1;
   }

   /* Read the GPT Header. */
   ret = read_block_dev(device, gpt_hdr_start_offset, sizeof(gpt_hdr_t), &gpt_header);

   if (ret != -1) {
      /* Iterate over each partition entry till a match is found. */
      for(partition_iter = 0; partition_iter < gpt_header.number_of_partitions;
                              partition_iter++) {
         /* Read a partition entry. */
         ret = read_block_dev(device,
                  gpt_parts_entry_start_offset + (sizeof(gpt_partition_entry_t) * partition_iter),
                  sizeof(gpt_partition_entry_t), &partition_entry);

         if (ret == -1) {
            DBG_ERR("libgpt: %s read error\n", __func__);
            return -1;
         }

         /* Return the partition index if a match is found. */
         if(!strcmp16(partition_name, partition_entry.partition_name)) {
            ret = partition_iter;
            if (entry)
               memcpy(entry, &partition_entry, sizeof(gpt_partition_entry_t));
            break;
         }
      }

      /*If all partitions were iterated over, without finding a match, then return -1. */
      if (partition_iter >= gpt_header.number_of_partitions) {
         errno = ENOENT;
         ret = -1;
      }
   } else {
      ret = -1;
   }

   return ret;
}

int libgpt_getAttribute(const char* device, const char16_t* partition_name,
                                            uint64_t *attribs)
{
   int ret = -1;
   gpt_partition_entry_t entry;

   if (attribs == NULL) {
      errno = EINVAL;
      return -1;
   }

   ret = libgpt_getPartitionEntry(device, partition_name, &entry);

   if(ret >= 0) {
      memcpy(attribs, &entry.attribute_flags, sizeof(uint64_t));
   } else {
      ret = -1;
   }

   return ret;
}

int libgpt_setAttribute(const char* device, const char16_t* partition_name,
                                            uint64_t attribs)
{
   int ret = -1;
   gpt_partition_entry_t entry;
   gpt_header_id_t GPT_write_sequence[2] = {0};

   ret = libgpt_getPartitionEntry(device, partition_name, &entry);

   if(ret >= 0) {
      entry.attribute_flags = attribs;
   } else {
      return -1;
   }

   /*
      If Primary GPT is valid, then update the secondary GPT first.
      Else, vice versa.
    */
   if(libgpt_gptSanityCheck(device, GPT_PRIMARY_HEADER_ID) == 0) {
      GPT_write_sequence[0] = GPT_SECONDARY_HEADER_ID;
      GPT_write_sequence[1] = GPT_PRIMARY_HEADER_ID;
   } else {
      GPT_write_sequence[0] = GPT_PRIMARY_HEADER_ID;
      GPT_write_sequence[1] = GPT_SECONDARY_HEADER_ID;
   }

   DBG_ERR("libgpt: %s: GPT update order for %s. First: %d, second: %d\n",
            __func__, device, GPT_write_sequence[0], GPT_write_sequence[1]);

   /* Update one GPT */
   if(write_gpt_part_entry(device, GPT_write_sequence[0], &entry, ret) != 0) {
      DBG_ERR("libgpt: %s: GPT update failed for %d\n",
               __func__, GPT_write_sequence[0]);
      return -1;
   }

   DBG_ERR("libgpt: %s: GPT update successful for %d\n",
            __func__, GPT_write_sequence[0]);

   /* Update the other GPT */
   if(write_gpt_part_entry(device, GPT_write_sequence[1], &entry, ret) != 0) {
      DBG_ERR("libgpt: %s: GPT update failed for %d\n",
               __func__, GPT_write_sequence[1]);
      return -1;
   }

   DBG_ERR("libgpt: %s: both GPT updated\n", __func__);

   return 0;
}

int libgpt_setTypeGUID(const char* device, const char16_t* partition_name,
                                           guid_t *type_guid)
{
   int ret = -1;
   gpt_partition_entry_t entry;
   gpt_header_id_t GPT_write_sequence[2] = {0};

   ret = libgpt_getPartitionEntry(device, partition_name, &entry);

   if(ret >= 0) {
      memcpy(&entry.partition_type_guid, type_guid, sizeof(guid_t));
   } else {
      return -1;
   }

   /*
      If Primary GPT is valid, then update the secondary GPT first.
      Else, vice versa.
    */
   if(libgpt_gptSanityCheck(device, GPT_PRIMARY_HEADER_ID) == 0) {
      GPT_write_sequence[0] = GPT_SECONDARY_HEADER_ID;
      GPT_write_sequence[1] = GPT_PRIMARY_HEADER_ID;
   } else {
      GPT_write_sequence[0] = GPT_PRIMARY_HEADER_ID;
      GPT_write_sequence[1] = GPT_SECONDARY_HEADER_ID;
   }

   DBG_ERR("libgpt: %s: GPT update order for %s. First: %d, second: %d\n",
            __func__, device, GPT_write_sequence[0], GPT_write_sequence[1]);

   /* Update one GPT */
   if(write_gpt_part_entry(device, GPT_write_sequence[0], &entry, ret) != 0) {
      DBG_ERR("libgpt: %s: GPT update failed for %d\n",
               __func__, GPT_write_sequence[0]);
      return -1;
   }

   DBG_ERR("libgpt: %s: GPT update successful for %d\n",
            __func__, GPT_write_sequence[0]);

   /* Update the other GPT */
   if(write_gpt_part_entry(device, GPT_write_sequence[1], &entry, ret) != 0) {
      DBG_ERR("libgpt: %s: GPT update failed for %d\n",
               __func__, GPT_write_sequence[1]);
      return -1;
   }

   DBG_ERR("libgpt: %s: both GPT updated\n", __func__);

   return 0;
}

void libgpt_dbg_prints(int do_prints)
{
   if(do_prints)
      _dbg_prints = 1;
   else
      _dbg_prints = 0;
}
