/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_WLAN_BASE_H__
#define __QAPI_WLAN_BASE_H__

/**
@file qapi_wlan_base.h
This section provides APIs, macros definitions, enumerations and data structures
for applications to perform WLAN control operations.
*/

#include <stdint.h>
#include "qapi_status.h"
#include "qapi_wlan_errors.h"

/** @addtogroup qapi_wlan
@{ */

/** Name of first WLAN virtual device, also known as Device 0. */
#define __QAPI_WLAN_DEVICE0_NAME  "wlan0"
/**
Name of second WLAN virtual device, also known as Device 1; currently the
maximum number of supported virtual devices is 2 (Device 0 and Device 1).
*/
#define __QAPI_WLAN_DEVICE1_NAME  "wlan1"

/** Size of the WLAN MAC address in bytes. */
#define __QAPI_WLAN_MAC_LEN                       6
/** Maximum size of the WLAN Service Set IDentifier (SSID) in bytes. */
#define __QAPI_WLAN_MAX_SSID_LEN                 32
/** Maximum SSID length (includes ending NULL character) in bytes. */
#define __QAPI_WLAN_MAX_SSID_LENGTH          (32+1)
/** Maximum passphrase length for WPA/WPA2 WLAN networks in bytes. */
#define __QAPI_WLAN_PASSPHRASE_LEN               64
/** Maximum number of profiles supported for preferred network offload operation. */
#define __QAPI_WLAN_PNO_MAX_NETWORK_PROFILES      5
/** Size of an IPv6 address in bytes. */
#define __QAPI_WLAN_IPV6_ADDR_LEN                16  
/** Size of an IPv4 address in bytes. */
#define __QAPI_WLAN_IPV4_ADDR_LEN                 4
/**
Maximum length of a Wi-Fi Protected Setup (WPS) PIN.
This length includes the terminating NULL character.
*/
#define __QAPI_WLAN_WPS_PIN_LEN                   9
/** Maximum size of a WPS key in bytes. */
#define __QAPI_WLAN_WPS_MAX_KEY_LEN              64

/** Maximum number of filters allowed in Promiscuous mode. */
#define __QAPI_WLAN_PROMISC_MAX_FILTER_IDX        3

/** Maximum number of supported channels that can be retrieved from tbe current regulatory domain channel list. */
#define __QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS     64

/** 
Allows the application to enable/disable a source MAC address based filter when 
operating in Promiscuous mode. 

This macro should be set in the filter_flags field 
of the qapi_WLAN_Promiscuous_Mode_Info_t data structure when issuing 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE.\n
If this bit is enabled, the application should also set the src_Mac address in the 
corresponding filter index of qapi_WLAN_Promiscuous_Mode_Info_t.

@sa
qapi_WLAN_Promiscuous_Mode_Info_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE\n
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PROM_FILTER_SOURCE         0x01

/** 
Allows the application to enable/disable the destination MAC address based filter when 
operating in Promiscuous mode. 

This macro should be set in the filter_flags field 
of the qapi_WLAN_Promiscuous_Mode_Info_t data structure when issuing 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE.\n
If this bit is enabled, the application should also set the dst_Mac address in the 
corresponding filter index of qapi_WLAN_Promiscuous_Mode_Info_t.

@sa
qapi_WLAN_Promiscuous_Mode_Info_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE\n
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PROM_FILTER_DEST           0x02

/** 
Allows the application to enable/disable the 802.11 packet type based filter when 
operating in Promiscuous mode. 

This macro should be set in the filter_flags field of the qapi_WLAN_Promiscuous_Mode_Info_t 
data structure when issuing qapi_WLAN_Set_Param() with 
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE.\n
The 802.11 packet type to be matched should be provided in the promisc_frametype field in the corresponding 
filter index of qapi_WLAN_Promiscuous_Mode_Info_t.

@sa
qapi_WLAN_Promiscuous_Mode_Info_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE\n
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PROM_FILTER_FRAME_TYPE     0x04

/** 
Allows the application to enable/disable the 802.11 packet subtype based filter when 
operating in Promiscuous mode. 

This macro should be set in the filter_flags field of the qapi_WLAN_Promiscuous_Mode_Info_t 
data structure when issuing qapi_WLAN_Set_Param() with 
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE.\n
The 802.11 packet type to be matched should be provided in the promisc_subtype field in the corresponding 
filter index of qapi_WLAN_Promiscuous_Mode_Info_t.

@sa
qapi_WLAN_Promiscuous_Mode_Info_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE\n
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PROM_FILTER_FRAME_SUB_TYPE 0x08

/**
This macro indicates that the data is RSSI information for a frame in promiscuous mode.
When the driver layer reports RSSI information for a frame, this macro is set and the application
should treat the data as RSSI information, not a frame context.
*/
#define __QAPI_WLAN_PROMISC_REPORT_RSSI_INFO   0x4000

/**
This macro indicates that the data is only payload of frame which which has no 802.11 mac header.
For AMSDU packet, it has only one wlan 802.11 mac header and may occupy multiple netbufs so the
driver layer may report several packets to application layer. 
If this macro is set, it means the data is only payload and should append to the previous frame.
*/
#define __QAPI_WLAN_PROMISC_INDICATE_APPEND_PAYLOAD 0x8000

/** @cond EXPORT_PKTLOG */
/** 
Default number of pktlog buffers used to allocate in a target 
when pktlog was enabled if the user did not specify it. 
*/
#define __QAPI_WLAN_PKTLOG_NUM_BUFF                  4
/** 
Maximum number of pktlog buffers that the user can set to allocate buffers 
in a target when pktlog is enabled. 
*/
#define __QAPI_WLAN_MAX_PKTLOG_NUM_BUFF              10
/** @endcond */

/** Total number of Dblog modules supported in the target. */
#define __QAPI_WLAN_DBGLOG_MAX_MODULE_ID             64
/** 
Number of word length buffers required to store the DBGLOG Loglevel 
information for all 64 (maximum supported) modules. 
*/
#define __QAPI_WLAN_DBGLOG_LOGLEVEL_INFO_LEN         8
/** 
Module mask that can be used to enable all modules with a 
desired loglevel.
*/
#define __QAPI_WLAN_DBGLOG_DEFAULT_MODULE_MASK       0xFFFFFFFFFFFFFFFFLL
/** 
Global module ID to set the same configuration(loglevel) 
for all modulues. 
*/
#define __QAPI_WLAN_DBGLOG_GLOBAL_MODULE_ID          0xFF
/** 
Default value to capture only the ERROR loglevel for all 64 modules 
when dbglog is enabled. 
*/
#define __QAPI_WLAN_DBGLOG_DEFAULT_MODULE_LOG_LEVEL  0x88888888
/** 
Used to configure to send the dbglogs on the local UART on 
the target chip (Kingfisher). 
*/
#define __QAPI_WLAN_DBGLOG_LOCAL_DBG_PORT            0
/** 
Used to configure to send the dbglogs from the target to the host, which 
in turn sends them to its UART, where an external tool captures and parses the 
dbglog binary logs. 
*/
#define __QAPI_WLAN_DBGLOG_REMOTE_DBG_PORT           1
/** 
Flag bit to be used to notify the target when a change in loglevel is needed
during a dbglog configuration update. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_LOG_LEVEL_FLAG_MASK         0x01
/** 
Flag bit to be used to notify the target when a change in debug port is needed
to use during a dbglog configuration update. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_DEBUG_PORT_FLAG_MASK        0x02
/** 
Flag bit to be used to notify the target when a change in reporting is
required or not during a dbglog configuration update. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_REPORT_FLAG_MASK            0x04
/** 
Flag bit to be used to notify the target when a change in 
report size is needed during a dbglog configuration update. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_REPORT_SIZE_FLAG_MASK       0x08
/** 
Flag bit to be used to notify the target when a change in 
time resolution is needed during a dbglog configuration update. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_TIMER_RESOLUTION_FLAG_MASK  0x10
/** 
Flag bit to be used to notify the target to flush log buffer. 
*/
#define __QAPI_WLAN_DBGLOG_CFG_FLUSH_FLAG_MASK              0x20
/** 
Value for the number of nibbles in a word. Used for dbglog loglevel 
update calculation. 
*/
#define __QAPI_WLAN_DBGLOG_NIBBLE_CNT_IN_WORD_MEMORY     8
/** 
Value for the number of bits in a word memory. Used for dbglog loglevel
update calculation. 
*/
#define __QAPI_WLAN_DBGLOG_BIT_CNT_IN_WORD_MEMORY        32

/** IEEE 802.11 channel 1 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_1            (2412)
/** IEEE 802.11 channel 2 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_2            (2417)
/** IEEE 802.11 channel 3 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_3            (2422)
/** IEEE 802.11 channel 4 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_4            (2427)
/** IEEE 802.11 channel 5 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_5            (2432)
/** IEEE 802.11 channel 6 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_6            (2437)
/** IEEE 802.11 channel 7 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_7            (2442)
/** IEEE 802.11 channel 8 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_8            (2447)
/** IEEE 802.11 channel 9 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_9            (2452)
/** IEEE 802.11 channel 10 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_10           (2457)
/** IEEE 802.11 channel 11 in MHz. This channel is allowed in USA, Canada, Europe, and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_11           (2462)
/** IEEE 802.11 channel 12 in MHz. This channel is allowed in Europe and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_12           (2467)
/** IEEE 802.11 channel 13 in MHz. This channel is allowed in Europe and Japan. */
#define __QAPI_WLAN_CHAN_FREQ_13           (2472)
/** IEEE 802.11 channel 14 in MHz. This channel is allowed in Japan. */
#define __QAPI_WLAN_CHAN_FREQ_14           (2484)
/** IEEE 802.11a channel 36 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_36           (5180)
/** IEEE 802.11a channel 40 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_40           (5200)
/** IEEE 802.11a channel 44 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_44           (5220)
/** IEEE 802.11a channel 48 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_48           (5240)
/** IEEE 802.11a channel 52 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_52           (5260)
/** IEEE 802.11a channel 56 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_56           (5280)
/** IEEE 802.11a channel 60 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_60           (5300)
/** IEEE 802.11a channel 64 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_64           (5320)
/** IEEE 802.11a channel 100 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_100          (5500)
/** IEEE 802.11a channel 104 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_104          (5520)
/** IEEE 802.11a channel 108 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_108          (5540)
/** IEEE 802.11a channel 112 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_112          (5560)
/** IEEE 802.11a channel 116 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_116          (5580)
/** IEEE 802.11a channel 132 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_132          (5660)
/** IEEE 802.11a channel 136 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_136          (5680)
/** IEEE 802.11a channel 140 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_140          (5700)
/** IEEE 802.11a channel 149 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_149          (5745)
/** IEEE 802.11a channel 153 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_153          (5765)
/** IEEE 802.11a channel 157 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_157          (5785)
/** IEEE 802.11a channel 161 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_161          (5805)
/** IEEE 802.11a channel 165 in MHz. */
#define __QAPI_WLAN_CHAN_FREQ_165          (5825)

/** 
Flag that indicates whether a scanned access point's authentication type is of type 
Pre-Shared Key.
This is indicated in the wpa_auth field (for WPA APs) and rsn_Auth field (for WPA2 APs)
of the qapi_WLAN_BSS_Scan_Info_t structure.
*/
#define __QAPI_WLAN_SECURITY_AUTH_PSK      0x01

/** 
Flag that indicates whether the scanned access point's authentication type is of type 
802.1x(EAP based).
This is indicated in the wpa_auth field (for WPA APs) and rsn_Auth field (for WPA2 APs)
of the qapi_WLAN_BSS_Scan_Info_t structure.
*/
#define __QAPI_WLAN_SECURITY_AUTH_1X       0x02

/** @cond */
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_MODE_NORMAL    (0x00000002)
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_MODE_BMI       (0x00000003)
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_MODE_MASK      (0x0000000f)
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_QUAD_SPI_FLASH (0x80000000)
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_RAWMODE_BOOT   (0x00000010 | __QAPI_WLAN_AR4XXX_PARAM_MODE_NORMAL)
/** Boot parameter. */
#define __QAPI_WLAN_AR4XXX_PARAM_MACPROG_MODE   (0x00000020 | __QAPI_WLAN_AR4XXX_PARAM_MODE_NORMAL)
/** Combined boot parameter for common cases. */
#define __QAPI_WLAN_AR4XXX_PARAM_RAW_QUAD       (__QAPI_WLAN_AR4XXX_PARAM_RAWMODE_BOOT | __QAPI_WLAN_AR4XXX_PARAM_QUAD_SPI_FLASH)
/** Default regulatory domain. This settings can be passed to WLAN firmware during boot. */
#define __QAPI_WLAN_AR4XXX_PARAM_REG_DOMAIN_DEFAULT  (0x00000000)    
/** @endcond */

/** 
Flag that indicates whether the scanned access point's encryption type is of type 
TKIP (Temporal Key Integrity Protocol).
This is indicated in the wpa_Cipher field (for WPA APs) and rsn_Cipher field (for WPA2 APs)
of the qapi_WLAN_BSS_Scan_Info_t structure as part of the scan result indication.
*/
#define __QAPI_WLAN_CIPHER_TYPE_TKIP 0x04

/** 
Flag that indicates whether the scanned access point's encryption type is of type CCMP.
This is indicated in the wpa_Cipher field (for WPA APs) and rsn_Cipher field (for WPA2 APs) 
of the qapi_WLAN_BSS_Scan_Info_t structure as part of the scan result indication.
*/
#define __QAPI_WLAN_CIPHER_TYPE_CCMP 0x08

/** 
Flag that indicates whether the scanned access point's encryption type is of type 
WEP (Wired Equivalent Privacy).
This is indicated in the wpa_Cipher field and rsn_Cipher field of the qapi_WLAN_BSS_Scan_Info_t
structure as part of the scan result indication.
*/
#define __QAPI_WLAN_CIPHER_TYPE_WEP  0x02

/** Maximum size of the WEP key. */
#define __QAPI_WLAN_MAX_WEP_KEY_SZ 16

/** 
Maximum number of scan results that the driver buffer can hold when using 
QAPI_WLAN_BUFFER_SCAN_RESULTS_BLOCKING_E and 
QAPI_WLAN_BUFFER_SCAN_RESULTS_NON_BLOCKING_E options to store WLAN scan results.
*/
#define __QAPI_MAX_SCAN_RESULT_ENTRY 12

/**
Macro to indicate the default Short Scan ratio. 
*/
#define __QAPI_WLAN_SHORTSCANRATIO_DEFAULT      3

/** 
Macro that indicates the maximum number of network solicitation entries that the WLAN FW 
can offload.
Every tuple here corresponds to one virtual WLAN device.
*/
#define __QAPI_WLAN_MAX_NS_OFFLOAD_TUPLES           2

/** 
Macro that indicates the maximum number of ARP entries that the WLAN FW can offload.
Every tuple corresponds to one virtual WLAN device.
*/
#define __QAPI_WLAN_MAX_ARP_OFFLOAD_TUPLES          2

/** 
Macro that indicates the maximum number of network solicitation addresses that the WLAN FW 
supports per device (tuple);
one for a global IPv6 address and another for a link local IPv6 address (interchangable).
*/
#define __QAPI_WLAN_NSOFF_MAX_TARGET_IPS            2

/** 
Macro that indicates the maximum number of channels that can be issued in a channel 
list while scanning.
*/
#define __QAPI_WLAN_START_SCAN_PARAMS_CHANNEL_LIST_MAX   12

/** 
Macro that indicates the maximum number of WLAN firmware events that can be filtered 
within the firmware.
*/
#define __QAPI_WLAN_MAX_NUM_FILTERED_EVENTS   64

/** 
Macro that indicates the group ID that can be used to configure system parameters of 
the WLAN subsystem.
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM                            0

/** 
Macro that indicates the group ID that can be used to configure wireless parameters of 
the WLAN subsystem.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS                          1

/** 
Macro that indicates the group ID that can be used to configure security parameters of 
the WLAN subsystem.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY                 2

/** 
Macro that indicates the group ID that can be used to configure various 
P2P (Peer-to-Peer/Wi-Fi Direct) parameters.
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P                               3

/**
An application can enable/disable suspend/resume operations of a WLAN subsystem by 
setting this parameter as TRUE (to enable) or FALSE (to disable).

This setting just enables suspend/resume and does not actually put the WLAN
subsystem in suspend mode.

The application should use qapi_WLAN_Suspend_Start() to put the WLAN subsystem in 
Suspend mode.

By default, this feature is enabled by the WLAN driver. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@sa
qapi_WLAN_Suspend_Start
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_ENABLE_SUSPEND_RESUME      0

/**
Used to enable/disable the target (Kingfisher) debug logging.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DBGLOG_ENABLE              1
/**
Used to configure dbglog configuration, such as debug port, report enable, report size, etc.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DBGLOG_CONFIG              2
/**
Used to configure dbglog module loglevel configuration for a specific module or all modules.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DBGLOG_MODULE_CONFIG       3

/** @cond EXPORT_PKTLOG */
/**
Used to enable/disable the infrastructure required for pktlog.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_PKTLOG_ENABLE              4
/**
Used to start pktlogs with the desired events and options.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_PKTLOG_START               5

/** @endcond */

/**
Command ID to obtain the WLAN firmware version by querying the driver.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param(). 

@param[out] uint32   Variable of type qapi_WLAN_Firmware_Version_String_t, 
                     which will be filled by the driver.

@sa
qapi_WLAN_Firmware_Version_String_t
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_FIRMWARE_VERSION           7

/**
Command ID to control the number of buffers in the WLAN driver 
buffer pool.

This command also allows the application to configure the number of 
buffers to be reserved for Rx in the driver. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] pool_config_info    Application populates the buffer pool configuration 
                               values in qapi_WLAN_A_Netbuf_Pool_Config_t and 
                               passes them to the driver.

@sa
qapi_WLAN_A_Netbuf_Pool_Config_t

@sideeffects
Changing this value can have an impact on WLAN performance.
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DRIVER_NETBUF_POOL_SIZE    8

/**
Used to get the last system error.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_LAST_ERROR                 9

/** @cond EXPORT_DEBUG_APIS */
/**
Used to get driver registration information.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DRIVER_REG_QUERY           10
/**
Used to force set the WLAN firmware assert.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DRIVER_FORCE_ASSERT        11
/**
Used to give sleep info to the WLAN firmware.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_DRIVER_SLEEP_INFO        12

/**
Used to bypass 5G cal section data in OTP and BDF data will be used.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_SYSTEM_BYPASS_5GCAL_OTP           13

/** @endcond */

/**
Command ID to set/get the operating mode of a given virtual device in the WLAN subsystem.

Mode-specific parameters should be set only after setting the mode (AP/STA) using 
this command.

If a concurrent mode of operation is enabled, the SAP can only be operated in virtual
device 0, and the STA mode of operation can only operate in virtual device 1.

In the case of a single device mode of operation, device 0 can be used for both STA and
SAP mode of operation.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] opMode  Address of the variable type qapi_WLAN_Dev_Mode_e

@sa
qapi_WLAN_Dev_Mode_e
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE                       1

/**
Command ID to set/get the operating wireless channel of a given virtual device in 
the WLAN subsystem.

For set/get operations, channel values are set in numbers (channel number 1-14, 36-165)
and not in frequency values.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] uint32_t    Variable that holds the channel number.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CHANNEL                              2

/**
Command ID to set scan parameters to the driver.
These parameters should be set before performing a scan operation.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Scan_Params_t         Address of the variable holding all 
                                           customizable scan parameters.

@sa
qapi_WLAN_Scan_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_SCAN_PARAMS                          3

/**
Command ID to set/get the transmit power in dBm of a given virtual device.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] uint32_t        Address of the variable that holds the power value.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_TX_POWER_IN_DBM                      4

/**
Command ID to set/get the SSID of/for a given virtual device in the WLAN subsystem. 

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] uint8_t[] Unsigned byte array of size __QAPI_WLAN_MAX_SSID_LENGTH.

@sa
__QAPI_WLAN_MAX_SSID_LENGTH
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_SSID                                 5

/**
Command ID to set/get the BSSID of/for a given virtual device in the WLAN subsystem when operation in STA mode. 

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] uint8_t[] Unsigned byte array of size __QAPI_WLAN_MAC_LEN.

@sa
__QAPI_WLAN_MAC_LEN

*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_BSSID                                 6

/**
Command ID to set/get the wireless PHY mode of/for a given virtual device.

The Set operation for this should be done before establishing a connection.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_Phy_Mode_e    Required PHY mode should be specified.

@sa
qapi_WLAN_Phy_Mode_e
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_PHY_MODE                             7

/**
Command ID to enable/disable forwarding of incoming probe request frames when 
operating in Softap mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable forwarding, FALSE to disable.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROBE_REQ_FWD_TO_HOST         8

/**
Command ID to allow/disallow aggregation for Tx and Rx on a TID basis.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Aggregation_Params_t  Strucuture populated with 
                                           required aggregation parameters
                                           for Tx and Rx.

@sa
qapi_WLAN_Aggregation_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ALLOW_TX_RX_AGGR_SET_TID             9

/**
Command ID to enable/disable roaming.
Disabling roaming disables any autonomous scans (like background scans)
performed by the firmware.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to 1 to enable roaming, 3 to disable roaming.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_ROAMING                      10

/**
Command ID to enable/disable Promiscuous mode, which is only supported on virtual device 0.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Promiscuous_Mode_Info_t      Parameters should be set 
                                                  appropriately while enabling.
@sa
qapi_WLAN_Promiscuous_Mode_Info_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE             11

/** @cond */
/** For future use */
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CHIP_LINK_STATE                     12
/** @endcond */

/**
Command ID to set WLAN power mode policy when entering Power Save mode. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Power_Policy_Params_t      Policy parameters populated 
                                                by the application.
@sa
qapi_WLAN_Power_Policy_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_POWER_MODE_POLICY                   13

/**
Command ID to set/get the virtual device power mode.

The supported modes are Power Save mode (also known as REC_POWER) and Performance mode (MAX_PERF).
Applications are recommended to configure virtual devices in Performance mode 
when more than one virtual device is connected (concurrency enabled). 

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_Power_Mode_Params_t         Power mode configurations.

@sa
qapi_WLAN_Power_Mode_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_POWER_MODE_PARAMS                   14

/** @cond */
/**
Used to get the reason the device was disconnected.
Not used at this time.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_DISCONNECT_REASON                   15
/** Not used at this time. */
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_COUNTRY_CODE                        16
/** @endcond */

/**
Command ID to enable/disable Wake on Wireless (WOW).

This command just initializes the WOW feature and does not perform any filtering
unless appropriate wake patterns are set.
If this feature is enabled, the firmware uses an out-of-band interrupt to awaken the host
in case of a pattern match.

The application should only enable this feature if the bus interrupt cannot be configured
as a wakeup interrupt.
If the bus interrupt can be a wakeup interrupt, the packet filtering feature should be
sufficient to perform necessary filtering functionality.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable WOW, FALSE to disable.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_WAKE_ON_WIRELESS             17

/**
Command ID to add a new packet filter/WOW filter pattern.

This command allows the application to add one filter pattern at a time.
The pattern index value passed by the application should not exceed 8.

Pattern update is currently not supported.

If a given pattern index must be updated, the application should delete the pattern
in that index first and then add a pattern again for the same index.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Add_Pattern_t       Parameter with the necessary pattern
                                         information.
@sa
qapi_WLAN_Add_Pattern_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_DELETE_PATTERN
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ADD_PATTERN                         18

/**
Command ID to enable/disable the Green TX feature.
This is a power optimization feature where WLAN transmit power is
reduced when operating under good link conditions.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t          Set to TRUE to enable Green TX, FALSE to disable.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_GREEN_TX                     19

/**
Command ID to enable/disable the Low Power Listen (LPL) feature.
This is a power optimization feature where WLAN listen is compromised by
operating some of the components with reduced power.
Under good link conditions, this compromise should not have any impact on packet reception.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().Device ID must use 0.

@param[in] uint32_t          Set to TRUE to enable LPL, FALSE to disable.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_LOW_POWER_LISTEN             20

/**
Command ID to set (for TX)/get (for RX) the WLAN rate of packets transmitted over radio.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_Bit_Rate_t    Rate value to be set 

@sa
qapi_WLAN_Bit_Rate_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_RATE                                21

/**
Command ID to get the current operating regulatory domain from the driver.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] uint32_t        Current operating regulatory domain that will 
                            be filled by the driver.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_REG_DOMAIN                          22

/** @cond */
/**
Used to set device MAC address.

@note1hang This parameter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_MAC_ADDRESS                         23
/** @endcond */

/**
Command ID to get the driver transmit queue status.
The application should ensure that no packets are pending for transmission before
putting WLAN in Suspend mode.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] uint32_t        Variable in which the driver returns the Tx status. 
                            The driver populates __QAPI_WLAN_TX_STATUS_IDLE if 
                            the driver queue is empty, some other value otherwise.

@sa
__QAPI_WLAN_TX_STATUS_IDLE
__QAPI_WLAN_TX_STATUS_HOST_PENDING
__QAPI_WLAN_TX_STATUS_WIFI_PENDING
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_TX_STATUS                           24

/**
Command ID to get various statistics information from the WLAN driver/firmware.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] qapi_WLAN_Get_Statistics_t      Variable in which WLAN populates 
                                            stats information collected from the firmware.

@sa
qapi_WLAN_Get_Statistics_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STATS                               25

/**
Command ID to enable/disable the preferred network offload feature.

This command also enables the application to configure various global parameters for the PNO feature.
The application should enable the PNO feature after WLAN disconnects from the AP and 
disable PNO before connecting to an AP.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Preferred_Network_Offload_Config_t    Holds global configuration
                                                           parameters for the PNO feature.

@sa
qapi_WLAN_Preferred_Network_Offload_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_PREFERRED_NETWORK_OFFLOAD_ENABLE    26

/**
Command ID to set the preferred network offload (PNO) profile for which the firmware 
performs autonomous scans based on the PNO global configuration provided.

This command allows the user to add one SSID profile to be scanned on the given index. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Preferred_Network_Profile_t   PNO profile information to scan.

@sa
qapi_WLAN_Preferred_Network_Profile_t\n
__QAPI_WLAN_PNO_MAX_NETWORK_PROFILES
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_PREFERRED_NETWORK_PROFILE           27

/** @cond */
/**
Used to set application information element in outgoing frames.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_APP_IE                              28
/** @endcond */

