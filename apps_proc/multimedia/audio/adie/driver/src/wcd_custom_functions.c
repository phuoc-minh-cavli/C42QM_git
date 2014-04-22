/**
@file wcd_custom_functions.c
@brief
Custom function definition for WCD codec driver.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_custom_functions.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/12/14   KRP      Reset WG timer before PA enable, as a fix to the no audio issue and remove workaround delay
01/16/14   AG       Update for the disable sequence for HPHR
09/30/13   AG       SAL warning fixes after logging is enabled in debug mode
09/20/13   AG       Update compander shutdown sequence
                    Diff Line compander shutdown is not being handled correctly
                    and the Comp2 ch2 is never being disabled, so the compander
                    for LO diff is not disabled properly
                    Update compander sample rate dependent values
09/11/13   AG       Set DMIC TLMM bit to operational mode
                    Earout CnP changes
09/04/13   KNM      Reverted Earout sequence update as THD is bad with that
09/02/13   KNM      Temporarily 40ms delay has been added before HPH PA enable
                    to ensure that clock is available before enabling of PAs.
09/01/13   KNM      Class-h parameters of headset are updated with 20% reduction
                    in load from the actual value that is asked for in QCRG. Earout
                    impedance table is also added. Corrected ADC analog gain setting.
08/06/13   SP       Optimized headset micbias opa enable & hph enable function.
08/06/13   KNM      DMIC leakage mitigation by switching DMIC TLMM pins.
07/23/13   KNM      Added low impedance support for headset, set ear pa bias current
                    to 8uA and V5V6 mode specific changes.
07/15/13   KNM      Added ADC concurrency support.
06/18/13   SP/KNM   Support HPH PA enable/disable command concurrency between
                    MBHC and Core WCD modules. Cosmetic changes to remove tabs.
05/31/13   KNM      Adopted wcd9306/02 2.0 hw revision specific changes and
                    common code logging messages related changes.
03/21/13   KNM      Fixed Few more SAL warnings in micbias manager enable function.
03/04/13   KNM      Fixed SAL warnings in micbias manager function.
03/01/13   KNM      Initial revision.
===============================================================================*/

#include "wcd_custom_functions.h"
#include "wcd_gen_id_defines.h"
#include "wcd_gen_elements.h"
#include "bus_manager.h"
#include "wcd_os_specific.h"
#include "wcd_utility.h"
#include "target_defs.h"
#ifdef WCD_MBHC_ENABLE
  #include "mbhc_state.h"
#endif



#ifdef SPL_LOG_SUPPORT
	#include "wcd_custom_functions.tmh"
#endif

#define WCD_MAX_DB_GAIN           40
#define WCD_MIN_DB_GAIN           -84

#define WCD_RESET_DELAY_MS        1

/* Enable Reset for TX path Decimator */
#define WCD_PATH_RESET_ENABLE     0x1
#define WCD_PATH_RESET_DISABLE    0x0

#define WCD_PATH_ENABLE           0x1
#define WCD_PATH_DISABLE          0x0

#define WCD_RX_BUS_PORT_COUNT     5
#define WCD_TX_BUS_PORT_COUNT     5

#define WCD_RX_PATH_COUNT         4

#define WCD_DMIC_COUNT            2
#define WCD_DMIC_ENABLE           0x1
#define WCD_DMIC_DISABLE          0x0

#define WCD_ADC_COUNT             5
#define WCD_ADC_ENABLE_TXFE       0x1
#define WCD_ADC_DISABLE_TXFE      0x0
#define WCD_ADC_ENABLE            0x1
#define WCD_ADC_DISABLE           0x0

#define WCD_EAROUT_IMPEDANCE      32  // 32ohm

wcd_bool HS_MBIAS_OPA_EN = WCD_FALSE;
wcd_bool HS_HPHL_EN = WCD_FALSE, HS_HPHR_EN = WCD_FALSE;

uint8 HPH_PA_State = 0;

#ifdef WCD_MBHC_ENABLE
static uint8 Z_DET_HPH_CLSH_TABLE[15][15] = {
  //load, 0x324, 0x326, 0x32A, 0x32B, 0x32A, 0x328, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329
  {    4,  0x04,  0x0C,  0x40,  0x03,  0x15,  0x00,  0x11,  0x02,  0x9B,  0x02,  0x15,  0x01,  0x1C,  0x01},
  {    6,  0x07,  0x0F,  0x40,  0x15,  0x08,  0x00,  0xAC,  0x02,  0x17,  0x01,  0x5F,  0x01,  0xCF,  0x01},
  {    8,  0x09,  0x12,  0x60,  0x04,  0x16,  0x00,  0x11,  0x01,  0x37,  0x01,  0xA6,  0x01,  0xAE,  0x01},
  {   10,  0x0C,  0x14,  0x60,  0x08,  0x0A,  0x00,  0x14,  0x01,  0xB7,  0x01,  0x13,  0x00,  0x14,  0x00},
  {   12,  0x0E,  0x15,  0x60,  0x16,  0x19,  0x00,  0x25,  0x01,  0xAE,  0x01,  0x1F,  0x00,  0x1D,  0x00},
  {   14,  0x10,  0x16,  0x80,  0x05,  0x08,  0x00,  0x2C,  0x01,  0xAC,  0x01,  0x1D,  0x00,  0x1C,  0x00},
  {   16,  0x13,  0x17,  0x80,  0x06,  0x09,  0x00,  0xD0,  0x01,  0x14,  0x00,  0x1B,  0x00,  0x1B,  0x00},
  {   18,  0x15,  0x17,  0x80,  0x06,  0x18,  0x00,  0xB7,  0x01,  0x10,  0x00,  0x1B,  0x00,  0x24,  0x00},
  {   20,  0x17,  0x18,  0x80,  0x08,  0x0C,  0x00,  0xA4,  0x01,  0x1D,  0x00,  0x24,  0x00,  0x25,  0x00},
  {   22,  0x19,  0x17,  0x80,  0x19,  0x00,  0x00,  0xAE,  0x01,  0x1C,  0x00,  0x25,  0x00,  0x27,  0x00},
  {   24,  0x1C,  0x13,  0x80,  0x17,  0x0B,  0x00,  0xAC,  0x01,  0x1B,  0x00,  0x20,  0x00,  0x2E,  0x00},
  {   26,  0x1F,  0x13,  0xA0,  0x06,  0x08,  0x00,  0x13,  0x00,  0x1B,  0x00,  0x2E,  0x00,  0x2D,  0x00},
  {   28,  0x22,  0x14,  0xA0,  0x06,  0x1A,  0x00,  0x14,  0x00,  0x24,  0x00,  0x2D,  0x00,  0x2C,  0x00},
  {   30,  0x22,  0x14,  0xA0,  0x06,  0x1A,  0x00,  0x14,  0x00,  0x24,  0x00,  0x2D,  0x00,  0x2C,  0x00},
  {   32,  0x25,  0x15,  0xA0,  0x07,  0x09,  0x00,  0x10,  0x00,  0x25,  0x00,  0x2C,  0x00,  0x1C,  0x00}
};
#endif

static uint8 Z_DET_EAROUT_CLSH_TABLE[15][15] = {
  //load, 0x325, 0x327, 0x32C, 0x32D, 0x32C, 0x328, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329, 0x329
  {    4,  0x04,  0x14,  0x40,  0x03,  0x15,  0x08,  0x11,  0x02,  0x9B,  0x02,  0x15,  0x01,  0x1C,  0x01},
  {    6,  0x07,  0x19,  0x40,  0x15,  0x08,  0x08,  0xAC,  0x02,  0x17,  0x01,  0x5F,  0x01,  0xCF,  0x01},
  {    8,  0x09,  0x1C,  0x60,  0x04,  0x16,  0x08,  0x11,  0x01,  0x37,  0x01,  0xA6,  0x01,  0xAE,  0x01},
  {   10,  0x0C,  0x20,  0x60,  0x08,  0x0A,  0x08,  0x14,  0x01,  0xB7,  0x01,  0x13,  0x00,  0x14,  0x00},
  {   12,  0x0E,  0x22,  0x60,  0x16,  0x19,  0x08,  0x25,  0x01,  0xAE,  0x01,  0x1F,  0x00,  0x1D,  0x00},
  {   14,  0x10,  0x23,  0x80,  0x05,  0x08,  0x08,  0x2C,  0x01,  0xAC,  0x01,  0x1D,  0x00,  0x1C,  0x00},
  {   16,  0x13,  0x25,  0x80,  0x06,  0x09,  0x08,  0xD0,  0x01,  0x14,  0x00,  0x1B,  0x00,  0x1B,  0x00},
  {   18,  0x15,  0x26,  0x80,  0x06,  0x18,  0x08,  0xB7,  0x01,  0x10,  0x00,  0x1B,  0x00,  0x24,  0x00},
  {   20,  0x17,  0x27,  0x80,  0x08,  0x0C,  0x08,  0xA4,  0x01,  0x1D,  0x00,  0x24,  0x00,  0x25,  0x00},
  {   22,  0x19,  0x28,  0x80,  0x19,  0x00,  0x08,  0xAE,  0x01,  0x1C,  0x00,  0x25,  0x00,  0x27,  0x00},
  {   24,  0x1C,  0x28,  0x80,  0x17,  0x0B,  0x08,  0xAC,  0x01,  0x1B,  0x00,  0x20,  0x00,  0x2E,  0x00},
  {   26,  0x1F,  0x29,  0xA0,  0x06,  0x08,  0x08,  0x13,  0x00,  0x1B,  0x00,  0x2E,  0x00,  0x2D,  0x00},
  {   28,  0x22,  0x2A,  0xA0,  0x06,  0x1A,  0x08,  0x14,  0x00,  0x24,  0x00,  0x2D,  0x00,  0x2C,  0x00},
  {   30,  0x22,  0x2A,  0xA0,  0x06,  0x1A,  0x08,  0x14,  0x00,  0x24,  0x00,  0x2D,  0x00,  0x2C,  0x00},
  {   32,  0x25,  0x2B,  0xA0,  0x07,  0x09,  0x08,  0x10,  0x00,  0x25,  0x00,  0x2C,  0x00,  0x1C,  0x00}
};

/**********************************************************************************
 *******************                  BUS PORT                  *******************
 *********************************************************************************/

uint32 sbrx_bringup_count[WCD_RX_BUS_PORT_COUNT] = { 0 };
uint32 sbtx_bringup_count[WCD_TX_BUS_PORT_COUNT] = { 0 };

//default bitwidth is 24 on wcd9306/02
uint32 sbrx_bitwidth[WCD_RX_BUS_PORT_COUNT] = { 24 };
uint32 sbtx_bitwidth[WCD_TX_BUS_PORT_COUNT] = { 24 };

wcd_result WCDH_BUSPORT_SET_BITWIDTH(wcd_element* p_element, uint32 bitwidth)
{
  uint32* bringup_count = NULL;
  uint32* assigned_bitwidth = NULL;

  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = 0x0;

  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    if (p_element == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_SET_BITWIDTH. Busport element passed in is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    switch(p_element->id)
    {
      case WCD_SBRX_1_ID:
        bringup_count = &(sbrx_bringup_count[0]);
        assigned_bitwidth = &(sbrx_bitwidth[0]);
        reg = 0x3AE;
        mask = 0x03;
        shift = 0;
        break;
      case WCD_SBRX_2_ID:
        bringup_count = &(sbrx_bringup_count[1]);
        assigned_bitwidth = &(sbrx_bitwidth[1]);
        reg = 0x3AE;
        mask = 0x0C;
        shift = 2;
        break;
      case WCD_SBRX_3_ID:
        bringup_count = &(sbrx_bringup_count[2]);
        assigned_bitwidth = &(sbrx_bitwidth[2]);
        reg = 0x3AE;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBRX_4_ID:
        bringup_count = &(sbrx_bringup_count[3]);
        assigned_bitwidth = &(sbrx_bitwidth[3]);
        reg = 0x3AE;
        mask = 0xC0;
        shift = 6;
        break;
      case WCD_SBRX_5_ID:
        bringup_count = &(sbrx_bringup_count[4]);
        assigned_bitwidth = &(sbrx_bitwidth[4]);
        reg = 0x3AF;
        mask = 0x03;
        shift = 0;
        break;
      case WCD_SBTX_1_ID:
        bringup_count = &(sbtx_bringup_count[0]);
        assigned_bitwidth = &(sbtx_bitwidth[0]);
        reg = 0x3A3;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_2_ID:
        bringup_count = &(sbtx_bringup_count[1]);
        assigned_bitwidth = &(sbtx_bitwidth[1]);
        reg = 0x3A4;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_3_ID:
        bringup_count = &(sbtx_bringup_count[2]);
        assigned_bitwidth = &(sbtx_bitwidth[2]);
        reg = 0x3A5;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_4_ID:
        bringup_count = &(sbtx_bringup_count[3]);
        assigned_bitwidth = &(sbtx_bitwidth[3]);
        reg = 0x3A6;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_5_ID:
        bringup_count = &(sbtx_bringup_count[4]);
        assigned_bitwidth = &(sbtx_bitwidth[4]);
        reg = 0x3A7;
        mask = 0x30;
        shift = 4;
        break;
      default:
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_SET_BITWIDTH. Invalid busport element passed.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
    }

    switch(bitwidth)
    {
      case 12:
        val = 0x3;
        break;
      case 16:
        val = 0x2;
        break;
      case 20:
        val = 0x1;
        break;
      case 24:
        val = 0x0;
        break;
      default:
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_SET_BITWIDTH. Invalid bit width passed.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
    }

    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    if ( (*bringup_count) != 0  && (*assigned_bitwidth != bitwidth) )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_SET_BITWIDTH. Busport already brought up at a different bit width.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if ( (*bringup_count) == 0 )
    {
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    }

    (*bringup_count)++;
    *assigned_bitwidth = bitwidth;

  } while(WCD_G_FALSE_BOOLEAN);

  return return_code;
}

wcd_result WCDH_BUSPORT_RESET_BITWIDTH(wcd_element* p_element)
{
  uint32* bringup_count = NULL;
  uint32* assigned_bitwidth = NULL;

  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = 0x0; //default 24 bit

  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    if (p_element == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_RESET_BITWIDTH. Busport element passed in is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    switch(p_element->id)
    {
      case WCD_SBRX_1_ID:
        bringup_count = &(sbrx_bringup_count[0]);
        assigned_bitwidth = &(sbrx_bitwidth[0]);
        reg = 0x3AE;
        mask = 0x03;
        shift = 0;
        break;
      case WCD_SBRX_2_ID:
        bringup_count = &(sbrx_bringup_count[1]);
        assigned_bitwidth = &(sbrx_bitwidth[1]);
        reg = 0x3AE;
        mask = 0x0C;
        shift = 2;
        break;
      case WCD_SBRX_3_ID:
        bringup_count = &(sbrx_bringup_count[2]);
        assigned_bitwidth = &(sbrx_bitwidth[2]);
        reg = 0x3AE;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBRX_4_ID:
        bringup_count = &(sbrx_bringup_count[3]);
        assigned_bitwidth = &(sbrx_bitwidth[3]);
        reg = 0x3AE;
        mask = 0xC0;
        shift = 6;
        break;
      case WCD_SBRX_5_ID:
        bringup_count = &(sbrx_bringup_count[4]);
        assigned_bitwidth = &(sbrx_bitwidth[4]);
        reg = 0x3AF;
        mask = 0x03;
        shift = 0;
        break;
      case WCD_SBTX_1_ID:
        bringup_count = &(sbtx_bringup_count[0]);
        assigned_bitwidth = &(sbtx_bitwidth[0]);
        reg = 0x3A3;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_2_ID:
        bringup_count = &(sbtx_bringup_count[1]);
        assigned_bitwidth = &(sbtx_bitwidth[1]);
        reg = 0x3A4;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_3_ID:
        bringup_count = &(sbtx_bringup_count[2]);
        assigned_bitwidth = &(sbtx_bitwidth[2]);
        reg = 0x3A5;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_4_ID:
        bringup_count = &(sbtx_bringup_count[3]);
        assigned_bitwidth = &(sbtx_bitwidth[3]);
        reg = 0x3A6;
        mask = 0x30;
        shift = 4;
        break;
      case WCD_SBTX_5_ID:
        bringup_count = &(sbtx_bringup_count[4]);
        assigned_bitwidth = &(sbtx_bitwidth[4]);
        reg = 0x3A7;
        mask = 0x30;
        shift = 4;
        break;
      default:
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_BUSPORT_RESET_BITWIDTH. Invalid busport element passed.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
    }

    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    if (*bringup_count != 0)
    {
      (*bringup_count)--;
    }

    if ( (*bringup_count) == 0 )
    {
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
      *assigned_bitwidth = 24;
    }


  } while(WCD_G_FALSE_BOOLEAN);

  return return_code;
}



/**********************************************************************************
 *******************                    DMIC                    *******************
 *********************************************************************************/

