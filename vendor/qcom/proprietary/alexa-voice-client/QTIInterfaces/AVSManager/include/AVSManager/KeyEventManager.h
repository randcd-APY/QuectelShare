/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*************************************************************
*  @file    KeyEventManager.h
*  @brief   Handler for key events
*
*  DESCRIPTION
*    Handles key pres and release events
***************************************************************/

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_KEYEVENTMANAGER_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_KEYEVENTMANAGER_H_

#define __STDC_FORMAT_MACROS 1
#define __STDC_LIMIT_MACROS 1

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <MediaPlayer/MediaPlayer.h>
#include <AVSManager/LEDManager.h>
#include <AVSManager/SampleApp/UIManager.h>
#include <AVSManager/SampleApp/InteractionManager.h>

#define KEY_EVTMGR_LOG_FUNC_ENTRY()   (std::cout << __func__ << ": function entry" << std::endl)
#define KEY_EVTMGR_LOG_FUNC_RETURN()  (std::cout << __func__ << ": function exit" << std::endl)
#define BILLION 1E9
#define VOCAL 1
#define MUTED 0


using namespace std;
using namespace alexaClientSDK::mediaPlayer;
using namespace alexaClientSDK::avsManager;

extern "C" {
#define _GNU_SOURCE
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <libinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <poll.h>
#include <input_helper.h>
#include <signal.h>
#include <time.h>
}

class KeyEventManager
{
  public:
    void handleKeyEvent(struct libinput_event *);
    KeyEventManager();
    void run(std::shared_ptr<alexaClientSDK::avsManager::UIManager>,
             std::shared_ptr<alexaClientSDK::avsManager::InteractionManager>);

  private:
    LEDManager m_ledManager;
    void keyPressEventHandler(const char *, enum libinput_key_state);
    void keyReleaseEventHandler(const char *, enum libinput_key_state);
    void list_all_devices(struct libinput *);
    void setMuteStatus();
    int getMuteStatus();
    int audio_state=1;
    bool m_mute;
    std::shared_ptr<alexaClientSDK::avsManager::UIManager> m_userInterfaceManager;
    std::shared_ptr<alexaClientSDK::avsManager::InteractionManager> m_interactionManager;
};

extern "C" {
void keyEventManagerCallBack(struct libinput_event *);
} // end of extern "C"

#endif //AVSMANAGER_INCLUDE_AVSMANAGER_KEYEVENTMANAGER_H_
