/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __XTRA_CLIENT_H__
#define __XTRA_CLIENT_H__
#include <string>
#include "XtraNvItems.h"
#include "XtraDataRequest.h"
#include "XtraTimeRequest.h"
#include "XtraIzatAdapter.h"
#include "XtraConfig.h"
#include "XtraTimer.h"
#include <vector>
#include <LocNetIface.h>
#include <LocOSIface.h>

using namespace std;

namespace izat_xtra {

enum REQ_SOURCE { REQ_SOURCE_BOOTUP,
                  REQ_SOURCE_WIFI_CONNECTION,
                  REQ_SOURCE_MP_REQUEST,
                  REQ_SOURCE_AUTO_DOWNLOAD,
                  REQ_SOURCE_CLIENT_REQUEST,
                  REQ_SOURCE_MAX = 0XFFFF};

enum STATE { STATE_IDLE,
             STATE_PENDING,
             STATE_PENDING_4XX_5XX,
             STATE_PENDING_LOCK,
             STATE_BUSY };

class XtraClient {
public:
    // constructor & destructor
    XtraClient();
    ~XtraClient();

    // AP triggered download
    void onDevicePowerUp();
    void onXtraDownloadTimerExpired();
    void onXtraIntegrityTimerExpired();
    void onXtraIntRetryTimerExpired();
    void onXtraRetryTimerExpired();

    // XTRA data request
    void onRequestXtraData(bool retry, uint32_t reqSource = REQ_SOURCE_MAX);
    void requestXtraData(bool retry, uint32_t reqSource);
    // XTRA INT request - return value indicates whether DL will be attempted
    // isBlockingRequest - whether this INT download is blocking a GNSS session start
    bool onRequestXtraIntegrity(
            bool retry, uint32_t reqSource = REQ_SOURCE_MAX,
            bool isBlockingRequest = false);
    bool requestXtraIntegrity(
        bool retry, uint32_t reqSource, bool isBlockingNewSessionStart);

    // These methods get invoked in XTRA MsgTask; and simply transfer control
    // to onXtraDownloadResultInternal()/onXtraIntegrityResultInternal()
    // which are executed in main LocThread MsgTask
    void onXtraDownloadResult(
            const XTRA_DOWNLOAD_STATUS status, const string& data);
    void onXtraIntegrityResult(
            const XTRA_DOWNLOAD_STATUS status, const string& data);

    void onXtraThrottle(
            const XTRA_DOWNLOAD_STATUS status, const string& data);

    // XTRA servers
    void handleReceiveXtraServers(const vector<string>& servers);
    void handleReceiveNtpServers(const vector<string>& servers);

    // XTRA time request
    LocationError onRequestNtpTime(bool retry, uint32_t reqSource = REQ_SOURCE_MAX);
    void onNtpDownloadResult(
            const bool success, const uint64_t time,
            const uint64_t reference, const uint64_t uncertainty);
    void onNtpDelayTimerExpired();

    // XTRA nv items
    inline XtraNvItems& getXtraNvItems() {
        return mXtraNvItems;
    }

    // XTRA config
    inline XtraConfig& getXtraConfig() {
        return mXtraConfig;
    }

    // Loc Net Iface
    inline LocNetIface& getLocNetIface(){
        return mLocNetIface;
    }

    // XTRA time request
    inline XtraTimeRequest& getXtraTimeRequest() {
        return mXtraTimeRequest;
    }

    // Currently handling XTRA or INT
    inline XTRA_DOWNLOAD_TYPE getCurrentRequestType() {
        return mCurrentRequestType;
    }

    // network status change
    void onConnectionStatusChange(bool connected, bool isWifi);

    // SSR
    void onHandleEngineUpEvent();

    //void sendMsg(function<void()>& runnable) const; // c++11 only
    void sendMsg(const Runnable& runnable, const void* dataPtr) const;
    void sendMsg(const LocMsg* msg) const;

    // traffic control
    bool checkXtraRequestRateLimit(bool isGnssTriggered, XTRA_DOWNLOAD_TYPE type);
    bool checkXtraRequestRetryLimit(const string& server, XTRA_DOWNLOAD_TYPE type);

    /* Computes the time remaining before next allowed Integrity
     * download, in seconds.
     * -1 for unavailable, 0 for immediate download, >0 for wait time. */
    int32_t getMinTimeToNextXtraIntDload();

    //Gps lock status changed.
    void onGpsLockUpdate(bool isGpsEnabled);
    void onGpsLockUpdateInternal(bool isGpsEnabled);

    inline void setIntRetry(bool retry) {
        mXtraIntRetry = retry;
    }
    inline bool isIntRetry() {
        return mXtraIntRetry;
    }

    string getXtraDataReqSur();

    void onReceiveXtraConfigInfo(uint32_t status, uint32_t cfgReqSource,
                                 uint32_t report_mask,
                                 uint16_t absAge, uint64_t relAge,
                                 uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                 const vector<string>& ntpServers,
                                 const vector<string>& xtraServers );

