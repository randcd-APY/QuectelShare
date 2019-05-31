#ifndef CSR_BT_GATT_PRIM_H__
#define CSR_BT_GATT_PRIM_H__

/******************************************************************************
 Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "att_prim.h"
#include "csr_bt_cm_prim.h"
#include "csr_bt_uuids.h"
#include "csr_bt_sc_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtGattPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* Basic types */
typedef CsrPrim         CsrBtGattPrim;
typedef CsrUint16       CsrBtGattHandle;
typedef att_attr_t      CsrBtGattDb;
typedef CsrUint32       CsrBtGattId;
typedef CsrUint32       CsrBtGattConnFlags;
typedef CsrUint32       CsrBtGattConnInfo;
typedef CsrUint8        CsrBtGattScanFlags;
typedef CsrUint16       CsrBtGattDbAccessRspCode;
typedef CsrUint16       CsrBtGattCliConfigBits;
typedef CsrUint16       CsrBtGattSrvConfigBits;
typedef CsrUint8        CsrBtGattPropertiesBits;
typedef CsrUint16       CsrBtGattExtPropertiesBits;
typedef CsrUint16       CsrBtGattAttrFlags;
typedef CsrUint16       CsrBtGattPermFlags;
typedef CsrUint8        CsrBtGattFormats;
typedef CsrUint8        CsrBtGattPeripheralPrivacyFlag;
typedef CsrUint32       CsrBtGattEventMask;
typedef CsrUint16       CsrBtGattAccessCheck;
typedef CsrUint8        CsrBtGattReportEvent;
typedef CsrUint16       CsrBtGattSecurityFlags;
typedef CsrUint8        CsrBtGattLeRole;

/* Special GATT identifiers */
#define CSR_BT_GATT_INVALID_GATT_ID                             ((CsrBtGattId)0x00000000)
#define CSR_BT_GATT_LOCAL_BT_CONN_ID                            ((CsrBtConnId)(0x00010000 | ATT_CID_LOCAL))

/* Special handle values */
#define CSR_BT_GATT_ATTR_HANDLE_INVALID                         ((CsrBtGattHandle) 0x0000)
#define CSR_BT_GATT_ATTR_HANDLE_MAX                             ((CsrBtGattHandle) 0xFFFF)

/* Special LE connection parameter values */
#define CSR_BT_GATT_INVALID_CONN_INTERVAL                       ((CsrUint16) 0x0000)
#define CSR_BT_GATT_INVALID_CONN_LATENCY                        ((CsrUint16) 0x0000)
#define CSR_BT_GATT_INVALID_CONN_SUPERVISION_TIMEOUT            ((CsrUint16) 0x0000)

/* Connect flags. Comments show what what flags are valid where. */
#define CSR_BT_GATT_FLAGS_NONE                                  ((CsrBtGattConnFlags)0x00000000)    /* no special options, use LE radio (all) */
#define CSR_BT_GATT_FLAGS_WHITELIST                             ((CsrBtGattConnFlags)0x00000001)    /* allow connections from whitelist only (peripheral/central) */
#define CSR_BT_GATT_FLAGS_UNDIRECTED                            ((CsrBtGattConnFlags)0x00000002)    /* If a connection exists in LE slave role, GATT returns success with connection
                                                                                                       information of existing connection, else starts new un-directed peripheral procedure. */
#define CSR_BT_GATT_FLAGS_ADVERTISE_TIMEOUT                     ((CsrBtGattConnFlags)0x00000004)    /* undirected advertising times out (peripheral) */
#define CSR_BT_GATT_FLAGS_NONDISCOVERABLE                       ((CsrBtGattConnFlags)0x00000008)    /* AD flags are non-discoverable (advertise/peripheral) */
#define CSR_BT_GATT_FLAGS_LIMITED_DISCOVERABLE                  ((CsrBtGattConnFlags)0x00000010)    /* AD flags are limited discoverable (advertise/peripheral) */
#define CSR_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE                 ((CsrBtGattConnFlags)0x00000020)    /* disable scan response (advertise) */
#define CSR_BT_GATT_FLAGS_APPEND_DATA                           ((CsrBtGattConnFlags)0x00000040)    /* append advertise/scan-rsp data (advertise/peripheral) */
#define CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY                      ((CsrBtGattConnFlags)0x00000080)    /* do not attempt to highten security (central/peripheral) */
#define CSR_BT_GATT_FLAGS_NO_AUTO_SIGN_UPGRADE                  ((CsrBtGattConnFlags)0x00000100)    /* do not allow sign-to-normal write commands while encrypted */
#define CSR_BT_GATT_FLAGS_WHITELIST_SCANRSP                     ((CsrBtGattConnFlags)0x00000200)    /* allow scan response to whitelist only (advertise/peripheral)*/
#define CSR_BT_GATT_FLAGS_UNDIRECTED_NEW                        ((CsrBtGattConnFlags)0x00000402)    /* GATT starts new un-directed peripheral procedure irrespective
                                                                                                        of existing LE connections */
#define CSR_BT_GATT_FLAGS_ATTEMPTED_SECURITY                    ((CsrBtGattConnFlags)0x40000000)    /* for internal use only */
#define CSR_BT_GATT_FLAGS_CENTRAL_TIMEOUT                       CSR_BT_GATT_FLAGS_ADVERTISE_TIMEOUT /* Central connection times out */

/* Connection info flags */
#define CSR_BT_GATT_CONNINFO_LE                                 ((CsrBtGattConnInfo)0x000000000) /* connection runs on the LE radio */
#define CSR_BT_GATT_CONNINFO_BREDR                              ((CsrBtGattConnInfo)0x000000001) /* connection runs on the BREDR radio */
/* Le Role */
#define CSR_BT_GATT_LE_ROLE_UNDEFINED                           ((CsrBtGattLeRole)0x00)         /* Using the BREDR radio */
#define CSR_BT_GATT_LE_ROLE_MASTER                              ((CsrBtGattLeRole)0x01)         /* Using the LE radio as Master/Central   */
#define CSR_BT_GATT_LE_ROLE_SLAVE                               ((CsrBtGattLeRole)0x02)         /* Using the LE radio as Slave/Peripheral */

/* Scan mode flags */
#define CSR_BT_GATT_SCAN_STANDARD                               ((CsrBtGattScanFlags)0x00)      /* standard active scan */
#define CSR_BT_GATT_SCAN_PASSIVE                                ((CsrBtGattScanFlags)0x01)      /* standard passive scan */
#define CSR_BT_GATT_SCAN_WHITELIST                              ((CsrBtGattScanFlags)0x02)      /* enable whitelist filtering */
#define CSR_BT_GATT_SCAN_NONDUP                                 ((CsrBtGattScanFlags)0x04)      /* do not attempt to filter duplicates */

/* LE security flags. Note bonding is controlled by SC based on keyDistribution preferences */
#define CSR_BT_GATT_SECURITY_FLAGS_DEFAULT                      ((CsrBtGattSecurityFlags) CSR_BT_SC_LE_USE_DEFAULT) /* Default low energy authentication requirement  */
#define CSR_BT_GATT_SECURITY_FLAGS_UNAUTHENTICATED              ((CsrBtGattSecurityFlags) CSR_BT_SC_LE_SECURITY_ENCRYPTION) /* Encrypt the link without MITM protection */
#define CSR_BT_GATT_SECURITY_FLAGS_AUTHENTICATED                ((CsrBtGattSecurityFlags) CSR_BT_SC_LE_SECURITY_MITM) /* Encrypt the link with MITM protection */

/*******************************************************************************
 * Database Access response codes
 *******************************************************************************/

