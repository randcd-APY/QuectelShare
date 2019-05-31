/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
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

/* Bootstrap strings (implemented in bootstrap/psr/pb-...c) */
extern const CsrCharString bootstrap_patch_string_pb_14r2_3164_psr[];
extern const CsrCharString bootstrap_patch_string_pb_22r14_4363_psr[];
extern const CsrCharString bootstrap_patch_string_pb_26r17_4698_psr[];
extern const CsrCharString bootstrap_patch_string_pb_28r2_4839_psr[];
extern const CsrCharString bootstrap_patch_string_pb_27r26_4841_psr[];
extern const CsrCharString bootstrap_patch_string_pb_74r3_6311_psr[];
extern const CsrCharString bootstrap_patch_string_pb_2r17_1958_psr[];
extern const CsrCharString bootstrap_patch_string_pb_49r3_5892_psr[];
extern const CsrCharString bootstrap_patch_string_pb_51r22_5985_bga_psr[];
extern const CsrCharString bootstrap_patch_string_pb_51r22_5985_csp0p4_psr[];
extern const CsrCharString bootstrap_patch_string_pb_51r22_5985_csp0p5_psr[];
extern const CsrCharString bootstrap_patch_string_pb_53r11_6247_csp_psr[];
extern const CsrCharString bootstrap_patch_string_pb_53r11_6247_qfn_psr[];
extern const CsrCharString bootstrap_patch_string_pb_90r1_6817_psr[];
extern const CsrCharString bootstrap_patch_string_pb_101r12_7558_csr8311_uart_psr[];
extern const CsrCharString bootstrap_patch_string_pb_101r12_7558_csr8311_usb_psr[];
extern const CsrCharString bootstrap_patch_string_pb_109r16_8241_csr8311_psr[];
extern const CsrCharString bootstrap_patch_string_pb_119r3_8833_psr[];
extern const CsrCharString bootstrap_patch_string_ps_default_psr[];
extern const CsrCharString bootstrap_patch_string_ps_host_bcsp_psr[];
extern const CsrCharString bootstrap_patch_string_ps_host_usb_psr[];
extern const CsrCharString bootstrap_patch_string_ps_ble_psr[];
extern const CsrCharString bootstrap_patch_string_ps_gemini_a06_psr[];
extern const CsrCharString bootstrap_patch_string_ps_purdey_7959_psr[];
extern const CsrCharString bootstrap_patch_string_ps_jem_a12_psr[];
extern const CsrCharString bootstrap_patch_string_ps_m2107_coex_psr[];
extern const CsrCharString bootstrap_patch_string_ps_m2399_coex_psr[];
extern const CsrCharString bootstrap_patch_string_ps_m2501_coex_psr[];
extern const CsrCharString bootstrap_patch_string_pb_132r10_9079_csr8350_psr[];
extern const CsrCharString bootstrap_patch_string_pb_120r16_8891_csr8510_psr[];
extern const CsrCharString bootstrap_patch_string_ps_interop_test_psr[];
extern const CsrCharString bootstrap_patch_string_pb_207r7_10520_csr8311_psr[];
extern const CsrCharString bootstrap_patch_string_pb_206r4_10391_csr8350_psr[];

/* Table of build-ids and associated patch bundles. Note that some
 * patch bundles exist in different variants based on what chip
 * package (CSP/QFN/BGA) that is used */