wcd_result WCDH_DMIC_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //set the DMIC's clock mode and also the TX path's clock mode
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = 0x0;
  wcd_element* tx_path_element = NULL;
  wcd_list_element* current;

  if(p_element == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF DMIC_SET_CLOCK_MODE. DMIC Set clock element passed in is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //first lets find the tx_path
  if (p_element_list != NULL)
  {
    current = p_element_list->head;

    while(current != NULL)
    {
      if ( ((wcd_element*)(current->data))->type == WCD_TX_PATH_TYPE_ID )
      {
        tx_path_element = (wcd_element*)(current->data);
        break;
      }

      current = current->next;
    }
  }

  if (tx_path_element == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF DMIC_SET_CLOCK_MODE. Could not find TX path element.>\n");
  }

  switch(p_element->id)
  {
    case WCD_DMIC_1_ID:
    case WCD_DMIC_2_ID:
      reg = 0x304;
      mask = 0x0E;
      shift = 1;
      break;
    case WCD_DMIC_3_ID:
    case WCD_DMIC_4_ID:
      reg = 0x304;
      mask = 0xE0;
      shift = 5;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF DMIC_SET_CLOCK_MODE. Invalid DMIC ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  /*
    <parameter name="DIV_2" value="1" />
    <parameter name="DIV_3" value="2" />
    <parameter name="DIV_4" value="3" />
    <parameter name="DIV_6" value="4" />
    <parameter name="DIV_16" value="5" />
  */
  switch(parameter)
  {
    case 1:
      val = 0x0;
      break;
    case 2:
      val = 0x1;
      break;
    case 3:
      val = 0x2;
      break;
    case 4:
      val = 0x3;
      break;
    case 5:
      val = 0x4;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF DMIC_SET_CLOCK_MODE. Invalid parameter.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //first write the DMIC clock division
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  //if no tx path element (for ANC) return
  if (tx_path_element == NULL)
  {
    return return_code;
  }

  switch(tx_path_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x225;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x22D;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x235;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x23D;
      break;
    default:
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  mask = 0x07;
  shift = 0;

  //write TX path clock division
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}
uint32 dmic_reference_count[WCD_DMIC_COUNT] = { 0 };


wcd_result WCDH_DMIC_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32* reference_count = NULL;
  uint32 reg = 0x0;
  uint32 dmic_tlmm_reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_DMIC_ENABLE;


  switch(p_element->id)
  {
    case WCD_DMIC_1_ID:
    case WCD_DMIC_2_ID:
      reference_count = &(dmic_reference_count[0]);
      dmic_tlmm_reg = 0xC1;
      reg = 0x304;
      mask = 0x1;
      shift = 0;
      break;
    case WCD_DMIC_3_ID:
    case WCD_DMIC_4_ID:
      reference_count = &(dmic_reference_count[1]);
      dmic_tlmm_reg = 0xC3;
      reg = 0x304;
      mask = 0x10;
      shift = 4;
      break;
  }

  if (reference_count == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_DMIC_ENABLE. Invalid parameter.>\n");
    return_code = E_WCD_INVALID_PARAMETER_ERROR;
    return return_code;
  }

  if((*reference_count) == 0)
  {
    //set TLMM to operational mode/DMIC_CLK mode
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)dmic_tlmm_reg, 0x1F, 0, 0x0);
    return_code |= wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  (*reference_count)++;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_DMIC_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32* reference_count = NULL;
  uint32 dmic_tlmm_reg = 0x0;
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_DMIC_DISABLE;



  switch(p_element->id)
  {
    case WCD_DMIC_1_ID:
    case WCD_DMIC_2_ID:
      reference_count = &(dmic_reference_count[0]);
      dmic_tlmm_reg = 0xC1;
      reg = 0x304;
      mask = 0x1;
      shift = 0;
      break;
    case WCD_DMIC_3_ID:
    case WCD_DMIC_4_ID:
      reference_count = &(dmic_reference_count[1]);
      dmic_tlmm_reg = 0xC3;
      reg = 0x304;
      mask = 0x10;
      shift = 4;
      break;
  }

  if (reference_count == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_DMIC_DISABLE. Invalid parameter.>\n");
    return_code = E_WCD_INVALID_PARAMETER_ERROR;
    return return_code;
  }

  if (*reference_count != 0)
  {
    (*reference_count)--;
  }

  if (*reference_count == 0)
  {
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

    //set TLMM to override mode/PIN_CTL mode
    return_code |= wcd_register_write(E_WCD_CODEC_MAIN, (uint16)dmic_tlmm_reg, 0x1F, 0, 0x4);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}



/**********************************************************************************
 *******************                     ADC                    *******************
 *********************************************************************************/

#define ADC_GAIN_POS_0_DB     0x0
#define ADC_GAIN_POS_28P5_DB  0x13

#define ADC_MIN_ANALOG_GAIN  ADC_GAIN_POS_0_DB
#define ADC_MAX_ANALOG_GAIN  ADC_GAIN_POS_28P5_DB

static uint32 adc_reference_count[WCD_ADC_COUNT] = { 0 };

wcd_result WCDH_ADC_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 *reference_count = NULL;
  uint32 reg_txfe = 0x0;
  uint32 mask_txfe = 0x0;
  uint32 shift_txfe = 0x0;
  uint32 val = 0x0;
  uint32 reg_ch = 0x0;
  uint32 mask_ch = 0x80;
  uint32 shift_ch = 0x7;

  switch(p_element->id)
  {
    case WCD_ADC_1_ID:
      reg_txfe = 0x158;
      mask_txfe = 0x80;
      shift_txfe = 7;
      reference_count = &(adc_reference_count[0]);
      reg_ch = 0x153;
      break;
    case WCD_ADC_2_ID:
      reg_txfe = 0x158;
      mask_txfe = 0x40;
      shift_txfe = 6;
      reference_count = &(adc_reference_count[1]);
      reg_ch = 0x154;
      break;
    case WCD_ADC_3_ID:
      reg_txfe = 0x158;
      mask_txfe = 0x40;
      shift_txfe = 6;
      reference_count = &(adc_reference_count[2]);
      reg_ch = 0x15D;
      break;
    case WCD_ADC_4_ID:
      reg_txfe = 0x16C;
      mask_txfe = 0x80;
      shift_txfe = 7;
      reference_count = &(adc_reference_count[3]);
      reg_ch = 0x167;
      break;
    case WCD_ADC_5_ID:
      reg_txfe = 0x16C;
      mask_txfe = 0x40;
      shift_txfe = 6;
      reference_count = &(adc_reference_count[4]);
      reg_ch = 0x168;
      break;

    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_ADC_ENABLE. Invalid ADC ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if(*reference_count == 0)
  {
    val = WCD_ADC_ENABLE_TXFE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg_txfe, (uint8)mask_txfe, (uint8)shift_txfe, (uint8)val);
    if (return_code != E_WCD_SUCCESS)
    {
      return return_code;
    }

    val = WCD_ADC_ENABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg_ch, (uint8)mask_ch, (uint8)shift_ch, (uint8)val);
    if (return_code != E_WCD_SUCCESS)
    {
      return return_code;
    }

    val = WCD_ADC_DISABLE_TXFE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg_txfe, (uint8)mask_txfe, (uint8)shift_txfe, (uint8)val);
  }

  (*reference_count)++;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_ADC_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 *reference_count = NULL;

  uint32 reg = 0x0;
  uint32 mask = 0x80;
  uint32 shift = 0x7;
  uint32 val = 0x0;



  switch(p_element->id)
  {
    case WCD_ADC_1_ID:
      reg = 0x153;
      reference_count = &(adc_reference_count[0]);
      break;
    case WCD_ADC_2_ID:
      reg = 0x154;
      reference_count = &(adc_reference_count[1]);
      break;
    case WCD_ADC_3_ID:
      reg = 0x15D;
      reference_count = &(adc_reference_count[2]);
      break;
    case WCD_ADC_4_ID:
      reg = 0x167;
      reference_count = &(adc_reference_count[3]);
      break;
    case WCD_ADC_5_ID:
      reg = 0x168;
      reference_count = &(adc_reference_count[4]);
      break;

    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_ADC_DISABLE. Invalid ADC ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if (*reference_count != 0)
  {
    (*reference_count)--;
  }

  if ((*reference_count) == 0)
  {
    val = WCD_ADC_DISABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_ADC_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32 reg   = 0x0;
  uint32 mask  = 0x7C;
  uint32 shift = 0x2;
  uint32 val = 0x0;

  if (parameter > ADC_MAX_ANALOG_GAIN /*|| parameter < ADC_MIN_ANALOG_GAIN*/)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_ADC_SET_ANALOG_GAIN. Invalid Analog Gain Value.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  switch(p_element->id)
  {
    case WCD_ADC_1_ID:
      reg = 0x153;
      break;
    case WCD_ADC_2_ID:
      reg = 0x154;
      break;
    case WCD_ADC_3_ID:
      reg = 0x15D;
      break;
    case WCD_ADC_4_ID:
      reg = 0x167;
      break;
    case WCD_ADC_5_ID:
      reg = 0x168;
      break;

    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_ADC_SET_ANALOG_GAIN. Invalid ADC ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code == E_WCD_SUCCESS)
  {
    val = (parameter-1);
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);
  return return_code;
}


/**********************************************************************************
 *******************                  TX PATHS                  *******************
 *********************************************************************************/

wcd_result WCDH_TX_SET_SAMPLE_RATE(wcd_element* p_element, uint32 sample_rate, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0x7;
  uint32 shift = 0x0;
  uint32 val = 0x0;

  if(p_element == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_SET_SAMPLE_RATE. Set sample rate element passed in is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  switch(p_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x224;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x22C;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x234;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x23C;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_SET_SAMPLE_RATE. Invalid TX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  switch(sample_rate)
  {
    case 8000:
      val = 0x0;
      break;
    case 16000:
      val = 0x1;
      break;
    case 32000:
      val = 0x2;
      break;
    case 48000:
      val = 0x3;
      break;
    /* 96Khz sample rate supported on wcd9306 but not supported on wcd9302.
       Validity of sample rate for a given codec is checked in ioctl manager */
    case 96000:
      val = 0x4;
      break;
    /* 192Khz sample rate supported on wcd9306 but not supported on wcd9302.
        Validity of sample rate for a given codec is checked in ioctl manager */
    case 192000:
      val = 0x5;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_SET_SAMPLE_RATE. Invalid sample rate.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if(return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_TX_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_PATH_ENABLE;

  wcd_list_element* iterator;

  if(context_ptr == NULL || context_ptr->busport_elements == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_PATH_ENABLE. Context pointer passed in is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //set bitwidth on busports
  iterator = context_ptr->busport_elements->head;
  while(iterator != NULL && return_code == E_WCD_SUCCESS)
  {
    return_code = WCDH_BUSPORT_SET_BITWIDTH( (wcd_element*)iterator->data, context_ptr->bit_width );

    iterator = iterator->next;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  switch(p_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x30A;
      mask = 0x01;
      shift = 0;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x30A;
      mask = 0x02;
      shift = 1;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x30A;
      mask = 0x04;
      shift = 2;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x30A;
      mask = 0x08;
      shift = 3;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_TX_PATH_ENABLE. Invalid TX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = WCDH_TX_SET_SAMPLE_RATE(p_element, context_ptr->sample_rate, p_element_list, context_ptr);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_TX_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_PATH_DISABLE;

  wcd_list_element* iterator = NULL;

  //reset sample rate to default 48000khz
  return_code = WCDH_TX_SET_SAMPLE_RATE(p_element, 48000, p_element_list, context_ptr);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  switch(p_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x30A;
      mask = 0x01;
      shift = 0;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x30A;
      mask = 0x02;
      shift = 1;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x30A;
      mask = 0x04;
      shift = 2;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x30A;
      mask = 0x08;
      shift = 3;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_TX_PATH_DISABLE. Invalid TX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //reset bus port bit widths
  iterator = context_ptr->busport_elements->head;
  while(iterator != NULL && return_code == E_WCD_SUCCESS)
  {
    return_code = WCDH_BUSPORT_RESET_BITWIDTH( (wcd_element*)iterator->data );

    iterator = iterator->next;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_TX_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = 0x1;

  switch(p_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x302;
      mask = 0x01;
      shift = 0;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x302;
      mask = 0x02;
      shift = 1;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x302;
      mask = 0x04;
      shift = 2;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x302;
      mask = 0x08;
      shift = 3;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_RESET. Invalid TX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  val = WCD_PATH_RESET_ENABLE;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_SLEEP(WCD_RESET_DELAY_MS);

  val = WCD_PATH_RESET_DISABLE;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_TX_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0xFF;
  uint32 shift = 0x0;
  uint32 val = 0;

  switch(p_element->id)
  {
    case WCD_TX_PATH_1_ID:
      reg = 0x221;
      break;
    case WCD_TX_PATH_2_ID:
      reg = 0x229;
      break;
    case WCD_TX_PATH_3_ID:
      reg = 0x231;
      break;
    case WCD_TX_PATH_4_ID:
      reg = 0x239;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_SET_GAIN. Invalid TX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if ((int32)parameter > WCD_MAX_DB_GAIN || (int32)parameter < WCD_MIN_DB_GAIN)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF TX_GAIN. Invalid parameter.>\n");
    return_code = E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  val = (uint8)parameter;
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

/**********************************************************************************
 *******************                  MIC BIAS                  *******************
 *********************************************************************************/

/* Max micbias count & cfilt count are chosen as per wcd9306
   so that common micbias & cfilt array can be used for both */
#define MICB_COUNT 3
#define CFILT_COUNT 3

#define WCD9306_MICB_COUNT 3
#define WCD9302_MICB_COUNT 2

#define WCD9306_CFILT_COUNT 3
#define WCD9302_CFILT_COUNT 2

#define MICB_MAX_K_VALUE  44
#define MICB_MIN_K_VALUE  4

uint8 micbias_count = WCD9306_MICB_COUNT;
uint8 micbias_cfilt_count = WCD9306_CFILT_COUNT;

/*
 * Bringup count of the micbias
 */
uint32 micb_bringup_count[MICB_COUNT] = { 0 };

/*
 * CFILT selection for each micbias
 */
uint32 micb_cfilt_selection[MICB_COUNT] = { 0 };

/*
 * External Bypass Capacitor mode for each micbias
 * Note: Opposite from register.
 * 0 = No external bypass capacitor, 1 = external bypass capacitor
 */
uint32 micb_bypass_capacitor[MICB_COUNT] = { 0 };

/*
 * Bringup count of the CFILT
 */
uint32 cfilt_bringup_count[CFILT_COUNT] = { 0 };

/*
 * K value for each CFILT
 */
uint32 cfilt_k_value[CFILT_COUNT] = { 0 };

/*
 * The current micbias configuration is applied to
 */
uint32 current_micb = 0;

/*
 * The settings that will need to be applied to the above arrays when enable micbias is actually called
 */
uint32 current_micb_cfilt_selection = 0;
uint32 current_micb_bypass_capacitor = 0;
uint32 current_micb_kvalue = 0;

wcd_result WCDH_MBIAS_MGR_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (parameter > micbias_count || parameter <= 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_SELECT_MICBIAS. Invalid micbias parameter.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  current_micb = parameter;
  current_micb_cfilt_selection = 0;     //default to no cfilter selection to force users to select one
  current_micb_bypass_capacitor = 0;    //default to no external bypass capacitor
  current_micb_kvalue = 36;             //default to 36 for default 1.8V

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (parameter > micbias_cfilt_count || parameter <= 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_SELECT_CFILTER. Invalid CFILT parameter.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_SELECT_CFILTER. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  current_micb_cfilt_selection = parameter;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0;
  uint32 val = 0x0;


 
  WCD_DEBUG_ERROR_0("Custom Message : Inside WCDH_MBIAS_MGR_ENABLE  fn\n");   //added a print message.

  //micbias needs to be selected
  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Micbias not selected. Assuming it has already been enabled>\n");
    return E_WCD_SUCCESS;
  }

  //cfilter needs to be selected
  if (current_micb_cfilt_selection <= 0 || current_micb_cfilt_selection > micbias_cfilt_count )
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Invalid CFILTER.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //check if micbias is already enabled and all the parameters match
  if (micb_bringup_count[current_micb - 1] > 0)
  {
    if (micb_cfilt_selection[current_micb - 1] != current_micb_cfilt_selection)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Micbias already enabled and CFILT does not match.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
    }

    if (micb_bypass_capacitor[current_micb - 1] != current_micb_bypass_capacitor)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Micbias already enabled and EXTERNAL BYPASS CAP does not match.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
    }

    /*
       For special headsets, MBHC sets Micbias at 2.7V and
       device profile updates to 1.8V so in this scenrio it
       is not an error
    */
    if (cfilt_k_value[current_micb_cfilt_selection - 1] != current_micb_kvalue && (2 != current_micb))
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. CFILT already enabled and K VALUE does not match.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }

  //save everything
  micb_cfilt_selection[current_micb - 1] = current_micb_cfilt_selection;
  micb_bypass_capacitor[current_micb - 1] = current_micb_bypass_capacitor;
  cfilt_k_value[current_micb_cfilt_selection - 1] = current_micb_kvalue;

  //write registers now (first micbias then CFILT)
  if (micb_bringup_count[current_micb - 1] == 0)
  {
	  WCD_DEBUG_ERROR_0("Custom Message : MicBias going to be selected   fn\n");   //added a print message.
    switch(current_micb)
    {
      case 1:
        reg = 0x12B;
		WCD_DEBUG_ERROR_0("Custom Message : MicBias selected 0x12B  fn\n");   //added a print message.
		
        break;
      case 2:
        reg = 0x131;
		
		
        break;
      case 3:
        reg = 0x137;
		

        break;
      default:
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Invalid MICBIAS.>\n");
        return E_WCD_INVALID_PARAMETER_ERROR;
    }

    //external bypass capacitor
    switch(micb_bypass_capacitor[current_micb - 1])
    {
      case 0:       //no external bypass cap
        val = 0x1;
		
		
        break;
      case 1:       //external bypass cap
        val = 0x0;
		
		
        break;
    }

    mask = 0x10;
    shift = 4;
    wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

    if (cfilt_bringup_count[current_micb_cfilt_selection - 1] == 0)
    {
      switch(current_micb_cfilt_selection)
      {
        case 1:
          reg = 0x129;
          break;
        case 2:
          reg = 0x12F;
#ifdef WCD_MBHC_ENABLE
          goto SkipSetCfiltKFactor;
#else
          break;
#endif
        case 3:
          reg = 0x135;
          break;
        default:
          WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Invalid CFILT.>\n");
          return E_WCD_INVALID_PARAMETER_ERROR;
      }

      //k value (K - 4...)
      val = cfilt_k_value[current_micb_cfilt_selection - 1] - 4;
      mask = 0xFC;
      shift = 2;

      wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    }
#ifdef WCD_MBHC_ENABLE
   SkipSetCfiltKFactor:
#endif
    // Set CFILT in Slow mode for Headset Recording
    if (cfilt_bringup_count[current_micb_cfilt_selection - 1] == 0)
    {
      //enable CFILT
      switch(current_micb_cfilt_selection)
      {
        case 1:
          reg = 0x128;
          goto SkipSetCfiltMode;
        case 2:
          reg = 0x12E;
          break;
        case 3:
          reg = 0x134;
          goto SkipSetCfiltMode;
      }

      mask = 0x40;
      shift = 6;
      val = 0x1;
      wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    }

  }

SkipSetCfiltMode:

  //only if the micbias being brought up and the cfilt have not already been brought up
  if ((micb_bringup_count[current_micb - 1] + cfilt_bringup_count[current_micb_cfilt_selection - 1]) == 0)
  {
    //enable LDO with default 2.35V
    reg = 0x110;
    mask = 0x80;
    shift = 7;
    val = 0x1;
    wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

	WCD_DEBUG_ERROR_0("Critical Custom Message : Enabled LDO");   //added a print message.

	wcd_bus_flush(E_WCD_CODEC_MAIN);
    //DELAY 1ms after enabling LDO
    WCD_SLEEP(1);
  }

  if (micb_bringup_count[current_micb - 1] == 0)
  {
    //enable MICBIAS
    switch(current_micb)
    {
      case 1:
        reg = 0x12B;
        break;
      case 2:
        reg = 0x131;
        break;
      case 3:
        reg = 0x137;
        break;
      default:
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_ENABLE. Invalid MICBIAS.>\n");
        return E_WCD_INVALID_PARAMETER_ERROR;
    }
    mask = 0x80;
    shift = 7;
    val = 0x1;

    WCDH_HS_MBIAS_OPA_ENABLE((uint16)reg, (uint8)mask, (uint8)shift, (uint8)val, WCD_TRUE, E_WCD_CORE);

  }

  WCD_SLEEP(10);

  //increment bringup count
  micb_bringup_count[current_micb - 1]++;
  cfilt_bringup_count[current_micb_cfilt_selection - 1]++;

  //reset the current micbias and all settings
  current_micb = 0;
  current_micb_cfilt_selection = 0;
  current_micb_bypass_capacitor = 0;
  current_micb_kvalue = 0;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_HS_MBIAS_OPA_ENABLE(uint16 reg, uint8 mask, uint8 shift, uint8 val, wcd_bool enable, wcd_owner_module owner)
{
    wcd_result rc = E_WCD_SUCCESS;

    // If this function is called from WCD CORE:
    //    1. Update the status of HS_MBIAS_OPA_EN
    //    2. Always execute the register write

    // If this function is called from MBHC:
    //    1. Always execute the register write if enable == WCD_FALSE
    //    2. Execute the register write ONLY IF HS_MBIAS_OPA_EN flag is TRUE
    //    3. Otherwise, just return from the function without executing the register write

    if (owner == E_WCD_CORE)
    {
      HS_MBIAS_OPA_EN = enable;
    }
    else if (owner == E_WCD_MBHC)
    {
      if ( (enable == WCD_TRUE) && (HS_MBIAS_OPA_EN == WCD_FALSE) )
      {
        WCD_DEBUG_INFO_3("=WCD_93XX: <CF WCDH_HS_MBIAS_OPA_ENABLE, No need to update MicBias OPA En, owner=(%d), enable=(%u), HS_MBIAS_OPA_EN=(%u)> \n", owner, enable, HS_MBIAS_OPA_EN);
        return rc;
      }
    }

    rc = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    WCD_DEBUG_INFO_2("=WCD_93XX: <CF WCDH_HS_MBIAS_OPA_ENABLE, Updated MicBias Reg, owner=(%d), rc=(%d)> \n", owner, rc );
    WCD_DEBUG_INFO_2("=WCD_93XX: <CF WCDH_HS_MBIAS_OPA_ENABLE, Updated MicBias Reg, enable=(%u), HS_MBIAS_OPA_EN=(%u)> \n", enable, HS_MBIAS_OPA_EN );

    return rc;
}

wcd_result WCDH_MBIAS_MGR_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32 reg = 0x0;
  uint32 mask = 0x0;
  uint32 shift = 0;
  uint32 val = 0x0;
  uint32 current_cfilt;
  uint32 total_bringup_count = 0;
  uint32 iterator;

  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF MICBIAS_MGR_DISABLE. Micbias not selected. Assuming it has already been disabled.>\n");
    return E_WCD_SUCCESS;
  }

  current_cfilt = micb_cfilt_selection[current_micb - 1];

  if (current_cfilt <= 0 || current_cfilt > micbias_cfilt_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_DISABLE. Invalid CFILT selected by MICBIAS.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //decrement the bringup counts
  if (micb_bringup_count[current_micb - 1] != 0)
  {
    micb_bringup_count[current_micb - 1]--;
  }

  if (cfilt_bringup_count[current_cfilt - 1] != 0)
  {
    cfilt_bringup_count[current_cfilt - 1]--;
  }

  //disable if the bringup count for each is 0
  if (micb_bringup_count[current_micb - 1] == 0)
  {
    switch(current_micb)
    {
      case 1:
        reg = 0x12B;
        break;
      case 2:
        reg = 0x131;
        break;
      case 3:
        reg = 0x137;
        break;
    }
    mask = 0x80;
    shift = 7;
    val = 0x0;
    WCDH_HS_MBIAS_OPA_ENABLE((uint16)reg, (uint8)mask, (uint8)shift, (uint8)val, WCD_FALSE, E_WCD_CORE);

  }

  if (cfilt_bringup_count[current_cfilt - 1] == 0)
  {
    switch(current_micb_cfilt_selection)
    {
      case 1:
        reg = 0x128;
        break;
      case 2:
        reg = 0x12E;
        break;
      case 3:
        reg = 0x134;
        break;
    }
    mask = 0x80;
    shift = 7;
    val = 0x0;
    wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  //disable ldo if total micb bringup count and cfilt bringup count is 0
  for(iterator = 0; iterator < micbias_count; iterator++)
  {
    total_bringup_count += micb_bringup_count[iterator];
  }

  for(iterator = 0; iterator < micbias_cfilt_count; iterator++)
  {
    total_bringup_count += cfilt_bringup_count[iterator];
  }

  if (total_bringup_count == 0)
  {
	WCD_DEBUG_ERROR_0("Critical Custom Message : Disabled LDO");   //added a print message.
    reg = 0x110;
    mask = 0x80;
    shift = 7;
    val = 0x0;
    wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  //reset the current micbias and all settings
  current_micb = 0;
  current_micb_cfilt_selection = 0;
  current_micb_bypass_capacitor = 0;
  current_micb_kvalue = 0;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (parameter > MICB_MAX_K_VALUE || parameter < MICB_MIN_K_VALUE)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_APPLY_DESIRED_VOLTAGE. Invalid K Value.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_APPLY_DESIRED_VOLTAGE. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  current_micb_kvalue = parameter;

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAP. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  /*
   <parameter name="EXTERNAL_BYPASS_CAPACITOR" value="1" />
   <parameter name="NO_EXTERNAL_BYPASS_CAPACITOR" value="2" />
  */
  switch(parameter)
  {
    case 1:
      current_micb_bypass_capacitor = 1;
      break;
    case 2:
      current_micb_bypass_capacitor = 0;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF MICBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAP. Invalid parameter.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_SET_IB_RES(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint16 reg = 0x0;
  uint8 mask = 0x0;
  uint8 shift = 0;
  uint8 value = 0x0;
  wcd_element* adc_element;
  uint32 adc_id = 0;

  //we do not need to buffer internal biasing because each internal bias + TX FE is independent of each other
  //however we need to use the current_micb buffer to see which micbias to enable internal biasing for
  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (p_element_list == NULL || p_element_list->head == NULL || p_element_list->head->next == NULL ||
    p_element_list->head->next->data == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Route or elements or second element is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //first element in route's elements is micbias manager, next is adc!
  adc_element = (wcd_element*)p_element_list->head->next->data;

  switch(adc_element->id)
  {
    case WCD_ADC_1_ID:
      adc_id = 1;
      break;
    case WCD_ADC_2_ID:
      adc_id = 2;
      break;
    case WCD_ADC_3_ID:
      adc_id = 3;
      break;
    case WCD_ADC_4_ID:
      adc_id = 4;
      break;
    /* Note: Though ADC_5 is present on wcd9302 but it is not connected to decimator.
       So, it is only valid for wcd9306. Validity of ADC is checked in ioctl manager */
    case WCD_ADC_5_ID:
      adc_id = 5;
      break;
  }

  if (adc_id == 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Internal micbias only applicable to ADCs.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  /*
  <parameter name="1.1KOHM" value="1" />
  <parameter name="2.2KOHM" value="2" />
  */
  switch ( parameter )
  {
    case 1:
      value = 0x1;
      break;
    case 2:
      value = 0x0;
      break;
  }

  //figure out register based on micbias
  //figure out mask + shift based on adc
  if ( current_micb == 1)
  {
    reg = 0x12C;

    //MICB 1 - internal biasing to ADC1 or ADC3
    if ( adc_id == 1 )
    {
      mask = 0x40;
      shift = 6;
    }
    else if ( adc_id == 3 )
    {
      mask = 0x08;
      shift = 3;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Micbias 1 can only conect to ADC 1 and ADC 3. ADC %u is currently connected to Micbias 1.>\n", adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 2)
  {
    reg = 0x132;

    //MICB 2 - internal biasing to ADC2 or ADC4 or ADC5
    if ( adc_id == 2 )
    {
      mask = 0x40;
      shift = 6;
    }
    else if ( adc_id == 4 )
    {
      mask = 0x08;
      shift = 3;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x01;
      shift = 0;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Micbias 2 can only conect to ADC 2, ADC 3, and ADC 4. ADC %u is currently connected to Micbias 2.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 3)
  {
    reg = 0x138;

    //MICB 3 - internal biasing to ADC4 or ADC5
    if ( adc_id == 4 )
    {
      mask = 0x40;
      shift = 6;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x08;
      shift = 3;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Micbias 3 can only conect to ADC 4 and ADC 5. ADC %u is currently connected to Micbias 3.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }

  if ( mask == 0 || reg == 0 )
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_SET_IB_RES. Register or mask was not set.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;

}

wcd_result WCDH_MBIAS_MGR_ENABLE_IB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint16 reg = 0x0;
  uint8 mask = 0x0;
  uint8 shift = 0;
  uint8 value;
  wcd_element* adc_element;
  uint32 adc_id = 0;

  //we do not need to buffer internal biasing because each internal bias + TX FE is independent of each other
  //however we need to use the current_micb buffer to see which micbias to enable internal biasing for
  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (p_element_list == NULL || p_element_list->head == NULL || p_element_list->head->next == NULL ||
    p_element_list->head->next->data == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Route or elements or second element is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //first element in route's elements is micbias manager, next is adc!
  adc_element = (wcd_element*)p_element_list->head->next->data;

  switch(adc_element->id)
  {
    case WCD_ADC_1_ID:
      adc_id = 1;
      break;
    case WCD_ADC_2_ID:
      adc_id = 2;
      break;
    case WCD_ADC_3_ID:
      adc_id = 3;
      break;
    case WCD_ADC_4_ID:
      adc_id = 4;
      break;
    /* Note: Though ADC_5 is present on wcd9302 but it is not connected to decimator.
       So, it is only valid for wcd9306. Validity of ADC is checked in ioctl manager */
    case WCD_ADC_5_ID:
      adc_id = 5;
      break;
   }

  if (adc_id == 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Internal micbias only applicable to ADCs.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //ENABLE value = 0x1
  value = 0x1;

  //figure out register based on micbias
  //figure out mask + shift based on adc
  if ( current_micb == 1)
  {
    reg = 0x12C;

    //MICB 1 - internal biasing to ADC1 or ADC3
    if ( adc_id == 1 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 3 )
    {
      mask = 0x10;
      shift = 4;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Micbias 1 can only conect to ADC 1 and ADC 3. ADC %u is currently connected to Micbias 1.>\n", adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 2)
  {
    reg = 0x132;

    //MICB 2 - internal biasing to ADC2 or ADC4 or ADC5
    if ( adc_id == 2 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 4 )
    {
      mask = 0x10;
      shift = 4;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x02;
      shift = 1;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Micbias 2 can only conect to ADC 2, ADC 3, and ADC 4. ADC %u is currently connected to Micbias 2.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 3)
  {
    reg = 0x138;

    //MICB 3 - internal biasing to ADC4 or ADC5
    if ( adc_id == 4 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x10;
      shift = 4;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Micbias 3 can only conect to ADC 3 and ADC 5. ADC %u is currently connected to Micbias 3.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }

  if ( mask == 0 || reg == 0 )
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_ENABLE_IB. Register or mask was not set.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_DISABLE_IB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint16 reg = 0x0;
  uint8 mask = 0x0;
  uint8 shift = 0;
  uint8 value;
  wcd_element* adc_element;
  uint32 adc_id = 0;

  //we do not need to buffer internal biasing because each internal bias + TX FE is independent of each other
  //however we need to use the current_micb buffer to see which micbias to enable internal biasing for
  if (current_micb <= 0 || current_micb > micbias_count)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Micbias not selected.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (p_element_list == NULL || p_element_list->head == NULL || p_element_list->head->next == NULL ||
    p_element_list->head->next->data == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Route or elements or second element is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }


  //first element in route's elements is micbias manager, next is adc!
  adc_element = (wcd_element*)p_element_list->head->next->data;

  switch(adc_element->id)
  {
    case WCD_ADC_1_ID:
      adc_id = 1;
      break;
    case WCD_ADC_2_ID:
      adc_id = 2;
      break;
    case WCD_ADC_3_ID:
      adc_id = 3;
      break;
    case WCD_ADC_4_ID:
      adc_id = 4;
      break;
    /* Note: Though ADC_5 is present on wcd9302 but it is not connected to decimator.
       So, it is only valid for wcd9306. Validity of ADC is checked in ioctl manager */
    case WCD_ADC_5_ID:
      adc_id = 5;
      break;
  }

  if (adc_id == 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Internal micbias only applicable to ADCs.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  //DISABLE value = 0x0
  value = 0x0;

  //figure out register based on micbias
  //figure out mask + shift based on adc
  if ( current_micb == 1)
  {
    reg = 0x12C;

    //MICB 1 - internal biasing to ADC1 or ADC3
    if ( adc_id == 1 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 3 )
    {
      mask = 0x10;
      shift = 4;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Micbias 1 can only conect to ADC 1 and ADC 3. ADC %u is currently connected to Micbias 1.>\n", adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 2)
  {
    reg = 0x132;

    //MICB 2 - internal biasing to ADC2 or ADC4 or ADC5
    if ( adc_id == 2 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 4 )
    {
      mask = 0x10;
      shift = 4;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x02;
      shift = 1;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Micbias 2 can only conect to ADC 2, ADC 3, and ADC 4. ADC %u is currently connected to Micbias 2.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }
  else if ( current_micb == 3)
  {
    reg = 0x138;

    //MICB 3 - internal biasing to ADC4 or ADC5
    if ( adc_id == 4 )
    {
      mask = 0x80;
      shift = 7;
    }
    else if ( adc_id == 5 )
    {
      mask = 0x10;
      shift = 4;
    }
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Micbias 3 can only conect to ADC 3 and ADC 5. ADC %u is currently connected to Micbias 3.>\n", (unsigned int)adc_id);
      return E_WCD_INVALID_PARAMETER_ERROR;
    }
  }

  if ( mask == 0 || reg == 0 )
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_MBIAS_MGR_DISABLE_IB. Register or mask was not set.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, value);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_MBIAS_MGR_CONFIG_MAD(wcd_configure_mad* config)
{
  HANDLE_UNUSED_PARAMETER(config);

  return E_WCD_SUCCESS;
}
wcd_result WCDH_MBIAS_MGR_DISABLE_MAD()
{
  return E_WCD_SUCCESS;
}
wcd_result WCDH_MBIAS_MGR_SET_IB_RES_MAD(wcd_configure_mad* config)
{
  HANDLE_UNUSED_PARAMETER(config);
  return E_WCD_SUCCESS;
}
/**********************************************************************************
 *******************                  RX PATHS                  *******************
 *********************************************************************************/

wcd_result WCDH_RX_SET_SAMPLE_RATE(wcd_element* p_element, uint32 sample_rate, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0xE0;
  uint32 shift = 5;
  uint32 val = 0x0;

  switch(p_element->id)
  {
    case WCD_RX_1_PATH_ID:
      reg = 0x2B4;
      break;
    case WCD_RX_2_PATH_ID:
      reg = 0x2BC;
      break;
    case WCD_RX_3_PATH_ID:
      reg = 0x2C4;
      break;
    case WCD_RX_4_PATH_ID:
      reg = 0x2CC;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_SET_SAMPLE_RATE. Invalid RX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  /**
    <parameter name="8KHZ" value="1" />
    <parameter name="16KHZ" value="2" />
    <parameter name="32KHZ" value="3" />
    <parameter name="48KHZ" value="4" />
    <parameter name="96KHZ" value="5" />
    <parameter name="192KHZ" value="6" />
  **/
  switch(sample_rate)
  {
    case 8000:
      val = 0x0;
      break;
    case 16000:
      val = 0x1;
      break;
    case 32000:
      val = 0x2;
      break;
    case 48000:
      val = 0x3;
      break;
    /* 96Khz sample rate supported on wcd9306 but not supported on wcd9302.
       Validity of sample rate for a given codec is checked in ioctl manager */
    case 96000:
      val = 0x4;
      break;
    /* 192Khz sample rate supported on wcd9306 but not supported on wcd9302.
       Validity of sample rate for a given codec is checked in ioctl manager */
    case 192000:
      val = 0x5;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_SET_SAMPLE_RATE. Invalid sample rate.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}


uint32 rx_path_reference_count[WCD_RX_PATH_COUNT] = { 0 };

wcd_result WCDH_RX_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x30F;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_PATH_ENABLE;
  uint32* reference_count = NULL;

  wcd_list_element* iterator;

  //set bitwidth on busports
  iterator = context_ptr->busport_elements->head;
  while(iterator != NULL && return_code == E_WCD_SUCCESS)
  {
    return_code = WCDH_BUSPORT_SET_BITWIDTH( (wcd_element*)iterator->data, context_ptr->bit_width );
    iterator = iterator->next;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  switch(p_element->id)
  {
    case WCD_RX_1_PATH_ID:
      mask = 0x01;
      shift = 0;
      reference_count = &(rx_path_reference_count[0]);
      break;
    case WCD_RX_2_PATH_ID:
      mask = 0x02;
      shift = 1;
      reference_count = &(rx_path_reference_count[1]);
      break;
    case WCD_RX_3_PATH_ID:
      mask = 0x04;
      shift = 2;
      reference_count = &(rx_path_reference_count[2]);
      break;
    case WCD_RX_4_PATH_ID:
      mask = 0x08;
      shift = 3;
      reference_count = &(rx_path_reference_count[3]);
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_RESET. Invalid RX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if (*reference_count == 0)
  {
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    if (return_code != E_WCD_SUCCESS)
    {
      return return_code;
    }
    return_code = WCDH_RX_SET_SAMPLE_RATE(p_element, context_ptr->sample_rate, p_element_list, context_ptr);
  }

  if (return_code == E_WCD_SUCCESS)
  {
    (*reference_count)++;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  //shadow_register[0x30F] = 0x08;
 // wcd_register_write(E_WCD_CODEC_MAIN, 0x30F, 0x8, 0, 0x8);
 // WCD_DEBUG_ERROR_0("custom message 0X30F = 0X08\n");

  return return_code;
}

wcd_result WCDH_RX_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x30F;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = WCD_PATH_DISABLE;
  uint32* reference_count = NULL;

  wcd_list_element* iterator = NULL;

  switch(p_element->id)
  {
    case WCD_RX_1_PATH_ID:
      mask = 0x01;
      shift = 0;
      reference_count = &(rx_path_reference_count[0]);
      break;
    case WCD_RX_2_PATH_ID:
      mask = 0x02;
      shift = 1;
      reference_count = &(rx_path_reference_count[1]);
      break;
    case WCD_RX_3_PATH_ID:
      mask = 0x04;
      shift = 2;
      reference_count = &(rx_path_reference_count[2]);
      break;
    case WCD_RX_4_PATH_ID:
      mask = 0x08;
      shift = 3;
      reference_count = &(rx_path_reference_count[3]);
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_RESET. Invalid RX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if (*reference_count != 0)
  {
    (*reference_count)--;
  }

  if (*reference_count == 0)
  {
    //reset sample rate to default 48000khz
    return_code = WCDH_RX_SET_SAMPLE_RATE(p_element, 48000, p_element_list, context_ptr);
    if (return_code != E_WCD_SUCCESS)
    {
      return return_code;
    }

    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //reset bus port bit widths
  iterator = context_ptr->busport_elements->head;
  while(iterator != NULL && return_code == E_WCD_SUCCESS)
  {
    return_code = WCDH_BUSPORT_RESET_BITWIDTH( (wcd_element*)iterator->data );

    iterator = iterator->next;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_RX_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x301;
  uint32 mask = 0x0;
  uint32 shift = 0x0;
  uint32 val = 0x1;
  uint32* reference_count = NULL;

  switch(p_element->id)
  {
    case WCD_RX_1_PATH_ID:
      mask = 0x01;
      shift = 0;
      reference_count = &(rx_path_reference_count[0]);
      break;
    case WCD_RX_2_PATH_ID:
      mask = 0x02;
      shift = 1;
      reference_count = &(rx_path_reference_count[1]);
      break;
    case WCD_RX_3_PATH_ID:
      mask = 0x04;
      shift = 2;
      reference_count = &(rx_path_reference_count[2]);
      break;
    case WCD_RX_4_PATH_ID:
      mask = 0x08;
      shift = 3;
      reference_count = &(rx_path_reference_count[3]);
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_RESET. Invalid RX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if (*reference_count == 0)
  {
    val = WCD_PATH_RESET_ENABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(WCD_RESET_DELAY_MS);

    val = WCD_PATH_RESET_DISABLE;
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_RX_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reg = 0x0;
  uint32 mask = 0xFF;
  uint32 shift = 0x0;
  uint32 val = 0;

  switch(p_element->id)
  {
    case WCD_RX_1_PATH_ID:
      reg = 0x2B7;
      break;
    case WCD_RX_2_PATH_ID:
      reg = 0x2BF;
      break;
    case WCD_RX_3_PATH_ID:
      reg = 0x2C7;
      break;
    case WCD_RX_4_PATH_ID:
      reg = 0x2CF;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_SET_GAIN. Invalid RX Path ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if ((int32)parameter > WCD_MAX_DB_GAIN || (int32)parameter < WCD_MIN_DB_GAIN)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF RX_SET_GAIN. Invalid parameter.>\n");
    return_code = E_WCD_INVALID_PARAMETER_ERROR;
  }

  val = (uint8)parameter;

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

/**********************************************************************************
 *******************                 COMPANDER                  *******************
 *********************************************************************************/

uint32 compander_0_bringup_count = 0; /* for speaker path */
uint32 compander_1_bringup_count = 0; /* for headphone left and right path */
uint32 compander_2_bringup_count = 0; /* for LO & speaker path */

wcd_bool  hphl_in_compander_mode = WCD_FALSE;
wcd_bool  hphr_in_compander_mode = WCD_FALSE;
wcd_bool  lo1_in_compander_mode = WCD_FALSE;
wcd_bool  lo2_in_compander_mode = WCD_FALSE;
wcd_bool  spkr_in_compander_mode = WCD_FALSE;

wcd_result WCDH_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32* bringup_count;
  uint32 rx4_path_rdac_id = 0;
  wcd_list_element *iterator;

  if(p_element->id == WCD_RX_1_COMPANDER_ID || p_element->id == WCD_RX_2_COMPANDER_ID || p_element->id == WCD_RX_4_COMPANDER_ID)
  {
    //verify this is not earout because earout does not support compander
    if (p_element_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_COMPANDER_ENABLE. Route or elements in route is NULL.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
    }

    iterator = p_element_list->head;

    while(iterator != NULL)
    {
      if ( ((wcd_element*)(iterator->data))->id == WCD_LO2_R_M_ID )
      {
        rx4_path_rdac_id = WCD_LO2_R_M_ID;
        break;
      }
      if ( ((wcd_element*)(iterator->data))->id == WCD_SPEAKER_ID )
      {
        rx4_path_rdac_id = WCD_SPEAKER_ID;
        break;
      }
      if ( ((wcd_element*)(iterator->data))->id == WCD_EAROUT_ID )
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_COMPANDER_ENABLE. Compander not supported on EAROUT.>\n");
        return E_WCD_INVALID_PARAMETER_ERROR;
      }
      iterator = iterator->next;
    }
    //done verification
  }

  if(p_element->id == WCD_RX_1_COMPANDER_ID || p_element->id == WCD_RX_2_COMPANDER_ID)
  {
    //bring up count is on the compander clock
    bringup_count = &compander_1_bringup_count;

    if (*bringup_count == 0)
    {
      /*
      #CDC_COMP1_B2_CTL__PEAK_METER_TIME_OUT = 9
      0x371; 0x0F; 0x09
      #CDC_CLK_RX_B2_CTL__RX_COMP1_CLK_EN = ENABLE
      0x310; 0x02; 0x02
      #CDC_COMP1_B3_CTL__LEVEL_METER_RESAMPLE_RATE = 1
      0x372; 0x02; 0x02
      #CDC_COMP1_B2_CTL__LEVEL_METER_DIV_FACTOR = 5
      0x371; 0xF0; 0x50
      #Delay_mS 1
      #CDC_COMP1_B3_CTL__LEVEL_METER_RESAMPLE_RATE = 40
      0x372; 0xFF; 0x28
      #CDC_COMP1_B2_CTL__LEVEL_METER_DIV_FACTOR = 11
      0x371; 0xF0; 0xB0
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x371, 0x0F, 0, 0x09);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x02, 1, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x372, 0x02, 1, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x371, 0xF0, 0, 0x50);
      wcd_bus_flush(E_WCD_CODEC_MAIN);
      WCD_SLEEP(1);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x372, 0xFF, 0, 0x28);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x371, 0xF0, 0, 0xB0);
    }

    /* Compander#1 left channel */
    if (p_element->id == WCD_RX_1_COMPANDER_ID)
    {
      /*
      #CDC_COMP1_B1_CTL__CH1_EN = ENABLE
      0x370; 0x02; 0x02
      #RX_HPH_L_GAIN__GAIN_SOURCE_SEL = COMPANDER
      0x1AE; 0x20; 0x00
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x02, 1, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1AE, 0x20, 5, 0x00);

      hphl_in_compander_mode = WCD_TRUE;
    }
    /* Compander#1 right channel */
    else if (p_element->id == WCD_RX_2_COMPANDER_ID)
    {
      /*
      #CDC_COMP1_B1_CTL__CH2_EN = ENABLE
      0x370; 0x01; 0x01
      #RX_HPH_R_GAIN__GAIN_SOURCE_SEL = COMPANDER
      0x1B4; 0x20; 0x00
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x01, 0, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B4, 0x20, 5, 0x00);

      hphr_in_compander_mode = WCD_TRUE;
    }

    (*bringup_count)++;
  }

  if(p_element->id == WCD_RX_3_COMPANDER_ID)
  {
    bringup_count = &compander_2_bringup_count;

    //bring up count is on the compander clock
    if (*bringup_count == 0)
    {
      /*
      #CDC_COMP2_B2_CTL__PEAK_METER_TIME_OUT = 9
      0x379; 0x0F; 0x09
      #CDC_CLK_RX_B2_CTL__RX_COMP2_CLK_EN = ENABLE
      0x310; 0x04; 0x04
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x379, 0x0F, 0, 0x09);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x04, 2, 0x01);
    }

    /* Compander#2 left channel */
    /*
      #CDC_COMP2_B1_CTL__CH1_EN = ENABLE
      0x378; 0x02; 0x02
      #RX_LINE_1_GAIN__GAIN_SOURCE_SEL = COMPANDER
      0x1CD; 0x20; 0x00
      #RX_LINE_2_GAIN__GAIN_SOURCE_SEL = COMPANDER
      0x1D1; 0x20; 0x00
    */
     wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x02, 1, 0x01);
     wcd_register_write(E_WCD_CODEC_MAIN, 0x1CD, 0x20, 5, 0x00);
     wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x20, 5, 0x00);

     lo1_in_compander_mode = WCD_TRUE;
     lo2_in_compander_mode = WCD_TRUE;

     (*bringup_count)++;
  }

  if (p_element->id == WCD_RX_4_COMPANDER_ID)
  {
    /* Compander#2 right channel */
    if (rx4_path_rdac_id == WCD_LO2_R_M_ID)
    {
      bringup_count = &compander_2_bringup_count;

      //bring up count is on the compander clock
      if (*bringup_count == 0)
      {
        /*
        #CDC_COMP2_B2_CTL__PEAK_METER_TIME_OUT = 9
        0x379; 0x0F; 0x09
        #CDC_CLK_RX_B2_CTL__RX_COMP2_CLK_EN = ENABLE
        0x310; 0x04; 0x04
        */
        wcd_register_write(E_WCD_CODEC_MAIN, 0x379, 0x0F, 0, 0x09);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x04, 2, 0x01);
      }

      /*
        #CDC_COMP2_B1_CTL__CH2_EN = ENABLE
        0x378; 0x01; 0x01
        #RX_LINE_2_GAIN__GAIN_SOURCE_SEL = COMPANDER
        0x1D1; 0x20; 0x00
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x01, 0, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x20, 5, 0x00);

      lo2_in_compander_mode = WCD_TRUE;

      (*bringup_count)++;
    }
    /* Speaker Compander#0 left channel */
    else if (rx4_path_rdac_id == WCD_SPEAKER_ID)
    {
      bringup_count = &compander_0_bringup_count;

      /*
      #CDC_COMP0_B2_CTL__PEAK_METER_TIME_OUT = 9
      0x369; 0x0F; 0x09
      #CDC_CLK_RX_B2_CTL__RX_COMP0_CLK_EN = ENABLE
      0x310; 0x01; 0x01
      #CDC_COMP0_B3_CTL__LEVEL_METER_RESAMPLE_RATE = 1
      0x36A; 0xFF; 0x01
      #CDC_COMP0_B2_CTL__LEVEL_METER_DIV_FACTOR = 5
      0x369; 0xF0; 0x50
      #Delay_mS 1
      #CDC_COMP0_B3_CTL__LEVEL_METER_RESAMPLE_RATE = 40
      0x36A; 0xFF; 0x28
      #CDC_COMP0_B2_CTL__LEVEL_METER_DIV_FACTOR = 11
      0x369; 0xF0; 0xB0
      */
      if (*bringup_count == 0)
      {
        wcd_register_write(E_WCD_CODEC_MAIN, 0x369, 0x0F, 0, 0x09);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x01, 0, 0x01);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x36A, 0xFF, 0, 0x01);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x369, 0xF0, 4, 0x5);
        wcd_bus_flush(E_WCD_CODEC_MAIN);
        WCD_SLEEP(1);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x36A, 0xFF, 0, 0x28);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x369, 0xF0, 4, 0xB);
      }

      /*
      #CDC_CONN_MISC__RDAC5_MUX = DEM3_INV
      0x3B1; 0x04; 0x04
      #CDC_COMP0_B1_CTL__CH1_EN = ENABLE
      0x368; 0x02; 0x02
      #SPKR_DRV_GAIN__PA_GAIN_SEL = COPMANDER
      0x1E0; 0x04; 0x00
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x04, 0, 0x04);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x368, 0x02, 1, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0x04, 2, 0x00);

      spkr_in_compander_mode = WCD_TRUE;

      (*bringup_count)++;
    }
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32* bringup_count;
  uint32 rx4_path_rdac_id = 0;
  wcd_list_element *iterator;

  if(p_element->id == WCD_RX_1_COMPANDER_ID || p_element->id == WCD_RX_2_COMPANDER_ID)
  {
    bringup_count = &compander_1_bringup_count;

    /*
    #CDC_COMP1_B1_CTL__COMP_HALT = HALT
    0x370; 0x04; 0x04
    #Delay_mS 3
    */
    wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x04, 2, 0x01);
    WCD_SLEEP(3);

    /* Compander#1 left channel */
    if (p_element->id == WCD_RX_1_COMPANDER_ID)
    {
      /*
        #CDC_COMP1_B1_CTL__CH1_EN = DISABLE
        0x370; 0x02; 0x00
        #RX_HPH_L_GAIN__GAIN_SOURCE_SEL = REGISTER
        0x1AE; 0x20; 0x20
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x02, 1, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1AE, 0x20, 5, 0x01);

      hphl_in_compander_mode = WCD_FALSE;
    }
    /* Compander#1 right channel */
    else if (p_element->id == WCD_RX_2_COMPANDER_ID)
    {
    /*
      #CDC_COMP1_B1_CTL__CH2_EN = DISABLE
      0x370; 0x01; 0x00
      #RX_HPH_R_GAIN__GAIN_SOURCE_SEL = REGISTER
      0x1B4; 0x20; 0x20
    */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x01, 0, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B4, 0x20, 5, 0x01);

      hphr_in_compander_mode = WCD_FALSE;
    }

    if (*bringup_count != 0)
    {
      (*bringup_count)--;
    }

    if (*bringup_count == 0)
    {
    /*
      #CDC_CLK_RX_B2_CTL__RX_COMP1_CLK_EN = DISABLE
      0x310; 0x02; 0x00
    */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x02, 1, 0x00);
    }


    /*
      #CDC_COMP1_B1_CTL__COMP_HALT = NO HALT
      0x370; 0x04; 0x04
      #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP1_RESET = ENABLE
      0x309; 0x02; 0x02
      #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP1_RESET = DISABLE
      0x309; 0x02; 0x00
    */
    wcd_register_write(E_WCD_CODEC_MAIN, 0x370, 0x04, 2, 0x00);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x02, 1, 0x01);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x02, 1, 0x00);
  }

  /* Compander#2 left channel */
  if(p_element->id == WCD_RX_3_COMPANDER_ID)
  {
    bringup_count = &compander_2_bringup_count;

    /*
    #CDC_COMP2_B1_CTL__COMP_HALT = HALT
    0x378; 0x04; 0x04
    #Delay_mS 3
    #CDC_COMP2_B1_CTL__CH1_EN = DISABLE
    0x378; 0x02; 0x00
    #RX_LINE_1_GAIN__GAIN_SOURCE_SEL = REGISTER
    0x1CD; 0x20; 0x20
    #RX_LINE_2_GAIN__GAIN_SOURCE_SEL = REGISTER
    0x1D1; 0x20; 0x20
    */
    wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x04, 2, 0x01);
    WCD_SLEEP(3);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x02, 1, 0x00);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CD, 0x20, 5, 0x01);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x20, 5, 0x01);

    lo1_in_compander_mode = WCD_FALSE;
    lo2_in_compander_mode = WCD_FALSE;
  }

  /* Compander#2 right channel or speaker compander#0 left channel  */
  if(p_element->id == WCD_RX_4_COMPANDER_ID)
  {
    //retrieve rdac id of RX4 path to disable corresponding compander
    if (p_element_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_COMPANDER_ENABLE. Route or elements in route is NULL.>\n");
      return E_WCD_INVALID_PARAMETER_ERROR;
    }

    iterator = p_element_list->head;

    while(iterator != NULL)
    {
      if ( ((wcd_element*)(iterator->data))->id == WCD_LO2_R_M_ID )
      {
        rx4_path_rdac_id = WCD_LO2_R_M_ID;
        break;
      }
      if ( ((wcd_element*)(iterator->data))->id == WCD_SPEAKER_ID )
      {
        rx4_path_rdac_id = WCD_SPEAKER_ID;
        break;
      }

      iterator = iterator->next;
    }
    //done verification

    /* Compander#2 right channel */
    if(rx4_path_rdac_id == WCD_LO2_R_M_ID)
    {
      bringup_count = &compander_2_bringup_count;

      /*
      #CDC_COMP2_B1_CTL__COMP_HALT = HALT
      0x378; 0x04; 0x04
      #Delay_mS 3
      #CDC_COMP2_B1_CTL__CH2_EN = DISABLE
      0x378; 0x01; 0x00
      #RX_LINE_2_GAIN__GAIN_SOURCE_SEL = REGISTER
      0x1D1; 0x20; 0x20
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x04, 2, 0x01);
      WCD_SLEEP(3);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x01, 0, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x20, 5, 0x01);

      lo2_in_compander_mode = WCD_FALSE;

      if (*bringup_count != 0)
      {
        (*bringup_count)--;
      }
      if (*bringup_count == 0)
      {
        /*
        #CDC_CLK_RX_B2_CTL__RX_COMP2_CLK_EN = DISABLE
        0x310; 0x04; 0x00
        */
        wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x04, 2, 0x00);

        /*
        #CDC_COMP2_B1_CTL__COMP_HALT = NO HALT
        0x378; 0x04; 0x04
        #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP2_RESET = ENABLE
        0x309; 0x04; 0x04
        #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP2_RESET = DISABLE
        0x309; 0x04; 0x00
        */
        wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x04, 2, 0x00);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x04, 2, 0x01);
        wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x04, 2, 0x00);
      }
    }
    /* Speaker Compander#0 left channel */
    else if(rx4_path_rdac_id == WCD_SPEAKER_ID)
    {
      bringup_count = &compander_0_bringup_count;

      (*bringup_count)--;

      /*
      #CDC_CONN_MISC__RDAC5_MUX = DEM4
      0x3B1; 0x04; 0x00
      #CDC_COMP0_B1_CTL__COMP_HALT = HALT
      #Delay_mS 3
      0x368; 0x04; 0x04
      #CDC_COMP0_B1_CTL__CH1_EN = DISABLE
      0x368; 0x02; 0x00
      #CDC_CLK_RX_B2_CTL__RX_COMP0_CLK_EN = DISABLE
      0x310; 0x01; 0x00
      #CDC_COMP0_B1_CTL__COMP_HALT = NO_HALT
      0x368; 0x04; 0x00
      #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP0_RESET = ENABLE
      0x309; 0x01; 0x01
      #CDC_CLK_OTHR_RESET_B2_CTL__RX_COMP0_RESET = DISABLE
      0x309; 0x01; 0x00
      #SPKR_DRV_GAIN__PA_GAIN_SEL = REGISTER
      0x1E0; 0x04; 0x04
      */
      wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x04, 0, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x368, 0x04, 2, 0x01);
      WCD_SLEEP(3);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x368, 0x02, 1, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x310, 0x01, 0, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x378, 0x04, 2, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x01, 0, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x309, 0x01, 0, 0x00);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0x04, 2, 0x01);

      spkr_in_compander_mode = WCD_FALSE;
    }
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

/**********************************************************************************
 *******************              CLASS CONTROLLER              *******************
 *********************************************************************************/

uint32 class_h_earout_count = 0;
uint32 class_h_hph_count    = 0;

//indicates if the class h controller needs to be connected to the speaker
//interpolation path
wcd_bool class_h_speaker_connect = WCD_FALSE;

void class_h_block_enable(void)
{
  if (class_h_earout_count + class_h_hph_count == 0)
  {
    //#CDC_CLSH_B1_CTL__EN = ENABLE
    //0x320; 0x01; 0x01
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x01, 0, 0x01);
    //#CDC_CLK_OTHR_CTL__CP_CLK_EN = ENABLE
    //0x30C; 0x01; 0x01
    wcd_register_write(E_WCD_CODEC_MAIN, 0x30C, 0x01, 0, 0x01);
  }
}

void class_h_block_disable(void)
{
  if (class_h_earout_count + class_h_hph_count == 0)
  {
    //#CDC_CLSH_B1_CTL__EN = DISABLE
    //0x320; 0x01; 0x00
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x01, 0, 0x00);
    //#CDC_CLK_OTHR_CTL__CP_CLK_EN = DISABLE
    //0x30C; 0x01; 0x00
    wcd_register_write(E_WCD_CODEC_MAIN, 0x30C, 0x01, 0, 0x00);
  }
}

void class_h_hphl_connection(void)
{
  //put the class controller in hphl mode as default
  //#CDC_CONN_CLSH_CTL__DSM_MUX = RX_HPHL;
  //0x3B0; 0x30; 0x10
  wcd_register_write(E_WCD_CODEC_MAIN, 0x3B0, 0x30, 4, 0x1);
  //#CDC_CONN_CLSH_CTL__ZOH_MUX = RX_HPHL;
  //0x3B0; 0x0C; 0x04
  wcd_register_write(E_WCD_CODEC_MAIN, 0x3B0, 0x0C, 2, 0x1);
}

void class_h_spkr_connection(void)
{
  //for class controller 7 (from SPKR) we need to set some muxes
  //#CDC_CONN_CLSH_CTL__DSM_MUX = RX_SPKR;
  //0x3B0; 0x30; 0x20
  wcd_register_write(E_WCD_CODEC_MAIN, 0x3B0, 0x30, 4, 0x2);
  //#CDC_CONN_CLSH_CTL__ZOH_MUX = RX_SPKR;
  //0x3B0; 0x0C; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x3B0, 0x0C, 2, 0x2);
}

void class_h_common_enable(void)
{
  //#CDC_CLSH_BUCK_NCP_VARS__TAU_NCP = T_4P2US
  //0x323; 0x03; 0x00
  //wcd_register_write(E_WCD_CODEC_MAIN, 0x323, 0x03, 0, 0x0);

  //#CDC_CLSH_BUCK_NCP_VARS__TAU_BUCK = T_1US
  //0x323; 0x0C; 0x04
  wcd_register_write(E_WCD_CODEC_MAIN, 0x323, 0x0C, 2, 0x1);

  //#CDC_CLSH_B2_CTL__FCLKONLY_TRIG_CODE = S_7680
  //0x321; 0x03; 0x01
  wcd_register_write(E_WCD_CODEC_MAIN, 0x321, 0x03, 0, 0x1);
  //#CDC_CLSH_B2_CTL__IDLE_TRIG_CODE = S_7680
  //0x321; 0x0C; 0x04
  wcd_register_write(E_WCD_CODEC_MAIN, 0x321, 0x0C, 2, 0x1);
  //#CDC_CLSH_B2_CTL__V_DLY_CODE = 3
  //0x321; 0xF0; 0x30
  wcd_register_write(E_WCD_CODEC_MAIN, 0x321, 0xF0, 4, 0x3);
  //#CDC_CLSH_B3_CTL__I_PA_DLY_CODE = 3
  //0x322; 0xF0; 0x30
  wcd_register_write(E_WCD_CODEC_MAIN, 0x322, 0xF0, 4, 0x3);
  //#CDC_CLSH_B3_CTL__FCLK_DLY_CODE = 11
  //0x322; 0x0F; 0x0B
  wcd_register_write(E_WCD_CODEC_MAIN, 0x322, 0x0F, 0, 0x0B);

  //#CDC_CLSH_B1_CTL__ANC_DLY_EN = ENABLE
  //0x320; 0x02; 0x02
  wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x02, 1, 0x1);
}

wcd_result earout_class_h_enable(void)
{
  uint8 earout_z;
  //as default put the class controller in hph connection
  //we will connect it to speaker later if we need to
  class_h_hphl_connection();

  //enable class H common registers between EAR and HPH
  class_h_common_enable();

  earout_z = (WCD_EAROUT_IMPEDANCE - 4)/2;

  //#CDC_CLSH_B1_CTL__EARPA_REF = DB_0
  //0x320; 0x80; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x80, 7, 0x0);
  //#CDC_CLSH_V_PA_HD_EAR__VAL = 13
  //0x32E; 0x3F; 0x0D
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32E, 0x3F, 0, 0x0D);
  //#CDC_CLSH_V_PA_MIN_EAR__VAL = 58
  //0x330; 0x3F; 0x3A
  wcd_register_write(E_WCD_CODEC_MAIN, 0x330, 0x3F, 0, 0x3A);

  //#CDC_CLSH_IDLE_EAR_THSD__VAL = 37
  //0x325; 0x3F; 0x25
  wcd_register_write(E_WCD_CODEC_MAIN, 0x325, 0x3F, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][1]);
  //#CDC_CLSH_FCLKONLY_EAR_THSD__VAL = 43
  //0x327; 0x3F; 0x2B
  wcd_register_write(E_WCD_CODEC_MAIN, 0x327, 0x3F, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][2]);

  //#CDC_CLSH_I_PA_FACT_EAR_L__S0 = 5
  //0x32C; 0xE0; 0xA0
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32C, 0xE0, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][3]);
  //#CDC_CLSH_I_PA_FACT_EAR_U__S1 = 7
  //0x32D; 0x1F; 0x07
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32D, 0x1F, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][4]);
  //#CDC_CLSH_I_PA_FACT_EAR_L__S2 = 9
  //0x32C; 0x1F; 0x09
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32C, 0x1F, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][5]);

  //#CDC_CLSH_K_ADD__VAL = 8
  //0x328; 0x7F; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x328, 0x7F, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][6]);

  //#CDC_CLSH_K_DATA__K = 16
  //0x329; 0xFF; 0x10
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][7]);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][8]);
  //#CDC_CLSH_K_DATA__K = 37
  //0x329; 0xFF; 0x25
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][9]);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][10]);
  //#CDC_CLSH_K_DATA__K = 44
  //0x329; 0xFF; 0x2C
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][11]);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][12]);
  //#CDC_CLSH_K_DATA__K = 28
  //0x329; 0xFF; 0x1C
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][13]);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_EAROUT_CLSH_TABLE[earout_z][14]);


  //enable class H block now after configuration
  class_h_block_enable();

  return E_WCD_SUCCESS;
}

