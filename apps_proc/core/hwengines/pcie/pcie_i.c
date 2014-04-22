/**
 * @file pcie_i.c
 * @brief
 * Provides defines and types that are internal to pcie and shared between platform and core.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_i.h"
#include "pcie_tgt.h"
#include "pcie_res.h"
#include "pcie.h"
#include "pcie_gpio.h"
#include "pcie_interrupt.h"
#include "pcie_core.h"
#include "pcie_phy.h"
#include "pcie_dma.h"
#ifndef  PCIE_BOOT
/* For PERST reset disable*/
#include "pcie_hwio.h"
#include "err.h"
#endif /* PCIE_BOOT */

#define PCIE_PERST_TIMEOUT_MS               5000            /** Timeout for PERST */

/** Link notification callback function and parameter */
typedef struct _pcie_i_link_notification_cb_type
{
   pcie_i_link_notification_cb_fn_type  pcie_registered_cb;        /** < -- Link notification callback function */
   void *pcie_cb_passed_param;                                     /** < -- Link notification callback function parameter */
}pcie_i_link_notification_cb_type;

/** PCIe IRQ function structure */
typedef struct _pcie_i_irq_function
{
   pcie_irq_id_type irq_num;         /** < -- IRQ ID */
   void *isr;                        /** < -- IRQ function */
}pcie_i_irq_function_type;

