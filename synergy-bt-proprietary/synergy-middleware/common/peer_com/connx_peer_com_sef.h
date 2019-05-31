#ifndef _CONNX_PEER_COM_SEF_H_
#define _CONNX_PEER_COM_SEF_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Send BAS data to bt audio service. */

void connx_app_bas_transport_send(void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_PEER_COM_SEF_H_ */
