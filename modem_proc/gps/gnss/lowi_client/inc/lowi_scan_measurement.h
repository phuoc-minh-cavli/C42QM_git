#ifndef __LOWI_SCAN_MEASUREMENT_H__
#define __LOWI_SCAN_MEASUREMENT_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Scan Measurement Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWIScanMeasurement

  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


=============================================================================*/
#if APSS
#include <inc/lowi_request.h>
#include <inc/lowi_ssid.h>
#include <inc/lowi_mac_address.h>
#else
#include "lowi_request.h"
#include "lowi_ssid.h"
#include "lowi_mac_address.h"
#include "vector.h"
#endif

namespace qc_loc_fw
{

/**
 * This struct defines the MSAP related data.
 */
struct LOWIMsapInfo
{
  uint8 protocolVersion;
  uint32        venueHash;
  uint8 serverIdx;
};

/** Beacon information element used to store LCI/LCR data */
struct LOWILocationIE
{
  uint8 id;       // element identifier
  uint8 len;      // number of bytes to follow
  uint8 *locData; // data blob
public:
  /** Constructor  & Copy constructor */
  LOWILocationIE();
  LOWILocationIE(const LOWILocationIE& rhs);
  /** Destructor */
  ~LOWILocationIE();

  /**
   * Prints Location IE
   */
  void printLocationIE ();
};

/**
 * This class defines Measurement Info per Wifi Node.
 * For a DiscoveryScan only rssi and rssi_timestamp are valid fields
 * For a Ranging scan rtt, rtt_timestamp and rssi are valid fields.
 */
class LOWIMeasurementInfo
{
public:
  /**
   * Measurement age msec
   * In units of 1 msec, -1 means info not available
   */
  int32         meas_age;

  /** Measurement time stamp.
   * For DiscoveryScanResponse
   *  time stamp is corresponding to when the beacon was received.
   */
  int64         rssi_timestamp;
  /** Signal strength in 0.5dBm*/
  int16         rssi;
public:
  // Constructor
  LOWIMeasurementInfo ()
  : meas_age (-1), rssi_timestamp (0), rssi (0)
  {
  }
  // Destructor
  virtual ~LOWIMeasurementInfo ()
  {
  }

  /** Type of the Measurement Info */
  enum eMeasurementInfoType
  {
    LOWI_SCAN_MEASUREMENT_INFO = 0,
    LOWI_RANGING_MEASUREMENT_INFO = 1
  };

  /**
   * Returns the MeasurementInfo type
   * @return eMeasurementInfoType Type of MeasurementInfo
   */
  virtual eMeasurementInfoType getMeasurementInfoType ()
  {
    return LOWI_SCAN_MEASUREMENT_INFO;
  }

};

/**
 * This class defines the Measurement info per Wifi node for Ranging scan
 * Some fields are valid only when Detailed ranging results are requested
 * in the Ranging request.
 */
class LOWIRangingMeasurementInfo : public LOWIMeasurementInfo
{
public:
  /** Number of spatial streams
   * A value of 0 means information is not available.
   * Only available in Detailed ranging results */
  uint8          tx_nss;

  /** Modulation Coding Scheme (MCS) index defines the
   *  following:
   *  1) The Number of Streams used for TX & RX
   *  2) the Type of mudulation used
   *  3) the Coding Rate
   *
   *  Note: Does not apply to legacy frames (frames using schemes
   *  prior to 802.11n)
   *  Only available in Detailed ranging results
   */
  uint8          tx_mcsIdx;

  /** Number of spatial streams
   * A value of 0 means information is not available.
   * Only available in Detailed ranging results */
  uint8          rx_nss;

  /** Modulation Coding Scheme (MCS) index defines the
   *  following:
   *  1) The Number of Streams used for TX & RX
   *  2) the Type of mudulation used
   *  3) the Coding Rate
   *
   *  Note: Does not apply to legacy frames (frames using schemes
   *  prior to 802.11n)
   *  Only available in Detailed ranging results
   */
  uint8          rx_mcsIdx;


  /** RTT - value in pico sec. 0 is considered to be invalid rtt*/
  int32         rtt_ps;

  /** TX Parameters */

  /** Tx bitrate in units of 100 Kbps.
   * A value of 0 means information is not available.
   * Only available in Detailed ranging results
   */
  uint32         tx_bitrate;

  /** TX ranging preamble */
  eRangingPreamble          tx_preamble;

  /** Bandwidth of the transmitted ack from the device to Wifi Node */
  eRangingBandwidth          tx_bw;

  /** RX Parameters */

