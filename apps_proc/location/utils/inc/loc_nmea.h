/* Copyright (c) 2012-2013, 2015-2018 The Linux Foundation. All rights reserved.
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
 *
 */

#ifndef LOC_ENG_NMEA_H
#define LOC_ENG_NMEA_H

#include <gps_extended.h>
#include <vector>
#include <string>
#define NMEA_SENTENCE_MAX_LENGTH 200

void loc_nmea_generate_sv(const GnssSvNotification &svNotify,
                          std::vector<std::string> &nmeaArraystr, uint32 constellationMask);

void loc_nmea_generate_pos(const Location &location,
                           const GpsLocationExtended &locationExtended,
                           unsigned char generate_nmea,
                           std::vector<std::string> &nmeaArraystr,
                           uint32_t disableNmeaTimeCheck, uint32 constellationMask);

#define DEBUG_NMEA_MINSIZE 6
#define DEBUG_NMEA_MAXSIZE 4096
inline bool loc_nmea_is_debug(const char* nmea, int length) {
    return ((NULL != nmea) &&
            (length >= DEBUG_NMEA_MINSIZE) && (length <= DEBUG_NMEA_MAXSIZE) &&
            (nmea[0] == '$') && (nmea[1] == 'P') && (nmea[2] == 'Q') && (nmea[3] == 'W'));
}

#endif // LOC_ENG_NMEA_H
