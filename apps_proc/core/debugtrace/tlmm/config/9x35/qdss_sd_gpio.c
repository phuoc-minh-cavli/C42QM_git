/*=============================================================================

  FILE:         qdss_sd_gpio.c

  DESCRIPTION:  

  ================================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ==============================================================================*/

#include "DDITlmm.h"

static __inline int qdss_sd_gpio_init(DALDEVICEHANDLE *phTlmm)
{
   int nErr = DAL_ERROR;
   DALDEVICEHANDLE hTlmm;

   TRY(nErr,DAL_DeviceAttach(DALDEVICEID_TLMM,&hTlmm));
   TRY(nErr, DalDevice_Open(hTlmm, DAL_OPEN_SHARED));

   *phTlmm = hTlmm;

   CATCH(nErr) {}
   return nErr;
}

static __inline int qdss_sd_gpio_enable(DALDEVICEHANDLE hTlmm)
{
   int nErr = DAL_ERROR;

   TRY(nErr, DalTlmm_GpioOut(hTlmm,
                             DAL_GPIO_CFG( 89, 0, DAL_GPIO_OUTPUT,
                                           DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                             DAL_GPIO_HIGH_VALUE));

   TRY(nErr, DalTlmm_ConfigGpio(hTlmm,
                                DAL_GPIO_CFG( 89, 0, DAL_GPIO_OUTPUT,
                                              DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                                DAL_TLMM_GPIO_ENABLE ));


   CATCH(nErr) {}
   return nErr;
}

static __inline int qdss_sd_gpio_disable(DALDEVICEHANDLE hTlmm)
{
   int nErr = DAL_ERROR;

   TRY(nErr, DalTlmm_GpioOut( hTlmm,
                              DAL_GPIO_CFG( 89, 0, DAL_GPIO_OUTPUT,
                                            DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                              DAL_GPIO_LOW_VALUE));

   TRY(nErr, DalTlmm_ConfigGpio( hTlmm,
                                 DAL_GPIO_CFG( 89, 0, DAL_GPIO_OUTPUT,
                                               DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                                 DAL_TLMM_GPIO_DISABLE ));

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

