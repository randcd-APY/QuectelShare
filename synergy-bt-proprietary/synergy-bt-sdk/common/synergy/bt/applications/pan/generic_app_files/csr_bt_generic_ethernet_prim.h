#ifndef CSR_BT_GENERIC_ETHERNET_PRIM_H__
#define CSR_BT_GENERIC_ETHERNET_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_bsl_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OWN_ETHERNET_ADDRESS_0        0x0002
#define OWN_ETHERNET_ADDRESS_1        0x5B01
#define OWN_ETHERNET_ADDRESS_2        0x6C15
#define OWN_ETHERNET_ADDRESS_0_NET    0x0200
#define OWN_ETHERNET_ADDRESS_1_NET    0x015B
#define OWN_ETHERNET_ADDRESS_2_NET    0x156C

#define ETHERNET_PRIM            CSR_BT_BSL_PRIM
#define ETHERNET_PACKET_IND        CSR_BT_BSL_DATA_IND

#define ETHERNET_PACKET    CsrBtBslDataInd

#define ALLOCATE_ETHERNET_PACKET_REQUEST(packet, length)    (packet) = (ETHERNET_PACKET *) CsrPmemAlloc(sizeof(ETHERNET_PACKET));\
                                                            (packet)->payload = (CsrUint8 *) CsrPmemAlloc((length) + ((length) & 0x1))
#define ALLOCATE_ETHERNET_PACKET_INDICATION(packet, length)    (packet) = (ETHERNET_PACKET *) CsrPmemAlloc(sizeof(ETHERNET_PACKET));\
                                                            (packet)->payload = (CsrUint8 *) CsrPmemAlloc((length) + ((length) & 0x1))
#define SEND_ETHERNET_PACKET(packet)                        (packet)->type = CSR_BT_BSL_DATA_REQ;\
                                                            (packet)->id = CSR_BT_BSL_ID_DEST_ADDR;\
                                                            CsrSchedMessagePut(CSR_BT_BSL_IFACEQUEUE, CSR_BT_BSL_PRIM, (packet))
#define FREE_ETHERNET_PACKET_REQUEST(packet)                CsrPmemFree((void *) (packet)->payload);\
                                                            CsrPmemFree((packet))
#define FREE_ETHERNET_PACKET_INDICATION(packet)                CsrPmemFree((packet)->payload);\
                                                            CsrPmemFree((packet))
#define ETHERNET_SET_LENGTH(packet, setLength)                (packet)->length = (setLength)
#define ETHERNET_GET_LENGTH(packet)                            (packet)->length
#define ETHERNET_SET_DESTINATION(packet, destination0, destination1, destination2)    \
                                                            ((CsrUint16 *) (packet)->dstAddr.w)[0] = destination0;\
                                                            ((CsrUint16 *) (packet)->dstAddr.w)[1] = destination1;\
                                                            ((CsrUint16 *) (packet)->dstAddr.w)[2] = destination2
#define ETHERNET_GET_DESTINATION0(packet)                    ((CsrUint16 *) (packet)->dstAddr.w)[0]
#define ETHERNET_GET_DESTINATION1(packet)                    ((CsrUint16 *) (packet)->dstAddr.w)[1]
#define ETHERNET_GET_DESTINATION2(packet)                    ((CsrUint16 *) (packet)->dstAddr.w)[2]
#define ETHERNET_SET_SOURCE(packet, source0, source1, source2)    \
                                                            ((CsrUint16 *) (packet)->srcAddr.w)[0] = source0;\
                                                            ((CsrUint16 *) (packet)->srcAddr.w)[1] = source1;\
                                                            ((CsrUint16 *) (packet)->srcAddr.w)[2] = source2
#define ETHERNET_GET_SOURCE0(packet)                        ((CsrUint16 *) (packet)->srcAddr.w)[0]
#define ETHERNET_GET_SOURCE1(packet)                        ((CsrUint16 *) (packet)->srcAddr.w)[1]
#define ETHERNET_GET_SOURCE2(packet)                        ((CsrUint16 *) (packet)->srcAddr.w)[2]
#define ETHERNET_SET_TYPE(packet, type)                        (packet)->etherType = type
#define ETHERNET_GET_TYPE(packet)                            (packet)->etherType
#define ETHERNET_DATA(packet)                                ((CsrUint16 *) (packet)->payload)

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_ETHERNET_PRIM_H__ */
