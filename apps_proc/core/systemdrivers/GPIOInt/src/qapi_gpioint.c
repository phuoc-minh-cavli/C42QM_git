/*======================================================================
                        qapi_gpioint.c 

GENERAL DESCRIPTION
  External interface for the GPIOInt module.

 EXTERNALIZED FUNCTIONS
  qapi_GPIOINT_Register_Interrupt
  qapi_GPIOINT_Deregister_Interrupt
  qapi_GPIOINT_Set_Trigger
  qapi_GPIOINT_Enable_Interrupt
  qapi_GPIOINT_Disable_Interrupt
  qapi_GPIOINT_Trigger_Interrupt
  qapi_GPIOINT_Is_Interrupt_Pending

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  Assume that __qapi_GPIOINT_Module_Init is called beforehand.

 Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 07/18/2019   shm      Removed QAPI handler registration for FR57064
 * 01/04/2017   spalepu  Initial Version
 ======================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "qapi_gpioint.h"
#include "qapi_txm_base.h"
#include "qurt_memory.h"
#include "GPIOInt.h"
#include "DalDevice.h"
#include "DDIGPIOInt.h"
#include "DALFramework.h"


/*==========================================================================

                  LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/
DalDeviceHandle   *gpioint_handle = NULL;
qbool_t gpioint_initialized = FALSE;


/*======================================================================
                          FUNCTIONS
=======================================================================*/

/*=============================================================================
  FUNCTION      qapi_gpioint_handler
=============================================================================*/
qapi_Status_t qapi_gpioint_handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
  qapi_Status_t retval = QAPI_ERROR;
  void *manager_cb;
  
  switch(id)
  {
    case TXM_QAPI_GPIOINT_REGISTER_INTERRUPT:
      if((FALSE == qurt_check_if_module_address((qurt_addr_t)c, sizeof(void*))) ||
         (FALSE == qurt_check_if_module_address((qurt_addr_t)h, sizeof(void*))))   
        break;

      if(!(qurt_qapi_callback_configure(TXM_QAPI_GPIOINT_REGISTER_INTERRUPT_CB, // type
                                     (void *)0xDEADCAFE, // dummy obj_ptr
                                     0,// id 
                                     (void *)c, // app_cb
                                     (void *)h, // app_cb_dispatcher
                                     &manager_cb,
                                     NULL)))
      {
        if(manager_cb == NULL)
        {
          return retval;
        }
      }
      retval = qapi_GPIOINT_Register_Interrupt((qapi_Instance_Handle_t*)a,(uint32_t)b,(qapi_GPIOINT_CB_t)manager_cb,(qapi_GPIOINT_Callback_Data_t)d,(qapi_GPIOINT_Trigger_e)e,(qapi_GPIOINT_Priority_e)f,(qbool_t)g);
      break;
    case TXM_QAPI_GPIOINT_DEREGISTER_INTERRUPT:
      retval = qapi_GPIOINT_Deregister_Interrupt((qapi_Instance_Handle_t*)a,(uint32_t)b);
      break;
    case TXM_QAPI_GPIOINT_SET_TRIGGER:
      retval = qapi_GPIOINT_Set_Trigger((qapi_Instance_Handle_t*)a,(uint32_t)b,(qapi_GPIOINT_Trigger_e)c);
      break;
    case TXM_QAPI_GPIOINT_ENABLE_INTERRUPT:
      retval = qapi_GPIOINT_Enable_Interrupt((qapi_Instance_Handle_t*)a,(uint32_t)b);
      break;
    case TXM_QAPI_GPIOINT_DISABLE_INTERRUPT:
      retval = qapi_GPIOINT_Disable_Interrupt((qapi_Instance_Handle_t*)a,(uint32_t)b);
      break;
    case TXM_QAPI_GPIOINT_TRIGGER_INTERRUPT:
      retval = qapi_GPIOINT_Trigger_Interrupt((qapi_Instance_Handle_t*)a,(uint32_t)b);
      break;
    case TXM_QAPI_GPIOINT_IS_INTERRUPT_PENDING:
      if(FALSE == qurt_check_if_module_address((qurt_addr_t)c, sizeof(qbool_t)))   
        break;
      retval = qapi_GPIOINT_Is_Interrupt_Pending((qapi_Instance_Handle_t*)a,(uint32_t)b,(qbool_t*)c);
      break;
    default:
      break;
  }
  return retval;
}

