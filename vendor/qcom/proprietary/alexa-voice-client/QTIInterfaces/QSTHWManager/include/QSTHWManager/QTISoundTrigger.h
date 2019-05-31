/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ALEXA_CLIENT_SDK_QTI_SOUND_TRIGGER_H_
#define ALEXA_CLIENT_SDK_QTI_SOUND_TRIGGER_H_

#include <memory>
#include <stdio.h>
#include <memory>
#include <ostream>
#include <thread>
#include <iostream>
#include <limits.h>

#include <AVSCommon/SDKInterfaces/KeyWordObserverInterface.h>
#include <AVSCommon/AVS/AudioInputStream.h>
#include <AVSCommon/Utils/SDS/SharedDataStream.h>
#include <AVSCommon/Utils/AudioFormat.h>
#include <QSTHWManager/QSTHWInterface.h>
#include <QSTHWManager/QSTHWClient.h>
#include <QSTHWManager/QSTHWCallbackIF.h>
#include <AIP/AudioInputProcessor.h>
#include <AIP/ASRProfile.h>
#include <AIP/AudioProvider.h>
#include <AIP/Initiator.h>
#include <DefaultClient/DefaultClient.h>
#include <AIP/ESPData.h>

namespace alexaClientSDK {
namespace QSTHWManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::capabilityAgents::aip;
using namespace alexaClientSDK::avsCommon::sdkInterfaces;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;
using namespace alexaClientSDK::QSTHWManager;

class QTISoundTrigger: public KeyWordObserverInterface, public QSTHWCallbackIF, public QSTHWInterface, public std::enable_shared_from_this<
        QTISoundTrigger> {
public:
    QTISoundTrigger(
            std::shared_ptr<avsCommon::avs::AudioInputStream> stream,
            std::shared_ptr<defaultClient::DefaultClient> client,
            capabilityAgents::aip::AudioProvider audioProvider);

    virtual ~QTISoundTrigger();

    virtual void startRecognition();

    virtual void stopRecognition();

    virtual void stopRecording();

    virtual void shutdown();

    virtual int initialize();

    void onKeyWordDetected(
            std::shared_ptr<AudioInputStream> stream,
            std::string keyword,
            AudioInputStream::Index beginIndex,
            AudioInputStream::Index endIndex);

    virtual void keywordRecognized(std::shared_ptr<AudioInputStream> stream, std::string keyword);

    virtual void keywordRecognizedWithIndices(std::shared_ptr<AudioInputStream> stream, std::string keyword, uint32_t startIndex, uint32_t endIndex);

    virtual void recognitionStateChanged(bool state);

    virtual void reinitializeSTHal();

    virtual int getDOADirection();
    virtual int getChannelIndex();

    int m_detectionCount;
    static bool m_isRecognizing;

private:
    std::shared_ptr<avsCommon::avs::AudioInputStream> m_stream;
    std::shared_ptr<AudioInputProcessor> m_aip;
    capabilityAgents::aip::AudioProvider m_audioProvider;
    AudioFormat m_compatibleAudioFormat;
    std::weak_ptr<alexaClientSDK::defaultClient::DefaultClient> m_client;
    int m_dOAIndex;
    int m_channelIndex;
    bool m_isWakewordEnabled;
    bool m_isESPEnabled;
    QSTHWClient* qsthwClient;
};
}
}

#endif
