/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <AVSManager/QTIAVSApp.h>
#include <curl/curl.h>

namespace alexaClientSDK {
namespace avsManager {

using namespace alexaClientSDK;
using namespace alexaClientSDK::QSTHWManager;
using namespace alexaClientSDK::qahwManager;
using namespace alexaClientSDK::avsCommon::utils::mediaPlayer;
using namespace alexaClientSDK::mediaPlayer;
using namespace alexaClientSDK::capabilityAgents::externalMediaPlayer;

std::unordered_map<std::string, ExternalMediaPlayer::AdapterCreateFunction> QTIAVSApp::m_adapterToCreateFuncMap;

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
static const size_t BUFFER_SIZE_IN_SAMPLES = (SAMPLE_RATE_HZ) * AMOUNT_OF_AUDIO_DATA_IN_BUFFER.count();

/// Key for the root node value containing configuration values for SampleApp.
static const std::string SAMPLE_APP_CONFIG_KEY("sampleApp");

/// Key for the endpoint value under the @c SAMPLE_APP_CONFIG_KEY configuration node.
static const std::string ENDPOINT_KEY("endpoint");

/// Key for setting if display cards are supported or not under the @c SAMPLE_APP_CONFIG_KEY configuration node.
static const std::string DISPLAY_CARD_KEY("displayCardsSupported");

/// Path to configuration file (from command line arguments).
std::string configPathJSON = "/etc/alexa/AlexaClientSDKConfig.json";

#define HOST_ADDR "www.developer.amazon.com"

//FIFO pipe for JSON file creation
#define BUF_SIZE 2048
char rd_buffer[BUF_SIZE];
const char *pipe_file = "/run/avs.fifo";
const char *conf_file = configPathJSON.c_str();
std::string alertString(
        ",\n\"alertsCapabilityAgent\": {\n    \"databaseFilePath\":\"/data/alerts.db\",\n    \"alarmSoundFilePath\" : \"/etc/alexa/med_system_alerts_melodic_01._TTH_.mp3\",\n    \"alarmShortSoundFilePath\" : \"/etc/alexa/med_system_alerts_melodic_01_short._TTH_.wav\",\n    \"timerSoundFilePath\" : \"/etc/alexa/med_system_alerts_melodic_02._TTH_.mp3\",\n    \"timerShortSoundFilePath\" : \"/etc/alexa/med_system_alerts_melodic_02_short._TTH_.wav\"\n}\n,\n\"settings\": {\n    \"databaseFilePath\":\"/data/settingsca.db\",\n    \"defaultAVSClientSettings\": {\n         \"locale\":\"en-US\"\n    }\n}\n,\n\"notifications\":{\n    \"databaseFilePath\":\"/data/notifications.db\"\n}\n,\n\"sampleApp\":{\n    \"displayCardsSupported\":false\n}\n,\n\"certifiedSender\":{\n    \"databaseFilePath\":\"/data/certifiedsender.db\"\n}\n}\n");

/// A set of all log levels.

static const std::set<alexaClientSDK::avsCommon::utils::logger::Level> allLevels = {
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG9,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG8,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG7,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG6,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG5,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG4,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG3,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG2,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG1,
    alexaClientSDK::avsCommon::utils::logger::Level::DEBUG0,
    alexaClientSDK::avsCommon::utils::logger::Level::INFO,
    alexaClientSDK::avsCommon::utils::logger::Level::WARN,
    alexaClientSDK::avsCommon::utils::logger::Level::ERROR,
    alexaClientSDK::avsCommon::utils::logger::Level::CRITICAL,
    alexaClientSDK::avsCommon::utils::logger::Level::NONE};

/**
 * Gets a log level consumable by the SDK based on the user input string for log level.
 *
 * @param userInputLogLevel The string to be parsed into a log level.
 * @return The log level. This will default to NONE if the input string is not properly parsable.
 */
static alexaClientSDK::avsCommon::utils::logger::Level getLogLevelFromUserInput(std::string userInputLogLevel) {
    std::transform(userInputLogLevel.begin(), userInputLogLevel.end(), userInputLogLevel.begin(), ::toupper);
    return alexaClientSDK::avsCommon::utils::logger::convertNameToLevel(userInputLogLevel);
}


std::unique_ptr<QTIAVSApp> QTIAVSApp::create() {
    return std::unique_ptr<QTIAVSApp>(new QTIAVSApp);
}

QTIAVSApp::~QTIAVSApp() {
    // First clean up anything that depends on the the MediaPlayers.
    m_userInputManager.reset();

    // Now it's safe to shut down the MediaPlayers.
    m_speakerMediaPlayer->shutdown();
    m_audioPlayerMediaPlayer->shutdown();
    m_alertsMediaPlayer->shutdown();
}

void QTIAVSApp::initialize() {

    alexaClientSDK::avsManager::ConsolePrinter::simplePrint("QTIAVSApp::initialize()");

    //Create LEDManager
    m_ledManager = std::make_shared<alexaClientSDK::avsManager::LEDManager>();

    m_ledManager->setLEDRingOn(RED_LED);

    //Check AVS SDK Credentials
    validateCredentials();

    //wait for network
    waitForNetwork();

    //setup system time
    setupSystemTime();

    //setup AVS SDK
    //TODO: EXIT APP if SDK SetUp FAils
    if (!setUpSDK()) {
        std::cout << "############ ERROR During SDK Initialization - Exiting Application #########" << std::endl;
        exit(0);
    }

    m_ledManager->setLEDRingOn(GREEN_LED);

}

void QTIAVSApp::waitForNetwork() {
    CURL *curl = NULL;
    CURLcode ret;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, HOST_ADDR);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

