/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ZCL_LEVELCONTROL_DEMO_H__
#define __ZCL_LEVELCONTROL_DEMO_H__

#include "qapi_zb_cl_scenes.h"

/**
   @brief Initializes the ZCL Group demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Group demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_LevelControl_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle);

/**
   @brief Creates an LevelControl server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_LevelControl_Demo_Create_Server(uint8_t Endpoint, void **PrivData);

/**
   @brief Creates an LevelControl client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_LevelControl_Demo_Create_Client(uint8_t Endpoint, void **PrivData);

/**
   @brief Gets the scenes data for an on/off cluster.

   @param ExtData Buffer for the extension data of the cluster.

   @return true if the operation is successful false otherwise.
*/
qbool_t ZCL_LevelControl_GetScenesData(qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtData);

#endif

