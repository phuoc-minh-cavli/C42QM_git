/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
           All Rights Reserved.

=============================================================================*/

#include "halqdss_etfetb.h"
#include "halqdss_etr.h"
#include "halqdss_csr.h"
#include "qdss_csr_config.h"
#include "halqdss_replicator.h"
#include "qdss_replicator_config.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITMC.h"
#include "qdss_tmc_config.h"
#include "qdss_throw.h"
#include "HALhwio.h"

#include "qurt_memory.h"
#include "qurt_error.h"

extern uint32 QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE;
extern uint32 QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE;

/*------------------------------------------------------------------------------
Declaring a "TMC" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TMCDrvCtxt TMCDrvCtxt;
typedef struct TMCDevCtxt TMCDevCtxt;
typedef struct TMCClientCtxt TMCClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "TMC" Vtable
------------------------------------------------------------------------------*/
typedef struct TMCDALVtbl TMCDALVtbl;
struct TMCDALVtbl
{
   int (*TMC_DriverInit)(TMCDrvCtxt *);
   int (*TMC_DriverDeInit)(TMCDrvCtxt *);
};

struct TMCDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TMCDrvCtxt  *pTMCDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TMC Dev state can be added by developers here
   DALSYSMemHandle hMemETFETB;
   DALSYSMemHandle hMemETR;
   DALSYSMemHandle hMemCSR;
   DALSYSMemHandle hMemReplicator;
   TMCTraceSinkMode mode;
   uint32  etrByteCounter;
};

struct TMCDrvCtxt
{
   //Base Members
   TMCDALVtbl TMCDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TMCDevCtxt TMCDevCtxt[1];
   //TMC Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TMC" Client Context
------------------------------------------------------------------------------*/
struct TMCClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TMCDevCtxt *pTMCDevCtxt;
   DalTMCHandle DalTMCHandle;
   //TMC Client state can be added by developers here
};

#define TMC_PAGE_SIZE 4096

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TMC_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALTMC_DriverInfo = { TMC_DeviceAttach, 0, NULL};


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

