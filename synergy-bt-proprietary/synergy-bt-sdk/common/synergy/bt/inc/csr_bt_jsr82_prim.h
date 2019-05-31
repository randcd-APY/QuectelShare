#ifndef CSR_BT_JSR82_PRIM_H__
#define CSR_BT_JSR82_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "bluetooth.h"
#include "rfcomm_prim.h"
#include "l2cap_prim.h"
#include "csr_bt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtJsr82Prim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */
typedef CsrPrim    CsrBtJsr82Prim;

/* ---------- Defines the JSR82 CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_JSR82_SUCCESS                    ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_JSR82_UNACCEPTABLE_PARAMETER     ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_JSR82_DEVICE_NOT_CONNECTED       ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_JSR82_CONNECT_FAILED             ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_JSR82_LINK_LOST                  ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_JSR82_UNSPECIFIED_ERROR          ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_JSR82_DATAWRITE_FAIL             ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_JSR82_COMMAND_DISALLOWED         ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_JSR82_SECURITY_FAIL              ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_JSR82_DEVICE_NOT_FOUND           ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_JSR82_SDC_DATABASE_STATE_CHANGED ((CsrBtResultCode) (0x000a))
#define CSR_BT_RESULT_CODE_JSR82_CSR_BT_SDC_EMPTY_RESPONSE  ((CsrBtResultCode) (0x000b))
#define CSR_BT_RESULT_CODE_JSR82_SDC_CONNECTION_RESPONSE_ERROR  ((CsrBtResultCode) (0x000c))
#define CSR_BT_RESULT_CODE_JSR82_MEMORY_CAPACITY_ERROR      ((CsrBtResultCode) (0x000d))



/* ---- Defines for CsrBtJsr82LdGetPropertyReq ---- */

#define CSR_BT_JSR82_ILLEGAL_PROPERTY_NAME                    ((CsrUint8)0x00)
#define CSR_BT_BLUETOOTH_API_VERSION                          ((CsrUint8)0x01)
#define CSR_BT_BLUETOOTH_MASTER_SWITCH                        ((CsrUint8)0x02)
#define CSR_BT_BLUETOOTH_SD_ATTR_RETRIEVABLE_MAX              ((CsrUint8)0x03)
#define CSR_BT_BLUETOOTH_CONNECTED_DEVICES_MAX                ((CsrUint8)0x04)
#define CSR_BT_BLUETOOTH_L2CAP_RECEIVEMTU_MAX                 ((CsrUint8)0x05)
#define CSR_BT_BLUETOOTH_SD_TRANS_MAX                         ((CsrUint8)0x06)
#define CSR_BT_BLUETOOTH_CONNECTED_INQUIRY_SCAN               ((CsrUint8)0x07)
#define CSR_BT_BLUETOOTH_CONNECTED_PAGE_SCAN                  ((CsrUint8)0x08)
#define CSR_BT_BLUETOOTH_CONNECTED_INQUIRY                    ((CsrUint8)0x09)
#define CSR_BT_BLUETOOTH_CONNECTED_PAGE                       ((CsrUint8)0x0A)
#define CSR_BT_JSR82_LD_NUMBER_OF_PROPERTIES                  ((CsrUint8)0x0B)

/* ---- Defines for CsrBtJsr82LdGetDiscoverableCfm ---- */

#define CSR_BT_DISCOVERY_AGENT_GIAC                            ((CsrUint24)(0x9e8b33))
#define CSR_BT_DISCOVERY_AGENT_LIAC                            ((CsrUint24)(0x9e8b00))
#define CSR_BT_DISCOVERY_AGENT_DISC_INTERVAL_START             (CSR_BT_DISCOVERY_AGENT_LIAC)
#define CSR_BT_DISCOVERY_AGENT_DISC_INTERVAL_END               ((CsrUint24)(0x9e8b3f))
#define CSR_BT_DISCOVERY_AGENT_NOT_DISCOVERABLE                ((CsrUint24)(0x000000))
#define CSR_BT_DISCOVERY_AGENT_ERROR_CODE                      ((CsrUint24)(0xffffff))

/*  ----- defines for Jsr82LdUpdateRecord ---- */

