#ifndef CSR_BT_AV_PRIM_H__
#define CSR_BT_AV_PRIM_H__

/******************************************************************************

Copyright (c) 2004-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "l2cap_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAvPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim         CsrBtAvPrim;
typedef CsrUint8        CsrBtAvResult;
typedef CsrUint8        CsrBtAvMedia;
typedef CsrUint8        CsrBtAvCodec;
typedef CsrUint8        CsrBtAvCodecLoc;
typedef CsrUint8        CsrBtAvInput;
typedef CsrUint8        CsrBtAvSep;
typedef CsrUint16       CsrBtAvTimeout;
typedef CsrUint16       CsrBtAvRole;
typedef CsrUint8        CsrBtAvLevel;
typedef CsrUint16       CsrBtAvServCap;
typedef CsrUint8        CsrBtAvHdrType;

/*
 * This is an internal value sent only
 * from the application to the AV profile.
 * It may NOT go over the air.
 * The value must be one that does not
 * conflict with an actual AVDTP/GAVDP
 * error code.
 */
#define CSR_BT_AV_ACCEPT 0x00

/* ---------- Defines the AV CsrBtResultCode ----------*/

/*
 * The following error codes are actual GAVDP/AVDTP/A2DP protocol
 * error codes that are sent over the air.  These SHALL be used
 * ONLY with a result supplier of CSR_BT_SUPPLIER_A2DP -- or
 * used by applications for AV response codes.
 */
#define CSR_BT_RESULT_CODE_A2DP_BAD_HEADER_FORMAT             ((CsrBtAvResult) 0x01) /* AVDTP */

#define CSR_BT_RESULT_CODE_A2DP_BAD_LENGTH                    ((CsrBtAvResult) 0x11)
#define CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID                  ((CsrBtAvResult) 0x12)
#define CSR_BT_RESULT_CODE_A2DP_SEP_IN_USE                    ((CsrBtAvResult) 0x13)
#define CSR_BT_RESULT_CODE_A2DP_SEP_NOT_IN_USE                ((CsrBtAvResult) 0x14)
#define CSR_BT_RESULT_CODE_A2DP_BAD_SERV_CATEGORY             ((CsrBtAvResult) 0x17)
#define CSR_BT_RESULT_CODE_A2DP_BAD_PAYLOAD_FORMAT            ((CsrBtAvResult) 0x18)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CMD             ((CsrBtAvResult) 0x19)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_CAPABILITIES          ((CsrBtAvResult) 0x1A)

#define CSR_BT_RESULT_CODE_A2DP_BAD_RECOVERY_TYPE             ((CsrBtAvResult) 0x22)
#define CSR_BT_RESULT_CODE_A2DP_BAD_MEDIA_TRANSPORT_FORMAT    ((CsrBtAvResult) 0x23)
#define CSR_BT_RESULT_CODE_A2DP_BAD_RECOVERY_FORMAT           ((CsrBtAvResult) 0x25)
#define CSR_BT_RESULT_CODE_A2DP_BAD_ROHC_FORMAT               ((CsrBtAvResult) 0x26)
#define CSR_BT_RESULT_CODE_A2DP_BAD_CP_FORMAT                 ((CsrBtAvResult) 0x27)
#define CSR_BT_RESULT_CODE_A2DP_BAD_MULTIPLEXING_FORMAT       ((CsrBtAvResult) 0x28)
#define CSR_BT_RESULT_CODE_A2DP_UNSUPPORTED_CONFIGURATION     ((CsrBtAvResult) 0x29)
#define CSR_BT_RESULT_CODE_A2DP_BAD_STATE                     ((CsrBtAvResult) 0x31) /* AVDTP END */

#define CSR_BT_RESULT_CODE_A2DP_BAD_SERVICE                   ((CsrBtAvResult) 0x80) /* GAVDP */
#define CSR_BT_RESULT_CODE_A2DP_INSUFFICIENT_RESOURCES        ((CsrBtAvResult) 0x81) /* GAVDP */

