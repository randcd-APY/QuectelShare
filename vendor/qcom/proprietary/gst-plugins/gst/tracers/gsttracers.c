/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/gst.h>
#include "gststartuptime.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_tracer_register (plugin, "startuptime",
          gst_startup_time_tracer_get_type ()))
    return FALSE;
  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, customtracers,
    "GStreamer QTI custom tracers", plugin_init, VERSION,
    GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
