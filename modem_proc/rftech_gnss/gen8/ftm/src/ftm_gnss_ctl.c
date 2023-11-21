 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                            FTM  GNSS CTL

GENERAL DESCRIPTION
  This is the FTM GNSS Control file which contains RF specific commands
  handled exclusively in Factory Test Mode(FTM)

Copyright (c) 2009 - 2019     by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rftech_gnss.mpss/2.10/gen8/ftm/src/ftm_gnss_ctl.c#1 $ 
  $DateTime: 2020/04/16 12:19:05 $ 
  $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/19   up      Removed debug F3s
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
11/16/09   dyc     Featurize for mode specific builds.
10/01/09   hp      Created. Added support for External LNA

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "rf_test_task_v.h"
#ifdef FEATURE_CDMA1X
#include "ftm_cmd.h"
#endif /* FEATURE_CDMA1X */
#include "ftm_gnss_ctl.h"
#include "ftm_common.h"
#include "ftm.h"
#include "rfm_gnss.h"




/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/




/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/




/*===========================================================================

                                 MACROS

===========================================================================*/




/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/




/*===========================================================================

FUNCTION FTM_SET_GNSS_EXTERNAL_LNA

DESCRIPTION
   This function will set the External LNA ON or OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_set_gnss_external_lna(boolean on_off) 
{
  //rfm_gnss_has_ext_lna (on_off); RB: removing as it is does not seem relevant
} /* End ftm_set_gnss_external_lna */

#endif /* FEATURE_FACTORY_TESTMODE */

