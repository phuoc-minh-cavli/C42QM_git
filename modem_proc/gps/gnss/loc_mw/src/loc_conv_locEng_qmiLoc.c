/******************************************************************************
  @file:  loc_conv_locEng_qmiLoc.c
  @brief: Handle conversions between locEng and qmiLoc structures

  DESCRIPTION
  This module translates the QMI_LOC messages to the corresponding
  loc Eng messages. loc Eng messages are IDL based and are intended to be
  used across GNSS engine.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
02/17/17   skm      BLE support for LPPE Phase 2 
06/08/16   yh       Lppe
04/06/15   sj       LB 2.0 integration 
01/30/15   sj       Fixed DOP & Sv Used Info transalation for DBT 
01/30/15   sj       Updated position transalation for DBT
03/10/14   ah       Added support for Get Avail WWAN Pos
10/22/12   ns       First revision

$Id: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_conv_locEng_qmiLoc.c#1 $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_api_internal.h"
#include "string.h"
#include "gnss_common.h"







typedef struct
{
  uint64 qmiLocMaskVal;
  uint64 locEngMaskVal;
}locEngQmiLocMaskMapStructT;




static locEngQmiLocMaskMapStructT techMaskTable[] =
{
  { (uint64_t)QMI_LOC_POS_TECH_MASK_SATELLITE_V02,
    (uint64_t)LOC_ENG_POS_TECH_MASK_SATELLITE_V01 },
  





  { (uint64)QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01 },

  { (uint64)QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01 }
};








/** convertLocEngMask
@brief : convert a loc Eng Mask to corresponding QMI LOC Mask *  
 
@param [in]: mapTable : table mapping loc Eng values to 
       qmiLoc values
@param [in]: size : size of the map table 
@param [in]: locEngMask : loc Eng Mask
@return qmiLoc Mask calue */

static uint64 convertLocEngMask(
  locEngQmiLocMaskMapStructT mapTable[],
  uint32 size,
  uint64 locEngMask)
{
  uint32 idx = 0;
  uint64 qmiLocMask = 0;
  for (idx = 0; idx < size; idx++)
  {
	if (locEngMask & mapTable[idx].locEngMaskVal)
    {
      qmiLocMask |= mapTable[idx].qmiLocMaskVal;
    }
  }

  return qmiLocMask;
}
































/*9x05: wifi not supported*/

