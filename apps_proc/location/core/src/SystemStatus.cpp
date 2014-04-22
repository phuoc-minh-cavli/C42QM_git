/* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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
#define LOG_TAG "LocSvc_SystemStatus"

#include <inttypes.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <platform_lib_log_util.h>
#include <MsgTask.h>
#include <loc_nmea.h>
#include <SystemStatus.h>

namespace izat_core
{
 
/******************************************************************************
 SystemStatusNmeaBase - base class for all NMEA parsers
******************************************************************************/
class SystemStatusNmeaBase
{
protected:
    std::vector<std::string> mField;

    SystemStatusNmeaBase(const char *str_in, uint32_t len_in)
    {
        // check size and talker
        if (!loc_nmea_is_debug(str_in, len_in)) {
            return;
        }

        std::string parser(str_in);
        std::string::size_type index = 0;

        // verify checksum field
        index = parser.find("*");
        if (index == std::string::npos) {
            return;
        }
        parser[index] = ',';

        // tokenize parser
        while (1) {
            std::string str;
            index = parser.find(",");
            if (index == std::string::npos) {
                break;
            }
            str = parser.substr(0, index);
            parser = parser.substr(index + 1);
            mField.push_back(str);
        }
    }

    virtual ~SystemStatusNmeaBase() { }

public:
    static const uint32_t NMEA_MINSIZE = DEBUG_NMEA_MINSIZE;
    static const uint32_t NMEA_MAXSIZE = DEBUG_NMEA_MAXSIZE;
};

/******************************************************************************
 SystemStatusPQWM1
******************************************************************************/
class SystemStatusPQWM1
{
public:
    uint16_t mGpsWeek;    // x1
    uint32_t mGpsTowMs;   // x2
    uint8_t  mTimeValid;  // x3
    uint8_t  mTimeSource; // x4
    int32_t  mTimeUnc;    // x5
    int32_t  mClockFreqBias; // x6
    int32_t  mClockFreqBiasUnc; // x7
    uint8_t  mXoState;    // x8
    int32_t  mPgaGain;    // x9
    uint32_t mGpsBpAmpI;  // xA
    uint32_t mGpsBpAmpQ;  // xB
    uint32_t mAdcI;       // xC
    uint32_t mAdcQ;       // xD
    uint32_t mJammerGps;  // xE
    uint32_t mJammerGlo;  // xF
    uint32_t mJammerBds;  // x10
    uint32_t mJammerGal;  // x11
    uint32_t mRecErrorRecovery; // x12
    double   mAgcGps;     // x13
    double   mAgcGlo;     // x14
    double   mAgcBds;     // x15
    double   mAgcGal;     // x16
    int32_t  mLeapSeconds;// x17
    int32_t  mLeapSecUnc; // x18
    uint32_t mGloBpAmpI;  // x19
    uint32_t mGloBpAmpQ;  // x1A
    uint32_t mBdsBpAmpI;  // x1B
    uint32_t mBdsBpAmpQ;  // x1C
    uint32_t mGalBpAmpI;  // x1D
    uint32_t mGalBpAmpQ;  // x1E
};

// parser
class SystemStatusPQWM1parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eGpsWeek = 1,
        eGpsTowMs = 2,
        eTimeValid = 3,
        eTimeSource = 4,
        eTimeUnc = 5,
        eClockFreqBias = 6,
        eClockFreqBiasUnc = 7,
        eXoState = 8,
        ePgaGain = 9,
        eGpsBpAmpI = 10,
        eGpsBpAmpQ = 11,
        eAdcI = 12,
        eAdcQ = 13,
        eJammerGps = 14,
        eJammerGlo = 15,
        eJammerBds = 16,
        eJammerGal = 17,
        eRecErrorRecovery = 18,
        eAgcGps = 19,
        eAgcGlo = 20,
        eAgcBds = 21,
        eAgcGal = 22,
        eLeapSeconds = 23,
        eLeapSecUnc = 24,
        eGloBpAmpI = 25,
        eGloBpAmpQ = 26,
        eBdsBpAmpI = 27,
        eBdsBpAmpQ = 28,
        eGalBpAmpI = 29,
        eGalBpAmpQ = 30,
        eMax
    };
    SystemStatusPQWM1 mM1;

