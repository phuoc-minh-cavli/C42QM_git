/*=========================================================================
  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef LOCATION_WIFI_INTERFACE_H
#define LOCATION_WIFI_INTERFACE_H

#include <IzatAdapterBase.h>
#include <LocationAPI.h>
#include <SystemStatus.h>
#include <map>
#include <gps_extended_c.h>
#include <loc_util_log.h>
#include <vector>
#include <loc_misc_utils.h>
#include <LocationWifiInterfaceTimer.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "qca4004_api.h"
#ifdef __cplusplus
}
#endif

using namespace std;
using namespace izat_core;

namespace izat_location_wifi_interface {

#define MAC_ADDR_LENGTH_BYTES_MAX  6    /* MAC address length in bytes */   
#define SSID_STR_LENGTH_MAX 32          /* SSID string length in bytes */
#define WIFI_SCAN_TIMEOUT_S 5000           /* Wifi scan request timeout */
#define WIFI_DRIVER_ON_REQ_TIMEOUT_S 5000       // seconds
#define WIFI_DRIVER_KEEP_ON_TIMEOUT_S 5000      // seconds

#define NUM_SCAN_APS_INVALID 0xFF

enum location_wifi_interface_op_status{
    OP_STATUS_SUCCESS = 0,              /**< Success. */
    OP_STATUS_GPIO_FAILURE,             /**< GPIO operation failure. */	
    OP_STATUS_DRIVER_INACTIVE,         /**< Wifi driver not responding. */
    OP_STATUS_GENERAL_FAILURE,         /**< General failure. */
    OP_STATUS_DRIVER_ON,               /**< Wifi driver ON. */
    OP_STATUS_DRIVER_OFF,              /**< Wifi driver OFF. */
	   OP_STATUS_TIMEOUT,                 /**< Operation timeout. */
} ;



enum location_wifi_interface_op_type{
  OP_TYPE_WIFI_SCAN = 0,
  OP_TYPE_DRIVER_ADDRESS,
  OP_TYPE_UNKNOWN,
};

 /**
 * Wifi driver power state.
 */
enum location_wifi_interface_driver_state{
  DRIVER_STATE_POWER_OFF = 0,              /**< Power On. */
  DRIVER_STATE_POWER_ON,                 /**< Power Off. */
	 DRIVER_STATE_POWER_MAX,                
};


 /**
 * The Wifi AP information
 */
typedef struct
{
  int32_t channel;                                 /** Channel on which beacon was recieved. */ 
  int8_t rssi;                                     /** Signal strength in 0.5dBm */
	 unsigned char bssid[MAC_ADDR_LENGTH_BYTES_MAX];   /** Mac address of the AP */
  unsigned char ssid_len;   
  unsigned char ssid[SSID_STR_LENGTH_MAX];          /** SSID info */
} Location_Wifi_Interface_Ap_Info;


 /**
 * The Wifi Scan information
 */
typedef struct
{
  uint32_t requestId;
  location_wifi_interface_op_status opStatus;
  location_wifi_interface_op_type opType;
  uint8_t numAps;
  uint8_t macAddWifiChip[MAC_ADDR_LENGTH_BYTES_MAX];
  vector<Location_Wifi_Interface_Ap_Info> pApInfo;
  time_t scanTimestamp;
}Location_Wifi_Interface_Operation_Info;

typedef void (*OpResultCb)(Location_Wifi_Interface_Operation_Info *pScanInfo);

class LocationWifiInterface{

	private:
        static LocationWifiInterface* sWifiInterfaceInstance;
        OpResultCb mClientScancb;
        uint32 mDriverOnRequestId;
        uint32 mScanRequestId;
        uint32 mDriverOnRetryCnt;
        location_wifi_interface_driver_state mDriverState;
        uint8_t mMacAddWifiChip[MAC_ADDR_LENGTH_BYTES_MAX];
        bool mMacAddWifiChipAvail;
        bool mScanRequested;
        bool mMacAddWifiChipRequested;
        /* Timer for wifi driver control */
        LocationWifiInterfaceTimer mWifiDriverOnReqTimer;

        /* Timer for wifi driver max switched ON time */
        LocationWifiInterfaceTimer mWifiDriverKeepOnTimer;

        /* Timer for wifi scan control */
        LocationWifiInterfaceTimer mWifiScanReqTimer;



    protected:
        // constructor & destructor
        LocationWifiInterface(void);
        ~LocationWifiInterface();
    public:

        static LocationWifiInterface* getInstance();
        static  void destroyInstance();
          
        void sendMsg(const Runnable& runnable, const void* dataPtr) const;
        void sendMsg(const LocMsg* msg) const;

        void requestWifiScan(uint32_t clientReqId);

        void requestWifiChipMac(uint32_t clientReqId);

        /*========================================================================
          FUNCTION	initLocationWifiInterface

          DESCRIPTION
          this function is used to initialise the wifi location interface

          PARAMETERS
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          ==========================================================================*/
        bool initLocationWifiInterface(OpResultCb ScanCbFn);



        void ScanStateCb(	uint32_t requestId,
	                         location_wifi_interface_op_status errCode,
                          unsigned char macIdWifiChip[6],
                          uint8_t numAps,
	                         void* pOut);


       void PowerStateCb(uint32_t requestId,
	                        location_wifi_interface_op_status errCode,
	                        location_wifi_interface_driver_state state,
                         unsigned char macAddWIfiChip[6]);

        void wifiDriverOnReqTimerExpired();

        void wifiDriverKeepOnTimerExpired();

        void wifiScanReqTimerExpired();

        
};

}// end of namespace

#endif /* LOCATION_WIFI_INTERFACE_H */
