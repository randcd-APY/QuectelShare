#ifndef CSR_BT_USR_CONFIG_H
#define CSR_BT_USR_CONFIG_H
/******************************************************************************
 Copyright (c) 2001-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_types.h"
#include "bluetooth.h"
#include "hci_prim.h"
#include "dm_prim.h"
#include "rfcomm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bluetooth versions as defined by the specification (DO NOT MODIFY) */
#ifndef CSR_BT_BLUETOOTH_VERSION_1P0
#define CSR_BT_BLUETOOTH_VERSION_1P0                       (0x00)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_1P1
#define CSR_BT_BLUETOOTH_VERSION_1P1                       (0x01)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_1P2
#define CSR_BT_BLUETOOTH_VERSION_1P2                       (0x02)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_2P0
#define CSR_BT_BLUETOOTH_VERSION_2P0                       (0x03)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_2P1
#define CSR_BT_BLUETOOTH_VERSION_2P1                       (0x04)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_3P0
#define CSR_BT_BLUETOOTH_VERSION_3P0                       (0x05)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_4P0
#define CSR_BT_BLUETOOTH_VERSION_4P0                       (0x06)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_4P1
#define CSR_BT_BLUETOOTH_VERSION_4P1                       (0x07)
#endif
#ifndef CSR_BT_BLUETOOTH_VERSION_4P2
#define CSR_BT_BLUETOOTH_VERSION_4P2                       (0x08)
#endif

#ifndef CSR_BT_BLUETOOTH_VERSION_5P0
#define CSR_BT_BLUETOOTH_VERSION_5P0                       (0x09)
#endif

/*--------------------------------------------------------------------------
 * HCI Build MTU sizes
 *--------------------------------------------------------------------------*/

/* This limit sets the maximum overall MTU/packet size that Synergy
 * may negotiate. This limit should be based on the memory managment
 * capabilities of the particular platform Synegy runs on */
#ifndef CSR_BT_MAXIMUM_AUTOMATIC_MTU
#define CSR_BT_MAXIMUM_AUTOMATIC_MTU                       (0x8000)
#endif

/* This is the maximum packet size any AMP radio may utilise. Note
 * that the *actual* maximum packet size is adjusted automatically
 * without any sort of user- or protocol intervention.
 * If UWB is present, 4192 is good.
 * If only 802.11 is present, 1500 is good */
#ifndef CSR_BT_MAXIMUM_AMP_PACKET_SIZE
#define CSR_BT_MAXIMUM_AMP_PACKET_SIZE                     (1500)
#endif

/*----------------------------------------------------------------------------*
 *
 *   Default Low Power Settings controls which lowpower settings are supported 
 *   - i.e. if sniff are supported or not
 *   Where the following bits can be set:
 *   DISABLE_ALL_LM_MODES.   Disable local support of sniff and park mode.
 *   ENABLE_SNIFF.           Enable local support of sniff mode.
  *   Note: Multiple bits can be set, and if sniff mode is disabled sniff
 *   subrating is also disabled.
 *---------------------------------------------------------------------------*/
#ifndef CSR_BT_DEFAULT_LOW_POWER_MODES
#define CSR_BT_DEFAULT_LOW_POWER_MODES ((link_policy_settings_t)ENABLE_SNIFF)
#endif

/*--------------------------------------------------------------------------
 *
 *   Defines which packet types can be used for a connection that is 
 *   currently established. The Host should enable as many packet types 
 *   as possible for the Link Manager to perform efficiently. However, 
 *   the Host must not enable packet types that the local device does not 
 *   support
 *---------------------------------------------------------------------------*/   
#ifdef CSR_BT_INSTALL_CHANGE_ACL_PACKET_TYPE
#define CSR_BT_ACL_PACKET_TYPE ((hci_pkt_type_t)(HCI_PKT_DM1|HCI_PKT_DH1|HCI_PKT_DM3|HCI_PKT_DH3|HCI_PKT_DM5|HCI_PKT_DH5))
#endif

/*--------------------------------------------------------------------------
 * Default advertising/scan/connection parameters for Low Energy
 *--------------------------------------------------------------------------*/
