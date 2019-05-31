/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_EQUALIZER_H__
#define __GST_QAHW_EQUALIZER_H__

#include <qahw_effect_equalizer.h>
#include "gstqahweffect.h"

G_BEGIN_DECLS

#define GST_TYPE_QAHW_EQUALIZER gst_qahw_equalizer_get_type ()
G_DECLARE_FINAL_TYPE (GstQahwEqualizer, gst_qahw_equalizer, GST, QAHW_EQUALIZER, GstQahwEffect);

G_END_DECLS

#endif /* __GST_QAHW_EQUALIZER_H__ */
