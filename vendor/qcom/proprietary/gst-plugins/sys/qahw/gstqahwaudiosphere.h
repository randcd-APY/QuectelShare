/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_AUDIOSPHERE_H__
#define __GST_QAHW_AUDIOSPHERE_H__

#include <qahw_effect_audiosphere.h>
#include "gstqahweffect.h"

G_BEGIN_DECLS

#define GST_TYPE_QAHW_AUDIOSPHERE gst_qahw_audiosphere_get_type ()
G_DECLARE_FINAL_TYPE (GstQahwAudiosphere, gst_qahw_audiosphere, GST, QAHW_AUDIOSPHERE, GstQahwEffect);

G_END_DECLS

#endif /* __GST_QAHW_AUDIOSPHERE_H__ */