#define CSR_BT_RESULT_CODE_A2DP_INVALID_CODEC_TYPE            ((CsrBtAvResult) 0xC1) /* A2DP GAVDP-extension */
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CODEC_TYPE      ((CsrBtAvResult) 0xC2)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_SAMPLING_FREQ         ((CsrBtAvResult) 0xC3)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_SAMPLING_FREQ   ((CsrBtAvResult) 0xC4)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_CHANNEL_MODE          ((CsrBtAvResult) 0xC5)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CHANNEL_MODE    ((CsrBtAvResult) 0xC6)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_SUBBANDS              ((CsrBtAvResult) 0xC7)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_SUBBANDS        ((CsrBtAvResult) 0xC8)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_ALLOC_METHOD          ((CsrBtAvResult) 0xC9)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_ALLOC_METHOD    ((CsrBtAvResult) 0xCA)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_MIN_BITPOOL           ((CsrBtAvResult) 0xCB)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MIN_BITPOOL     ((CsrBtAvResult) 0xCC)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_MAX_BITPOOL           ((CsrBtAvResult) 0xCD)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MAX_BITPOOL     ((CsrBtAvResult) 0xCE)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_LAYER                 ((CsrBtAvResult) 0xCF)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_LAYER           ((CsrBtAvResult) 0xD0)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CRC             ((CsrBtAvResult) 0xD1)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MPF             ((CsrBtAvResult) 0xD2)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_VBR             ((CsrBtAvResult) 0xD3)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_BIT_RATE              ((CsrBtAvResult) 0xD4)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_BIT_RATE        ((CsrBtAvResult) 0xD5)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_OBJECT_TYPE           ((CsrBtAvResult) 0xD6)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_OBJECT_TYPE     ((CsrBtAvResult) 0xD7)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_CHANNELS              ((CsrBtAvResult) 0xD8)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CHANNELS        ((CsrBtAvResult) 0xD9)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_VERSION               ((CsrBtAvResult) 0xDA)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_VERSION         ((CsrBtAvResult) 0xDB)
#define CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MAX_SUL         ((CsrBtAvResult) 0xDC)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_BLOCK_LENGTH          ((CsrBtAvResult) 0xDD)

#define CSR_BT_RESULT_CODE_A2DP_INVALID_CP_TYPE               ((CsrBtAvResult) 0xE0)
#define CSR_BT_RESULT_CODE_A2DP_INVALID_CP_FORMAT             ((CsrBtAvResult) 0xE1) /* A2DP END*/
/*
 * The following error codes are internal CSR API codes that SHALL
 * be used ONLY with a result supplier of CSR_BT_SUPPLIER_AV.
 * Internal AV codes have MSB set to avoid collisions.
 */
#define CSR_BT_RESULT_CODE_AV_SUCCESS                ((CsrBtResultCode) (0x8000))
#define CSR_BT_RESULT_CODE_AV_UNACCEPTABLE_PARAMETER ((CsrBtResultCode) (0x8001))
#define CSR_BT_RESULT_CODE_AV_INVALID_ROLE           ((CsrBtResultCode) (0x8002))
#define CSR_BT_RESULT_CODE_AV_SDS_REGISTER_FAILED    ((CsrBtResultCode) (0x8003)) /* Service record couldn't be registered*/
#define CSR_BT_RESULT_CODE_AV_CANCEL_CONNECT_ATTEMPT ((CsrBtResultCode) (0x8004))
#define CSR_BT_RESULT_CODE_AV_MAX_NUM_OF_CONNECTIONS ((CsrBtResultCode) (0x8005)) /*  The connection is refused based on maxing out the number of AV connections, defined by NUM_OF_CONNECTIONS in csr_bt_av_main.h*/
#define CSR_BT_RESULT_CODE_AV_NOT_CONNECTED          ((CsrBtResultCode) (0x8006))
#define CSR_BT_RESULT_CODE_AV_SIGNAL_TIMEOUT         ((CsrBtResultCode) (0x8007))
#define CSR_BT_RESULT_CODE_AV_FAILED                 ((CsrBtResultCode) (0x8008))
#define CSR_BT_RESULT_CODE_AV_SDC_SEARCH_FAILED      ((CsrBtResultCode) (0x8009))
#define CSR_BT_RESULT_CODE_AV_UNEXPECTED_REQUEST     ((CsrBtResultCode) (0x800A)) /* The request is not expected in present state */
#define CSR_BT_RESULT_CODE_AV_NOT_SUPPORTED          ((CsrBtResultCode) (0x800B)) /* The request is not supported by either local or remote device */

