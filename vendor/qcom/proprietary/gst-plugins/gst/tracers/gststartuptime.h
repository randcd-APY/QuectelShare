/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GST_STARTUP_TIME_TRACER_H__
#define __GST_STARTUP_TIME_TRACER_H__

#include <gst/gst.h>
#include <gst/gsttracer.h>

G_BEGIN_DECLS

#define GST_TYPE_STARTUP_TIME_TRACER \
  (gst_startup_time_tracer_get_type())
#define GST_STARTUP_TIME_TRACER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_STARTUP_TIME_TRACER,GstStartupTimeTracer))
#define GST_STARTUP_TIME_TRACER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_STARTUP_TIME_TRACER,GstStartupTimeTracerClass))
#define GST_IS_STARTUP_TIME_TRACER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_STARTUP_TIME_TRACER))
#define GST_IS_STARTUP_TIME_TRACER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_STARTUP_TIME_TRACER))
#define GST_STARTUP_TIME_TRACER_CAST(obj) ((GstStartupTimeTracer *)(obj))

typedef struct _GstStartupTimeTracer GstStartupTimeTracer;
typedef struct _GstStartupTimeTracerClass GstStartupTimeTracerClass;

/**
 * GstStartupTimeTracer:
 *
 * Opaque #GstStartupTimeTracer data structure
 */
struct _GstStartupTimeTracer {
  GstTracer parent;

  gboolean initial_launch;
  GstClockTime start_time;
};

struct _GstStartupTimeTracerClass {
  GstTracerClass parent_class;
};

G_GNUC_INTERNAL GType gst_startup_time_tracer_get_type (void);

G_END_DECLS

#endif /* __GST_STARTUP_TIME_TRACER_H__ */