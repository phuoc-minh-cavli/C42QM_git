/*=========================================================================
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==========================================================================*/
#include <GeofenceAdapter.h>
#include "loc_log.h"
#include <platform_lib_log_util.h>

extern pthread_cond_t gGeofenceCleanupSignal;
extern pthread_mutex_t gCleanupSignalMutex;

using namespace izat_core;

const LOC_API_ADAPTER_EVENT_MASK_T GEOFENCE_MASK =
     (LOC_API_ADAPTER_BIT_REPORT_GENFENCE_DWELL |
      LOC_API_ADAPTER_BIT_BATCHED_GENFENCE_BREACH_REPORT);

GeofenceAdapter::GeofenceAdapter() :
    IzatAdapterBase(GEOFENCE_MASK,
                    LocDualContext::getLocFgContext(
                    NULL,
                    NULL,
                    LocDualContext::mLocationHalName,
                    false)),
    mClientData(), mGeofences(), mGeofenceIds()
{
    LOC_LOGL0("GeofenceAdapter()");
}

/*------------------------------------------------
  Adapter init/cleanup
  Invoked from Client Thread
-------------------------------------------------*/
void
GeofenceAdapter::initializeCommand()
{
    LOC_LOGL0("initializeCommand()");
}

void
GeofenceAdapter::cleanUpCommand()
{
    LOC_LOGL0("cleanUpCommand()");

    struct GeofenceCleanUp : public LocMsg {
        GeofenceAdapter& mAdapter;
        inline GeofenceCleanUp(GeofenceAdapter& adapter) :
                LocMsg(),
                mAdapter(adapter) {}
        inline virtual void proc() const {

            pthread_mutex_lock(&gCleanupSignalMutex);
            pthread_cond_signal(&gGeofenceCleanupSignal);
            pthread_mutex_unlock(&gCleanupSignalMutex);
        }
    };

    sendMsg(new GeofenceCleanUp(*this));
}

/*------------------------------------------------
  Client Management APIs
  Invoked from Client Thread.
-------------------------------------------------*/
void
GeofenceAdapter::addClientCommand(
        LocationAPI* client, const LocationCallbacks& callbacks)
{
    LOC_LOGH1("addClientCommand() client %p", client);

    struct MsgAddClient : public LocMsg {
        GeofenceAdapter& mAdapter;
        LocationAPI* mClient;
        const LocationCallbacks mCallbacks;
        inline MsgAddClient(GeofenceAdapter& adapter,
                            LocationAPI* client,
                            const LocationCallbacks& callbacks) :
            LocMsg(),
            mAdapter(adapter),
            mClient(client),
            mCallbacks(callbacks) {}
        inline virtual void proc() const {
            mAdapter.saveClient(mClient, mCallbacks);
        }
    };

    sendMsg(new MsgAddClient(*this, client, callbacks));
}

void
GeofenceAdapter::removeClientCommand(LocationAPI* client)
{
    LOC_LOGH1("removeClientCommand() client %p", client);

    struct MsgRemoveClient : public LocMsg {
        GeofenceAdapter& mAdapter;
        LocationAPI* mClient;
        inline MsgRemoveClient(GeofenceAdapter& adapter,
                               LocationAPI* client) :
            LocMsg(),
            mAdapter(adapter),
            mClient(client) {}
        inline virtual void proc() const {
            mAdapter.removeClientGeofences(mClient);
            mAdapter.eraseClient(mClient);
        }
    };

    sendMsg(new MsgRemoveClient(*this, client));
}

