#ifndef MHILINK_H
#define MHILINK_H

/**
  @file mhi_link.h
  @brief
  This file defines the interface for the MHI LInk Abstraction component


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
#include "mhi.h"
#include "mhi_i.h"
#include "mhi_osal.h"

/* Inactivity timer counter value to program */
#define LINK_MIN_INACT_TIME_US           500        /**< Min Link inactivity value in us */
#define LINK_MAX_INACT_TIME_US           50000      /**< Max Link inactivity value in us */
#define MHI_PCNOC_NOMINAL_MODE_SPEED_MHZ 100        /**< PCNOC Nominal Frequency in MHz */
#define MHI_PCNOC_TURBO_MODE_SPEED_MHZ   133        /**< PCNOC Turbo Frequency in MHz */
#define DISABLE_INACTIVITY               0x0        /**< This value is programmed when the timer is to be disabled */

#define MIN_INACTIVITY          LINK_MIN_INACT_TIME_US   /**< Minimum link inactivity time that should cause timeout */
#define MAX_INACTIVITY          LINK_MAX_INACT_TIME_US   /**< Maximum allowable inactivity time that should cause timeout */

/** Address map structure used to map local to host addresses */
typedef struct _addr_map
{
   mhi_addr_t   local;   /**< Local address info */
   uint64   host;        /**< Host address */
   uint64   size;        /**< Size of address region */
}addr_map_t;

/* MHI Link return type used for cb completion */
typedef struct _mhi_link_ret
{
   void* userdata;         /**< Userdata */
   uint32 addr;            /**> Address */
   uint32 size;            /**> Size of operation in bytes */
   mhi_status_t status;
}mhi_link_ret_t;

/* Callback function used for async mhi link read/writes */
typedef void(*mhi_link_cb_func)(mhi_link_ret_t ret);

/** Device power state notified by the link driver */
typedef enum _mhi_link_d_state
{
  MHI_LINK_D0=0,
  MHI_LINK_D1=1,
  MHI_LINK_D2=2,
  MHI_LINK_D3hot=3,
  MHI_LINK_D3cold=4,
  MHI_LINK_INVALID_D_STATE
} mhi_link_d_state_t;

/** MHI link related information */
typedef struct _mhi_link_info
{
  boolean mhi_link_hw_enabled;           /**< Track whether it is safe to access MMIO */
  boolean mhi_link_inact_timer_running;  /**< Track whether link inactivity timer is running */
  mhi_config_t *mhiConfig;               /**< Pointer to the MHI configuration structure */
  mhi_osal_sync_type mhi_link_notifSync; /**< Mutex to protect MHI link notifications */
  mhi_link_cb_func cbFunc;               /**< Callback function over which to send link r/w completions */
  boolean is_DMA_flush_needed;           /**< Flag tracking whether DMA requests need to be flushed */
  uint64 time_max_link_inact_armed_ms;   /**< System time in ms when link inactivity was armed with MAX_INACTIVITY */
  uint32 min_inact_expiry_time_us;       /**< Minimum time after which link inactivity timer could fire */
  mhi_osal_meminfo scratch_pool;         /** < -- Scratch pool  */
  mhi_osal_sync_type scratch_sync;       /** < -- Scratch pool sync */
  boolean hw_link_inact_timer_supported; /**< Is the hardware link inacitvity timer supported */
} mhi_link_info_t;

/**
 * @brief      mhi_link_init
 *
 *             Initialize the MHI Link layer
 *
 *
 * @param[in]  *link_info - Pointer to link information context
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_init(mhi_link_info_t *link_info);

/**
 * @brief      mhi_link_triggerinterrupt
 *
 *             Trigger an interrupt over the link
 *
 *
 * @param[in]  vec - Vector to trigger
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_triggerinterrupt(uint32 vec);

/**
 * @brief      mhi_link_write
 *
 *             Perform a write operation over the link
 *
 *
 * @param[in]  index - Address index
 * @param[in]  destAddr  - Destination address
 * @param[in]  *srcAddr  - Source address
 * @param[in]  size      - Size of write in bytes
 * @param[in]  *userdata - User data to associate with write
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_write(uint32 index,uint64 destAddr, mhi_addr_t* srcAddr, uint32 size, void* userdata );

/**
 * @brief      mhi_link_read
 *
 *             Perform a read operation over the link
 *
 *
 * @param[in]  index      - Address index
 * @param[in]  srcAddr    - Source address
 * @param[in]  *destAddr  - Destination address
 * @param[in]  size       - Size of operation in bytes
 * @param[in]  *userdata  - User supplied data to associate with operation
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_read(uint32 index, uint64 srcAddr, mhi_addr_t* destAddr, uint32 size, void *userdata);

/**
 * @brief      mhi_link_reset
 *
 *             Reset the link layer
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_reset(void);

/**
 * @brief      mhi_link_maphostaddrspace
 *
 *             Map multiple host base to limit addresses
 *
 *
 * @param[in]  *base   - Host base address array
 * @param[in]  *limit  - Limit address array
 * @param[out] **local - Local Address information obtained from underlying transport
 * @param[in]  nMap    - Number of mappings
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_maphostaddrspace(uint64 *base, uint64 *limit, mhi_addr_t **local, uint32 nMap);

/**
 * @brief       mhi_link_wakeup_req
 *
 *              Abstract link API that actually does the wake up procedure.
 *
 *
 * @dependencies
 *              mhi_link_init() must be called first.
 *
 * @return
 *              MHI_STATUS if successful.
 *              MHI_ERROR  if error occurred.
 *
 * @sideeffect  None
 */