/* Operation was successful */
#define CSR_BT_GATT_ACCESS_RES_SUCCESS                          ((CsrBtGattDbAccessRspCode) ATT_RESULT_SUCCESS)
/* The attr handle given was not valid */
#define CSR_BT_GATT_ACCESS_RES_INVALID_HANDLE                   ((CsrBtGattDbAccessRspCode) ATT_RESULT_INVALID_HANDLE)
/* The attr cannot be read */
#define CSR_BT_GATT_ACCESS_RES_READ_NOT_PERMITTED               ((CsrBtGattDbAccessRspCode) ATT_RESULT_READ_NOT_PERMITTED)
/* The attr cannot be written */
#define CSR_BT_GATT_ACCESS_RES_WRITE_NOT_PERMITTED              ((CsrBtGattDbAccessRspCode) ATT_RESULT_WRITE_NOT_PERMITTED)
/* The attr PDU was invalid */
#define CSR_BT_GATT_ACCESS_RES_INVALID_PDU                      ((CsrBtGattDbAccessRspCode) ATT_RESULT_INVALID_PDU)
/* The attr requires authentication before it can be read or written */
#define CSR_BT_GATT_ACCESS_RES_INSUFFICIENT_AUTHENTICATION      ((CsrBtGattDbAccessRspCode) ATT_RESULT_INSUFFICIENT_AUTHENTICATION)
/* Target device doesn't support request */
#define CSR_BT_GATT_ACCESS_RES_REQUEST_NOT_SUPPORTED            ((CsrBtGattDbAccessRspCode) ATT_RESULT_REQUEST_NOT_SUPPORTED)
/* Offset specified was past the end of the long attribute */
#define CSR_BT_GATT_ACCESS_RES_INVALID_OFFSET                   ((CsrBtGattDbAccessRspCode) ATT_RESULT_INVALID_OFFSET)
/* The attr requires authorisation before it can be read or written */
#define CSR_BT_GATT_ACCESS_RES_INSUFFICIENT_AUTHORISATION       ((CsrBtGattDbAccessRspCode) ATT_RESULT_INSUFFICIENT_AUTHORIZATION)
/* Too many prepare writes have been queued */
#define CSR_BT_GATT_ACCESS_RES_PREPARE_QUEUE_FULL               ((CsrBtGattDbAccessRspCode) ATT_RESULT_PREPARE_QUEUE_FULL)
/* No attr found within the given attribute handle range */
#define CSR_BT_GATT_ACCESS_RES_ATTR_NOT_FOUND                   ((CsrBtGattDbAccessRspCode) ATT_RESULT_ATTR_NOT_FOUND)
/* This attr cannot be read or written using the Read Blob Request or Prepare Write Requests */
#define CSR_BT_GATT_ACCESS_RES_NOT_LONG                         ((CsrBtGattDbAccessRspCode) ATT_RESULT_NOT_LONG)
/* The Encryption Key Size used for encrypting this link is insufficient */
#define CSR_BT_GATT_ACCESS_RES_INSUFFICIENT_ENCR_KEY_SIZE       ((CsrBtGattDbAccessRspCode) ATT_RESULT_INSUFFICIENT_ENCR_KEY_SIZE)
/* The attr value length is invalid for the operation */
#define CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH                   ((CsrBtGattDbAccessRspCode) ATT_RESULT_INVALID_LENGTH)
/* The attr request that was requested has encountered an error that was very unlikely */
#define CSR_BT_GATT_ACCESS_RES_UNLIKELY_ERROR                   ((CsrBtGattDbAccessRspCode) ATT_RESULT_UNLIKELY_ERROR)
/* The attr requires encryption before it can be read or written */
#define CSR_BT_GATT_ACCESS_RES_INSUFFICIENT_ENCRYPTION          ((CsrBtGattDbAccessRspCode) ATT_RESULT_INSUFFICIENT_ENCRYPTION)
/* The attr type is not a supported grouping attr as defined by a higher layer specification */
#define CSR_BT_GATT_ACCESS_RES_UNSUPPORTED_GROUP_TYPE           ((CsrBtGattDbAccessRspCode) ATT_RESULT_UNSUPPORTED_GROUP_TYPE)
/* Insufficient Resources to complete the request */
#define CSR_BT_GATT_ACCESS_RES_INSUFFICIENT_RESOURCES           ((CsrBtGattDbAccessRspCode) ATT_RESULT_INSUFFICIENT_RESOURCES)
/* Client Characteristic Configuration Descriptor Improperly Configured */
#define CSR_BT_GATT_ACCESS_RES_CLIENT_CONFIG_IMPROPERLY_CONF    ((CsrBtGattDbAccessRspCode) 0x00FD)
/* A request cannot be serviced because an operation that has been previously triggered is still in progress */
#define CSR_BT_GATT_ACCESS_RES_PROCEDURE_ALREADY_IN_PROGRESS    ((CsrBtGattDbAccessRspCode) 0x00FE)
/* The attribute value is out of range as defined by a Profile or Service specification */
#define CSR_BT_GATT_ACCESS_RES_OUT_OF_RANGE                     ((CsrBtGattDbAccessRspCode) 0x00FF)

/* GATT error codes for the CSR_BT_SUPPLIER_GATT */
#define CSR_BT_GATT_RESULT_SUCCESS                              ((CsrBtResultCode)0x0000) /* Not an error */
#define CSR_BT_GATT_RESULT_UNACCEPTABLE_PARAMETER               ((CsrBtResultCode)0x0001) /* Invalid/unacceptable parameters */
#define CSR_BT_GATT_RESULT_UNKNOWN_CONN_ID                      ((CsrBtResultCode)0x0002) /* Unknown connection id */
#define CSR_BT_GATT_RESULT_ALREADY_ACTIVATED                    ((CsrBtResultCode)0x0003) /* App have called activate req before */
#define CSR_BT_GATT_RESULT_INTERNAL_ERROR                       ((CsrBtResultCode)0x0004) /* Internal GATT Error */
#define CSR_BT_GATT_RESULT_INSUFFICIENT_NUM_OF_HANDLES          ((CsrBtResultCode)0x0005) /* Insufficient number of free attribute handles */
#define CSR_BT_GATT_RESULT_ATTR_HANDLES_ALREADY_ALLOCATED       ((CsrBtResultCode)0x0006) /* The application have already allocated attribute handles */
#define CSR_BT_GATT_RESULT_CANCELLED                            ((CsrBtResultCode)0x0007) /* Operation cancelled */
#define CSR_BT_GATT_RESULT_SCATTERNET                           ((CsrBtResultCode)0x0008) /* Scatternet not allowed */
#define CSR_BT_GATT_RESULT_INVALID_LENGTH                       ((CsrBtResultCode)0x0009) /* The attribute value length is invalid for the operation */
#define CSR_BT_GATT_RESULT_RELIABLE_WRITE_VALIDATION_ERROR      ((CsrBtResultCode)0x000A) /* Validation of the written attribute value failed */
#define CSR_BT_GATT_RESULT_INVALID_ATTRIBUTE_VALUE_RECEIVED     ((CsrBtResultCode)0x000B) /* An invalid attribute value is received */
#define CSR_BT_GATT_RESULT_CLIENT_CONFIGURATION_IN_USED         ((CsrBtResultCode)0x000C) /* The application is allready using Client Configuration */
#define CSR_BT_GATT_RESULT_BR_EDR_NO_PRIMARY_SERVICES_FOUND     ((CsrBtResultCode)0x000D) /* No Primary Services that support BR/EDR were found  */
#define CSR_BT_GATT_RESULT_INVALID_HANDLE_RANGE                 ((CsrBtResultCode)0x000E) /* The given handle range is invalid */
#define CSR_BT_GATT_RESULT_PARAM_CONN_UPDATE_LOCAL_REJECT       ((CsrBtResultCode)0x000F) /* A local service/application has rejected connection parameter update request */
#define CSR_BT_GATT_RESULT_WHITE_FILTER_IN_USE                  ((CsrBtResultCode)0x0010) /* The application tries to add/clear its whitelist while using it */
#define CSR_BT_GATT_RESULT_ALREADY_CONNECTED                    ((CsrBtResultCode)0x0011) /* The application is already connected to the given address */
#define CSR_BT_GATT_RESULT_ALREADY_CONNECTING                   ((CsrBtResultCode)0x0012) /* The application is already connecting to the given address */
#define CSR_BT_GATT_RESULT_WHITE_LIST_LIMIT_EXCEEDED            ((CsrBtResultCode)0x0013) /* The Procedure fails because the limit of the number of devices on the whitelist is exceeded */    
#define CSR_BT_GATT_RESULT_ALREADY_ADVERTISING                  ((CsrBtResultCode)0x0014) /* The application is already advertising nothing to do */
#define CSR_BT_GATT_RESULT_WHITE_FILTER_SCANRSP_IN_USE          ((CsrBtResultCode)0x0015) /* Whitelist filter is already in use with a differend white list. **DEPRECATED** */
#define CSR_BT_GATT_RESULT_WHITE_FILTER_CONTROL_FAILED          ((CsrBtResultCode)0x0016) /* GATT should not get control over the whitelist during the Central Procedure */
#define CSR_BT_GATT_RESULT_BUSY                                 ((CsrBtResultCode)0x0017) /* GATT can not proccess current request. */

/* Report event types */
#define CSR_BT_GATT_EVENT_CONNECTABLE_UNDIRECTED                ((CsrBtGattReportEvent)HCI_ULP_EV_ADVERT_CONNECTABLE_UNDIRECTED)
#define CSR_BT_GATT_EVENT_CONNECTABLE_DIRECTED                  ((CsrBtGattReportEvent)HCI_ULP_EV_ADVERT_CONNECTABLE_DIRECTED)
#define CSR_BT_GATT_EVENT_SCANNABLE_UNDIRECTED                  ((CsrBtGattReportEvent)HCI_ULP_EV_ADVERT_DISCOVERABLE)
#define CSR_BT_GATT_EVENT_NON_CONNECTABLE                       ((CsrBtGattReportEvent)HCI_ULP_EV_ADVERT_NON_CONNECTABLE)
#define CSR_BT_GATT_EVENT_SCAN_RESPONSE                         ((CsrBtGattReportEvent)HCI_ULP_EV_ADVERT_SCAN_RESPONSE)

