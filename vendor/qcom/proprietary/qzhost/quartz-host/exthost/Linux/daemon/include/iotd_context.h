/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _IOTD_CXT_H_
#define _IOTD_CXT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include "queue.h"
#include "semaphore.h"
#include "parse_config.h"
#include "qapi_firmware_upgrade_ext.h"
#include "iotd_dbg.h"

#define IOTD_MAX_CLIENTS           (10)  /*Maximum nunber of client apps*/
#define IOTD_MAX_DEVICES           MAX_DEVICE_NUM   /*Maximum number of Quartz devices*/

/* Service IDs */
#define IOTD_SQ_ID_QAPI            0
#define IOTD_SQ_ID_BLE_DATA        1
#define IOTD_SQ_ID_WLAN_DATA       2
#define IOTD_SQ_ID_THREAD_DATA     3
#define IOTD_SQ_ID_MGMT            4
#define IOTD_SQ_ID_DEBUG           5
#define IOTD_NUM_SQ_ID             MAX_PER_DEVICE_SQ_NUM   /*Number of service queue IDs*/
#define IOTD_MAX_NUM_Q             MAX_TOTAL_SQ_NUM

#define HTC_HEADER_LEN             (4)
#define MML_HEADER_LEN             (6)
#define IOTD_HEADER_LEN            (HTC_HEADER_LEN + MML_HEADER_LEN)
#define IOTD_HEADER_COOKIE_OFFSET  (HTC_HEADER_LEN + 2)
#define IOTD_MAX_DATA_INTERFACE    (5)
#define IOTD_MAX_INTERFACE         (32)

#define IOTD_OK                    (0)
#define IOTD_ERROR                 (-1)
#define MAC_ADDR_LEN               (6)



typedef enum queue_dir
{
    IOTD_DIRECTION_TX,
    IOTD_DIRECTION_RX,
} QUEUE_DIR_E;


typedef enum iface_type
{
    IOTD_IFACE_UART,
    IOTD_IFACE_SPI,
    IOTD_IFACE_SDIO,
} IFACE_TYPE_E;


typedef struct _serviceq_map
{
    uint8_t dev_id;               /*Quartz Target device ID*/
    uint8_t q_id[IOTD_MAX_NUM_Q]; /*Queues owned by this interface*/
    uint8_t num_q;                /*Number of queues associated with the interface*/

} SERVICEQ_MAP_T;

typedef struct _uart_cxt
{
    uint8_t dev[32];      /*UART dev e.g. /dev/ttyUSB0*/
    uint32_t baud;        /*Configured baud rate*/
    int32_t fd;           /*File desciptor of the UART device*/
    uint8_t flow_control; /*Uart Flow control*/
} UART_CXT_T;

typedef struct _spi_cxt
{
    void  *pTarget;     /* HTC instance context pointer */
    sem_t rx_sem;       /* HTC receive buffer semaphore */
    uint8_t dev[32];	/*UART dev e.g. /dev/ttyUSB0*/
    uint32_t baud;			/*Configured baud rate*/
    uint32_t block_size;	/*Configured block size*/
    uint16_t spi_intr_pin_num;  
} SPI_CXT_T;

typedef struct _hif_cxt
{
    UART_CXT_T uart_cxt;
    SPI_CXT_T spi_cxt;

} HIF_CXT_T;

typedef struct _htc_iface_cxt
{
    HIF_CXT_T hif_cxt; /*Context for Host interface (UART/SPI/SDIO*/
    sem_t tx_sem;
    pthread_t tx_thread;
    pthread_t rx_thread;
    SERVICEQ_MAP_T map;
    IFACE_TYPE_E type;
    void* pIotdCxt; /*Backward pointer to iotd context*/
    long send_count;
    long recv_count;
} HTC_IFACE_CXT_T;

typedef struct _htc_cxt
{
    HTC_IFACE_CXT_T* iface_cxt[IOTD_MAX_DEVICES]; /*Array of HTC context pointers*/
    uint8_t num_iface;
}HTC_CXT_T;