mhi_status_t mhi_link_wakeup_req(void);

/**
 * @brief       mhi_link_enable
 *
 *              Abstract link API that re-enables link hardware after a power collapse.
 *
 *
 * @dependencies
 *              Should be called only after PERST de-assertion notification is received.
 *
 * @return
 *              MHI_SUCCESS MHI_ERROR
 *
 * @sideeffect  None
 */
mhi_status_t mhi_link_enable(void);

/**
 * @brief       mhi_link_disable
 *
 *              Abstract link API that disables link hardware.
 *
 *
 * @dependencies
 *              None
 *
 * @return
 *              MHI_STATUS if successful.
 *              MHI_ERROR  if error occurred.
 *
 * @sideeffect
 *              MMIO and link address space will be in accessible
 */
mhi_status_t mhi_link_disable(void);

/**
 * @brief       mhi_link_is_active
 *
 *              Determine whether the link can actively push traffic or is in some
 *              power saving state.
 *
 *
 * @dependencies
 *              mhi_link_init() should be called first.
 *
 * @return
 *              TRUE if link is active, False is not.
 *
 * @sideeffect  None
 */
boolean mhi_link_is_active(void);

/**
 * @brief       mhi_link_is_hw_enabled
 *
 *              Report whether link hw is enabled or not, i.e. that link has not
 *              turned off hardware blocks to save power. Otherwise will not be
 *              able to access MMIO.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              TRUE if link hardware is enabled, False if not.
 *
 * @sideeffect  None
 */
boolean mhi_link_is_hw_enabled(void);

/**
 * @brief       mhi_link_enable_inactivity_intr
 *
 *              Enable the L1ss inactivity timer interrupt.
 *
 *
 * @param[in]   inact_time - Amount of inactivity time should cause interrupt.
 *                           The unit of time is implementation specific
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_enable_inactivity_intr(uint32 inact_time);

/**
 * @brief       mhi_link_disable_inactivity_intr
 *
 *              Disable the L1ss inactivity timer interrupt.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_disable_inactivity_intr(void);

/**
 * @brief       mhi_link_update_latency
 *
 *              Update the tolerable latency in useconds to the link.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_update_latency(uint32 latency_us);

/**
 * @brief       mhi_link_release_hw_resources
 *
 *              Release link hardware resources. This may involve
 *              the link driver software turning off clocks and voting against
 *              hardware resources that are not required if the link will not
 *              be used. Note that when entering into D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              MHI_SUCCESS or MHI_ERROR
 *
 * @sideeffect  Link hardware is no longer accessible after this function is called.
 */
mhi_status_t mhi_link_release_hw_resources(void);

/**
 * @brief       mhi_link_acquire_hw_resources
 *
 *              Acquire link hardware resources. This may involve
 *              the link driver software turning on clocks and voting for
 *              hardware resources that are required if the link will be used.
 *              Note that when exiting out of D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              MHI_SUCCESS or MHI_ERROR
 *
 * @sideeffect  Link hardware is now accessible.
 */
mhi_status_t mhi_link_acquire_hw_resources(void);

/**
 * @brief       mhi_link_setup_hwacc_db_routing
 *
 *              Setup the doorbell routing for the hardware accelerator.
 *
 *@param[in]    chdb_mailbox_addr - Target address for routing channel doorbells to
 *              the hardware accelerator.
 *@param[in]    erdb_mailbox_addr - Target address for routing event ring doorbells
 *              to the hardware accelerator.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  Writes to the channel and event ring doorbell registers dedicated to the
 *              hardware accelerator in MMIO generate interrupts to the hardware accelerator.
 */
void mhi_link_setup_hwacc_db_routing(uint32 chdb_mailbox_addr, uint32 erdb_mailbox_addr);

/**
 * @brief       mhi_link_get_reset_line_state
 *
 *              Return the state of the reset line.
 *
 *
 * @dependencies
 *              None
 *
 * @return
 *              DEASSERTED\ASSERTED
 *
 * @sideeffect  None
 */
boolean mhi_link_get_reset_line_state(void);

/**
 * @brief      mhi_link_flush
 *
 *             Flush out any pending writes
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_flush(void);

/**
 * @brief      mhi_link_dma_flush
 *
 *             Flush out any pending DMA requests
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_dma_flush(void);

/**
 * @brief      mhi_link_notify_dma_state
 *
 *             Notify the link driver whether MHI disabled the link DMA
 *
 *
 * @param[in]  state - ENABLED/DISABLED
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_notify_dma_state(boolean state);

/**
 * @brief      mhi_link_dstate_ack
 *
 *             Acknowledge that device has entered the notified D state, and
 *             do any post processing if required
 *
 *
 * @param[in]  dstate - Device state
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_dstate_ack(mhi_link_d_state_t dstate);

/**
 * @brief      mhi_link_ctrl_core_interrupt
 *
 *             Enable or disable the MHI Core interrupts
 *
 *
 * @param[in]  ctrl    Enable or disable the interrupt
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_ctrl_core_interrupt(boolean ctrl);

/**
 * @brief      mhi_link_register_notifications_cb
 *
 *             Register for MHI link call back
 *
 *
 * @param[in]  None
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_register_notifications_cb(void);

#endif
