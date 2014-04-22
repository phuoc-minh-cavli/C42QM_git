/******************************************************************************
Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __WIFI_POSITIONING_IZAT_ADAPTER__
#define __WIFI_POSITIONING_IZAT_ADAPTER__

#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <vector>
#include <string>
#include "qapi_location.h"

using namespace std;
using namespace izat_core;

namespace izat_wifi_positioning {

class WifiPositioningEngine;

typedef void (*sWifiAdapterSingleshotCb)(
        void* userDataPtr, const Location& location, LocationError err);

class WifiPositioningIzatAdapter : public IzatAdapterBase {
    public:
        WifiPositioningIzatAdapter();
        virtual ~WifiPositioningIzatAdapter();
		  /*========================================================================
          FUNCTION	cleanUpCommand

          DESCRIPTION 
          This function will be from locaton API to clean up wifipositioning adapter 

          PARAMETERS
          None.

          RETURN VALUE
          None
          ==========================================================================*/
        void cleanUpCommand();

		void RequestSingleShotWifiPosition();
		void handleRequestSingleShotWifiPosition();
		void StartTrackingWifiPosition(void* userDataPtr, sWifiAdapterSingleshotCb locationcb, int tbf);
		void StopTrackingWifiPosition();
		void init();
		void handleinit();
		void forwardMetaData(const LocationMetaData& metaData);

		void wifiPositioningEngineCb(const Location& location, LocationError err);
		virtual bool requestAdapterGetSingleshot(
		        const SingleShotOptions& options, SingleshotRequestType requestType,
		        IzatAdapterBase* requestorAdapter);

    private:
        WifiPositioningEngine* mWifiPositioningEngine;
		

};

} // end of namespace

#endif //__WIFI_POSITIONING_IZAT_ADAPTER__
