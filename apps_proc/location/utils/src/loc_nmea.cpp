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

#include <loc_nmea.h>
#include <math.h>
#include <log_util.h>
#include <stdio.h>
#include <LocOSIface.h>
#include "loc_misc_utils.h"

#define GLONASS_SV_ID_OFFSET 64
#define MAX_SATELLITES_IN_USE 12
#define MAX_SV_COUNT_SUPPORTED_IN_ONE_CONSTELLATION  64

// GNSS system id according to NMEA spec
#define SYSTEM_ID_GPS          1
#define SYSTEM_ID_GLONASS      2
#define SYSTEM_ID_GALILEO      3
// Extended systems
#define SYSTEM_ID_BEIDOU       4
#define SYSTEM_ID_QZSS         5

//GNSS signal id according to NMEA spec
#define SIGNAL_ID_ALL_SIGNALS  0
#define SIGNAL_ID_GPS_L1CA     1
#define SIGNAL_ID_GPS_L1P      2
#define SIGNAL_ID_GPS_L1M      3
#define SIGNAL_ID_GPS_L2P      4
#define SIGNAL_ID_GPS_L2CM     5
#define SIGNAL_ID_GPS_L2CL     6
#define SIGNAL_ID_GPS_L5I      7
#define SIGNAL_ID_GPS_L5Q      8


#define SIGNAL_ID_GLO_G1CA     1
#define SIGNAL_ID_GLO_G1P      2
#define SIGNAL_ID_GLO_G2CA     3
#define SIGNAL_ID_GLO_G2P      4


#define SIGNAL_ID_GAL_E5A      1
#define SIGNAL_ID_GAL_E5B      2
#define SIGNAL_ID_GAL_E5AB     3
#define SIGNAL_ID_GAL_E6A      4
#define SIGNAL_ID_GAL_E6BC     5
#define SIGNAL_ID_GAL_L1A      6
#define SIGNAL_ID_GAL_L1BC     7

//Extended signal id
#define SIGNAL_ID_BDS_B1I      1
#define SIGNAL_ID_BDS_B1C      2
#define SIGNAL_ID_BDS_B2I      3
#define SIGNAL_ID_BDS_B2AI     4


typedef struct loc_nmea_sv_meta_s
{
    char talker[3];
    LocGnssConstellationType svType;
    uint32_t mask;
    uint32_t svCount;
    uint32_t svIdOffset;
    uint32_t signalId;
    uint32_t systemId;
    uint32_t totalSvUsedCount;
} loc_nmea_sv_meta;

typedef struct loc_sv_cache_info_s
{
    uint32_t gps_used_mask;
    uint32_t glo_used_mask;
    uint32_t gal_used_mask;
    uint32_t qzss_used_mask;
    uint32_t bds_used_mask;
    uint32_t gps_l1_count;
    uint32_t gps_l5_count;
    uint32_t glo_g1_count;
    uint32_t glo_g2_count;
    uint32_t gal_e1_count;
    uint32_t gal_e5_count;
    uint32_t qzss_l1_count;
    uint32_t qzss_l5_count;
    uint32_t bds_b1_count;
    uint32_t bds_b2_count;
    float hdop;
    float pdop;
    float vdop;
} loc_sv_cache_info;

/*===========================================================================
FUNCTION    convert_signalType_to_signalId

DESCRIPTION
   convert signalType to signal ID

DEPENDENCIES
   NONE

RETURN VALUE
   value of signal ID

SIDE EFFECTS
   N/A

===========================================================================*/
static uint32_t convert_signalType_to_signalId(GnssSignalTypeMask signalType)
{
    uint32_t signalId = SIGNAL_ID_ALL_SIGNALS;

    switch (signalType) {
        case GNSS_SIGNAL_GPS_L1CA:
            signalId = SIGNAL_ID_GPS_L1CA;
            break;
        case GNSS_SIGNAL_GPS_L2:
            signalId = SIGNAL_ID_GPS_L2CL;
            break;
        case GNSS_SIGNAL_GPS_L5:
            signalId = SIGNAL_ID_GPS_L5Q;
            break;
        case GNSS_SIGNAL_GLONASS_G1:
            signalId = SIGNAL_ID_GLO_G1CA;
            break;
        case GNSS_SIGNAL_GLONASS_G2:
            signalId = SIGNAL_ID_GLO_G2CA;
            break;
        case GNSS_SIGNAL_GALILEO_E1:
            signalId = SIGNAL_ID_GAL_L1BC;
            break;
        case GNSS_SIGNAL_GALILEO_E5A:
            signalId = SIGNAL_ID_GAL_E5A;
            break;
        case GNSS_SIGNAL_GALILEO_E5B:
            signalId = SIGNAL_ID_GAL_E5B;
            break;
        case GNSS_SIGNAL_QZSS_L1CA:
            signalId = SIGNAL_ID_GPS_L1CA;
            break;
        case GNSS_SIGNAL_QZSS_L2:
            signalId = SIGNAL_ID_GPS_L2CL;
            break;
        case GNSS_SIGNAL_QZSS_L5:
            signalId = SIGNAL_ID_GPS_L5Q;
            break;
        case GNSS_SIGNAL_BEIDOU_B1I:
            signalId = SIGNAL_ID_BDS_B1I;
            break;
        case GNSS_SIGNAL_BEIDOU_B1C:
            signalId = SIGNAL_ID_BDS_B1C;
            break;
        case GNSS_SIGNAL_BEIDOU_B2I:
            signalId = SIGNAL_ID_BDS_B2I;
            break;
        case GNSS_SIGNAL_BEIDOU_B2AI:
            signalId = SIGNAL_ID_BDS_B2AI;
            break;
        default:
            signalId = SIGNAL_ID_ALL_SIGNALS;
    }

    return signalId;

}

/*===========================================================================
FUNCTION    
	get_sv_count_from_mask

DESCRIPTION
   get the sv count from bit mask

DEPENDENCIES
   NONE

RETURN VALUE
   value of sv count

SIDE EFFECTS
   N/A

===========================================================================*/
static uint32_t get_sv_count_from_mask(uint64_t svMask, int totalSvCount)
{
    int index = 0;
    uint32_t svCount = 0;
    
    if(totalSvCount > MAX_SV_COUNT_SUPPORTED_IN_ONE_CONSTELLATION) {
        LOC_LOGE2("total SV count in this constellation %d exceeded limit %d",
                 totalSvCount, MAX_SV_COUNT_SUPPORTED_IN_ONE_CONSTELLATION);
    }
    for(index = 0; index < totalSvCount; index++) {
        if(svMask & 0x1)
            svCount += 1;
        svMask >>= 1;
    }
    LOC_LOGL1("SV count %d ", svCount);
    return svCount;
}

