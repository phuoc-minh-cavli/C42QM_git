#ifndef __PRNGEL_ENV_H__
#define __PRNGEL_ENV_H__

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW Crypto specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/prng/environment/env/amss/inc/PrngEL_Env.h#1 $
  $DateTime: 2018/06/26 03:18:17 $
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/11   nk      Added the Mutex Enter and Exit routines
7/25/10    yk     Initial version
============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "PrngCL_DALIntf.h"
#include <cache_mmu.h>
#include "err.h"

#define PRNGEL_MUTEX_TYPE rex_crit_sect_type

#define PRNGEL_MUTEX_INIT()\
        { if(prngel_mutex_init == 0) \
          { rex_init_crit_sect(&prngel_mutex); \
            prngel_mutex_init = 1; \
          }\
        }

#define PRNGEL_MUTEX_ENTER()\
        { if(prngel_mutex_init == 0) \
            ERR_FATAL("Prng mutex not initialized by RC init",0,0,0);\
          rex_enter_crit_sect(&prngel_mutex);\
        }

#define PRNGEL_MUTEX_EXIT() rex_leave_crit_sect(&prngel_mutex)

#define PRNGCL_ENABLE_CLK() PrngCL_DAL_Clock_Enable()

#define PRNGCL_DISABLE_CLK() PrngCL_DAL_Clock_Disable()

#define PRNGCL_STATUS_CLK( PTRCLKFLAG ) PrngCL_DAL_Clock_Status( PTRCLKFLAG )

#define PRNGEL_MEMORY_BARRIER() memory_barrier()


#endif /*__PRNGEL_ENV_H__ */