#define CSR_BT_JSR82_LD_UPDATE_RECORD_ERROR                    (0x01)

/* --- Defines for Discovery Agent --- */

#define CSR_BT_JSR82_INQUIRY_COMPLETED                  (0x00)
#define CSR_BT_JSR82_INQUIRY_TERMINATED                 (0x05)
#define CSR_BT_JSR82_INQUIRY_ERROR                      (0x07)

#define CSR_BT_JSR82_DISCOVERY_AGENT_CACHED             ((CsrUint8)0x00)
#define CSR_BT_JSR82_DISCOVERY_AGENT_PAIRED             ((CsrUint8)0x01)

#define CSR_BT_JSR82_SERVICE_SEARCH_COMPLETED                  (0x01)
#define CSR_BT_JSR82_SERVICE_SEARCH_DEVICE_NOT_REACHABLE       (0x06)
#define CSR_BT_JSR82_SERVICE_SEARCH_ERROR                      (0x03)
#define CSR_BT_JSR82_SERVICE_SEARCH_NO_RECORDS                 (0x04)
#define CSR_BT_JSR82_SERVICE_SEARCH_TERMINATED                 (0x02)

#define CSR_BT_JSR82_PROTOCOL_L2CAP                     (0x01)
#define CSR_BT_JSR82_PROTOCOL_RFCOMM                    (0x02)

/* --- Defines for security --- */
#define CSR_BT_JSR82_AUTHENTICATE                              (0x01)
#define CSR_BT_JSR82_ENCRYPT                                   (0x02)
#define CSR_BT_JSR82_AUTHORIZE                                 (0x04)
#define CSR_BT_JSR82_MASTER                                    (0x08)

/* --- Defines for Jsr82LdGetDeviceClass ---- */
#define CSR_BT_JSR82_COD_MAJOR                                 ((CsrUint8)0x01)
#define CSR_BT_JSR82_COD_MINOR                                 ((CsrUint8)0x02)
#define CSR_BT_JSR82_COD_SERVICES                              ((CsrUint8)0x04)
#define CSR_BT_JSR82_COD_FULL                                  ((CsrUint8)(CSR_BT_JSR82_COD_MAJOR | CSR_BT_JSR82_COD_MINOR | CSR_BT_JSR82_COD_SERVICES))


/* --- Defines  for Jsr82SetEventMask --- */
typedef CsrUint32              CsrBtJsr82EventMask;
/* Defines for event that the application can subscribe for */
#define CSR_BT_JSR82_EVENT_MASK_SUBSCRIBE_NONE                 ((CsrBtJsr82EventMask) 0x00000000)
#define CSR_BT_JSR82_EVENT_MASK_SUBSCRIBE_RFC_CLOSE_IND        ((CsrBtJsr82EventMask) 0x00000001)
#define CSR_BT_JSR82_EVENT_MASK_SUBSCRIBE_L2CA_CLOSE_IND       ((CsrBtJsr82EventMask) 0x00000002)

typedef CsrUint32              CsrBtJsr82EventMaskCond;
/* Condition for the eventmask */
#define CSR_BT_JSR82_EVENT_MASK_COND_SUCCESS                   ((CsrBtJsr82EventMaskCond) 0x00000000)
#define CSR_BT_JSR82_EVENT_MASK_COND_ALL                       ((CsrBtJsr82EventMaskCond) 0xFFFFFFFF)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST                                     (0x0000)

