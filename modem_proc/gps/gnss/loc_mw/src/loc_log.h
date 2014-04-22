#ifndef LOC_LOG_H
#define LOC_LOG_H
/******************************************************************************
  @file:  loc_log.h
  @brief: Location Middleware DM log module

  DESCRIPTION
  
  
  -----------------------------------------------------------------------------
  Copyright (c) 2013-2014 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential
  -----------------------------------------------------------------------------
 ******************************************************************************/


/*=============================================================================
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_log.h#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $
=============================================================================*/

#include "log.h"
#include "log_codes.h"

#if defined(T_WINNT)
#error code not present
#endif

#ifdef FEATURE_GEOFENCE_IOD_SENSOR_SUPPORT
#include "sns_sam_iod_v01.h"

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8        u_pkt_version;
  uint32       q_als_iod_utc_timestamp;
  uint8        u_is_async_resp;
  uint16       w_als_iod_instance_id;
  uint8        u_als_iod_sensor_resp;
  uint8        u_als_iod_sensor_output_class;
  uint8        u_als_iod_unknown_reason;
  float        f_als_iod_output_confidence;
} loc_mw_log_als_iod_resp;

#if defined(T_WINNT)
#error code not present
#endif

extern void loc_mw_log_als_iod_ind
(
  sns_sam_iod_report_ind_msg_v01 *p_als_iod_qmi_ind
);

#endif /*FEATURE_GEOFENCE_IOD_SENSOR_SUPPORT*/
#endif /* LOC_LOG_H */
