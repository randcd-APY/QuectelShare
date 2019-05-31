#ifndef CSR_BT_AV2_FILTER_H__
#define CSR_BT_AV2_FILTER_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_av_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_AV_SBC_DEFAULT_FILE_IN    "sbc_test_02.sbc"
#define CSR_BT_AV_SBC_DEFAULT_FILE_OUT   "csr_dump_sbc" 
#define CSR_BT_AV_AAC_DEFAULT_FILE_IN    "csr.aac"
#define CSR_BT_AV_MP3_DEFAULT_FILE_IN    "csr.mp3"
#define CSR_BT_AV_WAV_DEFAULT_FILE_IN    "csr.wav"
#define CSR_BT_AV_WAV_DEFAULT_FILE_OUT   "csr_dump.wav"
#define CSR_BT_AV_DUMP_DEFAULT_FILE_OUT  "av2_dump.raw"

#if defined(AUDIO_FOR_MDM9650) || defined(AUDIO_FOR_IMX)
#define CSR_BT_AV_ALSA_DEFAULT_DEVICE    "hw:0,0"
#else
#define CSR_BT_AV_ALSA_DEFAULT_DEVICE    "plughw:0,0"
#endif

/* Filter types */
#define FILTER_PRODUCER    0x01  /* Filter produces something */
#define FILTER_CONSUMER    0x02  /* Filter consumes something */
#define FILTER_ENCODER     0x04  /* Filter delivers encoded data */
#define FILTER_DECODER     0x08  /* Filter delivers raw (pcm) data */
#define FILTER_PASS        0x10  /* Data travels through filter */

/* Media/codec types for internal demo filters */
#define AV_NO_MEDIA        0xFF
#define AV_NO_CODEC        0xFF

/* Filter states */
#define FILTER_ST_NOINIT   0     /* Filter is un-initialised */
#define FILTER_ST_IDLE     1     /* Filter initialized */
#define FILTER_ST_OPEN     2     /* Filter has been opened */
#define FILTER_ST_START    3     /* Filter has been started */

/* Opcodes for simple filter runners */
#define FILTER_INIT        0     /* Run initialise */
#define FILTER_DEINIT      1     /* Run deinitialise */
#define FILTER_OPEN        2     /* Open or reconfig */
#define FILTER_CLOSE       3     /* Close */
#define FILTER_START       4     /* Start */
#define FILTER_STOP        5     /* Stop */

/*
 * Forward declaration of struct type.  av2filter_t
 * and av2instance_t are mutually recursive.
 */
struct av2instance_t;

/* Definition of the filter type */
struct av2filter_t
{
    /* Static filter identification */
    char *f_name;                                    /* Static filter name */
    CsrUint8 f_type;                                  /* Static filter type */
    CsrBtAvMedia f_media;                              /* A2DP media type */
    CsrBtAvCodec f_codec;                              /* A2DP codec type */
    CsrUint8 f_state;                                 /* Filter state */
    void *f_instance;                                /* Filter instance data */

    /* Initialise/shutdown */
    CsrBool (*f_init)(void **instance,                /* Called when initialising demo */
                     struct av2filter_t *filter,
                     struct av2instance_t *av2inst); /*   filter points to the static filter definition */
    CsrBool (*f_deinit)(void **instance);             /* Called when demo is shutting down */

    /* Open/close media, start/stop stream */
    CsrBool (*f_open)(void **instance);               /* Open device or (re)prepare current config */
    CsrBool (*f_close)(void **instance);              /* Close device */
    CsrBool (*f_start)(void **instance);              /* Start streaming */
    CsrBool (*f_stop)(void **instance);               /* Stop streaming */

    /* Real-time processing */
    CsrBool (*f_process)(CsrUint8 index,               /* Process/produce/consume data */
                        struct av2filter_t **chain,  /*   Use data/length if possible. Free 'data' */
                        CsrBool freeData,             /*   if present and if 'freeData' is set. Call */
                        void *data,                  /*   next filter in chain with index+1 like this */
                        CsrUint32 length);            /*   chain[index+1]->f_process(index+1, ...)*/

    /* Stream configuration */
    CsrUint8 *(*f_get_config)(void **instance,        /* Get configuration */
                            CsrUint8 *conf_len);      /*   Return pointer and store length in conf_len */

    CsrBtAvResult (*f_set_config)(void **instance,     /* Set configuration */
                               CsrUint8 *conf,        /*   New configuration is in conf/conf_len.*/
                               CsrUint8 conf_len);    /*   Return result code */

    CsrUint8 *(*f_get_caps)(void **instance,          /* Get service capabilities of this filter*/
                          CsrUint8 *cap_len);         /*   Return pointer and store length in cap_len */

    CsrUint8 *(*f_remote_caps)(void **instance,       /* Remote capabilities received. */
                             CsrUint8 *caps,          /*   Feed remote caps into function, and select */
                             CsrUint8 caps_len,       /*   the optimal configuration. Return "optimal config" */
                             CsrUint8 *conf_len);     /*   pointer and store length in *conf_len */

    void (*f_qos_update)(void **instance,            /* QoS buffer status level received. Called periodically */
                        CsrUint16 qosLevel);          /*   with 'qosLevel' buffer status ranging from 0 to 10 */


    /* User preferences */
    void (*f_menu)(void **instance,                  /* Print user-definable options to screen. Entries */
                   CsrUint8 *num_options);            /*    start at '0'. Return total number of entries */
    CsrBool (*f_setup)(void **instance,               /* User changed setting defined by index. */
                      CsrUint8 index,                 /*    User input is available as a null-terminated */
                      char *value);                  /*    string -- CsrPmemFree when done, return accepted true/false */
    char *(*f_return_filename)(void **instance);
};
typedef struct av2filter_t av2filter_t;

#ifdef __cplusplus
}
#endif

#endif

