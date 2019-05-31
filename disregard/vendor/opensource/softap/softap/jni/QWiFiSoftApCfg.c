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


#include "QWiFiSoftApCfg.h"

#define    UPDATE_ERROR_CODE(msg, code) \
    {    \
        int rc; \
        rc = snprintf(resp, sizeof(resp), "failure %s:%s",msg, code); \
        if ( rc == sizeof(resp)) resp[sizeof(resp)-1] = 0; \
            ALOGE("%s",resp); \
    }

static struct sockaddr_nl    rtnl_local;
static int    rtnl_fd = -1;
static char   evt_buf[MAX_EVT_BUF_SIZE];
static int    evt_len;

static void softap_handle_custom_event(char * buf, int len)
{
    if (strncmp(buf, "AUTO-SHUT.indication ", strlen("AUTO-SHUT.indication ")) == 0)
    {
        ALOGD("EVENT: Custom Event\n");
        snprintf(evt_buf, sizeof(evt_buf), "105 AP Shutdown");
    }
}

static void softap_handle_associated_event(char *mac_addr)
{
    snprintf(evt_buf, sizeof(evt_buf), "102 Station " HWA_FORM " Associated",
                            HWA_ARG(mac_addr));
}

static void softap_handle_disassociated_event(char *mac_addr)
{
    snprintf(evt_buf, sizeof(evt_buf), "103 Station " HWA_FORM " Disassociated",
                            HWA_ARG(mac_addr));
}

static void softap_handle_wireless_event(char *atr, int atrlen)
{
    int    len = 0;
    struct iw_event    iwe;
    char   *buffer = atr + RTA_ALIGN(RTATTRLEN);

    atrlen -= RTA_ALIGN(RTATTRLEN);

    while ((len + (int)IW_EV_LCP_LEN) < atrlen) {
        memcpy((char *)&iwe, buffer + len, sizeof(struct iw_event));

        if (iwe.len <= IW_EV_LCP_LEN)
            break;

        ALOGD("Received Wireless Event: cmd=0x%x len=%d", iwe.cmd, iwe.len);

        switch (iwe.cmd) {
            case IWEVEXPIRED:
                ALOGD("EVENT: IWEVEXPIRED\n");
                softap_handle_disassociated_event(iwe.u.addr.sa_data);
                break;

            case IWEVREGISTERED:
                ALOGD("EVENT: IWEVREGISTERED\n");
                softap_handle_associated_event(iwe.u.addr.sa_data);
                break;

            case IWEVCUSTOM:
                ALOGD("EVENT: Custom Event\n");
				softap_handle_custom_event(buffer + len + IW_EV_POINT_LEN, iwe.u.data.length);
                break;

            default:
                break;
        }

        len += iwe.len;
    }

    return;
}

void softap_handle_rtm_link_event(struct nlmsghdr *hdr)
{
    char   *ptr = (char *)NLMSG_DATA(hdr);
    struct rtattr        *atr;
    int    atr_len;

    if ((hdr->nlmsg_len - MSGHDRLEN) < IFINFOLEN) {
        ALOGD("Message Length Problem1");
        return;
    }

    if ((atr_len = hdr->nlmsg_len - NLMSG_ALIGN(IFINFOLEN)) < 0) {
        ALOGD("Message Length Problem2");
        return;
    }

    ptr += NLMSG_ALIGN(IFINFOLEN);
    atr = (struct rtattr *)ptr;

    while (RTA_OK(atr, atr_len)) {
        switch (atr->rta_type) {
            case IFLA_WIRELESS:
                softap_handle_wireless_event((char *)atr,
                            atr->rta_len);
                break;

            default:
                break;
        }

        atr = RTA_NEXT(atr, atr_len);
    }

    return;
}

static void softap_handle_iface_event(void)
{
    int       cnt, mlen = 0;
    char      *ptr, buffer[MAX_RECV_BUF_SIZE];
    socklen_t slen;
    struct nlmsghdr * hdr;

    while (1) {
        cnt = recvfrom(rtnl_fd, buffer, sizeof(buffer),
                MSG_DONTWAIT,
                (struct sockaddr *)&rtnl_local, &slen);

        if (cnt <= 0) {
            buffer[0] = '\0';
            ALOGD("recvfrom failed");
            return;
        }

        ptr = buffer;

        while (cnt >= MSGHDRLEN) {
            hdr = (struct nlmsghdr *)ptr;

            mlen = hdr->nlmsg_len;

            if ((mlen > cnt) || ((mlen - MSGHDRLEN) < 0)) {
                break;
            }

            switch (hdr->nlmsg_type) {
                case RTM_NEWLINK:
                case RTM_DELLINK:
                    softap_handle_rtm_link_event(hdr);
                    break;
            }

            mlen = NLMSG_ALIGN(hdr->nlmsg_len);
            cnt -= mlen;
            ptr += mlen;
        }
    }

    return;
}

static inline int softap_rtnl_wait(void)
{
    fd_set        fds;
    int        oldfd, ret;

    if (rtnl_fd < 0) {
        ALOGD("Netlink Socket Not Available");
        return -1;
    }

    /* Initialize fds */
    FD_ZERO(&fds);
    FD_SET(rtnl_fd, &fds);
    oldfd = rtnl_fd;

    /* Wait for some trigger event */
    ret = select(oldfd + 1, &fds, NULL, NULL, NULL);

    if (ret < 0) {
        /* Error Occurred */
        ALOGD("Select on Netlink Socket Failed");
        return ret;
    } else if (!ret) {
        ALOGD("Select on Netlink Socket Timed Out");
        /* Timeout Occurred */
        return -1;
    }

    /* Check if any event is available for us */
    if (FD_ISSET(rtnl_fd, &fds)) {
        softap_handle_iface_event();
    }

    return 0;
}