/** SW Interrupt notification callback function type */
typedef void (*pcie_i_sw_int_notification_cb_fn_type)(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

/** PCIe SW Interrupt function structure */
typedef struct _pcie_i_sw_int_function_type
{
   pcie_core_sw_int_id_type sw_int_id;                        /** < -- SW Interrupt Id */
   boolean enabled;                                           /** < -- Enabled status */
   pcie_i_sw_int_notification_cb_fn_type sw_int_handler;      /** < -- SW Interrupt handler function */
}pcie_i_sw_int_function_type;

#ifndef PCIE_BOOT
/** PCIe IRQ functions we support in the file */
static pcie_i_irq_function_type pcie_i_irq_function[] =
{
   /* IRQ ID */                 /* IRQ function*/
   { PCIE_GLOBAL_INT,             pcie_i_global_int_handler},
};

/** PCIe SW Interrupt mask and handler */
static pcie_i_sw_int_function_type pcie_i_sw_int_function[]=
{
   { PCIE_INT_UNUSED_0,             FALSE,  NULL},
   { PCIE_SMLH_REQ_RST_LINK_DOWN,   TRUE,   pcie_i_link_down_handler},
   { PCIE_CFG_BUS_MASTER_EN,        FALSE,  NULL},
   { PCIE_RADM_TURNOFF,             FALSE,  NULL},
   { PCIE_INT_DEBUG,                FALSE,  NULL},
   { PCIE_INT_LTR,                  FALSE,  NULL},
   { PCIE_INT_MHI_Q6,               FALSE,  NULL},
   { PCIE_INT_MHI_A7,               FALSE,  pcie_i_mhi_int_handler},
   { PCIE_INT_DSTATE_CHANGE,        TRUE,   pcie_i_device_state_change_handler},
   { PCIE_INT_L1SS_IDLE_TO,         TRUE,   pcie_i_l1_inact_timeout_handler},
   { PCIE_INT_MMIO_WRITE,           FALSE,  pcie_i_mhi_int_handler},
   { PCIE_INT_CFG_WRITE,            FALSE,  NULL},
   { PCIE_INT_LINK_REQ_RST_FLUSH,   FALSE,  NULL},
   { PCIE_INT_PLS_LINK_UP,          TRUE,   pcie_i_link_up_handler},
   { PCIE_INT_AER_LEGACY,           FALSE,  NULL},
   { PCIE_INT_PLS_ERR,              FALSE,  NULL},
   { PCIE_INT_PME_LEGACY,           FALSE,  NULL},
   { PCIE_INT_PLS_PME,              FALSE,  NULL},
   { PCIE_INTD,                     FALSE,  NULL},
   { PCIE_INTC,                     FALSE,  NULL},
   { PCIE_INTB,                     FALSE,  NULL},
   { PCIE_INTA,                     FALSE,  NULL},
   { PCIE_INT_EDMA_INT,             FALSE,  NULL},
   { PCIE_INT_MSI_DEV_7,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_6,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_5,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_4,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_3,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_2,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_1,            FALSE,  NULL},
   { PCIE_INT_MSI_DEV_0,            FALSE,  NULL},
   { PCIE_INT_UNUSED_31,            FALSE,  NULL},
};

#endif /* PCIE_BOOT */

/** Handle to the PCIe device from plat */
pcie_devinfo_type *pcie_dev     = NULL;

#ifndef PCIE_BOOT
/** Link notification callback function and parameter definition */
static pcie_i_link_notification_cb_type pcie_i_link_notification_cb;
/** Flag indicating whether the PCIe error-fatal callback has been registered. */
static boolean pcie_err_fatal_callback_registered = FALSE;
#endif /* PCIE_BOOT */

/* Enables additional settings to improve L1ss entry/exit latency */
static void pcie_i_enable_additional_l1ss_settings(void);

/* Register the IRQ ISRs to context structure */
static void pcie_i_irq_register(void);

/** Prototypes*/
#ifndef PCIE_BOOT
/* Error fatal callback function */
static void pcie_err_fatal(void);

/* Early error fatal callback function */
static void pcie_errfatal_early(void);

/* PERST ISR */
static void* pcie_i_perst_isr(uint32 param);

#ifdef PCIE_CLKREQ_INTR_SUPPORTED
/* CLKREQ ISR */
static void* pcie_i_clkreq_isr(uint32 param);
#endif /* PCIE_CLKREQ_INTR_SUPPORTED */
#endif /* PCIE_BOOT */

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_err_fatal
** ============================================================================
*/
/**
 * Calls the core dump in case of error fatal.
 *
 * @param[in]         void
 *
 * @return            void
 */
static void pcie_err_fatal(void)
{
   pcie_coredump_type *pcie_coredump_ptr = &pcie_dev->pcie_coredump;
   /* Early error fatal will turn on the boot clock */
   pcie_osal_debug(MSG_HIGH, "Trying to collect PCIe core dump");
   pcie_core_dump(pcie_coredump_ptr, PCIE_ERROR_FATAL_DUMP_TYPE);
}

/* ============================================================================
**  Function : pcie_errfatal_early
** ============================================================================
*/
/**
 * Err fatal hook for PCIE driver called before any err fatal processing
 *
 * @param[in]   void
 *
 * @return      void
 */
static void pcie_errfatal_early(void)
{
   pcie_res_boot_clk_enable();
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_i_enable_additional_l1ss_settings
** ============================================================================
*/
/**
 * Enables additional settings to improve L1ss entry/exit latency
 *
 * @param[in]         void
 *
 * @return            void
 */
static void pcie_i_enable_additional_l1ss_settings(void)
{
#ifndef  PCIE_BOOT
   /* Update the PHY settings once we enter AMSS */
   pcie_phy_config_update();
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_write_reg
** ============================================================================
*/
/**
 * Writes <c>val</c> to the register at <c>base+offset</c>.
 *
 * @param[in]  base      Base address of the module
 * @param[in]  offset    Offset of the register within the module
 * @param[in]  val       Value to be written
 *
 * @return     void
 */
void pcie_i_write_reg(uint32 base, uint32 offset, uint32 val)
{
   uint32 addr = base + offset;
   uint32 read_val;
#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "Write fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", addr);
      return;
   }
#endif

   (*((volatile uint32*)(addr))) = val;
   if((base == pcie_dev->dbi_base) || (base == pcie_dev->elbi_base))
   {
      read_val = (*((volatile uint32*)(addr)));
      read_val = read_val;
   }
}

/* ============================================================================
**  Function : pcie_i_write_reg_field
** ============================================================================
*/
/**
 * Writes <c>field_val</c> to the field defined by <c>mask</c> and <c>shift</c>
 * in the register at <c>base+offset</c>.
 *
 * @param[in]  base         Base address of the module
 * @param[in]  offset       Offset of the register within the module
 * @param[in]  mask         Mask for the field to be programmed
 * @param[in]  shift        Shift required to program the field
 * @param[in]  field_val    Value to be programmed in the field
 *
 * @return     void
 */
void pcie_i_write_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift, uint32 field_val)
{
   uint32 regval;
#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "Write fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", (uint32)(base + offset));
      return;
   }
#endif
   regval = pcie_i_read_reg(base, offset);
   regval &= ~mask;
   regval = regval | (field_val << shift);
   pcie_i_write_reg(base, offset, regval);
}

/* ============================================================================
**  Function : pcie_i_read_reg
** ============================================================================
*/
/**
 * Returns the value in the register at <c>base+offset</c>.
 *
 * @param[in]  base      Base address of the module
 * @param[in]  offset    Offset of the register within the module
 *
 * @return     uint32
 *    Value of the register at <c>base+offset</c>.
 */
uint32 pcie_i_read_reg(uint32 base, uint32 offset)
{
   uint32 addr = base + offset;
   uint32 value;

#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "Read fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", addr);
      return(0);
   }
#endif
   value = (*((volatile uint32*)(addr)));

   return(value);
}

/* ============================================================================
**  Function : pcie_i_read_reg_field
** ============================================================================
*/
/**
 * Returns the <c>field_val</c> from the field defined by <c>mask</c> and
 * <c>shift</c> in the register at <c>base+offset</c>.
 *
 * @param[in]  base      Base address of the module
 * @param[in]  offset    Offset of the register within the module
 * @param[in]  mask      Mask for the field to be read
 * @param[in]  shift     Shift required to read the field
 *
 * @return     uint32
 *    Value of the field
 */
