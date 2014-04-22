#ifndef DDITMC_H
#define DDITMC_H

//========================================================================
//
//                             Edit History
//
//========================================================================
//             Copyright (c) 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================

#include "DalDevice.h"

#define DALTMC_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{
   TMC_ETR_WRITE_PTR,
   TMC_ETR_LATCH_BUFFER_FILL,
   TMC_ETR_BYTE_COUNTER,
   _PLACEHOLDER_TMCParamType = 0x7f
} TMCParamType;

#define   TMC_TRACESINK_ETB   0
#define   TMC_TRACESINK_RAM   1
#define   TMC_TRACESINK_TPIU  2
#define   TMC_TRACESINK_USB   3
#define   TMC_TRACESINK_USB_BUFFERED   4
#define   TMC_TRACESINK_SD    6

extern uint32 QDSS_ETR_BLOCK_SIZE;

typedef uint8 TMCTraceSinkMode;



typedef struct DalTMC DalTMC;
struct DalTMC
{
   struct DalDevice DalDevice;
   DALResult (*GetMode)(DalDeviceHandle * _h,TMCTraceSinkMode *pMode);
   DALResult (*SetMode)(DalDeviceHandle * _h,TMCTraceSinkMode mode);
   DALResult (*GetTraceBuffer)(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen);
   DALResult (*SetTraceBuffer)(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen);

   DALResult (*EnableTrace)(DalDeviceHandle * _h);
   DALResult (*DisableTrace)(DalDeviceHandle * _h);
   DALResult (*ReadTrace)(DalDeviceHandle * _h, byte * buf, uint32  nBufLen, uint32 * pnLenRead);
   DALResult (*Flush)(DalDeviceHandle * _h, int  flags);
   DALResult (*GetParam)(DalDeviceHandle * _h, TMCParamType  param, uint32  *uVal);
   DALResult (*SetParam)(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal);
   DALResult (*SaveETBRegisters)(DalDeviceHandle * _h, byte *buf, uint32 nBufLen);
   DALResult (*SaveETRRegisters)(DalDeviceHandle * _h, byte *buf, uint32 nBufLen);
};

typedef struct DalTMCHandle DalTMCHandle;
struct DalTMCHandle
{
   uint32 dwDalHandleId;
   const DalTMC * pVtbl;
   void * pClientCtxt;
};

#define DAL_TMCDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALTMC_STRING_INTERFACE_VERSION,hDalDevice)


static __inline DALResult
DalTMC_GetMode(DalDeviceHandle * _h,TMCTraceSinkMode *pMode)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetMode( _h,pMode);
}

static __inline DALResult
DalTMC_SetMode(DalDeviceHandle * _h,TMCTraceSinkMode mode)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetMode( _h, mode);
}

static __inline DALResult
DalTMC_GetTraceBuffer(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetTraceBuffer(_h,pdwAddr,pdwBufLen);
}

static __inline DALResult
DalTMC_SetTraceBuffer(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetTraceBuffer( _h,dwAddr,dwBufLen);
}



static __inline DALResult
DalTMC_EnableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->EnableTrace( _h);
}


static __inline DALResult
DalTMC_DisableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->DisableTrace( _h);
}


static __inline DALResult
DalTMC_ReadTrace(DalDeviceHandle * _h, byte * buf, uint32  nBufLen, uint32 * pnLenRead)
{
   return ((DalTMCHandle *)_h)->pVtbl->ReadTrace( _h, buf, nBufLen, pnLenRead);
}

#define TMC_FLUSH_NOBLOCK 0
#define TMC_FLUSH_BLOCK   1
#define TMC_FLUSH_STOP    2


static __inline DALResult
DalTMC_Flush(DalDeviceHandle * _h, int  flags)
{
   return ((DalTMCHandle *)_h)->pVtbl->Flush( _h, flags);
}


static __inline DALResult
DalTMC_GetParam(DalDeviceHandle * _h, TMCParamType  param, uint32  *puVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetParam( _h, param, puVal);
}


static __inline DALResult
DalTMC_SetParam(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetParam( _h, param, uVal);
}


static __inline DALResult
DalTMC_SaveETBRegisters(DalDeviceHandle * _h, byte * buf, uint32  nBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SaveETBRegisters( _h, buf, nBufLen);
}


static __inline DALResult
DalTMC_SaveETRRegisters(DalDeviceHandle * _h, byte * buf, uint32  nBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SaveETRRegisters( _h, buf, nBufLen);
}

#endif
