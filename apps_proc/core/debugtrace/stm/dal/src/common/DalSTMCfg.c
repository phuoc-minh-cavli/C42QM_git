/* ==========================================================================
               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* ==========================================================================
$Header: //components/rel/core.tx/6.0/debugtrace/stm/dal/src/common/DalSTMCfg.c#1 $
=============================================================================*/

#include "halqdss_stm_config.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DDISTMCfg.h"
#include "qdss_throw.h"
#include "qurt_memory.h"
#include "qurt_error.h"

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct STMCfgDrvCtxt STMCfgDrvCtxt;
typedef struct STMCfgDevCtxt STMCfgDevCtxt;
typedef struct STMCfgClientCtxt STMCfgClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "STMCfg" Vtable
------------------------------------------------------------------------------*/
typedef struct STMCfgDALVtbl STMCfgDALVtbl;
struct STMCfgDALVtbl
{
   int (*STMCfg_DriverInit)(STMCfgDrvCtxt *);
   int (*STMCfg_DriverDeInit)(STMCfgDrvCtxt *);
};

struct STMCfgDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   STMCfgDrvCtxt  *pSTMCfgDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //STMCfg Dev state can be added by developers here
   uint32 claimTag;
   DALSYSMemHandle hMem;
   boolean bInitialized;
};

struct STMCfgDrvCtxt
{
   //Base Members
   STMCfgDALVtbl STMCfgDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   STMCfgDevCtxt STMCfgDevCtxt[1];
   //STMCfg Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "STMCfg" Client Context
------------------------------------------------------------------------------*/
struct STMCfgClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   STMCfgDevCtxt *pSTMCfgDevCtxt;
   DalSTMCfgHandle DalSTMCfgHandle;
   //STMCfg Client state can be added by developers here
};

static void _AccessGrant(uint32 claimTag)
{
   HAL_qdss_stm_AccessUnLock();

try_again:
   HAL_qdss_stm_SetClaimTag(claimTag);
   if(HAL_qdss_stm_GetClaimTag() != claimTag)
   {
      HAL_qdss_stm_ClrClaimTag(claimTag);
      goto try_again;
   }
}

static void _AccessRevoke(uint32 claimTag)
{
   HAL_qdss_stm_ClrClaimTag(claimTag);
   HAL_qdss_stm_AccessLock();
}

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
STMCfg_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
static DALDEVICEID DalSTMCfg_DeviceId[1] = {DALDEVICEID_STM_CONFIG};

const DALREG_DriverInfo
DALSTMCfg_DriverInfo = { STMCfg_DeviceAttach, 1, DalSTMCfg_DeviceId };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

static uint32
STMCfg_AddRef(DalSTMCfgHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
STMCfg_DriverInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
STMCfg_DriverDeInit(STMCfgDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
void STMCfg_STMEnable(STMCfgDevCtxt *pDevCtxt) 
{
   HAL_qdss_stm_PortSelRet portSel;

   if(!pDevCtxt->bInitialized)
   {
      pDevCtxt->bInitialized = TRUE;

      HAL_qdss_stm_MasterSelectCfg(0, MASTER_SELECT_CTL_ALL);

      HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);

      HAL_qdss_stm_PortSelectCfg(0, 0, PORT_SELECT_CTL_SELECT_ALL, &portSel);

      HAL_qdss_stm_TimeStampEnable();

      /*STM ATID is 16 per ATID document*/
      HAL_qdss_stm_TraceIdCfg(16);

      /*Generate ATID 0x7D for writes to TRIG location*/
      HAL_qdss_stm_TrigGenTrigLocation();

      /*Sync Period is 4K bytes*/
      HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,0xFFF);

   }
   HAL_qdss_stm_Enable();
}