public:
    inline uint16_t   getGpsWeek()    { return mM1.mGpsWeek; }
    inline uint32_t   getGpsTowMs()   { return mM1.mGpsTowMs; }
    inline uint8_t    getTimeValid()  { return mM1.mTimeValid; }
    inline uint8_t    getTimeSource() { return mM1.mTimeSource; }
    inline int32_t    getTimeUnc()    { return mM1.mTimeUnc; }
    inline int32_t    getClockFreqBias() { return mM1.mClockFreqBias; }
    inline int32_t    getClockFreqBiasUnc() { return mM1.mClockFreqBiasUnc; }
    inline uint8_t    getXoState()    { return mM1.mXoState;}
    inline int32_t    getPgaGain()    { return mM1.mPgaGain;          }
    inline uint32_t   getGpsBpAmpI()  { return mM1.mGpsBpAmpI;        }
    inline uint32_t   getGpsBpAmpQ()  { return mM1.mGpsBpAmpQ;        }
    inline uint32_t   getAdcI()       { return mM1.mAdcI;             }
    inline uint32_t   getAdcQ()       { return mM1.mAdcQ;             }
    inline uint32_t   getJammerGps()  { return mM1.mJammerGps;        }
    inline uint32_t   getJammerGlo()  { return mM1.mJammerGlo;        }
    inline uint32_t   getJammerBds()  { return mM1.mJammerBds;        }
    inline uint32_t   getJammerGal()  { return mM1.mJammerGal;        }
    inline uint32_t   getAgcGps()     { return mM1.mAgcGps;           }
    inline uint32_t   getAgcGlo()     { return mM1.mAgcGlo;           }
    inline uint32_t   getAgcBds()     { return mM1.mAgcBds;           }
    inline uint32_t   getAgcGal()     { return mM1.mAgcGal;           }
    inline uint32_t   getRecErrorRecovery() { return mM1.mRecErrorRecovery; }
    inline int32_t    getLeapSeconds(){ return mM1.mLeapSeconds; }
    inline int32_t    getLeapSecUnc() { return mM1.mLeapSecUnc; }
    inline uint32_t   getGloBpAmpI()  { return mM1.mGloBpAmpI; }
    inline uint32_t   getGloBpAmpQ()  { return mM1.mGloBpAmpQ; }
    inline uint32_t   getBdsBpAmpI()  { return mM1.mBdsBpAmpI; }
    inline uint32_t   getBdsBpAmpQ()  { return mM1.mBdsBpAmpQ; }
    inline uint32_t   getGalBpAmpI()  { return mM1.mGalBpAmpI; }
    inline uint32_t   getGalBpAmpQ()  { return mM1.mGalBpAmpQ; }

    SystemStatusPQWM1parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        memset(&mM1, 0, sizeof(mM1));
        if (mField.size() < eMax) {
            LOC_LOGE1("PQWM1parser - invalid size=%zu", mField.size());
            mM1.mTimeValid = 0;
            return;
        }
        mM1.mGpsWeek = atoi(mField[eGpsWeek].c_str());
        mM1.mGpsTowMs = atoi(mField[eGpsTowMs].c_str());
        mM1.mTimeValid = atoi(mField[eTimeValid].c_str());
        mM1.mTimeSource = atoi(mField[eTimeSource].c_str());
        mM1.mTimeUnc = atoi(mField[eTimeUnc].c_str());
        mM1.mClockFreqBias = atoi(mField[eClockFreqBias].c_str());
        mM1.mClockFreqBiasUnc = atoi(mField[eClockFreqBiasUnc].c_str());
        mM1.mXoState = atoi(mField[eXoState].c_str());
        mM1.mPgaGain = atoi(mField[ePgaGain].c_str());
        mM1.mGpsBpAmpI = atoi(mField[eGpsBpAmpI].c_str());
        mM1.mGpsBpAmpQ = atoi(mField[eGpsBpAmpQ].c_str());
        mM1.mAdcI = atoi(mField[eAdcI].c_str());
        mM1.mAdcQ = atoi(mField[eAdcQ].c_str());
        mM1.mJammerGps = atoi(mField[eJammerGps].c_str());
        mM1.mJammerGlo = atoi(mField[eJammerGlo].c_str());
        mM1.mJammerBds = atoi(mField[eJammerBds].c_str());
        mM1.mJammerGal = atoi(mField[eJammerGal].c_str());
        mM1.mRecErrorRecovery = atoi(mField[eRecErrorRecovery].c_str());
        mM1.mLeapSeconds = atoi(mField[eLeapSeconds].c_str());
        mM1.mLeapSecUnc = atoi(mField[eLeapSecUnc].c_str());
        mM1.mGloBpAmpI = atoi(mField[eGloBpAmpI].c_str());
        mM1.mGloBpAmpQ = atoi(mField[eGloBpAmpQ].c_str());
        mM1.mBdsBpAmpI = atoi(mField[eBdsBpAmpI].c_str());
        mM1.mBdsBpAmpQ = atoi(mField[eBdsBpAmpQ].c_str());
        mM1.mGalBpAmpI = atoi(mField[eGalBpAmpI].c_str());
        mM1.mGalBpAmpQ = atoi(mField[eGalBpAmpQ].c_str());
    }

    inline SystemStatusPQWM1& get() { return mM1;} //getparser
};

/******************************************************************************
 SystemStatusPQWP1
******************************************************************************/
class SystemStatusPQWP1
{
public:
    uint8_t  mEpiValidity; // x4
    float    mEpiLat;    // x5
    float    mEpiLon;    // x6
    float    mEpiAlt;    // x7
    float    mEpiHepe;   // x8
    float    mEpiAltUnc; // x9
    uint8_t  mEpiSrc;    // x10
};

class SystemStatusPQWP1parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eEpiValidity = 2,
        eEpiLat = 3,
        eEpiLon = 4,
        eEpiAlt = 5,
        eEpiHepe = 6,
        eEpiAltUnc = 7,
        eEpiSrc = 8,
        eMax
    };
    SystemStatusPQWP1 mP1;

public:
    inline uint8_t    getEpiValidity() { return mP1.mEpiValidity;      }
    inline float      getEpiLat() { return mP1.mEpiLat;           }
    inline float      getEpiLon() { return mP1.mEpiLon;           }
    inline float      getEpiAlt() { return mP1.mEpiAlt;           }
    inline float      getEpiHepe() { return mP1.mEpiHepe;          }
    inline float      getEpiAltUnc() { return mP1.mEpiAltUnc;        }
    inline uint8_t    getEpiSrc() { return mP1.mEpiSrc;           }

    SystemStatusPQWP1parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP1, 0, sizeof(mP1));
        mP1.mEpiValidity = strtol(mField[eEpiValidity].c_str(), NULL, 16);
        mP1.mEpiLat = atof(mField[eEpiLat].c_str());
        mP1.mEpiLon = atof(mField[eEpiLon].c_str());
        mP1.mEpiAlt = atof(mField[eEpiAlt].c_str());
        mP1.mEpiHepe = atoi(mField[eEpiHepe].c_str());
        mP1.mEpiAltUnc = atof(mField[eEpiAltUnc].c_str());
        mP1.mEpiSrc = atoi(mField[eEpiSrc].c_str());
    }

    inline SystemStatusPQWP1& get() { return mP1;}
};

/******************************************************************************
 SystemStatusPQWP2
******************************************************************************/
class SystemStatusPQWP2
{
public:
    float    mBestLat;   // x4
    float    mBestLon;   // x5
    float    mBestAlt;   // x6
    float    mBestHepe;  // x7
    float    mBestAltUnc; // x8
};

class SystemStatusPQWP2parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eBestLat = 2,
        eBestLon = 3,
        eBestAlt = 4,
        eBestHepe = 5,
        eBestAltUnc = 6,
        eMax
    };
    SystemStatusPQWP2 mP2;

public:
    inline float      getBestLat() { return mP2.mBestLat;          }
    inline float      getBestLon() { return mP2.mBestLon;          }
    inline float      getBestAlt() { return mP2.mBestAlt;          }
    inline float      getBestHepe() { return mP2.mBestHepe;         }
    inline float      getBestAltUnc() { return mP2.mBestAltUnc;       }

    SystemStatusPQWP2parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP2, 0, sizeof(mP2));
        mP2.mBestLat = atof(mField[eBestLat].c_str());
        mP2.mBestLon = atof(mField[eBestLon].c_str());
        mP2.mBestAlt = atof(mField[eBestAlt].c_str());
        mP2.mBestHepe = atof(mField[eBestHepe].c_str());
        mP2.mBestAltUnc = atof(mField[eBestAltUnc].c_str());
    }

    inline SystemStatusPQWP2& get() { return mP2;}
};

