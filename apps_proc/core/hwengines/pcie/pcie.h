/**
 * @file pcie.h
 * @brief
 * This header files defines the public interface to the PCIe Device Driver
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_H__
#define __PCIE_H__

#include "comdef.h"

/** GPIO pin value */
typedef enum _pcie_pin_value_type
{
   PCIE_PIN_LOW  = 0,        /** < -- GPIO Pin value is low */
   PCIE_PIN_HIGH = 1         /** < -- GPIO Pin value is high */
}pcie_pin_value_type;

/** PCIe read/write options */
#define PCIE_RW_OPTION_DEFAULT 0 /** Default: No special options set  */
#define PCIE_RW_PRIORITY_HIGH  1 /** read/write has high priority. Priority is low by default */

/** Various status codes returned by PCIe APIs */
typedef enum _pcie_status_type
{
   PCIE_STATUS_SUCCESS               = 0x0,    /** < -- Operation succeeded */
   PCIE_STATUS_ERROR                 = 0x1,    /** < -- Operation met an error */
   PCIE_STATUS_INVALIDPARAMS         = 0x2,    /** < -- Invalid paramaters were provided */
   PCIE_STATUS_MSIDISABLED           = 0x3,    /** < -- MSI is not enabled by the host */
   PCIE_STATUS_LINKTIMEOUT           = 0x4,    /** < -- Link Training timed out */
   PCIE_STATUS_UNEXPLINKDOWN         = 0x5,    /** < -- Unexpected link down */
   PCIE_BOOT_DISABLED                = 0x6,    /** < -- PCIe boot disabled */
   PCIE_FUSE_DISABLED                = 0x7,    /** < -- PCIe fuse disabled */
   PCIE_TRANSFER_REQUESTS_FULL       = 0x8,    /** < -- PCIe cannot accomodate more transfer requests */
   /* PHY Error Codes */
   PCIE_STATUS_PHYINITFAILED         = 0x700,  /** < -- PHY initialization failed */
   /* Misc Error Codes */
   PCIE_STATUS_CLKCFGFAILED          = 0x800   /** < -- Clock Configuration failed */
}pcie_status_type;

/** PCIe Link States */
typedef enum pcie_link_state
{
   PCIE_LINK_STATE_LINK_DOWN,     /** < -- Link Down */
   PCIE_LINK_STATE_L0,            /** < -- Link in L0 */
   PCIE_LINK_STATE_L0S,           /** < -- Link in L0s */
   PCIE_LINK_STATE_L1,            /** < -- Link in L1 */
   PCIE_LINK_STATE_L1SS,          /** < -- Link in L1SS */
   PCIE_LINK_STATE_L2,            /** < -- Link in L2 */
   PCIE_LINK_STATE_OTHERS         /** < -- Other Link states */
}pcie_link_state_type;

/** PCIe Notifications */
typedef enum _pcie_event_notification_type
{
   PCIE_PM_DEVICE_STATE_IN_D0,        /** < -- Device in D0 */
   PCIE_PM_DEVICE_STATE_IN_D3_HOT,    /** < -- Device in D3 Hot */
   PCIE_PM_DEVICE_STATE_IN_D3_COLD,   /** < -- Device in D3 Cold */
   PCIE_PM_L1_INACT_TIMEOUT,          /** < -- L1 Inactivity Timeout */
   PCIE_PM_PERST_DEASSERTED,          /** < -- PERST Deasserted */
   PCIE_LINK_UP,                      /** < -- Link up */
   PCIE_LINK_DOWN,                    /** < -- Link down */
   PCIE_CLKREQ_ASSERTED,              /** < -- CLKREQ Asserted */
   PCIE_MHI_INT,                      /** < -- MHI_A7 */
   PCIE_MMIO_WRITE,                   /** < -- MMIO Write */
   PCIE_TO_MHI_PM_MAX                 /** < -- Reserved */
}pcie_event_notification_type;

/** PCIe Address */
typedef struct _pcie_address_type
{
   uint32 va_hi;               /** < -- Upper 32 bits of the Virtual Address */
   uint32 va_low;              /** < -- Lower 32 bits of the Virtual Address */
   uint32 pa_hi;               /** < -- Upper 32 bits of the Physical Address */
   uint32 pa_low;              /** < -- Lower 32 bits of the Physical Address */
}pcie_address_type;