#define CSR_BT_LE_DEFAULT_ADV_INTERVAL_MIN             (0x00A0) /* 100msec - Default minimum advertising interval.
                                                                    Range: 0x0020 (20ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */
#define CSR_BT_LE_DEFAULT_ADV_INTERVAL_MAX             (0x0800) /* 1.28sec - Default maximum advertising interval.
                                                                    Range: 0x0020 (20ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */
#define CSR_BT_LE_DEFAULT_SCAN_INTERVAL                (0x0060) /* 60 msec - Default scan interval.
                                                                    Range: 0x0004 (2.5ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */
#define CSR_BT_LE_DEFAULT_SCAN_WINDOW                  (0x0030) /* 30 msec - Default scan window.
                                                                    Range: 0x0004 (2.5ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */
#define CSR_BT_LE_DEFAULT_CONN_INTERVAL_MIN            (0x0018) /* 30msec - Default minimum connection interval.
                                                                    Range: 0x0006 (7.5ms) to 0x0C80 (4 s)
                                                                    Time = N * 1.25msec */
#define CSR_BT_LE_DEFAULT_CONN_INTERVAL_MAX            (0x0028) /* 50msec - Default maximum connection interval.
                                                                    Range: 0x0006 (7.5ms) to 0x0C80 (4 s)
                                                                    Time = N * 1.25msec */
#define CSR_BT_LE_DEFAULT_CONN_LATENCY                 (0x0006) /* Default slave latency.
                                                                    General purpose value needs to be smaller than
                                                                    ((supervisionTimeout * 4) / connIntervalMax) - 1
                                                                    Range: 0x0000 to 0x01F3 */
#define CSR_BT_LE_DEFAULT_CONN_LATENCY_MAX             (0x01F3) /* Maximum connection latency defined by spec */
#define CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT     (0x0800) /* 20.48 sec - Default supervision timeout.
                                                                    Range: 0x000A (100ms) to 0x0C80 (32s)
                                                                    Time = N * 10 msec */
#define CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT_MIN (0x000a) /* 100ms - Minimum supervision timeout */
#define CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT_MAX (0x0c80) /* 32s - Maximum supervision timeout */
#define CSR_BT_LE_DEFAULT_CONN_ATTEMPT_TIMEOUT         (0x0040) /* 6.4s - Default connection attempt timeout
                                                                    Time = connAttemptTimeout * 0.1 sec */

#define CSR_BT_LE_DEFAULT_SLOW_SCAN_INTERVAL           (0x0060) /* 60 msec - Default slow scan interval.
                                                                    Range: 0x0004 (2.5ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */
#define CSR_BT_LE_DEFAULT_SLOW_SCAN_WINDOW             (0x0010) /* 10 msec - Default slow scan window.
                                                                    Range: 0x0004 (2.5ms) to 0x4000 (10.24s)
                                                                    Time = N * 0.625msec */

/* Aggressive connection parameters to be used during service discovery and configuration */
#define CSR_BT_LE_SCAN_INTERVAL_FAST                    ((CsrUint16) (22 / 0.625))
#define CSR_BT_LE_SCAN_WINDOW_FAST                      ((CsrUint16) (11.25 / 0.625))
#define CSR_BT_LE_CONNECTION_INTERVAL_MIN_FAST          ((CsrUint16) (7.5 / 1.25))
#define CSR_BT_LE_CONNECTION_INTERVAL_MAX_FAST          ((CsrUint16) (50 / 1.25))
#define CSR_BT_LE_CONNECTION_LATENCY_FAST               ((CsrUint16) 0)
#define CSR_BT_LE_SUPERVISION_TIMEOUT_FAST              CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT
#define CSR_BT_LE_CONNECTION_ATTEMPT_TIMEOUT_FAST       (2 * 10) /* 2 seconds */

/*--------------------------------------------------------------------------
 * Define for the Appearance Characteristic Value for Low Energy.
 * The values are composed of a category (10-bits) and sub-categories (6-bits). 
 *  and can be found in assigned numbers or csr_bt_profiles.h
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_LE_APPEARANCE_VALUE
#define CSR_BT_LE_APPEARANCE_VALUE               CSR_BT_APPEARANCE_GENERIC_DISPLAY
#endif /* CSR_BT_LE_APPEARANCE_VALUE */


/*--------------------------------------------------------------------------
 * Define the Permission for the Client Characteristic Configuration descriptor
 * which is part of the Service Changed Characteristic. The Service Changed 
 * characteristic is a control-point attribute own by GATT that can be used 
 * to indicate to connected devices that the GATT database have changed. 
 * This Characteristic Value may be configured by a remote client. This value 
 * defines permission for this write operation
 *
 * Valid values are:
 *      CSR_BT_GATT_ATTR_FLAGS_NONE
 *      CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
 *      CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_LE_PERMISSION_SERVICE_CHANGED
#define CSR_BT_LE_PERMISSION_SERVICE_CHANGED    CSR_BT_GATT_ATTR_FLAGS_NONE
#endif /* CSR_BT_LE_PERMISSION_SERVICE_CHANGED */

/*--------------------------------------------------------------------------
 * Define how long time in seconds GATT shall try to Connect as 
 * Central/Initiator if the application(s) want to scan and connect
 * simultaneous and the Controller does not support the  
 * Passive/Active Scanning State and Initiating State combination
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_LE_INITIATOR_TIMEOUT
#define CSR_BT_LE_INITIATOR_TIMEOUT             (10) 
#endif /* CSR_BT_LE_INITIATOR_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for private address timeout (in secs): By default it is configured to
 * 15 mins.
 * Valid range: 1 sec - approximately 11.5 hrs.
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_LE_DEFAULT_PVT_ADDR_TIMEOUT
#define CSR_BT_LE_DEFAULT_PVT_ADDR_TIMEOUT           ((CsrUint16) (15 * 60))
#endif /* CSR_BT_LE_DEFAULT_PVT_ADDR_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines static address to be used for local device.
 * A static address is a 48-bit randomly generated address and shall meet the
 * following requirements:
 * The two most significant bits of the address shall be equal to 1
 * At least one bit of the random part of the address shall be 0
 * At least one bit of the random part of the address shall be 1
 * The format of static address must be followed as shown below. 
 * LSB                                                         MSB
 * |-------------------------------------------------------------|
 * |           Random part of static address         |  1  |  1  |
 * |-------------------------------------------------------------|
 * <-----------------static address (48 bits)-------------------->
 * If all fields of static address are set to zeros or invalid static address is
 * configured, Synergy ignores the configured static address and generates its
 * own. Static address to be configured in below convention.
 * {Lower Address Part:00-23, upper Address Part:24-31, Non-significant:32-47}.
 * Note:- This is applicable only when CSR_BT_LE_RANDOM_ADDRESS_TYPE flag is set
 * with "STATIC".
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_LE_DEFAULT_STATIC_ADDRESS
#define CSR_BT_LE_DEFAULT_STATIC_ADDRESS            {0x000000, 0x00, 0x0000};
#endif /* CSR_BT_LE_DEFAULT_STATIC_ADDRESS */

/*---------------------------------------------------------------------------
 * defines used to prioritize the EIR data types in LE advertisement
 * and ScanResponse data.
 * Each number can only be used once and lowest value is highest
 * priority (starting from 1)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_GATT_ADV_DATA_UUID16
#define CSR_BT_GATT_ADV_DATA_UUID16                      1
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SERVICE_DATA
#define CSR_BT_GATT_ADV_DATA_SERVICE_DATA                2
#endif
#ifndef CSR_BT_GATT_ADV_DATA_MANUFACTURER_DATA
#define CSR_BT_GATT_ADV_DATA_MANUFACTURER_DATA           3
#endif
#ifndef CSR_BT_GATT_ADV_DATA_LOCALNAME
#define CSR_BT_GATT_ADV_DATA_LOCALNAME                   4
#endif
#ifndef CSR_BT_GATT_ADV_DATA_TX_POWER
#define CSR_BT_GATT_ADV_DATA_TX_POWER                    5
#endif
#ifndef CSR_BT_GATT_ADV_DATA_APPEARANCE
#define CSR_BT_GATT_ADV_DATA_APPEARANCE                  6
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SLAVE_CONN_INTERVAL
#define CSR_BT_GATT_ADV_DATA_SLAVE_CONN_INTERVAL         7
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SM_TK
#define CSR_BT_GATT_ADV_DATA_SM_TK                       8
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SM_OOB
#define CSR_BT_GATT_ADV_DATA_SM_OOB                      9
#endif
#ifndef CSR_BT_GATT_ADV_DATA_UUID128
#define CSR_BT_GATT_ADV_DATA_UUID128                    10
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SERV_SOLICITATION_16
#define CSR_BT_GATT_ADV_DATA_SERV_SOLICITATION_16       11
#endif
#ifndef CSR_BT_GATT_ADV_DATA_SERV_SOLICITATION_128
#define CSR_BT_GATT_ADV_DATA_SERV_SOLICITATION_128      12
#endif
#ifndef CSR_BT_GATT_ADV_DATA_PUBLIC_TARGET_ADDRESS
#define CSR_BT_GATT_ADV_DATA_PUBLIC_TARGET_ADDRESS      13
#endif
#ifndef CSR_BT_GATT_ADV_DATA_RANDOM_TARGET_ADDRESS
#define CSR_BT_GATT_ADV_DATA_RANDOM_TARGET_ADDRESS      14
#endif
/*number of priorities */
#define CSR_BT_GATT_ADV_DATA_PRIORITY_COUNT      14
/* Synergy BT can automatically add the local name to the LE advertising or scan data.
 * If set to FALSE, the application can still manually set local name in the AD Data supplied
 * in the Advertise Req. The local name sent out will always be the name used by the BT system.
 * The name will only be set in AD data for advertising data i GENERAL_DISCOVERABLE or LIMITED_DISCOVERABLE
 * as described in Core Spec volume3 Part C section 9.2.4 and 9.2.5
 */

#define CSR_BT_GATT_ADV_DATA_AUTO_LOCAL_NAME               TRUE
/*
 * If there is not enough room for the full local name in the adv data
 * we try to insert a shortened version of the name. This define
 * tells synergy bt how many bytes of the local name we should as
 * a minimum have room for when shortening the name.
 * Note that name is in utf8 and value is in bytes.
 */
#define CSR_BT_GATT_ADV_SHORT_LOCAL_NAME_MIN_LENGTH        1

/* Application error code for rejecting access to an attribute over an invalid transport.
 * Its value must be set between 0x80-0x9F */
#define CSR_BT_GATT_ACCESS_RES_INVALID_TRANSPORT            (0x80)

/*--------------------------------------------------------------------------
    Defines the default policy settings used for sniff.
----------------------------------------------------------------------------*/
/* Define the maximum acceptable number of baseband slots between
   each sniff period.
   Range: 0x0002 to 0xFFFE; only even values are valid
   Mandatory Range: 0x0006 to 0x0540
   Time = N * 0.625 msec Time Range: 1.25 msec to 40.9 sec

   Note that CSR_BT_SNIFF_MAX_TIME_INTERVAL must be less than
   CSR_BT_DEFAULT_LINK_SUPERVISION_TIMEOUT                          */
#ifndef CSR_BT_SNIFF_MAX_TIME_INTERVAL
#define CSR_BT_SNIFF_MAX_TIME_INTERVAL                          0x0320
#endif

/* Define the minimum acceptable number of baseband slots between
   each sniff period.

   Range: 0x0002 to 0xFFFE; only even values are valid
   Mandatory Range: 0x0006 to 0x0540
   Time = N * 0.625 msec
   Time Range: 1.25 msec to 40.9 sec

   Note that CSR_BT_SNIFF_MIN_TIME_INTERVAL must not be greater
   than CSR_BT_SNIFF_MAX_TIME_INTERVAL                              */
#ifndef CSR_BT_SNIFF_MIN_TIME_INTERVAL
#define CSR_BT_SNIFF_MIN_TIME_INTERVAL                          0x00C8
#endif

/* Define the number of baseband receive slots for sniff attempt
   Length = N* 1.25 msec
   Range for N: 0x0001 - 0x7FFF
   Time Range: 1.25msec - 40.9 Seconds                              */
#ifndef CSR_BT_SNIFF_ATTEMPT
#define CSR_BT_SNIFF_ATTEMPT                                    0x0004
#endif

/* Define the number of baseband receive slots for sniff timeout
   Length = N * 1.25 msec
   Range for N: 0x0000 - 0x7FFF
   Time Range: 0 msec - 40.9 Seconds                                */
#ifndef CSR_BT_SNIFF_TIMEOUT
#define CSR_BT_SNIFF_TIMEOUT                                    0x0001
#endif

/*--------------------------------------------------------------------------
 * Defines which sniff settings to use, when all logical connection
 *  on a given ACL is switch to AMP
 *--------------------------------------------------------------------------*/
/* Define the maximum acceptable number of baseband slots between each
   sniff period                                                             */
#ifndef CSR_BT_AMP_SNIFF_MAX_TIME_INTERVAL
#define CSR_BT_AMP_SNIFF_MAX_TIME_INTERVAL                                 (0x0320)
#endif

/* Define the minimum acceptable number of baseband slots between each
   sniff period                                                             */
#ifndef CSR_BT_AMP_SNIFF_MIN_TIME_INTERVAL
#define CSR_BT_AMP_SNIFF_MIN_TIME_INTERVAL                                 (0x00C8)
#endif

/* Define the number of baseband receive slots for sniff attempt            */
#ifndef CSR_BT_AMP_SNIFF_ATTEMPT
#define CSR_BT_AMP_SNIFF_ATTEMPT                                           (4)
#endif

/* Define the number of baseband receive slots for sniff timeout            */
#ifndef CSR_BT_AMP_SNIFF_TIMEOUT
#define CSR_BT_AMP_SNIFF_TIMEOUT                                           (2)
#endif

/*--------------------------------------------------------------------------
 * HCI Build MTU sizes
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_HCI_BUILD_L2CAP_EDR_MAX_FRAME_SIZE
#define CSR_BT_HCI_BUILD_L2CAP_EDR_MAX_FRAME_SIZE               (2033)
#endif /* CSR_BT_HCI_BUILD_L2CAP_EDR_MAX_FRAME_SIZE */

#ifndef CSR_BT_HCI_BUILD_RFCOMM_EDR_MAX_FRAME_SIZE
#define CSR_BT_HCI_BUILD_RFCOMM_EDR_MAX_FRAME_SIZE              (CSR_BT_HCI_BUILD_L2CAP_EDR_MAX_FRAME_SIZE-6)
#endif /* CSR_BT_HCI_BUILD_RFCOMM_EDR_MAX_FRAME_SIZE */

#ifndef CSR_BT_HCI_BUILD_L2CAP_NON_EDR_MAX_FRAME_SIZE
#define CSR_BT_HCI_BUILD_L2CAP_NON_EDR_MAX_FRAME_SIZE           (668)
#endif /* CSR_BT_HCI_BUILD_L2CAP_NON_EDR_MAX_FRAME_SIZE */

#ifndef CSR_BT_HCI_BUILD_RFCOMM_NON_EDR_MAX_FRAME_SIZE
#define CSR_BT_HCI_BUILD_RFCOMM_NON_EDR_MAX_FRAME_SIZE          (CSR_BT_HCI_BUILD_L2CAP_NON_EDR_MAX_FRAME_SIZE-6)
#endif /* CSR_BT_HCI_BUILD_RFCOMM_NON_EDR_MAX_FRAME_SIZE */

/*--------------------------------------------------------------------------
 * RFC Build MTU sizes
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_RFC_BUILD_L2CAP_MAX_FRAME_SIZE
#define CSR_BT_RFC_BUILD_L2CAP_MAX_FRAME_SIZE                   (335)
#endif /* CSR_BT_RFC_BUILD_L2CAP_MAX_FRAME_SIZE */

#ifndef CSR_BT_RFC_BUILD_RFCOMM_MAX_FRAME_SIZE
#define CSR_BT_RFC_BUILD_RFCOMM_MAX_FRAME_SIZE                  (CSR_BT_RFC_BUILD_L2CAP_MAX_FRAME_SIZE-6)
#endif /* CSR_BT_RFC_BUILD_RFCOMM_MAX_FRAME_SIZE */

/*--------------------------------------------------------------------------
 * General profiles default MTU sizes
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_AV_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_AV_PROFILE_DEFAULT_MTU_SIZE                      726  /* CSR_BT_HCI_BUILD_L2CAP_NON_EDR_MAX_FRAME_SIZE */
#endif /* CSR_BT_AV_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_BIPC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_BIPC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_BIPC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_BIPS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_BIPS_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_BIPS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_BPPC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_BPPC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_BPPC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_BPPS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_BPPS_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_BPPS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_BSL_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_BSL_PROFILE_DEFAULT_MTU_SIZE                     (1691)
#endif /* CSR_BT_BSL_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_DG_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_DG_PROFILE_DEFAULT_MTU_SIZE                      CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_DG_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_DUNC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_DUNC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_DUNC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_FTC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_FTC_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_FTC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_FTS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_FTS_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_FTS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_HCRP_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_HCRP_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_HCRP_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_HF_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_HF_PROFILE_DEFAULT_MTU_SIZE                      CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_HF_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_HFG_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_HFG_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_HFG_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_HIDD_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_HIDD_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_HIDD_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_HIDH_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_HIDH_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_HIDH_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_JSR82_L2CAP_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_JSR82_L2CAP_PROFILE_DEFAULT_MTU_SIZE             CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_JSR82_L2CAP_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_JSR82_RFCOMM_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_JSR82_RFCOMM_PROFILE_DEFAULT_MTU_SIZE            CSR_BT_RFC_BUILD_RFCOMM_MAX_FRAME_SIZE
#endif /* CSR_BT_JSR82_RFCOMM_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_OPC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_OPC_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_OPC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_OPS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_OPS_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_OPS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_PAC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_PAC_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_PAC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_PAS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_PAS_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_PAS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SAPC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SAPC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SAPC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SAPS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SAPS_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SAPS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SMLC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SMLC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SMLC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SMLS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SMLS_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SMLS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SPP_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SPP_PROFILE_DEFAULT_MTU_SIZE                     CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SPP_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_MAPC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_MAPC_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_MAPC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_MAPS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_MAPS_PROFILE_DEFAULT_MTU_SIZE                    CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_MAPS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SYNCC_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SYNCC_PROFILE_DEFAULT_MTU_SIZE                   CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SYNCC_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_SYNCS_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_SYNCS_PROFILE_DEFAULT_MTU_SIZE                   CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_SYNCS_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_GNSS_CLIENT_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_GNSS_CLIENT_PROFILE_DEFAULT_MTU_SIZE             CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_GNSS_CLIENT_PROFILE_DEFAULT_MTU_SIZE */

#ifndef CSR_BT_GNSS_SERVER_PROFILE_DEFAULT_MTU_SIZE
#define CSR_BT_GNSS_SERVER_PROFILE_DEFAULT_MTU_SIZE             CSR_BT_AUTO_SELECT_MAX_FRAME_SIZE
#endif /* CSR_BT_GNSS_SERVER_PROFILE_DEFAULT_MTU_SIZE */

/*--------------------------------------------------------------------------
 * General defines for Synergy BT
 *--------------------------------------------------------------------------*/

/* This define specifies the Bluetooth version that should be
 * supported by Synergy BT. Setting CSR_BT_BT_VERSION lower than
 * default will exclude features requiring a newer Bluetooth version.
 *
 * For example will CSR_BT_BT_VERSION == CSR_BT_BLUETOOTH_VERSION_2P0
 * exclude support for Secure Simple Pairing, Extended Inquiry
 * Response and other features only supported by Bluetooth versions
 * newer than 2.0. The benefit of decreasing CSR_BT_BT_VERSION is that
 * the code base and memory consumption of Synergy BT can be
 * reduced. Possible values for the define can be found in
 * csr_bt_profiles.h.
 *
 * Note: AMP requires Bluetooth version 3.0
 * Note: Bluetooth Low Energy requires version 4.0 or later */
#ifndef CSR_BT_BT_VERSION
#define CSR_BT_BT_VERSION                         (CSR_BT_BLUETOOTH_VERSION_5P0)
#endif /* CSR_BT_BT_VERSION */

/*--------------------------------------------------------------------------
 * Defines for Service Discovery (SD) module
 *--------------------------------------------------------------------------*/

/* The maximum number of search results to send to the application
 * during a device search */
#ifndef SD_SEARCH_MAX_RESULTS_DEFAULT
#define SD_SEARCH_MAX_RESULTS_DEFAULT           (CSR_BT_UNLIMITED)
#endif /* SD_SEARCH_MAX_RESULTS_DEFAULT */

/* Time-out in milliseconds for reading the name of a remote device */
#ifndef CSR_BT_SD_READ_NAME_TIMEOUT_DEFAULT
#define CSR_BT_SD_READ_NAME_TIMEOUT_DEFAULT            ((CsrTime) 5000)
#endif /* CSR_BT_SD_READ_NAME_TIMEOUT_DEFAULT */

/* Bitmask specifying the default configuration to be used for device search */
#ifndef CSR_BT_SD_SEARCH_CONFIG_DEFAULT
#define CSR_BT_SD_SEARCH_CONFIG_DEFAULT                (CSR_BT_SD_SEARCH_USE_STANDARD)
#endif /* CSR_BT_SD_SEARCH_CONFIG_DEFAULT */

/* Bitmask specifying the default memory configuration for the SD - must be
 * set to SD_MEMORY_CONFIG_USE_STANDARD for now */
#ifndef CSR_BT_SD_MEMORY_CONFIG_DEFAULT
#define CSR_BT_SD_MEMORY_CONFIG_DEFAULT                (CSR_BT_SD_MEMORY_CONFIG_USE_STANDARD)
#endif /* CSR_BT_SD_MEMORY_CONFIG_DEFAULT */

/* Max total number of devices to store in the Device List */
#ifndef CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT
#define CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT              (100)
#endif /* CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT */

/* Max number of devices to store extended information (name, services...)
   for. Must be smaller or equal to CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT and must be
   set to a minimum of 1 */
#ifndef CSR_BT_SD_MEMORY_LIST_INFO_MAX_DEFAULT
#define CSR_BT_SD_MEMORY_LIST_INFO_MAX_DEFAULT         (25)
#endif /* CSR_BT_SD_MEMORY_LIST_INFO_MAX_DEFAULT */

/*--------------------------------------------------------------------------
 * Defines for Class Of Device (COD) and service/friendly names
 *--------------------------------------------------------------------------*/
/* Use to define the Major and Minor device class fields of the Class
 * of Device. */
#ifndef CSR_BT_MAJOR_MINOR_DEVICE_CLASS
#define CSR_BT_MAJOR_MINOR_DEVICE_CLASS               0x000204 /* (CSR_BT_PHONE_MAJOR_DEVICE_MASK + CSR_BT_CELLULAR_MINOR_DEVICE_MASK) */
#endif /* CSR_BT_MAJOR_MINOR_DEVICE_CLASS */

/* This is the maximum amount of chars to be passed between the
 * profile layer and the application for the remote name if the name
 * is shorter than the CSRMAX length, the name must be zero terminated.
 * If a name longer than 40 chars (20 words) is sent to the chip during
 * bootstrap, the name will be ignored. Max length for the friendly name
 * is defined as 248 octets in HCI, but this will be limited in Synergy BT
 * by this define.
 */
#ifndef CSR_BT_MAX_FRIENDLY_NAME_LEN
#define CSR_BT_MAX_FRIENDLY_NAME_LEN                                   (50)
#endif /* CSR_BT_MAX_FRIENDLY_NAME_LEN */

/* Define the default startup connectable mode. */
#ifndef CSR_BT_CM_DEFAULT_STARTUP_CONNECTABLE_MODE
#define CSR_BT_CM_DEFAULT_STARTUP_CONNECTABLE_MODE                     (HCI_SCAN_ENABLE_INQ)
#endif /* CSR_BT_CM_DEFAULT_STARTUP_CONNECTABLE_MODE */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT CM
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_CM_CONNECTIONLESS_UNMAP_DELAY
#define CSR_BT_CM_CONNECTIONLESS_UNMAP_DELAY                            (3*CSR_SCHED_SECOND)
#endif

/*--------------------------------------------------------------------------
  Defines for Synergy BT RFCOMM connection
  ----------------------------------------------------------------------------*/
/* Defines how many data frames each RFCOMM connection in the CM is
 * able to accept before its buffer is fill up. The value between 1-255.
 * Setting this value low can result in a lower bandrate */
#ifndef CSR_BT_CM_INIT_CREDIT
#define CSR_BT_CM_INIT_CREDIT                                          (25)
#endif /* CSR_BT_CM_INIT_CREDIT */

/* L2CAP mode used by RFCOMM
 *
 * It is _strongly_ recommended to leave this at BASIC mode. Enhanced
 * retransmission mode (ERTM) is, however, needed for RFCOMM-over-AMP
 * support. CSR does not recommend using RFCOMM-over-AMP.
 *
 * Possible values are: L2CA_FLOW_MODE_BASIC (highly recommended) or
 *                      L2CA_FLOW_MODE_ENHANCED_RETRANS */
#ifndef CSR_BT_RFCOMM_PREFERRED_MODE
#define CSR_BT_RFCOMM_PREFERRED_MODE                                   (L2CA_FLOW_MODE_BASIC)
#endif /* CSR_BT_RFCOMM_PREFERRED_MODE */

/* Window size for RFCOMM over L2CAP enhanced retransmission mode */
#ifndef CSR_BT_RFCOMM_WINSIZE
#define CSR_BT_RFCOMM_WINSIZE                                          (5)
#endif /* CSR_BT_RFCOMM_WINSIZE */

/* Port negotiation defines */
#ifndef CSR_BT_DEFAULT_PORT_SPEED
#define CSR_BT_DEFAULT_PORT_SPEED                                      (RFC_115200_BAUD)
#endif /* CSR_BT_DEFAULT_PORT_SPEED */

#ifndef CSR_BT_DEFAULT_DATA_BITS
#define CSR_BT_DEFAULT_DATA_BITS                                       (RFC_DATA_BITS_8)
#endif /* CSR_BT_DEFAULT_DATA_BITS */

#ifndef CSR_BT_DEFAULT_STOP_BITS
#define CSR_BT_DEFAULT_STOP_BITS                                       (RFC_STOP_BITS_ONE)
#endif /* CSR_BT_DEFAULT_STOP_BITS */

#ifndef CSR_BT_DEFAULT_PARITY
#define CSR_BT_DEFAULT_PARITY                                          (RFC_PARITY_ON)
#endif /* CSR_BT_DEFAULT_PARITY */

#ifndef CSR_BT_DEFAULT_PARITY_TYPE
#define CSR_BT_DEFAULT_PARITY_TYPE                                     (RFC_PARITY_TYPE_EVEN)
#endif /* CSR_BT_DEFAULT_PARITY_TYPE */

#ifndef CSR_BT_DEFAULT_FLOW_CTRL_MASK
#define CSR_BT_DEFAULT_FLOW_CTRL_MASK                                  (RFC_FLC_RTR_INPUT)
#endif /* CSR_BT_DEFAULT_FLOW_CTRL_MASK */

#ifndef CSR_BT_DEFAULT_XON
#define CSR_BT_DEFAULT_XON                                             (RFC_XON_CHAR_DEFAULT)
#endif /* CSR_BT_DEFAULT_XON */

#ifndef CSR_BT_DEFAULT_XOFF
#define CSR_BT_DEFAULT_XOFF                                            (RFC_XOFF_CHAR_DEFAULT)
#endif /* CSR_BT_DEFAULT_XOFF */

#ifndef CSR_BT_DEFAULT_PARAMETER_MASK
#define CSR_BT_DEFAULT_PARAMETER_MASK                                  (0x3FFF)
#endif /* CSR_BT_DEFAULT_PARAMETER_MASK */

#ifndef CSR_BT_DEFAULT_MODEM_STATUS
#define CSR_BT_DEFAULT_MODEM_STATUS                                   (0x00)
#endif /* CSR_BT_DEFAULT_MODEM_STATUS */

#ifndef CSR_BT_DEFAULT_BREAK_SIGNAL
#define CSR_BT_DEFAULT_BREAK_SIGNAL                                   (0x00)
#endif /* CSR_BT_DEFAULT_BREAK_SIGNAL */

#ifndef CSR_BT_DEFAULT_MSC_TIMEOUT
#define CSR_BT_DEFAULT_MSC_TIMEOUT                                    (250)
#endif /* CSR_BT_DEFAULT_MSC_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT page scan
 *--------------------------------------------------------------------------*/
/* Define the Page Scan interval: 0x12 - 0x1000, default 0x800 */
#ifndef CSR_BT_PAGE_SCAN_INTERVAL_DEFAULT
#define CSR_BT_PAGE_SCAN_INTERVAL_DEFAULT                              (0x800)
#endif /* CSR_BT_PAGE_SCAN_INTERVAL_DEFAULT */

/* Define the Page Scan window: 0x12 - 0x1000, default 0x12 */
#ifndef CSR_BT_PAGE_SCAN_WINDOW_DEFAULT
#define CSR_BT_PAGE_SCAN_WINDOW_DEFAULT                                (0x12)
#endif /* CSR_BT_PAGE_SCAN_WINDOW_DEFAULT */

/* Define the timeout window before page request fails.
 * Sec = CSR_BT_PAGE_TIMEOUT_DEFAULT * 0.000625 */
#ifndef CSR_BT_PAGE_TIMEOUT_DEFAULT
#define CSR_BT_PAGE_TIMEOUT_DEFAULT                                    (24000)
#endif /* CSR_BT_PAGE_TIMEOUT_DEFAULT */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT Page Scan Type and Inquiry Scan Type
 *--------------------------------------------------------------------------*/
/* Use HCI_SCAN_TYPE_LEGACY for standard scan.
 * HCI_SCAN_TYPE_INTERLACED for interlaced scan.
 *
 * Please note that the drawbacks of using HCI_SCAN_TYPE_INTERLACED
 * scan is that it uses more power than HCI_SCAN_TYPE_LEGACY. The
 * advantage of using HCI_SCAN_TYPE_INTERLACED is that a peer device
 * faster can make inquiries and connect to this
 * device. */
#ifndef CSR_BT_PAGE_SCAN_TYPE_DEFAULT
#define CSR_BT_PAGE_SCAN_TYPE_DEFAULT                                  (HCI_SCAN_TYPE_LEGACY)
#endif /* CSR_BT_PAGE_SCAN_TYPE_DEFAULT */

#ifndef CSR_BT_INQUIRY_SCAN_TYPE_DEFAULT
#define CSR_BT_INQUIRY_SCAN_TYPE_DEFAULT                               (HCI_SCAN_TYPE_LEGACY)
#endif /* CSR_BT_INQUIRY_SCAN_TYPE_DEFAULT */

/*--------------------------------------------------------------------------
  Defines for Synergy BT voice parameter
  ----------------------------------------------------------------------------*/
/* Voice setting parameter controls all the various settings for voice
 * connection. These settings apply to all voice connection, and
 * CANNOT be set for individual voice connections. The voice setting
 * parameter controls the configuration for voice connections : Input
 * coding, input data format, input sample size and air coding.
 *
 * In order to set the voice setting parameters, the user need to pick
 * one of each configuration parameter and OR them together. The
 * result must is the value of CSR_BT_VOICE_SETTING.
 *
 * E.g CSR_BT_VOICE_SETTING set to : *
 *   CSR_BT_INPUTCODING_LINEAR | CSR_BT_INPUTDATAFORMAT_2COMPLEMENT |
 *   CSR_BT_INPUTSAMPLESIZE_16BIT | CSR_BT_AIRCODING_CVSD)
 * means that the input coding is linear, the input data format is 2's
 * complement, the input sample size is 16 bit, and the aircoding is
 * CVSD */

/* Defines Input Coding */
#ifndef CSR_BT_INPUTCODING_LINEAR
#define CSR_BT_INPUTCODING_LINEAR                                      0x0000
#endif /* CSR_BT_INPUTCODING_LINEAR */

#ifndef CSR_BT_INPUTCODING_my_LAW
#define CSR_BT_INPUTCODING_my_LAW                                      0x0100
#endif /* CSR_BT_INPUTCODING_my_LAW */

#ifndef CSR_BT_INPUTCODING_A_LAW
#define CSR_BT_INPUTCODING_A_LAW                                       0x0200
#endif /* CSR_BT_INPUTCODING_A_LAW */

/* Defines input data format */
#ifndef CSR_BT_INPUTDATAFORMAT_1COMPLEMENT
#define CSR_BT_INPUTDATAFORMAT_1COMPLEMENT                             0x0000
#endif /* CSR_BT_INPUTDATAFORMAT_1COMPLEMENT */

#ifndef CSR_BT_INPUTDATAFORMAT_2COMPLEMENT
#define CSR_BT_INPUTDATAFORMAT_2COMPLEMENT                             0x0040
#endif /* CSR_BT_INPUTDATAFORMAT_2COMPLEMENT */

#ifndef CSR_BT_INPUTDATAFORMAT_SIGNMAGNITUDE
#define CSR_BT_INPUTDATAFORMAT_SIGNMAGNITUDE                           0x0080
#endif /* CSR_BT_INPUTDATAFORMAT_SIGNMAGNITUDE */

/* Defines input sample size */
#ifndef CSR_BT_INPUTSAMPLESIZE_8BIT
#define CSR_BT_INPUTSAMPLESIZE_8BIT                                    0x0000
#endif /* CSR_BT_INPUTSAMPLESIZE_8BIT */

#ifndef CSR_BT_INPUTSAMPLESIZE_16BIT
#define CSR_BT_INPUTSAMPLESIZE_16BIT                                   0x0020
#endif /* CSR_BT_INPUTSAMPLESIZE_16BIT */

/* Defines air Coding */
#ifndef CSR_BT_AIRCODING_CVSD
#define CSR_BT_AIRCODING_CVSD                                          0x0000
#endif /* CSR_BT_AIRCODING_CVSD */

#ifndef CSR_BT_AIRCODING_my_LAW
#define CSR_BT_AIRCODING_my_LAW                                        0x0001
#endif /* CSR_BT_AIRCODING_my_LAW */

#ifndef CSR_BT_AIRCODING_A_LAW
#define CSR_BT_AIRCODING_A_LAW                                         0x0002
#endif /* CSR_BT_AIRCODING_A_LAW */

#ifndef CSR_BT_AIRCODING_TRANSPARENT_DATA
#define CSR_BT_AIRCODING_TRANSPARENT_DATA                              0x0003
#endif /* CSR_BT_AIRCODING_TRANSPARENT_DATA */

/* This is the default air-coding voice setting */
#ifndef CSR_BT_VOICE_SETTING_AIR
#define CSR_BT_VOICE_SETTING_AIR                                       (CSR_BT_AIRCODING_CVSD)
#endif /* CSR_BT_VOICE_SETTING_AIR */

/* This is the default input-coding voice setting */
#ifndef CSR_BT_VOICE_SETTING_INPUT
#define CSR_BT_VOICE_SETTING_INPUT                                     (CSR_BT_INPUTCODING_LINEAR | \
                                                                 CSR_BT_INPUTDATAFORMAT_2COMPLEMENT | \
                                                                 CSR_BT_INPUTSAMPLESIZE_16BIT )
#endif /* CSR_BT_VOICE_SETTING_INPUT */

/* The default combined voice-setting. This will be written to the
 * chip when Synergy BT boots. Settings can after that only be changed
 * if the specific profile allows it, and only if there are no active
 * SCO connections. */
#ifndef CSR_BT_VOICE_SETTING
#define CSR_BT_VOICE_SETTING                                           (CSR_BT_VOICE_SETTING_AIR | CSR_BT_VOICE_SETTING_INPUT)
#endif /* CSR_BT_VOICE_SETTING */


/*--------------------------------------------------------------------------
 * Defines for Synergy BT SCO/SYNC connection
 *--------------------------------------------------------------------------*/

/* The maximum number of simultanrous eSCO/SCO connections that BT can
 * support */
#ifndef CSR_BT_MAX_NUM_SCO_CONNS
#define CSR_BT_MAX_NUM_SCO_CONNS                                       (3)
#endif

/* Default values for the accept_connect_sco for Handsfree
 * Gateway. Note that CVSD encoding is the only air-coding allowed for
 * HFG, so we use that and OR the default voice setting input
 * coding */
#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_TX_BANDWIDTH
#define CSR_BT_SCO_DEFAULT_ACCEPT_TX_BANDWIDTH                         (0xFFFFFFFF)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_TX_BANDWIDTH */

#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_RX_BANDWIDTH
#define CSR_BT_SCO_DEFAULT_ACCEPT_RX_BANDWIDTH                         (0xFFFFFFFF)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_RX_BANDWIDTH */

#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_MAX_LATENCY
#define CSR_BT_SCO_DEFAULT_ACCEPT_MAX_LATENCY                          (0xFFFF)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_MAX_LATENCY */

#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_VOICE_SETTINGS
#define CSR_BT_SCO_DEFAULT_ACCEPT_VOICE_SETTINGS                       (CSR_BT_AIRCODING_CVSD | CSR_BT_VOICE_SETTING_INPUT)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_VOICE_SETTINGS */

#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_RE_TX_EFFORT
#define CSR_BT_SCO_DEFAULT_ACCEPT_RE_TX_EFFORT                         (0xFF)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_RE_TX_EFFORT */

/* 3M EDR eSCO packets cannot be used on BC4 with CVSD.  This value
 * can be changed to 0xFC3F if using BC5 or newer, or if another
 * aircoding is used. */
#ifndef CSR_BT_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY
#define CSR_BT_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY (0xFEBF)
#endif /* CSR_BT_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY */

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_TX_BANDWIDTH
#define CSR_BT_ESCO_DEFAULT_CONNECT_TX_BANDWIDTH                       (8000)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_TX_BANDWIDTH */

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_RX_BANDWIDTH
#define CSR_BT_ESCO_DEFAULT_CONNECT_RX_BANDWIDTH                       (8000)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_RX_BANDWIDTH */

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_MAX_LATENCY
#define CSR_BT_ESCO_DEFAULT_CONNECT_MAX_LATENCY                        (0x000C)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_MAX_LATENCY */

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_VOICE_SETTINGS
#define CSR_BT_ESCO_DEFAULT_CONNECT_VOICE_SETTINGS                     (CSR_BT_AIRCODING_CVSD | CSR_BT_VOICE_SETTING_INPUT)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_VOICE_SETTINGS */

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_RE_TX_EFFORT
#define CSR_BT_ESCO_DEFAULT_CONNECT_RE_TX_EFFORT                       (HCI_ESCO_LINK_QUALITY_RETX)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_RE_TX_EFFORT */

/* 3M EDR eSCO packets cannot be used on BC4 with CVSD.  This value
 * can be changed to 0x003F if using BC5 or newer, or if another
 * aircoding is used. */

#ifndef HCI_ALL_HV_SCO_PACKET_TYPES
#define HCI_ALL_HV_SCO_PACKET_TYPES  ((hci_pkt_type_t)HCI_ESCO_PKT_HV1 | HCI_ESCO_PKT_HV2 | HCI_ESCO_PKT_HV3)
#endif

#ifndef HCI_ALL_SCO_PACKET_TYPES
#define HCI_ALL_SCO_PACKET_TYPES     ((hci_pkt_type_t)HCI_ALL_HV_SCO_PACKET_TYPES | HCI_ESCO_PKT_EV3 | HCI_ESCO_PKT_EV4 | HCI_ESCO_PKT_EV5)
#endif

/* Medium Rate eSCO defines to be used OR'ed "as-is" and not negated and AND'ed
   (Beware: bit set means the packet type may not be used!)                     */
#ifndef HCI_NOT_ESCO_PKT_2EV3
#define HCI_NOT_ESCO_PKT_2EV3  ((hci_pkt_type_t)0x0040)
#endif

#ifndef HCI_NOT_ESCO_PKT_3EV3
#define HCI_NOT_ESCO_PKT_3EV3  ((hci_pkt_type_t)0x0080)
#endif

#ifndef HCI_NOT_ESCO_PKT_2EV5
#define HCI_NOT_ESCO_PKT_2EV5  ((hci_pkt_type_t)0x0100)
#endif

#ifndef HCI_NOT_ESCO_PKT_3EV5
#define HCI_NOT_ESCO_PKT_3EV5  ((hci_pkt_type_t)0x0200)
#endif

#ifndef CSR_BT_ESCO_DEFAULT_CONNECT_AUDIO_QUALITY
#define CSR_BT_ESCO_DEFAULT_CONNECT_AUDIO_QUALITY          ((hci_pkt_type_t)HCI_ALL_SCO_PACKET_TYPES | HCI_NOT_ESCO_PKT_3EV3 | HCI_NOT_ESCO_PKT_3EV5)
#endif /* CSR_BT_ESCO_DEFAULT_CONNECT_AUDIO_QUALITY */

/* Default packet type for accept of incoming SCO and eSCO (S1, S2 ans S3) */
#ifndef CSR_BT_COMPLETE_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY              /* HV1 + HV2 + HV3 + EV3 + not 3-EV3 + not 2-EV5 + not 3-EV5 */
#define CSR_BT_COMPLETE_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY   ((hci_pkt_type_t)HCI_ALL_HV_SCO_PACKET_TYPES | HCI_ESCO_PKT_EV3 | HCI_NOT_ESCO_PKT_3EV3 | HCI_NOT_ESCO_PKT_2EV5 | HCI_NOT_ESCO_PKT_3EV5)
#endif /* CSR_BT_COMPLETE_SCO_DEFAULT_ACCEPT_AUDIO_QUALITY */

/*----------------------------------------------------------------------------
 * Defines for JSR82
 *----------------------------------------------------------------------------*/

/* This is the CSRMAX number of bytes the JSR82 layer will accept from
 * the SD in one SD_READ_TRUSTED_DEVICES_IND/CFM signal */
#ifndef CSR_BT_JSR82_MAX_MEMORY_BLOCK
#define CSR_BT_JSR82_MAX_MEMORY_BLOCK                        (2000)
#endif /* CSR_BT_JSR82_MAX_MEMORY_BLOCK */

/* This is the time in seconds that the JSR-82 layer uses for the
 * inquiry procedure */
#ifndef CSR_BT_JSR82_INQ_TIME
#define CSR_BT_JSR82_INQ_TIME                                          (11)
#endif /* CSR_BT_JSR82_INQ_TIME */

/* This is the CSRMAX number of results that should be returned during
 * an inquiry in JSR-82. 0 means no limit. */
#ifndef CSR_BT_JSR82_INQ_MAX_RESULTS
#define CSR_BT_JSR82_INQ_MAX_RESULTS                                   (0)
#endif /* CSR_BT_JSR82_INQ_MAX_RESULTS */

/* Max number of packets to store in the receive buffer for a JSR-82
 * RFCOMM connection. If more than this is received, the oldest packets
 * will be dropped */
#ifndef CSR_BT_JSR82_RFC_BUFFER_SIZE
#define CSR_BT_JSR82_RFC_BUFFER_SIZE                                   ((CsrUint32) 5)
#endif /* CSR_BT_JSR82_RFC_BUFFER_SIZE */

/* Max number of packets to store in the receive buffer for a JSR-82
 * L2CAP connection. If more than this is received, the oldest packets
 * will be dropped */
#ifndef CSR_BT_JSR82_MAX_UNPROCESSED_DATA_PACKETS
#define CSR_BT_JSR82_MAX_UNPROCESSED_DATA_PACKETS                      ((CsrUint32) 25)
#endif /* CSR_BT_JSR82_MAX_UNPROCESSED_DATA_PACKETS */

/* The maximum number of dynamic range PSMs that may be allocated to the JSR82 task */
#ifndef CSR_BT_JSR82_MAX_ALLOCATED_PSMS
#define CSR_BT_JSR82_MAX_ALLOCATED_PSMS                                (16)
#endif /* CSR_BT_JSR82_MAX_ALLOCATED_PSMS */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT GNSS
 *--------------------------------------------------------------------------*/
/* GNSS client active to sniff timeout when connections are established */
#ifndef CSR_BT_GNSS_CLIENT_LPM_TIME
#define CSR_BT_GNSS_CLIENT_LPM_TIME                     (1 * CSR_SCHED_SECOND)
#endif /* CSR_BT_GNSS_CLIENT_LPM_TIME */

/* GNSS server default active to sniff timeout */
#ifndef CSR_BT_GNSS_SERVER_LPM_TIME_DEFAULT
#define CSR_BT_GNSS_SERVER_LPM_TIME_DEFAULT             (100)
#endif /* CSR_BT_GNSS_SERVER_LPM_TIME_DEFAULT */

/* GNSS server default LPM mode */
#ifndef CSR_BT_GNSS_SERVER_LPM_MODE
#define CSR_BT_GNSS_SERVER_LPM_MODE                     (CSR_BT_GNSS_SERVER_LPM_AUTO)
#endif /* CSR_BT_GNSS_SERVER_LPM_MODE */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT BNEP connection
 *--------------------------------------------------------------------------*/
/* Defines the maximum number of packets allowed on queue for each
 * bnep connection */
#ifndef CSR_BT_MAX_BSL_QUEUE_LENGTH
#define CSR_BT_MAX_BSL_QUEUE_LENGTH                                    (8)
#endif /* CSR_BT_MAX_BSL_QUEUE_LENGTH */

#ifndef CSR_BT_MAX_BSL_LOCAL_MULTICAST_QUEUE_LENGTH
#define CSR_BT_MAX_BSL_LOCAL_MULTICAST_QUEUE_LENGTH                    (8)
#endif /* CSR_BT_MAX_BSL_LOCAL_MULTICAST_QUEUE_LENGTH */

#ifndef CSR_BT_MAX_NUM_OF_SIMULTANEOUS_BNEP_CONNECTIONS
#define CSR_BT_MAX_NUM_OF_SIMULTANEOUS_BNEP_CONNECTIONS                (8) /* most be aleast 8 */
#endif /* CSR_BT_MAX_NUM_OF_SIMULTANEOUS_BNEP_CONNECTIONS */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT PAN connection
 *--------------------------------------------------------------------------*/
/* Enable or disable sniff mode when connections are established */
#ifndef CSR_BT_BSL_ENABLE_SNIFF
#define CSR_BT_BSL_ENABLE_SNIFF                        (TRUE)
#endif /* CSR_BT_BSL_ENABLE_SNIFF */

#ifndef CSR_BT_BSL_LP_TIMEOUT
#define CSR_BT_BSL_LP_TIMEOUT               (3000)   /* In Milliseconds */
#endif /* CSR_BT_BSL_LP_TIMEOUT */

#ifndef CSR_BT_BSL_MAX_NUM_OF_CONNECTIONS
#define CSR_BT_BSL_MAX_NUM_OF_CONNECTIONS              CSR_BT_MAX_NUM_OF_SIMULTANEOUS_BNEP_CONNECTIONS
#endif /* CSR_BT_BSL_MAX_NUM_OF_CONNECTIONS */

/*--------------------------------------------------------------------------
 * Defines for the Link Supervision timeout
 *--------------------------------------------------------------------------*/

/* Define the default link supervision timeout. Range is 0x0001 -
 * 0xFFFF and default is 0x7D00 (32000 ~ 20 seconds).  time_in_seconds
 * = CSR_BT_DEFAULT_LINK_SUPERVISION_TIMEOUT * 0.000625 0x0000 means infinite
 * link supervision timeout. */
#ifndef CSR_BT_DEFAULT_LINK_SUPERVISION_TIMEOUT
#define CSR_BT_DEFAULT_LINK_SUPERVISION_TIMEOUT                        (32000)
#endif /* CSR_BT_DEFAULT_LINK_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for Synergy BT OBEX parameter
 *--------------------------------------------------------------------------*/

/* The obex CSRMAX packet size received from the peer side will be
 * this value - 1024 dec.  Please notice that the largest acceptable
 * packet size value for IrOBEX is 0xFFFF, and the minimum value is
 * 0x00FF. Due to L2CAP headers, the maximum is 0xFFF0 */
#ifndef CSR_BT_MAX_OBEX_SIGNAL_LENGTH
#define CSR_BT_MAX_OBEX_SIGNAL_LENGTH                                  (0xFF00)
#endif /* CSR_BT_MAX_OBEX_SIGNAL_LENGTH */

#ifndef CSR_BT_OBEX_CONNECT_TIMEOUT
#define CSR_BT_OBEX_CONNECT_TIMEOUT                             (20000000) /* Mircosec */
#endif /* CSR_BT_OBEX_CONNECT_TIMEOUT (Recommended value if used is 5000000) */

#ifndef CSR_BT_OBEX_CONNECT_WITH_AUTH_TIMEOUT
#define CSR_BT_OBEX_CONNECT_WITH_AUTH_TIMEOUT                   (20000000) /* Mircosec */
#endif /* CSR_BT_OBEX_CONNECT_WITH_AUTH_TIMEOUT (Recommended value if used is 20000000)*/

#ifndef CSR_BT_OBEX_DISCONNECT_TIMEOUT
#define CSR_BT_OBEX_DISCONNECT_TIMEOUT                          (5000000) /* Mircosec */
#endif /* CSR_BT_OBEX_DISCONNECT_TIMEOUT (Recommended value if used is 5000000)*/



/*--------------------------------------------------------------------------
 * Defines for the Linux port
 *--------------------------------------------------------------------------*/

/* Number of USB devices to support in pure user mode */
#ifndef CSR_BT_BT_USB_COUNT
#define CSR_BT_BT_USB_COUNT                                            (32)
#endif

/* Setup which PCM SLOT the AV2ALSA device is allocated to */
#ifndef CSR_BT_AV2ALSA_PCM_SLOT
#define CSR_BT_AV2ALSA_PCM_SLOT                                        (0)
#endif /* CSR_BT_AV2ALSA_PCM_SLOT */

/*--------------------------------------------------------------------------
 * Defines for AV
 *--------------------------------------------------------------------------*/

/* Supported features to be advertised in A2DP service record. Sink
 * feature values */
#ifndef CSR_BT_AV_FEATURES_HEADPHONE
#define CSR_BT_AV_FEATURES_HEADPHONE                                0x0001
#endif /* CSR_BT_AV_FEATURES_HEADPHONE */

#ifndef CSR_BT_AV_FEATURES_SPEAKER
#define CSR_BT_AV_FEATURES_SPEAKER                                  0x0002
#endif /* CSR_BT_AV_FEATURES_SPEAKER */

#ifndef CSR_BT_AV_FEATURES_RECORDER
#define CSR_BT_AV_FEATURES_RECORDER                                 0x0004
#endif /* CSR_BT_AV_FEATURES_RECORDER */

#ifndef CSR_BT_AV_FEATURES_AMPLIFIER
#define CSR_BT_AV_FEATURES_AMPLIFIER                                0x0008
#endif /* CSR_BT_AV_FEATURES_AMPLIFIER */

/* Source feature values */
#ifndef CSR_BT_AV_FEATURES_PLAYER
#define CSR_BT_AV_FEATURES_PLAYER                                   0x0001
#endif /* CSR_BT_AV_FEATURES_PLAYER */

#ifndef CSR_BT_AV_FEATURES_MICROPHONE
#define CSR_BT_AV_FEATURES_MICROPHONE                               0x0002
#endif /* CSR_BT_AV_FEATURES_MICROPHONE */

#ifndef CSR_BT_AV_FEATURES_TUNER
#define CSR_BT_AV_FEATURES_TUNER                                    0x0004
#endif /* CSR_BT_AV_FEATURES_TUNER */

#ifndef CSR_BT_AV_FEATURES_MIXER
#define CSR_BT_AV_FEATURES_MIXER                                    0x0008
#endif /* CSR_BT_AV_FEATURES_MIXER */

/* Specify supported features by combining above values */
#ifndef CSR_BT_AV_SUPPORTED_FEATURES
#define CSR_BT_AV_SUPPORTED_FEATURES                                0x0000
#endif /* CSR_BT_AV_SUPPORTED_FEATURES */

/* Product/provider specific names */
#ifndef CSR_BT_AV_PROVIDER_NAME
#define CSR_BT_AV_PROVIDER_NAME                                     "CSR"
#endif /* CSR_BT_AV_PROVIDER_NAME */

#ifndef CSR_BT_AV_SERVICE_NAME
#define CSR_BT_AV_SERVICE_NAME                                      "Audio/Video Service"
#endif /* CSR_BT_AV_SERVICE_NAME */

/* the maximum number of concurrent AV connections */
#ifndef CSR_BT_AV_MAX_NUM_CONNECTIONS
#define CSR_BT_AV_MAX_NUM_CONNECTIONS      3
#endif /* CSR_BT_AV_MAX_NUM_CONNECTIONS */

/* the maximum number of concurrent AV streams, should at
   least be same value as CSR_BT_AV_MAX_NUM_CONNECTIONS */
#ifndef CSR_BT_AV_MAX_NUM_STREAMS
#define CSR_BT_AV_MAX_NUM_STREAMS          4
#endif /* CSR_BT_AV_MAX_NUM_STREAMS */

/* size of the media stream buffer, maximum number of media
   packets that can be buffered before flushing oldest data.
   Should be able to hold at least 50 ms data, must be >2 */
#ifndef CSR_BT_AV_MEDIA_BUFFER_SIZE
#define CSR_BT_AV_MEDIA_BUFFER_SIZE        100
#endif /* CSR_BT_AV_MEDIA_BUFFER_SIZE */

/* how often should AV report the buffer status (CSR_BT_AV_QOS_IND) in
   counts of CSR_BT_AV_STREAM_DATA_REQ primitives sent. If set to 0 (zero)
   an CSR_BT_AV_QOS_IND will only be sent in case the buffer is full
   and subsequently when the buffer again is emptied */
#ifndef CSR_BT_AV_QOS_IND_INTERVAL
#define CSR_BT_AV_QOS_IND_INTERVAL         50
#endif /* CSR_BT_AV_QOS_IND_INTERVAL */

/* Timer T-gavdp100 duration in milliseconds (500 - 3000 ms recommended) */
#ifndef CSR_BT_AV_CMD_SIGNAL_TIMEOUT
#define CSR_BT_AV_CMD_SIGNAL_TIMEOUT       3000
#endif /* CSR_BT_AV_CMD_SIGNAL_TIMEOUT */

/* Timer supervising signalling activity - if no signalling traffic has happened within the
    specified interval, the profile will request sniff mode on the signalling channel.
    Default 5000 msec. */
#ifndef CSR_BT_AV_SIGNAL_LP_TIMEOUT
#define CSR_BT_AV_SIGNAL_LP_TIMEOUT            5000
#endif /* CSR_BT_AV_SIGNAL_LP_TIMEOUT */

/* Timer supervising stream activity - trigger entering of sniff mode when
   stream is inactive for the time specified here. Default 3000 msec.
   This timer value is only applicable when AV_STREAM_INACTIVITY_LP_ENABLE
   is defined to something other than 0 */
#if defined(AV_STREAM_INACTIVITY_LP_ENABLE) && (AV_STREAM_INACTIVITY_LP_ENABLE > 0)
#ifndef CSR_BT_AV_LP_TIMEOUT
#define CSR_BT_AV_LP_TIMEOUT               (3000)
#endif /* CSR_BT_AV_LP_TIMEOUT */
#endif

/* When streaming data the following Quality of Service parameters
   is set for the concerned ACL connection. Note default values for
   ACL connection without streaming data are:
        service type    = HCI_QOS_BEST_EFFORT
        token rate      = 0x00000000
        peak bandwidth  = 0x00000000
        latency         = 0x000061A8
        delay variation = 0x000061A8
   Valid service type values are HCI_QOS_BEST_EFFORT,
   HCI_QOS_NO_TRAFFIC, HCI_QOS_GUARANTEED. Token Rate and Peak
   Bandwidth are in bytes per second.  Latency and Delay Variation are
   in micro seconds. */
#ifndef CSR_BT_AV_QOS_SERVICE_TYPE
#define CSR_BT_AV_QOS_SERVICE_TYPE        (HCI_QOS_BEST_EFFORT)
#endif
#ifndef CSR_BT_AV_QOS_TOKEN_RATE
#define CSR_BT_AV_QOS_TOKEN_RATE          (0x00000000)
#endif
#ifndef CSR_BT_AV_QOS_PEAK_BANDWIDTH
#define CSR_BT_AV_QOS_PEAK_BANDWIDTH      (0x00000000)
#endif
#ifndef CSR_BT_AV_QOS_LATENCY
#define CSR_BT_AV_QOS_LATENCY             (0x00002BF2)
#endif
#ifndef CSR_BT_AV_QOS_DELAY_VARIATION
#define CSR_BT_AV_QOS_DELAY_VARIATION     (0x00000000)
#endif

/*--------------------------------------------------------------------------
 * Defines for AVRCP
 *--------------------------------------------------------------------------*/
#define CSR_BT_AVRCP_QOS_RETRANSMIT_TIMEOUT                            300 /* FIXME:  */
#define CSR_BT_AVRCP_QOS_MONITOR_TIMEOUT                               300

/* Timers [us] from specification - do not change */
#define CSR_BT_AVRCP_TIMER_RCP                                         (100000)    /**< Response time for any pass through command */
#define CSR_BT_AVRCP_TIMER_MTC                                         (200000)    /**< Response time for vendor dependent CONTROL */
#define CSR_BT_AVRCP_TIMER_MTP                                         (1000000)   /**< Response time for vendor dependent INTERIM and STATUS */

/* Other timers */
#define CSR_BT_AVRCP_CT_TIMER_AIR_OVERHEAD                             (1500000)   /**< Additional time between commands and responses */
#define CSR_BT_AVRCP_TIMER_BROWSING                                    (1000000)   /**< Response time for browsing commands */
#define CSR_BT_AVRCP_TIMER_BROWSING_SEARCH                             (20000000)  /**< Response time for search command (browsing) */

/* AVRCP Imaging defines */
#define CSR_BT_AVRCP_IMAGING_CLIENT_LP_SUPERVISION_TIMEOUT             (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#define CSR_BT_AVRCP_IMAGING_SERVER_INCOMING_OBEX_PKT_SIZE             CSR_BT_MAX_OBEX_SIGNAL_LENGTH
/* This controls both incoming & outgoing OBEX pkt size */
#define CSR_BT_AVRCP_IMAGING_CLIENT_OBEX_PKT_SIZE                      CSR_BT_MAX_OBEX_SIGNAL_LENGTH
/*--------------------------------------------------------------------------
 * Defines for Synergy BT security
 *--------------------------------------------------------------------------*/

/* Set the default sec level to mode 4 (Only applicable for CSR_BT_BLUETOOTH_VERSION_2P1) */
#if (CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1)
#ifndef CSR_BT_DEFAULT_SEC_MODE
#define CSR_BT_DEFAULT_SEC_MODE                                        SEC_MODE4_SSP
#endif /* CSR_BT_DEFAULT_SEC_MODE */
#else
#ifndef CSR_BT_DEFAULT_SEC_MODE
#define CSR_BT_DEFAULT_SEC_MODE                                        SEC_MODE2_SERVICE
#endif /* CSR_BT_DEFAULT_SEC_MODE */
#endif

/* Set the default encryption mode for security mode 3. Please note
 * that this value is only used for security mode 3.  It can any one
 * of:
 * - HCI_ENC_MODE_OFF
 * - HCI_ENC_MODE_PT_TO_PT
 * - HCI_ENC_MODE_PT_TO_PT_AND_BCAST */
#ifndef CSR_BT_DEFAULT_ENC_MODE3
#define CSR_BT_DEFAULT_ENC_MODE3                                       (HCI_ENC_MODE_PT_TO_PT)
#endif /* CSR_BT_DEFAULT_ENC_MODE3 */

/* The default application layer above the SC profile manager. Used if
 * no other handle is given when the function is activated */
#ifndef CSR_BT_APPLICATION_SECURITY_HANDLER
#define CSR_BT_APPLICATION_SECURITY_HANDLER                            TESTQUEUE
#endif /* CSR_BT_APPLICATION_SECURITY_HANDLER */

/* Set the time window in which the user has to respond to a
 * CSR_BT_SC_PASSKEY_IND, if the user fails to respond within this time
 * the SC will automatically respond the passkey indication with
 * error. The time is given in mSec and must not be higher than
 * 29000 mSec. Default 28000 mSec. */
#ifndef CSR_BT_SC_PASSKEY_RESPONSE_TIMEOUT
#define CSR_BT_SC_PASSKEY_RESPONSE_TIMEOUT                             28000
#endif /* CSR_BT_SC_PASSKEY_RESPONSE_TIMEOUT */

/* Set default event mask */
#ifndef CSR_BT_SC_DEFAULT_EVENT_MASK
#define CSR_BT_SC_DEFAULT_EVENT_MASK                                   (CSR_BT_SC_EVENT_MASK_NONE)
#endif /* CSR_BT_SC_DEFAULT_EVENT_MASK */

/* Set default IO capability to a fully functional device */
#ifndef CSR_BT_DEFAULT_IO_CAPABILITY
#define CSR_BT_DEFAULT_IO_CAPABILITY                                   (HCI_IO_CAP_KEYBOARD_DISPLAY)
#endif /* CSR_BT_DEFAULT_IO_CAPABILITY */

/* Set default authentication requirements */
#ifndef CSR_BT_DEFAULT_AUTH_REQUIREMENTS
#define CSR_BT_DEFAULT_AUTH_REQUIREMENTS                               (HCI_MITM_NOT_REQUIRED_GENERAL_BONDING)
#endif /* CSR_BT_DEFAULT_AUTH_REQUIREMENTS */

/* Set default authentication requirements */
#ifndef CSR_LE_DEFAULT_AUTH_REQUIREMENTS
#define CSR_LE_DEFAULT_AUTH_REQUIREMENTS                               (DM_SM_SECURITY_UNAUTHENTICATED_BONDING)
#endif /* CSR_LE_DEFAULT_AUTH_REQUIREMENTS */

/* Set Simple Pairing Debug Mode */
#ifndef CSR_BT_SC_ALLOW_SSP_DEBUG_MODE
#define CSR_BT_SC_ALLOW_SSP_DEBUG_MODE                                 (0)
#endif /* CSR_BT_SC_ALLOW_SSP_DEBUG_MODE */

/* Set default SC mode */
#ifndef CSR_BT_DEFAULT_SC_MODE
#define CSR_BT_DEFAULT_SC_MODE                                         (CSR_BT_SEC_MODE_PAIRABLE)
#endif /* CSR_BT_DEFAULT_SC_MODE */

#ifndef CSR_BT__DEFAULT_INCOMING_SECURITY_
#define CSR_BT__DEFAULT_INCOMING_SECURITY_                             (CSR_BT_SEC_SPECIFY | \
                                                                 CSR_BT_SEC_AUTHENTICATION |\
                                                                 CSR_BT_SEC_ENCRYPTION     |\
                                                                 CSR_BT_SEC_AUTHORISATION)
#endif /* CSR_BT__DEFAULT_INCOMING_SECURITY_ */

#ifndef CSR_BT__DEFAULT_OUTGOING_SECURITY_
#define CSR_BT__DEFAULT_OUTGOING_SECURITY_                             (CSR_BT_SEC_SPECIFY | \
                                                                 CSR_BT_SEC_AUTHENTICATION |\
                                                                 CSR_BT_SEC_ENCRYPTION)
#endif /* CSR_BT__DEFAULT_OUTGOING_SECURITY_ */

/* Security levels for each profile, incoming connections */
#ifndef CSR_BT_DUN_GW_DEFAULT_SECURITY_INCOMING
#define CSR_BT_DUN_GW_DEFAULT_SECURITY_INCOMING                        (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_DUN_GW_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_HANDSFREE_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HANDSFREE_DEFAULT_SECURITY_INCOMING                     (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HANDSFREE_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_INCOMING                  (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_IMAGING_RESPONDER_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_IMAGING_RESPONDER_DEFAULT_SECURITY_INCOMING        (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_IMAGING_RESPONDER_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_INCOMING              (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_INCOMING               (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_INCOMING           (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_INCOMING            (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_INCOMING
#define CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_INCOMING                    (CSR_BT_SEC_SPECIFY        |\
                                                                        CSR_BT_SEC_AUTHENTICATION |\
                                                                        CSR_BT_SEC_ENCRYPTION     |\
                                                                        CSR_BT_SEC_AUTHORISATION  |\
                                                                        CSR_BT_SEC_MITM)
#endif /* CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_INCOMING
#define CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_INCOMING                   (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_PBAP_DEFAULT_SECURITY_INCOMING
#define CSR_BT_PBAP_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_PBAP_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_BPPS_DEFAULT_SECURITY_INCOMING
#define CSR_BT_BPPS_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_BPPS_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_AV_DEFAULT_SECURITY_INCOMING
#define CSR_BT_AV_DEFAULT_SECURITY_INCOMING                            (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_AV_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_AV_RCP_DEFAULT_SECURITY_INCOMING
#define CSR_BT_AV_RCP_DEFAULT_SECURITY_INCOMING                        (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_AV_RCP_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_HIDH_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HIDH_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HIDH_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_HIDD_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HIDD_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HIDD_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_PAN_DEFAULT_SECURITY_INCOMING
#define CSR_BT_PAN_DEFAULT_SECURITY_INCOMING                           (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_PAN_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_HCRP_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HCRP_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HCRP_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_INCOMING
#define CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_INCOMING          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_MCAP_DEFAULT_SECURITY_INCOMING
#define CSR_BT_MCAP_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_MCAP_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_HDP_DEFAULT_SECURITY_INCOMING
#define CSR_BT_HDP_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_HDP_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_GATT_DEFAULT_SECURITY_INCOMING
#define CSR_BT_GATT_DEFAULT_SECURITY_INCOMING                          (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_GATT_DEFAULT_SECURITY_INCOMING */

#ifndef CSR_BT_GNSS_SERVER_DEFAULT_SECURITY_INCOMING
#define CSR_BT_GNSS_SERVER_DEFAULT_SECURITY_INCOMING                   (CSR_BT__DEFAULT_INCOMING_SECURITY_)
#endif /* CSR_BT_GNSS_SERVER_DEFAULT_SECURITY_INCOMING */


/* Security levels for each profile, outgoing connections */
#ifndef CSR_BT_HANDSFREE_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_HANDSFREE_DEFAULT_SECURITY_OUTGOING                     (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_HANDSFREE_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_OUTGOING                  (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_HANDSFREE_GW_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_OUTGOING            (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_FILE_TRANSFER_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_OUTGOING               (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_IR_MC_SYNC_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_OUTGOING           (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_MESSAGE_ACCESS_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_OUTGOING              (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_OBJECT_PUSH_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_IMAGING_INITIATOR_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_IMAGING_INITIATOR_DEFAULT_SECURITY_OUTGOING        (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_IMAGING_INITIATOR_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_PRINTING_INITIATOR_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_PRINTING_INITIATOR_DEFAULT_SECURITY_OUTGOING       (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_PRINTING_INITIATOR_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_OUTGOING                    (CSR_BT_SEC_SPECIFY        |\
                                                                        CSR_BT_SEC_AUTHENTICATION |\
                                                                        CSR_BT_SEC_ENCRYPTION     |\
                                                                        CSR_BT_SEC_MITM)
