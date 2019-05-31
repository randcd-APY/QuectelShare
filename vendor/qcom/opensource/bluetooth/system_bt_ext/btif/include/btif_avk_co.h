/******************************************************************************
 *  Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 *
 *  Not a contribution.
 ******************************************************************************/

/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef BTIF_AVK_CO_H
#define BTIF_AVK_CO_H

#include "btif_avk_media.h"

/*******************************************************************************
**  Constants & Macros
********************************************************************************/

/*******************************************************************************
**  Functions
********************************************************************************/

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_codec_reset
 **
 ** Description      Reset the current codec configuration
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_codec_reset(void);

/*******************************************************************************
 **
 ** Function         bta_avk_co_init
 **
 ** Description      Initialization
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void bta_avk_co_init(void);

/*******************************************************************************
 **
 ** Function         bta_avk_get_current_codec
 **
 ** Description      Get the current codec type.
 **
 ** Returns          Codec Type Value
 **
 *******************************************************************************/
UINT8 bta_avk_get_current_codec();

#endif
