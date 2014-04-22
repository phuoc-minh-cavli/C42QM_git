/***********************************************************
Copyright (c) 2018  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
#ifndef __PAL_NET_IF__
#define __PAL_NET_IF__

#include <XtraClient.h>
#include <XtraDataRequest.h>
#include <LocNetIface.h>

// APIs implemented in C
#ifdef __cplusplus
extern "C" {
#endif

#include <qapi_socket.h>
#include <qapi_ssl.h>

#ifdef __cplusplus
}
#endif

namespace izat_xtra {

typedef struct PalNetIfSslInst {

    qapi_Net_SSL_Obj_Hdl_t sslObjHdl;
    qapi_Net_SSL_Con_Hdl_t sslConHdl;
    qapi_Net_SSL_Config_t  sslConfig;
    qapi_Net_SSL_Role_t    sslRole;

} PalNetIfSslInst;

class PalNetIf {
public:
    inline PalNetIf(bool isSecure, XtraClient& xtraClient) :
        mXtraClient(xtraClient), 
        mIsSecure(isSecure),
        mServer(), 
        mServerSize(),
        mHostname(), 
        mSocketFd(-1), 
        mSslInstance(){ }

    ~PalNetIf();

    XTRA_DOWNLOAD_STATUS resolve(const string& hostname, const int& port);
    XTRA_DOWNLOAD_STATUS create();
    XTRA_DOWNLOAD_STATUS connect();
    XTRA_DOWNLOAD_STATUS write(const string& request);
    int read(char* const buf, const int& size);


private:
    XtraClient& mXtraClient;
    bool mIsSecure;
    sockaddr mServer;
    size_t mServerSize;
    string mHostname;
    int mSocketFd;
    PalNetIfSslInst mSslInstance;

    XTRA_DOWNLOAD_STATUS setupSsl();
    void teardownSsl();
};

}

#endif // __PAL_NET_IF__
