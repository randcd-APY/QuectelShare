/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <QAHWManager/QAHWManager.h>

namespace alexaClientSDK {
namespace qahwManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::capabilityAgents::aip;
using namespace alexaClientSDK::avsCommon::sdkInterfaces;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;
using namespace alexaClientSDK::mediaPlayer;

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

#define OK 0;

std::unique_ptr<AudioInputStream::Writer> QAHWManager::m_audioBufferWriter;
std::shared_ptr<AudioInputStream> QAHWManager::m_audioBuffer;
//FILE* QAHWManager::fp;

QAHWManager::QAHWManager(
        std::shared_ptr<avsCommon::avs::AudioInputStream> stream,
        std::shared_ptr<defaultClient::DefaultClient> client,
        capabilityAgents::aip::AudioProvider audioProvider,
        std::shared_ptr<alexaClientSDK::QSTHWManager::QSTHWInterface> stHal)
        : m_stream { stream }, m_client { client }, m_audioProvider { audioProvider }, m_stHal { stHal }, m_isRecording { false } {
    printf("QAHWManager::QAHWManager() - DefaultClient \n");

    m_compatibleAudioFormat.sampleRateHz = SAMPLE_RATE_HZ;
    m_compatibleAudioFormat.sampleSizeInBits = WORD_SIZE * CHAR_BIT;
    m_compatibleAudioFormat.numChannels = NUM_CHANNELS;
    m_compatibleAudioFormat.endianness = alexaClientSDK::avsCommon::utils::AudioFormat::Endianness::LITTLE;
    m_compatibleAudioFormat.encoding = alexaClientSDK::avsCommon::utils::AudioFormat::Encoding::LPCM;
}

QAHWManager::~QAHWManager() {
    std::cout << "QAHWManager::~QAHWManager()" << std::endl;
}

void QAHWManager::startRecording(int channelIndex) {
    printf("QAHWManager::startRecording() %d\n", m_isRecording);
    AudioProp prop;
    int err;
    if (m_isRecording == false) {
        m_isRecording = true;

        auto tempM_client = m_client.lock();
        if (tempM_client) {
            printf("QAHWManager::startRecording() -  m_client->notifyOfTapToTalk\n");

            size_t m_bufferSize = alexaClientSDK::avsCommon::avs::AudioInputStream::calculateBufferSize(BUFFER_SIZE_IN_SAMPLES, WORD_SIZE, MAX_READERS);
            m_buffer = std::make_shared<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer>(m_bufferSize);
            m_audioBuffer = alexaClientSDK::avsCommon::avs::AudioInputStream::create(m_buffer, WORD_SIZE, MAX_READERS);
            m_audioBufferWriter = m_audioBuffer->createWriter(AudioInputStream::Writer::Policy::NONBLOCKABLE, true);

            alexaClientSDK::capabilityAgents::aip::AudioProvider tapAudioProvider(m_audioBuffer, m_compatibleAudioFormat,
                alexaClientSDK::capabilityAgents::aip::ASRProfile::NEAR_FIELD, false, true, true);

            auto startRecognition = (tempM_client->notifyOfTapToTalk(tapAudioProvider,
                    avsCommon::sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX)).get();

            printf("QAHWManager::startRecording() - startRecognition = %d \n", startRecognition);
            //Restart ST Hal Keyword Recognition if Audio input processor fails to send Recognize Event.
            if (startRecognition == true) {
                // fp = fopen("/data/pcmdump", "wb");
                ptr = new MediaRecorder(0);
                //Audio format: 16000 Hz - 1 channel - 16 bit - Little-endian
                prop.rate = 16000;
                prop.channels = 1;
                prop.bufsize = 320;
                prop.format = "S16LE";
                prop.ffv_state = 1;
                prop.ffv_ec_ref_dev = MediaPlayer::getCurrentDeviceId();
                prop.ffv_channel_index = channelIndex;

                ptr->registerGetBufferEvent(&QAHWManager::GetBufferEvent);
                ptr->registerRecordFailedEvent(&QAHWManager::RecordFailedEvent);

                err = ptr->InitRecorder(&prop);
                if (err < 0)
                    printf("QAHWManager::startRecording() - FAILED InitRecorder() = %d()\n", err);

                err = ptr->StartRecorder();
                if (err < 0)
                    printf("QAHWManager::startRecording() - FAILED StartRecorder() = %d()\n", err);
            }
            else {
                //Restart ST Hal Keyword Recognition if Audio input processor fails to send Recognize Event.
                printf("QAHWManager::startRecording() - startRecognition FAILED !!!!!");
                m_stHal->startRecognition();
            }
        }
    }
}

int QAHWManager::create_buffer_stream() {
    return OK;
}

void QAHWManager::stopRecording() {
    int err;
    printf("QAHWManager::StopRecorder() %d\n", m_isRecording);
    if (m_isRecording) {
        m_isRecording = false;
        err = ptr->StopRecorder();
        if (err < 0)
            printf("QAHWManager::startRecording() - FAILED err<0 (3) \n");

        ptr->Clear();
        delete ptr;
        // fclose(fp);

        printf("QAHWManager::stopRecording()- STOP recording and RESTARTING KeyWord detection\n");

        //Restart ST Hal Keyword Recognition
        m_stHal->startRecognition();
    }
}

void QAHWManager::GetBufferEvent(guint8 * buf, guint size, gint streamid) {
    // printf("QAHWManager::GetBufferEvent1  - Received buffer size =%d\n", size);

    //save audio in a file
    //fwrite(buf, 1, size, fp);
    //fflush(fp);

    //save audio at AIP stream
    // write to inputStream then callback
    m_audioBufferWriter->write(buf, size / 2);
}

void QAHWManager::RecordFailedEvent(gint errorType, gint streamid) {
    printf("QAHWManager::RecordFailedEvent - ERROR: %d \n", errorType);
}

}
}
