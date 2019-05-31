###############################################################################
#
# Copyright 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

if(BT_CHIP_TYPE STREQUAL "QCA")
csr_define_cache_parameter(CSR_DSPM_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enables an application to control how audio data sources and sinks on the BlueCore™ are connected to each other, and additionally allows processing of these audio data streams")
else()
csr_define_cache_parameter(CSR_DSPM_ENABLE
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enables an application to control how audio data sources and sinks on the BlueCore™ are connected to each other, and additionally allows processing of these audio data streams")
endif()
if(CSR_HOST_PLATFORM STREQUAL "MDM")
csr_define_cache_parameter(CSR_BT_CONFIG_CARKIT
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Configures the Stack and application as car kit ie automotive defaults are used")
else()
csr_define_cache_parameter(CSR_BT_CONFIG_CARKIT
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Configures the Stack and application as car kit ie automotive defaults are used")
endif()
csr_define_cache_parameter(CSR_BOARD
                           TYPE STRING
                           DESCRIPTION "Updates the CSR board type for AMP to include right PSKeys, MIB and other dependency files related to this board. This flag is applicable only if CSR_AMP_ENABLE is enabled."
                           DEFAULT "CSR_BOARD_M2501_A10" STRINGS "CSR_BOARD_M2107_A05" "CSR_BOARD_M2107_B07" "CSR_BOARD_M2399_A10" "CSR_BOARD_M2501_A08" "CSR_BOARD_M2501_A10")

csr_define_cache_parameter(CSR_BT_APP_AMP_WIFI
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Bluetooth AMP Wifi support")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_AMPWIFI
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate Wi-Fi AMP in demo application")

csr_define_cache_parameter(CSR_BT_APP_AMP_UWB
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Bluetooth AMP UWB support")

csr_define_cache_parameter(AV_STREAM_INACTIVITY_LP_ENABLE
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Enables detection of inactive AV streams (no stream data sent/received) and that low power mode (sniff) is requested for such streams upon detection. When defined it is possible to set inactive period (AV_LP_TIMEOUT) in usr_config.h. This should not be enabled if ENABLE_AV_ROUTER_SUPPORT is also enabled.")

csr_define_cache_parameter(CSR_BT_BLUE_STACK_DEBUG
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Enables core stack debug features")

csr_define_cache_parameter(CSR_BT_CONFIG_L2CAP_FCS
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Enables L2CAP connection reconfiguration when a connection is moved from BT to Wi-Fi")

if (DEFINED CSR_COMPONENT_BT_AVRCP)
csr_define_cache_parameter(EXCLUDE_CSR_BT_AVRCP_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Audio/Video Remote Control module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AVRCP_CT_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Controller side of AVRCP in order to save code space if this is not needed")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AVRCP_TG_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Target side of AVRCP in order to save code space if this is not needed")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AVRCP_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Have been introduced to reduce code size, by disabling AVRCP 1.3 and 1.4 features, and corresponding API/LIB functions")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AVRCP_MODULE_COVER_ART
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Cover Art feature if this is not needed")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_AVRCP
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate AVRCP instance in demo application")
endif()
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AT_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the modem AT-Command module")

if (DEFINED CSR_COMPONENT_BT_AV)
csr_define_cache_parameter(EXCLUDE_CSR_BT_AV_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Audio Video module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_AV_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional AV parameters. Default values can be specified in csr_bt_usr_config.h")
                                                      
csr_define_cache_parameter(CSR_BT_APP_USE_AV
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate AV instance in demo application")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_CME_BH_FEATURE
                           TYPE BOOL 
                           DEFAULT ON
                           DESCRIPTION "Coex Management Entity for Bluetooth host feature shall be enabled only if a CSR combo chip based on hydra architecture is used.")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_BPP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_BPPS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Basic Printing server module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_BPPS_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude optional BPPS features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_BPPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate BPPS instance in demo application")
endif()  
                           
if (DEFINED CSR_COMPONENT_BT_OBEX_BPP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_BPPC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Basic Printing client module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_BPPC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional BPPC features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_BPPC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate BPPC instance in demo application")
endif()                             

if (DEFINED CSR_COMPONENT_BT_OBEX_BIP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_BIPC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Basic Image Push Initiator module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_BIPC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate BIPC instance in demo application")
endif()                                              

if (DEFINED CSR_COMPONENT_BT_OBEX_BIP_SERVER)                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_BIPS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Basic Image Push Responder module")

csr_define_cache_parameter(CSR_BT_APP_USE_BIPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate BIPS instance in demo application")
endif()         
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_BSL_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude the Personal Area Networking module")
                           
if (DEFINED CSR_COMPONENT_BT_DUNG)
csr_define_cache_parameter(EXCLUDE_CSR_BT_DG_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Dun Gateway module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_DG
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate DUN gateway instance in demo application")
endif()     

if (DEFINED CSR_COMPONENT_BT_DUNC)
csr_define_cache_parameter(EXCLUDE_CSR_BT_DUNC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the DUN-DT profile module")

csr_define_cache_parameter(CSR_BT_APP_USE_DUNC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate DUNC instance in demo application")
endif()     

if (DEFINED CSR_COMPONENT_BT_OBEX_FTP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_FTC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the File Transfer Client module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_FTC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional FTC features, unless they are needed by other enabled modules")

csr_define_cache_parameter(CSR_BT_APP_USE_FTC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate FTP Client instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_FTP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_FTS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the File Transfer Server module")


csr_define_cache_parameter(EXCLUDE_CSR_BT_FTS_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional FTS features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_FTS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate FTP Server instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_GNSS_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_GNSS_CLIENT_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the GNSS Profile Client module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_GNSS_CLIENT_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the optional GNSS client module features (Data request)")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_GNSS_CLIENT
                           TYPE STRING 
                           DESCRIPTION "Initiate GNSS client instance in demo application"
                           DEFAULT "0" STRINGS "0" "1")
endif()

if (DEFINED CSR_COMPONENT_BT_GNSS_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_GNSS_SERVER_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the GNSS Profile server module")

csr_define_cache_parameter(EXCLUDE_CSR_BT_GNSS_SERVER_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the optional GNSS server module features (Data indication and LPM request)")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_GNSS_SERVER
                           TYPE STRING 
                           DESCRIPTION "Number of GNSS server instances in demo application"
                           DEFAULT "0" STRINGS "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11" "12" "13" "14" "15" "16")
endif()

if (DEFINED CSR_COMPONENT_BT_HCRP)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HCRP_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Hardcopy Cable Replacement module")

csr_define_cache_parameter(CSR_BT_APP_USE_HCRPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate HCRP instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_HDP)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HDP_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Health Device Profile module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_HDP
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate HDP instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_HF)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HF_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Hands-Free module")
      
csr_define_cache_parameter(EXCLUDE_CSR_BT_HF_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional HF features, unless they are needed by other enabled modules")

csr_define_cache_parameter(CSR_BT_APP_USE_HF
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate hands free instance in demo application")

if(CSR_HOST_PLATFORM STREQUAL "MDM")
csr_define_cache_parameter(USE_HF_AUDIO
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Use HF audio service")
endif()
endif()

if (DEFINED CSR_COMPONENT_BT_HFG)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HFG_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Hands-Free Gateway module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional HF features, unless they are needed by other enabled modules")

csr_define_cache_parameter(CSR_BT_APP_USE_HFG
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate hands free gateway instance in demo application")

if(CSR_HOST_PLATFORM STREQUAL "MDM")
csr_define_cache_parameter(USE_HFG_AUDIO
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Use HFG audio service")

csr_define_cache_parameter(USE_HFG_RIL
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Use Qualcomm RIL(MODEM) for HFG")
endif()
endif()

if(CSR_HOST_PLATFORM STREQUAL "MDM")
csr_define_cache_parameter(USE_BT_AUDIO_SERVICE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Use bt audio service")
endif()

csr_define_cache_parameter(EXCLUDE_CSR_BT_HID_PARSER_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the HID Parser module")

if (DEFINED CSR_COMPONENT_BT_HIDD)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HIDD_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the HID Device Profile module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_HIDD
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate HID device instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_HIDH)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HIDH_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the HID Host Profile module")

csr_define_cache_parameter(CSR_BT_APP_USE_HIDH
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate HID host instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_HOGH)
csr_define_cache_parameter(EXCLUDE_CSR_BT_HOGH_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Excludes HID-over-GATT Host")
endif()
                           
if (DEFINED CSR_COMPONENT_BT_PAN)
csr_define_cache_parameter(CSR_BT_APP_USE_PAN
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1"
                           DESCRIPTION "Initiate PAN instance in demo application")

csr_define_cache_parameter(EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the BSL module flow control feature which will enable application to receive Extended Data Confirmation for every Extended Data Request it issues")
endif()
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_IWU_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the IWU application module (on top of DG)")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_JSR82_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the JSR-82 module")
 
if (DEFINED CSR_COMPONENT_BT_OBEX_MAP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_MAPC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Message Access Profile Client module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_MAPC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate MAP Client instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_MAP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_MAPS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Message Access Profile Server module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_MAPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate MAP Server instance in demo application")
endif()
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_MCAP_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Multi Channel Adaption Profile module. This module needs to be include when the HDP profile is used")

if (DEFINED CSR_COMPONENT_BT_OBEX_OPP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_OPC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the OBEX push client profile module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_OPC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional OPC features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_OPC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate OPP Client instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_OPP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_OPS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the OBEX push server profile module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_OPS_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional OPS features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_OPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate OPP Server instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_PBAP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_PAC_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the Phone Book Access Client module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_PAC
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate Phonebook access client instance in demo application")
endif()
                           
if (DEFINED CSR_COMPONENT_BT_OBEX_PBAP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_PAS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Phone Book Access Server module")

csr_define_cache_parameter(CSR_BT_APP_USE_PAS
                           TYPE STRING 
                           DESCRIPTION "Initiate Phonebook access server instance in demo application"
                           DEFAULT "0" STRINGS "0" "1")
endif()

if (DEFINED CSR_COMPONENT_BT_SAP_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SAPC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the SIM Access Profile Client module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_SAPC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate SIM access client instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_SAP_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SAPS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the SIM Access Profile Server module")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_SAPS
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate SIM access server instance in demo application")
endif()
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SCO_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude all SCO related code. This flag will be set automatically if none of the enabled profiles needs the SCO-Module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SD_SERVICE_RECORD_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Excludes the Device Identification code embedded within the Service Discovery module. Exclude this to save code space if it is not need. Note: HID and HDP needs the DI in order to be able to run. If EXCLUDE_CSR_BT_SD_MODULE_OPTIONAL is defined, this flag will automatically be set if neither HID nor HDP is enabled")

if (DEFINED CSR_COMPONENT_BT_OBEX_SYNCML_CLIENT)   
csr_define_cache_parameter(EXCLUDE_CSR_BT_SMLC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the OBEX SyncML Transfer Client module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SMLC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional SMLC features, unless they are needed by other enabled modules")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_SYNCML_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SMLS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the OBEX SyncML Transfer Server module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SMLS_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude optional SMLS features, unless they are needed by other enabled modules")
endif()

if (DEFINED CSR_COMPONENT_BT_SPP)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SPP_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude the Serial Port Profile module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SPP_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT ON
                           DESCRIPTION "Exclude optional SPP features")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_SPP
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate SPP instance in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_SYNC_CLIENT)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SYNCC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Synchronization client module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SYNCC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional SYNCC features, unless they are needed by other enabled modules")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_SYNCC
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate SYNC Client in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_OBEX_SYNC_SERVER)
csr_define_cache_parameter(EXCLUDE_CSR_BT_SYNCS_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the Synchronization server module")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SYNCS_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional SYNCS features, unless they are needed by other enabled modules")
endif()

if (DEFINED CSR_COMPONENT_BT_PROX_SRV)
csr_define_cache_parameter(EXCLUDE_CSR_BT_PROX_SRV_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude the LE Proximity Server. Only applicable if building with CSR_BT_LE_ENABLE=1")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_GATT_PROXS
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate Proximity Server in demo application")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_PROXC
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate Proximity Client in demo application")
endif()
                           
if (DEFINED CSR_COMPONENT_BT_THERM_SRV)
csr_define_cache_parameter(EXCLUDE_CSR_BT_THERM_SRV_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude the LE Health Thermometer Server. Only applicable if building with CSR_BT_LE_ENABLE=1")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_THERMS
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate thermometer Server in demo application")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_THERMC
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate thermometer client in demo application")
endif()

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_GENERIC_SERVER
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate LE generic browser in demo application")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_LE_BROWSER
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate LE Browser in demo application")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_HOGH
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate LE HID host in demo application")

csr_define_cache_parameter(CSR_BT_APP_USE_GATT_HOGD
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate LE HID device in demo application")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_GATT_RSCC
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate Run Speed Cadance Client in demo application")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_GATT_RSCS
                           TYPE STRING 
                           DEFAULT "1" STRINGS "0" "1" 
                           DESCRIPTION "Initiate Run Speed Cadance Server in demo application")

csr_define_cache_parameter(EXCLUDE_CSR_BT_TEST_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude the TEST module, also used for demo application tasks")
                           
csr_define_cache_parameter(EXCLUDE_CSR_EXCEPTION_HANDLER_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Enable handling of exception. What to do when an exception occur is depended of the port")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SC_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional SC features, not needed by any of the enabled profiles")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_SD_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude optional SD features, not needed by any of the enabled profiles. Including the SD_SERVICE_RECORD_MODULE unless HID or HDP is enabled. To avoid excluding the SD_SERVICE_RECORD_MODULE simply add the compiler flag INSTALL_CSR_BT_SD_SERVICE_RECORD_MODULE")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_CM_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude the majority of CM API functions not used by the enabled profiles. If only a few of the functions are needed, they can be enabled individually by specifying the corresponding CSR_BT_INSTALL_CM flags. E.g. to enable the Read RSSI API functionality, use the flag CSR_BT_INSTALL_CM_READ_RSSI. Please refer to Appendix A of the CSR Synergy Bluetooth User Guide, document name: gu-0101-users_guide.pdf")
                           
csr_define_cache_parameter(EXCLUDE_CSR_BT_OPTIONAL_UTILS
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude Utility functions not used by the stack")

csr_define_cache_parameter(EXCLUDE_CSR_BT_GOEP_20_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude CSR_BT_GOEP_20_MODULE")
                           
if (DEFINED CSR_COMPONENT_BT_PHDC_AG)
csr_define_cache_parameter(EXCLUDE_CSR_BT_PHDC_AG_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude PHDC_AG_MODULE")

csr_define_cache_parameter(CSR_BT_APP_USE_PHDC_AG
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate PHDC AG in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_PHDC_MGR)
csr_define_cache_parameter(EXCLUDE_CSR_BT_PHDC_MGR_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude PHDC_MGR_MODULE")
                           
csr_define_cache_parameter(CSR_BT_APP_USE_PHDC_MGR
                           TYPE STRING 
                           DEFAULT "0" STRINGS "0" "1" 
                           DESCRIPTION "Initiate PHDC MGR in demo application")
endif()

if (DEFINED CSR_COMPONENT_BT_ASM)
csr_define_cache_parameter(EXCLUDE_CSR_BT_ASM_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude Audio Stream Manager module")
endif()

csr_define_cache_parameter(CSR_BT_BT_VERSION
                           TYPE STRING 
                           DESCRIPTION "This define specifies the Bluetooth Core Specification version supported by CSR Synergy Bluetooth Host stack."
                           DEFAULT "CSR_BT_BLUETOOTH_VERSION_5P0" STRINGS "CSR_BT_BLUETOOTH_VERSION_1P0" "CSR_BT_BLUETOOTH_VERSION_1P1" "CSR_BT_BLUETOOTH_VERSION_1P2" "CSR_BT_BLUETOOTH_VERSION_2P0" "CSR_BT_BLUETOOTH_VERSION_2P1" "CSR_BT_BLUETOOTH_VERSION_3P0" "CSR_BT_BLUETOOTH_VERSION_4P0" "CSR_BT_BLUETOOTH_VERSION_4P1" "CSR_BT_BLUETOOTH_VERSION_4P2" "CSR_BT_BLUETOOTH_VERSION_5P0")

csr_define_cache_parameter(CSR_BT_APP_AV_FILTER
                           TYPE STRING 
                           DESCRIPTION "CSR_BT_APP_AV_FILTER"
                           DEFAULT "sbcwav" STRINGS "aac" "mp3" "sbcwav" "sbcaudio" "avrouter")

csr_define_cache_parameter(ENABLE_TEST_TIMEOUT
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "enable watch dog in test execution")

csr_define_cache_parameter(EXCLUDE_CSR_BT_TPT_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude CSR_BT_TPT_MODULE")

csr_define_cache_parameter(EXCLUDE_CSR_BT_MDER_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude CSR_BT_MDER_MODULE")


csr_define_cache_parameter(EXCLUDE_CSR_BT_SBC_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude CSR_BT_SBC_MODULE")

csr_define_cache_parameter(EXCLUDE_CSR_BT_PPP_MODULE
                           TYPE BOOL 
                           DEFAULT ON 
                           DESCRIPTION "Exclude CSR_BT_PPP_MODULE")

csr_define_cache_parameter(EXCLUDE_CSR_BT_GATT_MODULE_OPTIONAL
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude CSR_BT_GATT_MODULE_OPTIONAL")

csr_define_cache_parameter(EXCLUDE_CSR_BT_GATT_MODULE_OPTIONAL2
                           TYPE BOOL 
                           DEFAULT ON
                           DESCRIPTION "Prevents LE name discovery immediately on GATT connection.")

csr_define_cache_parameter(CSR_BT_LE_SIGNING_ENABLE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Prevents encryption on reconnection with previously bonded device. Preventing encryption allows application to use GATT Signed Write Without Response procedure. Note that security requirements of signing are satisfied with encryption. Thus it is highly recommended to keep this feature disabled.")

csr_define_cache_parameter(EXCLUDE_CSR_BT_L2CA_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude  CSR_BT_L2CA_MODULE")

csr_define_cache_parameter(EXCLUDE_CSR_BT_RFC_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Exclude  CSR_BT_RFC_MODULE")


csr_define_cache_parameter(EXCLUDE_CSR_BT_BNEP_MODULE
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Exclude  EXCLUDE_CSR_BT_BNEP_MODULE")


csr_define_cache_parameter(CSR_BT_INSTALL_INTERNAL_APP_DEPENDENCIES
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "CSR_BT_INSTALL_INTERNAL_APP_DEPENDENCIES")

csr_define_cache_parameter(EXCLUDE_CSR_BT_VCARD_MODULE
                           TYPE BOOL 
                           DEFAULT OFF 
                           DESCRIPTION "Excludes vCard module. This is excluded by default. Applications can choose to enable this module for parsing or generating vCard 2.1 or vCard 3.0.")

csr_define_cache_parameter(CSR_CVC_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enables to use cVc 1Mic voice processing software in BlueCore. CVCHF1MIC  operator will be used for both WB and NB speech")

csr_define_cache_parameter(CSR_CVC_NB_FE_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Expands the frequency while processing cVc for NB speech to WB (ie 8Khz to 16Khz). FE operators will be used instead of CVCHF1MIC NB operators")
                           
csr_define_cache_parameter(CSR_BT_APP_OUTPUT_A2DP_TO_I2S
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Routes the A2DP audio to BlueCore's I2S hardware ")
                           
csr_define_cache_parameter(CSR_DSPM_DISABLE_SCO_RATEMATCHING
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Disables Rate Matching in NB/WB operator. When rate matching is on samples may be inserted or deleted from the stream to allow for small differences in the rate at which samples are inserted into and removed from the operator.")

csr_define_cache_parameter(CSR_DSPM_SRC_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Allow DSPM to use Resampler Capability. Resampler capability takes a mono audio input and changes its sampling rate by upsampling (zero-insertion),filtering (low pass filter) and then down-sampling and/or interpolation.")

csr_define_cache_parameter(CSR_BT_APP_DSP_PATCHING
                           TYPE STRING 
                           DEFAULT "Memory" STRINGS "File" "Memory"
                           DESCRIPTION "DSP patching mechanism: filebased or memory based, Default method is memory based")

csr_define_cache_parameter(CSR_BT_INSTALL_L2CAP_CONNLESS_SUPPORT
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enables L2CAP connectionless feature")
                           
csr_define_cache_parameter(CSR_BT_SC_ONLY_MODE_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enables SC_ONLY mode. Caution: If this flag is enabled, Synergy would work only with devices supporting Secure Connections")
                           
csr_define_cache_parameter(CSR_BT_INSTALL_LESC_SUPPORT
                           TYPE BOOL
                           DEFAULT ON
                           DEPENDS CSR_BT_LE_ENABLE
                           DESCRIPTION "Enables LE Secure Connections support")

csr_define_cache_parameter(CSR_BT_INSTALL_CTKD_SUPPORT
                           TYPE BOOL
                           DEFAULT OFF
                           DEPENDS CSR_BT_INSTALL_LESC_SUPPORT
                           DESCRIPTION "Enables cross transport key derivation. Caution: This is an experimental feature and is recommended to be enabled only for advanced development.")

csr_define_cache_parameter(CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT
                           TYPE BOOL
                           DEFAULT ON
                           DEPENDS CSR_BT_LE_ENABLE
                           DESCRIPTION "Enables LE Privacy 1.2 support")

csr_define_cache_parameter(CSR_BT_LE_RANDOM_ADDRESS_TYPE
                           TYPE STRING
                           DEFAULT "RPA" STRINGS "RPA" "NRPA" "STATIC"
                           DESCRIPTION "Configures the type of local random address to be generated among RPA, NRPA & STATIC. Note : If it's value is set to STATIC, application may also provide its static address either by configuring it through user config file or using CM interface, if not provided by any ways then it would be generated internally. In case of NRPA, Synergy does not allow bonding with peer device")
