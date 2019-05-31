/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_services_defines.h"
#include "csr_bt_gatt_demo_db_utils.h"
#include "csr_bt_hogd_app.h"
#include "csr_bt_hogd_app_db.h"
#include "csr_bt_hogd_app_strings.h"


/******************************************************************************
 Wheel Mouse - simplified version - 5 button, vertical and horizontal wheel

 Input report - 5 bytes

 Byte | D7      D6      D5      D4      D3      D2      D1      D0
 ------+---------------------------------------------------------------------
 0   |  0       0       0    Forward  Back    Middle  Right   Left (Buttons)
 1   |                             X
 2   |                             Y
 3   |                       Vertical Wheel
 4   |                    Horizontal (Tilt) Wheel

 Feature report - 1 byte

 Byte | D7      D6      D5      D4   |  D3      D2  |   D1      D0
 ------+------------------------------+--------------+----------------
 0   |  0       0       0       0   |  Horizontal  |    Vertical
 (Resolution multiplier)
 ******************************************************************************/

static const CsrUint8 HID_REPORT_DESCRIPTOR[] =
{
  0x05,
  0x01, /* USAGE_PAGE (Generic Desktop)*/
  0x09, 0x02,   /* USAGE (Mouse)*/
  0xa1, 0x01, /* COLLECTION (Application)*/
  0x09, 0x02, /*   USAGE (Mouse)*/
  0xa1, 0x02, /*   COLLECTION (Logical)*/
  0x09, 0x01, /*     USAGE (Pointer)*/
  0xa1, 0x00, /*     COLLECTION (Physical)*/
              /* ------------------------------  Buttons*/
  0x05, 0x09, /*       USAGE_PAGE (Button)*/
  0x19, 0x01, /*       USAGE_MINIMUM (Button 1)*/
  0x29, 0x05, /*       USAGE_MAXIMUM (Button 5)*/
  0x15, 0x00, /*       LOGICAL_MINIMUM (0)*/
  0x25, 0x01, /*       LOGICAL_MAXIMUM (1)*/
  0x75, 0x01, /*       REPORT_SIZE (1)*/
  0x95, 0x05, /*       REPORT_COUNT (5)*/
  0x81, 0x02, /*       INPUT (Data,Var,Abs)*/
              /* ------------------------------  Padding*/
  0x75, 0x03, /*       REPORT_SIZE (3)*/
  0x95, 0x01, /*       REPORT_COUNT (1)*/
  0x81, 0x03, /*       INPUT (Cnst,Var,Abs)*/
              /* ------------------------------  X,Y position*/
  0x05, 0x01, /*       USAGE_PAGE (Generic Desktop)*/
  0x09, 0x30, /*       USAGE (X)*/
  0x09, 0x31, /*       USAGE (Y)*/
  0x15, 0x81, /*       LOGICAL_MINIMUM (-127)*/
  0x25, 0x7f, /*       LOGICAL_MAXIMUM (127)*/
  0x75, 0x08, /*       REPORT_SIZE (8)*/
  0x95, 0x02, /*       REPORT_COUNT (2)*/
  0x81, 0x06, /*       INPUT (Data,Var,Rel)*/
  0xa1, 0x02, /*       COLLECTION (Logical)*/
              /* ------------------------------  Vertical wheel res multiplier*/
  0x09, 0x48, /*         USAGE (Resolution Multiplier)*/
  0x15, 0x00, /*         LOGICAL_MINIMUM (0)*/
  0x25, 0x01, /*         LOGICAL_MAXIMUM (1)*/
  0x35, 0x01, /*         PHYSICAL_MINIMUM (1)*/
  0x45, 0x04, /*         PHYSICAL_MAXIMUM (4)*/
  0x75, 0x02, /*         REPORT_SIZE (2)*/
  0x95, 0x01, /*         REPORT_COUNT (1)*/
  0xa4,          /*         PUSH*/
  0xb1, 0x02, /*         FEATURE (Data,Var,Abs)*/
              /* ------------------------------  Vertical wheel*/
  0x09, 0x38, /*         USAGE (Wheel)*/
  0x15, 0x81, /*         LOGICAL_MINIMUM (-127)*/
  0x25, 0x7f, /*         LOGICAL_MAXIMUM (127)*/
  0x35, 0x00, /*         PHYSICAL_MINIMUM (0)        - reset physical*/
  0x45, 0x00, /*         PHYSICAL_MAXIMUM (0)*/
  0x75, 0x08, /*         REPORT_SIZE (8)*/
  0x81, 0x06, /*         INPUT (Data,Var,Rel)*/
  0xc0,  /*       END_COLLECTION*/
  0xa1, 0x02, /*       COLLECTION (Logical)*/
              /* ------------------------------  Horizontal wheel res multiplier*/
  0x09, 0x48, /*         USAGE (Resolution Multiplier)*/
  0xb4,           /*         POP*/
  0xb1, 0x02, /*         FEATURE (Data,Var,Abs)*/
              /* ------------------------------  Padding for Feature report*/
  0x35, 0x00, /*         PHYSICAL_MINIMUM (0)        - reset physical*/
  0x45, 0x00, /*         PHYSICAL_MAXIMUM (0)*/
  0x75, 0x04, /*         REPORT_SIZE (4)*/
  0xb1, 0x03, /*         FEATURE (Cnst,Var,Abs)*/
              /* ------------------------------  Horizontal wheel*/
  0x05, 0x0c, /*         USAGE_PAGE (Consumer Devices)*/
  0x0a, 0x38, 0x02, /*         USAGE (AC Pan)*/
  0x15, 0x81, /*         LOGICAL_MINIMUM (-127)*/
  0x25, 0x7f, /*         LOGICAL_MAXIMUM (127)*/
  0x75, 0x08, /*         REPORT_SIZE (8)*/
  0x81, 0x06, /*         INPUT (Data,Var,Rel)*/
  0xc0,  /*       END_COLLECTION*/
  0xc0,    /*     END_COLLECTION*/
  0xc0,      /*   END_COLLECTION*/
  0xc0        /* END_COLLECTION*/
};

