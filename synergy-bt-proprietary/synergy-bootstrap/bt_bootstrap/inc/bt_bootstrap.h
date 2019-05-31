/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _BT_BOOTSTRAP_H_
#define _BT_BOOTSTRAP_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/* BT chip type. */
typedef uint16_t    BtChipType;

#define BT_CHIP_QCA             ((BtChipType) 0x0000)
#define BT_CHIP_ROME            ((BtChipType) 0x0001)

#define BT_CHIP_BLUECORE        ((BtChipType) 0xFF00)
#define BT_CHIP_CSR8311         ((BtChipType) 0xFF01)

#define BT_CHIP_UNKNOWN         ((BtChipType) 0xFFFF)

#define IS_QCA_CHIP(type)       (((type) == BT_CHIP_QCA) || \
                                 ((type) == BT_CHIP_ROME))

#define IS_BLUECORE_CHIP(type)  (((type) == BT_CHIP_BLUECORE) || \
                                 ((type) == BT_CHIP_CSR8311))

#define BRI_GET_SIZE(ri)               ((ri)->size)
#define BRI_GET_CHIP_TYPE(ri)          ((ri)->chip_type)
#define BRI_GET_SERIAL_PORT(ri)        ((ri)->serial_port)
#define BRI_GET_INIT_BAUDRATE(ri)      ((ri)->init_baudrate)
#define BRI_GET_RESET_BAUDRATE(ri)     ((ri)->reset_baudrate)
#define BRI_GET_FLOW_CONTROL(ri)       ((ri)->flow_control)
#define BRI_GET_FW_PATH(ri)            ((ri)->fw_path)
#define BRI_GET_CONFIG_PATH(ri)        ((ri)->config_path)


typedef struct
{
    uint32_t    size;                   /* Total size of BT bootstrap register information. */
    uint16_t    chip_type;              /* BT chip type. */
    char       *serial_port;            /* Device name for UART port. */
    uint32_t    init_baudrate;          /* Initial UART baudrate. Default: 115200bps. */
    uint32_t    reset_baudrate;         /* Actual UART baudrate after BT is booted up. */
    bool        flow_control;           /* true: enable h/w flow control, false: disable. */
    char       *fw_path;                /* Path to store BT chip's firmware. */
    char       *config_path;            /* Path to store BT chip's firmware configuration file. */
} BtBootstrapRegisterInfo;


bool QcomBtBootstrap(BtBootstrapRegisterInfo *registerInfo);


#ifdef __cplusplus
}
#endif

#endif  /* _BT_BOOTSTRAP_H_ */