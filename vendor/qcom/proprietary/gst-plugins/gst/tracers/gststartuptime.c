/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gststartuptime.h"

GST_DEBUG_CATEGORY_STATIC (gst_startup_time_debug);
#define GST_CAT_DEFAULT gst_startup_time_debug

#define _do_init \
    GST_DEBUG_CATEGORY_INIT (gst_startup_time_debug, "startuptime", 0, "Pipeline startup time tracer");
#define gst_startup_time_tracer_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstStartupTimeTracer, gst_startup_time_tracer,
    GST_TYPE_TRACER, _do_init);

static GstTracerRecord *tr_startup_time;

static void
do_element_change_state_pre (GstTracer * tracer, guint64 ts, GstElement * elem,
    GstStateChange change)
{
  GstStartupTimeTracer *self = GST_STARTUP_TIME_TRACER_CAST (tracer);

  if (!GST_IS_PIPELINE (elem) || self->start_time != GST_CLOCK_TIME_NONE
      || change != GST_STATE_CHANGE_NULL_TO_READY)
    return;

  GST_TRACE_OBJECT (tracer, "pipeline changed state NULL to READY, startup "
      "tracer starting point : %" GST_TIME_FORMAT, GST_TIME_ARGS (ts));
  self->start_time = ts;
}

static void
do_element_change_state_post (GstTracer * tracer, guint64 ts, GstElement * elem,
    GstStateChange change, GstStateChangeReturn res)
{
  GstStartupTimeTracer *self = GST_STARTUP_TIME_TRACER_CAST (tracer);
  GstClockTimeDiff elapsed_time;

  if (!GST_IS_PIPELINE (elem) || self->start_time == GST_CLOCK_TIME_NONE)
    return;

  if (change == GST_STATE_CHANGE_READY_TO_NULL) {
    self->start_time = GST_CLOCK_TIME_NONE;
    return;
  }

  if (change != GST_STATE_CHANGE_PAUSED_TO_PLAYING)
    return;

  if (res == GST_STATE_CHANGE_ASYNC) {
    GST_TRACE_OBJECT (tracer,
        "state is changing asynchronously, wait for async done");
    return;
  }

  if (res != GST_STATE_CHANGE_SUCCESS) {
    self->start_time = GST_CLOCK_TIME_NONE;
    g_warning ("The result of state change is %s",
        gst_element_state_change_return_get_name (res));
    return;
  }

  /* In the first pipeline generation, the time is measured taking into account
   * the time taken from gst_init as cold startup time. */
  if (self->initial_launch) {
    elapsed_time = ts;
  } else {
    elapsed_time = ts - self->start_time;
  }

  gst_tracer_record_log (tr_startup_time, elapsed_time, self->initial_launch);
  GST_TRACE_OBJECT (tracer,
      "pipeline changed to PLAYING state, elapsed time : %" GST_TIME_FORMAT
      " %s", GST_TIME_ARGS (elapsed_time),
      (self->initial_launch ? "(the time calculated from gst init)" : ""));
  self->initial_launch = FALSE;
}

static void
do_element_new (GstTracer * tracer, guint64 ts, GstElement * elem)
{
  GstStartupTimeTracer *self = GST_STARTUP_TIME_TRACER_CAST (tracer);

  if (!GST_IS_PIPELINE (elem))
    return;

  GST_TRACE_OBJECT (tracer,
      "pipeline created, startup tracer starting point : %" GST_TIME_FORMAT,
      GST_TIME_ARGS (ts));
  self->start_time = ts;
}

static void
gst_startup_time_tracer_class_init (GstStartupTimeTracerClass * klass)
{
  /* announce trace formats */
  /* *INDENT-OFF* */
  tr_startup_time = gst_tracer_record_new ("startup-time.class",
      "elapsed-time", GST_TYPE_STRUCTURE, gst_structure_new ("value",
          "type", G_TYPE_GTYPE, GST_TYPE_CLOCK_TIME,
          "description", G_TYPE_STRING, "The time took to prepare the pipeline",
          NULL),
      "initial-launch", GST_TYPE_STRUCTURE, gst_structure_new ("value",
          "type", G_TYPE_GTYPE, G_TYPE_BOOLEAN,
          "description", G_TYPE_STRING, "The value representing the first pipeline, "
          "It's included up to gst_init time.",
          NULL),
      NULL);
  /* *INDENT-ON* */
}

static void
gst_startup_time_tracer_init (GstStartupTimeTracer * self)
{
  GstTracer *tracer = GST_TRACER (self);

  self->initial_launch = TRUE;
  self->start_time = GST_CLOCK_TIME_NONE;

  gst_tracing_register_hook (tracer, "element-change-state-pre",
      G_CALLBACK (do_element_change_state_pre));
  gst_tracing_register_hook (tracer, "element-change-state-post",
      G_CALLBACK (do_element_change_state_post));
  gst_tracing_register_hook (tracer, "element-new",
      G_CALLBACK (do_element_new));
}