/*===========================================================================
FUNCTION    loc_nmea_sv_meta_init

DESCRIPTION
   Init loc_nmea_sv_meta passed in

DEPENDENCIES
   NONE

RETURN VALUE
   Pointer to loc_nmea_sv_meta

SIDE EFFECTS
   N/A

===========================================================================*/
static loc_nmea_sv_meta* loc_nmea_sv_meta_init(loc_nmea_sv_meta& sv_meta,
                                               loc_sv_cache_info& sv_cache_info,
                                               GnssSvType svType,
                                               GnssSignalTypeMask signalType,
                                               bool needCombine)
{
    memset(&sv_meta, 0, sizeof(sv_meta));
    sv_meta.svType = svType;

    switch (svType)
    {
        case GNSS_SV_TYPE_GPS:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'P';
            sv_meta.mask = sv_cache_info.gps_used_mask;
            sv_meta.svIdOffset = GPS_SV_PRN_MIN - 1;
            sv_meta.systemId = SYSTEM_ID_GPS;
            if (GNSS_SIGNAL_GPS_L1CA == signalType) {
                sv_meta.svCount = sv_cache_info.gps_l1_count;
            } else if (GNSS_SIGNAL_GPS_L5 == signalType) {
                sv_meta.svCount = sv_cache_info.gps_l5_count;
            }
            break;
        case GNSS_SV_TYPE_GLONASS:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'L';
            sv_meta.mask = sv_cache_info.glo_used_mask;
            // GLONASS SV ids are reported from 65-96 and not from 1
            // So required offset is 0
            sv_meta.svIdOffset = 0;
            sv_meta.systemId = SYSTEM_ID_GLONASS;
            if (GNSS_SIGNAL_GLONASS_G1 == signalType) {
                sv_meta.svCount = sv_cache_info.glo_g1_count;
            } else if (GNSS_SIGNAL_GLONASS_G2 == signalType) {
                sv_meta.svCount = sv_cache_info.glo_g2_count;
            }
            break;
        case GNSS_SV_TYPE_GALILEO:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'A';
            sv_meta.mask = sv_cache_info.gal_used_mask;
            sv_meta.svIdOffset = GAL_SV_PRN_MIN - 1;
            sv_meta.systemId = SYSTEM_ID_GALILEO;
            if (GNSS_SIGNAL_GALILEO_E1 == signalType) {
                sv_meta.svCount = sv_cache_info.gal_e1_count;
            } else if (GNSS_SIGNAL_GALILEO_E5A == signalType) {
                sv_meta.svCount = sv_cache_info.gal_e5_count;
            }
            break;
        case GNSS_SV_TYPE_QZSS:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'Q';
            sv_meta.mask = sv_cache_info.qzss_used_mask;
            sv_meta.svIdOffset = QZSS_SV_PRN_MIN - 1;
            sv_meta.systemId = SYSTEM_ID_QZSS;
            if (GNSS_SIGNAL_QZSS_L1CA == signalType) {
                sv_meta.svCount = sv_cache_info.qzss_l1_count;
            } else if (GNSS_SIGNAL_QZSS_L5 == signalType) {
                sv_meta.svCount = sv_cache_info.qzss_l5_count;
            }
            break;
        case GNSS_SV_TYPE_BEIDOU:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'B';
            sv_meta.mask = sv_cache_info.bds_used_mask;
            sv_meta.svIdOffset = BDS_SV_PRN_MIN - 1;
            sv_meta.systemId = SYSTEM_ID_BEIDOU;
            if (GNSS_SIGNAL_BEIDOU_B1I == signalType) {
                sv_meta.svCount = sv_cache_info.bds_b1_count;
            } else if (GNSS_SIGNAL_BEIDOU_B2AI == signalType) {
                sv_meta.svCount = sv_cache_info.bds_b2_count;
            }
            break;
        default:
            LOC_LOGE1("NMEA Error unknow constellation type: %d", svType);
            return NULL;
    }
    sv_meta.signalId = convert_signalType_to_signalId(signalType);

    sv_meta.totalSvUsedCount =
        get_sv_count_from_mask(sv_cache_info.gps_used_mask,
            GPS_SV_PRN_MAX - GPS_SV_PRN_MIN + 1) +
        get_sv_count_from_mask(sv_cache_info.glo_used_mask,
            GLO_SV_PRN_MAX - GLO_SV_PRN_MIN + 1) +
        get_sv_count_from_mask(sv_cache_info.gal_used_mask,
            GAL_SV_PRN_MAX - GAL_SV_PRN_MIN + 1) +
        get_sv_count_from_mask(sv_cache_info.qzss_used_mask,
            QZSS_SV_PRN_MAX - QZSS_SV_PRN_MIN + 1) +
        get_sv_count_from_mask(sv_cache_info.bds_used_mask,
            BDS_SV_PRN_MAX - BDS_SV_PRN_MIN + 1);
    
    if (needCombine &&
                (sv_cache_info.gps_used_mask ? 1 : 0) +
                (sv_cache_info.glo_used_mask ? 1 : 0) +
                (sv_cache_info.gal_used_mask ? 1 : 0) +
                (sv_cache_info.qzss_used_mask ? 1 : 0) +
                (sv_cache_info.bds_used_mask ? 1 : 0) > 1)
    {
        // If GPS, GLONASS, Galileo, QZSS, BDS etc. are combined
        // to obtain the reported position solution,
        // talker shall be set to GN, to indicate that
        // the satellites are used in a combined solution
        sv_meta.talker[0] = 'G';
        sv_meta.talker[1] = 'N';
    }
    
    return &sv_meta;
}