/* Attribute Permission flags */
#define CSR_BT_GATT_PERM_FLAGS_NONE                             ((CsrBtGattPermFlags) 0x0000)                 /* Not Readable Not Writeable */
#define CSR_BT_GATT_PERM_FLAGS_READ                             ((CsrBtGattPermFlags) ATT_PERM_READ)          /* Permit reads */
#define CSR_BT_GATT_PERM_FLAGS_WRITE_CMD                        ((CsrBtGattPermFlags) ATT_PERM_WRITE_CMD)     /* Permit writes without response */
#define CSR_BT_GATT_PERM_FLAGS_WRITE_REQ                        ((CsrBtGattPermFlags) ATT_PERM_WRITE_REQ)     /* Permit writes with response */
#define CSR_BT_GATT_PERM_FLAGS_WRITE                            ((CsrBtGattPermFlags)(ATT_PERM_WRITE_CMD | ATT_PERM_WRITE_REQ)) /* Permit writes with and without response */
#define CSR_BT_GATT_PERM_FLAGS_AUTH_SIGNED_WRITES               ((CsrBtGattPermFlags)(ATT_PERM_AUTHENTICATED | ATT_PERM_WRITE_CMD)) /* Permit authenticated signed writes */
    
                                                                                       
/*******************************************************************************
 * Attribute flags
 *******************************************************************************/

/* No attribute flags*/
#define CSR_BT_GATT_ATTR_FLAGS_NONE                             ((CsrBtGattAttrFlags) 0x0000)
/* Attribute length can be changed */
#define CSR_BT_GATT_ATTR_FLAGS_DYNLEN                           ((CsrBtGattAttrFlags) ATT_ATTR_DYNLEN)
/* Read access to the attribute sends CSR_BT_GATT_DB_ACCESS_READ_IND
 * to the application */
#define CSR_BT_GATT_ATTR_FLAGS_IRQ_READ                         ((CsrBtGattAttrFlags) ATT_ATTR_IRQ_R)
/* Write access to the attribute sends CSR_BT_GATT_DB_ACCESS_WRITE_IND
 * to the application */
#define CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE                        ((CsrBtGattAttrFlags) ATT_ATTR_IRQ_W)
/* Legacy Encrypted link required for read access. */
#define CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION                  ((CsrBtGattAttrFlags) ATT_ATTR_SEC_R_ENUM(ATT_ATTR_SEC_ENCRYPTION))
/* Legacy Authenticated (MITM) link required for read access. */
#define CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION              ((CsrBtGattAttrFlags) ATT_ATTR_SEC_R_ENUM(ATT_ATTR_SEC_AUTHENTICATION))
/* Secure Connections Authenticated (SC_MITM) link required for read access. */
#define CSR_BT_GATT_ATTR_FLAGS_READ_SC_AUTHENTICATION           ((CsrBtGattAttrFlags) ATT_ATTR_SEC_R_ENUM(ATT_ATTR_SEC_SC_AUTHENTICATION))
/* Legacy Encrypted link required for write access. */
#define CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION                 ((CsrBtGattAttrFlags) ATT_ATTR_SEC_W_ENUM(ATT_ATTR_SEC_ENCRYPTION))
/* Legacy Authenticated (MITM) link required for write access. */
#define CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION             ((CsrBtGattAttrFlags) ATT_ATTR_SEC_W_ENUM(ATT_ATTR_SEC_AUTHENTICATION))
/* Secure Connections Authenticated (SC_MITM) link required for write access. */
#define CSR_BT_GATT_ATTR_FLAGS_WRITE_SC_AUTHENTICATION          ((CsrBtGattAttrFlags) ATT_ATTR_SEC_W_ENUM(ATT_ATTR_SEC_SC_AUTHENTICATION))
/* Authorisation (require application access ind/rsp before read/write is allowed).
   I.e. a CSR_BT_GATT_DB_ACCESS_READ_IND or a CSR_BT_GATT_DB_ACCESS_WRITE_IND it sent to the application */
#define CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION                    ((CsrBtGattAttrFlags) ATT_ATTR_AUTHORIZATION)
/* Encryption key size checks required. 
   I.e. a CSR_BT_GATT_DB_ACCESS_READ_IND or a CSR_BT_GATT_DB_ACCESS_WRITE_IND it sent to the application */
#define CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE                    ((CsrBtGattAttrFlags) ATT_ATTR_ENC_KEY_REQUIREMENTS)
/* Disable access or LE radio */
#define CSR_BT_GATT_ATTR_FLAGS_DISABLE_LE                       ((CsrBtGattAttrFlags) ATT_ATTR_DISABLE_ACCESS_LE)
/* Disable access on BR/EDR radio */
#define CSR_BT_GATT_ATTR_FLAGS_DISABLE_BREDR                    ((CsrBtGattAttrFlags) ATT_ATTR_DISABLE_ACCESS_BR_EDR)

/*******************************************************************************
 * Characteristic Properties bit fiels definition
 *******************************************************************************/

/* If set, permits broadcasts of the Charac Value using Charac Configuration Descriptor. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST                 ((CsrBtGattPropertiesBits) ATT_PERM_CONFIGURE_BROADCAST)
/* If set, permits reads of the Charac Value. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_READ                      ((CsrBtGattPropertiesBits) ATT_PERM_READ)
/* If set, permit writes of the Charac Value without response. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE    ((CsrBtGattPropertiesBits) ATT_PERM_WRITE_CMD)
/* If set, permits writes of the Charac Value with response. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_WRITE                     ((CsrBtGattPropertiesBits) ATT_PERM_WRITE_REQ)
/* If set, permits notifications of a Charac Value without acknowledgment. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY                    ((CsrBtGattPropertiesBits) ATT_PERM_NOTIFY)
/*If set, permits indications of a Charac Value with acknowledgment. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE                  ((CsrBtGattPropertiesBits) ATT_PERM_INDICATE)
/* If set, permits signed writes to the Charac Value. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES        ((CsrBtGattPropertiesBits) ATT_PERM_AUTHENTICATED)
/* If set, additional charac properties are defined in the Charac Ext Properties Descriptor. */
#define CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES       ((CsrBtGattPropertiesBits) ATT_PERM_EXTENDED)

/* Characteristic Extended Properties bit fiels definition. The
 * Characteristic Extended Properties bit field describes additional
 * properties on how the Characteristic Value can be used, or how the
 * characteristic descriptors can be accessed */

/* If set, permits reliable writes of the Charac Value. (0x0001)*/
#define CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE        ((CsrBtGattExtPropertiesBits) ATT_PERM_RELIABLE_WRITE)

/* If set, permits writes to the characteristic descriptor. (0x0002) */
#define CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX             ((CsrBtGattExtPropertiesBits) ATT_PERM_WRITE_AUX)

/* Client Characteristic Configuration bit definition */
#define CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT                ((CsrBtGattCliConfigBits) 0x0000)
#define CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION           ((CsrBtGattCliConfigBits) 0x0001)
#define CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION             ((CsrBtGattCliConfigBits) 0x0002)

/* Server Characteristic Configuration bit definition */
#define CSR_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE                ((CsrBtGattSrvConfigBits) 0x0000)
#define CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST              ((CsrBtGattSrvConfigBits) 0x0001)

/* Characteristic Format types use in the Characteristic Presentation Format declaration */
#define CSR_BT_GATT_CHARAC_FORMAT_RFU                           ((CsrBtGattFormats) 0x00) /* Reserved for future used */
#define CSR_BT_GATT_CHARAC_FORMAT_BOOLEAN                       ((CsrBtGattFormats) 0x01) /* Unsigned 1-bit, 0 = FALSE, 1 = TRUE */
#define CSR_BT_GATT_CHARAC_FORMAT_2BIT                          ((CsrBtGattFormats) 0x02) /* Unsigned 2-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_NIBBLE                        ((CsrBtGattFormats) 0x03) /* Unsigned 4-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT8                         ((CsrBtGattFormats) 0x04) /* Unsigned 8-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT12                        ((CsrBtGattFormats) 0x05) /* Unsigned 12-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT16                        ((CsrBtGattFormats) 0x06) /* Unsigned 16-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT24                        ((CsrBtGattFormats) 0x07) /* Unsigned 24-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT32                        ((CsrBtGattFormats) 0x08) /* Unsigned 32-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT48                        ((CsrBtGattFormats) 0x09) /* Unsigned 48-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT64                        ((CsrBtGattFormats) 0x0A) /* Unsigned 64-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_UINT128                       ((CsrBtGattFormats) 0x0B) /* Unsigned 128-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT8                         ((CsrBtGattFormats) 0x0C) /* Signed 8-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT12                        ((CsrBtGattFormats) 0x0D) /* Signed 12-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT16                        ((CsrBtGattFormats) 0x0E) /* Signed 16-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT24                        ((CsrBtGattFormats) 0x0F) /* Signed 24-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT32                        ((CsrBtGattFormats) 0x10) /* Signed 32-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT48                        ((CsrBtGattFormats) 0x11) /* Signed 48-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT64                        ((CsrBtGattFormats) 0x12) /* Signed 64-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_SINT128                       ((CsrBtGattFormats) 0x13) /* Signed 128-bit integer */
#define CSR_BT_GATT_CHARAC_FORMAT_FLOAT32                       ((CsrBtGattFormats) 0x14) /* IEEE-754 32-bit floating point */
#define CSR_BT_GATT_CHARAC_FORMAT_FLOAT64                       ((CsrBtGattFormats) 0x15) /* IEEE-754 64-bit floating point */
#define CSR_BT_GATT_CHARAC_FORMAT_SFLOAT                        ((CsrBtGattFormats) 0x16) /* IEEE-11073 16-bit SFLOAT */
#define CSR_BT_GATT_CHARAC_FORMAT_FLOAT                         ((CsrBtGattFormats) 0x17) /* IEEE-11073 32-bit FLOAT */
#define CSR_BT_GATT_CHARAC_FORMAT_DUINT16                       ((CsrBtGattFormats) 0x18) /* IEEE-20601 format */
#define CSR_BT_GATT_CHARAC_FORMAT_UTF8S                         ((CsrBtGattFormats) 0x19) /* UTF8-String */
#define CSR_BT_GATT_CHARAC_FORMAT_UTF16S                        ((CsrBtGattFormats) 0x1A) /* UTF16-String */
#define CSR_BT_GATT_CHARAC_FORMAT_STRUCT                        ((CsrBtGattFormats) 0x1B) /* Opaque structure */

