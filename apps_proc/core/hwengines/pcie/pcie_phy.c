/**
 * @file pcie_phy.c
 * @brief
 * Implements the interfaces to the PCIe PHY HW
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "comdef.h"
#include "pcie_phy.h"
#include "pcie_hwio.h"
#include "pcie_i.h"
#include "DDIChipInfo.h"

#define PCIE_PHY_CFG_VER_1            /** Only define the phy version you want to use here */

/** Structure to hold PCIe PHY register addresses and values */
typedef struct
{
   uint32 pcie_phy_regaddrs;                              /** < -- Offsets for the registers to be configured */
   uint8  __attribute__((aligned(4))) pcie_phy_regvals;   /** < -- Values for the registers listed above */
} pcie_phy_reg_addrs_vals;

/* Configures the PCIe PHY */
static void pcie_phyi_config(void);

/* Starts the PCIe PHY */
static void pcie_phyi_start(void);

/* Returns whether the PHY is ready or not */
static boolean pcie_phyi_isready(void);

/* Waits for PHY to stabilize and become ready for operation */
static boolean pcie_phyi_waitforready(void);

/** PCIe PHY register address value pair */
const pcie_phy_reg_addrs_vals  phy_reg_config[] =
{
#ifdef PCIE_PHY_PBL_SEQ
   /*  Settings for PBL */

   { QSERDES_COM_SYSCLK_EN_SEL,                                 0x00 }, /* QSERDES_COM_SYSCLK_EN_SEL */
   { QSERDES_COM_BIAS_EN_CLKBUFLR_EN,                           0x18 }, /* QSERDES_COM_BIAS_EN_CLKBUFLR_EN */
   { QSERDES_COM_BG_TRIM,                                       0x0F }, /* QSERDES_COM_BG_TRIM */
   { QSERDES_COM_RESETSM_CNTRL,                                 0x20 }, /* QSERDES_COM_RESETSM_CNTRL */
   { QSERDES_TX_LANE_MODE,                                      0x06 }, /* QSERDES_TX_LANE_MODE */
   { QSERDES_COM_DEC_START_MODE0,                               0x19 }, /* QSERDES_COM_DEC_START_MODE0 */
   { QSERDES_COM_CP_CTRL_MODE0,                                 0x3F }, /* QSERDES_COM_CP_CTRL_MODE0 */
   { QSERDES_COM_PLL_RCTRL_MODE0,                               0x1A }, /* QSERDES_COM_PLL_RCTRL_MODE0 */
   { QSERDES_COM_PLL_CCTRL_MODE0,                               0x00 }, /* QSERDES_COM_PLL_CCTRL_MODE0 */
   { QSERDES_COM_INTEGLOOP_GAIN1_MODE0,                         0x00 }, /* QSERDES_COM_INTEGLOOP_GAIN1_MODE0 */
   { QSERDES_COM_INTEGLOOP_GAIN0_MODE0,                         0xFF }, /* QSERDES_COM_INTEGLOOP_GAIN0_MODE0 */
   { QSERDES_COM_SVS_MODE_CLK_SEL,                              0x01 }, /* QSERDES_COM_SVS_MODE_CLK_SEL */
   { QSERDES_COM_CORE_CLK_EN,                                   0x00 }, /* QSERDES_COM_CORE_CLK_EN */
   { QSERDES_COM_LOCK_CMP2_MODE0,                               0x04 }, /* QSERDES_COM_LOCK_CMP2_MODE0 */
   { QSERDES_COM_LOCK_CMP1_MODE0,                               0xFF }, /* QSERDES_COM_LOCK_CMP1_MODE0 */
   { QSERDES_COM_LOCK_CMP_EN,                                   0x42 }, /* QSERDES_COM_LOCK_CMP_EN */
   { QSERDES_COM_VCO_TUNE_MAP,                                  0x00 }, /* QSERDES_COM_VCO_TUNE_MAP */
   { QSERDES_COM_VCO_TUNE_TIMER2,                               0x3F }, /* QSERDES_COM_VCO_TUNE_TIMER2 */
   { QSERDES_COM_VCO_TUNE_TIMER1,                               0xFF }, /* QSERDES_COM_VCO_TUNE_TIMER1 */
   { QSERDES_COM_BG_TIMER,                                      0x01 }, /* QSERDES_COM_BG_TIMER */
   { QSERDES_COM_CLK_SELECT,                                    0x30 }, /* QSERDES_COM_CLK_SELECT */
   { QSERDES_COM_PLL_IVCO,                                      0x0F }, /* QSERDES_COM_PLL_IVCO */

   { QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE,                  0x4B }, /* QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE */
   { QSERDES_RX_UCDR_FO_GAIN,                                   0x0A }, /* QSERDES_RX_UCDR_FO_GAIN */
   { QSERDES_RX_UCDR_FO_GAIN_HALF,                              0x0A }, /* QSERDES_RX_UCDR_FO_GAIN_HALF */
   { QSERDES_RX_UCDR_SO_GAIN,                                   0x04 }, /* QSERDES_RX_UCDR_SO_GAIN */
   { QSERDES_RX_UCDR_SO_GAIN_HALF,                              0x04 }, /* QSERDES_RX_UCDR_SO_GAIN_HALF */
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2,                          0x01 }, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2 */
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3,                          0x00 }, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3 */
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4,                          0xDB }, /* QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4 */
   { QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1,                    0x77 }, /* QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1 */
   { QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2,                       0x80 }, /* QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2 */
   { QSERDES_RX_SIGDET_ENABLES,                                 0x1C }, /* QSERDES_RX_SIGDET_ENABLES */
   { QSERDES_RX_SIGDET_CNTRL,                                   0x03 }, /* QSERDES_RX_SIGDET_CNTRL */
   { QSERDES_RX_SIGDET_DEGLITCH_CNTRL,                          0x14 }, /* QSERDES_RX_SIGDET_DEGLITCH_CNTRL */

   { QSERDES_TX_RES_CODE_LANE_OFFSET,                           0x02 }, /* QSERDES_TX_RES_CODE_LANE_OFFSET */
   { QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN,               0x45 }, /* QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN */
   { QSERDES_TX_RCV_DETECT_LVL_2,                               0x12 }, /* QSERDES_TX_RCV_DETECT_LVL_2 */
   { QSERDES_COM_CMN_CONFIG,                                    0x06 }, /* QSERDES_COM_CMN_CONFIG */

   { PCIE_USB3_PCS_OSC_DTCT_ACTIONS,                            0x0A }, /* PCIE_USB3_PCS_OSC_DTCT_ACTIONS */
   { PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL,                          0x4C }, /* PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL */
   { PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0,                          0x0F }, /* PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0 */
   { PCIE_USB3_PCS_POWER_STATE_CONFIG1,                         0xA3 }, /* PCIE_USB3_PCS_POWER_STATE_CONFIG1 */
   { PCIE_USB3_PCS_POWER_STATE_CONFIG2,                         0x1B }, /* PCIE_USB3_PCS_POWER_STATE_CONFIG2 */
   { PCIE_USB3_PCS_POWER_STATE_CONFIG4,                         0x00 }, /* PCIE_USB3_PCS_POWER_STATE_CONFIG4 */
   { PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L,                        0xF1 }, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L */
   { PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H,                        0x01 }, /* PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H */
   { PCIE_USB3_PCS_LOCK_DETECT_CONFIG1,                         0x80 }, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG1 */
   { PCIE_USB3_PCS_LOCK_DETECT_CONFIG2,                         0x02 }, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG2 */
   { PCIE_USB3_PCS_LOCK_DETECT_CONFIG3,                         0x40 }, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG3 */
   { PCIE_USB3_PCS_LOCK_DETECT_CONFIG4,                         0x07 }, /* PCIE_USB3_PCS_LOCK_DETECT_CONFIG4 */
   { PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK,                 0x40 }, /* PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK */
   { PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK,                   0x40 }, /* PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK */
   { PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB,             0x40 }, /* PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB */
   { PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME,                       0x73 }, /* PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME */
   { PCIE_USB3_PCS_ELECIDLE_DLY_SEL,                            0x01 }, /* PCIE_USB3_PCS_ELECIDLE_DLY_SEL */
   { PCIE_USB3_PCS_SIGDET_CNTRL,                                0x07 }, /* PCIE_USB3_PCS_SIGDET_CNTRL */
   { PCIE_USB3_PCS_RX_SIGDET_LVL,                               0x77 }, /* PCIE_USB3_PCS_RX_SIGDET_LVL */

   { 0xFFFFFFFF,                                                0x00 }, /* END */
