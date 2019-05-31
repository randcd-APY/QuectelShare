/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _BT_H4_TRANSPORT_H_
#define _BT_H4_TRANSPORT_H_

#include "bt_h4.h"
#include "connx_serial_com.h"
#include "bt_bootstrap_util.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int                     size;               /* Total size in byte for register information. */
    HciTransportSetting     transportSetting;   /* HCI transport setting. */
    ConnxUartDrvDataRx      rxDataFn;           /* Handler for data received in H4 transport. */
    char                   *btsnoop_file_name;  /* Full file name for BT snoop log. */
} H4RegisterInfo;


bool H4_OpenConnection(H4RegisterInfo *registerInfo);

void H4_CloseConnection(void);

bool H4_SendCommand(hci_op_code_t op_code, uint8_t length, void *data);
bool H4_SendVendorCommand(uint16_t ocf, uint8_t length, void *data);
bool H4_SendResetCommand(void);
bool H4_SendReadBdAddrCommand(void);

bool H4_ParseEvent(const uint8_t *buffer, uint32_t buffer_size, HCI_EV_GENERIC_T *hci_generic_event);


#ifdef __cplusplus
}
#endif

#endif  /* _BT_H4_TRANSPORT_H_ */