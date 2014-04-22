
/*===========================================================================

              S M S M   I N T E R R U P T S :   D A L S Y S 

===========================================================================*/
/**
  @file smsm_intr_dal.c
*/
/* Abstracts the OS specific implementation of system function implemented by 
   DALSYS.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the SMEM_mainpage file. To change
         any of the the file/group text for the PDF, edit the SMEM_mainpage
         file, or contact Tech Pubs.

         The above description for this file is part of the "smem" group
         description in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_intr_dal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/13   bt      Initial revision to separate from crit sect and malloc OS
                   abstractions, which must be used by the QDI interface.
===========================================================================*/

#include "smsm.h"
#include "smsm_internal.h"
#ifdef SMSM_DSR_SUPPORTED
#include "dsr.h"
#endif

#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIIPCInt.h"

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
/** The handle for DALIPCINT: outgoing interrupts. */
static DalDeviceHandle *smsm_intr_out_handle;

#ifdef SMSM_DSR_SUPPORTED
/** The handle for DSR to handle remote updates. */
static dsr_obj_t *smsm_dsr_handles[SMSM_NUM_HOSTS];
#endif

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smsm_dsr
===========================================================================*/
/*!
@brief
  This function is called when an DSR is enqueued from interrupt context.
  This means certain host's state has changed. 

@param dsr_obj    DSR object
@param ctxt       Context associated with DSR
@param payload    payload context

@return None

@note   In interupt context
*/
/*=========================================================================*/
#ifdef SMSM_DSR_SUPPORTED
static void smsm_dsr(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  smsm_host_type remote = (smsm_host_type)(uint32)payload;
  
  if (remote >= SMSM_NUM_HOSTS)
  {
    ERR_FATAL("smsm_dsr: Invalid remote host %d!", remote, 0, 0);
  }
  smsm_process_host_update(remote);
}
#endif

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smsm_isr
===========================================================================*/
/*!
@brief
  This function is called when an interrupt is received. This means
  certain host's state has changed. 

@param  param - the interrupt source

@return None

@note   In interupt context
*/
/*=========================================================================*/
void smsm_isr(void *param)
{
  smsm_host_type remote = (smsm_host_type)(uint32)param;

  if (remote >= SMSM_NUM_HOSTS)
  {
    ERR_FATAL("smsm_isr: Invalid remote host %d!", remote, 0, 0);
  }
#ifdef SMSM_DSR_SUPPORTED
  
  if (smsm_dsr_handles[remote] == NULL)
  {
    ERR_FATAL("smsm_isr: smsm_dsr_handles[] Invalid remote %d!", remote, 0, 0);
  }
  dsr_enqueue(smsm_dsr_handles[remote], param);

#else
  smsm_process_host_update(remote);
#endif
}

/*===========================================================================
  FUNCTION  smsm_register_interrupt
===========================================================================*/
/**
  This function registers SMSM interrupts.

  @param[in] info   A pointer to the smsm data struct.

  @return
  None
*/
/*=========================================================================*/
void smsm_register_interrupt(smsm_info_type *info)
{
  smsm_intr_info_type *intr = NULL;
  uint32               i;
  /* DAL Incoming Interrupt Controller device handle. */
  DalDeviceHandle *smsm_intr_in_handle;
  
  /* Create Interrupt Handler */
  if( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_IPCINT, 
                                      &smsm_intr_out_handle ) )
  {
    ERR_FATAL( "smsm_register_interrupt: Interrupt Handler creation failed!", 
               0, 0, 0);
  }
  
  /* Create Incoming Interrupt Controller Handle */
  if( ( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, 
                                         &smsm_intr_in_handle ) )
          || (smsm_intr_in_handle == NULL) )
  {
    ERR_FATAL("smsm_register_interrupt: Intr Cntrler Handle creation failed!", 
              0, 0, 0);
  }

  for( i = 0; i < SMSM_NUM_HOSTS; i++ )
  {
    /* Register the SMSM-specific interrupt */
    intr = &(info->interrupt_tbl[i]);
    
    if( ( intr->from == info->this_host ) && 
        ( intr->irq_in != SMSM_NULL_IN_INTERRUPT ) )
    {
      #ifdef SMSM_DSR_SUPPORTED
        /* Create DSR object for processing the remote host update */
        if (dsr_create(&smsm_dsr_handles[i], 
                       smsm_dsr, 
                       NULL, 
                       DSR_PRI_HIGH, 
                       DSR_CONTEXT_ISR) == FALSE)
        {
          ERR_FATAL("smsm_register_interrupt: dsr_create failed i=%d!", i, 0, 0);
        }
      #endif
      if( DalInterruptController_RegisterISR( 
            smsm_intr_in_handle,
            (uint32)intr->irq_in,
            (DALIRQ)smsm_isr,
            (DALIRQCtx)intr->to,
            DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER ) != DAL_SUCCESS )
      {
        ERR_FATAL("smsm_register_interrupt: SMSM ISR for host %d failed to "
            "register.", i, 0, 0);
      }
    }
  }
}

