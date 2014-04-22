/*=============================================================================
  Copyright (c) 2015, 2019 Qualcomm Technologies Incorporated.
           All Rights Reserved.
$Header: //components/rel/core.tx/6.0/debugtrace/tpdm/dal/src/DalTPDM.c#1 $
=============================================================================*/

#include "stringl/stringl.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITPDM.h"
#include "DalTPDM.h"
#include "qdss_throw.h"
#include "HALhwio.h"
#include "hal_qdss_tpdm.h"

#include "qurt_memory.h"
#include "qurt_error.h"
#include "qdss_control.h"
#include <stdlib.h>


#define TPDM_PAGE_SIZE 4096


/*------------------------------------------------------------------------------
Declaring a "TPDM" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TPDMDrvCtxt_Type TPDMDrvCtxt;
typedef struct TPDMDevCtxt_Type TPDMDevCtxt;
typedef struct TPDMClientCtxt_Type TPDMClientCtxt;

/*---------------------------------------------------------------------------
Declaring a private "TPDM" Vtable
------------------------------------------------------------------------------*/
typedef struct TPDMDALVtbl TPDMDALVtbl;
struct TPDMDALVtbl
{
   int (*TPDM_DriverInit)(TPDMDrvCtxt *);
   int (*TPDM_DriverDeInit)(TPDMDrvCtxt *);
};

struct TPDMDevCtxt_Type
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   TPDMDrvCtxt  *pTPDMDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //TPDM Dev state can be added by developers here
   DALSYSMemHandle *hMem; //pointer to memory handles
   uint32 *pVirtAddr;
   TPDMDeviceProp *pDeviceAttributes;
   uint32 attribTableLen;
   uint32 attribIndex;
   boolean useStrLookUp;
};

struct TPDMDrvCtxt_Type
{
   //Base Members
   TPDMDALVtbl TPDMDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   TPDMDevCtxt TPDMDevCtxt[1];
   //TPDM Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "TPDM" Client Context
------------------------------------------------------------------------------*/
struct TPDMClientCtxt_Type
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TPDMDevCtxt *pTPDMDevCtxt;
   DalTPDMHandle DalTPDMHandle;
   //TPDM Client state can be added by developers here
};



/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
TPDM_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALTPDM_DriverInfo = { TPDM_DeviceAttach, 0, NULL};


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

