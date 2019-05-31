#ifndef CSR_BT_ASM_PRIM_H__
#define CSR_BT_ASM_PRIM_H__

/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_dspm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAsmPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtAsmPrim;
typedef CsrBtResultCode CsrBtAsmResultCode;

/* ---------- Defines the ASM CsrBtAsmResultCode ----------*/
#define CSR_BT_RESULT_CODE_ASM_SUCCESS               ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_ASM_FAILURE               ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS    ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_ASM_RESOURCE_NOT_FOUND    ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_ASM_RESOURCE_FAILED       ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_ASM_RESOURCE_NOT_ACQUIRED ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_ASM_RESOURCE_BUSY         ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_ASM_SHORT_STREAM          ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_ASM_STREAM_NOT_CONNECTED  ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_ASM_STREAM_NOT_FOUND      ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_ASM_NOT_ACTIVATED         ((CsrBtResultCode) (0x000a))

/****************************************************************************
 * Start of primitive definitions
 ****************************************************************************/
/* Prim Downstream Lowest */
#define CSR_BT_ASM_P_DS_L       (0x0000)

#define ASM_PDSL                (CSR_BT_ASM_P_DS_L)

/* Register Chip information */
#define CSR_BT_ASM_ACTIVATE_REQ               ((CsrBtAsmPrim)(0x0000+ASM_PDSL))
#define CSR_BT_ASM_DEACTIVATE_REQ             ((CsrBtAsmPrim)(0x0001+ASM_PDSL))
#define CSR_BT_ASM_GET_AUDIO_RESOURCE_REQ     ((CsrBtAsmPrim)(0x0002+ASM_PDSL))
#define CSR_BT_ASM_RELEASE_AUDIO_RESOURCE_REQ ((CsrBtAsmPrim)(0x0003+ASM_PDSL))
#define CSR_BT_ASM_CONFIG_AUDIO_RESOURCE_REQ  ((CsrBtAsmPrim)(0x0004+ASM_PDSL))
#define CSR_BT_ASM_SYNC_AUDIO_RESOURCE_REQ    ((CsrBtAsmPrim)(0x0005+ASM_PDSL))
#define CSR_BT_ASM_CONNECT_AUDIO_RESOURCE_REQ ((CsrBtAsmPrim)(0x0006+ASM_PDSL))
#define CSR_BT_ASM_DISCONNECT_AUDIO_RESOURCE_REQ ((CsrBtAsmPrim)(0x0007+ASM_PDSL))
#define CSR_BT_ASM_START_AUDIO_STREAM_REQ   ((CsrBtAsmPrim)(0x0008+ASM_PDSL))
#define CSR_BT_ASM_STOP_AUDIO_STREAM_REQ    ((CsrBtAsmPrim)(0x0009+ASM_PDSL))
#define CSR_BT_ASM_ADD_CAPABILITY_REQ       ((CsrBtAsmPrim)(0x000A+ASM_PDSL))
#define CSR_BT_ASM_REMOVE_CAPABILITY_REQ    ((CsrBtAsmPrim)(0x000B+ASM_PDSL))

#define CSR_BT_ASM_P_DS_H     (0x000B + CSR_BT_ASM_P_DS_L)
#define CSR_BT_ASM_P_DS_COUNT (CSR_BT_ASM_P_DS_H + 1 - CSR_BT_ASM_P_DS_L)

/****************************************************************************/

/* Prim Upstream Lowest */
#define CSR_BT_ASM_P_US_L       (0x0000 + CSR_PRIM_UPSTREAM)
#define ASM_PUSL                (CSR_BT_ASM_P_US_L)

#define CSR_BT_ASM_ACTIVATE_CFM               ((CsrBtAsmPrim)(0x0000+ASM_PUSL))
#define CSR_BT_ASM_DEACTIVATE_CFM             ((CsrBtAsmPrim)(0x0001+ASM_PUSL))
#define CSR_BT_ASM_GET_AUDIO_RESOURCE_CFM     ((CsrBtAsmPrim)(0x0002+ASM_PUSL))
#define CSR_BT_ASM_RELEASE_AUDIO_RESOURCE_CFM ((CsrBtAsmPrim)(0x0003+ASM_PUSL))
#define CSR_BT_ASM_CONFIG_AUDIO_RESOURCE_CFM  ((CsrBtAsmPrim)(0x0004+ASM_PUSL))
#define CSR_BT_ASM_SYNC_AUDIO_RESOURCE_CFM    ((CsrBtAsmPrim)(0x0005+ASM_PUSL))
#define CSR_BT_ASM_CONNECT_AUDIO_RESOURCE_CFM ((CsrBtAsmPrim)(0x0006+ASM_PUSL))
#define CSR_BT_ASM_DISCONNECT_AUDIO_RESOURCE_CFM ((CsrBtAsmPrim)(0x0007+ASM_PUSL))
#define CSR_BT_ASM_START_AUDIO_STREAM_CFM     ((CsrBtAsmPrim)(0x0008+ASM_PUSL))
#define CSR_BT_ASM_STOP_AUDIO_STREAM_CFM      ((CsrBtAsmPrim)(0x0009+ASM_PUSL))
#define CSR_BT_ASM_ADD_CAPABILITY_CFM         ((CsrBtAsmPrim)(0x000A+ASM_PUSL))
#define CSR_BT_ASM_REMOVE_CAPABILITY_CFM      ((CsrBtAsmPrim)(0x000B+ASM_PUSL))
#define CSR_BT_ASM_AUDIO_RESOURCE_MESSAGE_IND ((CsrBtAsmPrim)(0x000C+ASM_PUSL))

