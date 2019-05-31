/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ALEXA_CLIENT_SDK_QTIAUDIOMANAGER_INCLUDE_AUDIOMANAGER_H_
#define ALEXA_CLIENT_SDK_QTIAUDIOMANAGER_INCLUDE_AUDIOMANAGER_H_

#include <gst/gstmediarecorder.h>

#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <limits.h>

#include <AVSCommon/Utils/Logger/LogEntry.h>
#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/AVS/AudioInputStream.h>
#include <AVSCommon/Utils/SDS/SharedDataStream.h>
#include <AVSCommon/Utils/AudioFormat.h>
#include <QSTHWManager/QSTHWInterface.h>
#include <AIP/AudioInputProcessor.h>
#include <AIP/ASRProfile.h>
#include <AIP/AudioProvider.h>
#include <AIP/Initiator.h>
#include <DefaultClient/DefaultClient.h>
#include <AVSCommon/SDKInterfaces/KeyWordObserverInterface.h>
#include <MediaPlayer/MediaPlayer.h>

namespace alexaClientSDK {
namespace qahwManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::capabilityAgents::aip;
using namespace alexaClientSDK::avsCommon::sdkInterfaces;
using namespace alexaClientSDK::avsCommon::avs;
using namespace alexaClientSDK::avsCommon::utils;

/**
 * Class that handles interaction to audio HAL
 */
class QAHWManager {

public:
    QAHWManager(
            std::shared_ptr<avsCommon::avs::AudioInputStream> stream,
            std::shared_ptr<defaultClient::DefaultClient> client,
            capabilityAgents::aip::AudioProvider audioProvider,
            std::shared_ptr<alexaClientSDK::QSTHWManager::QSTHWInterface> stHal);

    ~QAHWManager();

    void startRecording(int channelIndex);
    void stopRecording();

    static std::unique_ptr<AudioInputStream::Writer> m_audioBufferWriter;
    static std::shared_ptr<AudioInputStream> m_audioBuffer;
    std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer> m_buffer;

private:
    static void GetBufferEvent(guint8 * buf, guint size, gint streamid);
    static void RecordFailedEvent(gint errorType, gint streamid);
    int create_buffer_stream();

    std::shared_ptr<avsCommon::avs::AudioInputStream> m_stream;
    capabilityAgents::aip::AudioProvider m_audioProvider;
    std::weak_ptr<alexaClientSDK::defaultClient::DefaultClient> m_client;
    std::shared_ptr<alexaClientSDK::QSTHWManager::QSTHWInterface> m_stHal;
    bool m_isRecording;
    MediaRecorder* ptr;
    static FILE *fp;
    AudioFormat m_compatibleAudioFormat;

};

}
}

#endif /* CAPABILITYAGENTS_SPEECHSYNTHESIZER_INCLUDE_AUDIOMANAGER_H_ */
