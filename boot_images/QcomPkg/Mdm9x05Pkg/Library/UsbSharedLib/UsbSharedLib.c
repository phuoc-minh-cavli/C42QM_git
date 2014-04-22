/*=======================================================================*//**
 * @file        UsbSharedInit.c
 * @author:     gothekar
 * @date        15-Sept-2016
 *
 * @brief       USB common & shared USB core init implementation.
 *
 * @details     This file contains API's the Init sequence for HS and SS core implementation.
 *
 * @note        
 *              Copyright 2016-18 QUALCOMM Technologies Incorporated.
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
// 10/05/16     arung    Initial revision
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "DALSys.h"    // To get phy settings
#include "qusb_log.h"
#include "qusb_dci_common.h"
#include "UsbSharedLib.h"
#include "HalusbHWIO.h"
#include <stdio.h>
#include <string.h>

// initialization without reading the foundry information
//static uint32 qusb_dci_ss_phy_foundry_id = 0;

// Array keeps tracking of starting index of override programming sequence
// per foundry
/*static const uint32 qusb_dci_ss_phy_cfg_override_start_index[QUSB_DCI_SS_PHY_FOUNDRY_CNT_MAX] =
{ 
    0,    // 0
    16,   // 1
    32,   // 2
    48,   // 3
    64,   // 4
    80,   // 5
    96,   // 6
    112,  // 7
};
*/
// Between 2 foundries, there must be QUSB_HWIO_ADDR_END as sentinel value to
// prevent going over the boundry.
/*static const uint32 qusb_dci_ss_phy_cfg_address_override[QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT] = 
{
    //  0
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 16
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 32
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 48
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 64
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 80
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 96
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 112
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 128
};
*/
//ALIGN(4)
/*static const uint8 qusb_dci_ss_phy_cfg_value_override[QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT]    = 
{
  //  0
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 16
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 32
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 48
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 64
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 80
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 96
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 112
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 128
};*/

// ===========================================================================
/**
 * @function    usb_shared_lib_process_phy_reg_array
 * 
 * @brief   This function reads from array which define list of hwio writes for
 *          USB PHY
 * 
 * @param   address_array   - array holding address of HW register
 *          value_array     - array holding values to be written to HW register
 *          start_index     - starting index for array processing
 *          array_entry_cnt - number of entries in the array
 * 
 * @return  None
 * 
 */
// ===========================================================================
static void usb_shared_lib_process_phy_reg_array
(
  const uint32 *address_array, 
  const uint8  *value_array, 
  uint32        start_index, 
  uint32        array_entry_cnt
)
{
  uint32 index = start_index;
  
  usb_shared_uart_log_info("Process Array: ++");
  
  if ( (NULL == address_array)
      || (NULL == value_array)
      || (0 == array_entry_cnt) )
  {
    usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__FAIL_LOG, 0, (uint32)address_array);
    usb_shared_uart_log_error("Process_Array: Null Parameter Passed", (uint32)address_array);
  }
  else
  {
    usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__START_LOG, 0, start_index);
    
    for (; index < array_entry_cnt; index++)
    {
      if (QUSB_HWIO_ADDR_END == address_array[index])
      {
        break;
      }

      if (QUSB_HWIO_ADDR_EMPTY == address_array[index])
      {
        continue;
      }

      out_byte(address_array[index], value_array[index]);
    }
  }
  usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS____END_LOG, 0, index);
  usb_shared_uart_log_info("Process Array: --");
}

//============================================================================