wcd_result hph_class_h_enable(void)
{

#ifdef WCD_MBHC_ENABLE
  int32 left_impedance=0, right_impedance=0;
  uint8 z_det_index=6;

  mbhc_query_impedance_details(&left_impedance, &right_impedance);

  if (left_impedance == 0)
    z_det_index = 6;
  else if (left_impedance <= 4)
    z_det_index = 0;
  else if (left_impedance >= 32)
    z_det_index = 14;
  else
    z_det_index = (uint8)((left_impedance - 4) / 2);

  WCD_DEBUG_INFO_3("=WCD_93XX: <CF hph_class_h_enable, left_impedance=%ld, right_impedance=%ld, index=%u >\n", left_impedance, right_impedance, z_det_index);
#endif

  //as default put the class controller in hph connection
  //we will connect it to speaker later if we need to
  class_h_hphl_connection();

  //enable class H common registers between EAR and HPH
  class_h_common_enable();

  //#CDC_CLSH_B1_CTL__HPHPA_REF = 0dB
  //0x320; 0x40; 0x0
  wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x40, 6, 0x0);

  //#CDC_CLSH_V_PA_HD_HPH__VAL = 13
  //0x32F; 0x3F; 0x0D
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32F, 0x3F, 0, 0x0D);
  //#CDC_CLSH_V_PA_MIN_HPH__VAL = 29
  //0x331; 0x3F; 0x1D
  wcd_register_write(E_WCD_CODEC_MAIN, 0x331, 0x3F, 0, 0x1D);

