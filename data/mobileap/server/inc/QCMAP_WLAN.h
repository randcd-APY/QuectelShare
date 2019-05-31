#ifndef _QCMAP_WLAN_H_
#define _QCMAP_WLAN_H_

/*====================================================

FILE:  QCMAP_WLAN.h

SERVICES:
   QCMAP Connection Manager WLAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/14/14   ka           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"

#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_BT_Tethering.h"


#define INITIALIZE_WLAN_IF_INFO(buf_ptr)   memset(buf_ptr, 0, sizeof(buf_ptr));
#define QCMAP_DEFAULT_ALWAYS_ON_WLAN 0

  /*===========================================================================
  MACRO P2P_ROLE_ENABLE

  DESCRIPTION
    This macro is used to enable the peer-to-peer role
  ===========================================================================*/

#define P2P_ROLE_ENABLE 1

  /*===========================================================================
  MACRO P2P_ROLE_DISABLE

  DESCRIPTION
    This macro is used to disable the peer-to-peer role
  ===========================================================================*/

#define P2P_ROLE_DISABLE 0


class QCMAP_WLAN
{
  private:
    static bool flag;
    static QCMAP_WLAN *wlan_object;

    QCMAP_WLAN();
    ~QCMAP_WLAN();

  public:
    static QCMAP_WLAN* Get_Instance(boolean obj_create=false);
    static void Destroy_Instance();

    /*----------------------MobileAP WLAN Execution-----------------------*/

    //ALL VARIABLES HERE//
    qcmap_cm_wlan_conf_t wlan_cfg;
    qcmap_cm_wlan_if_info_t wlan_if_info[QCMAP_MAX_NUM_INTF];
    uint8 active_if_count;
    boolean is_STA_GSB_conf;
    int last_STA_GSB_IF;

      /* Which mode is wifi brought up in */
    qcmap_cm_debug_conf_t debug_config;
    qcmap_msgr_wlan_mode_enum_v01 wifi_mode;
    qcmap_msgr_access_profile_v01 prev_guest_1_profile;
    qcmap_msgr_access_profile_v01 prev_guest_2_profile;
    qcmap_msgr_access_profile_v01 prev_guest_3_profile;
    boolean activate_wlan_in_progress;
    boolean disable_wlan_in_progress;

    /* Always on WLAN status */
    boolean always_on_wlan;

    /* eth device number for first AP iface */
    static bool  priv_client_restart_wlan;

    /* Variable to Store the STAOnlyMode enabled state */
    boolean is_sta_only_mode_enabled;

    qcmap_msgr_wlan_status_ind_msg_v01 current_wlan_status;

    /* Variable to Store the P2P config */
    qcmap_p2p_config p2p_config;

    boolean AssociateWLANtoBridge(int wlan_index);

    boolean ReadWLANConfigFromXML();

    static boolean ReadWLANConfigPathsFromXML(qcmap_cm_wlan_paths_conf_t *wlan_paths);

    static boolean GetsetWLANConfigFromXML( qcmap_wlan_config_enum conf,
                                            qcmap_action_type action,
                                            uint32 *value );

    inline boolean IsWlanModeUpdated()
                        {return (this->wifi_mode != this->wlan_cfg.wlan_mode); }

    static boolean EnableWLAN( qmi_error_type_v01 *qmi_err_num,
                                     boolean enable_wlan_onbootup = false,
                                     boolean privileged_client    = false );

    boolean EnableWLANModule(char *,boolean bootup);

    /* Bring down Linux LAN. */
    static boolean DisableWLAN( qmi_error_type_v01 *qmi_err_num,
                                      boolean privileged_client    = false );
    inline boolean IsSSRInProgress(void)
                                  {return QCMAP_WLAN::priv_client_restart_wlan;}

    boolean DisableWLANModule(void);
    /* Set WLAN Mode. */
    static boolean SetWLANConfig
                           ( qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
                             qcmap_msgr_guest_profile_config_v01*  guest_ap_access_profile,
                             qcmap_msgr_station_mode_config_v01 *station_config,
                             qmi_error_type_v01 *qmi_err_num );

    /* Get Configured LAN Mode */
    static boolean GetWLANConfig
                          ( qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                            qcmap_msgr_guest_profile_config_v01 *guest_access_profile,
                            qcmap_msgr_station_mode_config_v01 *station_config,
                            qmi_error_type_v01 *qmi_err_num );

