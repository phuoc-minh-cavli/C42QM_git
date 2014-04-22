
/* Target: GENERIC*/

#ifndef DAL_CONFIG_H
#define DAL_CONFIG_H


#include "DALPropDef.h"

#define DAL_BUILD_MULTI_PROC_IMAGE

   #include "customer.h"
   //assume, for now, rvct22 compiler directiver for align
   #define DAL_ALIGN(bits) __align(bits)

#define DAL_CONFIG_ARCH    DAL_CONFIG_ARCH_ARM_V7


/* define the OS env for apps processor */

#define DAL_CONFIG_HLOS_ENV DALPROP_HLOS_ENV_BREW

#define DALGLBCTXT_SPINLOCK_PID  DAL_CONFIG_PROC_APSS

#endif /* DAL_CONFIG_H */