/* Media types */
#define CSR_BT_AV_AUDIO                         ((CsrBtAvMedia) 0x00)
#define CSR_BT_AV_VIDEO                         ((CsrBtAvMedia) 0x01)
#define CSR_BT_AV_MULTIMEDIA                    ((CsrBtAvMedia) 0x02)

/* Stream end-point type */
#define CSR_BT_AV_SOURCE                        ((CsrBtAvSep) 0x00)
#define CSR_BT_AV_SINK                          ((CsrBtAvSep) 0x01)

/* Media codecs */
#define CSR_BT_AV_SBC                           ((CsrBtAvCodec) 0x00)
#define CSR_BT_AV_MPEG12_AUDIO                  ((CsrBtAvCodec) 0x01)
#define CSR_BT_AV_MPEG24_AAC                    ((CsrBtAvCodec) 0x02)
#define CSR_BT_AV_ATRAC                         ((CsrBtAvCodec) 0x04)
#define CSR_BT_AV_APTX_AUDIO                    ((CsrBtAvCodec) 0x08)
#define CSR_BT_AV_NON_A2DP_CODEC                ((CsrBtAvCodec) 0xFF)

/* AV media codec location */
#define CSR_BT_AV_CODEC_LOCATION_OFF_CHIP       ((CsrBtAvCodecLoc) 0x00)
#define CSR_BT_AV_CODEC_LOCATION_ON_CHIP        ((CsrBtAvCodecLoc) 0x01)

/* Audio inputs (BC3MM only) */
#define CSR_BT_AV_INPUT_ANALOGUE                ((CsrBtAvInput) 0x00)
#define CSR_BT_AV_INPUT_I2S_MASTER              ((CsrBtAvInput) 0x01)
#define CSR_BT_AV_INPUT_I2S_SLAVE               ((CsrBtAvInput) 0x02)
#define CSR_BT_AV_INPUT_SPDIF                   ((CsrBtAvInput) 0x03)

/* Video codecs: (assigned numbers) */
#define CSR_BT_AV_H263_BASELINE                 ((CsrBtAvCodec) 0x01)
#define CSR_BT_AV_MPEG_VISUAL_SIMPLE_PROFILE    ((CsrBtAvCodec) 0x02)
#define CSR_BT_AV_H263_PROFILE3                 ((CsrBtAvCodec) 0x03)
#define CSR_BT_AV_H263_PROFILE8                 ((CsrBtAvCodec) 0x04)
#define CSR_BT_AV_NON_VDP_CODEC                 ((CsrBtAvCodec) 0xFF)

#define CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE   12

#define CSR_BT_AV_MEDIA_PACKET_HDR_TYPE_RTP     ((CsrBtAvHdrType) 0)
#define CSR_BT_AV_MEDIA_PACKET_HDR_TYPE_NONE    ((CsrBtAvHdrType) 1)

/* Service capability header sizes for video codecs */
#define CSR_BT_H263_MEDIA_CODEC_SC_SIZE         5

/* Defined in spec, same for all H263 profiles */
#define CSR_BT_H263_CAP_LEVEL_10                ((CsrBtAvLevel)0x80)
#define CSR_BT_H263_CAP_LEVEL_20                ((CsrBtAvLevel)0x40)
#define CSR_BT_H263_CAP_LEVEL_30                ((CsrBtAvLevel)0x20)

/* Service capability header sizes for video codecs */
#define CSR_BT_H263_MEDIA_CODEC_SC_SIZE 5