static uint32
STMCfg_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
STMCfg_DeviceInit(DalDeviceHandle *h)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALSYSPropertyVar PropVar;
   QDSSMemInfo memInfo;
   boolean nsnid_status;
   int nErr;
   qurt_mem_region_t STMCfg_region;
   qurt_mem_region_attr_t STMCfg_region_attr;
   qurt_mem_pool_t STMCfg_Pool;
      

   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle( pDevCtxt->DevId,
                                                   pDevCtxt->hProp ))
      return DAL_ERROR;


   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                "stm_claim_tag",
                                                 0,
                                                &PropVar ))
      return DAL_ERROR;

   pDevCtxt->claimTag = PropVar.Val.dwVal;


   if (DAL_SUCCESS != DALSYS_GetPropertyValue( pDevCtxt->hProp,
                                                "stm_phys_addr",
                                                 0,
                                                &PropVar ))
      return DAL_ERROR;


  if (QURT_EOK != qurt_mem_map_static_query(&memInfo.VirtualAddr, PropVar.Val.dwVal, 4096, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
  {    
	   STMCfg_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
	   STMCfg_region_attr.region_type = QURT_MEM_REGION_DEVICE;
	   STMCfg_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
	   STMCfg_region_attr.permission = QURT_PERM_FULL;
	   STMCfg_region_attr.physaddr = PropVar.Val.dwVal;	
	   STMCfg_region_attr.virtaddr = (qurt_addr_t)-1;
	   STMCfg_region_attr.size = 4096;

	   nErr = qurt_mem_pool_attach("HWIO_POOL", &STMCfg_Pool);
	   if (QURT_EOK != nErr)
	   {
			return DAL_ERROR;
	   }
	   nErr = qurt_mem_region_create(&STMCfg_region, 4096, STMCfg_Pool, &STMCfg_region_attr );
	   if (QURT_EOK != nErr)
	   {
			return DAL_ERROR;
	   }
	   
	   nErr = qurt_mapping_create (STMCfg_region_attr.virtaddr, STMCfg_region_attr.physaddr, STMCfg_region_attr.size, STMCfg_region_attr.cache_attribs, STMCfg_region_attr.permission);
	   if (QURT_EOK != nErr)
	   {
			return DAL_ERROR;
	   }
	   
	   nErr = qurt_mem_region_attr_get(STMCfg_region, &STMCfg_region_attr);
	   if (QURT_EOK != nErr)
	   {
			return DAL_ERROR;
	   }
	   memInfo.VirtualAddr = STMCfg_region_attr.virtaddr;
  }	
  
  // memInfo.VirtualAddr = STMCfg_region_attr.virtaddr;
   HAL_qdss_stm_HalConfigInit(memInfo.VirtualAddr);

   pDevCtxt->bInitialized=FALSE;

   nsnid_status=HAL_qdss_stm_AuthStatusNSNID();
   return (nsnid_status==TRUE)?DAL_SUCCESS:DAL_ERROR_DEVICE_ACCESS_DENIED;
}

static DALResult
STMCfg_DeviceDeInit(DalDeviceHandle *h)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;

   _AccessGrant(pDevCtxt->claimTag);
   HAL_qdss_stm_Disable();
  _AccessRevoke(pDevCtxt->claimTag);
#if 0
   if(pDevCtxt->hMem)
      DALSYS_DestroyObject(pDevCtxt->hMem);
#endif
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
STMCfg_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTMCFG_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
STMCfg_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSTMCfg Interface.
------------------------------------------------------------------------------*/

static DALResult
STMCfg_STMControl( DalDeviceHandle * h, STMControlType  uCtrl, uint32  uVal)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALResult nErr = DAL_SUCCESS;

   _AccessGrant(pDevCtxt->claimTag);

   switch(uCtrl)
   {
      case STM_EN:
         if(uVal) {
            STMCfg_STMEnable(pDevCtxt);
         }
         else {
            HAL_qdss_stm_Disable();
         }
         break;

      case STM_SW_TRACE_EN:
         if(uVal)
            HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);
         else
            HAL_qdss_stm_PortEnableMask(0x0);
         break;

   case STM_HW_EVT_EN:
         if(uVal) {
            HAL_qdss_stm_HWEventEnable();
         }
         else {
            HAL_qdss_stm_HWEventDisable();
         }
         break;


      case STM_TIMESTAMP_EN:
         if(uVal)
            HAL_qdss_stm_TimeStampEnable();
         else
            HAL_qdss_stm_TimeStampDisable();
         break;

      case STM_TIMESTAMP_FREQ:
         HAL_qdss_stm_SetTimeStampFreq(uVal);
         break;

      case STM_TIMESTAMP_FREQ_PKTGEN:
         if(uVal)
         {  HAL_qdss_stm_SetTimeStampFreq(HAL_qdss_stm_GetTimeStampFreq());
         }
         break;

      case STM_COMPRESSION_EN:
         if(uVal)
         {
            HAL_qdss_stm_CompEnable();
         }
         else
         {
            HAL_qdss_stm_CompDisable();
         }
      break;

      case STM_SYNC_EN:
         if(uVal)
         {
            HAL_qdss_stm_SyncEnable();
         }
         else
         {
            HAL_qdss_stm_SyncDisable();
         }
         break;

      case STM_SYNC_PERIOD_N:
       if(uVal)
       {
         HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_N_BYTES,uVal);
       }
       break;

       case STM_SYNC_PERIOD_2POW_N:
        if(uVal)
        {
          HAL_qdss_stm_SyncPeriodCfg(STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES,uVal);
        }
         break;

       case STM_HW_EVT_ENABLE_MASK:
         HAL_qdss_stm_HWEventEnableMask(uVal);
         break;

       case STM_HW_EVT_TRIGGER_MASK:
          HAL_qdss_stm_HWEventTriggerSetMask(uVal);
          break;
      default:
         nErr = DAL_ERROR;
   }

   _AccessRevoke(pDevCtxt->claimTag);
   return nErr;
}