/******************************************************************************
 SystemStatusPQWP3
******************************************************************************/
class SystemStatusPQWP3
{
public:
    uint8_t   mXtraValidMask;
    uint32_t  mGpsXtraAge;
    uint32_t  mGloXtraAge;
    uint32_t  mBdsXtraAge;
    uint32_t  mGalXtraAge;
    uint32_t  mQzssXtraAge;
    uint32_t  mGpsXtraValid;
    uint32_t  mGloXtraValid;
    uint64_t  mBdsXtraValid;
    uint64_t  mGalXtraValid;
    uint8_t   mQzssXtraValid;
};

class SystemStatusPQWP3parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eXtraValidMask = 2,
        eGpsXtraAge = 3,
        eGloXtraAge = 4,
        eBdsXtraAge = 5,
        eGalXtraAge = 6,
        eQzssXtraAge = 7,
        eGpsXtraValid = 8,
        eGloXtraValid = 9,
        eBdsXtraValid = 10,
        eGalXtraValid = 11,
        eQzssXtraValid = 12,
        eMax
    };
    SystemStatusPQWP3 mP3;

public:
    inline uint8_t    getXtraValid() { return mP3.mXtraValidMask;   }
    inline uint32_t   getGpsXtraAge() { return mP3.mGpsXtraAge;       }
    inline uint32_t   getGloXtraAge() { return mP3.mGloXtraAge;       }
    inline uint32_t   getBdsXtraAge() { return mP3.mBdsXtraAge;       }
    inline uint32_t   getGalXtraAge() { return mP3.mGalXtraAge;       }
    inline uint32_t   getQzssXtraAge() { return mP3.mQzssXtraAge;      }
    inline uint32_t   getGpsXtraValid() { return mP3.mGpsXtraValid;     }
    inline uint32_t   getGloXtraValid() { return mP3.mGloXtraValid;     }
    inline uint64_t   getBdsXtraValid() { return mP3.mBdsXtraValid;     }
    inline uint64_t   getGalXtraValid() { return mP3.mGalXtraValid;     }
    inline uint8_t    getQzssXtraValid() { return mP3.mQzssXtraValid;    }

    SystemStatusPQWP3parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP3, 0, sizeof(mP3));
        mP3.mXtraValidMask = strtol(mField[eXtraValidMask].c_str(), NULL, 16);
        mP3.mGpsXtraAge = atoi(mField[eGpsXtraAge].c_str());
        mP3.mGloXtraAge = atoi(mField[eGloXtraAge].c_str());
        mP3.mBdsXtraAge = atoi(mField[eBdsXtraAge].c_str());
        mP3.mGalXtraAge = atoi(mField[eGalXtraAge].c_str());
        mP3.mQzssXtraAge = atoi(mField[eQzssXtraAge].c_str());
        mP3.mGpsXtraValid = strtol(mField[eGpsXtraValid].c_str(), NULL, 16);
        mP3.mGloXtraValid = strtol(mField[eGloXtraValid].c_str(), NULL, 16);
        mP3.mBdsXtraValid = strtol(mField[eBdsXtraValid].c_str(), NULL, 16);
        mP3.mGalXtraValid = strtol(mField[eGalXtraValid].c_str(), NULL, 16);
        mP3.mQzssXtraValid = strtol(mField[eQzssXtraValid].c_str(), NULL, 16);
    }

    inline SystemStatusPQWP3& get() { return mP3;}
};

/******************************************************************************
 SystemStatusPQWP4
******************************************************************************/
class SystemStatusPQWP4
{
public:
    uint32_t  mGpsEpheValid;
    uint32_t  mGloEpheValid;
    uint64_t  mBdsEpheValid;
    uint64_t  mGalEpheValid;
    uint8_t   mQzssEpheValid;
};

class SystemStatusPQWP4parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eGpsEpheValid = 2,
        eGloEpheValid = 3,
        eBdsEpheValid = 4,
        eGalEpheValid = 5,
        eQzssEpheValid = 6,
        eMax
    };
    SystemStatusPQWP4 mP4;

public:
    inline uint32_t   getGpsEpheValid() { return mP4.mGpsEpheValid;     }
    inline uint32_t   getGloEpheValid() { return mP4.mGloEpheValid;     }
    inline uint64_t   getBdsEpheValid() { return mP4.mBdsEpheValid;     }
    inline uint64_t   getGalEpheValid() { return mP4.mGalEpheValid;     }
    inline uint8_t    getQzssEpheValid() { return mP4.mQzssEpheValid;    }

    SystemStatusPQWP4parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP4, 0, sizeof(mP4));
        mP4.mGpsEpheValid = strtol(mField[eGpsEpheValid].c_str(), NULL, 16);
        mP4.mGloEpheValid = strtol(mField[eGloEpheValid].c_str(), NULL, 16);
        mP4.mBdsEpheValid = strtol(mField[eBdsEpheValid].c_str(), NULL, 16);
        mP4.mGalEpheValid = strtol(mField[eGalEpheValid].c_str(), NULL, 16);
        mP4.mQzssEpheValid = strtol(mField[eQzssEpheValid].c_str(), NULL, 16);
    }

    inline SystemStatusPQWP4& get() { return mP4;}
};

/******************************************************************************
 SystemStatusPQWP5
******************************************************************************/
class SystemStatusPQWP5
{
public:
    uint32_t  mGpsUnknownMask;
    uint32_t  mGloUnknownMask;
    uint64_t  mBdsUnknownMask;
    uint64_t  mGalUnknownMask;
    uint8_t   mQzssUnknownMask;
    uint32_t  mGpsGoodMask;
    uint32_t  mGloGoodMask;
    uint64_t  mBdsGoodMask;
    uint64_t  mGalGoodMask;
    uint8_t   mQzssGoodMask;
    uint32_t  mGpsBadMask;
    uint32_t  mGloBadMask;
    uint64_t  mBdsBadMask;
    uint64_t  mGalBadMask;
    uint8_t   mQzssBadMask;
};

