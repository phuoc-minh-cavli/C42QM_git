/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/ddr_target.c#9 $
$DateTime: 2019/03/01 00:02:29 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/01/16   sk      CA Training changes
01/25/16   sk      DDR Training changes
12/30/15   sk      Gen1 PHY DQ read and write training changes
05/27/15   aus     Update WR CDC delay and CA CDC delay for 533 and 400
01/22/15   yps     Use memsmove instead of memmove
12/15/14   xl      Add ddr parameters enhance set
02/24/14   tw      Updated update clock period function pointer
02/09/14   tw      added support for common ddr library
01/24/14   tw      initial port for 8916
                   added support for sbl\rpm shared ddr driver
09/05/13   sr      changes to not support training in boot.
06/20/13   rj      Added support for lpddr2 on GIMLI
06/06/13   rj      Updated 8926 delta settings and added function
                   related to ddr draining required.
05/13/13   sl      Removed ddr_apply_delta_settings().
04/10/13   sl      Use voltage mode instead of voltage number.
04/03/13   sl      Added macros for vddmx voltage levels.
03/01/13   tw      Added stubs for applying delta settings
02/21/13   sl      Initial version.
================================================================================
                      Copyright 2013-2014, 2015-2016 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_params.h"
#include "ddr_internal.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "ddr_log.h"
#include "DDIChipInfo.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "DDIChipInfo.h"
#include "mpm_hwio.h"
#include <stringl/stringl.h>
#include "ClockBoot.h"
#include "ddr_phy_training.h"
#include "ddr_hwio.h"
#include "boot_error_if.h"
#include "HALbootHWIO.h"

/*==============================================================================
                                  DATA
==============================================================================*/

/* Initial DDR PHY CDC delay */
__attribute__((section("ddr_func_ptr"))) uint32 DDR_PHY_INIT_CDC_DELAY;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/


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
  struct ddr_shared_params *header;
  
  extern uint32 ddr_bimc_config_9205_LPDDR3[][2];
  extern uint32 ddr_phy_config_9205_LPDDR3[][2];
  extern uint32 ddr_caphy_config_9205_LPDDR3[][2];
  extern uint32 ddr_dqphy_config_9205_LPDDR3[][2];
  
  //if (DalChipInfo_ChipFamily() == DALCHIPINFO_ID_MSM8916)
  {
    if (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type == DDR_TYPE_LPDDR3)
    {
      ddr_bimc_config_delta = ddr_bimc_config_9205_LPDDR3;
      ddr_phy_config_delta = ddr_phy_config_9205_LPDDR3;
      ddr_caphy_config_delta = ddr_caphy_config_9205_LPDDR3;
      ddr_dqphy_config_delta = ddr_dqphy_config_9205_LPDDR3;
    }
    /* DDR initialized @ 168 Mhz*/
    DDR_PHY_INIT_CDC_DELAY = 0x341;
  }
     
  /* clear out shared header and fill in with updated information */
  header = ddr_get_shared_header();
  memset(header, 0, sizeof(struct ddr_shared_params));
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
	uint32 ddr_type;
	uint32 ddr_speed_khz = 336000;
    
	 /*DDR Frequency Switching to NOM 336 Mhz */
	ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
	HAL_SDRAM_BIMC_Update_Clock_Period(0, ddr_speed_khz, ddr_divide_func,ddr_type);
	HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_speed_khz,ddr_type);     
	Clock_SetBIMCSpeed(ddr_speed_khz);
	HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_speed_khz,ddr_type);
	Clock_SetDDRMinMax(1, 3);
	return FALSE;
} /* ddr_post_init */

/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. LPDDR3 will always require training syncronization
*   to be done between rpm and sbl
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_required( void )
{
  return FALSE;
} /* ddr_params_is_training_required */

/* =============================================================================
**  Function : ddr_params_is_training_on_rpm_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not in RPM. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. 
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_on_rpm_required( void )
{
  return FALSE;
} /* ddr_params_is_training_on_rpm_required */

/* =============================================================================
**  Function : ddr_do_phy_training
** =============================================================================
*/
/**
*   @brief
*   function called to do the DDR PHY training 
*
*   @param  None
*
*   @retval  NONE
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
void ddr_do_phy_training( void )
{  
} /* ddr_do_phy_training */


/* ============================================================================
**  Function : ddr_remapper
** ============================================================================
*/
/**
*   @brief
*   remap DDR base addresses
*                
*
*   @return
*   None
*
*   @dependencies
*  None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_remapper(void)
{
  
} /* ddr_remapper */

/* =============================================================================
**  Function : ddr_preinit_rpm
** =============================================================================
*/
/**
*   @brief
*   function called to initialize DDR wrapper in RPM
*
*   @retval  NONE
*
*   @dependencies
*   Needs to be called only after all the DDR subsystem register read/write
*   sequences are completed in DDR driver in SBL 
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_preinit_rpm( void )
{

} /* ddr_preinit_rpm */
