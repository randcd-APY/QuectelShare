/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * MediaRecorder is a wrapper on AUDIO HAL provides functionality
 * to configure audio recording session and streaming PCM samples
*/
#include <stdio.h>

#ifdef __cplusplus

extern "C" {

#endif

#include <gst/gst.h>

#include <gst/audio/audio.h>

#ifdef __cplusplus

}

#endif

struct AudioProp
{
  /* No cof audio Channels*/
  guint8 channels;
  /* Audio Sample Rate*/
  guint rate;
  /*PCM audio format*/
  gchar* format;
  /* Streaming PCM sample size*/
  gint bufsize;
  /* Device id*/
  gint device;
  /* FFV State*/
  gint ffv_state;
  /* FFV output refernce device*/
  gint ffv_ec_ref_dev;
  /* FFV channel index*/
  gint ffv_channel_index;
};

/**
     * Callback declaration of streaming PCM samples
     *
     * @param buf Buffer contains PCM samples.
     * @param size Size of buf in bytes
     * @streamid Should be zero
     * @return void.
*/

typedef void GetBufferEvent (guint8 * buf, guint size, gint streamid);

/**
     * Callback declaration of error event in raudio recording session
     *
     * @param errorType Error no
     * @streamid Should be zero
     * @return void.
*/

typedef void RecordFailedEvent (gint errorType, gint streamid);

class MediaRecorder {

private:
  /* Gstreamer data structures*/
  GMainLoop *m_loop;
  GstElement *m_source;
  GstElement *m_sink;
  GstElement *m_pipeline;
  gint m_streamid;
  guint m_busWatch;
  /* Callback pointer for record failed event*/
  RecordFailedEvent * m_RecordFailedEvent;
  /*Callback pointer for streaming PCM samples*/
  GetBufferEvent * m_GetBufferEvent;

public:

/**
     * Constructor
     *
     * @streamid Should be zero
*/

  MediaRecorder(gint streamid);

/**
     * Destructor
*/

  ~MediaRecorder();
/**
     * Initialize Audio recoding session with specified audio properties
     *
     * @param prop Audio properties
     * @return Success code or error code.
*/

  gint InitRecorder(AudioProp * prop);

/**
     * Start Audio recoding session
     *
     * @return Success code or error code.
*/

  gint StartRecorder();

/**
     * Stop Audio recoding session
     *
     * @return Success code or error code.
*/

  gint StopRecorder();

/**
     * Clean up all resources, should be called before calling destructor
     *
     * @return void.
*/

  void Clear();
/**
     *  Notification of a callback to execute on Audio recoding session error event
     *
     * @param eventFunc The callback to execute
     * @return void.
*/

  void registerRecordFailedEvent(RecordFailedEvent *eventFunc);
/**
     *  Notification of a callback to execute on PCM buffer capture of specified buffer size
     *
     * @param eventFunc The callback to execute
     * @return void.
*/

  void registerGetBufferEvent(GetBufferEvent *eventFunc);
/**
     *  Wrapper for calling m_GetBufferEvent
     * @param data PCM data buffer
     @ @param size PCM data size
     * @return void.
*/

  void callGetBufferEvent(guint8 * data, int size);
/**
     * Gstreamer bus message handler
     * @param bus Pointer to Gst Bus
     @ @param msg Bus message
     * @return success or error code
*/

  gboolean handle_bus_msg (GstBus * bus, GstMessage * msg);
/**
     *  Wrapper for running Glib Main loop
     * @return void.
*/

  void runMainLoop();
};
