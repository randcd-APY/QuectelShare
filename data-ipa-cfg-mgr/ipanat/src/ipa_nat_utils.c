/*
 * Copyright (c) 2013, 2018 The Linux Foundation. All rights reserved.
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
#include "ipa_nat_utils.h"
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define IPA_MAX_MSG_LEN 4096

static char dbg_buff[IPA_MAX_MSG_LEN];

static ipa_descriptor ipa_desc;
static int ipa_desc_count;
static pthread_mutex_t ipa_desc_mutex = PTHREAD_MUTEX_INITIALIZER;

#if !defined(MSM_IPA_TESTS) && !defined(USE_GLIB) && !defined(FEATURE_IPA_ANDROID)
size_t strlcpy(char* dst, const char* src, size_t size)
{
	size_t i;

	if (size == 0)
		return strlen(src);

	for (i = 0; i < (size - 1) && src[i] != '\0'; ++i)
		dst[i] = src[i];

	dst[i] = '\0';

	return i + strlen(src + i);
}
#endif

ipa_descriptor* ipa_descriptor_open(void)
{
	int res;
	ipa_descriptor* ret = &ipa_desc;

	IPADBG("\n");

	if (pthread_mutex_lock(&ipa_desc_mutex))
	{
		IPAERR("unable to lock the IPA descriptor mutex\n");
		return NULL;
	}

	if (ipa_desc_count)
		goto incr_count;

	ipa_desc.fd = open(IPA_DEV_NAME, O_RDONLY);
	if (ipa_desc.fd < 0)
	{
		IPAERR("unable to open ipa device\n");
		ret = NULL;
		goto bail;
	}


	res = ioctl(ipa_desc.fd, IPA_IOC_GET_HW_VERSION, &ipa_desc.ver);
	if (res)
	{
		IPAERR("unable to get IPA version. Error %d\n", res);
		ipa_desc.ver = IPA_HW_None;
	}
	else
	{
		IPADBG("IPA version is %d\n", ipa_desc.ver);
	}

incr_count:
	++ipa_desc_count;
bail:
	if (pthread_mutex_unlock(&ipa_desc_mutex))
	{
		IPAERR("unable to unlock the IPA descriptor mutex\n");
	}
	return ret;
}

void ipa_descriptor_close(void)
{
	IPADBG("\n");
	if (pthread_mutex_lock(&ipa_desc_mutex))
	{
		IPAERR("unable to lock the IPA descriptor mutex\n");
		return;
	}

	if (--ipa_desc_count)
	{
		return;
	}

	close(ipa_desc.fd);
	memset(&ipa_desc, 0, sizeof(ipa_desc));

	if (pthread_mutex_unlock(&ipa_desc_mutex))
	{
		IPAERR("unable to unlock the IPA descriptor mutex\n");
	}
	IPADBG("return\n");
}

void ipa_read_debug_info(const char* debug_file_path)
{
	size_t result;
	FILE* debug_file;

	debug_file = fopen(debug_file_path, "r");
	if (debug_file == NULL)
	{
		printf("Failed to open %s\n", debug_file_path);
		return;
	}

	for (;;)
	{
		result = fread(dbg_buff, sizeof(char), IPA_MAX_MSG_LEN, debug_file);
		if (!result)
			break;

		if (result < IPA_MAX_MSG_LEN)
		{
			if (ferror(debug_file))
			{
				printf("Failed to read from %s\n", debug_file_path);
				break;
			}

			dbg_buff[result] = '\0';
		}
		else
		{
			dbg_buff[IPA_MAX_MSG_LEN - 1] = '\0';
		}


		printf("%s", dbg_buff);

		if (feof(debug_file))
			break;
	}
	fclose(debug_file);
}

void log_nat_message(char *msg)
{
	 return;
}


