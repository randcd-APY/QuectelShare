/*
 * Copyright (c) 2010, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.

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


#ifndef __QWIFISOFTAPCFG
#define __QWIFISOFTAPCFG

#define LOG_TAG "QWIFIAPCFG"

#include "jni.h"
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include <ctype.h>

#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>

#include <cutils/sockets.h>
#include <private/android_filesystem_config.h>

typedef unsigned char    u8;

#define HWA_FORM        "%02X:%02X:%02X:%02X:%02X:%02X"
#define HWA_ARG(x)        *(((u8 *)x + 0)), *(((u8 *)x + 1)), \
                          *(((u8 *)x + 2)), *(((u8 *)x + 3)), \
                          *(((u8 *)x + 4)), *(((u8 *)x + 5))

#define MAX_RESP_SIZE       256
#define MAX_CMD_SIZE        256
#define MAX_EVT_BUF_SIZE    256
#define MAX_RECV_BUF_SIZE   256

#define MSGHDRLEN        ((int)(sizeof(struct nlmsghdr)))
#define IFINFOLEN        ((int)(sizeof(struct ifinfomsg)))
#define RTATTRLEN        ((int)(sizeof(struct rtattr)))

#ifndef IFLA_WIRELESS
#define IFLA_WIRELESS    (IFLA_MASTER + 1)
#endif

#endif
