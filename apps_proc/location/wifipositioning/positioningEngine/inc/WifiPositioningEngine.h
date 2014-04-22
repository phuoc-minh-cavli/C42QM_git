/******************************************************************************
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef __WIFI_POSITIONING_ENGINE_H__
#define __WIFI_POSITIONING_ENGINE_H__
#include <string>

#include <loc_util_log.h>
#include <vector>
#include <LocNetIface.h>
#include "WifiPositioningIzatAdapter.h"
#include "WifiPositioningTimer.h"
#include "LocationWifiInterface.h"
#include <loc_misc_utils.h>
#include <LocTimer.h>
#include <qurt_mutex.h>


#ifdef __cplusplus
extern "C" {
#endif
#include "libel.h"
#include "crc32.h"
#include "beacons.h"
#ifdef __cplusplus
}
#endif

using namespace std;
using namespace izat_location_wifi_interface;

#define MAX_AP_SUPPORTED 20



namespace izat_wifi_positioning {

typedef enum
{
	WIFI_POS_SESSION_UNKNOWN,
	WIFI_POS_SESSION_SINGLE_SHOT,
	WIFI_POS_SESSION_START_TRACKING,
	WIFI_POS_SESSION_STOP_TRACKING
}wifiPos_sessionType;

typedef enum
{
    STATE_UNKNOWN,
    STATE_CONNECTED,
    STATE_DISCONNECTED
}dataConnection_state;

typedef enum
{
    STATE_IDLE,
    STATE_DOWNLOAD_PENDING,
    STATE_DOWNLOADED
}resp_download_state;

typedef enum {
    WIFI_PE_DOWNLOAD_UNKNOWN = -1,
    WIFI_PE_DOWNLOAD_SUCCESS = 0,
    WIFI_PE_DOWNLOAD_FAILURE
}wifiPE_download_status;

typedef enum {
    WIFI_PE_CONNECTION_UNKNOWN = -1,
    WIFI_PE_CONNECTION_SUCCESS = 0,
    WIFI_PE_CONNECTION_FAIL_URL,
    WIFI_PE_CONNECTION_FAIL_HTTP,
    WIFI_PE_CONNECTION_FAIL_NEW_SESSION,
    WIFI_PE_CONNECTION_FAIL_CONNECTION,
    WIFI_PE_CONNECTION_FAIL_SET_BODY,
    WIFI_PE_CONNECTION_FAIL_REQUEST,
    WIFI_PE_CONNECTION_FAIL_RESPONSE,
    WIFI_PE_CONNECTION_FAIL_SOCKET,
    WIFI_PE_CONNECTION_FAIL_HOST
}wifiPE_connection_status;

/** Macro for  Network Bearer values*/
#define LTE_TDD         5
#define LTE_FDD         6
#define GSM_NW_BEARER         0
#define WCDMA_NW_BEARER         2

#define MODE_LTE_WB 0x01 /**<  Camped on LTE wideband \n  */
#define MODE_LTE_M1 0x02 /**<  Camped on LTE M1 \n	*/
#define MODE_LTE_NB1 0x03 /**<	Camped on LTE NB1  */


typedef struct {
	bool validCellInfo;
	uint8 networkType;
	uint8 LTEOpMode;
	uint16_t mcc;
	uint16_t mnc;
	uint16_t tac;
	uint16_t lac;
	uint32_t cellId;
	int16_t rsrp;	
}cellInfo;

typedef enum {
	WIFI_LIB_LOG_DISABLED = 0,
    WIFI_LIB_LOG_CRITICAL = 1,
    WIFI_LIB_LOG_ERROR,
    WIFI_LIB_LOG_WARNING,
    WIFI_LIB_LOG_DEBUG,
    WIFI_LIB_LOG_ALL = WIFI_LIB_LOG_DEBUG,
} Wifi_lib_log_level_t;



extern void *wifiPE_malloc_internal
(
  size_t  memsize, int line
);
#define wifiPE_malloc(a) wifiPE_malloc_internal(a, __LINE__)

extern void WifiPE_free_internal
(
  void  *ptr, int line
);
#define wifiPE_free(a) wifiPE_free_internal(a, __LINE__)

class WifiPositioningEngine {
	private:
        MsgTask*             mWifiPEMsgTask;  // Wifi PE  msg task
        dataConnection_state mDataConnState;
        resp_download_state mDownloadState;
    protected:
        // constructor & destructor
        WifiPositioningEngine(WifiPositioningIzatAdapter* adapter);
        ~WifiPositioningEngine();
    public:
		static WifiPositioningEngine* sWifiPEInstance;

