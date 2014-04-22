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
 
#ifndef SYS_MUTEX_H
#define SYS_MUTEX_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <kernel.h>

#ifdef __cplusplus
 extern "C" {
#endif

void sys_Mtxinit(void **mutex);
void sys_Mtxdst(void **mutex);
void sys_Mtxlock(void **mutex);
void sys_Mtxunlock(void **mutex);

#ifdef __cplusplus
}
#endif

#endif // DLFS_H