/* Characteristic Peripheral Privacy Flag types use in the Peripheral Privacy Flag Characteristis */
#define CSR_BT_GATT_PERIPHERAL_PRIVACY_DISABLED                 ((CsrBtGattPeripheralPrivacyFlag) 0x00) /* Privacy is disabled */
#define CSR_BT_GATT_PERIPHERAL_PRIVACY_ENABLED                  ((CsrBtGattPeripheralPrivacyFlag) 0x01) /* Privacy is enabled */

/* Defines the event that the application can subscribe for */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_NONE                   ((CsrBtGattEventMask) 0x00000000)
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS   ((CsrBtGattEventMask) 0x00000001) /* LE/BR/EDR link status */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_WHITELIST_CHANGE       ((CsrBtGattEventMask) 0x00000002) /* Whitelist changes. DEPRECATED */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PARAM_CONN_UPDATE_IND  ((CsrBtGattEventMask) 0x00000004) /* Slave initiate LE Conn Param Update */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_RESTART_IND            ((CsrBtGattEventMask) 0x00000008) /* Ind received if Advertise or Scan cannot be restarted */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SCAN_RSP_STATUS        ((CsrBtGattEventMask) 0x00000010) /* Ind received when advertise Filter Policy regarding scan response data changes  */
#define CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SERVICE_CHANGED        ((CsrBtGattEventMask) 0x00000020) /* Ind received when the services database on a peer device is changes, i.e. added, removed or modified  */

/* Special check requirements for read/write access indications */
#define CSR_BT_GATT_ACCESS_CHECK_NONE                           ((CsrBtGattAccessCheck) 0x0000) /* no auth/strength checks */
#define CSR_BT_GATT_ACCESS_CHECK_AUTHORISATION                  ((CsrBtGattAccessCheck) 0x0001) /* authorisation access check */
#define CSR_BT_GATT_ACCESS_CHECK_ENCR_KEY_SIZE                  ((CsrBtGattAccessCheck) 0x0002) /* encryption key size check */
#define CSR_BT_GATT_ACCESS_CHECK_RELIABLE_WRITE                 ((CsrBtGattAccessCheck) 0x0004) /* reliable write - app shall wait for
                                                                                                 * final 'execute write ok' before committing
                                                                                                 * database changes */

/* local handle where the preferred connection parameteres are stored in the database */
#define CSR_BT_GATT_ATTR_HANDLE_CONNECTION_PARAMS    ((CsrBtGattHandle)(7))



/*******************************************************************************
 * Advertising/scan-response AD Type values for Low Energy.
 *
 * Note GATT is responsible of setting and inserting the Flags 
 * AD type (CSR_BT_GATT_AD_TYPE_FLAGS) into the Advertising Data.
 * E.g. the application shall never add the Flags AD type to the 
 * Advertising or Scan response Data
 *******************************************************************************/
#define CSR_BT_GATT_AD_TYPE_FLAGS                               (CSR_BT_EIR_DATA_TYPE_FLAGS) 
#define CSR_BT_GATT_AD_TYPE_16BIT_UUID_LIST_INCOMPLETE          (CSR_BT_EIR_DATA_TYPE_MORE_16_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_16BIT_UUID_LIST_COMPLETE            (CSR_BT_EIR_DATA_TYPE_COMPLETE_16_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_128BIT_UUID_LIST_INCOMPLETE         (CSR_BT_EIR_DATA_TYPE_MORE_128_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_128BIT_UUID_LIST_COMPLETE           (CSR_BT_EIR_DATA_TYPE_COMPLETE_128_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_LOCAL_NAME_SHORT                    (CSR_BT_EIR_DATA_TYPE_SHORT_LOCAL_NAME)
#define CSR_BT_GATT_AD_TYPE_LOCAL_NAME_COMPLETE                 (CSR_BT_EIR_DATA_TYPE_COMPLETE_LOCAL_NAME)
#define CSR_BT_GATT_AD_TYPE_TX_POWER_LEVEL                      (CSR_BT_EIR_DATA_TYPE_TX_POWER)
#define CSR_BT_GATT_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE     (CSR_BT_EIR_DATA_TYPE_SLAVE_CONN_INTERVAL_RANGE)
#define CSR_BT_GATT_AD_TYPE_SERVICE_SOLICITATION_16BIT_UUID     (CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_16_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_SERVICE_SOLICITATION_128BIT_UUID    (CSR_BT_EIR_DATA_TYPE_SERV_SOLICITATION_128_BIT_UUID)
#define CSR_BT_GATT_AD_TYPE_SERVICE_DATA                        (CSR_BT_EIR_DATA_TYPE_SERVICE_DATA)
#define CSR_BT_GATT_AD_TYPE_PUBLIC_TARGET_ADDRESS               (CSR_BT_EIR_DATA_TYPE_PUBLIC_TARGET_ADDRESS)
#define CSR_BT_GATT_AD_TYPE_RANDOM_TARGET_ADDRESS               (CSR_BT_EIR_DATA_TYPE_RANDOM_TARGET_ADDRESS)
#define CSR_BT_GATT_AD_TYPE_APPEARANCE                          (CSR_BT_EIR_DATA_TYPE_APPEARANCE)
#define CSR_BT_GATT_AD_TYPE_MANUFACTURER_SPECIFIC_DATA          (CSR_BT_EIR_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

/* Downstream */
#define CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST              (0x0000)

#define CSR_BT_GATT_REGISTER_REQ                        ((CsrBtGattPrim)(0x0000 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_UNREGISTER_REQ                      ((CsrBtGattPrim)(0x0001 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ALLOC_REQ                        ((CsrBtGattPrim)(0x0002 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DB_DEALLOC_REQ                      ((CsrBtGattPrim)(0x0003 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ADD_REQ                          ((CsrBtGattPrim)(0x0004 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DB_REMOVE_REQ                       ((CsrBtGattPrim)(0x0005 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ACCESS_RES                       ((CsrBtGattPrim)(0x0006 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_EVENT_SEND_REQ                      ((CsrBtGattPrim)(0x0007 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_CENTRAL_REQ                         ((CsrBtGattPrim)(0x0008 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PERIPHERAL_REQ                      ((CsrBtGattPrim)(0x0009 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DISCONNECT_REQ                      ((CsrBtGattPrim)(0x000A + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_SCAN_REQ                            ((CsrBtGattPrim)(0x000B + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))  
#define CSR_BT_GATT_ADVERTISE_REQ                       ((CsrBtGattPrim)(0x000C + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_SERVICES_REQ               ((CsrBtGattPrim)(0x000D + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_REQ                 ((CsrBtGattPrim)(0x000E + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_REQ     ((CsrBtGattPrim)(0x000F + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_FIND_INCL_SERVICES_REQ              ((CsrBtGattPrim)(0x0010 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_READ_REQ                            ((CsrBtGattPrim)(0x0011 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))    
#define CSR_BT_GATT_READ_BY_UUID_REQ                    ((CsrBtGattPrim)(0x0012 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_READ_MULTI_REQ                      ((CsrBtGattPrim)(0x0013 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_WRITE_REQ                           ((CsrBtGattPrim)(0x0014 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_CANCEL_REQ                          ((CsrBtGattPrim)(0x0015 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_WRITE_CLIENT_CONFIGURATION_REQ      ((CsrBtGattPrim)(0x0016 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_BREDR_CONNECT_REQ                   ((CsrBtGattPrim)(0x0017 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_BREDR_ACCEPT_REQ                    ((CsrBtGattPrim)(0x0018 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_SCAN_REQ                      ((CsrBtGattPrim)(0x0019 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_ADVERTISE_REQ                 ((CsrBtGattPrim)(0x001A + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONNECTION_REQ                ((CsrBtGattPrim)(0x001B + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_ADD_REQ                   ((CsrBtGattPrim)(0x001C + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_CLEAR_REQ                 ((CsrBtGattPrim)(0x001D + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_READ_REQ                  ((CsrBtGattPrim)(0x001E + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONN_UPDATE_REQ               ((CsrBtGattPrim)(0x001F + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_SUBSCRIPTION_REQ                    ((CsrBtGattPrim)(0x0020 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_SET_EVENT_MASK_REQ                  ((CsrBtGattPrim)(0x0021 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONN_UPDATE_RES               ((CsrBtGattPrim)(0x0022 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_SECURITY_REQ                        ((CsrBtGattPrim)(0x0023 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GATT_PRIM_DOWNSTREAM_HIGHEST                             (0x0023 + CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST)

