/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ALEXA_CLIENT_SDK_QSTHW_CLIENT_H_
#define ALEXA_CLIENT_SDK_QSTHW_CLIENT_H_

#include <thread>
#include <stdio.h>
#include <ostream>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <system/sound_trigger.h>
#include "qsthw_api.h"
#include "qsthw_defs.h"

#include <QSTHWManager/QSTHWCallbackIF.h>
#include <AVSCommon/Utils/AudioFormat.h>
#include <AVSCommon/AVS/AudioInputStream.h>
#include <AIP/AudioProvider.h>

namespace alexaClientSDK {
namespace QSTHWManager {

using namespace alexaClientSDK::avsCommon;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;
using namespace alexaClientSDK::QSTHWManager;
using namespace avsCommon::utils::sds;

// #define OK 0
#define MAX_SOUND_TRIGGER_SESSIONS 8
#define SM_FILE_PATH_MAX_LENGTH 128
#define MIN_REQ_PARAMS_PER_SESSION 3
#define SM_MINOR_VERSION 1

struct sm_session_data {
    int session_id;
    sound_trigger_uuid_t vendor_uuid;
    char sm_file_path[SM_FILE_PATH_MAX_LENGTH];
    sound_model_handle_t sm_handle;
    unsigned int num_kws;
    unsigned int num_users;
    bool loaded;
    bool started;
    unsigned int counter;
    struct sound_trigger_recognition_config *rc_config;
    struct qsthw_phrase_recognition_event *qsthw_event;
    bool versioned_det_event_payload;
    bool esp_levels;
};

class QSTHWClient {
public:
    QSTHWClient(QSTHWCallbackIF * _cb, std::shared_ptr<avsCommon::avs::AudioInputStream> stream);

    int init(char* inp);

    bool startRecognition();

    void stopRecognition();

    void doneLookahead();

    void shutdown();

    std::unique_ptr<AudioInputStream::Writer> audioBufferWriter;
    std::shared_ptr<AudioInputStream> audioBuffer;
    std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer> m_buffer;

    QSTHWCallbackIF * cb_if;

    void event_handler_thread(struct sm_session_data *sm_data);
    void eventCallbackLocal(struct sound_trigger_recognition_event *event);

    int getDOADirection();
    int getChannelIndex();

    int getESPVoiceEnergy();
    int getESPAmbientEnergy();

private:

    std::shared_ptr<avsCommon::avs::AudioInputStream> m_stream;

    int parse_input(char** inpArray, int size);
    int create_kw_lab_stream();
    struct sm_session_data *get_sm_session_data(int session_id);
    struct sm_session_data *get_sound_trigger_info(sound_model_handle_t sm_handle);
    void init_sm_session_data(void);
    void deinit_sm_session_data(void);
    void capture_lab_data(struct qsthw_phrase_recognition_event *qsthw_event, struct sm_session_data *sm_data);
    void process_detection_event(struct sm_session_data *sm_data);
    static int string_to_uuid(const char *str, sound_trigger_uuid_t *uuid);
    const char* handle_set_parameters();
    void process_get_param_data(const char *param, qsthw_get_param_payload_t *payload, size_t param_data_size);

    const qsthw_module_handle_t *st_mod_handle;
    struct sm_session_data sound_trigger_info[MAX_SOUND_TRIGGER_SESSIONS];
    unsigned int num_sessions;
    int lab_duration;   //5sec is default duration
    int kb_duration_ms; //2000 msec is default duration
    int total_duration_ms;
    struct sound_trigger_phrase_sound_model *sound_model;
    FILE *fpSm;
    int ses_id;
    std::thread worker;

    static int m_currentDeviceId;

    int target_chan_idx;
    int target_doa_idx;

    int voice_energy;
    int ambient_energy;

    uint32_t keywordStartIndex;
    uint32_t keywordEndIndex;
    uint32_t lastWakeWordAndUserUtteranceEndIndex;
};
} // ns
} // ns

#endif
