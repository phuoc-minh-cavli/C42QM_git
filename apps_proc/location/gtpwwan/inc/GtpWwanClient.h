/******************************************************************************
Copyright (c) 2018 - 2020, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef __GTPWWAN_CLIENT_H__
#define __GTPWWAN_CLIENT_H__
#include <string>

#include <loc_util_log.h>
#include <vector>
#include <LocNetIface.h>
#include "GtpWwanIzatAdapter.h"
#include "GtpWwanConfig.h"
#include "GtpWwanTimer.h"
#include "GtpWwanDataRequest.h"
#include "GtpWwanNvItems.h"
#include "GtpProtocol.h"
#include <loc_misc_utils.h>
#include "location_service_iot_v02.h"
#ifdef FEATURE_LOCATION_MCS
#include "CSManager.h"
#endif

#ifdef FEATURE_GTP_QSP
#include "GtpQSPProtocol.h"
#endif

using namespace std;

namespace izat_gtpwwan {

#define BLOCKING_WAIT_TIME_SEC 1

#define GTP_SERVER_CLIENT_CONTROL_CS_ENABLED (1 << 0)
#define GTP_SERVER_CLIENT_CONTROL_WWAN_SCAN_ENABLED (1 << 3) // if set WWAN scan else Serving cell CS
#define GTP_SERVER_CLIENT_CONTROL_WWAN_SCAN_POS_ENABLED (1 << 4)
#define GTP_SERVER_CLIENT_CONTROL_CS_MASK (1 << 5) // if set active crowdsourcing, if not passive
#define GTP_SERVER_CLIENT_CONTROL_CS_PREFERRED (1 << 6)
#define REQ_MASK_L4_CELLS             (0x01)
#define REQ_MASK_L3_REGIONS           (0x02)
#define REQ_MASK_L2_REGIONS           (0x04)
#define REQ_MASK_RESTRICTED_REGIONS   (0x08)
#define REQ_MASK_WWAN_ASSISTANCE_DATA (0x10)
#define REQ_MASK_TIME_ASSISTANCE_DATA (0x20)
#define REQ_MASK_WWAN_SCAN_POSITION   (0x40)


enum SOC_STATE
{
    STATE_UNKNOWN,
    STATE_CONNECTED,
    STATE_DISCONNECTED
};

enum GTP_STATE
{
	STATE_INIT_FAILED,
    STATE_IDLE,
    STATE_PENDING_DOWNLOAD,
    STATE_PENDING_DOWNLOADED
};

class GtpWwanClient {
    protected:
        // constructor & destructor
        GtpWwanClient(GtpWwanIzatAdapter* adapter);
        ~GtpWwanClient();
    public:
        static GtpWwanClient* createInstance(GtpWwanIzatAdapter* adapter);
        static  void destroyInstance();
        /*========================================================================
          FUNCTION	onConnectionStatusChange

          DESCRIPTION
          This function will inject failure incase if we donot get data
          connection status from dss within a particular time

          PARAMETERS
          connected - (IN) status if datacall  is established
          isWifi  - (IN) wifi is not there for these devices this is kept
          for future enhancements.
          RETURN VALUE
          None
          ==========================================================================*/
        void onConnectionStatusChange(bool connected, bool isWifi);
        /*========================================================================
          FUNCTION	onGtpWwanDownloadResult

          DESCRIPTION
          This method gets invoked in GTPWWAN MsgTask; and simply transfers control
          to onGtpWwanDownloadResultInternal() which is executed in main LocThread
          MsgTask


          PARAMETERS
          status - (IN) status if datacall  is established
          data  - (IN) data that we downloadloaded from server, null incase of failure
          length - (IN) length of data that we are passing
          .
          RETURN VALUE
          None
          ==========================================================================*/

        void onGtpWwanDownloadResult(
                const GTPWWAN_DOWNLOAD_STATUS status);
        /*========================================================================
          FUNCTION	setRetryAfterTime

          DESCRIPTION
          This method gets invoked when we get fdcl event from QMI, this used to initialise
          all members of gtp client, get data from the QMI and encode data

          PARAMETERS
          time -(IN) time after which we should contact server
          .
          RETURN VALUE
          None.
          ==========================================================================*/
        void setRetryAfterTime(const uint64_t time);
        /*========================================================================
          FUNCTION	onRequestBsObservedData

          DESCRIPTION
          This method gets invoked when we get fdcl event from QMI, this used to initialise
          all members of gtp client, get data from the QMI and encode data

          PARAMETERS
          None.
          .
          RETURN VALUE
          None.
          ==========================================================================*/
        void onRequestBsObservedData();
		bool initGtpWwanClient();

        void sendMsg(const Runnable& runnable, const void* dataPtr) const;
        void sendMsg(const LocMsg* msg) const;


        // GTP nv items
        inline GtpWwanNvItems* getGtpWwanNvItems() {
            return mGtpWwanNvItems;
        }

        // GTP config
        inline GtpWwanConfig* getGtpWwanConfig() {
            return mGtpWwanConfig;
        }

        // GTP config
        inline GtpProtocol* getGtpProtocol() {
            return mGtpProtocol;
        }

        // Loc Net Iface
        inline LocNetIface& getLocNetIface(){
            return mLocNetIface;
        }
        //To store observed data which we get from qmi
        LocBsObsData* mBsObsData;
        //To store observed data which we get from qmi
        LocFdclBsList* mFdclBsList;
		uint8_t mScanObsLength;
        //this same as pcid in xtra
        uint64_t pcid;
        //to store state of gtp
        SOC_STATE mSocState;
        GTP_STATE mGtpState;
        //this is used to establish data call
        LocNetIface 	mLocNetIface;
        //To store data which we decode from server and inject TLE
        Xt_Data* mInjectData;
        GtpWwanIzatAdapter* mGtpWwanIzatAdapter;