static const CsrBtHogdAppDescReportReference MOUSE_INPUT_REPORT_REFERENCE =
{
  0,
  CSR_BT_HIDS_REPORT_TYPE_INPUT
};

static const CsrBtHogdAppDescReportReference MOUSE_FEATURE_REPORT_REFERENCE =
{
  0,
  CSR_BT_HIDS_REPORT_TYPE_FEATURE
};

static const CsrBtHidsInformation HID_INFORMATION =
{
  0x111,
  0,
  CSR_BT_HIDS_INFO_FLAGS_REMOTE_WAKE | CSR_BT_HIDS_INFO_FLAGS_NORMALLY_CONNECTABLE
};

static CsrBtGattDb *createHidService(CsrBtGattDb *head,
                                     CsrBtGattDb **tail,
                                     CsrBtGattHandle *attrHandle)
{
    /*********************** Service 3: HID service ****************************
     Handle          UUID    Value                                                                       Permissions                 Comment
     (startHandle+)
     1               0x2800  UUID=0x1812                                                                 Read                        Primary service declaration. HID service declared.
     2               0x2803  handle=startHandle+3, UUID=0x2A4E, (Read|WriteWithoutResponse)              Read                        Characteristic declaration for protocol mode.
     3               0x2A4E  <<protocol mode>>                                                           Read|WriteWithoutResponse   Protocol mode is uint8 value with following possible values: 0=Boot protocol mode, 1=Report protocol mode
     4               0x2803  handle=startHandle+5, UUID=0x2A4D, (Read|Notify)                            Read                        Characteristic declaration for mouse input report
     5               0x2A4D  <<mouse input report>>                                                      Read|Notify                 Mouse input report is array of 5 bytes type
     6               0x2902  <<mouse input report Client Characteristic Configuration>>                  Read|Write                  Mouse input report Client Characteristic Configuration. Possible values are 0 and 1.
     7               0x2908  ReportID=0, ReportType=1                                                    Read                        Report reference descriptor for mouse input report
     8               0x2803  handle=startHandle+9, UUID=0x2A4D, (Read|Write)                             Read                        Characteristic declaration for mouse feature report
     9               0x2A4D  <<mouse feature report>>                                                    Read|Write                  Mouse feature report is uint8 type. Holds vertical and horizontal scroll resolution.
     10              0x2908  ReportID=0, ReportType=3                                                    Read                        Report reference descriptor for mouse feature report
     11              0x2803  handle=startHandle+12, UUID=0x2A4B, Read                                    Read                        Characteristic declaration for mouse report map
     12              0x2A4B  <<report map>>                                                              Read                        Report map is array of unint8 type
     13              0x2803  handle=startHandle+14, UUID=0x2A32, (Read|Notify)                           Read                        Characteristic declaration for boot mouse input report
     14              0x2A32  <<boot mouse input report>>                                                 Read|Notify                 Boot mouse input report is array of 3 bytes
     15              0x2902  <<boot mouse input report client characteristic configuration>>             Read|Write                  Boot mouse input report client characteristic configuration. Possible values are 0 and 1.
     16              0x2803  handle=startHandle+17, UUID=0x2A4A, Read                                    Read                        Characteristic declaration for HID information
     17              0x2A4A  bcdHID=0x111, bCountryCode=13, flags(RemoteWake=1, NormallyConnectable=1)   Read                        HID information
     18              0x2803  handle=startHandle+16, UUID=0x2A4C, WriteWithoutResponse                    Read                        Characteristic declaration for HID control point
     19              0x2A4C  <<HID control point>>                                                       WriteWithoutResponse        HID control point is a 8-bit value. Possible values are Suspend(0) and Exit Suspend(1).
     **************************************************************************/
    /* HID service */
    head = CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(head,
                                                          attrHandle,
                                                          CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE,
                                                          TRUE,
                                                          tail);
    /* Protocol mode characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                                            | CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
                                                            CSR_BT_GATT_UUID_PROTOCOL_MODE_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                                            | CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                                            | CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                                            | CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);
    /* Mouse input report characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                                            | CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY,
                                                            CSR_BT_GATT_UUID_REPORT_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                                            | CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);
    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        tail);
    head = CsrBtGattUtilCreateDbEntryFromUuid16(head,
                                                attrHandle,
                                                CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC,
                                                CSR_BT_GATT_PERM_FLAGS_READ,
                                                CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                sizeof(MOUSE_INPUT_REPORT_REFERENCE),
                                                (CsrUint8 *) &MOUSE_INPUT_REPORT_REFERENCE,
                                                tail);
    /* Mouse feature report characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                                            | CSR_BT_GATT_CHARAC_PROPERTIES_WRITE,
                                                            CSR_BT_GATT_UUID_REPORT_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                                            | CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                                                            | CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                                            | CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);
    head = CsrBtGattUtilCreateDbEntryFromUuid16(head,
                                                attrHandle,
                                                CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC,
                                                CSR_BT_GATT_PERM_FLAGS_READ,
                                                CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                sizeof(MOUSE_FEATURE_REPORT_REFERENCE),
                                                (CsrUint8 *) &MOUSE_FEATURE_REPORT_REFERENCE,
                                                tail);
    /* Mouse report map characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_UUID_REPORT_MAP_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(HID_REPORT_DESCRIPTOR),
                                                            HID_REPORT_DESCRIPTOR,
                                                            tail);
    /* Boot mouse input report characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                                            | CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY,
                                                            CSR_BT_GATT_UUID_BOOT_MOUSE_INPUT_REPORT_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                                            | CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);
    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        tail);
    /* HID information characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_UUID_HID_INFORMATION_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(HID_INFORMATION),
                                                            (CsrUint8 *) &HID_INFORMATION,
                                                            tail);
    /* HID control point characteristic */
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
                                                            CSR_BT_GATT_UUID_HID_CONTROL_POINT_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                                            | CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);

    return (head);
}

