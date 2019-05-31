/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _BLUESTACK_ATT_PRIM_H_
#define _BLUESTACK_ATT_PRIM_H_

#include "csr_synergy.h"

#include "hci_prim.h"
#include "csr_bt_bluestack_types.h"
#include "l2cap_prim.h"


/* search_string="att_prim_t" */
/* conversion_rule="ADD_UNDERSCORE_AND_UPPERCASE_T" */

#ifdef __cplusplus
extern "C" {
#endif

/*!
    \name Response/result error and status codes

    \{
*/
/*! Operation was successful */
#define ATT_RESULT_SUCCESS                      0x0000
/*! The attribute handle given was not valid */
#define ATT_RESULT_INVALID_HANDLE               0x0001
/*! The attribute cannot be read */
#define ATT_RESULT_READ_NOT_PERMITTED           0x0002
/*! The attribute cannot be written */
#define ATT_RESULT_WRITE_NOT_PERMITTED          0x0003
/*! The attribute PDU was invalid */
#define ATT_RESULT_INVALID_PDU                  0x0004
/*! The attribute requires an authentication before it can be read or
    written */
#define ATT_RESULT_INSUFFICIENT_AUTHENTICATION  0x0005
/*! Target device doesn't support request */
#define ATT_RESULT_REQUEST_NOT_SUPPORTED        0x0006
/*! Offset specified was past the end of the long attribute */
#define ATT_RESULT_INVALID_OFFSET               0x0007
/*! The attribute requires authorization before it can be read or written */
#define ATT_RESULT_INSUFFICIENT_AUTHORIZATION   0x0008
/*! Too many prepare writes have been queued */
#define ATT_RESULT_PREPARE_QUEUE_FULL           0x0009
/*! No attribute found within the given attribute handle range. */
#define ATT_RESULT_ATTR_NOT_FOUND               0x000a
/*! This attribute cannot be read or written using the Read Blob Request
    or Write Blob Requests. */
#define ATT_RESULT_NOT_LONG                     0x000b
/*! The Encryption Key Size used for encrypting this link is insufficient. */
#define ATT_RESULT_INSUFFICIENT_ENCR_KEY_SIZE   0x000c
/*! The attribute value length is invalid for the operation. */
#define ATT_RESULT_INVALID_LENGTH               0x000d
/*! The attribute request that was requested has encountered an error
    that was very unlikely, and therefore could not be completed as
    requested. */
#define ATT_RESULT_UNLIKELY_ERROR               0x000e
/*! The attribute requires encryption before it can be read or written */
#define ATT_RESULT_INSUFFICIENT_ENCRYPTION      0x000f
/*! The attribute type is not a supported grouping attribute as defined by a
    higher layer specification. */
#define ATT_RESULT_UNSUPPORTED_GROUP_TYPE       0x0010
/*! Insufficient Resources to complete the request. */
#define ATT_RESULT_INSUFFICIENT_RESOURCES       0x0011
/*! Mask for application error codes. Application error codes are defined by
    a higher layer specification. */
#define ATT_RESULT_APP_MASK                     0x0080


/*! Error to indicate that request to DM can not be completed becuase device ACL entity is not found */
#define ATT_RESULT_DEVICE_NOT_FOUND             0xff73
/*! Attribute signing failed. */
#define ATT_RESULT_SIGN_FAILED                  0xff74
/*! Operation can't be done now. */
#define ATT_RESULT_BUSY                         0xff75
/*! Current operation timed out. */
#define ATT_RESULT_TIMEOUT                      0xff76
/*! Invalid MTU */
#define ATT_RESULT_INVALID_MTU                  0xff77
/*! Invalid UUID type */
#define ATT_RESULT_INVALID_UUID                 0xff78
/*! Operation was successful, and more responses will follow */
#define ATT_RESULT_SUCCESS_MORE                 0xff79
/*! Indication sent, awaiting confirmation from the client */
#define ATT_RESULT_SUCCESS_SENT                 0xff7a
/*! Invalid connection identifier */
#define ATT_RESULT_INVALID_CID                  0xff7b
/*! Attribute database is invalid */
#define ATT_RESULT_INVALID_DB                   0xff7c
/*! Attribute server database is full */
#define ATT_RESULT_DB_FULL                      0xff7d
/*! Requested server instance is not valid */
#define ATT_RESULT_INVALID_PHANDLE              0xff7e
/*! Attribute permissions are not valid */
#define ATT_RESULT_INVALID_PERMISSIONS          0xff7f
/*! Signed write done on an encrypted link */
#define ATT_RESULT_SIGNED_DISALLOWED            0xff80

#ifdef BUILD_FOR_HOST_FOR_ENCRYPTION_ATT_RACE
/*! It indicates that encryption is in process */
#define ATT_RESULT_ENCRYPTION_PENDING           0xff81
#endif /* BUILD_FOR_HOST_FOR_ENCRYPTION_ATT_RACE */
/*! \} */

/*!
    \name Generic Attribute Profile properties

    \brief The Characteristic Properties bit field determines how the
    Characteristic Value can be used, or how the characteristic descriptors
    can be accessed.

    \{
*/
/* FIXME: change name to PROP */
/*! If set, permits broadcasts of the Characteristic Value using
  Characteristic Configuration Descriptor. */
#define ATT_PERM_CONFIGURE_BROADCAST            0x01
/*! If set, permits reads of the Characteristic Value. */
#define ATT_PERM_READ                           0x02
/*! If set, permit writes of the Characteristic Value without response. */
#define ATT_PERM_WRITE_CMD                      0x04
/*! If set, permits writes of the Characteristic Value with response. */
#define ATT_PERM_WRITE_REQ                      0x08
/*! If set, permits notifications of a Characteristic Value without
    acknowledgment. */
#define ATT_PERM_NOTIFY                         0x10
/*! If set, permits indications of a Characteristic Value with
    acknowledgment. */
#define ATT_PERM_INDICATE                       0x20
/*! If set, permits signed writes to the Characteristic Value. */
#define ATT_PERM_AUTHENTICATED                  0x40
/*! If set, additional characteristic properties are defined in the
    Characteristic Extended Properties Descriptor. */
#define ATT_PERM_EXTENDED                       0x80
/*! \} */

/*!
    \name Generic Attribute Profile extended properties

    \brief The Characteristic Extended Properties bit field describes
    additional properties on how the Characteristic Value can be used,
    or how the characteristic descriptors can be accessed.

    \{
*/
/*! If set, permits reliable writes of the Characteristic Value. */
#define ATT_PERM_RELIABLE_WRITE                 0x0001
/*! If set, permits writes to the characteristic descriptor. */
#define ATT_PERM_WRITE_AUX                      0x0002
/*! \} */

/* FIXME: can these be moved to att_private.h? */
#define ATT_FORMAT_UUID16                       0x01
#define ATT_FORMAT_UUID128                      0x02

/*!
    \name UUID Types

    \{
*/
/*! UUID Type */
typedef CsrUint16                                att_uuid_type_t;
/*! No UUID present. */
#define ATT_UUID_NONE                           0x0000
/*! UUID is a 16-bit Attribute UUID */
#define ATT_UUID16                              0x0001
/*! UUID is a 128-bit UUID */
#define ATT_UUID128                             0x0002
/*! UUID is a 32-bit Attribute UUID */
#define ATT_UUID32                              0x0003
/* \} */

/* Flat DB attribute types */
typedef enum att_type_tag
{
    /* GATT Attribute Types */
    att_type_pri_service,       /* 0 Service */
    att_type_sec_service,       /* 1 Secondary service */
    att_type_include,           /* 2 Include */
    att_type_declaration,       /* 3 Characteristic declaration */
    att_type_ch_extended,       /* 4 Characteristic Extended Properties */
    att_type_ch_descr,          /* 5 Characteristic User Description */
    att_type_ch_c_config,       /* 6 Client Characteristic Configuration */
    att_type_ch_s_config,       /* 7 Server Characteristic Configuration */
    att_type_ch_format,         /* 8 Characteristic Format */
    att_type_ch_agg,            /* 9 Characteristic Aggregate Format */
    att_type_value32,           /* a Characteristic value */
    att_type_full32,            /* b generic attribute */

    att_type_value128,          /* c Characteristic value */
    att_type_value,             /* d Characteristic value */

    /* full attribute data */
    att_type_full,              /* e generic attribute */
    att_type_full128            /* f generic attribute */
} att_type_t;

/*! \brief Full attribute type with 16-bit UUID (att_type_full) */
typedef struct
{
    CsrUint16    uuid;           /*!< UUID16 */
    CsrUint16    perm;           /*!< Attribute permissions */
    CsrUint16    data[1];        /*!< Attribute value */
} att_attr_full_t;

/*! \brief Full attribute type with 32-bit UUID (att_type_full32) */
typedef struct
{
    CsrUint32    uuid;        /*!< UUID32 */
    CsrUint16    perm;           /*!< Attribute permissions */
    CsrUint16    data[1];        /*!< Attribute value */
} att_attr_full32_t;

/*! \brief Full attribute type with 128-bit UUID (att_type_full128) */
typedef struct
{
    CsrUint32    uuid[4];        /*!< UUID128 */
    CsrUint16    perm;           /*!< Attribute permissions */
    CsrUint16    data[1];        /*!< Attribute value */
} att_attr_full128_t;

/*! \brief Attribute length can be changed. */
#define ATT_ATTR_DYNLEN         0x8
/*! \brief Access to the attribute sends ATT_ACCESS_IND to the application. */
#define ATT_ATTR_IRQ            0x4
/*!
    \brief Read access to the attribute sends ATT_ACCESS_IND to the
    application.

    This flag is only available on host version of ATT.
    ATT does not own the value, it will not be written into it's database
*/
#define ATT_ATTR_IRQ_R          0x10
/*!
    \brief Write access to the attribute sends ATT_ACCESS_IND to the
    application.

    This flag is only available on host version of ATT.
    ATT owns the value, database will be updated on a successful ATT_ACCESS_RSP
*/
#define ATT_ATTR_IRQ_W          0x20

#define ATT_ATTR_SEC_R_OFFS     6       /* 0x00c0 */
#define ATT_ATTR_SEC_W_OFFS     0       /* 0x0003 */
#define ATT_ATTR_SEC_MASK       0x3

/* macros for setting security flags */
#define ATT_ATTR_SEC_R_ENUM(value)      ((CsrUint16)((value) & ATT_ATTR_SEC_MASK) << ATT_ATTR_SEC_R_OFFS)
#define ATT_ATTR_SEC_W_ENUM(value)      ((CsrUint16)((value) & ATT_ATTR_SEC_MASK) << ATT_ATTR_SEC_W_OFFS)

/* macros for extracting security flags */
#define ATT_ATTR_SEC_R_EXTRACT_ENUM(value)      ((CsrUint16)((value) >> ATT_ATTR_SEC_R_OFFS) & ATT_ATTR_SEC_MASK)
#define ATT_ATTR_SEC_W_EXTRACT_ENUM(value)      ((CsrUint16)((value) >> ATT_ATTR_SEC_W_OFFS) & ATT_ATTR_SEC_MASK)

/*! \name Special security requirement options
  \{ */
typedef enum
{
    /*! \brief No security requirements */
    ATT_ATTR_SEC_NONE,
    /*! \brief Encrypted link is required for access. */
    ATT_ATTR_SEC_ENCRYPTION,
    /*! \brief Authenticated MITM protection is required for access. */
    ATT_ATTR_SEC_AUTHENTICATION,
    /*! \brief Secure connection paired authenticated MITM protection is
     *   required for access.
     */
    ATT_ATTR_SEC_SC_AUTHENTICATION,

    ATT_ATTR_SEC_RESERVED
} ATT_ATTR_SEC_T;
/*! \} */

/*! \brief Access to an attribute need Authorization from the application */
#define ATT_ATTR_AUTHORIZATION            0x0100
/*! \brief Access to an attribute will ask the application about the key requirements */
#define ATT_ATTR_ENC_KEY_REQUIREMENTS     0x0200
/*! \brief Access to an attribute cannot be done on LE Radio*/
#define ATT_ATTR_DISABLE_ACCESS_LE        0x0400
/*! \brief Access to an attribute cannot be done on BR/EDR Radio*/
#define ATT_ATTR_DISABLE_ACCESS_BR_EDR    0x0800

/*!
    \name Generic Attribute Profile Attribute types and Characteristic Descriptors.

    \{
*/
/*! Attribute type for Primary Service */
#define ATT_UUID_PRI_SERVICE                    0x2800
/*! Attribute type for Secondary Service */
#define ATT_UUID_SEC_SERVICE                    0x2801
/*! Attribute type for Include */
#define ATT_UUID_INCLUDE                        0x2802
/*! Attribute type for Characteristic */
#define ATT_UUID_CHARACTERISTIC                 0x2803
/*! Attribute type for Characteristic Extended Properties */
#define ATT_UUID_CH_EXTENDED                    0x2900
/*! Attribute type for Characteristic User Description*/
#define ATT_UUID_CH_DESCRIPTION                 0x2901
/*! Attribute type for Client Characteristic Configuration */
#define ATT_UUID_CH_C_CONFIG                    0x2902
/*! Attribute type for Server Characteristic Configuration */
#define ATT_UUID_CH_S_CONFIG                    0x2903
/*! Attribute type for Characteristic Format */
#define ATT_UUID_CH_FORMAT                      0x2904
/*! Attribute type for Characteristic Aggregate Format */
#define ATT_UUID_CH_AGG_FORMAT                  0x2905
/*! \} */

#define ATT_HANDLE_MIN          0x0001  /*!< Minimum handle value. */
#define ATT_HANDLE_MAX          0xffff  /*!< Maximum handle value. */

/*! \name ATT primitive segmentation and numbering

    \brief ATT primitives occupy the number space from
    ATT_PRIM_BASE to (ATT_PRIM_BASE | 0x00FF).

    \{ */
#define ATT_PRIM_DOWN           (ATT_PRIM_BASE)
#define ATT_PRIM_UP             (ATT_PRIM_BASE | 0x0080)

typedef enum att_prim_tag
{
    /* downstream primitives */
    ENUM_ATT_REGISTER_REQ = ATT_PRIM_DOWN,
    ENUM_ATT_UNREGISTER_REQ,
    ENUM_ATT_ADD_DB_REQ, /* onchip only */
    ENUM_ATT_ADD_REQ, /* host only */
    ENUM_ATT_REMOVE_REQ, /* host only */
    ENUM_ATT_CONNECT_REQ,
    ENUM_ATT_DISCONNECT_REQ,
    ENUM_ATT_EXCHANGE_MTU_REQ,
    ENUM_ATT_EXCHANGE_MTU_RSP,
    ENUM_ATT_FIND_INFO_REQ,
    ENUM_ATT_FIND_BY_TYPE_VALUE_REQ,
    ENUM_ATT_READ_BY_TYPE_REQ,
    ENUM_ATT_READ_REQ,
    ENUM_ATT_READ_BLOB_REQ,
    ENUM_ATT_READ_MULTI_REQ,
    ENUM_ATT_READ_BY_GROUP_TYPE_REQ,
    ENUM_ATT_WRITE_REQ,
    ENUM_ATT_PREPARE_WRITE_REQ,
    ENUM_ATT_EXECUTE_WRITE_REQ,
    ENUM_ATT_HANDLE_VALUE_REQ,
    ENUM_ATT_HANDLE_VALUE_RSP,
    ENUM_ATT_ACCESS_RSP,
    ENUM_ATT_CONNECT_RSP,

    /* upstream primitives */
    ENUM_ATT_REGISTER_CFM = ATT_PRIM_UP,
    ENUM_ATT_UNREGISTER_CFM,
    ENUM_ATT_ADD_DB_CFM, /* onchip only */
    ENUM_ATT_ADD_CFM, /* host only */
    ENUM_ATT_REMOVE_CFM, /* host only */
    ENUM_ATT_CONNECT_CFM,
    ENUM_ATT_CONNECT_IND,
    ENUM_ATT_DISCONNECT_CFM, /* robinson only */
    ENUM_ATT_DISCONNECT_IND,
    ENUM_ATT_EXCHANGE_MTU_CFM,
    ENUM_ATT_EXCHANGE_MTU_IND,
    ENUM_ATT_FIND_INFO_CFM,
    ENUM_ATT_FIND_BY_TYPE_VALUE_CFM,
    ENUM_ATT_READ_BY_TYPE_CFM,
    ENUM_ATT_READ_CFM,
    ENUM_ATT_READ_BLOB_CFM,
    ENUM_ATT_READ_MULTI_CFM,
    ENUM_ATT_READ_BY_GROUP_TYPE_CFM,
    ENUM_ATT_WRITE_CFM,
    ENUM_ATT_PREPARE_WRITE_CFM,
    ENUM_ATT_EXECUTE_WRITE_CFM,
    ENUM_ATT_HANDLE_VALUE_CFM,
    ENUM_ATT_HANDLE_VALUE_IND,
    ENUM_ATT_ACCESS_IND,

    ENUM_ATT_DEBUG_IND,

    ENUM_SEP_ATT_LAST
} ATT_PRIM_T;

/* downstream primitives */
#define ATT_REGISTER_REQ        ((att_prim_t)ENUM_ATT_REGISTER_REQ)
#define ATT_UNREGISTER_REQ      ((att_prim_t)ENUM_ATT_UNREGISTER_REQ)
#define ATT_ADD_DB_REQ          ((att_prim_t)ENUM_ATT_ADD_DB_REQ)
#define ATT_ADD_REQ             ((att_prim_t)ENUM_ATT_ADD_REQ)
#define ATT_REMOVE_REQ          ((att_prim_t)ENUM_ATT_REMOVE_REQ)
#define ATT_CONNECT_REQ         ((att_prim_t)ENUM_ATT_CONNECT_REQ)
#define ATT_DISCONNECT_REQ      ((att_prim_t)ENUM_ATT_DISCONNECT_REQ)
#define ATT_EXCHANGE_MTU_REQ    ((att_prim_t)ENUM_ATT_EXCHANGE_MTU_REQ)
#define ATT_EXCHANGE_MTU_RSP    ((att_prim_t)ENUM_ATT_EXCHANGE_MTU_RSP)
#define ATT_FIND_INFO_REQ       ((att_prim_t)ENUM_ATT_FIND_INFO_REQ)
#define ATT_FIND_BY_TYPE_VALUE_REQ ((att_prim_t)ENUM_ATT_FIND_BY_TYPE_VALUE_REQ)
#define ATT_READ_BY_TYPE_REQ    ((att_prim_t)ENUM_ATT_READ_BY_TYPE_REQ)
#define ATT_READ_REQ            ((att_prim_t)ENUM_ATT_READ_REQ)
#define ATT_READ_BLOB_REQ       ((att_prim_t)ENUM_ATT_READ_BLOB_REQ)
#define ATT_READ_MULTI_REQ      ((att_prim_t)ENUM_ATT_READ_MULTI_REQ)
#define ATT_READ_BY_GROUP_TYPE_REQ ((att_prim_t)ENUM_ATT_READ_BY_GROUP_TYPE_REQ)
#define ATT_WRITE_REQ           ((att_prim_t)ENUM_ATT_WRITE_REQ)
#define ATT_PREPARE_WRITE_REQ   ((att_prim_t)ENUM_ATT_PREPARE_WRITE_REQ)
#define ATT_EXECUTE_WRITE_REQ   ((att_prim_t)ENUM_ATT_EXECUTE_WRITE_REQ)
#define ATT_HANDLE_VALUE_REQ    ((att_prim_t)ENUM_ATT_HANDLE_VALUE_REQ)
#define ATT_HANDLE_VALUE_RSP    ((att_prim_t)ENUM_ATT_HANDLE_VALUE_RSP)
#define ATT_ACCESS_RSP          ((att_prim_t)ENUM_ATT_ACCESS_RSP)
#define ATT_CONNECT_RSP         ((att_prim_t)ENUM_ATT_CONNECT_RSP)

/* upstream primitives */
#define ATT_REGISTER_CFM        ((att_prim_t)ENUM_ATT_REGISTER_CFM)
#define ATT_UNREGISTER_CFM      ((att_prim_t)ENUM_ATT_UNREGISTER_CFM)
#define ATT_ADD_DB_CFM          ((att_prim_t)ENUM_ATT_ADD_DB_CFM)
#define ATT_ADD_CFM             ((att_prim_t)ENUM_ATT_ADD_CFM)
#define ATT_REMOVE_CFM          ((att_prim_t)ENUM_ATT_REMOVE_CFM)
#define ATT_CONNECT_CFM         ((att_prim_t)ENUM_ATT_CONNECT_CFM)
#define ATT_CONNECT_IND         ((att_prim_t)ENUM_ATT_CONNECT_IND)
#define ATT_DISCONNECT_CFM     ((att_prim_t)ENUM_ATT_DISCONNECT_CFM)
#define ATT_DISCONNECT_IND      ((att_prim_t)ENUM_ATT_DISCONNECT_IND)
#define ATT_EXCHANGE_MTU_CFM    ((att_prim_t)ENUM_ATT_EXCHANGE_MTU_CFM)
#define ATT_EXCHANGE_MTU_IND    ((att_prim_t)ENUM_ATT_EXCHANGE_MTU_IND)
#define ATT_FIND_INFO_CFM       ((att_prim_t)ENUM_ATT_FIND_INFO_CFM)
#define ATT_FIND_BY_TYPE_VALUE_CFM ((att_prim_t)ENUM_ATT_FIND_BY_TYPE_VALUE_CFM)
#define ATT_READ_BY_TYPE_CFM    ((att_prim_t)ENUM_ATT_READ_BY_TYPE_CFM)
#define ATT_READ_CFM            ((att_prim_t)ENUM_ATT_READ_CFM)
#define ATT_READ_BLOB_CFM       ((att_prim_t)ENUM_ATT_READ_BLOB_CFM)
#define ATT_READ_MULTI_CFM      ((att_prim_t)ENUM_ATT_READ_MULTI_CFM)
#define ATT_READ_BY_GROUP_TYPE_CFM ((att_prim_t)ENUM_ATT_READ_BY_GROUP_TYPE_CFM)
#define ATT_WRITE_CFM           ((att_prim_t)ENUM_ATT_WRITE_CFM)
#define ATT_PREPARE_WRITE_CFM   ((att_prim_t)ENUM_ATT_PREPARE_WRITE_CFM)
#define ATT_EXECUTE_WRITE_CFM   ((att_prim_t)ENUM_ATT_EXECUTE_WRITE_CFM)
#define ATT_HANDLE_VALUE_CFM    ((att_prim_t)ENUM_ATT_HANDLE_VALUE_CFM)
#define ATT_HANDLE_VALUE_IND    ((att_prim_t)ENUM_ATT_HANDLE_VALUE_IND)
#define ATT_ACCESS_IND          ((att_prim_t)ENUM_ATT_ACCESS_IND)

#define ATT_DEBUG_IND           ((att_prim_t)ENUM_ATT_DEBUG_IND)
/* \} */

/*! \brief Types for ATT */
typedef CsrUint16                att_prim_t;
typedef CsrUint16                att_result_t;

/*! \brief cid for accessing the local database. */
#define ATT_CID_LOCAL           0x0000

/*! \brief Register the ATT subsystem request

    Before any ATT operations can be performed the ATT subsystem shall
    be registered in order to allocate required resources.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_REGISTER_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
} ATT_REGISTER_REQ_T;

/*! \brief ATT subsystem register confirmation */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_REGISTER_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_REGISTER_CFM_T;

/*! \brief Unregister the ATT subsystem request */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_UNREGISTER_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
} ATT_UNREGISTER_REQ_T;