/**
Command ID to enable/disable the coex feature in the WLAN subsystem.

This command also allows modification of the coex policy and mode of operation.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_Config_Data_t    Coex configuration data to be 
                                           applied.

@sa
qapi_WLAN_Coex_Config_Data_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_COEX                         29

/**
Used to retrieve WLAN coexistence statistics.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().
 
@param[in] qapi_WLAN_Coex_Stats_t    WLAN coex statistics.
 
@sa qapi_WLAN_Coex_Stats_t
 
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_COEX_STATS                          30

/**
Command ID to configure the keepalive frame interval period in Station mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Keepalive interval in seconds.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_KEEP_ALIVE_IN_SEC               31

/** 
Command ID to configure the 802.11 listen interval when operating in Station mode.
This value will be used in the listen interval field of the association request frame.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Listen interval in multiples of beacon intervals 
                           (a value of 1 corresponds to 1 beacon interval). 
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_LISTEN_INTERVAL_IN_TU           32

/**
Command ID to get the RSSI of the associated peer.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] uint8_t         RSSI value variable received from the firmware.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_RSSI                                33

/**
Used to set the received signal strength indicator threshold.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_RSSI_THRESHOLD                      34

/** 
Command ID to configure global parameters of the TCP keepalive (KA) offload feature.

This command also allows application to set global parameters when using the TCP KA 
offload feature.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_TCP_Offload_Enable_t      Configuration parameters to be 
                                               set for the TCP KA feature.

@sa
qapi_WLAN_TCP_Offload_Enable_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_TCP_KEEPALIVE_OFFLOAD_ENABLE        35

/**
Command ID to configure session parameters of the TCP keepalive offload session.
This command allows application to configure one session information at a time.
 
@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_TCP_Offload_Config_Params_t   Per-session configuration
                                                   information to be used in a TCP 
                                                   KA session. The maximum number of 
                                                   TCP KA sessions should 
                                                   not exceed 3.

@sa
qapi_WLAN_TCP_Offload_Config_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_TCP_KEEPALIVE_OFFLOAD_SESSION_CFG   36

/**
Command ID to enable/disable unscheduled automatic power save delivery (UAPSD) 
in Station mode.
This should be done before associating to an access point.

Currently, this command only supports all ACs or none, and explicit UAPSD (ADDTS) 
is not supported.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Bitmask of access categories for which UAPSD 
                           is to be enabled. Following are the bitmask 
                           mappings for various classes of traffic:
                           - Bit 0 -- Best effort traffic
                           - Bit 1 -- Background traffic
                           - Bit 2 -- Video traffic
                           - Bit 3 -- Voice traffic
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_UAPSD                           37

/**
Used to set the maximum number of total buffered MSDUs and MMPDUs delivered by the AP
to the station during a service period.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_MAX_SP_LEN                      38

/**
Command ID to set the beacon interval (in time units) when operating in SoftAP mode.
One TU = 1024 microseconds.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t      Number of TUs between every beacon in SoftAP mode.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_BEACON_INTERVAL_IN_TU            40

/**
Command ID to enable/disable the hidden SSID feature when operating a virtual device 
in SoftAP mode.

This should be done after setting the operating mode as AP and before 
committing the AP profile using qapi_WLAN_Commit().

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable the hidden SSID feature, FALSE otherwise.

@dependencies
Should be set after setting the operating mode to AP by issuing __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE.\n
Should be set before invoking qapi_WLAN_Commit() to start SoftAP.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE\n
qapi_WLAN_Commit()
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_ENABLE_HIDDEN_MODE               41

/** @cond */
/** Not used at this time. */
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_CONNECT_CONTROL_FLAG             42
/** @endcond */

/**
Command ID to set an AP's inactivity period in minutes.

If no keepalive frames are received from an associated station during
this period, the AP deassociates that station. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Inactivity interval for associated stations in 
                           minutes.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_INACTIVITY_TIME_IN_MINS          43

/**
Command ID to enable/disable the WPS feature when operating a virtual device 
in SoftAP mode.

This should be done after setting the operating mode as AP and before 
committing the AP profile using qapi_WLAN_Commit().

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable the WPS feature, FALSE otherwise.

@dependencies
Should be set after setting the operating mode to AP by issuing __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE.\n
Should be set before invoking qapi_WLAN_Commit() to start SoftAP.

@sa
qapi_WLAN_Commit() \n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_WPS_FLAG                         44

/**
Command ID to change the DTIM interval when operating a virtual device 
in SoftAP mode.

This setting should be done before committing the AP profile using qapi_WLAN_Commit().

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        DTIM interval in multiples of the beacon interval.

@dependencies
Should be set after setting the operating mode to AP by issuing __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE.\n
Should be set before invoking qapi_WLAN_Commit() to start SoftAP.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE\n
qapi_WLAN_Commit
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_DTIM_INTERVAL                    45

/** @cond */
/**
Used to set per-STA buffers in AP mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_PS_BUF                           46

/**
Used to set the WOW GPIO configuration.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_WOW_GPIO_CONFIG                     47
/** @endcond */

/**
Command ID to set Address Resolution Protocol (ARP) offload parameters for the 
given virtual device.
This should only be used when operating in Station mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_ARP_Offload_Config_t    ARP offload configuration parameters.

@sa
qapi_WLAN_ARP_Offload_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ARP_OFFLOAD_PARAMS                  48

/**
Command ID to set network solicitation (NS) offload parameters for the 
given virtual device.
This should only be used when operating in Station mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_NS_Offload_Config_t    NS offload configuration parameters.

@sa
qapi_WLAN_NS_Offload_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_NS_OFFLOAD_PARAMS                   49

/**
Command ID to enable/disable the packet filtering feature.

This command just initializes the packet filtering feature and does not
perform any filtering until packet filter patterns are set.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable packet filtering, FALSE to disable.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PKT_FILTER                   50
/**
Command ID to delete a patten in a given pattern index.
The pattern index should not exceed 8.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Pattern index to be deleted.
@param[in] uint32_t        Pattern header type.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ADD_PATTERN
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_DELETE_PATTERN                      51

/**
Command ID to enable/disable UAPSD in SoftAP mode.

UAPSD must be enabled in SoftAP after setting the operating mode as AP and
before committing the AP profile (qapi_WLAN_Commit()) to start SoftAP.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Set to TRUE to enable UAPSD, FALSE otherwise.

@dependencies
Should be set after setting the operating mode to AP by issuing __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE.\n
Should be set before invoking qapi_WLAN_Commit() to start SoftAP.

@sa
qapi_WLAN_Commit\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_ENABLE_UAPSD                     52

/**
Command ID to configure A-MPDU parameters for an AMPDU session.

These parameters should be set before establishing the connection in both STA and AP modes.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Rx_Aggrx_Params_t     Aggregation parameters to be set.

@sa
qapi_WLAN_Rx_Aggrx_Params_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AGGRX_CONFIG                        53

/**
Command ID to enable/disable 802.11v functionality for the WLAN subsystem.

This command instantiates WNM context in the WLAN subsystem but does not actually enable 
any WNM features.

Applications should use other WNM commands to enable required features and 
enable the WNM configuration using this command before enabling any WNM features.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint8_t         Set to TRUE to enable WNM, FALSE to disable
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_CONFIG                          54

/**
Command ID to enter/exit 802.11v WNM sleep along with the sleep parameters.
This command is only supported only in Station mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_WNM_Sleep_Period_t    Parameters to enter/exit WNM sleep.

@dependencies
Applications should enable WNM using __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_CONFIG
before enabling the WNM sleep feature.

@sa
qapi_WLAN_WNM_Sleep_Period_t\n
__QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_CONFIG
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_STA_SLEEP_PERIOD                55

/**
Command ID to enable/disable the WNM BSS maximum idle period feature.

This command is only supported in SoftAP mode.

If this feature is enabled, SoftAP beacons will start including the WNM BSS maximum idle period IE.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_BSS_Max_Idle_Period_t Parameter to enable the WNM BSS maximum 
                                           idle period feature in SoftAP.

@dependencies
Applications should enable WNM using __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_CONFIG
before enabling the WNM BSS maximum idle feature.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_CONFIG\n
qapi_WLAN_BSS_Max_Idle_Period_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_AP_BSS_MAX_IDLE_PERIOD          56

/** @cond */
/**
Used to set the AP's response for a WNM sleep request from the station.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param(). \n
It is only used for internal WNM sleep testing in SoftAP mode.
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_WNM_AP_SLEEP_RESPONSE               57
/** @endcond */

/**
Command ID to initiate a channel switch in SoftAP mode.

This command enables SoftAP to send a channel switch request in its beacons to all its
associated clients for given periods and changes SoftAP's operating channel to a new channel 
requested by the application upon completion of a requested period.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Channel_Switch_t      Parameters to be set for a
                                           channel switch request.

@dependencies
SoftAP must be up and running.

@sa
qapi_WLAN_Channel_Switch_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_CHANNEL_SWITCH                   58

/**
Command ID to set up event filters in the WLAN subsystem.

This command enables an applications to choose events that are not in its interest
so that the firmware can skip sending those events to hosts, thereby avoiding unnecessary host wakeups.

Note that all the events are not filterable. Event filters should be set for every virtual device. The
maximum number of events that can be filtered for each virtual device should not exceed 
__QAPI_WLAN_MAX_NUM_FILTERED_EVENTS.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Event_Filter_t        Event filter configuration to be 
                                           set.

@sa
qapi_WLAN_Filterable_Event_e \n
qapi_WLAN_Event_Filter_t \n
__QAPI_WLAN_MAX_NUM_FILTERED_EVENTS
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_EVENT_FILTER                        59

/**
Command ID to set wireless 11n HT parameters of a given virtual device. The set 
operation for this should be done before establishing a connection.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_11n_HT_Config_e    Required 11n HT configuration must be specified.

@sa
qapi_WLAN_11n_HT_Config_e
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_11N_HT                              60

/**
Command ID to set Beacon Miss configuration. 

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in,out] qapi_WLAN_Sta_Config_Bmiss_Config_t    Beacon Miss parameters to be set.

@sa
qapi_WLAN_Sta_Config_Bmiss_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_BMISS_CONFIG                              61


/**
Command ID to change the action for the default filter for a given header type.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint32_t        Pattern action flag.
@param[in] uint32_t        Pattern header type.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_ADD_PATTERN
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CHANGE_DEFAULT_FILTER_ACTION        62

/**
Command ID to configure the country code for AP mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint8_t[]        Country code string.

*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_COUNTRY_CODE                        63

/**
Command ID to configure SCO coex operating parameters in the WLAN subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_Sco_Config_t     Coex SCO configuration data to be 
                                           applied.

@sa
qapi_WLAN_Coex_Sco_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CONFIG_SCO_COEX                        64

/**
Command ID to configure A2DP coex operating parameters in the WLAN subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_A2dp_Config_t    Coex A2DP configuration data to be 
                                           applied.

@sa
qapi_WLAN_Coex_A2dp_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CONFIG_A2DP_COEX                       65

/**
Command ID to configure ACL coex operating parameters in the WLAN subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_Acl_Config_t     Coex ACL configuration data to be 
                                           applied.

@sa
qapi_WLAN_Coex_Acl_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CONFIG_ACL_COEX                       66

/**
Command ID to configure INQPAGE coex operating parameters in the WLAN subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_InqPage_Config_t   Coex INQPAGE configuration data to be 
                                             applied.

@sa
qapi_WLAN_Coex_InqPage_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CONFIG_INQPAGE_COEX                  67

/**
Command ID to configure HID coex operating parameters in the WLAN subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Coex_Hid_Config_t     Coex HID configuration data to be 
                                           applied.

@sa
qapi_WLAN_Coex_Hid_Config_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_CONFIG_HID_COEX                     68

/**
Command ID to override the default weight table values used by the WLAN coex subsystem.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param()..

@param[in] qapi_WLAN_Coex_Override_Wghts_t     Coex weight value overrides to be 
                                               applied.

@sa
qapi_WLAN_Coex_Override_Wghts_t
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_OVERRIDE_WGHTS_COEX                 69

/**
Command ID to get the channel list for the current regulatory setting.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] qapi_WLAN_Get_Channel_List_t    Variable in which WLAN populates 
                                            channel information collected from the firmware.

@sa
#qapi_WLAN_Get_Channel_List_t

*/

#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_GET_CURR_REGDOMAIN_CHANNEL_LIST                    70

/**
Command ID to set antenna diversity, such as enable or disable antenna diversity, 
tx antenna follows rx antenna or not, the antenna switch mode, the time interval or 
the number of packet used for antenna selection.

@note1hang This paramter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_Ant_Div_Config_t    Variable used to set antenna diversity.

@sa
#qapi_WLAN_Ant_Div_Config_t

*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_SET_ANT_DIV               					    71

/**
Command ID to get the status of antenna diversity, such as the current rx physical antenna in 2g or 5g, 
the current tx physical antenna in 2g or 5g, the average main rssi, the average alternative rssi and so on.

@note1hang This paramter can only be used with qapi_WLAN_Get_Param().

@param[out] qapi_WLAN_Get_Ant_Div_t    Variable in which WLAN populates the status of antenna diversity
										  information collected from the firmware
@sa
#qapi_WLAN_Get_Ant_Div_t

*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_GET_ANT_DIV               					    72

/**
Command ID to set physical antenna.

@note1hang This paramter can only be used with qapi_WLAN_Set_Param().

@param[in] uint8_t the physical antenna number
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_SET_ANTENNA               					    73

/** 
Command ID to set/get the authentication mode for an upcoming association operation.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_Auth_Mode_e   Authentication mode to be set.

@dependencies
Authentication mode must be set before connecting to the peer.

@sa
qapi_WLAN_Auth_Mode_e
*/

/**
Internal use
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_DISABLE_CHANNEL									74

/**
0: will use default bData, 1: will use ext-bData 1, for future, '2' will use ext-bData 2, etc...

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_WIRELESS_EXT_BOARDDATA_INDEX	      							75


#define __QAPI_WLAN_PARAM_GROUP_SECURITY_AUTH_MODE                0

/** 
Command ID to set/get the encryption mode for an upcoming association operation.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_Crypt_Type_e   Encryption mode to be set.

@dependencies
Encryption mode must be set before connecting to the peer.

@sa
qapi_WLAN_Crypt_Type_e
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_ENCRYPTION_TYPE          1

/** 
Command ID to set the pairwise master key for the upcoming WPA/WPA2 association 
procedure.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint8_t[]       Pairwise master key to be set. The master key 
                           should be of length __QAPI_WLAN_PASSPHRASE_LEN.
@dependencies
This should be done before initiating an association.

@sa
__QAPI_WLAN_PASSPHRASE_LEN
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_PMK                      2

/** 
Command ID to set the passphrase for the upcoming WPA/WPA2 association procedure.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] uint8_t[]       Passphrase to be set. The passphrase length  
                           should not exceed __QAPI_WLAN_PASSPHRASE_LEN.
@dependencies
This should be done before initiating an association.

@sa
__QAPI_WLAN_PASSPHRASE_LEN
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_PASSPHRASE               3

/**
Command ID to set the active WEP key index for an upcoming association.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param() APIs.

@param[in,out] uint32_t        Active key index to be set. The value should be 
                               set so that \n 1 <= index <= 4.

@dependencies
This should be done before initiating an association.
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_WEP_KEY_INDEX            4

/**
Command ID to set a WEP {key index , key} pair for an upcoming association.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param() APIs.

@param[in,out] qapi_WLAN_Security_Wep_Key_Pair_Params_t   Parameters to set the 
                                                          WEP key parameters.
@dependencies
This should be done before initiating an association.
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_WEP_KEY_PAIR             5

/**
Command ID to set WPS credentials received after WPS negotiation with the peer.
These are the credentials that will be used for secure association with the peer.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_WPS_Credentials_t     WPS credential information to be used 
                                           for a secure association.

@dependencies
This should be done after WPS negotiation is completed and before performing a 
secure association.

@sa
qapi_WLAN_WPS_Credentials_t
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_WPS_CREDENTIALS          6

/**
Command ID to set/get the 802.1x method.

@note1hang This paramter can be used with qapi_WLAN_Set_Param() and qapi_WLAN_Get_Param().

@param[in,out] qapi_WLAN_8021x_Method_e   802.1x mode to be set.

@sa
qapi_WLAN_8021x_Method_e

*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_METHOD           7

/**
The first Command ID to set/get the 802.1x information.
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_START            (__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_METHOD)

/**
@ingroup qapi_wlan
set the 802.1x identity for PEAP/TTLS method.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_IDENTITY
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_IDENTITY         8

/**
@ingroup qapi_wlan
set the 802.1x username for PEAP/TTLS method.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_USERNAME
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_USERNAME         9

/**
@ingroup qapi_wlan
set the 802.1x password for PEAP/TTLS method.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_PASSWORD
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_PASSWORD         10

/**
@ingroup qapi_wlan
Set/get the 802.1x CA certificate filename.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_CA_CERT
qapi_WLAN_Set_Param
qapi_WLAN_Get_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_CA_CER           11

/**
@ingroup qapi_wlan
Set/get the 802.1x certificate filename.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_CERT
qapi_WLAN_Set_Param
qapi_WLAN_Get_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_CER              12

/**
@ingroup qapi_wlan
Set the 802.1x private key filename and its password.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_PRIVATE_KEY
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_PRIVATE_KEY      13

/**
@ingroup qapi_wlan
Set security debug level.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_DEBUG_LEVEL
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_DEBUG_LEVEL              14

/**
@ingroup qapi_wlan
Set security priv.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_PRIV
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_PRIV           15

/**
The last Command ID to set/get the 802.1x information.
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_END            20

/**
@ingroup qapi_wlan
Set PMKID.\n

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_PMKID
qapi_WLAN_Set_Param
*/
#define __QAPI_WLAN_PARAM_GROUP_SECURITY_PMKID          21

/**
Command ID to configure P2P device parameters. 
Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Config_Params_t   Device parameters to be configured.

@dependencies
P2P mode must be enabled before device parameter configuration.

@sa
qapi_WLAN_P2P_Config_Params_t \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Enable()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_CONFIG_PARAMS 0

/**
Command ID to configure P2P opportunistic power save parameters. 
Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Opps_Params_t   Opportunistic power save parameters.

@dependencies
Opportunistic power save mode applies only to P2P group owners. A device can initiate 
an autonomous group owner operation or it can become 
a group owner through a group owner negotiation process.

@sa
qapi_WLAN_P2P_Opps_Params_t \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Connect() \n
qapi_WLAN_P2P_Start_Go()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_OPPS_PARAMS 1

/**
Command ID to configure P2P notice of absence (NOA) parameters. 
Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Noa_Params_t   Notice of absence configuration parameters.

@dependencies
NOA applies only to P2P group owners. A device can initiate autonomous group owner
operation or it can become a group owner through a group 
owner negotiation process.

@sa
qapi_WLAN_P2P_Noa_Params_t \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Connect() \n
qapi_WLAN_P2P_Start_Go()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_NOA_PARAMS 2

/**
Command ID to get a list of P2P peer devices (/nodes) found through the P2P find phase. 
Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Get_Param().

@param[in, out] qapi_WLAN_P2P_Node_List_Params_t   List to get peer device information.

@dependencies
Nearby devices must be discovered using qapi_WLAN_P2P_Find() before the node list can show those.

@sa
qapi_WLAN_P2P_Node_List_Params_t
qapi_WLAN_Get_Param() \n
qapi_WLAN_P2P_Find()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST 3

/**
Command ID to get a list of P2P groups stored in the nonvolatile storage. This list is persistent across reboots. 
Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Get_Param().

@param[in, out] qapi_WLAN_P2P_Network_List_Params_t   List to get persistent connections information.

@dependencies
Persistent groups should be formed by using the 'persistent' option while connecting to a peer device and/or 
authenticating a peer device for a connection. Without this option, the connections will be lost when the
device reboots.

@sa
qapi_WLAN_P2P_Network_List_Params_t \n
qapi_WLAN_Get_Param() \n
qapi_WLAN_P2P_Connect() \n
qapi_WLAN_P2P_Auth()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_NETWORK_LIST 4

/**
Command ID to configure a device listen channel that is used for the device to be discoverable.

The listen channel should be one of the social channels (1, 6, 11 for 2.4 Ghz) and it should remain the same until the 
device discovery completes. Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this macro as 'config_Id' and
'listen_Channel' members of union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Listen_Channel_t   Listen channel information.

@sa
qapi_WLAN_P2P_Listen_Channel_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL 6

/**
Command to configure the postfix to be appended to the P2P group SSID.

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this macro
as 'config_Id' and 'ssid_Postfix' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Ssid_Postfix_t   SSID postfix information.

@dependencies
Only a group owner can add an SSID postfix.

@sa
qapi_WLAN_P2P_Set_Ssid_Postfix_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX 8

/**
Command ID to enable/disable intra BSS data forwarding support.

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this macro
as 'config_Id' and 'intra_Bss' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Intra_Bss_t   Flag to enable/disable intra BSS data forwarding support.

@sa
qapi_WLAN_P2P_Set_Intra_Bss_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS 9

/**
Command ID to configure a device's group owner intent.

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this 
macro as 'config_Id' and 'go_Intent' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Go_Intent_t   Device's group owner intent.

@sa
qapi_WLAN_P2P_Set_Go_Intent_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT 11

/**
Command ID to configure a device name.

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this 
macro as 'config_Id' and 'device_Name' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Dev_Name_t   Device name to be configured.

@sa
qapi_WLAN_P2P_Set_Dev_Name_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME 12

/**
Command ID to configure a device's P2P operating mode. 

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param() 
to set the device operation. It is used only when the mode to be set is 
__QAPI_WLAN_P2P_CLIENT before mode to client before calling qapi_WLAN_P2P_Join().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this
macro as 'config_Id' and 'mode' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Mode_t   Operating mode.

@sa
__QAPI_WLAN_P2P_CLIENT \n
qapi_WLAN_P2P_Set_Mode_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE 13

/**
Command ID to enable/disable a complementary code keying (CCK) modulation scheme. 

Use this parameter with group ID __QAPI_WLAN_PARAM_GROUP_P2P to call qapi_WLAN_Set_Param().
Alternately, applications can use an object of structure qapi_WLAN_P2P_Set_Cmd_t with this 
macro as 'config_Id' and 'cck_Rates' members of 
union 'val' as data to call qapi_WLAN_Set_Param().

@param[in] qapi_WLAN_P2P_Set_Cck_Rates_t   Enable/disable a CCK modulation scheme.

@sa
qapi_WLAN_P2P_Set_Cck_Rates_t \n
qapi_WLAN_P2P_Set_Cmd_t \n
qapi_WLAN_Set_Param()
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES 14

/**
Command ID used for calling qapi_WLAN_Set_Param() to configure P2P group owner parameters. 
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_GO_PARAMS 5

/** @cond */
/**
Use this macro to call qapi_WLAN_Set_Param() to configure P2P cross 
connect parameters.

@sa
qapi_WLAN_P2P_Set_Cross_Connect_t
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_CROSS_CONNECT 7
/**
Set whether a device should operate in concurrent mode.

@note1hang This parameter can only be used with qapi_WLAN_Set_Param().
*/
#define __QAPI_WLAN_PARAM_GROUP_P2P_CONCURRENT_MODE 10
/** @endcond */

/**
Macro that defines the maximum length of a string for each of the subversion information strings.
*/
#define __QAPI_WLAN_VERSION_SUBSTRING_LEN 20

/** 
Macro that indicates the maximum size of patterns supported for packet filtering and 
WOW filtering features.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_MAX_SIZE 128

/** 
Macro that indicates the maximum size of a supported pattern mask.

This is the function of __QAPI_WLAN_PATTERN_MAX_SIZE as a pattern mask, which is
a bitmap that indicates the validity of pattern data where every bit in the pattern
mask corresponds to a byte in the pattern data.

For an application, if a pattern to be matched is not a contiguous byte stream,
the application should set valid bytes to be matched by setting appropriate bits
in the pattern mask member of qapi_WLAN_Add_Pattern_t.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_MASK (__QAPI_WLAN_PATTERN_MAX_SIZE/8)

/** 
Macro to indicate a filter action mask as a REJECT on finding a matched pattern 
during WOW and packet filtering.

If this flag is set in pattern_Action_Flag of qapi_WLAN_Add_Pattern_t for
a given pattern, the received packet that matches this pattern is dropped by the firmware.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_REJECT_FLAG 0x1

/** 
Macro to indicate a filter action as ACCEPT on finding a matched pattern during 
WOW and packet filtering.

If this flag is set in pattern_Action_Flag of qapi_WLAN_Add_Pattern_t for a given pattern,
the received packet that matches this pattern is forwarded to the host by the firmware.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_ACCEPT_FLAG 0x2

/** 
Macro to indicate a filter action mask as DEFER on finding a matched pattern 
during WOW and packet filtering.

If this flag is set in pattern_Action_Flag of qapi_WLAN_Add_Pattern_t for
a given pattern, the received packet that matches this pattern defers to the next header type.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_DEFER_FLAG 0x4

/** 
Macro to indicate that a given pattern is a WOW pattern.

If pattern_Action_Flag has this bit set and if data is received with the pattern matched,
the firmware wakes up the host by asserting an out-of-band GPIO interrupt.

If this bit is not set in the pattern_Action_Flag of the pattern added, the pattern
is just considered as a packet filter and an out-of-band interrupt will not be asserted
on matching the pattern.

@sa
qapi_WLAN_Add_Pattern_t
*/
#define __QAPI_WLAN_PATTERN_WOW_FLAG 0x80

/** Default pattern index for WOW and packet filtering. */
#define __QAPI_WLAN_DEFAULT_FILTER_INDEX      0

/** Maximum value of the pattern index for WOW and packet filtering. */
#define __QAPI_WLAN_MAX_FILTER_INDEX      8

/** Pattern index to delete all filters in the protocol header. */
#define __QAPI_WLAN_DELETE_ALL_FILTER_INDEX      0xFF

/** Default pattern prioirty for WOW and packet filtering. */
#define __QAPI_WLAN_DEFAULT_FILTER_PRIORITY   0

