/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <syslog.h>
#include "iotd_context.h"
#include "mml.h"
#include "bufpool.h"
#include <termios.h> 
#include <unistd.h> 
#include <errno.h>
#include "htc.h"
#include <sys/ioctl.h>
#include <linux/serial.h>


/*
 * Function:uart_getBaud
 *          Get baud rate macro based on configured baud value
 * Input: baud- configured baud value
 * Returns: UART Baud rate or IOTD_ERROR
 *
 */
int32_t uart_getBaud(int baud)
{
    switch (baud) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return IOTD_ERROR;
    }
}


/*
 * Function:uart_Read_Nbytes
 * Input: handle- device handle
 *        buffer- input buffer
 *        nbytes- bytes to read
 * Returns: IOTD_OK/ IOTD_ERROR
 * Description: Block until requested number of bytes are read from the UART interface.
 */
static int32_t uart_Read_Nbytes(int32_t handle, char *buffer, uint32_t nbytes)
{
    uint32_t needed = nbytes, remaining = nbytes;
    int bytes_read = 0;
    if(!buffer)
    {
        return IOTD_ERROR;
    }
    while(remaining)
    {
        bytes_read = read(handle, &buffer[needed - remaining], remaining); /* Read the header */
        if (bytes_read < 0 )
        {
            IOTD_LOG(LOG_TYPE_CRIT,"Error in read\n");
            return IOTD_ERROR;
        }
        remaining -= bytes_read;
    }
    return IOTD_OK;
}

/*
 * Function: uart_RXthread
 *           receive packet on UART interface and queue to the corresponding
 *           queue based on sq_id.
 * Input:  Interface context pointer
 * Returns:
 *
 */
void* uart_RXthread(void* arg)
{
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)arg;
    UART_CXT_T* pUartCxt = NULL;
    char *packet = NULL;
    uint8_t devId = 0;
    uint16_t total_Size = 0;
    uint32_t cur_Ptr = 0;

    if(!pIfaceCxt)
    {
        pthread_exit(arg);
    }
    pUartCxt = &(pIfaceCxt->hif_cxt.uart_cxt);
    devId = (pIfaceCxt->map.dev_id << 4) & 0xF0;
    IOTD_LOG(LOG_TYPE_CRIT,"Uart RX thread started for target ID %u\n",pIfaceCxt->map.dev_id);

    tcflush(pUartCxt->fd, TCIFLUSH); /* Discards old data in the rx buffer */
    while(1)
    {
        while((packet = (char *)buf_alloc(MAX_BUFFER_SIZE)) == NULL){
            /*Lets wait for buffer to be available*/
            usleep(5000);
        }

#ifdef LATENCY_DEBUG
        uint16_t rx_size = 0;
        if (0 == uart_Read_Nbytes(pUartCxt->fd, packet, size))
        {
            stop = getMicrotime();
            printf ("Stop:%ld, Start:%ld, Total time: %ld\n\n",stop, start,stop-start);
            memcpy(&rx_size, packet, 2);
            printf("Received Size: %u\n", rx_size);
            continue;
        }
#endif /* LATENCY_DEBUG */

        if (0 == uart_Read_Nbytes(pUartCxt->fd, packet, IOTD_HEADER_LEN))
        {
            //toggleGPIO();
            memcpy(&total_Size, &packet[0], 2);
            if(total_Size)
            {
                cur_Ptr = IOTD_HEADER_LEN;
                if (0 != uart_Read_Nbytes(pUartCxt->fd, &packet[cur_Ptr], total_Size-IOTD_HEADER_LEN))
                {
                    /* Error */
                    buf_free(packet);
                    break;
                }
                
                pIfaceCxt->recv_count += total_Size;

                /* Fill device ID */
                packet[HTC_HEADER_LEN] = devId | (packet[HTC_HEADER_LEN] & 0x0F);
                /*Send it RX queue*/
                if(IOTD_OK != mml_enqueue(GET_IOTD_CXT(pIfaceCxt), packet, IOTD_DIRECTION_RX))
                {
                    IOTD_LOG(LOG_TYPE_CRIT,"Uart: RX enqueue failed\n");
                    buf_free(packet);
                }
            }
            else
            {
                IOTD_LOG(LOG_TYPE_CRIT,"Uart: RX total size zero\n");
                buf_free(packet);
                break;
            }
        }
    }
    IOTD_LOG(LOG_TYPE_INFO,"Exiting UART RX thread\n");
    pthread_exit(arg);
}


/*
 * Function: uart_TXthread
 *           Dequeue packet from TX queues and send to uart.
 * Input: arg- Interface context pointer
 * Returns:
 *
 */
void* uart_TXthread(void* arg)
{
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)arg;
    UART_CXT_T* pUartCxt = NULL;
    void *buf = NULL;
    uint8_t i = 0, q_id = 0;
    uint16_t len = 0;

    if(!pIfaceCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"Uart TX thread started for target ID %u\n",pIfaceCxt->map.dev_id);

    pUartCxt = &(pIfaceCxt->hif_cxt.uart_cxt);
    /*Block on semaphore, wait for something to be posted to queue*/
    while(1){
        sem_wait(&(pIfaceCxt->tx_sem));

        /*Iterate through all queues to dequeue packets for TX*/
        for(i = 0; i < IOTD_NUM_SQ_ID; i++)
        {
            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pIfaceCxt->map.q_id[i]);
            while((buf = mml_dequeue(GET_IOTD_CXT(pIfaceCxt), q_id, IOTD_DIRECTION_TX)) != NULL)
            {
                /*Read the length from header*/
                len = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(buf);

                IOTD_LOG(LOG_TYPE_VERBOSE, "uart_TXthread: Packet dequeued from queue %d, len: %d\n", i, len);

#ifdef LOOPBACK_TEST
                /*Send it back on RX queue*/
                if(IOTD_OK != mml_enqueue(GET_IOTD_CXT(pIfaceCxt), buf, IOTD_DIRECTION_RX))
                {
                    IOTD_LOG(LOG_TYPE_CRIT,"Uart: TX enqueue failed\n");
                }
#else
                /*Send to the bus*/

                write(pUartCxt->fd, buf, len);
                pIfaceCxt->send_count += len;
                buf_free(buf);
                buf = NULL;
#endif
            }
        }
    }
    pthread_exit(arg);
}


