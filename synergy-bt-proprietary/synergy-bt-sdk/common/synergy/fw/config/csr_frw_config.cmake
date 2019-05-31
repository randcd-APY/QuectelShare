##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_define_cache_parameter(CSR_DSPM_FORCE_PATCH_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Force DSPM patching during Synergy Host DSPM Activate")
						   
csr_define_cache_parameter(CSR_MASK_ERROR_REASON_VALUES
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Mask passing of Synergy Frw DSPM Error Reason Values to Synergy Apps")
						   
csr_define_cache_parameter(CSR_HYDRA_SSD
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable Synergy Service driver for Hydra")

csr_define_cache_parameter(CSR_USE_BCSP_HTRANS
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable Synergy BCSP over Hydra Transport")

csr_define_cache_parameter(CSR_SDIO_USE_SDIO
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable SDIO support")

csr_define_cache_parameter(CSR_SDIO_USE_CSPI
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable CSPI support")

csr_define_cache_parameter(CSR_SDIO_ASYNC_ENABLE
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable SDIO async support")

csr_define_cache_parameter(CSR_HCI_SOCKET_TRANSPORT
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable BlueZ HCI socket transport support in Linux BSP. Framework example app will use this for USB transport. **Note: Requires CSR_ALLOW_NON_ANSI=ON since BlueZ headers won't compile with -ansi.")

csr_define_cache_parameter(CSR_IP_MAX_ETHERS
                           TYPE STRING
                           DEFAULT 8
                           DESCRIPTION "Maximum number of IP ether interfaces")

csr_define_cache_parameter(CSR_IP_MAX_SOCKETS
                           TYPE STRING
                           DEFAULT 16
                           DESCRIPTION "Maximum number of concurrent IP sockets"
                           )

csr_define_cache_parameter(CSR_TLS_MAX_SOCKETS
                           TYPE STRING
                           DEFAULT 8
                           DESCRIPTION "Maximum number of concurrent TLS sockets")

csr_define_cache_parameter(CSR_IP_SUPPORT_FLOWCONTROL
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable IP ether flow control support")

csr_define_cache_parameter(CSR_IP_SUPPORT_ETHER
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable IP ether support")

csr_define_cache_parameter(CSR_IP_SUPPORT_IFCONFIG
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable IP ifconfig support")

csr_define_cache_parameter(CSR_IP_SUPPORT_TLS
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable TLS support")

csr_define_cache_parameter(CSR_TLS_SUPPORT_PSK
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable TLS PSK support")

csr_define_cache_parameter(CSR_USE_STDC_LIB
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable Standard C library mapping for utility functions")

csr_define_cache_parameter(CSR_MEMALLOC_PROFILING
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable profiling of memory allocations")

csr_define_cache_parameter(CSR_PMEM_DEBUG
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable passing file and line numbers to CsrPmemAlloc")

#--------------------------------------------------------------------------
# Defines for the application framework
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_SCHEDULER_INSTANCES
                           TYPE STRING
                           DEFAULT 3
                           DESCRIPTION "Number of scheduler threads in application framework")

#--------------------------------------------------------------------------
# Defines for the generic scheduler
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_SCHED_MESSAGE_POOL_LIMIT
                           TYPE STRING
                           DEFAULT 10
                           DESCRIPTION "Maximum number of messages in per-scheduler instance free list")

csr_define_cache_parameter(CSR_SCHED_TIMER_POOL_LIMIT
                           TYPE STRING
                           DEFAULT 100
                           DESCRIPTION "Maximum number of timers per scheduler instance")

#--------------------------------------------------------------------------
# Defines for the BlueCore bootstrap procedure
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_BLUECORE_RESET_TIMER
                           TYPE STRING
                           DEFAULT 500000
                           DESCRIPTION "The fixed time (in us) to wait after a reset command, before transport is restarted")

csr_define_cache_parameter(CSR_BLUECORE_ONOFF
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enables an application to control activation and deactivation of the BlueCore")

csr_define_cache_parameter(CSR_BLUECORE_RESET_TIMEOUT
                           TYPE STRING
                           DEFAULT 5000000
                           DESCRIPTION "The maximum time (in us) to wait for BlueCore to come alive after sending a reset command")

csr_define_cache_parameter(CSR_BLUECORE_PING_INTERVAL
                           TYPE STRING
                           DEFAULT 5000000
                           DESCRIPTION "Periodic ping timer interval to monitor BlueCore")

csr_define_cache_parameter(CSR_BCCMD_CMD_TIMEOUT
                           TYPE STRING
                           DEFAULT 2000000
                           DESCRIPTION "The maximum time (in us) to wait for the response to a BlueCore command")

#--------------------------------------------------------------------------
# Defines for Type-A
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_TYPE_A_SLEEP_TIMEOUT
                           TYPE STRING
                           DEFAULT 0
                           DESCRIPTION "The maximum time (in us) to wait for the response to a BlueCore command")

csr_define_cache_parameter(CSR_TYPE_A_BUSSPEED_AWAKE
                           TYPE STRING
                           DEFAULT 12500000
                           DESCRIPTION "The maximum bus speed during normal operation")

csr_define_cache_parameter(CSR_TYPE_A_WAKEUP_TIMEOUT
                           TYPE STRING
                           DEFAULT 125
                           DESCRIPTION "Type-A deep sleep wakeup delay in ms")

csr_define_cache_parameter(CSR_TYPE_A_ENABLE_DELAY
                           TYPE STRING
                           DEFAULT 1000
                           DESCRIPTION "Type-A initialisation delay in ms")

csr_define_cache_parameter(CSR_TYPE_A_WAKEUP_DELAY
                           TYPE STRING
                           DEFAULT 1000
                           DESCRIPTION "Time to wait for chip wakeup in ms")

csr_define_cache_parameter(CSR_TYPE_A_RXBUF_POOLSIZE
                           TYPE STRING
                           DEFAULT 32
                           DESCRIPTION "Type-A Rx buffer pool size")

csr_define_cache_parameter(CSR_TYPE_A_RXBUF_ELMSIZE
                           TYPE STRING
                           DEFAULT "8 * 1024"
                           DESCRIPTION "Type-A Rx buffer element size")

csr_define_cache_parameter(CSR_TYPE_A_TXBUF_SIZE
                           TYPE STRING
                           DEFAULT "64 * 1024 + 4 + 4"
                           DESCRIPTION "Type-A Tx buffer size")

#--------------------------------------------------------------------------
# Defines for BCSP
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_ABCSP_TXCRC
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable CRC check on Tx messages")

csr_define_cache_parameter(CSR_ABCSP_RXCRC
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable CRC check on Rx messages")

csr_define_cache_parameter(CSR_BCSP_AUTO_TIMER
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable BCSP auto retransmission timer")

csr_define_cache_parameter(CSR_BCSP_RETRANSMISSION_TIMER
                           TYPE STRING
                           DEFAULT 250000
                           DESCRIPTION "BCSP retransmission timer")

csr_define_cache_parameter(CSR_BCSP_RETRANSMISSION_JITTER
                           TYPE STRING
                           DEFAULT 10000
                           DESCRIPTION "BCSP retransmission jitter")

csr_define_cache_parameter(CSR_BCSP_RETRANSMISSION_MINIMUM
                           TYPE STRING
                           DEFAULT 10000
                           DESCRIPTION "BCSP retransmission minimum")

csr_define_cache_parameter(CSR_BCSP_TSHY_TIMER
                           TYPE STRING
                           DEFAULT 250000
                           DESCRIPTION "BCSP TSHY timer")

csr_define_cache_parameter(CSR_BCSP_TCONF_TIMER
                           TYPE STRING
                           DEFAULT 250000
                           DESCRIPTION "BCSP TCONF timer")

csr_define_cache_parameter(CSR_ABCSP_TXWINSIZE
                           TYPE STRING
                           DEFAULT 4
                           DESCRIPTION "The size of the BCSP transmit window"
                           STRINGS 1 2 3 4 5 6 7)

#--------------------------------------------------------------------------
# Defines for FastPipe
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_FP_CONTROLER_CREDIT_MAX
                           TYPE STRING
                           DEFAULT 3132
                           DESCRIPTION "FastPipe controller credit max")

csr_define_cache_parameter(CSR_FP_CONTROLLER_PACKET_SIZE_MAX
                           TYPE STRING
                           DEFAULT 1019
                           DESCRIPTION "FastPipe controller packet size max")

csr_define_cache_parameter(CSR_FP_HOST_PACKET_SIZE_MAX
                           TYPE STRING
                           DEFAULT 1
                           DESCRIPTION "FastPipe host packet size max")

csr_define_cache_parameter(CSR_FP_PACEKTS_MAX
                           TYPE STRING
                           DEFAULT 7
                           DESCRIPTION "FastPipe packets max")

#--------------------------------------------------------------------------
# Defines for Csr Log
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_LOG_TEXT_MAX_STRING_LEN
                           TYPE STRING
                           DEFAULT 255
                           DESCRIPTION "Max size log string")

csr_define_cache_parameter(CSR_LOG_PRIM_SIZE_UPPER_LIMIT
                           TYPE STRING
                           DEFAULT 64
                           DESCRIPTION "Upper limit on bytes written to scheduler log message entry")

csr_define_cache_parameter(CSR_LOG_CLEARTEXT_FORMAT
                           TYPE STRING
                           DEFAULT "CSR_LOG_CLEARTEXT_TEMPLATE_YEAR \"/\" CSR_LOG_CLEARTEXT_TEMPLATE_MONTH \"/\" CSR_LOG_CLEARTEXT_TEMPLATE_DAY CSR_LOG_CLEARTEXT_TEMPLATE_HOUR \":\" CSR_LOG_CLEARTEXT_TEMPLATE_MIN \":\" CSR_LOG_CLEARTEXT_TEMPLATE_TIME_SEC \":\" CSR_LOG_CLEARTEXT_TEMPLATE_TIME_MSEC \" \" CSR_LOG_CLEARTEXT_TEMPLATE_TASK_NAME \" \" CSR_LOG_CLEARTEXT_TEMPLATE_SUBORIGIN_NAME \" \" CSR_LOG_CLEARTEXT_TEMPLATE_LOG_LEVEL_NAME \": \" CSR_LOG_CLEARTEXT_TEMPLATE_STRING CSR_LOG_CLEARTEXT_TEMPLATE_BUFFER"
                           DESCRIPTION "Output format for cleartext logger")

#--------------------------------------------------------------------------
# Defines for the IP stack
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_IP_USE_BIG_ENDIAN
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "IP stack endianess")

#--------------------------------------------------------------------------
# Defines for the CSR_DATA_STORE
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_DATA_STORE_ROOT_DIR
                           TYPE STRING
                           DEFAULT "./data_store/"
                           DESCRIPTION "Root data store dir")

csr_define_cache_parameter(CSR_DATA_STORE_DEFAULT_ENTRY_NAME
                           TYPE STRING
                           DEFAULT "ds_info.cdi"
                           DESCRIPTION "Data store default entry")

#--------------------------------------------------------------------------
# Defines for Chip Manager
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_BLUECORE_DEFAULT_PING_INTERVAL
                           TYPE STRING
                           DEFAULT 5000000
                           DESCRIPTION "Number of microseconds between sending PING request")

#--------------------------------------------------------------------------
# Defines for DSPM
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_DSPM_SUPPORT_CAPABILITY_DOWNLOAD
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable support for downloading capabilities in DSPM")

csr_define_cache_parameter(CSR_DSPM_SUPPORT_ACCMD
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Enable support for ACCMDs in DSPM")
#--------------------------------------------------------------------------
# Defines for pclin DHCP client configuration
#--------------------------------------------------------------------------
csr_define_cache_parameter(DHCPCD_PATH
                           TYPE STRING
                           DEFAULT "/system/bin/dhcpcd"
                           DESCRIPTION "Full path to dhcpcd (used on Android)")

csr_define_cache_parameter(DHCLIENT_PATH
                           TYPE STRING
                           DEFAULT "/sbin/dhclient"
                           DESCRIPTION "Full path to dhclient (used on vanilla Linux)")

#--------------------------------------------------------------------------
# Defines for DHCP_SERVER
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_DHCP_SERVER_USE_IFCONFIG_ARP
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable unicasting of DHCP messages")

#--------------------------------------------------------------------------
# Defines for BTM
#--------------------------------------------------------------------------
csr_define_cache_parameter(CSR_BTM_TASK
                           TYPE BOOL 
                           DEFAULT OFF
                           DESCRIPTION "Enable use of BTM task")





