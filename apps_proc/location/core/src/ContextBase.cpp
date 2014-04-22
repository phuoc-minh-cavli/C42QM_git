/* Copyright (c) 2011-2018, The Linux Foundation. All rights reserved.
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
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_CtxBase"

#include "stringl.h"
#include <dlfcn.h>
#include <ContextBase.h>
#include <msg_q.h>
#include <loc_target.h>
#include <log_util.h>
#include <loc_log.h>

#ifndef LOCATION_MODULE_DISABLED
    #include "IzatApiV02.h"
#endif

using namespace izat_core;

namespace izat_core {

bool ContextBase::sIsEngineCapabilitiesKnown = false;
uint64_t ContextBase::sSupportedMsgMask = 0;
uint8_t ContextBase::sFeaturesSupported[MAX_FEATURE_LENGTH];
bool ContextBase::sGnssMeasurementSupported = false;

ContextBase::~ContextBase()
{
    delete mIzatApi;
}

IzatApiBase* ContextBase::createIzatApi(LOC_API_ADAPTER_EVENT_MASK_T exMask)
{
    LOC_LOGM0("Enter ContextBase::createIzatApi");
    IzatApiBase* izatApi = NULL;
#ifndef LOCATION_MODULE_DISABLED
    izatApi = new IzatApiV02(mMsgTask, exMask, this);
#else
    LOC_LOGE0("GPS Disabled, IZAT APIs not available");
#endif
    return izatApi;
}

ContextBase::ContextBase(const MsgTask* msgTask,
                         LOC_API_ADAPTER_EVENT_MASK_T exMask,
                         const char* libName) :
    mMsgTask(msgTask),
    mIzatApi(createIzatApi(exMask))
{
    LOC_LOGM0("ContextBase constructor");
}

void ContextBase::setEngineCapabilities(uint64_t supportedMsgMask,
       uint8_t *featureList, bool gnssMeasurementSupported) {

    if (ContextBase::sIsEngineCapabilitiesKnown == false) {
        ContextBase::sSupportedMsgMask = supportedMsgMask;
        ContextBase::sGnssMeasurementSupported = gnssMeasurementSupported;
        if (featureList != NULL) {
            memscpy((void *)ContextBase::sFeaturesSupported,
                    sizeof(ContextBase::sFeaturesSupported),
                    (void *)featureList, sizeof(ContextBase::sFeaturesSupported));
        }

        ContextBase::sIsEngineCapabilitiesKnown = true;
    }
}

bool ContextBase::isFeatureSupported(uint8_t featureVal)
{
    uint8_t arrayIndex = featureVal >> 3;
    uint8_t bitPos = featureVal & 7;

    if (arrayIndex >= MAX_FEATURE_LENGTH) return false;
    return ((ContextBase::sFeaturesSupported[arrayIndex] >> bitPos ) & 0x1);
}

}