static DALResult
STMCfg_STMStatus( DalDeviceHandle * h, STMControlType  uCtrl, uint32  *puVal)
{
   STMCfgDevCtxt *pDevCtxt = (STMCfgDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   DALResult nErr = DAL_SUCCESS;

   _AccessGrant(pDevCtxt->claimTag);

   switch(uCtrl)
      {
      case STM_EN:
         if (HAL_qdss_stm_isEnabled()) {
            *puVal=1;
         }
         else {
            *puVal=0;
         }
         break;

      case STM_HW_EVT_EN:
         if (HAL_qdss_stm_HWevent_isEnabled()) {
            *puVal=1;
         }
         else {
            *puVal=0;
         }
         break;

      default:
         nErr = DAL_ERROR;
      }

   _AccessRevoke(pDevCtxt->claimTag);
   return nErr;
}


static void
STMCfg_InitInterface(STMCfgClientCtxt* pclientCtxt)
{
   static const DalSTMCfg vtbl =
   {
      {
         STMCfg_DeviceAttach,
         STMCfg_DeviceDetach,
         STMCfg_DeviceInit,
         STMCfg_DeviceDeInit,
         STMCfg_DeviceOpen,
         STMCfg_DeviceClose,
         STMCfg_DeviceInfo,
         STMCfg_DevicePowerEvent,
         STMCfg_DeviceSysRequest
      },

      STMCfg_STMControl,
      STMCfg_STMStatus,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalSTMCfgHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalSTMCfgHandle.pVtbl  = &vtbl;
   pclientCtxt->DalSTMCfgHandle.pClientCtxt = pclientCtxt;
}

DALResult
STMCfg_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static STMCfgDrvCtxt drvCtxt = {{STMCfg_DriverInit,
                                      STMCfg_DriverDeInit
                                     },1,
                                     sizeof(STMCfgDevCtxt)};
   static STMCfgClientCtxt clientCtxt;

   STMCfgClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      STMCfg_InitInterface(pclientCtxt);
      STMCfg_AddRef(&(pclientCtxt->DalSTMCfgHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSTMCfgHandle);
   }
   return nErr;
}

