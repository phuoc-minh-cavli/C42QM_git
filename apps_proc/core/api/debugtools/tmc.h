#ifndef TMC_H
#define TMC_H

/*===========================================================================

                    T A S K   M A I N   C O N T R O L L E R

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  the Task Main controller task.

Copyright (c) 2002-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/debugtools/tmc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#if defined(__GNUC__)
#pragma message "UNSUPPORTED API THIS PRODUCT LINE : REMOVE REFERENCES TO INTERFACE"
#else
#warning "UNSUPPORTED API THIS PRODUCT LINE : REMOVE REFERENCES TO INTERFACE"
//#error "UNSUPPORTED API THIS PRODUCT LINE : REMOVE REFERENCES TO INTERFACE"
#endif

#include "err.h"
#define tmc_cmd(x)            ERR_FATAL("TMC API not present", 0, 0, 0)
#define tmc_task_ack()        ERR_FATAL("TMC API not present", 0, 0, 0)
#define tmc_task_start()      ERR_FATAL("TMC API not present", 0, 0, 0)
#define tmc_task_offline()    ERR_FATAL("TMC API not present", 0, 0, 0)
#define tmc_task_stop()       ERR_FATAL("TMC API not present", 0, 0, 0)

#if defined(__cplusplus)
extern "C" {
#endif

void tmc_task(unsigned long);

// *EMPTY*

#if defined(__cplusplus)
}
#endif

#endif