  /** Rx bitrate in units of 100 Kbps.
   * A value of 0 means information is not available.
   * Only available in Detailed ranging results
   */
  uint32         rx_bitrate;

  /** RX ranging preamble */
  eRangingPreamble          rx_preamble;

  /** Bandwidth of the received frame from the Wifi Node */
  eRangingBandwidth          rx_bw;

  /** Azimuth for Angle of Arrival (AOA).
   *  Only available in Detailed ranging results
   */
  int32         azimuthAOA;

  /** Elevation for Angle of Arrival (AOA).
   *  Only available in Detailed ranging results
   */
  int32         elevationAOA;

public:
  // Constructor
  LOWIRangingMeasurementInfo ()
  : rtt_ps (0),
    tx_bitrate (0), tx_preamble (RTT_PREAMBLE_LEGACY),
    tx_nss (0), tx_bw (BW_20MHZ), tx_mcsIdx (0),
    rx_bitrate (0), rx_preamble (RTT_PREAMBLE_LEGACY),
    rx_nss (0), rx_bw (BW_20MHZ), rx_mcsIdx (0),
    azimuthAOA (0), elevationAOA (0)
  {
  }

  // Destructor
  virtual ~LOWIRangingMeasurementInfo ()
  {
  }

  /**
   * Returns the MeasurementInfo type
   * @return eMeasurementInfoType Type of MeasurementInfo
   */
  virtual eMeasurementInfoType getMeasurementInfoType ()
  {
    return LOWI_RANGING_MEASUREMENT_INFO;
  }
};

/**
 * This class defines the measurement taken for every scan request.
 * This contains the measurements corresponding the discovery / ranging
 * scan requests. However, the fields are valid / invalid based on type
 * of scan as documented below.
 */
class LOWIScanMeasurement
{
private:
  static const char* const TAG;
public:
  /** This enum defines the Phy mode */
  enum eLOWIPhyMode
  {
    LOWI_PHY_MODE_UNKNOWN       = -1,
    LOWI_PHY_MODE_11A           = 0,  /* 11a Mode */
    LOWI_PHY_MODE_11G           = 1,  /* 11b/g Mode */
    LOWI_PHY_MODE_11B           = 2,  /* 11b Mode */
    LOWI_PHY_MODE_11GONLY       = 3,  /* 11g only Mode */
    LOWI_PHY_MODE_11NA_HT20     = 4,  /* 11na HT20 mode */
    LOWI_PHY_MODE_11NG_HT20     = 5,  /* 11ng HT20 mode */
    LOWI_PHY_MODE_11NA_HT40     = 6,  /* 11na HT40 mode */
    LOWI_PHY_MODE_11NG_HT40     = 7,  /* 11ng HT40 mode */
    LOWI_PHY_MODE_11AC_VHT20    = 8,  /* 5G 11ac VHT20 mode */
    LOWI_PHY_MODE_11AC_VHT40    = 9,  /* 5G 11ac VHT40 mode */
    LOWI_PHY_MODE_11AC_VHT80    = 10, /* 5G 11ac VHT80 mode */
    LOWI_PHY_MODE_11AC_VHT20_2G = 11, /* 2G 11ac VHT20 mode */
    LOWI_PHY_MODE_11AC_VHT40_2G = 12, /* 2G 11ac VHT40 mode */
    LOWI_PHY_MODE_11AC_VHT80_2G = 13, /* 2G 11ac VHT80 mode */
    LOWI_PHY_MODE_11AC_VHT80_80 = 14, /* 5G 11ac VHT80_80 mode */
    LOWI_PHY_MODE_11AC_VHT160   = 15, /* 5G 11ac VHT160 mode */
    LOWI_PHY_MODE_11AX_HE20     = 16, /* 5G 11ax HE20 mode */
    LOWI_PHY_MODE_11AX_HE40     = 17, /* 5G 11ax HE40 mode */
    LOWI_PHY_MODE_11AX_HE80     = 18, /* 5G 11ax HE80 mode */
    LOWI_PHY_MODE_11AX_HE80_80  = 19, /* 5G 11ax HE80_80 mode */
    LOWI_PHY_MODE_11AX_HE160    = 20, /* 5G 11ax HE160 mode */
    LOWI_PHY_MODE_11AX_HE20_2G  = 21, /* 2G 11ax HE20 mode */
    LOWI_PHY_MODE_11AX_HE40_2G  = 22, /* 2G 11ax HE40 mode */
    LOWI_PHY_MODE_11AX_HE80_2G  = 23, /* 2G 11ax HE80 mode */

    LOWI_PHY_MODE_MAX           = LOWI_PHY_MODE_11AX_HE80_2G
  };

