/*============================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================
 *                      INCLUDE FILES
 *===========================================================================*/
#include "chromatix_cpp.h"

static chromatix_cpp_type chromatix_imx477_parms = {
#ifdef ENABLED_IMX477_RAW10_MODES
#include "chromatix_imx477_raw10_hfr_60_cpp.h"
#else
#include "chromatix_imx477_raw12_hfr_60_cpp.h"
#endif
};

/*============================================================================
 * FUNCTION    - load_chromatix -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *load_chromatix(void)
{
  return &chromatix_imx477_parms;
}
