#ifndef CSR_LIB_H__
#define CSR_LIB_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    CsrPrim type;
} CsrMessage;

/*----------------------------------------------------------------------------*
 *  CsrMessage_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessage
 *
 *----------------------------------------------------------------------------*/
CsrMessage *CsrMessage_struct(CsrUint16 primtype, CsrUint16 msgtype);

typedef struct
{
    CsrPrim  type;
    CsrUint8 value;
} CsrMessageCsrUint8;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint8_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint8
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint8 *CsrMessageCsrUint8_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint8 value);

typedef struct
{
    CsrPrim   type;
    CsrUint16 value;
} CsrMessageCsrUint16;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint16_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint16
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint16 *CsrMessageCsrUint16_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint16 value);

typedef struct
{
    CsrPrim   type;
    CsrUint16 value1;
    CsrUint8  value2;
} CsrMessageCsrUint16CsrUint8;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint16CsrUint8_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint16CsrUint8
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint16CsrUint8 *CsrMessageCsrUint16CsrUint8_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint16 value1, CsrUint8 value2);

typedef struct
{
    CsrPrim   type;
    CsrUint16 value1;
    CsrUint16 value2;
} CsrMessageCsrUint16CsrUint16;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint16CsrUint16_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint16CsrUint16
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint16CsrUint16 *CsrMessageCsrUint16CsrUint16_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint16 value1, CsrUint16 value2);

typedef struct
{
    CsrPrim   type;
    CsrUint16 value1;
    CsrUint32 value2;
} CsrMessageCsrUint16CsrUint32;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint16_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint16
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint16CsrUint32 *CsrMessageCsrUint16CsrUint32_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint16 value1, CsrUint32 value2);

typedef struct
{
    CsrPrim        type;
    CsrUint16      value1;
    CsrCharString *value2;
} CsrMessageCsrUint16CsrCharString;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint16CsrCharString_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint16CsrCharString
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint16CsrCharString *CsrMessageCsrUint16CsrCharString_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint16 value1, CsrCharString *value2);

typedef struct
{
    CsrPrim   type;
    CsrUint32 value;
} CsrMessageCsrUint32;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint32_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint32
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint32 *CsrMessageCsrUint32_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint32 value);

typedef struct
{
    CsrPrim   type;
    CsrUint32 value1;
    CsrUint16 value2;
} CsrMessageCsrUint32CsrUint16;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint32CsrUint16_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint32CsrUint16
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint32CsrUint16 *CsrMessageCsrUint32CsrUint16_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint32 value1, CsrUint32 value2);

typedef struct
{
    CsrPrim        type;
    CsrUint32      value1;
    CsrCharString *value2;
} CsrMessageCsrUint32CsrCharString;

/*----------------------------------------------------------------------------*
 *  CsrMessageCsrUint32CsrCharString_struct
 *
 *  DESCRIPTION
 *      Generic message creator.
 *      Allocates and fills in a message with the signature CsrMessageCsrUint32CsrCharString
 *
 *----------------------------------------------------------------------------*/
CsrMessageCsrUint32CsrCharString *CsrMessageCsrUint32CsrCharString_struct(CsrUint16 primtype, CsrUint16 msgtype, CsrUint32 value1, CsrCharString *value2);

#ifdef __cplusplus
}
#endif

#endif /* CSR_LIB_H__ */