/* Other bits in H263 service capabilities */
#define CSR_BT_MPEG_CAP_LEVEL_0                 ((CsrBtAvLevel)0x80)
#define CSR_BT_MPEG_CAP_LEVEL_1                 ((CsrBtAvLevel)0x40)
#define CSR_BT_MPEG_CAP_LEVEL_2                 ((CsrBtAvLevel)0x20)
#define CSR_BT_MPEG_CAP_LEVEL_3                 ((CsrBtAvLevel)0x10)

/* AV roles */
#define CSR_BT_AV_AUDIO_SOURCE                  ((CsrBtAvRole) 0x00)
#define CSR_BT_AV_AUDIO_SINK                    ((CsrBtAvRole) 0x01)
#define CSR_BT_AV_VIDEO_SOURCE                  ((CsrBtAvRole) 0x02)
#define CSR_BT_AV_VIDEO_SINK                    ((CsrBtAvRole) 0x03)

typedef struct
{
    CsrUint8            acpSeid;
    CsrBool             inUse;
    CsrBtAvMedia        mediaType;
    CsrBtAvSep          sepType;
} CsrBtAvSeidInfo;

#define CSR_BT_AV_SC_MEDIA_TRANSPORT                   ((CsrBtAvServCap) 0x0001)
#define CSR_BT_AV_SC_REPORTING                         ((CsrBtAvServCap) 0x0002)
#define CSR_BT_AV_SC_RECOVERY                          ((CsrBtAvServCap) 0x0003)
#define CSR_BT_AV_SC_CONTENT_PROTECTION                ((CsrBtAvServCap) 0x0004)
#define CSR_BT_AV_SC_HDR_COMPRESSION                   ((CsrBtAvServCap) 0x0005)
#define CSR_BT_AV_SC_MULTIPLEXING                      ((CsrBtAvServCap) 0x0006)
#define CSR_BT_AV_SC_MEDIA_CODEC                       ((CsrBtAvServCap) 0x0007)
#define CSR_BT_AV_SC_DELAY_REPORTING                   ((CsrBtAvServCap) 0x0008)
#define CSR_BT_AV_SC_NEXT                              ((CsrBtAvServCap) 0xFFFF)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_AV_ACTIVATE_REQ                  ((CsrBtAvPrim) (0x0000 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DEACTIVATE_REQ                ((CsrBtAvPrim) (0x0001 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_CONNECT_REQ                   ((CsrBtAvPrim) (0x0002 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_CANCEL_CONNECT_REQ            ((CsrBtAvPrim) (0x0003 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DISCONNECT_REQ                ((CsrBtAvPrim) (0x0004 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DISCOVER_REQ                  ((CsrBtAvPrim) (0x0005 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_CAPABILITIES_REQ          ((CsrBtAvPrim) (0x0006 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SET_CONFIGURATION_REQ         ((CsrBtAvPrim) (0x0007 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_CONFIGURATION_REQ         ((CsrBtAvPrim) (0x0008 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_RECONFIGURE_REQ               ((CsrBtAvPrim) (0x0009 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_OPEN_REQ                      ((CsrBtAvPrim) (0x000A + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_START_REQ                     ((CsrBtAvPrim) (0x000B + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_CLOSE_REQ                     ((CsrBtAvPrim) (0x000C + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SUSPEND_REQ                   ((CsrBtAvPrim) (0x000D + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_ABORT_REQ                     ((CsrBtAvPrim) (0x000E + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_CONTROL_REQ          ((CsrBtAvPrim) (0x000F + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_STREAM_DATA_REQ               ((CsrBtAvPrim) (0x0010 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_REGISTER_STREAM_HANDLE_REQ    ((CsrBtAvPrim) (0x0011 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_IN_REQ               ((CsrBtAvPrim) (0x0012 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_OUT_REQ              ((CsrBtAvPrim) (0x0013 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DISCOVER_RES                  ((CsrBtAvPrim) (0x0014 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_CAPABILITIES_RES          ((CsrBtAvPrim) (0x0015 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SET_CONFIGURATION_RES         ((CsrBtAvPrim) (0x0016 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_CONFIGURATION_RES         ((CsrBtAvPrim) (0x0017 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_RECONFIGURE_RES               ((CsrBtAvPrim) (0x0018 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_OPEN_RES                      ((CsrBtAvPrim) (0x0019 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_START_RES                     ((CsrBtAvPrim) (0x001A + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_CLOSE_RES                     ((CsrBtAvPrim) (0x001B + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SUSPEND_RES                   ((CsrBtAvPrim) (0x001C + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_ABORT_RES                     ((CsrBtAvPrim) (0x001D + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_CONTROL_RES          ((CsrBtAvPrim) (0x001E + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SET_QOS_INTERVAL_REQ          ((CsrBtAvPrim) (0x001F + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_LP_NEG_CONFIG_REQ             ((CsrBtAvPrim) (0x0020 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DELAY_REPORT_REQ              ((CsrBtAvPrim) (0x0021 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_ALL_CAPABILITIES_RES      ((CsrBtAvPrim) (0x0022 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_CHANNEL_INFO_REQ          ((CsrBtAvPrim) (0x0023 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_GET_STREAM_CHANNEL_INFO_REQ   ((CsrBtAvPrim) (0x0024 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_DELAY_REPORT_RES              ((CsrBtAvPrim) (0x0025 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_SET_STREAM_INFO_REQ           ((CsrBtAvPrim) (0x0026 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_PRIM_DOWNSTREAM_HIGHEST                      (0x0026 + CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_AV_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AV_CONNECT_IND                   ((CsrBtAvPrim) (0x0000 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DISCONNECT_IND                ((CsrBtAvPrim) (0x0001 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DISCOVER_IND                  ((CsrBtAvPrim) (0x0002 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_CAPABILITIES_IND          ((CsrBtAvPrim) (0x0003 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SET_CONFIGURATION_IND         ((CsrBtAvPrim) (0x0004 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_CONFIGURATION_IND         ((CsrBtAvPrim) (0x0005 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_RECONFIGURE_IND               ((CsrBtAvPrim) (0x0006 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_OPEN_IND                      ((CsrBtAvPrim) (0x0007 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_START_IND                     ((CsrBtAvPrim) (0x0008 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_CLOSE_IND                     ((CsrBtAvPrim) (0x0009 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SUSPEND_IND                   ((CsrBtAvPrim) (0x000A + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_ABORT_IND                     ((CsrBtAvPrim) (0x000B + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_CONTROL_IND          ((CsrBtAvPrim) (0x000C + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_STREAM_DATA_IND               ((CsrBtAvPrim) (0x000D + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_QOS_IND                       ((CsrBtAvPrim) (0x000E + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_STREAM_MTU_SIZE_IND           ((CsrBtAvPrim) (0x000F + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_STATUS_IND                    ((CsrBtAvPrim) (0x0010 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_ACTIVATE_CFM                  ((CsrBtAvPrim) (0x0011 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DEACTIVATE_CFM                ((CsrBtAvPrim) (0x0012 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_CONNECT_CFM                   ((CsrBtAvPrim) (0x0013 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DISCOVER_CFM                  ((CsrBtAvPrim) (0x0014 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_CAPABILITIES_CFM          ((CsrBtAvPrim) (0x0015 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SET_CONFIGURATION_CFM         ((CsrBtAvPrim) (0x0016 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_CONFIGURATION_CFM         ((CsrBtAvPrim) (0x0017 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_RECONFIGURE_CFM               ((CsrBtAvPrim) (0x0018 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_OPEN_CFM                      ((CsrBtAvPrim) (0x0019 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_START_CFM                     ((CsrBtAvPrim) (0x001A + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_CLOSE_CFM                     ((CsrBtAvPrim) (0x001B + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SUSPEND_CFM                   ((CsrBtAvPrim) (0x001C + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_ABORT_CFM                     ((CsrBtAvPrim) (0x001D + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_CONTROL_CFM          ((CsrBtAvPrim) (0x001E + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_REGISTER_STREAM_HANDLE_CFM    ((CsrBtAvPrim) (0x001F + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_IN_CFM               ((CsrBtAvPrim) (0x0020 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_SECURITY_OUT_CFM              ((CsrBtAvPrim) (0x0021 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DELAY_REPORT_IND              ((CsrBtAvPrim) (0x0022 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_ALL_CAPABILITIES_IND      ((CsrBtAvPrim) (0x0023 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_GET_CHANNEL_INFO_CFM          ((CsrBtAvPrim) (0x0024 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_DELAY_REPORT_CFM              ((CsrBtAvPrim) (0x0025 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AV_PRIM_UPSTREAM_HIGHEST                      (0x0025 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_AV_PRIM_DOWNSTREAM_COUNT         (CSR_BT_AV_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AV_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_AV_PRIM_UPSTREAM_COUNT           (CSR_BT_AV_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_AV_PRIM_UPSTREAM_LOWEST)

