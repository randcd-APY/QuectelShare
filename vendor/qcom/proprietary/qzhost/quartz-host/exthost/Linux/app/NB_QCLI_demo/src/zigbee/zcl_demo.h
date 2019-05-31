/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ZCL_DEMO_H__
#define __ZCL_DEMO_H__

#include "pal.h"
#include "qapi_zb.h"
#include "qapi_zb_cl.h"

#define ZCL_DEMO_IGNORE_CLUSTER_ID                                      (0xFFFF)

typedef enum
{
   ZCL_DEMO_CLUSTERTYPE_UNKNOWN,
   ZCL_DEMO_CLUSTERTYPE_CLIENT,
   ZCL_DEMO_CLUSTERTYPE_SERVER
} ZCL_Demo_ClusterType_t;

/*
   Forward define the ZCL_Demo_Cluster_Info_t structure.
*/
typedef struct ZCL_Demo_Cluster_Info_s ZCL_Demo_Cluster_Info_t;

/**
   @brief Prototype for a function to initialize a cluster demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for demo.

   @return true if the ZigBee demo was initialized successfully, false
           otherwise.
*/
typedef qbool_t (*ZCL_Cluster_Demo_Init_Func_t)(QCLI_Group_Handle_t ZigBee_QCLI_Handle);

/**
   @brief Prototype for a function to create a cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data allocated for the cluster
                   demo.  This will be initaialized to NULL before the create
                   function is called so can be ignored if the demo has no
                   private data.  If set to a non-NULL value, the data will be
                   freed when the cluster is removed.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
typedef qapi_ZB_Cluster_t (*ZCL_Cluster_Demo_Create_Func_t)(uint8_t Endpoint, void **PrivData);

/**
   @brief Prototype for a function that is called when a cluster is removed.

   Provided to allow the cluster demo to cleanup any extra resources.

   @param Cluster_Info is the information for the cluster being removed.
*/
typedef void (*ZCL_Cluster_Cleanup_CB_t)(ZCL_Demo_Cluster_Info_t *ClusterInfo);

/*
   Structure represents the information for a cluster.
*/
typedef struct ZCL_Demo_Cluster_Info_s
{
   qapi_ZB_Cluster_t         Handle;      /** Handle of the cluster. */
   uint16_t                  ClusterID;   /** ID of the cluster. */
   uint8_t                   Endpoint;    /** endpoint used by the cluster. */
   ZCL_Demo_ClusterType_t    ClusterType; /** Indicates the type of cluster as either server or client. */
   const char               *ClusterName; /** Name of the cluster being added. */
   const char               *DeviceName;  /** Device Name for the cluster. */
   void                     *PrivData;    /** Private data for the cluster. */
   ZCL_Cluster_Cleanup_CB_t  Cleanup_CB;  /** Function called when cluster is removed. */
} ZCL_Demo_Cluster_Info_t;

/**
   @brief Registers the ZigBee cluster commands with QCLI.
*/
qbool_t Initialize_ZCL_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle);

/**
   @brief Function to add a cluster entry to a cluster list.

   @param Cluster_Info is the information for the cluster to add.

   @return The ClusterIndex of the newly added cluster or a negative value if
           there was an error.
*/
int16_t ZB_Cluster_AddCluster(const ZCL_Demo_Cluster_Info_t *Cluster_Info);

/**
   @brief Called when the stack is shutdown to cleanup the cluster list.
*/
void ZB_Cluster_Cleanup(void);

/**
   @brief Gets the cluster handle for a specified index.

   @param ClusterIndex is the index of the cluster being requested.
   @param ClusterID    is the expected ID of the cluster being requested.  Set
                       to 0xFFFF to ignore.

   @return The info structure for the cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByIndex(uint16_t ClusterIndex, uint16_t ClusterID);

/**
   @brief Finds a cluster with a matching ID and endpoint.

   @param Endpoint    is the endpoint for the cluster to find.
   @param ClusterID   is the ID fo the cluster to find.
   @param ClusterType is the type of cluster (server or client).

   @return The handle for the requested cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByEndpoint(uint8_t Endpoint, uint16_t ClusterID, ZCL_Demo_ClusterType_t ClusterType);

/**
   @brief Finds a cluster with a matching ID and endpoint.

   @param Handle is the handle of the cluster to find.

   @return The handle for the requested cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByHandle(qapi_ZB_Cluster_t Handle);

#endif

