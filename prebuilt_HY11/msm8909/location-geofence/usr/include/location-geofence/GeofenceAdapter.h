/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCE_ADAPTER_H
#define GEOFENCE_ADAPTER_H

#include <IzatAdapterBase.h>
#include <LocDualContext.h>
#include <LocationAPI.h>
#include <map>

using namespace izat_core;

#define COPY_IF_NOT_NULL(dest, src, len) do { \
    if (NULL!=dest && NULL!=src) { \
        for (size_t i=0; i<len; ++i) { \
            dest[i] = src[i]; \
        } \
    } \
} while (0)

typedef struct GeofenceKey {
    LocationAPI* client;
    uint32_t id;
    inline GeofenceKey() :
        client(NULL), id(0) {}
    inline GeofenceKey(LocationAPI* _client, uint32_t _id) :
        client(_client), id(_id) {}
} GeofenceKey;
inline bool operator <(GeofenceKey const& left, GeofenceKey const& right) {
    return left.id < right.id || (left.id == right.id && left.client < right.client);
}
inline bool operator ==(GeofenceKey const& left, GeofenceKey const& right) {
    return left.id == right.id && left.client == right.client;
}
inline bool operator !=(GeofenceKey const& left, GeofenceKey const& right) {
    return left.id != right.id || left.client != right.client;
}
typedef struct {
    GeofenceKey key;
    GeofenceBreachTypeMask breachMask;
    uint32_t responsiveness;
    uint32_t dwellTime;
    double latitude;
    double longitude;
    double radius;
    bool paused;
} GeofenceObject;
typedef std::map<uint32_t, GeofenceObject> GeofencesMap; //map of hwId to GeofenceObject
typedef std::map<GeofenceKey, uint32_t> GeofenceIdMap; //map of GeofenceKey to hwId



class GeofenceAdapter : public IzatAdapterBase {

    /* ==== CLIENT ========================================================================= */
    typedef std::map<LocationAPI*, LocationCallbacks> ClientDataMap;
    ClientDataMap mClientData;

    /* ==== GEOFENCES ====================================================================== */
    GeofencesMap mGeofences; //map hwId to GeofenceObject
    GeofenceIdMap mGeofenceIds; //map of GeofenceKey to hwId

public:

    GeofenceAdapter();
    virtual ~GeofenceAdapter() {}

    /* ==== SSR ============================================================================ */
    /* ======== EVENTS ====(Called from QMI Thread)========================================= */
    virtual void handleEngineUpEvent();
    /* ======== UTILITIES ================================================================== */
    void restartGeofences();

    /* ==== CLIENT ========================================================================= */
    /* ======== COMMANDS ====(Called from Client Thread)==================================== */
    void addClientCommand(LocationAPI* client, const LocationCallbacks& callbacks);
    void removeClientCommand(LocationAPI* client);
    void requestCapabilitiesCommand(LocationAPI* client);
    /* ======== UTILITIES ================================================================== */
    void saveClient(LocationAPI* client, const LocationCallbacks& callbacks);
    void eraseClient(LocationAPI* client);
    void updateClientsEventMask();
    void removeClientGeofences(LocationAPI* client);
    LocationCallbacks getClientCallbacks(LocationAPI* client);

    /* ==== GEOFENCES ====================================================================== */
    /* ======== COMMANDS ====(Called from Client Thread)==================================== */
    uint32_t* addGeofencesCommand(LocationAPI* client, size_t count,
                                  GeofenceOption* options, GeofenceInfo* info);
    void removeGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void pauseGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void resumeGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void modifyGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids,
                                GeofenceOption* options);
    /* ======== RESPONSES ================================================================== */
    void reportResponse(LocationAPI* client, size_t count, LocationError* errs, uint32_t* ids);
    /* ======== UTILITIES ================================================================== */
    void saveGeofenceItem(LocationAPI* client,
                          uint32_t clientId,
                          uint32_t hwId,
                          const GeofenceOption& options,
                          const GeofenceInfo& info);
    void removeGeofenceItem(uint32_t hwId);
    void pauseGeofenceItem(uint32_t hwId);
    void resumeGeofenceItem(uint32_t hwId);
    void modifyGeofenceItem(uint32_t hwId, const GeofenceOption& options);
    LocationError getHwIdFromClient(LocationAPI* client, uint32_t clientId, uint32_t& hwId);
    LocationError getGeofenceKeyFromHwId(uint32_t hwId, GeofenceKey& key);
    void dump();

    /* ==== REPORTS ======================================================================== */
    /* ======== EVENTS ====(Called from QMI Thread)========================================= */
    void geofenceBreachEvent(size_t count, uint32_t* hwIds, Location& location,
                             GeofenceBreachType breachType, uint64_t timestamp);
    void geofenceStatusEvent(GeofenceStatusAvailable available);
    /* ======== UTILITIES ================================================================== */
    void geofenceBreach(size_t count, uint32_t* hwIds, const Location& location,
                        GeofenceBreachType breachType, uint64_t timestamp);
    void geofenceStatus(GeofenceStatusAvailable available);
};

#endif /* GEOFENCE_ADAPTER_H */