/* correspond to methods of the DiscoveryAgent class */
#define CSR_BT_JSR82_DA_START_INQUIRY_REQ                          ((CsrBtJsr82Prim) (0x0000 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_CANCEL_INQUIRY_REQ                         ((CsrBtJsr82Prim) (0x0001 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_SEARCH_SERVICES_REQ                        ((CsrBtJsr82Prim) (0x0002 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_CANCEL_SERVICE_SEARCH_REQ                  ((CsrBtJsr82Prim) (0x0003 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_SELECT_SERVICE_REQ                         ((CsrBtJsr82Prim) (0x0004 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_RETRIEVE_DEVICES_REQ                       ((CsrBtJsr82Prim) (0x0005 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* correspond to methods of the LocalDevice class */
#define CSR_BT_JSR82_LD_GET_BLUETOOTH_ADDRESS_REQ                  ((CsrBtJsr82Prim) (0x0006 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_UPDATE_RECORD_REQ                          ((CsrBtJsr82Prim) (0x0007 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_FRIENDLY_NAME_REQ                      ((CsrBtJsr82Prim) (0x0008 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_DISCOVERABLE_REQ                       ((CsrBtJsr82Prim) (0x0009 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_SET_DISCOVERABLE_REQ                       ((CsrBtJsr82Prim) (0x000a + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_PROPERTY_REQ                           ((CsrBtJsr82Prim) (0x000b + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_DEVICE_CLASS_REQ                       ((CsrBtJsr82Prim) (0x000c + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_SET_DEVICE_CLASS_REQ                       ((CsrBtJsr82Prim) (0x000d + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_SECURITY_LEVEL_REQ                     ((CsrBtJsr82Prim) (0x000e + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_IS_MASTER_REQ                              ((CsrBtJsr82Prim) (0x000f + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* ServiceRecord class */
#define CSR_BT_JSR82_SR_CREATE_RECORD_REQ                          ((CsrBtJsr82Prim) (0x0010 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_REGISTER_RECORD_REQ                        ((CsrBtJsr82Prim) (0x0011 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_REMOVE_RECORD_REQ                          ((CsrBtJsr82Prim) (0x0012 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_POPULATE_RECORD_REQ                        ((CsrBtJsr82Prim) (0x0013 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* RemoteDevice class */
#define CSR_BT_JSR82_RD_GET_FRIENDLY_NAME_REQ                      ((CsrBtJsr82Prim) (0x0014 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_AUTHENTICATE_REQ                           ((CsrBtJsr82Prim) (0x0015 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_AUTHENTICATED_REQ                       ((CsrBtJsr82Prim) (0x0016 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_ENCRYPT_REQ                                ((CsrBtJsr82Prim) (0x0017 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_ENCRYPTED_REQ                           ((CsrBtJsr82Prim) (0x0018 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_TRUSTED_REQ                             ((CsrBtJsr82Prim) (0x0019 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_CONNECTED_REQ                           ((CsrBtJsr82Prim) (0x001a + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* L2CAP */
#define CSR_BT_JSR82_L2CA_GET_PSM_REQ                              ((CsrBtJsr82Prim) (0x001b + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_ACCEPT_REQ                               ((CsrBtJsr82Prim) (0x001c + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_OPEN_REQ                                 ((CsrBtJsr82Prim) (0x001d + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_DISCONNECT_REQ                           ((CsrBtJsr82Prim) (0x001e + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_TX_DATA_REQ                              ((CsrBtJsr82Prim) (0x001f + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_RX_DATA_REQ                              ((CsrBtJsr82Prim) (0x0020 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_RX_READY_REQ                             ((CsrBtJsr82Prim) (0x0021 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_GET_CONFIG_REQ                           ((CsrBtJsr82Prim) (0x0022 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_CLOSE_REQ                                ((CsrBtJsr82Prim) (0x0023 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* RFCOMM */
#define CSR_BT_JSR82_RFC_GET_SERVER_CHANNEL_REQ                    ((CsrBtJsr82Prim) (0x0024 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_ACCEPT_AND_OPEN_REQ                       ((CsrBtJsr82Prim) (0x0025 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_CONNECT_REQ                               ((CsrBtJsr82Prim) (0x0026 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_DISCONNECT_REQ                            ((CsrBtJsr82Prim) (0x0027 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_CLOSE_REQ                                 ((CsrBtJsr82Prim) (0x0028 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_SEND_DATA_REQ                             ((CsrBtJsr82Prim) (0x0029 + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_RECEIVE_DATA_REQ                          ((CsrBtJsr82Prim) (0x002a + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_GET_AVAILABLE_REQ                         ((CsrBtJsr82Prim) (0x002b + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* cleanup signal */
#define CSR_BT_JSR82_CLEANUP_REQ                                   ((CsrBtJsr82Prim) (0x002c + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
/* subscribe signal ------- */
#define CSR_BT_JSR82_SET_EVENT_MASK_REQ                            ((CsrBtJsr82Prim) (0x002d + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_JSR82_PRIM_DOWNSTREAM_HIGHEST                                    (0x002d + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_JSR82_HOUSE_CLEANING                                ((CsrBtJsr82Prim) (0x002e + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_JSR82_CONTINUE_RFC_SEND_DATA                        ((CsrBtJsr82Prim) (0x002f + CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST                                       (0x0000 + CSR_PRIM_UPSTREAM)