/* Not part of interface, used internally only */
#define CSR_BT_AV_LP_SUPERVISION_TIMER          ((CsrBtAvPrim) (0x0026 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_HOUSE_CLEANING                ((CsrBtAvPrim) (0x0027 + CSR_BT_AV_PRIM_UPSTREAM_LOWEST))
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/*************************************************************************************
 Util. structure typedefs
*************************************************************************************/

typedef struct
{
    CsrBtAvCodecLoc          codecLocation; /* Identifies the location of the codec ; on/off-chip */
} CsrBtAvStreamInfo;

/*************************************************************************************
 Primitive typedefs
************************************************************************************/

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrSchedQid                    phandle;                       /* application handle */
    CsrBtAvRole                localRole;                     /* role of the device */
} CsrBtAvActivateReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvActivateCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrBtAvRole                localRole;                     /* role of the device */
} CsrBtAvDeactivateReq;


typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvDeactivateCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrSchedQid                    streamHandle;                  /* application handle for streams */
} CsrBtAvRegisterStreamHandleReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
} CsrBtAvRegisterStreamHandleCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrSchedQid              phandle;                       /* application handle */
    BD_ADDR_T                deviceAddr;                    /* address of device to connect to */
    CsrBtAvRole              remoteRole;                    /* role of the remote device */
    CsrBtAvRole              localRole;                     /* local role indicated to other devices */
} CsrBtAvConnectReq;

