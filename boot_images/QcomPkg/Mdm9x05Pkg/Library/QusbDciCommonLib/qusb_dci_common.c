/*=======================================================================*//**
 * @file        qusb_dci_common.c
 * @author:     kameya
 * @date        28-Dec-2015
 *
 * @brief       QUSB common parts of DCI shared by QusbFedlLib & QusbTargetLib.
 *
 * @details     This file contains the implementation of basic APIs to be used
 *              by both QusbTargetLib and QusbFedlLib
 *
 * @note        
 *
 *              Copyright 2015 - 2022 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2016-10-05    arung   Add UsbSharedLib
// 2015-12-28    kameya  First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"    // basic types as byte word uint32 etc
#include "busywait.h"  // busywait APIs
#include "DALSys.h"    // To get phy settings
#include "qusb_dci_common.h"
#include "HalusbHWIO.h"
#include "qusb_log.h"
#include "UsbSharedLib.h"
#include "DDIChipInfo.h"

//defines for enabling the 1.5k pull up 
#define QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN     (HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN   (HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS   (0x1 << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_SHFT)
#define QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE          (0x0 << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE_SHFT)
#define QUSB2PHY_PORT_UTMI_CTRL1_OP_NON_MODE      (0x1 << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE_SHFT)

#define QUSB_LINESTATE_CHECK_DELAY                (0x5)
#define QUSB_LINESTATE_CHECK_RETRY_CNT            (10000)  //50msec is the total wait time to deal with a bad cable


#ifdef QUSB_FEDL
// ---------------------------------------------------------------------------
// Force Emergency Download (EDL) detecting Dp Ground ++
// ---------------------------------------------------------------------------
#define USB_FEDL_MAX_DP_LOW_TIMEOUT_MS   (50)
#define USB_FEDL_MAX_DP_HIGH_TIMEOUT_MS  (10000)
#define USB_FEDL_DP_BMSK                 (0x1)

// Force EDL check using Dp GND and pull up to Dp HIGH back in timeout
// This will prevent device accidentally entering EDL when connected to
// powered off USB port on PC or Hub that has D+ GND.
// It requires User to unplug special Dp GND USB cable within the timeout
// in order to enter EDL.
boolean qusb_forced_download_checks_dp_high = TRUE;
#endif

//============================================================================

/**
* @function  qusb_dci_delay_ms
*
* @brief Perform delay in milliseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param milliseconds
*
* @return none
*
*/
//============================================================================
void qusb_dci_delay_ms(uint32 msecs)
{
  int counter;

  for (counter = 0; counter < msecs; counter++)
  {
    busywait(1000); // 1 ms
  }   
}