/**   convertLocEngPosToBatchPos
 
@brief : convert loc Eng common position to QMI_LOC batched
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBatchedPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngPosToBatchedPos
(
  const locEngPositionReportMsgT_v01       *pLocEngPosition,
  qmiLocBatchedReportStructT_v02           *pBatchedPosition
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pBatchedPosition)
  {
    LOC_MSG_ERROR("convertLocEngPosToBatchedPos: NULL param error, "
                  "pLocEngPosition = %p, pBatchedPosition = %p", 
				  pLocEngPosition, pBatchedPosition, 0);
    return FALSE;
  }

  memset(pBatchedPosition, 0, sizeof(*pBatchedPosition));

  /* copy the optional TLV's */

  if( TRUE == pLocEngPosition->latitude_valid)
  {
	pBatchedPosition->latitude = 
	  pLocEngPosition->latitude;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_LATITUDE_V02;
  }

  if( TRUE == pLocEngPosition->longitude_valid)
  {
	pBatchedPosition->longitude = 
	  pLocEngPosition->longitude;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_LONGITUDE_V02;
  }

  if( TRUE == pLocEngPosition->horUncCircular_valid)
  {
	pBatchedPosition->horUncCircular = 
	  pLocEngPosition->horUncCircular;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CIR_UNC_V02;
  }

  if( TRUE == pLocEngPosition->horSpeed_valid)
  {
	pBatchedPosition->speedHorizontal = 
	  pLocEngPosition->horSpeed;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_HOR_V02;
  }

 
  if(TRUE ==  pLocEngPosition->speedUnc3d_valid)
  {
	pBatchedPosition->speedUnc = pLocEngPosition->speedUnc3d;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_UNC_V02;
  }


  if( TRUE == pLocEngPosition->altitudeWrtEllipsoid_valid)
  {
    pBatchedPosition->altitudeWrtEllipsoid = 
	  pLocEngPosition->altitudeWrtEllipsoid;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_ALT_WRT_ELP_V02;
  }

  if( TRUE == pLocEngPosition->vertSpeed_valid)
  {
	pBatchedPosition->speedVertical = 
	  pLocEngPosition->vertSpeed;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_VER_V02;
  }

  if( TRUE == pLocEngPosition->heading_valid)
  {
	pBatchedPosition->heading = pLocEngPosition->heading;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_V02;
  }
   
  if( TRUE == pLocEngPosition->headingUnc_valid)
  {
	pBatchedPosition->headingUnc = pLocEngPosition->headingUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_UNC_V02;
  }

  if( TRUE == pLocEngPosition->technologyMask_valid)
  {
	pBatchedPosition->technologyMask = convertLocEngMask(
	  techMaskTable, 
	  GNSS_ARRAY_SIZEOF(techMaskTable), 
	  pLocEngPosition->technologyMask );
	
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TECH_MASK_V02;
  }

  if( TRUE == pLocEngPosition->timestampUtc_valid)
  {
	pBatchedPosition->timestampUtc = pLocEngPosition->timestampUtc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_UTC_V02;
  }

  if( TRUE == pLocEngPosition->timeUnc_valid)
  {
	pBatchedPosition->timeUnc = pLocEngPosition->timeUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIME_UNC_V02;
  }

  if( TRUE == pLocEngPosition->magneticDeviation_valid)
  {
	pBatchedPosition->magneticDeviation = pLocEngPosition->magneticDeviation;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_MAGNETIC_DEV_V02;
  }

  if( TRUE == pLocEngPosition->vertUnc_valid)
  {
	pBatchedPosition->vertUnc = pLocEngPosition->vertUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_VERT_UNC_V02;
  }
 
  if( TRUE == pLocEngPosition->horCircularConfidence_valid)
  {
	pBatchedPosition->horConfidence = pLocEngPosition->horCircularConfidence;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CONF_V02;
  }

  if( TRUE == pLocEngPosition->gpsTime_valid)
  {
	pBatchedPosition->gpsTime.gpsTimeOfWeekMs = 
	  pLocEngPosition->gpsTime.gpsTimeOfWeekMs;
	pBatchedPosition->gpsTime.gpsWeek = pLocEngPosition->gpsTime.gpsWeek;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_GPS_V02;
  }

  return TRUE;
}

