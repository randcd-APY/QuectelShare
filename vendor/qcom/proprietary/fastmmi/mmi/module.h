/*
 * Copyright (c) 2014, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __MUDULE_H
#define __MUDULE_H


/* Should be the same name as config file, all those modules
 *  need to test in MMI process for event dispatch and draw screen
 *  did not support in Modules.
 */
#ifdef ANDROID
#define MODULE_AUDIO "mmi_audio.so"
#define MODULE_BATTERY "mmi_battery.so"
#define MODULE_BLUETOOTH "mmi_bluetooth.so"
#define MODULE_CAMERA "mmi_camera.so"
#define MODULE_CPU "mmi_cpu.so"
#define MODULE_FM "mmi_fm.so"
#define MODULE_GPS "mmi_gps.so"
#define MODULE_HEADSET_KEY "mmi_headset.so"
#define MODULE_KEY "mmi_key.so"
#define MODULE_LIGHT "mmi_light.so"
#define MODULE_LCD "mmi_lcd.so"
#define MODULE_MEMORY "mmi_memory.so"
#define MODULE_NFC "mmi_nfc.so"
#define MODULE_SENSOR "mmi_sensor.so"
#define MODULE_SIM "mmi_sim.so"
#define MODULE_STORAGE "mmi_storage.so"
#define MODULE_TOUCH "mmi_touch.so"
#define MODULE_VIBRATOR "mmi_vibrator.so"
#define MODULE_WIFI "mmi_wifi.so"
#else
#define MODULE_AUDIO "libmmi_audio.so"
#define MODULE_BATTERY "libmmi_battery.so"
#define MODULE_BLUETOOTH "libmmi_bluetooth.so"
#define MODULE_CAMERA "libmmi_camera.so"
#define MODULE_CPU "libmmi_cpu.so"
#define MODULE_FM "libmmi_fm.so"
#define MODULE_GPS "libmmi_gps.so"
#define MODULE_HEADSET_KEY "libmmi_headset.so"
#define MODULE_KEY "libmmi_key.so"
#define MODULE_LIGHT "libmmi_light.so"
#define MODULE_LCD "libmmi_lcd.so"
#define MODULE_MEMORY "libmmi_memory.so"
#define MODULE_NFC "libmmi_nfc.so"
#define MODULE_SENSOR "libmmi_sensor.so"
#define MODULE_SIM "libmmi_sim.so"
#define MODULE_STORAGE "libmmi_storage.so"
#define MODULE_TOUCH "libmmi_touch.so"
#define MODULE_VIBRATOR "libmmi_vibrator.so"
#define MODULE_WIFI "libmmi_wifi.so"

#define WORKAROUND_PTHREAD_EXIT  //avoid the issue of pthread_exit;
#endif
/**Module export function*/
void send_run_extra(module_info * mod, const char *subcmd);

int fork_launch_module(module_info * mod);
int module_exec_ui(module_info * mod);
void module_exec_pcba(module_info * mod);
void update_main_status();
void do_run_all(void *m);
void process_exit(void *m);
void process_exit(void *m, int result);

void launch_main();
void initial_screen(module_info * mod);
void switch_module(void *m);
void module_cleanup(module_info * mod);

/**Local module export function*/
bool is_audio_module(module_info * mod);
bool is_audio_headset(module_info * mod);
bool is_battery_module(module_info * mod);
bool is_bluetooth_module(module_info * mod);
bool is_camera_module(module_info * mod);
bool is_cpu_module(module_info * mod);
bool is_fm_module(module_info * mod);
bool is_gps_module(module_info * mod);
bool is_headset_key_module(module_info * mod);
bool is_key_module(module_info * mod);
bool is_light_module(module_info * mod);
bool is_lcd_module(module_info * mod);
bool is_memory_module(module_info * mod);
bool is_nfc_module(module_info * mod);
bool is_sensor_module(module_info * mod);
bool is_storage_module(module_info * mod);
bool is_sim_module(module_info * mod);
bool is_touch_module(module_info * mod);
bool is_vibrator_module(module_info * mod);
bool is_wifi_module(module_info * mod);
bool is_autoswitch_module(module_info * mod);

bool is_local_module(module_info * mod);
void launch_lcd_show_rgb(module_info * mod);
void launch_lcd_show_image(module_info * mod) ;
void stop_lcd_show_image() ;

void launch_touch_show_rgb(module_info * mod);
void launch_local_module(module_info * mod);

void do_next(void *m);

#endif
