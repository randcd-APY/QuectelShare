#ifndef CSR_BT_AAC_PARSER_H__
#define CSR_BT_AAC_PARSER_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_platform.h"

/****  types ****/
/* #ifndef CsrBool */
/*     typedef unsigned        char    CsrBool; */
/* #endif */
/* #ifndef CsrUint8 */
/*     typedef unsigned        char    CsrUint8; */
/* #endif */
/* #ifndef CsrUint16 */
/*     typedef unsigned        short   CsrUint16; */
/* #endif */
/* #ifndef CsrUint32 */
/*     typedef unsigned        int     CsrUint32; */
/* #endif */
/* #ifndef CsrPmemZalloc */
/* #define CsrPmemZalloc malloc */
/* #endif */
/* #ifndef pmalloc */
/* #define pmalloc malloc */
/* #define CsrPmemFree    free */
/* #endif */

/* #ifndef TRUE */
/* #define TRUE 1 */
/* #endif */
/* #ifndef FALSE */
/* #define FALSE 0 */
/* #endif */
/* #ifndef CsrTime */
/*     typedef CsrUint32   CsrTime; */
/* #endif */

    /********** PUBLIC FUNCTIONS ***********/

    /************************************************************************
     * NAME: AACInitParser
     * ARGS: filename as string
     * RETS: TRUE if success, else FALSE.
     * DESC: Init parser and relevant structures. opens AAC file.
     *       This function needs to be called before using the parser.
     ************************************************************************/
    CsrBool   AACInitParser(char* file);



    /************************************************************************
     * NAME: AACDeInitParser
     * ARGS: none
     * RETS: TRUE os success, else FALSE.
     * DESC: DeInit parser and free relevant structures. close AAC file.
     *       This function should be called after end usage of the parser.
     ************************************************************************/
    CsrBool   AACDeInitParser();



    /************************************************************************
     * NAME: AACGetNextFrame
     * ARGS: pointer address where the frame data should be stored.
     * RETS: TRUE if new data is store in *frame, else FALSE.
     * DESC: Gets the next frame payload data and stores it in *frame.
     *       Headers from the frame are updated into the structures in the
     *       parser.
     ************************************************************************/
    CsrBool   AACGetNextFrame(CsrUint8 **frame);



    /************************************************************************
     * NAME: AACGetLatmPacket
     * ARGS: pointer address where the packet should be stored
     *       maxSize the packet can reach.
     *       pointer to CsrTime where duration of returned data will be stored
     * RETS: actual size of packet data.
     * DESC: Gets the amount of frame you can have in a packet of size maxSize
     *       An LATM header is added in the beginning of the packet.
     *       If one frame cannot be inside maxSize, then a larger packet
     *       is created (check return value against maxSize).
     ************************************************************************/
    CsrUint16 AACGetLatmPacket(void **packetIn,
                              CsrUint16 maxSize,
                              CsrTime *duration);


    /************************************************************************
     * NAME: AACGetSampleRate
     * ARGS: none
     * RETS: samplerate for current frame
     * DESC: Gets the samplerate of the last read frame.
     ************************************************************************/
    int      AACGetSampleRate();


    /************************************************************************
     * NAME: AACGetDuration
     * ARGS: none
     * RETS: duration for current frame
     * DESC: Gets the duration  of the last read frame.
     ************************************************************************/
    CsrTime     AACGetDuration();


    /************************************************************************
     * NAME: AACGetChannels
     * ARGS: none
     * RETS: number of channels in frame
     * DESC: Gets the number of channels in a frame
     ************************************************************************/
    CsrUint8  AACGetChannels();


    /************************************************************************
     * NAME: AACGetFrameLength
     * ARGS: none
     * RETS: length of the frame payload.
     * DESC: Gets the payload length in bytes of the last read frame.
     ************************************************************************/
    CsrUint16 AACGetFrameLength();


    /************************************************************************
     * NAME: AACGetBitRate
     * ARGS: none
     * RETS: bitrate for current frame if ADIF format, else 0
     * DESC: Gets the bitrate for the last read frame.
     ************************************************************************/
    CsrUint32 AACGetBitRate();

    /************************************************************************
     * NAME: AACGetObjectType
     * ARGS: none
     * RETS: Objecttype of current frame
     * DESC: Gets the object type of the current frame.
     ************************************************************************/
    CsrUint8 AACGetObjectType();

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AAC_PARSER_H__ */

