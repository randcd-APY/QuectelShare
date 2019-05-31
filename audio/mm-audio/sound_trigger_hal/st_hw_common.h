/* st_hw_common.h
 *
 * Contains common functionality between
 * sound trigger hw and sound trigger extension interface.

 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sound_trigger_hw.h"
#include "st_session.h"

int stop_other_sessions(struct sound_trigger_device *stdev,
                        st_session_t *cur_ses);
int start_other_sessions(struct sound_trigger_device *stdev,
                        st_session_t *cur_ses);
st_session_t* get_sound_trigger_session(struct sound_trigger_device *stdev,
                                   sound_model_handle_t sound_model_handle);

/* callback to hw for session events */
typedef void (*hw_session_notify_callback_t)(sound_model_handle_t handle,
                                     st_session_event_id_t event);
int hw_session_notifier_init(hw_session_notify_callback_t cb);
void hw_session_notifier_deinit();
int hw_session_notifier_enqueue(sound_model_handle_t handle,
                                st_session_event_id_t event, uint64_t delay_ms);
int hw_session_notifier_cancel(sound_model_handle_t handle,
                               st_session_event_id_t event);
