/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #3 $
******************************************************************************/

#ifndef _BT_HCI_CMD_H_
#define _BT_HCI_CMD_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Packet sizes */
#define HCI_COMMAND_HDR_SIZE            3   /* opcode=2, length=1 */
#define HCI_EVENT_HDR_SIZE              2   /* event=1, length=1 */

#define HCI_COMMAND_DATA_SIZE           255
#define HCI_EVENT_DATA_SIZE             255

#define MAX_HCI_COMMAND_SIZE            (HCI_COMMAND_HDR_SIZE + HCI_COMMAND_DATA_SIZE)
#define MAX_HCI_EVENT_SIZE              (HCI_EVENT_HDR_SIZE + HCI_EVENT_DATA_SIZE)

#define HCI_OCF_MASK                    0x03FF

typedef uint16_t    hci_op_code_t;
#define HCI_NOP                         ((hci_op_code_t) 0x0000)
#define HCI_HOST_BB                     ((hci_op_code_t) 0x0C00)
#define HCI_INFO                        ((hci_op_code_t) 0x1000)
#define HCI_TEST                        ((hci_op_code_t) 0x1800)

#define HCI_READ_BD_ADDR                ((hci_op_code_t) HCI_INFO | 0x0009)

#define HCI_RESET                       ((hci_op_code_t) HCI_HOST_BB | 0x0003)
#define HCI_SET_EVENT_FILTER            ((hci_op_code_t) HCI_HOST_BB | 0x0005)
#define HCI_WRITE_SCAN_ENABLE           ((hci_op_code_t) HCI_HOST_BB | 0x001A)
#define HCI_WRITE_AUTH_ENABLE           ((hci_op_code_t) HCI_HOST_BB | 0x0020)
#define HCI_WRITE_ENC_MODE              ((hci_op_code_t) HCI_HOST_BB | 0x0022)

#define HCI_ENABLE_DUT_MODE             ((hci_op_code_t) HCI_TEST | 0x0003)

#define HCI_MANUFACTURER_EXTENSION      ((hci_op_code_t) 0xFC00)
#define HCI_VENDOR_CMD(ocf)             ((hci_op_code_t) HCI_MANUFACTURER_EXTENSION | ((ocf) & 0x03FF))

typedef uint8_t     hci_event_code_t;
#define HCI_EV_COMMAND_COMPLETE         ((hci_event_code_t) 0x0E)
#define HCI_EV_COMMAND_STATUS           ((hci_event_code_t) 0x0F)
#define HCI_EV_VENDOR_SPECIFIC          ((hci_event_code_t) 0xFF)

typedef uint8_t     hci_return_t;
#define HCI_SUCCESS                     ((hci_return_t) 0x00)

#define IS_HCI_SUCCESS(ret)             ((ret) == HCI_SUCCESS)


#pragma pack(push, 1)

typedef struct
{
    hci_op_code_t       op_code;                        /* op code of command */
    uint8_t             length;                         /* parameter total length */
} HCI_COMMAND_HEADER_T;

/* HCI Reset command */
typedef HCI_COMMAND_HEADER_T    HCI_RESET_T;

/* HCI generic command */
typedef struct
{
    hci_op_code_t       op_code;                        /* op code of command */
    uint8_t             length;                         /* parameter total length */
    uint8_t             data[HCI_COMMAND_DATA_SIZE];        
} HCI_COMMAND_GENERIC_T;

/* HCI Vendor-Specific command */
typedef HCI_COMMAND_GENERIC_T   HCI_VENDOR_SPECIFIC_T;

typedef struct
{
    hci_event_code_t    event_code;                     /* event code */
    uint8_t             length;                         /* parameter total length */
} HCI_EVENT_HEADER_T;

typedef struct
{
    hci_event_code_t    event_code;                     /* event code */
    uint8_t             length;                         /* parameter total length */
    uint8_t             num_hci_command_pkts;           /* number of hci command packets which are allowed to be sent to the controller */
    hci_op_code_t       op_code;                        /* op code of command that caused this event */
    hci_return_t        status;
} HCI_EV_COMMAND_COMPLETE_T;

typedef struct
{
    hci_event_code_t    event_code;                     /* event code */
    uint8_t             length;                         /* parameter total length */
    hci_return_t        status;
    uint8_t             num_hci_command_pkts;           /* number of hci command packets which are allowed to be sent to the controller */
    hci_op_code_t       op_code;                        /* op code of command that caused this event */
} HCI_EV_COMMAND_STATUS_T;

typedef struct
{
    hci_event_code_t    event_code;                     /* event code */
    uint8_t             length;                         /* parameter total length */
    uint8_t             data[HCI_EVENT_DATA_SIZE];      /* parameter data */
} HCI_EV_VENDOR_SPECIFIC_T;

typedef struct
{
    hci_event_code_t    event_code;                     /* event code */
    uint8_t             length;                         /* parameter total length */
    uint8_t             data[HCI_EVENT_DATA_SIZE];      /* parameter data */
} HCI_EV_GENERIC_T;

typedef struct
{
    HCI_EV_COMMAND_COMPLETE_T   command_complete;
    uint8_t                     bd_addr[BT_ADDR_LENGTH];
} HCI_EV_READ_BD_ADDR_T;

#pragma pack(pop)

#define HCI_COMMAND_SIZE(cmd)           (sizeof(HCI_COMMAND_HEADER_T) + ((HCI_COMMAND_HEADER_T *)(cmd))->length)

#define HCI_EVENT_SIZE(evt)             (sizeof(HCI_EVENT_HEADER_T) + ((HCI_EVENT_HEADER_T *)(evt))->length)


#ifdef __cplusplus
}
#endif

#endif  /* _BT_HCI_CMD_H_ */