void
GeofenceAdapter::removeClientGeofences(LocationAPI* client)
{
    LOC_LOGL1("removeClientGeofences() client %p", client);

    for (GeofenceIdMap::iterator it = mGeofenceIds.begin(); it != mGeofenceIds.end();) {
        uint32_t hwId = it->second;
        GeofenceKey key(it->first);
        if (client == key.client) {
            LocationError err = mIzatApi->removeGeofence(hwId, key.id);
            if (LOCATION_ERROR_SUCCESS == err) {
                GeofencesMap::iterator it2 = mGeofences.find(hwId);
                if (it2 != mGeofences.end()) {
                    mGeofences.erase(it2);
                } else {
                    LOC_LOGE1("Geofence item not found. hwId %u", hwId);
                }
                mGeofenceIds.erase(it++);
                continue;
            }
        }
        ++it; // increment only when not erasing an iterator
    }
}

void
GeofenceAdapter::saveClient(
        LocationAPI* client, const LocationCallbacks& callbacks)
{
    mClientData[client] = callbacks;
    //updateClientsEventMask();
}

void
GeofenceAdapter::eraseClient(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end()) {
        mClientData.erase(it);
    }
    //updateClientsEventMask();
}

/*------------------------------------------------
  GEOFENCE APIs
  Invoked from Client Thread
-------------------------------------------------*/
uint32_t*
GeofenceAdapter::addGeofencesCommand(
        LocationAPI* client, size_t count,
        GeofenceOption* options, GeofenceInfo* infos)
{
    LOC_LOGL2("addGeofencesCommand() client %p count %u", client, count);

    struct MsgAddGeofences : public LocMsg {
        GeofenceAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        size_t mCount;
        uint32_t* mIds;
        GeofenceOption* mOptions;
        GeofenceInfo* mInfos;
        inline MsgAddGeofences(GeofenceAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               size_t count,
                               uint32_t* ids,
                               GeofenceOption* options,
                               GeofenceInfo* infos) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mCount(count),
            mIds(ids),
            mOptions(options),
            mInfos(infos) {}
        inline virtual void proc() const {

            /* Download XTRA Integrity before starting any session.
             * Only for the first time this message is posted, not for retries via sendPendingMsg. */
            if (!mIsPendingMsg) {
                uint32_t waitTime = mAdapter.getXtraIntDloadWaitTimeSec();
                if (waitTime > 0) {
                    LOC_LOGH1("Add geofence pending on XTRA INT, wait %d sec", waitTime);
                    mAdapter.sendPendingMsg(new MsgAddGeofences(
                            mAdapter, mApi, mClient, mCount, mIds, mOptions, mInfos));
                    if (!mAdapter.mPendingMsgTimer.isStarted()) {
                        mAdapter.triggerXtraIntDload();
                        mAdapter.mPendingMsgTimer.start(waitTime);
                    } else  {
                        LOC_LOGH0("Pending msg timer already running");
                    }
                    return;
                }
            }

            LocationError* errs = new LocationError[mCount];
            for (size_t i=0; i < mCount; ++i) {
                if (NULL == mIds || NULL == mOptions || NULL == mInfos) {
                    errs[i] = LOCATION_ERROR_INVALID_PARAMETER;
                } else {
                    uint32_t hwId = 0;
                    errs[i] = mApi.addGeofence(hwId,
                                                   mIds[i],
                                                   mOptions[i],
                                                   mInfos[i],
                                                   GF_CONFIDENCE_HIGH);
                    if (LOCATION_ERROR_SUCCESS == errs[i]) {
                        mAdapter.saveGeofenceItem(mClient,
                                                  mIds[i],
                                                  hwId,
                                                  mOptions[i],
                                                  mInfos[i]);
                    }
                }
            }
            mAdapter.reportResponse(mClient, mCount, errs, mIds);
            delete[] errs;
            delete[] mIds;
            delete[] mOptions;
            delete[] mInfos;
        }
    };

    if (0 == count) {
        return NULL;
    }
    uint32_t* ids = new uint32_t[count];
    for (size_t i=0; i < count; ++i) {
        ids[i] = generateSessionId();
    }
    GeofenceOption* optionsCopy;
    if (options == NULL) {
        optionsCopy = NULL;
    } else {
        optionsCopy = new GeofenceOption[count];
        for (size_t i=0; i < count; ++i) {
            optionsCopy[i] = options[i];
        }
    }
    GeofenceInfo* infosCopy;
    if (infos == NULL) {
        infosCopy = NULL;
    } else {
        infosCopy = new GeofenceInfo[count];
        for (size_t i=0; i < count; ++i) {
            infosCopy[i] = infos[i];
        }
    }

    sendMsg(new MsgAddGeofences(*this, *mIzatApi, client, count, ids, optionsCopy, infosCopy));
    return ids;
}