/* Discovery agent class return values */
#define CSR_BT_JSR82_DA_SELECT_SERVICE_CFM                         ((CsrBtJsr82Prim) (0x0000 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_DA_RETRIEVE_DEVICES_CFM                       ((CsrBtJsr82Prim) (0x0001 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* correspond to methods of the DiscoveryListener class */
#define CSR_BT_JSR82_DL_INQUIRY_COMPLETED_IND                      ((CsrBtJsr82Prim) (0x0002 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_DL_DEVICE_DISCOVERED_IND                      ((CsrBtJsr82Prim) (0x0003 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_DL_SERVICES_DISCOVERED_IND                    ((CsrBtJsr82Prim) (0x0004 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_DL_SERVICE_SEARCH_COMPLETED_IND               ((CsrBtJsr82Prim) (0x0005 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* correspond to methods of the LocalDevice class */
#define CSR_BT_JSR82_LD_GET_BLUETOOTH_ADDRESS_CFM                  ((CsrBtJsr82Prim) (0x0006 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_UPDATE_RECORD_CFM                          ((CsrBtJsr82Prim) (0x0007 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_FRIENDLY_NAME_CFM                      ((CsrBtJsr82Prim) (0x0008 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_DISCOVERABLE_CFM                       ((CsrBtJsr82Prim) (0x0009 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_PROPERTY_CFM                           ((CsrBtJsr82Prim) (0x000A + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_SET_DISCOVERABLE_CFM                       ((CsrBtJsr82Prim) (0x000B + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_DEVICE_CLASS_CFM                       ((CsrBtJsr82Prim) (0x000C + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_SET_DEVICE_CLASS_CFM                       ((CsrBtJsr82Prim) (0x000D + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_GET_SECURITY_LEVEL_CFM                     ((CsrBtJsr82Prim) (0x000E + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_LD_IS_MASTER_CFM                              ((CsrBtJsr82Prim) (0x000F + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* ServiceRecord class */
#define CSR_BT_JSR82_SR_CREATE_RECORD_CFM                          ((CsrBtJsr82Prim) (0x0010 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_REGISTER_RECORD_CFM                        ((CsrBtJsr82Prim) (0x0011 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_REMOVE_RECORD_CFM                          ((CsrBtJsr82Prim) (0x0012 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_SR_POPULATE_RECORD_CFM                        ((CsrBtJsr82Prim) (0x0013 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* RemoteDevice class */
#define CSR_BT_JSR82_RD_GET_FRIENDLY_NAME_CFM                      ((CsrBtJsr82Prim) (0x0014 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_AUTHENTICATE_CFM                           ((CsrBtJsr82Prim) (0x0015 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_AUTHENTICATED_CFM                       ((CsrBtJsr82Prim) (0x0016 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_ENCRYPT_CFM                                ((CsrBtJsr82Prim) (0x0017 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_ENCRYPTED_CFM                           ((CsrBtJsr82Prim) (0x0018 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_TRUSTED_CFM                             ((CsrBtJsr82Prim) (0x0019 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RD_IS_CONNECTED_CFM                           ((CsrBtJsr82Prim) (0x001A + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* L2CAP */
#define CSR_BT_JSR82_L2CA_GET_PSM_CFM                              ((CsrBtJsr82Prim) (0x001B + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_ACCEPT_CFM                               ((CsrBtJsr82Prim) (0x001C + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_OPEN_CFM                                 ((CsrBtJsr82Prim) (0x001D + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_DISCONNECT_CFM                           ((CsrBtJsr82Prim) (0x001E + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_TX_DATA_CFM                              ((CsrBtJsr82Prim) (0x001F + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_RX_DATA_CFM                              ((CsrBtJsr82Prim) (0x0020 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_RX_READY_CFM                             ((CsrBtJsr82Prim) (0x0021 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_GET_CONFIG_CFM                           ((CsrBtJsr82Prim) (0x0022 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_CLOSE_CFM                                ((CsrBtJsr82Prim) (0x0023 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* RFCOMM */
#define CSR_BT_JSR82_RFC_GET_SERVER_CHANNEL_CFM                    ((CsrBtJsr82Prim) (0x0024 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_ACCEPT_AND_OPEN_CFM                       ((CsrBtJsr82Prim) (0x0025 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_CONNECT_CFM                               ((CsrBtJsr82Prim) (0x0026 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_DISCONNECT_CFM                            ((CsrBtJsr82Prim) (0x0027 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_CLOSE_CFM                                 ((CsrBtJsr82Prim) (0x0028 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_SEND_DATA_CFM                             ((CsrBtJsr82Prim) (0x0029 + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_RECEIVE_DATA_CFM                          ((CsrBtJsr82Prim) (0x002A + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_GET_AVAILABLE_CFM                         ((CsrBtJsr82Prim) (0x002B + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
/* subscribe signal ------- */
#define CSR_BT_JSR82_SET_EVENT_MASK_CFM                            ((CsrBtJsr82Prim) (0x002C + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_RFC_CLOSE_IND                                 ((CsrBtJsr82Prim) (0x002D + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_JSR82_L2CA_CLOSE_IND                                ((CsrBtJsr82Prim) (0x002E + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_JSR82_PRIM_UPSTREAM_HIGHEST                                      (0x002E + CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_JSR82_PRIM_DOWNSTREAM_COUNT                         (CSR_BT_JSR82_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_JSR82_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_JSR82_PRIM_UPSTREAM_COUNT                           (CSR_BT_JSR82_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_JSR82_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* ---- DiscoveryAgent ----------------------------------------- */
typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint24                iac;
} CsrBtJsr82DaStartInquiryReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82DaCancelInquiryReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint16                uuidSetLength;
    CsrUint8                 *uuidSet;
    CsrUint16                attrSetLength; /* Number of _items_ in attrSet, _not_ byte size */
    CsrUint16                *attrSet;
} CsrBtJsr82DaSearchServicesReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82DaCancelServiceSearchReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint16                requestedUuidLength;
    CsrUint8                 *requestedUuid;
} CsrBtJsr82DaSelectServiceReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8                 protocol;
    CsrUint16                chanPsm;
    CsrBtResultCode          resultCode;
} CsrBtJsr82DaSelectServiceCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8                 option;
} CsrBtJsr82DaRetrieveDevicesReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint16                devicesCount;
    CsrBtDeviceAddr            *devices;
} CsrBtJsr82DaRetrieveDevicesCfm;

