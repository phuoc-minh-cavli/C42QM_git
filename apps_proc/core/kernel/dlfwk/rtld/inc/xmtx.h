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
/* xmtx.h internal header */
#ifndef _XMTX
#define _XMTX
 #include <yvals.h>

#include <stdlib.h>

typedef void *_Rmtx;

   extern void sys_Mtxinit(_Rmtx *_Mtx);
   extern void sys_Mtxdst(_Rmtx *_Mtx);
   extern void sys_Mtxlock(_Rmtx *_Mtx);
   extern void sys_Mtxunlock(_Rmtx *_Mtx);

   #define _Mtxinit(Mtx)    sys_Mtxinit(Mtx)
   #define _Mtxdst(Mtx)     sys_Mtxdst(Mtx)
   #define _Mtxlock(Mtx)    sys_Mtxlock(Mtx)
   #define _Mtxunlock(Mtx)  sys_Mtxunlock(Mtx)

#endif /* _XMTX */

/*
 * Copyright (c) 1992-2006 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.01:1566 */
