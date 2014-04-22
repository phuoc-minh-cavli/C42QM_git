#pragma once
/*==============================================================================

        MODEM_CFG  -     Baseband_config_proc.h

GENERAL DESCRIPTION

Copyright (c) 2014-2015 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/baseband_config/inc/baseband_config_proc.h#1 $
$DateTime: 2020/11/16 21:23:19 $ 
$Author: pwbldsvc $
$Change: 27281151 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
02/24/15 sbt   Initial creation

==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION  baseband_config_init() 

DESCRIPTION
  This Functions reads the profile configured from EFS and updates it to the global cache.
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

PARAMETERS
  None
RETURN VALUE
  None
 
==============================================================================*/
void baseband_config_init
(
 void
);