/*===========================================================================
FUNCTION    loc_nmea_put_checksum

DESCRIPTION
   Generate NMEA sentences generated based on position report

DEPENDENCIES
   NONE

RETURN VALUE
   Total length of the nmea sentence

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_nmea_put_checksum(char *pNmea, int maxSize)
{
    uint8_t checksum = 0;
    int length = 0;
    if(NULL == pNmea)
        return 0;

    pNmea++; //skip the $
    while (*pNmea != '\0')
    {
        checksum ^= *pNmea++;
        length++;
    }

    // length now contains nmea sentence string length not including $ sign.
    int checksumLength = snprintf(pNmea,(maxSize-length-1),"*%02X\r\n", checksum);

    // total length of nmea sentence is length of nmea sentence inc $ sign plus
    // length of checksum (+1 is to cover the $ character in the length).
    return (length + checksumLength + 1);
}

/*===========================================================================
FUNCTION    loc_nmea_generate_GSA

DESCRIPTION
   Generate NMEA GSA sentences generated based on position report
   Currently below sentences are generated:
   - $GPGSA : GPS DOP and active SVs
   - $GLGSA : GLONASS DOP and active SVs
   - $GAGSA : GALILEO DOP and active SVs
   - $GNGSA : GNSS DOP and active SVs

DEPENDENCIES
   NONE

RETURN VALUE
   Number of SVs used

SIDE EFFECTS
   N/A

===========================================================================*/
static uint32_t loc_nmea_generate_GSA(const GpsLocationExtended &locationExtended,
                              char* sentence,
                              int bufSize,
                              loc_nmea_sv_meta* sv_meta_p,
                              std::vector<std::string> &nmeaArraystr)
{
    if (!sentence || bufSize <= 0 || !sv_meta_p)
    {
        LOC_LOGE0("NMEA Error invalid arguments.");
        return 0;
    }

    char* pMarker = sentence;
    int lengthRemaining = bufSize;
    int length = 0;

    uint32_t svUsedCount = 0;
    uint32_t svUsedList[32] = {0};

    char fixType = '\0';

    const char* talker = sv_meta_p->talker;
    uint32_t mask = sv_meta_p->mask;

    for (uint8_t i = 1; mask > 0 && svUsedCount < 32; i++)
    {
        if (mask & 1) 
        {
            if(SYSTEM_ID_GLONASS == sv_meta_p->systemId) 
            {
                svUsedList[svUsedCount++] = i + GLONASS_SV_ID_OFFSET;
            }
            else 
            {
                svUsedList[svUsedCount++] = i;
            }
        } 
        mask = mask >> 1;
    }

    if (svUsedCount == 0 && GNSS_SV_TYPE_GPS != sv_meta_p->svType) {
        LOC_LOGE0("SV count zero or SV type is not GPS");
        return 0;
    }

    if (sv_meta_p->totalSvUsedCount == 0)
        fixType = '1'; // no fix
    else if (sv_meta_p->totalSvUsedCount <= 3)
        fixType = '2'; // 2D fix
    else
        fixType = '3'; // 3D fix

    // Start printing the sentence
    // Format: $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,p.p,h.h,v.v,s*cc
    // a : Mode  : A : Automatic, allowed to automatically switch 2D/3D
    // x : Fixtype : 1 (no fix), 2 (2D fix), 3 (3D fix)
    // xx : 12 SV ID
    // p.p : Position DOP (Dilution of Precision)
    // h.h : Horizontal DOP
    // v.v : Vertical DOP
    // s : GNSS System Id
    // cc : Checksum value
    length = snprintf(pMarker, lengthRemaining, "$%sGSA,A,%c,", talker, fixType);

    if (length < 0 || length >= lengthRemaining)
    {
        LOC_LOGE0("NMEA Error in string formatting");
        return 0;
    }
    pMarker += length;
    lengthRemaining -= length;

    // Add first 12 satellite IDs
    for (uint8_t i = 0; i < 12; i++)
    {
        if (i < svUsedCount) {
            length = snprintf(pMarker, lengthRemaining, "%02d,", svUsedList[i]);
        } else {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return 0;
        }
        pMarker += length;
        lengthRemaining -= length;
    }

    // Add the position/horizontal/vertical DOP values
    if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DOP)
    {
//      length = snprintf(pMarker, lengthRemaining, "%.1f,%.1f,%.1f,",
//              locationExtended.pdop,
//              locationExtended.hdop,
//              locationExtended.vdop);
        // %f printed using %d instead
        length = snprintf(pMarker, lengthRemaining, "%d.%d,%d.%d,%d.%d,",
                (int)locationExtended.pdop,
                (abs((int)(locationExtended.pdop * 10))) % 10,
                (int)locationExtended.hdop,
                (abs((int)(locationExtended.hdop * 10))) % 10,
                (int)locationExtended.vdop,
                (abs((int)(locationExtended.vdop * 10))) % 10);
    }
    else
    {   // no dop
        length = snprintf(pMarker, lengthRemaining, ",,,");
    }
    pMarker += length;
    lengthRemaining -= length;

    // system id
    length = snprintf(pMarker, lengthRemaining, "%d", sv_meta_p->systemId);
    pMarker += length;
    lengthRemaining -= length;

    /* Sentence is ready, add checksum and broadcast */
    length = loc_nmea_put_checksum(sentence, bufSize);
    nmeaArraystr.push_back(sentence);

    return svUsedCount;
}