/** 
Macro that indicates that the TX queue of the WLAN driver is empty. If the driver 
returns this on querying the TX status, the application can safely put the WLAN 
subsystem in Suspend mode.
*/
#define __QAPI_WLAN_TX_STATUS_IDLE 0x01
/** 
Macro that indicates that the WLAN driver TX queue has one or more frames waiting to be 
transmitted.
If the driver returns this on querying the TX status, the application should 
not proceed with the suspend operation.
*/
#define __QAPI_WLAN_TX_STATUS_HOST_PENDING 0x02
/** 
Macro that indicates that the WLAN driver has not completed transmission of all the frames, 
even if the TX queue is empty.
If the driver returns this on querying the TX status, the application should not proceed
with the suspend operation. 
*/
#define __QAPI_WLAN_TX_STATUS_WIFI_PENDING 0x03

/** 
Macro that indicates the maximum number of buffers that can be aggregated by the WLAN 
subsystem when transmitting an AMPDU frame.
*/
#define __QAPI_WLAN_AGGRX_BUFFER_SIZE_MAX 16

/** 
Macro to indicate in the host for the firmware not to consider a particular aggregation parameter.

The firmware considers values other than 0xFF as valid aggregation parameter from the host while
configuring aggregation parameters.
To avoid using invalid aggregation parameters in the target, the host resets all the aggregation
parameters to 0xFF (this macro) before configuring the desired parameters.
*/
#define __QAPI_WLAN_AGGRX_CFG_INVAL       0xFF

/** @cond */
#ifdef WLAN_TESTS
#define __QAPI_WLAN_PARAM_WLAN_TEST_MCC                           0
#endif
/** @endcond */

/** @} */ /* end_addtogroup qapi_wlan */

/**
@ingroup qapi_wlan
Identifies the enable/disable options for WLAN.
*/
typedef enum
{
    QAPI_WLAN_DISABLE_E  = 0, /**< Disable the Wi-Fi module. */
    QAPI_WLAN_ENABLE_E   = 1  /**< Enable the Wi-Fi module. */
} qapi_WLAN_Enable_e;

/**
@ingroup qapi_wlan
Identifies the WLAN enable/disable status for OM operations.
*/
typedef enum
{
    QAPI_WLAN_OM_DISABLED = 0, /**< wlan is disabled in OM. */
    QAPI_WLAN_OM_ENABLED  = 1  /**< wlan is enabled in OM. */
} qapi_WLAN_OM_STATUS;

/**
@ingroup qapi_wlan
Enum declarations for the scan control flags.

@note1hang The ScanCtrlFlag value of 0xFF is used to disable all flags in the Scan Params Cmd.
Do not add any more flags to qapi_WLAN_Scan_Ctrl_Flag_bits_e.
*/
typedef enum 
{
    QAPI_WLAN_CONNECT_SCAN_CTRL_FLAGS_E = 0x01,    /**< Set if the Connect command can be scanned. */
    QAPI_WLAN_SCAN_CONNECTED_CTRL_FLAGS_E = 0x02,  /**< Set if the scan is for the SSID it is already connected to. */
    QAPI_WLAN_ACTIVE_SCAN_CTRL_FLAGS_E = 0x04,     /**< Set if active scan is enabled. */
    QAPI_WLAN_ROAM_SCAN_CTRL_FLAGS_E = 0x08,       /**< Set if roam scan is enabled when BMISS and LOWRSSI. */
    QAPI_WLAN_REPORT_BSSINFO_CTRL_FLAGS_E = 0x10,  /**< Set if the scan follows the customer's BSSINFO reporting rule. */
    QAPI_WLAN_ENABLE_AUTO_CTRL_FLAGS_E = 0x20,     /**< If disabled, the target does not scan after a disconnect event. */
    QAPI_WLAN_ENABLE_SCAN_ABORT_EVENT_E = 0x40,    /**< Scan complete event with cancelled status will be generated
						        when a scan is pre-empted before it is completed. */
    QAPI_WLAN_ENABLE_DFS_SKIP_CTRL_FLAGS_E = 0x80  /**< Set to skip scanning a DFS channel. */
} qapi_WLAN_Scan_Ctrl_Flag_bits_e;

/**
@ingroup qapi_wlan
Macro to use the default scan control flags for a scan.
*/
#define __QAPI_WLAN_DEFAULT_SCAN_CTRL_FLAGS  ( QAPI_WLAN_CONNECT_SCAN_CTRL_FLAGS_E| QAPI_WLAN_SCAN_CONNECTED_CTRL_FLAGS_E| QAPI_WLAN_ACTIVE_SCAN_CTRL_FLAGS_E| QAPI_WLAN_ROAM_SCAN_CTRL_FLAGS_E| QAPI_WLAN_ENABLE_AUTO_CTRL_FLAGS_E )

/**
@ingroup qapi_wlan
Data structure used by the application to pass wireless scan options to the driver.
If the device is connected to an AP, a successful scan returns the current AP along
with newly scanned APs.
*/
typedef struct //qapi_WLAN_Start_Scan_Params_s
{
    int32_t         force_Fg_Scan;
    /**< Force a high priority scan. */
    uint32_t        home_Dwell_Time_In_Ms;
    /**< Maximum scan duration in the home channel (in ms). If set to 0, the default value of 50 ms is used. */
    uint32_t        force_Scan_Interval_In_Ms;
    /**< Time interval (in ms) between scanning channels from the list. If set to 0, the default value of 100 ms is used.  */
    uint8_t         scan_Type;
    /**< This parameter currently supports only 0 as an input value. */
    uint8_t         num_Channels;
    /**< Number of channels to scan. */
    uint16_t        channel_List[1];
    /**< List of channels to scan. */
} qapi_WLAN_Start_Scan_Params_t;

/**
@ingroup qapi_wlan
This data type enumerates various options provided by the WLAN SDK to manage 
scan results.

@sa
qapi_WLAN_Start_Scan
*/
typedef enum
{
    QAPI_WLAN_BUFFER_SCAN_RESULTS_BLOCKING_E     = 0,  
        /**<
        Blocking scan. The application that initiated the scan will be 
        blocked inside the driver until the scan completes and scan results 
        are copied into the application supplied buffer.
        */
    QAPI_WLAN_BUFFER_SCAN_RESULTS_NON_BLOCKING_E = 1,  
        /**<
        Nonblocking scan. The application that initiated the scan will not 
        be blocked inside the driver.

        All the scan results are buffered within the driver until the scan operation is complete.
        On scan completion, the driver invokes a callback function registered by the application to indicate
	its completion.
        On receiving the callback, the application is expected to invoke the get scan results API to retrieve the
        buffered scan results from the driver.
        */
    QAPI_WLAN_NO_BUFFERING_E                     = 2   
        /**<
        Nonblocking scan. The application that initiated the scan will not 
        be blocked inside the driver.

	In this option, all BSS information 
        events that are received during the scan operation are not stored 
        within the driver buffer, but instead passed directly as events through 
        application registered callbacks. At the end of the scan, the driver invokes 
        a scan completion callback to indicate the completion of the scan. It is 
        up to the application to manage the scan BSS information, so the driver 
        does not hold any scan results in this case.
        */
} qapi_WLAN_Store_Scan_Results_e;

/**
@ingroup qapi_wlan
Data structure that the application uses to interpret the scan results for 
all access point information received during the scan.

All the information in this structure is for one particular BSS found during the scan.
*/
typedef struct //qapi_WLAN_BSS_Scan_Info_s
{
    uint8_t  channel;                        /**< Wireless channel. */
    uint8_t  ssid_Length;                    /**< SSID length. */
    uint8_t  rssi;                           /**< Received signal strength indicator. */
    uint8_t  security_Enabled;               /**< 1: Security enabled; 0: Security disabled. */
    uint16_t beacon_Period;                  /**< Beacon period. */
    uint8_t  preamble;                       /**< Preamble. */
    uint8_t  bss_type;                       /**< BSS type. */
    uint8_t  bssid[__QAPI_WLAN_MAC_LEN];     /**< BSSID. */
    uint8_t  ssid[__QAPI_WLAN_MAX_SSID_LEN]; /**< SSID. */
    uint8_t  rsn_Cipher;                     /**< RSN cipher. */
    uint8_t  rsn_Auth;                       /**< RSN authentication. */
    uint8_t  wpa_Cipher;                     /**< WPA cipher. */
    uint8_t  wpa_Auth;                       /**< WPS authentication. */
    uint16_t caps;                           /**< Capability IE. */
    uint8_t  wep_Support;                    /**< Supprt for WEP. */
    uint8_t  reserved;                       /**< Reserved. */
} qapi_WLAN_BSS_Scan_Info_t; 

/**
@ingroup qapi_wlan
This data type enumerates the scan status to indicate the scan is succeeded, required to rescan or failed.
*/
typedef enum
{
	QAPI_WLAN_SCAN_STATUS_SUCCESS_E = 0,          /*scan is succeeded*/
	QAPI_WLAN_SCAN_STATUS_REQUIRE_RESCAN_E = 1,	 /*require to rescan if the scan is cancelled or aborted as the target is busy or has more urgent task*/
	QAPI_WLAN_SCAN_STATUS_FAILURE_E = 2   /*scan is failed*/
}qapi_WLAN_Scan_Status_e;

/**
@ingroup qapi_wlan
 Data structure that the application uses to interpret the scan mode and scan status. 
 Scan mode is the way that WLAN SDK to manage scan results, refer to enum qapi_WLAN_Store_Scan_Results_e. 
 Scan status shows the scan is succeeded, failed or required to rescan,refer to enum qapi_WLAN_Scan_Status_e.
*/
typedef struct
{
    uint16_t scan_Mode;      /*scan mode, refer to enum qapi_WLAN_Store_Scan_Results_e. */
    uint16_t scan_Status;	 /*scan status, refer to enum qapi_WLAN_Scan_Status_e. */
} qapi_WLAN_Scan_Status_t;

/**
@ingroup qapi_wlan
qapi_WLAN_BSS_Scan_Info_t is not enough and cannot be changed, 
here add a new structure to report more information.
*/
typedef struct
{
    qapi_WLAN_BSS_Scan_Info_t   info;           /**< The basic info. */
    int32_t                     is_beacon;      /**< TRUE: beacon, FALSE: ProbeRsp. */
    const uint8_t               *beacon_IEs;    /**< Beacon or Probe Response IEs which do not include fixed fields. */
    uint16_t                    beacon_IEs_Len; /**< Length of beacon_IEs. */
    uint8_t                     reserved[6];    /**< Reserved, 2 bytes for allignment, 4 bytes for extension. */
} qapi_WLAN_BSS_Scan_Info_ExtV1_t;

/**
@ingroup qapi_wlan
WLAN driver invokes an application-registered callback function to indicate various 
asynchronous events to the application. This data structure enumerates the list 
of various event IDs for which the WLAN driver invokes the application-registered callback
function.
*/
typedef enum
{
   QAPI_WLAN_CONNECT_CB_E                   = 0,  
          /**< ID to indicate connect/disconnect events. */
   QAPI_WLAN_SCAN_COMPLETE_CB_E             = 1,  
          /**<
          ID to indicate a wireless scan complete event, received for 
          nonblocking/nonbuffering scans.
          */
   QAPI_WLAN_FWD_PROBE_REQUEST_INFO_CB_E    = 2,  
          /**<
          ID to indicate a probe request forwarded from the WLAN firmware 
          when probe request forwarding is enabled by the application.
          */
   QAPI_WLAN_RESUME_CB_INFO_CB_E            = 3,  
          /**<
          ID to indicate a WLAN driver/firmware resume completed event and that the 
          application can start sending data over the WLAN driver after receiving 
          this event (for future use).
          */
   QAPI_WLAN_PROMISCUOUS_MODE_CB_INFO_CB_E  = 4,  
          /**<
          ID to indicate that packets were captured in Promiscuous mode. For every 
          packet received in Promiscuous mode, the driver indicates this event to 
          the application. Since it is quite possible to receive packets too 
          frequently in Promiscuous mode, the application is expected to do a minimal 
          operation in the callback implementation.
          */
   QAPI_WLAN_DISCONNECT_CB_E                = 5,  
          /**<
          Currently not used. A disconnect event is indicated in the parameter 
          of QAPI_WLAN_CONNECT_CB_E.
          */
   QAPI_WLAN_DEAUTH_CB_E                    = 6,  
          /**<
          Currently not used. A disconnect event is indicated in the parameter 
          of QAPI_WLAN_CONNECT_CB_E.
          */
   QAPI_WLAN_WPS_CB_E                       = 7,  
          /**<
          ID to indication completion of a WPS handshake to the application.
          The application should use qapi_WLAN_WPS_Await_Completion() to get event information.
          */
   QAPI_WLAN_P2P_CB_E                       = 8,  
          /**<
          ID to indicate all P2P events. Since most of the P2P event 
          handling is done by the application, it is necessary for the application to 
          copy necessary information from events(in an event callback) and handle this in 
          the application's own thread context.
          */
   QAPI_WLAN_BSS_INFO_CB_E                  = 9,  
          /**<
          ID to indicate that AP profile information was received when performing a nonbuffering 
          scan. Each event of this type indicates only the AP's profile information.
          */
   QAPI_WLAN_TCP_KEEPALIVE_OFFLOAD_CB_E     = 10, 
          /**<
          ID to indicate an event for a TCP Keepalive Offload request, received 
          either on termination of TCP KA offload or when a TCP timeout occurs 
          for one or more of the TCP KA sessions.
          */
   QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_CB_E = 11, 
          /**<
          ID to indicate that a PNO profile event was received when a profile match is 
          found or when PNO is disabled by the application.
          */
   QAPI_WLAN_WNM_CB_E                       = 12, 
          /**<
          ID to indicate events received when WNM commands, such as setting 
          BSS maximum idle period, enter/exit WNM sleep complete execution.
          */
   QAPI_WLAN_CHANNEL_SWITCH_CB_E            = 13, 
          /**<
          ID to indicate a wireless channel change event received when STA 
          changes the operating channel due to a channel switch announcement IE 
          from the connected access point.
          */
   QAPI_WLAN_READY_CB_E                     = 14, 
          /**<
          ID to indicate the event that announces that the Wi-Fi module (firmware) 
          is enabled and ready.
          */
   QAPI_WLAN_SUSPEND_CB_E                   = 15, 
          /**< ID to indicate a WLAN firmware suspend event. */
   QAPI_WLAN_DRIVER_DISABLE_CB_E            = 16,
          /**< ID to indicate a driver shut down complete event. */
   QAPI_WLAN_ERROR_HANDLER_CB_E             = 17,
          /**< ID to indicate a fatal error in the WLAN driver. */
   QAPI_WLAN_RESUME_HANDLER_CB_E            = 18,
          /**< ID to indicate to resume completion of the WLAN firmware. */
   QAPI_WLAN_RX_EAPOL_KEY_CB_E              = 19,
          /**< ID to indicate to receive eapol key frame. */
} qapi_WLAN_Callback_ID_e;

/**
@ingroup qapi_wlan
Data structure that presents connect event information from the driver to the 
application.

The application uses this data structure to interpret the event 
payload received with a QAPI_WLAN_CONNECT_CB_E event.
*/
typedef struct //qapi_WLAN_Connect_Cb_Info_s
{
   int32_t   value;
        /**< TRUE: To indicate connect events \n
             FALSE: To indicate disconnect/deauthorization events
   */
   uint8_t   mac_Addr[__QAPI_WLAN_MAC_LEN];   
        /**<
        MAC address related to the connect event. Based on the operating mode, 
        the driver fills in a different MAC addresses as follows. 
        - STA mode connect event -- MAC address of the connected access point
        - STA mode disconnect event -- MAC address contains all zeroes
        - AP mode connect event -- MAC address of the device itself, or a peer 
                                   station that was connected to the AP
                                   (based on the bss_Connection_Status parameter)
        - AP mode disconnect event -- MAC address contains zeros or a peer 
                                      station that was disconnected from the AP
                                      (based on the bss_Connection_Status parameter) @vertspace{-14}
        */
   uint32_t  bss_Connection_Status;           
        /**<
        Flag that indicates whether it is a BSS level connection/disconnection  
        or an individual station-level connection/disconnection.
        - STA mode connect event -- A value of 1 indicates that the device is connected to an AP
        - STA mode disconnect event -- A value of 1 indicates that the device 
                                       is disconnected from an AP
        - STA mode -- It is not expected to receive this value as 0 when 
                      operating in STA mode
        - AP mode connect Event -- A value of 1 indicates that the SoftAP 
                                   session has been started
        - AP mode disconnect event -- A value of 1 indicates that the SoftAP 
                                      session has been stopped
        - AP mode connect event -- A value of 0 indicates that a peer station 
                                   with a MAC address in mac_Addr 
                                   has connected to SoftAP
        - AP Mode disconnect event -- A value of 0 indicates that a peer 
                                      station with a MAC address in mac_Addr 
                                      has  disconnected from SoftAP @vertspace{-14}
        */
} qapi_WLAN_Connect_Cb_Info_t;

/**
@ingroup qapi_wlan
qapi_WLAN_Connect_Cb_Info_t is not enough and cannot be changed, 
here add a new structure to report more information.
*/
typedef struct
{
    qapi_WLAN_Connect_Cb_Info_t info;               /**< The basic info. */
    const uint8_t               *req_IEs;           /**< (Re)Association Request IEs which do not include fixed fields. */
    const uint8_t               *resp_IEs;          /**< (Re)Association Response IEs which do not include fixed fields. */
    const uint8_t               *beacon_IEs;        /**< Now only include WPA or RSN IE. */
    uint8_t                     req_IEs_Len;        /**< Length of req_IEs. */
    uint8_t                     resp_IEs_Len;       /**< Length of resp_IEs. */
    uint8_t                     beacon_IEs_Len;     /**< Length of beacon_IEs. */
    uint8_t                     channel;            /**< Wireless channel. */
    uint16_t                    listen_Interval;    /**< listen interval. */
    uint16_t                    beacon_Interval;    /**< beacon interval. */
    uint8_t                     reserved[8];        /**< Reserved, 2 bytes for internal member, 2 bytes for allignment, 4 bytes for extension. */
} qapi_WLAN_Connect_Cb_Info_ExtV1_t;

/**
@ingroup qapi_wlan
Data structure used to indicate the WLAN enablement status to a coexistence subsystem.
This callback is not passed on for the application, but only for other internal 
subsystems.
*/
typedef struct //  qapi_WLAN_Ready_Cb_Info_s
{
    uint32_t numDevices;  
          /**< Supported number of virtual interfaces/devices. */
} qapi_WLAN_Ready_Cb_Info_t;

/**
@ingroup qapi_wlan
Data structure used to indicate WLAN virtual device connection/disconnection 
information to a coexistence subsystem. 
*/
typedef struct //qapi_WLAN_Connect_Coex_Cb_Info_s
{
   uint16_t       frequency_MHz;   /**< Frequency(802.11 Channel) in MHz. */
   uint8_t        chan_40Mhz;
   /**< 1: 40 MHz (in 2.4 GHz band) channel \n
        0: 20 MHz channel
   */
   uint32_t       value;
   /**< 1: Connected \n
        0: Not connected
   */
} qapi_WLAN_Connect_Coex_Cb_Info_t;

/** @cond */
/**
@ingroup qapi_wlan
Data structure used to pass data from driver to application callback after scan 
is completed
*/
typedef struct  //qapi_WLAN_Scan_Complete_Cb_Info_s
{
   uint32_t                            num_Bss_Info;   /**< Number of BSS. */
   qapi_WLAN_BSS_Scan_Info_t           bss_Info[1];    /**< BSS information */
} qapi_WLAN_Scan_Complete_Cb_Info_t;


/**
@ingroup qapi_wlan
Data structure used to pass data from driver to application callback for probe request command
*/
typedef struct //qapi_WLAN_Fwd_Probe_Request_Cb_Info_s
{
   int32_t                             frequency;           /**< Frequency. */
   int32_t                             buffer_Length;       /**< Buffer length. */
   void                                *probe_Req_Buffer;   /**< Buffer for probe request. */
} qapi_WLAN_Fwd_Probe_Request_Cb_Info_t;
/** @endcond */

/**
@ingroup qapi_wlan
Data structure that represents WLAN statistics information for each virtual device.
This data structure contains counts of various types of packets transmitted/received 
on the requested virtual device.
*/
typedef struct
{
    uint32_t unicast_Tx_Pkts;                      /**< Unicast TX packets. */
    uint32_t unicast_Rx_Pkts;                      /**< Unicast RX packets. */
    uint32_t multicast_Tx_Pkts;                    /**< Multicast TX packets. */
    uint32_t multicast_Rx_Pkts;                    /**< Multicast RX packets. */
    uint32_t broadcast_Tx_Pkts;                    /**< Broadcast TX packets. */
    uint32_t broadcast_Rx_Pkts;                    /**< Broadcast RX packets. */
    uint32_t unicast_Non_Null_Tx_Pkts;             /**< Unicast TX packets excluding NULL and QoS NULL packets. */
    uint32_t unicast_Non_Null_Rx_Pkts;             /**< Unicast RX packets excluding NULL and QoS NULL packets. */
    uint32_t unicast_Filtered_Accepted_Tx_Pkts;    /**< Unicast filtered and accepted TX packets. */
    uint32_t unicast_Filtered_Accepted_Rx_Pkts;    /**< Unicast filtered and accepted RX packets. */
    uint32_t multicast_Filtered_Accepted_Tx_Pkts;  /**< Multicast filtered and accepted TX packets. */
    uint32_t multicast_Filtered_Accepted_Rx_Pkts;  /**< Multicast filtered and accepted RX packets. */
    uint32_t broadcast_Filtered_Accepted_Tx_Pkts;  /**< Broadcast filtered and accepted TX packets. */
    uint32_t broadcast_Filtered_Accepted_Rx_Pkts;  /**< Broadcast filtered and accepted RX packets. */
    uint32_t unicast_Filtered_Rejected_Tx_Pkts;    /**< Unicast filtered and rejected TX packets. */
    uint32_t unicast_Filtered_Rejected_Rx_Pkts;    /**< Unicast filtered and rejected RX packets. */
    uint32_t multicast_Filtered_Rejected_Tx_Pkts;  /**< Multicast filtered and rejected TX packets. */
    uint32_t multicast_Filtered_Rejected_Rx_Pkts;  /**< Multicast filtered and rejected RX packets. */
    uint32_t broadcast_Filtered_Rejected_Tx_Pkts;  /**< Broadcast filtered and rejected TX packets. */
    uint32_t broadcast_Filtered_Rejected_Rx_Pkts;  /**< Broadcast filtered and rejected RX packets. */
    uint32_t null_Tx_Pkts;                         /**< NULL TX packets. */
    uint32_t null_Rx_Pkts;                         /**< NULL RX packets. */
    uint32_t qos_Null_Tx_Pkts;                     /**< QOS NULL TX packets. */
    uint32_t qos_Null_Rx_Pkts;                     /**< QOS NULL RX packets. */
    uint32_t ps_Poll_Tx_Pkts;                      /**< PS Poll TX packets. */
    uint32_t ps_Poll_Rx_Pkts;                      /**< PS Poll RX packets. */
    uint32_t tx_Retry_Cnt;                         /**< TX retry count. */
    uint32_t beacon_Miss_Cnt;                      /**< Beacon miss count. */
    uint32_t beacons_Received_Cnt;                 /**< Received beacon miss count. */
    uint32_t beacon_Resync_Success_Cnt;            /**< Beacon resync success count. */
    uint32_t beacon_Resync_Failure_Cnt;            /**< Beacon resync failure count. */
    uint32_t curr_Early_Wakeup_Adj_In_Ms;          /**< Current early wakeup adjustment. */
    uint32_t avg_Early_Wakeup_Adj_In_Ms;           /**< Average early wakeup adjustment. */
    uint32_t early_Termination_Cnt;                /**< Early termination count. */
    uint32_t uapsd_Trigger_Rx_Cnt;                 /**< UAPSD trigger RX count. */
    uint32_t uapsd_Trigger_Tx_Cnt;                 /**< UAPSD trigger TX count. */
}qapi_WLAN_Device_Stats_t;

/**
@ingroup qapi_wlan
Data structure that represents buffer inforamtion for WLAN Transmit/Receive.
The buffer information in this data structure are independent of the virtual devices.
*/
typedef struct
{
	uint8_t htc_inf_cur_cnt;		/**< The count of current available buffers in htc interface. */
	uint8_t htc_inf_reaped_cnt;	    /**< The count of reaped buffers in htc interface. */
	uint8_t mac_inf_cur_cnt;		/**< The count of current available buffers in mac interface. */
	uint8_t mac_inf_reaped_cnt;		/**< The count of reaped buffers in mac interface. */
	uint8_t fw_inf_cur_cnt;         /**< The count of current available buffers in fw interface. */ 
	uint8_t fw_inf_reaped_cnt;		/**< The count of reaped buffers in fw interface. */ 
	uint8_t free_buf_cnt;			/**< The count of free buffers in buffer pool. */	
	uint8_t mgmt_buf_cnt;			/**< The count of remaining management buffers. */	
	uint8_t smmgmt_buf_cnt;			/**< The count of remaining small management buffers. */	
	uint8_t num_txbuf_queued;		/**< The number of buffers queued for tx. */	
	uint8_t num_rxbuf_queued;		/**< The number of buffers queued for rx. */	
	uint8_t reserved;
} qapi_WLAN_Common_TxRx_Buffer_Info;

/**
@ingroup qapi_wlan
Data structure that represents system level statistics information of the WLAN 
subsystem. The statistics information in this data structure are independent of 
the virtual devices.
*/
typedef struct
{
    uint32_t total_Active_Time_In_Ms; 
       /**< Total time in milliseconds for which the WLAN subsystem has been Active. */
    uint32_t total_Powersave_Time_In_Ms; 
       /**< Total time in milliseconds for which the WLAN subsystem has been in Power Save. */
	qapi_WLAN_Common_TxRx_Buffer_Info  txrx_buffer_info; 
	   /**< The buffer information for WLAN transmit/receive. */
} qapi_WLAN_Common_Stats_t;