/**
* @function USB_SHARED_HS_PHY_INIT
*
* @brief    API used to initialize the High Speed PHY 
*
*/
//============================================================================
boolean usb_shared_hs_phy_init()
{
  const char *hs_phy_cfg_addr_prop_name = "hs_phy_cfg_addr";
  const char *hs_phy_cfg_val_prop_name = "hs_phy_cfg_val";
  DALSYSPropertyVar dal_hs_phy_addr;
  DALSYSPropertyVar dal_hs_phy_val;
  DALResult result;
  DALSYS_PROPERTY_HANDLE_DECLARE(hs_phy_cfg_h);

  usb_shared_uart_log_info("usb_shared_hs_phy_init: ++");

  /* get handle to hs phy settings */ 
  result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_PRIM", hs_phy_cfg_h);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_hs_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
     return FALSE;
  }

  /* get hs phy address */
  result = DALSYS_GetPropertyValue(hs_phy_cfg_h, hs_phy_cfg_addr_prop_name, 0, &dal_hs_phy_addr);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy address Failed", result);
     return FALSE;
  }

  /* get hs phy values */
  result = DALSYS_GetPropertyValue(hs_phy_cfg_h, hs_phy_cfg_val_prop_name, 0, &dal_hs_phy_val);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy value Failed", result);
     return FALSE;
  }

  usb_shared_lib_process_phy_reg_array(
      (const uint32*) dal_hs_phy_addr.Val.pStruct,
      (const uint8*) dal_hs_phy_val.Val.pStruct,
      0,
      QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT);

  usb_shared_delay_us(20);

  usb_shared_uart_log_info("usb_shared_hs_phy_init: --");
  return TRUE; 
}

//============================================================================
/**
* @function USB_SHARED_HS_PHY_RUMI_INIT
*
* @brief API used to initialize High Speed RUMI PHY
*
* @Note : 
*
* @param    core_num
*             1st core (primary) = 0, 2nd = 1, etc
*           port_num
*             If core supports multiple ports, 0..4. (0 default)
*
* @return   TRUE- Success
*           FALSE- Failure
*
*/
//============================================================================
boolean usb_shared_hs_phy_rumi_init(uint32 core_num, uint32 phy_num)
{
// RUMI Phy Base Address for primary, sec and MP controllers
// Subject to change per RUMI release
#define RUMI_PHY_PRIM                            (USB20S_BASE + 0x0000CD00)  // 0x078CCD00
//#define RUMI_PHY_SEC                             (USB3_2ND_USB30_PRIM_BASE + 0x00120000)  // 0x0A920000
      
#define HWIO_HS2_EMU_VERSION_OFFS                (0x00000000)
#define HWIO_HS2_LOCAL_RESET_REG_OFFS            (0x00000004)
#define HWIO_HS2_CLK_STATUS_OFFS                 (0x00000010)
#define HWIO_HS2_CLK_STATUS_SEL_OFFS             (0x00000014)
#define HWIO_HS2_IMEM_DELAY_CTRL_OFFS            (0x00000100)
#define HWIO_HS2_IMEM_DELAY_MIN_DELAY_OFFS       (0x00000104)
#define HWIO_HS2_USB_CLK_CTRL_OFFS               (0x00000020)
#define HWIO_HS2_USB_CLK_STATUS_OFFS             (0x00000024)
#define HWIO_HS2_QRBTC_CONTROL_OFFS              (0x00000028)
#define HWIO_HS2_QRBTC_CONTROL_OE_OFFS           (0x0000002c)
#define HWIO_HS2_USB30_CTRL_OFFS                 (0x00000034)
#define HWIO_HS2_USB30_STATUS_OFFS               (0x00000038)
#define HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS         (0x0000003c)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_RISE_OFFS (0x00000040)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_RISE_OFFS (0x00000044)
#define HWIO_HS2_USB30_IRQ_DELAY_CTRL_OFFS       (0x00000048)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_FALL_OFFS (0x00000060)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_FALL_OFFS (0x00000064)
#define HWIO_HS2_SETUP_VERSION_OFFS              (0x00000050)
#define HWIO_HS2_USB_UNIPHY_CLK_CTRL_OFFS        (0x00000054)
#define HWIO_HS2_ROM_STRING_SIZE_OFFS            (0x00010004)

#define USB_RUMI_PHY_INIT_RETRY_COUNT_MAX        (5)

  uint32        phy_base         = RUMI_PHY_PRIM;
  const uint32  UTMI_MMCM_SHIFT  = 0x000001E0;
  uint32        retry_count;
  uint32        utmi_khz;
  
  // PHY number is not used since target does not support multi-port
  (void)phy_num;

  switch (core_num)
  {
    case 0:   phy_base = RUMI_PHY_PRIM;   break;
    //case 1:   phy_base = RUMI_PHY_SEC;    break;
    default:
    {
      usb_shared_uart_log_error("RUMI USB PHY invalid core", core_num);
    }
    break;
  }

  for (retry_count = 0; retry_count < USB_RUMI_PHY_INIT_RETRY_COUNT_MAX; retry_count++)
  {
    usb_shared_uart_log_error("RUMI PHY INIT:", retry_count);

    // Put everything in Reset except the PHY
    usb_shared_uart_log_error("RUMI PHY INIT", 0);
    out_dword(phy_base + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF0);
    usb_shared_delay_us(500);
    out_dword(phy_base + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF3);
    usb_shared_delay_us(500); 
    out_dword(phy_base + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF0);

    // Shifting ULPI/UTMI clock MMCM
    out_dword(phy_base + HWIO_HS2_USB_CLK_CTRL_OFFS, 0x00100000);
    usb_shared_delay_us(500);
    out_dword(phy_base + HWIO_HS2_USB_CLK_CTRL_OFFS, 0x00000000);
    usb_shared_delay_us(500);
    out_dword(phy_base + HWIO_HS2_USB_CLK_CTRL_OFFS, UTMI_MMCM_SHIFT);
    usb_shared_delay_us(500);

    usb_shared_uart_log_error("UTMI MMCM clock shift value set", UTMI_MMCM_SHIFT);

    // Shifting PIPE3 clock MMCM - Not used since only USB2 supported on RUMI
    out_dword(phy_base + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00100000);
    usb_shared_delay_us(500);
    out_dword(phy_base + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00000000);
    usb_shared_delay_us(500);
    out_dword(phy_base + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00000004);
    usb_shared_delay_us(500);

    // Clear the other Resets */
    out_dword(phy_base + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0x0);
    usb_shared_delay_us(500);

    // Put clock selector to read UTMI clock */
    out_dword(phy_base + HWIO_HS2_CLK_STATUS_SEL_OFFS, 0x00000009);
    usb_shared_delay_us(500);
    utmi_khz = in_dword(phy_base + HWIO_HS2_CLK_STATUS_OFFS);
    usb_shared_uart_log_error("UTMI CLK @ PAD (KHz):", utmi_khz);

    // Try until 60Mhz
    if (60 == (utmi_khz / 1000))
    {
      break;
    }
  }
  
  return TRUE;
}

