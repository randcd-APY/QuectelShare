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
#include <unistd.h>
#include <errno.h>
#include "hif.h"


/*
 * Function:htc_add_header
 * Input: buf- pointer to buffer. 
 *        length- length of packet including HTC and MML headers.
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t htc_add_header(uint8_t* buf, uint16_t length)
{
    if(buf){
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(buf, length);
        return IOTD_OK;
    }

    return IOTD_ERROR;
}

/*
 * Function:htc_iface_queue_init
 * Input: pIfaceCxt- HTC interface context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t htc_iface_queue_init(HTC_IFACE_CXT_T* pIfaceCxt)
{
    int8_t i = 0;
    uint8_t q_id = 0;

    for(i=0; i<pIfaceCxt->map.num_q; i++)
    {
        /* Get queue ID from the registered service QIDs */
        q_id = GET_Q_ID(pIfaceCxt->map.q_id[i]);
        if(IOTD_OK != mml_open_q(pIfaceCxt->pIotdCxt, q_id, IOTD_DIRECTION_TX,  &(pIfaceCxt->tx_sem)))
        {
            return IOTD_ERROR;
        }
    }
    return IOTD_OK;
}

/*
 * Function:htc_init_iface
 * Input: pCxt- IOTD Context, nIface- Number of interfaces
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 *
 */
static int32_t htc_init_iface(void* pCxt, int nIface)
{
    HTC_CXT_T* pHTCCxt = GET_HTC_CXT(pCxt);
    HTC_IFACE_CXT_T* pIfaceCxt;
    uint8_t i,j;
    uint8_t iCount = 0;   /*Active Interface counter*/
    cfg_ini* cfg = GET_CFG(pCxt);

    pHTCCxt->num_iface = 0;
    if(nIface > (IOTD_MAX_INTERFACE - 1))
    {
        return IOTD_ERROR;
    }

    for(i=0;i<nIface;i++)
    {
        if(cfg->config_interface[i].enable != 1){
            /*This interface is not enabled, check the next one*/
            continue;
        }

        if((pHTCCxt->iface_cxt[iCount] = malloc(sizeof(HTC_IFACE_CXT_T))) == NULL)
        {
            return IOTD_ERROR;
        }

        /*This is a valid interface, increment interface count*/
        pHTCCxt->num_iface++;

        pIfaceCxt = pHTCCxt->iface_cxt[iCount];
        pIfaceCxt->pIotdCxt = pCxt;
        
        pIfaceCxt->send_count = 0;
        pIfaceCxt->recv_count = 0;        

        pIfaceCxt->map.dev_id = cfg->config_interface[i].device_id;
        pIfaceCxt->map.num_q = cfg->config_interface[i].num_service_q;

        pIfaceCxt->type = cfg->config_interface[i].type;
        for(j=0;j < pIfaceCxt->map.num_q; j++){
            pIfaceCxt->map.q_id[j] = cfg->config_interface[i].qid[j];
        }

        switch(pIfaceCxt->type)
        {
            case IOTD_IFACE_UART:
                IOTD_LOG(LOG_TYPE_INFO,"HTC: Initializing UART interface for target ID %u\n",pIfaceCxt->map.dev_id);
                if(hif_UartInit(pCxt, pIfaceCxt, cfg, i) != IOTD_OK){
                    IOTD_LOG(LOG_TYPE_CRIT, "HTC: UART Iface init failed\n");
                    return IOTD_ERROR;
                }
                break;

            case IOTD_IFACE_SPI:
                IOTD_LOG(0,"HTC: Initializing SPI interface for target ID %u\n",pIfaceCxt->map.dev_id);
                if(htc_spi_init(pCxt, pIfaceCxt, cfg, i) != IOTD_OK){
                    IOTD_LOG(LOG_TYPE_CRIT, "HTC: SPI Iface init failed\n");
                    return IOTD_ERROR;
                }
                break;

            case IOTD_IFACE_SDIO:
                /*Not supported*/
                break;

            default:
                IOTD_LOG(LOG_TYPE_CRIT,"HTC: Invalid interface type\n");
                return IOTD_ERROR;
                break;
        }

        iCount++;
    }
    return IOTD_OK;
}

/*
 * Function:htc_init
 * Input: pCxt- IOTD Context
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description: Initialize Serial interfaces specified in the daemon config file
 */
int htc_init(void* pCxt)
{
    HTC_CXT_T* pHTCCxt = GET_HTC_CXT(pCxt);
    cfg_ini* cfg = GET_CFG(pCxt);
    uint8_t nIface = cfg->config_system.num_interface;

    pHTCCxt->num_iface = cfg->config_system.num_device;
    IOTD_LOG(LOG_TYPE_INFO,"\n\nInitializing HTC. Number of interfaces:%u\n", nIface);
    return(htc_init_iface(pCxt, nIface));
}


/*
 * Function:htc_deinit
 * Input: pCxt- iotd Context
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description: Deinitialize HTC interfaces
 */
int htc_deinit(void* pCxt)
{
    HTC_CXT_T* pHTCCxt = GET_HTC_CXT(pCxt);
    HTC_IFACE_CXT_T* pIfaceCxt;
    int i;

    for(i = 0;i< pHTCCxt->num_iface; i++){

        if(pHTCCxt->iface_cxt[i]){
            pIfaceCxt = pHTCCxt->iface_cxt[i];
            switch(pIfaceCxt->type)
            {
                case IOTD_IFACE_UART:
                    if(hif_UartDeinit(pIfaceCxt) != IOTD_OK){
                        IOTD_LOG(LOG_TYPE_CRIT, "HTC: UART Iface de-init failed\n");
                        return IOTD_ERROR;
                    }
                    break;

                case IOTD_IFACE_SPI:
                    if(htc_spi_deinit(pIfaceCxt) != IOTD_OK){
                        IOTD_LOG(LOG_TYPE_CRIT, "HTC: SPI Iface de-init failed\n");
                        return IOTD_ERROR;
                    }
                    break;

                    break;

                case IOTD_IFACE_SDIO:
		    /*Future support*/	
                    break;

                default:
                    IOTD_LOG(LOG_TYPE_CRIT,"HTC: Invalid interface type\n");
                    return IOTD_ERROR;
                    break;
            }
            free(pHTCCxt->iface_cxt[i]);
        }
    }
    return IOTD_OK;
}

int htc_get_statistics(HTC_IFACE_CXT_T* pIfaceCxt, long *send_count, long *recv_count)
{
    if( pIfaceCxt == NULL ) return 0;
    if( send_count != NULL ) *send_count = pIfaceCxt->send_count;
    if( recv_count != NULL ) *recv_count = pIfaceCxt->recv_count;
    return 1;
}
