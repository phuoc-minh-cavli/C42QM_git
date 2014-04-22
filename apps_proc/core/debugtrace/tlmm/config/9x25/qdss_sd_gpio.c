/*=============================================================================

FILE:         qdss_sd_gpio.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2013 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "DDISpmi.h"

static __inline int qdss_sd_gpio_init(DALDEVICEHANDLE *phSPMI)
{
   int nErr = DAL_ERROR;
   DALDEVICEHANDLE hSPMI;

   TRY(nErr, DAL_StringDeviceAttach("DALDEVICEID_SPMI_DEVICE",
                                       &hSPMI));
   TRY(nErr, DalDevice_Open(hSPMI, DAL_OPEN_SHARED));

   *phSPMI=hSPMI;

   CATCH(nErr) {}
   return nErr;
}

static __inline int qdss_sd_gpio_enable(DALDEVICEHANDLE hSPMI)
{
   int nErr = DAL_ERROR;
   unsigned char ucVal;

   // Set GPIO4 to Enable SD Card
   ucVal = QDSS_SPMI_GPIO4_MODE_CTL_SD_VAL;
   TRY(nErr,DalSpmi_WriteLong(hSPMI, 0,
                              SPMI_BUS_ACCESS_PRIORITY_LOW,
                              QDSS_SPMI_GPIO4_MODE_CTL_ADDR, &ucVal, 1));

   CATCH(nErr) {}
   return nErr;
}

static __inline int qdss_sd_gpio_disable(DALDEVICEHANDLE hSPMI)
{
   int nErr = DAL_ERROR;
   unsigned char ucVal;

   ucVal = QDSS_SPMI_GPIO4_MODE_CTL_POR_VAL;
   TRY(nErr,DalSpmi_WriteLong(hSPMI, 0,
                              SPMI_BUS_ACCESS_PRIORITY_LOW,
                              QDSS_SPMI_GPIO4_MODE_CTL_ADDR, &ucVal, 1));

   CATCH(nErr) {}
   return nErr;
}



static int qdss_chip_sdio_init(void) 
{
   int nErr = DAL_ERROR;

   if (NULL == qdss_tlmm.hGPIO)
   {
      TRY(nErr,qdss_sd_gpio_init(&qdss_tlmm.hGPIO));
   }

   TRY(nErr,qdss_dal_memmap(QDSS_TLMM_BASE_PHYS + QDSS_TLMM_OFFSET,
                            QDSS_TLMM_SIZE,
                            DALSYS_MEM_PROPS_HWIO,
                            &qdss_tlmm.mem_handle,
                            &qdss_tlmm.mem_info));
   HAL_qdss_tlmm_HalConfigInit(
      qdss_tlmm.mem_info.VirtualAddr - QDSS_TLMM_OFFSET);

   CATCH(nErr) {}
   return nErr;
}

static int qdss_chip_sdio_enable(void) 
{

   // Set ETM mode and SDC2
   HAL_qdss_tlmm_SetEtmMode(QDSS_TLMM_ETM_MODE_ON_VAL);
   HAL_qdss_tlmm_SetSdc2HdrvPullCtl(QDSS_TLMM_SDC2_HDRV_PULL_CTL_ON_VAL);
   return qdss_sd_gpio_enable(qdss_tlmm.hGPIO);
}


static int qdss_chip_sdio_disable(void) 
{
   qdss_sd_gpio_disable(qdss_tlmm.hGPIO);
   HAL_qdss_tlmm_EtmMode_POR();
   HAL_qdss_tlmm_Sdc2HdrvPullCtl_POR();
   return DAL_SUCCESS;
}