/* Upstream */
#define CSR_BT_GATT_PRIM_UPSTREAM_LOWEST                                (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_GATT_REGISTER_CFM                        ((CsrBtGattPrim)(0x0000 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_UNREGISTER_CFM                      ((CsrBtGattPrim)(0x0001 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ALLOC_CFM                        ((CsrBtGattPrim)(0x0002 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_DEALLOC_CFM                      ((CsrBtGattPrim)(0x0003 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ADD_CFM                          ((CsrBtGattPrim)(0x0004 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_REMOVE_CFM                       ((CsrBtGattPrim)(0x0005 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ACCESS_READ_IND                  ((CsrBtGattPrim)(0x0006 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ACCESS_WRITE_IND                 ((CsrBtGattPrim)(0x0007 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_EVENT_SEND_CFM                      ((CsrBtGattPrim)(0x0008 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))  
#define CSR_BT_GATT_CENTRAL_CFM                         ((CsrBtGattPrim)(0x0009 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))    
#define CSR_BT_GATT_PERIPHERAL_CFM                      ((CsrBtGattPrim)(0x000A + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_CONNECT_IND                         ((CsrBtGattPrim)(0x000B + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCONNECT_IND                      ((CsrBtGattPrim)(0x000C + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SCAN_CFM                            ((CsrBtGattPrim)(0x000D + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_REPORT_IND                          ((CsrBtGattPrim)(0x000E + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_ADVERTISE_CFM                       ((CsrBtGattPrim)(0x000F + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_MTU_CHANGED_IND                     ((CsrBtGattPrim)(0x0010 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_SERVICES_IND               ((CsrBtGattPrim)(0x0011 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_SERVICES_CFM               ((CsrBtGattPrim)(0x0012 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_IND                 ((CsrBtGattPrim)(0x0013 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_CFM                 ((CsrBtGattPrim)(0x0014 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_IND     ((CsrBtGattPrim)(0x0015 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_CFM     ((CsrBtGattPrim)(0x0016 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_FIND_INCL_SERVICES_IND              ((CsrBtGattPrim)(0x0017 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_FIND_INCL_SERVICES_CFM              ((CsrBtGattPrim)(0x0018 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_CFM                            ((CsrBtGattPrim)(0x0019 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_MULTI_CFM                      ((CsrBtGattPrim)(0x001A + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_BY_UUID_IND                    ((CsrBtGattPrim)(0x001B + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_BY_UUID_CFM                    ((CsrBtGattPrim)(0x001C + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_EXTENDED_PROPERTIES_CFM        ((CsrBtGattPrim)(0x001D + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_USER_DESCRIPTION_CFM           ((CsrBtGattPrim)(0x001E + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_CLIENT_CONFIGURATION_CFM       ((CsrBtGattPrim)(0x001F + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_SERVER_CONFIGURATION_CFM       ((CsrBtGattPrim)(0x0020 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_PRESENTATION_FORMAT_CFM        ((CsrBtGattPrim)(0x0021 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_AGGREGATE_FORMAT_CFM           ((CsrBtGattPrim)(0x0022 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_READ_PROFILE_DEFINED_DESCRIPTOR_CFM ((CsrBtGattPrim)(0x0023 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_WRITE_CFM                           ((CsrBtGattPrim)(0x0024 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_NOTIFICATION_IND                    ((CsrBtGattPrim)(0x0025 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_BREDR_CONNECT_CFM                   ((CsrBtGattPrim)(0x0026 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_BREDR_ACCEPT_CFM                    ((CsrBtGattPrim)(0x0027 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_SCAN_CFM                      ((CsrBtGattPrim)(0x0028 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_ADVERTISE_CFM                 ((CsrBtGattPrim)(0x0029 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONNECTION_CFM                ((CsrBtGattPrim)(0x002A + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_ADD_CFM                   ((CsrBtGattPrim)(0x002B + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_CLEAR_CFM                 ((CsrBtGattPrim)(0x002C + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_READ_CFM                  ((CsrBtGattPrim)(0x002D + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONN_UPDATE_CFM               ((CsrBtGattPrim)(0x002E + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SERVICE_CHANGED_IND                 ((CsrBtGattPrim)(0x002F + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SUBSCRIPTION_CFM                    ((CsrBtGattPrim)(0x0030 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SET_EVENT_MASK_CFM                  ((CsrBtGattPrim)(0x0031 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND            ((CsrBtGattPrim)(0x0032 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_DB_ACCESS_COMPLETE_IND              ((CsrBtGattPrim)(0x0033 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_WHITELIST_CHANGE_IND                ((CsrBtGattPrim)(0x0034 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONN_UPDATE_IND               ((CsrBtGattPrim)(0x0035 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_PARAM_CONN_CHANGED_IND              ((CsrBtGattPrim)(0x0036 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SECURITY_CFM                        ((CsrBtGattPrim)(0x0037 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SCAN_IND                            ((CsrBtGattPrim)(0x0038 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_ADVERTISE_IND                       ((CsrBtGattPrim)(0x0039 + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GATT_SCAN_RSP_STATUS_IND                 ((CsrBtGattPrim)(0x003A + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_GATT_PRIM_UPSTREAM_HIGHEST                               (0x003A + CSR_BT_GATT_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_GATT_PRIM_DOWNSTREAM_COUNT               (CSR_BT_GATT_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_GATT_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_GATT_PRIM_UPSTREAM_COUNT                 (CSR_BT_GATT_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_GATT_PRIM_UPSTREAM_LOWEST)

/*******************************************************************************
 * Common structures
 *******************************************************************************/
typedef struct
{
    CsrBtGattHandle     attrHandle;             /* The handle of the attribute to be written */
    CsrUint16           offset;                 /* Reserved for future used */
    CsrUint16           valueLength;            /* Length of the value */
    CsrUint8           *value;                  /* Pointer to Value */
} CsrBtGattAttrWritePairs;   

typedef struct 
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
} CsrBtGattStdCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtConnId         btConnId;               /* Connection identifier */
} CsrBtGattStdBtConnIdCfm;

typedef struct
{
    CsrUint8            adType;                 /* Advertising data type, aka 'tag' */
    CsrUint8            interval;               /* Interval between pattern checks. 0 only matches at start */
    CsrUint8            dataLength;             /* Length of data pattern to match */
    CsrUint8            *data;                  /* Data pattern to match for */
} CsrBtGattDataFilter;

/*******************************************************************************
 * Primitive signal type definitions - APPLICATION INTERFACE
 *******************************************************************************/

/* Covers LE connect procedures */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtTypedAddr      address;                /* Peer address (may be all-zeros if whitelist) */
    CsrBtGattConnFlags  flags;                  /* Connection flags */
    CsrUint16           preferredMtu;           /* The MTU the application prefers. 0 = no preference */
} CsrBtGattCentralReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattCentralCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtTypedAddr      address;                /* Peer address (may be all-zeros if whitelist/undirected) */
    CsrBtGattConnFlags  flags;                  /* Connection flags */
    CsrUint8            advertisingDataLength;  /* Length of advertising data, max 31 */
    CsrUint8           *advertisingData;        /* pointer to advertising data */
    CsrUint8            scanResponseDataLength; /* Length of scan-response data, max 31 */
    CsrUint8           *scanResponseData;       /* Pointer to scan-response data */
    CsrUint16           preferredMtu;           /* The MTU the application prefers. 0 = no preference */
} CsrBtGattPeripheralReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattPeripheralCfm;

/* Covers BR/EDR connect procedures */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtTypedAddr      address;                /* Peer address (may be all-zeros if whitelist) */
    CsrBtGattConnFlags  flags;                  /* Connection flags */
} CsrBtGattBredrConnectReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattBredrConnectCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtGattConnFlags  flags;                  /* Connection flags */
} CsrBtGattBredrAcceptReq;

/* Note: CsrBtGattBredrAcceptCfm will return immediately with a
 * btConnId of 0 as the L2CAP/ATT CID has not yet been established */
typedef CsrBtGattStdBtConnIdCfm CsrBtGattBredrAcceptCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattConnInfo   connInfo;               /* Connection info flags (radio type etc.) */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrUint16           mtu;                    /* Maximum packet size */
    CsrBtGattLeRole     leRole;                 /* Defines which role the connection has on the LE Radio */    
} CsrBtGattConnectInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtConnId         btConnId;               /* connection instance index */
} CsrBtGattDisconnectReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtReasonCode     reasonCode;             /* Reason code */
    CsrBtSupplier       reasonSupplier;         /* Reason code supplier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrBtGattConnInfo   connInfo;               /* Connection info flags (radio type etc.) */
} CsrBtGattDisconnectInd;

