/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/ddr_target.c#4 $
$DateTime: 2019/02/10 21:18:21 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/09/19   ds      Removed shared function table and added BIMC and PHY API's
01/18/16   sk      Updated CDC delay for Turbo in static array
10/01/15   aus     Updated CDC delay for 307MHz to 400pS
07/29/15   aus     Initial version.
================================================================================
                      Copyright 2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_log.h"
#include "ddr_hwio.h"
#include "ddr_target.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "mpm.h"
#include "railway.h"
#include "busywait.h"

/*==============================================================================
                                  DATA
==============================================================================*/
/* DDR status */
extern ddr_interface_state ddr_status;

/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

/* DDR PHY dynamic settings per vddmx voltage level */
static uint32 ddr_phy_config_vddmx[RAILWAY_CORNERS_COUNT] =
{
  0x341,  /* RAILWAY_NO_REQUEST */
  0x341,  /* RAILWAY_RETENTION */
  0x341,  /* RAILWAY_SVS_KRAIT */
  0x341,  /* RAILWAY_SVS_SOC */
  0x341,  /* RAILWAY_SVS_HIGH */
  0x341,  /* RAILWAY_NOMINAL */
  0x341,  /* RAILWAY_NOMINAL_HIGH */
  0x190,  /* RAILWAY_TURBO */
  0x190,  /* RAILWAY_SUPER_TURBO */
  0x190,  /* RAILWAY_SUPER_TURBO_NO_CPR */
};
/* Variable for saving SHKE_CONFIG register */
static uint32 shke_config;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

uint32 ddr_get_device_type(void)
{
	 struct ddr_device_params_common *ddr_params; 
	 
	 /* Get DDR device parameters */
     ddr_params = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
	 
	 return ddr_params->device_type;
}

/* =============================================================================
**  Function : ddr_target_init
** =============================================================================
*/
/**
*   @brief
*   Function called at the end of ddr_init() to initialize target specific
*   configurations.
*
*   @param  None
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
void ddr_target_init()
{
  
  /* Railway voter variables */
  const int cx_rail_id = rail_id("vddcx");
  const int mx_rail_id = rail_id("vddmx");
   
  ddr_status.vddmx_voltage = VDDMX_VOLTAGE_INIT_IN_UV;

  /* Register ddr driver for railway callback */
  railway_set_callback(cx_rail_id,RAILWAY_PRECHANGE_CB, ddr_pre_vddcx_switch,NULL);
  railway_set_callback(cx_rail_id,RAILWAY_POSTCHANGE_CB, ddr_post_vddcx_switch,NULL);
  railway_set_callback(mx_rail_id,RAILWAY_PRECHANGE_CB, ddr_pre_vddmx_switch,NULL);
  railway_set_callback(mx_rail_id,RAILWAY_POSTCHANGE_CB, ddr_post_vddmx_switch,NULL);
  
  
  /* Eng change to test out function table and shared library */
//  HAL_SDRAM_Enter_Self_Refresh(DDR_BASE, SDRAM_INTERFACE_0, SDRAM_CS0, HAL_SDRAM_DDR_Wait);
//  HAL_SDRAM_Exit_Self_Refresh(DDR_BASE, SDRAM_INTERFACE_0, SDRAM_CS0, HAL_SDRAM_DDR_Wait);
} /* ddr_target_init */

/* =============================================================================
**  Function : ddr_post_init
** =============================================================================
*/
/**
*   @brief
*   Function called after DDR is initialized for DDR training if required.
*
*   @param  None
*
*   @retval  TURE   DDR training required
*   @retval  FALSE  DDR training not required
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
boolean ddr_post_init()
{
  return FALSE;

} /* ddr_post_init */

/* =============================================================================
**  Function : ddr_pre_clock_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right before DDR clock switch.
*
*   @param[in]  curr_clk        Current clock speed (in KHz)
*   @param[in]  new_clk         New clock speed (in KHz)
*   @param[in]  interface_name  Interface to switch clock for
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
void ddr_pre_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{
    if (interface_name == SDRAM_INTERFACE_0)
    {
      /* Configure BIMC clock period */
      HAL_SDRAM_BIMC_Update_Clock_Period(DDR_BASE, new_clk, ddr_divide_func);

      /* Configure DPE before clock switch */
      HAL_SDRAM_DPE_Pre_Clock_Switch(DDR_BASE, interface_name, curr_clk, new_clk);
    }

} /* ddr_pre_clock_switch */

/* =============================================================================
**  Function : ddr_post_clock_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right after DDR clock switch.
*
*   @param[in]  curr_clk        Current clock speed (in KHz)
*   @param[in]  new_clk         New clock speed (in KHz)
*   @param[in]  interface_name  Interface to switch clock for
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
void ddr_post_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{
  if (interface_name == SDRAM_INTERFACE_0)
  {
    /* Configure DPE before clock switch */
    HAL_SDRAM_DPE_Post_Clock_Switch(DDR_BASE, interface_name, curr_clk, new_clk);
  }

} /* ddr_post_clock_switch */

/* =============================================================================
**  Function : ddr_pre_vddmx_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right before vddmx switch.
*
*   @param[in]  voltage  New vddmx voltage (in uV)
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
void ddr_pre_vddmx_switch(const railway_settings *proposal, void * callback_cookie)
{
  if (ddr_status.sdram0_cs0 == DDR_ACTIVE ||
      ddr_status.sdram0_cs1 == DDR_ACTIVE)
  {
    if (proposal->microvolts < ddr_status.vddmx_voltage)
    {
      /* Update CDC configuration according to voltage */
      HAL_SDRAM_PHY_Update_CDC_Config(DDR_BASE, SDRAM_INTERFACE_0, ddr_phy_config_vddmx[proposal->mode]);

      /* Issue manual CDCCAL */
      HAL_SDRAM_PHY_Manual_CDC_Cal(DDR_BASE, SDRAM_INTERFACE_0);
    }
    /* Enable rapid CDCCAL */
    HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal(DDR_BASE, SDRAM_INTERFACE_0);
  }

} /* ddr_pre_vddmx_switch */