#ifdef WCD_MBHC_ENABLE

  //0x324;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x324, 0x3F, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][1]);
  //0x326;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x326, 0x3F, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][2]);

  //0x32A;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32A, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][3]);
  //0x32B;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32B, 0x1F, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][4]);
  //0x32A;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32A, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][5]);

  //0x328;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x328, 0x7F, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][6]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][7]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][8]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][9]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][10]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][11]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][12]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][13]);
  //0x329;
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, Z_DET_HPH_CLSH_TABLE[z_det_index][14]);

#else

  //#CDC_CLSH_IDLE_HPH_THSD__VAL = 37
  //0x324; 0x3F; 0x25
  wcd_register_write(E_WCD_CODEC_MAIN, 0x324, 0x3F, 0, 0x13);
  //#CDC_CLSH_FCLKONLY_HPH_THSD__VAL = 21
  //0x326; 0x3F; 0x15
  wcd_register_write(E_WCD_CODEC_MAIN, 0x326, 0x3F, 0, 0x17);

  //#CDC_CLSH_I_PA_FACT_HPH_L__S0 = 5
  //0x32A; 0xE0; 0xA0
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32A, 0xE0, 0, 0x97);
  //#CDC_CLSH_I_PA_FACT_HPH_U__S1 = 7
  //0x32B; 0x1F; 0x07
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32B, 0x1F, 0, 0x06);
  //#CDC_CLSH_I_PA_FACT_HPH_L__S2 = 9
  //0x32A; 0x1F; 0x09
  wcd_register_write(E_WCD_CODEC_MAIN, 0x32A, 0x1F, 0, 0x89);


  //#CDC_CLSH_K_ADD__VAL = 0
  //0x328; 0x7F; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x328, 0x7F, 0, 0x00);
  //#CDC_CLSH_K_DATA__K = 16
  //0x329; 0xFF; 0x10
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0xD0);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x01);
  //#CDC_CLSH_K_DATA__K = 37
  //0x329; 0xFF; 0x25
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x14);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x00);
  //#CDC_CLSH_K_DATA__K = 44
  //0x329; 0xFF; 0x2C
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x1B);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x00);
  //#CDC_CLSH_K_DATA__K = 28
  //0x329; 0xFF; 0x1C
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x1B);
  //#CDC_CLSH_K_DATA__K = 0
  //0x329; 0xFF; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x329, 0xFF, 0, 0x00);