  enum eLOWIRTTFlags {
    LOWI_RTT_FLAGS_UNKNOWN                           = 0,
    LOWI_RTT_FLAGS_IS_80211MC_RTT_RESPONDER          = (1<<0), // EID 127, bit 70
    LOWI_RTT_FLAGS_HAS_SECONDARY_CHANNEL_OFFSET      = (1<<1), // HT 8.4.2.59
    LOWI_RTT_FLAGS_SECONDARY_CHANNEL_OFFSET_IS_BELOW = (1<<2), // HT 8.4.2.59, secondary channel is below primary channel
  };

  /** Encryption type supported by the Wifi Node
   * NOTE: Expect changes to this interface. Not yet finalized
   */
  enum eEncryptionType
  {
    LOWI_ENCRYPTION_TYPE_UNKNOWN  = 0,
    LOWI_ENCRYPTION_TYPE_OPEN     = 1,
    LOWI_ENCRYPTION_TYPE_WEP      = 2,
    LOWI_ENCRYPTION_TYPE_WPA_PSK  = 3,
    LOWI_ENCRYPTION_TYPE_WPA_EAP  = 4
  };

  /**
   * Provides the PEER OEM information
   * This info is going to be available through Ranging scans
   * on Lithium and through Discovery scans in future chip sets.
   */
  enum ePeerOEM
  {
    LOWI_PEER_OEM_UNKNOWN = 0,
    LOWI_PEER_OEM_QTI     = 1
  };

  /** BSSID of the wifi node*/
  LOWIMacAddress    bssid;
  /** Frequency at which the node is detected*/
  uint32          frequency;
  /** Secure access point or not. Only valid for DiscoveryScanResponse*/
  bool            isSecure;
  /** Type of the Wifi Node. Only valid for DiscoveryScanResponse*/
  eNodeType       type;
  /** SSID. Only valid for DiscoveryScanResponse*/
  LOWISsid        ssid;
  /** MsapInfo - valid if not NULL. Only valid for DiscoveryScanResponse*/
  LOWIMsapInfo*   msapInfo;
  /** Cell power limit in dBm. Only valid for discovery scan results,
   *  if available. For ranging scan results will be always 0. */
  int8            cellPowerLimitdBm;
  /**
   * Dynamic array containing measurement info per Wifi node.
   * DiscoveryScan will only have one measurements where as the
   * the vector can contain multiple MeasurementInfo for a Ranging scan.
   */
  vector <LOWIMeasurementInfo*> measurementsInfo;

  /** 
   * A set of fields from eLOWIRTTFlags enumeration
   */
  uint32          rttFlags;

  /** Band center frequency 1 in MHz. */
  uint32          bandCenterFreq1;

  /** Band center frequency 2 in MHz. */
  uint32          bandCenterFreq2;

  /** Flag indicating if we are associated with this AP
   */
  bool            	associatedToAp;

  /** Contains the Country Code
   * i.e. 'U' 'S'
   * if there is no country code found, then the array will contain 0 0
   */
  uint8         	country_code [LOWI_COUNTRY_CODE_LEN];

  /** Phy Mode. */
  eLOWIPhyMode 		phyMode;

  /** Max Tx rate in Kbps supported by the Wifi Node.
  * Not available from LOWI-LP yet.
  */
  uint32          maxTxRate;

  /** Capability Info field as per 802.11 7.3.1.4 */
  uint16          capabilityInfo;

  /** Encryption Type supported by WiFi Node */
  eEncryptionType encryptionType;
  /** Target TSF
  * Wifi Node's time stamp embedded in the last beacon.
  * Note: 0 value for this field indicates "NOT VALID"
  * TSF: Time synchronization Function
  */
  uint64          targetTSF;

  /** Delta between the Local TSF and TSF reported by AP in microseconds */
  uint32          tsf_delta;

  /** For Discovery / Snoop Scan indicates type of Ranging supported by AP
  * For Ranging indicates Type of ranging measurement performed, if applicable
  */
  eRttType        rttType;

  /** Constructor*/
  LOWIScanMeasurement ();
  /** Destructor*/
  virtual ~LOWIScanMeasurement ();

  /** Copy constructor **/
  LOWIScanMeasurement( const LOWIScanMeasurement& rhs );
  /** Assignment operator **/
  LOWIScanMeasurement& operator=( const LOWIScanMeasurement& rhs );

  /** Type of the Scan Measurement */
  enum eScanMeasurementType
  {
    LOWI_SCAN_MEASUREMENT 				= 0,
    LOWI_RANGING_SCAN_MEASUREMENT 		= 1,
	LOWI_EXTENDED_RANGING_MEASUREMENT 	= 2,
  };