typedef struct
{
    CsrBtAvPrim            type;                          /* primitive/message identity */
    CsrUint8               connectionId;                  /* connection instance index */
    BD_ADDR_T              deviceAddr;                    /* address of device connected to */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier          avResultSupplier;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
} CsrBtAvConnectCfm;

typedef struct
{
    CsrBtAvPrim              type;                         /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    BD_ADDR_T              deviceAddr;                     /* address of device connected to */
    CsrBtConnId            btConnId;                       /* Global Bluetooth connection ID */
} CsrBtAvConnectInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    BD_ADDR_T              deviceAddr;                    /* address of device to cancel connect to */
} CsrBtAvCancelConnectReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
} CsrBtAvDisconnectReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrBool                 localTerminated;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtAvDisconnectInd;

typedef struct
{
    CsrBtAvPrim             type;                          /* primitive/message identity */
    CsrUint8                shandle;                       /* stream handle */
    CsrBtAvHdrType          hdr_type;                      /* media packet header type */
    CsrUint16               length;                        /* length of data (in bytes) */
    CsrUint8                *data;                         /* pointer to media packet data */
    CsrUint16               context;
} CsrBtAvStreamDataReq;

typedef struct
{
    CsrBtAvPrim             type;                          /* primitive/message identity */
    CsrUint8                shandle;                       /* stream handle */
    CsrUint8                pad;                           /* padding - to be removed in future release, do not use */
    CsrUint16               pad2;                          /* padding - to be removed in future release, do not use */
    CsrUint16               length;                        /* length of data (in bytes) */
    CsrUint8                *data;                         /* pointer to media packet data */
    CsrUint16               context;
} CsrBtAvStreamDataInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* indication will be send for every 100 samples */
    CsrUint8                shandle;                       /* except if buffer runs full > 8 */
    CsrUint16               bufferStatus;                  /* 0 for buf empty - 10 for full */
} CsrBtAvQosInd;

