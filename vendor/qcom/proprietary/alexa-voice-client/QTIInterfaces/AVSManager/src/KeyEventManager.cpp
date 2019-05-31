/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*************************************************************
*  @file    KeyEventManager.cpp
*  @brief   Handler for key events
*
*  DESCRIPTION
*    Handles key pres and release events
***************************************************************/

#define __STDC_FORMAT_MACROS 1
#define __STDC_LIMIT_MACROS 1

#include "AVSManager/KeyEventManager.h"
#include "AVSManager/LEDManager.h"
//===========================================================================
// KeyEventManager::KeyEventManager
//===========================================================================
using namespace alexaClientSDK;

KeyEventManager::KeyEventManager()
{
m_mute=false;
} // KeyEventManager::KeyEventManager

//===========================================================================
// KeyEventManager::run
//===========================================================================
void KeyEventManager::run(std::shared_ptr<alexaClientSDK::avsManager::UIManager>  userInterfaceManager_ptr,
                          std::shared_ptr<alexaClientSDK::avsManager::InteractionManager> interfactionManager_ptr)
{
   KEY_EVTMGR_LOG_FUNC_ENTRY();
   m_userInterfaceManager = userInterfaceManager_ptr;
   m_interactionManager = interfactionManager_ptr;
   input_helper_instance ghelper;
   struct pollfd fds;

   memset(&ghelper, 0, sizeof(input_helper_instance));

   ghelper.cb_func = &keyEventManagerCallBack;
   ghelper.userdata = this;

   input_helper_create_instance(&ghelper);

   list_all_devices(ghelper.linput);

   fds.fd = libinput_get_fd(ghelper.linput);
   fds.events = POLLIN;
   fds.revents = 0;

   while (poll(&fds, 1, -1) > -1)
      input_helper_handle_key_events(&ghelper);

   input_helper_release_instance(&ghelper);

    KEY_EVTMGR_LOG_FUNC_RETURN();
} // KeyEventManager::run

//===========================================================================
// KeyEventManager::run
//===========================================================================
int kpset =0,krset=0;
double t_elapsed;

struct timespec kptime, krtime;

void KeyEventManager::keyPressEventHandler(const char *key_name, enum libinput_key_state state) {

    //kptime = 1000 * kp.tv_sec + kp.tv_usec / 1000; //time in miliseconds  34
    cout <<"KEY_NAME is ========= " << key_name << endl;
    if(strcmp(key_name,"KEY_MICMUTE")==0 && state == LIBINPUT_KEY_STATE_PRESSED){
        if (!m_mute) {
           m_ledManager.setLEDRingOff();
        }
    }
    if(strcmp(key_name,"KEY_POWER")==0 && state == LIBINPUT_KEY_STATE_PRESSED){
        cout<<"######### KEY_POWER PRESSED ######"<<endl;
        clock_gettime(CLOCK_REALTIME, &kptime);
        kpset=1;
    }

    return;
}


