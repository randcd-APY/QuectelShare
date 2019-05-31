/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WDS_HCI_PFAL_LOOPBACK__H
#define __WDS_HCI_PFAL_LOOPBACK__H
#ifndef DEBUG
#define DEBUG   printf
#endif

#ifndef ERROR
#define ERROR   printf
#endif

#include "bt_vendor_qcom.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

typedef struct {
    uint8  lb_enabled;
    uint16 pkt_delay;
    uint32 cmd_pkt_success;
    uint32 cmd_pkt_failure;
    uint32 max_pkt_lb_cnt;
    uint8 *tx_pkt_buffer;
    uint8 *rx_pkt_buffer;

    volatile uint8 valid_signal;

    pthread_t loopback_rw_thread;

    pthread_mutex_t tx_q_lock;
    pthread_mutex_t rx_q_lock;
    pthread_cond_t event_cond;
}hci_loopback_pkt_info;

typedef struct {
    uint8 lb_pkt_type; //Looback Packet type: Packet types to Loopback: BT/FM/BT+FM/ANT/BT+ANT/FM+ANT/BT+FM+ANT
    uint8 lb_exec_type; //Loopback execution type: Continuous or fixed no. of packets to loopback
    uint8 lb_suspend_enable;
    uint8 lb_enable_status;
    uint8 ssr_in_prog;

    pthread_mutex_t lb_ssr_lock;
    pthread_mutex_t lb_enable_lock;
    pthread_mutex_t lb_error_log_file_lock;

    hci_loopback_pkt_info pkt_type_bt;
    hci_loopback_pkt_info pkt_type_fm;
    hci_loopback_pkt_info pkt_type_ant;

    time_t time_stamp;
    struct tm *tm_info;

    FILE *lb_cfg_file_fd;
}hci_loopback_core_struct;



#define BT_HCI_COMMAND_PKT_LEN           (256)
#define BT_ACL_DATA_PKT_LEN              (1024)
#define FM_HCI_COMMAND_PKT_LEN           (256)
#define ANT_HCI_COMMAND_PKT_LEN          (256)
#define ANT_ACL_DATA_PKT_LEN             (1024)

#define READ_FROM_CONSOLE                (0)
#define READ_FROM_FILE                   (1)
#define SET_DEFAULT_OPTIONS              (2)

#define BT_SHORT_CMD                     (1)
#define BT_LONG_CMD                      (2)

#define BT_SHORT_CMD_SIZE                (32)
#define BT_LONG_CMD_SIZE                 (248)
#define BT_ACL_PKT_MAX_SIZE              (1019)
#define FM_CMD_PKT_MAX_SIZE              (248)
#define DEFAULT_ACL_HANDLE               (0x0001)

#define MAX_PKT_BYTE                     (0xFF)

#define BT_COMMAND                       (1)
#define BT_ACL_DATA                      (2)

#define HCI_INQUIRY                      (0x0401)
#define HCI_CHANGE_LOCAL_NAME            (0x0417)
#define HCI_FM_RX_ENABLE                 (0x4C01)

#define ANT_COMMAND                      (1)
#define ANT_ACL_DATA                     (2)

#define HCI_PKT_IND_OFFSET               (0)
#define HCI_CMD_PKT_OPCODE_OFFSET        (1)
#define HCI_CMD_PKT_LEN_OFFSET           (3)
#define HCI_CMD_PKT_PARAM_OFFSET         (4)

#define HCI_EVT_PKT_EVT_CODE_OFFSET      (1)
#define HCI_EVT_PKT_LEN_OFFSET           (2)
#define HCI_EVT_PKT_CMD_CREDITS_OFFSET   (3)
#define HCI_EVT_PKT_OPCODE_OFFSET        (4)
#define HCI_EVT_PKT_CMD_STATUS_OFFSET    (6)

#define HCI_LB_EVT_PKT_PARAM_OFFSET      (3)

#define HCI_ACL_PKT_HANDLE_OFFSET        (1)
#define HCI_ACL_PKT_LENGTH_OFFSET        (3)

#define HCI_PKT_IND_SIZE                 (1)
#define HCI_CMD_PKT_HDR_SIZE             (3)
#define HCI_EVT_PKT_HDR_SIZE             (2)
#define HCI_ACL_PKT_HDR_SIZE             (4)

#define HCI_ACL_PKT_HANDLE_SIZE          (2)
#define HCI_ACL_PKT_LENGTH_SIZE          (2)

#define QCA_DEBUG_ACL_LOG_HANDLE         (0x2EDC)

#define LOOPBACK_MODE_DISABLED           (0)
#define LOOPBACK_MODE_BT                 (1)
#define LOOPBACK_MODE_FM                 (2)
#define LOOPBACK_MODE_BT_FM              (3)
#define LOOPBACK_MODE_ANT                (4)
#define LOOPBACK_MODE_BT_ANT             (5)
#define LOOPBACK_MODE_FM_ANT             (6)
#define LOOPBACK_MODE_BT_FM_ANT          (7)
#define LOOPBACK_MODE_TRIGGER_SSR        (8)

#define LOOPBACK_PKT_TIMEOUT             (2) //timeout in seconds
#define LB_STATUS_DISABLED               (0)
#define LB_STATUS_ENABLED                (1)

#define LB_TYPE_CONTINUOUS               (0)
#define LB_TYPE_MAX_PKT_CNT              (1)

#define HCI_LOOPBACK_EVENT               (0x19)
#define HCI_CC_EVENT                     (0x0E)

#define HCI_INTRA_PKT_DELAY              (1) /* now in sec */
#define CTXT_SWITCH_DELAY                (1000 * 100) /* 100 ms */
#define SSR_COMPLETE_DELAY               (5)

#define INPUT_FILE_NAME                  "/data/misc/bluetooth/lb_cfg.txt"
#define ERROR_LOGING_FILE_PATH           "/data/misc/bluetooth/lb_error_log"

#define ACL_6P_CMD_4P                    (0)
#define ACL_1P_CMD_1P                    (1)

int init_loopback_test_mode(wdsdaemon *wds);
void lb_deinit(hci_loopback_core_struct *ptr_hci_lb_core);
#endif /* ifndef __WDS_HCI_PFAL_LOOPBACK__H */
