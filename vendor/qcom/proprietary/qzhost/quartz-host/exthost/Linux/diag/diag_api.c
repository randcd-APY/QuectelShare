/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/timex.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "qsCommon.h"
#include "diag_api.h"

#define QS_IOTD_TYPE_OFFSET               (2)


/*
 * Function: diag_open() - opens the msg q to iot daemon, exchanges handshake
 *                        messages with the daemon. It returns a handle which
 *                       must be used in all future communications.
 * Input: name- iot daemon msg q name
 * Returns: valid handle if handshake successful


 *          NULL on failure.
 */
void* diag_open(char* name)
{
    struct mq_attr attr;
    handle_t* hdl = NULL;
    char clientQ[32];
    char* buf = NULL;
    uint32_t bufLen = 0;   
    int32_t sz;
    uint32_t pid = getpid();
    uint8_t type= 0, CtrlType = 0;

    if((hdl = malloc(sizeof(handle_t))) == NULL)
        return NULL;

    hdl->client_handle = -1;

    if((buf=malloc(MSG_BUFFER_SIZE)) == NULL){
        goto ERROR;
    }

    memset(buf,0,MSG_BUFFER_SIZE);

    /* Initialize receive queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create the client queue for receiving messages from server*/
    sprintf (clientQ, "/client-%d", pid);

    strncpy(hdl->client_name, clientQ, strlen(clientQ));

    hdl->client_handle = mq_open((const char *)clientQ, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (-1 == hdl->client_handle)
    {
        perror ("Client: mq_open (server)");
        goto ERROR;
    }

    /*****Create HELLO message*****/

    /*Set Control message flag*/
    QS_SET_CONTROL_BIT(buf);

    /*Store the Client ProcessID in the hello message*/
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[QS_IOTD_COOKIE_OFFSET], getpid());

    /*Write Message type*/
    bufLen = QS_CONTROL_MSG_TYPE_OFFSET;
    buf[bufLen] = QS_HELLO_REQ;

    /*Copy name of client queue*/
    bufLen += 1;
    memcpy(&buf[bufLen], clientQ, strlen(clientQ));

    bufLen += strlen(clientQ);

    hdl->serv_handle = mq_open((const char *)name, O_WRONLY);
    if (-1 == hdl->serv_handle)
    {
        perror ("Client: mq_open (server)");
        goto ERROR;
    }

    sz = mq_send(hdl->serv_handle, (const char *)buf, bufLen, 0);
    if(-1 == sz)
    {
        perror ("Client: Not able to send message to server");
        goto ERROR;
    }

    sz = mq_receive(hdl->client_handle, buf, MSG_BUFFER_SIZE, NULL);
    if (-1 == sz)
    {
        perror ("Client: mq_receive");
        goto ERROR;
    }

    CtrlType = QS_GET_CONTROL_BIT(buf);

    if(CtrlType == 1){
	type = buf[QS_CONTROL_MSG_TYPE_OFFSET];

        if(type == QS_HELLO_RESP){
            printf("rx hello resp\n");
            free(buf);
            return hdl;
        }   else if(type == QS_SERVER_EXIT){
            printf("Iotd exit\n");
            goto ERROR;
        }   else if(type == QS_TARGET_ASSERT){
            printf("Target Asserted\n");
            goto ERROR;
        }   else {
            printf("Invalid response\n");
            goto ERROR;
        }
    } else {
        printf("Received invalid packet\n");
        goto ERROR;
    }

ERROR:
    if(buf)
        free(buf);

    if(hdl){
        if(hdl->client_handle != -1){
            mq_close(((handle_t*)(hdl))->client_handle);
            mq_unlink(((handle_t*)(hdl))->client_name);
        }

        free(hdl);
    }
    return NULL;

}


/*
 * Function: diag_send() - sends diag frame to iot daemon via a posix msg q
 * Input: hdl- handle that was created on diag_open
 *        devID - ID of destination QCA402X device, default 0, only applicable
 *                when more than one QCA402X devices are connected.
 *        buf- buffer to send
 *        len- buffer size
 * Returns: 0 - success
 *          -1 - failure
 */
