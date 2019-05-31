/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

/* IMPORTANT NOTICE: This file sets up basic and constant settings for
 * the CSR Synergy BlueCore bootstrap system.  You may need to tweak
 * the settings in this file to match your platform */

#include "csr_bt_bootstrap_generic.h"
#include "csr_bt_bootstrap_specific.h"

#define CSR_BT_BOOTSTRAP_MY_PATCH "pb-bc7fm.psr"
#define CSR_BT_BOOTSTRAP_MY_SETTINGS "ps-handset.psr"

/* Example file for a custom platform bootstrap file */
void CsrBtBootstrapSpecific(void *bootContext,
                            CsrUint16 buildId,
                            CsrBool *bc7up)
{
    /* Load patch bundle for this chip */
    CsrBtBootstrapParsePsrFile(bootContext, CSR_BT_BOOTSTRAP_MY_PATCH);

    /* Load default platform settings */
    CsrBtBootstrapParsePsrFile(bootContext, CSR_BT_BOOTSTRAP_MY_SETTINGS);

    /* Don't care about the buildId - we know _exactly_ what chip
     * we're using in this project */
    CSR_UNUSED(buildId);
}
