/*==============================================================================
  FILE:         le_utils_nf_trans.h

  OVERVIEW:     Location Estimation utility functions like initializing 
                structs. 
                To be merged into /le/src/le_utils.h after the transition from
                MGP/PE/NF -> LE is completed.

  DEPENDENCIES: NA
 
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/


/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/le/inc/le_utils_nf_trans.h#1 $ 
   $DateTime: 2020/07/21 21:23:44 $ 
   $Author: pwbldsvc $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-05-01  wtr  new file for LE utilities - that need a transitional 
                   location

==============================================================================*/

#ifndef _LE_UTILS_NF_TRANS_H
#define _LE_UTILS_NF_TRANS_H

/*------------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------*/

#include "com_dtypes.h" // for int32
#include "le_types.h" // for le_WifiCovLocType

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Maximum uncertainty of the inter-GNSS time bias */
#define LE_MAX_INTER_GNSS_TB_UNC_METERS      ((FLT)(2.0 * LIGHT_MSEC))

/* Maximum Heading uncertainty in radians, assuming uniform distribution of heading 
   MaxHeadingUncRad = 360(deg)/sqrt(12) * pi/180  (rad)
   MaxHeadingUncDeg = 103.923048454133            (deg)*/
#define LE_MAX_HEADING_UNC_RAD            1.81379936423422 /* radians */

/*------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/*==============================================================================

  FUNCTION      le_FloatToS32RangedAndScaled()

  DESCRIPTION   Safely converts floats to int32's for logging

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    f_ConvertMe - Value to convert
    f_MaxValue  - Max value (+/- f_MaxValue)
    f_Scale     - Scalar upon convertion (e.g. 1000.0F to get
                  abc.def reported as abcdef)

  RETURN VALUE  The converted int32

  SIDE EFFECTS  None

==============================================================================*/
int32 le_FloatToS32RangedAndScaled(const FLT f_ConvertMe, 
                                   const FLT f_MaxValue, 
                                   const FLT f_Scale);

/*=============================================================================

  FUNCTION      le_InitGnssNavSolution()

  DESCRIPTION   Initialize a GNSS nav solution structure.

  DEPENDENCIES  None

  PARAMETERS    Description of the parameters to the functions

    pz_NavSolution - Pointer to the structure to be initialized

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void le_InitGnssNavSolution( gnss_NavSolutionStructType* pz_NavSolution );

/*=============================================================================

  FUNCTION      le_WifiCovLocToNavSolution

  DESCRIPTION   Use the contents of a Wifi Coverage Location structure to 
                populate a nav solution structure.

                Note that this does not do the Wifi HEPE check, that must be
                done separately before use in ALE - see ale_CoarsePosUncProcess

  DEPENDENCIES  None

  PARAMETERS    pz_NavSolution: Nav solution structure to be populated
                pz_WifiCovLoc: Input wifi cov location structure

  RETURN VALUE  None

  SIDE EFFECTS  None

==============================================================================*/
void le_WifiCovLocToNavSolution( gnss_NavSolutionStructType* pz_NavSolution,
                                 const le_WifiCovLocType* pz_WifiCovLoc );

/*=============================================================================
 FUNCTION      le_IsSkepticalOfChange

 DESCRIPTION   Compare to previous fix of same src, and block if there's an
                unexpectedly large jump - with convincible skeptic logic to
                ensure we get out of a hole (e.g. if it is initialed wrong,
                we can 'jump' to the right location.)

                If there is no previous fix to check against, or
                if the check passes, or
                if the check has failed 3 times a row 
                - this returns TRUE, so that the fix can go to ALE, and we
                  don't get stuck in a hole

                Only if there is a previous fix to check against, and 
                it mismatches, and
                it's the 1st or 2nd mistmatch in a row,
                - then we return FALSE, to block a transient jump in the
                  location from going to ALE.

  DEPENDENCIES  None

  PARAMETERS    Description of the parameters to the functions

    pz_NewPos   - New position whose acceptance needs to be checked.
    pz_OldPos   - Old position against whom acceptance needs to be checked
    pu_cIntraSrcConsistencyFailCnt  - Pointer to inconsistency count

  RETURN VALUE  True  If the new position is acceptable
                False if input pos is not acceptable

  SIDE EFFECTS 
==============================================================================*/
boolean le_IsSkepticalOfChange(
  const gnss_NavSolutionStructType *pz_NewPos,
  const gnss_NavSolutionStructType *pz_OldPos,
  uint8 *pu_cIntraSrcConsistencyFailCnt);

#endif  /* _LE_UTILS_NF_TRANS_H */