void
GeofenceAdapter::removeGeofencesCommand(
        LocationAPI* client, size_t count, uint32_t* ids)
{
    LOC_LOGL2("removeGeofencesCommand() client %p count %u", client, count);

    struct MsgRemoveGeofences : public LocMsg {
        GeofenceAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        size_t mCount;
        uint32_t* mIds;
        inline MsgRemoveGeofences(GeofenceAdapter& adapter,
                                  IzatApiBase& api,
                                  LocationAPI* client,
                                  size_t count,
                                  uint32_t* ids) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mCount(count),
            mIds(ids) {}
        inline virtual void proc() const  {
            LocationError* errs = new LocationError[mCount];
            for (size_t i=0; i < mCount; ++i) {
                uint32_t hwId = 0;
                errs[i] = mAdapter.getHwIdFromClient(mClient, mIds[i], hwId);
                if (LOCATION_ERROR_SUCCESS == errs[i]) {
                    errs[i] = mApi.removeGeofence(hwId, mIds[i]);
                    if (LOCATION_ERROR_SUCCESS == errs[i]) {
                        mAdapter.removeGeofenceItem(hwId);
                    }
                }
            }
            mAdapter.reportResponse(mClient, mCount, errs, mIds);
            delete[] errs;
            delete[] mIds;
        }
    };

    if (0 == count) {
        return;
    }
    uint32_t* idsCopy = new uint32_t[count];
    for (size_t i=0; i < count; ++i) {
        idsCopy[i] = ids[i];
    }
    sendMsg(new MsgRemoveGeofences(*this, *mIzatApi, client, count, idsCopy));
}

void
GeofenceAdapter::pauseGeofencesCommand(
        LocationAPI* client, size_t count, uint32_t* ids)
{
    LOC_LOGE0("pauseGeofencesCommand() unsupported.");
}

void
GeofenceAdapter::resumeGeofencesCommand(
        LocationAPI* client, size_t count, uint32_t* ids)
{
    LOC_LOGE0("resumeGeofencesCommand() unsupported.");
}

void
GeofenceAdapter::modifyGeofencesCommand(
        LocationAPI* client, size_t count, uint32_t* ids,
        GeofenceOption* options)
{
    LOC_LOGE0("modifyGeofencesCommand() unsupported.");
}

/*------------------------------------------------
  Utility Methods
  Invoked from Message Thread
-------------------------------------------------*/
LocationError
GeofenceAdapter::getHwIdFromClient(LocationAPI* client, uint32_t clientId, uint32_t& hwId)
{
    GeofenceKey key(client, clientId);
    GeofenceIdMap::iterator it = mGeofenceIds.find(key);
    if (it != mGeofenceIds.end()) {
        hwId = it->second;
        return LOCATION_ERROR_SUCCESS;
    }
    return LOCATION_ERROR_ID_UNKNOWN;
}

LocationError
GeofenceAdapter::getGeofenceKeyFromHwId(uint32_t hwId, GeofenceKey& key)
{
    GeofencesMap::iterator it = mGeofences.find(hwId);
    if (it != mGeofences.end()) {
        key = it->second.key;
        return LOCATION_ERROR_SUCCESS;
    }
    return LOCATION_ERROR_ID_UNKNOWN;
}

