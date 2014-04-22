/**
 * @file pcie_i.h
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
#ifndef __PCIE_I_H__
#define __PCIE_I_H__

#include "com_dtypes.h"
#include "pcie_gpio.h"
#include "pcie_interrupt.h"
#include "pcie_core.h"
#include "pcie_res.h"

#ifndef ENABLED
#define ENABLED 1
#endif

#ifndef DISABLED
#define DISABLED 0
#endif

#define PCIE_L1SUB_TIMER_ZERO_VAL  0x0         /** L1ss zero value */

/** Field Macro
 *
 * @note Can be used only as a param to a pcie_read/write_reg_field!!
 */
#define PCIE_FLD(reg,field) reg, reg##_##field##_MASK, reg##_##field##_SHFT

/** Mask macro */
#define PCIE_MASK(reg,field) ( reg##_##field##_MASK )

/** Shift Macro */
#define PCIE_SHFT(reg,field) ( reg##_##field##_SHFT )

/** Field Value */
#define PCIE_FVAL(reg, field, val) (((uint32)(val) << ( reg##_##field##_SHFT )) & ( reg##_##field##_MASK ))

/** Various status codes returned by PCIe APIs */
typedef enum _pcie_return_status
{
   PCIE_SUCCESS          = 0x0, /** < -- Operation was a success */
   PCIE_FAILED           = 0x1, /** < -- Operation failed */
   PCIE_INVALID_CONFIG   = 0x2, /** < -- Invalid Config provided */
   PCIE_INVALID_BUFSIZE  = 0X3, /** < -- Invalid buffer size for dump */
   PCIE_IPA_NOT_READY    = 0x4  /** < -- IPA not ready */
}pcie_return_status;

/** Link Status indicator structure */
typedef struct _pcie_link_info_type
{
   pcie_core_linkstate_type link_state;    /** < -- State of the PHY link */
   uint32                   enum_status;   /** < -- State of the enumeration */
}pcie_linkinfo_type;

/** PCIe PERST callback parameter */
typedef struct pcie_perst_cb_param
{
   uint8 expected_perst;          /** < -- Expected PERST state */
}pcie_perst_cb_param_type;

/** Device Info structure */
typedef struct _pcie_devinfo_type
{
   uint32 parf_base;                                    /** < -- Base Address of the PARF */
   uint32 parf_base_phys;                               /** < -- Physical Base Address of the PARF */
   uint32 phy_base;                                     /** < -- Base Address of the PHY */
   uint32 phy_base_phys;                                /** < -- Physical Base Address of the PHY */
   uint32 memmap_base;                                  /** < -- Base Address of the PCIE AXI Region */
   uint32 memmap_base_phys;                             /** < -- Physical Base Address of the PCIE AXI Region */
   uint32 dbi_base;                                     /** < -- Base Address of the DBI */
   uint32 dbi_base_phys;                                /** < -- Physical Base Address of the DBI */
   uint32 dbi_reg_base_offs;                            /** < -- Offset of the DBI */
   uint64 parf_slv_addr_space_size;                     /** < -- PARF Slave Address Space size */
   uint32 elbi_base;                                    /** < -- Base Address of the ELBI */
   uint32 elbi_base_phys;                               /** < -- Physical Base Address of the ELBI */
   uint32 mhi_base;                                     /** < -- Base Address of the MHI */
   uint32 mhi_base_phys;                                /** < -- Physical Base Address of the MHI */

   uint32 pcie_req_l1_cnt;                              /** < -- Count for l1ss entry/exit */
   uint32 pcie_link_down_counter;                       /** < -- Debug counter for link down */
   uint32 pcie_link_up_counter;                         /** < -- Debug counter for link up */

   pcie_irqs_type pcie_irq[PCIE_MAX_IRQ];               /** < -- Handle for PCIe IRQs */

   pcie_gpio_cfg_type *pcie_gpio_config_ptr;            /** < -- Handle for PCIe GPIOs */

   uint32 max_bars;                                     /** < -- Max number of bars */
   uint32 link_timeout;                                 /** < -- Link timeout */

   pcie_perst_cb_param_type  perst_in_expect;           /** Holds the expected PERST */

   uint32 pcie_enumeration_timeout;                     /** < -- PCIe enumeration timeout */
   uint32 pcie_default_ltr;                             /** < -- PCIe default LTR */

   pcie_res_npa_clk_param_type pcnoc;                   /** < -- PCNOC NPA parameters */
   pcie_res_npa_clk_param_type snoc;                    /** < -- SNOC NPA parameters */

   pcie_res_npa_rails_param_type cx;                    /** < -- CX NPA parameters */
   pcie_res_npa_rails_param_type mx;                    /** < -- MX NPA parameters */
   
   uint32 pcnoc_turbo_freq_mhz;                         /** < -- PCNOC Turbo Frequency in MHz */

   pcie_coredump_type pcie_coredump;                    /** < -- PCIe Coredump */

   pcie_osal_sync_type pcie_sync;                       /** < -- PCIe sync for PM callback and clocks */

   boolean pcie_clocks_on;                              /** < -- Specifies if clocks are on */
   uint32 dev_version;                                  /** < -- Holds the Chip version */
   uint32 guard_time_violation_count;                   /** < -- Count for guard time violation */
   uint32 pcie_sw_int_mask;                             /** < -- Holds the current SW interrupt mask */
}pcie_devinfo_type;