uint32 pcie_i_read_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift)
{
   uint32 regval;
#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "Read fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", (uint32)(base + offset));
      return(0);
   }
#endif
   regval = pcie_i_read_reg(base, offset);
   regval &= mask;
   regval >>= shift;
   return(regval);
}

/* ============================================================================
**  Function : pcie_i_init
** ============================================================================
*/
/**
 * Initialize PCIe Internals.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_i_init(void)
{
#ifndef PCIE_BOOT
   pcie_osal_syncinit(&pcie_dev->pcie_sync);
#endif /* PCIE_BOOT */

   /* Initalize PCIe Resources */
   pcie_res_init();

   /* Enable PCIe Resources */
   pcie_res_enable();

   /* Enable only relevant SW interrupt */
   pcie_i_enable_sw_int();

   /* Register for IRQs that we use */
   pcie_i_irq_register();

   /* Register error fatal callbacks */
   pcie_i_register_system_notifications();

   /* Install PCIe IRQs */
   pcie_interrupt_install_isrs();
   pcie_core_enable_interrupt();
}

/* ============================================================================
**  Function : pcie_i_pm_sync_deinit
** ============================================================================
*/
/**
 * De-initialize the sync for pm callbacks.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_i_pm_sync_deinit(void)
{
#ifndef PCIE_BOOT
   pcie_osal_syncdestroy(&pcie_dev->pcie_sync);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_assert_wake
** ============================================================================
*/
/**
 * Assert PCIe GPIO WAKE# to wake up Host.
 *
 * @param[in]      void
 *
 * @return         void.
 */
