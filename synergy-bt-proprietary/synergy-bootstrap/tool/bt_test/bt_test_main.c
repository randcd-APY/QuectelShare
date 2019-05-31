/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "connx_arg_search.h"
#include "connx_util.h"
#include "connx_list.h"
#include "connx_time.h"
#include "connx_log.h"
#include "connx_log_setting.h"
#include "bt_h4_transport.h"
#include "product_test_cmd.h"


#define DEFAULT_UART_PORT                           "/dev/ttyHS0"
#define DEFAULT_UART_BAUDRATE                       3000000         /* Assume QCA BT chip is booted up. */

#define DEFAULT_BT_TEST_CONFIG_PATH                 "/data"
#define BT_TEST_CONFIG_FILE                         "bt_test.conf"
#define BT_TEST_SNOOP_FILE                          "bt_test.cfa"
#define BT_TEST_LOG_FILE                            "bt_test.log"

/* Delay in second */
#define DEFAULT_BT_TEST_DELAY                       1

#define PROD_TEST_SLAVE_STR                         "slave"
#define PROD_TEST_MASTER_STR                        "master"
#define PROD_TEST_STATS_STR                         "stats"
#define PROD_TEST_TX_BURST_STR                      "tx_burst"
#define PROD_TEST_TX_CONTINUOUS_STR                 "tx_continous"
#define PROD_TEST_RX_BURST_STR                      "rx_burst"
#define PROD_TEST_RX_BURST_CONFIG_STR               "rx_burst_config"

#define DEBUG_READ_RSSI_STR                         "rssi"

#define PROD_TEST_CMD_FORMAT                        "%s "
#define PROD_TEST_CMD_FIELD_COUNT                   1

/* hop_channels, tx_pattern_payload, packet_type, data_whitening, tx_output_power, receiver_gain, target_addr, hopping_func, payload_length, logical_transport_addr */
#define PROD_TEST_COMMON_CMD_FORMAT                 "%s = %02X-%02X-%02X-%02X-%02X, %02X, %02X, %02X, %02X, %02X, %02X-%02X-%02X-%02X-%02X-%02X, %02X, %04X, %02X "
#define PROD_TEST_COMMON_CMD_FIELD_COUNT            20

/* channel_number, tx_output_power, tx_type, packet_length, bit_pattern */
#define PROD_TEST_TX_CONTINUOUS_CMD_FORMAT          "%s = %02X, %02X, %02X, %02X, %08X "
#define PROD_TEST_TX_CONTINUOUS_CMD_FIELD_COUNT     6

/* number_of_packets */
#define PROD_TEST_RX_BURST_CONFIG_CMD_FORMAT        "%s = %04X "
#define PROD_TEST_RX_BURST_CONFIG_CMD_FIELD_COUNT   2

/* Channel offset, Number of samples */
#define READ_RSSI_CMD_FORMAT                        "%s = %02X, %02X "
#define READ_RSSI_CMD_FIELD_COUNT                   3

/* Timeout in ms. */
#define WAIT_TIMEOUT_IN_MS                          5000

#define EVENT_BIT_PROD_TEST_SLAVE                   0x1000
#define EVENT_BIT_PROD_TEST_MASTER                  0x1001
#define EVENT_BIT_PROD_TEST_STATS                   0x1002
#define EVENT_BIT_PROD_TEST_TX_BURST                0x1003
#define EVENT_BIT_PROD_TEST_TX_CONTINUOUS           0x1004
#define EVENT_BIT_PROD_TEST_RX_BURST                0x1005
#define EVENT_BIT_PROD_TEST_RX_BURST_CONFIG         0x1006

#define EVENT_BIT_READ_RSSI                         0x2000

#define EVENT_BIT_READ_NVM                          0x3000

#define EVENT_BIT_ENABLE_DUT                        0x4000
#define EVENT_BIT_WRITE_SCAN_ENABLE                 0x4001
#define EVENT_BIT_SET_EVENT_FILTER                  0x4002
#define EVENT_BIT_WRITE_AUTH_ENABLE                 0x4003
#define EVENT_BIT_WRITE_ENC_MODE                    0x4004

#define EVENT_BIT_RESET                             0x5000

#define EVENT_BIT_EXIT                              0xF000

#define EVENT_BIT_ALL                               0xFFFF
#define EVENT_BIT_UNKNOWN                           0xFFFF

#define BT_TEST_FUNC_ENTER()                        BT_FUNC_ENTER()
#define BT_TEST_FUNC_LEAVE()                        BT_FUNC_LEAVE()

#define OUTPUT_HCI_STATUS(status, info)             { \
                                                        if (IS_HCI_SUCCESS(status)) \
                                                        { \
                                                            IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("%s succeed"), info)); \
                                                        } \
                                                        else \
                                                        { \
                                                            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("%s fail, status: 0x%02X"), info, status)); \
                                                        } \
                                                    }

#undef CHK
#define CHK(res, info)                              { \
                                                        if (!IS_CONNX_RESULT_SUCCESS(res)) \
                                                        { \
                                                            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> %s fail"), __FUNCTION__, info)); \
                                                            break; \
                                                        } \
                                                    }


/* HCI SetEventFilter command */
/* --------------------------------------------------------- */
typedef struct
{
     uint24_t  class_of_device;
     uint24_t  mask;
} CLASS_MASK_T;

typedef struct
{
    CLASS_MASK_T    class_mask;
    uint8_t         auto_accept;
} CLASS_MASK_AUTO_T;

typedef struct
{
    BT_ADDR_T       bd_addr;
    uint8_t         auto_accept;
} ADDR_AUTO_T;

typedef union
{
    CLASS_MASK_T        class_mask;                     /* type 1 condtype 1 */
    BT_ADDR_T           bd_addr;                        /* type 1 condtype 2 */
    uint8_t             auto_accept;                    /* type 2 condtype 0 */
    CLASS_MASK_AUTO_T   cma;                            /* type 2 condtype 1 */
    ADDR_AUTO_T         addr_auto;                      /* type 2 condtype 2 */
} CONDITION_T;

typedef struct
{
    uint8_t                 filter_type;
    uint8_t                 filter_condition_type;
    CONDITION_T             condition;
} HCI_SET_EVENT_FILTER_T;
/* --------------------------------------------------------- */


typedef struct
{
    uint32_t        size;
    char           *serial_port;            /* Device name for UART port. */
    uint32_t        baudrate;               /* Actual UART baudrate after BT is booted up. */
    bool            flow_control;           /* true: enable h/w flow control, false: disable. */
    char           *config_path;            /* Path to store QCA test configuration & BT snoop file. */
    QcaSubOpcodeT   test_cmd;               /* QCA Product test command. */
    QcaSubOpcodeT   debug_cmd;              /* QCA debug command, such as "Read RSSI". */
    uint8_t         nvm_tag;                /* Tag used in NVM read command. */
    bool            enable_dut;             /* true: enable device under test mode, false: not. */
    char           *hci_cmd;                /* Raw HCI command (without H4 header). */
    char           *hci_cmd_file;           /* File which store raw HCI command (without H4 header). */
    uint32_t        delay;                  /* Delay in second. */
} QcaTestParameter;

typedef struct
{
    bool                            valid_slave_cmd;
    PROD_TEST_SLAVE_CMD_T           slave_cmd;

    bool                            valid_master_cmd;
    PROD_TEST_MASTER_CMD_T          master_cmd;

    bool                            valid_tx_burst_cmd;
    PROD_TEST_TX_BURST_CMD_T        tx_burst_cmd;

    bool                            valid_tx_continous_cmd;
    PROD_TEST_TX_CONTINUOUS_CMD_T   tx_continous_cmd;

    bool                            valid_rx_burst_cmd;
    PROD_TEST_RX_BURST_CMD_T        rx_burst_cmd;

    bool                            valid_rx_burst_config_cmd;
    PROD_TEST_RX_BURST_CONFIG_CMD_T rx_burst_config_cmd;

    bool                            valid_read_rssi_cmd;
    READ_RSSI_CMD_T                 read_rssi_cmd;
} QcaTestConfiguration;

typedef struct
{
    QcaTestParameter            test_para;                  /* Test parameter. */
    QcaTestConfiguration        test_config;                /* Test configuration parsed from the file. */

    ConnxHandle                 event_handle;               /* Event handle to wait for HCI event. */

    hci_event_code_t            event_code;                 /* HCI event code. */
    HCI_EV_COMMAND_STATUS_T     hci_ev_command_status;      /* HCI command status event. */
    HCI_EV_COMMAND_COMPLETE_T   hci_ev_command_complete;    /* HCI command complete event. */

    hci_return_t                slave_cmd_status;           /* Status in "command status event" for PROD_TEST_SLAVE command. */
    hci_return_t                master_cmd_status;          /* Status in "command status event" for PROD_TEST_MASTER command. */
    hci_return_t                stats_cmd_status;           /* Status in "command status event" for PROD_TEST_STATS command. */
    PROD_TEST_STATS_EVT_T       stats_cmd_result;           /* "vendor event" for PROD_TEST_STATS command. */
    hci_return_t                tx_burst_cmd_status;        /* Status in "command status event" for PROD_TEST_TX_BURST command. */
    hci_return_t                tx_continous_cmd_status;    /* Status in "command complete event" for PROD_TEST_TX_CONTINUOUS command. */
    hci_return_t                rx_burst_cmd_status;        /* Status in "command status event" for PROD_TEST_TX_BURST command. */
    hci_return_t                rx_burst_config_cmd_status; /* Status in "command status event" for PROD_TEST_RX_BURST_CONFIG command. */

    HCI_EV_READ_RSSI_T          read_rssi_result;           /* "command complete event" for Read RSSI command. */

    READ_NVM_EVT_T              nvm_response;               /* Response for NVM read command. */

    hci_return_t                enable_dut_status;          /* Status for HCI EnableDUT command  */
    hci_return_t                write_scan_enable_status;   /* Status for HCI WriteScanEnable command */
    hci_return_t                set_event_filter_status;    /* Status for HCI SetEventFilter command */
    hci_return_t                write_auth_status;          /* Status for HCI WriteAuthentication command */
    hci_return_t                write_enc_mode_status;      /* Status for HCI WriteEncryption command */
    hci_return_t                reset_status;               /* Status for HCI Reset command */
    hci_return_t                generic_hci_cmd_status;     /* Status for generic HCI command */

    ConnxCmnList_t              hci_cmd_list;               /* HCI command list */
    bool                        issue_hci_cmd;              /* true: raw hci command is issued. false: not */
} QcaTestInstance;


static QcaTestInstance  g_test_inst;


static QcaSubOpcodeT Map2ProdTestCmd(char *str);
static QcaSubOpcodeT Map2DebugCmd(char *str);
static uint32_t MapProdTestCmd2EventBit(QcaSubOpcodeT test_cmd);
static uint32_t MapDebugCmd2EventBit(QcaSubOpcodeT debug_cmd);
static void DumpProdTestCommonCmd(PROD_TEST_COMMON_CMD_T *cmd);
static void DumpProdTestTxContinousCmd(PROD_TEST_TX_CONTINUOUS_CMD_T *cmd);
static void DumpProdTestRxBurstConfigCmd(PROD_TEST_RX_BURST_CONFIG_CMD_T *cmd);
static void DumpReadRssiCmd(READ_RSSI_CMD_T *cmd);
static void DumpTestParameter(QcaTestParameter *test_para);

static bool InitTest(QcaTestInstance *inst);
static void DeinitTest(QcaTestInstance *inst);
static bool InitTestParameter(QcaTestInstance *inst);
static bool InitTestConfig(QcaTestInstance *inst);
static void InitLog(QcaTestInstance *inst);
static void DeinitLog(QcaTestInstance *inst);

static bool ParseProdTestCommonCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_COMMON_CMD_T *cmd);
static bool ParseProdTestTxContinousCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_TX_CONTINUOUS_CMD_T *cmd);
static bool ParseProdTestRxBurstConfigCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_RX_BURST_CONFIG_CMD_T *cmd);
static bool ParseReadRssiCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, READ_RSSI_CMD_T *cmd);
static bool ParseConfigFile(char *config_file, QcaTestConfiguration *test_config);