//============================================================================
/**
* @function  qusb_dci_delay_us
*
* @brief Perform delay in microseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param microseconds
*
* @return none
*
*/
//============================================================================
void qusb_dci_delay_us(uint32 usecs)
{
  busywait( usecs );
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_gcc_reset
 * 
 * @brief   API used for resetting High Speed QUSB2 PHY using GCC control
 *  
 * @details API is used for resetting High Speed QUSB2 PHY using GCC control
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_hs_phy_gcc_reset(void)
{
  HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,
    (0x1 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,
    (0x0 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
}

// ===========================================================================
/**
 * @function    qusb_dci_usb30_gcc_reset
 * 
 * @brief   API used for resetting the Link and PHYs using GCC control
 *  
 * @details This API is used for resetting the Link and PHYs using clock control 
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_usb30_gcc_reset(void)
{
  /* Reset SNPS Link controller */
  HWIO_GCC_USB_HS_BCR_OUTM(HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT));
    
  qusb_dci_delay_us(100);
    
  HWIO_GCC_USB_HS_BCR_OUTM(HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT));
    
  /* Reset USB 2.0 and QMP (USB 3.0) PHYs */

  // SNPS Femto GCC RESET
  HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
  //HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));
  //HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x1 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);
    
  //HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x0 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));
  //HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));
  HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
    
  qusb_dci_delay_us(100);
  return; 
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_refclk_enable
 * 
 * @brief   This function will be used to enable / disable HS PHY reference clock.
 * 
 * @param  TRUE or FALSE depending on enable or disable.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_hs_phy_refclk_enable(boolean enable)
{

#if 0
  HWIO_GCC_RX0_USB2_CLKREF_EN_OUTM(HWIO_GCC_RX0_USB2_CLKREF_EN_RX0_USB2_ENABLE_BMSK,
    enable << HWIO_GCC_RX0_USB2_CLKREF_EN_RX0_USB2_ENABLE_SHFT);

  HWIO_GCC_RX0_USB2_CLKREF_EN_OUTM(HWIO_GCC_RX0_USB2_CLKREF_EN_SW_RXTAP1_EN_BMSK,
    enable << HWIO_GCC_RX0_USB2_CLKREF_EN_SW_RXTAP1_EN_SHFT);
#endif

}

// ===========================================================================
/**
 * @function    qusb_dci_select_utmi_clk
 * 
 * @brief   This is used for configuring the core to UTMI clock instead of pipe
 *          clock.  This needs to be called when there is no SS USB PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_select_utmi_clk(void)
{
  qusb_error_log(QUSB_DCI_SELECT_UTMI_CLK_LOG, 0, 0);
  
  // If operating without SS PHY, follow this sequence to disable 
  // pipe clock requirement
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
                      0x1 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);

  qusb_dci_delay_us(100);

  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK,
                      0x1 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_SHFT);

  qusb_dci_delay_us(100);
  
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK,
    0x1 << HWIO_GENERAL_CFG_PIPE3_PHYSTATUS_SW_SHFT);

  qusb_dci_delay_us(100);

  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
                      0x0 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);
}

#ifdef QUSB_FEDL
void qusb_dci_dp_high(void)
{
  // Set UTMI_PHY_CMN_CNTRL_OVERRIDE_EN
  HWIO_USB2_PHY_USB_PHY_CFG0_OUTM(HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_BMSK,
    0x1 << HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_SHFT);

  // Set UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN
  HWIO_USB2_PHY_USB_PHY_CFG0_OUTM(HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_BMSK,
    0x1 << HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_SHFT);

  // Set OPMODE to normal i.e. 0x00
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OUTM(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OPMODE_BMSK,
    0x0 << HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OPMODE_SHFT);

  // Set XCVRSEL and TERMSEL to FS.
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_OUTM(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_BMSK,
    0x1 << HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_SHFT);

  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OUTM(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_BMSK,
    0x1 << HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_SHFT);
}


void qusb_dci_dp_default_state(void)
{
  // Clear TERMSEL and XCVRSEL
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OUTM(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_BMSK,
    0x0 << HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_SHFT);

  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_OUTM(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_BMSK,
    0x0 << HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_SHFT);

  // Clear UTMI_PHY_CMN_CTRL_OVERRIDE_EN
  HWIO_USB2_PHY_USB_PHY_CFG0_OUTM(HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_BMSK,
    0x0 << HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_SHFT);

  // Clear UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN
  HWIO_USB2_PHY_USB_PHY_CFG0_OUTM(HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_BMSK,
    0x0 << HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_SHFT);
}
#endif

// ===========================================================================
/**
 * @function    qusb_dci_hs_phy_update_hstx_trim
 * 
 * @brief   This function will update TUNE2 HSTX_TRIM register bits if feature is enabled.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_hs_phy_update_hstx_trim(void)
{
#if 0
    uint8 hstx_trim_val = 
        HWIO_QFPROM_CORR_CALIB_ROW11_LSB_INM(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_QUSB_PORT0_HSTX_TRIM_LSB_BMSK) 
        >> HWIO_QFPROM_CORR_CALIB_ROW11_LSB_QUSB_PORT0_HSTX_TRIM_LSB_SHFT;

    if(hstx_trim_val)
    {
       /* HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_TUNE2_OUTM(
            HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_TUNE2_UTM_HSTX_TRIM_BMSK, 
            (hstx_trim_val - QUSB_HSTX_TRIM_OFFSET) << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_TUNE2_UTM_HSTX_TRIM_SHFT);*/
    }
#endif
}

