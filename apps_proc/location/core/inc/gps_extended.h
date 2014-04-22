/* Copyright (c) 2013, 2016, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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

#ifndef GPS_EXTENDED_H
#define GPS_EXTENDED_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gps_extended_c.h"

struct LocPosMode
{
    LocPositionMode mode;
    GpsPositionRecurrence recurrence;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    LocPosMode(LocPositionMode m, GpsPositionRecurrence recr,
               uint32_t gap, uint32_t accu) :
        mode(m), recurrence(recr),
        min_interval(gap < MIN_POSSIBLE_FIX_INTERVAL ? MIN_POSSIBLE_FIX_INTERVAL : gap), preferred_accuracy(accu) {
        }


    inline LocPosMode() :
        mode(LOC_POSITION_MODE_MS_BASED),
        recurrence(GPS_POSITION_RECURRENCE_PERIODIC),
        min_interval(MIN_POSSIBLE_FIX_INTERVAL),
        preferred_accuracy(50) {
    }

    inline bool equals(const LocPosMode &anotherMode) const
    {
        return anotherMode.mode == mode &&
            anotherMode.recurrence == recurrence &&
            anotherMode.min_interval == min_interval &&
            anotherMode.preferred_accuracy == preferred_accuracy;
    }

    //void logv() const;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GPS_EXTENDED_H */

