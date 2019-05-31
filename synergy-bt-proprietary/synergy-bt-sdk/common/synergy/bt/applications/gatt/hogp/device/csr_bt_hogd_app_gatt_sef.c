/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_app_lib.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_hogd_app.h"
#include "csr_bt_hogd_app_db.h"
#include "csr_bt_hogd_app_strings.h"
#include "csr_bt_sc_lib.h"


static CsrBtGattDbAccessRspCode getCliConfigNotif(CsrBtGattAttrWritePairs* writeUnit,
                                                  CsrBool *notify)
{
    CsrBtGattDbAccessRspCode result = CSR_BT_GATT_ACCESS_RES_SUCCESS;

    if (writeUnit->valueLength == sizeof(CsrBtGattCliConfigBits))
    {
        CsrBtGattCliConfigBits cliConfig = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(writeUnit->value);
        if (cliConfig == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION
            || cliConfig == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT)
        {
            *notify = (cliConfig & CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION) ?
                            TRUE : FALSE;
        }
        else
        {
            /* Illegal value for this client configuration descriptor */
            result = CSR_BT_GATT_ACCESS_RES_CLIENT_CONFIG_IMPROPERLY_CONF;
        }
    }
    else
    {
        result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
    }

    return (result);
}

static void readAccessHandler(CsrBtHogdAppInstData *inst,
                              CsrBtGattDbAccessReadInd *ind)
{
    CsrUint8 *value = NULL;
    CsrUint16 size = 0;
    CsrBtResultCode result = CSR_BT_GATT_ACCESS_RES_SUCCESS;
    CsrBtGattHandle handle = ind->attrHandle;

    switch (handle - inst->db.hStart + 1)
    {
        case HIDS_PROTOCOL_HANDLE_OFFSET:
        {
            size = sizeof(inst->profileData.protocol);
            value = CsrMemDup(&inst->profileData.protocol, size);
            break;
        }
        case HIDS_INPUT_REPORT_HANDLE_OFFSET:
        {
            size = 5;
            value = CsrMemDup(&inst->profileData.mouseData, size);
            break;
        }
        case HIDS_INPUT_REPORT_CLIENT_CONFIG_HANDLE_OFFSET:
        {
            size = sizeof(CsrBtGattCliConfigBits);
            value = CsrPmemAlloc(size);
            *(CsrBtGattCliConfigBits *) value =
                            (inst->activeConnection->mouseInputReportNotify
                             != FALSE) ? CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION : CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
            break;
        }
        case HIDS_FEATURE_REPORT_HANDLE_OFFSET:
        {
            CsrBtHogdAppMouseData *mouseData = &inst->profileData.mouseData;
            size = 1;
            value = CsrPmemZalloc(size);
            *value = GET_FEATURE_REPORT(mouseData);
            break;
        }
        case HIDS_BOOT_MOUSE_INPUT_HANDLE_OFFSET:
        {
            size = sizeof(CsrBtHogdAppMouseBootReport);
            value = (CsrUint8 *) CsrBtHogdAppGetMouseBootInputReport(&inst->profileData.mouseData);
            break;
        }
        case HIDS_BOOT_MOUSE_INPUT_CLIENT_CONFIG_HANDLE_OFFSET:
        {
            size = sizeof(CsrBtGattCliConfigBits);
            value = CsrPmemAlloc(size);
            *(CsrBtGattCliConfigBits *) value =
                            (inst->activeConnection->mouseBootInputReportNotify
                             != FALSE) ? CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION : CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
            break;
        }
        case HIDS_CONTROL_POINT_HANDLE_OFFSET:
        {
            size = 1;
            value = CsrPmemAlloc(size);
            *value = (inst->lowPower != FALSE) ?
                            CSR_BT_HIDS_SUSPEND :
                            CSR_BT_HIDS_EXIT_SUSPEND;
            break;
        }
        case BAS_BATTERY_LEVEL_HANDLE_OFFSET:
        {
            size = 1;
            value = CsrMemDup(&inst->profileData.batteryPercentage, size);
            break;
        }
        case BAS_BATTERY_LEVEL_CLIENT_CONFIG_HANDLE_OFFSET:
        {
            size = sizeof(CsrBtGattCliConfigBits);
            value = CsrPmemAlloc(size);
            *(CsrBtGattCliConfigBits *) value =
                            (inst->activeConnection->batteryLevelNotify
                             != FALSE) ? CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION : CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
            break;
        }
        case SCP_SCAN_REFRESH_CLIENT_CONFIG_HANDLE_OFFSET:
        {
            size = sizeof(CsrBtGattCliConfigBits);
            value = CsrPmemAlloc(size);
            *(CsrBtGattCliConfigBits *) value =
                            (inst->activeConnection->scanRefreshNotify
                             != FALSE) ? CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION : CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;

            break;
        }
        default:
        {
            result = CSR_BT_GATT_ACCESS_RES_READ_NOT_PERMITTED;
            break;
        }
    }
    CsrBtGattDbReadAccessResSend(inst->gattId,
                                 inst->btConnId,
                                 handle,
                                 result,
                                 size,
                                 value);
}