    /* Actiavte WLAN Request. */
    static boolean ActivateWLAN(qmi_error_type_v01 *qmi_err_num);

    /* Restart WLAN Request. */
    static boolean RestartWLAN(void);


    /* Get the Current LAN Status. */
    static boolean GetWLANStatus( qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                                  qmi_error_type_v01 *qmi_err_num );

    /* Activate HostapdConfig Request.*/
    static boolean ActivateHostapdConfig
                       ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                         qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
                         qmi_error_type_v01 *qmi_err_num );

    boolean ActivateHostapdActionStart
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 int guest_ap2_pid,
                                 int guest_ap3_pid,
                                 int active_ap_num,
                                 qmi_error_type_v01 *qmi_err_num );

    boolean ActivateHostapdActionStop
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 int guest_ap2_pid,
                                 int guest_ap3_pid,
                                 qmi_error_type_v01 *qmi_err_num);

    boolean ActivateHostapdActionRestart
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 int guest_ap2_pid,
                                 int guest_ap3_pid,
                                 int active_ap_num,
                                 qmi_error_type_v01 *qmi_err_num);

    /* Checks if hostapd is running. */
    boolean IsHostapdRunning(int iface);

    /* Checks if wpa_supplicant is running. */
    boolean IsSupplicantRunning(void);

    /* Kills the appropriate instance of hostapd_cli*/
    void StopHostapdCli(qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type);

    /* Starts the appropriate instance of hostapd_cli*/
    void StartHostapdCli(qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type);

    /* Configure, start and stop the Linux HostAPD server. */
    boolean StopHostAPD(void);
    boolean StartHostAPD(int intf,int wlan_dev_index);

    boolean IsGuestProfile1Updated();
    boolean IsGuestProfile2Updated();
    void UpdateAccessProfileRules();
    void InstallGuestAPAccessRules(int ap_dev_num);
    void DeleteGuestAPAccessRules();
    boolean IsHostapdkilled(int omit_pid1, int omit_pid2, int omit_pid3);

    /*Get count of APs active*/
    int GetActiveAPCount(qcmap_msgr_wlan_mode_enum_v01 wifi_mode);

    /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
    /* Updates the IPACM_cfg file */
    boolean SetIPAWLANMode(int ap_dev_num,int mode);
#endif /* FEATURE_DATA_TARGET_MDM9607 */

    /* Enable STA Only Mode.
       Putting it in WLAN Class so that we dont have to create any additional objects */
    static boolean EnableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num);
    static boolean DisableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num);

    /* obtain active IF info*/
    static boolean GetActiveWLANIFInfo(qcmap_msgr_wlan_if_info_v01 *wlan_if_info,
                            uint8 *num_of_entries,
                            qmi_error_type_v01 *qmi_err_num);
    static void UpdateWLANStatuswithSCMind(qcmap_msgr_scm_ind_enum_v01 ind);
    static boolean QCMAP_WLAN::GetWLANDeviceTypeFromAP
    (
      qcmap_msgr_device_type_enum_v01 ap_type,
      qcmap_msgr_wlan_device_type_v01 *wlan_dev_type,
      qmi_error_type_v01 *qmi_err_num
    );
    static boolean IsAPUpOnTUF(void);

    /* Send WLAN Status IND */
    void SendWLANStatusIND(void);
    boolean HostapdCliCmdParseMac
    (
      int dev,
      qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
    );
    void ParseMacAndAddDeviceEntry
    (
      qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
    );
    void AddDeviceEntryForMac
    (
      char *mac_str,
      qcmap_msgr_device_type_enum_v01 device_type
    );

    /*set and get Always on WLAN from XML*/
    static boolean GetSetAlwaysOnWLANFromXML(qcmap_action_type action, char *data, int data_len);

    /*set Always On WLAN flag*/
    static boolean SetAlwaysOnWLAN(boolean always_on_wlan_state,
                                         qmi_error_type_v01 *qmi_err_num);

    /*get Always On WLAN flag*/
    static boolean GetAlwaysOnWLAN(boolean *always_on_wlan_status,
                                         qmi_error_type_v01 *qmi_err_num);

    static boolean set_p2p_role(qcmap_p2p_config p2p_config,
                                    qmi_error_type_v01 *qmi_err_num);

    static boolean get_p2p_role(qcmap_p2p_config* p2p_config,
                                    qmi_error_type_v01 *qmi_err_num);
};
#endif
