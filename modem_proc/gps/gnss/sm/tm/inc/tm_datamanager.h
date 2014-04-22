#ifndef _TM_DATAMANAGER_H
#define _TM_DATAMANAGER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM Data Manager

GENERAL DESCRIPTION
  This file contains TM data manager classes used to store the data
  subscribed through PDAPI events & perform specific operations on the
  received data from engine. The operations could be of simple store and
  report, report at specific time interval etc.

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

   $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/inc/tm_datamanager.h#1 $
   $DateTime: 2020/07/21 21:23:44 $
   $Author: pwbldsvc $

   when       who     what, where, why
   --------   ---     ----------------------------------------------------------
   01/04/17   rk      Intial version.
===========================================================================*/

/* MPSS/OS header files*/
#include "msg.h"
/* IZAT header files */
#include "pdapi_lite.h"
#include "tm_data.h"
#include "tm_pdapi_client.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef uint8 GpsSvAsBitType;
typedef uint16 GpsWeekType;

typedef struct {
  GpsWeekType w_RomAlmGW;
  GpsWeekType w_BuildGW;
  GpsWeekType w_XtraGW;
} tdm_SimulatorDetectionType;


/*----------------------------------------------------------------------------
 * Class Declarations
 * -------------------------------------------------------------------------*/

/**
 * @class TMDataManager
 * @brief Abstract template class used by all the strategy sub-classes
 *
 * This class define the common interface needed in all strategy sub-classes to set & process any
 * type of data. The data are categorized into analyze data, benchmark data (used for comparison
 * purpose) and transit (forward) data which are reported to the client without any processing.
 *
 */
template<typename AnalyzeType, typename BenchMarkType, typename TransitType = bool>
class TMDataManager
{
public:
  /**
  * @brief TMDataManager C'tor.
  */
  TMDataManager( pdsm_client_id_type const ce_ClientId):
    me_ClientId(ce_ClientId)
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, " CId %d",  me_ClientId, 0 );
  }

  /**
  * @brief TMDataManager C'tor.
  */
  TMDataManager(  pdsm_client_id_type const ce_ClientId,
               BenchMarkType& r_BenchData ) :
     me_ClientId( ce_ClientId ), m_BenchMarkData( r_BenchData )
  {
    MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_MED, " CId %d GPSWeek Rom %u Build %u Xtra %u", 
           me_ClientId, m_BenchMarkData.w_RomAlmGW, m_BenchMarkData.w_BuildGW, m_BenchMarkData.w_XtraGW, 0 );
  }

  /**
  * @brief TMDataManager D'tor.
  */
  virtual ~TMDataManager( void )
  {
  }

  /**
  * @brief This function takes the data that need to be analyze & process against the benchmark
  *        data.
  *
  * @param[in] Reference to the analyze data
  */

  /**
  * @brief Copy the passed benchmark data into internal member. Use this function to set the whole
  *        benchmark data.
  *
  * @param[in] Reference to the benchmark data
  */
  virtual void SetBenchmarkData( BenchMarkType const & ) = 0;

  /**
  * @brief The data need to be forwarded to the client without any comparison or processing the
  *        data. The derived class will implement this function as need based.
  *
  * @param[in] r_TransitData Reference to transit data
  */
  virtual void SetTransitData( TransitType const& )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "base can't handle transit data");
  }

  /**
  * @brief Use this function to get the whole benchmark data. This can also be used to set part of
  *        the benchmark data structure.
  *
  */
  BenchMarkType& PointBenchmarkData( void )
  {
    return ( m_BenchMarkData );
  }

protected:

  /**
  * @brief The derived class should implement this function to report combined final data as PDSM
  *        extended status report to the PDAPI client (LocMw etc.).
  *
  * @param[in] pz_pd_ext_status_info Pointer to PDSM extended status info struct
  */

  /**
  * @brief Report the combined final data to PDAPI client (LocMw etc.) in the form of PDSM extended
  *        event status. The final data could be combination of benchmark and/or transit data. The
  *        PDSM memory allocation & deallocation is handled by this function.
  *
  * @retval TRUE If the data was reported to PDAPI client successfully.
  * @retval FALSE If the data was not able to report.
  */


public:
  /* Pointer to benchmark data */
  BenchMarkType m_BenchMarkData;

protected:
  /* PDAPI client id which has requested this data */
  pdsm_client_id_type me_ClientId;
  /* PDAPI extended event id. Used while reporting the data */

};

/**
 * @class TDMReportOnAnalyzeData
 * @brief Concrete derived-class to report the information upon receiving each analysis data.
 *
 * This class provies specialized constructor to take initial one-time data needs for analysis.
 * Then, the data is analyzed against the benchmark data for reporting.
 */
template<typename AnalyzeDType, typename BenchmarkDType, typename TransitDType = bool>
class TDMReportOnAnalyzeData : virtual public TMDataManager<AnalyzeDType, BenchmarkDType, TransitDType>
{
public:

