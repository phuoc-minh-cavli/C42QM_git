/**
 * @file pcie_interrupt.c
 * @brief
 * Handles all pcie interrupts
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_interrupt.h"
#include "pcie_i.h"
#include "DDIInterruptController.h"
#include "pcie_tgt.h"

/** Interrupt Handle */
DalDeviceHandle *hpcieDALInterrupt = NULL;

#ifndef PCIE_BOOT
/* Registers an isr to service an IRQ id with the interrupt controller */
static pcie_return_status pcie_interrupti_isrinstall(pcie_interrupt_irq_type *irqhandle, uint32 irq, void *isr, void *data, uint32 int_type);

/* Deregisters the ISR from the interrupt controller */
static void pcie_interrupti_isruninstall(pcie_interrupt_irq_type *irqhandle);
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupti_isrinstall
** ============================================================================
*/
/**
 * Registers an isr to service an IRQ id with the interrupt controller
 *
 * @param[in,out]  irqhandle    Handle to be used with the interrupt controller
 * @param[in]      irq          IRQ ID to be registered for
 * @param[in,out]  isr          ISR to be invoked to the service the irq
 * @param[in,out]  data         data to be supplied to the isr upon invocation
 * @param[in]      int_type     Trigger for the interrupt
 *
 * @return         pcie_return_status
 *    Status of the operation.
 */
static pcie_return_status pcie_interrupti_isrinstall(pcie_interrupt_irq_type *irqhandle, uint32 irq, void *isr, void *data, uint32 int_type)
{
   if(DAL_SUCCESS != DalInterruptController_RegisterISR(hpcieDALInterrupt,
                                                        irq,
                                                        (DALISR)isr,
                                                        (DALISRCtx)data,
                                                        int_type))
   {
      return(PCIE_FAILED);
   }

   irqhandle->dwIrqId = irq;

   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupti_isruninstall
** ============================================================================
*/
/**
 * Deregisters the ISR from the interrupt controller
 *
 * @param[in,out]  irqhandle    Interrupt controller handle to be used
 *
 * @return         void
 */
void pcie_interrupti_isruninstall(pcie_interrupt_irq_type *irqhandle)
{
   DalInterruptController_Unregister(hpcieDALInterrupt,
                                     irqhandle->dwIrqId);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_config_isr_params
** ============================================================================
*/
/**
 * Configures the parameters for IRQ ISRs to update the IRQ structure.
 *
 * @param[in]      pcie_irq_id   IRQ id
 * @param[in,out]  isr           ISR handler
 *
 * @return         void
 */
void pcie_interrupt_config_isr_params(pcie_irq_id_type pcie_irq_id, void *isr)
{
   pcie_dev->pcie_irq[pcie_irq_id].isr = isr;
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_init
** ============================================================================
*/
/**
 * Initialize PCIe interrupts.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_interrupt_init(void)
{
   DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                    &hpcieDALInterrupt);
}
#endif /* PCIE_BOOT */
/* ============================================================================
**  Function : pcie_interrupt_install_isrs
** ============================================================================
*/
/**
 * Install PCIe interrupt ISR.
 *
 * @param[in]      void
 *
 * @return         void.
 */
void pcie_interrupt_install_isrs(void)
{
#ifndef PCIE_BOOT

   uint32 irq;

   pcie_osal_debug(MSG_HIGH,"Install ISRs begin");

   for(irq = 0;irq < PCIE_MAX_IRQ;irq++)
   {
      if(TRUE == pcie_dev->pcie_irq[irq].irq_enabled)
      {
         pcie_interrupti_isrinstall((pcie_interrupt_irq_type *)&pcie_dev->pcie_irq[irq].pcie_irq_handle, pcie_dev->pcie_irq[irq].pcie_irq_num, pcie_dev->pcie_irq[irq].isr, (void*)pcie_dev, pcie_dev->pcie_irq[irq].int_trig);
      }
   }

   pcie_osal_debug(MSG_HIGH,"Install ISRs end");
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_uninstall_isrs
** ============================================================================
*/
/**
 * Uninstall PCIe interrupt ISR.
 *
 * @param[in]      void
 *
 * @return         void.
 */
void pcie_interrupt_uninstall_isrs(void)
{
   uint32 irq;

   for(irq = 0;irq < PCIE_MAX_IRQ;irq++)
   {
      if(TRUE == pcie_dev->pcie_irq[irq].irq_enabled)
      {
         pcie_interrupti_isruninstall((pcie_interrupt_irq_type *)&pcie_dev->pcie_irq[irq].pcie_irq_handle);
      }
   }
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_ctrl
** ============================================================================
*/
/**
 * Controls the enabling/disabling of the interrupt
 *
 * @param[in]     irq          IRQ ID to be registered for
 * @param[in]     ctrl         Enable(True) or Disable(False) interrupt
 *
 * @return        void
 */
void pcie_interrupt_ctrl(uint32 irq, boolean ctrl)
{
   if(ctrl)
   {
      if(DAL_SUCCESS != DalInterruptController_InterruptEnable(hpcieDALInterrupt, irq))
      {
         pcie_osal_debug(MSG_FATAL, "Failed to enable interrupt for irq id %d", irq);
      }
   }
   else
   {
      if(DAL_SUCCESS != DalInterruptController_InterruptDisable(hpcieDALInterrupt, irq))
      {
         pcie_osal_debug(MSG_FATAL, "Failed to disable interrupt for irq id %d", irq);
      }
   }
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_israck
** ============================================================================
*/
/**
 * Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
 * to re-activate the IRQ.
 *
 * @param[in,out]  irqhandle    Interrupt handle
 *
 * @return         void
 */
void pcie_interrupt_israck(pcie_interrupt_irq_type *irqhandle)
{
   DalInterruptController_InterruptDone(hpcieDALInterrupt,
                                        irqhandle->dwIrqId);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_interrupt_is_irq_set
** ============================================================================
*/
/**
 * Check with the interrupt controller if the supplied IRQ is set
 *
 * @param[in]     irq          IRQ ID to be registered for
 *
 * @return     boolean
 *    TRUE - Interrupt is set
 *    FALSE - Interrpt is not set
 */
boolean pcie_interrupt_is_irq_set(uint32 irq)
{
   if (DAL_INTERRUPT_SET == DalInterruptController_InterruptStatus(hpcieDALInterrupt,irq))
      return TRUE;
   else
      return FALSE;
}
#endif /* PCIE_BOOT */

