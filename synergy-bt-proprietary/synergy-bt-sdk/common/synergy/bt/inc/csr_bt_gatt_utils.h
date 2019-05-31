#ifndef CSR_BT_GATT_UTILS_H__
#define CSR_BT_GATT_UTILS_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_gatt_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilGetEirInfo

  Description: 
    Functions to extract EIR type information from a CsrBtGattReportInd
    message

  Parameters: 
    CsrUint8 *data             - The data received in the CsrBtGattReportInd
                                 message.   
    CsrUint8 eirDataType       - The EIR Data type to search for. The EIR Data 
                                 types are defined in csr_bt_profiles.h.  
    CsrUint8 *returnDataOffset - Offset to where the found EIR Data starts 
    CsrUint8 *returnDataLength - Length of the found EIR Data
   ---------------------------------------------------------------------------- */
CsrBool CsrBtGattUtilGetEirInfo(CsrUint8 *data,
                                CsrUint8 eirDataType,
                                CsrUint8 *returnDataOffset,
                                CsrUint8 *returnDataLength);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateDbEntryFromUuid128

  Description: 
    Functions to create a generic declaration attribute with a 128-Bit uuid 
    atribute type to the local database. This function 
    returns the new head pointer to the caller. The generic declaration attribute 
    with a 128-Bit uuid atribute type will have the following structure:

    +----------+------------+----------------------------+---------------------+
    | Attribute| Attribute  |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type     |                            |                     |
    +----------+------------+----------------------------+---------------------+
    | <0xNNNN> | <A 128-Bit | The data described by      |  Application or     |
    |          |  UUID that | the Attribute Type         |  Profile Specific   |
    |          |  describes |                            |                     |
    |          |  Attribute |                            |                     |
    |          |  Value>    |                            |                     |
    |          |            |                            |                     |
    |    #1    |    #2      |             #3             |         #4          |
    +----------+------------+----------------------------+---------------------+

    Note according to the BLUETOOTH SPECIFICATION Version 4.0 Vol 3
    Part G Section 3.3.3 - Characteristic Descriptor Declarations, higher
    layer profiles may define additional characteristic descriptors 
    that are profile specific. This function may be use for this. 

  Parameters: 
    CsrBtGattDb *head                  - The head pointer, a reference pointer
                                         which point to the first element
                                         in the linked list. 
    CsrBtGattHandle *attrHandle        - The Attribute Handle [#1]. I.e. 
                                         the index of where the generic 
                                         declaration attribute must
                                         be placed in the local database.
                                         Note the 'attrHandle' parameter 
                                         is automatic incremented by 1. 
    CsrBtUuid128 uuid128               - A 128-Bit UUID that describes the
                                         attribute value [#2].
    CsrBtGattPermFlags attrPermission  - The permission flag field is 
                                         use to determined whether read or 
                                         write access is permitted or not.
                                         The permission flags are defined 
                                         in csr_bt_gatt_prim.h and can be 
                                         set to: 
                                         CSR_BT_GATT_PERM_FLAGS_NONE
                                         CSR_BT_GATT_PERM_FLAGS_READ
                                         CSR_BT_GATT_PERM_FLAGS_WRITE_CMD
                                         CSR_BT_GATT_PERM_FLAGS_WRITE_REQ
                                         CSR_BT_GATT_PERM_FLAGS_WRITE
                                         CSR_BT_GATT_PERM_FLAGS_AUTH_SIGNED_WRITES      
                                         Please note security is set in
                                         attrFlags parameter and multiple 
                                         permission flags can be set [#4]. 
    CsrBtGattAttrFlags attrFlags       - The attribute flag field defines 
                                         how generic Declaration can be access 
                                         The attribute flags are defined 
                                         in csr_bt_gatt_prim.h and can 
                                         be set to:
                                         CSR_BT_GATT_ATTR_FLAGS_NONE
                                         CSR_BT_GATT_ATTR_FLAGS_DYNLEN
                                         CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                         CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                         CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                                         CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                         CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                         CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                         CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION
                                         CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                                         CSR_BT_GATT_ATTR_FLAGS_DISABLE_LE
                                         CSR_BT_GATT_ATTR_FLAGS_DISABLE_BREDR

                                         Note multiple attribute flags can be set. 
    CsrUint16 attrValueLength          - The length of the Attribute Value in octets
    const CsrUint8 *attrValue          - The Attribute Value. Note this value shall be 
                                         const data, i.e. not an allocated pointer.[#3].
    CsrBtGattDb **tail                 - The tail pointer, a reference pointer
                                         which point to the last element
                                         in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateDbEntryFromUuid128(CsrBtGattDb        *head,
                                                          CsrBtGattHandle    *attrHandle,
                                                          CsrBtUuid128       uuid128,
                                                          CsrBtGattPermFlags attrPermission,
                                                          CsrBtGattAttrFlags attrFlags,
                                                          CsrUint16          attrValueLength,
                                                          const CsrUint8     *attrValue,
                                                          CsrBtGattDb        **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateDbEntryFromUuid16

  Description: 
    Functions to create a generic declaration attribute with a 16-Bit uuid 
    atribute type to the local database. This function 
    returns the new head pointer to the caller. The generic declaration attribute 
    with a 16-Bit uuid atribute type will have the following structure:

    +----------+------------+----------------------------+---------------------+
    | Attribute| Attribute  |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type     |                            |                     |
    +----------+------------+----------------------------+---------------------+
    | <0xNNNN> | <A 16-Bit  | The data described by      |  Application or     |
    |          |  UUID that | the Attribute Type         |  Profile Specific   |
    |          |  describes |                            |                     |
    |          |  Attribute |                            |                     |
    |          |  Value>    |                            |                     |
    |          |            |                            |                     |
    |    #1    |    #2      |             #3             |         #4          |
    +----------+------------+----------------------------+---------------------+

    Note according to the BLUETOOTH SPECIFICATION Version 4.0 Vol 3
    Part G Section 3.3.3 - Characteristic Descriptor Declarations, higher
    layer profiles may define additional characteristic descriptors 
    that are profile specific. This function may be use for this. 

  Parameters: 
    CsrBtGattDb *head                  - The head pointer, a reference pointer
                                         which point to the first element
                                         in the linked list. 
    CsrBtGattHandle *attrHandle        - The Attribute Handle [#1]. I.e. 
                                         the index of where the generic 
                                         declaration attribute must
                                         be placed in the local database.
                                         Note the 'attrHandle' parameter 
                                         is automatic incremented by 1. 
    CsrBtUuid16 uuid16                 - A 16-Bit UUID that describes the
                                         attribute value [#2].
    CsrBtGattPermFlags attrPermission  - The permission flag field is 
                                         use to determined whether read or 
                                         write access is permitted or not.
                                         The permission flags are defined 
                                         in csr_bt_gatt_prim.h and can be 
                                         set to: 
                                         CSR_BT_GATT_PERM_FLAGS_NONE
                                         CSR_BT_GATT_PERM_FLAGS_READ
                                         CSR_BT_GATT_PERM_FLAGS_WRITE_CMD
                                         CSR_BT_GATT_PERM_FLAGS_WRITE_REQ
                                         CSR_BT_GATT_PERM_FLAGS_WRITE
                                         CSR_BT_GATT_PERM_FLAGS_AUTH_SIGNED_WRITES      
                                         Please note security is set in
                                         attrFlags parameter and multiple 
                                         permission flags can be set [#4]. 
    CsrBtGattAttrFlags attrFlags       - The attribute flag field defines 
                                         how generic Declaration can be access 
                                         The attribute flags are defined 
                                         in csr_bt_gatt_prim.h and can 
                                         be set to:
                                         CSR_BT_GATT_ATTR_FLAGS_NONE
                                         CSR_BT_GATT_ATTR_FLAGS_DYNLEN
                                         CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                         CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                         CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                                         CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                         CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                         CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                         CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION
                                         CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                                         CSR_BT_GATT_ATTR_FLAGS_DISABLE_LE
                                         CSR_BT_GATT_ATTR_FLAGS_DISABLE_BREDR

                                         Note multiple attribute flags can be set. 
    CsrUint16 attrValueLength          - The length of the Attribute Value in octets
    const CsrUint8 *attrValue          - The Attribute Value. Note this value shall be 
                                         const data, i.e. not an allocated pointer.[#3].
    CsrBtGattDb **tail                 - The tail pointer, a reference pointer
                                         which point to the last element
                                         in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateDbEntryFromUuid16(CsrBtGattDb        *head,
                                                         CsrBtGattHandle    *attrHandle,
                                                         CsrBtUuid16        uuid16,
                                                         CsrBtGattPermFlags attrPermission,
                                                         CsrBtGattAttrFlags attrFlags,
                                                         CsrUint16          attrValueLength,
                                                         const CsrUint8     *attrValue,
                                                         CsrBtGattDb        **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreatePrimaryServiceWith16BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.1 - Service Definition 

  Description: 
    Functions to Create a Primary Service Declaration Attribute to the local
    database with a 16-Bit uuid as defined defined in [Vol 3]. This function 
    returns the new head pointer to the caller. The created Primary Service 
    Declaration Attribute will have the following structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2800 -  |        <16-Bit UUID>       |  Read Only,         |
    |          | UUID for  |                            |  No Authentication, |
    |          | Primary   |                            |  No Authorisation   |
    |          | Service   |                            |                     |
    |    #1    |           |             #2             |                     |
    +----------+-----------+----------------------------+---------------------+

    Note a Service definition shall contain a Service Declaration and may 
    contain zero or more Include definitions and Characterictis definitions. 
    All Include Definitions shall immediately follow the Service Declaration 
    and can be create by using one of the functions:
    CsrBtGattUtilCreateIncludeDefinitionWithUuid
    CsrBtGattUtilCreateIncludeDefinitionWithoutUuid
    and all Characteristic Definitions shall immediately following the last 
    Include Definition. In case that no Include Definitions exists all 
    Characteristic Definitions must instead immediately follow the Service 
    Declaration. A Characteristic Definitions can be created by using one of 
    the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Whenever it is possible it is recommended that services definitions with
    service declarations using 16-Bit UUIDs is listed sequentially 
    and services definitons with service declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head           - The head pointer, a reference pointer
                                  which point to the first element
                                  in the linked list. 
    CsrBtGattHandle *attrHandle - The Attribute Handle [#1]. I.e. 
                                  the index of where the Primary
                                  Service Declaration must
                                  be placed in the local database.
                                  Note the 'attrHandle' parameter 
                                  is automatic incremented by 1. 
    CsrBtUuid16 serviceUuid16   - A 16-Bit Service UUID [#2].
    CsrBool leOnly              - If the Service Definition shall be 
                                  supported over BR/EDR 'leOnly' shall 
                                  be set to FALSE otherwise TRUE. If set 
                                  to FALSE GATT will automatic generate 
                                  and publish a generic SDP record. In 
                                  order for GATT to do this a complete 
                                  Service definition SHALL be created, 
                                  i.e. all Include Definitions, 
                                  Characteristic Definitions, and all 
                                  Characteristic Descriptor Definitions 
                                  SHALL be created before 
                                  CSR_BT_GATT_DB_ADD_REQ is called. 
                                  Note, If the application for some 
                                  reason cannot Create a complete 
                                  Service Definition and the Service 
                                  shall be supported over BR/EDR the 
                                  application shall set 'leOnly' to 
                                  TRUE. The application then needs 
                                  to generate and publish the SDP record 
                                  by using the CM - Connection Manager API.
    CsrBtGattDb **tail          - The tail pointer, a reference pointer
                                  which point to the last element
                                  in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(CsrBtGattDb        *head,
                                                                   CsrBtGattHandle    *attrHandle,
                                                                   CsrBtUuid16        serviceUuid16,
                                                                   CsrBool            leOnly,
                                                                   CsrBtGattDb        **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreatePrimaryServiceWith128BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.1 - Service Definition 

  Description: 
    Functions to Create a Primary Service Declaration Attribute to the local
    database with a 128-Bit uuid as defined defined in [Vol 3]. This function 
    returns the new head pointer to the caller. The created Primary Service 
    Declaration Attribute will have the following structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2800 -  |        <128-Bit UUID>      |  Read Only,         |
    |          | UUID for  |                            |  No Authentication, |
    |          | Primary   |                            |  No Authorisation   |
    |          | Service   |                            |                     |
    |    #1    |           |             #2             |                     |
    +----------+-----------+----------------------------+---------------------+

    Note a Service definition shall contain a Service Declaration and may 
    contain zero or more Include definitions and Characterictis definitions. 
    All Include Definitions shall immediately follow the Service Declaration 
    and can be create by using one of the functions:
    CsrBtGattUtilCreateIncludeDefinitionWithUuid
    CsrBtGattUtilCreateIncludeDefinitionWithoutUuid
    and all Characteristic Definitions shall immediately following the last 
    Include Definition. In case that no Include Definitions exists all 
    Characteristic Definitions must instead immediately follow the Service 
    Declaration. A Characteristic Definitions can be created by using one of 
    the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Whenever it is possible it is recommended that services definitions with
    service declarations using 16-Bit UUIDs is listed sequentially 
    and services definitons with service declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head           - The head pointer, a reference pointer
                                  which point to the first element
                                  in the linked list. 
    CsrBtGattHandle *attrHandle - The Attribute Handle [#1]. I.e.
                                  the index of where the Primary
                                  Service Declaration must
                                  be placed in the local database.
                                  Note the 'attrHandle' parameter 
                                  is automatic incremented by 1. 
    CsrBtUuid16 uuid128         - A 128-Bit Service UUID [#2].
    CsrBool leOnly              - If the Service Definition shall be 
                                  supported over BR/EDR 'leOnly' shall 
                                  be set to FALSE otherwise TRUE. If set 
                                  to FALSE GATT will automatic generate 
                                  and publish a generic SDP record. In 
                                  order for GATT to do this a complete 
                                  Service definition SHALL be created, 
                                  i.e. all Include Definitions, 
                                  Characteristic Definitions, and all 
                                  Characteristic Descriptor Definitions 
                                  SHALL be created before 
                                  CSR_BT_GATT_DB_ADD_REQ is called. 
                                  Note, If the application for some 
                                  reason cannot Create a complete 
                                  Service Definition and the Service 
                                  shall be supported over BR/EDR the 
                                  application shall set 'leOnly' to 
                                  TRUE. The application then needs 
                                  to generate and publish the SDP record 
                                  by using the CM - Connection Manager API.
    CsrBtGattDb **tail          - The tail pointer, a reference pointer
                                  which point to the last element
                                  in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreatePrimaryServiceWith128BitUuid(CsrBtGattDb     *head,
                                                                    CsrBtGattHandle *attrHandle,
                                                                    CsrBtUuid128    uuid128,
                                                                    CsrBool         leOnly,
                                                                    CsrBtGattDb     **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateSecondaryServiceWith16BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.1 - Service Definition 

  Description: 
    Functions to Create a Secondary Service Declaration Attribute to the local
    database with a 16-Bit uuid as defined defined in [Vol 3]. This function 
    returns the new head pointer to the caller. The created Secondary Service 
    Declaration Attribute will have the following structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2801 -  |        <16-Bit UUID>       |  Read Only,         |
    |          | UUID for  |                            |  No Authentication, |
    |          | Secondary |                            |  No Authorisation   |
    |          | Service   |                            |                     |
    |    #1    |           |             #2             |                     |
    +----------+-----------+----------------------------+---------------------+

    Note a Service definition shall contain a Service Declaration and may 
    contain zero or more Include definitions and Characterictis definitions. 
    All Include Definitions shall immediately follow the Service Declaration 
    and can be create by using one of the functions:
    CsrBtGattUtilCreateIncludeDefinitionWithUuid
    CsrBtGattUtilCreateIncludeDefinitionWithoutUuid
    and all Characteristic Definitions shall immediately following the last 
    Include Definition. In case that no Include Definitions exists all 
    Characteristic Definitions must instead immediately follow the Service 
    Declaration. A Characteristic Definitions can be created by using one of 
    the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Whenever it is possible it is recommended that services definitions with
    service declarations using 16-Bit UUIDs is listed sequentially 
    and services definitons with service declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head           - The head pointer, a reference pointer
                                  which point to the first element
                                  in the linked list. 
    CsrBtGattHandle *attrHandle - The Attribute Handle [#1]. I.e. 
                                  the index of where the Secondary
                                  Service Declaration must
                                  be placed in the local database.
                                  Note the 'attrHandle' parameter 
                                  is automatic incremented by 1. 
    CsrBtUuid16 serviceUuid16   - A 16-Bit Service UUID [#2].
    CsrBtGattDb **tail          - The tail pointer, a reference pointer
                                  which point to the last element
                                  in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateSecondaryServiceWith16BitUuid(CsrBtGattDb     *head,
                                                                     CsrBtGattHandle *attrHandle,
                                                                     CsrBtUuid16     serviceUuid16,
                                                                     CsrBtGattDb     **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateSecondaryServiceWith128BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.1 - Service Definition 

  Description: 
    Functions to Create a Secondary Service Declaration Attribute to the local
    database with a 128-Bit uuid as defined defined in [Vol 3]. This function 
    returns the new head pointer to the caller. The created Secondary Service 
    Declaration Attribute will have the following structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2801 -  |        <128-Bit UUID>      |  Read Only,         |
    |          | UUID for  |                            |  No Authentication, |
    |          | Secondary |                            |  No Authorisation   |
    |          | Service   |                            |                     |
    |    #1    |           |             #2             |                     |
    +----------+-----------+----------------------------+---------------------+

    Note a Service definition shall contain a Service Declaration and may 
    contain zero or more Include definitions and Characterictis definitions. 
    All Include Definitions shall immediately follow the Service Declaration 
    and can be create by using one of the functions:
    CsrBtGattUtilCreateIncludeDefinitionWithUuid
    CsrBtGattUtilCreateIncludeDefinitionWithoutUuid
    and all Characteristic Definitions shall immediately following the last 
    Include Definition. In case that no Include Definitions exists all 
    Characteristic Definitions must instead immediately follow the Service 
    Declaration. A Characteristic Definitions can be created by using one of 
    the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Whenever it is possible it is recommended that services definitions with
    service declarations using 16-Bit UUIDs is listed sequentially 
    and services definitons with service declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head           - The head pointer, a reference pointer
                                  which point to the first element
                                  in the linked list. 
    CsrBtGattHandle *attrHandle - The Attribute Handle [#1]. I.e. 
                                  the index of where the Secondary
                                  Service Declaration must
                                  be placed in the local database.
                                  Note the 'attrHandle' parameter 
                                  is automatic incremented by 1. 
    CsrBtUuid16 uuid128         - A 128-Bit Service UUID [#2].
    CsrBtGattDb **tail          - The tail pointer, a reference pointer
                                  which point to the last element
                                  in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateSecondaryServiceWith128BitUuid(CsrBtGattDb     *head,
                                                                      CsrBtGattHandle *attrHandle,
                                                                      CsrBtUuid128    uuid128,
                                                                      CsrBtGattDb     **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateIncludeDefinitionWithUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.2 - Include Defintion 

  Description: 
    Functions to Create a Include Declaration Attribute to the local
    database with a 16-bit service uuid as defined in [Vol 3]. This 
    function returns the new head pointer to the caller.
    The created Include Declaration Attribute will have the following 
    structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2802 -  |<Included|<End     |<16-Bit |  Read Only,         |
    |          | UUID for  |Service  | Group   | Service|  No Authentication, |
    |          | Include   |Attribute| Handle> | UUID>  |  No Authorisation   |
    |          |           |Handle>  |         |        |                     |
    |          |           |         |         |        |                     |
    |    #1    |           |   #2    |    #3   |   #4   |                     |
    +----------+-----------+----------------------------+---------------------+

    Note the Service UUID shall only be present when the UUID is a 16-Bit 
    UUID. If the Service UUID is not a 16-Bit UUID the function 
    CsrBtGattUtilCreateIncludeDefinitionWithUuid must be used instead. 
    
    Also note that All Include Declarations shall immediately follow the 
    Service Declaration which is created by using one of the functions:
    CsrBtGattUtilCreatePrimaryServiceWith16BitUuid
    CsrBtGattUtilCreatePrimaryServiceWith128BitUuid
    CsrBtGattUtilCreateSecondaryServiceWith16BitUuid
    CsrBtGattUtilCreateSecondaryServiceWith128BitUuid
    and that a server shall not contain a Service Definition with an 
    Include Definition to another service that reference the original 
    service.

  Parameters: 
    CsrBtGattDb *head                     - The head pointer, a reference pointer
                                            which point to the first element
                                            in the linked list. 
    CsrBtGattHandle *attrHandle           - The Attribute Handle [#1]. I.e. 
                                            the index of where the
                                            Include Declaration must
                                            be placed in the local database.
                                            Note the 'attrHandle' parameter 
                                            is automatic incremented by 1. 
    CsrBtGattHandle inclServiceAttrHandle - The attribute handle/index of 
                                            where the included service 
                                            starts [#2].
    CsrBtGattHandle endGroupHandle        - The attribute handle/index of 
                                            where the included service 
                                            stops [#3].
    CsrBtUuid16 serviceUuid               - The UUID of the included service [#4].
    CsrBtGattDb **tail                    - The tail pointer, a reference pointer
                                            which point to the last element
                                            in the linked list
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateIncludeDefinitionWithUuid(CsrBtGattDb     *head,
                                                                 CsrBtGattHandle *attrHandle,
                                                                 CsrBtGattHandle inclServiceAttrHandle,
                                                                 CsrBtGattHandle endGroupHandle,  
                                                                 CsrBtUuid16     serviceUuid,
                                                                 CsrBtGattDb     **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateIncludeDefinitionWithoutUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.2 - Include Defintion 

  Description: 
    Functions to Create a Include Declaration Attribute to the local
    database without any service uuid, i.e. the service uuid must be 
    128-bits, as defined in [Vol 3]. This function returns the new 
    head pointer to the caller. The created Include Declaration 
    Attribute will have the following structure:

    +----------+-----------+----------------------------+---------------------+
    | Attribute| Attribute |       Attribute Value      |Attribute Permissions|
    |  Handle  |   Type    |                            |                     |
    +----------+-----------+----------------------------+---------------------+
    | <0xNNNN> | 0x2802 -  |  <Included   |   <End      |  Read Only,         |
    |          | UUID for  |   Service    |    Group    |  No Authentication, |
    |          | Include   |   Attribute  |    Handle>  |  No Authorisation   |
    |          |           |   Handle>    |             |                     |
    |          |           |              |             |                     |    
    |    #1    |           |     #2       |      #3     |                     |
    +----------+-----------+----------------------------+---------------------+

    Note the Service UUID shall only be present when the UUID is a 16-Bit 
    UUID. If the Service UUID is a 16-Bit UUID the function 
    CsrBtGattUtilCreateIncludeDefinitionWithUuid must be used instead.

    Also note that All Include Declarations shall immediately follow the 
    Service Declaration which is created by using one of the functions:
    CsrBtGattUtilCreatePrimaryServiceWith16BitUuid
    CsrBtGattUtilCreatePrimaryServiceWith128BitUuid
    CsrBtGattUtilCreateSecondaryServiceWith16BitUuid
    CsrBtGattUtilCreateSecondaryServiceWith128BitUuid
    and that a server shall not contain a Service Definition with an 
    Include Definition to another service that reference the original 
    service.

  Parameters: 
    CsrBtGattDb *head                     - The head pointer, a reference pointer
                                            which point to the first element
                                            in the linked list. 
    CsrBtGattHandle *attrHandle           - The Attribute Handle [#1]. I.e. 
                                            the index of where the
                                            Include Declaration must
                                            be placed in the local database.
                                            Note the 'attrHandle' parameter 
                                            is automatic incremented by 1. 
    CsrBtGattHandle inclServiceAttrHandle - The attribute handle/index of 
                                            where the included service 
                                            starts [#2].
    CsrBtGattHandle endGroupHandle        - The attribute handle/index of 
                                            where the included service 
                                            stops [#3].
    CsrBtGattDb **tail                    - The tail pointer, a reference pointer
                                            which point to the last element
                                            in the linked list
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateIncludeDefinitionWithoutUuid(CsrBtGattDb     *head,
                                                                    CsrBtGattHandle *attrHandle,
                                                                    CsrBtGattHandle inclServiceAttrHandle,
                                                                    CsrBtGattHandle endGroupHandle,  
                                                                    CsrBtGattDb     **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.1 - Characteristic Declaration
               and Section 3.3.2 - Characteristic Value Declaration        

  Description: 
  
    Functions to Create the mandatory part of Characteristic Definition. I.e
    this function Creates a Characteristic Declaration and a Characteristic
    Value Declaration Attribute to the local database with a 16-Bit uuid, 
    as defined defined in [Vol 3]. This function returns the new head pointer 
    to the caller. 
    
    The created Characteristic Declaration Attribute will have the following 
    structure:

    +----------+----------+----------------------------------+-------------------+
    | Attribute| Attribute|       Attribute Value            |     Attribute     |
    |  Handle  |   Type   |                                  |    Permissions    |
    +----------+----------+----------------------------------+-------------------+
    | <0xNNNN> | 0x2803 - |<Characte-  |Characte- |<Characte-| Read Only,        |
    |          | UUID for | ristic     |ristic    | ristic   | No Authentication,|
    |          | Characte-| Properties>|Value     | UUID>    | No Authorisation  |
    |          | ristic   |            |Attribute |          |                   |
    |          |          |            |Handle    |          |                   |
    |          |          |            |0xNNNN + 1|          |                   |
    |    #1    |          |      #2    |          |   #3     |                   |
    +----------+----------+------------+----------+----------+-------------------+
 
    and the created Characteristic Characteristic Value Declaration Attribute
    will have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    |0xNNNN + 1| <0xUUUU - A     |  <Characteristic Value> |  <Higher layer      |
    |          | 16-Bit UUID for |                         |   profile or        |
    |          | Characteristic  |                         |   implementation    |
    |          | UUID>           |                         |   specific          |
    |          |                 |                         |                     |
    |          |       #3        |           #4            |          #5         |
    +----------+-----------------+-------------------------+---------------------+

    Note a Service (Primary or Secondary) may have multiple characteristic 
    definitions with the same Charactetistic UUID, and whenever it is possible
    it is recommended that characteristic declarations using 16-Bit UUIDs is 
    listed sequentially and characteristic declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head                      - The head pointer, a reference 
                                             pointer which point to the 
                                             first element in the linked 
                                             list. 
    CsrBtGattHandle *attrHandle            - The Attribute Handle [#1]. 
                                             I.e. the index of where the 
                                             Characteristic Declaration
                                             and the Characteristic 
                                             Value Declaration must be 
                                             placed in the local database.
                                             The Characteristic Value
                                             Declaration will be place right
                                             after (index + 1) the 
                                             Characteristic Declaration. 
                                             
                                             Note the 'attrHandle' parameter 
                                             is automatic incremented by 2.
    CsrBtGattPropertiesBits properties     - The Characteristic Properties bits
                                             which determines how the Characteristic
                                             Value can be used [#2].

                                             The Properties bits are defined 
                                             in csr_bt_gatt_prim.h and can 
                                             be set to:
                                             CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST
                                             CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                             CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_WRITE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY
                                             CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES
                                             CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES
                                             
                                             If CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST is set
                                             a Server Characteric Configuration declaration
                                             shall be created within this Characteristic 
                                             definition by calling
                                             CsrBtGattUtilCreateServerCharacConfiguration.

                                             If CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY or 
                                             CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE is set
                                             a Client Characteric Configuration declaration
                                             shall be created within this Characteristic 
                                             definition by calling 
                                             CsrBtGattUtilCreateClientCharacConfiguration.

                                             If CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES 
                                             is set a Characteristic Extended Properties Descriptor
                                             shall be created within this Characteristic 
                                             definition by calling
                                             CsrBtGattUtilCreateCharacExtProperties.
                                             
                                             Note multiple Properties bits can be set.   

    CsrBtUuid16 uuid16                     - A 16-Bit UUID for the Characterictic
                                             Value [#3].   
    CsrBtGattAttrFlags attrValueFlags      - The attribute flag field defines 
                                             how the Characteristic Value 
                                             Declaration can be access [#5].
                                             The attribute flags are defined 
                                             in csr_bt_gatt_prim.h and can 
                                             be set to:
                                             CSR_BT_GATT_ATTR_FLAGS_NONE
                                             CSR_BT_GATT_ATTR_FLAGS_DYNLEN
                                             CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                             CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                             CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                                             CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                             CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                             CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                             CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                                             CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION   
                                             CSR_BT_GATT_ATTR_FLAGS_DISABLE_LE
                                             CSR_BT_GATT_ATTR_FLAGS_DISABLE_BREDR
                                               
                                             Note multiple attribute flags can be set.

    CsrUint16 attrValueLength              - The length of the Attribute Value 
                                             in octets.
    const CsrUint8 *attrValue              - The Value of the characteristic[#4].
                                             Note this value shall be const data, 
                                             i.e. not an allocated pointer.
    CsrBtGattDb **tail                     - The tail pointer, a reference 
                                             pointer which point to the 
                                             last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(CsrBtGattDb             *head,
                                                                     CsrBtGattHandle         *attrHandle,
                                                                     CsrBtGattPropertiesBits properties,
                                                                     CsrBtUuid16             uuid16,
                                                                     CsrBtGattAttrFlags      attrValueFlags,
                                                                     CsrUint16               attrValueLength,
                                                                     const CsrUint8          *attrValue,
                                                                     CsrBtGattDb             **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.1 - Characteristic Declaration
               and Section 3.3.2 - Characteristic Value Declaration        

  Description: 
  
    Functions to Create the mandatory part of Characteristic Definition. I.e
    this function Creates a Characteristic Declaration and a Characteristic
    Value Declaration Attribute to the local database with a 128-Bit uuid 
    as defined defined in [Vol 3]. This function returns the new head pointer 
    to the caller. 
    
    The created Characteristic Declaration Attribute will have the following 
    structure:

    +----------+----------+----------------------------------+-------------------+
    | Attribute| Attribute|       Attribute Value            |     Attribute     |
    |  Handle  |   Type   |                                  |    Permissions    |
    +----------+----------+----------------------------------+-------------------+
    | <0xNNNN> | 0x2803 - |<Characte-  |Characte- |<Characte-| Read Only,        |
    |          | UUID for | ristic     |ristic    | ristic   | No Authentication,|
    |          | Characte-| Properties>|Value     | UUID>    | No Authorisation  |
    |          | ristic   |            |Attribute |          |                   |
    |          |          |            |Handle    |          |                   |
    |          |          |            |0xNNNN + 1|          |                   |
    |    #1    |          |      #2    |          |   #3     |                   |
    +----------+----------+------------+----------+----------+-------------------+
 
    and the created Characteristic Characteristic Value Declaration Attribute
    will have the following structure:

    +----------+-------------------+-----------------------+---------------------+
    | Attribute|   Attribute       |    Attribute Value    |Attribute Permissions|
    |  Handle  |     Type          |                       |                     |
    +----------+-------------------+-----------------------+---------------------+
    |0xNNNN + 1|<0xUUUUUUUUUUUUUUUU| <Characteristic Value>|  <Higher layer      |
    |          |A 128-Bit UUID for |                       |   profile or        |
    |          |Characteristic     |                       |   implementation    |
    |          |UUID>              |                       |   specific          |
    |          |                   |                       |                     |
    |          |       #3          |           #4          |          #5         |
    +----------+-------------------+-----------------------+---------------------+

    Note a Service (Primary or Secondary) may have multiple characteristic 
    definitions with the same Charactetistic UUID, and whenever it is possible
    it is recommended that characteristic declarations using 16-Bit UUIDs is 
    listed sequentially and characteristic declarations using 128-Bit UUIDs is 
    listed sequentially.

  Parameters: 
    CsrBtGattDb *head                      - The head pointer, a reference 
                                             pointer which point to the 
                                             first element in the linked 
                                             list. 
    CsrBtGattHandle *attrHandle            - The Attribute Handle [#1]. 
                                             I.e. the index of where the 
                                             Characteristic Declaration
                                             and the Characteristic 
                                             Value Declaration must be 
                                             placed in the local database.
                                             The Characteristic Value
                                             Declaration will be place right
                                             after (index + 1) the 
                                             Characteristic Declaration. 
                                             
                                             Note the 'attrHandle' parameter 
                                             is automatic incremented by 2.
    CsrBtGattPropertiesBits properties     - The Characteristic Properties bits
                                             which determines how the Characteristic
                                             Value can be used [#2].

                                             The Properties bits are defined 
                                             in csr_bt_gatt_prim.h and can 
                                             be set to:
                                             CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST
                                             CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                             CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_WRITE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY
                                             CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE
                                             CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES
                                             CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES
                                             
                                             If CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST is set
                                             a Server Characteric Configuration declaration
                                             shall be created within this Characteristic 
                                             definition by calling
                                             CsrBtGattUtilCreateServerCharacConfiguration.

                                             If CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY or 
                                             CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE is set
                                             a Client Characteric Configuration declaration
                                             shall be created within this Characteristic 
                                             definition by calling 
                                             CsrBtGattUtilCreateClientCharacConfiguration.

                                             If CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES 
                                             is set a Characteristic Extended Properties Descriptor
                                             shall be created within this Characteristic 
                                             definition by calling
                                             CsrBtGattUtilCreateCharacExtProperties.
                                             
                                             Note multiple Properties bits can be set.   

    CsrBtUuid128 uuid128                   - A 128-Bit UUID for the Characterictic
                                             Value [#3].   
    CsrBtGattAttrFlags attrValueFlags      - The attribute flag field defines 
                                             how the Characteristic Value 
                                             Declaration can be access [#5].
                                             The attribute flags are defined 
                                             in csr_bt_gatt_prim.h and can 
                                             be set to:
                                             CSR_BT_GATT_ATTR_FLAGS_NONE
                                             CSR_BT_GATT_ATTR_FLAGS_DYNLEN
                                             CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                             CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                             CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                                             CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                             CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                             CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                             CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                                             CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION   
                                             CSR_BT_GATT_ATTR_FLAGS_DISABLE_LE
                                             CSR_BT_GATT_ATTR_FLAGS_DISABLE_BREDR
                                               
                                             Note multiple attribute flags can be set.

    CsrUint16 attrValueLength              - The length of the Attribute Value 
                                             in octets.
    const CsrUint8 *attrValue              - The Value of the characteristic[#4].
                                             Note this value shall be const data, 
                                             i.e. not an allocated pointer.
    CsrBtGattDb **tail                     - The tail pointer, a reference 
                                             pointer which point to the 
                                             last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacDefinitionWith128BitUuid(CsrBtGattDb             *head,
                                                                      CsrBtGattHandle         *attrHandle,
                                                                      CsrBtGattPropertiesBits properties,
                                                                      CsrBtUuid128            uuid128,
                                                                      CsrBtGattAttrFlags      attrValueFlags,
                                                                      CsrUint16               attrValueLength,
                                                                      const CsrUint8          *attrValue,
                                                                      CsrBtGattDb             **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacExtProperties

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.1 - Characteristic Extended Properties 

  Description: 
    Functions to Create a Characteristic Extended Properties declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Characteristic Extended Properties Declaration Attribute will 
    have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    | <0xNNNN> | 0x2900 -        |<Characteristic Extended |  Read Only,         |
    |          | UUID for        | Properties Bit Field>   |  No Authentication, |
    |          | Characteristic  |                         |  No Authorisation   |
    |          | Extended        |                         |                     |
    |    #1    | Properties      |             #2          |                     |
    +----------+-----------------+-------------------------+---------------------+
 
    Note, only one Characteristic Extended Properties Descriptor shall exist 
    within a Characteristic definition, and it may occur in any position 
    after the Characteristic Value which is created by using one of the 
    functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Note if the <CsrBtGattPropertiesBits properties> parameter in one of 
    above functions is set to CSR_BT_GATT_CHARAC_PROPERTIES_EXTENDED_PROPERTIES 
    the Characteristic Extended Properties declaration shall be created.

  Parameters: 
    CsrBtGattDb *head                        - The head pointer, a reference 
                                               pointer which point to the 
                                               first element in the linked 
                                               list. 
    CsrBtGattHandle *attrHandle              - The Attribute Handle [#1]. 
                                               I.e. the index of where the 
                                               Characteristic Extended 
                                               Properties Declaration must
                                               be placed in the local database.
                                               Note the 'attrHandle' parameter 
                                               is automatic incremented by 1. 
    CsrBtGattExtPropertiesBits extProperties - This bit field describes additional
                                               properties on how the Characteristic
                                               Value can be used, or how the 
                                               Characteristic User Description 
                                               Declaration can be access.
                                               The following bits 
                                               are defined in csr_bt_gatt_prim.h
                                               CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE
                                               CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX
                                               
                                               If CSR_BT_GATT_CHARAC_EXT_PROPERTIES_RELIABLE_WRITE
                                               is set the server permits reliable writes of the 
                                               Characteristic Value.
                                               
                                               If CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX
                                               is set the server permits writes to the 
                                               Characteristic User Description Declaration, 
                                               which then shall be created by using 
                                               CsrBtGattUtilCreateCharacUserDescription
                                               
                                               Note multiple Characteristic
                                               Properties can be set [#2].
    CsrBtGattDb **tail                       - The tail pointer, a reference 
                                               pointer which point to the 
                                               last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacExtProperties(CsrBtGattDb                 *head,
                                                           CsrBtGattHandle             *attrHandle,
                                                           CsrBtGattExtPropertiesBits  extProperties,
                                                           CsrBtGattDb                 **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacUserDescription

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.2 - Characteristic User Description 

  Description: 
    Functions to Create a Characteristic User Description declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Characteristic User Description Declaration Attribute will 
    have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    | <0xNNNN> | 0x2901 -        |<Characteristic User     |  Application or     |
    |          | UUID for        | Description UTF-8       |  Profile Specific   |
    |          | Characteristic  | String>                 |                     |
    |          | User            |                         |                     |
    |    #1    | Description     |           #2            |         #3          |
    +----------+-----------------+-------------------------+---------------------+
 
    Note, only one Characteristic User Description Descriptor shall exist 
    within a Characteristic definition, and it may occur in any position 
    after the Characteristic Value which is created by using one of the 
    functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Note if a Characteristic Extended Properties Descriptor is created 
    by calling CsrBtGattUtilCreateCharacExtProperties and the 
    <CsrBtGattExtPropertiesBits extProperties> parameter were set
    to CSR_BT_GATT_CHARAC_EXT_PROPERTIES_WRITE_AUX the Characteristic 
    User Description declaration shall be created and the <CsrBtGattPermFlags 
    attrPermission> parameter shall be set the CSR_BT_GATT_PERM_FLAGS_WRITE flag.

  Parameters: 
    CsrBtGattDb *head                    - The head pointer, a reference 
                                           pointer which point to the 
                                           first element in the linked 
                                           list. 
    CsrBtGattHandle *attrHandle          - The Attribute Handle [#1]. 
                                           I.e. the index of where the 
                                           Characteristic User Descriptor 
                                           Declaration must be placed in 
                                           the local database. Note the 
                                           'attrHandle' parameter 
                                           is automatic incremented by 1. 
    const CsrUtf8String *userDescription - A NULL terminated UTF8 string 
                                           describing the the Characteristic 
                                           Value. Note this value shall be 
                                           const data, i.e. not an 
                                           allocated pointer.[#2].
    CsrBtGattPermFlags attrPermission    - The permission flag field is 
                                           use to determined whether read or 
                                           write access is permitted or not.
                                           The permission flags are defined 
                                           in csr_bt_gatt_prim.h and can be 
                                           set to: 
                                           CSR_BT_GATT_PERM_FLAGS_READ
                                           CSR_BT_GATT_PERM_FLAGS_WRITE
                                               
                                           Please note security is set in
                                           attrFlags parameter and multiple 
                                           permission flags can be set [#3].
    CsrBtGattAttrFlags attrFlags         - The attribute flag field defines 
                                           how User Description Declaration 
                                           can be access 
                                           The attribute flags are defined 
                                           in csr_bt_gatt_prim.h and can 
                                           be set to:
                                           CSR_BT_GATT_ATTR_FLAGS_NONE
                                           CSR_BT_GATT_ATTR_FLAGS_DYNLEN
                                           CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                           CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                           CSR_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                                           CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                           CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                           CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                           CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION
                                           CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE.
                                               
                                           Note multiple attribute flags can be set.

    CsrBtGattDb **tail                   - The tail pointer, a reference 
                                           pointer which point to the 
                                           last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacUserDescription(CsrBtGattDb                 *head,
                                                             CsrBtGattHandle             *attrHandle,
                                                             const CsrUtf8String         *userDescription,
                                                             CsrBtGattPermFlags          attrPermission,
                                                             CsrBtGattAttrFlags          attrFlags,
                                                             CsrBtGattDb                 **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateClientCharacConfiguration

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.3 - Client Characteristic Configuration 

  Description: 
    Functions to Create a Client Characteristic Configuration Declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Client Characteristic Configuration Declaration Attribute will 
    have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    | <0xNNNN> | 0x2902 -        | None - as it is own     |  Readable with no   |
    |          | UUID for        | by the application      |  authentication or  |
    |          | Client          |                         |  authorisation      |
    |          | Characteristic  |                         |                     |
    |          | Configuration   |                         |  Writable with      |
    |          |                 |                         |  authentication     |
    |          |                 |                         |  and authorisation  |
    |          |                 |                         |  defined by a       |
    |          |                 |                         |  higher layer       |
    |          |                 |                         |  specification or   |
    |          |                 |                         |  is implementation  |
    |          |                 |                         |  specific           |
    |          |                 |                         |                     |
    |    #1    |                 |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
 
    The Client Characteristic Configuration Declaration is an optional 
    Characteristic descriptor that defines how the characteristic may be 
    configured by a specific client. Only one Client Characteristic Configuration 
    Declaration may exist within a Characteristic definition, and it may 
    occur in any position after the Characteristic Value which is created 
    by using one of the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Multiple clients can connect to this server, and as only a single instance of 
    the Client Characteristic Configuration can exist, the application has to own 
    and handle the value in order to provide an individual Client Characteristic 
    Configuration Descriptor Value for each connected client. I.e. reads of 
    this Value shall only show the Client Characteristic Configuration 
    Descriptor Value for that client and write only affect the configuration 
    of that client. The Client Characteristic Configuration Descriptor Value 
    shall also be persistent across connections for bonded devices. For non-bonded 
    devices the Client Characteristic Configuration Descriptor Value
    shall be responded with CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT, i.e. the
    Client Characteristic Configuration Declaration in not in used. The 
    following Client Characteristic Configuration Descriptor Values are valid
    and are defined in csr_bt_gatt_prim.h.

    CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT
    CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION
    CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION

    In order for the Server to accept that the Client sets the descriptor Value 
    to CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION the Server shall set the
    <CsrBtGattPropertiesBits properties> parameter to 
    CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY when creating the Characteristic definition.
    Similar, in order for the Server to accept that the Client sets the descriptor 
    Value to CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION the Server shall set the
    <CsrBtGattPropertiesBits properties> parameter to 
    CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE when creating the Characteristic definition.


    As the Client Characteristic Configuration descriptor Value
    can have many different Values and it shall be persistent across connections,
    this Value is always own by the application, e.g. if a Client tries to read
    the Client Characteristic Configuration Declaration the application will 
    receive a CSR_BT_GATT_DB_ACCESS_READ_IND message which it shall respond to 
    by calling CsrBtGattDbReadAccessResSend.
    Similar, if a Client tries to write to the Client Characteristic Configuration 
    Declaration the application will receive a CSR_BT_GATT_DB_ACCESS_WRITE_IND 
    message which it shall respond to by calling CsrBtGattDbWriteAccessResSend.

  Parameters: 
    CsrBtGattDb *head                         - The head pointer, a reference 
                                                pointer which point to the 
                                                first element in the linked 
                                                list. 
    CsrBtGattHandle *attrHandle               - The Attribute Handle [#1]. 
                                                I.e. the index of where the 
                                                Client Characteristic Configuration 
                                                Declaration must be placed in 
                                                the local database. Note the 
                                                'attrHandle' parameter 
                                                is automatic incremented by 1. 
    CsrBtGattAttrFlags attrFlags              - The attribute flag field defines 
                                                how the Client Characteristic 
                                                Configuration Declaration can 
                                                be access. The attribute flags 
                                                are defined in csr_bt_gatt_prim.h 
                                                and can be set to:
                                                CSR_BT_GATT_ATTR_FLAGS_NONE
                                                CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                                CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                                CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION
                                                CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE.
                                               
                                                Note multiple attribute flags can be set.
    CsrBtGattDb **tail                        - The tail pointer, a reference 
                                                pointer which point to the 
                                                last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateClientCharacConfiguration(CsrBtGattDb        *head,
                                                                 CsrBtGattHandle    *attrHandle,
                                                                 CsrBtGattAttrFlags attrFlags,
                                                                 CsrBtGattDb        **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateServerCharacConfiguration

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.4 - Server Characteristic Configuration 

  Description: 
    Functions to Create a Server Characteristic Configuration Declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Server Characteristic Configuration Declaration Attribute will 
    have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    | <0xNNNN> | 0x2903 -        |<Characteristic Server   |  Readable with no   |
    |          | UUID for        | Configuration Bits>     |  authentication or  |
    |          | Server          |                         |  authorisation      |
    |          | Characteristic  |                         |                     |
    |          | Configuration   |                         |  Writable with      |
    |          |                 |                         |  authentication     |
    |          |                 |                         |  and authorisation  |
    |          |                 |                         |  defined by a       |
    |          |                 |                         |  higher layer       |
    |          |                 |                         |  specification or   |
    |          |                 |                         |  is implementation  |
    |          |                 |                         |  specific           |
    |          |                 |                         |                     |
    |    #1    |                 |           #2            |         #3          |
    +----------+-----------------+-------------------------+---------------------+
 
    The Server Characteristic Configuration Declaration is an optional 
    Characteristic descriptor that defines how the characteristic may be 
    configured for the server. Only one Server Characteristic Configuration 
    Declaration may exist within a Characteristic definition, and it may 
    occur in any position after the Characteristic Value which is created 
    by using one of the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    Multiple clients can connect to the server, and as only a single instance 
    of the Server Characteristic Configuration can exist and all clients shall 
    share this value reads of the Server Characteristic 
    Configuration shows the configuration for all clients and writes
    affect the configuration for all clients.
    
    As the application is responsible of executing/controlling 
    the broadcast procedure the application will receive a 
    CSR_BT_GATT_DB_ACCESS_WRITE_IND message every time a Client tries to 
    write to the Server Characteristic Configuration Declaration. The 
    application shall respond the CSR_BT_GATT_DB_ACCESS_WRITE_IND message 
    by calling CsrBtGattDbWriteAccessResSend

    In order for the Server to accept that the Client sets the descriptor Value 
    to CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST the Server shall set the
    <CsrBtGattPropertiesBits properties> parameter to 
    CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST when creating the Characteristic 
    definition.

  Parameters: 
    CsrBtGattDb *head                         - The head pointer, a reference 
                                                pointer which point to the 
                                                first element in the linked 
                                                list. 
    CsrBtGattHandle *attrHandle               - The Attribute Handle [#1]. 
                                                I.e. the index of where the 
                                                Server Characteristic Configuration 
                                                Declaration must be placed in 
                                                the local database. Note the 
                                                'attrHandle' parameter 
                                                is automatic incremented by 1. 
    CsrBtGattAttrFlags attrFlags              - The attribute flag field defines 
                                                how the Server Characteristic 
                                                Configuration Declaration can 
                                                be access. The attribute flags 
                                                are defined in csr_bt_gatt_prim.h 
                                                and can be set to:
                                                CSR_BT_GATT_ATTR_FLAGS_NONE
                                                CSR_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                                                CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION
                                                CSR_BT_GATT_ATTR_FLAGS_AUTHORISATION
                                                CSR_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE.
                                               
                                                Note multiple attribute flags can be set.

     CsrBtGattSrvConfigBits configurationBits - The following Server Characteristic
                                                Configuration bits are defined in 
                                                csr_bt_gatt_prim.h and can be set to [#2]:
                                                CSR_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE
                                                CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST
                                                
                                                Setting 
                                                CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST
                                                is a method for a Client to indicate to the 
                                                Server that the Characteristic Value shall
                                                be broadcast in the advertising data when
                                                the Server is executing the 
                                                CSR_BT_GATT_ADVERTISE_REQ procedure and if 
                                                advertising data resources are available. 
                                                Note advertising only works
                                                for LE physical links.
    CsrBtGattDb **tail                        - The tail pointer, a reference 
                                                pointer which point to the 
                                                last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateServerCharacConfiguration(CsrBtGattDb            *head,
                                                                 CsrBtGattHandle        *attrHandle,
                                                                 CsrBtGattAttrFlags     attrFlags,
                                                                 CsrBtGattSrvConfigBits configurationBits,
                                                                 CsrBtGattDb            **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacPresentationFormat

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.5 - Characteristic Presentation Format 

  Description: 
    Functions to Create a Characteristic Presentation Format declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Characteristic Presentation Format Declaration Attribute will 
    have the following structure:

    +---------+--------------+------------------------------------------------+--------------------+
    |Attribute|   Attribute  |               Attribute Value                  |Attribute           |
    | Handle  |     Type     |                                                |Permissions         |
    +---------+--------------+------------------------------------------------+--------------------+
    |<0xNNNN> |0x2904 -      |<Format>|<Exponent>|<Unit>|<Name  |<Description>|Read Only           |
    |         |UUID for      |        |          |      | Space>|             |No Authentication,  |
    |         |Characteristic|        |          |      |       |             |No Authorisation    |
    |         |Format        |        |          |      |       |             |                    |
    |   #1    |              |   #2   |    #3    |  #4  |  #5   |     #6      |                    |
    +---------+--------------+--------+----------+------+-------+-------------+--------------------+
 
    The Characteristic Presentation Format Declaration is an optional 
    Characteristic descriptor that defines the format of the Characteristic 
    Value which is created by using one of the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    The Characteristic Presentation Format Declaration may occur in any 
    position within the characteristic definition after the Characteristic 
    Value. The application is allow to create multiple Characteristic 
    Presentation Format Declaration within the characteristic definition if 
    it also created the Characteristic Aggregate Format declaration by using 
    the function CsrBtGattUtilCreateCharacAggregateFormat.
      

  Parameters: 
    CsrBtGattDb *head           - The head pointer, a reference 
                                  pointer which point to the 
                                  first element in the linked 
                                  list. 
    CsrBtGattHandle *attrHandle - The Attribute Handle [#1]. 
                                  I.e. the index of where the 
                                  Characteristic Presentation 
                                  Format Declaration must
                                  be placed in the local database.
                                  Note the 'attrHandle' parameter 
                                  is automatic incremented by 1. 
    CsrBtGattFormats format     - Defines the format of the Value of 
                                  this characteristic. The formats 
                                  are defined in csr_bt_gatt_prim.h
                                  and can be set to [#2]:
                                  CSR_BT_GATT_CHARAC_FORMAT_RFU
                                  CSR_BT_GATT_CHARAC_FORMAT_BOOLEAN
                                  CSR_BT_GATT_CHARAC_FORMAT_2BIT
                                  CSR_BT_GATT_CHARAC_FORMAT_NIBBLE
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT8  (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT12 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT16 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT24 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT32 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT48 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT64 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_UINT128(*) 
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT8  (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT12 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT16 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT24 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT32 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT48 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT64 (*)
                                  CSR_BT_GATT_CHARAC_FORMAT_SINT128(*) 
                                  CSR_BT_GATT_CHARAC_FORMAT_FLOAT32
                                  CSR_BT_GATT_CHARAC_FORMAT_FLOAT64
                                  CSR_BT_GATT_CHARAC_FORMAT_SFLOAT
                                  CSR_BT_GATT_CHARAC_FORMAT_FLOAT
                                  CSR_BT_GATT_CHARAC_FORMAT_DUINT16
                                  CSR_BT_GATT_CHARAC_FORMAT_UTF8S
                                  CSR_BT_GATT_CHARAC_FORMAT_UTF16S
                                  CSR_BT_GATT_CHARAC_FORMAT_STRUCT
    CsrInt8 exponent            - The exponent field is only used with 
                                  integer format types (mark with * above). 
                                  The exponent field is a signed integer 
                                  and is use determines how the value of 
                                  this characteristic is further formatted 
                                  after the formular [#3]: 
                                  actual value = Characteristic Value * 10Exponent
                                  I.e. if the Exponent is 2 and the 
                                  Characteristic Value is 23, the 
                                  actual value would be 2300
    CsrUint16 unit              - The unit is a UUID as defined in 
                                  the Assigned Numbers Specification [#4].
    CsrUint8 nameSpace          - The nameSpace of the description as defined 
                                  in the Assigned Numbers Specification [#5].  
    CsrUint16 description       - The Description is an enumerated value as 
                                  defined in the Assigned Numbers document [#6].
    CsrBtGattDb **tail          - The tail pointer, a reference 
                                  pointer which point to the 
                                  last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacPresentationFormat(CsrBtGattDb      *head,
                                                                CsrBtGattHandle  *attrHandle,
                                                                CsrBtGattFormats format,
                                                                CsrInt8          exponent,
                                                                CsrUint16        unit, 
                                                                CsrUint8         nameSpace,
                                                                CsrUint16        description, 
                                                                CsrBtGattDb      **tail);

/* ----------------------------------------------------------------------------
  Name: 
    CsrBtGattUtilCreateCharacAggregateFormat

  Document Reference: 
    [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3
               Part G, Section 3.3.3.6 - Characteristic Aggregate Format

  Description: 
    Functions to Create a Characteristic Aggregate Format declaration Attribute 
    to the local database. This function returns the new head pointer to the caller. 
    The created Characteristic Aggregate Format Declaration Attribute will 
    have the following structure:

    +----------+-----------------+-------------------------+---------------------+
    | Attribute|    Attribute    |     Attribute Value     |Attribute Permissions|
    |  Handle  |      Type       |                         |                     |
    +----------+-----------------+-------------------------+---------------------+
    | <0xNNNN> | 0x2905 -        |<List of Attribute       |  Read Only          |
    |          | UUID for        | Handles for the         |  No Authentication, |
    |          | Characteristic  | Characteristic Presenta |  No Authorisation   |
    |          | Aggregate       | Presentation Format     |                     |
    |          | Format          | Declaration>            |                     |
    |          |                 |                         |                     |
    |    #1    |                 |           #2            |                     |
    +----------+-----------------+-------------------------+---------------------+
 
    The Characteristic Aggregate Format Declaration is an optional 
    Characteristic descriptor that defines the format of an aggregated 
    Characteristic Value which is created by using one of the functions:
    CsrBtGattUtilCreateCharacDefinitionWith16BitUuid
    CsrBtGattUtilCreateCharacDefinitionWith128BitUuid

    The Characteristic Aggregate Format Declaration may occur in any 
    position within the characteristic definition after the 
    Characteristic Value. Only one Characteristic Aggregate Format 
    declaration shall exist in a characteristic definition.
      
    If more than one Characteristic Presentation Format declarations 
    is created, within the characteristic definition, by using the function:
    CsrBtGattUtilCreateCharacPresentationFormat
    there shall also be created one Characteristic Aggregate Format 
    declaration

    Also note that the Characteristic Aggregate Format declaration 
    may be created without a Characteristic Presentation Format declaration 
    existing within the characteristic definition, as this declaration may 
    use Characteristic Presentation Format declarations from other 
    characteristic definitions.    

  Parameters: 
    CsrBtGattDb *head              - The head pointer, a reference 
                                     pointer which point to the 
                                     first element in the linked 
                                     list. 
    CsrBtGattHandle *attrHandle    - The Attribute Handle [#1]. 
                                     I.e. the index of where the 
                                     Characteristic Aggregate Format
                                     Declaration must be placed in 
                                     the local database. Note the 
                                     'attrHandle' parameter 
                                     is automatic incremented by 1. 
    CsrUint16 handlesCount.        - The number of Attribute Handles
                                     in the list.
    const CsrBtGattHandle *handles - A list of Attribute Handles of 
                                     Characteristic Presentation Format 
                                     declarations, where each Attribute 
                                     Handle points to a Characteristic 
                                     Presentation Format declaration [#2].
                                     Note this value shall be const data, 
                                     i.e. not an allocated pointer.
    CsrBtGattDb **tail             - The tail pointer, a reference 
                                     pointer which point to the 
                                     last element in the linked list.
   ---------------------------------------------------------------------------- */
extern CsrBtGattDb *CsrBtGattUtilCreateCharacAggregateFormat(CsrBtGattDb            *head,
                                                             CsrBtGattHandle        *attrHandle,
                                                             CsrUint16              handlesCount,
                                                             const CsrBtGattHandle  *handles,
                                                             CsrBtGattDb            **tail);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_GATT_UTILS_H__ */