// =============================================================================
/**
 * @function    qusb_dci_ahb2phy_cfg
 * 
 * @brief   Configures AHB2PHY read/write states to be 1 wait cycle each.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// =============================================================================
void qusb_dci_ahb2phy_cfg(void)
{
  qusb_dci_delay_us(10);
  
  // Write 0x11 to AHB2PHY bridge CSR PERIPH_SS_AHB2PHY_TOP_CFG so that 
  // writes and reads to/from the PHY use one wait state.
  // This is essential to operate at nominal freq with lower CX rail voltages.
  // Need to check why is it not accessible in Device Programmer path
  HWIO_EBI1_AHB2PHY_TOP_CFG_OUT(0x11);
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_init
 * 
 * @brief   API used to initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
boolean qusb_hs_phy_init(void)
{
  return usb_shared_hs_phy_init();
}

// ===========================================================================
/**
 * @function    qusb_dci_dp_gnd_check
 * 
 * @brief   API used to check if D+ is grounded.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
#ifdef QUSB_FEDL
boolean qusb_dci_dp_gnd_check(void)
{
  uint8 linestate = 0;
  uint16 ls_tries = 0;
  boolean enter_fedl = FALSE;

  qusb_uart_log("fedl: check start", qusb_forced_download_checks_dp_high);

  qusb_dci_dp_high();

  usb_shared_delay_us(20);

  // Enter forced EDL mode if D+ is grounded for 50ms.
  for(ls_tries = 0; (linestate == 0) && (ls_tries < USB_FEDL_MAX_DP_LOW_TIMEOUT_MS);  ls_tries++)
  {
     linestate = (HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_INM(
       HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_BMSK)
       >> HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_SHFT) & USB_FEDL_DP_BMSK;

    if(linestate == USB_FEDL_DP_BMSK)
    {
      break;
    }
    usb_shared_delay_us(1000);
  }

  if ((linestate == 0) && (ls_tries == USB_FEDL_MAX_DP_LOW_TIMEOUT_MS))
  {
    qusb_uart_log("dpGnd detected", 0);

    // Force EDL check using Dp GND and pull up to Dp HIGH back in timeout
    // This flag prevent device accidentally entering EDL when connected to
    // powered off USB port on PC or Hub that has D+ GND.
    // It requires User to unplug special Dp GND USB cable within the timeout
    // in order to enter EDL.
    if (FALSE == qusb_forced_download_checks_dp_high)
    {

      // Enter forced EDL mode just with D+ GND.
      // qusb_uart_log("fedl: skip check dp high, enter FEDL", qusb_forced_download_checks_dp_high);
      enter_fedl = TRUE;
    }
    else
    {
      // Enter forced EDL mode if D+ is now back-up within N seconds.
      for(ls_tries = USB_FEDL_MAX_DP_HIGH_TIMEOUT_MS; (linestate == 0) && (ls_tries > 0);  ls_tries--)
      {
	       linestate = (HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_INM(
       HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_BMSK)
       >> HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_SHFT) & USB_FEDL_DP_BMSK;
        

        if(ls_tries%1000==0)
        {
          qusb_uart_log("timeout in ms:", ls_tries);
        }

        if (linestate == USB_FEDL_DP_BMSK)
        {
          qusb_uart_log("dpHigh, enter FEDL", 0);
          enter_fedl = TRUE;
          break;
        }
        usb_shared_delay_us(1000);
      }
    }
  }

  qusb_dci_dp_default_state();

  usb_shared_delay_us(20);

  qusb_error_log(DCI_ENTER_FEDL_LOG, 0, enter_fedl);

  return enter_fedl;
}
#endif

// =============================================================================
/**
 * @function    qusb_hs_phy_nondrive_cfg
 * 
 * @brief   Function used to configure High Speed PHY in non-drive mode.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// =============================================================================
void qusb_hs_phy_nondrive_cfg(void)
{

 // uint32 enable_fs_xvr = 0x0;

  //Disable the D+ Pull down 
  /*HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_DPPULLDOWN_BMSK, 
    (0x0 <<HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_DPPULLDOWN_SHFT ));
    

  // Disable the D- Pull down 
  HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_DMPULLDOWN_BMSK,
      (0x0 <<HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_DMPULLDOWN_SHFT));
  
  // Select the FS transceiver and put into non-drive mode
  // Putting phy to suspend as SUSPEND_N = 0 (power on default)
  enable_fs_xvr = (QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN
                   | QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS
                   | QUSB2PHY_PORT_UTMI_CTRL1_OP_NON_MODE 
                   | QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN);

  HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL1_OUT(enable_fs_xvr);

  // Enable UTMI_TEST_MUX_SEL
  HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_BMSK,
    0x1 << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_SHFT);
*/

  qusb_dci_delay_us(10);

  // Keep PHY in power down
  /*HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_POWERDOWN_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
    (0x1 << HWIO_PERIPH_SS_QUSB2PHY_PRIM_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));

  //Power down the Secondary QUSB2 PHY to plug leakage
  if (DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_SDM660)
  {
    HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PORT_POWERDOWN_OUTM(
	  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
	  (0x1 << HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));
  }
*/
}

