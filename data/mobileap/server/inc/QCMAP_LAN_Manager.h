#ifndef _QCMAP_LAN_MANAGER_H_
#define _QCMAP_LAN_MANAGER_H_

/*====================================================

FILE:  QCMAP_LAN_Manager.h

SERVICES:
   QCMAP LAN Manager Class

=====================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/

/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse
  chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------
  03/05/18   jt         initial LAN Manager creation
==========================================================*/

#include <vector>
#include <unordered_map>

#include <errno.h>

#include "QCMAP_Backhaul.h"
#include "QCMAP_LAN.h"
#include "QCMAP_Virtual_LAN.h"
#include "QCMAP_L2TP.h"

class QCMAP_LAN_Manager
{
  private:
    std::unordered_map<int16_t, QCMAP_LAN*> bridge_vlan_id_ht;
    static QCMAP_LAN_Manager* lan_mgr_object;

    void CreateLANBridge();
    QCMAP_LAN* CreateLANBridge(qcmap_msgr_vlan_config_v01 vlan_config);
    bool DeleteLANBridge(int16_t bridge_vlan_id);

    QCMAP_LAN_Manager(void);
    ~QCMAP_LAN_Manager(void);

  public:
    static QCMAP_LAN_Manager* GetInstance(void);
    static void DestroyInstance(void);

    QCMAP_LAN* GetLANBridge(int16_t bridge_vlan_id);

    int GetNumOfBridges(void);
    std::vector<int16_t> GetBridgeIDs(void);

    QCMAP_LAN* AssociateIface(qcmap_msgr_interface_type_enum_v01 iface_type,
                                     qcmap_msgr_vlan_config_v01 vlan_config,
                                     bool createConfig);
    bool DisassociateIface(qcmap_msgr_interface_type_enum_v01 iface_type,
                                  int16_t bridge_vlan_id);

    bool IsVLANToPDNMappingAllowed(void);
};
#endif