static bool OpenH4(QcaTestInstance *inst);
static void CloseH4(QcaTestInstance *inst);

static void StartTest(QcaTestInstance *inst);
static void HandleTestCmd(QcaTestInstance *inst);
static void HandleDebugCmd(QcaTestInstance *inst);
static void HandleNvmCmd(QcaTestInstance *inst);
static void HandleEnableDutCmd(QcaTestInstance *inst);
static void HandleHciCmd(QcaTestInstance *inst);
static void HandleHciCmdFile(QcaTestInstance *inst);

static void OutputStatistics(PROD_TEST_STATS_EVT_T *stats);
static void OutputProductTestResult(QcaTestInstance *inst, QcaSubOpcodeT test_cmd);
static void OutputDebugResult(QcaTestInstance *inst, QcaSubOpcodeT debug_cmd);
static void OutputNvmResponse(READ_NVM_EVT_T *response);

static ConnxResult QCA_IssueProductTestCmd(QcaTestInstance *inst, uint8_t length, void *data);
static ConnxResult QCA_IssueProductTestCommonCmd(QcaTestInstance *inst, PROD_TEST_COMMON_CMD_T *cmd);
static ConnxResult QCA_IssueSlaveCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueMasterCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueStatsCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueTxBurstCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueTxContinousCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueRxBurstCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueRxBurstConfigCmd(QcaTestInstance *inst);

static ConnxResult QCA_IssueDebugCmd(QcaTestInstance *inst, uint8_t length, void *data);
static ConnxResult QCA_IssueReadRssiCmd(QcaTestInstance *inst);
static ConnxResult QCA_IssueReadNvmCmd(QcaTestInstance *inst, uint8_t nvm_tag);

static ConnxResult QCA_IssueEnableDutCmd(QcaTestInstance *inst);
static ConnxResult QCA_EnableDut(QcaTestInstance *inst);

static ConnxResult QCA_IssueWriteScanEnableCmd(QcaTestInstance *inst, uint8_t scan_enable);
static ConnxResult QCA_EnableScan(QcaTestInstance *inst);

static ConnxResult QCA_IssueSetEventFilterCmd(QcaTestInstance *inst, uint8_t cmd_length, uint8_t *cmd);
static ConnxResult QCA_EnableAutoConnect(QcaTestInstance *inst);

static ConnxResult QCA_IssueWriteAuthCmd(QcaTestInstance *inst, bool enable);
static ConnxResult QCA_IssueWriteEncModeCmd(QcaTestInstance *inst, bool enable);

static ConnxResult QCA_IssueResetCmd(QcaTestInstance *inst);

static ConnxResult QCA_IssueHciCmd(QcaTestInstance *inst, uint8_t *hci_cmd_data, uint32_t hci_cmd_size);

static void StoreEventCode(QcaTestInstance *inst, hci_event_code_t event_code);
static bool ParseCommandStatusEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data);
static bool ParseVendorEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data);
static bool ParseCommandCompleteEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data);
static bool ParseTestEvent(QcaTestInstance *inst, HCI_EV_GENERIC_T *hci_event);
static void HandleTestEvent(QcaTestInstance *inst);
static uint32_t HandleH4PacketReceived(const uint8_t *data, uint32_t dataLength);

static bool VerifyHciCmd(uint8_t *buf, uint32_t buf_size);
static bool ParseHciCmd(char *hci_cmd, uint8_t **buf, uint32_t *buf_size);
static bool ParseHciCmdFile(char *file_name, ConnxCmnList_t *hci_cmd_list);

static void InitHciCmdList(ConnxCmnList_t *hci_cmd_list);
static void DeinitHciCmdList(ConnxCmnList_t *hci_cmd_list);
static bool AddHciCmd(ConnxCmnList_t *hci_cmd_list, uint8_t *buf, uint32_t size);
static void FreeHciCmd(ConnxCmnListElm_t *elem);

static void ProcessHciCmdList(QcaTestInstance *inst);

static ConnxResult WaitTestEvent(QcaTestInstance *inst, uint16_t timeoutInMs, uint32_t eventBits);
static void SetTestEvent(QcaTestInstance *inst, uint32_t eventBits);

static void BlockTest(QcaTestInstance *inst, QcaSubOpcodeT test_cmd);

/* ------------------------------------------------------------------------------- */

static QcaSubOpcodeT Map2ProdTestCmd(char *str)
{
    if (!str)
        return PROD_TEST_UNKNOWN;

    if (!strcmp(str, PROD_TEST_SLAVE_STR))
        return PROD_TEST_SLAVE;
    else if (!strcmp(str, PROD_TEST_MASTER_STR))
        return PROD_TEST_MASTER;
    else if (!strcmp(str, PROD_TEST_STATS_STR))
        return PROD_TEST_STATS;
    else if (!strcmp(str, PROD_TEST_TX_BURST_STR))
        return PROD_TEST_TX_BURST;
    else if (!strcmp(str, PROD_TEST_TX_CONTINUOUS_STR))
        return PROD_TEST_TX_CONTINUOUS;
    else if (!strcmp(str, PROD_TEST_RX_BURST_STR))
        return PROD_TEST_RX_BURST;
    else if (!strcmp(str, PROD_TEST_RX_BURST_CONFIG_STR))
        return PROD_TEST_RX_BURST_CONFIG;
    else
        return PROD_TEST_UNKNOWN;
}

static QcaSubOpcodeT Map2DebugCmd(char *str)
{
    if (!str)
        return UNKNOWN_DEBUG_CMD;

    if (!strcmp(str, DEBUG_READ_RSSI_STR))
        return READ_RSSI_CMD;
    else
        return UNKNOWN_DEBUG_CMD;
}

static uint32_t MapProdTestCmd2EventBit(QcaSubOpcodeT test_cmd)
{
    switch (test_cmd)
    {
        case PROD_TEST_SLAVE:
            return EVENT_BIT_PROD_TEST_SLAVE;

        case PROD_TEST_MASTER:
            return EVENT_BIT_PROD_TEST_MASTER;

        case PROD_TEST_STATS:
            return EVENT_BIT_PROD_TEST_STATS;

        case PROD_TEST_TX_BURST:
            return EVENT_BIT_PROD_TEST_TX_BURST;

        case PROD_TEST_TX_CONTINUOUS:
            return EVENT_BIT_PROD_TEST_TX_CONTINUOUS;

        case PROD_TEST_RX_BURST:
            return EVENT_BIT_PROD_TEST_RX_BURST;

        case PROD_TEST_RX_BURST_CONFIG:
            return EVENT_BIT_PROD_TEST_RX_BURST_CONFIG;

        default:
            return EVENT_BIT_UNKNOWN;
    }
}

static uint32_t MapDebugCmd2EventBit(QcaSubOpcodeT debug_cmd)
{
    switch (debug_cmd)
    {
        case READ_RSSI_CMD:
            return EVENT_BIT_READ_RSSI;

        default:
            return EVENT_BIT_UNKNOWN;
    }
}

static void DumpTestParameter(QcaTestParameter *test_para)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> +++ "), __FUNCTION__));

    if (!test_para)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" serial_port:     '%s' "),  test_para->serial_port));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" baudrate:        %d"),     test_para->baudrate));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" flow_control:    %x"),     test_para->flow_control));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" config_path:     '%s' "),  test_para->config_path));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" test_cmd:        %02x"),   test_para->test_cmd));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" debug_cmd:       %02x"),   test_para->debug_cmd));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" nvm_tag:         %02x"),   test_para->nvm_tag));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" enable_dut:      %x"),     test_para->enable_dut));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" hci_cmd:         '%s' "),  test_para->hci_cmd ? test_para->hci_cmd : "n/a"));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" hci_cmd_file:    '%s' "),  test_para->hci_cmd_file ? test_para->hci_cmd_file : "n/a"));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" delay:            %d"),    test_para->delay));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> --- "), __FUNCTION__));
}

static void DumpProdTestCommonCmd(PROD_TEST_COMMON_CMD_T *cmd)
{
    uint8_t *hop_channels;
    uint8_t *target_addr;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!cmd)
        return;

    hop_channels = cmd->hop_channels;
    target_addr  = cmd->target_addr;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" sub_opcode:              %02X"), cmd->sub_opcode));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" hop_channels:            %02X-%02X-%02X-%02X-%02X"),
                   hop_channels[0], hop_channels[1], hop_channels[2], hop_channels[3], hop_channels[4]));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" tx_pattern_payload:      %02X"), cmd->tx_pattern_payload));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" packet_type:             %02X"), cmd->packet_type));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" data_whitening:          %02X"), cmd->data_whitening));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" tx_output_power:         %02X"), cmd->tx_output_power));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" receiver_gain:           %02X"), cmd->receiver_gain));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" target_addr:             %02X-%02X-%02X-%02X-%02X-%02X"),
                   target_addr[0], target_addr[1], target_addr[2], target_addr[3], target_addr[4], target_addr[5]));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" hopping_func:            %02X"), cmd->hopping_func));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" payload_length:          %04X"), cmd->payload_length));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" logical_transport_addr:  %02X"), cmd->logical_transport_addr));
}