#define CSR_BT_ASM_P_US_H     (0x000C + CSR_BT_ASM_P_US_L)
#define CSR_BT_ASM_P_US_COUNT (CSR_BT_ASM_P_US_H + 1 - CSR_BT_ASM_P_US_L)

/****************************************************************************
 * End of primitive definitions
 ****************************************************************************/

/****************************************************************************
 * Downstream messages
 ****************************************************************************/

/* PCM/I2S/CODEC interfaces (TX/RX) are considered external points 
    (external sources/sinks) and Operator inputs & outputs are considered 
    internal points (internal sources/sinks).
 */
typedef CsrUint16 CsrBtAsmAudioResourceCategory;
#define CSR_BT_ASM_AR_CAT_INVALID   0x0000
#define CSR_BT_ASM_AR_CAT_SOURCE    0x0001
#define CSR_BT_ASM_AR_CAT_SINK      0x0002
#define CSR_BT_ASM_AR_CAT_OPERATOR  0x0003

typedef CsrUint16 CsrBtAsmAudioResourceType;

/* Interface types under category: -
    CSR_BT_ASM_AR_CAT_SOURCE
    CSR_BT_ASM_AR_CAT_SINK
 */
#define CSR_BT_ASM_AR_TYPE_PCM   CSR_DSPM_CP_TYPE_PCM
#define CSR_BT_ASM_AR_TYPE_I2S   CSR_DSPM_CP_TYPE_I2S     
#define CSR_BT_ASM_AR_TYPE_CODEC CSR_DSPM_CP_TYPE_CODEC   
#define CSR_BT_ASM_AR_TYPE_FM    CSR_DSPM_CP_TYPE_FM      
#define CSR_BT_ASM_AR_TYPE_SPDIF CSR_DSPM_CP_TYPE_SPDIF   
#define CSR_BT_ASM_AR_TYPE_DM    CSR_DSPM_CP_TYPE_DM      
#define CSR_BT_ASM_AR_TYPE_FASTPIPE CSR_DSPM_CP_TYPE_FASTPIPE
/* Note: SCO/A2DP are Dynamic endpoints and are only available through 
    CsrBtAsmGetAudioResource() and when SCO/A2DP-L2CAP connections 
    gets disconnected, these resources should be released using 
    CsrBtAsmReleaseAudioResource().
    */
#define CSR_BT_ASM_AR_TYPE_A2DP  CSR_DSPM_CP_TYPE_A2DP    
#define CSR_BT_ASM_AR_TYPE_SCO   CSR_DSPM_CP_TYPE_SCO     

/* Capability types under category: -
    CSR_BT_ASM_AR_CAT_OPERATOR.
 */
#define CSR_BT_ASM_AR_TYPE_NONE                    0x0000
#define CSR_BT_ASM_AR_TYPE_MONO_PASSTHROUGH        0x0001
#define CSR_BT_ASM_AR_TYPE_STEREO_PASSTHROUGH      0x0002
#define CSR_BT_ASM_AR_TYPE_SCO_SEND                0x0003
#define CSR_BT_ASM_AR_TYPE_SCO_RECEIVE             0x0004
#define CSR_BT_ASM_AR_TYPE_WBS_ENCODER             0x0005
#define CSR_BT_ASM_AR_TYPE_WBS_DECODER             0x0006
#define CSR_BT_ASM_AR_TYPE_A2DP_SBC_ENCODER        0x0007
#define CSR_BT_ASM_AR_TYPE_A2DP_MP3_ENCODER        0x0008
#define CSR_BT_ASM_AR_TYPE_RESAMPLER               0x0009
#define CSR_BT_ASM_AR_TYPE_2_INTO_1_MIXER          0x000A
#define CSR_BT_ASM_AR_TYPE_A2DP_APTX_ENCODER       0x000B
#define CSR_BT_ASM_AR_TYPE_FM_ENHANCEMENT          0x000C
#define CSR_BT_ASM_AR_TYPE_A2DP_SBC_DECODER        0x000D
#define CSR_BT_ASM_AR_TYPE_A2DP_MP3_DECODER        0x000E
#define CSR_BT_ASM_AR_TYPE_A2DP_AAC_DECODER        0x000F
#define CSR_BT_ASM_AR_TYPE_A2DP_APTX_DECODER       0x0010
#define CSR_BT_ASM_AR_TYPE_1KHZ_RINGTONE_GENERATOR 0x0011
#define CSR_BT_ASM_AR_TYPE_EQUALISER_COMPANDER     0x0012
#define CSR_BT_ASM_AR_TYPE_SPLITTER                0x0013

