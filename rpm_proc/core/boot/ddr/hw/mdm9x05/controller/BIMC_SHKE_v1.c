/*
===========================================================================

FILE:         BIMC_SHKE_v1.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for 
  BIMC SHKE.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/controller/BIMC_SHKE_v1.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/21/14   tw      Default write 34ohmz for MR3
02/09/14   tw      added support for common ddr library
08/23/13   rj      Added support to read drive strength from CDT and 
                   configure.
09/29/12   sl      Updated wait timer for writing MR1.
07/19/12   sl      Updated HAL_SDRAM_SHKE_Enter/Exit_Self_Refresh().
09/17/12   sl      Updated HAL_SDRAM_SHKE_Device_Init().
09/12/12   sl      Use macro-style DDR logging.
08/24/12   sl      Added/updated SHKE functions.
08/17/12   sl      Added HAL_SDRAM_Read_MR().
08/06/12   tw      New api for querying init done on a specific rank
05/15/12   tw      New api for SRR, ZQCAL, DPD
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

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
extern uint32 ddr_divide_func(uint32 a, uint32 b);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Device_Init
** ============================================================================
*/
/**
*   @brief
*   Initialize DDR device.
*
*   @param[in]  interface    Interface to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_speed    Clock speed (in KHz)
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
void HAL_SDRAM_SHKE_Device_Init(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  uint32 offset;
  struct ddr_device_params_common *ddr_params;
  struct ddr_device_params_lpddr *ddr_params_lpddr;
  uint32 data, rl, wl;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Get DDR device parameters */
  ddr_params = &(ddr_get_params(interface)->common);
  ddr_params_lpddr = &(ddr_get_params(interface)->lpddr); 

  /* Set WAIT_TIMER_DOMAIN to XO Clock domain and set WAIT_TIMER_BEFORE_HW_CLEAR to 0x5 */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x5 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));   

  /* start the memory clock and indicate which ranks are being initialized */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CK_ON) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL), 
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CK_ON) |
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));     

  /* 
    Poll for clk_on status 
    CK_ON bit is self cleared to 0 once it is done
  */
  while( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), 
                       offset, 
                       HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0),CK_ON)))
  {}

  /* 
    Set WAIT_TIMER_DOMAIN back to time clock domain 
    WAIT_TIMER_BEFORE_HW_CLEAR to 0x11 (this is updated for 500 us according to HPG)
  */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x11 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));     

  /* start the memory clock and indicate which ranks are being initialized */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL), 
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_ON) |
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));     

  /* 
    Poll for cke_on status
    CKE_ON bit is self cleared to 0 once it is done
  */
  while( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), 
                       offset, 
                       HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0),CKE_ON)))
  {}

  /* 
    Set WAIT_TIMER_DOMAIN back to time clock domain 
    WAIT_TIMER_BEFORE_HW_CLEAR to 0x2
  */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x2 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));     
  
  if(ddr_params->device_type == DDR_TYPE_LPDDR2)
  {
    /* Write to 0x3F for ddr reset */
    HAL_SDRAM_Write_MR(base, interface, chip_select, 0x3F, 0);
  }
  else if(ddr_params->device_type == DDR_TYPE_LPDDR3)
  {

    /* Write to 0x3F with data 0xFC for ddr reset */
    HAL_SDRAM_Write_MR(base, interface, chip_select, 0x3F, 0xFC);

    /*
      WAIT_TIMER_BEFORE_HW_CLEAR to 0x0
    */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR),
                   0x0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));
  }


  /* 
    Set WAIT_TIMER_DOMAIN to xo domain
    WAIT_TIMER_BEFORE_HW_CLEAR to 0x14
  */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x14 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));
  
  /* ZQ CAL needs to be done on a per rank basis */
  if (chip_select & SDRAM_CS0)
  {
    /* Long ZQCAL for CS0 */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG),
                   SDRAM_CS0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG));

    while( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                         HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG)) )
    {}
  }
  if (chip_select & SDRAM_CS1)
  {
    /* Long ZQCAL for CS1 */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG),
                   SDRAM_CS1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG));

    while( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                         HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ZQCAL_LONG)) )
    {}
  }

  /* 
    Set WAIT_TIMER_DOMAIN to XO domain 
    WAIT_TIMER_BEFORE_HW_CLEAR to 0x5
  */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x5 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));       

  /* Program MR1 */
  /*  
    Configure the Device Feature 1 Mode Register in the LPDDR2 device using the following
    sequence. This sets attributes like burst length & addressing mode in the memory device.
  
    MR1
  //       BA1 BA0     A13 A12 A11 A10    A9 A8      A7 A6 A5       A4           A3                     A2 A1 A0
  //        0   0       R   R   R   R     R  R         nWR          WC         BURST TYPE              BURST LENGTH
  //                                                "110" nWR=8    "0" Wrap    "0" SEQ                "010" BL=4
  //                                                                           "1" INTERLEAVED        "011" BL=8(Use)      
  //                                                                                                  "100" BL=16(Use)   
  */

  /* set nWR for 0b110 */
  data = 0xC0;

  /* P1 design can only support BL of 8 */
  switch(ddr_params_lpddr->burst_length)
  {
    case 4:
      data |= 0x2;
      break;
    case 8:
      data |= 0x3;
      break;
    case 16:
      data |= 0x4;
      break;
    default:
      break;
  }

  HAL_SDRAM_Write_MR(base, interface, chip_select, 0x1, data);

  /* To configure drive strength. 3rd byte of CAS latency 
     field is use as drive strength parameter. If the field
	 is 0, then goes with default parameter */
