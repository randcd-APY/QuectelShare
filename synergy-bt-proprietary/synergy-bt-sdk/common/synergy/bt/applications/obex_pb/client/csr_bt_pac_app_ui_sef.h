#ifndef CSR_BT_PAC_APP_UI_SEF_H__
#define CSR_BT_PAC_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

void CsrBtPacAppHandleUiConnectDisconnect(CsrBtPacAppInstData_t *inst);

void CsrBtPacAppHandleUiPrim(CsrBtPacAppInstData_t *inst);

void CsrBtPacAppUiPopup(CsrBtPacAppInstData_t *inst,
                               char *dialogHeading,
                               char *dialogText);

void CsrBtPacAppDownloadedUiPopup(CsrBtPacAppInstData_t *inst);

void CsrBtPacAppUiPopupHide(CsrBtPacAppInstData_t *inst);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PAC_APP_UI_SEF_H__ */