#endif /* PCIE_PHY_PBL_SEQ */

#ifdef PCIE_PHY_CFG_VER_1

   /* Common block settings */
   { QSERDES_COM_BIAS_EN_CLKBUFLR_EN,                           0x18 },
   { QSERDES_COM_CLK_ENABLE1,                                   0x00 }, /* Disable Endpoint clock divider/driver in EP mode */
   { QSERDES_COM_BG_TRIM,                                       0x0F }, /* Moved BG_TRIM before CMN_CONFIG */
   { QSERDES_COM_LOCK_CMP_EN,                                   0x01 },
   { QSERDES_COM_VCO_TUNE_MAP,                                  0x00 },
   { QSERDES_COM_VCO_TUNE_TIMER1,                               0xFF },
   { QSERDES_COM_VCO_TUNE_TIMER2,                               0x1F },
   { QSERDES_COM_CMN_CONFIG,                                    0x06 },
   { QSERDES_COM_PLL_IVCO,                                      0x0F },
   { QSERDES_COM_HSCLK_SEL,                                     0x00 },
   { QSERDES_COM_SVS_MODE_CLK_SEL,                              0x01 },
   { QSERDES_COM_CORE_CLK_EN,                                   0x20 },
   { QSERDES_COM_CORECLK_DIV,                                   0x0A },
   { QSERDES_COM_RESETSM_CNTRL,                                 0x20 },
   { QSERDES_COM_BG_TIMER,                                      0x01 },

   /* PLL Config Settings */
   { QSERDES_COM_SYSCLK_EN_SEL,                                 0x00 },
   { QSERDES_COM_DEC_START_MODE0,                               0x19 },
   { QSERDES_COM_DIV_FRAC_START3_MODE0,                         0x00 },
   { QSERDES_COM_DIV_FRAC_START2_MODE0,                         0x00 },
   { QSERDES_COM_DIV_FRAC_START1_MODE0,                         0x00 },
   { QSERDES_COM_LOCK_CMP3_MODE0,                               0x00 },
   { QSERDES_COM_LOCK_CMP2_MODE0,                               0x02 },
   { QSERDES_COM_LOCK_CMP1_MODE0,                               0x7F },
   { QSERDES_COM_CLK_SELECT,                                    0x30 },
   { QSERDES_COM_SYS_CLK_CTRL,                                  0x06 },
   { QSERDES_COM_SYSCLK_BUF_ENABLE,                             0x1E },
   { QSERDES_COM_CP_CTRL_MODE0,                                 0x3F }, /* From silicon characterization of PLL loopbandwidth result */
   { QSERDES_COM_PLL_RCTRL_MODE0,                               0x1A }, /* From silicon characterization of PLL loopbandwidth result */
   { QSERDES_COM_PLL_CCTRL_MODE0,                               0x00 }, /* From silicon characterization of PLL loopbandwidth result */
   { QSERDES_COM_INTEGLOOP_GAIN1_MODE0,                         0x03 }, /* From silicon characterization of PLL loopbandwidth result */
   { QSERDES_COM_INTEGLOOP_GAIN0_MODE0,                         0xFF }, /* From silicon characterization of PLL loopbandwidth result */

   /* TX settings */
   { QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN,               0x45 },
   { QSERDES_TX_LANE_MODE,                                      0x06 },
   { QSERDES_TX_RES_CODE_LANE_OFFSET,                           0x02 },
   { QSERDES_TX_RCV_DETECT_LVL_2,                               0x12 },

   /* RX settings */
   { QSERDES_RX_SIGDET_ENABLES,                                 0x1C },
   { QSERDES_RX_SIGDET_DEGLITCH_CNTRL,                          0x14 },
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2,                          0x01 },
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3,                          0x00 },
   { QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4,                          0xDB },
   { QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE,                  0x4B },
   { QSERDES_RX_UCDR_SO_GAIN,                                   0x04 },
   { QSERDES_RX_UCDR_SO_GAIN_HALF,                              0x04 },

   /* EP_REF_CLK settings */
   { QSERDES_COM_CLK_EP_DIV,                                    0x19 }, /* POR value, this may change after si char. */
   { PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE,                       0x00 }, /* Disable EP clock. */

   /* PCIE L1SS settings */
   { PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK,                 0x40 },
   { PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB,             0x00 },
   { PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB,             0x40 },
   { PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_MSB,               0x00 },
   { PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK,                   0x40 },
   { PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME,                       0x73 },

   /* PCS settings */
   { PCIE_USB3_PCS_SIGDET_CNTRL,                                0x07 }, /* Disable the feature of masking rx_elecidle */
   { PCIE_USB3_PCS_RX_SIGDET_LVL,                               0x99 },
   { PCIE_USB3_PCS_TXDEEMPH_M6DB_V0,                            0x15 },
   { PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0,                          0x0E },

   { 0xFFFFFFFF,                                                0x00 }, /* END */