/*! \brief ATT subsystem unregister confirmation */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_UNREGISTER_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_UNREGISTER_CFM_T;

/*! \brief cid for accessing the local database. */
#define ATT_ADD_DB_OVERWRITE    0x0001  /*!< Allow overwriting existing DB */

/*! \brief Add a complete attribute database

    This primitive is only available on onchip version of ATT.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ADD_DB_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            flags;          /*!< Execute flags - uses ATT_ADD_DB */
    CsrUint16            size_db;        /*!< The size of the database */
    CsrUint16            *db;            /*!< Attribute database */
} ATT_ADD_DB_REQ_T;

/*! \brief Confirmation to attribute add

    This primitive is only available on onchip version of ATT.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ADD_DB_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_ADD_DB_CFM_T;

#ifdef BUILD_FOR_HOST
/*! \brief Attribute type definition for a single attribute */
typedef struct att_attr_tag
{
    CsrUint16 handle;                    /*!< Attribute handle */
    CsrUint16 perm;                      /*!< Attribute permissions - uses ATT_PERM range */
    CsrUint16 flags;                     /*!< Attribute flags - uses ATT_ATTR range */
    CsrUint32 uuid[4];                   /*!< Attribute UUID */
    CsrUint16 size_value;                /*!< The size of the value */
    CsrUint8 *value;                     /*!< value */

    struct att_attr_tag *next;
} att_attr_t;
#endif /* !BUILD_FOR_HOST */

