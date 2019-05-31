/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ZDP_DEMO_H__
#define __ZDP_DEMO_H__

/**
   @brief Registers ZigBee ZDP demo commands with QCLI.

   @param ZigBee_QCLI_Handle is the QCLI handle for the main ZigBee demo.

   @return true if the ZigBee ZDP demo initialized successfully, false
           otherwise.

*/
qbool_t Initialize_ZDP_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle);

/**
   @brief Initialize the ZigBee ZDP demo after the ZB stack had been
          initialized.

   @return true if the ZigBee ZDP demo initialized successfully, false
           otherwise.

*/
qbool_t ZDP_Demo_StackInitialize(qapi_ZB_Handle_t ZigBee_Handle);

#endif

