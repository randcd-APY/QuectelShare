/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #7 $
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "connx_log.h"
#include "connx_log_setting.h"
#include "connx_util.h"
#include "connx_file.h"
#include "connx_time.h"
#include "bt_hci_cmd.h"
#include "bt_h4_transport.h"
#include "boot_qca.h"


/* Timeout in ms to wait the response for HCI command. */
#define DEFAULT_WAIT_TIMEOUT                5000  /* CONNX_EVENT_WAIT_INFINITE */

/* Delay in ms after HCI vendor command of setting baudrate is issued. */
#define DEFAULT_SET_BAUDRATE_DELAY          10

/* Timeout in ms to restart UART delay. */
#define DEFAULT_RESTART_UART_DELAY          100

/* Event bits for QCA event handle. */
#define EVENT_BITS_GET_CHIP_VER_CFM             ((uint16_t) 0x0001)
#define EVENT_BITS_DOWNLOAD_FW_CFM              ((uint16_t) 0x0002)
#define EVENT_BITS_FW_LOG_CFM                   ((uint16_t) 0x0003)
#define EVENT_BITS_SET_POWER_CFG_CFM            ((uint16_t) 0x0004)
#define EVENT_BITS_SET_LE_TX_POWER_CTRL_CFM     ((uint16_t) 0x0005)
#define EVENT_BITS_SET_XTAL_FINE_TUNING_CFM     ((uint16_t) 0x0006)
#define EVENT_BITS_SET_XTAL_FINE_TUNING2_CFM    ((uint16_t) 0x0007)
#define EVENT_BITS_HCI_RESET_CFM                ((uint16_t) 0x0008)
#define EVENT_BITS_HCI_READ_BD_ADDR_CFM         ((uint16_t) 0x0009)
#define EVENT_BITS_UNKNOWN                      ((uint16_t) 0xffff)

#define VALID_QCA_EVENT_BITS(eventBits)     (((eventBits) != 0) && ((eventBits) != EVENT_BITS_UNKNOWN))

#define QCA_EVENT_BITS(eventBits, result)   SET_DWORD(eventBits, result)

typedef uint16_t    QcaBootStageT;
#define QCA_BOOT_STAGE_NULL                     ((QcaBootStageT) 0)
#define QCA_BOOT_STAGE_H4_TRANSPORT_OPEN        ((QcaBootStageT) 1)
#define QCA_BOOT_STAGE_SET_BAUDRATE             ((QcaBootStageT) 2)
#define QCA_BOOT_STAGE_GET_CHIP_VERSION         ((QcaBootStageT) 3)
#define QCA_BOOT_STAGE_GET_FW_FILE              ((QcaBootStageT) 4)
#define QCA_BOOT_STAGE_DOWNLOAD_RAM_PATCH       ((QcaBootStageT) 5)
#define QCA_BOOT_STAGE_DOWNLOAD_NVM             ((QcaBootStageT) 6)
#define QCA_BOOT_STAGE_SEND_FW_LOG_CMD          ((QcaBootStageT) 7)
#define QCA_BOOT_STAGE_SET_POWER_CFG            ((QcaBootStageT) 8)
#define QCA_BOOT_STAGE_SET_LE_TX_POWER_CTRL     ((QcaBootStageT) 9)
#define QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING     ((QcaBootStageT) 10)
#define QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING2    ((QcaBootStageT) 11)
#define QCA_BOOT_STAGE_HCI_RESET                ((QcaBootStageT) 12)

#define QCA_GET_TRANSPORT_SETTING(inst)     (&(inst)->transportSetting)
#define QCA_GET_FIRMWARE_PATH(inst)         ((inst)->firmware_path)
#define QCA_GET_LOG_PATH(inst)              ((inst)->log_path)
#define QCA_GET_CONFIG_PATH(inst)           ((inst)->config_path)
#define QCA_GET_EXTRA_CONFIG(inst)          (&(inst)->extraConfig)
#define QCA_GET_EVENT_HANDLE(inst)          ((inst)->eventHandle)
#define QCA_GET_BOOT_STAGE(inst)            ((inst)->boot_stage)
#define QCA_GET_CHIP_VER(inst)              ((inst)->qca_chip_ver)
#define QCA_GET_RAM_PATCH_FILE_NAME(inst)   ((inst)->ram_patch_file_name)
#define QCA_GET_NVM_FILE_NAME(inst)         ((inst)->nvm_file_name)
#define QCA_GET_FW_DOWNLOAD_RESULT(inst)    ((inst)->fw_download_result)
#define QCA_GET_WAIT_VS_EVT(inst)           ((inst)->wait_vs_evt)
#define QCA_GET_WAIT_CC_EVT(inst)           ((inst)->wait_cc_evt)
#define QCA_GET_EVENT_CODE(inst)            ((inst)->event_code)
#define QCA_GET_EVENT_PARSED(inst)          ((inst)->event_parsed)
#define QCA_GET_COMMAND_COMPLETE_EVT(inst)  (&(inst)->hci_ev_command_complete)
#define QCA_GET_BD_ADDR(inst)               ((inst)->bd_addr)
#define QCA_GET_BTSNOOP_FILE_NAME(inst)     ((inst)->btsnoop_file_name)