typedef struct
{
    CsrUint16 id;
    const CsrCharString *patch;
    CsrBool bc7up;
} csrBtBootstrapIdPatchTable;
static const csrBtBootstrapIdPatchTable csrBtBootstrapTable[] =
{
    { 1958, bootstrap_patch_string_pb_2r17_1958_psr,                FALSE }, /* BC4-ROM A04 */
    { 3164, bootstrap_patch_string_pb_14r2_3164_psr,                FALSE }, /* BC4-ROM A06 */
    { 4363, bootstrap_patch_string_pb_22r14_4363_psr,               FALSE }, /* BC5-FM A12 */
    { 4698, bootstrap_patch_string_pb_26r17_4698_psr,               FALSE }, /* BC5-FM A14 */
    { 4839, bootstrap_patch_string_pb_28r2_4839_psr,                FALSE }, /* BC4-ROM A07 */
    { 4841, bootstrap_patch_string_pb_27r26_4841_psr,               FALSE }, /* BC6-ROM A04 */
    { 6311, bootstrap_patch_string_pb_74r3_6311_psr,                FALSE }, /* BC6-ROM A05 */
    { 5985, bootstrap_patch_string_pb_51r22_5985_csp0p5_psr,        TRUE  }, /* BC7820 A12, CSP-0.5 */
    { 6247, bootstrap_patch_string_pb_53r11_6247_csp_psr,           TRUE  }, /* BC7820 A16/BC7830 A16, CSP */
    { 5892, bootstrap_patch_string_pb_49r3_5892_psr,                TRUE  }, /* BC7830 A14 */
    { 6817, bootstrap_patch_string_pb_90r1_6817_psr,                TRUE  }, /* CSR8810 A04 */
    { 7557, bootstrap_patch_string_pb_101r12_7558_csr8311_uart_psr, TRUE  }, /* CSR8811 - LPC Final Release Candidate */
    { 7558, bootstrap_patch_string_pb_101r12_7558_csr8311_uart_psr, TRUE  }, /* CSR8311 A06 ROM */
    { 7727, bootstrap_patch_string_ps_gemini_a06_psr,               TRUE  }, /* CSR8811 [LPC pre-A08] */
    { 7837, bootstrap_patch_string_ps_gemini_a06_psr,               TRUE  }, /* CSR8505 with LPC [LPC pre-A08] */
    { 8211, bootstrap_patch_string_ps_gemini_a06_psr,               TRUE  }, /* CSR8505 with LPC [LPC pre-A08 RC7] */
    { 8231, bootstrap_patch_string_ps_gemini_a06_psr,               TRUE  }, /* CSR8505 with LPC [LPC pre-A08 RC9] */
    { 8241, bootstrap_patch_string_pb_109r16_8241_csr8311_psr,      TRUE  }, /* CSR8311 Gemini A08 */
    { 8833, bootstrap_patch_string_pb_119r3_8833_psr,               TRUE  }, /* CSR8820 BGA */
    { 8129, bootstrap_patch_string_ps_gemini_a06_psr,               TRUE  }, /* CSR8505 with LPC B-98367 */
    { 7959, bootstrap_patch_string_ps_purdey_7959_psr,              TRUE  }, /* Gemini A06, LPC - Release candidate 4 */
    { 9079, bootstrap_patch_string_pb_132r10_9079_csr8350_psr,      TRUE  }, /* Dale Auto A07 (CSR8350) */
    { 8891, bootstrap_patch_string_pb_120r16_8891_csr8510_psr,      TRUE  }, /* Gemini A10 (CSR8510) */
    { 10520, bootstrap_patch_string_pb_207r7_10520_csr8311_psr,     TRUE  }, /* Gemini A12 (CSR8311) */
    { 10391, bootstrap_patch_string_pb_206r4_10391_csr8350_psr,     TRUE  }, /* Dale Auto A08 (CSR8350) */
}; 

static const csrBtBootstrapIdPatchTable csrBtBootstrapUsbTable[] =
{
    { 7558, bootstrap_patch_string_pb_101r12_7558_csr8311_usb_psr, TRUE  },  /* CSR8311 A06 ROM */
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
    const CsrCharString *patch = NULL;
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
                CsrBtBootstrapParsePsrConstant(bootContext, patch);
            }
        }
    }

#if defined(CSR_AMP_ENABLE)
#if defined(CSR_WIFI_DESIGN_M2107_R02) || defined(CSR_WIFI_DESIGN_M2107_R03)
    CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_jem_a12_psr);
    CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_m2107_coex_psr);
#endif
#if defined(CSR_WIFI_DESIGN_M2399_R03)
    CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_m2399_coex_psr);
#endif
#if defined(CSR_WIFI_DESIGN_M2501_R03)
    CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_m2501_coex_psr);
#endif
#endif /* CSR_AMP_ENABLE */

    /* Load default platform settings */
#ifdef CSR_USE_BLUECORE_CHIP
#if defined(CSR_BT_INTEROP_TEST_BUILD)
    CsrBtBootstrapParsePsrConstant(bootContext,
                                   bootstrap_patch_string_ps_interop_test_psr);
#else
    CsrBtBootstrapParsePsrConstant(bootContext,
                                   bootstrap_patch_string_ps_default_psr);
#endif
#endif
    if (inst->iface == CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H2_USB)
    {
        CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_host_usb_psr);
    }
    else
    {
        CsrBtBootstrapParsePsrConstant(bootContext, bootstrap_patch_string_ps_host_bcsp_psr);
    }

}