#endif /* CSR_BT_SIM_ACCESS_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_OUTGOING                   (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_SERIAL_PORT_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_AV_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_AV_DEFAULT_SECURITY_OUTGOING                            (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_AV_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_AV_RCP_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_AV_RCP_DEFAULT_SECURITY_OUTGOING                        (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_AV_RCP_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_HIDH_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_HIDH_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_HIDH_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_HIDD_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_HIDD_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_HIDD_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_PAN_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_PAN_DEFAULT_SECURITY_OUTGOING                           (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_PAN_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_PBAP_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_PBAP_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_PBAP_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_DUN_CLIENT_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_DUN_CLIENT_DEFAULT_SECURITY_OUTGOING                    (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_DUN_CLIENT_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_OUTGOING          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_OBEX_SYNCML_TRANSFER_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_MCAP_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_MCAP_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_MCAP_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_HDP_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_HDP_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_HDP_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_GATT_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_GATT_DEFAULT_SECURITY_OUTGOING                          (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_GATT_DEFAULT_SECURITY_OUTGOING */

#ifndef CSR_BT_GNSS_CLIENT_DEFAULT_SECURITY_OUTGOING
#define CSR_BT_GNSS_CLIENT_DEFAULT_SECURITY_OUTGOING                  (CSR_BT__DEFAULT_OUTGOING_SECURITY_)
#endif /* CSR_BT_GNSS_CLIENT_DEFAULT_SECURITY_OUTGOING */