/*! \brief Add attributes to the database

    After passing attributes to the ATT subsystem the application shall
    not use the attribute memory again.

    This primitive is only available on host version of ATT.
*/
#ifndef BUILD_FOR_HOST
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ADD_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
} ATT_ADD_REQ_T;
#else /* !BUILD_FOR_HOST */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ADD_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    att_attr_t          *attrs;         /*!< List of attributes to add */
} ATT_ADD_REQ_T;
#endif /* BUILD_FOR_HOST */

/*! \brief Confirmation to attribute add

    This primitive is only available on host version of ATT.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ADD_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_ADD_CFM_T;

/*! \brief Remove attributes from the database

    This primitive is only available on host version of ATT.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_REMOVE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            start;          /*!< First handle number to be removed */
    CsrUint16            end;            /*!< Last handle number to be removed */
} ATT_REMOVE_REQ_T;

/*! \brief Confirmation to attribute removal

    This primitive is only available on host version of ATT.
 */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_REMOVE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            num_attr;       /*!< Number of attributes removed */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_REMOVE_CFM_T;

/*----------------------------------------------------------------------------*
 * API:
 *     ATT_CONNECT_REQ_T
 *
 * PURPOSE:
 * Primarily as a request to establish an ATT bearer between a client & a server
 *
 * -----------------------------------------------------------------------------
 * "LE Advertising module related behavior":
 * -----------------------------------------------------------------------------
 * Bits-2:4 are special connection flags and imply the following:
 * <    Binary     >    <               Description                     >
 * xxxxxxxx xxx000xx    Channel to be used for UCD
 * xxxxxxxx xxx001xx    Channel to be used for broadcast connectionless
 * xxxxxxxx xxx010xx    Connect as Master
 * xxxxxxxx xxx011xx    Connect as Master using whitelist
 * xxxxxxxx xxx100xx    Start high duty cycle directed adverts
 * xxxxxxxx xxx101xx    Use whitelist for undirected adverts
 * xxxxxxxx xxx110xx    Start undirected adverts
 * xxxxxxxx xxx111xx    Start low duty cycle directed adverts
 *
 * If its used to enable LE connectable advertising (using above connection
 * flags) then it won't fail because of invalid advertising parameter values.
 * Hence application's request to enable LE advertising using this API would
 * always succeed provided enable advertising is not in progress nor
 * advertising is already enabled.
 *
 * Filter policy rules:
 * a) Filter policy is not applicable for directed adverts.
 * b) Only connection specific filter policy is set via ATT and based on
 *    above connection flags.
 *     - If "undirected adverts" then process connection requests from all
 *       devices.
 *     - If "Use whitelist for undirected adverts" then process connection
         requests from devices in whitelist.
 * c) ATT i/f is insensitive to scan specific filter policy and hence scan
 *    specific filter policy set via DM i/f
 *    (i.e. DM_HCI_ULP_SET_ADVERTISING_PARAMETERS_REQ) is considered.
 * -----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_CONNECT_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    TYPED_BD_ADDR_T     addrt;          /*!< Bluetooth address of remote device */
    l2ca_conflags_t     flags;          /*!< L2CAP connection flags */
} ATT_CONNECT_REQ_T;

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_CONNECT_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    TYPED_BD_ADDR_T     addrt;          /*!< Bluetooth address of remote device */
    l2ca_conflags_t     flags;          /*!< L2CAP connection flags */
    CsrUint16            cid;
    CsrUint16            mtu;            /*!< MTU for the connection */
    l2ca_conn_result_t  result;         /*!< Result code - uses L2CA_CONNECT range */
} ATT_CONNECT_CFM_T;

