/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "QSTHWManager/QSTHWClient.h"
#include <MediaPlayer/MediaPlayer.h>

namespace alexaClientSDK {
namespace QSTHWManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::avsCommon;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;
using namespace alexaClientSDK::mediaPlayer;
using namespace avsCommon::utils::sds;

/// The sample rate of microphone audio data.
static const unsigned int SAMPLE_RATE_HZ = 16000;
/// The number of audio channels.
static const unsigned int NUM_CHANNELS = 1;
/// The size of each word within the stream.
static const size_t WORD_SIZE = 2;
/// The maximum number of readers of the stream.
static const size_t MAX_READERS = 10;
/// The amount of audio data to keep in the ring buffer.
static const std::chrono::seconds AMOUNT_OF_AUDIO_DATA_IN_BUFFER = std::chrono::seconds(15);
/// The size of the ring buffer.
static const size_t BUFFER_SIZE_IN_SAMPLES = (SAMPLE_RATE_HZ)* AMOUNT_OF_AUDIO_DATA_IN_BUFFER.count();


#define DEFAULT_DEVICE 2
#define OK 0

struct keyword_buffer_config {
    int version;
    uint32_t kb_duration;
} __packed;

/* SVA vendor uuid */
static const sound_trigger_uuid_t qti_uuid = { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
static const sound_trigger_uuid_t qc_arm_uuid = { 0x67fabb70, 0x79e8, 0x4e1c, 0xa202,{ 0xbc, 0xb0, 0x50, 0x24, 0x3a, 0x70 } };


int QSTHWClient::m_currentDeviceId = DEFAULT_DEVICE;

#define MAX_SET_PARAM_KEYS 4
#define MAX_GET_PARAM_KEYS 2

/* sound trigger set parameter keys */
#define QSTHW_PARAMETER_CUSTOM_CHANNEL_MIXING "st_custom_channel_mixing"
#define QSTHW_PARAMETER_SESSION_PAUSE "st_session_pause"
#define QSTHW_PARAMETER_BAD_MIC_CHANNEL_INDEX "st_bad_mic_channel_index"
#define QSTHW_PARAMETER_EC_REF_DEVICE "st_ec_ref_device"
#define QSTHW_PARAMETER_DIRECTION_OF_ARRIVAL "st_direction_of_arrival"
#define QSTHW_PARAMETER_CHANNEL_INDEX "st_channel_index"
#define QSTHW_PARAMETER_ESP_ENERGY_LEVELS "st_esp_energy_levels"

static const char *set_param_key_array[] = { QSTHW_PARAMETER_CUSTOM_CHANNEL_MIXING, QSTHW_PARAMETER_SESSION_PAUSE,
        QSTHW_PARAMETER_BAD_MIC_CHANNEL_INDEX, QSTHW_PARAMETER_EC_REF_DEVICE };
static const char *get_param_key_array[] = { QSTHW_PARAMETER_DIRECTION_OF_ARRIVAL, QSTHW_PARAMETER_CHANNEL_INDEX, QSTHW_PARAMETER_ESP_ENERGY_LEVELS }; 

static const uint32_t KWD_CONFIDENCE_LEVEL = 0x0;
static const uint32_t KWD_INDEX = 0x1;
static const uint32_t KWD_MAX = 0x2;

std::string captureLabDataDir = "/data/SVA";

QSTHWClient::QSTHWClient(QSTHWCallbackIF * _cb, std::shared_ptr<avsCommon::avs::AudioInputStream> stream) {

    cb_if = _cb;            // callback when detection happens
    m_stream = stream;
    st_mod_handle = NULL;
    num_sessions = 0;
    lab_duration = 5;      // 5sec is default duration       // not used
    kb_duration_ms = 700; // 2000 msec is default duration  // not used
    total_duration_ms = 0;
    sound_model = NULL;
    fpSm = NULL;
    ses_id = 1;            // session id assumed to be 1
    target_chan_idx = 0;
    target_doa_idx = 0;
    keywordStartIndex = 0;
    keywordEndIndex = 0;
    lastWakeWordAndUserUtteranceEndIndex = 0;
    printf("inside QSTHWClient::QSTHWClient\n");

    if (access(captureLabDataDir.c_str(), F_OK) != -1) {
        printf("SVA directory exists...\n");
    } else {
        printf("Create /data/SVA directory...\n");
        mkdir(captureLabDataDir.c_str(), 0700);
    }
}

// simple tokenizer that assumes tokens will be separated by one delimiter only
// it keeps a static copy of the string passed in during the first call
// dynamically allocates space for a word and returns it as the token
char* tok(char* str, char d) {

    static int p = 0;
    static char* s = NULL;
    int l, t;

    if (str != NULL) {
        p = 0;
        if (s != NULL)
            delete[] s;

        l = strlen(str);
        s = new char[l + 1];

        strcpy(s, str);
    }

    t = p;
    while (s[t] != d && (s[t] != '\0')) {
        t++;
    }

    l = t - p;
    if (l == 0) {
        return NULL;
    }

    char* w = new char[l + 1];
    memcpy(w, s + p, l);
    w[l] = '\0';

    if (s[t] != '\0')
        p = ++t;            // skip over the delim
    else
        p = t;

    return w;
}


int QSTHWClient::init(char* inp) {

    const int arraySize = 15;

    char d = ' ';           // the token separator
    char *token;            // one token
    char** tokArray;        // array of tokens

    printf("inside QSTHWClient::init - %s\n", inp);

    tokArray = new char*[arraySize];
    memset(tokArray, NULL, arraySize);
    token = tok(inp, d);
    tokArray[0] = token;

    int i = 1;
    while (token != NULL) {

        token = tok(NULL, d);

        if (token != NULL) {
            tokArray[i++] = token;
        }
        else {
            tokArray[i] = NULL;
        }
    }

    int numTokens = i;      // number of array elements = numTokens + 1

    int result = parse_input(tokArray, numTokens);

    for (int j = 0; j < numTokens; j++) {
        if (tokArray[i] != NULL)
            delete [] tokArray[i];
    }
    delete[] tokArray;

    if (result == OK) {
        result = create_kw_lab_stream();
    }

    printf("exit QSTHWClient::init - %d\n", result);
    return result;
}

int QSTHWClient::create_kw_lab_stream() {

    int result = OK;

    if (m_stream == nullptr) {
        printf("assert_error: create_kw_lab_stream - m_stream == nullptr\n");
        result = -1;
    }
    else {
        audioBuffer = m_stream;
        audioBufferWriter = audioBuffer->createWriter(AudioInputStream::Writer::Policy::NONBLOCKABLE, true);
        if (audioBufferWriter == nullptr) {
            printf("assert_error: create_kw_lab_stream - audioBufferWriter == nullptr\n");
            result = -1;
        }
    }

    return result;
}

struct sm_session_data * QSTHWClient::get_sm_session_data(int session_id) {
    unsigned int i;

    for (i = 0; i < num_sessions; i++) {
        if (sound_trigger_info[i].session_id == session_id)
            return &sound_trigger_info[i];
    }
    return NULL;
}

struct sm_session_data * QSTHWClient::get_sound_trigger_info(sound_model_handle_t sm_handle) {
    unsigned int i;

    for (i = 0; i < num_sessions; i++) {
        if (sound_trigger_info[i].sm_handle == sm_handle)
            return &sound_trigger_info[i];
    }
    return NULL;
}

void QSTHWClient::deinit_sm_session_data(void) {
    int i;
    for (i = 0; i < MAX_SOUND_TRIGGER_SESSIONS; i++) {
        if (sound_trigger_info[i].rc_config)
            free(sound_trigger_info[i].rc_config);
        if (sound_trigger_info[i].qsthw_event)
            free(sound_trigger_info[i].qsthw_event);
    }
}

void QSTHWClient::init_sm_session_data(void) {
    int i;

    for (i = 0; i < MAX_SOUND_TRIGGER_SESSIONS; i++) {
        sound_trigger_info[i].session_id = -1;
        sound_trigger_info[i].vendor_uuid = qti_uuid;
        sound_trigger_info[i].sm_file_path[0] = '\0';
        sound_trigger_info[i].sm_handle = -1;
        sound_trigger_info[i].num_kws = 0;
        sound_trigger_info[i].num_users = 0;
        sound_trigger_info[i].loaded = false;
        sound_trigger_info[i].started = false;
        sound_trigger_info[i].counter = 0;
        sound_trigger_info[i].rc_config = NULL;
        sound_trigger_info[i].qsthw_event = NULL;
        sound_trigger_info[i].versioned_det_event_payload = false;
        sound_trigger_info[i].esp_levels = false;
    }
}

/* global fn passed to qsthw_start_recognition */
void eventCallback(struct sound_trigger_recognition_event *event, void *sessionHndl) {

    QSTHWClient* me = (QSTHWClient*) sessionHndl;

    if (me == NULL) {
        printf("QSTHWClient: error: token not passed back by ST HAL\n");
        return;
    }
    me->eventCallbackLocal(event);
}

void QSTHWClient::eventCallbackLocal(struct sound_trigger_recognition_event *event) {

    int rc;

    sound_model_handle_t sm_handle = event->model;
    struct qsthw_phrase_recognition_event *qsthw_event;
    uint64_t event_timestamp;
    unsigned int event_size;

    printf("[%d] Callback event received: %d\n", event->model, event->status);
    qsthw_event = (struct qsthw_phrase_recognition_event *) event;
    event_timestamp = qsthw_event->timestamp;
    printf("[%d] Event detection timestamp %llu\n", sm_handle, event_timestamp);

    const char *param = QSTHW_PARAMETER_ESP_ENERGY_LEVELS;
    size_t param_data_size;
    qsthw_get_param_payload_t payload;
    size_t payload_size = sizeof(qsthw_get_param_payload_t);

    struct sm_session_data *sm_data = get_sound_trigger_info(sm_handle);
    if (sm_data == NULL) {
        printf("Error: Invalid sound model handle %d\n", sm_handle);
        return;
    }

    if (sm_data->qsthw_event) {
        printf("Error: previous callback in progress for session %d\n", sm_handle);
        return;
    }

    if (sm_data->esp_levels) {
        rc = qsthw_get_param_data(st_mod_handle, sm_handle, param, (void *) &payload, payload_size, &param_data_size);
        if (rc < 0) {
            printf("[%d] failed to get esp levels\n", sm_handle);
        }
        else {
            process_get_param_data(param, &payload, param_data_size);
        }
    }


    event_size = sizeof(struct qsthw_phrase_recognition_event) + qsthw_event->phrase_event.common.data_size;
    sm_data->qsthw_event = (struct qsthw_phrase_recognition_event *)calloc(1, event_size);
    if (sm_data->qsthw_event == NULL) {
        printf("Could not allocate memory for sm data recognition event");
        return;
    }

    memcpy(sm_data->qsthw_event, qsthw_event, sizeof(struct qsthw_phrase_recognition_event));
    memcpy((char *)sm_data->qsthw_event + sm_data->qsthw_event->phrase_event.common.data_offset,
        (char *)qsthw_event + qsthw_event->phrase_event.common.data_offset,
        qsthw_event->phrase_event.common.data_size);

    worker = std::thread(std::bind(&QSTHWClient::event_handler_thread, this, sm_data));   // check outcome
    worker.detach();
}

void QSTHWClient::capture_lab_data(struct qsthw_phrase_recognition_event *qsthw_event, struct sm_session_data *sm_data) {
    int ret = 0;
    void *buffer;
    unsigned int period_size;
    size_t bytes, written;
    char lab_capture_file[128] = "";
    size_t cur_bytes_read = 0;
    size_t total_bytes_to_read;
    FILE *fp = NULL;
    struct sound_trigger_phrase_recognition_event phrase_event = qsthw_event->phrase_event;
    sound_model_handle_t sm_handle = phrase_event.common.model;
    audio_config_t *audio_config = &phrase_event.common.audio_config;
    uint32_t sample_rate = audio_config->sample_rate;
    uint32_t channels = audio_channel_count_from_in_mask(audio_config->channel_mask);
    audio_format_t format = audio_config->format;
    size_t samp_sz = audio_bytes_per_sample(format);

    /*
    This code for reinitialization of the stream is added here.
    ***************************************************************
    */
    size_t m_bufferSize = alexaClientSDK::avsCommon::avs::AudioInputStream::calculateBufferSize(BUFFER_SIZE_IN_SAMPLES, WORD_SIZE, MAX_READERS);
    m_buffer = std::make_shared<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer>(m_bufferSize);
    audioBuffer = alexaClientSDK::avsCommon::avs::AudioInputStream::create(m_buffer, WORD_SIZE, MAX_READERS);
    audioBufferWriter = audioBuffer->createWriter(AudioInputStream::Writer::Policy::NONBLOCKABLE, true);

    if (audioBufferWriter == nullptr) {
        printf("######################################### BAD STATE assert_error: create_kw_lab_stream - audioBufferWriter == nullptr\n");
        return;
    }
    printf("######################################### REINITIALIZED SHARED STREAM ########################################################\n");


    printf("Inside capture_lab_data\n");

    bytes = qsthw_get_buffer_size(st_mod_handle, sm_handle);
    if (bytes <= 0) {
        printf("Invalid buffer size returned\n");
        return;
    }

    /* total bytes to read = bytes to read per sec * duration where
     * audio configuration passed as part of recognition event is
     * used to obtain bytes per sec.
     */

    // clear audioBuffer for next recognition cycle (if required, since this is a circular buffer)
    // 16000*1*2*12 = 384000 bytes // assuming 12 s of total_duration_ms
    total_bytes_to_read = ((sample_rate * channels * samp_sz) * total_duration_ms) / 1000;
    printf("sample rate %d, channels %d, samp sz %d, duration %d\n, total_bytes_to_read %d", sample_rate, channels, samp_sz, total_duration_ms,
            total_bytes_to_read);

    buffer = calloc(1, bytes);
    if (buffer == NULL) {
        printf("Could not allocate memory for capture buffer\n");
        return;
    }

    snprintf(lab_capture_file, sizeof(lab_capture_file), "/data/SVA/lab_capture_file_%d", sm_handle);
    fp = fopen(lab_capture_file, "wb");
    if (fp == NULL) {
        printf("Could not open lab capture file : %s\n", lab_capture_file);
        free(buffer);
        return;
    }

    // callback to QSTHWManager while data is being buffered
    printf("\n indices [start,end] [%d,%d] \n", keywordStartIndex, keywordEndIndex);
    if (keywordStartIndex == 0 || keywordEndIndex == 0) {
        printf("calling keywordRecognized()%d, %d", keywordStartIndex, keywordEndIndex);
        cb_if->keywordRecognized(audioBuffer, "ALEXA");
    }
    else {
        printf("calling keywordRecognizedWithIndices() %d %d", keywordStartIndex, keywordEndIndex);
        cb_if->keywordRecognizedWithIndices(audioBuffer, "ALEXA", keywordStartIndex, keywordEndIndex);
    }

    printf("\nlab capture file : %s\n", lab_capture_file);
    printf("\n Starting LAB capture\n");
    while (sm_data->started == false) {
        // while (cur_bytes_read < total_bytes_to_read) {
        qsthw_read_buffer(st_mod_handle, sm_handle, (unsigned char*) buffer, bytes);

        // write to inputStream then callback
        audioBufferWriter->write(buffer, bytes / 2);

        written = fwrite(buffer, 1, bytes, fp);
        if (written != bytes) {
            printf("written %d, bytes %d\n", written, bytes);
            if (ferror(fp)) {
                printf("Error writing lab capture data into file %s\n", strerror(errno));
                break;
            }
        }

        cur_bytes_read += bytes;
        memset(buffer, 0, bytes);
    }

    printf("exited lab processing\n");
    lastWakeWordAndUserUtteranceEndIndex += (cur_bytes_read / 2);
    printf("counter of buffer lastWakeWordAndUserUtteranceEndIndex = %d", lastWakeWordAndUserUtteranceEndIndex);
    printf("bytes to read %d - old style, ignore, bytes read %d\n", total_bytes_to_read, cur_bytes_read);
    qsthw_stop_buffering(st_mod_handle, sm_handle);
    free(buffer);
    fclose(fp);
}

void QSTHWClient::process_detection_event(struct sm_session_data *sm_data) {
    int i, j, k, user_id;
    void *payload;
    char *payload_8;
    uint32_t *payload_32;
    uint32_t version, key, key_version, key_size;
    struct qsthw_phrase_recognition_event *event = sm_data->qsthw_event;
    struct sound_trigger_phrase_recognition_event phrase_event;

    printf("%s: offset %d, size %d\n", __func__, event->phrase_event.common.data_offset,
        event->phrase_event.common.data_size);
    payload = calloc(1, event->phrase_event.common.data_size);
    if (!payload) {
        printf("%s: Memory allocation for payload failed\n", __func__);
        return;
    }
    memcpy(payload, ((char *)event) + event->phrase_event.common.data_offset,
        event->phrase_event.common.data_size);
    payload_32 = (uint32_t *)payload;
    printf("%s: detection event status %d\n", __func__, event->phrase_event.common.status);
    printf("%s: detection event timestamp %ld\n", __func__, event->timestamp);

    phrase_event = event->phrase_event;
    if (sm_data->versioned_det_event_payload) {
        /*
        * versioned detection event payload will be in following format and
        * needs to be parsed accordingly.
        * uint32 version - describes the release version (starting from 0x1)
        * uint32 key1 - KWD_CONFIDENCE_LEVEL
        * uint32 key1_version - describes release version of key1 (starting from 0x1)
        * uint32 key1_size - payload size of key1 following this attribute
        * Key1_payload - payload of key1
        * ....
        *
        * Similarly key2 represents KWD_INDEX.
        * The payload format can be extended accordingly for any newly added keys
        * distinguished based on version.
        *
        */
        version = *payload_32++;
        printf("%s: version %d\n", __func__, version);
        if (version == 0x1) {
            for (k = 0; k < KWD_MAX; k++) {
                key = *payload_32++;
                printf("%s: [%d]: key %d\n", __func__, k, key);
                key_version = *payload_32++;
                printf("%s: key version %d\n", __func__, key_version);
                key_size = *payload_32++;
                printf("%s: key size %d\n", __func__, key_size);
                switch (key) {
                case KWD_CONFIDENCE_LEVEL:
                {
                    /* parse payload for key == KWD_CONFIDENCE_LEVEL */
                    if (key_version != 0x1) {
                        printf("%s: Invalid version for confidence level key %d\n",
                            __func__, key_version);
                        goto exit;
                    }
                    payload_8 = (char *)payload_32;
                    uint8_t conf_levels = *payload_8++;
                    printf("%s: conf levels %d\n", __func__, conf_levels);
                    for (i = 0; i < conf_levels; i++) {
                        printf("%s: [%d] kw_id level %d\n", __func__, i,
                            payload_8[i]);
                    }
                    payload_32 = (uint32_t *)((char *)payload_32 + key_size);
                    break;
                }
                case KWD_INDEX:
                {
                    /* parse payload for key == KWD_INDEX */
                    if (key_version != 0x1) {
                        printf("%s: Invalid version for keyword index key %d\n",
                            __func__, key_version);
                        goto exit;
                    }
                    /* logic for handling keyword start index */
                    /* comment for reinitialzation on a new capture */
                    //keywordStartIndex = *payload_32 + lastWakeWordAndUserUtteranceEndIndex;
                    keywordStartIndex = *payload_32;
                    printf("%s: class level keyword start index %d, last utterance index %d\n", __func__, keywordStartIndex, lastWakeWordAndUserUtteranceEndIndex);
                    printf("%s: keyword start index %d\n", __func__, *payload_32++);

                    //keywordEndIndex = *payload_32 + lastWakeWordAndUserUtteranceEndIndex;
                    keywordEndIndex = *payload_32;
                    printf("%s: class level keyword stop index %d, last utterance index %d\n", __func__, keywordEndIndex, lastWakeWordAndUserUtteranceEndIndex);
                    printf("%s: keyword stop index %d\n", __func__, *payload_32++);
                    payload_32 += key_size;
                    break;
                }
                default:
                {
                    printf("%s: Invalid key %d\n", __func__, key);
                    goto exit;
                }
                }
            }
        }
        else {
            printf("%s: Invalid version for detection event payload %d\n",
                __func__, version);
            goto exit;
        }
    }
    else {
        for (i = 0; i < phrase_event.num_phrases; i++) {
            printf("%s: [%d] kw_id %d level %d\n", __func__, i,
                phrase_event.phrase_extras[i].id,
                ((char *)payload)[i]);
            printf("%s: Num levels %d\n", __func__,
                phrase_event.phrase_extras[i].num_levels);
            for (j = 0; j < phrase_event.phrase_extras[i].num_levels; j++) {
                user_id = phrase_event.phrase_extras[i].levels[j].user_id;
                printf("%s: [%d] user_id %d level %d\n", __func__, i,
                    user_id, ((char *)payload)[user_id]);
            }
        }
    }

exit:
    if (payload)
        free(payload);
}

void QSTHWClient::event_handler_thread(struct sm_session_data *sm_data) {

    int system_ret;

    if (!sm_data) {
        printf("Error: context is null\n");
        return;
    }

    system_ret = system("echo qti_services > /sys/power/wake_lock");
    if (system_ret < 0)
        printf("%s: Failed to acquire qti lock\n", __func__);
    else
        printf("%s: Success to acquire qti lock\n", __func__);

    sound_model_handle_t sm_handle = sm_data->sm_handle;
    printf("[%d] session params %p, %d\n", sm_handle, sm_data->rc_config, total_duration_ms);
    if (sm_data->qsthw_event) {
        process_detection_event(sm_data);
        sm_data->started = false; // this will be set to true externally
        if (sm_data->qsthw_event->phrase_event.common.capture_available) {
            printf("capture LAB data\n");
            capture_lab_data(sm_data->qsthw_event, sm_data);
        }
        free(sm_data->qsthw_event);
        sm_data->qsthw_event = NULL;
    }

    /* ignore error */
    printf("event_handler_thread - exited capture_lab_data based on a call to doneLookahead()\n");
    printf("event_handler_thread - *** qsthw_start_recognition ***\n");

    int ret = qsthw_start_recognition(st_mod_handle, sm_handle, sm_data->rc_config, eventCallback, this);
    if (OK != ret) {
        printf("qsthw_start_recognition - FAILED: %d", ret);
        //Try to reinitialize STHAL
        cb_if->reinitializeSTHal();
    }
    else {
        sm_data->started = true;
        cb_if->recognitionStateChanged(true);
    }

    sm_data->counter++;
    printf("[%d] callback event processed, detection counter %d\n", sm_handle, sm_data->counter);
    printf("proceed with utterance or command \n");
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n");

    if (system_ret >= 0) {
        system_ret = system("echo qti_services > /sys/power/wake_unlock");
        if (system_ret < 0)
            printf("%s: Failed to release qti lock\n", __func__);
        else
            printf("%s: Success to release qti lock\n", __func__);
    }

    return;
}

const char * QSTHWClient::handle_set_parameters() {
    char kv_pair[128];
    char *id, *test_r, *temp_str;
    int i;

    printf("Enter set param key value string:(ex - <st_session_pause=1>");
    fgets(kv_pair, 128, stdin);
    printf("Received key value pair: %s\n", kv_pair);

    kv_pair[strcspn(kv_pair, "\r\n")] = '\0';
    temp_str = strdup(kv_pair);

    id = strtok_r(kv_pair, "= ", &test_r);
    if (!id) {
        printf("%s: incorrect key value pair", __func__);
        return NULL;
    }

    for (i = 0; i < MAX_SET_PARAM_KEYS; i++) {
        if (!strncmp(id, set_param_key_array[i], strlen(set_param_key_array[i])))
            return temp_str;
    }

    return NULL;
}

bool QSTHWClient::startRecognition() {

    printf("in QSTHWClient::startRecognition\n");

    struct sm_session_data *ses_data = get_sm_session_data(ses_id);

    if (ses_data == NULL) {
        printf("Error: Invalid sound model handle %d\n", ses_id);
    }

    struct sound_trigger_recognition_config *rc_config = ses_data->rc_config;

    sound_model_handle_t sm_handle = ses_data->sm_handle;

    int status = qsthw_start_recognition(st_mod_handle, sm_handle, rc_config, eventCallback, this);
    if (OK != status) {
        printf("start_recognition failed - int code =  %d\n", status);
        return false;
    }
    else {
        ses_data->started = true;
        return true;
    }
}

void QSTHWClient::stopRecognition() {

    printf("in QSTHWClient::stopRecognition\n");

    struct sm_session_data *ses_data = get_sm_session_data(ses_id);

    if (ses_data == NULL) {
        printf("Error: Invalid sound model handle %d\n", ses_id);
    }

    sound_model_handle_t sm_handle = ses_data->sm_handle;

    int status = qsthw_stop_recognition(st_mod_handle, sm_handle);
    if (OK != status) {
        printf("stop_recognition failed\n");
    }
    else {
        ses_data->started = false;
    }
}

void QSTHWClient::doneLookahead() {

    struct sm_session_data *ses_data = get_sm_session_data(ses_id);
    ses_data->started = true;                                                      // to break LAB processing looping
}

void QSTHWClient::shutdown() {

    printf("in QSTHWClient::qsthw_shutdown\n");

    int status;

    struct sm_session_data *ses_data = get_sm_session_data(ses_id);

    if (ses_data == NULL) {
        printf("Error: Invalid sound model handle %d\n", ses_id);
    }

    //QTI struct sound_trigger_recognition_config *rc_config = &ses_data->rc_config;

    for (int i = 0; i < num_sessions; i++) {
        sound_model_handle_t sm_handle = sound_trigger_info[i].sm_handle;
        if (sound_trigger_info[i].started) {
            status = qsthw_stop_recognition(st_mod_handle, sm_handle);
            if (OK != status) {
                printf("stop_recognition failed\n");
            }
            sound_trigger_info[i].started = false; // ignore errors and set it to false, since shutting down
        }
        if (sound_trigger_info[i].loaded) {
            status = qsthw_unload_sound_model(st_mod_handle, sm_handle);
            if (OK != status)
                printf("unload_sound_model failed\n");
            sound_trigger_info[i].loaded = false;
        }
    }

    //QTI
    //TODO: Check free. It is failing

    status = qsthw_unload_module(st_mod_handle);
    if (OK != status) {
        printf("qsthw_unload_module failed, status %d\n", status);
    }
    if (sound_model)
        free(sound_model);
    if (fpSm)
        fclose(fpSm);
    deinit_sm_session_data();
}

int QSTHWClient::string_to_uuid(const char *str, sound_trigger_uuid_t *uuid) {
    int tmp[10];

    if (str == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (sscanf(str, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x", tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5, tmp + 6, tmp + 7, tmp + 8,
            tmp + 9) < 10) {
        return -EINVAL;
    }
    uuid->timeLow = (uint32_t) tmp[0];
    uuid->timeMid = (uint16_t) tmp[1];
    uuid->timeHiAndVersion = (uint16_t) tmp[2];
    uuid->clockSeq = (uint16_t) tmp[3];
    uuid->node[0] = (uint8_t) tmp[4];
    uuid->node[1] = (uint8_t) tmp[5];
    uuid->node[2] = (uint8_t) tmp[6];
    uuid->node[3] = (uint8_t) tmp[7];
    uuid->node[4] = (uint8_t) tmp[8];
    uuid->node[5] = (uint8_t) tmp[9];

    return 0;
}

/* called from init that passes in a user string */

int QSTHWClient::parse_input(char** inpArray, int arraySize) {

    bool exit_loop = false;
    int sm_data_size = 0, opaque_data_size = 0;
    int sound_model_size = 0, i;
    unsigned int j, k;
    uint32_t rc_config_size;
    struct keyword_buffer_config kb_config;
    bool user_verification = false;
    unsigned int kw_conf = 60;     //  default confidence level is 60
    unsigned int user_conf = 60;
    bool capture_requested = true; //  false;
    bool lookahead_buffer = true; //  false;
    bool keyword_buffer = true; //  false;
    bool usr_req_lookahead_buffer = false;
    int index = -1;
    char ec_ref_dev_kvpair[30];

    if (arraySize < 3) {
        printf("too few arguments - check\n");
        return 0;
    }

    // arguments to start with number of sessions always
    if ((strcmp(inpArray[1], "-ns") == 0)) {
        num_sessions = atoi(inpArray[2]);
    }

    if ((num_sessions <= 0) || (num_sessions > MAX_SOUND_TRIGGER_SESSIONS)) {
        printf("num_sessions <= 0 or num_sessions > MAX_SOUND_TRIGGER_SESSIONS");
        return 0;
    }

    init_sm_session_data();
    unsigned int params = 0, count = 0;
    i = 3;

    while (count < num_sessions) {
        if ((i < arraySize) && (strcmp(inpArray[i], "-id") == 0) && ((i + 1) < arraySize)) {
            index++;
            sound_trigger_info[index].session_id = atoi(inpArray[i + 1]);
            params = 1;
        }
        if (((i + 2) < arraySize) && (strcmp(inpArray[i + 2], "-sm") == 0)) {
            if (((i + 3) < arraySize) && (index >= 0)) {
                strncpy(sound_trigger_info[index].sm_file_path, inpArray[i + 3],  // tito strlcpy not present in string.h
                        sizeof(sound_trigger_info[index].sm_file_path));
                params++;
            }
        }
        if (((i + 4) < arraySize) && (strcmp(inpArray[i + 4], "-nk") == 0)) {
            if (((i + 5) < arraySize) && (index >= 0)) {
                sound_trigger_info[index].num_kws = atoi(inpArray[i + 5]);
                params++;
            }
        }
        if (((i + 6) < arraySize) && (strcmp(inpArray[i + 6], "-nu") == 0)) {
            if (((i + 7) < arraySize) && (index >= 0)) {
                sound_trigger_info[index].num_users = atoi(inpArray[i + 7]);
                params++;
            }
        }
        /* If vendor uuid entered without entering number of users*/
        if (((i + 6) < arraySize) && (strcmp(inpArray[i + 6], "-vendor_uuid") == 0)) {
            if (((i + 7) < arraySize) && (index >= 0)) {
                string_to_uuid(inpArray[i + 7], &sound_trigger_info[index].vendor_uuid);
                params++;
            }
        }
        if (((i + 8) < arraySize) && (strcmp(inpArray[i + 8], "-vendor_uuid") == 0)) {
            if (((i + 9) < arraySize) && (index >= 0)) {
                string_to_uuid(inpArray[i + 9], &sound_trigger_info[index].vendor_uuid);
                params++;
            }
        }
        if (params < MIN_REQ_PARAMS_PER_SESSION) {
            printf("Insufficient data entered for session %d\n", count);
            return 0;
        }

        count++;
        i += (params * 2);

        if (!memcmp(&sound_trigger_info[index].vendor_uuid, &qc_arm_uuid,
            sizeof(sound_trigger_uuid_t))) {
            printf("Versioned detection event\n");
            sound_trigger_info[index].versioned_det_event_payload = true;
            sound_trigger_info[index].esp_levels = true;
        }
    }

    if (++index != num_sessions) {
        printf("Insufficient data entered for sessions\n");
        return 0;
    }

    while (i < arraySize) {
        if ((strcmp(inpArray[i], "-user") == 0) && ((i + 1) < arraySize)) {
            user_verification = (0 == strncasecmp(inpArray[i + 1], "true", 4)) ? true : false;
        }
        else if ((strcmp(inpArray[i], "-lab") == 0) && ((i + 1) < arraySize)) {
            lookahead_buffer = (0 == strncasecmp(inpArray[i + 1], "true", 4)) ? true : false;
            usr_req_lookahead_buffer = true;
        }
        else if ((strcmp(inpArray[i], "-lab_duration") == 0) && ((i + 1) < arraySize)) {
            lab_duration = atoi(inpArray[i + 1]);
        }
        else if ((strcmp(inpArray[i], "-kb") == 0) && ((i + 1) < arraySize)) {
            keyword_buffer = (0 == strncasecmp(inpArray[i + 1], "true", 4)) ? true : false;
        }
        else if ((strcmp(inpArray[i], "-kb_duration") == 0) && ((i + 1) < arraySize)) {
            kb_duration_ms = atoi(inpArray[i + 1]);
        }
        else if ((strcmp(inpArray[i], "-kwcnf") == 0) && ((i + 1) < arraySize)) {
            kw_conf = atoi(inpArray[i + 1]);
        }
        else if ((strcmp(inpArray[i], "-usrcnf") == 0) && ((i + 1) < arraySize)) {
            user_conf = atoi(inpArray[i + 1]);
        }
        else {
            printf("Invalid syntax\n");
            exit(0);
        }
        i += 2;
    }

    int status = 0;
    if (usr_req_lookahead_buffer) {
        if ((lookahead_buffer == false) && (keyword_buffer == true)) {
            printf("Invalid usecase: lab can't be false when keyword buffer is true ");
            status = -EINVAL;
            return status;
        }
    }
    printf("keyword buffer %d", keyword_buffer);
    capture_requested = (lookahead_buffer || keyword_buffer) ? true : false;
    total_duration_ms = (lookahead_buffer ? lab_duration * 1000 : 0) + (keyword_buffer ? kb_duration_ms : 0);

    char command[128];

    st_mod_handle = qsthw_load_module(QSTHW_MODULE_ID_PRIMARY);
    if (NULL == st_mod_handle) {
        printf("qsthw_load_module() failed\n");
        status = -EINVAL;
        return status;
    }

    m_currentDeviceId = MediaPlayer::getCurrentDeviceId();

    memset(ec_ref_dev_kvpair, 0x0, sizeof(ec_ref_dev_kvpair));
    snprintf(ec_ref_dev_kvpair, 30, "%s=%d", QSTHW_PARAMETER_EC_REF_DEVICE, m_currentDeviceId);
    printf("ec_ref_dev_kvpair: %s \n", ec_ref_dev_kvpair);

    qsthw_set_parameters(st_mod_handle, NULL, ec_ref_dev_kvpair);

    fpSm = NULL;  // FILE *fpSm = NULL;
    for (k = 0; k < num_sessions; k++) {
        unsigned int num_kws = sound_trigger_info[k].num_kws;
        unsigned int num_users = user_verification ? sound_trigger_info[k].num_users : 0;
        sound_model_handle_t sm_handle = 0;
        struct sound_trigger_recognition_config *rc_config = NULL;

        if (fpSm)
            fclose(fpSm);
        fpSm = fopen(sound_trigger_info[k].sm_file_path, "rb");
        if (fpSm == NULL) {
            printf("Could not open sound model file : %s\n", sound_trigger_info[k].sm_file_path);
            goto error;
        }

        /* Get the sound mode size i.e. file size */
        fseek(fpSm, 0, SEEK_END);
        sm_data_size = ftell(fpSm);
        fseek(fpSm, 0, SEEK_SET);

        sound_model_size = sizeof(struct sound_trigger_phrase_sound_model) + sm_data_size;
        if (sound_model)
            free(sound_model);

        sound_model = (struct sound_trigger_phrase_sound_model *) calloc(1, sound_model_size);
        if (sound_model == NULL) {
            printf("Could not allocate memory for sound model");
            goto error;
        }

        sound_model->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
        sound_model->common.data_size = sm_data_size;
        sound_model->common.data_offset = sizeof(*sound_model);
        sound_model->num_phrases = num_kws;
        for (i = 0; i < num_kws; i++) {
            sound_model->phrases[i].num_users = num_users;
            if (user_verification)
                sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_USER_IDENTIFICATION;
            else
                sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER;
        }
        int bytes_read = fread((char*) sound_model + sound_model->common.data_offset, 1, sm_data_size, fpSm);
        printf("bytes from the file %d\n", bytes_read);
        if (bytes_read != sm_data_size) {
            printf("Something wrong while reading data from file: bytes_read %d file_size %d", bytes_read, sm_data_size);
            goto error;
        }

        memcpy(&sound_model->common.vendor_uuid, &sound_trigger_info[k].vendor_uuid, sizeof(sound_trigger_uuid_t));
        printf("sound model data_size %d data_offset %d\n", sm_data_size, sound_model->common.data_offset);
        status = qsthw_load_sound_model(st_mod_handle, &sound_model->common, NULL, NULL, &sm_handle);
        if (OK != status) {
            printf("load_sound_model failed\n");
            goto error;
        }
        sound_trigger_info[k].loaded = true;

        if (keyword_buffer)
            opaque_data_size = sizeof(struct keyword_buffer_config);

        rc_config_size = sizeof(struct sound_trigger_recognition_config) + opaque_data_size;
        sound_trigger_info[k].rc_config = (struct sound_trigger_recognition_config *) calloc(1, rc_config_size);
        if (sound_trigger_info[k].rc_config == NULL) {
            printf("Could not allocate memory for recognition config");
            goto error;
        }
        rc_config = sound_trigger_info[k].rc_config;
        rc_config->capture_handle = AUDIO_IO_HANDLE_NONE;
        rc_config->capture_device = AUDIO_DEVICE_NONE;
        rc_config->capture_requested = capture_requested;
        rc_config->num_phrases = num_kws;

        if (keyword_buffer) {
            kb_config.version = SM_MINOR_VERSION;
            kb_config.kb_duration = kb_duration_ms;
            memcpy((char *) rc_config + rc_config->data_offset, &kb_config, sizeof(struct keyword_buffer_config));
            rc_config->data_size = sizeof(struct keyword_buffer_config);
            rc_config->data_offset = sizeof(struct sound_trigger_recognition_config);
        }
        int user_id = num_kws; //user_id should start from num_kws
        for (i = 0; i < num_kws; i++) {
            rc_config->phrases[i].id = i;
            rc_config->phrases[i].confidence_level = kw_conf;
            rc_config->phrases[i].num_levels = num_users;
            for (j = 0; j < num_users; j++) {
                rc_config->phrases[i].levels[j].level = user_conf;
                rc_config->phrases[i].levels[j].user_id = user_id++;
            }
            if (user_verification)
                rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_USER_IDENTIFICATION;
            else
                rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER;
        }

        sound_trigger_info[k].sm_handle = sm_handle;
        printf("[%d]session params %p, %p, %d\n", k, &sound_trigger_info[k], rc_config, sm_handle);
    }

    // return here, since no errors encountered.
    printf("mainfn: no errors encountered during init... proceed with recognition\n");
    return status;

    error:

    printf("mainfn: errors encountered during init... don't proceed with recognition\n");
    shutdown();

    return status;
}

int QSTHWClient::getChannelIndex() {
    qsthw_get_param_payload_t payload;
    size_t payload_size = sizeof(qsthw_get_param_payload_t);
    int status = 0;

    const char *param = QSTHW_PARAMETER_CHANNEL_INDEX;
    struct sm_session_data *ses_data = get_sm_session_data(ses_id);
    sound_model_handle_t sm_handle = ses_data->sm_handle;
    size_t param_data_size;

    printf("QSTHWClient::getChannelIndex() - [Session:%d] in get parameters %d\n", ses_id, ses_data->started);
    printf("[Session:%d] param to get %s\n", ses_id, param);

    status = qsthw_get_param_data(st_mod_handle, sm_handle, QSTHW_PARAMETER_CHANNEL_INDEX, (void *) &payload, payload_size, &param_data_size);

    if (status < 0) {
        printf("[Session:%d] failed to get param data %s\n", ses_id, param);
    }
    else {
        process_get_param_data(param, &payload, param_data_size);
    }

    return target_chan_idx;
}

int QSTHWClient::getDOADirection() {
    qsthw_get_param_payload_t payload;
    size_t payload_size = sizeof(qsthw_get_param_payload_t);
    int status = 0;

    const char *param = QSTHW_PARAMETER_DIRECTION_OF_ARRIVAL;
    struct sm_session_data *ses_data = get_sm_session_data(ses_id);
    sound_model_handle_t sm_handle = ses_data->sm_handle;
    size_t param_data_size;

    printf("QSTHWClient::getDOADirection() - [Session:%d] in get parameters %d\n", ses_id, ses_data->started);
    printf("[Session:%d] param to get %s\n", ses_id, param);

    status = qsthw_get_param_data(st_mod_handle, sm_handle, QSTHW_PARAMETER_DIRECTION_OF_ARRIVAL, (void *) &payload, payload_size, &param_data_size);

    if (status < 0) {
        printf("[Session:%d] failed to get param data %s\n", ses_id, param);
    }
    else {
        process_get_param_data(param, &payload, param_data_size);
    }

    return target_doa_idx;
}

int QSTHWClient::getESPAmbientEnergy() {
    return ambient_energy;
}

int QSTHWClient::getESPVoiceEnergy() {
    return voice_energy;
}

void QSTHWClient::process_get_param_data(const char *param, qsthw_get_param_payload_t *payload, size_t param_data_size) {
    struct qsthw_source_tracking_param st_params = payload->st_params;

    if (!strncmp(param, QSTHW_PARAMETER_DIRECTION_OF_ARRIVAL, sizeof(param))) {
        if (param_data_size != sizeof(struct qsthw_source_tracking_param)) {
            printf("%s: ERROR. Invalid param data size returned %d\n", __func__, param_data_size);
            return;
        }

        //TODO: CHECK LOGIC to translate ANGLES in SECTORS
        target_doa_idx = st_params.target_angle_L16[0] / 40;
        printf("%s: target angle boundaries [%d, %d]\n", __func__, st_params.target_angle_L16[0], st_params.target_angle_L16[1]);
        //printf("%s: interference angle boundaries [%d, %d]\n", __func__, st_params.interf_angle_L16[0], st_params.interf_angle_L16[1]);

    } else if (!strncmp(param, QSTHW_PARAMETER_CHANNEL_INDEX, sizeof(param))) {
        printf("%s: procsesing channel index params\n", __func__);
        struct qsthw_target_channel_index_param ch_index_params = payload->ch_index_params;
        if (param_data_size != sizeof(struct qsthw_target_channel_index_param)) {
            printf("%s: ERROR. Invalid param data size returned %d\n",
                    __func__, param_data_size);
            return;
        }
        target_chan_idx = ch_index_params.target_chan_idx;
        printf("%s: target channel index - [%d]\n", __func__, target_chan_idx);
    }
    else if (!strncmp(param, QSTHW_PARAMETER_ESP_ENERGY_LEVELS, sizeof(param))) {
        printf("%s: procsesing esp energy levels params %p %d\n", __func__, payload, param_data_size);
        struct qsthw_esp_energy_levels_param energy_levels_params = payload->esp_energy_levels_params;
        if (param_data_size != sizeof(struct qsthw_esp_energy_levels_param)) {
            printf("%s: ERROR. Invalid param data size returned %d\n", __func__, param_data_size);
            return;
        }
        printf("%s: ESP: voice energy %d, ambient noise energy %d\n", __func__, energy_levels_params.voiceEnergy,
                energy_levels_params.ambientNoiseEnergy);

        voice_energy = energy_levels_params.voiceEnergy;
        ambient_energy = energy_levels_params.ambientNoiseEnergy;
    }
}

} // ns
} // ns