/**
@ingroup qapi_wlan
Data structure that represents extensional WLAN statistics information for each virtual device.
This data structure contains the number of amsdu packets received on the requested virtual device.
*/
typedef struct
{
    uint32_t rx_amsdu_pkts;      /**the number of received amsdu packets */
    uint32_t reserved; 			 /** reserved data for future */
} qapi_WLAN_Device_Stats_Ext_t;

/**
@ingroup qapi_wlan
Data structure that represents extensional WLAN statistics information for each virtual device on version 2.
*/
typedef struct
{
    uint16_t wmi_event_missed_last;             /**< The last missed WMI event. */
    uint16_t reserved;                          /**< Reserved. */
    uint32_t wmi_event_missed_bitmap;           /**< Bitmap of missed WMI events. */
    uint32_t wmi_event_missed_cnt;              /**< Count of total missed WMI events. */
} qapi_WLAN_Device_Stats_Ext2_t;

/**
@ingroup qapi_wlan
Data structure that represents system and MAC statistics of the WLAN subsytem. This 
structure includes both virtual device-specific statistics and virtual device 
independent statistics.
*/
typedef struct
{
    qapi_WLAN_Device_Stats_t dev_Stats;   /**< Per device statistics. */
    qapi_WLAN_Common_Stats_t common_Stats;  /**< Statistics common to all devices. */
	qapi_WLAN_Device_Stats_Ext_t dev_Stats_Ext; /**< Per device extensional statistics. */
	qapi_WLAN_Device_Stats_Ext2_t dev_Stats_Ext2;   /**< Per device extensional statistics for version 2. */
}qapi_WLAN_Statistics_t;

/**
@ingroup qapi_wlan
Data structure used to retrieve WLAN statistics from the driver. The application 
passes da ata structure of this type to get the statistics information by calling 
qapi_WLAN_Get_Param() with the command ID as __QAPI_WLAN_PARAM_GROUP_WIRELESS_STATS.
*/
typedef struct
{
    uint8_t                     reset_Counters_Flag;    
                      /**<
                      [IN] 1: Reset statistics counters after getting the current count; \n 
                           0: Do not reset counters
                      */
    qapi_WLAN_Statistics_t      *wlan_Stats_Data;       
                      /**<
                      [OUT] Data structure that is to be filled by the driver 
                      that actually holds the statistics information. 
                      */
}qapi_WLAN_Get_Statistics_t;

/**
@ingroup qapi_wlan
Data structure used to retrieve the current regulatory domain channel list. The application 
passes a data structure of this type to get the channel list information by calling 
qapi_WLAN_Get_Param() with the command ID as __QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS.

When using the getRegDomainChannelList() command, this data structure should be allocated from heap memory (not from the stack). 
*/

typedef struct
{
    uint16_t                     channel_List[__QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS];  /**< WLAN Channel list array */
    uint8_t                      number_Of_Channels; /**< Number of channels supported in the current regulatory setting. */                       


}qapi_WLAN_Get_Channel_List_t;

/**
@ingroup qapi_wlan
Data structure that the application uses to interpret the event payload 
information on receiving QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_CB_E from the WLAN driver.
The event data provides the result of the PNO operation.
*/
typedef struct
{
    uint8_t   profile_Matched;          
          /**<
          A value of 1 indicates that a matching profile has been found on the PNO scan. \n
          A value of 0 indicates that a matching profile has not been found on the PNO scan.
          */
    uint8_t   matched_Index;            
          /**<
          Index of a matched profile. This index number is the same number 
          given by the application when adding an AP profile to the PNO profile list.
          */
    int32_t   rssi;                     
          /**< RSSI of the AP found that corresponds to the matching profile index. */
    uint32_t  num_Fast_Scans_Remaining; /**< Number of fast scans remaining. */
} qapi_WLAN_Preferred_Network_Offload_Info_t;

/**
@ingroup qapi_wlan
Data structure that enumerates the list of WLAN subsystem events, which the 
application can chose to filter, thereby avoiding application processor wakeups 
because of these events.
*/
typedef enum
{
  QAPI_WLAN_BSSINFO_EVENTID_E               = 0x1004, /**< Event ID to filter the BSS Info event sent by the WLAN firmware. */
  QAPI_WLAN_EXTENSION_EVENTID_E             = 0x1010, /**< Event ID to filter the WMI Extension events sent by the WLAN firmware; applicable for device-0 only. */
  QAPI_WLAN_CHANNEL_CHANGE_EVENTID_E        = 0x101A, /**< Event ID to filter the channel change event sent by the WLAN firmware. */
  QAPI_WLAN_PEER_NODE_EVENTID_E             = 0x101B, /**< Event ID to filter the events sent by the WLAN firmware during an IBSS connection; applicable for device-0 only. */
  QAPI_WLAN_ADDBA_REQ_EVENTID_E             = 0x1020, /**<
                                                       Event ID to filter the event sent by the WLAN firmware notifying the host that
                                                       it has sent an ADDBA request for a BlockACK session.
                                                       */
  QAPI_WLAN_ADDBA_RESP_EVENTID_E            = 0x1021, /**<
                                                       Event ID to filter the event sent by the WLAN firmware indicating the reception
                                                       of an ADDBA response for the ADDBA request sent.
                                                       */
  QAPI_WLAN_DELBA_REQ_EVENTID_E             = 0x1022, /**< Event ID to filter a DELBA request event. */
  QAPI_WLAN_P2P_INVITE_REQ_EVENTID_E        = 0x103E, /**< Event ID to filter the events from the WLAN firmware when a P2P invitation request is received from a device; applicable for device-0 only. */
  QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID_E= 0x103F, /**< Event ID to filter the events from the WLAN firmware when a P2P invitation response is sent to a peer device; applicable for device-0 only */
  QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID_E= 0x1040, /**< Event ID to filter the events from the WLAN firmware when a P2P invitation response is received from a peer device; applicable for device-0 only. */
  QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID_E    = 0x1041, /**< Event ID to filter the events from the WLAN firmware when a P2P provision discovery response is received from a peer device; applicable for device-0 only. */
  QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID_E     = 0x1042, /**< Event ID to filter the events from the WLAN firmware when a P2P provision discovery request is received from a peer device; applicable for device-0 only. */
  QAPI_WLAN_P2P_START_SDPD_EVENTID_E        = 0x1045, /**< Event ID to filter a P2P service discovery, provision discovery start events; applicable for device-0 only. */
  QAPI_WLAN_P2P_SDPD_RX_EVENTID_E           = 0x1046, /**< Event ID to filter a P2P service discovery, provision discovery Rx events; applicable for device-0 only. */
  QAPI_WLAN_DBGLOG_EVENTID_E                = 0x3008, /**<
                                                       Event ID to filter the DBGLOG event sent by the WLAN firmware when the dbglog buffer
                                                       is full or the threshold is reached; applicable for device-0 only.
                                                       */
  QAPI_WLAN_PKTLOG_EVENTID_E                = 0x300A, /**< Event ID to filter the PKTLOG event sent by the WLAN firmware when the pktlog buffer is full; applicable for device-0 only. */
  QAPI_WLAN_WPS_PROFILE_EVENTID_E           = 0x900F, /**<
                                                       Event ID to filter events sent by the WLAN firmware when a WPS profile is received
                                                       from a successful WPS handhshake.
                                                       */
  QAPI_WLAN_FLOW_CONTROL_EVENTID_E          = 0x9014, /**< Event ID to filter the bus flow control event sent by the WLAN firmware. */
  QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID_E       = 0x901B, /**< Event ID to filter an event indicating pending authentication for a P2P group owner negotiation request received from a peer device; applicable for device-0 only. */
  QAPI_WLAN_DIAGNOSTIC_EVENTID_E            = 0x901C, /**< Event ID to filter diagnostic events sent by the WLAN firmware; applicable for device-0 only. */
  QAPI_WLAN_WNM_EVENTID_E                   = 0x9048, /**< Event ID to filter WNM events sent by the WLAN firmware. */
} qapi_WLAN_Filterable_Event_e;

/**
@ingroup qapi_wlan
Data structure that enables applications to filter unnecessary events from 
waking up the application processor.

The applications can program event filters by invoking qapi_WLAN_Set_Param() with
command ID __QAPI_WLAN_PARAM_GROUP_WIRELESS_EVENT_FILTER.

This data structure does not allow applications to incrementally add filter 
events. It is only possible to enable or disable filtering for all events at once.
*/
typedef struct
{
    qapi_WLAN_Enable_e                   action;                                     
          /**< 1 to start filtering a list of events provided, 0 to disable filtering. */
    uint32_t                             num_Events;                                 
          /**< Number of events provided by the application in event[]. */
    qapi_WLAN_Filterable_Event_e         event[__QAPI_WLAN_MAX_NUM_FILTERED_EVENTS]; 
          /**<
          Actual list of events to be filtered. The events being provided 
          should be of type #qapi_WLAN_Filterable_Event_e and the number of 
          events in this list should not exceed __QAPI_WLAN_MAX_NUM_FILTERED_EVENTS.
          */
} qapi_WLAN_Event_Filter_t;


/**
@ingroup qapi_wlan
Enumeration that provides various blocking options for qapi_WLAN_Set_Param() 
and qapi_WLAN_Get_Param() APIs when invoked by the application.
*/
typedef enum
{
  QAPI_WLAN_NO_WAIT_E  = 0,   /**< Makes a QAPI nonblocking. */
  QAPI_WLAN_WAIT_E     = 1    /**< Makes a QAPI blocking. */
} qapi_WLAN_Wait_For_Status_e;

/**
@ingroup qapi_wlan
Enumeration that provides a list of supported operating modes for each virtual device.
*/
typedef enum
{
    QAPI_WLAN_DEV_MODE_STATION_E = 0, 
        /**<
        Infrastructure non-AP Station mode, supported in both Single Device 
        mode and Concurrent mode. When operating in Concurrent mode, virtual 
        device 1 must be used for the Station mode of operation.
        */
    QAPI_WLAN_DEV_MODE_AP_E      = 1, /**< Soft-AP mode */
        /**<
        Infrastructure AP Station mode, supported in both Single Device 
        mode and Concurrent mode. When operating in Concurrent mode, virtual 
        device 0 must be used for the AP mode of operation.
        */
    QAPI_WLAN_DEV_MODE_ADHOC_E   = 2, /* Adhoc mode */
        /**<
        Independent BSS mode of operation, supported in Single Device mode 
        only. Virtual device 0 must be used for the IBSS mode of operation.
        */
    QAPI_WLAN_DEV_MODE_INVALID_E = 3  /**< Invalid device mode. */
} qapi_WLAN_Dev_Mode_e;

/**
@ingroup qapi_wlan
Enumeration that provides a list of supported 802.11 PHY modes.
*/
typedef enum
{
    QAPI_WLAN_11A_MODE_E        = 0x1,  /**< 802.11a. */
    QAPI_WLAN_11G_MODE_E        = 0x2,  /**< 802.11g. */
    QAPI_WLAN_11AG_MODE_E       = 0x3,  /**< 802.11ag. */
    QAPI_WLAN_11B_MODE_E        = 0x4,  /**< 802.11b. */
    QAPI_WLAN_11GONLY_MODE_E    = 0x5,  /**< 802.11g only. */
} qapi_WLAN_Phy_Mode_e;

/**
@ingroup qapi_wlan
Enumeration that provides 11n HT configurations.
*/
typedef enum
{
    QAPI_WLAN_11N_DISABLED_E      = 0x1,  /**< 802.11n disabled. */
    QAPI_WLAN_11N_HT20_E          = 0x2,  /**< 802.11n with bandwith 20M. */
    QAPI_WLAN_11N_HT40_E          = 0x3,  /**< 802.11n with bandwith 40M. */
} qapi_WLAN_11n_HT_Config_e;

/** @cond */
/**
@ingroup qapi_wlan
Data structure used to get firmware version information.
*/
typedef struct //qapi_WLAN_Firmware_Version_s
{
    uint32_t        host_ver;   /**< Host version. */
    uint32_t        target_ver; /**< Target version. */
    uint32_t        wlan_ver;   /**< WLAN version. */
    uint32_t        abi_ver;    /**< Device ABI version. */
}qapi_WLAN_Firmware_Version_t;
/** @endcond */

/**
@ingroup qapi_wlan
Data structure to get the WLAN host/firmware version information in string format.
*/
typedef struct //qapi_WLAN_Firmware_Version_String_s
{
    uint8_t        host_Version[__QAPI_WLAN_VERSION_SUBSTRING_LEN];   /**< Host version. */
    uint8_t        target_Version[__QAPI_WLAN_VERSION_SUBSTRING_LEN]; /**< Target version. */
    uint8_t        wlan_Version[__QAPI_WLAN_VERSION_SUBSTRING_LEN];   /**< WLAN version. */
    uint8_t        abi_Version[__QAPI_WLAN_VERSION_SUBSTRING_LEN];    /**< Device ABI version. */
}qapi_WLAN_Firmware_Version_String_t;

/**
@ingroup qapi_wlan
Data structure that enables the application to configure scan parameters for subsequent 
scan operations.
*/
typedef struct //qapi_WLAN_Scan_Params_s
{
    uint16_t                           fg_Start_Period;
    /**<
    Foreground scan start period in seconds.\n
    If this parameter is set to 0, the period is set to the default value of 1 second.\n
    If this parameter is set to 0xFFFF, the periodic background scan is disabled.
    */
    uint16_t                           fg_End_Period;
    /**<
    Foreground scan end period in seconds.\n
    If this parameter is set to 0, the period is set to the default value of 60 seconds.
    */
    uint16_t                           bg_Period;
    /**<
    Background scan period in seconds. 
    Stations can use this mode to scan for neighboring APs without disconnecting 
    from the AP to which they are currently connected.\n
    If this parameter is set to 0, the period is set to the default value of 60 seconds.\n
    If this parameter is set to 0xFFFF, the periodic background scan is disabled.
    */
    uint16_t                           max_Act_Chan_Dwell_Time_In_Ms;
    /**<
    Active channel maximum dwell time in ms. 
    If this parameter is set to 0, the default period is 20 ms.
    */
    uint16_t                           passive_Chan_Dwell_Time_In_Ms;
    /**<
    Channel dwell time for passive scan mode in which the client waits at each channel to receive beacon frames from APs within the proximity.\n
    If set to 0, the period is set to the default value of 50 ms.
	*/
    uint8_t                            short_Scan_Ratio;
    /**<
    Ratio of the number of short scans per long scan for a foreground search. The default value is 3.
	Short scan is done only in the channels with configured SSIDs, whereas long scan is done in all channels.
	*/
    uint8_t                            scan_Ctrl_Flags;
    /**<
    Scan control flags.\n
    - 0x00 -- Use the current scan control settings.
    - 0xFF -- Disable all scan control flags; this will set the flag to 0.\n
    For setting specific scan control flags, use each bit in this byte-sized field as one flag.\n
    For each bit, 1 = TRUE and 0 = FALSE.\n
    - Bit0 -- Scan is allowed during connection.\n
    - Bit1 -- Scan is allowed for the SSID station if it is already connected.\n
    - Bit2 -- Enable an active scan.\n
    - Bit3 -- Scan is allowed for roaming when a beacon misses or a low signal strength is identified.\n
    - Bit4 -- Follow the customer BSSINFO reporting rule.\n
    - Bit5 -- Device is to scan after a disconnection.\n
    - Bit6 -- Scan complete event with a cancelled status is generated when a scan is preempted before it is completed.\n
    - Bit7 -- Scanning DFS channels is to be skipped.
	*/
    uint16_t                           min_Act_Chan_Dwell_Time_In_Ms;
    /**<
    Minimum dwell time at each channel in case there is no activity during Active Scan mode.\n
    The default value is 0, where each channel follows the period defined by the maximum active channel dwell time.
    */
    uint16_t                           max_Act_Scan_Per_Ssid;
    /**< Maximum number of scans allowed per channel to search for configured SSIDs during Active mode. Default value is 1. */
    uint32_t                           max_Dfs_Chan_Act_Time_In_Ms;
    /**<
    Maximum allowed time for scanning in DFS channels. The default is 2000 ms.    
    */
} qapi_WLAN_Scan_Params_t;

/**
@ingroup qapi_wlan
Enumeration of a list of supported power modes in the WLAN subsystem.
*/
typedef enum
{
    QAPI_WLAN_POWER_MODE_REC_POWER_E = 1,  
      /**< Power Save mode that enables both MAC and system power save. */
    QAPI_WLAN_POWER_MODE_MAX_PERF_E  = 2   
      /**< Maximum performance mode that disables both MAC and system power save. */
} qapi_WLAN_Power_Mode_e; 

/**
@ingroup qapi_wlan
Enumeration of various WLAN modules that have the ability to request the necessary 
power mode for their operation. Each of the modules here has the ability to 
request Power Save mode or Maximum Performance mode. This level of granularity 
mainly helps to debug mismatches between Operating Power mode and Expected Power 
mode of the WLAN subsystem.
*/
typedef enum
{
    QAPI_WLAN_POWER_MODULE_USER_E     = 0,  
          /**< If the application requires the system to be in Maximum Performance mode. */
    QAPI_WLAN_POWER_MODULE_WPS_E      = 1,  
          /**<
          WPS handshakes should be in Maximum Performance mode 
          when the application initiates a WPS handshake. After a WPS handshake is completed, 
          the application can request Power Save mode with the source module as 
          WPS.
          */
    QAPI_WLAN_POWER_MODULE_P2P_E      = 2,
          /**<
          Power mode set by the P2P module.
          P2P handshakes should be in Maximum Performance mode 
          when the application initiates P2P control commands. After a P2P session is 
          established (Group Owner or Client mode), if the device operates in GO mode, 
          the application is expected to request Maximum Perfmance mode with the module 
          owner as P2P and is expected to be in Maximum Performance mode until the P2P session is 
          active. If the device operates in P2P Client mode, the application can 
          optionally request Power Save mode with the module owner as P2P 
          once the connection is established.
          */
    QAPI_WLAN_POWER_MODULE_SOFTAP_E   = 3,  
          /**<
          Power mode set by SoftAP module.
          Applications must operate in Maximum Performance mode as long as 
          the device is operating in SoftAP mode.
          */
    QAPI_WLAN_POWER_MODULE_SRCALL_E   = 4,  
          /**<
          Power mode request by the store recall module. At this point, the WLAN 
          driver internally uses this and the application is not expected to use it.
          */
    QAPI_WLAN_POWER_MODULE_TKIP_E     = 8,  
          /**<
          Power mode set by the TKIP countermeasure module. At this point, the WLAN 
          driver internally uses this and the application is not expected to use it.
          */
    QAPI_WLAN_POWER_MODULE_RAW_MODE_E = 9,  
          /**<
          Power mode request for sending packets in Raw mode. This is the 
          mode where the application constructs and transmits packets over the WLAN radio, 
          bypassing the IP stack and WLAN MLME layer. 
          Applications must request Maximum Performance before initiating any 
          Raw mode packet transmissions.
          */
    QAPI_WLAN_POWER_MODULE_PWR_MAX_E  = 10, /**< Maximum value for modules */
} qapi_WLAN_Power_Module_e; 

/**
@ingroup qapi_wlan
Data structure used to request the necessary operating power mode (Maximum 
Performance or Power Save (rec_power) mode).
*/
typedef struct //qapi_WLAN_Power_Mode_Params_s
{
    qapi_WLAN_Power_Mode_e        power_Mode;    /**< Power mode to be set */
    qapi_WLAN_Power_Module_e      power_Module;  /**< Module requesting power mode change */
} qapi_WLAN_Power_Mode_Params_t;

/**
@ingroup qapi_wlan
Enumumeration that identifies WLAN rates.
*/
typedef enum
{
     QAPI_WLAN_RATE_AUTO_E     = -1, /**< 1 Mbps. */
     QAPI_WLAN_RATE_1Mb_E       = 0, /**< 1 Mbps. */
     QAPI_WLAN_RATE_2Mb_E       = 1, /**< 1 Mbps. */
     QAPI_WLAN_RATE_5_5Mb_E     = 2, /**< 5 Mbps. */
     QAPI_WLAN_RATE_11Mb_E      = 3, /**< 11 Mbps. */
     QAPI_WLAN_RATE_6Mb_E       = 4, /**< 6 Mbps. */
     QAPI_WLAN_RATE_9Mb_E       = 5, /**< 9 Mbps. */
     QAPI_WLAN_RATE_12Mb_E      = 6, /**< 12 Mbps. */
     QAPI_WLAN_RATE_18Mb_E      = 7, /**< 18 Mbps. */
     QAPI_WLAN_RATE_24Mb_E      = 8, /**< 24 Mbps. */
     QAPI_WLAN_RATE_36Mb_E      = 9, /**< 36 Mbps. */
     QAPI_WLAN_RATE_48Mb_E      = 10, /**< 48 Mbps. */
     QAPI_WLAN_RATE_54Mb_E      = 11, /**< 54 Mbps. */
     QAPI_WLAN_RATE_MCS_0_20_E  = 12, /**< MCS rate index 0 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_1_20_E  = 13, /**< MCS rate index 1 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_2_20_E  = 14, /**< MCS rate index 2 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_3_20_E  = 15, /**< MCS rate index 3 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_4_20_E  = 16, /**< MCS rate index 4 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_5_20_E  = 17, /**< MCS rate index 5 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_6_20_E  = 18, /**< MCS rate index 6 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_7_20_E  = 19, /**< MCS rate index 7 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_8_20_E  = 20, /**< MCS rate index 8 for 20 Mhz channel. */
     QAPI_WLAN_RATE_MCS_9_20_E  = 21, /**< MCS rate index 9 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_10_20_E = 22, /**< MCS rate index 10 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_11_20_E = 23, /**< MCS rate index 11 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_12_20_E = 24, /**< MCS rate index 12 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_13_20_E = 25, /**< MCS rate index 13 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_14_20_E = 26, /**< MCS rate index 14 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_15_20_E = 27, /**< MCS rate index 15 for 20 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_0_40_E  = 28, /**< MCS rate index 0 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_1_40_E  = 29, /**< MCS rate index 1 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_2_40_E  = 30, /**< MCS rate index 2 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_3_40_E  = 31, /**< MCS rate index 3 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_4_40_E  = 32, /**< MCS rate index 4 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_5_40_E  = 33, /**< MCS rate index 5 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_6_40_E  = 34, /**< MCS rate index 6 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_7_40_E  = 35, /**< MCS rate index 7 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_8_40_E  = 36, /**< MCS rate index 8 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_9_40_E  = 37, /**< MCS rate index 9 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_10_40_E = 38, /**< MCS rate index 10 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_11_40_E = 39, /**< MCS rate index 11 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_12_40_E = 40, /**< MCS rate index 12 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_13_40_E = 41, /**< MCS rate index 13 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_14_40_E = 42, /**< MCS rate index 14 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MCS_15_40_E = 43, /**< MCS rate index 15 for 40 Mhz channel; not supported at this time. */
     QAPI_WLAN_RATE_MAX_E,
} qapi_WLAN_Bit_Rate_t;

/**
@ingroup qapi_wlan
Enumeration of supported DTIM policies.
*/
typedef enum
{
    QAPI_WLAN_DTIM_IGNORE_E = 0, 
          /**<
          Non-AP station wakeups only at TIM intervals; ignores 
          content after beacon (CAB).
          */
    QAPI_WLAN_DTIM_NORMAL_E = 1, /**< Non-AP station wakeups at TIM intervals. */
    QAPI_WLAN_DTIM_STICK_E  = 2, /**< Non-AP station wakeups only at DTIM intervals. */
    QAPI_WLAN_DTIM_AUTO_E   = 3  /**< Non-AP station wakeups at both TIM and DTIM intervals. */
} qapi_WLAN_DTIM_Policy_e;

/**
@ingroup qapi_wlan
Enumeration that identifies Tx wake-up policy.
*/
typedef enum
{
    QAPI_WLAN_TX_WAKEUP_ON_SLEEP_E = 1,     
        /**<
        Transmit packets can exit WLAN from the protocol power save after
        transmitting 'n' number of packets in Power Save mode. The value 
        of 'n' can be programmed through qapi_WLAN_Power_Policy_Params_t.
        */
    QAPI_WLAN_TX_WAKEUP_SKIP_ON_SLEEP_E = 2 
        /**<
        Transmit packets need not exit WLAN from protocol power save irrespective
        of the number of packets transmitted while in Power Save mode.
        */
} qapi_WLAN_TX_Wakeup_Policy_e;

/**
@ingroup qapi_wlan
Enumeration that identifies the WLAN power save event policy.
*/
typedef enum
{

    QAPI_WLAN_PS_SEND_POWER_SAVE_FAIL_EVENT_ALWAYS_E = 1, 
        /**<
        Firmware should indicate NULL frame transmission failures when 
        entering and exiting protocol power save (PM bit transitions) through 
        target events.
        */
    QAPI_WLAN_PS_IGNORE_POWER_SAVE_FAIL_EVENT_DURING_SCAN_E = 2 
        /**<
        Firmware can ignore the NULL frame transmission failures when 
        entering and exiting protocol power save (PM bit transitions).
        */
} qapi_WLAN_Power_Save_Policy_e;

/**
@ingroup qapi_wlan
Data structure to configure WLAN power policy information.

@sa
qapi_WLAN_Set_Param
__QAPI_WLAN_PARAM_GROUP_WIRELESS_POWER_MODE_POLICY
*/
typedef struct //qapi_WLAN_Power_Policy_Params_s
{  
    uint16_t                           idle_Period_In_Ms; 
          /**<
          Idle period during traffic after which WLAN can go to sleep. 
          The system default for this parameter is 200 ms.
          */
    uint16_t                           ps_Poll_Num;       
          /**<
          This parameter dictates the number of contiguous PS-Poll frames that the WLAN firmware
          is to send before sending an 802.11 NULL frame to indicate a power save exit to
          the access point. The system default for this parameter is 1.
          */
    qapi_WLAN_DTIM_Policy_e            dtim_Policy;       
          /**<
          DTIM policy to be used. The application can select one of the DTIM policies from
          qapi_WLAN_DTIM_Policy_e. The system default for this parameter is QAPI_WLAN_DTIM_NORMAL_E.
          */
    qapi_WLAN_TX_Wakeup_Policy_e       tx_Wakeup_Policy;  
          /**<
          Tx wake-up policy to be used. The application can select one of the TX wake-up policies
          from qapi_WLAN_TX_Wakeup_Policy_e. The system default for this parameter is
          QAPI_WLAN_TX_WAKEUP_ON_SLEEP_E.
          */
    uint16_t                           num_Tx_To_Wakeup;  
          /**<
          Number of contiguous transmit packets after which the WLAN firmware 
          is to exit Protocol Power Save mode. This parameter is effective 
          only if tx_Wakeup_Policy is set to QAPI_WLAN_TX_WAKEUP_ON_SLEEP_E.
          The system default for this parameter is 1.
          */
    qapi_WLAN_Power_Save_Policy_e      ps_Fail_Event_Policy;   
          /**<
          Policy to indicate a power save entry/exit failure event. The application 
          can select one of the policies from qapi_WLAN_Power_Save_Policy_e. 
          The system default for this parameter is 
          QAPI_WLAN_PS_SEND_POWER_SAVE_FAIL_EVENT_ALWAYS_E.
          */
} qapi_WLAN_Power_Policy_Params_t;