#ifdef FEATURE_GTP_QSP
		GtpQSPProtocol* mGtpQSPProtocol;
		GtpWwanConfig*       mGtpWwanConfig;
#endif
#ifdef FEATURE_LOCATION_MCS
		CSManager* mCsManager;
#endif
        void* mReponseData;
        uint32_t mReponseDataLength;
		uint32_t mTotalBytesReceived;
		uint32_t mBytesRemaining;

    /*========================================================================
      FUNCTION	onGtpWwanDataDownloadResult

      DESCRIPTION
      This method gets invoked in GTPWWAN MsgTask; and simply transfers control
      to onGtpWwanDownloadResult() which is executed in GTPWWAN MsgTask

      PARAMETERS
      status - (IN) status if datacall	is established
      data	- (IN) data that we downloadloaded from server, null incase of failure
      length - (IN) length of data that we are passing

      RETURN VALUE
      None
     ==========================================================================*/

     void onGtpWwanDataDownloadResult(
            const GTPWWAN_DOWNLOAD_STATUS status);

     MsgTask* getDlMsgTask();

     /*========================================================================
       FUNCTION  GtpWwanstartDataCall
       DESCRIPTION
       This method is used to start data call

       PARAMETERS
       gtpWwanClientPtr - (IN) this contains a pointer to instance gtpwwanclient
       where GtpWwanUploadRequestThread is invoke

       RETURN VALUE
       None.
       ==========================================================================*/

     void GtpWwanstartDataCall();

    void reportEngineStatusEvent(LocationMetaDataEngineStatus status);

    void reportEngineStatus(LocationMetaDataEngineStatus status);
	#ifdef FEATURE_LOCATION_MCS
	void fdclListReceived(qmiLocGetFdclBsListIndMsgT_v02 ind);
	bool processfdclList(void* fdcllist);
	#endif

    private:
        static GtpWwanClient* sGtpClientInstance;
        void onGtpWwanDownloadResultInternal(
                const GTPWWAN_DOWNLOAD_STATUS status);
        GtpWwanConfig*       mGtpWwanConfig;
        GtpWwanDataRequest*  mGtpWwanDataRequest;
        GtpWwanNvItems*      mGtpWwanNvItems;
        GtpProtocol*         mGtpProtocol;
        MsgTask*             mGtpWwanMsgTask;  // GtpWwan  msg task
        GtpWwanTimer         mDataRetry;
        uint32_t             mNoOfRetry;
        /*========================================================================
          FUNCTION	initGtpWwanClient

          DESCRIPTION
          this function is passed to DSS library and will be called when
          datacall is established or disconnected

          PARAMETERS
          None.

          RETURN VALUE
          TRUE - Initialization succeed.
          FALSE - Initialization failed.

          ==========================================================================*/
        
        void deinitGtpWwanClient();
#ifdef FEATURE_LOCATION_MCS
		int populateCSdata();
		
#endif
        /*========================================================================
          FUNCTION	locNetIfaceCallback

          DESCRIPTION
          this function is passed to DSS library and will be called when
          datacall is established or disconnected


          PARAMETERS
          dataCallSuccess - (IN)  status if datacall  is established .
          gtpWwanClientPtr - (IN) this contains a pointer to instance gtpwwanclient
          where GtpWwanUploadRequestThread is invoke
          .
          RETURN VALUE
          None.
          ==========================================================================*/
        static void locNetIfaceCallback(bool dataCallSuccess, void* gtpWwanClientPtr);
        /*========================================================================
          FUNCTION	sendMsgGtpWwanUploadRequest

          DESCRIPTION
          This method gets invoked in main LocThread MsgTask once we receive
          data(fdcl list and bsa data) from modem and create a gtpwwan thread/msg task
          to create a conncetion and contact server to get xtra-t data

          PARAMETERS
          gtpWwanClientPtr - (IN) this contains a pointer to instance gtpwwanclient
          where GtpWwanUploadRequestThread is invoke
          .
          RETURN VALUE
          None.
          ==========================================================================*/


        static void sendMsgGtpWwanUploadRequest(void* gtpwwanClientPtr);
        /*========================================================================
          FUNCTION	GtpWwanUploadRequestThread

          DESCRIPTION
          This method is used to start data request GtpWwanDownloadMsgTask context

          PARAMETERS
          gtpWwanClientPtr - (IN) this contains a pointer to instance gtpwwanclient
          where GtpWwanUploadRequestThread is invoke
          .
          RETURN VALUE
          None.
          ==========================================================================*/

        static void GtpWwanUploadRequestThread(void* gtpwwanClientPtr);

        static void GtpWwanStartDlTask(void* gtpwwanClientPtr);

};

} // end of namespace

#endif // __GtpWwan_CLIENT_H__