static uint32
TPDM_AddRef(DalTPDMHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
TPDM_DriverInit(TPDMDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
TPDM_DriverDeInit(TPDMDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
TPDM_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult TPDM_MemMap(uint32 physAddr,
                            DALSYSMemHandle *phMem,
                            uint32 *virtAddr)
{

   uint32 i =0;
   int nErr;
   DALResult result;
   qurt_mem_region_t TPDM_MemMap_region;
   qurt_mem_region_attr_t TPDM_MemMap_region_attr;
   qurt_mem_pool_t TPDM_Pool;
   QDSSMemInfo memInfo;

   qurt_mem_region_attr_init(&TPDM_MemMap_region_attr);

   if (QURT_EOK != qurt_mem_map_static_query(&memInfo.VirtualAddr, physAddr, TPDM_PAGE_SIZE, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
   {
     TPDM_MemMap_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
     TPDM_MemMap_region_attr.region_type = QURT_MEM_REGION_DEVICE;
     TPDM_MemMap_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
     TPDM_MemMap_region_attr.permission = QURT_PERM_FULL;
     TPDM_MemMap_region_attr.physaddr = (qurt_addr_t)physAddr;
     TPDM_MemMap_region_attr.virtaddr = (qurt_addr_t)-1;
     TPDM_MemMap_region_attr.size = TPDM_PAGE_SIZE;
   
     TRY(nErr,qurt_mem_pool_attach("HWIO_POOL", &TPDM_Pool));
     TRY(nErr,qurt_mem_region_create(&TPDM_MemMap_region, TPDM_MemMap_region_attr.size, TPDM_Pool, &TPDM_MemMap_region_attr));
     TRY(nErr,qurt_mem_region_attr_get(TPDM_MemMap_region, &TPDM_MemMap_region_attr));

     TPDM_MemMap_region_attr.physaddr = (qurt_addr_t)physAddr;
     TPDM_MemMap_region_attr.region_type = QURT_MEM_REGION_DEVICE;
     TPDM_MemMap_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
     TPDM_MemMap_region_attr.permission = QURT_PERM_FULL;
   
     TRY(nErr,qurt_mapping_create(TPDM_MemMap_region_attr.virtaddr, TPDM_MemMap_region_attr.physaddr, TPDM_MemMap_region_attr.size, TPDM_MemMap_region_attr.cache_attribs, TPDM_MemMap_region_attr.permission));
     *virtAddr = TPDM_MemMap_region_attr.virtaddr;
   }
   else
   {
     *virtAddr = memInfo.VirtualAddr;
   }
   nErr = DAL_SUCCESS;
   CATCH(nErr) {}
   return nErr;
   /*int nErr;
   DALSYSMemInfo memInfo;


   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  TPDM_PAGE_SIZE, phMem, NULL ));
   DALSYS_MemInfo(*phMem, &memInfo);
   *virtAddr = memInfo.VirtualAddr;
   CATCH(nErr) {}

   return nErr;*/
}


static DALResult
TPDM_DeviceInit(DalDeviceHandle *h)
{
   DALClientCtxt * pClientCtxt = (DALClientCtxt *)(h->pClientCtxt);
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *)(h->pClientCtxt))->pDALDevCtxt;
   TPDMDeviceProp *pDeviceAttributes;
   TableLength *pAttribListLen;
   DALSYSPropertyVar PropVar;
   uint32 nErr,i;
   
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( pClientCtxt->pDALDevCtxt->strDeviceName,
                                                    pDevCtxt->hProp ))
       return DAL_ERROR;

   if (DAL_SUCCESS!=DALSYS_GetPropertyValue( pDevCtxt->hProp,"tpdm_device_attributes",0,&PropVar ))
         return DAL_ERROR;

   pDeviceAttributes = (TPDMDeviceProp *)PropVar.Val.pStruct;
   pDevCtxt->pDeviceAttributes = pDeviceAttributes;
   
   if (DAL_SUCCESS!=DALSYS_GetPropertyValue(pDevCtxt->hProp,"tpdm_device_attributes_len",0,&PropVar ))
            return DAL_ERROR;
   pAttribListLen=(TableLength *)PropVar.Val.pStruct;
   pDevCtxt->attribTableLen=pAttribListLen[0].length;

   pDevCtxt->hMem = (void*)malloc(pDevCtxt->attribTableLen*sizeof(DALSYSMemHandle));
   if (pDevCtxt->hMem==NULL)
      return DAL_ERROR;

   pDevCtxt->pVirtAddr = (void *)malloc(pDevCtxt->attribTableLen*sizeof(uint32));
   if (pDevCtxt->pVirtAddr==NULL)
      return DAL_ERROR;
      

   for (i=0;i<pDevCtxt->attribTableLen;i++)
   {
      nErr=TPDM_MemMap(pDeviceAttributes[i].phys_addr,&pDevCtxt->hMem[i],&pDevCtxt->pVirtAddr[i]);
      if (nErr!=DAL_SUCCESS)
         return DAL_ERROR;
   }

   pDevCtxt->useStrLookUp=TRUE;   

//Determine size of list
//Perform memory map
//Vote for TPDM clocks when enabling - Disable clocks when detach occurs
   return DAL_SUCCESS;
}


static DALResult
TPDM_DeviceDeInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
TPDM_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
TPDM_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{

   return DAL_SUCCESS;
}

static DALResult
TPDM_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
TPDM_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTPDM_STRING_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
TPDM_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTPDM Interface.
------------------------------------------------------------------------------*/
static DALResult _TPDM_GetAttribIndex(DalDeviceHandle *h,const char 
*block,uint32 *pIndex)
{
   TPDMDevCtxt          *pDevCtxt;
   uint32 i,len;
   uint32 ret=DAL_TPDM_INVALID_BLOCK;
   pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *) (h->pClientCtxt))->pDALDevCtxt;
   len=pDevCtxt->attribTableLen;
   
   for (i = 0; i < len; i++) {
       if (0==strncmp(block,
                      pDevCtxt->pDeviceAttributes[i].tpdm_name,
                      TPDM_NAME_LEN)) {
           *pIndex= i;
           ret=DAL_SUCCESS;
           break;
       }
   }
   
   /* Did not find block name in array */
   return ret;
}


