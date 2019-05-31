/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #6 $
******************************************************************************/

#ifndef _BOOT_QCA_H_
#define _BOOT_QCA_H_

#include "bt_bootstrap_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_QCA_FIRMWARE_PATH       BT_FIRMWARE_PATH

/* QCA ROME 3.0 firmware */
#define ROME_RAMPATCH_TLV_3_0_0_FILE    "rampatch_tlv_3.0.tlv"
#define ROME_NVM_TLV_3_0_0_FILE         "nvm_tlv_3.0.bin"

/* QCA ROME 3.2 firmware */
#define ROME_RAMPATCH_TLV_3_0_2_FILE    "btfw32.tlv"
#define ROME_NVM_TLV_3_0_2_FILE         "btnv32.bin"

/* QCA Tufello 1.0 firmware */
#define TF_RAMPATCH_TLV_1_0_0_FILE      "rampatch_tlv_tf_1.0.tlv"
#define TF_NVM_TLV_1_0_0_FILE           "nvm_tlv_tf_1.0.bin"

/* QCA Tufello 1.1 firmware */
#define TF_RAMPATCH_TLV_1_0_1_FILE      "tfbtfw11.tlv"
#define TF_NVM_TLV_1_0_1_FILE           "tfbtnv11.bin"

#define DEFAULT_QCA_CONFIG_PATH         BT_CONFIG_PATH

#define DEFAULT_QCA_LOG_PATH            BT_BOOTSTRAP_LOG_PATH

/* QCA firmware type: RAM patch(1) or NVM(2). */
#define QCA_FW_TYPE_RAM_PATCH           1
#define QCA_FW_TYPE_NVM                 2

/* QCA firmware download configuration. See "download_configuration" in "QCA_RAM_PATCH_HEADER_T". */
#define FW_DOWNLOAD_CFG_SKIP_NONE_EVT   0x00        /* Wait for both Vendor Status Event and Command Complete Event. */
#define FW_DOWNLOAD_CFG_SKIP_VS_EVT     0x01        /* Skip Vendor Status Event to HOST. */
#define FW_DOWNLOAD_CFG_SKIP_CC_EVT     0x02        /* Skip Command Complete Event to HOST. */
#define FW_DOWNLOAD_CFG_SKIP_VS_CC_EVT  0x03        /* Skip both Vendor Status Event and Command Complete Event. */

#define VALID_FW_DOWNLOAD_CFG(cfg)      (((cfg) == FW_DOWNLOAD_CFG_SKIP_NONE_EVT) || \
                                         ((cfg) == FW_DOWNLOAD_CFG_SKIP_VS_CC_EVT))


typedef uint8_t     QcaBaudrateT;
#define QCA_BAUDRATE_115200             ((QcaBaudrateT) 0x00)
#define QCA_BAUDRATE_57600              ((QcaBaudrateT) 0x01)
#define QCA_BAUDRATE_38400              ((QcaBaudrateT) 0x02)
#define QCA_BAUDRATE_19200              ((QcaBaudrateT) 0x03)
#define QCA_BAUDRATE_9600               ((QcaBaudrateT) 0x04)
#define QCA_BAUDRATE_230400             ((QcaBaudrateT) 0x05)
#define QCA_BAUDRATE_250000             ((QcaBaudrateT) 0x06)
#define QCA_BAUDRATE_460800             ((QcaBaudrateT) 0x07)
#define QCA_BAUDRATE_500000             ((QcaBaudrateT) 0x08)
#define QCA_BAUDRATE_720000             ((QcaBaudrateT) 0x09)
#define QCA_BAUDRATE_921600             ((QcaBaudrateT) 0x0A)
#define QCA_BAUDRATE_1000000            ((QcaBaudrateT) 0x0B)
#define QCA_BAUDRATE_1250000            ((QcaBaudrateT) 0x0C)
#define QCA_BAUDRATE_2000000            ((QcaBaudrateT) 0x0D)
#define QCA_BAUDRATE_3000000            ((QcaBaudrateT) 0x0E)
#define QCA_BAUDRATE_4000000            ((QcaBaudrateT) 0x0F)
#define QCA_BAUDRATE_1600000            ((QcaBaudrateT) 0x10)
#define QCA_BAUDRATE_3200000            ((QcaBaudrateT) 0x11)
#define QCA_BAUDRATE_3500000            ((QcaBaudrateT) 0x12)
#define QCA_BAUDRATE_AUTO               ((QcaBaudrateT) 0xFE)
#define QCA_BAUDRATE_RESERVED           ((QcaBaudrateT) 0xFF)

