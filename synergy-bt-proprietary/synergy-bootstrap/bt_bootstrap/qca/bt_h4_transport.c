/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #3 $
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "bt_h4_transport.h"
#include "connx_log.h"
#include "connx_log_btsnoop.h"


typedef struct
{
    void                   *uart_handle;        /* Handle for UART device. */
    uint8_t                 reset;              /* 0: Open UART with initial baudrate, 1: with new reset baudrate. */
    ConnxHandle             btsnoop_handle;     /* Handle for BT snoop log. */
} H4Instance;


static H4Instance h4_inst = { NULL, 0, NULL };


static bool IsValidH4RegisterInfo(H4RegisterInfo *registerInfo);
static uint32_t CalculateH4PacketLength(hci_op_code_t op_code, uint8_t length, void *data);
static bool IsValidH4EventPacket(const uint8_t *buffer, uint32_t buffer_size);


static bool IsValidH4RegisterInfo(H4RegisterInfo *registerInfo)
{
    return ((registerInfo != NULL) &&
            (registerInfo->size == sizeof(H4RegisterInfo)) &&
            (registerInfo->rxDataFn != NULL)) ? true : false;
}

static uint32_t CalculateH4PacketLength(hci_op_code_t op_code, uint8_t length, void *data)
{
    CONNX_UNUSED(op_code);
    CONNX_UNUSED(data);

    return sizeof(H4_COMMAND_HEADER_T) + length;
}

static bool IsValidH4EventPacket(const uint8_t *buffer, uint32_t buffer_size)
{
    H4_EVENT_HEADER_T *packet = (H4_EVENT_HEADER_T *)buffer;
    HCI_EVENT_HEADER_T *hci_event;
    uint8_t packet_type;

    if (!buffer || (buffer_size < MIN_H4_EVT_SIZE))
        return false;

    packet_type = packet->packet_type;
    hci_event   = &packet->hci_event;

    if (!IS_H4_EVT(packet_type))
        return false;

    if (buffer_size < (MIN_H4_EVT_SIZE + hci_event->length))
        return false;

    return true;
}

bool H4_OpenConnection(H4RegisterInfo *registerInfo)
{
    HciTransportSetting *transportSetting;
    ConnxUartDrvDataRx rxDataFn;
    H4Instance *inst = &h4_inst;
    void *uart_handle;
    uint8_t reset = inst->reset;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!IsValidH4RegisterInfo(registerInfo))
        return false;

    transportSetting = &registerInfo->transportSetting;
    rxDataFn = registerInfo->rxDataFn;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Initialize UART driver"), __FUNCTION__));

    uart_handle = ConnxUartDrvInitialise(HTS_GET_TRANSPORT_DEVICE(transportSetting),
                                         HTS_GET_INIT_BAUDRATE(transportSetting),
                                         HTS_GET_RESET_BAUDRATE(transportSetting),
                                         CONNX_UART_DRV_DATA_BITS_8,
                                         CONNX_UART_DRV_PARITY_NONE,
                                         CONNX_UART_DRV_STOP_BITS_1,
                                         HTS_GET_FLOW_CONTROL(transportSetting),
                                         NULL);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Register UART driver"), __FUNCTION__));

    ConnxUartDrvRegister(uart_handle, rxDataFn, INVALID_BGINT_HANDLE);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Start UART driver, reset: %x"), __FUNCTION__, reset));

    if (!ConnxUartDrvStart(uart_handle, reset))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Fail to start UART driver"), __FUNCTION__));

        ConnxUartDrvDeinitialise(uart_handle);
        return false;
    }

    inst->uart_handle = uart_handle;

#ifdef ENABLE_BTSNOOP_LOG
    if (!inst->reset)
    {
        inst->btsnoop_handle = BtsnoopCreate(registerInfo->btsnoop_file_name);

        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> btsnoop_handle: %p, btsnoop_file_name: %s"),
                       __FUNCTION__, inst->btsnoop_handle, registerInfo->btsnoop_file_name));
    }
#endif

    inst->reset = reset ? 0 : 1;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> uart_handle: %p, new reset: %x"),
                   __FUNCTION__, inst->uart_handle, inst->reset));

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Done"), __FUNCTION__));

    return true;
}

