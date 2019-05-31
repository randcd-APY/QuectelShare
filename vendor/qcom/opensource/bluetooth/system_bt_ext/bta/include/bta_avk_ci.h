/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  This is the interface file for advanced audio/video call-in functions.
 *
 ******************************************************************************/
#ifndef BTA_AVK_CI_H
#define BTA_AVK_CI_H

#include "bta_avk_api.h"

/*****************************************************************************
**  Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         bta_avk_ci_src_data_ready
**
** Description      This function sends an event to the AV indicating that
**                  the phone has audio stream data ready to send and AV
**                  should call bta_avk_co_audio_src_data_path() or
**                  bta_avk_co_video_src_data_path().
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_ci_src_data_ready(tBTA_AVK_CHNL chnl);

/*******************************************************************************
**
** Function         bta_avk_ci_setconfig
**
** Description      This function must be called in response to function
**                  bta_avk_co_audio_setconfig() or bta_avk_co_video_setconfig.
**                  Parameter err_code is set to an AVDTP status value;
**                  AVDT_SUCCESS if the codec configuration is ok,
**                  otherwise error.
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_ci_setconfig(tBTA_AVK_HNDL hndl, UINT8 err_code,
                                        UINT8 category, UINT8 num_seid, UINT8 *p_seid,
                                        BOOLEAN recfg_needed, UINT8 avdt_handle);


#ifdef __cplusplus
}
#endif

#endif /* BTA_AVK_CI_H */
