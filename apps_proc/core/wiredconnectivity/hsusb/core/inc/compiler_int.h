/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.


==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/compiler_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _COMPILER_INT_H
#define _COMPILER_INT_H
#include <jconfig.h>

#ifndef T_REXNT
#define CONFIG_ARM_COMPILER  1
#else
#define CONFIG_CL_COMPILER  1
#endif

#if defined CONFIG_GCC_COMPILER 
#include "compiler_gcc.h"
#elif defined CONFIG_ARM_COMPILER
#include "compiler_arm.h"
#elif defined CONFIG_IAR_COMPILER
#include "compiler_iar.h"
#elif defined CONFIG_CL_COMPILER
#include "compiler_cl.h"
#elif defined CONFIG_ST20_COMPILER
#include "compiler_st20.h"
#else
#error "Error - Compiler type is not defined!!!"
#endif


#ifndef __RESTRICT__
#error "Variable __RESTRICT__ was not defined"
#endif

#ifndef __VOLATILE__
#error "Variable __VOLATILE__ was not defined"
#endif

#ifndef __INLINE__
#error "Variable __INLINE__ was not defined"
#endif

#if defined(__cplusplus)
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#ifndef __PACKED_PRE__
#error "Variable __PACKED_PRE__ was not defined"
#endif
#ifndef __PACKED_POST__
#error "Variable __PACKED_POST__ was not defined"
#endif

#ifndef DECLARE_VAR_ZERO_INIT_FUNC 
#define DECLARE_VAR_ZERO_INIT_FUNC(var) 
#endif

#ifndef VAR_ZERO_INIT
#define VAR_ZERO_INIT(var)              do {} while (0)
#endif

#ifndef EXTERN_VAR_ZERO_INIT
#define EXTERN_VAR_ZERO_INIT(var)       do {} while (0)
#endif
                                      
#define UNUSED_VAR(x) (x)=(x)

#endif

