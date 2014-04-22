/*=========================================================================
  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef FLP_LOCATION_ADAPTER_H
#define FLP_LOCATION_ADAPTER_H

#include <IzatAdapterBase.h>
#include <LocationAPI.h>
#include <FlpTimer.h>
#include <SystemStatus.h>
#include <map>
#include <gps_extended_c.h>
#include <WifiPositioningIzatAdapter.h>
#include <FlpSlimClient.h>
#include <GtpWwanNvItems.h>

using namespace izat_core;
using namespace izat_wifi_positioning;
using namespace izat_gtpwwan;

#define FLP_SINGLE_SHOT_TIMER_INTERVAL 5 // seconds
#define FLP_SINGLE_SHOT_GNSS_TIMEOUT 15000 // millis

#define FLP_NMEA_PROVIDER_AP 0
#define FLP_NMEA_PROVIDER_MP 1

/* SingleShot session info */
typedef struct {
    uint64_t sessionStartTime;
    SingleShotOptions options;
    IzatAdapterBase* requestorAdapter;
    SingleshotRequestType requestType;
    boolean isWeakGnssSignal;
    Location lastRecvdLocation;
} SingleShotSessionInfo;

class FlpLocationAdapter : public IzatAdapterBase {

private:
    typedef std::map<LocationAPI*, LocationClientInfo> ClientDataMap;
    ClientDataMap mClientData;

    LocationSessionMap mTimeBasedTrackingSessions;
    LocationSessionMap mDistanceBasedTrackingSessions;
    LocationSessionMap mBatchingSessions;

    MotionSessionMap mMotionTrackingSessions;

    LocationControlAPI* mControlClient;
    LocationControlCallbacks mControlCallbacks;

    typedef std::map<LocationSessionKey, SingleShotSessionInfo> SingleShotSessionMap;
    SingleShotSessionMap mSingleShotSessions;

    uint32_t mGnssDataSessionCount;
    bool mGnssSvIdUsedInPosAvail;
    GnssSvUsedInPosition mGnssSvIdUsedInPosition;

    // Singleshot session pending flags
    bool mIsSingleshotGnssCbPending;
    bool mIsSingleshotWwanCbPending;
    bool mIsSingleshotMcsCbPending;
    bool mIsSingleshotWifiCbPending;

    /*************************Constellation Mask***********************
      #define LOC_GNSS_CONSTELLATION_MASK_GPS          1<<0
      #define LOC_GNSS_CONSTELLATION_MASK_GALILEO      1<<1
      #define LOC_GNSS_CONSTELLATION_MASK_GLONASS      1<<2
      #define LOC_GNSS_CONSTELLATION_MASK_QZSS         1<<3
      #define LOC_GNSS_CONSTELLATION_MASK_BEIDOU       1<<4
      #define LOC_GNSS_CONSTELLATION_MASK_NAVIC        1<<5
    *************************Constellation Mask***********************/
    uint32_t mActiveConstellationMask;
    uint32_t getActiveConstellationFromXtraUrl(const std::string& path);

public:
    FlpLocationAdapter(LocationConfig config);

    inline virtual ~FlpLocationAdapter() {
        LOC_LOGL0("Inside ~FlpLocationAdapter");
#ifndef SENSORS_DISABLED
        if (NULL != mSlimClient) {
            delete mSlimClient;
        }
#endif
        if (NULL != mGtpWwanNvItems) {
            delete mGtpWwanNvItems;
        }
    }

    /* Config values common for all sessions */
    LocationConfig mConfig;

    bool mFirstEngineUpEventRecvd;

    /* Timer for single shot state machine control */
    FlpTimer mSingleShotTimer;

    /* === SystemStatus ===================================================================== */
    SystemStatus* mSystemStatus;

    GtpWwanNvItems* mGtpWwanNvItems;
#ifndef SENSORS_DISABLED
    // SLIM
    FlpSlimClient* mSlimClient;
#endif

    boolean mIsGnssLoaded;

    /* Adapter init/deinit Commands */
    void initializeCommand(size_t batchSize);
    void cleanUpCommand();
    void configCommand();

    /* Client Management Commands */
    void addClientCommand(LocationAPI* client, const LocationCallbacks& callbacks);
    void removeClientCommand(LocationAPI* client);
    void setControlCallbacksCommand(LocationControlCallbacks& controlCallbacks);

