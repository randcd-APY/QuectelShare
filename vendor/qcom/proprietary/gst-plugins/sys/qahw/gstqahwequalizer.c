/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwequalizer
 * @see_also: qahwequalizer
 *
 * This filters enables DSP equalizer effect.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v filesrc location=/path/to/audio.flac ! flacparse ! qahwequalizer ! qahwsink
 * ]| Play a FLAC file applying equalizer effect to the stream.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

#include "gstqahwequalizer.h"
#include "gstqahw-common.h"

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

#define MIN_LEVEL -1500
#define MAX_LEVEL 1500
#define NUM_BANDS 5

enum
{
  PROP_0,
  PROP_PRESET,
  PROP_BAND_LEVELS,
  PROP_LAST
};

struct _GstQahwEqualizer
{
  GstQahwEffect parent;
  gint cur_preset;
  GstStructure *band_levels;
};

#define gst_qahw_equalizer_parent_class parent_class
G_DEFINE_TYPE (GstQahwEqualizer, gst_qahw_equalizer, GST_TYPE_QAHW_EFFECT);

static GType
gst_qahw_equalizer_preset_get_type (void)
{
  static volatile gsize preset_type = 0;
  static const GEnumValue preset[] = {
    {0, "Normal", "normal"},
    {1, "Classical", "classical"},
    {2, "Dance", "dance"},
    {3, "Flat", "flat"},
    {4, "Folk", "folk"},
    {5, "Heavy Metal", "heavy-metal"},
    {6, "Hiphop", "hiphop"},
    {7, "Jazz", "jazz"},
    {8, "Pop", "pop"},
    {9, "Rock", "rock"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&preset_type)) {
    GType tmp = g_enum_register_static ("GstQahwEqualizerPreset", preset);
    g_once_init_leave (&preset_type, tmp);
  }

  return (GType) preset_type;
}

static gboolean
gst_qahw_equalizer_set_cur_preset (GstQahwEqualizer * self, gint16 cur_preset)
{
  GstQahwEffect *effect = GST_QAHW_EFFECT (self);

  GST_DEBUG_OBJECT (self, "Setting current preset to %i", cur_preset);

  if (gst_qahw_effect_set_param (effect, EQ_PARAM_CUR_PRESET, 0, NULL,
          sizeof (gint16), &cur_preset)) {
    if (self->cur_preset != cur_preset) {
      g_object_notify (G_OBJECT (self), "preset");
      self->cur_preset = cur_preset;
    }
  }

  return TRUE;
}

static gboolean
gst_qahw_equalizer_set_band_levels (GstQahwEqualizer * self,
    const GstStructure * band_levels)
{
  GstQahwEffect *effect = GST_QAHW_EFFECT (self);
  const GValue *value;
  gint32 band;

  if (gst_structure_n_fields (band_levels) == 0)
    return TRUE;

  value = gst_structure_get_value (band_levels, "levels");
  if (!value) {
    g_warning ("Missing \"levels\" field in band-levels structure.");
    return FALSE;
  }

  for (band = 0; band < NUM_BANDS; band++) {
    const GValue *vlevel = gst_value_list_get_value (value, band);
    gint16 level;

    if (!vlevel) {
      g_warning ("There should be exactly %i bands set as \"levels\" field.",
          NUM_BANDS);
      return FALSE;
    }

    level = g_value_get_int (vlevel);

    if ((level < MIN_LEVEL) || (level > MAX_LEVEL)) {
      g_warning ("level must between %i and %i", MIN_LEVEL, MAX_LEVEL);
      return FALSE;
      break;
    }

    if (!gst_qahw_effect_set_param (effect, EQ_PARAM_BAND_LEVEL,
            sizeof (gint32), &band, sizeof (gint16), &level)) {
      return FALSE;
    }
  }

  if (!gst_structure_is_equal (self->band_levels, band_levels)) {
    gst_structure_free (self->band_levels);
    self->band_levels = gst_structure_copy (band_levels);
    g_object_notify (G_OBJECT (self), "band-levels");
  }

  return TRUE;
}

static gboolean
gst_qahw_equalizer_setup (GstQahwEffect * effect)
{
  GstQahwEqualizer *self = GST_QAHW_EQUALIZER (effect);

  GST_DEBUG_OBJECT (self, "Stream is ready, setting up properties.");

  gst_qahw_equalizer_set_cur_preset (self, self->cur_preset);
  gst_qahw_equalizer_set_band_levels (self, self->band_levels);

  return TRUE;
}

static void
gst_qahw_equalizer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwEqualizer *self = GST_QAHW_EQUALIZER (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_PRESET:
      gst_qahw_equalizer_set_cur_preset (self, g_value_get_enum (value));
      break;
    case PROP_BAND_LEVELS:{
      gst_qahw_equalizer_set_band_levels (self,
          (const GstStructure *) g_value_get_boxed (value));
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_equalizer_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwEqualizer *self = GST_QAHW_EQUALIZER (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_PRESET:
      g_value_set_enum (value, self->cur_preset);
      break;
    case PROP_BAND_LEVELS:
      g_value_set_boxed (value, self->band_levels);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_equalizer_finalize (GObject * object)
{
  GstQahwEqualizer *self = GST_QAHW_EQUALIZER (object);
  gst_structure_free (self->band_levels);
}

static void
gst_qahw_equalizer_init (GstQahwEqualizer * self)
{
  self->band_levels = gst_structure_new_empty ("band-levels");
}

static void
gst_qahw_equalizer_class_init (GstQahwEqualizerClass * klass)
{
  GstElementClass *gstelement_class;
  GObjectClass *gobject_class;
  GstQahwEffectClass *effect_class;

  gstelement_class = GST_ELEMENT_CLASS (klass);
  effect_class = GST_QAHW_EFFECT_CLASS (klass);
  gobject_class = G_OBJECT_CLASS (klass);

  effect_class->library = QAHW_EFFECT_EQUALIZER_LIBRARY;
  effect_class->uuid = SL_IID_EQUALIZER_UUID;
  effect_class->setup = gst_qahw_equalizer_setup;

  gobject_class->get_property = gst_qahw_equalizer_get_property;
  gobject_class->set_property = gst_qahw_equalizer_set_property;
  gobject_class->finalize = gst_qahw_equalizer_finalize;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio Equalizer Effect (QAHW)", "Filter/Effect/Audio",
      "Apply equalizer effect using QAHW",
      "Nicolas Dufresne <nicolas.dufresne@collabora.com>");

  g_object_class_install_property (gobject_class, PROP_PRESET,
      g_param_spec_enum ("preset", "Preset",
          "The currently selected preset.",
          gst_qahw_equalizer_preset_get_type (), 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));

  g_object_class_install_property (gobject_class, PROP_BAND_LEVELS,
      g_param_spec_boxed ("band-levels", "Band Levels",
          "This let you sel all 5 band levels at once.", GST_TYPE_STRUCTURE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}