#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_FE_SEND     0x001A
#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_FE_RCV      0x001B

#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_NBS_SEND    0x001C 
#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_NBS_RCV     0x001D 
#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_WBS_SEND    0x001E
#define CSR_BT_ASM_AR_TYPE_CVC_HF_1MIC_WBS_RCV     0x001F

/* 4001 - 5FFF
    First free value for downloadable capabilities */
#define CSR_BT_ASM_AR_TYPE_CAP_DOWNLOAD_lOWER      0x4001     

typedef struct
{
    /* Represents the Audio Resource. For example: PCM/ADC/WBS... */
    CsrBtAsmAudioResourceType type;
    /* Represents the Interface number. For instance: DaleA07 has 2 pcm ports
     *  then this should be presented as 0 and 1
     */
    /* Note:For operators instanceNumber does not hold any meaning it shall 
     *      always be set to 0
     */
    CsrUint8                  instanceNumber;
    /* Represents the channels or slots available per Interface.
     *  For instance: 
     *  On DaleA07: PCM interface has 4 slots so numChannelsPerInstance=4
     *              ADC has left & right channels so numChannelsPerInstance=2.
     *
     *  Note:For operators numChannelsPerInstance does not hold any meaning 
     *       it shall always be set to 1
     */
    CsrUint8                  numChannelsPerInstance;
} CsrBtAsmAudioResourceInfo;

typedef struct
{
    /* List (array) of sources available on-chip */
    CsrBtAsmAudioResourceInfo *sourceInfo;
    /* Total number of listed sources */
    CsrUint8                   sourceInfoCount;
    /* List (array) of sinks available on-chip */
    CsrBtAsmAudioResourceInfo *sinkInfo;
    /* Total number of listed sinks */
    CsrUint8                   sinkInfoCount;
    /* List (array) of default operators available on-chip */
    CsrBtAsmAudioResourceInfo *operatorInfo;
    /* Total operators in the list */
    CsrUint8                   operatorInfoCount;
} CsrBtAsmAudioChipInfo;

typedef struct
{
    CsrBtAsmPrim           type;
    CsrSchedQid            appQid;
    CsrBtAsmAudioChipInfo *chipInfo;
} CsrBtAsmActivateReq;

typedef struct
{
    CsrBtAsmPrim           type;
    CsrBtAsmResultCode     result;
} CsrBtAsmActivateCfm;

typedef struct
{
    CsrBtAsmPrim type;
    CsrSchedQid  appQid;
} CsrBtAsmDeactivateReq;

typedef struct
{
    CsrBtAsmPrim           type;
} CsrBtAsmDeactivateCfm;

/* Unique resource ID allocated by onchip dsp */
typedef CsrUint16 CsrBtAsmAudioResourceId;
#define CSR_BT_ASM_INVALID_AUDIO_RESOURCE_ID ((CsrBtAsmAudioResourceId) 0)

/* Info structure describing an Operator Key & value */
typedef CsrUint16 CsrAsmOpKey;

/* Range 0 upto 4 */
#define CSR_ASM_OP_KEY_OPERATOR_PRIORITY   ((CsrAsmOpKey) 0x0001)
/* Varies from chip to chip */
#define CSR_ASM_OP_KEY_PROCESSOR_ID        ((CsrAsmOpKey) 0x0002)

typedef struct
{
    CsrAsmOpKey       key;
    CsrUint32         value;
} CsrAsmOpKeyValue;

typedef struct CsrBtAsmAudioResource
{
    /* (IN) Pointer to the next resource */
    struct CsrBtAsmAudioResource *next;
    /* (IN) Source/Sink/Operator */
    CsrBtAsmAudioResourceCategory cat;
    /* (IN) ADC/WBS/CVC-1MIC/... */
    CsrBtAsmAudioResourceType     type;
    /* (IN)
     * For "cat" being external SOURCE and SINK then "instance" is the 
     * interface number and "channel" is channel/slot number.
     * Example:
     * - To use channel 1 of 0th interface of I2S RX (source)
     * then instance = 0, channel = 1.
     * - To use pcm slot3 (starting from slot1, slot2, slot3, slot4) of 1st 
     * interface of PCM RX (sink) then instance = 1, channel = 2.
     *
     * For "cat" being OPERATOR; "instance" has no meaning and "channel"
     * represents the connection point number.
     * Example:
     * - Mixer operator has 2 inputs (2 internal sinks) and 
     * 1 output (1 internal source). In case if this resource is used as
     * "sink"(internal sink) the channel numbrer ("channel") would be 
     * either 0 or 1. If this resource is an "source"(internal source) then 
     * channel number ("channel") is 0.
     */
    CsrUint16                     instance;
    CsrUint16                     channel;
    /* (IN) Total number of keys for the category operator. 
        If this holds zero then the legacy create operator would be invoked */
    CsrUint16                     keyValueCount;
    /* (IN) Key value pairs to be supplied during the creation of operator.
        If this holds NULL then the legacy create operator would be invoked */
    CsrAsmOpKeyValue             *keyValue;
    /* (OUT) Result of particular operation performed on this resource */
    CsrBtAsmResultCode            result;
    /* (OUT) Source, Sink or Operator id assigned by chip. */
    CsrBtAsmAudioResourceId       id;
} CsrBtAsmAudioResource;

