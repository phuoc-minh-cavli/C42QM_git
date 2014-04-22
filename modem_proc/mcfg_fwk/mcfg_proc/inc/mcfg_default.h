/*===========================================================================
                        mcfg_default.h

DESCRIPTION

   Internal header file for modem_default
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/inc/mcfg_default.h#1 $ $DateTime: 2020/02/19 23:07:19 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
02/01/13   cy     Initial Creation.

===========================================================================*/

#ifndef MCFG_DEFAULT_H
#define MCFG_DEFAULT_H

#include "mcfg_common.h"


/* Function prototypes*/
boolean mcfg_default_config_enabled
(
   uint8  type
);

boolean mcfg_default_config_force_load
(
   uint8  type,
   boolean is_default,
   uint8  subs_id
);

uint32 mcfg_default_config_process
( 
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type subs_id
);
#endif /* MCFG_DEFAULT_H */