#define QCA_VERSION(soc, build)         SET_DWORD(soc, build)
#define QCA_SOC_ID(val)                 GET_LOWORD(val)
#define QCA_BUILD_VER(val)              GET_HIWORD(val)

/* QCA SoC ID. */
typedef uint16_t    QcaSocIdT;
#define QCA_SOC_ID_13                   ((QcaSocIdT) 0x0013)
#define QCA_SOC_ID_22                   ((QcaSocIdT) 0x0022)
#define QCA_SOC_ID_23                   ((QcaSocIdT) 0x0023)
#define QCA_SOC_ID_44                   ((QcaSocIdT) 0x0044)

/* QCA Build Version. */
typedef uint16_t    QcaBuildVerT;
#define QCA_BUILD_VER_0300              ((QcaBuildVerT) 0x0300)
#define QCA_BUILD_VER_0302              ((QcaBuildVerT) 0x0302)

/* QCA version. */
#define QCA_VER_UNKNOWN                 (0)
#define QCA_VER_ROME_3_0                QCA_VERSION(QCA_SOC_ID_22, QCA_BUILD_VER_0300)      /* (0x0022, 0x0300) */
#define QCA_VER_ROME_3_2                QCA_VERSION(QCA_SOC_ID_44, QCA_BUILD_VER_0302)      /* (0x0044, 0x0302) */
#define QCA_VER_TUFELLO_1_0             QCA_VERSION(QCA_SOC_ID_13, QCA_BUILD_VER_0300)      /* (0x0013, 0x0300) */
#define QCA_VER_TUFELLO_1_1             QCA_VERSION(QCA_SOC_ID_23, QCA_BUILD_VER_0302)      /* (0x0023, 0x0302) */

/* Max size in byte of QCA firmware(tlv) packet, which is wrapped in HCI vendor command. */
#define MAX_TLV_PACKET_SIZE             0xf3        /* 243 */

#define MAX_NVM_TAG_VALUE_LENGTH        0xfc        /* 252 */

/* OCF in HCI VS(Vendor-Specific) Command, in which only low 10 bits are valid. Refer to chaper 
   "8 HCI Vendor-Specific Commands" in "80-Y7674-70_B_QCA61x4_QCA937x_QCA65x4_Bluetooth_Software_User_Guide". */
typedef uint16_t    QcaVendorCmdT;
#define HCI_VS_DOWNLOAD_COMMAND         ((QcaVendorCmdT) 0x00)
#define HCI_VS_NVM_COMMAND              ((QcaVendorCmdT) 0x0B)
#define HCI_VS_HOST_LOG_OPCODE          ((QcaVendorCmdT) 0x17)
#define HCI_VS_SET_BAUDRATE             ((QcaVendorCmdT) 0x48)

/* QCA NVM command, wrapped in HCI Vendor-Specific Command. */
typedef uint8_t     QcaNvmCmdT;
#define NVM_CMD_READ                    ((QcaNvmCmdT) 0x00)     /* Sub-opcode to get NVM tag value. */
#define NVM_CMD_WRITE                   ((QcaNvmCmdT) 0x01)     /* Sub-opcode to set NVM tag value. */

/* QCA EDL(Embedded Downloader) command, wrapped in HCI Vendor-Specific Command. */
typedef uint8_t     QcaEdlCmdT;
#define EDL_CMD_CORE_DUMP               ((QcaEdlCmdT) 0x00)     /* EDL command to enable/disable core dump in QCA chip. */
#define EDL_CMD_GET_CHIP_VERSION        ((QcaEdlCmdT) 0x19)     /* EDL command to get QCA chip's version. */
#define EDL_CMD_DOWNLOAD_FIRMWARE       ((QcaEdlCmdT) 0x1e)     /* EDL command to downalod QCA firmware(tlv). */