/*===========================================================================
FUNCTION    loc_nmea_generate_GSV

DESCRIPTION
   Generate NMEA GSV sentences generated based on sv report
   Currently below sentences are generated:
   - $GPGSV: GPS Satellites in View
   - $GLGSV: GLONASS Satellites in View
   - $GAGSV: GALILEO Satellites in View

DEPENDENCIES
   NONE

RETURN VALUE
   NONE

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_nmea_generate_GSV(const GnssSvNotification &svNotify,
                              char* sentence,
                              int bufSize,
                              loc_nmea_sv_meta* sv_meta_p,
                              std::vector<std::string> &nmeaArraystr)
{
    if (!sentence || bufSize <= 0)
    {
        LOC_LOGE0("NMEA Error invalid argument.");
        return;
    }

    char* pMarker = sentence;
    int lengthRemaining = bufSize;
    int length = 0;
    int sentenceCount = 0;
    int sentenceNumber = 1;
    size_t svNumber = 1;

    const char* talker = sv_meta_p->talker;
    uint32_t svIdOffset = sv_meta_p->svIdOffset;
    int svCount = sv_meta_p->svCount;
    if (svCount <= 0)
    {
        // no svs in view, so just send a blank $--GSV sentence
        LOC_LOGSM1("SV count less than zero for talker id %s",talker);    
        snprintf(sentence, lengthRemaining, "$%sGSV,1,1,0,%d", talker, sv_meta_p->systemId);
        length = loc_nmea_put_checksum(sentence, bufSize);
        nmeaArraystr.push_back(sentence);
        return;
    }

    svNumber = 1;
    sentenceNumber = 1;
    sentenceCount = svCount / 4 + (svCount % 4 != 0);

    while (sentenceNumber <= sentenceCount)
    {
        pMarker = sentence;
        lengthRemaining = bufSize;

        length = snprintf(pMarker, lengthRemaining, "$%sGSV,%d,%d,%02d",
                talker, sentenceCount, sentenceNumber, svCount);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        for (int i=0; (svNumber <= svNotify.count) && (i < 4);  svNumber++)
        {
            GnssSignalTypeMask signalType = svNotify.gnssSvs[svNumber-1].gnssSignalTypeMask;
            if (0 == signalType) {
                // If no signal type in report, it means default L1,G1,E1,B1I
                switch (svNotify.gnssSvs[svNumber - 1].type)
                {
                    case GNSS_SV_TYPE_GPS:
                        signalType = GNSS_SIGNAL_GPS_L1CA;
                        break;
                    case GNSS_SV_TYPE_GLONASS:
                        signalType = GNSS_SIGNAL_GLONASS_G1;
                        break;
                    case GNSS_SV_TYPE_GALILEO:
                        signalType = GNSS_SIGNAL_GALILEO_E1;
                        break;
                    case GNSS_SV_TYPE_QZSS:
                        signalType = GNSS_SIGNAL_QZSS_L1CA;
                        break;
                    case GNSS_SV_TYPE_BEIDOU:
                        signalType = GNSS_SIGNAL_BEIDOU_B1I;
                        break;
                    default:
                        LOC_LOGE1("NMEA Error unknow constellation type: %d",
                                svNotify.gnssSvs[svNumber - 1].type);
                        continue;
                }
            }

            if (sv_meta_p->svType == svNotify.gnssSvs[svNumber - 1].type &&
                    sv_meta_p->signalId == convert_signalType_to_signalId(signalType))
            {
                length = snprintf(pMarker, lengthRemaining,",%02d",
                        svNotify.gnssSvs[svNumber - 1].svId - svIdOffset); //float to int

                if (length < 0 || length >= lengthRemaining)
                {
                    LOC_LOGE0("NMEA Error in string formatting");
                    return;
                }
                pMarker += length;
                lengthRemaining -= length;

                if(svNotify.gnssSvs[svNumber  -1].svInfoMask & GNSS_SV_INFO_MASK_VALID_ELEVATION) {
                    
                    length = snprintf(pMarker, lengthRemaining,",%02d",
                            (int)(0.5f + svNotify.gnssSvs[svNumber - 1].elevation)); //float to int
                }
                else {
                    length = snprintf(pMarker, lengthRemaining,",");
                }

                if (length < 0 || length >= lengthRemaining)
                {
                    LOC_LOGE0("NMEA Error in string formatting");
                    return;
                }
                pMarker += length;
                lengthRemaining -= length;
                
                if(svNotify.gnssSvs[svNumber  -1].svInfoMask & GNSS_SV_INFO_MASK_VALID_AZIMUTH) {

                    length = snprintf(pMarker, lengthRemaining,",%03d",
                            (int)(0.5f + svNotify.gnssSvs[svNumber - 1].azimuth)); //float to int
                }
                else {
                    length = snprintf(pMarker, lengthRemaining,",");
                }

                if (length < 0 || length >= lengthRemaining)
                {
                    LOC_LOGE0("NMEA Error in string formatting");
                    return;
                }
                pMarker += length;
                lengthRemaining -= length;

                if(svNotify.gnssSvs[svNumber  -1].svInfoMask & GNSS_SV_INFO_MASK_VALID_SNR) {
                    
                    length = snprintf(pMarker, lengthRemaining,",%02d",
                            (int)(0.5f + svNotify.gnssSvs[svNumber - 1].cN0Dbhz)); //float to int   
                }
                else {
                    length = snprintf(pMarker, lengthRemaining,",");
                }
                
                if (length < 0 || length >= lengthRemaining)
                {
                    LOC_LOGE0("NMEA Error in string formatting");
                    return;
                }
                pMarker += length;
                lengthRemaining -= length;

                i++;
            }

        }

        // append systemId
        length = snprintf(pMarker, lengthRemaining,",%d",sv_meta_p->systemId);
        pMarker += length;
        lengthRemaining -= length;

        length = loc_nmea_put_checksum(sentence, bufSize);
        nmeaArraystr.push_back(sentence);
        sentenceNumber++;

    }  //while
}

/*===========================================================================
FUNCTION    loc_nmea_get_fix_quality

DESCRIPTION
	This function obtains the fix quality for GGA sentence, mode indicator
    for RMC and VTG sentence based on nav solution mask and tech mask in
    the postion report.
 
DEPENDENCIES
    NONE

Output parameter
	ggaGpsQuality: gps quality field in GGA sentence
	rmcModeIndicator: mode indicator field in RMC sentence
	vtgModeIndicator: mode indicator field in VTG sentence

SIDE EFFECTS
	N/A

===========================================================================*/
static void loc_nmea_get_fix_quality(const Location & location,
                                      const GpsLocationExtended & locationExtended,
                                      bool custom_gga_fix_quality,
                                      char ggaGpsQuality[3],
                                      char & rmcModeIndicator,
                                      char & vtgModeIndicator,
                                      char gnsModeIndicator[7]) {
 
     ggaGpsQuality[0] = '0'; // 0 means no fix
     rmcModeIndicator = 'N'; // N means no fix
     vtgModeIndicator = 'N'; // N means no fix
     memset(gnsModeIndicator, 'N', 6); // N means no fix
     gnsModeIndicator[6] = '\0';
     do {
         // GGA fix quality is defined in NMEA spec as below:
         // https://www.trimble.com/OEM_ReceiverHelp/V4.44/en/NMEA-0183messages_GGA.html
         // Fix quality: 0 = invalid
         //              1 = GPS fix (SPS)
         //              2 = DGPS fix
         //              3 = PPS fix
         //              4 = Real Time Kinematic
         //              5 = Float RTK
         //              6 = estimated (dead reckoning) (2.3 feature)
         //              7 = Manual input mode
         //              8 = Simulation mode
         if (!(location.flags & LOCATION_HAS_LAT_LONG_BIT)){
             break;
         }
         // NOTE: Order of the check is important
         if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK) {
             if (LOC_NAV_MASK_PPP_CORRECTION & locationExtended.navSolutionMask) {
                 ggaGpsQuality[0] = '2';    // 2 means DGPS fix
                 rmcModeIndicator = 'P'; // P means precise
                 vtgModeIndicator = 'P'; // P means precise
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'P'; // P means precise
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0) 
                     gnsModeIndicator[1] = 'P'; // P means precise
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'P'; // P means precise
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'P'; // P means precise
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'P'; // P means precise
                 break;
             } else if (LOC_NAV_MASK_RTK_FIXED_CORRECTION & locationExtended.navSolutionMask){
                 ggaGpsQuality[0] = '4';    // 4 means RTK Fixed fix
                 rmcModeIndicator = 'R'; // use R (RTK fixed)
                 vtgModeIndicator = 'D'; // use D (differential) as
                                         // no RTK fixed defined for VTG in NMEA 183 spec
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'R'; // R means RTK fixed
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[1] = 'R'; // R means RTK fixed
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'R'; // R means RTK fixed
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'R'; // R means RTK fixed
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'R'; // R means RTK fixed
                 break;
             } else if (LOC_NAV_MASK_RTK_CORRECTION & locationExtended.navSolutionMask){
                 ggaGpsQuality[0] = '5';    // 5 means RTK float fix
                 rmcModeIndicator = 'F'; // F means RTK float fix
                 vtgModeIndicator = 'D'; // use D (differential) as
                                         // no RTK float defined for VTG in NMEA 183 spec
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'F'; // F means RTK float fix
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[1] = 'F'; // F means RTK float fix
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'F'; // F means RTK float fix
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'F'; // F means RTK float fix
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'F'; // F means RTK float fix
                 break;
             } else if (LOC_NAV_MASK_DGNSS_CORRECTION & locationExtended.navSolutionMask){
                 ggaGpsQuality[0] = '2';    // 2 means DGPS fix
                 rmcModeIndicator = 'D'; // D means differential
                 vtgModeIndicator = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[1] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'D'; // D means differential
                 break;
             } else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask){
                 ggaGpsQuality[0] = '2';    // 2 means DGPS fix
                 rmcModeIndicator = 'D'; // D means differential
                 vtgModeIndicator = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[1] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'D'; // D means differential
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'D'; // D means differential
                 break;
             }
         }
         // NOTE: Order of the check is important
         if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK) {
             if (LOC_POS_TECH_MASK_SATELLITE & locationExtended.tech_mask){
                 ggaGpsQuality[0] = '1'; // 1 means GPS
                 rmcModeIndicator = 'A'; // A means autonomous
                 vtgModeIndicator = 'A'; // A means autonomous
                 if (locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[0] = 'A'; // A means autonomous
                 if (locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[1] = 'A'; // A means autonomous
                 if (locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[2] = 'A'; // A means autonomous
                 if (locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[3] = 'A'; // A means autonomous
                 if (locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask ? 1 : 0)
                     gnsModeIndicator[4] = 'A'; // A means autonomous
                 break;
             } else if (LOC_POS_TECH_MASK_SENSORS & locationExtended.tech_mask){
                 ggaGpsQuality[0] = '6'; // 6 means estimated (dead reckoning)
                 rmcModeIndicator = 'E'; // E means estimated (dead reckoning)
                 vtgModeIndicator = 'E'; // E means estimated (dead reckoning)
                 memset(gnsModeIndicator, 'E', 6); // E means estimated (dead reckoning)
                 break;
             }
         }
     } while (0);
 
     do {
         // check for customized nmea enabled or not
         // with customized GGA quality enabled
         // PPP fix w/o sensor: 59, PPP fix w/ sensor: 69
         // DGNSS/SBAS correction fix w/o sensor: 2, w/ sensor: 62
         // RTK fixed fix w/o sensor: 4, w/ sensor: 64
         // RTK float fix w/o sensor: 5, w/ sensor: 65
         // SPE fix w/o sensor: 1, and w/ sensor: 61
         // Sensor dead reckoning fix: 6
         if (true == custom_gga_fix_quality) {
             if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK) {
                 // PPP fix w/o sensor: fix quality will now be 59
                 // PPP fix w sensor: fix quality will now be 69
                 if (LOC_NAV_MASK_PPP_CORRECTION & locationExtended.navSolutionMask) {
                     if ((locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK) &&
                         (LOC_POS_TECH_MASK_SENSORS & locationExtended.tech_mask)) {
                         ggaGpsQuality[0] = '6';
                         ggaGpsQuality[1] = '9';
                     } else {
                         ggaGpsQuality[0] = '5';
                         ggaGpsQuality[1] = '9';
                     }
                     break;
                 }
             }
 
             if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK) {
                 if (LOC_POS_TECH_MASK_SENSORS & locationExtended.tech_mask){
                     char ggaQuality_copy = ggaGpsQuality[0];
                     ggaGpsQuality[0] = '6'; // 6 sensor assisted
                     // RTK fixed fix w/ sensor: fix quality will now be 64
                     // RTK float fix w/ sensor: 65
                     // DGNSS and/or SBAS correction fix and w/ sensor: 62
                     // GPS fix without correction and w/ sensor: 61
                     if ((LOC_NAV_MASK_RTK_FIXED_CORRECTION & locationExtended.navSolutionMask)||
                             (LOC_NAV_MASK_RTK_CORRECTION & locationExtended.navSolutionMask)||
                             (LOC_NAV_MASK_DGNSS_CORRECTION & locationExtended.navSolutionMask)||
                             (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)||
                             (LOC_POS_TECH_MASK_SATELLITE & locationExtended.tech_mask)) {
                         ggaGpsQuality[1] = ggaQuality_copy;
                         break;
                     }
                 }
             }
         }
     } while (0);
 
     LOC_LOGH3("gps quality: %s, rmc mode indicator: %c, vtg mode indicator: %c",
              ggaGpsQuality, rmcModeIndicator, vtgModeIndicator);
 }