/* Covers LE Scanning procedures */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBool             enable;                 /* TRUE scanning is enable */
    CsrBtGattScanFlags  scanFlags;              /* What sort of scan to use */
    CsrUint8            filterAddrCount;        /* Number of address filters */
    CsrBtTypedAddr     *filterAddr;             /* Address filter. Empty means all */
    CsrUint8            filterDataCount;        /* Number of data filters */
    CsrBtGattDataFilter *filterData;            /* Data filters */
} CsrBtGattScanReq;

typedef CsrBtGattStdCfm CsrBtGattScanCfm;
typedef CsrBtGattStdCfm CsrBtGattScanInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtGattReportEvent eventType;             /* Report type, CSR_BT_GATT_EVENT__ */
    CsrBtTypedAddr      address;                /* Current peer address */
    CsrBtTypedAddr      permanentAddress;       /* Permanent (resolved) peer address */
    CsrUint8            lengthData;             /* Length of report data */
    CsrUint8            data[CSR_BT_CM_LE_MAX_REPORT_LENGTH]; /* Report data */
    CsrInt8             rssi;                   /* RSSI */
} CsrBtGattReportInd;

/* Covers LE Advertise  procedures */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBool             enable;                 /* TRUE advertising is enable */
    CsrBtGattConnFlags  advertisingFlags;       /* Special advertising mode flags */
    CsrUint8            advertisingDataLength;  /* Length of advertising data, max 31 */
    CsrUint8           *advertisingData;        /* pointer to advertising data */
    CsrUint8            scanResponseDataLength; /* Length of scan-response data, max 31 */
    CsrUint8           *scanResponseData;       /* Pointer to scan-response data */
} CsrBtGattAdvertiseReq;

typedef CsrBtGattStdCfm CsrBtGattAdvertiseCfm;
typedef CsrBtGattStdCfm CsrBtGattAdvertiseInd;

/* Covers Registration and Un-register an application instance to Gatt */
typedef struct
{
    CsrBtGattPrim          type;                   /* Identity */
    CsrSchedQid            qid;                    /* Application handle */
    CsrUint16              context;                /* Value returned in CsrBtGattRegisterCfm */
} CsrBtGattRegisterReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* An application handle provide by GATT */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrUint16           context;                /* Value returned from CsrBtGattRegisterReq */
} CsrBtGattRegisterCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
} CsrBtGattUnregisterReq;

typedef CsrBtGattStdCfm CsrBtGattUnregisterCfm;

/* Covers DataBase Configuration */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrUint16           numOfAttrHandles;       /* Number of attribute handles */
    CsrUint16           preferredStartHandle;   /* The StartHandle the application prefers. 0 = no preference */
} CsrBtGattDbAllocReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtGattHandle     start;                  /* Start attribute handle */
    CsrBtGattHandle     end;                    /* End attribute handle */
    CsrUint16           preferredStartHandle;   /* The preferredStartHandle given in CsrBtGattDbAllocReq */
} CsrBtGattDbAllocCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
} CsrBtGattDbDeallocReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtGattHandle     start;                  /* Start attribute handle */
    CsrBtGattHandle     end;                    /* End attribute handle */
} CsrBtGattDbDeallocCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application handle */
    CsrBtGattDb         *db;                    /* Database */
} CsrBtGattDbAddReq;

typedef CsrBtGattStdCfm CsrBtGattDbAddCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application handle */
    CsrBtGattHandle     start;                  /* Start attribute handle for removal (inclusive) */
    CsrBtGattHandle     end;                    /* End attribute handle for removal (inclusive) */
} CsrBtGattDbRemoveReq;

typedef struct 
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result supplier */
    CsrUint16           numOfAttr;              /* Number of attributes removed */
} CsrBtGattDbRemoveCfm;

typedef struct
{
    CsrBtGattPrim           type;               /* Identity */
    CsrBtGattId             gattId;             /* Application handle */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrBtGattHandle         attrHandle;         /* The handle of the attribute */
    CsrUint16               offset;             /* The offset of the first octet to be accessed */
    CsrUint16               maxRspValueLength;  /* The maximum length that the value of the attribute must have */
    CsrBtGattAccessCheck    check;              /* Special conditions that needs to be checked */
    CsrBtGattConnInfo       connInfo;           /* Connection info flags (radio type etc.) */
    CsrBtTypedAddr          address;            /* Peer address */
} CsrBtGattDbAccessReadInd;

typedef struct
{
    CsrBtGattPrim           type;               /* Identity */
    CsrBtGattId             gattId;             /* Application handle */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrBtGattAccessCheck    check;              /* Special conditions that needs to be checked */
    CsrBtGattConnInfo       connInfo;           /* Connection info flags (radio type etc.) */
    CsrBtTypedAddr          address;            /* Peer address */
    CsrUint16               writeUnitCount;     /* Number of sub-write units in list */
    CsrBtGattAttrWritePairs *writeUnit;         /* Array of sub-write units. Only offset/value/valueLength should be used. */
    CsrBtGattHandle         attrHandle;         /* The handle of the attribute. Return this handle in the AccessRes. */
} CsrBtGattDbAccessWriteInd;

typedef struct
{
    CsrBtGattPrim            type;              /* Identity */
    CsrBtGattId              gattId;            /* Application handle */
    CsrBtConnId              btConnId;          /* Connection identifier */
    CsrBtGattHandle          attrHandle;        /* The handle of the attribute */
    CsrBtGattDbAccessRspCode responseCode;      /* Database Access response code */
    CsrUint16                valueLength;       /* Length of the attribute that has been read */
    CsrUint8                *value;             /* The value of the attribute that has been read*/
} CsrBtGattDbAccessRes;

typedef struct
{
    CsrBtGattPrim           type;               /* Identity: CSR_BT_GATT_DB_ACCESS_COMPLETE_IND */
    CsrBtGattId             gattId;             /* Application handle */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrBtGattConnInfo       connInfo;           /* Connection info flags (radio type etc.) */
    CsrBtTypedAddr          address;            /* Peer address */
    CsrBtGattHandle         attrHandle;         /* The handle of the attribute */
    CsrBool                 commit;             /* True only if all prepare writes succedeed */
} CsrBtGattDbAccessCompleteInd;

/* Covers Server Initiated Notification, Indication, and Service Changed */

/* Intern used Event Send Request flags */

/* Send a Notification Event to the Client */
#define CSR_BT_GATT_NOTIFICATION_EVENT      ((CsrUint16) ATT_HANDLE_VALUE_NOTIFICATION) 
/* Send an Indication Event to the Client */
#define CSR_BT_GATT_INDICATION_EVENT        ((CsrUint16) ATT_HANDLE_VALUE_INDICATION)   
/* Received a long Notification/Indication Event from server. Note for intern used only */
#define CSR_BT_GATT_LONG_ATTRIBUTE_EVENT    ((CsrUint16) 0x0000)
/* Send a Service Changed Event to the Client. Note for intern used only */
#define CSR_BT_GATT_SERVICE_CHANGED_EVENT   ((CsrUint16) 0xFFFF)

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application handle */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattHandle     attrHandle;             /* Attribute Handle */
    CsrBtGattHandle     endGroupHandle;         /* End of Affected Attribute Handle Range, Only used by service Changed*/
    CsrUint16           flags;                  /* Request Flag for internal use only */
    CsrUint16           valueLength;            /* Length of the Value in octects */
    CsrUint8           *value;                  /* The current value of the attribute */
} CsrBtGattEventSendReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattEventSendCfm;

/* Covers item 1, Server Configuration, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrUint16           mtu;                    /* MTU for the connection */
} CsrBtGattMtuChangedInd;

/* Covers item 2, Primary Service Discovery, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtUuid           uuid;                   /* Service UUID, if CSR_BT_GATT_PRIMARY_SERVICE_UUID all primary services are found */
} CsrBtGattDiscoverServicesReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattHandle     startHandle;            /* Start handle */
    CsrBtGattHandle     endHandle;              /* The End Group handle */
    CsrBtUuid           uuid;                   /* The Service UUID */
} CsrBtGattDiscoverServicesInd;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattDiscoverServicesCfm;

/* Covers item 3, Relationship Discovery, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattHandle     startHandle;            /* Starting handle of the specified service */
    CsrBtGattHandle     endGroupHandle;         /* Ending handle of the specified service */
} CsrBtGattFindInclServicesReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* connection identifier */
    CsrBtGattHandle     attrHandle;             /* Attribute Handle of the Include Service */
    CsrBtGattHandle     startHandle;            /* Starting Handle of the Included Service declaration */
    CsrBtGattHandle     endGroupHandle;         /* The End Group handle */
    CsrBtUuid           uuid;                   /* The Service UUID */
} CsrBtGattFindInclServicesInd;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattFindInclServicesCfm;

/* Covers item 4, Characteristic Discovery, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtUuid           uuid;                   /* Characteristic UUID. If CSR_BT_GATT_CHARACTERISTIC_UUID all characs of a service are found */
    CsrBtGattHandle     startHandle;            /* Starting handle of the specified service */
    CsrBtGattHandle     endGroupHandle;         /* End group handle of the specified service */
} CsrBtGattDiscoverCharacReq;