/*! \brief Indication for ATT connection

    Note this prim solicits a response (ATT_CONNECT_RSP)
    if it has been recieved on BR/EDR channel (see l2ca_conflags_t).
 */
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_CONNECT_IND */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    TYPED_BD_ADDR_T     addrt;          /*!< Bluetooth address of remote device */
    l2ca_conflags_t     flags;          /*!< L2CAP connection flags */
    CsrUint16            mtu;            /*!< MTU for the connection */
    CsrUint16            cid;
} ATT_CONNECT_IND_T;


typedef struct
{
    att_prim_t          type;           /*!< Always ATT_CONNECT_RSP */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;            /*!< Local CID */
    l2ca_conn_result_t  response;       /*!< Response from the application */
} ATT_CONNECT_RSP_T;

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_DISCONNECT_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
} ATT_DISCONNECT_REQ_T;

/*! \brief Confirmation to a disconnect request.

    This primitive is only available on robinson version of ATT.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_DISCONNECT_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    l2ca_disc_result_t  reason;         /*!< Reason code - uses L2CA_DISCONNECT range */
} ATT_DISCONNECT_CFM_T;

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_DISCONNECT_IND */
    CsrSchedQid           phandle;/*!< Destination phandle */
    CsrUint16            cid;
    l2ca_disc_result_t  reason;         /*!< Reason code - uses L2CA_DISCONNECT range */
} ATT_DISCONNECT_IND_T;

