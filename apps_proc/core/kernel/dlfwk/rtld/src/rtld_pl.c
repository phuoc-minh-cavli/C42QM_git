/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/



#include "dlfcn.h"
#include "platform_libs.h"

#include "HAP_debug.h"
#include "sigverify.h"

PL_DEP(platform_fs);
PL_DEP(sigverify)

char* builtin[] __attribute__((weak)) = {(char*)"libc.so",(char*)"libgcc.so"};

int rtld_init(void)
{
  DL_vtbl vtbl = {sizeof(DL_vtbl), HAP_debug, SigVerify_AuthenticateELF};

  if (1 != dlinitex(2, builtin, &vtbl)) {
    return 1;
  }
  return PL_INIT(platform_fs) + PL_INIT(sigverify);
}

void rtld_deinit(void)
{
  // TODO dl deinit

  PL_DEINIT(sigverify);
  PL_DEINIT(platform_fs);
  return;
}

PL_DEFINE(rtld, rtld_init, rtld_deinit);
