#ifndef CSR_BT_GAP_APP_UI_SEF_H__
#define CSR_BT_GAP_APP_UI_SEF_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_bt_gap_app_ui_strings.h"
#include "hci_prim.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CSR_BT_UNDEFINED                 0xFF

/* generel key defines                                  */
#define CSR_BT_GAP_FIRST_DYNAMIC_KEY                0x0001
#define CSR_BT_GAP_INVALID_KEY                      0x7FFF

/* key defines for the SD menu UI                    */
#define CSR_BT_GAP_SD_SEARCH_KEY                    0x0000
#define CSR_BT_GAP_SD_PROXIMITY_SEARCH_KEY          0x0001
#define CSR_BT_GAP_SD_CONFIGURE_SEARCH_KEY          0x0002
#define CSR_BT_GAP_SD_TRUSTED_DEIVCES_KEY           0x0003
#define CSR_BT_GAP_SD_READ_SERVICES_KEY             0x0004
#define CSR_BT_GAP_SD_READ_DEVICE_INFO_KEY          0x0005

/* key defines for the SC menu UI                    */
#define CSR_BT_GAP_SC_BOND_KEY                      0x0000
#define CSR_BT_GAP_SC_DEBOND_KEY                    0x0001
#define CSR_BT_GAP_SC_MODE_KEY                      0x0002
#define CSR_BT_GAP_SC_DISP_CAPB_KEY                 0x0003
#define CSR_BT_GAP_SC_AUTH_REQ_KEY                  0x0004

/* Key defines for Authentication requirement menu UI
 * Directly mapping authentication requirement values onto menu item keys */
#define CSR_BT_GAP_SC_AUTH_REQ_MITM_NOT_REQUIRED_DEDICATED_BONDING_KEY  HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING
#define CSR_BT_GAP_SC_AUTH_REQ_MITM_REQUIRED_DEDICATED_BONDING_KEY      HCI_MITM_REQUIRED_DEDICATED_BONDING

/* key defines for the CM menu UI                    */
#define CSR_BT_GAP_CM_SET_LOCAL_NAME_KEY                    0x0000
#define CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_KEY               0x0051
#define CSR_BT_GAP_CM_SET_MISCELLANEOUS_MAJOR_CLASS_KEY     0x0001
#define CSR_BT_GAP_CM_SET_COMPUTER_MAJOR_CLASS_KEY          0x0002
#define CSR_BT_GAP_CM_SET_PHONE_MAJOR_CLASS_KEY             0x0003
#define CSR_BT_GAP_CM_SET_LAN_MAJOR_CLASS_KEY               0x0004
#define CSR_BT_GAP_CM_SET_AV_MAJOR_CLASS_KEY                0x0005
#define CSR_BT_GAP_CM_SET_PERIPHERAL_MAJOR_CLASS_KEY        0x0006
#define CSR_BT_GAP_CM_SET_IMAGING_MAJOR_CLASS_KEY           0x0007
#define CSR_BT_GAP_CM_SET_HEALTH_MAJOR_CLASS_KEY            0x0008
#define CSR_BT_GAP_CM_SET_WEARABLE_MAJOR_CLASS_KEY          0x0009
#define CSR_BT_GAP_CM_SET_TOY_MAJOR_CLASS_KEY               0x000A
#define CSR_BT_GAP_CM_SET_UNCLASSIFIED_MAJOR_CLASS_KEY      0x000B
#define CSR_BT_GAP_CM_SET_CUSTOM_CLASS_KEY                  0x000C

#define CSR_BT_GAP_GATT_SET_OWN_ADDRESS_TYPE_KEY            0x0000

/* Constants with amount of minor device masks for a given major device class */
#define CSR_BT_GAP_CM_START_OF_COMPUTER_COD                 0    
#define CSR_BT_GAP_CM_AMOUNT_OF_COMPUTER_COD                7
#define CSR_BT_GAP_CM_AMOUNT_OF_PHONE_COD                   6 + CSR_BT_GAP_CM_AMOUNT_OF_COMPUTER_COD
#define CSR_BT_GAP_CM_AMOUNT_OF_LAN_COD                     9 + CSR_BT_GAP_CM_AMOUNT_OF_PHONE_COD 
#define CSR_BT_GAP_CM_AMOUNT_OF_AV_COD                      19 + CSR_BT_GAP_CM_AMOUNT_OF_LAN_COD
#define CSR_BT_GAP_CM_AMOUNT_OF_PERIPHERAL_COD              6 + CSR_BT_GAP_CM_AMOUNT_OF_AV_COD 
#define CSR_BT_GAP_CM_AMOUNT_OF_IMAGING_COD                 4 + CSR_BT_GAP_CM_AMOUNT_OF_PERIPHERAL_COD
#define CSR_BT_GAP_CM_AMOUNT_OF_WEARABLE_COD                5 + CSR_BT_GAP_CM_AMOUNT_OF_IMAGING_COD
#define CSR_BT_GAP_CM_AMOUNT_OF_TOY_COD                     5 + CSR_BT_GAP_CM_AMOUNT_OF_WEARABLE_COD
#define CSR_BT_GAP_CM_AMOUNT_OF_ALL_COD                     10 + CSR_BT_GAP_CM_AMOUNT_OF_TOY_COD

/* app csrUi event states                           */
#define CSR_BT_GAP_CREATE_SK1_EVENT                 0x01
#define CSR_BT_GAP_CREATE_SK2_EVENT                 0x02
#define CSR_BT_GAP_CREATE_DEL_EVENT                 0x03
#define CSR_BT_GAP_CREATE_BACK_EVENT                0x04

/* Handler functions for the CSR_UI upstream messages                           */
void CsrBtGapCsrUiUieCreateCfmHandler(CsrBtGapInstData * inst);
void CsrBtGapCsrUiInputdialogGetCfmHandler(CsrBtGapInstData* inst);
void CsrBtGapCsrUiEventIndHandler(CsrBtGapInstData * inst)
;


#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_GAP_APP_UI_SEF_H__ */
