/******************************************************************************
  @file:  loc_conv_locEng_qmiLoc.h
  @brief: Function headers for translation between locEng and qmiLoc

  DESCRIPTION
  This module defines the functions that handle the conversions between locEng
  and qmiLoc messages. locEng messages are IDL based and intended to be used
  within the GNSS engine.

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
01/30/15   sj       Updated position transalation for DBT 
03/10/14   ah       Added support for Get Avail WWAN Pos
10/22/12   ns       First revision

$Id: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_conv_locEng_qmiLoc.h#1 $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/
#ifndef LOC_CONV_LOC_ENG_QMI_LOC_H
#define LOC_CONV_LOC_ENG_QMI_LOC_H

#include "comdef.h" 
#include "location_service_iot_v02.h"
#include "locEng_sm_common_msg_v01.h"
#include "aon_api.h"
#include "loc_api_2.h"
















/*9x05: Wifi not supported*/


/**   convertLocEngPosToBatchPos
 
@brief : convert loc Eng common position to QMI_LOC batched
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBatchedPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToBatchedPos
(
  const locEngPositionReportMsgT_v01       *pLocEngPosition,
  qmiLocBatchedReportStructT_v02           *pBatchedPosition
);

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
  
);




#endif //LOC_CONV_LOC_ENG_QMI_LOC_H
