/*=========================================================================
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GEOFENCE_ADAPTER_H
#define GEOFENCE_ADAPTER_H

#include <IzatAdapterBase.h>
#include <LocationAPI.h>
#include <IzatApiBase.h>
#include <geofence.h>
#include <LocDualContext.h>
#include <map>

using namespace izat_core;

/* Geofence specific Type Definitions */
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

//map of hwId to GeofenceObject
typedef std::map<uint32_t, GeofenceObject> GeofencesMap;

//map of GeofenceKey to hwId
typedef std::map<GeofenceKey, uint32_t> GeofenceIdMap;

/* Geofence Adapter Class */
class GeofenceAdapter : public IzatAdapterBase {

private:
    typedef std::map<LocationAPI*, LocationCallbacks> ClientDataMap;
    ClientDataMap mClientData;
    GeofencesMap mGeofences; //map hwId to GeofenceObject
    GeofenceIdMap mGeofenceIds; //map of clientId to hwId

public:

    GeofenceAdapter();
    inline virtual ~GeofenceAdapter() {
        LOC_LOGL0("~GeofenceAdapter()");
    }

    /* Adapter init/deinit */
    void initializeCommand();
    void cleanUpCommand();

    /* Client Management APIs */
    void addClientCommand(LocationAPI* client, const LocationCallbacks& callbacks);
    void removeClientCommand(LocationAPI* client);

    /* Geofence APIs */
    uint32_t* addGeofencesCommand(LocationAPI* client, size_t count,
                                  GeofenceOption* options, GeofenceInfo* info);
    void removeGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void pauseGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void resumeGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids);
    void modifyGeofencesCommand(LocationAPI* client, size_t count, uint32_t* ids,
                                GeofenceOption* options);

    /* Response APIs
     * IzatApi performs the context switch to MsgTask before invoking this */
    virtual bool geofenceBreach(uint32_t id,
                                Location location,
                                GeofenceBreachType breachTypeMask,
                                uint64_t timestamp);
    virtual bool geofenceAlert(GeofenceAlertType alert);

    void reportResponse(LocationAPI* client, size_t count, LocationError* errs, uint32_t* ids);

    /* Engine up/down events */
    virtual void handleEngineUpEvent();

    /* Internal Utility Methods */
    void removeClientGeofences(LocationAPI* client);
    void saveClient(LocationAPI* client, const LocationCallbacks& callbacks);
    void eraseClient(LocationAPI* client);

    LocationError getHwIdFromClient(LocationAPI* client, uint32_t clientId, uint32_t& hwId);
    LocationError getGeofenceKeyFromHwId(uint32_t hwId, GeofenceKey& key);

    void saveGeofenceItem(LocationAPI* client,
                          uint32_t clientId,
                          uint32_t hwId,
                          const GeofenceOption& options,
                          const GeofenceInfo& info);
    void removeGeofenceItem(uint32_t hwId);
    void pauseGeofenceItem(uint32_t hwId);
    void resumeGeofenceItem(uint32_t hwId);
    void modifyGeofenceItem(uint32_t hwId, const GeofenceOption& options);

    void restartGeofences();

private:
    void dump();
};

#endif /* GEOFENCE_ADAPTER_H */