#undef CHK
#define CHK(res, info)                      { \
                                                if (!IS_CONNX_RESULT_SUCCESS(res)) \
                                                { \
                                                    IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> %s fail"), __FUNCTION__, info)); \
                                                    break; \
                                                } \
                                            }

#undef VERIFY
#define VERIFY(res, info)                   { \
                                                if (!IS_CONNX_RESULT_SUCCESS(res)) \
                                                { \
                                                    IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> %s fail"), __FUNCTION__, info)); \
                                                    return res; \
                                                } \
                                            }


typedef struct
{
    int     speed;          /* Host baudrate in bps. */
    uint8_t qca_baudrate;   /* Key value of QCA chip's baudrate, used in HCI vendor command. */
} QcaBaudrateMap;

typedef struct
{
    HciTransportSetting         transportSetting;           /* HCI transport setting. */
    char                       *firmware_path;              /* Path for QCA chip's firmware (tlv/bin). */
    char                       *config_path;                /* Path for QCA chip's configuration, such as BD_ADDR or PCM setting. */
    char                       *log_path;                   /* Path for bootstrap log, such as btsnoop. */
    QcaExtraConfig              extraConfig;                /* Extra firmware configuration for QCA chip. */
    ConnxHandle                 eventHandle;                /* Event handle for thread's synchronization. */

    QcaBootStageT               boot_stage;                 /* QCA chip boot stage. */
    uint32_t                    qca_chip_ver;               /* QCA version (soc_id, build_ver). */

    char                       *ram_patch_file_name;        /* RAM patch full file name. */
    char                       *nvm_file_name;              /* NVM full file name. */

    QcaHciVendorCmdResT         fw_download_result;         /* QCA firmware download result. */
    bool                        wait_vs_evt;                /* true: wait vendor status event during fw downloading, false: not. */
    bool                        wait_cc_evt;                /* true: wait command complete event during fw downloading, false: not. */

    hci_event_code_t            event_code;                 /* HCI event code. */
    void                       *event_parsed;               /* HCI vendor event parsed. */
    HCI_EV_COMMAND_COMPLETE_T   hci_ev_command_complete;    /* HCI event of command complete. */
    uint8_t                     bd_addr[BT_ADDR_LENGTH];    /* Bluetooth device address. */

    char                       *btsnoop_file_name;          /* Full file name for BT snoop log. */
} QcaInstance;


typedef struct
{
    uint32_t    clock_value;
    uint16_t    clock_divisor;
} QcaBitClockMap;

static QcaInstance qca_inst;

static QcaBitClockMap qca_bit_clk_map[] = 
{
    /* PCM bit clock,               PCM bit clock divisor */
    { FW_CONF_PCM_BCLK_256K,        FW_CONF_PCM_CLOCK_DIVISOR_256K, },

    { FW_CONF_PCM_BCLK_2M,          FW_CONF_PCM_CLOCK_DIVISOR_2M,   },

    { FW_CONF_PCM_BCLK_4M,          FW_CONF_PCM_CLOCK_DIVISOR_4M,   }
};

static QcaBitClockMap qca_sync_clk_map[] = 
{
    /* PCM sync clock ,             PCM sync clock divisor */
    { FW_CONF_PCM_SYNC_CLOCK_8K,    FW_CONF_PCM_FRAME_DIVISOR_8K,   },

    { FW_CONF_PCM_SYNC_CLOCK_16K,   FW_CONF_PCM_FRAME_DIVISOR_16K,  },
};

static QcaFirmwareMap qca_firmware_map[] =
{
    /* QCA chip version,    RAM patch file,                 NVM file */

    /* ROME 3.0 (QCA6174) */
    { QCA_VER_ROME_3_0,     ROME_RAMPATCH_TLV_3_0_0_FILE,   ROME_NVM_TLV_3_0_0_FILE },

    /* ROME 3.2 (QCA6574) */
    { QCA_VER_ROME_3_2,     ROME_RAMPATCH_TLV_3_0_2_FILE,   ROME_NVM_TLV_3_0_2_FILE },

    /* Tufello 1.0 */
    { QCA_VER_TUFELLO_1_0,  TF_RAMPATCH_TLV_1_0_0_FILE,     TF_NVM_TLV_1_0_0_FILE   },

    /* Tufello 1.1 */
    { QCA_VER_TUFELLO_1_1,  TF_RAMPATCH_TLV_1_0_1_FILE,     TF_NVM_TLV_1_0_1_FILE   }
};

static QcaBaudrateMap qca_baudrate_map[] =
{
    /* Host baudrate(bps),  QCA baudrate */
    { 115200,               0x00 },
    { 57600,                0x01 },
    { 38400,                0x02 },
    { 19200,                0x03 },
    { 9600,                 0x04 },
    { 230400,               0x05 },
    { 250000,               0x06 },
    { 460800,               0x07 },
    { 500000,               0x08 },
    { 720000,               0x09 },
    { 921600,               0x0A },
    { 1000000,              0x0B },
    { 1250000,              0x0C },
    { 2000000,              0x0D },
    { 3000000,              0x0E },
    { 4000000,              0x0F },
    { 1600000,              0x10 },
    { 3200000,              0x11 },
    { 3500000,              0x12 },
};


static const char *GetQcaChipString(uint32_t qca_chip_ver);
static uint8_t Map2QcaBaudrate(int speed);
static uint32_t CalculateFirmwareSize(QCA_FW_HEADER_T *fw_header);
static void DumpExtraConfig(QcaExtraConfig *extraConfig);
static const char *GetBootStageString(QcaBootStageT boot_stage);
static bool GetFirmwareFileName(uint32_t qca_chip_ver, char **ram_patch_file, char **nvm_file);

static uint32_t HandleH4DataReceived(const uint8_t *data, uint32_t dataLength);

static bool QCA_ParseEvent(QcaInstance *inst, HCI_EV_GENERIC_T *hci_event);
static bool QCA_ParseCommandCompleteEvent(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseVendorEvent(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtSetBaudrateResp(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtRequestResult(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtGetChipVerResult(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtDownloadFwResult(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtFwLog(QcaInstance *inst, uint8_t length, uint8_t *data);
static bool QCA_ParseEdlEvtNvmResp(QcaInstance *inst, uint8_t length, uint8_t *data);

static void QCA_HandleEvent(QcaInstance *inst);
static void QCA_HandleEventCommandComplete(QcaInstance *inst);
static void QCA_HandleCommandCompleteNop(QcaInstance *inst);
static void QCA_HandleCommandCompleteReset(QcaInstance *inst);
static void QCA_HandleCommandCompleteReadBdAddr(QcaInstance *inst);

static bool QCA_MapEdlEvt2EventBits(QcaInstance *inst, void *event_parsed, uint16_t *eventBits, bool *success);

static ConnxResult QCA_GetFirmwareFile(QcaInstance *inst);
static ConnxResult QCA_ReadFirmwareFile(QcaInstance *inst, uint8_t fw_type, uint8_t **buffer, uint32_t *buffer_size);

static void QCA_InitInstance(QcaInstance *inst);
static void QCA_DeinitInstance(QcaInstance *inst);

static void QCA_InitExtraConfig(char *config_path, QcaExtraConfig *extraConfig);
static void QCA_InitBtsnoop(QcaInstance *inst);

static bool QCA_OpenH4Transport(QcaInstance *inst);
static void QCA_CloseH4Transport(QcaInstance *inst);

static ConnxResult QCA_IssueEdlCmd(QcaInstance *inst, QcaEdlCmdT edl_cmd, uint8_t *payload, uint8_t payload_length);

static ConnxResult QCA_GetChipVersion(QcaInstance *inst);
static ConnxResult QCA_SetBaudrate(QcaInstance *inst);
static ConnxResult QCA_DownloadAllFirmware(QcaInstance *inst);
static ConnxResult QCA_DownloadRamPatch(QcaInstance *inst);
static ConnxResult QCA_DownloadNvm(QcaInstance *inst);
static ConnxResult QCA_DownloadFirmware(QcaInstance *inst, uint8_t fw_type);
static ConnxResult QCA_RetrieveDownloadCfg(QcaInstance *inst, uint8_t *buffer, uint32_t buffer_size);
static ConnxResult QCA_UpdateFirmware(QcaInstance *inst, uint8_t *buffer, uint32_t buffer_size);
static void QCA_UpdateNvmBtAddr(QcaInstance *inst, QCA_NVM_HEADER_T *nvm);
static void QCA_UpdateNvmHciParameters(QcaInstance *inst, QCA_NVM_HEADER_T *nvm);
static void QCA_UpdateNvmDeepSleep(QcaInstance *inst, QCA_NVM_HEADER_T *nvm);
static void QCA_UpdateNvmPcmLoopback(QcaInstance *inst, QCA_NVM_HEADER_T *nvm);
static void QCA_UpdateNvmCodecConfiguration(QcaInstance *inst, QCA_NVM_HEADER_T *nvm);

static ConnxResult QCA_EnableFwLog(QcaInstance *inst);

static ConnxResult QCA_IssueNvmCmd(QcaInstance *inst, QcaNvmCmdT sub_opcode, uint8_t nvm_tag,
                                   uint8_t *tag_value, uint8_t tag_value_length);
static ConnxResult QCA_WriteNvm(QcaInstance *inst, uint8_t nvm_tag, uint8_t *tag_value,
                                uint8_t tag_value_length, uint16_t boot_stage, uint32_t eventBits);
static ConnxResult QCA_SetPowerConfiguration(QcaInstance *inst);
static ConnxResult QCA_SetLETxPowerControl(QcaInstance *inst);
static ConnxResult QCA_SetXtalFineTuning(QcaInstance *inst);
static ConnxResult QCA_SetXtalFineTuning2(QcaInstance *inst);

static ConnxResult QCA_IssueHciResetCmd(QcaInstance *inst);

static ConnxResult QCA_WaitEvent(QcaInstance *inst, uint16_t timeoutInMs, uint32_t eventBits);
static void QCA_SetEvent(QcaInstance *inst, uint32_t eventBits);
static void QCA_NotifyResult(QcaInstance *inst, uint16_t eventBits, bool success);

static void QCA_SetBootStage(QcaInstance *inst, uint16_t boot_stage);

static void QCA_StoreEventCode(QcaInstance *inst, hci_event_code_t event_code);
static void QCA_StoreEventParsed(QcaInstance *inst, void *event_parsed);
static void QCA_FreeEventParsed(QcaInstance *inst);

static void QCA_StoreChipVer(QcaInstance *inst, QCA_EDL_EVT_GET_CHIP_VER_RES_T *chip_ver_res);
static void QCA_StoreFwDownloadCfg(QcaInstance *inst, bool wait_vs_evt, bool wait_cc_evt);

static bool QCA_IsFwLogCmdSent(QcaInstance *inst);

static void QCA_IssueOptionalCommand(QcaInstance *inst);


static const char *GetQcaChipString(uint32_t qca_chip_ver)
{
    switch (qca_chip_ver)
    {
        case QCA_VER_ROME_3_0:
            return "Rome 3.0";

        case QCA_VER_ROME_3_2:
            return "Rome 3.2";

        case QCA_VER_TUFELLO_1_0:
            return "Tufello 1.0";

        case QCA_VER_TUFELLO_1_1:
            return "Tufello 1.1";

        default:
            return "Unknown";
    }
}

uint16_t MapBitClock2Divisor(uint32_t bit_clock)
{
    uint32_t index = 0;
    
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s>"), __FUNCTION__));

    for (index = 0; index < sizeof(qca_bit_clk_map)/sizeof(QcaBitClockMap); index++)
    {    
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> index:%d"), __FUNCTION__, index));    

        if (bit_clock == qca_bit_clk_map[index].clock_value)
        {
            return qca_bit_clk_map[index].clock_divisor;
        }
    }    

    return FW_CONF_PCM_CLOCK_DIVISOR_256K;
}

uint16_t MapSyncClock2Divisor(uint32_t sync_clock)
{
    uint32_t index = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s>"), __FUNCTION__));

    for (index = 0; index < sizeof(qca_sync_clk_map)/sizeof(QcaBitClockMap); index++)
    {    
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> index:%d"), __FUNCTION__, index));        

        if (sync_clock == qca_sync_clk_map[index].clock_value)
        {
            return qca_sync_clk_map[index].clock_divisor;
        }
    }    

    return FW_CONF_PCM_FRAME_DIVISOR_8K;
}

static void DumpExtraConfig(QcaExtraConfig *extraConfig)
{
    IFLOG(BT_ADDR_T *bt_addr);
    IFLOG(char *pcm_role);

    if (!extraConfig)
        return;

    IFLOG(bt_addr = &extraConfig->bt_addr);
    IFLOG(pcm_role = (extraConfig->pcm_role == FW_CONF_PCM_MASTER) ? "master" : "slave");

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_addr: %x, bt_addr %04x:%02x:%06x"),
                   __FUNCTION__, extraConfig->valid_addr, bt_addr->nap, bt_addr->uap, bt_addr->lap));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_ibs: %x, enable_ibs: %x"),
                   __FUNCTION__, extraConfig->valid_ibs, extraConfig->enable_ibs));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_deep_sleep: %x, enable_deep_sleep: %x"),
                   __FUNCTION__, extraConfig->valid_deep_sleep, extraConfig->enable_deep_sleep));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_pcm: %x, pcm_role: %x (%s)"),
                   __FUNCTION__, extraConfig->valid_pcm, extraConfig->pcm_role, pcm_role));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_invert_bclk: %x, invert_bclk: %x"),
                   __FUNCTION__, extraConfig->valid_invert_bclk, extraConfig->invert_bclk));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_nbs_bclk: %x, nbs_bclk: %d (0x%x)"),
                   __FUNCTION__, extraConfig->valid_nbs_bclk, extraConfig->nbs_bclk, extraConfig->nbs_bclk));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_wbs_bclk: %x, wbs_bclk: %d (0x%x)"),
                   __FUNCTION__, extraConfig->valid_wbs_bclk, extraConfig->wbs_bclk, extraConfig->wbs_bclk));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_nbs_sclk: %x, nbs_sclk: %d (0x%x)"),
                   __FUNCTION__, extraConfig->valid_nbs_sclk, extraConfig->nbs_sclk, extraConfig->nbs_sclk));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_wbs_sclk: %x, wbs_sclk: %d (0x%x)"),
                   __FUNCTION__, extraConfig->valid_wbs_sclk, extraConfig->wbs_sclk, extraConfig->wbs_sclk));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_pcm_lp: %x, enable_pcm_lp: %x"),
                   __FUNCTION__, extraConfig->valid_pcm_lp, extraConfig->enable_pcm_lp));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_fw_log: %x, enable_fw_log: %x"),
                   __FUNCTION__, extraConfig->valid_fw_log, extraConfig->enable_fw_log));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_power_cfg: %x"), __FUNCTION__, extraConfig->valid_power_cfg));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_le_tx_power_ctrl: %x"), __FUNCTION__, extraConfig->valid_le_tx_power_ctrl));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_xtal_fine_tuning: %x"), __FUNCTION__, extraConfig->valid_xtal_fine_tuning));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_xtal_fine_tuning2: %x"), __FUNCTION__, extraConfig->valid_xtal_fine_tuning2));
}

static const char *GetBootStageString(QcaBootStageT boot_stage)
{
    switch (boot_stage)
    {
        case QCA_BOOT_STAGE_NULL:
            return "NULL";

        case QCA_BOOT_STAGE_H4_TRANSPORT_OPEN:
            return "H4_TRANSPORT_OPEN";

        case QCA_BOOT_STAGE_SET_BAUDRATE:
            return "SET_BAUDRATE";

        case QCA_BOOT_STAGE_GET_CHIP_VERSION:
            return "GET_CHIP_VERSION";

        case QCA_BOOT_STAGE_GET_FW_FILE:
            return "GET_FW_FILE";

        case QCA_BOOT_STAGE_DOWNLOAD_RAM_PATCH:
            return "DOWNLOAD_RAM_PATCH";

        case QCA_BOOT_STAGE_DOWNLOAD_NVM:
            return "DOWNLOAD_NVM";

        case QCA_BOOT_STAGE_SEND_FW_LOG_CMD:
            return "SEND_FW_LOG_CMD";

        case QCA_BOOT_STAGE_SET_POWER_CFG:
            return "QCA_BOOT_STAGE_SET_POWER_CFG";

        case QCA_BOOT_STAGE_SET_LE_TX_POWER_CTRL:
            return "QCA_BOOT_STAGE_SET_LE_TX_POWER_CTRL";

        case QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING:
            return "QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING";

        case QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING2:
            return "QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING2";

        case QCA_BOOT_STAGE_HCI_RESET:
            return "HCI_RESET";

        default:
            return "UNKNOWN";
    }
}

static uint32_t CalculateFirmwareSize(QCA_FW_HEADER_T *fw_header)
{
    uint8_t *ptr = (uint8_t *)fw_header;
    uint32_t fw_size = sizeof(QCA_FW_HEADER_T);
    uint32_t tmp_val = 0;

    if (!fw_header)
        return 0;

    /* The 1st byte in firmware header is for firmware type. */
    ++ptr;

    CONNX_CONVERT_24_FROM_XAP(tmp_val, ptr);

    fw_size += tmp_val & 0x00FFFFFF;

    return fw_size;
}

