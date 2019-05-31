/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <syslog.h>
#include "iotd_context.h"
#include "mml.h"
#include "bufpool.h"
#include "htc_internal.h"
#include "htc.h"

/********************************************************/
/* This module interfaces with the low level SPI HTC interface.
   It invokes HTC APIs to setup the SPI interface and registers
   callbacks for various events. 
*/
/********************************************************/

HTC_ENDPOINT_ID endPointId = 0;

/*
 * Function:htc_spi_init
 * Input: pCxt- IOTD Context
 *        pIfaceCxt- HTC Interface Context
 *        cfg_ini- pointer to config data 
 *        Id- Device ID
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description: Initialize the SPI Interface(s). Start the TX and RX thread, 
 *              registers callbacks for events from lower layers.
 */
int32_t htc_spi_init(void* pCxt, HTC_IFACE_CXT_T* pIfaceCxt, cfg_ini* cfg, uint8_t Id)
{
    SPI_CXT_T  *spiCxt;
    HTC_TARGET *appTarget;
    HTC_STATUS  status;

    if(Id > (IOTD_MAX_DEVICES - 1))
    {
        return IOTD_ERROR;
    }

    /*************************************************/
    /*fill SPI iface structure with information from config file*/
    spiCxt = &pIfaceCxt->hif_cxt.spi_cxt;

    strcpy((char *)spiCxt->dev, cfg->config_interface[Id].name);
    spiCxt->baud = cfg->config_interface[Id].speed;
    spiCxt->block_size = cfg->config_interface[Id].block_size;
    spiCxt->spi_intr_pin_num = cfg->config_interface[Id].intr_gpio;

    IOTD_LOG(LOG_TYPE_INFO,"SPI dev name:%s\n", spiCxt->dev);
    IOTD_LOG(LOG_TYPE_INFO,"SPI speed:%d bytes/per sec\n", spiCxt->baud);
    IOTD_LOG(LOG_TYPE_INFO,"SPI block size:%d\n", spiCxt->block_size);
    IOTD_LOG(LOG_TYPE_INFO,"SPI Interrupt pin#:%d\n", spiCxt->spi_intr_pin_num);

    sem_init(&(spiCxt->rx_sem),0, HTC_SPI_RX_BUF);     // 2 buffers for receive

    status = HTCInit(spiCxt);
    if (status != HTC_OK)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"HTCInit failed %d\n",status);
        return IOTD_ERROR;
    }

    /*Obtain HTC target instance*/
    appTarget = htcTargetInstance(HTC_TARGET_INSTANCE_0);

    /*Register callback for HTC_EVENT_BUFFER_SENT event. On receiving this event, the 
     TX buffer is released to buffer pool.*/ 
    status = HTCEventReg(appTarget,  endPointId,
            HTC_EVENT_BUFFER_SENT, event_sent_Handler, pIfaceCxt);

    /*Register callback for HTC_EVENT_BUFFER_RECEIVED event. On receiving this event, the 
     RX buffer is queued to corresponding MML queue.*/
    status = HTCEventReg(appTarget,  endPointId,
            HTC_EVENT_BUFFER_RECEIVED, event_received_Handler, pIfaceCxt);

    status = HTCStart(appTarget);

    pIfaceCxt->hif_cxt.spi_cxt.pTarget = appTarget;

    sem_init(&(pIfaceCxt->tx_sem),0,1);

    /*Create TX and RX threads for handling Packet TX/RX*/
    if(pthread_create(&(pIfaceCxt->tx_thread),NULL, spi_TXthread,pIfaceCxt) != 0)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"tx thread creation failed %s\n",strerror(errno));
        exit(1);
    }

    if(pthread_create(&(pIfaceCxt->rx_thread),NULL, spi_RXthread,pIfaceCxt) != 0)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"rx thread creation failed %s\n",strerror(errno));
        exit(1);
    }

    htc_iface_queue_init(pIfaceCxt);

    return IOTD_OK;
}

/*
 * Function:hif_spi_deinit
 * Input: pIfaceCxt- Interface Context
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 *
 */
int32_t htc_spi_deinit(HTC_IFACE_CXT_T* pIfaceCxt)
{
    HTC_TARGET *appTarget;

    appTarget = htcTargetInstance(HTC_TARGET_INSTANCE_0);

    if(pIfaceCxt->tx_thread)
        pthread_cancel(pIfaceCxt->tx_thread);

    if(pIfaceCxt->rx_thread)
        pthread_cancel(pIfaceCxt->rx_thread);

    /*Stop the HTC instance and clean up all resources*/
    HTCStop(appTarget);

    return IOTD_OK;
}