/* ---- DiscoveryListener ---------------------------------------- */
typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8                 respCode; /* Java specific result code */
} CsrBtJsr82DlInquiryCompletedInd;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtClassOfDevice         classOfDevice;
    CsrInt8                  rssi;
} CsrBtJsr82DlDeviceDiscoveredInd;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint16                attributesLength;
    CsrUint8                 *attributes;
} CsrBtJsr82DlServicesDiscoveredInd;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint32                serviceDataBaseState;
    CsrBool                  serviceDBStateValid;
    CsrUint8                 respCode; /* Java specific result code */
} CsrBtJsr82DlServiceSearchCompletedInd;

/* ---- LocalDevice ----------------------------------------------- */
typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82LdGetBluetoothAddressReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtJsr82LdGetBluetoothAddressCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
    CsrUint8                 *serviceRecord;
    CsrUint16                serviceRecordSize;
} CsrBtJsr82LdUpdateRecordReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
    CsrBtResultCode          resultCode;
} CsrBtJsr82LdUpdateRecordCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82LdGetFriendlyNameReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUtf8String                *localName;
} CsrBtJsr82LdGetFriendlyNameCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82LdGetDiscoverableReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint24                mode;
} CsrBtJsr82LdGetDiscoverableCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint24                mode;
} CsrBtJsr82LdSetDiscoverableReq;

