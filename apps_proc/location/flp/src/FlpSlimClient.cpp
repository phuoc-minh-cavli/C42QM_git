/*=========================================================================
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <log_util.h>
#include <loc_util_log.h>
#include <timer.h>

#include <FlpSlimClient.h>
#include <LocationAPI.h>
#include <FlpLocationAdapter.h>

LocationError FlpSlimClient::enableAmdDetection() {

    LOC_LOGL0("enableAmdDetection()");

    LocationError err = LOCATION_ERROR_SUCCESS;

    if (NULL == mClient) {

        mClient = new qmislim::Client(
                qmislim::eNODE_AP, qmislim::eNODE_MP);
        mClient->setCallback(this);

        LOC_LOGEVENT("FLP -> SLIM :: Connect");
        if (!mClient->Connect()) {
            LOC_LOGE0("Failed to connect to SLIM MPSS service !");
            err = LOCATION_ERROR_GENERAL_FAILURE;
        }

        if (LOCATION_ERROR_SUCCESS == err) {

            LOC_LOGEVENT("FLP -> SLIM :: Register");
            slimErrorEnumT slimErr = mClient->Register();
            if (eSLIM_SUCCESS != slimErr) {
                LOC_LOGE1("Failed to register slim client, err %d", slimErr);
                err = LOCATION_ERROR_GENERAL_FAILURE;
            }

            LOC_LOGL0("Wait for register success from MPSS");
            timer_sleep(50, T_MSEC, FALSE);
        }
    }

    if (LOCATION_ERROR_SUCCESS == err) {

        slimEnableQmdDataRequestStructT req = {0};
        req.enableConf.enable = 1;
        req.service = eSLIM_SERVICE_AMD;

        LOC_LOGEVENT("FLP -> SLIM :: EnableQmdData");
        slimErrorEnumT slimErr = mClient->EnableQmdData(req, NULL);
        if (eSLIM_SUCCESS != slimErr) {
            LOC_LOGE1("EnableQmdData failed, err %d", slimErr);
            err = LOCATION_ERROR_GENERAL_FAILURE;
        }
    }

    if (LOCATION_ERROR_SUCCESS == err) {
        mAmdDetectionEnabled = true;
    }

    return err;
}

LocationError FlpSlimClient::disableAmdDetection() {

    LocationError err = LOCATION_ERROR_SUCCESS;

    LOC_LOGEVENT("FLP -> SLIM :: DisableData AMD");
    slimErrorEnumT slimErr = mClient->DisableData(
            eSLIM_SERVICE_AMD, NULL, eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP);
    if (eSLIM_SUCCESS != slimErr) {
        LOC_LOGE1("Failed to disable qmd data, err %d", slimErr);
        err = LOCATION_ERROR_GENERAL_FAILURE;
    }

    if (LOCATION_ERROR_SUCCESS == err) {
        mAmdDetectionEnabled = false;
    }

    return err;
}

LocationError FlpSlimClient::enableSmdDetection() {

    LOC_LOGL0("enableSmdDetection()");

    LocationError err = LOCATION_ERROR_SUCCESS;

    if (NULL == mClient) {

        mClient = new qmislim::Client(
                qmislim::eNODE_AP, qmislim::eNODE_MP);

        LOC_LOGEVENT("FLP -> SLIM :: Connect");
        if (!mClient->Connect()) {
            LOC_LOGE0("Failed to connect to SLIM MPSS service !");
            err = LOCATION_ERROR_GENERAL_FAILURE;
        }

        if (LOCATION_ERROR_SUCCESS == err) {

            LOC_LOGEVENT("FLP -> SLIM :: Register");
            slimErrorEnumT slimErr = mClient->Register();
            if (eSLIM_SUCCESS != slimErr) {
                LOC_LOGE1("Failed to register slim client, err %d", slimErr);
                err = LOCATION_ERROR_GENERAL_FAILURE;
            }

            LOC_LOGL0("Wait for register success from MPSS");
            timer_sleep(50, T_MSEC, FALSE);
        }
    }

    if (LOCATION_ERROR_SUCCESS == err) {

        slimEnableQmdDataRequestStructT req = {0};
        req.enableConf.enable = 1;
        req.service = eSLIM_SERVICE_SMD;

        LOC_LOGEVENT("FLP -> SLIM :: EnableQmdData");
        slimErrorEnumT slimErr = mClient->EnableQmdData(req, NULL);
        if (eSLIM_SUCCESS != slimErr) {
            LOC_LOGE1("EnableQmdData failed, err %d", slimErr);
            err = LOCATION_ERROR_GENERAL_FAILURE;
        }
    }

    if (LOCATION_ERROR_SUCCESS == err) {
        mSmdDetectionEnabled = true;
    }

    return err;
}

LocationError FlpSlimClient::disableSmdDetection() {

    LocationError err = LOCATION_ERROR_SUCCESS;

    LOC_LOGEVENT("FLP -> SLIM :: DisableData SMD");
    slimErrorEnumT slimErr = mClient->DisableData(
            eSLIM_SERVICE_SMD, NULL, eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP);
    if (eSLIM_SUCCESS != slimErr) {
        LOC_LOGE1("Failed to disable qmd data, err %d", slimErr);
        err = LOCATION_ERROR_GENERAL_FAILURE;
    }

    if (LOCATION_ERROR_SUCCESS == err) {
        mSmdDetectionEnabled = false;
    }

    return err;
}

FlpSlimClient::~FlpSlimClient() {

    LOC_LOGL0("~FlpSlimClient()");

    if (NULL != mClient) {

        LOC_LOGEVENT("FLP -> SLIM :: Unregister");
        slimErrorEnumT slimErr = mClient->Unregister();
        if (eSLIM_SUCCESS != slimErr) {
            LOC_LOGE1("Failed to register slim client, err %d", slimErr);
        }

        LOC_LOGL0("Wait for unregister success from MPSS");
        timer_sleep(50, T_MSEC, FALSE);

        LOC_LOGEVENT("FLP -> SLIM :: Disconnect");
        if (!mClient->Disconnect()) {
            LOC_LOGE0("Failed to disconnect from SLIM MPSS service !");
        }

        LOC_LOGL0("Wait for disconnect success from MPSS");
        timer_sleep(50, T_MSEC, FALSE);

        delete mClient;
        mClient = NULL;
    }
}

void FlpSlimClient::onQmiClientResponse
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
)
{
    LOC_LOGH2("SLIM -> FLP :: Client Response, err %d, msg %d",
              eError, eMsgId);
}

void FlpSlimClient::onQmiClientData
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
)
{
    LOC_LOGH3("SLIM -> FLP :: Client Data, svc %d, err %d, msg %d",
              eService, eError, eMsgId);

    // interpret and forward indication to flp adapter
    if (eSLIM_MESSAGE_ID_QMD_DATA_IND == eMsgId) {

        if (qDataSize > 0 && NULL != pData) {

            slimQmdDataStructT* data = (slimQmdDataStructT*)pData;
            LOC_LOGH6("provider: %d, timeSource: %d, timestamp: 0x%x%x, flags: 0x%x, motionState: %d",
                      data->provider, data->timeSource, (int)(data->timestamp >> 32), (int)data->timestamp,
                      data->flags, data->motionState);

            if (mFlpAdapter != NULL) {

                LocationMotionInfo info = {};
                info.size = sizeof(LocationMotionInfo);
                info.timestamp = data->timestamp;
                if (eService == eSLIM_SERVICE_AMD){
                    switch(data->motionState) {
                    case eSLIM_QMD_MOTION_STATE_REST:
                        info.motionType = LOCATION_MOTION_TYPE_REST;
                        break;
                    case eSLIM_QMD_MOTION_STATE_MOVE:
                        info.motionType = LOCATION_MOTION_TYPE_MOVE_ABSOLUTE;
                        break;
                    default:
                        LOC_LOGE1("Invalid motion state in QMD indication, %d", data->motionState);
                    }
                } else if (eService == eSLIM_SERVICE_SMD) {
                    info.motionType = LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT;
                } else {
                    LOC_LOGE1("Invalid service %d", eService);
                }

                mFlpAdapter->reportMotionStateEvent(info);

            } else {
                LOC_LOGE0("FLP Adapter NULL");
            }

        } else {
            LOC_LOGE0("Invalid QMD data size.");
        }
    }
}

/**
 * @brief Callback for handling time update response.
 */
