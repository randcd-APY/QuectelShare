#ifndef CSR_AM_PRIM_H__
#define CSR_AM_PRIM_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrAmPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrAmPrim;
typedef CsrUint16 CsrAmEndpointType;
typedef CsrUint16 CsrAmAudioType;

/* AM result/error codes */
#define CSR_AM_RESULT_UNSUPPORTED            ((CsrResult) 0x0001)
#define CSR_AM_RESULT_ERROR_SINK_BUSY        ((CsrResult) 0x0002)
#define CSR_AM_RESULT_ERROR_SRC_BUSY         ((CsrResult) 0x0003)
#define CSR_AM_RESULT_CONFIG_INVALID         ((CsrResult) 0x0004)
#define CSR_AM_RESULT_UNKNOWN_ID             ((CsrResult) 0x0005)
#define CSR_AM_RESULT_WRONG_SINK             ((CsrResult) 0x0006)
#define CSR_AM_RESULT_UNKNOWN_CONNECTION     ((CsrResult) 0x0007)
#define CSR_AM_RESULT_INVALID_PARAMETER      ((CsrResult) 0x0008)

/* AM endpoint type */
#define CSR_AM_EP_SOURCE_LEFT                     ((CsrAmEndpointType) 0x0000)
#define CSR_AM_EP_SOURCE_RIGHT                    ((CsrAmEndpointType) 0x0001)
#define CSR_AM_EP_SOURCE_ALL                      ((CsrAmEndpointType) 0x0002)
#define CSR_AM_EP_SINK_LEFT                       ((CsrAmEndpointType) 0x0003)
#define CSR_AM_EP_SINK_RIGHT                      ((CsrAmEndpointType) 0x0004)
#define CSR_AM_EP_SINK_BOTH_SIDES                 ((CsrAmEndpointType) 0x0005)
#define CSR_AM_EP_ALL_LEFT_SINKS                  ((CsrAmEndpointType) 0x0006)
#define CSR_AM_EP_ALL_RIGHT_SINKS                 ((CsrAmEndpointType) 0x0007)
#define CSR_AM_EP_ALL_SINKS_BOTH_SIDES            ((CsrAmEndpointType) 0x0008)
#define CSR_AM_EP_ALL_LEFT                        ((CsrAmEndpointType) 0x0009)
#define CSR_AM_EP_ALL_RIGHT                       ((CsrAmEndpointType) 0x000A)
#define CSR_AM_EP_ALL_BOTH_SIDES                  ((CsrAmEndpointType) 0x000B)

/* AM endpoint definitions */
#define CSR_AM_ENDPOINT_PCM                  (0x0001)
#define CSR_AM_ENDPOINT_I2S                  (0x0002)
#define CSR_AM_ENDPOINT_CODEC                (0x0003)
#define CSR_AM_ENDPOINT_FM                   (0x0004)
#define CSR_AM_ENDPOINT_ALL                  (0xFFFF)

/* AM endpoint instance definitions */
#define CSR_AM_INSTANCE_0                    (0x0000)
#define CSR_AM_INSTANCE_1                    (0x0001)
#define CSR_AM_NUMBER_OF_INSTANCES           (0x0002)
#define CSR_AM_INSTANCE_AUTO                 (0xFFFF)

/* AM endpoint slot/channel definitions */
#define CSR_AM_CHANNEL_0                     (0x0000)
#define CSR_AM_CHANNEL_1                     (0x0001)
#define CSR_AM_NUMBER_OF_CHANNELS            (0x0002)
#define CSR_AM_CHANNEL_AUTO                  (0xFFFF)

/* AM audio type definitions*/
#define CSR_AM_FM_AUDIO                      ((CsrAmAudioType) 0x0000)
#define CSR_AM_WBS_AUDIO                     ((CsrAmAudioType) 0x0001)
#define CSR_AM_CVSD_AUDIO                    ((CsrAmAudioType) 0x0002)
#define CSR_AM_AURISTREAM_AUDIO              ((CsrAmAudioType) 0x0003)