//============================================================================

/**
* @function USB_SHARED_SS_PHY_INIT
*
* @brief    API used to initialize the Super Speed PHY 
*
*/
//============================================================================
boolean usb_shared_ss_phy_init(uint32 usb_ss_phy_lane)
{
#if 0
  uint32 qusb_timeout=0x0;
  const char *ss_phy_cfg_addr_prop_name = "ss_phy_cfg_addr";
  const char *ss_phy_cfg_val_prop_name = "ss_phy_cfg_val";
  DALSYSPropertyVar dal_ss_phy_addr;
  DALSYSPropertyVar dal_ss_phy_val;
  DALResult result;
  DALSYS_PROPERTY_HANDLE_DECLARE(ss_phy_cfg_h);

  usb_shared_error_log(QUSB_SS_PHY_INIT_LOG, 0, 0);
  usb_shared_uart_log_info("usb_shared_ss_phy_init: ++");

  /* get handle to ss phy settings */
  result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_PRIM", ss_phy_cfg_h);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
     return FALSE;
  }

  /* get ss phy address */
  result = DALSYS_GetPropertyValue(ss_phy_cfg_h, ss_phy_cfg_addr_prop_name, 0, &dal_ss_phy_addr);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy address Failed", result);
     return FALSE;
  }

  /* get ss phy values */
  result = DALSYS_GetPropertyValue(ss_phy_cfg_h, ss_phy_cfg_val_prop_name, 0, &dal_ss_phy_val);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy value Failed", result);
     return FALSE;
  }
  
    /* Put PHY into power down mode */
  HWIO_PERIPH_SS_USB3PHY_PCS_POWER_DOWN_CONTROL_OUT(0x1);

  usb_shared_lib_process_phy_reg_array(
    (const uint32*) dal_ss_phy_addr.Val.pStruct,
    (const uint8*) dal_ss_phy_val.Val.pStruct,
    0,
    QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT);

  // Read foundry specific information and program addition overrides
  // qusb_dci_ss_phy_foundry_id = pbl_get_foundry_id();
  if (qusb_dci_ss_phy_foundry_id >= QUSB_DCI_SS_PHY_FOUNDRY_CNT_MAX)
  {
    usb_shared_error_log(INVALID_QUSB_DCI_SS_PHY_FOUNDRY_ID_LOG, 0, qusb_dci_ss_phy_foundry_id);
    usb_shared_uart_log_error("usb_shared_ss_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
}
  else
  {
    usb_shared_lib_process_phy_reg_array(
      qusb_dci_ss_phy_cfg_address_override, 
      qusb_dci_ss_phy_cfg_value_override, 
      qusb_dci_ss_phy_cfg_override_start_index[qusb_dci_ss_phy_foundry_id],
      QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT);
  }

  // Override Type-C lane detection  override
  HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_OUTM(
    HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_SW_PORTSELECT_BMSK, 
    usb_ss_phy_lane << HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_SW_PORTSELECT_SHFT);

  HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_OUTM(
    HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_SW_PORTSELECT_MX_BMSK,
    0x1 << HWIO_PERIPH_SS_USB3PHY_PCS_MISC_TYPEC_CTRL_SW_PORTSELECT_MX_SHFT);

  usb_shared_delay_us(100);
  
  HWIO_PERIPH_SS_USB3PHY_PCS_SW_RESET_OUT(0x0);
  HWIO_PERIPH_SS_USB3PHY_PCS_START_CONTROL_OUT(0x3);

  usb_shared_delay_us(100);

  /* Poll PHYSTATUS field of Register USB3_PHY_PCS_STATUS to Go LOW after reset*/
  while(HWIO_PERIPH_SS_USB3PHY_PCS_PCS_STATUS_INM(
        HWIO_PERIPH_SS_USB3PHY_PCS_PCS_STATUS_PHYSTATUS_BMSK))
  {
    usb_shared_delay_us(100);
    if (qusb_timeout++ > 100)
    {
        USB_SHARED_ASSERT(DCI_SS_PHY_RESET_ERROR_LOG);
        return FALSE;
    }
  }
  
  usb_shared_uart_log_info("usb_shared_ss_phy_init: --");
#endif
  return TRUE;
}