/**
@ingroup qapi_wlan
Data structure for RSSI thresholds.
*/
typedef struct //qapi_WLAN_Rssi_Threshold_Params_s
{
    uint32_t    poll_Time;                /**< Polling time as a factor of LI. */
    int16_t     threshold_Above1_Val;     /**< Lowest of the upper thresholds. */
    int16_t     threshold_Above2_Val;     /**< Higher than above1. */
    int16_t     threshold_Above3_Val;     /**< Higher than above2. */
    int16_t     threshold_Above4_Val;     /**< Higher than above3. */
    int16_t     threshold_Above5_Val;     /**< Higher than above5. */
    int16_t     threshold_Above6_Val;     /**< Highest of the upper thresholds. */
    int16_t     threshold_Below1_Val;     /**< Lowest of lower thresholds. */
    int16_t     threshold_Below2_Val;     /**< Higher than below1. */
    int16_t     threshold_Below3_Val;     /**< Higher than below2. */
    int16_t     threshold_Below4_Val;     /**< Higher than below3. */
    int16_t     threshold_Below5_Val;     /**< Higher than below4. */
    int16_t     threshold_Below6_Val;     /**< Highest of the lower thresholds. */
    uint8_t     weight;                   /**< Alpha. */
} qapi_WLAN_Rssi_Threshold_Params_t;

/**
@ingroup qapi_wlan
Data structure to be used if the application wants to perform 
a channel switch after a given number of beacon intervals.

A channel switch request can be issued only when the device is operating in SoftAP mode.

On receiving this request, the WLAN firmware starts adding channel switch 
information elements in its beacons with the necessary information until the channel 
switch operation is performed.
*/
typedef struct //qapi_WLAN_Channel_Switch_s
{
    uint32_t    channel;     
        /**< New wireless channel frequency to which the SoftAP should switch. */
    uint8_t	tbtt_Count;  
        /**< Number of beacon intervals after which the channel switch is to be performed. */
} qapi_WLAN_Channel_Switch_t;

/**
@ingroup qapi_wlan
Enumeration of various IP versions, which is used for the
TCP Keepalive offload feature.
*/
typedef enum
{
    QAPI_WLAN_IP_PROTOCOL_V4_E = 4, /**< Used to indicate IPv4 protocol. */
    QAPI_WLAN_IP_PROTOCOL_V6_E = 6  /**< Used to indicate IPv6 protocol. */
}qapi_WLAN_IP_Protocol_Type_e;

/**
@ingroup qapi_wlan
Data structure that enables the application to enable/disable the TCP Keepalive offload 
feature along with the ability to configure session-independent parameters of the 
TCP Keepalive offload feature.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_TCP_KEEPALIVE_OFFLOAD_ENABLE \n
qapi_WLAN_Set_Param
*/
typedef struct
{
    uint8_t enable;                        
        /**< Used to enable (1) or disable (0) the TCP Keepalive offload feature. */
    uint16_t keepalive_Interval;           
        /**<
        TCP Keepalive interval in seconds. The keepalive frames for every 
        configured TCP session are sent periodically with this interval as 
        periodicity.
        */
    uint16_t keepalive_Ack_Recv_Threshold; 
        /**<
        Number of keepalive frames to try before confirming that the remote connection is 
        dead if TCP acknowledgements are not received from the peer.
        */
} qapi_WLAN_TCP_Offload_Enable_t; 

/**
@ingroup qapi_wlan
Data structure to configure parameters for a TCP session
for which the keepalive transmission is to be offloaded to the firmware.

The application should ensure that maximum number of offloaded TCP KA sessions does not
exceed 3.

This data structure allows parameters to be configured for one TCP session by invoking 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_WIRELESS_TCP_KEEPALIVE_OFFLOAD_SESSION_CFG 
as the command ID.

@sa
__QAPI_WLAN_PARAM_GROUP_WIRELESS_TCP_KEEPALIVE_OFFLOAD_SESSION_CFG
qapi_WLAN_Set_Param
*/
typedef struct
{
    int32_t         sock_Id;                                   
          /**< Socket handle of the TCP session to be offloaded. */
    int16_t         src_Port;   
          /**< Source port of the TCP session in host byte order. */
    int16_t         dst_Port;             
          /**< Destination port of the TCP session in host byte order. */
    uint32_t        src_IP;           
          /**<
          Source IPv4 address of the TCP session in host byte order 
          if ip_Protocol_Type is of type IPv4.
          */
    uint32_t        dst_IP;                                    
          /**<
          Destination IPv4 address of the TCP session in host byte order 
          if ip_Protocol_Type is of type IPv4.
          */
    uint8_t         dest_MAC[__QAPI_WLAN_MAC_LEN];             
          /**< MAC address of the TCP peer that corresponds to the destination IP address. */
    int32_t         sequence_Num;                              
          /**< TCP sequence number of the TCP session at the time of enabling the KA offload. */
    int32_t         ack_Sequence_Num; 
          /**<
          TCP acknowledgement number of the TCP session at the time of enabling 
          the KA offload.
          */
    qapi_WLAN_IP_Protocol_Type_e ip_Protocol_Type;   
          /**<
          IP protocol type of the socket: IPv4/IPv6 (should be one of the 
          types in qapi_WLAN_IP_Protocol_Type_e).
          */
    uint8_t         src_IP_v6addr[__QAPI_WLAN_IPV6_ADDR_LEN];  
          /**<
          Source IPv6 address of the TCP session if ip_Protocol_Type 
          is of type IPv6.
          */
    uint8_t         dst_IP_v6addr[__QAPI_WLAN_IPV6_ADDR_LEN];  
          /**<
          Destination IPv6 address of the TCP session if ip_Protocol_Type 
          is of type IPv6.
          */
} qapi_WLAN_TCP_Offload_Config_Params_t;

/**
@ingroup qapi_wlan
Data structure to store TCP keepalive event information.

This data structure represents the session-specific information when a TCP KA
offload event is received from the WLAN firmware. The combination of 
socket ID, source port, and destination port can be used to uniquely identify a TCP 
session.
*/
typedef struct
{
    int32_t  sock_Id;             
        /**< Socket ID of the TCP session for which the event is received. */
    uint32_t sequence_Num;        
        /**<
        Updated TCP sequence number after transmitting a few TCP KA frames 
        from the firmware.
        */
    uint32_t ack_Sequence_Num;    
        /**<
        Updated TCP acknowledgement number after transmitting a few TCP KA frames 
        from the firmware.
        */
    uint16_t src_Port;            
        /**< Source port of the TCP session for which the event is received. */
    uint16_t dst_Port;
        /**< Destination port of the TCP session for which the event is received. */
    uint8_t status;               
        /**<
          Status of the TCP KA offload for this TCP session:
          - A value of 1 indicates that the peer did not transmit an acknowledgment for the keepalive 
            frames transmitted by the firmware
          - A value of 2 indicates that the TCP KA offload feature has been disabled 
            by the application. An event with this status is needed to 
            ensure that the TCP sequence number and the TCP acknowledgement number are 
            synchronized between the TCP stack in the host and the offloaded TCP KA 
            offload feature in the WLAN firmware. @vertspace{-14}
        */
} qapi_WLAN_TCP_Keepalive_Event_Info_t;

/**
@ingroup qapi_wlan
Data structure to interpret the event payload 
when the WLAN driver invokes an application-registered callback with the event ID 
QAPI_WLAN_TCP_KEEPALIVE_OFFLOAD_CB_E.

This event is received in two cases from the firmware -- when TCP ACK is 
not received from the peer for the keepalive frames sent by the firmware, and when the TCP 
KA feature is disabled by the application.
*/
typedef struct
{
    uint32_t  session_cnt;           
          /**<
          Number of TCP KA sessions for which information is available in the
          event_info parameter.
          */
    qapi_WLAN_TCP_Keepalive_Event_Info_t event_info[1];  
          /**<
          Flexible array that holds the TCP KA event information 
          for one or more TCP sessions. The number of sessions is available in 
          the session_cnt parameter.
          */
} qapi_WLAN_TCP_Keepalive_Event_t;

/**
@ingroup qapi_wlan
Data structure to customize the WLAN driver buffer pool parameters.

@sa
qapi_WLAN_Set_Param \n
__QAPI_WLAN_PARAM_GROUP_SYSTEM_DRIVER_NETBUF_POOL_SIZE
*/
typedef struct //qapi_WLAN_A_Netbuf_Pool_Config_s
{
    uint32_t pool_Size;       
        /**<
        Number of driver buffer objects the WLAN driver is to use for transmitting/receiving
        data packets over WLAN. This buffer pool is only for WLAN driver metadata and not for
        actual packet buffers. Modifying this parameter can have an impact on WLAN performance.
        The system default for this parameter is 10.
        */
    uint32_t rx_Threshold;    
        /**<
        Number of driver buffer objects to reserve for receive.
        The system default for this parameter is 2.
        */
} qapi_WLAN_A_Netbuf_Pool_Config_t;


/** @cond */

/**
@ingroup qapi_wlan
Enum for the GPIO active states that the host can configure for wake-up in the WLAN firmware. 
The values can be low or high.
*/
typedef enum
{
    QAPI_WLAN_WOW_GPIO_ACTIVE_LOW_E = 0, /**< Used to configure the GPIO active state in the WLAN firmware to wake up the host with GPIO in the active LOW state. */
    QAPI_WLAN_WOW_GPIO_ACTIVE_HIGH_E,    /**< Used to configure the GPIO active state in the WLAN firmware to wake up the host with GPIO in the active HIGH state. */
}qapi_WLAN_Wow_Gpio_Active_Edge_e;

/** 
@ingroup qapi_wlan
Enum for the GPIO trigger mechanism that the host can configure in the WLAN firmware to wake up through GPIO with an edge/level triggered mechanism.
*/
typedef enum
{
    QAPI_WLAN_WOW_GPIO_EDGE_TRIGGER_E = 0, /**< Wake up the host with GPIO in the EDGE triggered mechanism. */
    QAPI_WLAN_WOW_GPIO_LEVEL_TRIGGER_E,    /**< Wake up the host with GPIO in the LEVEL triggered mechanism. */
}qapi_WLAN_Wow_Gpio_Trigger_Policy_e;

/** @endcond */

/** 
@ingroup qapi_wlan
Enumeration of header types supported by the WLAN packet filtering feature..
*/
typedef enum
{
    QAPI_WLAN_PKT_FILTER_MAC_HEADER_E = 1, 
          /**< Used to configure the WOW/packet filter pattern for the MAC header packet. */
    QAPI_WLAN_PKT_FILTER_SNAP_E,
          /**< Used to configure the WOW/packet filter pattern for the SNAP header packet. */
    QAPI_WLAN_PKT_FILTER_ARP_E,
          /**< Used to configure the WOW/packet filter pattern for the ARP packet. */
    QAPI_WLAN_PKT_FILTER_IPV4_E,
          /**< Used to configure the WOW/packet filter pattern for the IPV4 packet. */
    QAPI_WLAN_PKT_FILTER_IPV6_E,
          /**< Used to configure the WOW/packet filter pattern for the IPV6 packet. */
    QAPI_WLAN_PKT_FILTER_ICMP_E,
          /**< Used to configure the WOW/packet filter pattern for the ICMP packet. */
    QAPI_WLAN_PKT_FILTER_IGMP_E,
          /**< Used to configure the WOW/packet filter pattern for the IGMP packet. */
    QAPI_WLAN_PKT_FILTER_ICMPV6_E,
          /**< Used to configure the WOW/packet filter pattern for the ICMPV6 packet. */
    QAPI_WLAN_PKT_FILTER_UDP_E,
          /**< Used to configure the WOW/packet filter pattern for the UDP packet. */
    QAPI_WLAN_PKT_FITLER_TCP_E,
          /**< Used to configure the WOW/packet filter pattern for the TCP packet. */
    QAPI_WLAN_PKT_FITLER_PAYLOAD_E,
          /**< Used to configure the WOW/packet filter pattern for the payload. */
} qapi_WLAN_Pkt_Filter_Type_e;

/** @cond */
/**
@ingroup qapi_wlan
Data structure to configure the WoW GPIO configuration in the WLAN firmware to wake up the host through GPIO.
*/
typedef struct //qapi_WLAN_Wow_Gpio_Config_s
{
    uint32_t                               gpio;          /**< WOW PIN number of the GPIO to be configured to wake up the host by the WLAN firmware. */
    qapi_WLAN_Wow_Gpio_Active_Edge_e       is_Active_Low; /**< GPIO active states. */
    qapi_WLAN_Wow_Gpio_Trigger_Policy_e    trigger_Mechanism; /**< GPIO trigger mechanism. */
} qapi_WLAN_Wow_Gpio_Config_t;
/** @endcond */

/** 
@ingroup qapi_wlan
Data structure to add a pattern structure for WOW and filtering.
*/
typedef struct //qapi_WLAN_Add_Pattern_s
{
    uint32_t        pattern_Index;                            /**< Pattern index. */
    uint32_t        pattern_Action_Flag;                      /**< Pattern action flag. */
    uint32_t        offset;                                   /**< Offset. */
    uint32_t        pattern_Size;                             /**< Pattern size. */
    uint16_t        header_Type;                              /**< Header type. */
    uint8_t         pattern_Priority;                         /**< Pattern priority. */
    uint8_t         pattern_Mask[__QAPI_WLAN_PATTERN_MASK];   /**< Pattern mask. */
    uint8_t         pattern[__QAPI_WLAN_PATTERN_MAX_SIZE];    /**< Pattern string. */
}qapi_WLAN_Add_Pattern_t;

/** 
@ingroup qapi_wlan
Data structure to delete a pattern structure for WOW and filtering.
*/
typedef struct //qapi_WLAN_Delete_Pattern_s
{
    uint8_t        pattern_Index;                            /**< Pattern index. */
    uint8_t        header_Type;                              /**< Header type. */
}qapi_WLAN_Delete_Pattern_t;

/** 
@ingroup qapi_wlan
Data structure to change the action for the default packet filter.
*/
typedef struct //qapi_WLAN_Change_Default_Filter_Action_s
{
    uint8_t        pattern_Action_Flag;              /**< Pattern action flag. */
    uint8_t        header_Type;                      /**< Header type. */
}qapi_WLAN_Change_Default_Filter_Action_t;

/**
@ingroup qapi_wlan
Data structure to set/get the WEP key by the key index.\n
The key can be a maximum of 13 bytes long. Each byte represents either a single ASCII character or two hex characters. 
The application is responsible for converting ASCII and hex characters into hex numbers, and vice versa, if needed.

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_WEP_KEY_PAIR
qapi_WLAN_Set_Param
qapi_WLAN_Get_Param
*/
typedef struct //qapi_WLAN_Security_Wep_Key_Pair_Params_s
{
    uint32_t   key_Index;  /**< WEP key index for which the key is provided. */
    uint32_t   key_Length; /**< Length of the WEP Key */
    int8_t     *key;       /**<
                           WEP key; memory for this key is to be provided 
                           by the application.
                           */
} qapi_WLAN_Security_Wep_Key_Pair_Params_t;

/**
@ingroup qapi_wlan
Enumeration that idetifies authentication modes supported by the WLAN subsystem. The application 
sets the required authentication from one of these modes using 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_SECURITY_AUTH_MODE as the command 
ID. 

@dependencies
Authentication mode should be set before calling qapi_WLAN_Commit() to make 
the authentication mode set effective.
*/
typedef enum
{
    QAPI_WLAN_AUTH_NONE_E            = 0, /**< Open mode authentication. */
    QAPI_WLAN_AUTH_WPA_E             = 1, 
        /**< Wi-Fi Protected Access v1 Protocol, if a preshared key (PSK) is not used (currently not supported). */
    QAPI_WLAN_AUTH_WPA2_E            = 2, 
        /**< Wi-Fi Protected Access v2 Protocol, if a PSK is not used (currently not supported). */
    QAPI_WLAN_AUTH_WPA_PSK_E         = 3, 
        /**< Wi-Fi Protected Access v1, if a PSK is used. */
    QAPI_WLAN_AUTH_WPA2_PSK_E        = 4, 
        /**< Wi-Fi Protected Access v2, if a PSK is used. */
    QAPI_WLAN_AUTH_WPA_CCKM_E        = 5, 
        /**< WPA v1 with Cisco Centralized Key Management (currently not supported). */
    QAPI_WLAN_AUTH_WPA2_CCKM_E       = 6, 
        /**< WPA v2 with Cisco Centralized Key Management (currently not supported) */
    QAPI_WLAN_AUTH_WPA2_PSK_SHA256_E = 7, 
        /**< WPA2-PSK using SHA256 (currently not supported). */
    QAPI_WLAN_AUTH_WEP_E             = 8, 
        /**< Wired Equivalent Privacy (WEP) mode of authentication. */
    QAPI_WLAN_AUTH_INVALID_E         = 9  /**< Invalid authentication method. */
} qapi_WLAN_Auth_Mode_e;

/**
@ingroup qapi_wlan
Enumeration that identifies a list of supported encryption methods. The application 
sets the required encryption from one of these modes using 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_SECURITY_AUTH_MODE as the command 
ID. 

@dependencies
Encryption mode should be set before calling qapi_WLAN_Commit() to make 
the authentication mode set effective.
*/
typedef enum
{
    QAPI_WLAN_CRYPT_NONE_E        = 0, /**< No encryption, Open mode. */
    QAPI_WLAN_CRYPT_WEP_CRYPT_E   = 1, 
        /**< WEP mode of encryption. */
    QAPI_WLAN_CRYPT_TKIP_CRYPT_E  = 2, 
        /**< Temporal Key Integrity Protocol (TKIP). */
    QAPI_WLAN_CRYPT_AES_CRYPT_E   = 3, 
        /**< Advanced Encryption Standard (AES). */
    QAPI_WLAN_CRYPT_WAPI_CRYPT_E  = 4, 
        /**< WLAN Authentication and Privacy Infrastructure; currently not supported. */
    QAPI_WLAN_CRYPT_BIP_CRYPT_E   = 5, 
        /**< Broadcast Integrity Protocol; currently not supported. */
    QAPI_WLAN_CRYPT_KTK_CRYPT_E   = 6, 
        /**< Key Transport Key; currently not supported. */
    QAPI_WLAN_CRYPT_INVALID_E     = 7  /**< Invalid encryption type. */
} qapi_WLAN_Crypt_Type_e;

/**
@ingroup qapi_wlan
Enumeration that identifies a list of supported 802.1x methods.
The application sets the required method from one of these modes
using qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_METHOD
as the command ID.

@dependencies
802.1x method should be set before calling qapi_WLAN_Commit()
to make the method set effective.
*/
typedef enum
{
    QAPI_WLAN_8021X_METHOD_UNKNOWN              = 0, /**< Unknown. */
    QAPI_WLAN_8021X_METHOD_EAP_TLS_E            = 1, /**< EAP_TLS. */
    QAPI_WLAN_8021X_METHOD_EAP_TTLS_MSCHAPV2_E  = 2, /**< EAP_TTLS_MSCHAPV2. */
    QAPI_WLAN_8021X_METHOD_EAP_PEAP_MSCHAPV2_E  = 3, /**< EAP_PEAP_MSCHAPV2. */
    QAPI_WLAN_8021X_METHOD_EAP_TTLS_MD5_E  = 4,     /**< EAP_TTLS_MD5. */
    QAPI_WLAN_8021X_METHOD_MAX  = 50,               /**< max. */
} qapi_WLAN_8021X_Method_e;

/**
@ingroup qapi_wlan
Data structure to set the 802.1x private key filename and its password.\n
Both Private_Key_filename and Private_Key_Password are ASCII.

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_8021X_PRIVATE_KEY
qapi_WLAN_Set_Param
*/
typedef struct //qapi_WLAN_Security_8021x_Private_Key_s
{
    char    *Private_Key_filename;     /**< Point to address where stores the private key filename */
    char    *Private_Key_Password;     /**< Point to address where stores the private key password */
} qapi_WLAN_Security_8021x_Private_Key_t;

/**
@ingroup qapi_wlan
Data structure to enable/disable the preferred network 
offload (PNO) feature. This data structure also allows applications to configure 
some of the profile-independent global parameters of the PNO feature.

The application sets this parameter by invoking qapi_WLAN_Set_Param() with 
__QAPI_WLAN_PARAM_GROUP_WIRELESS_PREFERRED_NETWORK_OFFLOAD_ENABLE as the command 
ID.

@dependencies
This configuration should be done before adding any PNO profiles.
*/
typedef struct
{
    uint16_t max_Num_Preferred_Network_Profiles; 
        /**< 
          Maximum number of PNO profiles that the application requires. 
          Note that this is used for memory allocation in the WLAN firmware. The application 
          must ensure that this value is always less than 15. */
    uint32_t fast_Scan_Interval_In_Ms;           
        /**< 
          PNO framework allows applications to perform PNO scans in two different 
          intervals -- fast scans and slow scans. Fast scans are performed 
          with the period of this parameter from the time PNO is enabled to
          fast_Scan_Duration_In_Ms. The periodicity of this scan is provided 
          in milliseconds.
        */
    uint32_t fast_Scan_Duration_In_Ms; 
        /**< 
          Total fast scan duration during which a scan is to be performed every 
          fast_Scan_Interval_In_Ms. This duration always starts from the time PNO 
          is enabled. The reasoning behind this fast scan logic is to perform 
          aggressive PNO scans for some time to actively search for APs. If no matching 
          profiles are found for fast scan duration, the PNO scan interval increases to 
          slow_Scan_Interval_In_Ms to perform the scan at an increased interval, thereby 
          saving power. The fast scan duration is provided in 
          milliseconds.
        */
    uint32_t slow_Scan_Interval_In_Ms;           
        /**< 
          This parameter signifies the PNO scan interval after the fast scan duration 
          in fast_Scan_Duration_In_Ms has expired. In order to make 
          PNO more power efficient, applications should ensure that this parameter is 
          expected to have a value greater than fast_Scan_Interval_In_Ms, and the 
          input is given in milliseconds.
        */
    uint8_t start_Network_List_Offload;          
        /**< Enable/disable a switch for the PNO feature; 1: Enable PNO, 0: Disable PNO. */
} qapi_WLAN_Preferred_Network_Offload_Config_t;

/**
@ingroup qapi_wlan
Data structure to set an AP profile information to search for 
when enabling the PNO feature.

This data structure is used to set the profile by passing the required profile 
information to qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_WIRELESS_PREFERRED_NETWORK_PROFILE 
as the command ID.
*/
typedef struct
{
    uint8_t         index;                          
        /**< 
          Index of the AP profile to be set. This value should start with value 
          0 (for the first profile) and be incremented as each profile is added.
          However, the applications must ensure that this value does not exceed 
          (max_Num_Preferred_Network_Profiles - 1) whereas 
          max_Num_Preferred_Network_Profiles is the parameter provided by the 
          application when enabling the PNO feature.
        */
    uint8_t         ssid_Len;                       
        /**< Length of the SSID of the AP profile that the application is looking for in the PNO scan. */
    uint8_t         ssid[__QAPI_WLAN_MAX_SSID_LEN]; 
        /**< SSID of the AP profile that the application is looking for in the PNO scan. */
    qapi_WLAN_Auth_Mode_e      auth_Mode;           
        /**< Authentication mode of the AP profile. */
    qapi_WLAN_Crypt_Type_e     encryption_Type;     
        /**< Encryption type of the AP profile. */
} qapi_WLAN_Preferred_Network_Profile_t;

/**
@ingroup qapi_wlan
Data structure to configure a device in Promiscuous mode with the necessary filters, if 
required. Promiscuous mode is only supported in virtual device 0. The maximum number 
of filters supported is __QAPI_WLAN_PROMISC_MAX_FILTER_IDX. All the necessary 
filters should be set at once before passing this data structure to 
qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_WIRELESS_ENABLE_PROMISCUOUS_MODE 
as the command ID.

@dependencies
The device should be set to Maximum Performance mode before enabling Promiscuous mode.
*/
typedef struct //qapi_WLAN_Promiscuous_Mode_Info_s
{
    uint8_t src_Mac[__QAPI_WLAN_PROMISC_MAX_FILTER_IDX][__QAPI_WLAN_MAC_LEN]; 
          /**< 
            Array of source MAC addresses that are of interest to the application. 
            The first array index here corresponds to the filter index 
            and the second index of the array holds the actual source MAC address
            on which incoming 802.11 frames are to be filtered by the firmware.
            */
    uint8_t dst_Mac[__QAPI_WLAN_PROMISC_MAX_FILTER_IDX][__QAPI_WLAN_MAC_LEN]; 
          /**< 
            Array of destination MAC addresses that are of interest to the application. 
            The first array index here corresponds to the filter index 
            and the second index of the array holds the actual destination MAC address
            on which incoming 802.11 frames are to be filtered by the firmware.
            */
    uint8_t enable; 
          /**< Promiscuous mode control; 1: Enable, 0: Disable. */
    uint8_t filter_flags[__QAPI_WLAN_PROMISC_MAX_FILTER_IDX]; 
          /**< 
            Filter flags that represent the validity of filters being 
            programmed. Promiscuous mode supports filters based on four factors: 
            source MAC, destination MAC, frame type, and frame subtype.
            The application must set the corresponding bit in this field for 
            each of the filter indexes to indicate which of the configured filters 
            are valid:
            - Set Bit 0 if the source MAC address filter is valid
            - Set Bit 1 if the destination MAC address filter is valid
            - Set Bit 2 if the frame type filter is valid
            - Set Bit 3 if the frame subtype filter is valid @vertspace{-14}
          */
    uint8_t promisc_frametype[__QAPI_WLAN_PROMISC_MAX_FILTER_IDX];  
          /**< 
            Frame type based filter. The application can choose whether it is 
            interested in an 802.11 Control/Data/Management packet type. The application 
            can only choose one of these packet types for a given filter index.
            */
    uint8_t promisc_subtype[__QAPI_WLAN_PROMISC_MAX_FILTER_IDX]; 
          /**< 
            Frame subtype based filter. The application can choose to filter frames of 
            a given subtype. This subtype filter is valid only if the frame type filter is 
            valid in the corresponding filter index. In other words, if a subtype 
            filter is set on a filter index, but the corresponding frame type filter 
            is not set, the subtype filter is not considered valid.
          */
    uint8_t promisc_num_filters;   
          /**< 
            Number of programmed promiscuous filters. This should not 
            exceed __QAPI_WLAN_PROMISC_MAX_FILTER_IDX. */
} qapi_WLAN_Promiscuous_Mode_Info_t;