class SystemStatusPQWP5parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eGpsUnknownMask = 2,
        eGloUnknownMask = 3,
        eBdsUnknownMask = 4,
        eGalUnknownMask = 5,
        eQzssUnknownMask = 6,
        eGpsGoodMask = 7,
        eGloGoodMask = 8,
        eBdsGoodMask = 9,
        eGalGoodMask = 10,
        eQzssGoodMask = 11,
        eGpsBadMask = 12,
        eGloBadMask = 13,
        eBdsBadMask = 14,
        eGalBadMask = 15,
        eQzssBadMask = 16,
        eMax
    };
    SystemStatusPQWP5 mP5;

public:
    inline uint32_t   getGpsUnknownMask() { return mP5.mGpsUnknownMask;   }
    inline uint32_t   getGloUnknownMask() { return mP5.mGloUnknownMask;   }
    inline uint64_t   getBdsUnknownMask() { return mP5.mBdsUnknownMask;   }
    inline uint64_t   getGalUnknownMask() { return mP5.mGalUnknownMask;   }
    inline uint8_t    getQzssUnknownMask() { return mP5.mQzssUnknownMask;  }
    inline uint32_t   getGpsGoodMask() { return mP5.mGpsGoodMask;      }
    inline uint32_t   getGloGoodMask() { return mP5.mGloGoodMask;      }
    inline uint64_t   getBdsGoodMask() { return mP5.mBdsGoodMask;      }
    inline uint64_t   getGalGoodMask() { return mP5.mGalGoodMask;      }
    inline uint8_t    getQzssGoodMask() { return mP5.mQzssGoodMask;     }
    inline uint32_t   getGpsBadMask() { return mP5.mGpsBadMask;       }
    inline uint32_t   getGloBadMask() { return mP5.mGloBadMask;       }
    inline uint64_t   getBdsBadMask() { return mP5.mBdsBadMask;       }
    inline uint64_t   getGalBadMask() { return mP5.mGalBadMask;       }
    inline uint8_t    getQzssBadMask() { return mP5.mQzssBadMask;      }

    SystemStatusPQWP5parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP5, 0, sizeof(mP5));
        mP5.mGpsUnknownMask = strtol(mField[eGpsUnknownMask].c_str(), NULL, 16);
        mP5.mGloUnknownMask = strtol(mField[eGloUnknownMask].c_str(), NULL, 16);
        mP5.mBdsUnknownMask = strtol(mField[eBdsUnknownMask].c_str(), NULL, 16);
        mP5.mGalUnknownMask = strtol(mField[eGalUnknownMask].c_str(), NULL, 16);
        mP5.mQzssUnknownMask = strtol(mField[eQzssUnknownMask].c_str(), NULL, 16);
        mP5.mGpsGoodMask = strtol(mField[eGpsGoodMask].c_str(), NULL, 16);
        mP5.mGloGoodMask = strtol(mField[eGloGoodMask].c_str(), NULL, 16);
        mP5.mBdsGoodMask = strtol(mField[eBdsGoodMask].c_str(), NULL, 16);
        mP5.mGalGoodMask = strtol(mField[eGalGoodMask].c_str(), NULL, 16);
        mP5.mQzssGoodMask = strtol(mField[eQzssGoodMask].c_str(), NULL, 16);
        mP5.mGpsBadMask = strtol(mField[eGpsBadMask].c_str(), NULL, 16);
        mP5.mGloBadMask = strtol(mField[eGloBadMask].c_str(), NULL, 16);
        mP5.mBdsBadMask = strtol(mField[eBdsBadMask].c_str(), NULL, 16);
        mP5.mGalBadMask = strtol(mField[eGalBadMask].c_str(), NULL, 16);
        mP5.mQzssBadMask = strtol(mField[eQzssBadMask].c_str(), NULL, 16);
    }

    inline SystemStatusPQWP5& get() { return mP5;}
};

/******************************************************************************
 SystemStatusPQWP6parser
******************************************************************************/
class SystemStatusPQWP6
{
public:
    uint32_t  mFixInfoMask;
};

class SystemStatusPQWP6parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eFixInfoMask = 2,
        eMax
    };
    SystemStatusPQWP6 mP6;

public:
    inline uint32_t   getFixInfoMask() { return mP6.mFixInfoMask;      }

    SystemStatusPQWP6parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mP6, 0, sizeof(mP6));
        mP6.mFixInfoMask = strtol(mField[eFixInfoMask].c_str(), NULL, 16);
    }

    inline SystemStatusPQWP6& get() { return mP6;}
};

/******************************************************************************
 SystemStatusPQWS1parser
******************************************************************************/
class SystemStatusPQWS1
{
public:
    uint32_t  mFixInfoMask;
    uint32_t  mHepeLimit;
};

class SystemStatusPQWS1parser : public SystemStatusNmeaBase
{
private:
    enum
    {
        eTalker = 0,
        eUtcTime = 1,
        eFixInfoMask = 2,
        eHepeLimit = 3,
        eMax
    };
    SystemStatusPQWS1 mS1;

public:
    inline uint16_t   getFixInfoMask() { return mS1.mFixInfoMask;      }
    inline uint32_t   getHepeLimit()   { return mS1.mHepeLimit;      }

    SystemStatusPQWS1parser(const char *str_in, uint32_t len_in)
        : SystemStatusNmeaBase(str_in, len_in)
    {
        if (mField.size() < eMax) {
            return;
        }
        memset(&mS1, 0, sizeof(mS1));
        mS1.mFixInfoMask = atoi(mField[eFixInfoMask].c_str());
        mS1.mHepeLimit = atoi(mField[eHepeLimit].c_str());
    }

    inline SystemStatusPQWS1& get() { return mS1;}
};

/******************************************************************************
 SystemStatusTimeAndClock
******************************************************************************/
SystemStatusTimeAndClock::SystemStatusTimeAndClock(const SystemStatusPQWM1& nmea) :
    mGpsWeek(nmea.mGpsWeek),
    mGpsTowMs(nmea.mGpsTowMs),
    mTimeValid(nmea.mTimeValid),
    mTimeSource(nmea.mTimeSource),
    mTimeUnc(nmea.mTimeUnc),
    mClockFreqBias(nmea.mClockFreqBias),
    mClockFreqBiasUnc(nmea.mClockFreqBiasUnc),
    mLeapSeconds(nmea.mLeapSeconds),
    mLeapSecUnc(nmea.mLeapSecUnc)
{
}