static uint8_t Map2QcaBaudrate(int speed)
{
    QcaBaudrateMap *brm;
    uint32_t count = CONNX_COUNT_OF(qca_baudrate_map);
    uint32_t index = 0;

    for (index = 0; index < count; index++)
    {
        brm = &qca_baudrate_map[index];

        if (speed == brm->speed)
        {
            /* Found. */
            return brm->qca_baudrate;
        }
    }

    return QCA_BAUDRATE_RESERVED;
}

static bool GetFirmwareFileName(uint32_t qca_chip_ver, char **ram_patch_file, char **nvm_file)
{
    uint32_t count = CONNX_COUNT_OF(qca_firmware_map);
    uint32_t index;
    QcaFirmwareMap *qfm;
    bool found = false;

    if (!ram_patch_file || !nvm_file)
    {
        return false;
    }

    for (index = 0; index < count; index++)
    {
        qfm = &qca_firmware_map[index];

        if (qfm->qca_chip_ver == qca_chip_ver)
        {
            *ram_patch_file = qfm->ram_patch_file;
            *nvm_file       = qfm->nvm_file;

            found = true;
            break;
        }
    }

    return found;
}

static void QCA_SetBootStage(QcaInstance *inst, uint16_t boot_stage)
{
    QCA_GET_BOOT_STAGE(inst) = boot_stage;
}

static void QCA_StoreEventCode(QcaInstance *inst, hci_event_code_t event_code)
{
    QCA_GET_EVENT_CODE(inst) = event_code;
}

static void QCA_StoreEventParsed(QcaInstance *inst, void *event_parsed)
{
    QCA_GET_EVENT_PARSED(inst) = event_parsed;
}

static void QCA_FreeEventParsed(QcaInstance *inst)
{
    if (QCA_GET_EVENT_PARSED(inst))
    {
        free(QCA_GET_EVENT_PARSED(inst));
        QCA_GET_EVENT_PARSED(inst) = NULL;
    }
}

static void QCA_StoreChipVer(QcaInstance *inst, QCA_EDL_EVT_GET_CHIP_VER_RES_T *chip_ver_res)
{
    uint32_t qca_chip_ver = QCA_VERSION(chip_ver_res->soc_id, chip_ver_res->rom_build_ver);

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> %s, chip ver: 0x%08x"),
                   __FUNCTION__, GetQcaChipString(qca_chip_ver), qca_chip_ver));

    QCA_GET_CHIP_VER(inst) = qca_chip_ver;
}

static void QCA_StoreFwDownloadCfg(QcaInstance *inst, bool wait_vs_evt, bool wait_cc_evt)
{
    QCA_GET_WAIT_VS_EVT(inst) = wait_vs_evt;

    QCA_GET_WAIT_CC_EVT(inst) = wait_cc_evt;
}

static bool QCA_IsFwLogCmdSent(QcaInstance *inst)
{
    QcaBootStageT boot_stage = QCA_GET_BOOT_STAGE(inst);

    return (boot_stage == QCA_BOOT_STAGE_SEND_FW_LOG_CMD) ? true : false;;
}

static bool QCA_MapEdlEvt2EventBits(QcaInstance *inst, void *event_parsed, uint16_t *eventBits, bool *success)
{
    QcaEdlEvtT edl_evt;
    uint16_t tmp_event_bits = 0;
    bool tmp_success = true;
    bool mapped = true;

    if (!event_parsed || !eventBits || !success)
        return false;

    *eventBits = EVENT_BITS_UNKNOWN;
    *success   = false;

    edl_evt = *((QcaEdlEvtT *)event_parsed);

    if (edl_evt == EDL_EVT_REQUEST_RESULT)
    {
        if (QCA_IsFwLogCmdSent(inst))
        {
            tmp_event_bits = EVENT_BITS_FW_LOG_CFM;
        }
        else
        {
            QCA_EDL_EVT_RES_HEADER_T *header = (QCA_EDL_EVT_RES_HEADER_T *)event_parsed;
            QcaEdlEvtResT edl_evt_res = header->edl_evt_res;

            switch (edl_evt_res)
            {
                case EDL_EVT_RES_GET_APP_VER:
                case EDL_EVT_RES_GET_CHIP_VER:
                {
                    tmp_event_bits = EVENT_BITS_GET_CHIP_VER_CFM;
                    break;
                }

                case EDL_EVT_RES_DOWNLOAD_FIRMWARE:
                {
                    QCA_EDL_EVT_DOWNLOAD_FW_RES_T *mv = (QCA_EDL_EVT_DOWNLOAD_FW_RES_T *)event_parsed;

                    tmp_success = IS_HCI_VS_CMD_RESULT_SUCCESS(mv->result);

                    tmp_event_bits = EVENT_BITS_DOWNLOAD_FW_CFM;
                    break;
                }

                default:
                {
                    /* Unknown EDL event result. */
                    mapped = false;
                    break;
                }
            }
        }
    }
    else if (edl_evt == EDL_EVT_NVM)
    {
        QCA_EDL_EVT_NVM_HEADER_T *header = (QCA_EDL_EVT_NVM_HEADER_T *)event_parsed;
        uint8_t nvm_tag = header->nvm_tag;

        switch (nvm_tag)
        {
            case NVM_TAG_NUM_28:
            {
                tmp_event_bits = EVENT_BITS_SET_XTAL_FINE_TUNING_CFM;
                break;                
            }

            case NVM_TAG_NUM_36:
            {
                tmp_event_bits = EVENT_BITS_SET_POWER_CFG_CFM;
                break;
            }

            case NVM_TAG_NUM_82:
            {
                tmp_event_bits = EVENT_BITS_SET_XTAL_FINE_TUNING2_CFM;
                break;                
            }

            case NVM_TAG_NUM_83:
            {
                tmp_event_bits = EVENT_BITS_SET_LE_TX_POWER_CTRL_CFM;
                break;                
            }

            default:
            {
                /* Un-handled NVM tag. */
                mapped = false;
                break;
            }
        }
    }
    else if (edl_evt == EDL_EVT_SET_BAUDRATE_RESP)
    {
        /* Needn't to map event bits here. The reason is that the main thread doesn't wait the
           event for HCI_VS_SET_BAUDRATE. So it's not necessary to notify the event either. */
        mapped = false;
    }
    else
    {
        /* Unknown EDL event. */
        mapped = false;
    }

    if (mapped)
    {
        *eventBits = tmp_event_bits;
        *success   = tmp_success;
    }

    return mapped;
}