void
GeofenceAdapter::saveGeofenceItem(
        LocationAPI* client, uint32_t clientId, uint32_t hwId,
        const GeofenceOption& options, const GeofenceInfo& info)
{
    LOC_LOGL3("saveGeofenceItem() hwId %u client %p clientId %u",
              hwId, client, clientId);
    GeofenceKey key(client, clientId);
    GeofenceObject object = {key,
                             options.breachTypeMask,
                             options.responsiveness,
                             options.dwellTime,
                             info.latitude,
                             info.longitude,
                             info.radius,
                             false};
    mGeofences[hwId] = object;
    mGeofenceIds[key] = hwId;
    dump();
}

void
GeofenceAdapter::removeGeofenceItem(uint32_t hwId)
{
    GeofenceKey key;
    LocationError err = getGeofenceKeyFromHwId(hwId, key);

    LOC_LOGL1("removeGeofenceItem() hwId %u", hwId);

    if (LOCATION_ERROR_SUCCESS != err) {
        LOC_LOGE0("Can not find the key for hwId");
    } else {
        GeofenceIdMap::iterator it1 = mGeofenceIds.find(key);
        if (it1 != mGeofenceIds.end()) {
            mGeofenceIds.erase(it1);

            GeofencesMap::iterator it2 = mGeofences.find(hwId);
            if (it2 != mGeofences.end()) {
                mGeofences.erase(it2);
                dump();
            } else {
                LOC_LOGE0("Geofence item not found.");
            }
        } else {
            LOC_LOGE0("Geofence item not found.");
        }
    }
}

void
GeofenceAdapter::pauseGeofenceItem(uint32_t hwId)
{
    GeofencesMap::iterator it = mGeofences.find(hwId);

    LOC_LOGL1("pauseGeofenceItem() hwId %u", hwId);

    if (it != mGeofences.end()) {
        it->second.paused = true;
        dump();
    } else {
        LOC_LOGE0("Geofence item not found.");
    }
}

void
GeofenceAdapter::resumeGeofenceItem(uint32_t hwId)
{
    GeofencesMap::iterator it = mGeofences.find(hwId);

    LOC_LOGL1("resumeGeofenceItem() hwId %u", hwId);

    if (it != mGeofences.end()) {
        it->second.paused = false;
        dump();
    } else {
        LOC_LOGE0("Geofence item not found.");
    }
}

void
GeofenceAdapter::modifyGeofenceItem(uint32_t hwId,
                                    const GeofenceOption& options)
{
    GeofencesMap::iterator it = mGeofences.find(hwId);

    LOC_LOGL1("modifyGeofenceItem() hwId %u", hwId);

    if (it != mGeofences.end()) {
        it->second.breachMask = options.breachTypeMask;
        it->second.responsiveness = options.responsiveness;
        it->second.dwellTime = options.dwellTime;
        dump();
    } else {
        LOC_LOGE0("Geofence item not found.");
    }
}

/*------------------------------------------------
  RESPONSE APIs
  Invoked from Message Thread
-------------------------------------------------*/
void
GeofenceAdapter::reportResponse(LocationAPI* client, size_t count,
                                LocationError* errs, uint32_t* ids)
{
    LOC_LOGL2("reportResponse() client %p count %u", client, count);

    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end() && it->second.collectiveResponseCb != NULL) {
        it->second.collectiveResponseCb(client, count, errs, ids);
    } else {
        LOC_LOGE0("response cb not found in info");
    }
}

bool
GeofenceAdapter::geofenceBreach(uint32_t id, Location location,
                                GeofenceBreachType breachType, uint64_t timestamp)
{
    for (ClientDataMap::iterator it = mClientData.begin();
            it != mClientData.end(); ++it) {
        uint32_t* clientIds = new uint32_t[1];
        GeofenceKey key;
        LocationError err = getGeofenceKeyFromHwId(id, key);
        if (LOCATION_ERROR_SUCCESS == err &&
                key.client == it->first &&
                it->second.geofenceBreachCb != NULL) {
            clientIds[0] = key.id;
            GeofenceBreachNotification notify = {sizeof(GeofenceBreachNotification),
                                                 1,
                                                 clientIds,
                                                 location,
                                                 breachType,
                                                 timestamp};

            it->second.geofenceBreachCb(it->first, notify);
        }
        delete[] clientIds;
    }

    return true;
}