        do {
            ret = curl_easy_perform(curl);
            if(ret == CURLE_OK) break;
            std::cout << "Waiting for network" << std::endl;
            sleep(5);
        }while(ret != CURLE_OK);

        curl_easy_cleanup(curl);
    }

    return;
}

void QTIAVSApp::validateCredentials() {

    if (access(conf_file, F_OK) != -1) {
        std::ifstream alexaCfg(conf_file, std::ifstream::in);
        if(alexaCfg.peek() != EOF) {
          // file exists, proceed with sdk setup
          alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Valid JSON file exists, proceed with AVS SDK setup");
          return;
        }
    }

    //Create the FIFO named pipe with read/write permissions
    int fd = -1;
    int res = mkfifo(pipe_file, 0666);

    //Set LEDS to RED to indicate that onboarding procedure is needed.
    m_ledManager->setLEDRingOn(RED_LED);

    printf("mkfifo returned %d\n", res);

    fd = open(pipe_file, O_RDONLY);

    memset(rd_buffer, 0, sizeof(rd_buffer));

    printf("pipe open, waiting for read, fd = %d\n", fd);

    int bytesRead = read(fd, rd_buffer, sizeof(rd_buffer));

    printf("QTIAVSApp::validateCredentials() read %d bytes from fifo\n", bytesRead);

    if (bytesRead > 0) {   // -1 => error, 0 => pipe closed

        // update the read bytes with the addon snippet for Alarms configuration
        char * pch;
        pch = strrchr(rd_buffer, '}');
        memcpy(pch, alertString.c_str(), alertString.length());

        int pos = pch - rd_buffer;
        int totalLen = pos + alertString.length() + 1;

        // create and write to conf_file
        int fdo = open(conf_file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

        if (fdo > 0) {

            printf("writing to %s \n", conf_file);

            write(fdo, rd_buffer, totalLen);
            close(fdo);

            printf("done writing %s \n", conf_file);
        }
        else {
            printf("failed writing %s \n", conf_file);
        }
    }

    //Turn off LEDs as onboarding is completed.
    m_ledManager->setLEDRingOff();
}

void QTIAVSApp::run() {

    printf("Entering run() of QTIAVSApp\n");
    alexaClientSDK::avsManager::ConsolePrinter::simplePrint(
            "QTIAVSApp:: run() - Running Alexa APP with log level: "
                    + alexaClientSDK::avsCommon::utils::logger::convertLevelToName(alexaClientSDK::avsCommon::utils::logger::Level::DEBUG0));

    //Configure and Start Keyword Detection
    m_stHal->startRecognition();

    m_KeyEventManager = new KeyEventManager();
    m_KeyEventManager->run(m_userInterfaceManager, m_interactionManager);

    //ShutDown AVS APP
    m_userInterfaceManager->shutdown();

    alexaClientSDK::avsManager::ConsolePrinter::simplePrint("QTIAVSApp::run() - Done!!");

}


//TODO Merge InteractionManager with UIManager
bool QTIAVSApp::setUpSDK() {
    alexaClientSDK::avsManager::ConsolePrinter::simplePrint("QTIAVSApp::SetUp() ");

    /*
     * Set up the SDK logging system to write to the SampleApp's ConsolePrinter.  Also adjust the logging level
     * if requested.
     */
    std::shared_ptr<alexaClientSDK::avsCommon::utils::logger::Logger> consolePrinter = std::make_shared<alexaClientSDK::avsManager::ConsolePrinter>();

    auto logLevelValue = alexaClientSDK::avsCommon::utils::logger::Level::DEBUG9;
    if (alexaClientSDK::avsCommon::utils::logger::Level::UNKNOWN == logLevelValue) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Unknown log level input!");
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Possible log level options are: ");
        for (auto it = allLevels.begin(); it != allLevels.end(); ++it) {
            alexaClientSDK::avsManager::ConsolePrinter::simplePrint(alexaClientSDK::avsCommon::utils::logger::convertLevelToName(*it));
        }
        return false;
    }
    alexaClientSDK::avsManager::ConsolePrinter::simplePrint(
            "Running app with log level: " + alexaClientSDK::avsCommon::utils::logger::convertLevelToName(logLevelValue));
    consolePrinter->setLevel(logLevelValue);
    alexaClientSDK::avsCommon::utils::logger::LoggerSinkManager::instance().initialize(consolePrinter);

    std::ifstream configInfile(configPathJSON);
    if (!configInfile.good()) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to read config file!");
        return false;
    }

    /*
     * This is a required step upon startup of the SDK before any modules are created. For that reason, it is being
     * called here, before creating the MediaPlayer, audio streams, DefaultClient, etc.
     */
    if (!avsCommon::avs::initialization::AlexaClientSDKInit::initialize( { &configInfile })) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to initialize SDK!");
        return false;
    }

    auto config = alexaClientSDK::avsCommon::utils::configuration::ConfigurationNode::getRoot();
    m_httpContentFetcherFactory = std::make_shared<avsCommon::utils::libcurlUtils::HTTPContentFetcherFactory>();

    /*
     * Creating the media players. Here, the default GStreamer based MediaPlayer is being created. However, any
     * MediaPlayer that follows the specified MediaPlayerInterface can work.
     */