static void DumpProdTestTxContinousCmd(PROD_TEST_TX_CONTINUOUS_CMD_T *cmd)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!cmd)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" sub_opcode:              %02X"), cmd->sub_opcode));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" channel_number:          %02X"), cmd->channel_number));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" tx_output_power:         %02X"), cmd->tx_output_power));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" tx_type:                 %02X"), cmd->tx_type));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" packet_length:           %02X"), cmd->packet_length));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" bit_pattern:             %08X"), cmd->bit_pattern));
}

static void DumpProdTestRxBurstConfigCmd(PROD_TEST_RX_BURST_CONFIG_CMD_T *cmd)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!cmd)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" sub_opcode:              %02X"), cmd->sub_opcode));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" number_of_packets:       %04X"), cmd->number_of_packets));
}

static void DumpReadRssiCmd(READ_RSSI_CMD_T *cmd)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!cmd)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" sub_opcode:              %02X"), cmd->sub_opcode));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" channel_offset:          %02X"), cmd->channel_offset));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT(" number_of_samples:       %02X"), cmd->number_of_samples));
}

static void OutputStatistics(PROD_TEST_STATS_EVT_T *stats)
{
    CHANNEL_STATISTICS_T *channel_stats;
    uint32_t index;

    if (!stats)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> +++"), __FUNCTION__));

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("op_code: %02x, received_packets: %04x (%d)"),
                   stats->op_code, stats->received_packets, stats->received_packets));

    for (index = 0; index < HOP_CHANNELS_COUNT; index++)
    {
        channel_stats = &stats->channel_stats[index];

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("channel#%d"), index + 1));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  packets_received:          %08x (%d)"),
                       channel_stats->packets_received, channel_stats->packets_received));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  packets_access_error_code: %08x (%d)"),
                       channel_stats->packets_access_error_code, channel_stats->packets_access_error_code));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  hec_errors:                %08x (%d)"),
                       channel_stats->hec_errors, channel_stats->hec_errors));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  crc_errors:                %08x (%d)"),
                       channel_stats->crc_errors, channel_stats->crc_errors));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  total_packet_bit_errors:   %08x (%d)"),
                       channel_stats->total_packet_bit_errors, channel_stats->total_packet_bit_errors));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  first_half_bit_errors:     %08x (%d)"),
                       channel_stats->first_half_bit_errors, channel_stats->first_half_bit_errors));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  last_half_bit_errors:      %08x (%d)"),
                       channel_stats->last_half_bit_errors, channel_stats->last_half_bit_errors));

        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  rssi_reading:              %08x (%d)"),
                       channel_stats->rssi_reading, channel_stats->rssi_reading));
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> ---"), __FUNCTION__));
}

static void OutputProductTestResult(QcaTestInstance *inst, QcaSubOpcodeT test_cmd)
{
    switch (test_cmd)
    {
        case PROD_TEST_SLAVE:
        {
            OUTPUT_HCI_STATUS(inst->slave_cmd_status, "'PROD_TEST_SLAVE'");
            break;
        }

        case PROD_TEST_MASTER:
        {
            OUTPUT_HCI_STATUS(inst->master_cmd_status, "'PROD_TEST_MASTER'");
            break;
        }

        case PROD_TEST_STATS:
        {
            OUTPUT_HCI_STATUS(inst->stats_cmd_status, "'PROD_TEST_STATS'");

            if (IS_HCI_SUCCESS(inst->stats_cmd_status))
            {
                OutputStatistics(&inst->stats_cmd_result);
            }
            break;
        }

        case PROD_TEST_TX_BURST:
        {
            OUTPUT_HCI_STATUS(inst->tx_burst_cmd_status, "'PROD_TEST_TX_BURST'");
            break;
        }

        case PROD_TEST_TX_CONTINUOUS:
        {
            OUTPUT_HCI_STATUS(inst->tx_continous_cmd_status, "'PROD_TEST_TX_CONTINUOUS'");
            break;
        }

        case PROD_TEST_RX_BURST:
        {
            OUTPUT_HCI_STATUS(inst->rx_burst_cmd_status, "'PROD_TEST_RX_BURST'");
            break;
        }

        case PROD_TEST_RX_BURST_CONFIG:
        {
            /* TODO */
            break;
        }

        default:
        {
            break;
        }
    }
}

static void OutputDebugResult(QcaTestInstance *inst, QcaSubOpcodeT debug_cmd)
{
    switch (debug_cmd)
    {
        case READ_RSSI_CMD:
        {
            HCI_EV_READ_RSSI_T *read_rssi_result = &inst->read_rssi_result;
            HCI_EV_COMMAND_COMPLETE_T *command_complete = &read_rssi_result->command_complete;

            if (IS_HCI_SUCCESS(command_complete->status))
            {
                IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Read RSSI succeed, rssi: 0x%02x (%d)"),
                               read_rssi_result->rssi, read_rssi_result->rssi));
            }
            else
            {
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Read RSSI fail, status: 0x%02x"),
                               __FUNCTION__, command_complete->status));
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

static void OutputNvmResponse(READ_NVM_EVT_T *response)
{
    if (!response)
        return;

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Output NVM response")));

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  op_code:       %02x"), response->op_code));
    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  sub_opcode:    %02x"), response->sub_opcode));
    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  nvm_tag:       %02x"), response->nvm_tag));
    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("  length:        %02x"), response->length));

    IFLOG(DumpBuff(DEBUG_BT_MESSAGE, response->data, response->length));
}

