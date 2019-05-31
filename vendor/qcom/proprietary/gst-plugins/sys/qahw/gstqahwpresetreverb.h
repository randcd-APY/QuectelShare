/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_PRESET_REVERB_H__
#define __GST_QAHW_PRESET_REVERB_H__

#include <qahw_effect_presetreverb.h>
#include "gstqahweffect.h"

G_BEGIN_DECLS

#define GST_TYPE_QAHW_PRESET_REVERB gst_qahw_preset_reverb_get_type ()
G_DECLARE_FINAL_TYPE (GstQahwPresetReverb, gst_qahw_preset_reverb, GST, QAHW_PRESET_REVERB, GstQahwEffect);

G_END_DECLS

#endif /* __GST_QAHW_PRESET_REVERB_H__ */