/*!
    \brief Send Exchange MTU Request to the server

    Inform the server of the client's maximum receive MTU size.

    The ATT subsystem will try to allocate memory block of requested MTU value
    size, if it succeeds then it will go ahead and send Exchange MTU Req to peer
    otherwise configuration would fail with ATT_RESULT_INVALID_MTU result.

    It is application's responsibility to configure MTU to a sensible value
    so that memory blocks of size MTU value is going to be readily available
    from the memory pool, otherwise chip may panic because of heap exhaustion
    if overall memory usage in the system is high. Maximum suggested MTU value
    is 64 words, considering the memory restrictions for an onchip solution.

    \return ATT_EXCHANGE_MTU_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXCHANGE_MTU_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            mtu;            /*< Client receive MTU size */
} ATT_EXCHANGE_MTU_REQ_T;

/*!
    \brief Confirmation of MTU exchange
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXCHANGE_MTU_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            mtu;            /*!< MTU for the connection */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_EXCHANGE_MTU_CFM_T;

/*!
    \brief Indication of client's maximum receive size

    When receiving client's maximum receive size the server shall respond it's
    own MTU back to the client using ATT_EXCHANGE_MTU_RSP_T.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXCHANGE_MTU_IND */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            client_mtu;     /*< Client receive MTU size */
} ATT_EXCHANGE_MTU_IND_T;

