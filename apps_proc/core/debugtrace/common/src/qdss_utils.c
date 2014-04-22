/*=============================================================================

FILE:         qdss_utils.c

DESCRIPTION:

=============================================================================*/
/*=============================================================================
  Copyright (c) 2011-13, 2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include "qdss_throw.h"
#include "qdss_utils.h"
#include "qurt_memory.h"
#include "qurt_error.h"


DALResult qdss_dal_memmap(uint32 physAddr,
                          uint32 size,
                          uint32 dwAttribs,
                          void *phMem,
                          QDSSMemInfo *pMemInfo)
{
   int nErr;
   
   qurt_addr_t virtaddr_temp;
   
   if ( QURT_EOK != qurt_mem_map_static_query(&virtaddr_temp, physAddr, size, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE))
   {
	   qurt_mem_pool_t QdssMemmap_pool;	
	   qurt_mem_region_t QdssMemmap_region;
	   qurt_mem_region_attr_t QdssMemmap_region_attr;
	   QdssMemmap_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
	   QdssMemmap_region_attr.region_type = QURT_MEM_REGION_DEVICE;
	   QdssMemmap_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
	   QdssMemmap_region_attr.permission = QURT_PERM_FULL;
	   QdssMemmap_region_attr.physaddr = physAddr;
	   QdssMemmap_region_attr.virtaddr = (qurt_addr_t)-1;
	   QdssMemmap_region_attr.size = size;
	   
	   if (0 == dwAttribs)
	   {
			TRY(nErr,qurt_mem_pool_attach("HWIO_POOL", &QdssMemmap_pool));
	   }
	   else
	   {
			TRY(nErr,qurt_mem_pool_attach("STATIC_POOL", &QdssMemmap_pool));
	   }
	   TRY(nErr,qurt_mem_region_create(&QdssMemmap_region, size, QdssMemmap_pool, &QdssMemmap_region_attr ));
	   TRY(nErr,qurt_mem_region_attr_get(QdssMemmap_region, &QdssMemmap_region_attr));
	   
	   QdssMemmap_region_attr.physaddr = physAddr;
	   QdssMemmap_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
	   QdssMemmap_region_attr.region_type = QURT_MEM_REGION_DEVICE;
	   QdssMemmap_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
	   QdssMemmap_region_attr.permission = QURT_PERM_FULL;
	   	   
	   TRY(nErr,qurt_mapping_create (QdssMemmap_region_attr.virtaddr, QdssMemmap_region_attr.physaddr, QdssMemmap_region_attr.size, QdssMemmap_region_attr.cache_attribs, QdssMemmap_region_attr.permission));
	   
	   pMemInfo->VirtualAddr = QdssMemmap_region_attr.virtaddr;
    }
	else
	{
		pMemInfo->VirtualAddr = (uint32) virtaddr_temp;
		nErr = QURT_EOK; 	
	}
	
   CATCH(nErr) {}
	
   return nErr;
}