static ConnxResult QCA_GetFirmwareFile(QcaInstance *inst)
{
    uint32_t qca_chip_ver = QCA_GET_CHIP_VER(inst);
    char *firmware_path = QCA_GET_FIRMWARE_PATH(inst);
    char *ram_patch_file = NULL;
    char *nvm_file = NULL;

    if (!GetFirmwareFileName(qca_chip_ver, &ram_patch_file, &nvm_file))
    {
        return CONNX_RESULT_FAIL;
    }

    QCA_GET_RAM_PATCH_FILE_NAME(inst) = ConnxCreateFullFileName(firmware_path, ram_patch_file);
    QCA_GET_NVM_FILE_NAME(inst)       = ConnxCreateFullFileName(firmware_path, nvm_file);

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_GET_FW_FILE);

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_ReadFirmwareFile(QcaInstance *inst, uint8_t fw_type, uint8_t **buffer, uint32_t *buffer_size)
{
    char *file_name = NULL;
    uint32_t file_size = 0;
    ConnxFileHandle file_handle = NULL;
    uint8_t *tmp_buf = NULL;
    size_t byte_read = 0;
    ConnxResult result = 0;

    if (!buffer || !buffer_size)
        return CONNX_RESULT_INVALID_PARAMETER;

    if (fw_type == QCA_FW_TYPE_RAM_PATCH)
    {
        file_name = QCA_GET_RAM_PATCH_FILE_NAME(inst);
    }
    else if (fw_type == QCA_FW_TYPE_NVM)
    {
        file_name = QCA_GET_NVM_FILE_NAME(inst);
    }
    else
    {
        /* Unknown firmware type. */
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> fw_type: 0x%02x, fw_file: %s"), __FUNCTION__, fw_type, file_name));

    result = ConnxFileGetSize(file_name, &file_size);

    VERIFY(result, "Get firmware file size");

    if (!file_size)
    {
        return CONNX_FILE_RESULT_NO_SPACE;
    }

    tmp_buf = (uint8_t *)malloc(file_size);

    if (!tmp_buf)
    {
        return CONNX_RESULT_OUT_OF_MEMEORY;
    }

    do
    {
        result = ConnxFileOpen(&file_handle,
                               file_name,
                               CONNX_FILE_OPEN_FLAGS_READ_ONLY,
                               CONNX_FILE_PERMS_USER_READ);
        CHK(result, "Open QCA firmware file");

        result = ConnxFileRead(tmp_buf, file_size, file_handle, &byte_read);
        CHK(result, "Read QCA firmware file");

        if (byte_read != file_size)
        {
            result = CONNX_RESULT_FAIL;
            break;
        }
    }
    while (0);

    if (IS_CONNX_RESULT_SUCCESS(result))
    {
        *buffer = tmp_buf;
        *buffer_size = file_size;
    }
    else
    {
        *buffer = NULL;
        *buffer_size = 0;

        free(tmp_buf);
        tmp_buf = NULL;
    }

    ConnxFileClose(file_handle);

    return result;
}

static ConnxResult QCA_WaitEvent(QcaInstance *inst, uint16_t timeoutInMs, uint32_t eventBits)
{
    ConnxHandle eventHandle = QCA_GET_EVENT_HANDLE(inst);
    uint32_t tempVal = 0;
    ConnxResult result = 0;
    ConnxTime startTime = ConnxGetTickCount();
    ConnxTime timeElapsed = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> + timeout: %dms, eventBits: 0x%x"), __FUNCTION__, timeoutInMs, eventBits));

    do
    {
        result = ConnxEventWait(eventHandle, timeoutInMs, &tempVal);

        CHK(result, "Wait event");

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

static void QCA_SetEvent(QcaInstance *inst, uint32_t eventBits)
{
    ConnxHandle eventHandle = QCA_GET_EVENT_HANDLE(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> eventBits: 0x%x"), __FUNCTION__, eventBits));

    ConnxEventSet(eventHandle, eventBits);
}

static void QCA_NotifyResult(QcaInstance *inst, uint16_t eventBits, bool success)
{
    ConnxResult result = success ? CONNX_RESULT_SUCCESS : CONNX_RESULT_FAIL;
    uint32_t tmp_val = QCA_EVENT_BITS(eventBits, result);

    QCA_SetEvent(inst, tmp_val);
}

static void QCA_InitExtraConfig(char *config_path, QcaExtraConfig *extraConfig)
{
    BT_ADDR_T bt_addr;

    if (!config_path || !extraConfig)
        return;

    memset(extraConfig, 0, sizeof(QcaExtraConfig));

    QCA_GetFirmwareConfig(config_path, BT_FIRMWARE_CONFIG_FILE_NAME, extraConfig);

    if (ReadBluetoothAddress(config_path, BT_ADDR_FILE_NAME, &bt_addr))
    {
        extraConfig->valid_addr = true;

        memcpy(&extraConfig->bt_addr, &bt_addr, sizeof(BT_ADDR_T));
    }

    DumpExtraConfig(extraConfig);
}

static void QCA_InitBtsnoop(QcaInstance *inst)
{
    BtBootstrapInstance *mainInst = GetBtBootstrapInstance();
    ConnxLogSetting *logSetting = &mainInst->logSetting;
    bool enable_btsnoop = logSetting->enable_log && logSetting->enable_btsnoop;
    char *log_path = QCA_GET_LOG_PATH(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s BT snoop"), __FUNCTION__, enable_btsnoop ? "Enable" : "Disable"));

    QCA_GET_BTSNOOP_FILE_NAME(inst) = enable_btsnoop ? ConnxCreateFullFileName(log_path, BTSNOOP_LOG_FILE_NAME) : NULL;
}

static void QCA_InitInstance(QcaInstance *inst)
{
    BtBootstrapInstance *mainInst = GetBtBootstrapInstance();
    ConnxLogSetting *logSetting = &mainInst->logSetting;
    BtBootstrapRegisterInfo *ri = &mainInst->registerInfo;
    HciTransportSetting *transportSetting = QCA_GET_TRANSPORT_SETTING(inst);
    char *firmware_path;
    char *config_path;
    char *log_path;

    QCA_FUNC_ENTER();

    memset(inst, 0, sizeof(QcaInstance));

    transportSetting->device         = ConnxStrDup(BRI_GET_SERIAL_PORT(ri));
    transportSetting->init_baudrate  = BRI_GET_INIT_BAUDRATE(ri);
    transportSetting->reset_baudrate = BRI_GET_RESET_BAUDRATE(ri);
    transportSetting->flow_control   = BRI_GET_FLOW_CONTROL(ri);

    firmware_path = BRI_GET_FW_PATH(ri) ? BRI_GET_FW_PATH(ri) : DEFAULT_QCA_FIRMWARE_PATH;
    config_path   = BRI_GET_CONFIG_PATH(ri) ? BRI_GET_CONFIG_PATH(ri) : DEFAULT_QCA_CONFIG_PATH;
    log_path      = logSetting->log_path ? logSetting->log_path : DEFAULT_QCA_LOG_PATH;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> fw-path: %s, config-path: %s, log-path: %s"),
                   __FUNCTION__, firmware_path, config_path, log_path));

    QCA_GET_FIRMWARE_PATH(inst) = ConnxStrDup(firmware_path);
    QCA_GET_CONFIG_PATH(inst)   = ConnxStrDup(config_path);
    QCA_GET_LOG_PATH(inst)      = ConnxStrDup(log_path);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Init extra config"), __FUNCTION__));

    QCA_InitExtraConfig(config_path, QCA_GET_EXTRA_CONFIG(inst));

    QCA_GET_EVENT_HANDLE(inst) = ConnxEventCreate();

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> event handle: %p"), __FUNCTION__, QCA_GET_EVENT_HANDLE(inst)));

    QCA_InitBtsnoop(inst);

    QCA_FUNC_LEAVE();
}

static void QCA_DeinitInstance(QcaInstance *inst)
{
    HciTransportSetting *transportSetting;

    QCA_FUNC_ENTER();

    if (!inst)
        return;

    if (inst->btsnoop_file_name)
    {
        free(inst->btsnoop_file_name);
        inst->btsnoop_file_name = NULL;
    }

    transportSetting = &inst->transportSetting;

    if (transportSetting->device)
    {
        free(transportSetting->device);
        transportSetting->device = NULL;
    }

    if (inst->firmware_path)
    {
        free(inst->firmware_path);
        inst->firmware_path = NULL;
    }

    if (inst->config_path)
    {
        free(inst->config_path);
        inst->config_path = NULL;
    }

    if (inst->ram_patch_file_name)
    {
        free(inst->ram_patch_file_name);
        inst->ram_patch_file_name = NULL;
    }

    if (inst->nvm_file_name)
    {
        free(inst->nvm_file_name);
        inst->nvm_file_name = NULL;
    }

    if (inst->log_path)
    {
        free(inst->log_path);
        inst->log_path = NULL;
    }

    if (inst->eventHandle)
    {
        ConnxEventDestroy(inst->eventHandle);
        inst->eventHandle = NULL;
    }

    QCA_FUNC_LEAVE();
}

static bool QCA_OpenH4Transport(QcaInstance *inst)
{
    H4RegisterInfo ri;
    H4RegisterInfo *registerInfo = &ri;
    bool res;
    IFLOG(ConnxTime startTime = ConnxGetTickCount());

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    memset(registerInfo, 0, sizeof(H4RegisterInfo));

    registerInfo->size = sizeof(H4RegisterInfo);

    memcpy(&registerInfo->transportSetting, QCA_GET_TRANSPORT_SETTING(inst), sizeof(HciTransportSetting));

    registerInfo->rxDataFn = HandleH4DataReceived;

    registerInfo->btsnoop_file_name = QCA_GET_BTSNOOP_FILE_NAME(inst);

    res = H4_OpenConnection(registerInfo);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s, time elapsed: %dms"), __FUNCTION__,
                   res ? "done" : "fail", ConnxGetTimeElapsed(startTime)));

    QCA_SetBootStage(inst, res ? QCA_BOOT_STAGE_H4_TRANSPORT_OPEN : QCA_BOOT_STAGE_NULL);

    return res;
}

static void QCA_CloseH4Transport(QcaInstance *inst)
{
    IFLOG(ConnxTime startTime = ConnxGetTickCount());

    CONNX_UNUSED(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    H4_CloseConnection();

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> done, time elapsed: %dms"),
                   __FUNCTION__, ConnxGetTimeElapsed(startTime)));

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_NULL);
}

static ConnxResult QCA_SetBaudrate(QcaInstance *inst)
{
    HciTransportSetting *transportSetting = QCA_GET_TRANSPORT_SETTING(inst);
    uint8_t qca_baudrate = Map2QcaBaudrate(transportSetting->reset_baudrate);
    uint16_t delay = DEFAULT_SET_BAUDRATE_DELAY;     /* ms */

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> qca_baudrate: 0x%02x, reset_baudrate: %d"),
                   __FUNCTION__, qca_baudrate, transportSetting->reset_baudrate));

    if (!H4_SendVendorCommand(HCI_VS_SET_BAUDRATE, 1, &qca_baudrate))
    {
        return CONNX_RESULT_FAIL;
    }

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_SET_BAUDRATE);

    /* After HCI vendor cmd is issued to set new baudrate, HCI vendor event and
       command complete event will be received in new baudrate instead of initial
       baudrate. That is, it's not necessary to wait event here. However, it's
       better to add some delay to guarantee that HCI vendor command is sent out. */

    ConnxThreadSleep(delay);

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_IssueEdlCmd(QcaInstance *inst, QcaEdlCmdT edl_cmd, uint8_t *payload, uint8_t payload_length)
{
    QCA_EDL_CMD_GENERIC_T edl_packet;
    uint8_t actual_length = CONNX_MIN(payload_length, MAX_TLV_PACKET_SIZE);
    uint8_t total_length = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> edl_cmd: 0x%02x, payload_length: %d"), __FUNCTION__, edl_cmd, payload_length));

    edl_packet.edl_cmd = edl_cmd;

    if (payload && payload_length)
    {
        edl_packet.payload_length = actual_length;

        /* payload_length should be less than "MAX_TLV_PACKET_SIZE". */
        memcpy(edl_packet.payload, payload, actual_length);

        total_length = sizeof(QCA_EDL_CMD_HEADER_T) + actual_length;
    }
    else
    {
        total_length = sizeof(QcaEdlCmdT);
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Send vendor cmd, total_length: %d"), __FUNCTION__, total_length));

    if (!H4_SendVendorCommand(HCI_VS_DOWNLOAD_COMMAND, total_length, &edl_packet))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send vendor cmd, edl_cmd: 0x%02x"), __FUNCTION__, edl_cmd));

        return CONNX_RESULT_FAIL;
    }

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_GetChipVersion(QcaInstance *inst)
{
    ConnxResult result = 0;
    uint16_t timeout = DEFAULT_WAIT_TIMEOUT;
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_GET_CHIP_VER_CFM, CONNX_RESULT_SUCCESS);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue EDL cmd to get chip version"), __FUNCTION__));

    result = QCA_IssueEdlCmd(inst, EDL_CMD_GET_CHIP_VERSION, NULL, 0);
    VERIFY(result, "Issue EDL cmd to get chip version");

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_GET_CHIP_VERSION);

    result = QCA_WaitEvent(inst, timeout, eventBits);

    QCA_OUTPUT_RESULT(result);

    return result;
}

static ConnxResult QCA_DownloadAllFirmware(QcaInstance *inst)
{
    ConnxResult result = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Download RAM patch"), __FUNCTION__));

    result = QCA_DownloadRamPatch(inst);
    VERIFY(result, "Download RAM patch");

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Download NVM"), __FUNCTION__));

    result = QCA_DownloadNvm(inst);
    VERIFY(result, "Download NVM");

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_DownloadRamPatch(QcaInstance *inst)
{
    return QCA_DownloadFirmware(inst, QCA_FW_TYPE_RAM_PATCH);
}

static ConnxResult QCA_DownloadNvm(QcaInstance *inst)
{
    return QCA_DownloadFirmware(inst, QCA_FW_TYPE_NVM);
}

static ConnxResult QCA_DownloadFirmware(QcaInstance *inst, uint8_t fw_type)
{
    ConnxResult result = 0;
    uint8_t *buffer = NULL;
    uint32_t buffer_size = 0;
    uint8_t payload_length = 0;
    int size_remained = 0;
    uint8_t *ptr = NULL;
    uint16_t timeout = DEFAULT_WAIT_TIMEOUT;
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_DOWNLOAD_FW_CFM, CONNX_RESULT_SUCCESS);
    bool wait_vs_evt, wait_cc_evt;
    bool wait = false;
    IFLOG(ConnxTime startTime = ConnxGetTickCount());

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> fw_type: %d"), __FUNCTION__, fw_type));

    /* Read QCA firmware into buffer. */
    result = QCA_ReadFirmwareFile(inst, fw_type, &buffer, &buffer_size);
    if (!IS_CONNX_RESULT_SUCCESS(result))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Read QCA firmware file fail"), __FUNCTION__));
        exit(EXIT_FAILURE);
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Orig firmware size: %d (0x%06x)"), __FUNCTION__, buffer_size, buffer_size));

    IFLOG(DumpBuff(DEBUG_OUTPUT, buffer, buffer_size));

    if (fw_type == QCA_FW_TYPE_RAM_PATCH)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Retrieve download config for RAM patch"), __FUNCTION__));

        /* Retrieve download configuration from RAM patch.
           This is only valid for RAM patch downloading. */
        result = QCA_RetrieveDownloadCfg(inst, buffer, buffer_size);
        VERIFY(result, "Retrieve download configuration for RAM patch");

        QCA_SetBootStage(inst, QCA_BOOT_STAGE_DOWNLOAD_RAM_PATCH);
    }
    else if (fw_type == QCA_FW_TYPE_NVM)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Update NVM"), __FUNCTION__));

        /* Update NVM according to the extra config (such as PCM role). */
        result = QCA_UpdateFirmware(inst, buffer, buffer_size);
        VERIFY(result, "Update NVM");

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Reset download config for NVM"), __FUNCTION__));

        /* Reset firmware downloading configuration for NVM. That is, NVM downloading
           requires to wait for both Vendor Status Event and Command Complete Event. */
        QCA_StoreFwDownloadCfg(inst, true, true);

        QCA_SetBootStage(inst, QCA_BOOT_STAGE_DOWNLOAD_NVM);
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid fw_type: %d"), __FUNCTION__, fw_type));

        return CONNX_RESULT_INVALID_PARAMETER;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> New firmware size: %d (0x%06x)"), __FUNCTION__, buffer_size, buffer_size));

    IFLOG(DumpBuff(DEBUG_OUTPUT, buffer, buffer_size));

    ptr = buffer;
    size_remained = (int)buffer_size;

    do
    {
        payload_length = CONNX_MIN(MAX_TLV_PACKET_SIZE, size_remained);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue EDL cmd to download firmware, payload_length: %d, old size_remained: %d"),
                       __FUNCTION__, payload_length, size_remained));

        result = QCA_IssueEdlCmd(inst, EDL_CMD_DOWNLOAD_FIRMWARE, ptr, payload_length);
        CHK(result, "Download firmware");

        ptr += payload_length;
        size_remained -= payload_length;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> New size_remained: %d"), __FUNCTION__, size_remained));

        wait_vs_evt = QCA_GET_WAIT_VS_EVT(inst);
        wait_cc_evt = QCA_GET_WAIT_CC_EVT(inst);

        if (fw_type == QCA_FW_TYPE_RAM_PATCH)
        {
            if (wait_vs_evt && wait_cc_evt)
            {
                wait = true;
            }
            else if (!wait_vs_evt && !wait_cc_evt)
            {
                /* Only wait last HCI Vendor Status Event & Command Complete Event. */
                wait = (size_remained <= 0) ? true : false;
            }
        }
        else if (fw_type == QCA_FW_TYPE_NVM)
        {
            wait = true;
        }

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> fw_type: %d, wait: %x"), __FUNCTION__, fw_type, wait));

        if (wait)
        {
            result = QCA_WaitEvent(inst, timeout, eventBits);
            CHK(result, "Wait firmware download cfm");
        }
    }
    while (size_remained > 0);

    if (buffer)
    {
        free(buffer);
        buffer = NULL;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s to download %s, time elapsed: %dms"),
                   __FUNCTION__, IS_CONNX_RESULT_SUCCESS(result) ? "Succeed" : "Fail",
                   (fw_type == QCA_FW_TYPE_RAM_PATCH) ? "RAM patch" : "NVM",
                   ConnxGetTimeElapsed(startTime)));

    return result;
}