  /**
   * Returns the ScanMeasurement type
   * @return eScanMeasurementType Type of ScanMeasurement
   */
  virtual eScanMeasurementType getScanMeasurementType () const
  {
    return LOWI_SCAN_MEASUREMENT;
  }
};

/**
 * Class to provide the Ranging Scan measurements.
 */
class LOWIRangingScanMeasurement : public LOWIScanMeasurement
{
private:
  static const char* const TAG;

public:

  /**
   * This is an enumation of the list of error codes the Wi-fi
   * Driver will send to LOWI controller with the scan
   * measurements on a per Target basis.
   */
  enum eTargetStatus
  {
    LOWI_TARGET_STATUS_SUCCESS                         = 0,
    LOWI_TARGET_STATUS_FAILURE                         = 1,
    LOWI_TARGET_STATUS_FAIL_NO_RSP                     = 2, /* Target Doesn't respond to RTT request */
    LOWI_TARGET_STATUS_FAIL_REJECTED                   = 3, /* Target rejected RTT3 request - Applies to RTT3 only */
    LOWI_TARGET_STATUS_FAIL_FTM_TIMEOUT                = 4, /* Timing measurement Timesout */
    LOWI_TARGET_STATUS_TARGET_ON_DIFF_CHANNEL          = 5, /* Target on different Channel - failure to Range */
    LOWI_TARGET_STATUS_FAIL_TARGET_NOT_CAPABLE         = 6, /* Target not capable of Rtt3 ranging */
    LOWI_TARGET_STATUS_FAIL_INVALID_TS                 = 7, /* Invalid Time stamps when ranging with Target */
    LOWI_TARGET_STATUS_FAIL_TARGET_BUSY_TRY_LATER      = 8, /* Target is busy, please try RTT3 at a later time */
    LOWI_TARGET_STATUS_MAX
  };

  /**
   * Status for the measurements associated with this Target.
   */
  eTargetStatus        targetStatus;

  /**
   * Provides the OEM information.
   * Only provided as part of 2 sided ranging with the peer (RTT3)
   */
  ePeerOEM peerOEM;

  /**
   * Indicates maximum possible number of BSSs, including the reference BSS
   * sharing the same antenna connector.
   * If this field is not configured (value 0), then look at the colocatedBssids field
   */
  uint8 maxBssidsIndicator;

  /** Indicates, if the measurement contains detailed or basic info*/
  bool			detailed;

  /**
   * Returns the ScanMeasurement type
   * @return eScanMeasurementType Type of ScanMeasurement
   */
  virtual eScanMeasurementType getScanMeasurementType () const
  {
    return LOWI_RANGING_SCAN_MEASUREMENT;
  }

  // Constructor
  LOWIRangingScanMeasurement ()
  : targetStatus (LOWI_TARGET_STATUS_SUCCESS),
    maxBssidsIndicator (0),
    peerOEM (LOWI_PEER_OEM_UNKNOWN), detailed (false)
  {
  }

  // Destructor
  virtual ~LOWIRangingScanMeasurement ()
  {
  }
};

/**
 * Class to provide the Extended Ranging measurements.
 */
class LOWIExtendedRangingMeasurement : public LOWIScanMeasurement
{
private:
  static const char* const TAG;

public:


  /** LCI information element */
  LOWILocationIE *lciInfo;

  /** LCR information element */
  LOWILocationIE *lcrInfo;

  /**
   * Contains the explicit list of the Virtual mac addresses
   * which share the same antenna connector.
   * This field contains explicit Bssids only if the maxBssidsIndicator field is 0
   */
  vector <LOWIMacAddress> colocatedBssids;

    // Constructor
  LOWIExtendedRangingMeasurement ()
  : lciInfo (NULL), lcrInfo (NULL)
  {
  }

  // Destructor
  virtual ~LOWIExtendedRangingMeasurement ()
  {
    delete lciInfo;
    delete lcrInfo;
  }

  /**
   * Returns the ScanMeasurement type
   * @return eScanMeasurementType Type of ScanMeasurement
   */
  virtual eScanMeasurementType getScanMeasurementType ()
  {
    return LOWI_EXTENDED_RANGING_MEASUREMENT;
  }

  /** Copy constructor **/
  LOWIExtendedRangingMeasurement( const LOWIExtendedRangingMeasurement& rhs );
  /** Assignment operator **/
  LOWIExtendedRangingMeasurement& operator=( const LOWIExtendedRangingMeasurement& rhs );
};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_SCAN_MEASUREMENT_H__
