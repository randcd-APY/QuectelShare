/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "../include/QSTHWManager/QTISoundTrigger.h"
#include <cutils/properties.h>
#define PROP_MAX_VALUE 10

namespace alexaClientSDK {
namespace QSTHWManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::capabilityAgents::aip;
using namespace alexaClientSDK::avsCommon::sdkInterfaces;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;

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


char* THREE_MIC_CMD = "sound_trigger_test -ns 1 -id 1 -sm /etc/alexa/alexa.uim -nk 1 -lab true";
char* SIX_MIC_CMD = "sound_trigger_test -ns 1 -id 1 -sm /etc/alexa/alexa.uim -nk 1 -vendor_uuid 67fabb70-79e8-4e1c-a202-bcb050243a70";

bool QTISoundTrigger::m_isRecognizing = false;

QTISoundTrigger::QTISoundTrigger(
        std::shared_ptr<avsCommon::avs::AudioInputStream> stream,
        std::shared_ptr<defaultClient::DefaultClient> client,
        capabilityAgents::aip::AudioProvider audioProvider)
        :
                m_stream { stream },
                m_client { client },
                m_audioProvider { audioProvider },
                qsthwClient { nullptr },
                m_detectionCount { 0 },
                m_dOAIndex { 0 },
                m_channelIndex { 0 },
                m_isWakewordEnabled { true } {
}

QTISoundTrigger::~QTISoundTrigger() {
    std::cout << "QTISoundTrigger::~QTISoundTrigger()" << std::endl;
}

int QTISoundTrigger::initialize() {

    int result = 0;
    char mic[PROP_MAX_VALUE] = "6";
    char wakeword[PROP_MAX_VALUE] = "1";
    char ESP[PROP_MAX_VALUE] = "1";
    qsthwClient = new QSTHWClient(this, m_stream);

    m_compatibleAudioFormat.sampleRateHz = SAMPLE_RATE_HZ;
    m_compatibleAudioFormat.sampleSizeInBits = WORD_SIZE * CHAR_BIT;
    m_compatibleAudioFormat.numChannels = NUM_CHANNELS;
    m_compatibleAudioFormat.endianness = alexaClientSDK::avsCommon::utils::AudioFormat::Endianness::LITTLE;
    m_compatibleAudioFormat.encoding = alexaClientSDK::avsCommon::utils::AudioFormat::Encoding::LPCM;

    property_get("ro.qc.sdk.fwk.mic_support", mic, "6");
    printf("######## Value of ro.qc.sdk.fwk.mic_support = %s\n",mic);

    if (!strncmp(mic, "3", strlen(mic))) {
        result = qsthwClient->init(THREE_MIC_CMD);
        printf("######## Trying to set 3 Mic Solution\n");
    }
    else {
        result = qsthwClient->init(SIX_MIC_CMD);
        printf("######## Trying to set 6 Mic Solution\n");
    }


    property_get("ro.qc.sdk.fwk.wakeword.enabled", wakeword, "1");
    printf("######## Value of ro.qc.sdk.fwk.wakeword.enabled = %s\n", wakeword);

    if (!strncmp(wakeword, "1", strlen(wakeword))) {
        m_isWakewordEnabled = true;
        printf("######## ENABLE  WAKEWORD \n");
    }
    else {
        m_isWakewordEnabled = false;
        printf("######## DISABLE  WAKEWORD \n");
    }

    property_get("ro.qc.sdk.fwk.esp.enabled", ESP, "1");
    printf("######## Value of ro.qc.sdk.fwk.esp.enabled = %s\n", ESP);

    if (!strncmp(ESP, "1", strlen(ESP))) {
        m_isESPEnabled = true;
        printf("######## ENABLE ESP REPORTING  \n");
    }
    else {
        m_isESPEnabled = false;
        printf("######## DISABLE ESP REPORTING  \n");
    }


    m_isRecognizing = false;

    if (result != 0)
        printf("QTISoundTrigger::initialize - failed\n");

    return result;
}

void QTISoundTrigger::startRecognition() {
    printf("****** QTISoundTrigger::startRecognition() - STHAL Running = %d\n", m_isRecognizing);

    if (m_isRecognizing == false) {
        //restart sthal recognition
        m_isRecognizing = qsthwClient->startRecognition();
        if (m_isRecognizing == false)
            reinitializeSTHal();
    }
}

void QTISoundTrigger::stopRecognition() {
    printf("****** QTISoundTrigger::stopRecognition()  - STHAL Running = %d\n", m_isRecognizing);
    if (m_isRecognizing == true) {
        qsthwClient->stopRecognition();
        m_isRecognizing = false;
    }
}

void QTISoundTrigger::stopRecording() {
    printf("######## QTISoundTrigger::stopRecording() %d\n", m_isRecognizing);
    qsthwClient->doneLookahead();
}

void QTISoundTrigger::shutdown() {

    qsthwClient->shutdown();
}

void QTISoundTrigger::keywordRecognized(std::shared_ptr<AudioInputStream> stream, std::string keyword) {

    printf("\n######################QTISoundTrigger::keywordRecognized()########################## %s\n", keyword.c_str());
    onKeyWordDetected(stream, keyword, KeyWordObserverInterface::UNSPECIFIED_INDEX, KeyWordObserverInterface::UNSPECIFIED_INDEX);
}

void QTISoundTrigger::keywordRecognizedWithIndices(std::shared_ptr<AudioInputStream> stream, std::string keyword, uint32_t startIndex, uint32_t endIndex) {

    printf("\n######################QTISoundTrigger::keywordRecognizedWithIndices()########################## %s, %d, %d\n", keyword.c_str(), startIndex, endIndex);
    onKeyWordDetected(stream, keyword, startIndex, endIndex);
}

void QTISoundTrigger::reinitializeSTHal() {
    printf("######## QTISoundTrigger::reinitializeSTHal() - STHAL Running = %d\n", m_isRecognizing);

    if (m_isRecognizing == false) {
        printf("****** QTISoundTrigger::startRecognition() - FAIL TO RESTART - Try to reinitialize STHAL and Restart recognition ...\n");
        shutdown();
        if (initialize() < 0)
            std::cout << "ERROR: m_stHal->initialize() - FAILED - KEYWORD Engine DEACTIVATED !!!" << std::endl;
        else
            m_isRecognizing = qsthwClient->startRecognition();
    }
}

void QTISoundTrigger::recognitionStateChanged(bool state) {
    printf("QTISoundTrigger::recognitionStateChanged - m_isRecognizing = %d\n", m_isRecognizing);
    m_isRecognizing = state;
}

void QTISoundTrigger::onKeyWordDetected(
        std::shared_ptr<AudioInputStream> stream,
        std::string keyword,
        AudioInputStream::Index beginIndex,
        AudioInputStream::Index endIndex) {

    //beep
    std::cout << "\a" << std::endl;

    m_isRecognizing = false;
    m_detectionCount++;

    printf("QTISoundTrigger::onKeyWordDetected - KEYWORD DETECTED = %d \n", m_detectionCount);

    auto tempM_client = m_client.lock();
    if (tempM_client) {
        AudioInputStream::Index aipBegin = AudioInputProcessor::INVALID_INDEX;
        AudioInputStream::Index aipEnd = AudioInputProcessor::INVALID_INDEX;

        if (endIndex != KeyWordObserverInterface::UNSPECIFIED_INDEX) {
            if (beginIndex != KeyWordObserverInterface::UNSPECIFIED_INDEX) {
                // If we know where the keyword starts and ends, pass both of those along to AIP.
                std::cout << "QTISoundTrigger::onKeyWordDetected WITH INDICES START = " << beginIndex << " END = " << endIndex << std::endl;
                aipBegin = beginIndex;
                aipEnd = endIndex;
            }
            else {
                // If we only know where the keyword ends, AIP should begin recording there.
                aipBegin = endIndex;
            }
        }
        printf("QTISoundTrigger::onKeyWordDetected() - m_client->notifyOfWakeWord() \n");

        //Get DOA Index value and Channel Index;
        m_dOAIndex = qsthwClient->getDOADirection();
        m_channelIndex = qsthwClient->getChannelIndex();

        alexaClientSDK::capabilityAgents::aip::AudioProvider wakeWordAudioProvider(stream, m_compatibleAudioFormat,
            alexaClientSDK::capabilityAgents::aip::ASRProfile::NEAR_FIELD, false, true, true);

        auto tempM_client = m_client.lock();

        float currentESPVoiceEnergy = (float) qsthwClient->getESPVoiceEnergy();
        float currentESPAmbientEnergy =  (float) qsthwClient->getESPAmbientEnergy();

        ESPData espData(std::to_string(currentESPVoiceEnergy), std::to_string(currentESPAmbientEnergy));

        auto startRecognition = false;
        if (m_isWakewordEnabled) {
            printf("######################################_____________WAKEWORD_ENABLED__________________ #############################\n"); 
            std::cout << "####################################_______________WITH INDICES START = " << beginIndex << " END = " << endIndex << std::endl;
            if (m_isESPEnabled) {
                printf("######################################_____________ WITH ESP Data [voiceEnergy = %s, ambientEnergy = %s]\n",
                    espData.getVoiceEnergy().c_str(), espData.getAmbientEnergy().c_str());
                startRecognition = (tempM_client->notifyOfWakeWord(wakeWordAudioProvider, beginIndex, endIndex, keyword, espData)).get();
            }
            else {
                startRecognition = (tempM_client->notifyOfWakeWord(wakeWordAudioProvider, beginIndex, endIndex, keyword)).get();
            }
        }
        else {
            printf("######################################_____________WAKEWORD_DISABLED___________________ #############################\n"); 
            if (m_isESPEnabled) {
                printf("######################################_____________ WITH ESP Data [voiceEnergy = %s, ambientEnergy = %s]\n",
                    espData.getVoiceEnergy().c_str(), espData.getAmbientEnergy().c_str());
                startRecognition = (tempM_client->notifyOfWakeWord(wakeWordAudioProvider, AudioInputProcessor::INVALID_INDEX, AudioInputProcessor::INVALID_INDEX, keyword, espData)).get();
            }
            else {
                startRecognition = (tempM_client->notifyOfWakeWord(wakeWordAudioProvider, AudioInputProcessor::INVALID_INDEX, AudioInputProcessor::INVALID_INDEX, keyword)).get();
            }
        }

        printf("QTISoundTrigger::onKeyWordDetected - startRecognition = %d \n", startRecognition);
        //Restart ST Hal Keyword Recognition and bring Audio input processor to IDLE state if it fails to send Recognize Event.
        if (startRecognition == false) {
            printf("QTISoundTrigger::onKeyWordDetected - startRecognition  FAILED !!!!!");
            qsthwClient->doneLookahead();
            tempM_client->stopForegroundActivity();
        }

    }
}

int QTISoundTrigger::getDOADirection() {
    printf("QTISoundTrigger::getDOADirection() - DOA - m_dOAIndex = %d \n", m_dOAIndex);
    return m_dOAIndex;
}

int QTISoundTrigger::getChannelIndex() {
    printf("QTISoundTrigger::getChannelIndex() - Channel Index - m_channelIndex = %d \n", m_channelIndex);
    return m_channelIndex;
}

} // NS
} // NS
