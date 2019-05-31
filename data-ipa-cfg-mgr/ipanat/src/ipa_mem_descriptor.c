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
#include "ipa_mem_descriptor.h"
#include "ipa_nat_utils.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

#define IPA_DEV_DIR "/dev/"

#ifdef IPA_ON_R3PC
#define IPA_DEVICE_MMAP_MEM_SIZE (2 * 1024UL * 1024UL - 1)
#endif

static int AllocateMemory(ipa_mem_descriptor* desc, int ipa_fd);
static int MapMemory(ipa_mem_descriptor* desc);
static int DeallocateMemory(ipa_mem_descriptor* desc, int ipa_fd);

void ipa_mem_descriptor_init(ipa_mem_descriptor* desc, const char* device_name, int size,
	uint8_t table_index, unsigned long allocate_ioctl_num, unsigned long delete_ioctl_num)
{
	IPADBG("\n");

	strlcpy(desc->name, device_name, IPA_RESOURCE_NAME_MAX);
	desc->size = size;
	desc->table_index = table_index;
	desc->allocate_ioctl_num = allocate_ioctl_num;
	desc->delete_ioctl_num = delete_ioctl_num;

	IPADBG("return\n");
}

int ipa_mem_descriptor_allocate_memory(ipa_mem_descriptor* desc, int ipa_fd)
{
	int ret;

	IPADBG("\n");

	ret = AllocateMemory(desc, ipa_fd);
	if (ret)
	{
		IPAERR("unable to allocate %s\n", desc->name);
		return ret;
	}

	ret = MapMemory(desc);
	if (ret)
	{
		IPAERR("unable to map %s\n", desc->name);
		DeallocateMemory(desc, ipa_fd);
		return ret;
	}

	desc->valid = TRUE;
	IPADBG("return\n");
	return 0;
}

int ipa_mem_descriptor_delete(ipa_mem_descriptor* desc, int ipa_fd)
{
	int ret;

	IPADBG("\n");
	if (!desc->valid)
	{
		IPAERR("invalid desc handle passed\n");
		return -EINVAL;
	}

	desc->valid = FALSE;

#ifndef IPA_ON_R3PC
	munmap(desc->base_addr, desc->size);
#else
	munmap(desc->base_addr, IPA_DEVICE_MMAP_MEM_SIZE);
#endif

	ret = DeallocateMemory(desc, ipa_fd);

	IPADBG("return\n");
	return ret;
}

static int AllocateMemory(ipa_mem_descriptor* desc, int ipa_fd)
{
	int ret;
	struct ipa_ioc_nat_ipv6ct_table_alloc cmd;

	IPADBG("\n");

	memset(&cmd, 0, sizeof(cmd));
	cmd.size = desc->size;

	ret = ioctl(ipa_fd, desc->allocate_ioctl_num, &cmd);
	if (ret)
	{
		IPAERR("unable to post %s allocate table command. Error %d IPA fd %d\n", desc->name, ret, ipa_fd);
		return ret;
	}

	desc->addr_offset = cmd.offset;

	IPADBG("The memory desc for %s allocated successfully\n", desc->name);
	return 0;
}

static int MapMemory(ipa_mem_descriptor* desc)
{
	int ret = 0, device_fd;
	char device_full_path[IPA_RESOURCE_NAME_MAX];
	size_t ipa_dev_dir_path_len;

	IPADBG("\n");

	ipa_dev_dir_path_len = strlcpy(device_full_path, IPA_DEV_DIR, IPA_RESOURCE_NAME_MAX);
	if (ipa_dev_dir_path_len >= IPA_RESOURCE_NAME_MAX)
	{
		IPAERR("unable to copy a string with size %d to buffer with size %d\n",
			ipa_dev_dir_path_len, IPA_RESOURCE_NAME_MAX);
		return -EINVAL;
	}

	strlcpy(device_full_path + ipa_dev_dir_path_len, desc->name, IPA_RESOURCE_NAME_MAX - ipa_dev_dir_path_len);

	device_fd = open(device_full_path, O_RDWR);
	if (device_fd < 0)
	{
		IPAERR("unable to open the desc %s in path %s. Error:%d\n", desc->name, device_full_path, device_fd);
		return -EIO;
	}

#ifndef IPA_ON_R3PC
	desc->base_addr = (void*)mmap(NULL, desc->size, PROT_READ | PROT_WRITE, MAP_SHARED, device_fd, 0);
#else
	IPADBG("user space r3pc\n");
	desc->base_addr =
		(void *)mmap((caddr_t)0, IPA_DEVICE_MMAP_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, device_fd, 0);
#endif
	if (MAP_FAILED == desc->base_addr)
	{
		IPAERR("unable to mmap the memory for %s\n", desc->name);
		ret = -EINVAL;
		goto bail;
	}
	IPADBG("mmap for %s return value 0x%lx\n", desc->name, (long unsigned int)desc->base_addr);

bail:
	if (close(device_fd))
	{
		IPAERR("unable to close the file descriptor for %s\n", desc->name);
		return -EINVAL;
	}
	return ret;
}

static int DeallocateMemory(ipa_mem_descriptor* desc, int ipa_fd)
{
	int ret;
	struct ipa_ioc_nat_ipv6ct_table_del cmd;

	IPADBG("\n");
	cmd.table_index = desc->table_index;
	ret = ioctl(ipa_fd, desc->delete_ioctl_num, &cmd);
	if (ret)
	{
		IPAERR("unable to post table delete command for %s Error: %d IPA fd %d\n", desc->name, ret, ipa_fd);
		return ret;
	}

	IPADBG("posted delete command for %s to kernel successfully\n", desc->name);
	return 0;
}