/**
@ingroup qapi_wlan
Enumeration of a list of supported 802.11 header types for a Raw mode transmission.
*/
typedef enum
{
    QAPI_WLAN_RAW_MODE_HDR_TYPE_BEACON_E         = 0, 
          /**< Raw mode frame header is of type beacon. */
    QAPI_WLAN_RAW_MODE_HDR_TYPE_QOS_DATA_E       = 1, 
          /**< Raw mode frame header is of type QOS data. */
    QAPI_WLAN_RAW_MODE_HDR_TYPE_FOUR_ADDR_DATA_E = 2  
          /**< Raw mode frame header is of type 4 address data. */
} qapi_WLAN_Raw_Mode_Header_Type_e;

/**
@ingroup qapi_wlan
Data structure that the application is to pass when invoking qapi_WLAN_Raw_Send() 
to transmit a raw frame.
*/
typedef struct //qapi_WLAN_Raw_Send_Params_s
{
    uint8_t                           rate_Index;  
          /**< 0: 1 Mbps, 1: 2 Mbps, 2: 5.5 Mbps, etc. */
    uint8_t                           num_Tries; 
          /**< Packet transmission count: 1 to 14. */
    uint32_t                          payload_Size; /**< Payload size: 0 to 1400. */
    uint32_t                          channel;  
          /**< Channel; 0 to 11. 0: Send on the current channel. */
    qapi_WLAN_Raw_Mode_Header_Type_e  header_Type; 
          /**< 0: Beacon frame, 1: QoS data frame, 2: Four addresses data frame. */
    uint16_t                          seq;  
          /**< Sequence number to be filled in the 802.11 header. */
    uint8_t                           addr1[__QAPI_WLAN_MAC_LEN];  /**< Address 1. */
    uint8_t                           addr2[__QAPI_WLAN_MAC_LEN];  /**< Address 2. */
    uint8_t                           addr3[__QAPI_WLAN_MAC_LEN];  /**< Address 3. */
    uint8_t                           addr4[__QAPI_WLAN_MAC_LEN];  /**< Address 4. */
    uint32_t                          data_Length;   
          /**< Size of the data to be transmitted. */
    uint8_t                           *data;   /**< Data. */
} qapi_WLAN_Raw_Send_Params_t;

/** Size of the WLAN PMKID in bytes. */
#define __QAPI_WLAN_PMKID_LEN  16

/**
@ingroup qapi_wlan
Enumeration the enable/disable options for WLAN PMKID.
*/
typedef enum {
   QAPI_WLAN_PMKID_DISABLE_E = 0,   /**< Disable the WLAN PMKID. */
   QAPI_WLAN_PMKID_ENABLE_E  = 1,   /**< Enable the WLAN PMKID. */
} qapi_WLAN_PMKID_ENABLE_e;

/**
@ingroup qapi_wlan
Data structure that the application is to set pmkid.
*/
typedef struct {
    uint8_t     bssid[__QAPI_WLAN_MAC_LEN]; /**< bssid of the wlan connection. */
    uint8_t     enable;                     /**< qapi_WLAN_PMKID_ENABLE_e. */
    uint8_t     pmkid[__QAPI_WLAN_PMKID_LEN];   /**< pmkid of the wlan connection. */
} qapi_WLAN_Set_PMKID_Params_t;

/**
@ingroup qapi_wlan
Enumeration that provides various EAPOL key type for qapi_WLAN_RxEapolKey_Cb_Info_t.
*/
typedef enum {
    QAPI_EAPOL_KEY_TYPE_WPA2 = 0x02,    /**< WPA2 key type. */
    QAPI_EAPOL_KEY_TYPE_WPA  = 0xFE,    /**< WPA key type. */
} qapi_EAPOL_KEY_TYPE_e;

/**
@ingroup qapi_wlan
Data structure that presents rx_eapol_key event information from the driver to the
application.

The application uses this data structure to interpret the event
payload received with a QAPI_WLAN_RX_EAPOL_KEY_CB_E event.
*/
typedef struct
{
    uint8_t                 descType;       /**< Eapol key type, qapi_EAPOL_KEY_TYPE_e. */
    uint8_t                 keyInfo[2];     /**< Key information, big endian. */
    uint8_t                 pmkid_valid;    /**< Is pmkid valid. */
    uint8_t                 rsrv[4];        /**< Reserved. */
    uint8_t                 pmkid[__QAPI_WLAN_PMKID_LEN];   /**< Pmkid. */
} qapi_WLAN_RxEapolKey_Cb_Info_t;

/**
@ingroup qapi_wlan
Enumeration that identifies the action to be taken after a WPS initial request succeeds.
*/
typedef enum
{
    QAPI_WLAN_WPS_NO_ACTION_POST_CONNECT_E        = 0, /**< No action to be taken after WPS initiation succeeds. */
    QAPI_WLAN_WPS_CONNECT_REGISTRAR_IN_ENROLLEE_E = 1  /**< Set up a connection after WPS initiation succeeds. */
} qapi_WLAN_WPS_Connect_Action_e;

/**
@ingroup qapi_wlan
Enumeration of supported WPS modes.
*/
typedef enum
{
    QAPI_WLAN_WPS_PIN_MODE_E = 0, /**< WPS Pushbutton method. */
    QAPI_WLAN_WPS_PBC_MODE_E = 1  /**< WPS PIN method. */
} qapi_WLAN_WPS_Mode_e;

/**
@ingroup qapi_wlan
Data structure to pass WPS credentials from the driver to the application.
*/
typedef struct //qapi_WLAN_WPS_Credentials_s
{
    uint16_t                           ap_Channel;  /**< AP's channel. */
    uint8_t                            ssid[__QAPI_WLAN_MAX_SSID_LEN];  /**< SSID. */
    uint8_t                            ssid_Length;   /**< SSID length. */
    qapi_WLAN_Auth_Mode_e              auth_Mode;    /**< Authentication mode. */
    qapi_WLAN_Crypt_Type_e             encryption_Type;   /**< Encryption type. */
    uint8_t                            key_Index; /**< Index of the key. */
    uint8_t                            key[__QAPI_WLAN_WPS_MAX_KEY_LEN+1];  /**< Key. */
    uint8_t                            key_Length; /**< Key length. */
    uint8_t                            mac_Addr[__QAPI_WLAN_MAC_LEN];   /**< MAC address. */
} qapi_WLAN_WPS_Credentials_t;

/**
@ingroup qapi_wlan
Enumeration that identifies the types of WPS command results.
*/
typedef enum
{
    QAPI_WLAN_WPS_STATUS_SUCCESS = 0x0,      /**< WPS succeeded. */
    QAPI_WLAN_WPS_STATUS_FAILURE = 0x1,      /**< WPS failed. */
    QAPI_WLAN_WPS_STATUS_IDLE = 0x2,         /**< WPS in the Idle state. */
    QAPI_WLAN_WPS_STATUS_IN_PROGRESS  = 0x3  /**< WPS in progress. */
} qapi_WLAN_WPS_Status_Code_e;

/**
@ingroup qapi_wlan
Data structure to get WPS command results from the driver.
*/
typedef struct //qapi_WLAN_WPS_Cb_Info_s
{
  uint8_t status;                               /**< WPS status. */
  uint8_t error;                                /**< WPS error. */
  qapi_WLAN_WPS_Credentials_t  credential;      /**< WPS credentials. */
  uint8_t  peer_dev_addr[__QAPI_WLAN_MAC_LEN];  /**< MAC address of a peer device. */
} qapi_WLAN_WPS_Cb_Info_t;

/**
@ingroup qapi_wlan
Data structure for A-MPDU enablement. A-MPDU aggregation is enabled on a per-TID basis, 
where each TID (0-7) represents a different traffic priority.

The mapping to WMM access categories is as follows:
         - WMM best effort = TID 0-3
         - WMM background  = TID 1-2
         - WMM video       = TID 4-5
         - WMM voice       = TID 6-7

Once enabled, A-MPDU aggregation may be negotiated with an access point/Peer
device and then both devices may optionally use A-MPDU aggregation for
transmission. Due to other bottle necks in the data path, a system may not
get improved performance by enabling A-MPDU aggregation.
*/
typedef struct //qapi_WLAN_Aggregation_Params_s
{
    uint16_t                       tx_TID_Mask; /**< Bitmask to enable Tx A-MPDU aggregation. */
    uint16_t                       rx_TID_Mask; /**< Bitmask to enable Rx A-MPDU aggregation. */
} qapi_WLAN_Aggregation_Params_t;

/**
@ingroup qapi_wlan
Enumeration that identifies results of WPS pushbutton/WPS PIN operations.
*/
typedef enum
{
    QAPI_WLAN_WPS_ERROR_SUCCESS_E = 0x0,          /**< WPS initiation succeeded. */
    QAPI_WLAN_WPS_ERROR_INVALID_START_INFO_E,     /**< Invalid information given to initialize WPS. */
    QAPI_WLAN_WPS_ERROR_MULTIPLE_PBC_SESSIONS_E,  /**< Multiple WPS pushbutton sessions. */
    QAPI_WLAN_WPS_ERROR_WALKTIMER_TIMEOUT_E,      /**< WPS walktimer expired. */
    QAPI_WLAN_WPS_ERROR_M2D_RCVD_E,               /**< M2D message received. */
    QAPI_WLAN_WPS_ERROR_PWD_AUTH_FAIL_E,          /**< Authentication failed. */
    QAPI_WLAN_WPS_ERROR_CANCELLED_E,              /**< WPS cancelled. */
    QAPI_WLAN_WPS_ERROR_INVALID_PIN_E             /**< Incorrect WPS PIN. */
} qapi_WLAN_WPS_Error_Code_e;

/**
@ingroup qapi_wlan
Data structure to store results of a scan.
*/
typedef struct //qapi_WLAN_Scan_List_s
{
    uint32_t num_Scan_Entries;   /**< Number of scan results. */
    void* scan_List;             /**< Scan results. */
} qapi_WLAN_Scan_List_t;

/**
@ingroup qapi_wlan
Data structure to get the Tx pipe status.
*/
typedef struct //qapi_WLAN_Tx_Status_s
{
    uint16_t status; /**< One of QAPI_WLAN_TX_STATUS_* types. */
} qapi_WLAN_Tx_Status_t;

/**
@ingroup qapi_wlan
Enum that identifies the results of the command to set the MAC address.
*/
typedef enum
{
    QAPI_WLAN_SET_MAC_RESULT_SUCCESS_E = 1,
    /**< Successfully (re)programmed the MAC address into the Wi-Fi device. */

    QAPI_WLAN_SET_MAC_RESULT_DEV_DENIED_E,
    /**<
    Device denied the operation for several possible reasons, the most
    of which is that the MAC address equals the
    current MAC address that is already in the device. An invalid
    MAC address value can also cause this result.
    */

    QAPI_WLAN_SET_MAC_RESULT_DEV_FAILED_E,
    /**<
    Device tried but failed to program the MAC address. An error occurred
    on the device as it tried to program the MAC address.
    */

    QAPI_WLAN_SET_MAC_RESULT_DRIVER_FAILED_E
    /**<
    Driver tried but failed to program the MAC address. Possibly, the driver
    did not have the proper code compiled to perform this operation.
    */
} qapi_WLAN_Mac_Result_e;

/**
@ingroup qapi_wlan
Data structure to store data for setting the MAC address command.
*/
typedef struct //qapi_WLAN_Program_Mac_Addr_Param_s
{
    uint8_t addr[6]; /**< MAC address. */
    uint8_t result;  /**< Result of the command. */
} qapi_WLAN_Program_Mac_Addr_Param_t;

/**
@ingroup qapi_wlan
Enumeration to configure the debug port for the debug logs in the firmware.

The target can send the captured debug logs either on its UART (local) or 
to the host, which in turn sends on its configured debug UART when connected to a PC,
where an external tool (QDL) that is running can capture and parse the dbglogs.
*/
typedef enum 
{
    QAPI_WLAN_DBGLOG_LOCAL_PORT_E = 0,  
       /**< 
           Used when logs are needed on the target (Kingfisher) UART. 
       */
    QAPI_WLAN_DBGLOG_REMOTE_PORT_E = 1, 
       /**< 
           Used when logs are needed on a host that is connected to a PC running 
           an external tool (QDL). 
       */
} qapi_WLAN_Dbglog_Port_e;

/**
@ingroup qapi_wlan
Data structure to enable/disable logging in the target.

This data structure enables the DBGLOG feature and configures it with the default configuration 
for dbglogs. The default dbglog configuration enables the 
loglevel ERROR logs for all modules, configures a remote debug port,
enables reporting, sets the report size to 0 (send to host only when the buffer is full), 
and sets the time resolution to zero.
*/
typedef struct
{
    qapi_WLAN_Enable_e enable; /**< 1:Enable, 0:Disable */
} qapi_WLAN_Dbglog_Enable_t;


/**
@ingroup qapi_wlan
Data structure to modify the dbglog log level configuration for one or 
more modules in the target to capture the logs accordingly.

There are a total of 64 modules in the target to which the debug logs belong.
Specify the module ID mask for which the log level is to be configured/modified.
Update the log level buffer with log level mask information at the proper index 
and position for each of the modules in the module ID mask specified.

The log level mask is 4 bits in length and is as following: \n
  Bit0 -- INFO;, Bit1 -- LOW; Bit2 -- HIGH; Bit3 -- ERROR.

Each module occupies a nibble space in the log level buffer to store its 
log level mask at the index calculated using its module ID.
*/
typedef struct
{
    uint64_t    module_Id_Mask;  
        /**< 
            Module ID mask to configure/modify the log level for 
            intended modules. 
        */
    uint32_t    log_Level[__QAPI_WLAN_DBGLOG_LOGLEVEL_INFO_LEN];
        /**< 
             Contains the log level information for the modules in the module ID mask 
             specified with the intended log level for each module. 
        */
} qapi_WLAN_Dbglog_Module_Config_t;

/** 
As part of misc_Config, start bit offset of flush, to be used to flush the dbglogs in current buffer to the host.
*/
#define __QAPI_WLAN_DBGLOG_MISC_CONFIG_FLUSH_FLAG_BIT_OFFSET    0

/** 
As part of misc_Config, range mask of flush, to be used to flush the dbglogs in current buffer to the host.
*/
#define __QAPI_WLAN_DBGLOG_MISC_CONFIG_FLUSH_FLAG_MASK              0x01

/**
@ingroup qapi_wlan
Data structure to modify the dbglog configuration parameters in the target 
to capture the logs accordingly.
*/
typedef struct
{
    qapi_WLAN_Dbglog_Port_e    debug_Port;	
        /**< 
            Used to configure the debug port on which the dbglogs is to 
            be sent in the target.

            The target can send the captured debug logs either on its UART (local) or
            to the host, which in turn sends the logs on its configured debug UART when connected to a PC
            that has an external tool (QDL) running, which can capture and parse the dbglogs.
        */
    qapi_WLAN_Enable_e	       reporting_Enable;  
        /**< 
          Used to control the reporting of debug logs to the host or not when 
          the debug port is configured as a remote UART.

          When the remote debug port is enabled, the host can configure the target to
          report the logs to the host or not. If reporting is enabled, the target can
          send logs to host when the buffer is full or when the reporting size is met,
          otherwise the logs keep getting overwritten in the target dbglog buffer with
          the new logs until reporting is enabled by the host.
        */
    uint32_t                   report_Trigger_Size_In_Bytes; 
        /**< 
          Used to configure number of debug messages after which the firmware 
          sends the dbglogs to the host. By default, the firmware waits 
          until the buffer is full.

          When the remote debug port is enabled, by default, the logs are sent to the
          host when the dbglog buffer in the target becomes full. The host can configure
          the report size for the number of dbglog messages the target
          can send logs to the host. The minimun number of messages expected is
          atleast 15 to avoid frequent events to the host.
        */
    uint32_t                   misc_Config;
        /**< 
          Miscellaneous configuration for wlan dbglog.
        */
} qapi_WLAN_Dbglog_Config_t;

/** @cond EXPORT_PKTLOG */
/**
@ingroup qapi_wlan
Data structure to enable/disable pktlog in the target.
*/
typedef struct //qapi_WLAN_Pktlog_Enable_s
{
    qapi_WLAN_Enable_e         enable;  
            /**< Used to enable/disable packet logs. */
    uint8_t                    num_Of_Buffers; 
            /**< 
                Used to specify the number of buffers to be allocated in the target 
                for packet logging. The default is 4 and the maximum value is 10. 
            */
}qapi_WLAN_Pktlog_Enable_t;

/**
@ingroup qapi_wlan
Data structure to start packet logging for selected events with 
selected options in the target.

Packet logging is time-based for the options throughput and PER. The trigger interval and 
trigger threshold must be set for this. For the options throughput and PER, 
packet logging stops either when a trigger threshold is reached or when a trigger interval 
has elapsed.
*/
typedef struct
{
    uint8_t 	event_List;         
            /**< 
              Event mask used to configure the events for which packet logging is
              to be captured in the firmware.

              There are different events for which packet logs can be captured as required.
              Packets for RX, TX, Rate Ctrl find, and Rate Ctrl update events can be captured.
              Event masks must be configured for required events as follows: \n
              Bit0 -- RX; Bit1 -- TX; Bit2 -- RC find; Bit3 -- RC update.
              */
    uint8_t 	log_Options;        
            /**< 
              Options mask to configure the options for which packet logging is
              to be captured in the firmware.

              There are different options for which the packet logs can be configured.
              Log options for protocol information, throughput information, and PER information can be configured.
              TCP SACK, PHYERR, and DIAGNOSTICS options are currently not supported.
              The log options mask must be configured for required options as follows: \n
              Bit0 -- Proto; Bit1 -- SACK; Bit2 -- Throughput; Bit3-- PER,
              Bit4 -- PHYERR; Bit5 -- DIAGNOSTIC.
              */
    uint32_t	trigger_Threshold;  
            /**< 
              The minimum threshold value that must be reached before the packet 
              logging can stop for PER/Throughput options.
              */
    uint32_t	trigger_Interval;   
            /**< 
              The time for which packet logging captures the packets for 
              PER/Throughput options.

              This is the minimum interval for which the pktlog must do capturing before 
              it stops if the threshold set is not met.
            */
    uint32_t	trigger_Tail_Count; 
            /**< 
              The number of more packets to be captured even after the 
              threshold has been reached for context information for the SACK option,
              which is currently not supported. 
            */
    uint32_t	buffer_Size;		/**< Buffer size; currently not used. */
}qapi_WLAN_Pktlog_Start_Params_t;
/** @endcond */


/** @cond */
/**
@ingroup qapi_wlan
Data structure to pass command data from the application to the driver for getting driver register information.
*/
typedef struct
{
    uint32_t address;     /**< Memory address. */
    uint32_t value;       /**< Value. */
    uint32_t mask;        /**< Mask. */
    uint32_t size;        /**< Size. */
    uint32_t operation;   /**< Operation. */
} qapi_WLAN_Driver_RegQuery_Params_t;

/**
@ingroup qapi_wlan
Enumeration that identifies driver register operations.
*/
typedef enum
{
    QAPI_WLAN_REG_OP_READ = 1, /**< Read. */
    QAPI_WLAN_REG_OP_WRITE,    /**< Write. */
    QAPI_WLAN_REG_OP_RMW       /**< Read-Modify-Write. */
} qapi_WLAN_Driver_RegQuery_Params_e;
/** @endcond */

/**
@ingroup qapi_wlan
Enumeration that identifies COEX enable/disable operations.
*/
typedef enum
{
    QAPI_WLAN_COEX_DISABLE_E = 0, /**< Disable coexistence. */
    QAPI_WLAN_COEX_ENABLED_E      /**< Enable coexistence. */
} QAPI_WLAN_Coex_Enable_Disable_e;

/**
@ingroup qapi_wlan
Enumeration that identifies COEX modes.
*/
typedef enum
{
    QAPI_WLAN_COEX_3_WIRE_MODE_E = 0, /**< 3-wire mode: An unslotted mode of coex. */
    QAPI_WLAN_COEX_PTA_MODE_E,        /**< PTA mode: A slotted mode of coex. */
    QAPI_WLAN_COEX_EPTA_MODE_E,       /**< EPTA mode: 3-wire mode interacting with an external PTA device.  */
} QAPI_WLAN_Coex_Mode_e;

/**
@ingroup qapi_wlan
Enumeration that identifies the number of COEX priority levels.
*/
typedef enum
{
    QAPI_WLAN_COEX_NUM_PRIORITY_LEVELS_TWO_E = 0, /**< Two levels of priority; uses one coex signal. */
    QAPI_WLAN_COEX_NUM_PRIORITY_LEVELS_FOUR_E     /**< Four levels of priority; uses two coex signals. */
}QAPI_WLAN_Coex_Num_Priority_Levels_e;

/**
@ingroup qapi_wlan
Enumeration that identifies COEX priority thresholds.
*/
typedef enum
{
    QAPI_WLAN_COEX_WGHT_ONLY_SLAVE_TRAFFIC_GE_PRI_0_E = 0, /**< Coex priority threshold level 0. */
    QAPI_WLAN_COEX_WGHT_ONLY_SLAVE_TRAFFIC_GE_PRI_1_E,     /**< Coex priority threshold level 1. */
    QAPI_WLAN_COEX_WGHT_ONLY_SLAVE_TRAFFIC_GE_PRI_2_E,     /**< Coex priority threshold level 2. */
    QAPI_WLAN_COEX_WGHT_ONLY_SLAVE_TRAFFIC_GE_PRI_3_E,     /**< Coex priority threshold level 3. */
}QAPI_WLAN_Coex_Priority_Level_Threshold_e;

/**
@ingroup qapi_wlan
Enumeration that identifies the COEX priority.
*/
typedef enum
{
    QAPI_WLAN_COEX_PRIORITY_LOW_E = 0,  /**< Low priority. */
    QAPI_WLAN_COEX_PRIORITY_HIGH_E      /**< High priority. */
}QAPI_WLAN_Coex_Wlan_Priority_e;

/**
@ingroup qapi_wlan
Enum values for the profileType field of the qapi_WLAN_Coex_Stats_t struct.
Enum values for the coex_Profile field of the qapi_WLAN_Coex_Config_Data_t struct.
*/
typedef enum {
    QAPI_BT_PROFILE_UNDEF = 0,
    QAPI_BT_PROFILE_SCO,             /**< SCO stream. */
    QAPI_BT_PROFILE_A2DP,            /**< A2DP stream. */
    QAPI_BT_PROFILE_SCAN,            /**< BT discovery or page. */
    QAPI_BT_PROFILE_ESCO,            /**< ESCO stream. */
    QAPI_BT_PROFILE_HID,             /**< HID stream. */
    QAPI_BT_PROFILE_PAN,             /**< PAN stream -- Reserved. */
    QAPI_BT_PROFILE_RFCOMM,          /**< RFCOMM stream -- Reserved. */
    QAPI_BT_PROFILE_LE,              /**< BLE/15.4 stream. */
    QAPI_BT_PROFILE_SDP,             /**< SDP stream -- Reserved. */
    QAPI_BT_PROFILE_PAGESCAN,        /**< PAGE/SCAN stream -- Reserved. */
    QAPI_BT_PROFILE_HIGH_CMD,        /**< Yield to all narrowband -- Reserved. */
    QAPI_BT_PROFILE_802_15_4,        /**< 15.4 stream -- Reserved. */
    QAPI_BT_PROFILE_MESH,            /**< Mesh configuration -- Reserved. */
	QAPI_BT_PROFILE_MAX
} qapi_Bt_Profile_e;

