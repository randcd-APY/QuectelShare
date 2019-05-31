/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_gatt_demo_rsc_server_db.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_profiles.h"
#include "csr_bt_gatt_utils.h"
#include "csr_pmem.h"

CsrBtGattDb * CsrBtRscsDbCreate(CsrUint16 handleOffset)
{

    CsrBtGattDb                *head           = NULL;
    CsrBtGattDb                *tail           = NULL;
    CsrBtGattHandle            attrHandle;
    CsrBtGattPropertiesBits    properties;        
    CsrBtGattAttrFlags         attrValueFlags;
    CsrBtUuid16                uuid16;
    CsrUint16                  valuesize = 0;
    CsrUint8                  *value;

    /********* Service 1 *********
     *
     * Structure:
     * Handle   |   UUID    |   Value                                                            | Permission
     * ------------------------------------------------------------------------------------------------------
     * 0x0001   |   0x2800  |   Primary Service, UUID=0x1800 (Generic Attribute Profile)         | Read
     * 0x0002   |   0x2803  |   Charac Descriptor, R, Handle:0x0003, UUID:0xxxxx                 | Read
     * 0x0003   |   0xXXXX  |   xxxxx                                                            | Read
     *
     *  and more

     Already added by GATT*/

    /********* Service 2 *********
     *
     * Structure:
     * Handle   |   UUID    |   Value                                                            | Permission
     * ------------------------------------------------------------------------------------------------------
     * 0x0008   |   0x2800  |   Primary Service, UUID=0x1801 (Attribute Profile)                 | Read
     * 0x0009   |   0x2803  |   Charac Descriptor, R, Handle:0x000a, UUID:0xxxxx                 | Read
     *
     * More

     Already added by GATT*/

    /********* Service 3 *********
     *
     * Structure:
     * Handle   |   UUID    |   Value                                                            | Permission
     * ------------------------------------------------------------------------------------------------------
     * 0x0010   |   0x2800  |   Primary Service, UUID=0x1814 (RSC)                               | Read
     * 0x0011   |   0x2803  |   Charac Descriptor, R, Handle:0x0012, UUID:0x2A53                 | Read
     * 0x0012   |   0x2A53  |   0x...                                                            | Notify
     * 0x0013   |   0x2902  |   Client Charac Configuration                                      | Read | Write
     * 0x0014   |   0x2803  |   Charac Descriptor, W, Handle:0x001a, UUID:0x2A54                 | Read
     * 0x0015   |   0x2A54  |   0x...                                                            | Read
     * 0x0016   |   0x2803  |   Charac Descriptor, W, Handle:0x001a, UUID:0x2A5D                 | Read
     * 0x0017   |   0x2A5D  |   0x...                                                            | Read
     * 0x0018   |   0x2803  |   Charac Descriptor, W, Handle:0x001a, UUID:0x2A55                 | Read
     * 0x0019   |   0x2A55  |   0x...                                                            | Write | Indicate
     * 0x001A   |   0x2902  |   Client Charac Configuration                                      | Read | Write
     */
    properties           =  CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY;
    attrValueFlags       =  CSR_BT_GATT_ATTR_FLAGS_NONE;
    uuid16               =  CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC;
    valuesize            =  sizeof(CsrUint8)+sizeof(CsrUint16)+sizeof(CsrUint8)/*Optional part: +sizeof(CsrUint16)+sizeof(CsrUint24)*/;
    value                =  CsrPmemZalloc(valuesize);
/*TODO create default value*/
    attrHandle           =  CSR_BT_GATT_APP_RSC_SERVICE_HANDLE + handleOffset;

    head = CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(head,
                                                          &attrHandle,
                                                          CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE,
                                                          FALSE,
                                                          &tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            &attrHandle,
                                                            properties,
                                                            uuid16,
                                                            attrValueFlags,
                                                            valuesize,
                                                            value,
                                                            &tail);
    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        &attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        &tail);

    CsrPmemFree(value);
    properties           =  CSR_BT_GATT_CHARAC_PROPERTIES_READ;
    attrValueFlags       =  CSR_BT_GATT_ATTR_FLAGS_IRQ_READ;
    uuid16               =  CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC;
    valuesize            =  sizeof(CsrUint16);
    value                =  CsrPmemZalloc(valuesize);
/*TODO create default value*/
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            &attrHandle,
                                                            properties,
                                                            uuid16,
                                                            attrValueFlags,
                                                            valuesize,
                                                            value,
                                                            &tail);

    CsrPmemFree(value);
    properties           =  CSR_BT_GATT_CHARAC_PROPERTIES_READ;
    attrValueFlags       =  CSR_BT_GATT_ATTR_FLAGS_IRQ_READ;
    uuid16               =  CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC;
    valuesize            =  sizeof(CsrUint8);
    value                =  CsrPmemZalloc(valuesize);
/*TODO create default value*/
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            &attrHandle,
                                                            properties,
                                                            uuid16,
                                                            attrValueFlags,
                                                            valuesize,
                                                            value,
                                                            &tail);

    CsrPmemFree(value);
    properties           =  CSR_BT_GATT_CHARAC_PROPERTIES_WRITE|CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE;
    attrValueFlags       =  CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE;
    uuid16               =  CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC;
    valuesize            =  5*sizeof(CsrUint8);
    value                =  CsrPmemZalloc(valuesize);
/*TODO create default value*/
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            &attrHandle,
                                                            properties,
                                                            uuid16,
                                                            attrValueFlags,
                                                            valuesize,
                                                            value,
                                                            &tail);

    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        &attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        &tail);
    CsrPmemFree(value);
    return head;
}