#endif

  //enable class H block now after configuration
  class_h_block_enable();

  return E_WCD_SUCCESS;
}


wcd_result WCDH_CLASS_CONTROLLER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32 rdac_element_id;

  if (p_element_list == NULL || p_element_list->tail == NULL || p_element_list->tail->data == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_CLASS_CONTROLLER_ENABLE. Route or route elements is NULL.>\n");
    return E_WCD_INVALID_HANDLE;
  }
  rdac_element_id = ((wcd_element*)p_element_list->tail->data)->id;

  if (rdac_element_id == WCD_EAROUT_ID)
  {
    if (class_h_earout_count == 0)
    {
      //set earout is on speaker interpolation path to TRUE, only if class H isn't already
      //in earout mode
      if (p_element != NULL && p_element->id == WCD_CLASS_CONTROLLER_4_ID)
      {
        class_h_speaker_connect = WCD_TRUE;
      }
    }

    //if class H controller isn't in HPH or EAROUT mode enable it in EAROUT mode
    if (class_h_earout_count == 0 && class_h_hph_count == 0)
    {
      earout_class_h_enable();

      //if we are connected to the speaker interpolation path, we need to connect
      if (class_h_speaker_connect == WCD_TRUE)
      {
        class_h_spkr_connection();
      }
    }

    //increment class H earout count
    class_h_earout_count++;
  }
  else if (rdac_element_id == WCD_HPHL_ID || rdac_element_id == WCD_HPHR_ID)
  {
    //if class H controller isn't in HPH or EAROUT mode enable it in HPH mode
    if (class_h_earout_count == 0 && class_h_hph_count == 0)
    {
      hph_class_h_enable();
    }

    //increment class H hph count
    class_h_hph_count++;
  }
  else if (rdac_element_id == WCD_SPEAKER_ID)
  {
    //nothing to do for speaker
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_CLASS_CONTROLLER_ENABLE. Integrated speaker does not need anything for class D mode.>\n");
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;

}

wcd_result WCDH_CLASS_CONTROLLER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  uint32 rdac_element_id;

  if (p_element_list == NULL || p_element_list->tail == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_CLASS_CONTROLLER_ENABLE. Route or route elements is NULL.>\n");
    return E_WCD_INVALID_HANDLE;
  }
  rdac_element_id = ((wcd_element*)p_element_list->tail->data)->id;

  if (rdac_element_id == WCD_EAROUT_ID)
  {
    //decrement class H earout count
    class_h_earout_count--;

    //disable class H block.
    class_h_block_disable();

    //if class H controller does not need to be in earout mode
    //but is still in HPH mode, we need to enable it in HPH mode
    if (class_h_earout_count == 0 && class_h_hph_count != 0)
    {
      hph_class_h_enable();
    }

    //if we no longer need class H controller to be in earout mode
    //set the earout interpolation to speaker path to be FALSE
    if (class_h_earout_count == 0)
    {
      class_h_speaker_connect = WCD_FALSE;
    }
  }
  else if (rdac_element_id == WCD_HPHL_ID || rdac_element_id == WCD_HPHR_ID)
  {
    //decrement class H hph count
    class_h_hph_count--;

    //disable class H block.
    class_h_block_disable();

    //if class H controller does not need to be in HPH mode
    //but is still in EAR mode, we need to enable it in EAR mode
    if (class_h_earout_count != 0 && class_h_hph_count == 0)
    {
      earout_class_h_enable();

      if (class_h_speaker_connect == WCD_TRUE)
      {
        class_h_spkr_connection();
      }
    }
  }
  else if (rdac_element_id == WCD_SPEAKER_ID)
  {
    //nothin to do for speaker
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_CLASS_CONTROLLER_DISABLE. Integrated speaker does not need anything for class D mode.>\n");
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

/**********************************************************************************
 *******************                   RX BIAS                  *******************
 *********************************************************************************/

uint32 rx_bias_enable_count = 0;

wcd_result WCDH_RX_BIAS_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if (rx_bias_enable_count == 0)
  {
    //#RX_COM_BIAS__RX_BIAS_EN = ENABLE
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1A2, 0x80, 7, 0x1);
  }

  if (return_code == E_WCD_SUCCESS)
  {
    rx_bias_enable_count++;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

wcd_result WCDH_RX_BIAS_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if (rx_bias_enable_count == 1)
  {
    //#RX_COM_BIAS__RX_BIAS_EN = DISABLE
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1A2, 0x80, 7, 0x0);
  }

  if (return_code == E_WCD_SUCCESS)
  {
    rx_bias_enable_count--;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return return_code;
}

/**********************************************************************************
 *******************                     DAC                    *******************
 *********************************************************************************/

/*******************                   DAC GAIN                 ******************/

//todo apply gain if in non compander mode
uint8 hphl_gain = 0x2; //-3 dB
uint8 hphr_gain = 0x2; //-3 dB
uint8 lo1_gain = 0x0;  //0 dB
uint8 lo2_gain = 0x0;  //0 dB

uint32 gain_register_value(uint32 parameter)
{
  /*
  <parameter name="0DB" value="1" />
  <parameter name="-1.5DB" value="2" />
  <parameter name="-3.0DB" value="3" />
  <parameter name="-4.5DB" value="4" />
  <parameter name="-6.0DB" value="5" />
  <parameter name="-7.5DB" value="6" />
  <parameter name="-9.0DB" value="7" />
  <parameter name="-10.5DB" value="8" />
  <parameter name="-12.0DB" value="9" />
  <parameter name="-13.5DB" value="10" />
  <parameter name="-15.0DB" value="11" />
  <parameter name="-16.5DB" value="12" />
  <parameter name="-18.0DB" value="13" />
  <parameter name="-19.5DB" value="14" />
  <parameter name="-21.0DB" value="15" />
  <parameter name="-22.5DB" value="16" />
  <parameter name="-23.0DB" value="17" />
  <parameter name="-25.5DB" value="18" />
  <parameter name="-27.0DB" value="19" />
  <parameter name="-28.5DB" value="20" />
  <parameter name="-30.0DB" value="21" />
  */
  switch(parameter)
  {
    case 1:
      return 0x0;
    case 2:
      return 0x1;
    case 3:
      return 0x2;
    case 4:
      return 0x3;
    case 5:
      return 0x4;
    case 6:
      return 0x5;
    case 7:
      return 0x6;
    case 8:
      return 0x7;
    case 9:
      return 0x8;
    case 10:
      return 0x9;
    case 11:
      return 0xA;
    case 12:
      return 0xB;
    case 13:
      return 0xC;
    case 14:
      return 0xD;
    case 15:
      return 0xE;
    case 16:
      return 0xF;
    case 17:
      return 0x10;
    case 18:
      return 0x11;
    case 19:
      return 0x12;
    case 20:
      return 0x13;
    case 21:
      return 0x14;
    default:
      return 0x0;
  }
}