static ConnxResult QCA_RetrieveDownloadCfg(QcaInstance *inst, uint8_t *buffer, uint32_t buffer_size)
{
    QCA_FW_HEADER_T *fw_header = (QCA_FW_HEADER_T *)buffer;
    QCA_RAM_PATCH_HEADER_T ram_patch_header;
    QCA_RAM_PATCH_HEADER_T *ram_patch = &ram_patch_header;
    uint32_t total_fw_size;
    uint8_t *ptr = buffer;
    uint32_t offset = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> buffer_size: %d (0x%06x)"), __FUNCTION__, buffer_size, buffer_size));

    if (!buffer || !buffer_size)
    {
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    total_fw_size = CalculateFirmwareSize(fw_header);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> total_fw_size: %d (0x%06x)"), __FUNCTION__, total_fw_size, total_fw_size));

    /* Verify firmware header to only update NVM according to the extra config, such as PCM role. */
    if ((fw_header->fw_type != QCA_FW_TYPE_RAM_PATCH) ||
        (buffer_size < total_fw_size))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid fw_type: %d, buffer_size: %d, total_fw_size: %d"),
                       __FUNCTION__, fw_header->fw_type, buffer_size, total_fw_size));

        return CONNX_RESULT_INVALID_FIRMWARE;
    }

    offset = sizeof(QCA_FW_HEADER_T);
    ptr += offset;

    CONNX_CONVERT_32_FROM_XAP(ram_patch->total_length, ptr);
    CONNX_CONVERT_32_FROM_XAP(ram_patch->patch_data_length, ptr);
    CONNX_CONVERT_8_FROM_XAP(ram_patch->signing_format_version, ptr);
    CONNX_CONVERT_8_FROM_XAP(ram_patch->signature_algorithm, ptr);
    CONNX_CONVERT_8_FROM_XAP(ram_patch->download_config, ptr);
    CONNX_CONVERT_8_FROM_XAP(ram_patch->reserved1, ptr);
    CONNX_CONVERT_16_FROM_XAP(ram_patch->product_id, ptr);
    CONNX_CONVERT_16_FROM_XAP(ram_patch->rom_build_version, ptr);
    CONNX_CONVERT_16_FROM_XAP(ram_patch->patch_version, ptr);
    CONNX_CONVERT_16_FROM_XAP(ram_patch->reserved2, ptr);
    CONNX_CONVERT_32_FROM_XAP(ram_patch->patch_entry_address, ptr);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> download_config: 0x%02x"), __FUNCTION__, ram_patch->download_config));

    if (ram_patch->download_config == FW_DOWNLOAD_CFG_SKIP_VS_CC_EVT)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Clear fw download config"), __FUNCTION__));

        QCA_StoreFwDownloadCfg(inst, false, false);
    }
    else if (ram_patch->download_config == FW_DOWNLOAD_CFG_SKIP_NONE_EVT)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set fw download config"), __FUNCTION__));

        QCA_StoreFwDownloadCfg(inst, true, true);
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid download_config: 0x%02x"), __FUNCTION__, ram_patch->download_config));

        return CONNX_RESULT_INVALID_FIRMWARE;
    }

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_UpdateFirmware(QcaInstance *inst, uint8_t *buffer, uint32_t buffer_size)
{
    QCA_FW_HEADER_T *fw_header = (QCA_FW_HEADER_T *)buffer;
    QCA_NVM_HEADER_T nvm_header;
    QCA_NVM_HEADER_T *tmp_nvm_header = &nvm_header;
    QCA_NVM_HEADER_T *nvm = NULL;
    uint32_t total_fw_size;
    uint8_t *ptr = buffer;
    uint32_t offset = 0;
    uint16_t tag_number;
    uint16_t tag_length;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> buffer_size: %d (0x%06x)"), __FUNCTION__, buffer_size, buffer_size));

    if (!buffer || !buffer_size)
    {
        return CONNX_RESULT_INVALID_PARAMETER;
    }

    total_fw_size = CalculateFirmwareSize(fw_header);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> total_fw_size: %d (0x%06x)"), __FUNCTION__, total_fw_size, total_fw_size));

    /* Verify firmware header to only update NVM according to the extra config, such as PCM role. */
    if ((fw_header->fw_type != QCA_FW_TYPE_NVM) ||
        (buffer_size < total_fw_size))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid fw_type: %d, buffer_size: %d, total_fw_size: %d"),
                       __FUNCTION__, fw_header->fw_type, buffer_size, total_fw_size));

        return CONNX_RESULT_INVALID_FIRMWARE;
    }

    offset = sizeof(QCA_FW_HEADER_T);
    ptr += offset;

    while (offset < total_fw_size)
    {
        nvm = (QCA_NVM_HEADER_T *)ptr;

        /* Init NVM header. */
        CONNX_CONVERT_16_FROM_XAP(tmp_nvm_header->tag_number, ptr);
        CONNX_CONVERT_16_FROM_XAP(tmp_nvm_header->tag_length, ptr);
        CONNX_CONVERT_32_FROM_XAP(tmp_nvm_header->tag_ptr, ptr);
        CONNX_CONVERT_32_FROM_XAP(tmp_nvm_header->tag_ex_flag, ptr);

        offset += sizeof(QCA_NVM_HEADER_T);

        /* Re-verify tag_length. */
        if (offset + tmp_nvm_header->tag_length > total_fw_size)
        {
            /* Invalid tag length or firmware size. */
            break;
        }

        tag_number = tmp_nvm_header->tag_number;
        tag_length = tmp_nvm_header->tag_length;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> tag_number: %d (0x%04x), tag_length: %d (0x%04x)"),
                       __FUNCTION__, tag_number, tag_number, tag_length, tag_length));

        if (tag_number == NVM_TAG_NUM_2)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> TAG_NUM_2 for BT address"), __FUNCTION__));

            QCA_UpdateNvmBtAddr(inst, nvm);
        }
        else if (tag_number == NVM_TAG_NUM_17)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> TAG_NUM_17 for HCI Transport Layer Parameters (IBS + Baudrate)"), __FUNCTION__));

            QCA_UpdateNvmHciParameters(inst, nvm);
        }
        else if (tag_number == NVM_TAG_NUM_27)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> TAG_NUM_27 Deep-Sleep"), __FUNCTION__));

            QCA_UpdateNvmDeepSleep(inst, nvm);
        }
        else if (tag_number == NVM_TAG_NUM_38)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> TAG_NUM_38 for Debug Control"), __FUNCTION__));
            QCA_UpdateNvmPcmLoopback(inst, nvm);
        }
        else if (tag_number == NVM_TAG_NUM_44)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> TAG_NUM_44 for Codec Configuration"), __FUNCTION__));

            QCA_UpdateNvmCodecConfiguration(inst, nvm);
        }

        offset += tmp_nvm_header->tag_length;
        ptr += tmp_nvm_header->tag_length;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Done"), __FUNCTION__));

    return CONNX_RESULT_SUCCESS;
}

static void QCA_UpdateNvmBtAddr(QcaInstance *inst, QCA_NVM_HEADER_T *nvm)
{
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *ptr = (uint8_t *)nvm;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_addr: %x"), __FUNCTION__, extraConfig->valid_addr));

    if (!nvm || (nvm->tag_length != NVM_TAG_NUM_2_LENGTH))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid TAG_NUM_2 length: %d, expected: %d"),
                       __FUNCTION__, (nvm != NULL) ? nvm->tag_length : 0, NVM_TAG_NUM_2_LENGTH));

        return;
    }

    ptr += sizeof(QCA_NVM_HEADER_T);

    if (extraConfig->valid_addr)
    {
        BT_ADDR_T *bt_addr = &extraConfig->bt_addr;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set BT addr %04x:%02x:%06x"),
                       __FUNCTION__, bt_addr->nap, bt_addr->uap, bt_addr->lap));

        CONNX_ADD_UINT24_TO_XAP(ptr, bt_addr->lap);
        CONNX_ADD_UINT8_TO_XAP(ptr, bt_addr->uap);
        CONNX_ADD_UINT16_TO_XAP(ptr, bt_addr->nap);
    }
}

static void QCA_UpdateNvmHciParameters(QcaInstance *inst, QCA_NVM_HEADER_T *nvm)
{
    HciTransportSetting *transportSetting = QCA_GET_TRANSPORT_SETTING(inst);
    uint8_t qca_baudrate = Map2QcaBaudrate(transportSetting->reset_baudrate);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *ptr = (uint8_t *)nvm;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_ibs: %x"), __FUNCTION__, extraConfig->valid_ibs));

    if (!nvm || (nvm->tag_length != NVM_TAG_NUM_17_LENGTH))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid TAG_NUM_17 length: %d, expected: %d"),
                       __FUNCTION__, (nvm != NULL) ? nvm->tag_length : 0, NVM_TAG_NUM_17_LENGTH));

        return;
    }

    ptr += sizeof(QCA_NVM_HEADER_T);

    if (extraConfig->valid_ibs)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s SW IBS in NVM"), __FUNCTION__,
                       extraConfig->enable_ibs ? "Enable" : "Disable"));

        if (extraConfig->enable_ibs)
        {
            ptr[FW_CONF_SW_IBS_BYTE] |= (0x1 << FW_CONF_SW_IBS_BIT);
        }
        else
        {
            ptr[FW_CONF_SW_IBS_BYTE] &= ~(0x1 << FW_CONF_SW_IBS_BIT);
        }
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> reset_baudrate: %d, new qca_baudrate: 0x%02x"),
                   __FUNCTION__, transportSetting->reset_baudrate, qca_baudrate));

    ptr[FW_CONF_UART_BAUDRATE_BYTE] = qca_baudrate;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> HCI transport options: 0x%02x"),
                   __FUNCTION__, ptr[FW_CONF_TRANSFER_OPTIONS_BYTE]));
}