  /**
  * @brief TDMReportOnAnalyzeData C'tor : Specialized constructor to take the data for analysis.
  *        In this case GpsWeekType is the analysis data type.
  */
  TDMReportOnAnalyzeData( BenchmarkDType& rz_BenchData, 
                         pdsm_client_id_type const ce_ClientId ) :
  TMDataManager< BenchmarkDType, TransitDType>(  ce_ClientId, rz_BenchData ),
  m_AnyzData(0), m_TransitData(0)
  {
  }

  /**
  * @brief TDMReportOnAnalyzeData D'tor.
  */
  ~TDMReportOnAnalyzeData( void )
  {
  }

  /**
  * @brief This function takes the data that need to be analyze & process against the benchmark
  *        data.
  *
  * @param[in] Reference to the analyze data
  */


  /**
  * @brief Inherited functions. See the parent class for details.
  */
  void SetBenchmarkData( BenchmarkDType const &rc_BenchData )
  {
    this->m_BenchMarkData = rc_BenchData;
  }
  /**
  * @brief Inherited functions. See the parent class for details.
  */
  void SetTransitData( TransitDType const &rc_GpsSvASBitIndSim )
  {
    m_TransitData = rc_GpsSvASBitIndSim;
  }

protected:
  /**
  * @brief Inherited functions. See the parent class for details.
  */

private:
  /* AS bit was decoded to be 0 in the last subframe for any GPS SV */
  TransitDType m_TransitData;
  AnalyzeDType m_AnyzData;
};



/**
 * @class TDMSpoofDetection
 * @brief Context class to handle simulation/spoof detection.
 *
 * This context class uses appropiate strategy for handling the spoof detection data.
 */
class TDMSpoofDetection
{
public:
  typedef TDMReportOnAnalyzeData<GpsWeekType, tdm_SimulatorDetectionType, GpsSvAsBitType> TDMGpsWeekBenchmarkStrtType;
  typedef TMDataManager<GpsWeekType, tdm_SimulatorDetectionType, GpsSvAsBitType> TDMGpsWeekInterface;

  /**
  * @brief TDMSpoofDetection C'tor.
  */
  TDMSpoofDetection(pdsm_client_id_type const cq_ClientId, bool const v_SpoofCfg);

  /**
  * @brief TDMSpoofDetection D'tor.
  */
  ~TDMSpoofDetection();

  /**
  * @fn HandleGnssMeasurements
  * @brief Handle GNSS measurements from the GNSS engine
  *
  * @param crz_GnssMeasRpt : Reference to GNSS measurement report
  */
  void HandleGnssMeasurements( sm_GpsMeasRptStructType const &rz_GnssMeasRpt );

  /**
  * @fn HandleGpsSvASBit
  * @brief Handle GPS SV AS bit value from the SV decoding.
  *
  * @param  cu_ASBitSv : AS bit indicates GPS simulator or not. Value 1 means AS bit was decoded to
  *                    be 0 (which would indicate from GPS Simulator) in the last subframe.
  */
  void HandleGpsSvASBit( GpsSvAsBitType const cu_ASBitSv );

  /**
  * @fn HandleXtraGpsDataStartTime
  * @brief Handle gps week number for start of gps data (first fit interval) from XTRA header.
  *
  * @param  w_GpsWeekNum : gps week number for start of gps data (first fit interval) from XTRA header
  */
  void HandleXtraGpsDataStartTime( GpsWeekType const& w_GpsWeekNum );

private:

  /**
  * @fn FindOldGpsWeekFromRomGpsAlm
  * @brief Find the oldest GPS week number from the GPS ROM Almanac SVs
  *
  * @retval If successful return the oldest GPS week number of GPS ROM Almanac SVs. Otherwise
  *         unknown (i.e. C_GPS_WEEK_UNKNOWN)
  */
  uint16 FindOldGpsWeekFromRomGpsAlm( void );

  /**
  * @fn ConvertBuildTime2GpsWeek
  * @brief Convert build compile time into GPS week number
  *
  * @retval If successful return the GPS week number of build compile time. Otherwise unknown (i.e.
  *         C_GPS_WEEK_UNKNOWN)
  */
  uint16 ConvertBuildTime2GpsWeek( void );

private:
  /* Pointer the strategy class */
  TDMGpsWeekInterface *mpc_AnalyzeGnssMeasStrt;
  /* Pointer to modem heap storage for mpc_AnalyzeGnssMeasStrt object */
  void* mp_StorageAnalyzeGnssMeasStrt;
};

/*----------------------------------------------------------------------------
 * Template Definition
 * -------------------------------------------------------------------------*/

/**
* @brief Report the combined final data to PDAPI client (LocMw etc.) in the form of PDSM extended
*        event status. The final data could be combination of benchmark and/or transit data. The
*        PDSM memory allocation & deallocation is handled by this function.
*
* @retval TRUE If the data was reported to PDAPI client successfully.
* @retval FALSE If the data was not able to report.
*/


#endif
