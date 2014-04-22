/**
 * @file DDR_PHY.c
 * @brief
 * Hardware abstraction layer of DDR PHY drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/phy/DDR_PHY.c#1 $
$DateTime: 2019/01/13 21:22:39 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/24/14   tw      removed statically compiled dq offsets to avoid access issue
                   from rpm when executing shared library
02/09/14   tw      added support for common ddr library
08/15/13   sl      Added functions for updating drive strength and CDC delay.
05/13/13   sl      Added delta table calls.
04/10/13   sl      Updated HAL_SDRAM_PHY_Update_CDC_Config().
02/26/13   sl      Added PHY settings of both channels.
11/16/12   sl      Update CDC configuration according to voltage.
10/31/12   sl      Changed ddr_static_config_init() to ddr_set_config().
09/26/12   sl      Initial version.
================================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_hwio.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "HAL_DDR.h"
#include "DDR_PHY.h"
#include "ddr_params.h"

/*==============================================================================
                                  DATA
==============================================================================*/

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR PHY.
*
*   @param[in]  interface  Interface to initialize for
*   @param[in]  clk_speed  Clock speed (in KHz) to initialize for
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
void HAL_SDRAM_PHY_Init(uint32 base, SDRAM_INTERFACE interface, uint32 clk_speed)
{
	
} /* HAL_SDRAM_PHY_Init */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable auto IOCAL.
*
*   @param[in]  interface  Interface to enable auto IOCAL for
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
void HAL_SDRAM_PHY_Enable_IO_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 ca_master_offset;

  /* Offset for CA Master PHY */
  ca_master_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  /* Enable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO) |
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_USE_LAST),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_USE_LAST));

} /* HAL_SDRAM_PHY_Enable_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable auto IOCAL.
*
*   @param[in]  interface  Interface to disable auto IOCAL for
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
void HAL_SDRAM_PHY_Disable_IO_Cal(uint32 base, SDRAM_INTERFACE interface, ddr_wait wait)
{
  uint32 ca_master_offset;

  /* Offset for CA Master PHY */
  ca_master_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  /* Disable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO));

  /* Wait for 65us (at least 2 sleep clock cycles + 3 XO clock cycles) */
  wait(65, 0);

  /* Check for any on-going IOCAL operations to be completed */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_STATUS), ca_master_offset,
                         HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_STATUS), IOCAL_DONE_D)) )
  {}

} /* HAL_SDRAM_PHY_Disable_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Manual_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Issue manual IOCAL.
*
*   @param[in]  interface  Interface to issue manual IOCAL for
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
void HAL_SDRAM_PHY_Manual_IO_Cal(uint32 base, SDRAM_INTERFACE interface, ddr_wait wait)
{
  uint32 ca_master_offset;
  uint32 bandgap_ena, io_cal_auto;

  /* Offset for CA Master PHY */
  ca_master_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  bandgap_ena = BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                              HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA1) |
                              HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA0));

  /* Do manual IOCAL only when bandgap is enabled */
  if ( bandgap_ena == (0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA1) |
                       0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA0)) )
  {
    io_cal_auto = BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                                HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO));

    if (io_cal_auto)
    {
      /* Disable auto IOCAL */
      HAL_SDRAM_PHY_Disable_IO_Cal(base, interface, wait);
    }

    /* Configure IOCAL controller for manual IOCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                   HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW));

    /* Check for IOCAL to be completed */
    while ( !BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_STATUS), ca_master_offset,
                           HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_STATUS), IOCAL_DONE_D)) )
    {}

    /* De-assert manual IOCAL trigger */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                   HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW));

    if (io_cal_auto)
    {
      /* Enable auto IOCAL */
      HAL_SDRAM_PHY_Enable_IO_Cal(base, interface);
    }
  }

} /* HAL_SDRAM_PHY_Manual_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable auto CDCCAL.
*
*   @param[in]  interface  Interface to enable auto CDCCAL for
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
void HAL_SDRAM_PHY_Enable_CDC_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;

    /* Enable DQ auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Enable DQ auto-CDCCAL timer */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Enable CA auto CDCCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

  /* Enable CA auto-CDCCAL timer */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

} /* HAL_SDRAM_PHY_Enable_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable auto CDCCAL.
*
*   @param[in]  interface  Interface to disable auto CDCCAL for
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
void HAL_SDRAM_PHY_Disable_CDC_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;

    /* Disable DQ auto-CDCCAL timer */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

    /* Disable DQ auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  }

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;
    
    /* Check state machine idle status */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS0), CTLR_SM_IDLE)) )
    {}
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS0), CTLR_SM_IDLE)) )
    {}
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Disable CA auto-CDCCAL timer */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  /* Disable CA auto CDCCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

  /* Check state machine idle status */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(CDC_STATUS0), offset,
                         HWIO_FMSK(PHY_CA_ADDR(CDC_STATUS0), CTLR_SM_IDLE)) )
  {}

} /* HAL_SDRAM_PHY_Disable_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Manual_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Issue manual CDCCAL.
*
*   @param[in]  interface  Interface to issue manual CDCCAL for
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
void HAL_SDRAM_PHY_Manual_CDC_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 hw_autocal_ena, timer_ena;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  hw_autocal_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CTLR_CFG0), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  timer_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), phy_offset + PHY_DQ0_OFFSET,
                            HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  if (hw_autocal_ena && timer_ena)
  {
    /* Disable auto CDCCAL */
    HAL_SDRAM_PHY_Disable_CDC_Cal(base, interface);
  }

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;
    
    /* Assert DQ SW CDCCAL trigger */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

    /* De-assert DQ SW CDCCAL trigger */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));
  }

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;
    
    /* Wait for DQ write CDCCAL done */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS0), CALIBRATION_DONE)) )
    {}
    /* Wait for DQ read CDCCAL done */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS0), CALIBRATION_DONE)) )
    {}
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Assert CA SW CDCCAL trigger */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

  /* De-assert CA SW CDCCAL trigger */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

  /* Wait for CA CDCCAL done */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(CDC_STATUS0), offset,
                         HWIO_FMSK(PHY_CA_ADDR(CDC_STATUS0), CALIBRATION_DONE)) )
  {}

  if (hw_autocal_ena && timer_ena)
  {
    /* Enable auto CDCCAL */
    HAL_SDRAM_PHY_Enable_CDC_Cal(base, interface);
  }

} /* HAL_SDRAM_PHY_Manual_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable rapid CDCCAL.
*
*   @param[in]  interface  Interface to enable rapid CDCCAL for
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
void HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 hw_autocal_ena, timer_ena;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  hw_autocal_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CTLR_CFG0), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  timer_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), phy_offset + PHY_DQ0_OFFSET,
                            HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  /* Enable rapid auto CDCCAL only when normal auto CDCCAL is enabled */
  if (hw_autocal_ena && timer_ena)
  {
    /* Disable normal auto CDCCAL */
    HAL_SDRAM_PHY_Disable_CDC_Cal(base, interface);

    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + PHY_DQ1_OFFSET * dq;
      
      /* Configure DQ CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                     0x10U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     0x8U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

      /* Enable DQ auto CDCCAL */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                     0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

      /* Enable DQ auto-CDCCAL timer based on ff_clk */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                     0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));
    }

    /* Offset for CA Master PHY */
    offset = phy_offset + PHY_CA_MST_OFFSET;

    /* Configure CA CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_REFCOUNT_CFG), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                   0x10U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   0x8U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

    /* Enable CA auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Enable CA auto-CDCCAL timer based on ff_clk */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));
  }

} /* HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable rapid CDCCAL.
*
*   @param[in]  interface  Interface to disable rapid CDCCAL for
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
void HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal(uint32 base, SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  /* Disable rapid auto CDCCAL only when rapid auto CDCCAL is enabled */
  if ( BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA)) )
  {
    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + PHY_DQ1_OFFSET * dq;
      
      /* Disable DQ auto-CDCCAL timer based on ff_clk */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                     0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));

      /* Disable DQ auto CDCCAL */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                     0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

      /* Configure DQ CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                     0x20U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     0x20U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));
    }

    /* Offset for CA Master PHY */
    offset = phy_offset + PHY_CA_MST_OFFSET;

    /* Disable CA auto-CDCCAL timer based on ff_clk */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));

    /* Disable CA auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Configure CA CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_REFCOUNT_CFG), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                   0x20U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   0x20U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

    /* Enable normal auto CDCCAL */
    HAL_SDRAM_PHY_Enable_CDC_Cal(base, interface);
  }

} /* HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Update_CDC_Config
** =============================================================================
*/
/**
*   @brief
*   Update CDC configuration according to voltage.
*
*   @param[in]  interface  Interface to update CDC configuration for
*   @param[in]  config     Configuration to update with
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
void HAL_SDRAM_PHY_Update_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);
  
#ifdef FEATURE_FORCE_DDR_TURBO_518_FOR_GPS
  if (config == 0x1D5)
     config = 0x1E2;
#endif

  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;
    
    /* Update DQ read CDC delay */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                   config << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));

    /* Update DQ write CDC delay */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                   config << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Update CA CDC delay */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                 config << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));

} /* HAL_SDRAM_PHY_Update_CDC_Config */