/*--------------------------------------------------------------------------
 * Default key distribution parameters for Low Energy security
 *--------------------------------------------------------------------------*/
/* What low energy encryption keys shall be exchanged by default? */
#define CSR_BT_SC_KEY_DIST_DEFAULT                                     (DM_SM_KEY_DIST_INITIATOR_ENC_CENTRAL\
                                                                        | DM_SM_KEY_DIST_INITIATOR_SIGN\
                                                                        | DM_SM_KEY_DIST_INITIATOR_ID\
                                                                        | DM_SM_KEY_DIST_RESPONDER_ENC_CENTRAL\
                                                                        | DM_SM_KEY_DIST_RESPONDER_SIGN\
                                                                        | DM_SM_KEY_DIST_RESPONDER_ID)

/* The default low energy authentication requirement for responder */
#define CSR_BT_SC_AUTH_REQ_DEFAULT                                     (CSR_BT_SC_LE_SECURITY_ENCRYPTION)

/*--------------------------------------------------------------------------
 * Defines for Device Manager (DM)
 *--------------------------------------------------------------------------*/

/* Encryption retry timer. Allows the LM enough time to recover
 between encryption requests. */
#ifndef DM_CONFIG_SM_REENCRYPTION_TIMEOUT
#define DM_CONFIG_SM_REENCRYPTION_TIMEOUT                   (2)
#endif