/*
 * Function: uart_Device_Init
 * Input: pCxt- Interface context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
static int32_t uart_Device_Init(void* pCxt)
{
    struct termios serial_Port;
    struct serial_struct serial;
    int32_t baud;
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)pCxt;
    UART_CXT_T* pUartCxt = &(pIfaceCxt->hif_cxt.uart_cxt);

    if((baud = uart_getBaud(pUartCxt->baud)) == -1){
        IOTD_LOG(LOG_TYPE_CRIT, "UART: incorrect baud\n");
        return IOTD_ERROR;
    }

    pUartCxt->fd = open((const char *)pUartCxt->dev, O_RDWR | O_NOCTTY);
    if(-1 == pUartCxt->fd)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Error! in Opening device \n");
        return IOTD_ERROR;
    }

    ioctl(pUartCxt->fd, TIOCGSERIAL, &serial);
    serial.flags |= ASYNC_LOW_LATENCY;
    serial.xmit_fifo_size = 16; 
    ioctl(pUartCxt->fd, TIOCSSERIAL, &serial);

    memset(&serial_Port, 0, sizeof(serial_Port));
    if (0 != tcgetattr(pUartCxt->fd, &serial_Port)) /* Get the current attributes of the Serial port */
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Error in getting existing serial port config\n");
        return IOTD_ERROR;
    }

    if(0 != cfsetispeed(&serial_Port, baud))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"cfsetispeed failed\n");
        return IOTD_ERROR;
    }
    if(0 != cfsetospeed(&serial_Port, baud))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"cfsetospeed failed\n");
        return IOTD_ERROR;
    }

    cfmakeraw(&serial_Port);

    serial_Port.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    serial_Port.c_cflag |= (CLOCAL | CREAD | CS8 );

    /*Enable Flow control if configured*/
    if(pUartCxt->flow_control == 1){
        serial_Port.c_cflag |= CRTSCTS;
    }

    serial_Port.c_oflag &= ~OPOST;
    serial_Port.c_cc[VMIN] = 1;
    serial_Port.c_cc[VTIME] = 0;

    if(0 != tcsetattr(pUartCxt->fd, TCSANOW, &serial_Port))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"ERROR ! in Setting attributes\n");
        return IOTD_ERROR;
    }
    if(0 != tcflush(pUartCxt->fd, TCIOFLUSH)) /* Discards old data in the rx buffer */
    {
        IOTD_LOG(LOG_TYPE_CRIT,"ERROR in flushing IO buffers\n");
        return IOTD_ERROR;
    }
    return IOTD_OK;
}


/*
 * Function: hif_UartInit
 *           Initialize UART interface, start TX/RX threads, configure queues.
 * Input: cxt- iotd context
 *        pIfaceCxt- Interface context
 *        cfg- pointer to system config
 *        index- interface index
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 *
 */
int32_t hif_UartInit(void* cxt, HTC_IFACE_CXT_T* pIfaceCxt, cfg_ini* cfg, uint8_t index)
{
    /*Get device name*/
    memset(pIfaceCxt->hif_cxt.uart_cxt.dev, 0, 32);
    strncpy((char *)pIfaceCxt->hif_cxt.uart_cxt.dev, cfg->config_interface[index].name, strlen(cfg->config_interface[index].name));
    /*Get baud rate*/
    pIfaceCxt->hif_cxt.uart_cxt.baud = cfg->config_interface[index].speed;

    /*Get Flow control*/
    pIfaceCxt->hif_cxt.uart_cxt.flow_control = cfg->config_interface[index].flow_control;

    if(uart_Device_Init(pIfaceCxt) == IOTD_OK)
    {
        sem_init(&(pIfaceCxt->tx_sem),0,1);

        if(pthread_create(&(pIfaceCxt->tx_thread),NULL, uart_TXthread,pIfaceCxt) != 0)
        {
            IOTD_LOG(LOG_TYPE_CRIT,"Uart: Tx thread creation failed, %s\n",strerror(errno));
            exit(1);
        }

        if(pthread_create(&(pIfaceCxt->rx_thread),NULL, uart_RXthread,pIfaceCxt) != 0)
        {
            IOTD_LOG(LOG_TYPE_CRIT,"Uart: Rx thread creation failed, %s\n",strerror(errno));
            exit(1);
        }

        if(IOTD_OK != htc_iface_queue_init(pIfaceCxt))
        {
            IOTD_LOG(LOG_TYPE_CRIT,"Uart: Queue initialization failed\n");
            exit(1);
        }
        return IOTD_OK;
    }
    return IOTD_ERROR;
}

/*
 * Function: hif_UartDeinit
 *           De-initializes UART HIF
 * Input:    Pointer to Interface Context
 * Returns:  IOTD_OK/IOTD_ERROR
 *
 */
int32_t hif_UartDeinit(HTC_IFACE_CXT_T* pIfaceCxt)
{
    UART_CXT_T* pUartCxt = &(pIfaceCxt->hif_cxt.uart_cxt);

    close(pUartCxt->fd);
    pthread_cancel(pIfaceCxt->rx_thread);
    pthread_cancel(pIfaceCxt->tx_thread);
    return IOTD_OK;
}
