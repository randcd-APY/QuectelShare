/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstqahwsink.h"
#include "gstqahwsrc.h"
#include "gstqahwequalizer.h"
#include "gstqahwaudiosphere.h"
#include "gstqahwbassboost.h"
#include "gstqahwpresetreverb.h"
#include "gstqahwvirtualizer.h"

GST_DEBUG_CATEGORY (qahw_debug);

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "qahwsrc", GST_RANK_PRIMARY,
          GST_TYPE_QAHW_SRC))
    return FALSE;

  if (!gst_element_register (plugin, "qahwsink", GST_RANK_PRIMARY,
          GST_TYPE_QAHW_SINK))
    return FALSE;

  if (!gst_element_register (plugin, "qahwequalizer", GST_RANK_NONE,
          GST_TYPE_QAHW_EQUALIZER))
    return FALSE;

  if (!gst_element_register (plugin, "qahwaudiosphere", GST_RANK_NONE,
          GST_TYPE_QAHW_AUDIOSPHERE))
    return FALSE;

  if (!gst_element_register (plugin, "qahwbassboost", GST_RANK_NONE,
          GST_TYPE_QAHW_BASSBOOST))
    return FALSE;

  if (!gst_element_register (plugin, "qahwvirtualizer", GST_RANK_NONE,
          GST_TYPE_QAHW_VIRTUALIZER))
    return FALSE;

  if (!gst_element_register (plugin, "qahwpresetreverb", GST_RANK_NONE,
          GST_TYPE_QAHW_PRESET_REVERB))
    return FALSE;

  GST_DEBUG_CATEGORY_INIT (qahw_debug, "qahw", 0, "HAL audio plugins");

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    qahw,
    "QTI HAL audio plugins",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