static bool InitTestParameter(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    char *parameter, *value;
    char *port = DEFAULT_UART_PORT;
    uint32_t baudrate = DEFAULT_UART_BAUDRATE;
    bool flow_control = true;
    char *config_path = DEFAULT_BT_TEST_CONFIG_PATH;
    QcaSubOpcodeT test_cmd = PROD_TEST_UNKNOWN;
    QcaSubOpcodeT debug_cmd = UNKNOWN_DEBUG_CMD;
    uint8_t nvm_tag = UNKNOWN_NVM_TAG;
    bool enable_dut = false;
    char *hci_cmd = NULL;
    char *hci_cmd_file = NULL;
    uint32_t delay = DEFAULT_BT_TEST_DELAY;
    uint32_t tmp_val = 0;

    memset(test_para, 0, sizeof(QcaTestParameter));

    if (ConnxArgSearch(NULL, "--bt-port", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        port = value;
    }

    if (ConnxArgSearch(NULL, "--bt-baud", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%d", &tmp_val) != 1))
        {
            return false;
        }

        baudrate = tmp_val;
    }

    if (ConnxArgSearch(NULL, "--flow-control", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%x", &tmp_val) != 1))
        {
            return false;
        }

        flow_control = tmp_val ? true : false;
    }

    if (ConnxArgSearch(NULL, "--config-path", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        config_path = value;
    }

    if (ConnxArgSearch(NULL, "--test", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        test_cmd = Map2ProdTestCmd(value);
    }

    if (ConnxArgSearch(NULL, "--debug", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        debug_cmd = Map2DebugCmd(value);
    }

    if (ConnxArgSearch(NULL, "--read-nvm", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%d", &tmp_val) != 1))
        {
            return false;
        }

        nvm_tag = (uint8_t)(tmp_val & 0xFF);
    }

    if (ConnxArgSearch(NULL, "--enable", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        if (!strcmp(value, "dut"))
        {
            enable_dut = true;
        }
    }

    if (ConnxArgSearch(NULL, "--hci-cmd", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        hci_cmd = value;
    }

    if (ConnxArgSearch(NULL, "--hci-cmd-file", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        hci_cmd_file = value;
    }

    if (ConnxArgSearch(NULL, "--delay", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%d", &tmp_val) != 1))
        {
            return false;
        }

        delay = tmp_val;
    }

    test_para->size         = sizeof(QcaTestParameter);
    test_para->serial_port  = port;
    test_para->baudrate     = baudrate;
    test_para->flow_control = flow_control;
    test_para->config_path  = config_path;
    test_para->test_cmd     = test_cmd;
    test_para->debug_cmd    = debug_cmd;
    test_para->nvm_tag      = nvm_tag;
    test_para->enable_dut   = enable_dut;
    test_para->hci_cmd      = hci_cmd;
    test_para->hci_cmd_file = hci_cmd_file;
    test_para->delay        = delay;

    return true;
}

/* ------------------------------------------------------------------------------- */
/* Parse bt_test configuration file                                                */
/* ------------------------------------------------------------------------------- */

static bool ParseProdTestCommonCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_COMMON_CMD_T *cmd)
{
    uint8_t key[256];
    uint32_t hop_channels[HOP_CHANNELS_COUNT];
    uint32_t tx_pattern_payload;
    uint32_t packet_type;
    uint32_t data_whitening;
    uint32_t tx_output_power;
    uint32_t receiver_gain;
    uint32_t target_addr[BT_ADDR_LENGTH];
    uint32_t hopping_func;
    uint32_t payload_length;
    uint32_t logical_transport_addr;
    uint32_t index;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

    if (!line || !valid_cmd || !cmd)
        return false;

    *valid_cmd = false;

    if (sscanf(line, PROD_TEST_COMMON_CMD_FORMAT, key,
        &hop_channels[0], &hop_channels[1], &hop_channels[2], &hop_channels[3], &hop_channels[4],
        &tx_pattern_payload, &packet_type, &data_whitening, &tx_output_power, &receiver_gain,
        &target_addr[0], &target_addr[1], &target_addr[2], &target_addr[3], &target_addr[4], &target_addr[5],
        &hopping_func, &payload_length, &logical_transport_addr) == PROD_TEST_COMMON_CMD_FIELD_COUNT)
    {
        cmd->sub_opcode = sub_opcode;

        for (index = 0; index < HOP_CHANNELS_COUNT; index++)
            cmd->hop_channels[index] = (uint8_t)(hop_channels[index] & 0xFF);

        cmd->tx_pattern_payload     = (uint8_t)(tx_pattern_payload & 0xFF);
        cmd->packet_type            = (uint8_t)(packet_type & 0xFF);
        cmd->data_whitening         = (uint8_t)(data_whitening & 0xFF);
        cmd->tx_output_power        = (uint8_t)(tx_output_power & 0xFF);
        cmd->receiver_gain          = (uint8_t)(receiver_gain & 0xFF);

        for (index = 0; index < BT_ADDR_LENGTH; index++)
            cmd->target_addr[index] = (uint8_t)(target_addr[index] & 0xFF);

        cmd->hopping_func           = (uint8_t)(hopping_func & 0xFF);
        cmd->payload_length         = (uint16_t)(payload_length & 0xFFFF);
        cmd->logical_transport_addr = (uint8_t)(logical_transport_addr & 0xFF);

        *valid_cmd = true;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Succeed to parse, sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

        IFLOG(DumpProdTestCommonCmd(cmd));

        return true;
    }

    IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to parse line '%s' "), __FUNCTION__, line));

    return false;
}

static bool ParseProdTestTxContinousCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_TX_CONTINUOUS_CMD_T *cmd)
{
    uint8_t key[256];
    uint32_t channel_number;
    uint32_t tx_output_power;
    uint32_t tx_type;
    uint32_t packet_length;
    uint32_t bit_pattern;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

    if (!line || !valid_cmd || !cmd)
        return false;

    *valid_cmd = false;

    if (sscanf(line, PROD_TEST_TX_CONTINUOUS_CMD_FORMAT, key,
        &channel_number, &tx_output_power, &tx_type, &packet_length, &bit_pattern) == 
        PROD_TEST_TX_CONTINUOUS_CMD_FIELD_COUNT)
    {
        cmd->sub_opcode      = sub_opcode;

        cmd->channel_number  = (uint8_t)(channel_number & 0xFF);
        cmd->tx_output_power = (uint8_t)(tx_output_power & 0xFF);
        cmd->tx_type         = (uint8_t)(tx_type & 0xFF);
        cmd->packet_length   = (uint8_t)(packet_length & 0xFF);
        cmd->bit_pattern     = bit_pattern;

        *valid_cmd = true;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Succeed to parse, sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

        IFLOG(DumpProdTestTxContinousCmd(cmd));

        return true;
    }

    IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to parse line '%s' "), __FUNCTION__, line));

    return false;
}

static bool ParseProdTestRxBurstConfigCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, PROD_TEST_RX_BURST_CONFIG_CMD_T *cmd)
{
    uint8_t key[256];
    uint32_t number_of_packets;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

    if (!line || !valid_cmd || !cmd)
        return false;

    *valid_cmd = false;

    if (sscanf(line, PROD_TEST_RX_BURST_CONFIG_CMD_FORMAT, key,
        &number_of_packets) == PROD_TEST_RX_BURST_CONFIG_CMD_FIELD_COUNT)
    {
        cmd->sub_opcode        = sub_opcode;
        cmd->number_of_packets = (uint16_t)(number_of_packets & 0xFFFF);

        *valid_cmd = true;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Succeed to parse, sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

        IFLOG(DumpProdTestRxBurstConfigCmd(cmd));

        return true;
    }

    IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to parse line '%s' "), __FUNCTION__, line));

    return false;
}

static bool ParseReadRssiCmd(char *line, QcaSubOpcodeT sub_opcode, bool *valid_cmd, READ_RSSI_CMD_T *cmd)
{
    uint8_t key[256];
    uint32_t channel_offset;
    uint32_t number_of_samples;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

    if (!line || !valid_cmd || !cmd)
        return false;

    *valid_cmd = false;

    if (sscanf(line, READ_RSSI_CMD_FORMAT, key,
        &channel_offset, &number_of_samples) == READ_RSSI_CMD_FIELD_COUNT)
    {
        cmd->sub_opcode        = sub_opcode;
        cmd->channel_offset    = (uint8_t)(channel_offset & 0xFF);
        cmd->number_of_samples = (uint8_t)(number_of_samples & 0xFF);

        *valid_cmd = true;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Succeed to parse, sub_opcode: 0x%02x"), __FUNCTION__, sub_opcode));

        IFLOG(DumpReadRssiCmd(cmd));

        return true;
    }

    IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to parse line '%s' "), __FUNCTION__, line));

    return false;
}

static bool ParseConfigFile(char *config_file, QcaTestConfiguration *test_config)
{
    FILE *fp = NULL;
    uint8_t line[512];
    uint8_t key[256];

    BT_TEST_FUNC_ENTER();

    if (!config_file || !test_config)
        return false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> config_file: '%s'"), __FUNCTION__, config_file));

    fp = fopen(config_file, "r");

    if (!fp)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Open config_file: '%s' fail"), __FUNCTION__, config_file));
        return false;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, PROD_TEST_CMD_FORMAT, key) == PROD_TEST_CMD_FIELD_COUNT)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> key: '%s' "), __FUNCTION__, key));

            if (!strcmp(key, "PROD_TEST_SLAVE"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_SLAVE' "), __FUNCTION__));
                ParseProdTestCommonCmd(line, PROD_TEST_SLAVE, &test_config->valid_slave_cmd, &test_config->slave_cmd);
            }
            else if (!strcmp(key, "PROD_TEST_MASTER"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_MASTER' "), __FUNCTION__));
                ParseProdTestCommonCmd(line, PROD_TEST_MASTER, &test_config->valid_master_cmd, &test_config->master_cmd);
            }
            else if (!strcmp(key, "PROD_TEST_TX_BURST"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_TX_BURST' "), __FUNCTION__));
                ParseProdTestCommonCmd(line, PROD_TEST_TX_BURST, &test_config->valid_tx_burst_cmd, &test_config->tx_burst_cmd);
            }
            else if (!strcmp(key, "PROD_TEST_TX_CONTINUOUS"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_TX_CONTINUOUS' "), __FUNCTION__));
                ParseProdTestTxContinousCmd(line, PROD_TEST_TX_CONTINUOUS, &test_config->valid_tx_continous_cmd, &test_config->tx_continous_cmd);
            }
            else if (!strcmp(key, "PROD_TEST_RX_BURST"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_RX_BURST' "), __FUNCTION__));
                ParseProdTestCommonCmd(line, PROD_TEST_RX_BURST, &test_config->valid_rx_burst_cmd, &test_config->rx_burst_cmd);
            }
            else if (!strcmp(key, "PROD_TEST_RX_BURST_CONFIG"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_RX_BURST_CONFIG' "), __FUNCTION__));
                ParseProdTestRxBurstConfigCmd(line, PROD_TEST_RX_BURST_CONFIG, &test_config->valid_rx_burst_config_cmd, &test_config->rx_burst_config_cmd);
            }
            else if (!strcmp(key, "READ_RSSI"))
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'READ_RSSI' "), __FUNCTION__));
                ParseReadRssiCmd(line, READ_RSSI_CMD, &test_config->valid_read_rssi_cmd, &test_config->read_rssi_cmd);
            }
            else
            {
                /* Unknown product/debug command. */
                continue;
            }
        }
    }

    fclose(fp);

    BT_TEST_FUNC_LEAVE();

    return true;
}

static bool InitTestConfig(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    QcaTestParameter *test_para = &inst->test_para;
    char *config_file = ConnxCreateFullFileName(test_para->config_path, BT_TEST_CONFIG_FILE);
    bool res = false;

    BT_TEST_FUNC_ENTER();

    res = ParseConfigFile(config_file, test_config);

    free(config_file);

    BT_TEST_FUNC_LEAVE();

    return res;
}

static bool VerifyHciCmd(uint8_t *buf, uint32_t buf_size)
{
    HCI_COMMAND_HEADER_T *hci_cmd_header = (HCI_COMMAND_HEADER_T *) buf;

    if (!buf || (buf_size < HCI_COMMAND_HDR_SIZE))
        return false;

    if ((hci_cmd_header->length + HCI_COMMAND_HDR_SIZE) != buf_size)
        return false;

    return true;
}

static bool ParseHciCmd(char *cmd, uint8_t **buf, uint32_t *buf_size)
{
    char *ptr = cmd;
    size_t length = 0;
    uint8_t hci_cmd[MAX_HCI_COMMAND_SIZE];
    uint32_t actual_count = 0;
    uint32_t tmp_val = 0;
    uint8_t *tmp_buf = NULL;
    bool res = true;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!cmd || !buf || !buf_size)
        return false;

    *buf = NULL;
    *buf_size = 0;

    length = strlen(cmd);

    if (!length)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> empty hci cmd"), __FUNCTION__));
        return false;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> cmd: '%s', length: %d"), __FUNCTION__, cmd, length));

    while (ptr < cmd + length)
    {
        if ((*ptr == ' ') || (*ptr == ',') || (*ptr == '\r') || (*ptr == '\n'))
        {
            ++ptr;
            continue;
        }

        if (sscanf(ptr, "%02x", &tmp_val) == 1)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> actual_count: %d, val: %02x"),
                           __FUNCTION__, actual_count, tmp_val));

            if (actual_count < MAX_HCI_COMMAND_SIZE)
            {
                hci_cmd[actual_count++] = (uint8_t)(tmp_val & 0xFF);

                /* 2 hex, 1 delimiter */
                ptr += 3;
            }
            else
            {
                /* Exceed max HCI command size. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Exceed max hci cmd size: %d "),
                               __FUNCTION__, MAX_HCI_COMMAND_SIZE));
                res = false;
                break;
            }
        }
        else
        {
            /* Invalid HCI command. */
            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid hci cmd: '%s' "), __FUNCTION__, cmd));
            res = false;
            break;
        }
    }

    if (res)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> hci cmd size: %d"), __FUNCTION__, actual_count));

        if (actual_count)
        {
            tmp_buf = (uint8_t *)malloc(actual_count);

            if (!tmp_buf)
                return false;

            memcpy(tmp_buf, hci_cmd, actual_count);

            *buf = tmp_buf;
            *buf_size = actual_count;
        }
    }

    return res;
}

static bool ParseHciCmdFile(char *file_name, ConnxCmnList_t *hci_cmd_list)
{
    FILE *fp = NULL;
    uint8_t line[1024];
    uint8_t *buf = NULL;
    uint32_t buf_size = 0;
    bool res = true;

    if (!file_name || !hci_cmd_list)
        return false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Open file '%s' "), __FUNCTION__, file_name));

    fp = fopen(file_name, "r");

    if (!fp)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Open file_name: '%s' fail"), __FUNCTION__, file_name));
        return false;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /* Comment starts from '#'. */
        if (strchr(line, '#'))
        {
            /* The line is comment. */
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Ignore comment"), __FUNCTION__));
            continue;
        }

        if (ParseHciCmd(line, &buf, &buf_size))
        {
            AddHciCmd(hci_cmd_list, buf, buf_size);
        }
        else
        {
            /* Invalid HCI command. */
            res = false;
            break;
        }
    }

    fclose(fp);

    if (!res)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to parse file"), __FUNCTION__));

        DeinitHciCmdList(hci_cmd_list);

        return false;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Succeed to parse file"), __FUNCTION__));

    return true;
}

static void InitHciCmdList(ConnxCmnList_t *hci_cmd_list)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> list: %p"), __FUNCTION__, hci_cmd_list));

    if (!hci_cmd_list)
        return;

    ConnxCmnListInit(hci_cmd_list, 0, NULL, FreeHciCmd);
}

static void DeinitHciCmdList(ConnxCmnList_t *hci_cmd_list)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> list: %p"), __FUNCTION__, hci_cmd_list));

    if (!hci_cmd_list)
        return;

    ConnxCmnListDeinit(hci_cmd_list);
}

static bool AddHciCmd(ConnxCmnList_t *hci_cmd_list, uint8_t *data, uint32_t size)
{
    SG_BUFFER_T *sg_buf = NULL;
    ConnxCmnListDataElm_t *elem = NULL;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> list: %p, data: %p, size: %d"),
                   __FUNCTION__, hci_cmd_list, data, size));

    if (!data || !size)
        return false;

    sg_buf = (SG_BUFFER_T *) malloc(sizeof(SG_BUFFER_T));

    if (!sg_buf)
        return false;

    sg_buf->data = data;
    sg_buf->size = size;

    elem = (ConnxCmnListDataElm_t *) ConnxCmnListElementAddLast(hci_cmd_list, sizeof(ConnxCmnListDataElm_t));

    /* [QTI] Fix KW issue#265560. */
    if (!elem)
        return false;

    elem->data = sg_buf;

    return true;
}

