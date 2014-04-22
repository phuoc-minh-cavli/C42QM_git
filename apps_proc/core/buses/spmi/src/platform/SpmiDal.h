/*
  Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
  
  $DateTime: 2020/01/30 22:49:35 $
  $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/SpmiDal.h#1 $
  $Change: 22400605 $
  
                               Edit History
  Date      Description
  --------  -------------------------------------------------------------------
  06/27/16   rk      MSM9206: API porting from DALsys to Threadx (CR - 1034553) 
  11/10/15  Initial Version
*/

#include "DDISpmi.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "SpmiBus.h"
#include "qurt_mutex.h"


#define MAX_INTRS_PER_CLIENT 8
#define SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES 16

/*------------------------------------------------------------------------------
Declaring a "Spmi" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct SpmiDrvCtxt SpmiDrvCtxt;
typedef struct SpmiDevCtxt SpmiDevCtxt;
typedef struct SpmiClientCtxt SpmiClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "Spmi" Vtable
------------------------------------------------------------------------------*/
typedef struct SpmiDALVtbl SpmiDALVtbl;
struct SpmiDALVtbl
{
    int (*Spmi_DriverInit)(SpmiDrvCtxt *);
    int (*Spmi_DriverDeInit)(SpmiDrvCtxt *);
};

struct SpmiDevCtxt
{
    //Base Members
    uint32 dwRefs;
    uint32 dwDevCtxtRefIdx;
    SpmiDrvCtxt *pSpmiDrvCtxt;
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    const char *strDeviceName;
    uint32 Reserved[16];
    //Spmi Dev state can be added by developers here
    qurt_mutex_t * hSpmiBusDeviceSynchronization;
    //DALSYS_SYNC_OBJECT(SpmiBusDeviceSyncObject);
    qurt_mutex_t *hSpmiIntrCtlrDeviceSynchronization;
    //DALSYS_SYNC_OBJECT(SpmiIntrCtlrDeviceSyncObject);
    uint32 uDeviceInitDone;

    /* Debug registers */
    DalSpmiHandle *pSpmiHandles[SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES];
    uint32 uHandlesCurrentlyActive;

};

struct SpmiDrvCtxt
{
    //Base Members
    SpmiDALVtbl SpmiDALVtbl;
    uint32 dwNumDev;
    uint32 dwSizeDevCtxt;
    uint32 bInit;
    uint32 dwRefs;
    SpmiDevCtxt SpmiDevCtxt[1];
    //Spmi Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Spmi" Client Context
------------------------------------------------------------------------------*/
typedef struct
{
    int32 periphID;
    uint8 mask;
    SpmiToken token;
}Spmi_RegisteredIntrType;

struct SpmiClientCtxt
{
    //Base Members
    uint32 dwRefs;
    uint32 dwAccessMode;
    void *pPortCtxt;
    SpmiDevCtxt *pSpmiDevCtxt;
    DalSpmiHandle DalSpmiHandle;
    //Spmi Client state can be added by developers here
    uint32 uOpened;
    Spmi_RegisteredIntrType registeredIntrs[MAX_INTRS_PER_CLIENT];
};