/* AM audio codec definitions */
#define CSR_AM_WBS_CVSD_CODEC                ((CsrUint8) 0x01)
#define CSR_AM_WBS_MSBC_CODEC                ((CsrUint8) 0x02)

/* Default values in case of error */
#define CSR_AM_NO_CONN_ID                    ((CsrUint16) 0xFFFF)

/* Audio stream configuration keys */
#define CSR_AM_PCM_SYNC_RATE                 ((CsrUint16) 0x0100)
#define CSR_AM_PCM_MASTER_CLOCK_RATE         ((CsrUint16) 0x0101)
#define CSR_AM_PCM_MASTER_MODE               ((CsrUint16) 0x0102)
#define CSR_AM_PCM_SLOT_COUNT                ((CsrUint16) 0x0103)
#define CSR_AM_I2S_SYNC_RATE                 ((CsrUint16) 0x0200)
#define CSR_AM_I2S_MASTER_CLOCK_RATE         ((CsrUint16) 0x0201)
#define CSR_AM_I2S_MASTER_MODE               ((CsrUint16) 0x0202)
#define CSR_AM_CODEC_ADC_RATE                ((CsrUint16) 0x0300)
#define CSR_AM_CODEC_DAC_RATE                ((CsrUint16) 0x0301)
#define CSR_AM_CODEC_ADC_GAIN                ((CsrUint16) 0x0302)
#define CSR_AM_CODEC_DAC_GAIN                ((CsrUint16) 0x0303)
#define CSR_AM_FM_INPUT_RATE                 ((CsrUint16) 0x0400)
#define CSR_AM_FM_OUTPUT_RATE                ((CsrUint16) 0x0401)
#define CSR_AM_FM_INPUT_GAIN                 ((CsrUint16) 0x0402)
#define CSR_AM_FM_OUTPUT_GAIN                ((CsrUint16) 0x0403)

/*******************************************************************************
 * Structure definitions
 *******************************************************************************/
typedef struct
{
    CsrUint16 endPoint;
    CsrUint16 instance;
    CsrUint16 channel;
} CsrAmEndPointDefType;

typedef struct
{
    CsrAmEndPointDefType endPoint;
    CsrUint8             configDataCount;
    CsrUint32           *configData;
} CsrEndpointType;

typedef struct
{
    CsrUint8  sinkIndex;
    CsrUint32 key;
    CsrUint32 value;
} CsrAmSinkConfigType;


typedef struct
{
    CsrUint8              sinkCount;                 /* Number of sink entries in the 'sinkId' field below */
    CsrAmEndPointDefType *newSinkId;                 /* List of sink endpoint: FM, codec, pcm,...          */
    CsrUint8              sinkConfigCount;           /* Number of sink configuration elements              */
    CsrAmSinkConfigType  *sinkConfig;                /* Sink configuration elements containing sink index, key and value              */
} CsrAmSinksToAddType;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_AM_PRIM_DOWNSTREAM_LOWEST               (0x0000)

#define CSR_AM_INIT_REQ                             ((CsrAmPrim) (0x0000 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_CONNECT_REQ               ((CsrAmPrim) (0x0001 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_ADD_SINK_REQ              ((CsrAmPrim) (0x0002 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_REMOVE_SINK_REQ           ((CsrAmPrim) (0x0003 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_CONFIG_REQ                ((CsrAmPrim) (0x0004 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_RELEASE_REQ               ((CsrAmPrim) (0x0005 + CSR_AM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_AM_PRIM_DOWNSTREAM_HIGHEST              (0x0005 + CSR_AM_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_AM_PRIM_UPSTREAM_LOWEST                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_AM_INIT_CFM                             ((CsrAmPrim) (0x0000 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_CONNECT_CFM               ((CsrAmPrim) (0x0001 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_ADD_SINK_CFM              ((CsrAmPrim) (0x0002 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_REMOVE_SINK_CFM           ((CsrAmPrim) (0x0003 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_CONFIG_CFM                ((CsrAmPrim) (0x0004 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_RELEASE_CFM               ((CsrAmPrim) (0x0005 + CSR_AM_PRIM_UPSTREAM_LOWEST))
#define CSR_AM_AUDIO_PATH_RELEASE_IND               ((CsrAmPrim) (0x0006 + CSR_AM_PRIM_UPSTREAM_LOWEST))