typedef struct _serviceq
{
    uint8_t sid;/*Service Queue ID*/
    QUEUE_T q;
} SERVICEQ_T;

typedef struct _mml_cxt
{
    SERVICEQ_T sq[IOTD_MAX_NUM_Q][2];  /*There are two service queues per ID. To-Do- Make this a dynamic allocation based on config file*/
    uint16_t num_queue;   /*Number of service queues in the daemon*/
} MML_CXT_T;

typedef struct _ipc_client
{
    mqd_t handle;                /*Client queue handle*/
    uint8_t client_q[32];        /*mqueue name */
    uint8_t in_use;              /*indicates if entry is in use*/
    uint32_t cookie;             /*Used to identify client process*/
} IPC_CLIENT_T;

typedef struct _ipc_cxt
{
    mqd_t iotd_server;   /*iotd server queue handle*/
    IPC_CLIENT_T iotd_client[IOTD_MAX_CLIENTS];  /*iotd client queue handles*/
    char  server_q[32];
    unsigned short max_msgs;
    unsigned short max_msg_sz;
    unsigned short msg_bug_sz;
    pthread_t tx_thread;
    pthread_t rx_thread;
    sem_t rx_sem;
    SERVICEQ_MAP_T map;
    void* iotd_cxt;      /* Backwards pointer to context*/
} IPC_CXT_T;

typedef struct _data_interface
{
    char name[32];        /*Interface name string*/
    uint32_t id;          /*ID- assigned when interface is created*/
    uint8_t mac[MAC_ADDR_LEN]; /*TAP interface mac address*/
    uint8_t name_len;     /*Size of interface name string*/
    uint8_t type;         /*Type- TUN or TAP*/
    uint8_t in_use;
    uint32_t ifindex;    /*Interface index*/
    uint8_t sq_id;
    uint8_t devId;        /*QCA402X device ID, in case of multiple device attahc*/  
    pthread_t rx_thread;  /*Thread to listen on interface*/
    uint32_t mcast_fd;    /*Fd for multicast UDP socket*/
    QUEUE_T mcast_q;      /*Queue to store mcast subscriptions*/
    QUEUE_T route_q;      /*Queue to store added routes*/
    QUEUE_T ipaddr_q;
    void* arg;
    void* iotd_cxt;       /*Backwards pointer to context*/
}DATA_INTERFACE_T;

typedef struct _dataMgr_context
{
    pthread_t mgmt_thread;  /*Data Mgr command handler thread*/
    SERVICEQ_MAP_T map;
    sem_t rx_sem;
    DATA_INTERFACE_T interface[IOTD_MAX_DATA_INTERFACE];
    void* iotd_cxt;       /*Backwards pointer to context*/
} DATAMGR_CXT_T;

typedef struct _iotdMgmt_context
{
    pthread_t   mgmt_thread;   /* MGMT handler thread*/
    timer_t     timer_id;    
    uint8_t     timer_start;   /* timer flag */
    uint8_t     target_initialized;  /* Set when hello response is received from target */
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    SERVICEQ_MAP_T  map;
    sem_t rx_sem;
    void* iotd_cxt;       /*Backwards pointer to context*/
    qapi_FW_Ver_t target_ver;   /* target version info */
    uint8_t target_status;      /* target status */
    uint8_t *buf_s, *buf_r; /*throughput test */ 
    int buf_len, recv_len;  /*throughput test */ 
    int run_once;           /*throughput test */ 
    int test_mode;          /*throughput test */ 
    long send_count;         /*throughput test */ 
    long recv_count;        /*throughput test */ 
} IOTDMGMT_CXT_T;

typedef struct _iotd_context
{
    cfg_ini  cfg;
    char* cfg_file;        /*Config file location, passed by user*/
    uint16_t num_devices;

    IPC_CXT_T ipc_cxt;     /*IPC Context*/
    IPC_CXT_T diag_cxt;
    MML_CXT_T mml_cxt;     /*MML layer Context*/
    HTC_CXT_T htc_cxt;     /*HTC Context*/
    DATAMGR_CXT_T datamgr_cxt; /*Data Mgr Context*/
    IOTDMGMT_CXT_T mgmt_cxt;  /*Iotd Management Context*/
} IOTD_CXT_T;