#endif /* PCIE_PHY_CFG_VER_1 */

};

/* ============================================================================
**  Function : pcie_phyi_config
** ============================================================================
*/
/**
 * Configures  the PHY. Starts the PHY.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_phyi_config(void)
{
   uint32 index;
   pcie_osal_debug(MSG_HIGH, "Begin");

   /* Release pwrdn_b Active low enable for powering down PHY */
   pcie_i_write_reg(pcie_dev->phy_base,  PCIE_USB3_PCS_POWER_DOWN_CONTROL, 0x01);

   for(index = 0;index < sizeof(phy_reg_config) / sizeof(pcie_phy_reg_addrs_vals);index++)
   {
      if(0xFFFFFFFF == phy_reg_config[index].pcie_phy_regaddrs)
      {
         break;
      }
      pcie_i_write_reg(pcie_dev->phy_base, phy_reg_config[index].pcie_phy_regaddrs, phy_reg_config[index].pcie_phy_regvals);
   }

   if(pcie_dev->dev_version == DALCHIPINFO_VERSION(1,0))
   {
      /* Allow PLL logic to control the Bandgap */
      pcie_i_write_reg(pcie_dev->phy_base,QSERDES_COM_BIAS_EN_CTRL_BY_PSM,0x01);
      pcie_i_write_reg(pcie_dev->phy_base,QSERDES_COM_BG_TIMER,0x02);
   }

   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_phyi_start