static void QCA_UpdateNvmDeepSleep(QcaInstance *inst, QCA_NVM_HEADER_T *nvm)
{
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *ptr = (uint8_t *)nvm;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_deep_sleep: %x"), __FUNCTION__, extraConfig->valid_deep_sleep));

    if (!nvm || (nvm->tag_length != NVM_TAG_NUM_27_LENGTH))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid TAG_NUM_27 length: %d, expected: %d"),
                       __FUNCTION__, (nvm != NULL) ? nvm->tag_length : 0, NVM_TAG_NUM_27_LENGTH));

        return;
    }

    ptr += sizeof(QCA_NVM_HEADER_T);

    if (extraConfig->valid_deep_sleep)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s deep sleep in NVM"), __FUNCTION__,
                       extraConfig->enable_deep_sleep ? "Enable" : "Disable"));

        if (extraConfig->enable_deep_sleep)
        {
            ptr[FW_CONF_DEEP_SLEEP_BYTE] |= (0x1 << FW_CONF_DEEP_SLEEP_BIT);
        }
        else
        {
            ptr[FW_CONF_DEEP_SLEEP_BYTE] &= ~(0x1 << FW_CONF_DEEP_SLEEP_BIT);
        }
    }
}

static void QCA_UpdateNvmPcmLoopback(QcaInstance *inst, QCA_NVM_HEADER_T *nvm)
{
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *ptr = (uint8_t *)nvm;
    
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_pcm_lp: %x"), __FUNCTION__, extraConfig->valid_pcm_lp));

    if (!nvm || (nvm->tag_length != NVM_TAG_NUM_38_LENGTH))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid TAG_NUM_38 length: %d, expected: %d"),
                       __FUNCTION__, (nvm != NULL) ? nvm->tag_length : 0, NVM_TAG_NUM_38_LENGTH));

        return;
    }

    ptr += sizeof(QCA_NVM_HEADER_T);
    
    if (extraConfig->valid_pcm_lp)
    {
    	IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s pcm loopback in NVM"), __FUNCTION__,
    				   extraConfig->enable_pcm_lp ? "Enable" : "Disable"));
    
    	if (extraConfig->enable_pcm_lp)
    	{
    		ptr[FW_CONF_PCM_LOOPBACK_MODE_BYTE] |= 0x01;
    	}
    	else
    	{
    		ptr[FW_CONF_PCM_LOOPBACK_MODE_BYTE] &= ~(0x01);
    	}
    }
}

static void QCA_UpdateNvmCodecConfiguration(QcaInstance *inst, QCA_NVM_HEADER_T *nvm)
{
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *ptr = (uint8_t *)nvm;
    uint16_t tmp_val = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_pcm: %x"), __FUNCTION__, extraConfig->valid_pcm));
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_invert_bclk: %x"), __FUNCTION__, extraConfig->valid_invert_bclk));

    if (!nvm || (nvm->tag_length != NVM_TAG_NUM_44_LENGTH))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid TAG_NUM_44 length: %d, expected: %d"),
                       __FUNCTION__, (nvm != NULL) ? nvm->tag_length : 0, NVM_TAG_NUM_44_LENGTH));

        return;
    }

    ptr += sizeof(QCA_NVM_HEADER_T);

    if (extraConfig->valid_pcm)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set PCM %s"), __FUNCTION__,
                       (extraConfig->pcm_role == FW_CONF_PCM_SLAVE) ? "slave" : "master"));

        if (extraConfig->pcm_role == FW_CONF_PCM_SLAVE)
        {
            ptr[FW_CONF_NBS_CONTROL_BYTE] |= (0x1 << FW_CONF_NBS_PCM_ROLE_BIT);
            ptr[FW_CONF_WBS_CONTROL_BYTE] |= (0x1 << FW_CONF_WBS_PCM_ROLE_BIT);
        }
        else if (extraConfig->pcm_role == FW_CONF_PCM_MASTER)
        {
            ptr[FW_CONF_NBS_CONTROL_BYTE] &= ~(0x1 << FW_CONF_NBS_PCM_ROLE_BIT);
            ptr[FW_CONF_WBS_CONTROL_BYTE] &= ~(0x1 << FW_CONF_WBS_PCM_ROLE_BIT);
        }
    }

    if (extraConfig->valid_invert_bclk)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set invert BCLK: %x"), __FUNCTION__, extraConfig->invert_bclk));

        if (extraConfig->invert_bclk)
        {
            ptr[FW_CONF_NBS_CONTROL_BYTE] |= (0x1 << FW_CONF_NBS_INVERT_BLCK_BIT);
            ptr[FW_CONF_WBS_CONTROL_BYTE] |= (0x1 << FW_CONF_WBS_INVERT_BLCK_BIT);
        }
        else
        {
            ptr[FW_CONF_NBS_CONTROL_BYTE] &= ~(0x1 << FW_CONF_NBS_INVERT_BLCK_BIT);
            ptr[FW_CONF_WBS_CONTROL_BYTE] &= ~(0x1 << FW_CONF_WBS_INVERT_BLCK_BIT);
        }
    }

    if (extraConfig->valid_nbs_bclk)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set NBS BCLK: %d"), __FUNCTION__, extraConfig->nbs_bclk));

        tmp_val = MapBitClock2Divisor(extraConfig->nbs_bclk);

        ptr[FW_CONF_NBS_PCM_CLOCK_DIVISOR_BYTE] = (uint8_t)(tmp_val & 0xff);
        ptr[FW_CONF_NBS_PCM_CLOCK_DIVISOR_BYTE + 1] = (uint8_t)((tmp_val >> 8) & 0xff);
    }

    if (extraConfig->valid_wbs_bclk)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set WBS BCLK: %d"), __FUNCTION__, extraConfig->wbs_bclk));

        tmp_val = MapBitClock2Divisor(extraConfig->wbs_bclk);

        ptr[FW_CONF_WBS_PCM_CLOCK_DIVISOR_BYTE] = (uint8_t)(tmp_val & 0xff);
        ptr[FW_CONF_WBS_PCM_CLOCK_DIVISOR_BYTE + 1] = (uint8_t)((tmp_val >> 8) & 0xff);
    }

    if (extraConfig->valid_nbs_sclk)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set NBS SCLK: %d"), __FUNCTION__, extraConfig->nbs_sclk));

        tmp_val = MapSyncClock2Divisor(extraConfig->nbs_sclk);

        ptr[FW_CONF_NBS_PCM_FRAME_DIVISOR_BYTE] = (uint8_t)(tmp_val & 0xff);
    }

    if (extraConfig->valid_wbs_sclk)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set WBS SCLK: %d"), __FUNCTION__, extraConfig->wbs_sclk));

        tmp_val = MapSyncClock2Divisor(extraConfig->wbs_sclk);

        ptr[FW_CONF_WBS_PCM_FRAME_DIVISIOR_BYTE] = (uint8_t)(tmp_val & 0xff);
    }
}

static ConnxResult QCA_EnableFwLog(QcaInstance *inst)
{
    ConnxResult result = 0;
    uint16_t timeout = DEFAULT_WAIT_TIMEOUT;
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_FW_LOG_CFM, CONNX_RESULT_SUCCESS);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    QCA_EDL_CMD_COREDUMP_T edl_cmd_fw_log;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_fw_log: %x"), __FUNCTION__, extraConfig->valid_fw_log));

    if (!extraConfig->valid_fw_log)
    {
        /* Still consider success, if it's not necessary to enable/disable core dump in QCA chip. */
        return CONNX_RESULT_SUCCESS;
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s fw log"), __FUNCTION__,
                   extraConfig->enable_fw_log ? "Enable" : "Disable"));

    edl_cmd_fw_log.edl_cmd = EDL_CMD_CORE_DUMP;
    edl_cmd_fw_log.enable  = extraConfig->enable_fw_log ? 1 : 0;

    if (!H4_SendVendorCommand(HCI_VS_HOST_LOG_OPCODE,
                              sizeof(QCA_EDL_CMD_COREDUMP_T),
                              &edl_cmd_fw_log))
    {
        return CONNX_RESULT_FAIL;
    }

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_SEND_FW_LOG_CMD);

    result = QCA_WaitEvent(inst, timeout, eventBits);

    QCA_OUTPUT_RESULT(result);

    return result;
}

static ConnxResult QCA_IssueNvmCmd(QcaInstance *inst, QcaNvmCmdT sub_opcode, uint8_t nvm_tag,
                                   uint8_t *tag_value, uint8_t tag_value_length)
{
    QCA_NVM_WRITE_CMD_T nvm_write_cmd;
    uint8_t total_length = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> sub_opcode: %02x, nvm_tag: 0x%02x, tag_value_length: %d"),
                   __FUNCTION__, sub_opcode, nvm_tag, tag_value_length));

    nvm_write_cmd.sub_opcode = sub_opcode;
    nvm_write_cmd.nvm_tag    = nvm_tag;

    if (tag_value && tag_value_length)
    {
        nvm_write_cmd.tag_value_length = CONNX_MIN(tag_value_length, MAX_NVM_TAG_VALUE_LENGTH);

        memcpy(nvm_write_cmd.tag_value, tag_value, nvm_write_cmd.tag_value_length);

        total_length = sizeof(QCA_NVM_WRITE_CMD_HEADER_T) + nvm_write_cmd.tag_value_length;
    }
    else
    {
        nvm_write_cmd.tag_value_length = 0;

        total_length = sizeof(QCA_NVM_WRITE_CMD_HEADER_T);
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Send vendor cmd, total_length: %d"), __FUNCTION__, total_length));

    if (!H4_SendVendorCommand(HCI_VS_NVM_COMMAND, total_length, &nvm_write_cmd))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send vendor cmd, nvm_cmd: 0x%02x, nvm_tag: 0x%02x"),
                        __FUNCTION__, HCI_VS_NVM_COMMAND, nvm_tag));

        return CONNX_RESULT_FAIL;
    }

    return CONNX_RESULT_SUCCESS;
}

static ConnxResult QCA_WriteNvm(QcaInstance *inst, uint8_t nvm_tag, uint8_t *tag_value,
                                uint8_t tag_value_length, uint16_t boot_stage, uint32_t eventBits)
{
    ConnxResult result = 0;
    uint16_t timeout = DEFAULT_WAIT_TIMEOUT;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    result = QCA_IssueNvmCmd(inst, NVM_CMD_WRITE, nvm_tag, tag_value, tag_value_length);
    VERIFY(result, "Issue NVM write cmd");

    QCA_SetBootStage(inst, boot_stage);

    result = QCA_WaitEvent(inst, timeout, eventBits);

    QCA_OUTPUT_RESULT(result);

    return result;    
}

