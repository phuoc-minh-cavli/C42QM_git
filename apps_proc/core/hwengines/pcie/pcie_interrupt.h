/**
 * @file pcie_interrupt.h
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

#ifndef __PCIE_INTERRUPT_H__
#define __PCIE_INTERRUPT_H__

#include "com_dtypes.h"

/** IrqHandle Object */
typedef struct {
   uint32  dwIrqId;        /** < -- IRQ number to be registered to */
}pcie_interrupt_irq_type;


typedef enum _pcie_int_trig_type
{
   DEFAULT                = 0x00000001, /** Default interrupt trigger value */
   LEVEL_TRIGGERED_HIGH   = 0x00000002, /** Trigger interrupt only if the input signal at the source is high */
   LEVEL_TRIGGERED_LOW    = 0x00000003, /** Trigger interrupt only if the input signal at the source is low */
   RISING_EDGE_TRIGGERED  = 0x00000004, /** Trigger interrupt only if the input signal at the source transitions from low to high */
   FALLING_EDGE_TRIGGERED = 0x00000005, /** Trigger interrupt only if the input signal at the source transitions from high  to low */
}pcie_int_trig_type;

/** ALL PCIE IRQs IDs */
typedef enum
{
   PCIE_GLOBAL_INT              = 0x00000000,     /** < -- ID for PCIe Global Interrupt */
   PCIE_MSI_0_INT               = 0x00000001,     /** < -- ID for PCIe MSI 0 Interrupt */
   PCIE_MSI_1_INT               = 0x00000002,     /** < -- ID for PCIe MSI 1 Interrupt */
   PCIE_MSI_2_INT               = 0x00000003,     /** < -- ID for PCIe MSI 2 Interrupt */
   PCIE_MSI_3_INT               = 0x00000004,     /** < -- ID for PCIe MSI 3 Interrupt */
   PCIE_MSI_4_INT               = 0x00000005,     /** < -- ID for PCIe MSI 4 Interrupt */
   PCIE_MSI_5_INT               = 0x00000006,     /** < -- ID for PCIe MSI 5 Interrupt */
   PCIE_MSI_6_INT               = 0x00000007,     /** < -- ID for PCIe MSI 6 Interrupt */
   PCIE_MSI_7_INT               = 0x00000008,     /** < -- ID for PCIe MSI 7 Interrupt */
   PCIE_EDMA_INT                = 0x00000009,     /** < -- ID for PCIe EDMA Interrupt */
   PCIE_MAX_IRQ                 = 0x0000000A      /** < -- Reserved */
}pcie_irq_id_type;

/** PCIe IRQ handle */
typedef struct _pcie_irqs_types
{
   boolean irq_enabled;                /** < -- IRQ enabled/Disabled */
   uint32 pcie_irq_handle;             /** < -- Handle for the IRQ */
   uint32 pcie_irq_num;                /** < -- IRQ number */
   void *isr;                          /** < -- ISR */
   uint32 int_trig;                    /** < -- Specifies the trigger for the interrupt */
}pcie_irqs_type;

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
void pcie_interrupt_config_isr_params(pcie_irq_id_type pcie_irq_id, void *isr);

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
void pcie_interrupt_init(void);

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
void pcie_interrupt_install_isrs(void);

/* ============================================================================
**  Function : pcie_uninstall_isrs
** ============================================================================
*/
/**
 * Uninstall PCIe interrupt ISR.
 *
 * @param[in]      void
 *
 * @return         void.
 */
void pcie_interrupt_uninstall_isrs(void);

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
void pcie_interrupt_ctrl(uint32 irq, boolean ctrl);

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
void pcie_interrupt_israck(pcie_interrupt_irq_type *irqhandle);

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
boolean pcie_interrupt_is_irq_set(uint32 irq);

#endif /* __PCIE_INTERRUPT_H__ */