/** MSI Config Structure */
typedef struct _pcie_msiconfig_type
{
   uint32 enabled;                  /** < -- Flag to indicate whether MSI is enabled or not */
   uint32 addr_low;                 /** < -- Lower 32 bits of the MSI Address */
   uint32 addr_hi;                  /** < -- Upper 32 bits of the MSI Address */
   uint32 data;                     /** < -- Data Pattern to use when generating the MSI */
   pcie_address_type local_addr;    /** < -- Local address to access the MSI address */
   uint32 num_multi_msgs;           /** <- Number of messages assigned by the host */
}pcie_msiconfig_type;

/** IATU Entry Structure */
typedef struct
{
   pcie_address_type host_addr;      /** < -- Host Address */
   pcie_address_type *local_addr;    /** < -- Pointer to Local Address */
   uint64 size;                      /** < -- Size */
}pcie_iatu_entry_type;

/** PCIe DMA read/write Request type */
typedef struct
{
   uint32 src_addr;             /** < -- Source Address */
   uint32 dest_addr;            /** < -- Destination Address */
   uint16 size;                 /** < -- Size */
   void *user_data;             /** < -- User data */
   pcie_status_type result;     /** < -- Result */
}dma_read_write_req_type;

/** DMA request callback */
typedef void (*dma_req_cb)(dma_read_write_req_type dma_req);
/** Link Notification callback function type */
typedef void (*pcie_i_link_notification_cb_fn_type)(pcie_event_notification_type pm_notify, void *ctx);

/* ============================================================================
**  Function : pcie_reg_dump
** ============================================================================
*/
/**
 * Calls the core dump.
 *
 * @param[in,out]     dump_addr  Address where the registers are dumped .
 * @param[in]         max_size  Maximum allowed size for the dump .
 *
 * @return            void
 *
 * @dependencies
 * None.
 */
void pcie_reg_dump(void *dump_addr, uint32 max_size);

/* ============================================================================
**  Function : pcie_init
** ============================================================================
*/
/**
 * Initializes the PCIe Link,PCIe MAC and the PHY layer and starts link enumeration.
 *
 * @param[in]         void
 *
 * @return            pcie_status_type
 *    Status of the initialization.
 *
 * @dependencies
 * None.
 */
pcie_status_type pcie_init(void);

/* ============================================================================
**  Function : pcie_remove_index_obatu
** ============================================================================
*/
/**
 * Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 *
 * @param[in]      index     Index to be removed from IATU.
 *
 * @return         void
 */
void  pcie_remove_index_obatu(uint8 index);

/* ============================================================================
**  Function : pcie_read_pa
** ============================================================================
*/
/**
 * Reads data from the host memory to a physical address via PCIe link.
 *
 * @param[in,out]     buf             Physical Buffer to read into
 * @param[in]         pcie_axi_addr   PCIe AXI address in modem address space
 * @param[in]         size            Size of the data to be read in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided read options
 *
 * @return        uint32
 *    PCIE_STATUS_SUCCESS - Successfully .
 *    PCIE_STATUS_ERROR - An error code .
 */
uint32 pcie_read_pa(void *buf, uint32 pcie_axi_addr, uint32 size, void *user_data, uint32 options);

/* ============================================================================
**  Function : pcie_write_pa
** ============================================================================
*/
/**
 * Writes data to the host memory using a physical buffer
 * Writes data to the host memory via PCIe link.
 *
 * @param[in,out]     buf             Buffer that contains data to be written  into host
 * @param[in]         pcie_axi_addr   PCIe AXI virtual address in modem address space
 * @param[in]         size            Size of the data to be written in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided write options
 *
 * @return        uint32
 *    PCIE_STATUS_SUCCESS - Successfully .
 *    PCIE_STATUS_ERROR - An error code .
 */
uint32 pcie_write_pa(void *buf, uint32 pcie_axi_addr, uint32 size, void *user_data, uint32 options);