wcd_result WCDH_HPH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_bool *compander_enable_bool = NULL;
  uint8* gain = NULL;

  if (p_element->id == WCD_HPHL_ID)
  {
    compander_enable_bool = &hphl_in_compander_mode;
    gain = &hphl_gain;
  }
  else if (p_element->id == WCD_HPHR_ID)
  {
    compander_enable_bool = &hphr_in_compander_mode;
    gain = &hphr_gain;
  }

  if (gain == NULL || compander_enable_bool == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_HPH_SET_GAIN. Got an unknown HPH HAL Element.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (*compander_enable_bool == WCD_TRUE)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_HPH_SET_GAIN. Compander is enabled, gain setting is ignored.>\n");
    return E_WCD_SUCCESS;
  }

  *gain = (uint8)gain_register_value(parameter);

  if (p_element->id == WCD_HPHL_ID)
  {
    //#RX_HPH_L_GAIN__PA_GAIN =
    //0x1AE; 0x1F; gain value
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1AE, 0x1F, 0, *gain);
  }
  else if (p_element->id == WCD_HPHR_ID)
  {
    //#RX_HPH_R_GAIN__PA_GAIN =
    //0x1B4; 0x1F; gain value
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1B4, 0x1F, 0, *gain);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_LO_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_bool *compander_enable_bool = NULL;
  uint8* gain = NULL;

  if (p_element->id == WCD_LO1_L_P_ID)
  {
    compander_enable_bool = &lo1_in_compander_mode;
    gain = &lo1_gain;
  }
  else if (p_element->id == WCD_LO2_R_M_ID)
  {
    compander_enable_bool = &lo2_in_compander_mode;
    gain = &lo2_gain;
  }

  if (gain == NULL || compander_enable_bool == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_LO_SET_GAIN. Got an unknown lineout HAL Element.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (*compander_enable_bool == WCD_TRUE)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_LO_SET_GAIN. Compander is enabled, gain setting is ignored.>\n");
    return E_WCD_SUCCESS;
  }

  *gain = (uint8)gain_register_value(parameter);

  if (p_element->id == WCD_LO1_L_P_ID)
  {
    //#RX_LINE_1_GAIN__PA_GAIN =
    //0x1CD; 0x1F; gain value
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CD, 0x1F, 0, *gain);
  }
  else if (p_element->id == WCD_LO2_R_M_ID)
  {
    //#RX_LINE_2_GAIN__PA_GAIN =
    //0x1D1; 0x1F; gain value
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x1F, 0, *gain);
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

/*******************                 DAC PRECHARGE              ******************/

//tells us which HPH is going to be enabled using the above
//defines. this is set in precharge and cleared in enable
uint8 hph_to_process = 0x0;

#define ENABLE_LO1  0x1
#define ENABLE_LO2  0x2

//tells us which LOs are going to be enabled using the above defines.
//set in precharge and cleared in enable
uint8 lo_to_process = 0x0;

wcd_result WCDH_HPH_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (p_element->id == WCD_HPHL_ID)
  {
    hph_to_process |= ENABLE_HPHL;
  }
  else if (p_element->id == WCD_HPHR_ID)
  {
    hph_to_process |= ENABLE_HPHR;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_LO_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (p_element->id == WCD_LO1_L_P_ID)
  {
    lo_to_process |= ENABLE_LO1;
  }
  else if (p_element->id == WCD_LO2_R_M_ID)
  {
    lo_to_process |= ENABLE_LO2;
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

/*******************                 DAC ENABLE              ******************/

#define EAROUT_ENABLED    0x1
#define EAROUT_ENABLED_S  0

#define HPH_G_ENABLED     0x6
#define HPHL_ENABLED      0x2
#define HPHR_ENABLED      0x4
#define HPH_G_ENABLED_S   1
#define HPHL_ENABLED_S    1
#define HPHR_ENABLED_S    2

#define LO_G_ENABLED      0x78
#define LO1_ENABLED       0x8
#define LO2_ENABLED       0x10
#define LO_G_ENABLED_S    3
#define LO1_ENABLED_S     3
#define LO2_ENABLED_S     4

// keeps track of what has been enabled so far for concurrency in the BUCK
// uses the above defines for individual RDACs as well as the grouping defines
// indicated with a _G_
uint32 rdacs_enabled = 0x0;

// keeps track of how many times a certain rdac has been enabled
uint8 earout_enable_count = 0;
uint8 hphl_enable_count = 0;
uint8 hphr_enable_count = 0;
uint8 lo1_enable_count = 0;
uint8 lo2_enable_count = 0;

/********** ENABLE CHECK **********/

wcd_bool check_headphone_enabled(void)
{
  if ( (rdacs_enabled & HPH_G_ENABLED) >> HPH_G_ENABLED_S)
  {
    return WCD_TRUE;
  }

  return WCD_FALSE;
}

wcd_bool check_earout_enabled(void)
{
  if ( (rdacs_enabled & EAROUT_ENABLED) >> EAROUT_ENABLED_S)
  {
    return WCD_TRUE;
  }

  return WCD_FALSE;
}

wcd_bool check_lineout_enabled(void)
{
  if ( (rdacs_enabled & LO_G_ENABLED) >> LO_G_ENABLED_S)
  {
    return WCD_TRUE;
  }

  return WCD_FALSE;
}

/********** BUCK CONTROLS **********/

void buck_analog_override(void)
{
  //for single use BUCK is PWM mode and doesnt use IEST -- no need to bypass

  //1. Analog override ON to make BUCK independent of controller
  //a. Set bit 1 (CTRL_VREF_BYP) of reg 0x185 (BUCK_MODE_5) to 1

  //#BUCK_MODE_5__CTRL_VREF_BYP = FROM_I2C
  //0x185; 0x02; 0x02
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x02, 1, 0x1);
}

void buck_analog_override_concurrent(void)
{
  //for concurrent BUCK could be in PFM mode and need to set VREF +IEST

  //3. Analog override ON to make BUCK independent of controller
  //a. Set bit 1 (CTRL_VREF_BYP) of reg 0x185 (BUCK_MODE_5) to 0x1
  //b. Set bit 0 (CTRL_IEST_BYP) of reg 0x185 (BUCK_MODE_5) to 0x1

  //#BUCK_MODE_5__CTRL_VREF_BYP = FROM_I2C
  //0x185; 0x02; 0x02
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x02, 1, 0x1);

  //#BUCK_MODE_5__CTRL_IEST_BYP = FROM_I2C
  //0x185; 0x01; 0x01
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x01, 0, 0x1);
}

void buck_analog_override_off(void)
{
  //6. Analog override OFF to make BUCK dependent of controller
  //a. Set bit 1 (CTRL_VREF_BYP) of reg 0x185 (BUCK_MODE_5) to 0x0
  //b. Set bit 0 (CTRL_IEST_BYP) of reg 0x185 (BUCK_MODE_5) to 0x0

  //#BUCK_MODE_5__CTRL_VREF_BYP = FROM_CONTROLLER
  //0x185; 0x02; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x02, 1, 0x0);

  //#BUCK_MODE_5__CTRL_IEST_BYP = FROM_CONTROLLER
  //0x185; 0x01; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x01, 0, 0x0);
}

void buck_dependent_controller(void)
{
  //12. Analog override OFF to make BUCK dependent of controller
  //a.  Set bit 1 (CTRL_VREF_BYP) of reg 0x185 (BUCK_MODE_5) to 0

  //#BUCK_MODE_5__CTRL_VREF_BYP = FROM_CONTROLLER
  //0x185; 0x02; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x02, 1, 0x0);
}

void force_buck_2v(void)
{
  wcd_codec_info codec_info;
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_get_codec_info(&codec_info);
  if (return_code != E_WCD_SUCCESS)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec information.>\n");
  }

  if(codec_info.version == E_WCD_VER_2P0)
  {
    //3. Force analog Buck Buck_Vref = 63
    //a. Set <7:0> (VREF_I2C) reg 0x184 (BUCK_MODE_4) to 0x3F

    //#BUCK_MODE_4__VREF_I2C = 63
    //0x184; 0xFF; 0x3F
    wcd_register_write(E_WCD_CODEC_MAIN, 0x184, 0xFF, 0, 0x3F);
  }
  else
  {
    //3. Force analog Buck Buck_Vref = 2.0 V
    //a. Set <7:0> (VREF_I2C) reg 0x184 (BUCK_MODE_4) to 0xFF

    //#BUCK_MODE_4__VREF_I2C = V_2V
    //0x184; 0xFF; 0xFF
    wcd_register_write(E_WCD_CODEC_MAIN, 0x184, 0xFF, 0, 0xFF);
  }
}

void force_buck_1p8v(void)
{
  //7. Force analog Buck Buck_Vreg = 1.8V
  //a. Set bits 7:0 (VREF_I2C) of 0x184 (BUCK_MODE_4) to 0xE6

  //#BUCK_MODE_4__VREF_I2C = V1.8V
  //0x184; 0xFF; 0xE6
  wcd_register_write(E_WCD_CODEC_MAIN, 0x184, 0xFF, 0, 0xE6);
}

void buck_disable_bypass(void)
{
  //1. Force static register bypass=0 and disable dynamic bypass in BUCK auto mode
  //a. Set bit 3 (BUCK_MODE_I2C_BYPASS) of register 0x181 (BUCK_MODE_1) to 0
  //b. Set bit 3 (BYP_MODE_SEL) of register 0x183 (BUCK_MODE_3) to 0

  //#BUCK_MODE_1__BUCK_MODE_I2C_BYPASS = PWM_PFM_MODE_WHEN_BUCK_MODE_3_BIT_3_IS_0
  //0x181; 0x08; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x08, 3, 0x0);

  //#BUCK_MODE_3__BYP_MODE_SEL = FROM_REGISTER
  //0x183; 0x08; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x08, 3, 0x0);
}

void setup_buck_pwm_mode(void)
{
  //3. Setup the Buck in forced PWM mode
  //a. Set bit 2 (BUCK_MODE_I2C_PWMPFM) of register 0x181 (BUCK_MODE_1) to 1
  //b. Set bit 3 (BUCK_MODE_I2C_BYPASS) of register 0x181 (BUCK_MODE_1) to 0
  //c. Ensure that automatic mode is disabled
  //i. Register 0x183 (BUCK_MODE_3) bit 2 (PWM_PFM_MODE_SEL) set to 0
  //ii.Register 0x183 (BUCK_MODE_3) bit 3 (BYP_MODE_SEL) set to 0

  //#BUCK_MODE_1__BUCK_MODE_I2C_PWMPFM = PWM_MODE_WHEN_BUCK_MODE_3_BIT_2_IS_0
  //0x181; 0x04; 0x04
  //#BUCK_MODE_1__BUCK_MODE_I2C_BYPASS = PWM_PFM_MODE_WHEN_BUCK_MODE_3_BIT_3_IS_0
  //0x181; 0x08; 0x00
  //#BUCK_MODE_3__BYP_MODE_SEL = FROM_REGISTER
  //0x183; 0x08; 0x00
  //#BUCK_MODE_3__PWM_PFM_MODE_SEL = FROM_REGISTER
  //0x183; 0x04; 0x00

  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x04, 2, 0x1);
  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x08, 3, 0x0);
  wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x08, 3, 0x0);
  wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x04, 2, 0x0);
}

void setup_buck_pfm_mode(void)
{
  //3. Setup the Buck in forced PWM mode
  //a. Set bit 2 (BUCK_MODE_I2C_PWMPFM) of register 0x181 (BUCK_MODE_1) to 0
  //#BUCK_MODE_1__BUCK_MODE_I2C_PWMPFM = PWM_MODE_WHEN_BUCK_MODE_3_BIT_2_IS_0
  //0x181; 0x04; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x04, 2, 0x0);
}

void setup_buck_dynamic_bypass(void)
{
  //5. Enable dynamic bypass in Buck auto mode
  //a. Set bit 3 (BYP_MODE_SEL) of reg 0x183 (BUCK_MODE_3) to1

  //#BUCK_MODE_3__BYP_MODE_SEL = DYNAMIC
  //0x183; 0x08; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x08, 3, 0x1);
}

void set_buck_auto_mode(void)
{
  //14. Set the Buck to Auto mode
  //a. Register 0x183 (BUCK_MODE_3) bit 2 (PWM_PFM_MODE_SEL) set to 1
  //b. Register 0x183 (BUCK_MODE_3) bit 3 (BYP_MODE_SEL) set to 1

  //#BUCK_MODE_3__PWM_PFM_MODE_SEL = DYNAMIC
  //0x183; 0x04; 0x04
  wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x04, 2, 0x1);

  if(wcd_get_codec_buck_voltage() != E_WCD_VDD_BUCK_1P8V_2P15V)
  {
    //#BUCK_MODE_3__BYP_MODE_SEL = DYNAMIC
    //0x183; 0x08; 0x08
    wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x08, 3, 0x1);
  }
}

void power_up_buck(void)
{
  //4. Power up the Buck
  //a. Set bit 7 (BUCK_EN) of register 0x181 (BUCK_MODE_1) to 1

  //#BUCK_MODE_1__BUCK_EN = ENABLE
  //0x181; 0x80; 0x80
  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x80, 7, 0x1);
  WCD_DEBUG_INFO_0("POWERING UP BUCK!!  ")
}

void power_down_buck(void)
{
  //a. Power down the Buck,
  //i. Set bit 7 (BUCK_EN) of register 0x181 (BUCK_MODE_1) to 0

  //#BUCK_MODE_1__BUCK_EN = DISABLE
  //0x181; 0x80; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x181, 0x80, 7, 0x0);
}



/********** NCP CONTROLS **********/

void disable_ncp_bypass(void)
{
  //Set bit 4 (BYPASS_EN) of reg 0x194 (NCP_STATIC) to 0

  //#NCP_STATIC__BYPASS_EN = DISABLE
  //0x194; 0x10; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x10, 4, 0x0);
}

void enable_ncp_bypass(void)
{
  //Set bit 4 (BYPASS_EN) of reg 0x194 (NCP_STATIC) to 1

  //#NCP_STATIC__BYPASS_EN = ENABLE
  //0x194; 0x10; 0x10
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x10, 4, 0x1);
}

void enable_ncp(void)
{
  //7. Power up NCP
  //a. Set bit 0 (CP_EN) of register 0x192 (NCP_EN) to 1

  //#NCP_EN__CP_EN = ENABLE
  //0x192; 0x01; 0x01
  wcd_register_write(E_WCD_CODEC_MAIN, 0x192, 0x01, 0, 0x1);
}

void disable_ncp(void)
{
  //3. Turn off NCP
  //a. Set bit 0 (CP_EN) of register 0x192 (NCP_EN) to 0

  //#NCP_EN__CP_EN = DISABLE
  //0x192; 0x01; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x192, 0x01, 0, 0x0);
}

void switch_ncp_override_mode(void)
{
  //c. Set bit 5 (OVERRIDE_MODE_EN) of reg 0x194 (NCP_STATIC) to 1

  //#NCP_STATIC__OVERRIDE_MODE_EN = ENABLE
  //0x194; 0x20; 0x20
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x20, 5, 0x1);
}

void switch_ncp_normal_mode(void)
{
  //c. Set bit 5 (OVERRIDE_MODE_EN) of reg 0x194 (NCP_STATIC) to 0

  //#NCP_STATIC__OVERRIDE_MODE_EN = DISABLE
  //0x194; 0x20; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x20, 5, 0x0);
}

void force_ncp_flck_level_8(void)
{
  //b. Set bits 3:0 (OVERRIDE_FREQ) of reg 0x194 (NCP_STATIC) to 0x8

  //#NCP_STATIC__OVERRIDE_FREQ = F_1536_KHZ
  //0x194; 0x0F; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x0F, 0, 0x8);
}

void force_ncp_flck_level_5(void)
{
  //a. Set bits 3:0 (OVERRIDE_FREQ) of reg 0x194 (NCP_STATIC) to 0x5

  //#NCP_STATIC__OVERRIDE_FREQ = F_192_KHZ
  //0x194; 0x0F; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x194, 0x0F, 0, 0x5);
}

void disable_ncp_low_freq_switch(void)
{
  //a. Disable low freq switch reduction
  //i. Set bit 6 (LOWFREQ_EN) of reg 0x192 (NCP_EN) to 0
  //#NCP_EN__LOWFREQ_EN = DISABLE
  //0x192; 0x40; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x192, 0x40, 6, 0x0);
}

void set_ncp_iest_20ma(void)
{
  //c. Force Iest to 20mA
  //i. Set bit 0 (CTRL_IEST_BYP) of reg 0x185 (BUCK_MODE_5) to 1
  //i. Set bits 7:2 (IEST_I2C) of reg 0x185 (BUCK_MODE_5) to 0x02.

  //#BUCK_MODE_5__CTRL_IEST_BYP = FROM_I2C
  //0x185; 0x01; 0x01
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0x01, 0, 0x1);

  //#BUCK_MODE_5__IEST_I2C = I_20MA
  //0x185; 0xFC; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x185, 0xFC, 2, 0x2);
}



/********** CLASS H BLOCK **********/

void enable_class_h_ear(void)
{
  //11. Turn on class H controller computation and enable the ear PA
  //a. Set bit 4 (EARPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 1

  //#CDC_CLSH_B1_CTL__EARPA_EN = ENABLE
  //0x320; 0x10; 0x10
  wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x10, 4, 0x1);
}

