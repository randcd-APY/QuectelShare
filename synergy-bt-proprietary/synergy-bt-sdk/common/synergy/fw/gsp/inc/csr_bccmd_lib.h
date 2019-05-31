#ifndef CSR_BCCMD_LIB_H__
#define CSR_BCCMD_LIB_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_bccmd_prim.h"
#include "csr_msg_transport.h"
#include "csr_bccmd_task.h"
#include "csr_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdWriteReqSend
 *
 *  DESCRIPTION
 *      This function build and send a SETREQ bccmd message with the structure
 *
 *         +---------+---------+---------+---------+---------+----------------+
 *         |  type   |  length |  seqNo  |  varId  |  status |    payload     |
 *         +---------+---------+---------+---------+---------+----------------+
 *
 * uint16s |    1    |    2    |    3    |    4    |    5    |                |
 *         +---------+---------+---------+---------+---------+----------------+
 *
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      varId         : The identifier of the variable being manipulated by
 *                      the message.
 *      seqNo         : This value in returned in the confirmed message
 *      payloadLength : The length of the message payload field measured in
 *                      8-bit integers. Note that the payload length must be
 *                      an even number.
 *      *payload      : The payload field of the BCCMD message. The payload
 *                      must in XAP format. E.g. 8-bit integers travel as 16-bit
 *                      integers, 16-bit integers travel with the less significant
 *                      byte first, as illustrated below
 *
 *                                  +--------------+--------------+
 *                                  |      LSB     |      MSB     |
 *                                  +--------------+--------------+
 *
 *                            bytes |       1      |       2      |
 *                                  +--------------+--------------+
 *
 *                         CsrUint16 |              1              |
 *                                  +-----------------------------+
 *
 *                      and 32-bit integers travel as illustrated below
 *
 *                              +--------------+--------------+--------------+--------------+
 *                              |    Byte 3    |    Byte 4    |     Byte 1   |     Byte 2   |
 *                              |              |     MSB      |      LSB     |              |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                        bytes |       1      |       2      |       3      |        4     |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                      uint16s |              1              |              2              |
 *                              +-----------------------------+-----------------------------+
 *
 *                      Macros for converting these integers types to XAP format
 *                      is defined in csr_macro.h
 *----------------------------------------------------------------------------*/
void CsrBccmdWriteReqSend(CsrSchedQid phandle,
    CsrUint16 varId,
    CsrUint16 seqNo,
    CsrUint16 payloadLength,
    CsrUint8 *payload);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdReadReqSend
 *
 *  DESCRIPTION
 *      This function build and send a GETREQ bccmd message with the structure
 *
 *         +---------+---------+---------+---------+---------+----------------+
 *         |  type   |  length |  seqNo  |  varId  |  status |    payload     |
 *         +---------+---------+---------+---------+---------+----------------+
 *
 * uint16s |    1    |    2    |    3    |    4    |    5    |                |
 *         +---------+---------+---------+---------+---------+----------------+
 *
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      varId         : The identifier of the variable being manipulated by
 *                      the message.
 *      seqNo         : This value in returned in the confirmed message
 *      payloadLength : The length of the message payload field measured in
 *                      8-bit integers. Note that the payload length must be
 *                      an even number and that payload must carries an empty
 *                      payload of a size suitable for holding the variable's
 *                      being read
 *      *payload      : The payload field of the BCCMD message. The payload
 *                      must in XAP format. E.g. 8-bit integers travel as 16-bit
 *                      integers, 16-bit integers travel with the less significant
 *                      byte first, as illustrated below
 *
 *                                  +--------------+--------------+
 *                                  |      LSB     |      MSB     |
 *                                  +--------------+--------------+
 *
 *                            bytes |       1      |       2      |
 *                                  +--------------+--------------+
 *
 *                           CsrUint16 |              1              |
 *                                  +-----------------------------+
 *
 *                      and 32-bit integers travel as illustrated below
 *
 *                              +--------------+--------------+--------------+--------------+
 *                              |    Byte 3    |    Byte 4    |     Byte 1   |     Byte 2   |
 *                              |              |     MSB      |      LSB     |              |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                        bytes |       1      |       2      |       3      |        4     |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                      uint16s |              1              |              2              |
 *                              +-----------------------------+-----------------------------+
 *
 *                      Macros for converting these integers types to XAP format
 *                      is defined in csr_macro.h
 *----------------------------------------------------------------------------*/
