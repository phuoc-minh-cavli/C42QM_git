/*===========================================================================

             M A I N   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2019 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/tlmm/src/qapi_tlmm.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/19   shm     Removed QAPI handler registration for FR57064
01/04/17   spalepu Initial version. 
===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "qapi_tlmm.h"
#include "qurt_memory.h"
#include "qapi_txm_base.h"
#include "DalDevice.h"
#include "DDITlmm.h"
#include "DALTLMM.h"
#include "DALFramework.h"


/*==========================================================================

                  LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/
DalDeviceHandle   *tlmm_handle = NULL;
qbool_t tlmm_initialized = FALSE;

/*======================================================================
                          FUNCTIONS
=======================================================================*/

/*=============================================================================
  FUNCTION      qapi_tlmm_handler
=============================================================================*/
qapi_Status_t qapi_tlmm_handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
  qapi_Status_t retval = QAPI_ERROR;
  qapi_TLMM_Config_t qapi_TLMM_Config;

  switch(id)
  {
    case TXM_QAPI_TLMM_GET_GPIO:
     if((FALSE == qurt_check_if_module_address((qurt_addr_t)b, sizeof(qapi_GPIO_ID_t))) ||
        (QURT_EOK != qurt_copy_from_user((void*)&qapi_TLMM_Config,sizeof(qapi_TLMM_Config_t),(void*)a,sizeof(qapi_TLMM_Config_t))))
            break;
      retval = qapi_TLMM_Get_Gpio_ID(&qapi_TLMM_Config,(qapi_GPIO_ID_t*)b);
      break;
    case TXM_QAPI_TLMM_RELEASE_GPIO:
     if(QURT_EOK != qurt_copy_from_user((void*)&qapi_TLMM_Config,sizeof(qapi_TLMM_Config_t),(void*)a,sizeof(qapi_TLMM_Config_t)))
            break;
      retval = qapi_TLMM_Release_Gpio_ID(&qapi_TLMM_Config,(qapi_GPIO_ID_t)b);
      break;
    case TXM_QAPI_TLMM_CONFIG_GPIO:
     if(QURT_EOK != qurt_copy_from_user((void*)&qapi_TLMM_Config,sizeof(qapi_TLMM_Config_t),(void*)b,sizeof(qapi_TLMM_Config_t)))
            break;
      retval = qapi_TLMM_Config_Gpio((qapi_GPIO_ID_t)a,&qapi_TLMM_Config);
      break;
    case TXM_QAPI_TLMM_DRIVE_GPIO:
      retval = qapi_TLMM_Drive_Gpio((qapi_GPIO_ID_t)a,(uint32_t)b,(qapi_GPIO_Value_t)c);
      break;
    case TXM_QAPI_TLMM_READ_GPIO:
     if(FALSE == qurt_check_if_module_address((qurt_addr_t)c, sizeof(qapi_GPIO_Value_t)))   
         break;
      retval = qapi_TLMM_Read_Gpio((qapi_GPIO_ID_t)a,(uint32_t)b,(qapi_GPIO_Value_t*)c);
      break;
    default:
      break;
  }
  return retval;
}

