/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwvirtualizer
 * @see_also: qahwvirtualizer
 *
 * This filters enables DSP virtualizer effect.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v filesrc location=/path/to/audio.flac ! flacparse ! qahwvirtualizer ! qahwsink
 * ]| Play a FLAC file applying virtualizer effect to the stream.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

#include "gstqahwvirtualizer.h"
#include "gstqahw-common.h"

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

enum
{
  PROP_0,
  PROP_STRENGTH,
  PROP_LAST
};

struct _GstQahwVirtualizer
{
  GstQahwEffect parent;
  gint strength;
};

#define gst_qahw_virtualizer_parent_class parent_class
G_DEFINE_TYPE (GstQahwVirtualizer, gst_qahw_virtualizer, GST_TYPE_QAHW_EFFECT);

static gboolean
gst_qahw_virtualizer_set_strength (GstQahwVirtualizer * self, gint16 strength)
{
  GstQahwEffect *effect = GST_QAHW_EFFECT (self);

  GST_DEBUG_OBJECT (self, "Setting strength to %i", strength);

  if (gst_qahw_effect_set_param (effect, VIRTUALIZER_PARAM_STRENGTH, 0, NULL,
          sizeof (gint16), &strength)) {
    if (self->strength != strength) {
      g_object_notify (G_OBJECT (self), "strength");
      self->strength = strength;
    }
  }

  return TRUE;
}

static gboolean
gst_qahw_virtualizer_setup (GstQahwEffect * effect)
{
  GstQahwVirtualizer *self = GST_QAHW_VIRTUALIZER (effect);
  gst_qahw_virtualizer_set_strength (self, self->strength);
  return TRUE;
}

static void
gst_qahw_virtualizer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwVirtualizer *self = GST_QAHW_VIRTUALIZER (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_STRENGTH:
      gst_qahw_virtualizer_set_strength (self, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_virtualizer_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwVirtualizer *self = GST_QAHW_VIRTUALIZER (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_STRENGTH:
      g_value_set_int (value, self->strength);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_virtualizer_init (GstQahwVirtualizer * self)
{
  /* nothing to do */
}

static void
gst_qahw_virtualizer_class_init (GstQahwVirtualizerClass * klass)
{
  GstElementClass *gstelement_class;
  GObjectClass *gobject_class;
  GstQahwEffectClass *effect_class;

  gstelement_class = GST_ELEMENT_CLASS (klass);
  effect_class = GST_QAHW_EFFECT_CLASS (klass);
  gobject_class = G_OBJECT_CLASS (klass);

  effect_class->library = QAHW_EFFECT_VIRTUALIZER_LIBRARY;
  effect_class->uuid = SL_IID_VIRTUALIZER_UUID;
  effect_class->setup = gst_qahw_virtualizer_setup;

  gobject_class->get_property = gst_qahw_virtualizer_get_property;
  gobject_class->set_property = gst_qahw_virtualizer_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio Virtualizer Effect (QAHW)", "Filter/Effect/Audio",
      "Apply virtualizer effect using QAHW",
      "Nicolas Dufresne <nicolas.dufresne@collabora.com>");

  g_object_class_install_property (gobject_class, PROP_STRENGTH,
      g_param_spec_int ("strength", "Strength",
          "The strength of this effect.", 0, 1000, 100,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));
}