/* Application shall acquire the resources using this request.
 *  Note: 
 *  1. For category type CSR_BT_ASM_AR_CAT_OPERATOR, ASM shall always 
 *     create a new instance.
 *  2. For any category if the instantiated id (CsrBtAsmAudioResourceId id)
 *     is valid (i.e. if not equal to CSR_BT_ASM_INVALID_AUDIO_RESOURCE_ID) 
 *     then it shall move onto the next resource.
 */
typedef struct
{
    CsrBtAsmPrim           type;
    CsrSchedQid            appQid;
    /* Pointer to the list of audio resources to be acquired.
     * Note: 
     * On receiving this request the Audio Resource "asmRes" shall be completely 
     * owned by ASM.
     */
    CsrBtAsmAudioResource *asmRes;
    /* If true then ASM shall continue to fetch other resources (pointed by 
     * asmRes->next).
     */
    CsrBool                continueOnFailure;
} CsrBtAsmGetAudioResourceReq;

typedef struct
{
    CsrBtAsmPrim            type;
    /* Pointer to the list of resources which were acquired by ASM for the 
     * application. The resources shall be in the same order in which the 
     * application had requested.
     * It shall be the responsibility of application to free it upon its 
     * discretion.
     */
    CsrBtAsmAudioResource  *appRes;
    /* 
     * result would have following values:
     *
     * CSR_BT_RESULT_CODE_ASM_SUCCESS - All the resources were fetched 
     *  successfully
     *
     * CSR_BT_RESULT_CODE_ASM_FAILURE - Indicates there were failures in 
     *  fetching resources. Application have to individually scan through 
     *  the appRes to find the failures by checking appropriate resources 
     * "result" field holding the failure reason.
     *
     * CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS - appRes was null
     */
    CsrBtAsmResultCode      result;
} CsrBtAsmGetAudioResourceCfm;

typedef struct
{
    CsrBtAsmPrim           type;
    CsrSchedQid            appQid;
    /* Pointer to the list of audio resources to be released.
     * Note: 
     * On receiving this request the Audio Resource "asmRes" shall be completely 
     * owned by ASM.
     */
    CsrBtAsmAudioResource *asmRes;
} CsrBtAsmReleaseAudioResourceReq;

typedef struct
{
    CsrBtAsmPrim            type;
} CsrBtAsmReleaseAudioResourceCfm;

typedef CsrUint16 CsrBtAsmConfigKey;
/* key: From-air sample-based rate matching */
/* value: 0 (disabled) or non-zero (enabled) */
#define CSR_BT_ASM_CONFIG_KEY_SCO_FROM_AIR_SAMPLE_RATE_MATCHING 0x0000
/* key: To-air frame-based rate matching */
/* value: 0 (disabled) or non-zero (enabled) */
#define CSR_BT_ASM_CONFIG_KEY_SCO_TO_AIR_FRAME_RATE_MATCHING    0x0009
/* Key: For any source/sink Notify when Stream drains. */
/* Value: disable (0) or enable (non-zero) */
#define CSR_BT_ASM_CONFIG_KEY_SRC_SNK_NOTIFY_STREAM_DRAIN       0x000a
/* Sync rate in hertz */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SYNC_RATE                     0x0100
/* 0 (auto generated) or Master clock rate in hertz */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MASTER_CLOCK_RATE             0x0101
/* 0 (slave), 1 (master) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MASTER_MODE                   0x0102
/* 0 (derived from clock and sync rates), 1 - 4 (number of slots) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SLOT_COUNT                    0x0103
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MANCHESTER_MODE               0x0104
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SHORT_SYNC_MODE               0x0105
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MANCHESTER_SLAVE_MODE         0x0106
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SIGN_EXTEND_MODE              0x0107
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_LSB_FIRST_MODE                0x0108
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_TX_TRISTATE_MODE              0x0109
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_TX_TRISTATE_RISING_EDGE_MODE  0x010a
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SYNC_SUPPRESS_ENABLE          0x010b
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_GCI_MODE                      0x010c
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MUTE_ENABLE                   0x010d
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_LONG_LENGTH_SYNC              0x010e
/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SAMPLE_RISING_EDGE            0x010f
/* 0 - 7 */
#define CSR_BT_ASM_CONFIG_KEY_PCM_RX_RATE_DELAY                 0x0113
/* 0 (13 bits in a 16 bit slot)
   1 (16 bits in a 16 bit slot)
   2 (8 bits in a 16 bit slot)
   3 (8 bits in an 8 bit slot)  */
#define CSR_BT_ASM_CONFIG_KEY_PCM_SAMPLE_FORMAT                 0x0114
/* 0 - 3 */
#define CSR_BT_ASM_CONFIG_KEY_PCM_MANCHESTER_MODE_RX_OFFSET     0x0115
/* 0 - 7 */
#define CSR_BT_ASM_CONFIG_KEY_PCM_AUDIO_GAIN                    0x0116

