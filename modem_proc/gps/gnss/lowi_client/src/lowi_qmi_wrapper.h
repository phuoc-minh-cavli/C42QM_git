#ifndef __LOWI_QMI_WRAPPER_H__
#define __LOWI_QMI_WRAPPER_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Low Power Scan Result Receiver Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWI LP Scan Result Receiver

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

=============================================================================*/
#if APSS
#include <base_util/sync.h>
#include <inc/lowi_request.h>
#include <inc/lowi_response.h>
#include <lowi_server/lowi_measurement_result.h>
#include <lowi_server/lowi_scan_result_receiver.h>
#include <lowi_server/lowi_qmi_client.h>
#else
#include "lowi_request.h"
#include "lowi_response.h"
#include "lowi_qmi_client.h"
#endif

namespace qc_loc_fw
{

// Forward declaration
class LOWIClientListener;

/**
 * This class provides the mechanism to get the scan measurements
 * from LOWI LP.
 * It's responsibility is to receive the results and provide the results
 * to it's listener. It's responsibility is to convert the data between
 * LOWI structures to LOWIQMIClient structures and pass the request and
 * responses. Also provides the mechanism to notify the listener when the
 * communication is lost with the QMI service.
 */
class LOWIQMIWrapper : public LOWIQMIClientListenerInterface
{
private:
  static const char * const         TAG;
  LOWIClientListener*               mListener;
  LOWIQMIClient*                    mQMIClient;
  bool                              mQMIClientReady;
  /** Vector containing the scan measurements for each AP */
  vector <LOWIScanMeasurement*>     mScanMeasurements;
  /** Vector containing the scanned frequencies */
  vector <LOWIChannelInfo>          mScannedFreq;
  /** Vector containing the ranging measurements for each AP */
  vector <LOWIScanMeasurement*>     mRangingScanMeasurements;
  bool 						        mClientNameValid;
  char						        mClientName[CLIENT_NAME_LEN+1];

  LOWIResponse::eScanStatus injectScanMeasurements
  (vector <LOWIScanMeasurement*>& result,
      QmiLowiDiscoveryResponseInd_v01* measurements);

  bool injectMeasurementInfo
  (vector <LOWIMeasurementInfo*>& vec,
      sLOWIDiscoveryMeasurementInfo_v01* pAp,
      uint32 num_of_meas);

  LOWIResponse::eScanStatus injectScanMeasurements
  (vector <LOWIScanMeasurement*>& result,
      QmiLowiRangingResponseInd_v01* measurements);

  bool injectMeasurementInfo
  (vector <LOWIMeasurementInfo*>& vec,
      sLOWIRangingMeasurementBasicInfo_v01* pAp,
      uint32 num_of_basic_meas,
      sLOWIRangingMeasurementDetailedInfo_v01* pDetailedMeas,
      uint32 num_of_detailed_meas);

  /**
   * Parses the RTT capabilities into fields in 
   * LOWIScanMeasurement::eLOWIRTTFlags 
   * @param rtt_cap RTT Capabilities received from WLAN
   * @return uint32 A bit mask using bit definitions from
   *                LOWIScanMeasurement::eLOWIRTTFlags
   */
  uint32 getRttFlags (uint32 rtt_cap);
  /**
   * Converts the Location IE from one format to another
   * 
   * @param sLOWILocationIE_v01& Reference to the Location IE 
   * 
   * @return LOWILocationIE* NULL or valid pointer containing 
   *         location IE
   */
  LOWILocationIE* getIEInfo (sLOWILocationIE_v01& ie);
public:
	
  /**
   * Sets the name of the client
   * @param char [] Null termintated array with client's name.
   */
  void setClientName (char client_name [CLIENT_NAME_LEN+1])
  {
	strlcpy (mClientName, client_name, CLIENT_NAME_LEN);
	mClientNameValid = true;
  }

  /**
   * It is a blocking call until the registeration
   * with the underlying messaging hub (LOWI QMI service)
   * It is a blocking call until the initialization completes or fails.
   * This function or asyncInit () must be called before issuing requests.
   * @param timeout_ms Init timeout with failure or success in msec
   *        default (0) - it is going to wait forever
   *
   * @return bool true - success, false - failure
   */
  bool init (uint32 timeout_ms);

  /**
   * Sends the Async initialization request. This is a 
   * non-blocking initialization. LOWI Client starts the QMI 
   * service discovery and notifies the 
   * LOWIQMIClientListenerInterface of the Service status as it 
   * becomes available. 
   * 
   * NOTE: This function or init() should be called before sending
   * any request, else expect request failure. Success from this 
   * function does not guarantee that the scan requests will 
   * succeed. Client should use the callback to know if the 
   * service is up before sending the scan request or expect 
   * failure. 
   * 
   * @return bool true if success, false otherwise
   */
  bool asyncInit ();

  /**
   * Sends LOWIRequest.
   * @param LOWIRequest*               LOWIRequest for which the
   *                                   scan is to be performed
   * @return bool true is success, false otherwise
   */
  virtual bool sendRequest (LOWIRequest* request);

  /**
   * Constructor
   * @param LOWIClientListener* listener to be notified
   *                            with measurement results / responses.
   */
  LOWIQMIWrapper(LOWIClientListener* listener);

  /** Destructor*/
  virtual ~LOWIQMIWrapper();

public:
  /**
   * Notifies acknowledgment to a request
   * @param QmiLOWIGenResp_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyAckResponse (QmiLOWIGenResp_v01* response,
      uint32 msg_id);

  /**
   * Notifies Discovery Scan response
   * @param QmiLowiDiscoveryResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (QmiLowiDiscoveryResponseInd_v01* response,
      uint32 msg_id);

  /**
   * Notifies Capabilities response
   * @param QmiLowiCapabilitiesResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiCapabilitiesResponseInd_v01* response,
      uint32 msg_id);

  /**
   * Notifies Wlan State Events response
   * @param QmiLowiWlanInterfaceEventsResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiWlanInterfaceEventsResponseInd_v01* response,
      uint32 msg_id);
  /**
   * Notifies LOWI Ranging response
   * @param QmiLowiRangingResponseInd_v01* Response pointer
   * @param uint32 QMI Message ID
   */
  virtual void notifyResponse (
      QmiLowiRangingResponseInd_v01* response,
      uint32 msg_id);
  /**
   * Notifies Error in QMI messaging with the service
   * @param Error code.
   */
  virtual void notifyError (qmi_client_error_type error);

  /**
   * Notifies Service Found / Lost in QMI thread context 
   * Note: This notification only comes if client used asyncInit 
   * () and not init () for initialization. 
   * @param bool true if Service is found, false if service is 
   *             lost
   */
  virtual void notifyServiceStatus (bool found);

};
} // namespace
#endif //#ifndef __LOWI_QMI_WRAPPER_H__
