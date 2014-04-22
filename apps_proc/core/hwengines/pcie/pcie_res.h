/**
 * @file pcie_res.h
 * @brief
 * Handles and defines types for pcie power and clock resources.
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
#ifndef __PCIE_CLK_H__
#define __PCIE_CLK_H__

#include "com_dtypes.h"

/**
  Used to search for a matching frequency input. This type describes how
  frequency matching is done for the clock frequency selection function. It
  also indicates units in Hz, kHz, or MHz.
 */
typedef enum
{
  PCIE_CLOCK_FREQUENCY_HZ_AT_LEAST  = 0,  /**< Frequency is the minimum allowed in Hz. */
  PCIE_CLOCK_FREQUENCY_HZ_AT_MOST   = 1,  /**< Frequency is the maximum allowed in Hz. */
  PCIE_CLOCK_FREQUENCY_HZ_CLOSEST   = 2,  /**< Closest match in Hz. */
  PCIE_CLOCK_FREQUENCY_HZ_EXACT     = 3,  /**< Exact match only in Hz. */

  PCIE_CLOCK_FREQUENCY_KHZ_AT_LEAST = 0x10,  /**< Frequency is the minimum allowed in kHz. */
  PCIE_CLOCK_FREQUENCY_KHZ_AT_MOST  = 0x11,  /**< Frequency is the maximum allowed in kHz. */
  PCIE_CLOCK_FREQUENCY_KHZ_CLOSEST  = 0x12,  /**< Closest match in kHz. */
  PCIE_CLOCK_FREQUENCY_KHZ_EXACT    = 0x13,  /**< Exact match only in kHz. */

  PCIE_CLOCK_FREQUENCY_MHZ_AT_LEAST = 0x20,  /**< Frequency is the minimum allowed in MHz. */
  PCIE_CLOCK_FREQUENCY_MHZ_AT_MOST  = 0x21,  /**< Frequency is the maximum allowed in MHz. */
  PCIE_CLOCK_FREQUENCY_MHZ_CLOSEST  = 0x22, /**< Closest match in MHz. */
  PCIE_CLOCK_FREQUENCY_MHZ_EXACT    = 0x23, /**< Exact match only in MHz. */
} pcie_res_clockfreq_type;

/** ALL PCIE Clock IDs for Internal mapping within PCIe */
typedef enum
{
   PCIE_CONFIG_CLK    = 0x0,     /** < -- ID for Config AHB clock */
   PCIE_INTERFACE_MSTR_CLK,      /** < -- ID for AXI Master clock */
   PCIE_INTERFACE_SLV_CLK,       /** < -- ID for AXI slave clock */
   PCIE_PIPE_CLK,                /** < -- ID for PCIe Pipe Clock */
   PCIE_AUX_CLK,                 /** < -- ID for PCIe Aux/Sleep Clock */
   PCIE_PHY_CLK,                 /** < -- ID for PCIe PHY Clock */
   PCIE_LDO_CLK,                 /** < -- ID for PCIE LDO Clock */
   PCIE_MAX_CLK                  /** < -- Reserved */
}pcie_res_clk_id_type;

/** Clock control object */
typedef struct {
   uint32 clock_id;
}pcie_res_clock_ctl_type;

/** Structure to hold the clock name mapped in GCC */
typedef struct _pcie_res_clk_name_type
{
   char* pcie_clk_name[PCIE_MAX_CLK];       /** < -- Holds the clock string names */
}pcie_res_clk_names_type;

typedef struct _pcie_res_npa_clk_param_type
{
   char* res_name;
   char* client_name;
   uint32 freq_vote;
}pcie_res_npa_clk_param_type;

typedef struct _pcie_res_npa_rails_param_type
{
   char* res_name;
   char* client_name;
   uint32 vote;
}pcie_res_npa_rails_param_type;