    /* SingleShot API Commands */
    uint32_t getSingleShotCommand(
            LocationAPI* client, const SingleShotOptions& options,
            IzatAdapterBase* requestorAdapter = NULL,
            SingleshotRequestType requestType = SINGLESHOT_REQUEST_TYPE_INVALID);
    void cancelSingleShotCommand(LocationAPI* client, uint32_t id);
    // to be processed in location thread
    void getSingleShot(
            LocationAPI* client, uint32_t sessionId, SingleShotOptions options,
            IzatAdapterBase* requestorAdapter = NULL,
            SingleshotRequestType requestType = SINGLESHOT_REQUEST_TYPE_INVALID);

    /* Best Available Position */
    uint32_t getBestAvailablePositionCommand(LocationAPI* client);

    /* Tracking API Commands */
    uint32_t startTrackingCommand(LocationAPI* client, LocationOptions& options, bool isGnssDataRequested);
    void updateTrackingOptionsCommand(
            LocationAPI* client, uint32_t id, LocationOptions& options);
    void stopTrackingCommand(LocationAPI* client, uint32_t id, bool isGnssDataRequested);

    /* Batching API Commands */
    uint32_t startBatchingCommand(
            LocationAPI* client, LocationOptions& locationOptions);
    void updateBatchingOptionsCommand(
            LocationAPI* client, uint32_t id,
            LocationOptions& locationOptions);
    void stopBatchingCommand(LocationAPI* client, uint32_t id);
    void getBatchedLocationsCommand(LocationAPI* client, uint32_t id, size_t count);

    /* Meta data commands */
    uint32_t getBackoffTimeCommand(LocationAPI* client);
    uint32_t getGnssEnergyConsumedCommand(LocationAPI* client);

    /* Motion Tracking commands */
    uint32_t startMotionTrackingCommand(LocationAPI* client, LocationMotionOptions& options);
    void stopMotionTrackingCommand(LocationAPI* client, uint32_t id);

    /* Motion Tracking Response API
     * Invoked without context switch, must be done before processing */
    void reportMotionStateEvent(const LocationMotionInfo motionInfo);
    /* Invoked after context switch to location context */
    void reportMotionState(const LocationMotionInfo& motionInfo);

    /* Motion Tracking SLIM service update notification */
    void motionTrackingServiceChangeEvent(LocationMotionType serviceType, bool isAvailable);
    void motionTrackingServiceChange(LocationMotionType serviceType, bool isAvailable);

    /* GTP WWAN Command*/
    void enableGtpWwanCommand(void);
    void disableGtpWwanCommand(void);

    /* Control Commands */
    void enableCommand(LocationTechnologyType techType);
    void disableCommand(LocationTechnologyType techType);

    /* Response APIs
     * IzatApi performs the context switch to MsgTask before invoking these */
    virtual bool reportLocation(const Location& location,
            const GpsLocationExtended& locationExtended, enum loc_sess_status status);
    virtual bool reportLocations(Location* locations, size_t count);
    virtual bool reportWwanZppFix(const Location& location, LocationError err, bool isBestAvailPosition=false);
    virtual bool reportWifiFix(const Location& location, LocationError err);

    void reportResponse(LocationAPI* client, LocationError err, uint32_t sessionId);
    void reportControlResponse(LocationError err);

    virtual bool reportNmea(const char* nmea, int length, NmeaSource source);
    bool reportJammer(GnssData gnssData);

    virtual bool reportSv(GnssSvNotification& svInfo);

    /* Engine up/down Events */
    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();

    /* Incoming request from other adapters */
    virtual bool requestAdapterGetSingleshot(
            const SingleShotOptions& options, SingleshotRequestType requestType,
            IzatAdapterBase* requestorAdapter);

    /* Internal utility methods */
    void setBatchSize(size_t size);
    void saveClient(LocationAPI* client, const LocationCallbacks& callbacks);
    void eraseClient(LocationAPI* client);
    void updateClientLoadSuccess(LocationAPI* client, bool isEngineLoadSuccessReported);
    void updateClientLoadFailure(LocationAPI* client, bool isEngineLoadFailureReported);
    void stopClientSessions(LocationAPI* client);
    void stopAllClientSessions();
    LocationError startTimeBasedTrackingMultiplex(const LocationOptions& options);
    LocationError stopTimeBasedTrackingMultiplex(LocationAPI* client, uint32_t id);
    LocationError restoreTimeBasedTrackingMultiplex();
    LocationError updateTimeBasedTrackingMultiplex(
            const LocationOptions& oldOptions, const LocationOptions& newOptions);

