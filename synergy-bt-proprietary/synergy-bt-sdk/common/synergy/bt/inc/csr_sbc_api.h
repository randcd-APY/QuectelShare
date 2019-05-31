#ifndef CSR_SBC_API_H__
#define CSR_SBC_API_H__

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

#include "csr_types.h"

#define CSR_SBC_MAX_BLOCKS                  16
#define CSR_SBC_MAX_CHANNELS                2
#define CSR_SBC_MAX_SUBBANDS                8

#define CSR_SBC_MAX_WINDOW                  80
#define CSR_SBC_BLOCKS_IN_WINDOW   (CSR_SBC_MAX_WINDOW/8)

#define CSR_SBC_MAX_AUDIO_FRAME_SIZE_BYTES  512
#define CSR_SBC_MAX_FRAME_SIZE_BYTES        512

typedef CsrUint8 CsrSbcChannelMode;
#define CSR_SBC_MONO            ((CsrSbcChannelMode) 0)
#define CSR_SBC_DUAL            ((CsrSbcChannelMode) 1)
#define CSR_SBC_STEREO          ((CsrSbcChannelMode) 2)
#define CSR_SBC_JOINT_STEREO    ((CsrSbcChannelMode) 3)

typedef CsrUint8 CsrSbcAllocMethod;
#define CSR_SBC_METHOD_LOUDNESS ((CsrSbcAllocMethod) 0)
#define CSR_SBC_METHOD_SNR      ((CsrSbcAllocMethod) 1)

typedef CsrInt16                         CsrSbcFixpt16;
typedef CsrInt32                         CsrSbcFixpt32;

typedef CsrSbcFixpt32 CsrSbcFixptVector;

/****************************************************************************
  CsrSbcOpen - allocate SBC stream handle

  Returns:
    * void pointer to SBC stream handle

  This function allocates and initializes an SBC stream handle.  The
  function returns a pointer to the allocated stream handle is.  This
  stream handle should later be deallocated using CsrSbcClose() when no
  longer needed.

****************************************************************************/
void *CsrSbcOpen(void);


/****************************************************************************
  CsrSbcClose - deallocate SBC stream data

  Inputs:
    * pointer to pointer

  Returns:
    * void, i.e. nothing.

  This function is passed a pointer to a pointer to an SBC stream
  handle previously allocated using CsrSbcOpen().  Upon returning,
  the pointer passed to the function will point at a NULL
  pointer, indicating that the stream handle has been deallocated.

****************************************************************************/
void CsrSbcClose(void **hdl);


/****************************************************************************
  CsrSbcCalcBitPool - calculate the bitpool value

  Returns:
    * the main bitpool
    * the alternative bitpool (via the first argument)
    * the toggle period (via the second argument)

  Notes:
    If the bitpool needed to give a more accurate bit rate
    (i.e. closer to that required by the user) is not too close
    to a whole number [1],  then the toggle period is set to
    greater than zero and an alternate bitpool is specified.
    On the application level the main bitpool can then be made to
    switch to the alternative bitpool after every
    'toggle period'/2  frames.  The application may alternatively
    choose not to use a variable bitpool.

    [1] In this version of the codec the algoritm used in working
    out whether the bitpool falls 'close to a whole number' is
    very simple. When the bitpool is calculated, if it falls in
    the middle half     between two whole numbers (i.e.>1/4 & <3/4),
    the toggle period is set to two and the alternative bitpool
    is 'the main bitpool'+1.
****************************************************************************/
CsrUint8 CsrSbcCalcBitPool(CsrUint8 *bitPoolAlt, CsrUint8 *togglePeriod,
                      CsrSbcChannelMode channel_mode,
                      CsrUint16 sample_freq,
                      CsrUint8 nof_blocks,
                      CsrUint8 nof_subbands,
                      CsrUint16 bitrate);


/****************************************************************************
  CsrSbcConfig - configure the settings required before encoding

  Returns:
    frame size in bytes if inputs are valid else returns zero.

  Notes:
    * sample_freq must be either 16000, 32000, 44100 or 48000.
    * nof_subbands must either 4 or 8.
    * nof_blocks must be either 4, 8, 12 or 16.
    * bitpool must be greater than 2 and no greater than
       16 * nof_subbands for MONO and DUAL_CHANNEL channel modes
    or 32 * nof_subbands for STEREO and JOINT_STEREO channel modes

    If all inputs are correct, all the appropriate settings will be set.
    This function does not need to be called before decoding.
****************************************************************************/
CsrUint16 CsrSbcConfig(void *hdl,
                  CsrSbcChannelMode channel_mode,
                  CsrSbcAllocMethod alloc_method,
                  CsrUint16 sample_freq,
                  CsrUint8 nof_blocks,
                  CsrUint8 nof_subbands,
                  CsrUint8 bitpool);

/****************************************************************************
  CsrSbcGetChannelMode,
  CsrSbcGetAllocMethod,
  CsrSbcGetSampleFreq,
  CsrSbcGetNumBlocks,
  CsrSbcGetNumSubBands
  CsrSbcGetBitPool - internal SBC control data access functions

  Inputs:
    * pointer to SBC stream handle

  Returns:
    * the desired SBC control configuration parameter

****************************************************************************/
CsrSbcChannelMode CsrSbcGetChannelMode(void *hdl);
CsrSbcAllocMethod CsrSbcGetAllocMethod(void *hdl);
CsrUint16 CsrSbcGetSampleFreq(void *hdl);
CsrUint8 CsrSbcGetNumBlocks(void *hdl);
CsrUint8 CsrSbcGetNumSubBands(void *hdl);
CsrUint8 CsrSbcGetBitPool(void *hdl);


/****************************************************************************
  CsrSbcEncode - encodes a single sbc frame

  Inputs:
    * pointer to the SBC stream handle
    * pointer to the input wav samples casted to a (CsrInt16 (*)[2])
    * char* to the frame where sbc frame will be written
    * bitpool to be used for encoding the frame

  Returns:
    frame size in bytes of the sbc frame just encoded
****************************************************************************/
CsrUint16 CsrSbcEncode(void *hdl,
                  const CsrInt16 audio_samples[][2],
                  CsrUint8 *frame,
                  CsrUint8 bit_pool);


/****************************************************************************
  CsrSbcInitDecoder - initialise the decoder ready to accept another file.

  Input:
    * pointer to the SBC stream handle

  Returns:
    nothing
****************************************************************************/
void CsrSbcInitDecoder(void *hdl);


/****************************************************************************
  CsrSbcReadHeader - verify the sbc frame header before attempting to decode
                        the frame

  Input:
    * pointer to the SBC stream handle
    * char* to the frame to be read

  Returns:
    * frame size if frame is valid else returns zero.
****************************************************************************/
CsrUint16 CsrSbcReadHeader(void *hdl, CsrUint8 *frame);


/****************************************************************************
  CsrSbcDecode - decodes a single sbc frame

  Inputs:
    * pointer to the SBC stream handle
    * char* to the frame where sbc frame to be decoded begins
    * pointer to the buffer storing decoded wav samples
****************************************************************************/
void CsrSbcDecode(
    void *hdl,
    CsrUint8 *frame,
    CsrInt16 audio_samples[CSR_SBC_MAX_BLOCKS][CSR_SBC_MAX_CHANNELS][CSR_SBC_MAX_SUBBANDS]);

#ifdef __cplusplus
}
#endif

#endif /* CSR_SBC_API_H__ */
