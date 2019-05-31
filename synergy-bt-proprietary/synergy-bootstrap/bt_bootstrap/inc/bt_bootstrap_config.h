/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _BT_BOOTSTRAP_CONFIG_H_
#define _BT_BOOTSTRAP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Path to store BT firmware file. */
#define BT_FIRMWARE_PATH                    "/firmware/image"

/* Path to store BT config file. */
#define BT_CONFIG_PATH                      "/etc/bluetooth"

/* BT address file name. */
#define BT_ADDR_FILE_NAME                   "bt.inf"

/* Config file name for BT firmware. */
#define BT_FIRMWARE_CONFIG_FILE_NAME        "firmware.conf"

/* Path to store BT bootstrap log. */
#define BT_BOOTSTRAP_LOG_PATH               "/data"

/* BT log file for debug message output. */
#define BT_BOOTSTRAP_LOG_FILE_NAME          "bt_bootstrap.log"
#define BT_BOOTSTRAP_LOG_FILE               BT_BOOTSTRAP_LOG_PATH "/" BT_BOOTSTRAP_LOG_FILE_NAME

/* BT snoop log file for HCI dump. */
#define BTSNOOP_LOG_FILE_NAME               "hci_bootstrap.cfa"
#define BTSNOOP_LOG_FILE                    BT_BOOTSTRAP_LOG_PATH "/" BTSNOOP_LOG_FILE_NAME


#ifdef __cplusplus
}
#endif

#endif  /* _BT_BOOTSTRAP_CONFIG_H_ */
