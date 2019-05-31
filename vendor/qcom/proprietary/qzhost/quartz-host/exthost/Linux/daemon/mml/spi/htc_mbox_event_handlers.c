/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include "iotd_context.h"
#include "htc_internal.h"
#include "dbg.h"
#include "iotd_context.h"
#include "bufpool.h"
#include "mml.h"


extern HTC_ENDPOINT_ID endPointId;


/* Function: event_received_Handler
 * Input: target- HTC Target pointer
 *        ep- endpoint ID
 *        eventId- Event ID
 *        eventInfo- pointer to event structure
 *        context- application context 
 * Description:  enqueue received packet into mml queue, upper layer is responsible for releasing the buffer; 
 *  kick spi RX thread to add new receiving buffer into HTC queue.
 *  rx_sem is one count semaphore.
 *
 */

void event_received_Handler(HTC_TARGET *target,
        HTC_ENDPOINT_ID ep, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo, void *context)
{
    SPI_CXT_T *spiCxt;
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)context;
    char* buf = (char*)(eventInfo->buffer)-HTC_HEADER_LEN;
    uint16_t rxLen = eventInfo->actualLength + HTC_HEADER_LEN;
    IOTD_CXT_T* pCxt = (IOTD_CXT_T*) pIfaceCxt->pIotdCxt;

    spiCxt = &pIfaceCxt->hif_cxt.spi_cxt;
    pIfaceCxt->recv_count += rxLen;

    /*Upper layers expect HTC header length included in packet header,
     update the length field*/
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(buf, rxLen);

    if(mml_enqueue(pCxt, buf, IOTD_DIRECTION_RX) != IOTD_OK){
	IOTD_LOG(LOG_TYPE_CRIT,"SPI enq failed\n");
        buf_free(buf);
    }
     
    sem_post(&(spiCxt->rx_sem));
}

/*
 * Function: spi_RXthread
 * Input: arg- Interface Context
 * Returns: Does not return
 * Description: Waits on rx signal, when a packet it received this thread will queue
 *              a new buffer to SPI driver.
 */
void* spi_RXthread(void* arg)
{
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)arg;
    SPI_CXT_T *spiCxt = NULL;
    char *packet = NULL;
    uint8_t devId = 0;

    if(!pIfaceCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"SPI RX thread started\n");

    spiCxt = &pIfaceCxt->hif_cxt.spi_cxt;
    devId = (pIfaceCxt->map.dev_id << 4) & 0xF0;

    while(1)
    {
        sem_wait(&(spiCxt->rx_sem));

        while((packet = (char *)buf_alloc(MAX_BUFFER_SIZE)) == NULL){
            /*Lets wait for buffer to be available*/
            usleep(BUFPOOL_WAIT_TIME_US);
            IOTD_LOG(LOG_TYPE_WARN, "Wait SPI RX buf\n");
        }

        /* Fill device ID */
        packet[HTC_HEADER_LEN] = devId | (packet[HTC_HEADER_LEN] & 0x0F);
        HTCBufferReceive(pIfaceCxt->hif_cxt.spi_cxt.pTarget, endPointId, (uint8_t *)&packet[HTC_HEADER_LEN], MAX_BUFFER_SIZE, pIfaceCxt);
    }
    IOTD_LOG(LOG_TYPE_INFO,"exiting SPI RX thread\n");
    pthread_exit(arg);
}


/*
 * Function:event_sent_Handler
  * Input: target- HTC Target pointer
 *        ep- endpoint ID
 *        eventId- Event ID
 *        eventInfo- pointer to event structure
 *        context- application context 
 * Returns: None
 * Description: Handle a TX completion. Release the buffer back to bufpool.
 */
void event_sent_Handler(HTC_TARGET * target,
        HTC_ENDPOINT_ID ep, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo, void *context)
{
    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)context;    
    uint8_t *pbuf = eventInfo->buffer;
    pbuf -= HTC_HEADER_LEN;

    pIfaceCxt->send_count += READ_UNALIGNED_LITTLE_ENDIAN_UINT16(pbuf);
    buf_free(pbuf);
}


/*
 * Function:spi_TXthread
 * Input: arg- Interface Context
 * Returns: None
 * Description: TX thread is unblocked when someone enqueues a packet to the queues
 *              mapped to SPI interface. The queued packet is sent to SPI HTC layer for transmission.
 */
void* spi_TXthread(void* arg)
{
    HTC_STATUS status;

    HTC_IFACE_CXT_T* pIfaceCxt = (HTC_IFACE_CXT_T*)arg;
    IOTD_CXT_T* iotdCxt = GET_IOTD_CXT(pIfaceCxt);
    void *buf = NULL;
    uint8_t i = 0, q_id = 0;
    uint16_t len = 0;

    if(!pIfaceCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"SPI TX thread started\n");

    /*Block on semaphore, wait for something to be posted to queue*/
    while(1){
        sem_wait(&(pIfaceCxt->tx_sem));

        /*Iterate through all queues to dequeue packets for TX*/
        for(i = 0; i < IOTD_NUM_SQ_ID; i++)
        {
            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pIfaceCxt->map.q_id[i]);
            while((buf = mml_dequeue(iotdCxt, q_id, IOTD_DIRECTION_TX)) != NULL)
            {
                len = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(buf);

                /*Send to the bus*/
                len -= HTC_HEADER_LEN;
                status = HTCBufferSend(pIfaceCxt->hif_cxt.spi_cxt.pTarget, endPointId, ((uint8_t *)buf)+HTC_HEADER_LEN, len, NULL);
                if (status != HTC_OK)
                {
                    IOTD_LOG(LOG_TYPE_CRIT, "spi_TXthread: sending error\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    pthread_exit(arg);
}
