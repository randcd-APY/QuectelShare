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


*******************************************************************************/
/*!
  \file
  \brief  Helper functions for opening and initialising an DV-258 instance  $Revision: $
*/

#ifndef DV258_HELPER_H
#define DV258_HELPER_H

#ifdef DOLBY_VOLUME_LIBRARY

#include "ak.h"


/*!
  \brief Frees up memory allocated and used by Dolby Volume 258
*/
int DolbyVolume258Free(ak_instance*    ak,      /*!< pointer to an audio kernel instance */
                       void*          scratch,  /*!< pointer to the scratch memory */
                       ak_memory_pool  *pool,   /*!< pointer to a pool of static memory blocks */
                       void*           *pmem);  /*!< pointer to static memory */

/*!
  \brief Dolby Volume 258 memory allocation and initialization
*/
int DolbyVolume258Open(ak_instance**    pAk,
                       void**           pScratch,
                       ak_memory_pool **pPool,
                       void*          **pPmem,
                       int              maxChannels);

/*! \brief Prints out recursively the audio kernel tree nodes and parameter values */
void DolbyVolume258DumpNodes(ak_instance *ak, /*!< pointer to an audio kernel instance */
                             int node,        /*!< handle to the current ak node */
                             int level);      /*!< tree depth of the current ak node */


/*! \brief Sets the amount of leveling for Dolby Volume 258 */
void DolbyVolume258SetLevelingAmount(ak_instance *ak, /*!< pointer to an audio kernel instance */
                                     int levAmount);  /*!< amount of leveling to be set (lvla) */

#endif /* DOLBY_VOLUME */

#endif /* #define DV258_HELPER_H */