static uint32
TMC_AddRef(DalTMCHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TMC_DriverInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TMC_DriverDeInit(TMCDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TMC_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult TMC_MemMap(uint32 physAddr,
                            DALSYSMemHandle *phMem,
                            uint32 *virtAddr)
{
   int nErr;
   
   if ( QURT_EOK != qurt_mem_map_static_query(virtAddr, physAddr, TMC_PAGE_SIZE, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
   {	
	   qurt_mem_pool_t TMC_pool;
	   qurt_mem_region_t TMC_region;
	   qurt_mem_region_attr_t TMC_region_attr;
	   TMC_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
	   TMC_region_attr.region_type = QURT_MEM_REGION_DEVICE;
	   TMC_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
	   TMC_region_attr.permission = QURT_PERM_FULL;
	   TMC_region_attr.physaddr = physAddr;
	   TMC_region_attr.virtaddr = (qurt_addr_t)-1;
	   TMC_region_attr.size = TMC_PAGE_SIZE;
	   
	   
	   TRY(nErr,qurt_mem_pool_attach("HWIO_POOL", &TMC_pool));
	   TRY(nErr,qurt_mem_region_create(&TMC_region, TMC_PAGE_SIZE, TMC_pool, &TMC_region_attr ));
	   TRY(nErr,qurt_mem_region_attr_get(TMC_region, &TMC_region_attr));
	   TRY(nErr,qurt_mapping_create (TMC_region_attr.virtaddr, TMC_region_attr.physaddr, TMC_region_attr.size, TMC_region_attr.cache_attribs, TMC_region_attr.permission));
	   
	   *virtAddr = TMC_region_attr.virtaddr;
	   CATCH(nErr) {}
	}
	else
	{
		nErr = QURT_EOK;	
	}	
    return nErr;
}


static void ConfigureETRToRAM(uint8 mode)
{
   HAL_qdss_etr_AccessUnLock();


   HAL_qdss_replicator_AccessUnLock();
   HAL_qdss_replicator_DisableTPIU();
   HAL_qdss_replicator_AccessLock();

   HAL_qdss_etr_DisableTraceCapture();

   // Program the MODE Register for Circular Buffer mode.
   HAL_qdss_etr_SetMode(mode);

   //Program the WrBurstLen field of the AXI Control Register
   HAL_qdss_etr_SetWriteBurstLength(0xF);//recommended value in HPG


   // Program the ScatterGatherMode field of the AXI Control Register (AXICTL) to 0
   HAL_qdss_etr_DisableScatterGather();

   //Set cache and prot ctrl to HPG recommendations
   HAL_qdss_etr_SetCacheProtCtrl();

   HAL_qdss_etr_SetFFCRETBDefault();

   HAL_qdss_etr_AccessLock();
}


void ConfigureETRToUSB()
{

   HAL_qdss_etr_AccessUnLock();
   HAL_qdss_etr_DisableTraceCapture();

   //QDSS Configuration
   //----------------

   //0.Set all bit fields of ATB Replicator IDFILTER1 to 1

   HAL_qdss_replicator_AccessUnLock();
   HAL_qdss_replicator_DisableTPIU();
   HAL_qdss_replicator_AccessLock();


   HAL_qdss_csr_AccessUnLock();


   //a.Configure the BLKSIZE field of the USB NDP BAM Control Register (USBBAMCTRL).
   HAL_qdss_csr_SetBlkSize(USBBAMCTRL_BLKSIZE_512);

   //b.Configure the PERFLSH and PERFLSHTHRS fields of the USB Trace Flush ControlRegister
   HAL_qdss_csr_SetPeriodicFlushThreshold(4096);
   HAL_qdss_csr_EnablePeriodicFlush();


   //c.Set the USBENAB field of USB NDP BAM Control Register (USBBAMCTRL) to one
   //Done later


   HAL_qdss_csr_AccessLock();

   //ETR Configuration
   //-----------------
   //a. Configure the RSZ field of the RAM Size Register (RSZ).
   // Done separately

   //b. Program the MODE Register for Circular Buffer mode.
   HAL_qdss_etr_SetMode(ETR_MODE_CIRCULAR_BUFFER);

   //c. Program the WrBurstLen field of the AXI Control Register
   HAL_qdss_etr_SetWriteBurstLength(0xF);


   //d. Program the ScatterGatherMode field of the AXI Control Register (AXICTL) to 0
   HAL_qdss_etr_DisableScatterGather();


   //e. Program the remaining fields of the AXI Control Register
   //Set cache and prot ctrl to HPG recommendations
   HAL_qdss_etr_SetCacheProtCtrl();


   //f.Configure the BUFADDRLO field of the Data Buffer Address Low Register (DBALO)
   //and the BUFADDRHI field of the Data Buffer Address High Register (DBAHI).
   //Done separately

   //g. Set the EnFt, FonFlIn, and EnTI bits in the FFCR Register to one.

   HAL_qdss_etr_SetFFCRETBDefault();


   HAL_qdss_etr_AccessLock();
}



static DALResult
TMC_DeviceInit(DalDeviceHandle *h)
{
   int nErr;
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   uint32 VirtualAddr=0;

   TRY(nErr,TMC_MemMap(QDSS_ETFETB_BASE_PHYS,
                       &pDevCtxt->hMemETFETB,
                       &VirtualAddr));

   HAL_qdss_etfetb_HalConfigInit(VirtualAddr);

   TRY(nErr,TMC_MemMap(QDSS_ETR_BASE_PHYS,
                       &pDevCtxt->hMemETR,
                       &VirtualAddr));

   HAL_qdss_etr_HalConfigInit(VirtualAddr);



   TRY(nErr,TMC_MemMap(QDSS_QDSSCSR_REG_BASE_PHYS,
                       &pDevCtxt->hMemCSR,
                       &VirtualAddr));

   HAL_qdss_csr_HalConfigInit(VirtualAddr);


   TRY(nErr,TMC_MemMap(QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE_PHYS,
                       &pDevCtxt->hMemReplicator,
                       &VirtualAddr));

   HAL_qdss_replicator_HalConfigInit(VirtualAddr);

   HAL_qdss_etfetb_AccessUnLock();
   if (HAL_qdss_etfetb_IsEnabled()) {
      //ETB got initialized from RPM (e.g for sysinit test)
      //Do nothing, we want to preserve what is in there
   }
   else {
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_CBUF);
      HAL_qdss_etfetb_EnableTraceCapture();
      pDevCtxt->mode = TMC_TRACESINK_ETB;
   }
   HAL_qdss_etfetb_AccessLock();


   CATCH(nErr) {}
   return nErr;
}


static DALResult
TMC_DeviceDeInit(DalDeviceHandle *h)
{
   #if 0
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
	
   
   if(pDevCtxt->hMemETFETB) {
      DALSYS_DestroyObject(pDevCtxt->hMemETFETB);
   }

   if(pDevCtxt->hMemETR) {
      DALSYS_DestroyObject(pDevCtxt->hMemETR);
   }

   if(pDevCtxt->hMemCSR) {
      DALSYS_DestroyObject(pDevCtxt->hMemCSR);
   }

   if(pDevCtxt->hMemReplicator) {
      DALSYS_DestroyObject(pDevCtxt->hMemReplicator);
   }
   #endif

   return DAL_SUCCESS;
}

static DALResult
TMC_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TMC_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTMC_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TMC_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTMC Interface.
------------------------------------------------------------------------------*/
static  DALResult
TMC_GetMode(DalDeviceHandle * _h,TMCTraceSinkMode *pMode)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(_h->pClientCtxt))->pDALDevCtxt;

   *pMode=pDevCtxt->mode;
   return DAL_SUCCESS;
}


static DALResult TMC_DisableTrace( DalDeviceHandle * h);


static  DALResult TMC_SetMode(DalDeviceHandle * _h,TMCTraceSinkMode mode)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(_h->pClientCtxt))->pDALDevCtxt;
   int nErr = DAL_ERROR;

   if (mode > TMC_TRACESINK_USB_BUFFERED
       && mode != TMC_TRACESINK_SD) {
      return nErr;
   }


   //Disable trace before switching mode
   //Needs to be enabled by calling EnableTrace

   TMC_DisableTrace(_h);
   HAL_qdss_etfetb_AccessUnLock();

   nErr=DAL_SUCCESS;
   switch(mode) {
   case  TMC_TRACESINK_ETB:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
      break;
   case  TMC_TRACESINK_RAM:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      ConfigureETRToRAM(QDSS_TMC_MODE_CBUF);
      break;
   case  TMC_TRACESINK_TPIU:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      HAL_qdss_replicator_AccessUnLock();
      HAL_qdss_replicator_EnableTPIU();
      HAL_qdss_replicator_AccessLock();
      break;
   case  TMC_TRACESINK_USB:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      ConfigureETRToUSB();
      break;
   case  TMC_TRACESINK_USB_BUFFERED:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      ConfigureETRToRAM(QDSS_TMC_MODE_CBUF);
      HAL_qdss_csr_AccessUnLock();
      HAL_qdss_csr_SetETRByteCounter(pDevCtxt->etrByteCounter);
      HAL_qdss_csr_AccessLock();
      break;
   case  TMC_TRACESINK_SD:
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);
      HAL_qdss_replicator_AccessUnLock();
      HAL_qdss_replicator_EnableTPIU();
      HAL_qdss_replicator_AccessLock();
      break;
   default:
      nErr = DAL_ERROR;
   }
   HAL_qdss_etfetb_AccessLock();
   if (DAL_SUCCESS == nErr) {
      pDevCtxt->mode = mode;
   }

   return nErr;
}