static void softap_rtnl_close(void)
{
    close(rtnl_fd);
}

static int softap_rtnl_open(void)
{
    int            addr_len;

    rtnl_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if (rtnl_fd < 0) {
        ALOGE("open netlink socket failed");
        return -1;
    }

    memset(&rtnl_local, 0, sizeof(rtnl_local));
    rtnl_local.nl_family = AF_NETLINK;
    rtnl_local.nl_groups = RTMGRP_LINK;

    if (bind(rtnl_fd, (struct sockaddr*)&rtnl_local,
                sizeof(rtnl_local)) < 0) {
        ALOGE("bind netlink socket failed");
        return -1;
    }

    addr_len = sizeof(rtnl_local);

    if (getsockname(rtnl_fd, (struct sockaddr*)&rtnl_local,
            (socklen_t *) &addr_len) < 0) {
        ALOGE("getsockname failed");
        return -1;
    }

    if (addr_len != sizeof(rtnl_local)) {
        ALOGE("Wrong address length %d\n", addr_len);
        return -1;
    }

    if (rtnl_local.nl_family != AF_NETLINK) {
        ALOGE("Wrong address family %d\n", rtnl_local.nl_family);
        return -1;
    }

    return 0;
}

JNIEXPORT void JNICALL
    Java_com_qualcomm_wifi_softap_QWiFiSoftApCfg_SapCloseNetlink
                        (JNIEnv *env, jobject obj)
{
    softap_rtnl_close();
    return;
}

JNIEXPORT jstring JNICALL
    Java_com_qualcomm_wifi_softap_QWiFiSoftApCfg_SapWaitForEvent
                        (JNIEnv *env, jobject obj)
{
    int    ret;

    do {
        evt_len = 0;
        memset(evt_buf, 0, sizeof(evt_buf));

        ret = softap_rtnl_wait();
    } while (!strlen(evt_buf));

    return (*env)->NewStringUTF(env, evt_buf);
}

JNIEXPORT jboolean JNICALL
    Java_com_qualcomm_wifi_softap_QWiFiSoftApCfg_SapOpenNetlink
                        (JNIEnv *env, jobject obj)
{
    if (softap_rtnl_open() != 0) {
        ALOGD("Netlink Open Fail");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

JNIEXPORT jstring JNICALL
        Java_com_qualcomm_wifi_softap_QWiFiSoftApCfg_SapSendCommand
                (JNIEnv *env, jobject obj, jstring jcmd)
{
    const char *pcmd;
    char       cmd[MAX_CMD_SIZE];
    char       resp[MAX_RESP_SIZE];
    int        sock = -1;
    int        rc;
    int        done = 0;
    char       code[32] = {0};
    int        connect_retry;

    strlcpy(cmd, "softap qccmd ", sizeof(cmd));

    pcmd = (char *) ((*env)->GetStringUTFChars(env, jcmd, NULL));

    if ( pcmd == NULL ) {
        UPDATE_ERROR_CODE("Command not handled","");
        goto end;
    }

    ALOGD("Received Command: %s\n", pcmd);

    if ((strlen(cmd) + strlen(pcmd)) >= sizeof(cmd)) {
        UPDATE_ERROR_CODE("Command length is larger than MAX_CMD_SIZE", "");
        goto end;
    }

    strlcat(cmd, pcmd, sizeof(cmd));

    connect_retry = 0;

    while ( 1 ) {
        if ((sock = socket_local_client("netd",
                    ANDROID_SOCKET_NAMESPACE_RESERVED, 
                        SOCK_STREAM)) < 0) {
            if (connect_retry > 3) {
                UPDATE_ERROR_CODE("Error connecting",
                        strerror(errno));
                goto end;
            }

            ALOGW("Unable to connect to netd, retrying ...\n");
            sleep(1);
        } else {
            break;
        }

        connect_retry++;
    }

    if (write(sock, cmd, strlen(cmd) + 1) < 0) {
        UPDATE_ERROR_CODE("Error Writing to socket", strerror(errno));
        goto end;
    }

    while (!done) {
        int i;

        if ((rc = read(sock, resp, sizeof(resp))) <= 0) {
            if (rc == 0) {
                UPDATE_ERROR_CODE("Lost connection to Netd",
                        strerror(errno));
            } else {
                UPDATE_ERROR_CODE("Error reading data",
                        strerror(errno));
            }

            done = 1;
        } else {
            /* skip broadcase messages */
            i = 0;

            while(resp[i] && (i<(int)(sizeof(code)-1)) &&
                (resp[i] != ' ') && (resp[i] != '\t')) {
                code[i] = resp[i];
                i++;
            }

            code[i] = '\0';

            if ( (!strcmp(code, "success")) ||
                    (!strcmp(code, "failure")) ) {
                done=1;
            } else {
                ALOGW("Code(%s)\n", code);
                ALOGW("Ignore messages : %s\n", resp);
            }
        }
    }

end:
    (*env)->ReleaseStringUTFChars(env, jcmd, pcmd);

    if( sock >= 0 ){
        close(sock);
        sock = -1;
    }

    return (*env)->NewStringUTF(env, resp);
}