static void writeAccessHandler(CsrBtHogdAppInstData *inst,
                               CsrBtGattDbAccessWriteInd *ind)
{
    CsrBtGattDbAccessRspCode result = CSR_BT_GATT_ACCESS_RES_SUCCESS;
    CsrBtGattHandle handle = ind->attrHandle;
    CsrUint16 count = ind->writeUnitCount;
    CsrBtGattAttrWritePairs *writeUnit = ind->writeUnit;

    /* We haven't set any permission. So don't expect GATT to ask us to check conditions.
     * Reliable write is not supported. */
    if (count == 1 && writeUnit != NULL && writeUnit->offset == 0)
    {
        switch (handle - inst->db.hStart + 1)
        {
            case HIDS_PROTOCOL_HANDLE_OFFSET:
            {
                if (writeUnit->valueLength == 1)
                {
                    inst->profileData.protocol = *(CsrUint8 *) writeUnit->value;
                }
                else
                {
                    result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
                }
                break;
            }
            case HIDS_INPUT_REPORT_CLIENT_CONFIG_HANDLE_OFFSET:
            {
                result = getCliConfigNotif(writeUnit,
                                           &inst->activeConnection->mouseInputReportNotify);
                break;
            }
            case HIDS_FEATURE_REPORT_HANDLE_OFFSET:
            {
                if (writeUnit->valueLength == 1)
                {
                    inst->profileData.mouseData.resolutionHorizontal =
                                    FEATURE_REPORT_HORIZONTAL_RESOLUTION(*(CsrUint8 * )writeUnit->value);
                    inst->profileData.mouseData.resolutionVertical =
                                    FEATURE_REPORT_VERTICAL_RESOLUTION(*(CsrUint8 * )writeUnit->value);
                }
                else
                {
                    result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
                }
                break;
            }
            case HIDS_BOOT_MOUSE_INPUT_CLIENT_CONFIG_HANDLE_OFFSET:
            {
                result = getCliConfigNotif(writeUnit,
                                           &inst->activeConnection->mouseBootInputReportNotify);
                break;
            }
            case HIDS_CONTROL_POINT_HANDLE_OFFSET:
            {
                if (writeUnit->valueLength == 1)
                {
                    switch (*(CsrUint8 *) writeUnit->value)
                    {
                        case CSR_BT_HIDS_SUSPEND:
                        {
                            inst->lowPower = TRUE;
                            break;
                        }
                        case CSR_BT_HIDS_EXIT_SUSPEND:
                        {
                            inst->lowPower = FALSE;
                            break;
                        }
                        default:
                        {
                            result = CSR_BT_GATT_ACCESS_RES_UNLIKELY_ERROR;
                        }
                    }
                }
                else
                {
                    result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
                }
                break;
            }
            case BAS_BATTERY_LEVEL_CLIENT_CONFIG_HANDLE_OFFSET:
            {
                result = getCliConfigNotif(writeUnit,
                                           &inst->activeConnection->batteryLevelNotify);
                break;
            }
            case SCP_SCAN_INTERVAL_WINDOW_HANDLE_OFFSET:
            {
                if (writeUnit->valueLength
                    == sizeof(CsrBtHogdAppClientScanParameters))
                {
                    inst->profileData.scanParameters =
                                    *(CsrBtHogdAppClientScanParameters *) writeUnit->value;
                    CsrBtHogdAppUiUpdateScanParams(inst);
                }
                else
                {
                    result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
                }
                break;
            }
            case SCP_SCAN_REFRESH_CLIENT_CONFIG_HANDLE_OFFSET:
            {
                result = getCliConfigNotif(writeUnit,
                                           &inst->activeConnection->scanRefreshNotify);
                break;
            }
            default:
            {
                result = CSR_BT_GATT_ACCESS_RES_WRITE_NOT_PERMITTED;
                break;
            }
        }
    }
    else if (count == 0)
    {
        result = CSR_BT_GATT_ACCESS_RES_UNLIKELY_ERROR;
    }
    else
    {
        result = CSR_BT_GATT_ACCESS_RES_NOT_LONG;
    }
    CsrBtGattDbWriteAccessResSend(inst->gattId,
                                  inst->btConnId,
                                  handle,
                                  result);
}