int diag_send(void* hdl, uint8_t devID, uint8_t* buf, int len)
{
    uint8_t* txBuf = NULL;
    handle_t* handle = (handle_t*)hdl;
    int sz = -1, result=0;
    uint16_t total_Len = QS_IOTD_HEADER_LEN + len;

    if(handle == NULL){
        return -1;
    }

    if(total_Len > MAX_MSG_SIZE){
        return -1;
    }

    if((txBuf = malloc(total_Len)) == NULL){
        return -1;
    }

    memset(txBuf, 0, total_Len);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&txBuf[QS_IOTD_COOKIE_OFFSET], getpid());
    memcpy(&txBuf[QS_IOTD_HEADER_LEN], buf, len);

    APPLY_DEV_ID(txBuf, devID);

    QS_RESET_CONTROL_BIT(buf);

    sz = mq_send(handle->serv_handle, (const char *)txBuf, total_Len, 0);
    if(-1 == sz)
    {
        perror ("Client: Not able to send message to server");
        result = -1;
    }


    free(txBuf);
    return result;
}


/*
 * Function: diag_recv() - wait for specified time to receive a packet.
 * Input:hdl - valid handle
 *       buf - buffer to recieve the packet
 *       len - size in bytes of provided buffer
 *       msec - wait time in milliseconds, if 0, wait until a message is available
 * Returns: Number of bytes read on success
 -1 on error
 */
int diag_recv(void* hdl, uint8_t* buf, int len, int msec)
{
    handle_t* handle = (handle_t*)hdl;
    uint8_t* rxBuf;
    int sz;
    uint8_t type, CtrlType;
    struct   timespec tm;

    if(handle == NULL){
        return -1;
    }

    if((rxBuf = malloc(MSG_BUFFER_SIZE)) == NULL){
        return -1;
    }

    if(len > MSG_BUFFER_SIZE){
        len = MSG_BUFFER_SIZE;
    }

    /*If time in msec is provided, convert to timespec format*/
    if(msec != 0){
        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += msec/1000;
        tm.tv_nsec += (msec%1000)*1000000;
        if(tm.tv_nsec > 1e9){
            tm.tv_sec++;
            tm.tv_nsec -= 1e9;
        }
        sz = mq_timedreceive(handle->client_handle, rxBuf, MSG_BUFFER_SIZE, NULL, &tm);
    } else {
        sz = mq_receive(handle->client_handle, rxBuf, MSG_BUFFER_SIZE, NULL);
    }
    if (-1 == sz)
    {
        perror ("Client: mq_receive");
    }else{
        CtrlType = QS_GET_CONTROL_BIT(rxBuf);

        if(CtrlType == 0){
            memcpy(buf,&rxBuf[QS_IOTD_HEADER_LEN],sz-QS_IOTD_HEADER_LEN);
        } else if(CtrlType == 1){
	    sz = -1;
            type = rxBuf[QS_CONTROL_MSG_TYPE_OFFSET];

            if(type == QS_HELLO_RESP){
	        printf("rx hello resp\n");
            }else if(type == QS_SERVER_EXIT){
	        printf("Iotd exit\n");
   	    }else if(type == QS_TARGET_ASSERT){
	        printf("Target Asserted\n");
	    } else {
  	        printf("Invalid response\n");
	    }
        }
    }

    free(rxBuf);
    return sz-QS_IOTD_HEADER_LEN;
}


/*
 * Function: diag_close() - closes the connection to the iot daemon.
 * Input: hdl- valid handle
 * Returns: 0 - success
 *           1 - failure
 */
int diag_close(void* hdl)
{
    char exit_msg[64] = {'\0'};
    int sz = 0;
    handle_t* handle = (handle_t*)hdl;

    if(hdl == NULL){
        return -1;
    }

    /*Set Control message flag*/
    QS_SET_CONTROL_BIT(exit_msg);

    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&exit_msg[QS_IOTD_COOKIE_OFFSET], getpid());
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&exit_msg[QS_IOTD_HEADER_LEN], QS_CLIENT_EXIT);

    sz = mq_send(handle->serv_handle, (const char *)exit_msg, QS_IOTD_HEADER_LEN+1, 0);
    if (-1 == sz) {
        perror ("Client: Exit error\n");
        return -1;
    }
    mq_close(((handle_t*)(hdl))->client_handle);
    mq_unlink(((handle_t*)(hdl))->client_name);
    return 0;
}