#define CSR_BT_ASM_CONFIG_KEY_I2S_SYNC_RATE                 0x0200
#define CSR_BT_ASM_CONFIG_KEY_I2S_MASTER_CLOCK_RATE         0x0201
#define CSR_BT_ASM_CONFIG_KEY_I2S_MASTER_MODE               0x0202
#define CSR_BT_ASM_CONFIG_KEY_I2S_JUSTIFY_FORMAT            0x0203
#define CSR_BT_ASM_CONFIG_KEY_I2S_LEFT_JUSTIFY_DELAY        0x0204
#define CSR_BT_ASM_CONFIG_KEY_I2S_CHANNEL_POLARITY          0x0205
#define CSR_BT_ASM_CONFIG_KEY_I2S_AUDIO_ATTENUATION_ENABLE  0x0206
#define CSR_BT_ASM_CONFIG_KEY_I2S_AUDIO_ATTENUATION         0x0207
#define CSR_BT_ASM_CONFIG_KEY_I2S_JUSTIFY_RESOLUTION        0x0208
#define CSR_BT_ASM_CONFIG_KEY_I2S_CROP_ENABLE               0x0209
#define CSR_BT_ASM_CONFIG_KEY_I2S_BITS_PER_SAMPLE           0x020a
#define CSR_BT_ASM_CONFIG_KEY_I2S_TX_START_SAMPLE           0x020b
#define CSR_BT_ASM_CONFIG_KEY_I2S_RX_START_SAMPLE           0x020c

#define CSR_BT_ASM_CONFIG_KEY_CODEC_INPUT_SAMPLE_RATE                 0x0300
#define CSR_BT_ASM_CONFIG_KEY_CODEC_OUTPUT_SAMPLE_RATE                0x0301
#define CSR_BT_ASM_CONFIG_KEY_CODEC_INPUT_GAIN                        0x0302
#define CSR_BT_ASM_CONFIG_KEY_CODEC_OUTPUT_GAIN                       0x0303
#define CSR_BT_ASM_CONFIG_KEY_CODEC_RAW_INPUT_GAIN                    0x0304
#define CSR_BT_ASM_CONFIG_KEY_CODEC_RAW_OUTPUT_GAIN                   0x0305
#define CSR_BT_ASM_CONFIG_KEY_CODEC_OUTPUT_GAIN_BOOST_ENABLE          0x0306
#define CSR_BT_ASM_CONFIG_KEY_CODEC_SIDE_TONE_GAIN                    0x0307
#define CSR_BT_ASM_CONFIG_KEY_CODEC_SIDE_TONE_ENABLE                  0x0308
#define CSR_BT_ASM_CONFIG_KEY_CODEC_MIC_INPUT_GAIN_ENABLE             0x0309
#define CSR_BT_ASM_CONFIG_KEY_CODEC_LOW_POWER_OUTPUT_STAGE            0x030a
#define CSR_BT_ASM_CONFIG_KEY_CODEC_QUALITY_MODE                      0x030b
#define CSR_BT_ASM_CONFIG_KEY_CODEC_OUTPUT_INTERPOLATION_FILTER_MODE  0x030c
#define CSR_BT_ASM_CONFIG_KEY_CODEC_OUTPUT_POWER_MODE                 0x030d
#define CSR_BT_ASM_CONFIG_KEY_CODEC_SIDE_TONE_SOURCE                  0x030e

/* 32000, 44100, 48000 */
#define CSR_BT_ASM_CONFIG_KEY_SPDIF_OUTPUT_SAMPLE_RATE  0x0500

#define CSR_BT_ASM_CONFIG_KEY_DIGITAL_MIC_INPUT_SAMPLE_RATE  0x0600
#define CSR_BT_ASM_CONFIG_KEY_DIGITAL_MIC_INPUT_GAIN         0x0601
#define CSR_BT_ASM_CONFIG_KEY_DIGITAL_MIC_SIDE_TONE_GAIN     0x0602
#define CSR_BT_ASM_CONFIG_KEY_DIGITAL_MIC_SIDE_TONE_ENABLE   0x0603

/* 0 (disable), 1 (enable) */
#define CSR_BT_ASM_CONFIG_KEY_AUDIO_CHANNEL_MUTE_ENABLE  0x0700

/* L2CAP To-Air Shunt config parameters */
/* Value: Between 1 and the MTU for the L2CAP channel */
#define CSR_BT_ASM_CONFIG_KEY_L2CAP_SINK_ATU               0x0800
/* Value: 0 (disabled) or 1 (enabled) */
#define CSR_BT_ASM_CONFIG_KEY_L2CAP_SINK_AUTOFLUSH         0x0801
/* At least enough tokens to send a PDU on the L2CAP channel of the size 
    specified by the ATU */
#define CSR_BT_ASM_CONFIG_KEY_L2CAP_SINK_TOKENS_ALLOCATED  0x0802

/* L2CAP From-Air Shunt config parameters */
#define CSR_BT_ASM_CONFIG_KEY_L2CAP_SOURCE_MAX_PDU_LENGTH       0x0803
#define CSR_BT_ASM_CONFIG_KEY_L2CAP_SOURCE_NOTIFY_STREAM_DRAIN  0x0804