/******************************************************************************
 * Function to handle all hogd-primitives.
 ******************************************************************************/
void CsrBtHogdAppGattPrimHandler(CsrBtHogdAppInstData *inst)
{
    CsrPrim *primType;

    primType = (CsrPrim *) inst->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
        {
            CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*) inst->recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                inst->gattId = cfm->gattId;
                /* Allocate and add DB in Gatt*/
                CsrBtGattDbAllocReqSend(inst->gattId,
                                        CSR_BT_HOGD_DB_HANDLE_COUNT,
                                        inst->db.hStart);
            }
            else
            {
                CsrBtHogdAppUiPopup(inst, "Error", "Cannot register with Gatt");
            }

            break;
        }
        case CSR_BT_GATT_UNREGISTER_CFM:
        {
            CsrBtHogdAppUiSetDeactivated(inst);
            break;
        }
        case CSR_BT_GATT_DB_ALLOC_CFM:
        {
            CsrBtGattDbAllocCfm *cfm = (CsrBtGattDbAllocCfm*) inst->recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                inst->db.hStart = cfm->start;
                inst->db.hEnd = cfm->end;
                CsrBtHogdAppCreateDb(inst);
                CsrBtGattDbAddReqSend(inst->gattId, inst->db.dbPrimServices);
            }
            else
            {
                CsrBtHogdAppUiPopup(inst, "Error", "Unable to create database");
            }
            break;
        }
        case CSR_BT_GATT_DB_ADD_CFM:
        {
            CsrBtGattDbAddCfm *cfm = (CsrBtGattDbAddCfm*) inst->recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrBtHogdAppStartPeripheralRole(inst);
            }
            else
            {
                CsrBtHogdAppUiPopup(inst, "Error", "Unable to create database");
            }
            break;
        }
        case CSR_BT_GATT_PERIPHERAL_CFM:
        {
            CsrBtGattPeripheralCfm *cfm =
                            (CsrBtGattPeripheralCfm*) inst->recvMsgP;

            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                inst->btConnId = cfm->btConnId;
                CsrBtHogdAppUiSetActivated(inst);
            }
            else
            {
                inst->btConnId = CSR_BT_CONN_ID_INVALID;
                CsrBtHogdAppUiPopup(inst, "Error", "Activation failed");
                CsrBtHogdAppUiSetDeactivated(inst);
            }

            break;
        }
        case CSR_BT_GATT_CONNECT_IND:
        {
            CsrBtGattConnectInd *ind;

            ind = (CsrBtGattConnectInd *) inst->recvMsgP;

            if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrBtHogdAppConnection *connection =
                                (CsrBtHogdAppConnection *) CSR_BT_HOGD_APP_FIND_CONNECTION_BY_ADDR(inst->connections,
                                                                                                   &ind->address);

                if (connection == NULL)
                {
                    connection = (CsrBtHogdAppConnection *) CSR_BT_HOGD_APP_ADD_CONNECTION(inst->connections);
                    CsrBtBdAddrCopy(&connection->connectedDeviceAddr,
                                    &ind->address);
                }
                inst->activeConnection = connection;

                CsrBtHogdAppUiSetConnected(inst);

                CsrBtGattSecurityReqSend(inst->gattId,
                                         ind->btConnId,
                                         CSR_BT_GATT_SECURITY_FLAGS_AUTHENTICATED);

                CsrAppBacklogReqSend(TECH_BTLE,
                                     PROFILE_NAME(HOGD_APP_PROFILE_NAME),
                                     TRUE,
                                     "Successfully Connected with: %04X:%02X:%06X",
                                     ind->address.addr.nap,
                                     ind->address.addr.uap,
                                     ind->address.addr.lap);
            }
            else
            {
                CsrBtHogdAppUiPopup(inst, "Error", "Connection failed");
            }
            break;
        }
        case CSR_BT_GATT_DISCONNECT_IND:
        {
            inst->activeConnection = NULL;
            if (inst->gattId != CSR_BT_GATT_INVALID_GATT_ID)
            {
                CsrBtHogdAppStartPeripheralRole(inst);
                CsrBtHogdAppUiSetActivated(inst);
            }
            break;
        }
        case CSR_BT_GATT_WHITELIST_ADD_CFM:
        {
            CsrBtGattWhitelistAddCfm *cfm =
                            (CsrBtGattWhitelistAddCfm*) inst->recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrBtHogdAppUiUpdateWhitelist(inst);
            }
            else
            {
                inst->activeConnection->whitelist = FALSE;
            }

            break;
        }
        case CSR_BT_GATT_WHITELIST_READ_CFM:
        {
            if (inst->activeConnection->whitelist == FALSE)
            {
                CsrBtGattWhitelistReadCfm *cfm =
                                (CsrBtGattWhitelistReadCfm *) inst->recvMsgP;
                if (cfm->addressCount > 0)
                {
                    CsrUint8 i, count = cfm->addressCount - 1;
                    CsrBtTypedAddr *addresses =
                                    (CsrBtTypedAddr *) CsrMemDup(cfm->address,
                                                                 count);
                    for (i = 0; i < count; i++)
                    {
                        if (CsrBtBdAddrEq(&cfm->address[i].addr,
                                          &inst->activeConnection->connectedDeviceAddr.addr))
                        {
                            addresses[i] = cfm->address[count];
                            break;
                        }
                    }
                    CsrBtGattWhitelistClearReqSend(inst->gattId);
                    CsrBtGattWhitelistAddReqSend(inst->gattId, count, addresses);
                    CsrPmemFree(cfm->address);
                }
                CsrBtHogdAppUiUpdateWhitelist(inst);
            }
            break;
        }
        case CSR_BT_GATT_WHITELIST_CLEAR_CFM:
        {
            break;
        }
        case CSR_BT_GATT_DB_ACCESS_READ_IND:
        {
            readAccessHandler(inst, inst->recvMsgP);
            break;
        }
        case CSR_BT_GATT_DB_ACCESS_WRITE_IND:
        {
            writeAccessHandler(inst, inst->recvMsgP);
            break;
        }
        case CSR_BT_GATT_SECURITY_CFM:
        {
            CsrBtGattSecurityCfm *cfm = (CsrBtGattSecurityCfm *) inst->recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS
                && cfm->resultSupplier == CSR_BT_SUPPLIER_GATT)
            {
                ;
            }
            break;
        }
        default:
        {
            /* unexpected primitive received */
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM,
                                                 inst->recvMsgP);
            break;
        }
    }
}