/* QCA EDL(Embedded Downloader) event, wrapped in HCI Vendor-Specific event. */
typedef uint8_t     QcaEdlEvtT;
#define EDL_EVT_REQUEST_RESULT          ((QcaEdlEvtT) 0x00)     /* EDL event for EDL command(request) result. */
#define EDL_EVT_BT_CONTROLLER_LOG       ((QcaEdlEvtT) 0x01)     /* EDL event for QCA BT controller log. */
#define EDL_EVT_NVM                     ((QcaEdlEvtT) 0x0B)     /* EDL event for NVM command class. */
#define EDL_EVT_SET_BAUDRATE_RESP       ((QcaEdlEvtT) 0x92)     /* EDL event for setting baudrate response. */

/* QCA EDL event result type. */
typedef uint8_t     QcaEdlEvtResT;
#define EDL_EVT_RES_GET_APP_VER         ((QcaEdlEvtResT) 0x02)  /* EDL event result for getting app version. */
#define EDL_EVT_RES_DOWNLOAD_FIRMWARE   ((QcaEdlEvtResT) 0x04)  /* EDL event result for downloading firmware. */
#define EDL_EVT_RES_GET_CHIP_VER        ((QcaEdlEvtResT) 0x19)  /* EDL event result for getting chip version. */

/* QCA HCI vendor command result code. */
typedef uint8_t     QcaHciVendorCmdResT;
#define HCI_VS_CMD_RESULT_SUCCESS           ((QcaHciVendorCmdResT) 0x00)
#define HCI_VS_CMD_RESULT_PATCH_LEN_ERROR   ((QcaHciVendorCmdResT) 0x01)
#define HCI_VS_CMD_RESULT_PATCH_VER_ERROR   ((QcaHciVendorCmdResT) 0x02)
#define HCI_VS_CMD_RESULT_PATCH_CRC_ERROR   ((QcaHciVendorCmdResT) 0x03)
#define HCI_VS_CMD_RESULT_PATCH_NOT_FOUND   ((QcaHciVendorCmdResT) 0x04)
#define HCI_VS_CMD_RESULT_TLV_TYPE_ERROR    ((QcaHciVendorCmdResT) 0x10)

#define IS_HCI_VS_CMD_RESULT_SUCCESS(res)   ((res) == HCI_VS_CMD_RESULT_SUCCESS)

#define EDL_EVT_OFFSET                  0   /* EDL event offset relative to the payload/data in HCI vendor event. */
#define EDL_EVT_RES_OFFSET              1   /* EDL event result offset relative to the payload/data in HCI vendor event. */

#define SET_BAUDRATE_RESULT_SUCCESS     0x01

/* NVM(Nonvolatile Memory) TAG definition. Refer to "7.2 NVM tag summary" in 
   "80-Y7674-70_B_QCA61x4_QCA937x_QCA65x4_Bluetooth_Software_User_Guide". */
typedef uint16_t    QcaNvmTagT;
#define NVM_TAG_NUM_2                   ((QcaNvmTagT) 0x02)     /* 2  : Bluetooth address */
#define NVM_TAG_NUM_17                  ((QcaNvmTagT) 0x11)     /* 17 : HCI transport layer parameters, including IBS setting. */
#define NVM_TAG_NUM_27                  ((QcaNvmTagT) 0x1B)     /* 27 : Sleep-enable mask */
#define NVM_TAG_NUM_28                  ((QcaNvmTagT) 0x1C)     /* 28 : Xtal Fine Tuning for v3.1 */
#define NVM_TAG_NUM_36                  ((QcaNvmTagT) 0x24)     /* 36 : Power Configuration */
#define NVM_TAG_NUM_38                  ((QcaNvmTagT) 0x26)     /* 38 : Debug Control */
#define NVM_TAG_NUM_44                  ((QcaNvmTagT) 0x2C)     /* 44 : Voice options, including PCM role setting. */
#define NVM_TAG_NUM_47                  ((QcaNvmTagT) 0x2F)     /* 47 : Transport user baud rate. */
#define NVM_TAG_NUM_82                  ((QcaNvmTagT) 0x52)     /* 82 : XtalFine Tuning for v3.2 */
#define NVM_TAG_NUM_83                  ((QcaNvmTagT) 0x53)     /* 83 : LE Tx Power Control */

/* NVM TAG num 2 definition for Bluetooth address. */
#define NVM_TAG_NUM_2_LENGTH            6