/**
@ingroup qapi_wlan
Enum values for the coex_Antenna_Config field of the qapi_WLAN_Coex_Config_Data_t struct.
*/
typedef enum {
    QAPI_BT_FE_ANT_NOT_ENABLED = 0,
    QAPI_BT_FE_ANT_SINGLE = 1,        /**< Single, shared antenna: WLAN and narrowband share the same antenna. */
    QAPI_BT_FE_ANT_DUAL = 2,          /**< Dual antennas: WLAN and narrowband use separate antennas. */
    QAPI_BT_FE_ANT_DUAL_HIGH_ISO = 3, /**< Dual antennas: WLAN and narrowband use separate antennas with high isolation. */
    QAPI_BT_FE_ANT_TYPE_MAX
} qapi_Bt_Fe_Ant_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex conguration parameters.
*/
typedef struct
{
    QAPI_WLAN_Coex_Enable_Disable_e enable_Disable_Coex;           /**< Enable/disable COEX. */
    QAPI_WLAN_Coex_Mode_e coex_Mode;                               /**< 3-wire, PTA or EPTA mode. */
    uint32_t coex_Profile_Specific_Param1;                          /**< 1st Additional parameter value related to the profile specified. */
    uint32_t coex_Profile;                                         /**< Narrowband profile in use. */
    uint8_t coex_Profile_Specific_Param2;                          /**< 2nd Additional parameter value related to the profile specified. */
    uint8_t coex_Profile_Specific_Param3;                          /**< 3rd Additional parameter value related to the profile specified. */
    uint16_t reserved;
} qapi_WLAN_Coex_Config_Data_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex SCO operational mode conguration parameters.
*/
typedef struct
{
    uint32_t scoSlots;   
    /**< Number of SCO Tx/Rx slots. \n
         HVx, EV3, 2EV3 = 2. */

    uint32_t scoIdleSlots;
    /**< Number of Bluetooth idle slots between
         consecutive SCO Tx/Rx slots. \n
         HVx, EV3 = 4; 2EV3 = 10. */

    uint32_t scoFlags;
    /**< SCO options flag bits: 
         - 0 -- Allow close range optimization
         - 1 -- Is EDR is capable
         - 2 -- Is a colocated BT role master
         - 3 -- Firmware determines the periodicity of SCO
         - 4 -- No stomping BT during WLAN scan/connection @vertspace{-14} */

    uint32_t linkId;
    /**< Reserved. */

    uint32_t  scoCyclesForceTrigger;
    /**< Number of SCO cycles after which
         a PS-poll is forced; \n
         default = 10. */

    uint32_t scoDataResponseTimeout;
    /**< Timeout waiting for a downlink packet
         in response to a PS-poll; default = 20 ms. */

    uint32_t scoStompDutyCyleVal;
    /**< Reserved. */

    uint32_t scoStompDutyCyleMaxVal;
    /**< Reserved. */

    uint32_t scoPsPollLatencyFraction;
    /**< Fraction of an idle period within which
         additional PS-polls can be queued:
         - 1 -- 1/4 of idle duration
         - 2 -- 1/2 of idle duration
         - 3 -- 3/4 of idle duration
         - Default = 2 (1/2) @vertspace{-14} */

    uint32_t scoStompCntIn100ms;
    /**< Maximum number of SCO stomps in 100 ms allowed in
         Opt mode (force awake). If this exceeds the configured value,
         switch to PS-poll mode; default = 3. */

    uint32_t scoContStompMax;
    /**< Maximum number of continuous stomps allowed in Opt mode (force awake).
         If exceeded, switch to PS-poll mode; default = 3. */

    uint32_t scoMinlowRateMbps;
    /**< Low rate threshold. */

    uint32_t scoLowRateCnt;
    /**< Number of low rate packets (< scoMinlowRateMbps) allowed in 100 ms.
         If exceeded, switch to PS-poll mode or, if lower, stay in Opt mode;
         default = 36. */

    uint32_t scoHighPktRatio;
    /**< Total Rx packets in 100 ms + 1 or
         ((Total Tx packets in 100 ms - the number of high rate packets in 100 ms) + 1) in 100 ms.
         If exceeded, switch to or stay in Opt mode (force awake) and, if lower, switch to or stay in
         PS-poll mode; default = 5 (80% of high rates). */

    uint32_t scoMaxAggrSize;
    /**< Maximum number of Rx subframes allowed in this mode. The firmware renegogiates
         the maximum number of aggregates if it was negotiated to a higher value;
         default = 1. \n
         The recommended value basic rate for headsets = 1, EDR (2-EV3) = 4. */
    uint32_t NullBackoff;
    /**< Number of microseconds the NULL frame should go out before the next SCO slot. */

    uint32_t scanInterval;
    uint32_t maxScanStompCnt;
} qapi_WLAN_Coex_Sco_Config_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex A2DP operational mode conguration parameters.
*/
typedef struct
{
    uint32_t a2dpFlags;
    /**< A2DP option flags:
         - Bit 0 -- Allow close range optimization
         - Bit 1 -- Is EDR capable
         - Bit 2 -- Is a colocated BT role master
         - Bit 3 -- A2DP traffic is high priority
         - Bit 4 -- Firmware detects the role of Bluetooth
         - Bit 5 -- No stomping BT during WLAN scan/connection @vertspace{-14}  */
    uint32_t linkId;
    /**< Reserved. */

    uint32_t a2dpWlanMaxDur;
    /**< Maximum time the firmware uses the medium for
         WLAN after it identifies the idle time;
         default = 30 ms. */

    uint32_t a2dpMinBurstCnt;
    /**< Minimum number of Bluetooth data frames
         to replenish the WLAN usage limit; default = 3. */

    uint32_t a2dpDataRespTimeout;
    /**< Maximum duration the firmware waits for a downlink
         by stomping on Bluetooth
         after PS-poll is acknowledged;
         default = 20 ms. */

    uint32_t a2dpMinlowRateMbps;
    /**< Low rate threshold. */

    uint32_t a2dpLowRateCnt;
    /**< Number of low rate packets (< a2dpMinlowRateMbps) allowed in 100 ms.
         If exceeded, switch to or stay in PS-poll mode; if lower, stay in Opt mode (force awake);
         default = 36. */

    uint32_t a2dpHighPktRatio;
    /**< (Total Rx pkts in 100 ms + 1)/
         ((Total Tx pkts in 100 ms - No of high rate pkts in 100 ms) + 1) in 100 ms.
         If exceeded, switch to or stay in Opt mode (force awake) and, if lower, switch to or stay in PS-poll mode;
         default = 5 (80% of high rates). */

    uint32_t a2dpMaxAggrSize;
    /**< Maximum number of Rx subframes allowed in this mode. The firmware renegogiates
         the maximum number of aggregates if it was negogiated to a higher value;
         default = 1. \n
         The recommended value for basic rate headsets = 1; EDR (2-EV3) = 8. */

    uint32_t a2dpPktStompCnt;
    /**< Number of A2DP packets that can be stomped per burst; \n
         default = 6. */
} qapi_WLAN_Coex_A2dp_Config_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex ACL operational mode conguration parameters.
*/
typedef struct {
    uint32_t aclWlanMediumDur;
    /**< WLAN usage time during ACL (non-A2DP)
         coexistence; default = 30 ms. */

    uint32_t aclBtMediumDur;
    /**< BT usage time during ACL coexistence;
         default = 30 ms. */

    uint32_t aclDetectTimeout;
    /**< BT activity observation time limit.
         In this time duration, the number of BT packets are counted.
         If the count reaches the "aclPktCntLowerLimit" value
         for the "aclIterToEnableCoex" iteration continuously,
         the firmware goes into ACL Coexistence mode.
         Similarly, if the BT traffic count during ACL coexistence
         has not reached "aclPktCntLowerLimit" continuously
         for "aclIterToEnableCoex", ACL coexistence is
         disabled; default = 100 ms. */

     uint32_t aclPktCntLowerLimit;
     /**< ACL packet count to be received in the duration of
          "aclDetectTimeout" for
          "aclIterForEnDis" times to enable ACL coex.
          Similar logic is used to disable ACL coexistence.
          (If the "aclPktCntLowerLimit" count of ACL packets
          is not seen by "aclIterForEnDis",
          ACL coexistence is disabled);
          default = 10. */

     uint32_t aclIterForEnDis;
     /**< Number of iterations of "aclPktCntLowerLimit" for enabling and
          disabling ACL coexistence;
          default = 3. */

     uint32_t aclPktCntUpperLimit;
     /**< Upper bound limit; if there is more than
          "aclPktCntUpperLimit" seen in "aclDetectTimeout",
          ACL coexistence is enabled immediately;
          default = 15. */

    uint32_t aclCoexFlags;
    /**< A2DP option flags:
         - Bit 0 -- Allow close range optimization
         - Bit 1 -- Disable firmware detection
         - Bit 2 -- No stomping BT during WLAN scan/connection
         (The currently supported configuration is aclCoexFlags = 0). */

    uint32_t linkId;
    /**< Reserved. */

    uint32_t aclDataRespTimeout;
    /**< Maximum duration the firmware waits for a downlink
         by stomping on Bluetooth
         after PS-poll is acknowledged; \n
         default = 20 ms. */

    uint32_t aclCoexMinlowRateMbps;
    /**< Reserved. */
    uint32_t aclCoexLowRateCnt;
    /**< Reserved. */
    uint32_t aclCoexHighPktRatio;
    /**< Reserved. */
    uint32_t aclCoexMaxAggrSize;
    /**< Reserved. */
    uint32_t aclPktStompCnt;
    /**< Reserved. */
} qapi_WLAN_Coex_Acl_Config_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex inquiry/page scan operational mode conguration parameters.
*/
typedef struct {
    uint32_t btInquiryDataFetchFrequency;
    /**< Frequency of querying the AP for data
         (via PS-poll) is configured by this parameter;
         default = 10 ms. */

    uint32_t protectBmissDurPostBtInquiry;
    /**< Firmware will continue to be in an inquiry state
         for the configured duration after inquiry completion.
         This is to ensure other Bluetooth transactions
         (RDP, SDP profiles, link key exchange, etc.)
         go through smoothly without Wi-Fi stomping;
         default = 10 secs. */

    uint32_t maxpageStomp;
    /**< Reserved. */
    uint32_t btInquiryPageFlag;
    /**< Reserved. */
} qapi_WLAN_Coex_InqPage_Config_t;

/**
@ingroup qapi_wlan
Data structure for WLAN coex HID operational mode conguration parameters.
*/
typedef struct {
    uint32_t hidFlags;
    /**< HID option flags:
         - Bit 0 -- Allow close range optimization
         - Bit 4 -- No stomping BT during WLAN scan/connection @vertspace{-14} */

    uint32_t hiddevices;
    /**< Current device number of the HID. */

    uint32_t maxStompSlot;
    /**< Maximum stomped. */

    uint32_t aclPktCntLowerLimit;
    /**< ACL detect when HID is on. */

    uint32_t reserved[2];
    /**< Reserved. */

    uint32_t hidWlanMaxDur;
    /**< Reserved. */

    uint32_t hidMinBurstCnt;
    /**< Reserved. */

    uint32_t hidDataRespTimeout;
    /**< Reserved. */

    uint32_t hidMinlowRateMbps;
    /**< Reserved. */

    uint32_t hidLowRateCnt;
    /**< Reserved. */

    uint32_t hidHighPktRatio;
    /**< Reserved. */

    uint32_t hidMaxAggrSize;
    /**< Reserved. */

    uint32_t hidPktStompCnt;
    /**< Reserved. */
} qapi_WLAN_Coex_Hid_Config_t;


/**
@ingroup qapi_wlan
Data structure for the WLAN coex general statistics portion of a coex stats query.
*/
/* Used for firmware development and debugging*/
typedef struct {
    uint32_t highRatePktCnt;
    /**< Count of high rate (typically > 36 Mbps) WLAN packets received.
         Used to assess the impact of narrowband coex on WLAN performance. */
    uint32_t firstBmissCnt;
    /**< Count of first beacon misses. Used to assess whether narrowband is
         causing WLAN to miss beacons and to help debug a WLAN loss of
         connection with the AP. */
    uint32_t BmissCnt;
    /**< Overall count of beacon misses. Used to assess whether narrowband
         is causing WLAN to miss beacons and to help debug a WLAN loss of 
         connection with the AP. */
    uint32_t psPollFailureCnt;
    /**< Count of failures to issue PS-poll to the AP for traffic  shaping purposes.
         Used to debug situations where traffic shaping is in use. Traffic
         shaping is primarily used when SCO/ESCO and A2DP profiles are active. */
    uint32_t nullFrameFailureCnt;
    /**< Count of failures to issue a NULL frame to the AP for traffic shaping purposes.
         Used to debug situations where traffic shaping is in use. Traffic shaping
         is primarily used when SCO/ESCO and A2DP profiles are active. */
    uint32_t stompCnt;
    /**< Count of the times WLAN denies a narrowband request to access to the medium.
         This includes instances where narrowband is currently accessing the medium
         and WLAN asserts its DENY signal, effectively canceling the ongoing narrowband
         transaction. */
} qapi_Coex_General_Stats_t;
/**
@ingroup qapi_wlan
Data structure for the WLAN coex SCO statistics portion of a coex stats query. This is a union member that 
is selected based on the value returned in the profileType field of the #qapi_WLAN_Coex_Stats_t struct.
*/
typedef struct {
    uint32_t    scoStompCntAvg;         /**< SCO stomp count average. */
    uint32_t    scoStompIn100ms;        /**< SCO stomp count in 100 ms. */
    uint32_t    scoMaxContStomp;        /**< SCO maximum continuous stomp count. */
    uint32_t    scoAvgNoRetries;        /**< SCO average number of retries. */
    uint32_t    scoMaxNoRetriesIn100ms; /**< SCO maximum number of retries in 100 ms. */
} qapi_Coex_Sco_Stats_t;

/**
@ingroup qapi_wlan
Data structure for the WLAN coex A2DP statistics portion of a coex stats query. This is a union member that 
is selected based on the value returned in the profileType field of the #qapi_WLAN_Coex_Stats_t struct.
*/
typedef struct {
    uint32_t    a2dpBurstCnt;           /**< A2DP burst count. */
    uint32_t    a2dpMaxBurstCnt;        /**< A2DP maximum burst count. */
    uint32_t    a2dpAvgIdletimeIn100ms; /**< A2DP average idle time in 100 ms. */
    uint32_t    a2dpAvgStompCnt;        /**< A2DP average stomp count. */
} qapi_Coex_A2dp_Stats_t;

/**
@ingroup qapi_wlan
Data structure for that WLAN coex ACL statistics portion of a coex stats query. This is a union member that 
is selected based on the value returned in the profileType field of the #qapi_WLAN_Coex_Stats_t struct.
*/
typedef struct {
    uint32_t    aclPktCntInBtTime;     /**< ACL packet count in BT time. */
    uint32_t    aclStompCntInWlanTime; /**< ACL stomp count in WLAN time. */
    uint32_t    aclPktCntIn100ms;      /**< ACL packet count in 100 ms. */
} qapi_Coex_Aclcoex_Stats_t;


/**
@ingroup qapi_wlan
Data structure used to retrieve WLAN coex statistics from the driver. The application 
should pass data structure of this type to get the stats information by calling 
qapi_WLAN_Get_Param() with command ID as __QAPI_WLAN_PARAM_GROUP_WIRELESS_COEX_STATS.
The member of statsU is selected based on the value returned in the profileType field.
*/
typedef struct
{
    uint32_t resetStats;  /**< Reset stats. */
    struct {
        qapi_Bt_Profile_e profileType;  /**< Selects the statsU union member. */
        qapi_Coex_General_Stats_t generalStats;   /**< General statistics. */
        /** Union of coex stats, selected based on the profileType field. */
        union {
            qapi_Coex_Sco_Stats_t scoStats;         /**< SCO stats. */
            qapi_Coex_A2dp_Stats_t a2dpStats;       /**< A2DP stats. */
            qapi_Coex_Aclcoex_Stats_t aclCoexStats; /**< ACL coex stats. */
        } statsU; /**< Stats union. */
    } coex_Stats_Data; /**< Coex stats data. */
} qapi_WLAN_Coex_Stats_t;

/**
@ingroup qapi_wlan
Data structure for setting WLAN coex weight table entries. If the "enable" field is
0, the rest of the values in the data structure are ignored.
*/
typedef struct {
    uint32_t enable;       /**< Enable. */
    uint32_t btWghtVal[8]; /**< BT weighted value. */
    uint32_t wlWghtVal[4]; /**< WLAN weighted value. */
} qapi_WLAN_Coex_Override_Wghts_t;

/**
@ingroup qapi_wlan
Enum values for the coex_Antenna_Config field of the qapi_WLAN_Coex_Config_Data_t structure.
*/
typedef enum {
    QAPI_COEX_CONFIG_WHGTS_OVERRIDE,
    QAPI_COEX_CONFIG_ONCHIP,
    QAPI_COEX_CONFIG_EPTA_PARAMS,
    QAPI_COEX_CONFIG_EPTA,
    QAPI_COEX_CONFIG_MAX
} qapi_WLAN_Coex_Config_t;

/**
@ingroup qapi_wlan
Data structure for caching an in-use WLAN coex configuration over Suspend/Resume.
The member of configU is selected based on the value returned in the profileType field.
*/
typedef struct {
    qapi_WLAN_Coex_Override_Wghts_t whgts_override;        /**< Cached weight table entry overrides. */
    qapi_WLAN_Coex_Config_Data_t onchip_narrowband_config; /**< Cached on-chip narrowband coex configuration parameters. */
    qapi_Bt_Profile_e profileType;                         /**< 0: no EPTA profile in use; nonzero: selectes the configU member. */
    union {
        qapi_WLAN_Coex_Sco_Config_t sco_config;            /**< Cached EPTA SCO configuration parameters. */
        qapi_WLAN_Coex_A2dp_Config_t a2dp_config;          /**< Cached EPTA A2DP configuration parameters. */
        qapi_WLAN_Coex_Acl_Config_t acl_config;            /**< Cached EPTA ACL configuration parameters. */
        qapi_WLAN_Coex_InqPage_Config_t inqpage_config;    /**< Cached EPTA inquiry/page configuration parameters. */
        qapi_WLAN_Coex_Hid_Config_t hid_config;            /**< Cached EPTA HID configuration parameters. */
    } configU;                                             /**< Union of configuration members. */
    qapi_WLAN_Coex_Config_Data_t epta_narrowband_config;   /**< Cached off-chip/EPTA coex configuration parameters. @newpagetable */
} qapi_WLAN_Coex_Current_Config;



/**
@ingroup qapi_wlan
Data structure used to pass WPS SSID information from the application to the driver.
*/
typedef struct //qapi_WPS_Scan_List_Entry_s
{
    uint8_t ssid[__QAPI_WLAN_MAX_SSID_LEN];   /**< SSID. */
    uint8_t macaddress[__QAPI_WLAN_MAC_LEN];  /**< MAC address. */
    uint16_t channel;                         /**< Wireless channel. */
    uint8_t ssid_Len;                         /**< SSID length. */
} qapi_WPS_Scan_List_Entry_t;

/**
@ingroup qapi_wlan
Data structure to pass WPS command parameters from the application to the driver.
*/
typedef struct //qapi_WLAN_WPS_Start_s
{
    qapi_WPS_Scan_List_Entry_t ssid_info;   /**< SSID information. */
    uint8_t wps_Mode;                       /**< WPS pushbutton or WPS PIN. */
    uint8_t timeout_Seconds;                /**< WPS timeout in seconds. */
    uint8_t connect_Flag;                   /**< Connect action (TRUE/FALSE) after intial WPS success. */
    uint8_t pin[__QAPI_WLAN_WPS_PIN_LEN];   /**< PIN. */
    uint8_t pin_Length;                     /**< PIN length. */
} qapi_WLAN_WPS_Start_t;

/**
@ingroup qapi_wlan
Data structure for cipher.
*/
typedef struct //qapi_WLAN_Cipher_s
{
    uint32_t ucipher; /**< Unicast cipher. */
    uint32_t mcipher; /**< Multicast cipher. */
} qapi_WLAN_Cipher_t;

/**
@ingroup qapi_wlan
Data structure for wireless network parameters.
*/
typedef struct //qapi_WLAN_Netparams_s
{
    uint16_t ap_Channel;                       /**< Wireless channel for the AP. */
    int8_t ssid[__QAPI_WLAN_MAX_SSID_LENGTH];  /**< [OUT] Network SSID. */
    int16_t ssid_Len;                          /**< [OUT] Number of valid chars in ssid[]. */
    qapi_WLAN_Cipher_t cipher;                 /**< [OUT] Network cipher type values not defined. */
    uint8_t key_Index;                         /**< [OUT] For WEP only; key index for Tx. */
    union{
        uint8_t wepkey[__QAPI_WLAN_PASSPHRASE_LEN+1];
		/**< WEP key. */

        uint8_t passphrase[__QAPI_WLAN_PASSPHRASE_LEN+1];
		/**< Passphrase. */
    }u;
	/**<
	[OUT] Security key or passphrase.
	*/

    uint8_t sec_Type;                          /**< [OUT] Security type. */
    uint8_t error;                             /**< [OUT] Error code. */
    uint8_t dont_Block;                        /**<
                                               [IN] 1 -- Returns immediately if the operation is not complete \n
                                                    0 -- Blocks until the operation completes @newpagetable
                                               */
} qapi_WLAN_Netparams_t;

/**
@ingroup qapi_wlan
Enumeration that identifies the device mode.
*/
typedef enum
{
    MODE_STATION_E = 0, /**< Station mode. */
    MODE_AP_E,          /**< SoftAP mode supported only on device 0. */
    MODE_ADHOC_E,       /**< Adhoc network; supported only on device 0. */
    MODE_MAXIMUM_E      /**< Maximum value for the device mode. */
} qapi_WLAN_Mode_e;

/**
@ingroup qapi_wlan
Data structure for the IPv6 address.
*/
typedef struct
{
    uint8_t ip_Address[__QAPI_WLAN_IPV6_ADDR_LEN];  /**< IPv6 address. */
} qapi_WLAN_IPv6_Addr_t;
     
/**
@ingroup qapi_wlan
Data structure for ARP offload.
*/
typedef struct
{
    uint8_t           enable;                               /**< Enable/disable. */
    uint8_t           target_IP[__QAPI_WLAN_IPV4_ADDR_LEN]; /**< IPV4 addresses of the local node. */
    uint8_t           target_Mac[__QAPI_WLAN_MAC_LEN];      /**< MAC address for this tuple; if not valid, the local MAC is used. */
}qapi_WLAN_ARP_Offload_Config_t;

/**
@ingroup qapi_wlan
Data structure for neighbor solicitation offload.
*/
typedef struct
{
    uint8_t                   enable;                                      /**< Enable/disable. */
    qapi_WLAN_IPv6_Addr_t     target_IP[__QAPI_WLAN_NSOFF_MAX_TARGET_IPS]; /**< IPV6 WLAN firmware address of the local node. */
    uint8_t                   target_Mac[__QAPI_WLAN_MAC_LEN];             /**< MAC address for this tuple; if not valid, the local MAC is used. */
    qapi_WLAN_IPv6_Addr_t     solicitation_IP;                             /**< Multicast source IP addresses for receiving solicitations. */
} qapi_WLAN_NS_Offload_Config_t;

/**
@ingroup qapi_wlan
Enum declaration for management frame types.
*/
typedef enum {
    QAPI_WLAN_FRAME_BEACON_E = 0,  /**< Beacon frame type. */
    QAPI_WLAN_FRAME_PROBE_REQ_E,   /**< Probe request frame type. */
    QAPI_WLAN_FRAME_PROBE_RESP_E,  /**< Probe response frame type. */
    QAPI_WLAN_FRAME_ASSOC_REQ_E,   /**< Association request frame type */
    QAPI_WLAN_FRAME_ASSOC_RESP_E,  /**< Association response frame type. */
    QAPI_WLAN_NUM_MGMT_FRAME_E     /**< Number of management frame types. */
} qapi_WLAN_Mgmt_Frame_Type_e;

/**
@ingroup qapi_wlan
Data structure to pass application information element data from the application to the driver.
*/
typedef struct //qapi_WLAN_App_Ie_Params_s
{
    uint8_t mgmt_Frame_Type;   /**< Frame in which IE is to be added. */
    uint8_t ie_Len;            /**< IE length. */
    uint8_t *ie_Info;          /**< Application specified IE. */
} qapi_WLAN_App_Ie_Params_t;

/**
@ingroup qapi_wlan
Data structure for Rx A-MPDU.
*/
typedef struct //qapi_WLAN_Rx_Aggrx_Params_s
{
    uint8_t aggrx_Buffer_Size;                   /**< Buffer size. */
    uint8_t aggrx_Reorder_Buffer_Timeout_In_Ms;  /**< Buffer reorder timeout. */
    uint8_t aggrx_Session_Timeout_Val_In_Ms;     /**< Session timeout. */
    uint8_t aggrx_Reorder_Cfg;                   /**< Reorder configuration. */
    uint8_t aggrx_Session_Timeout_Cfg;           /**< Session timeout. */
    uint8_t reserved0;                           /**< Reserved. */
} qapi_WLAN_Rx_Aggrx_Params_t;

/**
@ingroup qapi_wlan
Data structure to pass 802.11v BSS maximum idle period information from the
application to the driver. Used only in SoftAP mode.
*/
typedef struct //qapi_WLAN_BSS_Max_Idle_Period_s
{
    uint16_t period;                /**< Period in terms of 1000 TUs. */
    uint16_t protected_Keep_Alive;  /**< If protected, keepalives are required. */
} qapi_WLAN_BSS_Max_Idle_Period_t;

/**
@ingroup qapi_wlan
Data structure to pass 802.11v WNM sleep period information from the application to the driver. Used only in Station mode.
*/
typedef struct //qapi_WLAN_WNM_Sleep_Period_s
{
    uint16_t action_Type; /**< Enter: 1, exit: 0. */
    uint16_t duration;    /**< Duration in terms of DTIM intervals. */
} qapi_WLAN_WNM_Sleep_Period_t;

/**
@ingroup qapi_wlan
Data structure to get 802.11v event information from the driver.
*/
typedef struct //qapi_WLAN_WNM_Cb_Info_s
{
    uint16_t cmd_Type;   /**< WNM command type. */
    uint16_t response;   /**< WNM command result. */
} qapi_WLAN_WNM_Cb_Info_t;