static DALResult _TPDM_GetVirtAddr(DalDeviceHandle *h,
                                   const char *block,
                                   uint32 *virt_addr,
                                   uint32 *pIndex)
{
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *) (h->pClientCtxt))->pDALDevCtxt;
   uint32   index = 0;
   int  nErr = DAL_TPDM_INVALID_BLOCK;
   //DALSYSMemInfo mem_info;
   
   TRY(nErr,_TPDM_GetAttribIndex(h,block,&index));
   //TRY(nErr,DALSYS_MemInfo(pDevCtxt->hMem[index],&mem_info));
   *virt_addr = (uint32)pDevCtxt->pVirtAddr[index];
   *pIndex = index;

   CATCH(nErr) {}
   return nErr;
}

static DALResult _TPDM_SetBaseAddress(DalDeviceHandle *h, const char *block, uint32 *pIndex)
{
    DALResult   err;
    uint32      virt_addr;

    TRY(err, _TPDM_GetVirtAddr(h, block, &virt_addr,pIndex));
    HAL_qdss_tpdm_SetBaseAddress(virt_addr);

    CATCH(err) {}
    return err;
}

static void TPDM_DSB_Defaults(void) 
{
   static DALBOOL bInitialized = FALSE;
   int i;

   if (!bInitialized) {
      HAL_qdss_tpdm_dsb_set_patt_type(1); //OR pattern
      HAL_qdss_tpdm_dsb_set_patt_match(1);//Enable timsestamp generation on pattern match
      //Timestamp is needed for all DSB inputs
      for (i=0; i < 8; i++) {
         HAL_qdss_tpdm_dsb_set_tpmr(i,0xFFFFFFFF);
      }
      bInitialized = TRUE;
   }
}


static void TPDM_CMB_Defaults(void) 
{
   static DALBOOL bInitialized = FALSE;

   if (!bInitialized) {
      HAL_qdss_tpdm_cmb_set_mode(1);
      HAL_qdss_tpdm_cmb_set_ext_trigger_on_off(1);
      bInitialized = TRUE;
   }
}