void disable_class_h_ear(void)
{
  //11. Turn off class H controller computation and disable the ear PA
  //a. Set bit 4 (EARPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 0

  //#CDC_CLSH_B1_CTL__EARPA_EN = DISABLE
  //0x320; 0x10; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x10, 4, 0x0);
}

void enable_class_h_hph(void)
{
  //11. Turn on class H controller computation and enable the PAs
  //a. Set bit 3 (HPHLPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 1.
  //b. Set bit 2 (HPHRPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 1.

  if ( hph_to_process & ENABLE_HPHL )
  {
    //#CDC_CLSH_B1_CTL__HPHLPA_EN = ENABLE
    //0x320; 0x08; 0x08
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x08, 3, 0x1);
  }

  if ( hph_to_process & ENABLE_HPHR )
  {
    //#CDC_CLSH_B1_CTL__HPHRPA_EN = ENABLE
    //0x320; 0x04; 0x04
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x04, 2, 0x1);
  }
}

void disable_class_h_hph(void)
{
  //11. Turn off class H controller computation and disable the PAs
  //a.  Set bit 3 (HPHLPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 0.
  //b.  Set bit 2 (HPHRPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 0.

  if ( hph_to_process & ENABLE_HPHL && hphl_enable_count == 0)
  {
    //#CDC_CLSH_B1_CTL__HPHLPA_EN = DISABLE
    //0x320; 0x08; 0x00
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x08, 3, 0x0);
  }

  if ( hph_to_process & ENABLE_HPHR && hphr_enable_count == 0)
  {
    //#CDC_CLSH_B1_CTL__HPHRPA_EN = DISABLE
    //0x320; 0x04; 0x00
    wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x04, 2, 0x0);
  }
}

/********** PAs **********/

void prepare_ear_pa(void)
{
  //Enable DAC Reference and DAC data to EAR PA
  //#RX_EAR_EN__DAC_REF_EN = ENABLE
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x20, 5, 0x1);
  //#RX_EAR_EN__DAC_DATA_EN = ENABLE
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x40, 6, 0x1);

  //#RX_EAR_BIAS_PA__PA_BIAS_CURR = I_8_UA
  // 0x1BA; 0x0F; 0x08
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1BA, 0x0F, 0, 0x8);

  //Override PA enable from analog
  //#RX_EAR_CMBUFF__EAR_EN_SOURCE = ANALOG_I2C
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1BE, 0x01, 0, 0x1);
}

void enable_ear_pas(void)
{
  //a.	Set bit <4> (AMP_EN) of reg 0x1BC (RX_EAR _EN) to 1
  //#RX_EAR_EN__AMP_EN = ENABLE
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x10, 4, 0x1);
  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_SLEEP(5);

  earout_enable_count++;

  //mark it enabled in the RDAC
  rdacs_enabled |= EAROUT_ENABLED;
}

void disable_ear_pa(void)
{
  earout_enable_count--;

  if (earout_enable_count == 0)
  {
    //2. Turn off PAs
    //a. Set bit <4> (AMP_EN) of reg 0x1BC (RX_EAR_EN) to 0
    //#RX_EAR_EN__AMP_EN = DISABLE
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x10, 4, 0x0);
    wcd_bus_flush(E_WCD_CODEC_MAIN);
    WCD_SLEEP(5);

    disable_class_h_ear();

    //Disable DAC Reference and DAC data to EAR PA
    //#RX_EAR_EN__DAC_REF_EN = DISABLE
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x20, 5, 0x0);
    //#RX_EAR_EN__DAC_DATA_EN = DISABLE
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1BC, 0x40, 6, 0x0);

    //mark it disabled in the RDAC
    rdacs_enabled ^= EAROUT_ENABLED;
  }
}

void prepare_hph_pas(void)
{
  wcd_codec_info codec_info;
  wcd_result return_code = E_WCD_SUCCESS;

  if ( hph_to_process & ENABLE_HPHL )
  {
    //Enable DAC Reference and DAC data to HPHL PA
    //#RX_HPH_L_DAC_CTL__DAC_REF_EN = ENABLE
    //0x1B1; 0x40; 0x40
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x40, 6, 0x1);
    //#RX_HPH_L_DAC_CTL__DAC_DATA_EN = ENABLE
    //0x1B1; 0x80; 0x80
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x80, 7, 0x1);

    return_code = wcd_get_codec_info(&codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec information.>\n");
    }

    if(codec_info.version == E_WCD_VER_2P0)
    {
      //#RX_HPH_L_DAC_CTL__GAIN_DAC = G_0P27DB
      //0x1B1; 0x03; 0x01
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x03, 0, 0x1);

      //#RX_HPH_R_DAC_CTL__GAIN_DAC = G_0P27DB
      //0x1B7; 0x03; 0x01
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x03, 0, 0x1);

      //#CDC_CLSH_B1_CTL__HPH_REF = DB_0P27
      //0x320; 0x40; 0x40
      wcd_register_write(E_WCD_CODEC_MAIN, 0x320, 0x40, 0, 0x40);
    }

    //#CDC_CLK_RDAC_CLK_EN_CTL__RDAC2_CLK_EN = 1
    //0x30D; 0x02; 0x02
    wcd_register_write(E_WCD_CODEC_MAIN, 0x30D, 0x02, 1, 0x01);
  }

  if ( hph_to_process & ENABLE_HPHR )
  {
    //Enable DAC Reference and DAC data to HPHR PA
    //#RX_HPH_R_DAC_CTL__DAC_REF_EN = ENABLE
    //0x1B7; 0x40; 0x40
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x40, 6, 0x1);
    //#RX_HPH_R_DAC_CTL__DAC_DATA_EN = ENABLE
    //0x1B7; 0x80; 0x80
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x80, 7, 0x1);

    //#CDC_CLK_RDAC_CLK_EN_CTL__RDAC3_CLK_EN = 1
    //0x30D; 0x04; 0x04
    wcd_register_write(E_WCD_CODEC_MAIN, 0x30D, 0x04, 2, 0x01);
  }

  //3. Override PA enable from analog
  //a. Set bit 0 (PA_EN_OVERRIDE) of reg 0x1AA to 1
  //#RX_HPH_OCP_CTL__PA_EN_OVERRIDE = FROM_I2C
  //0x1AA; 0x01; 0x01
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1AA, 0x01, 0, 0x1);
}

wcd_result WCDH_HS_HPH_ENABLE(uint16 reg, uint8 mask, uint8 shift, uint8 val, uint8 hph_pa_en, wcd_bool enable, wcd_owner_module owner)
{
  wcd_result rc = E_WCD_SUCCESS;

    // If this function is called from WCD CORE:
    //    1. Update the status of HS_HPHx_EN
    //    2. Always execute the register write

    // If this function is called from MBHC:
    //    1. Always execute the register write if enable == WCD_FALSE
    //    2. Execute the register write ONLY IF corresponding HS_HPHx_EN flag is TRUE
    //    3. Otherwise, just return from the function without executing the register write

    if (owner == E_WCD_CORE)
    {
      if (hph_pa_en == ENABLE_HPHL)
      {
          HS_HPHL_EN = enable;
      }
      else if (hph_pa_en == ENABLE_HPHR)
      {
          HS_HPHR_EN = enable;
      }

      // 0 means default or unchanged state
      // 1 means HPH_PA is enabled
      // 2 means HPH_PA is disabled
      if ((HS_HPHL_EN == WCD_TRUE) || (HS_HPHR_EN == WCD_TRUE))
      {
        HPH_PA_State = 1;
      }
      else if ((HS_HPHL_EN == WCD_FALSE) && (HS_HPHR_EN == WCD_FALSE))
      {
        HPH_PA_State = 2;
      }
      WCD_DEBUG_INFO_1("=WCD_93XX: <CF WCDH_HS_HPH_ENABLE, HPH_PA_State=(%u)> \n", HPH_PA_State);
    }
    else if (owner == E_WCD_MBHC)
    {
      if (enable == WCD_TRUE)
      {
        if ( ((hph_pa_en == ENABLE_HPHL) && (HS_HPHL_EN == WCD_FALSE)) ||
             ((hph_pa_en == ENABLE_HPHR) && (HS_HPHR_EN == WCD_FALSE))   )
        {
            WCD_DEBUG_INFO_3("=WCD_93XX: <CF WCDH_HS_HPH_ENABLE, No need to update HPH PAs, hph_pa_en=(%u), HS_HPHL_EN=(%u), HS_HPHR_EN=(%u)> \n", hph_pa_en, HS_HPHL_EN, HS_HPHR_EN);
            return rc;
        }
      }
    }

    rc = wcd_register_write(E_WCD_CODEC_MAIN, (uint16)reg, (uint8)mask, (uint8)shift, (uint8)val);
    WCD_DEBUG_INFO_2("=WCD_93XX: <CF WCDH_HS_HPH_ENABLE, Updated HPH PAs, owner=(%d), rc=(0%d)> \n", owner, rc );
    WCD_DEBUG_INFO_3("=WCD_93XX: <CF WCDH_HS_HPH_ENABLE, Updated HPH PAs, hph_pa_en=(%u), HS_HPHL_EN=(%u), HS_HPHR_EN=(%u)> \n", hph_pa_en, HS_HPHL_EN, HS_HPHR_EN);

    return rc;
}

void enable_hph_pas(void)
{
  //a. Set bits <5:4> to 0x3 (HPHR_HPHL) of reg 0x1AB (RX_HPH_CNP_EN)

    //reset WG timer to 0x15(default value) before enabling PA's.
    //#RX_HPH_CNP_WG_TIME_WG_FINE_TIMER=0x15 ~ 2.4ms
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1AD, 0xfe, 0, 0x14);
    WCD_SLEEP(5);
  if ((hph_to_process & ENABLE_HPHL) && (hph_to_process & ENABLE_HPHR))
  {
    //#RX_HPH_CNP_EN__HPH_PA_EN = HPHL
    //0x1AB; 0x20; 0x20
    WCDH_HS_HPH_ENABLE(0x1AB, 0x20, 5, 0x1, ENABLE_HPHL, WCD_TRUE, E_WCD_CORE);
    hphl_enable_count++;

	//#RX_HPH_CNP_EN__HPH_PA_EN = HPHR
	//0x1AB; 0x10; 0x10
	WCDH_HS_HPH_ENABLE(0x1AB, 0x10, 4, 0x1, ENABLE_HPHR, WCD_TRUE, E_WCD_CORE);
	hphr_enable_count++;

	WCD_SLEEP(5);
  }

  else if ( hph_to_process & ENABLE_HPHL )
  {
    /* Temporarily 40ms delay has been added to ensure clock is available
       before enabling PAs
    */
   // WCD_SLEEP(40);
    //#RX_HPH_CNP_EN__HPH_PA_EN = HPHL
    //0x1AB; 0x20; 0x20
    WCDH_HS_HPH_ENABLE(0x1AB, 0x20, 5, 0x1, ENABLE_HPHL, WCD_TRUE, E_WCD_CORE);

    hphl_enable_count++;

    WCD_SLEEP(5);
  }

  else if ( hph_to_process & ENABLE_HPHR )
  {
    /* Temporarily 40ms delay has been added to ensure clock is available
       before enabling PAs
    */
   // WCD_SLEEP(40);
    //#RX_HPH_CNP_EN__HPH_PA_EN = HPHR
    //0x1AB; 0x10; 0x10
    WCDH_HS_HPH_ENABLE(0x1AB, 0x10, 4, 0x1, ENABLE_HPHR, WCD_TRUE, E_WCD_CORE);

    hphr_enable_count++;

    WCD_SLEEP(5);
  }

  rdacs_enabled |= (hph_to_process << HPH_G_ENABLED_S);

  //clear the HPHs that need to be enabled to do nothing on the next HPH enable
  hph_to_process = 0x0;

  wcd_bus_flush(E_WCD_CODEC_MAIN);
  WCD_SLEEP(5); //5ms delay on all PAs only
}

void disable_hph_pas(void)
{
  //11. Turn off class H controller computation and disable the PAs
  //a.  Set bit 3 (HPHLPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 0.
  //b.  Set bit 2 (HPHRPA_EN) of reg 0x320 (CDC_CLSH_B1_CTL) to 0.

  if (( hph_to_process & ENABLE_HPHL ) && ( hph_to_process & ENABLE_HPHR ))
  {
    hphl_enable_count--;
    hphr_enable_count--;
    if (hphl_enable_count == 0)
    {
      //#RX_HPH_CNP_EN__HPH_PA_EN = HPHL
      //0x1AB; 0x20; 0x00
      WCDH_HS_HPH_ENABLE(0x1AB, 0x20, 5, 0x0, ENABLE_HPHL, WCD_FALSE, E_WCD_CORE);
    }
    if (hphr_enable_count == 0)
    {
      //#RX_HPH_CNP_EN__HPH_PA_EN = HPHR
      //0x1AB; 0x10; 0x00
      WCDH_HS_HPH_ENABLE(0x1AB, 0x10, 4, 0x0, ENABLE_HPHR, WCD_FALSE, E_WCD_CORE);
    }

    WCD_SLEEP(10);
    disable_class_h_hph();

    if (hphl_enable_count == 0)
    {
      //Disable DAC Reference and DAC data to HPHL PA
      //#RX_HPH_L_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1B1; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x80, 7, 0x0);
      //#RX_HPH_L_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1B1; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x40, 6, 0x0);

      //mark it disabled in the RDAC
      rdacs_enabled ^= HPHL_ENABLED;
    }

    if (hphr_enable_count == 0)
    {
      //Disable DAC Reference and DAC data to HPHR PA
      //#RX_HPH_R_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1B7; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x80, 7, 0x0);
      //#RX_HPH_R_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1B7; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x40, 6, 0x0);

      //mark it disabled in the RDAC
      rdacs_enabled ^= HPHR_ENABLED;
    }
  }

  else if ( hph_to_process & ENABLE_HPHL )
  {
    hphl_enable_count--;

    if (hphl_enable_count == 0)
    {
      //#RX_HPH_CNP_EN__HPH_PA_EN = HPHL
      //0x1AB; 0x20; 0x00
      WCDH_HS_HPH_ENABLE(0x1AB, 0x20, 5, 0x0, ENABLE_HPHL, WCD_FALSE, E_WCD_CORE);


      WCD_SLEEP(5);

      disable_class_h_hph();

      //Disable DAC Reference and DAC data to HPHL PA
      //#RX_HPH_L_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1B1; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x40, 6, 0x0);
      //#RX_HPH_L_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1B1; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B1, 0x80, 7, 0x0);

      //mark it disabled in the RDAC
      rdacs_enabled ^= HPHL_ENABLED;
    }
  }

  else if ( hph_to_process & ENABLE_HPHR )
  {
    hphr_enable_count--;

    if (hphr_enable_count == 0)
    {
      //#RX_HPH_CNP_EN__HPH_PA_EN = HPHR
      //0x1AB; 0x10; 0x00
      WCDH_HS_HPH_ENABLE(0x1AB, 0x10, 4, 0x0, ENABLE_HPHR, WCD_FALSE, E_WCD_CORE);


      WCD_SLEEP(5);

      disable_class_h_hph();

      //Disable DAC Reference and DAC data to HPHR PA
      //#RX_HPH_R_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1B7; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x40, 6, 0x0);
      //#RX_HPH_R_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1B7; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B7, 0x80, 7, 0x0);

      //mark it disabled in the RDAC
      rdacs_enabled ^= HPHR_ENABLED;
    }
  }

  //clear the HPHs that need to be enabled to do nothing on the next HPH enable
  hph_to_process = 0x0;

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_SLEEP(5); //5ms delay on all PAs only
}

void prepare_lo_pas(void)
{
  if ( lo_to_process & ENABLE_LO1 )
  {
    //#RX_LINE_1_DAC_CTL__DAC_REF_EN = ENABLE
    //0x1CF; 0x40; 0x40
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CF, 0x40, 6, 0x1);
    //#RX_LINE_1_DAC_CTL__DAC_DATA_EN = ENABLE
    //0x1CF; 0x80; 0x80
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CF, 0x80, 7, 0x1);
  }

  if ( lo_to_process & ENABLE_LO2 )
  {
    //Enable DAC Reference and DAC data to LO2 PA
    //#RX_LINE_2_DAC_CTL__DAC_REF_EN = ENABLE
    //0x1D3; 0x40; 0x40
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1D3, 0x40, 6, 0x1);
    //#RX_LINE_2_DAC_CTL__DAC_DATA_EN = ENABLE
    //0x1D3; 0x80; 0x80
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1D3, 0x80, 7, 0x1);
  }
}

void enable_lo_pas(void)
{
  //8. Enable line PA(s)

  if ( lo_to_process & ENABLE_LO1 )
  {
    //#RX_LINE_CNP_EN__LINE_PA_EN = LO1
    //0x1CA; 0x0F; 0x01
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CA, 0x01, 0, 0x1);
    lo1_enable_count++;
  }

  if ( lo_to_process & ENABLE_LO2 )
  {
    //#RX_LINE_CNP_EN__LINE_PA_EN = LO2
    //0x1CA; 0x0F; 0x02
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1CA, 0x02, 1, 0x1);
    lo2_enable_count++;
  }

  rdacs_enabled |= (lo_to_process << LO_G_ENABLED_S);

  //clear the LOs that need to be enabled to do nothing on the next LO enable
  lo_to_process = 0x0;

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_SLEEP(5); //5ms delay on all PAs only
}

void disable_lo_pas(void)
{
  //8. Disable line PA(s)

  if ( lo_to_process & ENABLE_LO1 )
  {
    lo1_enable_count--;

    if (lo1_enable_count == 0)
    {
      //#RX_LINE_CNP_EN__LINE_PA_EN = LO1
      //0x1CA; 0x0F; 0x01
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1CA, 0x01, 0, 0x0);

      //Disable DAC Reference and DAC data to LO1 PA
      //#RX_LINE_1_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1CF; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1CF, 0x40, 6, 0x0);
      //#RX_LINE_1_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1CF; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1CF, 0x80, 7, 0x0);

      //mark LO1 disabled
      rdacs_enabled ^= LO1_ENABLED;
    }
  }

  if ( lo_to_process & ENABLE_LO2 )
  {
    lo2_enable_count--;

    if (lo2_enable_count == 0)
    {
      //#RX_LINE_CNP_EN__LINE_PA_EN = LO2
      //0x1CA; 0x0F; 0x02
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1CA, 0x02, 1, 0x0);

      //Disable DAC Reference and DAC data to LO2 PA
      //#RX_LINE_2_DAC_CTL__DAC_REF_EN = DISABLE
      //0x1D3; 0x40; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1D3, 0x40, 6, 0x0);
      //#RX_LINE_2_DAC_CTL__DAC_DATA_EN = DISABLE
      //0x1D3; 0x80; 0x00
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1D3, 0x80, 7, 0x0);

      //mark LO2 disabled
      rdacs_enabled ^= LO2_ENABLED;
    }
  }

  //clear the LOs that need to be enabled to do nothing on the next LO enable
  lo_to_process = 0x0;
  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_SLEEP(5); //5ms delay on all PAs only
}

