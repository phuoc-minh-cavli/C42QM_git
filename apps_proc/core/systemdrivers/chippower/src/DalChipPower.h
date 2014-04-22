/*============================================================================

FILE: DalChipPower.h

DESCRIPTION:
  Function and data structure declarations.

PUBLIC CLASSES:

===============================================================================
Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.tx/6.0/systemdrivers/chippower/src/DalChipPower.h#1 $
=============================================================================*/

#ifndef DALCHIPPOWER_H
#define DALCHIPPOWER_H

#include "DALFramework.h"
#include "DalDevice.h"
#include "DDIChipPower.h"
#include "string.h" //for memset warning

#include "DALDeviceId.h"
#include "DALQueue.h"
#include "qurt.h"
#include <stdbool.h>

/*------------------------------------------------------------------------------
Declaring a "ChipPower" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct ChipPowerDrvCtxt ChipPowerDrvCtxt;
typedef struct ChipPowerDevCtxt ChipPowerDevCtxt;
typedef struct ChipPowerClientCtxt ChipPowerClientCtxt;
typedef struct ChipPowerDescNodeType ChipPowerDescNodeType;

struct ChipPowerDescNodeType
{
   ChipPowerDescNodeType *pNext;
   ChipPowerEventObj   *pEventObj;
};

/*------------------------------------------------------------------------------
Declaring a private "ChipPower" Vtable
------------------------------------------------------------------------------*/
typedef struct ChipPowerDALVtbl ChipPowerDALVtbl;
struct ChipPowerDALVtbl
{
   int (*ChipPower_DriverInit)(ChipPowerDrvCtxt *);
   int (*ChipPower_DriverDeInit)(ChipPowerDrvCtxt *);
};

struct ChipPowerDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   ChipPowerDrvCtxt  *pChipPowerDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //ChipPower Dev state can be added by developers here
};

struct ChipPowerDrvCtxt
{
   //Base Members
   ChipPowerDALVtbl ChipPowerDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   ChipPowerDevCtxt ChipPowerDevCtxt[1];
   //ChipPower Drv state can be added by developers here
   bool             bMaster;
   uint32           nPSHold;
   ChipPowerDescNodeType*     eventQueue;
   qurt_mutex_t*     syncObj;
};

/*------------------------------------------------------------------------------
Declaring a "ChipPower" Client Context
------------------------------------------------------------------------------*/
struct ChipPowerClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   ChipPowerDevCtxt *pChipPowerDevCtxt;
   DalChipPowerHandle DalChipPowerHandle;
   //ChipPower Client state can be added by developers here
};


/*==============================================================================

                            FUNCTION PROTOTYPES

==============================================================================*/

DALResult
ChipPower_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);

void ChipPower_Reset( DalDeviceHandle * h,  uint32  nNotUsed);
void ChipPower_PowerOff( DalDeviceHandle * h,  uint32  nNotUsed);
void ChipPower_PrepareForPowerEvent (void);
void ChipPower_PerformCallbacks (ChipPowerDrvCtxt *pDrvCtxt,
                                 ChipPowerEventType event);
void ChipPowerQueueNode(ChipPowerDescNodeType **ppList, ChipPowerDescNodeType *pNode);
ChipPowerDescNodeType* ChipPowerDequeueNode(ChipPowerDescNodeType **ppList);


#endif