//============================================================================
/**
* @function  usb_dci_get_chip_serial_number
*
* @brief Call API to uint32 unique MSM serial string
*
* @Note : None
*
* @param  None 
*
* @return uint32: Chip serial number
*
*/
//============================================================================
uint32 usb_dci_get_chip_serial_number(void)
{
  return HWIO_SERIAL_NUM_IN;
}

//============================================================================
/** 
 * API is used to add chip serial number to the string and return 
 * Product String Descriptor
 * 
 * @param pointer to product string
 *              Pointer to the buffer to be updated with Product string desc
 *              Length of the buffer
 *
 * @return Appended string length
 */ 
 //============================================================================
uint32 usb_get_product_str_desc_with_chip_serial(char* product_str, char* prod_str_desc, uint32 desc_len)
{
  char    chip_serial_str[QUSB_FD_PRODUCT_STRING_LENGTH_MAX];
  uint32  chip_serial;
  uint32  str_desc_index;
  uint32  str_desc_size;
  uint32  chip_str_len;
  uint32  chip_str_index;

  if ((NULL == product_str)||(NULL == prod_str_desc) )
  {
    // String is NULL;
    return 0;
  }

  chip_serial = usb_dci_get_chip_serial_number();
  
  chip_str_len = snprintf(chip_serial_str, sizeof(chip_serial_str), "%s_SN:%08X", product_str, chip_serial);

  if (chip_str_len >= sizeof(chip_serial_str))
  {
    // Chip Seiral is too long to fit
    return 0;
  }

  // "ASCII string size" X 2 (UNICODE) 
  //   + 2 (1 byte SIZE and 1 byte STRING DESC TYPE ENUM)
  str_desc_size = (chip_str_len * 2) + 2;

  if (str_desc_size >= desc_len)
  {
    // Destination buffer over flow
    return 0;    
  }

  str_desc_index = 0;
  prod_str_desc[str_desc_index++] = (char)str_desc_size;
  prod_str_desc[str_desc_index++] = USB_DESCTYPE_STRING;

  for (chip_str_index = 0; chip_str_index < chip_str_len; chip_str_index++)
  {
    prod_str_desc[str_desc_index++] = chip_serial_str[chip_str_index];
    prod_str_desc[str_desc_index++] = 0x0;  
  }

  return str_desc_size;
}


