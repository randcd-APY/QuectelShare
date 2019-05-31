/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#ifndef _BT_H4_H_
#define _BT_H4_H_

#include "bt_hci_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif


/* HCI Package Headers */
#define HCI_HDR_CMD             0x01
#define HCI_HDR_ACL             0x02
#define HCI_HDR_SCO             0x03
#define HCI_HDR_EVT             0x04

#define IS_H4_CMD(type)         ((type) == HCI_HDR_CMD) 
#define IS_H4_EVT(type)         ((type) == HCI_HDR_EVT) 

#pragma pack(push, 1)

typedef struct
{
    uint8_t                 packet_type;    /* HCI packet type(cmd). */
    HCI_COMMAND_HEADER_T    hci_command;    /* HCI command. */
} H4_COMMAND_HEADER_T;

typedef struct
{
    uint8_t                 packet_type;    /* HCI packet type(cmd). */
    HCI_COMMAND_GENERIC_T   hci_command;    /* HCI command. */
} H4_COMMAND_T;

typedef struct
{
    uint8_t                 packet_type;    /* HCI packet type(evt). */
    HCI_EVENT_HEADER_T      hci_event;      /* HCI event. */
} H4_EVENT_HEADER_T;

typedef struct
{
    uint8_t                 packet_type;    /* HCI packet type(evt). */
    HCI_EV_GENERIC_T        hci_event;      /* HCI event. */
} H4_EVENT_T;

#pragma pack(pop)

#define H4_COMMAND_SIZE(cmd)    (sizeof(H4_COMMAND_HEADER_T) + (((H4_COMMAND_HEADER_T *)cmd)->hci_command.length))

#define H4_EVENT_SIZE(evt)      (sizeof(H4_EVENT_HEADER_T) + (((H4_EVENT_HEADER_T *)evt)->hci_event.length))

#define MIN_H4_CMD_SIZE         (sizeof(H4_COMMAND_HEADER_T))   /* 4 byte */

#define MIN_H4_EVT_SIZE         (sizeof(H4_EVENT_HEADER_T))     /* 3 byte */


#ifdef __cplusplus
}
#endif

#endif  /* _BT_H4_H_ */