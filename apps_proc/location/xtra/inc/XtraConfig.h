/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __XTRA_CONFIG_H__
#define __XTRA_CONFIG_H__

#include <comdef.h>
#include <stdlib.h>
#include <stdint.h>

#include <vector>
#include <string>
#include <algorithm>


using namespace std;
namespace izat_xtra {

class XtraConfig {
public:
    XtraConfig();
    ~XtraConfig();

    inline uint32_t getXtraVersionCheck() {
        return mXtraVersionCheck;
    }

    void updateXtraServers(const vector<string>& servers);
    void updateNtpServers(const vector<string>& servers);

    inline const vector<string>& getXtraServers() {
        return mXtraServers;
    }
    inline const vector<string>& getNtpServers() {
        return mNtpServers;
    }

    inline void shuffleXtraServers(vector<string>& servers) {
        random_shuffle(servers.begin(), servers.end());
    }

    inline bool getOnDemandTime() {
        return mOnDemandTime;
    }

    inline uint32_t getNtpInterval() {
        return mNtpIntervalSec;
    }

    inline uint32_t getNtpTimeDownloadDelay() {
        return mNtpTimeDownloadDelaySec;
    }

    inline uint32_t getXtraInterval() {
        return mXtraIntervalSec;
    }

    inline uint32_t getXtraIntInterval() {
        return mXtraIntIntervalSec;
    }

    inline uint32_t getRetryIntervalBase() {
        return mRetryIntervalBaseSec;
    }

    inline uint32_t getXtraRetryLongInterval() {
        return mXtraRetryLongIntervalSec;
    }

    inline uint32_t getXtraRetryInterval() {
        return mXtraRetryIntervalSec;
    }

    inline uint32_t getNtpRetryInterval() {
        return mNtpRetryIntervalSec;
    }

    inline uint32_t getXtraRetryTimesCap() {
        return mXtraRetryTimesCap;
    }

    inline uint32_t getNtpRetryTimesCap() {
        return mNtpRetryTimesCap;
    }

    inline bool getXtraTestEnabled() {
        return mXtraTestEnabled;
    }

    inline string& getXtraUaManufacturer() {
        return mXtraUaManufacturer;
    }

    inline string& getXtraUaModel() {
        return mXtraUaModel;
    }

    inline string& getXtraUaCarrier() {
        return mXtraUaCarrier;
    }

    inline string& getXtraUaBrand() {
        return mXtraUaBrand;
    }

    inline bool getXtraThrottleEnabled() {
        return mXtraThrottleEnabled;
    }

    inline string& getXtraCaPath() {
        return mXtraCaPath;
    }

    inline void setPreferedValidAge(uint16_t validAge){
        mXtraFilePreferedValidAgeInHrs = validAge;
    }
    inline uint16_t getPreferedValidAge(){
        return (mXtraFilePreferedValidAgeInHrs);
    }

    inline bool getXtraMpAudoDownloadTimerDisaled() {
        return mIsMpAutoDownloadTimerDisabled;
    }

    inline bool getXtraSysTimeInject() {
        return mSysTimeInject;
    }

    inline void setModemBootTime(int64_t modemBootTime){
        mModemBootTimeInSec = modemBootTime;
    }
    inline int64_t getModemBootTime() const{
        return mModemBootTimeInSec;
    }

    inline bool getXtraMpOnly() {
        return mXtraMpOnly;
    }
    inline bool getNtpMpOnly() {
        return mNtpMpOnly;
    }

    inline bool getXtraForceHttp() {
        return mXtraForceHttp;
    }
    inline bool getXtraForceHttps() {
        return mXtraForceHttps;
    }

    inline bool getXtraIntegrityDownloadEnabled() {
        return mXtraIntegrityDownloadEnabled;
    }

    inline uint32_t getDeviceProfileType() {
        return mDeviceProfileType;
    }

    inline uint32_t getXtraFileDurationHrs() {
        if (0 == mXtraFileDurationHrs) { // unspecified
            uint32_t profileType = getDeviceProfileType();
            switch(profileType) {
            case 1:
            case 2:
                return 168;
            case 3:
                return 72;
            case 4:
                return 24;
            default:
                return 72;
            }
        }
        return mXtraFileDurationHrs;
    }

    inline bool getXtraUrlDurationUpdateEnabled() {
        return mXtraUrlDurationUpdateEnabled;
    }

    inline bool getXtraUrlRegionUpdateEnabled() {
        return mXtraUrlRegionUpdateEnabled;
    }

    inline uint32_t getXtraIntRateCheckMinPerDay() {
        return mXtraIntRateCheckMinPerDay;
    }

    inline uint32_t getXtraIntBackoffRetryIntervalSec() {
        return mXtraIntBackoffRetryIntervalSec;
    }

	inline uint32_t getSockRecvTimeout() {
		return mSockRecvTimeout;
	}

	inline bool getSocketKeepAlive() {
		return mSocketKeepAlive;
	}

private:
    // XTRA servers
    vector<string> mXtraServers;

    // NTP servers
    vector<string> mNtpServers;

    // XTRA version check
    uint32_t mXtraVersionCheck;

    // on demand time
    bool mOnDemandTime;

    // for testing
    bool mXtraTestEnabled;

    // XTRA interval
    uint32_t mXtraIntervalSec;

    // XTRA integrity interval
    uint32_t mXtraIntIntervalSec;

    // NTP interval
    uint32_t mNtpIntervalSec;

    // NTP time download delay
    uint32_t mNtpTimeDownloadDelaySec;

    // short interval to retry
    uint32_t mRetryIntervalBaseSec;

    // long interval to retry after hitting th cap
    uint32_t mXtraRetryLongIntervalSec;

    // retry caps
    uint32_t mXtraRetryIntervalSec;
    uint32_t mNtpRetryIntervalSec;
    uint32_t mXtraRetryTimesCap;
    uint32_t mNtpRetryTimesCap;

    string mXtraUaManufacturer;
    string mXtraUaModel;
    string mXtraUaCarrier;
    string mXtraUaBrand;
    bool mXtraThrottleEnabled;

    // system time injection
    bool mSysTimeInject;

    // XTRA CA path
    string mXtraCaPath;

    uint16_t mXtraFilePreferedValidAgeInHrs;
    bool mIsMpAutoDownloadTimerDisabled;

    // latest modem boot time
    int64_t mModemBootTimeInSec;

    // Allow only MP requested downloads
    bool mXtraMpOnly;
    bool mNtpMpOnly;

    // Enforce HTTP or HTTPS despite Modem configuration
    // Both should never be set to true at same time
    bool mXtraForceHttp;
    bool mXtraForceHttps;

    // XTRA Integrity download config
    bool mXtraIntegrityDownloadEnabled;

    uint32_t mDeviceProfileType;
    uint32_t mXtraFileDurationHrs;
    bool mXtraUrlDurationUpdateEnabled;
    bool mXtraUrlRegionUpdateEnabled;

    uint32_t mXtraIntRateCheckMinPerDay;
    uint32_t mXtraIntBackoffRetryIntervalSec;

	/*XTRA sockets params*/
	uint32_t mSockRecvTimeout;
	bool mSocketKeepAlive;
};

} // end of namespace

#endif // __XTRA_CONFIG_H__
