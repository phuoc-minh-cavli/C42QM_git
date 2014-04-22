/*=============================================================================

FILE:         qdss_sd_gpio.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "HALhwio.h"

#define HWIO_PAGE_SIZE 0x1000
#define HWIO_PAGE_BASE(nAddr)   ((nAddr) & ~(HWIO_PAGE_SIZE - 1))
#define HWIO_PAGE_OFFSET(nAddr) ((nAddr)& (HWIO_PAGE_SIZE - 1))


static int qdsd_reg_write(uint32 phys_addr, uint32 value) 
{
   int nErr;
   DALSYSMemHandle mem_handle;
   QDSSMemInfo   mem_info;
   uint32 virt_addr;


   TRY(nErr,qdss_dal_memmap(HWIO_PAGE_BASE(phys_addr),
                            HWIO_PAGE_SIZE,
                            0,
                            &mem_handle,
                            &mem_info));

   virt_addr=mem_info.VirtualAddr+HWIO_PAGE_OFFSET(phys_addr);
   out_dword(virt_addr,value);
  #if 0
   TRY(nErr,DALSYS_DestroyObject(mem_handle));
  #endif
   CATCH(nErr) {}
   return nErr;

}



static int qdss_chip_sdio_init(void) 
{

   return 0;
}

#define TLMM_QDSD_CONFIG_CTL           0x0119E000
#define TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL 0x0119C000

static int qdss_chip_sdio_enable(void) 
{
   int nErr;

   TRY(nErr,qdsd_reg_write(TLMM_QDSD_CONFIG_CTL,0x181));
   TRY(nErr,qdsd_reg_write(TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL,0x8a5294a5));
       
   CATCH(nErr){}

   return nErr;
}


static int qdss_chip_sdio_disable(void) 
{
   return 0;
}

