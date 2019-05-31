/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwpresetreverb
 * @see_also: qahwpresetreverb
 *
 * This filters enables DSP reverb effect.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v filesrc location=/path/to/audio.flac ! flacparse ! qahwpresetreverb ! qahwsink
 * ]| Play a FLAC file applying preset reverb effect to the stream.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

#include "gstqahwpresetreverb.h"
#include "gstqahw-common.h"

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

enum
{
  PROP_0,
  PROP_PRESET,
  PROP_LAST
};

struct _GstQahwPresetReverb
{
  GstQahwEffect parent;
  gint cur_preset;
};

#define gst_qahw_preset_reverb_parent_class parent_class
G_DEFINE_TYPE (GstQahwPresetReverb, gst_qahw_preset_reverb,
    GST_TYPE_QAHW_EFFECT);

static GType
gst_qahw_preset_reverb_preset_get_type (void)
{
  static volatile gsize preset_type = 0;
  static const GEnumValue preset[] = {
    {0, "None", "none"},
    {1, "Small Room", "small-room"},
    {2, "Medium Room", "medium-room"},
    {3, "Large Room", "large-room"},
    {4, "Medium Hall", "media-hall"},
    {5, "Large Hall", "large-hall"},
    {6, "plate", "plate"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&preset_type)) {
    GType tmp = g_enum_register_static ("GstQahwPresetReverbPreset", preset);
    g_once_init_leave (&preset_type, tmp);
  }

  return (GType) preset_type;
}

static gboolean
gst_qahw_preset_reverb_set_cur_preset (GstQahwPresetReverb * self,
    gint16 cur_preset)
{
  GstQahwEffect *effect = GST_QAHW_EFFECT (self);

  GST_DEBUG_OBJECT (self, "Setting current preset to %i", cur_preset);

  if (gst_qahw_effect_set_param (effect, REVERB_PARAM_PRESET, 0, NULL,
          sizeof (gint16), &cur_preset)) {
    if (self->cur_preset != cur_preset) {
      g_object_notify (G_OBJECT (self), "preset");
      self->cur_preset = cur_preset;
    }
  }

  return TRUE;
}

static gboolean
gst_qahw_preset_reverb_setup (GstQahwEffect * effect)
{
  GstQahwPresetReverb *self = GST_QAHW_PRESET_REVERB (effect);
  gst_qahw_preset_reverb_set_cur_preset (self, self->cur_preset);
  return TRUE;
}

static void
gst_qahw_preset_reverb_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwPresetReverb *self = GST_QAHW_PRESET_REVERB (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_PRESET:
      gst_qahw_preset_reverb_set_cur_preset (self, g_value_get_enum (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_preset_reverb_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwPresetReverb *self = GST_QAHW_PRESET_REVERB (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_PRESET:
      g_value_set_enum (value, self->cur_preset);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_preset_reverb_init (GstQahwPresetReverb * self)
{
  /* nothing to do */
}

static void
gst_qahw_preset_reverb_class_init (GstQahwPresetReverbClass * klass)
{
  GstElementClass *gstelement_class;
  GObjectClass *gobject_class;
  GstQahwEffectClass *effect_class;

  gstelement_class = GST_ELEMENT_CLASS (klass);
  effect_class = GST_QAHW_EFFECT_CLASS (klass);
  gobject_class = G_OBJECT_CLASS (klass);

  effect_class->library = QAHW_EFFECT_PRESET_REVERB_LIBRARY;
  effect_class->uuid = SL_IID_INS_PRESETREVERB_UUID;
  effect_class->setup = gst_qahw_preset_reverb_setup;

  gobject_class->get_property = gst_qahw_preset_reverb_get_property;
  gobject_class->set_property = gst_qahw_preset_reverb_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio Preset Reverb Effect (QAHW)", "Filter/Effect/Audio",
      "Apply preet reverb effect using QAHW",
      "Nicolas Dufresne <nicolas.dufresne@collabora.com>");

  g_object_class_install_property (gobject_class, PROP_PRESET,
      g_param_spec_enum ("preset", "Preset",
          "The currently selected preset.",
          gst_qahw_preset_reverb_preset_get_type (), 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));
}