static  DALResult
TMC_GetTraceBuffer(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   HAL_qdss_etfetb_AccessUnLock();

   *pdwAddr = HAL_qdss_etr_GetDataBufferAddress();
   *pdwBufLen = HAL_qdss_etr_GetRAMSize();

   HAL_qdss_etfetb_AccessLock();

   return DAL_SUCCESS;
}

static  DALResult
TMC_SetTraceBuffer(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{

   HAL_qdss_etr_AccessUnLock();
   HAL_qdss_etr_SetDataBufferAddress(dwAddr);
   HAL_qdss_etr_SetRAMSize(dwBufLen);
   HAL_qdss_etr_AccessLock();

   return DAL_SUCCESS;
}



static DALResult
TMC_EnableTrace( DalDeviceHandle * h)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   HAL_qdss_etfetb_AccessUnLock();
   if (TMC_TRACESINK_ETB==pDevCtxt->mode) { 
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_CBUF);
   }
   else {
      HAL_qdss_etfetb_SetFFCRETBDefault(QDSS_TMC_MODE_HWFIFO);
   }

   HAL_qdss_etfetb_EnableTraceCapture();
   HAL_qdss_etfetb_AccessLock();

   if (TMC_TRACESINK_USB_BUFFERED==pDevCtxt->mode) {
      HAL_qdss_csr_AccessUnLock();
      HAL_qdss_csr_SetETRByteCounter(pDevCtxt->etrByteCounter);
      HAL_qdss_csr_AccessLock();
   }

   if ((TMC_TRACESINK_RAM==pDevCtxt->mode)||
       (TMC_TRACESINK_USB==pDevCtxt->mode)||
       (TMC_TRACESINK_USB_BUFFERED==pDevCtxt->mode)) {
      HAL_qdss_etr_AccessUnLock();
      HAL_qdss_etr_EnableTraceCapture();
      HAL_qdss_etr_AccessLock();
   }

   if (TMC_TRACESINK_USB==pDevCtxt->mode) {
      HAL_qdss_csr_AccessUnLock();
      HAL_qdss_csr_EnableUSB();
      HAL_qdss_csr_AccessLock();
   }

   return DAL_SUCCESS;
}


