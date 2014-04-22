/*
===========================================================================

FILE:         BIMC_v1.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for 
  HSDDRX controller.

Copyright 2012-2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/controller/BIMC_v1.c#3 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/19/18   ds      Added support for 256 Mb DDR density.
03/10/16   sk      Added support for 1.5GB and 3GB DDR 
01/05/16   sk      Masking MR5 register value with 0xFF.
11/28/14   aus     Added 6Gb support
05/27/14   sk      Added support for 512MB density based on CDT parameter read
02/24/14   tw      refactored code to avoid using divide when possible
                   use divide function pointer passed in when not possible
02/09/14   tw      added support for common ddr library
01/13/14   tw      Updated to support 1ps resolution for ddr timing
08/12/13   sl      Adjust RL/WL with frequency for LPDDR2.
05/13/13   sl      Updated delta table calls.
04/03/13   sl      Added Read Latency for 266MHz.
03/01/13   tw      Added calling delta table
11/13/12   tw      Re-enabled 800mhz, enable hw self refresh
11/13/12   sl      Corrected HAL_SDRAM_Get_Base_Addr().
11/02/12   sy      Back out 800Mhz
10/31/12   sl      Changed ddr_static_config_init() to ddr_set_config().
10/12/12   sl      Added parameter detection.
10/04/12   tw      SITE initialization JCPLL workaround
09/21/12   sl      Added BOOT_PRE_SILICON flag.
09/19/12   sl      Updated HAL_SDRAM_Enter/Exit_Self_Refresh().
09/17/12   sl      Updated HAL_SDRAM_RL/WL().
09/12/12   sl      Use macro-style DDR logging.
09/07/12   sl      Removed functions for HAL DDR status.
09/06/12   sl      Moved parameter tweaking functions to ddr_target.c.
08/30/12   sl      Reordered DDR initialization sequence
08/24/12   sl      Update for rank detection
08/17/12   sl      Removed function pointers
08/10/12   sl      Added HAL_SDRAM_PHY_Init() call
08/03/12   tw      Added IO Init and IO Calibration to init sequence
08/03/12   tw      Fixed incorrect base address calculation
08/01/12   tw      Added calls to ddr_config for one time settings
07/10/12   tw      Added api calls for training sequences
05/15/12   tw      Updated api name to Power_Down instead of powerdown
                   Added new bimc api for deep power down
01/19/12   tw      Initial revision
===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_internal.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "ddr_log.h"
#include "BIMC.h"
#include "DDR_PHY.h"
//#include "SITE.h"
#include "busywait.h"
#include "HAL_DDR.h"

/*==============================================================================
                                   DATA
==============================================================================*/

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/


uint32 ddr_divide_func(uint32 a, uint32 b)
{
  return a/b;
}

/* =============================================================================
**  Function : HAL_SDRAM_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR controller, as well as DDR device.
*
*   @param[in]  interface    Interface to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_speed    Clock speed (in KHz) to initialize for
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
	
}
/* ============================================================================
**  Function : HAL_SDRAM_Get_Base_Addr
** ============================================================================
*/
/*!
*   @brief
*   This function return the base address of the specific interface and chip select
*   
*   @details
*   This function will return pre determined base address for the particular interface\chipselect requested
*   
*   @param  None
*   
*   @par Dependencies
*   
*   
*   @par Side Effects
*
*   
*   @retval  uint32 - a return value of 0xFFFFFFFF indicates that this is not a valid interface\chipselect to use.
*
*   
*   @sa None
*/

uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
	return 0;
} /* HAL_SDRAM_Get_Base_Addr */
/* ============================================================================
**  Function : HAL_SDRAM_DDR_Wait
** ============================================================================
*/
/*!
*   @brief
*   Waits for the given amount of time in us
*   
*   @details
*   waits the given amount of time in us
*   implemented using pause timer when available
*   
*   @param wait_us     -  [IN] Time to wait in terms of microseconds
*   @param clk_speed   -  [IN] Current clock frequency in KHz
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
void HAL_SDRAM_DDR_Wait(uint32 wait_us, uint32 clk_speed)
{
    busywait(wait_us);
	
} /* HAL_SDRAM_DDR_Wait */

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   Puts the ddr into self refresh mode using idle self refresh
*
*   @details
*   Puts the ddr into self refresh mode using idle self refresh.
*   This means the controller will wait for the device to become idle
*   before putting the ddr into self refresh.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   Once ddr has entered self refresh no further access to it is allowed.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_Enter_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  /* Disable ranks */
  HAL_SDRAM_SCMO_Rank_Disable(base, interface, chip_select);

  /* Disable HW based self refresh */
  HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(base, interface, chip_select);

  /* Disable auto IOCAL */
  HAL_SDRAM_PHY_Disable_IO_Cal(base, interface, wait);

  /* Enter self refresh */
  HAL_SDRAM_SHKE_Enter_Self_Refresh(base, interface, chip_select);

  /* Update DDR log */
  DDR_LOG(DDR_STATUS, DDR_SELF_REFRESH_ENTRY_IDLE, interface, chip_select);

} /* HAL_SDRAM_Enter_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   Takes the ddr out of self refresh mode using idle self refresh
*
*   @details
*   Takes the ddr out of self refresh mode.
*   After the ddr has been taken out of self refresh memory access can resume
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_Exit_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  /* Enable ranks */
  HAL_SDRAM_SCMO_Rank_Enable(base, interface, chip_select);

  /* Issue manual IOCAL */
  HAL_SDRAM_PHY_Manual_IO_Cal(base, interface, wait);

  /* Exit self refresh */
  HAL_SDRAM_SHKE_Exit_Self_Refresh(base, interface, chip_select);

  /* Enable auto IOCAL */
  HAL_SDRAM_PHY_Enable_IO_Cal(base, interface);

  /* Enable HW based self refresh */
  HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(base, interface, chip_select);

  /* Update DDR log */
  DDR_LOG(DDR_STATUS, DDR_SELF_REFRESH_EXIT, interface, chip_select);

} /* HAL_SDRAM_Exit_Self_Refresh */

/* =============================================================================
**  Function : HAL_SDRAM_RL
** =============================================================================
*/
/**
*   @brief
*   Return Read Latency according to DDR type and frequency.
*
*   @param[in]  ddr_type   DDR type
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @return
*   Read Latency.
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 HAL_SDRAM_RL(DDR_TYPE ddr_type, uint32 clk_speed)
{
  uint32 rl = 0;

  if (ddr_type == DDR_TYPE_LPDDR2)
  {
    if (clk_speed <= 200000) { rl = 3; }
	else if (clk_speed <= 267000) { rl = 4; }
    else if (clk_speed <= 334000) { rl = 5; }
    else if (clk_speed <= 400000) { rl = 6; }
    else if (clk_speed <= 466500) { rl = 7; }
    else if (clk_speed <= 534000) { rl = 8; }
  }
  else if (ddr_type == DDR_TYPE_LPDDR3)
  {
    if (clk_speed <= 400000) { rl = 6; }
    else if (clk_speed <= 533000) { rl = 8; }
    else if (clk_speed <= 600000) { rl = 9; }
    else if (clk_speed <= 667000) { rl = 10; }
    else if (clk_speed <= 733000) { rl = 11; }
    else { rl = 12; }
  }

  return rl;

} /* HAL_SDRAM_RL */

