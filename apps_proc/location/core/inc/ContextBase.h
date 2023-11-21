/* Copyright (c) 2011-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __LOC_CONTEXT_BASE__
#define __LOC_CONTEXT_BASE__

#include <stdbool.h>
#include <ctype.h>
#include <MsgTask.h>
#include <IzatApiBase.h>

namespace izat_core {

#define MAX_FEATURE_LENGTH 100

class IzatAdapterBase;
class IzatApiBase;

class ContextBase {
    IzatApiBase* createIzatApi(LOC_API_ADAPTER_EVENT_MASK_T exMask);
protected:
    const MsgTask* mMsgTask;
    IzatApiBase* mIzatApi;
public:
    ContextBase(const MsgTask* msgTask,
                LOC_API_ADAPTER_EVENT_MASK_T exMask,
                const char* libName);
    virtual ~ContextBase();
    inline const MsgTask* getMsgTask() { return mMsgTask; }
    inline IzatApiBase* getIzatApi() { return mIzatApi; }
    inline void sendMsg(const LocMsg *msg) { getMsgTask()->sendMsg(msg); }

    static bool sIsEngineCapabilitiesKnown;
    static uint64_t sSupportedMsgMask;
    static uint8_t sFeaturesSupported[MAX_FEATURE_LENGTH];
    static bool sGnssMeasurementSupported;

    void setEngineCapabilities(uint64_t supportedMsgMask,
            uint8_t *featureList, bool gnssMeasurementSupported);
    static inline bool isEngineCapabilitiesKnown() {
        return sIsEngineCapabilitiesKnown;
    }
    static bool isFeatureSupported(uint8_t featureVal);

};

} // namespace izat_core

#endif //__LOC_CONTEXT_BASE__
