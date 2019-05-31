/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
#ifndef CONFIG_DAEMON_MODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>/* File Control Definitions           */
#include <string.h>
#include <signal.h>
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <errno.h>   /* ERROR Number Definitions           */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include "qsHost.h"
#include "qsHostUart.h"

typedef struct
{
    uint32_t        ref_Cnt;
    int32_t         handle;
    char            *device_Name;
    pthread_t       thread;
    uint32_t        thread_Exit;
    pthread_mutex_t mutex;
    pthread_cond_t  cv;
} UartCxt_t;

typedef struct
{
    uint32_t index;
    uint8_t initialized;
} UartThreadParam_t;

static UartCxt_t *UartCxt[QS_UART_MAX_PORTS_E] = { NULL, NULL };

static int32_t QsUartReadNbytes(int32_t handle, char *buffer, uint32_t nbytes)
{
    uint32_t needed = nbytes, remaining = nbytes;
    int bytes_read = 0;

    if(!buffer)
    {
        return ssFailure;
    }

    while(remaining)
    {
        bytes_read = read(handle, &buffer[needed - remaining], remaining); /* Read the header */
        if (bytes_read < 0 )
        {
            printf("Error in read\n");
            return ssFailure;
        }
        remaining -= bytes_read;
    }

    return ssSuccess;
}

static void* UartRxThread(void *param)
{
    uint32_t index = 0, cur_Ptr = 0, ret_Val = ssFailure;
    uint16_t total_Size = 0;
    uint8_t type = 0;
    UartCxt_t *cxt = NULL;
    BufInfo_t *packet_Info = NULL;
    qsQapiPacketHeader_t *header = NULL;
    char *packet = NULL;

    index = ((UartThreadParam_t *)param)->index;
    cxt = UartCxt[index];

    pthread_mutex_lock(&cxt->mutex);
    tcflush(cxt->handle, TCIFLUSH); /* Discards old data in the rx buffer */
    cxt->ref_Cnt = 1;
    ((UartThreadParam_t *)param)->initialized = 1;
    pthread_cond_signal(&cxt->cv);

    while(FALSE == cxt->thread_Exit)
    {
        pthread_mutex_unlock(&cxt->mutex);

        if(ssSuccess != QsGetBuf(&packet_Info, BUF_SIZE_2048))
        {
            ret_Val = ssFailure;
            break;
        }
        packet = packet_Info->buf;

#ifdef LATENCY_DEBUG
        if (ssSuccess == QsUartReadNbytes(cxt->handle, packet, size))
        {
            stop = getMicrotime();
            printf ("Stop:%ld, Start:%ld, Total time: %ld\n\n", stop, start, stop-start);
            header = (qsQapiPacketHeader_t *)packet;
            total_Size = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&header->TotalLength);
            memcpy(&rx_size, packet, 2);
            printf("Received Size: %u\n", total_Size);
            QsReleaseBuf(packet_Info);
            continue;
        }
#endif /* LATENCY_DEBUG */

        if (ssSuccess == QsUartReadNbytes(cxt->handle, packet, sizeof(qsQapiPacketHeader_t)))
        {
            cur_Ptr = sizeof(qsQapiPacketHeader_t);
            header = (qsQapiPacketHeader_t *)packet;
            total_Size = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&header->TotalLength);
            if (ssSuccess != QsUartReadNbytes(cxt->handle, &packet[cur_Ptr], total_Size - sizeof(qsQapiPacketHeader_t)))
            {
                ret_Val = ssFailure;
                break;
            }

            type = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&header->PacketType);
            switch(type)
            {
                case QS_RETURN_E:
                    ret_Val = QsEnqResp(packet_Info);
                    if(ssSuccess != ret_Val)
                    {
                        QsReleaseBuf(packet_Info);
                    }
                    break;

                case QS_ASYNC_E:
                    ret_Val = QsProcessEvent(packet_Info);
                    QsReleaseBuf(packet_Info);
                    break;

                default:
                    ret_Val = ssFailure;
                    QsReleaseBuf(packet_Info);
                    break;
            }
        }

        pthread_mutex_lock(&cxt->mutex);
    }

    pthread_cond_signal(&cxt->cv);
    pthread_mutex_unlock(&cxt->mutex);
    pthread_exit(param);
}

uint32_t QsUartTransmit(int32_t handle, uint32_t len, char *buffer)
{
    uint32_t  bytes_written = 0;  /* Value for storing the number of bytes written to the port */
    bytes_written += write(handle, buffer, len);
    return ssSuccess;
}