/* NVM TAG num 17 definition for HCI transport layer parameters. */
#define NVM_TAG_NUM_17_LENGTH           8
#define FW_CONF_TRANSFER_OPTIONS_BYTE   0       /* Byte0 (transport options) in TAG num 17. */
#define FW_CONF_UART_BAUDRATE_BYTE      2       /* Byte 2 (UART baud rate) ¨CDefault is 0x0E */
#define FW_CONF_IBS_BYTE                FW_CONF_TRANSFER_OPTIONS_BYTE
#define FW_CONF_IBS_BIT                 6       /* Bit6 (H4 in-band sleep) 
                                                   0: Enable EXT_WAKE/HOST_WAKE signaling
                                                   1: Enable in-band sleep over H4 HCI */
#define FW_CONF_SW_IBS_BYTE             FW_CONF_TRANSFER_OPTIONS_BYTE
#define FW_CONF_SW_IBS_BIT              7       /* Bit 7 (H4 software in-band sleep)
                                                   0: Disable software in-band sleep over H4 HCI
                                                   1: Enable software in-band sleep over H4 HCI */
#define FW_CONF_UART_FLOW_CONTROL_BYTE  FW_CONF_TRANSFER_OPTIONS_BYTE
#define FW_CONF_UART_FLOW_CONTROL_BIT   0       /* Bit 0 (3-wire UART obey FlowControl) ¨CDefault = 0
                                                   0: Disable hardware flow control
                                                   1: Force three-wire UART to obey hardware flow control from the host */

/* NVM TAG num 27 definition for Sleep-enable mask. */
#define NVM_TAG_NUM_27_LENGTH           1
#define FW_CONF_DEEP_SLEEP_BYTE         0
#define FW_CONF_DEEP_SLEEP_BIT          0       /* 0: Disable Deep Sleep mode, 1: Enable Deep Sleep (default). */

/* NVM TAG num 38 definition for Debug Control. */
#define NVM_TAG_NUM_38_LENGTH           13      /* 0x0D */
#define FW_CONF_PCM_LOOPBACK_MODE_BYTE  9       /* Byte9 (transport options) in TAG num 38. */

/* NVM TAG num 44 definition for voice options. */
/* -------------------------------------------- */
#define NVM_TAG_NUM_44_LENGTH           41          /* 0x29 */

/* QCA PCM role: master or slave. */
#define FW_CONF_PCM_MASTER              0
#define FW_CONF_PCM_SLAVE               1

/* PCM clock divisor */
#define FW_CONF_NBS_PCM_CLOCK_DIVISOR_BYTE      6         /* Bytes 6-7 (NBS PCM clock divisor) */    

#define FW_CONF_WBS_PCM_CLOCK_DIVISOR_BYTE      30        /* Bytes 30-31 (WBS PCM clock divisor) */ 

/* PCM frame divisor */
#define FW_CONF_NBS_PCM_FRAME_DIVISOR_BYTE      8         /* Byte 8 (NBS PCM frame divisor) */    

#define FW_CONF_WBS_PCM_FRAME_DIVISIOR_BYTE     32        /* Byte 32 (WBS PCM frame divisor) */ 

/* Audio interface control */
#define FW_CONF_NBS_CONTROL_BYTE                9           /* Byte9 (NBS audio interface control) */
#define FW_CONF_NBS_FRAME_SYNC_TYPE_BIT         0           /* Bits 0-3 (frame sync type) */
#define FW_CONF_NBS_PCM_ROLE_BIT                4           /* Bit 4 (PCM role) */
#define FW_CONF_NBS_INVERT_BLCK_BIT             6           /* Bit 6 (invert BCLK) */

#define FW_CONF_WBS_CONTROL_BYTE                33          /* Byte33 (WBS audio interface control) */
#define FW_CONF_WBS_FRAME_SYNC_TYPE_BIT         0           /* Bits 0-3 (frame sync type) */
#define FW_CONF_WBS_PCM_ROLE_BIT                4           /* Bit 4 (PCM role) */
#define FW_CONF_WBS_INVERT_BLCK_BIT             6           /* Bit 6 (invert BCLK) */

#define FW_CONF_PCM_FRAME_SYNC_TYPE_SHORT_LF    0x00        /* Short LF */
#define FW_CONF_PCM_FRAME_SYNC_TYPE_SHORT_FR    0x02        /* Short FR */
#define FW_CONF_PCM_FRAME_SYNC_TYPE_SHORT_FF    0x04        /* Short FF */
#define FW_CONF_PCM_FRAME_SYNC_TYPE_LONG        0x06        /* Long */
#define FW_CONF_PCM_FRAME_SYNC_TYPE_I2S         0x08        /* I2S */