/* =============================================================================
**  Function : HAL_SDRAM_WL
** =============================================================================
*/
/**
*   @brief
*   Return Write Latency according to DDR type and frequency.
*
*   @param[in]  ddr_type   DDR type
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @return
*   Write Latency.
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 HAL_SDRAM_WL(DDR_TYPE ddr_type, uint32 clk_speed)
{
  uint32 wl = 0;

  if (ddr_type == DDR_TYPE_LPDDR2)
  {
    if (clk_speed <= 200000) { wl = 1; }
	else if (clk_speed <= 267000) { wl = 2; }
	else if (clk_speed <= 334000) { wl = 2; }
    else if (clk_speed <= 400000) { wl = 3; }
    else if (clk_speed <= 534000) { wl = 4; }
  }
  else if (ddr_type == DDR_TYPE_LPDDR3)
  {
    if (clk_speed <= 400000) { wl = 3; }
    else if (clk_speed <= 533000) {  wl = 4; }
    else if (clk_speed <= 600000) {  wl = 5; }
    else { wl = 6; }
  }

  return wl;

} /* HAL_SDRAM_WL */

/* =============================================================================
**  Function : HAL_SDRAM_BIMC_Update_Clock_Period
** =============================================================================
*/
/**
*   @brief
*   Updates the bimc clock period for clock switching
*
*   @param[in]  interface_name  interface to configure new clock for
*   @param[in]  clk_speed       Clock speed (in KHz)
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void HAL_SDRAM_BIMC_Update_Clock_Period(uint32 base, uint32 clk_speed, ddr_divide divide_func)
{
  uint32 period;
 
  if(BOOT_HWIO_IN(BIMC_CORE_CLK_PERIOD, base) & HWIO_FMSK(BIMC_CORE_CLK_PERIOD, CLK_PERIOD_RESOLUTION))
  {
   /*1ps resolution */
    if(clk_speed == 19200)
    {
      period = 0xCB73;
    }
    else
    {
      period = MIN(divide_func(1000000000,clk_speed), 0x1ffff);
    }
  }
  else
  {
  	/* 100ps resolution */
    if(clk_speed == 19200)
    {
      period = 0x208;
    }   
    else
    {
      period = MIN(divide_func(10000000,clk_speed), 0xFF);
    }
  }
  
 BOOT_HWIO_OUTM(BIMC_CORE_CLK_PERIOD, base, HWIO_FMSK(BIMC_CORE_CLK_PERIOD, PERIOD), period);  
}

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr into DPD mode
*
*   @details
*   This function will trigger the ddr to be put into dee power down mode.
*   In the case where we only have 1 device on the interface (CS0 only), or both
*   chip select are chosen, we will also turn off ddr clock.
*
*   @param interface_name   -  [IN] Interface to configure dpd for
*
*   @par Dependencies
*   The desired CS to be configured for Deep Power Down Must be configured first by
*   calling HAL_SDRAM_Chipselect().
*
*   @par Side Effects
*   After ddr has gone into Deep Power Down, all the data stored will be lost.
*   Also, when returning from deep power down, we must reinitialize the ddr.
*
*   @retval  None
*
*   @sa None
*/

void HAL_SDRAM_Enter_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  /* Disable Rank access */
  HAL_SDRAM_SCMO_Rank_Disable(base, interface_name, chip_select);

  /* Put ddr into deep power down */
  HAL_SDRAM_SHKE_DPD_Enter(base, interface_name, chip_select);

  DDR_LOG(DDR_STATUS, DDR_DPD_ENTRY, interface_name, chip_select);

} /* HAL_SDRAM_Enter_Deep_Power_Down */

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will take ddr out of deep power down
*
*   @details
*   This function will take ddr out of deep power down
*
*   @param clk_speed   -  [IN] Clock speed to reinitialize ddr for
*
*   @par Dependencies
*   Clock must be setup for initialization speed.
*   LPDDR1 - no limitation
*   LPDDR2 - < 50Mhz
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void HAL_SDRAM_Exit_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  /* Enable Rank access */
  HAL_SDRAM_SCMO_Rank_Enable(base, interface_name, chip_select);

  /* Take ddr out of deep power down */
  HAL_SDRAM_SHKE_DPD_Exit(base, interface_name, chip_select, clk_speed);

  DDR_LOG(DDR_STATUS, DDR_DPD_EXIT, interface_name, chip_select);

} /* HAL_SDRAM_Exit_Deep_Power_Down */