bool SystemStatusTimeAndClock::equals(const SystemStatusTimeAndClock& peer)
{
    if ((mGpsWeek != peer.mGpsWeek) ||
        (mGpsTowMs != peer.mGpsTowMs) ||
        (mTimeValid != peer.mTimeValid) ||
        (mTimeSource != peer.mTimeSource) ||
        (mTimeUnc != peer.mTimeUnc) ||
        (mClockFreqBias != peer.mClockFreqBias) ||
        (mClockFreqBiasUnc != peer.mClockFreqBiasUnc) ||
        (mLeapSeconds != peer.mLeapSeconds) ||
        (mLeapSecUnc != peer.mLeapSecUnc)) {
        return false;
    }
    return true;
}

void SystemStatusTimeAndClock::dump()
{
    LOC_LOGL5("TimeAndClock: g=%d:%d v=%d ts=%d tu=%d ",
        mGpsWeek,
        mGpsTowMs,
        mTimeValid,
        mTimeSource,
        mTimeUnc);

    LOC_LOGL4(" b=%d bu=%d ls=%d lu=%d",
        mClockFreqBias,
        mClockFreqBiasUnc,
        mLeapSeconds,
        mLeapSecUnc);

    return;
}

/******************************************************************************
 SystemStatusXoState
******************************************************************************/
SystemStatusXoState::SystemStatusXoState(const SystemStatusPQWM1& nmea) :
    mXoState(nmea.mXoState)
{
}

bool SystemStatusXoState::equals(const SystemStatusXoState& peer)
{
    if (mXoState != peer.mXoState) {
        return false;
    }
    return true;
}

void SystemStatusXoState::dump()
{
    LOC_LOGM1("XoState: x=%d",
             mXoState);
    return;
}

/******************************************************************************
 SystemStatusRfAndParams
******************************************************************************/
SystemStatusRfAndParams::SystemStatusRfAndParams(const SystemStatusPQWM1& nmea) :
    mPgaGain(nmea.mPgaGain),
    mGpsBpAmpI(nmea.mGpsBpAmpI),
    mGpsBpAmpQ(nmea.mGpsBpAmpQ),
    mAdcI(nmea.mAdcI),
    mAdcQ(nmea.mAdcQ),
    mJammerGps(nmea.mJammerGps),
    mJammerGlo(nmea.mJammerGlo),
    mJammerBds(nmea.mJammerBds),
    mJammerGal(nmea.mJammerGal),
    mAgcGps(nmea.mAgcGps),
    mAgcGlo(nmea.mAgcGlo),
    mAgcBds(nmea.mAgcBds),
    mAgcGal(nmea.mAgcGal),
    mGloBpAmpI(nmea.mGloBpAmpI),
    mGloBpAmpQ(nmea.mGloBpAmpQ),
    mBdsBpAmpI(nmea.mBdsBpAmpI),
    mBdsBpAmpQ(nmea.mBdsBpAmpQ),
    mGalBpAmpI(nmea.mGalBpAmpI),
    mGalBpAmpQ(nmea.mGalBpAmpQ)
{
}

bool SystemStatusRfAndParams::equals(const SystemStatusRfAndParams& peer)
{
    if ((mPgaGain != peer.mPgaGain) ||
        (mGpsBpAmpI != peer.mGpsBpAmpI) ||
        (mGpsBpAmpQ != peer.mGpsBpAmpQ) ||
        (mAdcI != peer.mAdcI) ||
        (mAdcQ != peer.mAdcQ) ||
        (mJammerGps != peer.mJammerGps) ||
        (mJammerGlo != peer.mJammerGlo) ||
        (mJammerBds != peer.mJammerBds) ||
        (mJammerGal != peer.mJammerGal) ||
        (mAgcGps != peer.mAgcGps) ||
        (mAgcGlo != peer.mAgcGlo) ||
        (mAgcBds != peer.mAgcBds) ||
        (mAgcGal != peer.mAgcGal) ||
        (mGloBpAmpI != peer.mGloBpAmpI) ||
        (mGloBpAmpQ != peer.mGloBpAmpQ) ||
        (mBdsBpAmpI != peer.mBdsBpAmpI) ||
        (mBdsBpAmpQ != peer.mBdsBpAmpQ) ||
        (mGalBpAmpI != peer.mGalBpAmpI) ||
        (mGalBpAmpQ != peer.mGalBpAmpQ)) {
        return false;
    }
    return true;
}

void SystemStatusRfAndParams::dump()
{
    LOC_LOGL5("RfAndParams: p=%d bi=%d bq=%d ai=%d aq=%d ",
             mPgaGain,
             mGpsBpAmpI,
             mGpsBpAmpQ,
             mAdcI,
             mAdcQ);
    LOC_LOGL8("jgp=%d jgl=%d jbd=%d jga=%d "
        "agp=%d agl=%d abd=%d aga=%d",
        mJammerGps,
        mJammerGlo,
        mJammerBds,
        mJammerGal,
        mAgcGps,
        mAgcGlo,
        mAgcBds,
        mAgcGal);
    return;
}

/******************************************************************************
 SystemStatusErrRecovery
******************************************************************************/
SystemStatusErrRecovery::SystemStatusErrRecovery(const SystemStatusPQWM1& nmea) :
    mRecErrorRecovery(nmea.mRecErrorRecovery)
{
}

bool SystemStatusErrRecovery::equals(const SystemStatusErrRecovery& peer)
{
    if (mRecErrorRecovery != peer.mRecErrorRecovery) {
        return false;
    }
    return true;
}

void SystemStatusErrRecovery::dump()
{
    LOC_LOGM1("ErrRecovery: e=%d",
             mRecErrorRecovery);
    return;
}

/******************************************************************************
 SystemStatusInjectedPosition
******************************************************************************/
SystemStatusInjectedPosition::SystemStatusInjectedPosition(const SystemStatusPQWP1& nmea) :
    mEpiValidity(nmea.mEpiValidity),
    mEpiLat(nmea.mEpiLat),
    mEpiLon(nmea.mEpiLon),
    mEpiAlt(nmea.mEpiAlt),
    mEpiHepe(nmea.mEpiHepe),
    mEpiAltUnc(nmea.mEpiAltUnc),
    mEpiSrc(nmea.mEpiSrc)
{
}