void pcie_i_assert_wake(void)
{
#ifndef  PCIE_BOOT
   pcie_gpio_set_state(PCIE_GPIO_WAKE, PCIE_GPIO_LOW);
   pcie_osal_debug(MSG_HIGH, "Endpoint assert WAKE#");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_deassert_wake
** ============================================================================
*/
/**
 * De-assert PCIe GPIO WAKE#.
 *
 * @param[in]      void
 *
 * @return     void
 */
void pcie_i_deassert_wake(void)
{
#ifndef  PCIE_BOOT
   pcie_gpio_set_state(PCIE_GPIO_WAKE, PCIE_GPIO_HIGH);
   pcie_osal_debug(MSG_HIGH, "Endpoint deassert WAKE");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_perst_isr
** ============================================================================
*/
/**
 * PCIe GPIO PERST# interrupt handler.
 *
 * @param[in]      param    expected PERST value
 *
 * @return         void*
 */
static void* pcie_i_perst_isr(uint32 param)
{
#ifndef PCIE_BOOT
   pcie_perst_cb_param_type *perst_exp = (pcie_perst_cb_param_type * )param;
   pcie_gpio_pin_value_type perst_pin;
   pcie_gpio_int_trigger_type trigger;

   perst_pin = pcie_gpio_get_state(PCIE_GPIO_PERST);
   /* Determine next trigger based on gpio value */
   if(PCIE_GPIO_HIGH == perst_pin)
   {
      pcie_osal_debug(MSG_HIGH, "GPIO line is HIGH");
      trigger = LEVEL_LOW;
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "GPIO line is LOW");
      trigger = LEVEL_HIGH;
   }

   /* Set trigger based on gpio value */
   pcie_gpio_interrupt_set_trigger(PCIE_GPIO_PERST, trigger);

   /* If perst pin is not whats expected, nothing to do */
   if(perst_pin != perst_exp->expected_perst)
   {
      pcie_osal_debug(MSG_ERR, "Perst doesnt match expected nothing to do, possible"
                      "spurious interrupt or line toggled too fast");
      return(0);
   }

   if(PCIE_GPIO_LOW == perst_pin)
   {
      pcie_osal_debug(MSG_HIGH, "GPIO PERST# is asserted");

      /* Call MHI registered Callback function */
      pcie_osal_syncenter(&pcie_dev->pcie_sync);
      if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
      {
         pcie_i_link_notification_cb.pcie_registered_cb(PCIE_PM_DEVICE_STATE_IN_D3_COLD, pcie_i_link_notification_cb.pcie_cb_passed_param);
      }

      pcie_osal_syncleave(&pcie_dev->pcie_sync);

      /* Set PERST GPIO trigger as level HIGH: PERST# is asserted */
      pcie_dev->perst_in_expect.expected_perst = PCIE_GPIO_HIGH;
   }
   else if(PCIE_GPIO_HIGH == perst_pin)
   {
      pcie_osal_debug(MSG_HIGH, "GPIO PERST# is deasserted");

      /* Call MHI registered Callback function */
      pcie_osal_syncenter(&pcie_dev->pcie_sync);
      if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
      {
         pcie_i_link_notification_cb.pcie_registered_cb(PCIE_PM_PERST_DEASSERTED, pcie_i_link_notification_cb.pcie_cb_passed_param);
      }

      pcie_osal_syncleave(&pcie_dev->pcie_sync);

      /* Set PERST GPIO trigger as level LOW: PERST# is deasserted */
      pcie_dev->perst_in_expect.expected_perst = PCIE_GPIO_LOW;
   }
#endif /* PCIE_BOOT */
   return(0);
}

/* ============================================================================
**  Function : pcie_i_irq_register
** ============================================================================
*/
/**
 * Register the IRQ ISRs to context structure.
 *
 * @param[in]      void
 *
 * @return         void
 */
static void pcie_i_irq_register(void)
{
#ifndef  PCIE_BOOT
   uint32 i;

   for(i = 0;i < sizeof(pcie_i_irq_function)/sizeof(pcie_i_irq_function[0]);i++)
   {
      pcie_interrupt_config_isr_params(pcie_i_irq_function[i].irq_num,
                                       pcie_i_irq_function[i].isr);
   }
#endif /* PCIE_BOOT */
}

#ifdef PCIE_CLKREQ_INTR_SUPPORTED
/* ============================================================================
**  Function : pcie_i_clkreq_isr
** ============================================================================
*/
/**
 * PCIe GPIO CLKREQ interrupt handler.
 *
 * @param[in]      param    parameter for clkreq ISR
 *
 * @return         void*
 */
static void* pcie_i_clkreq_isr(uint32 param)
{
   pcie_osal_debug(MSG_HIGH, "Enter");

   /* De-register the CLKREQ ISR */
   pcie_gpio_deregister_isr(PCIE_GPIO_CLKREQ);

   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
   {
      pcie_i_link_notification_cb.pcie_registered_cb(PCIE_CLKREQ_ASSERTED, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }
   pcie_osal_syncleave(&pcie_dev->pcie_sync);

   pcie_osal_debug(MSG_HIGH, "Exit");

   return(0);
}
#endif /* #ifdef PCIE_CLKREQ_INTR_SUPPORTED */

/* ============================================================================
**  Function : pcie_i_register_link_notification_cb
** ============================================================================
*/
/**
 * Registers link notification call back from upper layers.
 *
 * @param[in,out]      link_notification_cb    Callback function
 * @param[in,out]      param                   Pointer to parameters provided by client
 *
 * @return             void
 */
void pcie_i_register_link_notification_cb(pcie_i_link_notification_cb_fn_type link_notification_cb, void *param)
{
#ifndef PCIE_BOOT
   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   pcie_i_link_notification_cb.pcie_registered_cb = link_notification_cb;
   pcie_i_link_notification_cb.pcie_cb_passed_param = param;

#ifndef PCIE_BOOT
   /* Disable PERST chip Reset after the link notification call back is registered */
   HWIO_TCSR_PCIE_PERST_EN_OUTM(HWIO_TCSR_PCIE_PERST_EN_PCIE_PERST_EN_BMSK,0);
#endif /* PCIE_BOOT */

   pcie_osal_syncleave(&pcie_dev->pcie_sync);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_enable_sw_int
** ============================================================================
*/
/**
 * Enables only those SW interrupts that we support.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_i_enable_sw_int(void)
{
#ifndef PCIE_BOOT
   uint32 mask = 0;
   uint32 i;
   for(i=0;i<sizeof(pcie_i_sw_int_function)/sizeof(pcie_i_sw_int_function[0]);i++)
   {
      /* All interrupts are enabled by default. Disable those interrupts that we don't support */
      if(FALSE == pcie_i_sw_int_function[i].enabled)
      {
         /* Mask value has the interrupts that should be disabled */
         mask |= (1<<pcie_i_sw_int_function[i].sw_int_id);
      }
   }
   /* Disable the interrupts in the mask */
   pcie_core_sw_int_ctrl(mask,FALSE);
#endif /* PCIE_BOOT */
}

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_i_sw_int_ctrl
** ============================================================================
*/
/**
 * Enable/Disable a particular SW interrupt.
 *
 * @param[in]      sw_id   ID for the SW interrupt
 * @param[in]      ctrl    Control value(Enable/disable)
 *
 * @return             void
 */
void pcie_i_sw_int_ctrl(pcie_core_sw_int_id_type sw_id,boolean ctrl)
{
   uint32 mask;
   mask = 1 << sw_id;
   /* Update the table for enabled or disabled */
   pcie_i_sw_int_function[sw_id].enabled = ctrl;
   pcie_core_sw_int_ctrl(mask,ctrl);
}

/* ============================================================================
**  Function : pcie_i_global_int_handler
** ============================================================================
*/
/**
 * Handle PCIe Global Interrupt.
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 *
 * @return           void
 */
void pcie_i_global_int_handler(void *pcie_dev_r)
{
   uint32 int_status_reg;
   uint32 i,regval;

   /* Read the status of the SW Interrupts*/
   int_status_reg = pcie_core_get_sw_int_status() & pcie_dev->pcie_sw_int_mask;

   while(int_status_reg)
   {
      i=0;
      do
      {
         if(int_status_reg & 0x1)
         {
            regval = (1<<pcie_i_sw_int_function[i].sw_int_id);
            /* Clear the SW interrupt */
            pcie_core_set_sw_int_clear(regval);

            /* Call individual SW interrupt handlers from the table depending on the masks */
            if(NULL != pcie_i_sw_int_function[i].sw_int_handler)
            {
               pcie_i_sw_int_function[i].sw_int_handler(pcie_dev,(pcie_core_sw_int_id_type)i);
               pcie_osal_debug(MSG_LOW,"Handled SW interrupt with id %d",pcie_i_sw_int_function[i].sw_int_id);
            }
            else
            {
               pcie_osal_debug(MSG_FATAL,"Unhandled SW interrupt with id %d",pcie_i_sw_int_function[i].sw_int_id);
            }
         }
         i++;
      }while(int_status_reg>>=1);

      /* Check the status bits again until the interrupt got cleared.
         Read the status of the SW Interrupts and mask with the interrupts enabled to process only enabled interrupts */
      int_status_reg = pcie_core_get_sw_int_status() & pcie_dev->pcie_sw_int_mask;
   }
}

/* ============================================================================
**  Function : pcie_i_mhi_int_handler
** ============================================================================
*/
/**
 * Handles PCIe MHI SW interrupts.
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_mhi_int_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id)
{
   pcie_event_notification_type evnt;

   if(PCIE_INT_MHI_A7 == sw_int_id)
   {
      evnt = PCIE_MHI_INT;
   }
   else if(PCIE_INT_MMIO_WRITE == sw_int_id)
   {
      evnt = PCIE_MMIO_WRITE;
   }
   else
   {
      evnt = PCIE_TO_MHI_PM_MAX;
      pcie_osal_debug(MSG_FATAL,"Unsupported SW Interrupt id %d",sw_int_id);
   }

   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
   {
      pcie_i_link_notification_cb.pcie_registered_cb(evnt, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }
   else
   {
      pcie_osal_debug(MSG_HIGH,"Client callback not registered for SW Interrupt id %d",sw_int_id);
   }
   pcie_osal_syncleave(&pcie_dev->pcie_sync);
}

/* ============================================================================
**  Function : pcie_i_link_down_handler
** ============================================================================
*/
/**
 * Handle PCIe link down.reset the PCIe PHY and Core and try to bring PCIe link back to L0 state.
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_link_down_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id)
{
   pcie_core_linkstate_type link_state;
   pcie_dev->pcie_link_down_counter++;

   pcie_core_get_link_status(&link_state);

   pcie_osal_debug(MSG_HIGH, "PCIE link down interrupt is firing with PCIE LTSSM state = %d",link_state);

   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
   {
      pcie_i_link_notification_cb.pcie_registered_cb(PCIE_LINK_DOWN, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }
   pcie_osal_syncleave(&pcie_dev->pcie_sync);
}

/* ============================================================================
**  Function : pcie_i_link_up_handler
** ============================================================================
*/
/**
 * Handle PCIe link up
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return         void
 */
void pcie_i_link_up_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id)
{
   pcie_core_linkstate_type link_state;
   pcie_dev->pcie_link_up_counter++;

   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
   {
      pcie_i_link_notification_cb.pcie_registered_cb(PCIE_LINK_UP, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }
   pcie_osal_syncleave(&pcie_dev->pcie_sync);

   pcie_core_get_link_status(&link_state);

   pcie_osal_debug(MSG_HIGH, "PCIE link up interrupt is firing with PCIE LTSSM state = %d",link_state);
}

/* ============================================================================
**  Function : pcie_i_device_state_change_handler
** ============================================================================
*/
/**
 * PCIe device state change interrupt handler.
 *
 * @param[in,out]     pcie_dev_r    Pointer to pcie_dev info.
 * @param[in]         sw_int_id     SW Interrupt Id
 *
 * @return            void
 */
void pcie_i_device_state_change_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id)
{
   uint8 cur_d_state;
   pcie_devinfo_type *pcie_dev_info = (pcie_devinfo_type * )pcie_dev_r;

   cur_d_state = pcie_i_read_reg_field(pcie_dev_info->dbi_base, PCIE_FLD(TYPE0_CON_STATUS_REG_1, POWER_STATE));
   pcie_osal_debug(MSG_HIGH, "Device State change interrupt is firing, Device state=%d", cur_d_state, 0, 0);
   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   if(POWER_D0 == cur_d_state)
   {
      /* Indicate to core that link isn't ready to enter L23 */
      pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_CTRL, READY_ENTR_L23), 0);
      if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
         pcie_i_link_notification_cb.pcie_registered_cb(PCIE_PM_DEVICE_STATE_IN_D0, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }
   else if(POWER_D3 == cur_d_state)
   {
      /* By Spec when we enter a non-D0 state, we must clear the requirements for both the LTRs */
      pcie_send_ltr(0);

      /* Send enterL23 immediately if no callback is registered */
      if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
         pcie_i_link_notification_cb.pcie_registered_cb(PCIE_PM_DEVICE_STATE_IN_D3_HOT, pcie_i_link_notification_cb.pcie_cb_passed_param);
      else
         pcie_device_enable_EnterL23();
   }

   pcie_osal_syncleave(&pcie_dev->pcie_sync);
}

