/******************************************************************************
Copyright (c) 2018 - 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __GtpWwan_IZAT_ADAPTER__
#define __GtpWwan_IZAT_ADAPTER__

#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <vector>
#include <string>

using namespace std;
using namespace izat_core;

namespace izat_gtpwwan {

class GtpWwanClient;

class GtpWwanIzatAdapter : public IzatAdapterBase {
    public:
        clientinformation* mClientInfo;
        // this is set when locdeinit is tirggered (gtp is cleaned up)
        static bool mCleanup;
        GtpWwanIzatAdapter();
        virtual ~GtpWwanIzatAdapter();
        /*========================================================================
          FUNCTION	onRequestBsObservedData

          DESCRIPTION 
          This function will be called when we receive a fdcl event from modem 

          PARAMETERS
          clientinfo - (IN) client information which we get from modem 

          RETURN VALUE
          None
          ==========================================================================*/
        virtual bool onRequestBsObservedData(clientinformation* clientinfo);
        /*========================================================================
          FUNCTION	onInjectGtpWwanData

          DESCRIPTION 
          This function will be called to inject xtra-t data to modem after xta-t download 
          or in case of any error

          PARAMETERS
          data - (IN) xtra-t data which we need to intject into tle 

          RETURN VALUE
          None
          ==========================================================================*/

        void onInjectGtpWwanData();
        /*========================================================================
          FUNCTION	cleanUpCommand

          DESCRIPTION 
          This function will be from locaton API to clean uo gtp adapter 

          PARAMETERS
          None.

          RETURN VALUE
          None
          ==========================================================================*/
        void cleanUpCommand();
        void cleanUp();
        /*========================================================================
          FUNCTION	InitCommand

          DESCRIPTION 
          This function will be from locaton API indicate gtp adapter started  

          PARAMETERS
          None.

          RETURN VALUE
          None
          ==========================================================================*/
        void InitCommand();
        void startUp();
        /*========================================================================
          FUNCTION	SendMsgRequestBsObsList

          DESCRIPTION 
          This function will be called to get BsObsList from TLE for which we have to get
          cell inforamtion 

          PARAMETERS
          data - (OUT) BsObsList we should get from QMI

          RETURN VALUE
          None
          ==========================================================================*/
        int SendMsgRequestBsObsList(LocBsObsData* gtprequest);
        /*========================================================================
          FUNCTION	SendMsgRequestFdclList

          DESCRIPTION 
          This function will be called to get fdcl list from TLE for which we have to get
          cell inforamtion 
          PARAMETERS
          fdclbslist - (OUT) fdcl list we should get from QMI

          RETURN VALUE
          None
          ==========================================================================*/
        int SendMsgRequestFdclList(LocFdclBsList* fdclbslist);
        
        static void   setGtpDisabled(bool disabled);
        static bool   isGtpDisabled();

        void forwardMetaData(const LocationMetaData& metaData);
		void setRequestTag(uint8_t* requestTag, int length);
		#ifdef FEATURE_LOCATION_MCS
		virtual bool handleReportfdclList(void* fdclList);
		int SendMsgRequestFdclListAsync();
		#endif

    uint64_t mRequestTag;
    private:
        GtpWwanClient* mGtpWwanClient;
        /*========================================================================
          FUNCTION	sendMsgCbInjectGtpWwanData

          DESCRIPTION 
          This function will be called to inject xtra-t data to modem after xta-t download 
          or in case of any error

          PARAMETERS
          fdclbslist - (IN) fdcl list we should get from QMI

          RETURN VALUE
          None
          ==========================================================================*/
        static void sendMsgCbInjectGtpWwanData(void* gtpWwanIzatAdapterPtr);
};

} // end of namespace

#endif //__GtpWwan_IZAT_ADAPTER__