/**   convertLocEngPosToDbtPosInd
 
@brief : convert loc Eng common position to QMI_LOC DBT
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position 
@param [in]   positionType    :  Origin or Tracking Position 
@param [in]   locReqId        :  Request Id associated 
@param [out]  pDbtPositionInd :  Pointer to DBT position 
                                 Indication

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToDbtPosInd
(
  const locEngPositionReportMsgT_v01            *pLocEngPosition,
  aonPositionType                               positionType,
  uint32                                        locReqId,
  qmiLocEventDbtPositionReportIndMsgT_v02       *pDbtPositionInd
  
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pDbtPositionInd)
  {
    LOC_MSG_ERROR("convertLocEngPosToDbtPos: NULL param error, "
                  "pLocEngPosition = %p, pDbtPosition = %p", 
				  pLocEngPosition, pDbtPositionInd, 0);
    return FALSE;
  }

  memset(pDbtPositionInd, 0, sizeof(*pDbtPositionInd));

  if(eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01 == pLocEngPosition->sessionStatus)
  {
    if(pLocEngPosition->timestampUtc_valid)
	   pDbtPositionInd->dbtPosition.timestampUtc = pLocEngPosition->timestampUtc;

	if(pLocEngPosition->latitude_valid)
	   pDbtPositionInd->dbtPosition.latitude = pLocEngPosition->latitude;

	if(pLocEngPosition->longitude_valid)
	   pDbtPositionInd->dbtPosition.longitude = pLocEngPosition->longitude;

	if(pLocEngPosition->horUncEllipseSemiMinor_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseSemiMinor = 
        pLocEngPosition->horUncEllipseSemiMinor;

	if(pLocEngPosition->horUncEllipseSemiMajor_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseSemiMajor = 
        pLocEngPosition->horUncEllipseSemiMajor;

	if(pLocEngPosition->horUncEllipseOrientAzimuth_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseOrientAzimuth = 
        pLocEngPosition->horUncEllipseOrientAzimuth;

	pDbtPositionInd->dbtPosition.speedHorizontal_valid = pLocEngPosition->horSpeed_valid;
	pDbtPositionInd->dbtPosition.speedHorizontal = pLocEngPosition->horSpeed;

	pDbtPositionInd->dbtPosition.altitudeWrtEllipsoid_valid = pLocEngPosition->altitudeWrtEllipsoid_valid;
	pDbtPositionInd->dbtPosition.altitudeWrtEllipsoid = pLocEngPosition->altitudeWrtEllipsoid;

	pDbtPositionInd->dbtPosition.vertUnc_valid = pLocEngPosition->vertUnc_valid ;
	pDbtPositionInd->dbtPosition.vertUnc = pLocEngPosition->vertUnc;

	pDbtPositionInd->dbtPosition.speedVertical_valid = pLocEngPosition->vertSpeed_valid;
	pDbtPositionInd->dbtPosition.speedVertical = pLocEngPosition->vertSpeed;

	pDbtPositionInd->dbtPosition.heading_valid = pLocEngPosition->heading_valid;
	pDbtPositionInd->dbtPosition.heading = pLocEngPosition->heading;
    //Need to add the additional fields required in the indication
    pDbtPositionInd->reqId = locReqId;
	pDbtPositionInd->headingUnc_valid =  pLocEngPosition->headingUnc_valid;
	pDbtPositionInd->headingUnc = pLocEngPosition->headingUnc;
	pDbtPositionInd->speedUnc_valid = pLocEngPosition->speedUnc3d_valid;
    pDbtPositionInd->speedUnc = pLocEngPosition->speedUnc3d;	
	pDbtPositionInd->horConfidence_valid = pLocEngPosition->horEllipticalConfidence_valid;
	pDbtPositionInd->horConfidence = pLocEngPosition->horEllipticalConfidence;
	pDbtPositionInd->vertConfidence_valid = pLocEngPosition->vertConfidence_valid;
	pDbtPositionInd->vertConfidence = pLocEngPosition->vertConfidence;
    pDbtPositionInd->positionType = (AON_POSITION_TYPE_TRACKING == positionType)?
                                        eQMI_LOC_DBT_POSITION_TYPE_TRACKING_V02: 
                                        eQMI_LOC_DBT_POSITION_TYPE_ORIGIN_V02;

    pDbtPositionInd->DOP_valid = pLocEngPosition->DOP_valid;
	pDbtPositionInd->DOP.PDOP = pLocEngPosition->DOP.PDOP;
	pDbtPositionInd->DOP.HDOP = pLocEngPosition->DOP.HDOP;
	pDbtPositionInd->DOP.VDOP = pLocEngPosition->DOP.VDOP;
	pDbtPositionInd->gnssSvUsedList_valid = pLocEngPosition->gnssSvUsedList_valid;
	pDbtPositionInd->gnssSvUsedList_len = pLocEngPosition->gnssSvUsedList_len;
	/* SV  used info */
    memscpy(&pDbtPositionInd->gnssSvUsedList, 
            sizeof(pDbtPositionInd->gnssSvUsedList),
		    &pLocEngPosition->gnssSvUsedList, sizeof(pLocEngPosition->gnssSvUsedList));
    if(pLocEngPosition->technologyMask_valid)
    {
        pDbtPositionInd->positionSrc_valid =  pLocEngPosition->technologyMask_valid;

        if (pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_SATELLITE_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_GNSS_V02;
        }

        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_WIFI_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_WIFI_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_SENSORS_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_OTHER_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_CELLID_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02;
        }
     }
    return TRUE;
  } 
  else 
  {
	
	LOC_MSG_ERROR("convertLocEngPosToDbtPos: Incorrect session status, "
                  "sessionStatus = %d", 
				  pLocEngPosition->sessionStatus, 0, 0);
    return FALSE;
  }
}







