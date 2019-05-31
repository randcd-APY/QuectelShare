/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "qca_h4_transport.h"
#include "bt_bootstrap_util.h"


static HandleH4PacketFunc s_h4PacketHandler = NULL;


static uint32_t HandleH4DataReceived(const uint8_t *data, uint32_t dataLength)
{
    HCI_EV_GENERIC_T hci_generic_event;
    const uint8_t *ptr;
    uint32_t size_remained = dataLength;
    uint32_t size_parsed = 0;
    uint32_t cur_event_size = 0;
    uint32_t offset = 0;
    int i;

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

        if (s_h4PacketHandler != NULL)
        {
            s_h4PacketHandler(ptr, dataLength);
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

bool QCA_OpenH4Transport(char *device, uint32_t baudrate, HandleH4PacketFunc h4PacketHandler)
{
    H4RegisterInfo ri;
    H4RegisterInfo *registerInfo = &ri;
    HciTransportSetting *transportSetting = &registerInfo->transportSetting;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    memset(registerInfo, 0, sizeof(H4RegisterInfo));

    registerInfo->size = sizeof(H4RegisterInfo);

    transportSetting->device         = device;
    transportSetting->init_baudrate  = baudrate; /* QCA chip has been booted up through synergy bt_bootstrap. */
    transportSetting->reset_baudrate = baudrate;
    transportSetting->flow_control   = true;

    registerInfo->rxDataFn = HandleH4DataReceived;

    registerInfo->btsnoop_file_name = NULL;

    s_h4PacketHandler = h4PacketHandler;

    return H4_OpenConnection(registerInfo);  
}

void QCA_CloseH4Transport(void)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));
    
    H4_CloseConnection();
}

int QCA_SendHciCommand(uint8_t *buf, int totalSize)
{
    uint16_t op_code;
    uint32_t length;
    char *data;
    int res = 0;

    if (totalSize >= MIN_H4_CMD_SIZE)
    {
        op_code = CONNX_GET_UINT16_FROM_LITTLE_ENDIAN(&buf[1]);

        length = buf[3];

        data = (totalSize > MIN_H4_CMD_SIZE) ? &buf[4] : NULL;

        res = H4_SendCommand(op_code, length, data);

        return res ? totalSize : 0;
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid H4 command"), __FUNCTION__));
        return 0;
    }
}
