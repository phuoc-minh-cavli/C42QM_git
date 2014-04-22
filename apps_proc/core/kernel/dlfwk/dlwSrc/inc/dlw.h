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
#ifndef DLW_H
#define DLW_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Values for dlopen `mode'. */
#define DLW_RTLD_LAZY 1
#define DLW_RTLD_NOW 2

typedef struct {
  const char *dli_fname;	/* File defining the symbol */
  void *dli_fbase;	/* Base address */
  const char *dli_sname;	/* Symbol name */
  const void *dli_saddr;	/* Symbol address */
} dlw_info;

int dlw_Init(void);
void* dlw_Open(const char*, int);
int dlw_Close(void*);
void* dlw_Sym(void* __restrict, const char* __restrict);
int dlw_Addr(const void* __restrict, dlw_info* __restrict);
char* dlw_Error(void);

#ifdef __cplusplus
}
#endif

#endif // DLW_H