/**
@ingroup qapi_wlan
Identifies the disconnection reason
*/
typedef enum {
    QAPI_WLAN_NO_NETWORK_AVAIL_E              = 0x01, /**< No network available. */
    QAPI_WLAN_LOST_LINK_E                     = 0x02, /**< Missed beacons. */
    QAPI_WLAN_DISCONNECT_CMD_E                = 0x03, /**< User disconnect command. */
    QAPI_WLAN_BSS_DISCONNECTED_E              = 0x04, /**< BSS disconnected. */
    QAPI_WLAN_AUTH_FAILED_E                   = 0x05, /**< Authentication failed. */
    QAPI_WLAN_ASSOC_FAILED_E                  = 0x06, /**< Association failed. */
    QAPI_WLAN_NO_RESOURCES_AVAIL_E            = 0x07, /**< No resources available. */
    QAPI_WLAN_CSERV_DISCONNECT_E              = 0x08, /**< Disconnection due to connection services. */
    QAPI_WLAN_INVALID_PROFILE_E               = 0x0a, /**< RSNA failure. */
    QAPI_WLAN_DOT11H_CHANNEL_SWITCH_E         = 0x0b, /**< 802.11h channel switch. */
    QAPI_WLAN_PROFILE_MISMATCH_E              = 0x0c, /**< Profile mismatched. */
    QAPI_WLAN_CONNECTION_EVICTED_E            = 0x0d, /**< Connection evicted. */
    QAPI_WLAN_IBSS_MERGE_E                    = 0x0e, /**< Disconnection due to merging of IBSS. */
    QAPI_WLAN_EXCESS_TX_RETRY_E               = 0x0f, /**< TX frames failed after excessive retries. */
    QAPI_WLAN_SEC_HS_TO_RECV_M1_E             = 0x10, /**< Security 4-way handshake timed out waiting for M1. */
    QAPI_WLAN_SEC_HS_TO_RECV_M3_E             = 0x11, /**< Security 4-way handshake timed out waiting for M3. */
    QAPI_WLAN_TKIP_COUNTERMEASURES_E          = 0x12, /**< TKIP counter-measures. */
    QAPI_WLAN_CCX_TARGET_ROAMING_INDICATION_E = 0xfd, /**< Unused. */
    QAPI_WLAN_CCKM_ROAMING_INDICATION_E       = 0xfe, /**< Unused. */
} qapi_WLAN_Disconnect_Reason_t;

/**
@ingroup qapi_wlan
Data structure to be used in case the application wants to 
change Beacon Miss parameters.

On receiving this request from a user in STA mode, the WLAN firmware reports
a link loss after the specified interval. Either one of the values will be 
set by the user; bmiss_Time_In_Ms or num_Beacons. Based on the value set, the firmware 
detects a beacon miss and triggers a disconnect 
in STA mode.
*/
typedef struct //qapi_WLAN_Sta_Config_Bmiss_Config_s
{
    uint16_t     bmiss_Time_In_Ms;
        /**< New beacon miss time (in ms) to be set by application. */
    uint16_t     num_Beacons;
        /**< New number of beacons to be set by the application to detect the number of beacons missed, after which a disconnect is triggered. */
} qapi_WLAN_Sta_Config_Bmiss_Config_t;

/**
@ingroup qapi_wlan
enumerate antenna switch based on which mode
*/
typedef enum {
	QAPI_WLAN_ANT_DIV_AUTO_MODE_E					= 0x0, /**ant siwtch based on auto mode. In this mode, we switch antenna 
														    immediately once it is neccessary to switch antenna, for example
														    the main rssi is  very low and the alt rssi is strong*/
	QAPI_WLAN_ANT_DIV_PACKET_NUMBER_STRICTLY_E		= 0x1, /**ant siwtch based on packet number strictly. */
	QAPI_WLAN_ANT_DIV_TIME_INTERVAL_STRICTLY_E		= 0x2, /**ant siwtch based on time interval strictly. */
} qapi_WLAN_Ant_Div_Mode_e;

/**
@ingroup qapi_wlan
enumerate antenna diversity enable mode
*/
typedef enum {
	QAPI_WLAN_DISABLE_ANT_DIV_E					= 0x0, /** disable ant div. */
	QAPI_WLAN_ENABLE_HW_ANT_DIV_E				= 0x1, /** enable hw ant div. */
	QAPI_WLAN_ENABLE_SW_ANT_DIV_E				= 0x2, /** enable sw ant div, for future use. */
} qapi_WLAN_Ant_Div_Enable_e;

/**
@ingroup qapi_wlan
Data structure to configure the paramters of HW antenna diversity .
*/
typedef struct //qapi_WLAN_Hw_Ant_Div_Config_s
{
	uint8_t		mode;				/**< refer to enum qapi_WLAN_Ant_Div_Mode_e . */
	uint8_t		reserved1;			/**< reserved for future use . */
	uint16_t 	reserved2;			/**< reserved for future use . */
	uint32_t	param;				/**< if mode is auto mode, it can be skipped, if mode is packet number strictly,
									it is  packet number and if mode is time interval strictly, it is time interval . */
} qapi_WLAN_Hw_Ant_Div_Config_t;

/**
@ingroup qapi_wlan
Data structure to configure the paramters of SW antenna diversity .
*/
typedef struct//qapi_WLAN_Sw_Ant_Div_Config_s
{
	uint16_t	interval;					/**< the time interval for sw antenna diversity . */	
	uint16_t	reserved;					/**< reserved for future use . */
	uint32_t	high_traffic_threshold;	    /**< high traffic threshold . */
} qapi_WLAN_Sw_Ant_Div_Config_t;

/**
@ingroup qapi_wlan
Data structure to be used in case the application wants to 
configure HW antenna diversity.
*/
typedef struct //qapi_WLAN_Ant_Div_Config_s
{
    uint8_t     enable_Ant_Div;  	/**< refer to enum qapi_WLAN_Ant_Div_Enable_e . */
	uint8_t     tx_Follow_Rx;   	/**< 1: tx follows rx, 0:tx not follows rx. */
  	union 
  	{
  		qapi_WLAN_Hw_Ant_Div_Config_t hw_ant_div_config; /**< configure hw antenna diversity parameter . */
		qapi_WLAN_Sw_Ant_Div_Config_t sw_ant_div_config; /**< reserved for future use. */
  	}ant_div_config;
} qapi_WLAN_Ant_Div_Config_t;

/**
@ingroup qapi_wlan
Data structure used to retrieve the current antenna diversity information . 
*/
typedef struct //qapi_WLAN_Get_Ant_Div_s
{
	uint8_t					enable_Ant_Div;		 /**< refer to enum qapi_WLAN_Ant_Div_Enable_e. */
	uint8_t					tx_Follow_Rx;	 	 /**< 1: tx antenna follows rx antenna, 0:tx antenna not follows rx antenna. */
	uint8_t					curr_Rx_Ant_2g;	 	/**< current rx physical antenna in 2g band. */
	uint8_t					curr_Tx_Ant_2g;	 /**< current tx physical antenna in 2g band. */
	uint8_t					curr_Rx_Ant_5g;	 /**< current rx physical antenna in 5g band. */
	uint8_t					curr_Tx_Ant_5g;	 /**< current tx physical antenna in 5g band. */	
	uint8_t					avg_Main_Rssi;   	/**< the current average main rssi. */
	uint8_t					avg_Alt_Rssi;    	/**< the current average alternative rssi. */
	uint32_t				total_Pkt_Cnt;   	/**< the count of packet is used to calculate the sum of rssi. */
	uint32_t				ant_Switch_Cnt;	 	/**< the number of switching antenna. */
}qapi_WLAN_Get_Ant_Div_t;

/**
@ingroup qapi_wlan
Function pointer to an application specified callback handler function.

The callback handler for WLAN commands can be set using qapi_WLAN_Set_Callback().

@param[in] device_ID                Device ID.
@param[in] cb_ID                    Callback ID associated to a command.
@param[in] application_Context      Application context.
@param[in] payload                  Data.
@param[in] payload_Length           Data size.

@return
None.
*/
typedef void (* qapi_WLAN_Callback_t)(uint8_t device_ID,
                                           uint32_t cb_ID,
                                           void *application_Context,
                                           void  *payload,
                                           uint32_t payload_Length);

/**
@ingroup qapi_wlan
Data structure to store an application-specific callback function and application context in the driver context.
Set the application context set to NULL if not needed.
*/
typedef struct //qapi_WLAN_Callback_Context_s
{
    void *application_Context;              /**< Application context. */
    qapi_WLAN_Callback_t callback_Function; /**< Callback function. */
} qapi_WLAN_Callback_Context_t;

typedef void (* qapi_WLAN_Coex_Callback_t)(uint8_t device_ID,
                                           uint32_t cb_ID,
                                           void *application_Context,
                                           void  *payload,
                                           uint32_t payload_Length);

/**
@ingroup qapi_wlan
Data structure to store an application-specific callback function and application context in the driver context.
Set the application context to NULL if not needed.
*/
typedef struct qapi_WLAN_Coex_Callback_Context_s
{
    void *application_Context;              /**< Application context. */
    qapi_WLAN_Coex_Callback_t callback_Function; /**< Callback function. */
} qapi_WLAN_Coex_Callback_Context_t;

/** 
@ingroup qapi_wlan  
Enables, configures, and disables coexistence. 
 
@datatypes 
#qapi_WLAN_Coex_Config_Data_t 
  
@param[in] data       Pointer to coex configuration data.
 
@return 
QAPI_OK -- Coexistence configuration operation succeeded.     \n
Nonzero value --  Coexistence configuration operation failed.
 
@dependencies 
QCA402x Wi-Fi firmware must be loaded and running. 
*/

qapi_Status_t qapi_WLAN_Coex_Control(const qapi_WLAN_Coex_Config_Data_t *data);

/** 
@ingroup qapi_wlan  
Gets assorted statistics counters from the WLAN coex subsystem. WLAN coex statistics counters are enabled 
in the WLAN firmware by default. Setting the WLAN_CoexStats.resetStats field to a nonzero value causes the WLAN 
firmware to reset its copy of the counters after returning the current counter values to the host.

@datatypes 
#qapi_WLAN_Coex_Stats_t
  
@param[in] WLAN_CoexStats        Pointer to a WLAN coex statistics counter structure.
 
@return 
QAPI_OK -- Coexistence statistics counters were populated successfully. \n
Nonzero value -- The get coexistence statistics counters operation failed.
 
@dependencies 
QCA402x WLAN firmware must be loaded and running.
*/

qapi_Status_t qapi_Get_WLAN_Coex_Stats(qapi_WLAN_Coex_Stats_t *WLAN_CoexStats);

/**
@ingroup qapi_wlan
Enables/disables the Wi-Fi module.
This is a blocking call and returns on receipt of a WMI_READY event from KF.

Use QAPI_WLAN_ENABLE_E as the parameter for enabling and QAPI_WLAN_DISABLE_E for disabling WLAN.

This API brings up the KF firmware but does not add any devices.

@datatypes
#qapi_WLAN_Enable_e

@param[in] enable  QAPI_WLAN_ENABLE_E or QAPI_WLAN_DISABLE_E.

@sa
qapi_WLAN_Add_Device

@return
QAPI_OK -- Enabling or disabling WLAN succeeded. \n
QAPI_BUSY -- Disabling WLAN failed due to open AF_INET (IPv4) sockets. \n
Nonzero value -- Enabling or disabling WLAN failed.

@dependencies
Use qapi_WLAN_Add_Device() after qapi_WLAN_Enable() to add devices before using other
WLAN control commands.\n
Use qapi_WLAN_Remove_Device() before disabling WLAN.
*/
qapi_Status_t qapi_WLAN_Enable (qapi_WLAN_Enable_e enable);


/**
@ingroup qapi_wlan
get the wlan status before OM, this API has no interaction with KF.

@return
QAPI_WLAN_DISABLE_E if not enabled, QAPI_WLAN_ENABLE_E if enabled.\n
*/

int32_t qapi_WLAN_Get_OM_Status (void);


/**
@ingroup qapi_wlan
Adds an interface in the Wi-Fi driver. This API has no interaction with KF.

WLAN must be enabled before calling this API. A maximum of two devices are supported at this time. 

@param[in] device_ID        Device ID.

@return
QAPI_OK -- Adding a new device (interface) succeeded. \n
Nonzero value -- Adding a new device (interface) failed.

@dependencies
Use qapi_WLAN_Enable() to enable the Wi-Fi module before using this API.
*/
qapi_Status_t qapi_WLAN_Add_Device (uint8_t device_ID);


/**
@ingroup qapi_wlan
Removes an interface from the Wi-Fi driver.
This API removes device but does not disable WLAN.

@param[in] device_ID        Device ID.

@return
QAPI_OK -- Removing Wi-Fi interface succeeded. \n
Nonzero value -- Removing WiFi interface failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_Remove_Device (uint8_t device_ID);


/**
@ingroup qapi_wlan
Initiates a wireless scan to fnid nearby access points.

Users can configure the scan type as they want. Additional scan parameters can be tuned by using qapi_WLAN_Set_Params() with
GroupID for Wireless and ParamID scan_params.

By default, this API is a blocking call. For the nonblocking mode, set the store_Scan_Results field appropriately.

In nonbuffering mode, scan results are not filtered, whereas in blocking and nonblocking modes, they are filtered.

@datatypes
#qapi_WLAN_Start_Scan_Params_t \n
#qapi_WLAN_Store_Scan_Results_e

@param[in] device_ID            Device ID.
@param[in] scan_Params          Data pointer to pass information from the caller to the driver.
@param[in] store_Scan_Results   Scan type: blocking/nonblocking/nonbuffering.

@return
QAPI_OK -- Wireless scan started. \n
Nonzero value -- Wireless scan failed.

@dependencies
Scan cannot be started in SoftAP mode.
*/
qapi_Status_t qapi_WLAN_Start_Scan (uint8_t device_ID,
                                                 const qapi_WLAN_Start_Scan_Params_t  *scan_Params,
                                                 qapi_WLAN_Store_Scan_Results_e store_Scan_Results);


/**
@ingroup qapi_wlan
Returns the scan results from the most recent wireless scan performed to find nearby access points.

This is a blocking API and should be used when the QAPI_WLAN_BUFFER_SCAN_RESULTS_BLOCKING option is used.

@datatypes
#qapi_WLAN_BSS_Scan_Info_t

@param[in]     device_ID              Device ID.
@param[out]    scan_Info              Data pointer to get the scan result list from the driver.
@param[in,out] num_Results            Number of scan results.

@return
QAPI_OK -- Getting the results from most recent wireless scan succeeded. \n
Nonzero value -- Getting the results from most recent wireless scan failed.

@dependencies
Call qapi_WLAN_Start_Scan() to start a wireless scan before calling this API.
*/
qapi_Status_t qapi_WLAN_Get_Scan_Results (uint8_t device_ID,
                                                           qapi_WLAN_BSS_Scan_Info_t  *scan_Info,
                                                           int16_t  *num_Results);


/**
@ingroup qapi_wlan
This API is part of connect/disconnect process in both non-AP Station and SoftAP modes.
It uses previously set SSID and security configurations.

In non-AP Station mode, it allows the station to connect to or disconnect from an associated AP.
In SoftAP mode, it starts the device as a SoftAP.

This API is already called from qapi_WLAN_Disconnect() and qapi_WLAN_WPS_Connect(), so it does not need to be called explicitly for these
operations; however for connect operations (except for WPS), it will need to be called explicitly.

@param[in] device_ID           Device ID.

@return
QAPI_OK -- Commit operation succeeded. \n
Nonzero value -- Commit operation failed.

@dependencies
SSID and security configurations must be set using qapi_WLAN_Set_Param() before calling this API.\n
If SSID is set to a nonempty string before calling this API, it is considered as a connect request, and if the SSID is set to
an empty string, it is considered as a disconnect request in both non-AP Station and SoftAP modes.\n
If the user wants to use Tx/Rx aggregation, __QAPI_WLAN_PARAM_GROUP_WIRELESS_ALLOW_TX_RX_AGGR_SET_TID must be set using qapi_WLAN_Set_Param()
before calling this API.\n
Similarly, if the user wants to use UAPSD, __QAPI_WLAN_PARAM_GROUP_WIRELESS_AP_ENABLE_UAPSD (in SoftAP mode) and
__QAPI_WLAN_PARAM_GROUP_WIRELESS_STA_UAPSD in non-AP Station mode should be set using qapi_WLAN_Set_Param().
*/
qapi_Status_t qapi_WLAN_Commit (uint8_t device_ID);

/**
@ingroup qapi_wlan
Sets a callback function and application context (const void *), if any, for WLAN control commands.

Users can use the callback function to handle events received from the WLAN firmware for asynchronous commands.
The application context, while setting the callback, will be returned when the callback handler function is called for events.
The callback handling is done in the Wi-Fi driver thread's context, hence the event handling should be as fast as possible to avoid
performance delays.

@datatypes
#qapi_WLAN_Callback_t

@param[in] device_ID              Device ID.
@param[in] callback               Callback function.
@param[in] application_Context    Application context set by the application.

@return
QAPI_OK -- Application callback handler function for WLAN commands set. \n
Nonzero value -- Setting callback function failed.

@dependencies
The callback function to be set must be a valid function.
*/
qapi_Status_t qapi_WLAN_Set_Callback (uint8_t device_ID,
                                      qapi_WLAN_Callback_t callback,
                                      const void *application_Context);

/** @cond */
#ifdef WLAN_TESTS
/**
@ingroup qapi_wlan
This API is used for wlan specific test commands.

@param[in]      wlan_test_id      Test ID.

@return
QAPI_OK -- Test command succeeded. \n
Nonzero value -- Test command failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_tests(uint8_t wlan_test_id);
#endif /* WLAN_TESTS */
/** @endcond */


/**
@ingroup qapi_wlan
Disconnects a device or aborts the current connection process. 
This API internally calls qapi_WLAN_Commit(), hence no explicit call is needed in the disconnect process when using this API.

@param[in] device_ID         Device ID.

@return
QAPI_OK -- Disconnect process succeeded. \n
Nonzero value -- Disconnection failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_Disconnect (uint8_t device_ID);


/**
@ingroup qapi_wlan
Sets requested parameters in the WLAN firmware.

The group_ID parameter is used to determine to which group the the command belongs, and the param_ID is used to provide the
requested command (/parameter).

The __QAPI_WLAN_PARAM_GROUP_* types give more information regarding the possible values for group_ID and param_ID.
Currently, only nonblocking calls are supported, hence wait_For_Status should be set to FALSE (0).

@datatypes
#qapi_WLAN_Wait_For_Status_e
 
@param[in] device_ID         Device ID.
@param[in] group_ID          Group ID: system, wireless, security, P2P.
@param[in] param_ID          Parameter/command ID.
@param[in] data              Value to be set for the requested parameter.
@param[in] length            Size of the value to be set.
@param[in] wait_For_Status   0: No wait; 1: Wait for result of the set request.

@return
QAPI_OK -- Requested parameter was set. \n
Nonzero value -- Requested parameter was not set.
 
@dependencies
None.
*/
qapi_Status_t qapi_WLAN_Set_Param (uint8_t device_ID,
                                               uint16_t group_ID,
                                               uint16_t param_ID,
                                               const void  *data,
                                               uint32_t length,
                                               qapi_WLAN_Wait_For_Status_e wait_For_Status);


/**
@ingroup qapi_wlan
Retrieves the requested parameter value from the WLAN firmware.

The group_ID parameter is used to determine to which group the the command belongs, and the param_ID is used to provide the
requested command (/parameter).

The __QAPI_WLAN_PARAM_GROUP_* types give more information regarding the possible values for group_ID and param_ID.

@param[in]  device_ID     Device ID.
@param[in]  group_ID      Group ID: system, wireless, security, P2P.
@param[in]  param_ID      Parameter ID in the given group.
@param[out] data          Value retrieved from the WLAN firmware.
@param[out] length        Size of the parameter value.

@return
QAPI_OK -- Requested was parameter retrieved from the WLAN firmware. \n
Nonzero value -- Parameter retrieval failed.
  
@dependencies
None.
*/
qapi_Status_t qapi_WLAN_Get_Param (uint8_t device_ID,
                                               uint16_t group_ID,
                                               uint16_t param_ID,
                                               void *data,
                                               uint32_t *length);


/**
@ingroup qapi_wlan
Suspends a KF operation for a fixed duration.

@param[in] suspend_Time_In_Ms   Suspend time duration in milliseconds.

@return
QAPI_OK -- Suspend operation succeeded. \n
Nonzero value -- Suspend operation failed.
  
@dependencies
None.
*/
qapi_Status_t qapi_WLAN_Suspend_Start (uint32_t suspend_Time_In_Ms);


/**
@ingroup qapi_wlan
Sends packets over the air when the device is in the Disconnected state.

@datatypes
#qapi_WLAN_Raw_Send_Params_t
 
@param[in] device_ID    Device ID.
@param[in] raw_Params   Data pointer to pass information from the caller to the driver.

@return
QAPI_OK -- Requested number of raw packets sent over the air. \n
Nonzero value -- Raw packets not sent.
  
@dependencies
Device must be in Maximum Performance mode to send raw packets. \n
Device should not be in the Connected state while calling this API. \n
Raw mode is supported only in 2.4 GHz.
*/
qapi_Status_t qapi_WLAN_Raw_Send (uint8_t device_ID,
                             const qapi_WLAN_Raw_Send_Params_t  *raw_Params);

/**
@ingroup qapi_wlan
Initiates a WPS process. It can be used in both non-AP Station mode and SoftAP mode.

The connect_Action parameter specifies whether the WPS connection is to take place after the WPS handshake completes successfully.

The application is responsible for starting the connection process.
An event callback with ID QAPI_WLAN_WPS_CB_E is received asychronously when the WPS handshake completes. The application should use 
qapi_WLAN_WPS_Await_Completion to handle this event. If the handshake succeeds, the application should use qapi_WLAN_WPS_Connect() to complete 
the connection process. 

@datatypes
#qapi_WLAN_WPS_Connect_Action_e \n
#qapi_WLAN_WPS_Mode_e

@param[in] device_ID         Device ID.
@param[in] connect_Action    0: No action after WPS succeeds; 1: Connect to AP after WPS succeeds.
@param[in] mode              WPS mode: Pushbutton/PIN.
@param[in] pin               PIN.

@return
QAPI_OK -- WPS intiation succeeded. \n
Nonzero value -- WPS process failed.
  
@sa
qapi_WLAN_Set_Param() \n
qapi_WLAN_WPS_Await_Completion() \n
qapi_WLAN_WPS_Connect()
*/
qapi_Status_t qapi_WLAN_WPS_Start (uint8_t device_ID,
                              qapi_WLAN_WPS_Connect_Action_e connect_Action,
                              qapi_WLAN_WPS_Mode_e mode,
                              const char  *pin);
                              

/**
@ingroup qapi_wlan
Waits for WPS completion. Use the qapi_WLAN_WPS_Start() before calling this API to initialize a WPS connection.

Use the member dont_Block in qapi_WLAN_Netparams_t to set this API as blocking or nonblocking.
In the case of success, net_Params will give the peer the WPS device's profile, found via handshake. The applications can then use
qapi_WLAN_WPS_Connect() to complete the WPS connection.

If a nonblocking option is used and if a WPS event is not received from the target, qapi_WLAN_WPS_Await_Completion() 
returns immediately with an error. However, the error field of the input argument net_Params is set to 0. 
Applications should use both the return value and above mentioned member to decide if the API failed or an event is 
pending.

In the case of a failure, the WPS handshake failed, hence the WPS connection process cannot be completed.
It is recommended to call this API after the application receives a WPS handshake completion notification by the WLAN driver to avoid 
unnecessary blocking or polling for the notification.

@datatypes
#qapi_WLAN_Netparams_t

@param[in]     device_ID     Device ID.
@param[in,out] net_Params    Data pointer to get information from the driver regarding the initial WPS request.
                             The dont_Block member should be set to 1 (nonblocking) or 0 (blocking) before calling this API.

@return
QAPI_OK -- Peer WPS device profile found succeeded. \n
Nonzero value -- Peer WPS device was not found.

@dependencies
Call qapi_WLAN_WPS_Start() before this API to initiate the WPS process.
 */
qapi_Status_t qapi_WLAN_WPS_Await_Completion(uint32_t device_ID, qapi_WLAN_Netparams_t *net_Params);


/**
@ingroup qapi_wlan
This API allows user to start WPS connection on a specific device using the previously found peer WPS device profile.\n
The device should first initiate WPS processing by calling qapi_WLAN_WPS_Start(). Then qapi_WLAN_WPS_Await_Completion() will provide
application with the peer WPS profiles found during the initial process.\n
Only after the first two steps, qapi_WLAN_WPS_Connect() API can successfully connect to the peer WPS device.\n
This API may still return error in spite of first two steps succeeding.

@param[in] device_ID    Device ID.

@return
QAPI_OK -- WPS handshake succeeded. \n
Nonzero value -- WPS handshake failed.
   
@dependencies
The application should configure SSID, security type, authentication method, and WPS credentials before calling this API. Also, qapi_WLAN_WPS_Start() 
and qapi_WLAN_WPS_Await_Completion() should be called before this API to initiate the WPS process and get the WPS device profiles.

@sa
__QAPI_WLAN_PARAM_GROUP_SECURITY_WPS_CREDENTIALS\n
__QAPI_WLAN_PARAM_GROUP_SECURITY_PASSPHRASE\n
__QAPI_WLAN_PARAM_GROUP_SECURITY_PMK\n
__QAPI_WLAN_PARAM_GROUP_SECURITY_PASSPHRASE\n
__QAPI_WLAN_PARAM_GROUP_SECURITY_WPS_CREDENTIALS\n
qapi_WLAN_WPS_Start\n
qapi_WLAN_WPS_Await_Completion
*/
qapi_Status_t qapi_WLAN_WPS_Connect (uint8_t device_ID);

/**
@ingroup qapi_wlan
Enum values for the FTM mode command type.
*/
typedef enum {
    QAPI_WLAN_FTM_SEND_REQ=0, /**< Send a packet request. */
    QAPI_WLAN_FTM_LOAD_BD /**< Load board data. */
} qapi_WLAN_FTM_CMD_e;

/**
@ingroup qapi_wlan
Sends the TLV1 command buffer to the KF UTF.

This function is used by the FTM mode; the supported command type is limited.

In the Diag module, the handler of Diag packets for WLAN should call this interface after receiving packets from the QDART,
and this interface synchronously waits for receiving the response packet from UTF.

@param[in]  cmd_type        Type of the manufacture command in FTM mode.
@param[in]  buf             Buffer of the manufacture testing packet destined to KF UTF.
@param[in]  tx_Len          Length of the manufacture testing packet destined to KF UTF.
@param[out] buf_Response    Buffer pointer pointing to the received response packet from KF UTF.
@param[out] response_Len    Buffer length of the received response packet from KF UTF.

@return
QAPI_OK -- Command send succeeded. \n
Nonzero value -- Command failed.
   
@dependencies
This fucntion can only be used in FTM mode for manufacture use.
*/
qapi_Status_t qapi_WLAN_FTM_Command_Send (qapi_WLAN_FTM_CMD_e cmd_type, uint8_t *buf,uint32_t tx_Len, uint8_t **buf_Response, uint32_t *response_Len);
#endif // __QAPI_WLAN_BASE_H__

