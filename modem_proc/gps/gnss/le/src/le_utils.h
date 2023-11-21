/*============================================================================
  FILE:         le_utils.h

  OVERVIEW:     Location Estimation utility functions like initializing 
                structs. 

  DEPENDENCIES: NA
 
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/le/src/le_utils.h#1 $ 
   $DateTime: 2020/07/21 21:23:44 $ 
   $Author: pwbldsvc $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-02  wtr  new file for LE utilities

============================================================================*/

#ifndef _LE_UTILS_H
#define _LE_UTILS_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h" /* for boolean */
#include "gnss_common.h" /* for gnss_ExternCoarsePosInjectionType */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*============================================================================

  FUNCTION      le_InitExternCoarsePosInjection()

  DESCRIPTION   Safely initialize a gnss_ExternCoarsePosInjection struct

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    pz_Ecpi - incoming struct to initialize

  RETURN VALUE  TRUE if successfully initalized

  SIDE EFFECTS  None

============================================================================*/
boolean le_InitExternCoarsePosInjection(
    gnss_ExternCoarsePosInjectionType *pz_Ecpi);


#endif  /* _LE_UTILS_H */
