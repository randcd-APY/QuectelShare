/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef LOC_SRV_PROC_REQ_HDLRS_H
#define LOC_SRV_PROC_REQ_HDLRS_H

//=============================================================================
// FUNCTIONS
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// FUNCTION: loc_set_ind_proc_req_hdlr
//
// DESCRIPTION:
// Register for indications
//
// @param[in] loc_ind                      register for Location indcation
// @param[in] status_ind                   register for status indcation
// @param[in] sv_ind                       register for sv indication
// @param[in] nmea_ind                     register for nmea indication
// @param[in] cap_ind                      register for capabilities indication
// @param[in] utc_req_ind                  register for UTC time request indication
// @param[in] xtra_data_req_ind            register for XTRA data request indication
// @param[in] agps_status_ind              register for AGPS status indication
// @param[in] ni_ind                       register for NI Indication
// @param[in] xtra_report_server_ind       register for XTRA report XTRA server URLs indication
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_set_ind_proc_req_hdlr (
    int loc_ind,
    int status_ind,
    int sv_ind,
    int nmea_ind,
    int cap_ind,
    int utc_req_ind,
    int xtra_data_req_ind,
    int agps_status_ind,
    int ni_ind,
    int xtra_report_server_ind);



//=============================================================================
// FUNCTION: loc_set_position_mode_proc_req_hdlr  <mode> <recurrence>
//           <min_interval> <preferred_accuracy> <preferred_time>
//
// DESCRIPTION:
// Initialize the MCM LOC interface for location services
// This function should be called at startup after init method and before any
// other methods are called.
//
// @param[in] mode GPS Position Mode @ref gps_mode
// @param[in] recurrence GPS Position Recurrence @ref gps_recurrence
// @param[in] min_interval Represents the time between fixes in milliseconds
// @param[in] preferred_accuracy Represents requested fix accuracy in meters
// @param[in] preferred_time Represents the requested time to first fix in
//                          milliseconds.
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_set_position_mode_proc_req_hdlr (mcm_gps_position_mode_t_v01 mode,
                                   mcm_gps_position_recurrence_t_v01 recurrence,
                                   uint32_t min_interval,
                                   uint32_t preferred_accuracy,
                                   uint32_t preferred_time);

//=============================================================================
// FUNCTION: loc_start_nav_proc_req_hdlr
//
// DESCRIPTION:
// Starts Navigation
//
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_start_nav_proc_req_hdlr ();

//=============================================================================
// FUNCTION: loc_stop_nav_proc_req_hdlr
//
// DESCRIPTION:
// Stops Navigation
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_stop_nav_proc_req_hdlr ();

//=============================================================================
// FUNCTION: loc_close_proc_req_hdlr
//
// DESCRIPTION:
// Closes the location interface
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_close_proc_req_hdlr ();

//=============================================================================
// FUNCTION: loc_delete_aiding_data_proc_req_hdlr  <flags>
//
// DESCRIPTION:
// Specifies that the next call to start will not use the information
// defined in the flags. LOC_GPS_DELETE_ALL is passed for a cold start.
//
// @param[in] flags Aiding data flags @ref gps_aiding_data
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_delete_aiding_data_proc_req_hdlr (mcm_gps_aiding_data_t_v01 flags);

//=============================================================================
// FUNCTION: loc_inject_time_proc_req_hdlr <time> <time_reference> <uncertainty>
//
//
// DESCRIPTION:
// Injects the current time
//
// @param[in] time UTC time
// @param[in] time_reference Time Reference
// @param[in] uncertainty Uncertainty
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_inject_time_proc_req_hdlr (int64_t time,
                                 int64_t time_reference,
                                 int32_t uncertainty);

//=============================================================================
// FUNCTION: loc_inject_location_proc_req_hdlr <latitude> <longitude> <accuracy>
//
// DESCRIPTION:
// Injects current location from another location provider
//  (typically cell ID).
//
// @param[in] latitude latitude measured in degrees
// @param[in] longitude longitude measured in degrees
// @param[in] accuracy Expected accuracy measured in meters.
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_inject_location_proc_req_hdlr (double latitude,
                                     double longitude,
                                     float accuracy);


//=============================================================================
// FUNCTION: loc_xtra_inject_data_proc_req_hdlr  <data> <length>
//
// DESCRIPTION:
// Injects XTRA data into the GPS
//
// @param[in] data XTRA data
// @param[in] length Length of XTRA data in bytes
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_xtra_inject_data_proc_req_hdlr (char * data,
                                      int32_t length);


//=============================================================================
// FUNCTION: loc_agps_data_conn_open_proc_req_hdlr <agps_type> <apn>
// <brearer_type>
//
// DESCRIPTION:
// Notifies that a data connection is available and sets the name of the APN
// to be used for SUPL
//
// @param[in] agps_type type @ref agps_t
// @param[in] apn Access point name
// @param[in] bearer_type Bearer type @ref agps_bearer
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_agps_data_conn_open_proc_req_hdlr (mcm_agps_t_v01 agps_type,
                                         const char * apn,
                                         mcm_agps_bearer_t_v01 bearer_type);

//=============================================================================
// FUNCTION: loc_agps_data_conn_closed_proc_req_hdlr  <agps_type>
//
// DESCRIPTION:
// Notifies that the data connection has been closed
//
// @param[in] agps_type type @ref agps_t.
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_agps_data_conn_closed_proc_req_hdlr (mcm_agps_t_v01 agps_type);

//=============================================================================
// FUNCTION: loc_agps_data_conn_failed_proc_req_hdlr  <agps_type>
//
// DESCRIPTION:
// Notifies that the data connection is not available for AGPS.
//
// @param[in] agps_type Agps type @ref agps_t.
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_agps_data_conn_failed_proc_req_hdlr (mcm_agps_t_v01 agps_type);

//=============================================================================
// FUNCTION: loc_agps_set_server_proc_req_hdlr  <agps_type> <host_name> <port>
//
// DESCRIPTION:
// Sets the hostname and port for the AGPS server.
//
// @param[in] agps_type type @ref agps_t.
// @param[in] host_name Host name
// @param[in] port Port
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_agps_set_server_proc_req_hdlr (mcm_agps_t_v01 agps_type,
                                     const char * host_name,
                                     uint32_t port);

//=============================================================================
// FUNCTION: loc_ni_respond_proc_req_hdlr  <notif_id> <user_response>
//
// DESCRIPTION:
// Sends a Response to GPS HAL
//
// @param[in] notif_id Notification ID
// @param[in] user_response User Response @ref gps_user_response
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_ni_respond_proc_req_hdlr (int32_t notif_id,
                                mcm_gps_user_response_t_v01 user_response);


//=============================================================================
// FUNCTION: loc_agps_ril_update_network_availability_proc_req_hdlr <available>
//                                                                  <apn>
// DESCRIPTION:
// Notify GPS of network availability.
//
// @param[in] available available
// @param[in] apn Access Point name
//
// @return
//    0 -- Success
//    -1 -- Failure
//=============================================================================
int loc_agps_ril_update_network_availability_proc_req_hdlr (int32_t available,
                                                          const char* apn);
#ifdef __cplusplus
}
#endif

#endif //LOC_SRV_PROC_REQ_HDLRS_H