/* ============================================================================
**  Function : pcie_i_l1_inact_timeout_handler
** ============================================================================
*/
/**
 * PCIe L1SS inactivity timer interrupt handler.
 *
 * @param[in,out]    pcie_dev_r    Pointer to pcie_dev info.
 * @param[in]        sw_int_id     SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_l1_inact_timeout_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id)
{
   uint32 l1sub_cnt_val;
   l1sub_cnt_val = pcie_i_read_reg(pcie_dev->mhi_base, PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB);
   pcie_osal_debug(MSG_HIGH, "L1 inactive time out interrupt is firing with l1subcnt=0x%x", l1sub_cnt_val);

   pcie_osal_syncenter(&pcie_dev->pcie_sync);

   /* Disable L1ss inactivity interrupt */
   pcie_control_l1ss_inact_interrupt(FALSE, PCIE_L1SUB_TIMER_ZERO_VAL);

   /* Call MHI registered Callback function */
   if(NULL != pcie_i_link_notification_cb.pcie_registered_cb)
   {
      pcie_i_link_notification_cb.pcie_registered_cb(PCIE_PM_L1_INACT_TIMEOUT, pcie_i_link_notification_cb.pcie_cb_passed_param);
   }

   pcie_osal_syncleave(&pcie_dev->pcie_sync);
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_i_is_pcie_fuse_disabled
** ============================================================================
*/
/**
 * Return True if PCIe fuse is disabled.
 *
 * @param[in]      void
 *
 * @return         boolean
 *    True if fuse is disabled else returns false
 */