bool
GeofenceAdapter::geofenceAlert(GeofenceAlertType alert)
{
    LOC_LOGL1("geofenceAlert: %d", alert);

    if (GEOFENCE_ALERT_MGP_UNAVAILABLE == alert)
    {
        LocationMetaData metaData = {0};
        metaData.size = sizeof(LocationMetaData);
        metaData.flags = LOCATION_META_DATA_ENGINE_STATUS;
        metaData.engineStatus = LOCATION_ENGINE_STATUS_LOAD_FAILED;

        // Report to all clients with meta data cb
        for (ClientDataMap::iterator it=mClientData.begin();
                it != mClientData.end(); ++it)
        {
            if (NULL != it->second.metaDataCb)
            {
                it->second.metaDataCb(it->first, metaData);
            }
        }
    }

    return true;
}

/*------------------------------------------------
  Engine up/down Events
  Invoked from QMI Thread
-------------------------------------------------*/
void
GeofenceAdapter::handleEngineUpEvent()
{
    struct MsgSSREvent : public LocMsg {
        GeofenceAdapter& mAdapter;
        inline MsgSSREvent(GeofenceAdapter& adapter) :
            LocMsg(),
            mAdapter(adapter) {}
        virtual void proc() const {
            mAdapter.restartGeofences();
        }
    };

    sendMsg(new MsgSSREvent(*this));
}

void
GeofenceAdapter::restartGeofences()
{
    if (mGeofences.empty()) {
        return;
    }

    GeofencesMap oldGeofences(mGeofences);
    mGeofences.clear();
    mGeofenceIds.clear();

    for (GeofencesMap::iterator it = oldGeofences.begin();
            it != oldGeofences.end(); it++) {
        GeofenceObject object = it->second;
        GeofenceOption options = {sizeof(GeofenceOption),
                                   object.breachMask,
                                   object.responsiveness,
                                   object.dwellTime};
        GeofenceInfo info = {sizeof(GeofenceInfo),
                             object.latitude,
                             object.longitude,
                             object.radius};
        uint32_t hwId = 0;
        mIzatApi->addGeofence(hwId, object.key.id, options, info, GF_CONFIDENCE_HIGH);
        if (true == object.paused) {
            mIzatApi->pauseGeofence(hwId, object.key.id);
        }
        saveGeofenceItem(object.key.client, object.key.id, hwId, options, info);
    }
}

/*------------------------------------------------
  Utility Methods
  Invoked from Message Thread
-------------------------------------------------*/
void
GeofenceAdapter::dump()
{
    LOC_LOGL0(
        "HAL | hwId  | mask | respon | latitude | longitude | radius | paused | clientId |");
    for (GeofencesMap::iterator it = mGeofences.begin();
        it != mGeofences.end();
        it++)
    {
        uint32_t hwId = it->first;
        GeofenceObject object = it->second;
        LOC_LOGL8("    | %5u | %4u | %6u | %d | %d | %d | %6u | %08x |",
                   hwId, object.breachMask, object.responsiveness,
                   (int)object.latitude, (int)object.longitude,
                   (int)object.radius, object.paused, object.key.id);
    }

    LOC_LOGL0("HAL | clientId | hwId     |");

    for (GeofenceIdMap::iterator it2 = mGeofenceIds.begin();
        it2 != mGeofenceIds.end();
        it2++)
    {
        uint32_t clientId = it2->first.id;
        uint32_t hwId = it2->second;
        LOC_LOGL2("    | %08x | %08x |", clientId, hwId);
    }
}