#ifdef GSTREAMER_MEDIA_PLAYER
    m_speakerMediaPlayer = MediaPlayer::create(m_httpContentFetcherFactory, avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED, "SpeakMediaPlayer");
    if (!m_speakerMediaPlayer) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create media player for speech!");
        return false;
    }
    m_audioPlayerMediaPlayer = MediaPlayer::create(m_httpContentFetcherFactory, avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED, "AudioMediaPlayer");
    if (!m_audioPlayerMediaPlayer) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create media player for content!");
        return false;
    }
    m_notificationsMediaPlayer = alexaClientSDK::mediaPlayer::MediaPlayer::create(m_httpContentFetcherFactory,
        avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED,
        "NotificationsMediaPlayer");
    if (!m_notificationsMediaPlayer) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create media player for notifications!");
        return false;
    }
    m_mp3PlayerMediaPlayer = MediaPlayer::create(m_httpContentFetcherFactory, avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED, "mp3MediaPlayer");
    if (!m_mp3PlayerMediaPlayer) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create media player for Local MP3!");
        return false;
    }
    /*
     * The ALERTS speaker type will cause volume control to be independent and localized. By assigning this type,
     * Alerts volume/mute changes will not be in sync with AVS. No directives or events will be associated with volume
     * control.
     */
    m_alertsMediaPlayer = MediaPlayer::create(m_httpContentFetcherFactory, avsCommon::sdkInterfaces::SpeakerInterface::Type::LOCAL, "AlertsMediaPlayer");
    if (!m_alertsMediaPlayer) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create media player for alerts!");
        return false;
    }
#else
    auto m_speakerMediaPlayer = EmptyMediaPlayer::create();
    auto m_audioPlayerMediaPlayer = EmptyMediaPlayer::create();
    auto m_mp3PlayerMediaPlayer = EmptyMediaPlayer::create();
    auto m_alertsMediaPlayer = EmptyMediaPlayer::create();
    auto m_notificationsMediaPlayer = EmptyMediaPlayer::create();
