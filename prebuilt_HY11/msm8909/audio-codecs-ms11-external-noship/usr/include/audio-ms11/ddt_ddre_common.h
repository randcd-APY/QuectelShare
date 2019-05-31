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


****************************************************************************/
/*!
  \file   ddt_ddre_common.h
  \brief  Common types for DDT and DDRE $Revision: #1 $
*/

#ifndef DDT_DDRE_COMMON_H
#define DDT_DDRE_COMMON_H

/*! dsurmod */
#define  DSURMOD_NO_INDICATION    (0) /*!< dsurmod not indicated           */
#define  DSURMOD_NOT_SURR_ENCODED (1) /*!< signal is not surround encoded  */
#define  DSURMOD_SURR_ENCODED     (2) /*!< signal is surround encoded      */
#define  DSURMOD_RESERVED         (3) /*!< reserved                        */


/*!
  Channel configuration for Dolby Digital output, 6 channel output and mixer callback
*/
typedef struct TRANSCODER_CHANNEL_CONFIG {
  uint16_t acmod;        /*!< Audio coding mode as defined for Dolby Digital.
                               The audio coding mode associated with the output from the Dolby Pulse Decoder-Transcoder
                               can be DD_ONEPLUSONE=0, DD_MONO=1, DD_STEREO=2, or DD_3_2=7. */
  uint16_t bLfePresent;  /*!< Flag indicating presence of the low frequency enhancement channel. */
  uint16_t dsurmod;      /*!< Indicates if the signal is dolby surround encoded (only possible for stereo signals) */
} TRANSCODER_CHANNEL_CONFIG;


#endif
