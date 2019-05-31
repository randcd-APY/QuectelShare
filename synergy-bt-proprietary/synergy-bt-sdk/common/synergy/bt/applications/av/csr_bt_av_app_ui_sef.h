#ifndef CSR_BT_AV_APP_UI_SEF_H__
#define CSR_BT_AV_APP_UI_SEF_H__

/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#ifndef CSR_BT_APP_MPAA_ENABLE

#include "csr_bt_av2.h"
#include "csr_bt_av_app_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* generel key defines                                  */
#define CSR_BT_AV_FIRST_DYNAMIC_KEY                                    (0x0001)
#define CSR_BT_AV_INVALID_KEY                                          (0x7FFF)

/* key defines for the AV menu UI                    */
#define CSR_BT_AV_ACTIVATE_KEY                                         (0x0000)
#define CSR_BT_AV_MODE_KEY                                             (0x0001)
#define CSR_BT_AV_CONNECTION_MODE_KEY                                  (0x0002)
#define CSR_BT_AV_RF_SHIELD_ON_KEY                                     (0x0003)
#define CSR_BT_AV_RF_SHIELD_OFF_KEY                                    (0x0004)
#define CSR_BT_AV_DISPLAY_CURRENT_CON_KEY                              (0x0005)
#define CSR_BT_AV_ESTABLISH_CONN_KEY                                   (0x0006)
#define CSR_BT_AV_TOGGLE_RF_KEY                                        (0x0007)
#define CSR_BT_AV_CON_SINGLE_KEY                                       (0x0008)
#define CSR_BT_AV_CON_MULTIPLE_KEY                                     (0x0009)
#define CSR_BT_AV_SOURCE_MODE_KEY                                      (0x000A)
#define CSR_BT_AV_SINK_MODE_KEY                                        (0x000B)
#define CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_KEY                       (0x000C)
#define CSR_BT_AV_START_OR_RESUME_STREAM_KEY                           (0x000D)
#define CSR_BT_AV_RECONFIGURE_STREAM_MENU_KEY                          (0x000E)
#define CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_KEY              (0x000F)
#define CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_KEY                  (0x0010)
#define CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_KEY             (0x0011)
#define CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_KEY          (0x0012)
#define CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_KEY                  (0x0013)
#define CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_KEY                    (0x0014)
#define CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_KEY                  (0x0015)
#define CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_KEY                  (0x0016)
#define CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_KEY                  (0x0017)
#define CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_KEY               (0x0018)
#define CSR_BT_AV_CLOSE_STREAM_KEY                                     (0x0019)
#define CSR_BT_AV_GET_STREAM_CONFIG_KEY                                (0x001A)
#define CSR_BT_AV_ABORT_OPEN_STREAM_MENU_KEY                           (0x001B)
#define CSR_BT_AV_DISCONNECT_KEY                                       (0x001C)
#define CSR_BT_AV_ABORT_KEY                                            (0x001D)
#define CSR_BT_AV_FILTER_MENU_KEY                                      (0x001E)
#define CSR_BT_AV_CSR_WINDOW_GRABBER_KEY                               (0x001F)
#define CSR_BT_AV_SBC_FILE_STREAMER_KEY                                (0x0020)
#define CSR_BT_AV_CHANGE_FILENAME_KEY                                  (0x0021)
#define CSR_BT_AV_START_OF_CSR_SBC_ENCODER_MENU_KEY                    (0x0022)
#define CSR_BT_AV_CSR_SBC_ENCODER_MENU_KEY                             (0x0023)
#define CSR_BT_AV_SBC_CAPABILITIES_KEY                                 (0x0024)
#define CSR_BT_AV_CHANNEL_MODE_KEY                                     (0x0025)
#define CSR_BT_AV_ALLOCATION_METHOD_KEY                                (0x0026)
#define CSR_BT_AV_PCM_SAMPLE_FREQUENCY_KEY                             (0x0027)
#define CSR_BT_AV_PCM_CHANNELS_KEY                                     (0x0028)
#define CSR_BT_AV_SBC_BLOCKS_KEY                                       (0x0029)
#define CSR_BT_AV_SBC_SUBBANDS_KEY                                     (0x002A)
#define CSR_BT_AV_MINM_BITPOOL_KEY                                     (0x002B)
#define CSR_BT_AV_MAXM_BITPOOL_KEY                                     (0x002C)
#define CSR_BT_AV_MAXM_FRAME_SIZE_KEY                                  (0x002D)
#define CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY                           (0x002E)
#define CSR_BT_AV_CSR_SBC_DECODER_MENU_KEY                             (0x002F)
#define CSR_BT_AV_CSR_WINDOWS_PLAYBACK_KEY                             (0x0030)
#define CSR_BT_AV_CSR_SBCFILE_WRITER_KEY                               (0x0031)
#define CSR_BT_AV_CSR_SINK_TERMINATOR_KEY                              (0x0032)
#define CSR_BT_AV_SUSPEND_STREAM_KEY                                   (0x0033)
#define CSR_BT_AV_CLOSE_STREAM_START_OR_RESUME_STREAM_KEY              (0x0034)
#define CSR_BT_AV_GET_STREAM_CONFIG_START_OR_RESUME_STREAM_KEY         (0x0035)
#define CSR_BT_AV_ABORT_START_OR_RESUME_STREAM_KEY                     (0x0036)
#define CSR_BT_AV_TOGGLE_CONNECTION_KEY                                (0x0037)
#define CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY                          (0x0038)
#define CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY                             (0x0039)
#define CSR_BT_AV_CANCEL_FILTER_SELECTION_RECONFIGURE_KEY              (0x003A)
#define CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY              (0x003B)
#define CSR_BT_AV_CHANGE_ROLE_KEY                                      (0x003C)
#define CSR_BT_AV_MAIN_MENU_KEY                                        (0x003D)
#define CSR_BT_AV_MAIN_MENU_DEACTIVATE_KEY                             (0x003E)

/* app csrUi event states                                                 */
#define CSR_BT_AV_CREATE_SK1_EVENT                                     (0x01)
#define CSR_BT_AV_CREATE_SK2_EVENT                                     (0x02)
#define CSR_BT_AV_CREATE_DEL_EVENT                                     (0x03)
#define CSR_BT_AV_CREATE_BACK_EVENT                                    (0x04)

/* Handler functions for the CSR_UI upstream messages                           */
extern void CsrBtAvCsrUiUieCreateCfmHandler(av2instance_t * inst);
extern void CsrBtAvCsrUiInputdialogGetCfmHandler(av2instance_t * inst);
extern void CsrBtAvCsrUiEventIndHandler(av2instance_t * inst);
extern void CsrBtAvAppUiAllowDeactivate(av2instance_t *inst);
extern void CsrBtAvAppUiDisallowDeactivate(av2instance_t *inst);




#ifdef __cplusplus
}
#endif

#endif

#endif /* CSR_BT_GAP_APP_UI_SEF_H__ */
