/*!
  @file
  ftm_common_command_interface.c

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rffactory.mpss/1.0/src/ftm_common_command_interface.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
08/24/17   zhw     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rflm_defs.h"
#include "ftm_command_id.h"

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TRM_ASSIGNED_RADIO_ALLOCATIONS
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! List of property names to be incremented as list of ARA property grows.*/
/* based on ftm_trm_ara_property_type */
const char *ftm_trm_ara_property_names_v2[] = 
{
  "UNASSIGNED",    
  "CARRIER_IDX",
  "BAND",
  "RXTX",
  "CHAIN",
  "DEVICE",
  "SIGNAL_PATH",
  "ANTENNA_PATH",
  "PLL_ID"
};

/* A COMPILE ERROR HERE MEANS fix the above array as it must have same number of elements as struct it represents. */
COMPILE_ASSERT( ARR_SIZE(ftm_trm_ara_property_names_v2) == FTM_TRM_ARA_PROP_NUM_V2 );

/*! @} */