/* Time in quarters (1/4) of a second between bonding completing successfully
 and the ACL being closed. If bonding fails or is cancelled then the ACL
 always closes immediately. This will not work in conjunction with
 powerstate tables and so should only be used by Synergy BT.

 The example here is for a 2 second timeout after bonding finishes.

#define DM_CONFIG_ACL_IDLE_TIMEOUT_BONDING                  ((CsrUint32) ((2*CSR_SCHED_SECOND)/(250*CSR_SCHED_MILLISECOND)))
*/
#define DM_CONFIG_ACL_IDLE_TIMEOUT_BONDING                  (0)

/* Mode change holdoff times after a failed locally-initiated mode
 change request and after a remotely-initiated mode change. */
#ifndef DM_CONFIG_MODE_CHANGE_FAILED_HOLDOFF_TIME
#define DM_CONFIG_MODE_CHANGE_FAILED_HOLDOFF_TIME           (1 * CSR_SCHED_SECOND)
#endif

#ifndef DM_CONFIG_MODE_CHANGE_REMOTE_HOLDOFF_TIME
#define DM_CONFIG_MODE_CHANGE_REMOTE_HOLDOFF_TIME           (5 * CSR_SCHED_SECOND)
#endif

/* Maximum size of data packets allowed from the controller in bytes */
#define DM_CONFIG_HOST_ACL_DATA_PACKET_LENGTH               ((CsrUint16)1021)


