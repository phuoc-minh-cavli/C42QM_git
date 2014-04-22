/******************************************************************************
Copyright (c) 2018, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

#ifndef __GTPWWAN_DATA_REQUEST__
#define __GTPWWAN_DATA_REQUEST__
#include <string>
#include <LocNetIface.h>


enum GTPWWAN_DOWNLOAD_STATUS {
    GTPWWAN_DOWNLOAD_UNKNOWN = -1,
    GTPWWAN_DOWNLOAD_SUCCESS = 0,
    GTPWWAN_INTIALIZATION_FAIL,
    GTPWWAN_QMI_FDCL_LIST_FAIL,
    GTPWWAN_ENCODE_FAIL,
    GTPWWAN_DATA_SERVICE_FAIL,
    GTPWWAN_SSL_FAIL,
    GTPWWAN_CONNECTION_FAIL,
    GTPWWAN_HTTP_REQUEST_FAIL,
    GTPWWAN_HTTP_DOWNLOAD_ERROR,
    GTPWWAN_SERVER_REJECT, 
    GTPWWAN_DECODE_FAIL
};

#ifdef __cplusplus
extern "C" {
#endif
#include "qapi_netservices.h"
#ifdef __cplusplus
}
#endif

using namespace std;

namespace izat_gtpwwan {

class GtpWwanClient;
class GtpWwanConfig;


typedef struct http_client
{
    qapi_Net_HTTPc_handle_t client;
    qapi_Net_SSL_Obj_Hdl_t sslCtx;
    qapi_Net_SSL_Config_t *sslCfg;
    qapi_Net_HTTPc_Config_t *httpc_cfg;
    GtpWwanClient* instance;
    inline http_client():client(NULL),sslCtx(QAPI_NET_SSL_INVALID_HANDLE),sslCfg(NULL),httpc_cfg(NULL),instance(NULL){}
} http_client;

class GtpWwanDataRequest {
    public:
        GtpWwanDataRequest(GtpWwanClient* client);
        ~GtpWwanDataRequest();
        /**
         * @brief invoked in gtpwwan thread context once connection is established 
         *
         * @details this function is used 
         * @return void
         */
        void handleGtpWwanDataRequest();

        /**
         * @brief this will be invoked once you get http response   
         *
         * @details once invoked ,will handle response in this function based on the state we receive 
         * @param[in] arg	 this is parameter returned is same parameter which is send in request  .
         * @param[in] state   describe about state whether we received complete or time out etc
         *@param[in] http_resp contains response 
         * 
         * @return void
         */
        static void  http_client_cb(void *arg, int32_t state, void *http_resp);
        http_client*  mhttp_instance;

    private:	
        uint8_t *cert_data_buf;
        uint16_t cert_data_buf_len;
        GtpWwanClient* mGtpWwanClient;
#ifdef FEATURE_GTP_QSP
		LocNetIface mLocNetIface;
#endif
        /**
         * @brief will be invoked once asn encoding is done for http connection and request    
         *
         * @details once invoked ,will handle response in this function based on the state we receive 
         *
         * @param[in] url	 url of server which we have to contact to get xtra data .
         * @param[in] os  byte stream of asn encoded data 
         * 
         * @return GTPWWAN_DOWNLOAD_STATUS returns if status http connection and download 
         */
        void cleanHttpParamStack();
        GTPWWAN_DOWNLOAD_STATUS doDownloadGtpWwanData(string& url,  void* os,uint32_t length);
#ifdef FEATURE_GTP_QSP
		GTPWWAN_DOWNLOAD_STATUS doDownloadGtpWWANTCP(void* data,uint32_t length);
		static void locNetIfaceCallback(bool dataCallSuccess, void* Ptr);
#endif

};

} // end of namespace

#endif //__GTPWWAN_DATA_REQUEST__

