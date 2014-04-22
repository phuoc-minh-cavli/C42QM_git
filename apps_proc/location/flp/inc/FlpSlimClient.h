/*=========================================================================
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef FLP_SLIM_CLIENT_H
#define FLP_SLIM_CLIENT_H

#include <QmiSlimClient.h>
#include <LocationAPI.h>

class FlpLocationAdapter;

class FlpSlimClient : public qmislim::Client::ICallback {

private:
    FlpLocationAdapter* mFlpAdapter;
    qmislim::Client* mClient;
    bool mAmdDetectionEnabled;
    bool mSmdDetectionEnabled;

public:
    FlpSlimClient(FlpLocationAdapter* flpAdapter) :
        mFlpAdapter(flpAdapter),
        mClient(NULL),
        mAmdDetectionEnabled(false),
        mSmdDetectionEnabled(false) {}
    virtual ~FlpSlimClient();

    // Enable/Disable QMD Data
    LocationError enableAmdDetection();
    LocationError disableAmdDetection();
    LocationError enableSmdDetection();
    LocationError disableSmdDetection();

    inline bool isAmdEnabled() { return mAmdDetectionEnabled; }
    inline bool isSmdEnabled() { return mSmdDetectionEnabled; }

    /* OVERRIDEN METHODS OF ICALLBACK */
    virtual void onQmiClientResponse
    (
        void                *pUserCookie,
        /**< [in] User-provided cookie pointer */
        slimErrorEnumT       eError,
        /**< [in] Operation status */
        slimMessageIdEnumT   eMsgId,
        /**< [in] SLIM message id that corresponds to response */
        const void          *pData,
        /**< [in] SLIM message payload */
        uint32_t             qDataSize
        /**< [in] SLIM message payload size */
    );
    virtual void onQmiClientData
    (
        slimServiceEnumT     eService,
        /**< [in] Reporting service */
        slimErrorEnumT       eError,
        /**< [in] Indication status */
        slimMessageIdEnumT   eMsgId,
        /**< [in] SLIM message id that corresponds to indication */
        const void          *pData,
        /**< [in] SLIM message payload */
        uint32_t             qDataSize
        /**< [in] SLIM message payload size */
    );
    virtual void onQmiClientTimeUpdate
    (
        void                *pUserCookie,
        slimErrorEnumT       eError,
        uint64_t             tRemoteRxTimeMs,
        uint64_t             tRemoteTxTimeMs
    );
    virtual void onQmiClientError
    (
        int32_t             lError
        /**< [in] Operation status. */
    );
    virtual void onQmiClientServiceChange
    (
        slimServiceEnumT         eService,
        /**< [in] Reporting service */
        slimServiceProviderMaskT qServiceProviderMask
        /**< [in] Available service providers */
    );
    virtual void onQmiClientDisconnect();
    virtual void onQmiClientRegistered
    (
        slimErrorEnumT       eError
        /**< [in] Operation status. */
    );
    virtual void onQmiClientUnregistered
    (
        slimErrorEnumT       eError
        /**< [in] Operation status. */
    );
};

#endif /* FLP_SLIM_CLIENT_H */