/*=============================================================================
  FUNCTION      __qapi_GPIOINT_Module_Init
=============================================================================*/
qbool_t __qapi_GPIOINT_Module_Init
(
  void
)
{
  if(DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_GPIOINT,&gpioint_handle))
  {
    gpioint_initialized = TRUE;
    return TRUE;
  }
 else
 {
   return FALSE;
 }
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Register_Interrupt
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Register_Interrupt
(
   qapi_Instance_Handle_t       *pH,
   uint32_t                      nGpio,
   qapi_GPIOINT_CB_t             pfnCallback,
   qapi_GPIOINT_Callback_Data_t  nData,
   qapi_GPIOINT_Trigger_e        eTrigger,
   qapi_GPIOINT_Priority_e       ePriority,
   qbool_t                       bNmi
)
{
  uint32_t nTrigger;
  int32_t  nErr;
  
  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }

  switch ( eTrigger )
  {
    case QAPI_GPIOINT_TRIGGER_LEVEL_HIGH_E:
      nTrigger = GPIOINT_TRIGGER_HIGH;
      break;
    case QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E:
      nTrigger = GPIOINT_TRIGGER_LOW;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_RISING_E:
      nTrigger = GPIOINT_TRIGGER_RISING;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_FALLING_E:
      nTrigger = GPIOINT_TRIGGER_FALLING;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_DUAL_E:
      nTrigger = GPIOINT_TRIGGER_DUAL_EDGE;
      break;
    default:
      return QAPI_ERR_INVALID_PARAM;
  }

  nErr = GPIOIntr_RegisterIsr((GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),
                               nGpio,
                                (GPIOIntTriggerType)nTrigger,(GPIOINTISR)pfnCallback,
                               (GPIOINTISRCtx)nData);
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Deregister_Interrupt
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Deregister_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
)
{
  int32_t nErr;

  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }
  
  nErr = GPIOIntr_Deregister( (GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt), 
                                  nGpio) ;
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Set_Trigger
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Set_Trigger
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio,
  qapi_GPIOINT_Trigger_e  eTrigger
)
{
  uint32_t nTrigger;
  int32_t  nErr;
  
  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }

  switch ( eTrigger )
  {
    case QAPI_GPIOINT_TRIGGER_LEVEL_HIGH_E:
      nTrigger = GPIOINT_TRIGGER_HIGH;
      break;
    case QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E:
      nTrigger = GPIOINT_TRIGGER_LOW;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_RISING_E:
      nTrigger = GPIOINT_TRIGGER_RISING;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_FALLING_E:
      nTrigger = GPIOINT_TRIGGER_FALLING;
      break;
    case QAPI_GPIOINT_TRIGGER_EDGE_DUAL_E:
      nTrigger = GPIOINT_TRIGGER_DUAL_EDGE;
      break;
    default:
      return QAPI_ERR_INVALID_PARAM;
  }

  nErr = GPIOIntr_SetTrigger((GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),nGpio,(GPIOIntTriggerType)nTrigger);
  
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Enable_Interrupt
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Enable_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
)
{
  int32_t nErr;
  
  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }
  
  nErr = GPIOIntr_EnableInterrupt((GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),nGpio);
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Disable_Interrupt
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Disable_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
)
{
  int32_t nErr;
  
  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }
  
  nErr = GPIOIntr_DisableInterrupt((GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),nGpio);
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Trigger_Interrupt
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Trigger_Interrupt
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio
)
{
  int32_t nErr;

  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }
  
  nErr = GPIOIntr_TriggerInterrupt((GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),nGpio);
  if ( nErr == DAL_SUCCESS )
  {
    return QAPI_OK;
  }
  return QAPI_ERROR;
}

/*=============================================================================
  FUNCTION      qapi_GPIOINT_Is_Interrupt_Pending
=============================================================================*/
qapi_Status_t qapi_GPIOINT_Is_Interrupt_Pending
(
  qapi_Instance_Handle_t *pH,
  uint32_t                nGpio,
  qbool_t                *pbIsPending
)
{
  int32_t  nErr;
  uint32_t nIsPending = 0;

  if(!gpioint_initialized)
  {
     return QAPI_ERROR;
  }
  
  if (pbIsPending == NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
  nErr = GPIOIntr_IsInterruptPending( (GPIOIntClientCtxt*)(gpioint_handle->pClientCtxt),nGpio, (uint32*)&nIsPending );
  if ( nErr == DAL_SUCCESS )
  {

    if ( nIsPending == 1 )
    {
      *pbIsPending = true;
    }
    else
    {
      *pbIsPending = false;
    }

    return QAPI_OK;
  }

  return QAPI_ERROR;
}