static void FreeHciCmd(ConnxCmnListElm_t *elem)
{
    ConnxCmnListDataElm_t *element = (ConnxCmnListDataElm_t *) elem;
    SG_BUFFER_T *sg_buf = NULL;

    if (!element)
        return;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> element: %p, data: %p"), __FUNCTION__, element, element->data));

    if (element->data)
    {
        sg_buf = (SG_BUFFER_T *) element->data;

        if (sg_buf)
        {
            free(sg_buf->data);
            sg_buf->data = NULL;
        }

        free(element->data);
        element->data = NULL;
    }
}

/* ------------------------------------------------------------------------------- */

static void StartTest(QcaTestInstance *inst)
{
    BT_TEST_FUNC_ENTER();

    DumpTestParameter(&inst->test_para);

    HandleTestCmd(inst);

    HandleDebugCmd(inst);

    HandleNvmCmd(inst);

    HandleEnableDutCmd(inst);

    HandleHciCmd(inst);

    HandleHciCmdFile(inst);

    BT_TEST_FUNC_LEAVE();
}

static void HandleTestCmd(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;    
    QcaSubOpcodeT test_cmd = test_para->test_cmd;

    if (!VALID_PROD_TEST_CMD(test_cmd))
    {
        /* Invalid product test command. */
        return;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> test_cmd: %02x"), __FUNCTION__, test_cmd));

    if (!InitTestConfig(inst))
    {
        /* Invalid test config. */
        return;
    }

    /* Issue test command. */
    switch (test_cmd)
    {
        case PROD_TEST_SLAVE:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_SLAVE' "), __FUNCTION__));
            QCA_IssueSlaveCmd(inst);
            break;
        }

        case PROD_TEST_MASTER:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_MASTER' "), __FUNCTION__));
            QCA_IssueMasterCmd(inst);
            break;
        }

        case PROD_TEST_STATS:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_STATS' "), __FUNCTION__));
            QCA_IssueStatsCmd(inst);
            break;
        }

        case PROD_TEST_TX_BURST:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_TX_BURST' "), __FUNCTION__));
            QCA_IssueTxBurstCmd(inst);
            break;
        }

        case PROD_TEST_TX_CONTINUOUS:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_TX_CONTINUOUS' "), __FUNCTION__));
            QCA_IssueTxContinousCmd(inst);
            break;
        }

        case PROD_TEST_RX_BURST:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_RX_BURST' "), __FUNCTION__));
            QCA_IssueRxBurstCmd(inst);
            break;
        }

        case PROD_TEST_RX_BURST_CONFIG:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'PROD_TEST_RX_BURST_CONFIG' "), __FUNCTION__));
            QCA_IssueRxBurstConfigCmd(inst);
            break;
        }

        default:
        {
            /* Unknown test command. */
            break;
        }
    }
}

static void HandleDebugCmd(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    QcaSubOpcodeT debug_cmd = test_para->debug_cmd;

    if (!VALID_DEBUG_CMD(debug_cmd))
    {
        /* Invalid debug command. */
        return;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> debug_cmd: %02x"), __FUNCTION__, debug_cmd));

    if (!InitTestConfig(inst))
    {
        /* Invalid test config. */
        return;
    }

    /* Issue debug command. */
    switch (debug_cmd)
    {
        case READ_RSSI_CMD:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue 'Read RSSI' "), __FUNCTION__));
            QCA_IssueReadRssiCmd(inst);
            break;
        }
        
        default:
        {
            /* Unknown debug command. */
            break;
        }
    }
}

static void HandleNvmCmd(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    uint8_t nvm_tag = test_para->nvm_tag;

    if (!VALID_NVM_TAG(nvm_tag))
    {
        /* Invalid nvm tag. */
        return;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> nvm tag: %d (%02x)"), __FUNCTION__, nvm_tag, nvm_tag));

    QCA_IssueReadNvmCmd(inst, nvm_tag);
}

static void HandleEnableDutCmd(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    bool enable_dut = test_para->enable_dut;
    ConnxResult result;

    if (!enable_dut)
    {
        return;
    }

    /* Assume that BT is booted up, in which BT firmware has been downloaded and HCI Reset has been issued. */

    do
    {
        /* Step1. Issue HCI Reset. */
        result = QCA_IssueResetCmd(inst);
        CHK(result, "Issue reset cmd");

        /* Step2. Enable DUT mode. */
        result = QCA_EnableDut(inst);
        CHK(result, "Enable DUT");

        /* Step3. Disable authentication. */
        result = QCA_IssueWriteAuthCmd(inst, false);
        CHK(result, "Disable authentication");

        /* Step4. [Optional] Disable encryption. This command is obsolete. */
        result = QCA_IssueWriteEncModeCmd(inst, false);
        CHK(result, "Disable encryption");

        /* Step5. Page/inquiry scan enable. */
        result = QCA_EnableScan(inst);
        CHK(result, "Enable page/inquiry scan");

        /* Step6. Auto accept all connection requests. */
        result = QCA_EnableAutoConnect(inst);
        CHK(result, "Enable auto-connect");
    }
    while (0);

    if (IS_CONNX_RESULT_SUCCESS(result))
    {
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Enable DUT succeed")));
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("Enable DUT fail")));
    }
}

static void HandleHciCmd(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    char *hci_cmd = test_para->hci_cmd;
    uint8_t *buf = NULL;
    uint32_t buf_size = 0;

    if (!hci_cmd)
    {
        return;
    }

    BT_TEST_FUNC_ENTER();

    if (!ParseHciCmd(hci_cmd, &buf, &buf_size))
    {
        /* Invalid HCI command. */
        return;
    }

    AddHciCmd(&inst->hci_cmd_list, buf, buf_size);

    ProcessHciCmdList(inst);

    BT_TEST_FUNC_LEAVE();
}

static void HandleHciCmdFile(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    char *hci_cmd_file = test_para->hci_cmd_file; 
    ConnxCmnList_t *hci_cmd_list = &inst->hci_cmd_list;
    uint32_t delay = test_para->delay;  /* in second. */

    if (!hci_cmd_file)
    {
        return;
    }

    BT_TEST_FUNC_ENTER();

    if (!ParseHciCmdFile(hci_cmd_file, hci_cmd_list))
    {
        /* Invalid HCI command file. */
        return;
    }

    ProcessHciCmdList(inst);

    /* Add some delay to check whether there is un-parsed HCI event. */
    if (delay)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sleep %d sec"), __FUNCTION__, delay));
        sleep(delay);
    }

    BT_TEST_FUNC_LEAVE();
}

static void ProcessHciCmdList(QcaTestInstance *inst)
{
    ConnxCmnList_t *hci_cmd_list = &inst->hci_cmd_list;
    uint32_t count = ConnxCmnListGetCount(hci_cmd_list);
    ConnxCmnListDataElm_t *element = NULL;
    SG_BUFFER_T *sg_buf = NULL;
    ConnxResult result = 0;
    uint32_t index = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> hci cmd count: %d"), __FUNCTION__, count));

    for (index = 0; index < count; index++)
    {
        element = (ConnxCmnListDataElm_t *) ConnxCmnListGetFromIndex(hci_cmd_list, index);

        if (element)
        {
            sg_buf = (SG_BUFFER_T *) element->data;

            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> hci cmd#%d (data: %p, size: %d)"),
                           __FUNCTION__, index, sg_buf->data, sg_buf->size));

            result = QCA_IssueHciCmd(inst, sg_buf->data, sg_buf->size);

            if (!IS_CONNX_RESULT_SUCCESS(result))
            {
                /* Fail to issue HCI command. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Issue hci cmd#%d (data: %p, size: %d) fail, result: 0x%04x"),
                               __FUNCTION__, index, sg_buf->data, sg_buf->size, result));
                break;
            }
        }
        else
        {
            break;
        }
    }
}

/* ------------------------------------------------------------------------------- */
/* Issue product test command                                                      */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueProductTestCmd(QcaTestInstance *inst, uint8_t length, void *data)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    QcaSubOpcodeT sub_opcode = *((QcaSubOpcodeT *)data);
    uint32_t eventBits = MapProdTestCmd2EventBit(sub_opcode);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Send 'HCI_VS_PROD_TEST_COMMAND', sub_opcode: %02x, length: %02x (%d)"),
                   __FUNCTION__, sub_opcode, length, length));

    if (!H4_SendVendorCommand(HCI_VS_PROD_TEST_COMMAND, length, data))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_VS_PROD_TEST_COMMAND' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OutputProductTestResult(inst, sub_opcode);

    BlockTest(inst, sub_opcode);

    return result;
}