/* PCM clock */
#define FW_CONF_PCM_BCLK_256K                   256000      /* 256KHz bit clock (0x3E800) */
#define FW_CONF_PCM_BCLK_2M                     2048000     /* 2.048MHz bit clock (0x1F4000) */
#define FW_CONF_PCM_BCLK_4M                     4096000     /* 4.096MHz bit clock (0x3E8000) */

/* PCM clock divisor value (Clock in QCA6574 is 64MHz) */
#define FW_CONF_PCM_CLOCK_DIVISOR_256K          0x0500      /* 256KHz bit clock divisor, valid for NBS and WBS */
#define FW_CONF_PCM_CLOCK_DIVISOR_2M            0x2800      /* 2.048MHz bit clock divisor, valid for NBS and WBS */
#define FW_CONF_PCM_CLOCK_DIVISOR_4M            0x5000      /* 4.096MHz bit clock divisor, valid for NBS and WBS */

/* PCM sync clock */
#define FW_CONF_PCM_SYNC_CLOCK_8K               8000        /* 8KHz sync clock (0x1F40) */
#define FW_CONF_PCM_SYNC_CLOCK_16K              16000       /* 16KHz sync clock (0x3E80) */

/* PCM frame divisor */
#define FW_CONF_PCM_FRAME_DIVISOR_8K            0x1F        /* 8KHz sync clock divisor, valid for NBS */
#define FW_CONF_PCM_FRAME_DIVISOR_16K           0x0F        /* 16KHz sync clock divisor, valid for WBS */
/* -------------------------------------------- */

/* NVM TAG num 36 (Power Configuration) definition */
#define NVM_TAG_NUM_36_LENGTH                   12          /* 0x0C */

/* NVM TAG num 83 (LE Tx Power Control) definition */
#define NVM_TAG_NUM_83_LENGTH                   8           /* 0x08 */

/* NVM TAG num 28 (Xtal Fine Tuning for v3.1) definition */
/* -------------------------------------------- */
#define NVM_TAG_NUM_28_LENGTH                   20          /* 0x14 */

#define MIN_CDACIN_OFFSET                       0
#define MAX_CDACIN_OFFSET                       7F

#define MIN_CDACOUT_OFFSET                      0
#define MAX_CDACOUT_OFFSET                      7F

#define DISABLE_OFFSET_FROM_NVM                 ((uint16_t) 0x0000)
#define ENABLE_OFFSET_FROM_NVM                  ((uint16_t) 0x0080)
/* -------------------------------------------- */

/* NVM TAG num 82 (Xtal Fine Tuning for v3.2) definition */
#define NVM_TAG_NUM_82_LENGTH                   16          /* 0x10 */


#pragma pack(push, 1)

typedef struct 
{
    uint8_t     fw_type;        /* QCA firmware type: RAM patch(1) or NVM(2). */
    uint8_t     fw_length1;     /* Byte0(LSB) for firmware length, excluding firmware header. */
    uint8_t     fw_length2;     /* Byte1 for firmware length. */
    uint8_t     fw_length3;     /* Byte2 for firmware length. */
} QCA_FW_HEADER_T;

typedef struct 
{
    uint32_t    total_length;               /* Length of the whole TLV data includes this field. */
    uint32_t    patch_data_length;          /* Length of patch data. */
    uint8_t     signing_format_version;     /* Version of the signing format. */
    uint8_t     signature_algorithm;        /* 0: Undefined
                                               1: ECDSA_P-256_SHA256 
                                               2: RSA-256_SHA256 
                                               3..255: Reserved */
    uint8_t     download_config;            /* Bit0=1: Skip Vendor Status Event to HOST
                                               Bit1=1: Skip Command completes Event to HOST */
    uint8_t     reserved1;                  /* Reserved bytes. */
    uint16_t    product_id;                 /* Product ID the patch applies. */
    uint16_t    rom_build_version;          /* Build version of the patch. */
    uint16_t    patch_version;              /* Patch version. */
    uint16_t    reserved2;                  /* Reserved bytes. */
    uint32_t    patch_entry_address;        /* Patch entry address(absolute address). */
} QCA_RAM_PATCH_HEADER_T;