typedef struct
{
    CsrBtGattPrim           type;               /* Identity */
    CsrBtGattId             gattId;             /* Application identifier */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrBtGattHandle         declarationHandle;  /* Handle for the characteristic declaration*/
    CsrBtGattPropertiesBits property;           /* Characteristic Property */
    CsrBtUuid               uuid;               /* Characteristic UUID */
    CsrBtGattHandle         valueHandle;        /* Characteristic Value Handle */
} CsrBtGattDiscoverCharacInd;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattDiscoverCharacCfm;

/* Covers item 5, Characteristic Descriptor Discovery, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattHandle     startHandle;            /* Starting handle of the specified characteristic value + 1 */
    CsrBtGattHandle     endGroupHandle;         /* End Group handle of the specified characteristic */
} CsrBtGattDiscoverCharacDescriptorsReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtUuid           uuid;                   /* Characteristic Descriptor UUID */
    CsrBtGattHandle     descriptorHandle;       /* The handle of the Characteristic Descriptor declaration */
} CsrBtGattDiscoverCharacDescriptorsInd;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattDiscoverCharacDescriptorsCfm;

/* Covers item 6, Characteristic Value Read, and item 10 Characteristic Descriptor Value Read 
   in the GATT feature table */

/* Intern used Read Request flags */
#define CSR_BT_GATT_READ_VALUE                 ((CsrUint16) 0x0000) /* Reads the Characteristic Value declaration */ 
#define CSR_BT_GATT_READ_EXT_PROPERTIES        ((CsrUint16) 0x0001) /* Reads the Characteristic Extended Properties declaration */
#define CSR_BT_GATT_READ_USER_DESCRIPTOR       ((CsrUint16) 0x0002) /* Reads the Characteristic User Description declaration */
#define CSR_BT_GATT_READ_CLIENT_CONFIGURATION  ((CsrUint16) 0x0003) /* Reads the Client Characteristic Configuration declaration */
#define CSR_BT_GATT_READ_SERVER_CONFIGURATION  ((CsrUint16) 0x0004) /* Reads the Server Characteristic Configuration declaration */
#define CSR_BT_GATT_READ_PRESENTATION_FORMAT   ((CsrUint16) 0x0005) /* Reads the Characteristic Presentation Format declaration */
#define CSR_BT_GATT_READ_AGGREGATE_FORMAT      ((CsrUint16) 0x0006) /* Reads the Characteristic Aggregate Format declaration */
#define CSR_BT_GATT_READ_PROFILE_DEFINED       ((CsrUint16) 0x0007) /* Reads a Profile Defined Characteristic declaration */

typedef struct
{
    CsrBtGattPrim       type;               /* Identity */
    CsrBtGattId         gattId;             /* Application identifier */
    CsrBtConnId         btConnId;           /* Connection identifier */
    CsrBtGattHandle     handle;             /* Characteristic Value Handle */
    CsrUint16           offset;             /* The offset of the first octet that shall be read */
    CsrUint16           flags;              /* For intern use only */
} CsrBtGattReadReq;

typedef struct
{
    CsrBtGattPrim       type;               /* Identity */
    CsrBtGattId         gattId;             /* Application identifier */
    CsrBtResultCode     resultCode;         /* Result code */
    CsrBtSupplier       resultSupplier;     /* Result error code supplier */
    CsrBtConnId         btConnId;           /* Connection identifier */
    CsrUint16           valueLength;        /* Length of the Characteristic Value in octects */
    CsrUint8           *value;              /* Pointer to the Characteristic Value */
    CsrBtGattHandle     handle;             /* Attribute Handle */
} CsrBtGattReadCfm;

typedef struct
{
    CsrBtGattPrim               type;           /* Identity */
    CsrBtGattId                 gattId;         /* Application identifier */
    CsrBtResultCode             resultCode;     /* Result code */
    CsrBtSupplier               resultSupplier; /* Result error code supplier */
    CsrBtConnId                 btConnId;       /* Connection identifier */
    CsrBtGattExtPropertiesBits  extProperties;  /* The Characteristic Extended Properties bit field */
    CsrBtGattHandle             handle;         /* Extended properties descriptor handle */
} CsrBtGattReadExtendedPropertiesCfm;

typedef struct
{
    CsrBtGattPrim           type;           /* Identity */
    CsrBtGattId             gattId;         /* Application identifier */
    CsrBtResultCode         resultCode;     /* Result code */
    CsrBtSupplier           resultSupplier; /* Result error code supplier */
    CsrBtConnId             btConnId;       /* Connection identifier */
    CsrUtf8String           *usrDescription;/* Characteristic User Description UTF-8 String */
    CsrBtGattHandle         handle;         /* User description descriptor handle */
} CsrBtGattReadUserDescriptionCfm;

typedef struct
{
    CsrBtGattPrim           type;           /* Identity */
    CsrBtGattId             gattId;         /* Application identifier */
    CsrBtResultCode         resultCode;     /* Result code */
    CsrBtSupplier           resultSupplier; /* Result error code supplier */
    CsrBtConnId             btConnId;       /* Connection identifier */
    CsrBtGattCliConfigBits  configuration;  /* Client Characteristic Configuration bits */
    CsrBtGattHandle         handle;         /* Client characteristic configuration descriptor handle */
} CsrBtGattReadClientConfigurationCfm;

typedef struct
{
    CsrBtGattPrim           type;           /* Identity */
    CsrBtGattId             gattId;         /* Application identifier */
    CsrBtResultCode         resultCode;     /* Result code */
    CsrBtSupplier           resultSupplier; /* Result error code supplier */
    CsrBtConnId             btConnId;       /* Connection identifier */
    CsrBtGattSrvConfigBits  configuration;  /* Server Characteristic Configuration bits */
    CsrBtGattHandle         handle;         /* Server characteristic configuration descriptor handle */
} CsrBtGattReadServerConfigurationCfm;

typedef struct
{
    CsrBtGattPrim           type;           /* Identity */
    CsrBtGattId             gattId;         /* Application identifier */
    CsrBtResultCode         resultCode;     /* Result code */
    CsrBtSupplier           resultSupplier; /* Result error code supplier */
    CsrBtConnId             btConnId;       /* Connection identifier */
    CsrBtGattFormats        format;         /* Format of the value of this characteristic */
    CsrUint8                exponent;       /* Exponent field determines how the value is formatted */
    CsrUint16               unit;           /* The Unit is a UUID defined in the Assigned Numbers Specification */
    CsrUint8                nameSpace;      /* The Name Space field identify defined in the Assigned Numbers Specification */
    CsrUint16               description;    /* The Description is an enumerated value as defined in the Assigned Numbers Specification */
    CsrBtGattHandle         handle;         /* Presentation format descriptor handle */
} CsrBtGattReadPresentationFormatCfm;

typedef struct
{
    CsrBtGattPrim           type;           /* Identity */
    CsrBtGattId             gattId;         /* Application identifier */
    CsrBtResultCode         resultCode;     /* Result code */
    CsrBtSupplier           resultSupplier; /* Result error code supplier */
    CsrBtConnId             btConnId;       /* Connection identifier */
    CsrUint16               handlesCount;   /* Num of attribute Handles */
    CsrBtGattHandle         *handles;       /* List of Attribute Handles for the Characteristic Presentation Format Declarations */
    CsrBtGattHandle         handle;         /* Aggregate presentation format descriptor handle */
} CsrBtGattReadAggregateFormatCfm;

typedef CsrBtGattReadCfm CsrBtGattReadProfileDefinedDescriptorCfm;

typedef struct
{
    CsrBtGattPrim       type;               /* Identity */
    CsrBtGattId         gattId;             /* Application identifier */
    CsrBtConnId         btConnId;           /* Connection identifier */
    CsrUint16           handlesCount;       /* Num of attribute Handles */
    CsrBtGattHandle     *handles;           /* A set of two or more attribute handles. First handle is returned in CsrBtGattReadMultiCfm */
} CsrBtGattReadMultiReq;

typedef CsrBtGattReadCfm CsrBtGattReadMultiCfm;

typedef struct
{
    CsrBtGattPrim       type;               /* Identity */
    CsrBtGattId         gattId;             /* Application identifier */
    CsrBtConnId         btConnId;           /* Connection identifier */
    CsrBtGattHandle     startHandle;        /* Starting handle from where the read shall start */
    CsrBtGattHandle     endGroupHandle;     /* Ending handle of where the read shall end */
    CsrBtUuid           uuid;               /* Characteristic UUID */
} CsrBtGattReadByUuidReq;

typedef struct
{
    CsrBtGattPrim       type;               /* Identity */
    CsrBtGattId         gattId;             /* Application identifier */
    CsrBtConnId         btConnId;           /* Connection identifier */
    CsrBtGattHandle     valueHandle;        /* Characteristic Value Handle */
    CsrUint16           valueLength;        /* Length of the Characteristic Value in octets */
    CsrUint8           *value;              /* Pointer to the Characteristic Value */
} CsrBtGattReadByUuidInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtUuid           uuid;                   /* Characteristic UUID */
} CsrBtGattReadByUuidCfm;

/* Covers item 7 Characteristic Value Write and item 11 Characteristic Descriptor Value Write, 
   in the GATT feature table */

/* Intern used Write Request flags */

