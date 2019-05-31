/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_VIRTUALIZER_H__
#define __GST_QAHW_VIRTUALIZER_H__

#include <qahw_effect_virtualizer.h>
#include "gstqahweffect.h"

G_BEGIN_DECLS

#define GST_TYPE_QAHW_VIRTUALIZER gst_qahw_virtualizer_get_type ()
G_DECLARE_FINAL_TYPE (GstQahwVirtualizer, gst_qahw_virtualizer, GST, QAHW_VIRTUALIZER, GstQahwEffect);

G_END_DECLS

#endif /* __GST_QAHW_VIRTUALIZER_H__ */