boolean  pcie_i_is_pcie_fuse_disabled(void)
{
#ifndef PCIE_BOOT
   uint32 read_fuse;

   read_fuse = HWIO_FEATURE_CONFIG2_INM(SECURITY_CONTROL_CORE_REG_BASE, HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_BMSK);

   if(read_fuse)
   {
      return(TRUE);
   }
   return(FALSE);
#else
   return(FALSE);
#endif
}

/* ============================================================================
**  Function : pcie_i_register_system_notifications
** ============================================================================
*/
/**
 * Registers for system notifications
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_i_register_system_notifications(void)
{
#ifndef  PCIE_BOOT
   boolean  err_crash_register_status;
   pcie_osal_debug(MSG_HIGH,"System notification registration begin");
   /* Register callback during error fatal */
   if(pcie_err_fatal_callback_registered == FALSE)
   {
      /* Make sure this callback is only registered once */
      err_crash_register_status = err_crash_cb_register(pcie_err_fatal);
      if(!err_crash_register_status)
      {
         pcie_osal_debug(MSG_HIGH, "Could not Register callback for Error Fatal");
      }
      else
      {
         pcie_err_fatal_callback_registered = TRUE;
         pcie_osal_debug(MSG_HIGH, "Registered callback for Error Fatal");
      }

      if(!err_crash_cb_reg_next_to_STM(pcie_errfatal_early))
      {
         pcie_osal_debug(MSG_ERR, "Could not register err fatal early callback");
      }
   }
   pcie_osal_debug(MSG_HIGH,"System notification registration end");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_get_link_status
** ============================================================================
*/
/**
 * Returns the status of the link.The link can be in one of the three possible
 * states as defined in pcie_linkstate_type.
 *
 * @param[in,out]     pcie_link_info   Pointer to link info
 *
 * @return            pcie_linkstatus_type
 *    Returns the status of the link.
 */
pcie_linkstatus_type pcie_i_get_link_status(pcie_linkinfo_type* pcie_link_info)
{
   pcie_linkstatus_type link_status = pcie_core_get_link_status(&pcie_link_info->link_state);
   pcie_link_info->link_state = (pcie_core_linkstate_type)pcie_i_read_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_LTSSM, LTSSM_STATE));

   if(PCIE_LINKSTATUS_UP != link_status)
   {
      return(link_status);
   }

   /* Check if the link is up and BME is set */
   pcie_link_info->enum_status = pcie_core_isenumerated();
   return((TRUE == pcie_link_info->enum_status) ? PCIE_LINKSTATUS_ENUMERATED : PCIE_LINKSTATUS_UP);
}

/* ============================================================================
**  Function : pcie_i_execute_linkup_sequence
** ============================================================================
*/
/**
 * Performs PCIe linkup sequence and enumeration
 *
 * @param[in]         void
 *
 * @return            pcie_return_status
 *    Status of the operation
 */
