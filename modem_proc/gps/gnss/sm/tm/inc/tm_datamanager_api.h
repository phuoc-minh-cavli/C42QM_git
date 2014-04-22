#ifndef _TM_DATAMANAGER_API_H
#define _TM_DATAMANAGER_API_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM Data Manager API

GENERAL DESCRIPTION
  This file contains TM data manager interface to C module.

 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         EDIT HISTORY FOR MODULE

   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.

   $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/inc/tm_datamanager_api.h#1 $
   $DateTime: 2020/07/21 21:23:44 $
   $Author: pwbldsvc $

   when       who     what, where, why
   --------   ---     ----------------------------------------------------------
   01/04/17   rk      Intial version.
===========================================================================*/

#include "aries_os_api.h"
#include "gnss_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn tdm_HandleGnssMeasurements
 * @brief Interface to handle the GNSS measurement report.
 *
 * This function handles GNSS measurement report for TDM module.
 *
 * @param [in] cpz_GnssMeasRpt : Pointer to GNSS measurment report struct.
 *
 * @retval None
 */
void tdm_HandleGnssMeasurements( const sm_GpsMeasRptStructType * cpz_GnssMeasRpt );

/**
 * @fn tdm_SetSpoofingCfg
 * @brief Interface to report simulator/spoofing detection configuration
 *
 * This function process the change in simulator/spoofing detection configuration.
 *
 * @param [in] cq_ClientId : PDSM client's id which enable/disable spoofing feature
 * @param [in] cv_Status : simulator/spoofing detection is enabled or not.
 *
 * @retval None
 */
void tdm_SetSpoofingCfg( pdsm_client_id_type const cq_ClientId, boolean const cv_Status );

/**
 * @fn tdm_HandleGpsAsBitIndication
 * @brief Interface to process the GPS SV AS bit simulator detection status.
 *
 * This function process the GPS SV AS bit simulator detection status.
 *
 * @param [in] cpu_ASBitSv : AS bit indicates simulated GPS. Value 1 means AS bit was decoded to be
 *                           0 (which would indicate from GPS Simulator) in the last subframe.
 *
 * @retval None
 */
void tdm_HandleGpsAsBitIndication( uint8* const cpu_ASBitSv );

/**
 * @fn tdm_ReportGnssEngineMonitorStatus
 * @brief Interface to report the GNSS engine monitor status
 *
 * This function reports the GNSS engine monitor status to PDAPI client.
 *
 * @param [in] cpt_EngMonitorStatusRpt : GNSS engine monitor status
 *
 * @retval None
 */
void tdm_ReportGnssEngineMonitorStatus( uint64 const *const cpt_EngMonitorStatusRpt );

/**
 * @fn tdm_HandleXtraStartOfGpsData
 * @brief Interface to take the gps week number for start of gps data (first fit interval) from XTRA
 *        header.
 *
 * This function uses the gps week number for start of gps data (first fit interval) from XTRA
 * header for inconsistency check & other future purposes.
 *
 * @param [in] w_GpsWeekNum : gps week number for start of gps data (first fit interval) from XTRA
 *        header.
 *
 * @retval None
 */
void tdm_HandleXtraStartOfGpsData( uint16 const w_GpsWeekNum);

#ifdef __cplusplus
}
#endif

#endif /* _TM_DATAMANAGER_API_H */