/* Send Write Request to the server */
#define CSR_BT_GATT_WRITE_REQUEST        ((CsrUint16) ATT_WRITE_REQUEST) 
/* Send Write Command to the server */
#define CSR_BT_GATT_WRITE_COMMAND        ((CsrUint16) ATT_WRITE_COMMAND) 
/* Send Signed Write Command to the server, can only be sent to bonded server */
#define CSR_BT_GATT_WRITE_SIGNED_COMMAND ((CsrUint16) (ATT_WRITE_COMMAND | ATT_WRITE_SIGNED))  
/* Send Reliable Write to the server*/
#define CSR_BT_GATT_WRITE_RELIABLE       ((CsrUint16) 0xFFFF)            

typedef struct 
{
    CsrBtGattPrim           type;               /* Identity */
    CsrBtGattId             gattId;             /* Application identifier */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrUint16               flags;              /* For intern use only */
    CsrUint16               attrWritePairsCount;/* Num of attribute to be written */ 
    CsrBtGattAttrWritePairs *attrWritePairs;    /* Handle, value, offset of the attributes that must be written*/  
} CsrBtGattWriteReq;

typedef struct
{
    CsrBtGattPrim           type;               /* Identity */
    CsrBtGattId             gattId;             /* Application identifier */
    CsrBtConnId             btConnId;           /* Connection identifier */
    CsrBtTypedAddr          address;            /* Peer address */
    CsrBtGattHandle         clientConfigHandle; /* Handle for the Client Characteristic Configuration */
    CsrBtGattHandle         valueHandle;        /* Attribute Handle of the Characteristic Value */
    CsrBtGattCliConfigBits  configuration;      /* Client Characteristic Configuration bits */
    CsrBool                 disableReadBlob;    /* If FALSE GATT automatic read blob request when receiving full PDU */
} CsrBtGattWriteClientConfigurationReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* The application handle */
    CsrBtResultCode     resultCode;             /* Result code */
    CsrBtSupplier       resultSupplier;         /* Result error code supplier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattHandle     handle;                 /* Attribute Handle. CSR_BT_GATT_ATTR_HANDLE_INVALID in case of reliable write */
} CsrBtGattWriteCfm;

/* Covers item 8 and 9, Characteristic Value Indication/Notification, in the GATT feature table */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrBtGattHandle     valueHandle;            /* Attribute Handle of the Characteristic Value */
    CsrUint16           valueLength;            /* Length of the Characteristic Value in octects */
    CsrUint8           *value;                  /* Pointer to the Characteristic Value */
    CsrBtGattConnInfo   connInfo;               /* Connection info flags (radio type etc.) */
} CsrBtGattNotificationInd;

/* Allow the application to cancel a given procedure, Note the confirm
 * message is the confirm message of the procedure being cancelled. */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
} CsrBtGattCancelReq;

/* Advanced parameter configurations for scan/advertise/connection */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrUint16           scanInterval;           /* Scanning interval */
    CsrUint16           scanWindow;             /* Scanning window */
} CsrBtGattParamScanReq;

typedef CsrBtGattStdCfm CsrBtGattParamScanCfm;

/* Advanced parameter configurations for scan/advertise/connection */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrUint16           advIntervalMin;         /* Advertising interval minimum */
    CsrUint16           advIntervalMax;         /* Advertising interval maximum */
} CsrBtGattParamAdvertiseReq;

typedef CsrBtGattStdCfm CsrBtGattParamAdvertiseCfm;

/* Connection parameter defaults setup */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrUint16           scanInterval;           /* Scan interval for central */
    CsrUint16           scanWindow;             /* Scan window for central */
    CsrUint16           connIntervalMin;        /* Connection interval minimum */
    CsrUint16           connIntervalMax;        /* Connection interval maximum */
    CsrUint16           connLatency;            /* Default connection latency */
    CsrUint16           supervisionTimeout;     /* Default connection supervision timeout */
    CsrUint16           connAttemptTimeout;     /* Connection attempt timeout */
    CsrUint16           advIntervalMin;         /* Peripheral advertise interval min */
    CsrUint16           advIntervalMax;         /* Peripheral advertise interval max */
    CsrUint16           connLatencyMax;         /* Max acceptable connection latency */
    CsrUint16           supervisionTimeoutMin;  /* Min acceptable supervision timeout */
    CsrUint16           supervisionTimeoutMax;  /* Max acceptable supervision timeout */
} CsrBtGattParamConnectionReq;

typedef CsrBtGattStdCfm CsrBtGattParamConnectionCfm;

/* Connection paramter update */
typedef struct
{
    CsrBtGattPrim                   type;               /* Identity */
    CsrBtGattId                     gattId;             /* Application identifier */
    CsrBtConnId                     btConnId;           /* Connection identifier */
    CsrUint16                       connIntervalMin;    /* Connection interval min */
    CsrUint16                       connIntervalMax;    /* Connection interval max */
    CsrUint16                       connLatency;        /* Connection latency */
    CsrUint16                       supervisionTimeout; /* Supervision timeout */
    CsrUint16                       minimumCeLength;    /* Connection event length min */
    CsrUint16                       maximumCeLength;    /* Connection event length max */
    CsrBool                         accept;             /* For intern use only */
    l2ca_identifier_t               l2caSignalId;       /* For intern use only */    
} CsrBtGattParamConnUpdateReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattParamConnUpdateCfm;

/* Whitelist control */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */   
    CsrUint16           addressCount;           /* Number of address items in 'address' */
    CsrBtTypedAddr      *address;               /* Array */
} CsrBtGattWhitelistAddReq;

typedef CsrBtGattStdCfm CsrBtGattWhitelistAddCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */    
} CsrBtGattWhitelistClearReq;

typedef CsrBtGattStdCfm CsrBtGattWhitelistClearCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */    
} CsrBtGattWhitelistReadReq;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */    
    CsrUint16           addressCount;           /* Number of address items in 'address' */
    CsrBtTypedAddr      *address;               /* Array */
} CsrBtGattWhitelistReadCfm;

/* Notification when subscribed to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SERVICE_CHANGED */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrUint16           startHandle;            /* Start of Affected Attribute Handle Range */
    CsrUint16           endHandle;              /* End of Affected Attribute Handle Range */
    CsrBtGattConnInfo   connInfo;               /* Connection info flags (radio type etc.) */
    CsrBtTypedAddr      permanentAddress;       /* Permanent address */
} CsrBtGattServiceChangedInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrBtGattHandle     valueHandle;            /* Attribute Handle of the Characteristic Value */
    CsrBool             subscribe;              /* TRUE = subscribe, FALSE = unsubscribe */
    CsrBool             disableReadBlob;        /* If FALSE GATT automatic read blob request when receiving full PDU */
} CsrBtGattSubscriptionReq;

typedef CsrBtGattStdCfm CsrBtGattSubscriptionCfm;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtGattEventMask  eventMask;              /* Defines the event(s) to subscribe for */
} CsrBtGattSetEventMaskReq;

typedef CsrBtGattStdCfm CsrBtGattSetEventMaskCfm;

/* Notification when subscribed to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtTypedAddr      address;                /* Peer address */
    CsrBtGattConnInfo   connInfo;               /* Connection info flags (radio type etc.) */
    CsrBool             status;                 /* TRUE = connected, FALSE = disconnected */   
} CsrBtGattPhysicalLinkStatusInd;

/* Notification when subscribed to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SCAN_RSP_STATUS */
typedef struct
{
    CsrBtGattPrim       type;                   /* CSR_BT_GATT_SCAN_RSP_STATUS_IND */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBool             scanRspStatus;          /* Describe if GATT is allowing Scan Request from the whitelist or not */
} CsrBtGattScanRspStatusInd;

/* Notification when subscribed to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_WHITELIST_CHANGE */
typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
} CsrBtGattWhitelistChangeInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrUint16           connIntervalMax;        /* Connection interval max */
    CsrUint16           connIntervalMin;        /* Connection interval min */
    CsrUint16           connLatency;            /* Connection latency */
    CsrUint16           supervisionTimeout;     /* Supervision timeout */
    CsrUint16           identifier;             /* Used to identify the ParamConnUpdate signal */
} CsrBtGattParamConnUpdateInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrUint16           identifier;             /* Used to identify the ParamConnUpdate signal */
    CsrBool             accept;                 /* TRUE - if parameters are acceptable, FALSE - non-acceptable  */ 
} CsrBtGattParamConnUpdateRes;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrUint16           interval;               /* Connection interval (in slots, i.e. x * 0.625ms) */
    CsrUint16           latency;                /* Connection latency (in slots, i.e. x * 0.625ms) */           
    CsrUint16           timeout;                /* Supervision timeout (in 10ms units) */
} CsrBtGattParamConnChangedInd;

typedef struct
{
    CsrBtGattPrim       type;                   /* Identity */
    CsrBtGattId         gattId;                 /* Application identifier */
    CsrBtConnId         btConnId;               /* Connection identifier */
    CsrBtGattSecurityFlags securityRequirements;/* LE security requirements */
} CsrBtGattSecurityReq;

typedef CsrBtGattStdBtConnIdCfm CsrBtGattSecurityCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_GATT_PRIM_H__ */
