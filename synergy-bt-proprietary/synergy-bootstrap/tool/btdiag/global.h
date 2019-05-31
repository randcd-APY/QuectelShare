/*
 * Copyright (c) 2011-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef QCMBR
#define QCMBR 1
#endif

#define TCP_BUF_LENGTH         256
#define HCI_MAX_EVENT_SIZE     260
#define HCI_CMD_IND            (1)
#define HCI_COMMAND_HDR_SIZE   3

typedef char BYTE;

typedef enum
{
    SERIAL,
    USB,
    INVALID
} ConnectionType;

#endif  /* GLOBAL_H_ */