/* =============================================================================
**  Function : ddr_post_vddmx_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right after vddmx switch.
*
*   @param[in]  voltage  New vddmx voltage (in uV)
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
void ddr_post_vddmx_switch(const railway_settings *proposal, void * callback_cookie)
{
  if (ddr_status.sdram0_cs0 == DDR_ACTIVE ||
      ddr_status.sdram0_cs1 == DDR_ACTIVE)
  {
    /* Disable rapid CDCCAL */
    HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal(DDR_BASE, SDRAM_INTERFACE_0);

    if (proposal->microvolts > ddr_status.vddmx_voltage)
    {
      /* Update CDC configuration according to voltage */
      HAL_SDRAM_PHY_Update_CDC_Config(DDR_BASE, SDRAM_INTERFACE_0, ddr_phy_config_vddmx[proposal->mode]);

      /* Issue manual CDCCAL */
      HAL_SDRAM_PHY_Manual_CDC_Cal(DDR_BASE, SDRAM_INTERFACE_0);
    }
  }

  /* Update VDDMX voltage */
  ddr_status.vddmx_voltage = proposal->microvolts;

} /* ddr_post_vddmx_switch */

/* =============================================================================
**  Function : ddr_pre_xo_shutdown
** =============================================================================
*/
/**
*   @brief
*   Function called right before DDR XO shutdown.
*
*   @param[in]  clk_speed  Clock speed (in KHz) before XO shutdown
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
void ddr_pre_xo_shutdown(uint32 clk_speed)
{
  ddr_enter_self_refresh_all(clk_speed);

} /* ddr_pre_xo_shutdown */

/* =============================================================================
**  Function : ddr_post_xo_shutdown
** =============================================================================
*/
/**
*   @brief
*   Function called right after DDR XO shutdown.
*
*   @param[in]  clk_speed  Clock speed (in KHz) after XO shutdown
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
void ddr_post_xo_shutdown(uint32 clk_speed)
{
  ddr_exit_self_refresh_all(clk_speed);

} /* ddr_post_xo_shutdown */

/* =============================================================================
**  Function : ddr_enter_power_collapse
** =============================================================================
*/
/**
*   @brief
*   Function called before DDR controller goes into power collapse.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_enter_power_collapse(uint32 clk_speed)
{
  /* Put DDR into self refresh */
  ddr_enter_self_refresh_all(clk_speed);

  /* Enable EBI1 clamps via freeze_io */
  mpm_config_ebi1_freeze_io(TRUE);

  /* Save SHKE_CONFIG registers that are not backed up by DEHR */
  shke_config = BOOT_HWIO_IN(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET);

} /* ddr_enter_power_collapse */

/* =============================================================================
**  Function : ddr_exit_power_collapse
** =============================================================================
*/
/**
*   @brief
*   Function called after DDR controller comes out of power collapse.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_exit_power_collapse(uint32 clk_speed)
{
  uint32 rank_init_fmsk;
  SDRAM_CHIPSELECT chip_select;

  /* Field masks for SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
  rank_init_fmsk = HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE) |
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE);

  chip_select = SDRAM_CS_NONE;
  if (ddr_status.sdram0_cs0 == DDR_SELF_REFRESH)
  {
    chip_select |= SDRAM_CS0;
  }
  if (ddr_status.sdram0_cs1 == DDR_SELF_REFRESH)
  {
    chip_select |= SDRAM_CS1;
  }

  if (chip_select != SDRAM_CS_NONE)
  {
    /* Compute timing parameters in cycles */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), SDRAM_0_OFFSET,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS));
    /* Wait for computing done */
    while ( BOOT_HWIO_INM(DDR_ADDR(DPE_MEMC_STATUS_1), SDRAM_0_OFFSET,
                          HWIO_FMSK(DDR_ADDR(DPE_MEMC_STATUS_1), CYC_CALC_VALID)) )
    {}

    /* Load timing registers */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), SDRAM_0_OFFSET,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));

    /* Restore SHKE_CONFIG expect SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
    BOOT_HWIO_OUT(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                  shke_config & ~rank_init_fmsk);

    /* Toggle SHKE_CONFIG[RANK0/1_INITCOMPLETE] as workaround for HW issue that
       TREFI is not loaded correctly */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                   rank_init_fmsk, 0x0);

    /* Wait for 1us (at least 3 XO cycles) */
    HAL_SDRAM_DDR_Wait(1, 0);

    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                   rank_init_fmsk, shke_config);

    /* Put DDR into self refresh with EBI1 clamped to restore controller state */
    HAL_SDRAM_Enter_Self_Refresh(DDR_BASE, SDRAM_INTERFACE_0, chip_select, HAL_SDRAM_DDR_Wait);
  }

  /* Disable EBI1 clamps via freeze_io */
  mpm_config_ebi1_freeze_io(FALSE);

  /* Take DDR out of self refresh */
  ddr_exit_self_refresh_all(clk_speed);

} /* ddr_exit_power_collapse */

void ddr_pre_vddcx_switch(const railway_settings *proposal, void * callback_cookie) {}
void ddr_post_vddcx_switch(const railway_settings *proposal, void * callback_cookie) {}