typedef struct
{
    CsrBtJsr82Prim                type;
    CsrUint32                 reqID;
    CsrBtResultCode          resultCode;
} CsrBtJsr82LdSetDiscoverableCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8                 propertyName; /* See defines above */
} CsrBtJsr82LdGetPropertyReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8                 propertyName; /* see defines above */
    CsrUint16                propertyValue;
} CsrBtJsr82LdGetPropertyCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8                 fieldsMask;
} CsrBtJsr82LdGetDeviceClassReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    device_class_t          deviceClass;
    CsrBtResultCode         resultCode;
} CsrBtJsr82LdGetDeviceClassCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    device_class_t          deviceClass;
} CsrBtJsr82LdSetDeviceClassReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtResultCode         resultCode;
} CsrBtJsr82LdSetDeviceClassCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82LdGetSecurityLevelReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint16                secLevel;
} CsrBtJsr82LdGetSecurityLevelCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtJsr82LdIsMasterReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8                 role;
} CsrBtJsr82LdIsMasterCfm;

/* ---- ServiceRecord ---------------------------------------------- */
typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82SrCreateRecordReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
} CsrBtJsr82SrCreateRecordCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint16                serviceRecordLength;
    CsrUint8                 *serviceRecord;
} CsrBtJsr82SrRegisterRecordReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
} CsrBtJsr82SrRegisterRecordCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
} CsrBtJsr82SrRemoveRecordReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint32                serviceRecHandle;
} CsrBtJsr82SrRemoveRecordCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint32                serviceDataBaseState;
    CsrUint32                serviceRecHandle;
    CsrUint16                attrSetLength; /* Number of _items_ in attrSet, _not_ byte size */
    CsrUint16                *attrSet;
} CsrBtJsr82SrPopulateRecordReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint16                attributesLength;
    CsrUint8                 *attributes;
    CsrBtResultCode          resultCode;
} CsrBtJsr82SrPopulateRecordCfm;

/* ------------ RemoteDevice class primitives ----------------- */

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            address;
    CsrBool                  alwaysAsk;
} CsrBtJsr82RdGetFriendlyNameReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUtf8String                *remoteName;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RdGetFriendlyNameCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            address;
} CsrBtJsr82RdAuthenticateReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RdAuthenticateCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtJsr82RdIsAuthenticatedReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBool                  authenticated;
} CsrBtJsr82RdIsAuthenticatedCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBool                  encrypt;
    CsrBtDeviceAddr            address;
} CsrBtJsr82RdEncryptReq;


typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RdEncryptCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtJsr82RdIsEncryptedReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint16                  encrypted;
} CsrBtJsr82RdIsEncryptedCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
} CsrBtJsr82RdIsTrustedReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBool                  trusted;
} CsrBtJsr82RdIsTrustedCfm;


typedef struct
{
    CsrBtJsr82Prim       type;
    CsrUint32        reqID;
    CsrSchedQid       appHandle;
    CsrBtDeviceAddr    deviceAddr;
}CsrBtJsr82RdIsConnectedReq;

typedef struct
{
    CsrBtJsr82Prim       type;
    CsrUint32        reqID;
    CsrUint8         handle;
}CsrBtJsr82RdIsConnectedCfm;

/* ---- L2CAP ------------------------------------------------------ */

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82L2caGetPsmReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    psm_t                   localPsm;
} CsrBtJsr82L2caGetPsmCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    psm_t                   localPsm;
    l2ca_mtu_t                   receiveMtu;
    l2ca_mtu_t                   transmitMtu;
    CsrUint32                attrs;
} CsrBtJsr82L2caAcceptReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    psm_t                   localPsm;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    l2ca_mtu_t                   transmitMtu;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtResultCode         resultCode;
} CsrBtJsr82L2caAcceptCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    psm_t                   remotePsm;
    l2ca_mtu_t                   receiveMtu;
    l2ca_mtu_t                   transmitMtu;
    CsrUint32                attrs;
} CsrBtJsr82L2caOpenReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    l2ca_mtu_t                   transmitMtu;
    CsrBtResultCode            resultCode;
} CsrBtJsr82L2caOpenCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    psm_t                   localPsm;
} CsrBtJsr82L2caDisconnectReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    psm_t                   localPsm;
    CsrBool                  localTerminated;
    CsrBtResultCode         resultCode;
} CsrBtJsr82L2caDisconnectCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16                payloadLength;
    CsrUint8                 *payload;
} CsrBtJsr82L2caTxDataReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrBtResultCode         resultCode;
} CsrBtJsr82L2caTxDataCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrUint16                dataLength;
} CsrBtJsr82L2caRxDataReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrBtResultCode         resultCode;
    CsrUint16                payloadLength;
    CsrUint8                 *payload;
} CsrBtJsr82L2caRxDataCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
} CsrBtJsr82L2caRxReadyReq;