static DALResult TPDM_EnableDisable(DalDeviceHandle *phandle,const char *pTPDMStr,uint32 
TPDMTypeReq,DALBOOL bEnable)
{
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   uint32 i,setbit;
   uint32 tpdm_types;

   if (DAL_SUCCESS!=_TPDM_SetBaseAddress(phandle,pTPDMStr,&i)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   tpdm_types = pDevCtxt->pDeviceAttributes[i].tpdm_types;
   if((TPDMTypeReq|tpdm_types) != tpdm_types ) {
      return DAL_TPDM_INCORRECT_OPTION;
   }

   TPDMTypeReq=TPDMTypeReq&tpdm_types;
   setbit=(bEnable==TRUE)?1:0;

   HAL_qdss_tpdm_unlock_access();

   if (TPDMTypeReq&(TPDM_CMB|TPDM_MCMB)) {
      TPDM_CMB_Defaults();
      HAL_qdss_tpdm_cmb_set_enable_disable(setbit);
   }
            
   if (TPDMTypeReq&TPDM_DSB) {
      TPDM_DSB_Defaults();
      HAL_qdss_tpdm_dsb_set_enable_disable(setbit);
   }
            
   if  (TPDMTypeReq&TPDM_BC) {
      HAL_qdss_tpdm_bc_set_enable_disable(setbit);
   }

   if (TPDMTypeReq&TPDM_TC) {
      HAL_qdss_tpdm_tc_set_enable_disable(setbit);
   }
   
   if (TPDMTypeReq&TPDM_IMPLDEF) {
      HAL_qdss_tpdm_impldef_set_enable_disable(setbit);
   }

   HAL_qdss_tpdm_lock_access();
   return DAL_TPDM_SUCCESS;
}

static int TPDM_AccessBegin(DalDeviceHandle *phandle,const char *pTPDMStr, uint32 type) 
{
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   int nErr = DAL_ERROR;
   uint32 index;

   TRY(nErr,_TPDM_SetBaseAddress(phandle,pTPDMStr,&index));

   THROW_IF(nErr,(!(pDevCtxt->pDeviceAttributes[index].tpdm_types & type)),
            DAL_TPDM_INVALID_BLOCK);

   nErr= DAL_SUCCESS;
   HAL_qdss_tpdm_unlock_access();

   CATCH(nErr) {}
   return nErr;
}

static void TPDM_AccessEnd(void) 
{
   HAL_qdss_tpdm_lock_access();
}

static DALResult TPDM_CMB_GetParam(DalDeviceHandle *phandle,const char *pTPDMStr,
                                   uint8 param_id,uint32 *pval)
{
   DALResult ret=DAL_SUCCESS;
   uint32 val = 0;

   if(DAL_SUCCESS!= TPDM_AccessBegin(phandle,pTPDMStr,TPDM_CMB)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   switch(param_id) {
   case TPDM_CONTROL_CMB_MODE:
      val = HAL_qdss_tpdm_cmb_get_mode();
      break;
   
   case TPDM_CONTROL_CMB_FLOWCTRL_ON_OFF:
      val = HAL_qdss_tpdm_cmb_get_flow_control();
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MATCH_ON_OFF:
      val = HAL_qdss_tpdm_cmb_get_patt_match();
      break;
         
   case TPDM_CONTROL_CMB_TS_EXT_TRIGGER_ON_OFF:
      val = HAL_qdss_tpdm_cmb_get_ext_trigger_on_off();
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_VAL_LSW:
      val = HAL_qdss_tpdm_cmb_get_ts_patt_val_lsw();
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_VAL_MSW:         
      val = HAL_qdss_tpdm_cmb_get_ts_patt_val_msw();
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MASK_LSW:         
      val = HAL_qdss_tpdm_cmb_get_ts_patt_mask_lsw();
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MASK_MSW:         
      val = HAL_qdss_tpdm_cmb_get_ts_patt_mask_msw();
      break;

   case TPDM_CONTROL_MCMB_LANES_SELECT:         
      val = HAL_qdss_tpdm_cmb_get_mcmb_lanes();
      break;
      
   case TPDM_CONTROL_MCMB_CA_MODE:         
      val = HAL_qdss_tpdm_cmb_get_mcmb_ca_mode();
      break;
      
   case TPDM_CONTROL_MCMB_CA_XTRIG_LNSEL:         
      val = HAL_qdss_tpdm_cmb_get_mcmb_ca_xtrig_lnsel();
      break;

   case TPDM_CONTROL_CMB_TS_ALL:         
      val = HAL_qdss_tpdm_cmb_get_ts_all();
      break;
         
   default:
      ret=DAL_TPDM_INCORRECT_OPTION; 
   }

   *pval = val;
   TPDM_AccessEnd();
   return ret;
}




static DALResult TPDM_CMB_SetParam(DalDeviceHandle *phandle,const char *pTPDMStr,
                                   uint8 param_id,uint32 val)
{
   DALResult ret=DAL_SUCCESS;
   uint32 state;

   if(DAL_SUCCESS!= TPDM_AccessBegin(phandle,pTPDMStr,TPDM_CMB)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   //save state and disable
   state = HAL_qdss_tpdm_cmb_get_enable_disable();
   HAL_qdss_tpdm_cmb_set_enable_disable(0);

   switch(param_id) {
   case TPDM_CONTROL_CMB_MODE:
      HAL_qdss_tpdm_cmb_set_mode(val&0x1);
      break;
   
   case TPDM_CONTROL_CMB_FLOWCTRL_ON_OFF:
      HAL_qdss_tpdm_cmb_set_flow_control(val&0x1);
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MATCH_ON_OFF:
      HAL_qdss_tpdm_cmb_set_patt_match(val&0x1);
      break;
         
   case TPDM_CONTROL_CMB_TS_EXT_TRIGGER_ON_OFF:
      HAL_qdss_tpdm_cmb_set_ext_trigger_on_off(val&0x1);
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_VAL_LSW:
      HAL_qdss_tpdm_cmb_set_ts_patt_val_lsw(val);
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_VAL_MSW:         
      HAL_qdss_tpdm_cmb_set_ts_patt_val_msw(val);
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MASK_LSW:         
      HAL_qdss_tpdm_cmb_set_ts_patt_mask_lsw(val);
      break;
         
   case TPDM_CONTROL_CMB_TS_PATT_MASK_MSW:         
      HAL_qdss_tpdm_cmb_set_ts_patt_mask_msw(val);
      break;

   case TPDM_CONTROL_MCMB_LANES_SELECT:         
      HAL_qdss_tpdm_cmb_set_mcmb_lanes(val);
      break;
      
   case TPDM_CONTROL_MCMB_CA_MODE:         
      HAL_qdss_tpdm_cmb_set_mcmb_ca_mode(val);
      break;

   case TPDM_CONTROL_MCMB_CA_XTRIG_LNSEL:         
      HAL_qdss_tpdm_cmb_set_mcmb_ca_xtrig_lnsel(val);
      break;

   case TPDM_CONTROL_CMB_TS_ALL:         
      HAL_qdss_tpdm_cmb_set_ts_all(val);
      break;
      

   default:
      ret=DAL_TPDM_INCORRECT_OPTION; 
   }

   HAL_qdss_tpdm_cmb_set_enable_disable(state);

   TPDM_AccessEnd();
   return ret;
}


static DALResult TPDM_DSB_GetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 *pval)
{
   DALResult ret=DAL_SUCCESS;
   uint32 val = 0;

   if(DAL_SUCCESS!= TPDM_AccessBegin(phandle,pTPDMStr,TPDM_DSB)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   
   switch(param_id) {
   case TPDM_CONTROL_DSB_MODE:
      val=HAL_qdss_tpdm_dsb_get_mode();
      break;

   case TPDM_CONTROL_DSB_TS_EXT_TRIGGER_ON_OFF:
      val=HAL_qdss_tpdm_dsb_get_ext_trigger_on_off();
      break;

   case TPDM_CONTROL_DSB_TS_PATT_MATCH_ON_OFF:
     val=HAL_qdss_tpdm_dsb_get_patt_match();
      break;

   default:
      ret = DAL_TPDM_INCORRECT_OPTION;
   }

   TPDM_AccessEnd();
   *pval = val;
   return ret;
}



static DALResult TPDM_DSB_SetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 val)
{
   DALResult ret=DAL_SUCCESS;
   uint32 state;

   if(DAL_SUCCESS!= TPDM_AccessBegin(phandle,pTPDMStr,TPDM_DSB)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   //save state and ensure disabled
   state = HAL_qdss_tpdm_dsb_get_enable_disable();
   HAL_qdss_tpdm_dsb_set_enable_disable(0);
   
   switch(param_id) {
   case TPDM_CONTROL_DSB_MODE:
      HAL_qdss_tpdm_dsb_set_mode(val & 0x1);
      break;

   case TPDM_CONTROL_DSB_TS_EXT_TRIGGER_ON_OFF:
      HAL_qdss_tpdm_dsb_set_ext_trigger_on_off(val&0x1);
      break;

   case TPDM_CONTROL_DSB_TS_PATT_MATCH_ON_OFF:
      HAL_qdss_tpdm_dsb_set_patt_match(val&0x1);
      break;

   default:
      ret = DAL_TPDM_INCORRECT_OPTION;
   }

   HAL_qdss_tpdm_dsb_set_enable_disable(state);
   TPDM_AccessEnd();
   return ret;
}





static DALResult TPDM_DSB_SetEDCMR(DalDeviceHandle *phandle, const char *pTPDMStr,
                                   uint8 start, uint8 end, uint8 state) 
{
   DALResult ret=DAL_SUCCESS;
   uint32 saved_dsb_state;

   if(DAL_SUCCESS!= TPDM_AccessBegin(phandle,pTPDMStr,TPDM_DSB)) {
      return DAL_TPDM_INVALID_BLOCK;
   }

   //save state and ensure disabled
   saved_dsb_state = HAL_qdss_tpdm_dsb_get_enable_disable();
   HAL_qdss_tpdm_dsb_set_enable_disable(0);
   {
      uint8 index,start_index,end_index;
      uint8 start_bit,end_bit;

      uint32 start_word,end_word;
      uint32 original_start_word, original_end_word;

      THROW_IF(ret,
               (start >end),
               DAL_TPDM_INCORRECT_OPTION);      

      start_index=start >> 5;
      end_index=end >> 5;

      original_start_word =  HAL_qdss_tpdm_dsb_get_edcmr(start_index);
      original_end_word = HAL_qdss_tpdm_dsb_get_edcmr(end_index);

      start_bit=start & 0x1F;
      end_bit=end & 0x1F;

      //bit mask for start word and end word
      start_word=~((1<<start_bit)-1);
      end_word=(31==end_bit)?0xffffffff:(1<<(end_bit+1))-1;
      

      if (state) {
         //enabling 
         start_word |= original_start_word;
         end_word |= original_end_word;

         if (start_index==end_index) {
            start_word=end_word=(start_word & end_word);
         }

         HAL_qdss_tpdm_dsb_set_edcmr(start_index,start_word);

         for (index=start_index+1; index < end_index; index++) {
            HAL_qdss_tpdm_dsb_set_edcmr(index,0xffffffff);
         }

         if (start_index!=end_index) {
            HAL_qdss_tpdm_dsb_set_edcmr(end_index,end_word);
         }
      }
      else {
         //disabling
         start_word = ~start_word;
         end_word   = ~end_word;

         start_word &= original_start_word;
         end_word &= original_end_word;

         if (start_index==end_index) {
            start_word=end_word=(start_word | end_word);
         }
         HAL_qdss_tpdm_dsb_set_edcmr(start_index,start_word);

         for (index=start_index+1; index < end_index; index++) {
            HAL_qdss_tpdm_dsb_set_edcmr(index,0);
         }

         if (start_index!=end_index) {
            HAL_qdss_tpdm_dsb_set_edcmr(end_index,end_word);
         }
      }
   }
   CATCH(ret) {}

   HAL_qdss_tpdm_dsb_set_enable_disable(saved_dsb_state);
   TPDM_AccessEnd();
   return ret;
}


static DALResult TPDM_Get_GPR(DalDeviceHandle *phandle,const char *pTPDMStr,
                              uint32 GPRIndex,uint32 reg_type,uint32 *pval)
{
   uint32 index;
   int nErr = DAL_SUCCESS;
   uint32 val = 0;

   if (DAL_SUCCESS!=_TPDM_SetBaseAddress(phandle,pTPDMStr,&index)) {
      return DAL_TPDM_INVALID_BLOCK;
   }
   HAL_qdss_tpdm_unlock_access();
   switch (reg_type) {
   case 0:
      nErr=HAL_qdss_tpdm_GetGPR(GPRIndex,&val);
      break;
   case 1:
      nErr=HAL_qdss_tpdm_GetDSBMSR(GPRIndex,&val);
      break;
   case 2:
      nErr=HAL_qdss_tpdm_GetCMBMSR(GPRIndex,&val);
      break;
   default:
      nErr = DAL_TPDM_INCORRECT_OPTION;
      break;
   }

   HAL_qdss_tpdm_lock_access();
   *pval = val;
   return nErr;
}


static DALResult TPDM_Set_GPR(DalDeviceHandle *phandle,const char *pTPDMStr,
                              uint32 GPRIndex,uint32 reg_type,uint32 val)
{
   uint32 index;
   int nErr = DAL_SUCCESS;

   if (DAL_SUCCESS!=_TPDM_SetBaseAddress(phandle,pTPDMStr,&index)) {
      return DAL_TPDM_INVALID_BLOCK;
   }
   HAL_qdss_tpdm_unlock_access();
   switch (reg_type) {
   case 0:
      nErr=HAL_qdss_tpdm_SetGPR(GPRIndex,val);
      break;
   case 1:
      nErr=HAL_qdss_tpdm_SetDSBMSR(GPRIndex,val);
      break;
   case 2:
      nErr=HAL_qdss_tpdm_SetCMBMSR(GPRIndex,val);
      break;
   default:
      nErr = DAL_TPDM_INCORRECT_OPTION;
      break;
   }
   HAL_qdss_tpdm_lock_access();
   return nErr;
}
   
static DALResult TPDM_Control(DalDeviceHandle *phandle,const char *pTPDMStr, uint8 param_id,uint32 val)
{
   return DAL_ERROR;
}


DALResult TPDM_GetCompSupported(DalDeviceHandle *phandle,const char *pTPDMStr, uint32 *pVal)
{
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   DALResult ret=DAL_ERROR;
   uint32 index;

   TRY(ret,_TPDM_GetAttribIndex(phandle,pTPDMStr,&index));
   *pVal=pDevCtxt->pDeviceAttributes[index].tpdm_types;
   ret=DAL_SUCCESS;

   CATCH(ret) {}

   return ret;
}


DALResult TPDM_GetTPDA(DalDeviceHandle *phandle,const char *pTPDMStr,const char **ppTPDAVal)
{
   TPDMDevCtxt *pDevCtxt = (TPDMDevCtxt *)((DALClientCtxt *)(phandle->pClientCtxt))->pDALDevCtxt;
   DALResult ret=DAL_ERROR;
   uint32 index;
   TRY(ret,_TPDM_GetAttribIndex(phandle,pTPDMStr,&index));

   *ppTPDAVal=pDevCtxt->pDeviceAttributes[index].tpda_name;
   
   ret=DAL_SUCCESS;

   CATCH(ret) {}
   return ret;
}

static void
TPDM_InitInterface(TPDMClientCtxt* pclientCtxt)
{
 static const DalTPDM vtbl =
   {
      {
         TPDM_DeviceAttach,
         TPDM_DeviceDetach,
         TPDM_DeviceInit,
         TPDM_DeviceDeInit,
         TPDM_DeviceOpen,
         TPDM_DeviceClose,
         TPDM_DeviceInfo,
         TPDM_DevicePowerEvent,
         TPDM_DeviceSysRequest
      },
      TPDM_EnableDisable,
      TPDM_Control,
      TPDM_GetCompSupported,
      TPDM_GetTPDA,

      TPDM_Get_GPR,
      TPDM_Set_GPR,

      TPDM_DSB_SetEDCMR,
      TPDM_DSB_GetParam,
      TPDM_DSB_SetParam,

      TPDM_CMB_GetParam,
      TPDM_CMB_SetParam
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalTPDMHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalTPDMHandle.pVtbl  = &vtbl;
   pclientCtxt->DalTPDMHandle.pClientCtxt = pclientCtxt;
}

DALResult
TPDM_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static TPDMDrvCtxt drvCtxt = {{TPDM_DriverInit,
                                 TPDM_DriverDeInit
                                },1,
                                sizeof(TPDMDevCtxt)};
   static TPDMClientCtxt clientCtxt;

   TPDMClientCtxt *pclientCtxt = &clientCtxt;


   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                        (DALClientCtxt *)pclientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      TPDM_InitInterface(pclientCtxt);
      TPDM_AddRef(&(pclientCtxt->DalTPDMHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTPDMHandle);
   }
   return nErr;
}