pcie_return_status pcie_i_execute_linkup_sequence(void)
{
   uint64 zero_time;
   /* Assert Wake to notify the Host */
   pcie_i_assert_wake();

   /* In SBL we expect all gpios references to be high. */
   /* Wait for PERST deassertion */
   zero_time=pcie_osal_get_systime_ms();
   while(PCIE_GPIO_HIGH != pcie_gpio_get_state(PCIE_GPIO_PERST))
   {
      if((pcie_osal_get_systime_ms() - zero_time) > PCIE_PERST_TIMEOUT_MS)
      {
         pcie_osal_debug(MSG_FATAL, "Timed out PERST");
         return(PCIE_FAILED);
      }
   }

   pcie_osal_debug(MSG_HIGH, "PERST pin is HIGH");

   /* Deassert Wake after PERST is deasserted */
   pcie_i_deassert_wake();

   /* Initialize the PHY */
   if(PCIE_SUCCESS != pcie_phy_init())
   {
      pcie_osal_debug(MSG_FATAL, "pcie_phy_init failed");
      return(PCIE_FAILED);
   }

   /* Configure and Intialize the PCIe core */
   pcie_core_config();

   /* Finally enable the link training and enumeration */
   pcie_osal_debug(MSG_HIGH, "Starting enumeration");

   /* Start Link Training */
   pcie_enable_ltssm();

   pcie_osal_debug(MSG_HIGH, "PCIE_LINKSTATUS_ENUMERATED");

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_i_post_linkup_sequence
** ============================================================================
*/
/**
 * Performs post linkup Initializations and notifications
 *
 * @param[in]         pcie_boot_chain   Type of boot chain
 *
 * @return            pcie_return_status
 */
pcie_return_status pcie_i_post_linkup_sequence(pcie_boot_chain_type pcie_boot_chain)
{

   /* Register for PERST ISR */
   pcie_gpio_register_isr(PCIE_GPIO_PERST, pcie_i_perst_isr, LEVEL_LOW, (uint32)&pcie_dev->perst_in_expect);

   /* Initialize the DMA */
   if(PCIE_FAILED == pcie_dma_init())
   {
      pcie_osal_debug(MSG_FATAL, "PCIe DMA init failed");
      return PCIE_FAILED;
   }

   /* Collect the PCIe full dump at Init. All clocks are already enabled at this point.
      so no need to enable clks */
   pcie_osal_debug(MSG_HIGH, "Saving inital PCIe core dump");
   pcie_core_dump(&pcie_dev->pcie_coredump, PCIE_FULL_DUMP);

   if(PCIE_FULL_BOOT == pcie_boot_chain)
   {
      /* Configure and Intialize the PCIe core */
      pcie_core_config();

      /* Do additional l1ss settings and phy config update */
      pcie_i_enable_additional_l1ss_settings();

      /* Enable L1ss */
      pcie_core_req_enter_l1();

      /* Send Default LTR value .
         Should be done at the end after doing the PHY config update
         since this enables L1ss after sending LTR */
      pcie_send_ltr(pcie_dev->pcie_default_ltr);

   }
   return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_i_check_for_l1ss
** ============================================================================
*/
/**
 * Checks if link is in L1ss. Will flag an error otherwise
 *
 * @param[in]         void
 *
 * @return            pcie_return_status
 */
pcie_return_status pcie_i_check_for_l1ss(void)
{
   /* If not in L1ss return */
   if(FALSE == pcie_core_is_in_l1ss())
   {
#ifdef PCIE_CLKREQ_INTR_SUPPORTED
      pcie_osal_debug(MSG_FATAL, "Link not in L1ss");
#else
      pcie_osal_debug(MSG_FATAL, "Link not in L1ss");
#endif /* PCIE_CLKREQ_INTR_SUPPORTED */
      return(PCIE_FAILED);
   }
   return (PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_i_exit_lpm
** ============================================================================
*/
/**
 * Request PCIe core to exit low power mode
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_i_exit_lpm(void)
{
#ifndef PCIE_BOOT
   pcie_osal_syncenter(&pcie_dev->pcie_sync);
   pcie_core_exit_lpm();
   /* Enable Global Interrupt after turning on clocks */
   pcie_interrupt_ctrl(pcie_dev->pcie_irq[PCIE_GLOBAL_INT].pcie_irq_num, TRUE);
   pcie_osal_syncleave(&pcie_dev->pcie_sync);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_i_enter_lpm
** ============================================================================
*/
/**
 * Checks if CLKREQ is deasserted before registering for ISR.
 * Else turn on clocks and return failed
 *
 * @param[in]         void
 *
 * @return            pcie_return_status
 */
pcie_return_status pcie_i_enter_lpm(void)
{
#ifndef PCIE_BOOT
#ifdef PCIE_CLKREQ_INTR_SUPPORTED
   pcie_osal_intlock();
   /* GPIO LOW means that CLKREQ is asserted and core should not enter lpm */
   if(PCIE_GPIO_LOW == pcie_gpio_get_state(PCIE_GPIO_CLKREQ))
   {
      pcie_osal_debug(MSG_FATAL, "pcie_get_clkreq_gpio_state is low");
      pcie_dev->guard_time_violation_count++;
      pcie_osal_intfree();
      return (PCIE_FAILED);
   }

   /* Switch the function to SW controlled */
   pcie_gpio_switch_function(PCIE_GPIO_CLKREQ, 0);

   /* GPIO LOW means that CLKREQ is asserted and core should not enter lpm */
   if(PCIE_GPIO_LOW == pcie_gpio_get_state(PCIE_GPIO_CLKREQ))
   {
      /* Switch the function to HW controlled */
      pcie_gpio_switch_function(PCIE_GPIO_CLKREQ, pcie_dev->pcie_gpio_config_ptr[PCIE_GPIO_CLKREQ].func);
      pcie_osal_debug(MSG_FATAL, "pcie_get_clkreq_gpio_state is low after gpio function switch");
      pcie_dev->guard_time_violation_count++;
      pcie_osal_intfree();
      return (PCIE_FAILED);
   }

#endif /* PCIE_CLKREQ_INTR_SUPPORTED */

   /* Disable Global Interrupt before turning off clocks */
   pcie_interrupt_ctrl(pcie_dev->pcie_irq[PCIE_GLOBAL_INT].pcie_irq_num, FALSE);
   /* Sync not required due to INTLOCK */
   pcie_core_enter_lpm();

#ifdef PCIE_CLKREQ_INTR_SUPPORTED

   /* Switch the function to HW controlled */
   pcie_gpio_switch_function(PCIE_GPIO_CLKREQ, pcie_dev->pcie_gpio_config_ptr[PCIE_GPIO_CLKREQ].func);

   /* GPIO LOW means that CLKREQ is asserted and core should not enter lpm. Revert whatever the fucntion has done till here */
   if(PCIE_GPIO_LOW == pcie_gpio_get_state(PCIE_GPIO_CLKREQ))
   {
      /* Sync not required due to INTLOCK */
      pcie_core_exit_lpm();
      /* Enable Global Interrupt after turning on clocks */
      pcie_interrupt_ctrl(pcie_dev->pcie_irq[PCIE_GLOBAL_INT].pcie_irq_num, TRUE);

      pcie_osal_debug(MSG_FATAL, "pcie_get_clkreq_gpio_state is low. Turning on clocks");
      pcie_dev->guard_time_violation_count++;
      pcie_osal_intfree();
      return (PCIE_FAILED);
   }

   /* If there are pending global_int from the gic, do not enter lpm. Revert whatever the fucntion has done till here */
   if(TRUE == pcie_interrupt_is_irq_set(pcie_dev->pcie_irq[PCIE_GLOBAL_INT].pcie_irq_num))
   {
      /* Sync not required due to INTLOCK */
      pcie_core_exit_lpm();

      /* Enable Global Interrupt after turning on clocks */
      pcie_interrupt_ctrl(pcie_dev->pcie_irq[PCIE_GLOBAL_INT].pcie_irq_num, TRUE);

      pcie_osal_debug(MSG_HIGH, "Pending Global interrupt");
      pcie_osal_intfree();
      return (PCIE_FAILED);
   }

   /* Register for CLKREQ ISR */
   pcie_gpio_register_isr(PCIE_GPIO_CLKREQ, pcie_i_clkreq_isr, LEVEL_LOW, NULL);

   pcie_osal_intfree();
#endif /* PCIE_CLKREQ_INTR_SUPPORTED */
#endif /* PCIE_BOOT */
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_i_lock_region
** ============================================================================
*/
/**
 * Locks specified regions in AXI space
 *
 * @param[in]      offset   Start of the region to be locked
 * @param[in]      size     Size of the region
 *
 * @return         void
 */
void pcie_i_lock_regions(uint32 offset, uint32 size)
{
#ifdef PCIE_PERIPH_MEM_LOCK_ENABLED
   pcie_res_tz_lock_regions(offset, size);
#else
   pcie_core_enable_ecam_blocker(offset, size);
#endif /* PCIE_PERIPH_MEM_LOCK_ENABLED */
}

/* ============================================================================
**  Function : pcie_i_unlock_regions
** ============================================================================
*/
/**
 * Unlocks locked regions in AXI space
 *
 * @param[in]         void
 *
 * @return            void
 */
void pcie_i_unlock_regions(void)
{
#ifdef PCIE_PERIPH_MEM_LOCK_ENABLED
   pcie_res_tz_unlock_regions();
#else
   pcie_core_disable_ecam_blocker();
#endif /* PCIE_PERIPH_MEM_LOCK_ENABLED */
}
