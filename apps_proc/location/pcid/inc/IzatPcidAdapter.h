/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __IZAT_PCID_ADAPTER__
#define __IZAT_PCID_ADAPTER__

#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <vector>
#include <string>

#include "IzatPcid.h"

using namespace std;
using namespace izat_core;

namespace izat_pcid {

class IzatPcidAdapter : public IzatAdapterBase {
public:
    IzatPcidAdapter();
    virtual ~IzatPcidAdapter();

    // modem synchronization
    void onSyncModem();

    // SSR
    virtual void handleEngineUpEvent();

private:
    // PCID injection
    void injectIzatPcid(IzatPcid_t devId);
};

} // end of namespace

#endif // __IZAT_PCID_ADAPTER__
