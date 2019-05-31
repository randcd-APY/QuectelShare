/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc..
=============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  DRExternal generic definition header

GENERAL DESCRIPTION
 This file contains generic definition header for Dead Reckoning API interfaces.

=============================================================================*/
#ifndef __DRPLUGIN_CLIENT_H
#define __DRPLUGIN_CLIENT_H

#include "dr_plugin_msg.h"

#define SOCKET_NAME "/data/vendor/location/mq/location-APDR-s"

/* Logging */
#define LOG_DEBUG 1

#ifdef LOG_DEBUG
#define DR_LOGV(...) LOC_LOGV("DR_CLNT: V/" __VA_ARGS__)
#define DR_LOGD(...) LOC_LOGD("DR_CLNT: D/" __VA_ARGS__)
#define DR_LOGI(...) LOC_LOGI("DR_CLNT: I/" __VA_ARGS__)
#define DR_LOGW(...) LOC_LOGW("DR_CLNT: W/" __VA_ARGS__)
#define DR_LOGE(...) LOC_LOGE("DR_CLNT: E/" __VA_ARGS__)
#else /* log Debug enable */
#define DR_LOGV(...) printf(__VA_ARGS__)
#define DR_LOGD(...) printf(__VA_ARGS__)
#define DR_LOGI(...) printf(__VA_ARGS__)
#define DR_LOGW(...) printf(__VA_ARGS__)
#define DR_LOGE(...) printf(__VA_ARGS__)
#endif /* log Debug enable */

/* Retry error socket 200 Millisec */
#define ERROR_SOCKET_TIME (200*1000)

#define DRCLT_MAX_NMEA_SZ 512 // Max NMEA Size

typedef struct GnssSetPosModeReq_str {
    LocGpsPositionMode  posMode;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    uint32_t preferred_time;
}GnssSetPosModeReq;

/**
 * @brief Initialization function of AP DR engine.
 *        Parameter is the callback function to DRPlugin Component
 *
 * @param dr_callbacks_t Register Callback function for report
 *
 * @return bool In case of success, it return true otherwise false.
 *         true denotes the DR engine is in ready state.
 *
 */
bool drClient_init(dr_callbacks_t *drPluginCallbacks);

/**
 * @brief De-initialize DR
 *
 * @return bool In case of success, it return true otherwise false.
 *
 */
bool drClient_deInit(void);


/**
  @brief Start message receiving thread from AP DR.

  This function creates thread.

  @param   void none.
  @return true on success; false on error.
*/
bool drClient_initIpcRxMsg(void);

/**
  @brief Process Position Report received from AP DR.

  This function receives Position report from AP DR.
  The received Position report sends to DR Plugin.

  @param[in]  *input      Context Info Pointer.
  @param[in]  rx_srv_msg  Received Service Message.
  @return true on success; false on error.
*/
bool drClient_prcoss_posRpt(void *input, dr_plugin_msg *rx_srv_msg);

/**
  @brief Process NMEA Report received from AP DR.

  This function receives NEMA report from AP DR.
  The received NEMA report sends to DR Plugin.

  @param[in]  *input      Context Info Pointer.
  @param[in]  rx_srv_msg  Received Service Message.
  @return true on success; false on error.
*/
bool drClient_prcoss_nmeaRpt(void *input, dr_plugin_msg *rx_srv_msg);

/**
  @brief Process SV Report.

  This function receives SV report from AP DR .
  The received SV report sends to DR Plugin.

  @param[in]  *input      Context Info Pointer.
  @param[in]  rx_srv_msg  Recevided Service Message.
  @return true on success; false on error.
*/
bool drClient_prcoss_svRpt(void *input, dr_plugin_msg *rx_srv_msg);

/**
  @brief Process received message header.

  This functions decodes received message header
  and based on header request for read complete message

  @param[in]  *input      Context Info Pointer.
  @param[in]  rx_srv_msg  Received Service Message.
  @return true on success; false on error.
*/
bool drIpc_prcoss_rec_msg(void *input, dr_plugin_msg *rx_srv_msg);

/**
  @brief Open IPC with AP DR.

  This function opens client socket with AP DR.

  @param  void none.
  @return true on success; false on error.
*/
bool drIpc_open(void);

/**
  @brief Close IPC.

  This function closes client socket with AP DR.

  @param  void none.
  @return true on success; false on error.
*/
bool drIpc_close(void);

/**
  @brief Send Start Request.

  This function start session message to AP DR.

  @param  void none.
  @return true on success; false on error.
*/
bool drIpc_start(void);

/**
  @brief Send Stop Request.

  This function stop session message to AP DR.

  @param  void none.
  @return true on success; false on error.
*/
bool drIpc_stop(void);

