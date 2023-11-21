#ifndef __DDR_TARGET_H__
#define __DDR_TARGET_H__


/**
 * @file ddr_target.h
 * @brief
 * Header file for DDR target-specific info.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/ddr_target.h#4 $
$DateTime: 2019/03/01 00:02:29 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/29/16   sk      Added support for 3GB DDR 
03/01/16   sk      CA Training changes
01/25/16   sk      DDR Training changes
03/04/15   sc      Moved PHY_DQ_MAX_NUM to ddr_target.h from DDR_PHY.c
12/15/14   xl      Add structs,enhance set
04/10/13   sl      Use voltage mode instead of voltage number.
04/03/13   sl      Added macros for vddmx voltage levels.
02/21/13   sl      Initial version.
================================================================================
             Copyright 2013-2014, 2015-2016 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_hwio.h"
#include "ddr_drivers.h"
#include "msmhwiobase.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
#define FEATURE_DDR_PARAM_SAVE_TO_EMMC 1
/* DQ PHY offsets */
#define PHY_DQ_MAX_NUM 2

/* DDR controller register names */
#define DDR_ADDR(ddr_reg)     BIMC_S_DDR0_##ddr_reg
#define PHY_CA_ADDR(ddr_reg)  DIM_C00_DIM_CA_##ddr_reg
#define PHY_DQ_ADDR(ddr_reg)  DIM_D00_DIM_DQ_##ddr_reg

/* DDR controller offsets */
#define SDRAM_0_OFFSET     0x0
#define SDRAM_1_OFFSET     0xFFFFFFFF
#define PHY_0_OFFSET       0x0
#define PHY_1_OFFSET       0xFFFFFFFF
#define PHY_CA_MST_OFFSET  0x0
#define PHY_DQ0_OFFSET     0x0
#define PHY_DQ1_OFFSET     (DIM_D01_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define PHY_DQ2_OFFSET     (DIM_D02_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define PHY_DQ3_OFFSET     (DIM_D03_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define DQ_BCAST_OFFSET    (EBI1_AHB2PHY_BROADCAST_SWMAN_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)

/* DDR base address */
#define DDR_BASE  0x40000000

/* Maximum DDR clock frequency in KHz */
#define DDR_CLK_FREQ_MAX_IN_KHZ  533000

/* XO clock frequency in KHz */
#define XO_SPEED_IN_KHZ  19200

/* Initial DDR PHY CDC delay */
extern uint32 DDR_PHY_INIT_CDC_DELAY;


/*struct for CDB2 pairs,enhance set*/
struct ddr_boot_setting_params
{
     uint32 address;
     uint32 mask;
     uint32 value;
};

/*struct for CDB3,enhance set*/
struct ddr_post_boot_setting_params
{
	uint32 dq_read_cdc_delay;
	uint32 dq_write_cdc_delay;
};


/*Head for CDB2 in CDT,enhance set*/
struct ddr_boot_set_header_v1
{
  /*Version number of the CDT DDR parameters, always the first four byte of this header*/
  uint32 version_number;

  /*Magic number of the DDR block of CDT, must have the value of DDR_PARAMS_MAGIC_NUM*/
  uint32 magic_number;
  
  /*Total size of the ddr parameters for a single interface*/
  uint32 num_of_groups;

  /**More fields can be added here, each field must be multiple of 4 bytes**/
  
};


/*Head for CDB3 in CDT,enhance set*/
struct ddr_post_boot_set_header_v1
{
  /*Version number of the CDT DDR parameters, always the first four byte of this header*/
  uint32 version_number;

  /*Magic number of the DDR block of CDT, must have the value of DDR_PARAMS_MAGIC_NUM*/
  uint32 magic_number;
  
  /*Total size of the ddr parameters for a single interface*/
  uint32 size_of_param;
};



/*Magic number for CDB2 and CDB3 in CDT,enhance set*/
#define DDR_BOOT_SET_MAGIC_NUM 0x06078899
#define DDR_POST_BOOT_SET_MAGIC_NUM 0x04030201

#define DDR_CLOCK_RATIO 1
#define DDR_CLOCK_TEST_FRAME_WORK

/* Define the current ddr training version number */
#define DDR_TRAINING_PARAM_VERSION 0x4


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
void HAL_SDRAM_PHY_Update_CDC_Delay_Target(uint32 base, SDRAM_INTERFACE interface, boolean read, uint32 cdc_delay, uint32 voltage, uint32 ddr_type);

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
void HAL_SDRAM_PHY_Update_CDC_Config_Training(uint32 base, SDRAM_INTERFACE interface, uint32 ca_dq, uint32 num, uint32 config,  uint32 ddr_type);

/* ============================================================================
**  Function : ddr_mem_write_high_speed
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a write at high speed
*   
*   @details
*   This api will make the assumption that ddr is already at high speed and 
*   just does a burst\non-burst write based on input parameter
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param pattern          -  [IN] array of pattern to set
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_mem_write_high_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst);

/* ============================================================================
**  Function : ddr_mem_write_low_speed
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a write at low speed
*   
*   @details
*   This api will call the relevent clock function to scale down the clock frequency
*   so we can have a guaranteed write, then scale back up to the previous speed.
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param pattern          -  [IN] array of pattern to set
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_mem_write_low_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst);

/* ============================================================================
**  Function : ddr_write
** ============================================================================
*/
/*!
*   @brief
*   This function does single transaction writes
*
*   @details
*   This function does single transaction writes
*
*   @param interface_name             -  [IN] interface to set pattern for
*   @param chip_select                -  [IN] chip select to set pattern for
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
void ddr_write(volatile uint32 *address, uint32 * pattern, uint32 size);

/* ============================================================================
**  Function : ddr_mem_read
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a read
*   
*   @details
*   This api will fill the input data array with the size specified
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param data             -  [IN/OUT] array of data to read to
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_mem_read( uint32 chnl, uint32 rank, uint32 * data, uint32 size, uint32 burst);

#endif /* __DDR_TARGET_H__ */