#ifdef FEATURE_GEN1_PHY
  data = 0;
#else
  data = ((ddr_params->cas_latency) >> 0x10) & 0xF;
#endif

  if (data != 0)
  {
    HAL_SDRAM_Write_MR(base, interface, chip_select, 0x3, data);
  }
  else
  {
    /* Configure MR3 for optimal drive strength */
    HAL_SDRAM_Write_MR(base, interface, chip_select, 0x3, 0x1);  
  }
  
  /* Set SHKE_DRAM_MANUAL_1[WAIT_TIMER_DOMAIN] to “1” (XO clock domain) and
   * set SHKE_DRAM_MANUAL_1[WAIT_TIMER_BEFORE_HW_CLEAR] to “0x5”.
   */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR),
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_DOMAIN) |
                 0x5 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));


  /* Program MR2 */
  rl = HAL_SDRAM_RL((DDR_TYPE)ddr_params->device_type, clk_speed);
  wl = HAL_SDRAM_WL((DDR_TYPE)ddr_params->device_type, clk_speed);

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
  
  /* Mark ddr as initialized */
  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE),
                   0x1 << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE));
  }  
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE),
                   0x1 << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE));
  }   

  /* Clear WAIT_TIMER_BEFORE_HW_CLEAR to 0 */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                 offset, 
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                 0x0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));       

} /* HAL_SDRAM_SHKE_Device_Init */

/* =============================================================================
**  Function : HAL_SDRAM_Read_MR
** =============================================================================
*/
/**
*   @brief
*   Read DDR mode register.
*
*   @param[in]  interface    Interface to read mode register for
*   @param[in]  chip_select  Chip select to read mode register for
*   @param[in]  mr_addr      Mode register address
*
*   @return
*   Mode register data that is read back.
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
uint32 HAL_SDRAM_Read_MR(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  if (chip_select == SDRAM_BOTH)
  {
    chip_select = SDRAM_CS0;
  }

  /* Configure mode register address */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_ADDR),
                 mr_addr << HWIO_SHFT(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_ADDR));

  /* Configure chip select and trigger mode register read */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_READ),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_READ));

  /* Poll for mode register read done */
  while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                        HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_READ)) )
  {}

  return (chip_select == SDRAM_CS0) ? BOOT_HWIO_IN(DDR_ADDR(SHKE_MREG_RDATA_RANK0_L), offset)
                                    : BOOT_HWIO_IN(DDR_ADDR(SHKE_MREG_RDATA_RANK1_L), offset);

} /* HAL_SDRAM_Read_MR */