    inline int64_t elapsedRealtimeMs() const {
        return LocOSIface_getMillisSincePowerup();
    }
    inline void updateModemBootTime() {

        int64_t mModemBootTimeInSec = elapsedRealtimeMs() / 1000;
        mXtraConfig.setModemBootTime(mModemBootTimeInSec);
        mXtraNvItems.setModemBootTime(mModemBootTimeInSec);

        LOC_LOGM2("modem boot/reboot time set to 0x%x%x sec",
                mModemBootTimeInSec>>32, mModemBootTimeInSec);
    }
    inline int64_t getApBootTimeInSec() const {
        return elapsedRealtimeMs() / 1000;
    }
    inline void setModemBootTime(int64_t modemBootTime){
        mXtraConfig.setModemBootTime(modemBootTime);
    }
    inline int64_t getMpBootTimeInSec() const {
        return elapsedRealtimeMs() / 1000 - mXtraConfig.getModemBootTime();
    }

    void reportEngineStatusEvent(LocationMetaDataEngineStatus status);
    void reportEngineStatus(LocationMetaDataEngineStatus status);

    static inline MsgTask* getDlMsgTask() {

        if(sXtraNtpMsgTask == NULL) {
            sXtraNtpMsgTask = new MsgTask("XtraNtpDownloadMsgTask", false);
            if (sXtraNtpMsgTask == NULL) {
                LOC_LOGE0("Failed to create new MsgTask");
                return NULL;
            }
        }
        return sXtraNtpMsgTask;
    }

    XtraIzatAdapter* getXtraIzatAdapter();

private:
    // These methods are invoked in main LocThread MsgTask after
    // xtra/ntp download is complete in XTRA / NTP MsgTask
    void onXtraDownloadResultInternal(
            const XTRA_DOWNLOAD_STATUS status, const string& data);
    void onNtpDownloadResultInternal(
            const bool success, const uint64_t time, const uint64_t reference,
            const uint64_t uncertainty);
    void onXtraIntegrityResultInternal(
            const XTRA_DOWNLOAD_STATUS status, const string& data);

    void onReceiveXtraConfigInfoInternal(uint32_t status, uint32_t cfgReqSource,
                                         uint32_t report_mask, uint8_t timereqValid);

    uint32_t getMpXtraConfig();
    void onXtraFileAbsAgeUpdate(uint32_t reqSource);

    void calculateRetryDelay(const XTRA_DOWNLOAD_STATUS status,
            uint32_t &retryTimes, STATE &state, time_t &delaySec, XTRA_DOWNLOAD_TYPE type);

    STATE getXtraState();
    void setXtraState(STATE s);

private:
    STATE mXtraState;
    STATE mXtraIntState;
    STATE mNtpState;

    // XTRA Download Timers
    XtraTimer       mXtraTimer;
    XtraTimer       mXtraRetryTimer;
    XtraTimer       mBootUpDelayTimer;

    // XTRA Integrity Download Timers
    XtraTimer       mXtraIntTimer;
    XtraTimer       mXtraIntRetryTimer;

    XtraNvItems     mXtraNvItems;
    XtraDataRequest mXtraDataRequest;
    XtraTimeRequest mXtraTimeRequest;
    XtraConfig      mXtraConfig;

    // NTP Download Timers
    XtraTimer       mNtpTimer;
    XtraTimer       mNtpRetryTimer;
    XtraTimer       mNtpDelayTimer;

    uint32_t        mXtraRetryTimes;
    uint32_t        mXtraIntRetryTimes;
    uint32_t        mNtpRetryTimes;

    /** Xtra file Abs Age received from MP in units of Hrs **/
    uint16_t        mMpXtraFileAbsAgeHrs;
    /** Xtra file relative age received form MP in units of Sec **/
    uint64_t        mMpXtraFileRelAgeSec;

    uint32_t        mTimeReqSource;
    uint32_t        mXtraReqSource;

    // Interface to DSS library
    LocNetIface     mLocNetIface;
    static void locNetIfaceCallback(bool dataCallSuccess, void* xtraClientPtr);

    // XTRA and NTP download msg task
    static MsgTask* sXtraNtpMsgTask;

    // Has XTRA Integrity download retry been done
    bool            mXtraIntRetry;

    // Is this the first time engine is coming up ( or SSR )
    bool            mFirstEngineUp;

    // Current Request Type
    XTRA_DOWNLOAD_TYPE mCurrentRequestType;

    /** Set to true when boot up is done. **/
    bool            mIsbootupdone ;

    bool            mIsXtraInitDone;
	
    XtraIzatAdapter mXtraIzatAdapter;

    // This method gets invoked in main LocThread MsgTask
    static void sendMsgXtraDataRequest(void* xtraClientPtr);
    // This method gets invoked in XTRA msg task (sMsgTask)
    static void xtraDataRequestThread(void* xtraClientPtr);

    // This method gets invoked in main LocThread MsgTask
    static void sendMsgXtraTimeRequest(void* xtraClientPtr);
    // This method gets invoked in XTRA msg task (sMsgTask)
    static void xtraTimeRequestThread(void* xtraClientPtr);
};

} // end of namespace

#endif // __XTRA_CLIENT_H__
