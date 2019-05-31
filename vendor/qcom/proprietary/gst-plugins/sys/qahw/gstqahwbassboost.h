/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_BASSBOOST_H__
#define __GST_QAHW_BASSBOOST_H__

#include <qahw_effect_bassboost.h>
#include "gstqahweffect.h"

G_BEGIN_DECLS

#define GST_TYPE_QAHW_BASSBOOST gst_qahw_bassboost_get_type ()
G_DECLARE_FINAL_TYPE (GstQahwBassboost, gst_qahw_bassboost, GST, QAHW_BASSBOOST, GstQahwEffect);

G_END_DECLS

#endif /* __GST_QAHW_BASSBOOST_H__ */
