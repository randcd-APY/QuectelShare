/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
*/


#ifndef __GST_QSTHW_SOUND_TRIGGER_H__
#define __GST_QSTHW_SOUND_TRIGGER_H__

#include <gst/audio/audio.h>
#include <mm-audio/qsthw_api/qsthw_api.h>
#include <mm-audio/qsthw_api/qsthw_defs.h>

G_BEGIN_DECLS

#define GST_TYPE_QSTHW_SOUND_TRIGGER            (gst_qsthw_sound_trigger_get_type())
#define GST_QSTHW_SOUND_TRIGGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_QSTHW_SOUND_TRIGGER,GstQsthwSoundTrigger))
#define GST_QSTHW_SOUND_TRIGGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_QSTHW_SOUND_TRIGGER,GstQsthwSoundTriggerClass))
#define GST_IS_QSTHW_SOUND_TRIGGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_QSTHW_SOUND_TRIGGER))
#define GST_IS_QSTHW_SOUND_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_QSTHW_SOUND_TRIGGER))
#define GST_QSTHW_SOUND_TRIGGER_CAST(obj)       ((GstQsthwSoundTrigger *)(obj))

typedef struct _GstQsthwSoundTrigger GstQsthwSoundTrigger;
typedef struct _GstQsthwSoundTriggerClass GstQsthwSoundTriggerClass;

/**
 * GstQsthwSoundTrigger:
 *
 * Opaque data structure
 */
struct _GstQsthwSoundTrigger {
  GstPushSrc            element;

  char                  *module_id;
  char                  *model_location;
  guint                 num_keywords;
  guint                 num_users;
  gboolean              user_identification;
  guint                 keyword_confidence_level;
  guint                 user_confidence_level;
  char                  *vendor_uuid;
  gboolean              lab;
  qsthw_module_handle_t *module;
  struct sound_trigger_phrase_sound_model *model_data;
  sound_model_handle_t  model;
  struct sound_trigger_recognition_config config;

  GCond                 create_cond;
  gboolean              flushing;
  gboolean              enable_capturing;
  gboolean              user_module;
  GstClockTime          current_ts;
  GstAudioInfo          info;
};

struct _GstQsthwSoundTriggerClass {
  GstPushSrcClass parent_class;
};

GType gst_qsthw_sound_trigger_get_type(void);

G_END_DECLS

#endif /* __GST_QSTHW_SOUND_TRIGGER_H__ */
