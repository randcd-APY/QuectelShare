/******************************************************************************

Copyright (c) 2010-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

/* IMPORTANT NOTICE: This file sets up basic and constant settings for
 * the CSR Synergy BlueCore bootstrap system.  You may need to tweak
 * the settings in this file to match your platform */

#include "csr_bt_bootstrap_generic.h"
#include "csr_bt_bootstrap_specific.h"

#define MACRO_EXP(X) MACRO_EXP_Q(X)
#define MACRO_EXP_Q(X)          #X
/* Default PSR files and paths */
#define _PSRPATH_	DEFAULT_PSR_PATH
#if defined(CSR_BT_INTEROP_TEST_BUILD)
#define CSR_BT_BOOTSTRAP_DEFAULT_PLATFORM _PSRPATH_"ps-interop_test.psr"
#else
#define CSR_BT_BOOTSTRAP_DEFAULT_PLATFORM _PSRPATH_"ps-default.psr"
#endif

/* Table of build-ids and associated patch bundles. Note that some
 * patch bundles exist in different variants based on what chip
 * package (CSP/QFN/BGA) that is used */
typedef struct
{
    CsrUint16 id;
    CsrCharString *patch;
    CsrBool bc7up;
} csrBtBootstrapIdPatchTable;
static const csrBtBootstrapIdPatchTable csrBtBootstrapTable[] =
{
    { 1958, _PSRPATH_"pb-2r17_1958.psr",                FALSE }, /* BC4-ROM A04 */
    { 3164, _PSRPATH_"pb-14r2_3164.psr",                FALSE }, /* BC4-ROM A06 */
    { 4363, _PSRPATH_"pb-22r14_4363.psr",               FALSE }, /* BC5-FM A12 */
    { 4698, _PSRPATH_"pb-26r17_4698.psr",               FALSE }, /* BC5-FM A14 */
    { 4839, _PSRPATH_"pb-28r2_4839.psr",                FALSE }, /* BC4-ROM A07 */
    { 4841, _PSRPATH_"pb-27r26_4841.psr",               FALSE }, /* BC6-ROM A04 */
    { 6311, _PSRPATH_"pb-74r3_6311.psr",                FALSE }, /* BC6-ROM A05 */
    { 5985, _PSRPATH_"pb-51r22_5985_csp0p5.psr",        TRUE  }, /* BC7820 A12, CSP-0.5 */
    { 6247, _PSRPATH_"pb-53r11_6247_csp.psr",           TRUE  }, /* BC7820 A16/BC7830 A16, CSP */
    { 5892, _PSRPATH_"pb-49r3_5892.psr",                TRUE  }, /* BC7830 A14 */
    { 6817, _PSRPATH_"pb-90r1_6817.psr",                TRUE  }, /* CSR8810 A04 */
    { 7557, _PSRPATH_"pb-101r12_7558_csr8311_uart.psr", TRUE  }, /* CSR8811 - LPC Final Release Candidate */
    { 7558, _PSRPATH_"pb-101r12_7558_csr8311_uart.psr", TRUE  }, /* CSR8311 A06 ROM */
    { 7727, _PSRPATH_"ps-gemini_a06.psr",               TRUE  }, /* CSR8811 [LPC pre-A08] */
    { 7837, _PSRPATH_"ps-gemini_a06.psr",               TRUE  }, /* CSR8505 with LPC [LPC pre-A08] */
    { 8211, _PSRPATH_"ps-gemini_a06.psr",               TRUE  }, /* CSR8505 with LPC [LPC pre-A08] RC7 */
    { 8231, _PSRPATH_"ps-gemini_a06.psr",               TRUE  }, /* CSR8505 with LPC [LPC pre-A08] RC9 */
    { 8129, _PSRPATH_"ps-gemini_a06.psr",               TRUE  }, /* CSR8505 with LPC B-98367 */
    { 8241, _PSRPATH_"pb-109r16_8241_csr8311.psr",      TRUE  }, /* CSR8311 Gemini A08 */
    { 8833, _PSRPATH_"pb-119r3_8833.psr",               TRUE  }, /* CSR8820 BGA */
    { 9079, _PSRPATH_"pb-132r10_9079_csr8350.psr",      TRUE  }, /* Dale Auto A07 (CSR8350) */
    { 8891, _PSRPATH_"pb-120r16_8891_csr8510.psr",      TRUE  }, /* Gemini A10 (CSR8510) */
    { 10520, _PSRPATH_"pb-207r7_10520_csr8311.psr",     TRUE  }, /* Gemini A12 (CSR8311) */
    { 10391, _PSRPATH_"pb-206r4_10391_csr8350.psr",     TRUE  }, /* Dale Auto A08 (CSR8350) */
};

static const csrBtBootstrapIdPatchTable csrBtBootstrapUsbTable[] =
{
    { 7558, _PSRPATH_"pb-101r12_7558_csr8311_usb.psr", TRUE  },  /* CSR8311 A06 ROM */
    { 8241, _PSRPATH_"pb-109r13_8241_csr8311_usb.psr", TRUE  },  /* CSR8311 A08 QFN */
};

/* Application developer implemented function. This function will
 * automatically get called by the bootstrap process to retrieve any
 * platform specific settings that can only be determined at run-time
 * - e.g. the Bluetooth device friendly name, frequency trim,
 * Bluetooth address, etc. */
void CsrBtBootstrapSpecific(void *bootContext,
                            CsrUint16 buildId,
                            CsrBool *bc7up)
{
    CsrCharString *patch = NULL;
    CsrUint16 i;
    csrBtBootstrapInst  *inst = bootContext;
    
    if (inst->iface == CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H2_USB)
    {
        for (i=0; i<CSR_ARRAY_SIZE(csrBtBootstrapUsbTable); i++)
        {
            if(csrBtBootstrapUsbTable[i].id == buildId)
            {
                patch = csrBtBootstrapUsbTable[i].patch;
                *bc7up = csrBtBootstrapUsbTable[i].bc7up;
                CsrBtBootstrapParsePsrConstant(bootContext, patch);
            }
        }
    }
    if (patch == NULL)
    {
        for(i=0; i<CSR_ARRAY_SIZE(csrBtBootstrapTable); i++)
        {
            if(csrBtBootstrapTable[i].id == buildId)
            {
                patch = csrBtBootstrapTable[i].patch;
                *bc7up = csrBtBootstrapTable[i].bc7up;
                CsrBtBootstrapParsePsrFile(bootContext, patch);
            }
        }
    }

#if defined(CSR_AMP_ENABLE)
#if defined(CSR_WIFI_DESIGN_M2107_R02) || defined(CSR_WIFI_DESIGN_M2107_R03)
    CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-jem_a12.psr");
    CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-m2107_coex.psr");
#endif
#if defined(CSR_WIFI_DESIGN_M2399_R03)
    CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-m2399_coex.psr");
#endif
#if defined(CSR_WIFI_DESIGN_M2501_R03)
    CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-m2501_coex.psr");
#endif
#endif /* CSR_AMP_ENABLE */

    /* Load default platform settings */
#ifdef CSR_USE_BLUECORE_CHIP
    CsrBtBootstrapParsePsrFile(bootContext, 
                               CSR_BT_BOOTSTRAP_DEFAULT_PLATFORM);
#endif
    if (inst->iface == CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H2_USB)
    {
        CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-host_usb.psr");
    }
    else
    {
        CsrBtBootstrapParsePsrFile(bootContext, _PSRPATH_"ps-host_bcsp.psr");
    }
}
