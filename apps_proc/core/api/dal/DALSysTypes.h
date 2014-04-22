#ifndef DALSYSTYPES_H
#define DALSYSTYPES_H
/*==============================================================================
@file DALSysTypes.h


        Copyright (c) 2008,2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/api/dal/DALSysTypes.h#1 $

==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "devcfgTypes.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALReg.h"

/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/
typedef void  * DALSYSMemHandle;
typedef void  * DALSYSWorkLoopHandle;

#define DALDDI_MEM_DESC_LIST_OBJECT(varName,numBufs) \
struct {\
    uint32          dwFlags;\
    uint32          dwNumBufs;\
    DALDDIMemBufDesc   bufList[numBufs];\
} varName

//typedefs of external functions needed by Remoting
typedef DALResult
(*DALRemote_NewFncPtr)(const char *, DALDEVICEID, DalDeviceHandle **);
typedef int
(*DALRemote_CommonInitFncPtr)(void);
typedef void
(*DALRemote_IPCInitFcnPtr)(uint32);
typedef void
(*DALRemote_InitFncPtr)(void);
typedef DALSYSEventHandle
(*DALRemote_CreateEventFncPtr)(void *pClientCtxt, DALSYSEventHandle hRemote);
typedef uint32
(*DALRemoteInterProcessCallFncPtr)(void *in_buf, void *out_buf);
typedef void
(*DALRemote_DeinitFncPtr)(void);

typedef struct DALRemoteVtbl DALRemoteVtbl;
struct DALRemoteVtbl
{
   DALRemote_NewFncPtr DALRemote_NewFnc;
   DALRemote_CommonInitFncPtr DALRemote_CommonInitFnc;
   DALRemote_InitFncPtr DALRemote_InitFnc;
   DALRemote_DeinitFncPtr DALRemote_DeinitFnc;
   DALRemote_CreateEventFncPtr DALRemote_CreateEventFnc;
   DALRemote_CreateEventFncPtr DALRemote_CreatePayloadEventFnc;
   DALRemoteInterProcessCallFncPtr DALRemoteInterProcessCallFnc;
   DALRemote_IPCInitFcnPtr DALRemote_IPCInitFcn;
};

typedef struct DALSYSConfig DALSYSConfig;
struct DALSYSConfig
{
    void *pNativeEnv;
    uint32 dwConfig;
   DALRemoteVtbl *pRemoteVtbl;
};

#endif /* DALSYSTYPES_H */