/* ============================================================================
**  Function : pcie_res_set_clock_freq
** ============================================================================
*/
/**
 * Set Clock Frequency.
 *
 * @param[in] id            ID for the PCIe Clock
 * @param[in] nFreq         Clock frequency in Hz, kHz, or MHz, depending on the
 * eMatch parameter.
 * @param[in] eMatch        Type of match request.
 *
 * @return     pcie_return_status
 *    Success of the operation.
 */
enum _pcie_return_status pcie_res_set_clock_freq(pcie_res_clk_id_type id, uint32  nFreq,  pcie_res_clockfreq_type  eMatch);

/* ============================================================================
**  Function : pcie_res_init
** ============================================================================
*/
/**
 * Initialize PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_init(void);

/* ============================================================================
**  Function : pcie_res_enable
** ============================================================================
*/
/**
 * Enable PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_enable(void);

/* ============================================================================
**  Function : pcie_res_disable
** ============================================================================
*/
/**
 * Disable PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_disable(void);

/* ============================================================================
**  Function : pcie_res_suspend
** ============================================================================
*/
/**
 * Suspend votes for PCIe power and clocks.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_suspend(void);

/* ============================================================================
**  Function : pcie_res_resume
** ============================================================================
*/
/**
 * Resume vote for PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_resume(void);

/* ============================================================================
**  Function : pcie_res_clk_enable
** ============================================================================
*/
/**
 * Enable PCIe Core Clocks.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_enable(void);

/* ============================================================================
**  Function : pcie_res_clk_disable
** ============================================================================
*/
/**
 * Disable the PCIe Core clock
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_disable(void);

/* ============================================================================
**  Function : pcie_res_boot_clk_enable
** ============================================================================
*/
/**
 * Enable PCIe boot Clock.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_boot_clk_enable(void);

/* ============================================================================
**  Function : pcie_res_boot_clk_disable
** ============================================================================
*/
/**
 * Disable the PCIe Boot clock
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_boot_clk_disable(void);

#ifdef PCIE_AHB_AXI_CLK_CHECK
/* ============================================================================
**  Function : pcie_res_get_interface_clk_state
** ============================================================================
*/
/**
 * Checks if either AHB or AXI clock is off
 *
 * @param[in]  void
 *
 * @return     boolean
 *    Returns 1 if either of them is off else returns 0.
 */
boolean pcie_res_get_interface_clk_state(void);
#endif

/* ============================================================================
**  Function : pcie_res_clk_select_extern_src
** ============================================================================
*/
/**
 * Selects the external clk source based on the tgt speed.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_select_extern_src(void);

/* ============================================================================
**  Function : pcie_res_is_clock_ctl_on
** ============================================================================
*/
/**
 * Checks if the clock is on.
 *
 * @param[in]      id         ID for the PCIe Clock
 *
 * @return         boolean
 *    True if the clock is on else returns false.
 */
boolean pcie_res_is_clock_ctl_on(pcie_res_clk_id_type id);

/* ============================================================================
**  Function : pcie_res_get_clock_freq
** ============================================================================
*/
/**
 * Get Clock Frequency.
 *
 * @param[in] id            ID for the PCIe Clock
 * @param[in] nFreq         Pointer to Clock frequency in Hz
 *
 * @return    void
 */
void pcie_res_get_clock_freq(pcie_res_clk_id_type id, uint32* nFreq);

/* ============================================================================
**  Function : pcie_res_restore_tz_sys_call
** ============================================================================
*/
/**
 * Restore system call to TZ
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_restore_tz_sys_call(void);

/* ============================================================================
**  Function : pcie_res_tz_lock_regions
** ============================================================================
*/
/**
 * System call to TZ to lock regions specified
 *
 * @param[in]      offset   Start of the region to be locked
 * @param[in]      size     Size of the region
 *
 * @return         void
 */
void pcie_res_tz_lock_regions(uint32 offset, uint32 size);

/* ============================================================================
**  Function : pcie_res_tz_unlock_regions
** ============================================================================
*/
/**
 * System call to TZ to unlock previously locked regions
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_tz_unlock_regions(void);
#endif /* __PCIE_CLK_H__ */
