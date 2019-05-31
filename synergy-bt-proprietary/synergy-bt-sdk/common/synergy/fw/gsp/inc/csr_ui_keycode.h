#ifndef CSR_UI_KEYCODE_H__
#define CSR_UI_KEYCODE_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Input keys */
#define CSR_UI_KEY_N0         0x8000
#define CSR_UI_KEY_N1         0x8001
#define CSR_UI_KEY_N2         0x8002
#define CSR_UI_KEY_N3         0x8003
#define CSR_UI_KEY_N4         0x8004
#define CSR_UI_KEY_N5         0x8005
#define CSR_UI_KEY_N6         0x8006
#define CSR_UI_KEY_N7         0x8007
#define CSR_UI_KEY_N8         0x8008
#define CSR_UI_KEY_N9         0x8009
#define CSR_UI_KEY_STAR       0x800A
#define CSR_UI_KEY_HASH       0x800B

/* Control keys */
#define CSR_UI_KEY_DEL        0x800C
#define CSR_UI_KEY_BACK       0x800D
#define CSR_UI_KEY_SK1        0x800E
#define CSR_UI_KEY_SK2        0x800F
#define CSR_UI_KEY_LEFT       0x8010
#define CSR_UI_KEY_RIGHT      0x8020
#define CSR_UI_KEY_UP         0x8040
#define CSR_UI_KEY_DOWN       0x8080

/* ASCII keys (valid range for code parameter: 0x20-0x7E) */
#define CSR_UI_KEY_ASCII(code) ((CsrUint16) (0x8100 | ((code) & 0x7F)))

/* Key maps */
#define CSR_UI_KEYMAP_NUMERIC         0
#define CSR_UI_KEYMAP_CONTROLNUMERIC  1
#define CSR_UI_KEYMAP_ALPHANUMERIC    2

#ifdef __cplusplus
}
#endif

#endif