static DALResult
TMC_DisableTrace( DalDeviceHandle * h)
{
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;
   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_etfetb_DisableTraceCapture();
   HAL_qdss_etfetb_AccessLock();

   HAL_qdss_etr_AccessUnLock();
   HAL_qdss_etr_DisableTraceCapture();
   HAL_qdss_etr_AccessLock();


   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_DisableUSB();
   HAL_qdss_csr_SetETRByteCounter(0); //to disable interrupts
   HAL_qdss_csr_AccessLock();


   IGNORE(pClientCtxt);
   return DAL_SUCCESS;
}


static DALResult
ETB_ReadTrace(TMCDevCtxt *pDevCtxt, byte * buf, uint32  nBufLen, uint32 *pnLenRead)
{
   int nErr = 0;
   uint32 dwVal = 0;
   uint32 i = 0;
   uint32 *pdwBuf = (uint32 *)buf;

   HAL_qdss_etfetb_AccessUnLock();

   if (!HAL_qdss_etfetb_IsEnabled()) {
      THROW(nErr,DAL_ERROR);
   }

   while (i <= nBufLen-4 )  {
      dwVal = HAL_qdss_etfetb_ReadData();
      if (0xFFFFFFFF == dwVal) {
         break;
      }
      *pdwBuf = dwVal;
      pdwBuf++;
      i+=4;
   }

   if (pnLenRead) {
      *pnLenRead = i;
   }

   nErr =  DAL_SUCCESS;
   CATCH(nErr) {}
   HAL_qdss_etfetb_AccessLock();
   return nErr;
}


