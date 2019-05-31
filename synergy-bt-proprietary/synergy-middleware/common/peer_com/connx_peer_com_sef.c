/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_common_def.h"
#include "connx_peer_com_sef.h"
#include "connx_peer_com_main.h"
#include "connx_app_transport.h"
#include "connx_app_bas_transport.h"

static ConnxHandle GetMainTransportHandle();

static ConnxHandle GetMainTransportHandle()
{
    ConnxPeerComInstance *inst = GetPeerComInst();

    return PCI_GET_MAIN_TRANSPORT_HANDLE(inst);
}

/* Send bt audio service client data to bt audio service server. */
void connx_app_bas_transport_send(void *data, size_t size)
{
    ConnxHandle mainTransportHandle = GetMainTransportHandle();
    ConnxHandle handle = ConnxAppGetBasTransport(mainTransportHandle);

    ConnxAppBasTransportSend(handle, data, size);
}