typedef struct
{
    CsrBtAsmConfigKey key;
    CsrUint32         value;
} CsrBtAsmSrcSnkConfigKeyValue;

/* Please check the capability library for the default values */

typedef CsrUint8 CsrBtAsmOperatorConfigKey;
/* Note: Each argument size is of 16bits. */
/* No arguments */
#define CSR_BT_ASM_OP_CONFIG_KEY_ENABLE_FADE_OUT          0
/* No arguments */
#define CSR_BT_ASM_OP_CONFIG_KEY_DISABLE_FADE_OUT         1
/* 1 argument */
/* Value
    0 - No fade, instant change
    1 - 1 second fade
    2 - 1/2 second fade
    3 - 1/4 second fade
*/
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_FADE_RATE            2
/* 2 arguments: MSB 8 bits(0x00XX) LSB 16 bits(0xXXXX) */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_GAIN_MONO            3
/* 4 arguments */
/* Left Channel : MSB 8 bits(0x00XX), LSB 16 bits(0xYYZZ) */
/* Right Channel: MSB 8 bits(0x00XX), LSB 16 bits(0xYYZZ) */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_GAIN_STEREO          4
/* 1 argument */
/* Off(0) or On(1) */
/* Note: This shall not be used while operator is running */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_RATE_MATCHING        5
/* 6 arguments */
/* SBC Encode Parameters */
/* Meaning             Value
   ---------------------------------------
   num_subbands        4, 8
   num_blocks          4, 8, 12, 16
   bitpool             Refer specification
   sampling_freq       0(16K),
                       1(32k),
                       2(44.1k),
                       3(48k)
   channel_mode        0(mono)
                       1(dual mono)
                       2(stereo)
                       3(joint stereo)
   allocation_method   0(loudness),
                       1(snr)
*/
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_A2DP_SBC_ENCODE_PARAMS  6
/* 1 argument (16bits) */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_ATU_SIZE                7
/* 1 argument (16bits) */
/* Value: disable(0), enable(0) */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_FASTSTREAM              8
/* 1 argument (16bits) */
/* Value: disable(0), enable(0) */
#define CSR_BT_ASM_OP_CONFIG_KEY_ENABLE_CONTENT_PROTECTION   9
/* 1 argument */
/* Input/Output Data Type */
/* Val  Meaning
    0   16-bit big-endian data
    1   16-bit little-endian linear audio
    2   16-bit little-endian A-law audio
    3   16-bit little-endian u-law audio
    4   13-bit audio
*/
/* Note: Data type cannot be changed if the operator has started */
#define CSR_BT_ASM_OP_CONFIG_KEY_CHANGE_INPUT_DATA_TYPE      10
#define CSR_BT_ASM_OP_CONFIG_KEY_CHANGE_OUTPUT_DATA_TYPE     11

/* 1 argument */
/* Values: 1 or 2 */
#define CSR_BT_ASM_OP_CONFIG_KEY_NUM_CHANNELS_TO_MIX         12
/* 1 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_CONVRERSION_RATE        13
/* 5 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_CUSTOM_RATE             14
/* Variable argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_FILTER_COEFFICIENTS         15
/* 5 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_EQ_GAIN                 16
/* 8 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_COMPRESSOR_PARAMS       17
/* 8 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_EXPANDER_PARAMS         18
/* 8 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_LIMITER_PARAMS          19
/* 1 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_ENABLE_AUTO_COMPRESSION     20
/* 1 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_SAMPLE_FREQUENCY        21
/* 1 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_BITRATE                 22
/* 1 argument */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_CHANNEL_MODE            23
/* 5 arguments */
/*
   Meaning            Value                   Default
    tone_freq           0 to SAMPLE_FREQ/2      2000
    num_bursts          0 to 0xffffff           32
    sample_freq         Valid for h/w           8000
    burst_period        0 to 0xffffff           250
    silence_period      0 to 0xffffff           16000
*/
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_RINGTONE_PARAMS         24
/* 2 arguments: MSB 8 bits(0x00XX) LSB 16 bits(0xXXXX) */
#define CSR_BT_ASM_OP_CONFIG_KEY_SET_TONE_VOLUME             25

typedef struct
{
    CsrBtAsmOperatorConfigKey key;
    CsrUint8                  valueLen;
    CsrUint16*                value;
} CsrBtAsmOperatorConfigKeyValue;

/* If a source or sink has to be configured then store the config under epCfg 
 * and set the opCfg to NULL.
 * Similarly if an operator has to be configured epCfg shall be NULL and the 
 * message to operator shall be stored under opCfg.
 *
 * cfgSize would indicate the total number of source/sink or operator 
 * configurations.
 */
typedef struct CsrBtAsmAudioResourceConfig
{
    struct CsrBtAsmAudioResourceConfig *next;
    /* (IN) Source/Sink/Operator instance id */
    CsrBtAsmAudioResourceId             id;
    /* Total number of configurations for source/sink or operator */
    CsrUint8                            cfgSize;
    /* Source/sink endpoint configuration */
    CsrBtAsmSrcSnkConfigKeyValue       *epCfg;
    /* Operator configuration */
    CsrBtAsmOperatorConfigKeyValue     *opCfg;
    /* (OUT) Result of the configuration */
    CsrBtAsmResultCode                  result;
} CsrBtAsmAudioResourceConfig;