static CsrBtGattDb *createBatteryService(CsrBtGattDb *head,
                                         CsrBtGattDb **tail,
                                         CsrBtGattHandle *attrHandle)
{
    head = CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(head,
                                                          attrHandle,
                                                          CSR_BT_GATT_SERVICE_BATTERY_UUID,
                                                          FALSE,
                                                          tail);
    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ
                                                            | CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY,
                                                            CSR_BT_GATT_SERVICE_BATT_LEVEL_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_READ
                                                            | CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            0, /*value size */
                                                            NULL,
                                                            tail);
    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        tail);
    return (head);
}

static CsrBtGattDb *createDeviceInformationService(CsrBtGattDb *head,
                                                   CsrBtGattDb **tail,
                                                   CsrBtGattHandle *attrHandle)
{
    head = CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(head,
                                                          attrHandle,
                                                          CSR_BT_GATT_SERVICE_HEALTH_DEV_INFO_UUID,
                                                          FALSE,
                                                          tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_MANUFACT_NAME_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(MANUFACTURER_NAME),
                                                            MANUFACTURER_NAME,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_MODEL_NUM_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(MODEL_NUMBER),
                                                            MODEL_NUMBER,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_SERIAL_NUM_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(SERIAL_NUMBER),
                                                            SERIAL_NUMBER,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_HW_REVISION_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(HARDWARE_REVISION_NUMBER),
                                                            HARDWARE_REVISION_NUMBER,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_FW_REVISION_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(FIRMWARE_REVISION_NUMBER),
                                                            FIRMWARE_REVISION_NUMBER,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_READ,
                                                            CSR_BT_GATT_SERVICE_HEALTH_INFO_SW_REVISION_CHARAC_UUID,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            sizeof(SOFTWARE_REVISION_NUMBER),
                                                            SOFTWARE_REVISION_NUMBER,
                                                            tail);
    return (head);
}

