/*=============================================================================
  Copyright (c) 2011-2012 QUALCOMM Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/core.tx/6.0/debugtrace/hwevent/dal/src/DalHWEvent.c#1 $
=============================================================================*/
#include <stdlib.h>		// malloc,free

#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIHWEvent.h"
#include "DDIHWIO.h"
#include "HALhwio.h"
#include "npa.h"
#include "qdss_fuse_check.h"
#include "ClockDefs.h"
#include "qdss_throw.h"
#include "qurt_memory.h"
#include "qurt_error.h"


/*
 * Define the page size for HWIO regions - 4K.
 */
#define HWIO_PAGE_SIZE 0x1000

/*
 * Define macro to get the page base and offset given an address.
 */
#define HWIO_PAGE_BASE(nAddr)   ((nAddr) & ~(HWIO_PAGE_SIZE - 1))

#define HWIO_PAGE_OFFSET(nAddr) ((nAddr)& (HWIO_PAGE_SIZE - 1))

#define CORESIGHT_LOCK_OFFSET 0x00000fb0


typedef struct HWEventMemMapType
{
   uint32 uphys_addr;
   uint32 uvirt_addr;
   DALSYSMemHandle hMem;
} HWEventMemMap;

typedef struct HWEventRegisterValue_t HWEventRegisterValue;
typedef struct HWEventAddrRange_t HWEventAddrRange;

/*
address-value pairs for HW Events.
*/
struct HWEventRegisterValue_t{
   uint32 reg_phys_addr;
   uint32 reg_value;
};

struct HWEventAddrRange_t
{
   uint32 lower_range;
   uint32 higher_range;
   uint32 unlock_value;

};

/*------------------------------------------------------------------------------
Declaring a "HWEvent" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct HWEventDrvCtxt HWEventDrvCtxt;
typedef struct HWEventDevCtxt HWEventDevCtxt;
typedef struct HWEventClientCtxt HWEventClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "HWEvent" Vtable
------------------------------------------------------------------------------*/
typedef struct HWEventDALVtbl HWEventDALVtbl;
struct HWEventDALVtbl
{
   int (*HWEvent_DriverInit)(HWEventDrvCtxt *);
   int (*HWEvent_DriverDeInit)(HWEventDrvCtxt *);
};

struct HWEventDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   HWEventDrvCtxt  *pHWEventDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //HWEvent Dev state can be added by developers here
   HWEventMemMap *pMemMapTable;
   HWEventRegisterValue *pConfigTable;
   HWEventAddrRange *pAddrRangeTable;
   uint32 uTableSize;
   uint32 uAddrRangeTableSize;
};

struct HWEventDrvCtxt
{
   //Base Members
   HWEventDALVtbl HWEventDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   HWEventDevCtxt HWEventDevCtxt[1];
   //HWEvent Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "HWEvent" Client Context
------------------------------------------------------------------------------*/
struct HWEventClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   HWEventDevCtxt *pHWEventDevCtxt;
   DalHWEventHandle DalHWEventHandle;
   //HWEvent Client state can be added by developers here
};



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
HWEvent_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);

static DALResult HWEvent_FindPhysAddrMapping(uint32 phys_addr,
                                      uint32 *pvirt_addr,
                                      HWEventDevCtxt *pDevCtxt);

static void HWEvent_SetReg(uint32 virtAddr,uint32 regAddr,uint32 regValue);

static DALResult HWEvent_AddPhysAddrMapping(uint32 phys_addr,
                                     uint32 *pvirt_addr,
                                     HWEventDevCtxt *pDevCtxt);

static void CoreSightUnLock(uint32 uBaseAddr);
static void CoreSightLock(uint32 uBaseAddr);




/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALHWEvent_DriverInfo = { HWEvent_DeviceAttach, 0, NULL};


typedef struct HWEventTableSize_t HWEventTableSize;