/* QCA RAM patch file, which is in TLV(Tag-Length Value) format. */
typedef struct 
{
    QCA_FW_HEADER_T         fw_header;      /* QCA firmware header. */

    QCA_RAM_PATCH_HEADER_T  ram_patch;      /* RAM patch header */
} QCA_RAM_PATCH_INFO_T;

typedef struct 
{
    uint16_t    tag_number;     /* NVM tag number. */
    uint16_t    tag_length;     /* NVM tag length, excluding NVM header. */
    uint32_t    tag_ptr;
    uint32_t    tag_ex_flag;
} QCA_NVM_HEADER_T;

/* QCA NVM file, which is in TLV(Tag-Length Value) format. */
typedef struct 
{
    QCA_FW_HEADER_T     fw_header;  /* QCA firmware header. */

    QCA_NVM_HEADER_T    nvm;        /* NVM header */
} QCA_NVM_INFO_T;

typedef struct
{
    QcaEdlCmdT      edl_cmd;                        /* EDL command. */
    uint8_t         payload_length;                 /* Length in byte for the payload of EDL command. */
} QCA_EDL_CMD_HEADER_T;

typedef struct
{
    QcaEdlCmdT      edl_cmd;                        /* EDL command. */
    uint8_t         payload_length;                 /* Length in byte for the payload of EDL command. */
    uint8_t         payload[MAX_TLV_PACKET_SIZE];   /* Payload of EDL command. */
} QCA_EDL_CMD_GENERIC_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* EDL event. */
    uint8_t         result;                         /* 1: success, 0: fail. */
} QCA_EDL_EVT_SET_BAUDRATE_RESP_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* EDL event. */
    QcaEdlEvtResT   edl_evt_res;                    /* EDL event result type. */
} QCA_EDL_EVT_RES_HEADER_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* EDL event. */
    QcaEdlEvtResT   edl_evt_res;                    /* EDL event result type. */
    uint32_t        product_id;                     /* QCA product id. */
    uint16_t        fw_patch_ver;                   /* Firmware patch version. */
    uint16_t        rom_build_ver;                  /* ROM build version. */
    uint32_t        soc_id;                         /* QCA SoC id. */
} QCA_EDL_EVT_GET_CHIP_VER_RES_T;

typedef struct
{
    QcaEdlEvtT          edl_evt;                    /* EDL event. */
    QcaEdlEvtResT       edl_evt_res;                /* EDL event result type. */
    QcaHciVendorCmdResT result;                     /* HCI vendor command result code. */
} QCA_EDL_EVT_DOWNLOAD_FW_RES_T;

typedef struct
{
    QcaEdlCmdT      edl_cmd;                        /* EDL command. */
    uint8_t         enable;                         /* 1: enable core dump in QCA, 0: disable. */
} QCA_EDL_CMD_COREDUMP_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* [FIXME] EDL event or EDL event result of core dump? */
} QCA_EDL_EVT_COREDUMP_T;

typedef struct
{
    QcaNvmCmdT      sub_opcode;                     /* Sub-opcode for NVM command. */
    uint8_t         nvm_tag;                        /* NVM tag index number. */
} QCA_NVM_READ_CMD_T;

typedef struct
{
    QcaNvmCmdT      sub_opcode;                     /* Sub-opcode for NVM command. */
    uint8_t         nvm_tag;                        /* NVM tag index number. */    
    uint8_t         tag_value_length;               /* NVM tag value length. */
} QCA_NVM_WRITE_CMD_HEADER_T;

typedef struct
{
    QcaNvmCmdT      sub_opcode;                     /* Sub-opcode for NVM command. */
    uint8_t         nvm_tag;                        /* NVM tag index number. */    
    uint8_t         tag_value_length;               /* NVM tag value length. */
    uint8_t         tag_value[MAX_NVM_TAG_VALUE_LENGTH];    /* NVM tag value. */
} QCA_NVM_WRITE_CMD_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* EDL event for NVM command class. */
    QcaNvmCmdT      sub_opcode;                     /* Sub-opcode for NVM command. */
    uint8_t         nvm_tag;                        /* NVM tag index number. */    
    uint8_t         tag_value_length;               /* NVM tag value length. */
} QCA_EDL_EVT_NVM_HEADER_T;