/**
  @brief Send SV Measurement to AP DR.

  This function send SV Measurment Info.

  @param[in] svMeasurementSet SV Measurment Info.
  @return true on success; false on error.
*/
bool drIpc_injectSvMsr(GnssSvMeasurementSet &inSvMSR);

/**
  @brief send SV polynomial to AP DR.

  This function send SV polynomial Info.

  @param[in]  inSvPolyData SV Measurment Info.
  @return true on success; false on error.
*/
bool drIpc_injectPolynomial(GnssSvPolynomial &svPolynomial);

/**
  @brief Send Delete Aiding Data fo AP DR.

  This function send Delete Aiding message.

  @param[in]  inAidData Aiding Data.
  @return true on success; false on error.
*/
bool drIpc_deleteAidData(LocGpsAidingData inAidData);

/**
  @brief send Position Report to AP DR.

  This function send position reprot.

  @param[in]  location Location Report.
  @param[in]  locationExtended Extn Location Rpt.
  @return true on success; false on error.
*/
bool drIpc_injectPosRpt(LocGpsLocation &location,
                       GpsLocationExtended &locationExtended,
                       enum loc_sess_status status,
                       LocPosTechMask loc_technology_mask);

/**
  @brief send GNSS SV Status Report to AP DR.

  This function send GNSS Sv Report.

  @param[in] Gnss SV Report.
  @return true on success; false on error.
*/
bool drIpc_injectGnssSvRpt(const GnssSvNotification& svNotify);


/**
  @brief send Position Mode to AP DR.

  This function send position Mode.

  @param[in]  posMode operational mode for AP DR.
  @param[in]  min_interval represents the time between fixes in milliseconds.
  @param[in]  preferred_accuracy represents the requested fix accuracy in meters
  @param[in]  preferred_time requested time to first fix in milliseconds.
  @return true on success; false on error.
*/
bool drIpc_setPositionMode(LocGpsPositionMode  posMode,
                               uint32_t min_interval,
                               uint32_t preferred_accuracy,
                               uint32_t preferred_time);

/**
  @brief Send Shutdown Request.

  This function Shutdown message to AP DR.

  @param  void none.
  @return true on success; false on error.
*/
bool drIpc_shutdown(void);
/**
  @brief Read message header.

  This function reads all message header.

  @param[out]  *to_rx_msg  DR message header.
  @return true on success; false on error.
*/
bool drIpc_rec_msg(dr_plugin_msg *to_rx_msg);

/**
  @brief Receive complete message.

  This function reads compelete message from rx scoket.

  @param[in]  *out_msg    Input Pointer for fill received data.
  @param[in]  rx_msg_len  Max data tobe read from socket.
  @return true on success; false on error.
*/

bool drIpc_rec_cmplt_msg(unsigned char *out_msg, unsigned int rx_msg_len);

/**
  @brief Handle Socket Error.

  This function handles scoket error and try to reopen socket.

  @param[in]  void none.
  @return true on success; false on error.
*/
bool drIpc_socket_errorHandler(void);

/**
   @brief Store Set Pos Mode.

   This function store Pos Mode of Current Session.

   @param[in]  GnssSetPosModeReq Pos Mode Parameters.
   @return     none.
*/
void drClient_SetPosMode(GnssSetPosModeReq &setPosMode);

/**
  @brief Change Client as in Session.

  This function changes client as in Session.

  @param[in]  none.
  @return     none.
*/
void drClient_StartSessionState(void);

/**
  @brief Change Client as Session Stop.

  This function changes client state as ready.

  @param[in]  none.
  @return     none.
*/
void drClient_StopSessionState(void);

/**
   @brief Change Client State as Error.

   This function changes client state as Error.

   @param[in]  none.
   @return     none.
*/
void drClient_errorHandleState(void);

/**
  @brief Static Log for Measurement .

  This function provide static log for measurement time arrival.

  @param[in]  std_dev    Standard deviation of time arrival of measurement.
  @param[in]  mean     Mean time of arrival of measurement.
  @param[in]  max     Max time of arrival of measurement.
  @return true on success; false on error.
*/
bool drIpc_staticLog_msr(double std_dev,double mean,double max);

/**
  @brief Static Log for Position.

  This function provide time static of position arrival.

  @param[in]  std_dev    Standard deviation of time arrival of position.
  @param[in]  mean     Mean time of arrival of position.
  @param[in]  max     Max time of arrival of position.
  @return true on success; false on error.
*/
bool drIpc_staticLog_pos(double std_dev,double mean,double max);

#endif  //__DRPLUGIN_CLIENT_H
