/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/ddr_rpm_target.c#4 $
$DateTime: 2019/02/08 18:06:59 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/31/16   sk      Support for 2+2 GB DDR 
03/29/16   sk      Added support for 3GB DDR
01/25/16   sk      DDR Training changes
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
#include "LoaderUtils.h"
#include "ClockBoot.h"
#include "ddr_phy_training.h"
#include "ddr_hwio.h"
#include "boot_error_if.h"

/*==============================================================================
                                  MACROS
==============================================================================*/

/*==============================================================================
                                  DATA
==============================================================================*/
/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

/* Size of DDR device table entry, updated by ddr_set_params() */
uint32 ddr_device_table_entry_size = 0;

extern union ddr_device_params *ddr_device_table;

boolean wdog_reset = FALSE;

uint32 ddr_params_set_enhance_whitelist[][2] =
{
  {HWIO_ADDR(PHY_CA_ADDR(PAD_CFG0)), 0x00330070},
  {HWIO_ADDR(PHY_DQ_ADDR(PAD_CFG0)), 0x00330070},
  {HWIO_ADDR(PHY_DQ_ADDR(PAD_CFG1)), 0x00330070},
  {HWIO_ADDR(PHY_CA_ADDR(CDC_DELAY_CFG)), 0x00000FFF},
  {HWIO_ADDR(PHY_DQ_ADDR(RD_CDC_DELAY_CFG)), 0x00000FFF},
  {HWIO_ADDR(PHY_DQ_ADDR(WR_CDC_DELAY_CFG)), 0x00000FFF},
  {0x0, 0x0}
};

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 20

/* ddr driver implementation for WDOG reset case */
boolean ddr_busywait_init(void)
{
  HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, EN, 0x1); 
  HWIO_OUTI(MPM2_QTMR_AC_CNTACR_n,0,0x3F);
  
  return TRUE;
}

void ddr_busywait (uint32 pause_time_us)
{
  uint32 start_count = 0;
  uint64 delay_count = 0;

  start_count = HWIO_IN(MPM2_QTMR_V1_CNTPCT_LO);
  /*
   * Perform the delay and handle potential overflows of the timer.
   */

  delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ);
  while ( (HWIO_IN(MPM2_QTMR_V1_CNTPCT_LO) - start_count) < delay_count);
  
}

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Update_CDC_Delay_Target
** =============================================================================
*/
/**
*   @brief
*   Update CDC delay.
*
*   @param[in]  interface  Interface to update CDC delay for
*   @param[in]  read       TRUE for read CDC and FALSE for write CDC
*   @param[in]  cdc_delay  CDC delay value
*   @Param[in]  voltage    voltage level
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
void HAL_SDRAM_PHY_Update_CDC_Delay_Target(uint32 base, SDRAM_INTERFACE interface, boolean read, uint32 cdc_delay, uint32 voltage, uint32 ddr_type)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  if (read)
  {
    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + PHY_DQ1_OFFSET * dq;

      /* Update DQ read CDC delay */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                     cdc_delay << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
    }
  }
  else
  {
    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
       /* Offset for each DQ PHY */
       offset = phy_offset + PHY_DQ1_OFFSET * dq;
       /* WR CDC delay for DQ1 and DQ2 to 300ps @ 533MHz */
       if ((dq==1 || dq==2) && (voltage > 1225000)) {
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        0x12C << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));

       }
       /* WR CDC delay for all DQs to 400ps @ 400MHz */
       else if (voltage > 1050000 && voltage <= 1225000) {
         /* Update DQ write CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        0x190 << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
         /* Update DQ read CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                        0x271 << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
       }
       else {
         /* Update DQ write CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        cdc_delay << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
       }
     }
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;
  /* CA CDC delay to 350ps @ 533MHz */
  if (voltage > 1225000) {
    /* Update CA CDC delay */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                0x15E << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));
  }
  /* CA CDC delay to 400ps @ 400MHz */
  else if (voltage > 1050000 && voltage <= 1225000) {
    /* Update CA CDC delay */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                0x190 << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));
  }
  
} /* HAL_SDRAM_PHY_Update_CDC_Delay_Target */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Update_CDC_Config_Training
** =============================================================================
*/
/**
*   @brief
*   Update CDC delay.
*
*   @param[in]  interface  Interface to update CDC delay for
*   @param[in]  read       TRUE for read CDC and FALSE for write CDC
*   @param[in]  cdc_delay  CDC delay value
*   @Param[in]  voltage    voltage level
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
void HAL_SDRAM_PHY_Update_CDC_Config_Training(uint32 base, SDRAM_INTERFACE interface, uint32 ca_dq, uint32 num, uint32 config, uint32 ddr_type)
{
  uint32 phy_offset;
  //uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  if (ca_dq==2)
  {    
      /* Offset for each DQ PHY */
      offset = phy_offset + PHY_DQ1_OFFSET * num;

	  /* Update DQ Write CDC delay */
	  BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        config << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
  }
  else if(ca_dq==1)
  {    
       /* Offset for each DQ PHY */
       offset = phy_offset + PHY_DQ1_OFFSET * num;
	   
       /* Update DQ read CDC delay */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                     config << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
  }
  else if(ca_dq==0)
  {
  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;
  
  /* Update CA CDC delay*/  
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                config << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));
  }  
} /* HAL_SDRAM_PHY_Update_CDC_Delay_Target */