static ConnxResult QCA_SetPowerConfiguration(QcaInstance *inst)
{
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_SET_POWER_CFG_CFM, CONNX_RESULT_SUCCESS);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *power_cfg = extraConfig->power_cfg;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_power_cfg: %x"), __FUNCTION__, extraConfig->valid_power_cfg));

    if (!extraConfig->valid_power_cfg)
    {
        /* Still consider success. */
        return CONNX_RESULT_SUCCESS;
    }

    IFLOG(DumpBuff(DEBUG_OUTPUT, power_cfg, NVM_TAG_NUM_36_LENGTH));

    return QCA_WriteNvm(inst, NVM_TAG_NUM_36, power_cfg, NVM_TAG_NUM_36_LENGTH, QCA_BOOT_STAGE_SET_POWER_CFG, eventBits);
}

static ConnxResult QCA_SetLETxPowerControl(QcaInstance *inst)
{
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_SET_LE_TX_POWER_CTRL_CFM, CONNX_RESULT_SUCCESS);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *le_tx_power_ctrl = extraConfig->le_tx_power_ctrl;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_le_tx_power_ctrl: %x"), __FUNCTION__, extraConfig->valid_le_tx_power_ctrl));

    if (!extraConfig->valid_le_tx_power_ctrl)
    {
        /* Still consider success. */
        return CONNX_RESULT_SUCCESS;
    }

    IFLOG(DumpBuff(DEBUG_OUTPUT, le_tx_power_ctrl, NVM_TAG_NUM_83_LENGTH));

    return QCA_WriteNvm(inst, NVM_TAG_NUM_83, le_tx_power_ctrl, NVM_TAG_NUM_83_LENGTH, QCA_BOOT_STAGE_SET_LE_TX_POWER_CTRL, eventBits);
}

static ConnxResult QCA_SetXtalFineTuning(QcaInstance *inst)
{
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_SET_XTAL_FINE_TUNING_CFM, CONNX_RESULT_SUCCESS);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *xtal_fine_tuning = extraConfig->xtal_fine_tuning;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_xtal_fine_tuning: %x"), __FUNCTION__, extraConfig->valid_xtal_fine_tuning));

    if (!extraConfig->valid_xtal_fine_tuning)
    {
        /* Still consider success. */
        return CONNX_RESULT_SUCCESS;
    }

    IFLOG(DumpBuff(DEBUG_OUTPUT, xtal_fine_tuning, NVM_TAG_NUM_28_LENGTH));

    return QCA_WriteNvm(inst, NVM_TAG_NUM_28, xtal_fine_tuning, NVM_TAG_NUM_28_LENGTH, QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING, eventBits);
}

static ConnxResult QCA_SetXtalFineTuning2(QcaInstance *inst)
{
    uint32_t eventBits = QCA_EVENT_BITS(EVENT_BITS_SET_XTAL_FINE_TUNING2_CFM, CONNX_RESULT_SUCCESS);
    QcaExtraConfig *extraConfig = QCA_GET_EXTRA_CONFIG(inst);
    uint8_t *xtal_fine_tuning2 = extraConfig->xtal_fine_tuning2;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> valid_xtal_fine_tuning2: %x"), __FUNCTION__, extraConfig->valid_xtal_fine_tuning2));

    if (!extraConfig->valid_xtal_fine_tuning2)
    {
        /* Still consider success. */
        return CONNX_RESULT_SUCCESS;
    }

    IFLOG(DumpBuff(DEBUG_OUTPUT, xtal_fine_tuning2, NVM_TAG_NUM_82_LENGTH));

    return QCA_WriteNvm(inst, NVM_TAG_NUM_82, xtal_fine_tuning2, NVM_TAG_NUM_82_LENGTH, QCA_BOOT_STAGE_SET_XTAL_FINE_TUNING2, eventBits);
}

static void QCA_IssueOptionalCommand(QcaInstance *inst)
{
    /* [Optional] Issue HCI NVM write cmd to set power configuration. */
    QCA_SetPowerConfiguration(inst);

    /* [Optional] Issue HCI NVM write cmd to set le tx power control. */
    QCA_SetLETxPowerControl(inst);

    /* [Optional] Issue HCI NVM write cmd to set xtal fine tuning for v3.1. */
    QCA_SetXtalFineTuning(inst);

    /* [Optional] Issue HCI NVM write cmd to set xtal fine tuning for v3.2. */
    QCA_SetXtalFineTuning2(inst);
}

static ConnxResult QCA_IssueHciResetCmd(QcaInstance *inst)
{
    uint16_t timeout = DEFAULT_WAIT_TIMEOUT;
    ConnxResult result = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!H4_SendResetCommand())
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to send HCI_RESET"), __FUNCTION__));

        return CONNX_RESULT_FAIL;
    }

    QCA_SetBootStage(inst, QCA_BOOT_STAGE_HCI_RESET);

    result = QCA_WaitEvent(inst, timeout, EVENT_BITS_HCI_RESET_CFM);

    QCA_OUTPUT_RESULT(result);

    return result;
}

static bool QCA_ParseEvent(QcaInstance *inst, HCI_EV_GENERIC_T *hci_event)
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

    QCA_StoreEventCode(inst, ev);

    if (ev == HCI_EV_VENDOR_SPECIFIC)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse HCI vendor event"), __FUNCTION__));

        /* Parse HCI vendor event and store internally. */
        res = QCA_ParseVendorEvent(inst, len, hci_event->data);
    }
    else if (ev == HCI_EV_COMMAND_COMPLETE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse HCI command complete event"), __FUNCTION__));

        /* Parse HCI event of command complete and store internally. */
        res = QCA_ParseCommandCompleteEvent(inst, len, hci_event->data);
    }
    else
    {
        /* Unknown HCI event. */
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown event_code: 0x%02x"), __FUNCTION__, ev));
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, res ? "Done" : "Fail"));

    return res;
}

static bool QCA_ParseCommandCompleteEvent(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_event = QCA_GET_COMMAND_COMPLETE_EVT(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    hci_event->event_code = HCI_EV_COMMAND_COMPLETE;
    hci_event->length     = length;

    CONNX_CONVERT_8_FROM_XAP(hci_event->num_hci_command_pkts, data);
    CONNX_CONVERT_16_FROM_XAP(hci_event->op_code, data);
    CONNX_CONVERT_8_FROM_XAP(hci_event->status, data);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> num_hci_command_pkts: %d, op_code: 0x%04x, status: 0x%02x"),
                   __FUNCTION__, hci_event->num_hci_command_pkts, hci_event->op_code, hci_event->status));

    if (hci_event->op_code == HCI_READ_BD_ADDR)
    {
        uint8_t *addr = QCA_GET_BD_ADDR(inst);

        /* Store BD_ADDR. */
        memcpy(addr, data, BT_ADDR_LENGTH);
    }

    return true;
}

static bool QCA_ParseVendorEvent(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QcaEdlEvtT edl_evt;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> length: 0x%02x"), __FUNCTION__, length));

    if (!data || !length)
        return false;

    edl_evt = data[EDL_EVT_OFFSET];

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> edl_evt: 0x%02x"), __FUNCTION__, edl_evt));

    if (edl_evt == EDL_EVT_REQUEST_RESULT)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> EDL_EVT_REQUEST_RESULT"), __FUNCTION__));

        if (QCA_IsFwLogCmdSent(inst))
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse EDL event for FW LOG cmd"), __FUNCTION__));

            /* Parse specifically for EDL event of core command. */
            res = QCA_ParseEdlEvtFwLog(inst, length, data);
        }
        else
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse EDL event for request result"), __FUNCTION__));

            /* Parse normal EDL event of request result. */
            res = QCA_ParseEdlEvtRequestResult(inst, length, data);
        }
    }
    else if (edl_evt == EDL_EVT_SET_BAUDRATE_RESP)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> EDL_EVT_SET_BAUDRATE_RESP"), __FUNCTION__));

        res = QCA_ParseEdlEvtSetBaudrateResp(inst, length, data);
    }
    else if (edl_evt == EDL_EVT_BT_CONTROLLER_LOG)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> EDL_EVT_BT_CONTROLLER_LOG"), __FUNCTION__));

        /* Ignore to parse QCA BT controller log in the bootstrap. Instead,
           btsnoop log can be generated to capture the core dump fully in
           BT stack run. And the specific tool can be used to extract QCA
           coredump log from btsnoop log. */
    }
    else if (edl_evt == EDL_EVT_NVM)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> EDL_EVT_NVM"), __FUNCTION__));

        res = QCA_ParseEdlEvtNvmResp(inst, length, data);
    }
    else
    {
        /* Unknown EDL event. */
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown edl_evt: 0x%02x"), __FUNCTION__, edl_evt));
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, res ? "Done" : "Fail"));

    return res;
}

static bool QCA_ParseEdlEvtSetBaudrateResp(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QCA_EDL_EVT_SET_BAUDRATE_RESP_T *mv;

    CONNX_UNUSED(length);

    QCA_FUNC_ENTER();

    mv = (QCA_EDL_EVT_SET_BAUDRATE_RESP_T *)malloc(sizeof(QCA_EDL_EVT_SET_BAUDRATE_RESP_T));

    if (!mv)
        return false;

    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt, data);
    CONNX_CONVERT_8_FROM_XAP(mv->result, data);

    QCA_StoreEventParsed(inst, mv);

    QCA_FUNC_LEAVE();

    return true;
}

static bool QCA_ParseEdlEvtRequestResult(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QcaEdlEvtResT edl_evt_res;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> length: 0x%02x"), __FUNCTION__, length));

    if (!data || (length <= 1))
        return false;

    edl_evt_res = data[EDL_EVT_RES_OFFSET];

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> edl_evt_res: 0x%02x"), __FUNCTION__, edl_evt_res));

    switch (edl_evt_res)
    {
        case EDL_EVT_RES_GET_APP_VER:
        case EDL_EVT_RES_GET_CHIP_VER:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse EDL event for getting chip version result"), __FUNCTION__));

            res = QCA_ParseEdlEvtGetChipVerResult(inst, length, data);
            break;
        }

        case EDL_EVT_RES_DOWNLOAD_FIRMWARE:
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Parse EDL event for downloading fw result"), __FUNCTION__));

            res = QCA_ParseEdlEvtDownloadFwResult(inst, length, data);
            break;
        }

        default:
        {
            /* Unknown EDL event result. */
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Unknown edl_evt_res: 0x%02x"), __FUNCTION__, edl_evt_res));

            break;
        }
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, res ? "Done" : "Fail"));

    return res;
}

