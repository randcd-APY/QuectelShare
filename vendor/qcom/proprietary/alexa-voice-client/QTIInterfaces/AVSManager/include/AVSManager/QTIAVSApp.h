/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_QTIAVSAPP_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_QTIAVSAPP_H_

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <cctype>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>

#include <AVSCommon/SDKInterfaces/KeyWordObserverInterface.h>
#include <AVSCommon/SDKInterfaces/KeyWordDetectorStateObserverInterface.h>
#include <AVSCommon/AVS/Initialization/AlexaClientSDKInit.h>
#include <AVSCommon/Utils/Configuration/ConfigurationNode.h>
#include <AVSCommon/Utils/LibcurlUtils/HTTPContentFetcherFactory.h>
#include <AuthDelegate/AuthDelegate.h>
#include <Audio/AudioFactory.h>
#include <AVSCommon/Utils/Logger/ConsoleLogger.h>
#include <DefaultClient/DefaultClient.h>
#include <QAHWManager/QAHWManager.h>
#include <MediaPlayer/MediaPlayer.h>
#include <QSTHWManager/QTISoundTrigger.h>
#include <Alerts/Storage/SQLiteAlertStorage.h>
#include <Notifications/SQLiteNotificationsStorage.h>
#include <Settings/SQLiteSettingStorage.h>
#include <AVSManager/SampleApp/UserInputManager.h>
#include <AVSManager/SampleApp/InteractionManager.h>
#include <AVSManager/SampleApp/ConsolePrinter.h>
#include <AVSManager/SampleApp/UIManager.h>
#include <AVSManager/SampleApp/KeywordObserver.h>
#include <AVSManager/SampleApp/ConnectionObserver.h>
#include <AVSManager/SampleApp/GuiRenderer.h>
#include <AVSManager/KeyEventManager.h>
#include <AVSManager/TimeManager.h>
#include <AVSCommon/Utils/Configuration/ConfigurationNode.h>
#include <AVSCommon/Utils/LibcurlUtils/HTTPContentFetcherFactory.h>
#include <AVSCommon/Utils/Logger/LoggerSinkManager.h>

namespace alexaClientSDK {
namespace avsManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::QSTHWManager;
using namespace alexaClientSDK::qahwManager;
using namespace alexaClientSDK::avsCommon::utils::mediaPlayer;
using namespace alexaClientSDK::mediaPlayer;

class QTIAVSApp {

public:
    static std::unique_ptr<QTIAVSApp> create();
    void initialize();
    void run();
    /// Destructor .
    ~QTIAVSApp();

private:

    bool setUpSDK();
    void setupSystemTime();
    void validateCredentials();
    void waitForNetwork();
    std::shared_ptr<DirectiveSequencerInterface> m_directiveSequencer;
    std::shared_ptr<LEDManager> m_ledManager;

    KeyEventManager* m_KeyEventManager;

#ifdef GSTREAMER_MEDIA_PLAYER
    std::shared_ptr<MediaPlayer> m_speakerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_audioPlayerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_mp3PlayerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_alertsMediaPlayer;
    std::shared_ptr<MediaPlayer> m_notificationsMediaPlayer;

#else
    std::shared_ptr<MediaPlayer> m_speakerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_audioPlayerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_mp3PlayerMediaPlayer;
    std::shared_ptr<MediaPlayer> m_alertsMediaPlayer;
    std::shared_ptr<MediaPlayer> m_notificationsMediaPlayer;
#endif

    std::shared_ptr<alexaClientSDK::authDelegate::AuthDelegate> m_authDelegate;
    std::shared_ptr<alexaClientSDK::avsManager::ConnectionObserver> m_connectionObserver;
    std::shared_ptr<alexaClientSDK::avsManager::UIManager> m_userInterfaceManager;
    std::shared_ptr<alexaClientSDK::capabilityAgents::alerts::storage::SQLiteAlertStorage> m_alertStorage;
    std::shared_ptr<alexaClientSDK::certifiedSender::SQLiteMessageStorage> m_messageStorage;
    std::shared_ptr<alexaClientSDK::capabilityAgents::notifications::SQLiteNotificationsStorage> m_notificationStorage;
    std::shared_ptr<alexaClientSDK::capabilityAgents::settings::SQLiteSettingStorage> m_settingsStorage;
    std::shared_ptr<avsCommon::utils::libcurlUtils::HTTPContentFetcherFactory> m_httpContentFetcherFactory;
    AudioFormat m_compatibleAudioFormat;
    size_t m_bufferSize;
    std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer> m_buffer;
    std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream> m_sharedDataStream;

    std::shared_ptr<alexaClientSDK::QSTHWManager::QTISoundTrigger> m_stHal;
    std::shared_ptr<alexaClientSDK::qahwManager::QAHWManager> m_audioHal;

    std::shared_ptr<alexaClientSDK::defaultClient::DefaultClient> m_client;
    std::shared_ptr<alexaClientSDK::avsManager::InteractionManager> m_interactionManager;
    std::unique_ptr<alexaClientSDK::avsManager::UserInputManager> m_userInputManager;

    std::unordered_map<std::string, std::shared_ptr<avsCommon::utils::mediaPlayer::MediaPlayerInterface>>
        m_externalMusicProviderMediaPlayersMap;

    /// The singleton map from @c playerId to @c ExternalMediaAdapter creation functions.
    static capabilityAgents::externalMediaPlayer::ExternalMediaPlayer::AdapterCreationMap m_adapterToCreateFuncMap;
}
;

}
}

#endif
