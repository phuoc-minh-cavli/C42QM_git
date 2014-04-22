#include "ddr_target.h"
#include "ddr_params.h"
#include "busywait.h"
#include "ClockBoot.h"
#include "BIMC.h"

/*==============================================================================
                                  DATA
==============================================================================*/
extern uint32 mc_fmax, ddr_fmax;
extern void __blocksCopy(uint32 *source, volatile uint32 *destination, uint32 num_words);
/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* ============================================================================
**  Function : HAL_SDRAM_Get_Training_Address
** ============================================================================
*/
/*!
*   @brief
*   This helper function finds the appropriate address for ddr to do training on.
*
*   @details
*   This helper function finds the appropriate address for ddr to do training on.
*   Taking into account for interleave status, interface, and chip select
*
*   @param interface_name              -  [IN] Interface to train on
*   @param chip_select                 -  [IN] chip select to train on

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

static uint32 HAL_SDRAM_Get_Training_Address(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select)
{
  uint32 test_address = 0;

  ddr_info ddr_configuration = ddr_get_info();
  
  if(chip_select == SDRAM_BOTH)
	{
	 test_address = 0x80000000;
	}
  
  else if(ddr_configuration.ddr_size.sdram0_cs0_addr == 0x0)
	{
		if (chip_select == SDRAM_CS0)
		{
		  test_address = 0x20000000;
		}
		else if (chip_select == SDRAM_CS1)
		{
		  test_address = 0x80000000;
		}
	}
	else
	{
		if (chip_select == SDRAM_CS0)
		{
		  test_address = ddr_configuration.ddr_size.sdram0_cs0_addr;
		}
		else if (chip_select == SDRAM_CS1)
		{
		  test_address = ddr_configuration.ddr_size.sdram0_cs1_addr;
		}
	}
		
  return test_address;
};

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
void ddr_write(volatile uint32 *address, uint32 * pattern, uint32 size)
{
  uint32 pattern_offset;
  
    /* Memory controller writes data pattern into DRAM */
    for(pattern_offset = 0; pattern_offset <  size; pattern_offset++)
    {
      address[pattern_offset] = pattern[pattern_offset];
    }
}

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
void ddr_mem_write_low_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst)
{
 volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  struct ddr_device_params_common *ddr_params_ch0;
  uint32 ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
  ddr_interface_state ddr_status;

  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  
  ddr_status = ddr_get_status();
  
  /* Clock API to scale to low speed */
  HAL_SDRAM_BIMC_Update_Clock_Period(0, ddr_status.clk_speed, ddr_divide_func, ddr_type);
  HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed, ddr_type);    
  Clock_SetBIMCSpeed(ddr_status.clk_speed);  
  HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed, ddr_type);

  if(burst)
  {
    /* perform a burst write */
    __blocksCopy(pattern, address, size);
  }
  else
  {
    ddr_write(address, pattern, size);
  }

  if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
  
  	/* clock api to restore to high speed */
	  HAL_SDRAM_BIMC_Update_Clock_Period(0, ddr_fmax, ddr_divide_func, ddr_type);
	  HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_fmax, ddr_type);
	  Clock_SetBIMCSpeed(ddr_fmax);
	  HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_fmax, ddr_type);

  }
}

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
void ddr_mem_write_high_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst)
{
 volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  
  if(burst)
  {
    /* perform a burst write */
    __blocksCopy(pattern, address, size);
  }
  else
  {
    ddr_write(address, pattern, size);
  }
}

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
void ddr_mem_read( uint32 chnl, uint32 rank, uint32 * data, uint32 size, uint32 burst)
{
 volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  uint32 i;

  for(i = 0; i < size; i++)
  {
    data[i] = address[i];
  }
}

