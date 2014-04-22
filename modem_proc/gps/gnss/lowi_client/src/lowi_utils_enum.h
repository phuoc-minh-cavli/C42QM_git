#ifndef __LOWI_UTILS_ENUM_H__
#define __LOWI_UTILS_ENUM_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Utils Enum Interface Header file

GENERAL DESCRIPTION
  This file contains the definition and implementation of LOWIUtilsEnum class
 
  Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/
#include "comdef.h"
#include "lowi_request.h"
#include "lowi_client.h"
#include "lowi_log.h"
#include "lowi_scan_measurement.h"
#include "lowi_service_v01.h"

namespace qc_loc_fw
{

/**
 * Utility Class
 */
class LOWIUtilsEnum
{
private:
public:
  inline static eNodeType to_eNodeType (uint8 a)
  {
    switch (a)
    {
       case LOWI_NODE_TYPE_UNKNOWN_V01:     return NODE_TYPE_UNKNOWN;
       case LOWI_ACCESS_POINT_V01:          return ACCESS_POINT;
       case LOWI_PEER_DEVICE_V01:           return PEER_DEVICE;
       case LOWI_NAN_DEVICE_V01:            return NAN_DEVICE;
       case LOWI_STA_DEVICE_V01:            return STA_DEVICE;
       case LOWI_SOFT_AP_V01:               return SOFT_AP;
       default:                             return NODE_TYPE_UNKNOWN;
    }
  }
  inline static eQmiLowiNodeType_v01 to_eNodeType_v01 (uint8 a)
  {
    switch (a)
    {
       case NODE_TYPE_UNKNOWN:     return LOWI_NODE_TYPE_UNKNOWN_V01;
       case ACCESS_POINT:          return LOWI_ACCESS_POINT_V01;
       case PEER_DEVICE:           return LOWI_PEER_DEVICE_V01;
       case NAN_DEVICE:            return LOWI_NAN_DEVICE_V01;
       case STA_DEVICE:            return LOWI_STA_DEVICE_V01;
       case SOFT_AP:               return LOWI_SOFT_AP_V01;
       default:                    return LOWI_NODE_TYPE_UNKNOWN_V01;
    }
  }

  inline static eLOWIRangingReportType_v01 to_eLOWIRangingReportType_v01 (uint8 a)
  {
    switch (a)
    {
       case LOWI_RANGING_REPORT_BASIC:     return LOWI_RANGING_REPORT_BASIC_V01;
       case LOWI_RANGING_REPORT_DETAILED:  return LOWI_RANGING_REPORT_DETAILED_V01;
       case LOWI_RANGING_REPORT_AOA:       return LOWI_RANGING_REPORT_AOA_V01;
       case LOWI_RANGING_REPORT_CFR:       return LOWI_RANGING_REPORT_CFR_V01;
       default:                            return LOWI_RANGING_REPORT_BASIC_V01;
    }
  }

  inline static eLOWIRangingReportType to_eLOWIRangingReportType (uint8 a)
  {
    switch (a)
    {
       case LOWI_RANGING_REPORT_BASIC_V01:     return LOWI_RANGING_REPORT_BASIC;
       case LOWI_RANGING_REPORT_DETAILED_V01:  return LOWI_RANGING_REPORT_DETAILED;
       case LOWI_RANGING_REPORT_AOA_V01:       return LOWI_RANGING_REPORT_AOA;
       case LOWI_RANGING_REPORT_CFR_V01:       return LOWI_RANGING_REPORT_CFR;
       default:                                return LOWI_RANGING_REPORT_BASIC;
    }
  }

  inline static eRangingPreamble_v01 to_eRangingPreamble_v01 (uint8 a)
  {
    switch (a)
    {
       case RTT_PREAMBLE_LEGACY:    return RANGING_PREAMBLE_LEGACY_V01;
       case RTT_PREAMBLE_HT:        return RANGING_PREAMBLE_HT_V01;
       case RTT_PREAMBLE_VHT:       return RANGING_PREAMBLE_VHT_V01;
       default:                     return RANGING_PREEMBLE_UNKNOWN_V01;
    }
  }

