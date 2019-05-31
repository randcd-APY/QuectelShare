#ifndef CSR_BT_PAC_APP_FILES_H__
#define CSR_BT_PAC_APP_FILES_H__

/******************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_bt_pb_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_PAC_APP_FILE_SIM_PB  (CSR_BT_PB_FOLDER_SIM1_STR "-" CSR_BT_PB_FOLDER_PB_VCF)
#define CSR_BT_PAC_APP_FILE_SIM_ICH (CSR_BT_PB_FOLDER_SIM1_STR "-" CSR_BT_PB_FOLDER_ICH_VCF)
#define CSR_BT_PAC_APP_FILE_SIM_OCH (CSR_BT_PB_FOLDER_SIM1_STR "-" CSR_BT_PB_FOLDER_OCH_VCF)
#define CSR_BT_PAC_APP_FILE_SIM_MCH (CSR_BT_PB_FOLDER_SIM1_STR "-" CSR_BT_PB_FOLDER_MCH_VCF)
#define CSR_BT_PAC_APP_FILE_SIM_CCH (CSR_BT_PB_FOLDER_SIM1_STR "-" CSR_BT_PB_FOLDER_CCH_VCF)

#define CSR_BT_PAC_APP_FILE_LISTING_TXT     "Listing"
#define CSR_BT_PAC_APP_FILE_LISTING_OUTPUT  (CSR_BT_PAC_APP_FILE_LISTING_TXT CSR_BT_PB_VCF_EXT)

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PAC_APP_FILES_H__ */