static ConnxResult QCA_IssueProductTestCommonCmd(QcaTestInstance *inst, PROD_TEST_COMMON_CMD_T *cmd)
{
    return QCA_IssueProductTestCmd(inst, sizeof(PROD_TEST_COMMON_CMD_T), cmd);
}

static ConnxResult QCA_IssueSlaveCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_slave_cmd;
    PROD_TEST_COMMON_CMD_T *cmd = &test_config->slave_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid slave cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCommonCmd(inst, cmd);
}

static ConnxResult QCA_IssueMasterCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_master_cmd;
    PROD_TEST_COMMON_CMD_T *cmd = &test_config->master_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid master cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCommonCmd(inst, cmd);
}

static ConnxResult QCA_IssueStatsCmd(QcaTestInstance *inst)
{
    QcaSubOpcodeT sub_opcode = PROD_TEST_STATS;

    return QCA_IssueProductTestCmd(inst, sizeof(QcaSubOpcodeT), &sub_opcode);
}

static ConnxResult QCA_IssueTxBurstCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_tx_burst_cmd;
    PROD_TEST_COMMON_CMD_T *cmd = &test_config->tx_burst_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid tx_burst cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCommonCmd(inst, cmd);
}

static ConnxResult QCA_IssueTxContinousCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_tx_continous_cmd;
    PROD_TEST_TX_CONTINUOUS_CMD_T *cmd = &test_config->tx_continous_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid tx_continous cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCmd(inst, sizeof(PROD_TEST_TX_CONTINUOUS_CMD_T), cmd);
}

static ConnxResult QCA_IssueRxBurstCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_rx_burst_cmd;
    PROD_TEST_COMMON_CMD_T *cmd = &test_config->rx_burst_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid rx_burst cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCommonCmd(inst, cmd);
}

static ConnxResult QCA_IssueRxBurstConfigCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_rx_burst_config_cmd;
    PROD_TEST_RX_BURST_CONFIG_CMD_T *cmd = &test_config->rx_burst_config_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid rx_burst_config cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueProductTestCmd(inst, sizeof(PROD_TEST_RX_BURST_CONFIG_CMD_T), cmd);
}

/* ------------------------------------------------------------------------------- */
/* Issue debug command                                                             */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueDebugCmd(QcaTestInstance *inst, uint8_t length, void *data)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    QcaSubOpcodeT sub_opcode = *((QcaSubOpcodeT *)data);
    uint32_t eventBits = MapDebugCmd2EventBit(sub_opcode);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Send 'HCI_VS_DEBUG_OPCODE', sub_opcode: %02x, length: %02x (%d)"),
                   __FUNCTION__, sub_opcode, length, length));

    if (!H4_SendVendorCommand(HCI_VS_DEBUG_OPCODE, length, data))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_VS_DEBUG_OPCODE' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OutputDebugResult(inst, sub_opcode);

    return result;
}

static ConnxResult QCA_IssueReadRssiCmd(QcaTestInstance *inst)
{
    QcaTestConfiguration *test_config = &inst->test_config;
    bool valid_cmd = test_config->valid_read_rssi_cmd;
    READ_RSSI_CMD_T *cmd = &test_config->read_rssi_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!valid_cmd)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid read_rssi cmd"), __FUNCTION__));
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    return QCA_IssueDebugCmd(inst, sizeof(READ_RSSI_CMD_T), cmd);
}

/* ------------------------------------------------------------------------------- */
/* Issue NVM command                                                               */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueReadNvmCmd(QcaTestInstance *inst, uint8_t nvm_tag)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_READ_NVM;
    READ_NVM_CMD_T read_nvm_cmd;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    read_nvm_cmd.sub_opcode = NVM_CMD_READ;
    read_nvm_cmd.nvm_tag    = nvm_tag;

    if (!H4_SendVendorCommand(HCI_VS_NVM_COMMAND, sizeof(read_nvm_cmd), &read_nvm_cmd))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_VS_NVM_COMMAND' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OutputNvmResponse(&inst->nvm_response);

    return result;
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI EnableDUT command                                                     */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueEnableDutCmd(QcaTestInstance *inst)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_ENABLE_DUT;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendCommand(HCI_ENABLE_DUT_MODE, 0, NULL))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_ENABLE_DUT_MODE' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->enable_dut_status, "'HCI_ENABLE_DUT_MODE'");

    return result;
}

static ConnxResult QCA_EnableDut(QcaTestInstance *inst)
{
    return QCA_IssueEnableDutCmd(inst);
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI WriteScanEnable command                                               */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueWriteScanEnableCmd(QcaTestInstance *inst, uint8_t scan_enable)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_WRITE_SCAN_ENABLE;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendCommand(HCI_WRITE_SCAN_ENABLE, 1, &scan_enable))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_WRITE_SCAN_ENABLE' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->write_scan_enable_status, "'HCI_WRITE_SCAN_ENABLE'");

    return result;
}

static ConnxResult QCA_EnableScan(QcaTestInstance *inst)
{
    uint8_t scan_enable = 0x03;     /* Enable both inquiry scan and page scan. */

    return QCA_IssueWriteScanEnableCmd(inst, scan_enable);
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI SetEventFilter command                                                */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueSetEventFilterCmd(QcaTestInstance *inst, uint8_t cmd_length, uint8_t *cmd)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_SET_EVENT_FILTER;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendCommand(HCI_SET_EVENT_FILTER, cmd_length, cmd))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_SET_EVENT_FILTER' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->set_event_filter_status, "'HCI_SET_EVENT_FILTER'");

    return result;
}

static ConnxResult QCA_EnableAutoConnect(QcaTestInstance *inst)
{
    /* Filter Type: Connection Setup (0x02) */
    /* Connection_Setup_Filter_Condition_Type: Allow Connections from all devices (0x00) */
    /* Auto Accept Flag: Do Auto accept the connection (0x02) */
    uint8_t enable_auto_connect_cmd[] = { 0x02, 0x00, 0x02 };

    return QCA_IssueSetEventFilterCmd(inst, sizeof(enable_auto_connect_cmd), enable_auto_connect_cmd);
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI WriteAuthentication command                                                */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueWriteAuthCmd(QcaTestInstance *inst, bool enable)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_WRITE_AUTH_ENABLE;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendCommand(HCI_WRITE_AUTH_ENABLE, 1, &enable))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_WRITE_AUTH_ENABLE' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->write_auth_status, "'HCI_WRITE_AUTH_ENABLE'");

    return result;
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI WriteEncryptionMode command                                           */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueWriteEncModeCmd(QcaTestInstance *inst, bool enable)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_WRITE_ENC_MODE;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendCommand(HCI_WRITE_ENC_MODE, 1, &enable))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_WRITE_ENC_MODE' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->set_event_filter_status, "'HCI_WRITE_ENC_MODE'");

    return result;
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI Reset command                                                         */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueResetCmd(QcaTestInstance *inst)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_RESET;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendResetCommand())
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send 'HCI_RESET' "), __FUNCTION__));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    OUTPUT_HCI_STATUS(inst->reset_status, "'HCI_RESET'");

    return result;
}

/* ------------------------------------------------------------------------------- */
/* Issue HCI raw command                                                           */
/* ------------------------------------------------------------------------------- */

static ConnxResult QCA_IssueHciCmd(QcaTestInstance *inst, uint8_t *hci_cmd_data, uint32_t hci_cmd_size)
{
    ConnxResult result = 0;
    uint16_t timeout = WAIT_TIMEOUT_IN_MS;
    uint32_t eventBits = EVENT_BIT_ALL;
    HCI_COMMAND_HEADER_T header;
    HCI_COMMAND_HEADER_T *hci_cmd_header = &header;
    uint8_t *ptr = hci_cmd_data;
    uint8_t *hci_cmd_payload = hci_cmd_data + HCI_COMMAND_HDR_SIZE;

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Send HCI command")));

    if (!VerifyHciCmd(hci_cmd_data, hci_cmd_size))
        return CONNX_RESULT_INVALID_PARAMETER;

    IFLOG(DumpBuff(DEBUG_BT_MESSAGE, hci_cmd_data, hci_cmd_size));

    CONNX_CONVERT_16_FROM_XAP(hci_cmd_header->op_code, ptr);
    CONNX_CONVERT_8_FROM_XAP(hci_cmd_header->length, ptr);

    inst->issue_hci_cmd = true;

    if (!H4_SendCommand(hci_cmd_header->op_code, hci_cmd_header->length, hci_cmd_payload))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send HCI command, op_code: 0x%04x "),
                       __FUNCTION__, hci_cmd_header->op_code));
        return result;
    }

    result = WaitTestEvent(inst, timeout, eventBits);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> WaitTestEvent, result: 0x%x"), __FUNCTION__, result));

    if (IS_HCI_SUCCESS(inst->generic_hci_cmd_status))
    {
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("HCI_COMMAND (op_code: 0x%04x) succeed"), hci_cmd_header->op_code));
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("HCI_COMMAND (op_code: 0x%04x) fail, status: 0x%02x"),
                       hci_cmd_header->op_code, inst->generic_hci_cmd_status));
    }

    return result;
}

/* ------------------------------------------------------------------------------- */

static void StoreEventCode(QcaTestInstance *inst, hci_event_code_t event_code)
{
    inst->event_code = event_code;
}