/*===========================================================================
  FUNCTION  smsm_send_interrupt
===========================================================================*/
/**
  This function sends out the interprocessor interrupts.

  @param[in] intr   A pointer to the interrupt data struct.

  @return
  None
*/
/*=========================================================================*/
void smsm_send_interrupt(smsm_intr_info_type *intr)
{ 
  /* Check that IRQ is set */
  if( intr->irq_out != SMSM_NULL_OUT_INTERRUPT )
  {
    DalIPCInt_Trigger( smsm_intr_out_handle,
                     ( DalIPCIntProcessorType )intr->processor,
                     ( DalIPCIntInterruptType )intr->irq_out);
  }
}

/*===========================================================================
  FUNCTION  smsm_target_init_interrupt
===========================================================================*/
/**
  This function initialize the interprocessor interrupts for this target.

  @param[in] info   Pointer to the smsm_info structure
 
  @return
  None
*/
/*=========================================================================*/
void smsm_target_init_interrupt(smsm_info_type *info)
{
  uint32 i;
  byte *smsm_intr_enbl_arr;
  DALSYSPropertyVar prop;
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(smsm_prop_handle);
  
  /* Clear out the interrupt table */
  for( i = 0; i < SMSM_NUM_HOSTS; i++ )
  {
    info->interrupt_tbl[i].from = SMSM_INVALID_HOST;
    info->interrupt_tbl[i].to = SMSM_INVALID_HOST;
    info->interrupt_tbl[i].processor = DALIPCINT_PROC_NULL;
    info->interrupt_tbl[i].irq_out = SMSM_NULL_OUT_INTERRUPT;
    info->interrupt_tbl[i].irq_out_mask = 0;
    info->interrupt_tbl[i].irq_in = SMSM_NULL_IN_INTERRUPT;
  }
  
  if (DALSYS_GetDALPropertyHandleStr("/core/mproc/smsm", smsm_prop_handle) !=
        DAL_SUCCESS)
  {
    ERR_FATAL("smsm_target_init_interrupts: Cannot get DALProp handle.", 
              0, 0, 0);
  }
  else if (DALSYS_GetPropertyValue(smsm_prop_handle, "smsm_intr_enabled", 0, 
                                   &prop) != DAL_SUCCESS)
  {
    ERR_FATAL("smsm_target_init_interrupts: Cannot get smsm_intr_enabled DAL "
              "prop.", 0, 0, 0);
  }
  /* The SMSM intr enable array will be put in prop.Val.pbVal (array of bytes) 
   */
  smsm_intr_enbl_arr = prop.Val.pbVal;
  
  /* From/to APPS Processor */
  if (smsm_intr_enbl_arr[SMSM_APPS])
  {
    info->interrupt_tbl[SMSM_APPS].from          = info->this_host;
    info->interrupt_tbl[SMSM_APPS].to            = SMSM_APPS;
    info->interrupt_tbl[SMSM_APPS].processor     = SMSM_APPS_IPCPROC;
    info->interrupt_tbl[SMSM_APPS].irq_out       = SMSM_APPS_OUT;
    info->interrupt_tbl[SMSM_APPS].irq_out_mask  = 0; /* Not used. */
    info->interrupt_tbl[SMSM_APPS].irq_in        = SMSM_APPS_IN;
  }

  /* From/to Modem/GSS processor */
  if (smsm_intr_enbl_arr[SMSM_MODEM])
  {
    info->interrupt_tbl[SMSM_MODEM].from         = info->this_host;
    info->interrupt_tbl[SMSM_MODEM].to           = SMSM_MODEM;
    info->interrupt_tbl[SMSM_MODEM].processor    = SMSM_MPSS_IPCPROC;
    info->interrupt_tbl[SMSM_MODEM].irq_out      = SMSM_MPSS_OUT;
    info->interrupt_tbl[SMSM_MODEM].irq_out_mask = SMSM_MPSS_OUT_M;
    info->interrupt_tbl[SMSM_MODEM].irq_in       = SMSM_MPSS_IN;
  }

  /* From/to ADSP Q6 (LPASS) processor */
  if (smsm_intr_enbl_arr[SMSM_Q6])
  {
    info->interrupt_tbl[SMSM_Q6].from            = info->this_host;
    info->interrupt_tbl[SMSM_Q6].to              = SMSM_Q6;
    info->interrupt_tbl[SMSM_Q6].processor       = SMSM_ADSP_IPCPROC;
    info->interrupt_tbl[SMSM_Q6].irq_out         = SMSM_ADSP_OUT;
    info->interrupt_tbl[SMSM_Q6].irq_out_mask    = SMSM_ADSP_OUT_M;
    info->interrupt_tbl[SMSM_Q6].irq_in          = SMSM_ADSP_IN;
  }

  /* From/to RIVA processor */
  if (smsm_intr_enbl_arr[SMSM_WCNSS])
  {
    info->interrupt_tbl[SMSM_WCNSS].from         = info->this_host;
    info->interrupt_tbl[SMSM_WCNSS].to           = SMSM_WCNSS;
    info->interrupt_tbl[SMSM_WCNSS].processor    = SMSM_WCNSS_IPCPROC;
    info->interrupt_tbl[SMSM_WCNSS].irq_out      = SMSM_WCNSS_OUT;
    info->interrupt_tbl[SMSM_WCNSS].irq_out_mask = SMSM_WCNSS_OUT_M;
    info->interrupt_tbl[SMSM_WCNSS].irq_in       = SMSM_WCNSS_IN;
  }
}