void H4_CloseConnection(void)
{
    H4Instance *inst = &h4_inst;
    void *uart_handle;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

#ifdef ENABLE_BTSNOOP_LOG
    if (!inst->reset)
    {
        IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> btsnoop_handle: %p"),
                       __FUNCTION__, inst->btsnoop_handle));

        BtsnoopDestroy(inst->btsnoop_handle);
        inst->btsnoop_handle = NULL;
    }
#endif

    uart_handle = inst->uart_handle;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Stop UART driver"), __FUNCTION__));

    ConnxUartDrvStop(uart_handle);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Deinitialize UART driver"), __FUNCTION__));

    ConnxUartDrvDeinitialise(uart_handle);
}

bool H4_SendCommand(hci_op_code_t op_code, uint8_t length, void *data)
{
    H4Instance *inst = &h4_inst;
    void *uart_handle;
    H4_COMMAND_HEADER_T *packet;
    HCI_COMMAND_HEADER_T *hci_command;
    uint32_t total_length = 0;
    uint32_t size_written = 0;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> op_code: 0x%04x, length: 0x%02x"), __FUNCTION__, op_code, length));

    uart_handle = inst->uart_handle;

    total_length = CalculateH4PacketLength(op_code, length, data);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> total_length: 0x%02x"), __FUNCTION__, total_length));

    packet = (H4_COMMAND_HEADER_T *)malloc(total_length);

    if (!packet)
        return false;

    packet->packet_type = HCI_HDR_CMD;

    hci_command = &packet->hci_command;
    hci_command->op_code = op_code;
    hci_command->length  = length;

    if (length && data)
    {
        uint8_t *tmp_buf = ((uint8_t *)packet) + sizeof(H4_COMMAND_HEADER_T);

        memcpy(tmp_buf, data, length);
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Tx data, total_length: %d"), __FUNCTION__, total_length));

#ifdef ENABLE_BTSNOOP_LOG
    BtsnoopWriteRecord(inst->btsnoop_handle, false, ((uint8_t *) packet) + 1, total_length - 1);
#endif

    IFLOG(DumpBuff(DEBUG_OUTPUT, (const uint8_t *)packet, total_length));

    res = ConnxUartDrvTx(uart_handle,
                         (const uint8_t *)packet,
                         total_length,
                         &size_written);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s, written: 0x%02x"), __FUNCTION__, res ? "succeed" : "fail", size_written));

    free(packet);

    return res;
}

bool H4_SendVendorCommand(uint16_t ocf, uint8_t length, void *data)
{
    hci_op_code_t op_code = HCI_VENDOR_CMD(ocf);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> op_code: 0x%04x, length: 0x%02x"), __FUNCTION__, op_code, length));

    return H4_SendCommand(op_code, length, data);
}

bool H4_SendResetCommand(void)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    return H4_SendCommand(HCI_RESET, 0, NULL);
}

bool H4_SendReadBdAddrCommand(void)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    return H4_SendCommand(HCI_READ_BD_ADDR, 0, NULL);
}

bool H4_ParseEvent(const uint8_t *buffer, uint32_t buffer_size, HCI_EV_GENERIC_T *hci_generic_event)
{
    H4_EVENT_HEADER_T *packet = (H4_EVENT_HEADER_T *)buffer;
    HCI_EVENT_HEADER_T *hci_event_header;
    uint32_t actual_size;
#ifdef ENABLE_BTSNOOP_LOG
    H4Instance *inst = &h4_inst;
#endif

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> buffer_size: %d"), __FUNCTION__, buffer_size));

    if (!IsValidH4EventPacket(buffer, buffer_size) || !hci_generic_event)
    {
        /* Maybe in-complete H4 packet, which need to be parsed by upper layer. */
        return false;
    }

    hci_event_header = &packet->hci_event;

    actual_size = CONNX_MIN(MIN_H4_EVT_SIZE + hci_event_header->length, buffer_size - 1);

    memcpy(hci_generic_event, hci_event_header, actual_size);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Valid H4 event packet"), __FUNCTION__));

#ifdef ENABLE_BTSNOOP_LOG
    BtsnoopWriteRecord(inst->btsnoop_handle, true, buffer + 1, buffer_size - 1);
#endif

    return true;
}