static bool QCA_ParseEdlEvtGetChipVerResult(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QCA_EDL_EVT_GET_CHIP_VER_RES_T *mv;

    CONNX_UNUSED(length);

    QCA_FUNC_ENTER();

    mv = (QCA_EDL_EVT_GET_CHIP_VER_RES_T *)malloc(sizeof(QCA_EDL_EVT_GET_CHIP_VER_RES_T));

    if (!mv)
        return false;

    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt, data);
    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt_res, data);
    CONNX_CONVERT_32_FROM_XAP(mv->product_id, data);
    CONNX_CONVERT_16_FROM_XAP(mv->fw_patch_ver, data);
    CONNX_CONVERT_16_FROM_XAP(mv->rom_build_ver, data);
    CONNX_CONVERT_32_FROM_XAP(mv->soc_id, data);

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> soc_id: 0x%08x, rom_build_ver: 0x%04x, fw_patch_ver: 0x%04x, product_id: 0x%08x"),
                   __FUNCTION__, mv->soc_id, mv->rom_build_ver, mv->fw_patch_ver, mv->product_id));

    QCA_StoreEventParsed(inst, mv);

    /* Store chip version directly. */
    QCA_StoreChipVer(inst, mv);

    QCA_FUNC_LEAVE();

    return true;
}

static bool QCA_ParseEdlEvtDownloadFwResult(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QCA_EDL_EVT_DOWNLOAD_FW_RES_T *mv;

    CONNX_UNUSED(length);

    QCA_FUNC_ENTER();

    mv = (QCA_EDL_EVT_DOWNLOAD_FW_RES_T *)malloc(sizeof(QCA_EDL_EVT_DOWNLOAD_FW_RES_T));

    if (!mv)
        return false;

    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt, data);
    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt_res, data);
    CONNX_CONVERT_8_FROM_XAP(mv->result, data);

    /* Notify the result of QCA firmware download right here. The reason is that QCA chip may not respond
       with HCI Command Complete (HCI NOP) after HCI vendor event, when RAM patch is completed to download. */
    QCA_NotifyResult(inst, EVENT_BITS_DOWNLOAD_FW_CFM, IS_HCI_VS_CMD_RESULT_SUCCESS(mv->result));

    QCA_FUNC_LEAVE();

    return true;
}

static bool QCA_ParseEdlEvtFwLog(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QCA_EDL_EVT_COREDUMP_T *mv;

    CONNX_UNUSED(length);

    QCA_FUNC_ENTER();

    mv = (QCA_EDL_EVT_COREDUMP_T *)malloc(sizeof(QCA_EDL_EVT_COREDUMP_T));

    if (!mv)
        return false;

    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt, data);

    QCA_StoreEventParsed(inst, mv);

    QCA_FUNC_LEAVE();

    return true;
}

static bool QCA_ParseEdlEvtNvmResp(QcaInstance *inst, uint8_t length, uint8_t *data)
{
    QCA_EDL_EVT_NVM_T *mv;

    QCA_FUNC_ENTER();

    if (length < sizeof(QCA_EDL_EVT_NVM_HEADER_T))
    {
        /* Invalid length. */
        return false;
    }

    mv = (QCA_EDL_EVT_NVM_T *)malloc(sizeof(QCA_EDL_EVT_NVM_T));

    if (!mv)
        return false;

    CONNX_CONVERT_8_FROM_XAP(mv->edl_evt, data);
    CONNX_CONVERT_8_FROM_XAP(mv->sub_opcode, data);
    CONNX_CONVERT_8_FROM_XAP(mv->nvm_tag, data);
    CONNX_CONVERT_8_FROM_XAP(mv->tag_value_length, data);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> edl_evt: 0x%02x, sub_opcode: 0x%02x, nvm_tag: 0x%02x, tag_value_length: 0x%02x"),
                    __FUNCTION__, mv->edl_evt, mv->sub_opcode, mv->nvm_tag, mv->tag_value_length));

    if (length > sizeof(QCA_EDL_EVT_NVM_HEADER_T))
    {
        memcpy(mv->tag_value, data, CONNX_MIN(MAX_NVM_TAG_VALUE_LENGTH, length - sizeof(QCA_EDL_EVT_NVM_HEADER_T)));
    }

    QCA_StoreEventParsed(inst, mv);

    QCA_FUNC_LEAVE();

    return true;
}

static void QCA_HandleEvent(QcaInstance *inst)
{
    hci_event_code_t event_code = QCA_GET_EVENT_CODE(inst);

    if (event_code == HCI_EV_COMMAND_COMPLETE)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> HCI_EV_COMMAND_COMPLETE (0x%02x)"), __FUNCTION__, event_code));

        QCA_HandleEventCommandComplete(inst);
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> not handle event_code: 0x%02x"), __FUNCTION__, event_code));
    }
}

static void QCA_HandleEventCommandComplete(QcaInstance *inst)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_ev_command_complete = QCA_GET_COMMAND_COMPLETE_EVT(inst);
    hci_op_code_t op_code = hci_ev_command_complete->op_code;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> op_code: 0x%04x"), __FUNCTION__, op_code));

    if (op_code == HCI_NOP)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> HCI_NOP"), __FUNCTION__));

        QCA_HandleCommandCompleteNop(inst);
    }
    else if (op_code == HCI_RESET)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> HCI_RESET"), __FUNCTION__));

        QCA_HandleCommandCompleteReset(inst);
    }
    else if (op_code == HCI_READ_BD_ADDR)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> HCI_READ_BD_ADDR"), __FUNCTION__));

        QCA_HandleCommandCompleteReadBdAddr(inst);
    }
    else
    {
        /* Unknown HCI command op_code. */
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown HCI command op_code: 0x%04x"), __FUNCTION__, op_code));
    }
}

static void QCA_HandleCommandCompleteNop(QcaInstance *inst)
{
    void *event_parsed = QCA_GET_EVENT_PARSED(inst);
    uint16_t eventBits = 0;
    bool success = false;
    IFLOG(QcaBootStageT boot_stage = QCA_GET_BOOT_STAGE(inst));
    IFLOG(const char *boot_stage_str = GetBootStageString(boot_stage));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> event_parsed: %p, boot_stage: %d (%s)"),
                   __FUNCTION__, event_parsed, boot_stage, boot_stage_str));

    /* NO useful information in HCI command completed during booting QCA chip.
       Instead, it's necessary to retrieve HCI event parsed previously. */

    if (!event_parsed)
    {
        /* NO vendor event parsed. */
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> NO HCI vendor event parsed? "), __FUNCTION__));
        return;
    }

    if (QCA_MapEdlEvt2EventBits(inst, event_parsed, &eventBits, &success))
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Notify result, eventBits: 0x%04x, success: %x"),
                       __FUNCTION__, eventBits, success));

        /* Limit to only notify result when it's during the proper boot stage. For example,
           HCI vendor event for setting baudrate from H4 transport can be ignored during
           local baudrate in host is changing to high speed. The reason is that such HCI
           event may well be lost, dependent on when H4 transport is re-opened. */
        QCA_NotifyResult(inst, eventBits, success);
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> NO eventBits mapped "), __FUNCTION__));
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Free message parsed"), __FUNCTION__));

    QCA_FreeEventParsed(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Done"), __FUNCTION__));
}

static void QCA_HandleCommandCompleteReset(QcaInstance *inst)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_ev_command_complete = QCA_GET_COMMAND_COMPLETE_EVT(inst);
    hci_return_t status = hci_ev_command_complete->status;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> status: 0x%02x"), __FUNCTION__, status));

    QCA_NotifyResult(inst, EVENT_BITS_HCI_RESET_CFM, IS_HCI_SUCCESS(status));
}

static void QCA_HandleCommandCompleteReadBdAddr(QcaInstance *inst)
{
    HCI_EV_COMMAND_COMPLETE_T *hci_ev_command_complete = QCA_GET_COMMAND_COMPLETE_EVT(inst);
    hci_return_t status = hci_ev_command_complete->status;
    uint8_t *addr = QCA_GET_BD_ADDR(inst);
    BT_ADDR_T bt_addr;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> status: 0x%02x, addr { %02x:%02x:%02x:%02x:%02x:%02x }"),
                   __FUNCTION__, status, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]));

    CONNX_CONVERT_24_FROM_XAP(bt_addr.lap, addr);
    CONNX_CONVERT_8_FROM_XAP(bt_addr.uap, addr);
    CONNX_CONVERT_16_FROM_XAP(bt_addr.nap, addr);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> bt_addr %04x:%02x:%06x"),
                   __FUNCTION__, bt_addr.nap, bt_addr.uap, bt_addr.lap));

    QCA_NotifyResult(inst, EVENT_BITS_HCI_READ_BD_ADDR_CFM, IS_HCI_SUCCESS(status));
}

static uint32_t HandleH4DataReceived(const uint8_t *data, uint32_t dataLength)
{
    QcaInstance *inst = &qca_inst;
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

        IFLOG(DumpBuff(DEBUG_OUTPUT, (const uint8_t *)&hci_generic_event, HCI_EVENT_SIZE(&hci_generic_event)));

        /* 1 byte for H4 packet type. */
        ++size_parsed;

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> size_parsed: %d"), __FUNCTION__, size_parsed));

        if (QCA_ParseEvent(inst, &hci_generic_event))
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Handle event"), __FUNCTION__));

            QCA_HandleEvent(inst);
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

ConnxResult BootQca(void)
{
    ConnxResult result = 0;
    QcaInstance *inst = &qca_inst;
    uint16_t restart_uart_delay = DEFAULT_RESTART_UART_DELAY;
    IFLOG(ConnxTime startTime = ConnxGetTickCount());

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Init instance"), __FUNCTION__));

    QCA_InitInstance(inst);

    do
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Open H4 transport"), __FUNCTION__));

        if (!QCA_OpenH4Transport(inst))
        {
            break;
        }

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Set baudrate"), __FUNCTION__));

        /* 1. Issue HCI vendor cmd to set UART baudrate in QCA. */
        result = QCA_SetBaudrate(inst);
        CHK(result, "Set UART baudrate in QCA");

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Close H4 transport"), __FUNCTION__));

        /* 2. Restart UART in host to match with new baudrate set. */
        QCA_CloseH4Transport(inst);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Re-open H4 transport"), __FUNCTION__));

        if (!QCA_OpenH4Transport(inst))
        {
            break;
        }

        /* Add some delay to make sure UART is ready after baudrate is changed. */
        ConnxThreadSleep(restart_uart_delay);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Get QCA chip version"), __FUNCTION__));

        /* 3. Issue HCI vendor cmd to get QCA chip version. */
        result = QCA_GetChipVersion(inst);
        CHK(result, "Get QCA chip version");

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Get firmware file"), __FUNCTION__));

        /* 4. Search QCA firmware file. */
        result = QCA_GetFirmwareFile(inst);
        CHK(result, "Get QCA firmware file");

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Download all firmware"), __FUNCTION__));

        /* 5. Download all QCA firmware. */
        result = QCA_DownloadAllFirmware(inst);
        CHK(result, "Download all firmware");

        /* Optional */
        QCA_IssueOptionalCommand(inst);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Issue HCI RESET"), __FUNCTION__));

        /* 9. Issue HCI reset cmd to make firmware active. */
        result = QCA_IssueHciResetCmd(inst);
        CHK(result, "Issue HCI reset cmd");
    
        /* 10. Issue HCI vendor cmd to Enable(or Disable) FW Log. */
        result = QCA_EnableFwLog(inst);
        CHK(result, "Enable FW Log");    

    }
    while (0);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Close H4 transport"), __FUNCTION__));

    QCA_CloseH4Transport(inst);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Deinit instance"), __FUNCTION__));

    QCA_DeinitInstance(inst);

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> %s, total boot time: %dms"), __FUNCTION__,
                   IS_CONNX_RESULT_SUCCESS(result) ? "Succeed" : "Fail", ConnxGetTimeElapsed(startTime)));

    return result;
}