void CsrBccmdReadReqSend(CsrSchedQid phandle,
    CsrUint16 varId,
    CsrUint16 seqNo,
    CsrUint16 payloadLength,
    CsrUint8 *payload);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdWritePsValueReqSend
 *
 *  DESCRIPTION
 *      This function build and send a SETREQ bccmd message with VarId 0x7003.
 *      E.g. a persistent store command with the structure
 *
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *         |  type   |  length |  seqNo  |  varId  |  status |   key   | length  |  stores |    psValue   |
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *
 * uint16s |    1    |    2    |    3    |    4    |    5    |    6    |    7    |    8    |              |
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      seqNo         : This value in returned in the confirmed message
 *      key           : The identifier of the PS Key of the database element
 *                      being accessed.
 *      stores        : Controls the searching of the four component stores
 *                      that make up the fullPersistent Store. Stores field
 *                      values are defined in csr_bccmd_prim.h
 *      psValuelength : The psValuelength of the PS Value field measured in
 *                      8-bit integers. Note that the psValue length must be
 *                      an even number
 *      *psValue      : The psValue field of the BCCMD message. The psValue
 *                      must in XAP format. E.g. 8-bit integers travel as 16-bit
 *                      integers, 16-bit integers travel with the less significant
 *                      byte first, as illustrated below
 *
 *                                  +--------------+--------------+
 *                                  |      LSB     |      MSB     |
 *                                  +--------------+--------------+
 *
 *                            bytes |       1      |       2      |
 *                                  +--------------+--------------+
 *
 *                           CsrUint16 |              1              |
 *                                  +-----------------------------+
 *
 *                      and 32-bit integers travel as illustrated below
 *
 *                              +--------------+--------------+--------------+--------------+
 *                              |    Byte 3    |    Byte 4    |     Byte 1   |     Byte 2   |
 *                              |              |     MSB      |      LSB     |              |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                        bytes |       1      |       2      |       3      |        4     |
 *                              +--------------+--------------+--------------+--------------+
 *
 *                      uint16s |              1              |              2              |
 *                              +-----------------------------+-----------------------------+
 *
 *                      Macros for converting these integers types to XAP format
 *                      is defined in csr_macro.h
 *----------------------------------------------------------------------------*/
void CsrBccmdWritePsValueReqSend(CsrSchedQid phandle,
    CsrUint16 seqNo,
    CsrUint16 key,
    CsrBccmdStoresType stores,
    CsrUint16 psValuelength,
    CsrUint8 *psValue);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdReadPsValueReqSend
 *
 *      This function build and send a GETREQ bccmd message with VarId 0x7003.
 *      E.g. a persistent store command with the structure
 *
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *         |  type   |  length |  seqNo  |  varId  |  status |   key   | length  |  stores |    psValue   |
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *
 * uint16s |    1    |    2    |    3    |    4    |    5    |    6    |    7    |    8    |              |
 *         +---------+---------+---------+---------+---------+---------+---------+---------+--------------+
 *
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      seqNo         : This value in returned in the confirmed message
 *      key           : The identifier of the PS Key of the database element
 *                      being accessed.
 *      stores        : Controls the searching of the four component stores
 *                      that make up the fullPersistent Store. Stores field
 *                      values are defined in csr_bccmd_prim.h
 *      psValuelength : The psValuelength of the PS Value field measured in
 *                      8-bit integers. Note that the psValuelength must be
 *                      an even number and it must be set to the maximum
 *                      length of the data that can be taken from persistent
 *                      Store in this operation
 *----------------------------------------------------------------------------*/
void CsrBccmdReadPsValueReqSend(CsrSchedQid phandle,
    CsrUint16 seqNo,
    CsrUint16 key,
    CsrBccmdStoresType stores,
    CsrUint16 psValuelength);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdReqSend
 *
 *  DESCRIPTION
 *      Send a Bluecore command (bccmd).
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      payloadLength : The total length of the message measured in 8-bit integers
 *      *payload      : The BCCMD message given as a byte string
 *----------------------------------------------------------------------------*/
CsrBccmdReq *CsrBccmdReq_struct(CsrSchedQid phandle, CsrUint16 payloadLength, CsrUint8 *payload);

#define CsrBccmdReqSend(_ph, _pl, _p){ \
        CsrBccmdReq *msg__; \
        msg__ = CsrBccmdReq_struct(_ph, _pl, _p); \
        CsrMsgTransport(CSR_BCCMD_IFACEQUEUE, CSR_BCCMD_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdFlushReqSend
 *
 *  DESCRIPTION
 *      Request to flush any queued bccmd from given handle
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrBccmdFlushReq *CsrBccmdFlushReq_struct(CsrSchedQid phandle);
#define CsrBccmdFlushReqSend(_ph){            \
        CsrBccmdFlushReq *msg__; \
        msg__ = CsrBccmdFlushReq_struct(_ph); \
        CsrMsgTransport(CSR_BCCMD_IFACEQUEUE, CSR_BCCMD_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdPsKeyCommandBuild
 *
 *  DESCRIPTION
 *      Send a Bluecore command (PS key).
 *  PARAMETERS
 *      type            : CSR_BCCMD_GET_REQ or CSR_BCCMD_SET_REQ
 *      seqNo           : Sequence number
 *      key             : Identifier of the PS Key of the database
 *                        element being accessed
 *      stores          : One of the CSR_BCCMD_STORES_* values from
 *                        csr_bccmd_prim.h
 *      psValueLength   : Length of the buffer pointed to by psValue
 *      psValue         : Pointer to a buffer holding the PS key value
 *      bccmdLength     : Pointer to a CsrUint16 to which the size
 *                        of the resulting bccmd is written
 *
 *----------------------------------------------------------------------------*/
#define CSR_BCCMD_GET_REQ                   ((CsrUint16) 0x0000)
#define CSR_BCCMD_SET_REQ                   ((CsrUint16) 0x0002)

CsrUint8 *CsrBccmdPsKeyCommandBuild(CsrUint16 type,
    CsrUint16 seqNo,
    CsrUint16 key,
    CsrBccmdStoresType stores,
    CsrUint16 psValueLength,
    CsrUint8 *psValue,
    CsrUint16 *bccmdLength);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBccmdFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR BCCMD upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BCCMD_PRIM,
 *      msg:          The message received from CSR BCCMD
 *----------------------------------------------------------------------------*/
void CsrBccmdFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