/*!
    \brief Response to Exchange MTU indication

    Server shall set server_mtu to the maximum value it supports.

    The ATT subsystem will set internal ATT_MTU to the minimum of the
    Client Rx MTU (from the indication) and the Server Rx MTU.
    Before sending out Exchange MTU Response to peer, ATT will try to allocate
    memory block of size equal to ATT_MTU value. If it succeeds then ATT_MTU
    will be used in all subsequent ATT packets creation, otherwise ATT_MTU
    value will be set to ATT default MTU.

    It is application's responsibility to configure MTU to a sensible value so
    that memory blocks of MTU value size is going to be readily available from
    the memory pool, otherwise chip may panic because of heap exhaustion if
    overall memory usage in the system is high.Maximum suggested MTU value
    is 64 words, considering the memory restrictions for an onchip solution.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXCHANGE_MTU_RSP */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            server_mtu;     /*< Server receive MTU size */
} ATT_EXCHANGE_MTU_RSP_T;

/*!
    \brief Send Find Information Request to the server

    The Find Information Request is used to obtain the mapping of attribute
    handles with their associated types.

    \return ATT_FIND_INFO_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_FIND_INFO_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            start;          /*!< First requested handle number */
    CsrUint16            end;            /*!< Last requested handle number */
} ATT_FIND_INFO_REQ_T;

/*!
    \brief Confirmation of Find Information Request

    Every response contains information for one handle, and a single Find
    Information Request may yield multiple confirmations.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_FIND_INFO_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    att_uuid_type_t     uuid_type;      /*!< Type of the UUID */
    CsrUint32            uuid[4];        /*!< The UUID of the attribute */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_FIND_INFO_CFM_T;

/*!
    \brief Send Find By Type Value Request to the server

    The Find By Type Value Request is used to obtain the handles of attributes
    that have a 16-bit UUID attribute type and attribute value. This allows
    the range of handles associated with a given attribute to be discovered
    when the attribute type determines the grouping of a set of attributes.

    Generic Attribute Profile defines two grouping UUIDs, ATT_UUID_PRI_SERVICE
    for Primary Services and ATT_UUID_SEC_SERVICE for Secondary Services.

    \return ATT_FIND_BY_TYPE_VALUE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_FIND_BY_TYPE_VALUE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            start;          /*!< First requested handle number */
    CsrUint16            end;            /*!< Last requested handle number */
    CsrUint16            uuid;           /*!< 16-bit Attribute UUID to find */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< Attribute value to find */
} ATT_FIND_BY_TYPE_VALUE_REQ_T;

/*!
    \brief Confirmation of Find By Type Value request

    Every response contains information for one handle, and a single Find
    By Type Value Request may yield multiple confirmations.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_FIND_BY_TYPE_VALUE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    CsrUint16            end;            /*!< The group end handle */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_FIND_BY_TYPE_VALUE_CFM_T;

/*!
    \brief Send Read By Type Request to the server

    The Read By Type Request is used to obtain the values of attributes where
    the attribute type is known but the handle is not known.

    \return ATT_READ_BY_TYPE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BY_TYPE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            start;          /*!< First requested handle number */
    CsrUint16            end;            /*!< Last requested handle number */
    /*! Type of the UUID field.
     *
     * If type is ATT_UUID16 only lower 16 bits of uuid[0] are considered to be
     * valid, and the rest are ignored. */
    att_uuid_type_t     uuid_type;
    CsrUint32            uuid[4];        /*!< 16 or 128 bit UUID */
} ATT_READ_BY_TYPE_REQ_T;

/*!
    \brief Confirmation of Read By Type request

    Every response contains information for one handle, and a single Read
    By Type Request may yield multiple confirmations.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BY_TYPE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_READ_BY_TYPE_CFM_T;

/*!
    \brief Send Read Request to the server

    The Read Request is used to request the server to read the value of an
    attribute when the handle is known.

    \return ATT_READ_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute to be read */
} ATT_READ_REQ_T;

/*!
    \brief Confirmation of Read Request
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< The attribute value */
} ATT_READ_CFM_T;

/*!
    \brief Send Read Blob Request to the server

    The Read Blob Request is used to request the server to read part of the
    value of an attribute at a given offset and return a specific part of the
    value.

    \return ATT_READ_BLOB_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BLOB_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute to be read */
    CsrUint16            offset;         /*!< The offset of the first octet to be read */
} ATT_READ_BLOB_REQ_T;

/*!
    \brief Confirmation of Read Blob Request
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BLOB_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< The attribute value */
} ATT_READ_BLOB_CFM_T;

/*!
    \brief Send Read Multiple Request to the server

    The Read Multiple Request is used to request the server to read two or
    more values of a set of attributes and return their values.

    Return value does not contain attribute length information, i.e. only
    values that have a known fixed size can be read, with the exception of
    the last value that can have a variable length.

    \return ATT_READ_MULTI_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_MULTI_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            size_handles;   /*!< Length of the handles list */
    CsrUint16            *handles;       /*!< A set of two or more attribute handles to be read */
} ATT_READ_MULTI_REQ_T;