/*--------------------------------------------------------------------------
 * Defines for Audio Codec support (HFG/HF)
 *--------------------------------------------------------------------------*/
/* Supported codec mask
 * CVSD               : 0x01
 * ADPCM 2bit/sample  : 0x02
 * ADPCM 4bit/sample  : 0x04
*/
#ifndef CSR_BT_AUDIO_CODEC_SUPPORT_MASK
#define CSR_BT_AUDIO_CODEC_SUPPORT_MASK                            (0x01)
#endif /* CSR_BT_AUDIO_CODEC_SUPPORT_MASK */

/* custom AT mode, default settings */
#define CSR_BT_HFG_AT_CUSTOM_BYTE_0                                 (0x1D)
#define CSR_BT_HFG_AT_CUSTOM_BYTE_1                                 (0xFC)
#define CSR_BT_HFG_AT_CUSTOM_BYTE_2                                 (0x01)
#define CSR_BT_HFG_AT_CUSTOM_BYTE_3                                 (0xFF)
#define CSR_BT_HFG_AT_CUSTOM_BYTE_4                                 (0xFD)
#define CSR_BT_HFG_AT_CUSTOM_UNUSED                                 (0xFF)

#ifndef CSR_BT_HFG_CONNECTION_GUARD_TIME
#define CSR_BT_HFG_CONNECTION_GUARD_TIME                            (30 * CSR_SCHED_SECOND)
#endif /* CSR_BT_HFG_CONNECTION_GUARD_TIME */