/* ============================================================================
**  Function : pcie_set_mhi_ipa_dbs_addr
** ============================================================================
*/
/**
 * Set registers for routing doorbells
 *
 * @param[in]     cdb_base            Channel doorbell base
 * @param[in]     cdb_end             Channel doorbell end
 * @param[in]     edb_base            Event doorbell base
 * @param[in]     edb_end             Event doorbell end
 * @param[in]     mhi_ipa_cdb_addr    MHI IPA channel Doorbell Target address
 * @param[in]     mhi_ipa_edb_addr    MHI IPA event Doorbell Target address
 *
 * @return        void
 */
void pcie_set_mhi_ipa_dbs_addr(uint8 cdb_base, uint8 cdb_end, uint8 edb_base, uint8 edb_end, uint32 mhi_ipa_cdb_addr, uint32 mhi_ipa_edb_addr);

/* ============================================================================
**  Function : pcie_get_msi_config
** ============================================================================
*/
/**
 * Return MSI configuration information.
 *
 * @param[in,out]      pcie_msi_info     PCIE MSI information.
 *
 * @return         void
 */
void pcie_get_msi_config(pcie_msiconfig_type *pcie_msi_info);

/* ============================================================================
**  Function : pcie_is_pcie_boot_enabled
** ============================================================================
*/
/**
 * Checks if PCIe boot is enabled.
 *
 * @param[in]      void
 *
 * @return         Boolean
 *    TRUE  - if PCIe boot is enabled
 *    FALSE - if PCIe boot is not enabled
 */
boolean  pcie_is_pcie_boot_enabled(void);

/* ============================================================================
**  Function : pcie_register_link_notifcation_cb
** ============================================================================
*/
/**
 * Register link notification call back with PCIe driver.
 *
 * @param[in,out]      pm_cb         Call back function
 * @param[in,out]      param         pointer to parameters provided by client.
 *
 * @return         void
 */
void pcie_register_link_notifcation_cb(pcie_i_link_notification_cb_fn_type pm_cb, void *param);

/* ============================================================================
**  Function : pcie_wakeup_host
** ============================================================================
*/
/**
 * Assert GPIO siganl WAKE# to wakeup host.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_wakeup_host(void);

/* ============================================================================
**  Function : pcie_enable_ltssm
** ============================================================================
*/
/**
 * API to enable LTSSSM.
 *
 * @param[in]      void
 *
 * @return         void
 *
 * @SideEffect
 * wake gpio is deasserted
 */
void pcie_enable_ltssm(void);

/* ============================================================================
**  Function : pcie_read_perst_pin_value
** ============================================================================
*/
/**
 * Reads and returns the current value on the perst pin input.
 *
 * @param[in]  void
 *
 * @return     pcie_pin_value_type
 *    PCIE_PIN_HIGH - if perst pin is high
 *    PCIE_PIN_LOW  - if perst pin is low
 */
pcie_pin_value_type pcie_read_perst_pin_value(void);

/* ============================================================================
**  Function : pcie_enable_endpoint
** ============================================================================
*/
/**
 * Handle PCIe link reestablishment.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_enable_endpoint(void);

/* ============================================================================
**  Function : pcie_disable_endpoint
** ============================================================================
*/
/**
 * Handle PCIe link down.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_disable_endpoint(void);

/* ============================================================================
**  Function : pcie_get_link_state
** ============================================================================
*/
/**
 * Return current PCIe link state.
 *
 * @param[in]      void
 *
 * @return         pcie_link_state_type
 *    PCIE_LINK_STATE_LINK_DOWN
 *    PCIE_LINK_STATE_L0
 *    PCIE_LINK_STATE_L0S
 *    PCIE_LINK_STATE_L1
 *    PCIE_LINK_STATE_L1SS
 *    PCIE_LINK_STATE_L2
 *    PCIE_LINK_STATE_OTHERS
 */
pcie_link_state_type pcie_get_link_state(void);

/* ============================================================================
**  Function : pcie_trigger_perst_int
** ============================================================================
*/
/**
 * Trigger PERST GPIO interrupt.. Used for internal testing purpose only.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_trigger_perst_int(void);

/* ============================================================================
**  Function : pcie_change_device_state
** ============================================================================
*/
/**
 * Change device state from EP driver side. Used only for internal testing purpose.
 *
 * @param[in]      d_state device state( o for D0 state, 3 for D3 state)
 *
 * @return         void
 */