typedef struct
{
    QcaEdlEvtT      edl_evt;                        /* EDL event for NVM command class. */
    QcaNvmCmdT      sub_opcode;                     /* Sub-opcode for NVM command. */
    uint8_t         nvm_tag;                        /* NVM tag index number. */    
    uint8_t         tag_value_length;               /* NVM tag value length. */
    uint8_t         tag_value[MAX_NVM_TAG_VALUE_LENGTH];    /* NVM tag value. */
} QCA_EDL_EVT_NVM_T;

#pragma pack(pop)

typedef struct
{
    uint32_t     qca_chip_ver;
    char        *ram_patch_file;
    char        *nvm_file;
} QcaFirmwareMap;

typedef struct
{
    bool        valid_addr;         /* true: 'bt_addr' is valid, false: not. */
    BT_ADDR_T   bt_addr;            /* Local Bluetooth address. */

    bool        valid_ibs;          /* true: 'ibs' is valid, false: not. */
    bool        enable_ibs;         /* true: enable ibs, false: disable. */

    bool        valid_deep_sleep;   /* true: 'deep_sleep' is valid, false: not. */
    bool        enable_deep_sleep;  /* true: enable deep sleep, false: disable. */

    bool        valid_pcm;          /* true: 'pcm_role' is valid, false: not. */
    uint8_t     pcm_role;           /* PCM role: master(0) or slave(1). */

    bool        valid_invert_bclk;  /* true: 'invert_bclk' is valid, false: not. */
    bool        invert_bclk;        /* true: invert bit clock, false: normal bit clock. */

    bool        valid_nbs_bclk;     /* true: 'nbs_bclk' is valid, false: not. */
    uint32_t    nbs_bclk;           /* true: NBS(Narrow Band Speech) bit clock. E.g. 256KHz = 256000 */

    bool        valid_wbs_bclk;     /* true: 'wbs_bclk' is valid, false: not. */
    uint32_t    wbs_bclk;           /* true: WBS(Wide Band Speech) bit clock. E.g. 256KHz = 256000 */

    bool        valid_nbs_sclk;     /* true: 'nbs_sclk' is valid, false: not. */
    uint32_t    nbs_sclk;           /* true: NBS(Narrow Band Speech) sync clock. E.g. 8KHz = 8000 */

    bool        valid_wbs_sclk;     /* true: 'wbs_sclk' is valid, false: not. */
    uint32_t    wbs_sclk;           /* true: WBS(Wide Band Speech) syc clock. E.g. 16KHz = 16000 */

    bool        valid_pcm_lp;       /* true: 'pcm_lp' is valid, false: not. */
    bool        enable_pcm_lp;      /* true: enable pcm loopback, false: disable. */
                                       
    bool        valid_fw_log;       /* true: 'fw_log' is valid, false: not. */
    bool        enable_fw_log;      /* true: enable fw log, false: disable. */

    bool        valid_power_cfg;                            /* true: power configuration is valid, false: not. */
    uint8_t     power_cfg[NVM_TAG_NUM_36_LENGTH];           /* power configuration (12 bytes). */

    bool        valid_le_tx_power_ctrl;                     /* true: le tx power control is valid, false: not. */
    uint8_t     le_tx_power_ctrl[NVM_TAG_NUM_83_LENGTH];    /* le tx power control (3 bytes). */

    bool        valid_xtal_fine_tuning;                     /* true: xtal fine tuning for v3.1 is valid, false: not */
    uint8_t     xtal_fine_tuning[NVM_TAG_NUM_28_LENGTH];    /* xtal fine tuning for v3.1 */

    bool        valid_xtal_fine_tuning2;                    /* true: xtal fine tuning for v3.2 is valid, false: not */
    uint8_t     xtal_fine_tuning2[NVM_TAG_NUM_82_LENGTH];   /* xtal fine tuning for v3.2 */
} QcaExtraConfig;

ConnxResult BootQca(void);

bool QCA_GetFirmwareConfig(char *config_path, char *file_name, QcaExtraConfig *extraConfig);

uint16_t MapBitClock2Divisor(uint32_t bit_clock);

uint16_t MapSyncClock2Divisor(uint32_t sync_clock);


#ifdef __cplusplus
}
#endif

#endif  /* _BOOT_QCA_H_ */