/* APT should be less than 10 sec as per HF 1.7 spec and is multiple of 10ms,
   This value can be configured from 1 - 1000, by default (10 * 100) (10 secs) */
#ifndef CSR_BT_HFP_AUTHENTICATED_PAYLOAD_TIMEOUT
#define CSR_BT_HFP_AUTHENTICATED_PAYLOAD_TIMEOUT        ((CsrUint16) (10 * 100))
#endif /* CSR_BT_HFP_AUTHENTICATED_PAYLOAD_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for PBAP (PAC/PAS)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_PAC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_PAC_LP_SUPERVISION_TIMEOUT        (2000) /* Number of milliseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_PAC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_PAS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_PAS_LP_SUPERVISION_TIMEOUT        (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_PAS_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_PAC_SUPPORTED_FEATURES
#define CSR_BT_PAC_SUPPORTED_FEATURES            (CSR_BT_PB_FEATURE_ALL) /* Feautures supported by PBAP Client */
#endif /* CSR_BT_PAC_SUPPORTED_FEATURES */

/*--------------------------------------------------------------------------
 * Defines for FTP (FTS/FTC)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_FTC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_FTC_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_FTC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_FTS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_FTS_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_FTS_LP_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for OPP (OPS/OPC)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_OPC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_OPC_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_OPC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_OPS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_OPS_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_OPS_LP_SUPERVISION_TIMEOUT */


