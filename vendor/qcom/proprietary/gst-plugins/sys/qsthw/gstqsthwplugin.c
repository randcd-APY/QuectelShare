/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstqsthwstrig.h"

GST_DEBUG_CATEGORY (qsthw_debug);

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "qsthwstrig", GST_RANK_NONE,
          GST_TYPE_QSTHW_SOUND_TRIGGER))
    return FALSE;

  GST_DEBUG_CATEGORY_INIT (qsthw_debug, "qsthw", 0,
      "sound trigger plugin");

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    qsthw,
    "Qualcom sound trigger plugin",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