typedef struct
{
    CsrBtAvPrim             type;                          /* primitive/message identity */
    CsrUint8                shandle;
    l2ca_mtu_t              remoteMtuSize;                 /* holds the remote MTU size.*/
    CsrBtConnId             btConnId;
} CsrBtAvStreamMtuSizeInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrBtAvPrim              statusType;
    CsrBtAvRole                roleType;
    CsrSchedQid                    appHandle;
} CsrBtAvStatusInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
} CsrBtAvDiscoverReq;

typedef CsrBtAvDiscoverReq CsrBtAvDiscoverInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                seidInfoCount;                 /* number of CsrBtAvSeidInfo elements in "seidInfo" */
    CsrBtAvSeidInfo           *seidInfo;                     /* pointer to array of "seidInfoCount" number of elements */
    CsrBtAvResult          avResponse;
} CsrBtAvDiscoverRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                seidInfoCount;                 /* number of CsrBtAvSeidInfo elements in "seidInfo" */
    CsrBtAvSeidInfo        *seidInfo;                     /* pointer to array of "seidInfoCount" number of elements */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvDiscoverCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                acpSeid;
} CsrBtAvGetCapabilitiesReq;

typedef CsrBtAvGetCapabilitiesReq CsrBtAvGetCapabilitiesInd;
typedef CsrBtAvGetCapabilitiesReq CsrBtAvGetAllCapabilitiesInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint16               servCapLen;
    CsrUint8                *servCapData;                  /* only pmallocated if not error */
    CsrBtAvResult          avResponse;
} CsrBtAvGetCapabilitiesRes;

typedef CsrBtAvGetCapabilitiesRes CsrBtAvGetAllCapabilitiesRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint16               servCapLen;
    CsrUint8                *servCapData;                  /* only pmallocated if not error */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvGetCapabilitiesCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                acpSeid;
    CsrUint8                intSeid;
    CsrUint16               appServCapLen;
    CsrUint8                *appServCapData;
} CsrBtAvSetConfigurationReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                shandle;
    CsrUint8                acpSeid;
    CsrUint8                intSeid;
    CsrUint16               servCapLen;
    CsrUint8                *servCapData;
} CsrBtAvSetConfigurationInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint8                servCategory;                  /* valid if result is a reject */
    CsrBtAvResult          avResponse;
} CsrBtAvSetConfigurationRes;

/* if result is SIGNAL_TIME_OUT the following signals are invalid */
typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                shandle;                       /* Only valid if result is accept */
    CsrUint8                servCategory;                  /* valid if result is a reject */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvSetConfigurationCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvGetConfigurationReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvGetConfigurationInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                shandle;
    CsrUint16               servCapLen;                    /* the following elements are only */
    CsrUint8                *servCapData;                  /* valid if result is an accept */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvGetConfigurationCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               servCapLen;                    /* the following elements are only */
    CsrUint8                *servCapData;                  /* valid if result is an accept */
    CsrBtAvResult          avResponse;
} CsrBtAvGetConfigurationRes;


typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               servCapLen;
    CsrUint8                *servCapData;
} CsrBtAvReconfigureReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               servCapLen;
    CsrUint8                *servCapData;
} CsrBtAvReconfigureInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint8                servCategory;                  /* valid if result is a reject */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvReconfigureCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint8                servCategory;                  /* valid if result is a reject */
    CsrBtAvResult          avResponse;
} CsrBtAvReconfigureRes;


typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvOpenReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvOpenInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrBtAvResult          avResponse;
} CsrBtAvOpenRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvOpenCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
} CsrBtAvStartReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
} CsrBtAvStartInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                reject_shandle;                /* only valid if result is reject */
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
    CsrBtAvResult          avResponse;
} CsrBtAvStartRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                reject_shandle;                /* only valid if result is reject */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvStartCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvCloseReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvCloseInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrBtAvResult          avResponse;
} CsrBtAvCloseRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                shandle;                       /* stream handle */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvCloseCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
} CsrBtAvSuspendReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
} CsrBtAvSuspendInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                connectionId;                  /* connection instance index */
    CsrUint8                tLabel;
    CsrUint8                reject_shandle;                /* only valid if result is reject */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvSuspendCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                tLabel;
    CsrUint8                reject_shandle;                /* only valid if result is reject */
    CsrUint8                listLength;                    /* length of pointer */
    CsrUint8                *list;                         /* array of stream handles */
    CsrBtAvResult          avResponse;
} CsrBtAvSuspendRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvAbortReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvAbortInd;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
} CsrBtAvAbortRes;