void pcie_change_device_state(uint8 d_state);

/* ============================================================================
**  Function : pcie_send_ltr
** ============================================================================
*/
/**
 * Sends the LTR.
 *
 * @param[in]      ltr_us    LTR value in usecs (max value is ~ 30 seconds)
 *
 * @return         void
 */
void pcie_send_ltr(uint32 ltr_us);

/* ============================================================================
**  Function : pcie_control_l1ss_inact_interrupt
** ============================================================================
*/
/**
 * API to enable or disable L1SS inactivity timer interrupt.
 *
 * @param[in]       ctrl        True to enable interrut,False to disable interrupt.
 * @param[in]       maxtimer    Max time out value in us
 *
 * @return          uint32      Mininum time in us in which the timer can expire
 */
uint32 pcie_control_l1ss_inact_interrupt(boolean ctrl,uint32 maxtimer);

/* ============================================================================
**  Function : pcie_set_host2dev_mmap
** ============================================================================
*/
/**
 * Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 *
 * @param[in]      atu_array      Array of ATU configuration
 * @param[in]      entry_count     atu_array size
 *
 * @return         uint16
 *    Status of the operation
 */
int16 pcie_set_host2dev_mmap(pcie_iatu_entry_type atu_array[], uint8 entry_count);

/* ============================================================================
**  Function : pcie_pm_enter_lpm
** ============================================================================
*/
/**
 * Request PCIe to enter low power mode
 *
 * @param[in]         void
 *
 * @return            pcie_status_type
 *    PCIE_STATUS_ERROR    if the clkreq gpio is ASSERTED(low) while enabling interrupt
 *    PCIE_STATUS_SUCCESS  if successful.
 */
pcie_status_type pcie_pm_enter_lpm(void);

/* ============================================================================
**  Function : pcie_pm_exit_lpm
** ============================================================================
*/
/**
 * Request PCIe to exit low power mode
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_pm_exit_lpm(void);

/* ============================================================================
**  Function : pcie_set_dma_state
** ============================================================================
*/
/**
 * Used by upper layer client to indicate if it suspended the DMA
 *
 * @param[in]      state      ENABLED/DISABLED
 *
 * @return         void
 */
void pcie_set_dma_state(boolean state);

/* ============================================================================
**  Function : pcie_register_dma_cb
** ============================================================================
*/
/**
 * Register callbacks for DMA
 *
 * @param[in,out]      read_cb      Read callback
 * @param[in,out]      write_cb     Write callback
 *
 * @return         void
 */
void pcie_register_dma_cb(dma_req_cb read_cb, dma_req_cb write_cb);

/* ============================================================================
**  Function : pcie_device_enable_EnterL23
** ============================================================================
*/
/**
 * Send the ready enter L23 DLLP to the RC. This should be done when device
 * is in D3hot state and is ready to enter D3cold.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_device_enable_EnterL23(void);

/* ============================================================================
 **  Function : pcie_flush_dma
 ** ============================================================================
 */
/**
 * Setup a transaction to flush out all descriptors
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_flush_dma(void);

/* ============================================================================
**  Function : pcie_poll_dma
** ============================================================================
*/
/**
 * API for PCIe clients to poll for dma request
 *
 * @param[in]     Void
 *
 * @return
 *     0 - Successful
 *    -1 - Error
 */
int pcie_poll_dma(void);

/* ============================================================================
**  Function : pcie_enable_mhi_interrupts
** ============================================================================
*/
/**
 * Enable MHI core interrupts.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_enable_mhi_interrupts(void);

/* ============================================================================
**  Function : pcie_disable_mhi_interrupts
** ============================================================================
*/
/**
 * Disable MHI core interrupts.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_disable_mhi_interrupts(void);

/* ============================================================================
**  Function : pcie_l1ss_supported
** ============================================================================
*/
/**
 * Returns whether L1ss is supported
 *
 *
 * @return
 *     0 - False
 *     1 - True            
 *
 * @dependencies
 * None.
 */
boolean pcie_l1ss_supported(void);
#endif /* __PCIE_H__ */