/*=============================================================================
  FUNCTION      __qapi_TLMM_Module_Init
=============================================================================*/
qbool_t __qapi_TLMM_Module_Init
(
void
)
{
  if(DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_TLMM,&tlmm_handle))
  {
    tlmm_initialized = TRUE;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*=============================================================================
  FUNCTION      qapi_TLMM_Get_Gpio_ID
=============================================================================*/
qapi_Status_t qapi_TLMM_Get_Gpio_ID
(
  qapi_TLMM_Config_t* qapi_TLMM_Config,
  qapi_GPIO_ID_t*     qapi_GPIO_ID
)
{
  if(!tlmm_initialized)
  {
     return QAPI_ERROR;
  }
  
  if ((qapi_TLMM_Config != NULL) && (qapi_GPIO_ID != NULL) && tlmm_handle)
  {
    if (DAL_SUCCESS == TLMM_GetGpioPinId((TLMMClientCtxt *)(tlmm_handle->pClientCtxt),qapi_TLMM_Config->pin, (DALGpioIdType*)qapi_GPIO_ID))
    {
      return QAPI_OK;
    }
  }
  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_TLMM_Release_Gpio_ID
=============================================================================*/
qapi_Status_t qapi_TLMM_Release_Gpio_ID
(
  qapi_TLMM_Config_t* qapi_TLMM_Config,
  qapi_GPIO_ID_t      qapi_GPIO_ID
)
{
  if(!tlmm_initialized)
  return QAPI_ERROR;
  
  if ((qapi_TLMM_Config != NULL) && (qapi_GPIO_ID != NULL) && tlmm_handle)
  {
    if (DAL_SUCCESS == TLMM_ReleaseGpioPinId((TLMMClientCtxt *)(tlmm_handle->pClientCtxt),qapi_TLMM_Config->pin, (DALGpioIdType)qapi_GPIO_ID))
    {
      return QAPI_OK;
    }
  }
  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_TLMM_Config_Gpio
=============================================================================*/
qapi_Status_t qapi_TLMM_Config_Gpio
(
  qapi_GPIO_ID_t      qapi_GPIO_ID,
  qapi_TLMM_Config_t* qapi_TLMM_Config
)
{
  DalTlmm_GpioConfigIdType pincfg;
  DALGpioSignalType configuration;
  
  if(!tlmm_initialized)
  {
    return QAPI_ERROR;
  }
  if ((qapi_TLMM_Config != NULL) && tlmm_handle)
  { 
    configuration = (DALGpioSignalType)DAL_GPIO_CFG(qapi_TLMM_Config->pin,
                                          qapi_TLMM_Config->func,
                                          (DALGpioDirectionType)qapi_TLMM_Config->dir,
                                          (DALGpioPullType)qapi_TLMM_Config->pull,
                                          (DALGpioDriveType)qapi_TLMM_Config->drive);
    if(DAL_SUCCESS == TLMM_ConfigGpioInternal((TLMMClientCtxt *)(tlmm_handle->pClientCtxt),
                                        configuration,DAL_TLMM_GPIO_ENABLE,
                                       (DALGpioIdType)qapi_GPIO_ID))
    {
      return(QAPI_OK);
    }
  }
  return(QAPI_ERR_INVALID_PARAM);

}

/*=============================================================================
  FUNCTION      qapi_TLMM_Drive_Gpio
=============================================================================*/
qapi_Status_t qapi_TLMM_Drive_Gpio
(
  qapi_GPIO_ID_t    qapi_GPIO_ID,
  uint32_t          pin,
  qapi_GPIO_Value_t Value
)
{
  DALGpioValueType output_val = DAL_GPIO_LOW_VALUE;
  DALGpioSignalType configuration;

  if(!tlmm_initialized)
  {
    return QAPI_ERROR;
  }
  if (Value == QAPI_GPIO_HIGH_VALUE_E)
  {
    output_val = DAL_GPIO_HIGH_VALUE;
  }
  
  configuration = (DALGpioSignalType)DAL_GPIO_CFG(pin,0,0,0,0);

  if(tlmm_handle)
  {
    if (DAL_SUCCESS == TLMM_GpioOutInternal
   (
    (TLMMClientCtxt *)(tlmm_handle->pClientCtxt),
    configuration,
    output_val,
   (DALGpioIdType)qapi_GPIO_ID
   ))
   {
     return (QAPI_OK);
   }
   
    else
    return(QAPI_ERROR);
  }
  
  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_TLMM_Read_Gpio
=============================================================================*/
qapi_Status_t qapi_TLMM_Read_Gpio
(
  qapi_GPIO_ID_t    qapi_GPIO_ID,
  uint32_t          pin,
  qapi_GPIO_Value_t* qapi_GPIO_Value
)
{
  DALGpioSignalType configuration;
  configuration = (DALGpioSignalType)DAL_GPIO_CFG(pin,0,0,0,0);
  
   if(!tlmm_initialized)
  {
    return QAPI_ERROR;
  }
  
  if(qapi_GPIO_Value==NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
  if (DAL_SUCCESS == TLMM_GpioIn((TLMMClientCtxt *)(tlmm_handle->pClientCtxt),
  configuration,
  (DALGpioValueType*)(qapi_GPIO_Value)))
  {
    return QAPI_OK;
  }
  else
  {
    return(QAPI_ERR_INVALID_PARAM);
  }
}