        //this is used to establish data call
        LocNetIface mLocNetIface;
		WifiPositioningIzatAdapter* mWifiPositioningIzatAdapter;
		bool mSkyLibIntialized;
		bool mWaitingforScanResults;
		wifiPos_sessionType mWifiPosSessionType;
		bool mCellInfoAvailable;
		Sky_ctx_t *mSkyCtx;
		void* mServerResponse;
		WifiPositioningTimer mTBFTimer;
		WifiPositioningTimer mPosReqTimer;
		LocationWifiInterface* mLocationWifiInterface;
		MsgTask* mMsgTask;
		uint32_t mClientReqId;
		bool mWifiMacIdValid;
		uint32_t mTbf;
		uint64_t mPosRequestTimeStamp;
		uint32_t mWorkspaceSize;
		uint32_t mServerPort;
		bool mPendingPosRequest;
		bool mWifiPositioningMode;
		Wifi_lib_log_level_t mSkyLoglevel;
		LocationAPI* mLocationAPI;
		bool mSensorOptimizationEnabled;
		uint32_t mNumOfCachePosThreshold;
		uint32_t mNumOfCachePosSent;
		uint8_t mWifiMacAddress[MAC_ADDR_LENGTH_BYTES_MAX];
		//server config items
		uint32_t mPartnerId;
		uint32_t mAesKeyId;
		uint8_t mAesKey[AES_SIZE];
		string mServerUrl;
		cellInfo mCellInfo;
		vector<vector<int> > mRegulatedMCCMNCList;
		vector<int> mRegulatedMCCList;
		Location mLastWifiLocation;
		LocationMotionInfo mLastMotionInfo;
		uint64_t mMotionAgeThresholdMs;
		uint64_t mLastPosAgeThresholdMs;
		
		
		
		 /**
         * @brief this will be invoked once you get http response
         *
         * @details once invoked ,will handle response in this function based on the state we receive
         * @param[in] arg	 this is parameter returned is same parameter which is send in request  .
         * @param[in] state   describe about state whether we received complete or time out etc
         *@param[in] http_resp contains response
         *
         * @return void
         */
        static void  server_response_cb(void *arg, int32_t state, void *http_resp);
		static void scan_result_cb(Location_Wifi_Interface_Operation_Info *pScanInfo);
		void stopTrackingSession();
		void onTBFTimerExpired();
		void onPosReqTimerExpired();
		void handleMotionResponse(LocationMotionInfo motionInfo);
		static void locNetIfaceCallback(bool dataCallSuccess, void* wifiPEPtr);
		static WifiPositioningEngine* createInstance(WifiPositioningIzatAdapter* adapter);
        static  void destroyInstance();

        void sendMsg(const Runnable& runnable, const void* dataPtr) const;
        void sendMsg(const LocMsg* msg) const;
		void handleConnectionAndWifiScans();
		MsgTask* getMsgTaskInstance();
		//void handleServerDownload(void* data,uint32_t length, uint32_t response_size);
		void handleConnectionAbort();
		void handlePendingRequests(Location_Wifi_Interface_Operation_Info wifiOperationOInfo);
		//void handleServerResponse(const wifiPE_connection_status status, uint32_t length);
		void decodeServerResponse(const wifiPE_connection_status status,void* data,uint32_t length);
		void handleScanResponse(Location_Wifi_Interface_Operation_Info *pScanInfo);
		int handleScanResult(Location_Wifi_Interface_Operation_Info wifiOperationInfo);
		int handlePositionRequest();
        bool init();
	private:
		void deInit();
		int readWifiConfigItems();
		int initSkyLib();
		wifiPE_connection_status sendPositionRequestToServer(void* data,uint32_t length, uint32_t response_size);

		wifiPE_connection_status sendPositionRequest(void* data,uint32_t length, uint32_t response_size);
		int processPendingPositionRequest(Location_Wifi_Interface_Operation_Info wifiOperationInfo);
		int getCurrentCellInfo();
		uint32 readNvItem(const string& name);
		void saveNvItem(const string& name, const string& value);
		int openNvDb();
		void closeNvDb();
		void sendLocationReport(Location location,
				LocationError err);
		uint32_t hex2bin(unsigned char *hexstr, uint32_t hexlen, uint8_t *result, uint32_t reslen);
		void populateMccMncList(string embargoed_mcc_list);
		unsigned char* decodeEmbargoedList(string embargoed_mcc_list);
		void encodeEmbargoedList(string embargoed_mcc_list);
		bool isDeviceInRegulatedArea();
		qapi_Location_Error_t StartMotionTracking();
		
		void handleMotionResult(LocationMotionInfo motionInfo);
		bool checkLocationValidity(Location location);
		bool isDeviceStaticAndLastLocationValid();
		void reportWIFIPEStatusEvent(LocationMetaDataEngineStatus status);
		void reportWIFIPEStatus(LocationMetaDataEngineStatus status);


};

} // end of namespace

#endif // __WIFI_POSITIONING_ENGINE_H__
