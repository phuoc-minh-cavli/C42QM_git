/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager Utility Module

GENERAL DESCRIPTION
  This module has SM translation function to convert internal nav solution
  structures to gps/gnss structures.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/sm_gal.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/14   jv      Compile time Constellation configurability


============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "sm_api.h"
#include "sm_util.h"


sm_ConstellationCfgType sm_set_constellation_config_gal()
{
    return SM_CCFG_CT_GAL;
}