/*--------------------------------------------------------------------------
 * Defines for MAP (MAPC/MAPS)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_MAPC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_MAPC_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_MAPC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_MAPC_SUPPORTED_FEATURES
#define CSR_BT_MAPC_SUPPORTED_FEATURES           (CSR_BT_MAP_SUPPORTED_FEATURE_ALL & ((CsrBtMapSupportedFeatures) ~CSR_BT_MAP_SF_SUPP_FEATURES_IN_CONNECT_REQ))
#endif /* CSR_BT_MAPC_SUPPORTED_FEATURES */

#ifndef CSR_BT_MAPS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_MAPS_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_MAPS_LP_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for OBEX SYNC (SYNCC/SYNCS)
 *--------------------------------------------------------------------------*/

#ifndef CSR_BT_SYNCC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_SYNCC_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_SYNCC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_SYNCS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_SYNCS_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_SYNCS_LP_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for OBEX SyncML (SMLC/SMLS)
 *--------------------------------------------------------------------------*/

#ifndef CSR_BT_SMLC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_SMLC_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_SMLC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_SMLS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_SMLS_LP_SUPERVISION_TIMEOUT      (4000000) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_SMLS_LP_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for BIP (BIPC/BIPS)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_BIPC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_BIPC_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_BIPC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_BIPS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_BIPS_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_BIPS_LP_SUPERVISION_TIMEOUT */

/*--------------------------------------------------------------------------
 * Defines for BPP (BPPC/BPPS)
 *--------------------------------------------------------------------------*/
#ifndef CSR_BT_BPPC_LP_SUPERVISION_TIMEOUT
#define CSR_BT_BPPC_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_BPPC_LP_SUPERVISION_TIMEOUT */

#ifndef CSR_BT_BPPS_LP_SUPERVISION_TIMEOUT
#define CSR_BT_BPPS_LP_SUPERVISION_TIMEOUT      (CSR_BT_INFINITE_TIME) /* Number of microseconds without data before entering sniff (if possible) */
#endif /* CSR_BT_BPPS_LP_SUPERVISION_TIMEOUT */

/*---------------------------------------------------------------------------
* Defines for HOGH
*----------------------------------------------------------------------------*/
#define CSR_BT_HOGH_MAX_CONNECTIONS                         (8)     /* Range: 1-254 */

/* Default Scan parameters */
#define CSR_BT_HOGH_DEFAULT_SCAN_INTERVAL                   (CSR_BT_LE_DEFAULT_SCAN_INTERVAL)
#define CSR_BT_HOGH_DEFAULT_SCAN_WINDOW                     (CSR_BT_LE_DEFAULT_SCAN_WINDOW)

/* Default connection parameters */
#define CSR_BT_HOGH_DEFAULT_CONN_INTERVAL_MIN               (CSR_BT_LE_DEFAULT_CONN_INTERVAL_MIN)
#define CSR_BT_HOGH_DEFAULT_CONN_INTERVAL_MAX               (CSR_BT_LE_DEFAULT_CONN_INTERVAL_MAX)
#define CSR_BT_HOGH_DEFAULT_CONN_LATENCY                    (CSR_BT_LE_DEFAULT_CONN_LATENCY)
#define CSR_BT_HOGH_DEFAULT_CONN_SUPERVISION_TIMEOUT        (CSR_BT_LE_DEFAULT_CONN_SUPERVISION_TIMEOUT)

/* Aggressive connection parameters used during service discovery and configuration */
#define CSR_BT_HOGH_DEFAULT_SCAN_INTERVAL_FAST               (CSR_BT_LE_SCAN_INTERVAL_FAST)
#define CSR_BT_HOGH_DEFAULT_SCAN_WINDOW_FAST                 (CSR_BT_LE_SCAN_WINDOW_FAST)
#define CSR_BT_HOGH_DEFAULT_CONNECTION_INTERVAL_MIN_FAST     (CSR_BT_LE_CONNECTION_INTERVAL_MIN_FAST)
#define CSR_BT_HOGH_DEFAULT_CONNECTION_INTERVAL_MAX_FAST     (CSR_BT_LE_CONNECTION_INTERVAL_MAX_FAST)
#define CSR_BT_HOGH_DEFAULT_CONNECTION_LATENCY_FAST          (CSR_BT_LE_CONNECTION_LATENCY_FAST)
#define CSR_BT_HOGH_DEFAULT_SUPERVISION_TIMEOUT_FAST         (CSR_BT_LE_SUPERVISION_TIMEOUT_FAST)

#define CSR_BT_HOGH_DEFAULT_IDLE_TIMEOUT                    (0xFFFF) /* Never disconnect */
#define CSR_BT_HOGH_DEFAULT_RECONNECT_TIMEOUT               (0x0000) /* Connect immediately */

/* Default HOGH security requirements */
#define CSR_BT_HOGH_DEFAULT_SECURITY_REQUIREMENTS           (CSR_BT_GATT_SECURITY_FLAGS_UNAUTHENTICATED)

/*---------------------------------------------------------------------------
* Defines for AT config
*----------------------------------------------------------------------------*/
#define CSR_BT_MAX_NUMBER_OF_TOKEN_IN_ONE_COMMAND          (20)   /*    Valid range is between 1 and 255 */
#define CSR_BT_MAX_DIAL_STRING_LENGTH                      (50)   /*    Valid range is between 1 and 255 */
#define CSR_BT_MAX_UNKNOWN_EXTENDED_COMMAND_LENGTH         (10)   /*    Valid range is between 1 and 255 */
#define CSR_BT_MAX_NUMBER_OF_UNKNOWN_EXTENDED_COMMAND      (5)    /*    Valid range is between 1 and 255 */
#define CSR_BT_RETURN_TO_ONLINE_DATA_STATE                 (0)    /*    (O parameter):    Valid range is between 0-255. */
#define CSR_BT_SET_TO_FACTORY_DEFINED_CONFIGURATION        (0)    /*    (&F parameter): Valid range is between 0-255. */
#define CSR_BT_RESET_TO_DEFAULT_CONFIGURATION              (0)    /*    (Z parameter):    Valid range is between 0-255 */


/************************************************************************************************************

  If the specified signal is defined it is send to application, else the AT module takes
    care of it and the signal is NOT send to the application

*************************************************************************************************************/
#ifdef BUILD_FROM_COMMAND_LINE
/*    CompleteCapabilitiesList */
#define CSR_BT_AT_SEND_GCAP_TOKEN_IND
/*    ManufacturerIdentification */
#define CSR_BT_AT_SEND_GMI_TOKEN_IND
/*    ModelIdentification */
#define CSR_BT_AT_SEND_GMM_TOKEN_IND
/*    RevisionIdentification */
#define CSR_BT_AT_SEND_GMR_TOKEN_IND
/*    resultCodeSuppression */
#define CSR_BT_AT_SEND_Q_TOKEN_IND
/*    monitorSpeakerLoudness */
#define CSR_BT_AT_SEND_L_TOKEN_IND
/*    monitorSpeakerMode */
#define CSR_BT_AT_SEND_M_TOKEN_IND
/*    dceResponseFormat */
#define CSR_BT_AT_SEND_V_TOKEN_IND
/*    commandEcho */
#define CSR_BT_AT_SEND_E_TOKEN_IND
/*    selectPulseDialing */
#define CSR_BT_AT_SEND_P_TOKEN_IND
/*    selectToneDialing */
#define CSR_BT_AT_SEND_T_TOKEN_IND
/*    automaticAnswer and readAutomaticAnswerValue */
#define CSR_BT_AT_SEND_S0_TOKEN_IND
/*    cmdLineTerminationCharacter and readCmdTerminationValue */
#define CSR_BT_AT_SEND_S3_TOKEN_IND
/*    responseFormattingCharacter     and readResponseFormattingValue */
#define CSR_BT_AT_SEND_S4_TOKEN_IND
/*    cmdLineEditingCharacter    and readCmdLineEditingValue */
#define CSR_BT_AT_SEND_S5_TOKEN_IND
/*    pauseBeforeBlindDialing and readPauseBeforeBlindDialingValue */
#define CSR_BT_AT_SEND_S6_TOKEN_IND
/*    ConnectionCompletionTimeout and readConnectionCompleteTimeoutValue */
#define CSR_BT_AT_SEND_S7_TOKEN_IND
/*    commaDialModifierTime and readCommaDialModifierTimeValue */
#define CSR_BT_AT_SEND_S8_TOKEN_IND
/*    automaticDisconnectDelay and readAutomaticDisconnectValue */
#define CSR_BT_AT_SEND_S10_TOKEN_IND
#endif

#ifndef CSR_BT_AT_SEND_GCAP_TOKEN_IND
#define CSR_BT_AT_GCAP_TEXT    "CSR"
#endif

#ifndef CSR_BT_AT_SEND_GMI_TOKEN_IND
#define CSR_BT_AT_GMI_TEXT     "CSR"
#endif

#ifndef CSR_BT_AT_SEND_GMM_TOKEN_IND
#define CSR_BT_AT_GMM_TEXT     "CSR"
#endif

#ifndef CSR_BT_AT_SEND_GMR_TOKEN_IND
#define CSR_BT_AT_GMR_TEXT     "CSR"
#endif

#ifndef CSR_BT_AT_SEND_S0_TOKEN_IND
#define CSR_BT_AT_S0_DEFAULT_VALUE        0
#endif

#ifndef CSR_BT_AT_SEND_S6_TOKEN_IND
#define CSR_BT_AT_S6_DEFAULT_VALUE        2
#endif

#ifndef CSR_BT_AT_SEND_S7_TOKEN_IND
#define CSR_BT_AT_S7_DEFAULT_VALUE        50
#endif

#ifndef CSR_BT_AT_SEND_S8_TOKEN_IND
#define CSR_BT_AT_S8_DEFAULT_VALUE        2
#endif

#ifndef CSR_BT_AT_SEND_S10_TOKEN_IND
#define CSR_BT_AT_S10_DEFAULT_VALUE        20
#endif

#ifndef CSR_BT_GATT_ADV_DATA_APPEARANCE
#define CSR_BT_GATT_ADV_DATA_APPEARANCE        6
#endif

/* For QC Chips since Fake NOP would always be emitted by TM module so 
   this timer is not necessary. At the same this needs a value to be 
   defined so keep it big enough so this never expires */
#ifdef CSR_USE_QCA_CHIP
#ifndef DM_CONFIG_POWERUP_TIMEOUT
#define DM_CONFIG_POWERUP_TIMEOUT          (300 * CSR_SCHED_SECOND)
#endif
#endif

#include "csr_bt_bluestack_config.h"


#ifdef __cplusplus
}
#endif

#endif