static bool ParseCommandStatusEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data)
{
    HCI_EV_COMMAND_STATUS_T *hci_event = &inst->hci_ev_command_status;
    QcaTestParameter *test_para = &inst->test_para;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    hci_event->event_code = HCI_EV_COMMAND_STATUS;
    hci_event->length     = length;

    CONNX_CONVERT_8_FROM_XAP(hci_event->status, data);
    CONNX_CONVERT_8_FROM_XAP(hci_event->num_hci_command_pkts, data);
    CONNX_CONVERT_16_FROM_XAP(hci_event->op_code, data);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> status: 0x%02x, num_hci_command_pkts: %d, op_code: 0x%04x"),
                   __FUNCTION__, hci_event->status, hci_event->num_hci_command_pkts, hci_event->op_code));

    if (hci_event->op_code == HCI_VENDOR_CMD(HCI_VS_PROD_TEST_COMMAND))
    {
        QcaSubOpcodeT test_cmd = test_para->test_cmd;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_VS_PROD_TEST_COMMAND', test_cmd: %02x"), __FUNCTION__, test_cmd));

        switch (test_cmd)
        {
            case PROD_TEST_SLAVE:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_SLAVE' "), __FUNCTION__));
                inst->slave_cmd_status = hci_event->status;
                res = true;
                break;
            }

            case PROD_TEST_MASTER:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_MASTER' "), __FUNCTION__));
                inst->master_cmd_status = hci_event->status;
                res = true;
                break;
            }

            case PROD_TEST_STATS:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_STATS' "), __FUNCTION__));
                inst->stats_cmd_status = hci_event->status;
                res = true;
                break;
            }

            case PROD_TEST_TX_BURST:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_TX_BURST' "), __FUNCTION__));
                inst->tx_burst_cmd_status = hci_event->status;
                res = true;
                break;
            }

            case PROD_TEST_RX_BURST:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_RX_BURST' "), __FUNCTION__));
                inst->rx_burst_cmd_status = hci_event->status;
                res = true;
                break;
            }

            default:
            {
                /* NOT handle. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown test_cmd: %02x"), __FUNCTION__, test_cmd));
                break;
            }
        }
    }

    return res;
}

static bool ParseVendorEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data)
{
    uint8_t op_code;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> length: 0x%02x"), __FUNCTION__, length));

    if (!data || !length)
        return false;

    op_code = *((uint8_t *)data);

    if (op_code == ((uint8_t) HCI_VS_PROD_TEST_COMMAND))
    {
        PROD_TEST_STATS_EVT_T *stats = &inst->stats_cmd_result;
        uint32_t index = 0;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_VS_PROD_TEST_COMMAND'"), __FUNCTION__));

        CONNX_CONVERT_8_FROM_XAP(stats->op_code, data);
        CONNX_CONVERT_16_FROM_XAP(stats->received_packets, data);

        for (index = 0; index < HOP_CHANNELS_COUNT; index++)
        {
            CHANNEL_STATISTICS_T *channel_stats = &stats->channel_stats[index];

            CONNX_CONVERT_32_FROM_XAP(channel_stats->packets_received, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->packets_access_error_code, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->hec_errors, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->crc_errors, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->total_packet_bit_errors, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->first_half_bit_errors, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->last_half_bit_errors, data);
            CONNX_CONVERT_32_FROM_XAP(channel_stats->rssi_reading, data);
        }

        res = true;
    }
    else if (op_code == ((uint8_t) HCI_VS_NVM_COMMAND))
    {
        READ_NVM_EVT_T *nvm_response = &inst->nvm_response;

        CONNX_CONVERT_8_FROM_XAP(nvm_response->op_code, data);
        CONNX_CONVERT_8_FROM_XAP(nvm_response->sub_opcode, data);
        CONNX_CONVERT_8_FROM_XAP(nvm_response->nvm_tag, data);
        CONNX_CONVERT_8_FROM_XAP(nvm_response->length, data);

        memcpy(nvm_response->data, data, CONNX_MIN(nvm_response->length, sizeof(nvm_response->data)));

        res = true;
    }

    return res;
}

static bool ParseCommandCompleteEvent(QcaTestInstance *inst, uint8_t length, uint8_t *data)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_event = &inst->hci_ev_command_complete;
    QcaTestParameter *test_para = &inst->test_para;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    hci_event->event_code = HCI_EV_COMMAND_COMPLETE;
    hci_event->length     = length;

    CONNX_CONVERT_8_FROM_XAP(hci_event->num_hci_command_pkts, data);
    CONNX_CONVERT_16_FROM_XAP(hci_event->op_code, data);
    CONNX_CONVERT_8_FROM_XAP(hci_event->status, data);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> num_hci_command_pkts: %d, op_code: 0x%04x, status: 0x%02x"),
                   __FUNCTION__, hci_event->num_hci_command_pkts, hci_event->op_code, hci_event->status));

    if (inst->issue_hci_cmd)
    {
        inst->generic_hci_cmd_status = hci_event->status;
        return true;
    }

    if (hci_event->op_code == HCI_VENDOR_CMD(HCI_VS_PROD_TEST_COMMAND))
    {
        QcaSubOpcodeT test_cmd = test_para->test_cmd;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_VS_PROD_TEST_COMMAND', test_cmd: %02x"), __FUNCTION__, test_cmd));

        switch (test_cmd)
        {
            case PROD_TEST_TX_CONTINUOUS:
            {
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'PROD_TEST_TX_CONTINUOUS' "), __FUNCTION__));
                inst->tx_continous_cmd_status = hci_event->status;
                res = true;
                break;
            }

            default:
            {
                /* Unknown test command. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown test command: %02x"), __FUNCTION__, test_cmd));
                break;
            }
        }
    }
    else if (hci_event->op_code == HCI_VENDOR_CMD(HCI_VS_DEBUG_OPCODE))
    {
        QcaSubOpcodeT debug_cmd = test_para->debug_cmd;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_VS_PROD_TEST_COMMAND', debug_cmd: %02x"), __FUNCTION__, debug_cmd));

        switch (debug_cmd)
        {
            case READ_RSSI_CMD:
            {
                HCI_EV_READ_RSSI_T *read_rssi_result = &inst->read_rssi_result;

                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse 'READ_RSSI_CMD' "), __FUNCTION__));

                memcpy(&read_rssi_result->command_complete, hci_event, sizeof(HCI_EV_COMMAND_COMPLETE_T));

                CONNX_CONVERT_8_FROM_XAP(read_rssi_result->sub_opcode, data);

                CONNX_CONVERT_8_FROM_XAP(read_rssi_result->rssi, data);

                res = true;

                break;
            }
            default:
            {
                /* Unknown debug command. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown debug command: %02x"), __FUNCTION__, debug_cmd));
                break;
            }
        }
    }
    else if (hci_event->op_code == HCI_NOP)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_NOP'"), __FUNCTION__));
        res = true;
    }
    else if (hci_event->op_code == HCI_RESET)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_RESET'"), __FUNCTION__));
        inst->reset_status = hci_event->status;
        res = true;
    }
    else if (hci_event->op_code == HCI_ENABLE_DUT_MODE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_ENABLE_DUT_MODE'"), __FUNCTION__));
        inst->enable_dut_status = hci_event->status;
        res = true;
    }
    else if (hci_event->op_code == HCI_WRITE_AUTH_ENABLE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_WRITE_AUTH_ENABLE'"), __FUNCTION__));
        inst->write_auth_status = hci_event->status;
        res = true;
    }
    else if (hci_event->op_code == HCI_WRITE_ENC_MODE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_WRITE_ENC_MODE'"), __FUNCTION__));
        inst->write_enc_mode_status = hci_event->status;
        res = true;
    }
    else if (hci_event->op_code == HCI_WRITE_SCAN_ENABLE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_WRITE_SCAN_ENABLE'"), __FUNCTION__));
        inst->write_scan_enable_status = hci_event->status;
        res = true;        
    }
    else if (hci_event->op_code == HCI_SET_EVENT_FILTER)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> 'HCI_SET_EVENT_FILTER'"), __FUNCTION__));
        inst->set_event_filter_status = hci_event->status;
        res = true;
    }

    return res;
}

static bool ParseTestEvent(QcaTestInstance *inst, HCI_EV_GENERIC_T *hci_event)
{
    hci_event_code_t ev;
    uint8_t len;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!hci_event)
        return false;

    ev  = hci_event->event_code;
    len = hci_event->length;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> event_code: 0x%02x, length: 0x%02x"), __FUNCTION__, ev, len));

    StoreEventCode(inst, ev);

    if (ev == HCI_EV_COMMAND_STATUS)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse HCI command status event"), __FUNCTION__));

        /* Parse HCI command status event. */
        res = ParseCommandStatusEvent(inst, len, hci_event->data);
    }
    else if (ev == HCI_EV_VENDOR_SPECIFIC)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse HCI vendor event"), __FUNCTION__));

        /* Parse HCI vendor event and store internally. */
        res = ParseVendorEvent(inst, len, hci_event->data);
    }
    else if (ev == HCI_EV_COMMAND_COMPLETE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse HCI command complete event"), __FUNCTION__));

        /* Parse HCI event of command complete and store internally. */
        res = ParseCommandCompleteEvent(inst, len, hci_event->data);
    }
    else
    {
        /* Un-parsed HCI event. */
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> Un-parsed event_code: 0x%02x"), __FUNCTION__, ev));

        if (inst->issue_hci_cmd)
        {
            res = true;
        }
    }

    return res;
}

static void HandleCommandStatusEvent(QcaTestInstance *inst)
{
    CONNX_UNUSED(inst);

    /* NOT handle until HCI_NOP is received. */
}

static void HandleVendorEvent(QcaTestInstance *inst)
{
    CONNX_UNUSED(inst);

    /* NOT handle until HCI_NOP is received. */
}

