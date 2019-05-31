#ifndef _CSR_BT_BOOT_SPECIFIC_H_
#define _CSR_BT_BOOT_SPECIFIC_H_

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

/* IMPORTANT NOTICE: This file sets up basic and constant settings for
 * the CSR Synergy BlueCore bootstrap system.  You may need to tweak
 * the settings in this file to match your platform */

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Automatic bootstrap system
 *
 * Bootstrap can either happen from PSR files or from a constant
 * in-memory C strings containing the PSR values. You can use either,
 * but only ONE "patch" and ONE "platform" setting can be used.
 *
 * The default CSR approach is to use neither and instead rely on the
 * reported firmware build-id. This allows us to work with *all* chips
 * without requiring any changes to the bootstrap system. This is
 * probably NOT what you want to do in your real world application.
 *
 * In any case PSR, files and strings all have the same
 * syntax. Comments start with "#" or "/" and are ignored. Valid
 * entryes have the format "& dddd = dddd dddd ..."
 * Where 'd' is any hexadecimal ASCII character (0-9, a-f, A-F).
 * Whitespace may appear anywhere. The next entry starts after a line
 * break (\n) and/or carriage return (\r)
 ****************************************************************************/

/* Chip specific PSR patch file */
/*#define CSR_BT_BOOTSTRAP_PATCH_FILE           "../bootstrap/pb-27_r21.psr"*/

/* Platform/application specific PSR file */
/*#define CSR_BT_BOOTSTRAP_PLATFORM_FILE        "../bootstrap/ps-4841.psr"*/

/* Chip specific PSR patch constant string */
/*
extern const CsrCharString bootstrap_patch_string[];
#define CSR_BT_BOOTSTRAP_PATCH_STRING         (NULL)
*/

/* Platform/application specific PSR constant string */
/*
extern const CsrCharString bootstrap_platform_string[];
#define CSR_BT_BOOTSTRAP_PLATFORM_STRING      (NULL)
*/

#ifdef __cplusplus
}
#endif

#endif