#define CSR_AM_PRIM_UPSTREAM_HIGHEST                (0x0006 + CSR_AM_PRIM_UPSTREAM_LOWEST)

#define CSR_AM_PRIM_DOWNSTREAM_COUNT                (CSR_AM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_AM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_AM_PRIM_UPSTREAM_COUNT                  (CSR_AM_PRIM_UPSTREAM_HIGHEST + 1 - CSR_AM_PRIM_UPSTREAM_LOWEST)

/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrAmPrim   type;                           /* Event identifier                                   */
    CsrSchedQid pHandle;                        /* Handle to application                              */
} CsrAmInitReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 buildId;                          /* Firmware build Id read from the chip               */
} CsrAmInitCfm;


typedef struct
{
    CsrAmPrim           type;                        /* Event identifier                                   */
    CsrSchedQid         pHandle;                     /* Handle to application                              */
    CsrEndpointType     source;                      /* Type of source endpoint: FM, codec, pcm,...        */
    CsrEndpointType     sourceR;                     /* Type of source endpoint for the right side of an stereo connection, if needed */
    CsrAmSinksToAddType sinks;                       /* Structure wth the sinks to add                     */
    CsrAmAudioType      audioType;                   /* Audio type: FM, WBS, CVSD, Auristream              */
    CsrBool             stereo;                      /* If TRUE: both right and left side of each
                                                        endpoint need to be handled                        */
} CsrAmAudioPathConnectReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathConnectCfm;

typedef struct
{
    CsrAmPrim           type;                        /* Event identifier                                   */
    CsrSchedQid         pHandle;                     /* Handle to application                              */
    CsrUint16           amConnectionId;              /* Connection identifier                              */
    CsrAmSinksToAddType sinks;
} CsrAmAudioPathAddSinkReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathAddSinkCfm;


typedef struct
{
    CsrAmPrim            type;                       /* Event identifier                                   */
    CsrSchedQid          pHandle;                    /* Handle to application                              */
    CsrUint16            amConnectionId;             /* Connection identifier                              */
    CsrAmEndPointDefType sinkId;                     /* What sink to remove                                */
} CsrAmAudioPathRemoveSinkReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathRemoveSinkCfm;


typedef struct
{
    CsrAmPrim         type;                     /* Event identifier                                   */
    CsrSchedQid       pHandle;                  /* Handle to application                              */
    CsrUint16         amConnectionId;           /* Connection identifier                              */
    CsrAmEndpointType epType;                   /* Endpoint: source, sink or both                     */
    CsrUint16         sinkId;                   /* What sink (or all sinks) in multi-sink case        */
    CsrUint16         configDataLen;            /* length in bytes allocated in 'configData'          */
    CsrUint8         *configData;               /* Type-Value configuration data pairs                */
} CsrAmAudioPathConfigReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathConfigCfm;


typedef struct
{
    CsrAmPrim   type;                           /* Event identifier                                   */
    CsrSchedQid pHandle;                        /* Handle to application                              */
    CsrUint16   amConnectionId;                 /* Connection identifier                              */
} CsrAmAudioPathReleaseReq;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrResult result;                           /* Status information about the AM operation          */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathReleaseCfm;

typedef struct
{
    CsrAmPrim type;                             /* Event identifier                                   */
    CsrUint16 amConnectionId;                   /* Connection identifier                              */
} CsrAmAudioPathReleaseInd;

#ifdef __cplusplus
}
#endif

#endif