    bool hasTrackingCallback(LocationAPI* client);
    bool anyTrackingSessionActive(LocationAPI* client);
    bool anyTimeBasedTrackingSessionActive();
    bool isTimeBasedTrackingSession(LocationAPI* client, uint32_t sessionId);
    bool isDistanceBasedTrackingSession(
            LocationAPI* client, uint32_t sessionId);
    void saveTrackingSession(LocationAPI* client, uint32_t sessionId,
                             const ExtendedLocationOptions& extOptions);
    void eraseTrackingSession(LocationAPI* client, uint32_t sessionId);
    void getTrackingSessionOptions(LocationAPI* client, uint32_t sessionId,
            ExtendedLocationOptions& extOptions);

    bool hasMotionTrackingCallback(LocationAPI* client);
    bool anyMotionTrackingSessionActive(
            LocationAPI* client = NULL, LocationMotionTypeMask mask = 0);
    bool isMotionTrackingSession(LocationAPI* client, uint32_t sessionId);
    void saveMotionTrackingSession(LocationAPI* client, uint32_t sessionId,
                                   const LocationMotionOptions& options);
    void eraseMotionTrackingSession(LocationAPI* client, uint32_t sessionId);
    void getMotionTrackingSessionOptions(LocationAPI* client, uint32_t sessionId,
                                         LocationMotionOptions& extOptions);

    bool anySingleShotSessionActive();
    bool anySingleShotSessionActive(LocationAPI* client);
    bool anySingleShotGnssSessionActive();
    bool anySingleShotWwanSessionActive();
    bool isSingleShotSession(LocationAPI* client, uint32_t sessionId);
    bool hasSingleShotCallback(LocationAPI* client);
    singleShotCallback getSingleShotCallback(LocationAPI* client);
    void saveSingleShotSession(
            LocationAPI* client, uint32_t sessionId,
            SingleShotOptions options, IzatAdapterBase* requestorAdapter = NULL,
            SingleshotRequestType requestType = SINGLESHOT_REQUEST_TYPE_INVALID);
    void eraseSingleShotSession(
            LocationAPI* client, uint32_t sessionId);
    void getSingleShotSessionOptions(
            LocationAPI* client, uint32_t sessionId, SingleShotOptions& options);
    void clearAllSingleShotSessions();
    void singleShotTimerCallback();
    void singleShotGnssTimeout();
    bool meetsAccuracyThreshold(const Location& location, const SingleShotOptions& options);
    bool anyMatchingSingleShotSessionActive(const SingleShotOptions& options);
    SingleshotRequestType getSingleShotRequestType(const SingleShotOptions& options);
    bool isEngineLoadSuccessReported(LocationAPI* client);
    bool isEngineLoadFailureReported(LocationAPI* client);
    void updateLastRecvdGnssLocation(Location location);
    void updateGnssSignalStrength(GnssSvNotification& svInfo);

    bool hasBatchingCallback(LocationAPI* client);
    bool anyBatchingSessionActive(LocationAPI* client);
    bool isBatchingSession(LocationAPI* client, uint32_t sessionId);
    void saveBatchingSession(LocationAPI* client, uint32_t sessionId,
                             const LocationOptions& options);
    void eraseBatchingSession(LocationAPI* client, uint32_t sessionId);
    void restartSessions();
    LocationControlCallbacks& getControlCallbacks() { return mControlCallbacks; }
    void setControlCallbacks(const LocationControlCallbacks& controlCallbacks) {
        mControlCallbacks = controlCallbacks;
    }
    void incrementGnssDataSessionCount() {
        mGnssDataSessionCount++;
    }
    void decrementGnssDataSessionCount() {
        if (mGnssDataSessionCount > 0) {
            mGnssDataSessionCount--;
        }
    }
    uint32_t getGnssDataSessionCount() {
        return mGnssDataSessionCount;
    }
    metaDataCallback getMetaDataCallback(LocationAPI* client);
    virtual bool reportMetaData(const LocationMetaData& metaData, LocationAPI* client = NULL);
    LocationMetaDataEngineStatus reportMetaDataForSessionStatus(enum loc_sess_status status);
    void reportMetaDataEngineStatus(LocationMetaDataEngineStatus status, LocationAPI* client = NULL);

    // WIFI Positioning
    void wifiAdapterSingleshotCb(const Location& location, LocationError err);
    void gnssDeleteAidingDataCommand(GnssAidingData& data);
    void deleteAidingData(GnssAidingData data);
    virtual bool onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                      uint32_t report_mask,
                                      uint16_t absAge, uint64_t relAge,
                                      uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                      const char* ntpServer1, const char* ntpServer2,
                                      const char* ntpServer3,
                                      const char* xtraServer1, const char* xtraServer2,
                                      const char* xtraServer3);
};

#endif /* FLP_LOCATION_ADAPTER_H */
