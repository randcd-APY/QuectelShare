/****************************************************************************

 This program is protected under international copyright laws as an
 unpublished work. Do not copy.

                    (C) Copyright Dolby International AB (2011)
                            All Rights Reserved

 This software is company confidential information and the property of
 Dolby International AB, and can not be reproduced or disclosed in any form
 without written authorization of Dolby International AB.

 Those intending to use this software module for other purposes are advised
 that this infringe existing or pending patents. Dolby International AB has no
 liability for use of this software module or derivatives thereof in any
 implementation. Copyright is not released for any means. Dolby International AB
 retains full right to use the code for its own purpose, assign or sell the
 code to a third party and to inhibit any user or third party from using the
 code. This copyright notice must be included in all copies or derivative
 works.

 $Id$

*******************************************************************************/
/*!
  \file callback_258.h
  \brief  Typedefs and \#defines used by the example callback functions for 258 integration

  Dolby Volume is always called with the maximum number of allowed channels. This prevents a full reset
  of Dolby Volume if the number of channels changes, thus allowing seamless switching between channel
  modes.
  The only exception is the dual mono channel mode where two mono instances are run for each channel.
*/

#ifndef CALLBACK_258_H
#define CALLBACK_258_H


#ifdef DOLBY_VOLUME_LIBRARY
#include "ak.h"
#endif
#include "ddt_ddre_common.h"


#define DV_258_OK  0  /*!< Return code for successful operation */
#define DV_258_MAX_CHANNELS 6
#define DV_258_DELAY 256

/*! \brief  Static data accessible in the dolby volume callback function

  A reference to this structure is supplied to each callback call and it contains all
  static data needed by the callback.
*/
typedef struct DV_258_DATA {
#ifdef DOLBY_VOLUME_LIBRARY
  void*              pScratch;        /*!< Pointer to scratch memory for temporary buffers */
  ak_instance*       pAkInstance[2];  /*!< Static data for an audio kernel instance, 2nd is for dual mono processing */
#else
  DLB_SFRACT        delayBuffer[DV_258_DELAY * DV_258_MAX_CHANNELS];  /*!< Used to simulate the delay of DV-258 */
#endif
  TRANSCODER_CHANNEL_CONFIG prevChannelConfig; /*!< Channel config of previous frame */
} DV_258_DATA;


typedef struct DV_258_DATA * HANDLE_DV_258;


/* Prototype of callback function for the DV_258. This must match the DOLBY_VOLUME_CALLBACK type. */
//int Dv258AudioCallback(HANDLE_DV_258  hDv258,                       /*!< Callback data (e.g. instance handles) */
//                       DLB_SFRACT ** ppInOutPCM,                   /*!< Pointer to an array of PCM buffers in cinema order */
//                       uint16_t     inOutLen,                     /*!< Number of samples per channel */
//                       TRANSCODER_CHANNEL_CONFIG * pChannelConfig); /*!< Current channel configuration */

#endif /* #define CALLBACK_H */
