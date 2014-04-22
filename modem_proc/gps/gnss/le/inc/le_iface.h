/*============================================================================
  FILE:         le_iface.h

  OVERVIEW:     Input API structure definitions and function 
                prototypes for LE  module, the place where NF task is 
                migrating.

  DEPENDENCIES: NA
 
                Copyright (c) 2015 - 2016 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/le/inc/le_iface.h#1 $ 
   $DateTime: 2020/07/21 21:23:44 $ 
   $Author: pwbldsvc $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-16  wtr  new file for LE module (transition from nf_iface.h)

============================================================================*/

#ifndef _LE_IFACE_H
#define _LE_IFACE_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h" // for boolean
#include "le_types.h" // for structs 
#include "gnss_common.h" // for gnss_PdrCtrlSrcEnumType
#include "tdp_common.h" /* for tdp_MeasBlkStructType */

#ifdef __cplusplus
extern "C"
{
#endif 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/*======================================================================

  FUNCTION 		  le_WifiCovMeasAdPut()

  DESCRIPTION 	 Send to LE that a new WiFi Measurement & Assistance 
                 Data package for consumption to update location 
                 estimation, and send out (if reqeusted.)

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS      Description of the parameters to the functions

    pz_WifiCovFix - data to pack into IPC to send

  RETURN VALUE   True if input message was successfully enqueued

  SIDE EFFECTS   None

======================================================================*/
boolean le_WifiCovMeasAdPut(const le_WifiCovFixRequestType *pz_WifiCovFix);

/*==============================================================================

  FUNCTION      le_InjectTlePosition

  DESCRIPTION 	Inject TLE position to ALE (via MGP, & PE to NF for now)

  DEPENDENCIES  None

  PARAMETERS    pz_TlePos: input, pointer to a TLE position

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void le_InjectTlePosition( const ale_CellDBInputPosStructType *pz_TlePos );

/*==============================================================================

  FUNCTION      le_InjectPdrControl

  DESCRIPTION 	Inject PDR control to NF(via MGP & PE per modularity req's)

  DEPENDENCIES  None

  PARAMETERS    
    b_PdrDisableFlag: PDR control config, TRUE if feature is 
                          disabled, FALSE otherwise
    e_PdrCtrlSrc: Source of the config, build-config, QMI or NV

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
boolean le_InjectPdrControl
(
  boolean b_PdrDisableFlag,
  gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc
);


/*
 ******************************************************************************
 * le_TdpMeasBlkPut
 *
 * Function description:
 *
 *    This function is called by TLE to send TDP measurements to the 
 *    Position Engine.
 *    
 * Parameters: 
 *
 *  pz_TdpMeasBlk -  TDP measurement block structure
 *
 * Return value: 
 *
 *  void
 *
 * ******************************************************************************
*/
void le_TdpMeasBlkPut( const tdp_MeasBlkStructType *pz_TdpMeasBlk );

/*
 ******************************************************************************
 * le_TrackPosConsistencyStartStop
 *
 * Function description:
 *
 *    This function is called by SM to send PosConsistencyCheck start/stop
 *    msg to NF.
 *    
 * Parameters: 
 *
 *  v_PosConsCheckStatus -  Set to TRUE, to indicate to LE to track pos 
 *                          consistency and report inconsistency to SM upon
 *                          request for DBH position, else NOT.
 *
 * Return value: 
 *
 *  void
 *
 * ******************************************************************************
*/
void le_TrackPosConsistencyStartStop(boolean v_PosConsCheckStatus);

#ifdef __cplusplus
}
#endif 

#endif  /* _LE_IFACE_H */