/* =============================================================================
**  Function : sys_debug_bric_settings
** =============================================================================
*/
/**
*   @brief
*   function called to update the BRIC register settings.
*
*   @param [in] None
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
void sys_debug_bric_settings(void)
{
  uint32 mr8, device_density, device_io_width;
  
  mr8 = (uint32)HAL_SDRAM_Read_MR(0, SDRAM_INTERFACE_0, SDRAM_CS0, 0x8);
  device_density = (mr8 & 0x3C) >> 2;
  device_io_width = (mr8 & 0xC0) >> 6;
  
 //SYSNOC slave address range configuration to 512MB 
 HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,1,0xE0000000);
 
  // 1.5GB + 1.5GB configuration
  if ((HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS0) == 0x0) && (device_density == 14) && (device_io_width == 1)) {
    //Disable segment address registers
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER,0,0x00000000);

    //DDR memory configuration: (3GB = 4GB -512MB -512MB) 4GB(0x0000_0000 – 0xFFFF_FFFF)
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,0,0x00000000);
    
    //Remove first 512MB(0x0000_0000 – 0x1FFF_FFFF)
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER,0,0xE0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000002);

    //Remove last 512MB(0xE000_0000 – 0xFFFF_FFFF)
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER,0,0xE0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER,0,0xE0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER,0,0xE0000002);

    //Remap 0x6000_0000 to 0x0000_00000 (1.5GB shift)
    HWIO_OUTI(BIMC_BRIC_REMAPn_ADDR_BASE_LOWER,0,0x60000000);
    HWIO_OUTI(BIMC_BRIC_REMAPn_ADDR_MASK_LOWER,0,0xE0000000);
    HWIO_OUTI(BIMC_BRIC_REMAPn_ADDR_BASE_LOWER,0,0x60000001);
    HWIO_OUTI(BIMC_BRIC_REMAPn_OP1,0,0x00FA0000);
    HWIO_OUTI(BIMC_BRIC_REMAPn_OP1,0,0x00FA0001);

    //Enable segment address registers
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER,0,0xE0000003);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000003);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000001);    
  }
   // 2GB + 2GB configuration
  else if (HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS0) == 0x0) {
    //Disable segment address registers
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000000);
  
    //DDR memory configuration: 4GB(0x0000_0000 – 0xFFFF_FFFF)
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,0,0x00000000);

    //Remove first 256MB(0x0000_0000 – 0x0FFF_FFFF)
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER,0,0xF0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000002);

    //Enable segment address registers
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x00000003);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x00000001);	 
  
    //SYSNOC slave address range configuration to 256MB 
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,1,0xF0000000);
	return;
  } 
  // 1GB + 2GB configuration
  else {
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x80000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,0,0x80000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER,3,0xC0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER,0,0x80000001);

    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x40000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER,0,0xC0000000);
    HWIO_OUTI(BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER,0,0x40000001);
  }  
}

/* =============================================================================
**  Function : ddr_restore_from_wdog_reset
** =============================================================================
*/
/**
*   @brief
*   function called to take ddr out of self refresh following a wdog reset
*
*   @param [in] clk_speed_khz  clock speed of ddr in khz
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
void ddr_restore_from_wdog_reset( uint32 clk_speed_khz )
{
  uint32 ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
  struct ddr_device_params_common *ddr_params_ch0, *ddr_params_ch1;
  SDRAM_CHIPSELECT ch0_chip_sel = SDRAM_CS_NONE, ch1_chip_sel = SDRAM_CS_NONE;
  /* Get DDR device parameters */
  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);  
  ddr_params_ch1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);
  
  if(ddr_params_ch0->num_banks_cs0 != 0)
  {
    ch0_chip_sel |= SDRAM_CS0;
  }
  if(ddr_params_ch0->num_banks_cs1 != 0)
  {
    ch0_chip_sel |= SDRAM_CS1;
  }
  if(ddr_params_ch1->num_banks_cs0 != 0)
  {
    ch1_chip_sel |= SDRAM_CS0;
  }
  if(ddr_params_ch1->num_banks_cs1 != 0)
  {
    ch1_chip_sel |= SDRAM_CS1;
  }
  
  /* set status variable to indicate wdog reset */
  wdog_reset = TRUE;
  
  /* initialize ddr version of busywait */
  ddr_busywait_init();
  
  /* hack mpm to unfreeze io */
  HWIO_OUT(MPM2_MPM_DDR_PHY_FREEZEIO_EBI1, 0x0);
  
  /* restore cdt parameters */
  ddr_restore_param_wdog_reset();
  
  /* restore ddr from self refresh */
  if(ch0_chip_sel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Init(SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_0, ch0_chip_sel, HAL_SDRAM_DDR_Wait, ddr_type);
    
    /* update rl\wl latency to match configuration */
    HAL_SDRAM_DPE_Force_Update_Latency(0, SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
  }

  if(ch1_chip_sel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Init(SDRAM_INTERFACE_0, ch1_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_0, ch1_chip_sel, HAL_SDRAM_DDR_Wait, ddr_type);
  }  
}

