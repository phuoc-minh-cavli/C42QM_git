#include "ddr_phy_training.h"
#include "bimc_training_apis.h"
//#include "vv_msg.h"


void HAL_SDRAM_BIMC_CA_Training_Pattern_Toggle(uint32 reg_base, SDRAM_INTERFACE interface)
{
	 uint32 offset;

  /* Configure for correct offset */
  if(interface == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }
//HAL_SDRAM_DDR_PHY_CA_training_mode(0, interface, 0);

//HAL_SDRAM_DDR_PHY_CA_training_mode(0, interface, 1);
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0 ),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0 ), CA_TRAIN_PATTERN_CMD),
                 1u << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0 ), CA_TRAIN_PATTERN_CMD));
	
//update - ngamini
}
void HAL_SDRAM_BIMC_CA_Training_Exit(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
	
	
  uint32 offset;

  /* Configure for correct offset */
  if(interface == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }

  /* Enable CKE */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON),
                 1u << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON));
				 
	while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                          HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON)) )
    {}
		

  /* Exit CA training Mode */
  HAL_SDRAM_Write_MR(reg_base,interface, (SDRAM_CHIPSELECT)chip_select, 42, 0xA8);

  /* Re-enable rank init */

  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE),
                   1u << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE));
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE),
                   1u << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE));
  }	
	
	
//update - ngamini
}
void HAL_SDRAM_BIMC_CA_Training_Pattern_Set(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint8 mapping, uint32
    pre_rising_pattern, uint32 pre_falling_pattern, uint32 rising_pattern, uint32 falling_pattern, uint32 post_rising_pattern,
    uint32 post_falling_pattern){
		
		uint32 offset;

  /* Configure for correct offset */
  if(interface == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }

  /* Ensure CKE is on is set */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON),
                 1u << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON));

  while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                          HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON)) )
    {}
				 
				 
  /* Set CA training set information mapping*/
  if (mapping == 0)
  {
    /* set mapping = 0 by triggering MR41*/
    HAL_SDRAM_Write_MR(reg_base,interface, (SDRAM_CHIPSELECT)chip_select, 41, 0xA4);
  }
  else
  {
    /* set mapping = 1 by triggering MR48*/
    HAL_SDRAM_Write_MR(reg_base,interface, (SDRAM_CHIPSELECT)chip_select, 48, 0xC0);
  }

  /* Disable INITCOMPLETE */

  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE),
                   0 << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE));
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE),
                   0 << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE));
  }


  /* Set SHKE_DRAM_MANUAL_0[RANK_SEL] to indicate the rank being calibrated.
   * Set SHKE_DRAM_MANUAL_0[CKE_OFF] to “1” in order to drive the memory clock
   */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF),
                 1u << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF));
				 

				 
    while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                          HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF)) )
    {}
  /* DPE_CA_TRAIN_PRE_CS: This is the PE/NE data for the CA bus 1 cycle before the
   * chip select.
   * DPE_CA_TRAIN_CS: This is the PE/NE data for the CA bus during the chip select cycle.
   * DPE_CA_TRAIN_POST_CS: This is the PE/NE data for the CA bus 1 cycle after the
   * chip select cycle.
  */



    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),CA_BUS_1), pre_rising_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),CA_BUS_1) );
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),CA_BUS_2), pre_falling_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_PRE_CS),CA_BUS_2) );

  


    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_CS),CA_BUS_3), rising_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_CS),CA_BUS_3) );
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_CS),CA_BUS_4), falling_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_CS),CA_BUS_4) );
				   
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_POST_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_POST_CS),CA_BUS_5), post_rising_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_POST_CS),CA_BUS_5) );
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CA_TRAIN_POST_CS),
                   offset,HWIO_FMSK(DDR_ADDR(DPE_CA_TRAIN_POST_CS),CA_BUS_6), post_falling_pattern << HWIO_SHFT(DDR_ADDR(DPE_CA_TRAIN_POST_CS),CA_BUS_6) );

//update - ngamini
}

//moved this function BIMC_DPE_V1.c file
#if 0
void HAL_SDRAM_DPE_Disable_Power_Down(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 offset;
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_PWR_CTRL_0),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), PWR_DN_EN) | HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), CLK_STOP_PWR_DN_EN),
      0x0);
	  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                 0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));
} /* HAL_SDRAM_DPE_Disable_Power_Down */
#endif

void HAL_SDRAM_DPE_Enable_Power_Down(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 offset;
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_PWR_CTRL_0),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), PWR_DN_EN) ,0x1 << HWIO_SHFT(DDR_ADDR(DPE_PWR_CTRL_0), PWR_DN_EN));
	  BOOT_HWIO_OUTM(
      DDR_ADDR(DPE_PWR_CTRL_0),
      offset,
      HWIO_FMSK(DDR_ADDR(DPE_PWR_CTRL_0), CLK_STOP_PWR_DN_EN) ,0x1 << HWIO_SHFT(DDR_ADDR(DPE_PWR_CTRL_0), CLK_STOP_PWR_DN_EN));
	  BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                 HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                 0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));
} /* HAL_SDRAM_DPE_Disable_Power_Down */

void BIMC_Disable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select)
{
 uint32 offset;
 offset = reg_base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
 
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_0), offset,
	               HWIO_FMSK(DDR_ADDR(DPE_CONFIG_0),CA_BUS_OE_CTRL), 
				   0 << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_0),CA_BUS_OE_CTRL) );
	 HAL_SDRAM_DPE_Disable_Power_Down(reg_base,interface);
  HAL_SDRAM_SHKE_Disable_Auto_Refresh(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Disable_ZQCAL(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Disable_Periodic_SRR(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
}

void BIMC_Enable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select)
{
 uint32 offset;
 offset = reg_base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
 
	BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_0), offset,
	               HWIO_FMSK(DDR_ADDR(DPE_CONFIG_0),CA_BUS_OE_CTRL), 
				   1 << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_0),CA_BUS_OE_CTRL) );
				   
	HAL_SDRAM_DPE_Enable_Power_Down(reg_base,interface);
  HAL_SDRAM_SHKE_Enable_Auto_Refresh(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Enable_ZQCAL(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
  HAL_SDRAM_SHKE_Enable_Periodic_SRR(reg_base,interface, (SDRAM_CHIPSELECT)chip_select);
}