#define GET_MGMT_CXT(p)     (p ? &(((IOTD_CXT_T*)p)->mgmt_cxt) : NULL)
#define GET_DATAMGR_CXT(p)  (p ? &(((IOTD_CXT_T*)p)->datamgr_cxt) : NULL)
#define GET_IPC_CXT(p)      (p ? &(((IOTD_CXT_T*)p)->ipc_cxt) : NULL)
#define GET_DIAG_CXT(p)     (p ? &(((IOTD_CXT_T*)p)->diag_cxt) : NULL)
#define GET_MML_CXT(p)      (p ? &(((IOTD_CXT_T*)p)->mml_cxt) : NULL)
#define GET_HTC_CXT(p)      (p ? &(((IOTD_CXT_T*)p)->htc_cxt) : NULL)
#define GET_IOTD_CXT(p)     (p ? ((HTC_IFACE_CXT_T*)p)->pIotdCxt : NULL)
#define GET_CFG(p)          (p ? &(((IOTD_CXT_T*)p)->cfg) : NULL)

/* Queues for deviceId 0 from 0 to (IOTD_NUM_SQ_ID - 1).
 * Queues for deviceId 1 from IOTD_NUM_SQ_ID and so on
 */
#define GET_Q_ID(p)         ((p & 0x0F) + (((p & 0xF0) >> 4) * IOTD_NUM_SQ_ID))
#define GET_SQ_ID(p)        (p & 0x0F)
#define GET_DEV_ID(p)       ((p & 0xF0) >> 4)
#define GET_TOTAL_SIZE(p)   (p ? *((uint16_t*)p) : 0)
#define GET_PAYLOAD_SIZE(p) ((p >= IOTD_HEADER_LEN) ? (p - IOTD_HEADER_LEN): (IOTD_ERROR))
#define IS_Q_DIR_VALID(p)   (((p == IOTD_DIRECTION_TX) || (p == IOTD_DIRECTION_RX)) ? 1 : 0)



/**
  Converts a host-ordered, 8-bit integer to an unalgined, little-endian
  integer.

  @param __dest__ Pointer to where the unaligned value should be written.
  @param __src__  Integer value to write.
  */
#define WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(__dest__, __src__) \
    do {                                                           \
        ((uint8_t *)(__dest__))[0] = ((uint8_t)(__src__));           \
    } while(0)

/**
  Converts a host-ordered, 16-bit integer to an unalgined, little-endian
  integer.

  @param __dest__ Pointer to where the unaligned value should be written.
  @param __src__  Integer value to write.
  */
#define WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(__dest__, __src__)                  \
    do {                                                                             \
        ((uint8_t *)(__dest__))[0] = ((uint8_t)(((uint16_t)(__src__)) & 0xFF));        \
        ((uint8_t *)(__dest__))[1] = ((uint8_t)((((uint16_t)(__src__)) >> 8) & 0xFF)); \
    } while(0)

/**
  Converts a host-ordered, 32-bit integer to an unalgined, little-endian
  integer.

  @param __dest__ Pointer to where the unaligned value should be written.
  @param __src__  Integer value to write.
  */
#define WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(__dest__, __src__)                   \
    do {                                                                              \
        ((uint8_t *)(__dest__))[0] = ((uint8_t)(((uint32_t)(__src__)) & 0xFF));         \
        ((uint8_t *)(__dest__))[1] = ((uint8_t)((((uint32_t)(__src__)) >> 8) & 0xFF));  \
        ((uint8_t *)(__dest__))[2] = ((uint8_t)((((uint32_t)(__src__)) >> 16) & 0xFF)); \
        ((uint8_t *)(__dest__))[3] = ((uint8_t)((((uint32_t)(__src__)) >> 24) & 0xFF)); \
    } while(0)

