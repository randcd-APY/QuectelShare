/******************************************************************************
#@file    mcm_ssr_util.c
#@brief   To notify SSR status to client
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "mcm_ssr_util.h"
#include "util_log.h"

int clientSockFd = -1;
int srvSockFd = -1;
int number_of_client_connected = 0;
int number_of_client_reported_ssr = 0;
int mcm_ssr_fds[SSR_CLIENT_NUM_LISTEN_QUEUE] = {-1};
pthread_mutex_t ssr_lock_mutex;
ssr_status_cb mcm_ril_cb = NULL;

uint8_t mcm_ssr_client_init(void)
{
    struct sockaddr_un servaddr;
    int sockFd = -1, ret = -1;
    uint8_t status = FALSE;

    do
    {
        if((sockFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_client_init: sockFd creation failed");
            break;
        }

        /* configuring server address structure */
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family      = AF_UNIX;
        strlcpy(servaddr.sun_path, SSR_SOCKET_PATH, sizeof(servaddr.sun_path));

        if((ret = connect(sockFd, &servaddr, sizeof(servaddr))) < 0 )
        {
            UTIL_LOG_MSG("mcm_ssr_client_init: connecting to socket failed");
            close(sockFd);
            sockFd = -1;
            break;
        }

        clientSockFd = sockFd;
        status = TRUE;
    }while(FALSE);

    UTIL_LOG_MSG("mcm_ssr_client_init: status %d", status);
    return status;
}

uint8_t mcm_ssr_client_send_radio_available(void)
{
    char buf[20];
    int ret = -1;
    uint8_t status = FALSE;

    do
    {
        if(clientSockFd < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_client_send_radio_available: invalid clientSockFd");
            break;
        }

        memset(buf,0,sizeof(buf));
        snprintf(buf,sizeof(buf),"%d",MCM_SSR_RADIO_STATE_AVAILABLE_V01);
        ret = write(clientSockFd, buf, strlen(buf));
        if(ret < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_client_send_radio_available: writing to clientSockFd failed");
            break;
        }

        status = TRUE;
    }while(FALSE);

    UTIL_LOG_MSG("mcm_ssr_client_send_radio_available: status %d", status);
    return status;
}

