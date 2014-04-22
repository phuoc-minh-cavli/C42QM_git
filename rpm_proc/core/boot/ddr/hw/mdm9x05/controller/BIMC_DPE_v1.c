/*
 ===========================================================================

 FILE:         BIMC_DPE_v1.c

 DESCRIPTION:
 This is the platform hardware abstraction layer implementation for
 BIMC DPE.

 Copyright 2012-2013,2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
 ===========================================================================

 Edit History

 $Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/controller/BIMC_DPE_v1.c#1 $

 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 03/01/16   sk      CA Training changes
 02/09/14   tw      added support for common ddr library
 08/12/13   sl      Different interface widths for different ranks.
 11/15/12   tw      Restore 800mhz, enable clock stop power down
 11/02/12   sy      Back out 800Mhz
 10/31/12   sl      Changed pad power mode switch logic.
 09/21/12   sl      Combined LPDDR device parameters.
 09/17/12   sl      Updated DPE clock switch functions.
 09/12/12   tw      Disable HW based self refresh.
 09/06/12   sl      Updated HAL_SDRAM_DPE_Update_AC_Parameters().
 08/30/12   sl      Load DPE configuration at the end of DPE initialization.
 08/24/12   sl      Updated DPE functions.
 08/07/12   tw      Modified dpe configuration to support one time settings
 08/06/12   tw      reimplemented time based units, removed hard coded config
                    values, use one time settings instead.
 07/12/12   tw      Revert back to legacy cycle based timing
 02/02/12   tw      Initial revision
 ===========================================================================
 */


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_target.h"
#include "ddr_log.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* ============================================================================
 **  Function : HAL_SDRAM_DPE_Disable_Power_Down
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will Disable Power down and clock stop power down
 *
 *   @details
 *   This function will Disable Power down and clock stop power down
 *
 *   @param interface   -  [IN] Param_description_goes_here
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Disable_Power_Down(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 offset;
  /* Configure for correct offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  
  /* Disable powerdown/clockstop-powerdown */
  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_PWR_CTRL_0),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), PWR_DN_EN) | HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), CLK_STOP_PWR_DN_EN),
      0x0);
} /* HAL_SDRAM_DPE_Disable_Power_Down */

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Update_Latency
** ============================================================================
*/
/*!
 *   @brief
 *   This function will update the read write latency based on the given
 *   clock speed
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param clk_speed  -  [IN] clock speed in khz
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Update_Latency(uint32 base, SDRAM_INTERFACE interface, uint32 clk_speed)
{
  uint32 offset;
  //struct ddr_device_params_common *ddr_params;
  uint32 rl, wl, data;
  DDR_TYPE ddr_type;
  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* get ddr device type */
  if(BOOT_HWIO_INM(DDR_ADDR(DPE_CONFIG_0), offset, 
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_0),DEVICE_TYPE)) == 0)
  {
    /* LPDDR2 detected */
    ddr_type = DDR_TYPE_LPDDR2;
  }
  else
  {
    /* LPDDR3 detected */
    ddr_type = DDR_TYPE_LPDDR3; 
  }

  rl = HAL_SDRAM_RL(ddr_type, clk_speed);
  wl = HAL_SDRAM_WL(ddr_type, clk_speed); 
     
  if (rl == 3 && wl == 1) { data = 0x1; }
  else if (rl == 4 && wl == 2) { data = 0x2; }
  else if (rl == 5 && wl == 2) { data = 0x3; }
  else if (rl == 6 && wl == 3) { data = 0x4; }
  else if (rl == 7 && wl == 4) { data = 0x5; }
  else if (rl == 8 && wl == 4) { data = 0x6; }
  else if (rl == 9 && wl == 5) { data = 0x7; }
  else if (rl == 10 && wl == 6) { data = 0x8; }
  else if (rl == 11 && wl == 6) { data = 0x9; }
  else { data = 0xA; }  /* Use RL=12/WL=6 in case all else fails */

  /* Write data to MRW_AFTER_FREQ_SWITCH to load latency after clock switch */
  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_MRW_AFTER_FREQ_SWITCH_0),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_MRW_AFTER_FREQ_SWITCH_0), W_DATA) |
      HWIO_FMSK(DDR_ADDR(DPE_MRW_AFTER_FREQ_SWITCH_0), W_ADDR) ,
      0x2 << HWIO_SHFT(DDR_ADDR(DPE_MRW_AFTER_FREQ_SWITCH_0), W_ADDR) |
      data << HWIO_SHFT(DDR_ADDR(DPE_MRW_AFTER_FREQ_SWITCH_0), W_DATA));

  /* Enable DPE_MRW_FREQ_SWITCH[AFTER_0].*/
  BOOT_HWIO_OUTM( DDR_ADDR(DPE_MRW_FREQ_SWITCH), offset,
      HWIO_FMSK(DDR_ADDR(DPE_MRW_FREQ_SWITCH), AFTER_0),
      0x1 << HWIO_SHFT(DDR_ADDR(DPE_MRW_FREQ_SWITCH), AFTER_0));

  /* Update Timing 16 for same READ and WRITE latency */
  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_DRAM_TIMING_16),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_DRAM_TIMING_16), RD_LATENCY) |
      HWIO_FMSK(DDR_ADDR(DPE_DRAM_TIMING_16), WR_LATENCY),
      rl << HWIO_SHFT(DDR_ADDR(DPE_DRAM_TIMING_16), RD_LATENCY) |
      wl << HWIO_SHFT(DDR_ADDR(DPE_DRAM_TIMING_16), WR_LATENCY));
}

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Force_Update_Latency
** ============================================================================
*/
/*!
 *   @brief
 *   This function will force update the read write latency based on the given
 *   clock speed
 *
 *   @details
 *   This function will force update the read write latency based on the given
 *   clock speed, unlike HAL_SDRAM_DPE_Update_Latency, this function will apply
 *   the new latency immediately
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param clk_speed  -  [IN] clock speed in khz
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Force_Update_Latency(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  uint32 offset;
  //struct ddr_device_params_common *ddr_params;
  uint32 rl, wl, data;
  DDR_TYPE ddr_type;
  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* get ddr device type */
  if(BOOT_HWIO_INM(DDR_ADDR(DPE_CONFIG_0), offset, 
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_0),DEVICE_TYPE)) == 0)
  {
    /* LPDDR2 detected */
    ddr_type = DDR_TYPE_LPDDR2;
  }
  else
  {
    /* LPDDR3 detected */
    ddr_type = DDR_TYPE_LPDDR3; 
  }

  rl = HAL_SDRAM_RL(ddr_type, clk_speed);
  wl = HAL_SDRAM_WL(ddr_type, clk_speed); 
     
  if (rl == 3 && wl == 1) { data = 0x1; }
  else if (rl == 4 && wl == 2) { data = 0x2; }
  else if (rl == 5 && wl == 2) { data = 0x3; }
  else if (rl == 6 && wl == 3) { data = 0x4; }
  else if (rl == 7 && wl == 4) { data = 0x5; }
  else if (rl == 8 && wl == 4) { data = 0x6; }
  else if (rl == 9 && wl == 5) { data = 0x7; }
  else if (rl == 10 && wl == 6) { data = 0x8; }
  else if (rl == 11 && wl == 6) { data = 0x9; }
  else { data = 0xA; }  /* Use RL=12/WL=6 in case all else fails */


  HAL_SDRAM_Write_MR(base, interface, chip_select, 0x2, data);
}
/* ============================================================================
**  Function : HAL_SDRAM_DPE_Pre_Clock_Switch
** ============================================================================
*/
/*!
 *   @brief
 *   This function update DPE configuration for preparing to clock switch
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed and update DPE configuration for clock switch
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param old_clk    -  [IN] Old clock speed (in KHz)
 *   @param new_clk    -  [IN] New clock speed (in KHz)
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Pre_Clock_Switch(uint32 base, SDRAM_INTERFACE interface, uint32 old_clk, uint32 new_clk)
{
  uint32 offset;
  uint32 threshold;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* TODO: This is for legacy mode only */
  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_6), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_WR_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_CA_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), CDC_WR_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), CDC_CA_DEBUG_MODE),
                 1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_WR_DEBUG_MODE) |
                 1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_CA_DEBUG_MODE) |
                 1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), CDC_WR_DEBUG_MODE) |
                 1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), CDC_CA_DEBUG_MODE));

  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                 0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));

  /* Pad power mode switch (for Gen1 DDR PHY only): force to use self refresh
     mode for frequency switch if pad power mode is switched from low to high */
  if ( BOOT_HWIO_INM(DDR_ADDR(DPE_CONFIG_1), offset,
                     HWIO_FMSK(DDR_ADDR(DPE_CONFIG_1), PAD_POWER_MODE)) )
  {
    threshold = BOOT_HWIO_INM(DDR_ADDR(DPE_CONFIG_1), offset,
                              HWIO_FMSK(DDR_ADDR(DPE_CONFIG_1), PERIOD_BUS_THRESHOLD_POWER_MODE))
                >> HWIO_SHFT(DDR_ADDR(DPE_CONFIG_1), PERIOD_BUS_THRESHOLD_POWER_MODE);

    /*  
      move operations around to avoid using divider, original code is 
      if ( (10000000/old_clk > threshold) && (10000000/new_clk <= threshold) )
    */
    if ( (10000000 > threshold * old_clk) && (10000000 <= threshold * new_clk) )
    {
      BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_3), offset,
                     HWIO_FMSK(DDR_ADDR(DPE_CONFIG_3), HW_FREQ_SW_SR_ONLY) ,
                     0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_3), HW_FREQ_SW_SR_ONLY));

      BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                     HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                     0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));
    }
  }

  /* Update RL/WL */
  HAL_SDRAM_DPE_Update_Latency(base, interface, new_clk);

} /* HAL_SDRAM_DPE_Pre_Clock_Switch */

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Post_Clock_Switch
** ============================================================================
*/
/*!
 *   @brief
 *   This function update DPE configuration cleaning up after clock switch
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed, and also prepare the DPE for clock switch operation
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param old_clk    -  [IN] Old clock speed (in KHz)
 *   @param new_clk    -  [IN] New clock speed (in KHz)
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Post_Clock_Switch(uint32 base, SDRAM_INTERFACE interface, uint32 old_clk, uint32 new_clk)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* TODO: This is for legacy mode only */
  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_6), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_WR_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_CA_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), CDC_WR_DEBUG_MODE) |
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_6), CDC_CA_DEBUG_MODE),
                 0U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_WR_DEBUG_MODE) |
                 0U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), IOSTAGE_CA_DEBUG_MODE) |
                 0U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), CDC_WR_DEBUG_MODE) |
                 0U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_6), CDC_CA_DEBUG_MODE));

  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                 0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));

  if ( BOOT_HWIO_INM(DDR_ADDR(DPE_CONFIG_1), offset,
                     HWIO_FMSK(DDR_ADDR(DPE_CONFIG_1), PAD_POWER_MODE)) )
  {
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_3), offset,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_3), HW_FREQ_SW_SR_ONLY) ,
                   0x0U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_3), HW_FREQ_SW_SR_ONLY));

    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));
  }

} /* HAL_SDRAM_DPE_Post_Clock_Switch */
