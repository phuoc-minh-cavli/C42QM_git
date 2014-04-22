/*======================================================================================
                                dcvs_marker.c

GENERAL DESCRIPTION
  This file contains the marker only implementation relating to DCVS.
  This file is intended to create the DCVS /core/cpu marker in order
  for other resources using resource available to be ungated.


Copyright (c) 2010 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
=========================================================================================*/
/*===========================================================================
                             Edit History

$Header: //components/rel/core.mpss/3.10/power/dcvs/src/common/dcvs_marker.c#1 $ 
$DateTime: 2018/06/26 03:18:17 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/10   bjs      Initial.
===========================================================================*/

#include "npa_resource.h"
#include "dcvs_init.h"

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/


void CoreCpuInitialize( void )
{
}


/**
  @brief DCVSInitialize

  This function is empty for the targets that dont support the 
  actual DCVS code.

  @param  : None.

  @return : None.

*/
void DCVSInitialize(void)
{
  npa_define_marker("/core/cpu");
}