void KeyEventManager::keyReleaseEventHandler(const char *key_name, enum libinput_key_state state){
    cout <<"KEY_NAME is ========= " << key_name << endl;
#ifdef HWMSM8909
    if(strcmp(key_name,"KEY_MICMUTE")==0 && state == LIBINPUT_KEY_STATE_RELEASED){
        if(!m_mute){
            cout<<"######### MUTE MIC ######"<<endl;
            system("echo 92 > /sys/class/gpio/export");
            system("echo \"out\" > /sys/class/gpio/gpio92/direction");
            system("echo 0 > /sys/class/gpio/gpio92/value");
            m_mute=true;
            m_ledManager.setLEDRingOn(RED_LED);
        }else{
            cout<<"######### UNMUTE MIC ######"<<endl;
            system("echo 92 > /sys/class/gpio/export");
            system("echo \"out\" > /sys/class/gpio/gpio92/direction");
            system("echo 1 > /sys/class/gpio/gpio92/value");
            m_mute=false;
            m_ledManager.setLEDRingOff();
        }
    }
    else if(strcmp(key_name,"KEY_VOLUMEUP")==0 && state == LIBINPUT_KEY_STATE_RELEASED){
       cout<<"######### VOLUME_UP by 10 ######"<<endl;
       m_interactionManager->adjustVolume(alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED, 10);
    }
    else if(strcmp(key_name,"KEY_VOLUMEDOWN")==0 && state == LIBINPUT_KEY_STATE_RELEASED){
       cout<<"######### VOLUME DOWN by 10 ######"<<endl;
       m_interactionManager->adjustVolume(alexaClientSDK::avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED, -10);
    }
#else
    if(strcmp(key_name,"KEY_POWER")==0 && state == LIBINPUT_KEY_STATE_RELEASED){

        clock_gettime(CLOCK_REALTIME, &krtime);

        double t_elapsed = ( krtime.tv_sec - kptime.tv_sec ) + ( krtime.tv_nsec - kptime.tv_nsec ) / BILLION;
        cout << "Key pressed for " << key_name << t_elapsed << " second(s)" << endl;

        if (kpset ==1 && t_elapsed < 2.5){
            setMuteStatus();
            kpset=0;
        }

        if(audio_state==MUTED){
            cout << "Swtiching on LED as audio is muted" << endl;
            m_ledManager.setLEDRingOn(RED_LED);
        }

        if(audio_state==VOCAL){
            cout << "Swtiching off LED as audio is back"  << endl;
            m_ledManager.setLEDRingOff();
        }
    }
#endif
    return;
 }


void KeyEventManager::setMuteStatus(){

    string command;
    cout << "setMuteStatus Called" << endl;
    static double vol = 0;

    if (audio_state==VOCAL){
#ifndef HWMSM8909
        m_userInterfaceManager->setMicrophoneOff();
#endif
        audio_state=MUTED;
    }
    else
    {
#ifndef HWMSM8909
        m_userInterfaceManager->setMicrophoneOn();
#endif
        audio_state=VOCAL;
    }

}

void KeyEventManager::handleKeyEvent(struct libinput_event *ev)
{
    KEY_EVTMGR_LOG_FUNC_ENTRY();

   struct libinput_event_keyboard *kb = libinput_event_get_keyboard_event(ev);
   struct libinput_device *dev = libinput_event_get_device(ev);
   enum libinput_key_state state;
   const char *key_name;
   string key_state_string;

   state = libinput_event_keyboard_get_key_state(kb);
   key_name = libevdev_event_code_get_name(EV_KEY,
                        libinput_event_keyboard_get_key(kb));

   if(state == LIBINPUT_KEY_STATE_PRESSED)
        keyPressEventHandler(key_name,state);

   if(state == LIBINPUT_KEY_STATE_RELEASED)
        keyReleaseEventHandler(key_name,state);

   key_state_string = (state == LIBINPUT_KEY_STATE_PRESSED) ? "pressed " : "released";
   cout << key_name << " is " << key_state_string << endl;

    KEY_EVTMGR_LOG_FUNC_RETURN();
} // KeyEventManager::run


void KeyEventManager::list_all_devices(struct libinput *linput)
{
   struct libinput_event *ev;

   libinput_dispatch(linput);
   while ((ev = libinput_get_event(linput))) {
      if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED) {
         struct libinput_device *dev = libinput_event_get_device(ev);
         //printf("Input Device:   %s\n",
            //     udev_device_get_devnode(libinput_device_get_udev_device(dev)));
      }

      libinput_event_destroy(ev);
      libinput_dispatch(linput);
   }
}



extern "C" {

//===========================================================================
// keyEventManagerCallBack
//===========================================================================
void keyEventManagerCallBack(struct libinput_event *ev)
{
  KEY_EVTMGR_LOG_FUNC_ENTRY();
  KeyEventManager *kObject;
  struct libinput *glibinput;

  glibinput = libinput_event_get_context(ev);
  kObject = static_cast<KeyEventManager*>(libinput_get_user_data(glibinput));

  kObject->handleKeyEvent(ev);
  KEY_EVTMGR_LOG_FUNC_RETURN();
}

} // end of extern "C"