uint8_t mcm_ssr_server_init(ssr_status_cb cb)
{
    pthread_mutexattr_t mtx_atr;
    struct sockaddr_un servaddr;
    int sockFd = -1, ret = -1;
    uint8_t status = FALSE;

    do
    {
        if(NULL == cb)
        {
            UTIL_LOG_MSG("mcm_ssr_server_init: no callback passed");
            break;
        }

        if((sockFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_server_init: sockFd creation failed");
            break;
        }

        pthread_mutexattr_init(&mtx_atr);
        pthread_mutex_init(&ssr_lock_mutex, &mtx_atr);

        /* configuring server address structure */
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family      = AF_UNIX;
        strlcpy(servaddr.sun_path, SSR_SOCKET_PATH, sizeof(servaddr.sun_path));
        unlink(servaddr.sun_path);

        if((ret = bind(sockFd, &servaddr, sizeof(servaddr))) < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_server_init: binding to socket failed");
            close(sockFd);
            sockFd = -1;
            break;
        }

        /* convert our socket to a listening socket */
        if ((ret = listen(sockFd, SSR_CLIENT_NUM_LISTEN_QUEUE)) < 0)
        {
            UTIL_LOG_MSG("mcm_ssr_server_init: listening to socket failed");
            close(sockFd);
            sockFd = -1;
            break;
        }

        srvSockFd = sockFd;
        mcm_ril_cb = cb;
        if((ret = create_client_attach_thread()) <= 0)
        {
            UTIL_LOG_MSG("mcm_ssr_server_init: thread creation failed");
            close(sockFd);
            sockFd = -1;
            srvSockFd = -1;
            mcm_ril_cb = NULL;
            break;
        }

        status = TRUE;
    }while(FALSE);

    UTIL_LOG_MSG("mcm_ssr_server_init: status %d", status);
    return status;
}

uint8_t create_client_attach_thread(void)
{
    uint8_t ret = TRUE;
    pthread_t thread_id;
    pthread_attr_t attr;

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if ( 0 == pthread_create(&thread_id, &attr, client_attach_thread_func, NULL) )
    {
        pthread_setname_np(thread_id, SSR_CLIENT_THREAD_NAME);
    }
    else
    {
        ret = FALSE;
    }

    pthread_attr_destroy(&attr);
    return ret;
}

void *client_attach_thread_func(void *buf)
{
    struct sockaddr_un cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int client_fd = -1, i = 0, result = -1, fd = -1, nread = -1;
    char read_buf[20];
    fd_set readfds;
    fd_set testfds;

    do
    {
        if(srvSockFd < 0)
        {
            break;
        }

        FD_ZERO(&readfds);
        FD_SET(srvSockFd, &readfds);

        for (;;)
        {
            testfds = readfds;
            UTIL_LOG_MSG("Waiting for connection");
            result  = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
            UTIL_LOG_MSG("connection received");
            if (result < 0 && errno == EINTR)
            {
                UTIL_LOG_MSG("connection failed");
                break;
            }

            for (fd = 0; fd < FD_SETSIZE; fd++)
            {
                if (FD_ISSET(fd,&testfds))
                {
                    if (fd == srvSockFd)
                    {
                        client_fd = accept(srvSockFd, (struct sockaddr*) &cliaddr, &clilen);
                        if(client_fd < 0)
                        {
                            continue;
                        }

                        FD_SET(client_fd, &readfds);
                        for (i = 0; i < SSR_CLIENT_NUM_LISTEN_QUEUE && mcm_ssr_fds[i] != -1; i++)
                        {
                            continue;
                        }

                        if (i < SSR_CLIENT_NUM_LISTEN_QUEUE)
                        {
                            acquire_lock();
                            mcm_ssr_fds[i] = client_fd;
                            number_of_client_connected++;
                            release_lock();
                            UTIL_LOG_MSG("1. number_of_client_connected = %d", number_of_client_connected);
                        }
                    }
                    else
                    {
                        memset(read_buf,0,sizeof(read_buf));
                        nread = read(fd, read_buf, sizeof(read_buf));
                        if (nread == 0)
                        {
                            close(fd);
                            FD_CLR(fd, &readfds);
                            for (i = 0; i < SSR_CLIENT_NUM_LISTEN_QUEUE && mcm_ssr_fds[i] != fd; i++)
                            {
                                continue;
                            }

                            if (i < SSR_CLIENT_NUM_LISTEN_QUEUE)
                            {
                                acquire_lock();
                                mcm_ssr_fds[i] = -1;
                                number_of_client_connected--;
                                release_lock();
                                UTIL_LOG_MSG("2. number_of_client_connected = %d", number_of_client_connected);
                            }
                        }
                        else
                        {
                            acquire_lock();
                            number_of_client_reported_ssr++;
                            if(number_of_client_connected == number_of_client_reported_ssr)
                            {
                                //Call mcm-ril callback
                                if(mcm_ril_cb)
                                {
                                    mcm_ril_cb();
                                }
                            }
                            release_lock();
                            UTIL_LOG_MSG("3. number_of_client_connected = %d", number_of_client_connected);
                            UTIL_LOG_MSG("1. number_of_client_reported_ssr = %d", number_of_client_reported_ssr);
                        }
                    }
                }
            }
        }
    }while(FALSE);

    for (i = 0; i < SSR_CLIENT_NUM_LISTEN_QUEUE; i++)
    {
        if (mcm_ssr_fds[i] > -1)
        {
            close(mcm_ssr_fds[i]);
        }
    }

    close(srvSockFd);
    srvSockFd = -1;
    return NULL;
}

void mcm_ssr_reset_client_reported_status(void)
{
    acquire_lock();
    number_of_client_reported_ssr = 0;
    release_lock();
    UTIL_LOG_MSG("2. number_of_client_reported_ssr = %d", number_of_client_reported_ssr);
}

uint8_t mcm_ssr_is_client_reported_status(void)
{
    uint8_t ret = FALSE;
    UTIL_LOG_MSG("3. number_of_client_reported_ssr = %d", number_of_client_reported_ssr);
    UTIL_LOG_MSG("4. number_of_client_connected = %d", number_of_client_connected);
    acquire_lock();
    if(number_of_client_connected == number_of_client_reported_ssr)
    {
        ret = TRUE;
    }
    release_lock();
    return ret;
}

void acquire_lock(void)
{
    pthread_mutex_lock(&ssr_lock_mutex);
}

void release_lock(void)
{
    pthread_mutex_unlock(&ssr_lock_mutex);
}
