
/*==============================================================================

                      TMS -> BOOT DLOAD MODE CONTROLS

  DESCRIPTION
    Internal implementation of the boot team's download magic number scheme.
    (if you want a public version talk to the boot team) 
 
    refer to "go/shared_imem" or similar for addresses and details.

  Copyright (c) 2010 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/internal/tms_dload/src/tms_dload.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/10/12     mcg     creating based on dload_entry.c
==============================================================================*/

#ifndef _TMS_DLOAD_H
#define _TMS_DLOAD_H


void tms_dload_write_magic_numbers(void);
void tms_dload_write_magic_numbers_STM(void);
void tms_dload_clear_magic_numbers(void);
void tms_dload_clear_magic_numbers_STM(void);
void tms_dload_reset(void);
void tms_dload_reset_STM(void);
boolean tms_dload_check_state(void);



#endif /* _TMS_DLOAD_H */
  

