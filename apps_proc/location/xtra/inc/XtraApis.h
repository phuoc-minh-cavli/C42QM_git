/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __XTRA_CLIENT_APIS_H__
#define __XTRA_CLIENT_APIS_H__

#include <string>
#include "XtraClient.h"
#include "XtraIzatAdapter.h"

namespace izat_xtra {
class IXtraInfoCache {
public:
   inline virtual void updateConnectionStatus(bool connected, int type) = 0;
   inline virtual bool getConnectionStatus() = 0;
   inline virtual void updateGpsLockStatus(const int gpslock) = 0;
   inline virtual bool isGpsEnabled() = 0;
   inline virtual void setXtraEnabled(bool isXtraEnabled) = 0;
   inline virtual bool isXtraEnabled() = 0;
   inline virtual bool isConnectionWifi() = 0;
   inline virtual void updateXtraMccmnc(const std::string& mccmnc) = 0;
   inline virtual const std::string& getXtraMccmnc() = 0;
   inline virtual void updateXtraTac(const std::string& tac) = 0;
   inline virtual const std::string& getXtraTac() = 0;
   inline virtual void setConnectionStatusIsKnown() = 0;
   inline virtual bool getConnectionStatusIsKnown() = 0;
   inline virtual ~IXtraInfoCache() { /*noop*/ };
};

IXtraInfoCache& getXtraInfoCache();
void initXtra();
void deinitXtra();
XtraClient* getXtraClient();
XtraIzatAdapter* getXtraAdapter();
} // end of namespace

#endif //__XTRA_CLIENT_APIS_H__
