/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #4 $
******************************************************************************/

#ifndef _BT_BOOTSTRAP_UTIL_H_
#define _BT_BOOTSTRAP_UTIL_H_

#include "connx_util.h"
#include "connx_log.h"
#include "connx_log_setting.h"
#include "bt_bootstrap.h"
#include "bt_bootstrap_config.h"

#ifdef __cplusplus
extern "C" {
#endif


#define QCA_FUNC_ENTER()                BT_FUNC_ENTER()
#define QCA_FUNC_LEAVE()                BT_FUNC_LEAVE()
#define QCA_OUTPUT_RESULT(r)            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Result: 0x%x"), __FUNCTION__, (r)))

/* HCI transport type. */
#define HCI_TRANSPORT_TYPE_UNKNOWN      (0x0)
#define HCI_TRANSPORT_TYPE_BCSP         (0x1)
#define HCI_TRANSPORT_TYPE_H4           (0x2)

#define IS_VALID_HCI_TRANSPORT(type)    ((type) != HCI_TRANSPORT_TYPE_UNKNOWN)

/* The format of BT address stored in bt.inf, e.g. "BT_ADDR = 0002:5b:123456 " */
#define BT_ADDR_FORMAT                  "BT_ADDR = %04x:%02x:%06x "

/* Bluetooth address include nap(16bit) + uap(8bit) + lap(24bit). */
#define BT_ADDR_FIELD_COUNT             3

/* Bit mask for the fields of Bluetooth address. */
#define BT_ADDR_NAP_MASK                0xffff
#define BT_ADDR_UAP_MASK                0xff
#define BT_ADDR_LAP_MASK                0xffffff

#define HTS_GET_TRANSPORT_DEVICE(s)     ((s)->device)
#define HTS_GET_INIT_BAUDRATE(s)        ((s)->init_baudrate)
#define HTS_GET_RESET_BAUDRATE(s)       ((s)->reset_baudrate)
#define HTS_GET_FLOW_CONTROL(s)         ((s)->flow_control)


typedef struct
{
    char       *device;             /* UART device name. */
    uint32_t    init_baudrate;      /* Initial UART baudrate, such as 115200bps. */
    uint32_t    reset_baudrate;     /* Actual UART baudrate after BT chip reset, such as 3Mbps */
    bool        flow_control;       /* true: enable h/w flow control, false: not. */
} HciTransportSetting;

typedef struct
{
    BtBootstrapRegisterInfo     registerInfo;
    ConnxLogSetting             logSetting;
} BtBootstrapInstance;


BtBootstrapInstance *GetBtBootstrapInstance(void);

bool IsValidRegisterInfo(BtBootstrapRegisterInfo *registerInfo);
/* [QTI] Fix KW issue#16794. */
void CpyRegisterInfo(BtBootstrapRegisterInfo *dst, BtBootstrapRegisterInfo *src);
void FreeRegisterInfo(BtBootstrapRegisterInfo *registerInfo);

bool ReadBluetoothAddress(const char *path, const char *file_name, BT_ADDR_T *bt_addr);

void InitBtBootstrapLog(void);
void DeinitBtBootstrapLog(void);

uint32_t FindValidH4Head(const uint8_t *ptr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  /* _BT_BOOTSTRAP_UTIL_H_ */