typedef struct
{
    CsrBtAsmPrim                 type;
    CsrSchedQid                  appQid;
    /* After receiving this request, config shall be completely owned by ASM.
     */
    CsrBtAsmAudioResourceConfig *config;
} CsrBtAsmConfigAudioResourceReq;

typedef struct
{
    CsrBtAsmPrim                  type;
    /* List of config in the same order as sent in the request is
     * sent back to application as part of the 
     * confirmation. This is for application to know for which configuration 
     * there was a failure when multiple config request are sent. 
     * Application shall free both the config and the src/snk/operator 
     * config allocated memory (u.op or u.ep) under the union.
     * This is true for the other configs pointed by config->next.
     * 
     */
    CsrBtAsmAudioResourceConfig  *config;
    /* 
     * result would have following values:
     *
     * CSR_BT_RESULT_CODE_ASM_SUCCESS - All the resources were configured 
     *  successfully
     *
     * CSR_BT_RESULT_CODE_ASM_FAILURE - Indicates there were failures in 
     *  configuring resources. Application have to individually scan through 
     *  the config to find the failures by checking appropriate config's 
     * "result" field holding the failure reason.
     *
     * CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS - config was null
     */
    CsrBtAsmResultCode            result;
} CsrBtAsmConfigAudioResourceCfm;

/* Note:
 *  1. Following restrictions apply on (resA, resB) pair:-
 *     (resA, resB) - both shall be external sources
 *                    both shall be external sinks
 *                    both internal sources
 *                    both internal sinks
 *                    one internal sink and one internal source.
 *
 *  2. Any other combinations would fail.
 *  3. resB - can be NULL to break the existing synchronization with resA.
 *  4. For external sources/sinks resA and resB could be of any sources 
 *     or sinks.
 *     For instance:
 *     resA could be ADC channel 0 and resB could be ADC channel 1. 
 *     resA could be DAC channel 0 and resB could be DAC channel 1. 
 *     resA could be ADC channel 0 and resB could be PCM RX slot 0. 
 */
typedef struct CsrBtAsmSyncAudioResourcePair
{
    struct CsrBtAsmSyncAudioResourcePair *next;

    /* catA or catB - Source or Sink.
     * Indicates if the source or a sink has to be synchronized.
     * Note: Since operators coud have multiple sources (internal sources) and 
     * multiple sinks (internal sinks) application has to explicitly specify 
     * the same here i.e. if it is Source pair or Sink pair of an operator 
     * which is be requested to synchronize.
     *
     * Note:
     * catA or catB - shall not be set to operator.
     */
    CsrBtAsmAudioResourceCategory         catA;
    CsrBtAsmAudioResource                *resA;
    CsrBtAsmAudioResourceCategory         catB;
    CsrBtAsmAudioResource                *resB;
    CsrBtAsmResultCode                    result;
} CsrBtAsmSyncAudioResourcePair;

typedef struct
{
    CsrBtAsmPrim                   type;
    CsrSchedQid                    appQid;
    /* On receiving this request ASM shall completely own the syncPair.
     */
    CsrBtAsmSyncAudioResourcePair* syncPair;
} CsrBtAsmSyncAudioResourceReq;

typedef struct
{
    CsrBtAsmPrim                   type;
    /*
     * List of syncPair in the same order as sent in the request is 
     * sent back to application as part of the 
     * confirmation. This is for application to know for which sync pair there 
     * was a failure when multiple sync requests were sent.
     * Application shall free the resA, resB and the syncPair.
     * This is true for the other configs pointed by syncPair->next.
     */
    CsrBtAsmSyncAudioResourcePair *syncPair;
    /* 
     * result would have following values:
     *
     * CSR_BT_RESULT_CODE_ASM_SUCCESS - All the resources were synchornized 
     *  successfully
     *
     * CSR_BT_RESULT_CODE_ASM_FAILURE - Indicates there were failures in 
     *  synchronizing resources. Application have to individually scan through 
     *  the syncPair to find the failures by checking appropriate syncPair's 
     * "result" field holding the failure reason.
     *
     * CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS - syncPair was null
     */
    CsrUint16                      result;
} CsrBtAsmSyncAudioResourceCfm;

typedef struct CsrBtAsmAudioStream
{
    /* (In) Pointer to the list of Audio Stream */
    struct CsrBtAsmAudioStream *next;
    /* (In) Pointer to the list of resources */
    CsrBtAsmAudioResource      *res;
} CsrBtAsmAudioStream;

/* This shall connect sources(physical/sco/operator) to 
 *  sinks (physical/sco/operator) of an Audio Stream pointed by "as". 
 *  Multiple Audio Streams could be part of this request
 *
 * Note:
 * On receiving this request the Audio Stream "as" shall be completely owned 
 *  by ASM.
 */