bool SystemStatusInjectedPosition::equals(const SystemStatusInjectedPosition& peer)
{
    if ((mEpiValidity != peer.mEpiValidity) ||
        (mEpiLat != peer.mEpiLat) ||
        (mEpiLon != peer.mEpiLon) ||
        (mEpiAlt != peer.mEpiAlt) ||
        (mEpiHepe != peer.mEpiHepe) ||
        (mEpiAltUnc != peer.mEpiAltUnc) ||
        (mEpiSrc != peer.mEpiSrc)) {
        return false;
    }
    return true;
}

void SystemStatusInjectedPosition::dump()
{
    LOC_LOGM7("InjectedPosition: v=%x la=%d lo=%d al=%d he=%d au=%d es=%d",
             mEpiValidity,
             mEpiLat,
             mEpiLon,
             mEpiAlt,
             mEpiHepe,
             mEpiAltUnc,
             mEpiSrc);
    return;
}

/******************************************************************************
 SystemStatusBestPosition
******************************************************************************/
SystemStatusBestPosition::SystemStatusBestPosition(const SystemStatusPQWP2& nmea) :
    mValid(true),
    mBestLat(nmea.mBestLat),
    mBestLon(nmea.mBestLon),
    mBestAlt(nmea.mBestAlt),
    mBestHepe(nmea.mBestHepe),
    mBestAltUnc(nmea.mBestAltUnc)
{
}

bool SystemStatusBestPosition::equals(const SystemStatusBestPosition& peer)
{
    if ((mBestLat != peer.mBestLat) ||
        (mBestLon != peer.mBestLon) ||
        (mBestAlt != peer.mBestAlt) ||
        (mBestHepe != peer.mBestHepe) ||
        (mBestAltUnc != peer.mBestAltUnc)) {
        return false;
    }
    return true;
}

void SystemStatusBestPosition::dump()
{
    LOC_LOGM5("BestPosition: la=%d lo=%d al=%d he=%d au=%d",
             mBestLat,
             mBestLon,
             mBestAlt,
             mBestHepe,
             mBestAltUnc);
    return;
}

/******************************************************************************
 SystemStatusXtra
******************************************************************************/
SystemStatusXtra::SystemStatusXtra(const SystemStatusPQWP3& nmea) :
    mXtraValidMask(nmea.mXtraValidMask),
    mGpsXtraAge(nmea.mGpsXtraAge),
    mGloXtraAge(nmea.mGloXtraAge),
    mBdsXtraAge(nmea.mBdsXtraAge),
    mGalXtraAge(nmea.mGalXtraAge),
    mQzssXtraAge(nmea.mQzssXtraAge),
    mGpsXtraValid(nmea.mGpsXtraValid),
    mGloXtraValid(nmea.mGloXtraValid),
    mBdsXtraValid(nmea.mBdsXtraValid),
    mGalXtraValid(nmea.mGalXtraValid),
    mQzssXtraValid(nmea.mQzssXtraValid)
{
}

bool SystemStatusXtra::equals(const SystemStatusXtra& peer)
{
    if ((mXtraValidMask != peer.mXtraValidMask) ||
        (mGpsXtraAge != peer.mGpsXtraAge) ||
        (mGloXtraAge != peer.mGloXtraAge) ||
        (mBdsXtraAge != peer.mBdsXtraAge) ||
        (mGalXtraAge != peer.mGalXtraAge) ||
        (mQzssXtraAge != peer.mQzssXtraAge) ||
        (mGpsXtraValid != peer.mGpsXtraValid) ||
        (mGloXtraValid != peer.mGloXtraValid) ||
        (mBdsXtraValid != peer.mBdsXtraValid) ||
        (mGalXtraValid != peer.mGalXtraValid) ||
        (mQzssXtraValid != peer.mQzssXtraValid)) {
        return false;
    }
    return true;
}

void SystemStatusXtra::dump()
{
    LOC_LOGM6("SystemStatusXtra: m=%x a=%d:%d:%d:%d:%d",
             mXtraValidMask,
             mGpsXtraAge,
             mGloXtraAge,
             mBdsXtraAge,
             mGalXtraAge,
             mQzssXtraAge);
    LOC_LOGM5("SystemStatusXtra: v=%x:%x:%llx:%llx:%x",
        mGpsXtraValid,
        mGloXtraValid,
        mBdsXtraValid,
        mGalXtraValid,
        mQzssXtraValid);
    return;
}

/******************************************************************************
 SystemStatusEphemeris
******************************************************************************/
SystemStatusEphemeris::SystemStatusEphemeris(const SystemStatusPQWP4& nmea) :
    mGpsEpheValid(nmea.mGpsEpheValid),
    mGloEpheValid(nmea.mGloEpheValid),
    mBdsEpheValid(nmea.mBdsEpheValid),
    mGalEpheValid(nmea.mGalEpheValid),
    mQzssEpheValid(nmea.mQzssEpheValid)
{
}

bool SystemStatusEphemeris::equals(const SystemStatusEphemeris& peer)
{
    if ((mGpsEpheValid != peer.mGpsEpheValid) ||
        (mGloEpheValid != peer.mGloEpheValid) ||
        (mBdsEpheValid != peer.mBdsEpheValid) ||
        (mGalEpheValid != peer.mGalEpheValid) ||
        (mQzssEpheValid != peer.mQzssEpheValid)) {
        return false;
    }
    return true;
}

void SystemStatusEphemeris::dump()
{
    LOC_LOGM5("Ephemeris: ev=%x:%x:%llx:%llx:%x",
             mGpsEpheValid,
             mGloEpheValid,
             mBdsEpheValid,
             mGalEpheValid,
             mQzssEpheValid);
    return;
}