/* ============================================================================
**  Function : ddr_set_params_enhance,enhance set
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR DC timing parameters stored in CDB2 and CDB3.enhance set.
*   For CDB2,this function will directly set the registers of DDR driver .
*   For CDB3,this function will put the data in IMEM for RPM setting.
*   @param[in]  ddr_params_ptr   pointer to the base address of CDB2 or CDB3 in memory
*                
*
*   @return
*   TRUE if the parameters was successfully set.
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
boolean ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size)
{
	boolean status = FALSE;
	struct ddr_boot_setting_params *ddr_boot_setting_ptr,*ddr_setting_current_ptr;
	uint32 counter = 0, ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
	uint32 device_params_size = 0;

	void *ddr_imem_top_addr;
	
		
	do
	{
		if(((struct ddr_boot_set_header_v1*)ddr_params_ptr)->magic_number==DDR_BOOT_SET_MAGIC_NUM)
		{     

		    if((ddr_params_size==sizeof(struct ddr_boot_set_header_v1))||( ((struct ddr_boot_set_header_v1*)ddr_params_ptr)->num_of_groups==0))
		    {
                 break;
			}
			if(((((struct ddr_boot_set_header_v1*)ddr_params_ptr)->num_of_groups)*sizeof(struct ddr_boot_setting_params)+sizeof(struct ddr_boot_set_header_v1))!=ddr_params_size)
			{
				break;
			}
			ddr_boot_setting_ptr=(struct ddr_boot_setting_params*)((uint32)ddr_params_ptr+sizeof(struct ddr_boot_set_header_v1));
			
			for(ddr_setting_current_ptr=ddr_boot_setting_ptr;((uint32)ddr_setting_current_ptr)<(uint32)ddr_boot_setting_ptr+ddr_params_size;ddr_setting_current_ptr++)
			{ 
				   for(counter=0; ddr_params_set_enhance_whitelist[counter][0]!=0;counter++)
				   {
					if(ddr_setting_current_ptr->address==ddr_params_set_enhance_whitelist[counter][0])
						out_dword_masked_ns(ddr_setting_current_ptr->address,(ddr_setting_current_ptr->mask)&ddr_params_set_enhance_whitelist[counter][1],ddr_setting_current_ptr->value,in_dword(ddr_setting_current_ptr->address) );
				   }
		  
			}
			/* Issue manual IO CAL */
			HAL_SDRAM_PHY_Manual_IO_Cal(0, SDRAM_INTERFACE_0, HAL_SDRAM_DDR_Wait, ddr_type);			
         		/* Issue manual CDC CAL */
			HAL_SDRAM_PHY_Manual_CDC_Cal(0, SDRAM_INTERFACE_0, ddr_type);
			status = TRUE;
			break;
		}
		else if(((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->magic_number==DDR_POST_BOOT_SET_MAGIC_NUM)
		{

            if((sizeof(struct ddr_post_boot_set_header_v1*)==ddr_params_size) || (((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->size_of_param==0))
            {
				break;
			}
			if((((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->size_of_param)!=sizeof(struct ddr_post_boot_setting_params))
			{
				break;
			}			
			
			/* Get size of device table entry */
			switch (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type)
			{
			     case DDR_TYPE_LPDDR2:
			     case DDR_TYPE_LPDDR3:
				       device_params_size = sizeof(struct ddr_device_params_lpddr);
				       break;

			     case DDR_TYPE_PCDDR3:
				       device_params_size = sizeof(struct ddr_device_params_pcddr3);
				       break;
			      default:
				       device_params_size = 0;
					break;
			}

			/* DDR parameter partition is after two device table entries */
			ddr_imem_top_addr =(void*)  ((uint32)ddr_get_params(SDRAM_INTERFACE_0) + device_params_size * 2);
			
			if(ddr_params_is_training_required())
			{
             	ddr_imem_top_addr =(void*)((uint32)ddr_imem_top_addr+sizeof(struct ddr_params_partition));				
			}
			memsmove((void*)ddr_imem_top_addr,ddr_params_size,(void*)ddr_params_ptr,ddr_params_size);		
			status = TRUE;
			break;
		}
		else
		{
			break;
		}

	}	
 	while(0);
	return status;
}

/* =============================================================================
**  Function : ddr_restore_from_sdi_exit
** =============================================================================
*/
/**
*   @brief
*   function called to put ddr in self refresh after SDI exit
*
*   @param [in] clk_speed_khz  clock speed of ddr in khz
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
void ddr_restore_from_sdi_exit( uint32 clk_speed_khz )
{
	uint32 ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
 
    HAL_SDRAM_Enter_Self_Refresh(0, SDRAM_INTERFACE_0, SDRAM_CS0, HAL_SDRAM_DDR_Wait, ddr_type);
  
}

