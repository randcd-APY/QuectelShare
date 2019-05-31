#ifndef _CONNX_APP_TRANSPORT_H_
#define _CONNX_APP_TRANSPORT_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


ConnxHandle ConnxAppTransportOpen();

void ConnxAppTransportClose(ConnxHandle mainTransportHandle);

ConnxHandle ConnxAppGetBasTransport(ConnxHandle mainTransportHandle);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_APP_TRANSPORT_H_ */