/*!
    \brief Confirmation of Read Multiple Request

    Value is concatenated list of attributes requested without any field
    length or ending information.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_MULTI_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< The concatenated value of requested attributes */
} ATT_READ_MULTI_CFM_T;

/*!
    \brief Send Read By Group Type Request to the server

    The Read By Group Type Request is used to obtain the values of attributes
    where the attribute type is known, the type of a grouping attribute as
    defined by a higher layer specification, but the handle is not known.

    Generic Attribute Profile defines two grouping UUIDs, ATT_UUID_PRI_SERVICE
    for Primary Services and ATT_UUID_SEC_SERVICE for Secondary Services.

    \return ATT_READ_BY_GROUP_TYPE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BY_GROUP_TYPE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            start;          /*!< First requested handle number */
    CsrUint16            end;            /*!< Last requested handle number */
    /*! Type of the group UUID field.
     *
     * If type is ATT_UUID16 only lower 16 bits of group[0] are considered to
     * be valid, and the rest are ignored. */
    att_uuid_type_t     group_type;
    CsrUint32            group[4];       /*!< The attribute group type UUID */
} ATT_READ_BY_GROUP_TYPE_REQ_T;

/*!
    \brief Confirmation of Read By Group Type Request

    Every response contains information for one handle, and a single Read
    By Group Type Request may yield multiple confirmations.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_READ_BY_GROUP_TYPE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            handle;         /*!< The handle of the attribute */
    CsrUint16            end;            /*!< The group end handle */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< The value of the attribute */
} ATT_READ_BY_GROUP_TYPE_CFM_T;

/*!
    \name Flags for Write Request

    \{
*/
/*! Send Write Request to the server */
#define ATT_WRITE_REQUEST       0x0000
/*! Send Write Command to the server. */
#define ATT_WRITE_COMMAND       0x0040
/*! Send Signed Write to the server. Only Write Command can be signed. */
#define ATT_WRITE_SIGNED        0x0080
/* \} */

/*!
    \brief Send Write Request, Write Command, or Signed Write Command to the
    server

    Flags is used to determine whether to send Write Request
    (ATT_WRITE_REQUEST), Write Command (ATT_WRITE_COMMAND) or Signed Write
    Command (ATT_WRITE_COMMAND | ATT_WRITE_SIGNED).

    Signed Write Command can only be sent to bonded server.

    \return ATT_WRITE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_WRITE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute to be written */
    CsrUint16            flags;          /*!< Flags for the operation */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_WRITE_REQ_T;

/*!
    \brief Confirmation of Write Request, Write Command, or Signed Write
    Command

    Confirmation of Write Request is sent when the server acknowledges the
    write.

    Confirmation of Write Command and Signed Write Command is sent when the
    command has been sent to the server.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_WRITE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_WRITE_CFM_T;

/*!
    \brief Send Prepare Write Request to the server

    The Prepare Write Request is used to request the server to prepare to
    write the value of an attribute. The server will respond to this request
    with a Prepare Write Response, so that the client can verify that the
    value was received correctly.

    \return ATT_PREPARE_WRITE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_PREPARE_WRITE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute to be written */
    CsrUint16            offset;         /*!< The offset of the first octet to be written */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_PREPARE_WRITE_REQ_T;

/*!
    \brief Confirmation of Prepare Write Request
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_PREPARE_WRITE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;
    CsrUint16            offset;         /*!< The offset of the first octet to be written */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_PREPARE_WRITE_CFM_T;

/*
    \name Execute Write Request flags

    \{
*/
/*! Cancel all pending prepared writes */
#define ATT_EXECUTE_CANCEL      0x0000
/*! Immediately write all pending prepared values */
#define ATT_EXECUTE_WRITE       0x0001
/*! \} */

/*!
    \brief Send Execute Write Request to the server

    The Execute Write Request is used to request the server to write or cancel
    the write of all the prepared values currently held in the prepare queue
    from this client.

    \return ATT_EXECUTE_WRITE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXECUTE_WRITE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            flags;          /*!< Execute flags - uses ATT_EXECUTE range */
} ATT_EXECUTE_WRITE_REQ_T;

/*!
    \brief Confirmation of Execute Write Request
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_EXECUTE_WRITE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< Handle of failed attribute in case of error, 0 on success. */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_EXECUTE_WRITE_CFM_T;

/*!
    \name Handle Value flags

    \{
*/

#define ATT_HANDLE_VALUE_NOTIFICATION   0x0000
/*!
    \brief Send Indication to the client.

    The client must acknowledge the indication with ATT_HANDLE_VALUE_RSP_T
    before any other indications can be sent.
*/
#define ATT_HANDLE_VALUE_INDICATION     0x0001
/* \} */

/*!
    \brief Send Handle Value Notification/Indication to the client

    Notify the client of an attribute's value.

    Two kinds of server initiated messages exist: Handle Value Notification
    and Handle Value Indication.

    The notification can be send at any time, and the client does not
    confirm receiving it.

    The indication must be confirmed by the client before any other indication
    can be sent.

    Flags is used to select whether to notify or indicate the client.

    Sending a notification or an indication does not change the attribute's
    value in the database.

    \return ATT_HANDLE_VALUE_CFM_T
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_HANDLE_VALUE_REQ */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    CsrUint16            flags;          /*!< Request flags, uses ATT_HANDLE_VALUE range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;         /*!< The value of the attribute */
} ATT_HANDLE_VALUE_REQ_T;

/*!
    \brief Confirmation of Handle Value Notification/Indication

    In case of Indication two ATT_HANDLE_VALUE_CFMs are sent, one when the
    indication was sent (ATT_RESULT_SUCCESS_SENT), and another when the client
    confirmed the indication (ATT_RESULT_SUCCESS).
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_HANDLE_VALUE_CFM */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
} ATT_HANDLE_VALUE_CFM_T;