  inline static eRangingPreamble to_eRangingPreamble (uint8 a)
  {
    switch (a)
    {
       case RANGING_PREAMBLE_LEGACY_V01:    return RTT_PREAMBLE_LEGACY;
       case RANGING_PREAMBLE_HT_V01:        return RTT_PREAMBLE_HT;
       case RANGING_PREAMBLE_VHT_V01:       return RTT_PREAMBLE_VHT;
       default:                             return RTT_PREAMBLE_LEGACY;
    }
  }

  inline static eRangingBandwidth_v01 to_eRangingBandwidth_v01 (uint8 a)
  {
    switch (a)
    {
       case BW_20MHZ:       return BW_20MHZ_V01;
       case BW_40MHZ:       return BW_40MHZ_V01;
       case BW_80MHZ:       return BW_80MHZ_V01;
       case BW_160MHZ:      return BW_160MHZ_V01;
       default:             return BW_UNKNOWN_V01;
    }
  }

  inline static eRangingBandwidth to_eRangingBandwidth (uint8 a)
  {
    switch (a)
    {
       case BW_20MHZ_V01:       return BW_20MHZ;
       case BW_40MHZ_V01:       return BW_40MHZ;
       case BW_80MHZ_V01:       return BW_80MHZ;
       case BW_160MHZ_V01:      return BW_160MHZ;
       default:                 return BW_20MHZ;
    }
  }

  inline static eRangingType_v01 to_eRangingType_v01 (uint8 a)
  {
    switch (a)
    {
       case SINGLE_SIDED_RANGING:     return SINGLE_SIDED_RANGING_V01;
       case TWO_SIDED_RANGING:        return TWO_SIDED_RANGING_V01;
       case BEST_EFFORT_RANGING:      return BEST_EFFORT_RANGING_V01;
       default:                       return SINGLE_SIDED_RANGING_V01;
    }
  }

  inline static eRttType to_eRttType (uint8 a)
  {
    switch (a)
    {
       case SINGLE_SIDED_RANGING_V01:       return SINGLE_SIDED_RANGING;
       case TWO_SIDED_RANGING_V01:          return TWO_SIDED_RANGING;
       case BEST_EFFORT_RANGING_V01:        return BEST_EFFORT_RANGING;
       default:                             return SINGLE_SIDED_RANGING;
    }
  }

  inline static LOWIRangingScanMeasurement::eTargetStatus to_eTargetStatus (uint8 a)
  {
    switch (a)
    {
       case LOWI_TARGET_STATUS_SUCCESS_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_SUCCESS;
       case LOWI_TARGET_STATUS_FAILURE_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAILURE;
       case LOWI_TARGET_STATUS_FAIL_NO_RSP_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_NO_RSP;
       case LOWI_TARGET_STATUS_FAIL_REJECTED_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_REJECTED;
       case LOWI_TARGET_STATUS_FAIL_FTM_TIMEOUT_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_FTM_TIMEOUT;
       case LOWI_TARGET_STATUS_TARGET_ON_DIFF_CHANNEL_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_TARGET_ON_DIFF_CHANNEL;
       case LOWI_TARGET_STATUS_FAIL_TARGET_NOT_CAPABLE_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_TARGET_NOT_CAPABLE;
       case LOWI_TARGET_STATUS_FAIL_INVALID_TS_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_INVALID_TS;
       case LOWI_TARGET_STATUS_FAIL_TARGET_BUSY_TRY_LATER_V01:
         return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAIL_TARGET_BUSY_TRY_LATER;
       default: return LOWIRangingScanMeasurement::LOWI_TARGET_STATUS_FAILURE;
    }
  }

  inline static LOWIScanMeasurement::ePeerOEM to_ePeerOEM (uint8 a)
  {
    switch (a)
    {
       case LOWI_PEER_OEM_UNKNOWN_V01:
         return LOWIScanMeasurement::LOWI_PEER_OEM_UNKNOWN;
       case LOWI_PEER_OEM_QTI_V01:
         return LOWIScanMeasurement::LOWI_PEER_OEM_QTI;
       default:
         return LOWIScanMeasurement::LOWI_PEER_OEM_UNKNOWN;
    }
  }
};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_UTILS_ENUM_H__