static DALResult
ETR_ReadTrace(TMCDevCtxt *pDevCtxt, byte * buf, uint32  nBufLen, uint32 *pnLenRead)
{
   int nErr = 0;
   uint32 dwVal = 0;
   uint32 i = 0;
   uint32 *pdwBuf = (uint32 *)buf;

   HAL_qdss_etr_AccessUnLock();

   if (!HAL_qdss_etr_IsEnabled()) {
      THROW(nErr,DAL_ERROR);
   }


   while (i <= nBufLen-4 )  {
      dwVal = HAL_qdss_etr_ReadData();
      if (0xFFFFFFFF == dwVal) {
         break;
      }
      *pdwBuf = dwVal;
      pdwBuf++;
      i+=4;
   }

   if (pnLenRead) {
      *pnLenRead = i;
   }
   nErr = DAL_SUCCESS;

   CATCH(nErr) {}
   HAL_qdss_etr_AccessLock();
   return nErr;
}



static DALResult
TMC_ReadTrace( DalDeviceHandle * h, byte * buf, uint32  nBufLen, uint32 *pnLenRead)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   if  ( (TMC_TRACESINK_ETB == pDevCtxt->mode)) {
     return ETB_ReadTrace(pDevCtxt,buf,nBufLen,pnLenRead);
   }
   else if ( (TMC_TRACESINK_RAM == pDevCtxt->mode)) {
     return ETR_ReadTrace(pDevCtxt,buf,nBufLen,pnLenRead);
   }
   else {
      return DAL_ERROR;
   }

}

static DALResult ETB_Flush(int flags)
{
   HAL_qdss_etfetb_AccessUnLock();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etfetb_StopOnFlush(1);
   }

   HAL_qdss_etfetb_Flush();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etfetb_WaitForTMCReady();
      HAL_qdss_etfetb_StopOnFlush(0);
   }

   if (flags & TMC_FLUSH_BLOCK) {
      HAL_qdss_etfetb_FlushWait();
   }
   HAL_qdss_etfetb_AccessLock();
   return DAL_SUCCESS;
}


static DALResult ETR_Flush(int flags)
{
   HAL_qdss_etr_AccessUnLock();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etr_StopOnFlush(1);
   }

   HAL_qdss_etr_Flush();

   if (flags & TMC_FLUSH_STOP) {
      HAL_qdss_etr_WaitForTMCReady();
      HAL_qdss_etr_StopOnFlush(0);
   }

   HAL_qdss_etr_AccessLock();
   return DAL_SUCCESS;
}



static DALResult
TMC_Flush( DalDeviceHandle * h, int flags)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   if  (TMC_TRACESINK_ETB == pDevCtxt->mode) {
      return ETB_Flush(flags);
   }
   else if (TMC_TRACESINK_RAM == pDevCtxt->mode) {
      return ETR_Flush(flags);
   }
   else if (TMC_TRACESINK_USB_BUFFERED == pDevCtxt->mode) {
      //ETR flush with out stop on flush seems to not complete
      return ETB_Flush(flags);
   }

   return DAL_SUCCESS;
}


static DALResult
TMC_GetParam( DalDeviceHandle * h, TMCParamType  param, uint32  *puVal)
{
   int nErr=DAL_ERROR;
   TMCClientCtxt *pClientCtxt = ((DalTMCHandle *)h)->pClientCtxt;
   uint32 uVal = 0;

   switch(param) {
   case TMC_ETR_WRITE_PTR:
      HAL_qdss_etr_AccessUnLock();
      uVal = HAL_qdss_etr_GetWritePtr();
      HAL_qdss_etr_AccessLock();
      nErr = DAL_SUCCESS;
      break;

   case TMC_ETR_LATCH_BUFFER_FILL:
      HAL_qdss_etr_AccessUnLock();
      uVal = HAL_qdss_etr_GetLatchBufferFill();
      HAL_qdss_etr_AccessLock();
      nErr = DAL_SUCCESS;
      break;
   default:
      nErr=DAL_ERROR;
   }
   *puVal = uVal;

   IGNORE(pClientCtxt);

   return nErr;
}