/**
  Converts a host-ordered, 64-bit integer to an unalgined, little-endian
  integer.

  @param __dest__ Pointer to where the unaligned value should be written.
  @param __src__  Integer value to write.
  */
#define WRITE_UNALIGNED_LITTLE_ENDIAN_UINT64(__dest__, __src__)                   \
    do {                                                                              \
        ((uint8_t *)(__dest__))[0] = ((uint8_t)(((uint64_t)(__src__)) & 0xFF));         \
        ((uint8_t *)(__dest__))[1] = ((uint8_t)((((uint64_t)(__src__)) >> 8) & 0xFF));  \
        ((uint8_t *)(__dest__))[2] = ((uint8_t)((((uint64_t)(__src__)) >> 16) & 0xFF)); \
        ((uint8_t *)(__dest__))[3] = ((uint8_t)((((uint64_t)(__src__)) >> 24) & 0xFF)); \
        ((uint8_t *)(__dest__))[4] = ((uint8_t)((((uint64_t)(__src__)) >> 32) & 0xFF)); \
        ((uint8_t *)(__dest__))[5] = ((uint8_t)((((uint64_t)(__src__)) >> 40) & 0xFF)); \
        ((uint8_t *)(__dest__))[6] = ((uint8_t)((((uint64_t)(__src__)) >> 48) & 0xFF)); \
        ((uint8_t *)(__dest__))[7] = ((uint8_t)((((uint64_t)(__src__)) >> 56) & 0xFF)); \
    } while(0)


/**
   Converts an unaligned, little-endian 8-bit integer to an aligned,
   host-ordered value.

   @param __src__ Pointer to the unaligned, big-endian value.

   @return The aligned, host ordered value.
*/
#define READ_UNALIGNED_LITTLE_ENDIAN_UINT8(__src__) \
   (((uint8_t *)(__src__))[0])

/**
   Converts an unaligned, little-endian 16-bit integer to an aligned,
   host-ordered value.

   @param __src__ Pointer to the unaligned, big-endian value.

   @return The aligned, host ordered value.
*/
#define READ_UNALIGNED_LITTLE_ENDIAN_UINT16(__src__)            \
   ((uint16_t)((((uint16_t)(((uint8_t *)(__src__))[1])) << 8) | \
                ((uint16_t)(((uint8_t *)(__src__))[0]))))

/**
   Converts an unaligned, little-endian 32-bit integer to an aligned,
   host-ordered value.

   @param __src__ Pointer to the unaligned, big-endian value.

   @return The aligned, host ordered value.
*/
#define READ_UNALIGNED_LITTLE_ENDIAN_UINT32(__src__)             \
   ((uint32_t)((((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
               (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
               (((uint32_t)(((uint8_t *)(__src__))[1])) << 8)  | \
                ((uint32_t)(((uint8_t *)(__src__))[0]))))


/**
   Converts an unaligned, little-endian 64-bit integer to an aligned,
   host-ordered value.

   @param __src__ Pointer to the unaligned, big-endian value.

   @return The aligned, host ordered value.
*/
#define READ_UNALIGNED_LITTLE_ENDIAN_UINT64(__src__)             \
   ((uint64_t)((((uint64_t)(((uint8_t *)(__src__))[7])) << 56) | \
               (((uint64_t)(((uint8_t *)(__src__))[6])) << 48) | \
               (((uint64_t)(((uint8_t *)(__src__))[5])) << 40) | \
               (((uint64_t)(((uint8_t *)(__src__))[4])) << 32) | \
               (((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
               (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
               (((uint32_t)(((uint8_t *)(__src__))[1])) << 8)  | \
                ((uint32_t)(((uint8_t *)(__src__))[0]))))

/**
   Converts a host-ordered, 8-bit integer to an unalgined, big-endian integer.

   @param __dest__ Pointer to where the unaligned value should be written.
   @param __src__  Integer value to write.
*/
#define WRITE_UNALIGNED_BIG_ENDIAN_UINT8(__dest__, __src__) \
do {                                                        \
  ((uint8_t *)(__dest__))[0] = ((uint8_t)(_y));             \
} while(0)

#endif
