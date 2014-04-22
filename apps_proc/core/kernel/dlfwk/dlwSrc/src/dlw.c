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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "dlw.h"
#include "dlw_dlfcn.h"

// if two threads contend for the first call to dlinit assume success for
// thread that loses
static int dlw_state = 1;

int dlw_Init(void)
{
  
  dlw_state = dlinit(0, NULL);
  
  return dlw_state;
}

void* dlw_Open(const char* file, int mode)
{
  return dlopen(file, mode);
}

int dlw_Close(void* h)
{
  return dlclose(h);
}

void* dlw_Sym(void* __restrict h, const char* __restrict name)
{
  return dlsym(h, name);
}

int dlw_Addr(const void* __restrict h, dlw_info* __restrict info)
{
  return dladdr(h, (Dl_info*)info);
}

char* dlw_Error(void)
{
  const char* pszNone = "dlerror returned NULL";
  char* psz = dlerror();

  if (psz) {
    return psz;
  }
  return (char*)pszNone;
}