void FlpSlimClient::onQmiClientTimeUpdate
(
    void                *pUserCookie,
    slimErrorEnumT       eError,
    uint64_t             tRemoteRxTimeMs,
    uint64_t             tRemoteTxTimeMs
)
{
    LOC_LOGH1("SLIM -> FLP :: Time Update, err %d", eError);
}

/**
 * @brief Callback for handling QCCI error notification.
 */
void FlpSlimClient::onQmiClientError
(
    int32_t             lError
    /**< [in] Operation status. */
)
{
    LOC_LOGE1("SLIM -> FLP :: Client Error %d", lError);
}

/**
 * @brief Callback for handling service change indications.
 */
void FlpSlimClient::onQmiClientServiceChange
(
    slimServiceEnumT         eService,
    /**< [in] Reporting service */
    slimServiceProviderMaskT qServiceProviderMask
    /**< [in] Available service providers */
)
{
    LOC_LOGH2("SLIM -> FLP :: Service Change, service %d, mask %p",
              eService, qServiceProviderMask);

    if (eSLIM_SERVICE_AMD == eService) {
        mFlpAdapter->motionTrackingServiceChangeEvent(
                LOCATION_MOTION_TYPE_MOVE_ABSOLUTE, (0 != qServiceProviderMask));
    }

    if (eSLIM_SERVICE_SMD == eService) {
        mFlpAdapter->motionTrackingServiceChangeEvent(
                LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT, (0 != qServiceProviderMask));
    }
}

/**
* @brief Callback for handling QCCI disconnect result.
*/
void FlpSlimClient::onQmiClientDisconnect()
{
    LOC_LOGH0("SLIM -> FLP :: Disconnect");
}

/**
* @brief Callback for handling QMI-SLIM registration result.
*/
void FlpSlimClient::onQmiClientRegistered
(
    slimErrorEnumT       eError
    /**< [in] Operation status. */
)
{
    if (eSLIM_SUCCESS == eError) {
        LOC_LOGH0("SLIM -> FLP :: Register Success");
    } else {
        LOC_LOGE1("Register Failed, err %d", eError);
    }
}

/**
* @brief Callback for handling QMI-SLIM deregistration result.
*/
void FlpSlimClient::onQmiClientUnregistered
(
    slimErrorEnumT       eError
    /**< [in] Operation status. */
)
{
    if (eSLIM_SUCCESS == eError) {
        LOC_LOGH0("SLIM -> FLP :: Unregister Success");
    } else {
        LOC_LOGE1("Unregister Failed, err %d", eError);
    }
}