struct HWEventTableSize_t{
   uint32 size;
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
HWEvent_AddRef(DalHWEventHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
HWEvent_DriverInit(HWEventDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
HWEvent_DriverDeInit(HWEventDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

static __inline boolean HWEvent_isBlockedAddress(uint32 phys_addr,HWEventDevCtxt 
*pDevCtxt,uint32 *pUnlockCode)
{
   HWEventAddrRange *pAddrRangeTable;
   uint32 i;
   i=0;
   pAddrRangeTable = pDevCtxt->pAddrRangeTable;
   while((pAddrRangeTable->lower_range!=NULL) &&
        (i<pDevCtxt->uAddrRangeTableSize))
   {
      if((phys_addr>=pAddrRangeTable->lower_range) && (phys_addr<=pAddrRangeTable->higher_range))
      {
         *pUnlockCode=pAddrRangeTable->unlock_value;
         return FALSE;//not blocked
      }
      pAddrRangeTable++;
      i++;
   }
   return TRUE; //not found, is blocked
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
HWEvent_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}



static DALResult
HWEvent_DeviceInit(DalDeviceHandle *h)
{
      DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
      HWEventDevCtxt *pDevCtxt;
      DALSYSPropertyVar PropVar;
      uint32 uNumReg,phys_addr,virt_addr=NULL;
      DALResult uFound;


      HWEventRegisterValue *pConfigTable;
      HWEventTableSize *pConfigTableSize;
      uint32 i;


      pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;



      if(DAL_SUCCESS!=DALSYS_GetDALPropertyHandleStr(pClientCtxt->pDALDevCtxt->strDeviceName,
         pDevCtxt->hProp))
      {
         return DAL_ERROR;
      }


      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_config",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pConfigTable=(HWEventRegisterValue *)PropVar.Val.pStruct;
      pDevCtxt->pConfigTable=pConfigTable;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_config_size",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pConfigTableSize=(HWEventTableSize *)PropVar.Val.pStruct;
      uNumReg=pConfigTableSize->size;
      pDevCtxt->uTableSize=uNumReg;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_addr_check",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      pDevCtxt->pAddrRangeTable=(HWEventAddrRange *)PropVar.Val.pStruct;

      if (DAL_SUCCESS!= DALSYS_GetPropertyValue(pDevCtxt->hProp,
                        "hwevent_addr_check_size",
                        0,
                        &PropVar ))
      {
         return DAL_ERROR;
      }
      
      pConfigTableSize=(HWEventTableSize *)PropVar.Val.pStruct;
      pDevCtxt->uAddrRangeTableSize = pConfigTableSize->size;
      


   /*allocate memory to store virtual address of registers*/
	  pDevCtxt->pMemMapTable = (void *)malloc(uNumReg*sizeof(HWEventMemMap));
	  if(NULL==pDevCtxt->pMemMapTable)
	  {
		return DAL_ERROR;
	  }
      memset(pDevCtxt->pMemMapTable,0x0,uNumReg*sizeof(HWEventMemMap));


      for(i=0;i<uNumReg;i++)
      {
         phys_addr= HWIO_PAGE_BASE(pConfigTable[i].reg_phys_addr);
         /*end of table*/
         if (phys_addr==NULL)
         {
            break;
         }
         uFound = HWEvent_FindPhysAddrMapping(phys_addr,&virt_addr,pDevCtxt);
         if (uFound==DAL_ERROR)
         {
            if (DAL_SUCCESS!=HWEvent_AddPhysAddrMapping(phys_addr,&virt_addr,pDevCtxt))
            {
               return DAL_ERROR;
            }
         }

      }

   return DAL_SUCCESS;
}


static DALResult
HWEvent_DeviceDeInit(DalDeviceHandle *h)
{
   DALResult ret=DAL_ERROR;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   HWEventDevCtxt *pDevCtxt;
   #if 0
   uint32 i;
   #endif

   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;
   if(pDevCtxt->pMemMapTable!=NULL)
   {
      #if 0
      for(i=0;i<pDevCtxt->uTableSize;i++)
      {
         if(pDevCtxt->pMemMapTable[i].hMem!=NULL)
         {
            DALSYS_DestroyObject(pDevCtxt->pMemMapTable[i].hMem);/*destroy mem object*/
         }
      }
	  #endif
      
	  free(pDevCtxt->pMemMapTable);
      ret=DAL_SUCCESS;
   }


   return ret;
}

static DALResult
HWEvent_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
HWEvent_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALHWEVENT_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
HWEvent_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalHWEvent Interface.
------------------------------------------------------------------------------*/
DALResult HWEvent_SetRegister(DalDeviceHandle* h,uint32 phys_addr,uint32 value,
                              uint8 *pret_code)
{
   HWEventDevCtxt *pDevCtxt;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   
   
   DALResult retcode = DAL_ERROR;
   uint32 unlock_code;
   
   qurt_addr_t virtaddr_temp;	
   qurt_mem_pool_t HWevent_Pool;
   qurt_mem_region_t HWevent_region;
   qurt_mem_region_attr_t HWevent_region_attr;


   
/*register peek-poke functionality needs to be guarded by a security fuse*/
   if(!qdss_fuse_trace_access())
   {
      *pret_code=QDSS_HWEVENT_FUSE_CHECK_ERR;
      return DAL_ERROR;
   }

   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;

   if(pDevCtxt==NULL)
     return DAL_ERROR;
   if (HWEvent_isBlockedAddress(phys_addr,pDevCtxt,&unlock_code))
   {
      *pret_code=QDSS_HWEVENT_ADDR_CHECK_ERR;
      return DAL_ERROR;
   }
   
   HWevent_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
   HWevent_region_attr.region_type = QURT_MEM_REGION_DEVICE;
   HWevent_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
   HWevent_region_attr.permission = QURT_PERM_FULL;
   HWevent_region_attr.physaddr = HWIO_PAGE_BASE(phys_addr);
   HWevent_region_attr.virtaddr = (qurt_addr_t)-1;
   HWevent_region_attr.size = HWIO_PAGE_SIZE;
   
   if ( QURT_EOK != qurt_mem_map_static_query(&virtaddr_temp, HWevent_region_attr.physaddr , HWevent_region_attr.size, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
   {
	   retcode = qurt_mem_pool_attach("HWIO_POOL", &HWevent_Pool);	
	   if (QURT_EOK != retcode)
	   { 
			return DAL_ERROR;
	   }
	   retcode = qurt_mem_region_create(&HWevent_region, HWevent_region_attr.size, HWevent_Pool, &HWevent_region_attr );
	   if (QURT_EOK != retcode)
	   { 
			return DAL_ERROR;
	   }
	   retcode = qurt_mem_region_attr_get(HWevent_region, &HWevent_region_attr);	
	   if(QURT_EOK == retcode)
	   {  
		  retcode = qurt_mapping_create (HWevent_region_attr.virtaddr, HWevent_region_attr.physaddr, HWevent_region_attr.size, HWevent_region_attr.cache_attribs, HWevent_region_attr.permission);	
		  if(QURT_EOK == retcode)
		  {
			  if(unlock_code=='U')
			  {
				 CoreSightUnLock((uint32)HWIO_PAGE_BASE(HWevent_region_attr.virtaddr));         
			  }
			  HWEvent_SetReg((uint32)HWevent_region_attr.virtaddr,phys_addr,value);
			  
			  if(unlock_code=='U')
			  {
				 CoreSightLock((uint32)HWIO_PAGE_BASE(HWevent_region_attr.virtaddr));
			  }	   
		  }
	   }
	}
	else
	{
		HWevent_region_attr.virtaddr =(uint32) virtaddr_temp;
		if(unlock_code=='U')
		{
			CoreSightUnLock((uint32)HWIO_PAGE_BASE(HWevent_region_attr.virtaddr));         
		}
		HWEvent_SetReg((uint32)HWevent_region_attr.virtaddr,phys_addr,value);
		  
		if(unlock_code=='U')
		{
			CoreSightLock((uint32)HWIO_PAGE_BASE(HWevent_region_attr.virtaddr));
		}
		retcode = QURT_EOK;	
	}
   #if 0
   retcode=DALSYS_DestroyObject(hMem);
   #endif
   
   if(retcode==QURT_EOK)
      *pret_code=QDSS_HWEVENT_CONFIG_SUCCESS;
   return retcode;
}

DALResult HWEvent_ConfigurePresetEvents(DalDeviceHandle* h)
{
   HWEventDevCtxt *pDevCtxt;
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   uint32 uNumReg,i,unlock_code;
   DALResult uFound;
   uint32 virt_addr;


   pDevCtxt=(HWEventDevCtxt *)pClientCtxt->pDALDevCtxt;

   if(pDevCtxt==NULL)
      return DAL_ERROR;

   uNumReg = pDevCtxt->uTableSize;


   for(i=0;i<uNumReg;i++)
   {
/*Silently drop writes to blocked address regions without flagging an error*/
      if (!HWEvent_isBlockedAddress(pDevCtxt->pConfigTable[i].reg_phys_addr,
         pDevCtxt,&unlock_code))
      {
         uFound = HWEvent_FindPhysAddrMapping(
                       HWIO_PAGE_BASE(pDevCtxt->pConfigTable[i].reg_phys_addr),
                       &virt_addr,
                       pDevCtxt);
         if (uFound==DAL_ERROR)
         {
            return DAL_ERROR;
         }
         if(virt_addr!=NULL)
         {
            HWEvent_SetReg((uint32)virt_addr,
                        pDevCtxt->pConfigTable[i].reg_phys_addr,
                        pDevCtxt->pConfigTable[i].reg_value);
         }
      }

   }
   return DAL_SUCCESS;
}

static void
HWEvent_InitInterface(HWEventClientCtxt* pclientCtxt)
{
 static const DalHWEvent vtbl =
   {
      {
         HWEvent_DeviceAttach,
         HWEvent_DeviceDetach,
         HWEvent_DeviceInit,
         HWEvent_DeviceDeInit,
         HWEvent_DeviceOpen,
         HWEvent_DeviceClose,
         HWEvent_DeviceInfo,
         HWEvent_DevicePowerEvent,
         HWEvent_DeviceSysRequest
      },
      HWEvent_SetRegister,
      HWEvent_ConfigurePresetEvents
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalHWEventHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalHWEventHandle.pVtbl  = &vtbl;
   pclientCtxt->DalHWEventHandle.pClientCtxt = pclientCtxt;
}

DALResult
HWEvent_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static HWEventDrvCtxt drvCtxt = {{HWEvent_DriverInit,
                                      HWEvent_DriverDeInit
                                     },1,
                                     sizeof(HWEventDevCtxt)};
   static HWEventClientCtxt clientCtxt;

   HWEventClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      HWEvent_InitInterface(pclientCtxt);
      HWEvent_AddRef(&(pclientCtxt->DalHWEventHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalHWEventHandle);
   }
   return nErr;
}


static __inline void HWEvent_SetReg(uint32 virtAddr,uint32 regAddr,uint32
regValue)
{
   virtAddr=virtAddr+HWIO_PAGE_OFFSET(regAddr);//mapping to a 4K aligned virtual page
   out_dword(virtAddr,regValue);
}


static DALResult HWEvent_FindPhysAddrMapping(uint32 phys_addr,uint32 *pvirt_addr,
HWEventDevCtxt *pDevCtxt)
{
   uint32 i;
   uint32 uTableSize=pDevCtxt->uTableSize;
   DALResult DalRet=DAL_ERROR;
   HWEventMemMap *pMap=pDevCtxt->pMemMapTable;
   *pvirt_addr=NULL;


   for(i=0;i<uTableSize;i++)
   {
      if (pMap[i].uphys_addr==phys_addr)
      {
         DalRet=DAL_SUCCESS;
         *pvirt_addr=pMap[i].uvirt_addr;
         break;
      }
   }
   return DalRet;
}

/*This function is called only during driver initialization*/
static DALResult HWEvent_AddPhysAddrMapping(uint32 phys_addr,
               uint32 *pvirt_addr,
               HWEventDevCtxt *pDevCtxt)
{
   uint32 i;
   uint32 uTableSize=pDevCtxt->uTableSize;
   DALSYSMemAddr virt_addr;
   DALResult DalRet=DAL_ERROR;
   
   #if 0
   DALSYSMemHandle hMem;
   QDSSMemInfo MemInfo;
   #endif
   
   HWEventMemMap *pMap=pDevCtxt->pMemMapTable;

   qurt_addr_t virtaddr_temp2;	
   qurt_mem_pool_t HWeventphy_Pool;
   qurt_mem_region_t HWeventphy_region;
   qurt_mem_region_attr_t HWeventphy_region_attr;
   HWeventphy_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
   HWeventphy_region_attr.region_type = QURT_MEM_REGION_DEVICE;
   HWeventphy_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
   HWeventphy_region_attr.permission = QURT_PERM_FULL;
   HWeventphy_region_attr.physaddr = phys_addr;
   HWeventphy_region_attr.virtaddr = (qurt_addr_t)-1;
   HWeventphy_region_attr.size = HWIO_PAGE_SIZE;
		 

   for(i=0;(i<uTableSize);i++)
   {
      if (pMap[i].uphys_addr==0x0)
      {
         #if 0
		 if (DAL_SUCCESS!=DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
               DALSYS_MEM_ADDR_NOT_SPECIFIED,
               (DALSYSMemAddr)phys_addr,
               HWIO_PAGE_SIZE,
               &hMem,
               NULL))
         {
            DalRet = DAL_ERROR;
            break;
         }
         DALSYS_MemInfo(hMem, &MemInfo);
		 #endif
		 
 
		if ( QURT_EOK != qurt_mem_map_static_query(&virtaddr_temp2, HWeventphy_region_attr.physaddr , HWeventphy_region_attr.size, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
		{
			qurt_mem_pool_attach("HWIO_POOL", &HWeventphy_Pool);
			if (QURT_EOK != DalRet)
			{ 
				DalRet = DAL_ERROR;
				break;
			}	
			DalRet = qurt_mem_region_create(&HWeventphy_region, HWeventphy_region_attr.size, HWeventphy_Pool, &HWeventphy_region_attr );
			if (QURT_EOK != DalRet)
			{ 
				DalRet = DAL_ERROR;
				break;
			}	     
			DalRet = qurt_mapping_create (HWeventphy_region_attr.virtaddr, HWeventphy_region_attr.physaddr, HWeventphy_region_attr.size, HWeventphy_region_attr.cache_attribs, HWeventphy_region_attr.permission);
			if (QURT_EOK == DalRet)		 
			{
				DalRet = qurt_mem_region_attr_get(HWeventphy_region, &HWeventphy_region_attr);
				if (QURT_EOK == DalRet)
				{
				virt_addr=HWeventphy_region_attr.virtaddr;
				pMap[i].uphys_addr=phys_addr;
				pMap[i].uvirt_addr=(uint32)virt_addr;
				*pvirt_addr=(uint32)virt_addr;
				DalRet = DAL_SUCCESS;
				}
			}
		
		} 
		else
		{
			virt_addr=(uint32)virtaddr_temp2;
			pMap[i].uphys_addr=phys_addr;
			pMap[i].uvirt_addr=(uint32)virt_addr;
			*pvirt_addr=(uint32)virt_addr;
			DalRet = DAL_SUCCESS;
		}
        break;
		 
      }
      else if (pMap[i].uphys_addr==phys_addr) //found existing mapping
      {
         *pvirt_addr=pMap[i].uvirt_addr;
         DalRet = DAL_SUCCESS;
         break;
      }
   }
   return DalRet;
}

static __inline void CoreSightUnLock(uint32 virtAddr)
{
   virtAddr=virtAddr+CORESIGHT_LOCK_OFFSET;
   out_dword(virtAddr,0xC5ACCE55);
}

static __inline void CoreSightLock(uint32 virtAddr)
{
   virtAddr=virtAddr+CORESIGHT_LOCK_OFFSET;
   out_dword(virtAddr,0x0);
}