/*!
    \brief Handle Value Notification or Indication from the server

    In case of Notification (flags contain bit ATT_HANDLE_VALUE_INDICATION) the
    client shall respond using ATT_HANDLE_VALUE_RSP_T that it has received
    the Notification.

    In case of Indication the client do not need to response back.
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_HANDLE_VALUE_IND */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    CsrUint16            flags;          /*!< Flags - uses ATT_HANDLE_VALUE range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_HANDLE_VALUE_IND_T;

/*!
    \brief Response to Handle Value Notification
*/
typedef struct
{
    att_prim_t          type;           /*!< Always ATT_HANDLE_VALUE_RSP */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
} ATT_HANDLE_VALUE_RSP_T;

/*!
    \name Flags for Access Indication

    \{
*/
/*! Request to read attribute value */
#define ATT_ACCESS_READ         0x0001
/*! Request to write attribute value. */
#define ATT_ACCESS_WRITE        0x0002
/*! Request for sufficient permission for r/w of attribute value. */
#define ATT_ACCESS_PERMISSION   0x8000
/*! Indication of completed write, rsp mandatory */
#define ATT_ACCESS_WRITE_COMPLETE   0x4000
/*! Indication that GATT will request APP for enc key len*/
#define ATT_ACCESS_ENCRYPTION_KEY_LEN  0x2000
/*! Indication to get error code from application for situation
    when there is attempt to access attribute on a disallowed radio */
#define ATT_ACCESS_DISALLOWED_OVER_RADIO  0x1000


/* \} */

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ACCESS_IND */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    CsrUint16            flags;          /*!< Flags - uses ATT_ACCESS range */
    CsrUint16            offset;         /*!< The offset of the first octet to be accessed */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_ACCESS_IND_T;

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_ACCESS_IND */
    CsrSchedQid           phandle;        /*!< Destination phandle */
    CsrUint16            cid;
    CsrUint16            handle;         /*!< The handle of the attribute */
    att_result_t        result;         /*!< Result code - uses ATT_RESULT range */
    CsrUint16            size_value;     /*!< Length of the value */
    CsrUint8             *value;
} ATT_ACCESS_RSP_T;

typedef struct
{
    att_prim_t          type;           /*!< Always ATT_DEBUG_IND */
    CsrUint16            size_debug;
    CsrUint8             *debug;
} ATT_DEBUG_IND_T;

/*! \brief Union of the primitives */
typedef union
{
    /* Shared */
    att_prim_t                  type;

    /* Downstream */
    ATT_REGISTER_REQ_T          att_register_req;
    ATT_UNREGISTER_REQ_T        att_unregister_req;
    ATT_ADD_DB_REQ_T            att_add_db_req;
    ATT_ADD_REQ_T               att_add_req;
    ATT_REMOVE_REQ_T            att_remove_req;
    ATT_CONNECT_REQ_T           att_connect_req;
    ATT_DISCONNECT_REQ_T        att_disconnect_req;
    ATT_EXCHANGE_MTU_REQ_T      att_exchange_mtu_req;
    ATT_EXCHANGE_MTU_RSP_T      att_exchange_mtu_rsp;
    ATT_FIND_INFO_REQ_T         att_find_info_req;
    ATT_FIND_BY_TYPE_VALUE_REQ_T att_find_by_type_value_req;
    ATT_READ_BY_TYPE_REQ_T      att_read_by_type_req;
    ATT_READ_REQ_T              att_read_req;
    ATT_READ_BLOB_REQ_T         att_read_blob_req;
    ATT_READ_MULTI_REQ_T        att_read_multi_req;
    ATT_READ_BY_GROUP_TYPE_REQ_T att_read_by_group_type_req;
    ATT_WRITE_REQ_T             att_write_req;
    ATT_PREPARE_WRITE_REQ_T     att_prepare_write_req;
    ATT_EXECUTE_WRITE_REQ_T     att_execute_write_req;
    ATT_HANDLE_VALUE_REQ_T      att_handle_value_req;
    ATT_HANDLE_VALUE_RSP_T      att_handle_value_rsp;
    ATT_ACCESS_RSP_T            att_access_rsp;
    ATT_CONNECT_RSP_T           att_connect_rsp;

    /* Upstream */
    ATT_REGISTER_CFM_T          att_register_cfm;
    ATT_UNREGISTER_CFM_T        att_unregister_cfm;
    ATT_ADD_DB_CFM_T            att_add_db_cfm;
    ATT_ADD_CFM_T               att_add_cfm;
    ATT_REMOVE_CFM_T            att_remove_cfm;
    ATT_CONNECT_CFM_T           att_connect_cfm;
    ATT_CONNECT_IND_T           att_connect_ind;
    ATT_DISCONNECT_IND_T        att_disconnect_ind;
    ATT_DISCONNECT_CFM_T        att_disconnect_cfm;
    ATT_EXCHANGE_MTU_CFM_T      att_exchange_mtu_cfm;
    ATT_EXCHANGE_MTU_IND_T      att_exchange_mtu_ind;
    ATT_FIND_INFO_CFM_T         att_find_info_cfm;
    ATT_FIND_BY_TYPE_VALUE_CFM_T att_find_by_type_value_cfm;
    ATT_READ_BY_TYPE_CFM_T      att_read_by_type_cfm;
    ATT_READ_CFM_T              att_read_cfm;
    ATT_READ_BLOB_CFM_T         att_read_blob_cfm;
    ATT_READ_MULTI_CFM_T        att_read_multi_cfm;
    ATT_READ_BY_GROUP_TYPE_CFM_T att_read_by_group_type_cfm;
    ATT_WRITE_CFM_T             att_write_cfm;
    ATT_PREPARE_WRITE_CFM_T     att_prepare_write_cfm;
    ATT_EXECUTE_WRITE_CFM_T     att_execute_write_cfm;
    ATT_HANDLE_VALUE_CFM_T      att_handle_value_cfm;
    ATT_HANDLE_VALUE_IND_T      att_handle_value_ind;
    ATT_ACCESS_IND_T            att_access_ind;

    ATT_DEBUG_IND_T             att_debug_ind;

} ATT_UPRIM_T;

#ifdef __cplusplus
}
#endif

#endif /* _BLUESTACK_ATT_PRIM_H_ */
