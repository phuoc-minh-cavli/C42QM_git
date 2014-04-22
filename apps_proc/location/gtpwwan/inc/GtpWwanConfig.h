/******************************************************************************
Copyright (c) 2018-2020, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef __GTPWWAN_CONFIG_H__
#define __GTPWWAN_CONFIG_H__

#include <comdef.h>
#include <stdlib.h>
#include <stdint.h>

#include <vector>
#include <string>
#include <algorithm>


using namespace std;
namespace izat_gtpwwan {

  class GtpWwanConfig {
    public:
      GtpWwanConfig();
      ~GtpWwanConfig();
      inline uint32_t getGtpWwanInterval() 
      {
        return mGtpWwanIntervalSec;
      }

      inline uint32_t getGtpDataCallInterval() 
      {
        return mTimeBtwDataCall;
      }
      inline uint32_t getGtpDataRetryNum() 
      {
        return mNoOfRetryNum;
      }
      inline string getGtpServer() 
      {
        return mgtpurl;
      }
      inline string getGtpTag() 
      {
        return mGtpUrlTag;
      }

  	  inline string getOemID() 
  	  {
        return mOemID;
      }

  	  inline string getModelID() 
  	  {
        return mModelID;
      }

  	  inline uint8_t getIsMCSEnabled() 
  	  {
        return mIsMCSEnabled;
      }

#ifdef FEATURE_GTP_QSP
	  inline uint8_t getIsQSPEnabled() 
  	  {
        return mIsQSPEnabled;
      }

	  inline uint32_t getGtpPort() 
  	  {
        return mGtpWwanPort;
      }
#endif
    private:

      // Gtp wwan interval
      uint32_t mGtpWwanIntervalSec;
      string mgtpurl;
	  string mOemID;
	  string mModelID;
    string mGtpUrlTag;
	  uint8_t mIsMCSEnabled;
    uint32_t mTimeBtwDataCall;
    uint32_t mNoOfRetryNum;
#ifdef FEATURE_GTP_QSP
	  uint32_t mGtpWwanPort;
	  uint8_t mIsQSPEnabled;
#endif

  };

} // end of namespace

#endif // __GTPWWAN_CONFIG_H__