static DALResult
TMC_SetParam( DalDeviceHandle * h, TMCParamType  param, uint32  uVal)
{
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   int nErr=DAL_SUCCESS;

   switch(param) {
   case TMC_ETR_BYTE_COUNTER:
      pDevCtxt->etrByteCounter = uVal;
      HAL_qdss_csr_AccessUnLock();
      HAL_qdss_csr_SetETRByteCounter(pDevCtxt->etrByteCounter);
      HAL_qdss_csr_AccessLock();
      break;

   default:
      nErr=DAL_ERROR;
   }


   return nErr;
}

static void TMC_SaveRegs(uint32 base_addr, byte *buf, uint32 buf_size)
{
   uint32 offset;
   uint32 max_offset;
#define TMC_RRD_OFFSET	0x10
#define TMC_RWD_OFFSET	0x24


   max_offset = (buf_size >TMC_PAGE_SIZE)  ? TMC_PAGE_SIZE:buf_size;

   for (offset=0; offset+4 <= max_offset; offset+=4) {

      if ((TMC_RRD_OFFSET==offset) || (TMC_RWD_OFFSET == offset)) {
         continue;
      }

      __outpdw((buf+offset), __inpdw((base_addr+offset)));
   }
}


static DALResult
TMC_SaveETBRegisters(DalDeviceHandle * h, byte * buf, uint32  nBufLen)
{
   #if 0
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   #endif
   
   int nErr=DAL_SUCCESS;
   QDSSMemInfo memInfo;

   #if 0
   DALSYS_MemInfo(pDevCtxt->hMemETFETB, &memInfo);
   #endif

   HAL_qdss_etfetb_AccessUnLock();

   TMC_SaveRegs(QDSS_ETFETB_ETFETB_CXTMC_F128W64K_REG_BASE,buf,nBufLen);

   HAL_qdss_etfetb_AccessLock();

   return nErr;
}



static DALResult
TMC_SaveETRRegisters(DalDeviceHandle * h, byte * buf, uint32  nBufLen)
{
   #if 0
   TMCDevCtxt *pDevCtxt = (TMCDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   #endif
   
   int nErr=DAL_SUCCESS;
   QDSSMemInfo memInfo;

   #if 0
   DALSYS_MemInfo(pDevCtxt->hMemETR, &memInfo);
   #endif

   HAL_qdss_etr_AccessUnLock();

   TMC_SaveRegs(QDSS_ETR_ETR_CXTMC_R64W32D_REG_BASE,buf,nBufLen);

   HAL_qdss_etr_AccessLock();

   return nErr;
}



static void
TMC_InitInterface(TMCClientCtxt* pclientCtxt)
{
 static const DalTMC vtbl =
   {
      {
         TMC_DeviceAttach,
         TMC_DeviceDetach,
         TMC_DeviceInit,
         TMC_DeviceDeInit,
         TMC_DeviceOpen,
         TMC_DeviceClose,
         TMC_DeviceInfo,
         TMC_DevicePowerEvent,
         TMC_DeviceSysRequest
      },

      TMC_GetMode,
      TMC_SetMode,
      TMC_GetTraceBuffer,
      TMC_SetTraceBuffer,
      TMC_EnableTrace,
      TMC_DisableTrace,
      TMC_ReadTrace,
      TMC_Flush,
      TMC_GetParam,
      TMC_SetParam,
      TMC_SaveETBRegisters,
      TMC_SaveETRRegisters
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTMCHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTMCHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTMCHandle.pClientCtxt = pclientCtxt;
}

DALResult
TMC_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static TMCDrvCtxt drvCtxt = {{TMC_DriverInit,
                                 TMC_DriverDeInit
                                },1,
                                sizeof(TMCDevCtxt)};
   static TMCClientCtxt clientCtxt;

   TMCClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      TMC_InitInterface(pclientCtxt);
      TMC_AddRef(&(pclientCtxt->DalTMCHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTMCHandle);
   }
   return nErr;
}