/******************************************************************************
 SystemStatusSvHealth
******************************************************************************/
SystemStatusSvHealth::SystemStatusSvHealth(const SystemStatusPQWP5& nmea) :
    mGpsUnknownMask(nmea.mGpsUnknownMask),
    mGloUnknownMask(nmea.mGloUnknownMask),
    mBdsUnknownMask(nmea.mBdsUnknownMask),
    mGalUnknownMask(nmea.mGalUnknownMask),
    mQzssUnknownMask(nmea.mQzssUnknownMask),
    mGpsGoodMask(nmea.mGpsGoodMask),
    mGloGoodMask(nmea.mGloGoodMask),
    mBdsGoodMask(nmea.mBdsGoodMask),
    mGalGoodMask(nmea.mGalGoodMask),
    mQzssGoodMask(nmea.mQzssGoodMask),
    mGpsBadMask(nmea.mGpsBadMask),
    mGloBadMask(nmea.mGloBadMask),
    mBdsBadMask(nmea.mBdsBadMask),
    mGalBadMask(nmea.mGalBadMask),
    mQzssBadMask(nmea.mQzssBadMask)
{
}

bool SystemStatusSvHealth::equals(const SystemStatusSvHealth& peer)
{
    if ((mGpsUnknownMask != peer.mGpsUnknownMask) ||
        (mGloUnknownMask != peer.mGloUnknownMask) ||
        (mBdsUnknownMask != peer.mBdsUnknownMask) ||
        (mGalUnknownMask != peer.mGalUnknownMask) ||
        (mQzssUnknownMask != peer.mQzssUnknownMask) ||
        (mGpsGoodMask != peer.mGpsGoodMask) ||
        (mGloGoodMask != peer.mGloGoodMask) ||
        (mBdsGoodMask != peer.mBdsGoodMask) ||
        (mGalGoodMask != peer.mGalGoodMask) ||
        (mQzssGoodMask != peer.mQzssGoodMask) ||
        (mGpsBadMask != peer.mGpsBadMask) ||
        (mGloBadMask != peer.mGloBadMask) ||
        (mBdsBadMask != peer.mBdsBadMask) ||
        (mGalBadMask != peer.mGalBadMask) ||
        (mQzssBadMask != peer.mQzssBadMask)) {
        return false;
    }
    return true;
}

void SystemStatusSvHealth::dump()
{
    LOC_LOGM5("SvHealth: u=%x:%x:%llx:%llx:%x",
             mGpsUnknownMask,
             mGloUnknownMask,
             mBdsUnknownMask,
             mGalUnknownMask,
             mQzssUnknownMask);
    LOC_LOGM5("SvHealth: g=%x:%x:%llx:%llx:%x",
        mGpsGoodMask,
        mGloGoodMask,
        mBdsGoodMask,
        mGalGoodMask,
        mQzssGoodMask);
    LOC_LOGM5("SvHealth: b=%x:%x:%llx:%llx:%x",
        mGpsBadMask,
        mGloBadMask,
        mBdsBadMask,
        mGalBadMask,
        mQzssBadMask);
    return;
}

/******************************************************************************
 SystemStatusPdr
******************************************************************************/
SystemStatusPdr::SystemStatusPdr(const SystemStatusPQWP6& nmea) :
    mFixInfoMask(nmea.mFixInfoMask)
{
}

bool SystemStatusPdr::equals(const SystemStatusPdr& peer)
{
    if (mFixInfoMask != peer.mFixInfoMask) {
        return false;
    }
    return true;
}

void SystemStatusPdr::dump()
{
    LOC_LOGM1("Pdr: m=%x", mFixInfoMask);
    return;
}

/******************************************************************************
 SystemStatusPositionFailure
******************************************************************************/
SystemStatusPositionFailure::SystemStatusPositionFailure(const SystemStatusPQWS1& nmea) :
    mFixInfoMask(nmea.mFixInfoMask),
    mHepeLimit(nmea.mHepeLimit)
{
}

bool SystemStatusPositionFailure::equals(const SystemStatusPositionFailure& peer)
{
    if ((mFixInfoMask != peer.mFixInfoMask) ||
        (mHepeLimit != peer.mHepeLimit)) {
        return false;
    }
    return true;
}

void SystemStatusPositionFailure::dump()
{
    LOC_LOGM2("PositionFailure: m=%d h=%d",
             mFixInfoMask,
             mHepeLimit);
    return;
}

/******************************************************************************
 SystemStatusLocation
******************************************************************************/
bool SystemStatusLocation::equals(const SystemStatusLocation& peer)
{
    if ((mLocation.gpsLocation.latitude != peer.mLocation.gpsLocation.latitude) ||
        (mLocation.gpsLocation.longitude != peer.mLocation.gpsLocation.longitude) ||
        (mLocation.gpsLocation.altitude != peer.mLocation.gpsLocation.altitude)) {
        return false;
    }
    return true;
}

void SystemStatusLocation::dump()
{
    LOC_LOGM4("Location: lat=%d lon=%d alt=%d spd=%d",
             mLocation.gpsLocation.latitude,
             mLocation.gpsLocation.longitude,
             mLocation.gpsLocation.altitude,
             mLocation.gpsLocation.speed);
    return;
}

/******************************************************************************
 SystemStatus
******************************************************************************/
pthread_mutex_t   SystemStatus::mMutexSystemStatus;
SystemStatus*     SystemStatus::mInstance = NULL;

SystemStatus* SystemStatus::getInstance(const MsgTask* msgTask)
{
    pthread_mutex_lock(&mMutexSystemStatus);

    if (!mInstance) {
        // Instantiating for the first time. msgTask should not be NULL
        if (msgTask == NULL) {
            LOC_LOGE0("SystemStatus: msgTask is NULL!!");
            pthread_mutex_unlock(&mMutexSystemStatus);
            return NULL;
        }
        mInstance = new SystemStatus(msgTask);
        LOC_LOGM2("SystemStatus::getInstance:%p. Msgtask:%p", mInstance, msgTask);
    }

    pthread_mutex_unlock(&mMutexSystemStatus);
    return mInstance;
}

void SystemStatus::destroyInstance()
{
    delete mInstance;
    mInstance = NULL;
}

SystemStatus::SystemStatus(const MsgTask* msgTask) :
    mConnected(false)
{
    mCache.mLocation.clear();

    mCache.mTimeAndClock.clear();
    mCache.mXoState.clear();
    mCache.mRfAndParams.clear();
    mCache.mErrRecovery.clear();

    mCache.mInjectedPosition.clear();
    mCache.mBestPosition.clear();
    mCache.mXtra.clear();
    mCache.mEphemeris.clear();
    mCache.mSvHealth.clear();
    mCache.mPdr.clear();

    mCache.mPositionFailure.clear();
}