typedef struct
{
    CsrBtJsr82Prim          type;
    CsrUint32               reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrBool                 dataReady;
    CsrBtResultCode         resultCode;
    CsrUint16               dataLength;
} CsrBtJsr82L2caRxReadyCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
} CsrBtJsr82L2caGetConfigReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    l2ca_mtu_t                   receiveMTU;
    l2ca_mtu_t                   transmitMTU;
    CsrBtResultCode         resultCode;
} CsrBtJsr82L2caGetConfigCfm;


typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
} CsrBtJsr82L2caCloseReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrBtResultCode         resultCode;
    CsrBool                  localTerminated;
} CsrBtJsr82L2caCloseCfm;


/* ---- RFCOMM ----------------------------------------------------- */

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
} CsrBtJsr82RfcGetServerChannelReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
} CsrBtJsr82RfcGetServerChannelCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                attrs;
} CsrBtJsr82RfcAcceptAndOpenReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint8           localServerChannel;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RfcAcceptAndOpenCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           remoteServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrUint32                attrs;
} CsrBtJsr82RfcConnectReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrBtDeviceAddr            deviceAddr;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RfcConnectCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
} CsrBtJsr82RfcDisconnectReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
} CsrBtJsr82RfcCloseReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrBtResultCode         resultCode;
    CsrBool                  localTerminated;
} CsrBtJsr82RfcDisconnectCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrBtResultCode         resultCode;
    CsrBool                  localTerminated;
} CsrBtJsr82RfcCloseCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                payloadLength;
    CsrUint8                 *payload;
} CsrBtJsr82RfcSendDataReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                bytesWritten;
} CsrBtJsr82RfcSendDataCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                bytesToRead;
} CsrBtJsr82RfcReceiveDataReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                payloadLength;
    CsrUint8                 *payload;
} CsrBtJsr82RfcReceiveDataCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               appHandle;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
} CsrBtJsr82RfcGetAvailableReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint32                reqID;
    CsrUint8           localServerChannel;
    CsrUint32                bytesAvailable;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RfcGetAvailableCfm;

/* -------- subscribe primitive ------- */

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrSchedQid               phandle;
    CsrBtJsr82EventMask       eventMask;
    CsrBtJsr82EventMaskCond  conditionMask;
} CsrBtJsr82SetEventMaskReq;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrBtJsr82EventMask       eventMask;
} CsrBtJsr82SetEventMaskCfm;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint8           localServerChannel;
    CsrBool                  localTerminated;
    CsrBtResultCode         resultCode;
} CsrBtJsr82RfcCloseInd;

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrBtConnId            btConnId;                      /* Global Bluetooth connection ID */
    CsrBool                  localTerminated;
    CsrBtResultCode         resultCode;
} CsrBtJsr82L2caCloseInd;

/* -------- cleanup primitive ------- */

typedef struct
{
    CsrBtJsr82Prim   type;
} CsrBtJsr82CleanupReq;


/* ---- Internal primitives ---------------------------------------- */

typedef struct
{
    CsrBtJsr82Prim               type;
    CsrUint8           localServerChannel;
}CsrBtJsr82ContinueRfcSendData;


typedef struct
{
    CsrBtJsr82Prim               type;
} CsrBtJsr82HouseCleaning;

#ifdef __cplusplus
}
#endif

#endif