#endif

    /*
     * Create Speaker interfaces to control the volume. For the SDK, the MediaPlayer happens to also provide
     * volume control functionality, but this does not have to be case.
     */
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface> speakSpeaker = std::static_pointer_cast<
            alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface > (m_speakerMediaPlayer);
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface> audioSpeaker = std::static_pointer_cast<
            alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface > (m_audioPlayerMediaPlayer);
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface> alertsSpeaker = std::static_pointer_cast<
            alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface > (m_alertsMediaPlayer);
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface> notificationsSpeaker =
        std::static_pointer_cast<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface>(
            m_notificationsMediaPlayer);

    auto audioFactory = std::make_shared<alexaClientSDK::applicationUtilities::resources::audio::AudioFactory>();

    // Creating the alert storage object to be used for rendering and storing alerts.
    auto alertStorage = alexaClientSDK::capabilityAgents::alerts::storage::SQLiteAlertStorage::create(config, audioFactory->alerts());
    m_alertStorage = std::move(alertStorage);

    auto messageStorage = alexaClientSDK::certifiedSender::SQLiteMessageStorage::create(config);
    m_messageStorage = std::move(messageStorage);

    auto notificationsStorage = alexaClientSDK::capabilityAgents::notifications::SQLiteNotificationsStorage::create(config);
    m_notificationStorage = std::move(notificationsStorage);

    /*
     * Creating settings storage object to be used for storing <key, value> pairs of AVS Settings.
     */
    auto settingsStorage = alexaClientSDK::capabilityAgents::settings::SQLiteSettingStorage::create(config);
    m_settingsStorage = std::move(settingsStorage);

    /*
     * Creating the UI component that observes various components and prints to the console accordingly.
     */
    m_userInterfaceManager = std::make_shared<alexaClientSDK::avsManager::UIManager>();

    /*
     * Setting up a connection observer to wait for connection and authorization prior to accepting user input at
     * startup.
     */
    m_connectionObserver = std::make_shared<alexaClientSDK::avsManager::ConnectionObserver>();

    /*
     * Creating the AuthDelegate - this component takes care of LWA and authorization of the client. At the moment,
     * this must be done and authorization must be achieved prior to making the call to connect().
     */
    m_authDelegate = alexaClientSDK::authDelegate::AuthDelegate::create();
    m_authDelegate->addAuthObserver(m_connectionObserver);


    int firmwareVersion = static_cast<int>(avsCommon::sdkInterfaces::softwareInfo::INVALID_FIRMWARE_VERSION);
    std::vector<std::shared_ptr<avsCommon::sdkInterfaces::SpeakerInterface>> additionalSpeakers;

    /*
     * Creating the DefaultClient - this component serves as an out-of-box default object that instantiates and "glues"
     * together all the modules.
     */
    m_client = alexaClientSDK::defaultClient::DefaultClient::create(
            m_externalMusicProviderMediaPlayersMap,
            m_adapterToCreateFuncMap,
            m_speakerMediaPlayer,
            m_audioPlayerMediaPlayer,
            m_alertsMediaPlayer,
            m_notificationsMediaPlayer,
            speakSpeaker,
            audioSpeaker,
            alertsSpeaker,
            notificationsSpeaker,
            additionalSpeakers,
            audioFactory,
            m_authDelegate,
            m_alertStorage,
            m_messageStorage,
            m_notificationStorage,
            m_settingsStorage,
            { m_userInterfaceManager },
            { m_connectionObserver, m_userInterfaceManager },
            false,
            firmwareVersion,
            true,
            nullptr);

    if (!m_client) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create default SDK client!");
        return false;
    }

    /*
     * TODO: ACSDK-384 Remove the requirement of clients having to wait for authorization before making the connect()
     * call.
     */
    if (!m_connectionObserver->waitFor(alexaClientSDK::avsCommon::sdkInterfaces::AuthObserverInterface::State::REFRESHED)) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to authorize SDK client!");
        return false;
    }

    std::string endpoint;
    config[SAMPLE_APP_CONFIG_KEY].getString(ENDPOINT_KEY, &endpoint);

    m_client->connect(endpoint);

    if (!m_connectionObserver->waitFor(avsCommon::sdkInterfaces::ConnectionStatusObserverInterface::Status::CONNECTED)) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to connect to AVS!");
        return false;
    }

    // Add userInterfaceManager as observer of locale setting.
    m_client->addSettingObserver("locale", m_userInterfaceManager);
    // Send default settings set by the user to AVS.
    m_client->sendDefaultSettings();

    m_client->addSpeakerManagerObserver(m_userInterfaceManager);

    /*
     * Add GUI Renderer as an observer if display cards are supported.  The default is supported unless specified
     * otherwise in the configuration.
     */
    bool displayCardsSupported;
    config[SAMPLE_APP_CONFIG_KEY].getBool(DISPLAY_CARD_KEY, &displayCardsSupported, false);
    if (displayCardsSupported) {
        auto guiRenderer = std::make_shared<GuiRenderer>();
        m_client->addTemplateRuntimeObserver(guiRenderer);
    }

    /*
     * Creating the buffer (Shared Data Stream) that will hold user audio data. This is the main input into the SDK.
     */
    m_bufferSize = alexaClientSDK::avsCommon::avs::AudioInputStream::calculateBufferSize(BUFFER_SIZE_IN_SAMPLES, WORD_SIZE, MAX_READERS);
    m_buffer = std::make_shared<alexaClientSDK::avsCommon::avs::AudioInputStream::Buffer>(m_bufferSize);
    m_sharedDataStream = alexaClientSDK::avsCommon::avs::AudioInputStream::create(m_buffer, WORD_SIZE, MAX_READERS);

    if (!m_sharedDataStream) {
        alexaClientSDK::avsManager::ConsolePrinter::simplePrint("Failed to create shared data stream!");
    }

    m_compatibleAudioFormat.sampleRateHz = SAMPLE_RATE_HZ;
    m_compatibleAudioFormat.sampleSizeInBits = WORD_SIZE * CHAR_BIT;
    m_compatibleAudioFormat.numChannels = NUM_CHANNELS;
    m_compatibleAudioFormat.endianness = alexaClientSDK::avsCommon::utils::AudioFormat::Endianness::LITTLE;
    m_compatibleAudioFormat.encoding = alexaClientSDK::avsCommon::utils::AudioFormat::Encoding::LPCM;

    /*
     * Creating each of the audio providers. An audio provider is a simple package of data consisting of the stream
     * of audio data, as well as metadata about the stream. For each of the three audio providers created here, the same
     * stream is used since this sample application will only have one microphone.
     */

    // Creating tap to talk audio provider
    bool tapAlwaysReadable = false; //QTI was:true;
    bool tapCanOverride = true;
    bool tapCanBeOverridden = true;

    alexaClientSDK::capabilityAgents::aip::AudioProvider tapToTalkAudioProvider(m_sharedDataStream, m_compatibleAudioFormat,
            alexaClientSDK::capabilityAgents::aip::ASRProfile::NEAR_FIELD, tapAlwaysReadable, tapCanOverride, tapCanBeOverridden);

    // Creating hold to talk audio provider
    bool holdAlwaysReadable = false;
    bool holdCanOverride = true;
    bool holdCanBeOverridden = false;

    alexaClientSDK::capabilityAgents::aip::AudioProvider holdToTalkAudioProvider(m_sharedDataStream, m_compatibleAudioFormat,
            alexaClientSDK::capabilityAgents::aip::ASRProfile::CLOSE_TALK, holdAlwaysReadable, holdCanOverride, holdCanBeOverridden);

    // Creating wake word audio provider, if necessary
    bool wakeAlwaysReadable = false; //QTI was:true;
    bool wakeCanOverride = true; //QTI was:false;
    bool wakeCanBeOverridden = true;

    alexaClientSDK::capabilityAgents::aip::AudioProvider wakeWordAudioProvider(m_sharedDataStream, m_compatibleAudioFormat,
            alexaClientSDK::capabilityAgents::aip::ASRProfile::NEAR_FIELD, wakeAlwaysReadable, wakeCanOverride, wakeCanBeOverridden);

    m_interactionManager = std::make_shared<alexaClientSDK::avsManager::InteractionManager>(m_client, m_userInterfaceManager, holdToTalkAudioProvider,
            tapToTalkAudioProvider, wakeWordAudioProvider);

    m_stHal = std::make_shared<QTISoundTrigger>(m_sharedDataStream, m_client, wakeWordAudioProvider);
    if (m_stHal->initialize() < 0)
        std::cout << "ERROR: m_stHal->initialize() - FAILED" << std::endl;

    m_audioHal = std::make_shared<QAHWManager>(m_sharedDataStream, m_client, tapToTalkAudioProvider, m_stHal);

    m_userInterfaceManager->addQTIInterfaces(m_stHal, m_audioHal, m_ledManager);

    return true;

}

void QTIAVSApp::setupSystemTime() {
    TimeManager timeManager;

    std::cout << "Sync system time to network time" << std::endl;
    timeManager.handleSystemTime();
}

}
}