extern pcie_devinfo_type *pcie_dev;

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
void pcie_i_write_reg(uint32 base, uint32 offset, uint32 val);

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
void pcie_i_write_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift, uint32 field_val);

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
uint32 pcie_i_read_reg(uint32 base, uint32 offset);

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
uint32 pcie_i_read_reg_field(uint32 base, uint32 offset, uint32 mask, uint32 shift);

/* ============================================================================
**  Function : pcie_i_assert_wake
** ============================================================================
*/
/**
 *  Assert PCIe GPIO WAKE# to wake up Host.
 *
 * @param[in]      void
 *
 * @return     void
 */
void pcie_i_assert_wake(void);

/* ============================================================================
**  Function : pcie_i_deassert_wake
** ============================================================================
*/
/**
 * De-assert PCIe GPIO WAKE#.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_i_deassert_wake(void);

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
void pcie_i_init(void);

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
void pcie_i_pm_sync_deinit(void);

/* ============================================================================
**  Function : pcie_i_register_link_notification_cb
** ============================================================================
*/
/**
 * Registers link notification call back from upper layers.
 *
 * @param[in,out]      link_notification_cb    Callback function
 * @param[in,out]      param                   Pointer to parameters provided by client.
 *
 * @return             void
 */
void pcie_i_register_link_notification_cb(pcie_i_link_notification_cb_fn_type link_notification_cb, void *param);

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
void pcie_i_sw_int_ctrl(pcie_core_sw_int_id_type sw_id,boolean ctrl);

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
void pcie_i_enable_sw_int(void);

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
void pcie_i_global_int_handler(void *pcie_dev_r);

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
void pcie_i_mhi_int_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

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
 * @return         void
 */
void pcie_i_link_down_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

/* ============================================================================
**  Function : pcie_i_link_up_handler
** ============================================================================
*/
/**
 * Handle PCIe link up .
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_link_up_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

/* ============================================================================
**  Function : pcie_i_device_state_change_handler
** ============================================================================
*/
/**
 * PCIe device state change interrupt handler.
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_device_state_change_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

/* ============================================================================
**  Function : pcie_i_l1_inact_timeout_handler
** ============================================================================
*/
/**
 * PCIe L1SS inactivity timer interrupt handler.
 *
 * @param[in,out]    pcie_dev_r     Pointer to pcie_dev info
 * @param[in]        sw_int_id      SW Interrupt Id
 *
 * @return           void
 */
void pcie_i_l1_inact_timeout_handler(void *pcie_dev_r, pcie_core_sw_int_id_type sw_int_id);

/* ============================================================================
**  Function : pcie_i_is_pcie_fuse_disabled
** ============================================================================
*/
/**
 * Return True if PCIe fuse is disabled.
 *
 * @param[in,out]    void
 *
 * @return           boolean
 *    True if fuse is disabled else returns false
 */
boolean  pcie_i_is_pcie_fuse_disabled(void);

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
void pcie_i_register_system_notifications(void);

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
pcie_linkstatus_type pcie_i_get_link_status(pcie_linkinfo_type* pcie_link_info);

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
pcie_return_status pcie_i_execute_linkup_sequence(void);

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
pcie_return_status pcie_i_post_linkup_sequence(pcie_boot_chain_type pcie_boot_chain);

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
pcie_return_status pcie_i_check_for_l1ss(void);

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
pcie_return_status pcie_i_enter_lpm(void);

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
void pcie_i_exit_lpm(void);

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
void pcie_i_lock_regions(uint32 offset, uint32 size);

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
void pcie_i_unlock_regions(void);

#endif /* __PCIE_I_H__ */