typedef CsrBtAvAbortRes CsrBtAvAbortCfm;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               contProtMethodLen;
    CsrUint8                *contProtMethodData;
} CsrBtAvSecurityControlReq;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               contProtMethodLen;
    CsrUint8                *contProtMethodData;
} CsrBtAvSecurityControlInd;


typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               contProtMethodLen;             /* The following elements are only */
    CsrUint8                *contProtMethodData;           /* valid if result is a accept */
    CsrBtAvResult          avResponse;
} CsrBtAvSecurityControlRes;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
    CsrUint8                shandle;
    CsrUint8                tLabel;
    CsrUint16               contProtMethodLen;             /* The following elements are only */
    CsrUint8                *contProtMethodData;           /* valid if result is a accept */
    CsrBtResultCode        avResultCode;
    CsrBtSupplier    avResultSupplier;
} CsrBtAvSecurityControlCfm;

typedef struct
{
    CsrBtAvPrim              type;
} CsrBtAvHouseCleaning;

typedef struct
{
    CsrBtAvPrim              type;                          /* primitive/message identity */
} CsrBtAvLpSupervisionTimer;

typedef struct
{
    CsrBtAvPrim              type;
    CsrSchedQid                    appHandle;
    CsrUint16               secLevel;
} CsrBtAvSecurityInReq;

typedef struct
{
    CsrBtAvPrim              type;
    CsrSchedQid                    appHandle;
    CsrUint16               secLevel;
} CsrBtAvSecurityOutReq;

typedef struct
{
    CsrBtAvPrim              type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtAvSecurityInCfm;

typedef struct
{
    CsrBtAvPrim              type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtAvSecurityOutCfm;

typedef struct
{
    CsrBtAvPrim              type;
    CsrUint16               qosInterval;
} CsrBtAvSetQosIntervalReq;

typedef struct
{
    CsrBtAvPrim               type;
    CsrBool                  enable;
} CsrBtAvLpNegConfigReq;


typedef struct
{
    CsrBtAvPrim               type;
    CsrUint8                  tLabel;
    CsrUint16                 delay;
    CsrUint8                  shandle;
}CsrBtAvDelayReportReq;

typedef CsrBtAvDelayReportReq CsrBtAvDelayReportInd;

typedef struct
{
    CsrBtAvPrim               type;
    CsrUint8                  tLabel;
    CsrUint8                  shandle;
    CsrBtAvResult             avResponse;
}CsrBtAvDelayReportRes;

typedef struct
{
    CsrBtAvPrim               type;
    CsrUint8                  tLabel;
    CsrUint8                  shandle;
    CsrBtResultCode           avResultCode;
    CsrBtSupplier             avResultSupplier;
}CsrBtAvDelayReportCfm;

typedef struct
{
    CsrBtAvPrim              type;
    CsrBtConnId              btConnId;
} CsrBtAvGetChannelInfoReq;

typedef struct
{
    CsrBtAvPrim              type;
    CsrUint8                 shandle;
} CsrBtAvGetStreamChannelInfoReq;

typedef struct
{
    CsrBtAvPrim               type;
    CsrBtAvPrim               aclHandle;
    CsrBtAvPrim               remoteCid;
    CsrBtResultCode           resultCode;
    CsrBtSupplier             resultSupplier;
} CsrBtAvGetChannelInfoCfm;

typedef struct
{
    CsrBtAvPrim              type;
    CsrUint8                 shandle; /* stream handle */
    CsrBtAvStreamInfo        sInfo;
} CsrBtAvSetStreamInfoReq;

#ifdef __cplusplus
}
#endif

#endif