** ============================================================================
*/
/**
 * Starts the PHY.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_phyi_start(void)
{
   pcie_osal_debug(MSG_HIGH, "Begin");
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_SW_RESET, 0x00);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_START_CONTROL,
                    PCIE_MASK(PCIE_USB3_PCS_START_CONTROL, PCS_START) |
                       PCIE_MASK(PCIE_USB3_PCS_START_CONTROL, SERDES_START)
                   );
   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_phyi_isready
** ============================================================================
*/
/**
 * Returns whether the PHY is ready or not.
 *
 * @param[in]  void
 *
 * @return     boolean
 *    TRUE - PHY is ready; FALSE - otherwise.
 */
static boolean pcie_phyi_isready(void)
{
   /* poll de-assertion(logic 0) of PHYSTATUS, which indicates PHY is ready*/
   return(0 == pcie_i_read_reg_field(pcie_dev->phy_base, PCIE_FLD(PCIE_USB3_PCS_PCS_STATUS, PHYSTATUS)));
}

/* ============================================================================
**  Function : pcie_phyi_waitforready
** ============================================================================
*/
/**
 * Waits for PHY to stabilize and become ready for operation.
 *
 * @param[in]  void
 *
 * @return     boolean
 *    TRUE if Phy is ready else FALSE.
 */
static boolean pcie_phyi_waitforready(void)
{
   pcie_osal_debug(MSG_HIGH, "Begin");
   /* Check for PHY Ready */
   while(!pcie_phyi_isready());

   pcie_osal_debug(MSG_HIGH, "Exit");
   return(pcie_phyi_isready());
}

/* ============================================================================
**  Function : pcie_phy_init
** ============================================================================
*/
/**
 * Performs initialization of the PCIe PHY.
 *
 * @param[in]  void
 *
 * @return     pcie_return_status
 *    Status of the initialization.
 */
pcie_return_status pcie_phy_init(void)
{
   pcie_osal_debug(MSG_HIGH, "Begin");

   /* Configure the PHY */
   pcie_phyi_config();

   /* Start PHY */
   pcie_phyi_start();

   /* Wait for PHY ready */
   if(FALSE == pcie_phyi_waitforready())
   {
      pcie_osal_debug(MSG_HIGH, "Exit: PHY INIT FAILED");
      return(PCIE_FAILED);
   }

   pcie_res_clk_select_extern_src();

   pcie_osal_debug(MSG_HIGH, "Exit");
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_phy_config_update
** ============================================================================
*/
/**
 * Update PHY additional settings.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_config_update(void)
{
   /* Disable the feature of masking rx_elecidle */
   pcie_i_write_reg(pcie_dev->phy_base,PCIE_USB3_PCS_SIGDET_CNTRL,0x07);

   if(pcie_dev->dev_version == DALCHIPINFO_VERSION(1,0))
   {
      /* Allow PLL logic to control the Bandgap */
      pcie_i_write_reg(pcie_dev->phy_base,QSERDES_COM_BIAS_EN_CTRL_BY_PSM,0x01);
      pcie_i_write_reg(pcie_dev->phy_base,QSERDES_COM_BG_TIMER,0x02);
   }

#ifndef PCIE_BOOT
   /* Notify the Clock driver for external frequency of the Pipe clock */
   pcie_res_clk_select_extern_src();
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_phy_clamp_interfaces
** ============================================================================
*/
/**
 * CLAMP digital and logical IO.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_clamp_interfaces(void)
{
   /* CLAMP digital and logical IO */
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_POWER_DOWN_CONTROL, 0);
}

/* ============================================================================
**  Function : pcie_phy_unclamp_interfaces
** ============================================================================
*/
/**
 * Unclamp digital and logical IO.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_phy_unclamp_interfaces(void)
{
   /* Release pwrdn_b */
   pcie_i_write_reg(pcie_dev->phy_base,PCIE_USB3_PCS_POWER_DOWN_CONTROL, 0x01);
}

