/*=============================================================================

FILE:         qdss_tlmm.c

DESCRIPTION:  QDSS interface to access TLMM, PMIC, and SPMI

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/tlmm/hal/src/qdss_tlmm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"

#include "pmapp_npa.h"
#include "npa.h"
#include "DALFramework.h"

#include "qdss_throw.h"
#include "qdss_utils.h"

#include "qdss_tlmm.h"
#include "halqdss_tlmm.h"
#include "qdss_tlmm_config.h"


struct qdss_tlmm_struct {
   boolean bInitialized;
   npa_client_handle npa_client;
   DALSYSMemHandle mem_handle;
   QDSSMemInfo   mem_info;
   DALDEVICEHANDLE hGPIO;
   DALDEVICEHANDLE hTlmm;
};

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

static struct qdss_tlmm_struct qdss_tlmm;


#include "qdss_sd_gpio.c"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internalized Function Definitions
 * ------------------------------------------------------------------------*/
int qdss_tlmm_init(void)
{
   int nErr;
   
   if (TRUE == qdss_tlmm.bInitialized) {
      return DAL_SUCCESS;
   }
   if (NULL == qdss_tlmm.npa_client)
   {
      qdss_tlmm.npa_client = npa_create_sync_client(
         PMIC_NPA_GROUP_ID_SD, "qdss", NPA_CLIENT_REQUIRED);
      THROW_IF(nErr, NULL == qdss_tlmm.npa_client, DAL_ERROR)
   }

   TRY(nErr,qdss_chip_sdio_init());

   qdss_tlmm.bInitialized = TRUE;
   nErr = DAL_SUCCESS;
   CATCH(nErr) {}

   return nErr;
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

int qdss_tlmm_sd_enable(void)
{
   int  nErr;
   TRY(nErr, qdss_tlmm_init());
   // Turn on LDO
   npa_issue_required_request(qdss_tlmm.npa_client,
                              PMIC_NPA_MODE_ID_SD_MODE_1);
   
   TRY(nErr,qdss_chip_sdio_enable());
   CATCH(nErr){}

   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_tlmm_sd_disable(void)
{
   if (TRUE == qdss_tlmm.bInitialized)
   {
      qdss_chip_sdio_disable();
      npa_issue_required_request(qdss_tlmm.npa_client,
                                 PMIC_NPA_MODE_ID_SD_MODE_OFF);
   }

   return DAL_SUCCESS;
}
/*-------------------------------------------------------------------------*/

int qdss_tlmm_config_cti_gpio(uint8 option)
{
   DALResult            nErr = DAL_SUCCESS;
   struct QDSS_CTI_GPIO qdss_cti_gpio;
   DALGpioSignalType    config_cti_gpio;
    
   if (option > (qdss_cti_gpios_len - 1))
   {
      THROW(nErr, DAL_ERROR);
   } 

   if (NULL == qdss_tlmm.hTlmm)
   {
      TRY(nErr, DAL_TlmmDeviceAttach(DALDEVICEID_TLMM, &qdss_tlmm.hTlmm));
   }

   qdss_cti_gpio = qdss_cti_gpios[option];
   config_cti_gpio = DAL_GPIO_CFG(qdss_cti_gpio.num,
                                    qdss_cti_gpio.function,
                                    qdss_cti_gpio.direction,
                                    DAL_GPIO_NO_PULL,
                                    DAL_GPIO_2MA);

   TRY(nErr, DalTlmm_ConfigGpio(qdss_tlmm.hTlmm,
                                    config_cti_gpio, 
                                    DAL_TLMM_GPIO_ENABLE));

   CATCH(nErr){}
   return nErr;
}