static uint32_t QsUartPortInit(char *device_Name, int32_t *handle, uint32_t *index)
{
    UartCxt_t *cxt = NULL;
    int32_t fd = -1;
    uint32_t len = 0, i = 0;
    struct termios serial_Port;

    fd = open(device_Name, O_RDWR | O_NOCTTY);
    if(-1 == fd)
    {
        printf("Error! in Opening device %s\n", device_Name);
        *index = -1;
        return ssFailure;
    }

    memset(&serial_Port, 0, sizeof(serial_Port));
    if (0 != tcgetattr(fd, &serial_Port)) /* Get the current attributes of the Serial port */
    {
        printf("Error in getting existing serial port config\n");
        return ssFailure;
    }

    if(0 != cfsetispeed(&serial_Port, B115200))
    {
        printf("cfsetispeed failed\n");
        return ssFailure;
    }
    if(0 != cfsetospeed(&serial_Port, B115200))
    {
        printf("cfsetospeed failed\n");
        return ssFailure;
    }

    cfmakeraw(&serial_Port);

    serial_Port.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    serial_Port.c_cflag |= (CLOCAL | CREAD | CS8);

    serial_Port.c_oflag &= ~OPOST;
    serial_Port.c_cc[VMIN] = 1;
    serial_Port.c_cc[VTIME] = 0;

    if(0 != tcsetattr(fd, TCSANOW, &serial_Port))
    {
        printf("ERROR ! in Setting attributes\n");
        return ssFailure;
    }

    for(i = 0; i < QS_UART_MAX_PORTS_E; i++)
    {
        if(!UartCxt[i])
        {
            break;
        }
    }

	if(i == QS_UART_MAX_PORTS_E)
    {
        return ssFailure;
    }

    UartCxt[i] = (UartCxt_t *)malloc(sizeof(UartCxt_t));
    if(!UartCxt[i])
    {
        return ssFailure;
    }

    cxt = UartCxt[i];
    memset(cxt, 0, sizeof(UartCxt_t));

    len = strlen(device_Name);
    cxt->device_Name = (char *)malloc(len + 1);
    memcpy(cxt->device_Name, device_Name, len);
    cxt->device_Name[len] = '\0';

    cxt->handle = fd;
    *handle = fd;
    *index = i;
    return ssSuccess;
}


static uint32_t QsUartDeviceInUse(char *device_Name, int32_t *handle, uint32_t *num_Devices_In_Use)
{
    uint32_t len = 0, i = 0;
    UartCxt_t *cxt = NULL;

    len = strlen(device_Name);
    for(i = 0; i < QS_UART_MAX_PORTS_E; i++)
    {
        cxt = UartCxt[i];
        if(cxt)
        {
            (*num_Devices_In_Use)++;
            if((len == strlen(cxt->device_Name)) && cxt->device_Name &&
                    !memcmp(device_Name, cxt->device_Name, len))
            {
                break;
            }
        }
    }

    if(QS_UART_MAX_PORTS_E == i)
    {
        return FALSE;
    }

    *handle = cxt->handle;
    cxt->ref_Cnt++;
    return TRUE;
}

static uint32_t CreateThread(uint32_t index)
{
    UartCxt_t *cxt = UartCxt[index];
    UartThreadParam_t param;

    param.index = index;
    param.initialized = 0;

    if(0 == pthread_create(&cxt->thread, NULL, UartRxThread, (void *)&param))
    {
        pthread_mutex_lock(&cxt->mutex);

        while(!param.initialized)
        {
            pthread_cond_wait(&cxt->cv, &cxt->mutex);
        }
        pthread_mutex_unlock(&cxt->mutex);
        return ssSuccess;
    }

    return ssFailure;
}

uint32_t QsUartInit(char *device_Name, int32_t *handle)
{
    uint32_t cxt_Index = 0, num_Devices = 0, ret_Val = ssSuccess;
    UartCxt_t *cxt = NULL;

    if(!device_Name || !handle)
    {
        return ssFailure;
    }

    ret_Val = QsUartDeviceInUse(device_Name, handle, &num_Devices);
    if(TRUE == ret_Val)
    {
        return ssSuccess;
    }

    /* Target accepts maximum 2 UART devices */
    if(QS_UART_MAX_PORTS_E == num_Devices)
    {
        return ssFailure;
    }

    ret_Val = QsUartPortInit(device_Name, handle, &cxt_Index);
    if(ssSuccess == ret_Val)
    {
        cxt = UartCxt[cxt_Index];
        /* Create independent threads each of which will execute function */
        if ((0 == pthread_mutex_init(&cxt->mutex, NULL)) && (0 == pthread_cond_init(&cxt->cv, NULL)))
        {
            ret_Val =  CreateThread(cxt_Index);
        }
        else
        {
            ret_Val = ssFailure;
        }
    }

    if(ssSuccess != ret_Val)
    {
        QsUartDeInit(*handle);
        return ssFailure;
    }

#ifdef LATENCY_DEBUG
    QsGetTotalTime(cxt);
#endif /* LATENCY_DEBUG */
    return ssSuccess;
}

uint32_t QsUartDeInit(int32_t handle)
{
    uint32_t i = 0;
    UartCxt_t *cxt = NULL;

    for(i = 0; i < QS_UART_MAX_PORTS_E; i++)
    {
        cxt = UartCxt[i];
        if(cxt)
        {
            pthread_mutex_lock(&cxt->mutex);
            if((handle == cxt->handle) && cxt->ref_Cnt)
            {
                cxt->ref_Cnt--;
                if(0 == cxt->ref_Cnt)
                {
                    cxt->thread_Exit = TRUE;
                    pthread_cond_wait(&cxt->cv, &cxt->mutex);
                }
            }
            pthread_mutex_unlock(&cxt->mutex);
            close(cxt->handle);
            free(cxt->device_Name);
            free(cxt);
            UartCxt[i] = NULL;
            return ssSuccess;
        }
    }
    return ssFailure;
}


#if 0
char *see_speed(speed_t speed, char* SPEED) {
    switch (speed) {
        case B115200:
            strcpy(SPEED, "B115200");
            break;

        case B38400:
            strcpy(SPEED, "B38400");
            break;

        default:
            sprintf(SPEED, "unknown (%d)", (int) speed);
            break;
    }
    return SPEED;
}
#endif /* 0 */

#endif /* not defined CONFIG_DAEMON_MODE */

