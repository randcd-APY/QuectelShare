#ifndef _CSR_BT_BOOT_GENERIC_H_
#define _CSR_BT_BOOT_GENERIC_H_

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009-2012 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
*****************************************************************************/

/* IMPORTANT NOTICE: This file contains generic functions for the CSR
 * Synegy BlueCore bootstrap system. You should NOT touch anything
 * here. Instead use the 'csr_bt_boot_specific.c/h' files to set up
 * the bootstrap for your particular system */

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_tm_bluecore_bootstrap.h"
#include "csr_transport.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Transport values */
#define CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_BCSP   ((CsrUint16)0x0001)
#define CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H2_USB ((CsrUint16)0x0002)
#define CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H4     ((CsrUint16)0x0003)
#define CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_H4DS   ((CsrUint16)0x0007)
#define CSR_BT_BOOTSTRAP_PSKEY_HOST_INTERFACE_SDIO   ((CsrUint16)0x0009)

#ifdef CSR_USE_QCA_CHIP
/** Basic type for an NVM element */
typedef CsrUint8 QsocBtCfgElementType;

typedef struct csrRomePatchNvmMapTag 
{
    CsrUint16                           socVer;
    CsrUint16                           romeBuildVer;
    CsrUint8                            chipsetVer[2];
    CsrCharString                      *ram_patch_file_name;
    CsrCharString                      *nvm_file_name;
} csrRomePatchNvmMap;

typedef struct
{
    CsrUint16                           rom_version;
    CsrUint16                           build_version;
} patch_version;

typedef struct
{
    CsrUint32                           tlv_data_len;
    CsrUint32                           tlv_patch_data_len;
    CsrUint8                            sign_ver;
    CsrUint8                            sign_algorithm;
    CsrUint8                            dwnd_cfg;
    CsrUint8                            reserved1;
    CsrUint16                           prod_id;
    CsrUint16                           build_ver;
    CsrUint16                           patch_ver;
    CsrUint16                           reserved2;
    CsrUint32                           patch_entry_addr;
} tlv_patch_hdr;

typedef struct
{
    CsrUint16                           tag_id;
    CsrUint16                           tag_len;
    CsrUint32                           tag_ptr;
    CsrUint32                           tag_ex_flag;
} tlv_nvm_hdr;

typedef struct
{
    CsrUint8                            tlv_type;
    CsrUint8                            tlv_length1;
    CsrUint8                            tlv_length2;
    CsrUint8                            tlv_length3;
    
    union
    {
        tlv_patch_hdr                   patch;
        tlv_nvm_hdr                     nvm;
    }tlv;
} tlv_patch_info;
#endif /* CSR_USE_QCA_CHIP */

/* Command list */
typedef struct csrBtBootstrapCmdTag
{
    struct csrBtBootstrapCmdTag        *next;   /* Next key */
    CsrUint16                           key;    /* Numeric PS Key identifier */
    CsrUint16                          *data;   /* Data of the PS Key to be set */
    CsrUint16                           length; /* length of data in CsrUint16s */
} csrBtBoostrapCmd;

/* Internal bootstrap instance */
typedef struct
{
    csrBtBoostrapCmd                   *ps_head;
    csrBtBoostrapCmd                   *ps_tail;
    CsrBool                             bc7up;
    CsrUint32                           bitRate;
    CsrUint16                           freq;
    CsrUint16                           freqTrim;
    CsrBtDeviceAddr                     address;
    CsrBtDeviceName                     name;
    CsrUint16                           iface;
} csrBtBootstrapInst;

#ifdef CSR_USE_QCA_CHIP
/* QC chip Specific bootstrap helper function to set the Firmware 
   logging over HCI transport */
void CsrBtBootstrapSetQcFirmwareLog(CsrBool logen);
#endif
/* Application developer implemented function. This file is to be
 * implemented by the application developer. An example can be found
 * in the csr_bt_boot_specific.c file.  The application must set the
 * 'bc7up' variable to indicate whether the chip is a BC7 or newer in
 * order to set the host interface bitrate correctly */
extern void CsrBtBootstrapSpecific(void *bootContext, CsrUint16 buildId,
                                   CsrBool *bc7up); /* is chip BC7 or newer? */

/* Specific bootstrap helper function: Set the host transport
 * interface. Use the framework TRANSPORT_TYPE defines */
void CsrBtBootstrapSetInterface(void *bootContext, CsrUint16 hostInterface);

/* Specific bootstrap helper function: Set the host interface bitrate
 * (for use with UART host transports like BCSP, H4, H4DS etc) */
void CsrBtBootstrapSetBitRate(void *bootContext, CsrUint32 bitRate);

/* Specific bootstrap helper function: The the Bluetooth friendly
 * name */
void CsrBtBootstrapSetFriendlyName(void *bootContext, CsrUtf8String *name);

/* Specific bootstrap helper function: Set the BlueCore Bluetooth
 * address */
void CsrBtBootstrapSetLocalBtAddress(void *bootContext,
                                     CsrBtDeviceAddr *address);

/* Specific bootstrap helper function: Set the crystal frequency */
void CsrBtBootstrapSetFrequency(void *bootContext, CsrUint16 frequency);

/* Specific bootstrap helper function: Set the crystal frequency
 * trim */
void CsrBtBootstrapSetFreqTrim(void *bootContext, CsrUint16 freqTrim);

/* Parse a PSR file and apply the settings from the file */
void CsrBtBootstrapParsePsrFile(void *bootContext,
                                const CsrCharString *filename);

/* Parse a compiled-in constant C string and apply the settings */
void CsrBtBootstrapParsePsrConstant(void *bootContext,
                                    const CsrCharString *string);

/* Add a raw PS-key */
void CsrBtBootstrapAddPskey(void *bootContext, CsrUint16 key, CsrUint16 *data,
                            CsrUint16 length);

#ifdef __cplusplus
}
#endif

#endif