typedef struct
{
    CsrBtAsmPrim         type;
    CsrSchedQid          appQid;
    /* Pointer to the list of Audio Streams.
     */
    CsrBtAsmAudioStream *audioStream;
} CsrBtAsmConnectAudioResourceReq;

/* Invalid audio stream ID */
#define CSR_BT_ASM_AUDIO_STREAM_ID_INVALID          ((CsrUint32) 0)

/*
 * On successfully connecting all the audio streams ASM shall include the 
 * identifier (audioStreamsId) in the confirmation and result shall be 
 * CSR_BT_RESULT_CODE_ASM_SUCCESS.
 * 
 * On Failure:
 * audioStreamsId shall be 0. If one of the audio stream (as) fails to get 
 * connected (due to reasons such as some of the resources are busy or any 
 * other reason), the result shall indicate the reason for failure. It shall 
 * not proceed with the other audio streams. The previous successfully connected
 * audio streams would be disconnected internally by ASM.
 */
typedef struct
{
    CsrBtAsmPrim        type;
    /* Unique identifier representing the Audio Streams */
    CsrUint32           audioStreamId;
    /* Result of connecting the audio streams */
    CsrBtAsmResultCode  result;
} CsrBtAsmConnectAudioResourceCfm;

/* This shall disconnect all the connections in an audio stream pointed to by 
audioStreamId. On disconnection ASM shall free up the owned Audio Stream "as" 
received in the connect request.
*/
typedef struct
{
    CsrBtAsmPrim type;
    CsrSchedQid  appQid;
    /* Unique identifier representing the Audio Streams */
    CsrUint32    audioStreamId;
} CsrBtAsmDisconnectAudioResourceReq;

/* On disconnection ASM shall free up the owned Audio Stream "as" received in 
the connect request.
*/
typedef struct
{
    CsrBtAsmPrim       type;
    /* Unique identifier representing the Audio Streams */
    CsrUint32          audioStreamId;
    /* Result of disconnecting the audio streams */
    CsrBtAsmResultCode result;
} CsrBtAsmDisconnectAudioResourceCfm;

/*
 * This shall be called only if the previous call to connect succeeded.
 * Note:
 * Would return CSR_BT_RESULT_CODE_ASM_STREAM_NOT_CONNECTED if resources were 
 * not connected using CsrBtAsmConnectAudioResources.
*/
typedef struct
{
    CsrBtAsmPrim type;
    CsrSchedQid  appQid;
    /* Unique identifier representing the Audio Streams */
    CsrUint32    audioStreamId;
} CsrBtAsmStartAudioStreamReq;

/*
In case of failure the previous audio stream which have started shall be 
stopped internally by ASM
*/
typedef struct
{
    CsrBtAsmPrim       type;
    /* Unique identifier representing the Audio Streams */
    CsrUint32          audioStreamId;
    /* Result of Starting the audio streams */
    CsrBtAsmResultCode result;
} CsrBtAsmStartAudioStreamCfm;

typedef struct
{
    CsrBtAsmPrim type;
    CsrSchedQid  appQid;
    /* Unique identifier representing the Audio Streams */
    CsrUint32    audioStreamId;
} CsrBtAsmStopAudioStreamReq;

typedef struct
{
    CsrBtAsmPrim       type;
    /* Unique identifier representing the Audio Streams */
    CsrUint32          audioStreamId;
    /* Result of stoping the audio streams */
    CsrBtAsmResultCode result;
} CsrBtAsmStopAudioStreamCfm;

/* Note: All the Donwloadable capabilities have to be downloaded before 
starting any other operators. If not there could be undesirable consequences 
for instance; if a WBS operator is started and then an aptX capability is 
downloaded this could lead to drop of SCO packets.
*/
typedef struct
{
    CsrBtAsmPrim              type;
    CsrSchedQid               appQid;
    CsrBtAsmAudioResourceType capId;
    CsrUint16                 dataLength;
    CsrUint8                 *data;
} CsrBtAsmAddCapabilityReq;

typedef struct
{
    CsrBtAsmPrim              type;
    CsrBtAsmAudioResourceType capId;
    CsrBtAsmResultCode        result;
} CsrBtAsmAddCapabilityCfm;

typedef struct
{
    CsrBtAsmPrim              type;
    CsrSchedQid               appQid;
    CsrBtAsmAudioResourceType capId;
} CsrBtAsmRemoveCapabilityReq;

typedef struct
{
    CsrBtAsmPrim              type;
    CsrBtAsmAudioResourceType capId;
    CsrBtAsmResultCode        result;
} CsrBtAsmRemoveCapabilityCfm;

/* Currently this message is only applicable for CsrBtAsmAudioResourceCategory
 * type CSR_BT_ASM_AR_CAT_OPERATOR
 */
typedef struct
{
    CsrBtAsmPrim                  type;
    CsrBtAsmAudioResourceCategory cat;
    CsrBtAsmAudioResourceType     resType;
    CsrBtAsmAudioResourceId       id;
    CsrUint16                     messageLength;
    CsrUint16                    *message;
} CsrBtAsmAudioResourceMessageInd;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_ASM_PRIM_H__ */