/*===========================================================================
FUNCTION    loc_nmea_generate_pos

DESCRIPTION
   Generate NMEA sentences generated based on position report
   Currently below sentences are generated within this function:
   - $GPGSA : GPS DOP and active SVs
   - $GLGSA : GLONASS DOP and active SVs
   - $GAGSA : GALILEO DOP and active SVs
   - $GNGSA : GNSS DOP and active SVs
   - $--VTG : Track made good and ground speed
   - $--RMC : Recommended minimum navigation information
   - $--GGA : Time, position and fix related data

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_nmea_generate_pos(const Location &location,
                           const GpsLocationExtended &locationExtended,
                           unsigned char generate_nmea,
                           std::vector<std::string> &nmeaArraystr,
                           uint32_t disableNmeaTimeCheck, uint32 constellationMask)
{
    LOC_LOGH1("loc_nmea_generate_pos %d", constellationMask);

    char sentence[NMEA_SENTENCE_MAX_LENGTH] = {0};
    char* pMarker = sentence;
    int lengthRemaining = sizeof(sentence);
    int length = 0;
    LocOSIface_Julian_Time_t julTime = {};
    int utcYear = 0;
    int utcMonth = 0;
    int utcDay = 0;
    int utcHours = 0;
    int utcMinutes = 0;
    int utcSeconds = 0;
    int utcMSeconds = 0;
    
    if(0 != location.timestamp) {

        LOC_LOGH1("Populating NMEA with GNSS time ", location.timestamp);
        convert_UTC_seconds_to_julian(&julTime,(location.timestamp)/1000);
        utcYear = julTime.year % 100; // 2 digit year
        utcMonth = julTime.month;
        utcDay = julTime.day;
        utcHours = julTime.hour;
        utcMinutes = julTime.minute;
        utcSeconds = julTime.second;
        utcMSeconds = (location.timestamp)%1000;
    }
    else {
        
        LOC_LOGE0("UTC time not available in position report, Keeping time field empty");
    }

    LOC_LOGE6("ConstellationMask %d SV id Mask GPS %d GLO %d GAL %d BDS %d QZSS %d ",
                        constellationMask,
                        locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask,
                        locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask,
                        locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask,
                        locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask,
                        locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask);

    loc_sv_cache_info sv_cache_info = {};

    if (GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA & locationExtended.flags) {
        sv_cache_info.gps_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask;
        sv_cache_info.glo_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask;
        sv_cache_info.gal_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask;
        sv_cache_info.qzss_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask;
        sv_cache_info.bds_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask;
    }
    if (generate_nmea) {
        char talker[3] = {'G', 'P', '\0'};
        uint32_t svUsedCount = 0;
        uint32_t gpsSvUsedCount = 0;
        uint32_t count = 0;
        loc_nmea_sv_meta sv_meta;

        if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GPS) {

            // -------------------
            // ---$GPGSA/$GNGSA---
            // -------------------
            count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GPS,
                            GNSS_SIGNAL_GPS_L1CA, true), nmeaArraystr);
            if (count > 0)
            {
                svUsedCount += count;
                gpsSvUsedCount += count;
                talker[0] = sv_meta.talker[0];
                talker[1] = sv_meta.talker[1];
            }
        }


        if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GLONASS) {

            // -------------------
            // ---$GLGSA/$GNGSA---
            // -------------------
            count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GLONASS,
                            GNSS_SIGNAL_GLONASS_G1, true), nmeaArraystr);
            if (count > 0)
            {
                svUsedCount += count;
                talker[0] = sv_meta.talker[0];
                talker[1] = sv_meta.talker[1];
            }
        }
        

        if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GALILEO) {

            // -------------------
            // ---$GAGSA/$GNGSA---
            // -------------------
            count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GALILEO,
                            GNSS_SIGNAL_GALILEO_E1, true), nmeaArraystr);
            if (count > 0)
            {
                svUsedCount += count;
                talker[0] = sv_meta.talker[0];
                talker[1] = sv_meta.talker[1];
            }
        }
        

        if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_QZSS) {

            // --------------------------
            // ---$GQGSA/$GNGSA (QZSS)---
            // --------------------------
            count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_QZSS,
                            GNSS_SIGNAL_QZSS_L1CA, true), nmeaArraystr);
            if (count > 0)
            {
                svUsedCount += count;
                // talker should be default "GP". If GPS, GLO etc is used, it should be "GN"
            }
        }

        if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_BEIDOU) {
            
            // ----------------------------
            // ---$GBGSA/$GNGSA (BEIDOU)---
            // ----------------------------
            count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_BEIDOU,
                            GNSS_SIGNAL_BEIDOU_B1I, true), nmeaArraystr);
            if (count > 0)
            {
                svUsedCount += count;
                // talker should be default "GP". If GPS, GLO etc is used, it should be "GN"
            }
        }

        bool custom_gga_fix_quality = false;
        char ggaGpsQuality[3] = {'0', '\0', '\0'};
        char rmcModeIndicator = 'N';
        char vtgModeIndicator = 'N';
        char gnsModeIndicator[7] = {'N', 'N', 'N', 'N', 'N', 'N', '\0'};
        
        loc_nmea_get_fix_quality(location, locationExtended, custom_gga_fix_quality,
                                 ggaGpsQuality, rmcModeIndicator, vtgModeIndicator, gnsModeIndicator);

        /* For GGA, VTG and RMC talker id doesnt matter, As its generated from all the constillations combined, 
           Here we pick any of the active constellation whose SV count is not zero*/

        // -------------------
        // ------$--VTG-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        if (location.flags & LOCATION_HAS_BEARING_BIT)
        {
            float magTrack = location.bearing;
            if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_MAG_DEV)
            {
                float magTrack = location.bearing - locationExtended.magneticDeviation;
                if (magTrack < 0.0f)
                    magTrack += 360.0f;
                else if (magTrack > 360.0f)
                    magTrack -= 360.0f;
            }

            // length = snprintf(pMarker, lengthRemaining, "$%sVTG,%.1lf,T,%.1lf,M,", talker, location.bearing, magTrack);

            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining,
                              "$%sVTG,%d.%d,T,%d.%d,M,", talker,
                              (int)location.bearing,
                              (abs((int)(location.bearing * 10))) % 10,
                              (int)magTrack,
                              (abs((int)(magTrack * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, "$%sVTG,,T,,M,", talker);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.flags & LOCATION_HAS_SPEED_BIT)
        {
            float speedKnots = location.speed * (3600.0/1852.0);
            float speedKmPerHour = location.speed * 3.6f;

            // length = snprintf(pMarker, lengthRemaining, "%.1lf,N,%.1lf,K,", speedKnots, speedKmPerHour);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining,
                              "%d.%d,N,%d.%d,K,",
                              (int)speedKnots,
                              (abs((int)(speedKnots * 10))) % 10,
                              (int)speedKmPerHour,
                              (abs((int)(speedKmPerHour * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",N,,K,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (!(location.flags & LOCATION_HAS_LAT_LONG_BIT))
            // N means no fix
            length = snprintf(pMarker, lengthRemaining, "%c", 'N');
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            // D means differential
            length = snprintf(pMarker, lengthRemaining, "%c", 'D');
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            // E means estimated (dead reckoning)
            length = snprintf(pMarker, lengthRemaining, "%c", 'E');
        else // A means autonomous
            length = snprintf(pMarker, lengthRemaining, "%c", 'A');

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        // -------------------
        // ------$--RMC-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        if(0 != location.timestamp) {
        length = snprintf(pMarker, lengthRemaining, "$%sRMC,%02d%02d%02d.%02d,A," ,
                          talker, utcHours, utcMinutes, utcSeconds, utcMSeconds/10);
        }
        else {
            length = snprintf(pMarker, lengthRemaining, "$%sRMC,,A,", talker);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.flags & LOCATION_HAS_LAT_LONG_BIT)
        {
            double latitude = location.latitude;
            double longitude = location.longitude;
            char latHemisphere;
            char lonHemisphere;
            double latMinutes;
            double lonMinutes;

            if (latitude > 0)
            {
                latHemisphere = 'N';
            }
            else
            {
                latHemisphere = 'S';
                latitude *= -1.0;
            }

            if (longitude < 0)
            {
                lonHemisphere = 'W';
                longitude *= -1.0;
            }
            else
            {
                lonHemisphere = 'E';
            }

            latMinutes = fmod(latitude * 60.0 , 60.0);
            lonMinutes = fmod(longitude * 60.0 , 60.0);

//          length = snprintf(pMarker, lengthRemaining, "%02d%09.6lf,%c,%03d%09.6lf,%c,",
//                            (uint8_t)floor(latitude), latMinutes, latHemisphere,
//                            (uint8_t)floor(longitude),lonMinutes, lonHemisphere);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%02d%02d.%05d,%c,%03d%02d.%05d,%c,",
                              (uint8_t)floor(latitude),
                              (int)latMinutes,
                              (abs((int)(latMinutes * 100000))) % 100000,
                              latHemisphere,
                              (uint8_t)floor(longitude),
                              (int)lonMinutes,
                              (abs((int)(lonMinutes * 100000))) % 100000,
                              lonHemisphere);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.flags & LOCATION_HAS_SPEED_BIT)
        {
            float speedKnots = location.speed * (3600.0/1852.0);
            //length = snprintf(pMarker, lengthRemaining, "%.1lf,", speedKnots);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%d.%d,",
                              (int)speedKnots,
                              (abs((int)(speedKnots * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.flags & LOCATION_HAS_BEARING_BIT)
        {
            // length = snprintf(pMarker, lengthRemaining, "%.1lf,", location.bearing);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%d.%d,",
                              (int)location.bearing,
                              (abs((int)(location.bearing * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if(0 != location.timestamp) {
        length = snprintf(pMarker, lengthRemaining, "%2.2d%2.2d%2.2d,",
                          utcDay, utcMonth, utcYear);
        }
        else {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_MAG_DEV)
        {
            float magneticVariation = locationExtended.magneticDeviation;
            char direction;
            if (magneticVariation < 0.0f)
            {
                direction = 'W';
                magneticVariation *= -1.0;
            }
            else
            {
                direction = 'E';
            }

//          length = snprintf(pMarker, lengthRemaining, "%.1lf,%c,",
//                            magneticVariation, direction);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%d.%d,%c,",
                              (int)magneticVariation,
                              (abs((int)(magneticVariation * 10))) % 10,
                              direction);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (!(location.flags & LOCATION_HAS_LAT_LONG_BIT))
            // N means no fix
            length = snprintf(pMarker, lengthRemaining, "%c", 'N');
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            // D means differential
            length = snprintf(pMarker, lengthRemaining, "%c", 'D');
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            // E means estimated (dead reckoning)
            length = snprintf(pMarker, lengthRemaining, "%c", 'E');
        else  // A means autonomous
            length = snprintf(pMarker, lengthRemaining, "%c", 'A');

        pMarker += length;
        lengthRemaining -= length;

        // hardcode Navigation Status field to 'V'
        length = snprintf(pMarker, lengthRemaining, ",%c", 'V');
        pMarker += length;
        lengthRemaining -= length;

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        // -------------------
        // ------$--GGA-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        if(0 != location.timestamp) {
        length = snprintf(pMarker, lengthRemaining, "$GPGGA,%02d%02d%02d.%02d," ,
                          utcHours, utcMinutes, utcSeconds, utcMSeconds/10);
        }
        else {
            length = snprintf(pMarker, lengthRemaining, "$GPGGA,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.flags & LOCATION_HAS_LAT_LONG_BIT)
        {
            double latitude = location.latitude;
            double longitude = location.longitude;
            char latHemisphere;
            char lonHemisphere;
            double latMinutes;
            double lonMinutes;

            if (latitude > 0)
            {
                latHemisphere = 'N';
            }
            else
            {
                latHemisphere = 'S';
                latitude *= -1.0;
            }

            if (longitude < 0)
            {
                lonHemisphere = 'W';
                longitude *= -1.0;
            }
            else
            {
                lonHemisphere = 'E';
            }

            latMinutes = fmod(latitude * 60.0 , 60.0);
            lonMinutes = fmod(longitude * 60.0 , 60.0);

//          length = snprintf(pMarker, lengthRemaining, "%02d%09.6lf,%c,%03d%09.6lf,%c,",
//                            (uint8_t)floor(latitude), latMinutes, latHemisphere,
//                            (uint8_t)floor(longitude),lonMinutes, lonHemisphere);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%02d%02d.%05d,%c,%03d%02d.%05d,%c,",
                              (uint8_t)floor(latitude),
                              (int)latMinutes,
                              (abs((int)(latMinutes * 100000))) % 100000,
                              latHemisphere,
                              (uint8_t)floor(longitude),
                              (int)lonMinutes,
                              (abs((int)(lonMinutes * 100000))) % 100000,
                              lonHemisphere);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        char gpsQuality;
        if (!(location.flags & LOCATION_HAS_LAT_LONG_BIT))
            gpsQuality = '0'; // 0 means no fix
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            gpsQuality = '2'; // 2 means DGPS fix
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            gpsQuality = '6'; // 6 means estimated (dead reckoning)
        else
            gpsQuality = '1'; // 1 means GPS fix

        // Number of satellites in use, 00-12
        if (gpsSvUsedCount > MAX_SATELLITES_IN_USE)
            gpsSvUsedCount = MAX_SATELLITES_IN_USE;
        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DOP)
        {
//          length = snprintf(pMarker, lengthRemaining, "%c,%02d,%.1f,",
//                            gpsQuality, svUsedCount, locationExtended.hdop);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%c,%02d,%d.%d,",
                              gpsQuality, gpsSvUsedCount,
                              (int)locationExtended.hdop,
                              (abs((int)(locationExtended.hdop * 10))) % 10);
        }
        else
        {   // no hdop
            length = snprintf(pMarker, lengthRemaining, "%c,%02d,,",
                              gpsQuality, gpsSvUsedCount);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL)
        {
//          length = snprintf(pMarker, lengthRemaining, "%.1lf,M,",
//                            locationExtended.altitudeMeanSeaLevel);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%d.%d,M,",
                              (int)locationExtended.altitudeMeanSeaLevel,
                              (abs((int)(locationExtended.altitudeMeanSeaLevel * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if ((location.flags & LOCATION_HAS_ALTITUDE_BIT) &&
            (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL))
        {
//          length = snprintf(pMarker, lengthRemaining, "%.1lf,M,,",
//                            location.altitude - locationExtended.altitudeMeanSeaLevel);
            // %f printed using %d instead
            length = snprintf(pMarker, lengthRemaining, "%d.%d,M,,",
                              (int)(location.altitude - locationExtended.altitudeMeanSeaLevel),
                              (abs((int)((location.altitude - locationExtended.altitudeMeanSeaLevel) * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,");
        }

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        // -------------------
        // ------$--GNS-------
        // -------------------
        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        if(0 != location.timestamp) {
            length = snprintf(pMarker, lengthRemaining, "$GNGNS,%02d%02d%02d.%02d," ,
                          utcHours, utcMinutes, utcSeconds, utcMSeconds/10);
        }
        else {
            length = snprintf(pMarker, lengthRemaining, "$GNGNS,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }

        pMarker += length;
        lengthRemaining -= length;


        if (location.flags & LOCATION_HAS_LAT_LONG_BIT)
        {
            double latitude = location.latitude;
            double longitude = location.longitude;
            char latHemisphere;
            char lonHemisphere;
            double latMinutes;
            double lonMinutes;

            if (latitude > 0)
            {
                latHemisphere = 'N';
            }
            else
            {
                latHemisphere = 'S';
                latitude *= -1.0;
            }

            if (longitude < 0)
            {
                lonHemisphere = 'W';
                longitude *= -1.0;
            }
            else
            {
                lonHemisphere = 'E';
            }

            latMinutes = fmod(latitude * 60.0 , 60.0);
            lonMinutes = fmod(longitude * 60.0 , 60.0);
            
            length = snprintf(pMarker, lengthRemaining, "%02d%02d.%05d,%c,%03d%02d.%05d,%c,",
                              (uint8_t)floor(latitude),
                              (int)latMinutes,
                              (abs((int)(latMinutes * 100000))) % 100000,
                              latHemisphere,
                              (uint8_t)floor(longitude),
                              (int)lonMinutes,
                              (abs((int)(lonMinutes * 100000))) % 100000,
                              lonHemisphere);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        length = snprintf(pMarker, lengthRemaining, "%s,", gnsModeIndicator);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }

        pMarker += length;
        lengthRemaining -= length;
  
        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DOP)
        {
            length = snprintf(pMarker, lengthRemaining, "%02d,%d.%d,",
                              svUsedCount,
                              (int)locationExtended.hdop,
                              (abs((int)(locationExtended.hdop * 10))) % 10);
        }
        else
        {   
            length = snprintf(pMarker, lengthRemaining, "%c,%02d,,",
                              gpsQuality, svUsedCount);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL)
        {
            length = snprintf(pMarker, lengthRemaining, "%d.%d,M,",
                              (int)locationExtended.altitudeMeanSeaLevel,
                              (abs((int)(locationExtended.altitudeMeanSeaLevel * 10))) % 10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if ((location.flags & LOCATION_HAS_ALTITUDE_BIT) &&
            (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL))
        {
            double geoidal_sepration = location.altitude - locationExtended.altitudeMeanSeaLevel;
            length = snprintf(pMarker, lengthRemaining, "%d.%d,", (int)geoidal_sepration, ((int)(geoidal_sepration*10))%10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }
        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;
        
        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE)
        {
            double dgnssDataAge = locationExtended.dgnssDataAgeMsec / 1000;
            length = snprintf(pMarker, lengthRemaining, "%d.%d,", (int)dgnssDataAge, ((int)(dgnssDataAge*10))%10);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }
        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE0("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID)
        {
            length = snprintf(pMarker, lengthRemaining, "%04d",
                              locationExtended.dgnssRefStationId);
            if (length < 0 || length >= lengthRemaining)
            {
                LOC_LOGE0("NMEA Error in string formatting");
                return;
            }
            pMarker += length;
            lengthRemaining -= length;
        }

        // hardcode Navigation Status field to 'V'
        length = snprintf(pMarker, lengthRemaining, ",%c", 'V');
        pMarker += length;
        lengthRemaining -= length;
        
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);
    }
    //Send blank NMEA reports for non-final fixes
    else {
        
        if(constellationMask & LOC_GNSS_CONSTELLATION_MASK_GPS) {
            strlcpy(sentence, "$GPGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
            length = loc_nmea_put_checksum(sentence, sizeof(sentence));
            nmeaArraystr.push_back(sentence);
        }

        if(constellationMask & LOC_GNSS_CONSTELLATION_MASK_GLONASS) {
            strlcpy(sentence, "$GLGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
            length = loc_nmea_put_checksum(sentence, sizeof(sentence));
            nmeaArraystr.push_back(sentence);
        }

        if(constellationMask & LOC_GNSS_CONSTELLATION_MASK_GALILEO) {
            strlcpy(sentence, "$GAGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
            length = loc_nmea_put_checksum(sentence, sizeof(sentence));
            nmeaArraystr.push_back(sentence);
        }

        if(constellationMask & LOC_GNSS_CONSTELLATION_MASK_BEIDOU) {
            strlcpy(sentence, "$GBGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
            length = loc_nmea_put_checksum(sentence, sizeof(sentence));
            nmeaArraystr.push_back(sentence);
        }

        if(constellationMask & LOC_GNSS_CONSTELLATION_MASK_QZSS) {
            strlcpy(sentence, "$GQGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);
        }

        strlcpy(sentence, "$GNGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GNVTG,,T,,M,,N,,K,N", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GNRMC,,V,,,,,,,,,,N,V", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GPGGA,,,,,,0,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);
    }
}



/*===========================================================================
FUNCTION    loc_nmea_generate_sv

DESCRIPTION
   Generate NMEA sentences generated based on sv report

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_nmea_generate_sv(const GnssSvNotification &svNotify,
                              std::vector<std::string> &nmeaArraystr, uint32 constellationMask)
{
    char sentence[NMEA_SENTENCE_MAX_LENGTH] = {0};
    int svCount = svNotify.count;
    int svNumber = 1;
    loc_sv_cache_info sv_cache_info = {};

    LOC_LOGL0("loc_nmea_generate_sv");

    //Count GPS SVs for saparating GPS from GLONASS and throw others
    for(svNumber=1; svNumber <= svCount; svNumber++) {
        if (GNSS_SV_TYPE_GPS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GPGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.gps_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            
                // GNSS_SIGNAL_GPS_L1CA or default
                // If no signal type in report, it means default L1
                sv_cache_info.gps_l1_count++;
            }
        else if (GNSS_SV_TYPE_GLONASS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GNGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.glo_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            
                // GNSS_SIGNAL_GLONASS_G1 or default
                // If no signal type in report, it means default G1
                sv_cache_info.glo_g1_count++;
            }
        else if (GNSS_SV_TYPE_GALILEO == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GAGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.gal_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            
                // GNSS_SIGNAL_GALILEO_E1 or default
                // If no signal type in report, it means default E1
                sv_cache_info.gal_e1_count++;
            }
        else if (GNSS_SV_TYPE_QZSS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $PQGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                  GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.qzss_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            
                // GNSS_SIGNAL_QZSS_L1CA or default
                // If no signal type in report, it means default L1
                sv_cache_info.qzss_l1_count++;
            }
        else if (GNSS_SV_TYPE_BEIDOU == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $PQGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                  GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.bds_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            
                // GNSS_SIGNAL_BEIDOU_B1I or default
                // If no signal type in report, it means default B1I
                sv_cache_info.bds_b1_count++;
            }
        }

    loc_nmea_sv_meta sv_meta;

    if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GPS) {

        // ---------------------
        // ------$GPGSV:L1CA----
        // ---------------------
        loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
                loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GPS,
                GNSS_SIGNAL_GPS_L1CA, false), nmeaArraystr);
    }

    if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GLONASS) {

        // ---------------------
        // ------$GLGSV:G1------
        // ---------------------
        loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
                loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GLONASS,
                GNSS_SIGNAL_GLONASS_G1, false), nmeaArraystr);
    }

    if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_GALILEO) {
        
        // ---------------------
        // ------$GAGSV:E1------
        // ---------------------
        loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
                loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GALILEO,
                GNSS_SIGNAL_GALILEO_E1, false), nmeaArraystr);
    }

    if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_QZSS) {

        // -----------------------------
        // ------$GQGSV (QZSS):L1CA-----
        // -----------------------------
        loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
                loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_QZSS,
                GNSS_SIGNAL_QZSS_L1CA, false), nmeaArraystr);
    }

    if(constellationMask&LOC_GNSS_CONSTELLATION_MASK_BEIDOU) {

        // -----------------------------
        // ------$GBGSV (BEIDOU:B1I)----
        // -----------------------------
        loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
                loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_BEIDOU,
                GNSS_SIGNAL_BEIDOU_B1I,false), nmeaArraystr);
    }
}
