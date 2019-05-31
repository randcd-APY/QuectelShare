/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __GST_QAHW_EFFECT_H__
#define __GST_QAHW_EFFECT_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

#include <qahw_effect_api.h>
#include <qahw_effect_equalizer.h>

G_BEGIN_DECLS

#define GST_TYPE_QAHW_EFFECT            (gst_qahw_effect_get_type())
#define GST_QAHW_EFFECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_QAHW_EFFECT,GstQahwEffect))
#define GST_QAHW_EFFECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_QAHW_EFFECT,GstQahwEffectClass))
#define GST_IS_QAHW_EFFECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_QAHW_EFFECT))
#define GST_IS_QAHW_EFFECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_QAHW_EFFECT))
#define GST_QAHW_EFFECT_CAST(obj)       ((GstQahwEffect *) (obj))
#define GST_QAHW_EFFECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_QAHW_EFFECT, GstQahwEffectClass))

typedef struct _GstQahwEffect GstQahwEffect;
typedef struct _GstQahwEffectClass GstQahwEffectClass;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GstQahwEffect, gst_object_unref)


/**
 * GstQahwEffect:
 *
 * Opaque data structure
 */
struct _GstQahwEffect {
  GstBaseTransform parent;
  qahw_effect_lib_handle_t lib_handle;
  qahw_effect_handle_t     effect_handle;
  qahw_effect_descriptor_t effect_desc;
  gboolean enabled;
};

struct _GstQahwEffectClass {
  GstBaseTransformClass parent_class;

  gboolean (*setup) (GstQahwEffect * effect);

  /* Use to implement extra steps, like for Audio Sphere */
  gboolean (*enable) (GstQahwEffect * effect);
  gboolean (*disable) (GstQahwEffect * effect);

  const gchar * library;
  const qahw_effect_uuid_t *uuid;
};

GType gst_qahw_effect_get_type(void);

gboolean gst_qahw_effect_get_param (GstQahwEffect * self, gint param_id,
    gsize psize, gpointer param, gsize vsize, gpointer value);

gboolean gst_qahw_effect_set_param (GstQahwEffect * self, gint param_id,
    gsize psize, gpointer param, gsize vsize, gpointer value);

G_END_DECLS

#endif /* __GST_QAHW_EFFECT_H__ */