/******************************************************************************
 SystemStatus - storing dataitems
******************************************************************************/
template <typename TYPE_REPORT, typename TYPE_ITEM>
bool SystemStatus::setIteminReport(TYPE_REPORT& report, const TYPE_ITEM& s)
{
    if (!report.empty() && report.back().equals(s)) {
        // there is no change - just update reported timestamp
        LOC_LOGE0("setIteminReport() returns false");
        return false;
    }

    // first event or updated
    report.push_back(s);
    if (report.size() > s.maxItem) {
        report.erase(report.begin());
    }
    return true;
}

template <typename TYPE_REPORT, typename TYPE_ITEM>
void SystemStatus::setDefaultIteminReport(TYPE_REPORT& report, const TYPE_ITEM& s)
{
    report.push_back(s);
    if (report.size() > s.maxItem) {
        report.erase(report.begin());
    }
}

template <typename TYPE_REPORT, typename TYPE_ITEM>
void SystemStatus::getIteminReport(TYPE_REPORT& reportout, const TYPE_ITEM& c) const
{
    reportout.clear();
    if (c.size() >= 1) {
        reportout.push_back(c.back());
        reportout.back().dump();
    }
}

/******************************************************************************
@brief      API to set report data into internal buffer

@param[In]  data pointer to the NMEA string
@param[In]  len  length of the NMEA string

@return     true when successfully done
******************************************************************************/
bool SystemStatus::setNmeaString(const char *data, uint32_t len)
{
    bool ret = false;
    if (!loc_nmea_is_debug(data, len)) {
        return false;
    }

    char buf[SystemStatusNmeaBase::NMEA_MAXSIZE + 1] = { 0 };
    strlcpy(buf, data, sizeof(buf));

    pthread_mutex_lock(&mMutexSystemStatus);

    // parse the received nmea strings here
    if (0 == strncmp(data, "$PQWM1", SystemStatusNmeaBase::NMEA_MINSIZE)) {
        SystemStatusPQWM1 s = SystemStatusPQWM1parser(buf, len).get();
        ret |= setIteminReport(mCache.mTimeAndClock, SystemStatusTimeAndClock(s));
        ret |= setIteminReport(mCache.mRfAndParams, SystemStatusRfAndParams(s));
    }
    pthread_mutex_unlock(&mMutexSystemStatus);

    return ret;
}

/******************************************************************************
@brief      API to set report position data into internal buffer

@param[In]  UlpLocation

@return     true when successfully done
******************************************************************************/
bool SystemStatus::eventPosition(const UlpLocation& location,
                                 const GpsLocationExtended& locationEx)
{
    bool ret = false;
    pthread_mutex_lock(&mMutexSystemStatus);

    ret = setIteminReport(mCache.mLocation, SystemStatusLocation(location, locationEx));
    LOC_LOGM4("eventPosition - lat=%d lon=%d alt=%d speed=%d",
             location.gpsLocation.latitude,
             location.gpsLocation.longitude,
             location.gpsLocation.altitude,
             location.gpsLocation.speed);

    pthread_mutex_unlock(&mMutexSystemStatus);
    return ret;
}
/******************************************************************************
@brief      API to get report data into a given buffer

@param[In]  reference to report buffer
@param[In]  bool flag to identify latest only or entire buffer

@return     true when successfully done
******************************************************************************/
bool SystemStatus::getReport(SystemStatusReports& report, bool isLatestOnly) const
{
    pthread_mutex_lock(&mMutexSystemStatus);

    if (isLatestOnly) {
        // push back only the latest report and return it
        getIteminReport(report.mLocation, mCache.mLocation);

        getIteminReport(report.mTimeAndClock, mCache.mTimeAndClock);
        getIteminReport(report.mXoState, mCache.mXoState);
        getIteminReport(report.mRfAndParams, mCache.mRfAndParams);
        getIteminReport(report.mErrRecovery, mCache.mErrRecovery);

        getIteminReport(report.mInjectedPosition, mCache.mInjectedPosition);
        getIteminReport(report.mBestPosition, mCache.mBestPosition);
        getIteminReport(report.mXtra, mCache.mXtra);
        getIteminReport(report.mEphemeris, mCache.mEphemeris);
        getIteminReport(report.mSvHealth, mCache.mSvHealth);
        getIteminReport(report.mPdr, mCache.mPdr);

        getIteminReport(report.mPositionFailure, mCache.mPositionFailure);
    }
    else {
        // copy entire reports and return them
        report.mLocation.clear();

        report.mTimeAndClock.clear();
        report.mXoState.clear();
        report.mRfAndParams.clear();
        report.mErrRecovery.clear();

        report.mInjectedPosition.clear();
        report.mBestPosition.clear();
        report.mXtra.clear();
        report.mEphemeris.clear();
        report.mSvHealth.clear();
        report.mPdr.clear();

        report.mPositionFailure.clear();
        report = mCache;
    }

    pthread_mutex_unlock(&mMutexSystemStatus);
    return true;
}

/******************************************************************************
@brief      API to set default report data

@param[In]  none

@return     true when successfully done
******************************************************************************/
bool SystemStatus::setDefaultReport(void)
{
    pthread_mutex_lock(&mMutexSystemStatus);

    setDefaultIteminReport(mCache.mLocation, SystemStatusLocation());

    setDefaultIteminReport(mCache.mTimeAndClock, SystemStatusTimeAndClock());
    setDefaultIteminReport(mCache.mXoState, SystemStatusXoState());
    setDefaultIteminReport(mCache.mRfAndParams, SystemStatusRfAndParams());
    setDefaultIteminReport(mCache.mErrRecovery, SystemStatusErrRecovery());

    setDefaultIteminReport(mCache.mInjectedPosition, SystemStatusInjectedPosition());
    setDefaultIteminReport(mCache.mBestPosition, SystemStatusBestPosition());
    setDefaultIteminReport(mCache.mXtra, SystemStatusXtra());
    setDefaultIteminReport(mCache.mEphemeris, SystemStatusEphemeris());
    setDefaultIteminReport(mCache.mSvHealth, SystemStatusSvHealth());
    setDefaultIteminReport(mCache.mPdr, SystemStatusPdr());

    setDefaultIteminReport(mCache.mPositionFailure, SystemStatusPositionFailure());
    pthread_mutex_unlock(&mMutexSystemStatus);
    return true;
}
} // namespace loc_core
