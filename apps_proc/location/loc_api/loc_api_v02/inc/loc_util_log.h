/* Copyright (c) 2011-2012, 2016,  The Linux Foundation. All rights reserved.
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
 */
#ifndef LOC_UTIL_LOG_H
#define LOC_UTIL_LOG_H

#if defined(_ANDROID_)
#include "loc_api_v02_log.h"
#include <log_util.h>

#else // no _ANDROID_

#include "msg.h"


// Non string logging
#define LOC_LOGE0(fmt, ...) MSG(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH0(fmt, ...) MSG(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM0(fmt, ...) MSG(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE1(fmt, ...) MSG_1(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH1(fmt, ...) MSG_1(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM1(fmt, ...) MSG_1(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE2(fmt, ...) MSG_2(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH2(fmt, ...) MSG_2(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM2(fmt, ...) MSG_2(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE3(fmt, ...) MSG_3(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH3(fmt, ...) MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM3(fmt, ...) MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE4(fmt, ...) MSG_4(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH4(fmt, ...) MSG_4(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM4(fmt, ...) MSG_4(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE5(fmt, ...) MSG_5(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH5(fmt, ...) MSG_5(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM5(fmt, ...) MSG_5(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE6(fmt, ...) MSG_6(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH6(fmt, ...) MSG_6(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM6(fmt, ...) MSG_6(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE7(fmt, ...) MSG_7(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH7(fmt, ...) MSG_7(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM7(fmt, ...) MSG_7(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGE8(fmt, ...) MSG_8(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGH8(fmt, ...) MSG_8(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGM8(fmt, ...) MSG_8(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)


// String logging
#define LOC_LOGSE1(fmt, ...) MSG_SPRINTF_1(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH1(fmt, ...) MSG_SPRINTF_1(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM1(fmt, ...) MSG_SPRINTF_1(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE2(fmt, ...) MSG_SPRINTF_2(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH2(fmt, ...) MSG_SPRINTF_2(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM2(fmt, ...) MSG_SPRINTF_2(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE3(fmt, ...) MSG_SPRINTF_3(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH3(fmt, ...) MSG_SPRINTF_3(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM3(fmt, ...) MSG_SPRINTF_3(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE4(fmt, ...) MSG_SPRINTF_4(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH4(fmt, ...) MSG_SPRINTF_4(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM4(fmt, ...) MSG_SPRINTF_4(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE5(fmt, ...) MSG_SPRINTF_5(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH5(fmt, ...) MSG_SPRINTF_5(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM5(fmt, ...) MSG_SPRINTF_5(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE6(fmt, ...) MSG_SPRINTF_6(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH6(fmt, ...) MSG_SPRINTF_6(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM6(fmt, ...) MSG_SPRINTF_6(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE7(fmt, ...) MSG_SPRINTF_7(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH7(fmt, ...) MSG_SPRINTF_7(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM7(fmt, ...) MSG_SPRINTF_7(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSE8(fmt, ...) MSG_SPRINTF_8(MSG_SSID_GPS, MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
#define LOC_LOGSH8(fmt, ...) MSG_SPRINTF_8(MSG_SSID_GPS, MSG_LEGACY_HIGH, fmt, ##__VA_ARGS__)
#define LOC_LOGSM8(fmt, ...) MSG_SPRINTF_8(MSG_SSID_GPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)


// Low level logging, we can keep with a different SSID so that
//   we can selectively disable at runtime
#define LOC_LOG_LOW_LEVEL_ENABLED

#ifdef LOC_LOG_LOW_LEVEL_ENABLED

#define LOC_LOGL0(fmt, ...) MSG(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL1(fmt, ...) MSG_1(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL2(fmt, ...) MSG_2(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL3(fmt, ...) MSG_3(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL4(fmt, ...) MSG_4(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL5(fmt, ...) MSG_5(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL6(fmt, ...) MSG_6(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL7(fmt, ...) MSG_7(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGL8(fmt, ...) MSG_8(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#define LOC_LOGSL1(fmt, ...) MSG_SPRINTF_1(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL2(fmt, ...) MSG_SPRINTF_2(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL3(fmt, ...) MSG_SPRINTF_3(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL4(fmt, ...) MSG_SPRINTF_4(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL5(fmt, ...) MSG_SPRINTF_5(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL6(fmt, ...) MSG_SPRINTF_6(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL7(fmt, ...) MSG_SPRINTF_7(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
#define LOC_LOGSL8(fmt, ...) MSG_SPRINTF_8(MSG_SSID_MMGPS, MSG_LEGACY_MED, fmt, ##__VA_ARGS__)

#else // LOC_LOG_LOW_LEVEL_ENABLED

#define LOC_LOGL0(fmt, ...)
#define LOC_LOGL1(fmt, ...)
#define LOC_LOGL2(fmt, ...)
#define LOC_LOGL3(fmt, ...)
#define LOC_LOGL4(fmt, ...)
#define LOC_LOGL5(fmt, ...)
#define LOC_LOGL6(fmt, ...)
#define LOC_LOGL7(fmt, ...)
#define LOC_LOGL8(fmt, ...)

#define LOC_LOGSL1(fmt, ...)
#define LOC_LOGSL2(fmt, ...)
#define LOC_LOGSL3(fmt, ...)
#define LOC_LOGSL4(fmt, ...)
#define LOC_LOGSL5(fmt, ...)
#define LOC_LOGSL6(fmt, ...)
#define LOC_LOGSL7(fmt, ...)
#define LOC_LOGSL8(fmt, ...)

#endif // LOC_LOG_LOW_LEVEL_ENABLED


// EVENT LOGGING : not actual QXDM events,
// Just messages with a separate SSID, so that they can be filtered
// separately.
// Can be used by test team to see validation results
#define LOC_LOGEVENT(fmt) MSG(MSG_SSID_PE, MSG_LEGACY_HIGH, fmt)
#define LOC_LOGEVENT_ERR(fmt) MSG(MSG_SSID_PE, MSG_LEGACY_ERROR, fmt)


#define MODEM_LOG_CALLFLOW(SPEC, VAL)
#define EXIT_LOG_CALLFLOW(SPEC, VAL)

#define loc_get_v02_event_name(X) #X
#define loc_get_v02_client_status_name(X) #X

#define loc_get_v02_qmi_status_name(X)  #X

//specific to OFF TARGET
#ifdef LOC_UTIL_TARGET_OFF_TARGET

#include <stdio.h>
# include <asm/errno.h>
# include <sys/time.h>

// get around strl*: not found in glibc
// TBD:look for presence of eglibc other libraries
// with strlcpy supported.
#define strlcpy(X,Y,Z) strcpy(X,Y)
#define strlcat(X,Y,Z) strcat(X,Y)

#endif //LOC_UTIL_TARGET_OFF_TARGET

#endif //_ANDROID_

#endif //LOC_UTIL_LOG_H