/* =============================================================================
**  Function : HAL_SDRAM_Write_MR
** =============================================================================
*/
/**
*   @brief
*   Write DDR mode register.
*
*   @param[in]  interface    Interface to write mode register for
*   @param[in]  chip_select  Chip select to write mode register for
*   @param[in]  mr_addr      Mode register address
*   @param[in]  mr_data      Mode register data to write
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
void HAL_SDRAM_Write_MR(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Configure mode register and data */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_ADDR) |
                 HWIO_FMSK(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_WDATA),
                 mr_addr << HWIO_SHFT(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_ADDR) |
                 mr_data << HWIO_SHFT(DDR_ADDR(SHKE_MREG_ADDR_WDATA_CNTL), MREG_WDATA));

  /* Configure chip select */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  if (mr_addr == 41 || mr_addr == 42 || mr_addr == 48)
  {
    /* Trigger extended mode register write */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXTND_MODE_REGISTER_WRITE),
                   0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXTND_MODE_REGISTER_WRITE));

    /* Poll for mode register write done */
    while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                          HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXTND_MODE_REGISTER_WRITE)) )
    {}
  }
  else
  {
    /* Trigger normal mode register write */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_WRITE),
                   0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_WRITE));

    /* Poll for mode register write done */
    while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                          HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), MODE_REGISTER_WRITE)) )
    {}
  }

} /* HAL_SDRAM_Write_MR */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the requested ddr into self refresh
*
*   @details
*   This function will put the requested ddr into self refresh.
*   Note in dual rank configuration both rank must go into self refresh
*   at the same time.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Enter_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Configure chip select and enter self refresh */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_SELF_REFRESH_IDLE),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_SELF_REFRESH_IDLE));

  /* Poll for self refresh entry done */
  while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                        HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_SELF_REFRESH_IDLE)) )
  {}

  /* Make sure DDR is in self refresh */
  while ( !BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_STATUS), offset,
                         HWIO_FMSK(DDR_ADDR(SHKE_DRAM_STATUS), SW_SELF_RFSH)) )
  {}

} /* HAL_SDRAM_SHKE_Enter_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take the requested ddr out of self refresh
*
*   @details
*   This function will take the requested ddr out of self refresh.
*   Note in dual rank configuration both rank must go and out of self refresh
*   at the same time.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Exit_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Configure chip select and exit self refresh */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXIT_SELF_REFRESH),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                 0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXIT_SELF_REFRESH));

  /* Poll for self refresh exit done */
  while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                        HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXIT_SELF_REFRESH)) )
  {}

  /* Make sure DDR is out of self refresh */
  while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_STATUS), offset,
                        HWIO_FMSK(DDR_ADDR(SHKE_DRAM_STATUS), SW_SELF_RFSH)) )
  {}

} /* HAL_SDRAM_SHKE_Exit_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_HW_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take turn on the hw based self refresh
*
*   @details
*   This function will take turn on the hardware based self refresh.
*   This will put the ddr into self refresh after a predetermined idle duration.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  if (chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK0),
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK0));
  }
  if (chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK1),
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK1));
  }

} /* HAL_SDRAM_SHKE_Enable_HW_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_HW_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take turn off the hw based self refresh
*
*   @details
*   This function will take turn off the hardware based self refresh.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  if (chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK0),
                   0x0U << HWIO_SHFT(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK0));
  }
  if (chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK1),
                   0x0U << HWIO_SHFT(DDR_ADDR(SHKE_SELF_REFRESH_CNTL), HW_SELF_RFSH_ENABLE_RANK1));
  }

} /* HAL_SDRAM_SHKE_Disable_HW_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_ZQCAL
** ============================================================================
*/
/*!
*   @brief
*   This function will turn on period ZQCAL
*
*   @details
*   This function will turn on period ZQCAL
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Enable_ZQCAL(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset, value;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Read-modify-write SHKE_PERIODIC_ZQCAL[RANK_SEL] */
  value = BOOT_HWIO_IN(DDR_ADDR(SHKE_PERIODIC_ZQCAL), offset);
  value |= (chip_select << HWIO_SHFT(DDR_ADDR(SHKE_PERIODIC_ZQCAL), RANK_SEL));
  BOOT_HWIO_OUT(DDR_ADDR(SHKE_PERIODIC_ZQCAL), offset, value);

} /* HAL_SDRAM_SHKE_Enable_ZQCAL */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_ZQCAL
** ============================================================================
*/
/*!
*   @brief
*   This function will turn off period ZQCAL
*
*   @details
*   This function will turn off period ZQCAL
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
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
void HAL_SDRAM_SHKE_Disable_ZQCAL(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset, value;

  /* Channel offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Read-modify-write SHKE_PERIODIC_ZQCAL[RANK_SEL] */
  value = BOOT_HWIO_IN(DDR_ADDR(SHKE_PERIODIC_ZQCAL), offset);
  value &= ~(chip_select << HWIO_SHFT(DDR_ADDR(SHKE_PERIODIC_ZQCAL), RANK_SEL));
  BOOT_HWIO_OUT(DDR_ADDR(SHKE_PERIODIC_ZQCAL), offset, value);

} /* HAL_SDRAM_SHKE_Disable_ZQCAL */

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_DPD_Enter
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr device into deep power down mode
*
*   @details
*   This function will put the ddr device into deep power down mode
*
*   @param interface   -  [IN] ddr interface select
*   @param chip_select      -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   All content on the ddr being put into deep power down will be lost.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_DPD_Enter(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;
  struct ddr_device_params_common *ddr_param;
  SDRAM_CHIPSELECT populated_cs = (SDRAM_CHIPSELECT)0;

  /* Configure for correct offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  
  /* Get DDR device parameters */
  ddr_param = &(ddr_get_params(interface)->common);

  if(ddr_param->num_banks_cs0 != 0)
  {
    populated_cs |= SDRAM_CS0;
  }
  if(ddr_param->num_banks_cs1 != 0)
  {
    populated_cs |= SDRAM_CS1;
  }

  /* Disable ZQ CAL */
  HAL_SDRAM_SHKE_Disable_ZQCAL(base, interface, chip_select);

  /* if all populated ranks are entering DPD, periodic CDC and IO CAL */
  if (populated_cs == chip_select)
  {
    /*
     *TODO : enable these api when ddr phy is done
    HAL_SDRAM_Disable_CDC
    HAL_SDRAM_Disable_IO_Cal
    */
  }

  /* Disable HW based self refresh */
  HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(base, interface, chip_select);

  /* Issue commend to put ddr into DPD */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_DEEP_PD),
                   1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_DEEP_PD));

  /* Poll for MANUAL CMD until Command goes through */
  while(
      BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
      HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), ENTER_DEEP_PD)));

  /* Poll for DPD status until device is in DPD */

  if(chip_select & SDRAM_CS0)
  {
    while(
        BOOT_HWIO_INM(DDR_ADDR(DPE_DRAM_STATUS_0), offset,
        HWIO_FMSK(DDR_ADDR(DPE_DRAM_STATUS_0), RANK_0_DPD)) == 0);
  }
  if(chip_select & SDRAM_CS1)
  {
    while(
        BOOT_HWIO_INM(DDR_ADDR(DPE_DRAM_STATUS_0), offset,
        HWIO_FMSK(DDR_ADDR(DPE_DRAM_STATUS_0), RANK_1_DPD)) == 0);
  }

  /* Disable CKE */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL),
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF),
                 1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF));

  while(
      BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
      HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF)));

  /* Set
   * INITCOMPLETE to 0
   * RANK1/0_EN to 0
   */

  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE) |
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_EN) ,
                   0x0);
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG),
                   offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE) |
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_EN),
                   0x0);
  }
} /* HAL_SDRAM_SHKE_DPD_Enter */


/* ============================================================================
**  Function : HAL_SDRAM_SHKE_DPD_Exit
** ============================================================================
*/
/*!
*   @brief
*   This function will take the ddr device out of deep power down mode
*
*   @details
*   This function will take the ddr device out of deep power down mode
*
*   @param interface   -  [IN] ddr interface select
*   @param chip_select      -  [IN] ddr chip select
*   @param clk_speed        -  [IN] ddr clock speed (in KHz)
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   All content on the ddr that was on ddr is gone, reset value is not specified.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_DPD_Exit(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  uint32 offset;

  /* Configure for correct offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  
  HAL_SDRAM_DPE_Disable_Power_Down(base, interface);

  /* Set SHKE_DRAM_MANUAL_0[RANK_SEL] to the specified rank */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) ,
                 chip_select << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL));

  /* Set SHKE_DRAM_MANUIAL_0[EXIT_DEEP_PD] */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXIT_DEEP_PD) ,
                 0x1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), EXIT_DEEP_PD));

  /* Perform re-initialization of DRAM device */
  HAL_SDRAM_SHKE_Device_Init(base, interface, chip_select, clk_speed);

} /* HAL_SDRAM_SHKE_DPD_Exit */