static CsrBtGattDb *createScanParameterService(CsrBtGattDb *head,
                                               CsrBtGattDb **tail,
                                               CsrBtGattHandle *attrHandle)
{
    head = CsrBtGattUtilCreatePrimaryServiceWith16BitUuid(head,
                                                          attrHandle,
                                                          CSR_BT_GATT_UUID_SCAN_PARAMETERS_SERVICE,
                                                          FALSE,
                                                          tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
                                                            CSR_BT_GATT_UUID_SCAN_INTERVAL_WINDOW_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                                                            | CSR_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);

    head = CsrBtGattUtilCreateCharacDefinitionWith16BitUuid(head,
                                                            attrHandle,
                                                            CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY,
                                                            CSR_BT_GATT_UUID_SCAN_REFRESH_CHARAC,
                                                            CSR_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION,
                                                            0,
                                                            NULL,
                                                            tail);
    head = CsrBtGattUtilCreateClientCharacConfiguration(head,
                                                        attrHandle,
                                                        CSR_BT_GATT_ATTR_FLAGS_NONE,
                                                        tail);
    return (head);
}

void CsrBtHogdAppCreateDb(CsrBtHogdAppInstData *inst)
{
    CsrBtHogdAppDatabase *db = &inst->db;
    CsrBtGattHandle handle = db->hStart;
    CsrBtGattDb *head = NULL, *tail = NULL;
    head = createHidService(head, &tail, &handle);
    head = createBatteryService(head, &tail, &handle);
    head = createDeviceInformationService(head, &tail, &handle);
    head = createScanParameterService(head, &tail, &handle);

    db->dbPrimServices = head;
    db->hEnd = handle;
}
