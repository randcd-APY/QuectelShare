/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_wlan_p2p.h"

qapi_Status_t _qapi_WLAN_P2P_Enable(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_Enable_e enable);

qapi_Status_t _qapi_WLAN_P2P_Cancel(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_P2P_Find(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_P2P_Disc_Type_e disc_Type, uint32_t timeout_In_Secs);

qapi_Status_t _qapi_WLAN_P2P_Stop_Find(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_P2P_Prov(uint8_t TargetID, uint8_t device_ID, uint16_t wps_Method, const uint8_t *mac);

qapi_Status_t _qapi_WLAN_P2P_Auth(uint8_t TargetID, uint8_t device_ID, int32_t dev_Auth, qapi_WLAN_P2P_WPS_Method_e wps_Method, const uint8_t *peer_Mac, qapi_WLAN_P2P_Persistent_e persistent);

qapi_Status_t _qapi_WLAN_P2P_Connect(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_P2P_WPS_Method_e wps_Method, const uint8_t *peer_Mac, qapi_WLAN_P2P_Persistent_e persistent);

qapi_Status_t _qapi_WLAN_P2P_Start_Go(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_P2P_Go_Params_t *params, int32_t channel, qapi_WLAN_P2P_Persistent_e persistent);

qapi_Status_t _qapi_WLAN_P2P_Invite(uint8_t TargetID, uint8_t device_ID, const char *ssid, qapi_WLAN_P2P_WPS_Method_e wps_Method, const uint8_t *mac, qapi_WLAN_P2P_Persistent_e persistent, qapi_WLAN_P2P_Inv_Role_e role);

qapi_Status_t _qapi_WLAN_P2P_Join(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_P2P_WPS_Method_e wps_Method, const uint8_t *mac, const char *pin, uint16_t channel);

qapi_Status_t _qapi_WLAN_P2P_Listen(uint8_t TargetID, uint8_t device_ID, uint32_t timeout_In_Secs);

qapi_Status_t _qapi_WLAN_P2P_Invite_Auth(uint8_t TargetID, uint8_t device_ID, const qapi_WLAN_P2P_Invite_Info_t *invite_Info);