static void HandleCommandCompleteEvent(QcaTestInstance *inst)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_event = &inst->hci_ev_command_complete;
    QcaTestParameter *test_para = &inst->test_para;
    uint32_t eventBits = EVENT_BIT_UNKNOWN;
    bool set_event = true;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (inst->issue_hci_cmd)
    {
        inst->issue_hci_cmd = false;
        SetTestEvent(inst, EVENT_BIT_ALL);
        return;
    }

    if ((hci_event->op_code == HCI_VENDOR_CMD(HCI_VS_PROD_TEST_COMMAND)) ||
        (hci_event->op_code == HCI_NOP))
    {
        QcaSubOpcodeT test_cmd = test_para->test_cmd;
        uint8_t nvm_tag = test_para->nvm_tag;

        if (VALID_PROD_TEST_CMD(test_cmd))
        {
            eventBits = MapProdTestCmd2EventBit(test_cmd);

            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> test_cmd: %02x, event_bits: %04x"),
                           __FUNCTION__, test_cmd, eventBits));
        }
        else if (VALID_NVM_TAG(nvm_tag))
        {
            eventBits = EVENT_BIT_READ_NVM;

            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> nvm_tag: %02x, event_bits: %04x"),
                           __FUNCTION__, nvm_tag, eventBits));
        }
        else
        {
            set_event = false;
        }
    }
    else if (hci_event->op_code == HCI_VENDOR_CMD(HCI_VS_DEBUG_OPCODE))
    {
        QcaSubOpcodeT debug_cmd = test_para->debug_cmd;

        eventBits = MapDebugCmd2EventBit(debug_cmd);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> debug_cmd: %02x, event_bits: %04x"),
                       __FUNCTION__, debug_cmd, eventBits));
    }
    else if (hci_event->op_code == HCI_RESET)
    {
        eventBits = EVENT_BIT_RESET;
    }
    else if (hci_event->op_code == HCI_ENABLE_DUT_MODE)
    {
        eventBits = EVENT_BIT_ENABLE_DUT;
    }
    else if (hci_event->op_code == HCI_WRITE_AUTH_ENABLE)
    {
        eventBits = EVENT_BIT_WRITE_AUTH_ENABLE;
    }
    else if (hci_event->op_code == HCI_WRITE_ENC_MODE)
    {
        eventBits = EVENT_BIT_WRITE_ENC_MODE;
    }
    else if (hci_event->op_code == HCI_WRITE_SCAN_ENABLE)
    {
        eventBits = EVENT_BIT_WRITE_SCAN_ENABLE;
    }
    else if (hci_event->op_code == HCI_SET_EVENT_FILTER)
    {
        eventBits = EVENT_BIT_SET_EVENT_FILTER;
    }
    else
    {
        set_event = false;
    }

    if (set_event)
    {
        SetTestEvent(inst, eventBits);
    }
}

static void HandleTestEvent(QcaTestInstance *inst)
{
    hci_event_code_t event_code = inst->event_code;

    if (event_code == HCI_EV_COMMAND_STATUS)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Handle HCI command status event"), __FUNCTION__));

        HandleCommandStatusEvent(inst);
    }
    else if (event_code == HCI_EV_VENDOR_SPECIFIC)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Handle HCI vendor event"), __FUNCTION__));

        HandleVendorEvent(inst);
    }
    else if (event_code == HCI_EV_COMMAND_COMPLETE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Handle HCI command complete event"), __FUNCTION__));

        HandleCommandCompleteEvent(inst);
    }
    else
    {
        /* Un-handled HCI event. */
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> Un-handled event_code: 0x%02x"), __FUNCTION__, event_code));

        if (inst->issue_hci_cmd)
        {
            inst->issue_hci_cmd = false;
            SetTestEvent(inst, EVENT_BIT_ALL);
        }
    }
}

static uint32_t HandleH4PacketReceived(const uint8_t *data, uint32_t dataLength)
{
    QcaTestInstance *inst = &g_test_inst;
    HCI_EV_GENERIC_T hci_generic_event;
    const uint8_t *ptr;
    uint32_t size_remained = dataLength;
    uint32_t size_parsed = 0;
    uint32_t cur_event_size = 0;
    uint32_t offset = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> dataLength: %d"), __FUNCTION__, dataLength));

    if (!data || !dataLength)
    {
        return 0;
    }

    /* filter the invalid head of H4 event and find the first the BYTE 0x04 */
    offset = FindValidH4Head(data, dataLength);

    if (offset == dataLength)
    {
        return offset;
    }

    /* the invalid byte should be parsed */
    size_parsed += offset;

    ptr = data + offset;

    do
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse H4 event"), __FUNCTION__));

        if (!H4_ParseEvent(ptr, size_remained, &hci_generic_event))
        {
            uint8_t h4_packet_type = ptr[0];

            if (IS_H4_EVT(h4_packet_type))
            {
                /* NOT parse the in-completed H4 event packet. */
                IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> In-completed H4 event packet"), __FUNCTION__));
            }
            else
            {
                /* Discard the whole packet, because it's invalid H4 event packet. */
                IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid H4 event packet, 0x%02x"), __FUNCTION__, h4_packet_type));
                size_parsed = dataLength;
            }

            break;
        }

        if (inst->issue_hci_cmd)
        {
            IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Recv HCI event")));
            IFLOG(DumpBuff(DEBUG_BT_MESSAGE, (const uint8_t *)&hci_generic_event, HCI_EVENT_SIZE(&hci_generic_event)));
        }
        else
        {
            IFLOG(DumpBuff(DEBUG_BT_OUTPUT, (const uint8_t *)&hci_generic_event, HCI_EVENT_SIZE(&hci_generic_event)));
        }

        /* 1 byte for H4 packet type. */
        ++size_parsed;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> size_parsed: %d"), __FUNCTION__, size_parsed));

        if (ParseTestEvent(inst, &hci_generic_event))
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Handle event"), __FUNCTION__));

            HandleTestEvent(inst);
        }

        cur_event_size = HCI_EVENT_SIZE(&hci_generic_event);
        size_parsed += cur_event_size;

        ptr = data + size_parsed;

        size_remained = dataLength - size_parsed;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> cur_event_size: %d, size_remained: %d, size_parsed: %d"),
                       __FUNCTION__, cur_event_size, size_remained, size_parsed));
    }
    while (size_parsed < dataLength);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Final size_parsed: %d"), __FUNCTION__, size_parsed));

    return size_parsed;
}

static bool OpenH4(QcaTestInstance *inst)
{
    QcaTestParameter *test_para = &inst->test_para;
    H4RegisterInfo ri;
    H4RegisterInfo *registerInfo = &ri;
    HciTransportSetting *transportSetting = &registerInfo->transportSetting;
    char *btsnoop_file = ConnxCreateFullFileName(test_para->config_path, BT_TEST_SNOOP_FILE);
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    registerInfo->size = sizeof(H4RegisterInfo);

    transportSetting->device         = test_para->serial_port;
    transportSetting->init_baudrate  = test_para->baudrate;
    transportSetting->reset_baudrate = test_para->baudrate;
    transportSetting->flow_control   = test_para->flow_control;

    registerInfo->rxDataFn = HandleH4PacketReceived;
    registerInfo->btsnoop_file_name = btsnoop_file;

    res = H4_OpenConnection(registerInfo);

    free(btsnoop_file);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, res ? "Succeed" : "Fail"));

    return res;
}

static void CloseH4(QcaTestInstance *inst)
{
    CONNX_UNUSED(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    H4_CloseConnection();
}

/* ------------------------------------------------------------------------------- */

static ConnxResult WaitTestEvent(QcaTestInstance *inst, uint16_t timeoutInMs, uint32_t eventBits)
{
    ConnxHandle eventHandle = inst->event_handle;
    uint32_t tempVal = 0;
    ConnxResult result = 0;
    ConnxTime startTime = ConnxGetTickCount();
    ConnxTime timeElapsed = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> + timeout: %dms, eventBits: 0x%x"), __FUNCTION__, timeoutInMs, eventBits));

    do
    {
        result = ConnxEventWait(eventHandle, timeoutInMs, &tempVal);

        if (tempVal == eventBits)
        {
            /* Complete waiting the eventBits expected. */
            break;
        }
        else
        {
            /* Not eventBits expected. Continue waiting. */
            IFLOG(DebugOut(DEBUG_BT_WARN, TEXT("<%s> NOT eventBits expected, actual_event: 0x%x"), __FUNCTION__, tempVal));

            if (timeoutInMs == CONNX_EVENT_WAIT_INFINITE)
            {
                /* FIXME */
                break;
            }

            timeElapsed = ConnxGetTimeElapsed(startTime);

            if (timeElapsed >= (ConnxTime) timeoutInMs)
            {
                /* Timeout. */
                break;
            }

            /* Continue waiting eventBits. */
        }
    }
    while (0);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> - result: 0x%x, actual_event: 0x%x, time elapsed: %dms"),
                   __FUNCTION__, result, tempVal, ConnxGetTimeElapsed(startTime)));

    return (IS_CONNX_RESULT_SUCCESS(result) && (tempVal == eventBits)) ? CONNX_RESULT_SUCCESS : CONNX_RESULT_FAIL;
}

static void SetTestEvent(QcaTestInstance *inst, uint32_t eventBits)
{
    ConnxHandle eventHandle = inst->event_handle;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> eventBits: 0x%x"), __FUNCTION__, eventBits));

    ConnxEventSet(eventHandle, eventBits);
}

static void BlockTest(QcaTestInstance *inst, QcaSubOpcodeT test_cmd)
{
    QcaTestParameter *test_para = &inst->test_para;
    uint32_t delay = test_para->delay; /* second */

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> test_cmd: %02x, delay: %d sec"), __FUNCTION__, test_cmd, delay));

    switch (test_cmd)
    {
        case PROD_TEST_SLAVE:
        case PROD_TEST_MASTER:
        case PROD_TEST_TX_BURST:
        case PROD_TEST_RX_BURST:
        {
            /* Block infinitely? */
            sleep(delay);
            break;
        }

        default:
        {
            sleep(delay);
            break;
        }
    }
}

static void InitLog(QcaTestInstance *inst)
{
    ConnxLogSetting log_setting;
    QcaTestParameter *test_para = &inst->test_para;

    ConnxInitLog(test_para->config_path,
                 BT_LOG_CONFIG_FILE,
                 BT_TEST_LOG_FILE,
                 &log_setting);
}

static void DeinitLog(QcaTestInstance *inst)
{
    CONNX_UNUSED(inst);

    ConnxDeinitLog();
}

static bool InitTest(QcaTestInstance *inst)
{
    memset(inst, 0, sizeof(QcaTestInstance));

    inst->event_handle = ConnxEventCreate();

    InitHciCmdList(&inst->hci_cmd_list);

    if (!InitTestParameter(inst))
        return false;

    InitLog(inst);

    if (!OpenH4(inst))
        return false;

    return true;
}

static void DeinitTest(QcaTestInstance *inst)
{
    CloseH4(inst);

    if (inst->event_handle)
    {
        ConnxEventDestroy(inst->event_handle);
        inst->event_handle = NULL;
    }

    DeinitHciCmdList(&inst->hci_cmd_list);
}

/* ------------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    QcaTestInstance *inst = &g_test_inst;
    ConnxTime startTime = ConnxGetTickCount();

    ConnxArgSearchInit((uint32_t) argc, (char **) argv);

    if (!InitTest(inst))
    {
        /* Fail to init test. */
        return 1;
    }

    StartTest(inst);

    DeinitTest(inst);

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("Finish bt_test, time elapsed: %dms"), ConnxGetTimeElapsed(startTime)));

    DeinitLog(inst);

    return 0;
}