/********** EVENT FUNCTIONS **********/

wcd_result WCDH_HPH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //pre charge needs to be called so we know what is being enabled
  if (hph_to_process == 0x0)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_HPH_ENABLE. No HPH PAs have been precharged.>\n");
    return E_WCD_SUCCESS;
  }

  //if either HPHL or HPHR has been enabled. Enable the other HPH PA (just in case)
  //and return
  if (hphl_enable_count > 0 || hphr_enable_count > 0)
  {
    prepare_hph_pas();

    enable_class_h_hph();

    enable_hph_pas();

    return E_WCD_SUCCESS;
  }

  //early exit for both EAR/LO -> HPH/LO/EAR concurrency
  if ( check_earout_enabled() == WCD_TRUE &&
       check_lineout_enabled() == WCD_TRUE )
  {
    prepare_hph_pas();

    enable_class_h_hph();

    enable_hph_pas();

    return E_WCD_SUCCESS;
  }

  //HPH SOLO
  if ( check_earout_enabled() == WCD_FALSE &&
       check_lineout_enabled() == WCD_FALSE)
  {
    prepare_hph_pas();

    enable_class_h_hph();

    buck_analog_override();

    force_buck_2v();

    setup_buck_pwm_mode();

    power_up_buck();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    disable_ncp_bypass();
    force_ncp_flck_level_8();
    switch_ncp_override_mode();

    enable_ncp();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    enable_hph_pas();

    buck_dependent_controller();

    switch_ncp_normal_mode();

    set_buck_auto_mode();
  }

  //EAR -> EAR/HPH
  if ( check_earout_enabled() == WCD_TRUE )
  {
    prepare_hph_pas();

    buck_disable_bypass();

    force_buck_2v();

    buck_analog_override_concurrent();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_8();
    switch_ncp_override_mode();

    enable_ncp_bypass();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    enable_class_h_hph();

    enable_hph_pas();
  }

  //LO -> LO/HPH
  if ( check_lineout_enabled() == WCD_TRUE )
  {
    prepare_hph_pas();

    buck_disable_bypass();

    force_buck_2v();

    buck_analog_override_concurrent();

    power_up_buck();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_8();
    switch_ncp_override_mode();

    enable_ncp_bypass();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    enable_class_h_hph();

    enable_hph_pas();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_HPH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //pre charge needs to be called so we know what is being disabled
  if (hph_to_process == 0x0)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_HPH_DISABLE. No HPH PAs have been precharged.>\n");
    return E_WCD_SUCCESS;
  }

  disable_hph_pas();

  if (hphl_enable_count > 0 || hphr_enable_count > 0)
  {
    return E_WCD_SUCCESS;
  }

  //early exit for EAR/LO/HPH -> EAR/LO
  if ( check_earout_enabled() == WCD_TRUE &&
       check_lineout_enabled() == WCD_TRUE )
  {
    return E_WCD_SUCCESS;
  }

  //HPH SOLO
  if ( check_earout_enabled() == WCD_FALSE &&
       check_lineout_enabled() == WCD_FALSE )
  {
    disable_ncp();

    power_down_buck();
  }

  //EAR/HPH -> EAR
  if ( check_earout_enabled() == WCD_TRUE )
  {
    disable_ncp_bypass();

    switch_ncp_normal_mode();

    setup_buck_dynamic_bypass();

    buck_analog_override_off();
  }

  //LO/HPH -> LO
  if ( check_lineout_enabled() == WCD_TRUE )
  {
    force_ncp_flck_level_5();

    disable_ncp_low_freq_switch();

    disable_ncp_bypass();

    WCD_SLEEP(1);

    set_ncp_iest_20ma();

    setup_buck_pfm_mode();

    force_buck_1p8v();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_EAROUT_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  if (earout_enable_count > 0)
  {
    prepare_ear_pa();

    enable_class_h_ear();

    enable_ear_pas();

    return E_WCD_SUCCESS;
  }

  //early exit for both HPH/LO -> HPH/LO/EAR concurrency
  if ( check_headphone_enabled() == WCD_TRUE &&
       check_lineout_enabled() == WCD_TRUE )
  {
    prepare_ear_pa();

    enable_class_h_ear();

    enable_ear_pas();

    return E_WCD_SUCCESS;
  }

  //EAR SOLO
  if ( check_headphone_enabled() == WCD_FALSE &&
       check_lineout_enabled() == WCD_FALSE)
  {
    prepare_ear_pa();

    enable_class_h_ear();

    buck_analog_override();

    force_buck_2v();

    setup_buck_pwm_mode();

    power_up_buck();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    disable_ncp_bypass();

    force_ncp_flck_level_8();

    switch_ncp_override_mode();

    enable_ncp();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    buck_dependent_controller();

    switch_ncp_normal_mode();

    set_buck_auto_mode();

    enable_ear_pas();
  }

  //HPH -> HPH/EAR
  if ( check_headphone_enabled() == WCD_TRUE )
  {
    prepare_ear_pa();

    buck_disable_bypass();

    force_buck_2v();

    buck_analog_override_concurrent();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_8();

    switch_ncp_override_mode();

    enable_ncp_bypass();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    enable_class_h_ear();

    enable_ear_pas();
  }

  //LO -> LO/EAR
  if ( check_lineout_enabled() == WCD_TRUE )
  {
    prepare_ear_pa();

    buck_disable_bypass();

    force_buck_2v();

    buck_analog_override_concurrent();

    power_up_buck();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_8();

    switch_ncp_override_mode();

    enable_ncp_bypass();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    enable_class_h_ear();

    enable_ear_pas();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_EAROUT_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  disable_ear_pa();

  if (earout_enable_count > 0)
  {
    return E_WCD_SUCCESS;
  }

  //early exit for HPH/LO/EAR -> HPH/LO
  if ( check_headphone_enabled() == WCD_TRUE &&
       check_lineout_enabled() == WCD_TRUE )
  {
    return E_WCD_SUCCESS;
  }

  //EAR SOLO
  if ( check_headphone_enabled() == WCD_FALSE &&
        check_lineout_enabled() == WCD_FALSE )
  {
    disable_ncp();

    power_down_buck();
  }

  //HPH/EAR -> HPH
  if ( check_headphone_enabled() == WCD_TRUE )
  {
    disable_ncp_bypass();

    switch_ncp_normal_mode();

    setup_buck_dynamic_bypass();

    buck_analog_override_off();
  }

  //LO/EAR -> LO
  if ( check_lineout_enabled() == WCD_TRUE )
  {
    force_ncp_flck_level_5();

    disable_ncp_low_freq_switch();

    disable_ncp_bypass();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    set_ncp_iest_20ma();

    setup_buck_pfm_mode();

    force_buck_1p8v();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_LO_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //pre charge needs to be called so we know what is being enabled
  if (lo_to_process == 0x0)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_LO_ENABLE. No LO PAs have been precharged.>\n");
    return E_WCD_SUCCESS;
  }

  //if either LO has been enabled. Enable the other LO PAs (just in case)
  //and return
  if (lo1_enable_count > 0 || lo2_enable_count > 0)
  {
    prepare_lo_pas();

    enable_lo_pas();

    return E_WCD_SUCCESS;
  }

  //early exit for both HPH/EAR -> HPH/EAR/LO concurrency
  if ( check_headphone_enabled() == WCD_TRUE &&
        check_earout_enabled() == WCD_TRUE )
  {
    prepare_lo_pas();

    enable_lo_pas();

    return E_WCD_SUCCESS;
  }

  //LO SOLO
  if ( check_headphone_enabled() == WCD_FALSE &&
       check_earout_enabled() == WCD_FALSE)
  {
    prepare_lo_pas();

    buck_analog_override();

    force_buck_1p8v();

    setup_buck_pwm_mode();

    power_up_buck();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_5();
    switch_ncp_override_mode();

    disable_ncp_low_freq_switch();
    enable_ncp();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    set_ncp_iest_20ma();

    setup_buck_pfm_mode();

    enable_lo_pas();
  }

  //HPH -> HPH/LO  or  EAR -> EAR/LO
  if ( check_headphone_enabled() == WCD_TRUE ||
       check_earout_enabled() == WCD_TRUE )
  {
    prepare_lo_pas();

    buck_disable_bypass();

    force_buck_2v();

    buck_analog_override_concurrent();

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    WCD_SLEEP(1);

    force_ncp_flck_level_8();
    switch_ncp_override_mode();

    enable_ncp_bypass();

    enable_lo_pas();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_LO_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //pre charge needs to be called so we know what is being disabled
  if (lo_to_process == 0x0)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_LO_DISABLE. No LO PAs have been precharged.>\n");
    return E_WCD_SUCCESS;
  }

  disable_lo_pas();

  if (lo1_enable_count > 0 || lo2_enable_count > 0)
  {
    return E_WCD_SUCCESS;
  }

  //early exit for HPH/EAR/LO -> HPH/EAR
  if (check_headphone_enabled() == WCD_TRUE &&
      check_earout_enabled() == WCD_TRUE )
  {
    return E_WCD_SUCCESS;
  }

  //LO SOLO
  if (check_headphone_enabled() == WCD_FALSE &&
      check_earout_enabled() == WCD_FALSE )
  {
    disable_ncp();

    power_down_buck();
  }

  //HPH/LO -> HPH  or  EAR/LO -> EAR
  if ( check_headphone_enabled() == WCD_TRUE ||
       check_earout_enabled() == WCD_TRUE )
  {
    disable_ncp_bypass();

    switch_ncp_normal_mode();

    setup_buck_dynamic_bypass();

    buck_analog_override_off();
  }

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}


/**********************************************************************************
 *******************                   SPEAKER                  *******************
 *********************************************************************************/

wcd_result WCDH_SPEAKER_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 val = 0x0;

  /*
    <parameter name="POS_12_DB" value="1" />
    <parameter name="POS_10P5_DB" value="2" />
    <parameter name="POS_9_DB" value="3" />
    <parameter name="POS_7P5_DB" value="4" />
    <parameter name="POS_6_DB" value="5" />
    <parameter name="POS_4P5_DB" value="6" />
    <parameter name="POS_3_DB" value="7" />
    <parameter name="POS_1P5_DB" value="8" />
    <parameter name="POS_0_DB" value="9" />
  */
  switch(parameter)
  {
    case 1:
      val = 0x0;
      break;
    case 2:
      val = 0x1;
      break;
    case 3:
      val = 0x2;
      break;
    case 4:
      val = 0x3;
      break;
    case 5:
      val = 0x4;
      break;
    case 6:
      val = 0x5;
      break;
    case 7:
      val = 0x6;
      break;
    case 8:
      val = 0x7;
      break;
    case 9:
      val = 0x8;
      break;
    default:
      WCD_DEBUG_ERROR_0("=WCD_93XX: <CF WCDH_SPEAKER_SET_GAIN. Invalid parameter.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  //#SPKR_DRV_GAIN__PA_GAIN_SEL = REGISTER
  //0x1E0; 0x04; 0x04
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0x04, 2, 0x1);

  //#SPKR_DRV_GAIN__PA_GAIN = (parameter - 1)
  //0x1E0; 0xF8; value selected from parameter
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0xF8, 3, val);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_SPEAKER_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  //don't need to do anything for speaker precharge!
  return E_WCD_SUCCESS;
}

wcd_result WCDH_SPEAKER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{

  wcd_codec_info codec_info;
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_get_codec_info(&codec_info);
  if (return_code != E_WCD_SUCCESS)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec information.>\n");
  }

  if(codec_info.version == E_WCD_VER_2P0)
  {
    //#SPKR_DRV_BIAS_INT__INT1_BIAS_CURR = I_7P5_UA
    //0x1E9; 0xF0; 0xF0
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1E9, 0xF0, 0, 0xF0);

    //#SPKR_DRV_BIAS_PA__COMP_BIAS_CURR = I_1P0_UA
    //0x1EA; 0xF0; 0x20
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1EA, 0xF0, 0, 0x20);
  }

  /* #INTR_MASK3__INT_BITS = 10
     0x096; 0x3F; 0x0A
    #INTR_CLEAR3__INT_BITS = 10
    0x09E; 0x3F; 0x0A */
  
  wcd_register_write(E_WCD_CODEC_MAIN, 0x096, 0x3F, 0, 0x0A);
  wcd_register_write(E_WCD_CODEC_MAIN, 0x09E, 0x3F, 0, 0x0A);


  if(codec_info.version == E_WCD_VER_2P0)
  {
    //#SPKR_DRV_GAIN__DAC_GAIN = POS_0P27_DB
    //0x1E0; 0x03; 0x01
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0x03, 0, 0x01);
  }
  else
  {

    /* #SPKR_DRV_DBG_DAC__GAIN_DBG_EN = 1
       0x1E5; 0x80; 0x80
       #SPKR_DRV_DBG_DAC__GAIN_DBG_VAL = 6
       0x1E5; 0x70; 0x60
    */
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1E5, 0x80, 7, 0x1);
    wcd_register_write(E_WCD_CODEC_MAIN, 0x1E5, 0x70, 4, 0x6);
  }

  //#SPKR_DRV_EN__CLASSD_PA_EN = ENABLE
  //0x1DF; 0x80; 0x80
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1DF, 0x80, 7, 0x1);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}

wcd_result WCDH_SPEAKER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr)
{
  //#SPKR_DRV_EN__CLASSD_PA_EN = DISABLE
  //0x1DF; 0x80; 0x00
  wcd_register_write(E_WCD_CODEC_MAIN, 0x1DF, 0x80, 7, 0x0);

  HANDLE_UNUSED_PARAMETER(p_element);
  HANDLE_UNUSED_PARAMETER(parameter);
  HANDLE_UNUSED_PARAMETER(p_element_list);
  HANDLE_UNUSED_PARAMETER(context_ptr);

  return E_WCD_SUCCESS;
}



/**********************************************************************************
 *******************                    RESET                   *******************
 *********************************************************************************/


wcd_result WCDH_RESET(void)
{
  //RESET Everything for custom functions!
  uint32 iterator = 0;

  //BUS PORT
  WCD_MEMSET((void*)sbrx_bringup_count, 0x0, sizeof(sbrx_bringup_count));
  WCD_MEMSET((void*)sbtx_bringup_count, 0x0, sizeof(sbtx_bringup_count));

  for(iterator = 0; iterator < WCD_RX_BUS_PORT_COUNT; iterator++)
  {
    sbrx_bitwidth[iterator] = 24;
  }

  for(iterator = 0; iterator < WCD_TX_BUS_PORT_COUNT; iterator++)
  {
    sbtx_bitwidth[iterator] = 24;
  }

  //MIC BIAS
  WCD_MEMSET((void*)micb_bringup_count, 0x0, sizeof(micb_bringup_count));
  WCD_MEMSET((void*)micb_cfilt_selection, 0x0, sizeof(micb_cfilt_selection));
  WCD_MEMSET((void*)micb_bypass_capacitor, 0x0, sizeof(micb_bypass_capacitor));
  WCD_MEMSET((void*)cfilt_bringup_count, 0x0, sizeof(cfilt_bringup_count));
  WCD_MEMSET((void*)cfilt_k_value, 0x0, sizeof(cfilt_k_value));

  current_micb = 0;
  current_micb_cfilt_selection = 0;
  current_micb_bypass_capacitor = 0;
  current_micb_kvalue = 0;

  //RX PATH
  WCD_MEMSET((void*)rx_path_reference_count, 0x0, sizeof(rx_path_reference_count));

  //COMPANDER
  compander_0_bringup_count = 0;
  compander_1_bringup_count = 0;
  compander_2_bringup_count = 0;

  hphl_in_compander_mode = WCD_FALSE;
  hphr_in_compander_mode = WCD_FALSE;
  lo1_in_compander_mode = WCD_FALSE;
  lo2_in_compander_mode = WCD_FALSE;
  spkr_in_compander_mode = WCD_FALSE;

  //CLASS CONTROLLER
  class_h_earout_count = 0;
  class_h_hph_count = 0;

  class_h_speaker_connect = WCD_FALSE;

  //RX BIAS
  rx_bias_enable_count= 0;

  //DAC
  hphl_gain = 0x2; //-3 dB
  hphr_gain = 0x2; //-3 dB
  lo1_gain = 0x0;  //0 dB
  lo2_gain = 0x0;  //0 dB

  hph_to_process = 0x0;
  lo_to_process = 0x0;

  rdacs_enabled = 0x0;

  earout_enable_count = 0;
  hphl_enable_count = 0;
  hphr_enable_count = 0;
  lo1_enable_count = 0;
  lo2_enable_count = 0;

  return E_WCD_SUCCESS;
}

wcd_result WCDH_SET_MICBIAS_COUNT(wcd_codec codec)
{
  if(codec == E_WCD_WCD9302)
  {
    micbias_count = (uint8)WCD9302_MICB_COUNT;
    micbias_cfilt_count = (uint8)WCD9302_CFILT_COUNT;
  }

  return E_WCD_SUCCESS;
}


wcd_result WCDH_INT_RBIAS_ENABLE(wcd_ctrl_bus_interface bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 val, uint8 in_path, wcd_bool enable)
{
  wcd_result rc = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("=WCD_93XX: <CF WCDH_INT_RBIAS_ENABLE skipped> \n");
  HANDLE_UNUSED_PARAMETER(bus_intf);
  HANDLE_UNUSED_PARAMETER(reg);
  HANDLE_UNUSED_PARAMETER(mask);
  HANDLE_UNUSED_PARAMETER(shift);
  HANDLE_UNUSED_PARAMETER(val);
  HANDLE_UNUSED_PARAMETER(in_path);
  HANDLE_UNUSED_PARAMETER(enable);

  return rc;
}

