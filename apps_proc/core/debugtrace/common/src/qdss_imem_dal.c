/*=============================================================================

FILE:         qdss_imem_dal.c

DESCRIPTION:

================================================================================
              Copyright (c) 2013, 2019 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
$ Header: $
  when       who      what, where, why
--------   -------    ----------------------------------------------------------
02/23/19     kdey     Changes for IMEM address and SNOC BIMC clocks
==============================================================================*/
#include "qdss_imem.h"
#include "DDIChipInfo.h"
#include "DALSys.h"
#include "qdss_imem_config.h"
#include "qdss_throw.h"

#include "qurt_memory.h"
#include "qurt_error.h"


typedef struct {
   void * hMem;
   QDSSMemInfo   memInfo;
   uint32 imem_base;
}qdss_imem_t;

static qdss_imem_t qdss_imem;

static DALResult qdss_imem_memmap(uint32 physAddr,
                                  uint32 size,
                                  void *phMem,
                                  QDSSMemInfo *pMemInfo)
{
   int nErr = DAL_ERROR;
   
   qurt_addr_t virtaddr_temp;
   if ( QURT_EOK != qurt_mem_map_static_query(&virtaddr_temp, physAddr, size, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
   {
	   qurt_mem_pool_t QdssIMemmap_pool;
	   qurt_mem_region_t QdssIMemmap_region;
	   qurt_mem_region_attr_t QdssIMemmap_region_attr;
	   QdssIMemmap_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
	   QdssIMemmap_region_attr.region_type = QURT_MEM_REGION_DEVICE;
	   QdssIMemmap_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
	   QdssIMemmap_region_attr.permission = QURT_PERM_FULL;
	   QdssIMemmap_region_attr.physaddr = physAddr;
	   QdssIMemmap_region_attr.virtaddr = (qurt_addr_t)-1;
	   QdssIMemmap_region_attr.size = size;
	   
	   TRY(nErr,qurt_mem_pool_attach("HWIO_POOL", &QdssIMemmap_pool));
	   TRY(nErr, qurt_mem_region_create(&QdssIMemmap_region, size, QdssIMemmap_pool, &QdssIMemmap_region_attr ));
	   TRY(nErr,qurt_mapping_create (QdssIMemmap_region_attr.virtaddr, QdssIMemmap_region_attr.physaddr, QdssIMemmap_region_attr.size, QdssIMemmap_region_attr.cache_attribs, QdssIMemmap_region_attr.permission));
	   TRY(nErr, qurt_mem_region_attr_get(QdssIMemmap_region, &QdssIMemmap_region_attr));
	   qdss_imem.memInfo.VirtualAddr = QdssIMemmap_region_attr.virtaddr;
	
		if (QdssIMemmap_region_attr.physaddr != physAddr) {
		//could happen if our address is not 4K aligned
		uint32 offset= physAddr & 0xFFF; //our offset from the  4K page

		pMemInfo->VirtualAddr    += offset;
		pMemInfo->PhysicalAddr   += offset;
		pMemInfo->dwLen          -= offset;
		}
	}		
	else
	{
		qdss_imem.memInfo.VirtualAddr = (uint32 )virtaddr_temp;
	}

      
   nErr = DAL_SUCCESS;
   
   CATCH(nErr) {}
   
   return nErr;
}


#define QDSS_IMEM_PAGE_SIZE 0x1000

uint32 qdss_imem_base(void)
{
   DALDEVICEHANDLE  hChipInfo = 0;
   DalChipInfoFamilyType  chip_family;
   DalChipInfoVersionType chip_version;
   int nErr;
   uint32 imem_base = qdss_imem.imem_base;

   if (QDSS_INVALID_IMEM_ADDR != imem_base) {
      return imem_base; //Already have it just return
   }

   TRY(nErr,DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo));
   TRY(nErr,DalDevice_Open(hChipInfo, DAL_OPEN_SHARED));
   TRY(nErr,DalChipInfo_GetChipFamily(hChipInfo, &chip_family));
   TRY(nErr,DalChipInfo_GetChipVersion(hChipInfo, &chip_version));


   switch (chip_family) {
   case DALCHIPINFO_FAMILY_MSM8974:
      if (chip_version >= DALCHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT;
      }
      else {
         //msg ram workaround for v1
         imem_base=QDSS_IMEM_BASE_BADGER_MSGRAM;
      }
      break;

   case DALCHIPINFO_FAMILY_MDM9x25:
      if (chip_version >= DALCHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_9x25_V2;
      }
      else {
         //msg ram workaround for 9x25 v1
         imem_base=QDSS_IMEM_BASE_9x25_V1;
      }
      break;

   case DALCHIPINFO_FAMILY_MSM8x10:
   case DALCHIPINFO_FAMILY_MSM8x26:
      imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT;
      break;

   case DALCHIPINFO_FAMILY_APQ8064:
   case DALCHIPINFO_FAMILY_MDM9x35:
      imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT; //These not yet in the IMEM doc
      break;

   case DALCHIPINFO_FAMILY_MSM8916:
      imem_base=QDSS_IMEM_BASE_BEAR_DEFAULT;
      break;


   case DALCHIPINFO_FAMILY_MDM9x45:
   case DALCHIPINFO_FAMILY_MDM9x07:
      imem_base=QDSS_IMEM_BASE_BEAR_DEFAULT;
      break;

   case DALCHIPINFO_FAMILY_MDM9205:
     imem_base=QDSS_IMEM_BASE_TWIZY_DEFAULT;
     break;

   default:
      imem_base=QDSS_INVALID_IMEM_ADDR;
      THROW(nErr,DAL_ERROR);
   }


   TRY(nErr,qdss_imem_memmap(imem_base,
                             QDSS_IMEM_PAGE_SIZE,
                             &qdss_imem.hMem,
                             &qdss_imem.memInfo));

   imem_base = qdss_imem.memInfo.VirtualAddr;
   imem_base += QDSS_IMEM_QDSS_OFFSET;


   CATCH(nErr) {
      imem_base = QDSS_INVALID_IMEM_ADDR;
   }

   if (0!=hChipInfo) {
      DalDevice_Close(hChipInfo);
      DAL_DeviceDetach(hChipInfo);
   }

   qdss_imem.imem_base = imem_base;
   return imem_base;
}


uint32 qdss_shared_imem_base(void)
{
   uint32 qdss_imem_addr= qdss_imem_base();
   if (QDSS_INVALID_IMEM_ADDR == qdss_imem_addr) {
      return QDSS_INVALID_IMEM_ADDR;
   }
   return (qdss_imem_addr - QDSS_IMEM_QDSS_OFFSET);
}
